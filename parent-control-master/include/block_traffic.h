#ifndef BLOCK_TRAFFIC_H
#define BLOCK_TRAFFIC_H

struct entry;

void block_traffic(struct entry *entry);

void unblock_traffic(struct entry *entry);

#endif // BLOCK_TRAFFIC_H