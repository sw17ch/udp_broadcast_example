#include "shared.h"

#include <netdb.h>

int setup_socket(void) {
  int sck = socket(AF_INET, SOCK_DGRAM, 0);

  int bcast = 1;
  if (0 > setsockopt(sck, SOL_SOCKET, SO_BROADCAST, &bcast, sizeof(bcast))) {
    return -1;
  }

  return sck;
}

void * get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
