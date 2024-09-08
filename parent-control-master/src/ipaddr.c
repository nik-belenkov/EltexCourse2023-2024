#include "ipaddr.h"
#include "entry.h"
#include "log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

struct ipaddr
{
    struct entry entry;
    char addr[INET_ADDRSTRLEN];
};

//-----------------------Public definitions------------------------

struct ipaddr *ipaddr_create(const char *str)
{
    struct ipaddr *ipaddr = (struct ipaddr *)malloc(sizeof(struct ipaddr));
    if (ipaddr == NULL) {
        LOG_SYS_ERR("malloc");
    }
    
    memcpy(ipaddr->addr, str, strlen(str));

    ipaddr->entry.type = IP_ENTRY;

    return ipaddr;
}

const char *ipaddr_get_addr(struct ipaddr *ipaddr)
{
    return ipaddr->addr;
}