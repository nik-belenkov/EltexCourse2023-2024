#include "command_executor.h"
#include "hash_table.h"
#include "block_traffic.h"
#include "cfg_parser.h"
#include "log.h"
#include "sniffer.h"

#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>

#define HT_INIT_SIZE 64

enum daemon_state
{
    DETECT,
    BLOCK,
} state;

static const char *short_opts = "hkrqlef:t:i:s";

static const struct option long_opts[] = {
    { "help",    no_argument,       NULL, 'h' },
    { "kill",    no_argument,       NULL, 'k' },
    { "quit",    no_argument,       NULL, 'q' },
    { "log",     no_argument,       NULL, 'l' },
    { "error",   no_argument,       NULL, 'e' },
    { "runtime", no_argument,       NULL, 'r' },
    { "file",    required_argument, NULL, 'f' },
    { "token",   required_argument, NULL, 't' },
    { "id",      required_argument, NULL, 'i' },
    { "state",   no_argument,       NULL, 's' },
};

static char *cfg_file = "cfg";
static char *log_file = "log";
static char *err_log_file = "err_log";
static char *pidfile = "pidfile";
static char *runtime_file = "runtime";

static char *bot_token = NULL;
static char *chat_id = NULL;

struct hash_table *ht = NULL;

void daemon_create(void);
void daemon_quit(int sig);
void daemon_write_runtime_cfg(int sig);

void daemon_switch_state(int sig);

void args_parser(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    args_parser(argc, argv);

    if (signal(SIGQUIT, daemon_quit) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGUSR1, daemon_switch_state) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGUSR2, daemon_write_runtime_cfg) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    daemon_create();

    log_init(log_file, err_log_file, bot_token, chat_id);

    sniffer_init();

    ht = hash_table_create();
    hash_table_ctor(ht, HT_INIT_SIZE);

    cfg_parser(&ht, cfg_file);

    sniffer_sniff(&ht);

    return 0;
}

void daemon_switch_state(int sig)
{
    (void)sig;

    if (state == DETECT) {
        state = BLOCK;
        hash_table_foreach(ht, block_traffic);
    } else {
        state = DETECT;
        hash_table_foreach(ht, unblock_traffic);
    }
    LOG_INFO("DAEMON SWITCH STATE TO %s", state == DETECT ? "DETECT" : "BLOCK");
}

void daemon_write_runtime_cfg(int sig)
{
    (void)sig;

    FILE *f = fopen(runtime_file, "w");
    if (f == NULL) {
        LOG_SYS_ERR("fopen");
    }

    hash_table_print(ht, f);

    fflush(f);

    if (fclose(f) < 0) {
        LOG_SYS_ERR("flose");
    }
}

void daemon_create(void) 
{
    // get pid
    const char *cmd = "ps -efj | grep -v grep | grep build/parental_control | awk '{print $2}'";

    pid_t pid;

    if (access(pidfile, F_OK) == 0) {
        fprintf(stderr, "daemon already created\n");
        exit(EXIT_SUCCESS);
    }

    FILE *f = fopen(pidfile, "w");
    if (f == NULL) {
        perror("daemon_create fopen");
        exit(EXIT_FAILURE);
    }

    if (daemon(1, 0) < 0) {
        perror("daemon_create");
        exit(EXIT_FAILURE);
    }

    FILE *fpout = popen(cmd, "r");
    if (fpout == NULL) {
        exit(EXIT_FAILURE);
    }

    fscanf(fpout, "%d", &pid);
    fprintf(f, "%d", pid);

    if (pclose(fpout) < 0) {
        exit(EXIT_FAILURE);
    }

    if (fclose(f) < 0) {
        exit(EXIT_FAILURE);
    }
}

void daemon_quit(int sig)
{
    (void)sig;

    log_deinit();

    sniffer_deinit();

    hash_table_foreach(ht, unblock_traffic);

    hash_table_dtor(ht);
    hash_table_destroy(ht);

    if (unlink(pidfile) < 0) {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

void args_parser(int argc, char *argv[])
{
    int flag, opt_index = 0;
    for (;;) {
        flag = getopt_long(argc, argv, short_opts, long_opts, &opt_index);
        switch (flag)
        {
        case 'h':
            command_executor(HELP, NULL);
            exit(EXIT_SUCCESS);
        case 'k':
            command_executor(KILL, pidfile);
            exit(EXIT_SUCCESS);
        case 'q':
            command_executor(QUIT, pidfile);
            exit(EXIT_SUCCESS);
        case 'l':
            command_executor(OUT_LOG, log_file);
            exit(EXIT_SUCCESS);
        case 'e':
            command_executor(OUT_ERRLOG, err_log_file);
            exit(EXIT_SUCCESS);
        case 'r':
            command_executor(RUNTIME_FILE, pidfile);
            exit(EXIT_SUCCESS);
        case 's':
            command_executor(SWITCH_STATE, pidfile);
            exit(EXIT_SUCCESS);
        case 'f':
            cfg_file = optarg;
            break;
        case 't':
            bot_token = optarg;
            break;
        case 'i':
            chat_id = optarg;
            break;
        case '?':
            command_executor(HELP, NULL);
            exit(EXIT_FAILURE);
        default:
            return;
        }
    }
}
