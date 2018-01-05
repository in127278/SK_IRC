#ifndef CLIENT_INFO_H
#define CLIENT_INFO_H

class client_info {
public:
    client_info();
    ~client_info();
    int servFd;
    int clientFd;
    int id;
    int channel;
    char  msg[100];
    char nick[100];
    struct sockaddr_in *IPv4Data;
    void reset();
};


#endif //CLIENT_INFO_H
