/**
 * d_a_npc_uk.cpp (WWHD)
 * NPC - Jin, Jan, & Jun-Roberto (the Killer Bees of Windfall)
 *
 * The GameCube decompilation of this TU
 * (zeldaret/tww src/d/actor/d_a_npc_uk.cpp) has only "Nonmatching" placeholders: every function
 * below is written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube
 * names and the TU's function list. Assert texts in the image give some original names
 * (m_maba_tex_pattern, table_bmt, head_bdl_table, headModelData, mk). "HD:" marks a known
 * difference from the GameCube build.
 */
#include "d/actor/d_a_npc_uk.h"

#define SAFESTRING_VTBL 0x10022FF8 /* this TU's sead::SafeString vtable */
#define UK_VTBL 0x10023050         /* daNpc_Uk_c vtable (HD virtual destructor) */

enum { fpcNm_NPC_UK_e = 0x170 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline dSv_event_c* dComIfGs_tmpEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline BOOL dComIfGs_isTmpBit(u16 f) { return dSv_event_isEventBit(dComIfGs_tmpEvent(), f); }
static inline void dComIfGs_onTmpBit(u16 f) { dSv_event_onEventBit(dComIfGs_tmpEvent(), f); }
/* 02520C0C dComIfGs_checkGetItem(u8) */
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
/* dComIfGp_getMesgAnimeAttrInfo(): play + 0x5BC5 */
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + 0x5BC5); }
/* daPy_getPlayerLinkActorClass(): play + 0x5B34 */
static inline fopAc_ac_c* daPy_getPlayerLinkActorClass() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B34)); }
/* dComIfGp_event_setTalkPartner(a): dEvt_control_c (play + 0x51D0) mPtTalk (+0xCC) = getPId(a) */
static inline void dComIfGp_event_setTalkPartner(fopAc_ac_c* a) {
    u32 evt = dComIfGp_ea() + 0x51D0;
    gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, a));
}
/* 02542EDC dEvent_manager_c::getMyActIdx(staffId, table, n, force, nameType) */
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
/* the s16 event index is passed on as the callee returned it (GHS does not re-extend a s16 result) */
static inline u32 evmng_getEventIdx_raw(const char* name, u8 evNo) {
    return gabi::call<u32>(0x02543F10, dComIfGp_getPEvtManager(), name, evNo);
}
/* 025D7874 fopAcM_orderChangeEventId(actor, s16 eventIdx, u16 flag, u16 hind) */
static inline BOOL fopAcM_orderChangeEventId_raw(fopAc_ac_c* a, u32 ev, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D7874, a, ev, flag, hind);
}
static inline BOOL fopAcM_orderOtherEventId_raw(fopAc_ac_c* a, u32 ev, u8 mapToolId, u16 p3, u16 prio, u16 flag) {
    return gabi::call<BOOL>(0x025D7A58, a, ev, mapToolId, p3, prio, flag);
}
/* 025D7C6C fopAcM_getTalkEventPartner(actor) */
static inline fopAc_ac_c* fopAcM_getTalkEventPartner(fopAc_ac_c* a) { return gabi::call<fopAc_ac_c*>(0x025D7C6C, a); }
/* 025D9F38 fopAcM_searchFromName(name, param mask, param) */
static inline fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm) {
    return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm);
}
/* 025E1988 HD: mDoAud_seStart(id) (one argument) */
static inline void mDoAud_seStart_1(u32 id) { gabi::call(0x025E1988, id); }
/* HD message manager (*(0x101F4B5C)): 025F795C status, 025F74D0 setStatus, 025F7DB0 messageSet(msgNo, cXyz* pos) */
static inline u32 msgMgr() { return gabi::load<u32>(0x101F4B5C); }
static inline u16 msgMgr_getStatus(u32 m) { return gabi::call<u16>(0x025F795C, m); }
static inline void msgMgr_setStatus(u32 m, u32 st) { gabi::call(0x025F74D0, m, st); }
static inline u32 msgMgr_messageSet(u32 m, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, m, msgNo, pos); }
/* 0259D454 dNpc_setAnm(morf, loopMode, morf, speed, anmIdx, soundIdx, arc) */
static inline BOOL dNpc_setAnm(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, s32 anmIdx, s32 soundIdx, const char* arc) {
    return gabi::call<BOOL>(0x0259D454, morf, loopMode, morfF, speed, anmIdx, soundIdx, arc);
}
/* 0259E6D0 dNpc_PathRun_c::setInf(u8 pathIdx, s8 roomNo, u8 forward) */
static inline BOOL dNpc_PathRun_setInf(MkPath_l* p, u8 path, s8 room, u8 fwd) { return gabi::call<BOOL>(0x0259E6D0, p, path, room, fwd); }
static inline s32 J3DAnm_getFrameMax(J3DAnmTexPattern* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                                     s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
/* 027F3F94 (matcher: __nw): J3DModelData joint-tree header; +8 joint count */
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline u32 dBgS_GetMtrlSndId_l(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
/* dAttention_c (play + 0x5804): 024EC8D0 is LockonTarget, 024EE464 ActionTarget (matcher swapped) */
static inline bool dAttention_LockonTruth(u32 a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(u32 a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(u32 a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
/* daNpc_Mk_Static_c (d_a_npc_mk_static, another TU) */
static inline BOOL Mk_chkGameSet(daNpc_Mk_Static_c* m) { return gabi::call<BOOL>(0x0229B54C, m); }
static inline void Mk_init(daNpc_Mk_Static_c* m, u8 a, u16 b) { gabi::call(0x0229AD60, m, a, b); }
static inline void Mk_aroundWalk(daNpc_Mk_Static_c* m, fopAc_ac_c* a, fopAc_ac_c* b, u8 c) { gabi::call(0x0229AB78, m, a, b, c); }
static inline f32 Mk_getSpeedF(daNpc_Mk_Static_c* m, f32 a, f32 b) { return gabi::call<f32>(0x0229ACC0, m, a, b); }
static inline u32 Mk_runAwayProc(daNpc_Mk_Static_c* m, fopAc_ac_c* a, MkPath_l* p, void* cyl, be<s16>* ang) {
    return gabi::call<u32>(0x0229B164, m, a, p, cyl, ang);
}
static inline void Mk_setRndPathPos(daNpc_Mk_Static_c* m, fopAc_ac_c* a, MkPath_l* p) { gabi::call(0x0229B5F0, m, a, p); }
static inline BOOL mMk_chkPointPass(daNpc_Mk_Static_c* m, cXyz* a, cXyz* b, cXyz* c) { return gabi::call<BOOL>(0x0229B768, m, a, b, c); }
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz (pointers to copies), f32* dist, s16* angle) */
static inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* ang) { gabi::call(0x0259D624, a, b, dist, ang); }
/* 0253E9B0 dEvt_info_c::setEventName(name) (eventInfo at actor + 0xF8) */
static inline void dEvt_info_setEventName(fopAc_ac_c* a, const char* name) { gabi::call(0x0253E9B0, gabi::ea(a) + 0xF8, name); }
/* mDoLib_clipper::getFar(): the float at 0x1048D04C */
static inline f32 mDoLib_clipper_getFar() { return gabi::load<f32>(0x1048D04C); }

/* GHS pointer to member function call (this, void*) */
static inline BOOL pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        return gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        return gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}

/* 12-word matrix copy: all loads, then all stores (lfs/stfs: an SNaN is quieted) */
static inline void mtx_wcopy(Mtx34* dst, const Mtx34* src) {
    u32 t[12];
    for (int i = 0; i < 12; i++) t[i] = gabi::load<u32>(gabi::ea(src) + 4 * i);
    for (int i = 0; i < 12; i++) {
        u32 v = t[i];
        if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x003FFFFFu) != 0 && !(v & 0x00400000u))
            v |= 0x00400000u;
        gmem_stf32(gabi::ea(dst) + 4 * i, v);
    }
}

/* three bit-exact words (cXyz copied with lwz/stw) */
static inline void wcopy3(u32 dst, u32 src) {
    for (u32 i = 0; i < 12; i += 4) gabi::store<u32>(dst + i, gabi::load<u32>(src + i));
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
/* l_msgId (HD: the message is reached through the message manager) */
static be<u32>& l_msgId() { return *gabi::at<be<u32>>(0x10468920); }
/* l_bck_ix_tbl (s32, .rodata 0x10023060), table_bmt {-1, 0x1C, 0x1D} (built on the stack),
 * head_bdl_table (s32[3], .data 0x101C6954), l_msg_anm_tbl (u8[8], 0x10023220),
 * staff names "UkC"/"UkD"/"UkB" and type 2 "UkB2"/"UkC2"/"UkD2" (0x10023260),
 * action_table[15] (.data 0x101C6960) */

/* setAction(target, NULL) (inline): end the current action, start the new one */
static inline void uk_setAction(daNpc_Uk_c* i_this, u32 target) {
    ProcFunc_l* cur = &i_this->mCurrActionFunc;
    bool same = false;
    bool callOld = true;
    s16 i = cur->i;
    if (i == -1) {
        same = cur->d == 0 && cur->f == target;
    } else if (i == 0) {
        callOld = false;
    }
    if (!same) {
        if (callOld) {
            i_this->mActionStatus = -1; /* ACTION_ENDING */
            pmf_call(i_this, cur, nullptr);
        }
        i_this->mActionStatus = 0; /* ACTION_STARTING */
        cur->d = 0;
        cur->i = -1;
        cur->f = target;
        gabi::call_ptr<BOOL>(cur->f, i_this, (void*)nullptr);
    }
}

/* 022F08DC */
static BOOL nodeCallBack_Uk(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022F08DC, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_Uk_c* i_this = gabi::at<daNpc_Uk_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        u32 jntNo = jntNo_of(node);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
                gabi::Local<cXyz> temp;
                gabi::Local<cXyz> temp2;
                temp->x = 0.0f;
                temp->y = 0.0f;
                temp->z = 0.0f;
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
                cMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][0]);
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
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
                cMtx_ZrotM(calc_mtx(), i_this->m_jnt.mAngles[1][0]);
            }
            PSMTXCopy(calc_mtx(), j3dSys_mCurrentMtx());
            mtx_wcopy(getAnmMtx(model, jntNo), calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        }
    }
    return TRUE;
}
VERIFY(0x022F08DC, nodeCallBack_Uk);

/* 022F0B68 */
u32 daNpc_Uk_c::getShapeType() {
    WWHD_FUNC(0x022F0B68, u32, this);
    return (fopAcM_GetParam(this) >> 16) & 0xF;
}
VERIFY(0x022F0B68, &daNpc_Uk_c::getShapeType);

/* 022F0B74 */
BOOL daNpc_Uk_c::initTexPatternAnm(u32 i_modify) {
    WWHD_FUNC(0x022F0B74, BOOL, this, i_modify);
    J3DModelData* modelData = J3DModel_getModelData_l(mpHeadModel);
    if (getShapeType() != 1) {
        return TRUE;
    }
    J3DAnmTexPattern* pat = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(0x1002309C) /* "Uk" */, gabi::load<s32>(0x10022FF4), SAFESTRING_VTBL);
    m_head_tex_pattern = pat;
    if (pat == nullptr) { /* JUT_ASSERT(0x2B0, m_maba_tex_pattern != NULL) */
        JUT_ASSERT_fail(STR(0x100230BC), 0x2B0, STR(0x100230A0));
        pat = m_head_tex_pattern;
    }
    if (!mDoExt_btpAnm_init(mBtpAnm, modelData, pat, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, i_modify, FALSE)) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x022F0B74, &daNpc_Uk_c::initTexPatternAnm);

/* 022F0CA0 */
BOOL daNpc_Uk_c::CreateHeap() {
    WWHD_FUNC(0x022F0CA0, BOOL, this);
    u32 shape = getShapeType();
    J3DModelData* modelData;
    if (shape == 0) {
        modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x100230DC) /* "Uk" */, 0x18, SAFESTRING_VTBL);
    } else {
        /* static const int table_bmt[] = {-1, 0x1C, 0x1D} (built on the stack) */
        static const s32 table_bmt[3] = {-1, 0x1C, 0x1D};
        if (shape >= 3) {
            JUT_ASSERT_fail(STR(0x10023130), 0xAF7, STR(0x100230E8));
            if (shape >= 3) return FALSE;
        }
        modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x100230DC), table_bmt[shape], SAFESTRING_VTBL);
    }
    if (modelData == nullptr) {
        return FALSE;
    }
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x100230DC), 0x10, SAFESTRING_VTBL);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x15021222);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    s8 head = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x100230E0) /* "head" */);
    m_jnt.mHeadJntNum = head;
    if (head < 0) /* JUT_ASSERT(0xB19, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail(STR(0x10023130), 0xB19, STR(0x10023140));
    s8 backbone = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x1002315C) /* "backbone" */);
    m_jnt.mBackboneJntNum = backbone;
    if (backbone < 0) /* JUT_ASSERT(0xB1E, m_jnt.getBackboneJntNum() >= 0) */
        JUT_ASSERT_fail(STR(0x10023130), 0xB1E, STR(0x10023168));
    u32 shape2 = getShapeType();
    if (shape2 >= 3) { /* JUT_ASSERT(0xB34, shape_type < ARRAY_SIZE(head_bdl_table)) */
        JUT_ASSERT_fail(STR(0x10023130), 0xB34, STR(0x10023188));
        if (shape2 >= 3) return FALSE;
    }
    J3DModelData* headModelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x100230DC), gabi::load<s32>(0x101C6954 + shape2 * 4) /* head_bdl_table */, SAFESTRING_VTBL);
    if (headModelData == nullptr) { /* JUT_ASSERT(0xB38, headModelData) */
        JUT_ASSERT_fail(STR(0x10023130), 0xB38, STR(0x10023120));
        return FALSE;
    }
    mpHeadModel = mDoExt_J3DModel__create(headModelData, 0x80000, 0x11020022);
    if (mpHeadModel.get() == nullptr) {
        return FALSE;
    }
    mTexPatternIdx = 0;
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    for (u16 i = 0; i < J3DModelData_getJointNum(modelData); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Uk) */
            J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
            u32 n = gabi::load<u32>(gabi::ea(md) + 4);
            u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
            if ((u32)i < n)
                joint += i * 0x1C;
            gabi::store<u32>(joint + 8, 0x022F08DC);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    mAcchCir.SetWall(60.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mPartnerId = 0xFFFFFFFF;
    maxFallSpeed = -90.0f;
    mObjAcch.SetGroundCheckOffset(100.0f);
    return TRUE;
}
VERIFY(0x022F0CA0, &daNpc_Uk_c::CreateHeap);

/* 022F105C (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022F105C, BOOL, i_this);
    return ((daNpc_Uk_c*)i_this)->CreateHeap();
}
VERIFY(0x022F105C, CheckCreateHeap);

/* 022F1060 (matcher: unnamed; GameCube getType) */
u8 daNpc_Uk_c::getType() {
    WWHD_FUNC(0x022F1060, u8, this);
    return fopAcM_GetParam(this) & 0xFF;
}
VERIFY(0x022F1060, &daNpc_Uk_c::getType);

/* 022F1068 */
u16 daNpc_Uk_c::getCaughtFlag() {
    WWHD_FUNC(0x022F1068, u16, this);
    switch (getShapeType()) {
    case 1:
        return 0x8;
    case 2:
        return 0x4;
    default:
        return 0x10;
    }
}
VERIFY(0x022F1068, &daNpc_Uk_c::getCaughtFlag);

/* 022F10B8 */
BOOL daNpc_Uk_c::chkGameStart() {
    WWHD_FUNC(0x022F10B8, BOOL, this);
    if (dComIfGs_isTmpBit(0x40) && !dComIfGs_isTmpBit(getCaughtFlag())) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022F10B8, &daNpc_Uk_c::chkGameStart);

/* 022F1144 */
u8 daNpc_Uk_c::getPath() {
    WWHD_FUNC(0x022F1144, u8, this);
    return (fopAcM_GetParam(this) >> 8) & 0xFF;
}
VERIFY(0x022F1144, &daNpc_Uk_c::getPath);

/* 022F1150 */
BOOL daNpc_Uk_c::init() {
    WWHD_FUNC(0x022F1150, BOOL, this);
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xA9); /* attention_info.distances[TALK] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances[SPEAK] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = ACTION_SPEAK | LOCKON_TALK */
    gravity = -3.3f;
    J3DModel* model = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mtx_wcopy(gabi::at<Mtx34>(gabi::ea(model) + 0xC8), mDoMtx_stack_c::get()); /* model->setBaseTRMtx */
    mpMorf->calc();
    mStts.Init(0x64, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    actor_status = (actor_status & ~0x3Fu) | 0x27; /* fopAcM_SetStatusMap(0x27) + OnStatus(SHOWMAP) */
    switch ((s8)mType) {
    case 0:
        uk_setAction(this, 0x022F5DB0); /* wait_action */
        break;
    case 1:
        if (chkGameStart()) {
            uk_setAction(this, 0x022F5E90); /* hind_action */
            actor_status &= ~0x20u;
            setFlag(0x10);
        } else {
            uk_setAction(this, 0x022F6058); /* visit_action */
        }
        break;
    case 2:
        mStts.SetWeight(0xFE);
        dNpc_PathRun_setInf(&mPath, getPath(), current.roomNo, 1);
        if (mPath.path.get() == nullptr) {
            return FALSE;
        }
        if (chkGameStart()) {
            uk_setAction(this, 0x022F6360); /* seek_action */
        } else {
            setFlag(0x10);
            actor_status &= ~0x20u;
            uk_setAction(this, 0x022F5E90); /* hind_action */
        }
        actor_status |= 0x40u;
        break;
    }
    mEyePos.copy(current.pos);
    mAttnBasePos.copy(current.pos);
    f32 far = mDoLib_clipper_getFar();
    if (far > 1.0f) {
        cullSizeFar = 5000.0f / far; /* fopAcM_setCullSizeFar */
    }
    mMtrlSndId = 0;
    mReverb = (s8)dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    return TRUE;
}
VERIFY(0x022F1150, &daNpc_Uk_c::init);

/* 022F193C */
cPhs_State daNpc_Uk_c::_create() {
    WWHD_FUNC(0x022F193C, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Uk_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025D4ED0, this); /* fopAc_ac_c::fopAc_ac_c */
            __vtbl = UK_VTBL;
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mObjAcch, dBgS_ObjAcch_vt{0x10023020, 0x10023040, 0x10023030});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x10023010);
            gabi::call(0x0259DAA0, &m_jnt); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x100231FC) /* "Uk" */);
    if (state == cPhs_COMPLEATE_e) {
        if (fpcM_GetName(this) != fpcNm_NPC_UK_e) {
            return cPhs_ERROR_e;
        }
        switch (getType()) {
        case 1:
            mType = 1;
            switch (getShapeType()) {
            case 0:
                m808 = 2.5f;
                m80C = 10.0f;
                break;
            case 1:
                m808 = 2.3f;
                m80C = 8.5f;
                break;
            case 2:
                m808 = 2.0f;
                m80C = 7.0f;
                break;
            }
            break;
        case 2:
            mType = 2;
            argument = (s8)(getShapeType() + 5);
            m80C = 14.0f;
            break;
        default:
            mType = 0;
            break;
        }
        if (!fopAcM_entrySolidHeap(this, (heapCallbackFunc)0x022F105C /* CheckCreateHeap */, 0xB7B0)) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
        tevStr.mRoomNo = current.roomNo;
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        fopAcM_setCullSizeBox(this, -35.0f, -10.0f, -35.0f, 35.0f, 100.0f, 35.0f);
        if (!init()) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
    }
    return state;
}
VERIFY(0x022F193C, &daNpc_Uk_c::_create);

/* 022F1CC0 */
static cPhs_State daNpc_Uk_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022F1CC0, cPhs_State, i_this);
    return ((daNpc_Uk_c*)i_this)->_create();
}
VERIFY(0x022F1CC0, daNpc_Uk_Create);

/* 022F1CC4 */
BOOL daNpc_Uk_c::_delete() {
    WWHD_FUNC(0x022F1CC4, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x100231FF) /* "Uk" */);
    if (mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x022F1CC4, &daNpc_Uk_c::_delete);

/* 022F1D10 */
static BOOL daNpc_Uk_Delete(daNpc_Uk_c* i_this) {
    WWHD_FUNC(0x022F1D10, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x022F1D10, daNpc_Uk_Delete);

/* 022F1D14 */
void daNpc_Uk_c::playTexPatternAnm() {
    WWHD_FUNC(0x022F1D14, void, this);
    if (getShapeType() == 1 && cLib_calcTimer(&mBlinkTimer) == 0) {
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
VERIFY(0x022F1D14, &daNpc_Uk_c::playTexPatternAnm);

/* 022F1DE4 */
void daNpc_Uk_c::setAnm(s8 anmIdx, u8 force) {
    WWHD_FUNC(0x022F1DE4, void, this, anmIdx, force);
    f32 morf = 8.0f;
    if (anmIdx == 4 && mCurrAnmIdx == 10) {
        morf = 0.0f;
    }
    if (force & 1) {
        morf = 0.0f;
        mCurrAnmIdx = 11;
    }
    if (anmIdx != mCurrAnmIdx && anmIdx != -1) {
        mAnmTimer = 0.0f;
        mCurrAnmIdx = anmIdx;
        dNpc_setAnm(mpMorf, -1, morf, 1.0f, gabi::load<s32>(0x10023060 + 4 * anmIdx) /* l_bck_ix_tbl */, -1, STR(0x10023208) /* "Uk" */);
    }
}
VERIFY(0x022F1DE4, &daNpc_Uk_c::setAnm);

/* 022F1E80 */
void daNpc_Uk_c::talkInit() {
    WWHD_FUNC(0x022F1E80, void, this);
    mTalkState = 0;
    mMsgAnmIdx = 0xFF;
}
VERIFY(0x022F1E80, &daNpc_Uk_c::talkInit);

/* 022F1E94 */
void daNpc_Uk_c::checkOrder() {
    WWHD_FUNC(0x022F1E94, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd != 2 /* !checkCommandDemoAccrpt() */) {
        if (cmd == 1 /* checkCommandTalk() */ && ChkOrder(3)) {
            setFlag(0x1);
            talkInit();
        }
    }
    ClrOrder();
}
VERIFY(0x022F1E94, &daNpc_Uk_c::checkOrder);

/* 022F1EE8 */
void daNpc_Uk_c::eventOrder() {
    WWHD_FUNC(0x022F1EE8, void, this);
    if (ChkOrder(3)) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (ChkOrder(2)) {
            fopAcM_orderSpeakEvent(this);
        }
    }
}
VERIFY(0x022F1EE8, &daNpc_Uk_c::eventOrder);

/* 022F1F10 */
void daNpc_Uk_c::setCollision() {
    WWHD_FUNC(0x022F1F10, void, this);
    gabi::Local<cXyz> centerPos;
    centerPos->copy(current.pos);
    mCyl.SetC(centerPos);
    mCyl.SetR(30.0f);
    mCyl.SetH(80.0f);
    dComIfG_Ccsp_Set(&mCyl);
}
VERIFY(0x022F1F10, &daNpc_Uk_c::setCollision);

/* 022F1F98 */
BOOL daNpc_Uk_c::_execute() {
    WWHD_FUNC(0x022F1F98, BOOL, this);
    m_jnt.setParam(0, 0x1C84, 0, -0x1C84, 0x1F40, 0x2328, -0x7D0, -0x2328, 0x3E8);
    playTexPatternAnm();
    switch ((s32)mCurrAnmIdx) {
    case 3: {
        f32 rate = speedF * 0.5f;
        if (rate < 1.0f) {
            rate = 1.0f;
        } else if (rate - 1.5f >= 0.0f) {
            rate = 1.5f;
        }
        mpMorf->setPlaySpeed(rate);
        break;
    }
    case 4: {
        f32 rate = speedF * 0.2f;
        if (rate < 1.0f) {
            rate = 1.0f;
        } else if (rate - 1.5f >= 0.0f) {
            rate = 1.5f;
        }
        mpMorf->setPlaySpeed(rate);
        break;
    }
    }
    mAnmEnded = (s8)mpMorf->play(&eyePos, mMtrlSndId, mReverb);
    if (mpMorf->getFrame() < mAnmTimer) {
        mAnmEnded = 1;
    }
    mAnmTimer = mpMorf->getFrame();
    if (mAnmEnded != 0) {
        switch ((s32)mCurrAnmIdx) {
        case 6:
            if (cM_rnd() < 0.4f) {
                setAnm(7, 0);
            }
            break;
        case 7:
            setAnm(6, 0);
            break;
        }
    }
    checkOrder();
    pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
    eventOrder();
    if (!chkFlag(0x100)) {
        shape_angle.y = current.angle.y;
    }
    clrFlag(0x100);
    mMtrlSndId = 0;
    if (!chkFlag(0x10)) {
        if (chkFlag(0x8)) {
            f32 y = gabi::fadds_ppc(speed.y, gravity);
            speed.y = y;
            if (y < maxFallSpeed) {
                speed.y = maxFallSpeed;
            }
            fopAcM_posMove(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        } else {
            fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts)));
        }
        mObjAcch.CrrPos(dComIfG_Bgsp());
        void* gnd = gabi::at<u8>(gabi::ea(&mObjAcch) + 0xD4 + 0x14); /* mObjAcch.m_gnd */
        if (!mObjAcch.ChkGroundHit()) {
            setFlag(0x200);
        } else {
            clrFlag(0x200);
            mMtrlSndId = dBgS_GetMtrlSndId_l(dComIfG_Bgsp(), gnd);
        }
        tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gnd);
        gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), gnd)); /* tevStr.mEnvrIdxOverride */
    } else {
        setFlag(0x200);
    }
    clrFlag(0x8);
    J3DModel* pModel = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mtx_wcopy(gabi::at<Mtx34>(gabi::ea(pModel) + 0xC8), mDoMtx_stack_c::get()); /* pModel->setBaseTRMtx */
    mpMorf->calc();
    if (chkFlag(0x10)) {
        mCyl.OffCoSPrmBit(1);
    } else {
        mCyl.OnCoSPrmBit(1);
        setCollision();
    }
    return TRUE;
}
VERIFY(0x022F1F98, &daNpc_Uk_c::_execute);

/* 022F251C */
static BOOL daNpc_Uk_Execute(daNpc_Uk_c* i_this) {
    WWHD_FUNC(0x022F251C, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x022F251C, daNpc_Uk_Execute);

/* 022F2520 */
BOOL daNpc_Uk_c::_draw() {
    WWHD_FUNC(0x022F2520, BOOL, this);
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pHeadData = J3DModel_getModelData_l(mpHeadModel);
    if (chkFlag(0x10)) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpHeadModel, &tevStr);
    if (getShapeType() == 1) {
        mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, pHeadData, mBlinkFrame);
    }
    mpMorf->entryDL();
    Mtx34* headMtx = getAnmMtx(pModel, m_jnt.mHeadJntNum);
    mtx_wcopy(gabi::at<Mtx34>(gabi::ea(mpHeadModel.get()) + 0xC8), headMtx); /* mpHeadModel->setBaseTRMtx */
    mDoExt_modelUpdateDL(mpHeadModel, 0);
    u32 shape = getShapeType();
    if (shape == 1) {
        gabi::store<u32>(gabi::ea(pHeadData) + 0x38, 0); /* mBtpAnm.remove(pHeadData) */
        shape = getShapeType();
    }
    /* HD: no dComIfGd_setShadow */
    if (shape <= 2) {
        dSnap_RegistFig(0x61 /* DSNAP_TYPE_NPC_UK */, this, 1.0f, 1.0f, 1.0f);
    }
    return TRUE;
}
VERIFY(0x022F2520, &daNpc_Uk_c::_draw);

/* 022F26AC */
static BOOL daNpc_Uk_Draw(daNpc_Uk_c* i_this) {
    WWHD_FUNC(0x022F26AC, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x022F26AC, daNpc_Uk_Draw);

/* 022F26B0 */
static BOOL daNpc_Uk_IsDelete(daNpc_Uk_c*) {
    WWHD_FUNC(0x022F26B0, BOOL, (daNpc_Uk_c*)nullptr);
    return TRUE;
}
VERIFY(0x022F26B0, daNpc_Uk_IsDelete);

/* 022F26B8 */
u16 daNpc_Uk_c::getFoundFlag() {
    WWHD_FUNC(0x022F26B8, u16, this);
    switch (getShapeType()) {
    case 1:
        return 0x180;
    case 2:
        return 0x140;
    default:
        return 0x1;
    }
}
VERIFY(0x022F26B8, &daNpc_Uk_c::getFoundFlag);

/* 022F2708 */
void daNpc_Uk_c::setAnmStatus() {
    WWHD_FUNC(0x022F2708, void, this);
    switch ((s32)mState) {
    case 6:
    case 7:
    case 8:
        setAnm(5, 0);
        break;
    case 12:
    case 13:
        break;
    default:
        setAnm(0, 0);
        break;
    }
}
VERIFY(0x022F2708, &daNpc_Uk_c::setAnmStatus);

/* 022F2750 */
BOOL daNpc_Uk_c::chkAttentionLocal() {
    WWHD_FUNC(0x022F2750, BOOL, this);
    u32 attention = dComIfGp_ea() + 0x5804; /* dComIfGp_getAttention() */
    if (chkFlag(0x1)) {
        return TRUE;
    }
    if (mTalkWait != 0) {
        mTalkWait = mTalkWait - 1;
        return TRUE;
    }
    if (dAttention_LockonTruth(attention)) {
        return this == dAttention_LockonTarget(attention, 0);
    }
    return this == dAttention_ActionTarget(attention, 0);
}
VERIFY(0x022F2750, &daNpc_Uk_c::chkAttentionLocal);

/* 022F2830 */
void daNpc_Uk_c::chkAttention() {
    WWHD_FUNC(0x022F2830, void, this);
    u32 old = (mFlags >> 2) & 1;
    if (chkAttentionLocal()) {
        setFlag(0x4);
    } else {
        clrFlag(0x4);
    }
    if (old != ((mFlags >> 2) & 1u) && old != 0) {
        m_jnt.mbTrn = 1; /* setTrn() */
    }
}
VERIFY(0x022F2830, &daNpc_Uk_c::chkAttention);

/* 022F28B4 (matcher: unnamed) */
u32 daNpc_Uk_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022F28B4, u32, this, pMsgNo);
    switch ((u32)*pMsgNo) {
    case 0x26BF:
    case 0x26C1:
    case 0x26C3:
    case 0x26D8:
        *pMsgNo = *pMsgNo + 1;
        return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    }
    return 0x10; /* fopMsgStts_MSG_ENDS_e */
}
VERIFY(0x022F28B4, &daNpc_Uk_c::next_msgStatus);

/* 022F28F0 */
u16 daNpc_Uk_c::getFirstTalk() {
    WWHD_FUNC(0x022F28F0, u16, this);
    switch (getShapeType()) {
    case 0:
        return 0x1202;
    case 1:
        return 0x1204;
    default:
        return 0x1201;
    }
}
VERIFY(0x022F28F0, &daNpc_Uk_c::getFirstTalk);

/* 022F293C */
u32 daNpc_Uk_c::getMsg() {
    WWHD_FUNC(0x022F293C, u32, this);
    u32 msgNo;
    switch ((s8)mType) {
    case 1:
        if (mState == 3 || mState == 4) {
            return mMsgNo;
        }
        if (dComIfGs_isTmpBit(0x40)) {
            return getShapeType() + 0x26B6;
        }
        if (dComIfGs_isEventBit(0x1340)) {
            return getShapeType() + 0x26AE;
        }
        if (dComIfGs_isEventBit(getFirstTalk())) {
            return getShapeType() + 0x26BA;
        }
        msgNo = getShapeType() * 2 + 0x26BF;
        dComIfGs_onEventBit(getFirstTalk());
        return msgNo;
    case 2:
        if (dComIfGs_isTmpBit(getCaughtFlag())) {
            return getShapeType() + 0x26B6;
        }
        msgNo = getShapeType() + 0x26B2;
        dComIfGs_onTmpBit(getCaughtFlag());
        return msgNo;
    default:
        return 0;
    }
}
VERIFY(0x022F293C, &daNpc_Uk_c::getMsg);

/* 022F2AAC (matcher: unnamed) */
void daNpc_Uk_c::msgAnm(u32 attr) {
    WWHD_FUNC(0x022F2AAC, void, this, attr);
    /* u8 argument, compared unnormalised (callers pass a lbz value) */
    if ((u32)mMsgAnmIdx != attr) {
        mMsgAnmIdx = (u8)attr;
        if (attr <= 7) {
            setAnm((s8)gabi::load<u8>(0x10023220 + attr) /* l_msg_anm_tbl */, 0);
        }
    }
}
VERIFY(0x022F2AAC, &daNpc_Uk_c::msgAnm);

/* 022F2ADC */
u16 daNpc_Uk_c::talk() {
    WWHD_FUNC(0x022F2ADC, u16, this);
    /* HD: the message is the message manager (GameCube: l_msg found by l_msgId) */
    u32 mgr = msgMgr();
    u16 msgStatus = 0xFF;
    s8 talkState = mTalkState;
    if (talkState == 0) {
        l_msgId() = 0xFFFFFFFF; /* fpcM_ERROR_PROCESS_ID_e */
        mCurrMsgNo = getMsg();
        mTalkState = 1;
    } else if (talkState != -1) {
        if (l_msgId() == 0xFFFFFFFF) {
            l_msgId() = msgMgr_messageSet(mgr, mCurrMsgNo, &eyePos);
        } else {
            if (!chkFlag(0x400)) {
                msgAnm(dComIfGp_getMesgAnimeAttrInfo());
                talkState = mTalkState;
            }
            switch ((s32)talkState) {
            case 1:
                mTalkState = 2; /* HD: no fopMsgM_SearchByID */
                if (chkFlag(0x80)) {
                    dComIfGp_event_setTalkPartner(this);
                    clrFlag(0x80);
                }
                break;
            case 2:
                msgStatus = msgMgr_getStatus(mgr);
                if (msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
                    msgMgr_setStatus(mgr, next_msgStatus(&mCurrMsgNo));
                    if (msgMgr_getStatus(mgr) == 0xF) {
                        msgMgr_messageSet(mgr, mCurrMsgNo, nullptr);
                    }
                } else if (msgStatus == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
                    msgMgr_setStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
                    mTalkState = -1;
                }
                break;
            }
        }
    }
    return msgStatus;
}
VERIFY(0x022F2ADC, &daNpc_Uk_c::talk);

/* 022F2C80 */
void daNpc_Uk_c::setAttention(u32 param_1) {
    /* bool argument, tested unnormalised (callers pass a BOOL result) */
    WWHD_FUNC(0x022F2C80, void, this, param_1);
    if (!param_1 && mAttnSetCount >= 2) {
        return;
    }
    u32 att = gabi::ea(this) + 0x390; /* attention_info.position */
    gabi::store<f32>(att, mAttnBasePos.x);
    gabi::store<f32>(att + 4, gabi::fadds_ppc(mAttnBasePos.y, 45.0f));
    gabi::store<f32>(att + 8, mAttnBasePos.z);
    eyePos.z = mEyePos.z; /* eyePos.set(mEyePos.x, mEyePos.y, mEyePos.z) */
    eyePos.x = mEyePos.x;
    eyePos.y = mEyePos.y;
}
VERIFY(0x022F2C80, &daNpc_Uk_c::setAttention);

/* 022F2CD4 */
u32 daNpc_Uk_c::getLookBackMode() {
    WWHD_FUNC(0x022F2CD4, u32, this);
    fopAc_ac_c* mk = fopAcM_SearchByID(mPartnerId);
    if (mk != nullptr && mState == 2) {
        u8 visitMode = mVisitMode;
        if (visitMode == 8 || visitMode == 4) {
            return 3;
        }
        u8 mkMode = gabi::load<u8>(gabi::ea(mk) + 0x818); /* daNpc_Mk_c mode */
        if (mkMode == 8 || mkMode == 9) {
            return 2;
        }
    }
    if (chkFlag(0x40)) {
        if (fopAcM_getTalkEventPartner(daPy_getPlayerLinkActorClass()) != this) {
            return 4;
        }
        return 2;
    }
    if (chkFlag(0x20)) {
        return 0;
    }
    if (chkFlag(0x4)) {
        return 1;
    }
    if (mState == 2 && (mVisitMode == 6 || mVisitMode == 7)) {
        return 1;
    }
    return 2;
}
VERIFY(0x022F2CD4, &daNpc_Uk_c::getLookBackMode);

/* 022F2E18 */
void daNpc_Uk_c::lookBack() {
    WWHD_FUNC(0x022F2E18, void, this);
    gabi::Local<cXyz> target;  /* sp+0x08 */
    gabi::Local<cXyz> eye;     /* sp+0x14: passed by value (a copy) */
    gabi::Local<cXyz> eyeRes;  /* sp+0x24: dNpc_playerEyePos result */
    fopAc_ac_c* mk = fopAcM_SearchByID(mPartnerId);
    cXyz* dstPos = nullptr;
    s16 desiredYRot = current.angle.y;
    u8 headOnly = 0;
    u32 mode = getLookBackMode();
    f32 eyeY = eyePos.y;
    f32 posZ = current.pos.z;
    f32 posX = current.pos.x;
    switch (mode) {
    case 0:
        m_jnt.mbTrn = 1; /* setTrn() */
        dNpc_playerEyePos(eyeRes, -20.0f);
        target->copy(*eyeRes);
        dstPos = target;
        break;
    case 1:
        headOnly = 1;
        dNpc_playerEyePos(eyeRes, -20.0f);
        target->copy(*eyeRes);
        dstPos = target;
        break;
    case 2:
        desiredYRot = current.angle.y;
        headOnly = 1;
        break;
    case 3:
        if (mk == nullptr) /* JUT_ASSERT(0x4EE, mk != NULL) */
            JUT_ASSERT_fail(STR(0x10023238), 0x4EE, STR(0x1002322C));
        m_jnt.mbTrn = 1;
        gabi::store<u32>(target->x.addr(), gabi::load<u32>(mk->current.pos.x.addr()));
        target->y = mk->eyePos.y;
        gabi::store<u32>(target->z.addr(), gabi::load<u32>(mk->current.pos.z.addr()));
        dstPos = target;
        break;
    case 4: {
        fopAc_ac_c* partner = fopAcM_getTalkEventPartner(daPy_getPlayerLinkActorClass());
        if (partner == nullptr) {
            partner = daPy_getPlayerLinkActorClass();
        }
        m_jnt.mbTrn = 1;
        target->copy(partner->eyePos);
        posZ = current.pos.z;
        posX = current.pos.x;
        eyeY = eyePos.y;
        dstPos = target;
        break;
    }
    }
    s16 vel;
    if (m_jnt.mbTrn != 0) { /* m_jnt.trnChk() */
        cLib_addCalcAngleS2(&mMaxHeadTurnVelocity, 0x5DC, 4, 0x800);
        vel = mMaxHeadTurnVelocity;
    } else {
        mMaxHeadTurnVelocity = 0;
        vel = 0;
    }
    eye->x = posX;
    eye->y = eyeY;
    eye->z = posZ;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, eye, desiredYRot, vel, headOnly);
}
VERIFY(0x022F2E18, &daNpc_Uk_c::lookBack);

/* 022F3138 */
BOOL daNpc_Uk_c::wait01() {
    WWHD_FUNC(0x022F3138, BOOL, this);
    if (chkFlag(0x1)) {
        s8 state = mState;
        mState = 1;
        mReturnToState = state;
        setAnmStatus();
    } else {
        SetOrder(1);
    }
    return isMorf();
}
VERIFY(0x022F3138, &daNpc_Uk_c::wait01);

/* 022F31CC */
BOOL daNpc_Uk_c::talk01() {
    WWHD_FUNC(0x022F31CC, BOOL, this);
    if (talk() == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mState = mReturnToState;
        setAnmStatus();
        dComIfGp_event_reset();
        mTalkWait = 5;
        clrFlag(0x1);
    }
    setFlag(0x20);
    return isMorf();
}
VERIFY(0x022F31CC, &daNpc_Uk_c::talk01);

/* 022F325C */
BOOL daNpc_Uk_c::talk02() {
    WWHD_FUNC(0x022F325C, BOOL, this);
    if (talk() == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        if (!Mk_chkGameSet(&mMk)) {
            mState = mReturnToState;
            setAnmStatus();
            dComIfGp_event_reset();
            mTalkWait = 5;
            clrFlag(0x1);
        } else {
            u32 idx = evmng_getEventIdx_raw(STR(0x10023248) /* "MK_GAMESET" */, 0xFF);
            mEventIdx = (s16)idx;
            fopAcM_orderChangeEventId_raw(this, idx, 0, 0xFFFF);
            mDoAud_seStart_1(0x8F1);
            mState = 4;
            mReturnToState = 5;
            mTalkWait = 5;
            clrFlag(0x1);
        }
    }
    setFlag(0x20);
    return isMorf();
}
VERIFY(0x022F325C, &daNpc_Uk_c::talk02);

/* 022F335C */
u32 daNpc_Uk_c::getStaffName() {
    WWHD_FUNC(0x022F335C, u32, this);
    if (mType == 2) {
        u32 shape = getShapeType();
        if (shape <= 2) {
            return gabi::load<u32>(0x10023260 + shape * 4); /* "UkB2", "UkC2", "UkD2" */
        }
        return 0x1002325C; /* "UkB" */
    }
    switch (getShapeType()) {
    case 1:
        return 0x10023254; /* "UkC" */
    case 2:
        return 0x10023258; /* "UkD" */
    default:
        return 0x1002325C; /* "UkB" */
    }
}
VERIFY(0x022F335C, &daNpc_Uk_c::getStaffName);

/* 022F340C */
s32 daNpc_Uk_c::getNowEventAction() {
    WWHD_FUNC(0x022F340C, s32, this);
    /* static char* action_table[15] = {"WAIT", "TALK", "TALK2", "HOME", "RUN", "RUN3", "HIND", "DISP",
     * "SPEAK", "LOOK_P", "WARNING", "JUMP", "JUMP2", "TURN", "WARP"} (.data 0x101C6960) */
    return dComIfGp_evmng_getMyActIdx(mStaffIdx, 0x101C6960, 15, FALSE, 0);
}
VERIFY(0x022F340C, &daNpc_Uk_c::getNowEventAction);

/* 022F3458 */
BOOL daNpc_Uk_c::checkDemoStart() {
    WWHD_FUNC(0x022F3458, BOOL, this);
    u32 name = getStaffName();
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(STR(name), nullptr, 0);
    mStaffIdx = staffIdx;
    if (staffIdx != -1) {
        mEventAction = getNowEventAction();
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022F3458, &daNpc_Uk_c::checkDemoStart);

/* 022F34E0 */
void daNpc_Uk_c::demoInitCom() {
    WWHD_FUNC(0x022F34E0, void, this);
    be<s32>* sound = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x100232F8) /* "SOUND" */);
    if (sound == nullptr) {
        return;
    }
    u32 se;
    switch ((u32)(s32)*sound) {
    case 1:
        switch (getShapeType()) {
        case 1:
            se = 0x490C;
            break;
        case 2:
            se = 0x4910;
            break;
        default:
            se = 0x4908;
            break;
        }
        break;
    case 2:
        switch (getShapeType()) {
        case 1:
            se = 0x490D;
            break;
        case 2:
            se = 0x4911;
            break;
        default:
            se = 0x4909;
            break;
        }
        break;
    default:
        return;
    }
    mDoAud_seStart(se, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
}
VERIFY(0x022F34E0, &daNpc_Uk_c::demoInitCom);

/* 022F40EC */
BOOL daNpc_Uk_c::demo02() {
    WWHD_FUNC(0x022F40EC, BOOL, this);
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        mState = mReturnToState;
        setAnmStatus();
        dComIfGp_event_reset();
        mTalkWait = 5;
        clrFlag(0x1);
        return TRUE;
    }
    if (!checkDemoStart()) /* JUT_ASSERT(0x796, 0) */
        JUT_ASSERT_fail(STR(0x10023354), 0x796, STR(0x10023350));
    return demoProc();
}
VERIFY(0x022F40EC, &daNpc_Uk_c::demo02);

/* 022F41B4 */
u32 daNpc_Uk_c::visitTalkInit() {
    WWHD_FUNC(0x022F41B4, u32, this);
    if (dComIfGs_isEventBit(0x1F80) && !dComIfGs_isEventBit(0x1E02)) {
        mEventIdx = gabi::load<s16>(gabi::ea(this) + 0xFC); /* eventInfo.mEventId */
        dComIfGp_event_setTalkPartner(fopAcM_SearchByID(mPartnerId));
        mState = 4;
        demo02();
        dComIfGs_onEventBit(0x1E04);
        return 4;
    }
    if (dComIfGs_isEventBit(0x1208) && !dComIfGs_checkGetItem(0x23) && !dComIfGs_checkGetItem(0x26)) {
        mEventIdx = gabi::load<s16>(gabi::ea(this) + 0xFC);
        dComIfGp_event_setTalkPartner(fopAcM_SearchByID(mPartnerId));
        mState = 4;
        demo02();
        dComIfGs_onEventBit(0x1602);
        return 4;
    }
    return 1;
}
VERIFY(0x022F41B4, &daNpc_Uk_c::visitTalkInit);

/* 022F437C */
BOOL daNpc_Uk_c::demo01() {
    WWHD_FUNC(0x022F437C, BOOL, this);
    if (!checkDemoStart()) {
        mState = mReturnToState;
        setAnmStatus();
        return TRUE;
    }
    return demoProc();
}
VERIFY(0x022F437C, &daNpc_Uk_c::demo01);

/* 022F3664 */
BOOL daNpc_Uk_c::demoProc() {
    WWHD_FUNC(0x022F3664, BOOL, this);
    BOOL ret = FALSE;
    if (dComIfGp_evmng_getIsAddvance(mStaffIdx)) {
        demoInitCom();
        switch ((u32)mEventAction) {
        case 0: /* WAIT */
        case 1: /* TALK */
        case 9: /* LOOK_P */
            speedF = 0.0f;
            setAnm(0, 0);
            break;
        case 2: /* TALK2 */
            speedF = 0.0f;
            setAnm(2, 0);
            break;
        case 3: /* HOME */
            current.angle.y = home.angle.y;
            old.pos.copy(home.pos);
            current.pos.copy(home.pos);
            speedF = 0.0f;
            break;
        case 4: /* RUN */
        case 5: { /* RUN3 */
            if (mCurrAnmIdx == 4) {
                speedF = m80C;
            } else {
                setAnm(4, 0);
            }
            u32 a_xyz = gabi::ea(dComIfGp_evmng_getMySubstanceP(mStaffIdx, STR(0x10023314) /* "Pos" */, 1));
            if (a_xyz == 0) /* JUT_ASSERT(0x7F4, a_xyz) */
                JUT_ASSERT_fail(STR(0x10023340), 0x7F4, STR(0x10023318));
            m840.copy(current.pos);
            wcopy3(gabi::ea(&m84C), a_xyz);
            u32 a_timer = gabi::ea(dComIfGp_evmng_getMySubstanceP(mStaffIdx, STR(0x10023320) /* "Timer" */, 3));
            if (a_timer != 0) {
                s16 timer = gabi::load<s16>(a_timer + 2); /* (s16)*a_timer */
                m858 = timer;
                if (getShapeType() == 1 && timer == 0x69) {
                    m858 = 0x9B;
                }
            } else {
                m858 = -1;
            }
            break;
        }
        case 13: { /* TURN */
            u32 a_xyz = gabi::ea(dComIfGp_evmng_getMySubstanceP(mStaffIdx, STR(0x10023314) /* "Pos" */, 1));
            if (a_xyz == 0) /* JUT_ASSERT(0x808, a_xyz) */
                JUT_ASSERT_fail(STR(0x10023340), 0x808, STR(0x10023318));
            m840.copy(current.pos);
            wcopy3(gabi::ea(&m84C), a_xyz);
            m85C = cLib_targetAngleY(&m840, &m84C);
            setAnm(0, 0);
            break;
        }
        case 6: /* HIND */
            setFlag(0x10);
            actor_status &= ~0x20u;
            speedF = 0.0f;
            mReturnToState = 5;
            break;
        case 7: /* DISP: mCurrActionFunc = &visit_action, started without ending the current action */
            mCurrActionFunc.d = 0;
            mCurrActionFunc.i = -1;
            mCurrActionFunc.f = 0x022F6058;
            mActionStatus = 0;
            gabi::call_ptr<BOOL>(0x022F6058 /* visit_action */, this, (void*)nullptr);
            actor_status |= 0x20u;
            mReturnToState = mState;
            mState = 3;
            clrFlag(0x10);
            break;
        case 8: { /* SPEAK */
            speedF = 0.0f;
            talkInit();
            setFlag(0x80);
            u32 a_intP = gabi::ea(dComIfGp_evmng_getMySubstanceP(mStaffIdx, STR(0x10023328) /* "MsgNo" */, 3));
            if (a_intP == 0) /* JUT_ASSERT(0x823, a_intP) */
                JUT_ASSERT_fail(STR(0x10023340), 0x823, STR(0x10023338));
            mMsgNo = gabi::load<u32>(a_intP);
            switch (getShapeType()) {
            case 0:
                setAnm(0, 0);
                break;
            case 1:
                setAnm(1, 0);
                break;
            default:
                setAnm(2, 0);
                break;
            }
            ret = TRUE;
            break;
        }
        case 10: /* WARNING */
            speedF = 0.0f;
            setFlag(0x2);
            setAnm(5, 1);
            break;
        case 11: /* JUMP */
        case 12: { /* JUMP2 */
            speed.y = 28.0f;
            setFlag(0x202);
            speedF = 12.0f;
            setAnm(9, 0);
            u32 se;
            switch (getShapeType()) {
            case 1:
                se = 0x490C;
                break;
            case 2:
                se = 0x4910;
                break;
            default:
                se = 0x4908;
                break;
            }
            mDoAud_seStart(se, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
            break;
        }
        case 14: { /* WARP */
            speedF = 0.0f;
            u32 a_xyz = gabi::ea(dComIfGp_evmng_getMySubstanceP(mStaffIdx, STR(0x10023314) /* "Pos" */, 1));
            if (a_xyz == 0) /* JUT_ASSERT(0x84E, a_xyz) */
                JUT_ASSERT_fail(STR(0x10023340), 0x84E, STR(0x10023318));
            wcopy3(gabi::ea(&current.pos), a_xyz);
            wcopy3(gabi::ea(&old.pos), a_xyz);
            u32 a_intP = gabi::ea(dComIfGp_evmng_getMySubstanceP(mStaffIdx, STR(0x10023330) /* "Angle" */, 3));
            if (a_intP == 0) /* JUT_ASSERT(0x854, a_intP) */
                JUT_ASSERT_fail(STR(0x10023340), 0x854, STR(0x10023338));
            current.angle.y = (s16)gabi::load<u32>(a_intP);
            shape_angle.y = (s16)gabi::load<u32>(a_intP);
            ret = TRUE;
            break;
        }
        }
    }
    switch ((u32)mEventAction) {
    case 1:
    case 2:
        setFlag(0x20);
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    case 4:
    case 5: { /* RUN, RUN3 */
        gabi::Local<be<s16>> angle;  /* sp+0x18 */
        gabi::Local<cXyz> pos;       /* sp+0x1C */
        gabi::Local<cXyz> target;    /* sp+0x28 */
        gabi::Local<be<f32>> dist;   /* sp+0x40 */
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        target->x = m84C.x;
        target->y = m84C.y;
        target->z = m84C.z;
        dNpc_calc_DisXZ_AngY(pos, target, dist, angle);
        if (*dist < m80C || mMk_chkPointPass(&mMk, &m840, &m84C, &current.pos)) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
            if (mEventAction == 5) {
                current.pos.x = m84C.x;
                speedF = 0.0f;
                current.pos.z = m84C.z;
            }
            return TRUE;
        }
        cLib_addCalcAngleS(&current.angle.y, *angle, 4, 0x2000, 0x400);
        cLib_chaseF(&speedF, m80C, 1.1f);
        s16 timer = m858;
        if (timer < 0) {
            return TRUE;
        }
        if (timer > 0) {
            timer = (s16)(timer - 1);
            m858 = timer;
            if (timer != 0) {
                return TRUE;
            }
        }
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    }
    case 8: { /* SPEAK */
        u32 st = talk();
        if (st == 0x12 || st == 0xFE) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        setFlag(0x20);
        if (ret) {
            return TRUE;
        }
        return isMorf();
    }
    case 9: /* LOOK_P */
        setFlag(0x40);
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    case 10: /* WARNING */
        if (chkFlag(0x2)) {
            if (mAnmEnded != 0) {
                dComIfGp_evmng_cutEnd(mStaffIdx);
                clrFlag(0x2);
            }
            return TRUE;
        }
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    case 11: /* JUMP */
    case 12: /* JUMP2 */
        if (chkFlag(0x200)) {
            return TRUE;
        }
        setAnm(10, 0);
        speedF = 0.0f;
        if (chkFlag(0x2)) {
            clrFlag(0x2);
            gabi::Local<cXyz> shockPos;
            shockPos->x = 0.0f;
            shockPos->y = 1.0f;
            shockPos->z = 0.0f;
            dComIfGp_getVibration_StartShock(4, -0x21, shockPos);
            if (mEventAction == 12) {
                /* static cXyz scale(0.6f, 0.6f, 0.6f) (function-local, initialised on first use) */
                if (gabi::load<u32>(0x1046894C) == 0) {
                    gabi::store<u32>(0x1046894C, 1);
                    gabi::store<f32>(0x10468940, 0.6f);
                    gabi::store<f32>(0x10468948, 0.6f);
                    gabi::store<f32>(0x10468944, 0.6f);
                }
                JPABaseEmitter* emitter = dComIfGp_particle_set(0x23, &current.pos, &current.angle, gabi::at<cXyz>(0x10468940));
                if (emitter != nullptr) {
                    gabi::store<f32>(gabi::ea(emitter) + 0x58, 1.0f); /* emitter rate / scale parameters */
                    gabi::store<u32>(gabi::ea(emitter) + 0x5C, 1);
                    gabi::store<f32>(gabi::ea(emitter) + 0x34, 18.0f);
                    return TRUE;
                }
            }
            return TRUE;
        }
        if (mAnmEnded != 0) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        return TRUE;
    case 13: /* TURN */
        cLib_addCalcAngleS(&current.angle.y, m85C, 1, 0x800, 0x800);
        if (current.angle.y == m85C) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
        return TRUE;
    default: /* WAIT, HOME, HIND, DISP, WARP */
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    }
}
VERIFY(0x022F3664, &daNpc_Uk_c::demoProc);

/* square of the XZ length of {x, 0, z} (a stack vector passed to PSVECSquareMag) */
static f32 uk_sqMagXZ(f32 x, f32 z) {
    gabi::Local<cXyz> v;
    v->x = x;
    v->y = 0.0f;
    v->z = z;
    return PSVECSquareMag(v);
}
static inline u8 mk_mode(fopAc_ac_c* mk) { return gabi::load<u8>(gabi::ea(mk) + 0x818); } /* daNpc_Mk_c mode */

/* 022F43DC */
u8 daNpc_Uk_c::nextVisitMode() {
    WWHD_FUNC(0x022F43DC, u8, this);
    fopAc_ac_c* mk = fopAcM_SearchByID(mPartnerId);
    fopAc_ac_c* player = daPy_getPlayerLinkActorClass();
    u8 mkMode;
    bool search = false;
    if (mk == nullptr) {
        mVisitMode = 0;
        search = true;
    } else {
        u8 mode = mVisitMode;
        mkMode = mk_mode(mk);
        if (mode == 10) {
            if (mVisitTimer != 0) {
                mVisitTimer = mVisitTimer - 1;
                return 10;
            }
        } else if (mode == 0) {
            search = true;
        }
    }
    if (search) {
        mk = fopAcM_searchFromName(STR(0x10023378) /* "Mk" */, 0xFF, 1);
        if (mk == nullptr) {
            return 0;
        }
        mPartnerId = fopAcM_GetID(mk);
        mkMode = mk_mode(mk);
    }
    if (mkMode == 2 || mkMode == 9 || mkMode == 8) {
        gabi::Local<cXyz> d;
        cXyz_mi(&mk->current.pos, d, &current.pos);
        f32 dx = d->x;
        u8 m = mVisitMode;
        f32 dz = d->z;
        if (m == 3) {
            if (uk_sqMagXZ(dx, dz) > 40000.0f) {
                return 2;
            }
            if (!(uk_sqMagXZ(dx, dz) < 6400.0f)) { /* bge: taken on NaN */
                return 3;
            }
            return 4;
        }
        if (m == 4) {
            if (uk_sqMagXZ(dx, dz) > 10000.0f) {
                return 3;
            }
            return 4;
        }
        if (!(uk_sqMagXZ(dx, dz) < 22500.0f)) { /* bge: taken on NaN */
            return 2;
        }
        return 3;
    }
    u8 m = mVisitMode;
    if (m == 9) {
        if (mVisitTimer != 0) {
            mVisitTimer = mVisitTimer - 1;
            return 9;
        }
        gabi::Local<cXyz> d;
        cXyz_mi(&player->current.pos, d, &current.pos);
        if (uk_sqMagXZ(d->x, d->z) < 22500.0f) {
            return 7;
        }
        if (mkMode == 4 || mkMode == 5) {
            return 5;
        }
        s16 diff = (s16)(fopAcM_searchActorAngleY(this, mk) - shape_angle.y);
        if (diff < 0) {
            diff = (s16)-diff;
        }
        if (diff < 0x3800) {
            return 2;
        }
        return 9;
    }
    if (mkMode == 6) {
        if (m == 8) {
            if (mVisitTimer != 0) {
                mVisitTimer = mVisitTimer - 1;
                return 8;
            }
            return 9;
        }
        return 8;
    }
    if (mkMode == 7) {
        return 10;
    }
    gabi::Local<cXyz> d;
    cXyz_mi(&player->current.pos, d, &current.pos);
    u8 m2 = mVisitMode;
    f32 dx = d->x;
    f32 dz = d->z;
    if (m2 < 5 || m2 > 7) {
        if (!(uk_sqMagXZ(dx, dz) < 22500.0f)) { /* bge: taken on NaN */
            return 2;
        }
        return 7;
    }
    if (m2 == 5) {
        if (uk_sqMagXZ(dx, dz) < 22500.0f) {
            return 7;
        }
        return 5;
    }
    if (uk_sqMagXZ(dx, dz) > 32400.0f) {
        return 5;
    }
    u8 m3 = mVisitMode;
    if (m3 == 7) {
        u8 t = mVisitTimer2;
        if (t == 0) {
            return 6;
        }
        m3 = mVisitMode;
        mVisitTimer2 = (u8)(t - 1);
    }
    return m3;
}
VERIFY(0x022F43DC, &daNpc_Uk_c::nextVisitMode);

/* 022F4B44 */
void daNpc_Uk_c::visitInit(u32 mode) {
    WWHD_FUNC(0x022F4B44, void, this, mode);
    /* u8 argument, compared unnormalised */
    switch (mode) {
    case 2:
        setAnm(4, 0);
        mVisitMode = (u8)mode;
        break;
    case 3:
    case 6:
        setAnm(3, 0);
        mVisitMode = (u8)mode;
        break;
    case 4:
        setAnm(0, 0);
        speedF = 0.0f;
        mVisitMode = (u8)mode;
        break;
    case 5:
        setAnm(4, 0);
        if (mVisitMode == 9) {
            m832 = 0x2D;
        } else {
            m832 = 0;
        }
        mVisitMode = (u8)mode;
        break;
    case 7: {
        u8 m = mVisitMode;
        if (m == 9 || m == 8 || m == 2) {
            mVisitTimer2 = 0x2D;
        } else {
            mVisitTimer2 = m832;
        }
        setAnm(4, 0);
        mVisitMode = (u8)mode;
        break;
    }
    case 8:
        setAnm(0, 0);
        mVisitMode = (u8)mode;
        speedF = 0.0f;
        mVisitTimer = 5;
        break;
    case 9:
        setAnm(0, 0);
        mVisitMode = (u8)mode;
        speedF = 0.0f;
        mVisitTimer = 15;
        break;
    case 10:
        setAnm(0, 0);
        mVisitMode = (u8)mode;
        speedF = 0.0f;
        mVisitTimer = 30;
        break;
    default:
        speedF = 0.0f;
        mVisitMode = (u8)mode;
        break;
    }
}
VERIFY(0x022F4B44, &daNpc_Uk_c::visitInit);

/* 022F4D0C */
void daNpc_Uk_c::approachRun(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x022F4D0C, void, this, i_actor);
    gabi::Local<be<s16>> angle; /* sp+0x08 */
    gabi::Local<cXyz> pos;      /* sp+0x0C */
    gabi::Local<cXyz> target;   /* sp+0x18 */
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    target->y = i_actor->current.pos.y;
    target->z = i_actor->current.pos.z;
    target->x = i_actor->current.pos.x;
    dNpc_calc_DisXZ_AngY(pos, target, nullptr, angle);
    cLib_addCalcAngleS2(&current.angle.y, *angle, 8, 0x800);
}
VERIFY(0x022F4D0C, &daNpc_Uk_c::approachRun);

/* 022F4D8C (matcher: unnamed) */
void daNpc_Uk_c::aroundWalk(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x022F4D8C, void, this, i_actor);
    Mk_aroundWalk(&mMk, this, i_actor, mVisitTimer2);
}
VERIFY(0x022F4D8C, &daNpc_Uk_c::aroundWalk);

/* 022F4DA0 */
void daNpc_Uk_c::surrender() {
    WWHD_FUNC(0x022F4DA0, void, this);
    fopAc_ac_c* mk = fopAcM_SearchByID(mPartnerId);
    if (mk == nullptr || !(mk->speedF > 1.0f)) {
        return;
    }
    gabi::Local<cXyz> v;  /* sp+0x08 */
    gabi::Local<cXyz> d;  /* sp+0x18 */
    cXyz_mi(&mk->current.pos, d, &current.pos);
    v->x = d->x;
    v->y = 0.0f;
    v->z = d->z;
    if (PSVECSquareMag(v) > 7225.0f) {
        return;
    }
    s16 diff = (s16)(fopAcM_searchActorAngleY(mk, this) - mk->shape_angle.y);
    s16 mkAngle = mk->shape_angle.y;
    if ((u32)((s32)diff + 0x2AAA) >= 0x5555u) {
        return;
    }
    s16 dir;
    if (diff >= 0) {
        dir = (s16)(mkAngle + 0x4000);
    } else {
        dir = (s16)(mkAngle - 0x4000);
    }
    speed.x = gabi::fmadds(cM_ssin(dir), 0.5f, speed.x);
    setFlag(0x8);
    speed.z = gabi::fmadds(cM_scos(dir), 0.5f, speed.z);
}
VERIFY(0x022F4DA0, &daNpc_Uk_c::surrender);

/* 022F4F28 */
void daNpc_Uk_c::visitProc() {
    WWHD_FUNC(0x022F4F28, void, this);
    fopAc_ac_c* mk = fopAcM_SearchByID(mPartnerId);
    fopAc_ac_c* player = daPy_getPlayerLinkActorClass();
    switch (mVisitMode) {
    case 2:
        if (mk == nullptr) /* JUT_ASSERT(0x5A0, mk != NULL) */
            JUT_ASSERT_fail(STR(0x1002338C), 0x5A0, STR(0x10023380));
        else
            approachRun(mk);
        break;
    case 3:
        if (mk == nullptr) /* JUT_ASSERT(0x5AB, mk != NULL) */
            JUT_ASSERT_fail(STR(0x1002338C), 0x5AB, STR(0x10023380));
        else
            approachRun(mk);
        break;
    case 5:
        if (m832 < 0x2D) {
            m832 = m832 + 1;
        }
        approachRun(player);
        break;
    case 6:
    case 7:
        aroundWalk(player);
        break;
    case 4:
    case 8:
    case 9:
    case 10:
        surrender();
        break;
    }
    u8 m = mVisitMode;
    if (m >= 9 && m <= 10) {
        setFlag(0x20);
    }
}
VERIFY(0x022F4F28, &daNpc_Uk_c::visitProc);

/* 022F5088 */
void daNpc_Uk_c::visitSetEvent() {
    WWHD_FUNC(0x022F5088, void, this);
    if (dComIfGs_isEventBit(0x1F80) && !dComIfGs_isEventBit(0x1E02)) {
        dEvt_info_setEventName(this, STR(0x100233A4) /* "MK_TALK3" */);
    } else if (dComIfGs_isEventBit(0x1208) && !dComIfGs_checkGetItem(0x23) && !dComIfGs_checkGetItem(0x26)) {
        dEvt_info_setEventName(this, STR(0x100233B0) /* "MK_TALK2" */);
    } else {
        dEvt_info_setEventName(this, STR(0x1002339C) /* "MK_TALK" */);
    }
}
VERIFY(0x022F5088, &daNpc_Uk_c::visitSetEvent);

/* 022F515C */
BOOL daNpc_Uk_c::visit01() {
    WWHD_FUNC(0x022F515C, BOOL, this);
    if (chkFlag(0x1)) {
        mReturnToState = mState;
        mState = (s8)visitTalkInit();
        setAnmStatus();
        speedF = 0.0f;
        mVisitMode = 1;
        return TRUE;
    }
    if (checkDemoStart()) {
        s8 state = mState;
        mState = 3;
        mReturnToState = state;
        setAnmStatus();
        speedF = 0.0f;
        mVisitMode = 1;
        demo01();
        return TRUE;
    }
    u32 next = nextVisitMode();
    if (next != mVisitMode) {
        visitInit(next);
    }
    visitProc();
    f32 target = 0.0f;
    if (mCurrAnmIdx == 3) {
        if (mVisitMode == 6) {
            target = 2.5f;
        } else {
            target = m808;
        }
    } else if (mCurrAnmIdx == 4) {
        target = m80C;
        if (mVisitMode == 7) {
            target = gabi::fsubs_ppc(target, 2.5f);
        }
    }
    f32 step = (gabi::fsubs_ppc(target, speedF) >= 0.0f) ? 1.1f : 2.8f;
    cLib_chaseF(&speedF, target, step);
    SetOrder(1);
    u8 m = mVisitMode;
    if (m == 6 || m == 7) {
        fopAc_ac_c* player = daPy_getPlayerLinkActorClass();
        s16 diff = (s16)(fopAcM_searchActorAngleY(player, this) - player->shape_angle.y);
        if (diff < 0) {
            diff = (s16)-diff;
        }
        if (diff > 0x1800) {
            mOrderFlags = (u8)(mOrderFlags & ~1);
        }
    }
    fopAc_ac_c* mk = fopAcM_SearchByID(mPartnerId);
    if (mk != nullptr && (mk_mode(mk) == 8 || mk_mode(mk) == 9)) {
        mOrderFlags = (u8)(mOrderFlags & ~1);
    }
    if (ChkOrder(1)) {
        visitSetEvent();
    }
    return TRUE;
}
VERIFY(0x022F515C, &daNpc_Uk_c::visit01);

/* 022F53E0 */
BOOL daNpc_Uk_c::chkPositioning(f32 maxDist, f32 minY, f32 maxY, s16 angMin, s16 angMax) {
    WWHD_FUNC(0x022F53E0, BOOL, this, maxDist, minY, maxY, angMin, angMax);
    fopAc_ac_c* player = daPy_getPlayerLinkActorClass();
    gabi::Local<cXyz> v;  /* sp+0x08 */
    gabi::Local<cXyz> d;  /* sp+0x14 */
    cXyz_mi(&player->current.pos, d, &current.pos);
    v->x = d->x;
    v->y = 0.0f;
    v->z = d->z;
    f32 dist = std_sqrtf(PSVECSquareMag(v));
    if (dist > maxDist) {
        return FALSE;
    }
    f32 dy = d->y;
    if (dy < minY || dy > maxY) {
        return FALSE;
    }
    s16 ang = (s16)(cLib_targetAngleY(&current.pos, &player->current.pos) - current.angle.y);
    if (angMin > angMax) {
        if (ang < angMin && ang > angMax) {
            return TRUE;
        }
        return FALSE;
    }
    if (ang < angMin || ang > angMax) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022F53E0, &daNpc_Uk_c::chkPositioning);

/* 022F554C */
BOOL daNpc_Uk_c::warningB() {
    WWHD_FUNC(0x022F554C, BOOL, this);
    if (chkPositioning(300.0f, -10.0f, 80.0f, 0x3A98, -0x3A98)) {
        mState = 9;
        u32 idx = evmng_getEventIdx_raw(STR(0x100233C4) /* "UkB_FOUND" */, 0xFF);
        mEventIdx = (s16)idx;
        fopAcM_orderOtherEventId_raw(this, idx, 0xFF, 0xFFFF, 0, 1);
    }
    return FALSE;
}
VERIFY(0x022F554C, &daNpc_Uk_c::warningB);

/* 022F55E8 */
BOOL daNpc_Uk_c::warningC() {
    WWHD_FUNC(0x022F55E8, BOOL, this);
    if (chkPositioning(180.0f, -10.0f, 80.0f, 0x3A98, -0x1388)) {
        mState = 9;
        u32 idx = evmng_getEventIdx_raw(STR(0x100233D4) /* "UkC_FOUND" */, 0xFF);
        mEventIdx = (s16)idx;
        fopAcM_orderOtherEventId_raw(this, idx, 0xFF, 0xFFFF, 0, 1);
    }
    return FALSE;
}
VERIFY(0x022F55E8, &daNpc_Uk_c::warningC);

/* 022F5684 */
BOOL daNpc_Uk_c::warningD() {
    WWHD_FUNC(0x022F5684, BOOL, this);
    if (chkPositioning(150.0f, -10.0f, 80.0f, 0x4650, 0)) {
        mState = 9;
        u32 idx = evmng_getEventIdx_raw(STR(0x100233E4) /* "UkD_FOUND" */, 0xFF);
        mEventIdx = (s16)idx;
        fopAcM_orderOtherEventId_raw(this, idx, 0xFF, 0xFFFF, 0, 1);
    }
    if (chkPositioning(150.0f, -10.0f, 80.0f, 0, -0x4650)) {
        mState = 9;
        u32 idx = evmng_getEventIdx_raw(STR(0x100233F0) /* "UkD_FOUND2" */, 0xFF);
        mEventIdx = (s16)idx;
        fopAcM_orderOtherEventId_raw(this, idx, 0xFF, 0xFFFF, 0, 1);
    }
    return FALSE;
}
VERIFY(0x022F5684, &daNpc_Uk_c::warningD);

/* 022F57DC */
BOOL daNpc_Uk_c::found() {
    WWHD_FUNC(0x022F57DC, BOOL, this);
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        mReturnToState = 10;
        mState = 4;
        Mk_init(&mMk, 0x50, 0x12C);
        dComIfGs_onTmpBit(getFoundFlag());
        return demo02();
    }
    fopAcM_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
    return FALSE;
}
VERIFY(0x022F57DC, &daNpc_Uk_c::found);

/* 022F588C */
void daNpc_Uk_c::runawayInit() {
    WWHD_FUNC(0x022F588C, void, this);
    switch (mMk.state) {
    case 1:
    case 2:
    case 5:
        if (mCurrAnmIdx == 5) {
            u32 se;
            switch (getShapeType()) {
            case 1:
                se = 0x490D;
                break;
            case 2:
                se = 0x4911;
                break;
            default:
                se = 0x4909;
                break;
            }
            mDoAud_seStart(se, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        }
        speedF = 0.0f;
        setAnm(4, 0);
        break;
    case 3:
        speedF = 0.0f;
        setAnm(5, 0);
        break;
    case 4: {
        speedF = 8.0f;
        setFlag(0x200);
        speed.y = 25.0f;
        setAnm(8, 0);
        mState = 11;
        current.angle.y = m85C;
        u32 se;
        switch (getShapeType()) {
        case 1:
            se = 0x490F;
            break;
        case 2:
            se = 0x4913;
            break;
        default:
            se = 0x490B;
            break;
        }
        mDoAud_seStart(se, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        break;
    }
    }
}
VERIFY(0x022F588C, &daNpc_Uk_c::runawayInit);

/* 022F5AE8 */
BOOL daNpc_Uk_c::runaway() {
    WWHD_FUNC(0x022F5AE8, BOOL, this);
    u32 st = Mk_runAwayProc(&mMk, this, &mPath, &mCyl, &m85C);
    u32 old = mMk.state;
    if (st != old) {
        if ((st == 1 && old == 2) || (st == 2 && old == 1)) {
            if (cM_rndF(1.0f) < 0.5f) {
                u32 se;
                switch (getShapeType()) {
                case 1:
                    se = 0x490E;
                    break;
                case 2:
                    se = 0x4912;
                    break;
                default:
                    se = 0x490A;
                    break;
                }
                mDoAud_seStart(se, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
            }
        }
        mMk.state = (u8)st;
        runawayInit();
    }
    s8 anm = mCurrAnmIdx;
    if (st == 3) {
        setFlag(0x20);
    }
    if (anm == 4) {
        cLib_chaseF(&speedF, Mk_getSpeedF(&mMk, 15.0f, 18.0f), 2.8f);
    }
    return FALSE;
}
VERIFY(0x022F5AE8, &daNpc_Uk_c::runaway);

/* 022F5C5C */
BOOL daNpc_Uk_c::jump() {
    WWHD_FUNC(0x022F5C5C, BOOL, this);
    if (!chkFlag(0x200)) {
        setAnm(0, 0);
        speedF = 0.0f;
        if (isMorf()) {
            mState = 12;
            setAnm(6, 0);
            SetOrder(2);
        }
    }
    return TRUE;
}
VERIFY(0x022F5C5C, &daNpc_Uk_c::jump);

/* 022F5CEC */
BOOL daNpc_Uk_c::jitanda01() {
    WWHD_FUNC(0x022F5CEC, BOOL, this);
    if (chkFlag(0x1)) {
        setFlag(0x400);
        mReturnToState = 13;
        mState = 1;
        return TRUE;
    }
    SetOrder(2);
    return TRUE;
}
VERIFY(0x022F5CEC, &daNpc_Uk_c::jitanda01);

/* 022F5D2C */
BOOL daNpc_Uk_c::jitanda02() {
    WWHD_FUNC(0x022F5D2C, BOOL, this);
    if (chkFlag(0x1)) {
        s8 state = mState;
        mState = 1;
        mReturnToState = state;
    }
    if (checkDemoStart()) {
        s8 state = mState;
        mState = 3;
        mReturnToState = state;
        demo01();
        return TRUE;
    }
    SetOrder(1);
    return TRUE;
}
VERIFY(0x022F5D2C, &daNpc_Uk_c::jitanda02);

/* 022F5DB0 */
BOOL daNpc_Uk_c::wait_action(void*) {
    WWHD_FUNC(0x022F5DB0, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        mState = 0;
        setAnmStatus();
        mActionStatus = mActionStatus + 1; /* ACTION_ONGOING */
        return TRUE;
    }
    if (mActionStatus != ACTION_ENDING) {
        chkAttention();
        clrFlag(0x60);
        u32 temp;
        switch ((s32)mState) {
        case 0:
            temp = wait01();
            break;
        case 1:
            temp = talk01();
            break;
        default:
            temp = 0;
            break;
        }
        lookBack();
        setAttention(temp);
    }
    return TRUE;
}
VERIFY(0x022F5DB0, &daNpc_Uk_c::wait_action);

/* 022F5E90 */
BOOL daNpc_Uk_c::hind_action(void*) {
    WWHD_FUNC(0x022F5E90, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        mActionStatus = ACTION_ONGOING;
        return TRUE;
    }
    if (mActionStatus != ACTION_ENDING) {
        clrFlag(0x60);
        s8 type = mType;
        if (type == 2) {
            if (chkGameStart()) {
                uk_setAction(this, 0x022F6360); /* seek_action */
                clrFlag(0x10);
                actor_status |= 0x20u;
            }
            type = mType;
        }
        if ((type == 1 || type == 2) && checkDemoStart()) {
            u32 temp = demoProc();
            lookBack();
            setAttention(temp);
        }
    }
    return TRUE;
}
VERIFY(0x022F5E90, &daNpc_Uk_c::hind_action);

/* 022F6058 */
BOOL daNpc_Uk_c::visit_action(void*) {
    WWHD_FUNC(0x022F6058, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        if (dComIfGs_isTmpBit(0x40)) {
            mState = 13;
            setAnm(6, 0);
            mVisitMode = 0;
            mStts.SetWeight(0xFE);
        } else {
            mState = 2;
            setAnmStatus();
            mVisitMode = 0;
            mStts.SetWeight(0x64);
        }
        mActionStatus = mActionStatus + 1;
        return TRUE;
    }
    if (mActionStatus != ACTION_ENDING) {
        chkAttention();
        clrFlag(0x60);
        u32 temp;
        switch ((s32)mState) {
        case 1:
            temp = talk01();
            break;
        case 2:
            temp = visit01();
            break;
        case 3:
            temp = demo01();
            break;
        case 4:
            temp = demo02();
            break;
        case 5:
            temp = 0;
            uk_setAction(this, 0x022F5E90); /* hind_action */
            break;
        case 13:
            temp = jitanda02();
            break;
        default:
            temp = 0;
            break;
        }
        lookBack();
        setAttention(temp);
    }
    return TRUE;
}
VERIFY(0x022F6058, &daNpc_Uk_c::visit_action);

/* 022F6360 */
BOOL daNpc_Uk_c::seek_action(void*) {
    WWHD_FUNC(0x022F6360, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        if (dComIfGs_isTmpBit(getFoundFlag())) {
            mState = 10;
            Mk_init(&mMk, 0x50, 0x12C);
            Mk_setRndPathPos(&mMk, this, &mPath);
            setAnmStatus();
        } else {
            mState = (s8)(getShapeType() + 6);
            setAnmStatus();
        }
        mActionStatus = mActionStatus + 1;
        return TRUE;
    }
    if (mActionStatus != ACTION_ENDING) {
        chkAttention();
        clrFlag(0x60);
        u32 temp;
        switch ((s32)mState) {
        case 1:
            temp = talk02();
            break;
        case 3:
            temp = demo01();
            break;
        case 4:
            temp = demo02();
            break;
        case 5:
            temp = 0;
            uk_setAction(this, 0x022F5E90); /* hind_action */
            break;
        case 6:
            temp = warningB();
            break;
        case 7:
            temp = warningC();
            break;
        case 8:
            temp = warningD();
            break;
        case 9:
            temp = found();
            break;
        case 10:
            temp = runaway();
            break;
        case 11:
            temp = jump();
            break;
        case 12:
            temp = jitanda01();
            break;
        case 13:
            temp = jitanda02();
            break;
        default:
            temp = 0;
            break;
        }
        lookBack();
        setAttention(temp);
    }
    return TRUE;
}
VERIFY(0x022F6360, &daNpc_Uk_c::seek_action);

/* 022F6764: static initialisation of the translation unit (only the per-TU header statics) */
static void __sinit_d_a_npc_uk_cpp() {
    WWHD_FUNC(0x022F6764, void);
    sinit_header_statics(0x10468924, 0x101C69A0);
}
VERIFY(0x022F6764, __sinit_d_a_npc_uk_cpp);

/* 022F67F8: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x022F67F8, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x022F67F8, SafeString_dt);

/* 022F680C: daNpc_Uk_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Uk_c_dt(daNpc_Uk_c* i_this, s32 flags) {
    WWHD_FUNC(0x022F680C, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x10023030);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x10023040);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x025D50BC, i_this, 0);            /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022F680C, daNpc_Uk_c_dt);

/* 022F68A8: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x022F68A8, void, (SafeString*)nullptr);
}
VERIFY(0x022F68A8, SafeString_assureTerminationImpl);
