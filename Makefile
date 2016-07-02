CC=gcc
CFLAGS=-std=c11 -lncurses -D_GNU_SOURCE -lcurl

all: shuodu

shuodu: shuodu.c grid.o grid.h
	$(CC) -o shuodu grid.o shuodu.c $(CFLAGS)

clean:
	rm grid.o shuodu
