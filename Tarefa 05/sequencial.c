#include <stdio.h>
#include <stdbool.h>
#include <time.h>

bool ehPrimo(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return false;
    }
    return true;
}

int main() {
    int n=20000000;
    int contador = 0;

    for (int i = 2; i <= n; i++) {
        if (ehPrimo(i)) {
            contador++;
        }
    }

    printf("Quantidade de numeros primos entre 2 e %d: %d\n", n, contador);

    return 0;
}