#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void trataFPE(int sinal) {
    printf("Erro: divisão por zero detectada (sinal %d - SIGFPE).\n", sinal);
    exit(1);  
}

int main(void) {
    int a, b;

    signal(SIGFPE, trataFPE);

    printf("Digite dois números inteiros (a b): ");
    scanf("%d %d", &a, &b);

    printf("Soma: %d + %d = %d\n", a, b, a + b);
    printf("Subtração: %d - %d = %d\n", a, b, a - b);
    printf("Multiplicação: %d * %d = %d\n", a, b, a * b);

    // Divisão e módulo: podem causar SIGFPE se b == 0
    printf("Divisão: %d / %d = %d\n", a, b, a / b);
    printf("Resto: %d %% %d = %d\n", a, b, a % b);

    return 0;
}