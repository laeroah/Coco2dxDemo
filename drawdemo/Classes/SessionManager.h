//
//  SessionManager.h
//  drawdemo
//
//  Created by Hao Wang on 14-1-20.
//
//

#ifndef __drawdemo__SessionManager__
#define __drawdemo__SessionManager__

#include <iostream>
#import <objc/runtime.h>
#import <qcc/String.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/InterfaceDescription.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>
#include <qcc/Log.h>
#include <qcc/String.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <signal.h>

using namespace ajn;

typedef enum
{
    ServerMode = 0,
    ClientMode
}SessionMode;

/* Bus object */
class DrawObject : public BusObject {
public:
    DrawObject(BusAttachment& bus, const char* path);
    QStatus sendDrawSignal(const SessionId sessionId, const int commandId , const char* commandContent);
    void DrawSignalHandler(const InterfaceDescription::Member* member, const char* srcPath, Message& msg);
    void ReqSync(const InterfaceDescription::Member* member, Message& msg);
    void SendSync(const InterfaceDescription::Member* member, Message& msg);
private:
    const InterfaceDescription::Member* drawSignalMember;
};

class DrawBusListener : public BusListener, public SessionPortListener, public SessionListener{
private:
    BusAttachment *mBusAttachment;
public:
    bool mJoinComplete;
public:
    DrawBusListener(BusAttachment *busAttachment);
    void FoundAdvertisedName(const char* name, TransportMask transport, const char* namePrefix);
    void LostAdvertisedName(const char* name, TransportMask transport, const char* namePrefix);
    void NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner);
    bool AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts);
    void SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner);
};

class SessionManager
{
    public:
    static SessionManager *getSharedInstance();
    SessionManager();
    ~SessionManager();
    bool  initServerWithDrawerName(const char *drawerName);
    bool  initClientWithDrawerName(const char *drawerName);
    QStatus RegisterBusObject(DrawObject *drawObject);
    QStatus ConnectToDaemon(void);
    QStatus RequestName(void);
    QStatus CreateSession(TransportMask mask);
    QStatus AdvertiseName(TransportMask mask);
    QStatus FindAdvertisedName(void);
    QStatus CallReqSync(SessionId sessionId,int commandId);
    QStatus CallSendSync(SessionId sessionId,int commandCount,std::string &commandContent);
    
    //client
    qcc::String mServiceName;
    SessionId mSessionId;
    
    //Session mode
    SessionMode mSessionMode;
    
    private:
        qcc::String mAdvertisedName;
        BusAttachment *mBusAttachment;
        QStatus CreateInterface(void);
        QStatus StartBus(void);
    DrawBusListener   *mBusListener;
    DrawObject *mDrawObject;
    qcc::String mUserName;
    
    
};

#endif /* defined(__drawdemo__SessionManager__) */
