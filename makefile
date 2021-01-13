CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wpedantic -pthread -O3

H_OBJ=rc.o eval_requests.o
H_SRC=nhost.c

C_SRC=nalloc.c

BIN_H=nhost
BIN_C=nalloc

all: nhost nalloc

rc.o: rc.c rc.h
eval_requests.o: eval_requests.c eval_requests.h

nhost: nhost.c shared.h rc.o eval_requests.o
	$(CC) $(CFLAGS) $(H_SRC) $(H_OBJ) -o $(BIN_H)
nalloc: nalloc.c shared.h
	$(CC) $(CFLAGS) $(C_SRC) -o $(BIN_C)

.PHONY:
clean:
	rm -rf *.o nhost nalloc
