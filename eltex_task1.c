#include "eltex_task1.h"

int main() {
  char login[64];
  scan_log(login);
  read_file(login);
  return 0;
}

void scan_log(char *login) {
  printf("Enter login: ");
  scanf("%s", login);
}

void read_file(char *login) {
  FILE *f;
  char buff[SIZE];
  f = fopen(PATH, "r");
  if (f) {
    while (fgets(buff, SIZE, f) != NULL) {
      search_name(login, buff);
    }
  }
  fclose(f);
}

void search_name(char *login, char *buff) {
  int cnt = 0;
  if (strstr(buff, login) != NULL) {
    for (int i = 0; buff[i] != '\n'; i++) {
      if (buff[i] == ':') {
        cnt++;
        continue;
      } else if (cnt == 4 && buff[i] != ',') {
        printf("%c", buff[i]);
      }
    }
    printf("\n");
  }
}