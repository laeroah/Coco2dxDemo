//
//  PictureManager.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#ifndef __drawdemo__PictureManager__
#define __drawdemo__PictureManager__

#include <iostream>
#include "cocos2d.h"
#include "Picture.h"

using namespace cocos2d;

class PictureManager : public CCObject
{
public:
    static PictureManager *getSharedInstance();
    Picture *createPicture();
};
#endif /* defined(__drawdemo__PictureManager__) */
