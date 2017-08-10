#ifndef _SERVER_H_
#define _SERVER_H_
#include <netinet/in.h>

typedef struct sockaddr_in SocketAddr;
class Server {
public:
    Server(int);
    ~Server();
    bool createServer();
    void acceptClients(void (*onConnect)(int, int));
    
private:
    int mPort;
    int mSocket_fd;
    SocketAddr mAddress;
    int* mClient_fd;
    int getInteger(char* str, int len);
};

#endif
