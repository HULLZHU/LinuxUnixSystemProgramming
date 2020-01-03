#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
# define MAXPID 32768
int main(int argc, char *argv[])
{
    if( argc < 2)
    {
        printf("usage: %s startpid [endpid]\n",argv[0]);
        exit(1);
    }

    int start = atoi(argv[1]);
    int end = MAXPID;
    
    if( argc == 3 && atoi(argv[2])< MAXPID)
        end = atoi(argv[2]);
    
    for(int i = start; i <= end; i++)
    {
        kill(i, SIGKILL);
    }

    printf("Process from %d to %d killed\n",start, end);
    exit(1);
}