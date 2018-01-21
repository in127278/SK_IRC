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

#include "parsers/parser.cpp"
#include "client/client_info.cpp"
#include "config.h"

struct server {
  int servFd;
  int receivesocket;
  client_info clientList[MAX_CLIENTS];
  std::vector <int> otherserv;
  client_info other[MAX_SERVER_CLIENTS];
  char* addr;
  int port;
  pthread_mutex_t mut1;
  void get_ctrl(int);
  void closing_server();
};

struct sdata {
  server* pointer;
  int whatsocket;
  client_info* client;
};


uint16_t readPort(char * txt);
void start_server(char *address,int port,char *pinger);
int index_check(server*s);
void *waiting_for_connection(void *arguments);
void *read_listener(void *arguments);
void* initialize_send(void *arguments);
void *server_loop(void *arguments);
void *ping_other(void *arguments);
void setReuseAddr(int sock);
//void ctrl_c(int);
int read_from_server(sdata* data,std::vector<std::string> &vec);

#endif //SERVER_H
