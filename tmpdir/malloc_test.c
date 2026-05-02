//#include "../tools/include/nolibc/stdio.h"
//#include "../tools/include/nolibc/stdlib.h"

#include<stdio.h>
#include<stdlib.h>

int main(void)
{
	long ret=9;
	int *p=NULL;

	
	while(1)
	{
		p = (int*)malloc(sizeof(int)*1024);
		free(p);
	}

	return 0;
}
