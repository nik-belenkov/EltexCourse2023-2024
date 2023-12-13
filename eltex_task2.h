#ifndef ELTEX_TASK2_H
#define ELTEX_TASK2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 20
#define TRUE 1
#define FALSE 0


struct student {
    char surname[SIZE];
    char gradebook[SIZE];
    char faculty[SIZE];
    char group[SIZE];
};

void process();
void print_menu();
int stop_process(char command);
void input_table_size(int *size);
void input_one_record(int ind, struct student *students);
void input_table(int size, struct student *students);
void save_to_file(int size, struct student *students);
int select_field();
void out_one_line(int ind, struct student *students);
void output(int size, struct student *students);
void sort(int size, struct student *students);
int surname_cmp(const void *a, const void *b);
int gradebook_cmp(const void *a, const void *b);
int faculty_cmp(const void *a, const void *b);
int group_cmp(const void *a, const void *b);
int search(int size, struct student *students);
struct student *delete_record(int size, int ind, struct student *students);
void change(int ind, struct student *students);
void search_and_change(int size, struct student *students);

#endif