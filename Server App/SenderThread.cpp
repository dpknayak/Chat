#include "constants.h"
#include "SenderThread.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include <sys/socket.h>

using namespace std;

bool debug = true;
SenderThread::SenderThread(const char* threadName) {
    THREAD_NAME = threadName;
}
bool SenderThread::CreateThread() {
    if (!m_thread)
        m_thread = new thread(&SenderThread::Process, this);
    return true;
}

void SenderThread::Process() {

    while (1)
    {
        ThreadMsg* msg = 0;
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            while (m_queue.empty()) {
                m_cv.wait(lk);
                
            }

            if (m_queue.empty()) {
                continue;
            }

            msg = m_queue.front();
            m_queue.pop();
        }

        if (debug) cout << "New Message: " << msg->mId << endl;
        switch (msg->mId) {
            case MSG_POST_USER_DATA: {

                const ChatData* data = static_cast<const ChatData*>(msg->mObj);
                if (mUserMap.count(data->mId) != 0) {
                    int val = mUserMap.find(data->mId)->second;
                    send(val , data->mText , strlen(data->mText) , 0 );
                }
                
                if (debug) cout << THREAD_NAME << ": " << data->mText << endl;

                delete data;
                delete msg;
                break;
            }
            case MSG_NEW_USER: {
                UserId* user = (UserId*) msg->mObj;
                std::map<int, int>::iterator it;
                it = mUserMap.find(user->mId);
                if (it != mUserMap.end())
                    mUserMap.erase(it);
                mUserMap.insert(std::pair<int, int> (user->mId, user->mSocketId));
                break;
            }
            case MSG_DELETE_USER: {
                UserId* user = (UserId*) msg->mObj;
                std::map<int, int>::iterator it;
                it = mUserMap.find(user->mId);
                if (it != mUserMap.end())
                    mUserMap.erase(it);
                break;
            }
            case MSG_EXIT_THREAD: {

                delete msg;
                std::unique_lock<std::mutex> lk(m_mutex);
                while (!m_queue.empty()) {
                    msg = m_queue.front();
                    m_queue.pop();
                    delete msg;
                }

                if (debug) cout << "Exit thread on " << THREAD_NAME << endl;
                return;
            }

            default:;
        }
    }
}


void SenderThread::PostMsg(int msgType, void* data) {

    ThreadMsg* threadMsg = new ThreadMsg(msgType, data);

    std::unique_lock<std::mutex> lk(m_mutex);
    m_queue.push(threadMsg);
    m_cv.notify_one();
}

void SenderThread::ExitThread() {
    if (!m_thread)
        return;

    ThreadMsg* threadMsg = new ThreadMsg(MSG_EXIT_THREAD, 0);

    {
        lock_guard<mutex> lock(m_mutex);
        m_queue.push(threadMsg);
        m_cv.notify_one();
    }

    m_thread->join();
    delete m_thread;
    m_thread = 0;
}

SenderThread::~SenderThread() {

}
