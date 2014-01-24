//
//  Picture.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#include "Picture.h"
#include "SessionManager.h"
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
    mCommandId = 0;
}

Picture::~Picture()
{
    if (mCommandList) {
        mCommandList->release();
    }
    
    if (mTempCommandList) {
        mTempCommandList->release();
    }
}

bool Picture::init()
{
    mCommandList = CCArray::create();
    if (mCommandList) {
        mCommandList->retain();
    }
    
    mTempCommandList = CCArray::create()
    ;
    if (mTempCommandList) {
        mTempCommandList->retain();
    }
    
    return true;
}

void    Picture::addNewCommand(DrawCommandType commandType, CCPoint &fromPoint , CCPoint &toPoint)
{
    DrawCommand *newCommand = DrawCommand::create();
    newCommand->mCommandType = commandType;
    newCommand->mCommandId = ++mCommandId;
    newCommand->mFromPoint = fromPoint;
    newCommand->mToPoint = toPoint;
    mCommandList->addObject(newCommand);
    
    printf("add point %f %f %f %f\n",newCommand->mFromPoint.x,newCommand->mFromPoint.y,
           newCommand->mToPoint.x,newCommand->mToPoint.y
           );
}

void    Picture::addTempNewCommand(DrawCommandType commandType, CCPoint &fromPoint , CCPoint &toPoint)
{
    DrawCommand *newCommand = DrawCommand::create();
    newCommand->mCommandType = commandType;
    newCommand->mCommandId = ++mCommandId;
    newCommand->mFromPoint = fromPoint;
    newCommand->mToPoint = toPoint;
    mTempCommandList->addObject(newCommand);
}

bool  Picture::getSyncTempCommandsContent(int maxCount, string &commandsContent, int &commandsCount, bool &hasMore)
{
    int totalCommandCount = mTempCommandList->count();
    Json::Value rootNode;
    hasMore = false;
    commandsCount = 0;
    CCArray *removeArray = CCArray::create();
    for(int i=0;i<totalCommandCount;i++)
    {
        DrawCommand *drawCommand = (DrawCommand *)mTempCommandList->objectAtIndex(i);
        if (drawCommand->mCommandType != DRAW_LINE) {
            continue;
        }
        
        Json::Value item;
        item["id"] = drawCommand->mCommandId;
        item["fromX"] = drawCommand->mFromPoint.x;
        item["fromY"] = drawCommand->mFromPoint.y;
        item["toX"] = drawCommand->mToPoint.x;
        item["toY"] = drawCommand->mToPoint.y;
        item["type"] = drawCommand->mCommandType;
        rootNode.append(item);
        removeArray->addObject(drawCommand);
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
    mTempCommandList->removeObjectsInArray(removeArray);
    removeArray->release();
    
    return true;
}


void    Picture::clearAllCommands()
{
    mCommandList->removeAllObjects();
}

bool  Picture::getSyncCommandsContent(int lastSendCommandId, int maxCount, int &lastCommandId, string &commandsContent, int &commandsCount, bool &hasMore)
{
    int totalCommandCount = mCommandList->count();
    
    if (totalCommandCount <= 0)
    {
        return false;
    }
    
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
        
        if (SessionManager::getSharedInstance()->mSessionMode == ServerMode)
        {
            CCPoint fromPoint = ccp(tempValue["fromX"].asDouble(), tempValue["fromY"].asDouble());
            CCPoint toPoint = ccp(tempValue["toX"].asDouble(), tempValue["toY"].asDouble());
            addNewCommand((DrawCommandType)tempValue["type"].asInt(), fromPoint, toPoint);
        }
        else
        {
            DrawCommand *drawCommand = DrawCommand::create();
            drawCommand->mCommandId = tempValue["id"].asInt();
            drawCommand->mFromPoint.x = tempValue["fromX"].asDouble();
            drawCommand->mFromPoint.y = tempValue["fromY"].asDouble();
            drawCommand->mToPoint.x = tempValue["toX"].asDouble();
            drawCommand->mToPoint.y = tempValue["toY"].asDouble()
        ;
            drawCommand->mCommandType = (DrawCommandType)tempValue["type"].asInt();
            mCommandList->addObject(drawCommand);
        }
    }
    
    return true;
}