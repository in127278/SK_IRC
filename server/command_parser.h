#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "utils.cpp"

void parse_command(data_s* arg, std::vector<std::string> &vec,int count);
void sendtolocal(server* server_data,int channel,const std::string &buf,int msgcount);
int initialize_client(data_s* args, std::vector<std::string> &vec);
int index_check2(server* s);
void parse_server_command(sdata* serverData, std::vector<std::string> &vec,const std::string &buf);
void add_client(int fd,client_info* client);
void remove_clients_from_server(sdata* serverData,int fd);
void send_to_nearby(server* server_data,client_info* sender,char buf[],int msgcount);
void resend_to_others(sdata* serverData,const std::string &buf,std::vector<std::string> &vec);
#endif //COMMAND_PARSER_H
