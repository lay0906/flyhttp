#include "flystring.h"
#include <ctype.h>
#include <stdlib.h>
//#include <stdio.h>

void fly_strcpy(char *src, const char *dest_s, const char *dest_e)
{
  while(dest_s <  dest_e) *src++ = *dest_s++;
  *src = '\0';
}

int fly_strsecmp(const char *src, const char *dest_s, const char *dest_e)
{
  int n = dest_e - dest_s + 1, i;
  for(i = 0; i < n; i++)
  {
     if(*src == *dest_s){
         src++;
         dest_s++;
     }else
         break;
  }
  return dest_s == dest_e;
}

void fly_tolower(char *src)
{
}

/*
int main()
{
   char *s = "hello world";
   char *dest_s = &s[6];
   char *dest_e = &s[9];

   char *ps = "wor";
   int ret = fly_strsecmp(ps,dest_s, dest_e);

   printf("ret=%d\n", ret);

   char pss[20];
   fly_strcpy(pss, dest_s, dest_e);
   printf("pss=%s\n", pss);

   char *us = "HELLO WorlD";
   fly_tolower(us);
   printf("us=%s\n", us);
}
*/
