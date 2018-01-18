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
    send_to_nearby(args->pointer,args->client,buf,msgcount);
  }

}

void sendtolocal(server* server_data,int channel,const std::string &buf,int msgcount){
  for(int i=0;i<10;i++){
    if(server_data->clientList[i].channel == channel){
      write(server_data->clientList[i].clientFd,buf.c_str(),msgcount);
      printf("Sending buf to %d from \n",server_data->clientList[i].clientFd);
    }
  }
}
//TODO Replace this function with resend_to_others
void send_to_nearby(server* server_data,client_info* sender,char buf[],int msgcount){
  if(server_data->otherserv.size()>0){
    unsigned iter=0;
    while(iter < server_data->otherserv.size()){
      for(int i=0;i<30;i++){
        if(server_data->other[i].clientFd == server_data->otherserv[iter] and server_data->other[i].channel == sender->channel){
          char bufe[200]="";
          strcat(bufe,"msg ");
          strcat(bufe,std::to_string(sender->channel).c_str());
          strcat(bufe," ");
          strcat(bufe,buf);
          write(server_data->otherserv[iter], bufe, sizeof(bufe));
          printf("Sending buf to other %d from %d \n",server_data->otherserv[iter],sender->clientFd);
          break;
        }
      }
      iter++;
    }
  }
}

int initialize_client(data_s* args, std::vector<std::string> &vec){
  if(strcmp(vec[0].c_str(),"serwer") == 0){
    vec.clear();
    return 0;
  }
  else {
    int index = args->client->id;
    args->pointer->clientList[index].channel=0;

    memset(&args->pointer->clientList[index].nick[0], 0, sizeof(&args->pointer->clientList[index].nick));
    memset(&args->pointer->clientList[index].msg[0], 0, sizeof(&args->pointer->clientList[index].msg));

    strcpy(args->client->nick,vec[1].c_str());

    write(args->pointer->clientList[index].clientFd,"Witaj na serwerze",sizeof("Witaj na serwerze"));
    vec.clear();
    return 1;
  }
}

void parse_server_command(sdata* serverData, std::vector<std::string> &vec,const std::string &buf){
  if(strcmp(vec[0].c_str(),"add" ) == 0){
    int index2=index_check2(serverData->pointer);
    serverData->pointer->other[index2].id=index2;
    strcpy(serverData->pointer->other[index2].nick,vec[1].c_str());
    int channel = std::stoi(vec[2].c_str());
    serverData->pointer->other[index2].channel=channel;
    serverData->pointer->other[index2].clientFd=serverData->whatsocket;
    resend_to_others(serverData,buf,vec);


    printf("nick %s\n",serverData->pointer->other[index2].nick);
    printf("clientFd %d\n",serverData->pointer->other[index2].clientFd);
    printf("channel %d\n",serverData->pointer->other[index2].channel);
      }
  if(strcmp(vec[0].c_str(),"msg" ) == 0){
    int channel = std::stoi(vec[1].c_str());
    std::string edited_bufer;
    for(std::vector<int>::size_type i = 2; i != vec.size(); i++) {
      edited_bufer.append(vec[i].c_str());
      edited_bufer.append(" ");
    }
    sendtolocal(serverData->pointer,channel,edited_bufer,sizeof(edited_bufer));
    resend_to_others(serverData,buf,vec);
  }
}

void add_client(int fd, client_info* client){
  char buf[200]="";
  std::string halo;
  strcat(buf,"add ");
  strcat(buf,client->nick);
  strcat(buf," ");
  strcat(buf,std::to_string(client->channel).c_str());
  int count=write(fd, buf, sizeof(buf));
  printf("sending %d %s %d\n",fd, buf,count);
  memset(&buf[0], 0, sizeof(buf));
}

void remove_clients_from_server(sdata* serverData,int fd){
  for(int iter=0;iter<30;iter++){
    if(serverData->pointer->other[iter].clientFd == fd){
      printf("Usuwanie %s %d \n",serverData->pointer->other[iter].nick,serverData->pointer->other[iter].clientFd);
      serverData->pointer->other[iter].clientFd=-2;
      serverData->pointer->other[iter].channel=-1;
      memset(&serverData->pointer->other[iter].nick[0], 0, sizeof(&serverData->pointer->other[iter].nick));
    }
  }

}
void resend_to_others(sdata* serverData,const std::string &buf,std::vector<std::string> &vec){
  if(serverData->pointer->otherserv.size()>1){
    if(strcmp(vec[0].c_str(),"add" ) == 0){
      unsigned iter=0;
      while(iter < serverData->pointer->otherserv.size()){
        if(serverData->whatsocket != serverData->pointer->otherserv[iter]){
            write(serverData->pointer->otherserv[iter],buf.c_str(),sizeof(buf));  //200
            }
        iter++;
      }
    }
    if(strcmp(vec[0].c_str(),"msg" ) == 0){
      unsigned iter=0;
      while(iter < serverData->pointer->otherserv.size()){
        for(int i=0;i<30;i++){
          if(serverData->pointer->other[i].clientFd == serverData->pointer->otherserv[iter] and serverData->whatsocket!=serverData->pointer->otherserv[iter] and serverData->pointer->other[i].channel == std::stoi(vec[1].c_str())){
            write(serverData->pointer->otherserv[iter],buf.c_str(),sizeof(buf));
            break;
          }
        }

        iter++;
      }
    }
  }
}






//TODO REMOVE IT / replace wih method
int index_check2(server* s){
  for (int i=0;i<10;i++){
		//printf("%d %d\n",i,s->clientList[i].clientFd);
    if(s->other[i].clientFd < 0){
        return i;
    }
  }
return -1;
}
