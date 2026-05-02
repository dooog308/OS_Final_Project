#include "../tools/include/nolibc/stdio.h"
#include "../tools/include/nolibc/stdlib.h"

int main(void)
{
	int *p=(int*)malloc(sizeof(int)*4096);
	
	for(int i=0;i<4096;i++) *p = 1;
	while(1);

	return 0;
}
