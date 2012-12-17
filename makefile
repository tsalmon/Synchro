CC=gcc
CFLAGS=-Wall -ansi -pedantic
LDFLAGS=
synchro: main.o sync.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o synchro main.o sync.o
	rm *.o

main.o: main.c
	$(CC) $(CFLAGS) $(LDFLAGS)  main.c -c

sync.o: sync.c
	$(CC) $(CFLAGS) $(LDFLAGS) sync.c -c

