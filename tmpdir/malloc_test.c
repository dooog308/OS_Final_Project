//#include "../tools/include/nolibc/stdio.h"
//#include "../tools/include/nolibc/stdlib.h"

#include<stdio.h>
#include<stdlib.h>

int main(void)
{
	long ret=9;
	int *p=NULL;

	p = (int*)malloc(sizeof(int)*10240);
	while(1)
	{
		for(int i=0;i<1024;i++) p[0] = 1;
	}
	free(p);
	return 0;
}
