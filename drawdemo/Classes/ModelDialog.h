//
//  ModelDialog.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-25.
//
//

#ifndef __drawdemo__ModelDialog__
#define __drawdemo__ModelDialog__

#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class ModelDialog: public cocos2d::CCLayer
{
private:
    
    void buttonCallback(CCObject* pSender);
    
    int m_contentPadding;
    int m_contentPaddingTop;
    
    CCObject* m_callbackListener;
    SEL_CallFuncND m_callback;
    
    CCArray *mButtonArray;
    
    CC_SYNTHESIZE_RETAIN(CCSprite*, m__sfBackGround, SpriteBackGround);
    CC_SYNTHESIZE_RETAIN(CCScale9Sprite*, m__s9BackGround, Sprite9BackGround);
    CC_SYNTHESIZE_RETAIN(CCLabelTTF*, m__ltTitle, LabelTitle);
    CC_SYNTHESIZE_RETAIN(CCLabelTTF*, m__ltContentText, LabelContentText);
    public:
    ModelDialog();
    ~ModelDialog();
    
    virtual bool init();
    
    virtual void registerWithTouchDispatcher(void);
    bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    
    void initWithBackgroundImage(const char* backgroundImage);
    
    void setTitle(const char* title, int fontsize = 20);
    void setContentText(const char* text, int fontsize = 20, int padding = 50, int paddintTop = 100);
    
    void setCallbackFunc(CCObject* target, SEL_CallFuncND callfun);
    
    bool addButton(const char* normalImage, const char* selectedImage, const char* title, const float xPosition, const float yPosition , int tag = 0 );
    virtual void onEnter();
    virtual void onExit();
    void    close();
    void    callCallback(void *param);
    CREATE_FUNC(ModelDialog);
};

#endif /* defined(__drawdemo__ModelDialog__) */
