//
//  TintMessageLayer.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-24.
//
//

#ifndef __drawdemo__TintMessageLayer__
#define __drawdemo__TintMessageLayer__

#include "cocos2d.h"
#include "PictureManager.h"
#include "SessionManager.h"
#include "MessageUtil.h"

using namespace cocos2d;

class TintMessageLayer : public cocos2d::CCLayerColor
{
private:
    
    CCLabelTTF* mLabel;
    bool isShowingMessage;
public:
    ~TintMessageLayer();
    virtual bool init();
    void setMessage(const char *message);
    CREATE_FUNC(TintMessageLayer);
};

#endif /* defined(__drawdemo__TintMessageLayer__) */
