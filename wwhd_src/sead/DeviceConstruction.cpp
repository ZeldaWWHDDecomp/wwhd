// WWHD platform controllers and motor addons, reconstructed from local instructions.
#include "sead/DeviceConstruction.h"

static sead::InputPair sInvalidTouchPoint;
static u32 sRemoteDeviceTable;
static u32 sTabletControllerTable;
static u32 sRemoteMotorTable;
static u32 sTabletMotorTable;
void* allocatePlatformObject(u32);
void constructPlatformArray(void*,u32,u32,u32);
sead::RemoteBatch* initializeRemoteBatch(sead::RemoteBatch*);
void initializeRemoteLibrary(void*,u32);
u32 remoteMotionWorkspaceSize();
void* allocateRemoteWorkspace(u32,void*,s32);
void installRemoteWorkspace(void*);
void constructMotorBase(void*,sead::Controller*);
void initializeMotorDeadline(void*);

template <class T>
static T* platformStorage(T* object,u32 size) {
    if (gabi::ea(object) == 0) object = static_cast<T*>(allocatePlatformObject(size));
    return object;
}

/* 0273D820 */
static sead::RemoteDevice* constructRemoteDevice(sead::RemoteDevice* device,sead::ControllerManager* manager,void* heap) {
    device = platformStorage(device,sizeof(sead::RemoteDevice));
    if (device == NULL) return NULL;
    device->mManager = manager;
    device->mType = 0;
    device->mLinks.mPrevious = NULL;
    device->mLinks.mNext = NULL;
    device->mTable = &sRemoteDeviceTable;
    sead::RemoteBatch* batches = platformStorage(device->mBatches,sizeof(device->mBatches));
    if (batches != NULL) constructPlatformArray(batches,4,sizeof(sead::RemoteBatch),
        gabi::guest_fn(reinterpret_cast<const void*>(&initializeRemoteBatch)));
    device->mType = 7;
    initializeRemoteLibrary(device->mMotionWorkspace,64);
    u32 size = remoteMotionWorkspaceSize();
    void* workspace = allocateRemoteWorkspace(size,heap,4);
    installRemoteWorkspace(workspace);
    return device;
}

/* 0273EA4C */
static TouchPoint* constructTouchPoint(TouchPoint* point) {
    point = platformStorage(point,sizeof(TouchPoint));
    if (point == NULL) return NULL;
    point->mPosition.mX = (u32)sInvalidTouchPoint.mX;
    point->mPosition.mY = (u32)sInvalidTouchPoint.mY;
    be<u32>* flags = platformStorage(&point->mFlags,4);
    if (flags != NULL) *flags = 0;
    return point;
}

/* 0273E4B4 */
static TabletController* constructTabletController(TabletController* controller,sead::ControllerManager* manager) {
    controller = platformStorage(controller,sizeof(TabletController));
    if (controller == NULL) return NULL;
    constructController(controller,manager);
    controller->mConnected = 0;
    controller->mTouchIndex = 0;
    controller->mTouchCapacity = 3;
    controller->mTable = &sTabletControllerTable;
    controller->mTouchCount = 0;
    constructPlatformArray(controller->mTouchPoints,3,sizeof(TouchPoint),
        gabi::guest_fn(reinterpret_cast<const void*>(&constructTouchPoint)));
    controller->mType = 8;
    return controller;
}

/* 0273E2A0 */
static void* constructRemoteMotor(PadMotor* motor,sead::Controller* controller) {
    motor = platformStorage(motor,32);
    if (motor == NULL) return NULL;
    constructMotorBase(motor,controller);
    motor->mTable = &sRemoteMotorTable;
    return motor;
}

/* 0273EAC8 */
static PadMotor* constructTabletMotor(PadMotor* motor,sead::Controller* controller) {
    motor = platformStorage(motor,sizeof(PadMotor));
    if (motor == NULL) return NULL;
    constructMotorBase(motor,controller);
    motor->mLength = 0;
    motor->mTable = &sTabletMotorTable;
    initializeMotorDeadline(motor->mDeadline);
    return motor;
}

/* 0273E54C */
void TouchPoint::update(u32 touching,f32 x,f32 y) {
    u32 flags = mFlags;
    u32 previous = flags & 1;
    if (touching != 0) flags |= 1;
    else flags &= ~1u;
    if (previous == 0 && touching != 0) flags = (flags | 2) & ~4u;
    else {
        flags &= ~2u;
        if (previous != 0 && touching == 0) flags |= 4;
        else flags &= ~4u;
    }
    mFlags = flags;
    if (touching == 0) return;
    reinterpret_cast<be<f32>&>(mPosition.mY) = y;
    reinterpret_cast<be<f32>&>(mPosition.mX) = x;
}

/* WWHD glue
 data sInvalidTouchPoint 1049FFA0
 data sRemoteDeviceTable 10143410
 data sTabletControllerTable 10143590
 data sRemoteMotorTable 101434C4
 data sTabletMotorTable 101435E8
 ext 0273AD10 void* allocatePlatformObject(u32)
 ext 028EFFD0 void constructPlatformArray(void*,u32,u32,u32)
 ext 0273D7AC sead::RemoteBatch* initializeRemoteBatch(sead::RemoteBatch*) callback
 ext C0008138 void initializeRemoteLibrary(void*,u32)
 ext C00080E8 u32 remoteMotionWorkspaceSize()
 ext 0273B0D4 void* allocateRemoteWorkspace(u32,void*,s32)
 ext C0008270 void installRemoteWorkspace(void*)
 ext 02767588 void constructMotorBase(void*,sead::Controller*)
 ext 02760DE8 void initializeMotorDeadline(void*)
*/
