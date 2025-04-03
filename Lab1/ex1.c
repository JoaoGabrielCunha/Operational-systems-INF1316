#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>


int main() {
    
    int mypid, pid, status;

    printf("PID do processo pai: %d\n", getpid());
    pid = fork();
    
    
    if (pid==0){

        
        printf("PID do processo filho: %d\n", getpid());
        exit(3);
    }

    else {

        waitpid(-1, &status,0);

    }

    return 0;
    }

//Funcionou, ambos os Pids foram printados.

