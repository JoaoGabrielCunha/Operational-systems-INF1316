#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "aviao.h"

void imprimir_status_avioes(Aviao* frota, int N) {
    for (int i = 0; i < N; i++) {
        printf("Avião %d:\n", i);
        printf("  PID: %d\n", frota[i].pid);
        printf("  Status: %d\n", frota[i].status);
        printf("  Posição: (%f, %f)\n", frota[i].x, frota[i].y);
        printf("  Direção: %d\n", frota[i].direcao);
        printf("  Pista de destino: %d\n", frota[i].pista_destino);
        printf("//-------------------------------------------------------------------------//\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <pid_do_controlador> <id_memoria_compartilhada> <numero_de_avioes>\n", argv[0]);
        return 1;
    }

    pid_t pid_pai = atoi(argv[1]);
    int shm_id = atoi(argv[2]);
    int N = atoi(argv[3]);

    // Anexar segmento de memória compartilhada
    Aviao* frota = (Aviao*) shmat(shm_id, NULL, 0);
    if (frota == (void*) -1) {
        perror("Erro ao anexar memória compartilhada");
        return 1;
    }

    char comando[100];

    while (1) {
        printf("Digite um dos comandos: pausar / retomar / sair): ");
        fflush(stdout); //flush para evitar problemas com atrasos deprint

        if (fgets(comando, sizeof(comando), stdin) == NULL)
            continue;

        // Remover \n do final
        comando[strcspn(comando, "\n")] = 0;

        if (strcmp(comando, "pausar") == 0) {
            kill(pid_pai, SIGUSR1);
            imprimir_status_avioes(frota, N);
        } 
        else if (strcmp(comando, "retomar") == 0) {
            kill(pid_pai, SIGCONT);
        }
        else if (strcmp(comando, "sair") == 0) {
            imprimir_status_avioes(frota, N);
            sleep(4); //para dar tempo de ver os prints
            kill(pid_pai, SIGUSR2);
            break;
        }
        else {
            printf("Comando inválido.\n");
        }
    }

    // Desanexar segmento de memória compartilhada
    if (shmdt(frota) == -1) {
        perror("Erro ao desanexar memória compartilhada");
    }

    return 0;
}
