#ifndef CLIENT_INFO_H
#define CLIENT_INFO_H

class client_info {
public:
    client_info();
    ~client_info();
    int clientFd;
    int id;
    int channel;
    char  msg[100];
    char nick[50];
    void reset();
};


#endif //CLIENT_INFO_H
