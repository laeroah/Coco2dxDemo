//
//  SessionManager.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-20.
//
//

#include "SessionManager.h"

const static char* DRAW_SERVICE_INTERFACE_NAME = "org.alljoyn.bus.draw";
const static char* NAME_PREFIX = "org.alljoyn.bus.draw.";
const static char* DRAW_SERVICE_OBJECT_PATH = "/drawService";
const static SessionPort  DRAW_PORT = 30;

DrawObject::DrawObject(BusAttachment& bus, const char* path) : BusObject(path), drawSignalMember(NULL)
{
    QStatus status;
        
    /* Add the chat interface to this object */
    const InterfaceDescription* drawIntf = bus.GetInterface(DRAW_SERVICE_INTERFACE_NAME);
    AddInterface(*drawIntf);
        
    /* Store the Chat signal member away so it can be quickly looked up when signals are sent */
    drawSignalMember = drawIntf->GetMember("Draw");
    assert(drawSignalMember);
        
    /* Register signal handler */
    status =  bus.RegisterSignalHandler(this,
                                            static_cast<MessageReceiver::SignalHandler>(&DrawObject::DrawSignalHandler),
                                            drawSignalMember,
                                            NULL);
        
    if (ER_OK != status) {
            printf("Failed to register signal handler for ChatObject::Chat (%s)\n", QCC_StatusText(status));
    }
    
    
    /** Register the method handlers with the object */
    const MethodEntry methodEntries[] = {
        { drawIntf->GetMember("ReqSync"), static_cast<MessageReceiver::MethodHandler>(&DrawObject::ReqSync) },
        { drawIntf->GetMember("SendSync"), static_cast<MessageReceiver::MethodHandler>(&DrawObject::SendSync) }
    };
    
    status = AddMethodHandlers(methodEntries, sizeof(methodEntries) / sizeof(methodEntries[0]));
    if (ER_OK != status) {
        printf("Failed to register method handlers for BasicSampleObject.\n");
    }
}

/** Send a Draw t signal */
QStatus DrawObject::sendDrawSignal(const SessionId sessionId, const int commandId , const char* commandContent) {
        MsgArg drawArg("is", commandId, commandContent);
        uint8_t flags = 0;
        if (0 == sessionId) {
            printf("Sending Chat signal without a session id\n");
        }
        return Signal(NULL, sessionId, *drawSignalMember, &drawArg, 1, 0, flags);
}
    
/** Receive a signal from another Draw client */
void DrawObject::DrawSignalHandler(const InterfaceDescription::Member* member, const char* srcPath, Message& msg)
{
        printf("%s: %d %s\n", msg->GetSender(), msg->GetArg(0)->v_int32, msg->GetArg(0)->v_string.str);
}

void DrawObject::ReqSync(const InterfaceDescription::Member* member, Message& msg)
{
    /* Concatenate the two input strings and reply with the result. */
    int commandId = msg->GetArg(0)->v_int32;
    
    
    
    MsgArg outArg("b", 1);
    QStatus status = MethodReply(msg, &outArg, 1);
    if (ER_OK != status) {
        printf("Ping: Error sending reply.\n");
    }
}

void DrawObject::SendSync(const InterfaceDescription::Member* member, Message& msg)
{
    /* Concatenate the two input strings and reply with the result. */
    int commandId = msg->GetArg(0)->v_int32;
    qcc::String inStr2 = msg->GetArg(1)->v_string.str;
    
    MsgArg outArg("b", 1);
    QStatus status = MethodReply(msg, &outArg, 1);
    if (ER_OK != status) {
        printf("Ping: Error sending reply.\n");
    }
}


DrawBusListener::DrawBusListener(SessionManager *sessionManager,BusAttachment *busAttachment)
{
    mBusAttachment = busAttachment;
    mSessionManager = sessionManager;
}

void DrawBusListener::NameOwnerChanged(const char* busName, const char* previousOwner, const char* newOwner)
{
    if (newOwner) {
        printf("NameOwnerChanged: name=%s, oldOwner=%s, newOwner=%s.\n",
               busName,
               previousOwner ? previousOwner : "<none>",
               newOwner ? newOwner : "<none>");
    }
}

void DrawBusListener::FoundAdvertisedName(const char* name, TransportMask transport, const char* namePrefix)
{
        const char* convName = name + strlen(NAME_PREFIX);
        printf("Discovered chat conversation: \"%s\"\n", convName);
        SessionManager::getSharedInstance()->mServiceName = name;
        /* Join the conversation */
        /* Since we are in a callback we must enable concurrent callbacks before calling a synchronous method. */
        mBusAttachment->EnableConcurrentCallbacks();
        SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, true, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
        SessionId sessionId;
        QStatus status = mBusAttachment->JoinSession(name, DRAW_PORT, this, sessionId, opts);
        if (ER_OK == status) {
            printf("Joined conversation \"%s\"\n", convName);
        } else {
            printf("JoinSession failed (status=%s)\n", QCC_StatusText(status));
        }
        SessionManager::getSharedInstance()->mSessionId = sessionId;
        uint32_t timeout = 20;
        status = mBusAttachment->SetLinkTimeout(sessionId, timeout);
        if (ER_OK == status) {
            printf("Set link timeout to %d\n", timeout);
        } else {
            printf("Set link timeout failed\n");
        }
        mJoinComplete = true;
    
}

void DrawBusListener::LostAdvertisedName(const char* name, TransportMask transport, const char* namePrefix)
{
        printf("Got LostAdvertisedName for %s from transport 0x%x\n", name, transport);
}

void DrawBusListener::SessionLost(SessionId sessionId, SessionLostReason reason)
{
    if (mSessionManager->mSessionMode == ClientMode) {
        
    }
}

bool DrawBusListener::AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts)
{
        if (sessionPort != DRAW_PORT) {
            printf("Rejecting join attempt on non-chat session port %d\n", sessionPort);
            return false;
        }
        
        printf("Accepting join session request from %s (opts.proximity=%x, opts.traffic=%x, opts.transports=%x)\n",
               joiner, opts.proximity, opts.traffic, opts.transports);
        return true;
}
    
void DrawBusListener::SessionJoined(SessionPort sessionPort, SessionId id, const char* joiner)
{
    printf("SessionJoined with %s (id=%d)\n", joiner, id);
    mBusAttachment->EnableConcurrentCallbacks();
    uint32_t timeout = 20;
    QStatus status = mBusAttachment->SetLinkTimeout(id, timeout);
    if (ER_OK == status) {
            printf("Set link timeout to %d\n", timeout);
    } else {
            printf("Set link timeout failed\n");
    }
    
    if (mSessionManager->mSessionMode == ClientMode)
    {
        CCNotificationCenter::sharedNotificationCenter()->postNotification(JOIN_SESSION_NOTIFICATION, NULL);
    }
}

SessionManager::SessionManager()
{
    
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

SessionManager *SessionManager::getSharedInstance()
{
    static SessionManager *sessionManager = NULL;
    
    if (!sessionManager) {
        sessionManager = new SessionManager();
    }
    return sessionManager;
}

bool  SessionManager::initServerWithDrawerName(const char *drawerName)
{
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
    
     const TransportMask SERVICE_TRANSPORT_TYPE = TRANSPORT_ANY;
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

    
    return ER_OK == status;
}

bool  SessionManager::initClientWithDrawerName(const char *drawerName)
{
    mUserName = drawerName;
    mAdvertisedName = NAME_PREFIX;
    mAdvertisedName += drawerName;
    mSessionMode = ClientMode;
    
    QStatus status = ER_OK;
    mBusAttachment = new BusAttachment("Draw", true);
    
    if (!mBusAttachment) {
        status = ER_OUT_OF_MEMORY;
    }
    
    
    if (ER_OK == status) {
        status = CreateInterface();
    }
    
    if (ER_OK == status) {
        status = StartBus();
    }
    
    if (ER_OK == status) {
        status = ConnectToDaemon();
    }
    
    if (ER_OK == status) {
        mBusListener = new DrawBusListener(this,mBusAttachment);
        if (!mBusListener) {
            status = ER_OUT_OF_MEMORY;
        }
    }

    if (ER_OK == status) {
        mBusAttachment->RegisterBusListener(*mBusListener);
        FindAdvertisedName();
    }
    
    return ER_OK == status;
}

QStatus SessionManager::CreateInterface(void)
{
    /* Create org.alljoyn.bus.samples.chat interface */
    InterfaceDescription* drawIntf = NULL;
    QStatus status = mBusAttachment->CreateInterface(DRAW_SERVICE_INTERFACE_NAME, drawIntf);
    
    if (ER_OK == status) {
        //parameter: commandId , commandContent
        drawIntf->AddSignal("Draw", "is",  "commandId,commandContent", 0);
        drawIntf->AddMethod("ReqSync", "ii",  "b", "sessionId,commandId,returnVal", 0);
        drawIntf->AddMethod("SendSync", "iis", "b", "commandCount,sessionId,commandsContent,returnVal", 0);
        drawIntf->Activate();
    } else {
        printf("Failed to create interface \"%s\" (%s)\n", DRAW_SERVICE_INTERFACE_NAME, QCC_StatusText(status));
    }
    
    return status;
}


QStatus SessionManager::StartBus(void)
{
    QStatus status = mBusAttachment->Start();
    
    if (ER_OK == status) {
        printf("BusAttachment started.\n");
    } else {
        printf("Start of BusAttachment failed (%s).\n", QCC_StatusText(status));
    }
    
    return status;
}

QStatus SessionManager::RegisterBusObject(DrawObject *drawObject)
{
    QStatus status = mBusAttachment->RegisterBusObject(*drawObject);
    
    if (ER_OK == status) {
        printf("RegisterBusObject succeeded.\n");
    } else {
        printf("RegisterBusObject failed (%s).\n", QCC_StatusText(status));
    }
    
    return status;
}


/** Connect to the daemon, report the result to stdout, and return the status code. */
QStatus SessionManager::ConnectToDaemon(void)
{
    QStatus status = mBusAttachment->Connect();
    
    if (ER_OK == status) {
        printf("Connect to '%s' succeeded.\n", mBusAttachment->GetConnectSpec().c_str());
    } else {
        printf("Failed to connect to '%s' (%s).\n", mBusAttachment->GetConnectSpec().c_str(), QCC_StatusText(status));
    }
    
    return status;
}

/** Request the service name, report the result to stdout, and return the status code. */
QStatus SessionManager::RequestName(void)
{
    QStatus status = mBusAttachment->RequestName(mAdvertisedName.c_str(), DBUS_NAME_FLAG_DO_NOT_QUEUE);
    
    if (ER_OK == status) {
        printf("RequestName('%s') succeeded.\n", mAdvertisedName.c_str());
    } else {
        printf("RequestName('%s') failed (status=%s).\n", mAdvertisedName.c_str(), QCC_StatusText(status));
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
        printf("BindSessionPort succeeded.\n");
    } else {
        printf("BindSessionPort failed (%s).\n", QCC_StatusText(status));
    }
    
    return status;
}

/** Advertise the service name, report the result to stdout, and return the status code. */
QStatus SessionManager::AdvertiseName(TransportMask mask)
{
    QStatus status = mBusAttachment->AdvertiseName(mAdvertisedName.c_str(), mask);
    
    if (ER_OK == status) {
        printf("Advertisement of the service name '%s' succeeded.\n", mAdvertisedName.c_str());
    } else {
        printf("Failed to advertise name '%s' (%s).\n", mAdvertisedName.c_str(), QCC_StatusText(status));
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
        printf("org.alljoyn.Bus.FindAdvertisedName ('%s') succeeded.\n", DRAW_SERVICE_INTERFACE_NAME);
    } else {
        printf("org.alljoyn.Bus.FindAdvertisedName ('%s') failed (%s).\n", DRAW_SERVICE_INTERFACE_NAME, QCC_StatusText(status));
    }
    
    return status;
}


QStatus SessionManager::CallSendSync(int commandCount,std::string &commandContent)
{
    ProxyBusObject remoteObj(*mBusAttachment, mServiceName.c_str(), DRAW_SERVICE_OBJECT_PATH, mSessionId);
    const InterfaceDescription* alljoynTestIntf = mBusAttachment->GetInterface(DRAW_SERVICE_INTERFACE_NAME);
    
    assert(alljoynTestIntf);
    remoteObj.AddInterface(*alljoynTestIntf);
    
    Message reply(*mBusAttachment);
    MsgArg inputs[3];
    
    inputs[0].Set("i", mSessionId);
    inputs[1].Set("i", commandCount);
    inputs[2].Set("s", commandContent.c_str());
    
    QStatus status = remoteObj.MethodCall(DRAW_SERVICE_INTERFACE_NAME, "SendSync", inputs, 3, reply, 5000);
    
    if (ER_OK == status) {
        printf("'%s.%s' (path='%s') returned '%d'.\n", DRAW_SERVICE_INTERFACE_NAME, "SendSync",
               DRAW_SERVICE_OBJECT_PATH, reply->GetArg(0)->v_bool);
    } else {
        printf("MethodCall on '%s.%s' failed.", DRAW_SERVICE_INTERFACE_NAME, "SendSync");
    }
    
    return status;
}

QStatus SessionManager::CallReqSync(int commandId)
{
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
        printf("'%s.%s' (path='%s') returned '%d'.\n", DRAW_SERVICE_INTERFACE_NAME, "ReqSync",
               DRAW_SERVICE_OBJECT_PATH, reply->GetArg(0)->v_bool);
    } else {
        printf("MethodCall on '%s.%s' failed.", DRAW_SERVICE_INTERFACE_NAME, "ReqSync");
    }
    
    return status;
}