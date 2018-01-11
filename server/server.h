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
#include <pthread.h>

#include "parser.cpp"
#include "client_info.cpp"

struct server {
  int servFd;
  client_info clientList[10];
};
struct data_s {
  server* pointer;
  client_info* client;
};

uint16_t readPort(char * txt);
void start_server(int port);
int index_check(server*s);
void *waiting_for_connection(void *arguments);
void *read_listener(void *arguments);

void setReuseAddr(int sock);
//void ctrl_c(int);


#endif //SERVER_H
