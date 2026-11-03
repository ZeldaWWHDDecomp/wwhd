// WWHD remote and extension input conversion, reconstructed from local instructions.
#include "sead/RemoteInput.h"

static sead::RuntimeType sRemoteInputType;
static u32 sRemoteInputTypeGuard;
static u32 sRemoteInputTypeTable;
static sead::InputPair sRemoteNoPointer;

static u32 remoteProButtons(u32 source) {
    u32 result = (source >> 4) & 1;
    if (source & (1u << 6)) result |= 2;
    if (source & (1u << 3)) result |= 8;
    if (source & (1u << 5)) result |= 16;
    if (source & (1u << 13)) result |= 0x2000;
    if (source & (1u << 9)) result |= 0x4000;
    if (source & (1u << 7)) result |= 4;
    if (source & (1u << 2)) result |= 0x20;
    if (source & (1u << 10)) result |= 0xC00;
    if (source & (1u << 12)) result |= 0x1200;
    if (source & (1u << 11)) result |= 0x100;
    if (source & (1u << 15)) result |= 0x80000;
    if (source & (1u << 1)) result |= 0x40000;
    if (source & (1u << 0)) result |= 0x10000;
    if (source & (1u << 14)) result |= 0x20000;
    if (source & (1u << 17)) result |= 0x80;
    if (source & (1u << 16)) result |= 0x40;
    return result;
}

static u32 remoteButtons(u32 source) {
    u32 result = (source >> 11) & 1;
    if (source & (1u << 10)) result |= 2;
    if (source & (1u << 9)) result |= 0x80;
    if (source & (1u << 8)) result |= 0x40;
    if (source & (1u << 13)) result |= 0x20;
    if (source & (1u << 14)) result |= 4;
    if (source & (1u << 4)) result |= 0xC00;
    if (source & (1u << 12)) result |= 0x1200;
    if (source & (1u << 15)) result |= 0x100;
    if (source & (1u << 1)) result |= 0x80000;
    if (source & (1u << 0)) result |= 0x40000;
    if (source & (1u << 3)) result |= 0x10000;
    if (source & (1u << 2)) result |= 0x20000;
    return result;
}

static void copyRemoteSticks(sead::RemoteController* controller,sead::RemoteSample* sample) {
    u32 x = sample->mExtension[3];
    u32 y = sample->mExtension[4];
    reinterpret_cast<be<u32>&>(controller->mStickX) = x;
    reinterpret_cast<be<u32>&>(controller->mStickY) = y;
    x = sample->mExtension[5];
    y = sample->mExtension[6];
    reinterpret_cast<be<u32>&>(controller->mCrossX) = x;
    reinterpret_cast<be<u32>&>(controller->mCrossY) = y;
}

/* 0273DA78 */
static void updateRemoteController(sead::RemoteController* controller) {
    sead::RemoteDevice* device = static_cast<sead::RemoteDevice*>(controller->mManager->findDevice(7));
    if (sRemoteInputTypeGuard == 0) {
        sRemoteInputTypeGuard = 1;
        sRemoteInputType.mTable = &sRemoteInputTypeTable;
    }
    if (device == NULL) return;
    be<u32>* table = device->mTable.get();
    if (gabi::call_ptr<u32>(table[3],device,&sRemoteInputType) == 0) return;
    u32 channel = controller->mChannel;
    sead::RemoteBatch* batch = &device->mBatches[0];
    if (channel < 4) batch = &device->mBatches[channel];
    s32 count = batch->mCount;
    if ((count <= 0 && (s32)batch->mError != -1) || (u8)batch->mSamples[0].mDeviceType == 0xFD) {
        controller->mConnected = 0;
        controller->mDeviceType = -3;
        return;
    }
    controller->mButtons = 0;
    sead::RemoteSample* sample = &batch->mSamples[0];
    u8 type = sample->mDeviceType;
    if (type == 31) {
        controller->mButtons = remoteProButtons(sample->mExtension[0]);
        copyRemoteSticks(controller,sample);
        controller->updatePointer(0,0,&sRemoteNoPointer);
        controller->mLeftTrigger = 0.0f;
        controller->mConnected = 1;
        controller->mRightTrigger = 0.0f;
    } else {
        bool extension = type == 2 || type == 7;
        if ((u8)controller->mDeviceFlags == 2 && extension) {
            controller->updatePointer(0,0,&sRemoteNoPointer);
        } else {
            u32 source = sample->mButtons;
            u32 contact = (source >> 11) & 1;
            controller->mButtons = remoteButtons(source);
            gabi::Local<sead::InputPair> point;
            point->mX = (u32)sample->mPointer.mX;
            point->mY = (u32)sample->mPointer.mY;
            s32 valid = sample->mPointerValid;
            controller->updatePointer(valid > 0,contact,point.get());
        }
        type = sample->mDeviceType;
        if (type == 1 || type == 6) {
            u32 x = sample->mExtension[0];
            u32 y = sample->mExtension[1];
            controller->mCrossX = 0.0f;
            reinterpret_cast<be<u32>&>(controller->mStickX) = x;
            controller->mCrossY = 0.0f;
            reinterpret_cast<be<u32>&>(controller->mStickY) = y;
        } else {
            controller->mCrossY = 0.0f;
            controller->mCrossX = 0.0f;
            controller->mStickX = 0.0f;
            controller->mStickY = 0.0f;
        }
        if ((u8)controller->mDeviceFlags != 0 && extension) {
            u32 source = sample->mExtension[0];
            if (source & 0x10) controller->mButtons = (u32)controller->mButtons | 1;
            if (source & 0x40) controller->mButtons = (u32)controller->mButtons | 2;
            if (source & 8) controller->mButtons = (u32)controller->mButtons | 8;
            if (source & 0x20) controller->mButtons = (u32)controller->mButtons | 16;
            if (source & 0x400) controller->mButtons = (u32)controller->mButtons | 0xC00;
            if (source & 0x1000) controller->mButtons = (u32)controller->mButtons | 0x1200;
            if (source & 0x2000) controller->mButtons = (u32)controller->mButtons | 0x2000;
            if (source & 0x200) controller->mButtons = (u32)controller->mButtons | 0x4000;
            if (source & 0x80) controller->mButtons = (u32)controller->mButtons | 4;
            if (source & 4) controller->mButtons = (u32)controller->mButtons | 0x20;
            if (source & 0x800) controller->mButtons = (u32)controller->mButtons | 0x100;
            if (source & 1) controller->mButtons = (u32)controller->mButtons | 0x10000;
            if (source & 0x4000) controller->mButtons = (u32)controller->mButtons | 0x20000;
            if (source & 2) controller->mButtons = (u32)controller->mButtons | 0x40000;
            if (source & 0x8000) controller->mButtons = (u32)controller->mButtons | 0x80000;
            copyRemoteSticks(controller,sample);
        }
        controller->mLeftTrigger = 0.0f;
        controller->mRightTrigger = 0.0f;
        controller->mConnected = 1;
    }
    type = sample->mDeviceType;
    controller->mDeviceType = type;
    if (type < 0xFC) controller->mPreviousDeviceType = type;
}

/* WWHD glue
 data sRemoteInputType 101FD978
 data sRemoteInputTypeGuard 101FD698
 data sRemoteInputTypeTable 10143458
 data sRemoteNoPointer 104A0908
*/
