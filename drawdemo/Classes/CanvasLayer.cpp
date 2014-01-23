//
//  CanvasLayer.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#include "CanvasLayer.h"

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
    mCommandId = 0;
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
    p->x=pTouche->getLocation().x;
    p->y=pTouche->getLocation().y;
    
    if (SessionManager::getSharedInstance()->mSessionMode == ServerMode)
    {
        mCurrentPicture->addNewCommand(DRAW_LINE,mLastPoint,p);
    }
    else
    {
    }
}

void CanvasLayer::ccTouchEnded(CCTouch  *pTouche, CCEvent *pEvent)
{
    if (!mLastPoint) {
        return;
    }
    
    CCPoint *p = new CCPoint();
    p->x=pTouche->getLocation().x;
    p->y=pTouche->getLocation().y;
    
    if (SessionManager::getSharedInstance()->mSessionMode == ServerMode)
    {
        DrawCommand *command = new DrawCommand();
        command->mCommandType = DRAW_LINE;
        command->mFromPoint = *mLastPoint;
        command->mToPoint = *p;
        mCurrentPicture->addNewCommand(command);
    }
    else
    {
        
    }
    
    releaseLastPoint();
}

void CanvasLayer::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, this->getTouchPriority(), true);
}