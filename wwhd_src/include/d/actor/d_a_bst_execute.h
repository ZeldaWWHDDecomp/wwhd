// Gohdan per-frame execution (d_a_bst_execute.cpp): globals, tables and bindings; the actor is bst_class (d_a_bst.h).
#pragma once
#include "d/actor/d_a_bst.h"
#include "gc_api/d/d_npc.h"
#include "gc_api/tu.h"
using BstExecuteID = be<u32>;
#include "gc_api/d/d_com_inf_game.h"
struct BstExecuteMessage;
inline bst_class* bstExecuteActor(u32 address) { return gabi::at<bst_class>(address); }
inline be<u32>& bstExecuteWords(be<f32>* value) { return *reinterpret_cast<be<u32>*>(value); }
void BstExecute_setCollision(cCcS*, dCcD_GObjInf*);
void BstExecute_translate(f32, f32, f32, s32);
void BstExecute_setCylinderCenter(cM3dGCyl*, cXyz*);
void BstExecute_setCylinderHeight(cM3dGCyl*, f32);
void BstExecute_setCylinderRadius(cM3dGCyl*, f32);
void BstExecute_setSphereRadius(cM3dGSph*, f32);
void BstExecute_setSphereCenter(cM3dGSph*, cXyz*);
void BstExecute_move(bst_class*);
void BstExecute_beamMove(bst_class*);
void BstExecute_demoCamera(bst_class*);
void BstExecute_setWall(dBgS_AcchCir*, f32, f32);
void BstExecute_correctPosition(dBgS_ObjAcch*, dBgS*);
void BstExecute_setGroundUpY(dBgS_ObjAcch*, f32);
void* BstExecute_cameraBody();
void BstExecute_cameraType(void*, const char*, void*);
void* BstExecute_play();
void BstExecute_customColor(s32, s32, f32);
BOOL BstExecute_isDungeonItem(dSv_memBit_c*, s32);
u32 BstExecute_searchActor(void*, be<u32>*);
void BstExecute_bgmStart(u32);
void BstExecute_morphPlay(mDoExt_McaMorf*, cXyz*, s32, s32);
void BstExecute_morphCalc(mDoExt_McaMorf*);
void BstExecute_animatorPlay(mDoExt_baseAnm*);
void BstExecute_translateMultiply(f32, f32, f32);
void BstExecute_messageState(BstExecuteMessage*, s32);
s32 BstExecute_messageStatus(BstExecuteMessage*);
void* BstExecute_getResource(void*, SafeString*, u32);
f32 BstExecute_magnitudeSquared(cXyz*);
void BstExecute_matrixCopy(Mtx34*, Mtx34*);
void BstExecute_matrixTranslate(Mtx34*, f32, f32, f32);
f32 BstExecute_sqrt(f32);
void BstExecute_initBrk(J3DFrameCtrl*, J3DModelData*, void*, s32, s32, s32, s32, s32, f32, s32);
void BstExecute_initBtk(J3DFrameCtrl*, J3DModelData*, void*, s32, s32, s32, s32, s32, f32, s32);

// Mapped references, not additional definitions of the TU-wide static objects.
struct BstExecuteGlobals {
    be<u32> hand[2];
    u8 _8;
    be<u8> mPause;
    u8 _A[6];
    be<u32> boss;
    be<u32> msg;
    u8 _18[8];
    be<s8> msg_end;
};
inline BstExecuteGlobals& bstExecuteGlobals() { return *gabi::at<BstExecuteGlobals>(0x10462978); }
struct BstExecuteTuning {
    u8 _0[8];
    be<f32> mRegF[30];
    be<s16> mRegS[8];
};
inline BstExecuteTuning& bstExecuteTuning() { return *gabi::at<BstExecuteTuning>(0x1047B608); }
struct BstExecutePalmOffsets {
    be<f32> x[4], y[4], z[4];
};
inline BstExecutePalmOffsets& bstExecutePalmOffsets() {
    return *gabi::at<BstExecutePalmOffsets>(0x10192934);
}
struct BstExecuteMessage {
    u8 _0[0x921];
    be<u8> mSend;
};
inline J3DModel* bstExecuteModel(u32 p) { return gabi::at<J3DModel>(p); }
inline mDoExt_McaMorf* bstExecuteMorf(u32 p) { return gabi::at<mDoExt_McaMorf>(p); }
inline J3DFrameCtrl* bstExecuteFrame(u32 p) { return gabi::at<J3DFrameCtrl>(p); }
inline BstExecuteMessage* bstExecuteMessage(u32 p) { return gabi::at<BstExecuteMessage>(p); }
inline dComIfG_inf_c* bstExecutePlay(void* p) { return reinterpret_cast<dComIfG_inf_c*>(p); }
inline be<u32>& bstExecuteResourceTable() { return *gabi::at<be<u32>>(0x101F4F28); }
inline be<u32>& bstExecuteSaveTable() { return *gabi::at<be<u32>>(0x101F84DC); }
inline be<u32>& bstExecuteMessageTable() { return *gabi::at<be<u32>>(0x101F4B5C); }
inline Mtx34* bstExecuteMatrix(u32 p) { return gabi::at<Mtx34>(p); }
inline be<u32>& bstExecuteCalcMatrix() { return *gabi::at<be<u32>>(0x1018C7B0); }
inline Mtx34* bstExecuteFixedMatrix() { return gabi::at<Mtx34>(0x1048D0CC); }
struct BstExecuteConstants {
    be<f32> mZero;
    u8 _1000B858[0x8];
    be<f32> mOne;
    be<f32> mDecay;
    u8 _1000B868[0x4];
    be<f32> mHalf;
    be<f32> mApproachDistance;
    be<f32> mWallRadius;
    be<f32> mHeadWallHeight;
    be<f32> mHaraiChance;
    be<f32> mPairChance;
    be<f32> mTwoHealthChance;
    be<f32> mThirty;
    be<f32> mForty;
    be<f32> mFifty;
    be<f32> mSeventy;
    be<f32> mOneFifty;
    be<f32> mMinusFiveHundred;
    be<f32> mPlayerHeightLimit;
    u8 _1000B8A4[0xC];
    be<f32> mFiveHundred;
    be<f32> mHundred;
    be<f32> mFive;
    be<f32> mTen;
    be<f32> mMinusThirty;
    be<f32> mMinusHundred;
    be<f32> mSixHundred;
    be<f32> mOneEighty;
    be<f32> mTwoFifty;
    be<f32> mSixty;
    be<f32> mHiddenHeight;
    be<f32> mSixtyFive;
    be<f32> mOneForty;
    be<f32> mColorStep;
    be<f32> mColorRise;
    be<f32> mColorFall;
};
inline BstExecuteConstants& bstExecuteConstants() {
    return *gabi::at<BstExecuteConstants>(0x1000B854);
}
WWHD_SIZE(BstExecuteGlobals, 0x21);
WWHD_OFFSET(BstExecuteGlobals, boss, 0x10);
WWHD_OFFSET(BstExecuteGlobals, msg_end, 0x20);
WWHD_SIZE(BstExecuteTuning, 0x90);
WWHD_OFFSET(BstExecuteTuning, mRegS, 0x80);
WWHD_SIZE(BstExecutePalmOffsets, 0x30);
WWHD_OFFSET(BstExecuteMessage, mSend, 0x921);

inline void* bstExecuteResources(u32 p) { return gabi::at<void>(p); }
inline mDoExt_baseAnm* bstExecuteAnimator(u32 p) { return gabi::at<mDoExt_baseAnm>(p); }
struct BstExecuteSave {
    u8 _0[0x798];
    dSv_memBit_c mStage;
};
inline dSv_memBit_c* bstExecuteStageBits(u32 p) { return &gabi::at<BstExecuteSave>(p)->mStage; }
WWHD_OFFSET(BstExecuteSave, mStage, 0x798);
inline void* bstExecuteSearchCallback() { return gabi::at<void>(0x025E1234); }
namespace bst_execute_names {
enum Part { Type_HEAD_e = 0, Type_LEFT_HAND_e = 1, Type_RIGHT_HAND_e = 2 };
enum Action {
    ACTION_STAY_e = 0,
    ACTION_FLY_e = 1,
    ACTION_DAMAGE_e = 5,
    ACTION_SLEEP_e = 6,
    ACTION_HEAD_DAMAGE_e = 7,
    ACTION_DOWN_ATTACK_e = 10,
    ACTION_PAA_ATTACK_e = 11,
    ACTION_KUMI_ATTACK_e = 12,
    ACTION_HARAI_ATTACK_e = 13,
    ACTION_BEAM_ATTACK_e = 14,
    ACTION_END_DEMO_e = 22
};
enum Sound {
    JA_BGM_BST_BATTLE = 0x80000023,
    JA_BGM_UNK_122 = 0x80000122,
    JA_SE_CM_BST_HEAD_EYE_OPEN = 0x58D7,
    JA_SE_CM_BST_HEAD_WORKING = 0x7038
};
} // namespace bst_execute_names

WWHD_SIZE(BstExecuteConstants, 0x9C);
WWHD_SIZE(BstExecuteMessage, 0x922);
WWHD_SIZE(BstExecuteSave, 0x7BC);
