LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ALLJOYN_DIST := ../../alljoyn-android/core/alljoyn-3.4.6-dbg/cpp

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_CFLAGS := -Wno-psabi -Wno-write-strings -fuse-ld=bfd -Wl,--fix-cortex-a8 -DANDROID_NDK  -DQCC_OS_GROUP_POSIX -DQCC_OS_ANDROID -DTARGET_ANDROID -DQCC_CPU_ARM -DANDROID -DUSE_FILE32API -DGL_GLEXT_PROTOTYPES=1

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/HelloWorldScene.cpp \
                   ../../Classes/CanvasLayer.cpp \
                   ../../Classes/CursorTextField.cpp \
                   ../../Classes/DrawPictureScene.cpp \
                   ../../Classes/GeneralColorLayer.cpp \
                   ../../Classes/MessageUtil.cpp \
                   ../../Classes/ModelDialog.cpp \
                   ../../Classes/MTNotificationQueue.cpp \
                   ../../Classes/Picture.cpp \
                   ../../Classes/PictureManager.cpp \
                   ../../Classes/PlayerListLayer.cpp \
                   ../../Classes/SessionManager.cpp \
                    ../../Classes/SendQueue.cpp \
                   ../../Classes/TimeUtil.cpp \
                   ../../Classes/TintMessageLayer.cpp \
                   ../../Classes/thirdparty/jsoncpp/src/lib_json/json_reader.cpp \
                   ../../Classes/thirdparty/jsoncpp/src/lib_json/json_value.cpp \
                   ../../Classes/thirdparty/jsoncpp/src/lib_json/json_writer.cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../Classes \
	$(ALLJOYN_DIST)/inc \
	$(LOCAL_PATH)/../../Classes/thirdparty/jsoncpp/include \
	$(NDK_PLATFORMS_ROOT)/$(TARGET_PLATFORM)/arch-arm/usr/include \
	$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/include \
	$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/libs/armeabi/include \
	$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.6/include \
	$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi/include

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += chipmunk_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

LOCAL_LDLIBS += \
		-L$(ALLJOYN_DIST)/lib \
        -L$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/libs/armeabi \
        -L$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi \
    	-L./libs \
    	 $(ALLJOYN_DIST)/lib/BundledDaemon.o \
	-lajdaemon -lalljoyn -llog -lz -ldl -lssl -lcrypto -lm -lc -lstdc++  -lgcc -lgnustl_static


LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,cocos2dx/platform/third_party/android/prebuilt/libcurl)
$(call import-module,CocosDenshion/android)
$(call import-module,extensions)
$(call import-module,external/Box2D)
$(call import-module,external/chipmunk)

 
