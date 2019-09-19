/* Primeiro trabalho de lab de SO. */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>

char *le_comando(char *linha, int *indice, int *cabou){
	char *sublinha = malloc(255 * sizeof(char)); 

	int i;
	for(i = 0; linha[*indice] != '\0' && linha[*indice] != ' '; *indice  = *indice + 1){
		sublinha[i] = linha[*indice];
		i++;
	}

	
	if(linha[*indice] == ' '){
		*cabou = 0;
		while(linha[*indice] == ' ' && linha[*indice] != '\0')
			*indice  = *indice + 1;

	}
	if(linha[*indice] == '\0')
		*cabou = 1;

	sublinha[i] = '\0';
	return sublinha;
}

int main(){

	// char buffer[255]; // Buffer para ler o comando.
	// pid_t pid; // Variavel para guardar o PID do processo.
	// char **vet = NULL;
	// int status;
	// int indice = 0;
	// do{
	// 	printf("$");
	// 	fgets(buffer,254,stdin);
	// 	buffer[strlen(buffer)-1] = '\0';
	// 	pid = fork();
	// 	if(!pid){
	// 		execve(buffer,vet,NULL);
	// 		exit(0);
	// 	}
	// 	else{
	// 		waitpid(-1,&status,0); // Processo pai espera o filho.
	// 	}

	// }while(strcmp(buffer,"exit") != 0);

	char buffer[255];
	fgets(buffer,254,stdin);

	int i = 0;
	int cabou = 0;
	while(cabou == 0){
		char *sublinha = le_comando(buffer,&i,&cabou);

		printf("%s\n",sublinha);
		printf("%d\n",i);
		printf("%d\n",cabou);
}
	return 0;
}