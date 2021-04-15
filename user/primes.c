#include"kernel/types.h"
#include"user/user.h"

void child_pro( int p[2] ) {
  int prime;
  int num;
  int pp[2];
  close(p[1]);
  int len = read(p[0],&prime,sizeof(int));
  if( len == 0 ) {
    close(p[0]);
    exit(0);
  }
  printf("prime %d\n",prime);
  pipe(pp);
  if( fork() == 0 ) {
    close(p[0]);
    child_pro(pp);
  } else {
    close(pp[0]);
    while(1) {
      len = read(p[0],&num,sizeof(int));
      if( len == 0 ) {
	break;
      }
      if( num % prime != 0 ) {
	      write(pp[1],&num,sizeof(int));
      }
    }
    close(p[0]);
    close(pp[1]);
    wait(0);
  }
  exit(0);
}
int main( int argc, char *argv[] )
{
  int p[2];
  pipe(p);
  if( argc > 1 ) {
    fprintf(2,"usage: prime");
  }
  if( fork() == 0 ) {
    child_pro(p);
    wait(0);   
  } else {
    close(p[0]);
    for( int i=2; i<=35; i++ ) {
      write(p[1],&i,sizeof(int));
    }
    close(p[1]);
    wait(0);
  }
  exit(0);
}
