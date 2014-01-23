//
//  Picture.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#include "Picture.h"
#include "thirdparty/jsoncpp/include/json/config.h"
#include "thirdparty/jsoncpp/include/json/json.h"

bool SyncCommandContent::init()
{
    return true;
}

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

bool  Picture::getSyncCommandsContent(int lastSendCommandId, int maxCount, int &lastCommandId, string &commandsContent, int &commandsCount, bool &hasMore)
{
    int totalCommandCount = mCommandList->count();
    Json::Value rootNode;
    hasMore = false;
    commandsCount = 0;
    for(int i=0;i<totalCommandCount;i++)
    {
        DrawCommand *drawCommand = (DrawCommand *)mCommandList->objectAtIndex(i);
        if (drawCommand->mCommandType != DRAW_LINE || drawCommand->mCommandId <= lastSendCommandId) {
            continue;
        }
        
        Json::Value item;
        item["id"] = drawCommand->mCommandId;
        item["fromX"] = drawCommand->mFromPoint.x;
        item["fromY"] = drawCommand->mFromPoint.y;
        item["toX"] = drawCommand->mToPoint.x;
        item["toY"] = drawCommand->mToPoint.y;
        item["type"] = drawCommand->mCommandType;
        lastCommandId = drawCommand->mCommandId;
        rootNode.append(item);
        if (++commandsCount >= maxCount) {
            if (i <= totalCommandCount - 1) {
                hasMore = true;
            }
            break;
        }
    }
    
    if (commandsCount <= 0) {
        return false;
    }
    
    commandsContent = rootNode.toStyledString();
    
    return true;
}

bool    Picture::setSyncCommandsContent(int commandsCount , string &commandsContent)
{
    Json::Reader reader;
    Json::Value rootNode;
    if (!reader.parse(commandsContent, rootNode, false))
    {
        return false;
    }
   
    for (int i = 0; i < rootNode.size(); ++i)
    {
        Json::Value tempValue = rootNode[i];
        DrawCommand *drawCommand = DrawCommand::create();
        drawCommand->mCommandId = tempValue["id"].asInt();
        drawCommand->mFromPoint.x = tempValue["fromX"].asDouble();
        drawCommand->mFromPoint.y = tempValue["fromY"].asDouble();
        drawCommand->mToPoint.x = tempValue["toX"].asDouble();
        drawCommand->mToPoint.y = tempValue["toY"].asDouble()
        ;
        printf("from %f %f to %f %f\n",drawCommand->mFromPoint.x
               ,drawCommand->mFromPoint.y,drawCommand->mToPoint.x,drawCommand->mToPoint.y);
        drawCommand->mCommandType = (DrawCommandType)tempValue["type"].asInt();
        mCommandList->addObject(drawCommand);
    }
    
    return true;
}