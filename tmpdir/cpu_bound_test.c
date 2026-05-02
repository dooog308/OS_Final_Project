#include<stdio.h>
#include<stdlib.h>

#define LOOP_SIZE    (1024)
#define ARRAY_SIZE   (1024)

int main(void)
{
	int num[ARRAY_SIZE]={0};
	
	for(int i=0;i<LOOP_SIZE;i++)
	{
		for(int j=0;j<ARRAY_SIZE-1;j++)
		{
			for(int k=j+1;k<ARRAY_SIZE;k++)
			{
				if(num[j]>num[k])
				{
					int tem = num[j];
					num[j] = num[k];
					num[k] = tem;
				}
			}
		}
	}

	return 0;
}
