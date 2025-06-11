#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void *memorypool;

void initmemorypool(void) {
    memorypool = sbrk(0); // Marca o início do pool
}

void *myallocate(int n) {
    return sbrk(n); // Aloca n bytes do heap
}

void resetmemorypool(void) {
    brk(memorypool); // Desaloca tudo de uma vez
}

int main(void) {
    initmemorypool();

    char *nome = (char*) myallocate(50);
    int *idade = (int*) myallocate(sizeof(int));

    strcpy(nome, "João Gabriel");
    *idade = 22;

    printf("Nome: %s\n", nome);
    printf("Idade: %d\n", *idade);

    resetmemorypool(); // Desaloca tudo que foi alocado com myallocate

    return 0;
}
