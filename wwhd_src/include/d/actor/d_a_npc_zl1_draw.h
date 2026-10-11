/* Tetra drawing: private HD views of animation, render and collision state.
 * Shared actor layout remains in d_a_npc_zl1.h; these views cover HD-only fields. */
#pragma once
#include "SSystem/SComponent/c_m3d_g_pla.h"
#include "d/actor/d_a_npc_zl1.h"
namespace Zl1Draw {
enum { DSNAP_TYPE_NPC_ZL1 = 0x72, ID_AK_JN_HAMON00 = 0x33 };
// HD: the unified particle setter dispatches ship-tail effects by kind.
enum ParticleKind { PARTICLE_SHIP_TAIL = 5 };
struct AnimationVtable {
    /* 0x0000 */ u8 mPrefix[0x14];
    /* 0x0014 */ be<u32> mGetFrameMax;
};
struct Animation {
    /* 0x0000 */ u8 mPrefix[4];
    /* 0x0004 */ gptr<AnimationVtable> mpVtable;
};
struct BtpAnimation {
    /* 0x0000 */ u8 mPrefix[0x10];
    /* 0x0010 */ gptr<Animation> mpAnimation;
};
struct BtkAnimation {
    /* 0x0000 */ u8 mPrefix[0x68];
    /* 0x0068 */ gptr<Animation> mpAnimation;
};
struct MatrixBlock {
    /* 0x0000 */ u8 mPrefix[4];
    /* 0x0004 */ be<u16> mFlags;
    /* 0x0006 */ u8 mPadding[10];
    /* 0x0010 */ gptr<Mtx34> mpMtx;
};
struct Model {
    /* 0x0000 */ u8 mPrefix[0x2C];
    /* 0x002C */ gptr<MatrixBlock> mpMatrixBlock;
    /* 0x0030 */ u8 mPadding[0xB8 - 0x30];
    /* 0x00B8 */ gptr<daNpc_Zl1_c> mpActor;
};
struct Joint {
    /* 0x0000 */ u8 mPrefix[4];
    /* 0x0004 */ be<u16> mJointNo;
};
struct Ship : fopAc_ac_c {
    /* 0x03AC */ u8 mPadding[8];
    /* 0x03B4 */ gptr<mDoExt_McaMorf> mpMorf;
};
struct Ripple {
    /* 0x0000 */ be<u32> mVtable;
    /* 0x0004 */ gptr<void> mpEmitter;
    /* 0x0008 */ u8 mPadding[8];
    /* 0x0010 */ be<f32> mRate;
};
struct Emitter {
    /* 0x0000 */ u8 mPrefix[0x34];
    /* 0x0034 */ be<f32> mRate;
    /* 0x0038 */ u8 mPadding[0x58 - 0x38];
    /* 0x0058 */ be<f32> mSpread;
    /* 0x005C */ u8 mPadding2[4];
    /* 0x0060 */ be<u16> mLifeTime;
    /* 0x0062 */ u8 mPadding3[0x70 - 0x62];
    /* 0x0070 */ be<f32> mDirectionalSpeed;
};
struct PositionPacket {
    /* 0x0000 */ u8 mPrefix[0xC];
    /* 0x000C */ cXyz mPosition;
};
struct Play {
    /* 0x0000 */ u8 mPrefix[0x5B3C];
    /* 0x5B3C */ gptr<Ship> mpShip;
    /* 0x5B40 */ u8 mPadding[0x5D30 - 0x5B40];
    /* 0x5D30 */ PositionPacket mPositionPacket;
    /* 0x5D48 */ u8 mPadding2[0x5D60 - 0x5D48];
    /* 0x5D60 */ gptr<void> mpListXlu;
    /* 0x5D64 */ gptr<void> mpListP0;
    /* 0x5D68 */ gptr<void> mpListOpa;
    /* 0x5D6C */ gptr<void> mpListP1;
};
struct Shape {
    /* 0x0000 */ u8 mPrefix[4];
    /* 0x0004 */ be<u8> mVisible;
};
struct Material {
    /* 0x0000 */ u8 mPrefix[0xC];
    /* 0x000C */ be<u16> mIndex;
};
struct Mesh {
    /* 0x0000 */ gptr<Material> mpMaterial;
    /* 0x0004 */ gptr<Mesh> mpNext;
    /* 0x0008 */ gptr<Shape> mpShape;
};
struct MeshJoint {
    /* 0x0000 */ u8 mPrefix[0x10];
    /* 0x0010 */ gptr<Mesh> mpMesh;
};
struct ModelData {
    /* 0x0000 */ u8 mPrefix[0x38];
    /* 0x0038 */ gptr<void> mpBtp;
    /* 0x003C */ u8 mPadding[8];
    /* 0x0044 */ gptr<void> mpBtk;
};
struct SafeVtable {
    /* 0x0000 */ u8 mPrefix[0x14];
    /* 0x0014 */ be<u32> mAssureTermination;
};
struct String {
    /* 0x0000 */ gptr<be<u8>> mpString;
    /* 0x0004 */ gptr<SafeVtable> mpVtable;
};
struct ModelPointer {
    /* 0x0000 */ gptr<J3DModel> mpModel;
};
struct BufferPointer {
    /* 0x0000 */ gptr<void> mpBuffer;
};
struct Word {
    /* 0x0000 */ be<u32> mValue;
};
WWHD_SIZE(Ripple, 0x14);
WWHD_OFFSET(Ship, mpMorf, 0x3B4);
WWHD_OFFSET(Play, mpListXlu, 0x5D60);
WWHD_OFFSET(Emitter, mDirectionalSpeed, 0x70);
} // namespace Zl1Draw
/* dBgS_LinChk (stack object, HD layout; this TU's vtables) */
struct Zl1DrawLineCheck {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> mLineVtable;
    /* 0x14 */ u8 mPolyInfo[4]; /* cBgS_PolyInfo: poly index, bg index */
    /* 0x18 */ u8 _18[0x20 - 0x18];
    /* 0x20 */ be<u32> mCheckVtable;
    /* 0x24 */ u8 _24[0x58 - 0x24];
    /* 0x58 */ be<u32> mPolyPassVtable; /* dBgS_PolyPassChk */
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> mGroupPassVtable; /* dBgS_GrpPassChk */
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(Zl1DrawLineCheck, 0x6C);

// HD: roof query with an embedded background-check base.
struct Zl1DrawRoofCheck {
    /* 0x00 */ u8 _00[0xC];
    /* 0x0C */ be<u32> mRoofVtable;
    /* 0x10 */ u8 mCheckBase[0x20 - 0x10];
    /* 0x20 */ be<u32> mCheckVtable;
    /* 0x24 */ be<u32> mObjectVtable;
    /* 0x28 */ be<u8> mFlags;
    /* 0x29 */ u8 _29[0x30 - 0x29];
    /* 0x30 */ be<u32> mGroupPassVtable;
    /* 0x34 */ u8 _34[4];
    /* 0x38 */ cXyz mPos;
    /* 0x44 */ u8 _44[8];
};
WWHD_SIZE(Zl1DrawRoofCheck, 0x4C);
