#include "all.h"

http_server_t server;

char ip[20];

void initServer(http_server_t *pserv);
http_client_t *initClient(int cfd, struct sockaddr_in *cli);
void do_request(http_client_t *c);
void do_response(http_client_t *c);


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

void do_response(http_client_t *c)
{
   char *resp; 
   if(c->resp_code == HTTP_RESP_BADREQUEST){
      resp = "HTTP/1.1 400 Bad Request\r\n\r\n400 Bad Request\r\n";
   }
   else if(c->resp_code == HTTP_RESP_NOT_SUPPORT){
      resp = "HTTP/1.1 404 Page Not Found\r\n\r\n404 Page Not Found\r\n";
   }
   else {
      resp = "HTTP/1.1 200 OK\r\n\r\n200 OK\r\n";
   }
   writen(c->cfd, resp, strlen(resp));
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






