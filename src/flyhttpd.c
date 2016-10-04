#include "all.h"
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>


http_server_t server;

char ip[20];

void initServer(http_server_t *pserv);
http_client_t *initClient(int cfd, struct sockaddr_in *cli);
void do_request(http_client_t *c);
void do_response(http_client_t *c);
void do_ok(http_client_t *c);


int main()
{  
   initServer(&server);
   printf("server [%d] start success\n", server.port);  

   pid_t pid;
   char ip[20];
   struct sockaddr_in *cli;
   while(1){
      cli = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
      memset(cli, 0, sizeof(struct sockaddr_in));
      int cfd = simple_accept2(server.fd, cli);  

      if((pid = Fork()) == 0){
 //         Close(server.fd);
          http_client_t *client = initClient(cfd, cli);
          do_request(client);
          free(client);
      }      
      else
         Close(cfd);
   }
   return 0;
}

void do_request(http_client_t *c)
{
  printf("cli ip=%s, port=%d\n", c->client_ip, c->client_port); 
  set_recv_timeout(c->cfd, 90);
  c->in = (char *)malloc(sizeof(char)*1024);
  c->parse_prev = c->parse_cur = c->in;
  c->resp_code = 0;
again:
  c->parse_n = read(c->cfd, c->in, 1024);
  c->resp_code = parse_http_req(c);
  if(!c->resp_code)
     goto end;
  if(c->parse_s == HTTPARSE_END)
    goto end;
  else
    goto again;

end:
   printf("method=%d,version=%d,req_uri=%s\n", c->method, c->version, c->req_uri);
   do_response(c);
   Close(c->cfd);
}


void do_resp(const char *msg, http_client_t *c)
{
   writen(c->cfd, msg, strlen(msg));
}

void do_response(http_client_t *c)
{
   char *resp; 
   if(c->resp_code == HTTP_RESP_BADREQUEST){
      resp = "HTTP/1.1 400 Bad Request\r\n\r\n400 Bad Request\r\n";
      do_resp(resp,c);
      return ;
   }
   else if(c->resp_code == HTTP_RESP_NOT_SUPPORT){
      resp = "HTTP/1.1 405 Method Not Allowed\r\n\r\n405 Method Not Allowed\r\n";
      do_resp(resp,c);
      return ;
   }else if(c->resp_code == HTTP_RESP_NOTFOUND){
     resp = "HTTP/1.1 404 Page Not Found\r\n\r\n404 Page Not Found\r\n";
     do_resp(resp,c);
     return ;
   }else if(c->resp_code == HTTP_RESP_FORBIDDEN){
     resp = "HTTP/1.1 403 Forbidden\r\n\r\n403 Forbidden\r\n";
     do_resp(resp,c);
     return ;
   }
   else {
      do_ok(c);
      return ;
   }
}

void do_ok(http_client_t *c)
{
  char *req_uri = c->req_uri;
  char *resp;
  int filefd;
  if(req_uri == NULL){
     c->resp_code = HTTP_RESP_NOTFOUND;
     do_response(c); 
     return ;
  }
  parse_http_requri(c);
   
  char *path = (char *)malloc(sizeof(char) * (strlen(server.web_path) + c->req_uri_end - c->req_uri + 20));
  char *p = path, *q = server.web_path;
  while(*q!=0){
     *p++ = *q++;
  }
  for(q=c->req_uri;q<=c->req_uri_end;q++){
    *p++=*q;
  }
  *p = 0;
  printf("path=%s\n", path);
  

  int _l = strlen(path);
  if(path[_l-1]=='/'){
     path = strcat(path, "index.html");
  }

  printf("path=%s\n", path);
  struct stat st;
  stat(path, &st);
  
  if(errno == ENOENT){
     c->resp_code = HTTP_RESP_NOTFOUND;
     do_response(c);
     goto end;
  }else  if((filefd = open(path, O_RDONLY)) < 0){
     c->resp_code = HTTP_RESP_FORBIDDEN;
     do_response(c);
     goto end;
  }
  if(S_ISDIR(st.st_mode)){
     //301
     char *location = (char *)malloc(100);
     *location = 0;
     char *host = (char *)map_get(c->header, "Host");   
     strcat(location, "http://");
     strcat(location,host);
     strcat(location,c->req_uri);
     strcat(location, "/\r\n\r\n");
     

     char *msg= "HTTP/1.1 301 Move Permanently\r\nContent-Type: text/html\r\nLocation: ";
     write(c->cfd, msg, strlen(msg));
     write(c->cfd,location,strlen(location));
  }else{
     char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"; 
     write(c->cfd, msg, strlen(msg));

     char *buf = (char *)malloc(sizeof(char) * 1024);
     int n;
     while((n=read(filefd, buf, 1024)) > 0){
       write(c->cfd, buf, n);
     }
     Close(filefd);
  }
end:
  free(path);
}

http_client_t *initClient(int cfd, struct sockaddr_in *cli)
{
   http_client_t *client = (http_client_t *)malloc(sizeof(http_client_t));
   client->cfd = cfd;
   client->client_ip = (char *)inet_ntop(AF_INET, &cli->sin_addr, ip, 20);    
   client->client_port = ntohs(cli->sin_port);
 
   client->parse_s = HTTPARSE_START;
   client->parse_n = 0;
   client->parse_cur = NULL;
   client->parse_prev = NULL;
   client->in = NULL;
   client->out = NULL;
   
   return client;
}

void initServer(http_server_t *pserv)
{
   pserv->port = 8081;
   pserv->web_path = "/opt/web";
   pserv->fd = create_server(pserv->port);
}






