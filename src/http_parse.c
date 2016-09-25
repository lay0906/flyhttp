#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "http_parse.h"

const http_req_support_method_t support_method[] = {
 {GET, "GET", 3},
 {POST, "POST", 4},
 {PUT, "PUT", 3},
 {DELETE, "DELETE", 6},
 {HEADER, "HEADER", 6}
};

const int support_method_length =  sizeof(support_method)/sizeof(http_req_support_method_t);






int fly_strsecmp(const char *src, int n, const char *dest_s, const char *dest_e)
{
  int dn = dest_e - dest_s, i;
  n = (n >= dn) ? n : dn;
  for(i = 0; i < n; i++)
  {
     if(*src == *dest_s){
         src++;
         dest_s++;
     }else
         break;
  }
  return dest_s == dest_e;
}


int fly_strcpy(char *src, const char *dest_s, const char *dest_e)
{
  while(dest_s <  dest_e) *src++ = *dest_s++;
  *src = '\0';
  return 0;
}

void parse_error(int errcode)
{
   printf("parse error:%d\n", errcode);
   exit(1);
}




int parse_http_reqline(http_req_t *req, char **cur, char **prev, int *n, int *s)
{
   int i=0,l,j;
   while(*n > 0){
     switch(*s){
        case 0:
          if(**cur == ' ' || **cur == '\t' || **cur == '\r' || **cur == '\n' ) {
            (*cur)++;
            (*prev)++;
          }else{
            *prev = *cur;
            (*cur)++;
            *s = 1;
          }
          break;
        case 1:
          if(**cur != ' '){
             (*cur)++;
            #ifdef HTTP_METHOD_MAX_LENGTH
             if(*cur - *prev > HTTP_METHOD_MAX_LENGTH) return 400;
            #endif
          }else{
            for(j=0;j<support_method_length;j++){
 	       if(fly_strsecmp(support_method[j].method_name, support_method[j].method_length, *prev, *cur)){
                  req->reqline = (http_req_reqline_t *)malloc(sizeof(http_req_reqline_t));
                  req->reqline->method = support_method[j].type;
                  (*cur)++;
                  *prev = *cur;
                  *s = 2;
                  break;
               }            
             }
             if(j == support_method_length) 
                return 501;
          }
          break;
        case 2:
          if(**cur != ' ') {
             (*cur)++;
          }else{
             l = *cur - *prev;
             req->reqline->req_uri = (char *)malloc(sizeof(char)*(l+1));
             strncpy(req->reqline->req_uri, *prev, l);
             req->reqline->req_uri[l] = 0;
             (*cur)++;
             *prev = *cur;
             *s = 3;
          }
          break;
        case 3:
          if(**cur != '\n'){ 
              (*cur)++;
          }else{
              if(*(*cur-1) != '\r')
                  return 501;
              if(fly_strsecmp("HTTP/1.1",8, *prev,*cur-1)){
                  req->reqline->version = 1;
                  (*cur)++;
                  *prev = *cur;
                  *s = 4;
               } else
                  return 502;
          }
          break;
         default: 
           return 0;
           break;
       }
       *n = *n - 1;
    }
    return 0;
}

int parse_http_reqheader(http_req_t *req, char **cur, char **prev, int *n, int *s)
{
  int i=0;
  char *k, *v;
  while(*n > 0)
  {
    switch(*s){
      case 4:
        if(**cur == '\r'){
           *s = 5;
        }else {
           *s = 6;
        }
        *prev = *cur;
        (*cur)++;
        break;
      case 5:
        if(**cur == '\n') {
          *s = 10;
          (*cur)++;
        }
        break;
      case 6:
        if(**cur != ':') (*cur)++;
        else {
          k = (char *)malloc((*cur-*prev + 1)*sizeof(char));
          fly_strcpy(k, *prev, *cur);
          (*cur)++;
          *prev = *cur;
          *s = 7;      
        }
        break;
      case 7:
        if(**cur == ' ' || **cur == '\t'){
           (*cur)++;
           *prev = *cur;
        }else if(**cur == '\n'){
           if(*(*cur-1) != '\r') return 400;
           *s = 9;
        }else {
           *prev = *cur;
           (*cur)++;
           if(**prev != '\r')
             *s = 8;
        }
        break;
      case 8:
        if(**cur == '\n'){
           if(*(*cur-1) != '\r') return 400;
           v = (char *)malloc((*cur - *prev - 1)*sizeof(char));
           fly_strcpy(v, *prev,(*cur - 1));
           printf("%s=%s\n", k, v);
           if(req->header == NULL)
              req->header = map_create(10,NULL);
           map_put(req->header, (void*)k, (void*)v);
           (*cur)++;
           *prev = *cur;
           *s = 9;
        }else {
           (*cur)++;
        }
        break;
      case 9:
       if(**cur == '\n'){
         if(*(*cur - 1) != '\r') return 400;
         (*cur)++;
         *prev = *cur;
         *s = 10;
       }else{
         *prev = *cur;
         (*cur)++;
         if(**prev != '\r')
            *s = 6;
       }
       break;
      default:
        return 0;
        break;
    }
    *n = *n - 1;
  }
  return 0;
}

int parse_http_reqbody(int sockfd, http_req_t *req, int *s)
{
  if(*s != 10)
    return 400;
  *s = 11;
  void *length = map_get(req->header, "Content-Length");
  if(length == NULL)
    return 0;
  int l = atoi((char *)length);
  if(l == 0)
    return 0;
  char *buf = (char *)malloc(sizeof(char) * (l + 1));
  recvn(sockfd, buf, l);
  buf[l] = 0;
  return 1;
}


int parse_http_req(int sockfd, http_req_t *req, char **cur, char **prev, int *n, int *s)
{
  int ret = -1;
  char *o = *cur;
  while(*n > 0){
  switch(*s){
    case 0:
    case 1:
    case 2:
    case 3:
      ret = parse_http_reqline(req, cur, prev, n, s);
      break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      ret = parse_http_reqheader(req, cur, prev, n ,s);
      break;
    case 10:
      ret = parse_http_reqbody(sockfd, req, s);
    default:
      return -1;
      break;
  }
 }
  if(*s ==10 && *n ==0) *s=11;
  return ret;
}


static int _rand(int n)
{
   return rand()%n + 1;
}

static int _read(char *src,char *dest, int l)
{
  if(src == dest) return 0;
  
  int n = rand()%l + 1;
  
  if(src + n > dest)
    return dest-src;
  
  return n;
}


/*
int main()
{

  char *http = "PUT /tes1t.cgi HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";

  http_req_t *req = (http_req_t *)malloc(sizeof(http_req_t));
  int i, l = strlen(http), n, s = 0;
  char *cur = http, *prev = http, *last = http + l;
  srand(time(NULL));

  while((n = _read(cur, last, l)) > 0)
  {  
    if(parse_http_req(req, &cur, &prev, n, &s))
        break;    
  }

  printf("method=%s\n", support_method[req->reqline->method].method_name);
  printf("request_uri=%s\n", req->reqline->req_uri);
  printf("http version=%d\n", req->reqline->version);
  return 0;
}*/
