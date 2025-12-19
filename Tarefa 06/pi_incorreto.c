#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double estimativa_pi_incorreta(long total_pontos) {
    long dentro_circulo = 0;

    #pragma omp parallel for
    for (long i = 0; i < total_pontos; i++) {
        unsigned int seed = omp_get_thread_num() + i;
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        if (x * x + y * y <= 1.0) {
            dentro_circulo++;
        }
    }

    return 4.0 * dentro_circulo / total_pontos;
}

int main() {
    long total_pontos = 10000000;

    double inicio = omp_get_wtime();

    double pi = estimativa_pi_incorreta(total_pontos);

    double fim = omp_get_wtime();

    printf("Estimativa de pi = %f\n", pi);
    printf("Tempo de execução = %f segundos\n", fim - inicio);

    return 0;
}