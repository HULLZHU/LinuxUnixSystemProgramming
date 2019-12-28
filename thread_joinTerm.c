#define _GNU_SOURCE
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define SLEEPTIME 3
#define THREADNUM 5

static int termNum = 0;
static int joinedNum = 0;

void errHandler(int s)
{
    printf("Err: %s\n", strerror(s));
    exit(EXIT_FAILURE);
}

/*status of a thread: alive ==> terminated ==> join*/
enum tstatus
{
    TS_ALIVE,
    TS_TERMINATED,
    TS_JOIN
};

static struct
{
    int struct_id;
    pthread_t tid;
    enum tstatus status;
} * threads;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t sigTerm = PTHREAD_COND_INITIALIZER;

void *simulation(void *arg)
{
    int idx = (int)arg;
    int s;

    printf("Thread %d is simulating: %ld\n", idx, pthread_self());
    threads[idx].status = TS_ALIVE;
    threads[idx].struct_id = idx;

    sleep(SLEEPTIME); /*simulate the work*/
    fflush(stdout);
    s = pthread_mutex_lock(&mtx);
    if (s != 0)
        errHandler(s);
    /*CRITICAL REGION BEGIN*/
    
    threads[idx].status = TS_TERMINATED; /*terminate after the job*/
    termNum++;
    
    /*CRITICAL REGION END*/
    s = pthread_mutex_unlock(&mtx);
    if (s != 0)
        errHandler(s);

    /*COND VAR SIGNAL*/
    s = pthread_cond_signal(&sigTerm);
    if (s != 0)
        errHandler(s);
}

void *joiner(void* arg)
{
    while (joinedNum != THREADNUM)
    {
        int s;
        /*(1) lock*/
        s = pthread_mutex_lock(&mtx);
        static int count = 1;
        if (s != 0)
            errHandler(s);

        /*(2) loop cond_wait*/
        while (termNum == 0)
        {
            s = pthread_cond_wait(&sigTerm, &mtx);
            if (s != 0)
                errHandler(s);
        }

        /*(3) core function*/

        for (int idx = 0; idx < THREADNUM; idx++)
        {
            if (threads[idx].status == TS_TERMINATED)
            {   
                s = pthread_join(threads[idx].tid, NULL);
                //if (s != 0)
                    //continue;
                threads[idx].status = TS_JOIN;
                printf("(%d)Joiner: Thread %d is joined : %ld, terminated = %d, joined = %d\n", count++, idx, threads[idx].tid,--termNum,++joinedNum);
            }
        }

        /*(4) unlock*/
        s = pthread_mutex_unlock(&mtx);
        if (s != 0)
            errHandler(s);
    }

    return NULL;
}

int main(int argc, char *argv)
{
    int idx, s;

    /*initialize the threads*/
    threads = calloc(THREADNUM, sizeof(*threads));

    pthread_t join;
    s = pthread_create(&join, NULL, joiner, NULL);
    if (s > 0)
        errHandler(s); 

    for (idx = 0; idx < THREADNUM; idx++)
    {
        sleep(2);
        s = pthread_create(&threads[idx].tid, NULL, simulation, (void *)idx);
        if (s > 0)
            errHandler(s);
    }

    s = pthread_join(join, NULL);
    if (s > 0)
    {
            errHandler(s);
    }
   
    exit(EXIT_SUCCESS);
}
