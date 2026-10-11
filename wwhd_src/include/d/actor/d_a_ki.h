/* ki_class (Keese), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c +0x11C; mDoExt_J3DModelPacketS grew from 0x14 to 0xB0
 * (constructor 02080404 allocates 0xB0), so the members after it are +0x1B8; the shadow id m2E0
 * is gone (HD shadows), so from mMaxAttackMoveDist300 on everything is +0x1B4. enemyfire grew by
 * 4 bytes (0x22C, constructor 0219D368). Size 0x10BC (constructor 0219D3F4; GameCube 0xF04). */
#pragma once
#include "bindings.h"
#include "gc_api/JSystem/J3DModelData.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"

#ifndef WWHD_ENEMYICE_FIRE_L
#define WWHD_ENEMYICE_FIRE_L
/* enemyice / enemyfire (c_damagereaction), WWHD layouts (SHARED-CANDIDATE) */
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

/* dPa_followEcallBack (HD: vtable at +0, 0x14 bytes) */
struct dPa_followEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x10];
};

struct ki_class {
    enum Action_e {
        ACT_WAIT_MOVE_e = 0,
        ACT_FLY_MOVE_e = 1,
        ACT_FIRE_SET_MOVE_e = 2,
        ACT_ATTACK_MOVE_e = 3,
        ACT_ATTACK_MOVE_INDEX_e = 10,
        ACT_DAMAGE_MOVE_e = 20,
        ACT_FAIL_MOVE_e = 30,
    };

    /* 0x000 */ fopEn_enemy_c actor;
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D4 */ u8 m2B8[0xB0];               /* mDoExt_J3DModelPacketS (HD 0xB0; set up only in GanonK) */
    /* 0x484 */ be<u8> mParameters;
    /* 0x485 */ be<u8> m2CD;
    /* 0x486 */ be<u8> mKiPathIndex;
    /* 0x487 */ be<u8> m2CF;
    /* 0x488 */ u8 _488;
    /* 0x489 */ be<u8> mAction;
    /* 0x48A */ u8 _48A;
    /* 0x48B */ be<s8> mBehaviorType;
    /* 0x48C */ be<u8> m2D4;
    /* 0x48D */ be<s8> mCurrKiPathIndex;
    /* 0x48E */ be<s8> m2D6;
    /* 0x48F */ be<s8> m2D7;
    /* 0x490 */ gptr<dPath> ppd;
    /* 0x494 */ be<u8> m2DC;
    /* 0x495 */ u8 _495[3];                   /* HD: no shadow id (GameCube m2E0) */
    /* 0x498 */ be<f32> mMaxAttackMoveDist300;
    /* 0x49C */ cXyz mPosMove;
    /* 0x4A8 */ be<f32> mPosMoveTarget;
    /* 0x4AC */ be<f32> mPosMoveMaxSpeed;
    /* 0x4B0 */ be<f32> mPosMoveDist;
    /* 0x4B4 */ u8 _4B4[4];
    /* 0x4B8 */ be<f32> m304;
    /* 0x4BC */ u8 _4BC[4];
    /* 0x4C0 */ be<s16> mTimers[4];
    /* 0x4C8 */ be<s16> m314;
    /* 0x4CA */ be<s16> m316;
    /* 0x4CC */ be<s16> mRand2000;
    /* 0x4CE */ u8 _4CE[2];
    /* 0x4D0 */ be<f32> m31C;
    /* 0x4D4 */ be<s16> m320;
    /* 0x4D6 */ be<s16> m322;
    /* 0x4D8 */ be<s16> m324;
    /* 0x4DA */ be<s16> m326;
    /* 0x4DC */ be<s16> m328;
    /* 0x4DE */ u8 _4DE[2];
    /* 0x4E0 */ gptr<J3DAnmTexPattern> m32C;
    /* 0x4E4 */ gptr<mDoExt_btpAnm> m330;     /* HD: an mDoExt_btpAnm (GameCube J3DTexNoAnm[]) */
    /* 0x4E8 */ be<u8> m334;
    /* 0x4E9 */ be<u8> m335;
    /* 0x4EA */ be<u8> m336;
    /* 0x4EB */ be<u8> m337;
    /* 0x4EC */ be<s8> m338;
    /* 0x4ED */ be<s8> m339;
    /* 0x4EE */ u8 _4EE[2];
    /* 0x4F0 */ dBgS_AcchCir mAcchCir;
    /* 0x530 */ dBgS_ObjAcch mAcch;
    /* 0x6F4 */ be<u8> m540;
    /* 0x6F5 */ u8 _6F5[3];
    /* 0x6F8 */ dCcD_Stts mStts;
    /* 0x734 */ dCcD_Sph m580;
    /* 0x860 */ dCcD_Sph m6AC;
    /* 0x98C */ dCcD_Sph mDamageSphere;
    /* 0xAB8 */ be<u8> m904;
    /* 0xAB9 */ be<u8> mDamageType;
    /* 0xABA */ u8 _ABA[2];
    /* 0xABC */ dPa_followEcallBack_l m908;
    /* 0xAD0 */ gptr<JPABaseEmitter> m91C;
    /* 0xAD4 */ gptr<mDoExt_btkAnm> m920;
    /* 0xAD8 */ enemyice_l mEnemyIce;
    /* 0xE90 */ enemyfire_l mEnemyFire;
};
WWHD_OFFSET(ki_class, mParameters, 0x484);
WWHD_OFFSET(ki_class, mMaxAttackMoveDist300, 0x498);
WWHD_OFFSET(ki_class, m32C, 0x4E0);
WWHD_OFFSET(ki_class, mAcchCir, 0x4F0);
WWHD_OFFSET(ki_class, mAcch, 0x530);
WWHD_OFFSET(ki_class, mStts, 0x6F8);
WWHD_OFFSET(ki_class, mDamageSphere, 0x98C);
WWHD_OFFSET(ki_class, m908, 0xABC);
WWHD_OFFSET(ki_class, mEnemyIce, 0xAD8);
WWHD_OFFSET(ki_class, mEnemyFire, 0xE90);
WWHD_SIZE(ki_class, 0x10BC);

/* kiHIO_c, HD: vtable pointer after the members (GameCube members -4) */
struct kiHIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ be<u8> m06;
    /* 0x03 */ be<u8> m07;
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<f32> m14;
    /* 0x14 */ be<f32> m18;
    /* 0x18 */ be<f32> m1C;
    /* 0x1C */ be<f32> m20;
    /* 0x20 */ be<f32> m24;
    /* 0x24 */ be<f32> m28;
    /* 0x28 */ be<f32> m2C;
    /* 0x2C */ be<f32> m30;
    /* 0x30 */ be<f32> m34;
    /* 0x34 */ be<f32> m38;
    /* 0x38 */ be<f32> m3C;
    /* 0x3C */ be<f32> m40;
    /* 0x40 */ be<f32> m44;
    /* 0x44 */ be<f32> m48;
    /* 0x48 */ be<s16> m4C;
    /* 0x4A */ be<s16> m4E;
    /* 0x4C */ be<s16> m50;
    /* 0x4E */ be<s16> m52;
    /* 0x50 */ be<s16> m54;
    /* 0x52 */ u8 _52[2];
    /* 0x54 */ be<f32> m58;
    /* 0x58 */ be<u32> __vtbl;
};
WWHD_SIZE(kiHIO_c, 0x5C);

enum {
    dRes_INDEX_KI_BAS_ATTACK1_e = 0x7,
    dRes_INDEX_KI_BAS_BITA1_e = 0x8,
    dRes_INDEX_KI_BAS_DAMAGE1_e = 0x9,
    dRes_INDEX_KI_BAS_FLY1_e = 0xA,
    dRes_INDEX_KI_BAS_FLY2_e = 0xB,
    dRes_INDEX_KI_BAS_WAIT1_e = 0xC,
    dRes_INDEX_KI_BCK_ATTACK1_e = 0xF,
    dRes_INDEX_KI_BCK_BITA1_e = 0x10,
    dRes_INDEX_KI_BCK_DAMAGE1_e = 0x11,
    dRes_INDEX_KI_BCK_FLY1_e = 0x12,
    dRes_INDEX_KI_BCK_FLY2_e = 0x13,
    dRes_INDEX_KI_BCK_WAIT1_e = 0x14,
    dRes_INDEX_KI_BDL_FK_e = 0x17,
    dRes_INDEX_KI_BDL_KI_e = 0x18,
    dRes_INDEX_KI_BTK_FK_e = 0x1B,
};
enum { fpcNm_KI_e = 0xD7 };
enum { DSNAP_TYPE_KI = 0xAE };
enum { JA_SE_CV_KI_ATTACK = 0x481D };

extern kiHIO_c& l_kiHIO;
extern be<u8>& hio_set;
extern be<s32>& ki_all_count;
extern be<s32>& ki_fight_count;
extern be<u16> (&ki_tex_anm_idx)[4];
extern be<u16> (&fk_tex_anm_idx)[4];
extern be<u16> (&ki_tex_max_frame)[4];
extern be<u8> (&ki_tex_loop)[4];
extern be<u8> (&ki_item_tbl)[4];
extern be<u32>& ki_emitterManager;
void kiMonsSe(u32 sound, cXyz* position, u32 actorID, u32 param, s32 reverb);
/* HD: fopAcM_monsSeStart inline (the process id is read before the reverb call) */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 se, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 id = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        kiMonsSe(se, &a->eyePos, id, param, reverb); /* mDoAud_monsSeStart */
    }
}

/* dBgS_LinChk (stack object), HD layout as in d_a_kamome (this TU's vtables) */
struct KiLineCheck {
    /* 0x00 */ be<u32> mpPolyPassChk;
    /* 0x04 */ be<u32> mpGrpPassChk;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x58 - 0x24];
    /* 0x58 */ be<u32> __vtbl_58;
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(KiLineCheck, 0x6C);
static inline void ki_LinChk_ct(KiLineCheck* c) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = 0x10012BD4;
    c->__vtbl_64 = 0x10012BF4;
    c->__vtbl_58 = 0x10012C04;
    c->__vtbl_20 = 0x10012BE4;
}
static inline void ki_LinChk_dt(KiLineCheck* c) {
    c->__vtbl_58 = 0x10012C04;
    c->__vtbl_64 = 0x10012B94;
    c->__vtbl_20 = 0x10012B84;
    cBgS_LinChk_dt(c, 0);
}


// HD: these private views preserve the signed low-byte path-count test and pattern frame order.
struct KiPathPoint { be<u8> mArg0, mArg1, mArg2, mArg3; cXyz m_position; };
WWHD_OFFSET(KiPathPoint,mArg3,3); WWHD_OFFSET(KiPathPoint,m_position,4); WWHD_SIZE(KiPathPoint,0x10);
struct KiPath { union { be<u16> m_num; be<s8> mCountBytes[2]; }; be<u16> m_nextID; be<u8> mArg0,m_closed,mArg2,mArg3; gptr<KiPathPoint> m_points; };
WWHD_OFFSET(KiPath,m_nextID,2); WWHD_OFFSET(KiPath,m_closed,5); WWHD_OFFSET(KiPath,m_points,8); WWHD_SIZE(KiPath,12);
inline KiPath* kiPath(dPath* path) { return reinterpret_cast<KiPath*>(path); }
inline cXyz* kiLineCross(KiLineCheck* check) { return &reinterpret_cast<dBgS_LinChk*>(check)->mLin.mEnd; }
struct KiTexturePattern { be<f32> mFrame; be<u32> mVtable; };
WWHD_OFFSET(KiTexturePattern,mFrame,0); WWHD_OFFSET(KiTexturePattern,mVtable,4); WWHD_SIZE(KiTexturePattern,8);
inline KiTexturePattern* kiPattern(J3DAnmTexPattern* pattern) { return reinterpret_cast<KiTexturePattern*>(pattern); }
struct KiPlayer { u8 mUnaccessed[0x3CC]; be<f32> mGrabWear; };
WWHD_OFFSET(KiPlayer,mGrabWear,0x3CC); WWHD_SIZE(KiPlayer,0x3D0);
inline KiPlayer* kiPlayer(fopAc_ac_c* actor) { return reinterpret_cast<KiPlayer*>(actor); }

// HD: named bindings keep the verified GHS register and stack signatures.
void kiMain_removeFire(enemyfire_l* fire);
void kiMain_iceEntry(mDoExt_McaMorf* morph, s32 mode, void* data);
BOOL kiMain_initPattern(mDoExt_btpAnm* animation, J3DModelData* model, J3DAnmTexPattern* pattern, s32 play, s32 mode, s16 start, s16 end, bool modify, s32 entry, f32 speed);
mDoExt_btpAnm* kiMain_createPattern(u32 storage);
mDoExt_btkAnm* kiMain_createTexture(void* storage);
BOOL kiMain_initTexture(mDoExt_btkAnm* animation, J3DModelData* model, void* key, s32 play, s32 mode, s16 start, s16 end, bool modify, s32 entry, f32 speed);
s32 kiMain_nameIndex(u32 table, u32 name);
void kiMain_jointEntry(J3DModel* model, u32 joint);
s32 kiMain_characterIndex(u32 table, const char* name, s32 mode);
void kiMain_packetInit(void* packet, u32 mode);
void kiMain_assureString(SafeString* string);
void kiMain_sphereCtor(dCcD_Sph* sphere);
void kiMain_acchCtor(dBgS_ObjAcch* acch);
void kiMain_packetCtor(void* packet);
void kiMain_circleCtor(dBgS_AcchCir* circle);
void kiMain_followCtor(dPa_followEcallBack_l* callback, u32 pos, u32 angle);
BOOL kiMain_execute(ki_class* actor);
void kiMain_acchDtor(dBgS_ObjAcch* acch, s32 flags);
void kiMain_sphereDtor(dCcD_Sph* sphere, s32 flags);
void kiMain_circleDtor(void* circle, s32 flags);
void kiMain_packetDtor(void* packet, s32 flags);
void kiMain_actorDtor(ki_class* actor, s32 flags);
void kiAttack_check(ki_class* actor);
void kiAttack_animation(ki_class* actor, int resource, f32 blend, u8 loop, f32 speed, int sound);
void kiAttack_texture(ki_class* actor, u16 index);
BOOL kiAttack_playerBackground(ki_class* actor);
void kiAttack_position(ki_class* actor, s8 mode);
void kiExecute_check(ki_class* actor);
void kiExecute_animation(ki_class* actor, int resource, f32 blend, u8 loop, f32 speed, int sound);
void kiExecute_texture(ki_class* actor, u16 index);
BOOL kiExecute_playerBackground(ki_class* actor);
void kiExecute_position(ki_class* actor, s8 mode);
void kiExecute_attack(ki_class* actor);
void kiExecute_removeFire(enemyfire_l* fire);
BOOL kiExecute_ice(enemyice_l* ice);
void kiExecute_fire(enemyfire_l* fire);
void kiExecute_report(s32 x, s32 y, const char* text, s32 count);
fopAc_ac_c* kiExecute_checkAttack(fopAc_ac_c* actor, void* info);
void kiExecute_deleteEmitter(u32 manager, u32 emitter, s32 mode);

struct KiJointData { u8 mUnaccessed[4]; be<u16> mJntNo; };
WWHD_OFFSET(KiJointData,mJntNo,4); WWHD_SIZE(KiJointData,6);
inline KiJointData* kiJoint(J3DJoint* joint) { return reinterpret_cast<KiJointData*>(joint); }

struct KiStringVtable { u8 mUnaccessed[0x14]; be<u32> assureTermination; };
WWHD_OFFSET(KiStringVtable,assureTermination,0x14); WWHD_SIZE(KiStringVtable,0x18);
inline void kiAssureString(u32 table, SafeString* string) { gabi::call_ptr(gabi::load<u32>(table + offsetof(KiStringVtable,assureTermination)),string); }
// HD: integer addresses preserve zero-address reads and 32-bit wrap in the bounded string comparison.
inline u8 kiStringByte(u32 address) { return gabi::load<u8>(address); }
struct KiFollowVtable { u8 mUnaccessed[0x44]; be<u32> remove; };
WWHD_OFFSET(KiFollowVtable,remove,0x44); WWHD_SIZE(KiFollowVtable,0x48);
inline void kiRemoveFollow(dPa_followEcallBack_l* callback) { gabi::call_ptr(gabi::load<u32>(callback->__vtbl + offsetof(KiFollowVtable,remove)),callback); }
struct KiPatternVtable { u8 mUnaccessed[0x1C]; be<u32> searchUpdateMaterialID; };
WWHD_OFFSET(KiPatternVtable,searchUpdateMaterialID,0x1C); WWHD_SIZE(KiPatternVtable,0x20);
inline u32 kiSearchPattern(J3DAnmTexPattern* pattern) { return gabi::load<u32>(kiPattern(pattern)->mVtable + offsetof(KiPatternVtable,searchUpdateMaterialID)); }
struct KiAcchPrefix { u8 mUnaccessed[0x10]; be<u32> mCheckVtable; be<u32> mPassVtable; be<u8> mObjectThrough; u8 mUnaccessed2[7]; be<u32> mGroupVtable; };
WWHD_OFFSET(KiAcchPrefix,mCheckVtable,0x10); WWHD_OFFSET(KiAcchPrefix,mPassVtable,0x14); WWHD_OFFSET(KiAcchPrefix,mObjectThrough,0x18); WWHD_OFFSET(KiAcchPrefix,mGroupVtable,0x20); WWHD_SIZE(KiAcchPrefix,0x24);
inline KiAcchPrefix* kiAcch(dBgS_ObjAcch* acch) { return reinterpret_cast<KiAcchPrefix*>(acch); }
enum : u32 { KI_SAFESTRING_VTABLE=0x10012B5C, KI_ACTOR_VTABLE=0x10012C24, KI_HIO_VTABLE=0x10012C14, KI_ACCH_CHECK_VTABLE=0x10012BA4, KI_ACCH_PASS_VTABLE=0x10012BC4, KI_ACCH_GROUP_VTABLE=0x10012BB4, KI_CYLINDER_VTABLE=0x10012B74, KI_HEADER_STATICS=0x10464B30, KI_HEADER_RECORDS=0x101B8290 };

struct KiCtorFrame { be<u32> w[4]; };
WWHD_SIZE(KiCtorFrame,16);

/* CcAtInfo (stack object passed to cc_at_check) */
struct KiAttackInfo {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ gptr<fopAc_ac_c> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ gptr<cXyz> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(KiAttackInfo, 0x1C);


WWHD_OFFSET(enemyice_l, mpActor, 0x000);
WWHD_OFFSET(enemyice_l, mFreezeDuration, 0x004);
WWHD_OFFSET(enemyice_l, mLightShrinkTimer, 0x006);
WWHD_OFFSET(enemyice_l, _007, 0x007);
WWHD_OFFSET(enemyice_l, mYOffset, 0x008);
WWHD_OFFSET(enemyice_l, m00C, 0x00C);
WWHD_OFFSET(enemyice_l, mMode, 0x00D);
WWHD_OFFSET(enemyice_l, mFreezeTimer, 0x00E);
WWHD_OFFSET(enemyice_l, mMoveDelayTimer, 0x010);
WWHD_OFFSET(enemyice_l, mAngleY, 0x012);
WWHD_OFFSET(enemyice_l, mAngularVelY, 0x014);
WWHD_OFFSET(enemyice_l, _016, 0x016);
WWHD_OFFSET(enemyice_l, mSpeed, 0x018);
WWHD_OFFSET(enemyice_l, mSpeedF, 0x024);
WWHD_OFFSET(enemyice_l, m028, 0x028);
WWHD_OFFSET(enemyice_l, m02C, 0x02C);
WWHD_OFFSET(enemyice_l, mStts, 0x030);
WWHD_OFFSET(enemyice_l, mCyl, 0x06C);
WWHD_OFFSET(enemyice_l, mCylHeight, 0x19C);
WWHD_OFFSET(enemyice_l, mWallRadius, 0x1A0);
WWHD_OFFSET(enemyice_l, mScaleXZ, 0x1A4);
WWHD_OFFSET(enemyice_l, mScaleY, 0x1A8);
WWHD_OFFSET(enemyice_l, mParticleScale, 0x1AC);
WWHD_OFFSET(enemyice_l, m1B0, 0x1B0);
WWHD_OFFSET(enemyice_l, mDeathSwitch, 0x1B1);
WWHD_OFFSET(enemyice_l, _1B2, 0x1B2);
WWHD_OFFSET(enemyice_l, mBgAcchCir, 0x1B4);
WWHD_OFFSET(enemyice_l, mBgAcch, 0x1F4);

WWHD_OFFSET(enemyfire_l, mpActor, 0x000);
WWHD_OFFSET(enemyfire_l, mFireDuration, 0x004);
WWHD_OFFSET(enemyfire_l, mMode, 0x006);
WWHD_OFFSET(enemyfire_l, _007, 0x007);
WWHD_OFFSET(enemyfire_l, mFireTimer, 0x008);
WWHD_OFFSET(enemyfire_l, _00A, 0x00A);
WWHD_OFFSET(enemyfire_l, mpMcaMorf, 0x00C);
WWHD_OFFSET(enemyfire_l, mFlameJntIdxs, 0x010);
WWHD_OFFSET(enemyfire_l, _01A, 0x01A);
WWHD_OFFSET(enemyfire_l, mParticleScale, 0x01C);
WWHD_OFFSET(enemyfire_l, mFlameTimers, 0x044);
WWHD_OFFSET(enemyfire_l, mpFlameEmitters, 0x058);
WWHD_OFFSET(enemyfire_l, mPrevPos, 0x080);
WWHD_OFFSET(enemyfire_l, mDirection, 0x08C);
WWHD_OFFSET(enemyfire_l, mFlameScaleY, 0x098);
WWHD_OFFSET(enemyfire_l, _09C, 0x09C);
WWHD_OFFSET(enemyfire_l, mHitboxFlameIdx, 0x09D);
WWHD_OFFSET(enemyfire_l, _09E, 0x09E);
WWHD_OFFSET(enemyfire_l, mStts, 0x0A0);
WWHD_OFFSET(enemyfire_l, mSph, 0x0DC);
WWHD_OFFSET(enemyfire_l, mLight, 0x208);
WWHD_OFFSET(enemyfire_l, m228, 0x228);

WWHD_OFFSET(ki_class, actor, 0x000);
WWHD_OFFSET(ki_class, mPhase, 0x3C8);
WWHD_OFFSET(ki_class, mpMorf, 0x3D0);
WWHD_OFFSET(ki_class, m2B8, 0x3D4);
WWHD_OFFSET(ki_class, m2CD, 0x485);
WWHD_OFFSET(ki_class, mKiPathIndex, 0x486);
WWHD_OFFSET(ki_class, m2CF, 0x487);
WWHD_OFFSET(ki_class, _488, 0x488);
WWHD_OFFSET(ki_class, mAction, 0x489);
WWHD_OFFSET(ki_class, _48A, 0x48A);
WWHD_OFFSET(ki_class, mBehaviorType, 0x48B);
WWHD_OFFSET(ki_class, m2D4, 0x48C);
WWHD_OFFSET(ki_class, mCurrKiPathIndex, 0x48D);
WWHD_OFFSET(ki_class, m2D6, 0x48E);
WWHD_OFFSET(ki_class, m2D7, 0x48F);
WWHD_OFFSET(ki_class, ppd, 0x490);
WWHD_OFFSET(ki_class, m2DC, 0x494);
WWHD_OFFSET(ki_class, _495, 0x495);
WWHD_OFFSET(ki_class, mPosMove, 0x49C);
WWHD_OFFSET(ki_class, mPosMoveTarget, 0x4A8);
WWHD_OFFSET(ki_class, mPosMoveMaxSpeed, 0x4AC);
WWHD_OFFSET(ki_class, mPosMoveDist, 0x4B0);
WWHD_OFFSET(ki_class, _4B4, 0x4B4);
WWHD_OFFSET(ki_class, m304, 0x4B8);
WWHD_OFFSET(ki_class, _4BC, 0x4BC);
WWHD_OFFSET(ki_class, mTimers, 0x4C0);
WWHD_OFFSET(ki_class, m314, 0x4C8);
WWHD_OFFSET(ki_class, m316, 0x4CA);
WWHD_OFFSET(ki_class, mRand2000, 0x4CC);
WWHD_OFFSET(ki_class, _4CE, 0x4CE);
WWHD_OFFSET(ki_class, m31C, 0x4D0);
WWHD_OFFSET(ki_class, m320, 0x4D4);
WWHD_OFFSET(ki_class, m322, 0x4D6);
WWHD_OFFSET(ki_class, m324, 0x4D8);
WWHD_OFFSET(ki_class, m326, 0x4DA);
WWHD_OFFSET(ki_class, m328, 0x4DC);
WWHD_OFFSET(ki_class, _4DE, 0x4DE);
WWHD_OFFSET(ki_class, m330, 0x4E4);
WWHD_OFFSET(ki_class, m334, 0x4E8);
WWHD_OFFSET(ki_class, m335, 0x4E9);
WWHD_OFFSET(ki_class, m336, 0x4EA);
WWHD_OFFSET(ki_class, m337, 0x4EB);
WWHD_OFFSET(ki_class, m338, 0x4EC);
WWHD_OFFSET(ki_class, m339, 0x4ED);
WWHD_OFFSET(ki_class, _4EE, 0x4EE);
WWHD_OFFSET(ki_class, m540, 0x6F4);
WWHD_OFFSET(ki_class, _6F5, 0x6F5);
WWHD_OFFSET(ki_class, m580, 0x734);
WWHD_OFFSET(ki_class, m6AC, 0x860);
WWHD_OFFSET(ki_class, m904, 0xAB8);
WWHD_OFFSET(ki_class, mDamageType, 0xAB9);
WWHD_OFFSET(ki_class, _ABA, 0xABA);
WWHD_OFFSET(ki_class, m91C, 0xAD0);
WWHD_OFFSET(ki_class, m920, 0xAD4);

WWHD_OFFSET(kiHIO_c, mNo, 0x00);
WWHD_OFFSET(kiHIO_c, m05, 0x01);
WWHD_OFFSET(kiHIO_c, m06, 0x02);
WWHD_OFFSET(kiHIO_c, m07, 0x03);
WWHD_OFFSET(kiHIO_c, m08, 0x04);
WWHD_OFFSET(kiHIO_c, m0C, 0x08);
WWHD_OFFSET(kiHIO_c, m10, 0x0C);
WWHD_OFFSET(kiHIO_c, m14, 0x10);
WWHD_OFFSET(kiHIO_c, m18, 0x14);
WWHD_OFFSET(kiHIO_c, m1C, 0x18);
WWHD_OFFSET(kiHIO_c, m20, 0x1C);
WWHD_OFFSET(kiHIO_c, m24, 0x20);
WWHD_OFFSET(kiHIO_c, m28, 0x24);
WWHD_OFFSET(kiHIO_c, m2C, 0x28);
WWHD_OFFSET(kiHIO_c, m30, 0x2C);
WWHD_OFFSET(kiHIO_c, m34, 0x30);
WWHD_OFFSET(kiHIO_c, m38, 0x34);
WWHD_OFFSET(kiHIO_c, m3C, 0x38);
WWHD_OFFSET(kiHIO_c, m40, 0x3C);
WWHD_OFFSET(kiHIO_c, m44, 0x40);
WWHD_OFFSET(kiHIO_c, m48, 0x44);
WWHD_OFFSET(kiHIO_c, m4C, 0x48);
WWHD_OFFSET(kiHIO_c, m4E, 0x4A);
WWHD_OFFSET(kiHIO_c, m50, 0x4C);
WWHD_OFFSET(kiHIO_c, m52, 0x4E);
WWHD_OFFSET(kiHIO_c, m54, 0x50);
WWHD_OFFSET(kiHIO_c, _52, 0x52);
WWHD_OFFSET(kiHIO_c, m58, 0x54);
WWHD_OFFSET(kiHIO_c, __vtbl, 0x58);

WWHD_OFFSET(dPa_followEcallBack_l, __vtbl, 0x00);
WWHD_OFFSET(dPa_followEcallBack_l, _04, 0x04);

WWHD_OFFSET(KiLineCheck, mpPolyPassChk, 0x00);
WWHD_OFFSET(KiLineCheck, mpGrpPassChk, 0x04);
WWHD_OFFSET(KiLineCheck, _08, 0x08);
WWHD_OFFSET(KiLineCheck, __vtbl_10, 0x10);
WWHD_OFFSET(KiLineCheck, _14, 0x14);
WWHD_OFFSET(KiLineCheck, __vtbl_20, 0x20);
WWHD_OFFSET(KiLineCheck, _24, 0x24);
WWHD_OFFSET(KiLineCheck, __vtbl_58, 0x58);
WWHD_OFFSET(KiLineCheck, mPass, 0x5C);
WWHD_OFFSET(KiLineCheck, _63, 0x63);
WWHD_OFFSET(KiLineCheck, __vtbl_64, 0x64);
WWHD_OFFSET(KiLineCheck, mGrp, 0x68);

WWHD_OFFSET(KiAttackInfo, mpObj, 0x00);
WWHD_OFFSET(KiAttackInfo, mpActor, 0x04);
WWHD_OFFSET(KiAttackInfo, mDamage, 0x08);
WWHD_OFFSET(KiAttackInfo, mbDead, 0x09);
WWHD_OFFSET(KiAttackInfo, mResultingAttackType, 0x0A);
WWHD_OFFSET(KiAttackInfo, _0B, 0x0B);
WWHD_OFFSET(KiAttackInfo, m0C, 0x0C);
WWHD_OFFSET(KiAttackInfo, mPlCutBit, 0x12);
WWHD_OFFSET(KiAttackInfo, pParticlePos, 0x14);
WWHD_OFFSET(KiAttackInfo, mHitSoundId, 0x18);

namespace KiAttackType { enum : u32 { LIGHT_ARROW=0x100000, ICE_ARROW=0x80000, FIRE_ARROW=0x40000, FIRE=0x200, WIND=0x200000 }; }
enum { KI_JNT_J_KI_HEAD_e=0xE, fpcNm_BGN_e=0xF3, fpcNm_BGN2_e=0xF4, fpcNm_BGN3_e=0xF5 };

WWHD_SIZE(dPa_followEcallBack_l,0x14);
