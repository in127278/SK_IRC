
#include "message_handler.cpp"
server::~server() {
}

server::server() {
}

std::string server::readMsg(int fd){
  int count;
  char buf[200]="";
  std::string msg;

  while((count = recv(fd, buf, 200, 0)) >= 0){
    if(count <= 0){
      return "";
    }
    //std::string temp(buf);
    msg.append(buf);
    if(msg[msg.length() - 1] == '\n')
    msg.pop_back();
    return msg;
  }
  return "";

}

bool server::checkifdc(client* connected,std::string &message){
  std::string result=this->readMsg(connected->fd);
  if( result == ""){
    if(connected->server == false){
      close(connected->fd);
      //resend to servers that client is removed (prepare message && send to all)
      this->sendtoall(this->prepare_message(1,connected));

      this->delete_client(connected);
      return false;
    }
    else{
      printf("Connection to server lost - deleting shared clients \n");
      this->delete_imported_client(connected);

      //resend_to_others

      this->delete_server(connected);
      close(connected->fd);

      return false;
    }

  }
  else{

    message=result;
    return true;
  }
}



void server::writeMsg(std::string msg, int fd){
	char buffer[200]="";
	while(msg.length() > 0){
		int lengthToSend = msg.length();
		if(lengthToSend > 200 - 1){
			lengthToSend = 200 - 1;
		}
		strcpy(buffer, msg.substr(0, lengthToSend).c_str());
		buffer[lengthToSend + 1] = '\n';
		int count = write(fd, buffer, lengthToSend);

		if(count == -1){
			//
		}
		msg = msg.substr(count);
	}
}

void server::init_server(char *address,int port){

	this->servFd=socket(AF_INET, SOCK_STREAM, 0);
	this->port=port;
	if(this->servFd == -1) error(1, errno, "socket failed");

	pthread_mutex_init(&this->mut1, NULL);
	sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
	inet_pton(AF_INET, address, &(serverAddr.sin_addr));

	int res = bind(this->servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res) error(1, errno, "bind failed");

	res = listen(this->servFd, 1);
	if(res) error(1, errno, "listen failed");

}
void server::delete_client(client* connected){
  pthread_mutex_lock(&this->mut1);
  for(unsigned i = 0; i != this->connected_clients.size(); i++) {
    if(connected->fd == this->connected_clients[i]->fd){
        this->connected_clients.erase(this->connected_clients.begin() + i);
        break;
    }
  }
  pthread_mutex_unlock(&this->mut1);
}

void server::delete_server(client* connected){
  pthread_mutex_lock(&this->mut1);
  for(unsigned i = 0; i != this->otherservvec.size(); i++) {
    if(connected->fd == this->otherservvec[i]->fd){
        this->otherservvec.erase(this->otherservvec.begin() + i);
        break;
    }
  }
  pthread_mutex_unlock(&this->mut1);
}

void server::delete_imported_client(client* connected_server){
  pthread_mutex_lock(&this->mut1);
  for(unsigned i = 0; i != this->imported_clients.size(); i++) {
    if(connected_server->fd == this->imported_clients[i]->fd){
        this->imported_clients.erase(this->imported_clients.begin() + i);
        i--;
    }
  }
  pthread_mutex_unlock(&this->mut1);
}


void server::manage_connected(std::vector<std::string> &vec, client* connected){
  if(strcmp(vec[0].c_str(),"serwer") == 0){

    connected->server=true;
    this->delete_client(connected);
    pthread_mutex_lock(&this->mut1);
    this->otherservvec.push_back(connected);
    pthread_mutex_unlock(&this->mut1);
  }
  else{
    connected->channel=0;
    memset(&connected->nick[0], 0, sizeof(&connected->nick));

    if(check_nick(this->connected_clients,vec[1]) == false){

      std::string name=generate_name(7);
      strcpy(connected->nick, name.c_str());
      std::string message = "Name is taken, try again using -> /nick \n";
      this->writeMsg(message,connected->fd);

    }
    else{
          strcpy(connected->nick,vec[1].c_str());
          std::string message = "Connected to server \n";
          this->writeMsg(message,connected->fd);

    }

  }

  vec.clear();

}


void *waiting_for_connection(void *arguments){
  class server *serv = (class server *)arguments;
  while(1){
      sockaddr_in clientAddr{0};
    	socklen_t clientAddrSize = sizeof(clientAddr);

    	auto clientFd = accept(serv->servFd, (sockaddr*) &clientAddr, &clientAddrSize);
    	if(clientFd == -1) error(1, errno, "accept failed");

      client* connected;
      connected = new client;
      connected->fd=clientFd;
      pthread_mutex_lock(&serv->mut1);
      serv->connected_clients.push_back(connected);
      pthread_mutex_unlock(&serv->mut1);
      sdata *data1;
      data1 = new sdata;
      data1->serv=serv;
      data1->connected=connected;

      pthread_t thread1;
      pthread_create(&thread1, NULL, client_main_thread, (void *) data1);
    }

}


void server::start_listening(char *server_to_ping){

    pthread_t thread1,thread2;
    pthread_create(&thread1, NULL, waiting_for_connection, (void *) this);

    if(server_to_ping!=NULL){
      this->addr=server_to_ping;
      pthread_create(&thread2, NULL, ping_other, (void *) this);

    }
    while(1){

    }
  }


uint16_t readPort(char * txt){
	char * ptr;
	auto port = strtol(txt, &ptr, 10);
	if(*ptr!=0 || port<1 || (port>((1<<16)-1))) error(1,0,"illegal argument %s", txt);
	return port;
}

void *ping_other(void *arguments){
  class server *serv = (class server *)arguments;
  printf("Starting pinging routine \n");
	adres* adr;
	adr = new adres;

  client* connected;
  connected = new client;
  connected->server=true;
  prepareforping(adr,serv,connected);
	while(1){
		adr->connected=connect(connected->fd, adr->resolved->ai_addr, adr->resolved->ai_addrlen);

		sleep(1);

		if(adr->connected == 0){
			printf("Connected to 'pinged' server %d\n",connected->fd);
      serv->writeMsg("serwer \n", connected->fd);


			sdata *thread_data;
			thread_data = new sdata;
			thread_data->serv=serv;

      thread_data->connected=connected;
      pthread_mutex_lock(&serv->mut1);
      serv->otherservvec.push_back(connected);
      pthread_mutex_unlock(&serv->mut1);

			//Sending currently connected clients to the other server
			pthread_t thread1;
			pthread_create(&thread1, NULL, exchange_clients, (void *) thread_data);

			std::vector<std::string> vec;
			//Read data from the pinged server until dc
			while(1){

        std::string command;
        bool check=thread_data->serv->checkifdc(thread_data->connected,command);
        if(check == false){
          printf("Closing socket from pinged server \n");
					sleep(1);
					prepareforping(adr,serv,connected);
          connected->fd=-1;
					adr->connected=-1;
          command.clear();
          vec.clear();
					break; //return to "connect" routine
        }

        split(command,vec);
        parse_server_command(thread_data,vec,command);
        command.clear();
        vec.clear();
      }

			}
		}

  pthread_exit((void *) -1);
}


void *client_main_thread(void *arguments){
  struct sdata *args = (class sdata *)arguments;
  std::vector<std::string> vec;
  while(1){
    std::string message(args->serv->readMsg(args->connected->fd));
    split(message,vec);
    args->serv->manage_connected(vec,args->connected);
    vec.clear();
    if(args->connected->server == true){
      printf("Server connected \n");

      pthread_t thread1;
			pthread_create(&thread1, NULL, exchange_clients, (void *) args);


      while(1){
          std::string command;
          bool check=args->serv->checkifdc(args->connected,command);
          if(check == false){
            printf("Client thread terminated \n");
            pthread_exit((void *) -1);
          }
          else{
            split(command,vec);
            parse_server_command(args,vec,command);
            command.clear();
            vec.clear();
          }


			}


    }
    else{
      printf("Client connected \n");
      //resend client to otherserv
      args->serv->sendtoall(args->serv->prepare_message(0,args->connected));

        while(1){
            std::string command;
            bool check=args->serv->checkifdc(args->connected,command);
            if(check == false){
              printf("Client thread terminated \n");
              pthread_exit((void *) -1);
            }
            split(command,vec);
            parse_command(args,vec,command);
            command.clear();
            vec.clear();
        }


    }

  }
pthread_exit((void *) -1);
}

void server::sendtoall(std::string message){
  if(this->otherservvec.size()>0){
    unsigned iter=0;
    while(iter < this->otherservvec.size()){
        printf("Sending this message to connected servers: %s %d\n",message.c_str(),this->otherservvec[iter]->fd);
        this->writeMsg(message,this->otherservvec[iter]->fd);


      iter++;
      }
    }
  }

std::string server::prepare_message(short message_type,client* client_to_send){

  std::string message;

  if(message_type == 0){
      message.append("/add ");
  }
  else{
      message.append("/remove ");
  }

  message.append(client_to_send->nick);
  message.append(" ");
  message.append(std::to_string(client_to_send->channel).c_str());
  message.append("\n");
  return message;
}

std::string server::prepare_client_message(short message_type, client* sender, std::string &message_to_send){

  std::string message;

  if(message_type == 0){
    message.append("/nickrequest ");
    message.append(sender->nick);


  }

  if(message_type == 1){
    message.append("/joinrequest ");
    message.append(sender->nick);
  }

  if(message_type == 2){
    message.append("/msg ");
    message.append(std::to_string(sender->channel));
    message.append(" ");
    message.append(sender->nick);
    message.append(":");

    }
  message.append(" ");
  message.append(message_to_send);
  message.append(" \n");

  return message;
}

void* exchange_clients(void *arguments){
	struct sdata *thread_data = (struct sdata *)arguments;
  for(unsigned i = 0; i != thread_data->serv->connected_clients.size(); i++) {
    thread_data->serv->writeMsg(thread_data->serv->prepare_message(0,thread_data->serv->connected_clients[i]),thread_data->connected->fd);
    printf("Sending client %s to connected server \n",thread_data->serv->connected_clients[i]->nick);
    }

  if(thread_data->serv->otherservvec.size()>1){
    for(unsigned i = 0; i != thread_data->serv->imported_clients.size(); i++) {
      if(thread_data->connected->fd != thread_data->serv->imported_clients[i]->fd){
        thread_data->serv->writeMsg(thread_data->serv->prepare_message(0,thread_data->serv->imported_clients[i]),thread_data->connected->fd);
        printf("Sending imported client %s to connected server \n",thread_data->serv->imported_clients[i]->nick);
      }
    }
  }
  pthread_exit((void *) -1);
}
