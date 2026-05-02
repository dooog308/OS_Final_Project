#include<stdio.h>
#include<string.h>

#define LOOP_SIZE   (1024)
#define WRITE_SIZE  (1024*1024)

int main(void)
{
	FILE *fp = fopen("./io_bound_test.txt", "w+");
	char s[WRITE_SIZE] = {0};

	memset(s, 'h', WRITE_SIZE);
	for(int i=0;i<LOOP_SIZE;i++) fprintf(fp, "%s", s);
	fclose(fp);

	return 0;
}
