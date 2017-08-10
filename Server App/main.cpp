#include <iostream>
#include <cstring>
#include "SenderThread.h"
#include "ReceiverThread.h"
#include "ChatData.h"
#define DEBUG
#include "Server.h"
#include "constants.h"
using namespace std;
SenderThread mSender("Sender Thread");
ReceiverThread mReceiver("Receiver Thread", &mSender);
Server mServer(8080);

void onConnect(int userId, int fd);
int main() {
    if (!mServer.createServer()) {
        cout << "Server Creation Failed..." << endl;
        return 1;
    }
    cout << "Server is Ready...." << endl;
    mSender.CreateThread();
    mReceiver.CreateThread();

    cout << "Enter any Key to terminate" << endl;
    mServer.acceptClients(onConnect);

    this_thread::sleep_for(2s);
    mSender.ExitThread();
    mReceiver.ExitThread();
    return 0;
}
void onConnect(int userId, int fd) {
    mSender.PostMsg(MSG_NEW_USER, new UserId(userId, fd));
    mReceiver.PostMsg(MSG_NEW_USER, new UserId(userId, fd));
}
