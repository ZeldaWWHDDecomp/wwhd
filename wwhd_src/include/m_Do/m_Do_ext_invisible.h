#pragma once
#include "bindings.h"

// HD: renderer objects use HD resource arrays and draw descriptors, not GC J3D layouts.
struct InvisibleDrawInfo;
struct InvisibleMaterial;
struct InvisibleMaterialMethods { u8 mPrefix[0x2C]; gfn draw; };
struct InvisibleShapeIndex { u8 mPrefix[0xC]; be<u16> mIndex; };
struct InvisibleShape { gptr<InvisibleShapeIndex> mpShape; be<u8> mEnabled; };
struct InvisibleMesh { u8 mPrefix[4]; gptr<InvisibleMesh> mpNext; gptr<InvisibleShape> mpShape; };
struct InvisibleJoint { u8 mPrefix[0x10]; gptr<InvisibleMesh> mpMesh; u8 mSuffix[8]; };
struct InvisibleModelData { u8 mPrefix[4]; be<u32> mJointCount; gptr<InvisibleJoint> mpJoints; };
struct InvisibleMatrixBuffer { u8 mPrefix[4]; be<u16> mFlags; u8 mPadding[0xA]; gptr<Mtx34> mpAnmMtx; };
struct InvisibleModel {
    u8 mPrefix[0x2C]; gptr<InvisibleMatrixBuffer> mpMtxBuffer;
    u8 mBeforeDrawState[0x30]; u8 mDrawState[0x4C]; gptr<InvisibleModelData> mpModelData;
    u8 mBeforeMaterials[0x78]; be<u32> mMaterialCount; gptr<InvisibleMaterial> mpMaterials;
    be<u32> mShapeCount; gptr<be<u16>> mpShapeMaterialIds;
};
struct InvisibleMaterial { u8 mPrefix[0xC]; gptr<InvisibleMaterialMethods> mpMethods; u8 mSuffix[0x9C]; };
struct InvisibleJointRegistry { u8 mPrefix[8]; be<u16> mJointCount; };
struct InvisibleDrawState { u8 mPrefix[0xEC]; be<u32> mFlags; u8 mSuffix[0x2C]; };
struct InvisibleDrawInfo {
    be<u32> mMaterial; be<u32> mEnabled; be<u32> mUnknown[3]; gptr<void> mpModelState; be<u32> mUnknownTail[7];
    InvisibleDrawState mState;
};
struct InvisibleDrawArguments { be<u32> mMaterial; u8 mPrefix[8]; be<u32> mEnabled; };
struct InvisiblePacketMethods { u8 mPrefix[0xC]; gfn destroy; };
struct InvisiblePacketMember { gptr<void> mpOwner; be<u32> mVtable; };
struct mDoExt_invJntPacket { u8 mPrefix[0xC]; be<u32> mVtable; u8 mState[0x88]; gptr<InvisibleModel> mModel; be<u16> mJntNum; u8 mPadding[2]; };
struct InvisiblePacketArray { u8 mPrefix[0xC]; gptr<InvisiblePacketMethods> mpMethods; InvisiblePacketMember mMember; be<u32> mCount; gptr<mDoExt_invJntPacket> mpPackets; };
struct mDoExt_invisibleModel { gptr<InvisibleModel> mModel; gptr<InvisiblePacketArray> mpPackets; };
struct InvisibleManagerMethods { u8 mPrefix[0x34]; gfn allocate; };
struct InvisibleManager { u8 mPrefix[0xC]; gptr<InvisibleManagerMethods> mpMethods; };
struct InvisiblePlay { u8 mPrefix[0x5D30]; u8 mDrawList[0x48]; gptr<void> mpOpa; gptr<void> mpXlu; u8 mPadding[8]; gptr<void> mpXluMaskOff; };
struct InvisibleDrawBuffers { gptr<void> mpOpa; gptr<void> mpXlu; };
struct mDoExt_3Dline_c { gptr<void> mpSegments; gptr<void> mpSize; gptr<void> mPosArr; be<u32> mVtable; };
WWHD_SIZE(InvisibleJoint, 0x1C);
WWHD_OFFSET(InvisibleModel, mpModelData, 0xAC);
WWHD_OFFSET(InvisibleModel, mMaterialCount, 0x128);
WWHD_SIZE(InvisibleMaterial, 0xAC);
WWHD_SIZE(InvisibleDrawState, 0x11C);
WWHD_SIZE(InvisibleDrawInfo, 0x150);
WWHD_OFFSET(mDoExt_invJntPacket, mModel, 0x98);
WWHD_SIZE(mDoExt_invJntPacket, 0xA0);
WWHD_SIZE(InvisiblePacketArray, 0x20);
WWHD_OFFSET(InvisiblePlay, mpXluMaskOff, 0x5D88);
WWHD_SIZE(mDoExt_invisibleModel, 8);
WWHD_SIZE(mDoExt_3Dline_c, 0x10);
void InvisibleDrawState_construct(InvisibleDrawState*);
void InvisibleDrawState_apply(InvisibleDrawState*);
void InvisiblePacketArray_construct(InvisiblePacketArray*);
void InvisiblePacket_construct(mDoExt_invJntPacket*);
InvisibleJointRegistry* InvisibleModelData_getJointRegistry(InvisibleModelData*);
InvisibleManager* InvisibleRenderer_getManager(void*);
void InvisibleDrawBuffer_entryImm(void*, mDoExt_invJntPacket*, u32);
void InvisibleDrawList_entryZSort(void*, void*, mDoExt_invJntPacket*, cXyz*);
InvisiblePlay* Invisible_getPlay();
void InvisibleMorf_updateDL(void*);
void InvisibleModel_entry(mDoExt_invisibleModel*);
void InvisibleModel_clear(mDoExt_invisibleModel*);
InvisiblePacketMember* InvisiblePacketMember_construct(InvisiblePacketMember*, void*);
enum : u32 { InvisiblePacketMember_vtable = 0x10058DA0, InvisiblePacketOwner_vtable = 0x1005875C,
    InvisibleJointPacket_vtable = 0x10058D70, InvisibleLine_vtable = 0x10058E50 };

struct InvisibleRendererPointer { gptr<void> mpRenderer; };
WWHD_OFFSET(InvisibleMaterialMethods, draw, 0x2C);
WWHD_OFFSET(InvisibleShapeIndex, mIndex, 0xC);
WWHD_OFFSET(InvisibleShape, mEnabled, 4);
WWHD_OFFSET(InvisibleMesh, mpNext, 4);
WWHD_OFFSET(InvisibleMesh, mpShape, 8);
WWHD_SIZE(InvisibleModelData, 0xC);
WWHD_OFFSET(InvisibleMatrixBuffer, mFlags, 4);
WWHD_OFFSET(InvisibleMatrixBuffer, mpAnmMtx, 0x10);
WWHD_OFFSET(InvisibleJointRegistry, mJointCount, 8);
WWHD_OFFSET(InvisibleDrawInfo, mpModelState, 0x14);
WWHD_OFFSET(InvisibleDrawInfo, mState, 0x34);
WWHD_SIZE(InvisibleDrawArguments, 0x10);
WWHD_OFFSET(InvisiblePacketMethods, destroy, 0xC);
WWHD_SIZE(InvisiblePacketMember, 8);
WWHD_OFFSET(InvisibleManagerMethods, allocate, 0x34);
WWHD_OFFSET(InvisibleManager, mpMethods, 0xC);
WWHD_SIZE(InvisibleDrawBuffers, 8);
WWHD_SIZE(InvisibleRendererPointer, 4);
