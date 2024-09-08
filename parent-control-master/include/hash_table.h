#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>
#include <stdio.h>

struct entry;

struct hash_table *hash_table_create(void);

void hash_table_ctor(struct hash_table *table, size_t init_size);

void hash_table_insert(struct hash_table **table, const char *key, struct entry *data);

void hash_table_remove(struct hash_table *table, const char *key);

const void *hash_table_search(const struct hash_table *table, const char *key);

void hash_table_foreach(const struct hash_table *table, void (*func)(struct entry *));

void hash_table_print(const struct hash_table *table, FILE *file);

void hash_table_dtor(struct hash_table *table);

void hash_table_destroy(struct hash_table *table);

#endif // HASH_TABLE_H
