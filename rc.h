#include <pthread.h>
#include <netinet/in.h>

/* this file defines structs for the host to manage requesters */
struct shared_mem{
    int sz, count, mem_id;
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
    int n_conn, next_mem_id;
};

void init_rc(struct requester_cont* rc);
void* rc_eval_th(void* rc_v);
struct requester* add_requester(struct requester_cont* rc, struct sockaddr_in addr);
struct requester* find_requester(struct requester_cont* rc, struct sockaddr_in addr);
void rc_dump(struct requester_cont* rc);
