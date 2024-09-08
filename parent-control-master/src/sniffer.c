#include "sniffer.h"
#include "hash_table.h"
#include "dns.h"
#include "log.h"
#include "entry.h"
#include "ipaddr.h"
#include "domain.h"
#include "block_traffic.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>
#include <net/if.h>

#define DNS_PORT 53
#define DNS_TYPE_A 1

static struct sniffer
{
    int fd;
    char ifname[IF_NAMESIZE];
    struct sockaddr_ll sll;
} sniffer;

//----------------------Private declarations-----------------------

static void sniffer_init_default_ifname(void);

static void sniffer_detector(struct hash_table **ht, const unsigned char *packet);

//-----------------------Public definitions-----------------------

void sniffer_init(void)
{
    sniffer_init_default_ifname();

    sniffer.fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sniffer.fd < 0) {
        LOG_SYS_ERR("socket");
    }

    sniffer.sll.sll_family = AF_PACKET;
    sniffer.sll.sll_halen = ETH_ALEN;
    sniffer.sll.sll_ifindex = if_nametoindex(sniffer.ifname);

    if (bind(sniffer.fd, (struct sockaddr *)&sniffer.sll, sizeof(struct sockaddr_ll)) < 0) {
        LOG_SYS_ERR("bind");
    }
}

void sniffer_sniff(struct hash_table **ht)
{
    unsigned char buffer[ETH_FRAME_LEN] = {0};
    for (;;) {
        ssize_t rbytes = recvfrom(sniffer.fd, buffer, ETH_FRAME_LEN, 0, NULL, NULL);
        if (rbytes < 0) {
            if (close(sniffer.fd) < 0) {
                LOG_SYS_ERR("close");
            }
            LOG_SYS_ERR("recvfrom");
        }

        sniffer_detector(ht, buffer);

        bzero(buffer, ETH_FRAME_LEN);
    }
}

void sniffer_deinit(void)
{
    if (close(sniffer.fd) < 0) {
        exit(EXIT_FAILURE);
    }
}

//----------------------Private definitions-----------------------

static void sniffer_detector(struct hash_table **ht, const unsigned char *packet) 
{
    size_t packet_offset = ETH_HLEN;
    struct iphdr *iph = (struct iphdr *)(packet + packet_offset);
    packet_offset += sizeof(struct iphdr);

    char ip[INET_ADDRSTRLEN] = {0};
    if (inet_ntop(AF_INET, &iph->daddr, ip, INET_ADDRSTRLEN) == NULL) {
        LOG_SYS_ERR("inet_ntop");
    }

    const void *ip_data = hash_table_search(*ht, ip);
    if (ip_data) {
        LOG_INFO("DETECTED banned ip:%s", ip);
    } else if (iph->protocol == IPPROTO_UDP) {
        struct udphdr *udph = (struct udphdr *)(packet + packet_offset);
        packet_offset += sizeof(struct udphdr);

        unsigned short dport = ntohs(udph->dest);
        unsigned short sport = ntohs(udph->source);
        if (sport == DNS_PORT || dport == DNS_PORT) {
            struct dnshdr *dnsh = (struct dnshdr *)(packet + packet_offset);
            packet_offset += sizeof(struct dnshdr);

            enum dns_type type = dns_get_type(dnsh);
            const unsigned short qdcount = ntohs(dnsh->qdcount);
            if (qdcount <= 0) {
                return;
            }

            char name[DOMAIN_MAX_LEN] = {0};
            const unsigned char *dnsqh = dns_extract_name(packet, packet + packet_offset, name);
            packet_offset = (dnsqh - packet) + sizeof(struct dnsqhdr);

            struct domain *domain = (struct domain *)hash_table_search(*ht, name);
            if (domain) {
                if (!domain_is_resolve(domain) && type == DNS_RESPONSE) {
                    const unsigned short ancount = ntohs(dnsh->ancount);
                    for (unsigned short i = 0; i < ancount; ++i) {
                        bzero(name, DOMAIN_MAX_LEN);

                        const unsigned char *dnsah = dns_extract_name(packet, packet + packet_offset, name);
                        struct dnsahdr *aflags = (struct dnsahdr *)dnsah;
                        if (ntohs(aflags->type) != DNS_TYPE_A) {
                            continue;
                        }
                        unsigned short rdlen = ntohs(aflags->rdlength);

                        packet_offset = (dnsah - packet) + sizeof(struct dnsahdr);

                        const unsigned char *paddr = packet + packet_offset;

                        bzero(ip, INET_ADDRSTRLEN);
                        if (inet_ntop(AF_INET, paddr, ip, INET_ADDRSTRLEN) == NULL) {
                            LOG_SYS_ERR("inet_ntop");
                        }

                        ip_data = hash_table_search(*ht, ip);
                        if (ip_data) {
                            LOG_INFO("DETECTED banned ip:%s", ip);
                        } else {
                            struct ipaddr *ipaddr = ipaddr_create(ip);
                            const char *key = ipaddr_get_addr(ipaddr);
                            hash_table_insert(ht, key, (void *)ipaddr);
                        }

                        packet_offset += rdlen;
                    }
                    domain_set_resolve(domain, true);
                } else if (type == DNS_QUERY) {
                    LOG_INFO("DETECTED banned domain:%s", name);
                }
            }
        }
    }

}

static void sniffer_init_default_ifname(void)
{
#define LINE_LEN 256
#define DEFAULT_ADDR "00000000"

    FILE *f;
    char line[LINE_LEN] = {0};

    f = fopen("/proc/net/route" , "r");
    if (f == NULL) {
        LOG_SYS_ERR("fopen");
    }

    while (fgets(line , LINE_LEN, f))
    {
        char *p = strtok(line, " \t");
        char *c = strtok(NULL, " \t");

        if (p != NULL && c != NULL)
        {
            if (strcmp(c , DEFAULT_ADDR) == 0)
            {
                strcpy(sniffer.ifname, c);
                break;
            }
        }
    }

    fclose(f);
}