// WWHD render targets and scanout configuration, reconstructed from local instructions.
#include "sead/DisplayTargets.h"
#include "sead/RuntimeType.h"
#include "sead/GraphicsDevice.h"
#include "port_native_draw.h"

static u32 sScanTable;
static sead::RuntimeType sScanType;
static sead::RuntimeType sTargetType;
static sead::RuntimeType sSurfaceType;
static sead::RuntimeType sDisplaySurfaceType;
static sead::RuntimeType sCafeTargetType;
static u32 sScanGuard;
static u32 sTargetGuard;
static u32 sSurfaceGuard;
static u32 sDisplaySurfaceGuard;
static u32 sCafeTargetGuard;
static u32 sScanTypeTable;
static u32 sTargetTypeTable;
static u32 sSurfaceTypeTable;
static u32 sDisplaySurfaceTypeTable;
void* allocateDisplayObject(u32 size);
void clearDisplayBuffers(void* color, void* depth, u32 stencil, u32 flags,
                         f32 red, f32 green, f32 blue, f32 alpha, f32 clearDepth);
void clearDisplayColor(void* color, f32 red, f32 green, f32 blue, f32 alpha);
void clearDisplayDepth(void* depth, u32 stencil, u32 flags, f32 clearDepth);
void setDisplayContext(void* context);
void setDisplayViewport(f32 x, f32 y, f32 width, f32 height, f32 nearDepth, f32 farDepth);
void setDisplayScissor(u32 x, u32 y, u32 width, u32 height);
void copyDisplayToScan(void* color, u32 target);
void bindDisplayColor(void* color, u32 target);
void bindDisplayDepth(void* depth);
void calculateTvSize(u32 mode, u32 format, u32 buffers, be<u32>* size, be<u32>* alignment);
void calculateTabletSize(u32 mode, u32 format, u32 buffers, be<u32>* size, be<u32>* alignment);
void setTvBuffer(void* buffer, u32 size, u32 mode, u32 format, u32 buffers);
void setTabletBuffer(void* buffer, u32 size, u32 mode, u32 format, u32 buffers);
void setTvScale(u32 width, u32 height);
void setTabletScale(u32 width, u32 height);
void waitDisplayVsync();

/* 02750E24 */
void sead::DisplayFrame::clear(u32 flags, const RenderQuaternion* color, u32 stencil, f32 depth) {
    void* colorBuffer = mColor;
    void* depthBuffer = mDepth;
    u32 depthFlags = (flags >> 1) & 3;
    if (flags & 1) {
        f32 red = color->x, green = color->y, blue = color->z, alpha = color->w;
        if (depthFlags != 0)
            clearDisplayBuffers(colorBuffer, depthBuffer, (u8)stencil, depthFlags, red, green, blue, alpha, depth);
        else clearDisplayColor(colorBuffer, red, green, blue, alpha);
    } else if (depthFlags != 0) {
        clearDisplayDepth(depthBuffer, (u8)stencil, depthFlags, depth);
    }
    setDisplayContext(reinterpret_cast<DeviceStateCache_l*>(sCurrentGraphicsDevice.get())->mContext);
}

/* 02750FAC */
void sead::DisplayFrame::copyToScan(void* display) {
    if (sScanGuard == 0) {
        sScanGuard = 1;
        sScanType.mTable = &sScanTypeTable;
    }
    struct DisplayCastTable_l { u8 mTypeAndDestroy[12]; be<u32> mMatches; };
    struct DisplayCastObject_l { u8 mCoordinates[8]; gptr<DisplayCastTable_l> mTable; be<u32> mTarget; };
    DisplayCastObject_l* target = (DisplayCastObject_l*)display;
    if (target != NULL) {
        if (gabi::call_ptr<u32>(target->mTable->mMatches, target, &sScanType) == 0) target = NULL;
    }
    ColorBuffer_l* color = mColor;
    u32 width = color->mWidth, height = color->mHeight;
    setDisplayViewport(0.0f, 0.0f, (f32)width, (f32)height, 0.0f, 1.0f);
    color = mColor;
    setDisplayScissor(0, 0, color->mWidth, color->mHeight);
    u32 output = target->mTarget;
    copyDisplayToScan(mColor, output == 0 ? 1 : 4);
}

/* 027510C0 */
void sead::DisplayFrame::bind() {
    bindDisplayColor(mColor, 0);
    bindDisplayDepth(mDepth);
}

/* 027510FC */
sead::ScanTarget* sead::ScanTarget::create(ScanTarget* object, u32 target, u32 progressive) {
    if (object == NULL) object = (ScanTarget*)allocateDisplayObject(sizeof(ScanTarget));
    if (object != NULL) {
        object->mTarget = target;
        object->mProgressive = progressive;
        object->mBuffer = NULL;
        object->mWidth = 0.0f;
        object->mTable = &sScanTable;
        object->mHeight = 0.0f;
    }
    return object;
}

static u32 scanDimension(f32 value) {
    const f32 signedBoundary = 2147483648.0f;
    if (value < signedBoundary) return (u32)gabi::ftoi(value);
    return (u32)gabi::ftoi(gabi::fsubs_ppc(value, signedBoundary)) + 0x80000000u;
}

/* 02751174 */
void sead::ScanTarget::allocate(AllocationHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 target = mTarget;
    if (target > 1) return;
    gabi::FrameLocal<be<u32>> size(16);
    gabi::FrameLocal<be<u32>> alignment(20);
    u32 mode;
    if (target == 0) {
        mode = mProgressive != 0 ? 5 : 3;
        calculateTvSize(mode, 0x1A, 2, size, alignment);
    } else {
        mode = 1;
        calculateTabletSize(mode, 0x1A, 2, size, alignment);
    }
    void* buffer = gabi::call_ptr<void*>(heap->mTable->mAllocate, heap, (u32)*size, 0x1000);
    u32 bytes = *size;
    mBuffer = buffer;
    if (target == 0) {
        setTvBuffer(buffer, bytes, mode, 0x1A, 2);
        u32 width = scanDimension(mWidth);
        u32 height = scanDimension(mHeight);
        setTvScale(width, height);
    } else {
        setTabletBuffer(buffer, bytes, mode, 0x1A, 2);
        u32 width = scanDimension(mWidth);
        u32 height = scanDimension(mHeight);
        setTabletScale(width, height);
    }
}

/* 02751400 */
void sead::ScanTarget::configure(u32 format) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 target = mTarget;
    if (target > 1) return;
    gabi::FrameLocal<be<u32>> size(8);
    gabi::FrameLocal<be<u32>> alignment(12);
    if (target == 0) {
        u32 mode = mProgressive != 0 ? 5 : 3;
        calculateTvSize(mode, 0x1A, 2, size, alignment);
        setTvBuffer(mBuffer, (u32)*size, mode, format, 2);
    } else {
        calculateTabletSize(1, 0x1A, 2, size, alignment);
        setTabletBuffer(mBuffer, (u32)*size, 1, format, 2);
    }
}

/* 027514F8 */
static u32 matchesCafeDisplay(void* object, const sead::RuntimeType* type) {
    if (sCafeTargetGuard == 0) {
        sCafeTargetGuard = 1;
        sCafeTargetType.mTable = &sTargetTypeTable;
    }
    if (type == &sCafeTargetType) return 1;
    if (sTargetGuard == 0) {
        sTargetGuard = 1;
        sTargetType.mTable = &sDisplaySurfaceTypeTable;
    }
    if (type == &sTargetType) return 1;
    if (sSurfaceGuard == 0) {
        sSurfaceGuard = 1;
        sSurfaceType.mTable = &sSurfaceTypeTable;
    }
    return type == &sSurfaceType;
}

/* 027515B4 */
static u32 matchesScanTarget(void* object, const sead::RuntimeType* type) {
    if (sScanGuard == 0) {
        sScanGuard = 1;
        sScanType.mTable = &sScanTypeTable;
    }
    if (type == &sScanType) return 1;
    if (sDisplaySurfaceGuard == 0) {
        sDisplaySurfaceGuard = 1;
        sDisplaySurfaceType.mTable = &sSurfaceTypeTable;
    }
    return type == &sDisplaySurfaceType;
}

/* 0275162C */
void sead::DeviceStateCache_l::initialize() {
    mRenderMode = 3;
    mAlphaReference = 0.0f;
    mBlendMask = 255;
    mPointOffset = 0;
    mStencilEnable = 0;
    mBackPolygon = 2;
    mAlphaComparison = 7;
    mContext = NULL;
    mCullMode = 1;
    mPolygonOffset = 0;
    mFrontPolygon = 2;
    mDepthWrite = 0;
    mDepthComparison = 3;
    mDepthTest = 0;
    mChannelMask = ~0u;
    mBlendEquation = 0;
    mLineOffset = 0;
}

/* 02751698 */
static void setCafeViewport(void* device, f32 x, f32 y, f32 width, f32 height) {
#if PORT_NATIVE_DRAW
    wwhd_native_pass_viewport(x, y, width, height, 0.0f, 1.0f);  // pass description (port_native_draw.h)
#endif
    setDisplayViewport(x, y, width, height, 0.0f, 1.0f);
}

/* 027516AC */
static void setCafeScissor(void* device, f32 x, f32 y, f32 width, f32 height) {
    u32 integerX = scanDimension(x);
    u32 integerY = scanDimension(y);
    u32 integerWidth = scanDimension(width);
    u32 integerHeight = scanDimension(height);
#if PORT_NATIVE_DRAW
    wwhd_native_pass_scissor(integerX, integerY, integerWidth, integerHeight);  // pass description (port_native_draw.h)
#endif
    setDisplayScissor(integerX, integerY, integerWidth, integerHeight);
}

/* 027517BC */
static void waitCafeVsync() { waitDisplayVsync(); }

/* 027517C0 */
static void cafePresentNoop() {}

/* 027517C4 */
static void cafeResizeNoop() {}

/* WWHD glue
 data sScanTable 101456A0
 data sScanType 101FDDFC
 data sScanGuard 101FD750
 data sScanTypeTable 10145638
 data sCafeTargetType 101FDF8C
 data sCafeTargetGuard 101FD74C
 data sTargetType 101FDCF4
 data sTargetGuard 101FD728
 data sSurfaceType 101FDCF0
 data sSurfaceGuard 101FD724
 data sDisplaySurfaceType 101FDE04
 data sDisplaySurfaceGuard 101FD730
 data sTargetTypeTable 10145628
 data sDisplaySurfaceTypeTable 10145618
 data sSurfaceTypeTable 10145608
 ext 0273AD10 void* allocateDisplayObject(u32)
 ext C00060F0 void clearDisplayBuffers(void*,void*,u32,u32,f32,f32,f32,f32,f32)
 ext C00060F8 void clearDisplayColor(void*,f32,f32,f32,f32)
 ext C0006100 void clearDisplayDepth(void*,u32,u32,f32)
 ext C00067E0 void setDisplayContext(void*)
 ext C0006A50 void setDisplayViewport(f32,f32,f32,f32,f32,f32)
 ext C0006970 void setDisplayScissor(u32,u32,u32,u32)
 ext C0006110 void copyDisplayToScan(void*,u32)
 ext C00067C8 void bindDisplayColor(void*,u32)
 ext C0006838 void bindDisplayDepth(void*)
 ext C00060D8 void calculateTvSize(u32,u32,u32,be<u32>*,be<u32>*)
 ext C00060A8 void calculateTabletSize(u32,u32,u32,be<u32>*,be<u32>*)
 ext C00069C8 void setTvBuffer(void*,u32,u32,u32,u32)
 ext C00067F8 void setTabletBuffer(void*,u32,u32,u32,u32)
 ext C00069E0 void setTvScale(u32,u32)
 ext C0006818 void setTabletScale(u32,u32)
 ext C0006AE8 void waitDisplayVsync()
*/
