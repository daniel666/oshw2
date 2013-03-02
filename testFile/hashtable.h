#ifndef HASHTABLE_H_
#define HASHTABLE_H_
#include<stdlib.h>
#include<sys/types.h>
typedef size_t hash_size;


typedef struct hashtbl {
      hash_size size;
      struct hashnode_s **nodes;
      hash_size (*hashfunc)(const char *);
} HASHTBL;

struct hashnode_s {
      char *key;
      pid_t data;
      struct hashnode_s *next;
};

HASHTBL *hashtbl_create(hash_size size, hash_size (*hashfunc)(const char *));
void hashtbl_fill(HASHTBL *hashtbl);
void hashtbl_destroy(HASHTBL *hashtbl);
int hashtbl_insert(HASHTBL *hashtbl, const char *key, pid_t data);
int hashtbl_remove(HASHTBL *hashtbl, const char *key);
pid_t hashtbl_get(HASHTBL *hashtbl, const char *key);
int hashtbl_resize(HASHTBL *hashtbl, hash_size size);
void hashtbl_traverse(HASHTBL *hashtbl, hash_size size);
#endif
