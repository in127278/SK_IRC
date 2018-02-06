#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "other/utils.cpp"

void parse_server_command(sdata* serverData, std::vector<std::string> &vec,const std::string &buf);
void resend_to_others(sdata* serverData,const std::string &buf,std::vector<std::string> &vec);
void parse_command(sdata* serverData, std::vector<std::string> &vec,const std::string &msg);
void sendtolocal(server* server_data,int channel,const std::string &buf,int msgcount);
#endif //MESSAGE_HANDLER_H
