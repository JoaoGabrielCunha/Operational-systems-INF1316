#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("shared.bin", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char *map = mmap(NULL, 1024, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("prog2 leu: %s\n", map);

    munmap(map, 1024);
    close(fd);
    return 0;
}
