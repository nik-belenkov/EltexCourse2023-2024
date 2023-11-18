CC=gcc
CFLAGS=-c -Wall -Werror -Wextra

eltex_task1 : eltex_task1.o
	$(CC) eltex_task1.o -o eltex_task1

eltex_task1.o : eltex_task1.c eltex_task1.h
	$(CC) $(CFLAGS) eltex_task1.c

clean :
	rm -rf *.o
	rm eltex_task1