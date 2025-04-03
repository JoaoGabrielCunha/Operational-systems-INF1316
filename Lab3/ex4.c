#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define TROCAS 10
#define EVER ;;

int main(void) {
   pid_t filho1, filho2;

   if ((filho1 = fork())==0)
   {//Filho1
    while (1)
    {
        printf("[Filho 1] executando");
        sleep(1);
    }
   }

   if ((filho2 = fork()) == 0) {
    // Filho 2
    while (1) {
        printf("[Filho 2] Executando...\n");
        sleep(1);
    }
}
    //Pai
    sleep(1); //Dá tempo dos filhos iniciarem.

    kill(filho1, SIGSTOP);
    kill(filho2,SIGSTOP);

    for (int i = 0; i < TROCAS; i++) {
        if (i % 2 == 0) {
            kill(filho1, SIGCONT);
            kill(filho2, SIGSTOP);
            printf("[Pai] Rodando filho 1 (troca %d)\n", i + 1);
        } else {
            kill(filho2, SIGCONT);
            kill(filho1, SIGSTOP);
            printf("[Pai] Rodando filho 2 (troca %d)\n", i + 1);
        }
        sleep(2);  // Tempo para o filho executar
    }

    printf("[Pai] Finalizando filhos...\n");
    kill(filho1, SIGKILL);
    kill(filho2, SIGKILL);

    waitpid(filho1, NULL, 0);
    waitpid(filho2, NULL, 0);

    printf("[Pai] Filhos terminados.\n");


    return 0;
}
