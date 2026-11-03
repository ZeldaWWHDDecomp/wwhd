// J3D HD renderer: texture binding layouts.
// Names are our own; written from the WWHD binary.
#pragma once
#include "JSystem/J3DGraph/RenderValueStorage.h"

namespace j3d {
struct RenderTextureVtable_l {
    sead::VtableSlot mHeader;
    sead::VtableSlot mDestroy;
};
struct RenderTexture_l {
    u8 mTextureState[0x194];
    gptr<RenderTextureVtable_l> mMethods;
};
struct RenderTextureSlot_l { u8 mPrefix[4]; u8 mSampler[16]; };
struct RenderTextureSlots_l { u8 mPrefix[0x14]; be<u32> mCount; gptr<RenderTextureSlot_l> mSlots; };
struct RenderTextureBindings {
    gptr<RenderTexture_l> mTextures[15];
    be<u8> mOwned[15];
    be<u8> mReady;
    static RenderTextureBindings* construct(RenderTextureBindings* storage);
    static void destroy(RenderTextureBindings* storage, u32 flags);
    void clear(u32 index);
    void clearAll();
    void bindOwned(const RenderTextureSlots_l* slots);
    void bindExtra(const RenderTextureSlots_l* slots);
    u32 allocate(u32 index, void* resource, void* heap);
};
}
WWHD_OFFSET(j3d::RenderTexture_l, mMethods, 0x194);
WWHD_SIZE(j3d::RenderTexture_l, 0x198);
WWHD_OFFSET(j3d::RenderTextureBindings, mOwned, 0x3C);
WWHD_OFFSET(j3d::RenderTextureBindings, mReady, 0x4B);
WWHD_SIZE(j3d::RenderTextureBindings, 0x4C);
void* allocateRenderTextureObject(u32 size);
void* allocateRenderTextureObjectAligned(u32 size, void* heap, s32 alignment);
void prepareRenderTextureStorage(void* storage, u32 size);
void releaseRenderTextureObject(void* storage);
j3d::RenderTexture_l* constructRenderTexture(j3d::RenderTexture_l* storage, void* resource);

WWHD_SIZE(j3d::RenderTextureSlot_l, 20);
WWHD_OFFSET(j3d::RenderTextureSlots_l, mCount, 0x14);
WWHD_OFFSET(j3d::RenderTextureSlots_l, mSlots, 0x18);
void bindRenderTexture(j3d::RenderTexture_l*,const void*,u32,u32);
