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
#include <poll.h>
#include <thread>
#include <unordered_set>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <memory>


// converts cstring to port
uint16_t readPort(char * txt);

// sets SO_REUSEADDR
void setReuseAddr(int sock);


class client_info {
public:
    int servFd;
    int clientFd;
    int id;
    int channel;
    char  msg[100];
    char nick[100];
    client_info(){
      //this->servFd=-1;
      this->clientFd=-1;
      this->channel=-1;
      this->id=-1;
    }
    void reset(){
      //this->servFd=-1;
      this->clientFd=-1;
      this->channel=-1;
      this->id=-1;
    }
  };
class server_info{
public:
  int servFd;
  //int clients_count;
  pollfd FdList[10];
  client_info clientList[10];
  int index_check(){
    for (int i=0;i<10;i++){
      if(this->clientList[i].clientFd == -1){
          return i;
      }
    }
  return -1;
  }
};

class threads_manager{
public:
  int size;
  pthread_t *thread_list;
  int *running;
  int* cl_index;
  threads_manager(int i){
    this->size=i;
    this->thread_list=new pthread_t[this->size];
    this->running=new int[this->size];
    this->cl_index=new int[this->size];
    //this->cl_pointer=NULL;
  }
  void empty(int index){
    this->running[index]=0;
  }

};

threads_manager tmanager(4);

void *waiting_for_connection(void *arguments){
  class server_info *args = (class server_info *)arguments;
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
        args->clientList[index].servFd=args->servFd;
        args->FdList[index].fd=clientFd;
        printf("index: %d client %d fd %d\n",index,args->FdList[index].fd,args->clientList[index].clientFd=clientFd);
        args->FdList[index].events=POLLIN;
        //args->clients_count+=1;
      }
      // TODO server full
      //TODO MUTEX UNLOCK TU
      // connected:
    	//printf("new connection from: %s:%hu (fd: %d)\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), clientFd);
      //printf("index: %d\n",index);
    }
}


void *poll_listener(void * arguments){
  class server_info *args = (class server_info *)arguments;
  while(1){
    poll(args->FdList,9,100);
  }
}

void *read_listener(void *arguments){
  class client_info *args = (class client_info *)arguments;
  while(1){

    int count=read(args->clientFd,args->msg,100);

    if(count>0){
      printf("Message: %d %s \n",args->id,args->msg);
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
  class server_info *args = (class server_info *)arguments;
  while(1){
      for(int i=0;i<10;i++){
        //printf("client id: %d %d %d\n",args->clientList[0].clientFd,args->FdList[0].fd,tmanager.running[0]);
        if(args->FdList[i].revents==POLLIN && args->clientList[i].clientFd!=-1){
        //  printf("client id: %d %d %d\n",args->clientList[0].clientFd,args->FdList[0].fd,tmanager.running[0]);
          if(tmanager.running[i]==0){

            tmanager.running[i]=1;
          //  printf("after close: %d %d\n",args->FdList[i].fd,args->clientList[i].clientFd);
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

	// create socket
	int servFd = socket(AF_INET, SOCK_STREAM, 0);
	if(servFd == -1) error(1, errno, "socket failed");

	// prevent dead sockets from throwing pipe errors on write
	signal(SIGPIPE, SIG_IGN);

	setReuseAddr(servFd);
	// bind to any address and port provided in arguments
	sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
	int res = bind(servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res) error(1, errno, "bind failed");

	// enter listening mode
	res = listen(servFd, 1);
	if(res) error(1, errno, "listen failed");

/****************************/
	pthread_t thread1,thread2,thread3;
  server_info args;
      args.servFd = servFd;
	pthread_create(&thread1, NULL, waiting_for_connection, (void *) &args);
  pthread_create(&thread2, NULL, basic_msghandler, (void *) &args);
  pthread_create(&thread3, NULL, poll_listener, (void *) &args);
	while(true){
    // tutaj komunikacja między serwerami???
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
