#include "utils.h"


void prepareforping(adres* adr, server* args,client* connected){

  adr->hints.ai_family = AF_INET;
  adr->hints.ai_protocol = IPPROTO_TCP;
  connected->fd=socket(AF_INET, SOCK_STREAM, 0);

  if(int err = getaddrinfo(args->addr, std::to_string(args->port).c_str(), &adr->hints, &adr->resolved))
      error(1, 0, "Resolving address failed: %s", gai_strerror(err));
    }


bool check_nick(std::vector <client*> connected_clients ,std::string &buf){
      return true;
  }


bool is_number(const std::string& s)
{
      return !s.empty() && std::find_if(s.begin(),
          s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}


std::string generate_name(int len)
    {
        static auto& chrs = "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

        thread_local static std::mt19937 rg{std::random_device{}()};
        thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

        std::string s;

        s.reserve(len);

        while(len--)
            s += chrs[pick(rg)];

        return s;
    }
