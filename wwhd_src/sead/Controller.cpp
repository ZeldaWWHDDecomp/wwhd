// WWHD controller registration and lookup. Reconstructed from local instructions.
#include "sead/Controller.h"
#include "sead/ElementAt.h"

static sead::ControllerManager* sControllerManager;

/* 0273C714 */
void sead::ControllerManager::install() {
    if (sControllerManager != NULL) return;
    sControllerManager = this;
    mInstalled = 1;
}

/* 0273CB44 */
sead::Controller* sead::ControllerManager::findController(u32 type, s32 occurrence) {
    u32 count = mControllers.mCount;
    gptr<Controller>* entry = reinterpret_cast<gptr<Controller>*>(mControllers.mStorage.get());
    gptr<Controller>* end = entry + count;
    while (entry != end) {
        Controller* controller = *entry;
        if ((u32)controller->mType == type) {
            if (occurrence == 0) return controller;
            occurrence = (u32)occurrence - 1;
        }
        ++entry;
    }
    return NULL;
}

/* 0273CB8C */
void* sead::ControllerManager::findDevice(u32 type) {
    u32 offset = mDevices.mMemberOffset;
    be<u8>* end = reinterpret_cast<be<u8>*>(&mDevices) - offset;
    be<u8>* object = reinterpret_cast<be<u8>*>(mDevices.mNext.get()) - offset;
    while (object != end) {
        DeviceRecord* device = reinterpret_cast<DeviceRecord*>(object);
        if ((u32)device->mType == type) return device;
        Link* node = reinterpret_cast<Link*>(object + offset);
        object = reinterpret_cast<be<u8>*>(node->mNext.get()) - offset;
    }
    return NULL;
}

/* 0273CCA4 */
s32 sead::ControllerManager::indexOf(Controller* controller) {
    u32 count = mControllers.mCount;
    gptr<Controller>* entry = reinterpret_cast<gptr<Controller>*>(mControllers.mStorage.get());
    gptr<Controller>* end = entry + count;
    s32 index = 0;
    while (entry != end) {
        if (entry->get() == controller) return index;
        ++entry;
        ++index;
    }
    return -1;
}

/* 0273C808 */
void sead::ControllerManager::allocateControllerBuffer(s32 capacity, void* heap) {
    mControllers.allocate(capacity, heap, 4);
}

/* 0273D084 */
void sead::ControllerWrapper::detach() {
    Controller* controller = mController;
    if (controller == NULL) return;
    u32 offset = controller->mWrappers.mMemberOffset;
    Link* node = reinterpret_cast<Link*>(reinterpret_cast<be<u8>*>(this) + offset);
    node->detach();
    controller->mWrappers.mCount = (u32)controller->mWrappers.mCount - 1;
    mController = NULL;
}

/* 0273D248 */
void sead::ControllerWrapper::attach(Controller* controller, u32 copyRepeat) {
    detach();
    u32 offset = controller->mWrappers.mMemberOffset;
    Link* node = reinterpret_cast<Link*>(reinterpret_cast<be<u8>*>(this) + offset);
    controller->mWrappers.insertBefore(node);
    controller->mWrappers.mCount = (u32)controller->mWrappers.mCount + 1;
    mController = controller;
    if (copyRepeat != 0) copyControllerRepeat(this, controller);
}

/* 0273D2C0 */
void sead::ControllerWrapper::setEnabled(u8 enabled) { mEnabled = enabled; }

/* 0273D394 */
void sead::BufferedController::appendSample(Controller* sample) {
    s32 ownIndex = mManager->indexOf(this);
    s32 otherIndex = sample->mManager->indexOf(sample);
    if (otherIndex < 0 || (ownIndex >= 0 && otherIndex >= ownIndex)) return;
    s32 count = mSamples.mCount;
    if (count >= (s32)mSamples.mCapacity) return;
    elementAt<gptr<Controller>>(u32(mSamples.mStorage.v), count) = sample;
    mSamples.mCount = (u32)mSamples.mCount + 1;
}

/* 0273D42C */
void sead::BufferedController::clearSamples() { mSamples.mCount = 0; }

/* 0273D69C */
u32 sead::BufferedController::isConnected() { return mConnected; }

/* 0273D7A0 */
u32 sead::ChannelController::isConnected() { return mConnected; }

/* 0273EAC0 */
static u32 terminalControllerIsConnected(sead::BufferedController* controller) {
    return controller->mConnected;
}

/* 0273D7A8 */
static void channelControllerUnused() {}

/* 0273ED50 */
static void terminalControllerUnused() {}

/* 0273ED54 */
static void terminalControllerPostCalcUnused() {}

/* 0273BA2C */
void sead::Controller::calc() {
    be<u32>* table = mTable.get();
    u32 previous = mButtons;
    u32 previousPointer = (u32)mPointerState & 1;
    gabi::call_ptr<void>(table[13],this);
    update(previous,previousPointer);
    u32 offset = mAddons.mMemberOffset;
    be<u8>* sentinel = reinterpret_cast<be<u8>*>(&mAddons) - offset;
    be<u8>* addon = reinterpret_cast<be<u8>*>(mAddons.mNext.get()) - offset;
    bool idle = true;
    while (addon != sentinel) {
        DeviceRecord* object = reinterpret_cast<DeviceRecord*>(addon);
        table = object->mTable.get();
        if (gabi::call_ptr<u32>(table[9],object) != 0) idle = false;
        Link* node = reinterpret_cast<Link*>(addon + offset);
        u32 currentOffset = mAddons.mMemberOffset;
        addon = reinterpret_cast<be<u8>*>(node->mNext.get()) - offset;
        sentinel = reinterpret_cast<be<u8>*>(&mAddons) - currentOffset;
    }
    if (idle) {
        table = mTable.get();
        idle = gabi::call_ptr<u32>(table[15],this) != 0;
    }
    u32 wrapperOffset = mWrappers.mMemberOffset;
    be<u8>* wrapper = reinterpret_cast<be<u8>*>(mWrappers.mNext.get()) - wrapperOffset;
    sentinel = reinterpret_cast<be<u8>*>(&mWrappers) - wrapperOffset;
    if (idle) mIdleFrames = (u32)mIdleFrames + 1;
    else mIdleFrames = 0;
    while (wrapper != sentinel) {
        ControllerWrapper* object = reinterpret_cast<ControllerWrapper*>(wrapper);
        table = object->mTable.get();
        gabi::call_ptr<void>(table[9],object,previous,previousPointer);
        Link* node = reinterpret_cast<Link*>(wrapper + wrapperOffset);
        wrapper = reinterpret_cast<be<u8>*>(node->mNext.get()) - wrapperOffset;
        sentinel = reinterpret_cast<be<u8>*>(&mWrappers) - (u32)mWrappers.mMemberOffset;
    }
}

/* WWHD glue
 data sControllerManager 101F8AE8
 ext 0273D1EC void copyControllerRepeat(sead::InputState*, const sead::InputState*)
*/
