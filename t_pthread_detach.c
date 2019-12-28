#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
void* func(void *arg)
{
    printf("Start: Thread, tid = %lu\n", pthread_self());
    sleep(5);
    printf("End: Thread, tid = %lu\n", pthread_self());

    return NULL;
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, func, NULL);
    pthread_detach(tid);

    sleep(8);

    // int ret = 0;
    // if((ret = pthread_join(tid, NULL))> 0 )
    // {
    //     printf("Join Error: %d, %s", ret, strerror(ret));
    // }

    return 0;
}