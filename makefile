CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wpedantic -pthread

all: nalloc
nhost.o: nhost.c shared.h
nalloc: nalloc.c shared.h

.PHONY:
clean:
	rm -rf nalloc *.o
