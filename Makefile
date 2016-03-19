CC=gcc
CFLAGS=-std=c11 -lncurses -D_GNU_SOURCE

all: shuodu

shuodu: shuodu.c grid.c
	$(CC) -o shuodu shuodu.c $(CFLAGS) 

clean:
	rm shuodu
