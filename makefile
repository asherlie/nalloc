CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wpedantic -pthread -g

all: nalloc
nhost: nhost.c shared.h
nalloc: nalloc.c shared.h

.PHONY:
clean:
	rm -rf nhost nalloc *.o
