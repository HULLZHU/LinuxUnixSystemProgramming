#define _GNU_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include  <errno.h>
#define MAX_LINE 100

static void displayCmdFmt()
{
    printf("\n Format: cmd lock start length [whence]\n\n");

}

int main(int argc, char *argv[])
{
    int fd, numRead, cmd, status;
    char lock, cmdCh, whence, line[MAX_LINE];
    struct flock fl;
    long long len, st;

    if( argc != 2 || strcmp(argv[1], "help") == 0 )
    {
        printf("Usage: open (%s)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //(1) open the file and get the file descriptor
    fd = open(argv[1], O_RDWR); 
    if(fd == -1)
    {
        printf("Error: open() \n");
        exit(EXIT_FAILURE);
    }

    printf("Enter ? for help\n");

    while(1)
    {
        printf("PID = %ld >", (long)getpid());
        fflush(stdout);

        /*read from stdin and store them into the buffer line*/
        /*the number of characters can be read is MAX_LINE - 1*/
        if(fgets(line, MAX_LINE, stdin) == NULL)
        {
            exit(EXIT_SUCCESS);
        }
        line[strlen(line) - 1] ='\0';                   /*remove the trailing '\n'*/

        if( *line == '\0')                              /*skip blank lines*/
        {
            continue;

        }

        if( line[0] =='?')
        {
            displayCmdFmt();
            continue;
        }

        whence = 's';

        numRead = sscanf(line, "%c %c %lld %lld %c", &cmdCh, &lock, &st, &len, &whence);
        fl.l_start = st;
        fl.l_len = len;

        if( numRead < 4 || strchr("gsw",cmdCh) == NULL || 
        strchr("rwu",lock) == NULL || strchr("sce",whence) == NULL)
        {
            printf("Invalid Command\n");
            continue;
        }

        cmd = (cmdCh == 'g') ? F_GETLK:(cmdCh== 's')?F_SETLK: F_SETLKW;
        fl.l_type = (lock == 'r')?F_RDLCK:(lock == 'w')?F_WRLCK:F_UNLCK;
        fl.l_whence = (whence == 'c') ? SEEK_CUR:(whence == 'e')?SEEK_END:SEEK_SET;
        
        status = fcntl(fd, cmd, &fl);

        if(cmd == F_GETLK)
        {
            if(status == -1)
            {
                perror("fcntl-FGETLK\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                if(fl.l_type == F_UNLCK)
                {
                    printf("[pid=%ld]Lock can be placed\n", (long)getpid());
                }
                else
                {
                    printf("[pid=%ld]Denied by %s lock on lld%:%lld, (held by PID %ld)\n",
                    (long)getpid(), (fl.l_type == F_RDLCK)?"READ":"WRITE", (long long)fl.l_start, (long long)fl.l_len, (long)fl.l_pid);
                }
            }
        }
        else
        {
            if(status == 0)
            {
                printf("[PID=%ld]%s\n", (long)getpid(), (lock=='u')?"unlcoked":"got lock");
            }
            else if(errno == EAGAIN || errno == EACCES )
            {
                printf("[PID=%ld]%s failed (incompatible lock)\n",(long)(getpid()));
            }
            else if(errno == EDEADLK)
            {
                printf("[PID=%ld] failed (deadlock)", (long)getpid());
            }
        }

    }
}