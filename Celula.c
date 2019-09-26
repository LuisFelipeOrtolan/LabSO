#include <stdio.h>
#include <stdlib.h>
#include "Celula.h"

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
	while(p != NULL){
		printf("%d\n", p->pid);
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

void retira(Celula **p){
	Celula *morta;
	morta = *p;
	*p = morta->prox;
	free(morta);
}