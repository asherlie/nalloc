CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wpedantic -pthread

all: nalloc
nhost.o: nhost.c
nalloc: nalloc.c

.PHONY:
clean:
	rm -rf nalloc *.o
