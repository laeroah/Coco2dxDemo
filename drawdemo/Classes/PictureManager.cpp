//
//  PictureManager.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#include "PictureManager.h"

static PictureManager *sPictureManager = NULL;

PictureManager *PictureManager::getSharedInstance()
{
    if (!sPictureManager) {
        sPictureManager = new PictureManager();
    }
    return sPictureManager;
}

Picture *PictureManager::createPicture()
{
    return Picture::create();
}