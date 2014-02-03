//
//  MTNotificationQueue.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-23.
//
//

#ifndef __Demo__MTNotificationQueue__
#define __Demo__MTNotificationQueue__

#include "cocos2d.h"
#include <pthread.h>
#include <vector>

using namespace cocos2d;
using namespace std;

class MTNotificationQueue : public CCObject {
    
public:
    
    static MTNotificationQueue* sharedInstance();
    
    void repostNotifications(float in_fDeltaTime);
    
    void postNotification(const char* in_pArrCharName, CCObject* in_pCcObj);
    
private:
    
    MTNotificationQueue();
    
    ~MTNotificationQueue();
    
private:
    
    static MTNotificationQueue* sm_pInstance;
    
    class CGarbo {
    public:
        ~CGarbo() {
            if (MTNotificationQueue::sm_pInstance) {
                delete MTNotificationQueue::sm_pInstance;
            }
            sm_pInstance = NULL;
        }
    };
    
    static CGarbo sm_oCGarbo;
    
    typedef struct {
        string m_oStrName;
        CCObject* m_pCcObj;
    } NotificationArgs;
    
    vector<NotificationArgs> m_oVecNotifications;
};

#endif /* defined(__Demo__MTNotificationQueue__) */