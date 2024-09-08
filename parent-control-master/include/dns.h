#ifndef DNS_H
#define DNS_H

enum dns_type
{
    DNS_RESPONSE,
    DNS_QUERY,
};

struct dnshdr
{
    unsigned short id;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int rd:1;
	unsigned int tc:1;
	unsigned int aa:1;
	unsigned int opcode:4;
	unsigned int qr:1;

	unsigned int rcode:4;
	unsigned int z:3;
	unsigned int ra:1;
#else
    unsigned int qr:1;
	unsigned int opcode:4;
	unsigned int aa:1;
	unsigned int tc:1;
	unsigned int rd:1;
	unsigned int ra:1;
	unsigned int z:3;
	unsigned int rcode:4;
#endif
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
};

struct dnsqhdr
{
    unsigned short qtype;
    unsigned short qclass;
};

struct __attribute__ ((__packed__)) dnsahdr
{
    unsigned short type;
    unsigned short class;
    unsigned int ttl;
    unsigned short rdlength;
};

enum dns_type dns_get_type(struct dnshdr *dnsh);

const unsigned char *dns_extract_name(const unsigned char *packet, const unsigned char *pname, char *name);

#endif // DNS_H