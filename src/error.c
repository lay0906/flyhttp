#include "all.h"

void err_sys(char *fmt, ...)
{
 va_list ap;
 va_start(ap, fmt);

 char buf[MAXLINE];
 vsnprintf(buf, MAXLINE, fmt, ap);
 
 snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s", strerror(errno));
 
 strcat(buf, "\n");
 fflush(stdout);
 fputs(buf, stderr);
 fflush(NULL);

 va_end(ap);
 exit(1);
}

