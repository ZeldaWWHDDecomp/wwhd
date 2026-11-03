// WWHD manager construction, platform setup and frame dispatch from local instructions.
#include "sead/ControllerManager.h"
#include "sead/ElementAt.h"

static u32 sManagerTaskTable;
static u32 sManagerBaseTable;
static u32 sManagerName;
static sead::RuntimeType sInitializeType;
static u32 sInitializeTypeGuard;
static u32 sInitializeTypeTable;
void* allocateManagerObject(u32);
void* allocateManagerHeapObject(u32,void*,s32);
void constructManagerBase(void*,void*,const char*);
void destroyManagerBase(void*,u32);
void destroyManagerToken(be<u8>*,u32);
void deleteManagerObject(void*);
sead::RemoteDevice* createRemoteDevice(sead::RemoteDevice*,sead::ControllerManager*,void*);
sead::RemoteController* createRemoteController(sead::RemoteController*,sead::ControllerManager*,s32);
sead::TabletDevice* createTabletDevice(sead::TabletDevice*,sead::ControllerManager*);
sead::Controller* createTabletController(sead::Controller*,sead::ControllerManager*);
void* createRemoteMotor(void*,sead::Controller*);
void* createTabletMotor(void*,sead::Controller*);

static void appendController(sead::ControllerManager* manager, sead::Controller* controller) {
    s32 count = manager->mControllers.mCount;
    if (count >= (s32)manager->mControllers.mCapacity) return;
    sead::elementAt<gptr<sead::Controller>>(u32(manager->mControllers.mStorage.v), count) = controller;
    manager->mControllers.mCount = (u32)manager->mControllers.mCount + 1;
}

static void appendDevice(sead::ControllerManager* manager, void* device) {
    u32 offset = manager->mDevices.mMemberOffset;
    sead::Link* node = reinterpret_cast<sead::Link*>(static_cast<be<u8>*>(device) + offset);
    manager->mDevices.insertBefore(node);
    manager->mDevices.mCount = (u32)manager->mDevices.mCount + 1;
}

static void appendAddon(sead::Controller* controller, void* addon) {
    u32 offset = controller->mAddons.mMemberOffset;
    sead::Link* node = reinterpret_cast<sead::Link*>(static_cast<be<u8>*>(addon) + offset);
    controller->mAddons.insertBefore(node);
    controller->mAddons.mCount = (u32)controller->mAddons.mCount + 1;
}

/* 0273C734 */
static sead::ControllerManager* constructControllerManager(sead::ControllerManager* manager, void* argument) {
    if (manager == NULL) manager = static_cast<sead::ControllerManager*>(allocateManagerObject(sizeof(sead::ControllerManager)));
    if (manager == NULL) return NULL;
    constructManagerBase(manager, argument, reinterpret_cast<const char*>(&sManagerName));
    manager->mInstalled = 0;
    manager->mTaskTable = gabi::ea(&sManagerTaskTable);
    manager->mBaseTable = gabi::ea(&sManagerBaseTable);
    sead::OffsetList* list = &manager->mDevices;
    if (gabi::ea(list) == 0) list = static_cast<sead::OffsetList*>(allocateManagerObject(16));
    if (list != NULL) {
        list->mCount = 0;
        list->mPrevious = list;
        list->mMemberOffset = ~0u;
        list->mNext = list;
    }
    sead::PointerBuffer* buffer = &manager->mControllers;
    if (gabi::ea(buffer) == 0) buffer = static_cast<sead::PointerBuffer*>(allocateManagerObject(12));
    if (buffer != NULL) {
        buffer->mCapacity = 0;
        buffer->mCount = 0;
        buffer->mStorage = NULL;
    }
    manager->mDevices.mMemberOffset = 0;
    return manager;
}

/* 0273CCE8 */
static void destroyControllerManager(sead::ControllerManager* manager, u32 flags) {
    if (manager == NULL) return;
    manager->mBaseTable = gabi::ea(&sManagerBaseTable);
    destroyManagerToken(&manager->mInstalled, 2);
    destroyManagerBase(manager, 0);
    if (flags & 1) deleteManagerObject(manager);
}

/* 0273C70C */
static void destroyManagerObjectBase(be<u8>* object, u32 flags) {
    destroyControllerManager(reinterpret_cast<sead::ControllerManager*>(object - 20), flags);
}

/* 0273C814 */
void sead::ControllerManager::initializeDefaults(void* heap) {
    allocateControllerBuffer(6, heap);
    ChannelController* channel = static_cast<ChannelController*>(allocateManagerHeapObject(0x164,heap,4));
    if (channel != NULL) channel = constructChannelController(channel,this,0);
    appendController(this,channel);
    RemoteDevice* remote = static_cast<RemoteDevice*>(allocateManagerHeapObject(0x4D38,heap,4));
    if (remote != NULL) remote = createRemoteDevice(remote,this,heap);
    appendDevice(this,remote);
    for (u32 index = 0; index < 4; ++index) {
        RemoteController* controller = static_cast<RemoteController*>(allocateManagerHeapObject(0x164,heap,4));
        if (controller != NULL) controller = createRemoteController(controller,this,index);
        void* motor = allocateManagerHeapObject(0x20,heap,4);
        if (motor != NULL) motor = createRemoteMotor(motor,controller);
        appendAddon(controller,motor);
        appendController(this,controller);
    }
    TabletDevice* tablet = static_cast<TabletDevice*>(allocateManagerHeapObject(0xADC,heap,4));
    if (tablet != NULL) tablet = createTabletDevice(tablet,this);
    appendDevice(this,tablet);
    Controller* controller = static_cast<Controller*>(allocateManagerHeapObject(0x190,heap,4));
    if (controller != NULL) controller = createTabletController(controller,this);
    void* motor = allocateManagerHeapObject(0x40,heap,4);
    if (motor != NULL) motor = createTabletMotor(motor,controller);
    appendAddon(controller,motor);
    appendController(this,controller);
}

/* 0273CA64 */
void sead::ControllerManager::initialize() {
    ControllerInit* argument = static_cast<ControllerInit*>(mInitParameters.get());
    if (sInitializeTypeGuard == 0) {
        sInitializeTypeGuard = 1;
        sInitializeType.mTable = &sInitializeTypeTable;
    }
    if (argument != NULL && gabi::call_ptr<u32>(argument->mTable[3],argument,&sInitializeType) == 0) argument = NULL;
    if (argument == NULL) {
        initializeDefaults(NULL);
        return;
    }
    allocateControllerBuffer(argument->mCapacity,NULL);
    ControllerInitCallback* callback = argument->mCallback;
    if (callback != NULL) gabi::call_ptr<void>(callback->mTable[3],callback,this);
}

/* 0273CBD0 */
void sead::ControllerManager::calc() {
    u32 offset = mDevices.mMemberOffset;
    be<u8>* sentinel = reinterpret_cast<be<u8>*>(&mDevices) - offset;
    be<u8>* object = reinterpret_cast<be<u8>*>(mDevices.mNext.get()) - offset;
    while (object != sentinel) {
        DeviceRecord* device = reinterpret_cast<DeviceRecord*>(object);
        be<u32>* table = device->mTable.get();
        gabi::call_ptr<void>(table[9],device);
        Link* node = reinterpret_cast<Link*>(object + offset);
        u32 currentOffset = mDevices.mMemberOffset;
        object = reinterpret_cast<be<u8>*>(node->mNext.get()) - offset;
        sentinel = reinterpret_cast<be<u8>*>(&mDevices) - currentOffset;
    }
    gptr<Controller>* entry = reinterpret_cast<gptr<Controller>*>(mControllers.mStorage.get());
    gptr<Controller>* end = entry + (u32)mControllers.mCount;
    while (entry != end) {
        Controller* controller = *entry;
        be<u32>* table = controller->mTable.get();
        gabi::call_ptr<void>(table[9],controller);
        ++entry;
        u32 count = mControllers.mCount;
        gptr<Controller>* storage = reinterpret_cast<gptr<Controller>*>(mControllers.mStorage.get());
        end = storage + count;
    }
}

/* WWHD glue
 data sManagerTaskTable 10143184
 data sManagerBaseTable 1014323C
 data sManagerName 10143170
 data sInitializeType 101FD998
 data sInitializeTypeGuard 101FD658
 data sInitializeTypeTable 10143160
 ext 0273AD10 void* allocateManagerObject(u32)
 ext 0273B050 void* allocateManagerHeapObject(u32, void*, s32)
 ext 027454E0 void constructManagerBase(void*, void*, const char*)
 ext 027452C8 void destroyManagerBase(void*, u32)
 ext 0273C6E0 void destroyManagerToken(be<u8>*, u32)
 ext 0273AF40 void deleteManagerObject(void*)
 ext 0273D820 sead::RemoteDevice* createRemoteDevice(sead::RemoteDevice*,sead::ControllerManager*,void*)
 ext 0273D9E4 sead::RemoteController* createRemoteController(sead::RemoteController*,sead::ControllerManager*,s32)
 ext 0273E380 sead::TabletDevice* createTabletDevice(sead::TabletDevice*,sead::ControllerManager*)
 ext 0273E4B4 sead::Controller* createTabletController(sead::Controller*,sead::ControllerManager*)
 ext 0273E2A0 void* createRemoteMotor(void*,sead::Controller*)
 ext 0273EAC8 void* createTabletMotor(void*,sead::Controller*)
*/
