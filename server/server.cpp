#include "parsers/command_parser.cpp"

int main(int argc, char ** argv){
	// get and validate port number
	//if(argc != 2) error(1, 0, "Need 1 arg (port)");
	char *address = argv[1];
	auto port = readPort(argv[2]);

	if(argc == 4) {
		char *server_to_ping = argv[3];
		start_server(address,port,server_to_ping);
	}
	else{
		start_server(address,port,NULL);
	}
	// prevent dead sockets from throwing pipe errors on write
	signal(SIGPIPE, SIG_IGN);
  //signal(SIGINT, ctrl_c);
/****************************/

	while(true){

  }

}

void start_server(char *address,int port,char *server_to_ping){

	server* serv;
	serv = new server;

	serv->servFd=socket(AF_INET, SOCK_STREAM, 0);
	serv->port=port;
	if(serv->servFd == -1) error(1, errno, "socket failed");

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
				for(int i=0;i<30;i++){
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

			// TODO MUTEX LOCK TUTAJ
      int index = index_check(serv);
			if (index != -1){
				serv->clientList[index].clientFd=clientFd;
				serv->clientList[index].id=index;
				char *bufer=inet_ntoa(clientAddr.sin_addr);
				printf("Connected from: %s\n",bufer);
			}
			//write(args->clientList[index].clientFd,"Connection established",sizeof("Connection established"));
      // TODO server full
      //TODO MUTEX UNLOCK TU
      // connected:
			pthread_t thread1;
			data_s *data1;
			data1 = new data_s;
			data1->pointer=serv;
			data1->client=&serv->clientList[index];
			//TODO starting client or server here
			pthread_create(&thread1, NULL, read_listener, (void *) data1);
    }
}

void *read_listener(void *arguments){
	//pthread_detach(pthread_self());;
  struct data_s *args = (class data_s *)arguments;
	std::vector<std::string> vec;
	char buffer[100]="";

	printf("ClientFd %d\n",args->client->clientFd);
	//int count=read(args->client->clientFd,buffer,100);
	if (read(args->client->clientFd,buffer,100) > 0){

		split(buffer,vec);
		//user=initialize_client(args,vec,count);
		//vec.clear();

		//Checking if connected user is a client or server
		if(initialize_client(args,vec) ==  0){

			//Remove server from the local client list TODO MUTEX
			int fd = args->client->clientFd;
			int index = args->client->id;
			args->pointer->clientList[index].reset();
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
					 serverData->pointer->otherserv.erase( std::remove( serverData->pointer->otherserv.begin(), serverData->pointer->otherserv.end(), serverData->whatsocket), serverData->pointer->otherserv.end());
					 remove_clients_from_server(serverData,serverData->whatsocket);
					 close(serverData->whatsocket);

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
					parse_command(args,vec,count);
					vec.clear();
	    }

	    if(count==0){
	        //printf("Closing: %d %d \n",args->client->id,count);
					//TODO MUTEX
					int index = args->client->id;
	        close(args->pointer->clientList[index].clientFd);
					args->pointer->clientList[index].reset();

					//TODO MUTEX

	        pthread_exit((void *) -1);

	    	}
	    memset(&args->client->msg[0], 0, count);
	  }
		}
	}


	//TODO DELETE  ClientFd

	pthread_exit((void *) -1);


}
/*
void ctrl_c(int){
	close(abc);
	printf("Closing server\n");
	exit(0);
}
*/
int index_check(server* s){
  for (int i=0;i<10;i++){
		//printf("%d %d\n",i,s->clientList[i].clientFd);
    if(s->clientList[i].clientFd == -1){
        return i;
    }
  }
return -1;
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
