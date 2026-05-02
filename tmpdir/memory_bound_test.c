
#include<stdio.h>
#include<stdlib.h>

#define LOOP_SIZE	  (1024)
#define MALLOC_SIZE   (1024*1024)


int main(void)
{
	long ret=9;
	int *p=NULL;
	
	for(int i=0;i<LOOP_SIZE;i++)
	{	
		p = (int*)malloc(sizeof(int)*MALLOC_SIZE);
		for(int j=0;j<MALLOC_SIZE;j++) p[j] = 1;
		free(p);
	}

	return 0;
}
