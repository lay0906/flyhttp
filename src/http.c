#include "http.h"
#include <stdlib.h>

char *get_header(http_client_t *c, char *key)
{
   if(c == NULL || c->header == NULL) return NULL;
   fly_tolower(key);
   return map_get(c->header, key); 
}

void put_header(http_client_t *c, char *key, char *val)
{
   if(c == NULL || c->header == NULL) return ;
   fly_tolower(key);
   map_put(c->header, key, val);
}
