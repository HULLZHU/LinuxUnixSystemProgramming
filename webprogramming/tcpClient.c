#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 8888
#define BUF_SIZ 1028

int sfd;

void disconnectServer(int sig)
{
    write(sfd,"SERVER STOP PASSWORD", sizeof("SERVER STOP PASSWORD"));
    printf("Sent the stop signal\n");
}

int main()
{

    //(0)
    signal(SIGKILL, disconnectServer);
    signal(SIGSTOP, disconnectServer);
    signal(SIGTSTP, disconnectServer);
    signal(SIGQUIT, disconnectServer);

    int len;
    struct sockaddr_in server_addr;
    char buf[BUF_SIZ];

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if( sfd < 0 )
    {
        perror("socket()\n");
        exit(EXIT_FAILURE);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERV_IP, &server_addr.sin_addr.s_addr);
    server_addr.sin_port = htons(SERV_PORT);
    

    //connect to the active socket referred to by the file descriptor sfd
    int s = connect(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if( s == -1 )
    {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        fgets(buf, sizeof(buf), stdin);
        /*write*/
        int r = write(sfd, buf, strlen(buf));
        if( r== 0)
        {
            printf("Write done\n");
            break;
        }
        else if( r < 0 )
        {
            perror("write()");
        }
        else
        {
            printf("Write ======%d\n",r);
        }
        /*read*/
        int len = read(sfd, buf, sizeof(buf));
        if( len < 0 )
        {
            perror("read()");
            break;
        }
        else if( len == 0 )
        {
            printf("Read Done");
        }
        else
        {
            printf("Read ======%d\n",len);
            printf("Read Content:\n%s",buf);
        }
    }

    close(sfd);

    return 0;

}