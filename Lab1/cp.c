#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFERSIZE 1024
#define COPYMORE 0644

void oops(char *, char *);
int copyFiles(char *src, char *dest);

int main(int ac, char *av[])
{
   char *src = av[1];
   char *dest = av[2];

   copyFiles(src, dest);
}

int copyFiles(char *source, char *destination)
{
  int in_fd, out_fd, n_chars;
  char buf[BUFFERSIZE];

  if( (in_fd = open(source, O_RDONLY)) == -1 )
  {
    oops("Cannot open ", source);
  }

  if( (out_fd = creat(destination, COPYMORE)) == -1 )
  {
    oops("Cannot creat ", destination);
  }

  while( (n_chars = read(in_fd, buf, BUFFERSIZE)) > 0 )
  {
    if( write(out_fd, buf, n_chars) != n_chars )
    {
      oops("Write error to ", destination);
    }

    if( n_chars == -1 )
    {
      oops("Read error from ", source);
    }
  }

  if( close(in_fd) == -1 || close(out_fd) == -1 )
  {
    oops("Error closing files", "");
  }

  return 1;
}


void oops(char *s1, char *s2)
{
  fprintf(stderr, "Error: %s ", s1);
  perror(s2);
  exit(1);
}
