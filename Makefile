CC=clang

all: client server

client: client.c shared.h shared.c
	clang client.c shared.c -pedantic -Wall -Wextra -Werror -g -o client

server: server.c shared.h shared.c
	clang server.c shared.c -pedantic -Wall -Wextra -Werror -g -o server

clean:
	rm -rf client server
