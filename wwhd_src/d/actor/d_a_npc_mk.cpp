/**
 * d_a_npc_mk.cpp (WWHD)
 * NPC - Ivan (Outset Island, hide-and-seek)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_mk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_mk.h"

#define SAFESTRING_VTBL 0x1001E1E8 /* this TU's sead::SafeString vtable */
#define MK_VTBL 0x1001E240         /* daNpc_Mk_c vtable (HD virtual destructor) */

enum { fpcNm_NPC_MK_e = 0xAA };

/* member functions (guest addresses) used through pointers to member functions */
enum : u32 {
    MK_WAIT_ACTION = 0x02299EAC,
    MK_HIND_ACTION = 0x02299F8C,
    MK_VISIT_ACTION = 0x0229A17C,
    MK_SEEK_ACTION = 0x0229A3C8,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* save info: dSv_event_c blocks at *(0x101F84DC) + 0x644 (event) and + 0x1178 (tmp event) */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline dSv_event_c* dComIfGs_tmpEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline BOOL dComIfGs_isTmpBit(u16 f) { return dSv_event_isEventBit(dComIfGs_tmpEvent(), f); }
static inline void dComIfGs_onTmpBit(u16 f) { dSv_event_onEventBit(dComIfGs_tmpEvent(), f); }
/* 025B8B7C dSv_event_c::offEventBit */
static inline void dComIfGs_offTmpBit(u16 f) { gabi::call(0x025B8B7C, dComIfGs_tmpEvent(), f); }
/* 02520C0C dComIfGs_checkGetItem(u8 itemNo) */
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, (u32)item); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
/* dComIfGp_event_setTalkPartner(a): event control (play+0x51D0) +0xCC = getPId(a) (0253F124) */
static inline void dComIfGp_event_setTalkPartner(fopAc_ac_c* a) {
    u32 evt = dComIfGp_ea() + PLAY_EVTCTRL;
    gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, a));
}
/* dComIfGp_event_setItemPartnerId: dEvt_control_c mPtItem (play + 0x52A0) */
static inline void dComIfGp_event_setItemPartnerId(s32 id) { gabi::store<s32>(dComIfGp_ea() + 0x52A0, id); }
/* 02542EDC dEvent_manager_c::getMyActIdx(staff, names, n, force, p) */
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staff, u32 names, s32 n, s32 force, s32 p) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staff, names, n, force, p);
}
static inline void* dComIfGp_evmng_getMyXyzP(s32 staff, const char* name) { return dComIfGp_evmng_getMySubstanceP(staff, name, 1); }
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staff, const char* name) { return dComIfGp_evmng_getMySubstanceP(staff, name, 3); }
/* 0253E9B0 dEvt_info_c::setEventName (eventInfo at actor + 0xF8) */
static inline void eventInfo_setEventName(fopAc_ac_c* a, const char* name) { gabi::call(0x0253E9B0, gabi::at<u8>(gabi::ea(a) + 0xF8), name); }
static inline s16 eventInfo_getEventId(fopAc_ac_c* a) { return gabi::load<s16>(gabi::ea(a) + 0xFC); }
/* 025D7874 fopAcM_orderChangeEventId(actor, s16 eventIdx, u16 flag, u16 hind) */
static inline BOOL fopAcM_orderChangeEventId(fopAc_ac_c* a, u32 idx, u16 flag, u16 hind) { return gabi::call<BOOL>(0x025D7874, a, idx, flag, hind); }
/* 02543F10 dEvent_manager_c::getEventIdx: its r3 is passed on as is (not re-extended) */
static inline u32 dComIfGp_evmng_getEventIdx_r(const char* name, u8 evNo) {
    return gabi::call<u32>(0x02543F10, dComIfGp_getPEvtManager(), name, evNo);
}
/* 025D7A58 fopAcM_orderOtherEventId(actor, s16 eventIdx, u8 mapToolId, u16, u16 prio, u16 flag) */
static inline BOOL fopAcM_orderOtherEventId_r(fopAc_ac_c* a, u32 idx, u8 tool, u16 p3, u16 prio, u16 flag) {
    return gabi::call<BOOL>(0x025D7A58, a, idx, tool, p3, prio, flag);
}
/* 025D9F38 fopAcM_searchFromName(name, mask, prm) */
static inline fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm) { return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm); }
/* 025D7C6C fopAcM_getTalkEventPartner(actor) */
static inline fopAc_ac_c* fopAcM_getTalkEventPartner(fopAc_ac_c* a) { return gabi::call<fopAc_ac_c*>(0x025D7C6C, a); }
/* 025D7DEC fopAcM_createItemForPresentDemo(pos, itemNo, argFlag, itemBitNo, roomNo, angle, scale) */
static inline s32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return gabi::call<s32>(0x025D7DEC, pos, itemNo, flag, bitNo, roomNo, angle, scale);
}
/* 025E1988 mDoAud_seStart without a position (HD) */
static inline void mDoAud_seStart_noPos(u32 id) { gabi::call(0x025E1988, id); }
/* HD message manager (*(0x101F4B5C)): 025F795C status, 025F74D0 setStatus, 025F7DB0
 * messageSet(msgNo, cXyz* pos) -> id; its select number (GameCube msg_class::mSelectNum) at +0x948 */
static inline u32 msgMgr() { return gabi::load<u32>(0x101F4B5C); }
static inline u16 msgMgr_getStatus(u32 m) { return gabi::call<u16>(0x025F795C, m); }
static inline void msgMgr_setStatus(u32 m, u32 st) { gabi::call(0x025F74D0, m, st); }
static inline u32 msgMgr_messageSet(u32 m, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, m, msgNo, pos); }
/* 0259D454 dNpc_setAnm(morf, loopMode, morf, speed, anmIdx, soundIdx, arc) */
static inline BOOL dNpc_setAnm(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, s32 anmIdx, s32 soundIdx, const char* arc) {
    return gabi::call<BOOL>(0x0259D454, morf, loopMode, morfF, speed, anmIdx, soundIdx, arc);
}
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz (pointers to copies), f32* dist, s16* angle) */
static inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* ang) { gabi::call(0x0259D624, a, b, dist, ang); }
/* dNpc_PathRun_c (MkPath_l) */
static inline void dNpc_PathRun_setInf(MkPath_l* p, u8 path, s8 room, u8 fwd) { gabi::call(0x0259E6D0, p, path, room, fwd); }
static inline u32 dNpc_PathRun_nextPath(MkPath_l* p, s8 room) { return gabi::call<u32>(0x0259E744, p, room); }
static inline void dNpc_PathRun_setInfDrct(MkPath_l* p, u32 path) { gabi::call(0x0259E730, p, path); }
static inline BOOL dNpc_PathRun_chkInside(MkPath_l* p, cXyz* pos) { return gabi::call<BOOL>(0x0259F220, p, pos); }
static inline void dNpc_PathRun_setNearPathIndx(MkPath_l* p, cXyz* pos, f32 r) { gabi::call(0x0259EE7C, p, pos, r); }
/* daNpc_Mk_Static_c (d_a_npc_mk_static, verified separately) */
static inline BOOL MkStatic_walkPath(daNpc_Mk_Static_c* s, fopAc_ac_c* a, MkPath_l* p, u8 o) { return gabi::call<BOOL>(0x0229AB24, s, a, p, o); }
static inline void MkStatic_aroundWalk(daNpc_Mk_Static_c* s, fopAc_ac_c* a, fopAc_ac_c* t, u8 o) { gabi::call(0x0229AB78, s, a, t, o); }
static inline f32 MkStatic_getSpeedF(daNpc_Mk_Static_c* s, f32 a, f32 b) { return gabi::call<f32>(0x0229ACC0, s, a, b); }
static inline void MkStatic_init(daNpc_Mk_Static_c* s, u8 w, u16 d) { gabi::call(0x0229AD60, s, w, d); }
static inline u8 MkStatic_runAwayProc(daNpc_Mk_Static_c* s, fopAc_ac_c* a, MkPath_l* p, void* cyl, be<s16>* ang) {
    return gabi::call<u8>(0x0229B164, s, a, p, cyl, ang);
}
static inline BOOL MkStatic_chkGameSet(daNpc_Mk_Static_c* s) { return gabi::call<BOOL>(0x0229B54C, s); }
static inline void MkStatic_setRndPathPos(daNpc_Mk_Static_c* s, fopAc_ac_c* a, MkPath_l* p) { gabi::call(0x0229B5F0, s, a, p); }
static inline BOOL MkStatic_chkPointPass(daNpc_Mk_Static_c* s, cXyz* a, cXyz* b, cXyz* c) { return gabi::call<BOOL>(0x0229B768, s, a, b, c); }
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
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
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline u32 dBgS_GetMtrlSndId_l(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
/* 025E5580 mDoExt_McaMorf::entry (HD) */
static inline void McaMorf_entry(mDoExt_McaMorf* m) { gabi::call(0x025E5580, m); }
/* mDoLib_clipper::mFar (0x1048D04C) */
static inline f32 mDoLib_clipper_getFar() { return gabi::load<f32>(0x1048D04C); }

/* GHS pointer to member function call */
static inline void pmf_call(void* self, MkActionFunc_l* pmf, void* arg) {
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

/* HD J3D joint matrices (see d_a_npc_ba1.cpp) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
static inline u32 jntNo_of(J3DNode* node) { return gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4); }
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }

/* ---- file statics ---- */
/* l_msgId (HD: l_msg is gone, the message is reached through the message manager) */
static be<u32>& l_msgId() { return *gabi::at<be<u32>>(0x10467C68); }
/* l_bck_ix_tbl[12] (.rodata 0x1001E250), l_btp_ix_tbl[1] (0x1001E1E4), msgAnm table (0x1001E36C),
 * getNowEventAction's action_table (.data 0x101C1F60, 17 names) */

bool daNpc_Mk_c::isMorf() { return gabi::load<f32>(gabi::ea(mpMorf.get()) + 0xB0) < 1.0f; }

/* setAction(actionFunc, NULL) (inline in every caller) */
void daNpc_Mk_c::setAction(u32 actionFunc) {
    MkActionFunc_l* cur = &mCurrActionFunc;
    bool callOld = true;
    s16 i = cur->i;
    if (i == -1) {
        if (cur->d == 0 && cur->f == actionFunc)
            return;
    } else if (i == 0) {
        callOld = false; /* NULL */
    }
    if (callOld) {
        mActionStatus = ACTION_ENDING;
        pmf_call(this, cur, nullptr);
    }
    cur->i = -1;
    cur->f = actionFunc;
    cur->d = 0;
    mActionStatus = ACTION_STARTING;
    gabi::call_ptr<BOOL>(cur->f, this, (u32)0);
}

/* 022955EC */
static BOOL nodeCallBack_Mk(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022955EC, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_Mk_c* i_this = gabi::at<daNpc_Mk_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        u32 jntNo = jntNo_of(node);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
                gabi::Local<cXyz> temp;
                gabi::Local<cXyz> temp2;
                temp->x = 0.0f;
                temp->y = 0.0f;
                temp->z = 0.0f;
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]); /* getHead_y() */
                cMtx_ZrotM(calc_mtx(), i_this->m_jnt.mAngles[0][0]); /* getHead_x() */
                MtxPosition(temp, temp2);
                i_this->mAttnBasePos.x = temp2->x; /* setAttentionBasePos(temp2) */
                i_this->mAttnBasePos.y = temp2->y;
                i_this->mAttnBasePos.z = temp2->z;
                temp->x = 20.0f;
                temp->y = -20.0f;
                temp->z = 0.0f;
                MtxPosition(temp, temp2);
                i_this->mEyePos.x = temp2->x; /* setEyePos(temp2) */
                i_this->mEyePos.y = temp2->y;
                i_this->mEyePos.z = temp2->z;
                if (i_this->mAttnSetCount != 0xFF) { /* incAttnSetCount() */
                    i_this->mAttnSetCount = i_this->mAttnSetCount + 1;
                }
            } else if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]); /* getBackbone_y() */
                cMtx_ZrotM(calc_mtx(), i_this->m_jnt.mAngles[1][0]); /* getBackbone_x() */
            }
            PSMTXCopy(calc_mtx(), j3dSys_mCurrentMtx());
            mtx_copy(getAnmMtx(model, jntNo), calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        }
    }
    return TRUE;
}
VERIFY(0x022955EC, nodeCallBack_Mk);

/* 02295870 */
BOOL daNpc_Mk_c::initTexPatternAnm(u32 i_modify) { /* bool, passed on unnormalised */
    WWHD_FUNC(0x02295870, BOOL, this, i_modify);
    J3DModelData* modelData = J3DModel_getModelData_l(mpMorf->getModel());
    /* HD: l_btp_ix_tbl[0] (not indexed with mTexPatternIdx) */
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(0x1001E290) /* "Mk" */, gabi::load<s32>(0x1001E1E4), SAFESTRING_VTBL);
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(373, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail(STR(0x1001E2B0), 0x175, STR(0x1001E294));
    if (!mDoExt_btpAnm_init(mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, i_modify, FALSE)) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x02295870, &daNpc_Mk_c::initTexPatternAnm);

/* 02295974 */
BOOL daNpc_Mk_c::CreateHeap() {
    WWHD_FUNC(0x02295974, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1001E2D8) /* "Mk" */, 0x13 /* dRes_INDEX_MK_BDL_MK_e */, SAFESTRING_VTBL);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1001E2D8), 0xF /* dRes_INDEX_MK_BCK_MK_WAIT_e */, SAFESTRING_VTBL);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    s8 head = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x1001E2D0) /* "head2" */);
    m_jnt.mHeadJntNum = head;
    if (head < 0) /* JUT_ASSERT(2603, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail(STR(0x1001E2DC), 0xA2B, STR(0x1001E2EC));
    s8 backbone = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x1001E308) /* "backbone" */);
    m_jnt.mBackboneJntNum = backbone;
    if (backbone < 0) /* JUT_ASSERT(2608, m_jnt.getBackboneJntNum() >= 0) */
        JUT_ASSERT_fail(STR(0x1001E2DC), 0xA30, STR(0x1001E314));
    mTexPatternIdx = 0;
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    for (u16 i = 0; i < J3DModelData_getJointNum(modelData); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Mk) */
            J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
            u32 n = gabi::load<u32>(gabi::ea(md) + 4);
            u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
            if ((u32)i < n)
                joint += i * 0x1C;
            gabi::store<u32>(joint + 8, 0x022955EC);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    mAcchCir.SetWall(60.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mVisitMode = 0;
    mObjAcch.SetGroundCheckOffset(100.0f);
    maxFallSpeed = -90.0f; /* fopAcM_SetMaxFallSpeed */
    return TRUE;
}
VERIFY(0x02295974, &daNpc_Mk_c::CreateHeap);

/* 02295C14 (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02295C14, BOOL, i_this);
    return ((daNpc_Mk_c*)i_this)->CreateHeap();
}
VERIFY(0x02295C14, CheckCreateHeap);

/* 02295C18 (matcher: unnamed) */
u8 daNpc_Mk_c::getType() {
    WWHD_FUNC(0x02295C18, u8, this);
    return fopAcM_GetParam(this) & 0xFF;
}
VERIFY(0x02295C18, &daNpc_Mk_c::getType);

/* 02295C24 */
u8 daNpc_Mk_c::getPath() {
    WWHD_FUNC(0x02295C24, u8, this);
    return fopAcM_GetParam(this) >> 8 & 0xFF;
}
VERIFY(0x02295C24, &daNpc_Mk_c::getPath);

/* 02295C30 */
BOOL daNpc_Mk_c::init() {
    WWHD_FUNC(0x02295C30, BOOL, this);
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA);  /* attention_info.flags = ACTION_SPEAK | LOCKON_TALK */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xA9); /* attention_info.distances[TALK] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances[SPEAK] */
    gravity = -3.3f; /* HD: GameCube -30.0 */
    J3DModel* pModel = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
    mpMorf->calc();
    mStts.Init(0xA0, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    actor_status = (actor_status & ~0x3Fu) | 0x27; /* fopAcM_SetStatusMap(this, 0x27); OnStatus(SHOWMAP) */
    switch ((u32)(s32)mType) {
    case TYPE_NORMAL:
        if (dComIfGs_isTmpBit(0x0040) && !dComIfGs_isTmpBit(0x0020)) {
            setAction(MK_HIND_ACTION);
            setFlag(0x10);
            actor_status = actor_status & ~0x20u; /* fopAcM_OffStatus(this, fopAcStts_SHOWMAP_e) */
        } else {
            setAction(MK_VISIT_ACTION);
            if (dComIfGs_isTmpBit(0x0580)) {
                actor_status = actor_status | 0x4000; /* fopAcM_OnStatus(this, fopAcStts_UNK4000_e) */
            }
        }
        dNpc_PathRun_setInf(&field_0x688, getPath(), fopAcM_GetRoomNo(this), true);
        if (field_0x688.path.get() == nullptr) {
            return FALSE;
        }
        dNpc_PathRun_setInfDrct(&field_0x690, dNpc_PathRun_nextPath(&field_0x688, fopAcM_GetRoomNo(this)));
        if (field_0x690.path.get() == nullptr) {
            return FALSE;
        }
        dNpc_PathRun_setInfDrct(&field_0x698, dNpc_PathRun_nextPath(&field_0x690, fopAcM_GetRoomNo(this)));
        if (field_0x698.path.get() == nullptr) {
            return FALSE;
        }
        break;
    case TYPE_MINIGAME:
        mStts.m_weight = 0xFE; /* SetWeight */
        dNpc_PathRun_setInf(&field_0x688, getPath(), fopAcM_GetRoomNo(this), true);
        if (field_0x688.path.get() == nullptr) {
            return FALSE;
        }
        if (dComIfGs_isTmpBit(0x0040) && !dComIfGs_isTmpBit(0x0020)) {
            setAction(MK_SEEK_ACTION);
        } else {
            setAction(MK_HIND_ACTION);
            setFlag(0x10);
            actor_status = actor_status & ~0x20u; /* fopAcM_OffStatus(this, fopAcStts_SHOWMAP_e) */
        }
        actor_status = actor_status | 0x40; /* fopAcM_OnStatus(this, fopAcStts_UNK40_e) */
        break;
    default:
        setAction(MK_WAIT_ACTION);
        break;
    }
    mAttnBasePos.copy(current.pos);
    mEyePos.copy(current.pos);
    if (mDoLib_clipper_getFar() > 1.0f) {
        cullSizeFar = 5000.0f / mDoLib_clipper_getFar(); /* fopAcM_setCullSizeFar */
    }
    mMtrlSndId = 0;
    mReverb = (s8)dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    return TRUE;
}
VERIFY(0x02295C30, &daNpc_Mk_c::init);

/* 022964F8 */
cPhs_State daNpc_Mk_c::_create() {
    WWHD_FUNC(0x022964F8, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Mk_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025D4ED0, this); /* fopAc_ac_c::fopAc_ac_c */
            __vtbl = MK_VTBL;
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mObjAcch, dBgS_ObjAcch_vt{0x1001E210, 0x1001E230, 0x1001E220});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x1001E200);
            gabi::call(0x0259DAA0, &m_jnt); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x1001E34C) /* "Mk" */);
    if (state == cPhs_COMPLEATE_e) {
        mpName = 0x1001E34C; /* "Mk" (the same literal) */
        switch (fpcM_GetName(this)) {
        case fpcNm_NPC_MK_e:
            switch (getType()) {
            case TYPE_NORMAL:
                mType = TYPE_NORMAL;
                break;
            case TYPE_MINIGAME:
                mType = TYPE_MINIGAME;
                argument = 4;
                mpName = 0x1001E348; /* "Mk2" */
                break;
            default:
                mType = TYPE_NONE;
                break;
            }
            break;
        default:
            return cPhs_ERROR_e;
        }
        tevStr.mRoomNo = current.roomNo;
        if (!fopAcM_entrySolidHeap(this, 0x02295C14 /* CheckCreateHeap */, 0xB7B0)) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        fopAcM_setCullSizeBox(this, -35.0f, -10.0f, -35.0f, 35.0f, 100.0f, 35.0f);
        if (!init()) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
    }
    return state;
}
VERIFY(0x022964F8, &daNpc_Mk_c::_create);

/* 022967BC */
static cPhs_State daNpc_Mk_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022967BC, cPhs_State, i_this);
    return ((daNpc_Mk_c*)i_this)->_create();
}
VERIFY(0x022967BC, daNpc_Mk_Create);

/* 022967C0 */
BOOL daNpc_Mk_c::_delete() {
    WWHD_FUNC(0x022967C0, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x1001E34F) /* "Mk" */);
    if (mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x022967C0, &daNpc_Mk_c::_delete);

/* 0229680C */
static BOOL daNpc_Mk_Delete(daNpc_Mk_c* i_this) {
    WWHD_FUNC(0x0229680C, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x0229680C, daNpc_Mk_Delete);

/* 02296810 */
void daNpc_Mk_c::playTexPatternAnm() {
    WWHD_FUNC(0x02296810, void, this);
    if (cLib_calcTimer(&mBlinkTimer) == 0) {
        s32 frameMax0 = J3DAnm_getFrameMax(m_head_tex_pattern);
        if ((s32)mBlinkFrame >= frameMax0) {
            s32 frameMax = J3DAnm_getFrameMax(m_head_tex_pattern);
            mBlinkFrame = (u8)(mBlinkFrame - frameMax);
            mBlinkTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 30.0f);
        } else {
            mBlinkFrame = mBlinkFrame + 1;
        }
    }
}
VERIFY(0x02296810, &daNpc_Mk_c::playTexPatternAnm);

/* 022968D4 */
void daNpc_Mk_c::setAnm(s8 newAnmIdx) {
    WWHD_FUNC(0x022968D4, void, this, newAnmIdx);
    f32 morf = 8.0f;
    if (newAnmIdx == 5 && mAnmIdx == 11) {
        morf = 0.0f;
    }
    if (newAnmIdx != mAnmIdx && newAnmIdx != -1) {
        mAnmIdx = newAnmIdx;
        mAnmTimer = 0.0f;
        dNpc_setAnm(mpMorf, -1 /* EMode_NULL */, morf, 1.0f, gabi::load<s32>(0x1001E250 + 4 * newAnmIdx) /* l_bck_ix_tbl */, -1,
                    STR(0x1001E358) /* "Mk" */);
    }
}
VERIFY(0x022968D4, &daNpc_Mk_c::setAnm);

/* 0229698C */
void daNpc_Mk_c::talkInit() {
    WWHD_FUNC(0x0229698C, void, this);
    mTalkState = TALK_INIT;
    mMsgAnmIdx = 0xFF;
}
VERIFY(0x0229698C, &daNpc_Mk_c::talkInit);

/* 022969A0 */
void daNpc_Mk_c::checkOrder() {
    WWHD_FUNC(0x022969A0, void, this);
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 1 /* eventInfo.checkCommandTalk() */ && ChkOrder(3)) {
        setFlag(1);
        talkInit();
    }
    ClrOrder();
}
VERIFY(0x022969A0, &daNpc_Mk_c::checkOrder);

/* 022969EC */
void daNpc_Mk_c::eventOrder() {
    WWHD_FUNC(0x022969EC, void, this);
    if (ChkOrder(3)) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (ChkOrder(2)) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}
VERIFY(0x022969EC, &daNpc_Mk_c::eventOrder);

/* 02296A14 */
void daNpc_Mk_c::setCollision() {
    WWHD_FUNC(0x02296A14, void, this);
    gabi::Local<cXyz> centerPos;
    centerPos->copy(current.pos);
    f32 cylCollisionRadius = 40.0f;
    f32 height = 80.0f;
    mCyl.SetC(centerPos);
    mCyl.SetR(cylCollisionRadius);
    mCyl.SetH(height);
    dComIfG_Ccsp_Set(&mCyl);
}
VERIFY(0x02296A14, &daNpc_Mk_c::setCollision);

/* 02296A9C */
BOOL daNpc_Mk_c::_execute() {
    WWHD_FUNC(0x02296A9C, BOOL, this);
    m_jnt.setParam(0, 7000, 0, -7000, 8000, 9000, -2000, -9000, 1000);
    playTexPatternAnm();
    /* HD: the walk and run animations play at a rate following the speed (at least 1.0) */
    if (mAnmIdx == 4) {
        f32 rate = speedF * 0.4f;
        mpMorf->setPlaySpeed(rate - 1.0f >= 0.0f ? rate : 1.0f);
    } else if (mAnmIdx == 5) {
        f32 rate = speedF * 0.08f;
        mpMorf->setPlaySpeed(rate - 1.0f >= 0.0f ? rate : 1.0f);
    }
    mAnmEnded = (s8)mpMorf->play(&eyePos, mMtrlSndId, mReverb);
    if (mpMorf->getFrame() < mAnmTimer) {
        mAnmEnded = 1;
    }
    mAnmTimer = mpMorf->getFrame();
    if (mAnmEnded != 0) {
        switch ((u32)(s32)mAnmIdx) {
        case 7:
            if (cM_rnd() < 0.4f) {
                setAnm(8);
            }
            break;
        case 8:
            setAnm(7);
            break;
        }
    }
    checkOrder();
    pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
    eventOrder();
    if (!chkFlag(0x80)) {
        shape_angle.y = current.angle.y;
    }
    clrFlag(0x80);
    mMtrlSndId = 0;
    if (!chkFlag(0x10)) {
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        mObjAcch.CrrPos(dComIfG_Bgsp());
        void* gnd = gabi::at<u8>(gabi::ea(&mObjAcch) + 0xD4 + 0x14); /* mObjAcch.m_gnd */
        if (mObjAcch.ChkGroundHit()) {
            clrFlag(0x40);
            mMtrlSndId = dBgS_GetMtrlSndId_l(dComIfG_Bgsp(), gnd);
        } else {
            setFlag(0x40);
        }
        tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gnd);
        gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), gnd)); /* tevStr.mEnvrIdxOverride */
    } else {
        setFlag(0x40);
    }
    J3DModel* pModel = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
    mpMorf->calc();
    if (chkFlag(0x10)) {
        mCyl.OffCoSPrmBit(1); /* OffCoSetBit */
    } else {
        mCyl.OnCoSPrmBit(1); /* OnCoSetBit */
        setCollision();
    }
    return TRUE;
}
VERIFY(0x02296A9C, &daNpc_Mk_c::_execute);

/* 02296FA8 */
static BOOL daNpc_Mk_Execute(daNpc_Mk_c* i_this) {
    WWHD_FUNC(0x02296FA8, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x02296FA8, daNpc_Mk_Execute);

/* 02296FAC */
BOOL daNpc_Mk_c::_draw() {
    WWHD_FUNC(0x02296FAC, BOOL, this);
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pModelData = J3DModel_getModelData_l(pModel);
    if (chkFlag(0x10)) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, pModelData, mBlinkFrame);
    McaMorf_entry(mpMorf);
    gabi::store<u32>(gabi::ea(pModelData) + 0x38, 0); /* mBtpAnm.remove(pModelData) */
    /* HD: no dComIfGd_setShadow */
    dSnap_RegistFig(0x61 /* DSNAP_TYPE_NPC_MK */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x02296FAC, &daNpc_Mk_c::_draw);

/* 02297060 */
static BOOL daNpc_Mk_Draw(daNpc_Mk_c* i_this) {
    WWHD_FUNC(0x02297060, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x02297060, daNpc_Mk_Draw);

/* 02297064 */
static BOOL daNpc_Mk_IsDelete(daNpc_Mk_c*) {
    WWHD_FUNC(0x02297064, BOOL, (daNpc_Mk_c*)nullptr);
    return TRUE;
}
VERIFY(0x02297064, daNpc_Mk_IsDelete);

/* 0229706C */
void daNpc_Mk_c::setAnmStatus() {
    WWHD_FUNC(0x0229706C, void, this);
    switch ((u32)(s32)mState) {
    case STATE_DEMO01:
        setAnm(2);
        break;
    case STATE_CLIMB01:
    case STATE_RUNAWAY:
        setAnm(6);
        break;
    case STATE_JITANDA02:
        break;
    default:
        setAnm(0);
        break;
    }
}
VERIFY(0x0229706C, &daNpc_Mk_c::setAnmStatus);

/* 022970AC */
bool daNpc_Mk_c::chkAttentionLocal() {
    WWHD_FUNC(0x022970AC, bool, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (chkFlag(0x1)) {
        return true;
    }
    if (mAttentionTimer != 0) {
        mAttentionTimer = mAttentionTimer - 1;
        return true;
    }
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x022970AC, &daNpc_Mk_c::chkAttentionLocal);

/* 0229718C */
void daNpc_Mk_c::chkAttention() {
    WWHD_FUNC(0x0229718C, void, this);
    bool temp = chkFlag(0x4);
    if (chkAttentionLocal()) {
        setFlag(0x4);
    } else {
        clrFlag(0x4);
    }
    if (temp != chkFlag(0x4) && temp == true) {
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
    }
}
VERIFY(0x0229718C, &daNpc_Mk_c::chkAttention);

/* 02297210 */
u32 daNpc_Mk_c::next_msgStatus(be<u32>* pMsgNo) { /* u16 */
    WWHD_FUNC(0x02297210, u32, this, pMsgNo);
    u16 msgStatus = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch (*pMsgNo) {
    case 0x26BD:
    case 0x26C5:
    case 0x26C6:
    case 0x26C7:
    case 0x26C8:
    case 0x26C9:
    case 0x26CB:
    case 0x26CD:
    case 0x26D0:
    case 0x26D1:
    case 0x26D3:
    case 0x26D4:
    case 0x26D8:
        *pMsgNo = *pMsgNo + 1;
        break;
    case 0x26CA:
        if (mMsgSelectNum == 0) {
            setFlag(0x8);
            msgStatus = 0x10; /* fopMsgStts_MSG_ENDS_e */
        } else {
            field_0x6A4 = 1;
            *pMsgNo = 0x26CB;
        }
        break;
    default:
        msgStatus = 0x10;
        break;
    }
    return msgStatus;
}
VERIFY(0x02297210, &daNpc_Mk_c::next_msgStatus);

/* 022972C8 */
u32 daNpc_Mk_c::getMsg() {
    WWHD_FUNC(0x022972C8, u32, this);
    u32 msgNo = 0;
    clrFlag(0x8);
    switch ((u32)(s32)mType) {
    case TYPE_NORMAL:
        if (mState == STATE_DEMO01 || mState == STATE_DEMO02 || mState == STATE_DEMO03) {
            msgNo = mMsgNo;
        } else if (dComIfGs_isTmpBit(0x0040)) {
            msgNo = 0x26B5;
        } else if (dComIfGs_isEventBit(0x1340)) {
            msgNo = 0x26AD;
        } else if (dComIfGs_isEventBit(0x1380)) {
            msgNo = 0x26C5;
        } else if (dComIfGs_isEventBit(0x1210)) {
            msgNo = 0x26B9;
        } else {
            msgNo = 0x26BD;
            dComIfGs_onEventBit(0x1210);
        }
        break;
    case TYPE_MINIGAME:
        if (dComIfGs_isTmpBit(0x0020)) {
            msgNo = 0x26B5;
        } else {
            msgNo = 0x26B1;
            dComIfGs_onTmpBit(0x0020);
        }
        break;
    }
    return msgNo;
}
VERIFY(0x022972C8, &daNpc_Mk_c::getMsg);

/* 02297404 */
void daNpc_Mk_c::msgAnm(u8 param_1) {
    WWHD_FUNC(0x02297404, void, this, param_1);
    if (mMsgAnmIdx != param_1) {
        mMsgAnmIdx = param_1;
        /* HD: cases 0-2 and 4-8 read a table (.rodata 0x1001E36C: 0, 1, 2, -, 7, 4, 5, 6, 9) */
        if (param_1 == 3) {
            mDoAud_seStart_noPos(0x8F0 /* JA_SE_START_WHISTLE */);
            setAnm(3);
        } else if (param_1 <= 8) {
            setAnm(gabi::load<u8>(0x1001E36C + param_1));
        }
    }
}
VERIFY(0x02297404, &daNpc_Mk_c::msgAnm);

/* 022974AC */
void daNpc_Mk_c::msgPushButton() {
    WWHD_FUNC(0x022974AC, void, this);
    u32 mgr = msgMgr();
    if ((s32)mCurrMsgNo != 0x26CA) {
        return;
    }
    mMsgSelectNum = (u8)gabi::load<u32>(mgr + 0x948); /* HD: l_msg->mSelectNum */
}
VERIFY(0x022974AC, &daNpc_Mk_c::msgPushButton);

/* 022974CC */
u16 daNpc_Mk_c::talk() {
    WWHD_FUNC(0x022974CC, u16, this);
    /* HD: the message is the message manager (GameCube: l_msg found by l_msgId) */
    u32 mgr = msgMgr();
    u16 msgStatus = 0xFF;
    if (mTalkState == TALK_INIT) {
        l_msgId() = 0xFFFFFFFF; /* fpcM_ERROR_PROCESS_ID_e */
        mCurrMsgNo = getMsg();
        mTalkState = TALK_MSG_CREATE;
    } else if (mTalkState == TALK_FINISHED) {
        msgStatus = 0xFE;
    } else if (l_msgId() == 0xFFFFFFFF) {
        l_msgId() = msgMgr_messageSet(mgr, mCurrMsgNo, &eyePos);
    } else {
        if (!chkFlag(0x100)) {
            msgAnm(dComIfGp_getMesgAnimeAttrInfo());
        }
        switch ((u32)(s32)mTalkState) {
        case 1:
            mTalkState = TALK_ACTIVE; /* HD: no fopMsgM_SearchByID */
            break;
        case 2:
            msgStatus = msgMgr_getStatus(mgr);
            if (msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
                msgPushButton();
                msgMgr_setStatus(mgr, next_msgStatus(&mCurrMsgNo));
                if (msgMgr_getStatus(mgr) == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
                    msgMgr_messageSet(mgr, mCurrMsgNo, nullptr);
                }
            } else if (msgStatus == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
                msgMgr_setStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
                mTalkState = TALK_FINISHED;
            }
            break;
        }
    }
    return msgStatus;
}
VERIFY(0x022974CC, &daNpc_Mk_c::talk);

/* 0229764C */
void daNpc_Mk_c::setAttention(bool param_1) {
    WWHD_FUNC(0x0229764C, void, this, param_1);
    if (!param_1 && mAttnSetCount >= 2) {
        return;
    }
    eyePos.x = mEyePos.x;
    eyePos.y = mEyePos.y;
    eyePos.z = mEyePos.z;
    cXyz* attPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attPos->x = mAttnBasePos.x;
    attPos->y = mAttnBasePos.y + 45.0f;
    attPos->z = mAttnBasePos.z;
}
VERIFY(0x0229764C, &daNpc_Mk_c::setAttention);

/* 022976A0 */
u8 daNpc_Mk_c::getLookBackMode() {
    WWHD_FUNC(0x022976A0, u8, this);
    if (chkFlag(0x200)) {
        return 3;
    }
    if (chkFlag(0x20)) {
        return 0;
    }
    if (field_0x6A4 != 0) {
        return 2;
    }
    if (chkFlag(0x400) || mAnmIdx == 7 || mAnmIdx == 8) {
        return 2;
    }
    if (chkFlag(0x4)) {
        return 1;
    }
    if (mState == STATE_VISIT && (mVisitMode == VISIT_WALK_AROUND_LINK || mVisitMode == VISIT_REACHED_LINK)) {
        return 1;
    }
    return 2;
}
VERIFY(0x022976A0, &daNpc_Mk_c::getLookBackMode);

/* 0229772C */
void daNpc_Mk_c::lookBack() {
    WWHD_FUNC(0x0229772C, void, this);
    gabi::Local<cXyz> eye;   /* dNpc_playerEyePos result */
    gabi::Local<cXyz> temp2;
    gabi::Local<cXyz> temp;  /* passed by value: a copy */
    f32 tx = 0.0f, ty = 0.0f, tz = 0.0f;
    cXyz* dstPos = nullptr;
    s16 desiredYRot = current.angle.y;
    u8 headOnlyFollow = false;
    switch (getLookBackMode()) {
    case 0:
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        dNpc_playerEyePos(eye, -20.0f);
        temp2->copy(*eye);
        dstPos = temp2;
        tx = current.pos.x;
        tz = current.pos.z;
        ty = eyePos.y;
        break;
    case 1:
        dNpc_playerEyePos(eye, -20.0f);
        temp2->copy(*eye);
        dstPos = temp2;
        tx = current.pos.x;
        tz = current.pos.z;
        ty = eyePos.y;
        headOnlyFollow = true;
        break;
    case 2:
        /* desiredYRot = current.angle.y (the value read on entry) */
        headOnlyFollow = true;
        break;
    case 3: {
        fopAc_ac_c* pLink = fopAcM_getTalkEventPartner(dComIfGp_getLinkPlayer());
        if (pLink == nullptr) {
            pLink = dComIfGp_getLinkPlayer();
        }
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        temp2->copy(pLink->eyePos);
        dstPos = temp2;
        tz = current.pos.z;
        tx = current.pos.x;
        ty = eyePos.y;
        break;
    }
    }
    if (m_jnt.mbTrn != 0) { /* m_jnt.trnChk() */
        cLib_addCalcAngleS2(&mMaxHeadTurnVelocity, 0x5DC, 4, 0x800);
    } else {
        mMaxHeadTurnVelocity = 0;
    }
    temp->x = tx;
    temp->y = ty;
    temp->z = tz;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, temp, desiredYRot, mMaxHeadTurnVelocity, headOnlyFollow);
}
VERIFY(0x0229772C, &daNpc_Mk_c::lookBack);

/* dist = pLink->current.pos - current.pos; dist.abs2XZ() */
static f32 mk_abs2XZ(fopAc_ac_c* a, fopAc_ac_c* b) {
    gabi::Local<cXyz> dist;
    gabi::Local<cXyz> flat;
    cXyz_mi(&a->current.pos, dist, &b->current.pos);
    flat->x = dist->x;
    flat->y = 0.0f;
    flat->z = dist->z;
    return PSVECSquareMag(flat);
}

/* 02297990 */
u8 daNpc_Mk_c::nextVisitMode() {
    WWHD_FUNC(0x02297990, u8, this);
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    switch (mVisitMode) {
    case VISIT_START:
        return VISIT_WALK_PATH;
    case VISIT_TALK:
        if (field_0x6A4 != 0) {
            return VISIT_WALK_PATH_FAST;
        }
        return VISIT_RUN_LINK;
    case VISIT_WALK_PATH:
        if (mk_abs2XZ(pLink, this) < 160000.0f && dNpc_PathRun_chkInside(&field_0x698, &pLink->current.pos)) {
            return VISIT_NOTICE_LINK;
        }
        return VISIT_WALK_PATH;
    case VISIT_RUN_LINK:
        if (!dNpc_PathRun_chkInside(&field_0x690, &pLink->current.pos)) {
            return VISIT_LEFT_PATH;
        }
        if (mk_abs2XZ(pLink, this) < 22500.0f) {
            return VISIT_REACHED_LINK;
        }
        return VISIT_RUN_LINK;
    case VISIT_WALK_AROUND_LINK:
    case VISIT_REACHED_LINK:
        if (!dNpc_PathRun_chkInside(&field_0x690, &pLink->current.pos)) {
            return VISIT_LEFT_PATH;
        }
        if (mk_abs2XZ(pLink, this) > 32400.0f) {
            return VISIT_RUN_LINK;
        }
        if (mVisitMode == VISIT_REACHED_LINK) {
            if (mRunAroundLinkTimer != 0) {
                mRunAroundLinkTimer = mRunAroundLinkTimer - 1;
            } else {
                return VISIT_WALK_AROUND_LINK;
            }
        }
        return mVisitMode;
    case VISIT_NOTICE_LINK:
    case VISIT_LEFT_PATH:
        if (mWaitTimer != 0) {
            mWaitTimer = mWaitTimer - 1;
            break;
        }
        if (mk_abs2XZ(pLink, this) < 160000.0f && dNpc_PathRun_chkInside(&field_0x698, &pLink->current.pos)) {
            return VISIT_RUN_LINK;
        }
        return VISIT_WALK_PATH;
    case VISIT_WALK_PATH_FAST:
        if (field_0x6A4 != 0) {
            field_0x6A4 = field_0x6A4 - 1;
            break;
        }
        return VISIT_WALK_PATH_IGNORE_LINK;
    case VISIT_WALK_PATH_IGNORE_LINK:
        if (field_0x6A4 != 0) {
            field_0x6A4 = field_0x6A4 - 1;
            break;
        }
        return VISIT_WALK_PATH;
    }
    return mVisitMode;
}
VERIFY(0x02297990, &daNpc_Mk_c::nextVisitMode);

/* 02297D78 */
void daNpc_Mk_c::visitInit(u8 i_nextVisitMode) {
    WWHD_FUNC(0x02297D78, void, this, i_nextVisitMode);
    switch (i_nextVisitMode) {
    case VISIT_WALK_PATH:
        dNpc_PathRun_setNearPathIndx(&field_0x688, &current.pos, 100.0f);
        setAnm(4);
        break;
    case VISIT_RUN_LINK:
        setAnm(5);
        if (mVisitMode == 6) {
            mTimerToReachLink = 55;
        } else {
            mTimerToReachLink = 0;
        }
        break;
    case VISIT_WALK_AROUND_LINK:
        setAnm(4);
        break;
    case VISIT_REACHED_LINK:
        mRunAroundLinkTimer = mTimerToReachLink;
        setAnm(5);
        break;
    case VISIT_NOTICE_LINK:
        setAnm(0);
        speedF = 0.0f;
        mWaitTimer = 0xF;
        break;
    case VISIT_LEFT_PATH:
        setAnm(0);
        speedF = 0.0f;
        mWaitTimer = 0x19;
        break;
    case VISIT_WALK_PATH_FAST:
        dNpc_PathRun_setNearPathIndx(&field_0x688, &current.pos, 100.0f);
        setAnm(5);
        field_0x6A4 = 0x64;
        break;
    case VISIT_WALK_PATH_IGNORE_LINK:
        setAnm(4);
        field_0x6A4 = 0x258;
        break;
    default: /* JUT_ASSERT(1113, 0) */
        JUT_ASSERT_fail(STR(0x1001E38C), 0x459, STR(0x1001E388));
        break;
    }
    mVisitMode = i_nextVisitMode;
}
VERIFY(0x02297D78, &daNpc_Mk_c::visitInit);

/* 02297F40 */
void daNpc_Mk_c::walkPath(u8 param_1) {
    WWHD_FUNC(0x02297F40, void, this, param_1);
    if (MkStatic_walkPath(&mMkStatic, this, &field_0x688, param_1)) {
        setFlag(0x80);
    }
}
VERIFY(0x02297F40, &daNpc_Mk_c::walkPath);

/* 02297F90 */
void daNpc_Mk_c::runLink() {
    WWHD_FUNC(0x02297F90, void, this);
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> a, b;
    gabi::Local<be<s16>> temp;
    *a = current.pos.get();
    *b = pLink->current.pos.get();
    dNpc_calc_DisXZ_AngY(a, b, nullptr, temp);
    cLib_addCalcAngleS2(&current.angle.y, *temp, 8, 0x800);
}
VERIFY(0x02297F90, &daNpc_Mk_c::runLink);

/* 02298018 */
void daNpc_Mk_c::aroundLink() {
    WWHD_FUNC(0x02298018, void, this);
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    MkStatic_aroundWalk(&mMkStatic, this, pLink, mRunAroundLinkTimer);
}
VERIFY(0x02298018, &daNpc_Mk_c::aroundLink);

/* 02298058 */
void daNpc_Mk_c::visitProc() {
    WWHD_FUNC(0x02298058, void, this);
    switch (mVisitMode) {
    case VISIT_WALK_PATH:
    case VISIT_WALK_PATH_IGNORE_LINK:
        walkPath(0);
        break;
    case VISIT_WALK_PATH_FAST:
        walkPath(2);
        break;
    case VISIT_RUN_LINK:
        runLink();
        if (mTimerToReachLink < 55) {
            mTimerToReachLink = mTimerToReachLink + 1;
        }
        break;
    case VISIT_WALK_AROUND_LINK:
    case VISIT_REACHED_LINK:
        aroundLink();
        break;
    case VISIT_NOTICE_LINK:
    case VISIT_LEFT_PATH:
        setFlag(0x20);
        break;
    }
}
VERIFY(0x02298058, &daNpc_Mk_c::visitProc);

/* fopAcM_seStart(this, id, 0) (HD: no null checks in this unit's copy) */
static inline void mk_seStart(fopAc_ac_c* a, u32 id) {
    mDoAud_seStart(id, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}

/* 0229814C */
void daNpc_Mk_c::runawayInit() {
    WWHD_FUNC(0x0229814C, void, this);
    switch (mMkStatic.state) {
    case 1:
    case 2:
    case 5:
        if (mAnmIdx == 6) {
            mk_seStart(this, 0x4905 /* JA_SE_CV_TR_KO_A_FOUND */);
        }
        speedF = 0.0f;
        setAnm(5);
        break;
    case 3:
        speedF = 0.0f;
        setAnm(6);
        break;
    case 4:
        speed.y = 25.0f;
        gravity = -3.3f;
        speedF = 8.0f;
        setFlag(0x40);
        setAnm(9);
        mState = STATE_JUMP;
        current.angle.y = field_0x6F0;
        mk_seStart(this, 0x4907 /* JA_SE_CV_TR_KO_A_CAUGHT */);
        break;
    }
}
VERIFY(0x0229814C, &daNpc_Mk_c::runawayInit);

/* 022982B4 */
bool daNpc_Mk_c::wait01() {
    WWHD_FUNC(0x022982B4, bool, this);
    if (chkFlag(0x1)) {
        mPrevState = mState;
        mState = STATE_TALK01;
        setAnmStatus();
    } else {
        SetOrder(0x1);
    }
    return isMorf();
}
VERIFY(0x022982B4, &daNpc_Mk_c::wait01);

/* 02298348 */
bool daNpc_Mk_c::talk01() {
    WWHD_FUNC(0x02298348, bool, this);
    if (talk() == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        if (chkFlag(0x8)) {
            u32 idx = dComIfGp_evmng_getEventIdx_r(STR(0x1001E3A0) /* "MK_GAMESTART" */, 0xFF);
            mEventIdx = (s16)idx;
            fopAcM_orderChangeEventId(this, idx, 0, 0xFFFF);
            mState = STATE_DEMO02;
            setFlag(0x400);
            dComIfGs_onTmpBit(0x0040);
            dComIfGs_onEventBit(0x2201);
        } else {
            mState = mPrevState;
            setAnmStatus();
            dComIfGp_event_reset();
            clrFlag(0x1);
            mAttentionTimer = 5;
        }
    }
    setFlag(0x20);
    return isMorf();
}
VERIFY(0x02298348, &daNpc_Mk_c::talk01);

/* 02298484 */
bool daNpc_Mk_c::talk02() {
    WWHD_FUNC(0x02298484, bool, this);
    if (talk() == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        if (!MkStatic_chkGameSet(&mMkStatic)) {
            mState = mPrevState;
            setAnmStatus();
            dComIfGp_event_reset();
        } else {
            u32 idx = dComIfGp_evmng_getEventIdx_r(STR(0x1001E3B0) /* "MK_GAMESET" */, 0xFF);
            mEventIdx = (s16)idx;
            fopAcM_orderChangeEventId(this, idx, 0, 0xFFFF);
            mDoAud_seStart_noPos(0x8F1 /* JA_SE_END_WHISTLE */);
            mState = STATE_DEMO02;
            setFlag(0x400);
        }
        clrFlag(0x1);
        mAttentionTimer = 5;
    }
    setFlag(0x20);
    return isMorf();
}
VERIFY(0x02298484, &daNpc_Mk_c::talk02);

/* 02298584 */
s32 daNpc_Mk_c::getNowEventAction() {
    WWHD_FUNC(0x02298584, s32, this);
    /* static char* action_table[17] (.data 0x101C1F60): WAIT, TALK, HOME, RUN, RUN3, SPEAK, PARTNER,
     * JUMP, JUMP2, JITABATA, LOOK_P, HIND, DISP, MAKEITEM, GAMESET, WARP, TURN */
    return dComIfGp_evmng_getMyActIdx(mStaffIdx, 0x101C1F60, 17, FALSE, 0);
}
VERIFY(0x02298584, &daNpc_Mk_c::getNowEventAction);

/* 022985D0 */
BOOL daNpc_Mk_c::checkDemoStart() {
    WWHD_FUNC(0x022985D0, BOOL, this);
    mStaffIdx = dComIfGp_evmng_getMyStaffId(STR(mpName), nullptr, 0);
    if (mStaffIdx != -1) {
        mEventAction = getNowEventAction();
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022985D0, &daNpc_Mk_c::checkDemoStart);

/* 02298654 */
void daNpc_Mk_c::remove_Um2() {
    WWHD_FUNC(0x02298654, void, this);
    fopAc_ac_c* ac = fopAcM_searchFromName(STR(0x1001E448) /* "Um2" */, 0, 0);
    if (ac != nullptr) {
        ac->mParameters = ac->mParameters | 0x80000000; /* fopAcM_SetParam */
        ac->actor_status = ac->actor_status | 0x800;     /* fopAcM_OnStatus(ac, fopAcStts_UNK800_e) */
    }
}
VERIFY(0x02298654, &daNpc_Mk_c::remove_Um2);

/* 022986A4 */
void daNpc_Mk_c::demoInitCom() {
    WWHD_FUNC(0x022986A4, void, this);
    void* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001E454) /* "RM_Um2" */);
    if (a_intP != nullptr) {
        remove_Um2();
    }
    a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001E44C) /* "SOUND" */);
    if (a_intP != nullptr) {
        switch (gabi::load<u32>(gabi::ea(a_intP))) {
        case 1:
            mk_seStart(this, 0x4904 /* JA_SE_CV_TR_KO_A_RUN_AWAY */);
            break;
        case 2:
            mk_seStart(this, 0x4905 /* JA_SE_CV_TR_KO_A_FOUND */);
            break;
        }
    }
}
VERIFY(0x022986A4, &daNpc_Mk_c::demoInitCom);

/* 022987B0 */
bool daNpc_Mk_c::demoProc() {
    WWHD_FUNC(0x022987B0, bool, this);
    BOOL temp = FALSE;
    if (dComIfGp_evmng_getIsAddvance(mStaffIdx)) {
        demoInitCom();
        switch ((u32)mEventAction) {
        case 0:
        case 1:
        case 10:
            speedF = 0.0f;
            setAnm(0);
            break;
        case 2:
            current.angle.y = home.angle.y;
            old.pos.copy(home.pos);
            current.pos.copy(home.pos);
            speedF = 0.0f;
            break;
        case 3:
        case 4: {
            if (mAnmIdx == 5) {
                speedF = 12.0f;
            } else if (mAnmIdx == 11) {
                speedF = 6.0f;
                setAnm(5);
            } else {
                setAnm(5);
            }
            cXyz* a_xyz = (cXyz*)dComIfGp_evmng_getMyXyzP(mStaffIdx, STR(0x1001E470) /* "Pos" */);
            if (a_xyz == nullptr) /* JUT_ASSERT(1306, a_xyz) */
                JUT_ASSERT_fail(STR(0x1001E49C), 0x51A, STR(0x1001E474));
            field_0x6D0.copy(current.pos);
            field_0x6DC.copy(*a_xyz);
            void* pTimer = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001E47C) /* "Timer" */);
            if (pTimer != nullptr) {
                field_0x6E8 = gabi::load<s16>(gabi::ea(pTimer) + 2); /* (s16)*pTimer */
            } else {
                field_0x6E8 = -1;
            }
            break;
        }
        case 5: {
            talkInit();
            dComIfGp_event_setTalkPartner(this);
            void* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001E484) /* "MsgNo" */);
            if (a_intP == nullptr) /* JUT_ASSERT(1320, a_intP) */
                JUT_ASSERT_fail(STR(0x1001E49C), 0x528, STR(0x1001E494));
            mMsgNo = gabi::load<u32>(gabi::ea(a_intP));
            temp = TRUE;
            break;
        }
        case 6:
            dComIfGp_event_setTalkPartner(this);
            break;
        case 8:
            setFlag(0x2 | 0x40);
            gravity = -3.3f;
            speed.y = 20.0f;
            speedF = 8.0f;
            setAnm(10);
            break;
        case 7:
            setFlag(0x2 | 0x40);
            gravity = -2.1f;
            speed.y = 30.0f;
            speedF = 10.0f;
            setAnm(10);
            break;
        case 9:
            setFlag(0x2);
            setAnm(7);
            break;
        case 11:
            speedF = 0.0f;
            setFlag(0x10);
            actor_status = actor_status & ~0x20u; /* fopAcM_OffStatus(this, fopAcStts_SHOWMAP_e) */
            mPrevState = 13;
            break;
        case 12:
            /* mCurrActionFunc = NULL; setAction(&daNpc_Mk_c::visit_action, NULL) */
            mCurrActionFunc.i = -1;
            mCurrActionFunc.f = MK_VISIT_ACTION;
            mCurrActionFunc.d = 0;
            mActionStatus = ACTION_STARTING;
            gabi::call_ptr<BOOL>(mCurrActionFunc.f, this, (u32)0);
            clrFlag(0x10);
            actor_status = actor_status | 0x20; /* fopAcM_OnStatus(this, fopAcStts_SHOWMAP_e) */
            mPrevState = mState;
            mState = STATE_DEMO01;
            break;
        case 13: {
            s32 itemID = fopAcM_createItemForPresentDemo(&current.pos, 7 /* dItemNo_HEART_PIECE_e */, 0, -1, -1, nullptr, nullptr);
            if (itemID != -1) {
                dComIfGp_event_setItemPartnerId(itemID);
            }
            dComIfGs_onEventBit(0x1340);
            break;
        }
        case 14:
            dComIfGs_offTmpBit(0x0040);
            dComIfGs_onTmpBit(0x0120);
            break;
        case 15: {
            speedF = 0.0f;
            cXyz* a_xyz = (cXyz*)dComIfGp_evmng_getMyXyzP(mStaffIdx, STR(0x1001E470) /* "Pos" */);
            if (a_xyz == nullptr) /* JUT_ASSERT(1386, a_xyz) */
                JUT_ASSERT_fail(STR(0x1001E49C), 0x56A, STR(0x1001E474));
            current.pos.copy(*a_xyz);
            old.pos.copy(*a_xyz);
            void* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001E48C) /* "Angle" */);
            if (a_intP == nullptr) /* JUT_ASSERT(1392, a_intP) */
                JUT_ASSERT_fail(STR(0x1001E49C), 0x570, STR(0x1001E494));
            current.angle.y = (s16)gabi::load<s32>(gabi::ea(a_intP));
            shape_angle.y = (s16)gabi::load<s32>(gabi::ea(a_intP));
            break;
        }
        case 16: {
            void* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x1001E48C) /* "Angle" */);
            if (a_intP == nullptr) /* JUT_ASSERT(1398, a_intP) */
                JUT_ASSERT_fail(STR(0x1001E49C), 0x576, STR(0x1001E494));
            field_0x6F0 = (s16)gabi::load<s32>(gabi::ea(a_intP));
            break;
        }
        }
    }
    switch ((u32)mEventAction) {
    case 3:
    case 4: {
        gabi::Local<cXyz> from, to;
        gabi::Local<be<f32>> temp2;
        gabi::Local<be<s16>> temp3;
        *from = current.pos.get();
        *to = field_0x6DC.get();
        dNpc_calc_DisXZ_AngY(from, to, temp2, temp3);
        if (*temp2 < speedF || MkStatic_chkPointPass(&mMkStatic, &field_0x6D0, &field_0x6DC, &current.pos) != 0) {
            if (mEventAction == 4) {
                current.pos.x = field_0x6DC.x;
                current.pos.z = field_0x6DC.z;
                speedF = 0.0f;
            }
            dComIfGp_evmng_cutEnd(mStaffIdx);
        } else {
            cLib_addCalcAngleS(&current.angle.y, *temp3, 8, 0x1000, 0x400);
            cLib_chaseF(&speedF, 12.0f, 1.1f);
            if (field_0x6E8 < 0) {
                return true;
            }
            if (field_0x6E8 > 0) {
                s16 t = (s16)(field_0x6E8 - 1);
                field_0x6E8 = t;
                if (t != 0) {
                    return true;
                }
            }
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        return true;
    }
    case 5: {
        u16 temp4 = talk();
        if (temp4 == 0x12 /* fopMsgStts_BOX_CLOSED_e */ || temp4 == 0xFE) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        setFlag(0x20);
        break;
    }
    case 2:
    case 6:
    case 15:
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return true;
    case 1:
        setFlag(0x20);
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    case 10:
        setFlag(0x200);
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    case 7:
    case 8:
        if (!chkFlag(0x40)) {
            setAnm(11);
            gravity = -3.3f; /* HD: GameCube -30.0 */
            speedF = 0.0f;
            if (chkFlag(0x2)) {
                clrFlag(0x2);
                if (mEventAction == 7) {
                    gabi::Local<cXyz> up;
                    up->x = 0.0f;
                    up->y = 1.0f;
                    up->z = 0.0f;
                    dComIfGp_getVibration_StartShock(4, -0x21, up);
                }
            } else if (mAnmEnded != 0) {
                dComIfGp_evmng_cutEnd(mStaffIdx);
            }
        }
        return true;
    case 9:
        if (chkFlag(0x2)) {
            if (mAnmEnded != 0) {
                dComIfGp_evmng_cutEnd(mStaffIdx);
                clrFlag(0x2);
            }
        } else {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        break;
    case 16:
        if (cLib_addCalcAngleS(&current.angle.y, field_0x6F0, 8, 0x1000, 0x400) == 0) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        break;
    default:
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    }
    if (temp) {
        return true;
    }
    return isMorf();
}
VERIFY(0x022987B0, &daNpc_Mk_c::demoProc);

/* 02299290 */
bool daNpc_Mk_c::demo03() {
    WWHD_FUNC(0x02299290, bool, this);
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        mState = mPrevState;
        setAnmStatus();
        dComIfGp_event_reset();
        clrFlag(0x1);
        mAttentionTimer = 5;
        return true;
    }
    if (!checkDemoStart()) { /* JUT_ASSERT(2093, NULL) */
        JUT_ASSERT_fail(STR(0x1001E4B0), 0x82D, STR(0x1001E4AC));
    }
    return demoProc();
}
VERIFY(0x02299290, &daNpc_Mk_c::demo03);

/* 02299358 */
u8 daNpc_Mk_c::visitTalkInit() {
    WWHD_FUNC(0x02299358, u8, this);
    if (dComIfGs_isEventBit(0x1F80) && !dComIfGs_isEventBit(0x1E02)) {
        mEventIdx = eventInfo_getEventId(this);
        mState = STATE_DEMO03;
        demo03();
        dComIfGs_onEventBit(0x1E04);
        return 12;
    }
    if (dComIfGs_isEventBit(0x1208) && !dComIfGs_checkGetItem(0x23 /* dItemNo_PICTO_BOX_e */) &&
        !dComIfGs_checkGetItem(0x26 /* dItemNo_DELUXE_PICTO_BOX_e */)) {
        mEventIdx = eventInfo_getEventId(this);
        mState = STATE_DEMO03;
        demo03();
        dComIfGs_onEventBit(0x1602);
        return 12;
    }
    return 1;
}
VERIFY(0x02299358, &daNpc_Mk_c::visitTalkInit);

/* 02299490 */
bool daNpc_Mk_c::demo01() {
    WWHD_FUNC(0x02299490, bool, this);
    if (!checkDemoStart()) {
        mState = mPrevState;
        setAnmStatus();
        return true;
    }
    return demoProc();
}
VERIFY(0x02299490, &daNpc_Mk_c::demo01);

/* 022994F0 */
void daNpc_Mk_c::visitSetEvent() {
    WWHD_FUNC(0x022994F0, void, this);
    if (dComIfGs_isEventBit(0x1F80)) {
        if (!dComIfGs_isEventBit(0x1E02)) {
            eventInfo_setEventName(this, STR(0x1001E4C8) /* "MK_TALK3" */);
            return;
        }
    }
    if (dComIfGs_isEventBit(0x1208) && !dComIfGs_checkGetItem(0x23 /* dItemNo_PICTO_BOX_e */) &&
        !dComIfGs_checkGetItem(0x26 /* dItemNo_DELUXE_PICTO_BOX_e */)) {
        eventInfo_setEventName(this, STR(0x1001E4D4) /* "MK_TALK2" */);
        return;
    }
    eventInfo_setEventName(this, STR(0x1001E4C0) /* "MK_TALK" */);
}
VERIFY(0x022994F0, &daNpc_Mk_c::visitSetEvent);

/* 022995C4 */
bool daNpc_Mk_c::visit01() {
    WWHD_FUNC(0x022995C4, bool, this);
    f32 temp = 0.0f; /* GameCube: uninitialised when mAnmIdx is neither 4 nor 5 */
    if (chkFlag(0x1)) {
        mPrevState = mState;
        mState = visitTalkInit();
        setAnmStatus();
        speedF = 0.0f;
        mVisitMode = VISIT_TALK;
        if (dComIfGs_isTmpBit(0x0580)) {
            dComIfGs_offTmpBit(0x0580);
            actor_status = actor_status & ~0x4000u; /* fopAcM_OffStatus(this, fopAcStts_UNK4000_e) */
        }
    } else if (checkDemoStart()) {
        mPrevState = mState;
        mState = STATE_DEMO01;
        setAnmStatus();
        speedF = 0.0f;
        mVisitMode = VISIT_TALK;
        demo01();
    } else {
        u8 nextvisitMode = nextVisitMode();
        if (nextvisitMode != mVisitMode) {
            visitInit(nextvisitMode);
        }
        visitProc();
        if (mAnmIdx == 4) {
            temp = 2.5f;
        } else if (mAnmIdx == 5) {
            if (mVisitMode == 5) {
                temp = 9.0f;
            } else {
                temp = 12.0f;
            }
        }
        /* if (speedF > temp) step 2.8 else 1.1 (fsel on temp - speedF) */
        f32 step = (temp - speedF >= 0.0f) ? 1.1f : 2.8f;
        cLib_chaseF(&speedF, temp, step);
        SetOrder(0x1);
        if (mVisitMode == 4 || mVisitMode == 5) {
            fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
            s16 angle = fopAcM_searchActorAngleY(pLink, this);
            angle = (s16)(angle - pLink->shape_angle.y);
            if (angle < 0) {
                angle = (s16)-angle;
            }
            if (angle > 0x1800) {
                mOrderFlags = (u8)(mOrderFlags & ~0x01);
            }
        }
        if (field_0x6A4 != 0) {
            mOrderFlags = (u8)(mOrderFlags & ~0x01);
        }
        if (dComIfGs_isEventBit(0x1F80) && !dComIfGs_isEventBit(0x1E02) && dComIfGs_isTmpBit(0x0580)) {
            SetOrder(0x2);
            eventInfo_setEventName(this, STR(0x1001E4EC) /* "MK_TALK4" */);
        } else if (ChkOrder(0x1)) {
            visitSetEvent();
        }
    }
    return true;
}
VERIFY(0x022995C4, &daNpc_Mk_c::visit01);

/* 022998A8 */
bool daNpc_Mk_c::climb01() {
    WWHD_FUNC(0x022998A8, bool, this);
    fopAc_ac_c* pLink = dComIfGp_getLinkPlayer();
    if (chkFlag(0x40)) {
        return false;
    }
    if (gabi::load<u32>(gabi::ea(pLink) + 0x3C0) & 0x2000) { /* pLink->checkFrontRollCrash() */
        gabi::Local<cXyz> a, b;
        gabi::Local<be<f32>> temp;
        *a = current.pos.get();
        *b = pLink->current.pos.get();
        dNpc_calc_DisXZ_AngY(a, b, temp, nullptr);
        if (*temp < 200.0f) {
            mState = STATE_DROP01;
            u32 idx = dComIfGp_evmng_getEventIdx_r(STR(0x1001E4FC) /* "MK_DROP" */, 0xFF);
            mEventIdx = (s16)idx;
            fopAcM_orderOtherEventId_r(this, idx, 0xFF, 0xFFFF, 0, 1);
            dComIfGs_onTmpBit(0x0002);
        }
    }
    return isMorf();
}
VERIFY(0x022998A8, &daNpc_Mk_c::climb01);

/* 022999D4 */
bool daNpc_Mk_c::drop01() {
    WWHD_FUNC(0x022999D4, bool, this);
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        mPrevState = STATE_RUNAWAY;
        mState = STATE_DEMO03;
        MkStatic_init(&mMkStatic, 0x50, 0x12C);
        return demo03();
    }
    fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
    return isMorf();
}
VERIFY(0x022999D4, &daNpc_Mk_c::drop01);

/* 02299A80 */
bool daNpc_Mk_c::runaway() {
    WWHD_FUNC(0x02299A80, bool, this);
    u8 temp = MkStatic_runAwayProc(&mMkStatic, this, &field_0x688, &mCyl, &field_0x6F0);
    if (temp != mMkStatic.state) {
        if ((temp == 1 && mMkStatic.state == 2) || (temp == 2 && mMkStatic.state == 1)) {
            if (cM_rndF(1.0f) < 0.5f) {
                mk_seStart(this, 0x4906 /* JA_SE_CV_TR_KO_A_TURN */);
            }
        }
        mMkStatic.state = temp;
        runawayInit();
    }
    if (temp == 3) {
        setFlag(0x20);
    }
    if (mAnmIdx == 5) {
        cLib_chaseF(&speedF, MkStatic_getSpeedF(&mMkStatic, 15.0f, 18.0f), 2.8f);
    }
    return true;
}
VERIFY(0x02299A80, &daNpc_Mk_c::runaway);

/* 02299B94 */
bool daNpc_Mk_c::jump() {
    WWHD_FUNC(0x02299B94, bool, this);
    if (!chkFlag(0x40)) {
        setAnm(0);
        gravity = -3.3f; /* HD: GameCube -30.0 */
        speedF = 0.0f;
        if (isMorf()) {
            mState = STATE_JITANDA01;
            setAnm(7);
            SetOrder(0x2);
        }
    }
    return true;
}
VERIFY(0x02299B94, &daNpc_Mk_c::jump);

/* 02299C28 */
bool daNpc_Mk_c::jitanda01() {
    WWHD_FUNC(0x02299C28, bool, this);
    if (chkFlag(0x1)) {
        setFlag(0x100);
        mPrevState = STATE_JITANDA02;
        mState = STATE_TALK02;
    } else {
        SetOrder(0x2);
    }
    return true;
}
VERIFY(0x02299C28, &daNpc_Mk_c::jitanda01);

/* 02299C68 */
bool daNpc_Mk_c::jitanda02() {
    WWHD_FUNC(0x02299C68, bool, this);
    if (chkFlag(0x1)) {
        mPrevState = 9;
        mState = STATE_TALK02;
    } else if (checkDemoStart()) {
        mPrevState = mState;
        mState = STATE_DEMO01;
        speedF = 0.0f;
        demo01();
    } else {
        SetOrder(0x1);
    }
    return true;
}
VERIFY(0x02299C68, &daNpc_Mk_c::jitanda02);

/* 02299D00 */
bool daNpc_Mk_c::demo02() {
    WWHD_FUNC(0x02299D00, bool, this);
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        dComIfGp_event_reset();
        setAction(MK_HIND_ACTION);
        setFlag(0x10);
        actor_status = actor_status & ~0x20u; /* fopAcM_OffStatus(this, fopAcStts_SHOWMAP_e) */
        clrFlag(0x1);
        clrFlag(0x400);
        return true;
    }
    if (!checkDemoStart()) { /* JUT_ASSERT(2074, NULL) */
        JUT_ASSERT_fail(STR(0x1001E514), 0x81A, STR(0x1001E510));
    }
    return demoProc();
}
VERIFY(0x02299D00, &daNpc_Mk_c::demo02);

/* 02299EAC */
BOOL daNpc_Mk_c::wait_action(void*) {
    WWHD_FUNC(0x02299EAC, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        mState = STATE_WAIT;
        setAnmStatus();
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        chkAttention();
        clrFlag(0x200 | 0x20);
        bool temp;
        switch ((u32)(s32)mState) {
        case STATE_WAIT:
            temp = wait01();
            break;
        case STATE_TALK01:
            temp = talk01();
            break;
        default:
            temp = false;
            break;
        }
        lookBack();
        setAttention(temp);
    }
    return TRUE;
}
VERIFY(0x02299EAC, &daNpc_Mk_c::wait_action);

/* 02299F8C */
BOOL daNpc_Mk_c::hind_action(void*) {
    WWHD_FUNC(0x02299F8C, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        mActionStatus = ACTION_ONGOING;
    } else if (mActionStatus != ACTION_ENDING) {
        clrFlag(0x200 | 0x20);
        if (mType == TYPE_MINIGAME && dComIfGs_isTmpBit(0x0040) && !dComIfGs_isTmpBit(0x0020)) {
            setAction(MK_SEEK_ACTION);
            clrFlag(0x10);
            actor_status = actor_status | 0x20; /* fopAcM_OnStatus(this, fopAcStts_SHOWMAP_e) */
        }
        if ((mType == TYPE_NORMAL || mType == TYPE_MINIGAME) && checkDemoStart()) {
            bool temp = demoProc();
            lookBack();
            setAttention(temp);
        }
    }
    return TRUE;
}
VERIFY(0x02299F8C, &daNpc_Mk_c::hind_action);

/* 0229A17C */
BOOL daNpc_Mk_c::visit_action(void*) {
    WWHD_FUNC(0x0229A17C, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        if (dComIfGs_isTmpBit(0x0040)) {
            mState = STATE_JITANDA02;
            setAnm(7);
            mStts.m_weight = 0xFE; /* SetWeight */
        } else {
            mState = STATE_VISIT;
            setAnmStatus();
            mStts.m_weight = 0xA0; /* SetWeight */
        }
        mActionStatus = mActionStatus + 1;
        mVisitMode = 0;
    } else if (mActionStatus != ACTION_ENDING) {
        bool temp;
        chkAttention();
        clrFlag(0x200 | 0x20);
        switch ((u32)(s32)mState) {
        case STATE_VISIT:
            temp = visit01();
            break;
        case STATE_JITANDA02:
            temp = jitanda02();
            break;
        case STATE_TALK01:
            temp = talk01();
            break;
        case STATE_TALK02:
            temp = talk02();
            break;
        case STATE_DEMO01:
            temp = demo01();
            break;
        case STATE_DEMO02:
            temp = demo02();
            break;
        case STATE_DEMO03:
            temp = demo03();
            break;
        default:
            temp = false;
            break;
        }
        lookBack();
        setAttention(temp);
    }
    return TRUE;
}
VERIFY(0x0229A17C, &daNpc_Mk_c::visit_action);

/* 0229A3C8 */
BOOL daNpc_Mk_c::seek_action(void*) {
    WWHD_FUNC(0x0229A3C8, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        if (dComIfGs_isTmpBit(0x0002)) {
            mState = STATE_RUNAWAY;
            MkStatic_init(&mMkStatic, 0x50, 0x12C);
            MkStatic_setRndPathPos(&mMkStatic, this, &field_0x688);
        } else {
            mState = STATE_CLIMB01;
        }
        setAnmStatus();
        mActionStatus = mActionStatus + 1;
    } else if (mActionStatus != ACTION_ENDING) {
        chkAttention();
        clrFlag(0x200 | 0x20);
        bool temp;
        switch ((u32)(s32)mState) {
        case STATE_CLIMB01:
            temp = climb01();
            break;
        case STATE_DROP01:
            temp = drop01();
            break;
        case STATE_RUNAWAY:
            temp = runaway();
            break;
        case STATE_JUMP:
            temp = jump();
            break;
        case STATE_JITANDA01:
            temp = jitanda01();
            break;
        case STATE_JITANDA02:
            temp = jitanda02();
            break;
        case STATE_DEMO01:
            temp = demo01();
            break;
        case STATE_DEMO02:
            temp = demo02();
            break;
        case STATE_DEMO03:
            temp = demo03();
            break;
        case STATE_TALK02:
            temp = talk02();
            break;
        case STATE_13:
            temp = false;
            setAction(MK_HIND_ACTION);
            break;
        default:
            temp = false;
            break;
        }
        lookBack();
        if (chkFlag(0x40)) {
            temp = true;
        }
        setAttention(temp);
    }
    return TRUE;
}
VERIFY(0x0229A3C8, &daNpc_Mk_c::seek_action);

/* 0229A7A4: static initialisation of the translation unit (only the per-TU header statics) */
static void __sinit_d_a_npc_mk_cpp() {
    WWHD_FUNC(0x0229A7A4, void);
    sinit_header_statics(0x10467C6C, 0x101C1FA4);
}
VERIFY(0x0229A7A4, __sinit_d_a_npc_mk_cpp);

/* 0229A838: sead::SafeString deleting destructor (this TU's copy; vtable 0x1001E1E8 slot +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x0229A838, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x0229A838, SafeString_dt);

/* 0229A84C: daNpc_Mk_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Mk_c_dt(daNpc_Mk_c* i_this, s32 flags) {
    WWHD_FUNC(0x0229A84C, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1001E220);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1001E230);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x025D50BC, i_this, 0);            /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0229A84C, daNpc_Mk_c_dt);

/* 0229A8E8: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0229A8E8, void, (SafeString*)nullptr);
}
VERIFY(0x0229A8E8, SafeString_assureTerminationImpl);
