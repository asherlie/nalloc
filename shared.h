#define NALLOC_PORT 190

/* for now we're using a single request type for all messages */
struct nalloc_request{
    enum req_type{MEM_ALLOC, READ_MEM, WRITE_MEM}request;
    int sz, count;
};
