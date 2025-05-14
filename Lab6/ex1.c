#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 5
#define PULO_MAXIMO 10
#define DESCANSO_MAXIMO 1
#define DISTANCIA_PARA_CORRER 100

int classificacao = 1;

void *Correr(void *sapo) {
    int pulos = 0;
    int distanciaJaCorrida = 0;

    while (distanciaJaCorrida < DISTANCIA_PARA_CORRER) {
        int pulo = rand() % PULO_MAXIMO + 1;
        distanciaJaCorrida += pulo;
        pulos++;
        printf("Sapo %d pulou. Total: %d metros\n", (int)(size_t)sapo, distanciaJaCorrida);
        sleep(rand() % (DESCANSO_MAXIMO + 1));
    }

    printf("🏁 Sapo %d chegou na posição %d com %d pulos!\n", (int)(size_t)sapo, classificacao, pulos);
    classificacao++;

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    srand(time(NULL));
    printf("Corrida iniciada!\n");

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_create(&threads[t], NULL, Correr, (void *)(size_t)t);
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    printf("Corrida encerrada.\n");
    return 0;
}
