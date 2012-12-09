CC=gcc
CFLAGS=-Wall -ansi -pedantic

synchro: main.o sync.o
	$(CC) $(CFLAGS) -o synchro main.o sync.o

main.o: main.c
	$(CC) $(CFLAGS) main.c -c

sync.o: sync.c
	$(CC) $(CFLAGS) sync.c -c
