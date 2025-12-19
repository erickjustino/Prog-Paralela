// pi_v4_array_rand_r_dinamico.c
#include <stdio.h>
#include <stdlib.h> // Necessário para malloc e free
#include <omp.h>

#define NUM_POINTS 1000000000

int main() {
    long long total_hits = 0;
    double start_time, end_time;
    
    // 1. Declarar um ponteiro em vez de um vetor de tamanho fixo
    long long *hits_per_thread;
    
    // Obtém o número de threads que a região paralela irá usar
    int num_threads_used = omp_get_max_threads();

    // 2. Alocar memória dinamicamente para o vetor
    // O tamanho será exatamente o número de threads usadas
    hits_per_thread = (long long *)malloc(num_threads_used * sizeof(long long));

    // É uma boa prática verificar se a alocação de memória funcionou
    if (hits_per_thread == NULL) {
        fprintf(stderr, "Falha na alocação de memória!\n");
        return 1; // Termina o programa com um código de erro
    }

    // Inicializa o vetor alocado com zeros
    for (int i = 0; i < num_threads_used; i++) {
        hits_per_thread[i] = 0;
    }

    start_time = omp_get_wtime();

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        unsigned int seed = thread_id;
        int num_threads = omp_get_num_threads(); // Número real de threads na equipe
        long long points_per_thread = NUM_POINTS / num_threads;

        // O contador de acertos é local para a thread dentro do vetor
        for (long long i = 0; i < points_per_thread; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;

            if (x * x + y * y <= 1.0) {
                hits_per_thread[thread_id]++;
            }
        }
    }

    // O laço de acumulação agora usa a mesma variável para o tamanho
    for (int i = 0; i < num_threads_used; i++) {
        total_hits += hits_per_thread[i];
    }

    end_time = omp_get_wtime();

    // 3. Liberar a memória que foi alocada com malloc
    free(hits_per_thread);

    double pi_estimate = 4.0 * total_hits / NUM_POINTS;
    printf("Versão 2.2 (array dinâmico, rand_r)\n");
    printf("Estimativa de PI: %f\n", pi_estimate);
    printf("Tempo de execução: %f segundos\n", end_time - start_time);

    return 0;
}