/* daDaiocta_c (Big Octo, body), WWHD layout. 
 *
 * GameCube -> WWHD, measured from _create (02119CE8, fopAcM_ct with the inline member
 * constructors), the deleting destructor (0211D204) and the other functions of the unit; the
 * profile size is 0x5910 (GameCube 0x31C8):
 * - fopAc_ac_c +0x11C up to m057C (0x698);
 * - HD-only s32 at 0x69C (a demo fallback timer: getArg zeroes it, modeDemo arms it with 60 and
 *   ends the suction event when it runs down), so mPhs and everything after it is +0x120;
 * - mDoExt_brkAnm 0x78 (GameCube 0x18): from mSph on +0x180;
 * - mDoExt_btkAnm 0x74, mDoExt_bckAnm 0x8C: the bubble ("awa") arrays grow, mAwaTimers.. +0x2748.
 * The separate eye actor is daDaiocta_Eye_c (d_a_daiocta_eye.h). */
#pragma once
#include "bindings.h"

struct daDaiocta_c : fopAc_ac_c {
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };
    enum Mode_e {
        MODE_HIDE = 0, MODE_APPEAR = 1, MODE_WAIT = 2, MODE_DAMAGE = 3,
        MODE_DAMAGE_BOMB = 4, MODE_DELETE = 5, MODE_DEMO = 6, MODE_NULL = 7
    };

    /* 0x03AC */ cXyz mSphCenters[37];          /* joint positions (_nodeControl) */
    /* 0x0568 */ u8 m0568[0x5FC - 0x568];
    /* 0x05FC */ be<u32> mAnmMtxIndices[12];     /* eye joints */
    /* 0x062C */ u8 m062C[0x688 - 0x62C];
    /* 0x0688 */ be<s32> mMode;
    /* 0x068C */ be<s8> mAnmIdx;
    /* 0x068D */ be<s8> mPrmIdx;
    /* 0x068E */ be<s8> mOldPrmIdx;
    /* 0x068F */ be<u8> mOctoType;
    /* 0x0690 */ be<u8> mSwitchNo;
    /* 0x0691 */ be<u8> m575;                    /* exit (scls) number after the suction demo */
    /* 0x0692 */ u8 m0692[2];
    /* 0x0694 */ be<f32> mAppearRadius;
    /* 0x0698 */ be<u8> m057C;                   /* from home.angle.z: 1 = the fairy-island variant */
    /* 0x0699 */ u8 m0699[3];
    /* 0x069C */ be<s32> mDemoEndTimer;          /* HD-only */
    /* 0x06A0 */ u8 mPhs[8];                     /* request_of_phase_process_class */
    /* 0x06A8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x06AC */ u8 mBrkAnm1[0x78];              /* mDoExt_brkAnm (frame at +4) */
    /* 0x0724 */ u8 mSph[6][0x12C];              /* dCcD_Sph */
    /* 0x0E2C */ u8 mCps[17][0x138];             /* dCcD_Cps */
    /* 0x22E4 */ u8 mStts[0x3C];                 /* dCcD_Stts */
    /* 0x2320 */ cXyz m21A0;
    /* 0x232C */ cXyz m21AC;
    /* 0x2338 */ cXyz m21B8;
    /* 0x2344 */ cXyz m21C4;
    /* 0x2350 */ cXyz m21D0;
    /* 0x235C */ cXyz m21DC;
    /* 0x2368 */ cXyz m21E8;
    /* 0x2374 */ cXyz m21F4;                     /* ship position during the suction demo */
    /* 0x2380 */ csXyz m2200;                    /* ship angle during the suction demo */
    /* 0x2386 */ u8 m2386[2];
    /* 0x2388 */ be<f32> m2208;
    /* 0x238C */ cXyz m220C;                     /* position at creation */
    /* 0x2398 */ u8 mAcch[0x1C4];                /* dBgS_ObjAcch (flags +0x28, sea height +0xD0, m_gnd +0xE8) */
    /* 0x255C */ u8 mAcchCir[0x40];              /* dBgS_AcchCir */
    /* 0x259C */ dPa_followEcallBack mParticleCallback;
    /* 0x25B0 */ be<f32> mWaterY;
    /* 0x25B4 */ cXyz m2434;                     /* mouth position */
    /* 0x25C0 */ be<u32> mDaioctaEyePcId[12];
    /* 0x25F0 */ be<u8> mEyeAlloc[12];
    /* 0x25FC */ u8 m25FC[0x283C - 0x25FC];
    /* 0x283C */ be<s32> mRotEyeTimer;
    /* 0x2840 */ be<u8> m26C0;
    /* 0x2841 */ be<u8> m26C1;
    /* 0x2842 */ u8 m2842[2];
    /* 0x2844 */ be<u32> mAuzuId;
    /* 0x2848 */ be<u32> mpJntHit;
    /* 0x284C */ gptr<J3DModel> mpSuikomiModel;
    /* 0x2850 */ u8 mBrkAnm2[0x78];              /* mDoExt_brkAnm */
    /* 0x28C8 */ u8 mBtkAnm[0x74];               /* mDoExt_btkAnm */
    /* 0x293C */ gptr<J3DModel> mpAwaModels[30];
    /* 0x29B4 */ u8 mAwaBckAnms[30][0x8C];       /* mDoExt_bckAnm */
    /* 0x3A1C */ u8 mAwaBtkAnms[30][0x74];       /* mDoExt_btkAnm */
    /* 0x47B4 */ u8 mAwaBrkAnms[30][0x78];       /* mDoExt_brkAnm */
    /* 0x55C4 */ be<s32> mAwaTimers[30];
    /* 0x563C */ cXyz mAwaTranslation[30];
    /* 0x57A4 */ cXyz mAwaScale[30];
    /* 0x590C */ be<u8> m31C4;
    /* 0x590D */ u8 m590D[3];

    void _coHit(fopAc_ac_c*);
    void _nodeControl(J3DNode*, J3DModel*);
    BOOL _createHeap();
    BOOL createAwaHeap();
    BOOL createSuikomiHeap();
    BOOL createBodyHeap();
    BOOL createArrowHitHeap();
    void setMtx();
    void setSuikomiMtx();
    void setAwaMtx();
    void initMtx();
    void setEffect(u16);
    void setAwaRandom(int);
    void initAwa();
    void execAwa();
    bool isLivingEye();
    bool isDead();
    bool isDamageEye();
    bool isDamageBombEye();
    void setRotEye();
    void setCollision();
    void modeHideInit();
    void modeHide();
    void modeAppearInit();
    void modeAppear();
    void modeWaitInit();
    void modeWait();
    void modeDamageInit();
    void modeDamage();
    void modeDamageBombInit();
    void modeDamageBomb();
    void modeDemoInit();
    void modeDemo();
    void modeDeleteInit();
    void modeDelete();
    void modeProc(int, int);
    void setAnm();
    void setWater();
    bool _execute();
    void drawAwa();
    void drawSuikomi();
    void drawDebug();
    bool _draw();
    void getArg();
    void createInit();
    cPhs_State _create();
    bool _delete();
};
WWHD_OFFSET(daDaiocta_c, mSphCenters, 0x3AC);
WWHD_OFFSET(daDaiocta_c, mAnmMtxIndices, 0x5FC);
WWHD_OFFSET(daDaiocta_c, mMode, 0x688);
WWHD_OFFSET(daDaiocta_c, mDemoEndTimer, 0x69C);
WWHD_OFFSET(daDaiocta_c, mpMorf, 0x6A8);
WWHD_OFFSET(daDaiocta_c, mSph, 0x724);
WWHD_OFFSET(daDaiocta_c, mCps, 0xE2C);
WWHD_OFFSET(daDaiocta_c, mStts, 0x22E4);
WWHD_OFFSET(daDaiocta_c, m21A0, 0x2320);
WWHD_OFFSET(daDaiocta_c, m2208, 0x2388);
WWHD_OFFSET(daDaiocta_c, mAcch, 0x2398);
WWHD_OFFSET(daDaiocta_c, mAcchCir, 0x255C);
WWHD_OFFSET(daDaiocta_c, mParticleCallback, 0x259C);
WWHD_OFFSET(daDaiocta_c, mWaterY, 0x25B0);
WWHD_OFFSET(daDaiocta_c, mDaioctaEyePcId, 0x25C0);
WWHD_OFFSET(daDaiocta_c, mEyeAlloc, 0x25F0);
WWHD_OFFSET(daDaiocta_c, mRotEyeTimer, 0x283C);
WWHD_OFFSET(daDaiocta_c, mAuzuId, 0x2844);
WWHD_OFFSET(daDaiocta_c, mBrkAnm2, 0x2850);
WWHD_OFFSET(daDaiocta_c, mpAwaModels, 0x293C);
WWHD_OFFSET(daDaiocta_c, mAwaBckAnms, 0x29B4);
WWHD_OFFSET(daDaiocta_c, mAwaBtkAnms, 0x3A1C);
WWHD_OFFSET(daDaiocta_c, mAwaBrkAnms, 0x47B4);
WWHD_OFFSET(daDaiocta_c, mAwaTimers, 0x55C4);
WWHD_OFFSET(daDaiocta_c, mAwaTranslation, 0x563C);
WWHD_OFFSET(daDaiocta_c, mAwaScale, 0x57A4);
WWHD_OFFSET(daDaiocta_c, m31C4, 0x590C);
WWHD_SIZE(daDaiocta_c, 0x5910);

/* daDaiocta_HIO_c: GameCube layout (0x108, the mDoHIO_entry_c vtable at +0) */
#define DAIOCTA_HIO 0x10463944u
#define DAIOCTA_ARC 0x1000D418u              /* "daiocta" (m_arc_name) */
#define DAIOCTA_SAFESTRING_VTBL 0x1000CFB4u  /* this TU's sead::SafeString vtable */
#define DAIOCTA_VTBL 0x1000D034u             /* daDaiocta_c vtable (HD virtual destructor) */
