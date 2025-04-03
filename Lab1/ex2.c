#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)

{
    int var = 1;
    printf("Valor de var antes do fork: %d\n", var);

    int pid = fork();

    if (pid<0){
        perror("Erro ao criar o processo filho");
        exit(1);

    }

    if (pid ==0){
        var = 5;
        printf("FIlho: var = %d\n", var);
        exit(0);

    } 
    else 
    {   
        waitpid(pid,NULL,0);
        printf("Pai, printado depois do wait: var = %d\n", var);

    }



    return 0;
}

//O processo pai printa 1, daí depois ele pausa e espera o processo filho terminar, ele atribui 5, printa 5 e encerra, daí o processo pai volta e como ele não fez 
//a atribuição de 5, ele printa 1 mesmo.