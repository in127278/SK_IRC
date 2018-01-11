#include "command_parser.h"

void parse_command(data_s* args, std::vector<std::string> &vec,int count){
  if(strcmp(vec[0].c_str(),"/nick" ) == 0){
      if(vec.size()<2){
        write(args->client->clientFd,"Not enough arguments",sizeof("Not enough arguments"));
      }
      else if( strcmp(args->client->nick,"") == 0){
        char str[50]="";
        strcat(str,"Welcome: ");
        strcat(str,vec[1].c_str());
        write(args->client->clientFd,str,sizeof(str));
        strcpy(args->client->nick,vec[1].c_str());
      }
      else{
        char str[100]="";
        strcat(str,args->client->nick);
        strcat(str," has changed name to: ");
        strcat(str,vec[1].c_str());
        sendtolocal(args->pointer,args->client,str,sizeof(str));
        strcpy(args->client->nick,vec[1].c_str());
      }
    }
    else if(strcmp(vec[0].c_str(),"/join" ) == 0){
      if(vec.size()<2){
        write(args->client->clientFd,"Not enough arguments",sizeof("Not enough arguments"));
      }
      else{
        char str[100]="";
        strcat(str,"Leaving channel: ");
        strcat(str,std::to_string(args->client->channel).c_str());
        strcat(str," And connecting to channel: ");
        strcat(str,vec[1].c_str());
        write(args->client->clientFd,str,sizeof(str));
        args->client->channel=atoi(vec[1].c_str());
      }
    }
  else{
    char buf[255]="";
    strcat(buf,args->client->nick);
    strcat(buf,": ");
    strcat(buf,args->client->msg);
    int msgcount=count+sizeof(args->client->nick)+sizeof(": ");
    sendtolocal(args->pointer,args->client,buf,msgcount);
  }

}
void sendtolocal(server* server_data,client_info* sender,char buf[],int msgcount){
  for(int i=0;i<10;i++){
    if(server_data->clientList[i].channel == sender->channel){
      write(server_data->clientList[i].clientFd,buf,msgcount);
      printf("Sending buf to %d from %d \n",server_data->clientList[i].clientFd,sender->clientFd);
    }
  }
}
