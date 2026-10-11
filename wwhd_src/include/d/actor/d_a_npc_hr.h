/* daNpc_Hr_c (Zephos & Cyclos), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_hr: daNpc_Hr_c derives from
 * fopAc_ac_c directly, so its members are +0x11C up to m_head_tex_pattern; mDoExt_btpAnm grew
 * from 0x14 to 0x74, so everything from mBlinkFrame on is +0x17C up to mProcId; the pointer to
 * member function is 8 bytes (GHS) instead of 12, so the members after it are +0x178.
 * daNpc_Wind_Eff (0x38) and daNpc_Wind_Clothes (0x134) are unchanged. Size 0x940 (GameCube
 * 0x7C8). */
#pragma once
#include "SSystem/SComponent/c_angle.h"
#include "bindings.h"
#include "d/d_item.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_msg.h"

/* GHS pointer to member function {s16 this delta, s16 vtable index (<0: not virtual), u32
 * function (or s16 vtable offset at +6)} */
enum dRes_INDEX_HR {
    /* BCKS */
    dRes_INDEX_HR_BCK_DAMAGE_e = 0x6,
    dRes_INDEX_HR_BCK_H_ANT_e = 0x7,
    dRes_INDEX_HR_BCK_H_WAIT01_e = 0x8,
    dRes_INDEX_HR_BCK_LOOK_e = 0x9,
    dRes_INDEX_HR_BCK_R_ANT_e = 0xA,
    dRes_INDEX_HR_BCK_R_WAIT01_e = 0xB,
    dRes_INDEX_HR_BCK_R_WAIT02_e = 0xC,
    dRes_INDEX_HR_BCK_TALK01_e = 0xD,
    dRes_INDEX_HR_BCK_TALK02_e = 0xE,
    dRes_INDEX_HR_BCK_TALK03_e = 0xF,
    /* BDLM */
    dRes_INDEX_HR_BDL_H_ANT_e = 0x12,
    dRes_INDEX_HR_BDL_H_BROW_e = 0x13,
    dRes_INDEX_HR_BDL_HR_e = 0x14,
    dRes_INDEX_HR_BDL_R_ANT_e = 0x15,
    dRes_INDEX_HR_BDL_R_BROW_e = 0x16,
    /* BMT  */
    dRes_INDEX_HR_BMT_R_BODY_e = 0x19,
    /* BTP  */
    dRes_INDEX_HR_BTP_MABA_H_e = 0x1C,
    dRes_INDEX_HR_BTP_MABA_R01_e = 0x1D,
    dRes_INDEX_HR_BTP_MABA_R02_e = 0x1E,
};

struct ProcFunc_hr {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    union {
        /* 0x4 */ be<u32> f;
        struct {
            be<s16> mVirtualReserved, mVirtualOffset;
        } mVirtual;
    };
};
WWHD_SIZE(ProcFunc_hr, 8);

namespace dPa_name {
enum {
    ID_AK_JN_WINDLINE00 = 0x31,
    ID_AK_JN_OK = 0xD,
    ID_IT_SN_KINTOUN_FT00 = 0x821F,
    ID_IT_SN_KINTOUN_RT00 = 0x8220,
};
}

struct daNpc_Wind_Eff {
    enum {
        WIND_EFF_INACTIVE = 0,
        WIND_EFF_FADE_IN = 1,
        WIND_EFF_ACTIVE = 2,
        WIND_EFF_FADE_OUT = 3
    };
    void init();
    void remove();
    BOOL create(cXyz*);
    BOOL end();
    void proc();
    void setspd();
    void move();

    /* 0x00 */ dPa_followEcallBack mpFollowECallBack;
    /* 0x14 */ cXyz mPos;
    /* 0x20 */ cXyz mSpeed;
    /* 0x2C */ gptr<cXyz> mpSquallPos;
    /* 0x30 */ be<f32> mAlphaFactor;
    /* 0x34 */ be<u8> mWindEffState;
    /* 0x35 */ u8 _35[3];
};
WWHD_SIZE(daNpc_Wind_Eff, 0x38);

struct daNpc_Wind_Clothes {
    void setSquallPos(int);
    BOOL create(fopAc_ac_c*, u32 /* u8 */, be<f32>*, int);
    BOOL end();
    void proc();
    void init();
    void remove();

    /* 0x000 */ daNpc_Wind_Eff mWindEff[4];
    /* 0x0E0 */ be<s16> mSquallCounter[4];
    /* 0x0E8 */ cXyz mSquallPos[4];
    /* 0x118 */ be<u8> mSquallState;
    /* 0x119 */ u8 _119[3];
    /* 0x11C */ be<u32> mProcId;
    /* 0x120 */ be<f32> squallOrbitParam[5];
};
WWHD_OFFSET(daNpc_Wind_Clothes, mSquallCounter, 0xE0);
WWHD_OFFSET(daNpc_Wind_Clothes, mProcId, 0x11C);
WWHD_SIZE(daNpc_Wind_Clothes, 0x134);

struct daNpc_Hr_c : fopAc_ac_c {
    enum SoundEvents {
        JA_SE_CV_RC_DAMAGE = 0x48EA,
        JA_SE_CV_RC_MODAE_1 = 0x48EB,
        JA_SE_CV_RC_MODAE_2 = 0x48EC,
        JA_SE_TAKT_WIND_DEMO = 0x876,
        JA_SE_CM_HU_RC_FLY_AWAY = 0x5926,
        JA_SE_CV_RC_FLY_AWAY = 0x48ED,
        JA_SE_CV_HU_FLY_AWAY = 0x48E9,
        JA_SE_CV_RC_ENTER = 0x48EE,
        JA_BGM_DIOCTA_BATTLE = 0x8000002B,
        JA_SE_CM_HU_RC_MOVE_RAPID = 0x69FD,
        JA_SE_CM_HU_RC_FLYING = 0x5125,
        JA_SE_PRE_TAKT = 0x8A7,
    };
    enum MessageStatus {
        fopMsgStts_MSG_DISPLAYED_e = 0xE,
        fopMsgStts_MSG_DESTROYED_e = 0x13,
        fopMsgStts_INPUT_e = 0x15,
        HR_MSG_NO_MESSAGE = 0xFE,
        HR_MSG_WAITING = 0xFF,
    };
    enum { DSNAP_TYPE_NPC_HR = 0x9A, dItemNo_WINDS_REQUIEM_e = 0x6D };
    struct dSv_event_flag_c {
        enum { UNK_2708 = 0x2708, UNK_2710 = 0x2710, UNK_3D01 = 0x3D01 };
    };
    struct dSv_event_tmp_flag_c {
        enum { UNK_0404 = 0x0404 };
    };
    enum DemoActions {
        HR_DEMO_WAIT = 0,
        HR_DEMO_SPEAK = 1,
        HR_DEMO_PATTEN = 2,
        HR_DEMO_TACT0 = 3,
        HR_DEMO_TACT1 = 4,
        HR_DEMO_TACT2 = 5,
        HR_DEMO_TACT3 = 6,
        HR_DEMO_WIND0 = 7,
        HR_DEMO_WIND1 = 8,
        HR_DEMO_HIDE_LINK = 9,
        HR_DEMO_DISP_LINK = 10,
        HR_DEMO_DISP_MY = 11,
        HR_DEMO_MOVE = 12,
        HR_DEMO_TURN_LINK = 13,
        HR_DEMO_LOOK = 14,
        HR_DEMO_SMALL = 15,
        HR_DEMO_INTRO = 16,
        HR_DEMO_CHANGE = 17,
        HR_DEMO_DEBUG = 18
    };
    enum ActionStatus { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_ENDING = -1 };
    enum TalkStates { TALK_INIT = 0, TALK_MSG_CREATE = 1, TALK_ACTIVE = 2, TALK_FINISHED = -1 };
    enum HrFlags {
        HR_FLAG_00000001 = 0x0001,
        HR_FLAG_00000002 = 0x0002,
        HR_FLAG_00000008 = 0x0008,
        HR_FLAG_00000010 = 0x0010,
        HR_FLAG_00000020 = 0x0020,
        HR_FLAG_00000040 = 0x0040,
        HR_FLAG_00000080 = 0x0080,
        HR_FLAG_00000100 = 0x0100,
        HR_FLAG_00000200 = 0x0200,
        HR_FLAG_00000400 = 0x0400,
        HR_FLAG_00000800 = 0x0800,
    };
    enum EvFlags {
        EVFLAG_LOCK_ROTATION = 0x01,
        EVFLAG_SKIP_UPDATE = 0x02,
        EVFLAG_TORNADO_ACTIVE = 0x08,
        EVFLAG_EVENT_CHANGE_REQ = 0x10,
        EVFLAG_TACT_SOUND_ACTIVE = 0x20,
    };
    enum HrStates {
        HR_STATE_WAIT_01 = 0,
        HR_STATE_WAIT_02 = 1,
        HR_STATE_TALK = 2,
        HR_STATE_HT_TACT = 3,
        HR_STATE_RT_SEARCH = 5,
        HR_STATE_RT_HIDE = 6,
        HR_STATE_RT_INTRO = 7,
        HR_STATE_RT_ANGRY = 8,
        HR_STATE_RT_HIT_0 = 9,
        HR_STATE_RT_HIT_1 = 10,
        HR_STATE_HT_HIDE = 11,
        HR_STATE_RT_WIN = 12,
    };

    bool chkFlag(u16 flag) {
        return (mFlags & flag) == flag;
    }
    void clrFlag(u16 flag) {
        mFlags = (u16)(mFlags & ~flag);
    }
    void setFlag(u16 flag) {
        mFlags = (u16)(mFlags | flag);
    }
    bool chkEvFlag(u8 f) {
        return (mEvFlags & f) == f;
    }
    void setEvFlag(u8 f) {
        mEvFlags = (u8)(mEvFlags | f);
    }
    void clrEvFlag(u8 f) {
        mEvFlags = (u8)(mEvFlags & ~f);
    }
    u32 ChkOrder(u8 f) {
        return mOrderFlags & f;
    }
    void SetOrder(u8 f) {
        mOrderFlags = (u8)(mOrderFlags | f);
    }
    void ClrOrder() {
        mOrderFlags = 0;
    }
    bool isMorf() {
        return mpHrMorf->mCurMorf < 1.0f;
    }

    int getShapeType();
    int getSwbit();
    s16 XyCheckCB(int);
    void onHide(int);
    void offHide(int);
    void defaultSetPos(cXyz*);
    s32 getNowEventAction();
    void demoInitWind();
    bool demoProcWind(int);
    void demoInitWait();
    BOOL demoProcWait();
    void demoInitSpeak();
    BOOL demoProcSpeak();
    BOOL demoProcPatten();
    BOOL demoProcTact0();
    BOOL demoProcTact1();
    BOOL demoProcTact2();
    BOOL demoProcTact3();
    int calcKaijou(int);
    void demoInitMove();
    void demoInitSmall();
    BOOL demoProcSmall();
    bool demoProcMove();
    void demoInitChange();
    void demoInitCom();
    void demoProcCom();
    u32 demoProc(); /* bool, passed on raw */
    void node_Ht_ant(int);
    BOOL initTexPatternAnm(u32 /* bool */);
    void playTexPatternAnm();
    u32 setTexPtn(s32 /* s8, compared unextended */);
    void setAnm(s32 /* s8, compared unextended */);
    void setAnmStatus();
    void eventOrder();
    void checkOrder();
    u32 next_msgStatus(be<u32>*); /* u16 */
    u32 getMsg();
    void setCollision();
    void nextAnm(s32 /* s8 */, int);
    void msgAnm(u32 /* u8, compared unextended */);
    void talkInit();
    u16 talk();
    BOOL init();
    void setAttention(int /* bool */);
    int getNowJointY();
    void getTornadoPos(int, cXyz*);
    bool rideTornado();
    u8 getLookBackMode();
    void lookBack();
    bool rt_search();
    bool rt_hide();
    bool rt_intro();
    void to_rt_hit();
    void to_rt_tact();
    bool rt_angry();
    bool rt_win();
    bool rt_hit0();
    bool rt_hit1();
    bool ht_hide();
    bool wait01();
    void endTalk();
    void endTact();
    void setEmitFlash(f32);
    void smokeProc();
    bool talk01();
    u32 ht_tact01(); /* bool, passed on raw */
    BOOL wait_action(void*);
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();

    /* 0x3AC */ be<s8> m_waist_jnt_num;
    /* 0x3AD */ u8 _3AD[3];
    /* 0x3B0 */ cXyz mCloudPos;
    /* 0x3BC */ be<s16> mEventIdx;
    /* 0x3BE */ be<s8> mAnmIdx;
    /* 0x3BF */ u8 _3BF;
    /* 0x3C0 */ be<s16> mHeadRotation;
    /* 0x3C2 */ u8 _3C2[2];
    /* 0x3C4 */ request_of_phase_process_class mPhs;
    /* 0x3CC */ gptr<mDoExt_McaMorf> mpHrMorf;
    /* 0x3D0 */ gptr<J3DModel> mpBrowModel;
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpAntennaMorf;
    /* 0x3D8 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x3DC */ mDoExt_btpAnm mBtpAnm; /* mDoExt_btpAnm (HD 0x74) */
    /* 0x450 */ be<u8> mBlinkFrame;
    /* 0x451 */ u8 _451;
    /* 0x452 */ be<s16> mBlinkTimer;
    /* 0x454 */ dCcD_Stts mStts;
    /* 0x490 */ dCcD_Cyl mCyl;
    /* 0x5C0 */ dCcD_Stts mStts2;
    /* 0x5FC */ dCcD_Cyl mCyl2;
    /* 0x72C */ dNpc_JntCtrl_c m_jnt;
    /* 0x760 */ cXyz mEyePos;
    /* 0x76C */ cXyz mAttnBasePos;
    /* 0x778 */ be<s16> mMaxHeadTurnVelocity;
    /* 0x77A */ be<s8> mAnmEnded;
    /* 0x77B */ be<u8> mAttnSetCount;
    /* 0x77C */ be<f32> mAnmTimer;
    /* 0x780 */ be<u32> mCurrMsgNo;
    /* 0x784 */ be<u16> mFlags;
    /* 0x786 */ be<u8> field_0x60A;
    /* 0x787 */ be<u8> mHitCount;
    /* 0x788 */ be<u8> mHitDelayTimer;
    /* 0x789 */ u8 _789[3];
    /* 0x78C */ dPa_followEcallBack mSmokeCallBack;
    /* 0x7A0 */ be<u32> mProcId;
    /* 0x7A4 */ ProcFunc_hr mCurrActionFunc;
    /* 0x7AC */ be<s8> mTexPatternIdx;
    /* 0x7AD */ be<u8> mMsgAnmIdx;
    /* 0x7AE */ be<u8> mAnmLoopCount;
    /* 0x7AF */ be<u8> mOrderFlags;
    /* 0x7B0 */ be<s8> mState;
    /* 0x7B1 */ be<s8> mReturnState;
    /* 0x7B2 */ be<s8> mType;
    /* 0x7B3 */ be<s8> mActionStatus;
    /* 0x7B4 */ be<s8> mTalkState;
    /* 0x7B5 */ u8 _7B5[3];
    /* 0x7B8 */ be<s32> mStaffIdx;
    /* 0x7BC */ be<u32> mMsgNo;
    /* 0x7C0 */ be<f32> mScaleFactor;
    /* 0x7C4 */ u8 _7C4[4];
    /* 0x7C8 */ be<s16> mMoveTimer;
    /* 0x7CA */ be<s16> mBrakeTime;
    /* 0x7CC */ cXyz mTargetPos;
    /* 0x7D8 */ be<s16> mTargetAngle;
    /* 0x7DA */ be<u8> mEvFlags;
    /* 0x7DB */ u8 _7DB;
    /* 0x7DC */ cXyz mPrevAntennaPos;
    /* 0x7E8 */ cXyz mAntennaVelocity;
    /* 0x7F4 */ cXyz mAntennaOffset;
    /* 0x800 */ cXyz mAntennaRotation;
    /* 0x80C */ daNpc_Wind_Clothes mClothes;
};
WWHD_OFFSET(daNpc_Hr_c, mPhs, 0x3C4);
WWHD_OFFSET(daNpc_Hr_c, mBlinkFrame, 0x450);
WWHD_OFFSET(daNpc_Hr_c, mCyl, 0x490);
WWHD_OFFSET(daNpc_Hr_c, mCyl2, 0x5FC);
WWHD_OFFSET(daNpc_Hr_c, m_jnt, 0x72C);
WWHD_OFFSET(daNpc_Hr_c, mSmokeCallBack, 0x78C);
WWHD_OFFSET(daNpc_Hr_c, mCurrActionFunc, 0x7A4);
WWHD_OFFSET(daNpc_Hr_c, mStaffIdx, 0x7B8);
WWHD_OFFSET(daNpc_Hr_c, mEvFlags, 0x7DA);
WWHD_OFFSET(daNpc_Hr_c, mClothes, 0x80C);
WWHD_SIZE(daNpc_Hr_c, 0x940);

// HD: private projections of the emitter, ship and renderer fields used by this actor.
struct HrEmitterHD {
    u8 padding[0x220];
    cXyz mGlobalScale;
    u8 padding22C[0x238 - 0x22C];
    cXyz mGlobalParticleScale;
    be<u8> mPrmR, mPrmG, mPrmB, mGlobalAlpha;
    be<u8> mEnvR, mEnvG, mEnvB;
};
WWHD_OFFSET(HrEmitterHD, mGlobalScale, 0x220);
WWHD_OFFSET(HrEmitterHD, mGlobalParticleScale, 0x238);
WWHD_OFFSET(HrEmitterHD, mPrmR, 0x244);
WWHD_OFFSET(HrEmitterHD, mGlobalAlpha, 0x247);
WWHD_OFFSET(HrEmitterHD, mEnvR, 0x248);
struct HrShipHD : fopAc_ac_c {
    u8 padding3AC[0x635 - 0x3AC];
    be<u8> mMode;
    u8 padding636[0x700 - 0x636];
    be<s32> mTornadoId;
};
WWHD_OFFSET(HrShipHD, mMode, 0x635);
WWHD_OFFSET(HrShipHD, mTornadoId, 0x700);
struct HrPlayerHD : fopAc_ac_c {
    u8 padding3AC[0x3B8 - 0x3AC];
    be<u32> mNoResetFlg0;
};
WWHD_OFFSET(HrPlayerHD, mNoResetFlg0, 0x3B8);
struct HrTornadoHD : fopAc_ac_c {
    u8 padding3AC[8];
    gptr<J3DModel> mpModel;
};
WWHD_OFFSET(HrTornadoHD, mpModel, 0x3B4);
struct HrEnvLightHD {
    u8 padding[0x9FC];
    cXyz mWindVec;
};
WWHD_OFFSET(HrEnvLightHD, mWindVec, 0x9FC);
struct HrModelDataHD {
    u8 padding[0x38];
    gptr<J3DAnmTexPattern> mpTexPattern;
};
WWHD_OFFSET(HrModelDataHD, mpTexPattern, 0x38);
struct HrJointNodeHD {
    J3DJoint mJoint;
    u8 padding[0x10];
};
WWHD_SIZE(HrJointNodeHD, 0x1C);
struct HrJointHD {
    u8 padding[4];
    be<u16> mJntNo;
};
WWHD_OFFSET(HrJointHD, mJntNo, 4);
struct HrMessageManagerHD {
    u8 padding[0x921];
    be<u8> mSendOn;
};
WWHD_OFFSET(HrMessageManagerHD, mSendOn, 0x921);
struct HrJ3DSysHD {
    gptr<J3DModel> mpModel;
};
struct HrMessageStaticsHD {
    be<u32> mMsgId;
};
struct HrMessageManagerPtrHD {
    gptr<HrMessageManagerHD> mpManager;
};
extern HrJ3DSysHD& l_hrJ3DSys;
extern Mtx34& l_hrCurrentMtx;
extern HrMessageStaticsHD& l_hrMessage;
extern HrMessageManagerPtrHD& l_hrMessageManager;
extern cSAngle& l_hrRightAngle;

// HD: virtual table slots retain the GHS two-word shape and exact full-register results.
struct HrVirtualSlotHD {
    be<s32> mThisDelta;
    be<u32> mFunction;
};
WWHD_SIZE(HrVirtualSlotHD, 8);
struct HrVirtualObjectHD {
    gptr<HrVirtualSlotHD> mpVtable;
};
struct HrPlayerVtableHD {
    u8 padding[0x34];
    be<u32> mGetTactTimerCancel;
    u8 padding38[0x114 - 0x38];
    be<u32> mSetPlayerPosAndAngle;
};
WWHD_OFFSET(HrPlayerVtableHD, mGetTactTimerCancel, 0x34);
WWHD_OFFSET(HrPlayerVtableHD, mSetPlayerPosAndAngle, 0x114);
struct HrTexturePatternVtableHD {
    u8 padding[0x14];
    be<u32> mGetFrameMax;
};
struct HrTexturePatternHD {
    u8 padding[4];
    gptr<HrTexturePatternVtableHD> mpVtable;
};
WWHD_OFFSET(HrTexturePatternHD, mpVtable, 4);

/* Unit-local HD bindings. */
#define HR_SAFESTRING_VTBL 0x1001A76C /* this TU's sead::SafeString vtable */
#define HR_VTBL 0x1001A794            /* daNpc_Hr_c vtable (HD virtual destructor) */
#define HR_AAB_VTBL 0x1001A784        /* this TU's cM3dGAab vtable */

/* daShip_c::checkTornadoUp (HD: byte +0x635 == 12), checkTornadoFlg (HD: tornado id +0x700 != -1)
 */
static inline bool daShip_checkTornadoUp(fopAc_ac_c* ship) {
    return ((HrShipHD*)ship)->mMode == 12;
}
static inline bool daShip_checkTornadoFlg(fopAc_ac_c* ship) {
    return ((HrShipHD*)ship)->mTornadoId != -1;
}
/* daPy_py_c::onPlayerNoDraw / offPlayerNoDraw: status word +0x3B8, bit 0x08000000 */
static inline void daPy_onPlayerNoDraw(fopAc_ac_c* player) {
    ((HrPlayerHD*)player)->mNoResetFlg0 |= 0x08000000;
}
static inline void daPy_offPlayerNoDraw(fopAc_ac_c* player) {
    ((HrPlayerHD*)player)->mNoResetFlg0 &= ~0x08000000u;
}
/* the player's virtuals in its HD vtable (+0xB4): +0x114 setPlayerPosAndAngle(cXyz*, s16), +0x34
 * getTactTimerCancel() */
static inline void daPy_setPlayerPosAndAngle(fopAc_ac_c* pl, cXyz* pos, s16 angle) {
    gabi::call_ptr(gabi::at<HrPlayerVtableHD>(pl->__vtbl)->mSetPlayerPosAndAngle, pl, pos, angle);
}
static inline s32 daPy_getTactTimerCancel(fopAc_ac_c* pl) {
    return gabi::call_ptr<s32>(gabi::at<HrPlayerVtableHD>(pl->__vtbl)->mGetTactTimerCancel, pl);
}

/* events */
/* 025D7970 fopAcM_orderChangeEventId(actor, partner, s16 idx, u16 flag, u16 hind) */
BOOL fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind);
/* dEvt_info_c (eventInfo at +0xF8): setEventName 0253E9B0; XyCheckCB +0x104, XyEventCB +0x100 */
void hrEventInfoSetEventName(dEvt_info_c* info, const char* name);
static inline void eventInfo_setEventName(fopAc_ac_c* actor, const char* name) {
    hrEventInfoSetEventName(&actor->eventInfo, name);
}

/* audio (HD: no position) */
void mDoAud_seStart_noPos(u32 id);
void mDoAud_subBgmStop();
void mDoAud_subBgmStart(u32 id);
void mDoAud_bgmAllMute(s32 t);
void mDoAud_tact_reset();
void mDoAud_tact_melodyPlay(s32 i);
void mDoAud_tact_ambientPlay();

/* environment / wind */
void dKyw_tact_wind_set(s16 a, s16 b);
void dKyw_tact_wind_set_go();
cXyz* dKyw_get_wind_vec();
void dKyw_custom_windpower(f32 p);
void dKy_actor_addcol_set(s16 r, s16 g, s16 b, f32 ratio);
void dKy_vrbox_addcol_set(s16 r, s16 g, s16 b, f32 ratio);
/* g_env_light.mWind.mWindVec: envlight + 0x9FC */
static inline void envlight_setWindVec(f32 x, f32 y, f32 z) {
    HrEnvLightHD* light = (HrEnvLightHD*)dKy_getEnvlight();
    light->mWindVec.x = x;
    light->mWindVec.z = z;
    light->mWindVec.y = y;
}
/* cSGlobe / cSAngle */
void cSGlobe_ct(void* g, const cXyz* v);
void cSAngle_ct_copy(be<s16>* a, const be<s16>* b);
void cSAngle_addeq(be<s16>* a, const be<s16>* b);
f32 cSAngle_Cos(const be<s16>* a);
f32 cSAngle_Sin(const be<s16>* a);

/* misc */
void fpoAcM_absolutePos(fopAc_ac_c* a, cXyz* rel, cXyz* out);
fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm);
s32 fopAcM_seenActorAngleY(fopAc_ac_c* a, fopAc_ac_c* b);
void execItemGet(u8 item);
bool hrVectorIsZero(cXyz* v);
void hrVectorNormalize(cXyz* v, cXyz* res);
s16 cM3d_CalcVecZAngle(const cXyz* v, csXyz* out);
s16 cLib_calcTimer(be<s16>* t);
s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr,
                       f32 rate, s32 start, s32 end, u32 modify, s32 entry);
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* pattern) {
    HrTexturePatternVtableHD* vtable = ((HrTexturePatternHD*)pattern)->mpVtable;
    return gabi::call_ptr<s32>(vtable->mGetFrameMax, pattern);
}
/* 0259D454 dNpc_setAnm(morf, loopMode, morf, speed, anmIdx, soundIdx, arc) */
BOOL dNpc_setAnm(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, s32 anmIdx, s32 soundIdx,
                 const char* arc);
void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot,
                               s16 vel, u8 headOnly);
static inline u32 J3DModelData_getJointName(J3DModelData* data) {
    return gabi::ea(data->getJointName());
}
s8 JUTNameTab_getIndex(u32 tab, const char* name);
static inline u16 J3DModelData_getJointNum(J3DModelData* data) {
    return data->getJointNum();
}
/* joint node callback: J3DModelData joint table (+4 count, +8 nodes of 0x1C), callback at +8 */
static inline void J3DModelData_setJointCallBack(J3DModelData* md, u32 i, u32 cb) {
    u32 n = md->mJointNum;
    u32 joint = md->mJointNodes;
    if (i < n)
        joint += i * sizeof(HrJointNodeHD);
    gabi::at<J3DJoint>(joint)->mCallBack = cb;
}
/* HD J3D joint matrices */
static inline Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    return model->getAnmMtx(jntNo);
}
static inline u32 jntNo_of(J3DNode* node) {
    return ((HrJointHD*)J3DNode_toJoint(node))->mJntNo;
}
static inline Mtx34* j3dSys_mCurrentMtx() {
    return &l_hrCurrentMtx;
}
static inline J3DModel* j3dSys_getModel() {
    return l_hrJ3DSys.mpModel;
}

/* HD JPABaseEmitter: global prm colour +0x244, env colour +0x248, alpha +0x247, scale +0x220/+0x238
 */
static inline void JPABaseEmitter_setGlobalScale(JPABaseEmitter* emitter, f32 x, f32 y, f32 z) {
    HrEmitterHD* e = (HrEmitterHD*)emitter;
    e->mGlobalParticleScale.z = z;
    e->mGlobalParticleScale.y = y;
    e->mGlobalParticleScale.x = x;
    e->mGlobalScale.y = y;
    e->mGlobalScale.z = z;
    e->mGlobalScale.x = x;
}

/* GHS pointer to member function call */
static inline void pmf_call_hr(void* self, ProcFunc_hr* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = pmf->mVirtual.mVirtualOffset;
        HrVirtualSlotHD* vtable = gabi::at<HrVirtualObjectHD>(thisp + voff)->mpVtable;
        gabi::call_ptr<BOOL>(vtable[i].mFunction, thisp, arg);
    }
}

/* file statics: l_msgId (HD: l_msg is gone, the message is the message manager) */
static inline be<u32>& l_msgId() {
    return l_hrMessage.mMsgId;
}
/* HD message manager (*(0x101F4B5C)) */
static inline u32 msgMgr() {
    return gabi::ea(l_hrMessageManager.mpManager.get());
}
u32 msgMgr_getStatus(u32 m);
void msgMgr_setStatus(u32 m, u32 st);
u32 msgMgr_messageSet(u32 m, u32 msgNo, cXyz* pos);

/* functions of this translation unit called by address */
f32 hr_colorGamma(u32 c); /* 02241C8C */

// HD: native wide-return and compiler-array bindings retain the verified register ABI.
f32 hrPowf(f32 value, f32 exponent);
void hrActorConstruct(fopAc_ac_c* actor);
void hrJointControllerConstruct(dNpc_JntCtrl_c* controller);
void hrConstructArray(void* array, u32 count, u32 size, u32 callback);
void hrDestroyArray(void* array, u32 count, u32 size, u32 callback, u32 p4, u32 p5);
void hrActorDestroy(fopAc_ac_c* actor, s32 flags);
f64 hrAddCalcWide(be<f32>* value, f32 target, f32 scale, f32 maximum, f32 minimum);
f64 hrVectorSquareMagWide(const cXyz* vector);
f64 hrSqrtWide(f64 value);
u32 hrDispatch_demoProcWind(daNpc_Hr_c* actor, int mode);
u32 hrDispatch_demoProcMove(daNpc_Hr_c* actor);
u32 hrDispatch_wait01(daNpc_Hr_c* actor);
u32 hrDispatch_talk01(daNpc_Hr_c* actor);
u32 hrDispatch_rt_search(daNpc_Hr_c* actor);
u32 hrDispatch_rt_hide(daNpc_Hr_c* actor);
u32 hrDispatch_rt_intro(daNpc_Hr_c* actor);
u32 hrDispatch_rt_angry(daNpc_Hr_c* actor);
u32 hrDispatch_rt_win(daNpc_Hr_c* actor);
u32 hrDispatch_rt_hit0(daNpc_Hr_c* actor);
u32 hrDispatch_rt_hit1(daNpc_Hr_c* actor);
u32 hrDispatch_ht_hide(daNpc_Hr_c* actor);
s16 daNpc_hr_XyCheckCB(void* actor, int button);
s16 daNpc_hr_XyEventCB(void* actor, int button);
daNpc_Wind_Eff* daNpc_Wind_Eff_ct(daNpc_Wind_Eff* effect);
void daNpc_Wind_Eff_dt(daNpc_Wind_Eff* effect, s32 flags);

BOOL hrWaitAction(daNpc_Hr_c* actor, void* arg);

WWHD_SIZE(HrEmitterHD, 0x24B);
WWHD_SIZE(HrShipHD, 0x704);
WWHD_SIZE(HrPlayerHD, 0x3BC);
WWHD_SIZE(HrTornadoHD, 0x3B8);
WWHD_SIZE(HrEnvLightHD, 0xA08);
WWHD_SIZE(HrJointHD, 0x6);
WWHD_SIZE(HrMessageManagerHD, 0x922);
WWHD_SIZE(HrJ3DSysHD, 0x4);
WWHD_SIZE(HrMessageStaticsHD, 0x4);
WWHD_SIZE(HrMessageManagerPtrHD, 0x4);
WWHD_SIZE(HrPlayerVtableHD, 0x118);
WWHD_SIZE(HrTexturePatternVtableHD, 0x18);
WWHD_SIZE(HrTexturePatternHD, 0x8);
WWHD_SIZE(HrModelDataHD, 0x3C);
WWHD_OFFSET(ProcFunc_hr, d, 0);
WWHD_OFFSET(ProcFunc_hr, i, 2);
WWHD_OFFSET(ProcFunc_hr, f, 4);
