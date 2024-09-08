#include "block_traffic.h"
#include "ipaddr.h"
#include "domain.h"
#include "entry.h"

#include <stdio.h>
#include <stdlib.h>

#define COMMAND_SIZE 512

static void iptables_block(const char *key);

void iptables_unblock(const char *key);

void block_traffic(struct entry *entry) 
{
    struct ipaddr *ipaddr = NULL;
    struct domain *domain = NULL;
    const char *data = NULL;

    switch (entry->type)
    {
    case IP_ENTRY:
        ipaddr = (struct ipaddr *)entry;
        data = ipaddr_get_addr(ipaddr);
        break;
    case DOMAIN_ENTRY:
        domain = (struct domain *)entry;
        data = domain_get_name(domain);
    default:
        return;
    }

    iptables_block(data);
}

void unblock_traffic(struct entry *entry) 
{
    struct ipaddr *ipaddr = NULL;
    struct domain *domain = NULL;
    const char *data = NULL;

    switch (entry->type)
    {
    case IP_ENTRY:
        ipaddr = (struct ipaddr *)entry;
        data = ipaddr_get_addr(ipaddr);
        break;
    case DOMAIN_ENTRY:
        domain = (struct domain *)entry;
        data = domain_get_name(domain);
    default:
        return;
    }

    iptables_unblock(data);
}

static void iptables_block(const char *key) 
{
    char command[COMMAND_SIZE] = {0};
    snprintf(command, sizeof(command), "iptables -A OUTPUT -d %s -j DROP", key);
    system(command);
}

void iptables_unblock(const char *key) 
{
    char command[COMMAND_SIZE] = {0};
    snprintf(command, sizeof(command),  "iptables -D OUTPUT -d %s -j DROP", key);
    system(command);
}