#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    size_t n = 1000;
    size_t tamanho = n * sizeof(int);

    // Cria mapeamento de memória anônimo (sem arquivo)
    int *vetor = mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (vetor == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Atribui valores
    for (int i = 0; i < n; i++) {
        vetor[i] = i * 2;
    }

    // Imprime os valores
    for (int i = 0; i < 10; i++) { // mostra só os 10 primeiros
        printf("vetor[%d] = %d\n", i, vetor[i]);
    }

    // Libera a memória mapeada
    munmap(vetor, tamanho);

    return 0;
}
