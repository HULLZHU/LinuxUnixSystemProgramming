#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mtx  = PTHREAD_MUTEX_INITIALIZER;
static int glob = 0;

static void cleanupHandler(void *arg)
{
    int s;

    printf("cleanup: freeing block at %p\n",arg);
    free(arg);

    printf("Cleanup: unlocking mutex \n");
    s = pthread_mutex_unlock(&mtx);
    if(s != 0 )
    {
        printf("Err: pthread_mutex_unlock %s\n",strerror(s));
        exit(EXIT_FAILURE);
    }
}

static void* threadFunc(void *arg)
{
    int s;
    void *buf = NULL;                                               /*Declare a pointer to a buffer of the thread*/

    buf = malloc(0x10000);                                          /*Not a cancellation point; allocate a memeory, size is 0x10000 bytes*/
    printf("thread: allocated memory at %p\n", buf);

    s = pthread_mutex_lock(&mtx);                                   /*Not a cancellation point*/

    if(s!=0)
    {
        printf("Err: pthread_mutex_lock %s\n",strerror(s));
        exit(EXIT_FAILURE);
    }

    pthread_cleanup_push(cleanupHandler,buf);                       /*Push a handler into the handler stack*/
    
    while(glob == 0)
    {
        s = pthread_cond_wait(&cond, &mtx);                         /*A cancellation point*/
        if( s!= 0 )
        {
            printf("Err: pthread_cond_wait %s\n", strerror(s));
            exit(EXIT_FAILURE);
        }
    }

    printf("Thread: condition wait loop completed\n");
    /*IF POP_TYPE != 0, the cleanup_pop() will execute unconditionally;*/
    /*IF POP_TYPE == 0, the cleanup_pop() will only be executed when the thread is cancelled*/
#define POP_TYPE 1
    pthread_cleanup_pop(POP_TYPE);                                         /*Execute cleanup handler*/
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t thr;
    void *result;
    int s;

    s = pthread_create(&thr, NULL, threadFunc, NULL);

    if(s!=0)
    {
        printf("Err: pthread_create %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    sleep(3);                                                       /*Give the thread chance to start*/

    if(argc == 1)                                                   /*Cancel the thread*/
    {
        printf("main: about to cancel the thread\n");
        s = pthread_cancel(thr);
        if( s!= 0 )
        {
            printf("Err: pthread_cancel %s\n",strerror(s));
            exit(EXIT_FAILURE);
        }
    }
    else                                                            /*terminate the thread normally*/
    {
        printf("main: about to signal condition variable\n");
       
        s = pthread_cond_signal(&cond);
        if( s!= 0)
        {
            printf("Err: pthread_cond_signal %s\n",strerror(s));
            exit(EXIT_FAILURE);
        }
         glob = 1;
    }

    s = pthread_join(thr, &result);

    if( s!= 0)
    {
        printf("Err: pthread_join %s\n",strerror(s));
        exit(EXIT_FAILURE);
    }

    if( result == PTHREAD_CANCELED)
    {
        printf("main: thread was canceled\n");
    }

    else
    {
        printf("main: thread terminated normally\n");
    }

    exit(EXIT_FAILURE);
}


