#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "utils.cpp"

void parse_command(data_s* arg, std::vector<std::string> &vec,int count);
void sendtolocal(server* server_data,int channel,char buf[],int msgcount);
void sendtoother(server* server_data,client_info* sender,char buf[],int msgcount);
int initialize_client(data_s* args, std::vector<std::string> &vec,int count);
int index_check2(server* s);
void parse_server_command(server* args, std::vector<std::string> &vec);
void AddClient(int fd,client_info* client);
#endif //COMMAND_PARSER_H
