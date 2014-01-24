//
//  CanvasLayer.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#ifndef __drawdemo__CanvasLayer__
#define __drawdemo__CanvasLayer__

#include "cocos2d.h"
#include "PictureManager.h"
#include "SessionManager.h"

using namespace cocos2d;

class CanvasLayer : public cocos2d::CCLayerColor
{
private:
    Picture *mCurrentPicture;
    CCPoint *mLastPoint;
    int     mLastSendCommandId;
    bool    mHasRecvReqSync;
    void    releaseLastPoint();
    unsigned long long mLastSendSyncCommandTime;
public:
    ~CanvasLayer();
    virtual bool init();
    
    virtual void onEnter();
    virtual void draw();
    
    virtual bool ccTouchBegan(CCTouch  *pTouches, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch  *pTouches, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch  *pTouches, CCEvent *pEvent);
    virtual void registerWithTouchDispatcher();
    
    void    setHasRecvReqSync(bool hasRecvReqSync){mHasRecvReqSync = hasRecvReqSync;};
    Picture *getCurrentPicture(){ return mCurrentPicture;}
    void    setLastSendCommandId(int commandId){mLastSendCommandId = commandId;};
    void    sendSyncCommand(float dt);
    void    sendTempSyncCommand(float dt);
    void    setSyncCommandsContent(int commandCount, std::string &commandContent);

    CREATE_FUNC(CanvasLayer);
};

#endif /* defined(__drawdemo__CanvasLayer__) */
