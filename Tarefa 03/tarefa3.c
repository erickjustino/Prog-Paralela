#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define _USE_MATH_DEFINES

double calcular_pi_leibniz(long num_iteracoes) {
    double pi_aproximado = 0.0;
    int sinal =1;

    for (long long i=0; i < num_iteracoes; i++){
        pi_aproximado += sinal*(1.0/(2.0*i+1.0));
        sinal *=-1;
    }
  
    return 4.0 * pi_aproximado;
}

int main() {
    long long iteracoes[] = {10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
    int num_testes = sizeof(iteracoes) / sizeof(iteracoes[0]);

    printf("%-12s %-20s %-20s %-20s\n", "Iterações", "π Aproximado", "Erro Absoluto", "Tempo (s)");

    for (int i = 0; i < num_testes; ++i) {
        long n = iteracoes[i];
        clock_t inicio = clock();

        double pi_aproximado = calcular_pi_leibniz(n);

        clock_t fim = clock();
        double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC;
        double erro = fabs(M_PI - pi_aproximado);

        printf("%-12ld %-20.15f %-20.15f %-20.6f\n", n, pi_aproximado, erro, tempo_execucao);
    }

    return 0;
}