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

class Picture : public CCObject
{
private:
    int mCommandId;
public:
    Picture();
    virtual ~Picture();
    bool init();
    
    CCArray *mCommandList;
    void    addNewCommand(DrawCommandType commandType, CCPoint *fromPoint , CCPoint *toPoint);
    void    clearAllCommands();
    int     getLastCommandId(){ return mCommandId;};
    CREATE_FUNC(Picture);
};

#endif /* defined(__drawdemo__Picture__) */
