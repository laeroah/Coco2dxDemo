//
//  CursorTextField.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-21.
//
//

#ifndef __drawdemo__CursorTextField__
#define __drawdemo__CursorTextField__

#include <iostream>
#include "cocos2d.h"

using namespace cocos2d;

class CursorTextField: public CCTextFieldTTF, public CCTextFieldDelegate, public CCTouchDelegate
{
private:
    // begin position
    CCPoint m_beginPos;
    
    CCSprite *m_pCursorSprite;
    
    CCAction *m_pCursorAction;
    
    CCPoint m_cursorPos;
    
    std::string *m_pInputText;
public:
    CursorTextField();
    ~CursorTextField();
    
    // static
    static CursorTextField* textFieldWithPlaceHolder(const char *placeholder, const char *fontName, float fontSize);
    
    // CCLayer
    void onEnter();
    void onExit();
    
    void initCursorSprite(int nHeight);
    
    // CCTextFieldDelegate
    virtual bool onTextFieldAttachWithIME(CCTextFieldTTF *pSender);
    virtual bool onTextFieldDetachWithIME(CCTextFieldTTF * pSender);
    virtual bool onTextFieldInsertText(CCTextFieldTTF * pSender, const char * text, int nLen);
    virtual bool onTextFieldDeleteBackward(CCTextFieldTTF * pSender, const char * delText, int nLen);
    
    // CCLayer Touch
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    bool isInTextField(CCTouch *pTouch);
    CCRect getRect();
    
    void openIME();
    void closeIME();
    
    std::string *getInputText(){ return m_pInputText;};
    
    CCTextFieldDelegate *myDelegate;
};

#endif /* defined(__drawdemo__CursorTextField__) */
