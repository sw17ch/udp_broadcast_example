#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <errno.h>

#include "shared.h"

/* Bind to the specified host and port. Host can be a broadcast address. */
int bind_socket(
    char * host, /* "192.168.1.255" */
    char * port  /* "5000"          */
    );

/* Given a 'listening socket' and a 'local socket', loop and respond to
 * messages. */
int listen_loop(
    int listen_sck, /* The socket we wait for detection messages on. */
    int local_sck); /* The socket we respond to detection messages on. */

int main(int argc, char * argv[]) {
  if (argc < 2) {
    fprintf(stderr, "usage:\n\t%s [bind address] [bind port]\n", argv[0]);
    return -1;
  }

  char * bind_addr = argv[1];
  char * bind_port = argv[2];

  int listen_sck;
  int local_sck;

  if (0 > (local_sck = setup_socket())) {
    fprintf(stderr, "Failed to setup local socket: %d\n", local_sck);
    return -1;
  }

  if (0 > (listen_sck = bind_socket(bind_addr, bind_port))) {
    fprintf(stderr, "Failed to bind to %s:%s. Error: %d\n", bind_addr, bind_port, listen_sck);
    return -1;
  }

  return listen_loop(listen_sck, local_sck);
}

int bind_socket(char * host, char * port) {
  struct addrinfo * info;
  struct addrinfo hints = {
    .ai_family = AF_INET,
    .ai_socktype = SOCK_DGRAM
  };
  int sck;
  int err;

  if (0 != (err = getaddrinfo(host, port, &hints, &info))) {
    return -1;
  }

  struct addrinfo * p;
  for (p = info; p != NULL; p = p->ai_next) {
    if (0 > (sck = socket(p->ai_family, p->ai_socktype, p->ai_protocol))) {
      continue;
    }

    if (0 > bind(sck, p->ai_addr, p->ai_addrlen)) {
      close(sck);
      continue;
    }

    break;
  }

  if (NULL == p) {
    return -2;
  }

  freeaddrinfo(info);

  int bcast = 1;
  if (0 > (err == setsockopt(sck, SOL_SOCKET, SO_BROADCAST, &bcast, sizeof(bcast)))) {
    return -3;
  }

  return sck;
}

int listen_loop(int listen_sck, int local_sck) {
  uint8_t reply[] = "hello";
  uint8_t buffer[128] = {0};
  struct sockaddr_storage from;
  socklen_t from_len;

  while (1) {
    from_len = sizeof(from);
    int len = recvfrom(listen_sck, (void*)buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &from_len);

    if (0 < len) {
      printf("Recevied message '%s'\n", buffer);

      len = sendto(local_sck, (void*)reply, sizeof(reply), 0, (struct sockaddr *)&from, from_len);
      if (errno) {
        fprintf(stderr, "Unable to 'sendto': %s\n", strerror(errno));
        return -1;
      }

      printf("Replied with %d bytes.\n", len);
    }
  }

  return 0;
}
