CC=gcc
CFLAGS=-c -Wall -Werror -Wextra

my_cat : my_cat.o
	$(CC) my_cat.o -o my_cat

my_cat.o : my_cat.c my_cat.h
	$(CC) $(CFLAGS) my_cat.c

clean :
	rm -rf *.o
	rm my_cat