#include "map.h"
#include <stdio.h>

unsigned int hash(char *key)
{
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


static int mapIndex(char *key, int size)
{
   unsigned int h = hash(key);
   int ids = h % size;
   return ids;
}


map_t *mapCreate(unsigned long size)
{
  map_t *map = (map_t *)malloc(sizeof(map_t));
  map->count = 0;
  map->size = size;
  map->hash = (map_entry_t **)malloc(size * sizeof(map_entry_t*));
  return map; 
}

static map_entry_t *mapFindMapEntry(map_t *map, char *key, int *ids)
{
  *ids = mapIndex(key, map->size);
  map_entry_t *p = map->hash[*ids];
  while(p != NULL)
  {
    if(!strcmp(key, p->key)) return p;
    p = p->next;
  }
  return NULL;
}


char *mapFind(map_t *map, char *key)
{
  int ids;
  map_entry_t *entry = mapFindMapEntry(map, key, &ids);
  return entry == NULL ? NULL : entry->value;
}

static void expandMap(map_t *map)
{
   map->size *=1.5;
   map->hash = (map_entry_t **)realloc(map->hash, sizeof(map_entry_t*)*map->size);
}


void mapModify(map_t *map, char *key, char *value)
{
 /*
  float factor = (float)(map->count*1.0/map->size);
  if(factor > 0.75)
    expandMap(map);
 */
  int ids;
  map_entry_t *entry = mapFindMapEntry(map, key, &ids);
  if(entry){
     free(entry->value);
     entry->value = value;
  }else{
     entry = (map_entry_t *)malloc(sizeof(map_entry_t));
     entry->key = key;
     entry->value = value;
     entry->next = map->hash[ids];
     map->count++;
     map->hash[ids] = entry;
  }
}

void mapFree(map_t *map)
{
  if(!map) return;
  int i=0;
  for(i;i<map->size;i++)
  {
    free(map->hash[i]);
  }
  free(map->hash);
  free(map);
}

int main()
{
  map_t *map = mapCreate(100);
  mapModify(map, "a", "a");
  mapModify(map, "b", "b");
  mapModify(map, "c", "c");
  mapModify(map, "d", "d");
  mapModify(map, "e", "e");

  char *v = mapFind(map, "a");
  printf("v=%s\n", v);

  v = mapFind(map, "b");
  printf("v=%s\n", v);
 
  v = mapFind(map, "c");
  printf("v=%s\n", v);

  v = mapFind(map, "d");
  printf("v=%s\n", v);

  v = mapFind(map, "e");
  printf("v=%s\n", v);

  mapFree(map);

  return 0;
}

