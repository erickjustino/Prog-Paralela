#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

void insert(Node** head_ref, int new_data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Erro de alocação de memória\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = new_data;
    new_node->next = *head_ref;
    *head_ref = new_node;
}

int count_elements(Node* head) {
    int count = 0;
    Node* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void free_list(Node** head_ref) {
    Node* current = *head_ref;
    Node* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    *head_ref = NULL;
}

int main() {
    const int N_INSERTIONS = 1000000;
    
    Node* list_A = NULL;
    Node* list_B = NULL;

    // Array de sementes para garantir que cada thread gere números aleatórios independentes.
    unsigned int* seeds = malloc(omp_get_max_threads() * sizeof(unsigned int));
    for (int i = 0; i < omp_get_max_threads(); ++i) {
        seeds[i] = time(NULL) ^ (i + 1);
    }

    printf("Realizando %d inserções...\n", N_INSERTIONS);

   
    #pragma omp parallel
    {
       
        #pragma omp single
        {
            printf("Executando com %d threads...\n\n", omp_get_num_threads());

            for (int i = 0; i < N_INSERTIONS; i++) {
                
                // Cria uma tarefa que será executada por uma thread livre.
                // 'firstprivate(i)' é crucial para garantir que cada tarefa capture o valor correto de 'i'.
                #pragma omp task firstprivate(i)
                {
                    int thread_id = omp_get_thread_num();
                    
                    // Gera um número aleatório de forma segura (thread-safe), usando a semente exclusiva desta thread.
                    int value_to_insert = rand_r(&seeds[thread_id]) % 1000;
                    
                    if (rand_r(&seeds[thread_id]) % 2 == 0) {
                        // INÍCIO DA SEÇÃO CRÍTICA PARA A LISTA A.
                        // Apenas uma thread por vez pode executar este bloco para evitar condição de corrida.
                        #pragma omp critical (list_a)
                        {
                            insert(&list_A, value_to_insert);
                        }
                    } else {
                        // SEÇÃO CRÍTICA PARA A LISTA B.
                        // O nome diferente ('list_b') cria um lock INDEPENDENTE, permitindo paralelismo.
                        // (Uma thread pode estar aqui enquanto outra está na seção 'list_a').
                        #pragma omp critical (list_b)
                        {
                            insert(&list_B, value_to_insert);
                        }
                    }
                } 
            } 
        } 
    }


    printf("Inserções concluídas.\n");
    int count_A = count_elements(list_A);
    int count_B = count_elements(list_B);

    printf("Lista 1: %d elementos\n", count_A);
    printf("Lista 2: %d elementos\n", count_B);

    free_list(&list_A);
    free_list(&list_B);
    free(seeds);
    
    return 0;
}