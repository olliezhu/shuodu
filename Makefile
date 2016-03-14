CC=gcc
CFLAGS=-lncurses -std=c11

all: shuodu

shuodu: shuodu.c grid.c
	$(CC) -o shuodu shuodu.c $(CFLAGS) 

clean:
	rm shuodu
