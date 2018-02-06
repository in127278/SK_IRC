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

void prepareforping(adres* adr, server* args, client* connected);
std::string generate_name(int len);
bool check_nick(std::vector <client*> connected_clients ,std::string &buf);
bool is_number(const std::string& s);
#endif //UTILS_H
