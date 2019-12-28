#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>

#define SLEEP

void* func(void *arg)
{
    printf("I am a thread; pid = %d, POSIX Thread Id = %ld, Kernel Thread Id = %ld\n", getpid(), pthread_self(), syscall(SYS_gettid));
    return NULL;
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, func, NULL);
    printf("I am a main thread; pid = %d \n", getpid());

#ifdef SLEEP    
    sleep(1);
#endif
    
    return 0;
}