// WWHD remote and tablet input batches; only locally observed fields are named.
#pragma once
#include "sead/ControllerConstruction.h"
namespace sead {
struct RemoteSample {
    be<u32> mButtons;
    u8 mMotion[28];
    InputPair mPointer;
    u8 mPointerStatus[52];
    be<s8> mDeviceType;
    be<u8> mStatus;
    be<s8> mPointerValid;
    u8 mAlignment;
    be<u32> mExtension[7];
    u8 mRemainingStatus[116];
};
struct RemoteBatch {
    RemoteSample mSamples[16];
    be<s32> mCount;
    be<s32> mError;
};
struct TabletTouch { be<u16> mX; be<u16> mY; be<u16> mTouched; be<u16> mValidity; };
struct TabletSample {
    be<u32> mButtons;
    u8 mButtonHistory[8];
    InputPair mStick;
    InputPair mCross;
    u8 mMotion[54];
    TabletTouch mTouches[3];
    u8 mRemaining[66];
};
struct TabletBatch {
    TabletSample mSamples[16];
    be<s32> mCount;
    be<s32> mError;
};
struct RemoteDevice : DeviceRecord {
    u8 mMotionWorkspace[0x1104];
    RemoteBatch mBatches[4];
    void read();
};
struct TabletDevice : DeviceRecord {
    TabletBatch mBatch;
    void read();
};
struct MotorControl {
    u8 mHeader[12];
    gptr<ChannelController> mController;
    void start();
    void stop();
};
struct RemoteController : Controller {
    be<s32> mChannel;
    be<s8> mDeviceType;
    be<s8> mPreviousDeviceType;
    be<u8> mDeviceFlags;
    be<u8> mConnected;
    u32 isConnected();
};
}
WWHD_OFFSET(sead::RemoteSample,mDeviceType,0x5C);
WWHD_SIZE(sead::RemoteSample,0xF0);
WWHD_OFFSET(sead::RemoteBatch,mCount,0xF00);
WWHD_OFFSET(sead::RemoteBatch,mError,0xF04);
WWHD_SIZE(sead::RemoteBatch,0xF08);
WWHD_OFFSET(sead::TabletBatch,mCount,0xAC0);
WWHD_OFFSET(sead::TabletBatch,mError,0xAC4);
WWHD_SIZE(sead::TabletBatch,0xAC8);
WWHD_OFFSET(sead::RemoteDevice,mBatches,0x1118);
WWHD_SIZE(sead::RemoteDevice,0x4D38);
WWHD_OFFSET(sead::TabletDevice,mBatch,0x14);
WWHD_SIZE(sead::TabletDevice,0xADC);
WWHD_OFFSET(sead::MotorControl,mController,12);
WWHD_SIZE(sead::MotorControl,16);
WWHD_OFFSET(sead::RemoteController,mConnected,0x163);
WWHD_SIZE(sead::RemoteController,0x164);

WWHD_OFFSET(sead::TabletSample,mTouches,0x52);
WWHD_SIZE(sead::TabletSample,0xAC);
WWHD_SIZE(sead::TabletTouch,8);
