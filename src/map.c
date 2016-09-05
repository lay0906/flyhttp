#include "map.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

unsigned int hash(void *k)
{
    char *key = (char *)k;
    uint32_t seed = 5381;
    const uint32_t m = 0x5bd1e995;
    const int r = 24;
    int len = strlen(key);

    /* Initialize the hash to a 'random' value */
    uint32_t h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = (const unsigned char *)key;

    while(len >= 4) {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0]; h *= m;
    };

    /* Do a few final mixes of the hash to ensure the last few
 *      * bytes are well-incorporated. */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned int)h;
}

static int __mask(int capacity)
{
  int s = 2;
  while(s <= capacity)
  {
    s = s<<1;
  }
  return s - 1;
}

map_t *map_create(int capacity, map_type_t* type)
{
  map_t *map = (map_t*)malloc(sizeof(map_t));
  map->capacity = capacity;
  map->size = 0;
  map->hash = (map_entry_t **)malloc(sizeof(map_entry_t*)*capacity);
  memset(map->hash, 0, sizeof(map_entry_t*)*capacity);
  map->type = type;
 // map->mask = __mask(capacity);
  return map;
}

static map_entry_t* __map_get(map_t *map, void *k, int *ids)
{
  unsigned int h = map_hash(map, k);
  *ids = map_index(map, h);
  map_entry_t *p = map->hash[*ids];
  while(p){
    if(!map_key_compare(map, k, p->k))
       break;
    p = p->next;
  }
  return p;
}

static void __map_expand(map_t *map)
{
   int l = map->capacity;
   map->capacity *=1.5;
   map_entry_t **p = (map_entry_t **)malloc(sizeof(map_entry_t*)*map->capacity);
   memset(p, 0, map->capacity*sizeof(map_entry_t*));
   int i;
   for(i=0;i<l;i++)
   {
     map_entry_t *last = map->hash[i];
     while(last){
    	 map_entry_t *tmp = last->next;
         int h = map_hash(map, last->k);
    	 int ids = map_index(map, h);
    	 last->next = p[ids];
    	 p[ids] = last;
    	 last = tmp;
     }
   }
   free(map->hash);
   map->hash = p;
}


void map_put(map_t *map, void *k, void *v)
{
  if((float)(map->size*1.0/map->capacity) > 0.75) 
     __map_expand(map);
  int ids;
  map_entry_t *p = __map_get(map, k, &ids);
  if(p){
    map_free_val(map, p);
    p->v = v;
  }else{
    p = (map_entry_t*)malloc(sizeof(map_entry_t));
    p->k = k;
    p->v = v;
    p->next = map->hash[ids];
    map->hash[ids] = p;
    map->size++;   
  }
}

void *map_get(map_t *map, void *k)
{
  int ids;
  map_entry_t *p = __map_get(map, k, &ids); 
  return p == NULL ? NULL : p->v;
}
