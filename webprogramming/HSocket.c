#define _GNU_SOURCE
#include "HSocket.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void perr_exit(const char *s)
{   
    perror(s);
    exit(EXIT_FAILURE);
}

int Accept(int socket, struct sockaddr *addr, socklen_t *len)
{
    int fd;
    
    while( (fd = accept(socket, addr, len)) == -1 )
    {
        if(errno == EAGAIN || errno ==EWOULDBLOCK)
        {
            continue;
        }
        else
        {
            perr_exit("Accept(): some unkown error\n");
        }
    }

    return fd;
}

int Bind(int socket, const struct sockaddr *addr, socklen_t len)
{
    int ret = bind(socket, addr, len);

    if(ret == -1)
    {
        printf("Bind(): %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return ret;
}

int Connect(int socket, const struct sockaddr *addr, socklen_t address_len)
{
    int ret = connect(socket, addr, address_len);

    if(ret == -1)
    {
        printf("Connect(): %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    return ret;
}

int Listen(int socket, const int backlog)
{
    int ret = listen(socket, backlog);
    if(ret == -1)
    {
        printf("Listen(): %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return ret;
}

int Socket(int domain, int type, int protocal)
{
    int ret = socket(domain, type, protocal);
    if(ret == -1)
    {
        printf("Socket(): %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return ret;
}

ssize_t Read(int fd, void *buf, size_t sz)
{
    int len;
    while( (len = read(fd, buf, sz)) == -1)
    {
        if( errno == EINTR)
        {
            continue;
        }
        else
        {
            return -1;
        }    
    }

    return len;
}

ssize_t Write(int fd, void* buf, size_t sz)
{
    int len;
    while( (len = write(fd,buf,sz)) == -1)
    {
        if(errno == EINTR)
        {
            continue;
        }
        else
        {
            return -1;
        }
        
    }
    return len;
}

int Close(int fd)
{
    int ret;
    while( (ret = close(fd)) == -1)
    {
        if(errno = EINTR)
        {
            continue;
        }
        else
        {
            return -1;
        }
    }
    return ret;
}

ssize_t WriteN(int fd, const void *vptr, size_t sz)
{
    size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = sz;
	while (nleft > 0) 
    {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) 
        {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return sz;

}

ssize_t ReadN(int fd, void *vptr, size_t sz)
{
    ssize_t nleft;
    ssize_t nread;
    const char* ptr;

    ptr = vptr;
    nleft = sz;

    while( nleft > 0 )
    {
        if((nread = read(fd, ptr, nleft)) < 0 )
        {
            if(errno == EINTR)
            {
                nread = 0;
                continue;
            }
            else
            {
                return -1;
            }
        }

        else if( nread ==  0)
        {
            break;
        }
        nleft -= nread;
        ptr += nread;
    }

    return sz-nleft; 
}










 