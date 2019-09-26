#include <stdio.h>
#include <stdlib.h>
#include "Celula.h"

Celula *lst = NULL;

void insere(Celula **lst, pid_t pid){
	Celula *nova;
	nova = malloc(sizeof(Celula));
	nova->pid = pid;
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
	int i = 1;
	while(p != NULL){
		printf("[%d] %d\n", i, p->pid);
		i++;
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
	int q = 0;
	while(p != NULL && q < pos){
		p = p->prox;
		q++;
	}
	return p;
}

void retira(Celula *lst, pid_t pid){
	Celula *p, *q;
	p = lst;
	q = lst->prox;
	while(q != NULL && q->pid != pid){
		p = q;
		q = q->prox;
	}
	if(q != NULL){
		p->prox = q->prox;
		free(q);
	}
}