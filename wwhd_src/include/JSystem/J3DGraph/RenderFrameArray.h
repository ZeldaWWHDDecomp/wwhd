// J3D HD renderer: frame array and uniform group layouts.
// Names are our own; written from the WWHD binary.
#pragma once
#include "JSystem/J3DGraph/RenderMaterialUniforms.h"
#include "JSystem/J3DGraph/RenderUniformBindings.h"
#include "sead/HeapRegistry.h"
namespace j3d {
struct RenderFrameArray : RenderStorageSpan {
    be<u8> mArrayReady;
    u8 mPadding[3];
    RenderFrameUniforms* frames() const { return static_cast<RenderFrameUniforms*>(mStorage.get()); }
    static RenderFrameArray* construct(RenderFrameArray* storage);
    static void destroy(RenderFrameArray* storage, u32 flags);
    void prepare(s32 count, sead::RegistryHeap* heap);
    void advance(u32 index);
    void advanceAll();
};
// The group as far as these functions reach it.
struct RenderUniformGroup_l : RenderFrameArray {
    RenderMatrixUniforms mMatrixUniforms;
    RenderMaterialUniforms mMaterialUniforms;
    be<u8> mGroupReady;
    u8 mPadding[3];
    void prepare(s32 count, sead::RegistryHeap* heap);
    void advance(u32 index);
    void bind(const RenderUniformLocations_l* locations, u32 index);
};
}
WWHD_OFFSET(j3d::RenderFrameArray, mArrayReady, 8);
WWHD_SIZE(j3d::RenderFrameArray, 12);
WWHD_OFFSET(j3d::RenderUniformGroup_l, mMatrixUniforms, 0xC);
WWHD_OFFSET(j3d::RenderUniformGroup_l, mMaterialUniforms, 0xB4);
WWHD_OFFSET(j3d::RenderUniformGroup_l, mGroupReady, 0x418);
WWHD_SIZE(j3d::RenderUniformGroup_l, 0x41C);
using RenderFloat3Construct = void (*)(j3d::RenderFloat3*);
using RenderFloat4Construct = void (*)(j3d::RenderFloat4*);
void initializeRenderFloat3(j3d::RenderFloat3* storage);
void initializeRenderFloat4(j3d::RenderFloat4* storage);
void constructRenderFloat3Array(void* storage, u32 count, u32 size, RenderFloat3Construct construct);
void constructRenderFloat4Array(void* storage, u32 count, u32 size, RenderFloat4Construct construct);
void prepareRenderFrameStorage(void* storage, u32 size);
// Function words of the registry heap's allocate/release slots.
static_assert(offsetof(sead::RegistryHeapDispatch, mAllocate) + offsetof(sead::VtableSlot, mFunction) == 0x34);
static_assert(offsetof(sead::RegistryHeapDispatch, mRelease) + offsetof(sead::VtableSlot, mFunction) == 0x3C);
static_assert(offsetof(sead::RegistryHeap, mObject) + offsetof(sead::ObjectHeader, mTable) == 0xC);
