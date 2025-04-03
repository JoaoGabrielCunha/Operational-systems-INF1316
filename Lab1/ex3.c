#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

#define TAM 10

//Função para ordenar 

void ordenar(int *vet, int tam) 
{
    for (int i = 0; i < tam - 1; i++) 
    {
        for (int j = 0; j < tam - i - 1; j++) 
        {
            if (vet[j] > vet[j + 1]) 
            {
                int temp = vet[j];
                vet[j] = vet[j + 1];
                vet[j + 1] = temp;
            }
        }
    }
}


//Função para printar o vetor:

// Função para imprimir vetor
void print_vetor(int *vet, int tam, char *msg) 
{
    printf("%s: [", msg);
    for (int i = 0; i < tam; i++) {
        printf("%d ", vet[i]);
    }
    printf("]\n");
}

int main(void)
{

int vetor[TAM] = {10,9,8,7,6,5,4,3,2,1};

printf("Vetor antes do fork: \n");
print_vetor(vetor,TAM,"Pai:");

int pid = fork();

if (pid<0)
{
    printf("Erro ao criar processo filho");
    exit(1);

}

if (pid ==0){
    ordenar(vetor,TAM);
    print_vetor(vetor, TAM, "Filho ordenado");
}
else{
        waitpid(pid, NULL,0);
        printf("Pós waitpid(): \n");
        print_vetor(vetor,TAM,"Pai:");
}




 return 0;
}

// Não usarão o mesmo vetor, o do filho ordena seu vetor e o do pai continua não ordenado.
