#include "validator.h"

#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

static const char *upper_domain_levels[] = {
    ".aero", ".arpa", ".asia",
    ".biz", ".cat", ".com",
    ".coop", ".edu", ".gov",
    ".info", ".int", ".jobs",
    ".mil", ".mobi", ".museum",
    ".name", ".net", ".org",
    ".post", ".pro", ".tel", 
    ".travel", ".xxx", ".ru",
};

//----------------------Private declarations-----------------------

static bool is_ip_public(unsigned int addr);

//-----------------------Public definitions-----------------------

enum entry_type validator(const char *str)
{
    unsigned int addr;
    if (inet_pton(AF_INET, str, &addr) > 0) {
        if (is_ip_public(addr)) {
            return IP_ENTRY;
        }
    } else {
        for (size_t i = 0; i < sizeof(upper_domain_levels) / sizeof(const char *); ++i) {
            if (strstr(str, upper_domain_levels[i]) != NULL) {
                return DOMAIN_ENTRY;
            }
        }
    }

    return INVALID_ENTRY;
}

//----------------------Private definitions-----------------------

static bool is_ip_public(unsigned int addr)
{
    unsigned char *byte = (unsigned char *)&addr;
    unsigned char *nbyte = NULL;

    if (*byte >= 1 && *byte <= 127) {
        if (*byte == 10) {
            return false;
        } 
        if (*byte == 127) {
            nbyte = byte + 1;
            for (int i = 1; i < 4; ++i, ++nbyte) {
                if (*nbyte != 0) {
                    return false;
                }
            }
        }
        return true;
    } else if (*byte >= 128 && *byte <= 191) {
        if (*byte == 172) {
            nbyte = byte + 1;
            if (*nbyte >= 16 && *nbyte <= 31) {
                return false;
            }
        }
        if (*byte == 191) {
            nbyte = byte + 2;
            for (int i = 2; i < 4; ++i, ++nbyte) {
                if (*nbyte != 0) {
                    return false;
                }
            }
        }
        return true;
    } else if (*byte >= 192 && *byte <= 223) {
        if (*byte == 192) {
            nbyte = byte + 1;
            if (*nbyte == 168) {
                return false;
            }
        }
        if (*byte == 223) {
            nbyte = byte + 3;
            if (nbyte != 0) {
                return false;
            }
        }
        return true;
    }
    
    return false;
}