/* Programa basico teste para o execve, printando os argumento de arc e argv. */
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[]){

	sleep(200);	
	printf("\nargc: %d\n",argc);
	for(int i = 0; i < argc; i++)
		printf("argv[%d]: %s\n",i,argv[i]);
	return 0;
}