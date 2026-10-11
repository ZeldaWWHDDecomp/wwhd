// Gohdan movement and actions (d_a_bst_move.cpp): globals, tables and bindings; the actor is bst_class (d_a_bst.h).
#pragma once
#include "d/actor/d_a_bst.h"
#include "gc_api/d/d_npc.h"
#include "gc_api/tu.h"
using BstMoveID = be<u32>;
#include "gc_api/d/d_com_inf_game.h"
struct BstMoveMessage;
inline bst_class* bstMoveActor(u32 address) { return gabi::at<bst_class>(address); }
inline be<u32>& bstMoveWords(be<f32>* value) { return *reinterpret_cast<be<u32>*>(value); }

struct BstMoveGlobals {
    be<u32> hand[2];
    be<s8> m4;
    be<u8> m5;
    be<u8> m6;
    u8 pad;
    be<u32> vtable;
    be<u32> boss;
};
inline BstMoveGlobals& bstMoveGlobals() { return *gabi::at<BstMoveGlobals>(0x10462978); }
struct BstMoveTuning {
    u8 pad[8];
    be<f32> mRegF[30];
    be<s16> mRegS[9];
};
inline BstMoveTuning& bstMoveTuning() { return *gabi::at<BstMoveTuning>(0x1047B608); }
inline be<u32>& bstMoveResourceTable() { return *gabi::at<be<u32>>(0x101F4F28); }
inline be<u32>& bstMoveSaveTable() { return *gabi::at<be<u32>>(0x101F84DC); }
inline be<u32>& bstMoveCalcMatrix() { return *gabi::at<be<u32>>(0x1018C7B0); }
struct BstMoveSave {
    u8 pad[0x89];
    be<u8> mArrowNum;
    be<u8> mBombNum;
};
struct BstMoveCollisionObject {
    u8 pad[0x44];
    be<u32> mpGObjInf;
};
struct BstMoveCollisionInfo {
    u8 pad[0xC];
    be<u32> mpActor;
};
struct BstMoveTrig {
    be<f32> mSin, mCos;
};
inline BstMoveTrig* bstMoveTrig() { return gabi::at<BstMoveTrig>(0x104A44F8); }
inline be<u16>* bstMoveBombEffects() { return gabi::at<be<u16>>(0x10192850); }
inline void* bstMoveSearchCallback() { return gabi::at<void>(0x025E1234); }
inline GXColor* bstMoveParticleColor() { return gabi::at<GXColor>(0x101D5E98); }
inline be<u32>* bstMoveAnimatorField(bst_class* a, bool brk) {
    auto* p = a;
    return brk ? &p->mpTevRegAnimator.v : &p->mpTexMtxAnimator.v;
}
inline be<f32>* bstMoveAxis(cXyz* v, u32 axis) {
    return axis == 0 ? &v->x : axis == 4 ? &v->y : &v->z;
}
WWHD_OFFSET(BstMoveGlobals, boss, 0x10);
WWHD_SIZE(BstMoveGlobals, 0x14);
WWHD_OFFSET(BstMoveTuning, mRegS, 0x80);
WWHD_SIZE(BstMoveTuning, 0x92);
WWHD_OFFSET(BstMoveSave, mArrowNum, 0x89);
WWHD_SIZE(BstMoveSave, 0x8B);
WWHD_OFFSET(BstMoveCollisionObject, mpGObjInf, 0x44);
WWHD_SIZE(BstMoveCollisionObject, 0x48);
WWHD_OFFSET(BstMoveCollisionInfo, mpActor, 0xC);
WWHD_SIZE(BstMoveCollisionInfo, 0x10);
WWHD_SIZE(BstMoveTrig, 8);
struct BstMoveConstants {
    be<f32> mZero;
    u8 pad858[0x8];
    be<f32> mOne;
    be<f32> mPositionChaseRatio;
    u8 pad868[0x4];
    be<f32> mHalf;
    u8 pad870[0x4];
    be<f32> mTwoHundred;
    be<f32> mThreeHundred;
    be<f32> mBoundaryRatio;
    u8 pad880[0x8];
    be<f32> mThirty;
    be<f32> mForty;
    be<f32> mFifty;
    u8 pad894[0x4];
    be<f32> mOneFifty;
    u8 pad89C[0x14];
    be<f32> mFiveHundred;
    be<f32> mHundred;
    be<f32> mFive;
    be<f32> mTen;
    u8 pad8C0[0x8];
    be<f32> mSixHundred;
    u8 pad8CC[0x8];
    be<f32> mSixty;
    be<f32> mTenThousand;
    u8 pad8DC[0x10];
    be<f32> mProgressStep;
    u8 pad8F0[0x34];
    be<f32> mTwo;
    u8 pad928[0x4];
    be<f32> mFourThousand;
    be<f32> mTwoThousand;
    u8 pad934[0x4];
    be<f32> mMinusTwoHundred;
    be<f32> mFourHundred;
    u8 pad940[0xC];
    be<f32> mTwenty;
    u8 pad950[0xC];
    be<f32> mNinety;
    be<f32> mArenaRadius;
    be<f32> mFifteen;
    be<f32> mEightHundred;
    be<f32> mThousand;
    be<f32> mThreeFifty;
    be<f32> mFifteenHundred;
    be<f32> mTwentyFive;
    be<f32> mSoundDistanceScale;
    be<f32> mUnsignedFloatSplit;
    be<f32> mRecoilCutoff;
    be<f32> mRecoilDecay;
};
inline BstMoveConstants& bstMoveConstants() { return *gabi::at<BstMoveConstants>(0x1000B854); }
WWHD_OFFSET(BstMoveConstants, mZero, 0x0);
WWHD_OFFSET(BstMoveConstants, mOne, 0xC);
WWHD_OFFSET(BstMoveConstants, mPositionChaseRatio, 0x10);
WWHD_OFFSET(BstMoveConstants, mHalf, 0x18);
WWHD_OFFSET(BstMoveConstants, mTwoHundred, 0x20);
WWHD_OFFSET(BstMoveConstants, mThreeHundred, 0x24);
WWHD_OFFSET(BstMoveConstants, mBoundaryRatio, 0x28);
WWHD_OFFSET(BstMoveConstants, mThirty, 0x34);
WWHD_OFFSET(BstMoveConstants, mForty, 0x38);
WWHD_OFFSET(BstMoveConstants, mFifty, 0x3C);
WWHD_OFFSET(BstMoveConstants, mOneFifty, 0x44);
WWHD_OFFSET(BstMoveConstants, mFiveHundred, 0x5C);
WWHD_OFFSET(BstMoveConstants, mHundred, 0x60);
WWHD_OFFSET(BstMoveConstants, mFive, 0x64);
WWHD_OFFSET(BstMoveConstants, mTen, 0x68);
WWHD_OFFSET(BstMoveConstants, mSixHundred, 0x74);
WWHD_OFFSET(BstMoveConstants, mSixty, 0x80);
WWHD_OFFSET(BstMoveConstants, mTenThousand, 0x84);
WWHD_OFFSET(BstMoveConstants, mProgressStep, 0x98);
WWHD_OFFSET(BstMoveConstants, mTwo, 0xD0);
WWHD_OFFSET(BstMoveConstants, mFourThousand, 0xD8);
WWHD_OFFSET(BstMoveConstants, mTwoThousand, 0xDC);
WWHD_OFFSET(BstMoveConstants, mMinusTwoHundred, 0xE4);
WWHD_OFFSET(BstMoveConstants, mFourHundred, 0xE8);
WWHD_OFFSET(BstMoveConstants, mTwenty, 0xF8);
WWHD_OFFSET(BstMoveConstants, mNinety, 0x108);
WWHD_OFFSET(BstMoveConstants, mArenaRadius, 0x10C);
WWHD_OFFSET(BstMoveConstants, mFifteen, 0x110);
WWHD_OFFSET(BstMoveConstants, mEightHundred, 0x114);
WWHD_OFFSET(BstMoveConstants, mThousand, 0x118);
WWHD_OFFSET(BstMoveConstants, mThreeFifty, 0x11C);
WWHD_OFFSET(BstMoveConstants, mFifteenHundred, 0x120);
WWHD_OFFSET(BstMoveConstants, mTwentyFive, 0x124);
WWHD_OFFSET(BstMoveConstants, mSoundDistanceScale, 0x128);
WWHD_OFFSET(BstMoveConstants, mUnsignedFloatSplit, 0x12C);
WWHD_OFFSET(BstMoveConstants, mRecoilCutoff, 0x130);
WWHD_OFFSET(BstMoveConstants, mRecoilDecay, 0x134);
WWHD_SIZE(BstMoveConstants, 0x138);
inline be<u16>* bstMove_sleep_anm() { return gabi::at<be<u16>>(0x10192870); }
inline be<u16>* bstMove_fly_anm() { return gabi::at<be<u16>>(0x10192878); }
inline be<u16>* bstMove_damage_anm() { return gabi::at<be<u16>>(0x10192880); }
inline be<u16>* bstMove_sleep_start_anm() { return gabi::at<be<u16>>(0x10192888); }
inline be<u16>* bstMove_start_anm() { return gabi::at<be<u16>>(0x10192890); }
inline be<u16>* bstMove_start2_anm() { return gabi::at<be<u16>>(0x10192898); }
inline be<u16>* bstMove_start_smoke() { return gabi::at<be<u16>>(0x101928A0); }
inline be<u16>* bstMove_fly_btk() { return gabi::at<be<u16>>(0x101928A8); }
inline be<u16>* bstMove_fly_brk() { return gabi::at<be<u16>>(0x101928B0); }
inline be<u16>* bstMove_damage_btk() { return gabi::at<be<u16>>(0x101928B8); }
inline be<u16>* bstMove_damage_brk() { return gabi::at<be<u16>>(0x101928C0); }
inline be<u16>* bstMove_down_anm() { return gabi::at<be<u16>>(0x101928C8); }
inline be<u16>* bstMove_sleep_btk() { return gabi::at<be<u16>>(0x101928E0); }
inline be<u16>* bstMove_sleep_brk() { return gabi::at<be<u16>>(0x101928E8); }

void* BstMove_getResource(void*, SafeString*, u32);
void BstMove_initBrk(void*, void*, void*, s32, s32, s32, s32, s32, f32, s32);
void BstMove_initBtk(void*, void*, void*, s32, s32, s32, s32, s32, f32, s32);
void* BstMove_play();
void BstMove_setHandCo(bst_class*, s32);
f32 BstMove_random(f32);
f32 BstMove_randomSymmetric(f32);
void BstMove_anmInit(bst_class*, s32, f32, s32, f32, s32);
void BstMove_rotateY(void*, s16);
void BstMove_rotateXMultiply(void*, s16);
void BstMove_transformVector(cXyz*, cXyz*);
void BstMove_vectorAddResult(cXyz*, cXyz*, cXyz*);
void BstMove_vectorSubtractResult(cXyz*, cXyz*, cXyz*);
f32 BstMove_magnitudeSquared(cXyz*);
f32 BstMove_sqrt(f32);
void BstMove_approach(be<f32>*, f32, f32, f32);
void BstMove_approachZero(be<f32>*, f32, f32);
s16 BstMove_targetAngleY(bst_class*, void*);
void BstMove_approachAngle(be<s16>*, s32, s32, s32);
void BstMove_moveFly(bst_class*, s32);
s8 BstMove_reverb(s8);
void BstMove_startSound(u32, cXyz*, s32, s8);
void BstMove_bgmStart(u32);
u32 BstMove_searchActor(void*, BstMoveID*);
void BstMove_startShock(void*, s32, s32, cXyz*);
u32 BstMove_setParticle(void*, s32, u32, cXyz*, void*, void*, s32, void*, s32, s32, void*, void*);
BOOL BstMove_checkCoHit(dCcD_Cyl*);
u32 BstMove_getCoHitObject(dCcD_Cyl*);
BOOL BstMove_bombCheckExplode(void*);
s32 BstMove_bombGetRestTime(void*);
void BstMove_bombSetCarry(void*);
void BstMove_bombChangeState(void*, s32);
void BstMove_bombSetNoHit(void*);
void BstMove_bombSetRestTime(void*, s32);
void BstMove_deleteActor(void*);
void BstMove_bgmStop(s32);
void BstMove_streamBgmStart(u32);
void BstMove_setEmitterMatrix(void*, Mtx34*, cXyz*); // JPASetRMtxTVecfromMtx
void BstMove_matrixCopy(void*, void*);
void BstMove_setSimpleParticle(void*, u32, cXyz*, s32, GXColor*, GXColor*, s32);
void BstMove_setHandAt(bst_class*, s32);
void BstMove_paaAttack(bst_class*);
void BstMove_kumiAttack(bst_class*);
void BstMove_haraiAttack(bst_class*);
void BstMove_beamAttack(bst_class*);
void BstMove_startDemo(bst_class*);
void BstMove_endDemo(bst_class*);
void BstMove_soundSe(bst_class*, u32, u32);
void BstMove_damageCheck(bst_class*);
void BstMove_vectorAdd(cXyz*, cXyz*, cXyz*);
void BstMove_setHandCoNoArgs();
enum BstMoveArchiveName : u32 {
    BST_FLY_ARCHIVE = 0x1000B6FC,
    BST_SLEEP_ARCHIVE = 0x1000B700,
    BST_DAMAGE_ARCHIVE = 0x1000B704,
    BST_HEAD_ARCHIVE = 0x1000B708,
    BST_ARCHIVE_VTABLE = 0x1000B714
};
inline mDoExt_McaMorf* bstMoveMorf(u32 address) { return gabi::at<mDoExt_McaMorf>(address); }
inline J3DModel* bstMoveModel(u32 address) { return gabi::at<J3DModel>(address); }
inline J3DModelData* bstMoveModelData(u32 address) { return gabi::at<J3DModelData>(address); }
inline J3DMtxBuffer_hd* bstMoveMatrixBuffer(u32 address) {
    return gabi::at<J3DMtxBuffer_hd>(address);
}
inline J3DFrameCtrl* bstMoveFrame(u32 address) { return gabi::at<J3DFrameCtrl>(address); }
inline BstMoveSave* bstMoveSave(u32 address) { return gabi::at<BstMoveSave>(address); }
inline BstMoveCollisionObject* bstMoveCollisionObject(u32 address) {
    return gabi::at<BstMoveCollisionObject>(address);
}
inline BstMoveCollisionInfo* bstMoveCollisionInfo(u32 address) {
    return gabi::at<BstMoveCollisionInfo>(address);
}
inline dComIfG_inf_c* bstMoveInfo(u32 address) { return gabi::at<dComIfG_inf_c>(address); }
inline Mtx34* bstMoveMatrix(u32 address) { return gabi::at<Mtx34>(address); }
inline void* bstMoveOpaque(u32 address) { return gabi::at<void>(address); }

namespace bst_move_names {
enum Part { Type_HEAD_e = 0, Type_LEFT_HAND_e = 1, Type_RIGHT_HAND_e = 2 };
enum Action {
    ACTION_STAY_e = 0,
    ACTION_FLY_e = 1,
    ACTION_DAMAGE_e = 5,
    ACTION_SLEEP_e = 6,
    ACTION_HEAD_DAMAGE_e = 7,
    ACTION_HEAD_HUKKI_e = 8,
    ACTION_DOWN_ATTACK_e = 10,
    ACTION_PAA_ATTACK_e = 11,
    ACTION_KUMI_ATTACK_e = 12,
    ACTION_HARAI_ATTACK_e = 13,
    ACTION_BEAM_ATTACK_e = 14,
    ACTION_HANA_DEMO_e = 20,
    ACTION_END_DEMO_e = 22
};
enum Sound {
    JA_BGM_BST_BATTLE = 0x80000023,
    JA_BGM_UNK_122 = 0x80000122,
    JA_SE_CM_BST_HEAD_EYE_OPEN = 0x58D7,
    JA_SE_CM_BST_HEAD_WORKING = 0x7038
};
} // namespace bst_move_names
namespace bst_move_names {
enum dRes_INDEX_BST {
    /* BCK  */
    dRes_INDEX_BST_BCK_BEAM_e = 0x6,
    dRes_INDEX_BST_BCK_BEAM_MAE_e = 0x7,
    dRes_INDEX_BST_BCK_BEAM_TAME_e = 0x8,
    dRes_INDEX_BST_BCK_BOM_BAKUHATU_e = 0x9,
    dRes_INDEX_BST_BCK_BOM_NOMI_e = 0xA,
    dRes_INDEX_BST_BCK_BST_DEAD_e = 0xB,
    dRes_INDEX_BST_BCK_BST_FLY_e = 0xC,
    dRes_INDEX_BST_BCK_BST_HIRAKU_e = 0xD,
    dRes_INDEX_BST_BCK_BST_SLEEP_e = 0xE,
    dRes_INDEX_BST_BCK_BST_START_e = 0xF,
    dRes_INDEX_BST_BCK_EYEALL_CLOSE_LOOP_e = 0x10,
    dRes_INDEX_BST_BCK_EYEL_CLOSE_LOOP_e = 0x11,
    dRes_INDEX_BST_BCK_EYER_CLOSE_LOOP_e = 0x12,
    dRes_INDEX_BST_BCK_EYE_OPEN_e = 0x13,
    dRes_INDEX_BST_BCK_HANA_DEMO_e = 0x14,
    dRes_INDEX_BST_BCK_KUTI_OPEN_e = 0x15,
    dRes_INDEX_BST_BCK_KUTI_OPEN_LOOP_e = 0x16,
    dRes_INDEX_BST_BCK_KUTI_TOJIRU_e = 0x17,
    dRes_INDEX_BST_BCK_LH_AWASE_e = 0x18,
    dRes_INDEX_BST_BCK_LH_DAMAGE_e = 0x19,
    dRes_INDEX_BST_BCK_LH_DOWN_e = 0x1A,
    dRes_INDEX_BST_BCK_LH_FLY_e = 0x1B,
    dRes_INDEX_BST_BCK_LH_GOO_e = 0x1C,
    dRes_INDEX_BST_BCK_LH_PAA_e = 0x1D,
    dRes_INDEX_BST_BCK_LH_SLEEP_e = 0x1E,
    dRes_INDEX_BST_BCK_LH_START_e = 0x1F,
    dRes_INDEX_BST_BCK_LH_START2_e = 0x20,
    dRes_INDEX_BST_BCK_RH_AWASU_e = 0x21,
    dRes_INDEX_BST_BCK_RH_DAMAGE_e = 0x22,
    dRes_INDEX_BST_BCK_RH_DOWN_e = 0x23,
    dRes_INDEX_BST_BCK_RH_FLY_e = 0x24,
    dRes_INDEX_BST_BCK_RH_GOO_e = 0x25,
    dRes_INDEX_BST_BCK_RH_PAA_e = 0x26,
    dRes_INDEX_BST_BCK_RH_SLEEP_e = 0x27,
    dRes_INDEX_BST_BCK_RH_START_e = 0x28,
    dRes_INDEX_BST_BCK_RH_START2_e = 0x29,
    /* BDLM */
    dRes_INDEX_BST_BDL_BST_e = 0x2C,
    dRes_INDEX_BST_BDL_HBSITA1_e = 0x2D,
    dRes_INDEX_BST_BDL_HBYOKO1_e = 0x2E,
    dRes_INDEX_BST_BDL_LHAND_e = 0x2F,
    dRes_INDEX_BST_BDL_MUNE_e = 0x30,
    dRes_INDEX_BST_BDL_RHAND_e = 0x31,
    dRes_INDEX_BST_BDL_TAMA_e = 0x32,
    dRes_INDEX_BST_BDL_UDEL_e = 0x33,
    dRes_INDEX_BST_BDL_UDER_e = 0x34,
    /* BRK  */
    dRes_INDEX_BST_BRK_BST_e = 0x37,
    dRes_INDEX_BST_BRK_BST_DAMAGE_e = 0x38,
    dRes_INDEX_BST_BRK_BST_END_e = 0x39,
    dRes_INDEX_BST_BRK_BST_START_e = 0x3A,
    dRes_INDEX_BST_BRK_HBSITA1_e = 0x3B,
    dRes_INDEX_BST_BRK_HBYOKO1_e = 0x3C,
    dRes_INDEX_BST_BRK_LHAND_e = 0x3D,
    dRes_INDEX_BST_BRK_LH_DAMAGE_e = 0x3E,
    dRes_INDEX_BST_BRK_LH_DEAD_e = 0x3F,
    dRes_INDEX_BST_BRK_LH_END_e = 0x40,
    dRes_INDEX_BST_BRK_LH_START_e = 0x41,
    dRes_INDEX_BST_BRK_MUNE_e = 0x42,
    dRes_INDEX_BST_BRK_MUNE_END_e = 0x43,
    dRes_INDEX_BST_BRK_MUNE_START_e = 0x44,
    dRes_INDEX_BST_BRK_RHAND_e = 0x45,
    dRes_INDEX_BST_BRK_RH_DAMAGE_e = 0x46,
    dRes_INDEX_BST_BRK_RH_DEAD_e = 0x47,
    dRes_INDEX_BST_BRK_RH_END_e = 0x48,
    dRes_INDEX_BST_BRK_RH_START_e = 0x49,
    dRes_INDEX_BST_BRK_UDEL_e = 0x4A,
    dRes_INDEX_BST_BRK_UDEL_END_e = 0x4B,
    dRes_INDEX_BST_BRK_UDEL_START_e = 0x4C,
    dRes_INDEX_BST_BRK_UDER_e = 0x4D,
    dRes_INDEX_BST_BRK_UDER_END_e = 0x4E,
    dRes_INDEX_BST_BRK_UDER_START_e = 0x4F,
    /* BTK  */
    dRes_INDEX_BST_BTK_BST_e = 0x52,
    dRes_INDEX_BST_BTK_BST_DAMAGE_e = 0x53,
    dRes_INDEX_BST_BTK_BST_END_e = 0x54,
    dRes_INDEX_BST_BTK_BST_START_e = 0x55,
    dRes_INDEX_BST_BTK_HBSITA1_e = 0x56,
    dRes_INDEX_BST_BTK_LHAND_e = 0x57,
    dRes_INDEX_BST_BTK_LH_DAMAGE_e = 0x58,
    dRes_INDEX_BST_BTK_LH_DEAD_e = 0x59,
    dRes_INDEX_BST_BTK_LH_END_e = 0x5A,
    dRes_INDEX_BST_BTK_LH_START_e = 0x5B,
    dRes_INDEX_BST_BTK_MUNE_e = 0x5C,
    dRes_INDEX_BST_BTK_MUNE_END_e = 0x5D,
    dRes_INDEX_BST_BTK_MUNE_START_e = 0x5E,
    dRes_INDEX_BST_BTK_RHAND_e = 0x5F,
    dRes_INDEX_BST_BTK_RH_DAMAGE_e = 0x60,
    dRes_INDEX_BST_BTK_RH_DEAD_e = 0x61,
    dRes_INDEX_BST_BTK_RH_END_e = 0x62,
    dRes_INDEX_BST_BTK_RH_START_e = 0x63,
    dRes_INDEX_BST_BTK_UDEL_e = 0x64,
    dRes_INDEX_BST_BTK_UDEL_END_e = 0x65,
    dRes_INDEX_BST_BTK_UDEL_START_e = 0x66,
    dRes_INDEX_BST_BTK_UDER_e = 0x67,
    dRes_INDEX_BST_BTK_UDER_END_e = 0x68,
    dRes_INDEX_BST_BTK_UDER_START_e = 0x69,
};

}
