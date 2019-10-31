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
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 40

typedef struct s_block *t_block;
#define align4(x) ( ( x - 1 ) / 4 * 4 + 4 )

struct s_block {
	size_t size;
	t_block next;
	t_block prev;
	void *ptr;
	int free;
	char data[1];
};

void *base = NULL;

/*	Função que vai dividir um bloco.
	Input: Um bloco e o tamanho que um dos blocos deve assumir.
	Output: Dois blocos, um com tamanho s e outro com o tamanho do original - s. */
void separaBloco(t_block bloco, size_t tam){
	t_block novo; //Cria um novo bloco.
	novo = (t_block)bloco->data + tam; 
	novo->size = bloco->size - tam - BLOCK_SIZE; //Tamanho do novo bloco é tamanho do bloco original, menos o novo tamanho, menos o tamanho dos metadados.
	novo->next = bloco->next; //Insere novo bloco a direita do original.
	novo->prev = bloco;
	novo->free = 1; //Marca o novo bloco como disponível.
	novo->ptr = novo->data;
	bloco->size = tam;
	bloco->next = novo;
	if(novo->next != NULL) //Se não era o fim do bloco,
		novo->next->prev = novo; //Novo bloco passa a apontar pro próximo do bloco antigo.
}

/*	Função que iaumenta o tamanho do heap quando está em sua capacidade máxima.
	Input: O último bloco e o seu tamanho.
	Ouput: Um bloco e o tamanho do heap aumentado. */
t_block cresceHeap(t_block last, size_t tam){
	t_block bloco;
	
	bloco = sbrk(0);

	if(sbrk(BLOCK_SIZE + tam) == (void*)-1) //Se não for possível aumentar o tamanho do heap,
		return NULL; //Retorne NULL.

	bloco->size = tam; 
	bloco->next = NULL;
	bloco->prev = last;
	bloco->ptr = bloco->data;
	if(last != NULL) //Se o último bloco for diferente de NULL,
		last->next = bloco; //O novo bloco vira o último.
	bloco->free = 0; 
	return bloco;
}

/*	Função que acha um bloco.
	Input: Ponteiro de um bloco e o seu tamanho.
	Output: O bloco buscado. */
t_block achaBloco(t_block *last, size_t tam){
	t_block bloco = base; //Bloco recebe o início da lista.
	while(bloco != NULL && !(bloco->free && bloco->size >= tam)){ //Enquanto não for o bloco procurado,
		*last = bloco;
		bloco = bloco->next; //Avança na lista.
	}
	return bloco;
}

/*	Função que executa o malloc.
	Input: Um tamanho para ser alocado.
	Ouput: Um ponteiro para a posição alocada. */
void *newMalloc(size_t size){
	t_block bloco, last;
	size_t tam;
	//tam = size * 4;
	tam = align4(size); 
	if(base){ // Se existem blocos alocados na lista
		last = base; 
		bloco = achaBloco(&last,tam); //Acha um espaço disponível.
		if(bloco){ //Se havia espaço disponível,
			if((bloco->size - tam)>= (BLOCK_SIZE + 4))
				separaBloco(bloco,tam); //Quebra um bloco e aloca o novo tamanho.
			bloco->free = 0; //Marca o bloco como ocupado.
		}
		else{ //Se não havia espaço disponível,
			bloco = cresceHeap(last,tam); //Aumenta o tamanho do heap e aloca novo espaço.
			if(!bloco) //Se não foi possível aumentar o tamanho do heap,
				return NULL; //Retorna NULL
		}
	}
	else{ //Se não existem nada alocado,
		bloco = cresceHeap(NULL,tam); //Aumenta o tamanho do heap e aloca novo espaço.
		if(!bloco) //Se não foi possível aumentar o tamanho do heap,
			return NULL; //Retorna NULL
		base = bloco; // Se foi bem sucedido, bloco vira a base da lista.
	}
	return bloco->data; // Retorna o ponteiro pro bloco.
}

/*	Função que funde blocos adjacentes que estão livres.
	Input: Um bloco.
	Output: Um bloco fundido se o próximo bloco estava livre ou só o bloco passado se não estava livre. */
t_block fundeBlocos(t_block bloco){
	if(bloco->next != NULL) //Se existe um próximo bloco e ele está livre,
		if(bloco->next->free == 1){
			bloco->size = bloco-> size + BLOCK_SIZE + bloco->next->size; //Funde os blocos.
			bloco->next = bloco->next->next;	
			if(bloco->next) //Se existia um bloco depois do bloco fundido, 
				bloco->next->prev = bloco; //Atualiza o ponteiro de anterior.
		}

	return bloco; //Retorna o bloco original ou fundido.
}

/*	Função que pega um bloco.
	Input: Um ponteiro.
	Output: O bloco apontado pelo ponteiro. */
t_block getBlock(void *p){
	t_block baseAux = base;
	while(baseAux){
		if(baseAux->ptr == p)// Caso seja igual, entao p corresponde a um espaco alocado pelo newMalloc.	 
			return baseAux;
		baseAux = baseAux->next;
	}
	return baseAux;
}

/* 	Função que retorna se um endereço de ponteiro é válido ou não.
	Input: Um ponteiro.
	Output: 1 se é válido, 0 caso contrário. */
int enderecoValido(void *p){
	if(base)
		if(p > base && p < sbrk(0)) //Se está estre o primeiro item da lista e a última posição válida da lista.
			return (p == (getBlock(p))->ptr);
	return 0;
}

/*	Implementação da função free.
	Input: Um ponteiro.
	Output: O conteúdo do ponteiro é desalocado. */ 
void newFree(void *ptr){
	t_block bloco;
	if(enderecoValido(ptr)){
		bloco = getBlock(ptr);
		bloco->free = 1;
		if(bloco->prev)
			if(bloco->prev->free)
				bloco = fundeBlocos(bloco->prev);

		if(bloco->next){
			fundeBlocos(bloco);
		}
		else{
			if(bloco->prev){
				bloco->prev->next = NULL;
			}
			else{
				// base = NULL;
				return;
			}
			brk(bloco);
		}

	}
}

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