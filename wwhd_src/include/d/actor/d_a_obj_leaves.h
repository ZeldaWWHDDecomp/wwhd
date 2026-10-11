/* Leaf pile, WWHD layout: actor members follow the expanded 0x3AC base.
 * The eight-byte HD member pointer follows the smoke callback. Private renderer views below
 * describe the HD material overrides. */
#pragma once
#include "d/actor/d_a_player.h"
#include "gc_api/gc_api.h"
struct daObjLeaves_c : fopAc_ac_c {
    using MemberFunc = void (daObjLeaves_c::*)();
    enum Param_e {
        PRM_ITEM_SAVE_BIT_NO_W = 7,
        PRM_ITEM_SAVE_BIT_NO_S = 6,
        PRM_ITEM_NO_W = 6,
        PRM_ITEM_NO_S = 0,
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 13
    };
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
    /* 0x3BC */ dCcD_Stts mStts;
    /* 0x3F8 */ dCcD_Sph mSph;
    /* 0x524 */ be<f32> m408, m40C;
    /* 0x52C */ dPa_smokeEcallBack mSmokeCallback;
    /* 0x54C */ be<s32> m430, m434, m438;
    /* 0x558 */ be<u8> m43C;
    u8 _559[3];
    /* 0x55C */ gabi::ptmf<MemberFunc> mMemberFunc;
    /* 0x564 */ be<u8> m44C;
    u8 _565[3];
    void birthEffect(s32, cXyz*, csXyz*, GXColor*);
    u32 chk_appear();
    bool create_heap();
    void init_mtx();
    s32 _create();
    bool _delete();
    bool _execute();
    bool _draw();
    void wait_proc();
    void alpha_calc_start_wait_proc();
    void item_set_wait_proc();
    // HD: the original retirement entry leaves this or the deletion result in r3.
    u32 retire_wait_proc();
};
WWHD_OFFSET(daObjLeaves_c, mSph, 0x3F8);
WWHD_OFFSET(daObjLeaves_c, mSmokeCallback, 0x52C);
WWHD_OFFSET(daObjLeaves_c, mMemberFunc, 0x55C);
WWHD_OFFSET(daObjLeaves_c, m44C, 0x564);
WWHD_SIZE(daObjLeaves_c, 0x568);
enum {
    fpcNm_PLAYER_e = 168,
    VOCHI_BDL_VOCHI = 4,
    VOCHI_DZB_VOCHI = 7,
    JA_SE_OBJ_OCHIBA_BLOW = 0x69EF,
    JA_SE_OBJ_OCHIBA_BURN = 0x69F0
};
// HD: direct action addresses until the member-function callback registry supports names.
enum LeavesAction {
    LeavesWait = 0x0236ABD0,
    LeavesAlphaWait = 0x0236ACCC,
    LeavesItemWait = 0x0236ACF8,
    LeavesRetireWait = 0x0236ADFC
};
struct LeavesEffect {
    be<u16> mId;
    u8 _02[2];
    be<s32> mKind, mFadeDelay, mRetireDelay;
    be<f32> mAlphaStep;
};
WWHD_SIZE(LeavesEffect, 0x14);
struct LeavesLinearColor {
    be<f32> channel[4];
};
WWHD_SIZE(LeavesLinearColor, 0x10);
struct LeavesBgW {
    u8 _00[0xB0];
    be<u32> mRideCallback;
};
WWHD_OFFSET(LeavesBgW, mRideCallback, 0xB0);
struct LeavesJointData {
    u8 _00[8];
    be<u16> mJointNum;
};
struct LeavesShape {
    u8 _00[4];
    be<u8> mVisible;
};
struct LeavesMaterialResource {
    u8 _00[0x20];
    be<u32> mBlendOffset;
};
struct LeavesTevVtable {
    u8 _00[0x3C];
    be<u32> mSetKColor;
    u8 _40[0xC];
    be<u32> mGetKColor;
};
struct LeavesTevBlock {
    u8 _00[4];
    gptr<LeavesTevVtable> mVtable;
};
struct LeavesMaterial {
    gptr<LeavesMaterialResource> mResource;
    gptr<LeavesMaterial> mNext;
    gptr<LeavesShape> mShape;
    u8 _0C[0xC];
    gptr<LeavesTevBlock> mTevBlock;
    u8 _1C[0x84];
    be<u32> mDirty;
};
struct LeavesJoint {
    u8 _00[0x10];
    gptr<LeavesMaterial> mMesh;
    u8 _14[8];
};
WWHD_SIZE(LeavesJoint, 0x1C);
WWHD_OFFSET(LeavesMaterial, mTevBlock, 0x18);
WWHD_OFFSET(LeavesMaterial, mDirty, 0xA0);
WWHD_OFFSET(LeavesTevVtable, mGetKColor, 0x4C);
// HD: unit-local ABI views for constructors and renderer-only operations.
void leavesSphere_ct(dCcD_Sph*);
void leavesSphere_dt(dCcD_Sph*, s32);
void leavesActor_dt(fopAc_ac_c*, s32);
void leavesSetSphereCenter(cM3dGSph*, f32, f32, f32);
void leavesSetCollision(cCcS*, dCcD_Sph*);
void leavesSubtract(cXyz*, cXyz*, cXyz*);
f32 leavesSquareMag(cXyz*);
f32 leavesSqrt(f32);
void leavesSetTranslucent(J3DModel*, bool);
LeavesJointData* leavesGetJointData(J3DModelData*);
void leavesSetMaterialAlpha(LeavesMaterial*, u32, u32);
void leavesSetBlend(void*, u32);
void leavesToLinear(cXyz*, LeavesLinearColor*, f32);
LeavesLinearColor* leavesGetColorOverride(be<u32>*, u32);

namespace leaves_attack {
enum dCcD_ObjAtType {
    /* 0x00000002 */ AT_TYPE_SWORD = (1 << 1),
    /* 0x00000008 */ AT_TYPE_UNK8 = (1 << 3),
    /* 0x00000020 */ AT_TYPE_BOMB = (1 << 5),
    /* 0x00000040 */ AT_TYPE_BOOMERANG = (1 << 6),
    /* 0x00000080 */ AT_TYPE_BOKO_STICK = (1 << 7),
    /* 0x00000100 */ AT_TYPE_WATER = (1 << 8),
    /* 0x00000200 */ AT_TYPE_FIRE = (1 << 9),
    /* 0x00000400 */ AT_TYPE_MACHETE = (1 << 10),
    /* 0x00000800 */ AT_TYPE_UNK800 = (1 << 11),
    /* 0x00001000 */ AT_TYPE_SPIKE = (1 << 12),
    /* 0x00002000 */ AT_TYPE_UNK2000 = (1 << 13),
    /* 0x00004000 */ AT_TYPE_NORMAL_ARROW = (1 << 14),
    /* 0x00008000 */ AT_TYPE_HOOKSHOT = (1 << 15),
    /* 0x00010000 */ AT_TYPE_SKULL_HAMMER = (1 << 16),
    /* 0x00020000 */ AT_TYPE_UNK20000 = (1 << 17),
    /* 0x00040000 */ AT_TYPE_FIRE_ARROW = (1 << 18),
    /* 0x00080000 */ AT_TYPE_ICE_ARROW = (1 << 19),
    /* 0x00100000 */ AT_TYPE_LIGHT_ARROW = (1 << 20),
    /* 0x00200000 */ AT_TYPE_WIND = (1 << 21),
    /* 0x00400000 */ AT_TYPE_UNK400000 = (1 << 22),
    /* 0x00800000 */ AT_TYPE_LIGHT = (1 << 23),
    /* 0x01000000 */ AT_TYPE_STALFOS_MACE = (1 << 24),
    /* 0x02000000 */ AT_TYPE_FAN_SWING = (1 << 25),
    /* 0x04000000 */ AT_TYPE_DARKNUT_SWORD = (1 << 26),
    /* 0x08000000 */ AT_TYPE_GRAPPLING_HOOK = (1 << 27),
    /* 0x10000000 */ AT_TYPE_MOBLIN_SPEAR = (1 << 28),
    /* 0x20000000 */ AT_TYPE_PGANON_SWORD = (1 << 29),
    /* 0xFFFFFFFF */ AT_TYPE_ALL = ~0,
};
}

WWHD_OFFSET(daObjLeaves_c, mPhase, 0x3AC);
WWHD_OFFSET(daObjLeaves_c, mModel, 0x3B4);
WWHD_OFFSET(daObjLeaves_c, mpBgW, 0x3B8);
WWHD_OFFSET(daObjLeaves_c, mStts, 0x3BC);
WWHD_OFFSET(daObjLeaves_c, m408, 0x524);
WWHD_OFFSET(daObjLeaves_c, m40C, 0x528);
WWHD_OFFSET(daObjLeaves_c, m430, 0x54C);
WWHD_OFFSET(daObjLeaves_c, m434, 0x550);
WWHD_OFFSET(daObjLeaves_c, m438, 0x554);
WWHD_OFFSET(daObjLeaves_c, m43C, 0x558);
WWHD_OFFSET(LeavesEffect, mKind, 4);
WWHD_OFFSET(LeavesEffect, mFadeDelay, 8);
WWHD_OFFSET(LeavesEffect, mRetireDelay, 0xC);
WWHD_OFFSET(LeavesEffect, mAlphaStep, 0x10);
WWHD_SIZE(LeavesBgW, 0xB4);
WWHD_OFFSET(LeavesJointData, mJointNum, 8);
WWHD_SIZE(LeavesJointData, 0xA);
WWHD_OFFSET(LeavesShape, mVisible, 4);
WWHD_SIZE(LeavesShape, 5);
WWHD_OFFSET(LeavesMaterialResource, mBlendOffset, 0x20);
WWHD_SIZE(LeavesMaterialResource, 0x24);
WWHD_OFFSET(LeavesTevVtable, mSetKColor, 0x3C);
WWHD_SIZE(LeavesTevVtable, 0x50);
WWHD_OFFSET(LeavesTevBlock, mVtable, 4);
WWHD_SIZE(LeavesTevBlock, 8);
WWHD_OFFSET(LeavesMaterial, mResource, 0);
WWHD_OFFSET(LeavesMaterial, mNext, 4);
WWHD_OFFSET(LeavesMaterial, mShape, 8);
WWHD_SIZE(LeavesMaterial, 0xA4);
WWHD_OFFSET(LeavesJoint, mMesh, 0x10);
