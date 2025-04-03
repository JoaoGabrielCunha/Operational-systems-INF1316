#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include<sys/shm.h>

#define CHAVE 8752
#define TAMANHO 1024

int main()  
{
    int shmid = shmget(CHAVE,TAMANHO, 0600);
    if (shmid == -1)
    {
        perror("Erro ao acessar a memória");
        exit(1);
    }

    char *mensagem = (char *) shmat(shmid, NULL, 0);
    if (mensagem == (char *) -1)
    {
        perror("Erro ao anexar memória compartilhada");
        exit(1);
    }

    printf("Mensagem do dia: %s", mensagem);
    
    shmdt(mensagem);
    shmctl(shmid, IPC_RMID,NULL);





    return 0;
}