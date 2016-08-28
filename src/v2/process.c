#include "all.h"

int Fork()
{
  int ret;
  if((ret = fork()) < 0)
    err_sys("fork error");
  return ret;
}
