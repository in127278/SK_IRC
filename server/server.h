#ifndef SERVER_H
#define SERVER_H
#include "client_info.h"
#include <poll.h>
class server {
public:
  server();
  ~server();
  int servFd;
  pollfd FdList[10];
  client_info clientList[10];
  int index_check();
  void initialize(int);
};


#endif //SERVER_H
