#include "command_parser.cpp"


int main(int argc, char ** argv){
	// get and validate port number
	if(argc != 2) error(1, 0, "Need 1 arg (port)");
	auto port = readPort(argv[1]);

  //initialize(args.servFd,port);
	start_server(port);
	// prevent dead sockets from throwing pipe errors on write
	signal(SIGPIPE, SIG_IGN);
  //signal(SIGINT, ctrl_c);
/****************************/

	while(true){
    // tutaj komunikacja między serwerami!
  }

}


int index_check(server* s){
  for (int i=0;i<10;i++){
		printf("%d %d\n",i,s->clientList[i].clientFd);
    if(s->clientList[i].clientFd == -1){
        return i;
    }
  }
return -1;
}

void start_server(int port){
	server* serv;
	serv = new server;
	serv->servFd=socket(AF_INET, SOCK_STREAM, 0);
	if(serv->servFd == -1) error(1, errno, "socket failed");
  sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};

  int res = bind(serv->servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res) error(1, errno, "bind failed");

	// enter listening mode
	res = listen(serv->servFd, 1);
	if(res) error(1, errno, "listen failed");
	pthread_t thread1,thread2,thread3;
	pthread_create(&thread1, NULL, waiting_for_connection, (void *) serv);
  pthread_create(&thread2, NULL, basic_msghandler, (void *) serv);
  pthread_create(&thread3, NULL, poll_listener, (void *) serv);
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
        args->clientList[index].channel=0;
        args->clientList[index].servFd=args->servFd;
        args->FdList[index].fd=clientFd;
        memset(&args->clientList[index].nick[0], 0, sizeof(&args->clientList[index].nick));
        memset(&args->clientList[index].msg[0], 0, sizeof(&args->clientList[index].nick));
        //printf("index: %d client %d fd %d\n",index,args->FdList[index].fd,args->clientList[index].clientFd=clientFd);
        args->FdList[index].events=POLLIN;
        write(args->clientList[index].clientFd,"Connection established",sizeof("Connection established"));

      }
      // TODO server full
      //TODO MUTEX UNLOCK TU
      // connected:
    }
}

void *poll_listener(void * arguments){
  struct server *args = (struct server *)arguments;
  while(1){
    poll(args->FdList,10,100);
  }
}


void *basic_msghandler(void *arguments){
  struct server *args = (struct server *)arguments;
  while(1){
      for(int i=0;i<10;i++){

        if(args->FdList[i].revents==POLLIN && args->clientList[i].clientFd!=-1){
          if(args->running[i]==0){
            args->running[i]=1;
            start_reading(args,&args->clientList[i]);
          }
        }
      }
  }
}
void start_reading(server* arg,client_info* client){
	data_s *data1;
	data1 = new data_s;
	data1->pointer=arg;
	data1->client=client;
	pthread_t thread1;
	pthread_create(&thread1, NULL, read_listener, (void *) data1);
}

void *read_listener(void *arguments){
	//pthread_detach(pthread_self());
  struct data_s *args = (class data_s *)arguments;
	std::vector<std::string> vec;
  while(1){
    int count=read(args->client->clientFd,args->client->msg,100);

    if(count>0){
				split(args->client->msg,vec);
				parse_command(args,vec,count);
				vec.clear();
    }

    if(count==0){
        printf("Closing: %d %d \n",args->client->id,count);
				//TODO MUTEX
				int index = args->client->id;
        close(args->pointer->clientList[index].clientFd);
				args->pointer->clientList[index].reset();
				args->pointer->running[index]=0;

				//TODO MUTEX

        pthread_exit((void *) -1);

    }
    memset(&args->client->msg[0], 0, sizeof(&args->client->msg));
  }
}
/*
void ctrl_c(int){
	close(abc);
	printf("Closing server\n");
	exit(0);
}
*/
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
