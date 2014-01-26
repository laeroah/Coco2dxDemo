//
//  MessageUtil.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-25.
//
//

#ifndef __drawdemo__MessageUtil__
#define __drawdemo__MessageUtil__

#include "cocos2d.h"
#include "PictureManager.h"
#include "SessionManager.h"
#include "TintMessageLayer.h"

using namespace cocos2d;

class SystemMessage : public CCObject
{
public:
    std::string mMessage;
};

class MessageUtil
{
public:
    static void postMessage(const char *format , ...);
    
};
#endif /* defined(__drawdemo__MessageUtil__) */
