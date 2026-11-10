/* kb_class (pig), WWHD layout. 
 *
 * GameCube -> WWHD: the two dKy_tevstr_c grew (0xB0 -> 0x1C8 each), mShadowId is gone (HD: no
 * blob shadow), so the members from mPhs on are +0x348. The eye animation (GameCube m500
 * J3DAnmTexPattern*, mpTexNoAnm, m508..m50A) is an inline mDoExt_btpAnm (0x74) in HD, so the
 * members from mBckNo (GameCube 0x50C) on are +0x3B0; the particle callbacks are +0x39C and
 * mpMaterialTable is gone, so the collision members are +0x398. Size 0xE2C (constructor 02193AC4).
 * Offsets from the verified functions. */
#pragma once
#include "bindings.h"
#include "d/actor/d_a_player.h"
#include "d/d_cc_uty.h"
#include "gc_api/JSystem/JPAEmitter.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

/* dPa_rippleEcallBack (HD 0x14): vtable, emitter, ..., rate at +0x10 */
struct KbRippleCallbackHD {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<f32> mRate;
};
struct kb_class : fopAc_ac_c {
    /* 0x3AC */ dKy_tevstr_c mTevStr;
    /* 0x574 */ dKy_tevstr_c m340;
    /* 0x73C */ request_of_phase_process_class mPhs;
    /* 0x744 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x748 */ be<u8> mShapeType;
    /* 0x749 */ be<u8> mMoveRangePrm;  /* was m401: daKb_Create 02193E48: param bits 4-7 (0xFF -> 0; 3 for the Windfall path pigs); mMoveRange = it * 100 (0 -> 300) */
    /* 0x74A */ be<u8> mbCanBeBigPig;
    /* 0x74B */ be<u8> mTgHitLatch;  /* was m403: pl_attack_hit_check (in daKb_Execute 02190CE0): 1 on the first frame of a Tg hit (the hit reaction runs only while it is 0), 0 once the hit is gone */
    /* 0x74C */ be<u8> mOnWaterGround;  /* was m404: daKb_Execute 02190CE0: 1 when the ground polygon's attribute is water (0x13), else 0 (not while carried); carry_move (in daKb_Execute) / swim_move set mTauraPos to the landing position only while it is 0; carry_check 0218EF4C clears it; swim_mode_change_check 0218F380 tests it */
    /* 0x74D */ be<u8> mPigBit;  /* was m405: daKb_Execute 02190CE0: 1 << (mShapeType & 3) while on ground of special code 4 on Outset (also ORed into event reg 0xBFFF), else 0; normal_move (in daKb_Execute) does not run at the player while it is set; the view Ob1Pig (d_a_npc_ob1.h) already calls it mPigBit */
    /* 0x74E */ be<u8> mDemoOwner;  /* was m406: esa_demo_check 02190708: 1 when this pig starts the bait demo (sets l_kbStatics.DEMO_START); daKb_Delete 02193778 and the demo end (esa_demo_move 0219509C) clear DEMO_START again only when it is set */
    /* 0x74F */ be<u8> mItemDropped;  /* was m407: money_drop 021907F4: 1 when a rupee/item was created; esa_demo_move 0219509C: 1 after digging up a daTagKbItem; picks the player's smile / other reaction (DEMO_SMILE) */
    /* 0x750 */ be<u8> mTauraPosSet;  /* was m408: TWW taura_pos_set(cXyz) (d_a_kb.h:17) sets it to 1 with mTauraPos; set/cleared by daNpcPeople (checkPig 022C3E78 tests it == 0 before tying the pig); no WWHD pig code reads it (normal_move notes the GameCube test became mbCanBeBigPig != 4) */
    /* 0x751 */ be<u8> mDigFound;  /* was m409: esa_demo_move 0219509C: 1 when the pig ends on ground of special code 5 (a dig spot, big pig) and digs; then it looks for the item tag (daTagKbItem::kb_dig) */
    /* 0x752 */ be<u8> mDigSeDone;  /* was m40A: esa_demo_move 0219509C: 0 in step 0x2C, 1 once the dig sound (JA_SE_CM_PG_DIG / _L_DIG) has played in step 0x2D */
    /* 0x753 */ be<u8> mGrabMissed;  /* was m40B: speed_pos_set 02194654: 1 while the player's grab-miss actor is this pig (animation stopped, no move), 0 after; normal_move (in daKb_Execute 02190CE0) does not turn while it is set */
    /* 0x754 */ gptr<dPath> mpPath;
    /* 0x758 */ u8 m410[0x765 - 0x758];
    /* 0x765 */ be<u8> mPathNo;  /* was m41D: daKb_Create 02193E48: param bits 16-23, dPath_GetRoomPath(it) when not 0xFF (mpPath); the Windfall pigs start at a random point of that path */
    /* 0x766 */ be<s16> mMode;  /* was m41E: daKb_Execute 02190CE0: switch 0 normal_move, 1 carry_move, 2 swim_move, 3 attack_move, 4 esa_demo_move; carry_check 0218EF4C sets 1, esa_demo_check 02190708 sets 4 */
    /* 0x768 */ be<s16> mAction;  /* was m420: daKb_Execute 02190CE0: the step inside mMode (normal 0-6, carry 0xA-0xD, swim 0x14-0x19, attack 0x21-0x24, bait demo 0x28-0x31), switched on by normal_move/carry_move/swim_move, attack_move 021947AC and esa_demo_move 0219509C */
    /* 0x76A */ be<s16> mTargetAngleY;  /* was m422: normal_move / swim_move (in daKb_Execute 02190CE0): current.angle.y turns to it (cLib_addCalcAngleS2 with mTurnStep); the angle to mTargetPos, target_set 02190400 (case 2) */
    /* 0x76C */ be<s16> mTurnStep;  /* was m424: normal_move (in daKb_Execute 02190CE0): the step of the turn to mTargetAngleY, 0x800 walking, 0x2000 running away */
    /* 0x76E */ be<s16> mTimers[8];  /* was m426: daKb_Execute 02190CE0: 8 timers counted down every frame ([0] the action timer, [1] no-flee wait, [2] voice/struggle wait, [3] keep-direction wait (target_set 02190400), [4] bait-demo cool-down (esa_demo_check 02190708), [5] anger flash, [6] swim voice, [7] first ground check, 5 at daKb_Create 02193E48) */
    /* 0x77E */ be<s16> mFlipState;  /* was m436: carry_move (in daKb_Execute 02190CE0): 0 while thrown (shape_angle.x spins), 1 after the first bounce; swim_move: + 0x800 per frame, the phase of the bobbing (sin) */
    /* 0x780 */ be<s16> mFallFrame;  /* was m438: carry_move (in daKb_Execute 02190CE0): + 1 per frame the carried pig is not at mCarryStartY; at 4 the squeal particle (particle_tuba); carry_check 0218EF4C / swim_move clear it */
    /* 0x782 */ be<s16> mSwimTurnWait;  /* was m43A: swim_move (in daKb_Execute 02190CE0): 0 -> target_set(2) then 1; after a wall a random 5-25 turn; counted down while facing mTargetAngleY */
    /* 0x784 */ u8 m43C[4];
    /* 0x788 */ be<s16> mInWater;  /* was m440: sibuki_set 0218F1FC sets 1 (splash on entering water); carry_move (in daKb_Execute 02190CE0) lands in water (swim, mMode 2) when set; swim_move clears it when out; swim_mode_change_check 0218F380 tests it */
    /* 0x78A */ be<s16> mAttackAngleY;  /* was m442: carry_move (in daKb_Execute 02190CE0): the player angle when the angry run starts; attack_move 021947AC turns shape_angle.y to it and picks a new one (+- rnd 4096) when reached */
    /* 0x78C */ be<s16> mLandSquash;  /* was m444: carry_move (in daKb_Execute 02190CE0): 6 on landing (0x1E for mbCanBeBigPig 4); daKb_Execute counts it down and squashes mScale.y (+0.5625 above 3, +1.125 below); no move while above 3 */
    /* 0x78E */ be<s16> m446;
    /* 0x790 */ be<s16> mStallFrames;  /* was m448: daKb_Execute 02190CE0: in attack (mMode 3, action >= 0x24) + 1 per frame the pig moves less than 1; above 10 the anger ends (ALL_ANGER 0, back to mMode 0); carry_move clears it */
    /* 0x792 */ be<s16> mRepeatCount;  /* was m44A: carry_move (in daKb_Execute 02190CE0): 3 struggle animations (JITA2) while carried, - 1 each; esa_demo_move 0219509C: the eat animation count (1) */
    /* 0x794 */ be<s16> mAngerFlashPhase;  /* was m44C: daKb_Execute 02190CE0: + REG + 1000 per frame while angry (mTimers[5] or actions 0x21-0x24); |sin| of it drives mAngerColorRate */
    /* 0x796 */ be<s16> m44E;
    /* 0x798 */ cXyz mTauraPos;  /* was m450: TWW taura_pos_set(cXyz) (d_a_kb.h:17) sets it with mTauraPosSet (daNpcPeople 022C3E78); daKb_Execute 02190CE0 sets it to the first ground position; normal_move (in daKb_Execute) heads back when farther than mMoveRange / 2; target_set 02190400 (case 2) aims at it */
    /* 0x7A4 */ cXyz mTargetPos;  /* was m45C: target_set 02190400: 1000 ahead in the chosen direction or the bait's position; normal_move (in daKb_Execute 02190CE0) walks towards it (mTargetAngleY) */
    /* 0x7B0 */ cXyz field_0x468[2];
    /* 0x7C8 */ cXyz field_0x480[2];
    /* 0x7E0 */ cXyz field_0x498;
    /* 0x7EC */ cXyz mTailPos;  /* was m4A4: nodeCallBack 0218F98C: MtxPosition of the J_PG_TAIL joint; he_set 0218F4EC starts the pig gas (ID_AK_SN_PIGGAS00) there */
    /* 0x7F8 */ be<f32> mAngerColorRate;  /* was m4B0: daKb_Execute 02190CE0: -> the anger flash value (from mAngerFlashPhase), else 0; daKb_Draw 0218FC3C blends the C0/K0 colours towards red by it */
    /* 0x7FC */ be<f32> mTargetSpeed;  /* was m4B4: normal_move (in daKb_Execute 02190CE0): 0 wait, 3 walk, 12 (6 big) run; speedF follows it with step mSpeedStep */
    /* 0x800 */ be<f32> mSpeedStep;  /* was m4B8: normal_move (in daKb_Execute 02190CE0): 2.5 / 2.0, the cLib_addCalc2 step of speedF towards mTargetSpeed */
    /* 0x804 */ be<f32> mCarryStartY;  /* was m4BC: carry_check 0218EF4C: current.pos.y when the pig is picked up; carry_move (in daKb_Execute 02190CE0) starts the fall reaction once the height differs */
    /* 0x808 */ be<f32> mDrawOffsetY;  /* was m4C0: draw_SUB 0218FABC: the model's y translation, -> mDrawOffsetYTarget (not in mMode 4); daKb_Create 02193E48 30; esa_demo_move 0219509C moves it (-33 dig / 30) */
    /* 0x80C */ be<f32> mDrawOffsetYTarget;  /* was m4C4: draw_SUB 0218FABC: mDrawOffsetY follows it; 30 normally, 20 while carried and falling (carry_move in daKb_Execute 02190CE0), 30 at sibuki_set 0218F1FC */
    /* 0x810 */ be<f32> mStruggleAnmSpeed;  /* was m4C8: carry_move (in daKb_Execute 02190CE0): play speed of the struggle animation BCK_JITA2, 1.8 at the start, - 0.6 per repeat (not below 0) */
    /* 0x814 */ be<f32> mMoveRange;  /* was m4CC: daKb_Create 02193E48: mMoveRangePrm * 100 (0 -> 300); normal_move (in daKb_Execute 02190CE0) returns to mTauraPos when farther than it / 2; 0 (no limit) after fleeing or being carried */
    /* 0x818 */ u8 m4D0[4];
    /* 0x81C */ be<f32> mSizeRate;  /* was m4D4: daKb_Create 02193E48: 2 for the big pig (mShapeType + 8), else 0; mScale = it + 1, and it scales the collider, eye/attention height, ripples and smoke (daKb_Execute 02190CE0, sibuki_set 0218F1FC, smoke_set 0218F600) */
    /* 0x820 */ be<u32> mEsaId;  /* was m4D8: esa_demo_check 02190708: fopAcM_GetID(the bait (esa) actor); target_set 02190400 / esa_demo_move 0219509C look it up; carry_check 0218EF4C releases the bait and sets fpcM_ERROR_PROCESS_ID_e */
    /* 0x824 */ cXyz mHitPos;  /* was m4DC: pl_attack_hit_check (in daKb_Execute 02190CE0): mSph.GetTgHitPosP(); attack_move 021947AC runs from it */
    /* 0x830 */ cXyz mScale;  /* was m4E8: daKb_Create 02193E48: mSizeRate + 1 (x = y = z); draw_SUB 0218FABC MtxScale(x, y, x); daKb_Execute 02190CE0 squashes y on landing (mLandSquash) */
    /* 0x83C */ csXyz mGroundAngle;  /* was m4F4: daKb_Execute 02190CE0: fopAcM_getGroundAngle; draw_SUB 0218FABC tilts the model by x/z; carry_move returns it to 0 */
    /* 0x842 */ csXyz mAttackTilt;  /* was m4FA: attack_move 021947AC: target x/z of shape_angle while running angry (from the ground slope), shape_angle follows it */
    /* 0x848 */ mDoExt_btpAnm mBtp; /* HD: GameCube m500/mpTexNoAnm/m508..m50A */
    /* 0x8BC */ be<s32> mBckNo;  /* was m50C: anm_init 0218EB64: the started BCK resource index; normal_move / pl_attack_hit_check (in daKb_Execute 02190CE0) restart WAIT1/WALK1/RUN1 only when it differs */
    /* 0x8C0 */ cXyz mCamEye;  /* was m514: esa_demo_move 0219509C: the bait demo camera's eye (copied into dCamera_Set's eye); demo camera eye */
    /* 0x8CC */ cXyz mCamCenter;  /* was m520: set_cam_center (in esa_demo_move 0219509C): current.pos + (REG, 60 + REG, REG), the bait demo camera's center; demo camera centre */
    /* 0x8D8 */ be<f32> mCamFovy;  /* was m53C: esa_demo_move 0219509C: 50, the fovy argument of dCamera_Set; demo camera fovy */
    /* 0x8DC */ KbRippleCallbackHD mRippleCb;  /* was m540: hamon_set 0218F150: the ripple (ID_AK_JN_HAMON00) emitter callback; mRate set from mRippleScale; carry_check 0218EF4C / swim_move end it */
    /* 0x8F0 */ dPa_smokeEcallBack mSmokeCb;  /* was m554: smoke_set 0218F600 / smoke_set2 0218F71C: the smoke (ELEMENTSMOKE00) emitter callback; ended when the anger ends (daKb_Execute 02190CE0) */
    /* 0x910 */ dPa_smokeEcallBack mDigSmokeCb;  /* was m574: smoke_set3 0218F868: the dig smoke emitter callback (onWindOff, scaled); esa_demo_move 0219509C ends it after digging */
    /* 0x930 */ dPa_followEcallBack mGasCb;  /* was m594: he_set 0218F4EC: the pig gas (ID_AK_SN_PIGGAS00) follow callback at mTailPos; carry_move (in daKb_Execute 02190CE0) ends it */
    /* 0x944 */ dPa_followEcallBack mShakeWaterCb[2];  /* was m5A8: swim_move (in daKb_Execute 02190CE0): the two shake-off-water (ID_AK_SN_PIGDASSUI) follow callbacks, frames 10/11 of BCK_DASSUI; pl_attack_hit_check ends them */
    /* 0x96C */ be<f32> mRippleScale;  /* was m5D0: hamon_set 0218F150: scale of the ripple; sibuki_set 0218F1FC / swim_mode_change_check 0218F380 set it from mSizeRate, swim_move 0.5 */
    /* 0x970 */ cXyz mParticlePos[2];  /* was m5D4: he_set 0218F4EC / smoke_set 0218F600 / smoke_set2 0218F71C / smoke_set3 0218F868: positions the particles start at ([0] body/tail, [1] the dig spot) */
    /* 0x988 */ csXyz mParticleAngle[2];  /* was m5EC: he_set 0218F4EC / smoke_set 0218F600 / attack_move 021947AC: angles for the particles at mParticlePos */
    /* 0x994 */ dBgS_AcchCir mAcchCir;
    /* 0x9D4 */ dBgS_ObjAcch mAcch;
    /* 0xB98 */ dCcD_Stts mStts;
    /* 0xBD4 */ dCcD_Sph mSph;
    /* 0xD00 */ dCcD_Sph m968;
};
WWHD_OFFSET(kb_class, mPhs, 0x73C);
WWHD_OFFSET(kb_class, mMode, 0x766);
WWHD_OFFSET(kb_class, mEsaId, 0x820);
WWHD_OFFSET(kb_class, mBtp, 0x848);
WWHD_OFFSET(kb_class, mRippleCb, 0x8DC);
WWHD_OFFSET(kb_class, mRippleScale, 0x96C);
WWHD_OFFSET(kb_class, mAcchCir, 0x994);
WWHD_OFFSET(kb_class, mStts, 0xB98);
WWHD_OFFSET(kb_class, m968, 0xD00);
WWHD_SIZE(kb_class, 0xE2C);

// HD: named virtual slots retain the native dispatch and object argument.
struct KbCallbackVtable {
    u8 prefix[0x44];
    gfn remove;
};
struct KbCallbackObject {
    gptr<KbCallbackVtable> __vtbl;
};
struct KbSafeStringVtable {
    u8 prefix[0x14];
    gfn assureTermination;
};
struct KbGrabVtable {
    u8 prefix[0x44];
    gfn getGrabMissActor;
};
WWHD_OFFSET(KbCallbackVtable, remove, 0x44);
WWHD_OFFSET(KbSafeStringVtable, assureTermination, 0x14);
WWHD_OFFSET(KbGrabVtable, getGrabMissActor, 0x44);
struct KbJ3DSysState {
    gptr<J3DModel> mModel;
};
extern KbJ3DSysState &l_j3dSys;
extern Mtx34 &l_currentMtx;
inline dComIfG_play_c *kbPlay();

/* acch members */
inline f32 kb_wtr_height(kb_class *i_this) {
    return i_this->mAcch.m_wtr.GetHeight();
} /* m_wtr.GetHeight() */
inline cBgS_PolyInfo *kb_gnd_poly(kb_class *i_this) {
    return &i_this->mAcch.m_gnd;
}

void kbGuest_cMtx_YrotS(Mtx34 *matrix, u32 angle);
u32 kbGuest_followEcallBack_ct(void *p, u8 a, u8 b);
void kbGuest_dCcD_Sph_ct(dCcD_Sph *p);
void kbGuest_dCcD_Sph_dt(dCcD_Sph *p, s32 flags);
void kbGuest_dBgS_Acch_dt(u32 p, s32 flags);
void kbGuest_cM3dGCir_dt(u32 p, s32 flags);
void kbGuest_fopAc_ac_c_dt(kb_class *p, s32 flags);
void kbGuest_cBgS_GndChk_dt(void *p, s32 flags);
void kbGuest_dCamera_ForceLockOff(u32 camera, u32 id);
void *kbGuest_getJointTree(J3DModelData *p);
/* ---- this TU's constants ---- */
#define SAFESTRING_VTBL 0x100127E0 /* this TU's sead::SafeString vtable */
enum { PROC_ESA = 0xDD, PROC_TAG_KB_ITEM = 0x1A1 };
enum { DSNAP_TYPE_KB = 0x54 };
enum { PG_JNT_J_PG_TAIL_e = 12 };
enum {
    dRes_INDEX_KB_BAS_JITA2_e = 9,
    dRes_INDEX_KB_BCK_JITA2_e = 0x14,
};
enum {
    JA_SE_CV_PG_NORMAL = 0x4820,
    JA_SE_CV_PG_TURN = 0x4821,
    JA_SE_CV_PG_CATCH = 0x4822,
    JA_SE_CV_PG_CARRY = 0x4823,
    JA_SE_CV_PG_L_NORMAL = 0x4915,
    JA_SE_CV_PG_L_TURN = 0x4916,
    JA_SE_CV_PG_L_CATCH = 0x4917,
    JA_SE_CV_PG_L_CARRY = 0x4918,
    JA_SE_OBJ_FALL_WATER_S = 0x6918,
};
/* file statics */
struct KbStatics {
    be<u8> ALL_ANGER, DEMO_START;
};
WWHD_SIZE(KbStatics, 2);
extern KbStatics &l_kbStatics;

/* this TU's dBgS_LinChk vtables (constructor and destructor) */
static const dBgS_LinChk_vt LINCHK_VT = {0x10012888, 0x10012898, 0x100128B8, 0x100128A8};
static inline void dBgS_LinChk_dt(void *c) {
    auto *check = (dBgS_LinChk *)c;
    check->dBgS_PolyPassChk::__vtbl = LINCHK_VT.v58;
    check->dBgS_GrpPassChk::__vtbl = 0x10012808;
    check->cBgS_PolyInfo::__vtbl = 0x100127F8;
    cBgS_LinChk_dt(c, 0);
}

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* esa_class (HD +0x11C): mTaken (u8) at 0x3B4, mState (s8) at 0x3BD */

/* attention_info.flags (+0x39C) */
static inline be<u32> &attn_flags(fopAc_ac_c *a) {
    return a->attention_info.flags;
}
static inline bool fopAcM_checkCarryNow(fopAc_ac_c *a) {
    return (a->actor_status & 0x2000) != 0;
}
/* 025E1AA4 kbGuest_mDoAud_monsSeStart(id, pos, procId, param, reverb) */
void kbGuest_mDoAud_monsSeStart(u32 id, cXyz *pos, u32 pid, u32 param, s32 reverb);
/* fopAcM_seStart / fopAcM_monsSeStart: the HD inlines test &eyePos only */
static inline void kb_se_start(fopAc_ac_c *a, u32 id) {
    cXyz *eye = &a->eyePos;
    if (gabi::ea(eye) != 0)
        mDoAud_seStart(id, eye, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline void kb_mons_se_start(fopAc_ac_c *a, u32 id) {
    cXyz *eye = &a->eyePos;
    if (gabi::ea(eye) != 0) {
        u32 pid = a != nullptr ? (u32)a->mBsPcId : 0xFFFFFFFFu; /* fopAcM_GetID */
        kbGuest_mDoAud_monsSeStart(id, eye, pid, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
    }
}
static inline void kb_catch_se(kb_class *i_this) {
    if (i_this->mShapeType >= 8) {
        kb_mons_se_start(i_this, JA_SE_CV_PG_L_CATCH);
    } else {
        kb_mons_se_start(i_this, JA_SE_CV_PG_CATCH);
    }
}
// HD: full projection of the emitter fields used by the pig's smoke and water effects.
struct KbEmitterHD {
    u8 prefix[0x34];
    be<f32> mRate;
    u8 pad38[0x20];
    be<f32> mSpread;
    be<s32> mMaxFrame;
    be<s16> mLifeTime;
    u8 pad62[6];
    be<f32> mInitialVelOmni, mInitialVelAxis, mInitialVelDir;
    u8 pad74[0x1F0 - 0x74];
    be<f32> mGlobalRotation[9];
    u8 pad214[0xC];
    cXyz mGlobalScale, mGlobalTranslation, mGlobalParticleScale;
};
WWHD_OFFSET(KbEmitterHD, mSpread, 0x58);
WWHD_OFFSET(KbEmitterHD, mGlobalRotation, 0x1F0);
WWHD_OFFSET(KbEmitterHD, mGlobalScale, 0x220);
WWHD_OFFSET(KbEmitterHD, mGlobalTranslation, 0x22C);
WWHD_OFFSET(KbEmitterHD, mGlobalParticleScale, 0x238);
inline KbEmitterHD *kbEmitter(u32 address) {
    return gabi::at<KbEmitterHD>(address);
}

/* particle emitters: JPABaseEmitter::setGlobalScale (HD: +0x220.. and +0x238..) */
template <class CB> static inline u32 emitter_ea(CB &cb) {
    return gabi::ea(cb.mpEmitter.get());
}
static inline void set_global_scale(u32 e, f32 x, f32 y, f32 z) {
    kbEmitter(e)->mGlobalScale.x = x;
    kbEmitter(e)->mGlobalScale.y = y;
    kbEmitter(e)->mGlobalScale.z = z;
    kbEmitter(e)->mGlobalParticleScale.x = x;
    kbEmitter(e)->mGlobalParticleScale.y = y;
    kbEmitter(e)->mGlobalParticleScale.z = z;
}
/* virtual remove() (vtable +0x44) of the particle callbacks */
template <class CB> static inline void vremove(CB *cb) {
    gabi::call_ptr(gabi::at<KbCallbackObject>(gabi::ea(cb))->__vtbl->remove.get(), cb);
}
void kbGuest_JPASetRMtxTVecfromMtx(Mtx34 *m, u32 r, u32 t);
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint block at model+0x2C */
static inline Mtx34 *getAnmMtx(J3DModel *model, s32 joint) {
    return model->getAnmMtx(joint);
}
static inline J3DModel *j3dSys_getModel() {
    return l_j3dSys.mModel;
}
static inline Mtx34 *J3DSys_mCurrentMtx() {
    return &l_currentMtx;
}
/* dBgS::GetRoomId / GetPolyColor */
s32 kbGuest_dBgS_GetRoomId(dBgS *bgs, cBgS_PolyInfo *p);
s32 kbGuest_dBgS_GetPolyColor(dBgS *bgs, cBgS_PolyInfo *p);
/* 0200FAAC cLib_distanceAngleS: HD returns abs((s16)(a - b)) as an int (0..0x8000); callers compare
 * the whole register (the shared binding's s16 result would turn 0x8000 negative) */
s32 kbGuest_cLib_distanceAngleS_i(s16 a, s16 b);
/* 025D8AB0 kbGuest_fopAcM_fastCreateItem(pos, itemNo, roomNo, rot, scale, speedF, speedY, gravity,
 * bitNo, createFunc) */
fopAc_ac_c *kbGuest_fopAcM_fastCreateItem(cXyz *pos, s32 itemNo, s32 roomNo, csXyz *rot,
                                          cXyz *scale, f32 speedF, f32 speedY, f32 gravity,
                                          s32 bitNo, u32 createFunc);
void kbGuest_dKy_tevstr_init(dKy_tevstr_c *t, s8 roomNo, u8 p);
/* 027F3F94 (the matcher names it __nw): returns the model data's joint tree; joint count (u16) at
 * +8 */
static inline u16 J3DModelData_getJointNum(J3DModelData *d) {
    return d->getJointNum();
}
/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index
 * checked against +4) */
static inline void setJointCallBack(J3DModelData *d, u16 i, u32 cb) {
    d->getJointNodePointer(i)->mCallBack = cb;
}
/* mDoExt_btpAnm (HD): constructor 025E7820, init 025E789C */
void kbGuest_mDoExt_btpAnm_ct(void *p);
BOOL kbGuest_mDoExt_btpAnm_init(void *a, J3DModelData *d, void *btp, s32 anmPlay, s32 mode,
                                f32 rate, s16 start, s16 end, bool modify, s32 entry);
/* 025A9084 dPa_rippleEcallBack::dPa_rippleEcallBack, 025A5B18
 * dPa_smokeEcallBack::dPa_smokeEcallBack(u8) */
void kbGuest_dPa_rippleEcallBack_ct(void *p);
void kbGuest_dPa_smokeEcallBack_ct(void *p, u8 a);
/* GHS array helpers: kbGuest___construct_array(ptr, n, size, ctor), kbGuest___destroy_arr(ptr, n,
 * size, dtor, flags) */
void kbGuest___construct_array(void *p, u32 n, u32 size, u32 ctor);
void kbGuest___destroy_arr(void *p, u32 n, u32 size, u32 dtor, u32 flags);
/* sead::SafeString equality (HD inline): both sides' virtual assureTermination (+0x14), then a
 * byte compare bounded by 0x40001 */
static inline bool SafeString_eq(SafeString *a, SafeString *b) {
    gabi::call_ptr(gabi::at<KbSafeStringVtable>(a->__vtbl)->assureTermination.get(), a);
    gabi::call_ptr(gabi::at<KbSafeStringVtable>(a->__vtbl)->assureTermination.get(), a);
    u32 s1 = a->mTextPtr;
    gabi::call_ptr(gabi::at<KbSafeStringVtable>(b->__vtbl)->assureTermination.get(), b);
    if (s1 == b->mTextPtr)
        return true;
    u32 p = a->mTextPtr, q = b->mTextPtr;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(p + i);
        if (c != gabi::load<u8>(q + i))
            return false;
        if (c == 0)
            return true;
    }
    return false;
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (HD: SafeString comparison; the start stage name
 * at play+0x5134) */
static inline bool kbIsStartStage(const char *lit) {
    gabi::Local<SafeString> a;
    a->mTextPtr = gabi::guest_str(lit);
    a->__vtbl = SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mTextPtr = gabi::ea(kbPlay()->mCurStage.mName);
    b->__vtbl = SAFESTRING_VTBL;
    return SafeString_eq(a, b);
}
/* dBgS_GndChk on the stack (this TU's vtables) */
static const dBgS_GndChk_vt GNDCHK_VT = {0x10012818, 0x10012828, 0x10012848, 0x10012838};
static inline void dBgS_GndChk_dt(void *c) {
    auto *check = (dBgS_GndChk *)c;
    check->cBgS_PolyInfo::__vtbl = GNDCHK_VT.v20;
    check->dBgS_PolyPassChk::__vtbl = GNDCHK_VT.v40;
    check->dBgS_GrpPassChk::__vtbl = 0x10012808;
    kbGuest_cBgS_GndChk_dt(c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}
#define KB_VTBL 0x100128C8 /* kb_class vtable (HD virtual destructor) */
static const dBgS_ObjAcch_vt OBJACCH_VT = {0x10012858, 0x10012878, 0x10012868};

enum { dIsleRoom_WindfallIsland_e = 11, dIsleRoom_OutsetIsland_e = 44 };
enum { dRes_INDEX_KB_BAS_WAIT1_e = 0xC, dRes_INDEX_KB_BCK_WAIT1_e = 0x17 };
enum { JA_SE_OBJ_LUPY_OUT = 0x69E9, JA_SE_OBJ_ITEM_OUT = 0x69ED };
/* kb_btp_idx (0x101B7FC4) / kb_bdl_idx (HD, 0x101B7FD4): u16[7] */

/* daPy_py_c::getGrabMissActor (virtual, HD vtable at +0xB4, slot +0x44) */
static inline fopAc_ac_c *daPy_getGrabMissActor(fopAc_ac_c *player) {
    return gabi::call_ptr<fopAc_ac_c *>(
        gabi::at<KbGrabVtable>(player->__vtbl)->getGrabMissActor.get(), player);
}
enum {
    dRes_INDEX_KB_BCK_DAMAGE1_e = 0x10,
    dRes_INDEX_KB_BAS_RUN1_e = 0xB,
    dRes_INDEX_KB_BCK_RUN1_e = 0x16
};
/* 02515BBC dCcD_GAtTgCoCommonBase::GetAc */
fopAc_ac_c *kbGuest_dCcD_GAtTgCoCommonBase_GetAc(void *p);
static inline void kb_mons_se2(kb_class *i_this, u32 big, u32 normal) {
    if (i_this->mShapeType >= 8) {
        kb_mons_se_start(i_this, big);
    } else {
        kb_mons_se_start(i_this, normal);
    }
}
enum {
    dRes_INDEX_KB_BAS_EAT1_e = 7,
    dRes_INDEX_KB_BAS_NAKU1_e = 0xA,
    dRes_INDEX_KB_BAS_WALK1_e = 0xD,
    dRes_INDEX_KB_BCK_EAT1_e = 0x12,
    dRes_INDEX_KB_BCK_NAKU1_e = 0x15,
    dRes_INDEX_KB_BCK_WALK1_e = 0x18,
};
enum {
    JA_SE_CM_PG_DIG = 0x5921,
    JA_SE_CM_PG_DIG_LANDING = 0x5922,
    JA_SE_CM_PG_L_JUMP = 0x593E,
    JA_SE_CM_PG_L_DIG = 0x593F,
};
enum { PROC_KS = 0xCD };
/* daPy_py_c demo (HD: mDemo at +0x420: type u16 +0x420, param0 +0x428, mode +0x430) */
enum {
    daPy_demo_DEMO_N_WAIT_e = 1,
    daPy_demo_DEMO_UNK_018_e = 0x12,
    daPy_demo_DEMO_SMILE_e = 0x32
};
static inline void daPy_changeOriginalDemo(fopAc_ac_c *pl) {
    ((daPy_py_c *)pl)->mDemo.mParam0 = 0;
    ((daPy_py_c *)pl)->mDemo.mDemoType = 3;
}
static inline void daPy_cancelOriginalDemo(fopAc_ac_c *pl) {
    ((daPy_py_c *)pl)->mDemo.mDemoType = 2;
    ((daPy_py_c *)pl)->mDemo.mDemoMode = 1;
}
static inline void daPy_changeDemoMode(fopAc_ac_c *pl, u32 mode) {
    ((daPy_py_c *)pl)->mDemo.mDemoMode = mode;
}
/* camera_process_class* dComIfGp_getCamera(dComIfGp_getPlayerCameraID(0)): play+0x5AF8 + id * 0x34,
 * id (s8) at play+0x5B30; its dCamera_c at +0x248 */
static inline u8 *kb_getPlayerCamera() {
    s8 id = dComIfGp_getPlayerCameraID(0);
    return reinterpret_cast<u8 *>(dComIfGp_getCamera(id));
}
// HD: the camera process prefix is opaque in the shared framework layout.
struct KbCameraProcessHD {
    u8 prefix[0x248];
    u8 mCamera;
};
WWHD_OFFSET(KbCameraProcessHD, mCamera, 0x248);
static inline dCamera_c *camera_body(u8 *cam) {
    return reinterpret_cast<dCamera_c *>(&gabi::at<KbCameraProcessHD>(gabi::ea(cam))->mCamera);
}
void kbGuest_dCamera_Stop(dCamera_c *c);
void kbGuest_dCamera_Start(dCamera_c *c);
void kbGuest_dCamera_SetTrimSize(dCamera_c *c, s32 s);
/* Set(cXyz center, cXyz eye, f32 fovy, s16 bank) / Reset(cXyz center, cXyz eye): by value (pointers
 * to copies) */
void kbGuest_dCamera_Set(dCamera_c *c, cXyz *center, cXyz *eye, f32 fovy, s16 bank);
void kbGuest_dCamera_Reset(dCamera_c *c, cXyz *center, cXyz *eye);
BOOL kbGuest_fopAcM_orderPotentialEvent(fopAc_ac_c *a, u16 type, u16 flag, u16 p);
s32 kbGuest_dBgS_GetSpecialCode(dBgS *bgs, cBgS_PolyInfo *p);
s32 kbGuest_dBgS_GetAttributeCode(dBgS *bgs, cBgS_PolyInfo *p);
/* 02526560 daTagKbItem_c::kb_dig(fopAc_ac_c*) (unnamed by the matcher) */
void kbGuest_daTagKbItem_kb_dig(void *item, fopAc_ac_c *pig);
/* fopAcM_seStart / monsSeStart variants that also test the actor */
static inline void kb_se_start_a(fopAc_ac_c *a, u32 id) {
    if (a != nullptr)
        kb_se_start(a, id);
}
static inline void kb_mons_se_start_a(fopAc_ac_c *a, u32 id) {
    if (a != nullptr)
        kb_mons_se_start(a, id);
}
enum {
    dRes_INDEX_KB_BAS_JITA1_e = 8,
    dRes_INDEX_KB_BCK_DASSUI_e = 0x11,
    dRes_INDEX_KB_BCK_JITA1_e = 0x13
};
enum { daPyStts0_CRAWL_e = 0x08000000 };
/* 025192A8 kbGuest_cc_at_check(fopAc_ac_c*, CcAtInfo*) (SHARED-CANDIDATE, as in d_a_pt/d_a_ph) */
void kbGuest_cc_at_check(fopAc_ac_c *a, CcAtInfo *info);
/* dComIfGs_getSelectEquip(0): save info (0x101F84DC) +0x2E */
static inline u8 dComIfGs_getSelectEquip0() {
    return dComIfGs_getSelectEquip(0);
}
void kbGuest_cBgS_GetTriPnt(dBgS *bgs, void *poly, cXyz *a, cXyz *b, cXyz *c);
/* dComIfGp_getCamera(0)->mCamera.ForceLockOff(id) */
static inline void dCamera_ForceLockOff(fopAc_ac_c *a) {
    dCamera_c *camera = camera_body(reinterpret_cast<u8 *>(dComIfGp_getCamera(0)));
    kbGuest_dCamera_ForceLockOff(gabi::ea(camera), fopAcM_GetID(a));
}
void kbGuest_dCcMassS_Mng_Set(void *manager, void *obj, u8 p);

// Cross-file calls in this translation unit.
u32 kbGuest_way_check_raw(kb_class *i_this, s16 angle, u8 p);
void money_drop(kb_class *i_this);
void smoke_set3(kb_class *i_this);
void smoke_set2(kb_class *i_this);
BOOL swim_mode_change_check(kb_class *i_this);
void smoke_set(kb_class *i_this);
void anm_init(kb_class *i_this, int bck, f32 morf, u8 loop, f32 speed, int bas);
void esa_demo_move(kb_class *i_this);
void attack_move(kb_class *i_this);
f32 kbGuest_JMASinShort_(u32 table, s16 a);
void speed_pos_set(kb_class *i_this);
void cLib_offBit_u32(be<u32> *v, u32 bit);
BOOL kb_eye_tex_anm(kb_class *i_this);
void BG_check(kb_class *i_this);
BOOL esa_demo_check(kb_class *i_this);
void target_set(kb_class *i_this, u8 p);
void draw_SUB(kb_class *i_this);
void he_set(kb_class *i_this);
void sibuki_set(kb_class *i_this);
BOOL carry_check(kb_class *i_this);

inline dComIfG_play_c *kbPlay() {
    return gabi::at<dComIfG_play_c>(dComIfGp_ea() + offsetof(dComIfG_inf_c, play));
}

// HD: mass manager inside the collision scene; shared cCcS leaves this member opaque.
struct KbCollisionSceneHD {
    u8 prefix[0x2854];
    u8 mMassManager;
};
WWHD_OFFSET(KbCollisionSceneHD, mMassManager, 0x2854);
inline void *kbMassManager() {
    return &gabi::at<KbCollisionSceneHD>(gabi::ea(&kbPlay()->mCcS))->mMassManager;
}

// HD: three enlarged light objects in tev state; typed fields preserve the native copy order.
struct KbLightObjectHD {
    cXyz position, direction;
    be<u8> color[4];
    be<s16> ambient[4];
    be<f32> intensity[2], cosAtten[3], distanceAtten[3];
    u8 unchangedTail[0x40];
};
WWHD_SIZE(KbLightObjectHD, 0x84);
WWHD_OFFSET(KbLightObjectHD, intensity, 0x24);
struct KbLightingHD {
    KbLightObjectHD light;
    u8 colorAndFog[0x3C];
    KbLightObjectHD materialLight, secondMaterialLight;
};
WWHD_SIZE(KbLightingHD, 0x1C8);
WWHD_OFFSET(KbLightingHD, materialLight, 0xC0);
WWHD_OFFSET(KbLightingHD, secondMaterialLight, 0x144);
struct KbAmbientBytes {
    be<u8> bytes[8];
};
struct KbTevPositionColorBytes {
    be<u8> bytes[0x30];
};
struct KbTevControlBytes {
    be<u8> bytes[9];
};
inline KbLightingHD *kbLighting(dKy_tevstr_c *t) {
    return gabi::at<KbLightingHD>(gabi::ea(t));
}
inline KbAmbientBytes *kbAmbientBytes(KbLightObjectHD *l) {
    return gabi::at<KbAmbientBytes>(gabi::ea(&l->ambient[0]));
}
inline KbTevPositionColorBytes *kbPositionColorBytes(dKy_tevstr_c *t) {
    return gabi::at<KbTevPositionColorBytes>(gabi::ea(&t->mLightPosWorld));
}
inline KbTevControlBytes *kbControlBytes(dKy_tevstr_c *t) {
    return gabi::at<KbTevControlBytes>(gabi::ea(&t->mInitTimer));
}

// HD: food actor members used by the pig; the shared actor remains opaque.
struct KbFoodActorHD : fopAc_ac_c {
    u8 prefix[8];
    be<u8> mTaken;  /* was field_0x298: esa_class mTaken (d_a_esa.h) */
    u8 pad3B5[8];
    be<s8> mState;
};
WWHD_OFFSET(KbFoodActorHD, mTaken, 0x3B4);
WWHD_OFFSET(KbFoodActorHD, mState, 0x3BD);
struct KbJointDataHD {
    u8 prefix[4];
    be<u16> mJntNo;
};
struct KbPathPointHD {
    be<u8> mArg[4];
    cXyz m_position;
};
struct KbPathHD {
    be<u16> m_num, m_nextID;
    be<u8> mArg0, m_closed, field4_0x6, field5_0x7;
    gptr<KbPathPointHD> m_points;
};
WWHD_SIZE(KbPathPointHD, 0x10);
WWHD_OFFSET(KbPathHD, m_points, 8);
struct KbSineEntry {
    be<f32> sine, cosine;
};
struct KbPlayerVtableHD {
    u8 prefix[0x114];
    gfn setPlayerPosAndAngle;
};
WWHD_OFFSET(KbPlayerVtableHD, setPlayerPosAndAngle, 0x114);
inline be<u8> &esa_mTaken(fopAc_ac_c *esa) {
    return ((KbFoodActorHD *)esa)->mTaken;
}
inline s8 esa_mState(fopAc_ac_c *esa) {
    return ((KbFoodActorHD *)esa)->mState;
}

void *item_tag_search(void *param_1, void *param_2);

WWHD_SIZE(KbRippleCallbackHD, 0x14);
WWHD_OFFSET(KbRippleCallbackHD, mRate, 0x10);

extern KbSineEntry &l_sincosTable;
