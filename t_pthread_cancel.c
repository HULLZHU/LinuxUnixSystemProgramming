#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

void* func(void *arg)
{
    while(1)
    {
        pthread_testcancel();
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    pthread_create(&tid, NULL, func, NULL);

    sleep(5);

    if(pthread_cancel(tid) != 0)
    {
        printf("Err: pthread_cancel()\n");
        exit(1);
    }

    void *ret;
    pthread_join(tid, &ret);
    
    printf("thread exit with %d\n", (int)ret);

    return 0;
}