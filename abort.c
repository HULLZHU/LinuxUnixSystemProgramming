/*The abort() first unblocks the SIGABRT signal, and then raises that signal for the calling process (as though raise(3) was
       called).  This results in the abnormal termination of the process unless the SIGABRT signal is caught and the signal  han‐
       dler does not return (see longjmp(3)).

       If  the  SIGABRT  signal  is  ignored,  or caught by a handler that returns, the abort() function will still terminate the
       process.  It does this by restoring the default disposition for SIGABRT and then raising the signal for a second time.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "tlpi_hdr.h"

void abort()
{
    struct sigaction sa;

    //retrieve the old sigset to sa
    sigaction(SIGABRT, NULL, &sa);

    //If the original signal handler is ignoring
    if( sa.sa_handler == SIG_IGN )
    {   
        //reset the handler to be SIG_DFL (default one)
        sa.sa_handler = SIG_DFL;
        sigaction(SIGABRT, &sa, NULL);
    }

    //if the signal handler has been reset to SIG_DFL
    if( sa.sa_handler == SIG_DFL)
    {
        //close all the opened buffer
        if(fcloseall() != 0)
        {
            errExit("fcloseall()");
        }
    }

    if( sigismember(&sa.sa_mask,SIGABRT))
    {
        //remove the SIGABRT from the blocked signals set
        sigdelset(&sa.sa_mask,SIGABRT);
        //reset the signal set again
        sigprocmask(SIG_SETMASK, &sa.sa_mask, NULL);
    }

    kill(getpid(), SIGABRT);

    //Finally
    fcloseall();
    sa.sa_handler =  SIG_DFL;
    sigaction(SIGABRT, &sa, NULL);
    sigprocmask(SIG_SETMASK, &sa.sa_mask, NULL);
    kill(getpid(), SIGABRT);
    exit(1);
}

void fa (int signo)
{	
    printf ("Siganl Caught% d\n", signo);
}

int main ()
{	
    signal (SIGABRT, fa);	
    printf ("1111111111111\n");	
    abort ();	
    printf ("2222222222222\n");	return 0;
}
