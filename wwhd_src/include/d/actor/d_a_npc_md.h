/* daNpc_Md_c (Medli), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_md (profile 0x101C1F10: size 0x43DC;
 * GameCube 0x32C0; the constructor is inlined in create 02286084):
 * - daPy_py_c: GameCube fields 0x290..0x31C at +0x11C (mDemo at 0x420); the GameCube vtable
 *   slot 0x31C is merged into the HD vtable at 0xB4 (0x438 is padding).
 * - daPy_npc_c (constructor 024450B4 allocates 0x608): mAcch 0x43C, mNpcFlags 0x600.
 * - members from mPhase (GameCube 0x4EC) on are +0x11C up to the btp animation;
 *   mDoExt_btpAnm / mDoExt_btkAnm grew from 0x14 to 0x74 (+0x60 each), and mShadowId
 *   (GameCube 0x548) is gone (HD shadows): +0x1D8 from mAcchCir to mJntCtrl;
 * - dDlst_mirrorPacket grew from 0x24DC to 0x3428: +0x1124 from mMirrorShieldCb to mBtpFrame;
 * - the two pointers to member (GHS: 8 bytes, GameCube 12): +0x111C from mMsgNo to the end.
 * Field names are the GameCube ones (mNNNN = GameCube offset). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l (GHS pointer to member, 8 bytes) */

#define MD_VTBL 0x1001E088          /* daNpc_Md_c vtable (create 02286084) */
#define MD_L_HIO 0x10467A90         /* l_HIO (daNpc_Md_HIO_c, 0x1CC) */

/* statics of daNpc_Md_c (HD addresses) */
#define MD_M_FLYING 0x101D5F3E      /* bool daNpc_Md_c::m_flying (setNpcAction) */
#define MD_M_PLAYERROOM 0x101D5F41  /* bool daNpc_Md_c::m_playerRoom (checkPlayerRoom) */

/* pointers to member (8-byte constants in .data: {s16 d = 0, s16 i = -1, u32 f}) */
enum : u32 {
    PMF_mwaitNpcAction = 0x1001D488,
    PMF_waitNpcAction = 0x1001D490,
    PMF_waitPlayerAction = 0x1001D498,
    PMF_mkamaePlayerAction = 0x1001D4A0,
    PMF_flyPlayerAction = 0x1001D4A8,
    PMF_shipNpcAction = 0x1001D4B0,
    PMF_jumpPlayerAction = 0x1001D4B8,
    PMF_fallNpcAction = 0x1001D4C0,
    PMF_fall02NpcAction = 0x1001D4C8,
    PMF_searchNpcAction = 0x1001D4D0,
    PMF_hitPlayerAction = 0x1001D4D8,
    PMF_hitNpcAction = 0x1001D4E0,
    PMF_carryNpcAction = 0x1001D4E8,
    PMF_demoFlyNpcAction = 0x1001D4F0,
    PMF_squatdownNpcAction = 0x1001D4F8,
    PMF_sqwait01NpcAction = 0x1001D500,
    PMF_throwNpcAction = 0x1001D508,
    PMF_02291E50 = 0x1001D510, /* land02/land03NpcAction (unnamed 02291E50) */
    PMF_land01NpcAction = 0x1001D518,
    PMF_wallHitNpcAction = 0x1001D520,
    PMF_glidingNpcAction = 0x1001D528,
    PMF_02291D50 = 0x1001D530, /* land02/land03NpcAction (unnamed 02291D50) */
    PMF_piyo2NpcAction = 0x1001D538,
    PMF_jumpNpcAction = 0x1001D540,
    PMF_0229310C = 0x1001D548, /* unnamed 0229310C */
    PMF_kyohiNpcAction = 0x1001D550,
    PMF_walkPlayerAction = 0x1001D558,
    PMF_landPlayerAction = 0x1001D560,
    PMF_deleteNpcAction = 0x1001D568,
    PMF_talkNpcAction = 0x1001D570,
    PMF_shipTalkNpcAction = 0x1001D578,
    PMF_harpWaitNpcAction = 0x1001D580,
    PMF_carryPlayerAction = 0x1001D588,
    PMF_escapeNpcAction = 0x1001D590,
};

static inline void md_pmf_load(ProcFunc_l* dst, u32 src) {
    gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(src));
    gabi::store<u32>(gabi::ea(dst) + 4, gabi::load<u32>(src + 4));
}
/* (this->*pmf)(arg) as GHS calls it (i < 0: plain function, else virtual) */
template <class R = BOOL, class A>
static inline R md_pmf_call(void* self, ProcFunc_l* pmf, A arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        return gabi::call_ptr<R>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        return gabi::call_ptr<R>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}

WWHD_OPAQUE(mDoExt_McaMorf2);

/* daPy_demo_c (0x18, unchanged) */
struct daPy_demo_l {
    /* 0x00 */ be<u16> mDemoType;
    /* 0x02 */ be<s16> mDemoMoveAngle;
    /* 0x04 */ be<s16> mTimer;
    /* 0x06 */ be<s16> mParam2;
    /* 0x08 */ be<s32> mParam0;
    /* 0x0C */ be<s32> mParam1;
    /* 0x10 */ be<u32> mDemoMode;
    /* 0x14 */ be<f32> mStick;
};
WWHD_SIZE(daPy_demo_l, 0x18);

/* daPy_py_c (HD: GameCube fields +0x11C, vtable merged into 0xB4) */
struct daPy_py_l : fopAc_ac_c {
    /* 0x3AC */ be<u8> mCutType;
    /* 0x3AD */ be<u8> mCutCount;
    /* 0x3AE */ u8 _3AE[2];
    /* 0x3B0 */ be<s16> mDamageWaitTimer;
    /* 0x3B2 */ be<s16> mQuakeTimer;
    /* 0x3B4 */ be<s32> mFace;
    /* 0x3B8 */ be<u32> mNoResetFlg0;
    /* 0x3BC */ be<u32> mNoResetFlg1;
    /* 0x3C0 */ be<u32> mResetFlg0;
    /* 0x3C4 */ be<f32> mMaxNormalSpeed;
    /* 0x3C8 */ be<f32> mHeight;
    /* 0x3CC */ be<f32> mGrabItemOffsetY;  /* was field_0x2b0: view of daPy_py_c::mGrabItemOffsetY (d_a_player.h) */
    /* 0x3D0 */ csXyz mBodyAngle;
    /* 0x3D6 */ u8 _3D6[2];
    /* 0x3D8 */ cXyz mHeadTopPos;
    /* 0x3E4 */ cXyz mSwordTopPos;
    /* 0x3F0 */ cXyz mLeftHandPos;
    /* 0x3FC */ cXyz mRightHandPos;
    /* 0x408 */ cXyz mRopePos;
    /* 0x414 */ cXyz mWindVelocity;  /* was field_0x2f8: 0x414: this decomp's daPy_py_c member at the same offset is mWindVelocity (d_a_player.h, GameCube mWindVelocity; cLib_addCalc in d_a_player.cpp 188); daPy_py_l is a view local to d_a_npc_md.h; no md use */
    /* 0x420 */ daPy_demo_l mDemo;
    /* 0x438 */ u8 _438[4];
};
WWHD_SIZE(daPy_py_l, 0x43C);

/* daPy_npc_c (HD 0x608, constructor 024450B4) */
struct daPy_npc_l : daPy_py_l {
    /* 0x43C */ dBgS_ObjAcch mAcch;
    /* 0x600 */ be<u32> mNpcFlags;  /* was m4E4: view of daPy_npc_c::mNpcFlags (d_a_player_npc.h) */
    /* 0x604 */ be<u8> mFallTimer;  /* was m4E8: view of daPy_npc_c::mFallTimer (d_a_player_npc.h) */
    /* 0x605 */ be<u8> mDamageFogTimer;
    /* 0x606 */ u8 _606[2];
};
WWHD_SIZE(daPy_npc_l, 0x608);

/* daPy_mtxFollowEcallBack_c (0xC: vtable, emitter, matrix) */
struct daPy_mtxFollowEcallBack_l {
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x8 */ be<u32> mpMtx;
};
WWHD_SIZE(daPy_mtxFollowEcallBack_l, 0xC);

/* daNpc_Md_followEcallBack_c (0x1C, vtable 0x1001E048 by create) */
struct daNpc_Md_followEcallBack_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ cXyz mPos;
    /* 0x14 */ csXyz mAngle;
    /* 0x1A */ u8 _1A[2];
    void end();
};
WWHD_SIZE(daNpc_Md_followEcallBack_c, 0x1C);

/* dPa_rippleEcallBack (0x14: vtable at +0, emitter at +4, rate at +0x10) */
struct dPa_rippleEcallBack_md {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<f32> mRate;
};
WWHD_SIZE(dPa_rippleEcallBack_md, 0x14);

/* cBgS_PolyInfo (0x10: s16 poly index, s16 bg index, u32 bgw, s32 id?, vtable at +0xC) */
struct cBgS_PolyInfo_l {
    /* 0x0 */ be<u16> mPolyIndex;
    /* 0x2 */ be<u16> mBgIndex;
    /* 0x4 */ be<u32> mpBgW;
    /* 0x8 */ be<s32> mProcId;
    /* 0xC */ be<u32> __vtbl;
};
WWHD_SIZE(cBgS_PolyInfo_l, 0x10);

struct daNpc_Md_c : daPy_npc_l {
    enum ActionStatus {
        ACTION_STARTING = 0,
        ACTION_ONGOING_1 = 1,
        ACTION_ONGOING_2 = 2,
        ACTION_ONGOING_3 = 3,
        ACTION_ENDING = -1,
    };
    enum daNpc_Md_StatusBit_e {
        daMdStts_UNK1 = 0x00000001,
        daMdStts_UNK2 = 0x00000002,
        daMdStts_UNK4 = 0x00000004,
        daMdStts_UNK8 = 0x00000008,
        daMdStts_FLY = 0x00000010,
        daMdStts_CAM_TAG_IN = 0x00000020,
        daMdStts_UNK40 = 0x00000040,
        daMdStts_UNK80 = 0x00000080,
        daMdStts_XY_TALK = 0x00000100,
        daMdStts_UNK200 = 0x00000200,
        daMdStts_UNK400 = 0x00000400,
        daMdStts_CARRY_ACTION = 0x00000800,
        daMdStts_SHIP_RIDE = 0x00001000,
        daMdStts_LIGHT_HIT = 0x00002000,
        daMdStts_UNK4000 = 0x00004000,
        daMdStts_LIGHT_BODY_HIT = 0x00008000,
        daMdStts_DEFAULT_TALK_XY = 0x00010000,
        daMdStts_UNK20000 = 0x00020000,
    };

    /* inline accessors (GameCube header) */
    void setBitStatus(u32 s) { mStatus = mStatus | s; }
    void clearStatus(u32 s) { mStatus = mStatus & ~s; }
    BOOL checkStatus(u32 s) { return (mStatus & s) != 0; }
    BOOL isTypeShipRide() { return mType == 7; }

    /* methods (address in d_a_npc_md_pending.cpp) */
    s16 XyCheckCB(int);
    s16 XyEventCB(int);
    cPhs_State create();
    BOOL createHeap();
    BOOL setAction(ProcFunc_l*, ProcFunc_l*, void*);
    void npcAction(void*);
    void setNpcAction(ProcFunc_l*, void*);
    void playerAction(void*);
    void setPlayerAction(ProcFunc_l*, void*);
    s16 getStickAngY(int);
    int calcStickPos(s16, cXyz*);
    BOOL flyCheck();
    BOOL mirrorCancelCheck();
    void setWingEmitter();
    void setHane02Emitter();
    void deleteHane02Emitter();
    void setHane03Emitter();
    void deleteHane03Emitter();
    void returnLinkPlayer();
    BOOL shipRideCheck();
    BOOL isFallAction();
    BOOL returnLinkCheck();
    BOOL lightHitCheck();
    int wallHitCheck();
    void NpcCall(be<s32>*);
    BOOL checkCollision(int);
    void restartPoint(s16);
    void setMessageAnimation(u8);
    void waitGroundCheck();
    BOOL chkAdanmaeDemoOrder();
    BOOL waitNpcAction(void*);
    BOOL harpWaitNpcAction(void*);
    BOOL XYTalkCheck();
    BOOL talkNpcAction(void*);
    BOOL shipTalkNpcAction(void*);
    BOOL kyohiNpcAction(void*);
    BOOL shipNpcAction(void*);
    BOOL mwaitNpcAction(void*);
    BOOL squatdownNpcAction(void*);
    BOOL sqwait01NpcAction(void*);
    void changeCaught02();
    BOOL carryNpcAction(void*);
    BOOL throwNpcAction(void*);
    BOOL glidingNpcAction(void*);
    s16 windProc();
    BOOL fallNpcAction(void*);
    BOOL fall02NpcAction(void*);
    BOOL wallHitNpcAction(void*);
    BOOL land01NpcAction(void*);
    BOOL land02NpcAction(void*);
    BOOL land03NpcAction(void*);
    BOOL piyo2NpcAction(void*);
    BOOL deleteNpcAction(void*);
    BOOL demoFlyNpcAction(void*);
    void routeAngCheck(cXyz*, be<s16>*);
    void routeWallCheck(cXyz*, cXyz*, be<s16>*);
    f32 checkForwardGroundY(s16);
    f32 checkWallJump(s16);
    BOOL routeCheck(f32, be<s16>*);
    BOOL searchNpcAction(void*);
    BOOL npcAction_0229310C(void*); /* unnamed action (HD order: after searchNpcAction) */
    BOOL hitNpcAction(void*);
    void setNormalSpeedF(f32, f32, f32, f32, f32);
    void setSpeedAndAngleNormal(f32, s16);
    void walkProc(f32, s16);
    BOOL jumpNpcAction(void*);
    BOOL escapeNpcAction(void*);
    BOOL waitPlayerAction(void*);
    BOOL walkPlayerAction(void*);
    BOOL hitPlayerAction(void*);
    BOOL jumpPlayerAction(void*);
    BOOL flyPlayerAction(void*);
    BOOL landPlayerAction(void*);
    BOOL mkamaePlayerAction(void*);
    BOOL carryPlayerAction(void*);
    BOOL eventProc();
    void initialDefault(int);
    BOOL actionDefault(int);
    void initialWaitEvent(int);
    BOOL actionWaitEvent(int);
    void initialLetterEvent(int);
    void initialMsgSetEvent(int);
    BOOL actionMsgSetEvent(int);
    BOOL actionMsgEndEvent(int);
    void initialMovePosEvent(int);
    void initialFlyEvent(int);
    BOOL actionFlyEvent(int);
    void initialGlidingEvent(int);
    BOOL actionGlidingEvent(int);
    void initialLandingEvent(int);
    BOOL actionLandingEvent(int);
    void initialWalkEvent(int);
    BOOL actionWalkEvent(int);
    BOOL actionDashEvent(int);
    void initialEndEvent(int);
    BOOL actionTactEvent(int);
    void initialTakeOffEvent(int);
    BOOL actionTakeOffEvent(int);
    void initialOnetimeEvent(int);
    BOOL actionOnetimeEvent(int);
    void initialQuake(int);
    void setHarpPlayNum(int);
    void initialHarpPlayEvent(int);
    BOOL actionHarpPlayEvent(int);
    void initialOffLinkEvent(int);
    void initialOnLinkEvent(int);
    void initialTurnEvent(int);
    BOOL actionTurnEvent(int);
    void initialSetAnmEvent(int);
    u32 initialLookDown(int);
    u32 initialLookUp(int);
    BOOL actionLookDown(int);
    BOOL talk_init();
    BOOL talk(int);
    int getAnmType(u8);
    BOOL initTexPatternAnm(u8, u32); /* HD: the full register of the bool reaches init */
    void playTexPatternAnm();
    BOOL initLightBtkAnm(u32); /* HD: the full register of the bool reaches init */
    u32 playLightBtkAnm();
    BOOL setAnm(int);
    bool dNpc_Md_setAnm(mDoExt_McaMorf2*, f32, int, f32, f32, const char*, const char*, const char*);
    bool dNpc_Md_setAnm(mDoExt_McaMorf*, int, f32, f32, const char*, const char*);
    bool chkAttention(cXyz*, s16, int); /* cXyz by value: a pointer to a caller copy */
    bool chkArea(cXyz*);
    void carryCheck();
    void eventOrder();
    void checkOrder();
    BOOL checkCommandTalk();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void setCollision();
    void setAttention(bool);
    void lookBack(int, int, int);
    void lookBack(cXyz*, int, int);
    s32 lookBackWaist(s16, f32);
    void setBaseMtx();
    void deletePiyoPiyo();
    BOOL init();
    BOOL draw();
    void animationPlay();
    void checkPlayerRoom();
    BOOL execute();
    void particle_set(be<u32>* /* JPABaseEmitter** */, u16);
    void emitterTrace(JPABaseEmitter*, Mtx34*, csXyz*);
    void emitterDelete(be<u32>* /* JPABaseEmitter** */);
    BOOL isTagCheckOK(); /* virtual (0228D950) */

    /* 0x0608 */ request_of_phase_process_class mPhase;   /* GameCube 0x4EC */
    /* 0x0610 */ gptr<J3DModel> mpHarpModel;
    /* 0x0614 */ gptr<J3DModel> mpHarpLightModel;
    /* 0x0618 */ gptr<mDoExt_McaMorf2> mpMorf;
    /* 0x061C */ gptr<mDoExt_McaMorf2> mpArmMorf;
    /* 0x0620 */ gptr<mDoExt_McaMorf> mpWingMorf;
    /* 0x0624 */ be<u32> mEmitters[6];  /* was m0508: setWingEmitter 0228C1B4 / setHane02Emitter 0228E2D4 set ID_IT_SN_MD_HANE* emitters in it; demoFlyNpcAction 022921B4 / flyPlayerAction 02294160 delete them (emitterDelete); JPABaseEmitter* */
    /* 0x063C */ u8 mBtpAnm[0x74];  /* was m0520: initTexPatternAnm 022846EC / playTexPatternAnm 0228729C init and play it as the eye texture pattern; draw 02289E2C enters it (btpAnm_entry); mDoExt_btpAnm (HD 0x74), GameCube 0x520 */
    /* 0x06B0 */ mDoExt_btkAnm mLightBtkAnm;               /* HD 0x74, GameCube 0x534 */
    /* HD: no mShadowId (GameCube 0x548) */
    /* 0x0724 */ dBgS_AcchCir mAcchCir[2];                 /* GameCube 0x54C */
    /* 0x07A4 */ u8 mLinChk[0x6C];                         /* dBgS_MirLightLinChk, GameCube 0x5CC */
    /* 0x0810 */ dCcD_Stts mStts;                          /* GameCube 0x638 */
    /* 0x084C */ dCcD_Cyl mCyl1;                           /* GameCube 0x674 */
    /* 0x097C */ dCcD_Cyl mCyl2;
    /* 0x0AAC */ dCcD_Cyl mCyl3;
    /* 0x0BDC */ dCcD_Cps mCps;                            /* GameCube 0xA04 */
    /* 0x0D14 */ dNpc_JntCtrl_c mJntCtrl;                  /* GameCube 0xB3C */
    /* 0x0D48 */ u8 mMirrorPacket[0x3428];  /* was m0B70: draw 02289E2C updates it (mirrorPacket_update) and enters it in the XLU list; daNpc_Md_dt 0228A254 destroys it; dDlst_mirrorPacket (HD 0x3428, GameCube 0x24DC) */
    /* 0x4170 */ daPy_mtxFollowEcallBack_l mMirrorShieldCb;  /* was m304C: lightHitCheck 02287754 makes its ID_AK_SN_MIRRORSHIELD00 emitter on the harp light model's matrix; daNpc_Md_dt 0228A254 ends it; GameCube 0x304C */
    /* 0x417C */ daNpc_Md_followEcallBack_c mHitShieldLightCb;  /* was m3058: 0x417C: lightHitCheck 02287754 sets the ID_AK_SN_HITSHIELDLIGHT00 (0x8232) emitter with it while the harp light hits, places it at the light's hit point on mCps (mPos) and the light direction (mAngle), else end(); create 02286084 sets its vtable; daNpc_Md_dt 0228A254 ends it (tww the same) */
    /* 0x4198 */ dPa_rippleEcallBack_md mRippleCb;  /* was m3074: execute 02288B08 sets the water ripple emitter with it when it has none; daNpc_Md_dt 0228A254 ends it */
    /* 0x41AC */ cXyz mEyePos;  /* was m3088: nodeCallBack 02283614: the head matrix times l_offsetEyePos; setAttention 02285984 copies it to eyePos; TWW getEyePos(); GameCube 0x3088 */
    /* 0x41B8 */ cXyz mAttentionBasePos;  /* was m3094: nodeCallBack 02283614: the head matrix times l_offsetAttPos; init 02285580 copies current.pos; TWW getAttentionBasePos() */
    /* 0x41C4 */ cXyz mThrowPos;  /* was m30A0: throwNpcAction 02291094 copies current.pos; glidingNpcAction 02291304 subtracts it from current.pos */
    /* 0x41D0 */ cXyz mLookDownOffset;  /* was m30AC: 0x41D0: init 02285580 sets (0, 0, 50); actionLookDown 0228D848 sets y = -50, rotates it by shape_angle.y and adds it to (pos.x, eyePos.y, pos.z) as the look-at target (tww the same) */
    /* 0x41DC */ cXyz mTurnTargetPos;  /* was m30B8: initialTurnEvent 0228D150 sets the event's position; actionTurnEvent 0228D32C turns to it (lookBack) */
    /* 0x41E8 */ cXyz mMoveBlockSpeed;  /* was m30C4: 0x41E8: execute 02288B08: chkMoveBlock's out vector (daPy_npc_c::chkMoveBlock: block current.pos - old.pos); escapeNpcAction 02293500 turns by cM_atan2s(it.x, it.z) +/- 0x4000 (tww the same); same role as daNpc_Cb1_c::mMoveBlockSpeed */
    /* 0x41F4 */ be<f32> mBtpFrame;  /* was m30D0: setAnm 02286D7C / initialSetAnmEvent 0228D564 copy the body morf frame; playTexPatternAnm 0228729C sets the eye texture pattern frame to it */
    /* 0x41F8 */ ProcFunc_l mCurrPlayerActionFunc;         /* GameCube 0x30D4 (12 bytes) */
    /* 0x4200 */ ProcFunc_l mCurrNpcActionFunc;            /* GameCube 0x30E0 */
    /* 0x4208 */ be<u32> mMsgNo;                           /* GameCube 0x30EC */
    /* 0x420C */ be<u32> mStatus;  /* was m30F0: init 02285580 sets 0 / daMdStts_UNK8; setBaseMtx 02285A6C tests daMdStts bits; TWW setStatus/checkStatus */
    /* 0x4210 */ be<u32> mHarpSe;  /* was m30F4: 0x4210: animationPlay 02287364: the SEA type's harp note SE (random from l_harp_play_se) played on the harp frames, kept for mHarpSeCount repeats; init 02285580 JA_SE_CM_MD_HARP_CN4 (tww the same) */
    /* 0x4214 */ be<f32> mCylH;  /* was m30F8: 0x4214: setCollision 02287A74 passes it to mCyl1 / mCyl2 / mCyl3 SetH; init 02285580 and the stand actions set 120, the squat / carry / land actions 75 (tww the same) */
    /* 0x4218 */ be<f32> mRunRate;
    /* 0x421C */ be<s32> mSwitchNo;  /* was m3100: init 02285580: (param >> 16) & 0xFF; waitNpcAction 0228EB88 tests dComIfGs_isSwitch(it) */
    /* 0x4220 */ be<s32> mAnmPrmIdx;  /* was m3104: 0x4220: setAnm 02286D7C stores its argument and indexes l_anmPrm with it (mAnmTblIdx is prm->anmTblIdx); waitNpcAction 0228EB88, harpWaitNpcAction 0228F424, animationPlay 02287364 and 9 more test it for animation numbers (tww the same); after daNpc_Cb1_c::mAnmPrmIdx */
    /* 0x4224 */ be<f32> mNormalSpeed;  /* was m3108: 0x4224: setNormalSpeedF 0228E37C eases it toward the target speed (cLib_addCalc / step); walkProc 0228E5CC sets speedF = it and the run rate |it / mMaxNormalSpeed| (tww the same) */
    /* 0x4228 */ be<f32> mStickValue;  /* was m310C: mkamaePlayerAction 02294984 sets md_stickValue(0); waitPlayerAction 02293840 clears it; lookBackWaist 0228DFF8 reads it */
    /* 0x422C */ be<s16> mLookMaxVel;  /* was m3110: demoFlyNpcAction 022921B4 eases it to HIO mMaxHeadTurnVel, else clears it; passed to dNpc_JntCtrl lookAtTarget as maxVel */
    /* 0x422E */ be<s16> mBtpFrameMax;  /* was m3112: initTexPatternAnm 022846EC: J3DAnm_getFrameMax of the eye texture pattern; playTexPatternAnm 0228729C restarts it with this length */
    /* 0x4230 */ be<s16> mWaistRotX;  /* was m3114: lookBackWaist 0228DFF8 eases it; waistNodeCallBack 022837D0 rotates the waist by it; TWW getWaistRotX() */
    /* 0x4232 */ be<s16> mWaistRotY;  /* was m3116: lookBackWaist 0228DFF8 eases it toward HIO m22 * stick; waistNodeCallBack 022837D0 rotates the waist by it; TWW getWaistRotY() */
    /* 0x4234 */ be<s16> mEventTimer;  /* was m3118: initialFlyEvent 0228C1C4 sets 60 * 30, initialWalkEvent 0228C4A4 20 * 30; actionFlyEvent 0228C234 ends the event when it runs out */
    /* 0x4236 */ be<s16> mStickAngY;  /* was m311A: mkamaePlayerAction 02294984 sets getStickAngY(FALSE); waitPlayerAction 02293840 clears it */
    /* 0x4238 */ be<s8> m_backbone1_jnt_num;
    /* 0x4239 */ be<s8> m_backbone2_jnt_num;
    /* 0x423A */ be<s8> m_armR_jnt_num;
    /* 0x423B */ be<s8> m_armL_jnt_num;
    /* 0x423C */ be<s8> m_armRloc_jnt_num;
    /* 0x423D */ be<s8> m_armLloc_jnt_num;
    /* 0x423E */ be<s8> m_wingRloc_jnt_num;
    /* 0x423F */ be<s8> m_wingLloc_jnt_num;
    /* 0x4240 */ be<s8> m_neck_jnt_num;
    /* 0x4241 */ be<s8> m_wingR2_jnt_num;
    /* 0x4242 */ be<s8> m_wingL2_jnt_num;
    /* 0x4243 */ be<s8> m_wingR3_jnt_num;
    /* 0x4244 */ be<s8> m_wingL3_jnt_num;
    /* 0x4245 */ be<s8> m_handL_jnt_num;
    /* 0x4246 */ be<s8> mAnmEnd;  /* was m312A: 0x4246: animationPlay 02287364 = mpMorf play() result, also 1 when the frame wraps; setAnm 02286D7C clears it; about 25 actions / events step on when it is set (tww the same); unit specs call it the animation end flag */
    /* 0x4247 */ be<u8> mAttnSetCount;  /* was m312B: setAttention 02285984 / nodeCallBack 02283614 add 1 up to 0xFF; execute 02288B08 clears it; TWW incAttnSetCount() */
    /* 0x4248 */ be<u8> mHasAttention;  /* was m312C: 0x4248: waitNpcAction 0228EB88 / talkNpcAction 0228F610 / shipNpcAction 0228FD20 and 4 more store chkAttention's result; chkAttention 0228DAF4 widens the check by +40 distance / +0x71C angle while set (tww the same); same role as daNpc_Cb1_c::mHasAttention */
    /* 0x4249 */ be<s8> mAnmTblIdx;  /* was m312D: setAnm 02286D7C stores the animation table index (and skips when it and the speed are unchanged); getMsg 0228DDEC tests == 0x10 */
    /* 0x424A */ be<s8> mCurEventMode;
    /* 0x424B */ be<u8> mSearchSide;  /* was m312F: 0x424B: searchNpcAction 02292C48 flips it (^= 1) when mSearchTimer runs out (tww the same); no read in WWHD or tww; same code as daNpc_Cb1_c::mSearchSide */
    /* 0x424C */ be<u8> mSearchTimer;  /* was m3130: 0x424C: searchNpcAction 02292C48 counts it down (cLib_calcTimer) and reloads it with cLib_getRndValue(8, 20) at 0, flipping mSearchSide (tww the same); same role as daNpc_Cb1_c::mSearchTimer */
    /* 0x424D */ be<u8> mGroundHit;  /* was m3131: 0x424D: execute 02288B08: mAcch.ChkGroundHit(); while set the moving-BG correction applies, the landing effect plays only when it was 0; routeCheck 022928A8 / initialMovePosEvent 0228B68C 1, carryNpcAction 022904DC 0 (tww the same); same role as daNpc_Cb1_c::mGroundHit */
    /* 0x424E */ be<s8> mActionStatus;
    /* 0x424F */ be<u8> mBlinkTimer;  /* was m3133: initTexPatternAnm 022846EC clears it; playTexPatternAnm 0228729C counts it down and restarts the eye pattern with 30 + rnd(60) */
    /* 0x4250 */ be<u8> mHairMode;  /* was m3134: hairNodeCallBack 02283A78 tests and sets bits; TWW checkBitHairMode / setBitHairMode */
    /* 0x4251 */ be<u8> mEffectStatus;  /* was m3135: flyPlayerAction 02294160 / landPlayerAction 02294838 set bit 1; execute 02288B08 tests it; TWW setBitEffectStatus / checkBitEffectStatus */
    /* 0x4252 */ be<u8> mBtpSyncBody;  /* was m3136: 0x4252: initTexPatternAnm 022846EC stores the btp table's flag (tww btpAnmTbl m20); playTexPatternAnm 0228729C: while 1 the eye pattern frame is set to mBtpFrame (body frame) instead of blinking (tww the same) */
    /* 0x4253 */ be<u8> mBtpAnmTblIdx;  /* was m3137: initTexPatternAnm 022846EC stores the texture pattern table index; setAnm 02286D7C re-inits the pattern when it differs */
    /* 0x4254 */ be<u8> mType;
    /* 0x4255 */ be<s8> mCurEvent;
    /* 0x4256 */ be<u8> m313A;
    /* 0x4257 */ u8 m313B;
    /* 0x4258 */ be<u8> mPiyo2TalkCnt;  /* was m313C: getMsg 0228DDEC reads and clears it; init 02285580 clears it; TWW getPiyo2TalkCNT / setPiyo2TalkCNT */
    /* 0x4259 */ be<u8> m313D;
    /* 0x425A */ be<u8> mHarpSeCount;  /* was m313E: 0x425A: animationPlay 02287364: 0 picks a new random mHarpSe and loads 2, else counts down and replays mHarpSe; 0 outside the harp frames; init 02285580 0 (tww the same) */
    /* 0x425B */ be<s8> mMoveBlockSide;  /* was m313F: 0x425B: execute 02288B08: chkMoveBlock(&mMoveBlockSpeed) (-1 / 1 for the side of a pushed block, else 0) starts escapeNpcAction; escapeNpcAction 02293500 turns by +/-0x4000 by its sign (tww the same); same role as daNpc_Cb1_c::mMoveBlockSide */
    /* 0x425C */ be<u8> mOldLightBodyHit;
    /* 0x425D */ u8 m3141[3];
    /* 0x4260 */ be<s16> m3144;
    /* 0x4262 */ be<s16> mActionTimer;  /* was m3146: 0x4262: a countdown in the actions: flyPlayerAction 02294160 loads HIO mFlapTime (maxFallSpeed 10 while it runs), harpWaitNpcAction 0228F424 0xC / 0x78, escapeNpcAction 02293500 0x1E; setAction 02285384 clears it (tww the same) */
    /* 0x4264 */ be<s16> mCrossWindHit;  /* was m3148: 0x4264: glidingNpcAction 02291304 sets 1 when windProc reports a cross wind (anm 0xA, glide ends after mGlideAnmCount); windProc 02290CF8 uses the throw gravity and the tail-wind lift only while 0; throwNpcAction 02291094 / setAction 02285384 0 (tww the same) */
    /* 0x4266 */ be<s16> mGlideAnmCount;  /* was m314A: 0x4266: glidingNpcAction 02291304 loads 2 and subtracts 1 per animation end once mCrossWindHit is set, then starts fall02NpcAction; setAction 02285384 0 (tww the same) */
    /* 0x4268 */ be<s16> mWindProcRet;  /* was m314C: 0x4268: glidingNpcAction 02291304 stores windProc's result (1 tail or no wind, 0 cross wind) and switches the animation when it changes; throwNpcAction 02291094 1, then 0 when windProc returns 0 (tww the same) */
    /* 0x426A */ be<s16> mThrowWindTimer;  /* was m314E: 0x426A: throwNpcAction 02291094 loads HIO mThrowWindTime; windProc 02290CF8 counts it down and lifts by a tail wind only between 0 and mThrowWindTime - mWindLiftDelay (tww the same) */
    /* 0x426C */ be<f32> mBasePosY;  /* was m3150: 0x426C: jumpPlayerAction 02293FA8 / fall02NpcAction 02291764 store current.pos.y and shake on landing after dropping HIO mShockFallHeight below it; flyPlayerAction 02294160 sets pos.y + 1500 and caps the flight height at it; setAction 02285384 0 (tww the same) */
    /* 0x4270 */ be<f32> mCrossWindPush;  /* was m3154: 0x4270: windProc 02290CF8: in a cross wind it grows by HIO mCrossWindStep up to the wind power times mCrossWindRate and is the applied power; throwNpcAction 02291094 0 (tww the same) */
    /* 0x4274 */ be<s16> mEventIdxTable[10];
    /* 0x4288 */ be<s8> m_hair_jnt_nums[8];
    /* 0x4290 */ cXyz mHairPos[8];  /* was m3174: hairTopNodeCallBack 022839F0 / hairNodeCallBack 02283A78: the hair joint positions; TWW getPHairPos(i) */
    /* 0x42F0 */ cXyz mHairVec[8];  /* was m31D4: hairNodeCallBack 02283A78: per hair joint vector; TWW getPHairVec(i) */
    /* 0x4350 */ cXyz mHairWall[4];  /* was m3234: waistNodeCallBack 022837D0: four waist points (waistVecDat) the hair collides with (hairCross); TWW getPHairWall() */
    /* 0x4380 */ be<f32> mHairDist[8];  /* was m3264: hairNodeCallBack 02283A78: per hair joint distance; TWW getPHairDist(i) */
    /* 0x43A0 */ char mModelArcName[3];
    /* 0x43A3 */ u8 m3287[0x43B4 - 0x43A3];
    /* 0x43B4 */ cXyz mCarryOffset;  /* was m3298: 0x43B4: carryNpcAction 022904DC clears y / z and chases it to HIO mCarryFlyOffsetX / Y / Z while Medli flies Link; setBaseMtx 02285A6C translates the model by it (tww the same); same role as daNpc_Cb1_c::mCarryOffset */
    /* 0x43C0 */ cXyz mGroundNormal;  /* was m32A4: 0x43C0: execute 02288B08 (md_setGroundInfo): the ground triangle's normal; waitGroundCheck 0228E9C0 turns to cM_atan2s(it.x, it.z); routeAngCheck 022926D0 crosses it with the wall normal (tww the same); same role as daNpc_Cb1_c::mGroundNormal */
    /* 0x43CC */ cBgS_PolyInfo_l mPolyInfo;
};
WWHD_OFFSET(daNpc_Md_c, mPhase, 0x608);
WWHD_OFFSET(daNpc_Md_c, mBtpAnm, 0x63C);
WWHD_OFFSET(daNpc_Md_c, mAcchCir, 0x724);
WWHD_OFFSET(daNpc_Md_c, mStts, 0x810);
WWHD_OFFSET(daNpc_Md_c, mCyl1, 0x84C);
WWHD_OFFSET(daNpc_Md_c, mCps, 0xBDC);
WWHD_OFFSET(daNpc_Md_c, mJntCtrl, 0xD14);
WWHD_OFFSET(daNpc_Md_c, mMirrorShieldCb, 0x4170);
WWHD_OFFSET(daNpc_Md_c, mRippleCb, 0x4198);
WWHD_OFFSET(daNpc_Md_c, mCurrPlayerActionFunc, 0x41F8);
WWHD_OFFSET(daNpc_Md_c, mCurrNpcActionFunc, 0x4200);
WWHD_OFFSET(daNpc_Md_c, mMsgNo, 0x4208);
WWHD_OFFSET(daNpc_Md_c, mActionStatus, 0x424E);
WWHD_OFFSET(daNpc_Md_c, mType, 0x4254);
WWHD_OFFSET(daNpc_Md_c, m3144, 0x4260);
WWHD_OFFSET(daNpc_Md_c, mBasePosY, 0x426C);
WWHD_OFFSET(daNpc_Md_c, mModelArcName, 0x43A0);
WWHD_OFFSET(daNpc_Md_c, mPolyInfo, 0x43CC);
WWHD_SIZE(daNpc_Md_c, 0x43DC);

/* dNpc_HIO_c (HD: fields first, vtable at +0x24; GameCube vtable at 0, fields +4) */
struct dNpc_HIO_l {
    /* 0x00 */ be<f32> mPlayerEyeOffsetY;  /* was m04: copy of dNpc_HIO_c (d/d_npc.h), see there */
    /* 0x04 */ be<s16> mMaxHeadX;
    /* 0x06 */ be<s16> mMaxBackboneX;
    /* 0x08 */ be<s16> mMaxHeadY;
    /* 0x0A */ be<s16> mMaxBackboneY;
    /* 0x0C */ be<s16> mMinHeadX;
    /* 0x0E */ be<s16> mMinBackboneX;
    /* 0x10 */ be<s16> mMinHeadY;
    /* 0x12 */ be<s16> mMinBackboneY;
    /* 0x14 */ be<s16> mMaxTurnStep;
    /* 0x16 */ be<s16> mMaxHeadTurnVel;
    /* 0x18 */ be<f32> mAttnYOffset;
    /* 0x1C */ be<s16> mMaxAttnAngleY;
    /* 0x1E */ be<u8> mDebugDraw;  /* was m22: copy of dNpc_HIO_c (d/d_npc.h), see there */
    /* 0x1F */ u8 _1F;
    /* 0x20 */ be<f32> mMaxAttnDistXZ;
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(dNpc_HIO_l, 0x28);

/* daNpc_Md_HIO_c (0x1CC; sub-HIOs have their vtable at 0 as on GameCube) */
struct daNpc_Md_HIO2_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<f32> mWalkAccel;  /* was m04: setSpeedAndAngleNormal 0228E4C0: the speed added per frame (it * stick * cos of the turn left) that setNormalSpeedF 0228E37C adds to mNormalSpeed (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (5) */
    be<f32> mWalkMaxSpeed;  /* was m08: walkProc 0228E5CC / walkPlayerAction 02293B5C: mMaxNormalSpeed; waitGroundCheck 0228E9C0 caps the slide speed on slope ground at it (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (17) */
    be<f32> mWalkMaxStep;  /* was m0C: setSpeedAndAngleNormal 0228E4C0: setNormalSpeedF 0228E37C's largest step of mNormalSpeed (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (7) */
    be<f32> mWalkMinStep;  /* was m10: setSpeedAndAngleNormal 0228E4C0: setNormalSpeedF 0228E37C's smallest step of mNormalSpeed (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (6) */
    be<f32> mWalkScale;  /* was m14: setSpeedAndAngleNormal 0228E4C0: setNormalSpeedF 0228E37C's cLib_addCalc scale for mNormalSpeed (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (0.7) */
    be<f32> mWalkAnmSpeed;  /* was m18: walkProc 0228E5CC: the body and arm animations play at |mNormalSpeed / mMaxNormalSpeed| * it (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (2.6) */
    be<f32> mWallSlowRate;  /* was m1C: setNormalSpeedF 0228E37C: facing a wall, the target speed is scaled by 1 - it * cos of the wall angle (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (0.6) */
    /* 0x20 */ be<s16> mTurnMaxStep;  /* was m20: setSpeedAndAngleNormal 0228E4C0 / searchNpcAction 02292C48 / escapeNpcAction 02293500 / walkPlayerAction 02293B5C: the largest cLib_addCalcAngleS step of the turn (the body turns twice it) (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (3300) */
    be<s16> mTurnMinStep;  /* was m22: setSpeedAndAngleNormal 0228E4C0 / searchNpcAction 02292C48 / escapeNpcAction 02293500 / walkPlayerAction 02293B5C: the smallest cLib_addCalcAngleS step of the turn (the body turns twice it) (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (2900) */
    be<s16> mTurnScale;  /* was m24: setSpeedAndAngleNormal 0228E4C0 / searchNpcAction 02292C48 / escapeNpcAction 02293500 / walkPlayerAction 02293B5C: the cLib_addCalcAngleS scale of the turn (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (5) */
    be<s16> mStickTurnScale;  /* was m26: waitPlayerAction 02293840: the cLib_addCalcAngleS scale of the turn toward the stick (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (25) */
    be<s16> mStickTurnMaxStep;  /* was m28: waitPlayerAction 02293840: the largest step of the turn toward the stick (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (0x7FFF) */
    be<s16> mStickTurnMinStep;  /* was m2A: waitPlayerAction 02293840: the smallest step of the turn toward the stick (tww the same); daNpc_Md_HIO2_c_ct 02294EA0 sets the default, never written elsewhere in WWHD or tww (1) */
};
WWHD_SIZE(daNpc_Md_HIO2_l, 0x2C);
struct daNpc_Md_HIO3_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<f32> mWaistRateX;  /* was m04: lookBackWaist 0228DFF8: the waist's X turn is it * the stick's forward part (tww the same); daNpc_Md_HIO3_c_ct 02294DFC sets the default, never written elsewhere in WWHD or tww (0.8) */
    be<f32> mWaistRateY;  /* was m08: lookBackWaist 0228DFF8: the waist's Y turn is it * the stick's side part (tww the same); daNpc_Md_HIO3_c_ct 02294DFC sets the default, never written elsewhere in WWHD or tww (0.2) */
    be<f32> mLightOffsetX;  /* was m0C: setCollision 02287A74 / draw 02289E2C, while daMdStts_LIGHT_HIT is set: the reflected light's x offset from the harp model (tww the same); daNpc_Md_HIO3_c_ct 02294DFC sets the default, never written elsewhere in WWHD or tww (-5) */
    be<f32> mLightOffsetY;  /* was m10: setCollision 02287A74 / draw 02289E2C, while daMdStts_LIGHT_HIT is set: the reflected light's y offset from the harp model (tww the same); daNpc_Md_HIO3_c_ct 02294DFC sets the default, never written elsewhere in WWHD or tww (-5) */
    be<f32> mLightOffsetZ;  /* was m14: setCollision 02287A74 / draw 02289E2C, while daMdStts_LIGHT_HIT is set: the reflected light's z offset from the harp model (tww the same); daNpc_Md_HIO3_c_ct 02294DFC sets the default, never written elsewhere in WWHD or tww (0) */
    /* 0x18 */ be<s16> mLightRotX;  /* was m18: setCollision 02287A74: the reflected light's X rotation at the harp model (tww the same); daNpc_Md_HIO3_c_ct 02294DFC sets the default, never written elsewhere in WWHD or tww (0) */
    be<s16> mLightRotY;  /* was m1A: setCollision 02287A74: the reflected light's Y rotation at the harp model (tww the same); daNpc_Md_HIO3_c_ct 02294DFC sets the default, never written elsewhere in WWHD or tww (0) */
    be<s16> mLightRotZ;  /* was m1C: setCollision 02287A74: the reflected light's Z rotation at the harp model (tww the same); daNpc_Md_HIO3_c_ct 02294DFC sets the default, never written elsewhere in WWHD or tww (0) */
    be<s16> mWaistMaxRotXPos;  /* was m1E: lookBackWaist 0228DFF8: mWaistRotX's target is it * the X turn when that is positive (tww the same); daNpc_Md_HIO3_c_ct 02294DFC sets the default, never written elsewhere in WWHD or tww (3800) */
    be<s16> mWaistMaxRotXNeg;  /* was m20: lookBackWaist 0228DFF8: mWaistRotX's target is it * the X turn when that is negative (tww the same); daNpc_Md_HIO3_c_ct 02294DFC sets the default, never written elsewhere in WWHD or tww (5800) */
    be<s16> mWaistMaxRotY;  /* was m22: lookBackWaist 0228DFF8: mWaistRotY's target is it * the Y turn (tww the same); daNpc_Md_HIO3_c_ct 02294DFC sets the default, never written elsewhere in WWHD or tww (0x7FFF) */
    be<s16> m24;
    /* 0x26 */ u8 _26[2];
};
WWHD_SIZE(daNpc_Md_HIO3_l, 0x28);
struct daNpc_Md_HIO4_l {
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<f32> m4;
    /* 0x8 */ be<s16> m8;
    /* 0xA */ u8 _A[2];
};
WWHD_SIZE(daNpc_Md_HIO4_l, 0xC);
struct daNpc_Md_HIO5_l {
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<f32> m4, m8;
};
WWHD_SIZE(daNpc_Md_HIO5_l, 0xC);
struct daNpc_Md_HIO6_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<f32> mShipMaxAttnDistXZ;  /* was m04: chkAttention 0228DAF4: the attention XZ distance while daMdStts_SHIP_RIDE is set (instead of mNpc.mMaxAttnDistXZ) (tww the same); daNpc_Md_HIO6_c_ct 02294CD0 sets the default, never written elsewhere in WWHD or tww (500) */
    /* 0x08 */ be<s16> mMaxHeadX;  /* was m08: execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's max_head_x (tww the same); daNpc_Md_HIO6_c_ct 02294CD0 sets the default, never written elsewhere in WWHD or tww (430) */
    be<s16> mMaxHeadY;  /* was m0A: execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's max_head_y (tww the same); daNpc_Md_HIO6_c_ct 02294CD0 sets the default, never written elsewhere in WWHD or tww (9000) */
    be<s16> mMinHeadX;  /* was m0C: execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's min_head_x (tww the same); daNpc_Md_HIO6_c_ct 02294CD0 sets the default, never written elsewhere in WWHD or tww (-5000) */
    be<s16> mMinHeadY;  /* was m0E: execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's min_head_y (tww the same); daNpc_Md_HIO6_c_ct 02294CD0 sets the default, never written elsewhere in WWHD or tww (-9000) */
    be<s16> mMaxBackboneX;  /* was m10: execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's max_backbone_x (tww the same); daNpc_Md_HIO6_c_ct 02294CD0 sets the default, never written elsewhere in WWHD or tww (0) */
    be<s16> mMaxBackboneY;  /* was m12: execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's max_backbone_y (tww the same); daNpc_Md_HIO6_c_ct 02294CD0 sets the default, never written elsewhere in WWHD or tww (0) */
    be<s16> mMinBackboneX;  /* was m14: execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's min_backbone_x (tww the same); daNpc_Md_HIO6_c_ct 02294CD0 sets the default, never written elsewhere in WWHD or tww (0) */
    be<s16> mMinBackboneY;  /* was m16: execute 02288B08: while daMdStts_SHIP_RIDE is set, mJntCtrl.setParam's min_backbone_y (tww the same); daNpc_Md_HIO6_c_ct 02294CD0 sets the default, never written elsewhere in WWHD or tww (0) */
};
WWHD_SIZE(daNpc_Md_HIO6_l, 0x18);
struct daNpc_Md_HIO_l {
    /* 0x000 */ be<u32> __vtbl;
    /* 0x004 */ be<s8> mNo;
    /* 0x005 */ u8 _005[3];
    /* 0x008 */ daNpc_Md_HIO2_l mWalk;  /* was m008: daNpc_Md_HIO2_c: the walk and turn tuning: walkProc 0228E5CC / walkPlayerAction 02293B5C take its mWalkMaxSpeed as mMaxNormalSpeed; setSpeedAndAngleNormal 0228E4C0 / setNormalSpeedF 0228E37C its speed factors and mTurnMaxStep / mTurnMinStep / mTurnScale; searchNpcAction 02292C48 / escapeNpcAction 02293500 turn the body with the same; waitPlayerAction 02293840 turns toward the stick with mStickTurnScale / mStickTurnMaxStep / mStickTurnMinStep; waitGroundCheck 0228E9C0 mWalkMaxSpeed; daNpc_Md_HIO_c_ct 02294F64 constructs it */
    /* 0x034 */ daNpc_Md_HIO3_l mMirror;  /* was m034: daNpc_Md_HIO3_c: the harp-mirror tuning: lookBackWaist 0228DFF8 (mkamaePlayerAction 02294984 / mwaitNpcAction 0228FF5C) turns the waist by the stick with its mWaistRateX / mWaistRateY factors and mWaistMaxRotXPos / mWaistMaxRotXNeg / mWaistMaxRotY limits; setCollision 02287A74 / draw 02289E2C place the reflected light at the harp model by its mLightOffsetX..Z offset and mLightRotX..Z rotation; daNpc_Md_HIO_c_ct 02294F64 constructs it */
    /* 0x05C */ daNpc_Md_HIO4_l mCarried;  /* was m05C: daNpc_Md_HIO4_c: setBaseMtx 02285A6C lowers the carried model by its m4 (86.0) while Link carries Medli in the no-carry-action pose; daNpc_Md_HIO_c_ct 02294F64 constructs it */
    /* 0x068 */ daNpc_Md_HIO5_l mKyoroAnm;  /* was m068: daNpc_Md_HIO5_c: setAnm 02286D7C takes the morf (m4) and speed (m8) of animation 0x28 from it (tww l_anmTbl: md_kyoro2.bck), which waitNpcAction 0228EB88 plays for the M_DAI / EDAICHI types; daNpc_Md_HIO_c_ct 02294F64 constructs it */
    /* 0x074 */ daNpc_Md_HIO6_l mShipRide;  /* was m074: daNpc_Md_HIO6_c: used while daMdStts_SHIP_RIDE is set: chkAttention 0228DAF4 takes its mShipMaxAttnDistXZ as the attention distance; execute 02288B08 passes its mMaxHeadX..mMinBackboneY to mJntCtrl.setParam as the head / backbone limits; daNpc_Md_HIO_c_ct 02294F64 constructs it */
    /* 0x08C */ dNpc_HIO_l mNpc;
    /* 0x0B4 */ gptr<fopAc_ac_c> mpActor;
    /* 0x0B8 */ be<f32> mDra09MaxAttnDistXZ;  /* was m0B8: chkAttention 0228DAF4: the attention XZ distance for the M_DRA09 type once event bit 1101 is set (instead of mNpc.mMaxAttnDistXZ); daNpc_Md_HIO_c_ct 02294F64 770.0 */
    /* 0x0BC */ be<f32> mMaxAttnY;  /* was m0BC: chkAttention 0228DAF4: Link counts for attention only below this height above Medli; waitNpcAction 0228EB88 the same bound for the SEA type's invitation; daNpc_Md_HIO_c_ct 02294F64 250.0 */
    /* 0x0C0 */ be<f32> mMinAttnY;  /* was m0C0: chkAttention 0228DAF4: Link counts for attention only above this height relative to Medli; waitNpcAction 0228EB88 the same bound for the SEA type's invitation; daNpc_Md_HIO_c_ct 02294F64 -450.0 */
    /* 0x0C4 */ be<f32> mFollowStopDist;  /* was m0C4: searchNpcAction 02292C48: Medli stops walking after Link inside this XZ distance (squared; walk speed rises beyond it); NpcCall 0228E8D8 / mwaitNpcAction 0228FF5C: a call ends (searchNpcAction) once Link is beyond twice it; daNpc_Md_HIO_c_ct 02294F64 90.0 */
    /* 0x0C8 */ be<f32> mCallDist;  /* was m0C8: NpcCall 0228E8D8 / mwaitNpcAction 0228FF5C: Link's call command reaches Medli within this distance (squared); daNpc_Md_HIO_c_ct 02294F64 400.0 */
    /* 0x0CC */ be<f32> mSeaEventDistXZ;  /* was m0CC: waitNpcAction 0228EB88: the SEA type starts its event (mode 0xB) while Link is within this XZ distance (squared) and between mMinAttnY / mMaxAttnY; daNpc_Md_HIO_c_ct 02294F64 600.0 */
    /* 0x0D0 */ be<f32> m0D0;
    /* 0x0D4 */ be<f32> mFlyMaxSpeed;  /* was m0D4: flyPlayerAction 02294160: the stick speed target is capped at it; daNpc_Md_HIO_c_ct 02294F64 17.0 */
    /* 0x0D8 */ be<f32> mFlyMinSpeed;  /* was m0D8: flyPlayerAction 02294160: a positive stick speed target below it is raised to it; daNpc_Md_HIO_c_ct 02294F64 0.0 */
    /* 0x0DC */ be<f32> mFlyAccel;  /* was m0DC: flyPlayerAction 02294160: cLib_chaseF step of speedF toward the stick speed; daNpc_Md_HIO_c_ct 02294F64 0.5 */
    /* 0x0E0 */ be<f32> m0E0;
    /* 0x0E4 */ be<f32> m0E4;
    /* 0x0E8 */ be<f32> mThrowSpeedF;  /* was m0E8: throwNpcAction 02291094: the forward speed Medli is thrown with; daNpc_Md_HIO_c_ct 02294F64 15.0 */
    /* 0x0EC */ be<f32> mThrowSpeedY;  /* was m0EC: throwNpcAction 02291094: the upward speed Medli is thrown with; daNpc_Md_HIO_c_ct 02294F64 22.0 */
    /* 0x0F0 */ be<f32> mThrowFlapSpeedY;  /* was m0F0: throwNpcAction 02291094: at wing frame 23 speed.y is raised to at least it (the flap); daNpc_Md_HIO_c_ct 02294F64 9.0 */
    /* 0x0F4 */ be<f32> mGravity;  /* was m0F4: gravity of the ground, jump and land actions: init 02285580, waitNpcAction 0228EB88, mwaitNpcAction 0228FF5C, jumpNpcAction 022933A0, jumpPlayerAction 02293FA8, landPlayerAction 02294838, wallHitNpcAction 022919D4 and 11 more; daNpc_Md_HIO_c_ct 02294F64 4.0 */
    /* 0x0F8 */ be<f32> mCarryFlyOffsetX;  /* was m0F8: carryNpcAction 022904DC: cLib_chaseF moves the carried model offset's x (mCarryOffset, setBaseMtx 02285A6C) toward it while Medli flies Link; daNpc_Md_HIO_c_ct 02294F64 -1.51367 */
    /* 0x0FC */ be<f32> mCarryFlyOffsetY;  /* was m0FC: carryNpcAction 022904DC: cLib_chaseF moves the carried model offset's y (mCarryOffset, setBaseMtx 02285A6C) toward it while Medli flies Link; daNpc_Md_HIO_c_ct 02294F64 -1.09863 */
    /* 0x100 */ be<f32> mCarryFlyOffsetZ;  /* was m100: carryNpcAction 022904DC: cLib_chaseF moves the carried model offset's z (mCarryOffset, setBaseMtx 02285A6C) toward it while Medli flies Link; daNpc_Md_HIO_c_ct 02294F64 -0.56152 */
    /* 0x104 */ be<f32> mStickTurnMin;  /* was m104: waitPlayerAction 02293840 / flyPlayerAction 02294160 / mkamaePlayerAction 02294984: Medli turns (and flies) by the stick only from this stick value on (or while locked on); daNpc_Md_HIO_c_ct 02294F64 0.1 */
    /* 0x108 */ be<f32> mStickWalkMin;  /* was m108: waitPlayerAction 02293840: from this stick value on the body follows the stick and Medli starts walkPlayerAction; daNpc_Md_HIO_c_ct 02294F64 0.2 */
    /* 0x10C */ be<f32> mFlyStickSpeed;  /* was m10C: flyPlayerAction 02294160: the stick value times it is the flying speed target; daNpc_Md_HIO_c_ct 02294F64 10.0 */
    /* 0x110 */ be<f32> mThrowMaxFallSpeed;  /* was m110: throwNpcAction 02291094 / windProc 02290CF8 / fall02NpcAction 02291764: maxFallSpeed while thrown or gliding down; daNpc_Md_HIO_c_ct 02294F64 -50.0 */
    /* 0x114 */ be<f32> mThrowGravity;  /* was m114: throwNpcAction 02291094 / windProc 02290CF8 / fall02NpcAction 02291764: gravity while thrown or gliding down; daNpc_Md_HIO_c_ct 02294F64 1.3 */
    /* 0x118 */ be<f32> mFiringAnmSpeed;  /* was m118: setAnm 02286D7C: the play speed of animation 6 (tww l_anmTbl: md_firing.bck), which throwNpcAction 02291094 plays; daNpc_Md_HIO_c_ct 02294F64 1.0 */
    /* 0x11C */ be<f32> mFly02AnmSpeed;  /* was m11C: setAnm 02286D7C: the play speed of animations 9 and 10 (tww l_anmTbl: md_fly02.bck), which carryNpcAction 022904DC, glidingNpcAction 02291304 and the fly event play; daNpc_Md_HIO_c_ct 02294F64 1.5 */
    /* 0x120 */ be<f32> mCarryTiredAnmSpeed;  /* was m120: carryNpcAction 022904DC: the body and wing animation speed once the flying timer is below 30; daNpc_Md_HIO_c_ct 02294F64 2.0 */
    /* 0x124 */ be<f32> mTiredFlyAnmSpeed;  /* was m124: setAnm 02286D7C: the play speed of animation 0x2E (tww l_anmTbl: md_fly02.bck), which flyPlayerAction 02294160 plays when the flying timer is below mTiredFlyTime; daNpc_Md_HIO_c_ct 02294F64 1.6 */
    /* 0x128 */ be<f32> m128;
    /* 0x12C */ be<f32> m12C;
    /* 0x130 */ be<f32> mCrossWindRate;  /* was m130: windProc 02290CF8: the wind power factor when the wind does not blow along Medli's facing (dot <= 0.7071); daNpc_Md_HIO_c_ct 02294F64 3.0 */
    /* 0x134 */ be<f32> mTailWindRate;  /* was m134: windProc 02290CF8: the wind power factor when the wind blows along Medli's facing; daNpc_Md_HIO_c_ct 02294F64 1.25 */
    /* 0x138 */ be<f32> mCrossWindStep;  /* was m138: windProc 02290CF8: in a cross wind the wind push (mCrossWindPush) grows by it each frame up to the power times mCrossWindRate; daNpc_Md_HIO_c_ct 02294F64 0.2 */
    /* 0x13C */ be<f32> mWindMaxSpeed;  /* was m13C: windProc 02290CF8: speedF after adding the wind is capped at it; daNpc_Md_HIO_c_ct 02294F64 100.0 */
    /* 0x140 */ be<f32> mWindLiftMaxFallSpeed;  /* was m140: windProc 02290CF8: maxFallSpeed while a tail wind lifts the thrown Medli (lift window of the mThrowWindTimer timer); daNpc_Md_HIO_c_ct 02294F64 50.0 */
    /* 0x144 */ be<f32> mWindLiftGravity;  /* was m144: windProc 02290CF8: gravity while a tail wind lifts the thrown Medli (lift window of the mThrowWindTimer timer); daNpc_Md_HIO_c_ct 02294F64 9.0 */
    /* 0x148 */ be<f32> mWindAtkPower;  /* was m148: windProc 02290CF8: the wind power when a wind attack hits mCyl2 (with mbWindAtkHit set); daNpc_Md_HIO_c_ct 02294F64 0.9 */
    /* 0x14C */ be<f32> mHairGravity;  /* was m14C: hairNodeCallBack 02283A78: the length of the downward pull vector on each hair joint; daNpc_Md_HIO_c_ct 02294F64 6.0 */
    /* 0x150 */ be<f32> mWallHitSpeedF;  /* was m150: wallHitNpcAction 022919D4: the speedF Medli bounces off the wall with; daNpc_Md_HIO_c_ct 02294F64 10.0 */
    /* 0x154 */ be<f32> mThrowWindFrame;  /* was m154: throwNpcAction 02291094: windProc runs only once the wing animation frame is past it; daNpc_Md_HIO_c_ct 02294F64 5.0 */
    /* 0x158 */ be<f32> m158;
    /* 0x15C */ be<f32> m15C;
    /* 0x160 */ be<f32> m160;
    /* 0x164 */ be<f32> m164;
    /* 0x168 */ be<f32> m168;
    /* 0x16C */ be<f32> m16C;
    /* 0x170 */ be<f32> m170;
    /* 0x174 */ be<f32> m174;
    /* 0x178 */ be<f32> m178;
    /* 0x17C */ be<f32> m17C;
    /* 0x180 */ be<f32> m180;
    /* 0x184 */ be<f32> m184;
    /* 0x188 */ be<f32> mFlyGravity;  /* was m188: flyPlayerAction 02294160 / initialFlyEvent 0228C1C4: gravity while flying; daNpc_Md_HIO_c_ct 02294F64 1.0 */
    /* 0x18C */ be<f32> mFlyMaxFallSpeed;  /* was m18C: flyPlayerAction 02294160: maxFallSpeed while flying (10.0 instead during mFlapTime); daNpc_Md_HIO_c_ct 02294F64 -10.0 */
    /* 0x190 */ be<f32> m190;
    /* 0x194 */ be<f32> m194;
    /* 0x198 */ be<f32> m198;
    /* 0x19C */ be<f32> m19C;
    /* 0x1A0 */ be<f32> m1A0;
    /* 0x1A4 */ be<f32> m1A4;
    /* 0x1A8 */ be<f32> mFly01AnmMorf;  /* was m1A8: setAnm 02286D7C: the morf of animations 7 and 8 (tww l_anmTbl: md_fly01.bck), which flyPlayerAction 02294160 plays; daNpc_Md_HIO_c_ct 02294F64 3.0 */
    /* 0x1AC */ be<f32> mHighFallHeight;  /* was m1AC: carryNpcAction 022904DC: put down at least this high above the ground, Medli falls with fall02NpcAction instead of fallNpcAction; daNpc_Md_HIO_c_ct 02294F64 1000.0 */
    /* 0x1B0 */ be<f32> mShockFallHeight;  /* was m1B0: jumpPlayerAction 02293FA8 / fall02NpcAction 02291764: a landing after a drop of at least this height (from mBasePosY) shakes the camera (the M_DAI type in fall02); daNpc_Md_HIO_c_ct 02294F64 1000.0 */
    /* 0x1B4 */ be<s16> mThrowWindTime;  /* was m1B4: throwNpcAction 02291094 loads the mThrowWindTimer timer with it; windProc 02290CF8 lifts by a tail wind only while the timer is between 0 and it minus mWindLiftDelay; daNpc_Md_HIO_c_ct 02294F64 23 */
    /* 0x1B6 */ be<s16> mWindLiftDelay;  /* was m1B6: windProc 02290CF8: the tail-wind lift starts only this many frames after the throw (timer below mThrowWindTime minus it); daNpc_Md_HIO_c_ct 02294F64 20 */
    /* 0x1B8 */ be<u16> m1B8;
    /* 0x1BA */ be<s16> mPiyoTime;  /* was m1BA: piyo2NpcAction 02291F50: the dizzy time (m3144), halved for the EDAICHI / M_DAI types; daNpc_Md_HIO_c_ct 02294F64 120 */
    /* 0x1BC */ be<s16> mTurnLookMaxVel;  /* was m1BC: lookBack 0228A524: mLookMaxVel eases toward it instead of mNpc.mMaxHeadTurnVel when actionTurnEvent 0228D32C turns Medli; daNpc_Md_HIO_c_ct 02294F64 500 */
    /* 0x1BE */ be<s16> mFlyKeepTime;  /* was m1BE: flyPlayerAction 02294160: each A press (and the take-off) reloads m3144 with it; the flight ends when m3144 runs out; daNpc_Md_HIO_c_ct 02294F64 70 */
    /* 0x1C0 */ be<s16> mFlapTime;  /* was m1C0: flyPlayerAction 02294160: each A press (and the take-off) reloads mActionTimer with it; while mActionTimer runs, maxFallSpeed is 10.0 instead of mFlyMaxFallSpeed; daNpc_Md_HIO_c_ct 02294F64 5 */
    /* 0x1C2 */ be<u16> m1C2;
    /* 0x1C4 */ be<s16> mTiredFlyTime;  /* was m1C4: flyPlayerAction 02294160: below this flying timer Medli switches to animation 0x2E and the hane03 emitter; daNpc_Md_HIO_c_ct 02294F64 150 */
    /* 0x1C6 */ be<u8> mbWindAtkHit;  /* was m1C6: windProc 02290CF8: while set, a wind attack hitting mCyl2 pushes Medli (power mWindAtkPower); daNpc_Md_HIO_c_ct 02294F64 1 */
    /* 0x1C7 */ be<u8> mbWindLiftAlways;  /* was m1C7: windProc 02290CF8: while set, the tail-wind lift applies even when mCrossWindHit is set; daNpc_Md_HIO_c_ct 02294F64 0 */
    /* 0x1C8 */ be<u8> m1C8;
    /* 0x1C9 */ u8 _1C9[3];
};
WWHD_OFFSET(daNpc_Md_HIO_l, mpActor, 0xB4);
WWHD_OFFSET(daNpc_Md_HIO_l, mThrowWindTime, 0x1B4);
WWHD_SIZE(daNpc_Md_HIO_l, 0x1CC);
static inline daNpc_Md_HIO_l& l_HIO() { return *gabi::at<daNpc_Md_HIO_l>(MD_L_HIO); }
