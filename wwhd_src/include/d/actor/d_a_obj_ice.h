/* Generic ice block. WWHD actor base is0x3AC; GameCube fields retain their order,
 * with an eight-byte GHS member pointer and two additional actor-owned effect matrices. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_actor_mng.h"

struct IceAction {
    be<s16> delta, index;
    be<u32> target;
};
WWHD_SIZE(IceAction, 8);
struct daObjIce_c : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ gptr<dBgW> mBgw;
    /* 0x528 */ Mtx34 m40C;
    /* 0x558 */ IceAction m43C;
    /* 0x560 */ be<u8> m448;
    /* 0x561 */ u8 _561[3];
    /* 0x564 */ be<f32> m44C;
    /* 0x568 */ be<f32> m450;
    /* 0x56C */ be<f32> m454;
    /* 0x570 */ be<s32> m458;
    /* 0x574 */ be<s32> m45C;
    /* 0x578 */ Mtx34 mEffectSmall;
    /* 0x5A8 */ Mtx34 mEffectLarge;
    u8 chk_appear();
    BOOL create_heap(); // HD: full-width return
    void set_mtx();
    void init_mtx();
    s32 _create();
    BOOL _delete();
    BOOL _execute();
    void wait_act_proc();
    void fade_out_retire_act_proc();
    BOOL _draw();
};
WWHD_OFFSET(daObjIce_c, mPhase, 0x3AC);
WWHD_OFFSET(daObjIce_c, mModel, 0x3B4);
WWHD_OFFSET(daObjIce_c, mStts, 0x3B8);
WWHD_OFFSET(daObjIce_c, mCyl, 0x3F4);
WWHD_OFFSET(daObjIce_c, mBgw, 0x524);
WWHD_OFFSET(daObjIce_c, m40C, 0x528);
WWHD_OFFSET(daObjIce_c, m43C, 0x558);
WWHD_OFFSET(daObjIce_c, m448, 0x560);
WWHD_OFFSET(daObjIce_c, m44C, 0x564);
WWHD_OFFSET(daObjIce_c, m450, 0x568);
WWHD_OFFSET(daObjIce_c, m454, 0x56C);
WWHD_OFFSET(daObjIce_c, m458, 0x570);
WWHD_OFFSET(daObjIce_c, m45C, 0x574);
WWHD_OFFSET(daObjIce_c, mEffectSmall, 0x578);
WWHD_OFFSET(daObjIce_c, mEffectLarge, 0x5A8);
WWHD_SIZE(daObjIce_c, 0x5D8);

struct IceColor {
    be<f32> r, g, b, a;
};
WWHD_SIZE(IceColor, 16);
struct IceRgba {
    be<u8> r, g, b, a;
};
WWHD_SIZE(IceRgba, 4);
struct IceTevVtable {
    u8 _00[0x3C];
    gfn setKonstColor;
    u8 _40[12];
    gfn getKonstColor;
};
WWHD_OFFSET(IceTevVtable, setKonstColor, 0x3C);
WWHD_OFFSET(IceTevVtable, getKonstColor, 0x4C);
WWHD_SIZE(IceTevVtable, 0x50);
struct IceTevBlock {
    u8 _00[4];
    gptr<IceTevVtable> mVtable;
};
WWHD_OFFSET(IceTevBlock, mVtable, 4);
WWHD_SIZE(IceTevBlock, 8);
struct IceMaterial {
    u8 _00[4];
    gptr<IceMaterial> mNext;
    gptr<J3DShape> mShape;
    u8 _0C[12];
    gptr<IceTevBlock> mTevBlock;
    u8 _1C[0x84];
    be<u32> mFlags;
};
WWHD_OFFSET(IceMaterial, mNext, 4);
WWHD_OFFSET(IceMaterial, mShape, 8);
WWHD_OFFSET(IceMaterial, mTevBlock, 0x18);
WWHD_OFFSET(IceMaterial, mFlags, 0xA0);
WWHD_SIZE(IceMaterial, 0xA4);
struct IceJointTree {
    u8 _00[8];
    be<u16> mJointNum;
};
WWHD_OFFSET(IceJointTree, mJointNum, 8);
WWHD_SIZE(IceJointTree, 0xA);
struct IceJoint {
    u8 _00[0x10];
    gptr<IceMaterial> mMesh;
};
WWHD_OFFSET(IceJoint, mMesh, 0x10);
WWHD_SIZE(IceJoint, 0x14);
struct IceShaderVariable {
    u8 _00[4];
    be<s32> mLocation;
    u8 _08[4];
    be<u16> mLinked;
    u8 _0E[6];
};
WWHD_OFFSET(IceShaderVariable, mLocation, 4);
WWHD_OFFSET(IceShaderVariable, mLinked, 0xC);
WWHD_SIZE(IceShaderVariable, 0x14);
struct IceShaderHeader {
    u8 _00[0x34];
    be<s32> mRelativeVariables;
};
WWHD_OFFSET(IceShaderHeader, mRelativeVariables, 0x34);
WWHD_SIZE(IceShaderHeader, 0x38);
struct IceShaderEntry {
    gptr<IceShaderHeader> mHeader;
    be<u16> mDirtyFlags;
    u8 _06[6];
    gptr<be<u32>> mDirtyWords;
    u8 _10[0x2C];
};
WWHD_OFFSET(IceShaderEntry, mDirtyFlags, 4);
WWHD_OFFSET(IceShaderEntry, mDirtyWords, 0xC);
WWHD_SIZE(IceShaderEntry, 0x3C);
struct IceTextureInfo {
    be<u32> mInfo;
};
WWHD_SIZE(IceTextureInfo, 4);
struct IceModel {
    u8 _00[0x2A];
    be<u16> mMaterialNum;
    u8 _2C[8];
    gptr<IceShaderEntry> mShaderEntries;
};
WWHD_OFFSET(IceModel, mMaterialNum, 0x2A);
WWHD_OFFSET(IceModel, mShaderEntries, 0x34);
WWHD_SIZE(IceModel, 0x38);
struct IceEmitter {
    u8 _000[0x220];
    cXyz mGlobalDynamicsScale;
    u8 _22C[12];
    cXyz mGlobalParticleScale;
};
WWHD_OFFSET(IceEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(IceEmitter, mGlobalParticleScale, 0x238);
WWHD_SIZE(IceEmitter, 0x244);
struct IceCamera {
    u8 _000[0xDC];
    cXyz mEye;
};
WWHD_OFFSET(IceCamera, mEye, 0xDC);
WWHD_SIZE(IceCamera, 0xE8);
struct IceShapeVtables {
    u8 _00[0x110];
    be<u32> mAabVtable, mShapeVtable;
};
WWHD_OFFSET(IceShapeVtables, mAabVtable, 0x110);
WWHD_OFFSET(IceShapeVtables, mShapeVtable, 0x114);
WWHD_SIZE(IceShapeVtables, 0x118);
struct IceHeaderData {
    be<u32> words[4];
};
WWHD_SIZE(IceHeaderData, 16);
enum { dRes_INDEX_IKORI_BDL_VICE_e = 4, dRes_INDEX_IKORI_DZB_VICE_e = 7 };
enum : u32 {
    ICE_ACTOR_VTABLE = 0x1002B11C,
    ICE_AAB_VTABLE = 0x1002B10C,
    ICE_SHAPE_BASE_VTABLE = 0x100015A8,
    ICE_HIT_VTABLE = 0x1004B108,
    ICE_CYL_VTABLE = 0x1004B150,
    ICE_SHAPE_CYL_VTABLE = 0x1004B160
};
void iceXYZrotM(Mtx34 *, s16, s16, s16);
dBgW *iceNewBackground(cBgD_t *, u32, Mtx34 *);
void iceConvertColor(IceColor *, IceColor *, f32);
IceColor *iceGetUniform(be<u32> *, s32);
void iceConstructGObj(dCcD_Cyl *);
void iceConstructCylinder(cM3dGCyl *);
void *iceAllocateMatrix(u32);
void iceReadColor(IceRgba *, IceShaderEntry *, s32);
IceJointTree *iceGetJointTree(J3DModelData *);
IceCamera *iceGetCamera();
void iceSubtractVector(cXyz *, cXyz *, cXyz *);
void iceLightVector(cXyz *, cXyz *, cXyz *);
void iceHalfAngle(cXyz *, cXyz *, cXyz *);
void iceLookAt(Mtx34 *, cXyz *, cXyz *, cXyz *);
void iceScaleMatrix(Mtx34 *, f32, f32, f32);
void iceConcatMatrix(Mtx34 *, Mtx34 *, Mtx34 *);
IceTextureInfo *iceGetTextureInfo(IceShaderEntry *, u32, be<s32> *);
gptr<Mtx34> *iceGetTextureMatrix(IceShaderEntry *, u32);
void iceSetEvent(dEvent_manager_c *, const char *, s32);
s32 iceCheckEvent(dEvent_manager_c *, const char *);
s32 iceGetReverb(s32);
void iceDestroyActor(fopAc_ac_c *, s32);

void iceInitCharacter(be<u8> *);
void iceInitByte(be<u8> *);

enum : u32 { ICE_WAIT_ACTION = 0x0235E6C4, ICE_FADE_ACTION = 0x0235E820 };
