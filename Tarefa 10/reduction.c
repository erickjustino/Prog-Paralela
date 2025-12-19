#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_POINTS 1000000000

int main() {
    long long circulo_total = 0;
    double start_time, end_time;

    start_time = omp_get_wtime();

    #pragma omp parallel reduction(+:circulo_total)
    {
        unsigned int seed = (unsigned int)omp_get_wtime() ^ omp_get_thread_num();

        #pragma omp for
        for (long long i = 0; i < NUM_POINTS; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;

            if (x * x + y * y <= 1.0) {
                circulo_total++;
            }
        }
    }

    end_time = omp_get_wtime();

    double pi_estimate = 4.0 * circulo_total / NUM_POINTS;

    printf("Versão Reduction\n");
    printf("Estimativa de PI: %f\n", pi_estimate);
    printf("Tempo de execução: %f segundos\n", end_time - start_time);

    return 0;
}