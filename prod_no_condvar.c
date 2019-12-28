#define _GNU_SOURCE
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static pthread_mutex_t mtx;

/*available locks*/
static int avail = 0;
int numConsumed = 0; /*Total Units so far consumed*/

/*producer*/
static void *producer(void *arg)
{
    int cnt = atoi((char *)arg);
    int s, j;

    for (j = 0; j < cnt; j++)
    {
        //sleep(1);

        s = pthread_mutex_lock(&mtx);
        if (s != 0)
        {
            printf("Err: pthread_mutex_lock() %s\n", strerror(s));
            exit(EXIT_FAILURE);
        }

        avail++; /*Let consumer know another unit is available*/
        printf("Producer:available unit = %d, consumed unit = %d\n", avail,numConsumed);
        

        s = pthread_mutex_unlock(&mtx);

        if (s != 0)
        {
            printf("Err: pthread_mutex_lock() %s\n", strerror(s));
            exit(EXIT_FAILURE);
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    int s, j;
    int totalRequired = 0; /*Total number of units that all threads will produce*/

    
    int done = 0;
    time_t t;

    /*Create all threads*/
    for (j = 1; j < argc; j++)
    {
        totalRequired += atoi(argv[j]);
        s = pthread_create(&tid, NULL, producer, argv[j]);
        if( s!= 0)
        {
            printf("Err: pthread_create() %s\n",strerror(s));
            exit(EXIT_FAILURE);
        }
    }

    /*Use a polling loop to check for available units*/

    while(1)
    {
        s = pthread_mutex_lock(&mtx);
        if( s!= 0)
        {
            printf("pthread_mutex_lock %s\n",strerror(s));
            exit(EXIT_FAILURE);
        }

        while( avail > 0)
        {
            /*do something with produced unit */
            numConsumed ++; 
            avail --;

            printf("Consumer: avalible unit = %d, consumed unit =%d\n", avail, numConsumed);
            done = numConsumed >= totalRequired;
        } 

        s = pthread_mutex_unlock(&mtx);
        
        if(s != 0)
        {
            printf("Err: pthread_mutex_unlock %s\n",strerror(s));
            exit(EXIT_FAILURE);
        }

        if (done)
        {
            break;
        }
    }
}