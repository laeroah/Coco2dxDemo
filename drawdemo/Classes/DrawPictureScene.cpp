//
//  DrawPictureScene.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#include "DrawPictureScene.h"
#include "SessionManager.h"

bool DrawPictureScene::init()
{
	if( CCScene::init() )
	{
		this->_layer = CanvasLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
        
        this->schedule(schedule_selector(DrawPictureScene::checkSessionManager),2,kCCRepeatForever,2);
		
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DrawPictureScene::joinSessionNotification), JOIN_SESSION_NOTIFICATION, NULL);
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DrawPictureScene::reqSyncNotification), REQ_SYNC_NOTIFICATION, NULL);
        
		return true;
	}
	else
	{
		return false;
	}
}


void DrawPictureScene::joinSessionNotification(CCObject* obj)
{
    if (SessionManager::getSharedInstance()->mSessionMode == ServerMode) {
        return;
    }
    
    int commandId = _layer->getCurrentPicture()->getLastCommandId();
    //send req sync to server
    SessionManager::getSharedInstance()->CallReqSync(commandId);
    
}

void DrawPictureScene::reqSyncNotification(CCObject* obj)
{
    if (SessionManager::getSharedInstance()->mSessionMode == ClientMode) {
        return;
    }
    
    //send sync command
}

void DrawPictureScene::checkSessionManager(float dt)  
{
    SessionManager::getSharedInstance()->runProcess();
}

DrawPictureScene::~DrawPictureScene()
{
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,JOIN_SESSION_NOTIFICATION);
     CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,REQ_SYNC_NOTIFICATION);
    
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}
