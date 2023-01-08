#include <stdio.h>
#define M 100
#define Print

int main()
{
  printf("%d\n", M);
  
  /*
  printf("%d\n", M);
  printf("%d\n", M);
  printf("%d\n", M);
  printf("%d\n", M);
  printf("%d\n", M);
  printf("%d\n", M);
  printf("%d\n", M);
  printf("%d\n", M);
  printf("%d\n", M);
  printf("%d\n", M);
  */
#ifdef Print 
  printf("Print");
#else 
  printf("None"); 
#endif 

  return 0;
}
