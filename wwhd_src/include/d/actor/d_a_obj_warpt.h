/* Warp pot. GC names and HD layouts.
 * HD: larger actor lighting/animation shifts the actor tail; GHS PTMFs use eight bytes.
 * Numbered members still left follow zeldaret/tww; the `was mXXX` comments keep tww's names of the renamed ones. */
#pragma once

#include "m_Do/m_Do_ext.h"
struct WarptColor { be<u8> r, g, b, a; };
WWHD_SIZE(WarptColor, 4);
struct WarptModel {
    u8 mPadding0[0xAC]; be<u32> mModelData;
    u8 mPaddingB0[0x18]; be<f32> mBaseTransformMtx[12];
};
WWHD_SIZE(WarptModel, 0xF8);
WWHD_OFFSET(WarptModel, mModelData, 0xAC);
WWHD_OFFSET(WarptModel, mBaseTransformMtx, 0xC8);
struct WarptBgW {
    be<u32> mUseFlags; u8 mPadding4[0xA4]; be<u32> mCorrection;
    u8 mPaddingAC[4]; be<u32> mRideCallback;
};
WWHD_SIZE(WarptBgW, 0xB4);
WWHD_OFFSET(WarptBgW, mCorrection, 0xA8);
WWHD_OFFSET(WarptBgW, mRideCallback, 0xB0);
struct WarptSave { u8 mPadding0[0x20]; u8 mZone[4]; u8 mPadding24[0x620]; u8 mEvent[4]; };
WWHD_SIZE(WarptSave, 0x648);
WWHD_OFFSET(WarptSave, mZone, 0x20);
WWHD_OFFSET(WarptSave, mEvent, 0x644);
struct WarptPlayer {
    u8 mPadding0[0x3AC]; be<u8> mCutType;
    u8 mPadding3AD[11]; be<u32> mNoResetFlg0, mNoResetFlg1;
};
WWHD_SIZE(WarptPlayer, 0x3C0);
WWHD_OFFSET(WarptPlayer, mCutType, 0x3AC);
WWHD_OFFSET(WarptPlayer, mNoResetFlg0, 0x3B8);
WWHD_OFFSET(WarptPlayer, mNoResetFlg1, 0x3BC);
struct WarptFollowVtable { u8 mPadding0[0x44]; be<u32> mRemove; };
WWHD_SIZE(WarptFollowVtable, 0x48);
WWHD_OFFSET(WarptFollowVtable, mRemove, 0x44);
struct WarptPlayerVtable { u8 mPadding0[0x4C]; be<u32> mCheckPlayerFly; };
WWHD_SIZE(WarptPlayerVtable, 0x50);
WWHD_OFFSET(WarptPlayerVtable, mCheckPlayerFly, 0x4C);
struct WarptDrawList { be<u32> mOpaque, mTranslucent; };
WWHD_SIZE(WarptDrawList, 8);
struct WarptPlay {
    u8 mPadding0[0x12A0]; u8 mBgS[4];
    u8 mPadding12A4[0x1400]; u8 mCcS[4];
    u8 mPadding26A8[0x2C10]; be<u16> mEventFlags;
    u8 mPadding52BA[10]; u8 mEventManager[4];
    u8 mPadding52C8[0x7E8]; be<u32> mpParticle;
    u8 mPadding5AB4[0x78]; be<u32> mpPlayer;
    u8 mPadding5B30[0x240]; WarptDrawList mBgList, mList;
};
WWHD_SIZE(WarptPlay, 0x5D80);
WWHD_OFFSET(WarptPlay, mBgS, 0x12A0);
WWHD_OFFSET(WarptPlay, mCcS, 0x26A4);
WWHD_OFFSET(WarptPlay, mEventFlags, 0x52B8);
WWHD_OFFSET(WarptPlay, mEventManager, 0x52C4);
WWHD_OFFSET(WarptPlay, mpParticle, 0x5AB0);
WWHD_OFFSET(WarptPlay, mpPlayer, 0x5B2C);
WWHD_OFFSET(WarptPlay, mBgList, 0x5D70);
WWHD_OFFSET(WarptPlay, mList, 0x5D78);
struct WarptAcch {
    u8 mPadding0[0x10]; be<u32> mBaseVtable, mVtable;
    be<u8> mObject; u8 mPadding19[7]; be<u32> mPolyVtable;
    u8 mPadding24[4]; be<u32> mFlags;
    u8 mPadding2C[0x198];
};
WWHD_SIZE(WarptAcch, 0x1C4);
WWHD_OFFSET(WarptAcch, mBaseVtable, 0x10);
WWHD_OFFSET(WarptAcch, mVtable, 0x14);
WWHD_OFFSET(WarptAcch, mObject, 0x18);
WWHD_OFFSET(WarptAcch, mPolyVtable, 0x20);
WWHD_OFFSET(WarptAcch, mFlags, 0x28);
struct WarptAcchCir { u8 mCircle[0x14]; u8 mBase[0x2C]; };
WWHD_SIZE(WarptAcchCir, 0x40);
WWHD_OFFSET(WarptAcchCir, mBase, 0x14);
struct WarptShapeAttribute { u8 mBounds[0x18]; be<u32> mBoundsVtable, mVtable; };
WWHD_SIZE(WarptShapeAttribute, 0x20);
WWHD_OFFSET(WarptShapeAttribute, mBoundsVtable, 0x18);
WWHD_OFFSET(WarptShapeAttribute, mVtable, 0x1C);
struct daObj_Warpt_HIO_c {
    be<u32> mVtable; be<u8> m04, m05, m06; u8 mPadding7;
    be<f32> m08, m0C, m10; be<u32> m14, m18;
};
WWHD_SIZE(daObj_Warpt_HIO_c, 0x1C);
WWHD_OFFSET(daObj_Warpt_HIO_c, m04, 4);
WWHD_OFFSET(daObj_Warpt_HIO_c, m08, 8);
WWHD_OFFSET(daObj_Warpt_HIO_c, m14, 0x14);
WWHD_OFFSET(daObj_Warpt_HIO_c, m18, 0x18);
struct WarptSafeString { be<u32> mText, mVtable; };
WWHD_SIZE(WarptSafeString, 8);
template<class T> inline T* Warpt_guest(u32 address) { return gabi::at<T>(address); }
inline be<u32>& Warpt_word(be<f32>& f) { return *reinterpret_cast<be<u32>*>(&f); }

#include "d/d_cc_d.h"
#include "f_op/f_op_actor.h"
// Warp-pot WWHD layout: the animation controller and Acch/Cir differ from GC.
struct WarptFollow_HD {
  be<u32> mVtable;
  gptr<u8> mEmitter;
  u8 _8[12];
};
struct daObj_Warpt_c : fopAc_ac_c {
  /* 0x3AC */ be<s32> mCurMode;  /* was m290: modeProc 023B9CB0: stores the new mode and calls its init / run from mode_tbl (tww mode_tbl[it].init / .run); _ride 023B9D5C / _draw 023BB074 test it (MODE_EVENT_WARP, the open mode 2); after tww daObj_Canon_c / daObj_Ikada_c mCurMode */
  /* 0x3B0 */ u8 _3B0[4];
  /* 0x3B4 */ be<s32> m298;
  /* 0x3B8 */ be<s32> mWarpSclsNo;  /* was m29C: getArg 023B9F74: parameter bits 12-19 (tww); normalWarp 023BB7AC warps to it once mWarpSwNo is on (tww warp(it), warp sets the next stage by scls number) */
  /* 0x3BC */ be<s32> mSpSclsNo1;  /* was m2A0: getArg 023B9F74: parameter bits 8-15 for the special pots (tww); spWarp 023BB6A4 warps to it when warp bit 1 is set (tww isWarpBit(1) -> warp(it)) */
  /* 0x3C0 */ be<s32> mSpSclsNo2;  /* was m2A4: getArg 023B9F74: parameter bits 16-23 for the special pots (tww); spWarp 023BB6A4 warps to it when warp bit 2 is set (tww isWarpBit(2) -> warp(it)) */
  /* 0x3C4 */ be<s32> mSpSclsNo4;  /* was m2A8: getArg 023B9F74: parameter bits 24-31 for the special pots (tww); spWarp 023BB6A4 warps to it when warp bit 4 is set (tww isWarpBit(4) -> warp(it)) */
  /* 0x3C8 */ be<s32> mLidSwNo;  /* was m2AC: getArg 023B9F74: switch number (tww uVar6 bits 0-7); openHuta 023BB400 turns it on, mLidClosed is set while it is off (tww dComIfGs_onSwitch / isSwitch(it, ..)); isOtherHuta 023B9884 / modeClose 023BB488 test it */
  /* 0x3CC */ be<s32> mWarpSwNo;  /* was m2B0: getArg 023B9F74: switch number (tww uVar6 bits 8-15); normalWarp 023BB7AC only warps while it is on (tww dComIfGs_isSwitch(it, ..)) */
  /* 0x3D0 */ be<s32> mPotType;  /* was m2B4: getArg 023B9F74: parameter bits 0-3 (tww); isSp 023B9864: 2 .. 4 are the special pots (tww it == 2 || 3 || 4); spWarp 023BB6A4 switches on it; createInit 023BA4E0 picks the smoke colours by it (tww m_def_smoke_prm_color[it]) */
  /* 0x3D4 */ be<s32> mEventRegNo;  /* was m2B8: getArg 023B9F74: parameter bits 4-7 for the special pots (tww); onWarpBit 023B9E94 / isWarpBit 023B9F1C use the event register m_event_reg[it] (tww) */
  /* 0x3D8 */ be<s32> m2BC;
  /* 0x3DC */ be<s32> m2C0;
  /* 0x3E0 */ be<u8> mWarpBit;  /* was m2C4: getArg 023B9F74: 1, 2 or 4 by mPotType (tww); openHuta 023BB400 sets it in the event register (tww onWarpBit(it)); getArg sets mLidClosed from isWarpBit(it) (tww) */
  /* 0x3E1 */ be<u8> m2C5;
  /* 0x3E2 */ be<u8> mLidClosed;  /* was m2C6: tww accessor isHuta() { return it; } (our isHuta 023B985C); getArg 023B9F74: set while the lid switch / warp bit is off (tww); openHuta 023BB400 clears it */
  /* 0x3E3 */ u8 _3E3;
  /* 0x3E4 */ request_of_phase_process_class mPhase;
  /* 0x3EC */ gptr<u8> mpBodyModel;
  /* 0x3F0 */ gptr<u8> mpLidModel1;
  /* 0x3F4 */ gptr<u8> mpBodyBgW1;
  /* 0x3F8 */ be<f32> mBodyBgW1Mtx[12];  /* was m2DC: createBodyHeap 023B9AE8: the matrix mpBodyBgW1 is set up with (tww mpBodyBgW1->Set(.., &it)); setMtx 023BA250 copies the pot matrix into it (tww cMtx_copy(.., it)) */
  /* 0x428 */ gptr<u8> mpBodyBgW2;
  /* 0x42C */ be<f32> mBodyBgW2Mtx[12];  /* was m310: createBodyHeap 023B9AE8: the matrix mpBodyBgW2 is set up with (tww mpBodyBgW2->Set(.., &it)); setMtx 023BA250 copies the pot matrix into it (tww) */
  /* 0x45C */ gptr<u8> mpLidBgW;
  /* 0x460 */ be<f32> mLidBgWMtx[12];  /* was m344: createHutaHeap 023B9944: the matrix mpLidBgW is set up with (tww mpLidBgW->Set(.., &it)); setMtx 023BA250 copies the lid matrix into it (tww) */
  /* 0x490 */ gptr<u8> mpLidModel2;
  /* 0x494 */ mDoExt_brkAnm mLidBrk;
  /* 0x50C */ dCcD_Stts mStts;
  /* 0x548 */ dCcD_Cyl mCyl1;
  /* 0x678 */ dCcD_Cyl mCyl2;
  /* 0x7A8 */ WarptAcch mAcch;
  /* 0x96C */ WarptAcchCir mAcchCir;
  /* 0x9AC */ cXyz mLidPos;  /* was m830: setMtx 023BA250: current.pos raised by l_HIO.m08 (tww it = current.pos; it.y += l_HIO.m08), where the lid models are put; setCollision 023BA1BC mCyl2 centre; breakHuta 023BAA28 particles; _draw 023BB074 tev position (tww) */
  /* 0x9B8 */ be<s32> mBreakFireTimer;  /* was m83C: modeBreakFireInit 023BB57C 60; modeBreakFire 023BB5CC acts when cLib_calcTimer(&it) reaches 0 (tww) */
  /* 0x9BC */ be<s8> mBgW2RegistTimer;  /* was m840: createInit 023BA4E0 30 or 5, modeEventWarp 023BB820 30 (tww); _execute 023BAD74 counts it down (tww cLib_calcTimer(&it)); setMtx 023BA250 registers mpBodyBgW2 when it is 1 (tww if (it == 1) .. Regist(mpBodyBgW2, this)) */
  /* 0x9BD */ u8 _9BD[3];
  /* 0x9C0 */ WarptFollow_HD mOpenSmokeCallBack;  /* was m844: modeOpenInit 023BB2A4: the follow callback of the open smoke particle 0x8161 (tww ID_AK_SN_WARPPOTOPEN00, &it); _execute 023BAD74 recolours its emitter; _delete 023BA8C8 removes it */
  /* 0x9D4 */ WarptFollow_HD mCloseSmokeCallBack;  /* was m858: modeCloseInit 023BB380: the follow callback of the close smoke particle 0x8162 (tww ID_AK_SN_WARPPOTCLOSE00, &it); modeOpenInit 023BB2A4 removes it; _delete 023BA8C8 removes it */
  /* 0x9E8 */ be<u32> mSmokePrmColor;  /* was m86C: createInit 023BA4E0: m_def_smoke_prm_color[mPotType] (tww); modeOpenInit 023BB2A4 / modeCloseInit 023BB380 pass it as the smoke particle's prim colour (tww particle_set(.., &it, &m870)) */
  /* 0x9EC */ be<u32> mSmokeEnvColor;  /* was m870: createInit 023BA4E0: m_def_smoke_env_color[mPotType] (tww); modeOpenInit 023BB2A4 / modeCloseInit 023BB380 pass it as the smoke particle's env colour (tww particle_set(.., &m86C, &it)) */
  u8 isHuta();
  bool isSp();
  bool isOtherHuta();
  bool isRealHuta();
  bool createHutaHeap();
  bool createBodyHeap();
  BOOL _createHeap();
  void _ride(fopAc_ac_c *);
  void onWarpBit(u8);
  bool isWarpBit(u8);
  void getArg();
  void initCollision();
  void setCollision();
  void setMtx();
  void createInit();
  s32 _create();
  BOOL _delete();
  void breakHuta(s32);
  void checkHitSE();
  BOOL _execute();
  BOOL _draw();
  void modeProc(s32, s32);
  void modeOpenInit();
  void modeOpen();
  void modeCloseInit();
  void modeClose();
  void openHuta();
  void modeBreakFireInit();
  void modeBreakFire();
  void warp(s32);
  bool spWarp();
  bool normalWarp();
  void modeEventWarp();
  void modeEventOpen();
};
WWHD_OFFSET(daObj_Warpt_c, mLidBrk, 0x494);
WWHD_OFFSET(daObj_Warpt_c, mStts, 0x50C);
WWHD_OFFSET(daObj_Warpt_c, mAcch, 0x7A8);
WWHD_OFFSET(daObj_Warpt_c, mLidPos, 0x9AC);
WWHD_OFFSET(daObj_Warpt_c, mOpenSmokeCallBack, 0x9C0);
WWHD_SIZE(daObj_Warpt_c, 0x9F0);

struct WarptAttack { u8 mPadding0[0x10]; be<u32> mType; };
WWHD_SIZE(WarptAttack, 0x14);
WWHD_OFFSET(WarptAttack, mType, 0x10);

u32 Warpt_getPlay();
s32 Warpt_isSwitch(void*, u32, u32);
u32 Warpt_getEventReg(void*, u32);
void Warpt_setEventReg(void*, u32, u32);
void Warpt_seStart(u32, u32);
void Warpt_setNextStageBySclsNum(u32, u32);
void Warpt_seStartGlobal(u32);
void Warpt_onSwitch(void*, u32, u32);
void Warpt_statusInit(void*, u32, u32, void*);
void Warpt_setCylinder(void*, void*);
void Warpt_cylinderSetC(void*, void*);
void Warpt_collisionSet(void*, void*);
s32 Warpt_getReverb(u32);
void Warpt_seStartCurrent(u32, void*, u32, u32);
void Warpt_animationPlay(void*);
s32 Warpt_calcTimerInt(void*);
void Warpt_bgRelease(void*, void*);
u32 Warpt_getTargetHit(void*);
u8* Warpt_getObjectRes(void*, void*, u32);
void Warpt_assert(void*, u32, void*);
u8* Warpt_modelCreate(void*, u32, u32);
u8* Warpt_bgNew(void*);
s32 Warpt_bgSet(void*, void*, u32, void*);
s32 Warpt_brkInit(void*, void*, void*, u32, u32, f32, u32, u32, u32, u32);
f32 Warpt_pow(f32, f32);
s32 Warpt_eventEndCheck(void*, const char*);
void Warpt_resDelete(void*, const char*);
void Warpt_delete(void*);
void Warpt_circleDtor(void*, u32);
void Warpt_acchDtor(void*, u32);
void Warpt_cylinderDtor(void*, u32);
void Warpt_statusDtor(void*, u32);
void Warpt_actorDtor(void*, u32);
s32 Warpt_searchPlayerAngleY(void*, void*);
s32 Warpt_checkPlayerInCircle(void*, f32, f32);
void Warpt_setParticle(void*, u32, u32, void*, void*, void*, u32, void*, u32, void*, void*, u32);
void Warpt_followRemove(void*);
void Warpt_transS(void*, f32, f32, f32);
void Warpt_YrotM(void*, u32);
void Warpt_scaleM(f32, f32, f32);
void Warpt_matrixCopy(void*, void*);
void Warpt_bgMove(void*);
void Warpt_bgRegist(void*, void*, void*);
void Warpt_setCullBox(void*, f32, f32, f32, f32, f32, f32);
void Warpt_setWall(void*, f32, f32);
void Warpt_acchSet(void*, void*, void*, void*, u32, void*, void*, void*, void*);
void Warpt_posMoveF(void*, void*);
void Warpt_actorCtor(void*);
void Warpt_brkCtor(void*);
void Warpt_statusBaseCtor(void*);
void Warpt_statusGameCtor(void*);
void Warpt_collisionObjectCtor(void*);
void Warpt_cylinderGeometryCtor(void*);
void Warpt_acchCtor(void*);
void Warpt_acchCirCtor(void*);
void Warpt_followCtor(void*, void*, void*);
s32 Warpt_resLoad(void*, const char*);
s32 Warpt_entrySolidHeap(void*, void*, u32);
u8* Warpt_new(u32);
void Warpt_copyMemory(void*, void*, u32);
void Warpt_registerDestructor(void*);
void Warpt_headerGuardCtor(void*);
void Warpt_headerByteCtor(void*);
u32 Warpt_getEnvLight();
void Warpt_settingTevStruct(void*, u32, void*, void*);
void Warpt_setLightTevColorType(void*, void*, void*);
void Warpt_modelUpdateDL(void*, u32);
void Warpt_brkEntry(void*, void*, f32);
void Warpt_statusMove(void*);
void Warpt_hitEffHibana(void*, void*);
void Warpt_acchCrrPos(void*, void*);
void Warpt_calcTimerChar(void*);
void Warpt_orderOtherEvent(void*, const char*, u32, u32);
s32 Warpt_getMyStaffId(void*, const char*, u32, u32);
u32 Warpt_getMyNowCutName(void*, u32);
void Warpt_cutEnd(void*, u32);

struct WarptEmitter { u8 mPadding0[0x244]; WarptColor mPrmColor, mEnvColor; };
WWHD_SIZE(WarptEmitter, 0x24C);
WWHD_OFFSET(WarptEmitter, mPrmColor, 0x244);
WWHD_OFFSET(WarptEmitter, mEnvColor, 0x248);

// HD: explicit GHS storage lets the image check include zero-argument weak init stubs.
struct WarptModeEntry {
    be<s16> mInitAdjustment, mInitIndex;
    be<u32> mInitTarget;
    be<s16> mRunAdjustment, mRunIndex;
    be<u32> mRunTarget, mName;
};
WWHD_SIZE(WarptModeEntry, 20);
WWHD_OFFSET(WarptModeEntry, mInitTarget, 4);
WWHD_OFFSET(WarptModeEntry, mRunAdjustment, 8);
WWHD_OFFSET(WarptModeEntry, mRunTarget, 12);
WWHD_OFFSET(WarptModeEntry, mName, 16);
using WarptModeProc = void (daObj_Warpt_c::*)();
inline const gabi::ptmf<WarptModeProc>& Warpt_init(const WarptModeEntry& e) {
    return *reinterpret_cast<const gabi::ptmf<WarptModeProc>*>(&e.mInitAdjustment);
}
inline const gabi::ptmf<WarptModeProc>& Warpt_run(const WarptModeEntry& e) {
    return *reinterpret_cast<const gabi::ptmf<WarptModeProc>*>(&e.mRunAdjustment);
}
inline void Warpt_removeFollow(WarptFollow_HD* callback) {
    u32 target = Warpt_guest<WarptFollowVtable>(callback->mVtable)->mRemove;
    gabi::call_ptr(target, callback);
}
inline s32 Warpt_checkPlayerFly(u32 table, fopAc_ac_c* player) {
    u32 target = Warpt_guest<WarptPlayerVtable>(table)->mCheckPlayerFly;
    return gabi::call_ptr<s32>(target, player);
}

struct WarptTU {
    u32 mBodyCylinderData;
    u32 mLidCylinderData;
    u32 mArchiveName;
    u32 mSafeStringVtable;
    u32 mLidAssertFile;
    u32 mLidModelAssert;
    u32 mLidBrkAssert;
    u32 mBodyAssertFile;
    u32 mBodyModelAssert;
    u32 mAcchPolyVtable;
    u32 mAcchVtable;
    u32 mMoveBgTypical;
    u32 mRideCallback;
    u32 mActorVtable;
    u32 mStatusBaseVtable;
    u32 mStatusGameVtable;
    u32 mBoundsVtable;
    u32 mShapeBaseVtable;
    u32 mCylinderVtable;
    u32 mCylinderShapeVtable;
    u32 mCylinderGeometryVtable;
    u32 mAcchBaseVtable;
    u32 mHeapCallback;
    u32 mHioVtable;
    u32 mDefaultColor;
    u32 mHeaderLink0;
    u32 mHeaderLink1;
    u32 mHeaderLink2;
    u32 mWarpCut;
    u32 mEventAssertFile;
    u32 mCutNameAssert;
};

enum WarptResource {
    dRes_INDEX_LTUBW_BDL_ITUBW_e = 6, dRes_INDEX_LTUBW_BDL_ITUHU_e = 7,
    dRes_INDEX_LTUBW_BDL_YWPFM00_e = 10, dRes_INDEX_LTUBW_BRK_YWPFM00_e = 13,
    dRes_INDEX_LTUBW_DZB_ITUBW_e = 16, dRes_INDEX_LTUBW_DZB_ITUHT_e = 17,
    dRes_INDEX_LTUBW_DZB_ITUSO_e = 18,
};
enum WarptProc { PROC_INIT_e, PROC_EXEC_e };
enum WarptMode { MODE_OPEN, MODE_CLOSE, MODE_BREAK_FIRE, MODE_EVENT_WARP, MODE_EVENT_OPEN };
WWHD_OFFSET(daObj_Warpt_c, mCurMode, 0x3AC);
WWHD_OFFSET(daObj_Warpt_c, m298, 0x3B4);
WWHD_OFFSET(daObj_Warpt_c, mWarpSclsNo, 0x3B8);
WWHD_OFFSET(daObj_Warpt_c, mSpSclsNo1, 0x3BC);
WWHD_OFFSET(daObj_Warpt_c, mSpSclsNo2, 0x3C0);
WWHD_OFFSET(daObj_Warpt_c, mSpSclsNo4, 0x3C4);
WWHD_OFFSET(daObj_Warpt_c, mLidSwNo, 0x3C8);
WWHD_OFFSET(daObj_Warpt_c, mWarpSwNo, 0x3CC);
WWHD_OFFSET(daObj_Warpt_c, mPotType, 0x3D0);
WWHD_OFFSET(daObj_Warpt_c, mEventRegNo, 0x3D4);
WWHD_OFFSET(daObj_Warpt_c, m2BC, 0x3D8);
WWHD_OFFSET(daObj_Warpt_c, m2C0, 0x3DC);
WWHD_OFFSET(daObj_Warpt_c, mWarpBit, 0x3E0);
WWHD_OFFSET(daObj_Warpt_c, m2C5, 0x3E1);
WWHD_OFFSET(daObj_Warpt_c, mLidClosed, 0x3E2);
WWHD_OFFSET(daObj_Warpt_c, mPhase, 0x3E4);
WWHD_OFFSET(daObj_Warpt_c, mpBodyModel, 0x3EC);
WWHD_OFFSET(daObj_Warpt_c, mpLidModel1, 0x3F0);
WWHD_OFFSET(daObj_Warpt_c, mpBodyBgW1, 0x3F4);
WWHD_OFFSET(daObj_Warpt_c, mBodyBgW1Mtx, 0x3F8);
WWHD_OFFSET(daObj_Warpt_c, mpBodyBgW2, 0x428);
WWHD_OFFSET(daObj_Warpt_c, mBodyBgW2Mtx, 0x42C);
WWHD_OFFSET(daObj_Warpt_c, mpLidBgW, 0x45C);
WWHD_OFFSET(daObj_Warpt_c, mLidBgWMtx, 0x460);
WWHD_OFFSET(daObj_Warpt_c, mpLidModel2, 0x490);
WWHD_OFFSET(daObj_Warpt_c, mCyl1, 0x548);
WWHD_OFFSET(daObj_Warpt_c, mCyl2, 0x678);
WWHD_OFFSET(daObj_Warpt_c, mAcchCir, 0x96C);
WWHD_OFFSET(daObj_Warpt_c, mBreakFireTimer, 0x9B8);
WWHD_OFFSET(daObj_Warpt_c, mBgW2RegistTimer, 0x9BC);
WWHD_OFFSET(daObj_Warpt_c, mCloseSmokeCallBack, 0x9D4);
WWHD_OFFSET(daObj_Warpt_c, mSmokePrmColor, 0x9E8);
WWHD_OFFSET(daObj_Warpt_c, mSmokeEnvColor, 0x9EC);
WWHD_SIZE(WarptFollow_HD, 0x14);
WWHD_OFFSET(WarptFollow_HD, mEmitter, 4);
