/* WWHD graphics access manager: own constructor extent, per-core lock selection and fallback mode. */
#pragma once
#include "sead/Thread.h"
#include "sead/GraphicsDevice.h"
namespace agl {
void setResourceFlag();
void clearResourceState();
void registerGraphicsResources(sead::RegistryHeap* heap);
// Ten-byte graphics metadata; the element meanings are not inferred from their initial values.
struct GraphicsByteLayout {
    be<u8> mValues[10];
    u8 mPadding[2];
    gptr<sead::DisposerVtable> mVtable;
    void initialize();
    GraphicsByteLayout* construct();
    void destroy(u32 flags);
};
namespace graphics_access_text {
struct TextA : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextB : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
}
struct GraphicsAccessMethods {
    sead::VtableSlot mHeader;
    sead::VtableSlot mReserved;
    sead::VtableSlot mDestroy;
};
using AccessMutexConstructor = sead::CriticalSection* (*)(sead::CriticalSection*);
using AccessMutexDestructor = void (*)(sead::CriticalSection*, u32);
struct GraphicsAccessDisposer : sead::RegisteredObject { void destroy(u32 flags); };
struct GraphicsAccessManager {
    gptr<GraphicsAccessMethods> mMethods;
    sead::RegisteredObject mDisposer;
    be<u32> mReferenceCount;
    be<u32> mMode;
    sead::CriticalSection mCoreLocks[3];
    sead::CriticalSection mSharedLock;
    be<u32> mReserved;
    be<u32> mState;
    GraphicsAccessManager* construct();
    static GraphicsAccessManager* create(sead::RegistryHeap* heap);
    void destroy(u32 flags);
    void lockCore();
    void unlockCore();
    void acquire();
    void release();
    void acquireLocal();
    void releaseLocal();
};
u32 currentGraphicsSlot();
WWHD_OFFSET(GraphicsByteLayout,mValues,0);
WWHD_OFFSET(GraphicsByteLayout,mVtable,0xC);
WWHD_SIZE(GraphicsByteLayout,0x10);
WWHD_OFFSET(GraphicsAccessMethods,mDestroy,0x10);
WWHD_SIZE(GraphicsAccessMethods,0x18);
WWHD_SIZE(GraphicsAccessDisposer,0x10);
WWHD_OFFSET(GraphicsAccessManager,mDisposer,4);
WWHD_OFFSET(GraphicsAccessManager,mReferenceCount,0x14);
WWHD_OFFSET(GraphicsAccessManager,mMode,0x18);
WWHD_OFFSET(GraphicsAccessManager,mCoreLocks,0x1C);
WWHD_OFFSET(GraphicsAccessManager,mSharedLock,0xD0);
WWHD_OFFSET(GraphicsAccessManager,mReserved,0x10C);
WWHD_OFFSET(GraphicsAccessManager,mState,0x110);
WWHD_SIZE(GraphicsAccessManager,0x114);
}

// Borrowed native-resource singleton, one local data view pending owner promotion.
extern gptr<void>& sNativeResources_l;
extern gptr<void>& sGraphicsResourceDebug_l;
extern gptr<void>& sShaderResources_l;
extern gptr<void>& sPrimitiveShapeResources_l;
extern gptr<void>& sPrimitiveTextureResources_l;
extern gptr<void>& sDynamicTextureResources_l;
extern gptr<void>& sDebugTextureResources_l;
extern gptr<agl::GraphicsAccessManager>& sGraphicsAccessManager;
extern sead::SafeTextVtable& sResourceTextMethods;
