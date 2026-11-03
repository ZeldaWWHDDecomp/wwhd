// WWHD input object layouts, reconstructed from the local game's accesses.
#pragma once
#include "sead/InputState.h"
namespace sead {
struct OffsetList : Link {
    be<s32> mCount;
    be<u32> mMemberOffset;
};
struct ControllerManager;
struct Controller : InputState {
    be<u32> mType;
    gptr<ControllerManager> mManager;
    OffsetList mAddons;
    OffsetList mWrappers;
    gptr<be<u32>> mTable;
    void calc();
};
struct ControllerWrapper : InputState {
    u8 mObjectHeader[12];
    be<u32> mBaseTable;
    gptr<Controller> mController;
    be<u8> mEnabled;
    u8 mAlignEnabled[3];
    be<u32> mSampleCount;
    be<u32> mSampleIndex;
    u8 mWrapperPadding[32];
    gptr<be<u32>> mTable;
    void detach();
    void attach(Controller* controller, u32 copyRepeat);
    void setEnabled(u8 enabled);
};
struct BufferedController : Controller {
    be<u8> mConnected;
    u8 mAlignment[3];
    PointerBuffer mSamples;
    void clearSamples();
    void appendSample(Controller* sample);
    u32 isConnected();
};
struct ChannelController : Controller {
    be<s32> mChannel;
    be<u8> mConnected;
    u8 mAlignment[3];
    u32 isConnected();
};
struct ControllerManager {
    u8 mBaseData[32];
    be<u32> mBaseTable;
    u8 mBaseReserved[8];
    gptr<void> mInitParameters;
    u8 mTaskData[64];
    be<u32> mTaskTable;
    u8 mTaskReserved[84];
    be<u8> mInstalled;
    u8 mAlignment[3];
    OffsetList mDevices;
    PointerBuffer mControllers;
    Controller* findController(u32 type, s32 occurrence);
    void* findDevice(u32 type);
    s32 indexOf(Controller* controller);
    void allocateControllerBuffer(s32 capacity, void* heap);
    void install();
    void initializeDefaults(void* heap);
    void initialize();
    void calc();
};
struct DeviceRecord {
    Link mLinks;
    be<u32> mType;
    gptr<ControllerManager> mManager;
    gptr<be<u32>> mTable;
};
}
WWHD_OFFSET(sead::OffsetList,mCount,8);
WWHD_OFFSET(sead::OffsetList,mMemberOffset,12);
WWHD_SIZE(sead::OffsetList,16);
WWHD_OFFSET(sead::Controller,mType,0x130);
WWHD_OFFSET(sead::Controller,mManager,0x134);
WWHD_OFFSET(sead::Controller,mAddons,0x138);
WWHD_OFFSET(sead::Controller,mWrappers,0x148);
WWHD_OFFSET(sead::Controller,mTable,0x158);
WWHD_SIZE(sead::Controller,0x15C);
WWHD_OFFSET(sead::ControllerWrapper,mBaseTable,0x13C);
WWHD_OFFSET(sead::ControllerWrapper,mController,0x140);
WWHD_OFFSET(sead::ControllerWrapper,mEnabled,0x144);
WWHD_OFFSET(sead::ControllerWrapper,mSampleCount,0x148);
WWHD_OFFSET(sead::ControllerWrapper,mSampleIndex,0x14C);
WWHD_OFFSET(sead::ControllerWrapper,mTable,0x170);
WWHD_SIZE(sead::ControllerWrapper,0x174);
WWHD_OFFSET(sead::BufferedController,mConnected,0x15C);
WWHD_OFFSET(sead::BufferedController,mSamples,0x160);
WWHD_SIZE(sead::BufferedController,0x16C);
WWHD_OFFSET(sead::ChannelController,mChannel,0x15C);
WWHD_OFFSET(sead::ChannelController,mConnected,0x160);
WWHD_SIZE(sead::ChannelController,0x164);
WWHD_OFFSET(sead::ControllerManager,mInstalled,0xC8);
WWHD_OFFSET(sead::ControllerManager,mDevices,0xCC);
WWHD_OFFSET(sead::ControllerManager,mControllers,0xDC);
WWHD_SIZE(sead::ControllerManager,0xE8);
WWHD_OFFSET(sead::DeviceRecord,mType,8);
WWHD_OFFSET(sead::DeviceRecord,mTable,16);
WWHD_SIZE(sead::DeviceRecord,20);
void copyControllerRepeat(sead::InputState* destination,const sead::InputState* source);
