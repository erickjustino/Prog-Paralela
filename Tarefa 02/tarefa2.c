#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define TAMANHO 100000000

// Aloca memória para o vetor
double* alocar_vetor(int tamanho) {
    double* vetor = (double*)malloc(tamanho * sizeof(double));
    if (vetor == NULL) {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }
    return vetor;
}

// LAÇO 1: Preenche o vetor com a sequência 1.0, 2.0, 3.0, ...
void preencher_vetor(double* vetor, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        vetor[i] = i + 1; 
    }
}


int main() {
    printf("Tamanho do vetor: %d elementos\n", TAMANHO);
    double* vetor = alocar_vetor(TAMANHO);
    preencher_vetor(vetor, TAMANHO);

    clock_t inicio, fim;
    double tempo_execucao;

    // -------------------------------------------------------------------------
    // LAÇO 2: Soma com dependência de dados
    // A iteração i+1 depende diretamente do resultado da iteração i.
    // Isso cria uma "fila" para as operações de soma, limitando o ILP.
    // -------------------------------------------------------------------------
    printf("\n--- Laço 2: Soma com Dependência ---\n");
    double soma1 = 0.0;
    
    inicio = clock();
    for (int i = 0; i < TAMANHO; i++) {
        soma1 += vetor[i];
    }
    fim = clock();
    
    tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Tempo de execução: %f segundos\n", tempo_execucao);
    printf("Resultado da soma: %f\n", soma1);

    // -------------------------------------------------------------------------
    // LAÇO 3: Soma com dependências quebradas
    // Usamos múltiplos acumuladores. As 4 somas dentro do laço são independentes
    // e podem ser executadas em paralelo por um processador moderno.
    // -------------------------------------------------------------------------
    printf("\n--- Laço 3: Soma com Dependências Quebradas ---\n");
    double soma_final = 0.0;
    double soma2 = 0.0, soma3 = 0.0, soma4 = 0.0, soma5 = 0.0;

    inicio = clock();
    for (int i = 0; i < TAMANHO; i += 4) {
        soma2 += vetor[i];
        soma3 += vetor[i + 1];
        soma4 += vetor[i + 2];
        soma5 += vetor[i + 3];
    }
    // Soma os resultados parciais fora do laço principal
    soma_final = soma2 + soma3 + soma4 + soma5;
    fim = clock();

    tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Tempo de execução: %f segundos\n", tempo_execucao);
    printf("Resultado da soma: %f\n", soma_final);
    free(vetor);
    return 0;
}