#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double estimativa_pi_com_firstprivate(long total_pontos) {
    long dentro_circulo_total = 0;
    
    //    Este valor sera copiado para as variaveis privadas de cada thread.
    long dentro_circulo_privado = 0; 
    
    // A clausula 'firstprivate' encontra a variavel externa 'dentro_circulo_privado'
    //  e cria uma copia para cada thread, inicializando-a com o valor da original (0).
    #pragma omp parallel firstprivate(dentro_circulo_privado)
    {
    // Nao declaramos 'dentro_circulo_privado' novamente aqui dentro.
    // A clausula 'firstprivate' já cuidou de criar e inicializar a variavel para nos.
        
        unsigned int seed = omp_get_thread_num();

        #pragma omp for
        for (long i = 0; i < total_pontos; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;

            if (x * x + y * y <= 1.0) {
                // Cada thread incrementa a sua propria cópia de 'dentro_circulo_privado'.
                dentro_circulo_privado++;
            }
        }

        // Apos o loop, cada thread adiciona seu subtotal privado ao total compartilhado.
        #pragma omp critical
        {
            dentro_circulo_total += dentro_circulo_privado;
        }
    }

    return 4.0 * dentro_circulo_total / total_pontos;
}

int main() {
    long total_pontos = 10000000;

    printf("Executando a versão com firstprivate (para estudo)...\n");
    
    double inicio = omp_get_wtime();
    double pi = estimativa_pi_com_firstprivate(total_pontos);
    double fim = omp_get_wtime();

    printf("Estimativa de pi = %f\n", pi);
    printf("Tempo de execução = %f segundos\n", fim - inicio);

    return 0;
}