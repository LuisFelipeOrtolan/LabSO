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
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

extern int errno; // Variavel utilizada para guardar erros ocorridos.

Celula *ini = NULL; // Ponteiro para a lista encadeada usada para o comando "jobs".

pid_t pidForground = -1; // Variavel que guarda o pid do processo em forground.
char **ultimoComando; // Variavel que armazena o ultimo comando.
int tamComando; // Variavel que armazena o tamanho do ultimo comando

/* Essa função tem a finalidade de  colocar todas as palavras presentes no buffer dentro de um vetor 
 * de string.
 * OBS: a função strtok é utilizada para devidir uma string dado um delimitador. Essa função destroi a
 * a string posteriormente, por isso foi necessario fazer uma copia do buffer.		 */
char **divLinha(char *buffer, int *tam){
	char *aux; // Variavel auxiliar para armazenar palavras do buffer.
	int cont = 0;
	int i = 0;

	char *copiaBuffer = malloc(sizeof(char) * (strlen(buffer)+1));
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
void progFunc(char **listaPalavras, int tam){
	int status;
	int i = 0;
	/* Guarda o indice do ">" e "<" encontrados. */
	int indIn,indOut,indOutAp,indOutErro,indOutErroAp;
	/* Variaveis logicas para ">" e "<". */
	int achouIn = 0;
	int achouOut = 0;
	int achouOutAp = 0;
	int achouOutErro = 0;
	int achouOutErroAp = 0;

	listaPalavras[tam] = NULL; // Necessario marcar o final do array com NULL se nao da erro Bad Adress.

	/* Procura redirecionadores de I/O "<" e ">". */
	while(i < tam){
		if(strcmp(listaPalavras[i],"<") == 0 && !achouIn){
			achouIn = 1;
			indIn = i;
		}
		if(strcmp(listaPalavras[i],">") == 0 && !achouOut){
			achouOut = 1;
			indOut = i;
		}
		if(strcmp(listaPalavras[i],">>") == 0 && !achouOut){
			achouOutAp = 1;
			indOutAp = i;
		}
		if(strcmp(listaPalavras[i],"2>") == 0 && !achouOut){
			achouOutErro = 1;
			indOutErro = i;
		}
		if(strcmp(listaPalavras[i],"2>>") == 0 && !achouOut){
			achouOutErroAp = 1;
			indOutErroAp = i;
		}
		i++;
	}


	/* Execução de programa com parametros background. */
	if((strcmp(listaPalavras[tam-1],"&") == 0)){
		status = fork();
		if(!status){
				if (achouIn) { // Caso tenha encontrado o operado "<"
				    int fd0;
				    /* Abre apenas para leitura. */
				    if ((fd0 = open(listaPalavras[indIn+1], O_RDONLY)) == -1) {
				        perror (listaPalavras[indIn+1]); // Caso tenha ocorrido um erro.
				        exit (0);
				    }
				    dup2(fd0, 0); // Faz o stdin apontar para o arquivo aberto
				    close(fd0);
				}

				if (achouOut)  // Caso tenha encontrado o operado ">"
				{
				    int fd1;
				    /* Abre para escrever, caso nao exista cria o arquivo, limpa o arquivo quandoa aberto. Modos setados para dar os privilegios ao precesso filho. */
				    if ((fd1 = open(listaPalavras[indOut+1],O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {             
				        perror(listaPalavras[indOut+1]); // Caso tenha ocorrido um erro.
				        exit(0);
				    }
				    dup2(fd1,1); // Faz o stdout apontar para o arquivo aberto
				    close(fd1);
				}

				if (achouOutAp)  // Caso tenha encontrado o operado ">>"
				{
				    int fd1;
				    /* Abre para escrever, caso nao exista cria o arquivo, append o que for escrito. Modos setados para dar os privilegios ao precesso filho. */
				    if ((fd1 = open(listaPalavras[indOutAp+1],O_WRONLY | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {             
				        perror(listaPalavras[indOutAp+1]); // Caso tenha ocorrido um erro.
				        exit(0);
				    }
				    dup2(fd1,1); // Faz o stdout apontar para o arquivo aberto
				    close(fd1);
				}

				if (achouOutErro)  // Caso tenha encontrado o operado "2>"
				{
				    int fd1;
				    /* Abre para escrever, caso nao exista cria o arquivo, limpa o arquivo quandoa aberto. Modos setados para dar os privilegios ao precesso filho. */
				    if ((fd1 = open(listaPalavras[indOutErro+1],O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {             
				        perror(listaPalavras[indOutErro+1]); // Caso tenha ocorrido um erro.
				        exit(0);
				    }
				    dup2(fd1,2); // Faz o stdout apontar para o arquivo aberto
				    close(fd1);
				}

				if (achouOutErroAp)  // Caso tenha encontrado o operado "2>>"
				{
				    int fd1;
				    /* Abre para escrever, caso nao exista cria o arquivo, append o que for escrito. Modos setados para dar os privilegios ao precesso filho. */
				    if ((fd1 = open(listaPalavras[indOutErroAp+1],O_WRONLY | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {             
				        perror(listaPalavras[indOutErroAp+1]); // Caso tenha ocorrido um erro.
				        exit(0);
				    }
				    dup2(fd1,2); // Faz o stdout apontar para o arquivo aberto
				    close(fd1);
				}

			int erro = execve(listaPalavras[0],listaPalavras,NULL);
			if(erro == -1)
				printf("erro encontrado: %s\n",strerror(errno));
			printf("miniShell: %s: comando não encontrado\n",listaPalavras[0]);
			exit(0); // Preciso mandar uma mensagem para o processo pai antes de morrer para conseguir retornar o -1.
		}
		insere(&ini, status, listaPalavras, tam);
		Celula *processo = busca(ini,status);
		printf("[%d]",processo->chave);
		printf(" %d\n",processo->pid);
		return;	
	}
	else{
		/* Execução de programa com parametros foreground. */
		status = fork();
		if(!status){

				if (achouIn) { // Caso tenha encontrado o operado "<"
				    int fd0;
				    /* Abre apenas para leitura. */
				    if ((fd0 = open(listaPalavras[indIn+1], O_RDONLY)) == -1) { 
				        perror (listaPalavras[indIn+1]); // Caso tenha ocorrido um erro.
				        exit (0);
				    }
				    dup2(fd0, 0); // Faz o stdin apontar para o arquivo aberto
				    close(fd0);
				}

				if (achouOut) // Caso tenha encontrado o operado ">"
				{
				    int fd1;
				    /* Abre para escrever, caso nao exista cria o arquivo, limpa o arquivo quandoa aberto. Modos setados para dar os privilegios ao precesso filho. */
				    if ((fd1 = open(listaPalavras[indOut+1],O_WRONLY | O_CREAT | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) { 
				        perror (listaPalavras[indOut+1]); // Caso tenha ocorrido um erro.
				        exit (0);
				    }
				    dup2(fd1, 1); // Faz o stdout apontar para o arquivo aberto
				    close(fd1);
				}

				if (achouOutAp)  // Caso tenha encontrado o operado ">>"
				{
				    int fd1;
				    /* Abre para escrever, caso nao exista cria o arquivo, append o que for escrito. Modos setados para dar os privilegios ao precesso filho. */
				    if ((fd1 = open(listaPalavras[indOutAp+1],O_WRONLY | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {             
				        perror(listaPalavras[indOutAp+1]); // Caso tenha ocorrido um erro.
				        exit(0);
				    }
				    dup2(fd1,1); // Faz o stdout apontar para o arquivo aberto
				    close(fd1);
				}

				if (achouOutErro)  // Caso tenha encontrado o operado "2>"
				{
				    int fd1;
				    /* Abre para escrever, caso nao exista cria o arquivo, limpa o arquivo quandoa aberto. Modos setados para dar os privilegios ao precesso filho. */
				    if ((fd1 = open(listaPalavras[indOutErro+1],O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {             
				        perror(listaPalavras[indOutErro+1]); // Caso tenha ocorrido um erro.
				        exit(0);
				    }
				    dup2(fd1,2); // Faz o stdout apontar para o arquivo aberto
				    close(fd1);
				}

				if (achouOutErroAp)  // Caso tenha encontrado o operado "2>>"
				{
				    int fd1;
				    /* Abre para escrever, caso nao exista cria o arquivo, append o que for escrito. Modos setados para dar os privilegios ao precesso filho. */
				    if ((fd1 = open(listaPalavras[indOutErroAp+1],O_WRONLY | O_CREAT | O_APPEND , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {             
				        perror(listaPalavras[indOutErroAp+1]); // Caso tenha ocorrido um erro.
				        exit(0);
				    }
				    dup2(fd1,2); // Faz o stdout apontar para o arquivo aberto
				    close(fd1);
				}

			int erro = execve(listaPalavras[0],listaPalavras,NULL);
			if(erro == -1)
				printf("erro encontrado: %s\n",strerror(errno));
			printf("miniShell: %s: comando não encontrado\n",listaPalavras[0]);
			exit(0);
		}
		else{
			pidForground = status;
			waitpid(status,NULL,WUNTRACED);
		}
		return;
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

/* Essa funcao simula o comando pwd, printando o diretório atual na tela. Tem como entrada 
 * um inteiro para sinalizar a forma que deve ser impresso na tela (com ou sem '\n'), para
 * ser utilizado junto do sinal de prompt (caso != 1).*/
int pwd(int opcao){
	char dir[256];
	if(opcao == 1){
		if(getcwd(dir,sizeof(dir)) != NULL){ // Encontra o diretório atual e o guarda dentor de dir.
			printf("%s\n",dir);
			return 1;
		}
		else{
			printf("erro encontrado: %s\n",strerror(errno)); // Caso erro seja encontrado.
			return 0;
		}
	}
	else{
		if(getcwd(dir,sizeof(dir)) != NULL){
			printf("%s",dir);
			return 1;
		}
		else{
			printf("erro encontrado: %s\n",strerror(errno));
			return 0;
		}
	}
}
/*Verifica se os processos em segundo plano já acabaram*/
void processoFinalizado(){
	Celula *p = ini;
	int stat = 1;
	while(p != NULL){
		stat = waitpid(p->pid, NULL, WNOHANG);
		if(stat != 0){
			strcpy(p->estado, "Concluido");
			printf("[%d] %s %s\n", p->chave, p->estado, p->comando);
			ini = retira(ini, p->pid);
		}
		p = p->prox;
	}
}

/* Rotina de tratamento para o SIGCHLD, tem como entrada o numero do signal. Essa rotina era checar, por
 * processos filhos zombies e era devidamente termina-los. */
void sigchld_rotina(int signum){
	int status; // status de retorno do processo filho ao terminar.
	pid_t pid;
	for(int i = 0; i < 20; i++) // for de 0 a 20, para caso um processo filho termine dentro da rotina de tratamento do SIGCHLD.
		pid = waitpid(-1,&status,WNOHANG); // WNOHANG checa por zombies.
}

/* Rotina de tratamento para o SIGINT, tem como entrada o numero do signal. Essa rotina termina o processo em forground. */
void sigint_rotina(int signum){
	if(pidForground != -1){
		int error = kill(pidForground, SIGKILL); // Envia o sinal SIGKILL para o processo em forground.
		if(error != 0) // Caso encontrou um erro.
			printf("erro encontrado ao matar processo (%d): %s\n",pidForground, strerror(errno));
		printf("\n");
	}
	else{
		printf("\n");
	}
}

void sigtstp_rotina(int signum){
	if(pidForground != -1){
		Celula *p = NULL;
		int error = kill(pidForground,SIGTSTP);
		if(error != 0){
			printf("erro encontrado ao matar processo (%d): %s\n", pidForground, strerror(errno));
			return;
		}
		insere(&ini, pidForground, ultimoComando, tamComando);
		p = busca(ini,pidForground);
		strcpy(p->estado, "Parado ");
		printf("\n[%d] %s %s\n", p->chave, p->estado, p->comando); 
	}
}


/* Essa função tem como finalidade identificar o comando. */ // A ideia é essa funcao meio que organizar a logica, ai tera uma função pra cada funcionalidade do shell.
int interComando(char *buffer){
	processoFinalizado();

	/* Caso seja apenas um "enter", apenas printa de novo o simbolo de prompt. */
	if(buffer[0] == '\0')
		return 1;

	int status;  // Guarda o status para auxiliar no valor de retorno da função.
	char ** listaPalavras; // Guarda as palavras presentes no buffer.
	int tam = 0; // Guarda o tamanho do vetor de strings.
	listaPalavras = divLinha(buffer,&tam);
	ultimoComando = listaPalavras;
	tamComando = tam;
	if(strcmp(listaPalavras[0],"pwd") == 0 && tam == 1){
		return pwd(1);
	}
	if(strcmp(listaPalavras[0],"jobs") == 0 && tam == 1){
		imprime(ini);
		return 1;
	}
	if(strcmp(listaPalavras[0],"fg") == 0 && tam == 2){ // Na verdade nao armazenamos os pids dos processos, só o comando e o estado deles, o fg e bg pegam por indice. ou vazio
		pid_t pid;                                      // utilizando o + como o ultimo colocado na lista e o que sera chamado e o - como o penultimo.
		int pos = atoi(listaPalavras[1]);
		Celula *p = selecao(ini,pos);
		pidForground = p->pid;
		waitpid(p->pid,NULL,WUNTRACED);
		return 1;
		
	}
	if(strcmp(listaPalavras[0],"bg") == 0 && tam == 2){
		Celula *p;
		int pos = atoi(listaPalavras[1]);
		p = selecao(ini, pos);
		strcpy(p->estado, "Executando ");
		kill(p->pid, SIGCONT);
		return 1;
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
	progFunc(listaPalavras, tam);
	pidForground = -1; // Atualiza o pid do processo em forground como invalido.

	// if(status != -1){ // Foi transferido para a função profFunc.
	// 	insere(&ini, status);
	// }

	free(listaPalavras);
	return 1;
}



int main(){

	char buffer[255]; // Buffer para ler o comando.
	memset(buffer,0,sizeof(buffer));
	int resp;
	signal(SIGTSTP,sigtstp_rotina); // Tratar SIGTSTP. esse aqui que vai ser usado para parar o processo em forground...
	signal(SIGINT,sigint_rotina); // Tratar SIGINT. (ctrl+c)
	signal(SIGCHLD,sigchld_rotina); // Tratar SIGCHLD.
	printf("\e[1;1H\e[2J");// Limpa o terminal antes de inicilizar o miniShell.

	do{

		printf("[miniShell]:");
		pwd(0);
		printf("$ ");
		fgets(buffer,254,stdin);
		buffer[strlen(buffer)-1] = '\0';
		resp = interComando(buffer);
		memset(buffer,0,sizeof(buffer));
	}while(resp);

	return 0;
}
