#include "command_executor.h"
#include "file_lock.h"
#include "log.h"

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

//----------------------Private declarations-----------------------

static void command_executor_kill(const char *path);

static pid_t command_executor_get_pid(const char *path);

static void command_executor_quit(const char *path);

static void command_executor_print_log(const char *path);

static void command_executor_write_runtime(const char *path);

static void command_executor_switch_state(const char *path);

static void command_executor_help(void);

//-----------------------Public definitions-----------------------

void command_executor(enum command_executor_type type, const char *path) 
{
    switch (type) {
        case HELP:
            command_executor_help();
            break;
        case KILL:
            command_executor_kill(path);
            break;
        case QUIT:
            command_executor_quit(path);
            break;
        case OUT_LOG:
            command_executor_print_log(path);
            break;
        case OUT_ERRLOG:
            command_executor_print_log(path);
            break;
        case RUNTIME_FILE:
            command_executor_write_runtime(path);
            break;
        case SWITCH_STATE:
            command_executor_switch_state(path);
            break;
        default:
            break;
    }
}

//----------------------Private definitions-----------------------

static void command_executor_help(void) 
{
    printf("Usage: Parent control [OPTION]\n");
    printf("  Options:\n");
    printf("   -h --help                 Print this message\n");
    printf("   -k --kill                 Kill daemon\n");
    printf("   -q --quit                 Quit daemon\n");
    printf("   -l --log                  Write logs to the stdout\n");
    printf("   -r --runtime              Write runtime cfg to file\n");
    printf("   -e --error                Write error logs to the stdout\n");
    printf("   -f --file  filename       Used custom cfg file\n");
    printf("   -t --token token          Set telegram bot token\n");
    printf("   -i --id  chat id          Set telegram bot chat id\n");
    printf("   -s --state                Daemon switch state\n");

    printf("\n");
}

static void command_executor_write_runtime(const char *path)
{
    pid_t pid = command_executor_get_pid(path);
   
    if (kill(pid, SIGUSR2) < 0) {
        perror("kill");
        exit(EXIT_FAILURE);
    }
}

static pid_t command_executor_get_pid(const char *path)
{
    FILE *f;
    pid_t pid;

    if (access(path, F_OK) != 0) {
        fprintf(stderr, "daemon not exist\n");
        exit(EXIT_SUCCESS);
    }

    f = fopen(path, "r");
    if (f == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fscanf(f, "%d", &pid);

    if (fclose(f) < 0) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    return pid;
}

static void command_executor_kill(const char *path) 
{
    pid_t pid = command_executor_get_pid(path);

    if (unlink(path) < 0) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    if (kill(pid, SIGKILL) < 0) {
        perror("kill");
        exit(EXIT_FAILURE);
    }
}

static void command_executor_quit(const char *path)
{
    pid_t pid = command_executor_get_pid(path);

    if (kill(pid, SIGQUIT) < 0) {
        perror("kill");
        exit(EXIT_FAILURE);
    }
}

static void command_executor_print_log(const char *path) 
{ 
#define LINE_LEN 100

    char line[LINE_LEN] = {0};

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        return;
    }

    file_rlock(fileno(f));
    while ((fgets(line, LINE_LEN, f)) != NULL) {
        printf("%s", line);
    }
    file_unlock(fileno(f));

    if (fclose(f) < 0) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
}

static void command_executor_switch_state(const char *path)
{
    pid_t pid = command_executor_get_pid(path);

    printf("Daemon switch state\n");

    if (kill(pid, SIGUSR1) < 0) {
        perror("kill");
        exit(EXIT_FAILURE);
    }
}