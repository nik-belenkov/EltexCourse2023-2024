CC=gcc
CFLAGS=-c -Wall -Werror -Wextra

tcp_server : tcp_server.o
	$(CC) tcp_server.o -o tcp_server

tcp_server.o : tcp_server.c
	$(CC) $(CFLAGS) tcp_server.c

clean :
	rm -rf *.o
	rm tcp_server