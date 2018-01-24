#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "utils.cpp"
#include <random>

void parse_command(sdata* arg, std::vector<std::string> &vec,int count,const std::string &msg);
void sendtolocal(server* server_data,int channel,const std::string &buf,int msgcount);
int initialize_client(sdata* args, std::vector<std::string> &vec);
int index_check2(server* s);
void parse_server_command(sdata* serverData, std::vector<std::string> &vec,const std::string &buf);
void add_client(int fd,client_info* client);
void remove_clients_from_server(sdata* serverData,int fd);
void send_to_nearby(server* server_data,client_info* sender,const std::string &buf,int msgcount,std::string &command);
void resend_to_others(sdata* serverData,const std::string &buf,std::vector<std::string> &vec);
bool check_nick(sdata* serverData,std::string &buf);
std::string generate_name(int len);
bool is_number(const std::string& s);
void remove_client(sdata* serverData,client_info* client);
#endif //COMMAND_PARSER_H
