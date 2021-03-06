//
//  Picture.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#ifndef __drawdemo__Picture__
#define __drawdemo__Picture__

#include <iostream>
#include "cocos2d.h"

using namespace cocos2d;
using namespace std;

typedef enum
{
    DRAW_LINE,
    CHANGE_COLOR,
    CHANGE_PEN_WIDTH,
    CHANGE_PEN_TYPE
}DrawCommandType;

class DrawCommand : public CCObject
{
public:
    int   mCommandId;
    DrawCommandType mCommandType;
    CCPoint mFromPoint;
    CCPoint mToPoint;
    
    bool init();
    
    CREATE_FUNC(DrawCommand);
};

class SyncCommandContent : public CCObject
{
public:
    int   mCommandCount;
    std::string mCommandContent;
    bool init();
    
    CREATE_FUNC(SyncCommandContent);
};


class Player : public cocos2d::CCObject
{
public:
    std::string mPlayerName;
    std::string mplayerWellKnownName;
    CCObject* copyWithZone(CCZone *pZone)
    {
        CCZone *pNewZone = NULL;
        Player *pRet = NULL;
        if (pZone && pZone->m_pCopyObject)
        {
            pRet = (Player*)(pZone->m_pCopyObject);
        }
        else
        {
            pRet = new Player();
            pNewZone = new CCZone(pRet);
        }
        //copy member data
        pRet->mPlayerName = mPlayerName;
        pRet->mplayerWellKnownName = mplayerWellKnownName;
        CC_SAFE_DELETE(pNewZone);
        return (CCObject*)pRet;
    }
};


class Picture : public CCObject
{
private:
    int mCommandId;
public:
    Picture();
    virtual ~Picture();
    bool init();
    
    CCArray *mCommandList;
    CCArray *mTempCommandList;
    void    addNewCommand(DrawCommandType commandType, CCPoint &fromPoint , CCPoint &toPoint);
    void    addTempNewCommand(DrawCommandType commandType, CCPoint &fromPoint , CCPoint &toPoint);
    void    clearAllCommands();
    int     getLastCommandId(){ return mCommandId;};
    bool    getSyncCommandsContent(int lastSendCommandId, int maxCount, int &lastCommandId, string &commandsContent, int &commandsCount, bool &hasMore);
    bool    getSyncTempCommandsContent(int maxCount, string &commandsContent, int &commandsCount, bool &hasMore);
    bool    setSyncCommandsContent(int commandsCount , string &commandsContent);
    CREATE_FUNC(Picture);
};

#endif /* defined(__drawdemo__Picture__) */
