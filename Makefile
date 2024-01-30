CC=gcc
CFLAGS=-c -Wall -Werror -Wextra

all : sys1 sys2

sys1 : sys1.o
	$(CC) sys1.o -o sys1

sys1.o : sys1.c
	$(CC) $(CFLAGS) sys1.c

sys2 : sys2.o
	$(CC) sys2.o -o sys2

sys2.o : sys2.c
	$(CC) $(CFLAGS) sys2.c

clean :
	rm -rf *.o
	rm sys1
	rm sys2