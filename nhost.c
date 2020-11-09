/*
 * TODO: use 1000 as a hard limit - don't dynamically alloc
 * handle this safely
 */
/* two ideas - persistent connection - once it's closed
 *   memory is freed
 *   client needs to establish connection and keep it open
 *   (no authentication required)?
 * 
 *   no persistent connections - based on IP address and UID
 *   THIS 
 */
#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "shared.h"

struct shared_mem{
    int sz, count;
    char* ptr;
};

/* TODO: mem should be a linked list of struct shared_mem */
struct requester{
    struct sockaddr_in addr;
    int n_allocs;
    struct shared_mem mem[1000];
    /* char* mem[1000]; */
};

/* stores connecters by ip and port */
struct requester_cont{
    pthread_mutex_t lock;
    /* struct sockaddr_in addr[1000]; */
    /* struct shared_mem peers[1000]; */
    struct requester peers[1000];
    int n_conn;
};

void init_rc(struct requester_cont* rc){
    pthread_mutex_init(&rc->lock, NULL);
}

void* rc_eval_th(void* rc_v){
    struct requester_cont* rc = rc_v;
    while(1){
        pthread_mutex_lock(&rc->lock);
        pthread_mutex_unlock(&rc->lock);
    }
}

/*
 * actually just have a thread to handle the struct
 * spawns a thread to evaulate commands from a peer - maybe not
 * adds new requester to requester_cont
 */
struct requester* add_requester(struct requester_cont* rc, struct sockaddr_in addr){
    /* pthread_mutex_lock(&rc->lock); */
    /* rc->addr[rc->n_conn++] = addr; */
    rc->peers[rc->n_conn].n_allocs = 0;
    rc->peers[rc->n_conn++].addr = addr;
    return rc->peers + rc->n_conn-1;
    /* pthread_mutex_unlock(&rc->lock); */
}

struct requester* find_requester(struct requester_cont* rc, struct sockaddr_in addr){
    for(int i = 0; i < rc->n_conn; ++i){
        if(rc->peers[i].addr.sin_addr.s_addr == addr.sin_addr.s_addr &&
           rc->peers[i].addr.sin_port == addr.sin_port)return rc->peers+i;
    }
    return NULL;
}

void alloc_mem(struct requester_cont* rc, struct sockaddr_in addr, int sz, int count){
    struct requester* r;
    pthread_mutex_lock(&rc->lock);
    if(!(r = find_requester(rc, addr))){
        r = add_requester(rc, addr);
    }
    r->mem[r->n_allocs].sz = sz;
    r->mem[r->n_allocs].count = count;
    r->mem[r->n_allocs++].ptr = malloc(sz*count);
    pthread_mutex_unlock(&rc->lock);
}

/*
 * wait for connections
 * add their address to our struct
 */
void* accept_conn_th(void* null){
    (void)null;
    int lsock = socket(AF_INET, SOCK_STREAM, 0), sock;
    struct sockaddr addr;
    socklen_t addrlen;
    while(1){
        if((sock = accept(lsock, &addr, &addrlen) != -1)){
        }
    }
}
