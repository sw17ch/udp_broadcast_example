CC=clang

all: client server

client: client.c shared.h shared.c
	clang client.c shared.c -Wall -Wextra -Werror -o client

server: server.c shared.h shared.c
	clang server.c shared.c -Wall -Wextra -Werror -o server

clean:
	rm -rf client server
