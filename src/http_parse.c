#include "http_parse.h"
#include <string.h>
#include <stdlib.h>

int parse_http_req(http_client_t *c)
{
  int ret = 0;
  while(c->parse_n > 0){
  switch(c->parse_s){
    case HTTPARSE_START:
    case HTTPARSE_METHOD:
    case HTTPARSE_REQURI:
    case HTTPARSE_VER:
      ret = parse_http_reqline(c);
      break;
    case HTTPARSE_HEADER_START:
    case HTTPARSE_HEADER_CR:
    case HTTPARSE_HEADER_KEY:
    case HTTPARSE_HEADER_VAL1:
    case HTTPARSE_HEADER_VAL2:
    case HTTPARSE_HEADER_ONEEND:
      ret = parse_http_reqheader(c);
      break;
    case HTTPARSE_BODY:
      ret = parse_http_reqbody(c);
    default:
      return ret;
      break;
  }
 }
 if(c->parse_s == HTTPARSE_BODY && c->parse_n == 0) 
      c->parse_s = HTTPARSE_END;
  return ret;
}


static int _get_http_method(char *prev, char *cur)
{
  int type = 0;
  type = fly_strsecmp("GET", prev, cur);
  if(type == 1) return HTTP_METHOD_GET;
  
  type = fly_strsecmp("POST", prev, cur);
  if(type == 1) return HTTP_METHOD_POST;
 
  type = fly_strsecmp("DELETE", prev, cur);
  if(type == 1) return HTTP_METHOD_DELETE;
  
  type = fly_strsecmp("HEAD", prev, cur);
  if(type == 1) return HTTP_METHOD_HEAD;
  
  type = fly_strsecmp("PUT", prev, cur);
  if(type == 1) return HTTP_METHOD_PUT;

  return HTTP_METHOD_NOT_SUPPORT;
}


int parse_http_reqline(http_client_t *c)
{
   int r,l;
   while(c->parse_n > 0){
     switch(c->parse_s){
        case HTTPARSE_START:
          if(*c->parse_cur == ' ' || *c->parse_cur == '\t' || *c->parse_cur == '\r' || *c->parse_cur == '\n' ) {
            c->parse_cur++;
            c->parse_prev++;
          }else{
            c->parse_prev = c->parse_cur;
            c->parse_cur++;
            c->parse_s = HTTPARSE_METHOD;
          }
          break;
        case HTTPARSE_METHOD:
          if(*c->parse_cur != ' '){
             c->parse_cur++;
          }else{
             r = _get_http_method(c->parse_prev, c->parse_cur);
             if(r == HTTP_METHOD_NOT_SUPPORT) 
                return HTTP_RESP_NOT_SUPPORT;
             c->method = r;
             c->parse_cur++;
             c->parse_prev = c->parse_cur;
             c->parse_s = HTTPARSE_REQURI;
          }
          break;
        case HTTPARSE_REQURI:
          if(*c->parse_cur != ' ') {
             c->parse_cur++;
          }else{
             l = c->parse_cur - c->parse_prev;
             c->req_uri = (char *)malloc(sizeof(char)*(l+1));
             strncpy(c->req_uri, c->parse_prev, l);
             c->req_uri[l] = 0;
             c->parse_cur++;
             c->parse_prev = c->parse_cur;
             c->parse_s = HTTPARSE_VER;
          }
          break;
        case HTTPARSE_VER:
          if(*c->parse_cur != '\n'){ 
	     c->parse_cur++;             
          }else{
             if(*(c->parse_cur-1) != '\r')
                return HTTP_RESP_BADREQUEST;
             if(fly_strsecmp("HTTP/1.1", c->parse_prev, c->parse_cur-1)){
                  c->version = HTTP_VERSION_1_1;
                  c->parse_cur++;
                  c->parse_prev = c->parse_cur;
                  c->parse_s = HTTPARSE_HEADER_START;
              } else
                 return HTTP_METHOD_NOT_SUPPORT;
          }
          break;
         default: 
           return 0;
           break;
       }
       c->parse_n--;
    }
    return 0;
}

int parse_http_reqheader(http_client_t *c)
{
  int i=0;
  char *k, *v;
  while(c->parse_n > 0)
  {
    switch(c->parse_s){
      case HTTPARSE_HEADER_START:
        if(*c->parse_cur == '\r'){
           c->parse_s = HTTPARSE_HEADER_CR;
        }else {
           c->parse_s = HTTPARSE_HEADER_KEY;
        }
        c->parse_prev = c->parse_cur;
        c->parse_cur++;
        break;
      case HTTPARSE_HEADER_CR:
        if(*c->parse_cur == '\n') {
          c->parse_s = HTTPARSE_BODY;
          c->parse_cur++;
        }
        break;
      case HTTPARSE_HEADER_KEY:
        if(*c->parse_cur != ':') c->parse_cur++;
        else {
          k = (char *)malloc((c->parse_cur - c->parse_prev + 1)*sizeof(char));
          fly_strcpy(k, c->parse_prev, c->parse_cur);
          c->parse_cur++;
          c->parse_prev = c->parse_cur;
          c->parse_s = HTTPARSE_HEADER_VAL1;
        }
        break;
      case HTTPARSE_HEADER_VAL1:
        if(*c->parse_cur == ' ' || *c->parse_cur == '\t'){
           c->parse_cur++;
           c->parse_prev = c->parse_cur;
        }else if(*c->parse_cur == '\n'){
           if(*(c->parse_cur-1) != '\r') return HTTP_RESP_BADREQUEST;
           c->parse_s = HTTPARSE_HEADER_ONEEND;
        }else {
           c->parse_prev = c->parse_cur;
           c->parse_cur++;
           if(*c->parse_prev != '\r')
             c->parse_s = HTTPARSE_HEADER_VAL2;
        }
        break;
      case HTTPARSE_HEADER_VAL2:
        if(*c->parse_cur == '\n'){
           if(*(c->parse_cur-1) != '\r') 
		return HTTP_RESP_BADREQUEST;
           v = (char *)malloc((c->parse_cur - c->parse_prev - 1)*sizeof(char));
           fly_strcpy(v, c->parse_prev,(c->parse_cur - 1));
           if(c->header == NULL)
              c->header = map_create(10,NULL);
           put_header(c, k, v);
           c->parse_cur++;
           c->parse_prev = c->parse_cur;
           c->parse_s = HTTPARSE_HEADER_ONEEND;
        }else {
           c->parse_cur++;
        }
        break;
      case HTTPARSE_HEADER_ONEEND:
       if(*c->parse_cur == '\n'){
         if(*(c->parse_cur - 1) != '\r') return HTTP_RESP_BADREQUEST;
         c->parse_cur++;
         c->parse_prev = c->parse_cur;
         c->parse_s = HTTPARSE_BODY;
       }else{
         c->parse_prev = c->parse_cur;
         c->parse_cur++;
         if(*c->parse_prev != '\r')
            c->parse_s = HTTPARSE_HEADER_KEY;
       }
       break;
      default:
        return 0;
        break;
    }
    c->parse_n--;
  }
  return 0;
}

int parse_http_reqbody(http_client_t *c)
{
  if(c->parse_s != HTTPARSE_BODY)
    return HTTP_RESP_BADREQUEST;
  c->parse_s = HTTPARSE_END;
  char *length = get_header(c, "Content-Length");
  if(length == NULL)
    return 0;
  int l = atoi(length);
  if(l == 0)
    return 0;
  c->body = (char *)malloc(sizeof(char) * (l + 1));
  recvn(c->cfd, c->body, l);
  c->body[l] = 0;
  return 0;
}

int parse_http_requri(http_client_t *c)
{
  char *url = c->req_uri;
  if(url == NULL)
     return HTTP_RESP_NOTFOUND;
  if(*url != '/')
     return 0;
 
   int s = 0;
   char *p1,*p2,*cur, *k, *v, *prev;
   p1=p2=cur=url;
   while(*cur != 0){
      switch(s){
          case 0:
            if(*cur!='/'){
               return HTTP_RESP_NOTFOUND;
            }
            s = 1;
            break;
          case 1:
            if(*cur == ';'){
               s = 2;
               p2 = cur - 1;
            }else if(*cur == '?'){
               s = 3;
               p2 = cur - 1;
            }
            break;
          case 2:
            if(*cur == '?'){
               s = 3;
            }
            break;
          case 3:
            s = 4;
            prev = cur;
            break;
           case 4:
            if(*cur == '='){
              k = (char *)malloc(sizeof(char)*(cur - prev));
              fly_strcpy(k, prev, cur);
              s = 5;
            }
            break;
          case 5:
            if(*cur == '&'){
               s = 3;
               break;
            }
            s = 6;
            prev = cur;
            break;
          case 6:
            if(*cur == '&' || *cur == '#'){
              if(prev != cur){
                  v = (char *)malloc(sizeof(char)*(cur - prev));
                  fly_strcpy(v, prev, cur);
                  if(c->args == NULL){
                     c->args = map_create(10,NULL);
                  }     
                  map_put(c->args, k , v);                 
                  *cur == '&' ? (s = 3) : (s = 7);
              }
            }
            break;
      }
      if(s == 7) break;
      cur++;
   }
   if(s != 7){
     c->req_uri_end = cur - 1;
   }else
     c->req_uri_end = p2;
   return 0;
}










