#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_POINTS 1000000000

int main() {
    long long total_hits = 0;
    double start_time, end_time;

    start_time = omp_get_wtime();

    #pragma omp parallel
    {
        long long local_hits = 0;
        // Cada thread precisa de sua própria semente
        unsigned int seed = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        long long points_per_thread = NUM_POINTS / num_threads;

        for (long long i = 0; i < points_per_thread; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;

            if (x * x + y * y <= 1.0) {
                local_hits++;
            }
        }

        #pragma omp critical
        {
            total_hits += local_hits;
        }
    }

    end_time = omp_get_wtime();

    double pi_estimate = 4.0 * total_hits / NUM_POINTS;
    printf("Versão 1.2 (critical, rand_r)\n");
    printf("Estimativa de PI: %f\n", pi_estimate);
    printf("Tempo de execução: %f segundos\n", end_time - start_time);

    return 0;
}