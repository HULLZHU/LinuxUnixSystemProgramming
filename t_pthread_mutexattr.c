#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

static volatile int glob = 0; 

pthread_mutex_t mtx;
pthread_mutexattr_t mtxAttr;

void* func1(void *arg)
{
    sleep(1);
    pthread_mutex_lock(&mtx);
}

void* func2(void *arg)
{
    sleep(3);
    pthread_mutex_unlock(&mtx);
}

int main(int argc, char *argv[])
{
    int s;
    int type;

    s = pthread_mutexattr_init(&mtxAttr);
    if( s != 0 )
    {
        printf("Err: pthread_mutex_init() %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_mutexattr_settype(&mtxAttr, PTHREAD_MUTEX_ERRORCHECK);
    if( s != 0 )
    {
        printf("Err: pthread_mutexattr_settype() %s\n",strerror(s));
        exit(EXIT_FAILURE);

    }

    s = pthread_mutex_init(&mtx,&mtxAttr);
    if( s != 0 )
    {
        printf("Err: pthread_mutex_init() %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    pthread_t thr1, thr2;

    s = pthread_create(&thr1,NULL,func1,NULL);
    if( s != 0 )
    {
        printf("Err: pthread_create() %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_create(&thr2,NULL, func2, NULL);
    if( s != 0 )
    {
        printf("Err: pthread_create() %s\n",strerror(s));
        exit(EXIT_FAILURE);
    }

    pthread_join(thr1, NULL);
    pthread_join(thr2, NULL);

    exit(EXIT_SUCCESS);
}