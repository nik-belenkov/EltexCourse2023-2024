#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

void delete_shm(int *shmid, key_t *key, char pathname[]);

int main() {
  int shmid1, shmid2; /* IPC дескриптор для области разделяемой памяти */
  char pathname1[] = "sys1.c", pathname2[] = "sys2.c";
  key_t key1, key2;
  delete_shm(&shmid1, &key1, pathname1);
  delete_shm(&shmid2, &key2, pathname2);

  return 0;
}

void delete_shm(int *shmid, key_t *key, char pathname[]) {
  if ((*key = ftok(pathname, 0)) < 0) {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if ((*shmid = shmget(*key, 3 * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL)) <
      0) {
    /* В случае ошибки пытаемся определить: возникла ли она
    из-за того, что сегмент разделяемой памяти уже существует
    или по другой причине */
    if (errno != EEXIST) {
      /* Если по другой причине – прекращаем работу */
      printf("Can\'t create shared memory\n");
      exit(-1);
    } else {
      /* Если из-за того, что разделяемая память уже
      существует, то пытаемся получить ее IPC
      дескриптор и, в случае удачи, сбрасываем флаг
      необходимости инициализации элементов массива */

      if ((*shmid = shmget(*key, 3 * sizeof(int), 0)) < 0) {
        printf("Can\'t find shared memory\n");
        exit(-1);
      }
    }
  }

  if (shmctl(*shmid, IPC_RMID, NULL) == 0) {
    printf("Shered memory was free\n");
  }
}