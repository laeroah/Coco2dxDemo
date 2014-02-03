//
//  SendQueue.h
//  drawdemo
//
//  Created by Hao Wang on 14-2-3.
//
//

#ifndef __drawdemo__SendQueue__
#define __drawdemo__SendQueue__

#include <iostream>
#include <pthread.h>
#include <vector>

using namespace std;

class SendQueue
{
public:
    typedef struct {
        int mCommandCount;
        string mCommandString;
    } NotificationArgs;
    vector<NotificationArgs> m_oVecCommands;
    static SendQueue *getSharedInstance();
    void SendCommand( const int commandCount , string &commandString);
    void actSendCommand();
    void Reset();
};

#endif /* defined(__drawdemo__SendQueue__) */
