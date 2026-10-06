/* daNpc_Hr_c (Zephos & Cyclos), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_hr: daNpc_Hr_c derives from
 * fopAc_ac_c directly, so its members are +0x11C up to m_head_tex_pattern; mDoExt_btpAnm grew
 * from 0x14 to 0x74, so everything from mBlinkFrame on is +0x17C up to mProcId; the pointer to
 * member function is 8 bytes (GHS) instead of 12, so the members after it are +0x178.
 * daNpc_Wind_Eff (0x38) and daNpc_Wind_Clothes (0x134) are unchanged. Size 0x940 (GameCube
 * 0x7C8). */
#pragma once
#include "bindings.h"

/* GHS pointer to member function {s16 this delta, s16 vtable index (<0: not virtual), u32
 * function (or s16 vtable offset at +6)} */
struct ProcFunc_hr {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(ProcFunc_hr, 8);

struct daNpc_Wind_Eff {
    enum { WIND_EFF_INACTIVE = 0, WIND_EFF_FADE_IN = 1, WIND_EFF_ACTIVE = 2, WIND_EFF_FADE_OUT = 3 };
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

    bool chkFlag(u16 flag) { return (mFlags & flag) == flag; }
    void clrFlag(u16 flag) { mFlags = (u16)(mFlags & ~flag); }
    void setFlag(u16 flag) { mFlags = (u16)(mFlags | flag); }
    bool chkEvFlag(u8 f) { return (mEvFlags & f) == f; }
    void setEvFlag(u8 f) { mEvFlags = (u8)(mEvFlags | f); }
    void clrEvFlag(u8 f) { mEvFlags = (u8)(mEvFlags & ~f); }
    u32 ChkOrder(u8 f) { return mOrderFlags & f; }
    void SetOrder(u8 f) { mOrderFlags = (u8)(mOrderFlags | f); }
    void ClrOrder() { mOrderFlags = 0; }
    bool isMorf(); /* mDoExt_McaMorf::isMorf: HD mCurMorf (+0xB0) < 1.0 */

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
    /* 0x3DC */ u8 mBtpAnm[0x74]; /* mDoExt_btpAnm (HD 0x74) */
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

/* ---- local bindings (SHARED-CANDIDATE) ---- */
#define HR_SAFESTRING_VTBL 0x1001A76C /* this TU's sead::SafeString vtable */
#define HR_VTBL 0x1001A794            /* daNpc_Hr_c vtable (HD virtual destructor) */
#define HR_AAB_VTBL 0x1001A784        /* this TU's cM3dGAab vtable */

/* save info: dSv_event_c blocks at *(0x101F84DC) + 0x644 (event) and + 0x1178 (tmp event) */
static inline dSv_event_c* hr_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline dSv_event_c* hr_tmpEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(hr_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(hr_event(), f); }
/* 025B8B7C dSv_event_c::offEventBit */
static inline void dComIfGs_offTmpBit(u16 f) { gabi::call(0x025B8B7C, hr_tmpEvent(), f); }
static inline void dComIfGs_onTmpBit(u16 f) { dSv_event_onEventBit(hr_tmpEvent(), f); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
/* dComIfGp_setMelodyNum: play + 0x5BDA (u8) */
static inline void dComIfGp_setMelodyNum(u8 n) { gabi::store<u8>(dComIfGp_ea() + 0x5BDA, n); }
/* dComIfGp_checkMesgCancelButton: play + 0x5BD3 (u8) */
static inline u8 dComIfGp_checkMesgCancelButton() { return gabi::load<u8>(dComIfGp_ea() + 0x5BD3); }
/* dComIfGp_getSelectItem(btn): play + 0x5BBB + btn (u8) */
static inline u8 dComIfGp_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
/* dComIfGp_event_onHindFlag(-1): play + 0x52A6 (u16) = 0xFFFF */
static inline void dComIfGp_event_onHindFlagAll() { gabi::store<u16>(dComIfGp_ea() + 0x52A6, 0xFFFF); }
static inline fopAc_ac_c* dComIfGp_getShipActor() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B3C)); }
/* daShip_c::checkTornadoUp (HD: byte +0x635 == 12), checkTornadoFlg (HD: tornado id +0x700 != -1) */
static inline bool daShip_checkTornadoUp(fopAc_ac_c* s) { return gabi::load<u8>(gabi::ea(s) + 0x635) == 0xC; }
static inline bool daShip_checkTornadoFlg(fopAc_ac_c* s) { return gabi::load<s32>(gabi::ea(s) + 0x700) != -1; }
/* daPy_py_c::onPlayerNoDraw / offPlayerNoDraw: status word +0x3B8, bit 0x08000000 */
static inline void daPy_onPlayerNoDraw(fopAc_ac_c* p) { u32 a = gabi::ea(p) + 0x3B8; gabi::store<u32>(a, gabi::load<u32>(a) | 0x08000000); }
static inline void daPy_offPlayerNoDraw(fopAc_ac_c* p) { u32 a = gabi::ea(p) + 0x3B8; gabi::store<u32>(a, gabi::load<u32>(a) & ~0x08000000u); }
/* the player's virtuals in its HD vtable (+0xB4): +0x114 setPlayerPosAndAngle(cXyz*, s16), +0x34 getTactTimerCancel() */
static inline void daPy_setPlayerPosAndAngle(fopAc_ac_c* pl, cXyz* pos, s16 angle) {
    gabi::call_ptr(gabi::load<u32>(pl->__vtbl + 0x114), pl, pos, angle);
}
static inline s32 daPy_getTactTimerCancel(fopAc_ac_c* pl) { return gabi::call_ptr<s32>(gabi::load<u32>(pl->__vtbl + 0x34), pl); }

/* events */
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 names, s32 n, s32 force, s32 p) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, names, n, force, p);
}
static inline be<s32>* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) {
    return (be<s32>*)dComIfGp_evmng_getMySubstanceP(staffId, name, 3);
}
static inline cXyz* dComIfGp_evmng_getMyXyzP(s32 staffId, const char* name) {
    return (cXyz*)dComIfGp_evmng_getMySubstanceP(staffId, name, 1);
}
static inline be<f32>* dComIfGp_evmng_getMyFloatP(s32 staffId, const char* name) {
    return (be<f32>*)dComIfGp_evmng_getMySubstanceP(staffId, name, 0);
}
static inline s32 dComIfGp_evmng_getMySubstanceNum(s32 staffId, const char* name) {
    return gabi::call<s32>(0x025448CC, dComIfGp_getPEvtManager(), staffId, name);
}
/* 025D7970 fopAcM_orderChangeEventId(actor, partner, s16 idx, u16 flag, u16 hind) */
static inline BOOL fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D7970, a, b, ev, flag, hind);
}
/* dEvt_info_c (eventInfo at +0xF8): setEventName 0253E9B0; XyCheckCB +0x104, XyEventCB +0x100 */
static inline void eventInfo_setEventName(fopAc_ac_c* a, const char* name) { gabi::call(0x0253E9B0, gabi::ea(a) + 0xF8, name); }

/* audio (HD: no position) */
static inline void mDoAud_seStart_noPos(u32 id) { gabi::call(0x025E1988, id); }
static inline void mDoAud_subBgmStop() { gabi::call(0x025E1928); }
static inline void mDoAud_subBgmStart(u32 id) { gabi::call(0x025E1918, id); }
static inline void mDoAud_bgmAllMute(s32 t) { gabi::call(0x025E1960, t); }
static inline void mDoAud_tact_reset() { gabi::call(0x025E1E94); }
static inline void mDoAud_tact_melodyPlay(s32 i) { gabi::call(0x025E1F58, i); }
static inline void mDoAud_tact_ambientPlay() { gabi::call(0x025E1F14); }

/* environment / wind */
static inline void dKyw_tact_wind_set(s16 a, s16 b) { gabi::call(0x0257E490, a, b); }
static inline void dKyw_tact_wind_set_go() { gabi::call(0x0257E52C); }
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
static inline void dKyw_custom_windpower(f32 p) { gabi::call(0x0257E594, p); }
static inline void dKy_actor_addcol_set(s16 r, s16 g, s16 b, f32 ratio) { gabi::call(0x025553B8, r, g, b, ratio); }
static inline void dKy_vrbox_addcol_set(s16 r, s16 g, s16 b, f32 ratio) { gabi::call(0x02556B38, r, g, b, ratio); }
/* g_env_light.mWind.mWindVec: envlight + 0x9FC */
static inline void envlight_setWindVec(f32 x, f32 y, f32 z) {
    u32 e = gabi::ea(dKy_getEnvlight());
    gabi::store<f32>(e + 0x9FC, x);
    gabi::store<f32>(e + 0xA04, z);
    gabi::store<f32>(e + 0xA00, y);
}
/* cSGlobe / cSAngle */
static inline void cSGlobe_ct(void* g, const cXyz* v) { gabi::call(0x02007324, g, v); }
static inline void cSAngle_ct_copy(be<s16>* a, const be<s16>* b) { gabi::call(0x02006644, a, b); }
static inline void cSAngle_addeq(be<s16>* a, const be<s16>* b) { gabi::call(0x020068CC, a, b); }
static inline f32 cSAngle_Cos(const be<s16>* a) { return gabi::call<f32>(0x02006838, a); }
static inline f32 cSAngle_Sin(const be<s16>* a) { return gabi::call<f32>(0x02006814, a); }

/* misc */
static inline void fpoAcM_absolutePos(fopAc_ac_c* a, cXyz* rel, cXyz* out) { gabi::call(0x025DA124, a, rel, out); }
static inline fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm) { return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm); }
static inline s32 fopAcM_seenActorAngleY(fopAc_ac_c* a, fopAc_ac_c* b) { return gabi::call<s32>(0x025D68A0, a, b); }
static inline void execItemGet(u8 item) { gabi::call(0x0254DA38, item); }
static inline bool cXyz_isZero_l(cXyz* v) { return gabi::call<bool>(0x0201B4E0, v); }
static inline void cXyz_normalize_l(cXyz* v, cXyz* res) { gabi::call(0x0201B31C, v, res); }
static inline s16 cM3d_CalcVecZAngle(const cXyz* v, csXyz* out) { return gabi::call<s16>(0x02017300, v, out); }
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
/* 0259D454 dNpc_setAnm(morf, loopMode, morf, speed, anmIdx, soundIdx, arc) */
static inline BOOL dNpc_setAnm(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, s32 anmIdx, s32 soundIdx, const char* arc) {
    return gabi::call<BOOL>(0x0259D454, morf, loopMode, morfF, speed, anmIdx, soundIdx, arc);
}
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
/* joint node callback: J3DModelData joint table (+4 count, +8 nodes of 0x1C), callback at +8 */
static inline void J3DModelData_setJointCallBack(J3DModelData* md, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(gabi::ea(md) + 4);
    u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
    if (i < n)
        joint += i * 0x1C;
    gabi::store<u32>(joint + 8, cb);
}
/* HD J3D joint matrices */
struct J3DMtxBlock_hr {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static inline Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    J3DMtxBlock_hr* blk = gabi::at<J3DMtxBlock_hr>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
static inline u32 jntNo_of(J3DNode* node) { return gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4); }
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline J3DModel* j3dSys_getModel() { return gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); }

/* HD JPABaseEmitter: global prm colour +0x244, env colour +0x248, alpha +0x247, scale +0x220/+0x238 */
static inline void JPABaseEmitter_setGlobalScale(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    u32 b = gabi::ea(e);
    gabi::store<f32>(b + 0x240, z);
    gabi::store<f32>(b + 0x23C, y);
    gabi::store<f32>(b + 0x238, x);
    gabi::store<f32>(b + 0x224, y);
    gabi::store<f32>(b + 0x228, z);
    gabi::store<f32>(b + 0x220, x);
}

/* GHS pointer to member function call */
static inline void pmf_call_hr(void* self, ProcFunc_hr* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}

/* file statics: l_msgId (HD: l_msg is gone, the message is the message manager) */
static inline be<u32>& l_msgId() { return *gabi::at<be<u32>>(0x104671CC); }
/* HD message manager (*(0x101F4B5C)) */
static inline u32 msgMgr() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 msgMgr_getStatus(u32 m) { return gabi::call<u32>(0x025F795C, m); }
static inline void msgMgr_setStatus(u32 m, u32 st) { gabi::call(0x025F74D0, m, st); }
static inline u32 msgMgr_messageSet(u32 m, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, m, msgNo, pos); }

/* functions of this translation unit called by address */
f32 hr_colorGamma(u32 c); /* 02241C8C */
