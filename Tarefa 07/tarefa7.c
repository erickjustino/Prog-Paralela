#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Estrutura para um no da lista encadeada
typedef struct Node {
    char *fileName;
    struct Node *next;
} Node;

// Função para criar um novo no
Node* createNode(const char* fileName) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Falha ao alocar memória para o nó");
        exit(EXIT_FAILURE);
    }
    newNode->fileName = (char*)malloc(strlen(fileName) + 1);
    if (newNode->fileName == NULL) {
        perror("Falha ao alocar memória para o nome do arquivo");
        exit(EXIT_FAILURE);
    }
    strcpy(newNode->fileName, fileName);
    newNode->next = NULL;
    return newNode;
}

// Funcao para inserir um novo no no inicio da lista
void insertNode(Node** head, const char* fileName) {
    Node* newNode = createNode(fileName);
    newNode->next = *head;
    *head = newNode;
}

// Função para liberar a memoria da lista encadeada
void freeList(Node* head) {
    Node* tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp->fileName);
        free(tmp);
    }
}

int main() {
    Node* head = NULL;

    // Lista de albuns dos Beatles para simular nomes de arquivos
    const char* beatlesAlbums[] = {
        "Please Please Me",
        "With the Beatles",
        "A Hard Day's Night",
        "Beatles for Sale",
        "Help!",
        "Rubber Soul",
        "Revolver",
        "Sgt. Pepper's Lonely Hearts Club Band",
        "Magical Mystery Tour",
        "The Beatles (The White Album)",
        "Yellow Submarine",
        "Abbey Road",
        "Let It Be"
    };
    int numAlbums = sizeof(beatlesAlbums) / sizeof(beatlesAlbums[0]);

    // Preenchendo a lista encadeada
    for (int i = numAlbums - 1; i >= 0; i--) {
        insertNode(&head, beatlesAlbums[i]);
    }

    #pragma omp parallel
{
    #pragma omp single
    {
        Node* current = head;
        while (current != NULL) {
            Node* nodeCopy = current;
            #pragma omp task firstprivate(nodeCopy)
            {
                printf("Arquivo: '%s' processado pela Thread: %d\n",
                       nodeCopy->fileName, omp_get_thread_num());
            }
            current = current->next;
        }

        // Espera todas as tarefas terminarem
        #pragma omp taskwait
    }
}


    // Liberando a memoria alocada para a lista
    freeList(head);

    return 0;
}