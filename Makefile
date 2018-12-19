CC=gcc
CFLAGS=-std=c11 -D_GNU_SOURCE -lncurses -lcurl -ltidy #-DCURSED

all: shuodu

shuodu: shuodu.c grid.o grid.h
	$(CC) -o shuodu grid.o shuodu.c $(CFLAGS)

clean:
	rm grid.o shuodu
