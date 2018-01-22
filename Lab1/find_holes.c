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

    if (ac < 2) {
         fprintf(stderr, "No entry file\n");
         return 0;
    }

    struct stat fileStat;
    if ((stat(av[1], &fileStat)) < 0) {
        fprintf(stderr, "Stat error: %s\n", strerror(errno));
    } else {
        printf("Theoretical blocks num is %ld\n", (long)fileStat.st_size / 512);
        printf("Actual blocks num is %ld\n", (long)fileStat.st_blocks);
        if ((long)fileStat.st_size / 512 > (long)fileStat.st_blocks) {
            printf("Holes are existed\n");
        }
    }

    return 0;
}
