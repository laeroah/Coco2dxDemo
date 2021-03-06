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
#include <string>
#include "cocos2d.h"
#include <qcc/String.h>
#define JOIN_SESSION_NOTIFICATION "JoinSession"
#define LOST_SESSION_NOTIFICATION "LostSession"
#define REQ_SYNC_NOTIFICATION "ReqSync"
#define SYNC_COMMAND_NOTIFICATION "SyncCommandContent"
#define SYSTEM_MESSAGE "systemMessage"
#define PLAYER_FOUND_NOTIFICATION "playerFound"
#define PLAYER_UNFOUND_NOTIFICATION "playerUnFound"

using namespace ajn;
using namespace std;

typedef enum
{
    ServerMode = 0,
    ClientMode
}SessionMode;

/* Bus object */
class DrawObject : public BusObject {
public:
    BusAttachment *mBusAttachment;
    DrawObject(BusAttachment& bus, const char* path);
    QStatus sendDrawSignal(const SessionId sessionId, const int commandId , const char* commandContent);
    void DrawSignalHandler(const InterfaceDescription::Member* member, const char* srcPath, Message& msg);
    void ReqSync(const InterfaceDescription::Member* member, Message& msg);
    void SendSync(const InterfaceDescription::Member* member, Message& msg);
private:
    const InterfaceDescription::Member* drawSignalMember;
};

class SessionManager;

class DrawBusListener : public BusListener, public SessionPortListener, public SessionListener{
private:
    BusAttachment *mBusAttachment;
    SessionManager *mSessionManager;
public:
    DrawBusListener(SessionManager *sessionManager, BusAttachment *busAttachment);
    void FoundAdvertisedName(const char* name, TransportMask transport, const char* namePrefix);
    void LostAdvertisedName(const char* name, TransportMask transport, const char* namePrefix);
    void NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner);
    bool AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts);
    void SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner);
    void SessionLost(SessionId sessionId, SessionLostReason reason);
};

class SessionManager
{
    public:
    static SessionManager *getSharedInstance();
    static void reset();
    SessionManager();
    ~SessionManager();
    bool  initServerWithDrawerName(const char *drawerName);
    void    clearProcess();
    QStatus RegisterBusObject(DrawObject *drawObject);
    QStatus ConnectToDaemon(void);
    QStatus RequestName(void);
    QStatus CreateSession(TransportMask mask);
    QStatus AdvertiseName(TransportMask mask);
    QStatus CancelAdvertiseName(TransportMask mask);
    QStatus CancelFindAdvertisedName(void);
    QStatus FindAdvertisedName(void);
    QStatus CallReqSync(int commandId);
    QStatus CallSendSync(int commandCount,std::string &commandContent);
    QStatus JoinSession(const char *wellKnownName);
    QStatus leaveSession();
    QStatus DestroySession(TransportMask mask);
    QStatus ReleaseName(void);
    QStatus DisconnectToDaemon(void);
    QStatus UnregisterBusObject();
    QStatus StopBus(void);
    bool isInitialized(){ return mIsInitialized; };
    void    runProcess();
    
    qcc::String mServiceName;
    SessionId mSessionId;
    
    //Session mode
    SessionMode mSessionMode;
    qcc::String& getAdvertiseName(){ return mAdvertisedName;};
    private:
        qcc::String mAdvertisedName;
        BusAttachment *mBusAttachment;
        QStatus CreateInterface(void);
        QStatus StartBus(void);
    DrawBusListener   *mBusListener;
    DrawObject *mDrawObject;
    qcc::String mUserName;
    
    bool mIsInitialized;
};

#endif /* defined(__drawdemo__SessionManager__) */
