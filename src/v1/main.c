#include "all.h"

int main()
{
 int servfd = create_server(7989);
 char buf[4096];
 int max = 4096;
 printf("server[%d] start success\n ",7989 );
 
 for(;;)
 {
   int cfd = simple_accept(servfd);
   printf("accept suc\n");
   bzero(buf, sizeof(buf));
   int n = read(cfd, buf, max);
   buf[n] = '\0';
   printf("recv:%s", buf);
   writen(cfd, buf, n);
   close(cfd);
 }

 return 0;
}
