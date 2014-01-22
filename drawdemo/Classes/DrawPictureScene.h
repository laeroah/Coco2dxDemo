//
//  DrawPictureScene.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#ifndef __drawdemo__DrawPictureScene__
#define __drawdemo__DrawPictureScene__

#include "cocos2d.h"
#include "CanvasLayer.h"
using namespace cocos2d;

class DrawPictureScene : public cocos2d::CCScene
{
public:
    DrawPictureScene():_layer(NULL) {};
    ~DrawPictureScene();
    bool init();
    CREATE_FUNC(DrawPictureScene);
    
    CC_SYNTHESIZE_READONLY(CanvasLayer*, _layer, Layer);
};

#endif /* defined(__drawdemo__DrawPictureScene__) */
