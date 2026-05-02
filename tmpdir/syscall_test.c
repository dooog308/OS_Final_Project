#include <stdio.h>
#include <syscall.h>

int main(void)
{
	long ret=123;
	
	printf("before syscall: %ld\n", ret);
	ret = syscall(471, ret);
	printf("after syscall: %ld\n", ret);
	while(1);

	return 0;
}
