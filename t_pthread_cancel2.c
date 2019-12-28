#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
static void* func(void *arg)
{
    int j;

    printf("New Thread Started\n");
    for( j = 1; ; j ++ )
    {
        printf("Loop %d\n",j);
        sleep(1);
    }

    /*NOT REACHED*/
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t thr;              /*declare a pthread_t type */  
    int s;                      /*s is used to debug*/
    void *result;               /*used to retrieve the result*/

    s = pthread_create(&thr, NULL, func, NULL);
    if( s!= 0 )
    {
        printf("Err: pthread_create %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    sleep(3);                   /*Allow new thread to run a while*/

    s = pthread_cancel(thr);    /*Cancel the infinite thread*/
    if( s!= 0 )
    {
        printf("Err: pthread_cancel %s\n",strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_join(thr, &result);
    if( s != 0 )
    {
        printf("Err: pthread_join %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    if(result == PTHREAD_CANCELED)
    {
        printf("The thread is canceled\n");
    }
    else
    {
        printf("The thread is not canceled\n");
    }

    exit(EXIT_SUCCESS);
}