#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

int beginnum = 1000;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void* writer(void *arg)
{
    while(1)
    {
        pthread_rwlock_wrlock(&rwlock);
        printf("---%s---self---%lu----beginnum----%d\n",__FUNCTION__, pthread_self(), ++ beginnum);
        usleep(2000);
        pthread_rwlock_unlock(&rwlock);
        usleep(2000);
    }
    return NULL;
}

void* reader(void *arg)
{
    while(1)
    {
        pthread_rwlock_rdlock(&rwlock);
        printf("---%s---self---%lu----beginnum----%d\n",__FUNCTION__, pthread_self(),beginnum);
        usleep(2000);
        pthread_rwlock_unlock(&rwlock);
        usleep(2000);
    }
    return NULL;
}


int main()
{
    int n = 8, i = 0;
    pthread_t tid[8];
    
    for(i = 0 ; i < 5; i ++ )
    {
        pthread_create(&tid[i],NULL, reader, NULL);
    }

    for(      ;i < 8; i ++)
    {
        pthread_create(&tid[i],NULL,writer,NULL);
    }

    for(i = 0 ; i < 8; i ++ )
    {
        pthread_join(tid[i], NULL);
    }

}