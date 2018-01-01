#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <error.h>
#include <iostream>


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

    const int msgCnt = 20;
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
