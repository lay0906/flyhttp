#ifndef __HTTP__
#define __HTTP__

#include "map.h"
#include <sys/socket.h>


#define HTTP_RESP_OK 200

#define HTTP_RESP_BADREQUEST 400
#define HTTP_RESP_FORBIDDEN 403
#define HTTP_RESP_NOTFOUND 404
#define HTTP_RESP_NOT_SUPPORT 405

#define HTTP_METHOD_NOT_SUPPORT 0
#define HTTP_METHOD_GET 1
#define HTTP_METHOD_POST 2
#define HTTP_METHOD_DELETE 3
#define HTTP_METHOD_PUT 4
#define HTTP_METHOD_HEAD 5

#define HTTP_VERSION_1_1 1


typedef struct http_server_s
{
  int fd;
  int port;
  char *web_path;
} http_server_t;

typedef struct http_client_s
{
  int cfd;
  char *client_ip;
  unsigned int client_port; 
 
  char *in;
  char *out;

  char method:5;
  char version:3;
  char *req_uri;
  char *req_uri_end;

  map_t *header;
  map_t *args;
  map_t *cookies;

  char *body;  

  char *parse_cur;
  char *parse_prev;
  int parse_n;
  int parse_s;

  int resp_code;
} http_client_t;

void create_http_server(http_server_t *http_server);
void create_http_client(http_client_t *http_client, int confd, struct sockaddr *cliaddr);
char *get_header(http_client_t *c, char *key);
void put_header(http_client_t *c, char *key, char *val);





#endif

