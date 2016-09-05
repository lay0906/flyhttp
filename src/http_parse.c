#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "map.h"

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

const http_req_support_method_t support_method[] = {
 {GET, "GET", 3},
 {POST, "POST", 4},
 {PUT, "PUT", 3},
 {DELETE, "DELETE", 6},
 {HEADER, "HEADER", 6}
};

const int support_method_length =  sizeof(support_method)/sizeof(http_req_support_method_t);

typedef struct http_req_reqline_s {
  char method:5;
  char version:3;
  char *req_uri;
} http_req_reqline_t;



typedef struct http_req_s {
  struct http_req_reqline_s *reqline;
  map_t *header;
} http_req_t;


int fly_strsecmp(const char *src, const char *dest_s, const char *dest_e)
{
   while(dest_s < dest_e && (*src++ == *dest_s++)) ;
   return dest_s == dest_e;
}

void parse_error(int errcode)
{
   printf("parse error:%d\n", errcode);
   exit(1);
}


http_req_t * parse_request(char *http, int n)
{
  http_req_t *req = (http_req_t*)malloc(sizeof(http_req_t));

  char *cur = http;
  char *prev = cur;
  char *head_key;
  int i = 0, j;
  int s = 0;
  int len;
  char *tmp;
  while(i++ < n)
  { 
    switch(s){
      case 0:
        if(*cur == ' ' || *cur == '\t' || *cur == '\r' || *cur == '\n'){
           cur++;
           prev++;
        }else{
          prev = cur;
          cur++;
          s = 1;
        }
        break;
     case 1:
       if(*cur != ' '){
         cur++;
       }else{
          for(j=0;j<support_method_length;j++){
 	     if(fly_strsecmp(support_method[j].method_name, prev, cur)){
                req->reqline = (http_req_reqline_t *)malloc(sizeof(http_req_reqline_t));
                req->reqline->method = support_method[j].type;
                cur++;
                prev = cur;
                s = 2;
                break;
             }            
          }
          if(j == support_method_length) parse_error(501); 
       }
       break;
     case 2:
       if(*cur != ' ') {
         cur++;
       }else{
         req->reqline->req_uri = (char *)malloc(sizeof(char)*(cur-prev+1));
         strncpy(req->reqline->req_uri, prev, cur-prev);
         req->reqline->req_uri[cur-prev] = 0;
         cur++;
         prev = cur;
         s = 3;
       }
       break;
     case 3:
       if(*cur != '\n'){ 
         cur++;
       }else{
         if(*(cur-1) != '\r')
             parse_error(501);
         if(fly_strsecmp("HTTP/1.1", prev, cur-1)){
             req->reqline->version = 1;
             cur++;
             prev = cur;
             s = 4;
         }
         else
            parse_error(501);
       }
     default:
       break;
    }
  }
  return req;
}

int main()
{
  char *http = "DELETE /test.cgi HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";
  http_req_t *req = parse_request(http, strlen(http));
  printf("method=%s\n", support_method[req->reqline->method].method_name);
  printf("request_uri=%s\n", req->reqline->req_uri);
  return 0;
}
