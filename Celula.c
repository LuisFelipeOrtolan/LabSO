#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Celula.h"

Celula *lst = NULL;
int nroProc = 1;

void rearranjaChaves(){
	Celula *p = lst;
	while(p != NULL){
		if(p->chave > nroProc)
			p->chave--;
	}
}

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
	printf("\n");
}

Celula *busca(Celula *lst, pid_t pid){
	Celula *p = lst;
	while(p != NULL && p->pid != pid)
		p = p->prox;
	return p;
}

Celula *selecao(Celula *lst, int pos){
	Celula *p = lst;
	while(p != NULL && p->chave < pos){
		p = p->prox;
	}
	return p;
}

Celula * retira(Celula *lst, pid_t pid){
	Celula *p, *q;
	p = lst;
	q = lst->prox;
	if(p->pid == pid){
		nroProc--;
		free(p);
		rearranjaChaves();
		return q;
	}
	while(q != NULL && q->pid != pid){
		p = q;
		q = q->prox;
	}
	if(q != NULL){
		nroProc--;
		printf("hello (%d)\n", nroProc);
		p->prox = q->prox;
		free(q);
		rearranjaChaves();
		return lst;
	}
	return lst;
}
