//
//  Picture.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#include "Picture.h"

bool DrawCommand::init()
{
    return true;
}

Picture::Picture()
{
}

Picture::~Picture()
{
    if (mCommandList) {
        mCommandList->release();
    }
}

bool Picture::init()
{
    mCommandList = CCArray::create();
    if (mCommandList) {
        mCommandList->retain();
    }

    return true;
}

void    Picture::addNewCommand(DrawCommandType commandType, CCPoint *fromPoint , CCPoint *toPoint)
{
    DrawCommand *newCommand = DrawCommand::create();
    newCommand->mCommandType = commandType;
    newCommand->mCommandId = ++mCommandId;
    newCommand->mFromPoint = *fromPoint;
    newCommand->mToPoint = *toPoint;
    mCommandList->addObject(newCommand);
}

void    Picture::clearAllCommands()
{
    mCommandList->removeAllObjects();
}