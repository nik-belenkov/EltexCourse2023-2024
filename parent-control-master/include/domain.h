#ifndef DOMAIN_H
#define DOMAIN_H

#include <stdbool.h>

#define DOMAIN_MAX_LEN 256

struct domain *domain_create(const char *str);

bool domain_is_resolve(struct domain *domain);

void domain_set_resolve(struct domain *domain, bool resolve);

const char *domain_get_name(struct domain *domain);

#endif // DOMAIN_H