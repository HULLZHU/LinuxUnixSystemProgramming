/********************************
THIS IS A CLASSICAL MULTI-PROCESS
SOCKET TCP SERVER FOR CONCURRENT 
CONNECTION REQUESTS
********************************/
#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>
#include <ctype.h>  //INADDR_ANY

#define USE_SA_RESTART

#define BACKLOG 128
#define SERV_PORT 8888



int Socket(const int domain, const int type, const int protocal)
{
    int fd = socket(domain, type,0);
    if(fd == -1)
    {
        perror("Server: Socket()\n");
        exit(EXIT_FAILURE);
    }
    return fd;
}

void Bind(int socket, const struct sockaddr* address, socklen_t address_len)
{
    int ret = bind(socket, address, address_len);
    if( ret == -1 )
    {
        perror("Server: Bind()\n");
        exit(EXIT_FAILURE);
    }
}

void Listen(int socket)
{
    int ret = listen(socket, BACKLOG);
    if( ret == -1 )
    {
        perror("Server: Listen()\n");
        exit(EXIT_FAILURE);
    }
}

int Accept(int socket, struct sockaddr * restrict address, socklen_t *restrict address_len)
{
    int communicatefd = accept(socket, address,address_len);
    if( communicatefd == -1 )
    {
        perror("Server: Accept()\n");
        exit(EXIT_FAILURE);
    }
    return communicatefd;
}

pid_t Fork()
{
    pid_t pid = fork();
    if( pid == -1)
    {
        perror("Server: Fork()\n");
        exit(EXIT_FAILURE);
    }
    return pid;
}

void recycle(int signo)
{   
    pid_t pid;
    while( (pid = waitpid(-1, NULL, WNOHANG))>0)
    {
        printf("child process %d recycled\n", pid);
    };
}


int main(int argc, char *argv[])
{
    //(1) set the listener fd
    int listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    //(2) set a IPv4 server address and bind it to the socket fd 
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    //(3) listen to the incoming connections
    Listen(listenfd);

    //(*Very Important, Recycle the Child Process*)
    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = recycle;
    //Make sure that when the child process terminates, it will
    //not interrupt the accpet() blocking
    
    act.sa_flags = SA_RESTART;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCHLD, &act, NULL);

    struct sockaddr_in client_addr;
    int len = sizeof(client_addr);

    while(1)
    {   
        
        //(4) block until
        char ipbuf[64] = {0};

        int communicatefd;
        
        while(communicatefd = accept(listenfd, (struct sockaddr*)&client_addr,&len)==-1 && errno == EINTR)
        {}
        
        

        printf("Server: IP %s accepted\n", inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,ipbuf,sizeof(ipbuf)));

        //(5) fork children to process communication
        pid_t pid = Fork();
        if(pid == 0)
        {
            //(5.1) child processes the communication
            close(listenfd);
            while(1)
            {
                char buf[1024] = {0};
                int len = read(communicatefd, buf, sizeof(buf));
                if(len == -1)
                {
                    perror("Server: read()\n");
                }
                else if(len == 0)
                {
                    perror("Server: disconnected from client\n");
                    close(communicatefd);
                    break;
                }
                else
                {
                    printf("Server: IP %s writes:\n%s\n",inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,ipbuf,sizeof(ipbuf)),buf);
                    char response[64] = "Server Response: Received Successfully\n";
                    write(communicatefd,response,sizeof(response));
                }
            }
        }
        else if(pid > 0 )
        {
            //(5.2) parent close the communicatedfd
            printf("Close file descriptor of child process %d \n",pid);
            close(communicatefd);
        }
        else
        {
            perror("Server: perror()\n");
            exit(EXIT_FAILURE);
        }
        
    }

    exit(EXIT_FAILURE);
        

}