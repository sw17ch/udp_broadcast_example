#ifndef SHARED_H
#define SHARED_H

#include <netdb.h>

/*
 * setup_socket
 *    create a socket suitable for transmission or reception
 */
int setup_socket(void);

/*
 * get_in_addr
 *    Extract the proper bytes of an address from a sockaddr
 *
 * This is a helper function defined by Beej from in "Beej's Guide to Network
 * Programming". http://beej.us/guide/bgnet/
 */
void * get_in_addr(struct sockaddr *sa);

#endif /* SHARED_H */
