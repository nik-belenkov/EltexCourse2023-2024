#include "file_lock.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

//----------------------Private declarations-----------------------

static int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len);

//-----------------------Public definitions-----------------------

void file_rlock(int fd)
{
    if (lock_reg(fd, F_SETLKW, F_RDLCK, 0, 0, 0) < 0) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
}

void file_wlock(int fd)
{
    if (lock_reg(fd, F_SETLKW, F_WRLCK, 0, 0, 0) < 0) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
}

void file_unlock(int fd)
{
    if (lock_reg(fd, F_SETLK, F_UNLCK, 0, 0, 0) < 0) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
}

//----------------------Private definitions-----------------------

static int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;
    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len; 

    return(fcntl(fd, cmd, &lock));
}