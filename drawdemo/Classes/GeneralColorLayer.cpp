//
//  GeneralColorLayer.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-24.
//
//

#include "GeneralColorLayer.h"

static CCArray *messageArray;

GeneralColorLayer::~GeneralColorLayer()
{
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,  SYSTEM_MESSAGE);

}

bool GeneralColorLayer::initWithColor(const ccColor4B& color)
{
	if ( !CCLayerColor::initWithColor( color) )
	{
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    
    mMessageLayer = TintMessageLayer::create();
    mMessageLayer->setContentSize(CCSizeMake(visibleSize.width, 10));
    mMessageLayer->retain();
    CCSize contentSize = mMessageLayer->getContentSize();
    mMessageLayer->setPosition(ccp(origin.x + (visibleSize.width - mMessageLayer->getContentSize().width)/2,
                                -origin.y - mMessageLayer->getContentSize().height/2));
    addChild(mMessageLayer);
    
    isShowingMessage = false;
    
    if (!::messageArray) {
        ::messageArray = CCArray::create();
        ::messageArray->retain();
    }
    
     CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(GeneralColorLayer::addMessage), SYSTEM_MESSAGE, NULL);
    
    return true;
}


void GeneralColorLayer::addMessage(CCObject* obj)
{
    ::messageArray->addObject(obj);
    
    if (!isShowingMessage) {
        showMessage();
    }
}

void GeneralColorLayer::showMessage()
{
    if (::messageArray->count() <= 0)
        return;
    
    SystemMessage *message =  (SystemMessage *)messageArray->objectAtIndex(0);
    messageArray->removeObjectAtIndex(0);
    mMessageLayer->setMessage(message->mMessage.c_str());
    showMessageLayer();
}

void GeneralColorLayer::showMessageLayer()
{
    isShowingMessage = true;
    CCSize contentSize = mMessageLayer->getContentSize();
    CCMoveTo* moveup = CCMoveTo::create(0.5f,ccp(0,contentSize.height));
    CCMoveTo* movedown = CCMoveTo::create(0.5f,ccp(0,-contentSize.height - 4));
    
    CCActionInterval* interval= CCActionInterval::create(2);
    CCCallFunc* finishCallFunc=CCCallFunc::create(this,callfunc_selector(GeneralColorLayer::finishCallFunc));
    CCSequence* sequence=CCSequence::create(moveup,interval,movedown,finishCallFunc,NULL);
    mMessageLayer->runAction(sequence);
    
}

void GeneralColorLayer::finishCallFunc()
{
    isShowingMessage = false;
    showMessage();
}
