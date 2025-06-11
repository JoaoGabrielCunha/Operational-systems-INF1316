#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd = open("shared.bin", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char *map = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    const char *mensagem = "Olá do prog1 via mmap!";
    strcpy(map, mensagem);  // Escreve diretamente na memória mapeada

    printf("prog1 escreveu: %s\n", mensagem);

    munmap(map, 1024);
    close(fd);
    return 0;
}
