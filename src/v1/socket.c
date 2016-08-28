#include "all.h"

int Socket(int family, int type, int protocol)
{
 int fd;
 fd = socket(family, type, protocol);
 if(fd < 0)
  err_sys("create socket error");
 return fd;
}

int Bind(int fd, const struct sockaddr *servaddr, socklen_t len)
{
 int ret;
 if((ret = bind(fd, servaddr, len) < 0))
  err_sys("bind error");
 return ret;
}

int Listen(int fd, int backlog)
{
  int ret;
  if((ret = listen(fd, backlog))<0)
   err_sys("listen error");
  return ret;
}

int create_server(int port)
{
  int fd = Socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in servaddr;
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  Bind(fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
  Listen(fd, BACKLOG);

  return fd;
}

int Accept(int fd, struct sockaddr *cliaddr, socklen_t *addrlen)
{
  int ret;
  if((ret = accept(fd, cliaddr, addrlen)) < 0)
    err_sys("accept error");
  return ret;
}

int simple_accept(int fd)
{
  struct sockaddr_in cliaddr;
  int len;
  return Accept(fd, (struct sockaddr *)&cliaddr, &len);
}

ssize_t readn(int fd, void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nread;
  char *ptr = vptr;
  nleft = n;
  
  while(nleft > 0)
  {
    if((nread = read(fd, ptr, nleft)) < 0){
      if(errno == EINTR){
        nread = 0;
      }
      else
        return -1;
    }else if(nread == 0)
      break;
    nleft -= nread;
    ptr += nread;
  }
  return (n - nleft);
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr = vptr;
  nleft = n;
  
  while(nleft > 0){
    if((nwritten = write(fd, ptr, nleft)) <= 0 ){
      if(errno == EINTR && nwritten < 0)
         nwritten = 0;
      else
         return -1; 
    }
    nleft -= nwritten;
    ptr += nwritten;
  }
  return n;
}
























