#include "parsers/command_parser.cpp"

int main(int argc, char ** argv){
	// get and validate port number
	//if(argc != 2) error(1, 0, "Need 1 arg (port)");
	if(argc<3){
		error(1,0,"<ip> <port> <ip*> *optional");
	}
	char *address = argv[1];
	auto port = readPort(argv[2]);
	if(argc == 4) {
		char *server_to_ping = argv[3];
		start_server(address,port,server_to_ping);
	}
	if(argc == 3){
		start_server(address,port,NULL);
	}

	error(1,0,"this should never happen - main");
	//signal(SIGPIPE, SIG_IGN);
  //signal(SIGINT, ctrl_c);
/****************************/
}

void start_server(char *address,int port,char *server_to_ping){

	server* serv;
	serv = new server;

	serv->servFd=socket(AF_INET, SOCK_STREAM, 0);
	serv->port=port;
	if(serv->servFd == -1) error(1, errno, "socket failed");
	pthread_mutex_init(&serv->mut1, NULL);
	sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
	inet_pton(AF_INET, address, &(serverAddr.sin_addr));

	int res = bind(serv->servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res) error(1, errno, "bind failed");

	res = listen(serv->servFd, 1);
	if(res) error(1, errno, "listen failed");

	pthread_t thread1,thread2;
	pthread_create(&thread1, NULL, waiting_for_connection, (void *) serv);

	if(server_to_ping!=NULL){
		serv->addr=server_to_ping;
		pthread_create(&thread2, NULL, ping_other, (void *) serv);

	}
	//signal(SIGINT, &serv->get_ctrl());
	while(true){
			char buf[100]="";
			read(1,buf,10);
			if(strcmp(buf,"stop") == 0){
				pthread_mutex_lock(&serv->mut1);
				for(int i=0;i<MAX_CLIENTS;i++){
					close(serv->clientList[i].clientFd);
				}
				close(serv->servFd);
				pthread_mutex_unlock(&serv->mut1);
				exit(1);
			}
	}


}

void *ping_other(void *arguments){

	struct server *serv = (struct server *)arguments;

	adres* adr;
	adr = new adres;
	prepareforping(adr,serv);

	while(1){
		adr->connected=connect(serv->receivesocket, adr->resolved->ai_addr, adr->resolved->ai_addrlen);
		//sometimes first two clients are missing without sleep(1)
		sleep(1);
		if(adr->connected == 0){
			//printf("Connected %d\n",args->receivesocket);

			write(serv->receivesocket, "serwer", sizeof("serwer"));

			sdata *serverData;
			serverData = new sdata;
			serverData->pointer=serv;
			serverData->whatsocket=serv->receivesocket;
			serv->otherserv.push_back(serv->receivesocket);

			//Sending connected client structures to the other server
			pthread_t thread1;
			pthread_create(&thread1, NULL, initialize_send, (void *) serverData);

			std::vector<std::string> vec;
			//Read data from the pinged server until dc
			while(1){
				//int count = read_from_server(serverData,vec);
				if(read_from_server(serverData,vec) == 0){
					serv->otherserv.erase( std::remove( serv->otherserv.begin(), serv->otherserv.end(), serv->receivesocket), serv->otherserv.end());
					remove_clients_from_server(serverData,serv->receivesocket);
					close(serv->receivesocket);
					prepareforping(adr,serv);
					adr->connected=-1;
					break;
				}
			}
		}
	}
}

int read_from_server(sdata* serverData,std::vector<std::string> &vec){
	char buf[200]="";
	int count=read(serverData->whatsocket, buf, 200);
	printf("Data received: %s \n",buf);
	if(count>0){
		split(buf,vec);
		parse_server_command(serverData,vec,buf);

		vec.clear();
	}
	memset(&buf[0], 0, count);
	return count;
}

void* initialize_send(void *arguments){
	struct sdata *serverData = (struct sdata *)arguments;
	int iterator=0;
	while(serverData->pointer->clientList[iterator].id!=-1){
		if(serverData->pointer->clientList[iterator].id!=-2){
			add_client(serverData->whatsocket, &serverData->pointer->clientList[iterator]);
		}
		iterator+=1;
	}
	if(serverData->pointer->otherserv.size()>1){
		unsigned iter=0;
		while(iter < serverData->pointer->otherserv.size()){
			if(serverData->whatsocket != serverData->pointer->otherserv[iter]){
				for(int i=0;i<MAX_SERVER_CLIENTS;i++){
					if(serverData->pointer->otherserv[iter] == serverData->pointer->other[i].clientFd){
						add_client(serverData->whatsocket,&serverData->pointer->other[i]);
					}
				}
			}
			iter++;
		}
	}
	pthread_exit((void *) -1);
}

void *waiting_for_connection(void *arguments){
  struct server *serv = (struct server *)arguments;
  while(1){
      sockaddr_in clientAddr{0};
    	socklen_t clientAddrSize = sizeof(clientAddr);
    	auto clientFd = accept(serv->servFd, (sockaddr*) &clientAddr, &clientAddrSize);
    	if(clientFd == -1) error(1, errno, "accept failed");


			pthread_mutex_lock(&serv->mut1);
      int index = index_check(serv);
			if (index != -1){
				serv->clientList[index].clientFd=clientFd;
				serv->clientList[index].id=index;
				char *bufer=inet_ntoa(clientAddr.sin_addr);
				printf("Connected from: %s\n",bufer);
			}
			pthread_mutex_unlock(&serv->mut1);
      // connected:
			pthread_t thread1;
			sdata *data1;
			data1 = new sdata;
			data1->pointer=serv;
			data1->client=&serv->clientList[index];

			pthread_create(&thread1, NULL, read_listener, (void *) data1);
    }
}

void *read_listener(void *arguments){
	//pthread_detach(pthread_self());;
  struct sdata *args = (class sdata *)arguments;
	std::vector<std::string> vec;
	char buffer[100]="";

	if (read(args->client->clientFd,buffer,100) > 0){

		split(buffer,vec);


		//Checking if connected user is a client or server
		if(initialize_client(args,vec) ==  0){

			//Remove server from the local client list TODO MUTEX
			int fd = args->client->clientFd;
			int index = args->client->id;
			pthread_mutex_lock(&args->pointer->mut1);
			args->pointer->clientList[index].reset();
			pthread_mutex_unlock(&args->pointer->mut1);
			printf("Other server found \n");
			//Main loop (read) -> thread
			sdata *serverData;
			serverData = new sdata;
			serverData->pointer=args->pointer;
			serverData->whatsocket=fd;
			args->pointer->otherserv.push_back(fd);

			pthread_t thread1;
			pthread_create(&thread1, NULL, initialize_send, (void *) serverData);

			while(1){

				 //int count=read_from_server(serverData,vec);
				 if(read_from_server(serverData,vec) == 0){
					 pthread_mutex_lock(&serverData->pointer->mut1);
					 serverData->pointer->otherserv.erase( std::remove( serverData->pointer->otherserv.begin(), serverData->pointer->otherserv.end(), serverData->whatsocket), serverData->pointer->otherserv.end());
					 remove_clients_from_server(serverData,serverData->whatsocket);
					 close(serverData->whatsocket);
					 pthread_mutex_unlock(&serverData->pointer->mut1);
					 pthread_exit((void *) -1);
				 }

			}


			}
		//Connected user is  a client
		else{
			//Resending connected client to server/s
			if(args->pointer->otherserv.size()>0){
				unsigned iter=0;
				while(iter < args->pointer->otherserv.size()){
					add_client(args->pointer->otherserv[iter],args->client);
					iter++;
					}
				}
		//Client loop
	  while(1){

	    int count=read(args->client->clientFd,args->client->msg,100);

	    if(count>0){
					split(args->client->msg,vec);
					parse_command(args,vec,count,args->client->msg);
					vec.clear();
	    }

	    if(count==0){

					printf("Closing: %d \n",args->client->id);

					remove_client(args,args->client);
					pthread_mutex_lock(&args->pointer->mut1);
					int index = args->client->id;
	        close(args->pointer->clientList[index].clientFd);
					memset(&args->pointer->clientList[index].nick[0], 0, sizeof(args->pointer->clientList[index].nick));
					args->pointer->clientList[index].reset();
					pthread_mutex_unlock(&args->pointer->mut1);


	        pthread_exit((void *) -1);

	    	}
	    memset(&args->client->msg[0], 0, count);
	  }
		}
	}


	int index = args->client->id;
	close(args->pointer->clientList[index].clientFd);
	memset(&args->pointer->clientList[index].nick[0], 0, sizeof(args->pointer->clientList[index].nick));
	args->pointer->clientList[index].reset();

	pthread_exit((void *) -1);


}


void server::get_ctrl(int){
	server::closing_server();
	exit(0);
}

int index_check(server* s){
  for (int i=0;i<MAX_CLIENTS;i++){
		//printf("%d %d\n",i,s->clientList[i].clientFd);
    if(s->clientList[i].clientFd == -1){
        return i;
    }
  }
return -1;
}

void server::closing_server(){
	pthread_mutex_lock(&this->mut1);
	for(int i=0;i<MAX_CLIENTS;i++){
		close(this->clientList[i].clientFd);
	}
	close(this->servFd);
	pthread_mutex_unlock(&this->mut1);
	printf("Closing server \n");
}


uint16_t readPort(char * txt){
	char * ptr;
	auto port = strtol(txt, &ptr, 10);
	if(*ptr!=0 || port<1 || (port>((1<<16)-1))) error(1,0,"illegal argument %s", txt);
	return port;
}

void setReuseAddr(int sock){
	const int one = 1;
	int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	if(res) error(1,errno, "setsockopt failed");
}
