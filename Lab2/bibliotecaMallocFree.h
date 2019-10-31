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

#define BLOCK_SIZE sizeof(struct s_block)
#define align4(x) ( ( x - 1 ) / 4 * 4 + 4 )

typedef struct s_block *t_block;

struct s_block {
	size_t size;
	t_block next;
	t_block prev;
	void *ptr;
	int free;
	char data[1];
};



extern void *base;

void separaBloco(t_block bloco, size_t tam);
t_block cresceHeap(t_block last, size_t tam);
t_block achaBloco(t_block *last, size_t tam);
void *newMalloc(size_t size);
t_block fundeBlocos(t_block bloco);
t_block getBlock(void *p);
int enderecoValido(void *p);
void newFree(void *ptr);