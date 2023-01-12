#include <stdio.h> 

int addToTop(int top)
{
    printf("enter addToTop\n");

    int sum = 0;
    int i = 0;
    for(i = 1; i <= top; i++)
    {
        sum += i;
    }
    printf("quit addToTop\n");
    return sum;
}

int main()
{
    int top = 100;
    int result = addToTop(top);
    printf("%d\n", result);
    return 0;
}

