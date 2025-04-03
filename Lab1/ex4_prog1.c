#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>

int main()
{
    pid_t pid = fork();
    if(pid<0){

        perror("Erro ao criar processo filho");
        exit(1);
    }
    if (pid ==0){
        execl("./alo_mundo","alo_mundo",NULL);
        perror("Erro ao executar mensagem");
        exit(1);
    }
    else {
        wait(NULL);
        printf("Pai: Filho terminou. \n");
    }

    return 0;

}