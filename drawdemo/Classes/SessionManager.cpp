//
//  SessionManager.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-20.
//
//

#include "SessionManager.h"
#include "cocos2d.h"
#include "Picture.h"
#include "MTNotificationQueue.h"
#include "MessageUtil.h"


using namespace cocos2d;


const static char* DRAW_SERVICE_INTERFACE_NAME = "org.alljoyn.bus.draw";
const static char* NAME_PREFIX = "org.alljoyn.bus.draw.";
const static char* DRAW_SERVICE_OBJECT_PATH = "/drawService";
const static SessionPort  DRAW_PORT = 30;
const static TransportMask SERVICE_TRANSPORT_TYPE = TRANSPORT_ANY;

DrawObject::DrawObject(BusAttachment& bus, const char* path) : BusObject(path), drawSignalMember(NULL)
{
    QStatus status;
    mBusAttachment = &bus;
    /* Add the draw interface to this object */
    const InterfaceDescription* drawIntf = bus.GetInterface(DRAW_SERVICE_INTERFACE_NAME);
    AddInterface(*drawIntf);
        
    /* Store the Draw signal member away so it can be quickly looked up when signals are sent */
    drawSignalMember = drawIntf->GetMember("Draw");
    assert(drawSignalMember);
        
    /* Register signal handler */
    status =  bus.RegisterSignalHandler(this,
                                            static_cast<MessageReceiver::SignalHandler>(&DrawObject::DrawSignalHandler),
                                            drawSignalMember,
                                            NULL);
        
    if (ER_OK != status) {
          CCLOG("Failed to register signal handler for DrawObject::Draw (%s)\n", QCC_StatusText(status));
    }
    
    
    /** Register the method handlers with the object */
    const MethodEntry methodEntries[] = {
        { drawIntf->GetMember("ReqSync"), static_cast<MessageReceiver::MethodHandler>(&DrawObject::ReqSync) },
        { drawIntf->GetMember("SendSync"), static_cast<MessageReceiver::MethodHandler>(&DrawObject::SendSync) }
    };
    
    status = AddMethodHandlers(methodEntries, sizeof(methodEntries) / sizeof(methodEntries[0]));
    if (ER_OK != status) {
    	CCLOG("Failed to register method handlers for BasicSampleObject.\n");
    }
}

/** Send a Draw t signal */
QStatus DrawObject::sendDrawSignal(const SessionId sessionId, const int commandId , const char* commandContent) {
        MsgArg drawArg("is", commandId, commandContent);
        uint8_t flags = 0;
        if (0 == sessionId) {
        	CCLOG("Sending Draw signal without a session id\n");
        }
        return Signal(NULL, sessionId, *drawSignalMember, &drawArg, 1, 0, flags);
}
    
/** Receive a signal from another Draw client */
void DrawObject::DrawSignalHandler(const InterfaceDescription::Member* member, const char* srcPath, Message& msg)
{
	CCLOG("%s: %d %s\n", msg->GetSender(), msg->GetArg(0)->v_int32, msg->GetArg(0)->v_string.str);
}

void DrawObject::ReqSync(const InterfaceDescription::Member* member, Message& msg)
{
    mBusAttachment->EnableConcurrentCallbacks();
    /* Concatenate the two input strings and reply with the result. */
    DrawCommand *drawCommand = new DrawCommand();
    drawCommand->mCommandId = msg->GetArg(1)->v_int32;
    MsgArg outArg("b", 1);
    QStatus status = MethodReply(msg, &outArg, 1);
    if (ER_OK != status) {
    	CCLOG("Ping: Error sending reply.\n");
    }
    
    MTNotificationQueue::sharedInstance()->postNotification(REQ_SYNC_NOTIFICATION, drawCommand);
    
}

void DrawObject::SendSync(const InterfaceDescription::Member* member, Message& msg)
{
    mBusAttachment->EnableConcurrentCallbacks();
    SyncCommandContent *syncCommandContent = new SyncCommandContent();
    syncCommandContent->mCommandCount = msg->GetArg(0)->v_int32;
    syncCommandContent->mCommandContent = msg->GetArg(1)->v_string.str;
    MsgArg outArg("b", 1);
    QStatus status = MethodReply(msg, &outArg, 1);
    if (ER_OK != status) {
    	CCLOG("Ping: Error sending reply.\n");
    }
    
    CCLOG("recv sync command %s",syncCommandContent->mCommandContent.c_str());
    
    MTNotificationQueue::sharedInstance()->postNotification(SYNC_COMMAND_NOTIFICATION, syncCommandContent);
}


DrawBusListener::DrawBusListener(SessionManager *sessionManager,BusAttachment *busAttachment)
{
    mBusAttachment = busAttachment;
    mSessionManager = sessionManager;
}

void DrawBusListener::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner)
{
    if (newOwner && strcmp(newOwner, mSessionManager->getAdvertiseName().c_str())) {
        printf("NameOwnerChanged: name=%s, oldOwner=%s, newOwner=%s.\n",
               busName,
               previousOwner ? previousOwner : "<none>",
               newOwner ? newOwner : "<none>");
    }
}

void DrawBusListener::FoundAdvertisedName(const char* name, TransportMask transport, const char* namePrefix)
{
    if (!strcmp(name, namePrefix) ||
        !strcmp(name, SessionManager::getSharedInstance()->getAdvertiseName().c_str())) {
        return;
    }
    
    const char* convName = name + strlen(NAME_PREFIX);
    MessageUtil::postMessage("Player %s found", convName);
    
    Player *newPlayer = new Player();
    newPlayer->mPlayerName = convName;
    newPlayer->mplayerWellKnownName = name;
    MTNotificationQueue::sharedInstance()->postNotification( PLAYER_FOUND_NOTIFICATION, newPlayer);
}

void DrawBusListener::LostAdvertisedName(const char* name, TransportMask transport, const char* namePrefix)
{
    if (mSessionManager->mSessionMode == ClientMode)
    {
        MTNotificationQueue::sharedInstance()->postNotification(LOST_SESSION_NOTIFICATION, NULL);
    }
    
    const char* convName = name + strlen(NAME_PREFIX);
    CCLOG("Lost draw conversation: \"%s\"\n", convName);

    Player *newPlayer = new Player();
    newPlayer->mPlayerName = convName;
    newPlayer->mplayerWellKnownName = name;
    MTNotificationQueue::sharedInstance()->postNotification( PLAYER_UNFOUND_NOTIFICATION, newPlayer);

}

void DrawBusListener::SessionLost(SessionId sessionId, SessionLostReason reason)
{
	CCLOG("Got Session Lost\n");
    mBusAttachment->EnableConcurrentCallbacks();
    mBusAttachment->LeaveSession(mSessionManager->mSessionId);
    mSessionManager->mSessionId = 0;
    
    if (mSessionManager->mSessionMode == ClientMode)
    {
        MTNotificationQueue::sharedInstance()->postNotification(LOST_SESSION_NOTIFICATION, NULL);
    }
}

bool DrawBusListener::AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts)
{
    if (sessionPort != DRAW_PORT) {
    	CCLOG("Rejecting join attempt on non-draw session port %d\n", sessionPort);
        return false;
    }
    
    if (mSessionManager->mSessionMode == ClientMode) {
        return false;
    }
    
    CCLOG("Accepting join session request from %s (opts.proximity=%x, opts.traffic=%x, opts.transports=%x)\n",
               joiner, opts.proximity, opts.traffic, opts.transports);
    
    return true;
}
    
void DrawBusListener::SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner)
{
    printf("SessionJoined with %s (id=%d)\n", joiner, id);
    SessionManager::getSharedInstance()->mServiceName = joiner;
    SessionManager::getSharedInstance()->mSessionId = id;
    mBusAttachment->EnableConcurrentCallbacks();
    uint32_t timeout = 40;
    QStatus status = mBusAttachment->SetLinkTimeout(id, timeout);
    if (ER_OK == status) {
    	CCLOG("Set link timeout to %d\n", timeout);
    } else {
    	CCLOG("Set link timeout failed\n");
    }
}

SessionManager::SessionManager()
{
    mIsInitialized = false;
}

SessionManager::~SessionManager()
{
    if (mDrawObject) {
        delete mDrawObject;
    }
    
    if (mBusListener) {
        delete  mBusListener;
    }
    
    if (mBusAttachment) {
        delete mBusAttachment;
    }
}

void    SessionManager::runProcess()
{
    if (mSessionMode != ClientMode) {
        return;
    }
}

static SessionManager *sessionManager = NULL;

SessionManager *SessionManager::getSharedInstance()
{
    if (!sessionManager) {
        sessionManager = new SessionManager();
    }
    return sessionManager;
}

void SessionManager::reset()
{
    if (!sessionManager)
    {
        delete sessionManager;
    }
    sessionManager = NULL;
}

bool  SessionManager::initServerWithDrawerName(const char *drawerName)
{
    MessageUtil::postMessage("Platform is initializing, please wait!");
    
    mAdvertisedName = NAME_PREFIX;
    mAdvertisedName += drawerName;
    mSessionMode = ServerMode;
    
    QStatus status = ER_OK;
    mBusAttachment = new BusAttachment("Draw", true);
    
    if (!mBusAttachment) {
        status = ER_OUT_OF_MEMORY;
    }
    
    if (ER_OK == status) {
        mBusListener = new DrawBusListener(this,mBusAttachment);
        if (!mBusListener) {
             status = ER_OUT_OF_MEMORY;
        }
    }
    
    if (ER_OK == status) {
        status = CreateInterface();
    }
    
    if (ER_OK == status) {
        mBusAttachment->RegisterBusListener(*mBusListener);
    }
    
    if (ER_OK == status) {
        status = StartBus();
    }
    
    mDrawObject = new DrawObject(*mBusAttachment, DRAW_SERVICE_OBJECT_PATH);
    
    if (!mDrawObject) {
        status = ER_OUT_OF_MEMORY;
    }
    
    if (ER_OK == status) {
        status = RegisterBusObject(mDrawObject);
    }
    
    if (ER_OK == status) {
        status = ConnectToDaemon();
    }
    
    
    /*
     * Advertise this service on the bus.
     * There are three steps to advertising this service on the bus.
     * 1) Request a well-known name that will be used by the client to discover
     *    this service.
     * 2) Create a session.
     * 3) Advertise the well-known name.
     */
    if (ER_OK == status) {
        status = RequestName();
    }
    
    if (ER_OK == status) {
        status = CreateSession(SERVICE_TRANSPORT_TYPE);
    }
    
    if (ER_OK == status) {
        status = AdvertiseName(SERVICE_TRANSPORT_TYPE);
    }
    
    FindAdvertisedName();
    
    if (ER_OK == status) {
        MessageUtil::postMessage("Platform was initialized successfully!");
        mIsInitialized = true;
    }
    else
    {
        MessageUtil::postMessage("Platform was initialized failure!");
    }
    return ER_OK == status;
}

QStatus SessionManager::CreateInterface(void)
{
    InterfaceDescription* drawIntf = NULL;
    QStatus status = mBusAttachment->CreateInterface(DRAW_SERVICE_INTERFACE_NAME, drawIntf);
    
    if (ER_OK == status) {
        //parameter: commandId , commandContent
        drawIntf->AddSignal("Draw", "is",  "commandId,commandContent", 0);
        drawIntf->AddMethod("ReqSync", "ii",  "b", "sessionId,commandId,returnVal", 0);
        drawIntf->AddMethod("SendSync", "is", "b", "commandCount,commandsContent,returnVal", 0);
        drawIntf->Activate();
    } else {
    	CCLOG("Failed to create interface \"%s\" (%s)\n", DRAW_SERVICE_INTERFACE_NAME, QCC_StatusText(status));
    }
    
    return status;
}


QStatus SessionManager::StartBus(void)
{
    QStatus status = mBusAttachment->Start();
    
    if (ER_OK == status) {
    	CCLOG("BusAttachment started.\n");
    } else {
    	CCLOG("Start of BusAttachment failed (%s).\n", QCC_StatusText(status));
    }
    
    return status;
}

QStatus SessionManager::RegisterBusObject(DrawObject *drawObject)
{
    QStatus status = mBusAttachment->RegisterBusObject(*drawObject);
    
    if (ER_OK == status) {
    	CCLOG("RegisterBusObject succeeded.\n");
    } else {
    	CCLOG("RegisterBusObject failed (%s).\n", QCC_StatusText(status));
    }
    
    return status;
}


/** Connect to the daemon, report the result to stdout, and return the status code. */
QStatus SessionManager::ConnectToDaemon(void)
{
    QStatus status = mBusAttachment->Connect();
    
    if (ER_OK == status) {
    	CCLOG("Connect to '%s' succeeded.\n", mBusAttachment->GetConnectSpec().c_str());
    } else {
    	CCLOG("Failed to connect to '%s' (%s).\n", mBusAttachment->GetConnectSpec().c_str(), QCC_StatusText(status));
    }
    
    return status;
}

/** Request the service name, report the result to stdout, and return the status code. */
QStatus SessionManager::RequestName(void)
{
    QStatus status = mBusAttachment->RequestName(mAdvertisedName.c_str(), DBUS_NAME_FLAG_DO_NOT_QUEUE );
    
    if (ER_OK == status)
    {
    	CCLOG("RequestName('%s') succeeded.\n", mAdvertisedName.c_str());
    } else {
    	CCLOG("RequestName('%s') failed (status=%s).\n", mAdvertisedName.c_str(), QCC_StatusText(status));
    }
    
    return status;
}

/** Create the session, report the result to stdout, and return the status code. */
QStatus SessionManager::CreateSession(TransportMask mask)
{
    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, mask);
    SessionPort sp = DRAW_PORT;
    QStatus status = mBusAttachment->BindSessionPort(sp, opts, *mBusListener);
    
    if (ER_OK == status) {
    	CCLOG("Create session succeeded.");
    } else {
    	CCLOG("Create session failed. %s",QCC_StatusText(status));
    }
    
    return status;
}

/** Advertise the service name, report the result to stdout, and return the status code. */
QStatus SessionManager::AdvertiseName(TransportMask mask)
{
    QStatus status = mBusAttachment->AdvertiseName(mAdvertisedName.c_str(), mask);
    
    if (ER_OK == status) {
    	CCLOG("Advertisement of the service name '%s' succeeded.\n", mAdvertisedName.c_str());
    } else {
    	CCLOG("Failed to advertise name '%s' (%s).\n", mAdvertisedName.c_str(), QCC_StatusText(status));
    }
    
    return status;
}

QStatus SessionManager::CancelAdvertiseName(TransportMask mask)
{
    QStatus status = mBusAttachment->CancelAdvertiseName(mAdvertisedName.c_str(), mask);
    
    if (ER_OK == status) {
    	CCLOG("Cancel advertisement of the service name '%s' succeeded.\n", mAdvertisedName.c_str());
    } else {
    	CCLOG("Failed to cancel advertise name '%s' (%s).\n", mAdvertisedName.c_str(), QCC_StatusText(status));
    }
    
    return status;
}

/** Begin discovery on the well-known name of the service to be called, report the result to
 stdout, and return the result status. */
QStatus SessionManager::FindAdvertisedName(void)
{
    /* Begin discovery on the well-known name of the service to be called */
    QStatus status = mBusAttachment->FindAdvertisedName(DRAW_SERVICE_INTERFACE_NAME);
    
    if (status == ER_OK) {
    	CCLOG("org.alljoyn.Bus.FindAdvertisedName ('%s') succeeded.\n", DRAW_SERVICE_INTERFACE_NAME);
    } else {
    	CCLOG("org.alljoyn.Bus.FindAdvertisedName ('%s') failed (%s).\n", DRAW_SERVICE_INTERFACE_NAME, QCC_StatusText(status));
    }
    
    return status;
}

QStatus SessionManager::CancelFindAdvertisedName(void)
{
    /* Begin discovery on the well-known name of the service to be called */
    QStatus status = mBusAttachment->CancelFindAdvertisedName(DRAW_SERVICE_INTERFACE_NAME);
    
    if (status == ER_OK) {
    	CCLOG("org.alljoyn.Bus.FindAdvertisedName ('%s') succeeded.\n", DRAW_SERVICE_INTERFACE_NAME);
    } else {
    	CCLOG("org.alljoyn.Bus.FindAdvertisedName ('%s') failed (%s).\n", DRAW_SERVICE_INTERFACE_NAME, QCC_StatusText(status));
    }
    
    return status;
}


QStatus SessionManager::CallSendSync(int commandCount,string &commandContent)
{
     mBusAttachment->EnableConcurrentCallbacks();
    ProxyBusObject remoteObj(*mBusAttachment, mServiceName.c_str(), DRAW_SERVICE_OBJECT_PATH, mSessionId);
    const InterfaceDescription* alljoynTestIntf = mBusAttachment->GetInterface(DRAW_SERVICE_INTERFACE_NAME);
    
    assert(alljoynTestIntf);
    remoteObj.AddInterface(*alljoynTestIntf);
    
    Message reply(*mBusAttachment);
    MsgArg inputs[2];
    
    inputs[0].Set("i", commandCount);
    inputs[1].Set("s", commandContent.c_str());
    
    QStatus status = remoteObj.MethodCall(DRAW_SERVICE_INTERFACE_NAME, "SendSync", inputs, 2, reply, 5000);
    
    if (ER_OK == status) {
    	CCLOG("'%s.%s' (path='%s') returned '%d'.\n", DRAW_SERVICE_INTERFACE_NAME, "SendSync",
               DRAW_SERVICE_OBJECT_PATH, reply->GetArg(0)->v_bool);
    } else {
    	CCLOG("MethodCall on '%s.%s' failed. %s", DRAW_SERVICE_INTERFACE_NAME, "SendSync",QCC_StatusText(status));
    }
    
    return status;
}

QStatus SessionManager::CallReqSync(int commandId)
{
    mBusAttachment->EnableConcurrentCallbacks();
    ProxyBusObject remoteObj(*mBusAttachment, mServiceName.c_str(), DRAW_SERVICE_OBJECT_PATH, mSessionId);
    const InterfaceDescription* alljoynTestIntf = mBusAttachment->GetInterface(DRAW_SERVICE_INTERFACE_NAME);
    
    assert(alljoynTestIntf);
    remoteObj.AddInterface(*alljoynTestIntf);
    
    Message reply(*mBusAttachment);
    MsgArg inputs[2];
    
    inputs[0].Set("i", mSessionId);
    inputs[1].Set("i", commandId);
    
    QStatus status = remoteObj.MethodCall(DRAW_SERVICE_INTERFACE_NAME, "ReqSync", inputs, 2, reply, 5000);
    
    if (ER_OK == status) {
    	CCLOG("'%s.%s' (path='%s') returned '%d'.\n", DRAW_SERVICE_INTERFACE_NAME, "ReqSync",
               DRAW_SERVICE_OBJECT_PATH, reply->GetArg(0)->v_bool);
    } else {
    	CCLOG("MethodCall on '%s.%s' failed. %s", DRAW_SERVICE_INTERFACE_NAME, "ReqSync",QCC_StatusText(status));
    }
    
    return status;
}

QStatus SessionManager::JoinSession(const char *wellKnownName)
{
    if (mSessionMode == ServerMode)
    {
        CancelAdvertiseName(SERVICE_TRANSPORT_TYPE);
        mSessionMode = ClientMode;
    }
    else
    {
        //leave old session first
        leaveSession();
    }
    
    mServiceName = wellKnownName;
    mBusAttachment->EnableConcurrentCallbacks();
    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
    SessionId sessionId;
    QStatus status = mBusAttachment->JoinSession(wellKnownName, DRAW_PORT, mBusListener, sessionId, opts);
    if (ER_OK == status) {
        MessageUtil::postMessage("Joined conversation \"%s\"\n", wellKnownName);
    } else {
        mSessionMode = ServerMode;
        MessageUtil::postMessage("JoinSession failed (status=%s)\n", QCC_StatusText(status));
        return status;
    }
    mSessionId = sessionId;
    uint32_t timeout = 40;
    status = mBusAttachment->SetLinkTimeout(sessionId, timeout);
    if (ER_OK == status) {
        printf("Set link timeout to %d\n", timeout);
        if (mSessionMode == ClientMode)
        {
            MTNotificationQueue::sharedInstance()->postNotification(JOIN_SESSION_NOTIFICATION, NULL);
        }
    } else {
    	CCLOG("Set link timeout failed\n");
    }
    return status;
}


QStatus SessionManager::leaveSession()
{
    if (mSessionId == 0) {
        return ER_OK;
    }
    
    QStatus status = mBusAttachment->LeaveSession(mSessionId);
    
    if (status == ER_OK) {
        mSessionId = 0;
    }
    
    mSessionMode = ServerMode;
    
    AdvertiseName(SERVICE_TRANSPORT_TYPE);
    
    return status;
}

