#include "all.h"


#define space(x) isspace((int)x)


void echo(int);
void processRequest(char *);
void processRequestLine(char *);

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


void processRequest(char *buf)
{
  char *s_buf = buf;
  if(buf)
   processRequestLine(buf);
}

void processRequestLine(char *buf)
{
  char *p = buf;
  char method[10],path[256],protocol[10];

  while(space(*buf)) buf++;

  p = buf;
  while(!space(*buf)) buf++;
  memcpy(method, p, buf-p);
  method[buf-p] = 0;
  printf("method:%s\n", method);

  buf++;
  p = buf;
  while(!space(*buf)) buf++;
  memcpy(path, p, buf-p);
  path[buf-p] = 0;
  printf("path:%s\n", path);
  
  buf++;
  p = buf;
  while(*buf != '\r') buf++;
  memcpy(protocol, p, buf-p);
  protocol[buf-p] = 0;
  printf("protocol:%s\n", protocol);

  buf++;
}



void echo(int fd)
{
  ssize_t n;
  char buf[MAXLINE];
again:
  n = read(fd, buf, MAXLINE);
  if(n < 0 && errno == EINTR)
     goto again;
  else if(n < 0)
     err_sys("echo server read error");
  buf[n] = 0;

  processRequest(buf);
}





