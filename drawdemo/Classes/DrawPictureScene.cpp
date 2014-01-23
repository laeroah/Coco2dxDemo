//
//  DrawPictureScene.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#include "DrawPictureScene.h"
#include "SessionManager.h"
#include "MTNotificationQueue.h"

bool DrawPictureScene::init()
{
	if( CCScene::init() )
	{
		this->_layer = CanvasLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
        
        this->schedule(schedule_selector(DrawPictureScene::checkSessionManager),2,kCCRepeatForever,2);
        this->schedule(schedule_selector(DrawPictureScene::postMessage),0.1,kCCRepeatForever,0);
		
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DrawPictureScene::joinSessionNotification), JOIN_SESSION_NOTIFICATION, NULL);
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DrawPictureScene::reqSyncNotification), REQ_SYNC_NOTIFICATION, NULL);
        
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DrawPictureScene::lostSessionNotification), LOST_SESSION_NOTIFICATION, NULL);
        if (SessionManager::getSharedInstance()->mSessionMode == ClientMode)
        {
            CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DrawPictureScene::syncCommandNotification), SYNC_COMMAND_NOTIFICATION, NULL);
            
        }
		return true;
	}
	else
	{
		return false;
	}
}

void DrawPictureScene::postMessage(CCObject* obj)
{
    MTNotificationQueue::sharedInstance()->postNotifications(0.1);
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

void DrawPictureScene::syncCommandNotification(CCObject* obj)
{
    if (SessionManager::getSharedInstance()->mSessionMode == ServerMode) {
        return;
    }
    
    SyncCommandContent *syncCommandContent = (SyncCommandContent*)obj;
    _layer->getCurrentPicture()->setSyncCommandsContent(syncCommandContent->mCommandCount, syncCommandContent->mCommandContent);
    delete syncCommandContent;
}


void DrawPictureScene::lostSessionNotification(CCObject* obj)
{
    //show dialog and quit to helloworld scene
     _layer->setHasRecvReqSync(false);
     CCDirector::sharedDirector()->popScene();
}


void DrawPictureScene::reqSyncNotification(CCObject* obj)
{
    if (SessionManager::getSharedInstance()->mSessionMode == ClientMode) {
        return;
    }
    
    DrawCommand *drawCommand = (DrawCommand*)obj;
    //send sync command
    _layer->setLastSendCommandId(drawCommand->mCommandId);
    _layer->sendSyncCommand(0);
    delete drawCommand;
    _layer->setHasRecvReqSync(true);
}

void DrawPictureScene::checkSessionManager(float dt)  
{
    SessionManager::getSharedInstance()->runProcess();
}

DrawPictureScene::~DrawPictureScene()
{
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,JOIN_SESSION_NOTIFICATION);
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,REQ_SYNC_NOTIFICATION);
     CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, LOST_SESSION_NOTIFICATION);
    
    if (SessionManager::getSharedInstance()->mSessionMode == ClientMode)
    {
        CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, SYNC_COMMAND_NOTIFICATION);
    }

	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}
