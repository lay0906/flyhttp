#include "all.h"


#define space(x) isspace((int)x)

#define respMsg(rspCode, rspMsg) do { \
      switch((rspCode)) { \
       case 404: \
          (rspMsg) = "404 Not Found"; break; \
       case 501: \
          (rspMsg) = "501 No Implemented"; break; \
       case 505: \
          (rspMsg) = "505 HTTP Version Not Supported"; break; \
       case 200: \
          (rspMsg) = "200 OK"; break; \
       default: \
          (rspMsg) = "400 Bad Request"; break; \
      } \
  }while(0)
       
         

extern const http_req_support_method_t support_method[];
extern const int support_method_length;
void echo(int);

int main()
{
 int servfd = create_server(7989);
 pid_t pid;
 printf("server[%d] start success\n ",7989 );
 for(;;)
 {
   int cfd = simple_accept(servfd);
   if((pid = Fork()) == 0){
     Close(servfd);
     echo(cfd);
     exit(0);
   }
   Close(cfd);
 }
 return 0;
}

void resp(int fd, int respCode)
{ 
  char *msg;
  respMsg(respCode, msg);
  writen(fd, msg, strlen(msg)); //@TODO:remove strlen
  Close(fd);
}

void resp_file(const char *path, int sockfd)
{
  char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"; 
  write(sockfd, msg, strlen(msg));
 
  char *buf = (char *)malloc(sizeof(char) * 1024);
  int n;
  char sssss[100]="/opt/web";
  strcat(sssss, path);
  int fd = open(sssss, O_RDONLY);
  while((n=read(fd, buf, 1024)) > 0){
    write(sockfd, buf, n);
  }
  Close(fd);
  Close(sockfd);
}



void echo(int fd)
{
  int sec = 90;
  set_recv_timeout(fd, sec);
  int n;
  int s = 0;
  char buf[MAXLINE];
  char *cur = buf, *prev = buf;
  http_req_t *req = (http_req_t *)malloc(sizeof(http_req_t));
again:
  n = read(fd, buf, MAXLINE);
  if(n < 0 && errno == EINTR)
     goto again;
  else if(n == 0)
     goto end;
  else {
     int ret = parse_http_req(fd, req, &cur, &prev, &n, &s);
     if(ret)
       resp(fd, ret);
     if(s>6)
        goto end;
     else
        goto again;
  }
end:
  if(s > 6){
      printf("request_uri=%s\n", req->reqline->req_uri);
      resp_file(req->reqline->req_uri, fd);
  }else{
     resp(fd, 400);
  }
}





