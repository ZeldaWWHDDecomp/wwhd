/* Boss magma: GameCube dMagma_ballBoss_c names with the HD mesh prefix and GHS vtable layout.
 * Private views model only fields this TU uses; no allocation relies on the opaque library views.
 * The private btd_class view avoids changing the shared actor header's padding. Local only. */
#pragma once
#include "gc_api/gc_api.h"

struct btd_class : fopAc_ac_c {
    u8 _btd[0x70C0 - sizeof(fopAc_ac_c)];
    /* 0x70C0 */ be<f32> m6E7C;
    u8 _70C4[4];
    /* 0x70C8 */ be<f32> m6E84;
    /* 0x70CC */ be<u8> m6E88;
    u8 _70CD[0x70D8 - 0x70CD];
};
WWHD_OFFSET(btd_class, m6E7C, 0x70C0);
WWHD_OFFSET(btd_class, m6E84, 0x70C8);
WWHD_OFFSET(btd_class, m6E88, 0x70CC);
WWHD_SIZE(btd_class, 0x70D8);
enum : s16 { fpcNm_BTD_e = 234 };

struct dMagma_BindingHD {
    u8 _opaque[0x1C];
};
WWHD_SIZE(dMagma_BindingHD, 0x1C);
struct dMagma_MeshBufferHD {
    /* 0x000 */ be<u32> mpData;
    /* 0x004 */ u8 mVertexBuffer[0x154];
    /* 0x158 */ u8 mBinding[0xF4];
    /* 0x24C */ be<u32> mAllocationCount;
    /* 0x250 */ be<u32> mpAllocation;
};
WWHD_OFFSET(dMagma_MeshBufferHD, mVertexBuffer, 4);
WWHD_OFFSET(dMagma_MeshBufferHD, mBinding, 0x158);
WWHD_OFFSET(dMagma_MeshBufferHD, mAllocationCount, 0x24C);
WWHD_OFFSET(dMagma_MeshBufferHD, mpAllocation, 0x250);
WWHD_SIZE(dMagma_MeshBufferHD, 0x254);
struct dMagma_MeshPairHD {
    /* 0x000 */ dMagma_MeshBufferHD mBuffers[2];
    u8 _4A8[0x10];
    /* 0x4B8 */ be<u32> mpManagedData;
};
WWHD_OFFSET(dMagma_MeshPairHD, mpManagedData, 0x4B8);
WWHD_SIZE(dMagma_MeshPairHD, 0x4BC);
struct dMagma_OwnedVtableHD {
    u8 _00[0xC];
    gfn destroy;
};
WWHD_OFFSET(dMagma_OwnedVtableHD, destroy, 0xC);
WWHD_SIZE(dMagma_OwnedVtableHD, 0x10);
struct dMagma_OwnedObjectHD {
    u8 _00[0xF0];
    /* 0xF0 */ gptr<dMagma_OwnedVtableHD> mVtable;
};
WWHD_OFFSET(dMagma_OwnedObjectHD, mVtable, 0xF0);
WWHD_SIZE(dMagma_OwnedObjectHD, 0xF4);
struct dMagma_OwnedArrayHD {
    be<s32> mCount;
    be<u32> mpObjects;
};
WWHD_OFFSET(dMagma_OwnedArrayHD, mpObjects, 4);
WWHD_SIZE(dMagma_OwnedArrayHD, 8);
struct dMagma_OwnedResourceHD {
    be<u32> mpResource;
    dMagma_OwnedArrayHD mArrays[2];
};
WWHD_OFFSET(dMagma_OwnedResourceHD, mArrays, 4);
WWHD_SIZE(dMagma_OwnedResourceHD, 0x14);
struct dMagma_ResourceEntryHD {
    u8 _00[0x10];
    /* 0x10 */ dMagma_BindingHD mBindings[2];
    u8 _48[0x23C - 0x48];
};
WWHD_OFFSET(dMagma_ResourceEntryHD, mBindings, 0x10);
WWHD_SIZE(dMagma_ResourceEntryHD, 0x23C);
struct dMagma_ResourceListHD {
    be<s32> mCount;
    be<u32> mpEntries;
    u8 _08[4];
};
WWHD_OFFSET(dMagma_ResourceListHD, mpEntries, 4);
WWHD_SIZE(dMagma_ResourceListHD, 0xC);
struct dMagma_BallVtableHD {
    u8 _00[0x24];
    gfn setup;
};
WWHD_OFFSET(dMagma_BallVtableHD, setup, 0x24);
WWHD_SIZE(dMagma_BallVtableHD, 0x28);
struct dMagma_ball_c {
    u8 _00[4];
    /* 0x004 */ be<s32> mOwnedResourceCount;
    /* 0x008 */ be<u32> mpOwnedResources;
    /* 0x00C */ dMagma_MeshPairHD mMeshPair;
    u8 _4C8[4];
    /* 0x4CC */ u8 mVertexDescriptor[0x18];
    /* 0x4E4 */ dMagma_ResourceListHD mResourceList;
    /* 0x4F0 */ u8 mFirstMaterial[0x10];
    /* 0x500 */ dMagma_BindingHD mFirstBindings[2];
    u8 _538[0x598 - 0x538];
    /* 0x598 */ u8 mSecondMaterial[0x10];
    /* 0x5A8 */ dMagma_BindingHD mSecondBindings[2];
    u8 _5E0[0xA20 - 0x5E0];
    /* 0xA20 */ u8 mFirstShape[0x198];
    /* 0xBB8 */ u8 mSecondShape[0x198];
    /* 0xD50 */ cXyz mPos;
    /* 0xD5C */ be<f32> mScale;
    /* 0xD60 */ be<f32> mBaseY;
    /* 0xD64 */ be<s16> mWave;
    /* 0xD66 */ be<u8> mTimer;
    u8 _D67;
    /* 0xD68 */ Mtx34 mPosMtx;
    /* 0xD98 */ Mtx34 mTexProjMtx;
    u8 _DC8[4];
    /* 0xDCC */ gptr<dMagma_BallVtableHD> mVtable;
};
WWHD_OFFSET(dMagma_ball_c, mOwnedResourceCount, 4);
WWHD_OFFSET(dMagma_ball_c, mpOwnedResources, 8);
WWHD_OFFSET(dMagma_ball_c, mMeshPair, 0xC);
WWHD_OFFSET(dMagma_ball_c, mVertexDescriptor, 0x4CC);
WWHD_OFFSET(dMagma_ball_c, mResourceList, 0x4E4);
WWHD_OFFSET(dMagma_ball_c, mFirstMaterial, 0x4F0);
WWHD_OFFSET(dMagma_ball_c, mFirstBindings, 0x500);
WWHD_OFFSET(dMagma_ball_c, mSecondMaterial, 0x598);
WWHD_OFFSET(dMagma_ball_c, mSecondBindings, 0x5A8);
WWHD_OFFSET(dMagma_ball_c, mFirstShape, 0xA20);
WWHD_OFFSET(dMagma_ball_c, mSecondShape, 0xBB8);
WWHD_OFFSET(dMagma_ball_c, mPos, 0xD50);
WWHD_OFFSET(dMagma_ball_c, mScale, 0xD5C);
WWHD_OFFSET(dMagma_ball_c, mBaseY, 0xD60);
WWHD_OFFSET(dMagma_ball_c, mWave, 0xD64);
WWHD_OFFSET(dMagma_ball_c, mTimer, 0xD66);
WWHD_OFFSET(dMagma_ball_c, mPosMtx, 0xD68);
WWHD_OFFSET(dMagma_ball_c, mTexProjMtx, 0xD98);
WWHD_OFFSET(dMagma_ball_c, mVtable, 0xDCC);
WWHD_SIZE(dMagma_ball_c, 0xDD0);
struct dMagma_ballBoss_c : dMagma_ball_c {
    /* 0xDD0 */ be<s16> field_0x7C;
    u8 _DD2[2];
    void calc(f32 param_1, u8 param_2, s32 param_3);
    void update();
    void setup(f32 param_1, u8 param_2, s32 param_3);
};
WWHD_OFFSET(dMagma_ballBoss_c, field_0x7C, 0xDD0);
WWHD_SIZE(dMagma_ballBoss_c, 0xDD4);
struct dMagma_HeapVtableHD {
    u8 _00[0x3C];
    gfn free;
};
WWHD_OFFSET(dMagma_HeapVtableHD, free, 0x3C);
WWHD_SIZE(dMagma_HeapVtableHD, 0x40);
struct dMagma_HeapHD {
    u8 _00[0xC];
    gptr<dMagma_HeapVtableHD> mVtable;
};
WWHD_OFFSET(dMagma_HeapHD, mVtable, 0xC);
WWHD_SIZE(dMagma_HeapHD, 0x10);

// HD: adapters retain the verified external library ABI; library layouts stay private to this TU.
void dMagma_prepareBallHD(dMagma_ball_c*);
void PSMTXConcatHD(Mtx34*, Mtx34*, Mtx34*);
u32 dMagma_searchActorHD(u32, u32);
void dMagma_destroyBindingHD(void*);
void dMagma_resetMeshBindingHD(void*);
void dMagma_destroyMeshBindingHD(void*, u32);
void dMagma_destroyVertexBufferHD(void*, u32);
void dMagma_destroyShapeHD(void*, u32);
void dMagma_destroyMaterialHD(void*, u32);
void dMagma_destroyResourceListHD(void*, u32);
void dMagma_destroyVertexDescriptorHD(void*, u32);
void dMagma_destroyArrayHD(void*, u32, u32, u32, u32, u32);
dMagma_HeapHD* dMagma_findHeapHD(u32, u32);
void dMagma_deleteHD(void*);
