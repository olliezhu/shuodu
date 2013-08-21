CC=gcc
CFLAGS=-lncurses

all: sudoku

sudoku: shuodu.c
	$(CC) -o sudoku shuodu.c $(CFLAGS) 

clean:
	rm sudoku
