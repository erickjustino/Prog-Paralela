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
    int num_lists;

    // O número de listas agora é dinâmico, definido pelo usuário em tempo de execução.
    printf("Digite o número de listas a serem usadas: ");
    scanf("%d", &num_lists);

    if (num_lists <= 0) {
        printf("O número de listas deve ser positivo.\n");
        return 1;
    }

    // Aloca um array de ponteiros para as cabeças das M listas.
    Node** lists = (Node**)calloc(num_lists, sizeof(Node*));

    // Aloca um array de locks, um para cada lista, pois o número é dinâmico.
    omp_lock_t* list_locks = (omp_lock_t*)malloc(num_lists * sizeof(omp_lock_t));
    // Cada lock no array precisa ser inicializado individualmente antes do uso.
    for (int i = 0; i < num_lists; i++) {
        omp_init_lock(&list_locks[i]);
    }

    unsigned int* seeds = malloc(omp_get_max_threads() * sizeof(unsigned int));
    for (int i = 0; i < omp_get_max_threads(); ++i) {
        seeds[i] = time(NULL) ^ (i + 1);
    }

    printf("\nRealizando %d inserções em %d listas...\n", N_INSERTIONS, num_lists);
    
    #pragma omp parallel
    {
        #pragma omp single
        {
            printf("Executando com %d threads...\n\n", omp_get_num_threads());

            for (int i = 0; i < N_INSERTIONS; i++) {
                // Cria uma tarefa. 'firstprivate(i)' garante que cada tarefa use o valor correto de 'i'.
                #pragma omp task firstprivate(i)
                {
                    int thread_id = omp_get_thread_num();
                    int value_to_insert = rand_r(&seeds[thread_id]) % 1000;
                    
                    // Escolhe aleatoriamente qual das M listas receberá a inserção.
                    int list_index = rand_r(&seeds[thread_id]) % num_lists;

                    // Adquire o lock específico da lista escolhida. Outras threads querendo o MESMO lock irão esperar.
                    omp_set_lock(&list_locks[list_index]);
                    
                    insert(&lists[list_index], value_to_insert);
                    
                    // Libera o lock, permitindo que outra thread que esteja esperando possa adquiri-lo.
                    omp_unset_lock(&list_locks[list_index]);
                } 
            } 
        } 
    }
    
    printf("Inserções concluídas.\n");
    
    for (int i = 0; i < num_lists; i++) {
        printf("Lista %d: %d elementos\n", i + 1, count_elements(lists[i]));
    }

    // Libera a memória de cada lista e também destrói cada lock para liberar seus recursos.
    for (int i = 0; i < num_lists; i++) {
        free_list(&lists[i]);
        omp_destroy_lock(&list_locks[i]);
    }

    free(lists);
    free(list_locks);
    free(seeds);
    
    return 0;
}