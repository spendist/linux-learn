#include <stdio.h>

main(int argc,char *argv[])
{
	int i,j;
	for (i=1;i<10;i++) {
		for (j=1;j<i+1;j++) {
			printf("%dx%d=%d  ",i,j,i*j);
		}
		printf("\n");
	}
	printf("\n");
	return (0);
}
