#ifndef _MAP_H_
#define _MAP_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct map_entry_s
{
  char *key;
  char *value;
  struct map_entry_s *next;
} map_entry_t;



typedef struct map_s
{
  map_entry_t **hash;
  unsigned long size;
} map_t;

unsigned int hash(char *key);

/*api*/
map_t *mapCreate(unsigned long size);
int mapAdd(map_t *map, char *key, char *value);
char* mapFind(map_t *map, char *key);
int mapFree(map_t *map);



#endif
