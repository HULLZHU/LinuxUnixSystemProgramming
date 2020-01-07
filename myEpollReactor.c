#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define SERVPORT 8888
#define MAXEVENT 1024
#define BUFSZ    1024

void eventdel(void *arg);
void eventadd(void *arg);
void sendData(void *arg);
void recvData(void *arg);

void errExit(char *arg)
{
    printf("Err: %s\n", arg);
    exit(EXIT_FAILURE);
}

struct Event
{
    int fd;
    int events;
    void *arg;
    int status;
    void (*call_back)(void *arg);
};

int epfd;                                   //we only maintain one epfd for one R-B tree
struct Event Events[MAXEVENT+1];
struct epoll_event EpollEvents[MAXEVENT+1];

void setevent(struct Event * event,int fd, int events, void *arg)
{   
    event->fd = fd;
    event->events = events;
    event->arg = arg;
    event->status = 0;
}

void sendData(void *arg)
{
    struct Event *event = (struct Event *)arg;

    int len = send(event->fd, "message from the server\n", 32 * sizeof(char) , 0);
    
    eventdel(event);

    if( len > 0 )
    {   
        printf("Server: one message sent\n");
        event ->call_back = recvData; 
        event->events = EPOLLIN;
        eventadd((void*)event);
    }
    else if( len == 0 )
    {
        printf("Server: one client disconnected\n");
        eventdel((void*)event);
        close(event->fd);
        
    }
    else
    {
        printf("Server: sending error\n");
        eventdel((void*)event);
        close(event->fd);
    }
    
    return;
}

void recvData(void *arg)
{
    struct Event *event = (struct Event*)arg;

    char buf[BUFSZ] = {0};
    int len = recv(event->fd, buf, sizeof(buf),0);

    eventdel(arg);

    if( len > 0 )
    {
        printf("Received: %s",buf);

        event->call_back = sendData;
        event->events = EPOLLOUT;
        eventadd((void*)event);
    }
    else if(len == 0)
    {
        printf("Server: one client disconnected\n");
        close(event->fd);
    }
    else
    {
        printf("Server: recv error\n");
        close(event->fd);
    }


    return;
}

void eventadd(void *arg)
{   
    struct Event *event = (struct Event*)arg;
    struct epoll_event epv;
    epv.events = event->events;
    epv.data.ptr = arg;

    int op;
    if(event->status == 1)
    {
        op = EPOLL_CTL_MOD;
    }
    else
    {
        op = EPOLL_CTL_ADD;
        event -> status = 1;
    }

    int ret = epoll_ctl(epfd, op, event->fd, &epv);
    if(ret == -1)
        errExit("eventadd(): epoll_ctl()");
    
    printf("client fd==%d added to the tree\n", event->fd);
    return;
}

void eventdel(void *arg)
{
    struct Event *event = (struct Event*)arg;
    
    int op;

    if(event->status == 0)
    {
        op = EPOLL_CTL_MOD;
    }
    else
    {
        event->status = 0;
        op = EPOLL_CTL_DEL;
    }

    int ret = epoll_ctl(epfd, op, event->fd, NULL);
    if(ret < 0 )
        errExit("event_del() : epoll_ctl()");
}


void acceptconnection(void* tempfd)
{
    struct Event *event = (struct Event *)tempfd;
    int lfd = event->fd;
    struct sockaddr_in client;
    client.sin_family = AF_INET;
    int len = sizeof(client);
    int ret = accept(lfd, (struct sockaddr*)&client, &len);
    if (ret < 0)errExit("accept()");

    printf("client connected\n");
    for(int i = 0 ; i < MAXEVENT; i ++ )
    {
        if( Events[i].status == 0 )
        {   
            setevent(&Events[i], ret, EPOLLIN, &Events[i]);
            eventadd((void*)&Events[i]);
            Events[i].call_back = recvData;
            break;
        }
    }
}



int main()
{
    epfd = epoll_create(MAXEVENT+1);

    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd < 0 )
    {
        printf("socket() %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVPORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    Events[MAXEVENT].fd = lfd;
    Events[MAXEVENT].status = 1;
    Events[MAXEVENT].call_back = acceptconnection;
    Events[MAXEVENT].arg = (void*)&Events[MAXEVENT];
    Events[MAXEVENT].events = EPOLLIN;

    struct epoll_event lepv = {0, {0}};
    lepv.events = EPOLLIN;

    lepv.data.ptr = (void*)&(Events[MAXEVENT]);

    if(epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &lepv)<0)
    {
        printf("epoll_ctl() %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    bind(lfd, (struct sockaddr*)&server, sizeof(server));
    listen(lfd, MAXEVENT);
    
    while(1)
    {
        int nready = epoll_wait(epfd, EpollEvents, MAXEVENT+1, -1);
        if(nready < 0 )
        {
            printf("epoll_wait() : %s\n",strerror(errno));
            exit(EXIT_FAILURE);
        }

        for(int i = 0 ; i < nready; i ++ )
        {
            struct Event* event = (struct Event*)EpollEvents[i].data.ptr;
            
            if((event->events & EPOLLIN) && (EpollEvents[i].events & EPOLLIN))
            {
                event->call_back((void*)event);
            }

            if((event->events & EPOLLOUT) && (EpollEvents[i].events & EPOLLOUT))
            {
                event->call_back((void*) event);
            }
        }
    }

    return 1;
}