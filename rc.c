#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
/* used to print ip address of connected peers */
#include <arpa/inet.h>

#include "rc.h"

void init_rc(struct requester_cont* rc){
    pthread_mutex_init(&rc->lock, NULL);
    rc->n_conn = 0;
    rc->next_mem_id = -1;
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
        /* fprintf(stderr, "%i == %i && %i == %i\n", rc->peers[i].addr.sin_addr.s_addr, addr.sin_addr.s_addr, rc->peers[i].addr.sin_port, addr.sin_port); */
        if(rc->peers[i].addr.sin_addr.s_addr == addr.sin_addr.s_addr/* &&
           rc->peers[i].addr.sin_port == addr.sin_port*/)return rc->peers+i;
    }
    return NULL;
}

struct shared_mem* find_mem_chunk(struct requester* r, int mem_id){
    if(r->n_allocs <= mem_id)return NULL;
    return r->mem+mem_id;
}

void rc_dump(struct requester_cont* rc){
    pthread_mutex_lock(&rc->lock);
    char buf[INET_ADDRSTRLEN+1];
    for(int i = 0; i < rc->n_conn; ++i){
        /* fprintf(stderr, "%i:\n", rc->peers[i].addr.sin_addr.s_addr); */
        fprintf(stderr, "%s:\n", 
                                 inet_ntop(AF_INET, &rc->peers[i].addr.sin_addr,
                                           buf, INET_ADDRSTRLEN));
        for(int j = 0; j < rc->peers[i].n_allocs; ++j){
            fprintf(stderr, "  %i) {%i*%i bytes}\n", 
                    rc->peers[i].mem[j].mem_id,
                    rc->peers[i].mem[j].sz,
                    rc->peers[i].mem[j].count);
        }
    }
    pthread_mutex_unlock(&rc->lock);
}
