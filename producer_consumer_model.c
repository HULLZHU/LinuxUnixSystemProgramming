/*************************************************************************\
*                  Copyright (C) He Zhu, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define USE_COND_VAR

/*TOTAL_COUNT means how many units will be produced and consumed*/
#define TOTAL_COUNT 20000

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

#ifdef USE_COND_VAR
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
#endif

static int consumed;
static int available;

void *producer(void *arg)
{
    int s;
    for (int j = 0; j < TOTAL_COUNT; j++)
    {
        s = pthread_mutex_lock(&mtx);
        if (s > 0)
        {
            printf("Err: pthread_mutex_lock %s\n", strerror(s));
            exit(EXIT_FAILURE);
        }

        /*begin of the critical region*/
        available++;
        printf("(%d)Producer: available = %d, consumed = %d \n", j, available, consumed);
        /*end of the critical */

        s = pthread_mutex_unlock(&mtx);
        if (s > 0)
        {
            printf("Err: pthread_mutex_unlock %s\n", strerror(s));
            exit(EXIT_FAILURE);
        }

    #ifdef USE_COND_VAR
        /*Wake sleeping consumer*/
        if( j % 10 == 0)
            s = pthread_cond_signal(&cond);
        if( s!= 0)
        {
            printf("Err: pthread_cond_signal %s\n",strerror(s));
            exit(EXIT_FAILURE);
        }
    #endif
    }

    return NULL;
}

void *consumer(void *arg)
{
    static int i;
    int s;
    while (consumed != TOTAL_COUNT)
    {
        s = pthread_mutex_lock(&mtx);
        if (s > 0)
        {
            printf("Err: pthread_mutex_lock %s\n", strerror(s));
            exit(EXIT_FAILURE);
        }
    
    #ifdef USE_COND_VAR
        while( available == 0)
        {
            s = pthread_cond_wait(&cond, &mtx);
            if( s > 0)
            {
                printf("Err: pthraed_cond_wait %s\n",strerror(s));
                exit(EXIT_FAILURE);
            }
        }
    #endif

        /*critical region begin*/
        while( available > 0 )
        {
            i++;
            consumed ++;
            available --;
            printf("(%d)Consumer: available = %d, consumed = %d \n", i, available, consumed);
        }

        s = pthread_mutex_unlock(&mtx);
        /*critical region end*/

        if (s > 0)
        {
            printf("Err: pthread_mutex_lock %s\n", strerror(s));
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    #ifdef USE_COND_VAR
    printf("producer Consumer model with Cond Vars\n");
    #else
    printf("Producer Consumer model without Cond Vars\n");
    #endif
    int s; /*used to track the error state*/

    pthread_t prod;
    pthread_t cons;

    s = pthread_create(&prod, NULL, producer, NULL);
    if (s > 0)
    {
        printf("Err: pthread_create %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    s = pthread_create(&cons, NULL, consumer, NULL);
    if (s > 0)
    {
        printf("Err: pthread_create %s\n", strerror(s));
        exit(EXIT_FAILURE);
    }

    pthread_join(prod,NULL);
    pthread_join(cons,NULL);
}