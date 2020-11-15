/*
 * TODO: enable writing to disk also
 *           requesting a FILE* on another machine
 *           fopen() abstraction
 */
/*
 * maybe an init function that puts some structs
 * in the global space
 * 
 * or just have that all handled by the daemon
 * 
 * our function will simply connect()
 * send a request
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "shared.h"

/* client */
struct nmem{
    int count, entry_sz, mem_id;
};

int _establish_connection(struct in_addr addr){
    struct sockaddr_in saddr;
    saddr.sin_addr.s_addr = addr.s_addr;
    saddr.sin_port = NALLOC_PORT;
    saddr.sin_family = AF_INET;
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    {
    struct sockaddr_in myaddr;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = NALLOC_PORT;
    myaddr.sin_addr.s_addr = INADDR_ANY;

    int tr = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)) == -1)
        perror("setsockopt()");

    if(0 && bind(sock, (struct sockaddr*)&myaddr, sizeof(struct sockaddr_in)) == -1){
        perror("BIND");
        return -1;
    }

    }
    if(connect(sock, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in)))return -1;
    return sock;
}

int establish_connection(char* ip){
    struct in_addr ia;
    inet_pton(AF_INET, ip, &ia);
    return _establish_connection(ia);
}

/* sock must be a socket connection to 
 * a nalloc host
 */
struct nmem nalloc(int sz, int entries, int sock){
    struct nmem ret;
    struct nalloc_request req;
    req.request = MEM_ALLOC;
    req.sz = sz*entries;

    /* mem_id isn't used for nalloc() */
    req.mem_id = -1;

    /* req.count = entries; */
    printf("wrote %li bytes\n", write(sock, &req, sizeof(struct nalloc_request)));
    ret.entry_sz = sz;
    ret.count = entries;
    ret.mem_id = -1;
    /* TODO: make this non blocking */
    read(sock, &ret.mem_id, sizeof(int));
    printf("assigned mem_id: %i\n", ret.mem_id);
    return ret;
}

#if !1
TODO: mem_id should be assigned on a per requester basis
they have different

we need nfree()

we need the following functionality which can all be abstractions of
write_mem() and read_mem():
    nind(type, struct nmem, int index);
        this works like the [] operator and will be #defined
        can i convert void* to float inlined?
        i can have a function that will return a (char/void)*
        #define nind(type, nmem, index) read_nm(nmem, sizeof(type)*index, *((type*)((sizeof(type)*index)+sizeof(type))))

    void nemcpy(struct nmem src, struct nmem dest, int nbytes);
    void lnemcpy(void* src, struct nmem dest, int nbytes);
    void nemlcpy(struct nmem src, void* dest, int nbytes);
    void nemmove(struct nmem src, struct nmem dest, int nbytes);
    _Bool nemcmp(struct nmem a, struct nmem b);
#endif

void client(){
}

/* client end */

int main(int a, char** b){
    if(a < 2)return 0;
    /* int ash = establish_connection("192.168.0.1"); */
    int ash = establish_connection(b[1]);
    /*
     * char buf[] = "hello";
     * write(ash, buf, 6);
     */
    nalloc(20, sizeof(int), ash);
    return EXIT_SUCCESS;    
}
