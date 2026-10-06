/* daNpc_Md_c (Medli), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_md (profile 0x101C1F10: size 0x43DC;
 * GameCube 0x32C0; the constructor is inlined in create 02286084):
 * - daPy_py_c: GameCube fields 0x290..0x31C at +0x11C (mDemo at 0x420); the GameCube vtable
 *   slot 0x31C is merged into the HD vtable at 0xB4 (0x438 is padding).
 * - daPy_npc_c (constructor 024450B4 allocates 0x608): mAcch 0x43C, m4E4 0x600.
 * - members from mPhase (GameCube 0x4EC) on are +0x11C up to the btp animation;
 *   mDoExt_btpAnm / mDoExt_btkAnm grew from 0x14 to 0x74 (+0x60 each), and mShadowId
 *   (GameCube 0x548) is gone (HD shadows): +0x1D8 from mAcchCir to mJntCtrl;
 * - dDlst_mirrorPacket grew from 0x24DC to 0x3428: +0x1124 from m304C to m30D0;
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
    /* 0x3CC */ be<f32> field_0x2b0;
    /* 0x3D0 */ csXyz mBodyAngle;
    /* 0x3D6 */ u8 _3D6[2];
    /* 0x3D8 */ cXyz mHeadTopPos;
    /* 0x3E4 */ cXyz mSwordTopPos;
    /* 0x3F0 */ cXyz mLeftHandPos;
    /* 0x3FC */ cXyz mRightHandPos;
    /* 0x408 */ cXyz mRopePos;
    /* 0x414 */ cXyz field_0x2f8;
    /* 0x420 */ daPy_demo_l mDemo;
    /* 0x438 */ u8 _438[4];
};
WWHD_SIZE(daPy_py_l, 0x43C);

/* daPy_npc_c (HD 0x608, constructor 024450B4) */
struct daPy_npc_l : daPy_py_l {
    /* 0x43C */ dBgS_ObjAcch mAcch;
    /* 0x600 */ be<u32> m4E4;
    /* 0x604 */ be<u8> m4E8;
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
    void setBitStatus(u32 s) { m30F0 = m30F0 | s; }
    void clearStatus(u32 s) { m30F0 = m30F0 & ~s; }
    BOOL checkStatus(u32 s) { return (m30F0 & s) != 0; }
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
    /* 0x0624 */ be<u32> m0508[6];                         /* JPABaseEmitter* */
    /* 0x063C */ u8 m0520[0x74];                           /* mDoExt_btpAnm (HD 0x74), GameCube 0x520 */
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
    /* 0x0D48 */ u8 m0B70[0x3428];                         /* dDlst_mirrorPacket (HD 0x3428, GameCube 0x24DC) */
    /* 0x4170 */ daPy_mtxFollowEcallBack_l m304C;          /* GameCube 0x304C */
    /* 0x417C */ daNpc_Md_followEcallBack_c m3058;
    /* 0x4198 */ dPa_rippleEcallBack_md m3074;
    /* 0x41AC */ cXyz m3088;                               /* GameCube 0x3088 */
    /* 0x41B8 */ cXyz m3094;
    /* 0x41C4 */ cXyz m30A0;
    /* 0x41D0 */ cXyz m30AC;
    /* 0x41DC */ cXyz m30B8;
    /* 0x41E8 */ cXyz m30C4;
    /* 0x41F4 */ be<f32> m30D0;
    /* 0x41F8 */ ProcFunc_l mCurrPlayerActionFunc;         /* GameCube 0x30D4 (12 bytes) */
    /* 0x4200 */ ProcFunc_l mCurrNpcActionFunc;            /* GameCube 0x30E0 */
    /* 0x4208 */ be<u32> mMsgNo;                           /* GameCube 0x30EC */
    /* 0x420C */ be<u32> m30F0;
    /* 0x4210 */ be<u32> m30F4;
    /* 0x4214 */ be<f32> m30F8;
    /* 0x4218 */ be<f32> mRunRate;
    /* 0x421C */ be<s32> m3100;
    /* 0x4220 */ be<s32> m3104;
    /* 0x4224 */ be<f32> m3108;
    /* 0x4228 */ be<f32> m310C;
    /* 0x422C */ be<s16> m3110;
    /* 0x422E */ be<s16> m3112;
    /* 0x4230 */ be<s16> m3114;
    /* 0x4232 */ be<s16> m3116;
    /* 0x4234 */ be<s16> m3118;
    /* 0x4236 */ be<s16> m311A;
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
    /* 0x4246 */ be<s8> m312A;
    /* 0x4247 */ be<u8> m312B;
    /* 0x4248 */ be<u8> m312C;
    /* 0x4249 */ be<s8> m312D;
    /* 0x424A */ be<s8> mCurEventMode;
    /* 0x424B */ be<u8> m312F;
    /* 0x424C */ be<u8> m3130;
    /* 0x424D */ be<u8> m3131;
    /* 0x424E */ be<s8> mActionStatus;
    /* 0x424F */ be<u8> m3133;
    /* 0x4250 */ be<u8> m3134;
    /* 0x4251 */ be<u8> m3135;
    /* 0x4252 */ be<u8> m3136;
    /* 0x4253 */ be<u8> m3137;
    /* 0x4254 */ be<u8> mType;
    /* 0x4255 */ be<s8> mCurEvent;
    /* 0x4256 */ be<u8> m313A;
    /* 0x4257 */ u8 m313B;
    /* 0x4258 */ be<u8> m313C;
    /* 0x4259 */ be<u8> m313D;
    /* 0x425A */ be<u8> m313E;
    /* 0x425B */ be<s8> m313F;
    /* 0x425C */ be<u8> mOldLightBodyHit;
    /* 0x425D */ u8 m3141[3];
    /* 0x4260 */ be<s16> m3144;
    /* 0x4262 */ be<s16> m3146;
    /* 0x4264 */ be<s16> m3148;
    /* 0x4266 */ be<s16> m314A;
    /* 0x4268 */ be<s16> m314C;
    /* 0x426A */ be<s16> m314E;
    /* 0x426C */ be<f32> m3150;
    /* 0x4270 */ be<f32> m3154;
    /* 0x4274 */ be<s16> mEventIdxTable[10];
    /* 0x4288 */ be<s8> m_hair_jnt_nums[8];
    /* 0x4290 */ cXyz m3174[8];
    /* 0x42F0 */ cXyz m31D4[8];
    /* 0x4350 */ cXyz m3234[4];
    /* 0x4380 */ be<f32> m3264[8];
    /* 0x43A0 */ char mModelArcName[3];
    /* 0x43A3 */ u8 m3287[0x43B4 - 0x43A3];
    /* 0x43B4 */ cXyz m3298;
    /* 0x43C0 */ cXyz m32A4;
    /* 0x43CC */ cBgS_PolyInfo_l mPolyInfo;
};
WWHD_OFFSET(daNpc_Md_c, mPhase, 0x608);
WWHD_OFFSET(daNpc_Md_c, m0520, 0x63C);
WWHD_OFFSET(daNpc_Md_c, mAcchCir, 0x724);
WWHD_OFFSET(daNpc_Md_c, mStts, 0x810);
WWHD_OFFSET(daNpc_Md_c, mCyl1, 0x84C);
WWHD_OFFSET(daNpc_Md_c, mCps, 0xBDC);
WWHD_OFFSET(daNpc_Md_c, mJntCtrl, 0xD14);
WWHD_OFFSET(daNpc_Md_c, m304C, 0x4170);
WWHD_OFFSET(daNpc_Md_c, m3074, 0x4198);
WWHD_OFFSET(daNpc_Md_c, mCurrPlayerActionFunc, 0x41F8);
WWHD_OFFSET(daNpc_Md_c, mCurrNpcActionFunc, 0x4200);
WWHD_OFFSET(daNpc_Md_c, mMsgNo, 0x4208);
WWHD_OFFSET(daNpc_Md_c, mActionStatus, 0x424E);
WWHD_OFFSET(daNpc_Md_c, mType, 0x4254);
WWHD_OFFSET(daNpc_Md_c, m3144, 0x4260);
WWHD_OFFSET(daNpc_Md_c, m3150, 0x426C);
WWHD_OFFSET(daNpc_Md_c, mModelArcName, 0x43A0);
WWHD_OFFSET(daNpc_Md_c, mPolyInfo, 0x43CC);
WWHD_SIZE(daNpc_Md_c, 0x43DC);

/* dNpc_HIO_c (HD: fields first, vtable at +0x24; GameCube vtable at 0, fields +4) */
struct dNpc_HIO_l {
    /* 0x00 */ be<f32> m04;
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
    /* 0x1E */ be<u8> m22;
    /* 0x1F */ u8 _1F;
    /* 0x20 */ be<f32> mMaxAttnDistXZ;
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(dNpc_HIO_l, 0x28);

/* daNpc_Md_HIO_c (0x1CC; sub-HIOs have their vtable at 0 as on GameCube) */
struct daNpc_Md_HIO2_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<f32> m04, m08, m0C, m10, m14, m18, m1C;
    /* 0x20 */ be<s16> m20, m22, m24, m26, m28, m2A;
};
WWHD_SIZE(daNpc_Md_HIO2_l, 0x2C);
struct daNpc_Md_HIO3_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<f32> m04, m08, m0C, m10, m14;
    /* 0x18 */ be<s16> m18, m1A, m1C, m1E, m20, m22, m24;
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
    /* 0x04 */ be<f32> m04;
    /* 0x08 */ be<s16> m08, m0A, m0C, m0E, m10, m12, m14, m16;
};
WWHD_SIZE(daNpc_Md_HIO6_l, 0x18);
struct daNpc_Md_HIO_l {
    /* 0x000 */ be<u32> __vtbl;
    /* 0x004 */ be<s8> mNo;
    /* 0x005 */ u8 _005[3];
    /* 0x008 */ daNpc_Md_HIO2_l m008;
    /* 0x034 */ daNpc_Md_HIO3_l m034;
    /* 0x05C */ daNpc_Md_HIO4_l m05C;
    /* 0x068 */ daNpc_Md_HIO5_l m068;
    /* 0x074 */ daNpc_Md_HIO6_l m074;
    /* 0x08C */ dNpc_HIO_l mNpc;
    /* 0x0B4 */ gptr<fopAc_ac_c> mpActor;
    /* 0x0B8 */ be<f32> m0B8, m0BC, m0C0, m0C4, m0C8, m0CC, m0D0, m0D4, m0D8, m0DC, m0E0, m0E4,
        m0E8, m0EC, m0F0, m0F4, m0F8, m0FC, m100, m104, m108, m10C, m110, m114, m118, m11C,
        m120, m124, m128, m12C, m130, m134, m138, m13C, m140, m144, m148, m14C, m150, m154,
        m158, m15C, m160, m164, m168, m16C, m170, m174, m178, m17C, m180, m184, m188, m18C,
        m190, m194, m198, m19C, m1A0, m1A4, m1A8, m1AC, m1B0;
    /* 0x1B4 */ be<s16> m1B4, m1B6;
    /* 0x1B8 */ be<u16> m1B8;
    /* 0x1BA */ be<s16> m1BA, m1BC, m1BE, m1C0;
    /* 0x1C2 */ be<u16> m1C2;
    /* 0x1C4 */ be<s16> m1C4;
    /* 0x1C6 */ be<u8> m1C6, m1C7, m1C8;
    /* 0x1C9 */ u8 _1C9[3];
};
WWHD_OFFSET(daNpc_Md_HIO_l, mpActor, 0xB4);
WWHD_OFFSET(daNpc_Md_HIO_l, m1B4, 0x1B4);
WWHD_SIZE(daNpc_Md_HIO_l, 0x1CC);
static inline daNpc_Md_HIO_l& l_HIO() { return *gabi::at<daNpc_Md_HIO_l>(MD_L_HIO); }
