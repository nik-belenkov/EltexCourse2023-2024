#ifndef CFG_PARSER_H
#define CFG_PARSER_H

struct hash_table;

void cfg_parser(struct hash_table **table, const char *path);

#endif // CFG_PARSER_H