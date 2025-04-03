#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include<sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>

#define LINHAS 3
#define COLUNAS 3
#define TAMANHO (LINHAS *COLUNAS* sizeof(int))

void imprimir_matriz (int *mat)
{
    for(int i =0; i< LINHAS; i++)
    {   for(int j=0; j< COLUNAS; j++)
        {
            printf(" %d ", mat[i*COLUNAS +j]);
        }
            printf("\n");
    }
}

int main()
{
    int shmA = shmget(IPC_PRIVATE,TAMANHO, IPC_CREAT | 0600);
    int shmB = shmget(IPC_PRIVATE, TAMANHO, IPC_CREAT | 0600);
    int shmS = shmget(IPC_PRIVATE, TAMANHO, IPC_CREAT| 0600);

    int *matA = (int *)shmat(shmA, NULL,0);
    int *matB = (int *)shmat(shmB, NULL, 0);
    int *matS = (int *)shmat(shmS,NULL,0);

    //Preenchendo as matrizes 
    matA[0] = 5; matA[1] = 7; matA[2] = 9;
    matA[3] = 6; matA[4] = 3; matA[5] = 6;
    matA[6] = 3; matA[7] = 1; matA[8] = 2;

    
    matB[0] = 5; matB[1] = 3; matB[2] = 0;
    matB[3] = 6; matB[4] = 2; matB[5] = 6;
    matB[6] = 5; matB[7] = 7; matB[8] = 0;

    //
    for(int i =0; i< LINHAS; i++)
    {  pid_t pid = fork();
        if(pid==0){
          for(int j=0; j <COLUNAS; j++){
            int idx = i*COLUNAS +j;
            matS[idx] = matA[idx] + matB[idx];
          }
            exit(0);
        }

    }
    // Processo pai espera os filhos
    for(int i=0; i < LINHAS; i++)
    {
        wait(NULL);
    }

    //Imprimindo as matrizes 
    printf("Matriz A: \n");
    imprimir_matriz(matA);

    printf("\nMatriz B: \n");
    imprimir_matriz(matB);

    printf("\nMatriz Soma(S): \n");
    imprimir_matriz(matS);


    shmdt(matA);
    shmdt(matB);
    shmdt(matS);

    shmctl(shmA, IPC_RMID,NULL);
    shmctl(shmB, IPC_RMID,NULL);
    shmctl(shmS, IPC_RMID, NULL);
    return 0;
}



















