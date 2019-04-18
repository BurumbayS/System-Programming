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
#include <pthread.h>
#include <time.h>

#define BUFFERSIZE 1024
#define COPYMORE 0644

int table[10][10];

struct coordinates {
    int row;
    int line;
    int thread;
};

void readTable() {
    FILE* in;

    in = fopen("input.txt","r");
    if (!in) {
        printf("File opening error\n" );
    } else {
        char c;
        int row = 0;
        int line = 0;

        while ((c = getc(in)) != EOF) {
            int x = (int)(c) - 48;
            if (x >= 0 && x <= 9) {
                table[line][row] = x;
                row++;
            }

            if (row == 9) {
                row = 0;
                line++;
            }
        }
        fclose(in);
    }
}

void *check (void *arguments) {
    struct coordinates *args = arguments;
    int row = args -> row;
    int line = args -> line;
    int thread = args -> thread;

    printf("Cur thread: %d\n", thread);

    int use[10];
    for (int i = 0; i <= 9; i++) {
        use[i] = 0;
    }

    for (int i = line; i < line + 3; i++) {
        for (int j = row; j < row + 3; j++) {
            use[table[i][j]] = 1;
        }
    }

    int isCorrect = 1;
    for (int i = 1; i <= 9; i++) {
        //printf("Cur thread: %d Use: %d\n", thread, use[i]);
        if (use[i] == 0) { isCorrect = 0; }
    }

    // printf("%d\n", isCorrect);

    int *intPointer = calloc(1,sizeof(int));
	*intPointer = isCorrect;

	void *pointer = calloc(1, sizeof(int));
	pointer = (void *)intPointer;

    pthread_exit(pointer);
}

int main () {
    readTable();

    pthread_t threads[9];
    pthread_t lineThread, rowThread;

    struct coordinates coords;

    int row = 0;
    int line = 0;
    int curThread = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            coords.row = row;
            coords.line = line;
            coords.thread = curThread;
            // printf("%d\n", curThread);
            if(pthread_create(&threads[curThread], 0, check, &coords)) {
                fprintf(stderr, "Error creating thread\n");
                return 1;
            }
            row += 3;
            curThread++;
        }
        row = 0;
        line += 3;
    }

    void *result = NULL;
    for (int i = 0; i < 9; i++) {
        pthread_join(threads[i], &result);
        int res = *((int *)(result));
        // printf("%d\n", res);
    }
}
