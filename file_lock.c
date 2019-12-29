#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/file.h>


#define _FILE_NAME_ "/home/hezhu/Documents/APUE/20191225Threads/test.txt"

int main()
{
    int fd = open(_FILE_NAME_, O_RDWR|O_CREAT, 0666);
    if(fd < 0)
    {
        perror("open error\n");
        exit(EXIT_FAILURE);
    }

    struct flock lk;
    lk.l_type = F_WRLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start = 0;
    lk.l_len = 0;

    if(fcntl(fd, F_SETLK, &lk) < 0)
    {
        printf("fcntl error\n");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        sleep(1);
    }

    //core logic
    return 0;
}