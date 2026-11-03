// HD uniform-buffer bindings used by JPA draw passes.
#pragma once
#include "wwhd.h"
namespace JPADrawExtension {
struct JPAUniformBinding_l {
    u8 mNameAndType[0xC];
    be<s16> mVertexLocation,mPixelLocation,mGeometryLocation;
    u8 mTail[2];
};
struct JPAUniformBufferSlot_l {
    be<u32> mFlags,mSize,mCapacity;
    gptr<void> mData;
    u8 mTail[0xC];
};
struct JPAUniformBuffer_l {
    u8 mPrefix[0x10];
    JPAUniformBufferSlot_l mSlots[2];
    u8 mBeforeCurrent[4];
    be<u32> mCurrent;
};
struct JPADynamicUniform_l { be<u32> mFlags; gptr<JPAUniformBuffer_l> mBuffer; be<u32> mState; };
struct JPADrawViewState_l { u8 mPrefix[0x240]; be<u32> mViewWords[16]; };
struct JPATextureDrawContext_l {
    u8 mPrefix[0x30];
    gptr<JPAUniformBuffer_l> mUniformBuffer;
};
WWHD_SIZE(JPAUniformBinding_l,0x14);
WWHD_SIZE(JPAUniformBufferSlot_l,0x1C);
WWHD_OFFSET(JPAUniformBuffer_l,mCurrent,0x4C);
WWHD_OFFSET(JPATextureDrawContext_l,mUniformBuffer,0x30);
}
