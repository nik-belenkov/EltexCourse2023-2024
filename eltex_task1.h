#ifndef ELTEX_TASK1_H
#define ELTEX_TASK1_H

#include <stdio.h>
#include <string.h>

#define PATH "./passwd"
#define SIZE 256

void scan_log(char *login);
void read_file(char *login);
void search_name(char *login, char *buff);

#endif