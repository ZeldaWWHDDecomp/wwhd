/** Graphics task setup and per-display rendering. */
#include "hd/hd_render_projection.h"
#include "hd/hd_render_commands.h"
#include "hd/hd_graphics_task.h"
#include "hd/hd_render_layers.h"
static hdGraphicsTask_c* sGraphicsActiveTask;
static hdRenderLayerRegistry_l* sGraphicsRenderer;
static hdGraphicsMode_l* sGraphicsMode;
static void* sGraphicsProfiler;
static hdGraphicsDisplayOwner_l* getGraphicsDisplayOwner(hdGraphicsTask_c *);
static cXy* getGraphicsScreenSize(u32);
static void enableGraphicsLayer(hdLayerBase_c *, u32);
static void finishGraphicsDisplay(hdGraphicsDisplayOwner_l *);
static void setGraphicsColorMask(u32, u32, s32);
static void beginGraphicsProfile(void *);
static void endGraphicsProfile(void *);
static void graphicsRenderCallback(hdGraphicsTask_c *, hdLayerRenderInfo_c *);
static void graphicsEmptyCallback();
static u8 sGraphicsTaskVtable;
static u8 sGraphicsTaskInterface;
static u8 sGraphicsTaskCallbackVtable;
static u8 sGraphicsNameVtable;
static u8 sGraphicsServiceVtable;
static u8 sGraphicsServiceDisposerVtable;
static hdGraphicsService_c* sGraphicsService;
static hdGraphicsDisposer_l* sGraphicsServiceDisposer;
static void constructGraphicsTask(hdGraphicsTask_c *, void *, const char *);
static void constructGraphicsRenderBuffers(void *, u32, u32, u32);
static void constructGraphicsRenderBuffer(void *);
static cXy* getGraphicsRenderSize();
static void constructGraphicsService(hdGraphicsService_c *);
static void* allocateGraphicsService(u32, void *, s32);
static void constructGraphicsDisposer(hdGraphicsDisposer_l *, void *, u32);
/* 0272923C */
static hdGraphicsService_c* hdGraphicsService_construct(hdGraphicsService_c* self) {
    if (self == NULL) {
        self = (hdGraphicsService_c *)operator_new(sizeof(hdGraphicsService_c));
        if (self == NULL)
            return NULL;
    }
    constructGraphicsService(self);
    self->mVtable = gabi::ea(&sGraphicsServiceVtable);
    return self;
}
/* 02729290 */
hdGraphicsService_c* hdGraphicsService_c::create(void* heap) {
    if (sGraphicsService == NULL) {
        hdGraphicsService_c* service =
            (hdGraphicsService_c *)allocateGraphicsService(sizeof(hdGraphicsService_c), heap, 4);
        hdGraphicsDisposer_l* disposer = &service->mDisposer;
        if (disposer != NULL) {
            constructGraphicsDisposer(disposer, heap, 3);
            disposer->mVtable = gabi::ea(&sGraphicsServiceDisposerVtable);
        }
        sGraphicsServiceDisposer = disposer;
        if (service != NULL)
            service = hdGraphicsService_construct(service);
        sGraphicsService = service;
    }
    return sGraphicsService;
}
/* 027294A8 */
static hdGraphicsTask_c* hdGraphicsTask_construct(hdGraphicsTask_c* self, void* heap) {
    if (self == NULL) {
        self = (hdGraphicsTask_c*)operator_new(sizeof(hdGraphicsTask_c));
        if (self == NULL)
            return NULL;
    }
    constructGraphicsTask(self, heap, "GraphicsTask");
    // Preserve 32-bit guest wrapping: an embedded member may start at zero even for a nonnull task.
    // Such members allocate separately, while the surrounding fields still belong to the task.
    self->mVtable = gabi::ea(&sGraphicsTaskVtable);
    self->mInterfaceVtable = gabi::ea(&sGraphicsTaskInterface);
    self->mCallbackVtable = gabi::ea(&sGraphicsTaskCallbackVtable);
    self->mRegistered = 0;
    be<u32>* flags = gabi::at<be<u32>>(gabi::ea(self) + offsetof(hdGraphicsTask_c, mFlags));
    if (flags == NULL)
        flags = (be<u32>*)operator_new(sizeof(be<u32>));
    if (flags != NULL)
        *flags = 2;
    self->mDisplayMode = 4;
    self->mScreenSize = *getGraphicsScreenSize(0);
    self->mRenderSize = *getGraphicsRenderSize();
    void* buffers = gabi::at<void>(gabi::ea(self) + offsetof(hdGraphicsTask_c, mRenderBuffers));
    if (buffers == NULL)
        buffers = operator_new(sizeof(self->mRenderBuffers));
    if (buffers != NULL)
        constructGraphicsRenderBuffers(buffers, 2, 0xA70,
                                       gabi::guest_fn((const void*)constructGraphicsRenderBuffer));
    self->mHistoryTiming = 0;
    self->mAdaptive = 1;
    self->mHighThreshold = 200.0f;
    self->mLowThreshold = 125.0f;
    cXy* requested = gabi::at<cXy>(gabi::ea(self) + offsetof(hdGraphicsTask_c, mRequestedSize));
    if (requested == NULL)
        requested = (cXy*)operator_new(sizeof(cXy));
    if (requested != NULL) {
        requested->x = 1280.0f;
        requested->y = 720.0f;
    }
    self->mMode = 0;
    self->mScaled = 0;
    self->mFlag = 0;
    self->mCooldown = 4;
    self->mValues[3] = 0.0f;
    self->mPresent = 1;
    self->mValues[2] = 0.0f;
    self->mValues[0] = 0.0f;
    self->mCooldownReset = 4;
    self->mValues[1] = 0.0f;
    SafeString* name = gabi::at<SafeString>(gabi::ea(self) + offsetof(hdGraphicsTask_c, mName));
    if (name == NULL)
        name = (SafeString*)operator_new(sizeof(SafeString));
    if (name != NULL) {
        name->__vtbl = gabi::ea(&sGraphicsNameVtable);
        name->mStringTop = gabi::guest_addr("");
    }
    return self;
}
static u32 sGraphicsDisplayTypeReady;
struct GraphicsDisplayType {
    be<u32> mName;
};
static GraphicsDisplayType sGraphicsDisplayType;
static void configureGraphicsDisplay(void *, u32);
/* 02729688 */
void hdGraphicsTask_c::configureBuffers() {
    hdGraphicsRendererFlags_l* renderer =
        (hdGraphicsRendererFlags_l *)(hdRenderLayerRegistry_l *)sGraphicsRenderer;
    renderer->mFormat = (u32(mFlags) & 2) ? 0x22 : 0x1D;
    renderer->mFlags = u16(renderer->mFlags) | 0x10;
    hdGraphicsDisplayOwner_l* owner = getGraphicsDisplayOwner(this);
    u32 flags = (u32(mFlags) & 2) ? 0x41A : 0x1A;
    for (u32 i = 0; i < 2; ++i) {
        hdGraphicsDisplayType_l* display = (hdGraphicsDisplayType_l *)(void *)owner->mpDisplays[i];
        if (!sGraphicsDisplayTypeReady) {
            sGraphicsDisplayType.mName = gabi::guest_addr("");
            sGraphicsDisplayTypeReady = 1;
        }
        if (display != NULL && !gabi::call_ptr<u32>(display->mpVtable->mIsKindOf.get(),
                                                    display,
                                                    &sGraphicsDisplayType))
            display = NULL;
        configureGraphicsDisplay(display, flags);
    }
}
static u8 sGraphicsBaseCallbackVtable;
static u8 sGraphicsBaseInterfaceVtable;
static u8 sGraphicsListenerDisposerVtable;
static u8 sGraphicsReferenceVtable;
static void destroyGraphicsRenderBuffer(void *, u32);
static void destroyGraphicsArray(void *, u32, u32, u32, u32, u32);
static void destroyGraphicsRegistration(void *, u32);
static void clearGraphicsListener(void *);
static void destroyGraphicsDisposer(void *, u32);
static void destroyGraphicsTaskBase(void *, u32);
static void releaseGraphicsReference(void *);
/* 02729330 */
void hdGraphicsReference_c::destroy(u32 flags) {
    if (this != NULL) {
        mDisposer.mVtable = gabi::ea(&sGraphicsReferenceVtable);
        if (mOwnsReference) {
            hdGraphicsReferenceBlock_l* block = mpBlock;
            if (block != NULL) {
                mpBlock = NULL;
                releaseGraphicsReference(mAllocator);
                block->mReferences = u32(block->mReferences) - 1;
            }
            mOwnsReference = 0;
        }
        destroyGraphicsDisposer(&mDisposer, 0);
        if (flags & 1)
            operator_delete(this);
    }
}
/* 0272B0D4 */
void hdGraphicsTask_c::destroy(u32 flags) {
    if (this != NULL) {
        hdGraphicsTaskBase_l* base = (hdGraphicsTaskBase_l *)this;
        base->mInterfaceVtable = gabi::ea(&sGraphicsTaskInterface);
        base->mCallbackVtable = gabi::ea(&sGraphicsTaskCallbackVtable);
        destroyGraphicsArray(mRenderBuffers,
                             2,
                             0xA70,
                             gabi::guest_fn((const void *)destroyGraphicsRenderBuffer),
                             0,
                             0);
        destroyGraphicsRegistration(&mRegistered, 2);
        base->mListeners[2].mVtable = gabi::ea(&sGraphicsBaseCallbackVtable);
        base->mInterfaceVtable = gabi::ea(&sGraphicsBaseInterfaceVtable);
        for (s32 i = 2; i >= 0; --i) {
            base->mListeners[i].mDisposer.mVtable = gabi::ea(&sGraphicsListenerDisposerVtable);
            clearGraphicsListener(&base->mListeners[i]);
            destroyGraphicsDisposer(&base->mListeners[i].mDisposer, 0);
            if (i != 0)
                base->mListeners[i - 1].mVtable = gabi::ea(&sGraphicsBaseCallbackVtable);
        }
        destroyGraphicsTaskBase(this, 0);
        if (flags & 1)
            operator_delete(this);
    }
}
/* 02729234 */
static void hdGraphicsTask_destroyInterface(void* self, u32 flags) {
    ((hdGraphicsTask_c *)((u8 *)self - 0x14))->destroy(flags);
}
static void destroyGraphicsServiceBase(void *, u32);
/* 02729200 */
static void hdGraphicsService_destroyInterface(void* self, u32 flags) {
    destroyGraphicsServiceBase((u8 *)self - 0x1C, flags);
}
/* 02729488 */
void hdGraphicsTask_c::registerTask() {
    if (sGraphicsActiveTask == NULL) {
        sGraphicsActiveTask = this;
        mRegistered = 1;
    }
}
/* 0272A010 */
void hdGraphicsTask_c::installRenderCallback() {
    getGraphicsDisplayOwner(this)->mRenderCallback =
        gabi::guest_fn((const void *)graphicsEmptyCallback);
}
/* 0272A03C */
void hdGraphicsTask_c::updateScreenSize() {
    u32 mode = sGraphicsMode->mDisplayMode;
    hdRenderLayerRegistry_l* renderer = sGraphicsRenderer;
    mScreenSize = *getGraphicsScreenSize(mode == 2);
    for (u32 i = 0; i < 14; ++i) {
        hdLayerBase_c* layer = renderer->layer(i);
        if (i < 7)
            enableGraphicsLayer(layer, mode == 2);
        else if (i < 11)
            enableGraphicsLayer(layer, mode != 2);
    }
}
/* 0272A104 */
void hdGraphicsTask_c::enableScene(u32 enabled) {
    for (u32 i = 0; i < 4; ++i) {
        hdLayerBase_c* layer = (hdLayerBase_c *)sGraphicsRenderer->layer(i);
        if (layer != NULL)
            layer->mFlags = enabled ? u32(layer->mFlags) | 1 : u32(layer->mFlags) & ~1u;
    }
}
/* 0272A178 */
void hdGraphicsTask_c::enableDRC(u32 enabled) {
    hdLayerBase_c* layer = (hdLayerBase_c *)sGraphicsRenderer->layer(7);
    if (layer != NULL)
        layer->mFlags = enabled ? u32(layer->mFlags) | 1 : u32(layer->mFlags) & ~1u;
}
/* 0272AEE0 */
void hdGraphicsTask_c::finish() {
    hdGraphicsDisplayOwner_l* owner = getGraphicsDisplayOwner(this);
    if (owner != NULL)
        finishGraphicsDisplay(owner);
}
/* 0272AF0C */
void hdGraphicsTask_c::renderCallback(hdLayerRenderInfo_c* info) {
    switch (u32(filterRenderInfo(info)->mPass)) {
    case 0:
        setGraphicsColorMask(15, 0, -1);
        beginGraphicsProfile(sGraphicsProfiler);
        break;
    case 1:
        setGraphicsColorMask(15, 0, -1);
        break;
    case 3:
        endGraphicsProfile(sGraphicsProfiler);
        break;
    }
}
/* 0272AFB8 */
static void __sinit_hd_graphics_task_cpp() {
    sinit_header_statics_rodata(0x1049F938, 0x101F86BC, 0x1049F944, 0x1014138C);
}

struct hdGraphicsDisplay_l {
    u8 mStart[12];
    gptr<void> mpColorBuffer;
    u8 mBeforeSize[0x728];
    cXy mSize;
    u8 mBeforeFlags[0x50];
    be<u32> mFlags;
    u8 mBeforeMode[0x20];
    be<u32> mMode;
    be<f32> mClearColor[4];
    u8 mTail[0x38];
};
WWHD_SIZE(hdGraphicsDisplay_l, 0x800);
struct hdGraphicsRenderer_l {
    u8 mReserved[0x18];
    hdGraphicsDisplay_l mDisplays[2];
};
struct hdGraphicsTiming_l {
    u8 mReserved[0x14];
    be<f32> mElapsed;
};
struct hdGraphicsProfiler_l {
    u8 mReserved[0x28];
    be<f32> mElapsed;
    u8 mPadding[4];
    gptr<hdGraphicsTiming_l> mpHistory;
    be<s32> mCapacity, mCursor;
    be<u32> mCount;
};
struct hdGraphicsScale_l {
    u8 mReserved[0x18];
    be<u8> mTVEnabled, mDRCEnabled;
    u8 mBeforeScale[10];
    be<f32> mScale;
    u8 mBeforeMode[0x338];
    be<u8> mMode;
    u8 mBeforeBuffers[0x323];
    gptr<void> mpBuffers[2];
};
WWHD_OFFSET(hdGraphicsScale_l, mMode, 0x360);
WWHD_OFFSET(hdGraphicsScale_l, mpBuffers, 0x684);
static hdGraphicsScale_l* sGraphicsScale;
static cXy sGraphicsDefaultSize;
static void* sGraphicsDevice;
static u32 queryGraphicsDevice(void *, u32);
static void updateGraphicsScale(hdGraphicsScale_l *);
static void synchronizeGraphicsScale();
static void beginGraphicsTiming(void *);
static void prepareGraphicsRenderer(hdRenderLayerRegistry_l *, u32);
/* 0272A1C4 */
void hdGraphicsTask_c::updateResolution() {
    u32 mode = sGraphicsMode->mDisplayMode;
    hdGraphicsRenderer_l* renderer =
        (hdGraphicsRenderer_l *)(hdRenderLayerRegistry_l *)sGraphicsRenderer;
    if (u32(mDisplayMode) != mode) {
        updateScreenSize();
        renderer->mDisplays[0].mMode = mode == 2 ? 1 : 3;
        renderer->mDisplays[1].mMode = mode == 2 ? 3 : 1;
    }
    void* device = sGraphicsDevice;
    enableScene(!queryGraphicsDevice(device, 7));
    enableDRC(!queryGraphicsDevice(device, 8));
    mScaled = f32(mRenderSize.x) == f32(mRequestedSize.x);
    if (mode == 2 || !mAdaptive)
        mRenderSize = sGraphicsDefaultSize;
    else {
        hdGraphicsProfiler_l* profiler = (hdGraphicsProfiler_l *)(void *)sGraphicsProfiler;
        if (profiler != NULL) {
            f32 timing;
            if (profiler->mCount) {
                s32 index = profiler->mCursor;
                if (index >= s32(profiler->mCapacity))
                    index -= s32(profiler->mCapacity);
                timing = profiler->mpHistory[index].mElapsed;
            } else
                timing = profiler->mElapsed;
            if (!mHistoryTiming)
                timing = profiler->mElapsed;
            if (f32(mRenderSize.x) == f32(sGraphicsDefaultSize.x)) {
                if (timing > f32(mHighThreshold)) {
                    mRenderSize = mRequestedSize;
                    mCooldown = 4;
                    profiler->mCursor = 0;
                    profiler->mCount = 0;
                }
            } else if (timing < f32(mLowThreshold)) {
                mCooldown = u32(mCooldown) - 1;
                if (!mCooldown) {
                    mRenderSize = sGraphicsDefaultSize;
                    profiler->mCount = 0;
                    profiler->mCursor = 0;
                }
            } else
                mCooldown = mCooldownReset;
        }
    }
    hdGraphicsScale_l* scale = sGraphicsScale;
    f32 factor = 1.0f;
    cXy tvSize;
    cXy drcSize;
    if (scale != NULL && (scale->mTVEnabled || scale->mDRCEnabled)) {
        tvSize = *getGraphicsRenderSize();
        drcSize = *getGraphicsScreenSize(0);
        if (scale->mTVEnabled || scale->mDRCEnabled)
            factor = scale->mScale;
    } else {
        tvSize = mRenderSize;
        drcSize = mScreenSize;
    }
    renderer->mDisplays[0].mSize.x = f32(tvSize.x) * factor;
    renderer->mDisplays[0].mSize.y = f32(tvSize.y) * factor;
    renderer->mDisplays[0].mFlags = u32(renderer->mDisplays[0].mFlags) | 0x200;
    renderer->mDisplays[1].mSize.x = f32(drcSize.x) * factor;
    renderer->mDisplays[1].mSize.y = f32(drcSize.y) * factor;
    renderer->mDisplays[1].mFlags = u32(renderer->mDisplays[1].mFlags) | 0x200;
    mDisplayMode = mode;
}
/* 0272A5F0 */
void hdGraphicsTask_c::beginFrame() {
    hdGraphicsScale_l* scale = sGraphicsScale;
    hdRenderLayerRegistry_l* renderer = sGraphicsRenderer;
    if (scale != NULL) {
        updateGraphicsScale(scale);
        scale->mMode = (u32(mFlags) >> 1) & 1;
        if (scale->mTVEnabled || scale->mDRCEnabled)
            synchronizeGraphicsScale();
    }
    beginGraphicsTiming(sGraphicsProfiler);
    updateResolution();
    prepareGraphicsRenderer(renderer, 1);
    if (scale != NULL) {
        hdGraphicsRenderer_l* displays = (hdGraphicsRenderer_l *)renderer;
        scale->mpBuffers[0] = displays->mDisplays[0].mpColorBuffer;
        scale->mpBuffers[1] = displays->mDisplays[1].mpColorBuffer;
    }
    mFlags = u32(mFlags) | 1;
}

struct hdGraphicsBuffer_l {
    u8 mReserved[8];
    be<f32> mBounds[4];
};
struct hdGraphicsDisplayFrame_l {
    u8 mReserved[0x51C];
    u8 mBuffer[0x274];
    be<u32> mFlags;
};
struct hdGraphicsDrawOwner_l {
    u8 mReserved[0x98];
    be<f32> mTVBounds[4];
    u8 mPadding[12];
    be<f32> mDRCBounds[4];
    u8 mBeforeBuffers[0x2A4];
    gptr<void> mpBuffers[2];
};
WWHD_OFFSET(hdGraphicsDrawOwner_l, mpBuffers, 0x368);
struct hdGraphicsDrawMode_l {
    u8 mReserved[0x80];
    be<u32> mModes[6];
};
static hdGraphicsDrawMode_l sGraphicsDrawMode;
static void* sGraphicsBufferManager;
static u32 isGraphicsDisplayEnabled(hdRenderLayerRegistry_l *, u32);
static void activateGraphicsBuffer(void *);
static void makeGraphicsViewport(void *, void *);
static void activateGraphicsViewport(void *, void *);
static void registerGraphicsBuffer(void *, void *, void *, u32);
static void finishGraphicsTV(hdGraphicsScale_l *);
static void finishGraphicsDRC(hdGraphicsScale_l *);
struct hdGraphicsViewport_l {
    u8 mStorage[0x18];
};
/* 0272A6B4 */
static void prepareGraphicsTV(hdGraphicsTask_c* self) {
    if (!(u32(self->mFlags) & 1))
        return;
    for (u32 i = 0; i < 3; ++i) {
        sGraphicsDrawMode.mModes[i * 2] = 3;
        sGraphicsDrawMode.mModes[i * 2 + 1] = 0;
    }
    hdRenderLayerRegistry_l* renderer = sGraphicsRenderer;
    u32 enabled = isGraphicsDisplayEnabled(renderer, 0);
    hdGraphicsDrawOwner_l* owner = (hdGraphicsDrawOwner_l *)getGraphicsDisplayOwner(self);
    hdGraphicsRenderer_l* displays = (hdGraphicsRenderer_l *)renderer;
    hdGraphicsBuffer_l* buffer = (hdGraphicsBuffer_l *)(void *)displays->mDisplays[0].mpColorBuffer;
    if (buffer != NULL) {
        if (enabled)
            owner->mpBuffers[0] = buffer;
        activateGraphicsBuffer(buffer);
        hdGraphicsViewport_l viewport;
        makeGraphicsViewport(&viewport, buffer);
        activateGraphicsViewport(&viewport, buffer);
        registerGraphicsBuffer(sGraphicsBufferManager, buffer, &viewport, 3);
    }
    if (sGraphicsScale != NULL)
        finishGraphicsTV(sGraphicsScale);
    if (buffer != NULL) {
        for (u32 i = 0; i < 4; ++i)
            owner->mTVBounds[i] = buffer->mBounds[i];
        hdGraphicsBuffer_l* drc =
            (hdGraphicsBuffer_l *)(void *)displays->mDisplays[1].mpColorBuffer;
        if (drc != NULL)
            for (u32 i = 0; i < 4; ++i)
                owner->mDRCBounds[i] = drc->mBounds[i];
    }
}
/* 0272A808 */
static void prepareGraphicsDRC(hdGraphicsTask_c* self) {
    if (!(u32(self->mFlags) & 1))
        return;
    hdRenderLayerRegistry_l* renderer = sGraphicsRenderer;
    hdGraphicsDrawOwner_l* owner = (hdGraphicsDrawOwner_l *)getGraphicsDisplayOwner(self);
    u32 enabled = isGraphicsDisplayEnabled(renderer, 1);
    hdGraphicsRenderer_l* displays = (hdGraphicsRenderer_l *)renderer;
    hdGraphicsDisplayFrame_l* display = (hdGraphicsDisplayFrame_l *)&displays->mDisplays[1];
    void* buffer = (u32(display->mFlags) & 0x400) ? display->mBuffer : NULL;
    if (buffer != NULL) {
        if (enabled)
            owner->mpBuffers[1] = buffer;
        activateGraphicsBuffer(buffer);
        hdGraphicsViewport_l viewport;
        makeGraphicsViewport(&viewport, buffer);
        activateGraphicsViewport(&viewport, buffer);
    }
    if (sGraphicsScale != NULL)
        finishGraphicsDRC(sGraphicsScale);
}

struct hdGraphicsCaptureVtable_l {
    u8 mReserved[0x24];
    be<u32> mGetCount;
};
struct hdGraphicsCapture_l {
    gptr<hdGraphicsCaptureVtable_l> mpVtable;
};
struct hdGraphicsStringVtable_l {
    u8 mReserved[0x14];
    be<u32> mAssureTermination;
};
static u32 sGraphicsSubmissionFlags;
static u8 sGraphicsSubmitting;
static void* sGraphicsCaptureDevice;
static void submitGraphicsDisplay(hdRenderLayerRegistry_l *, u32);
static hdGraphicsCapture_l* getGraphicsCapture(hdRenderLayerRegistry_l *, u32);
static void queueGraphicsCapture(
    hdGraphicsService_c *, const char *, hdGraphicsCapture_l *, be<u32> *, be<u32> *, be<u32> *);
static void processGraphicsCaptures(hdGraphicsService_c *);
static void finishGraphicsCaptures(hdGraphicsService_c *);
static void beginGraphicsDeviceCapture(void *);
static void endGraphicsDeviceCapture(void *);
static void flushGraphicsCommands();
/* 0272AD80 */
static void submitGraphicsFrame(hdGraphicsTask_c *) {
    hdRenderLayerRegistry_l* renderer = sGraphicsRenderer;
    sGraphicsSubmitting = 1;
    sGraphicsSubmissionFlags |= 2;
    submitGraphicsDisplay(renderer, 0);
    submitGraphicsDisplay(renderer, 1);
    hdGraphicsService_c* service = sGraphicsService;
    if (service != NULL) {
        SafeString name;
        name.mStringTop = gabi::guest_addr("RenderDisplay::calcGPU");
        name.__vtbl = gabi::ea(&sGraphicsNameVtable);
        hdGraphicsCapture_l* capture = getGraphicsCapture(renderer, 2);
        if (gabi::call_ptr<u32>(capture->mpVtable->mGetCount.get(), capture) > 0) {
            hdGraphicsStringVtable_l* vtable = gabi::at<hdGraphicsStringVtable_l>(name.__vtbl);
            gabi::call_ptr(vtable->mAssureTermination.get(), &name);
            gabi::Local<be<u32>> flags;
            gabi::Local<be<u32>> width;
            gabi::Local<be<u32>> height;
            *flags.get() = 5;
            *width.get() = 0;
            *height.get() = 0;
            queueGraphicsCapture(service,
                                 gabi::at<char>(name.mStringTop),
                                 capture,
                                 flags.get(),
                                 width.get(),
                                 height.get());
        }
        processGraphicsCaptures(service);
        finishGraphicsCaptures(service);
    }
    void* device = sGraphicsCaptureDevice;
    if (device != NULL) {
        beginGraphicsDeviceCapture(device);
        device = sGraphicsCaptureDevice;
    }
    sGraphicsSubmitting = 0;
    endGraphicsDeviceCapture(device);
    flushGraphicsCommands();
    sGraphicsSubmissionFlags &= ~2u;
}

struct hdGraphicsTaskState_l;
struct hdGraphicsTaskLink_l {
    u8 mReserved[0x10];
    gptr<hdGraphicsTaskState_l> mpParent;
};
struct hdGraphicsTaskState_l {
    gptr<hdGraphicsTaskLink_l> mpLink;
    u8 mReserved[0x40];
    be<u32> mState;
};
struct hdGraphicsTaskVtable_l {
    u8 mReserved[0xA4];
    be<u32> mGetState;
};
struct hdGraphicsTaskStateOwner_l {
    u8 mReserved[0x70];
    gptr<hdGraphicsTaskVtable_l> mpVtable;
};
struct hdGraphicsDrawState_l {
    u8 mReserved[0x38];
    Mtx34 mCamera;
    u8 mBeforeActive[0x30];
    be<u8> mActive;
    u8 mPadding[3];
    hdProjection_l mProjection;
    u8 mBeforeMatrix[4];
    be<u32> mProjectionMatrix[16];
};
static hdGraphicsDrawState_l sGraphicsDrawState;
static Mtx34 sGraphicsDefaultCamera;
static hdProjection_l sGraphicsDefaultProjection;
struct hdGraphicsParticleParams_l {
    gptr<Mtx34> mpCamera;
    be<u32> mProjection[16];
    be<f32> mFov, mAspect;
    be<u32> mFlags;
};
struct hdGraphicsParticleContext_l {
    u8 mReserved[0xD0];
    be<u8> mActive;
    u8 mBeforeContext[7];
    u8 mContext[0x44];
    u8 mPrimary[0x6C];
    u8 mSecondary[0x6C];
};
struct hdGraphicsParticleBuffers_l {
    u8 mReserved[0x4164];
    u8 mPrimary[0x20];
    u8 mSecondary[0x20];
};
struct hdGraphicsRendererBounds_l {
    u8 mReserved[0x75C];
    be<f32> mWidth, mHeight;
};
static u32 sGraphicsPerspectiveTypeReady;
struct hdGraphicsPerspectiveType_l {
    be<u32> mName;
};
static hdGraphicsPerspectiveType_l sGraphicsPerspectiveType;
static void* getGraphicsLayerContext(hdLayerBase_c *);
static void prepareGraphicsLayer(hdLayerBase_c *, void *);
static void finishGraphicsLayer(hdLayerBase_c *);
static void* getGraphicsProjectionMatrix(hdProjection_l *);
static void copyGraphicsProjectionMatrix(void *, void *);
static void setGraphicsProjectionBounds(hdProjection_l *, f32, f32, f32, f32);
static void setGraphicsDeviceContext(void *, u32, void *);
static void clearGraphicsDeviceContext(void *, u32);
static hdLayerPlay_l* getGraphicsPlay();
static void prepareGraphicsParticles(void *, hdGraphicsParticleParams_l *);
static void copyGraphicsParticleBuffer(void *, void *);
static void prepareGraphicsParticleContext(void *, hdGraphicsParticleContext_l *);
static void finishGraphicsParticleContext(hdGraphicsParticleContext_l *);
static void renderGraphicsDisplay(hdRenderLayerRegistry_l *, u32);
static void finishGraphicsDisplayCapture(hdRenderLayerRegistry_l *, u32);
/* 0272A8C4 */
static void renderGraphicsFrame(hdGraphicsTask_c* self) {
    hdRenderLayerRegistry_l* renderer = sGraphicsRenderer;
    hdGraphicsTaskStateOwner_l* owner = (hdGraphicsTaskStateOwner_l *)self;
    hdGraphicsTaskState_l* state =
        gabi::call_ptr<hdGraphicsTaskState_l *>(owner->mpVtable->mGetState.get(), self, 1u);
    if (u32(state->mState) == 1) {
        prepareGraphicsRenderer(renderer, 1);
        return;
    }
    state = state->mpLink != NULL ? state->mpLink->mpParent : NULL;
    while (state != NULL) {
        if (u32(state->mState) == 2) {
            prepareGraphicsRenderer(renderer, 1);
            return;
        }
        state = state->mpLink != NULL ? state->mpLink->mpParent : NULL;
    }
    hdLayerBase_c* layer = (hdLayerBase_c *)renderer->layer(1);
    u32 flags = layer->mFlags;
    Mtx34* camera;
    if ((flags & 0xC0) == 0xC0)
        camera = (Mtx34 *)&layer->mViewMatrix;
    else
        camera = layer->mpViewMatrix != NULL ? reinterpret_cast<Mtx34*>(layer->mpViewMatrix.get()) : &sGraphicsDefaultCamera;
    hdProjection_l* projection;
    if (flags & 0x1000)
        projection = reinterpret_cast<hdProjection_l*>(layer->mpInternalProjection.get());
    else
        projection = layer->mpProjection != NULL ? reinterpret_cast<hdProjection_l*>(layer->mpProjection.get())
                                                 : &sGraphicsDefaultProjection;
    if (camera != NULL && projection != NULL) {
        for (u32 i = 0; i < 3; ++i)
            for (u32 j = 0; j < 4; ++j)
                sGraphicsDrawState.mCamera.m[i][j] = camera->m[i][j];
        copyGraphicsProjectionMatrix(getGraphicsProjectionMatrix(projection),
                                     sGraphicsDrawState.mProjectionMatrix);
        hdGraphicsRendererBounds_l* bounds =
            (hdGraphicsRendererBounds_l *)(hdRenderLayerRegistry_l *)sGraphicsRenderer;
        setGraphicsProjectionBounds(
            &sGraphicsDrawState.mProjection, 0.0f, bounds->mHeight, 0.0f, bounds->mWidth);
        sGraphicsDrawState.mProjection.mDirty = 1;
        sGraphicsDrawState.mProjection.mNear = 0.0f;
        sGraphicsDrawState.mProjection.mFar = 1.0f;
    }
    void* context = getGraphicsLayerContext(layer);
    prepareGraphicsLayer(layer, (u8 *)context + 0x10);
    if (u32(sGraphicsActiveTask->mMode) <= 2) {
        context = getGraphicsLayerContext(layer);
        setGraphicsDeviceContext(sGraphicsDevice, 14, (u8 *)context + 0x10);
    }
    if (camera != NULL && projection != NULL) {
        hdGraphicsParticleContext_l* particles =
            (hdGraphicsParticleContext_l *)(void *)sGraphicsCaptureDevice;
        particles->mActive = 1;
        if (!sGraphicsPerspectiveTypeReady) {
            sGraphicsPerspectiveTypeReady = 1;
            sGraphicsPerspectiveType.mName = gabi::guest_addr("");
        }
        hdRenderProjection_c* perspective = (hdRenderProjection_c *)projection;
        hdRenderPerspective_l* perspectiveBase = (hdRenderPerspective_l *)projection;
        if (gabi::call_ptr<u32>(
                perspectiveBase->mpVtable->mIsKindOf.get(), projection, &sGraphicsPerspectiveType)) {
            hdGraphicsParticleParams_l params;
            params.mFlags = 0;
            params.mpCamera = camera;
            params.mFov = perspective->mFieldOfView;
            params.mAspect = perspectiveBase->mAspect;
            copyGraphicsProjectionMatrix(getGraphicsProjectionMatrix(projection),
                                         params.mProjection);
            if (getGraphicsPlay()->mpParticles != NULL) {
                prepareGraphicsParticles(getGraphicsPlay()->mpParticles, &params);
                particles = (hdGraphicsParticleContext_l *)(void *)sGraphicsCaptureDevice;
                hdGraphicsParticleBuffers_l* buffers =
                    (hdGraphicsParticleBuffers_l *)(void *)getGraphicsPlay()->mpParticles;
                copyGraphicsParticleBuffer(particles->mPrimary, buffers->mPrimary);
                particles = (hdGraphicsParticleContext_l *)(void *)sGraphicsCaptureDevice;
                buffers = (hdGraphicsParticleBuffers_l *)(void *)getGraphicsPlay()->mpParticles;
                copyGraphicsParticleBuffer(particles->mSecondary, buffers->mSecondary);
                particles = (hdGraphicsParticleContext_l *)(void *)sGraphicsCaptureDevice;
                prepareGraphicsParticleContext(particles->mContext, particles);
            }
        }
        particles = (hdGraphicsParticleContext_l *)(void *)sGraphicsCaptureDevice;
        particles->mActive = 0;
        finishGraphicsParticleContext(
            (hdGraphicsParticleContext_l *)(void *)sGraphicsCaptureDevice);
    }
    renderGraphicsDisplay(renderer, 0);
    renderGraphicsDisplay(renderer, 1);
    hdGraphicsService_c* service = sGraphicsService;
    sGraphicsDrawState.mActive = 1;
    if (service != NULL) {
        SafeString name;
        name.mStringTop = gabi::guest_addr("RenderDisplay::draw");
        name.__vtbl = gabi::ea(&sGraphicsNameVtable);
        hdGraphicsCapture_l* capture = getGraphicsCapture(renderer, 0);
        if (gabi::call_ptr<u32>(capture->mpVtable->mGetCount.get(), capture) > 0) {
            hdGraphicsStringVtable_l* vtable = gabi::at<hdGraphicsStringVtable_l>(name.__vtbl);
            gabi::call_ptr(vtable->mAssureTermination.get(), &name);
            gabi::Local<be<u32>> options;
            gabi::Local<be<u32>> width;
            gabi::Local<be<u32>> height;
            *options.get() = 5;
            *width.get() = 0;
            *height.get() = 0;
            queueGraphicsCapture(service,
                                 gabi::at<char>(name.mStringTop),
                                 capture,
                                 options.get(),
                                 width.get(),
                                 height.get());
        }
        processGraphicsCaptures(service);
        finishGraphicsCaptures(service);
        finishGraphicsDisplayCapture(renderer, 0);
        finishGraphicsDisplayCapture(renderer, 1);
    }
    finishGraphicsLayer(layer);
    clearGraphicsDeviceContext(sGraphicsDevice, 14);
    sGraphicsDrawState.mActive = 0;
}

struct hdGraphicsHeapVtable_l {
    u8 mReserved[0x2C];
    be<u32> mAdjust;
};
struct hdGraphicsHeap_l {
    u8 mReserved[12];
    gptr<hdGraphicsHeapVtable_l> mpVtable;
    u8 mBeforeParent[0x14];
    gptr<hdGraphicsHeap_l> mpParent;
};
struct hdGraphicsCreateParams_l {
    u8 mReserved[0x20];
    be<u32> mLayerCount, mFlags, mDisplayCount;
    u8 mTail[12];
};
struct hdGraphicsIntSize_l {
    be<s32> mWidth, mHeight;
};
struct hdGraphicsRendererConfig_l {
    u8 mReserved[0x1078];
    be<u16> mFlags;
    be<u8> mEnabled;
    u8 mBeforeAsset[0x1C5];
    gptr<void> mpAsset;
};
struct hdGraphicsAssetList_l {
    u8 mReserved[0xDC];
    be<u32> mCount;
    u8 mPad[4];
    gptr<gptr<void>> mpAssets;
};
struct hdGraphicsLayerPriority_l {
    u8 mReserved[0x5C];
    be<u32> mPriority;
};
/* 0x20 bytes: constructor 02759814 allocates 0x20 when this is NULL and writes +0x18 and +0x1C */
struct hdGraphicsServiceInit_l {
    u8 mStorage[0x20];
};
struct hdGraphicsServiceVtable_l {
    u8 mReserved[0x1C];
    be<u32> mInitialize;
};
struct hdGraphicsProfilerLimit_l {
    u8 mReserved[0x2C];
    be<f32> mLimit;
};
static hdGraphicsAssetList_l* sGraphicsAssets;
static void* sGraphicsContext;
static void* sGraphicsHeapManager;
static void* sGraphicsSupport;
struct hdGraphicsResourceList_l {
    u8 mReserved[0x10];
    be<u32> mCount;
    gptr<void> mpEntries;
};
static hdGraphicsResourceList_l* sGraphicsResourceList;
static hdShaderBackend_c* sGraphicsShaders;
static hdRenderResources_c* sGraphicsResources;
static u8 sGraphicsListenerDisposerTable;
static u8 sGraphicsListenerDelegateTable;
static u8 sGraphicsListenerTable;

static void* getGraphicsRendererHeap();
static void* getGraphicsSupportHeap();
static hdGraphicsHeap_l* createGraphicsHeap(u32, SafeString *, void *, u32, u32);
static hdGraphicsHeap_l* createGraphicsShaderHeap(u32, SafeString *, void *, u32, u32);
static void constructGraphicsCreateParams(hdGraphicsCreateParams_l *);
static u32 getGraphicsTVWidth();
static u32 getGraphicsTVHeight();
static u32 getGraphicsDRCWidth();
static u32 getGraphicsDRCHeight();
static void setGraphicsCreateSize(hdGraphicsCreateParams_l *,
                                  u32,
                                  hdGraphicsIntSize_l *,
                                  hdGraphicsIntSize_l *);
static hdRenderLayerRegistry_l* createGraphicsRenderer(void *);
static void
initializeGraphicsRenderer(hdRenderLayerRegistry_l *, hdGraphicsCreateParams_l *, void *, u32);
static void constructGraphicsListener(hdFadeListener_l *);
static void attachGraphicsListener(hdRenderLayerRegistry_l *, u32, hdFadeListener_l *);
static void beginGraphicsContext(void *);
static void endGraphicsContext(void *);
static void attachGraphicsShadow(hdLayerBase_c *, hdLayerBase_c *);
static void createGraphicsProfiler(u32);
static void initializeGraphicsProfiler(void *, void *);
static void initializeGraphicsRenderBuffer(void *, void *);
static void createGraphicsSupport(void *);
static void* switchGraphicsHeap(void *, void *);
static void constructGraphicsServiceParams(hdGraphicsServiceInit_l *);
static void createGraphicsCaptureDevice(u32);
static void initializeGraphicsCaptureDevice(void *, u32, void *);
static void createGraphicsSupportService(u32);
static void initializeGraphicsSupportService(void *, void *);
static void createGraphicsResourceService(u32);
static void initializeGraphicsResourceService(hdGraphicsResourceList_l *);
static void createGraphicsShaderService(void *);
static void* getGraphicsShaderArchive();
static void initializeGraphicsShaderArchive(void *, void *, u32);
static void initializeGraphicsDrawMode(hdGraphicsDrawMode_l *);
static void completeGraphicsTask(hdGraphicsTask_c *);
void initializeBackendShaders(hdShaderBackend_c *, void *);
static void adjustGraphicsHeap(hdGraphicsHeap_l* heap) {
    gabi::call_ptr(heap->mpVtable->mAdjust.get(), heap);
}
static u32 enterGraphicsHeap(void* heap) {
    return heap != NULL ? gabi::ea(switchGraphicsHeap(sGraphicsHeapManager, heap)) : 1;
}
static void leaveGraphicsHeap(u32 previous) {
    if (previous != 1)
        switchGraphicsHeap(sGraphicsHeapManager, gabi::at<void>(previous));
}
static void setGraphicsLayerName(SafeString* name, const char* text) {
    name->mStringTop = gabi::guest_addr(text);
    name->__vtbl = gabi::ea(&sGraphicsNameVtable);
}
/* 027297C0 */
static void initializeGraphicsTask(hdGraphicsTask_c* self) {
    void* rootHeap = getGraphicsRendererHeap();
    SafeString rendererName;
    setGraphicsLayerName(&rendererName, "agl::lyr::Renderer");
    hdGraphicsHeap_l* heap = createGraphicsHeap(0, &rendererName, rootHeap, 1, 0);
    hdGraphicsCreateParams_l params;
    constructGraphicsCreateParams(&params);
    params.mFlags = 0x400000;
    hdGraphicsIntSize_l renderSize;
    hdGraphicsIntSize_l screenSize;
    renderSize.mWidth = gabi::ftoi(self->mRenderSize.x);
    renderSize.mHeight = gabi::ftoi(self->mRenderSize.y);
    screenSize.mWidth = getGraphicsTVWidth();
    screenSize.mHeight = getGraphicsTVHeight();
    setGraphicsCreateSize(&params, 0, &renderSize, &screenSize);
    screenSize.mWidth = gabi::ftoi(self->mScreenSize.x);
    screenSize.mHeight = gabi::ftoi(self->mScreenSize.y);
    renderSize.mWidth = getGraphicsDRCWidth();
    renderSize.mHeight = getGraphicsDRCHeight();
    setGraphicsCreateSize(&params, 1, &screenSize, &renderSize);
    params.mDisplayCount = 2;
    params.mLayerCount = 14;
    hdRenderLayerRegistry_l* renderer = createGraphicsRenderer(heap);
    initializeGraphicsRenderer(renderer, &params, heap, 0);
    hdGraphicsRendererConfig_l* config = (hdGraphicsRendererConfig_l *)renderer;
    u16 rendererFlags = u16(config->mFlags) & ~0x20u;
    config->mEnabled = 1;
    config->mFlags = rendererFlags;
    if (!(rendererFlags & 8))
        config->mFlags = rendererFlags | 0x18;
    config->mpAsset = sGraphicsAssets->mCount ? (void *)sGraphicsAssets->mpAssets[0] : NULL;
    hdFadeListener_l* listener = (hdFadeListener_l *)operator_new(sizeof(hdFadeListener_l));
    if (listener != NULL) {
        constructGraphicsListener(listener);
        SafeString* listenerName =
            gabi::at<SafeString>(gabi::ea(listener) + offsetof(hdFadeListener_l, mListenerVtable));
        if (listenerName == NULL)
            listenerName = (SafeString*)operator_new(sizeof(SafeString));
        if (listenerName != NULL) {
            listenerName->__vtbl = gabi::ea(&sGraphicsNameVtable);
            listenerName->mStringTop = gabi::guest_addr("");
        }
        listener->mVtable = gabi::ea(&sGraphicsListenerDisposerTable);
        listener->mUserData = 0;
        listener->mDelegate.mpOwner = self;
        listener->mDelegate.mAdjustment = 0;
        listener->mDelegate.mVirtualIndex = -1;
        listener->mDelegate.mCallback = gabi::guest_fn((const void *)graphicsRenderCallback);
        listener->mDelegate.mVtable = gabi::ea(&sGraphicsListenerDelegateTable);
        listener->mListenerVtable = gabi::ea(&sGraphicsListenerTable);
        listener->mEnabled = 1;
    }
    attachGraphicsListener(renderer, 0, listener);
    attachGraphicsListener(renderer, 2, listener);
    attachGraphicsListener(renderer, 1, listener);
    attachGraphicsListener(renderer, 3, listener);
    attachGraphicsListener(renderer, 4, listener);
    beginGraphicsContext(sGraphicsContext);
    hdRenderLayerFactory_c* factory = (hdRenderLayerFactory_c *)renderer;
    SafeString name;
    setGraphicsLayerName(&name, "TV_Shadow");
    hdLayerBase_c* shadow = factory->createShadow(0, &name, 0, heap);
    setGraphicsLayerName(&name, "TV_3D");
    hdLayerBase_c* scene = factory->createScene(1, &name, 0, heap);
    setGraphicsLayerName(&name, "TV_3DMask");
    factory->createSceneMask(2, &name, 0, heap);
    setGraphicsLayerName(&name, "TV_2D");
    factory->createUI(3, &name, 0, heap);
    setGraphicsLayerName(&name, "TV_MultiFilter");
    factory->createParticle(4, &name, 0, heap);
    setGraphicsLayerName(&name, "TV_2DFore");
    factory->createUIForeground(5, &name, 0, heap);
    setGraphicsLayerName(&name, "TV_Fader");
    factory->createFade(6, &name, 0, heap);
    setGraphicsLayerName(&name, "TV_DRCMode2D");
    factory->createDRCScene(11, &name, 0, heap);
    setGraphicsLayerName(&name, "TV_ErrorViewer");
    factory->createDRCSceneMask(12, &name, 0, heap);
    setGraphicsLayerName(&name, "DRC_2D");
    factory->createDRCUI(7, &name, 1, heap);
    setGraphicsLayerName(&name, "DRC_MultiFilter");
    factory->createParticle(8, &name, 1, heap);
    setGraphicsLayerName(&name, "DRC_2DFore");
    factory->createDRCUIForeground(9, &name, 1, heap);
    setGraphicsLayerName(&name, "DRC_Fader");
    factory->createFade(10, &name, 1, heap);
    setGraphicsLayerName(&name, "DRC_ErrorViewer");
    factory->createDRCSceneMask(13, &name, 1, heap);
    ((hdGraphicsLayerPriority_l *)scene)->mPriority = 100;
    ((hdGraphicsLayerPriority_l *)shadow)->mPriority = 50;
    attachGraphicsShadow(shadow, scene);
    endGraphicsContext(sGraphicsContext);
    adjustGraphicsHeap(heap);
    void* parent = heap->mpParent;
    hdGraphicsRenderer_l* displays = (hdGraphicsRenderer_l *)renderer;
    for (u32 i = 0; i < 2; ++i)
        for (u32 j = 0; j < 4; ++j)
            displays->mDisplays[i].mClearColor[j] = 0.0f;
    displays->mDisplays[1].mMode = 2;
    displays->mDisplays[0].mMode = 2;
    self->configureBuffers();
    createGraphicsProfiler(0);
    initializeGraphicsProfiler(sGraphicsProfiler, parent);
    ((hdGraphicsProfilerLimit_l *)(void *)sGraphicsProfiler)->mLimit = 180.0f;
    for (u32 i = 0; i < 2; ++i)
        initializeGraphicsRenderBuffer(self->mRenderBuffers[i], parent);
    rootHeap = getGraphicsSupportHeap();
    SafeString supportName;
    setGraphicsLayerName(&supportName, "agl::utl::DebugPrimitiveMgr");
    hdGraphicsHeap_l* support = createGraphicsHeap(0, &supportName, rootHeap, 1, 0);
    createGraphicsSupport(support);
    adjustGraphicsHeap(support);
    u32 previous = enterGraphicsHeap(parent);
    hdGraphicsService_c::create(NULL);
    hdGraphicsServiceInit_l serviceParams;
    constructGraphicsServiceParams(&serviceParams);
    hdGraphicsService_c* service = sGraphicsService;
    hdGraphicsServiceVtable_l* serviceVtable =
        gabi::at<hdGraphicsServiceVtable_l>(service->mVtable);
    gabi::call_ptr(serviceVtable->mInitialize.get(), service, &serviceParams);
    leaveGraphicsHeap(previous);
    rendererFlags = config->mFlags;
    config->mFlags =
        sGraphicsService != NULL ? (rendererFlags | 4) & 0xFFDF : rendererFlags & 0xFFDB;
    previous = enterGraphicsHeap(parent);
    createGraphicsCaptureDevice(0);
    initializeGraphicsCaptureDevice(sGraphicsCaptureDevice, 0x5EE, parent);
    leaveGraphicsHeap(previous);
    createGraphicsSupportService(0);
    initializeGraphicsSupportService(sGraphicsSupport, parent);
    previous = enterGraphicsHeap(parent);
    createGraphicsResourceService(0);
    beginGraphicsContext(sGraphicsContext);
    initializeGraphicsResourceService(sGraphicsResourceList);
    endGraphicsContext(sGraphicsContext);
    leaveGraphicsHeap(previous);
    SafeString shaderName;
    setGraphicsLayerName(&shaderName, "PrimShp4WiiUtilHeap");
    hdGraphicsHeap_l* shaderHeap = createGraphicsShaderHeap(0x100000, &shaderName, parent, 1, 0);
    beginGraphicsContext(sGraphicsContext);
    createGraphicsShaderService(shaderHeap);
    endGraphicsContext(sGraphicsContext);
    adjustGraphicsHeap(shaderHeap);
    void* shaderResource =
        sGraphicsResourceList->mCount ? (void *)sGraphicsResourceList->mpEntries : NULL;
    void* archive = getGraphicsShaderArchive();
    initializeGraphicsShaderArchive(archive, shaderResource, 0);
    hdShaderBackend_c::create(NULL);
    initializeBackendShaders(sGraphicsShaders, parent);
    initializeGraphicsDrawMode(&sGraphicsDrawMode);
    hdRenderResources_c::create(NULL);
    sGraphicsResources->load();
    sGraphicsResources->bindLayers();
    completeGraphicsTask(self);
}
/* WWHD glue
 ext 0272A00C void graphicsEmptyCallback() callback
 str 101412CC "TV_Shadow"
 str 1014125C "TV_3D"
 str 101412D8 "TV_3DMask"
 str 10141264 "TV_2D"
 str 10141310 "TV_MultiFilter"
 str 101412E4 "TV_2DFore"
 str 10141320 "TV_Fader"
 str 10141274 "DRC_2D"
 str 1014127C "DRC_MultiFilter"
 str 1014134C "DRC_2DFore"
 str 101412F0 "DRC_Fader"
 str 1014132C "TV_DRCMode2D"
 str 1014133C "TV_ErrorViewer"
 str 1014128C "DRC_ErrorViewer"
 data sGraphicsAssets 101F8AE8 nocheck
 data sGraphicsContext 101F8B18 nocheck
 data sGraphicsHeapManager 101F8B4C nocheck
 data sGraphicsSupport 101F8A20 nocheck
 data sGraphicsResourceList 101F8A94 nocheck
 data sGraphicsShaders 101F8710 nocheck
 data sGraphicsResources 101F8664 nocheck
 str 101412FC "agl::lyr::Renderer"
 str 1014129C "agl::utl::DebugPrimitiveMgr"
 str 101412B8 "PrimShp4WiiUtilHeap"
 data sGraphicsListenerDisposerTable 10169E3C nocheck
 data sGraphicsListenerDelegateTable 10141220 nocheck
 data sGraphicsListenerTable 1014126C nocheck
 str 10141155 "" in 027297C0
 ext 0203E8F4 void* getGraphicsRendererHeap()
 ext 0203E924 void* getGraphicsSupportHeap()
 ext 02753004 hdGraphicsHeap_l* createGraphicsHeap(u32,SafeString*,void*,u32,u32)
 ext 02754B38 hdGraphicsHeap_l* createGraphicsShaderHeap(u32,SafeString*,void*,u32,u32)
 ext 027C21C4 void constructGraphicsCreateParams(hdGraphicsCreateParams_l*)
 ext 02739438 u32 getGraphicsTVWidth()
 ext 02739488 u32 getGraphicsTVHeight()
 ext 02739594 u32 getGraphicsDRCWidth()
 ext 027395E4 u32 getGraphicsDRCHeight()
 ext 027C22D0 void setGraphicsCreateSize(hdGraphicsCreateParams_l*,u32,hdGraphicsIntSize_l*,hdGraphicsIntSize_l*)
 ext 027C0A60 hdRenderLayerRegistry_l* createGraphicsRenderer(void*)
 ext 027C0CEC void initializeGraphicsRenderer(hdRenderLayerRegistry_l*,hdGraphicsCreateParams_l*,void*,u32)
 ext 02752A84 void constructGraphicsListener(hdFadeListener_l*)
 ext 027C1C64 void attachGraphicsListener(hdRenderLayerRegistry_l*,u32,hdFadeListener_l*)
 ext 0274FBF8 void beginGraphicsContext(void*)
 ext 0274FCCC void endGraphicsContext(void*)
 ext 02736260 void attachGraphicsShadow(hdLayerBase_c*,hdLayerBase_c*)
 ext 0278BC9C void createGraphicsProfiler(u32)
 ext 0278BDF8 void initializeGraphicsProfiler(void*,void*)
 ext 0279DDA4 void initializeGraphicsRenderBuffer(void*,void*)
 ext 0279C844 void createGraphicsSupport(void*)
 ext 02756170 void* switchGraphicsHeap(void*,void*)
 ext 02759814 void constructGraphicsServiceParams(hdGraphicsServiceInit_l*)
 ext 027F96F0 void createGraphicsCaptureDevice(u32)
 ext 027F9890 void initializeGraphicsCaptureDevice(void*,u32,void*)
 ext 02738370 void createGraphicsSupportService(u32)
 ext 02738418 void initializeGraphicsSupportService(void*,void*)
 ext 02738CD0 void createGraphicsResourceService(u32)
 ext 0273918C void initializeGraphicsResourceService(hdGraphicsResourceList_l*)
 ext 027FFCC8 void createGraphicsShaderService(void*)
 ext 027FFCBC void* getGraphicsShaderArchive()
 ext 028010F8 void initializeGraphicsShaderArchive(void*,void*,u32)
 ext 027F29B8 void initializeGraphicsDrawMode(hdGraphicsDrawMode_l*)
 ext 027488E8 void completeGraphicsTask(hdGraphicsTask_c*)
 data sGraphicsDrawState 104B45C0 nocheck
 data sGraphicsDefaultCamera 104A2098 nocheck
 data sGraphicsDefaultProjection 104A20FC nocheck
 data sGraphicsPerspectiveTypeReady 101FD9F0 nocheck
 data sGraphicsPerspectiveType 101FDCD4 nocheck
 str 10141210 "" in 0272A8C4
 str 10141360 "RenderDisplay::draw"
 ext 027C2580 void* getGraphicsLayerContext(hdLayerBase_c*)
 ext 0273011C void prepareGraphicsLayer(hdLayerBase_c*,void*)
 ext 02730164 void finishGraphicsLayer(hdLayerBase_c*)
 ext 0274D83C void* getGraphicsProjectionMatrix(hdProjection_l*)
 ext 028E8970 void copyGraphicsProjectionMatrix(void*,void*)
 ext 0274E488 void setGraphicsProjectionBounds(hdProjection_l*,f32,f32,f32,f32)
 ext 027F81C0 void setGraphicsDeviceContext(void*,u32,void*)
 ext 027F88C8 void clearGraphicsDeviceContext(void*,u32)
 ext 025200D4 hdLayerPlay_l* getGraphicsPlay()
 ext 025A8250 void prepareGraphicsParticles(void*,hdGraphicsParticleParams_l*)
 ext 02758AAC void copyGraphicsParticleBuffer(void*,void*)
 ext 027F9308 void prepareGraphicsParticleContext(void*,hdGraphicsParticleContext_l*)
 ext 027F9AC0 void finishGraphicsParticleContext(hdGraphicsParticleContext_l*)
 ext 027C1848 void renderGraphicsDisplay(hdRenderLayerRegistry_l*,u32)
 ext 027C1A24 void finishGraphicsDisplayCapture(hdRenderLayerRegistry_l*,u32)
 data sGraphicsSubmissionFlags 104A208C nocheck
 data sGraphicsSubmitting 104B4658 nocheck
 str 10141374 "RenderDisplay::calcGPU"
 data sGraphicsCaptureDevice 101F997C nocheck
 ext 027C1A60 void submitGraphicsDisplay(hdRenderLayerRegistry_l*,u32)
 ext 027C1E48 hdGraphicsCapture_l* getGraphicsCapture(hdRenderLayerRegistry_l*,u32)
 ext 027591F4 void queueGraphicsCapture(hdGraphicsService_c*,const char*,hdGraphicsCapture_l*,be<u32>*,be<u32>*,be<u32>*)
 ext 02759324 void processGraphicsCaptures(hdGraphicsService_c*)
 ext 02759564 void finishGraphicsCaptures(hdGraphicsService_c*)
 ext 027F9A10 void beginGraphicsDeviceCapture(void*)
 ext 027F9A7C void endGraphicsDeviceCapture(void*)
 ext C0009E90 void flushGraphicsCommands()
 data sGraphicsDrawMode 104B45C0 nocheck
 data sGraphicsBufferManager 101F8DC0 nocheck
 ext 027C1AD8 u32 isGraphicsDisplayEnabled(hdRenderLayerRegistry_l*,u32)
 ext 0274D690 void activateGraphicsBuffer(void*)
 ext 0274F5F0 void makeGraphicsViewport(void*,void*)
 ext 0274F964 void activateGraphicsViewport(void*,void*)
 ext 02799C68 void registerGraphicsBuffer(void*,void*,void*,u32)
 ext 0279B4EC void finishGraphicsTV(hdGraphicsScale_l*)
 ext 0279B568 void finishGraphicsDRC(hdGraphicsScale_l*)
 data sGraphicsScale 101F8DC8 nocheck
 data sGraphicsDefaultSize 1049FB6C nocheck
 data sGraphicsDevice 101F9968 nocheck
 ext 027F81A4 u32 queryGraphicsDevice(void*,u32)
 ext 0279AF10 void updateGraphicsScale(hdGraphicsScale_l*)
 ext C0006168 void synchronizeGraphicsScale()
 ext 0278BF5C void beginGraphicsTiming(void*)
 ext 027C106C void prepareGraphicsRenderer(hdRenderLayerRegistry_l*,u32)
 data sGraphicsBaseCallbackVtable 101411A0 nocheck
 data sGraphicsBaseInterfaceVtable 101448AC nocheck
 data sGraphicsListenerDisposerVtable 101411C0 nocheck
 data sGraphicsReferenceVtable 10141200 nocheck
 ext 0279D92C void destroyGraphicsRenderBuffer(void*,u32) callback
 ext 028F0164 void destroyGraphicsArray(void*,u32,u32,u32,u32,u32)
 ext 02729208 void destroyGraphicsRegistration(void*,u32)
 ext 02747A84 void clearGraphicsListener(void*)
 ext 02752BEC void destroyGraphicsDisposer(void*,u32)
 ext 027485F0 void destroyGraphicsTaskBase(void*,u32)
 ext 0273B310 void releaseGraphicsReference(void*)
 ext 0272B060 void destroyGraphicsServiceBase(void*,u32)
 data sGraphicsDisplayTypeReady 101FD750 nocheck
 data sGraphicsDisplayType 101FDDFC nocheck
 str 101411F0 "" in 02729688
 ext 02751400 void configureGraphicsDisplay(void*,u32)
 data sGraphicsTaskVtable 101413C8 nocheck
 data sGraphicsTaskInterface 10141490 nocheck
 data sGraphicsTaskCallbackVtable 101414A0 nocheck
 data sGraphicsNameVtable 10141158 nocheck
 str 10141154 "" in 027294A8
 data sGraphicsServiceVtable 101413A8 nocheck
 data sGraphicsServiceDisposerVtable 10141398 nocheck
 data sGraphicsService 101F86E0 nocheck
 data sGraphicsServiceDisposer 101F86E4 nocheck
 str 10141248
 ext 02746910 void constructGraphicsTask(hdGraphicsTask_c*,void*,const char*)
 ext 028EFFD0 void constructGraphicsRenderBuffers(void*,u32,u32,u32)
 ext 0279DFE0 void constructGraphicsRenderBuffer(void*) callback
 ext 0273971C cXy* getGraphicsRenderSize()
 ext 02758B88 void constructGraphicsService(hdGraphicsService_c*)
 ext 0273B0D4 void* allocateGraphicsService(u32,void*,s32)
 ext 02752B0C void constructGraphicsDisposer(hdGraphicsDisposer_l*,void*,u32)
 data sGraphicsActiveTask 101F86E8 nocheck
 data sGraphicsRenderer 101F95D0 nocheck
 data sGraphicsMode 101F5088 nocheck
 data sGraphicsProfiler 101F8D74 nocheck
 ext 02748974 hdGraphicsDisplayOwner_l* getGraphicsDisplayOwner(hdGraphicsTask_c*)
 ext 02739578 cXy* getGraphicsScreenSize(u32)
 ext 027C23D4 void enableGraphicsLayer(hdLayerBase_c*,u32)
 ext 0274AD68 void finishGraphicsDisplay(hdGraphicsDisplayOwner_l*)
 ext C00064A8 void setGraphicsColorMask(u32,u32,s32)
 ext 0278C4DC void beginGraphicsProfile(void*)
 ext 0278C544 void endGraphicsProfile(void*)
 ext 0272AF0C void graphicsRenderCallback(hdGraphicsTask_c*,hdLayerRenderInfo_c*) callback
*/
