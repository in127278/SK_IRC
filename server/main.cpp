#include "server.cpp"



int main(int argc, char ** argv){

  server* serv;
  serv = new server;
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


	error(1,0,"this should never happen - main");
	//signal(SIGPIPE, SIG_IGN);
  //signal(SIGINT, ctrl_c);
/****************************/
}
