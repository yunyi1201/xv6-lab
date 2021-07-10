#include"kernel/types.h"
#include"user/user.h"

int main( int argc, char *argv[] ) {
  if( argc < 2 ) {
    fprintf(2,"sleep: too few agument\n");
    exit(1);
  }
  int tikes = atoi(argv[1]);
  sleep(tikes);
  exit(0);
}
