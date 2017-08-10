#ifndef SENDER_THREAD_H_
#define SENDER_THREAD_H_
#include "ChatData.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <map>
class SenderThread {
public:
    SenderThread(const char*);
    ~SenderThread();

    bool CreateThread();
    void ExitThread();

    void PostMsg(int, void*);

private:

    void Process();

    std::thread* m_thread;
    std::queue<ThreadMsg*> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    const char* THREAD_NAME;
    std::map<int, int> mUserMap;
};

#endif
