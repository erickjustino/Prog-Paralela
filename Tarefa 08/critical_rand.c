// pi_v1_critical_rand.c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define NUM_POINTS 1000000000

int main() {
    long long total_hits = 0;
    double start_time, end_time;

    // A semente para rand() é global
    srand(time(NULL));

    start_time = omp_get_wtime();

    #pragma omp parallel
    {
        long long local_hits = 0;
        int num_threads = omp_get_num_threads();
        int thread_id = omp_get_thread_num();
        long long points_per_thread = NUM_POINTS / num_threads;

        // Cada thread calcula sua porção de pontos
        for (long long i = 0; i < points_per_thread; i++) {
            double x = (double)rand() / RAND_MAX;
            double y = (double)rand() / RAND_MAX;

            if (x * x + y * y <= 1.0) {
                local_hits++;
            }
        }

        // Seção crítica para acumular o total
        #pragma omp critical
        {
            total_hits += local_hits;
        }
    }

    end_time = omp_get_wtime();

    double pi_estimate = 4.0 * total_hits / NUM_POINTS;
    printf("Versão 1.1 (critical, rand)\n");
    printf("Estimativa de PI: %f\n", pi_estimate);
    printf("Tempo de execução: %f segundos\n", end_time - start_time);

    return 0;
}