#ifndef __HTTP_PARSE2__
#define __HTTP_PARSE2__

#include "http.h"

#define HTTPARSE_START 0
#define HTTPARSE_METHOD 1
#define HTTPARSE_REQURI 2
#define HTTPARSE_VER 3
#define HTTPARSE_HEADER_START 4
#define HTTPARSE_HEADER_CR 5
#define HTTPARSE_HEADER_KEY 6
#define HTTPARSE_HEADER_VAL1 7
#define HTTPARSE_HEADER_VAL2 8
#define HTTPARSE_HEADER_ONEEND 9
#define HTTPARSE_BODY 10
#define HTTPARSE_END 11

int parse_http_req(http_client_t *c);
int parse_http_reqline(http_client_t *c);
int parse_http_reqheader(http_client_t *c);
int parse_http_body(http_client_t *c);


#endif
