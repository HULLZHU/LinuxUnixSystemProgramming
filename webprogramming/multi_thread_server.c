/********************************
THIS IS A CLASSICAL MULTI-THREADING
SOCKET TCP SERVER FOR CONCURRENT 
CONNECTION REQUESTS
********************************/
#define _GNU_SOURCE
#include <pthread.h>
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
#include <ctype.h> //INADDR_ANY

/*How many threads can run concurrently*/
#define THREAD_LIMIT 3

#define SERV_PORT 8888
#define BACKLOG 64

void wake(int signo)
{
    printf("one thread leaves, server can accept request again\n");
}

void errExit(const char *str, int err)
{
    printf("server:%s %s\n", str, strerror(err));
    exit(EXIT_FAILURE);
}

typedef struct sockInfo
{
    int on;
    pthread_t tid; //thread id
    int cfd;       //communication socket fd
    struct sockaddr_in addr;
} SockInfo;

void *worker(void *arg)
{
    SockInfo *sock = (SockInfo *)arg;
    char buf[64];
    printf("Server: thread %ld created for client %s on= %d\n", sock->tid, inet_ntop(AF_INET, &sock->addr.sin_addr.s_addr, buf, sizeof(buf)), sock->on);

    while (1)
    {
        char cont[1024];
        int len = read(sock->cfd, cont, sizeof(buf));

        if (len == -1)
        {
            perror("Server(): read error");
            sock->on = 0;
            pthread_exit(NULL);
        }
        else if (len == 0)
        {
            printf("Server(): Thread %ld Client IP%s disconnected\n", sock->tid, inet_ntop(AF_INET, &sock->addr.sin_addr.s_addr, buf, sizeof(buf)));
            close(sock->cfd);
            break;
        }
        else
        {
            if (strcmp("SERVER STOP PASSWORD", cont) != 0)
            {
                printf("Message from Thread %ld IP %s:\n", sock->tid, inet_ntop(AF_INET, &sock->addr.sin_addr.s_addr, buf, sizeof(buf)));
                printf("%s", cont);
            }
            else
                break;
        }

        write(sock->cfd, "successfully sent\n", 32 * sizeof(char));
    }
    sock->on = 0;
    raise(SIGUSR1);
    return NULL;
}

SockInfo socks[THREAD_LIMIT];

int main(int argc, char *argv[])
{

    // for(int i = 0 ; i < THREAD_LIMIT ; i ++)
    //     socks[i].on = 0;

    //(0)
    struct sigaction act;
    act.sa_handler = wake;
    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask, SIGUSR1);
    act.sa_flags = SA_RESTART;
    sigaction(SIGUSR1,&act,NULL);

    sigset_t block;
    sigfillset(&block);
    sigdelset(&block, SIGUSR1);

    //(1) create a socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
        errExit("socket()", errno);

    //(2) create a socket address and bind it to the socket
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        errExit("bind()", errno);

    //(3) servers listens to the income connections
    if (listen(lfd, BACKLOG) == -1)
        errExit("listen()", errno);

    printf("Server Starts\n");
    socklen_t len = sizeof(struct sockaddr_in);
    while (1)
    {
        int i = 0;

        for (i = 0; i < THREAD_LIMIT; i++)
        {
            if (socks[i].on == 0)
                break;
        }

        if (i < THREAD_LIMIT)
        {

            while ((socks[i].cfd = accept(lfd, (struct sockaddr *)&socks[i].addr, &len)) == -1)
            {
                socks[i].cfd = accept(lfd, (struct sockaddr *)&socks[i].addr, &len);
            };

            if (pthread_create(&socks[i].tid, NULL, worker, &socks[i]) != 0)
                errExit("pthread_create()", errno);

            socks[i].on = 1;

            if (pthread_detach(socks[i].tid) != 0)
                errExit("pthread_detach()", errno);
        }
        else
        {
            printf("Server is busy, try later\n");
            sigsuspend(&block);
        }
    }

    exit(EXIT_FAILURE);
}