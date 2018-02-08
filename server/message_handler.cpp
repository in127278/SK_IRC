#include "message_handler.h"

void parse_command(sdata* thread_data, std::vector<std::string> &vec,std::string &msg){
  int msgtype=-1;
  if(strcmp(vec[0].c_str(),"/nick" ) == 0){
      if(vec.size()<2){

        thread_data->serv->writeMsg("Not enough arguments",thread_data->connected->fd);
      }
      if(check_nick(thread_data->serv,vec[1]) == false){

        thread_data->serv->writeMsg("Name is taken ",thread_data->connected->fd);

      }

      else{

        char localmsg[100]="";
        strcat(localmsg,thread_data->connected->nick);
        strcat(localmsg," has changed name to: ");
        strcat(localmsg,vec[1].c_str());
        strcat(localmsg," ");
        msg=thread_data->connected->nick;
        sendtolocal(thread_data->serv,thread_data->connected->channel,localmsg,sizeof(localmsg));
        strcpy(thread_data->connected->nick,vec[1].c_str());
        msgtype=0;
      }
    }
  else if(strcmp(vec[0].c_str(),"/join" ) == 0){
    if(vec.size()<2){
      thread_data->serv->writeMsg("Not enough arguments ",thread_data->connected->fd);
    }
    else{
      if(is_number(vec[1].c_str()) == true){
        char localmsg[100]="";
        strcat(localmsg,thread_data->connected->nick);
        strcat(localmsg," is leaving channel: ");
        strcat(localmsg,std::to_string(thread_data->connected->channel).c_str());
        strcat(localmsg," and joining channel: ");
        strcat(localmsg,vec[1].c_str());
        strcat(localmsg," ");

        sendtolocal(thread_data->serv,thread_data->connected->channel,localmsg,sizeof(localmsg));

        thread_data->connected->channel=atoi(vec[1].c_str());
        msgtype=1;
        msg=vec[1];
      }
    else{

        thread_data->serv->writeMsg("Incorrect channel ",thread_data->connected->fd);

      }
    }
  }
  else{
    char localmsg[255]="";
    strcat(localmsg,thread_data->connected->nick);
    strcat(localmsg,": ");
    strcat(localmsg,msg.c_str());
    strcat(localmsg," ");

    sendtolocal(thread_data->serv,thread_data->connected->channel,localmsg,sizeof(localmsg));
    msgtype=2;

    }
  if(msgtype>-1){

    thread_data->serv->sendtoall(thread_data->serv->prepare_client_message(msgtype,thread_data->connected,msg));

  }
}






  void sendtolocal(server* serv,int channel,const std::string &buf,int msgcount){
    pthread_mutex_lock(&serv->mut1);
    for(unsigned i = 0; i != serv->connected_clients.size(); i++) {
      if(serv->connected_clients[i]->channel == channel){
        serv->writeMsg(buf,serv->connected_clients[i]->fd);

      }
    }
    pthread_mutex_unlock(&serv->mut1);
  }


void parse_server_command(sdata* thread_data, std::vector<std::string> &vec,const std::string &buf){
  if(strcmp(vec[0].c_str(),"/add" ) == 0){

    client* connected;
    connected = new client;
    connected->fd=thread_data->connected->fd;
    strcpy(connected->nick,vec[1].c_str());
    connected->channel=std::stoi(vec[2].c_str());


    pthread_mutex_lock(&thread_data->serv->mut1);
    thread_data->serv->imported_clients.push_back(connected);
    pthread_mutex_unlock(&thread_data->serv->mut1);
    printf("New client added: %s %d \n",connected->nick,connected->channel);
    std::string localmsg;
    localmsg.append(vec[1].c_str());
    localmsg.append(" connected ");
    sendtolocal(thread_data->serv,connected->channel,localmsg,sizeof(localmsg));
    resend_to_others(thread_data,buf,vec);
  }
  if(strcmp(vec[0].c_str(),"/remove" ) == 0){
    printf("Removing client: %s \n",vec[1].c_str());
    int channel;
    pthread_mutex_lock(&thread_data->serv->mut1);
    for(unsigned i = 0; i != thread_data->serv->imported_clients.size(); i++) {
      if(strcmp(thread_data->serv->imported_clients[i]->nick,vec[1].c_str()) == 0){
          channel=thread_data->serv->imported_clients[i]->channel;
          thread_data->serv->imported_clients.erase(thread_data->serv->imported_clients.begin() + i);
          break;
      }
    }

    pthread_mutex_unlock(&thread_data->serv->mut1);
    std::string localmsg;
    localmsg.append(vec[1].c_str());
    localmsg.append(" disconnected ");
    sendtolocal(thread_data->serv,channel,localmsg,sizeof(localmsg));
    resend_to_others(thread_data,buf,vec);
  }
  if(strcmp(vec[0].c_str(),"/msg" ) == 0){
    int channel = std::stoi(vec[1].c_str());
    std::string edited_bufer;
    for(std::vector<int>::size_type i = 2; i != vec.size(); i++) {
      edited_bufer.append(vec[i].c_str());
      edited_bufer.append(" ");
    }

    sendtolocal(thread_data->serv,channel,edited_bufer,sizeof(edited_bufer));
    resend_to_others(thread_data,buf,vec);
  }
  if(strcmp(vec[0].c_str(),"/nickrequest" ) == 0){
    int channel;
    std::string nick;
    pthread_mutex_lock(&thread_data->serv->mut1);
    for(unsigned i = 0; i != thread_data->serv->imported_clients.size(); i++) {
      if(strcmp(thread_data->serv->imported_clients[i]->nick,vec[1].c_str()) == 0){
          nick=thread_data->serv->imported_clients[i]->nick;
          strcpy(thread_data->serv->imported_clients[i]->nick,vec[2].c_str());
          channel=thread_data->serv->imported_clients[i]->channel;
          break;
      }
    }
      pthread_mutex_unlock(&thread_data->serv->mut1);

      printf("Client updated \n");
      std::string localmsg;
      localmsg.append(vec[1].c_str());
      localmsg.append(" has changed name to: ");
      localmsg.append(vec[2].c_str());
      localmsg.append(" ");
      sendtolocal(thread_data->serv,channel,localmsg,sizeof(localmsg));
      resend_to_others(thread_data,buf,vec);
  }
  if(strcmp(vec[0].c_str(),"/joinrequest" ) == 0){
    int channel;
    pthread_mutex_lock(&thread_data->serv->mut1);
    for(unsigned i = 0; i != thread_data->serv->imported_clients.size(); i++) {
      if(strcmp(thread_data->serv->imported_clients[i]->nick,vec[1].c_str()) == 0){
          channel=thread_data->serv->imported_clients[i]->channel;
          thread_data->serv->imported_clients[i]->channel=std::stoi(vec[2].c_str());

          break;
      }
    }

    pthread_mutex_unlock(&thread_data->serv->mut1);

    printf("Client updated \n");
    std::string localmsg;
    localmsg.append(vec[1].c_str());
    localmsg.append(" is leaving channel: ");
    localmsg.append(std::to_string(channel));
    localmsg.append(" and joining channel: ");
    localmsg.append(vec[2].c_str());
    localmsg.append(" ");
    sendtolocal(thread_data->serv,channel,localmsg,sizeof(localmsg));
    resend_to_others(thread_data,buf,vec);

    }



}

void resend_to_others(sdata* thread_data,const std::string &buf,std::vector<std::string> &vec){
  pthread_mutex_lock(&thread_data->serv->mut1);
  if(thread_data->serv->otherservvec.size()>1){
    unsigned iter=0;
    while(iter < thread_data->serv->otherservvec.size()){
      if(thread_data->connected->fd != thread_data->serv->otherservvec[iter]->fd){

        thread_data->serv->writeMsg(buf,thread_data->serv->otherservvec[iter]->fd);
      }
      iter++;
      }
  }
  pthread_mutex_unlock(&thread_data->serv->mut1);
}
