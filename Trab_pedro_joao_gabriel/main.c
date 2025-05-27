#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <sys/stat.h>
#include <math.h>
#include "aviao.h"

//Variaveis Globais;
int em_pausa;
int N;
Aviao* frota;
int segmento;

//LEMBRAR COLOCAR CADA FUNÇÃO UTILIZADA POR CADA INCLUDE - PODE AJUDAR A ESCREVER O RELATÓRIO E REMOVER INCLUDES DESNECESSÁRIOS
//TIRAR DPS
void round_robin(int pid, float tempo);
void testa_mem(int segmento);
void teste_attach(Aviao* frota);
float calcula_dist2(float x1, float y1, float x2, float y2);
void trata_pause_user(int sinal);
void trata_exit_user(int sinal);
void trata_retomar_user(int sinal);


int main(int argc, char* argv[]){
    
    if (argc != 2) {
        fprintf(stderr, "Uso correto: main3 <n_avioes>\n");
        exit(1);
    }
    N = atoi(argv[1]);
    
    int pid_f,pid_interface, i,pid_pai = getpid();
    /*INICIALIZAÇÃO DA MEMÓRIA COMPARTILHADA */
    segmento = shmget(IPC_PRIVATE, N * sizeof(Aviao), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    testa_mem(segmento);
    frota = (Aviao*)shmat(segmento, NULL, 0);
    teste_attach(frota);
    /*****************************************/
    signal(SIGUSR1, trata_pause_user);
    signal(SIGUSR2, trata_exit_user);
    signal(SIGCONT, trata_retomar_user);

    /*Inicializa Interface!                  */ 
    pid_f = fork();
    if (pid_f < 0) {
        perror("Erro ao criar filho!");
        exit(1);
    }
    else if (pid_f == 0){
        char id_shm[20];
        sprintf(id_shm, "%d", segmento);  // Converte o ID do segmento para string
        pid_interface = getpid();
        execl("./interface", "interface", id_shm, NULL);
        perror("Erro ao executar interface"); // Se chegar aqui, houve erro no execl
        exit(1);
    }  
    /*************************************** */
    //Criação da interface para o usuário controlar

    pid_t pid_input_terminal = fork();
    if (pid_input_terminal == 0) {
        char pid_str[20];
        char seg[20];
        char N_str[20];
        sprintf(pid_str, "%d", pid_pai); // ou getpid() se for no mesmo escopo
        sprintf(seg, "%d", segmento); // ou getpid() se for no mesmo escopo
        sprintf(N_str, "%d", N); // ou getpid() se for no mesmo escopo
        
        execlp("xterm", "xterm", "-e", "./comandos", pid_str, seg, N_str, NULL);
        perror("Erro ao abrir terminal para comandos");
        exit(1);
    }



    /*Criação dos Processos Filho (Aviões)*/

    int pids[N];       //Vetor para armazenar os pids    
    for (i = 0; i < N; i++){
        if ((pid_f = fork()) < 0 ){
            perror("Erro ao criar processo filho\n");
            exit(1);
        }
        else if (pid_f == 0){
            char indice[10], id_shm[10];
            sprintf(indice, "%d", i);//transformando i em string para entrar no argv de voar
            sprintf(id_shm, "%d", segmento);//transformando o i
            // printf("%s\n",indice);printf("%s\n",id_shm);
            sleep(2);
            execl("./voar", "voar", indice, id_shm, NULL);//Implementar teste para execl (que nem na prova P1)
            exit(1);
        }
            else {
            pids[i] = pid_f;
        }
    }

    /***************************************** */
    // printf("ID da Frota (shm) = %d\n\n", segmento);
    

    int voo_atual = 0;
    int voos_ativos = N;
    sleep(3);

    while (voos_ativos > 0) { //Round Robin
        if (em_pausa == 1){
            printf("Recebi um pause!\n");
            pause();
        }
        
        printf("While 1 do pai\n");
        printf("//-------------------------------------------------------------------------//\n");
        printf("Aviao %d\n",voo_atual);
        printf("Meu status: %d\n", frota[voo_atual].status);
        printf("Avioes ativos = %d\n", voos_ativos);

        if (frota[voo_atual].status == STATUS_FORA_ESPACO_AEREO){
            goto round_robin;

        }
        if (frota[voo_atual].status == STATUS_VOANDO) {

            for (int j = 0; j < N; j++){
                //CASOS SEM RISCO!!
                if (j == voo_atual) continue;
                if (frota[j].status != STATUS_VOANDO) continue;
                else if (frota[j].direcao != frota[voo_atual].direcao) continue; //Direcoes diferentes
                else if (frota[j].pista_destino != frota[voo_atual].pista_destino) continue; //Direcoes iguais, mas pistas diferentes
                //***************/

                //CASOS COM RISCO (Mesma direção e mesma pista)!!
                else {
                    float dist2 = calcula_dist2(frota[voo_atual].x, frota[voo_atual].y, frota[j].x, frota[j].y);
                    printf("Caso de possivel risco! Distacia ao quadrado entre os avioes %d e %d: %f\n", voo_atual, j, dist2);
                    
                    if (dist2 < 0.01f){ //Caso a distancia entre eles seja menor que 0.1 (ja era!)
                        printf("Distancia critica! Matando o voo %d!\n", voo_atual);
                        kill(pids[voo_atual], SIGKILL);
                        frota[voo_atual].status = STATUS_REMOVIDO;
                        voos_ativos--;
                        goto proximo_voo;
                    }
                    
                    else if ((0.01f < dist2) && (dist2 < 0.04f)){ //Caso o aviao esteja a uma distancia entre 0.2 e 0.1 de outro aviao, tomar uma atitude!
                        //Podemos mudar fulano de pista! Mas posteriormente é preciso verificar se não tem ninguem indo para essa pista por perto tambem.
                        puts("Distancia perigosa! Analisando Situacao...\n");
                        //Vamos ver se podemos mudar o atual de pista:
                        for (int k = 0; k < N; k++){
                            //Para todas as pistas que estão na mesma direção mas não estão na mesma pista
                            if ((frota[k].pista_destino != frota[voo_atual].pista_destino) && (frota[k].direcao == frota[voo_atual].direcao)){ 
                                
                                float dist_alt2 = calcula_dist2(frota[voo_atual].x, frota[voo_atual].y, frota[k].x, frota[k].y); //Distancia euclidiana ao quadrado
                                
                                if (dist_alt2 < 0.04f){ //Se o aviao estiver muito perto, nao podemos mudar de pista de jeito nenhum! - Primeiro caso em que ele para! (toogle de status)
                                    printf("Aviao %d esta muito perto do aviao da outra pista para mudar de pista, entao tera que reduzir a velocidade(pausa)\n", voo_atual);
                                    printf("Meu status: %d\n", frota[voo_atual].status);
                                    printf("Enviando sinal de freio!\n");
                                    if (kill(pids[voo_atual], SIGUSR1) == -1) {//Reduz a velocidade! (para)
                                        perror("Erro ao enviar SIGUSR1");
                                        exit(-1); // TALVEZ SUBSTITUIR POR KILL DEPOIS
                                    }
                                    else printf("Sinal de freio enviado corretamente!\n");
                                    goto round_robin;
                                }
                                else {
                                    continue;
                                    puts("to no else continue\n");
                                }
                            }
                            else continue; //Passa para o proximo k
                        }
                        //Se nenhum aviao da outra pista tambem esta perto, podemos mudar de pista!
                        kill(pids[voo_atual], SIGUSR2); //Muda o aviao de pista! (toogle de pista)
                        printf("Enviando sinal de troca de pista!\n");
                        usleep(500000);
                        goto round_robin;

                    }

                }


            }
            round_robin:                 
            
            round_robin(frota[voo_atual].pid,2);

            printf("Local do aviao %d: %f %f\n",voo_atual,frota[voo_atual].x, frota[voo_atual].y);
            // Verifica se pousou
            if (frota[voo_atual].x == 0.5f && frota[voo_atual].y == 0.5f) {
                printf("Controlador: avião PID %d pousou.\n", frota[voo_atual].pid);
                frota[voo_atual].status = STATUS_ATERRISSADO;
                // kill(pids[voo_atual],SIGKILL); //Estamos matando pois pode ser que o round robin atrapalhe isso. -> NA VERDADE NAO, POIS O SIGCONT PARTE EXATAMENTE DE ONDE PAROU ANTES
                voos_ativos--;
            }
            if (frota[voo_atual].status == STATUS_PAUSADO) {
                goto proximo_voo; //Se chega nesse if, significa que acabou de mudar de estado. (Voando -> Pausado.)
            }
        }
        else if (frota[voo_atual].status == STATUS_ATERRISSADO || frota[voo_atual].status == STATUS_REMOVIDO) {
            // Já não está voando, ignora
            // printf("Avião %d já finalizado\n", atual);
        }
        else if (frota[voo_atual].status == STATUS_PAUSADO){
            printf("Avião %d está pausado. Verificando se pode continuar...\n", voo_atual);
            
            // Flag para indicar se podemos continuar
            int pode_continuar = 1;
            
            for (int j = 0; j < N; j++) {
                if (j == voo_atual) continue;
                if ((frota[j].status == STATUS_REMOVIDO) || (frota[j].status == STATUS_ATERRISSADO)) continue;
                
                // Calcula distância para outros aviões
                float dist2 = calcula_dist2(frota[voo_atual].x, frota[voo_atual].y, frota[j].x, frota[j].y);
                
                // Se ainda estiver muito perto de algum avião
                if (dist2 < 0.04f) {
                    // Calcular distância de cada avião para a pista
                    float dist_pista_atual2 = calcula_dist2(frota[voo_atual].x, frota[voo_atual].y, 0.5f, 0.5f);
                    float dist_pista_j2 = calcula_dist2(frota[j].x, frota[j].y, 0.5f, 0.5f);
                    
                    // Se estão na mesma pista, verificar prioridade
                    if (frota[voo_atual].pista_destino == frota[j].pista_destino) {
                        // Prioriza quem estiver mais perto da pista
                        if (dist_pista_atual2 <= dist_pista_j2) {
                            printf("Avião %d está mais próximo da pista que %d. Liberando...\n", voo_atual, j);
                            // Continua - prioridade para este avião
                        } else {
                            printf("Avião %d deve continuar esperando, %d está mais próximo da pista.\n", voo_atual, j);
                            pode_continuar = 0;
                            break; // Não precisa verificar os outros
                        }
                    } 
                    // Se estão em pistas diferentes OU direções diferentes, não há risco
                    else {
                        printf("Aviões %d e %d estão em pistas diferentes ou direções diferentes. Sem risco de colisão.\n", voo_atual, j);
                        // pode_continuar permanece true - não há problema
                    }
                }
            }
            
            if (pode_continuar) {
                printf("Avião %d pode continuar seu voo!\n", voo_atual);
                kill(frota[voo_atual].pid, SIGUSR1);
                kill(frota[voo_atual].pid, SIGCONT); // Envia sinal para despausar
                 // Envia sinal para despausar
                // Esperamos um pouco para o avião mudar seu status
                sleep(2); // 100ms
                if (frota[voo_atual].status == STATUS_VOANDO) {
                    printf("Avião %d retomou o voo com sucesso.\n", voo_atual);
                }
                // kill(frota[voo_atual].pid, SIGSTOP); MUDEI AQUI
                // if (frota[voo_atual].x == 0.5f && frota[voo_atual].y == 0.5f) { //Talvez deixar if status == aterrissado
                //     printf("Controlador: avião PID %d pousou.\n", frota[voo_atual].pid);
                //     frota[voo_atual].status = STATUS_ATERRISSADO;
                //     voos_ativos--;
                // } //O aviao nao pode ficar infinitamente voando se for despausado 
                
            }
            
            goto proximo_voo;
        }
         
        proximo_voo:
        voo_atual = (voo_atual + 1) % N;
        printf("//-------------------------------------------------------------------------//\n");

    }
    sleep(5); //Da um tempo antes de matar a interface;

    printf("Informações finais de todos os aviões:\n");
    for (i = 0; i < N; i++) {
        printf("Avião %d:\n", i);
        printf("  PID: %d\n", frota[i].pid);
        printf("  Status: %d\n", frota[i].status);
        printf("  Posição: (%f, %f)\n", frota[i].x, frota[i].y);
        printf("  Direção: %d\n", frota[i].direcao);
        printf("  Pista de destino: %d\n", frota[i].pista_destino);
        printf("//-------------------------------------------------------------------------//\n");
    }


    kill(pid_interface, SIGKILL); //mata a interface


    //Desanexando e limpando memoria compartilhada
    for (i = 0; i < N; i++){
        printf("Esperando os filhos terminarem\n");
        wait(NULL);
        
    }
    shmdt(frota);
    shmctl(segmento, IPC_RMID, 0);
    return 0;
    
}

void testa_mem(int segmento){
    if (segmento == -1){
        perror("Erro ao criar a memoria compartilhada!\n");
        exit(-1);
    }
    return;
}

float calcula_dist2(float x1, float y1, float x2, float y2){
    float dx = x1 - x2;
    float dy = y1 - y2;
    float dist2 = dx * dx + dy * dy;
    return dist2;
}

void round_robin(int pid, float tempo){
    kill(pid, SIGCONT); // Se o filho ta voando, voa normal, se foi Pausado, recebe um SIGCONT para entao receber os Sinais pendentes da Torre.
    sleep(tempo); // tempo de "voo", dando 2 segundos pro filho voar
    kill(pid, SIGSTOP);

}



void trata_pause_user(int sinal){
    printf("Recebi o sinal de pause %d\n",frota[0].pid);
    for(int i = 0; i < N; i++){
        pid_t pid = frota[i].pid;
        kill(pid, SIGSTOP); //Forçando todos os filhos a parar.
         // Printando todas as informações do avião
         printf("Avião %d:\n", i);
         printf("  PID: %d\n", frota[i].pid);
         printf("  Status: %d\n", frota[i].status);
         printf("  Posição: (%f, %f)\n", frota[i].x, frota[i].y);
         printf("  Direção: %d\n", frota[i].direcao);
         printf("  Pista de destino: %d\n", frota[i].pista_destino);
         printf("//-------------------------------------------------------------------------//\n");

    }
    sleep(5);
    em_pausa = 1;
}

void trata_exit_user(int sinal){
    printf("Recebi o sinal de exit\n");
    for(int i = 0; i < N; i++){
        pid_t pid = frota[i].pid;
        kill(pid, SIGKILL); //Matando todos os filhos.
         // Printando todas as informações do avião
         printf("Avião %d:\n", i);
         printf("  PID: %d\n", frota[i].pid);
         printf("  Status: %d\n", frota[i].status);
         printf("  Posição: (%f, %f)\n", frota[i].x, frota[i].y);
         printf("  Direção: %d\n", frota[i].direcao);
         printf("  Pista de destino: %d\n", frota[i].pista_destino);
         printf("//-------------------------------------------------------------------------//\n");

    }
    shmdt(frota);
    shmctl(segmento, IPC_RMID, 0);
    exit(0);
}

void trata_retomar_user(int sinal) {
    printf("Recebi sinal para retomar (SIGCONT)\n");
    em_pausa = 0;
    return;
}