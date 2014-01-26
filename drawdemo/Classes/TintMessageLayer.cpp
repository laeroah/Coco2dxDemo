//
//  TintMessageLayer.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-24.
//
//

#include "TintMessageLayer.h"

TintMessageLayer::~TintMessageLayer()
{
    if(mLabel)
    {
        mLabel->release()
        ;
    }
    
}

bool TintMessageLayer::init()
{
	if ( !CCLayerColor::initWithColor( ccc4(0,0,0,0) ) )
	{
        return false;
    }
    
    isShowingMessage = false;
    mLabel = CCLabelTTF::create("abc", "Arial", 10);
    mLabel->retain();
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    // position the label on the center of the screen
    mLabel->setPosition(ccp(0 + visibleSize.width/2,
                            origin.y +  mLabel->getContentSize().height / 2));
    mLabel->setHorizontalAlignment(kCCTextAlignmentLeft);
    addChild(mLabel);
    
    return true;
}


void TintMessageLayer::setMessage(const char *message)
{
    mLabel->setString(message);
}

