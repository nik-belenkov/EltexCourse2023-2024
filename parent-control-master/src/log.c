#include "log.h"
#include "file_lock.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <curl/curl.h>

#define URL_MAX_LEN BUFSIZ

enum log_state
{
    NOT_NOTIFY_TELEGRAM,
    NOTIFY_TELEGRAM,
};

static struct log 
{
    FILE* log_s;
    FILE* err_s;
    const char* file;
    char msg[BUFSIZ];
    int line;
    enum log_type type;
} log;

static enum log_state state = NOT_NOTIFY_TELEGRAM;

static const char *bot_token = NULL;
static const char *chat_id = NULL;

static pthread_t tid;
static int flag = 0;
static pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;

//----------------------Private declarations-----------------------

static void log_to_telegram(enum log_type type, const char *msg);

static void *log_write(void *arg);

//-----------------------Public definitions-----------------------

void log_init(const char *log_path, const char *err_log_path, const char *btoken, const char *cid)
{   
    log.log_s = fopen(log_path, "w");
    if (log.log_s == NULL) {
        perror("log_init fopen error");
        exit(EXIT_FAILURE);
    }
    
    log.err_s = fopen(err_log_path, "w");
    if (log.err_s == NULL) {
        perror("log_init fopen error");
        exit(EXIT_FAILURE);
    }

    bot_token = btoken;
    chat_id = cid;
    if (bot_token != NULL && chat_id != NULL) {
        state = NOTIFY_TELEGRAM;
    }

    if (pthread_create(&tid, NULL, log_write, NULL) != 0) {
        exit(EXIT_FAILURE);
    }

    if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) != 0) {
        exit(EXIT_FAILURE);
    }

    if (pthread_detach(tid) != 0) {
        exit(EXIT_FAILURE);
    }
}

void logger(enum log_type type, const char* file, int line, const char *fmt, ...)
{
    if (pthread_mutex_lock(&mlock) != 0) {
        exit(EXIT_FAILURE);
    }

    log.type = type;
    log.file = file;
    log.line = line;

    memset(log.msg, 0, BUFSIZ);
    va_list args;
    va_start(args, fmt);
    vsprintf(log.msg, fmt, args);
    va_end(args);

    flag = 0;

    if (pthread_mutex_unlock(&mlock) != 0) {
        exit(EXIT_FAILURE);
    }
    while (flag == 0) {
        if (pthread_cond_signal(&cv) != 0) {
            exit(EXIT_FAILURE);
        }
    }
}

void log_deinit(void)
{
    if (pthread_cancel(tid) != 0) {
        exit(EXIT_FAILURE);
    }

    if (fclose(log.err_s) < 0) {
        exit(EXIT_FAILURE);
    }

    if (fclose(log.log_s) < 0) {
        exit(EXIT_FAILURE);
    }
}

//----------------------Private definitions-----------------------

static void log_to_telegram(enum log_type type, const char *msg) 
{
    CURL *curl;
    CURLcode res;
    char url[URL_MAX_LEN] = {0};

    switch (type)
    {
    case INFO_LOG:
        snprintf(url, URL_MAX_LEN, "https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s", bot_token, chat_id, msg);
        break;
    case ERR_LOG:
        snprintf(url, URL_MAX_LEN, "https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s", bot_token, chat_id, msg);
        break;
    case SYS_ERR_LOG:
        snprintf(url, URL_MAX_LEN, "https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s:%s", bot_token, chat_id, msg, strerror(errno));
        break;
    default:
        return;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl) {
        if (curl_easy_setopt(curl, CURLOPT_URL, url) < 0) {
            state = NOT_NOTIFY_TELEGRAM;
            LOG_ERR("curl_easy_setopt:%s", curl_easy_strerror(res));
        }

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            state = NOT_NOTIFY_TELEGRAM;
            LOG_ERR("curl_easy_perform() failed:%s", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
    
    curl_global_cleanup();
}

static void *log_write(void *arg)
{
    (void)arg;

    for (;;) {
        if (pthread_mutex_lock(&mlock) != 0) {
            exit(EXIT_FAILURE);
        }
    
        if (pthread_cond_wait(&cv, &mlock) != 0) {
            exit(EXIT_FAILURE);
        }
        
        flag = 1;
        switch (log.type)
        {
        case INFO_LOG:
            file_wlock(fileno(log.log_s));
            fprintf(log.log_s, "%s %s\n", __TIMESTAMP__, log.msg); 
            fflush(log.log_s);
            file_unlock(fileno(log.log_s));
            if (state == NOTIFY_TELEGRAM) {
                log_to_telegram(INFO_LOG, log.msg);
            }
            break;
        case ERR_LOG:
            file_wlock(fileno(log.err_s));
            fprintf(log.err_s, "%s %s:%d ERROR %s\n", __TIMESTAMP__, log.file, log.line, log.msg);
            fflush(log.err_s);
            file_unlock(fileno(log.err_s));
            if (state == NOTIFY_TELEGRAM) {
                log_to_telegram(ERR_LOG, log.msg);
            }
            exit(EXIT_FAILURE);
        case SYS_ERR_LOG:
            file_wlock(fileno(log.err_s));
            fprintf(log.err_s, "%s %s:%d SYSTEM ERROR %s:%s\n", __TIMESTAMP__, log.file, log.line, log.msg, strerror(errno));
            fflush(log.err_s);
            file_unlock(fileno(log.err_s));
            if (state == NOTIFY_TELEGRAM) {
                log_to_telegram(SYS_ERR_LOG, log.msg);
            }
            exit(EXIT_FAILURE);
        default:
            break;
        }

        if (pthread_mutex_unlock(&mlock) != 0) {
            exit(EXIT_FAILURE);
        }
    }

    return NULL;
}