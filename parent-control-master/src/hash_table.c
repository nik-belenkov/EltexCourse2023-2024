#include "hash_table.h"
#include "entry.h"
#include "ipaddr.h"
#include "block_traffic.h"
#include "domain.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

struct node 
{
    const char *key;
    void *data;
    struct node *next;
};

struct hash_table
{
    struct node **buckets;
    size_t size;
    size_t capacity;
};

//----------------------Private declarations-----------------------

static struct node *node_create(void);

static void node_ctor(struct node *head, const char *key, void *data);

static struct node *node_search(struct node *head, const char *key);

static void node_dtor(struct node *key);

static void node_destroy(struct node *head);

static size_t hash_table_get_hash(const struct hash_table *table, const char *key);

static struct hash_table *hash_table_resize(struct hash_table *table, const size_t new_capacity);

//-----------------------Public definitions-----------------------

struct hash_table *hash_table_create(void) 
{
    struct hash_table *table = (struct hash_table *)malloc((sizeof(struct hash_table)));
    if (table == NULL) {
        LOG_SYS_ERR("malloc");
    }

    return table;
}

void hash_table_ctor(struct hash_table *table, size_t init_size)
{
#define DEFAULT_TABLE_SIZE 32

    if (init_size == 0) {
        init_size = DEFAULT_TABLE_SIZE;
    }

    table->buckets = (struct node **)malloc(sizeof(struct node *) * init_size);
    if (table->buckets == NULL) {
        hash_table_destroy(table);
        LOG_SYS_ERR("malloc");
    }

    table->size = 0;
    table->capacity = init_size;

    for (size_t i = 0; i < init_size; ++i) {
        table->buckets[i] = NULL;
    }
}

void hash_table_insert(struct hash_table **ptable, const char *key, struct entry *data) 
{
    if (key == NULL) {
        return;
    }

    struct hash_table *table = *ptable;

    size_t index = hash_table_get_hash(table, key);

    struct node *head = table->buckets[index];

    struct node *exist_node = node_search(head, key);
    if (exist_node != NULL) {
        free(exist_node->data);
        exist_node->data = data;
        return;
    }

    struct node *new_node = node_create();

    node_ctor(new_node, key, data);

    new_node->next = head;
    table->buckets[index] = new_node;
    table->size++;

    if (table->size >= 1.5 * table->capacity) {
        *ptable = hash_table_resize(table, 2 * table->capacity);
    }
}

void hash_table_remove(struct hash_table *table, const char *key) 
{
    if (key == NULL) {
        return;
    }

    size_t index = hash_table_get_hash(table, key);

    struct node *prev = NULL;
    struct node *head = table->buckets[index];

    while (head != NULL) {
        if (strcmp(head->key, key) == 0) {
            if (prev == NULL) {
                table->buckets[index] = head->next;
            } else {
                prev->next = head->next;
            }

            table->size--;
            node_dtor(head);
            node_destroy(head);
            break;
        }
        prev = head;
        head = head->next;
    }
}

const void *hash_table_search(const struct hash_table *table, const char *key) 
{
    if (key == NULL) {
        return NULL;
    }

    size_t index = hash_table_get_hash(table, key);

    struct node *head = table->buckets[index];

    struct node *node = node_search(head, key);

    return node == NULL ? NULL : node->data;
}

void hash_table_foreach(const struct hash_table *table, void (*func)(struct entry *))
{
    struct node *head = NULL;

    for (size_t i = 0; i < table->capacity; ++i)
    {
        head = table->buckets[i];

        while (head != NULL)
        {
            struct entry *entry = (struct entry*)head->data;
        
            func(entry);

            head = head->next;
        }
    }
}

void hash_table_print(const struct hash_table *table, FILE *file)
{
    struct domain *domain = NULL;
    struct ipaddr *ipaddr = NULL;
    const char *data = NULL;
    struct node *head = NULL;

    for (size_t i = 0; i < table->capacity; ++i)
    {
        head = table->buckets[i];

        while (head != NULL)
        {
            struct entry *entry = (struct entry*)head->data;
        
            switch (entry->type)
            {
            case IP_ENTRY:
                ipaddr = (struct ipaddr *)entry;
                data = ipaddr_get_addr(ipaddr);
                break;
            case DOMAIN_ENTRY:
                domain = (struct domain *)entry;
                data = domain_get_name(domain);
                break;
            default:
                continue;;
            }
            fprintf(file, "%s\n", data);

            head = head->next;
        }
    }
}

void hash_table_dtor(struct hash_table *table)
{
    struct node* head = NULL;

    for (size_t i = 0; i < table->capacity; ++i) {
        head = table->buckets[i];

        while (head)
        {
            struct node *next = head->next;
            node_dtor(head);
            node_destroy(head);

            head = next;
        }
    }
    
    free(table->buckets);
}

void hash_table_destroy(struct hash_table *table)
{
    free(table);
}

//----------------------Private definitions-----------------------

static struct hash_table *hash_table_resize(struct hash_table *table, const size_t new_capacity) 
{
    struct hash_table *new_table = hash_table_create();

    hash_table_ctor(new_table, new_capacity);

    struct node *head = NULL;
    struct node *next = NULL;
    for (size_t i = 0; i < table->capacity; ++i) {
        head = table->buckets[i];

        while (head != NULL) {
            hash_table_insert(&new_table, head->key, head->data);
            next = head->next;
            node_destroy(head);
            head = next;
        }
    }

    free(table->buckets);

    hash_table_destroy(table);

    return new_table;
}

static size_t hash_table_get_hash(const struct hash_table *table, const char *key) 
{
    // jenkins one at a time hash func
    size_t len = strlen(key);
    size_t hash = 0;
    for (size_t i = 0; i < len; ++i) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % table->capacity;
}

static struct node* node_create()
{
    struct node *node = (struct node *)malloc(sizeof(struct node));
    if (node == NULL) {
        LOG_SYS_ERR("malloc");
    }

    return node;
}

static void node_ctor(struct node *head, const char *key, void *data)
{
    head->next = NULL;
    head->key = key;
    head->data = data;
}

static struct node *node_search(struct node *head, const char *key)
{
    while (head != NULL) {
        if (strcmp(head->key, key) == 0) {
            break;
        }

        head = head->next;
    }

    return head;
}

static void node_dtor(struct node *head)
{
    free(head->data);
}

static void node_destroy(struct node *head)
{
    free(head);
}