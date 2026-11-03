// Shader-context fields and helper-object extents from local game instructions.
#pragma once
#include "sead/HeapRegistry.h"
#include "sead/Thread.h"
#include "sead/DisplayTargets.h"
#include "agl/RenderRecording.h"
namespace agl {
struct ShaderContextMethods { u8 mHeader[0x10]; sead::VtableSlot mDestroy; };
struct ShaderContextDisposer : sead::Disposer { void destroy(u32 flags); };
struct ShaderModeConfiguration {
    be<u8> mNormalVertexGprs,mNormalVertexStack;
    be<u8> mGeometryVertexGprs,mGeometryVertexStack;
    be<u8> mNormalPixelGprs,mNormalPixelStack;
    be<u8> mGeometryPixelGprs,mGeometryPixelStack;
    be<u8> mGeometryGprs,mGeometryStack;
    u8 mReserved[2];
    gptr<sead::VtableSlot> mMethods;
};
struct ShaderContext {
    gptr<ShaderContextMethods> mMethods;
    ShaderContextDisposer mDisposer;
    gptr<ShaderModeConfiguration> mModeConfiguration;
    ShaderModeConfiguration mOwnedModeConfiguration;
    be<u32> mInputItemSize,mOutputItemSize;
    gptr<void> mInputRing;
    be<u32> mInputRingBytes;
    gptr<void> mOutputRing;
    be<u32> mOutputRingBytes;
    sead::CriticalSection mMutex;
    be<u8> mUseContext;
    u8 mReservedContext;
    be<u8> mUseDisplayList;
    u8 mReservedDisplayList;
    DisplayListRecording mDisplayList;
    static ShaderContext* construct(ShaderContext* object);
    static void createSingleton(sead::RegistryHeap* heap);
    void destroy(u32 flags);
    void setMode(u32 mode,const ShaderModeConfiguration* configuration);
    void setConfiguredMode(u32 mode);
    u32 initializeRings(sead::RegistryHeap* heap,void* memory,u32 inputItemSize,u32 outputItemSize,u32 useDisplayList);
    void bindGeometryRings();
    void applyContext();
};
}
WWHD_SIZE(agl::ShaderModeConfiguration,16);
WWHD_SIZE(sead::CriticalSection,60);
WWHD_SIZE(agl::ShaderContext,156);
WWHD_OFFSET(agl::ShaderContext,mModeConfiguration,0x14);
WWHD_OFFSET(agl::ShaderContext,mOwnedModeConfiguration,0x18);
WWHD_OFFSET(agl::ShaderContext,mInputRing,0x30);
WWHD_OFFSET(agl::ShaderContext,mMutex,0x40);
WWHD_OFFSET(agl::ShaderContext,mUseContext,0x7C);
WWHD_OFFSET(agl::ShaderContext,mUseDisplayList,0x7E);
WWHD_OFFSET(agl::ShaderContext,mDisplayList,0x80);
WWHD_SIZE(agl::ShaderContextDisposer,16);
WWHD_OFFSET(agl::ShaderContext,mDisposer,4);
