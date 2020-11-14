#ifndef _SHARED_H
#define _SHARED_H
#define NALLOC_PORT 190

/* for now we're using a single request type for all messages */
struct nalloc_request{
    enum req_type{MEM_ALLOC, FREE_MEM, READ_MEM, WRITE_MEM}request;
    int sz, count;

    /* used for reads and writes */
    int index;
};
#endif
