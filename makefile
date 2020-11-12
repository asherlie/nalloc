CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wpedantic -pthread -g

all: nhost nalloc

rc.o: rc.c rc.h
eval_requests.o: eval_requests.c eval_requests.h

nhost: nhost.c shared.h rc.o eval_requests.o
nalloc: nalloc.c shared.h

.PHONY:
clean:
	rm -rf *.o nhost nalloc
