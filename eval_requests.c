#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "shared.h"
#include "eval_requests.h"
/* all functions in this file must be called with rc->lock acquired */

void* alloc_mem(struct requester_cont* rc, struct requester* r, int sz, int count){
    r->mem[r->n_allocs].sz = sz;
    r->mem[r->n_allocs].count = count;
    void* ret = r->mem[r->n_allocs].ptr = malloc(sz*count);
    r->mem[r->n_allocs++].mem_id = ++rc->next_mem_id;
    pthread_mutex_unlock(&rc->lock);
    return ret;
}

_Bool eval_nalloc_request(struct requester_cont* rc,
                          struct requester* r,
                          struct nalloc_request req){
    if(!r)return 0;
    switch(req.request){
        case MEM_ALLOC:
            alloc_mem(rc, r, req.sz, req.count);
            if(write(r->sock, &rc->next_mem_id, sizeof(int)) == -1)perror("write");
            break;
        default:
            break;
    }
    return 1;
}
