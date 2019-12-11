/*2019-12-11 He Zhu Linux C Programming Demo*/
/*Create a log using Daemon Process periodically*/
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>

void handler(int signum)
{
    char *homeDir = getenv("HOME");
    char strFilename[250] = {0};
    sprintf(strFilename, "%s/log/%ld.txt", homeDir, time(NULL));

    int fd = open(strFilename, O_RDWR|O_CREAT,0666);
    
    //if file open failed
    if( fd < 0 )
    {
        perror("open err");
        exit(1);
    }
    close(fd);
}

int main()
{
    //create a child process, and parent exits
    pid_t pid = fork();

    if( pid > 0 )
    {
        exit(1);
    }

    //setsid() to be the leader of the session
    setsid();

    //set the umask
    umask(0);
    //change the current directory
    chdir(getenv("HOME"));

    //close the file descriptor

    //core logic
    struct itimerval myit = {{60,0}, {1,0}};
    setitimer(ITIMER_REAL, &myit, NULL);
    
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler=handler;
    sigaction(SIGALRM, &sa, NULL);

    while (1) 
    {
        //create a file in /home/hezhu/Documents/log/ for every 60 seconds
        sleep(1);
    }

    return 0;

}