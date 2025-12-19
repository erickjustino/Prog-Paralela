#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h> 


double** alocar_matriz(int linhas, int colunas) {
    double** matriz = (double**)malloc(linhas * sizeof(double*));
    if (matriz == NULL) {
        return NULL;
    }
    for (int i = 0; i < linhas; i++) {
        matriz[i] = (double*)malloc(colunas * sizeof(double));
        if (matriz[i] == NULL) {
            return NULL;
        }
    }
    return matriz;
}

double* alocar_vetor(int tamanho) {
    double* vetor = (double*)malloc(tamanho * sizeof(double));
    if (vetor == NULL) {
        return NULL;
    }
    return vetor;
}

void preencher_matriz(double** matriz, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            matriz[i][j] = (double)rand() / RAND_MAX;
        }
    }
}

void preencher_vetor(double* vetor, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        vetor[i] = (double)rand() / RAND_MAX;
    }
}

void liberar_matriz(double** matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

void MxV_linha(double** matriz, const double* vetor, double* resultado, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        resultado[i] = 0.0;
    }
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            resultado[i] += matriz[i][j] * vetor[j];
        }
    }
}


// Função de comparação para o qsort, necessária para ordenar os tempos
int comparar_doubles(const void* a, const void* b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    if (da > db) return 1;
    if (da < db) return -1;
    return 0;
}

int main() {
    srand(time(NULL));

    const int N = 16384;
    // Número de amostras para tirar a mediana
    const int AMOSTRAS = 51;

    double tempos_individuais[AMOSTRAS];

    printf("### CÁLCULO DA MEDIANA DO TEMPO DA OPERAÇÃO(Acesso por linha externa) ###\n");
    printf("-------------------------------------------------------------------\n");
    printf("Tamanho da Matriz: %dx%d\n", N, N);

    // Alocação e preenchimento fora da medição de tempo
    double** matriz = alocar_matriz(N, N);
    double* vetor = alocar_vetor(N);
    double* resultado = alocar_vetor(N);
    preencher_matriz(matriz, N, N);
    preencher_vetor(vetor, N);

    // Loop para coletar os tempos de cada multiplicação
    for (int k = 0; k < AMOSTRAS; k++) {
        double inicio = omp_get_wtime();
        MxV_linha(matriz, vetor, resultado, N, N);
        double fim = omp_get_wtime();
        tempos_individuais[k] = fim - inicio;
    }

    // Liberação de memória
    liberar_matriz(matriz, N);
    free(vetor);
    free(resultado);

    // Ordena os tempos para achar a mediana
    qsort(tempos_individuais, AMOSTRAS, sizeof(double), comparar_doubles);

    double mediana = tempos_individuais[AMOSTRAS / 2];

    printf("-------------------------------------------------------------------\n");
    printf("-> MEDIANA DO TEMPO DA MULTIPLICAÇÃO: %.4f segundos\n", mediana);
    printf("-------------------------------------------------------------------\n");

    return 0;
}