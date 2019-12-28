#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define CMPL_OPT
#ifndef CMPL_OPT
static volatile int glob = 0;
#else
static int glob = 0;
#endif



//#define USE_LOCK
#ifdef USE_LOCK
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif


void* threadFunc(void* arg)
{
    #ifdef USE_LOCK
    pthread_mutex_lock(&mutex);
    #endif
    
    int loops = *((int*)arg);

    for(int i = 0 ; i < loops; i ++ )
    {
        glob ++;
    }

    #ifdef USE_LOCK
    pthread_mutex_unlock(&mutex);
    #endif

    return NULL;
}

int main(int argc, char *argv[])
{
    struct timeval end, start;
    gettimeofday(&start,NULL);
    int loops;

    if(argc == 1)
    {
        loops = 1000;
    }

    if(argc > 1 )
    {
        loops = atoi(argv[1]);
        if(loops <= 0 )
        {
            printf("Err: atoi() loops must be > 0\n");
            exit(EXIT_FAILURE);
        }
    }

    pthread_t thr1, thr2;

    int s;          /*s is to track the error status*/

    s = pthread_create(&thr1, NULL, threadFunc, &loops); /*create the 1st thread*/
    if(s != 0)
    {
        printf("Err: pthread_create %s",strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_create(&thr2, NULL, threadFunc, &loops );
    if(s != 0)
    {
        printf("Err: pthread_create %s",strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_join(thr1,NULL);
    if(s != 0)
    {
        printf("Err: pthread join %s",strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_join(thr2, NULL);
    if(s != 0)
    {
        printf("Err: pthread join %s",strerror(s));
        exit(EXIT_FAILURE);
    }

    printf("Finally, glob = %d\n", glob);
    gettimeofday(&end,NULL);
    printf("Totally Running Time %lu us\n",  (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);
    exit(EXIT_SUCCESS);
}