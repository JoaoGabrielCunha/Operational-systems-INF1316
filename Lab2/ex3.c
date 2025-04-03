#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define TAMANHO_VETOR 48
#define N_PROCESSOS 4
#define CHAVE_BUSCA 47

int main()
{
    int shm_id = shmget(IPC_PRIVATE, TAMANHO_VETOR*sizeof(int), IPC_CREAT | 0600);
    if (shm_id == -1)
    {
        perror("Erro ao alocar memória compartilhada  ");
        exit(1);
    }

    int *vetor = (int *)shmat(shm_id,NULL,0);
    if (vetor == (int *)-1) {
        perror("Erro ao anexar memória");
        exit(1);

    }

    srand(getpid());
    for(int i =0; i<TAMANHO_VETOR; i++)
    {
      vetor[i] = rand()%41;

      vetor[31] = CHAVE_BUSCA;

    }

    printf("Vetor:\n"); 
    for(int i =0; i<TAMANHO_VETOR; i++){
        printf(" %2d ", vetor[i]);
        if ((i+1)%12 == 0) printf("\n");
    }
    
    printf("\n");

    int intervalo = TAMANHO_VETOR/ N_PROCESSOS;

    for (int i = 0; i < N_PROCESSOS; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            int inicio = i * intervalo;
            int fim = (i == N_PROCESSOS - 1) ? TAMANHO_VETOR : inicio + intervalo;

            for (int j = inicio; j < fim; j++) {
                if (vetor[j] == CHAVE_BUSCA) {
                    printf("Processo %d encontrou o valor %d na posição %d\n", getpid(), CHAVE_BUSCA, j);
                    exit(0);
                }
            }

            // Se não achou
            printf("Processo %d não encontrou o valor\n", getpid());
            exit(0);
        }
    }

    // Pai espera os filhos
    for (int i = 0; i < N_PROCESSOS; i++) {
        wait(NULL);
    }

    // Libera memória
    shmdt(vetor);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}



