CC=gcc

CFLAGS=-c

OBJECTS= Celula.o

all: miniShell

miniShell: Shell.c Celula.o
	$(CC) Shell.c $(OBJECTS) -o miniShell

Celula.o: Celula.c
	$(CC) Celula.c $(CFLAGS)

.PHONY: clean
clean:
	-rm -f $(OBJECTS) miniShell
