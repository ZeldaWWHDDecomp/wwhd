// WWHD controller object setup and deleting destructors, reconstructed from local instructions.
#include "sead/ControllerConstruction.h"

static u32 sControllerTable;
static u32 sWrapperTable;
static u32 sWrapperBaseTable;
static u32 sBufferedControllerTable;
static u32 sChannelControllerTable;
static u32 sWrapperMappingTable;
static u32 sWrapperMappingBaseTable;

template <class T>
static T* controllerStorage(T* object, u32 size) {
    if (gabi::ea(object) == 0) object = static_cast<T*>(allocateControllerObject(size));
    return object;
}

static void initializeControllerList(sead::OffsetList* list) {
    list = controllerStorage(list, 16);
    if (list == NULL) return;
    list->mCount = 0;
    list->mPrevious = list;
    list->mMemberOffset = ~0u;
    list->mNext = list;
}

/* 0273B94C */
sead::Controller* constructController(sead::Controller* controller, sead::ControllerManager* manager) {
    controller = controllerStorage(controller, sizeof(sead::Controller));
    if (controller == NULL) return NULL;
    constructInputState(controller, 28, 20, 24, 15);
    controller->mManager = manager;
    controller->mType = 0;
    controller->mTable = &sControllerTable;
    initializeControllerList(&controller->mAddons);
    initializeControllerList(&controller->mWrappers);
    controller->mAddons.mMemberOffset = 0;
    controller->mWrappers.mMemberOffset = 0x148;
    return controller;
}

/* 0273D15C */
sead::ControllerWrapper* constructControllerWrapper(sead::ControllerWrapper* wrapper) {
    wrapper = controllerStorage(wrapper, sizeof(sead::ControllerWrapper));
    if (wrapper == NULL) return NULL;
    constructInputState(wrapper, 28, -1, -1, 15);
    constructWrapperObject(wrapper->mObjectHeader);
    wrapper->mTable = &sWrapperTable;
    wrapper->mBaseTable = gabi::ea(&sWrapperBaseTable);
    wrapper->mEnabled = 1;
    wrapper->mSampleCount = 0;
    wrapper->mController = NULL;
    wrapper->mSampleIndex = 0;
    return wrapper;
}

/* 0273D2D0 */
sead::BufferedController* constructBufferedController(sead::BufferedController* controller,
                                                    sead::ControllerManager* manager, s32 capacity, void* heap) {
    controller = controllerStorage(controller, sizeof(sead::BufferedController));
    if (controller == NULL) return NULL;
    constructController(controller, manager);
    controller->mConnected = 0;
    controller->mTable = &sBufferedControllerTable;
    sead::PointerBuffer* buffer = controllerStorage(&controller->mSamples, 12);
    if (buffer != NULL) {
        buffer->mCapacity = 0;
        buffer->mCount = 0;
        buffer->mStorage = NULL;
    }
    controller->mType = 9;
    controller->mSamples.allocate(capacity, heap, 4);
    return controller;
}

/* 0273D6A4 */
sead::ChannelController* constructChannelController(sead::ChannelController* controller,
                                                   sead::ControllerManager* manager, s32 channel) {
    controller = controllerStorage(controller, sizeof(sead::ChannelController));
    if (controller == NULL) return NULL;
    constructController(controller, manager);
    controller->mChannel = channel;
    controller->mConnected = 0;
    controller->mTable = &sChannelControllerTable;
    controller->mType = 5;
    return controller;
}

/* 0273D0E0 */
static void destroyControllerWrapper(sead::ControllerWrapper* wrapper, u32 flags) {
    if (wrapper == NULL) return;
    wrapper->mTable = &sWrapperTable;
    wrapper->mBaseTable = gabi::ea(&sWrapperBaseTable);
    wrapper->detach();
    destroyWrapperObject(wrapper->mObjectHeader, 0);
    if (flags & 1) deleteControllerObject(wrapper);
}

/* 0273D154 */
static void destroyWrapperObjectEntry(be<u8>* object, u32 flags) {
    destroyControllerWrapper(reinterpret_cast<sead::ControllerWrapper*>(object - 0x130), flags);
}

/* 0273D024 */
static void destroyMappingWrapper(sead::ControllerWrapper* wrapper, u32 flags) {
    if (wrapper == NULL) return;
    wrapper->mBaseTable = gabi::ea(&sWrapperMappingBaseTable);
    destroyControllerWrapper(wrapper, 0);
    if (flags & 1) deleteControllerObject(wrapper);
}

/* 0273CDCC */
static void destroyMappingObjectEntry(be<u8>* object, u32 flags) {
    destroyMappingWrapper(reinterpret_cast<sead::ControllerWrapper*>(object - 0x130), flags);
}

/* WWHD glue
 data sControllerTable 101430E0
 data sWrapperTable 101432D0
 data sWrapperBaseTable 10143308
 data sBufferedControllerTable 1014333C
 data sChannelControllerTable 101433A8
 data sWrapperMappingTable 10143278
 data sWrapperMappingBaseTable 101432B0
 ext 0273AD10 void* allocateControllerObject(u32)
 ext 02752A84 void constructWrapperObject(void*)
 ext 02752BEC void destroyWrapperObject(void*, u32)
 ext 0273AF40 void deleteControllerObject(void*)
*/
