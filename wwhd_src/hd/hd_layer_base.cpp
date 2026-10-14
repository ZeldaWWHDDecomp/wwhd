/* HD render-layer base, projection choice and virtual draw dispatch. Private decomp source. */
#include "hd/hd_layer_base.h"
static hdLayerBaseHeaderZero_c sLayerBaseHeaderZero;
static hdLayerBaseHeaderDescriptor_c sLayerBaseHeaderDescriptor;
static hdLayerBaseHeaderDescriptor_c sLayerBaseRotationDescriptor;
static hdLayerBaseHeaderDescriptor_c sLayerBasePositiveRotationDescriptor;
static f32 sLayerBaseMinRotation;
static f32 sLayerBaseMaxRotation;
static u8 sLayerBaseRotationGuard;
static u8 sLayerBasePositiveRotationGuard;
static void initializeRotationGuard(void* guard);
static void initializePositiveRotationGuard(void* guard);
static u32 sLayerBaseLayerVtable;
static u32 sLayerBaseDrawVtable;
static u32 sLayerBaseDerivedDrawVtable;
static u32 sLayerBaseStringVtable;
static u32 sLayerBaseFixedStringVtable;
static u32 sLayerBaseNameVtable;
static hdLayerDrawManager_l* sLayerBaseDrawManager;
static hdLayerProjection_c sLayerBaseDefaultProjection;
static hdLayerMatrix_c sLayerBaseDefaultViewMatrix;
static void* sLayerBaseDebugEnabled;
static u8 sLayerBaseDebugRenderer;
static void* allocateLayerBase(u32 size);
static void deleteLayerBase(hdLayerBase_c* layer);
static void destroyLayerBase(hdLayerBase_c* layer, u32 flags);
static void constructAglLayer(hdLayerBase_c* layer);
static void destroyAglLayer(hdLayerBase_c* layer, s32 flags);
static void constructLayerViewport(hdLayerViewport_c* viewport);
static void destroyLayerViewport(hdLayerViewport_c* viewport, s32 flags);
static void setLayerViewport(hdLayerViewport_c* viewport, hdLayerProjectionMatrix_c* projection);
static hdLayerProjectionMatrix_c* getLayerProjectionMatrix(hdLayerProjection_c* projection);
static void copyLayerName(be<u8>* destination, be<u8>* source, s32 size, s32 flags);
static u32 getRenderThreadIndex();
static void constructLayerDrawContext(hdLayerDrawContext_c* context);
static void copyLayerProjection(hdLayerProjectionMatrix_c* source, hdLayerProjectionMatrix_c* target);
static void copyLayerView(hdLayerMatrix_c* source, hdLayerMatrix_c* target);
static hdLayerDebugRenderer_l* setupDebugRenderer(void* renderer);
/* 02733EF4 */
static s32 layerBaseNoDraw() { return 0; }
/* 02733EFC */
static s32 layerBaseNoCalc() { return 0; }
/* 02733FC8 */
static void destroyLayerBaseDerived(hdLayerBase_c* layer, u32 flags) {
    if (layer != NULL) {
        layer->mDrawVtable = gabi::ea(&sLayerBaseDerivedDrawVtable);
        // This destructor targets the common layer body, without deleting it.
        destroyLayerBase(layer, 0);
        if (flags & 1) deleteLayerBase(layer);
    }
}
/* 02734028 */
static s32 layerBaseDrawOrder() { return 2; }
/* 027340D8 */
static hdLayerBase_c* constructLayerBase(hdLayerBase_c* layer, hdLayerSafeString_l* name) {
    if (layer == NULL) {
        layer = (hdLayerBase_c*)allocateLayerBase(sizeof(hdLayerBase_c));
        if (layer == NULL) return NULL;
    }
    constructAglLayer(layer);
    layer->mpVtable.v = gabi::ea(&sLayerBaseLayerVtable);
    layer->mDrawVtable = gabi::ea(&sLayerBaseDrawVtable);
    hdLayerString_c* fixed = gabi::at<hdLayerString_c>(gabi::ea(layer)+offsetof(hdLayerBase_c, mName));
    if (fixed == NULL) fixed = (hdLayerString_c*)allocateLayerBase(sizeof(hdLayerString_c));
    if (fixed != NULL) {
        hdLayerString_c* string = fixed;
        if (string == NULL) string = (hdLayerString_c*)allocateLayerBase(12);
        if (string != NULL) {
            string->mpString = &fixed->mBuffer[0];
            string->mVtable = gabi::ea(&sLayerBaseStringVtable);
            string->mCapacity = 32;
            fixed->mBuffer[31] = 0;
        }
        be<u8>* buffer = fixed->mpString;
        fixed->mVtable = gabi::ea(&sLayerBaseFixedStringVtable);
        *buffer = 0;
        fixed->mVtable = gabi::ea(&sLayerBaseNameVtable);
    }
    constructLayerViewport(&layer->mViewport);
    u32 ensureTerminated = name->mpVtable->mEnsureTerminated;
    be<u8>* buffer = layer->mName.mpString;
    gabi::call_ptr(ensureTerminated, name);
    be<u8>* text = name->mpString;
    s32 length = 0;
    while (*text != 0) {
        ++length;
        ++text;
        if (length > 0x40000) { length = 0; break; }
    }
    s32 capacity = layer->mName.mCapacity;
    ensureTerminated = name->mpVtable->mEnsureTerminated;
    if (length >= capacity) length = capacity - 1;
    gabi::call_ptr(ensureTerminated, name);
    text = name->mpString;
    copyLayerName(buffer, text, length, 0);
    buffer[length] = 0;
    return layer;
}
/* 02734248 */
static void destroyLayerBase(hdLayerBase_c* layer, u32 flags) {
    if (layer != NULL) {
        layer->mDrawVtable = gabi::ea(&sLayerBaseDrawVtable);
        destroyLayerViewport(&layer->mViewport, 2);
        destroyAglLayer(layer, 0);
        if (flags & 1) deleteLayerBase(layer);
    }
}
/* 027342B4 */
void hdLayerBase_c::initialize() {
    u32 flags = mFlags;
    hdLayerVtable_c* table = mpVtable;
    mFlags = flags & ~0x80u;
    gabi::call_ptr(table->mInitialize, this);
}
/* 027342D0 */
void hdLayerBase_c::calculate() { gabi::call_ptr(mpVtable->mCalculate, this); }
/* 027342E0 */
void hdLayerBase_c::draw(hdLayerRenderInfo_c* info) {
    getRenderThreadIndex();
    if (sLayerBaseDebugEnabled != NULL) {
        hdLayerDrawContext_c context;
        constructLayerDrawContext(&context);
        hdLayerProjectionMatrix_c projection;
        hdLayerProjectionMatrix_c* source = getLayerProjectionMatrix(info->mpProjection);
        copyLayerProjection(source, &projection);
        hdLayerMatrix_c view;
        copyLayerView(info->mpViewMatrix, &view);
    }
    hdLayerDebugRenderer_l* renderer = setupDebugRenderer(&sLayerBaseDebugRenderer);
    renderer->mTarget = 3;
    renderer->mFlags = 0;
    gabi::call_ptr(mpVtable->mDraw, this, info);
}
/* 027343C4 */
void hdLayerBase_c::setNameIndirect(gptr<void>* name) {
    u32 target = mpVtable->mSetName;
    gabi::call_ptr(target, this, name->get());
}
/* 027343D8 */
void hdLayerBase_c::setResourceIndirect(gptr<void>* resource) {
    u32 target = mpVtable->mSetName;
    gabi::call_ptr(target, this, resource->get());
}
/* 027343EC */
void hdLayerBase_c::finishDraw() { gabi::call_ptr(mpVtable->mFinishDraw, this); }
/* 027343FC */
void hdLayerBase_c::prepareDraw() {
    hdLayerDrawManager_l* manager = sLayerBaseDrawManager;
    u32 flags = mFlags;
    mDrawContext = manager->mContext;
    hdLayerProjection_c* projection;
    if (flags & 0x1000u) projection = mpInternalProjection;
    else {
        projection = mpProjection;
        if (projection == NULL) projection = &sLayerBaseDefaultProjection;
    }
    hdLayerMatrix_c* view;
    if ((flags & 0x40u) && (flags & 0x80u)) view = &mViewMatrix;
    else {
        view = mpViewMatrix;
        if (view == NULL) view = &sLayerBaseDefaultViewMatrix;
    }
    hdLayerProjectionMatrix_c* matrix = getLayerProjectionMatrix(projection);
    setLayerViewport(&mViewport, matrix);
    for (u32 index = 0; index < 12; ++index) mDrawViewMatrix.mWords[index] = view->mWords[index];
}
/* 02734504 */
static void __sinit_hd_layer_base_cpp() {
    for (u32 i = 0; i < 4; ++i) sLayerBaseHeaderZero.mWords[i] = 0;
    __register_global_object(gabi::ea(&sLayerBaseHeaderDescriptor));
    sLayerBaseMinRotation = -3.1415927410125732f;
    sLayerBaseMaxRotation = 3.1415927410125732f;
    initializeRotationGuard(&sLayerBaseRotationGuard);
    __register_global_object(gabi::ea(&sLayerBaseRotationDescriptor));
    initializePositiveRotationGuard(&sLayerBasePositiveRotationGuard);
    __register_global_object(gabi::ea(&sLayerBasePositiveRotationDescriptor));
}
/* WWHD glue
 data sLayerBaseLayerVtable 101421A8 nocheck
 data sLayerBaseDrawVtable 10142250 nocheck
 data sLayerBaseDerivedDrawVtable 10142110 nocheck
 data sLayerBaseStringVtable 10142138 nocheck
 data sLayerBaseFixedStringVtable 10142160 nocheck
 data sLayerBaseNameVtable 10142178 nocheck
 data sLayerBaseHeaderZero 1049FA24
 data sLayerBaseHeaderDescriptor 101F8884
 data sLayerBaseRotationDescriptor 101F8890
 data sLayerBasePositiveRotationDescriptor 101F889C
 data sLayerBaseMinRotation 1049FA18
 data sLayerBaseMaxRotation 1049FA1C
 data sLayerBaseRotationGuard 1049FA20
 data sLayerBasePositiveRotationGuard 1049FA21
 ext 028ED6F8 void initializeRotationGuard(void* guard)
 ext 028EAB2C void initializePositiveRotationGuard(void* guard)
 data sLayerBaseDrawManager 1018C404
 data sLayerBaseDefaultProjection 104A20FC
 data sLayerBaseDefaultViewMatrix 104A2098
 data sLayerBaseDebugEnabled 101F8DCC
 data sLayerBaseDebugRenderer 104B45C0
 ext 0273AD10 void* allocateLayerBase(u32 size)
 ext 0273AF40 void deleteLayerBase(hdLayerBase_c* layer)
 ext 027C3280 void constructAglLayer(hdLayerBase_c* layer)
 ext 027C33C0 void destroyAglLayer(hdLayerBase_c* layer, s32 flags)
 ext 02873D70 void constructLayerViewport(hdLayerViewport_c* viewport)
 ext 02873FD0 void destroyLayerViewport(hdLayerViewport_c* viewport, s32 flags)
 ext 02873FE4 void setLayerViewport(hdLayerViewport_c* viewport, hdLayerProjectionMatrix_c* projection)
 ext 0274D83C hdLayerProjectionMatrix_c* getLayerProjectionMatrix(hdLayerProjection_c* projection)
 ext C0009988 void copyLayerName(be<u8>* destination, be<u8>* source, s32 size, s32 flags)
 ext C0009BB0 u32 getRenderThreadIndex()
 ext 0274F358 void constructLayerDrawContext(hdLayerDrawContext_c* context)
 ext 028E8970 void copyLayerProjection(hdLayerProjectionMatrix_c* source, hdLayerProjectionMatrix_c* target)
 ext 028E90D4 void copyLayerView(hdLayerMatrix_c* source, hdLayerMatrix_c* target)
 ext 027F29D4 hdLayerDebugRenderer_l* setupDebugRenderer(void* renderer)
*/
