// 在C中有很多的字符 a.可显字符 b.控制字符
// /r：回车
// /n：换行
#include <stdio.h>
#include <unistd.h>

int main1()
{
  printf("Hello world\r");// 好像是先执行了sleep,然后再执行了printf 
  fflush(stdout);
  sleep(3);               // Hello world，没有被刷新，数据在sleep期间被保存起来。
  return 0;
}

int main2()
{
  int i = 10;
  for (; i >= 0; i--)
  {
    printf("%d\r", i);
    fflush(stdout);
    sleep(1);
  }
  return 0;
}

#include "proc.h"
int main()
{
  process();
  return 0;
}
