#include <signal.h>
#include<stdio.h>

void trataKill(int sinal)
{
    printf("Tentei interceptar SIGKILL (%d)\n", sinal);
    
}

int main()
{
    signal(SIGKILL, trataKill);
    while(1);
}