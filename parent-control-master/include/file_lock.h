#ifndef FILE_LOCK_H
#define FILE_LOCK_H

void file_rlock(int fd);

void file_wlock(int fd);

void file_unlock(int fd);

#endif // FILE_LOCK_H