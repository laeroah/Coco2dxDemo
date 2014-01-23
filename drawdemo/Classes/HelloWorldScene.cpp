#include "HelloWorldScene.h"
#include "SessionManager.h"
#include "CursorTextField.h"
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

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
   
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
    CursorTextField *userName = CursorTextField::textFieldWithPlaceHolder("Input your name", "Arial", 34);
    userName->setTag(100);
    userName->setPosition(ccp(visibleSize.width/2,visibleSize.height/2+80));
    userName->setColor(ccc3(255, 255, 255));
    userName->attachWithIME();
    userName->myDelegate = this;
    this->addChild(userName,1);
    
    return true;
}


bool HelloWorld::onTextFieldAttachWithIME(CCTextFieldTTF *pSender)
{
    return false;
}

void HelloWorld::gotoDrawPictureScene()
{
    DrawPictureScene *drawPictureScene = DrawPictureScene::create();
    CCDirector::sharedDirector()->replaceScene(drawPictureScene);
}

void HelloWorld::RunAsServer(CCObject* pSender)
{
    SessionManager::getSharedInstance()->initServerWithDrawerName(mUserName.c_str());
    gotoDrawPictureScene();
}

void HelloWorld::RunAsClient(CCObject* pSender)
{
   SessionManager::getSharedInstance()->initClientWithDrawerName(mUserName.c_str());
   gotoDrawPictureScene();
}

bool HelloWorld::onTextFieldDetachWithIME(CCTextFieldTTF * pSender)
{
    //Initialize session manager
    if (pSender->getString() != NULL && strlen(pSender->cocos2d::CCLabelTTF::getString()) > 0)
    {
        mUserName = pSender->getString();
        
        CCMenuItemFont::setFontName("Helvetica");
        CCMenuItemFont::setFontSize(35);
        CCMenuItemFont *pServerItem = CCMenuItemFont::create(
                                                        "RunAsServer",
                                                        this,
                                                        menu_selector(HelloWorld::RunAsServer) );
        CCMenuItemFont *pClientItem = CCMenuItemFont::create(
                                                         "RunAsClient",
                                                         this,
                                                         menu_selector(HelloWorld::RunAsClient) );
        
        CCMenu* pMenu = CCMenu::create(pServerItem,pClientItem,NULL);
        pMenu->alignItemsHorizontally();
        
        this->addChild(pMenu, 1);
        
        CCNode *pNode = this->getChildByTag(100);
        if (pNode) {
            pNode->removeFromParentAndCleanup(true);
        }

        
        return false;
    }

    return true;
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
