#include <stdio.h>
#include <stdlib.h>

typedef struct celula Celula;
struct celula{
	pid_t pid;
	Celula *prox;
};

void insere(Celula **lst, pid_t pid);
void imprime(Celula *lst);
Celula *busca(Celula *lst, pid_t pid);
Celula *selecao(Celula *lst, int pos);
Celula *retira(Celula *p, pid_t pid);