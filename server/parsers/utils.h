#ifndef UTILS_H
#define UTILS_H

#include "../server.h"

struct adres{
  addrinfo hints{};
  addrinfo* resolved;
  char name[10];
  char port[5];
  int connected;
};

void prepareforping(adres* adr, server* args);
#endif //UTILS_H
