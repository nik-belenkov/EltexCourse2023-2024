CC=gcc
CFLAGS=-c -Wall -Werror -Wextra

all: clean tcp_server tcp_client

tcp_server : tcp_server.o
	$(CC) tcp_server.o -o tcp_server

tcp_server.o : tcp_server.c
	$(CC) $(CFLAGS) tcp_server.c

tcp_client : tcp_client.o
	$(CC) tcp_client.o -o tcp_client

tcp_client.o : tcp_client.c
	$(CC) $(CFLAGS) tcp_client.c

clean :
	rm -rf *.o
	rm -rf tcp_server tcp_client