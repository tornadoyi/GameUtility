#include <stdio.h>
#include <unistd.h> 
#include "testso.h"



int main()
{
	printf("pid = %d\n", getpid());
	printf("%s\n", get_key());
	return 0;
}


