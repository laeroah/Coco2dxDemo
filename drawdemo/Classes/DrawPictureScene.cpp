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
#include "PlayerListLayer.h"

void DrawPictureScene::buttonCallback(CCNode *sender, void *param)
{
    if (param != NULL)
    {
        _layer->resetCurrentPicture();
    }
    this->removeChildByTag(200);
}

void DrawPictureScene::ShowNearPlayers(CCObject *sender)
{
    if (!SessionManager::getSharedInstance()->isInitialized())
    {
        return;
    }
    
    CCSize contentSize = mNearPlayerItems->getContentSize();
    if (mWaitingForJoin)
    {
        if (mPlayersArray->count() <= 0)
        {
            MessageUtil::postMessage("No players found");
            return;
        }
        CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
        CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
        CCLayerColor *layerColor = CCLayerColor::create(ccc4(0,0,0,150));
        this->addChild(layerColor,10,200);
        PlayerListLayer* pl = PlayerListLayer::create("BackGround.png");
        pl->setContentSize(CCSizeMake(250, 250));
        pl->setPosition(ccp(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2));
        pl->setTitle("Nearby Players",16);
        pl->setCallbackFunc(this, callfuncND_selector(DrawPictureScene::buttonCallback));
        pl->addButton("Close.png", "Close.png", "", 245,245, 0);
        pl->setPlayerList(mPlayersArray);
        this->addChild(pl,11);
        mWaitingForJoin = false;
        mNearPlayerItems->setNormalSpriteFrame(CCSpriteFrame::create("Door.png", CCRectMake(0,0,contentSize.width,contentSize.height)));
        mNearPlayerItems->setDisabledSpriteFrame(CCSpriteFrame::create("Door.png", CCRectMake(0,0,contentSize.width,contentSize.height)));
    }
    else
    {
        mWaitingForJoin = true;
        mNearPlayerItems->setNormalSpriteFrame(CCSpriteFrame::create("Players.png", CCRectMake(0,0,contentSize.width,contentSize.height)));
        mNearPlayerItems->setDisabledSpriteFrame(CCSpriteFrame::create("Players.png", CCRectMake(0,0,contentSize.width,contentSize.height)));
        
        _layer->resetCurrentPicture();
        SessionManager::getSharedInstance()->leaveSession();
    }
}

void DrawPictureScene::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}


bool DrawPictureScene::init()
{
	if( CCScene::init() )
	{
		this->_layer = CanvasLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
        
        mPlayersArray = CCArray::create();
        mPlayersArray->retain();
        //create menu
        mNearPlayerItems = CCMenuItemImage::create(
                                                                    "Players.png",
                                                                    "Players.png",
                                                                    this,
                                                                    menu_selector(DrawPictureScene::ShowNearPlayers));
        mNearPlayerItems->retain();
        CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
        CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
        mNearPlayerItems->setPosition(ccp(origin.x + visibleSize.width - mNearPlayerItems->getContentSize().width/2 - 10,
                                          origin.y + visibleSize.height - mNearPlayerItems->getContentSize().height/2 - 10));

        CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                               "CloseNormal.png",
                                               "CloseSelected.png",
                                               this,
                                               menu_selector(DrawPictureScene::menuCloseCallback));

        pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                        origin.y + pCloseItem->getContentSize().height/2));

        // Create a menu with the "close" menu item, it's an auto release object.
        CCMenu* pMenu = CCMenu::create(mNearPlayerItems, pCloseItem, NULL);
        pMenu->setPosition(CCPointZero);
        
        mWaitingForJoin = true;
        // Add the menu to HelloWorld layer as a child layer.
        this->addChild(pMenu, 1);
        
        this->schedule(schedule_selector(DrawPictureScene::checkSessionManager),2,kCCRepeatForever,2);
        this->schedule(schedule_selector(DrawPictureScene::postMessage),0.1,kCCRepeatForever,0);
		
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DrawPictureScene::joinSessionNotification), JOIN_SESSION_NOTIFICATION, NULL);
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DrawPictureScene::reqSyncNotification), REQ_SYNC_NOTIFICATION, NULL);
        
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DrawPictureScene::lostSessionNotification), LOST_SESSION_NOTIFICATION, NULL);
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DrawPictureScene::syncCommandNotification), SYNC_COMMAND_NOTIFICATION, NULL);
        
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DrawPictureScene::playerFoundNotification), PLAYER_FOUND_NOTIFICATION, NULL);
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DrawPictureScene::playerUnFoundNotification), PLAYER_UNFOUND_NOTIFICATION, NULL);

        
        this->schedule(schedule_selector(DrawPictureScene::discoveryPlayers),5,kCCRepeatForever,0);
        
  		return true;
	}
	else
	{
		return false;
	}
}

void DrawPictureScene::discoveryPlayers(float dt)
{
    if (SessionManager::getSharedInstance()->isInitialized())
    {
        SessionManager::getSharedInstance()->FindAdvertisedName();
    }
}

void DrawPictureScene::postMessage(CCObject* obj)
{
    MTNotificationQueue::sharedInstance()->repostNotifications(0.1);
}


void DrawPictureScene::playerFoundNotification(CCObject* obj)
{
    CCObject* item;
    Player *newPlayer = (Player*)obj;
    
    CCARRAY_FOREACH(mPlayersArray,item)
    {
        Player *player = (Player*)item;
        if (player->mplayerWellKnownName.compare(newPlayer->mplayerWellKnownName)== 0) {
            return;
        }
    }
    mPlayersArray->addObject(obj);
}

void DrawPictureScene::playerUnFoundNotification(CCObject* obj)
{
    CCObject* item;
    Player *newPlayer = (Player*)obj;
    CCARRAY_FOREACH(mPlayersArray,item)
    {
        Player *player = (Player*)item;
        if (player->mplayerWellKnownName.compare(newPlayer->mplayerWellKnownName)== 0) {
            mPlayersArray->removeObject(item);
            return;
        }
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

void DrawPictureScene::syncCommandNotification(CCObject* obj)
{
    SyncCommandContent *syncCommandContent = (SyncCommandContent*)obj;
    _layer->setSyncCommandsContent(syncCommandContent->mCommandCount, syncCommandContent->mCommandContent);
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
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, SYNC_COMMAND_NOTIFICATION);
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, PLAYER_FOUND_NOTIFICATION);
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, PLAYER_UNFOUND_NOTIFICATION);
    
    
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
    
    CC_SAFE_RELEASE(mPlayersArray);
}
