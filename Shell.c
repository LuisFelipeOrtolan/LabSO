/* Primeiro trabalho de lab de SO. */
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include "Celula.h"

extern int errno; // Variavel utilizada para guardar erros ocorridos.

Celula *ini = NULL;

/* Essa função tem a finalidade de  colocar todas as palavras presentes no buffer dentro de um vetor 
 * de string.
 * OBS: a função strtok é utilizada para devidir uma string dado um delimitador. Essa função destroi a
 * a string posteriormente, por isso foi necessario fazer uma copia do buffer.		 */
char **divLinha(char *buffer, int *tam){
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
	char **listaParametros = malloc(sizeof(char *) * (cont+1) );

	/* Encontra todas as palavras e coloca dentro do vetor de strings. */
	aux = strtok(copiaBuffer," ");
	listaParametros[i] = malloc( (strlen(aux)+1) * sizeof(char));
	listaParametros[i] = aux;

	for(i = 1; i < cont; i ++){
		aux = strtok(NULL," ");
		listaParametros[i] = malloc( (strlen(aux)+1) * sizeof(char));
		listaParametros[i] = aux;

	}

	*tam = cont; // Retorna através da variavel tam o tamanho do vetor de string.
	return listaParametros; // Retorna o vetor de strings com todas as palavras do buffer.
}

/* Essa função tem coomo finalidade interpretar o comando relacionado a execução de programas.
 * OBS: o argc é calculado no load, sabendo a primeira ocorrencia do NULL no argv. */
int progFunc(char **listaPalavras, int tam){
	int status;
	listaPalavras[tam] = NULL; // Necessario marcar o final do array com NULL se nao da erro Bad Adress.

	/* Execução de programa com parametros background. */
	if((strcmp(listaPalavras[tam-1],"&") == 0)){
		status = fork();
		if(!status){
			int erro = execve(listaPalavras[0],listaPalavras,NULL);
			if(erro == -1)
				printf("erro encontrado: %s\n",strerror(errno));
			printf("miniShell: %s: comando não encontrado\n",listaPalavras[0]);
			exit(0);
		}
		return status;	
	}
	else{
		/* Execução de programa com parametros foreground. */
		status = fork();
		if(!status){
			int erro = execve(listaPalavras[0],listaPalavras,NULL);
			if(erro == -1)
				printf("erro encontrado: %s\n",strerror(errno));
			printf("miniShell: %s: comando não encontrado\n",listaPalavras[0]);
			exit(0);
		}
		else{
			waitpid(status,NULL,0);
		}
		return -1;
	}
}

/* 	Essa função tem como finalidade implementar o comando cd.
	Inputs:	listapalavras: o nome do arquivo para o qual se deseja mudar
			tam: numero de argumentos passados junto com cd
	Output: retorna 1 para mostrar que o comando foi identificado pelo interpretador */
int cd(char *listaPalavras, int tam){
	char caminho[256];
	getcwd(caminho, sizeof(caminho));
	if(tam != 2){
			strcpy(caminho, "/home/");
			char *aux = NULL;
			aux = getenv("USER");
			strcat(caminho,aux);
			chdir(caminho);
			return 1;
	}
	FILE *file;
	if(file = fopen(listaPalavras,"r"))
		fclose(file);
	else{
		printf("Arquivo nao existe\n");
		return 1;
	}
	if(listaPalavras[0] != '/'){
		char aux[101];
		strcat(caminho,"/");
		strcat(caminho,listaPalavras);

		chdir(caminho);
	}
	else{
		strcat(caminho,listaPalavras);
		chdir(caminho);
	}
	return 1;
}

int pwd(){
	char dir[256];
	if(getcwd(dir,sizeof(dir)) != NULL){
		printf("%s\n",dir);
		return 1;
	}
	else{
		printf("erro encontrado: %s\n",strerror(errno));
		return 0;
	}
}

/* Essa função tem como finalidade identificar o comando. */ // A ideia é essa funcao meio que organizar a logica, ai tera uma função pra cada funcionalidade do shell.
int interComando(char *buffer){
	/* Caso seja apenas um "enter", apenas printa de novo o simbolo de prompt. */
	if(buffer[0] == '\0')
		return 1;

	int status;  // Guarda o status para auxiliar no valor de retorno da função.
	char ** listaPalavras; // Guarda as palavras presentes no buffer.
	int tam = 0; // Guarda o tamanho do vetor de strings.
	listaPalavras = divLinha(buffer,&tam);

	if(strcmp(listaPalavras[0],"pwd") == 0 && tam == 1){
		return pwd();
	}
	if(strcmp(listaPalavras[0],"jobs") == 0 && tam == 1){
		imprime(ini);
		return 1;
	}
	if(strcmp(listaPalavras[0],"fg") == 0 && tam == 1){
		
	}
	if(strcmp(listaPalavras[0],"bg") == 0 && tam == 1){
		
	}
	if(strcmp(listaPalavras[0],"cd") == 0){
		return cd(listaPalavras[1], tam);
	}
	if(strcmp(listaPalavras[0],"exit") == 0 && tam == 1){
		return 0;
	}
	if(strcmp(listaPalavras[0],"clear") == 0 && tam == 1){
		printf("\e[1;1H\e[2J"); // Limpa o terminal.
		return 1;
	}

	// Caso nenhum desses comandos ele vai interpretar como os casos de prog.	
	status = progFunc(listaPalavras, tam); // status caso tenha programa em background tera o pid do processo filho.
	if(status != -1){
		insere(&ini, status);
	}
	free(listaPalavras);
	return 1;
}



int main(){

	char buffer[255]; // Buffer para ler o comando.
	memset(buffer,0,sizeof(buffer));
	int resp;
	printf("\e[1;1H\e[2J");// Limpa o terminal antes de inicilizar o miniShell.

	do{

		printf("$ ");
		fgets(buffer,254,stdin);
		buffer[strlen(buffer)-1] = '\0';
		resp = interComando(buffer);
		memset(buffer,0,sizeof(buffer));

	}while(resp);

	return 0;
}
