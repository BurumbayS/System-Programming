#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFERSIZE 1024
#define COPYMORE 0666

int fd, n_chars;
char buf[BUFFERSIZE];

void readFile(char* source) {
    if( (fd = open(source, O_CREAT | O_RDWR | O_APPEND, COPYMORE)) == -1 )
    {
        fprintf(stderr, "File opening error: %s\n", strerror(errno));
        exit(0);
    }

    char *out_file = "file.txt";
    int out_fd;
    if( (out_fd = open(out_file, O_CREAT | O_WRONLY, COPYMORE)) == -1 )
    {
        fprintf(stderr, "File opening error: %s\n", strerror(errno));
        exit(0);
    }

    if ((lseek(fd, 50, SEEK_SET)) < 0) {
        fprintf(stderr, "Seek error: %s\n", strerror(errno));
    }
    if ((n_chars = read(fd, buf, 20)) > 0) {
        if( write(out_fd, buf, n_chars) != n_chars )
        {
            fprintf(stderr, "File writing error: %s\n", strerror(errno));
            exit(0);
        }
    } else {
        fprintf(stderr, "File reading error: %s\n", strerror(errno));
        exit(0);
    }

    if ((close(fd)) == -1) {
        fprintf(stderr, "File close error: %s\n", strerror(errno));
    }
    if ((close(out_fd)) == -1) {
        fprintf(stderr, "File close error: %s\n", strerror(errno));
    }

}

void writeToFile(char* source) {
    if( (fd = open(source, O_CREAT | O_RDWR | O_APPEND, COPYMORE)) == -1 )
    {
        fprintf(stderr, "File opening error: %s\n", strerror(errno));
        exit(0);
    }

    char* random_string = "Hello world!";
    for (int i = 1; i < 5; i++) {
        if ((write(fd, random_string, 12)) != 12) {
            fprintf(stderr, "Write error: %s\n", strerror(errno));
            exit(0);
        }
    }

    char* rand_str = "This string should be at the beggining ";
    if ((lseek(fd, 0, SEEK_SET)) < 0) {
        fprintf(stderr, "Seek error: %s\n", strerror(errno));
    }
    if ((write(fd, rand_str, 39)) != 39) {
        fprintf(stderr, "Write error: %s\n", strerror(errno));
    }

    if ((close(fd)) == -1) {
        fprintf(stderr, "File close error: %s\n", strerror(errno));
    }
}

int main (int ac, char *av[]) {
    if (ac < 2) {
        fprintf(stderr, "Please enter a file name\n");
        return 0;
    }

    writeToFile(av[1]);
    readFile(av[1]);
}


//We can read from arbitrary position of the file,
// but cannot change data in arbitrary position, because it always writes to the end of file
// cause of O_APPEND
