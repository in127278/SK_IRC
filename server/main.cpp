#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <signal.h>
#include <string.h>

#include "connection_manager.h"

#include "server.cpp"
#include "client_info.cpp"
#include "connection_manager.cpp"
// converts cstring to port
uint16_t readPort(char * txt);

// sets SO_REUSEADDR
void setReuseAddr(int sock);




threads_manager tmanager(4);

void *waiting_for_connection(void *arguments){
  class server *args = (class server *)arguments;
  while(1){

      sockaddr_in clientAddr{0};
    	socklen_t clientAddrSize = sizeof(clientAddr);
    	auto clientFd = accept(args->servFd, (sockaddr*) &clientAddr, &clientAddrSize);
    	if(clientFd == -1) error(1, errno, "accept failed");
      // TODO MUTEX LOCK TUTAJ

      int index = args->index_check();
      if (index != -1){
        args->clientList[index].clientFd=clientFd;
        args->clientList[index].id=index;
        args->clientList[index].channel=0;
        args->clientList[index].servFd=args->servFd;
        args->FdList[index].fd=clientFd;
        memset(&args->clientList[index].nick[0], 0, sizeof(&args->clientList[index].nick));
        memset(&args->clientList[index].msg[0], 0, sizeof(&args->clientList[index].nick));
        //printf("nick %s \n", args->clientList[index].nick);
        printf("index: %d client %d fd %d\n",index,args->FdList[index].fd,args->clientList[index].clientFd=clientFd);
        args->FdList[index].events=POLLIN;
        //char powitanie[50];
        //memset(&powitanie[0], 0, sizeof(&powitanie));
        //&powitanie="Connection established" ;
        write(args->clientList[index].clientFd,"Connection established",sizeof("Connection established"));

      }
      // TODO server full
      //TODO MUTEX UNLOCK TU
      // connected:
    }
}


void *poll_listener(void * arguments){
  class server *args = (class server *)arguments;
  while(1){
    poll(args->FdList,10,100);
  }
}
void sendtolocal(server* server_data,client_info* sender,int count){
  for(int i=0;i<10;i++){
    if(server_data->clientList[i].channel == sender->channel){
      write(server_data->clientList[i].clientFd,sender->msg,count);
      printf("Sending buf to %d from %d \n",server_data->clientList[i].clientFd,sender->clientFd);
    }
  }
}

void *read_listener(void *arguments){
  class client_info *args = (class client_info *)arguments;
  while(1){
    int count=read(args->clientFd,args->msg,100);

    if(count>0){
        if(args->msg=="/channel"){

        }

        sendtolocal(tmanager.pointer,args,count);
        //write(args->clientFd,args->msg,count-1);
        //printf("Message: %d %s %d\n",args->id,args->msg,count);
    }

    if(count==0){
        printf("Closing: %d %d \n",args->id,count);
        close(args->clientFd);
        tmanager.empty(args->id);
        args->reset();
        //tmanager.pointer->clientList[args->id].clientFd=-1;
        pthread_exit((void *) -1);

    }
    memset(&args->msg[0], 0, sizeof(&args->msg));
  }
}


void *basic_msghandler(void *arguments){
  class server *args = (class server *)arguments;
  while(1){
      for(int i=0;i<10;i++){

        if(args->FdList[i].revents==POLLIN && args->clientList[i].clientFd!=-1){
          if(tmanager.running[i]==0){
            tmanager.running[i]=1;
            pthread_create(&tmanager.thread_list[i], NULL, read_listener, (void *) &args->clientList[i]);
          }
        }

      }
  }
}



int main(int argc, char ** argv){
	// get and validate port number
	if(argc != 2) error(1, 0, "Need 1 arg (port)");
	auto port = readPort(argv[1]);

  server args;
  args.initialize(port);
	// prevent dead sockets from throwing pipe errors on write
	signal(SIGPIPE, SIG_IGN);

/****************************/
	pthread_t thread1,thread2,thread3;

  tmanager.pointer=&args;
	pthread_create(&thread1, NULL, waiting_for_connection, (void *) &args);
  pthread_create(&thread2, NULL, basic_msghandler, (void *) &args);
  pthread_create(&thread3, NULL, poll_listener, (void *) &args);
	while(true){
    // tutaj komunikacja między serwerami!
}
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
