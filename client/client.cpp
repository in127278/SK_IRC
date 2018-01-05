#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <error.h>
#include <iostream>
#include <string.h>
#include <memory>
class socks{
public:
  int socker;
};

void *listen(void *arguments){
  class socks *args = (class socks *)arguments;
  char msg[200];
  while(1){
    int count=read(args->socker,msg,200);

    if(count>0){


      write(1,msg,count);
      //printf("\n");
    }

    if(count==0){
        printf("Closing: %d \n",args->socker);
        //close(args->clientFd);
    }
    //memset(&msg[0], 0, sizeof(&msg));
  }
}

int main(int argc, char ** argv){
    if(argc!=3)
        error(1,0,"Usage: %s <ip> <port>", argv[0]);

    addrinfo hints {};
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo *resolved;
    if(int err = getaddrinfo(argv[1], argv[2], &hints, &resolved))
        error(1, 0, "Resolving address failed: %s", gai_strerror(err));

    int sock = socket(resolved->ai_family, resolved->ai_socktype, resolved->ai_protocol);
    if(connect(sock, resolved->ai_addr, resolved->ai_addrlen))
        error(1,errno, "Failed to connect");
    freeaddrinfo(resolved);

    //const int msgCnt = 20;
    pthread_t thread1;
    socks nowy;
    nowy.socker=sock;
    pthread_create(&thread1, NULL, listen, (void *) &nowy);
    while(1){
	char* buf = new char[255];
    //	for(int i = 0; i < msgCnt; i++){
       // std::string msg(std::to_string(i));
       // msg.resize (dist(gen),'.');
  int r=read(1,buf,sizeof(buf));
	write(sock, buf, r);
	}
    close(sock);
    return 0;
}
