#pragma once
#include "bindings.h"

#include "m_Do/m_Do_ext.h"
#include "JSystem/J3DGraph.h"
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"
#include "d/d_particle.h"

// HD: the emitter's global transform and flags lie beyond the shared JPABaseEmitter view (the HD
// emitter is larger than the GameCube one from 0x60 on).
struct BstEmitter : JPABaseEmitter {
    /* 0x070 */ u8 m070[0x1F0 - 0x70];
    /* 0x1F0 */ Mtx34 mGlobalRotation;    // GameCube 0x1A8
    /* 0x220 */ u8 m220[0xC];
    /* 0x22C */ cXyz mGlobalTranslation;  // GameCube 0x1E4
    /* 0x238 */ u8 m238[0x1C];
    /* 0x254 */ be<u32> mFlags;           // GameCube 0x20C
    /* GameCube inline: mMaxFrame = -1; stopCreateParticle() (HD loads the flags first) */
    void becomeInvalidEmitter() {
        u32 flags = mFlags;
        mMaxFrame = -1;
        mFlags = flags | 1;
    }
};
WWHD_OFFSET(BstEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(BstEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(BstEmitter, mFlags, 0x254);
WWHD_SIZE(BstEmitter, 0x258);
inline BstEmitter* bstEmitter(JPABaseEmitter* emitter) { return static_cast<BstEmitter*>(emitter); }

// Gohdan (head and both hands): the zeldaret/tww layout and member names at the HD offsets
// (one layout for all bst sources). HD: fopEn_enemy_c and dKy_tevstr_c are
// larger, so every member after mPhs sits 0x11C (0x234 after mEnvLight) above its tww offset;
// the descriptive meaning is noted where tww only has an offset name.
struct bst_class : fopEn_enemy_c {
    enum Part_Type_e {
        Type_HEAD_e = 0,
        Type_LEFT_HAND_e = 1,
        Type_RIGHT_HAND_e = 2,
    };

    enum ActionType {
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
        ACTION_END_DEMO_e = 22,
    };

    /* 0x03C8 */ request_of_phase_process_class mPhs;
    /* 0x03D0 */ be<u8> mBstPartType;
    /* 0x03D1 */ u8 m02B5[3];
    /* 0x03D4 */ gptr<mDoExt_McaMorf> m02B8;
    /* 0x03D8 */ gptr<mDoExt_btkAnm> mpTexMtxAnimator;
    /* 0x03DC */ gptr<mDoExt_brkAnm> mpTevRegAnimator;
    /* 0x03E0 */ be<s8> m02C4; // animation state
    /* 0x03E1 */ u8 m02C5[3];
    /* 0x03E4 */ gptr<J3DModel> m02C8; // pedestal model
    /* 0x03E8 */ gptr<mDoExt_btkAnm> mpBtkAnm;
    /* 0x03EC */ gptr<mDoExt_brkAnm> mpBrkAnm;
    /* 0x03F0 */ be<s8> m02D4;
    /* 0x03F1 */ u8 m02D5[3];
    /* 0x03F4 */ dKy_tevstr_c mEnvLight;
    /* 0x05BC */ gptr<J3DModel> m0388;
    /* 0x05C0 */ be<u32> mShadowID;
    /* 0x05C4 */ gptr<J3DModel> m0390[10]; // beam models
    /* 0x05EC */ cXyz m03B8[10]; // beam positions
    /* 0x0664 */ cXyz m0430[10]; // beam velocities
    /* 0x06DC */ csXyz m04A8[10]; // beam angles
    /* 0x0718 */ be<s8> m04E4[10]; // beam states
    /* 0x0722 */ u8 m04EE[2];
    /* 0x0724 */ dCcD_Sph mCcD_beams[10];
    /* 0x12DC */ be<f32> m10A8[10]; // beam lengths
    /* 0x1304 */ be<f32> m10D0;
    /* 0x1308 */ be<s16> mUpdateLastFacingDirIfMultipleOf32;
    /* 0x130A */ be<s16> mActionType;
    /* 0x130C */ be<s16> m10D8;
    /* 0x130E */ be<s16> mDamage;
    /* 0x1310 */ cXyz mTargetPos;
    /* 0x131C */ be<s16> mLastCheckedPlayerFacingDir;
    /* 0x131E */ u8 m10EA[2];
    /* 0x1320 */ cXyz m10EC; // turn scale
    /* 0x132C */ be<f32> m10F8;
    /* 0x1330 */ be<s16> m10FC[5]; // timers: 0 = update target pos, 1 = sleep
    /* 0x133A */ be<s16> mDamageTimer;
    /* 0x133C */ be<s16> mState;
    /* 0x133E */ be<s16> mStateTimer;
    /* 0x1340 */ u8 m110C[4];
    /* 0x1344 */ be<f32> m1110;
    /* 0x1348 */ be<f32> m1114;
    /* 0x134C */ be<f32> m1118;
    /* 0x1350 */ be<s16> m111C;
    /* 0x1352 */ u8 m111E[2];
    /* 0x1354 */ be<f32> mHeadHurtSpeed;
    /* 0x1358 */ be<s16> mHurtRecoilAngle1;
    /* 0x135A */ be<s16> mHurtRecoilAngle2;
    /* 0x135C */ be<s16> mHeadHurtAngle;
    /* 0x135E */ be<s16> m112A;
    /* 0x1360 */ be<s16> m112C;
    /* 0x1362 */ u8 m112E[2];
    /* 0x1364 */ dBgS_AcchCir mAcchCir;
    /* 0x13A4 */ dBgS_ObjAcch mAcch;
    /* 0x1568 */ dCcD_Stts mStts;
    /* 0x15A4 */ dCcD_Cyl mHeadHurtCyl;
    /* 0x16D4 */ dCcD_Cyl mHandHurtCyl;
    /* 0x1804 */ dCcD_Sph mFingerSphs[19];
    /* 0x2E48 */ dCcD_Sph mEyeSphs[2];
    /* 0x30A0 */ u8 m2E6C[2];
    /* 0x30A2 */ be<s16> m2E6E;
    /* 0x30A4 */ be<s16> mFacingDir;
    /* 0x30A6 */ be<s8> mEyeHealth[2];
    /* 0x30A8 */ be<s16> m2E74[2]; // eye timers
    /* 0x30AC */ be<s16> m2E78[2]; // eyelid angles
    /* 0x30B0 */ be<s8> m2E7C; // head phase
    /* 0x30B1 */ u8 m2E7D[1];
    /* 0x30B2 */ be<s16> m2E7E[3]; // head timers (1 = mHeadBeamAttackTimer)
    /* 0x30B8 */ be<s16> mBombArrowDropTimer;
    /* 0x30BA */ u8 m2E86[2];
    /* 0x30BC */ be<u32> mBombId;
    /* 0x30C0 */ be<u32> mCreatedItemId; // nose item
    /* 0x30C4 */ be<u32> m2E90[2]; // eye attachment ids
    /* 0x30CC */ be<s16> m2E98; // head light mode
    /* 0x30CE */ be<s8> m2E9A; // camera state
    /* 0x30CF */ u8 m2E9B[1];
    /* 0x30D0 */ be<s16> msFrameCount; // camera frame
    /* 0x30D2 */ be<s8> m2E9E; // camera side
    /* 0x30D3 */ u8 m2E9F[1];
    /* 0x30D4 */ cXyz m2EA0; // camera eye
    /* 0x30E0 */ cXyz m2EAC; // camera center
    /* 0x30EC */ u8 m2EB8[0xC];
    /* 0x30F8 */ be<f32> m2EC4; // camera chase speed
    /* 0x30FC */ be<f32> m2EC8; // camera fovy
    /* 0x3100 */ be<f32> m2ECC; // camera shake
    /* 0x3104 */ be<f32> m2ED0; // room lighting blend
    /* 0x3108 */ be<s8> mRoomState;
    /* 0x3109 */ u8 m2ED5[3];
    /* 0x310C */ gptr<JPABaseEmitter> m2ED8[2]; // beam charge emitters
    /* 0x3114 */ gptr<JPABaseEmitter> m2EE0[5]; // HD: 5 words where tww has m2EE0[2] + 12 gap bytes
    /* 0x3128 */ gptr<JPABaseEmitter> m2EF4;
    /* 0x312C */ gptr<JPABaseEmitter> m2EF8;
    /* 0x3130 */ u8 m2EFC[4];
    /* 0x3134 */ dPa_smokeEcallBack mPa_smokeEcallBack;
    /* 0x3154 */ dKy_tevstr_c m2F20;
    /* 0x331C */ gptr<J3DModel> m2FD0; // auxiliary model A
    /* 0x3320 */ gptr<mDoExt_btkAnm> m2FD4;
    /* 0x3324 */ gptr<mDoExt_brkAnm> m2FD8;
    /* 0x3328 */ gptr<J3DModel> m2FDC; // auxiliary model B
    /* 0x332C */ gptr<mDoExt_brkAnm> m2FE0;
    /* 0x3330 */ be<s8> m2FE4; // intro complete
    /* 0x3331 */ be<u8> mHioSet;
    /* 0x3332 */ u8 m2FE6[2];
};
WWHD_OFFSET(bst_class, mPhs, 0x3C8);
WWHD_OFFSET(bst_class, mBstPartType, 0x3D0);
WWHD_OFFSET(bst_class, m02B8, 0x3D4);
WWHD_OFFSET(bst_class, m02C8, 0x3E4);
WWHD_OFFSET(bst_class, mEnvLight, 0x3F4);
WWHD_OFFSET(bst_class, m0388, 0x5BC);
WWHD_OFFSET(bst_class, m0390, 0x5C4);
WWHD_OFFSET(bst_class, m03B8, 0x5EC);
WWHD_OFFSET(bst_class, m0430, 0x664);
WWHD_OFFSET(bst_class, m04A8, 0x6DC);
WWHD_OFFSET(bst_class, m04E4, 0x718);
WWHD_OFFSET(bst_class, mCcD_beams, 0x724);
WWHD_OFFSET(bst_class, m10A8, 0x12DC);
WWHD_OFFSET(bst_class, m10D0, 0x1304);
WWHD_OFFSET(bst_class, mActionType, 0x130A);
WWHD_OFFSET(bst_class, mDamage, 0x130E);
WWHD_OFFSET(bst_class, mTargetPos, 0x1310);
WWHD_OFFSET(bst_class, m10EC, 0x1320);
WWHD_OFFSET(bst_class, m10FC, 0x1330);
WWHD_OFFSET(bst_class, mState, 0x133C);
WWHD_OFFSET(bst_class, m1110, 0x1344);
WWHD_OFFSET(bst_class, m111C, 0x1350);
WWHD_OFFSET(bst_class, mHeadHurtSpeed, 0x1354);
WWHD_OFFSET(bst_class, mHeadHurtAngle, 0x135C);
WWHD_OFFSET(bst_class, m112A, 0x135E);
WWHD_OFFSET(bst_class, m112C, 0x1360);
WWHD_OFFSET(bst_class, mAcchCir, 0x1364);
WWHD_OFFSET(bst_class, mAcch, 0x13A4);
WWHD_OFFSET(bst_class, mStts, 0x1568);
WWHD_OFFSET(bst_class, mHeadHurtCyl, 0x15A4);
WWHD_OFFSET(bst_class, mHandHurtCyl, 0x16D4);
WWHD_OFFSET(bst_class, mFingerSphs, 0x1804);
WWHD_OFFSET(bst_class, mEyeSphs, 0x2E48);
WWHD_OFFSET(bst_class, mFacingDir, 0x30A4);
WWHD_OFFSET(bst_class, mEyeHealth, 0x30A6);
WWHD_OFFSET(bst_class, m2E74, 0x30A8);
WWHD_OFFSET(bst_class, m2E78, 0x30AC);
WWHD_OFFSET(bst_class, m2E7C, 0x30B0);
WWHD_OFFSET(bst_class, m2E7E, 0x30B2);
WWHD_OFFSET(bst_class, mBombArrowDropTimer, 0x30B8);
WWHD_OFFSET(bst_class, mBombId, 0x30BC);
WWHD_OFFSET(bst_class, mCreatedItemId, 0x30C0);
WWHD_OFFSET(bst_class, m2E90, 0x30C4);
WWHD_OFFSET(bst_class, m2E98, 0x30CC);
WWHD_OFFSET(bst_class, m2E9A, 0x30CE);
WWHD_OFFSET(bst_class, msFrameCount, 0x30D0);
WWHD_OFFSET(bst_class, m2EA0, 0x30D4);
WWHD_OFFSET(bst_class, m2EAC, 0x30E0);
WWHD_OFFSET(bst_class, m2EC4, 0x30F8);
WWHD_OFFSET(bst_class, m2ECC, 0x3100);
WWHD_OFFSET(bst_class, m2ED0, 0x3104);
WWHD_OFFSET(bst_class, mRoomState, 0x3108);
WWHD_OFFSET(bst_class, m2ED8, 0x310C);
WWHD_OFFSET(bst_class, m2EE0, 0x3114);
WWHD_OFFSET(bst_class, m2EF4, 0x3128);
WWHD_OFFSET(bst_class, m2EF8, 0x312C);
WWHD_OFFSET(bst_class, mPa_smokeEcallBack, 0x3134);
WWHD_OFFSET(bst_class, m2F20, 0x3154);
WWHD_OFFSET(bst_class, m2FD0, 0x331C);
WWHD_OFFSET(bst_class, m2FE0, 0x332C);
WWHD_OFFSET(bst_class, m2FE4, 0x3330);
WWHD_OFFSET(bst_class, mHioSet, 0x3331);
WWHD_SIZE(bst_class, 0x3334);

// HD: the split translation unit defines this descriptor once in d_a_bst_execute.cpp.
struct gc_tu_data;
extern const gc_tu_data gc_tu;
