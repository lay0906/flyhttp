#ifndef _ALL_H_
#define _ALL_H_ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdarg.h>
#include <error.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "http_parse.h"

#define MAXLINE 4096
#define BACKLOG 10

void err_sys(char *fmt, ...);
int Socket(int, int, int);
int Bind(int,const struct sockaddr *, socklen_t);
int Listen(int, int);
int Accept(int, struct sockaddr *, socklen_t *);
int Close(int);
int simple_accept(int);
ssize_t readn(int, void *, size_t);
ssize_t writen(int, const void *, size_t);

int create_server(int);
int simple_connect(const char*, int);

int Fork();

int set_recv_timeout(int sockfd, int secs);
int recvn(int sockfd, void *buf, size_t nbytes);

#endif
