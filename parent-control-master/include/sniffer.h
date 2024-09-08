#ifndef SNIFFER_H
#define SNIFFER_H

struct hash_table;

void sniffer_init(void);

void sniffer_sniff(struct hash_table **ht);

void sniffer_deinit(void);

#endif // SNIFFER_H