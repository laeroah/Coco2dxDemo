//
//  MessageUtil.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-25.
//
//

#include "MessageUtil.h"
#include "MTNotificationQueue.h"


void MessageUtil::postMessage(const char *format , ...)
{
    char msgBuffer[1024] = {0};
    va_list ap;
    va_start(ap, format);
    vsprintf(msgBuffer, format, ap);
    va_end(ap);
    
    SystemMessage *messageObject = new SystemMessage;
    messageObject->mMessage = msgBuffer;
    MTNotificationQueue::sharedInstance()->postNotification(SYSTEM_MESSAGE,messageObject);
                                                            
}