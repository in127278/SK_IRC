#include "command_parser.h"

void parse_command(sdata* serverData, std::vector<std::string> &vec,int count,const std::string &msg){
  if(strcmp(vec[0].c_str(),"/nick" ) == 0){
      if(vec.size()<2){
        write(serverData->client->clientFd,"Not enough arguments",sizeof("Not enough arguments"));
      }
      if(check_nick(serverData,vec[1]) == false){

        write(serverData->client->clientFd,"Name is taken",sizeof("Name is taken"));

      }
      else{

        char str[100]="";
        strcat(str,serverData->client->nick);
        strcat(str," has changed name to: ");
        strcat(str,vec[1].c_str());
        sendtolocal(serverData->pointer,serverData->client->channel,str,sizeof(str));
        send_to_nearby(serverData->pointer,serverData->client,vec[1].c_str(),sizeof(vec[1].c_str()),vec[0]);
        strcpy(serverData->client->nick,vec[1].c_str());

      }
    }
  else if(strcmp(vec[0].c_str(),"/join" ) == 0){
    if(vec.size()<2){
      write(serverData->client->clientFd,"Not enough arguments",sizeof("Not enough arguments"));
    }
    else{
       if(is_number(vec[1].c_str()) == true){
          char str[100]="";
          strcat(str,serverData->client->nick);
          strcat(str," is leaving channel: ");
          strcat(str,std::to_string(serverData->client->channel).c_str());
          strcat(str," and joining channel: ");
          strcat(str,vec[1].c_str());
          //write(serverData->client->clientFd,str,sizeof(str));
          sendtolocal(serverData->pointer,serverData->client->channel,str,sizeof(str));
          send_to_nearby(serverData->pointer,serverData->client,vec[1].c_str(),sizeof(vec[1].c_str()),vec[0]);
          serverData->client->channel=atoi(vec[1].c_str());
        }
        else{
          write(serverData->client->clientFd,"Incorrect channel",sizeof("Incorrect channel"));
        }
      }
  }
  else{
    char buf[255]="";
    strcat(buf,serverData->client->nick);
    strcat(buf,": ");
    strcat(buf,serverData->client->msg);
    int msgcount=count+sizeof(serverData->client->nick)+sizeof(": ");
    sendtolocal(serverData->pointer,serverData->client->channel,buf,msgcount);
    std::string name="msg";
    send_to_nearby(serverData->pointer,serverData->client,buf,msgcount,name);
  }

}

void sendtolocal(server* server_data,int channel,const std::string &buf,int msgcount){
  for(int i=0;i<MAX_CLIENTS;i++){
    if(server_data->clientList[i].channel == channel){
      write(server_data->clientList[i].clientFd,buf.c_str(),msgcount);
      printf("Sending msg to %d from \n",server_data->clientList[i].clientFd);
    }
  }
}

//OBSŁUGA NICK JOIN \/
void send_to_nearby(server* server_data,client_info* sender,const std::string &buf,int msgcount,std::string &command){
  if(server_data->otherserv.size()>0){
    unsigned iter=0;
    while(iter < server_data->otherserv.size()){
      if(strcmp(command.c_str(),"msg") == 0){
        for(int i=0;i<MAX_SERVER_CLIENTS;i++){
          if(server_data->other[i].clientFd == server_data->otherserv[iter] and server_data->other[i].channel == sender->channel){
            char bufe[200]="";
            strcat(bufe,"msg ");
            strcat(bufe,std::to_string(sender->channel).c_str());
            strcat(bufe," ");
            strcat(bufe,buf.c_str());
            write(server_data->otherserv[iter], bufe, sizeof(bufe));
            printf("Sending msg to server %d from %d \n",server_data->otherserv[iter],sender->clientFd);
            break;
          }
        }
      }
      if(strcmp(command.c_str(),"/nick") == 0){
        char bufe[200]="";
        strcat(bufe,"/nickrequest ");
        strcat(bufe,(sender->nick));
        strcat(bufe," ");
        strcat(bufe,buf.c_str());
        write(server_data->otherserv[iter], bufe, sizeof(bufe));
        printf("Sending nickrequest to server %d from %d \n",server_data->otherserv[iter],sender->clientFd);
      }
      if(strcmp(command.c_str(),"/join") == 0){
        char bufe[200]="";
        strcat(bufe,"/joinrequest ");
        strcat(bufe,(sender->nick));
        strcat(bufe," ");
        strcat(bufe,buf.c_str());
        write(server_data->otherserv[iter], bufe, sizeof(bufe));
        printf("Sending joinrequest to server %d from %d \n",server_data->otherserv[iter],sender->clientFd);
      }
      if(strcmp(command.c_str(),"/remove") == 0){
        char bufer[200]="";
        strcat(bufer,"/remove ");
        strcat(bufer,sender->nick);
        write(server_data->otherserv[iter], bufer, sizeof(bufer));
        printf("Sending removerequest %s to server %d from %d \n",bufer,server_data->otherserv[iter],sender->clientFd);
      }
      iter++;
    }
  }
}

int initialize_client(sdata* serverData, std::vector<std::string> &vec){
  if(strcmp(vec[0].c_str(),"serwer") == 0){
    vec.clear();
    return 0;
  }
  else {

    serverData->client->channel=0;

    memset(&serverData->client->nick[0], 0, sizeof(&serverData->client->nick));
    memset(&serverData->client->msg[0], 0, sizeof(&serverData->client->msg));
    if(check_nick(serverData,vec[1]) == false){
      //char str[50]="";
      //strcat(str,"anonymous");
      std::string name=generate_name(7);
      strcpy(serverData->client->nick,name.c_str());
      write(serverData->client->clientFd,"Name is taken, try again using -> /nick ",sizeof("Name is taken"));
    }
    else{
          strcpy(serverData->client->nick,vec[1].c_str());
          write(serverData->client->clientFd,"Connected to server",sizeof("Connected to server"));
    }



    vec.clear();
    return 1;
  }
}

void parse_server_command(sdata* serverData, std::vector<std::string> &vec,const std::string &buf){
  if(strcmp(vec[0].c_str(),"add" ) == 0){

    pthread_mutex_lock(&serverData->pointer->mut1);

    int index2=index_check2(serverData->pointer);
    serverData->pointer->other[index2].id=index2;
    strcpy(serverData->pointer->other[index2].nick,vec[1].c_str());
    int channel = std::stoi(vec[2].c_str());
    serverData->pointer->other[index2].channel=channel;
    serverData->pointer->other[index2].clientFd=serverData->whatsocket;

    pthread_mutex_unlock(&serverData->pointer->mut1);

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
  if(strcmp(vec[0].c_str(),"/nickrequest" ) == 0){
    pthread_mutex_lock(&serverData->pointer->mut1);
    for(int i=0;i<MAX_SERVER_CLIENTS;i++){
      if(strcmp(serverData->pointer->other[i].nick,vec[1].c_str()) ==0){
        strcpy(serverData->pointer->other[i].nick,vec[2].c_str());
        char msg[200]="";
        strcat(msg,vec[1].c_str());
        strcat(msg," has changed name to: ");
        strcat(msg,serverData->pointer->other[i].nick);
        //printf("%s \n",msg);
         pthread_mutex_unlock(&serverData->pointer->mut1);
        sendtolocal(serverData->pointer,serverData->pointer->other[i].channel,msg,sizeof(msg));
        break;
      }
    }
    //sendtolocal(serverData->pointer,channel,edited_bufer,sizeof(edited_bufer));
    resend_to_others(serverData,buf,vec);
  }
  if(strcmp(vec[0].c_str(),"/joinrequest" ) == 0){
    pthread_mutex_lock(&serverData->pointer->mut1);
    for(int i=0;i<MAX_SERVER_CLIENTS;i++){
      if(strcmp(serverData->pointer->other[i].nick,vec[1].c_str()) == 0){
        char msg[200]="";
        strcat(msg,serverData->pointer->other[i].nick);
        strcat(msg," is leaving channel: ");
        strcat(msg,std::to_string(serverData->pointer->other[i].channel).c_str());
        strcat(msg," and joining channel: ");
        strcat(msg,vec[2].c_str());
        printf("%s \n",msg);
        int channel_copy = serverData->pointer->other[i].channel;
        serverData->pointer->other[i].channel=std::stoi(vec[2].c_str());

        pthread_mutex_unlock(&serverData->pointer->mut1);

        sendtolocal(serverData->pointer,channel_copy,msg,sizeof(msg));
        char notice[200]="";
        strcat(notice,serverData->pointer->other[i].nick);
        strcat(notice," has joined your channel!");
        sendtolocal(serverData->pointer,serverData->pointer->other[i].channel,notice,sizeof(notice));
        break;
      }
    }
    //sendtolocal(serverData->pointer,channel,edited_bufer,sizeof(edited_bufer));
    resend_to_others(serverData,buf,vec);
  }
  if(strcmp(vec[0].c_str(),"/remove" ) == 0){
    pthread_mutex_lock(&serverData->pointer->mut1);
    for(int i=0;i<MAX_SERVER_CLIENTS;i++){
      if(strcmp(serverData->pointer->other[i].nick,vec[1].c_str()) == 0){
        serverData->pointer->other[i].reset();
        memset(&serverData->pointer->other[i].nick[0], 0, sizeof(serverData->pointer->other[i].nick));

        pthread_mutex_unlock(&serverData->pointer->mut1);
        break;
      }
    }
    //sendtolocal(serverData->pointer,channel,edited_bufer,sizeof(edited_bufer));
    resend_to_others(serverData,buf,vec);
  }
}

void add_client(int fd, client_info* client){
  char buf[200]="";
  strcat(buf,"add ");
  strcat(buf,client->nick);
  strcat(buf," ");
  strcat(buf,std::to_string(client->channel).c_str());
  int count=write(fd, buf, sizeof(buf));
  printf("sending %d %s %d\n",fd, buf,count);
  memset(&buf[0], 0, sizeof(buf));
}

void remove_clients_from_server(sdata* serverData,int fd){
  for(int iter=0;iter<MAX_SERVER_CLIENTS;iter++){
    if(serverData->pointer->other[iter].clientFd == fd){
      //remove_client(serverData,&serverData->pointer->other[iter]);
      printf("Removing %s %d \n",serverData->pointer->other[iter].nick,serverData->pointer->other[iter].clientFd);
      serverData->pointer->other[iter].clientFd=-2;
      serverData->pointer->other[iter].channel=-1;
      memset(&serverData->pointer->other[iter].nick[0], 0, sizeof(&serverData->pointer->other[iter].nick));
    }
  }

}
void remove_client(sdata* serverData,client_info* client){

  char buf[200]="";
  strcat(buf,"/remove ");
  strcat(buf,client->nick);
  strcat(buf," ");
  std::string name="/remove";
  send_to_nearby(serverData->pointer,serverData->client,buf,sizeof(buf),name);
  memset(&buf[0], 0, sizeof(buf));
}
void resend_to_others(sdata* serverData,const std::string &buf,std::vector<std::string> &vec){
  if(serverData->pointer->otherserv.size()>1){
    if((strcmp(vec[0].c_str(),"add" ) == 0) or (strcmp(vec[0].c_str(),"/nickrequest" ) == 0) or (strcmp(vec[0].c_str(),"/joinrequest" ) == 0) or (strcmp(vec[0].c_str(),"/remove" ) == 0)){
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
        for(int i=0;i<MAX_SERVER_CLIENTS;i++){
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

bool check_nick(sdata* serverData,std::string &buf){
  for(int i=0;i<MAX_CLIENTS;i++){
    if(serverData->pointer->clientList[i].clientFd>-1){
      if(strcmp(serverData->pointer->clientList[i].nick,buf.c_str()) == 0){
        return false;

      }
    }
  }

  for(int i=0;i<MAX_SERVER_CLIENTS;i++){
    if(serverData->pointer->other[i].clientFd>-1){
      if(strcmp(serverData->pointer->other[i].nick,buf.c_str()) == 0){
        return false;
      }
    }
  }

  return true;

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

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

//TODO REMOVE IT / replace wih method
int index_check2(server* s){
  for (int i=0;i<MAX_CLIENTS;i++){
		//printf("%d %d\n",i,s->clientList[i].clientFd);
    if(s->other[i].clientFd < 0){
        return i;
    }
  }
return -1;
}
