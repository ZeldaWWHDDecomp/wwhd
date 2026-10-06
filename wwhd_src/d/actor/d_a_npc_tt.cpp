/**
 * d_a_npc_tt.cpp (WWHD)
 * NPC - Tott (the dancer on Windfall Island)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_tt.cpp and d_a_npc_tt_anm.inc) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_tt.h"

#define SAFESTRING_VTBL 0x10022D5C /* this TU's sead::SafeString vtable */
#define TT_VTBL 0x10022DB4         /* daNpc_Tt_c vtable (HD virtual destructor) */

enum { fpcNm_NPC_TT_e = 0x16C };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
/* dComIfGp_getMesgAnimeAttrInfo(): play + 0x5BC5 */
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + 0x5BC5); }
/* dComIfGp_checkMesgSendButton(): play + 0x5BD2 */
static inline u8 dComIfGp_checkMesgSendButton() { return gabi::load<u8>(dComIfGp_ea() + 0x5BD2); }
/* dComIfGp_setMelodyNum(n): play + 0x5BDA */
static inline void dComIfGp_setMelodyNum(u8 n) { gabi::store<u8>(dComIfGp_ea() + 0x5BDA, n); }
/* dComIfGp_getSelectItem(btn): play + 0x5BBB + btn */
static inline u8 dComIfGp_getSelectItem(s32 btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
/* daPy_getPlayerLinkActorClass(): play + 0x5B34 */
static inline fopAc_ac_c* daPy_getPlayerLinkActorClass() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B34)); }
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
/* 02542EDC dEvent_manager_c::getMyActIdx(staffId, table, n, force, nameType) */
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* 025D7970 fopAcM_orderChangeEventId(actor, partner, s16 eventIdx, u16 flag, u16 hind) */
static inline BOOL fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D7970, a, b, ev, flag, hind);
}
/* HD message manager (*(0x101F4B5C)): 025F795C status, 025F74D0 setStatus, 025F7DB0
 * messageSet(msgNo, cXyz* pos) -> id; +0x921 the "send on" flag (fopMsgM_messageSendOn) */
static inline u32 msgMgr() { return gabi::load<u32>(0x101F4B5C); }
static inline u16 msgMgr_getStatus(u32 m) { return gabi::call<u16>(0x025F795C, m); }
static inline void msgMgr_setStatus(u32 m, u32 st) { gabi::call(0x025F74D0, m, st); }
static inline u32 msgMgr_messageSet(u32 m, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, m, msgNo, pos); }
/* 0259D454 dNpc_setAnm(morf, loopMode, morf, speed, anmIdx, soundIdx, arc) */
static inline BOOL dNpc_setAnm(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, s32 anmIdx, s32 soundIdx, const char* arc) {
    return gabi::call<BOOL>(0x0259D454, morf, loopMode, morfF, speed, anmIdx, soundIdx, arc);
}
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
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
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
static inline void mDoMtx_XrotS(Mtx34* m, s16 x) { gabi::call(0x025F18EC, m, x); }
/* mDoExt_3DlineMat0_c (HD) */
static inline void lineMat0_ct(mDoExt_3DlineMat0_tt* l) { gabi::call(0x025E9960, l); }
static inline void lineMat0_dt(mDoExt_3DlineMat0_tt* l, s32 flags) { gabi::call(0x025E99E0, l, flags); }
static inline BOOL lineMat0_init(mDoExt_3DlineMat0_tt* l, u16 numLines, u16 numSegs, BOOL hasSize) {
    return gabi::call<BOOL>(0x025E9B80, l, numLines, numSegs, hasSize);
}
static inline void lineMat0_update(mDoExt_3DlineMat0_tt* l, u16 segs, f32 size, u32 color, u16 space, dKy_tevstr_c* tev) {
    gabi::call(0x025EA548, l, segs, size, color, space, tev);
}
static inline cXyz* lineMat0_getPos(mDoExt_3DlineMat0_tt* l, s32 i) { return gabi::at<cXyz>(gabi::load<u32>(l->mpLines + i * 0x10)); }
static inline void dComIfGd_set3DlineMat(mDoExt_3DlineMat0_tt* l) {
    u32 pkt = dComIfGp_ea() + 0x5FB4;
    s32 id = gabi::call_ptr<s32>(gabi::load<u32>(l->__vtbl + 0x14), l); /* getMaterialID() */
    gabi::call(0x025EDD04, pkt + id * 0x9C, l);                     /* mDoExt_3DlineMatSortPacket::setMat */
}

/* GHS pointer to member function call */
static inline void pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
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

/* bit-exact word copies (GHS copies with lfs/stfs pairs that keep the bits, or lwz/stw) */
static inline void wcopy(u32 dst, u32 src, u32 n) {
    for (u32 i = 0; i < n; i += 4) gabi::store<u32>(dst + i, gabi::load<u32>(src + i));
}
/* 12-word matrix copy: all loads, then all stores */
static inline void mtx_wcopy(Mtx34* dst, const Mtx34* src) {
    u32 t[12];
    for (int i = 0; i < 12; i++) t[i] = gabi::load<u32>(gabi::ea(src) + 4 * i);
    /* lfs/stfs in the original: the recompiled pair goes through a double, which quiets an
     * SNaN (sets the quiet bit, keeps the payload); the NaN payload tolerance of float stores
     * does not cover unaligned words */
    for (int i = 0; i < 12; i++) {
        u32 v = t[i];
        if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x003FFFFFu) != 0 && !(v & 0x00400000u))
            v |= 0x00400000u;
        gmem_stf32(gabi::ea(dst) + 4 * i, v);
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

/* HD debug registers (g_regHIO, 0x1047B608): ke_control reads two float registers that the
 * GameCube build compiled out (0 in a normal run) */
static inline f32 tt_REG_F(u32 off) { return gabi::load<f32>(0x1047B608 + off); }

/* ---- file statics ---- */
/* l_msgId (HD: l_msg is gone, the message is reached through the message manager) */
static be<u32>& l_msgId() { return *gabi::at<be<u32>>(0x10468900); }
/* l_bck_ix_tbl[32] (s16, .rodata 0x10022DC4), l_bas_ix_tbl[32] (0x10022E04), l_btp_ix_tbl[1] (0x10022D58),
 * action_table[6] (.data 0x101C68C8), line colour (0x101C68C4) */

f32 daNpc_Tt_c::curMorf() { return gabi::load<f32>(gabi::ea(mpMorf.get()) + 0xB0); }

/* 022ED870 */
s16 daNpc_Tt_c::XyCheckCB(int i_itemBtn) {
    WWHD_FUNC(0x022ED870, s16, this, i_itemBtn);
    return dComIfGp_getSelectItem(i_itemBtn) == 0x22 /* dItemNo_WIND_WAKER_e */ ? TRUE : FALSE;
}
VERIFY(0x022ED870, &daNpc_Tt_c::XyCheckCB);

/* 022ED8B0 (tail call) */
static s16 daNpc_tt_XyCheckCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x022ED8B0, s16, i_this, i_itemBtn);
    return ((daNpc_Tt_c*)i_this)->XyCheckCB(i_itemBtn);
}
VERIFY(0x022ED8B0, daNpc_tt_XyCheckCB);

/* 022ED8B4 */
static s16 daNpc_tt_XyEventCB(void* i_this, int) {
    WWHD_FUNC(0x022ED8B4, s16, i_this, 0);
    daNpc_Tt_c* a_this = (daNpc_Tt_c*)i_this;
    s16 idx = dComIfGp_evmng_getEventIdx(STR(0x10022D48) /* "TACT_TT10" */, 0xFF);
    a_this->mEventIdx = idx;
    return idx;
}
VERIFY(0x022ED8B4, daNpc_tt_XyEventCB);

/* 022ED8F8 */
static BOOL nodeCallBack_Tt(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022ED8F8, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpc_Tt_c* i_this = gabi::at<daNpc_Tt_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        u32 jntNo = jntNo_of(node);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_head_jnt_num) {
                gabi::Local<cXyz> temp;
                gabi::Local<cXyz> temp2;
                temp->x = 0.0f;
                temp->y = 0.0f;
                temp->z = 0.0f;
                cMtx_YrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][1]);
                cMtx_ZrotM(calc_mtx(), (s16)-(i_this->m_jnt.mAngles[0][0] + i_this->mHeadAnm.field_0x00.x));
                MtxPosition(temp, temp2);
                i_this->mAttnBasePos.copy(*temp2); /* setAttentionBasePos(temp2) */
                temp->x = 20.0f;
                temp->y = -20.0f;
                temp->z = 0.0f;
                MtxPosition(temp, temp2);
                i_this->mEyePos.copy(*temp2); /* setEyePos(temp2) */
                if (i_this->mAttnSetCount != 0xFF) { /* incAttnSetCount() */
                    i_this->mAttnSetCount = i_this->mAttnSetCount + 1;
                }
            } else if (jntNo == (u32)(s32)i_this->m_backbone_jnt_num) {
                cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
                cMtx_ZrotM(calc_mtx(), i_this->m_jnt.mAngles[1][0]);
            }
            PSMTXCopy(calc_mtx(), j3dSys_mCurrentMtx());
            mtx_wcopy(getAnmMtx(model, jntNo), calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        }
    }
    return TRUE;
}
VERIFY(0x022ED8F8, nodeCallBack_Tt);

/* 022EDB94 */
BOOL daNpc_Tt_c::initTexPatternAnm(u32 i_modify) {
    WWHD_FUNC(0x022EDB94, BOOL, this, i_modify);
    J3DModelData* modelData = J3DModel_getModelData_l(mpMorf->getModel());
    /* HD: l_btp_ix_tbl[0] (not indexed with mTexPatternIdx) */
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectRes(STR(0x10022E54) /* "Tt" */, gabi::load<s32>(0x10022D58), SAFESTRING_VTBL);
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(130, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail(STR(0x10022E74), 0x82, STR(0x10022E58));
    if (!mDoExt_btpAnm_init(mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, i_modify, FALSE)) {
        return FALSE;
    }
    mBlinkFrame = 0;
    mBlinkTimer = 0;
    return TRUE;
}
VERIFY(0x022EDB94, &daNpc_Tt_c::initTexPatternAnm);

/* 022EDC98 */
BOOL daNpc_Tt_c::CreateHeap() {
    WWHD_FUNC(0x022EDC98, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10022E8C) /* "Tt" */, 0x1C /* dRes_INDEX_TT_BDL_TT_e */, SAFESTRING_VTBL);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10022E8C), 0x19 /* BCK_WAIT01 */, SAFESTRING_VTBL);
    void* bas = dComIfG_getObjectRes(STR(0x10022E8C), 0xE /* BAS_WAIT01 */, SAFESTRING_VTBL);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, bas, 0x80000, 0x11020022);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    s8 head = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10022E90) /* "head" */);
    m_head_jnt_num = head;
    if (head < 0) /* JUT_ASSERT(1718, m_head_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x10022ED0), 0x6B6, STR(0x10022EB0));
    s8 backbone = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10022EC4) /* "backbone1" */);
    m_backbone_jnt_num = backbone;
    if (backbone < 0) /* JUT_ASSERT(1720, m_backbone_jnt_num >= 0) */
        JUT_ASSERT_fail(STR(0x10022ED0), 0x6B8, STR(0x10022E98));
    switch ((s8)mType) {
    case 0:
        mTexPatternIdx = 0;
        break;
    }
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    for (u16 i = 0; i < J3DModelData_getJointNum(modelData); i++) {
        if (i == (u32)(s32)m_head_jnt_num || i == (u32)(s32)m_backbone_jnt_num) {
            /* mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack_Tt) */
            J3DModelData* md = J3DModel_getModelData_l(mpMorf->getModel());
            u32 n = gabi::load<u32>(gabi::ea(md) + 4);
            u32 joint = gabi::load<u32>(gabi::ea(md) + 8);
            if ((u32)i < n)
                joint += i * 0x1C;
            gabi::store<u32>(joint + 8, 0x022ED8F8);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 0.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    return lineMat0_init(&mLineMat, 8, 10, 0) == FALSE ? FALSE : TRUE;
}
VERIFY(0x022EDC98, &daNpc_Tt_c::CreateHeap);

/* 022EDF60 (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022EDF60, BOOL, i_this);
    return ((daNpc_Tt_c*)i_this)->CreateHeap();
}
VERIFY(0x022EDF60, CheckCreateHeap);

/* 022EDF64 */
void daNpc_Tt_c::set_ke_root(int param_1, int param_2, int param_3) {
    WWHD_FUNC(0x022EDF64, void, this, param_1, param_2, param_3);
    gabi::Local<cXyz> temp;
    gabi::Local<cXyz> a; /* GHS reuses three result temporaries */
    gabi::Local<cXyz> b;
    gabi::Local<cXyz> c;
    temp->x = 0.0f;
    temp->y = 0.0f;
    temp->z = 0.0f;
    PSMTXCopy(getAnmMtx(mpMorf->getModel(), param_1), calc_mtx());
    cXyz* k0 = &mLineKe[param_3].field_0x00[0];
    MtxPosition(temp, k0);
    PSMTXCopy(getAnmMtx(mpMorf->getModel(), param_2), calc_mtx());
    cXyz* k3 = &mLineKe[param_3 + 3].field_0x00[0];
    MtxPosition(temp, k3);
    cXyz_mi(k3, a, k0);
    cXyz_ml(a, b, 0.3333f);
    cXyz_pl(b, c, k0);
    mLineKe[param_3 + 1].field_0x00[0].copy(*c);
    cXyz_mi(k3, c, k0);
    cXyz_ml(c, b, 0.6667f);
    cXyz_pl(b, a, k0);
    mLineKe[param_3 + 2].field_0x00[0].copy(*a);
}
VERIFY(0x022EDF64, &daNpc_Tt_c::set_ke_root);

/* 022EE0D8 */
void tt_ke_s::ke_control(f32 param_1) {
    WWHD_FUNC(0x022EE0D8, void, this, param_1);
    gabi::Local<cXyz> temp;
    gabi::Local<cXyz> temp6;
    gabi::Local<cXyz> sum;
    f32 _065 = tt_REG_F(0x5A8) + 0.65f; /* HD: + a debug register */
    temp->x = 0.0f;
    temp->y = 0.0f;
    temp->z = 7.875f;
    cXyz* temp2 = &field_0x00[1];
    cXyz* temp3 = &field_0x78[1];
    for (int i = 1; i < 10; i++, temp2++, temp3++) {
        cXyz* prev = temp2 - 1;
        f32 x4 = gabi::fadds_ppc(gabi::fsubs_ppc(temp2->x, prev->x), temp3->x);
        f32 temp7 = gabi::fadds_ppc(gabi::fadds_ppc(gabi::fadds_ppc(temp2->y, temp3->y), -3.0f), tt_REG_F(0x5AC)); /* HD: + a debug register */
        f32 z4 = gabi::fadds_ppc(gabi::fsubs_ppc(temp2->z, prev->z), temp3->z);
        if (temp7 < param_1) {
            temp7 = param_1;
        }
        f32 y4 = gabi::fsubs_ppc(temp7, prev->y);
        Mtx34* m = calc_mtx();
        mDoMtx_XrotS(m, (s16)-cM_atan2s(y4, z4));
        m = calc_mtx();
        f32 len = std_sqrtf(gabi::fmadds(y4, y4, z4 * z4));
        mDoMtx_YrotM(m, cM_atan2s(x4, len));
        MtxPosition(temp, temp6);
        temp3->copy(*temp2);
        cXyz_pl(prev, sum, temp6);
        temp2->copy(*sum);
        temp3->x = gabi::fmuls_ppc(gabi::fsubs_ppc(temp2->x, temp3->x), _065);
        temp3->y = gabi::fmuls_ppc(gabi::fsubs_ppc(temp2->y, temp3->y), _065);
        temp3->z = gabi::fmuls_ppc(gabi::fsubs_ppc(temp2->z, temp3->z), _065);
    }
}
VERIFY(0x022EE0D8, &tt_ke_s::ke_control);

/* 022EE2FC */
void tt_ke_s::ke_pos_set(cXyz* i_param) {
    WWHD_FUNC(0x022EE2FC, void, this, i_param);
    for (int i = 0; i < 10; i_param++) {
        i_param->copy(field_0x00[i]);
        i++;
    }
}
VERIFY(0x022EE2FC, &tt_ke_s::ke_pos_set);

/* 022EE32C */
void daNpc_Tt_c::ke_execute() {
    WWHD_FUNC(0x022EE32C, void, this);
    mpMorf->calc();
    set_ke_root(6, 7, 0);
    set_ke_root(10, 11, 4);
    tt_ke_s* ke = mLineKe;
    f32 y = current.pos.y + 3.0f;
    for (int i = 0; i < 8; i++, ke++) {
        ke->ke_control(y);
        ke->ke_pos_set(lineMat0_getPos(&mLineMat, i));
    }
}
VERIFY(0x022EE32C, &daNpc_Tt_c::ke_execute);

/* 022EE3FC */
BOOL daNpc_Tt_c::init() {
    WWHD_FUNC(0x022EE3FC, BOOL, this);
    gravity = -30.0f;
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAB); /* attention_info.distances[TALK] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAB); /* attention_info.distances[SPEAK] */
    actor_status = (actor_status & ~0x3Fu) | 0x27; /* fopAcM_SetStatusMap(0x27) + OnStatus(SHOWMAP) */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = ACTION_SPEAK | LOCKON_TALK */
    switch ((s8)mType) {
    case 0: {
        /* setAction(&daNpc_Tt_c::wait_action, NULL) (inline) */
        ProcFunc_l* cur = &mCurrActionFunc;
        bool same = false;
        bool callOld = true;
        s16 i = cur->i;
        if (i == -1) {
            same = cur->d == 0 && cur->f == 0x022F0664;
        } else if (i == 0) {
            callOld = false;
        }
        if (!same) {
            if (callOld) {
                mActionStatus = -1; /* ACTION_ENDING */
                pmf_call(this, cur, nullptr);
            }
            mActionStatus = 0; /* ACTION_STARTING */
            cur->d = 0;
            cur->i = -1;
            cur->f = 0x022F0664; /* wait_action */
            gabi::call_ptr<BOOL>(cur->f, this, (void*)nullptr);
        }
        break;
    }
    }
    mAttnBasePos.copy(current.pos);
    mEyePos.copy(current.pos);
    J3DModel* model = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mtx_wcopy(gabi::at<Mtx34>(gabi::ea(model) + 0xC8), mDoMtx_stack_c::get()); /* model->setBaseTRMtx */
    ke_execute();
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    gabi::store<u32>(gabi::ea(this) + 0x104, 0x022ED8B0); /* eventInfo.setXyCheckCB(daNpc_tt_XyCheckCB) */
    gabi::store<u32>(gabi::ea(this) + 0x100, 0x022ED8B4); /* eventInfo.setXyEventCB(daNpc_tt_XyEventCB) */
    mMtrlSndId = 0;
    mReverb = (s8)dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    return TRUE;
}
VERIFY(0x022EE3FC, &daNpc_Tt_c::init);

/* 022EE674 */
cPhs_State daNpc_Tt_c::_create() {
    WWHD_FUNC(0x022EE674, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_Tt_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            gabi::call(0x025D4ED0, this); /* fopAc_ac_c::fopAc_ac_c */
            /* dNpc_HeadAnm_c::dNpc_HeadAnm_c() (inline) */
            mHeadAnm.field_0x14 = 0.0f;
            mHeadAnm.field_0x18 = 0.0f;
            mHeadAnm.field_0x1C = 0;
            mHeadAnm.field_0x1E = 0;
            mHeadAnm.field_0x20 = 0;
            mHeadAnm.field_0x00.x = 0;
            mHeadAnm.field_0x00.y = 0;
            mHeadAnm.field_0x00.z = 0;
            __vtbl = TT_VTBL;
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mObjAcch, dBgS_ObjAcch_vt{0x10022D84, 0x10022DA4, 0x10022D94});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x10022D74);
            gabi::call(0x0259DAA0, &m_jnt); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            lineMat0_ct(&mLineMat);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x10022EFC) /* "Tt" */);
    if (state == cPhs_COMPLEATE_e) {
        switch (fpcM_GetName(this)) {
        case fpcNm_NPC_TT_e:
            mType = 0;
            break;
        default:
            return cPhs_ERROR_e;
        }
        if (!fopAcM_entrySolidHeap(this, 0x022EDF60 /* CheckCreateHeap */, 0xE880)) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
        if (!init()) {
            mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
    }
    return state;
}
VERIFY(0x022EE674, &daNpc_Tt_c::_create);

/* 022EE894 */
static cPhs_State daNpc_Tt_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022EE894, cPhs_State, i_this);
    return ((daNpc_Tt_c*)i_this)->_create();
}
VERIFY(0x022EE894, daNpc_Tt_Create);

/* 022EE898 */
BOOL daNpc_Tt_c::_delete() {
    WWHD_FUNC(0x022EE898, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x10022EFF) /* "Tt" */);
    if (mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x022EE898, &daNpc_Tt_c::_delete);

/* 022EE8E4 */
static BOOL daNpc_Tt_Delete(daNpc_Tt_c* i_this) {
    WWHD_FUNC(0x022EE8E4, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x022EE8E4, daNpc_Tt_Delete);

/* 022EE8E8 */
void daNpc_Tt_c::playTexPatternAnm() {
    WWHD_FUNC(0x022EE8E8, void, this);
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
VERIFY(0x022EE8E8, &daNpc_Tt_c::playTexPatternAnm);

/* 022EE9AC */
void daNpc_Tt_c::setAnm(int newAnmIdx) {
    WWHD_FUNC(0x022EE9AC, void, this, newAnmIdx);
    if (mTexPatternIdx != 0) {
        mTexPatternIdx = 0;
        initTexPatternAnm(true);
    }
    s8 anmIdx = (s8)newAnmIdx;
    f32 morf = 8.0f;
    f32 speed = 1.0f;
    s32 loopMode = 2; /* J3DFrameCtrl::EMode_LOOP */
    if (newAnmIdx >= 7 && newAnmIdx <= 31) {
        morf = 7.0f;
        mSoundTimer = 7; /* (s16)morf */
    }
    if (newAnmIdx == 6) {
        speed = 0.9f;
    }
    if (newAnmIdx < 5) {
        clrFlag(0x40);
    } else {
        loopMode = 0; /* J3DFrameCtrl::EMode_NONE */
    }
    if (newAnmIdx == 2) {
        speed = 0.0f;
    }
    if (anmIdx != mCurrAnmIdx && anmIdx != -1) {
        mCurrAnmIdx = anmIdx;
        mAnmLoopCount = 0;
        mAnmTimer = 0.0f;
        dNpc_setAnm(mpMorf, loopMode, morf, speed, gabi::load<s16>(0x10022DC4 + 2 * anmIdx) /* l_bck_ix_tbl */,
                    gabi::load<s16>(0x10022E04 + 2 * anmIdx) /* l_bas_ix_tbl */, STR(0x10022F14) /* "Tt" */);
    }
}
VERIFY(0x022EE9AC, &daNpc_Tt_c::setAnm);

/* 022EEAFC */
void daNpc_Tt_c::talkInit() {
    WWHD_FUNC(0x022EEAFC, void, this);
    mTalkState = TALK_INIT;
    mMsgAnmIdx = 0xFF;
}
VERIFY(0x022EEAFC, &daNpc_Tt_c::talkInit);

/* 022EEB10 */
void daNpc_Tt_c::checkOrder() {
    WWHD_FUNC(0x022EEB10, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd != 2 /* !checkCommandDemoAccrpt() */) {
        if (cmd == 1 /* checkCommandTalk() */ && ChkOrder(7)) {
            if (dComIfGp_event_chkTalkXY()) {
                setFlag(0x4);
            } else {
                setFlag(0x2);
            }
            talkInit();
        }
    }
    ClrOrder();
}
VERIFY(0x022EEB10, &daNpc_Tt_c::checkOrder);

/* 022EEBA0 */
void daNpc_Tt_c::eventOrder() {
    WWHD_FUNC(0x022EEBA0, void, this);
    if (ChkOrder(1)) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        fopAcM_orderSpeakEvent(this);
    }
    if (ChkOrder(2)) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
    }
    if (ChkOrder(4)) {
        eventInfo_onCondition(this, 0x20 /* dEvtCnd_CANTALKITEM_e */);
    }
}
VERIFY(0x022EEBA0, &daNpc_Tt_c::eventOrder);

/* 022EEC18 */
void daNpc_Tt_c::setCollision() {
    WWHD_FUNC(0x022EEC18, void, this);
    gabi::Local<cXyz> centerPos;
    centerPos->copy(current.pos);
    mCyl.SetC(centerPos);
    mCyl.SetR(48.0f);
    mCyl.SetH(140.0f);
    dComIfG_Ccsp_Set(&mCyl);
}
VERIFY(0x022EEC18, &daNpc_Tt_c::setCollision);

/* 022EECA0 */
BOOL daNpc_Tt_c::_execute() {
    WWHD_FUNC(0x022EECA0, BOOL, this);
    m_jnt.setParam(0, 0x1C70, 0, -0x1C70, 0x1FFE, 9000, -0x1FFE, -9000, 0x1000);
    playTexPatternAnm();
    mAnmEnded = (s8)mpMorf->play(&eyePos, mMtrlSndId, mReverb);
    if (mpMorf->getFrame() < mAnmTimer) {
        mAnmEnded = 1;
    }
    mAnmTimer = mpMorf->getFrame();
    if (mAnmEnded != 0) {
        switch ((s32)mCurrAnmIdx) {
        case 3:
            if (mAnmLoopCount > 0) {
                setAnm(0);
            } else {
                mAnmLoopCount = mAnmLoopCount + 1;
            }
            break;
        }
    }
    mHeadAnm.move();
    checkOrder();
    pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
    eventOrder();
    fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
    mObjAcch.CrrPos(dComIfG_Bgsp());
    mMtrlSndId = 0;
    void* gnd = gabi::at<u8>(gabi::ea(&mObjAcch) + 0xD4 + 0x14); /* mObjAcch.m_gnd */
    if (!mObjAcch.ChkGroundHit()) {
        mMtrlSndId = dBgS_GetMtrlSndId_l(dComIfG_Bgsp(), gnd);
    }
    tevStr.mRoomNo = dBgS_GetRoomId(dComIfG_Bgsp(), gnd);
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor(dComIfG_Bgsp(), gnd)); /* tevStr.mEnvrIdxOverride */
    J3DModel* pModel = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mtx_wcopy(gabi::at<Mtx34>(gabi::ea(pModel) + 0xC8), mDoMtx_stack_c::get()); /* pModel->setBaseTRMtx */
    setCollision();
    ke_execute();
    return TRUE;
}
VERIFY(0x022EECA0, &daNpc_Tt_c::_execute);

/* 022EEF64 */
static BOOL daNpc_Tt_Execute(daNpc_Tt_c* i_this) {
    WWHD_FUNC(0x022EEF64, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x022EEF64, daNpc_Tt_Execute);

/* 022EEF68 */
BOOL daNpc_Tt_c::_draw() {
    WWHD_FUNC(0x022EEF68, BOOL, this);
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pModelData = J3DModel_getModelData_l(pModel);
    if (chkFlag(0x8)) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, pModelData, mBlinkFrame);
    mpMorf->entryDL();
    gabi::store<u32>(gabi::ea(pModelData) + 0x38, 0); /* mBtpAnm.remove(pModelData) */
    /* HD: no dComIfGd_setShadow */
    lineMat0_update(&mLineMat, 10, 0.8f, 0x101C68C4 /* {0xC9, 0xCA, 0xE4, 0xFF} */, 0, &tevStr);
    dComIfGd_set3DlineMat(&mLineMat);
    dSnap_RegistFig(0x57 /* DSNAP_TYPE_NPC_TT */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x022EEF68, &daNpc_Tt_c::_draw);

/* 022EF068 */
static BOOL daNpc_Tt_Draw(daNpc_Tt_c* i_this) {
    WWHD_FUNC(0x022EF068, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x022EF068, daNpc_Tt_Draw);

/* 022EF06C */
static BOOL daNpc_Tt_IsDelete(daNpc_Tt_c*) {
    WWHD_FUNC(0x022EF06C, BOOL, (daNpc_Tt_c*)nullptr);
    return TRUE;
}
VERIFY(0x022EF06C, daNpc_Tt_IsDelete);

/* 022EF074 */
void daNpc_Tt_c::danceInit(int param_1) {
    WWHD_FUNC(0x022EF074, void, this, param_1);
    if (param_1 != 0 || !chkFlag(0x40)) {
        mDanceStep = 0;
        mDanceStepTimer = 8;
        setAnm(5);
        setFlag(0x40);
    }
}
VERIFY(0x022EF074, &daNpc_Tt_c::danceInit);

/* 022EF0D8 */
void daNpc_Tt_c::setAnmStatus() {
    WWHD_FUNC(0x022EF0D8, void, this);
    switch ((s32)mState) {
    case 1:
    case 2:
        if (dComIfGs_isEventBit(0xC40 /* UNK_0C40 */)) {
            danceInit(0);
        } else {
            setAnm(4);
        }
        break;
    case 3:
        break;
    default:
        setAnm(0);
        break;
    }
}
VERIFY(0x022EF0D8, &daNpc_Tt_c::setAnmStatus);

/* 022EF190 */
u32 daNpc_Tt_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022EF190, u32, this, pMsgNo);
    u16 msgStatus = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch ((u32)*pMsgNo) {
    case 0x22C5:
    case 0x22C7:
    case 0x22C8:
    case 0x22D5:
    case 0x22D6:
        *pMsgNo = *pMsgNo + 1;
        break;
    case 0x22C6:
        *pMsgNo = 0x22D5;
        break;
    case 0x22D7:
        *pMsgNo = 0x22C7;
        break;
    default:
        msgStatus = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msgStatus;
}
VERIFY(0x022EF190, &daNpc_Tt_c::next_msgStatus);

/* 022EF208 */
u32 daNpc_Tt_c::getMsg() {
    WWHD_FUNC(0x022EF208, u32, this);
    u32 msgNo;
    s8 state = mState;
    clrFlag(0x20);
    switch ((s32)state) {
    case 2:
        if (dComIfGs_isEventBit(0xC40 /* UNK_0C40 */)) {
            msgNo = 0x22CB;
        } else if (dComIfGs_isEventBit(0xB08 /* UNK_0B08 */)) {
            msgNo = 0x22CA;
            setFlag(0x20);
        } else {
            dComIfGs_onEventBit(0xB08);
            msgNo = 0x22C5;
            setFlag(0x20);
        }
        break;
    default:
        msgNo = mMsgNo;
        switch (msgNo) {
        case 0x5B3:
            dComIfGp_setMelodyNum(5);
            break;
        }
    }
    return msgNo;
}
VERIFY(0x022EF208, &daNpc_Tt_c::getMsg);

/* 022EF2E4 */
void daNpc_Tt_c::msgPushButton() {
    WWHD_FUNC(0x022EF2E4, void, this);
    if ((s32)mCurrMsgNo != 0x22C6) {
        return;
    }
    mNoticeLinkTimer = 0xF;
}
VERIFY(0x022EF2E4, &daNpc_Tt_c::msgPushButton);

/* 022EF2FC */
void daNpc_Tt_c::msgContinue() {
    WWHD_FUNC(0x022EF2FC, void, this);
    switch ((u32)mCurrMsgNo) {
    case 0x22D5:
        if (mNoticeLinkTimer > 0) {
            mNoticeLinkTimer = mNoticeLinkTimer - 1;
        } else {
            gabi::store<u8>(msgMgr() + 0x921, 1); /* fopMsgM_messageSendOn() */
        }
        break;
    }
}
VERIFY(0x022EF2FC, &daNpc_Tt_c::msgContinue);

/* 022EF334 */
void daNpc_Tt_c::msgAnm() {
    WWHD_FUNC(0x022EF334, void, this);
    u8 attr = dComIfGp_getMesgAnimeAttrInfo();
    if (mMsgAnmIdx != attr) {
        mMsgAnmIdx = dComIfGp_getMesgAnimeAttrInfo();
        switch ((u32)mMsgAnmIdx) {
        case 0:
            setAnm(0);
            break;
        case 1:
            setAnm(1);
            break;
        case 2:
            setAnm(3);
            break;
        case 3:
            setAnm(4);
            break;
        case 4:
            danceInit(0);
            break;
        case 5:
            setAnm(2);
            break;
        }
    }
}
VERIFY(0x022EF334, &daNpc_Tt_c::msgAnm);

/* 022EF464 */
u16 daNpc_Tt_c::talk() {
    WWHD_FUNC(0x022EF464, u16, this);
    /* HD: the message is the message manager (GameCube: l_msg found by l_msgId) */
    u32 mgr = msgMgr();
    u16 msgStatus = 0xFF;
    if (mTalkState == TALK_INIT) {
        l_msgId() = 0xFFFFFFFF; /* fpcM_ERROR_PROCESS_ID_e */
        mCurrMsgNo = getMsg();
        mTalkState = TALK_MSG_CREATE;
    } else if (mTalkState != TALK_FINISHED) {
        if (l_msgId() == 0xFFFFFFFF) {
            l_msgId() = msgMgr_messageSet(mgr, mCurrMsgNo, &eyePos);
        } else {
            msgAnm();
            switch ((u32)(s32)mTalkState) {
            case TALK_MSG_CREATE:
                mTalkState = TALK_ACTIVE; /* HD: no fopMsgM_SearchByID */
                break;
            case TALK_ACTIVE:
                msgStatus = msgMgr_getStatus(mgr);
                if (msgStatus == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
                    msgContinue();
                } else if (msgStatus == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
                    msgPushButton();
                    msgMgr_setStatus(mgr, next_msgStatus(&mCurrMsgNo));
                    if (msgMgr_getStatus(mgr) == 0xF) {
                        msgMgr_messageSet(mgr, mCurrMsgNo, nullptr);
                    }
                } else if (msgStatus == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
                    msgMgr_setStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
                    l_msgId() = 0xFFFFFFFF;
                }
                break;
            }
        }
    }
    return msgStatus;
}
VERIFY(0x022EF464, &daNpc_Tt_c::talk);

/* 022EF5C4 */
void daNpc_Tt_c::setAttention(bool param_1) {
    WWHD_FUNC(0x022EF5C4, void, this, param_1);
    if (!param_1 && mAttnSetCount >= 2) {
        return;
    }
    u32 att = gabi::ea(this) + 0x390; /* attention_info.position */
    gabi::store<u32>(att, gabi::load<u32>(mAttnBasePos.x.addr()));
    gabi::store<f32>(att + 4, mAttnBasePos.y + 65.0f);
    gabi::store<u32>(att + 8, gabi::load<u32>(mAttnBasePos.z.addr()));
    eyePos.copy(mEyePos); /* eyePos.set(mEyePos.x, mEyePos.y, mEyePos.z) */
}
VERIFY(0x022EF5C4, &daNpc_Tt_c::setAttention);

/* 022EF618 */
void daNpc_Tt_c::lookBack() {
    WWHD_FUNC(0x022EF618, void, this);
    gabi::Local<cXyz> eye;   /* dNpc_playerEyePos result */
    gabi::Local<cXyz> temp2;
    gabi::Local<cXyz> temp;  /* passed by value: a copy */
    u32 tx = 0, ty = 0, tz = 0; /* 0.0f, bit copies */
    cXyz* dstPos = nullptr;
    s16 desiredYRot = current.angle.y;
    switch ((s32)mState) {
    case 1:
        desiredYRot = home.angle.y;
        m_jnt.mbTrn = desiredYRot != current.angle.y ? 1 : 0; /* setTrn() / clrTrn() */
        break;
    default:
        if (chkFlag(0x20)) {
            desiredYRot = home.angle.y;
            m_jnt.mbTrn = 0; /* clrTrn() */
        } else {
            m_jnt.mbTrn = 1; /* setTrn() */
            dNpc_playerEyePos_l(eye, -20.0f);
            temp2->copy(*eye);
            dstPos = temp2;
            tx = gabi::load<u32>(current.pos.x.addr());
            tz = gabi::load<u32>(current.pos.z.addr());
            ty = gabi::load<u32>(eyePos.y.addr());
        }
        break;
    }
    if (m_jnt.mbTrn != 0) { /* m_jnt.trnChk() */
        cLib_addCalcAngleS2(&mMaxHeadTurnVelocity, 0x800, 4, 0x800);
    } else {
        mMaxHeadTurnVelocity = 0;
    }
    gabi::store<u32>(temp->x.addr(), tx);
    gabi::store<u32>(temp->y.addr(), ty);
    gabi::store<u32>(temp->z.addr(), tz);
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos, temp, desiredYRot, mMaxHeadTurnVelocity, false);
    shape_angle.y = current.angle.y;
}
VERIFY(0x022EF618, &daNpc_Tt_c::lookBack);

/* 022EF830 */
s32 daNpc_Tt_c::getNowEventAction() {
    WWHD_FUNC(0x022EF830, s32, this);
    /* static char* action_table[] = {"WAIT", "SPEAK", "PATTEN", "CHANGE", "TACT0", "TACT1"} (.data 0x101C68C8) */
    return dComIfGp_evmng_getMyActIdx(mStaffIdx, 0x101C68C8, 6, FALSE, 1);
}
VERIFY(0x022EF830, &daNpc_Tt_c::getNowEventAction);

/* 022EF87C */
void daNpc_Tt_c::demoInitCom() {
    WWHD_FUNC(0x022EF87C, void, this);
    fopAc_ac_c* pLink = daPy_getPlayerLinkActorClass();
    be<s32>* hidePl = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x10022F58) /* "HIDE_PL" */);
    if (hidePl != nullptr) {
        /* daPy_lk_c::onPlayerNoDraw() / offPlayerNoDraw(): bit 0x08000000 of the word at +0x3B8 */
        u32 a = gabi::ea(pLink) + 0x3B8;
        if (*hidePl != 0) {
            gabi::store<u32>(a, gabi::load<u32>(a) | 0x08000000);
        } else {
            gabi::store<u32>(a, gabi::load<u32>(a) & ~0x08000000u);
        }
    }
    be<s32>* disp = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x10022F64) /* "DISP" */);
    if (disp != nullptr) {
        if (*disp != 0) {
            clrEvFlag(0x2);
        } else {
            setEvFlag(0x2);
        }
    }
    be<s32>* anm = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x10022F60) /* "Anm" */);
    if (anm != nullptr) {
        setAnm(*anm & 0xFF);
    }
}
VERIFY(0x022EF87C, &daNpc_Tt_c::demoInitCom);

/* 022EF9D8 */
void daNpc_Tt_c::demoInitSpeak() {
    WWHD_FUNC(0x022EF9D8, void, this);
    talkInit();
    be<s32>* a_intP = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x10022F6C) /* "MsgNo" */);
    if (a_intP == nullptr) /* JUT_ASSERT(888, a_intP) */
        JUT_ASSERT_fail(STR(0x10022F7C), 0x378, STR(0x10022F74));
    u32 msgNo = (u32)(s32)*a_intP;
    mMsgNo = msgNo;
    switch (msgNo) {
    case 0x22CE:
        if (dKy_daynight_check() == 1 /* dKy_TIME_NIGHT_e */) {
            mMsgNo = 0x22CF;
        }
        break;
    }
}
VERIFY(0x022EF9D8, &daNpc_Tt_c::demoInitSpeak);

/* 022EFA74 */
void daNpc_Tt_c::demoInitWait() {
    WWHD_FUNC(0x022EFA74, void, this);
    be<s32>* pTimer = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x10022F8C) /* "Timer" */);
    if (pTimer != nullptr) {
        mTimer = (s16)*pTimer;
    } else {
        mTimer = 0;
    }
}
VERIFY(0x022EFA74, &daNpc_Tt_c::demoInitWait);

/* 022EFAD8 */
void daNpc_Tt_c::demoInitPatten() {
    WWHD_FUNC(0x022EFAD8, void, this);
    setEvFlag(0x4);
    mSoundTimer = 0;
    danceInit(0);
    be<s32>* cnt = (be<s32>*)dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x10022F94) /* "Cnt" */);
    if (cnt != nullptr) {
        field_0xE7A = (u8)*cnt;
    } else {
        field_0xE7A = 2;
    }
}
VERIFY(0x022EFAD8, &daNpc_Tt_c::demoInitPatten);

/* 022EFB54 */
void daNpc_Tt_c::demoInitChange() {
    WWHD_FUNC(0x022EFB54, void, this);
    void* a_intP = dComIfGp_evmng_getMyIntegerP(mStaffIdx, STR(0x10022F98) /* "prm0" */);
    if (a_intP == nullptr) { /* JUT_ASSERT(763, a_intP) */
        JUT_ASSERT_fail(STR(0x10022FB4), 0x2FB, STR(0x10022FA0));
        return;
    }
    mEventIdx = dComIfGp_evmng_getEventIdx(STR(0x10022FA8) /* "TACT_TT11" */, 0xFF);
    setEvFlag(0x1);
    fopAc_ac_c* link = daPy_getPlayerLinkActorClass();
    s16 ev = mEventIdx;
    fopAcM_orderChangeEventId(link, this, ev, 0, 0xFFFF);
}
VERIFY(0x022EFB54, &daNpc_Tt_c::demoInitChange);

/* 022EFC24 */
bool daNpc_Tt_c::demoProcSpeak() {
    WWHD_FUNC(0x022EFC24, bool, this);
    u16 temp = talk();
    if (temp == 0x12 /* fopMsgStts_BOX_CLOSED_e */ || temp == 0xFE) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return false;
}
VERIFY(0x022EFC24, &daNpc_Tt_c::demoProcSpeak);

/* 022EFC78 */
bool daNpc_Tt_c::demoProcWait() {
    WWHD_FUNC(0x022EFC78, bool, this);
    if (mTimer > 0) {
        mTimer = mTimer - 1;
    } else {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return FALSE;
}
VERIFY(0x022EFC78, &daNpc_Tt_c::demoProcWait);

/* 022EFCD0 */
BOOL daNpc_Tt_c::danceNext() {
    WWHD_FUNC(0x022EFCD0, BOOL, this);
    if (mDanceStep < 6) {
        mDanceStep = mDanceStep + 1;
    } else {
        mDanceStep = 0;
    }
    switch ((u32)mDanceStep) {
    case 0:
    case 2:
        setAnm(5);
        mDanceStepTimer = 8;
        break;
    case 1:
        setAnm(10);
        mDanceStepTimer = 8;
        break;
    case 3:
        setAnm(9);
        mDanceStepTimer = 8;
        break;
    case 4:
        setAnm(6);
        mDanceStepTimer = 58;
        break;
    case 5:
        if (chkFlag(0x100)) {
            clrFlag(0x100);
            fopAcM_seStart(this, 0x48A1 /* JA_SE_CV_TT_DANCE_FIN */, 0);
        }
        setAnm(8);
        mDanceStepTimer = 53;
        break;
    }
    return mDanceStep == 0;
}
VERIFY(0x022EFCD0, &daNpc_Tt_c::danceNext);

/* 022EFE70 */
BOOL daNpc_Tt_c::danceProc() {
    WWHD_FUNC(0x022EFE70, BOOL, this);
    if (!chkFlag(0x40)) {
        return FALSE;
    }
    if (curMorf() < 1.0f) { /* mpMorf->isMorf() */
        return FALSE;
    }
    if (mDanceStep == 5 && mDanceStepTimer == 0x19) {
        mHeadAnm.swing_vertical_init(2, 0x1194, -1000, 1);
    }
    if (mDanceStepTimer != 0) {
        mDanceStepTimer = mDanceStepTimer - 1;
        return FALSE;
    }
    return danceNext();
}
VERIFY(0x022EFE70, &daNpc_Tt_c::danceProc);

/* 022EFF38 */
bool daNpc_Tt_c::demoProcPatten() {
    WWHD_FUNC(0x022EFF38, bool, this);
    if (field_0xE7A == 0) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
        clrEvFlag(0x4);
    } else {
        if (danceProc()) {
            field_0xE7A = field_0xE7A - 1;
        }
        if (mSoundTimer > 0) {
            mSoundTimer = mSoundTimer - 1;
            if (mSoundTimer == 0) {
                switch ((s32)mCurrAnmIdx) {
                case 8:
                    fopAcM_seStart(this, 0x48A1 /* JA_SE_CV_TT_DANCE_FIN */, 0);
                    break;
                case 9:
                    fopAcM_seStart(this, 0x4902 /* JA_SE_CV_TT_DANCE_2 */, 0);
                    break;
                case 10:
                    fopAcM_seStart(this, 0x4901 /* JA_SE_CV_TT_DANCE_1 */, 0);
                    break;
                }
            }
        }
    }
    return FALSE;
}
VERIFY(0x022EFF38, &daNpc_Tt_c::demoProcPatten);

/* 022F0070 */
bool daNpc_Tt_c::demoProcTact1() {
    WWHD_FUNC(0x022F0070, bool, this);
    /* daPy_lk_c::getTactTimerCancel(): virtual, HD vtable (+0xB4) slot 0x34 */
    fopAc_ac_c* link = daPy_getPlayerLinkActorClass();
    u32 cancel = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(link) + 0xB4) + 0x34), link);
    switch (cancel) {
    case 1: {
        mEventIdx = dComIfGp_evmng_getEventIdx(STR(0x10022FC4) /* "TACT_TT12" */, 0xFF);
        setEvFlag(0x1);
        fopAc_ac_c* l = daPy_getPlayerLinkActorClass();
        s16 ev = mEventIdx;
        fopAcM_orderChangeEventId(l, this, ev, 0, 0xFFFF);
        break;
    }
    case 2: {
        mEventIdx = dComIfGp_evmng_getEventIdx(STR(0x10022FD0) /* "TACT_TT13" */, 0xFF);
        setEvFlag(0x1);
        fopAc_ac_c* l = daPy_getPlayerLinkActorClass();
        s16 ev = mEventIdx;
        fopAcM_orderChangeEventId(l, this, ev, 0, 0xFFFF);
        break;
    }
    default:
        dComIfGs_onEventBit(0xC40 /* UNK_0C40 */);
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    }
    return TRUE;
}
VERIFY(0x022F0070, &daNpc_Tt_c::demoProcTact1);

/* 022F0190 */
void daNpc_Tt_c::demoProcCom() {
    WWHD_FUNC(0x022F0190, void, this);
    if (chkEvFlag(0x2)) {
        setFlag(0x8);
    }
}
VERIFY(0x022F0190, &daNpc_Tt_c::demoProcCom);

/* 022F01AC */
bool daNpc_Tt_c::demoProc() {
    WWHD_FUNC(0x022F01AC, bool, this);
    bool temp = false;
    if (chkEvFlag(0x1)) {
        mStaffIdx = dComIfGp_evmng_getMyStaffId(STR(0x10022FDC) /* "Tt" */, nullptr, 0);
        clrEvFlag(0x1);
    }
    s32 eventAction = getNowEventAction();
    if (dComIfGp_evmng_getIsAddvance(mStaffIdx)) {
        demoInitCom();
        setFlag(0x80);
        switch (eventAction) {
        case 1:
            demoInitSpeak();
            break;
        case 0:
            demoInitWait();
            break;
        case 2:
            demoInitPatten();
            temp = true;
            break;
        case 3:
            demoInitChange();
            break;
        }
    }
    switch (eventAction) {
    case 1:
        temp = demoProcSpeak();
        break;
    case 0:
        demoProcWait();
        break;
    case 2:
        demoProcPatten();
        break;
    case 3:
        break;
    case 4:
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    case 5:
        demoProcTact1();
        break;
    default:
        dComIfGp_evmng_cutEnd(mStaffIdx);
        break;
    }
    demoProcCom();
    if (eventAction != 2) {
        danceProc();
    }
    return temp;
}
VERIFY(0x022F01AC, &daNpc_Tt_c::demoProc);

/* 022F03B4 */
bool daNpc_Tt_c::wait01() {
    WWHD_FUNC(0x022F03B4, bool, this);
    if (chkFlag(0x4)) {
        mReturnToState = mState;
        mState = 3;
        setAnmStatus();
        mTimer = 0x14;
        setEvFlag(0x1);
        fopAcM_seStart(this, 0x8A7 /* JA_SE_PRE_TAKT */, 0);
        return demoProc();
    } else if (chkFlag(0x2)) {
        mReturnToState = mState;
        mState = 2;
        setAnmStatus();
    } else {
        SetOrder(0x2);
        if (!dComIfGs_isEventBit(0xC40 /* UNK_0C40 */) && dComIfGs_isEventBit(0xB08 /* UNK_0B08 */)) {
            SetOrder(0x4);
        }
    }
    danceProc();
    return curMorf() < 1.0f; /* mpMorf->isMorf() */
}
VERIFY(0x022F03B4, &daNpc_Tt_c::wait01);

/* 022F0514 */
bool daNpc_Tt_c::talk01() {
    WWHD_FUNC(0x022F0514, bool, this);
    u16 temp = talk();
    if (mCurrMsgNo == 0x22C6 && dComIfGp_checkMesgSendButton()) {
        clrFlag(0x20);
    }
    if (temp == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mState = mReturnToState;
        setAnmStatus();
        dComIfGp_event_reset();
        clrFlag(0x2 | 0x4);
    }
    danceProc();
    return curMorf() < 1.0f; /* mpMorf->isMorf() */
}
VERIFY(0x022F0514, &daNpc_Tt_c::talk01);

/* 022F05CC */
bool daNpc_Tt_c::tact00() {
    WWHD_FUNC(0x022F05CC, bool, this);
    if (dComIfGp_evmng_endCheck(mEventIdx)) {
        mState = mReturnToState;
        setAnmStatus();
        dComIfGp_event_reset();
        clrFlag(0x2 | 0x4);
        return true;
    }
    return demoProc();
}
VERIFY(0x022F05CC, &daNpc_Tt_c::tact00);

/* 022F0664 */
BOOL daNpc_Tt_c::wait_action(void*) {
    WWHD_FUNC(0x022F0664, BOOL, this, (void*)nullptr);
    if (mActionStatus == ACTION_STARTING) {
        mState = 1;
        setAnmStatus();
        mActionStatus = mActionStatus + 1; /* ACTION_ONGOING */
    } else if (mActionStatus != ACTION_ENDING) {
        bool temp;
        clrFlag(0x8);
        switch ((s32)mState) {
        case 1:
            temp = wait01();
            break;
        case 2:
            temp = talk01();
            break;
        case 3:
            temp = tact00();
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
VERIFY(0x022F0664, &daNpc_Tt_c::wait_action);

/* 022F0788: static initialisation of the translation unit (only the per-TU header statics) */
static void __sinit_d_a_npc_tt_cpp() {
    WWHD_FUNC(0x022F0788, void);
    sinit_header_statics(0x10468904, 0x101C68E0);
}
VERIFY(0x022F0788, __sinit_d_a_npc_tt_cpp);

/* 022F081C: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x022F081C, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x022F081C, SafeString_dt);

/* 022F0830: daNpc_Tt_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Tt_c_dt(daNpc_Tt_c* i_this, s32 flags) {
    WWHD_FUNC(0x022F0830, void, i_this, flags);
    if (i_this != nullptr) {
        lineMat0_dt(&i_this->mLineMat, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x10022D94);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x10022DA4);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0); /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x025D50BC, i_this, 0);            /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022F0830, daNpc_Tt_c_dt);

/* 022F08D8: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x022F08D8, void, (SafeString*)nullptr);
}
VERIFY(0x022F08D8, SafeString_assureTerminationImpl);
