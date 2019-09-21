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

/* Essa função tem a finalidade de  colocar todas as palavras presentes no buffer dentro de um vetor 
 * de string.
 * OBS: a função strtok é utilizada para devidir uma string dado um delimitador. Essa função destroi a
 * a string posteriormente, por isso foi necessario fazer uma copia do buffer.		 */
char **divLinha(char *buffer){
	char *aux; // Variavel auxiliar para armazenar palavras do buffer.
	int cont = 0;
	int i = 0;

	char *copiaBuffer = malloc(sizeof(char) * strlen(buffer+1));
	strcpy(copiaBuffer,buffer); // Copia o buffer para outra variavel para utilizar a funcao strtok

	/* Conta a quantidade de palavras dentro do buffer. */
	aux = strtok(buffer," ");
	while(aux != NULL){
		cont++;
		aux = strtok(NULL," ");
	}

	/* Instancia o vetor de strings com o tamanho referente a quantidade de palavras. */
	char **listaParametros = malloc(sizeof(char *) * cont);

	/* Encontra todas as palavras e coloca dentro do vetor de strings. */
	aux = strtok(copiaBuffer," ");
	listaParametros[i] = malloc( (strlen(aux)+1) * sizeof(char));
	listaParametros[i] = aux;

	for(i = 1; i < cont; i ++){
		aux = strtok(NULL," ");
		listaParametros[i] = malloc( (strlen(aux)+1) * sizeof(char));
		listaParametros[i] = aux;

	}

	return listaParametros; // Retorna o vetor de strings com todas as palavras do buffer.
}

/* Essa função tem coomo finalidade interpretar o comando relacionado a execução de programas. */
int progFunc(char **listaPalavras, int tam){
	int status;
	if(tam == 1){
		status = fork();
		if(!status){
			execve(listaPalavras[0],NULL,NULL);
			printf("miniShell: %s: comando não encontrado\n",listaPalavras[0]);
			exit(0);
		}
		else{
			waitpid(-1,&status,0);
		}
	}

	if(tam == 2 && strcmp(listaPalavras[1],"&") == 0){
		status = fork();
		if(!status){
			execve(listaPalavras[0],NULL,NULL);
			printf("miniShell: %s: comando não encontrado\n",listaPalavras[0]);
			exit(0);
		}
		return status;
	}

	return 0;
}

/* Essa função tem como finalidade identificar o comando. */ // A ideia é essa funcao meio que organizar a logica, ai tera uma função pra cada funcionalidade do shell.
int interComando(char *buffer){ 
	int status;  // Guarda o status para auxiliar no valor de retorno da função.
	char ** listaPalavras; // Guarda as palavras presentes no buffer.
	listaPalavras = divLinha(buffer);

	int tam = 0; // Guarda o tamanho do vetor de strings.
	while(listaPalavras[tam] != NULL)
		tam++;



	if(strcmp(listaPalavras[0],"pwd") == 0 && tam == 1){

	}
	if(strcmp(listaPalavras[0],"job") == 0 && tam == 1){
		
	}
	if(strcmp(listaPalavras[0],"fg") == 0 && tam == 1){
		
	}
	if(strcmp(listaPalavras[0],"bg") == 0 && tam == 1){
		
	}
	if(strcmp(listaPalavras[0],"cd") == 0 && tam == 1){
		
	}
	if(strcmp(listaPalavras[0],"exit") == 0 && tam == 1){
		return 0;
	}

	// Caso nenhum desses comandos ele vai interpretar como os casos de prog.	
	status = progFunc(listaPalavras, tam); // status caso tenha programa em background tera o pid do processo filho.

	return 1;
}



int main(){

	char buffer[255]; // Buffer para ler o comando.
	int resp;

	do{

		printf("$");
		fgets(buffer,254,stdin);
		buffer[strlen(buffer)-1] = '\0';
		resp = interComando(buffer);

	}while(resp);

	return 0;
}
