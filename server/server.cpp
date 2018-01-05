#include "server.h"

server::~server() {

}
server::server() {

}

int server::index_check(){
  for (int i=0;i<10;i++){
    if(this->clientList[i].clientFd == -1){
        return i;
    }
  }
return -1;
}
void server::initialize(int port){
  this->servFd = socket(AF_INET, SOCK_STREAM, 0);
  if(this->servFd == -1) error(1, errno, "socket failed");

  sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};

  int res = bind(this->servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res) error(1, errno, "bind failed");

	// enter listening mode
	res = listen(this->servFd, 1);
	if(res) error(1, errno, "listen failed");
}
