/**
 * d_a_obj_mknjd.cpp (WWHD)
 * Object - Earth God's Lyric / Wind God's Aria statues (MknjD).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_mknjd.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x1002CC08)  /* "MknjD" */
#define SAFESTRING_VTBL 0x1002C97C /* this TU's sead::SafeString vtable */
#define ACT_VTBL 0x1002CC10        /* HD: daObjMknjD::Act_c vtable */
#define M_tmp_mtx gabi::at<Mtx34>(0x1046A618)
/* static u16 joint_number_table[20] */
#define JOINT_NUMBER_TABLE 0x1046A5E0u
/* daObjMknjD_jointName[20] ("Hahen1".."Hahen20"), daObjMknjD_EventName[8], cut_name_tbl[9] */
#define JOINT_NAME_TBL 0x101CAEF4u
#define EVENT_NAME_TBL 0x101CAF8Cu
#define CUT_NAME_TBL 0x101CAF44u
static inline const char* EventName(int i) { return STR(gabi::load<u32>(EVENT_NAME_TBL + 4 * i)); }

/* nodeCallBack addresses (stored into the joints) and the XY callbacks (eventInfo) */
#define NODECALLBACK_L 0x023722E4u
#define NODECALLBACK_R 0x02372410u
#define NODECALLBACK_HAHEN 0x0237253Cu
#define XYCHECKCB 0x023726C8u
#define XYEVENTCB 0x023726D4u
#define SMOKECB_CT 0x023726D8u
#define SMOKECB_DT 0x02374A4Cu

enum {
    dRes_INDEX_MKNJD_BDL_MKNJD_e = 4,
    dRes_INDEX_MKNJD_BDL_MKNJH_e = 5,
    dRes_INDEX_MKNJD_BDL_MKNJK_e = 6,
    dRes_INDEX_MKNJD_DZB_MKNJD_e = 9,
};
enum { ACT_SETGOAL, ACT_SETANGLE, ACT_WAIT, ACT_INPUT, ACT_BREAK, ACT_HIDE_LINK, ACT_DISP_LINK, ACT_LESSON, ACT_TACT };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025D7874 fopAcM_orderChangeEventId(actor, s16 eventIdx, u16 flag, u16 hind) */
static inline void fopAcM_orderChangeEventId(fopAc_ac_c* a, s16 idx, u16 flag, u16 hind) { gabi::call(0x025D7874, a, idx, flag, hind); }
/* 0253E9B0 dEvt_info_c::setEventName(char*) (eventInfo at actor+0xF8) */
static inline void eventInfo_setEventName(fopAc_ac_c* a, const char* name) { gabi::call(0x0253E9B0, gabi::ea(a) + 0xF8, name); }
/* 0254DA50 checkItemGet(u8 item, BOOL) */
static inline BOOL checkItemGet(u8 item, s32 flag) { return gabi::call<BOOL>(0x0254DA50, item, flag); }
/* 025E18EC mDoAud_bgmStart(id), 025E1904 mDoAud_bgmStop(time), 025E1E88 mDoAud_taktModeMuteOff() */
static inline void mDoAud_bgmStart(u32 id) { gabi::call(0x025E18EC, id); }
static inline void mDoAud_bgmStop(u32 t) { gabi::call(0x025E1904, t); }
static inline void mDoAud_taktModeMuteOff() { gabi::call(0x025E1E88); }
/* 025E1988 HD: mDoAud_seStart(id) without position (as in d_a_obj_mkiek) */
static inline void mDoAud_seStart_1(u32 id) { gabi::call(0x025E1988, id); }
/* 02542EDC dEvent_manager_c::getMyActIdx(staff, tbl, n, force, nameType) -> s8 */
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, s32 force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* 02543714 dEvent_manager_c::setGoal(cXyz*) */
static inline void dComIfGp_evmng_setGoal(cXyz* pos) { gabi::call(0x02543714, dComIfGp_getPEvtManager(), pos); }
/* 025B8B68 dSv_event_c::onEventBit (save info + 0x624 = *(0x101F84DC) + 0x644) */
static inline void dComIfGs_onEventBit(u16 flag) { gabi::call(0x025B8B68, gabi::load<u32>(0x101F84DC) + 0x644, flag); }
/* fopAcM_SearchByName(procName): fopAcIt_Judge(fpcSch_JudgeForPName 025E121C, &name) */
static inline fopAc_ac_c* fopAcM_SearchByName(be<s16>* slot, s16 name) {
    *slot = name;
    return fopAcIt_Judge(0x025E121C, slot);
}
/* fopAcM_onDraw: fopDwTg_ToDrawQ(&draw_tag, fpcM_DrawPriority(a)) (025DF2B8, 025DA874) */
static inline void fopAcM_onDraw(fopAc_ac_c* a) {
    s16 prio = gabi::call<s16>(0x025DF2B8, a);
    gabi::call(0x025DA874, gabi::ea(a) + 0xDC, prio);
}
/* dComIfGp_event_setTalkPartner: dEvt_control_c (play+0x51D0) mPtTalk (+0xCC) = getPId (0253F124) */
static inline void dComIfGp_event_setTalkPartner(fopAc_ac_c* a) {
    u32 evt = dComIfGp_ea() + 0x51D0;
    gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, a));
}
/* dComIfGp_checkPlayerStatus1(0, flag): play+0x5CDC */
static inline bool dComIfGp_checkPlayerStatus1_0(u32 f) { return (gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & f) != 0; }
/* dComIfGp_getCb1Player(): play+0x5B38 */
static inline fopAc_ac_c* dComIfGp_getCb1Player() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B38)); }
/* dComIfGp_att_offAleart / revivalAleart (HD): bit 31 of play+0x5824 */
static inline void dComIfGp_att_offAleart() { u32 p = dComIfGp_ea() + 0x5824; gabi::store<u32>(p, gabi::load<u32>(p) | 0x80000000u); }
static inline void dComIfGp_att_revivalAleart() { u32 p = dComIfGp_ea() + 0x5824; gabi::store<u32>(p, gabi::load<u32>(p) & 0x7FFFFFFFu); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
/* HD: a dScnKy_env_light_c flag at +0x10A4 (set while the Wind statue's demo runs in "kaze") */
static inline void envLight_set10A4(u8 v) { gabi::store<u8>(gabi::ea(dKy_getEnvlight()) + 0x10A4, v); }
/* daPy_py_c::onPlayerNoDraw / offPlayerNoDraw: mNoResetFlg0 (+0x3B8) bit 0x08000000 */
static inline void player_onNoDraw(fopAc_ac_c* p) { u32 a = gabi::ea(p) + 0x3B8; gabi::store<u32>(a, gabi::load<u32>(a) | 0x08000000u); }
static inline void player_offNoDraw(fopAc_ac_c* p) { u32 a = gabi::ea(p) + 0x3B8; gabi::store<u32>(a, gabi::load<u32>(a) & ~0x08000000u); }
/* HD message manager (*(0x101F4B5C)): 025F795C status, 025F74D0 setStatus, 025F7DB0 messageSet */
static inline u32 msgMng_getStatus(u32 mgr) { return gabi::call<u32>(0x025F795C, mgr); }
static inline void msgMng_setStatus(u32 mgr, u32 st) { gabi::call(0x025F74D0, mgr, st); }
static inline u32 msgMng_messageSet(u32 mgr, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mgr, msgNo, pos); }
/* J3DModelData (HD): joint names (027F68FC header, relative offset at +0x10), joint count
 * (027F3F94 tree, u16 at +8), joint nodes of 0x1C bytes at +8 (index checked against +4) */
static inline u32 J3DModelData_getJointName(u32 d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s32 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum(u32 d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline u32 J3DModelData_getJointNode(u32 d, u32 i) {
    u32 n = gabi::load<u32>(d + 4);
    u32 p = gabi::load<u32>(d + 8);
    if (i < n) p += i * 0x1C;
    return p;
}
static inline u32 J3DModel_getModelData(u32 m) { return gabi::load<u32>(m + 0xAC); }
/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices in a block at +0x2C (+0x4 flags, +0x10 matrices) */
static inline u32 getAnmMtx(u32 model, u32 jntNo) {
    u32 blk = gabi::load<u32>(model + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10)); /* HD: dirty flag */
    return gabi::load<u32>(blk + 0x10) + jntNo * 0x30;
}
/* sead::SafeString equality (HD inline, as in d_a_kb) */
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
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (HD: SafeString comparison; play+0x5134) */
static inline bool dComIfGp_isStartStage(u32 lit) {
    gabi::Local<SafeString> a;
    a->mStringTop = lit;
    a->__vtbl = SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mStringTop = dComIfGp_ea() + 0x5134;
    b->__vtbl = SAFESTRING_VTBL;
    return SafeString_eq(a, b);
}
/* dPa_smokeEcallBack::remove(): virtual (+0x44) */
static inline void smokeCB_remove(u32 cb) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(cb) + 0x44), cb); }

namespace daObjMknjD {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_SWITCH_W = 8, PRM_SWITCH_S = 0, PRM_TYPE_W = 1, PRM_TYPE_S = 0x10 };
    s32 prm_get_swSave();
    u8 prm_get_Type();
    s16 XyCheckCB(int);
    s16 XyEventCB(int);
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    BOOL Delete();
    void set_mtx();
    void init_mtx();
    void setGoal(int);
    void setPlayerAngle(int);
    u16 talk(int);
    void privateCut();
    bool daObjMknjD_break();
    BOOL Execute(Mtx34**);
    BOOL Draw();
    u32 smokeCB(int i) { return gabi::ea(this) + 0x568 + 0x20 * i; }
    s16 tevStr_C0(int i) { return gabi::load<s16>(gabi::ea(this) + 0x1A0 + 2 * i); }

    /* 0x3E0 */ cXyz mLeftHalfPos;
    /* 0x3EC */ cXyz mRightHalfPos;
    /* 0x3F8 */ cXyz mShardPositions[0x14];
    /* 0x4E8 */ be<f32> mShardHeights[0x14];
    /* 0x538 */ request_of_phase_process_class mPhs;
    /* 0x540 */ gptr<J3DModel> mMainMdl;
    /* 0x544 */ gptr<J3DModel> mBreakMdl;
    /* 0x548 */ be<u16> m0430;
    /* 0x54A */ be<u16> m0432;
    /* 0x54C */ be<u16> m0434;
    /* 0x54E */ u8 _54E[2];
    /* 0x550 */ be<s32> mBreakTimer;
    /* 0x554 */ be<u8> mMainMdlAlpha;
    /* 0x555 */ be<u8> m043D;
    /* 0x556 */ be<u8> m043E;
    /* 0x557 */ be<u8> m043F;
    /* 0x558 */ gptr<JPABaseEmitter> mEmitters[4];
    /* 0x568 */ u8 mSmokeCBs[4][0x20]; /* dPa_smokeEcallBack (emitter at +4, rate-off flag at +0x11) */
    /* 0x5E8 */ cXyz mBrokenPos;
    /* 0x5F4 */ be<s16> mCheckEventIdx;
    /* 0x5F6 */ be<s16> mDemoEventIdx;
    /* 0x5F8 */ be<s16> mErrorEventIdx;
    /* 0x5FA */ be<s16> mLessonEventIdx;
    /* 0x5FC */ be<s8> mActionIdx;
    /* 0x5FD */ be<u8> mMelodyNum;
    /* 0x5FE */ be<u8> mGiveItemNo;
    /* 0x5FF */ u8 _5FF;
    /* 0x600 */ cXyz mGoalPos;
    /* 0x60C */ be<u32> mMsgNo;
    /* 0x610 */ be<s32> mMsgPID;
    /* 0x614 */ be<u8> mMsgFound; /* HD: replaces msg_class* mMsgPtr (the message manager is global) */
    /* 0x615 */ u8 _615[3];
    /* 0x618 */ be<s32> m0500;
    /* 0x61C */ be<u8> m0504;
    /* 0x61D */ u8 _61D[3];
};
WWHD_OFFSET(Act_c, mShardHeights, 0x4E8);
WWHD_OFFSET(Act_c, mMainMdl, 0x540);
WWHD_OFFSET(Act_c, mSmokeCBs, 0x568);
WWHD_OFFSET(Act_c, mBrokenPos, 0x5E8);
WWHD_OFFSET(Act_c, mGoalPos, 0x600);
WWHD_OFFSET(Act_c, m0504, 0x61C);
WWHD_SIZE(Act_c, 0x620);
}  // namespace daObjMknjD
using daObjMknjD::Act_c;

/* 02374AE0: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x02374AE0, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x02374AE0, PrmAbstract);
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWITCH_W, PRM_SWITCH_S); }
u8 Act_c::prm_get_Type() { return (u8)PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S); }

/* 023722E4 */
static BOOL nodeCallBackL(J3DNode* i_node, int calcTiming) {
    WWHD_FUNC(0x023722E4, BOOL, i_node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(i_node);
        u32 mdl = gabi::load<u32>(0x104B462C);
        Act_c* actor = gabi::at<Act_c>(gabi::load<u32>(mdl + 0xB8));
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (actor != nullptr) {
            PSMTXCopy(gabi::at<Mtx34>(getAnmMtx(mdl, jntNo)), calc_mtx());
            MtxTrans(actor->mLeftHalfPos.x, actor->mLeftHalfPos.y, actor->mLeftHalfPos.z, 1);
            mtx_copy(gabi::at<Mtx34>(getAnmMtx(mdl, jntNo)), calc_mtx());
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));
        }
    }
    return TRUE;
}
VERIFY(0x023722E4, nodeCallBackL);

/* 02372410 */
static BOOL nodeCallBackR(J3DNode* i_node, int calcTiming) {
    WWHD_FUNC(0x02372410, BOOL, i_node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(i_node);
        u32 mdl = gabi::load<u32>(0x104B462C);
        Act_c* actor = gabi::at<Act_c>(gabi::load<u32>(mdl + 0xB8));
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (actor != nullptr) {
            PSMTXCopy(gabi::at<Mtx34>(getAnmMtx(mdl, jntNo)), calc_mtx());
            MtxTrans(actor->mRightHalfPos.x, actor->mRightHalfPos.y, actor->mRightHalfPos.z, 1);
            mtx_copy(gabi::at<Mtx34>(getAnmMtx(mdl, jntNo)), calc_mtx());
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));
        }
    }
    return TRUE;
}
VERIFY(0x02372410, nodeCallBackR);

/* 0237253C */
static BOOL nodeCallBack_Hahen(J3DNode* i_node, int calcTiming) {
    WWHD_FUNC(0x0237253C, BOOL, i_node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(i_node);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (!((u32)(jntNo - 1) < 20)) /* HD: JUT_ASSERT(0xBF, (no - 1 >= 0) && (no - 1 < 20)) */
            JUT_ASSERT_fail(STR(0x1002CA14), 0xBF, STR(0x1002CA28));
        u32 mdl = gabi::load<u32>(0x104B462C);
        Act_c* actor = gabi::at<Act_c>(gabi::load<u32>(mdl + 0xB8));
        u16 shardIdx = gabi::load<u16>(JOINT_NUMBER_TABLE + 2 * (jntNo - 1));
        if (actor != nullptr) {
            PSMTXCopy(gabi::at<Mtx34>(getAnmMtx(mdl, jntNo)), calc_mtx());
            cXyz& p = actor->mShardPositions[shardIdx];
            MtxTrans(p.x, p.y, p.z, 1);
            mtx_copy(gabi::at<Mtx34>(getAnmMtx(mdl, jntNo)), calc_mtx());
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));
        }
    }
    return TRUE;
}
VERIFY(0x0237253C, nodeCallBack_Hahen);

/* 02372688 */
s16 Act_c::XyCheckCB(int i_itemBtn) {
    WWHD_FUNC(0x02372688, s16, this, i_itemBtn);
    return dComIfGp_getSelectItem(i_itemBtn) == 0x22 /* dItemNo_WIND_WAKER_e */;
}
VERIFY(0x02372688, &Act_c::XyCheckCB);

/* 023726C8 */
static s16 daObjMknjD_XyCheckCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x023726C8, s16, i_this, i_itemBtn);
    return static_cast<Act_c*>(i_this)->XyCheckCB(i_itemBtn);
}
VERIFY(0x023726C8, daObjMknjD_XyCheckCB);

/* 023726CC */
s16 Act_c::XyEventCB(int) {
    WWHD_FUNC(0x023726CC, s16, this, (s32)0);
    return mLessonEventIdx;
}
VERIFY(0x023726CC, &Act_c::XyEventCB);

/* 023726D4 */
static s16 daObjMknjD_XyEventCB(void* i_this, int i_param) {
    WWHD_FUNC(0x023726D4, s16, i_this, i_param);
    return static_cast<Act_c*>(i_this)->XyEventCB(i_param);
}
VERIFY(0x023726D4, daObjMknjD_XyEventCB);

/* 023726D8: array element constructor dPa_smokeEcallBack(1) (for __construct_array) */
static u32 smokeEcallBack_ct(void* p) {
    WWHD_FUNC(0x023726D8, u32, p);
    return gabi::call<u32>(0x025A5B18, p, 1);
}
VERIFY(0x023726D8, smokeEcallBack_ct);

/* 023726E0: daObjMknjD::Act_c::Act_c (inline member constructors; allocates when this == NULL) */
static Act_c* Act_c_ct(Act_c* p) {
    WWHD_FUNC(0x023726E0, Act_c*, p);
    if (p == nullptr) {
        p = (Act_c*)operator_new(0x620);
        if (p == nullptr)
            return p;
    }
    dBgS_MoveBgActor::ct(p);
    p->__vtbl = ACT_VTBL;
    gabi::call(0x028EFFD0, p->mSmokeCBs, 4, 0x20, SMOKECB_CT); /* __construct_array */
    return p;
}
VERIFY(0x023726E0, Act_c_ct);

/* 0237274C */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x0237274C, cPhs_State, this);
    /* fopAcM_ct(this, daObjMknjD::Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0)
            Act_c_ct(this);
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    m043E = prm_get_Type();
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        mEmitters[2] = nullptr;
        mEmitters[3] = nullptr;
        return 3 /* cPhs_STOP_e */;
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_MKNJD_DZB_MKNJD_e, 0, 0x65A0);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT (HD line 0x1EF) */
            JUT_ASSERT_fail(STR(0x1002CA4C), 0x1EF, STR(0x1002CA60));
    }
    return phase_state;
}
VERIFY(0x0237274C, &Act_c::Mthd_Create);

/* 02372884 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x02372884, BOOL, this);
    BOOL result = MoveBGDelete();
    if (gabi::load<u8>(gabi::ea(this) + 0xD) != 3 /* cPhs_STOP_e */) /* fpcM_CreateResult */
        dComIfG_resDelete(&mPhs, M_arcname);                       /* dComIfG_resDeleteDemo */
    return result;
}
VERIFY(0x02372884, &Act_c::Mthd_Delete);

/* inline strcmp (GHS: byte loop) */
static inline s32 strcmp_l(u32 a, u32 b) {
    for (;;) {
        u8 c = gabi::load<u8>(a++);
        u8 d = gabi::load<u8>(b++);
        if (c != d) return (s32)c - (s32)d;
        if (c == 0) return 0;
    }
}

/* 023728DC: HD: the main model's halves are found with JUTNameTab::getIndex; the shard table
 * store is bounds checked; an env-light flag is cleared */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x023728DC, BOOL, this);
    J3DModelData* model_data_d;
    if (m043E == 1) {
        model_data_d = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MKNJD_BDL_MKNJK_e, SAFESTRING_VTBL);
    } else {
        model_data_d = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MKNJD_BDL_MKNJD_e, SAFESTRING_VTBL);
    }
    J3DModelData* model_data_h = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MKNJD_BDL_MKNJH_e, SAFESTRING_VTBL);
    if (model_data_d == nullptr) /* JUT_ASSERT(0x128, model_data_d != NULL) */
        JUT_ASSERT_fail(STR(0x1002CAB4), 0x128, STR(0x1002CAC8));
    if (model_data_h == nullptr) /* JUT_ASSERT(0x129, model_data_h != NULL) */
        JUT_ASSERT_fail(STR(0x1002CAB4), 0x129, STR(0x1002CADC));

    mMainMdl = mDoExt_J3DModel__create(model_data_d, 0x80000, 0x31000002);
    mBreakMdl = mDoExt_J3DModel__create(model_data_h, 0x80000, 0x11000002);
    if (mMainMdl == nullptr || mBreakMdl == nullptr)
        return FALSE;

    s32 idx = JUTNameTab_getIndex(J3DModelData_getJointName(J3DModel_getModelData(gabi::ea(mMainMdl.get()))), STR(0x1002CAA4) /* "MknjL" */);
    if (idx >= 0) {
        u32 node = J3DModelData_getJointNode(J3DModel_getModelData(gabi::ea(mMainMdl.get())), (u16)idx);
        gabi::store<u32>(node + 8, NODECALLBACK_L);
    }
    idx = JUTNameTab_getIndex(J3DModelData_getJointName(J3DModel_getModelData(gabi::ea(mMainMdl.get()))), STR(0x1002CAAC) /* "MknjR" */);
    if (idx >= 0) {
        u32 node = J3DModelData_getJointNode(J3DModel_getModelData(gabi::ea(mMainMdl.get())), (u16)idx);
        gabi::store<u32>(node + 8, NODECALLBACK_R);
    }
    gabi::store<u32>(gabi::ea(mMainMdl.get()) + 0xB8, gabi::ea(this)); /* setUserArea */

    s32 curTblIdx = 0;
    for (u16 i = 0; i < J3DModelData_getJointNum(J3DModel_getModelData(gabi::ea(mBreakMdl.get()))); i++) {
        u32 node = J3DModelData_getJointNode(J3DModel_getModelData(gabi::ea(mBreakMdl.get())), i);
        for (u16 j = 0; j < 20; j++) {
            u32 joint = gabi::ea(J3DNode_toJoint(gabi::at<J3DNode>(node)));
            u32 off = gabi::load<u32>(joint);
            u32 jntName = off != 0 ? joint + off : 0; /* HD: the joint's own name */
            if (strcmp_l(gabi::load<u32>(JOINT_NAME_TBL + 4 * j), jntName) == 0) {
                u32 n2 = J3DModelData_getJointNode(J3DModel_getModelData(gabi::ea(mBreakMdl.get())), i);
                gabi::store<u32>(n2 + 8, NODECALLBACK_HAHEN);
                if (curTblIdx < 20)
                    gabi::store<u16>(JOINT_NUMBER_TABLE + 2 * curTblIdx, j);
                curTblIdx++;
                break;
            }
        }
    }
    gabi::store<u32>(gabi::ea(mBreakMdl.get()) + 0xB8, gabi::ea(this)); /* setUserArea */
    mMainMdlAlpha = 0xFF;
    envLight_set10A4(0);
    return TRUE;
}
VERIFY(0x023728DC, &Act_c::CreateHeap);

/* 02372C44 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x02372C44, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mMainMdl, mDoMtx_stack_c::get());
    J3DModel_setBaseTRMtx(mBreakMdl, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
}
VERIFY(0x02372C44, &Act_c::set_mtx);

/* 02372D7C */
void Act_c::init_mtx() {
    WWHD_FUNC(0x02372D7C, void, this);
    u32 m = gabi::ea(mMainMdl.get());
    gabi::store<f32>(m + 0xBC, 1.0f);
    gabi::store<f32>(m + 0xC0, 1.0f);
    gabi::store<f32>(m + 0xC4, 1.0f);
    m = gabi::ea(mBreakMdl.get());
    gabi::store<f32>(m + 0xBC, 1.0f);
    gabi::store<f32>(m + 0xC0, 1.0f);
    gabi::store<f32>(m + 0xC4, 1.0f);
    set_mtx();
}
VERIFY(0x02372D7C, &Act_c::init_mtx);

/* 02372DA8 */
BOOL Act_c::Create() {
    WWHD_FUNC(0x02372DA8, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mMainMdl)); /* fopAcM_SetMtx */
    init_mtx();
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mBreakMdl));
    init_mtx();
    fopAcM_setCullSizeBox(this, -400.0f, -1.0f, -400.0f, 400.0f, 405.0f, 400.0f);
    mLeftHalfPos.x = 0.0f; mLeftHalfPos.y = 0.0f; mLeftHalfPos.z = 0.0f;
    mRightHalfPos.x = 0.0f; mRightHalfPos.y = 0.0f; mRightHalfPos.z = 0.0f;
    for (int i = 0; i < 20; i++) {
        mShardPositions[i].x = 0.0f;
        mShardPositions[i].y = 0.0f;
        mShardPositions[i].z = 0.0f;
        mShardHeights[i] = 0.0f;
    }
    m043D = false;
    mBreakTimer = 0;
    for (int i = 0; i < 4; i++)
        mEmitters[i] = nullptr;

    if (m043E == 1) {
        mCheckEventIdx = dComIfGp_evmng_getEventIdx(EventName(3), 0xFF);
        mDemoEventIdx = dComIfGp_evmng_getEventIdx(EventName(1), 0xFF);
        mErrorEventIdx = dComIfGp_evmng_getEventIdx(EventName(5), 0xFF);
        mLessonEventIdx = dComIfGp_evmng_getEventIdx(EventName(7), 0xFF);
        mMelodyNum = 4;
        mGiveItemNo = 0x71; /* dItemNo_WIND_GODS_ARIA_e */
        eventInfo_setEventName(this, STR(0x1002CB04) /* "MKNJD_K_TALK" */);
        m0430 = 0x2910;
    } else {
        mCheckEventIdx = dComIfGp_evmng_getEventIdx(EventName(2), 0xFF);
        mDemoEventIdx = dComIfGp_evmng_getEventIdx(EventName(0), 0xFF);
        mErrorEventIdx = dComIfGp_evmng_getEventIdx(EventName(4), 0xFF);
        mLessonEventIdx = dComIfGp_evmng_getEventIdx(EventName(6), 0xFF);
        mMelodyNum = 3;
        mGiveItemNo = 0x70; /* dItemNo_EARTH_GODS_LYRIC_e */
        eventInfo_setEventName(this, STR(0x1002CB14) /* "MKNJD_D_TALK" */);
        m0430 = 0x2920;
    }
    u32 base = gabi::ea(this);
    gabi::store<u8>(base + 0x389, 0x3D); /* attention_info.distances[TALK] */
    gabi::store<u8>(base + 0x38B, 0x3D); /* attention_info.distances[SPEAK] */
    gabi::store<u32>(base + 0x39C, gabi::load<u32>(base + 0x39C) | 0x20000008);

    if (!checkItemGet(mGiveItemNo, TRUE)) {
        m043F = 8;
        gabi::store<u32>(base + 0x100, XYEVENTCB); /* HD eventInfo: event callback at +8 */
        gabi::store<u32>(base + 0x104, XYCHECKCB); /* check callback at +0xC */
    } else {
        m043F = 0;
    }
    mMsgFound = 0;
    mMsgPID = -1;
    m0504 = false;
    return TRUE;
}
VERIFY(0x02372DA8, &Act_c::Create);

/* 023730B4: HD: Medli's extra model (actor+0x628) follows her draw state */
static void manage_friend_draw(int i_param1) {
    WWHD_FUNC(0x023730B4, void, i_param1);
    gabi::Local<be<s16>[2]> names; /* two adjacent stack temporaries */
    fopAc_ac_c* judgeResult = fopAcM_SearchByName(&(*names)[0], 0x16F /* fpcNm_NPC_MD_e */);
    if (judgeResult != nullptr) {
        if (i_param1 == 1) {
            fopAcM_onDraw(judgeResult);
            u32 p = gabi::load<u32>(gabi::ea(judgeResult) + 0x628);
            if (p != 0)
                gabi::store<u32>(p + 0x254, gabi::load<u32>(p + 0x254) & ~4u);
        } else {
            fopAcM_offDraw(judgeResult);
            u32 p = gabi::load<u32>(gabi::ea(judgeResult) + 0x628);
            if (p != 0)
                gabi::store<u32>(p + 0x254, gabi::load<u32>(p + 0x254) | 4u);
        }
    }
    judgeResult = fopAcM_SearchByName(&(*names)[1], 0x14E /* fpcNm_NPC_CB1_e */);
    if (judgeResult != nullptr) {
        if (i_param1 == 1)
            fopAcM_onDraw(judgeResult);
        else
            fopAcM_offDraw(judgeResult);
    }
}
VERIFY(0x023730B4, manage_friend_draw);

/* 023731D8 */
void Act_c::setGoal(int i_staffIdx) {
    WWHD_FUNC(0x023731D8, void, this, i_staffIdx);
    cXyz* p = (cXyz*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x1002CB24) /* "Posion" */, 1);
    f32 px = p->x, py = p->y, pz = p->z;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), current.angle.y);
    mDoMtx_stack_c::transM(px, py, pz);
    u32 m = gabi::ea(mDoMtx_stack_c::get());
    mGoalPos.x = gabi::load<f32>(m + 0x0C); /* multVecZero */
    mGoalPos.y = gabi::load<f32>(m + 0x1C);
    mGoalPos.z = gabi::load<f32>(m + 0x2C);
    dComIfGp_evmng_setGoal(&mGoalPos);
}
VERIFY(0x023731D8, &Act_c::setGoal);

/* 023732D8: HD: getMyIntegerP is null checked */
void Act_c::setPlayerAngle(int i_staffIdx) {
    WWHD_FUNC(0x023732D8, void, this, i_staffIdx);
    void* p = dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x1002CB2C) /* "angle" */, 3);
    s16 angle = 0;
    if (p != nullptr)
        angle = gabi::load<s16>(gabi::ea(p) + 2);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 a = (s16)(current.angle.y + angle);
    /* daPy_py_c::setPlayerPosAndAngle(cXyz*, s16) (virtual +0x114) */
    gabi::call_ptr(gabi::load<u32>(player->__vtbl + 0x114), player, &player->current.pos, a);
}
VERIFY(0x023732D8, &Act_c::setPlayerAngle);

/* 02373360: HD message manager */
u16 Act_c::talk(int i_param1) {
    WWHD_FUNC(0x02373360, u16, this, i_param1);
    u16 msgStatus = 0xFF;
    u32 mgr = gabi::load<u32>(0x101F4B5C);
    if (mMsgPID == -1) {
        if (i_param1 == 1) {
            /* getMsg() */
            if (m0500 == 0) {
                u8 melody = mMelodyNum;
                gabi::store<u8>(dComIfGp_ea() + 0x5BDA, melody); /* dComIfGp_setMelodyNum */
                mMsgNo = 0x5B3;
            } else {
                mMsgNo = 0x1901;
            }
        }
        mMsgPID = msgMng_messageSet(mgr, mMsgNo, &eyePos);
        if (mMsgPID != -1)
            mMsgFound = 0;
    } else if (mMsgFound != 0) {
        msgStatus = (u16)msgMng_getStatus(mgr);
        if (msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
            if (gabi::load<u8>(dComIfGp_ea() + 0x5BD3) != 0) { /* dComIfGp_checkMesgCancelButton */
                m0504 = true;
                gabi::store<u8>(mgr + 0x921, 1); /* fopMsgM_messageSendOn */
            }
            msgMng_setStatus(mgr, 0x10 /* fopMsgStts_MSG_ENDS_e */);
        } else if (msgStatus == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
            msgMng_setStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
            mMsgPID = -1;
        }
    } else {
        mMsgFound = 1;
    }
    return msgStatus;
}
VERIFY(0x02373360, &Act_c::talk);

/* 023734B4 */
bool Act_c::daObjMknjD_break() {
    WWHD_FUNC(0x023734B4, bool, this);
    bool ret = false;
    mBreakTimer = mBreakTimer + 1;
    if (mBreakTimer < 60) {
        mLeftHalfPos.x = -(f32)((mBreakTimer * 20.0) / 60.0);
        mRightHalfPos.x = (f32)((mBreakTimer * 20.0) / 60.0);
    } else {
        mLeftHalfPos.x = -20.0f;
        mRightHalfPos.x = 20.0f;
    }

    if (mBreakTimer == 1) {
        mEmitters[0] = dComIfGp_particle_set(0x8185 /* ID_AK_SN_WGFLASH00 */, &current.pos, &current.angle);
        gabi::Local<GXColor> color;
        color->r = (u8)tevStr_C0(0);
        color->g = (u8)tevStr_C0(1);
        color->b = (u8)tevStr_C0(2);
        color->a = (u8)tevStr_C0(3);
        s8 roomNo = current.roomNo;
        dPa_control_c* pa = dComIfGp_getParticle();
        mEmitters[1] = dPa_control_set(pa, 4, 0x8186 /* ID_AK_SN_WGROCK00 */, &current.pos, &current.angle, nullptr, 0xFF, nullptr,
                                       roomNo, gabi::at<GXColor>(gabi::ea(this) + 0x1A8) /* tevStr.mColorK0 */, color, nullptr);
        pa = dComIfGp_getParticle();
        mEmitters[2] = dPa_control_set(pa, 2, 0xA187 /* ID_AK_ST_WGSMOKE00 */, &current.pos, &current.angle, nullptr, 0xFF,
                                       gabi::at<dPa_levelEcallBack>(smokeCB(2)), -1, nullptr, nullptr, nullptr);
        gabi::store<u8>(smokeCB(2) + 0x11, 0); /* setRateOff(0) */
        mDoAud_seStart(0x69A8 /* JA_SE_OBJ_SAGE_GATE_CREAK */, &current.pos, 0, dComIfGp_getReverb(current.roomNo));
    } else if (mBreakTimer == 30) {
        mDoAud_seStart(0x69A9 /* JA_SE_OBJ_SAGE_GATE_LIGHT */, &current.pos, 0, dComIfGp_getReverb(current.roomNo));
    } else if (mBreakTimer == 60) {
        m043D = true;
        for (u32 i = 0; i < 20; i++) {
            mShardPositions[i].y = 0.0f;
            mShardPositions[i].x = 0.0f;
            mShardPositions[i].z = 0.0f;
            if (i == 0x02 || i == 0x03 || i == 0x06 || i == 0x07 || i == 0x0A || i == 0x0D || i == 0x0E || i == 0x12 || i == 0x13) {
                mShardPositions[i].x = mShardPositions[i].x + 20.0f;
            } else {
                mShardPositions[i].x = mShardPositions[i].x - 20.0f;
            }
        }
    } else if (mBreakTimer == 160) {
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
        dComIfGp_getVibration_StartShock(6, -0x21, v);
        mDoAud_seStart(0x69AA /* JA_SE_OBJ_SAGE_GATE_BREAK */, &current.pos, 0, dComIfGp_getReverb(current.roomNo));
        f32 y = current.pos.y;
        mBrokenPos.y = y + 350.0f;
        mBrokenPos.x = current.pos.x;
        mBrokenPos.z = current.pos.z;
        dPa_control_c* pa = dComIfGp_getParticle();
        mEmitters[3] = dPa_control_set(pa, 2, 0x2027 /* ID_AK_JT_ELEMENTSMOKE01 */, &mBrokenPos, &current.angle, nullptr, 0xFF,
                                       gabi::at<dPa_levelEcallBack>(smokeCB(3)), -1, nullptr, nullptr, nullptr);
        if (mEmitters[3] != nullptr) {
            gabi::store<f32>(gabi::ea(mEmitters[3].get()) + 0x7C, 0.5f); /* setVolumeSweep */
            gabi::store<u16>(gabi::ea(mEmitters[3].get()) + 0x60, 0x2D);  /* setLifeTime */
            gabi::store<f32>(gabi::ea(mEmitters[3].get()) + 0x34, 50.0f); /* setRate */
            gabi::store<u32>(gabi::ea(mEmitters[3].get()) + 0x5C, 1);     /* setMaxFrame */
            u32 e = gabi::ea(mEmitters[3].get());
            gabi::store<f32>(e + 0x220, 3.0f); /* setGlobalDynamicsScale */
            gabi::store<f32>(e + 0x224, 3.0f);
            gabi::store<f32>(e + 0x228, 3.0f);
            e = gabi::ea(mEmitters[3].get());
            gabi::store<f32>(e + 0x238, 6.0f); /* setGlobalParticleScale */
            gabi::store<f32>(e + 0x23C, 6.0f);
            gabi::store<f32>(e + 0x240, 6.0f);
        }
    } else if (mBreakTimer == 255) {
        ret = true;
    }

    if (mBreakTimer < 60) {
        mMainMdlAlpha = 0xFF;
    } else if (mBreakTimer >= 60 && mBreakTimer < 160) {
        f64 mdlAlpha = ((0xA0 - mBreakTimer) * 255.0) / 100.0;
        if (mdlAlpha < 255.0) {
            mMainMdlAlpha = (u8)gabi::ftoi(mdlAlpha);
        } else {
            mMainMdlAlpha = 0xFF;
        }
    } else {
        mMainMdlAlpha = 0;
    }

    if (mBreakTimer >= 160) {
        int i = 19 - (mBreakTimer - 160);
        if (i < 0)
            i = 0;
        for (; i < 20; i++) {
            mShardHeights[i] = mShardHeights[i] - 2.0f;
            mShardPositions[i].y = mShardPositions[i].y + mShardHeights[i];
        }
    }
    return ret;
}
VERIFY(0x023734B4, &Act_c::daObjMknjD_break);

/* 02373A30 */
void Act_c::privateCut() {
    WWHD_FUNC(0x02373A30, void, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1002CB8C) /* "MknjD" */, nullptr, 0);
    if (staffIdx == -1)
        return;
    mActionIdx = dComIfGp_evmng_getMyActIdx(staffIdx, CUT_NAME_TBL, 9, TRUE, 0);
    if (mActionIdx == -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
        return;
    }
    bool doCutEnd = false;
    if (dComIfGp_evmng_getIsAddvance(staffIdx)) {
        switch (mActionIdx) {
        case ACT_SETGOAL: setGoal(staffIdx); break;
        case ACT_SETANGLE: setPlayerAngle(staffIdx); break;
        case ACT_BREAK: {
            mDoAud_seStart_1(0x806 /* JA_SE_READ_RIDDLE_1 */);
            s32 sw = prm_get_swSave();
            dComIfGs_onSwitch(sw, home.roomNo); /* fopAcM_onSwitch */
            dBgW* bgw = mpBgW;
            mBreakTimer = 0;
            if (bgw != nullptr && dBgW_ChkUsed(bgw)) {
                dBgS* bgs = dComIfG_Bgsp();
                cBgS_Release(bgs, mpBgW);
            }
            break;
        }
        case ACT_HIDE_LINK: player_onNoDraw(player); break;
        case ACT_DISP_LINK: player_offNoDraw(player); break;
        case ACT_LESSON:
            m0504 = false;
            m0500 = 0;
            mMsgPID = -1;
            break;
        }
    }
    switch (mActionIdx) {
    case ACT_INPUT:
        if (talk(1) == 0x12 /* fopMsgStts_BOX_CLOSED_e */)
            doCutEnd = true;
        break;
    case ACT_BREAK:
        if (daObjMknjD_break() == true) {
            if (dComIfGp_isStartStage(0x1002CB94 /* "Ekaze" */) || dComIfGp_isStartStage(0x1002CB84 /* "Edaichi" */)) {
                mDoAud_bgmStart(0x80000057 /* JA_BGM_JABOO_CAVE */);
            } else if (m043E == 1) {
                mDoAud_bgmStart(0x8000002F /* JA_BGM_D_WIND */);
            } else {
                mDoAud_bgmStart(0x8000002D /* JA_BGM_D_EARTH */);
            }
            if (gabi::load<u32>(smokeCB(2) + 4) != 0) /* getEmitter() */
                smokeCB_remove(smokeCB(2));
            if (gabi::load<u32>(smokeCB(3) + 4) != 0)
                smokeCB_remove(smokeCB(3));
            doCutEnd = true;
        }
        break;
    case ACT_LESSON: {
        u16 msgStatus = talk(1);
        if (msgStatus == 0x12 /* fopMsgStts_BOX_CLOSED_e */ || msgStatus == 0x15 /* fopMsgStts_INPUT_e */)
            doCutEnd = true;
        break;
    }
    case ACT_TACT:
        if (m0504 == false)
            doCutEnd = true;
        else
            dComIfGp_event_reset();
        break;
    default:
        doCutEnd = true;
        break;
    }
    if (doCutEnd)
        dComIfGp_evmng_cutEnd(staffIdx);
}
VERIFY(0x02373A30, &Act_c::privateCut);

/* 02373EA8 */
BOOL Act_c::Execute(Mtx34** i_mtx) {
    WWHD_FUNC(0x02373EA8, BOOL, this, i_mtx);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* partner = dComIfGp_getCb1Player();

    switch ((u8)m043F) {
    case 0:
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 1 /* checkCommandTalk */) {
            m0500 = 1;
            m043F = 0x0B;
        } else if (player != nullptr) {
            gabi::Local<cXyz> partnerDiff;
            cXyz_mi(&current.pos, partnerDiff, &player->current.pos);
            s16 rotDiff = (s16)(cM_atan2s(partnerDiff->x, partnerDiff->z) - current.angle.y);
            gabi::Local<cXyz> xz;
            xz->y = 0.0f;
            xz->x = partnerDiff->x;
            xz->z = partnerDiff->z;
            f32 absXZ = std_sqrtf(PSVECSquareMag(xz));
            if (absXZ < 800.0f && (rotDiff < -0x4000 || rotDiff > 0x4000)) {
                if (dComIfGp_checkPlayerStatus1_0(1 /* daPyStts1_WIND_WAKER_CONDUCT_e */)) {
                    m043F = 1;
                    manage_friend_draw(0);
                }
            }
        }
        break;
    case 1: {
        /* daPy_py_c::setTactZev(procId, melody, eventName) (virtual +0x84) */
        u32 fn = gabi::load<u32>(player->__vtbl + 0x84);
        u32 id = gabi::load<u32>(gabi::ea(this) + 4);
        if (m043E == 1)
            gabi::call_ptr(fn, player, id, 4, EventName(3));
        else
            gabi::call_ptr(fn, player, id, 3, EventName(2));
        m043F = 2;
        break;
    }
    case 2:
        if (dComIfGp_evmng_startCheck(mCheckEventIdx) != 0) {
            if (partner != nullptr && player != nullptr) {
                s16 rotDiff = (s16)(cM_atan2s(current.pos.x - partner->current.pos.x, current.pos.z - partner->current.pos.z) - current.angle.y);
                gabi::Local<cXyz> diff;
                cXyz_mi(&player->current.pos, diff, &partner->current.pos);
                gabi::Local<cXyz> xz;
                xz->x = diff->x;
                xz->y = 0.0f;
                xz->z = diff->z;
                f32 absXZ = std_sqrtf(PSVECSquareMag(xz));
                if (absXZ < 800.0f && (rotDiff < -0x4000 || rotDiff > 0x4000)) {
                    fopAcM_orderChangeEventId(this, mDemoEventIdx, 0, 0xFFFF);
                    dComIfGs_onEventBit(m0430);
                    gabi::Local<be<s16>> name;
                    fopAc_ac_c* judgeResult = fopAcM_SearchByName(name, 0x16F /* fpcNm_NPC_MD_e */);
                    if (judgeResult != nullptr)
                        dComIfGp_event_setTalkPartner(judgeResult);
                    m043F = 6;
                } else {
                    fopAcM_orderChangeEventId(this, mErrorEventIdx, 0, 0xFFFF);
                    m043F = 3;
                    manage_friend_draw(1);
                }
            } else {
                fopAcM_orderChangeEventId(this, mErrorEventIdx, 0, 0xFFFF);
                m043F = 3;
                manage_friend_draw(1);
            }
        } else if (!dComIfGp_checkPlayerStatus1_0(1)) {
            m043F = 0;
            manage_friend_draw(1);
        }
        break;
    case 3:
        if (eventInfo_checkCommandDemoAccrpt(this))
            m043F = 4;
        break;
    case 4:
        if (dComIfGp_evmng_endCheck(mErrorEventIdx)) {
            dComIfGp_event_reset();
            m043F = 5;
        }
        break;
    case 5:
        if (!dComIfGp_checkPlayerStatus1_0(1))
            m043F = 0;
        break;
    case 6:
        if (eventInfo_checkCommandDemoAccrpt(this)) {
            m043F = 7;
            /* HD: the Wind statue's demo in "kaze" room 12 sets an env-light flag */
            bool kaze = false;
            if (dComIfGp_isStartStage(0x1002CBF4 /* "kaze" */) && gabi::load<s8>(0x1047E6C8) == 0xC)
                kaze = true;
            if (kaze)
                envLight_set10A4(1);
            mDoAud_bgmStop(30);
            mDoAud_taktModeMuteOff();
            dComIfGp_att_offAleart();
            if (m043E == 1) {
                m0432 = 0x2B;
                m0434 = 5;
            } else {
                m0432 = 0x0E;
                m0434 = 5;
            }
        }
        break;
    case 7:
        privateCut();
        if (dComIfGp_evmng_endCheck(mDemoEventIdx)) {
            dComIfGp_att_revivalAleart();
            dComIfGp_event_reset();
            envLight_set10A4(0);
            fopAcM_delete(this);
        }
        break;
    case 8:
        eventInfo_onCondition(this, 0x21 /* dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e */);
        if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 1 /* checkCommandTalk */) {
            if (dComIfGp_event_chkTalkXY()) {
                m0500 = 0;
                m043F = 9;
                mDoAud_seStart(0x8A7 /* JA_SE_PRE_TAKT */, &eyePos, 0, dComIfGp_getReverb(current.roomNo)); /* fopAcM_seStart */
            } else {
                m0500 = 1;
                m043F = 11;
            }
        }
        break;
    case 9:
        m043F = 10;
        break;
    case 10:
        privateCut();
        if (!dComIfGp_event_runCheck()) {
            if (checkItemGet(mGiveItemNo, TRUE))
                m043F = 0;
            else
                m043F = 8;
        }
        break;
    case 11:
        player_onNoDraw(player);
        m043F = 12;
        break;
    case 12:
        if (talk(1) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
            player_offNoDraw(player);
            dComIfGp_event_reset();
            if (checkItemGet(mGiveItemNo, TRUE))
                m043F = 0;
            else
                m043F = 8;
        }
        break;
    }

    set_mtx();
    gabi::store<u32>(gabi::ea(i_mtx), gabi::ea(M_tmp_mtx));

    if (m0432 > 1)
        m0432 = (u16)(m0432 - 1);
    if (m0432 == 1) {
        if (m043E == 1)
            mDoAud_bgmStart(0x80000021 /* JA_BGM_TAKT_MAKORE */);
        else
            mDoAud_bgmStart(0x80000022 /* JA_BGM_TAKT_MEDRI */);
        m0432 = 0;
    }
    if (m0434 > 1)
        m0434 = (u16)(m0434 - 1);
    if (m0434 == 1) {
        manage_friend_draw(1);
        m0434 = 0;
    }
    return TRUE;
}
VERIFY(0x02373EA8, &Act_c::Execute);

/* 023747BC: HD: setMaterial is reduced to the shapes' visibility flag; no sky list */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x023747BC, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, mBreakMdl, &tevStr);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mMainMdl, &tevStr);
    dComIfGd_setListBG();

    u32 mdlData = J3DModel_getModelData(gabi::ea(mMainMdl.get()));
    u16 jointCount = J3DModelData_getJointNum(mdlData);
    for (u16 i = 0; i < jointCount; i++) {
        u32 node = J3DModelData_getJointNode(mdlData, i);
        u32 mat = gabi::load<u32>(node + 0x10); /* getMesh() */
        u8 alpha = mMainMdlAlpha;
        for (; mat != 0; mat = gabi::load<u32>(mat + 4)) /* getNext() */
            gabi::store<u8>(gabi::load<u32>(mat + 8) + 4, alpha != 0); /* getShape()->show()/hide() */
    }
    mDoExt_modelUpdateDL(mMainMdl);
    if (m043D)
        mDoExt_modelUpdateDL(mBreakMdl);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x023747BC, &Act_c::Draw);

/* 02374910 */
BOOL Act_c::Delete() {
    WWHD_FUNC(0x02374910, BOOL, this);
    dComIfGp_att_revivalAleart();
    for (int i = 0; i < 4; i++)
        smokeCB_remove(smokeCB(i));
    return TRUE;
}
VERIFY(0x02374910, &Act_c::Delete);

/* method table entries (HD: tail branches) */
/* 02374978 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x02374978, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x02374978, Mthd_Create);
/* 0237497C */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0237497C, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x0237497C, Mthd_Delete);
/* 02374980 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x02374980, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x02374980, Mthd_Execute);
/* 02374984: virtual Draw */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x02374984, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x02374984, Mthd_Draw);
/* 02374994: virtual IsDelete */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x02374994, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x02374994, Mthd_IsDelete);

/* 023749A4 */
static void __sinit_d_a_obj_mknjd_cpp() {
    WWHD_FUNC(0x023749A4, void, (u32)0);
    sinit_header_statics_z(0x1046A5D4, 0x101CAF68, 0x1046A608);
}
VERIFY(0x023749A4, __sinit_d_a_obj_mknjd_cpp);

/* 02374A38: sead::SafeString deleting destructor (per-TU copy; trivial destructor) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02374A38, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02374A38, SafeString_dt);

/* 02374A4C: dPa_smokeEcallBack destructor (array element, for __destroy_arr) */
static void smokeEcallBack_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02374A4C, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02374A4C, smokeEcallBack_dt);

/* 02374A60: dBgS_MoveBgActor::IsDelete (per-TU copy) */
static BOOL MoveBgActor_IsDelete(Act_c* i_this) {
    WWHD_FUNC(0x02374A60, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02374A60, MoveBgActor_IsDelete);

/* 02374A68: sead::SafeString::assureTerminationImpl_ (per-TU copy; empty) */
static void SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x02374A68, void, p);
}
VERIFY(0x02374A68, SafeString_assureTermination);

/* 02374A6C: daObjMknjD::Act_c deleting destructor (inline member destructors) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x02374A6C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x028F0164, i_this->mSmokeCBs, 4, 0x20, SMOKECB_DT, 0, 0); /* __destroy_arr */
        gabi::call(0x025D50BC, i_this, 0);                                     /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02374A6C, Act_c_dt);
