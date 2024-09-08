#include "dns.h"
#include "log.h"

#include <stdio.h>

#define DNS_NAME_LEN 256

//-----------------------Public definitions------------------------

enum dns_type dns_get_type(struct dnshdr *dnsh)
{
    return dnsh->qr ? DNS_RESPONSE : DNS_QUERY;
}

const unsigned char *dns_extract_name(const unsigned char *packet, const unsigned char *pname, char *name)
{
    if ((*pname & 0xC0) == 0xC0) {
        const int k = ((*pname & 0x3F) << 8) + pname[1];
        pname += 2;
        dns_extract_name(packet, packet + k, name);

        return pname;
    } else {
        const int len = *pname++;

        sprintf(name, "%.*s", len, pname);
        name += len;
        pname += len;
        if (*pname) {
            sprintf(name, ".");
            name += 1;

            return dns_extract_name(packet, pname, name);
        } else {
            return pname + 1;
        }
    }
}