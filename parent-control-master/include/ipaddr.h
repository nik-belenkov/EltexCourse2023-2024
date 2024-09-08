#ifndef IPADDR_H
#define IPADDR_H

struct ipaddr *ipaddr_create(const char *str);

const char *ipaddr_get_addr(struct ipaddr *ipaddr);

#endif // IPADDR_H