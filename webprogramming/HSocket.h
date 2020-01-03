#ifndef __HSOCKET_H_
#define __HSOCKET_H_H

#include <sys/socket.h>

void perr_exit(const char *s);
int Accept(int socket, struct sockaddr *addr, socklen_t *len);
int Bind(int socket, const struct sockaddr *addr, socklen_t len);
int Connect(int socket, const struct sockaddr *addr, socklen_t address_len);
int Listen(int socket, const int backlog);
int Socket(int domain, int type, int protocal);
ssize_t Read(int fd, void* buf, size_t sz );
ssize_t Write(int fd, void* buf, size_t sz);
int Close(int fd);
ssize_t WriteN(int fd, const void *vptr, size_t sz);
ssize_t ReadN(int fd, void *vptr, size_t sz);

#endif