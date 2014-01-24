    //
//  CanvasLayer.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#include "CanvasLayer.h"
#include "TimeUtil.h"

bool CanvasLayer::init()
{
	if ( !CCLayerColor::initWithColor( ccc4(255,255,255,255) ) )
	{
        return false;
    }
    //create current Picuture
    mCurrentPicture = PictureManager::getSharedInstance()->createPicture();
    mCurrentPicture->retain();
    this->setTouchEnabled(true);
    mLastSendCommandId = 0;
    mLastSendSyncCommandTime = 0;
    return true;
	
}

void CanvasLayer::onEnter()
{
    CCLayerColor::onEnter();
}

void CanvasLayer::draw()
{
    //CCLayerColor::draw();
    ::ccDrawColor4B(255,0,0,255);
    ::glLineWidth(5);
    
    if (!mCurrentPicture || mCurrentPicture->mCommandList->count() <= 0) {
        return;
    }
    
    int count= mCurrentPicture->mCommandList->count();
    
    for(int i=0;i<count;i++)
    {
        DrawCommand *drawCommand = (DrawCommand *)mCurrentPicture->mCommandList->objectAtIndex(i);
        if (drawCommand->mCommandType != DRAW_LINE) {
            continue;
        }
        
        if(i==0)
            ccDrawPoint(drawCommand->mFromPoint);
        
        ::ccDrawLine(drawCommand->mFromPoint,drawCommand->mToPoint);
    }
}

CanvasLayer::~CanvasLayer()
{
	if (mCurrentPicture) {
        mCurrentPicture->release();
    }
}

void    CanvasLayer::releaseLastPoint()
{
    if (mLastPoint)
    {
        delete mLastPoint;
        mLastPoint = NULL;
    }
}

bool  CanvasLayer::ccTouchBegan(CCTouch  *pTouche, CCEvent *pEvent)
{
    releaseLastPoint();
    mLastPoint = new CCPoint();
    mLastPoint->x=pTouche->getLocation().x ;
    mLastPoint->y=pTouche->getLocation().y;
   
    return true;
}

void CanvasLayer::ccTouchMoved(CCTouch  *pTouche, CCEvent *pEvent)
{
    if (!mCurrentPicture) {
        return;
    }
    
    CCPoint *p = new CCPoint();
    p->x= ceil(pTouche->getLocation().x);
    p->y= ceil(pTouche->getLocation().y);
    
    if (SessionManager::getSharedInstance()->mSessionMode == ServerMode)
    {
        mCurrentPicture->addNewCommand(DRAW_LINE,*mLastPoint,*p);
        if (mHasRecvReqSync) {
            sendSyncCommand(0);
        }
    }
    else
    {
        mCurrentPicture->addTempNewCommand(DRAW_LINE,*mLastPoint,*p);
        sendTempSyncCommand(0);
    }
    releaseLastPoint();
    mLastPoint = p;
}

void CanvasLayer::sendSyncCommand(float dt)
{
    string commandContent;
    int commandsCount;
    bool hasMore;
    int lastSendCommandId;
    
    if(getCurrentPicture()->getSyncCommandsContent(mLastSendCommandId, 40 , lastSendCommandId, commandContent, commandsCount, hasMore))
    {
        unsigned long long curtime = TimeUtil::getCurrentTime();
        
        if (curtime < mLastSendSyncCommandTime + 50) {
            this->unschedule(schedule_selector(CanvasLayer::sendSyncCommand));
            float delayTime = (float)(mLastSendSyncCommandTime + 50 - curtime);
            delayTime /= 1000;
            delayTime = delayTime > 0.05 ? 0.05 : delayTime;
            printf("mLastSendSyncCommandTime %lld %lld delayTime %f\n",mLastSendSyncCommandTime, curtime , delayTime);
            this->schedule(schedule_selector(CanvasLayer::sendSyncCommand), delayTime ,1,0);
            return;
        }
        
        printf("send sync %d %s\n",commandsCount,
               commandContent.c_str()
               );
        mLastSendSyncCommandTime = curtime;
        mLastSendCommandId = lastSendCommandId;
        SessionManager::getSharedInstance()->CallSendSync(commandsCount, commandContent);
        if (hasMore) {
            this->schedule(schedule_selector(CanvasLayer::sendSyncCommand),0.1,1,0);
        }
    }
    else
    {
        printf("no new command found %d\n",mLastSendCommandId
               );

    }
}

void CanvasLayer::sendTempSyncCommand(float dt)
{
    string commandContent;
    int commandsCount;
    bool hasMore;
    if(getCurrentPicture()->getSyncTempCommandsContent(40 , commandContent, commandsCount, hasMore))
    {
        SessionManager::getSharedInstance()->CallSendSync(commandsCount, commandContent);
        if (hasMore) {
            this->schedule(schedule_selector(CanvasLayer::sendTempSyncCommand),0.1,1,0);
        }
    }
}

void CanvasLayer::ccTouchEnded(CCTouch  *pTouche, CCEvent *pEvent)
{
    if (!mLastPoint) {
        return;
    }
    
    CCPoint *p = new CCPoint();
    p->x= ceil(pTouche->getLocation().x);
    p->y= ceil(pTouche->getLocation().y);
    
    if (SessionManager::getSharedInstance()->mSessionMode == ServerMode)
    {
        mCurrentPicture->addNewCommand(DRAW_LINE,*mLastPoint,*p);
        if (mHasRecvReqSync) {
            sendSyncCommand(0);
        }
    }
    else
    {
        mCurrentPicture->addTempNewCommand(DRAW_LINE,*mLastPoint,*p);
        sendTempSyncCommand(0);
    }
    
    releaseLastPoint();
}

void CanvasLayer::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, this->getTouchPriority(), true);
}

void    CanvasLayer::setSyncCommandsContent(int commandCount, std::string &commandContent)
{
    getCurrentPicture()->setSyncCommandsContent(commandCount, commandContent);
    if (SessionManager::getSharedInstance()->mSessionMode == ServerMode)
    {
        if (mHasRecvReqSync) {
            sendSyncCommand(0);
        }
    }
}