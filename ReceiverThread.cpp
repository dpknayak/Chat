#include "constants.h"
#include "ReceiverThread.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

ReceiverThread::ReceiverThread(const char* threadName, SenderThread* sender) {
    THREAD_NAME = threadName;
    mSenderThread = sender;
}
bool ReceiverThread::CreateThread() {
    if (!m_thread)
        m_thread = new thread(&ReceiverThread::Process, this);
    return true;
}

void ReceiverThread::Process() {
    fd_set readfds;
    struct timeval time_out;

    while (1)
    {
        ThreadMsg* msg = 0;
        {
            this_thread::sleep_for(200ms);
            for(std::map<int,int>::iterator iter = mUserMap.begin(); iter != mUserMap.end(); ++iter) {
                FD_ZERO(&readfds);
                time_out.tv_sec = 1;
                time_out.tv_usec = 0;
                int lFd = iter->second;
                FD_SET(lFd, &readfds);
                if (select(lFd + 1, &readfds,NULL,NULL,&time_out) >= 0) {
                    if (FD_ISSET(lFd, &readfds)) {
                        memset(mBuffer, 0, 1024);
                        int len = read( lFd , mBuffer, 1024);
                        if (len > 0) {
                            cout << "Msg is: " << mBuffer << " len is: " << len << endl; 
                            ChatData* data = parseMessage(mBuffer, len);
                            if (data != NULL) {
                                mSenderThread->PostMsg(MSG_POST_USER_DATA, data);
                            } else {
                                cout << "Message Dropped" << endl;
                            }
                        } else {
                            mSenderThread->PostMsg(MSG_DELETE_USER, new UserId(iter->first, lFd));
                            mUserMap.erase(iter);
                        }
                    }
                }
            }
            bool conti = true;
            m_mutex.lock();
            if (!m_queue.empty()) {
                conti = false;
                msg = m_queue.front();
                m_queue.pop();
            }
            m_mutex.unlock();

            if (conti)
                continue;
        }

        switch (msg->mId)
        {
            case MSG_POST_USER_DATA:
            {

                const ChatData* data = static_cast<const ChatData*>(msg->mObj);
                
                cout << data->mText << THREAD_NAME << endl;
                delete data;
                delete msg;
                break;
            }
            case MSG_NEW_USER:
            {
                UserId* user = (UserId*) msg->mObj;
                std::map<int, int>::iterator it;
                it = mUserMap.find(user->mId);
                if (it != mUserMap.end())
                    mUserMap.erase(it);
                mUserMap.insert(std::pair<int, int> (user->mId, user->mSocketId));
                break;
            }
            case MSG_EXIT_THREAD:
            {
                delete msg;
                std::unique_lock<std::mutex> lk(m_mutex);
                while (!m_queue.empty())
                {
                    msg = m_queue.front();
                    m_queue.pop();
                    delete msg;
                }

                cout << "Exit thread on " << THREAD_NAME << endl;
                return;
            }

            default:;
                //ASSERT();
        }
    }
}

ChatData* ReceiverThread::parseMessage(char* str, int len) {
    if (len < 11) // 1 ID TAG + 1 for len + 5 val of TAG + 1 TAG of MSG + 3 for len of MSG
        return NULL;
    int id;
    if ((str[0] - '0') == TAG_FOR_USER_ID) {
        id = getInteger(str, 2, 5);
    } else {
        return NULL;
    }
    int len1;
    if ((str[7] - '0') == TAG_FOR_MESSAGE) {
        len1 = getInteger(str, 8, 3);
        char lMsg[512] = {0,};
        strncpy(lMsg, str +11, len1);
        return new ChatData(id, lMsg, len1);
    }
    return NULL;
}

int ReceiverThread::getInteger(char* str, int start, int len) {
    int ret = 0;
    for (int i = 0; i < len; i++) {
        ret *= 10;
        ret += (str[start + i] - '0');
    }
    return ret;
}
void ReceiverThread::myStrCpy(char* str1, char* str2, int start, int len) {
    for (int i = 0; i < len; i++) {
        str1[i] = str2[i + start];
    }
}

void ReceiverThread::PostMsg(int msgType, void* data) {

    ThreadMsg* threadMsg = new ThreadMsg(msgType, data);

    m_mutex.lock();
    m_queue.push(threadMsg);
    m_mutex.unlock();
}

void ReceiverThread::ExitThread()
{
    if (!m_thread)
        return;

    ThreadMsg* threadMsg = new ThreadMsg(MSG_EXIT_THREAD, 0);

    {
        m_mutex.lock();
        m_queue.push(threadMsg);
        m_mutex.unlock();
    }

    m_thread->join();
    delete m_thread;
    m_thread = 0;
}

ReceiverThread::~ReceiverThread() {

}
