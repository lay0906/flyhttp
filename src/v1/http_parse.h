#ifndef __HTTP_PARSE_H
#define __HTTP_PARSE_H

#include "map.h"


#define HTTP_VERSION_MAX_LENGTH 8
#define HTTP_METHOD_MAX_LENGTH 6

typedef enum http_req_method_e {
  GET = 0,
  POST,
  PUT,
  DELETE,
  HEADER
} http_req_method_t;

typedef struct http_req_support_method_s {
  http_req_method_t type;
  char *method_name;
  int method_length;
} http_req_support_method_t;

typedef struct http_req_reqline_s {
  char method:5;
  char version:3;
  char *req_uri;
} http_req_reqline_t;



typedef struct http_req_s {
  struct http_req_reqline_s *reqline;
  map_t *header;
} http_req_t;


int fly_strsecmp(const char *src, int n, const char *dest_s, const char *dest_ea);
void parse_error(int errcode);
int parse_http_reqline(http_req_t *req, char **cur, char **prev, int *n, int *s);
int parse_http_req(int sockfd, http_req_t *req, char **cur, char **prev, int *n, int *s);
int parse_http_reqheader(http_req_t *req, char **cur, char **prev, int *n, int *s);
int parse_http_reqbody(int sockfd, http_req_t *req, int *s);
int fly_strcpy(char *src, const char *dest_s, const char *dest_e);

#endif


