/* ks_class (Morth), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c +0x11C; HD inserts two mDoExt_J3DModelPacketS (0xB0 each,
 * constructor 02080404) after the anm pointers, so the members from m2C8 (GameCube 0x2C8) on are
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
    /* 0x00C */ be<s8> m00C;
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 _016[2];
    /* 0x018 */ cXyz mSpeed;
    /* 0x024 */ be<f32> mSpeedF;
    /* 0x028 */ be<f32> m028;
    /* 0x02C */ be<f32> m02C;
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
    /* 0x228 */ be<f32> m228;     /* HD: 1.0f in the constructor */
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
    /* 0x544 */ be<u8> m2C8;
    /* 0x545 */ be<u8> m2C9;
    /* 0x546 */ be<u8> m2CA;
    /* 0x547 */ be<u8> mAction;
    /* 0x548 */ be<u8> mMode;
    /* 0x549 */ be<u8> m2CD;
    /* 0x54A */ be<u8> m2CE;
    /* 0x54B */ be<u8> m2CF;
    /* 0x54C */ be<u8> m2D0;
    /* 0x54D */ be<u8> m2D1;
    /* 0x54E */ be<u8> m2D2;
    /* 0x54F */ be<u8> m2D3;
    /* 0x550 */ be<u32> mGmID;
    /* 0x554 */ be<u32> mKsID;
    /* 0x558 */ cXyz m2DC;
    /* 0x564 */ be<s16> m2E8[4];
    /* 0x56C */ be<s16> m2F0[5];
    /* 0x576 */ be<s16> m2FA;
    /* 0x578 */ be<s16> m2FC;
    /* 0x57A */ be<s16> m2FE;
    /* 0x57C */ be<s16> m300;
    /* 0x57E */ be<s16> m302;
    /* 0x580 */ be<f32> m304;
    /* 0x584 */ be<f32> m308;
    /* 0x588 */ be<f32> m30C;
    /* 0x58C */ be<f32> m310;
    /* 0x590 */ be<f32> m314;
    /* 0x594 */ be<f32> m318;
    /* 0x598 */ be<f32> m31C;
    /* 0x59C */ be<f32> m320;
    /* 0x5A0 */ dBgS_AcchCir mAcchCir;
    /* 0x5E0 */ dBgS_ObjAcch mAcch;
    /* 0x7A4 */ be<u32> m528;           /* MtxP */
    /* 0x7A8 */ u8 m52C[0x14];          /* dPa_rippleEcallBack */
    /* 0x7BC */ dCcD_Stts mStts;
    /* 0x7F8 */ dCcD_Sph mSph;
    /* 0x924 */ enemyice_l mEnemyIce;
    /* 0xCDC */ enemyfire_l mEnemyFire;
};
WWHD_OFFSET(ks_class, m2C8, 0x544);
WWHD_OFFSET(ks_class, m320, 0x59C);
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
WWHD_OFFSET(ks_class, m2C9, 0x545);
WWHD_OFFSET(ks_class, m2CA, 0x546);
WWHD_OFFSET(ks_class, mAction, 0x547);
WWHD_OFFSET(ks_class, mMode, 0x548);
WWHD_OFFSET(ks_class, m2CD, 0x549);
WWHD_OFFSET(ks_class, m2CE, 0x54A);
WWHD_OFFSET(ks_class, m2CF, 0x54B);
WWHD_OFFSET(ks_class, m2D0, 0x54C);
WWHD_OFFSET(ks_class, m2D1, 0x54D);
WWHD_OFFSET(ks_class, m2D2, 0x54E);
WWHD_OFFSET(ks_class, m2D3, 0x54F);
WWHD_OFFSET(ks_class, mGmID, 0x550);
WWHD_OFFSET(ks_class, mKsID, 0x554);
WWHD_OFFSET(ks_class, m2DC, 0x558);
WWHD_OFFSET(ks_class, m2E8, 0x564);
WWHD_OFFSET(ks_class, m2F0, 0x56C);
WWHD_OFFSET(ks_class, m2FA, 0x576);
WWHD_OFFSET(ks_class, m2FC, 0x578);
WWHD_OFFSET(ks_class, m2FE, 0x57A);
WWHD_OFFSET(ks_class, m300, 0x57C);
WWHD_OFFSET(ks_class, m302, 0x57E);
WWHD_OFFSET(ks_class, m304, 0x580);
WWHD_OFFSET(ks_class, m308, 0x584);
WWHD_OFFSET(ks_class, m30C, 0x588);
WWHD_OFFSET(ks_class, m310, 0x58C);
WWHD_OFFSET(ks_class, m314, 0x590);
WWHD_OFFSET(ks_class, m318, 0x594);
WWHD_OFFSET(ks_class, m31C, 0x598);
WWHD_OFFSET(ks_class, mAcchCir, 0x5A0);
WWHD_OFFSET(ks_class, m528, 0x7A4);
WWHD_OFFSET(ks_class, m52C, 0x7A8);
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
    u8 mPad3F0[0x69EE - 0x3F0]; be<s16> mHeavyCount;
};
WWHD_OFFSET(KsPlayerView, mCutType, 0x3AC);
WWHD_OFFSET(KsPlayerView, mStateFlags, 0x3C0);
WWHD_OFFSET(KsPlayerView, mHeadTopPos, 0x3D8);
WWHD_OFFSET(KsPlayerView, mSwordTopPos, 0x3E4);
WWHD_OFFSET(KsPlayerView, mHeavyCount, 0x69EE); WWHD_SIZE(KsPlayerView, 0x69F0);
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

inline Mtx34* ksAttachedMatrix(ks_class* actor) { return gabi::at<Mtx34>(actor->m528); }
enum : u32 { KS_ICE_CYLINDER_VTABLE = 0x10013680, KS_HEADER_STATIC_OBJECT = 0x10464C74, KS_HEADER_STATIC_FLAG = 0x101B8934 };
enum KsAction { KS_MOVE=0, KS_ATTACK=1, KS_WIND=2, KS_DEAD=3, KS_ATTACHED=4, KS_POT_SEARCH=10, KS_BIRTH=20 };
