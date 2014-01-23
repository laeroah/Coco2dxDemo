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
    
    void    releaseLastPoint();
public:
    ~CanvasLayer();
    virtual bool init();
    
    virtual void onEnter();
    virtual void draw();
    
    virtual bool ccTouchBegan(CCTouch  *pTouches, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch  *pTouches, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch  *pTouches, CCEvent *pEvent);
    virtual void registerWithTouchDispatcher();
    
    Picture *getCurrentPicture(){ return mCurrentPicture;}
    CREATE_FUNC(CanvasLayer);
};

#endif /* defined(__drawdemo__CanvasLayer__) */
