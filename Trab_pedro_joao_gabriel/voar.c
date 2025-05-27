#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include "aviao.h"

Aviao* mem_aviao;  // variável global
Aviao* frota;

void trata_sigcont(int sinal){
    printf("[AVIÃO %d] Torre liberou! Continuando o voo. Sinal recebido: %d\n", getpid(), sinal);
    return;
}

void trata_toggle_freio(int sinal){
    printf("[AVIÃO %d] Sinal para alteração de velocidade recebido. Sinal recebido: %d\n", getpid(), sinal);
    if (mem_aviao->status == STATUS_VOANDO){
        printf("[AVIÃO %d] Reduzindo velocidade. Status: PAUSADO\n", getpid());
        mem_aviao->status = STATUS_PAUSADO;
        pause(); 
        return;
    }
    else if (mem_aviao->status == STATUS_PAUSADO){
        printf("[AVIÃO %d] Retomando velocidade normal. Status: VOANDO\n", getpid());
        mem_aviao->status = STATUS_VOANDO;
        pause();
        return;
    }
}

void trata_toggle_pista(int sinal){
    printf("[AVIÃO %d] Sinal para mudança de pista recebido. Sinal recebido: %d\n", getpid(), sinal);
    int pista_antiga = mem_aviao->pista_destino;
    
    if (mem_aviao->direcao == 'W'){
        if (mem_aviao->pista_destino == 18) mem_aviao->pista_destino = 03;
        else mem_aviao->pista_destino = 18;
        printf("[AVIÃO %d] Mudando da pista %d para a pista %d\n", 
            getpid(), pista_antiga, mem_aviao->pista_destino);
        pause();
        return;
        }
        else { // Direcao E
            if (mem_aviao->pista_destino == 06) mem_aviao->pista_destino = 27;
            else mem_aviao->pista_destino = 06;
            printf("[AVIÃO %d] Mudando da pista %d para a pista %d\n", 
                getpid(), pista_antiga, mem_aviao->pista_destino);
            pause();
            
            return;
    }
}

int main(int argc, char* argv[]){
    if (argc != 3) {
        fprintf(stderr, "Uso correto: voar <indice> <shm_id>\n");
        exit(1);
    }
    int indice = atoi(argv[1]);
    int segmento = atoi(argv[2]);
    frota = (Aviao*)shmat(segmento, NULL, 0);
    teste_attach(frota);
    mem_aviao = &frota[indice];
    
    // Handlers de Signals
    signal(SIGCONT, trata_sigcont);
    signal(SIGUSR2, trata_toggle_pista);
    signal(SIGUSR1, trata_toggle_freio);
    float random = (time(NULL) + getpid());
    srand(time(NULL) + getpid());
    // printf("Aleatorio: %f\n",random);
    float atraso, x, y;
    char direcao;
    int pid_aviao;
    int pista_destino;
    pid_aviao = getpid(); //armazena o pid do voo;
    atraso = ((float)rand() / RAND_MAX) * 2.0f; //numero aleatorio de 0 a 2
    y = ((float)rand() / RAND_MAX);
    direcao = (rand() % 2 == 0) ? 'W' : 'E'; // Escolher aleatoriamente entre W e E, utilizando resto de 2.
    // printf("%c\n\n",direcao);
    if (direcao == 'E') {
        x = 1.0f;
        pista_destino = rand() % 2;//Pistas 0 e 1 para E
        pista_destino = (pista_destino == 0) ? 06 : 27;

    }
    else if (direcao == 'W') {
        x = 0.0f;
        pista_destino = 2 + (rand() % 2);// Pistas 2 e 3 para W
        pista_destino = (pista_destino == 2) ? 03 : 18;
        
    }
    // printf("Indice no voar: %d\n",indice);
    // printf("Segmento no voar: %d\n",segmento);

    frota[indice].pid = pid_aviao;
    frota[indice].x = x;
    frota[indice].y = y;
    frota[indice].pista_destino = pista_destino;
    frota[indice].atraso = atraso;
    frota[indice].direcao = direcao;
    frota[indice].status = STATUS_FORA_ESPACO_AEREO ;
    frota[indice].velocidade = 0.0f; // inicialmente o aviao está parado em y
    printf("[AVIÃO %d] Criado: Índice %d - atraso: %.2f, x = %.2f, y = %.2f, direção = %c, pista = %d\n", 
    pid_aviao, indice, frota[indice].atraso, frota[indice].x, frota[indice].y, frota[indice].direcao, frota[indice].pista_destino);
    sleep(2);
    pause();//Aviao criado, agora esperamos o Processo Pai "continuar" o filho por Round Robin   
    printf("depois do pause\n");
    /*Implementar Tempo de Espera aqui!!*/
    printf("Aviao deve esperar inicialmente: %.2f segundos!\n",atraso);
    
    sleep(atraso);
    frota[indice].status = STATUS_VOANDO ;
    while(1){
        printf("[AVIÃO %d] Posição atual: (%.2f, %.2f) | Pista: %d | Status: %d\n",
               mem_aviao->pid, mem_aviao->x, mem_aviao->y, mem_aviao->pista_destino,
               mem_aviao->status);
        
               //movimento horizontal(em x)
               if (mem_aviao->status == STATUS_VOANDO){
                   //veridicando se pode pousar
                   if (fabs(mem_aviao->x - 0.5f) < 0.05) mem_aviao->x = 0.5f; //Definindo um valor minimo de proximidade
                   if (fabs(mem_aviao->y - 0.5f) < 0.05) mem_aviao->y = 0.5f; //Fazendo-se o mesmo para y
                   if (mem_aviao->x == 0.5f && mem_aviao->y == 0.5f){
                       mem_aviao->status = STATUS_ATERRISSADO;
                       printf("\n[AVIÃO %d] POUSO REALIZADO COM SUCESSO!\n", mem_aviao->pid);
                       shmdt(frota);
                       exit(0); 
                    }
                    
                    if ((mem_aviao->direcao == 'W') && ((mem_aviao->x) < 0.5)) {
                        mem_aviao->x += 0.05f; // vem da esquerda para o centro
                        printf("[AVIÃO %d] Movendo para leste: nova posição X = %.2f\n", mem_aviao->pid, mem_aviao->x);
                    } 
                    else if ((mem_aviao->direcao == 'E') && ((mem_aviao->x) > 0.5)) {
                        mem_aviao->x -= 0.05f; // vem da direita para o centro
                        printf("[AVIÃO %d] Movendo para oeste: nova posição X = %.2f\n", mem_aviao->pid, mem_aviao->x);
                    }
                    
                    //movimento vertical(em y)
                    if (mem_aviao->y < 0.5f){
                        mem_aviao->y += 0.05f; // se o aviao iniciar abaixo do aeroporto  
                        printf("[AVIÃO %d] Ajustando altura: nova posição Y = %.2f\n", mem_aviao->pid, mem_aviao->y);
                    }  
                    else if (mem_aviao->y > 0.5f){
                        mem_aviao->y -= 0.05f; //se iniciar acima do aeroporto  
                        printf("[AVIÃO %d] Ajustando altura: nova posição Y = %.2f\n", mem_aviao->pid, mem_aviao->y);
                    } 
                    
                }
                else if (mem_aviao->status == STATUS_PAUSADO) puts("Aviao Pausado! Aguardando instrucoes da Torre!\n");
                
                sleep(1);
            }
    return 0;
}
