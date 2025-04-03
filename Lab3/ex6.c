#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

time_t inicio = 0;
bool emChamada = false;

void iniciarChamada(int sinal) {
    if (!emChamada) {
        inicio = time(NULL);
        emChamada = true;
        printf("Chamada iniciada.\n");
    } else {
        printf("Já existe uma chamada em andamento.\n");
    }
}

void finalizarChamada(int sinal) {
    if (emChamada) {
        time_t fim = time(NULL);
        int duracao = fim - inicio;
        double custo = 0.0;

        if (duracao <= 60)
            custo = duracao * 0.02;
        else
            custo = 60 * 0.02 + (duracao - 60) * 0.01;

        printf("Chamada finalizada. Duração: %d segundos. Custo: R$ %.2f\n", duracao, custo);
        emChamada = false;
    } else {
        printf("Nenhuma chamada em andamento para finalizar.\n");
    }
}

int main(void) {
    // Captura os sinais de início e fim da chamada
    signal(SIGUSR1, iniciarChamada);
    signal(SIGUSR2, finalizarChamada);

    printf("Monitor de chamadas iniciado (PID: %d).\n", getpid());
    printf("Envie SIGUSR1 para iniciar e SIGUSR2 para finalizar uma chamada.\n");

    while (1) {
        pause();  // Fica esperando sinais
    }

    return 0;
}
