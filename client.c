#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <stdint.h>
#include <errno.h>
#include <arpa/inet.h>

#include "shared.h"

/* Given a socket, a host, and a port, configure a socket and send the message
 * "hello". Wait for a response and print it as if it was a string. */
int communicate(int sck, char * host, char * port);

int main(int argc, char * argv[]) {
  if (argc < 2) {
    fprintf(stderr, "usage:\n\t%s [remote address] [remote port]\n", argv[0]);
    return -1;
  }

  char * remote_addr = argv[1];
  char * remote_port = argv[2];

  int sck;

  if (0 > (sck = setup_socket())) {
    fprintf(stderr, "Failed to bind socket. Error: %d\n", sck);
  }

  return communicate(sck, remote_addr, remote_port);
}

int communicate(int sck, char * host, char * port) {
  uint8_t msg[] = "hello";
  uint8_t buffer[128] = {0};
  struct hostent * he;
  struct sockaddr_in remote;
  struct sockaddr_storage from;
  socklen_t from_len;

  if (NULL == (he = gethostbyname(host))) {
    return -1;
  }

  remote.sin_family = AF_INET;
  remote.sin_port = htons(atoi(port));
  remote.sin_addr = *(struct in_addr *)he->h_addr;
  memset(remote.sin_zero, 0, sizeof(remote.sin_zero));

  int len;
  len = sendto(sck, msg, sizeof(msg), 0, (struct sockaddr *)&remote, sizeof(remote));
  if (errno) {
    fprintf(stderr, "Unable to 'sendto': %s\n", strerror(errno));
    return -2;
  }
  len = recvfrom(sck, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &from_len);
  if (errno) {
    fprintf(stderr, "Unable to 'recvfrom': %s\n", strerror(errno));
    return -3;
  }

  char addr[32];

  inet_ntop(
      from.ss_family,
      get_in_addr((struct sockaddr *)&from),
      addr,
      sizeof(addr));

  printf("Message from %s:%u: %s\n",
      addr,
      ntohs((*((struct sockaddr_in *)&from)).sin_port),
      buffer);

  return 0;
}
