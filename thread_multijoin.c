#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static pthread_cond_t threadDied = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;

static int totThreads = 0;              /*Total number of threads created*/
static int numLive = 0;                 /*Total number of threads still alive or terminated but not yet joined*/

static int numUnjoined = 0;             /*Total number of threads terminated have not yet been joined*/

void errHandler(const int err)
{
    printf("Err: %s\n", strerror(err));
    exit(EXIT_FAILURE);
}

enum tstate                             /*thread state*/
{
    TS_ALIVE,                           /*thread is alive*/
    TS_TERMINATED,                      /*thread is terminated, not yet joined*/
    TS_JOINED                           /*thread terminated, and joined*/
};

/*Define a pointer to a struct as following*/
static struct                           /*Info about each thread*/
{
    pthread_t tid;                      /*ID of this thread*/
    enum tstate state;                  /*Thread State*/
    int sleepTime;                      /*Number of second to live before terminating*/
}*thread;

static void* threadFunc(void *arg)
{
    int idx = (int)arg;
    int s;
    sleep(thread[idx].sleepTime);
    printf("Thread %d terminating \n", idx);

    /*LOCK(MUTEX)*/
    s = pthread_mutex_lock(&threadMutex);
    if(s != 0 )errHandler(s);

    numUnjoined ++ ;
    thread[idx].state = TS_TERMINATED;

    /*UNLOCK(MUTEX)*/
    s = pthread_mutex_unlock(&threadMutex);
    if(s != 0 )errHandler(s);

    /*SIGNAL THREAD_DIED*/
    s = pthread_cond_signal(&threadDied);
    if( s!= 0)errHandler(s);

    return NULL;
}

int main( int argc, char *argv[])
{
    int s, idx;

    if(argc < 2 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s nsecs...\n", argv[0]);
    }

    /*Allocate a block of memory for an array for argc-1 elements.
    Each of them is sizeof(thread) bytes. After all, initialize 
    all of them to zero*/
    thread = calloc(argc - 1, sizeof(thread));
    if(thread == NULL)errHandler(ENOMEM);

    /*Create all threads*/
    for( idx = 0 ; idx < argc - 1; idx ++ )
    {   
        //set the thread[idx] sleep time, and 5 seconds is a default value
        thread[idx].sleepTime = atoi(argv[idx + 1]) > 0 ? atoi(argv[idx + 1]):5;
        thread[idx].state = TS_ALIVE;
        s = pthread_create(&thread[idx].tid,NULL,threadFunc, (void*)idx);
        if(s != 0) errHandler(s);
    }

    totThreads = argc - 1;
    numLive = totThreads;

    /*Join with terminated threads*/

    while( numLive > 0 )
    {
        s = pthread_mutex_lock(&threadMutex);
        if (s!=0) errHandler(s);

        while( numUnjoined == 0 )
        {
            s = pthread_cond_wait(&threadDied, &threadMutex);
            if( s != 0)errHandler(s);
        }

        for( idx = 0 ; idx < totThreads ; idx ++ )
        {
            if(thread[idx].state == TS_TERMINATED)
            {
                s = pthread_join(thread[idx].tid, NULL);
                if( s!=0 )errHandler(s);
            } 
            thread[idx].state = TS_JOINED;
            numLive --;
            numUnjoined --;
            printf("Join thread %d (numLive = %d)\n", idx, numLive);
        }

        s = pthread_mutex_unlock(&threadMutex);
        if(s!=0)errHandler(s);
    }
    exit(EXIT_SUCCESS);
}