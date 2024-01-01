#include "my_cat.h"

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    read_from_file(argv);
  } else {
    write_to_file();
  }
  return 0;
}

void read_from_file(char *argv[]) {
  int fd = open(argv[1], O_RDONLY, PERMS);
  char buf[BUFSIZ];
  size_t size;
  while ((size = read(fd, buf, BUFSIZ)) > 0) {
    if (write(0, &buf, size) != (int)size) {
      printf("ERROR");
    }
  }
  close(fd);
}

void write_to_file() {
  char buf[BUFSIZ];
  int n;
  while ((n = read(0, buf, BUFSIZ)) > 0) {
    write(1, buf, n);
  }
}