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

#include "bibliotecaMallocFree.h"
extern void *base = NULL;

int main(){
	
	printf("Tamanho dos metadados de um bloco: %ld\n\n",sizeof(struct s_block));

	printf("Criando um Vetor de inteiros com 20 posicoes com Malloc: \n");
	int *vetor = newMalloc(sizeof(int) * 20);
	for(int i = 0; i < 20; i++){
		vetor[i] = i;
		printf("Vetor[%d]: %d\n", i, i);
	}

	printf("\n\nCriando um segundo Vetor de inteiros com 10 posicoes com Malloc: \n");
	int *vetor1 = newMalloc(sizeof(int) * 10);
	for (int i = 0; i < 10; i++){
		vetor1[i] = i*5;
		printf("Vetor2[%d]: %d\n", i, vetor1[i]);
	}

	printf("\n\nDetalhando os blocos alocados pelo Malloc:\n");
	
	t_block aux = base;
	
	int bloco = 1;
	
	while(aux){
		printf("\nBloco %d: \n", bloco);
		printf("\nValor do ponteiro: [%p]\n",aux);
		printf("Tamanho dos dados: [%d]\n",aux->size);
		printf("Ponteiro para o próximo bloco: [%p]\n",aux->next);
		printf("Ponteiro para o  bloco anterior: [%p]\n",aux->prev);
		printf("Ponteiro para os dados: [%p]\n",aux->ptr);
		printf("Livre: [%d]\n",aux->free);
		aux = aux->next;
		bloco++;
	}

	printf("\n\nLiberando o primeiro vetor...\n");

	newFree(vetor);

	aux = base;
	bloco = 1;
	
	printf("\n\nDetalhando os blocos alocados pelo Malloc:\n");
	while(aux){
		printf("\nBloco %d: \n", bloco);
		printf("\nValor do ponteiro: [%p]\n",aux);
		printf("Tamanho dos dados: [%d]\n",aux->size);
		printf("Ponteiro para o próximo bloco: [%p]\n",aux->next);
		printf("Ponteiro para o  bloco anterior: [%p]\n",aux->prev);
		printf("Ponteiro para os dados: [%p]\n",aux->ptr);
		printf("Livre: [%d]\n",aux->free);
		aux = aux->next;
		bloco++;
	}

	printf("\nCriando um Vetor de inteiros com 5 posicoes com Malloc: \n");
	int *vetor3;

	vetor3 = newMalloc(sizeof(int) * 5);
	for(int i = 0; i < 5; i++){
		vetor3[i] = i;
		printf("Vetor3[%d] : %d\n",i, i);
	}
	aux = base;
	bloco = 1;

	printf("\n\nDetalhando os blocos alocados pelo Malloc:\n");
	while(aux){
		printf("\nBloco %d: \n", bloco);
		printf("\nValor do ponteiro: [%p]\n",aux);
		printf("Tamanho dos dados: [%d]\n",aux->size);
		printf("Ponteiro para o próximo bloco: [%p]\n",aux->next);
		printf("Ponteiro para o  bloco anterior: [%p]\n",aux->prev);
		printf("Ponteiro para os dados: [%p]\n",aux->ptr);
		printf("Livre: [%d]\n",aux->free);
		aux = aux->next;
		bloco++;
	}

	printf("\n\nObserve que o algoritmo segue o First Fit, colocando o novo bloco na primeira posição disponível.\n");
	printf("Para isso, foi usada uma funcao de separar blocos para quebrar o bloco maior em um do tamanho do novo bloco e outro com o tamanho restante. \n");

	printf("\nLiberando o terceiro vetor...\n");
	newFree(vetor3);

	aux = base;
	bloco = 1;

	printf("\n\nDetalhando os blocos alocados pelo Malloc:\n");
	while(aux){
		printf("\nBloco %d: \n", bloco);
		printf("\nValor do ponteiro: [%p]\n",aux);
		printf("Tamanho dos dados: [%d]\n",aux->size);
		printf("Ponteiro para o próximo bloco: [%p]\n",aux->next);
		printf("Ponteiro para o  bloco anterior: [%p]\n",aux->prev);
		printf("Ponteiro para os dados: [%p]\n",aux->ptr);
		printf("Livre: [%d]\n",aux->free);
		aux = aux->next;
		bloco++;
	}

	printf("\n\nObserve que o Bloco 1 foi fundido com o antigo Bloco 2, totalizando em um novo bloco com a soma dos tamanhos originais.\n");
	printf("Para isso, foi usada uma função que funde blocos livres adjacentes.\n\n");

	return 0;
}