#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void childhandler(int signo);
int delay;

int main(int argc, char *argv[]) {
    pid_t pid;

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <tempo> <programa-filho> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGCHLD, childhandler);

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Erro ao criar filho\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Processo filho
        execvp(argv[2], &argv[2]);
        // Se execvp falhar
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Processo pai
        sscanf(argv[1], "%d", &delay);
        sleep(delay);
        printf("Programa %s excedeu o limite de %d segundos!\n", argv[2], delay);
        kill(pid, SIGKILL);  // Mata o filho
        sleep(1); // Garante que o sinal SIGCHLD chegue
    }

    return 0;
}

void childhandler(int signo) {
    int status;
    pid_t pid = wait(&status);
    printf("Filho %d terminou dentro do limite de %d segundos com estado %d.\n", pid, delay, status);
    exit(0);
}
