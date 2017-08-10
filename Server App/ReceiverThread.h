#ifndef RECEIVER_THREAD_H_
#define RECEIVER_THREAD_H_
#include "ChatData.h"
#include "SenderThread.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <map>
class ReceiverThread {
public:
    ReceiverThread(const char*, SenderThread*);
    ~ReceiverThread();

    bool CreateThread();
    void ExitThread();
    void PostMsg(int, void* data);

private:
    void Process();
    ChatData* parseMessage(char* str, int len);
    int getInteger(char* str, int start, int len);
    void myStrCpy(char* str1, char* str2, int start, int len);
    
    std::thread* m_thread;
    std::queue<ThreadMsg*> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    const char* THREAD_NAME;
    std::map<int, int> mUserMap;
    SenderThread *mSenderThread;
    char mBuffer[1024];
};

#endif
