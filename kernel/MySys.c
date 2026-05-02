#include<linux/kernel.h>
#include<linux/syscalls.h>

static long MySys_Handler(int a)
{
	return a*a;
}

SYSCALL_DEFINE1(MySys, int, a)
{
	return MySys_Handler(a);
}
