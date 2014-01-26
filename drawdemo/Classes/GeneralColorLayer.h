//
//  GeneralColorLayer.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-24.
//
//

#ifndef __drawdemo__GeneralColorLayer__
#define __drawdemo__GeneralColorLayer__

#include "cocos2d.h"
#include "PictureManager.h"
#include "SessionManager.h"
#include "TintMessageLayer.h"

using namespace cocos2d;

class GeneralColorLayer : public cocos2d::CCLayerColor
{
private:
    bool isShowingMessage;
    TintMessageLayer *mMessageLayer;
    
    void finishCallFunc();
public:
    bool initWithColor(const ccColor4B& color);
    ~GeneralColorLayer();
    void addMessage(CCObject* obj);
    void showMessage();
    void showMessageLayer();
};

#endif /* defined(__drawdemo__GeneralColorLayer__) */
