#ifndef CHAT_DATA_H_
#define CHAT_DATA_H_
class ChatData{
public:
    int mId;
    char *mText;
    ChatData();
    ChatData(int, char*, int);
    ~ChatData();
};

class UserId {
public:
    int mId;
    int mSocketId;
    UserId(int id, int sock) {
        mId = id;
        mSocketId = sock;
    }
    ~UserId(){}
};
class ThreadMsg {
public:
    int mId;
    void* mObj;
    ThreadMsg (int id, void* obj) {
        mId = id;
        mObj = obj;
    }
};
#endif
