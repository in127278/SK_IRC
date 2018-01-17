#include "utils.h"

void prepareforping(adres* adr, server* args){

  adr->hints.ai_family = AF_INET;
  adr->hints.ai_protocol = IPPROTO_TCP;
  args->receivesocket=socket(AF_INET, SOCK_STREAM, 0);

  if(int err = getaddrinfo(args->addr, std::to_string(args->port).c_str(), &adr->hints, &adr->resolved))
      error(1, 0, "Resolving address failed: %s", gai_strerror(err));

}
