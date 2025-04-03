#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Erro ao criar processo filho");
        exit(1);
    }

    if (pid == 0) {  // Processo filho
        execlp("echo", "echo", "Alo Mundo", NULL); // Executa o comando echo
        perror("Erro ao executar echo");
        exit(1);
    } else {  // Processo pai
        wait(NULL);
        printf("Pai: Filho terminou.\n");
    }

    return 0;
}
