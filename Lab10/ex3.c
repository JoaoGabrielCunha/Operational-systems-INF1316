#define _GNU_SOURCE
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    const char *msg = "Alterando conteúdo do arquivo via mmap.\n";
    size_t msg_len = strlen(msg);

    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct stat sb;
    fstat(fd, &sb);
    size_t filesize = sb.st_size;

    int pagesize = getpagesize();
    int num_pages = (filesize + pagesize - 1) / pagesize;

    printf("Tamanho do arquivo: %ld bytes\n", filesize);
    printf("Tamanho de página: %d bytes\n", pagesize);
    printf("Número de páginas usadas: %d\n", num_pages);

    // --- Escrita via mmap
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);

    char *map = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    memcpy(map, msg, msg_len); // sobrescreve início do arquivo
    msync(map, filesize, MS_SYNC); // garante escrita em disco

    gettimeofday(&end1, NULL);

    munmap(map, filesize);

    // --- Escrita via write()
    struct timeval start2, end2;
    lseek(fd, 0, SEEK_SET); // volta ao início

    gettimeofday(&start2, NULL);
    write(fd, msg, msg_len);
    gettimeofday(&end2, NULL);

    close(fd);

    long mmap_us = (end1.tv_sec - start1.tv_sec) * 1000000 + (end1.tv_usec - start1.tv_usec);
    long write_us = (end2.tv_sec - start2.tv_sec) * 1000000 + (end2.tv_usec - start2.tv_usec);

    printf("Tempo mmap:  %ld us\n", mmap_us);
    printf("Tempo write: %ld us\n", write_us);

    return 0;
}
