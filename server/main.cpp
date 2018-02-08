#include "server.cpp"

server* pointer;
void ctrl_c(int);

int main(int argc, char ** argv){

  server* serv;
  serv = new server;
  pointer=serv;
  signal(SIGINT, ctrl_c);
	// get and validate port number
	if(argc == 3 or argc == 4){

    char *address = argv[1];
  	auto port = readPort(argv[2]);
    serv->init_server(address,port);

  	if(argc == 4) {
  		serv->start_listening(argv[3]);
  	}

  	if(argc == 3){
      serv->start_listening(NULL);
  	}
	}

  else{
    error(1,0,"<ip> <port> <ip*> *optional");
  }
  while(1){

  }

  	error(1,0,"this should never happen - main");
	//signal(SIGPIPE, SIG_IGN);
  //signal(SIGINT, ctrl_c);
/****************************/
}
void ctrl_c(int){
  pthread_mutex_lock(&pointer->mut1);
  if(pointer->connected_clients.size()>0){
    for(unsigned i = 0; i != pointer->connected_clients.size(); i++) {
          close(pointer->connected_clients[i]->fd);
    }
  }
  if(pointer->otherservvec.size()>0){
    for(unsigned i = 0; i != pointer->otherservvec.size(); i++) {
          close(pointer->otherservvec[i]->fd);
    }
  }
  pthread_mutex_unlock(&pointer->mut1);
	printf("Closing server\n");
	exit(0);
}
