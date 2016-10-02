#ifndef _MAP_H_
#define _MAP_H

typedef struct map_entry_s
{
  void *k, *v;
  struct map_entry_s *next;
} map_entry_t;

typedef struct map_type_s
{
  unsigned int (*hash)(const void *k);
  int (*key_compare)(const void *key1, const void *key2);
  void (*key_free)(void *key);
  void (*val_free)(void *key);
} map_type_t;

typedef struct map_s
{
  struct map_entry_s **hash;
  int size, capacity, mask;
  struct map_type_s *type;
} map_t;


#define map_free_val(map, entry) do { \
    if((map)->type && (map)->type->val_free) \
       (map)->type->val_free((entry)->v); \
    else \
     /* free((entry)->v)*/; \
} while(0)

#define map_free_key(map, entry) do { \
  if((map)->type && (map)>type->key_free) \
    (map)->type->key_free(entry->k); \
  else \
    /*free((entry)->k)*/; \
} while(0)

#define map_key_compare(map, key1, key2) \
  (((map)->type && (map)->type->key_compare) ? \
     (map)->type->key_compare(key1, key2): \
     strcmp((char*)(key1), (char*)(key2)))

#define map_hash(map, key) \
  ((map->type) && (map)->type->hash) ? \
    (map)->type->hash(key):hash(key)

#define map_index(map, h) ((h)%(map->capacity))

unsigned int hash(void *key);
map_t *map_create(int capacity, map_type_t* type);
void map_put(map_t *map, void *k, void *v);
void *map_get(map_t *map, void *k);
void map_remove(map_t *map, void *k);
void map_free(map_t *map);


#endif
