CC=gcc

CFLAGS=-pthread

all: STR-1

STR-1: STR-1.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f STR-1
