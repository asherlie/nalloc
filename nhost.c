/* two ideas - persistent connection - once it's closed
 *   memory is freed
 *   client needs to establish connection and keep it open
 *   (no authentication required)?
 * 
 *   no persistent connections - based on IP address and UID
 *   THIS 
 */
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "shared.h"

/* stores connecters by ip and port */
struct requester_cont{
};

void* accept_conn(void* x){
}
