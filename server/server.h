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
#include <vector>
#include <random>

#include "other/client.cpp"
#include "other/parser.cpp"

class server {
public:
  server();
  ~server();
  int servFd;
  char* addr;
  int port;
  pthread_mutex_t mut1;
  client* receiveserv;
  std::vector <client*> connected_clients;
  std::vector <client*> imported_clients;
  std::vector <client*> otherservvec;
  void delete_client(client* connected);
  void delete_imported_client(client* connected_server);
  void delete_server(client* connected);
  void sendtoall(std::string message);
  void sendtoothers(int fd,std::string message);
  std::string prepare_message(short message_type,client* client_to_send);
  std::string prepare_client_message(short message_type,client* sender,std::string &message_to_send);
  void init_server(char *address,int port);
  void manage_connected(std::vector<std::string> &vec,client* connected);
  void start_listening(char *server_to_ping);
  bool checkifdc(client* connected,std::string &message);
  std::string readMsg(int fd);
  void writeMsg(std::string msg, int fd);
};

struct sdata {
  server* serv;
  int whatsocket;
  client* connected;
};
void* exchange_clients(void *arguments);
void *waiting_for_connection(void *arguments);
void *ping_other(void *arguments);
void *client_main_thread(void *arguments);
bool check_nick(std::vector <client*> connected_clients ,std::string &buf);
std::string generate_name(int len);
#endif //SERVER_H
