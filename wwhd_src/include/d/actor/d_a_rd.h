/* daRd_c (ReDead), WWHD layout. 
 *
 * GameCube -> WWHD (size 0xD44 -> 0xF1C, constructor 0245AA10):
 * - +0x11C up to mpMorf (fopEn_enemy_c);
 * - mDoExt_btkAnm (HD 0x74) and mDoExt_brkAnm (HD 0x78) grew by 0x60 each: +0x1DC from mSpawnPos;
 * - mShadowId and the padding before it (GameCube 0x68C..0x694) are gone (HD has no blob shadow):
 *   +0x1D4 from mInvisModel;
 * - enemyfire grew by 4 (LIGHT_INFLUENCE 0x24): +0x1D8 from mpJntHit.
 * Offsets from the verified functions. */
#pragma once
#include "bindings.h"

#ifndef WWHD_ENEMYICE_FIRE_L
#define WWHD_ENEMYICE_FIRE_L
/* enemyice / enemyfire (c_damagereaction), WWHD layouts (SHARED-CANDIDATE, as in d_a_cc.h) */
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

/* mDoExt_brkAnm, HD 0x78: its J3DFrameCtrl first (frame +4) (SHARED-CANDIDATE, as d_a_npc_ji1.h) */
struct mDoExt_brkAnm_rd {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ u8 _10[0x78 - 0x10];
};
WWHD_SIZE(mDoExt_brkAnm_rd, 0x78);

WWHD_OPAQUE(JntHit_c);

struct daRd_c : fopEn_enemy_c {
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };
    enum Mode {
        MODE_WAIT = 0x0, MODE_DAMAGE = 0x1, MODE_PARALYSIS = 0x2, MODE_DEATH = 0x3, MODE_MOVE = 0x4,
        MODE_CRY = 0x5, MODE_CRY_WAIT = 0x6, MODE_ATTACK = 0x7, MODE_RETURN = 0x8, MODE_SILENT_PRAY = 0x9,
        MODE_SW_WAIT = 0xA, MODE_KANOKE = 0xB, MODE_NULL,
    };
    enum BckIdx { BckIdx_BEAM_HIT = 0xB, BckIdx_BEAM = 0xC, BckIdx_BEAM_END = 0xD };
    enum AnmPrm {
        AnmPrm_TACHIP0 = 0x0, AnmPrm_TACHIP1 = 0x1, AnmPrm_SUWARIP = 0x2, AnmPrm_WALK2ATACK = 0x3,
        AnmPrm_ATACK = 0x4, AnmPrm_ATACK2WALK = 0x5, AnmPrm_WALK = 0x6, AnmPrm_DAMAGE = 0x7,
        AnmPrm_DEAD = 0x8, AnmPrm_TATSU = 0x9, AnmPrm_SUWARU = 0xA, AnmPrm_KANOKEP = 0xB,
        AnmPrm_BEAM_HIT = 0xC, AnmPrm_BEAM = 0xD, AnmPrm_BEAM_END = 0xE, AnmPrm_NULL,
    };

    /* 0x3C8 */ be<s32> mMode;
    /* 0x3CC */ u8 m2B0[4];
    /* 0x3D0 */ be<s32> mWhichIdleAnm;
    /* 0x3D4 */ be<s32> mAreaRadius;
    /* 0x3D8 */ be<s32> mChecksSwitch;
    /* 0x3DC */ be<s32> mSwNo;
    /* 0x3E0 */ be<u8> m2C4;
    /* 0x3E1 */ be<u8> mHitType;
    /* 0x3E2 */ u8 m2C6[2];
    /* 0x3E4 */ request_of_phase_process_class mPhs;
    /* 0x3EC */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3F0 */ mDoExt_btkAnm mBtkAnm;
    /* 0x464 */ mDoExt_brkAnm_rd mBrkAnm;
    /* 0x4DC */ cXyz mSpawnPos;
    /* 0x4E8 */ be<s16> mSpawnAngle;
    /* 0x4EA */ u8 m30E[2];
    /* 0x4EC */ be<s32> mTimer1;
    /* 0x4F0 */ be<s32> mTimer2;
    /* 0x4F4 */ be<s32> mBreakFreeCounter;
    /* 0x4F8 */ dBgS_ObjAcch mAcch;
    /* 0x6BC */ dBgS_AcchCir mAcchCir;
    /* 0x6FC */ dCcD_Stts mStts;
    /* 0x738 */ dCcD_Cyl mCyl;
    /* HD: no mShadowId */
    /* 0x868 */ u8 mInvisModel[8];        /* mDoExt_invisibleModel */
    /* 0x870 */ dNpc_JntCtrl_c mJntCtrl;
    /* 0x8A4 */ be<u32> mCorpseID;
    /* 0x8A8 */ be<s32> m6D4;
    /* 0x8AC */ be<s8> mBckIdx;
    /* 0x8AD */ be<s8> mAnmPrmIdx;
    /* 0x8AE */ be<s8> mOldAnmPrmIdx;
    /* 0x8AF */ be<s8> m6DB;
    /* 0x8B0 */ be<s8> m6DC;
    /* 0x8B1 */ u8 m6DD[3];
    /* 0x8B4 */ enemyice_l mEnemyIce;
    /* 0xC6C */ enemyfire_l mEnemyFire;
    /* 0xE98 */ gptr<JntHit_c> mpJntHit;
    /* 0xE9C */ cXyz mTargetPos;
    /* 0xEA8 */ cXyz mRdEyePos;
    /* 0xEB4 */ be<s16> mMaxHeadTurnVel;
    /* 0xEB6 */ u8 mCDE[2];
    /* 0xEB8 */ be<s32> mCE0;
    /* 0xEBC */ be<s32> mCE4;
    /* 0xEC0 */ Mtx34 mCE8;
    /* 0xEF0 */ be<s16> mHeadAngle;
    /* 0xEF2 */ be<s16> mD1A;
    /* 0xEF4 */ be<s16> mD1C;
    /* 0xEF6 */ be<s16> mD1E;
    /* 0xEF8 */ u8 mD20[0xF06 - 0xEF8];
    /* 0xF06 */ be<s16> mD2E;
    /* 0xF08 */ u8 mD30[4];
    /* 0xF0C */ be<u8> mbIkari;
    /* 0xF0D */ u8 mD35[3];
    /* 0xF10 */ be<f32> mD38;
    /* 0xF14 */ be<s32> mD3C;
    /* 0xF18 */ be<s32> mD40;

    bool isAnm(s8 idx) { return mAnmPrmIdx == idx; }
    void onIkari() { mbIkari = 1; }
    void offIkari() { mbIkari = 0; }
    bool isIkari() { return mbIkari != 0; }
    void modeProcInit(int newMode) { modeProc(PROC_INIT_e, newMode); }

    fopAc_ac_c* _searchNearDeadRd(fopAc_ac_c*);
    void _nodeControl(J3DNode*, J3DModel*);
    void _nodeHeadControl(J3DNode*, J3DModel*);
    BOOL _createHeap();
    bool createArrowHeap();
    bool checkPlayerInAttack();
    bool checkPlayerInCry();
    void lookBack();
    bool checkTgHit();
    void setCollision();
    void setIceCollision();
    void setAttention();
    void setMtx();
    void modeWaitInit();
    void modeWait();
    void modeDeathInit();
    void modeDeath();
    void modeDamageInit();
    void modeDamage();
    void modeParalysisInit();
    void modeParalysis();
    void modeMoveInit();
    void modeMove();
    void modeCryInit();
    void modeCry();
    void modeCryWaitInit();
    void modeCryWait();
    void modeAttackInit();
    void modeAttack();
    void modeReturnInit();
    void modeReturn();
    void modeSilentPrayInit();
    void modeSilentPray();
    void modeSwWaitInit();
    void modeSwWait();
    void modeKanokeInit();
    void modeKanoke();
    void modeProc(Proc_e, int);
    void setBrkAnm(s8);
    void setBtkAnm(s8);
    void setAnm(s8, bool);
    bool _execute();
    void debugDraw();
    bool _draw();
    bool isLinkControl();
    void createInit();
    void getArg();
    cPhs_State _create();
    bool _delete();
};
WWHD_OFFSET(daRd_c, mMode, 0x3C8);
WWHD_OFFSET(daRd_c, mPhs, 0x3E4);
WWHD_OFFSET(daRd_c, mBrkAnm, 0x464);
WWHD_OFFSET(daRd_c, mSpawnPos, 0x4DC);
WWHD_OFFSET(daRd_c, mAcch, 0x4F8);
WWHD_OFFSET(daRd_c, mCyl, 0x738);
WWHD_OFFSET(daRd_c, mJntCtrl, 0x870);
WWHD_OFFSET(daRd_c, mEnemyIce, 0x8B4);
WWHD_OFFSET(daRd_c, mEnemyFire, 0xC6C);
WWHD_OFFSET(daRd_c, mpJntHit, 0xE98);
WWHD_OFFSET(daRd_c, mCE8, 0xEC0);
WWHD_OFFSET(daRd_c, mD2E, 0xF06);
WWHD_OFFSET(daRd_c, mD38, 0xF10);
WWHD_SIZE(daRd_c, 0xF1C);
