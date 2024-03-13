CC=gcc

CFLAGS=-pthread

all: Teste1

Teste1: Teste1.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f Teste1
