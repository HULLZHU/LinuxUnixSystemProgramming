#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 8888

int main(int argc, char *argv[])
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1)
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    /*(1) sockaddr_in struct: sockaddr struct for inet*/
    struct sockaddr_in server;
    server.sin_family = AF_INET;//IPv4 protocal
    server.sin_port = htons(SERV_PORT);//server port 8888
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind the socket to an address
    int ret = bind(lfd, (struct sockaddr*)&server, sizeof(server));
    if( ret == -1 )
    {
        perror("bind error\n");
        exit(EXIT_FAILURE);
    }

    // set the listener
    ret = listen(lfd, 20);
    if( ret == -1 )
    {
        perror("listen error\n");
        exit(EXIT_FAILURE);
    }

    /*(2) sockaddr_in struct*/
    //wait and accept the connection request
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int cfd = accept(lfd, (struct sockaddr*)&client, &len);
    if( cfd == -1 )
    {
        perror("accept() error\n");
        exit(1);
    }

    printf("accept successful");
    char ipbuf[64] = {0};
    printf("Client IP: %s, Port: %d\n", inet_ntop(AF_INET, &client.sin_addr.s_addr, ipbuf,sizeof(ipbuf)),
    ntohs(client.sin_port));

    while(1)
    {
        char buf[1024] = {0};
        int len = read(cfd, buf,sizeof(buf));

        if( len == -1)
        {
            perror("Read Error\n");
            exit(1);
        }
        else if( len == 0 )
        {   
            printf(" client disconnected \n");
            close(cfd);
            break;
        }
        else
        {
            printf("recv buf: %s\n", buf);

            for(int i = 0 ; i < len; ++i)
            {
                buf[i] = toupper(buf[i]);
            }
            printf("Send Buf: %s\n", buf);
            write(cfd, buf, len);
        }
    }


    close(lfd);

    return 0;
}
