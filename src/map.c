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
  map->size = size;
  map->hash = (map_entry_t **)malloc(size * sizeof(map_entry_t*));
  return map; 
}


int mapAdd(map_t *map, char *key, char *value)
{
  
  return 0;
}



int main()
{
  char *s = "aaaaaaaaaa";
  printf("%d", mapIndex(s, 10));

  map_t *map = mapCreate(10); 
  return 0;
}

