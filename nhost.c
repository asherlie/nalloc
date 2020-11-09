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
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
    struct requester peers[100];
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

void* alloc_mem(struct requester_cont* rc, struct sockaddr_in addr, int sz, int count){
    struct requester* r;
    pthread_mutex_lock(&rc->lock);
    if(!(r = find_requester(rc, addr))){
        r = add_requester(rc, addr);
    }
    r->mem[r->n_allocs].sz = sz;
    r->mem[r->n_allocs].count = count;
    void* ret = r->mem[r->n_allocs++].ptr = malloc(sz*count);
    pthread_mutex_unlock(&rc->lock);
    return ret;
}

/*
 * wait for connections
 * add their address to our struct
 */
void* accept_conn_th(void* null){
/* void accept_conn_t(){ */
    (void)null;
    struct requester_cont rc;
    init_rc(&rc);
    int lsock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0), sock;
    {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = NALLOC_PORT;
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(lsock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1){
        perror("BIND");
        return NULL;
    }
    if(listen(lsock, 0) == -1){
        perror("LISTEN");
        return NULL;
    }
    }

    struct sockaddr addr;
    struct sockaddr_in* saddr;

    socklen_t addrlen;
    struct nalloc_request request;
    while(1){
        if((sock = accept(lsock, &addr, &addrlen) != -1)){
            puts("accepted conn");
            if(read(sock, &request, sizeof(struct nalloc_request)) ==
               sizeof(struct nalloc_request)){
                printf("got request for %i bytes\n", request.sz*request.count);
                saddr = (struct sockaddr_in*)&addr;
                alloc_mem(&rc, *saddr, request.sz, request.count);
            }
            close(sock);
        }
    }
    /* return NULL; */
}

int main(){
    accept_conn_th(NULL);
    /* alloc_mem(NULL, ); */
}
