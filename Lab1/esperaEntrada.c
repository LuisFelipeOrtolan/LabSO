/* Programa basico teste para o execve, printando os argumento de arc e argv. */
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[]){
	int i,j;
	scanf("%d",&i);
	scanf("%d",&j);
	printf("i: %d, j: %d\n",i,j);
	sleep(5);
	printf("\nargc: %d\n",argc);
	for(int i = 0; i < argc; i++)
		printf("argv[%d]: %s\n",i,argv[i]);
	return 0;
}