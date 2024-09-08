
#include "domain.h"
#include "entry.h"
#include "log.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct domain
{
    struct entry entry;
    char name[DOMAIN_MAX_LEN];
    bool resolve;
};

//-----------------------Public definitions------------------------

struct domain *domain_create(const char *str)
{
    struct domain *domain = (struct domain *)malloc(sizeof(struct domain));
    if (domain == NULL) {
        LOG_SYS_ERR("malloc");
    }
    
    memcpy(domain->name, str, strlen(str));

    domain->entry.type = DOMAIN_ENTRY;
    
    return domain;
}

bool domain_is_resolve(struct domain *domain)
{
    return domain->resolve;
}

void domain_set_resolve(struct domain *domain, bool resolve)
{
    domain->resolve = resolve;
}

const char *domain_get_name(struct domain *domain)
{
    return domain->name;
}