#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void* func(void *arg)
{
    int num = (int) arg + 100;
    printf("I am thread %lu\n", pthread_self());
    return (void*)num;
}

int main()
{
    pthread_t tid[5];
    int i;
    
    for( i = 0 ; i < 5; i++)
    {
        pthread_create(&tid[i], NULL, func, (void*)i);
    }

    for( i = 0 ; i < 5; i++)
    {
        void* ret;
        pthread_join(tid[i], &ret);
        printf(" i == %d, retu == %d\n", i, (int)ret);
    }

    return 0;
}