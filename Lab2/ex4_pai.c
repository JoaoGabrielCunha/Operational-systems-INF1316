// ex4_pai.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/wait.h>

struct Mem {
    int valor;
    int seq;
};

int main() {
    key_t key1 = 1234;
    key_t key2 = 5678;

    int shmid1 = shmget(key1, sizeof(struct Mem), IPC_CREAT | 0600);
    int shmid2 = shmget(key2, sizeof(struct Mem), IPC_CREAT | 0600);

    if (shmid1 < 0 || shmid2 < 0) {
        perror("Erro ao criar memórias compartilhadas");
        exit(1);
    }

    struct Mem *m1 = (struct Mem *)shmat(shmid1, NULL, 0);
    struct Mem *m2 = (struct Mem *)shmat(shmid2, NULL, 0);

    m1->seq = 0;
    m2->seq = 0;

    printf("Memórias criadas. Iniciando filhos...\n");

    if (fork() == 0) {
        if (execl("./ex4_filho1", "ex4_filho1", NULL) == -1) {
            perror("exec filho1");
            exit(1);
        }
    }

    if (fork() == 0) {
        if (execl("./ex4_filho2", "ex4_filho2", NULL) == -1) {
            perror("exec filho2");
            exit(1);
        }
    }

    int last_seq1 = 0, last_seq2 = 0;

    while (1) {
        if (m1->seq > last_seq1 && m2->seq > last_seq2) {
            printf("Novo valor de P1: %d (seq %d)\n", m1->valor, m1->seq);
            printf("Novo valor de P2: %d (seq %d)\n", m2->valor, m2->seq);
            printf("Produto: %d\n\n", m1->valor * m2->valor);
            last_seq1 = m1->seq;
            last_seq2 = m2->seq;
        } else {
            printf("Esperando atualizações... (seq1=%d, seq2=%d)\n", m1->seq, m2->seq);
        }
        sleep(1);
    }

    return 0;
}
