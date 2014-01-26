//
//  DrawPictureScene.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#ifndef __drawdemo__DrawPictureScene__
#define __drawdemo__DrawPictureScene__

#include "cocos2d.h"
#include "CanvasLayer.h"
using namespace cocos2d;

class DrawPictureScene : public cocos2d::CCScene
{
private:
    int mLastSendCommandId;
    CCArray *mPlayersArray;
public:
    DrawPictureScene():_layer(NULL) {};
    ~DrawPictureScene();
    void checkSessionManager(float dt);
    bool init();
    void joinSessionNotification(CCObject* obj);
    void reqSyncNotification(CCObject* obj);
    void lostSessionNotification(CCObject* obj);
    void syncCommandNotification(CCObject* obj);
    void postMessage(CCObject* obj);
    void ShowNearPlayers(CCObject *sender);
    void buttonCallback(CCNode *sender, void *param);
    void playerFoundNotification(CCObject* obj);
    void playerUnFoundNotification(CCObject* obj);
    CREATE_FUNC(DrawPictureScene);
    CC_SYNTHESIZE_READONLY(CanvasLayer*, _layer, Layer);
};

#endif /* defined(__drawdemo__DrawPictureScene__) */
