#include "kernel/types.h"
#include "user/user.h"

int main()
{
	char c;
	if(fork() == 0) {
		c = '\\';
	} else {
		c = '/';
	}
	for(int i=0; ; i++){
		if(i % 100000 == 0)
			write(2, &c, 1);
	}
	exit(0);
}
