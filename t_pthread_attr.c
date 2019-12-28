#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static void* func(void *x)
{
    return x;
}

int main(int argc, char *argv[])
{
    pthread_t thr;
    
    /*(1)Create and initialize a pthread attribute */
    pthread_attr_t attr;
    int s = pthread_attr_init(&attr);
    if(s!=0)
    {
        printf("Err: pthread_attr_init %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }
    
    /*(2)set the pthread attribute attr to be compatible with the detached thread*/
    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(s!=0)
    {
        printf("Err: pthread_attr_setdetachedstate() %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    /*(3)create the thread */
    s = pthread_create(&thr, &attr, func, (void*)1);
    if(s!=0)
    {
        printf("Err: pthread_create() %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    /*(4)destroy the attribute*/
    s = pthread_attr_destroy(&attr);
    if(s!=0)
    {
        printf("Err: pthread_attr_destruoy() %s\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}