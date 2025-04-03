// ex4_filho1.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

struct Mem {
    int valor;
    int seq;
};

int main() {
    key_t key = 1234;
    int shmid = shmget(key, sizeof(struct Mem), 0600);
    if (shmid < 0) {
        perror("shmget filho1");
        exit(1);
    }

    struct Mem *mem = (struct Mem *)shmat(shmid, NULL, 0);

    srand(getpid());

    while (1) {
        sleep(rand() % 5 + 1);
        mem->valor = rand() % 100;
        mem->seq++;
        printf("[filho1] Novo valor: %d (seq: %d)\n", mem->valor, mem->seq);
    }

    return 0;
}
