//servidor
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define CHAVE 8752
#define TAMANHO 1024


int main()
{
    int shmid = shmget(CHAVE, TAMANHO, IPC_CREAT | 0600);
    if (shmid == -1)
    {
        perror("Erro ao criar memória compartilhada");
        exit(1);
    }

    char *mensagem = (char *)shmat(shmid, NULL, 0);
        if (mensagem == (char *)-1){
            perror("Erro ao anexar memória compartilhada");
            exit(1);
            
        }

    printf("Digite a mensagem do dia: ");
    fgets(mensagem, TAMANHO, stdin);
    printf("Mensagem salva na memória compartilhada");

    return 0;
}