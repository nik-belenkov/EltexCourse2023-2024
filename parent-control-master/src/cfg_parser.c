#include "cfg_parser.h"
#include "log.h"
#include "hash_table.h"
#include "validator.h"
#include "domain.h"
#include "ipaddr.h"
#include "block_traffic.h"

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define DELIMS "\n "

//-----------------------Public definitions------------------------

void cfg_parser(struct hash_table **table, const char *path)
{
    struct entry *entry = NULL;
    const char *key = NULL;
    enum entry_type type;

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        LOG_SYS_ERR("open");
    }

    struct stat s;
    if (fstat(fd, &s) < 0) {
        LOG_SYS_ERR("fstat");
    }

    off_t file_len = s.st_size;
    char *buf = (char *)malloc(file_len);
    if (buf == NULL) {
        LOG_SYS_ERR("malloc");
    }

    ssize_t rbytes = read(fd, buf, file_len);
    if (rbytes < 0) {
        LOG_SYS_ERR("read");
    }

    char *token = strtok(buf, DELIMS);
    while (token != NULL) {
        type = validator(token);
        switch (type)
        {
        case IP_ENTRY:
            entry = (struct entry *)ipaddr_create(token);
            key = ipaddr_get_addr((struct ipaddr *)entry);
            break;
        case DOMAIN_ENTRY:
            entry = (struct entry *)domain_create(token);
            key = domain_get_name((struct domain *)entry);
            break;
        default:
            token = strtok(NULL, DELIMS);
            continue;;
        }
       
        hash_table_insert(table, key, entry);
        token = strtok(NULL, DELIMS);
    }

    free(buf);

    if (close(fd) < 0) {
        LOG_SYS_ERR("close");
    }
}