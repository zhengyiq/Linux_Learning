#include "proc.h"
#include <string.h>

#define SIZE 101
#define STYLE '='
#define ARR '>'
void process()
{
    const char* label = "/|\\-";
    char bar[SIZE];
    memset(bar, '\0', sizeof(bar));
    int i = 0;
    while (i <= 100)
    {
        // printf("[%-100s][%d%%][%c]\r", bar, i, label[i%4]);
        printf("[\033[1;34;42m%-100s\033[0m][%d%%][%c]\r", bar, i, label[i%4]);
        // printf("[\033[1;34;42m%s\033[0m][%d%%][%c]\r", bar, i, label[i%4]);
        fflush(stdout);
        bar[i++] = STYLE;
        if (i != 100) bar[i] = ARR;
        usleep(100000);
    }
    printf("\n");
}
