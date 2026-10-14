/* HD render-command lists follow the active 3D layer and its projection. Private decomp source. */
#include "hd/hd_render_commands.h"
static hdRenderCommandsHeaderZero_c sRenderCommandsHeaderZero;
static hdRenderCommandsHeaderDescriptor_c sRenderCommandsHeaderDescriptor;
static hdRenderCommandsHeaderDescriptor_c sRenderCommandsRotationDescriptor;
static hdRenderCommandsHeaderDescriptor_c sRenderCommandsPositiveRotationDescriptor;
static f32 sRenderCommandsMinRotation;
static f32 sRenderCommandsMaxRotation;
static u8 sRenderCommandsRotationGuard;
static u8 sRenderCommandsPositiveRotationGuard;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static u32 sRenderCommandsManagerVtable;
static hdRenderCommandMgr_c* sRenderCommandsCommandManager;
static hdRenderCommandMgr_c* sRenderCommandsCommandDisposer;
static hdRenderSceneMgr_l* sRenderCommandsSceneManager;
static hdLayerMatrix_c sRenderCommandsDefaultCommandView;
static hdLayerProjection_c sRenderCommandsDefaultCommandProjection;
static u32 sRenderCommandsPerspectiveGuard;
static u32 sRenderCommandsPerspectiveType;
static u32 sRenderCommandsPerspectiveTypeSource;
static void* allocateCommandManager(u32 size);
static void* allocateCommandDisposer(u32 size, void* heap, u32 alignment);
static void constructCommandDisposer(hdRenderCommandMgr_c* manager, void* heap, u32 mode);
static void destroyCommandDisposer(hdRenderCommandMgr_c* manager, s32 flags);
static void deleteCommandManager(hdRenderCommandMgr_c* manager);
static void destroyCommandManagerBody(hdRenderCommandMgr_c* manager, s32 flags);
static void beginCommandPacket(hdRenderPacket_l* packet);
static void configureCommandPacket(hdRenderPacket_l* packet, u32 target, hdLayerMatrix_c* view, void* projection, void* frustum, f32 nearPlane, f32 farPlane, f32 fieldOfView, f32 aspect);
static void finishCommandPacket(hdRenderPacket_l* packet, u32 target);
static void* getCommandProjectionMatrix(hdRenderPerspective_l* projection);
static void enqueueCommandPacket(hdRenderPacket_l* packet, u32 index, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g);
static void enqueueExtraPacket(hdRenderPacket_l* packet, u32 index, u32 a, u32 b, u32 c, u32 d, u32 e);
/* inlined into render-command forwarding */
static hdRenderCommandLayer_l* activeCommandLayer(hdRenderSceneMgr_l* manager) {
    u32 count = manager->mLayerCount;
    gptr<hdRenderCommandLayer_l>* layers = manager->mpLayers;
    return layers[count > 1 ? 1 : 0];
}
/* 02738158 */
static s32 renderCommandsNoAction() { return 0; }
/* 02738228 */
static s32 renderCommandsTypeCount() { return 4; }
/* 027382D4 */
static BOOL renderCommandsSupportsTarget(hdRenderCommandMgr_c*, u32 target) { return target == 1; }
/* 02738308 */
static hdRenderCommandMgr_c* constructRenderCommands(hdRenderCommandMgr_c* manager) {
    if (manager == NULL) {
        manager = (hdRenderCommandMgr_c*)allocateCommandManager(sizeof(hdRenderCommandMgr_c));
        if (manager == NULL) return NULL;
    }
    if (u32(gabi::ea(manager)+offsetof(hdRenderCommandMgr_c, mDrawCount)) == 0) allocateCommandManager(16);
    manager->mDrawCount = 0;
    manager->mExtraDrawCount = 0;
    manager->mOverlayCount = 0;
    manager->mReservedCount = 0;
    return manager;
}
/* 02738370 */
static hdRenderCommandMgr_c* createRenderCommands(void* heap) {
    hdRenderCommandMgr_c* manager = sRenderCommandsCommandManager;
    if (manager == NULL) {
        manager = (hdRenderCommandMgr_c*)allocateCommandDisposer(sizeof(hdRenderCommandMgr_c), heap, 4);
        if (manager != NULL) {
            constructCommandDisposer(manager, heap, 3);
            manager->mVtable = gabi::ea(&sRenderCommandsManagerVtable);
        }
        sRenderCommandsCommandDisposer = manager;
        if (manager != NULL) manager = constructRenderCommands(manager);
        sRenderCommandsCommandManager = manager;
    }
    return manager;
}
/* 0273841C */
void hdRenderCommandMgr_c::clear() {
    mDrawCount = 0;
    mExtraDrawCount = 0;
    mOverlayCount = 0;
    mReservedCount = 0;
}
/* 02738438 */
void hdRenderCommandMgr_c::initialize() {
    hdRenderSceneMgr_l* manager = sRenderCommandsSceneManager;
    if (manager == NULL) return;
    hdRenderCommandLayer_l* layer = activeCommandLayer(manager);
    u32 flags = layer->mBase.mFlags;
    hdLayerMatrix_c* view;
    if ((flags & 0x40u) && (flags & 0x80u)) view = &layer->mBase.mViewMatrix;
    else {
        view = layer->mBase.mpViewMatrix;
        if (view == NULL) view = &sRenderCommandsDefaultCommandView;
    }
    hdRenderPerspective_l* projection;
    if (flags & 0x1000u) projection = (hdRenderPerspective_l*)layer->mBase.mpInternalProjection.get();
    else {
        projection = (hdRenderPerspective_l*)layer->mBase.mpProjection.get();
        if (projection == NULL) projection = (hdRenderPerspective_l*)&sRenderCommandsDefaultCommandProjection;
    }
    if (sRenderCommandsPerspectiveGuard == 0) {
        sRenderCommandsPerspectiveGuard = 1;
        sRenderCommandsPerspectiveType = gabi::ea(&sRenderCommandsPerspectiveTypeSource);
    }
    if (projection != NULL) {
        u32 target = projection->mpVtable->mIsKindOf;
        BOOL matches = gabi::call_ptr<BOOL>(target, projection, &sRenderCommandsPerspectiveType);
        if (!matches) projection = NULL;
    }
    flags = layer->mBase.mFlags;
    if ((flags & 0x40u) && (flags & 0x80u)) {
        hdLayerMatrix_c* internalView = &layer->mBase.mViewMatrix;
        if (gabi::ea(internalView) == 0) return;
    }
    if (layer->mBase.mpProjection.get() == NULL) return;
    hdRenderCommandList_l* list = layer->mpDrawList;
    u32 target = list->mpVtable->mSetCount;
    u32 count = mDrawCount;
    gabi::call_ptr(target, list, count);
    list = layer->mpExtraDrawList;
    target = list->mpVtable->mSetCount;
    count = mExtraDrawCount;
    gabi::call_ptr(target, list, count);
    list = layer->mpOverlayList;
    target = list->mpVtable->mSetCount;
    count = mOverlayCount;
    gabi::call_ptr(target, list, count);
    hdRenderPacket_l* packet = &layer->mPacket;
    beginCommandPacket(packet);
    void* matrix = getCommandProjectionMatrix(projection);
    f32 fieldOfView = projection->mFieldOfView;
    f32 farPlane = projection->mFarPlane;
    f32 aspect = projection->mAspect;
    f32 nearPlane = projection->mNearPlane;
    configureCommandPacket(packet, 0, view, matrix, &projection->mFrustum, nearPlane, farPlane, fieldOfView, aspect);
    finishCommandPacket(packet, 0);
}
/* 0273862C */
void hdRenderCommandMgr_c::enqueueDraw(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g) {
    hdRenderCommandLayer_l* layer = activeCommandLayer(sRenderCommandsSceneManager);
    if (layer != NULL) {
        enqueueCommandPacket(&layer->mPacket, mDrawCount, a, b, c, d, e, f, g);
        mDrawCount = mDrawCount + 1;
    }
}
/* 027386D4 */
void hdRenderCommandMgr_c::enqueueExtra(u32 a, u32 b, u32 c, u32 d, u32 e) {
    hdRenderCommandLayer_l* layer = activeCommandLayer(sRenderCommandsSceneManager);
    if (layer != NULL) {
        enqueueExtraPacket(&layer->mPacket, mExtraDrawCount, a, b, c, d, e);
        mExtraDrawCount = mExtraDrawCount + 1;
    }
}
/* 02738768 */
static void destroyRenderCommands(hdRenderCommandMgr_c* manager, u32 flags) {
    if (manager != NULL) {
        manager->mVtable = gabi::ea(&sRenderCommandsManagerVtable);
        if (manager == sRenderCommandsCommandDisposer) {
            hdRenderCommandMgr_c* body = sRenderCommandsCommandManager;
            sRenderCommandsCommandDisposer = NULL;
            destroyCommandManagerBody(body, 2);
            sRenderCommandsCommandManager = NULL;
        }
        destroyCommandDisposer(manager, 0);
        if (flags & 1) deleteCommandManager(manager);
    }
}
/* 02738804 */
static void __sinit_hd_render_commands_cpp() {
    for (u32 i = 0; i < 4; ++i) sRenderCommandsHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sRenderCommandsHeaderDescriptor));
    sRenderCommandsMinRotation = -3.1415927410125732f;
    sRenderCommandsMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sRenderCommandsRotationGuard);
    __register_global_object(gabi::ea(&sRenderCommandsRotationDescriptor));
    initializePositiveRotationGuard(&sRenderCommandsPositiveRotationGuard);
    __register_global_object(gabi::ea(&sRenderCommandsPositiveRotationDescriptor));
}
/* WWHD glue
 data sRenderCommandsManagerVtable 10142BA0 nocheck
 data sRenderCommandsHeaderZero 1049FADC
 data sRenderCommandsHeaderDescriptor 101F89FC
 data sRenderCommandsRotationDescriptor 101F8A08
 data sRenderCommandsPositiveRotationDescriptor 101F8A14
 data sRenderCommandsMinRotation 1049FAD0
 data sRenderCommandsMaxRotation 1049FAD4
 data sRenderCommandsRotationGuard 1049FAD8
 data sRenderCommandsPositiveRotationGuard 1049FAD9
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 data sRenderCommandsCommandManager 101F8A20
 data sRenderCommandsCommandDisposer 101F8A24
 data sRenderCommandsSceneManager 101F95D0
 data sRenderCommandsDefaultCommandView 104A2098
 data sRenderCommandsDefaultCommandProjection 104A20FC
 data sRenderCommandsPerspectiveGuard 101FD9F0
 data sRenderCommandsPerspectiveType 101FDCD4
 data sRenderCommandsPerspectiveTypeSource 10142B88
 ext 0273AD10 void* allocateCommandManager(u32 size)
 ext 0273B0D4 void* allocateCommandDisposer(u32 size, void* heap, u32 alignment)
 ext 02752B0C void constructCommandDisposer(hdRenderCommandMgr_c* manager, void* heap, u32 mode)
 ext 02752BEC void destroyCommandDisposer(hdRenderCommandMgr_c* manager, s32 flags)
 ext 0273AF40 void deleteCommandManager(hdRenderCommandMgr_c* manager)
 ext 02738404 void destroyCommandManagerBody(hdRenderCommandMgr_c* manager, s32 flags)
 ext 027AF91C void beginCommandPacket(hdRenderPacket_l* packet)
 ext 027B00D0 void configureCommandPacket(hdRenderPacket_l* packet, u32 target, hdLayerMatrix_c* view, void* projection, void* frustum, f32 nearPlane, f32 farPlane, f32 fieldOfView, f32 aspect)
 ext 027B0158 void finishCommandPacket(hdRenderPacket_l* packet, u32 target)
 ext 0274D83C void* getCommandProjectionMatrix(hdRenderPerspective_l* projection)
 ext 027B1548 void enqueueCommandPacket(hdRenderPacket_l* packet, u32 index, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g)
 ext 027B1734 void enqueueExtraPacket(hdRenderPacket_l* packet, u32 index, u32 a, u32 b, u32 c, u32 d, u32 e)
*/
