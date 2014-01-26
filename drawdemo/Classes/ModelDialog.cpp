//
//  ModelDialog.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-25.
//
//

#include "ModelDialog.h"

ModelDialog::ModelDialog():
m_contentPadding(0)
, m_contentPaddingTop(0)
, m_callbackListener(NULL)
, m_callback(NULL)
, m__sfBackGround(NULL)
, m__s9BackGround(NULL)
, m__ltContentText(NULL)
, m__ltTitle(NULL)
{
    
}

ModelDialog::~ModelDialog(){
    CC_SAFE_RELEASE(mButtonArray);
    CC_SAFE_RELEASE(m__sfBackGround);
    CC_SAFE_RELEASE(m__ltContentText);
    CC_SAFE_RELEASE(m__ltTitle);
    CC_SAFE_RELEASE(m__s9BackGround);
}

bool ModelDialog::init(){
    bool bRef = false;
    do{
        CC_BREAK_IF(!CCLayer::init());
        this->setContentSize(CCSizeZero);
        ignoreAnchorPointForPosition(false);
        setAnchorPoint(ccp(0.5,0.5));
       
        mButtonArray = CCArray::create();
        mButtonArray->retain();
        setTouchEnabled(true);
        
        bRef = true;
    }while(0);
    return bRef;
}

void ModelDialog::registerWithTouchDispatcher(){
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -128, true);
    
}

bool ModelDialog::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
    return true;
}

void ModelDialog::initWithBackgroundImage(const char *backgroundImage){
    setSpriteBackGround(CCSprite::create(backgroundImage));
    setSprite9BackGround(CCScale9Sprite::create(backgroundImage));
    return;
}

void ModelDialog::setTitle(const char *title, int fontsize){
    CCLabelTTF* ltfTitle = CCLabelTTF::create(title, "", fontsize);
    setLabelTitle(ltfTitle);
}

void ModelDialog::setContentText(const char *text, int fontsize, int padding, int paddingTop){
    CCLabelTTF* ltf = CCLabelTTF::create(text, "Thonburi-Bold", fontsize);
    setLabelContentText(ltf);
    m_contentPadding = padding;
    m_contentPaddingTop = paddingTop;
}

void ModelDialog::setCallbackFunc(cocos2d::CCObject *target, SEL_CallFuncND callfun){
    m_callbackListener = target;
    m_callback = callfun;
}


bool ModelDialog::addButton(const char *normalImage, const char *selectedImage, const char *title, const float xPosition , const float yPosition, int tag ){
    
    CCPoint position = ccp(xPosition, yPosition);
    CCMenuItemImage* menuImage = CCMenuItemImage::create(normalImage, selectedImage, this, menu_selector(ModelDialog::buttonCallback));
    menuImage->setTag(tag);
    menuImage->setPosition(position);
    
    CCSize imenu = menuImage->getContentSize();
    CCLabelTTF* ttf = CCLabelTTF::create(title, "", 20);
    ttf->setColor(ccc3(0, 0, 0));
    ttf->setPosition(ccp(imenu.width / 2, imenu.height / 2));
    menuImage->addChild(ttf);
    
    mButtonArray->addObject(menuImage);
    
    return true;
}

void ModelDialog::buttonCallback(cocos2d::CCObject *pSender){
    CCNode* node = dynamic_cast<CCNode*>(pSender);
    CCLog("touch tag: %d", node->getTag());
    if (m_callback && m_callbackListener){
        (m_callbackListener->*m_callback)(node,NULL);
    }
    close();
}

void    ModelDialog::callCallback(void *param)
{
    if (m_callback && m_callbackListener){
        (m_callbackListener->*m_callback)(NULL,param);
    }
}

void    ModelDialog::close()
{
    this->removeFromParent();
}

void ModelDialog::onEnter(){
    CCLayer::onEnter();
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    
    CCSize contentSize;
    if (getContentSize().equals(CCSizeZero)) {
        getSpriteBackGround()->setPosition(ccp(origin.x + visibleSize.width / 2 , origin.y + visibleSize.height / 2));
        this->addChild(getSpriteBackGround(), 0, 0);
        contentSize = getSpriteBackGround()->getTexture()->getContentSize();
    } else {
        contentSize = getContentSize();
        CCScale9Sprite *background = getSprite9BackGround();
        background->setContentSize(getContentSize());
        background->setPosition(ccp(contentSize.width / 2, contentSize.height / 2));
        this->addChild(background, 0, 0);
        
    }
    
    CCMenu* menu = CCMenu::createWithArray(mButtonArray);
    menu->setPosition(ccp(0,0));
    addChild(menu);
    
    if (getLabelTitle()){
        CCSize titleSize = getLabelTitle()->getContentSize();
        getLabelTitle()->setPosition(ccp(contentSize.width/2, contentSize.height - titleSize.height/2));
        this->addChild(getLabelTitle());
    }
    
    CCAction* popupLayerAction = CCSequence::create(CCScaleTo::create(0.0, 0.0),
                                              CCScaleTo::create(0.06, 1.05),
                                              CCScaleTo::create(0.08, 0.95),
                                              CCScaleTo::create(0.08, 1.0), NULL);
    this->runAction(popupLayerAction);
    
}

void ModelDialog::onExit(){
    CCLayer::onExit();
}