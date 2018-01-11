#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "server.h"

void parse_command(data_s* arg, std::vector<std::string> &vec,int count);
void sendtolocal(server* server_data,client_info* sender,char buf[],int msgcount);

#endif //COMMAND_PARSER_H
