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
#include <algorithm>
#include "parser.cpp"
#include "client_info.cpp"

struct server {
  int servFd;
  int otherservFd[2];
  int receivesocket;
  client_info clientList[10];
  std::vector <int> otherserv;
  client_info other[30];
  char* addr;
  int port;
};

struct sdata {
  server* pointer;
  int whatsocket;
};


struct data_s {
  server* pointer;
  client_info* client;
};

uint16_t readPort(char * txt);
void start_server(char *address,int port,char *pinger);
int index_check(server*s);
int index_check2(server*s);
void *waiting_for_connection(void *arguments);
void *read_listener(void *arguments);
void* init_send(void *arguments);
void *server_loop(void *arguments);
void *ping_other(void *arguments);
void setReuseAddr(int sock);
//void ctrl_c(int);
int readingfrombuffer(sdata* dta1,std::vector<std::string> &vec);

#endif //SERVER_H
