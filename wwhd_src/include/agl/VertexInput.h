// AGL vertex input records; layouts are inferred from the assigned WWHD instructions.
#pragma once
#include "agl/RenderRecording.h"
namespace agl {
struct VertexFormatInfo {
    be<u8> mChannelBits[4];
    be<u8> mChannelOrder[4];
    be<u8> mByteCount;
    be<u8> mChannelCount;
    u8 mPackingData[7];
};
struct VertexPackingInfo { be<u32> mMode[4]; };
struct VertexBufferElement {
    be<u32> mFormat;
    be<u32> mOffset;
    be<u8> mConversion;
    be<u8> mFetchMode;
    u8 mAlignment[2];
    be<u32> mComponentMask;
    be<u32> mByteSwapped;
};
// Borrowed prefix consumed by the input-binding API. No complete buffer-owner extent is claimed.
struct VertexBufferPrefix_l {
    VertexBufferElement mElements[16];
    gptr<void> mData;
    be<u32> mStride;
    u8 mOpaqueTail[4];
    be<u32> mBytes;
};
struct VertexInputSet {
    VertexAttributeBinding mBindings[16];
    be<s32> mBufferCount;
    gptr<gptr<VertexBufferPrefix_l>> mBuffers;
    be<u8> mPrepared;
    be<u8> mInitialized;
    u8 mAlignment[2];
    u8 mFetchShader[32];
    gptr<void> mFetchProgram;
    gptr<sead::DisposerVtable> mVtable;
    static VertexInputSet* create(VertexInputSet* storage);
    void release();
    void destroy(u32 flags);
    void clear();
    void allocate(s32 count,sead::RegistryHeap* heap);
    s32 removeBinding(VertexAttributeBinding* binding);
    s32 assignBinding(VertexAttributeBinding* binding,VertexBufferPrefix_l* source,s32 attribute);
    void setBinding(s32 location,VertexBufferPrefix_l* source,s32 attribute);
    void prepare();
    void bind();
};
u32 vertexFormatBytes(u32 format);
u32 vertexFormatChannels(u32 format);
u32 vertexFormatChannelBits(u32 format,u32 channel);
u32 vertexFormatChannelOrder(u32 format,u32 channel);
u32 vertexFormatPacking(u32 format,u32 component);
}
WWHD_SIZE(agl::VertexFormatInfo,17);
WWHD_OFFSET(agl::VertexFormatInfo,mByteCount,8);
WWHD_OFFSET(agl::VertexFormatInfo,mChannelCount,9);
WWHD_SIZE(agl::VertexPackingInfo,16);
WWHD_SIZE(agl::VertexBufferElement,20);
WWHD_OFFSET(agl::VertexBufferElement,mFetchMode,9);
WWHD_OFFSET(agl::VertexBufferElement,mComponentMask,12);
WWHD_OFFSET(agl::VertexBufferElement,mByteSwapped,16);
WWHD_OFFSET(agl::VertexBufferPrefix_l,mData,0x140);
WWHD_OFFSET(agl::VertexBufferPrefix_l,mStride,0x144);
WWHD_OFFSET(agl::VertexBufferPrefix_l,mBytes,0x14C);
WWHD_SIZE(agl::VertexInputSet,0xF4);
WWHD_OFFSET(agl::VertexInputSet,mBufferCount,0xC0);
WWHD_OFFSET(agl::VertexInputSet,mBuffers,0xC4);
WWHD_OFFSET(agl::VertexInputSet,mPrepared,0xC8);
WWHD_OFFSET(agl::VertexInputSet,mInitialized,0xC9);
WWHD_OFFSET(agl::VertexInputSet,mFetchShader,0xCC);
WWHD_OFFSET(agl::VertexInputSet,mFetchProgram,0xEC);
WWHD_OFFSET(agl::VertexInputSet,mVtable,0xF0);
