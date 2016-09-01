#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main()
{
 char *p = (char *)malloc(sizeof(char) * 9);
 char *q = (char *)realloc(p, sizeof(char) * 27);
 printf("p=%x\n", p);
 printf("q=%x\n", q);
 return 0;
}
