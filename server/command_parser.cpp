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
        sendtolocal(args->pointer,args->client->channel,str,sizeof(str));
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
    sendtolocal(args->pointer,args->client->channel,buf,msgcount);
    sendtoother(args->pointer,args->client,buf,msgcount);
  }

}
void sendtolocal(server* server_data,int channel,char buf[],int msgcount){
  for(int i=0;i<10;i++){
    if(server_data->clientList[i].channel == channel){
      write(server_data->clientList[i].clientFd,buf,msgcount);
      printf("Sending buf to %d from \n",server_data->clientList[i].clientFd);
    }
  }
}
void sendtoother(server* server_data,client_info* sender,char buf[],int msgcount){
  if(server_data->otherserv.size()>0){
    unsigned iter=0;
    while(iter < server_data->otherserv.size()){
      for(int i=0;i<30;i++){
        if(server_data->other[i].clientFd == server_data->otherserv[iter] and server_data->other[i].channel == sender->channel){
          break;
        }
      }
      char bufe[200]="";
      strcat(bufe,"msg ");
      strcat(bufe,std::to_string(sender->channel).c_str());
      strcat(bufe," ");
      strcat(bufe,buf);
      write(server_data->otherserv[iter], bufe, sizeof(bufe));
      printf("Sending buf to other %d from %d \n",server_data->otherserv[iter],sender->clientFd);

      iter++;
    }
  }
}


int initialize_client(data_s* args, std::vector<std::string> &vec,int count){
  if(strcmp(vec[0].c_str(),"serwer") == 0){
    return 0;
  }
  else {
    int index = args->client->id;
    args->pointer->clientList[index].channel=0;

    memset(&args->pointer->clientList[index].nick[0], 0, sizeof(&args->pointer->clientList[index].nick));
    memset(&args->pointer->clientList[index].msg[0], 0, sizeof(&args->pointer->clientList[index].msg));

    strcpy(args->client->nick,vec[1].c_str());

    write(args->pointer->clientList[index].clientFd,"Witaj na serwerze",sizeof("Witaj na serwerze"));
    return 1;
  }
}

void parse_server_command(sdata* args, std::vector<std::string> &vec){
  if(strcmp(vec[0].c_str(),"add" ) == 0){
    int id = std::stoi(vec[1].c_str());
    int index2=index_check2(args->pointer);
    args->pointer->other[index2].id=id;
    strcpy(args->pointer->other[index2].nick,vec[2].c_str());
    int channel = std::stoi(vec[3].c_str());
    args->pointer->other[index2].channel=channel;
    args->pointer->other[index2].clientFd=args->whatsocket;
    printf("nick %s\n",args->pointer->other[index2].nick);
    printf("id %d\n",args->pointer->other[index2].id);
    printf("channel %d\n",args->pointer->other[index2].channel);
      }
      char buf[200]="";
  if(strcmp(vec[0].c_str(),"msg" ) == 0){
    int channel = std::stoi(vec[1].c_str());
    for(std::vector<int>::size_type i = 2; i != vec.size(); i++) {
      strcat(buf,vec[i].c_str());
      strcat(buf," ");

    }
    sendtolocal(args->pointer,channel,buf,sizeof(buf));
  }
}
void AddClient(int fd,client_info* client){
  char buf[200]="";
  strcat(buf,"add ");
  strcat(buf,std::to_string(client->id).c_str());
  strcat(buf," ");
  strcat(buf,client->nick);
  strcat(buf," ");
  strcat(buf,std::to_string(client->channel).c_str());
  write(fd, buf, sizeof(buf));
  memset(&buf[0], 0,sizeof(buf));
  printf("sending %s\n", buf);
}




//TODO REMOVE IT
int index_check2(server* s){
  for (int i=0;i<10;i++){
		//printf("%d %d\n",i,s->clientList[i].clientFd);
    if(s->other[i].clientFd == -1){
        return i;
    }
  }
return -1;
}
