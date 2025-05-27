#include <stdio.h>
#include <stdlib.h>
#include "aviao.h"

void teste_attach(Aviao* frota) {
    if (frota == (void*)-1) {
        perror("Erro ao anexar memoria compartilhada!\n");
        exit(-1);
    }
}