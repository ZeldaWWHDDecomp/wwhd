// Shader command blocks and their GHS inline-array element layouts.
#pragma once
#include "wwhd.h"
#include "sead/FileDevice.h"
namespace agl::environment {
struct VertexAttribute {
    be<u32> mLocation; sead::SafeText mName; be<u16> mStream; be<u16> mReserved;
    static VertexAttribute* construct(VertexAttribute* attribute);
};
struct UniformBinding {
    sead::SafeText mName; be<s16> mIndices[4];
    static UniformBinding* construct(UniformBinding* binding);
};
struct InlineWordList { be<u32> mCount,mCapacity; gptr<be<u32>> mEntries; be<u32> mStorage[16]; };
struct CommandList { u8 mBeforeStorage[0xC]; gptr<void> mStorage; be<u32> mCount; u8 mTail[0x14]; };
struct DisplayCommandState {
    be<u8> mActive; u8 mPadding[3]; gptr<void> mSlots[16]; CommandList mList;
    void reset(); void bindStreams();
    void buildAttributes(void* resource,const VertexAttribute* attributes,s32 count); static DisplayCommandState* construct(DisplayCommandState* state);
    void setStorage(void* storage,u32 bytes);
};
struct PixelCommandState {
    UniformBinding mBindings[16]; InlineWordList mActiveBindings; be<u8> mActive; u8 mTail[3];
    static PixelCommandState* construct(PixelCommandState* state); void reset();
    void append(u32 key,const UniformBinding* binding); void bindSamplerStages(void* owner);
    void selectSamplerBindings(void* descriptor,void* material,u32 useOverrides,u32 includeDefaults);
};
struct ProgramInstance {
    gptr<void> mProgram,mResource; sead::SafeText mName;
    be<s16> mVertexIndex,mPixelIndex,mGeometryIndex; be<u16> mPassIndex;
    DisplayCommandState mDisplay; PixelCommandState mPixel; gptr<void> mStorage;
    static ProgramInstance* construct(ProgramInstance* instance);
    void reset(); void selectProgramName(void* context,void* descriptor,const be<u8>* name);
    void selectOverride(void* descriptor,void* selection,void* material,const be<u8>* name);
    void selectDefault(void* descriptor,void* selection,void* material,const be<u8>* name);
    void bindUniformBlocks(void* block,u32 bindingParameter); static u32 storageBytes(); void setStorage(void* storage); void initializeStorage(void* heap);
};
}
WWHD_SIZE(agl::environment::VertexAttribute,16);
WWHD_SIZE(agl::environment::UniformBinding,16);
WWHD_OFFSET(agl::environment::InlineWordList,mStorage,12);
WWHD_SIZE(agl::environment::InlineWordList,0x4C);
WWHD_OFFSET(agl::environment::CommandList,mStorage,0xC);
WWHD_OFFSET(agl::environment::CommandList,mCount,0x10);
WWHD_SIZE(agl::environment::CommandList,0x28);
WWHD_OFFSET(agl::environment::DisplayCommandState,mSlots,4);
WWHD_OFFSET(agl::environment::DisplayCommandState,mList,0x44);
WWHD_SIZE(agl::environment::DisplayCommandState,0x6C);
WWHD_OFFSET(agl::environment::PixelCommandState,mActiveBindings,0x100);
WWHD_OFFSET(agl::environment::PixelCommandState,mActive,0x14C);
WWHD_SIZE(agl::environment::PixelCommandState,0x150);
WWHD_OFFSET(agl::environment::ProgramInstance,mDisplay,0x18);
WWHD_OFFSET(agl::environment::ProgramInstance,mPixel,0x84);
WWHD_OFFSET(agl::environment::ProgramInstance,mStorage,0x1D4);
WWHD_SIZE(agl::environment::ProgramInstance,0x1D8);
