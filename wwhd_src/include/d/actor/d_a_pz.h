/* daPz_c (Princess Zelda, Ganon's Tower), WWHD layout. 
 *
 * GameCube -> WWHD (size 0x1049 -> 0x16B8, constructor 0245226C):
 * - fopNpc_npc_c is 0x7DC (d/d_npc.h): members +0x118 up to mPhs (0x884);
 * - mDoExt_invisibleModel (0x88C) and the private dKy_tevstr_c (0x894, HD 0x1C8): +0x114 / +0x22C;
 * - enemyice (0xB54) and enemyfire (0xF0C, HD 0x22C): +0x22C; from 0x1138 on +0x230;
 * - mDoExt_brkAnm/btkAnm/btpAnm grew (HD 0x78/0x74/0x74) and HD adds a second btp (0x1320);
 * - HD-only tail: material tables (0x1394..0x1418), four 0x98-byte packets (0x1418..0x1678) and
 *   a 16-entry material index table (0x1678).
 * Field names: GameCube names where the GameCube header has them, otherwise mNNNN = WWHD offset. */
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

/* daPz_matAnm_c : J3DMaterialAnm (HD 0x80, constructor 024514A4: vtable at +0x68) */
struct daPz_matAnm_c {
    /* 0x00 */ u8 _00[0x68];
    /* 0x68 */ be<u32> __vtbl;
    /* 0x6C */ be<f32> mNowOffsetX;
    /* 0x70 */ be<f32> mNowOffsetY;
    /* 0x74 */ u8 _74[8];
    /* 0x7C */ be<u8> mMoveFlag;
    /* 0x7D */ u8 _7D[3];
};
WWHD_SIZE(daPz_matAnm_c, 0x80);

/* the 0x98-byte HD packets at 0x1418.. (constructor 027F1278, vtable at +0xC) */
struct daPz_packet_l {
    /* 0x00 */ u8 _00[0x0C];
    /* 0x0C */ be<u32> __vtbl;
    /* 0x10 */ u8 _10[0x98 - 0x10];
};
WWHD_SIZE(daPz_packet_l, 0x98);

struct daPz_c : fopNpc_npc_c {
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };

    void _nodeHeadControl(J3DNode*, J3DModel*);
    void _nodeWaistControl(J3DNode*, J3DModel*);
    void _nodeWaist2Control(J3DNode*, J3DModel*);
    void _nodeSkirtControl(J3DNode*, J3DModel*);
    BOOL bodyCreateHeap();
    BOOL bowCreateHeap();
    BOOL _createHeap();
    void getGndPos();
    BOOL checkEyeArea(cXyz*);
    u32 getMsg();
    u16 next_msgStatus(be<u32>*);
    void anmAtr(u32); /* u16, compared as a full register */
    void eventOrder();
    void checkOrder();
    void setFallSplash();
    void setHeadSplash();
    void setRipple();
    void setJntStatus();
    BOOL demo();
    BOOL checkTgHit();
    void getArg();
    void setAttention();
    void setBowAnm(s8, s32); /* HD: the bool argument is passed on as a full register */
    void setBowString(s32); /* bool, tested as a full register */
    void setAnm(s8, s32, s32); /* (s8 anm, bool morf, int) */
    void setAnmRunSpeed();
    void setEyeBtp(int);
    void setEyeBtk(int);
    void setEyeAnm(s8);
    void ctrlEye();
    void playEyeAnm();
    void setMtx();
    void modeWaitInit();
    void modeWait();
    void modeMoveInit();
    void modeMove();
    void modeAttackWaitInit();
    void modeAttackWait();
    void modeAttackInit();
    void modeAttack();
    void modeDefendInit();
    void modeDefend();
    void modeDownInit();
    void modeDown();
    void modeAfraidInit();
    void modeAfraid();
    void modeSideStepInit();
    void modeSideStep();
    void modeBackStepInit();
    void modeBackStep();
    void modeTalkInit();
    void modeTalk();
    void modeFollowInit();
    void modeFollow();
    void modeProc(s32, s32);
    BOOL _execute();
    void bowDraw();
    void bodyDraw();
    BOOL _draw();
    void bodyCreateInit();
    void createInit();
    cPhs_State _create();
    BOOL _delete();

    /* 0x7DC */ be<s32> mMode;                 /* GameCube 0x6C4 */
    /* 0x7E0 */ be<s32> m07E0;
    /* 0x7E4 */ be<s32> m07E4;               /* anmAtr: message animation started */
    /* 0x7E8 */ be<u8> mTgHitTimer;
    /* 0x7E9 */ be<u8> mHitType;
    /* 0x7EA */ be<s8> mBckIdx;                /* dLib_bcks_setAnm: bck */
    /* 0x7EB */ be<s8> mAnm;                   /* GameCube m06D3: anm prm number */
    /* 0x7EC */ be<s8> mOldAnm;
    /* 0x7ED */ be<s8> mCurEye;                /* GameCube 0x6D5 */
    /* 0x7EE */ u8 _7EE[2];
    /* 0x7F0 */ be<s32> m07F0;
    /* 0x7F4 */ be<u8> m07F4;
    /* 0x7F5 */ be<s8> mBowBckIdx;
    /* 0x7F6 */ be<s8> mBowAnm;
    /* 0x7F7 */ be<s8> mBowOldAnm;
    /* 0x7F8 */ u8 mRipple[0x14];              /* dPa_rippleEcallBack */
    /* 0x80C */ dPa_followEcallBack mFollowCB0;
    /* 0x820 */ dPa_followEcallBack mFollowCB1;
    /* 0x834 */ cXyz m0834;
    /* 0x840 */ be<s32> m0840;               /* fall splash timer */
    /* 0x844 */ cXyz mHeadPos;                 /* head joint translation */
    /* 0x850 */ u8 _850[0x856 - 0x850];
    /* 0x856 */ be<u8> m0856;
    /* 0x857 */ be<u8> m0857;
    /* 0x858 */ be<u8> m0858;
    /* 0x859 */ u8 _859[3];
    /* 0x85C */ be<s32> m085C;                /* last hit was not Ganondorf */
    /* 0x860 */ cXyz mHitPos;
    /* 0x86C */ be<s32> m086C;                /* hits by others in a row */
    /* 0x870 */ be<s32> m0870;
    /* 0x874 */ be<u8> m0874;
    /* 0x875 */ u8 _875[3];
    /* 0x878 */ be<s32> m0878;                /* hits by Ganondorf in a row */
    /* 0x87C */ be<s32> m087C;
    /* 0x880 */ be<u8> m0880;
    /* 0x881 */ u8 _881[3];
    /* 0x884 */ request_of_phase_process_class mPhs;   /* GameCube 0x76C */
    /* 0x88C */ u8 mInvisibleModel[8];         /* mDoExt_invisibleModel, GameCube 0x778 */
    /* 0x894 */ dKy_tevstr_c mTevstr;          /* GameCube 0x780 */
    /* 0xA5C */ cXyz m0A5C;
    /* 0xA68 */ dBgS_LinChk mLinChk;
    /* 0xAD4 */ gptr<daPz_matAnm_c> mpMatAnm[2]; /* eyes L/R, GameCube m08A8 */
    /* 0xADC */ be<s32> m0ADC;
    /* 0xAE0 */ be<s32> m0AE0;
    /* 0xAE4 */ cXyz mEyePos;                  /* head joint * (0,0,0) */
    /* 0xAF0 */ cXyz mLookPos;
    /* 0xAFC */ cXyz m0AFC;
    /* 0xB08 */ cXyz mEyePos2;                 /* head joint * (24,-16,0) */
    /* 0xB14 */ be<s16> m0B14;
    /* 0xB16 */ be<u8> m0B16;
    /* 0xB17 */ u8 _B17;
    /* 0xB18 */ be<s32> m0B18;
    /* 0xB1C */ be<s32> m0B1C;
    /* 0xB20 */ be<s32> m0B20;
    /* 0xB24 */ be<s32> m0B24;
    /* 0xB28 */ cXyz m0B28;                    /* dLib_circle_path_c: centre */
    /* 0xB34 */ cXyz m0B34;                    /*   position on the circle */
    /* 0xB40 */ be<f32> m0B40;                 /*   radius */
    /* 0xB44 */ be<f32> m0B44;                 /*   walk speed */
    /* 0xB48 */ be<s16> m0B48;                 /*   angle */
    /* 0xB4A */ be<s16> m0B4A;                 /*   angle speed */
    /* 0xB4C */ be<s32> m0B4C;
    /* 0xB50 */ be<f32> m0B50;
    /* 0xB54 */ enemyice_l mEnemyIce;          /* GameCube 0x928 */
    /* 0xF0C */ enemyfire_l mEnemyFire;        /* GameCube 0xCE0 */
    /* 0x1138 */ Mtx34 mWaistMtx;
    /* 0x1168 */ cXyz mWaistPos;
    /* 0x1174 */ be<f32> m1174;
    /* 0x1178 */ be<u8> m1178;
    /* 0x1179 */ u8 _1179[3];
    /* 0x117C */ cXyz mGanondorfPosCurrent;  /* GameCube 0xF4C */
    /* 0x1188 */ cXyz mGanondorfPos4;        /* Ganondorf eyePos, GameCube 0xF58 */
    /* 0x1194 */ be<u8> mbHasGanondorf;      /* GameCube 0xF64 */
    /* 0x1195 */ be<u8> m1195;
    /* 0x1196 */ be<u8> m1196;
    /* 0x1197 */ be<u8> m1197;
    /* 0x1198 */ be<f32> m1198;
    /* 0x119C */ be<s16> m119C;
    /* 0x119E */ u8 _119E[2];
    /* 0x11A0 */ be<f32> m11A0;
    /* 0x11A4 */ u8 _11A4[4];
    /* 0x11A8 */ be<s16> mWaistAngleZ;
    /* 0x11AA */ be<s16> mWaistAngleY;
    /* 0x11AC */ be<s32> m11AC;
    /* 0x11B0 */ be<u8> m11B0;
    /* 0x11B1 */ be<u8> m11B1;
    /* 0x11B2 */ be<u8> mOrderState;           /* 1: speak event, 2: other event */
    /* 0x11B3 */ u8 _11B3;
    /* 0x11B4 */ be<s32> mArg;                 /* GameCube 0xF84 */
    /* 0x11B8 */ be<u8> m11B8;
    /* 0x11B9 */ u8 _11B9[3];
    /* 0x11BC */ gptr<mDoExt_McaMorf> mpBowMcaMorf;   /* GameCube 0xF8C */
    /* 0x11C0 */ u8 mBrkAnm[0x78];             /* mDoExt_brkAnm (HD 0x78), GameCube 0xF90 */
    /* 0x1238 */ u8 mBtkAnm[0x74];             /* mDoExt_btkAnm (HD 0x74), GameCube 0xFA8 */
    /* 0x12AC */ u8 mBtpAnm[0x74];             /* mDoExt_btpAnm (HD 0x74), GameCube 0xFBC */
    /* 0x1320 */ u8 mBtpAnm2[0x74];            /* HD: a second mDoExt_btpAnm */
    /* 0x1394 */ be<u32> m1394;                /* model data +8 */
    /* 0x1398 */ be<u32> mMat0[6];
    /* 0x13B0 */ be<u32> mMat1[6];
    /* 0x13C8 */ be<u32> mMat2[4];
    /* 0x13D8 */ be<u32> mMatSh0[6];
    /* 0x13F0 */ be<u32> mMatSh1[6];
    /* 0x1408 */ be<u32> mMatSh2[4];
    /* 0x1418 */ daPz_packet_l mPacket[4];
    /* 0x1678 */ be<u32> mMatIdx[16];          /* sead::SafeArray: low halfword = material index */
};
WWHD_OFFSET(daPz_c, mPhs, 0x884);
WWHD_OFFSET(daPz_c, mTevstr, 0x894);
WWHD_OFFSET(daPz_c, mLinChk, 0xA68);
WWHD_OFFSET(daPz_c, mEnemyIce, 0xB54);
WWHD_OFFSET(daPz_c, mEnemyFire, 0xF0C);
WWHD_OFFSET(daPz_c, mArg, 0x11B4);
WWHD_OFFSET(daPz_c, mBtpAnm2, 0x1320);
WWHD_OFFSET(daPz_c, mPacket, 0x1418);
WWHD_SIZE(daPz_c, 0x16B8);

/* ======== constants and local bindings used by the d_a_pz*.cpp part files ======== */
#define SAFESTRING_VTBL 0x10038558u /* this TU's sead::SafeString vtable */
#define PZ_VTBL 0x10038D60u         /* daPz_c vtable (constructor 0245226C) */
#define l_arcName STR(0x10038D00)   /* "Pz" */
#define m_cyl_src gabi::at<dCcD_SrcCyl>(0x10038D04) /* daPz_c::m_cyl_src */

/* daPz_HIO_c l_HIO (0x1046D454, size 0x10C): parameters read by offset */
#define L_HIO 0x1046D454u
static inline s16 hio_s16(u32 off) { return gabi::load<s16>(L_HIO + off); }
static inline f32 hio_f32(u32 off) { return gabi::load<f32>(L_HIO + off); }
static inline u8 hio_u8(u32 off) { return gabi::load<u8>(L_HIO + off); }

/* debug registers g_regHIO child 12 (REG12_S(i): 0x1047B688 + 12*0x90 + 2i) */
static inline s16 REG12_S(int i) { return REG_S(12, i); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint block at model+0x2C */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ gptr<J3DModelData> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
};
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty) */
static inline Mtx34* getAnmMtx(J3DModel* m, s32 jnt) {
    J3DMtxBlock_l* blk = ((J3DModel_l*)m)->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}
static inline J3DModel_l* j3dSys_getModel() { return gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline u16 J3DJoint_getJntNo(J3DJoint* j) { return gabi::load<u16>(gabi::ea(j) + 4); }

/* sead::SafeString equality (HD inline): both sides' virtual assureTermination (+0x14), then a
 * byte compare bounded by 0x40001 */
static inline bool SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b);
    if (s1 == b->mStringTop) return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(p + i);
        if (c != gabi::load<u8>(q + i)) return false;
        if (c == 0) return true;
    }
    return false;
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (HD: SafeString comparison; the start stage name at play+0x5134) */
static inline bool dComIfGp_isStartStage(u32 lit) {
    gabi::Local<SafeString> a;
    a->mStringTop = lit;
    a->__vtbl = SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mStringTop = dComIfGp_ea() + 0x5134;
    b->__vtbl = SAFESTRING_VTBL;
    return SafeString_eq(a, b);
}
static inline dSv_event_c* dComIfGs_getEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_getEvent(), f); }

/* 02587A0C dLib_bcks_setAnm(arc, morf, s8* bck, s8* prm, s8* old, const int* bcks, const dLib_anm_prm_c*, bool, HD int) */
static inline void dLib_bcks_setAnm(const char* arc, mDoExt_McaMorf* morf, be<s8>* bck, be<s8>* prm, be<s8>* old, u32 bcks,
                                    u32 prms, s32 force, s32 hd) {
    gabi::call(0x02587A0C, arc, morf, bck, prm, old, bcks, prms, force, hd);
}
static inline void dKy_tevstr_init(dKy_tevstr_c* t, s8 roomNo, u8 p) { gabi::call(0x0255FFF4, t, roomNo, p); }
/* eventInfo (dEvt_info_c at actor+0xF8): mCommand +0xF8, mCondition +0xFA, mEventId +0xFC */
static inline u16 eventInfo_getCommand(fopAc_ac_c* a) { return gabi::load<u16>(gabi::ea(a) + 0xF8); }
static inline void eventInfo_setEventId(fopAc_ac_c* a, s16 id) { gabi::store<s16>(gabi::ea(a) + 0xFC, id); }
