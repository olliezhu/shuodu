CC=gcc
CFLAGS=-lncurses

all: sudoku

sudoku: shuodu.c grid.c
	$(CC) -o sudoku shuodu.c $(CFLAGS) 

clean:
	rm sudoku
