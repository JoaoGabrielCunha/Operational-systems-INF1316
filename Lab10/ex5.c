#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

int main() {
    const char *arquivo = "dados.txt";
    const char *novos_dados = " <-- adicionado via mmap\n";

    int fd = open(arquivo, O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // Verifica o tamanho original
    struct stat sb;
    fstat(fd, &sb);
    size_t tamanho_antigo = sb.st_size;
    size_t tamanho_novo = tamanho_antigo + strlen(novos_dados);

    // Expande o arquivo
    if (ftruncate(fd, tamanho_novo) == -1) {
        perror("ftruncate");
        close(fd);
        return 1;
    }

    // Faz o mapeamento com o novo tamanho
    char *map = mmap(NULL, tamanho_novo, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    // Escreve no final do arquivo mapeado
    memcpy(map + tamanho_antigo, novos_dados, strlen(novos_dados));

    // Garante que a alteração vá para o disco
    msync(map, tamanho_novo, MS_SYNC);

    munmap(map, tamanho_novo);
    close(fd);

    printf("Dados adicionados com sucesso.\n");
    return 0;
}
