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
#define COPYMORE 0644
#define SIZE 100

int main(int ac, char* av[]) {
    FILE* in;
    char buf[BUFFERSIZE];
    char buff[1000][BUFFERSIZE];

    in = fopen("input.txt","r");
    if (!in) {
        printf("File opening error\n" );
    }

    int n = atoi(av[1]);

    int i = 0;
    int cnt = 0;
    int start = 0;
    while (fgets (buff[i], BUFFERSIZE, in) != NULL) {
        // printf("%s", buff[i]);
        i++;
        cnt++;
        if (cnt > n) {
            start++;
        }
    }

    // printf("%d\n", start);
    for (int i = start; i < start + n; i++) {
        printf("%s", buff[i]  );
    }
}
