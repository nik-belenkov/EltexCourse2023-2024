CC=gcc
CFLAGS=-c -Wall -Werror -Wextra

eltex_task2 : eltex_task2.o
	$(CC) eltex_task2.o -o eltex_task2

eltex_task2.o : eltex_task2.c eltex_task2.h
	$(CC) $(CFLAGS) eltex_task2.c

clean :
	rm -rf *.o
	rm eltex_task2