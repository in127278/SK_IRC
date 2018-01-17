#include "command_parser.cpp"


int main(int argc, char ** argv){
	// get and validate port number
	//if(argc != 2) error(1, 0, "Need 1 arg (port)");
	char *address = argv[1];
	auto port = readPort(argv[2]);
	if(argc == 4) {
	char *source = argv[3];
	start_server(address,port,source);
	}
	else{
		start_server(address,port,NULL);
	}
	//initialize(args.servFd,port);
	// prevent dead sockets from throwing pipe errors on write
	signal(SIGPIPE, SIG_IGN);
  //signal(SIGINT, ctrl_c);
/****************************/

	while(true){
    // tutaj komunikacja między serwerami!
  }

}

void start_server(char *address,int port,char *source){

	server* serv;
	serv = new server;

	serv->servFd=socket(AF_INET, SOCK_STREAM, 0);
	serv->port=port;
	if(serv->servFd == -1) error(1, errno, "socket failed");

	sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
	inet_pton(AF_INET, address, &(serverAddr.sin_addr));

	int res = bind(serv->servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res) error(1, errno, "bind failed");
	// enter listening mode
	res = listen(serv->servFd, 1);
	if(res) error(1, errno, "listen failed");

	pthread_t thread1,thread2;
	pthread_create(&thread1, NULL, waiting_for_connection, (void *) serv);
	if(source!=NULL){
		serv->addr=source;
		pthread_create(&thread2, NULL, ping_other, (void *) serv);
	}
}

void *ping_other(void *arguments){

	struct server *args = (struct server *)arguments;

	adres* adr;
	adr = new adres;
	prepareforping(adr,args);

	while(1){
		if(adr->connected==0){
			int connected=connect(args->receivesocket, adr->resolved->ai_addr, adr->resolved->ai_addrlen);

			if(connected == 0){
					adr->connected=1;
					printf("Connected %d\n",args->receivesocket);
						//MUTEX
					write(args->receivesocket, "serwer", sizeof("serwer"));

					sdata *serverData;
					serverData = new sdata;
					serverData->pointer=args;
					serverData->whatsocket=args->receivesocket;

					pthread_t thread1;
					pthread_create(&thread1, NULL, init_send, (void *) serverData);

					std::vector<std::string> vec;
					args->otherserv.push_back(args->receivesocket);

					while(1){
							int count = readingfrombuffer(serverData,vec);

						if(count==0){
							adr->connected=0;
							args->otherserv.erase( std::remove( args->otherserv.begin(), args->otherserv.end(), args->receivesocket), args->otherserv.end());
							close(args->receivesocket);
							prepareforping(adr,args);
							connected=-1;
							break;
						}
					}
			}
		}
	}
}
int readingfrombuffer(sdata* serverData,std::vector<std::string> &vec){
	char buf[200]="";
	int count=read(serverData->whatsocket, buf, 200);
	if(count>0){
		split(buf,vec);
		parse_server_command(serverData,vec);
		printf("Przyszło z bufera: %s\n",buf);
		memset(&buf[0], 0, count);
		vec.clear();
	}
	return count;
}

void* init_send(void *arguments){
	struct sdata *args = (struct sdata *)arguments;
	int iterator=0;
	while(args->pointer->clientList[iterator].id!=-1){
		AddClient(args->whatsocket, &args->pointer->clientList[iterator]);
		iterator+=1;
	}
	pthread_exit((void *) -1);
}

void *waiting_for_connection(void *arguments){
  struct server *args = (struct server *)arguments;
  while(1){
      sockaddr_in clientAddr{0};
    	socklen_t clientAddrSize = sizeof(clientAddr);
    	auto clientFd = accept(args->servFd, (sockaddr*) &clientAddr, &clientAddrSize);
    	if(clientFd == -1) error(1, errno, "accept failed");

			// TODO MUTEX LOCK TUTAJ
      int index = index_check(args);
			if (index != -1){
			args->clientList[index].clientFd=clientFd;
			args->clientList[index].id=index;
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
			data1->pointer=args;
			data1->client=&args->clientList[index];
			//TODO starting client or server here
			pthread_create(&thread1, NULL, read_listener, (void *) data1);
    }
}

void *read_listener(void *arguments){
	//pthread_detach(pthread_self());;
  struct data_s *args = (class data_s *)arguments;
	std::vector<std::string> vec;
	char buffer[100]="";
	int user=0;
	printf("ClientFd %d\n",args->client->clientFd);
	int count=read(args->client->clientFd,buffer,100);
	if(count>0){
			split(buffer,vec);
			user=initialize_client(args,vec,count);
			vec.clear();
	}
	if(user ==  0){
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
		pthread_create(&thread1, NULL, init_send, (void *) serverData);
		while(1){

			 int count=readingfrombuffer(serverData,vec);
			 if(count==0){
				 serverData->pointer->otherserv.erase( std::remove( serverData->pointer->otherserv.begin(), serverData->pointer->otherserv.end(), serverData->whatsocket), serverData->pointer->otherserv.end());
				 close(serverData->whatsocket);
				 pthread_exit((void *) -1);
			 }

		}


		}

	else{
		if(args->pointer->otherserv.size()>0){
			unsigned iter=0;
			while(iter < args->pointer->otherserv.size()){
				AddClient(args->pointer->otherserv[iter],args->client);
				iter++;
		}
		}
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
