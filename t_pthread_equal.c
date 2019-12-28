#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* func(void* arg)
{
    printf("This is the thread\n");
}

int main()
{
    printf("This is the main\n");


    return 0;
}