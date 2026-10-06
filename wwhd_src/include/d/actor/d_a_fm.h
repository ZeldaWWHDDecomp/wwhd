/* daFm_c (Floormaster), WWHD layout. 
 *
 * GameCube -> WWHD (size 0xEA0 -> 0x101C, constructor 02141074):
 * - +0x11C up to mBtkAnm (fopEn_enemy_c);
 * - mDoExt_btkAnm grew by 0x60 (HD 0x74): +0x17C from field_0x3E0 on.
 * Offsets from the constructor and the verified functions. */
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

WWHD_OPAQUE(JntHit_c);

struct Quaternion_fm { be<f32> x, y, z, w; };

struct daFm_c : fopEn_enemy_c {
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };

    bool isBodyAppear() {
        int m = mMode;
        return m != 1 && m != 0x12 && m != 0 && (m != 2 && (m != 4) && (m != 3 && (m != 0x11)));
    }
    void modeProcInit(int newMode) { modeProc(PROC_INIT_e, newMode); }

    void _nodeControl(J3DNode*, J3DModel*);
    BOOL _createHeap();
    bool holeCreateHeap();
    bool bodyCreateHeap();
    bool jntHitCreateHeap();
    BOOL _pathMove(cXyz*, cXyz*, cXyz*);
    fopAc_ac_c* searchNearOtherActor(fopAc_ac_c*);
    void* searchNearFm(fopAc_ac_c*);
    void moveRndBack();
    void moveRndEscape();
    void spAttackVJump();
    void spAttackJump();
    void spAttackNone();
    void iceProc();
    void bodySetMtx();
    void holeSetMtx();
    void setCollision();
    void setAttention();
    bool checkTgHit();
    void setGrabPos();
    void getOffsetPos(cXyz* out);  /* returns cXyz by hidden pointer */
    u8 checkPlayerGrabBomb();
    u8 checkPlayerGrabNpc();
    u8 checkPlayerGrabTarget();
    bool isGrabPos();
    bool isGrab();
    bool isGrabFoot();
    void modeSwWaitInit();
    void modeSwWait();
    void modeHideInit();
    void modeHide();
    void modeUnderFootInit();
    void modeUnderFoot();
    void modePathMoveInit();
    void modePathMove();
    void modeGoalKeeperInit();
    void modeGoalKeeper();
    void modeAppearInit();
    void modeAppear();
    void modeDisappearInit();
    void modeDisappear();
    void modeWaitInit();
    void modeWait();
    void modeAttackInit();
    void modeAttack();
    void modeThrowInit();
    void modeThrow();
    void modeGrabFootDemoInit();
    void modeGrabFootDemo();
    void modeParalysisInit();
    void modeParalysis();
    void modeDamageInit();
    void modeDamage();
    void modeGrabInit();
    void modeGrab();
    void modeGrabDemoInit();
    void modeGrabDemo();
    void modeDeathInit();
    void modeDeath();
    void modePrepareItemInit();
    void modePrepareItem();
    void modeGrabNpcDemoInit();
    void modeGrabNpcDemo();
    void modePlayerStartDemoInit();
    void modePlayerStartDemo();
    void modeDeleteInit();
    void modeDelete();
    void modeBikubikuInit();
    void modeBikubiku();
    void modeProc(Proc_e, int);
    void setAnm(s8, bool);
    void cancelGrab();
    void calcInvKine(fopAc_ac_c*);
    void resetInvKine();
    void grabBomb();
    void grabTsubo();
    void grabPlayer();
    void grabNPC();
    void searchTarget();
    void setBaseTarget();
    void turnToBaseTarget();
    bool isNpc(fopAc_ac_c*);
    bool checkHeight(fopAc_ac_c*);
    bool isLink(fopAc_ac_c*);
    bool isLinkControl();
    bool areaCheck();
    bool lineCheck(cXyz*, cXyz*);
    int setRnd(int, int);
    void setHoleEffect();
    void holeExecute();
    bool setHoleScale(f32, f32, f32);
    bool _execute();
    void MtxToRot(Mtx34*, csXyz*);
    void debugDraw();
    void holeDraw();
    void bodyDraw();
    bool _draw();
    void getArg();
    void createInit();
    cPhs_State _create();
    bool _delete();

    /* 0x3C8 */ be<s32> mMode;
    /* 0x3CC */ dPa_followEcallBack mpFollowEcallBack;
    /* 0x3E0 */ be<s8> mBckIdx;
    /* 0x3E1 */ be<s8> mAnmPrmIdx;
    /* 0x3E2 */ be<s8> mOldAnmPrmIdx;
    /* 0x3E3 */ be<u8> field_0x2C7;
    /* 0x3E4 */ be<s16> field_0x2C8;
    /* 0x3E6 */ u8 _3E6[2];
    /* 0x3E8 */ be<s32> m_path_no;
    /* 0x3EC */ be<s32> field_0x2D0;
    /* 0x3F0 */ be<s32> field_0x2D4;
    /* 0x3F4 */ be<s32> field_0x2D8;
    /* 0x3F8 */ be<s32> field_0x2DC;
    /* 0x3FC */ be<f32> field_0x2E0;
    /* 0x400 */ be<u8> field_0x2E4;
    /* 0x401 */ be<u8> field_0x2E5;
    /* 0x402 */ u8 _402[2];
    /* 0x404 */ cXyz field_0x2E8[6];
    /* 0x44C */ Quaternion_fm field_0x330[6];
    /* 0x4AC */ be<s32> field_0x390;
    /* 0x4B0 */ be<f32> field_0x394;
    /* 0x4B4 */ cXyz field_0x398;
    /* 0x4C0 */ cXyz field_0x3A4;
    /* 0x4CC */ cXyz field_0x3B0;
    /* 0x4D8 */ be<s8> field_0x3BC;
    /* 0x4D9 */ u8 _4D9[3];
    /* 0x4DC */ gptr<dPath> mpPath;
    /* 0x4E0 */ u8 _4E0[4];
    /* 0x4E4 */ gptr<J3DModel> mpModel;
    /* 0x4E8 */ mDoExt_btkAnm mBtkAnm;
    /* 0x55C */ be<f32> field_0x3E0;
    /* 0x560 */ cXyz field_0x3E4;
    /* 0x56C */ request_of_phase_process_class mPhs;
    /* 0x574 */ u8 _574[4];
    /* 0x578 */ be<u8> field_0x3FC;
    /* 0x579 */ u8 _579[3];
    /* 0x57C */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x580 */ dBgS_ObjAcch mObjAcch;
    /* 0x744 */ dBgS_AcchCir mAcchCir;
    /* 0x784 */ u8 mInvisibleModel[8];  /* mDoExt_invisibleModel */
    /* 0x78C */ cXyz field_0x610;
    /* 0x798 */ cXyz field_0x61C;
    /* 0x7A4 */ u8 _7A4[8];
    /* 0x7AC */ cXyz field_0x630;
    /* 0x7B8 */ cXyz field_0x63C;
    /* 0x7C4 */ be<s32> field_0x648;
    /* 0x7C8 */ be<s32> field_0x64C;
    /* 0x7CC */ be<s32> field_0x650;
    /* 0x7D0 */ be<s32> mSinkTimer;
    /* 0x7D4 */ be<s32> field_0x658;
    /* 0x7D8 */ be<s32> field_0x65C;
    /* 0x7DC */ cXyz field_0x660;
    /* 0x7E8 */ gptr<fopAc_ac_c> mpActorTarget;
    /* 0x7EC */ be<u32> mProcId;
    /* 0x7F0 */ cXyz mGrabPos;
    /* 0x7FC */ be<s16> field_0x680;
    /* 0x7FE */ u8 _7FE[2];
    /* 0x800 */ be<s32> field_0x684;
    /* 0x804 */ be<u8> field_0x688;
    /* 0x805 */ u8 _805;
    /* 0x806 */ be<s16> field_0x68A;
    /* 0x808 */ be<s16> field_0x68C;
    /* 0x80A */ u8 _80A[2];
    /* 0x80C */ cXyz field_0x690;
    /* 0x818 */ cXyz field_0x69C;
    /* 0x824 */ be<u32> mProcId2;
    /* 0x828 */ u8 _828[8];
    /* 0x830 */ be<s32> field_0x6B4;
    /* 0x834 */ be<u8> field_0x6B8;
    /* 0x835 */ u8 _835[3];
    /* 0x838 */ Mtx34 field_0x6BC;
    /* 0x868 */ gptr<JntHit_c> mpJntHit;
    /* 0x86C */ dCcD_Stts mStts;
    /* 0x8A8 */ dCcD_Stts mStts2;
    /* 0x8E4 */ dCcD_Sph mSph;
    /* 0xA10 */ dCcD_Cyl mCyl;
    /* 0xB40 */ be<s32> field_0x9C4;
    /* 0xB44 */ be<u8> mHitType;
    /* 0xB45 */ u8 _B45[3];
    /* 0xB48 */ gptr<fopAc_ac_c> mBaseTarget;
    /* 0xB4C */ be<s16> field_0x9D0;
    /* 0xB4E */ u8 _B4E[2];
    /* 0xB50 */ be<f32> field_0x9D4;
    /* 0xB54 */ dBgS_LinChk mLinChk;  /* dBgS_ObjLinChk */
    /* 0xBC0 */ u8 _BC0[4];
    /* 0xBC4 */ cXyz field_0xA48[12];
    /* 0xC54 */ be<u8> field_0xAD8[12];
    /* 0xC60 */ be<u8> field_0xAE4;
    /* 0xC61 */ be<u8> field_0xAE5;
    /* 0xC62 */ u8 _C62[2];
    /* 0xC64 */ enemyice_l mEnemyIce;
};
WWHD_OFFSET(daFm_c, mMode, 0x3C8);
WWHD_OFFSET(daFm_c, mBtkAnm, 0x4E8);
WWHD_OFFSET(daFm_c, field_0x3E0, 0x55C);
WWHD_OFFSET(daFm_c, mObjAcch, 0x580);
WWHD_OFFSET(daFm_c, mAcchCir, 0x744);
WWHD_OFFSET(daFm_c, mInvisibleModel, 0x784);
WWHD_OFFSET(daFm_c, mpJntHit, 0x868);
WWHD_OFFSET(daFm_c, mStts, 0x86C);
WWHD_OFFSET(daFm_c, mSph, 0x8E4);
WWHD_OFFSET(daFm_c, mCyl, 0xA10);
WWHD_OFFSET(daFm_c, mLinChk, 0xB54);
WWHD_OFFSET(daFm_c, field_0xA48, 0xBC4);
WWHD_OFFSET(daFm_c, mEnemyIce, 0xC64);
WWHD_SIZE(daFm_c, 0x101C);
