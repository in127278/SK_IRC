#ifndef SERVER_H
#define SERVER_H
#include <cstdlib>
#include <cstdio>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <signal.h>
#include <string.h>
#include <poll.h>
#include <pthread.h>

#include "client_info.cpp"
#include "parser.cpp"

struct server {
  int servFd;
  pollfd FdList[10];
  client_info clientList[10];
  int running[10];
};
struct data_s {
  server* pointer;
  client_info* client;
};

uint16_t readPort(char * txt);
void start_server(int port);
int index_check(server*s);
void *poll_listener(void * arguments);
void *waiting_for_connection(void *arguments);
void start_reading(server* arg,client_info* client);
void *read_listener(void *arguments);
void *basic_msghandler(void *arguments);

void setReuseAddr(int sock);
//void ctrl_c(int);

#endif //SERVER_H
