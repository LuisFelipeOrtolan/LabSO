#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Celula.h"

int nroProc = 1;

void rearranjaChaves(Celula *lst, int chave){
	Celula *p = lst;
	while(p != NULL){
		if(p->chave > chave)
			p->chave = p->chave-1;
		p = p->prox;
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
