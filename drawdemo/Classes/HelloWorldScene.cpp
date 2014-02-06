#include "HelloWorldScene.h"
#include "SessionManager.h"

#include "DrawPictureScene.h"

USING_NS_CC;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

void HelloWorld::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    mCharLimit = 255;
    
    setTouchEnabled(true);
   
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...
    mUserNameTextField = CCTextFieldTTF::textFieldWithPlaceHolder("Please input your name", "Arial", 20);
    mUserNameTextField->retain();
    mUserNameTextField->setTag(100);
    mUserNameTextField->setPosition(ccp(visibleSize.width/2,visibleSize.height/2+80));
    mUserNameTextField->setColor(ccc3(255, 255, 255));
    mUserNameTextField->setDelegate(this);
    this->addChild(mUserNameTextField,3);
    
    return true;
}

HelloWorld::~HelloWorld()
{
    CC_SAFE_RELEASE(mUserNameTextField);
}

bool HelloWorld::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    CCLOG("ccTouchBegan");
    mBeginPos = pTouch->getLocation();
    return true;
}

static CCRect getRect(CCNode * pNode)
{
    CCRect rc;
    rc.origin = pNode->getPosition();
    rc.size = pNode->getContentSize();
    rc.origin.x -= rc.size.width / 2;
    rc.origin.y -= rc.size.height / 2;
    return rc;
}

void HelloWorld::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    do
    {
        if (mUserNameTextField) {
            CCPoint endPos = pTouch->getLocation();
            
            float delta = 5.f;
            if (::abs(mBeginPos.x - endPos.x) > delta
                || ::abs(mBeginPos.y - endPos.y) > delta) {
                break;
            }
            
            CCRect rect;
            rect = getRect(mUserNameTextField);
            this->onClickTrackNode(rect.containsPoint(endPos));
            
        }
    } while (0);
    
}


void HelloWorld::onClickTrackNode(bool bClicked)
{
    if (bClicked)
    {
        mUserNameTextField->attachWithIME();
        CCLOG("attach IME");
    }
    else
    {
        mUserNameTextField->detachWithIME();
        CCLOG("detach IME");
    }
}

bool HelloWorld::onTextFieldAttachWithIME(CCTextFieldTTF *pSender)
{
	CCLOG ("onTextFieldAttachWithIME \n");
    return false;
}

void HelloWorld::gotoDrawPictureScene()
{
    DrawPictureScene *drawPictureScene = DrawPictureScene::create();
    CCDirector::sharedDirector()->pushScene(drawPictureScene);
}

void HelloWorld::RunAsServer(CCObject* pSender)
{
    SessionManager::reset();
    SessionManager::getSharedInstance()->initServerWithDrawerName(mUserName.c_str());
    gotoDrawPictureScene();
}

bool HelloWorld::onTextFieldDetachWithIME(CCTextFieldTTF * pSender)
{
    
    CCTextFieldTTF *userName = (CCTextFieldTTF *)pSender;
    //Initialize session manager
    if (userName != NULL && userName->getString() != NULL && strlen(userName->getString()) > 0)
    {
        mUserName = userName->getString();
        RunAsServer(NULL);
        CCLOG ("onTextFieldDetachWithIME %s \n",mUserName.c_str());
        return false;
    }

    return false;
}

bool HelloWorld::onTextFieldInsertText(CCTextFieldTTF * pSender, const char * text, int nLen)
{
    return false;
}

bool HelloWorld::onTextFieldDeleteBackward(CCTextFieldTTF * pSender, const char * delText, int nLen)
{
    
    return false;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
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
