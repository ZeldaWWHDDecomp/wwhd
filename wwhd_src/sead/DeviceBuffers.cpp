// WWHD platform input buffers and motor forwarding; reconstructed from local instructions.
#include "sead/DeviceBuffers.h"

void* allocateDeviceObject(u32);
void clearDeviceBytes(void*, u32, u32);
s32 readRemoteInput(s32, void*, u32, be<s32>*);
s32 readTabletInput(s32, void*, u32, be<s32>*);
void controlRemoteMotor(s32, u32);
static u32 sRemoteControllerTable;
static u32 sTabletDeviceTable;

template <class T>
static T* deviceStorage(T* object, u32 size) {
    if (gabi::ea(object) == 0) object = static_cast<T*>(allocateDeviceObject(size));
    return object;
}

/* 0273D7AC */
static sead::RemoteBatch* constructRemoteBatch(sead::RemoteBatch* batch) {
    batch = deviceStorage(batch, sizeof(sead::RemoteBatch));
    if (batch == NULL) return NULL;
    batch->mCount = 0;
    batch->mError = -2;
    clearDeviceBytes(batch, 0, 0xF00);
    for (u32 index = 0; index < 16; ++index) batch->mSamples[index].mDeviceType = -3;
    return batch;
}

/* 0273E324 */
static sead::TabletBatch* constructTabletBatch(sead::TabletBatch* batch) {
    batch = deviceStorage(batch, sizeof(sead::TabletBatch));
    if (batch == NULL) return NULL;
    batch->mCount = 0;
    batch->mError = -2;
    clearDeviceBytes(batch, 0, 0xAC0);
    return batch;
}

/* 0273E380 */
static sead::TabletDevice* constructTabletDevice(sead::TabletDevice* device, sead::ControllerManager* manager) {
    device = deviceStorage(device, sizeof(sead::TabletDevice));
    if (device == NULL) return NULL;
    device->mManager = manager;
    device->mType = 0;
    device->mLinks.mPrevious = NULL;
    device->mLinks.mNext = NULL;
    device->mTable = &sTabletDeviceTable;
    constructTabletBatch(&device->mBatch);
    device->mType = 8;
    return device;
}

/* 0273D8EC */
void sead::RemoteDevice::read() {
    for (u32 channel = 0; channel < 4; ++channel) {
        RemoteBatch* batch = &mBatches[channel];
        s32 count = readRemoteInput(channel, batch, 16, &batch->mError);
        batch->mCount = count;
    }
}

/* 0273E3FC */
void sead::TabletDevice::read() {
    mBatch.mCount = readTabletInput(0, &mBatch, 16, &mBatch.mError);
}

/* 0273D9E4 */
static sead::RemoteController* constructRemoteController(sead::RemoteController* controller,
                                                        sead::ControllerManager* manager, s32 channel) {
    controller = deviceStorage(controller, sizeof(sead::RemoteController));
    if (controller == NULL) return NULL;
    constructController(controller, manager);
    controller->mChannel = channel;
    controller->mPreviousDeviceType = -3;
    controller->mTable = &sRemoteControllerTable;
    controller->mConnected = 0;
    controller->mDeviceFlags = 0;
    controller->mDeviceType = -3;
    controller->mType = 7;
    return controller;
}

/* 0273E298 */
u32 sead::RemoteController::isConnected() { return mConnected; }

/* 0273E304 */
void sead::MotorControl::start() { controlRemoteMotor(mController->mChannel, 1); }

/* 0273E314 */
void sead::MotorControl::stop() { controlRemoteMotor(mController->mChannel, 0); }

/* WWHD glue
 data sRemoteControllerTable 1014346C
 data sTabletDeviceTable 1014351C
 ext 0273AD10 void* allocateDeviceObject(u32)
 ext C0009990 void clearDeviceBytes(void*, u32, u32)
 ext C00081A0 s32 readRemoteInput(s32, void*, u32, be<s32>*)
 ext C0007640 s32 readTabletInput(s32, void*, u32, be<s32>*)
 ext C0008398 void controlRemoteMotor(s32, u32)
*/
