#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Função para estimar pi de forma correta usando uma seção crítica
double estimativa_pi_correta(long total_pontos) {
    long dentro_circulo = 0;
    
    // Inicia a região paralela. Todas as threads são criadas aqui.
    #pragma omp parallel
    {
        // Variáveis declaradas aqui dentro são, por padrão, privadas para cada thread.
        unsigned int seed = omp_get_thread_num(); // Cada thread tem sua própria semente.

        // O trabalho do loop for é dividido entre as threads existentes.
        #pragma omp for
        for (long i = 0; i < total_pontos; i++) {
            // rand_r é thread-safe, precisa de um ponteiro para a semente.
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;

            if (x * x + y * y <= 1.0) {
                // Apenas uma thread por vez pode entrar nesta seção.
                // Isso evita a condição de corrida.
                #pragma omp critical
                {
                    dentro_circulo++;
                }
            }
        }
    } // Fim da região paralela. As threads são sincronizadas e destruídas aqui.

    return 4.0 * dentro_circulo / total_pontos;
}

int main() {
    long total_pontos = 10000000;

    printf("Executando a versão corrigida com #pragma omp critical...\n");
    double inicio = omp_get_wtime();
    double pi = estimativa_pi_correta(total_pontos);
    double fim = omp_get_wtime();

    printf("Estimativa de pi = %f\n", pi);
    printf("Tempo de execução = %f segundos\n", fim - inicio);

    return 0;
}