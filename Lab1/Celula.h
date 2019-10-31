/* biblioteca "Celula.h". Utilizada na lista de processos do miniShell. 
 * Todas informações acerca dos alunos envolvidos estão no cabeçalho do arquivo "Shell.c". */

#include <stdio.h>
#include <stdlib.h>


typedef struct celula Celula;
struct celula{
	pid_t pid;
	char estado[30];
	char comando[255];
	int chave;
	Celula *prox;
};

void insere(Celula **lst, pid_t pid, char **listaPalavras, int tam);
void imprime(Celula *lst);
Celula *busca(Celula *lst, pid_t pid);
Celula *selecao(Celula *lst, int pos);
Celula *retira(Celula *p, pid_t pid);