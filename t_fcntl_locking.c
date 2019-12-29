#define _GNU_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include  <errno.h>

#define _FILE_NAME "/home/hezhu/Documents/APUE/20191225Threads/test.txt"

void errExit(const char *arg)
{
    printf("Err: %s\n", arg);
    exit(EXIT_FAILURE);
}

//  (0)    (1) (2)    (3)      (4)     (5)
/* ./a.out cmd l_type l_whence l_start l_len*/
int main(int argc, char *argv[])
{
    //(0) open the file and get the file descriptor
    int fd = open(_FILE_NAME, O_CREAT|O_RDWR);
    if(fd == -1)errExit("open()");

    //(1) get the cmd to control the file region lock
    int cmd;
    if(strcmp("F_SETLK", argv[1]) == 0) cmd = F_SETLK;
    else if(strcmp("F_SETLKW", argv[1]) == 0) cmd = F_SETLKW;
    else if(strcmp("F_GETLK", argv[1]) == 0) cmd = F_GETLK;
    else errExit("Invalid cmd");
    
    //(2) get the type
    int type;
    if(strcmp("F_RDLCK", argv[2]) == 0)type =F_RDLCK;
    else if(strcmp("F_WRLCK", argv[2]) == 0)type = F_WRLCK;
    else if(strcmp("F_UNLCK", argv[2]) == 0)type = F_UNLCK;
    else errExit("Invalid type");

    //(3) get the l_whence
    int whence;
    if(strcmp("SEEK_SET", argv[3]) == 0)whence =SEEK_SET;
    else if(strcmp("SEEK_CUR", argv[3]) == 0)whence = SEEK_CUR;
    else if(strcmp("SEEK_END", argv[3]) == 0)whence = SEEK_END;
    else errExit("Invalid Whence");
    
    //(4) get the l_start
    int start = atoi(argv[4]);
    if(start < 0 && whence == SEEK_SET )
        errExit("l_whence == SEEK_SET, so make sure start >= 0");

    //(5) get the l_len
    int len = atoi(argv[5]);
    if( len < 0 )
        errExit("Invalid len");

    //(6) declare and initialize the struct flock
    struct flock flk;
    flk.l_type = type;
    flk.l_len = len;
    flk.l_whence = whence;
    flk.l_start = start;

    //(7) control the file as the cmd and flk requested
    int status = fcntl(fd,cmd, &flk);
    if(cmd == F_GETLK)
    {
        if(status == -1)
            errExit("Invalid fcntl-F_GETLK");
        else
        {
            if(flk.l_type == F_UNLCK)
                printf("[PID = %ld] Lock can be placed\n",(long)getpid());
            else
                printf("[PID = %ld] Lock Denied by Process %ld: %s - %lld - %lld", (long)getpid(), (long)flk.l_pid, 
                flk.l_type==F_RDLCK?"READ":"WRITE",(long long) flk.l_start, (long long)flk.l_len);
        }
    }
    else
    {
        if (status == 0)
        {
            if(type == F_RDLCK)
            {
                printf("[PID = %ld] READ_LOCK\n", (long)getpid());
            }
            else if(type == F_WRLCK)
            {
                printf("[PID = %ld] WRITE_LOCK", (long)getpid());
            }

            else if(errno == EAGAIN || errno == EACCES)
            {
                errExit("EAGAIN || EACCESS");
            }
            else if(errno == EDEADLOCK)
            {
                errExit("DEADLOCK");
            }
            else
            {
                errExit("fcntl-FSETLK(W)");
            }
        }
    }

}