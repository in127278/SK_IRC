#ifndef CLIENT_H
#define CLIENT_H
#include <string>

class client {
public:
  client();
  int fd;
  char nick[25];
  int channel;
  bool server;

};


#endif //CLIENT_H
