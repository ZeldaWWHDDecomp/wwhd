/* HD line sorting, weighted vertices and generated animation helpers. Local source only.
 * HD: render packets and material virtual interfaces are enlarged from their GameCube layouts. */
#pragma once
#include "bindings.h"
struct HdLineMaterial {
    /* 0x0 */ gptr<HdLineMaterial> mpNextLineMat;
    u8 padding[0x12C];
    /* 0x130 */ gptr<gfn> mVtable;
};
struct HdLineSortPacket {
    u8 padding[0xC];
    /* 0xC */ gptr<void> mVtable;
    u8 padding2[0x88];
    /* 0x98 */ gptr<HdLineMaterial> mp3DlineMat;
    void reset();
    void draw(void*);
    void setMat(HdLineMaterial*);
};
struct HdWeightVertex {
    cXyz mPosition;
    be<f32> mWeights[8];
    be<s8> mBones[8];
};
struct HdWeightInput { be<u32> mCount; HdWeightVertex mVertices[1]; };
struct HdWeightResource { u8 padding[4]; be<u16> mFlags; be<u8> mMatrixCount; u8 padding2[0x11]; u8 mMatrixResource; };
struct HdWeightModel {
    u8 padding[0x14];
    u8 mMatrixTarget;
    u8 padding2[0x17];
    gptr<HdWeightResource> mResource;
    u8 padding3[0x3C];
    be<u8> mBufferIndex;
};
struct HdVertexWeight {
    gptr<HdWeightInput> mpInput;
    gptr<cXyz> mpOutput;
    u32 init(HdWeightInput*);
    void calc(HdWeightModel*);
};
struct HdTailPacket {
    u8 padding[0xC]; gptr<gfn> mVtable; u8 padding2[0x90];
};
struct HdTailPacketArray {
    u8 padding[0xC]; gptr<void> mVtable;
    u8 padding2[4]; gptr<void> mMemberVtable;
    be<u32> mCount; gptr<HdTailPacket> mpPackets;
};
struct HdTailShape { u8 padding[0x34]; gptr<void> mVtable; u8 padding2[0xC]; be<u32> mBuffer; u8 padding3[0x20]; };
struct HdTailHeap { u8 padding[0xC]; gptr<gfn> mVtable; };
struct HdTailHeapPointer { gptr<HdTailHeap> mpHeap; };
struct HdTailDrawState { u8 padding[284]; };
struct HdTailGlobals { be<u32> mFirst, mSecond, mThird, mFourth; };
struct HdTailRange { be<f32> mLower, mUpper; u8 padding[1]; be<u8> mSignedInitialized, mUnsignedInitialized; };
WWHD_SIZE(HdLineSortPacket, 0x9C);
WWHD_OFFSET(HdLineMaterial, mVtable, 0x130);
WWHD_SIZE(HdWeightVertex, 0x34);
WWHD_OFFSET(HdWeightResource, mMatrixResource, 0x18);
WWHD_OFFSET(HdWeightModel, mResource, 0x2C);
WWHD_OFFSET(HdWeightModel, mBufferIndex, 0x6C);
WWHD_SIZE(HdVertexWeight, 8);
WWHD_SIZE(HdTailPacket, 0xA0);
WWHD_SIZE(HdTailPacketArray, 0x20);
WWHD_SIZE(HdTailShape, 0x68);
WWHD_SIZE(HdLineMaterial, 0x134);
WWHD_SIZE(HdWeightInput, 0x38);
WWHD_SIZE(HdWeightResource, 0x19);
WWHD_SIZE(HdWeightModel, 0x6D);
WWHD_SIZE(HdTailHeap, 0x10);
WWHD_SIZE(HdTailHeapPointer, 4);
WWHD_SIZE(HdTailDrawState, 284);
WWHD_SIZE(HdTailGlobals, 16);
WWHD_SIZE(HdTailRange, 11);
WWHD_OFFSET(HdTailRange, mSignedInitialized, 9);
WWHD_OFFSET(HdTailRange, mUnsignedInitialized, 10);
void hdTailPacketConstruct(void*);
void* hdTailAllocate(u32);
void* hdTailAllocateAligned(u32, u32);
void hdTailDelete(void*);
void hdTailRemovePacket(void*, void*, u32);
void hdTailEntryPacket(void*, void*, u32);
void hdTailDrawStateApply(HdTailDrawState*);
void hdTailUpdateMatrix(void*, u32);
Mtx34* hdTailGetMatrices(void*, u32);
void hdTailTransform(Mtx34*, cXyz*, cXyz*);
void hdTailRegister(void*);
void hdTailInitSigned(be<u8>*);
void hdTailInitUnsigned(be<u8>*);
HdTailHeap* hdTailFindHeap(HdTailHeap*, void*);
void hdTailPacketArrayDestroy(HdTailPacketArray*, u32);
void hdTailShapeConstruct(HdTailShape*);
