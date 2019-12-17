//gcc simpleSuspend.c  -o simpleSuspend -I include/ -L lib/ -lTLPI -lSig
#define _GNU_SOURCE
#include "tlpi_hdr.h"
#include "error_functions.h"
#include "signal_functions.h"
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

void handler(int signo)
{
    if( signo == SIGUSR1 || signo == SIGUSR2 || signo == SIGINT)
        printf("caught signal %d (%s)\n", signo, strsignal(signo));
    return;
}

int main( int argc, char * argv[])
{
    printf("PID=%d\n", getpid());

    sigset_t blockSet;
    sigset_t prevSet;

    sigemptyset(&blockSet);
    sigaddset(&blockSet,SIGUSR1);
    sigaddset(&blockSet,SIGUSR2);
    sigaddset(&blockSet,SIGINT);

    sigprocmask(SIG_BLOCK, &blockSet, &prevSet);

    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);


    sigset_t pendings;
    
    sleep(5);

    if(sigpending(&pendings) == -1)
    {
        printf("Error: sigpending\n");
    }
    for(   int i = 0 ; i < NSIG ; i ++  )
    {
        if(sigismember(&pendings, i))
        {
            printf("Pending Siganl %d (%s)\n", i, strsignal(i));
        }
    }

    
    //if sigsuspend is good, return -1, errno = EINTR
    if( sigsuspend(&prevSet) == -1 && errno != EINTR)
    {
        printf("Error: sigsuspend\n");
        exit(EXIT_FAILURE);
    }

    printf("After sigsuspend(), pending siganls:\n");

    if(sigpending(&pendings) == -1)
    {
        printf("Error: sigpending\n");
    }
    for(   int i = 0 ; i < NSIG ; i ++  )
    {
        if(sigismember(&pendings, i))
        {
            printf("Pending Signal %d (%s)\n", i, strsignal(i));
        }
    }

    exit(EXIT_SUCCESS);
}
