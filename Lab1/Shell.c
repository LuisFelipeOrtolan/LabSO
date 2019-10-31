/*///////////////////////////////////////////////////////
// Trabalho 2 - Criação de um programa shell           //
//                                                     //
// Autores:                                            //
// Nome: Roseval Donisete Malaquias Junior             //
// RA: 758597                                          //
// Turma: A                                            //
//                                                     //
// Nome: Luís Felipe Corrêa Ortolan                    //
// RA: 759375                                          //
// Turma: A                                            //
//                                                     //
// O principal objetivo do trabalho é entender como o  //
// shell funciona internamente. Dessa forma, utilizar  //
// as informações adquiridas para implementar versão   //
// simplificada de um programa shell.                  //
///////////////////////////////////////////////////////*/

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
#include <stdio.h>
#include <stdlib.h>

extern char **environ; // Variavel externa que possui as variaveis de ambiente.

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

/* Essa função é uma versão alterada do divLinha, utilizada para "cortar" a variavel path, para que 
 * seja retornado um vetor de strings contendo todas as paths presentes. Tem como entrada um ponteiro
 * para char representado o buffer onde vai estar a variavel de ambiente "$PATH" e o tamanho do vetor
 * de strings gerados.*/ 
char **divPath(char *buffer, int *tam){
	char *aux; // Variavel auxiliar para armazenar palavras do buffer.
	int cont = 0;
	int i = 0;

	char *copiaBuffer = malloc(sizeof(char) * (strlen(buffer)+1));
	strcpy(copiaBuffer,buffer); // Copia o buffer para outra variavel para utilizar a funcao strtok

	/* Conta a quantidade de palavras dentro do buffer. */
	aux = strtok(buffer,":");
	while(aux != NULL){
		cont++;
		aux = strtok(NULL,":");
	}

	/* Instancia o vetor de strings com o tamanho referente a quantidade de palavras. */
	char **listaParametros = malloc(sizeof(char *) * (cont+1) );

	/* Encontra todas as palavras e coloca dentro do vetor de strings. */
	aux = strtok(copiaBuffer,":");
	listaParametros[i] = malloc( (strlen(aux)+1) * sizeof(char));
	listaParametros[i] = aux;

	for(i = 1; i < cont; i ++){
		aux = strtok(NULL,":");
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
			signal(SIGINT,SIG_DFL); // Define como default e tratamento desses sinais para o processo filho.
			signal(SIGTSTP,SIG_DFL);
			setpgrp(); // Seta o group id do processo filho, como seu proprio pid.
			int erro = execve(listaPalavras[0],listaPalavras,environ);
			
			listaPalavras[tam-1] = NULL; // Retira o campo que contém "&" para passar como parametro.
			/* Caso o arquivo nao esteja no diretório atual, procurar no PATH. */
			if(erro == -1){
				char *aux = getenv("PATH"); // Guarda o PATH dentro de uma variavel auxiliar.
				int tam;
				char **listaPath = divPath(aux,&tam); // Gerado um vetor de strings com cada path.
				int i = 0;
				/* Tenta inicializar o programa, em todas as pathas disponiveis, caso nao consiga comando não encontrado. */
				while(i < tam){
					char paths[300];
					strcpy(paths,listaPath[i]);
					strcat(paths,"/");
					strcat(paths,listaPalavras[0]);
					execve(paths,listaPalavras,environ);
					i++;
				}
			}
			printf("miniShell: %s: comando não encontrado\n",listaPalavras[0]);
			exit(0); // Preciso mandar uma mensagem para o processo pai antes de morrer para conseguir retornar o -1.
		}
		insere(&ini, status, listaPalavras, tam); // Insere na lista de processos e printa seu indice e pid.
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
			signal(SIGINT,SIG_DFL); // Define como default e tratamento desses sinais para o processo filho.
			signal(SIGTSTP,SIG_DFL);
			setpgrp(); // Seta o group id do processo filho, como seu proprio pid.
			int erro = execve(listaPalavras[0],listaPalavras,environ);
			/* Caso o arquivo nao esteja no diretório atual, procurar no PATH. */
			if(erro == -1){
				char *aux = getenv("PATH"); // Guarda o PATH dentro de uma variavel auxiliar.
				int tam;
				char **listaPath = divPath(aux,&tam); // Gerado um vetor de strings com cada path.
				int i = 0;
				/* Tenta inicializar o programa, em todas as pathas disponiveis, caso nao consiga comando não encontrado. */
				while(i < tam){
					char paths[300];
					strcpy(paths,listaPath[i]);
					strcat(paths,"/");
					strcat(paths,listaPalavras[0]);
					execve(paths,listaPalavras,environ);
					i++;
				}
			}
			printf("miniShell: %s: comando não encontrado\n",listaPalavras[0]);
			exit(0);
		}
		else{
			/* Processo pia precisa designar o controlador do terminal para o processo filho. */
			pidForground = status; // Define o pid do processo em forground.
			int erro = tcsetpgrp(STDIN_FILENO,pidForground); // Designa o controlador do terminal.
			if(erro != 0)
				printf("erro encontrado: %s\n",strerror(errno));
			waitpid(pidForground,&status,WUNTRACED); // Wait no processo.

			/* Caso o processo tenha recebido um sinal de SIGSTSP. */
			if(WIFSTOPPED(status)){ // Coloca na fila de processos com o estado parado.
				Celula *p = NULL;

				p = busca(ini,pidForground);
				if(p == NULL){
					insere(&ini, pidForground, ultimoComando, tamComando);
					p = busca(ini,pidForground);
				}
				strcpy(p->estado, "Parado ");
				printf("\n[%d] %s %s\n", p->chave, p->estado, p->comando); 
			}

		return;
			}
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
	if((file = fopen(listaPalavras,"r")))
		fclose(file);
	else{
		printf("miniShell: cd: Arquivo ou diretório inexistente\n");
		return 1;
	}
	if(listaPalavras[0] != '/'){
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

/*Essa função tem como finalidade a cada sinal de prompt, retirar processos ja concluidos da lista de 
 * processos. */
void processoFinalizado(){
	Celula *p = ini; // Apontador para o inicio da lista.
	int stat = 1;
	while(p != NULL){
		stat = waitpid(p->pid, NULL, WNOHANG); // Termina processos zombies.
		if(stat != 0){ // Processo encontrado, altera seu estado para Concluido, printa na tela depois o retira da lista.
			strcpy(p->estado, "Concluido");
			printf("[%d] %s %s\n", p->chave, p->estado, p->comando);
			ini = retira(ini, p->pid);
		}
		p = p->prox;
	}
}

/* Rotina de tratamento para o SIGCHLD, tem como entrada o numero do signal. Essa rotina era checar, por
 * processos filhos zombies e era devidamente termina-los. Como os tratamentos de background e forground
 * estao sendo feitos fora da rotina do sigchld, essa funcao é usada apenas para terminar propriamente
 * os processos. */
void sigchld_rotina(int signum){
	while(waitpid(-1,0,WNOHANG) > 0); // Enquanto tiver processos zombies o retorno é maior que zero.
}

/* Essa funcao tem como finalidade imitar o comando fg para colocar processos em forground, tem como 
 * entrada a lista de palavras contendo o comando e seu atributo e o tamanho dessa lista. */
int fg(char **listaPalavras,int tam){ 
	int status; // Guarda o valor de retorno do waitpid.
	int pos = 1; // Guarda a posicao do processo na lista.
	Celula *p; // Ponteiro para a lista.

	if(tam == 1){ // Caso tamanho seja 1, apenas o comando fg foi dado, aplicar a funcao no ultimo elemento da lista.
		Celula *p = ini;
		if(p != NULL)
			while(p->prox != NULL){
				p = p->prox;
				pos++;
			}
	}
	else
		pos = atoi(listaPalavras[1]);

	p = selecao(ini,pos);


	if(p == NULL){ // Caso p seja NULL processo nao existe.
		printf("miniShell: fg: o trabalho %d não existe\n",pos);
		return 1;
	}

	pidForground = p->pid; // Altera o pid do processo em forground.
	int erro = tcsetpgrp(STDIN_FILENO,pidForground); // Designa o controlador do terminal para o processo em forground.
	if(erro != 0)
		printf("erro encontrado: %s\n",strerror(errno));
	waitpid(pidForground,&status,WUNTRACED); // Aguarda o processo.

	if(WIFSTOPPED(status)){ // Caso saiu do waitpid com sinal de parada, alterar o estado do processo e colocar na lista.
		Celula *p = NULL;

		p = busca(ini,pidForground);
		if(p == NULL){
			insere(&ini, pidForground, ultimoComando, tamComando);
			p = busca(ini,pidForground);
		}
		strcpy(p->estado, "Parado ");
		printf("\n[%d] %s %s\n", p->chave, p->estado, p->comando); 
	}
	return 1;
}

/* Essa funcao tem como finalidade imitar o comando bg para colocar processos em background, tem como 
 * entrada a lista de palavras contendo o comando e seu atributo e o tamanho dessa lista. */
int bg(char ** listaPalavras, int tam){ 
	int pos = 1; // Guarda a posicao do processo na lista.
	Celula *p; // Ponteiro para a lista.

	if(tam == 1){ // Caso tamanho seja 1, apenas o comando bg foi dado, aplicar a funcao no ultimo elemento da lista.
		Celula *p = ini;
		if(p != NULL)
			while(p->prox != NULL){
				p = p->prox;
				pos++;
			}
	}
	else
		pos = atoi(listaPalavras[1]);


	p = selecao(ini,pos);
	
	if(p == NULL){ // Caso esteja nesse estado, comando bg desconsiderado.
		printf("miniShell: bg: o trabalho %d já está em plano de fundo\n",pos);
		return 1;
	}

	if(strcmp(p->estado,"Executando ")!= 0){ // Caso ele nao esteja em estado executando.
		strcpy(p->estado, "Executando ");
		kill(p->pid, SIGCONT);
	}

	return 1;
}

/* Essa função tem como finalidade funcionar como o interpretador de comandos passado pelo usuario para o shell, tem como entrada o
 * próprio buffer de entrada e como saida um inteiro sinalizando se o shell deve ser interrompido ou nao. Essa função identifica o 
 * comando e chama a rotina apropriada para tratar desse comando.  */
int interComando(char *buffer){
	processoFinalizado();

	/* Caso seja apenas um "enter", apenas printa de novo o simbolo de prompt. */
	if(buffer[0] == '\0')
		return 1;

	char ** listaPalavras; // Guarda as palavras presentes no buffer.
	int tam = 0; // Guarda o tamanho do vetor de strings.
	listaPalavras = divLinha(buffer,&tam); // Utiliza o buffer para criar um vetor de strings.
	ultimoComando = listaPalavras;
	tamComando = tam;

	if(strcmp(listaPalavras[0],"pwd") == 0 && tam == 1){ // Comando pwd.
		return pwd(1);
	}
	if(strcmp(listaPalavras[0],"jobs") == 0 && tam == 1){ // Comando jobs.
		imprime(ini);
		return 1;
	}
	if(strcmp(listaPalavras[0],"fg") == 0 && (tam == 1 || tam == 2)){ // Comando fg.
		return fg(listaPalavras,tam);
	}
	if(strcmp(listaPalavras[0],"bg") == 0 && (tam == 1 || tam == 2)){ // Comando bg.
		return bg(listaPalavras,tam);
	}
	if(strcmp(listaPalavras[0],"cd") == 0){ // Comando cd.
		return cd(listaPalavras[1], tam);
	}
	if(strcmp(listaPalavras[0],"exit") == 0 && tam == 1){ // Comando exit.
		return 0;
	}
	if(strcmp(listaPalavras[0],"clear") == 0 && tam == 1){ // Comando clear.
		printf("\e[1;1H\e[2J"); // Limpa o terminal.
		return 1;
	}

	// Caso nenhum desses comandos ele vai interpretar como os casos de prog.	
	progFunc(listaPalavras, tam);
	pidForground = -1; // Atualiza o pid do processo em forground como invalido.

	free(listaPalavras);
	return 1;
}


/* Função main, onde sera recebido a entrada do usuario e printado o sinal de prompt na tela. */
int main(){

	char buffer[255]; // Buffer para ler o comando.
	memset(buffer,0,sizeof(buffer)); // Seta todas as posicoes com valor 0.
	int resp; // Guarda o retorno do interComando.
	int pidShell = getpid(); // Guarda o pid do miniShell.

	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP,SIG_IGN); // Tratar SIGTSTP. esse aqui que vai ser usado para parar o processo em forground...
	signal(SIGINT,SIG_IGN); // Tratar SIGINT. (ctrl+c)
	signal(SIGCHLD,sigchld_rotina); // Tratar SIGCHLD.
	printf("\e[1;1H\e[2J");// Limpa o terminal antes de inicilizar o miniShell.

	do{
		printf("\033[0;31m"); // Colocar a cor do output do terminal como verde.
		printf("[miniShell]:");
		printf("\033[0;34m"); // Colocar a cor do output do terminal como azul.
		pwd(0); // Printa o diretório atual.
		printf("\033[0;31m"); // Colocar a cor do output do terminal como verde.
		printf("$ ");
		printf("\033[0m"); // Reseta a cor.
		fgets(buffer,254,stdin); // Recebe a entrada em buffer.
		buffer[strlen(buffer)-1] = '\0';

		resp = interComando(buffer); // Interpreta a entrada
		memset(buffer,0,sizeof(buffer)); // Reseta o buffer.

		int erro = tcsetpgrp(STDIN_FILENO,pidShell); // Seta o processo em forground para o controlador como o miniShell.
			if(erro != 0)
		printf("erro encontrado: %s\n",strerror(errno));

	}while(resp);

	return 0;
}
