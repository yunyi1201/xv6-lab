#include"kernel/types.h"
#include"kernel/param.h"
#include"user/user.h"

#define MaxLine 512
int main( int argc, char *argv[] ) 
{
  if( argc < 2 )
  {
    fprintf(2,"uasge:xargs agument too few\n");
    exit(1);
  }
  if( argc + 1 > MAXARG ) {
    fprintf(2,"uasge:xargs args too much\n");
  }
  char buf[1024];
  char *args[MAXARG];
  char *cmd = argv[1];
 // int index = 0;
  for( int i=1; i<argc; i++ ) args[i-1] = argv[i];
  //args[index] = 0;
  while(1) {
    int i = 0, len;
    while(1) {
      len = read(0,&buf[i],1);
      if( len == 0 || buf[i] == '\n' ) break;
      i++;
    } 
    if( i == 0 ) break;
    buf[i] = 0;
    args[argc-1] = buf;
    args[argc] = 0;
    if( fork() == 0 ) {
      exec(cmd,args);
      exit(0);
    } else 
      wait(0);
  }
  exit(0);
}
