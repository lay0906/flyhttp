#include "all.h"

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

void echo(int fd)
{
  ssize_t n;
  char buf[MAXLINE];
again:
  while((n = read(fd, buf, MAXLINE)) > 0)
     writen(fd, buf, n);
  if(n < 0 && errno == EINTR)
     goto again;
  else if(n < 0)
     err_sys("echo server read error");
}





