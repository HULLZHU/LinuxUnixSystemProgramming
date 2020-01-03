#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>

#define SERV_PORT 8888
#define SERV_IP "127.0.0.1"

int fd;

void disconnectServer(int sig)
{
    write(fd,"SERVER STOP PASSWORD", sizeof("SERVER STOP PASSWORD"));
}


int main(int argc, char *argv[])
{
    //(0)
    signal(SIGKILL, disconnectServer);
    signal(SIGSTOP, disconnectServer);
    signal(SIGTSTP, disconnectServer);
    signal(SIGQUIT, disconnectServer);
    
    //(1) create a file descriptor
    fd = socket(AF_INET, SOCK_STREAM, 0);

    //(2) create a server socket and initialize it 
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, serv.sin_addr.s_addr);

    //(3) connect the app to the server
    connect(fd, (struct sockaddr*)&serv, sizeof(serv));

    while(1)
    {
        //send the data to the server
        char buf[1024];
        fgets(buf,sizeof(buf), stdin);
        write(fd, buf, strlen(buf));

        //wait for the data from the server
        int len = read(fd, buf, sizeof(buf));

        if( len == -1)
        {
            perror("read error\n");
            break;;
        }
        else if( len == 0 )
        {
            printf("Server closed\n");
            break;
        }
        else
        {
            printf("Received: %d\n", buf);
        }
        
    }


    
}