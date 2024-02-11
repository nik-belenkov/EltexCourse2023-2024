CC=gcc
CFLAGS=-c -Wall -Werror -Wextra

my_handler : my_handler.o
	$(CC) my_handler.o -o my_handler

my_handler.o : my_handler.c
	$(CC) $(CFLAGS) my_handler.c

clean :
	rm -rf *.o
	rm my_handler