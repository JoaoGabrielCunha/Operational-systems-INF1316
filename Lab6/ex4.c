#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXFILA 8
#define TOTAL_ITENS 64
#define NUM_PRODUTORES 2
#define NUM_CONSUMIDORES 2

int buffer[MAXFILA];
int in = 0, out = 0;
int count = 0;
int itens_produzidos = 0;
int itens_consumidos = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pode_produzir = PTHREAD_COND_INITIALIZER;
pthread_cond_t pode_consumir = PTHREAD_COND_INITIALIZER;

void *produtor(void *arg) {
    int id = (int)(size_t)arg;
    while (1) {
        pthread_mutex_lock(&mutex);

        if (itens_produzidos >= TOTAL_ITENS) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        while (count == MAXFILA)
            pthread_cond_wait(&pode_produzir, &mutex);

        if (itens_produzidos >= TOTAL_ITENS) {
            pthread_cond_broadcast(&pode_consumir);
            pthread_mutex_unlock(&mutex);
            break;
        }

        int item = ++itens_produzidos;
        buffer[in] = item;
        in = (in + 1) % MAXFILA;
        count++;

        printf("Produtor %d produziu: %d\n", id, item);

        pthread_cond_signal(&pode_consumir);
        pthread_mutex_unlock(&mutex);

        sleep(1); // produz a cada 1s
    }
    pthread_exit(NULL);
}

void *consumidor(void *arg) {
    int id = (int)(size_t)arg;
    while (1) {
        pthread_mutex_lock(&mutex);

        if (itens_consumidos >= TOTAL_ITENS) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        while (count == 0)
            pthread_cond_wait(&pode_consumir, &mutex);

        if (itens_consumidos >= TOTAL_ITENS) {
            pthread_cond_broadcast(&pode_produzir);
            pthread_mutex_unlock(&mutex);
            break;
        }

        int item = buffer[out];
        out = (out + 1) % MAXFILA;
        count--;
        itens_consumidos++;

        printf("Consumidor %d consumiu: %d\n", id, item);

        pthread_cond_signal(&pode_produzir);
        pthread_mutex_unlock(&mutex);

        sleep(2); // consome a cada 2s
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t produtores[NUM_PRODUTORES], consumidores[NUM_CONSUMIDORES];

    for (int i = 0; i < NUM_PRODUTORES; i++)
        pthread_create(&produtores[i], NULL, produtor, (void *)(size_t)i);

    for (int i = 0; i < NUM_CONSUMIDORES; i++)
        pthread_create(&consumidores[i], NULL, consumidor, (void *)(size_t)i);

    for (int i = 0; i < NUM_PRODUTORES; i++)
        pthread_join(produtores[i], NULL);

    for (int i = 0; i < NUM_CONSUMIDORES; i++)
        pthread_join(consumidores[i], NULL);

    printf("Todos os itens foram produzidos e consumidos!\n");

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&pode_produzir);
    pthread_cond_destroy(&pode_consumir);

    return 0;
}


