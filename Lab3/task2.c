#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
  setbuf(stdout, NULL);

  pid_t parent, child;
  switch (parent = fork()) {
  case -1:
    perror(argv[0]);
    exit(EXIT_FAILURE);
    break;
  case 0:
    switch (child = fork()) {
    case -1:
        perror(argv[0]);
        exit(EXIT_FAILURE);
        break;
    case 0: /* child */
        printf("child PPID: %ld\n", (long) getppid());
        sleep(2);
        printf("child PPID: %ld\n", (long) getppid());
        break;
    default: /* parent */
        sleep(1);
        printf("parent exiting -- going into zombie state\n");
    }
    break;
  default: /* grandparent */
    sleep(3);
    printf("grandparent exiting\n");
    break;
  }
}
