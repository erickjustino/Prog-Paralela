#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

void calcular_intensivo(double *vetor, int tamanho) {
    #pragma omp parallel for
    for (int i = 0; i < tamanho; i++) {
        vetor[i] = exp(sin(log(fabs(sin(vetor[i]) * cos(vetor[i]) + tan(vetor[i])) + 1.0) + sqrt(fabs(sin(vetor[i]) * cos(vetor[i]) + tan(vetor[i])))));
    }
}

int main(int argc, char *argv[]) {
    int tamanho = 100000000; // Tamanho do vetor

    // Aloca o vetor de dados
    double *vetor = (double *)malloc(tamanho * sizeof(double));
    if (vetor == NULL) {
        fprintf(stderr, "Falha na alocação de memória.\n");
        return 1;
    }

    // Preenche o vetor com valores aleatórios
    for (int i = 0; i < tamanho; i++) {
        vetor[i] = (double)rand() / RAND_MAX * 100.0;
    }

    // Varia o número de threads
    for (int num_threads = 1; num_threads <= 12; num_threads++) { 
        omp_set_num_threads(num_threads);  

        // Mede o tempo de execução
        double start_time, end_time, execution_time;
        start_time = omp_get_wtime();
        calcular_intensivo(vetor, tamanho);
        end_time = omp_get_wtime();
        execution_time = end_time - start_time;

        printf("Número de threads: %d\n", num_threads);
        printf("Tempo de execução: %f segundos\n", execution_time);
    }

    // Libera a memória alocada
    free(vetor);

    return 0;
}
