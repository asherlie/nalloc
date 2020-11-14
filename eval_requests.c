#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "shared.h"
#include "eval_requests.h"
/* all functions in this file must be called with rc->lock acquired */

void* alloc_mem(struct requester_cont* rc, struct requester* r, int sz, int count){
    r->mem[r->n_allocs].sz = sz;
    r->mem[r->n_allocs].count = count;
    void* ret = r->mem[r->n_allocs].ptr = malloc(sz*count);
    r->mem[r->n_allocs++].mem_id = ++rc->next_mem_id;
    return ret;
}

#if !1
these two functions need to be reworked to take into account the size of each block
len should be in terms of block size
i can maybe make a #define on the nalloc() side to hide this
#define NALLOC(type, addr, nbytes) nalloc(addr, sizeof(type)*nbytes)
#endif

_Bool move_mem(struct shared_mem* mem){
    (void)mem;
    return 1;
}

/* should len be in terms of bytes or n_elements
 * for now, like memcpy(), it's done in bytes
 */
_Bool write_mem(struct shared_mem* mem, int index, int len, void* src){
    if(len*mem->sz+ index*mem->sz >= mem->sz * mem->count)return 0;
    /* memcpy(mem->ptr+index, src, len*mem->sz); */
    memcpy(mem->ptr+index, src, len);
    return 1;
}

/* read_mem() reads len bytes from index and writes it to r->sock */
_Bool read_mem(struct shared_mem* mem, struct requester* r, int index, int len){
    if(len+index >= mem->sz * mem->count)return 0;
    return write(r->sock, mem->ptr+(index), len) == len*mem->sz;
}

_Bool eval_nalloc_request(struct requester_cont* rc,
                          struct requester* r,
                          struct nalloc_request req){
    if(!r)return 0;
    _Bool ret = 1;
    /* all requests but READ_MEM will reply with just 4 bytes
     * READ_MEM responses are the same length as the requested
     * number of bytes
     */
    switch(req.request){
        case MEM_ALLOC:
            alloc_mem(rc, r, req.sz, req.count);
            if(write(r->sock, &rc->next_mem_id, sizeof(int)) == -1)perror("write");
            break;
        case WRITE_MEM:
            break;
        case READ_MEM:
            break;
        case FREE_MEM:
            break;
        /* in case the user isn't cooperating */
        default:
            ret = 0;
            break;
    }
    return ret;
}
