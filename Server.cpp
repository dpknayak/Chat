#include <cstring>
#include "Server.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <future>
#define DEBUG
#ifdef DEBUG
#include <iostream>

using namespace std;

#endif

using namespace std;

Server::Server(int port) {
    mPort = port;
    memset(&mAddress, 0, sizeof(mAddress));
    mAddress.sin_family = AF_INET;
    mAddress.sin_addr.s_addr = inet_addr("192.168.1.12");
    mAddress.sin_port = htons( mPort );
}

Server::~Server() {
}

bool Server::createServer() {

    if ((mSocket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        #ifdef DEBUG
        cout << "Init socket failed" << endl;
        #endif
        return false;
    }
    int opt = 1;
    // Forcefully attaching socket to the port 8080
    if (setsockopt(mSocket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) {
        #ifdef DEBUG
        cout << "setsockopt failed" << endl;
        #endif
        return false;
    }
    if (bind(mSocket_fd, (struct sockaddr *)&mAddress, 
                                 sizeof(mAddress))<0) {
        #ifdef DEBUG
        cout << "bind socket failed" << endl;
        #endif
        return false;
    }
    if (listen(mSocket_fd, 3) < 0) {
        #ifdef DEBUG
        cout << "listen socket failed" << endl;
        #endif
        return false;
    }
    return true;
}

void Server::acceptClients(void (*onConnect)(int, int)) {
    auto f = std::async(std::launch::async, [] {
        auto s = ""s;
        if (std::cin >> s) return s;
        return s;
    });
    int addrlen = sizeof(mAddress);
    fd_set readfds, readfds2;
    struct timeval time_out;

    while (f.wait_for(2s) != std::future_status::ready) {
        time_out.tv_sec = 1;
        time_out.tv_usec = 0;
        FD_ZERO(&readfds);
        FD_SET(mSocket_fd, &readfds);
        if (select(mSocket_fd + 1, &readfds,NULL,NULL,&time_out) == 0) {
            continue;
        }
        if (FD_ISSET(mSocket_fd, &readfds)) {
            int client = accept(mSocket_fd, (struct sockaddr *)&mAddress, 
                       (socklen_t*)&addrlen);
            cout << "Client Id is: " << client << endl;
            
            int i = 0;
            while(i < 2) {
                i++;
                time_out.tv_sec = 1;
                time_out.tv_usec = 0;
                FD_ZERO(&readfds2);
                FD_SET(client, &readfds2);
                if (select(client + 1, &readfds2,NULL,NULL,&time_out) == 0) {
                    continue;
                }
                break;
            }
            if (i  == 2) {
                close(client);
            } else {
                char data[10] = {0,};
                int len = read( client , data, 10);
                int id = getInteger(data, len);
                cout << id << endl;
                onConnect(id, client);
            }
        }
    }
    std::cout << "Terminating because you entered: " << f.get() << std::endl;
}
int Server::getInteger(char* str, int len) {
    int ret = 0;
    for (int i = 0; i < len; i++) {
        ret *= 10;
        ret += (str[i] - '0');
    }
    return ret;
}


