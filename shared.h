#ifndef _SHARED_H
#define _SHARED_H
#define NALLOC_PORT 190

/* for now we're using a single request type for all messages */
/*
 * we can prob get rid of count - this can all be done in bytes
 * and the abstraction can occur on the nalloc() side
 */
struct nalloc_request{
    enum req_type{MEM_ALLOC, FREE_MEM, READ_MEM, WRITE_MEM}request;
    int sz;// count;

    int mem_id;

    /* used for reads and writes */
    int index;
};
#endif
