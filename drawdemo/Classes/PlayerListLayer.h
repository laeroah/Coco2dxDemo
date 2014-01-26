//
//  PlayerListLayer.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-25.
//
//

#ifndef __drawdemo__PlayerListLayer__
#define __drawdemo__PlayerListLayer__

#include "cocos2d.h"
#include "ModelDialog.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class PlayerListLayer : public ModelDialog , public CCTableViewDelegate,public CCTableViewDataSource
{
private:
    CCArray *mPlayerList;
    CCTableView *mTableview;
public:
    bool    init();
    ~PlayerListLayer();
    void    setPlayerList(CCArray *playerList);
    virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
    virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
    virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
    virtual unsigned int numberOfCellsInTableView(CCTableView *table);
    
    virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view) {}
    virtual void scrollViewDidZoom(cocos2d::extension::CCScrollView* view) {}
    static PlayerListLayer* create(const char* backgroundImage);
    CREATE_FUNC(PlayerListLayer);
};

#endif /* defined(__drawdemo__PlayerListLayer__) */
