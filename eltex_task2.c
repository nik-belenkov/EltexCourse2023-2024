#include "eltex_task2.h"

int main() {
  process();
  return 0;
}

void process() {
  int play = TRUE, table_size = 1, ind = 0;
  int command;
  struct student *students = malloc(table_size * sizeof *students);
  sprintf(students[0].surname, "Empty");
  sprintf(students[0].gradebook, "Empty");
  sprintf(students[0].faculty, "Empty");
  sprintf(students[0].group, "Empty");
  print_menu();
  while (play) {
    if (scanf("%d", &command)) {
      if (command == 1) {
        input_table_size(&table_size);
        students = malloc(table_size * sizeof *students);
        input_table(table_size, students);
      }
      if (command == 2) {
        save_to_file(table_size, students);
      }
      if (command == 3) {
        output(table_size, students);
      }
      if (command == 4) {
        sort(table_size, students);
      }
      if (command == 5) {
        ind = search(table_size, students);
        out_one_line(ind, students);
      }
      if (command == 6) {
        printf("Enter index to delete\n");
        scanf("%d", &ind);
        students = delete_record(table_size, ind, students);
        table_size--;
      }
      if (command == 7) {
        printf("Enter index to change\n");
        scanf("%d", &ind);
        change(ind, students);
      }
      if (command == 8) {
        search_and_change(table_size, students);
      }
      play = stop_process(command);
      print_menu();
    } else {
      break;
    }
  }
  free(students);
}

void print_menu() {
  printf("Push 1 to input\n");
  printf("Push 2 to save to file\n");
  printf("Push 3 to output\n");
  printf("Push 4 to sort\n");
  printf("Push 5 to search\n");
  printf("Push 6 to delete\n");
  printf("Push 7 to change\n");
  printf("Push 8 to search and change\n");
  printf("Push 0 to quit\n");
}

int stop_process(char command) {
  int condition = TRUE;
  if (command < 1 || command > 8) condition = FALSE;
  return condition;
}

void input_table_size(int *size) {
  printf("Input table size: ");
  scanf("%d", size);
}

void input_one_record(int ind, struct student *students) {
  printf("Input surname:\n");
  scanf("%s", students[ind].surname);
  printf("Input number:\n");
  scanf("%s", students[ind].gradebook);
  printf("Input faculty:\n");
  scanf("%s", students[ind].faculty);
  printf("Input group:\n");
  scanf("%s", students[ind].group);
}

void input_table(int size, struct student *students) {
  for (int i = 0; i < size; i++) {
    input_one_record(i, students);
  }
}

void save_to_file(int size, struct student *students) {
  FILE *f;
  if ((f = fopen("./table.txt", "w"))) {
    fprintf(f, "Number : Surname Gradebook Faculty Group\n");
    for (int i = 0; i < size; i++) {
      fprintf(f, "%d : %s %s %s %s\n", i + 1, students[i].surname,
              students[i].gradebook, students[i].faculty, students[i].group);
    }
    fclose(f);
  }
}

void out_one_line(int ind, struct student *students) {
  printf("%7s | %9s | %7s | %-40s\n", students[ind].surname,
         students[ind].gradebook, students[ind].faculty, students[ind].group);
}

void output(int size, struct student *students) {
  printf("INDEX: SURNAME | GRADEBOOK | FACULTY | GROUP\n");
  for (int i = 0; i < size; i++) {
    printf("%5d: ", i);
    out_one_line(i, students);
  }
  printf("\n");
}

int select_field() {
  int mode = 0;
  printf("Press 1 by surname\n");
  printf("Press 2 by gradebook\n");
  printf("Press 3 by faculty\n");
  printf("Press 4 by group\n");
  scanf("%d", &mode);
  return mode;
}

void sort(int size, struct student *students) {
  printf("Select sort field\n");
  int mode = select_field();
  if (mode == 1) qsort(students, size, sizeof *students, surname_cmp);
  if (mode == 2) qsort(students, size, sizeof *students, gradebook_cmp);
  if (mode == 3) qsort(students, size, sizeof *students, faculty_cmp);
  if (mode == 4) qsort(students, size, sizeof *students, group_cmp);
}

int surname_cmp(const void *a, const void *b) {
  return strcmp(((struct student *)a)->surname, ((struct student *)b)->surname);
}

int gradebook_cmp(const void *a, const void *b) {
  return strcmp(((struct student *)a)->gradebook,
                ((struct student *)b)->gradebook);
}

int faculty_cmp(const void *a, const void *b) {
  return strcmp(((struct student *)a)->faculty, ((struct student *)b)->faculty);
}

int group_cmp(const void *a, const void *b) {
  return strcmp(((struct student *)a)->group, ((struct student *)b)->group);
}

int search(int size, struct student *students) {
  int ind = 0;
  char pattern[SIZE];
  printf("Select search field\n");
  int mode = select_field();
  printf("Enter pattern\n");
  scanf("%s", pattern);
  for (int i = 0; i < size; i++) {
    int match = 0;
    if (mode == 1 && !(match = strcmp(students[i].surname, pattern))) {
      ind = i;
    } else if (mode == 2 && !(match = strcmp(students[i].gradebook, pattern))) {
      ind = i;
    } else if (mode == 3 && !(match = strcmp(students[i].faculty, pattern))) {
      ind = i;
    } else if (mode == 4 && !(match = strcmp(students[i].group, pattern))) {
      ind = i;
    }
  }
  return ind;
}

struct student *delete_record(int size, int ind, struct student *students) {
  int n = size - 1;
  struct student *new_students = malloc(n * sizeof *new_students);
  for (int i = 0, j = 0; i < n; j++) {
    if (j != ind) {
      new_students[i] = students[j];
      i++;
    }
  }
  return new_students;
}

void change(int ind, struct student *students) {
  input_one_record(ind, students);
}

void search_and_change(int size, struct student *students) {
  int ind = search(size, students);
  change(ind, students);
}