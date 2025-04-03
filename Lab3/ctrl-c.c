#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define EVER ;;

void intHandler(int sinal);
void quitHandler(int sinal);

int main (void) {
    void (*p)(int);
    
    p = signal(SIGINT, intHandler);
    printf("Endereco do manipulador anterior %p\n", p);
    
    p = signal(SIGQUIT, quitHandler);
    printf("Endereco do manipulador anterior %p\n", p);

    puts("Ctrl-C desabilitado. Use Ctrl-\\ para terminar");

    for(EVER); // loop infinito
}

void intHandler(int sinal) {
    printf("Você pressionou Ctrl-C (%d)\n", sinal);
}

void quitHandler(int sinal) {
    printf("Terminando o processo...\n");
    exit(0);
}
