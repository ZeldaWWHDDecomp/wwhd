/* Link range 05 private views. The shared daPy_lk_c layout retains the tww names;
 * these HD views describe equipment, blend buffers and actor fields used by the first 59
 * verified functions. Offsets and complete native argument widths are checked by this unit. */
#pragma once
#include "d/actor/d_a_player_main.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "SSystem/SComponent/c_m3d_g_pla.h"

struct Link05AnmDataHD;

// HD: entry points preserve the complete verified register widths.
void lk05CheckNextActionBowFly(daPy_lk_c* self);
void lk05CheckNextActionBoomerangFly(daPy_lk_c* self);
BOOL lk05CommonProcInit(daPy_lk_c* self, s32 proc);
BOOL lk05SetSingleMoveAnime(daPy_lk_c* self, s32 anm, f32 rate, f32 start, s32 end, f32 morph);
BOOL lk05CheckHeavyStateOn(daPy_lk_c* self);
BOOL lk05ItemTrigger(daPy_lk_c* self);
s32 lk05GetReadyItem(daPy_lk_c* self);
BOOL lk05ProcJumpCut_init(daPy_lk_c* self, s32 mode);
J3DAnmTransform* lk05GetItemAnimeResource(daPy_lk_c* self, u32 resource);
JKRHeap* lk05SetItemHeap(daPy_lk_c* self);
void lk05DeleteEquipItem(daPy_lk_c* self, s32 mode);
BOOL lk05CheckSetItemTrigger(daPy_lk_c* self, s32 item, s32 mode);
BOOL lk05CheckItemChangeFromButton(daPy_lk_c* self);
BOOL lk05ProcFall_init(daPy_lk_c* self, s32 mode, f32 morph);
void lk05ResetFootEffect(daPy_lk_c* self);
BOOL lk05ProcSubjectivity_init(daPy_lk_c* self, s32 mode);
BOOL lk05ProcCrouchDefense_init(daPy_lk_c* self);
BOOL lk05CheckGuardAccept(daPy_lk_c* self);
BOOL lk05GetCrawlMoveVec(daPy_lk_c* self, cXyz* top, cXyz* front, cXyz* vec);
BOOL lk05ProcCrawlStart_init(daPy_lk_c* self);
BOOL lk05ChangeSlideProc(daPy_lk_c* self);
BOOL lk05CheckNextActionFromButton(daPy_lk_c* self);
void lk05SetSpeedAndAngleNormal(daPy_lk_c* self, s32 angle);
void lk05SetBlendMoveAnime(daPy_lk_c* self, f32 morph);
cM3dGPla* lk05GetSlidePolygon(daPy_lk_c* self);
s32 lk05SetMoveAnime(daPy_lk_c* self, f32 ratio, f32 rate0, f32 rate1, s32 anm0, s32 anm1, s32 mode, f32 morph);

struct Link05AnmHeapHD {
    be<u16> mIdx;
    be<u16> field_0x2;
    be<u16> field_0x4;
    be<u16> field_0x6;
    be<u32> m_buffer;
    be<u32> mpAnimeHeap;
};
WWHD_SIZE(Link05AnmHeapHD, 0x10);
struct Link05EquipmentHD {
    u8 _0000[0x4440];
    gptr<J3DModel> mpEquipItemModel;
    u8 _4444[0x44D0 - 0x4444];
    gptr<mDoExt_McaMorf> mpParachuteFanMorf;
};
WWHD_OFFSET(Link05EquipmentHD, mpEquipItemModel, 0x4440);
WWHD_OFFSET(Link05EquipmentHD, mpParachuteFanMorf, 0x44D0);
WWHD_SIZE(Link05EquipmentHD, 0x44D4);
struct Link05OldFrameHD {
    u8 _00[0xC];
    be<f32> mOldFrameRate;
};
WWHD_OFFSET(Link05OldFrameHD, mOldFrameRate, 0xC);
WWHD_SIZE(Link05OldFrameHD, 0x10);
f64 lk05AddCalc(be<f32>* value, f32 target, f32 scale, f32 maxStep, f32 minStep);
f64 lk05GroundCross(dBgS* bgs, dBgS_GndChk* check);
f64 lk05VecSquareMag(const cXyz* value);
f64 lk05Sqrtf(f64 value);
void lk05SeStartSystem(u32 sound);
BOOL lk05AttentionLockonTruth(u32 attention);

namespace link05 {
enum : u32 {
    FANB_JNT_LROOT_e = 1,
    FANB_JNT_RROOT_e = 7,
    FANB_JNT_LARMB_e = 3,
    FANB_JNT_RARMB_e = 9,
    UNK_1620 = 0x1620,
    ModeFlg_00000100 = 0x100,
    ModeFlg_00000400 = 0x400,
    ANM_DAMFF = 0x5B,
    ANM_SALTATION = 0x96,
    ANM_WALKBARREL = 0x95,
    ANM_DAMFFUP = 0x5F,
    ANM_DAMFRUP = 0x5E,
    ANM_DAMFLUP = 0x5D,
    ANM_DAMFBUP = 0x60,
    ANM_ROPESWINGB = 0x77,
    dBgS_Attr_GIANT_FLOWER_e = 5,
    daPyItem_UNK102_e = 0x102,
    daPyItem_BOKO_e = 0x101,
    daPyItem_SWORD_e = 0x103,
    dItemNo_BOOMERANG_e = 0x2D,
    daPyProc_SIDE_STEP_LAND_e = 0xB,
    DIR_LEFT = 0x2,
    ANM_ATNJLLAND = 0x12,
    ANM_ATNJRLAND = 0x13,
    dItemNo_SKULL_HAMMER_e = 0x33,
    dRes_INDEX_LKANM_BCK_FANBA_e = 0x84,
    dRes_INDEX_LINK_BDL_FANB_e = 0x1A,
    daPyProc_FAN_GLIDE_e = 0x93,
    ANM_USEFANB = 0xA2,
    JA_SE_LK_FAN_CHUTE_OPEN = 0x2845,
    daPyStts1_DEKU_LEAF_FLY_e = 0x20,
    dItemNo_DEKU_LEAF_e = 0x34,
    JA_SE_ITEM_TARGET_OUT = 0x883,
    dActStts_DEFEND_e = 0x36,
    DIR_RIGHT = 0x3,
    daPyProc_NOCK_BACK_END_e = 0x20,
    ANM_ROLLFMIS = 0x33,
    dActStts_CROUCH_e = 0xF,
    daPyProc_SLIDE_FRONT_LAND_e = 0x1C,
    ANM_SLIDEFLAND = 0x36,
    JA_SE_LK_SLIP_SUS = 0x205A,
    daPyProc_SLIDE_BACK_LAND_e = 0x1D,
    ANM_SLIDEBLAND = 0x38,
    daPyProc_FRONT_ROLL_CRASH_e = 0x1F,
    JA_SE_LK_BODYATTACK = 0x282F,
    fpcNm_Obj_Movebox_e = 0x2B,
    ModeFlg_MIDAIR = 0x2,
    daPyProc_SIDE_ROLL_e = 0x21,
    ANM_MROLLL = 0x6E,
    ANM_MROLLR = 0x6F,
    daPyProc_BACK_JUMP_LAND_e = 0x23,
    ANM_ROLLBLAND = 0x3E,
    daPyProc_SHIP_JUMP_RIDE_e = 0x87,
    ANM_SLIPICE = 0xA9,
    JA_SE_LK_FT_JUMP_SHIP = 0x3838,
    fpcNm_SHIP_e = 0xA5,
    daPyProc_LAND_DAMAGE_e = 0x26,
    ANM_LANDDAMA = 0x30,
    JA_SE_LK_FALL_DAMAGE = 0x2816,
    daPyProc_VOMIT_LAND_e = 0x99,
    daPyProc_LAND_e = 0x25,
    ANM_JMPEDS = 0xD,
    daPyProc_CUT_EX_MJ_e = 0x49,
    DIR_BACKWARD = 0x1,
    DIR_FORWARD = 0x0,
    daPyProc_VOMIT_JUMP_e = 0x98,
    ANM_ROPESWINGF = 0x76,
    ANM_ROPECATCH = 0x75,
    daPyProc_ROPE_SWING_e = 0x78,
    fpcNm_HIMO3_e = 0x1BF,
    dItemNo_GRAPPLING_HOOK_e = 0x25,
    daPyStts0_UNK800000_e = 0x800000,
    dActStts_LET_GO_e = 0x6,
    fpcNm_NPC_MD_e = 0x16F,
    ModeFlg_DAMAGE = 0x8,
    ANM_LANDDAMAST = 0x31,
    daPyProc_LARGE_DAMAGE_WALL_e = 0x6A,
    JA_SE_MAJUTOU_JAIL_DOOR = 0x842,
    daPyProc_ICE_SLIP_FALL_UP_e = 0x9F,
    JA_SE_LK_FALL_DOWN = 0x3811,
    ANM_DAMFB = 0x5C,
    ANM_DAMFL = 0x59,
    ANM_DAMFR = 0x5A,
};
}
void lk05SetCurrentHeap(JKRHeap* heap);
BOOL lk05CameraChangeModeOK(u32 camera, s32 mode);
u32 lk05GetCameraBody(void);
u32 lk05GetCameraOption(u32 options);
s16 lk05Atan2s_d(f32 y, f64 x);
BOOL lk05ProcMoveTurn_init(daPy_lk_c* self, s32 mode);
void lk05LineSet(dBgS_LinChk* check, cXyz* start, cXyz* end, fopAc_ac_c* actor);
BOOL lk05LineCross(dBgS* bgs, dBgS_LinChk* check);
cM3dGPla* lk05GetTriPla(dBgS* bgs, u32 bg, u32 poly);
void lk05SeStart(u32 sound, cXyz* pos, u32 param, s32 reverb);
void lk05SoundSet(cXyz* pos, s32 radius, fpc_ProcID id, s32 param);
fopAc_ac_c* lk05GetActorPointer(dBgS* bgs, u32 bg);
void lk05EndFlameDamageEmitter(daPy_lk_c* self);
u32 lk05GetMtrlSndId(dBgS* bgs, cBgS_PolyInfo* poly);
BOOL lk05ProcCutTurn_init(daPy_lk_c* self, s32 mode);
void lk05SetShipRidePos(daPy_lk_c* self, s32 mode);
BOOL lk05SetDamagePoint(daPy_lk_c* self, f32 points);
BOOL lk05StartShock(dVibration_c* vibration, s32 power, s32 timer, cXyz* direction);
BOOL lk05ProcFrontRoll_init(daPy_lk_c* self, f32 morph);
BOOL lk05CheckBossGomaStage(daPy_lk_c* self);
Link05AnmDataHD* lk05GetAnmData(daPy_lk_c* self, s32 anm);
void lk05ActorKeepSetData(daPy_actorKeep_l* keep, fopAc_ac_c* actor);
void lk05VecNormalize(cXyz* out, cXyz* in);
f64 lk05Atan2f_d(f32 y, f64 x);
void lk05SetTextureAnime(daPy_lk_c* self, s32 anm, s32 param);
void lk05InitOldFrameMorf(u32 oldFrame, f32 morph, s32 first, s32 count);
BOOL lk05CheckJumpFlower(daPy_lk_c* self);
BOOL lk05ChangeFrontWallTypeProc(daPy_lk_c* self);
void lk05FreeGrabItem(daPy_lk_c* self);
BOOL lk05ProcHangStart_init(daPy_lk_c* self);
BOOL lk05ProcHangWallCatch_init(daPy_lk_c* self);
BOOL lk05ProcWait_init(daPy_lk_c* self);
void lk05SetOldRootQuaternion(daPy_lk_c* self, s32 x, s32 y, s32 z);
BOOL lk05ProcLargeDamageUp_init(daPy_lk_c* self, s32 anm, s32 mode, s32 x, s32 z);
BOOL lk05EventStartCheckOld(void* manager, const char* name);
f64 lk05PadStickValue(s32 pad);
BOOL lk05ProcLargeDamage_init(daPy_lk_c* self, s32 anm, s32 mode, s32 x, s32 z);
void lk05SetShipRidePosUseItem(daPy_lk_c* self);
void lk05EndDemoMode(daPy_lk_c* self);
void lk05SetShapeAngleToAtnActor(daPy_lk_c* self);
u32 lk05GetJointTree(u32 resource);
s32 lk05GetBlendCount(u32 resource, s32 index);

BOOL lk05CheckNextMode(daPy_lk_c* self, s32 mode);
// HD: only the attention lock flag and virtual voice entry are used by this range.
struct Link05AttentionHD {
    u8 _00[0x20];
    be<u32> mFlags;
};
WWHD_OFFSET(Link05AttentionHD, mFlags, 0x20);
WWHD_SIZE(Link05AttentionHD, 0x24);
struct Link05PlayerVTableHD {
    u8 _00[0xE4];
    be<u32> voiceStart;
};
WWHD_OFFSET(Link05PlayerVTableHD, voiceStart, 0xE4);
WWHD_SIZE(Link05PlayerVTableHD, 0xE8);
inline BOOL lk05CheckAttentionLock(u32 attention) {
    return lk05AttentionLockonTruth(attention) ||
           (gabi::at<Link05AttentionHD>(attention)->mFlags & 0x20000000);
}
inline void lk05VoiceStart(daPy_lk_c* self, u32 voice) {
    gabi::call_ptr(gabi::at<Link05PlayerVTableHD>(self->__vtbl)->voiceStart, self, voice);
}

s32 lk05ParachuteJointCallback(J3DNode* joint, s32 timing);

// HD: the blend pack carries its per-joint value buffer in addition to the tww pair.
struct Link05AnmRatioPackHD {
    be<f32> mRatio;
    gptr<J3DAnmTransform> mpAnmTransform;
    be<s32> mValueCount;
    gptr<be<f32>> mpValues;
};
WWHD_OFFSET(Link05AnmRatioPackHD, mpValues, 0xC);
WWHD_SIZE(Link05AnmRatioPackHD, 0x10);
struct Link05BlendCalcHD {
    u8 _00[0x7C];
    gptr<Link05AnmRatioPackHD> mAnmRatio;
    gptr<J3DModel> mpModel;
};
WWHD_OFFSET(Link05BlendCalcHD, mAnmRatio, 0x7C);
WWHD_OFFSET(Link05BlendCalcHD, mpModel, 0x80);
WWHD_SIZE(Link05BlendCalcHD, 0x84);
struct Link05ShipHD : fopAc_ac_c {
    u8 _3AC[8];
    gptr<mDoExt_McaMorf> mpBodyAnm;
    u8 _3B8[0x636 - 0x3B8];
    be<u8> mNextMode;
    u8 _637[0x644 - 0x637];
    be<u32> mStateFlag;
};
WWHD_OFFSET(Link05ShipHD, mpBodyAnm, 0x3B4);
WWHD_OFFSET(Link05ShipHD, mNextMode, 0x636);
WWHD_OFFSET(Link05ShipHD, mStateFlag, 0x644);
WWHD_SIZE(Link05ShipHD, 0x648);
struct Link05MoveboxHD : fopAc_ac_c {
    u8 _3AC[0x710 - 0x3AC];
    be<s32> mType;
    u8 _714[0x7D4 - 0x714];
    be<s32> mbRollCrash;
};
WWHD_OFFSET(Link05MoveboxHD, mType, 0x710);
WWHD_OFFSET(Link05MoveboxHD, mbRollCrash, 0x7D4);
WWHD_SIZE(Link05MoveboxHD, 0x7D8);
struct Link05RopeHD : fopAc_ac_c {
    u8 _3AC[0x1914 - 0x3AC];
    be<f32> m15FC;
};
WWHD_OFFSET(Link05RopeHD, m15FC, 0x1914);
WWHD_SIZE(Link05RopeHD, 0x1918);
// HD: copying float storage as words retains signaling-NaN payloads.
inline void lk05CopyFloatBits(be<f32>* dst, const be<f32>* src) {
    *reinterpret_cast<be<u32>*>(dst) = reinterpret_cast<const be<u32>*>(src)->get();
}

struct Link05SafeStringVTableHD {
    u8 _00[0x14];
    be<u32> assureTermination;
};
WWHD_OFFSET(Link05SafeStringVTableHD, assureTermination, 0x14);
WWHD_SIZE(Link05SafeStringVTableHD, 0x18);
inline void lk05AssureTermination(SafeString* string) {
    gabi::call_ptr(gabi::at<Link05SafeStringVTableHD>(string->__vtbl)->assureTermination, string);
}
// HD: guest address zero is readable storage during native string walking.
inline u8 lk05GuestByte(u32 address) { return gabi::load<u8>(address); }
struct Link05FloatHD { be<f32> mValue; };
WWHD_SIZE(Link05FloatHD, 4);
struct Link05AnmDataHD { be<u16> mBckIdx; };
WWHD_SIZE(Link05AnmDataHD, 2);
struct Link05TransformHD { be<f32> mFrame; };
WWHD_SIZE(Link05TransformHD, 4);
struct Link05RopeCounterHD {
    u8 _0000[0x6922];
    be<s16> mRopeCounter;
};
WWHD_OFFSET(Link05RopeCounterHD, mRopeCounter, 0x6922);
WWHD_SIZE(Link05RopeCounterHD, 0x6924);
struct Link05CameraOptionHD { u8 _00[2]; be<u8> mReverse; };
WWHD_OFFSET(Link05CameraOptionHD, mReverse, 2);
WWHD_SIZE(Link05CameraOptionHD, 3);
struct Link05SaveHD { u8 _0000[0x12C0]; u8 mCameraOptions; };
WWHD_OFFSET(Link05SaveHD, mCameraOptions, 0x12C0);
WWHD_SIZE(Link05SaveHD, 0x12C1);
namespace link05 {
enum : u32 { SAFE_STRING_VTBL = 0x10034B24 };
}

struct Link05TimerHD { be<u32> mTimer; };
WWHD_SIZE(Link05TimerHD, 4);

struct Link05AnimationHD {
    u8 _0000[0x57F0];
    gptr<Link05BlendCalcHD> m_pbCalc[2];
    Link05AnmRatioPackHD mAnmRatioUnder[2];
    u8 _5818[0x65CC - 0x5818];
    gptr<Link05OldFrameHD> m_old_fdata;
};
WWHD_OFFSET(Link05AnimationHD, m_pbCalc, 0x57F0);
WWHD_OFFSET(Link05AnimationHD, mAnmRatioUnder, 0x57F8);
WWHD_OFFSET(Link05AnimationHD, m_old_fdata, 0x65CC);
WWHD_SIZE(Link05AnimationHD, 0x65D0);
namespace link05 {
inline Link05AnimationHD* animation(daPy_lk_c* self) {
    return reinterpret_cast<Link05AnimationHD*>(self);
}
inline dComIfG_inf_c* gameInfo() { return gabi::at<dComIfG_inf_c>(dComIfGp_ea()); }
inline BOOL cameraReverse() {
    Link05SaveHD* save = gabi::at<Link05SaveHD>(dComIfGs_ea());
    return gabi::at<Link05CameraOptionHD>(lk05GetCameraOption(gabi::ea(&save->mCameraOptions)))->mReverse.get();
}
}

// HD: private demo views extend opaque blocks without changing the shared Link
// class.
#include "d/d_demo.h"
struct Link05DemoMessageManager {
  u8 beforeSelect[0x948];
  be<s32> mSelectNum;
};
WWHD_OFFSET(Link05DemoMessageManager, mSelectNum, 0x948);
WWHD_SIZE(Link05DemoMessageManager, 0x94C);
struct Link05DemoGameOver {
  u8 beforeStart[0x111];
  be<u8> mAnimeStart;
};
WWHD_OFFSET(Link05DemoGameOver, mAnimeStart, 0x111);
WWHD_SIZE(Link05DemoGameOver, 0x112);
struct Link05DemoCamera {
  u8 beforeAngle[0x236];
  be<s16> mAngleY;
};
WWHD_OFFSET(Link05DemoCamera, mAngleY, 0x236);
WWHD_SIZE(Link05DemoCamera, 0x238);
struct Link05DemoPlay {
  u8 beforeItem[0x52A4];
  be<u8> mGetItem;
  u8 beforeArchive[0x5A50 - 0x52A5];
  SafeString mLkDemoAnmArchive;
  u8 beforeGameOver[0x5BE3 - 0x5A58];
  be<u8> mGameoverStatus;
};
WWHD_OFFSET(Link05DemoPlay, mGetItem, 0x52A4);
WWHD_OFFSET(Link05DemoPlay, mLkDemoAnmArchive, 0x5A50);
WWHD_OFFSET(Link05DemoPlay, mGameoverStatus, 0x5BE3);
WWHD_SIZE(Link05DemoPlay, 0x5BE4);
struct Link05DemoSave {
  u8 beforeReserve[0x96];
  u8 mReserveItems[1];
};
WWHD_OFFSET(Link05DemoSave, mReserveItems, 0x96);
WWHD_SIZE(Link05DemoSave, 0x97);
struct Link05DemoFollow {
  u8 beforePos[8];
  cXyz mPos;
  u8 tail[8];
};
WWHD_OFFSET(Link05DemoFollow, mPos, 8);
WWHD_SIZE(Link05DemoFollow, 0x1C);
struct Link05DemoBoko : fopAc_ac_c {
  u8 beforeModel[8];
  gptr<J3DModel> mModel;
};
WWHD_OFFSET(Link05DemoBoko, mModel, 0x3B4);
WWHD_SIZE(Link05DemoBoko, 0x3B8);
struct Link05DemoOldTransform {
  u8 beforeTranslate[0x14];
  cXyz mTranslate;
};
WWHD_OFFSET(Link05DemoOldTransform, mTranslate, 0x14);
WWHD_SIZE(Link05DemoOldTransform, 0x20);
struct Link05DemoOldFrame {
  u8 beforeTransform[0x1C];
  gptr<Link05DemoOldTransform> mpTransform;
};
WWHD_OFFSET(Link05DemoOldFrame, mpTransform, 0x1C);
WWHD_SIZE(Link05DemoOldFrame, 0x20);
struct Link05DemoParseData {
  be<u32> mWords[2];
  be<u32> mSize;
  gptr<u8> mpData;
  be<u32> mpNext;
};
WWHD_OFFSET(Link05DemoParseData, mSize, 8);
WWHD_OFFSET(Link05DemoParseData, mpData, 0xC);
WWHD_OFFSET(Link05DemoParseData, mpNext, 0x10);
WWHD_SIZE(Link05DemoParseData, 0x14);
struct Link05DemoAnimationVtable {
  u8 beforeAttribute[0xC];
  be<u32> mGetAttribute;
  be<u32> mUnused;
  be<u32> mGetFrameMax;
};
WWHD_OFFSET(Link05DemoAnimationVtable, mGetFrameMax, 0x14);
WWHD_SIZE(Link05DemoAnimationVtable, 0x18);
struct Link05DemoAnimation {
  be<f32> mFrame;
  be<u32> mVtable;
  s32 getAttribute() {
    return gabi::call_ptr<s32>(
        gabi::at<Link05DemoAnimationVtable>(mVtable)->mGetAttribute, this);
  }
  s32 getFrameMax() {
    return gabi::call_ptr<s32>(
        gabi::at<Link05DemoAnimationVtable>(mVtable)->mGetFrameMax, this);
  }
};
WWHD_OFFSET(Link05DemoAnimation, mVtable, 4);
WWHD_SIZE(Link05DemoAnimation, 8);
inline Link05AnmHeapHD *lk05DemoUnderHeap(daPy_lk_c *self, s32 i) {
  return reinterpret_cast<Link05AnmHeapHD *>(self->m_anm_heap_under) + i;
}
inline Link05AnmHeapHD *lk05DemoUpperHeap(daPy_lk_c *self, s32 i) {
  return reinterpret_cast<Link05AnmHeapHD *>(self->m_anm_heap_upper) + i;
}
inline Link05AnmHeapHD *lk05DemoTextureHeap(daPy_lk_c *self) {
  return reinterpret_cast<Link05AnmHeapHD *>(self->m_tex_anm_heap);
}
inline Link05AnmHeapHD *lk05DemoScrollHeap(daPy_lk_c *self) {
  return reinterpret_cast<Link05AnmHeapHD *>(self->m_tex_scroll_heap);
}
inline Link05DemoPlay *lk05DemoPlay() {
  return gabi::at<Link05DemoPlay>(dComIfGp_ea());
}
inline void *lk05DemoReserveItems() {
  return gabi::at<Link05DemoSave>(dComIfGs_ea())->mReserveItems;
}
inline mDoExt_bckAnm *lk05DemoSword(daPy_lk_c *self) {
  return reinterpret_cast<mDoExt_bckAnm *>(
      &reinterpret_cast<Link05EquipmentHD *>(self)->mpEquipItemModel + 1);
}
inline Link05DemoFollow *lk05DemoFollow(daPy_lk_c *self) {
  return reinterpret_cast<Link05DemoFollow *>(self->m338C);
}
inline u16 lk05DemoProcVar6Low(daPy_lk_c *self) {
  return reinterpret_cast<be<u16> *>(&self->mProcVar6)[1];
}
inline Mtx34 *lk05DemoAnmMatrix(J3DModel *model, s32 joint) {
  J3DMtxBuffer_hd *buffer = model->mpMtxBuffer;
  u32 matrices = buffer->mpAnmMtx;
  buffer->mFlags = buffer->mFlags | 0x10;
  return gabi::at<Mtx34>(matrices + joint * sizeof(Mtx34));
}
inline void lk05DemoUpperRatioAll(u32 address, f32 value) {
  Link05BlendCalcHD *calc = gabi::at<Link05BlendCalcHD>(address);
  u32 resource = lk05GetJointTree(gabi::ea(calc->mpModel->mModelData.get()));
  s32 count = lk05GetBlendCount(resource, 0);
  for (s32 i = 0; i < count; i++)
    calc->mAnmRatio.get()[2].mpValues[i] = value;
}

BOOL lk05DemoSetActAnimeUpper(daPy_lk_c *, u32, s32, f32, f32, s32, f32);
BOOL lk05DemoCheckGrabHeavy(daPy_lk_c *);
u32 lk05DemoCreateItem(cXyz *, s32, s32, s32, s32, csXyz *, cXyz *);
fopAc_ac_c *lk05DemoGetItemPartner(daPy_lk_c *);
void lk05DemoShowItem(fopAc_ac_c *);
void lk05DemoDeleteItem(fopAc_ac_c *);
void lk05DemoHideItem(fopAc_ac_c *);
s32 lk05DemoGetItemNo(fopAc_ac_c *);
void lk05DemoSetBottleModel(daPy_lk_c *, u32);
u32 lk05DemoMessageSet(Link05DemoMessageManager *, u32, s32);
s32 lk05DemoMessageStatus(Link05DemoMessageManager *);
void lk05DemoSetMessageStatus(Link05DemoMessageManager *, s32);
void lk05DemoEmptyReserve(void *, u32);
void lk05DemoEndEventCamera(u32, u32);
J3DModelData *lk05DemoInitModel(daPy_lk_c *, gptr<J3DModel> *, s32, u32);
BOOL lk05DemoInitBck(mDoExt_bckAnm *, J3DModelData *, J3DAnmTransform *, s32,
                     s32, f32, s32, s32, s32);
fopAc_ac_c *lk05DemoTalkPartner(daPy_lk_c *);
void lk05DemoMessageFlagOn(s32);
u32 lk05DemoLoadTextureAnime(daPy_lk_c *, u32, s32);
void lk05DemoSetTextureAnimeResource(daPy_lk_c *, u32, s32);
u32 lk05DemoLoadTextureScroll(daPy_lk_c *, u32, s32);
void lk05DemoSetTextureScroll(daPy_lk_c *, u32, s32);
s16 lk05DemoRad2s(f32);
void lk05DemoSetBgCheckParam(daPy_lk_c *);
fopAc_ac_c *lk05DemoEventPartner(daPy_lk_c *);
s32 lk05DemoCheckShipRideUseItem(daPy_lk_c *, s32);
void lk05DemoSetAnimeUnequip(daPy_lk_c *);
void lk05DemoInitShipRideUseItem(daPy_lk_c *, s32, s32);
void lk05DemoItemGetColorOn();
BOOL lk05DemoCompulsory(void *, void *, s32, u32);
void lk05DemoLavaInitSub(daPy_lk_c *);
void lk05DemoRestartRoom(daPy_lk_c *, s32, s32, f32, s32);
void lk05DemoFreezeInitSub(daPy_lk_c *, s32);
BOOL lk05DemoSwimUpInit(daPy_lk_c *, s32);
void lk05DemoMakeEmitter(daPy_mtxFollowEcallBack_c *, u32, Mtx34 *, cXyz *,
                         void *);
void lk05DemoPlayBaseAnm(mDoExt_bckAnm *);
void lk05DemoEntryBck(mDoExt_bckAnm *, J3DModelData *, f32);
fopAc_ac_c *lk05DemoLookActor(daPy_lk_c *);
s16 lk05DemoTargetAngleY(cXyz *, cXyz *);
void lk05DemoKeepItemData(daPy_lk_c *);
s32 lk05DemoTalkStartBack(daPy_lk_c *);
void lk05DemoGetItemSound(daPy_lk_c *, u32, s32);
void lk05DemoDeadInitSub(daPy_lk_c *);
void lk05DemoDeadInitSub2(daPy_lk_c *);
Link05DemoGameOver *lk05DemoFindGameOver(s32);
void lk05DemoGameOverAlpha(Link05DemoGameOver *, f32);
BOOL lk05DemoGameOverDelete(Link05DemoGameOver *);
void lk05DemoGameStartStage();
void lk05DemoGameStart();
void lk05DemoConstructAngle(csXyz *, s32, s32, s32);
void lk05DemoSeaColor(GXColor *, GXColor *);
void lk05DemoStringConstruct(SafeString *);
dDemo_actor_c *lk05DemoGetActor(dDemo_object_c *, u32);
void lk05DemoOffsetPosition(cXyz *, cXyz *, s32, cXyz *);
void lk05DemoParseData(be<u32> *, Link05DemoParseData *);
void lk05DemoSelectEquip(s32, u32);
void lk05DemoSetSwordModel(daPy_lk_c *, s32);
void lk05DemoSetTactModel(daPy_lk_c *);
Link05DemoAnimation *lk05DemoGetIDRes(void *, SafeString *, u32);
void lk05DemoSetFrameCtrl(daPy_lk_c *, J3DFrameCtrl *, s32, s32, s32, f32, f32);
void lk05DemoSetSeAnime(daPy_lk_c *, Link05DemoAnimation *, Link05AnmHeapHD *,
                        J3DFrameCtrl *);

struct Link05DemoUpperRatio {
  u8 beforeRatio[0x5818];
  Link05AnmRatioPackHD mAnmRatioUpper[2];
};
WWHD_OFFSET(Link05DemoUpperRatio, mAnmRatioUpper, 0x5818);
WWHD_SIZE(Link05DemoUpperRatio, 0x5838);

struct Link05DemoFrameTable {
  be<u32> mFrames[8];
};
WWHD_SIZE(Link05DemoFrameTable, 0x20);

// HD: the sword animation exposes its current frame at byte 4.
struct Link05DemoSwordFrame {
  u8 beforeFrame[4];
  be<f32> mFrame;
};
WWHD_OFFSET(Link05DemoSwordFrame, mFrame, 4);
WWHD_SIZE(Link05DemoSwordFrame, 8);

namespace link05demo {
enum : u32 {
  daPyProc_GRAB_HEAVY_WAIT_e = 0x74,
  ANM_GRABWAITB = 0x69,
  dRes_INDEX_LKANM_BCK_GRABWAITB_e = 0x96,
  fopAcStts_UNK10000_e = 0x10000,
  fpcNm_ITEM_e = 0xFF,
  fpcNm_Demo_Item_e = 0x101,
  fopMsgStts_MSG_DISPLAYED_e = 0xE,
  fopMsgStts_MSG_CONTINUES_e = 0xF,
  fopMsgStts_MSG_ENDS_e = 0x10,
  fopMsgStts_BOX_CLOSED_e = 0x12,
  fopMsgStts_MSG_DESTROYED_e = 0x13,
  dRes_INDEX_LKANM_BCK_TETOLACH_e = 0x10F,
  dRes_INDEX_LINK_BDL_TETOLACH_e = 0x28,
  daPyItem_UNK104_e = 0x104,
  dRes_INDEX_LKANM_BCK_GETLETTERA_e = 0x8E,
  dRes_INDEX_LINK_BDL_LETTER_e = 0x21,
  fopMsgStts_MSG_PREPARING_e = 1,
  daPyProc_DEMO_DAMAGE_e = 0xAB,
  ANM_DAMF = 0x57,
  daPyProc_DEMO_HOLDUP_e = 0xAC,
  ANM_WALLHOLDUPDW = 0x93,
  ANM_WALLHOLDUP = 0x92,
  ANM_HOLDUP = 0x91,
  daPyProc_DEMO_OPEN_TREASURE_e = 0xAD,
  ANM_BOXOPENSHORTLINK = 0x8F,
  ANM_BOXOPENLINK = 0x8E,
  daPyItem_BOKO_e = 0x101,
  daPyProc_DEMO_GET_ITEM_e = 0xAE,
  daPyProc_DEMO_OPEN_SALVAGE_TREASURE_e = 0xD3,
  daPyItem_NONE_e = 0x100,
  ANM_ITEMGET = 0x90,
  daPyProc_DEMO_UNEQUIP_e = 0xAF,
  daPyProc_DEMO_DEAD_e = 0xB2,
  daPyProc_DEMO_LOOK_AROUND_e = 0xB3,
  ANM_COMEOUT = 0x94,
  daPyProc_DEMO_SALUTE_e = 0xB4,
  ANM_SALTATION = 0x96,
  daPyProc_DEMO_LOOK_AROUND2_e = 0xB5,
  ANM_WHO = 0x97,
  daPyStts0_SHIP_RIDE_e = 0x10000,
  daPyProc_DEMO_TALISMAN_PICKUP_e = 0xB6,
  ANM_PICKUP = 0x98,
  JA_SE_LK_ITEM_TAKEOUT = 0x2810,
  daPyProc_DEMO_TALISMAN_WAIT_e = 0xB7,
  ANM_WAITPICKUP = 0x99,
  JA_SE_OBJ_OMAMORI = 0x613E,
  daPyProc_DEMO_SURPRISED_e = 0xB8,
  ANM_SURPRISED = 0x9A,
  ANM_SURPRISEDWAIT = 0xD8,
  daPyProc_DEMO_TURN_BACK_e = 0xB9,
  ANM_TURNBACK = 0x9B,
  daPyProc_DEMO_LOOK_UP_e = 0xBA,
  ANM_LOOKUP = 0x9C,
  daPyProc_DEMO_QUAKE_WAIT_e = 0xBB,
  ANM_WAITQ = 0x9D,
  daPyProc_DEMO_DANCE_e = 0xBC,
  ANM_GLAD = 0x9E,
  daPyProc_DEMO_CAUGHT_e = 0xBD,
  ANM_MOGAKU1 = 0xA4,
  ANM_FM_BATA = 0xA5,
  daPyProc_DEMO_PUSH_PULL_WAIT_e = 0xBF,
  ANM_WAITPUSHPULL = 0x7E,
  daPyStts0_UNK4000000_e = 0x4000000,
  daPyProc_DEMO_PUSH_MOVE_e = 0xC0,
  ANM_WALKPUSH = 0x7F,
  daPyProc_DEMO_DOOR_OPEN_e = 0xC1,
  ANM_DOOROPENBLINK = 0xB0,
  ANM_DOOROPENALINK = 0xAF,
  daPyProc_DEMO_NOD_e = 0xC2,
  ANM_SEYYES = 0xB1,
  daPyStts0_UNK10_e = 0x10,
  ANM_PRESENTATIONA = 0xB2,
  daPyProc_DEMO_PRESENT_e = 0xC3,
  JA_SE_TALK_START = 0x80A,
  ANM_WAITS = 0,
  ANM_TALKA = 0x1B,
  daPyStts0_SWIM_e = 0x100000,
  daPyItem_SWORD_e = 0x103,
  dItemNo_WIND_WAKER_e = 0x22,
  dItemNo_SHIELD_e = 0x3B,
  dItemNo_MIRROR_SHIELD_e = 0x3C,
};
}

// HD: JStudio iterators can address guest zero or wrap; retain guest loads
// rather than host pointer arithmetic.
inline u16 lk05DemoReadHalfword(u32 address) {
  return gabi::load<u16>(address);
}

// HD: identical parser assertion text has distinct addresses for the byte
// iterator; literal registration is keyed by text, so retain these explicit
// guest string identities.
inline const char *lk05DemoHandParseFile() { return STR(0x10034B1C); }
inline const char *lk05DemoHandParseCondition() { return STR(0x10035054); }
