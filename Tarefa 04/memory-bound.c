#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

double **alocar_matriz(int linhas, int colunas) {
    double **matriz = (double **)malloc(linhas * sizeof(double *));
    if (matriz == NULL) return NULL;
    for (int i = 0; i < linhas; i++) {
        matriz[i] = (double *)malloc(colunas * sizeof(double));
        if (matriz[i] == NULL) {
            for (int j = 0; j < i; j++) free(matriz[j]);
            free(matriz);
            return NULL;
        }
    }
    return matriz;
}

double *alocar_vetor(int tamanho) {
    return (double *)malloc(tamanho * sizeof(double));
}

void preencher_matriz(double **matriz, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            matriz[i][j] = (double)rand() / RAND_MAX;
        }
    }
}

void preencher_vetor(double *vetor, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        vetor[i] = (double)rand() / RAND_MAX;
    }
}

void liberar_matriz(double **matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

void MxV_paralelo(double **matriz, const double *vetor, double *resultado, int linhas, int colunas) {
    #pragma omp parallel for
    for (int j = 0; j < colunas; j++) {
        for (int i = 0; i < linhas; i++) {
            resultado[i] += matriz[i][j] * vetor[j];
        }
    }
}

int main(int argc, char *argv[]) {
    int linhas = 30000;
    int colunas = 30000;

    double **matriz = alocar_matriz(linhas, colunas);
    double *vetor = alocar_vetor(colunas);
    double *resultado = alocar_vetor(linhas);

    if (matriz == NULL || vetor == NULL || resultado == NULL) {
        fprintf(stderr, "Falha na alocação de memória.\n");
        return 1;
    }

    // Zera o vetor resultado antes de usar
    for(int i=0; i<linhas; i++) resultado[i] = 0.0;

    preencher_matriz(matriz, linhas, colunas);
    preencher_vetor(vetor, colunas);

    // Testa o desempenho para diferentes números de threads
    for (int num_threads = 1; num_threads <= 12; num_threads++) { 
        omp_set_num_threads(num_threads);  

        // Mede o tempo de execução
        double start_time, end_time, execution_time;
        start_time = omp_get_wtime();
        MxV_paralelo(matriz, vetor, resultado, linhas, colunas);
        end_time = omp_get_wtime();
        execution_time = end_time - start_time;

        printf("Número de threads: %d\n", num_threads);
        printf("Tempo de execução: %f segundos\n", execution_time);
    }

    liberar_matriz(matriz, linhas);
    free(vetor);
    free(resultado);

    return 0;
}