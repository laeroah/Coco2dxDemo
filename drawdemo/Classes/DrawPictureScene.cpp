//
//  DrawPictureScene.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-22.
//
//

#include "DrawPictureScene.h"

bool DrawPictureScene::init()
{
	if( CCScene::init() )
	{
		this->_layer = CanvasLayer::create();
		this->_layer->retain();
		this->addChild(_layer);
		
		return true;
	}
	else
	{
		return false;
	}
}

DrawPictureScene::~DrawPictureScene()
{
	if (_layer)
	{
		_layer->release();
		_layer = NULL;
	}
}
