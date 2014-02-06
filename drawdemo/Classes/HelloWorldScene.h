#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

using namespace cocos2d;

class HelloWorld : public cocos2d::CCLayer, cocos2d::CCTextFieldDelegate
{
public:
    std::string mUserName;
    CCTextFieldTTF *mUserNameTextField;
    CCPoint  mBeginPos;
    int  mCharLimit;
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    ~HelloWorld();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    //delegate
    virtual bool onTextFieldAttachWithIME(CCTextFieldTTF *pSender);
    virtual bool onTextFieldDetachWithIME(CCTextFieldTTF * pSender);
    virtual bool onTextFieldInsertText(CCTextFieldTTF * pSender, const char * text, int nLen);
    virtual bool onTextFieldDeleteBackward(CCTextFieldTTF * pSender, const char * delText, int nLen);
    void 	registerWithTouchDispatcher();
    void gotoDrawPictureScene();
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    void RunAsServer(CCObject* pSender);
    void onClickTrackNode(bool bClicked);
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);
    
    
};

#endif // __HELLOWORLD_SCENE_H__
