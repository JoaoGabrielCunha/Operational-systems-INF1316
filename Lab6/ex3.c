#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXFILA 8
#define TOTAL_ITENS 64

int buffer[MAXFILA];
int in = 0, out = 0;
int count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pode_produzir = PTHREAD_COND_INITIALIZER;
pthread_cond_t pode_consumir = PTHREAD_COND_INITIALIZER;

void *produtor(void *arg) {
    for (int i = 1; i <= TOTAL_ITENS; i++) {
        pthread_mutex_lock(&mutex);

        while (count == MAXFILA)
            pthread_cond_wait(&pode_produzir, &mutex);

        buffer[in] = i;
        in = (in + 1) % MAXFILA;
        count++;

        printf("Produzido: %d\n", i);

        pthread_cond_signal(&pode_consumir);
        pthread_mutex_unlock(&mutex);

        sleep(1); // produz a cada 1s
    }
    pthread_exit(NULL);
}

void *consumidor(void *arg) {
    for (int i = 0; i < TOTAL_ITENS; i++) {
        pthread_mutex_lock(&mutex);

        while (count == 0)
            pthread_cond_wait(&pode_consumir, &mutex);

        int item = buffer[out];
        out = (out + 1) % MAXFILA;
        count--;

        printf("Consumido: %d\n", item);

        pthread_cond_signal(&pode_produzir);
        pthread_mutex_unlock(&mutex);

        sleep(2); // consome a cada 2s
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t th_produtor, th_consumidor;

    pthread_create(&th_produtor, NULL, produtor, NULL);
    pthread_create(&th_consumidor, NULL, consumidor, NULL);

    pthread_join(th_produtor, NULL);
    pthread_join(th_consumidor, NULL);

    printf("Produção e consumo finalizados!\n");

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&pode_produzir);
    pthread_cond_destroy(&pode_consumir);

    return 0;
}
