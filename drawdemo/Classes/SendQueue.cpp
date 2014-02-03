//
//  SendQueue.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-2-3.
//
//


#include "SendQueue.h"
#include "SessionManager.h"

pthread_mutex_t sharedSendQueueLock;

class SendQueueLifeManager_PThreadMutex {
    
private:
    pthread_mutex_t* m_pMutex;
public:
    SendQueueLifeManager_PThreadMutex(pthread_mutex_t* in_pMutex) : m_pMutex(in_pMutex) {
        pthread_mutex_init(m_pMutex, NULL);
    }
    
    ~SendQueueLifeManager_PThreadMutex() {
        pthread_mutex_destroy(m_pMutex);
    }
    
}__SendQueueLifeManager_sharedNotificationQueueLock(&sharedSendQueueLock);

class LifeCircleMutexLocker {
    
private:
    
    pthread_mutex_t* m_pMutex;
    
public:
    
    LifeCircleMutexLocker(pthread_mutex_t* in_pMutex) : m_pMutex(in_pMutex) {
        pthread_mutex_lock(m_pMutex);
    }
    
    ~LifeCircleMutexLocker() {
        pthread_mutex_unlock(m_pMutex);
    }
};

#define LifeCircleMutexLock(mutex) LifeCircleMutexLocker __locker__(mutex)

SendQueue * SendQueue::getSharedInstance()
{
    static SendQueue *queue = NULL;
    
    if (!queue) {
        queue = new SendQueue;
    }
    
    return queue;
}

void SendQueue::SendCommand( const int commandCount , string &commandString)
{
    LifeCircleMutexLock(&sharedSendQueueLock);
    
    NotificationArgs t_oNotiArgs;
    
    t_oNotiArgs.mCommandCount = commandCount;
    t_oNotiArgs.mCommandString = commandString;
    m_oVecCommands.push_back(t_oNotiArgs);
   
}

void SendQueue::Reset()
{
    LifeCircleMutexLock(&sharedSendQueueLock);
    m_oVecCommands.clear();
}

void SendQueue::actSendCommand()
{
    LifeCircleMutexLock(&sharedSendQueueLock);
    
    if (m_oVecCommands.size() <= 0)
    {
        return;
    }
    
    for (uint16_t i = 0; i < m_oVecCommands.size(); i ++) {
        NotificationArgs& tmp_oNotiArgs = m_oVecCommands[i];
        SessionManager::getSharedInstance()->CallSendSync(tmp_oNotiArgs.mCommandCount, tmp_oNotiArgs.mCommandString);
    }
    
    m_oVecCommands.clear();
}
