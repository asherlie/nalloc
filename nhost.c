/*
 * TODO: make this a daemon that can be enabled/disabled and
 *       started/stopped from systemd!
 */
/*
 * TODO: allow an admin address that has a special set of commands
 *       these commands will be able to update nalloc() size limitations
 *       and max number of requests per IP
 */

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
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "shared.h"
#include "eval_requests.h"
/* #include "rc.h" */

int SOCK;

/*
 * wait for connections
 * add their address to our struct
 */
void* accept_conn_th(void* null){
/* void accept_conn_t(){ */
    (void)null;
    struct requester_cont rc;
    init_rc(&rc);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    int lsock = socket(AF_INET, SOCK_STREAM, 0), sock = -1;
    SOCK = lsock;
    {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = NALLOC_PORT;
    addr.sin_addr.s_addr = INADDR_ANY;

    int tr = 1;
    if(setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)) == -1){
        perror("setsockopt()");
        return NULL;
    }

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
    memset(&addr, 0, sizeof(struct sockaddr));

    struct sockaddr_in* saddr;

    socklen_t addrlen = sizeof(struct sockaddr_in);
    struct nalloc_request request;
    /*
     * close(STDIN_FILENO);
     * close(STDOUT_FILENO);
     */
    perror("I");
    while(1){
        if((sock = accept(lsock, &addr, &addrlen) != -1)){
            /* fputs("accepted conn", stderr); */
            /* fprintf(stderr, "sock: %i %i %i %i\n", sock, lsock, STDOUT_FILENO, STDIN_FILENO); */
            /* struct sockaddr_in ff = *((struct sockaddr_in*)&addr); */
            /*
             * fprintf(stderr, "addr %i len %i sin: %li\n",
             *                 ff.sin_addr.s_addr, addrlen, sizeof(struct sockaddr_in));
             */
            
            /* FILE* fp = fdopen(sock, "r"); */
            #if 0
            char c[10] = {0};
            /* fprintf(stderr, "read %li\n", read(sock, c, 10)); */
            /* perror("read"); */
            fprintf(stderr, "%s\n", c);
            /* while((c = getc(fp)) != EOF)fprintf(stderr, "[%i]", c); */
            fflush(stderr);
            #endif

            /*
             * puts("HI");
             * close(sock);
             * continue;
             */

            /*
             * need to add a request type flag
             * have to be able to use mem id and request a read or a write
             * to a segment of memory
             * write(mem, offset, len, data)
             * read(mem, offset, len)
             */

            _Bool success;
            int nbb;
            if((success = ((nbb = read(sock, &request, sizeof(struct nalloc_request))) ==
               sizeof(struct nalloc_request)))){
                /* fprintf(stderr, "got request for %i bytes\n", request.sz); */
                saddr = (struct sockaddr_in*)&addr;
                /* should we add_requester here if needed and have eval_nalloc_request() 
                 * take in a struct requester
                 * if so, we'll need to check if request is MEM_ALLOC
                 * this will let us avoid passing address to anything in eval_requests.c
                 * would be nice to keep abstraction
                 */
                /* if we're allocating new memory we need to ensure
                 * that the struct requester* being passed to eval_nalloc_request()
                 * is valid
                 */
                pthread_mutex_lock(&rc.lock);
                struct requester* r = find_requester(&rc, *saddr);
                if(request.request == MEM_ALLOC && !r){
                    r = add_requester(&rc, *saddr);
                }
                if(r){
                    r->sock = sock;
                    success = eval_nalloc_request(&rc, r, request);
                }
                else success = 0;
                pthread_mutex_unlock(&rc.lock);
                /* alloc_mem(&rc, *saddr, request.sz, request.count); */
                /* if(write(sock, &rc.next_mem_id, sizeof(int)) == -1)perror("write"); */
            }
            /* if we didn't read the correct amount of bytes, eval_nalloc_request()
             * returned 0, or we were sent a command not in the request enum
             */
            if(!success){
                int x = -1;
                if(write(sock, &x, sizeof(int)) == -1)perror("write");
                fprintf(stderr, "got invalied request\n");
            }
            close(sock);
            /* rc_dump(&rc); */
        }
    }
    /* return NULL; */
}


void close_sock(){
    close(SOCK);
    fprintf(stderr, "\nsock %i closed. exiting safely.\n", SOCK);
    exit(0);
}

int main(){
    signal(SIGINT, close_sock);
    accept_conn_th(NULL);
    /* alloc_mem(NULL, ); */
}
