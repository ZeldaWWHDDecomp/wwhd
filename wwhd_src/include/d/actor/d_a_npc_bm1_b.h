/* Generic Rito behavior, private WWHD views.
 * HIO and path layouts retain their GameCube sizes. HD choices live in the global
 * message manager; its selected option is a word. Actor layout deltas are documented
 * in d_a_npc_bm1.h. Private names keep companion TU views distinct. */
#pragma once
#include "d/actor/d_a_npc_bm1.h"
#include "gc_api/d/d_com_inf_game.h"

#include "d/d_save.h"
#include "gc_api/tu.h"
#include "gc_api/f_op/f_op_actor.h"

struct Bm1BehaviorHioPrm {
    /* 0x00 */ be<s16> mMaxHeadX;
    /* 0x02 */ be<s16> mMaxHeadY;
    /* 0x04 */ be<s16> mMinHeadX;
    /* 0x06 */ be<s16> mMinHeadY;
    /* 0x08 */ be<s16> mMaxBackboneX;
    /* 0x0A */ be<s16> mMaxBackboneY;
    /* 0x0C */ be<s16> mMinBackboneX;
    /* 0x0E */ be<s16> mMinBackboneY;
    /* 0x10 */ be<s16> mMaxTurnStep;
    /* 0x12 */ be<s16> mCalcAngleTarget;
    /* 0x14 */ be<f32> mAttPosOffsetY;
    /* 0x18 */ be<u8> m18;
    /* 0x19 */ u8 _19;
    /* 0x1A */ be<s16> mFlyScale;
    /* 0x1C */ be<s16> mFlyMaxStep;
    /* 0x1E */ u8 _1E[2];
    /* 0x20 */ be<f32> m20;
    /* 0x24 */ be<f32> m24;
    /* 0x28 */ be<f32> m28;
    /* 0x2C */ be<f32> m2C;
    /* 0x30 */ be<f32> m30;
    /* 0x34 */ be<s16> m34;
    /* 0x36 */ be<s16> m36;
    /* 0x38 */ be<f32> m38;
    /* 0x3C */ be<f32> m3C;
    /* 0x40 */ be<f32> m40;
    /* 0x44 */ be<f32> m44;
    /* 0x48 */ be<f32> m48;
};
WWHD_SIZE(Bm1BehaviorHioPrm, 0x4C);
struct Bm1BehaviorChildHio {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ Bm1BehaviorHioPrm hio_prm;
    /* 0x50 */ be<u32> m50;
};
WWHD_SIZE(Bm1BehaviorChildHio, 0x54);
struct Bm1BehaviorHio {
    /* 0x000 */ be<u32> __vtbl;
    /* 0x004 */ be<s8> m4;
    /* 0x005 */ u8 _5[3];
    /* 0x008 */ be<s32> m8;
    /* 0x00C */ Bm1BehaviorChildHio children[10];
};
WWHD_SIZE(Bm1BehaviorHio, 0x354);

struct Bm1BehaviorPath {
    u8 _0[5];
    be<u8> mLoop;
};
WWHD_OFFSET(Bm1BehaviorPath, mLoop, 5);
struct Bm1BehaviorMessageCamera {
    be<u32> mCameraID;
    be<s32> mBasicID;
    gptr<fopAc_ac_c> mActor[10];
};
WWHD_OFFSET(Bm1BehaviorMessageCamera, mActor, 8);
WWHD_SIZE(Bm1BehaviorMessageCamera, 0x30);
struct Bm1BehaviorMessage {
    u8 _0[0x948];
    be<u32> mSelectNum;
};
WWHD_OFFSET(Bm1BehaviorMessage, mSelectNum, 0x948);
struct Bm1BehaviorModelData {
    u8 _0[0x38];
    gptr<void> mpTexPattern;
};
WWHD_OFFSET(Bm1BehaviorModelData, mpTexPattern, 0x38);
void Bm1Behavior_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot,
                              s16 vel, u8 headOnly);
void Bm1Behavior_playerEyePos(cXyz* out, f32 offs);
bool Bm1Behavior_lockonTruth(dAttention_c* attention);
fopAc_ac_c* Bm1Behavior_lockonTarget(dAttention_c* attention, s32 index);
fopAc_ac_c* Bm1Behavior_actionTarget(dAttention_c* attention, s32 index);
u32 Bm1Behavior_messageStatus(u32 manager);
s16 Bm1Behavior_calcTimer(be<s16>* timer);
void Bm1Behavior_pathPoint(dNpc_PathRun_bm1* path, cXyz* out, u8 index);
u8 Bm1Behavior_pathArgument(dNpc_PathRun_bm1* path, u8 index);
BOOL Bm1Behavior_pathPassed(dNpc_PathRun_bm1* path, cXyz* pos, u32 direction);
void Bm1Behavior_pathNext(dNpc_PathRun_bm1* path);
s32 Bm1Behavior_pathMax(dNpc_PathRun_bm1* path);
void Bm1Behavior_sendLetter(u16 number);
void Bm1Behavior_constructArray(void* array, s32 count, u32 size, u32 constructor);
void Bm1Behavior_groundCheckDestroy(dBgS_GndChk* check, s32 flags);
void Bm1Behavior_circleDestroy(cM3dGCir_l* circle, s32 flags);
void Bm1Behavior_acchDestroy(dBgS_ObjAcch* acch, s32 flags);
void Bm1Behavior_actorDestroy(daNpc_Bm1_c* actor, s32 flags);

enum Bm1BehaviorFigType {
    DSNAP_TYPE_NPC_BM1_PST = 0x8D,
    DSNAP_TYPE_NPC_BM1_SKT_KKT = 0x8E,
    DSNAP_TYPE_NPC_BM1_BMD_1 = 0x8F,
    DSNAP_TYPE_NPC_BM1_BMB_2 = 0x91,
    DSNAP_TYPE_NPC_BM1_BMD_0 = 0x92,
    DSNAP_TYPE_NPC_BM1_BMB_0_1 = 0x93,
    DSNAP_TYPE_NPC_BM1_BMC_0_1_2 = 0x90,
    DSNAP_TYPE_NPC_BM1_BMC_3 = 0x97
};

enum Bm1BehaviorBeastIndex { Bm1GoldenFeather = 2 };

struct Bm1BehaviorPtmf {
    be<s16> mThisAdjustment;
    be<s16> mVirtualIndex;
    be<u16> mFunctionHigh;
    be<s16> mVtableOffset;
};
WWHD_SIZE(Bm1BehaviorPtmf, 8);
struct Bm1BehaviorVirtualSlot {
    be<u32> mThisAdjustment;
    be<u32> mFunction;
};
WWHD_SIZE(Bm1BehaviorVirtualSlot, 8);
struct Bm1BehaviorActorVtable {
    u8 _0[0x24];
    be<u32> mAnmAtr;
};
WWHD_OFFSET(Bm1BehaviorActorVtable, mAnmAtr, 0x24);

using Bm1BehaviorAction = gabi::ptmf<BOOL (daNpc_Bm1_c::*)(void*)>;
WWHD_SIZE(Bm1BehaviorAction, 8);

WWHD_OFFSET(Bm1BehaviorHioPrm, mAttPosOffsetY, 0x14);
WWHD_OFFSET(Bm1BehaviorHioPrm, mFlyScale, 0x1A);
WWHD_OFFSET(Bm1BehaviorHioPrm, m20, 0x20);
WWHD_OFFSET(Bm1BehaviorChildHio, hio_prm, 4);
WWHD_OFFSET(Bm1BehaviorChildHio, m50, 0x50);
WWHD_OFFSET(Bm1BehaviorHio, children, 0xC);
