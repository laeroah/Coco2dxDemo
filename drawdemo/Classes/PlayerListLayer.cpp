//
//  PlayerListLayer.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-25.
//
//

#include "PlayerListLayer.h"
#include "SessionManager.h"
#include "Picture.h"

bool PlayerListLayer::init()
{
    if (!ModelDialog::init())
        return false;
    return true;
}

PlayerListLayer::~PlayerListLayer()
{
    CC_SAFE_RELEASE(mPlayerList);
}

void PlayerListLayer::setPlayerList(CCArray *playerList)
{
    if (mPlayerList != NULL)
    {
        CC_SAFE_RELEASE(mPlayerList);
    }
    else
    {
        mTableview = CCTableView::create(this, CCSizeMake(250, 200));
        mTableview->setTouchPriority(-200);
        mTableview->setDirection(kCCScrollViewDirectionVertical);
        mTableview->setDelegate(this);
        mTableview->setVerticalFillOrder(kCCTableViewFillTopDown);
        mTableview->setPosition(ccp(0,0));
        this->addChild(mTableview,20);
    }
    mPlayerList = CCArray::createWithArray(playerList);
    mPlayerList->retain();
    mTableview->reloadData();
}

PlayerListLayer* PlayerListLayer::create(const char* backgroundImage)
{
    PlayerListLayer *pl = PlayerListLayer::create();
    pl->initWithBackgroundImage(backgroundImage);
    return pl;
}

void PlayerListLayer::tableCellTouched(cocos2d::extension::CCTableView *table, cocos2d::extension::CCTableViewCell *cell)
{
    Player *player = (Player*)mPlayerList->objectAtIndex(cell->getIdx());
    SessionManager::getSharedInstance()->JoinSession(player->mplayerWellKnownName.c_str());
    callCallback(player);
    close();
}

CCSize PlayerListLayer::tableCellSizeForIndex(cocos2d::extension::CCTableView *table, unsigned int idx)
{
    return CCSizeMake(250, 20);
}

unsigned int PlayerListLayer::numberOfCellsInTableView(cocos2d::extension::CCTableView *table)
{
    if (!mPlayerList) {
        return 0;
    }
    
    return mPlayerList->count();
}

CCTableViewCell* PlayerListLayer::tableCellAtIndex(cocos2d::extension::CCTableView *table, unsigned int idx)
{
    CCTableViewCell *cell = table->dequeueCell();
    Player *player = (Player*)mPlayerList->objectAtIndex(idx);
    if (!cell) {
        cell = new CCTableViewCell();
        cell->autorelease();
        CCLabelTTF *label = CCLabelTTF::create(player->mPlayerName.c_str(), "Helvetica-BoldOblique", 15);
        label->setAnchorPoint(CCPointZero);
        label->setTag(123);
        label->setColor(ccc3(231, 202, 232));
        label->cocos2d::CCNode::setPosition(20, 5);
        cell->addChild(label,13);
    }
    else
    {
        CCLabelTTF *label = (CCLabelTTF*)cell->getChildByTag(123);
        label->setString(player->mPlayerName.c_str());
    }
    
    return cell;
}

