#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Celula.h"

int nroProc = 1;

/*Função responsável por reorganizar os números que correspondem a posição
na fila dos processos em segundo plano ou parados depois que um deles foi removido
Inputs: -
Output: uma lista com valores reorganizados*/
void rearranjaChaves(Celula *lst, int chave){
	Celula *p = lst;
	while(p != NULL){
		if(p->chave > chave)
			p->chave = p->chave-1;
		p = p->prox;
	}
}
/*Função responsável por inserir um processo na lista dos processos em
segundo plano. 
Inputs: Endereço do primeiro nó, pid do processo a ser
colocado na lista, um ponteiro de ponteiro de char com os comandos e 
o número de palavras no comando do processo. 
Output: uma nova lista com a nova célula inserida*/
void insere(Celula **lst, pid_t pid, char **listaPalavras, int tam){
	Celula *nova;
	nova = malloc(sizeof(Celula));
	nova->pid = pid;
	strcpy(nova->estado, "Executando ");
	int i = 0;
	strcpy(nova->comando, listaPalavras[i++]);
	strcat(nova->comando, " ");
	while(i < tam){
		strcat(nova->comando, listaPalavras[i++]);
		strcat(nova->comando, " "); 
	}
	nova->comando[strlen(nova->comando) - 1] = '\0';
	nova->chave = nroProc++;
	nova->prox = NULL;
	if(*lst == NULL)
		*lst = nova;
	else{
		Celula *p = *lst;
		while(p->prox != NULL){
			p = p->prox;
		}
		p->prox = nova;
	}
}
/*Função responsável por imprimir todos os processos em
segundo plano. 
Inputs: Endereço do primeiro nó da lista
Output: Todos as células da lista impressas no terminal*/
void imprime(Celula *lst){
	Celula *p = lst;
	int i = 0;
	while(p != NULL){
		printf("[%d]", p->chave);
		if(p->chave == nroProc - 1)
			printf("+ ");
		if(p->chave == nroProc - 2)
			printf("- ");
		printf("%s", p->estado );
		printf("%s \n", p->comando);
		p = p->prox;
	}
}
/*Função responsável por buscar um elemento na lista pelo PID
Input: Primeira Celula da lista onde se deseja buscar
Output: A célula buscada ou NULL se não foi encontrada */
Celula *busca(Celula *lst, pid_t pid){
	Celula *p = lst;
	while(p != NULL && p->pid != pid)
		p = p->prox;
	return p;
}
/*Função responsável por escolher uma célula através da chave associada ao processo
Input: Primeira Celula da lista onde se deseja buscar
Output: A célula escolhida ou NULL se ela não existe */
Celula *selecao(Celula *lst, int pos){
	Celula *p = lst;
	while(p != NULL && p->chave < pos){
		p = p->prox;
	}
	return p;
}
/*Função esponsável por retirar uma célula da lista de processos em segundo plano
Input: O PID da célula que se deseja remover e a primeira célula da lista
Output: A lista original sem a célula removida ou a mesma lista caso a célula não exista*/
Celula * retira(Celula *lst, pid_t pid){
	Celula *p, *q;
	p = lst;
	q = lst->prox;
	int chave;
	if(p->pid == pid){
		nroProc--;
		chave = p->chave;
		free(p);
		rearranjaChaves(lst, chave);
		return q;
	}
	while(q != NULL && q->pid != pid){
		p = q;
		q = q->prox;
	}
	if(q != NULL){
		nroProc--;
		p->prox = q->prox;
		chave = q->chave;
		free(q);
		rearranjaChaves(lst, chave);
		return lst;
	}
	return lst;
}
