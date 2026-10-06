/* daNpc_kam_c (Hyoi seagull, the seagull Link controls with a Hyoi Pear), WWHD layout.
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_kamome (profile 0x101BEDE8: size 0xBCC,
 * GameCube 0xC28; the constructor is inlined in create 0225F064):
 * - daPy_npc_c base: HD 0x608 (layout daPy_npc_l from d_a_npc_md.h).
 * - members from mPhs (GameCube 0x4EC) to mAcchCirs are +0x11C;
 * - HD: one dBgS_AcchCir instead of two (-0x40 from mLinChk on);
 * - HD: one dCcD_Sph instead of two (the GameCube mTgSph is gone; the hit checks use mAtSph):
 *   -0x50 net from mCps on (mCps 0x9EC, GameCube 0xA3C);
 * - mShadowId (GameCube 0xBC8) is gone (HD shadows) and the two pointers to member are 8 bytes
 *   (GHS) instead of 12: -0x5C from mHitFlags on;
 * - HD-only byte 0xBCA (GameCube padding 0xC26): set while the seagull waits hidden at its home
 *   (waitNpcAction), cleared by initialDescendEvent; draw and the movement part of execute skip
 *   while it is set.
 * Field names are the GameCube ones. */
#pragma once
#include "d/actor/d_a_npc_md.h" /* daPy_npc_l, ProcFunc_l, md_pmf_call, cBgS_PolyInfo_l */

/* this TU's statics (HD addresses) */
#define KAM_VTBL 0x1001B720           /* daNpc_kam_c vtable (create 0225F064) */
#define KAM_SAFESTRING_VTBL 0x1001B454 /* this TU's sead::SafeString vtable */
#define KAM_L_HIO 0x10467444          /* l_HIO (daNpc_kam_HIO_c, 0x50) */
#define KAM_M_HYOI_KAMOME 0x101D5F45  /* bool daNpc_kam_c::m_hyoi_kamome */
#define KAM_L_ACT 0x101BEC48          /* daNpc_kam_c* l_act */
#define KAM_L_STAFF_NAME 0x101BEC4C   /* const char* l_staff_name ("HyoiKam") */
#define KAM_L_DEMO_CHK_CNT 0x101BEC50 /* s16 l_demo_start_chk_cnt */
#define KAM_L_DEMO_CHK_FLAG 0x101BEC52 /* s16 l_demo_start_chk_flag */

/* daNpc_kam_HIO1_c (0x2C; GHS: vtable pointer after the members) */
struct daNpc_kam_HIO1_l {
    /* 0x00 */ be<f32> mSpeedF;
    /* 0x04 */ be<f32> mUnused08;
    /* 0x08 */ be<f32> mFlappingSpeedF;
    /* 0x0C */ be<f32> mAccelF;
    /* 0x10 */ be<s16> mGlidingAngVelY;
    /* 0x12 */ be<s16> mGlidingAngVelX;
    /* 0x14 */ be<s16> mMaxAngleZ;
    /* 0x16 */ be<s16> mFlappingAngVelY;
    /* 0x18 */ be<s16> mFlappingAngVelX;
    /* 0x1A */ be<s16> mAngVelStepScale;
    /* 0x1C */ be<s16> mAngVelMaxStep;
    /* 0x1E */ be<s16> mAngVelMinStep;
    /* 0x20 */ be<s16> mFlapDuration;
    /* 0x22 */ be<s16> mFlapExhaustedDuration;
    /* 0x24 */ be<s16> mFlapEnergyDuration;
    /* 0x26 */ u8 _26[2];
    /* 0x28 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_kam_HIO1_l, 0x2C);

/* daNpc_kam_HIO_c (0x50; vtable pointer last) */
struct daNpc_kam_HIO_l {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<u32> prm[6];       /* hio_prm_c (GameCube 0x08..0x1F) */
    /* 0x1C */ be<u32> mpActor;
    /* 0x20 */ daNpc_kam_HIO1_l mHio1;
    /* 0x4C */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_kam_HIO_l, 0x50);
static inline daNpc_kam_HIO_l& kam_l_HIO() { return *gabi::at<daNpc_kam_HIO_l>(KAM_L_HIO); }

/* pointers to member (8-byte constants in .data) */
enum : u32 {
    KAM_PMF_waitNpcAction = 0x1001B408,
    KAM_PMF_waitPlayerAction = 0x1001B410,
    KAM_PMF_damagePlayerAction = 0x1001B418,
};

struct daNpc_kam_c : daPy_npc_l {
    enum ActionStatus {
        ACTION_STARTING = 0,
        ACTION_ONGOING_1 = 1,
        ACTION_ONGOING_2 = 2,
        ACTION_ONGOING_3 = 3,
        ACTION_ENDING = -1,
    };
    enum Animation { ANM_WAIT1 = 0, ANM_WAIT2 = 1, ANM_SING = 2 };

    void offLineHit() { mHitFlags = mHitFlags & ~7u; }
    u32 isFrontLineHit() { return mHitFlags & 1; }
    u32 isLeftLineHit() { return mHitFlags & 2; }
    u32 isRightLineHit() { return mHitFlags & 4; }
    u32 isWaterHit() { return mHitFlags & 8; }
    u32 isNoBgCheck() { return mHitFlags & 0x10; }
    static void onHyoiKamome() { gabi::store<u8>(KAM_M_HYOI_KAMOME, 1); }
    static void offHyoiKamome() { gabi::store<u8>(KAM_M_HYOI_KAMOME, 0); }
    /* daPy_npc_c inlines (m4E4) */
    void returnLink() { m4E4 = m4E4 | 1; }
    void offReturnLink() { m4E4 = m4E4 & ~1u; }
    u32 isReturnLink() { return m4E4 & 1; }
    void onEventAccept() { m4E4 = m4E4 | 2; }
    void offEventAccept() { m4E4 = m4E4 & ~2u; }
    u32 isEventAccept() { return m4E4 & 2; }

    s16 XyCheckCB(int);
    BOOL callDemoStartCheck();
    s16 XyEventCB(int);
    void setAttention(bool); /* HD: the GameCube int parameter is not read */
    void setBaseMtx();
    BOOL createHeap();
    cPhs_State create();
    BOOL init();
    BOOL setAction(ProcFunc_l*, ProcFunc_l*, void*);
    void npcAction(void*);
    void setNpcAction(ProcFunc_l*, void*);
    void playerAction(void*);
    void setPlayerAction(ProcFunc_l*, void*);
    BOOL returnLinkCheck();
    BOOL changeAreaCheck();
    BOOL areaOutCheck();
    BOOL getStickAngY(be<s16>*, be<s16>*);
    s16 getAngleX();
    int wallHitCheck();
    BOOL waitNpcAction(void*);
    BOOL keyProc();
    BOOL waitPlayerAction(void*);
    BOOL damagePlayerAction(void*);
    void eventOrder();
    BOOL checkCommandTalk();
    void returnLinkPlayer();
    BOOL eventProc();
    void eventEnd();
    BOOL actionDefault(int);
    void initialWaitEvent(int);
    BOOL actionWaitEvent(int);
    void initialChangeEvent(int);
    void initialDescendEvent(int);
    BOOL actionDescendEvent(int);
    void initialAreaOutTurn(int);
    BOOL actionAreaOutTurn(int);
    void setAnm(int);
    void setCollision();
    void setLineBgCheck();
    void animationPlay();
    BOOL execute();
    BOOL draw();

    /* 0x608 */ request_of_phase_process_class mPhs;    /* GameCube 0x4EC */
    /* 0x610 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x614 */ dBgS_ObjAcch mAcch;                     /* GameCube 0x4F8 */
    /* 0x7D8 */ dBgS_AcchCir mAcchCir;                  /* HD: one (GameCube mAcchCirs[2], 0x6BC) */
    /* 0x818 */ u8 mLinChk[0x6C];                       /* dBgS_LinChk, GameCube 0x73C */
    /* 0x884 */ dCcD_Stts mStts;                        /* GameCube 0x7A8 */
    /* 0x8C0 */ dCcD_Sph mAtSph;                        /* GameCube 0x7E4; HD: no mTgSph */
    /* 0x9EC */ dCcD_Cps mCps;                          /* GameCube 0xA3C */
    /* 0xB24 */ cXyz mDescendStartPos;                  /* GameCube 0xB74 */
    /* 0xB30 */ cXyz mDescendStartPosUnangled;
    /* 0xB3C */ csXyz mDescendStartAngle;
    /* 0xB42 */ u8 mB92[0xB50 - 0xB42];
    /* 0xB50 */ cBgS_PolyInfo_l mPolyInfo;              /* GameCube 0xBA0 */
    /* 0xB60 */ ProcFunc_l mCurrNpcActionFunc;          /* GameCube 0xBB0 (12 bytes) */
    /* 0xB68 */ ProcFunc_l mCurrPlayerActionFunc;       /* GameCube 0xBBC */
    /* HD: no mShadowId (GameCube 0xBC8) */
    /* 0xB70 */ be<u32> mHitFlags;                      /* GameCube 0xBCC */
    /* 0xB74 */ u8 mBD0[4];
    /* 0xB78 */ be<s32> mAnmIdx;
    /* 0xB7C */ be<f32> mMaxY;
    /* 0xB80 */ be<f32> mMinY;
    /* 0xB84 */ be<f32> mPrevMorfFrame;
    /* 0xB88 */ be<f32> mTargetSpeedF;
    /* 0xB8C */ be<f32> mAccelF;
    /* 0xB90 */ be<s16> mTargetAngleX;
    /* 0xB92 */ be<s16> mLockAngleXTimer;
    /* 0xB94 */ be<s16> mAngVelY;
    /* 0xB96 */ be<s16> mAngVelX;
    /* 0xB98 */ be<s16> mTargetAngVelY;
    /* 0xB9A */ be<s16> mTargetAngVelX;
    /* 0xB9C */ be<s16> m_jnt_body;
    /* 0xB9E */ be<s16> mFlapTimer;
    /* 0xBA0 */ be<s16> mFlapExhaustedTimer;
    /* 0xBA2 */ be<s16> mFlapEnergyTimer;
    /* 0xBA4 */ be<s8> mAnmTblIdx;
    /* 0xBA5 */ be<s8> mReachedAnimEnd;
    /* 0xBA6 */ be<s8> mEventState;
    /* 0xBA7 */ be<s8> mCurrEventIdxIdx;
    /* 0xBA8 */ u8 mC04[1];
    /* 0xBA9 */ be<s8> mActionStatus;
    /* 0xBAA */ be<u8> mUnusedC06;
    /* 0xBAB */ u8 mC07[1];
    /* 0xBAC */ be<s16> mC08;
    /* 0xBAE */ be<s16> mC0A;
    /* 0xBB0 */ be<s16> mC0C;
    /* 0xBB2 */ be<s16> mUnusedC0E;
    /* 0xBB4 */ u8 mC10[4];
    /* 0xBB8 */ be<f32> mUnusedC14;
    /* 0xBBC */ u8 mC18[4];
    /* 0xBC0 */ be<s16> mWaitTimer;                     /* GameCube 0xC1C */
    /* 0xBC2 */ be<s16> mAreaOutTimer;
    /* 0xBC4 */ be<s16> mEventIdxs[3];
    /* 0xBCA */ be<u8> mHidden;                         /* HD-only (GameCube padding 0xC26) */
    /* 0xBCB */ u8 _BCB[1];
};
WWHD_OFFSET(daNpc_kam_c, mPhs, 0x608);
WWHD_OFFSET(daNpc_kam_c, mAcch, 0x614);
WWHD_OFFSET(daNpc_kam_c, mLinChk, 0x818);
WWHD_OFFSET(daNpc_kam_c, mAtSph, 0x8C0);
WWHD_OFFSET(daNpc_kam_c, mCps, 0x9EC);
WWHD_OFFSET(daNpc_kam_c, mDescendStartPos, 0xB24);
WWHD_OFFSET(daNpc_kam_c, mPolyInfo, 0xB50);
WWHD_OFFSET(daNpc_kam_c, mHitFlags, 0xB70);
WWHD_OFFSET(daNpc_kam_c, mActionStatus, 0xBA9);
WWHD_OFFSET(daNpc_kam_c, mEventIdxs, 0xBC4);
WWHD_SIZE(daNpc_kam_c, 0xBCC);

/* ---- this TU's statics (HD addresses) ---- */
#define KAM_L_MS_AT_LOCAL_VEC 0x104673E4   /* cXyz l_ms_at_local_vec (0, 0, -1) */
#define KAM_L_MS_AT_LOCAL_START 0x104673F0 /* cXyz l_ms_at_local_start (100, 20, 0) */
#define KAM_L_MS_AT_LOCAL_END 0x104673FC   /* cXyz l_ms_at_local_end (-100, 20, 0) */
#define KAM_L_LINE_BG_LOCAL_END 0x10467408 /* cXyz l_line_bg_local_end (0, 0, 500) */
#define KAM_L_CALL_LOCAL_KYORI 0x10467414  /* cXyz l_call_local_kyori (0, 0, 500) */
#define KAM_L_OFFSET 0x10467420            /* setBaseMtx: static cXyz l_offset, guard below */
#define KAM_GUARD_OFFSET 0x10467498
#define KAM_L_OFFSET_ATT_POS 0x1046742C    /* headNodeCallBack: static cXyz l_offsetAttPos */
#define KAM_GUARD_OFFSET_ATT_POS 0x10467494
#define KAM_L_CENTER 0x10467438            /* changeAreaCheck: static cXyz l_center */
#define KAM_GUARD_CENTER 0x104674BC
#define KAM_L_SPLASH_SCALE 0x104674A4      /* execute: static splash_scale (0.6), guard below */
#define KAM_GUARD_SPLASH_SCALE 0x1046749C
#define KAM_L_RIPPLE_SCALE 0x104674B0      /* execute: static ripple_scale (1.0), guard below */
#define KAM_GUARD_RIPPLE_SCALE 0x104674A0
#define KAM_L_SPH_SRC 0x101BEC54           /* dCcD_SrcSph l_sph_src */
#define KAM_EVENT_INIT_TBL 0x101BEC94      /* event_init_tbl (pointers to member) */
#define KAM_EVENT_ACTION_TBL 0x101BECB4    /* event_action_tbl */
#define KAM_CUT_NAME_TBL 0x101BECD4        /* cut_name_tbl */
#define KAM_EVENT_NAME_TBL 0x101BED04      /* event_name_tbl */
#define KAM_L_KAM_AT_CPS_SRC 0x101BED10    /* dCcD_SrcCps l_kam_at_cps_src */
#define KAM_L_ANM_TBL 0x101BED5C           /* setAnm: l_anmTbl {s8 bck, s8 bas} */
#define KAM_L_ANM_PRM 0x101BED64           /* setAnm: l_anmPrm (0x14 each) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void kam_copyWords(u32 dst, u32 src, int n) {
    for (int i = 0; i < n; i++) gabi::store<u32>(dst + 4 * i, gabi::load<u32>(src + 4 * i));
}
static inline void kam_setVec(u32 a, f32 x, f32 y, f32 z) {
    gabi::store<f32>(a, x);
    gabi::store<f32>(a + 4, y);
    gabi::store<f32>(a + 8, z);
}
/* eventInfo.mCommand (actor + 0xF8) */
static inline u16 kam_eventCommand(fopAc_ac_c* a) { return gabi::load<u16>(gabi::ea(a) + 0xF8); }
/* mDoAud_zelAudio_c::getInterface()->field_0x0062 (HD: the interface pointer at 0x101FFC78, +0x72) */
static inline void kam_setAudioFlag(u8 v) { gabi::store<u8>(gabi::load<u32>(0x101FFC78) + 0x72, v); }
/* 023D4688 daPy_py_c::changePlayer(fopAc_ac_c*) */
static inline void daPy_py_changePlayer(fopAc_ac_c* self, fopAc_ac_c* a) { gabi::call(0x023D4688, self, a); }
/* 025445B8 / 0254457C dEvent_manager_c::startCheckOld / endCheckOld(const char*) */
static inline BOOL dEvmng_startCheckOld(const char* name) { return gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), name); }
static inline BOOL dEvmng_endCheckOld(const char* name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
static inline void mDoMtx_ZXYrotS_kam(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F1AA4, m, x, y, z); }
/* 02055B64 cLib_calcTimer<s16> (out of line; the whole r3 is tested) */
static inline s32 kam_calcTimer(be<s16>* t) { return gabi::call<s32>(0x02055B64, t); }
/* 0207A9A0 cLib_calcTimer<u8> */
static inline s32 kam_calcTimerU8(be<u8>* t) { return gabi::call<s32>(0x0207A9A0, t); }
/* 021E1E78 cLib_getRndValue<int>(base, range) (another TU's copy) */
static inline s32 kam_getRndValue(s32 base, s32 range) { return gabi::call<s32>(0x021E1E78, base, range); }
/* 0200796C / 02007990 CPad_GET_STICK_POS_X / _Y(port) (HD: out of line) */
static inline f32 CPad_GET_STICK_POS_X_kam() { return gabi::call<f32>(0x0200796C, 0); }
static inline f32 CPad_GET_STICK_POS_Y_kam() { return gabi::call<f32>(0x02007990, 0); }
/* HD J3D: a model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices);
 * getAnmMtx marks them dirty */
static inline Mtx34* kam_getAnmMtx(J3DModel* model, u32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}
static inline u32 kam_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s32 JUTNameTab_getIndex_kam(u32 tab, u32 name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
/* HD: getJointNodePointer(idx) is bounds-checked (count +4, array +8, 0x1C each); setCallBack at +8 */
static inline void kam_setJointCallBack(J3DModelData* d, u16 idx, u32 cb) {
    u32 count = gabi::load<u32>(gabi::ea(d) + 4);
    u32 p = gabi::load<u32>(gabi::ea(d) + 8);
    if (idx < count) p += idx * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
/* (a - b).absXZ() */
static inline f32 kam_absXZ_delta(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d, b);
    gabi::Local<cXyz> xz;
    xz->x = d->x;
    xz->y = 0.0f;
    xz->z = d->z;
    return std_sqrtf(PSVECSquareMag(xz));
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0, HD: sead::SafeString operator== */
static inline bool kam_isStartStage(u32 lit) {
    gabi::Local<SafeString> sa;
    sa->mStringTop = lit;
    sa->__vtbl = KAM_SAFESTRING_VTBL;
    u32 stage = dComIfGp_ea() + 0x5134;
    gabi::Local<SafeString> sb;
    sb->mStringTop = stage;
    sb->__vtbl = KAM_SAFESTRING_VTBL;
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    u32 pa = sa->mStringTop;
    gabi::call_ptr(gabi::load<u32>(sb->__vtbl + 0x14), sb.get());
    u32 pb = sb->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        if (ca != gabi::load<u8>(pb + i)) return false;
        if (ca == 0) return true;
    }
    return false;
}
