#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFERSIZE 1024

int main (int ac, char* av[]) {

    int fd;
    char* random_string = "Hello world!  ";

    if (ac < 2) {
        fprintf(stderr, "No file name\n");
    }

    if ((fd = open(av[1], O_WRONLY | O_CREAT, 0666)) == -1) {
        fprintf(stderr, "Opening error: %s\n", strerror(errno));
    }

    for (int i = 0; i < 5; i++) {
        if ((write(fd, random_string, 12)) != 12) {
            fprintf(stderr, "Write error: %s\n", strerror(errno));
        }
        if ((lseek(fd, 5 * 4069, SEEK_CUR)) < 0) {
            fprintf(stderr, "Seek error: %s\n", strerror(errno));
        }
    }

    if ((close(fd)) == -1) {
        fprintf(stderr, "File close error: %s\n", strerror(errno));
    }

    return 0;
}
