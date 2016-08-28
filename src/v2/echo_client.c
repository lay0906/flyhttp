#include "all.h"

void echo(int);

int main(int argc, char **argv)
{
  if(argc != 3)
   err_sys("usage: echo_client ip port");
  int fd = simple_connect(argv[1], atoi(argv[2]));
  echo(fd);
  return 0;
}

void echo(int fd)
{
  int n;
  char sbuf[MAXLINE], rbuf[MAXLINE];
  while(fgets(sbuf, MAXLINE, stdin) != NULL){
    writen(fd, sbuf, sizeof(sbuf));
    if((n = read(fd, rbuf, MAXLINE)) == 0)
      err_sys("echo_client:server terminated permaturely");
    sbuf[n] = 0;
    fputs(rbuf, stdout);
  }
}

