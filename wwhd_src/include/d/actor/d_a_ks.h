/* ks_class (Morth), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c +0x11C; HD inserts two mDoExt_J3DModelPacketS (0xB0 each,
 * constructor 02080404) after the anm pointers, so the members from mType (GameCube 0x2C8) on are
 * +0x27C. enemyice keeps its GameCube layout (0x3B8), enemyfire is 0x22C. Size 0xF08 (constructor
 * 021AA364; GameCube 0xC8C). */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"

#ifndef WWHD_ENEMYICE_FIRE_L
#define WWHD_ENEMYICE_FIRE_L
/* enemyice / enemyfire (c_damagereaction), WWHD layouts (SHARED-CANDIDATE, same as d_a_cc.h) */
struct enemyice_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ be<s8> mFreezeType;  /* was m00C: view of enemyice::mFreezeType (c/c_damagereaction.h) */
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 _016[2];
    /* 0x018 */ cXyz mSpeed;
    /* 0x024 */ be<f32> mSpeedF;
    /* 0x028 */ be<f32> mBodyLiftY;  /* was m028: view of enemyice::mBodyLiftY (c/c_damagereaction.h) */
    /* 0x02C */ be<f32> mIceOffsetY;  /* was m02C: view of enemyice::mIceOffsetY (c/c_damagereaction.h) */
    /* 0x030 */ dCcD_Stts mStts;
    /* 0x06C */ dCcD_Cyl mCyl;
    /* 0x19C */ be<f32> mCylHeight;
    /* 0x1A0 */ be<f32> mWallRadius;
    /* 0x1A4 */ be<f32> mScaleXZ;
    /* 0x1A8 */ be<f32> mScaleY;
    /* 0x1AC */ be<f32> mParticleScale;
    /* 0x1B0 */ be<u8> m1B0;
    /* 0x1B1 */ be<u8> mDeathSwitch;
    /* 0x1B2 */ u8 _1B2[2];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_SIZE(enemyice_l, 0x3B8);

struct enemyfire_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFireDuration;
    /* 0x006 */ be<s8> mMode;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<s16> mFireTimer;
    /* 0x00A */ u8 _00A[2];
    /* 0x00C */ gptr<mDoExt_McaMorf> mpMcaMorf;
    /* 0x010 */ be<s8> mFlameJntIdxs[10];
    /* 0x01A */ u8 _01A[2];
    /* 0x01C */ be<f32> mParticleScale[10];
    /* 0x044 */ be<s16> mFlameTimers[10];
    /* 0x058 */ be<u32> mpFlameEmitters[10];
    /* 0x080 */ cXyz mPrevPos;
    /* 0x08C */ cXyz mDirection;
    /* 0x098 */ be<f32> mFlameScaleY;
    /* 0x09C */ u8 _09C;
    /* 0x09D */ be<u8> mHitboxFlameIdx;
    /* 0x09E */ u8 _09E[2];
    /* 0x0A0 */ dCcD_Stts mStts;
    /* 0x0DC */ dCcD_Sph mSph;
    /* 0x208 */ u8 mLight[0x20];  /* LIGHT_INFLUENCE */
    /* 0x228 */ be<f32> mHD20;  /* HD: 1.0f in the constructor; was m228: mLight (0x208, a LIGHT_INFLUENCE of HD size 0x24, declared here as its first 0x20 bytes) + 0x20 = LIGHT_INFLUENCE::mHD20 (bindings.h / d/d_kankyo.h), the light's HD tail; the enemyfire constructors (pt 02446C68, ki 0219D368, ks 021AA2D8, cc 02110B28, oq 023C3C80, am 0204B6A0, rd 0245A984, nz 0230F104) set it to 1.0 as the LIGHT_INFLUENCE constructor does; no reader in WWHD, so it keeps the class name */
};
WWHD_SIZE(enemyfire_l, 0x22C);
#endif

struct ks_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpEyeMorf;
    /* 0x3D8 */ gptr<mDoExt_btkAnm> mpEyeBtkAnm;
    /* 0x3DC */ gptr<mDoExt_brkAnm> mpBodyBrkAnm;
    /* 0x3E0 */ gptr<mDoExt_brkAnm> mpEyeBrkAnm;
    /* 0x3E4 */ u8 mPacket[2][0xB0];   /* HD: mDoExt_J3DModelPacketS x2 (set up in GanonK) */
    /* 0x544 */ be<u8> mType;  /* was m2C8: daKS_Create 021AA4C8: param bits 0-7 (0xFF -> 0); types 0 / 1 create mBirthNum Morths (param 3 / 2), 4 / 5 are linked to Mothula (fpcNm_GM_e; 5 is thrown out), 2 starts in KS_MOVE, 6 in KS_POT_SEARCH, 7 in KS_BIRTH; daKS_Draw 021A71EC draws nothing for type 6 and daKS_Execute 021A7F94 returns after its action */
    /* 0x545 */ be<u8> mBirthNum;  /* was m2C9: daKS_Create 021AA4C8: param bits 8-15 (1 when 0 or > 0x15); types 0 / 1 create that many Morths; action_tubo_search (in daKS_Execute 021A7F94) creates that many from the pot */
    /* 0x546 */ be<u8> mSearchRangePrm;  /* was m2CA: daKS_Create 021AA4C8: param bits 16-23 (0xFF -> 0); mSearchRange = it * 10 */
    /* 0x547 */ be<u8> mAction;
    /* 0x548 */ be<u8> mMode;
    /* 0x549 */ be<u8> mDrawSubDone;  /* was m2CD: daKS_Execute 021A7F94 sets 1 after it calls draw_SUB 021A6F64 itself (deleted after falling out of the map); daKS_Draw 021A71EC skips draw_SUB while it is set and clears it */
    /* 0x54A */ be<u8> mFirstStuck;  /* was m2CE: action_omoi (in daKS_Execute 021A7F94) sets 1 for the Morth that sticks to Link while KUTTUKU_ALL_COUNT is 0; only that one checks Link's front roll and resets HEAVY_IN / KUTTUKU_ALL_COUNT when they come off; body_atari_check 021A74E4 tests it in mode 43 */
    /* 0x54B */ be<u8> mRippleOn;  /* was m2CF: daKS_Execute 021A7F94 sets 1 and starts the water ripple (mRipple) when the Morth is in water, clears it and ends the ripple on ground / 100 above the water; ks_kuttuki_check 021A73E4 ends it when the Morth sticks to Link */
    /* 0x54C */ be<u8> mFrozen;  /* was m2D0: body_atari_check 021A74E4 sets 1 at an ice arrow / light arrow hit; daKS_Execute 021A7F94 steps mBrkFrame to 7 while it is set; daKS_Draw 021A71EC skips draw_SUB and draw_SUB 021A6F64 lifts the model by mBgOffsetY */
    /* 0x54D */ be<u8> m2D1;
    /* 0x54E */ be<u8> mStickJointDir;  /* was m2D2: action_omoi (in daKS_Execute 021A7F94): 0 = mStickJointNo counts up to 0x13, 1 = counts down to 0 (the Morth crawls over Link's joints) */
    /* 0x54F */ be<u8> mNoGroundCount;  /* was m2D3: daKS_Execute 021A7F94 (naraku_check): +1 per frame without ground, deletes the actor after 50 frames; cleared when there is ground */
    /* 0x550 */ be<u32> mGmID;
    /* 0x554 */ be<u32> mKsID;
    /* 0x558 */ cXyz m2DC;
    /* 0x564 */ be<s16> mTimers[4];  /* was m2E8: daKS_Execute 021A7F94 counts all 4 down each frame; [0] blink wait (reloads rnd 50 +-25 and starts mEyeBtkFrame), [1] / [2] action timers (action_kaze_move, action_kougeki_move, action_omoi) */
    /* 0x56C */ be<s16> m2F0[5];
    /* 0x576 */ be<s16> mSpinAngle;  /* was m2FA: draw_SUB 021A6F64: z rotation of the body model (+ before, - after the scale) and adds mSpinSpeed per draw */
    /* 0x578 */ be<s16> mMoveAngleOffset;  /* was m2FC: action_dousa_move (in daKS_Execute 021A7F94): random offset ((process id & 0xF) * cM_rndFX(512)) added to the player angle as the move angle; cleared while Link is slow or swimming */
    /* 0x57A */ be<s16> mSpinSpeed;  /* was m2FE: daKS_Create 021AA4C8: (process id & 7) * 50 + 1000; draw_SUB 021A6F64 adds it to mSpinAngle; action_omoi (in daKS_Execute 021A7F94) turns shape_angle.y by it / 4 */
    /* 0x57C */ be<s16> mStickJointNo;  /* was m300: action_omoi (in daKS_Execute 021A7F94): index 0..0x13 into pl_harituki_joint_dt (Link joint the Morth sticks to), stepped by mStickJointDir */
    /* 0x57E */ be<s16> mEyeBtkFrame;  /* was m302: daKS_Execute 021A7F94 sets 1 when mTimers[0] runs out and steps it to 7, then 0 (blink); daKS_Draw 021A71EC sets the eye BTK frame to it */
    /* 0x580 */ be<f32> mBgOffsetY;  /* was m304: daKS_Create 021AA4C8 sets 25; BG_check 021A7F24 lowers the position by it for the ground check and raises it back; draw_SUB 021A6F64 lifts the models by it while mFrozen */
    /* 0x584 */ be<f32> mWindRiseStep;  /* was m308: action_kaze_move (in daKS_Execute 021A7F94): cleared at the start, eased to 5; step of speed.y toward mWindRiseSpeed */
    /* 0x588 */ be<f32> mSwayAmp;  /* was m30C: action_dousa_move (in daKS_Execute 021A7F94) sets 0 / 10, action_kaze_move 2; speed_keisan 021A7A88 adds sin(m2F0[0]) * it to the move angle; action_kougeki_move / action_kaze_move clear it */
    /* 0x58C */ be<f32> mWindStartY;  /* was m310: action_kaze_move (in daKS_Execute 021A7F94) copies pos.y at the start; the wind lift ends 200 above it (or when mTimers[1] runs out) */
    /* 0x590 */ be<f32> mWindRiseSpeed;  /* was m314: action_kaze_move (in daKS_Execute 021A7F94): rnd(15) + 10; speed.y is eased to it */
    /* 0x594 */ be<f32> mSearchRange;  /* was m318: daKS_Create 021AA4C8: mSearchRangePrm * 10; action_tubo_search (in daKS_Execute 021A7F94) waits in the pot until the player is within it (0 = no wait) */
    /* 0x598 */ be<f32> mSphRadius;  /* was m31C: daKS_Create 021AA4C8 sets 20; action_kougeki_move 20 / 60, action_tubo_search 100; daKS_Execute 021A7F94 sets it as the mSph radius each frame */
    /* 0x59C */ be<f32> mBrkFrame;  /* was m320: daKS_Execute 021A7F94 adds 1 up to 7 while mFrozen; daKS_Draw 021A71EC sets the body and eye BRK frames to it */
    /* 0x5A0 */ dBgS_AcchCir mAcchCir;
    /* 0x5E0 */ dBgS_ObjAcch mAcch;
    /* 0x7A4 */ be<u32> mpStickJointMtx;  /* was m528: action_omoi (in daKS_Execute 021A7F94): daPy_getModelJointMtx(Link, pl_harituki_joint_dt[mStickJointNo]) (tww d217e2b type MtxP) of the Link joint the Morth sticks to; when the Morth comes off (mode 42 / later) it is cleared together with status 0x4000; MtxP */
    /* 0x7A8 */ u8 mRipple[0x14];  /* was m52C: ks_class_ct 021AA364: dPa_rippleEcallBack ctor (tww d217e2b type dPa_rippleEcallBack); daKS_Execute 021A7F94 starts particle ID_AK_JN_HAMON00 into it in water; ks_kuttuki_check 021A73E4 / daKS_Execute end it; dPa_rippleEcallBack */
    /* 0x7BC */ dCcD_Stts mStts;
    /* 0x7F8 */ dCcD_Sph mSph;
    /* 0x924 */ enemyice_l mEnemyIce;
    /* 0xCDC */ enemyfire_l mEnemyFire;
};
WWHD_OFFSET(ks_class, mType, 0x544);
WWHD_OFFSET(ks_class, mBrkFrame, 0x59C);
WWHD_OFFSET(ks_class, mAcch, 0x5E0);
WWHD_OFFSET(ks_class, mSph, 0x7F8);
WWHD_OFFSET(ks_class, mEnemyIce, 0x924);
WWHD_OFFSET(ks_class, mPhs, 0x3C8);
WWHD_OFFSET(ks_class, mpBodyMorf, 0x3D0);
WWHD_OFFSET(ks_class, mpEyeMorf, 0x3D4);
WWHD_OFFSET(ks_class, mpEyeBtkAnm, 0x3D8);
WWHD_OFFSET(ks_class, mpBodyBrkAnm, 0x3DC);
WWHD_OFFSET(ks_class, mpEyeBrkAnm, 0x3E0);
WWHD_OFFSET(ks_class, mPacket, 0x3E4);
WWHD_OFFSET(ks_class, mBirthNum, 0x545);
WWHD_OFFSET(ks_class, mSearchRangePrm, 0x546);
WWHD_OFFSET(ks_class, mAction, 0x547);
WWHD_OFFSET(ks_class, mMode, 0x548);
WWHD_OFFSET(ks_class, mDrawSubDone, 0x549);
WWHD_OFFSET(ks_class, mFirstStuck, 0x54A);
WWHD_OFFSET(ks_class, mRippleOn, 0x54B);
WWHD_OFFSET(ks_class, mFrozen, 0x54C);
WWHD_OFFSET(ks_class, m2D1, 0x54D);
WWHD_OFFSET(ks_class, mStickJointDir, 0x54E);
WWHD_OFFSET(ks_class, mNoGroundCount, 0x54F);
WWHD_OFFSET(ks_class, mGmID, 0x550);
WWHD_OFFSET(ks_class, mKsID, 0x554);
WWHD_OFFSET(ks_class, m2DC, 0x558);
WWHD_OFFSET(ks_class, mTimers, 0x564);
WWHD_OFFSET(ks_class, m2F0, 0x56C);
WWHD_OFFSET(ks_class, mSpinAngle, 0x576);
WWHD_OFFSET(ks_class, mMoveAngleOffset, 0x578);
WWHD_OFFSET(ks_class, mSpinSpeed, 0x57A);
WWHD_OFFSET(ks_class, mStickJointNo, 0x57C);
WWHD_OFFSET(ks_class, mEyeBtkFrame, 0x57E);
WWHD_OFFSET(ks_class, mBgOffsetY, 0x580);
WWHD_OFFSET(ks_class, mWindRiseStep, 0x584);
WWHD_OFFSET(ks_class, mSwayAmp, 0x588);
WWHD_OFFSET(ks_class, mWindStartY, 0x58C);
WWHD_OFFSET(ks_class, mWindRiseSpeed, 0x590);
WWHD_OFFSET(ks_class, mSearchRange, 0x594);
WWHD_OFFSET(ks_class, mSphRadius, 0x598);
WWHD_OFFSET(ks_class, mAcchCir, 0x5A0);
WWHD_OFFSET(ks_class, mpStickJointMtx, 0x7A4);
WWHD_OFFSET(ks_class, mRipple, 0x7A8);
WWHD_OFFSET(ks_class, mStts, 0x7BC);
WWHD_OFFSET(ks_class, mEnemyFire, 0xCDC);
WWHD_SIZE(ks_class, 0xF08);

/* ---- file statics (.data, d_a_ks.cpp) ---- */
extern be<s32>& KS_ALL_COUNT;
extern be<s32>& KUTTUKU_ALL_COUNT;
extern be<s32>& HEAVY_IN;
extern be<s32>& GORON_COUNT;

#define KS_SAFESTRING_VTBL 0x10013668 /* this TU's sead::SafeString vtable */
#define KS_VTBL 0x100136C0            /* ks_class vtable (HD virtual destructor) */

/* functions called across the source files of this unit */
void draw_SUB(ks_class* i_this);
BOOL tyaku_check(ks_class* i_this);
BOOL ks_kuttuki_check(ks_class* i_this);
BOOL body_atari_check(ks_class* i_this);
void speed_keisan(ks_class* i_this, s16 i_speed);
void dead_eff_set(ks_class* i_this, cXyz* i_pos);
void BG_check(ks_class* i_this);

void ksMain_fire(enemyfire_l* fire);
void ksMain_removeFire(enemyfire_l* fire);
void ksMain_rippleEnd(void* ripple);
s32 ksMain_characterIndex(u32 table, const char* name, s32 mode);
void ksMain_packetInit(void* packet, s32 mode);
void ksMain_sphereCtor(dCcD_Sph* sphere);
void ksMain_packetCtor(void* packet);
void ksMain_rippleCtor(void* ripple);
void ksMain_sphereDtor(dCcD_Sph* sphere, s32 flags);
void ksMain_statusDtor(dCcD_Stts* status, s32 flags);
void ksMain_acchDtor(dBgS_ObjAcch* acch, s32 flags);
void ksMain_circleDtor(void* circle, s32 flags);
void ksMain_packetDtor(void* packet, s32 flags);
void ksMain_actorDtor(ks_class* actor, s32 flags);
BOOL ksExecute_ice(enemyice_l* ice);
void ksExecute_rippleEnd(void* ripple);
void ksExecute_roomLayer(fopAc_ac_c* actor, s32 room);
void ksExecute_monsterSound(u32 sound, cXyz* position, u32 id, u32 param, s32 reverb);
// HD: nullable SafeString data retains 32-bit guest address wrap.
struct KsStringVtable { u8 mUnaccessed[0x14]; be<u32> assureTermination; };
WWHD_OFFSET(KsStringVtable, assureTermination, 0x14); WWHD_SIZE(KsStringVtable, 0x18);
inline void ksAssureString(SafeString* string) { gabi::call_ptr(gabi::load<u32>(string->__vtbl + offsetof(KsStringVtable, assureTermination)), string); }
inline u8 ksStringByte(u32 address) { return gabi::load<u8>(address); }
struct KsPlayerView {
    u8 mUnaccessed[0x3AC]; be<u8> mCutType;
    u8 mPad3AD[0x3C0 - 0x3AD]; be<u32> mStateFlags;
    u8 mPad3C4[0x3D8 - 0x3C4]; cXyz mHeadTopPos; cXyz mSwordTopPos;
    u8 mPad3F0[0x69EE - 0x3F0]; be<s16> mHeavyStateCounter;  /* was mHeavyCount (misnamed): view of daPy_lk_c::mHeavyStateCounter (d_a_player_main.h), the per-frame count execute turns into the heavy state */
};
WWHD_OFFSET(KsPlayerView, mCutType, 0x3AC);
WWHD_OFFSET(KsPlayerView, mStateFlags, 0x3C0);
WWHD_OFFSET(KsPlayerView, mHeadTopPos, 0x3D8);
WWHD_OFFSET(KsPlayerView, mSwordTopPos, 0x3E4);
WWHD_OFFSET(KsPlayerView, mHeavyStateCounter, 0x69EE); WWHD_SIZE(KsPlayerView, 0x69F0);
inline KsPlayerView* ksPlayer(fopAc_ac_c* player) { return reinterpret_cast<KsPlayerView*>(player); }
struct KsGmView { u8 mUnaccessed[0x43A]; be<s16> m31E; };
WWHD_OFFSET(KsGmView, m31E, 0x43A); WWHD_SIZE(KsGmView, 0x43C);
inline KsGmView* ksGm(fopAc_ac_c* gm) { return reinterpret_cast<KsGmView*>(gm); }
struct KsAnimationView { u8 mUnaccessed[4]; be<f32> mFrame; };
WWHD_OFFSET(KsAnimationView, mFrame, 4); WWHD_SIZE(KsAnimationView, 8);
inline KsAnimationView* ksAnimation(void* animation) { return reinterpret_cast<KsAnimationView*>(animation); }
struct KsModelDataView { u8 mUnaccessed[0x44]; be<u32> mTextureSRT; be<u32> mTevReg; };
WWHD_OFFSET(KsModelDataView,mTextureSRT,0x44); WWHD_OFFSET(KsModelDataView,mTevReg,0x48); WWHD_SIZE(KsModelDataView,0x4C);
inline KsModelDataView* ksModelData(J3DModel* model) { return reinterpret_cast<KsModelDataView*>(model->getModelData()); }
struct KsRippleView { u8 mUnaccessed[0x10]; be<f32> mRate; };
WWHD_OFFSET(KsRippleView,mRate,0x10); WWHD_SIZE(KsRippleView,0x14);
inline KsRippleView* ksRipple(void* ripple) { return reinterpret_cast<KsRippleView*>(ripple); }


struct KsCameraView { u8 mUnaccessed[0xDC]; cXyz mEye; };
WWHD_OFFSET(KsCameraView,mEye,0xDC); WWHD_SIZE(KsCameraView,0xE8);
inline cXyz* ksCameraEye(camera_class* camera) { return &reinterpret_cast<KsCameraView*>(camera)->mEye; }
inline u32 ksStartStageAddress() { return dComIfGp_ea() + PLAY_OFF(mCurStage.mName); }
inline fopAc_ac_c* ksPlayerAt(u32 play) { return gabi::at<fopAc_ac_c>(gabi::load<u32>(play + PLAY_PLAYER)); }
struct KsHitObjectView { u8 mUnaccessed[0x10]; be<u32> mAtType; };
WWHD_OFFSET(KsHitObjectView,mAtType,0x10); WWHD_SIZE(KsHitObjectView,0x14);
inline KsHitObjectView* ksHitObject(void* object) { return reinterpret_cast<KsHitObjectView*>(object); }
struct KsSaveView { u8 mUnaccessed[0x33]; be<u8> mMaxMagic,mMagic; u8 mPad35[0x89 - 0x35]; be<u8> mArrows; };
WWHD_OFFSET(KsSaveView,mMaxMagic,0x33); WWHD_OFFSET(KsSaveView,mMagic,0x34); WWHD_OFFSET(KsSaveView,mArrows,0x89); WWHD_SIZE(KsSaveView,0x8A);
inline KsSaveView* ksSave() { return gabi::at<KsSaveView>(dComIfGs_ea()); }
enum : u32 { KS_ACCH_VTABLE=0x100136A0, KS_GROUND_VTABLE=0x100136B0 };
enemyfire_l* ksMain_fireCtor(enemyfire_l* fire);
ks_class* ksMain_actorCtor(fopAc_ac_c* actor);

// HD: Link's virtual slots retain nullable vtable-address behavior.
struct KsPlayerVtable { u8 mUnaccessed[0x5C]; be<u32> checkFrontRoll; u8 mPad60[0xFC - 0x60]; be<u32> getModelJointMtx; };
WWHD_OFFSET(KsPlayerVtable,checkFrontRoll,0x5C); WWHD_OFFSET(KsPlayerVtable,getModelJointMtx,0xFC); WWHD_SIZE(KsPlayerVtable,0x100);
inline u32 ksPlayerJoint(fopAc_ac_c* player, u16 joint) { return gabi::call_ptr<u32>(gabi::load<u32>(player->__vtbl + offsetof(KsPlayerVtable,getModelJointMtx)),player,(u32)joint); }
inline BOOL ksPlayerRolling(fopAc_ac_c* player) { return gabi::call_ptr<BOOL>(gabi::load<u32>(player->__vtbl + offsetof(KsPlayerVtable,checkFrontRoll)),player); }
extern be<u32> (&pl_harituki_joint_dt)[20];
enum : u32 { KS_POT_SEARCH_CALLBACK = 0x021A7E68 };
void* ksMain_brkCtor(void* storage);
void* ksMain_btkCtor(void* storage);
BOOL ksMain_brkInit(void* animation,J3DModelData* model,void* key,s32 play,s32 mode,s16 start,s16 end,bool modify,s32 entry,f32 rate);
BOOL ksMain_btkInit(void* animation,J3DModelData* model,void* key,s32 play,s32 mode,s16 start,s16 end,bool modify,s32 entry,f32 rate);
struct KsAnimationCallFrame { be<u32> mWords[4]; };
WWHD_SIZE(KsAnimationCallFrame,16);

inline void ksAcchCtor(dBgS_ObjAcch* acch) { const dBgS_ObjAcch_vt table = {0x10013690,0x100136B0,0x100136A0}; dBgS_ObjAcch_ct(acch,table); }

inline Mtx34* ksAttachedMatrix(ks_class* actor) { return gabi::at<Mtx34>(actor->mpStickJointMtx); }
enum : u32 { KS_ICE_CYLINDER_VTABLE = 0x10013680, KS_HEADER_STATIC_OBJECT = 0x10464C74, KS_HEADER_STATIC_FLAG = 0x101B8934 };
enum KsAction { KS_MOVE=0, KS_ATTACK=1, KS_WIND=2, KS_DEAD=3, KS_ATTACHED=4, KS_POT_SEARCH=10, KS_BIRTH=20 };
