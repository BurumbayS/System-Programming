#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

pid_t fork(void);

int main(int argc, char const *argv[])
{
    pid_t pid;

    pid = fork();
    switch (pid) {
        case -1:
            perror(argv[0]);
            exit(EXIT_FAILURE);
            break;
        case 0:
            // child
            sleep(3);
            assert(getppid() == 1);
            break;
        default:
            _exit(EXIT_SUCCESS);
            break;
    }
    return 0;
}
