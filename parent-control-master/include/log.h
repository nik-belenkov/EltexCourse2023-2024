#ifndef LOG_H
#define LOG_H

#include <stdio.h>

enum log_type
{
    INFO_LOG, 
    ERR_LOG,
    SYS_ERR_LOG,
};

void log_init(const char *log_path, const char *err_log_path, const char *btoken, const char *cid); 

void logger(enum log_type type, const char* file, int line, const char *fmt, ...);

void log_deinit(void);

#define LOG_INFO(...)    logger(INFO_LOG,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERR(...)     logger(ERR_LOG,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_SYS_ERR(...) logger(SYS_ERR_LOG, __FILE__, __LINE__, __VA_ARGS__)

#endif // LOG_H
