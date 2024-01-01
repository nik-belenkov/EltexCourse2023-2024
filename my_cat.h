#ifndef MY_CAT_H
#define MY_CAT_H

#include <fcntl.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#define PERMS 0666

void read_from_file(char *argv[]);
void write_to_file();

#endif