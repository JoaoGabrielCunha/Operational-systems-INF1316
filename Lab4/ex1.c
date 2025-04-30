#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>

int main()
{
    int fd[2];
    int pid;
    char mensagem[] = "Oieee";
    char buffer[100];

    if (pipe(fd)<0)
    {
        perror("Erro ao criar pipe!");
        exit(1);
    }
    pid = fork();
    
    if (pid<0)
    {
        perror("Erro ao criar processo filho");
        exit(1);
    }

    if (pid == 0) {
        // Processo FILHO
        close(fd[0]); // fecha leitura, vai escrever
        write(fd[1], mensagem, strlen(mensagem) + 1); // +1 para incluir o '\0'
        close(fd[1]); // fecha escrita
        exit(0);
    } else {
        // Processo PAI
        close(fd[1]); // fecha escrita, vai ler
        read(fd[0], buffer, sizeof(buffer));
        printf("Pai leu: %s\n", buffer);
        close(fd[0]); // fecha leitura
    }




    return 0;
}