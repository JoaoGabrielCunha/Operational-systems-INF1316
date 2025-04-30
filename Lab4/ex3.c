#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main() {
    int fd[2];
    int pid1, pid2;

    if (pipe(fd) == -1) {
        perror("Erro ao criar o pipe");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 < 0) {
        perror("Erro ao criar primeiro filho");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // Primeiro processo: executa `ps`
        close(fd[0]);             // Fecha leitura
        dup2(fd[1], STDOUT_FILENO); // Redireciona stdout para o pipe
        close(fd[1]);

        execlp("ps", "ps", NULL);
        perror("Erro ao executar ps");
        exit(EXIT_FAILURE);
    }

    pid2 = fork();
    if (pid2 < 0) {
        perror("Erro ao criar segundo filho");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // Segundo processo: executa `wc`
        close(fd[1]);             // Fecha escrita
        dup2(fd[0], STDIN_FILENO); // Redireciona stdin para o pipe
        close(fd[0]);

        execlp("wc", "wc", NULL);
        perror("Erro ao executar wc");
        exit(EXIT_FAILURE);
    }

    // Processo pai fecha tudo
    close(fd[0]);
    close(fd[1]);

    // Espera os dois filhos
    wait(NULL);
    wait(NULL);

    return 0;
}
