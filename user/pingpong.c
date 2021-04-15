#include"kernel/types.h"
#include"user/user.h"

int main( int argc, char *argv[] ) 
{
  int p[2];
  char content;
  if( argc > 1 ) {
    fprintf(2,"usage: pingpong\n");
    exit(1);
  }
  pipe(p);
  if( fork() == 0 ) {
    close(p[1]);
    read(p[0],&content,1);
    close(p[0]);
    printf("%d: received ping\n", getpid());
    write(p[1],"0",1);
    close(p[1]);
  } else {
    close(p[0]);
    write(p[1],"0",1);
    wait(0);
    close(p[1]);
    read(p[0],&content,1);
    close(p[0]);
    printf("%d: received pong\n", getpid());
  }
  exit(0);
}
