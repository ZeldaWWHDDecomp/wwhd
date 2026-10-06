/**
 * d_a_npc_sv.cpp (WWHD)
 * NPC - Salvage Corp. members
 *
 * The GameCube decompilation of this TU
 * (zeldaret/tww src/d/actor/d_a_npc_sv.cpp) has only "Nonmatching" placeholders: every function
 * below is written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube
 * names and the TU's function list. "HD:" marks a known difference from the GameCube TU.
 */
#include "d/actor/d_a_npc_sv.h"

#define SAFESTRING_VTBL 0x10022430 /* this TU's sead::SafeString vtable */
#define SV_VTBL 0x100226A4         /* daNpcSv_c vtable (HD virtual destructor) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025A1458 fopNpc_npc_c::fopNpc_npc_c (matcher: cDyl_LinkASync) */
static inline void fopNpc_npc_c_ct(void* p) { gabi::call(0x025A1458, p); }
/* 02525FE4 dComLbG_PhaseHandler(request_of_phase_process_class*, cPhs__Handler* table, void* user) */
static inline cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class* p, u32 tbl, void* self) {
    return gabi::call<cPhs_State>(0x02525FE4, p, tbl, self);
}
/* 02520C0C dComIfGs_checkGetItem(u8) */
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
/* 025B7A2C dSv_player_collect_c::isCollect(int field, u8 item) (collect at save + 0xD4) */
static inline BOOL dComIfGs_isCollect(s32 field, u8 item) {
    return gabi::call<BOOL>(0x025B7A2C, gabi::load<u32>(0x101F84DC) + 0xD4, field, item);
}
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz (pointers to copies), f32* dist, s16* angle) */
static inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* ang) { gabi::call(0x0259D624, a, b, dist, ang); }
/* 025D9F38 fopAcM_searchFromName(name, param mask, param) */
static inline fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm) {
    return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm);
}
/* 025D7DEC fopAcM_createItemForPresentDemo(pos, itemNo, argFlag, itemBitNo, roomNo, angle, scale) */
static inline s32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return gabi::call<s32>(0x025D7DEC, pos, itemNo, flag, bitNo, roomNo, angle, scale);
}
/* 025E1988 HD: mDoAud_seStart(id) (one argument) */
static inline void mDoAud_seStart_1(u32 id) { gabi::call(0x025E1988, id); }
/* 025F19F8 mDoMtx_XYZrotM */
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
/* 025F74D0 HD message manager setStatus(status) */
static inline void fopMsgM_setStatus(u32 mng, u32 st) { gabi::call(0x025F74D0, mng, st); }
/* dComIfGp_event_setTalkPartner: dEvt_control_c (play + 0x51D0) mPtTalk (+0xCC) = getPId(actor) */
static inline void dComIfGp_event_setTalkPartner(fopAc_ac_c* a) {
    u32 evt = dComIfGp_ea() + 0x51D0;
    gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, a));
}
/* dComIfGp_event_setItemPartnerId: dEvt_control_c mPtItem (play + 0x52A0) */
static inline void dComIfGp_event_setItemPartnerId(s32 id) { gabi::store<s32>(dComIfGp_ea() + 0x52A0, id); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
/* dComIfGp_getMesgAnimeAttrInfo / clear: play + 0x5BC5 */
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + 0x5BC5); }
static inline void dComIfGp_clearMesgAnimeAttrInfo() { gabi::store<u8>(dComIfGp_ea() + 0x5BC5, 0xFF); }
/* GHS pointer-to-member call (no argument) returning r3 */
static inline s32 ptmf_call_r(u32 entry, void* self) {
    s16 delta = gabi::load<s16>(entry);
    s16 idx = gabi::load<s16>(entry + 2);
    void* p = gabi::at<void>(gabi::ea(self) + delta);
    if (idx < 0) return gabi::call_ptr<s32>(gabi::load<u32>(entry + 4), p);
    u32 vt = gabi::load<u32>(gabi::ea(p) + gabi::load<s16>(entry + 6));
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + idx * 8 + 4), p);
}
/* virtuals of the actor vtable (+0xB4): next_msgStatus +0x14, getMsg +0x1C, anmAtr +0x24 */
static inline u32 vcall_next_msgStatus(fopNpc_npc_c_l* a, be<u32>* p) { return gabi::call_ptr<u32>(gabi::load<u32>(a->__vtbl + 0x14), a, p); }

/* ---- file statics (.data / .rodata) ---- */
static inline sSvNpcDat_l* l_npc_dat(u32 no) { return gabi::at<sSvNpcDat_l>(0x101C6668 + no * 0x44); }
static inline const char* l_arcname(u32 no) { return STR(gabi::load<u32>(0x101C6414 + no * 4)); }   /* "Sv0".."Sv3", "Sv"... */
static inline const char* l_staff_name(u32 no) { return STR(gabi::load<u32>(0x101C6404 + no * 4)); } /* "Sv0".."Sv3" */
static inline s32 l_bdl_ix(u32 no) { return gabi::load<s32>(0x10022468 + no * 4); }
static inline s32 l_bck_ix(u32 i) { return gabi::load<s32>(0x10022478 + i * 4); }       /* [npcNo * 6 + anm] */
static inline sSvAnmDat* l_npc_anm_sv1_tbl(u32 i) { return gabi::at<sSvAnmDat>(gabi::load<u32>(0x101C6424 + i * 4)); }
static inline u32 l_npc_se_sv1_tbl(u32 i) { return gabi::load<u32>(0x101C643C + i * 4); }
static inline sSvAnmDat* l_msg_anm_tbl(u32 i) { return gabi::at<sSvAnmDat>(gabi::load<u32>(0x10022678 + i * 4)); }
#define l_anm_wait gabi::at<sSvAnmDat>(0x101C63F0)
#define l_anm_end gabi::at<sSvAnmDat>(0x101C63F3)
#define l_anm_ship gabi::at<sSvAnmDat>(0x101C63FC)
static inline u32 l_msg_tbl(u32 t) { return gabi::load<u32>(0x101C64CC + t * 4); }
static inline u16 l_msg_flag_tbl(u32 t) { return gabi::load<u16>(0x101C64FC + t * 2); }
static inline s16 l_talk_ev_tbl(u32 t) { return gabi::load<s16>(0x101C6514 + t * 2); }
#define l_cut_name_tbl 0x101C6784u /* MES_SET, GET_ITEM, SET_ANGLE, ATTENTION, TURN_OK */
#define l_init_tbl 0x101C652Cu     /* executeWaitInit, executeTalkInit (pointers to members) */
#define l_exec_tbl 0x101C653Cu     /* executeWait, executeTalk */
#define l_method 0x101C6778u       /* phase_1, phase_2 */

/* 022E4CA0 */
static BOOL daNpc_People_nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022E4CA0, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(j3dSys_getModel());
        daNpcSv_c* i_this = gabi::at<daNpcSv_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        u32 jntNo = gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4);
        PSMTXCopy(J3DModel_getAnmMtx(model, jntNo), calc_mtx());
        /* HD: no null check of the user area */
        if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
            cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
            cMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][0]);
        }
        if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
            cMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            cMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[1][0]);
        }
        mtx_copy(J3DModel_getAnmMtx(model, jntNo), calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx);
    }
    return TRUE;
}
VERIFY(0x022E4CA0, daNpc_People_nodeCallBack);

/* 022E4DF8 */
BOOL daNpcSv_c::createHeap() {
    WWHD_FUNC(0x022E4DF8, BOOL, this);
    J3DModelData* a_mdl_dat = (J3DModelData*)dComIfG_getObjectIDRes(l_arcname(mNpcNo), l_bdl_ix(mNpcNo), SAFESTRING_VTBL);
    J3DAnmTransform* a_anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname(mNpcNo), l_bck_ix(mNpcNo * 6 + mAnm), SAFESTRING_VTBL);
    mpMorf = mDoExt_McaMorf::create(nullptr, a_mdl_dat, nullptr, nullptr, a_anm, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x37441422);
    if (mpMorf == nullptr || mpMorf->mpModel == nullptr) {
        return FALSE;
    }
    m_jnt.mHeadJntNum = (s8)J3DModelData_getJointIndex(a_mdl_dat, STR(0x100224E0) /* "head" */);
    if (m_jnt.mHeadJntNum < 0) /* JUT_ASSERT(0x3B6, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail(STR(0x100224E8), 0x3B6, STR(0x100224F8));
    m_jnt.mBackboneJntNum = (s8)J3DModelData_getJointIndex(a_mdl_dat, STR(0x10022514) /* "backbone" */);
    if (m_jnt.mBackboneJntNum < 0) /* JUT_ASSERT(0x3BA, m_jnt.getBackboneJntNum() >= 0) */
        JUT_ASSERT_fail(STR(0x100224E8), 0x3BA, STR(0x10022520));
    for (u16 i = 0; i < J3DModelData_getJointNum(a_mdl_dat); i++) {
        if (i == m_jnt.mHeadJntNum || i == m_jnt.mBackboneJntNum) {
            J3DModelData_setJointCallBack(gabi::ea(a_mdl_dat), i, 0x022E4CA0 /* daNpc_People_nodeCallBack */);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->mpModel.get()) + 0xB8, gabi::ea(this)); /* model->setUserArea(this) */
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    return TRUE;
}
VERIFY(0x022E4DF8, &daNpcSv_c::createHeap);

/* 022E50AC */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022E50AC, BOOL, i_this);
    return static_cast<daNpcSv_c*>(i_this)->createHeap();
}
VERIFY(0x022E50AC, CheckCreateHeap);

/* 022E50B0 */
u8 daNpcSv_c::getPrmNpcNo() {
    WWHD_FUNC(0x022E50B0, u8, this);
    s32 no = argument;
    if ((u32)no >= 4) return 0;
    return (u8)no;
}
VERIFY(0x022E50B0, &daNpcSv_c::getPrmNpcNo);

/* 022E50CC */
daNpcSv_c* daNpcSv_c::ct(daNpcSv_c* p) {
    WWHD_FUNC(0x022E50CC, daNpcSv_c*, p);
    if (p == nullptr) {
        p = (daNpcSv_c*)operator_new(0x860);
        if (p == nullptr) return nullptr;
    }
    fopNpc_npc_c_ct(p);
    p->__vtbl = SV_VTBL;
    u8 no = p->getPrmNpcNo();
    p->mAnm = 0;
    p->mNpcNo = no;
    p->mHomeAngleY = p->home.angle.y;
    p->m856 = 1;
    p->mMode = 0;
    p->mHeadOnly = 1;
    p->mCreated = 0;
    p->mFrame = 0;
    p->mLookMode = 0;
    p->mFlags = 0;
    p->mNextMorf = -1.0f;
    if (dComIfGs_checkGetItem(0xE5)) {
        p->mFlags |= 0x20;
    }
    return p;
}
VERIFY(0x022E50CC, &daNpcSv_c::ct);

/* 022E5184 */
static cPhs_State phase_1(daNpcSv_c* i_this) {
    WWHD_FUNC(0x022E5184, cPhs_State, i_this);
    /* fopAcM_SetupActor(i_this, daNpcSv_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) daNpcSv_c::ct(i_this);
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    i_this->mCreated = 1;
    return 2; /* cPhs_NEXT_e */
}
VERIFY(0x022E5184, phase_1);

/* 022E51E0 */
void daNpcSv_c::setAnm(u8 anm, int loopMode, f32 morf) {
    WWHD_FUNC(0x022E51E0, void, this, anm, loopMode, morf);
    if (!(mNextMorf < 0.0f)) {
        morf = mNextMorf;
        mNextMorf = -1.0f;
    }
    u32 no = mNpcNo;
    J3DAnmTransform* a_anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname(no), l_bck_ix(no * 6 + anm), SAFESTRING_VTBL);
    mpMorf->setAnm(a_anm, loopMode, morf, 1.0f, 0.0f, -1.0f, nullptr);
    mAnm = anm;
}
VERIFY(0x022E51E0, &daNpcSv_c::setAnm);

/* 022E52FC */
BOOL daNpcSv_c::setAnmTbl(sSvAnmDat* p) {
    WWHD_FUNC(0x022E52FC, BOOL, this, p);
    if (p->mAnm == 0xFF) {
        mpAnmTbl = nullptr;
        return TRUE;
    }
    mpAnmTbl = p;
    s8 loop = p->mLoop;
    mAnmLoop = loop;
    if (loop > 0) {
        setAnm(p->mAnm, 0, (f32)p->mMorf);
    } else if (mAnm != p->mAnm) {
        setAnm(p->mAnm, 2, (f32)p->mMorf);
    }
    return FALSE;
}
VERIFY(0x022E52FC, &daNpcSv_c::setAnmTbl);

/* 022E53C8 */
u8 daNpcSv_c::getTalkNo() {
    WWHD_FUNC(0x022E53C8, u8, this);
    BOOL has = dComIfGs_checkGetItem(0x25) != 0;
    u16 flags = mFlags;
    u8 no;
    mTalkNo = 0;
    if (!(flags & 0x20)) {
        if (!(flags & 1)) {
            no = (u8)has;
        } else if (!(flags & 2)) {
            no = has ? 3 : 2;
        } else {
            no = has ? 5 : 4;
        }
    } else if (!has) {
        no = (flags & 4) ? 7 : 6;
    } else if (!dComIfGs_isCollect(0, 1)) {
        no = (mFlags & 8) ? 9 : 8;
    } else {
        no = dComIfGs_isEventBit(0x2F80) ? 0xB : 0xA;
    }
    mTalkNo = no;
    return no;
}
VERIFY(0x022E53C8, &daNpcSv_c::getTalkNo);

/* 022E54E8 */
void daNpcSv_c::setMtx() {
    WWHD_FUNC(0x022E54E8, void, this);
    J3DModel* model = mpMorf->getModel();
    J3DModel_setBaseScale(model, &scale);
    sSvNpcDat_l* dat = l_npc_dat(mNpcNo);
    gabi::Local<cXyz> offset;
    offset->x = dat->mOffset.x;
    offset->y = dat->mOffset.y;
    offset->z = dat->mOffset.z;
    mDoMtx_YrotS(mDoMtx_stack_c::get(), current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), offset, offset);
    mDoMtx_stack_c::transS(current.pos.x + offset->x, current.pos.y + offset->y, current.pos.z + offset->z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), current.angle.x, 0, current.angle.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}
VERIFY(0x022E54E8, &daNpcSv_c::setMtx);

/* 022E5638 */
void daNpcSv_c::setCollision(dCcD_Cyl* cyl, cXyz* pos, f32 r, f32 h) {
    WWHD_FUNC(0x022E5638, void, this, cyl, pos, r, h);
    cyl->SetC(pos);
    cyl->SetR(r);
    cyl->SetH(h);
    dComIfG_Ccsp_Set(cyl);
}
VERIFY(0x022E5638, &daNpcSv_c::setCollision);

/* 022E56C8 */
s32 daNpcSv_c::createInit() {
    WWHD_FUNC(0x022E56C8, s32, this);
    gravity = -9.0f;
    setAnmTbl(l_anm_wait);
    mEventIdx[0] = dComIfGp_evmng_getEventIdx(STR(0x10022560) /* "SV_TALK_P1_1ST" */, 0xFF);
    mEventIdx[1] = dComIfGp_evmng_getEventIdx(STR(0x10022570) /* "SV_TALK_P1_2ND" */, 0xFF);
    mEventIdx[2] = dComIfGp_evmng_getEventIdx(STR(0x10022580) /* "SV_TALK_P4_1ST" */, 0xFF);
    u8 talkNo = getTalkNo();
    gabi::store<s16>(gabi::ea(this) + 0xFC, mEventIdx[l_talk_ev_tbl(talkNo)]); /* eventInfo.setEventId */
    setActorInfo2(&mEventCut, l_staff_name(mNpcNo), this);
    mLookVel = 0;
    mAttn = 0;
    mTalk = 0;
    m859 = 0;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel()));
    fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xB1); /* attention_info.distances[1] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xB1); /* attention_info.distances[3] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    sSvNpcDat_l* dat = l_npc_dat(mNpcNo);
    m_jnt.setParam(dat->mMaxBackboneX, dat->mMaxBackboneY, dat->mMinBackboneX, dat->mMinBackboneY, dat->mMaxHeadX,
                   dat->mMaxHeadY, dat->mMinHeadX, dat->mMinHeadY, dat->mMaxTurnStep);
    dat = l_npc_dat(mNpcNo);
    m857 = dat->m42;
    m858 = dat->m43;
    mTalkDist = dat->mTalkDist;
    mLookAngle = dat->mLookAngle;
    setMtx();
    J3DModel_calc(mpMorf->getModel());
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    dat = l_npc_dat(mNpcNo);
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    mCyl.SetStts(&mStts);
    pos->z = current.pos.z;
    setCollision(&mCyl, pos, dat->mCylR, dat->mCylH);
    return 4; /* cPhs_COMPLEATE_e */
}
VERIFY(0x022E56C8, &daNpcSv_c::createInit);

/* 022E5910 */
static cPhs_State phase_2(daNpcSv_c* i_this) {
    WWHD_FUNC(0x022E5910, cPhs_State, i_this);
    cPhs_State ret = dComIfG_resLoad(&i_this->mPhs, l_arcname(i_this->mNpcNo));
    if (ret == 4 /* cPhs_COMPLEATE_e */) {
        if (!fopAcM_entrySolidHeap(i_this, 0x022E50AC /* CheckCreateHeap */, 0x3800)) {
            i_this->mpMorf = nullptr;
            return 5; /* cPhs_ERROR_e */
        }
        ret = i_this->createInit();
    }
    return ret;
}
VERIFY(0x022E5910, phase_2);

/* 022E59A4 */
cPhs_State daNpcSv_c::_create() {
    WWHD_FUNC(0x022E59A4, cPhs_State, this);
    return dComLbG_PhaseHandler(&mPhsHandler, l_method, this);
}
VERIFY(0x022E59A4, &daNpcSv_c::_create);

/* 022E59B8 */
static cPhs_State daNpc_PeopleCreate(void* i_this) {
    WWHD_FUNC(0x022E59B8, cPhs_State, i_this);
    return ((daNpcSv_c*)i_this)->_create();
}
VERIFY(0x022E59B8, daNpc_PeopleCreate);

/* 022E59BC */
BOOL daNpcSv_c::_delete() {
    WWHD_FUNC(0x022E59BC, BOOL, this);
    dComIfG_resDelete(&mPhs, l_arcname(mNpcNo));
    if (heap != nullptr && mpMorf != nullptr) {
        mpMorf->stopZelAnime();
    }
    return TRUE;
}
VERIFY(0x022E59BC, &daNpcSv_c::_delete);

/* 022E5A20 */
static BOOL daNpc_PeopleDelete(void* i_this) {
    WWHD_FUNC(0x022E5A20, BOOL, i_this);
    return ((daNpcSv_c*)i_this)->_delete();
}
VERIFY(0x022E5A20, daNpc_PeopleDelete);

/* 022E5A24 */
void daNpcSv_c::chkAttention() {
    WWHD_FUNC(0x022E5A24, void, this);
    m859 = 0;
    if (mEventCut.mbAttention != 0) {
        mLookPos.y = mEventCut.mPos.y;
        mLookPos.z = mEventCut.mPos.z;
        mLookMode = 1;
        mLookPos.x = mEventCut.mPos.x;
        if (m857 != 0) {
            mHeadOnly = 0;
            m_jnt.mbTrn = 1;
        } else {
            mHeadOnly = 1;
        }
        if (mAttn == 0) {
            mAttn = 1;
        }
        mLookSpeed = l_npc_dat(mNpcNo)->mLookSpeed;
        return;
    }

    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    gabi::Local<cXyz> plPos;
    plPos->x = player->current.pos.x;
    plPos->y = player->current.pos.y;
    f32 talkDist = mTalkDist;
    s32 lookAngle = mLookAngle;
    plPos->z = player->current.pos.z;
    gabi::Local<be<f32>> dist;
    gabi::Local<be<s16>> angY;
    dNpc_calc_DisXZ_AngY(pos, plPos, dist, angY);
    u8 attn = mAttn;
    s16 ang = *angY;
    s16 shapeY = shape_angle.y;
    if (attn != 0) {
        talkDist += 100.0f;
        lookAngle += 0x71C;
    }
    s16 diff = (s16)(ang - shapeY);
    *angY = diff;
    f32 d = *dist;
    s32 absDiff = diff < 0 ? -diff : diff;
    if (talkDist > d && lookAngle > absDiff) {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, l_npc_dat(mNpcNo)->mEyeOffset);
        mLookPos.copy(*eye);
        mLookMode = 1;
        mHeadOnly = m857 == 0;
        u8 attn2 = mAttn;
        if (m858 == 0) {
            mHeadOnly = 0;
            mLookAngleY = mHomeAngleY;
            mLookMode = 2;
            m_jnt.mbTrn = 1;
        }
        if (attn2 == 0) {
            mAttn = 1;
        }
        m859 = 1;
        mLookSpeed = l_npc_dat(mNpcNo)->mLookSpeed;
        return;
    }

    if (attn == 1) {
        mAttn = 0;
        mLookTimer = l_npc_dat(mNpcNo)->mLookTimer;
    }
    sSvNpcDat_l* dat = l_npc_dat(mNpcNo);
    if (dat->mLookDist > d) {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, dat->mEyeOffset);
        mLookPos.copy(*eye);
        mLookMode = 1;
        mHeadOnly = m857 == 0;
        if (m858 == 0) {
            mHeadOnly = 0;
            mLookAngleY = mHomeAngleY;
            mLookMode = 2;
            m_jnt.mbTrn = 1;
        }
        m859 = 1;
        mLookSpeed = l_npc_dat(mNpcNo)->mLookSpeed;
        return;
    }

    u32 keep = m7EC;
    mLookMode = 0;
    if (keep == 0) {
        if (mLookTimer != 0) {
            mLookTimer = mLookTimer - 1;
        } else {
            mHeadOnly = 0;
            mLookAngleY = mHomeAngleY;
            mLookMode = 2;
            m_jnt.mbTrn = 1;
        }
    }
    mLookSpeed = l_npc_dat(mNpcNo)->mLookSpeed;
}
VERIFY(0x022E5A24, &daNpcSv_c::chkAttention);

/* 022E5D94 */
void daNpcSv_c::checkOrder() {
    WWHD_FUNC(0x022E5D94, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd == 2 /* dEvtCmd_INDEMO_e */) {
        return;
    }
    if (cmd == 1 /* dEvtCmd_INTALK_e */) {
        if (mOrder == 2 || mOrder == 1) {
            mTalk = 1;
        }
    }
}
VERIFY(0x022E5D94, &daNpcSv_c::checkOrder);

/* 022E5DC8 */
void daNpcSv_c::executeSetMode(int mode) {
    WWHD_FUNC(0x022E5DC8, void, this, mode);
    mMode = (u8)ptmf_call_r(l_init_tbl + mode * 8, this);
}
VERIFY(0x022E5DC8, &daNpcSv_c::executeSetMode);

/* 022E5E48 */
void daNpcSv_c::setMessage(u32 msgNo) {
    WWHD_FUNC(0x022E5E48, void, this, msgNo);
    mCurrMsgNo = msgNo;
}
VERIFY(0x022E5E48, &daNpcSv_c::setMessage);

/* 022E5E50 */
void daNpcSv_c::eventMesSetInit(int staffId) {
    WWHD_FUNC(0x022E5E50, void, this, staffId);
    be<u32>* pMsgNo = (be<u32>*)dComIfGp_evmng_getMySubstanceP(staffId, STR(0x10022594) /* "MsgNo" */, 3 /* integer */);
    if (pMsgNo != nullptr) {
        mpMsgTbl = 0;
        if (*pMsgNo == 0) {
            setMessage(vcall_getMsg(this));
        } else {
            setMessage(*pMsgNo);
        }
    } else {
        mpMsgTbl = mpMsgTbl + 4;
        setMessage(gabi::load<u32>(mpMsgTbl));
    }
    dComIfGp_event_setTalkPartner(this);
}
VERIFY(0x022E5E50, &daNpcSv_c::eventMesSetInit);

/* 022E5F4C */
void daNpcSv_c::eventGetItemInit(int staffId) {
    WWHD_FUNC(0x022E5F4C, void, this, staffId);
    void* pItemNo = dComIfGp_evmng_getMySubstanceP(staffId, STR(0x1002259C) /* "ItemNo" */, 3 /* integer */);
    s8 roomNo = current.roomNo;
    s32 itemPid;
    if (pItemNo != nullptr) {
        itemPid = fopAcM_createItemForPresentDemo(&current.pos, gabi::load<s32>(0x101C63BC), 0, -1, roomNo, nullptr, nullptr);
    } else {
        itemPid = fopAcM_createItemForPresentDemo(&current.pos, mItemNo, 0, -1, roomNo, nullptr, nullptr);
    }
    if (itemPid != -1) {
        dComIfGp_event_setItemPartnerId(itemPid);
    }
}
VERIFY(0x022E5F4C, &daNpcSv_c::eventGetItemInit);

/* 022E600C */
void daNpcSv_c::eventSetAngleInit() {
    WWHD_FUNC(0x022E600C, void, this);
    gabi::Local<be<u32>> ship;
    fopAcM_SearchByID2(parentActorID, ship);
    if (*ship != 0) {
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        fopAc_ac_c* a = gabi::at<fopAc_ac_c>(*ship);
        gabi::Local<cXyz> shipPos;
        shipPos->x = a->current.pos.x;
        shipPos->y = a->current.pos.y;
        shipPos->z = a->current.pos.z;
        gabi::Local<cXyz> plPos;
        plPos->x = player->current.pos.x;
        plPos->y = player->current.pos.y;
        plPos->z = player->current.pos.z;
        gabi::Local<be<s16>> angY;
        dNpc_calc_DisXZ_AngY(shipPos, plPos, nullptr, angY);
        a = gabi::at<fopAc_ac_c>(*ship);
        s16 diff = (s16)(*angY - a->shape_angle.y);
        *angY = diff;
        if (diff > 0) {
            s16 y = a->shape_angle.y + 0x4000;
            mHomeAngleY = y;
            mEvtAngleY = y;
        } else {
            s16 y = a->shape_angle.y - 0x4000;
            mHomeAngleY = y;
            mEvtAngleY = y;
        }
        mEvtFlags |= 2;
    } else {
        mEvtAngleY = mHomeAngleY;
        mEvtFlags |= 2;
    }
}
VERIFY(0x022E600C, &daNpcSv_c::eventSetAngleInit);

/* 022E610C */
void daNpcSv_c::eventAttentionInit(int staffId) {
    WWHD_FUNC(0x022E610C, void, this, staffId);
    void* pTimer = dComIfGp_evmng_getMySubstanceP(staffId, STR(0x100225A4) /* "Timer" */, 3 /* integer */);
    s16 timer = 12;
    u8 no = mNpcNo;
    if (pTimer != nullptr) {
        timer = gabi::load<s16>(gabi::ea(pTimer) + 2); /* (s16)*(int*)pTimer */
    }
    mAttnTimer = timer;
    m85C = 0;
    if (no == 2) {
        dComIfGp_event_setTalkPartner(this);
    }
}
VERIFY(0x022E610C, &daNpcSv_c::eventAttentionInit);

/* 022E619C */
void daNpcSv_c::eventTurnOkInit() {
    WWHD_FUNC(0x022E619C, void, this);
    m857 = 1;
}
VERIFY(0x022E619C, &daNpcSv_c::eventTurnOkInit);

/* 022E61A8 */
u16 daNpcSv_c::talk2(int mode, fopAc_ac_c* actor) {
    WWHD_FUNC(0x022E61A8, u16, this, mode, actor);
    u32 mng = l_msgMng();
    u16 status = 0xFF; /* fopMsgStts_BOX_CLOSED_e */
    if (mCurrMsgBsPcId == 0xFFFFFFFF) {
        if (mode == 1) {
            mCurrMsgNo = vcall_getMsg(this);
        }
        mCurrMsgBsPcId = fopMsgM_messageSet(mng, mCurrMsgNo, &actor->eyePos);
        if (mCurrMsgBsPcId != 0xFFFFFFFF) {
            mbCurrMsg = 0;
        }
        return status;
    }
    if (mbCurrMsg == 0) {
        mbCurrMsg = 1;
        return status;
    }
    status = (u16)fopMsgM_SearchByID(mng);
    if (status == 0xE /* fopMsgStts_MSG_DISPLAYED_e */) {
        fopMsgM_setStatus(mng, vcall_next_msgStatus(this, &mCurrMsgNo));
        if (fopMsgM_SearchByID(mng) == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
            fopMsgM_messageSet(mng, mCurrMsgNo, nullptr);
        }
    } else if (status == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        fopMsgM_setStatus(mng, 0x13);
        mCurrMsgBsPcId = 0xFFFFFFFF;
    }
    vcall_anmAtr(this, status);
    return status;
}
VERIFY(0x022E61A8, &daNpcSv_c::talk2);

/* 022E6360 */
BOOL daNpcSv_c::eventMesSet() {
    WWHD_FUNC(0x022E6360, BOOL, this);
    return talk2(0, this) == 0x12;
}
VERIFY(0x022E6360, &daNpcSv_c::eventMesSet);

/* 022E6394 */
BOOL daNpcSv_c::eventAttention() {
    WWHD_FUNC(0x022E6394, BOOL, this);
    if (mAttnTimer != 0) {
        mAttnTimer = mAttnTimer - 1;
        if (mAttnTimer == 0) {
            fopAc_ac_c* sv0 = fopAcM_searchFromName(l_staff_name(0) /* "Sv0" */, 0, 0);
            if (sv0 != nullptr) {
                u8 a_ptn = static_cast<daNpcSv_c*>(sv0)->mMsgAnm;
                if (a_ptn >= 6) /* JUT_ASSERT(0x6BE, a_ptn < ARRAY_SIZE(l_npc_anm_sv1_tbl)) */
                    JUT_ASSERT_fail(STR(0x100225AC), 0x6BE, STR(0x100225BC));
                if (a_ptn < 6) {
                    setAnmTbl(l_npc_anm_sv1_tbl(a_ptn));
                }
                if (mNpcNo == 1) {
                    if (a_ptn >= 6) /* JUT_ASSERT(0x6C4, a_ptn < ARRAY_SIZE(l_npc_se_sv1_tbl)) */
                        JUT_ASSERT_fail(STR(0x100225AC), 0x6C4, STR(0x10022600));
                    if (a_ptn < 6) {
                        mDoAud_seStart_1(l_npc_se_sv1_tbl(a_ptn));
                    }
                }
            } else {
                setAnmTbl(l_npc_anm_sv1_tbl(2));
                mDoAud_seStart_1(l_npc_se_sv1_tbl(2));
            }
        }
        return FALSE;
    }
    if (mAnmFlags & 1) {
        setAnmTbl(l_npc_anm_sv1_tbl(0));
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022E6394, &daNpcSv_c::eventAttention);

/* 022E64C8 */
void daNpcSv_c::privateCut() {
    WWHD_FUNC(0x022E64C8, void, this);
    const char* name = l_staff_name(mNpcNo);
    s32 staffId = dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
    if (staffId != -1) {
        s8 actIdx = (s8)dComIfGp_evmng_getMyActIdx(staffId, l_cut_name_tbl, 5, TRUE, 0);
        mActIdx = actIdx;
        dEvent_manager_c* evtMng = dComIfGp_getPEvtManager();
        if (actIdx == -1) {
            gabi::call(0x02543280, evtMng, staffId); /* cutEnd */
        } else {
            if (gabi::call<BOOL>(0x025447C8, evtMng, staffId) /* getIsAddvance */) {
                switch ((u32)(s32)mActIdx) {
                case 0: eventMesSetInit(staffId); break;
                case 1: eventGetItemInit(staffId); break;
                case 2: eventSetAngleInit(); break;
                case 3: eventAttentionInit(staffId); break;
                case 4: eventTurnOkInit(); break;
                }
            }
            switch ((u32)(s32)mActIdx) {
            case 0:
                if (eventMesSet()) {
                    dComIfGp_evmng_cutEnd(staffId);
                }
                break;
            case 3:
                if (eventAttention()) {
                    dComIfGp_evmng_cutEnd(staffId);
                }
                break;
            default:
                dComIfGp_evmng_cutEnd(staffId);
                break;
            }
        }
    }
    if (mEvtFlags & 2) {
        mLookMode = 2;
        mLookAngleY = mEvtAngleY;
        mHeadOnly = 0;
        m_jnt.mbTrn = 1;
    }
}
VERIFY(0x022E64C8, &daNpcSv_c::privateCut);

/* 022E66A4 */
void daNpcSv_c::setAnmFromMsgTag() {
    WWHD_FUNC(0x022E66A4, void, this);
    u8 attr = dComIfGp_getMesgAnimeAttrInfo();
    if (attr <= 5) {
        setAnmTbl(l_msg_anm_tbl(attr));
    }
    dComIfGp_clearMesgAnimeAttrInfo();
}
VERIFY(0x022E66A4, &daNpcSv_c::setAnmFromMsgTag);

/* 022E6700 */
void daNpcSv_c::eventMove() {
    WWHD_FUNC(0x022E6700, void, this);
    if (dComIfGp_evmng_endCheck(mEventIdx[0]) || dComIfGp_evmng_endCheck(mEventIdx[1]) || dComIfGp_evmng_endCheck(mEventIdx[2])) {
        u8 talkNo = getTalkNo();
        gabi::store<s16>(gabi::ea(this) + 0xFC, mEventIdx[l_talk_ev_tbl(talkNo)]); /* eventInfo.setEventId */
        dComIfGp_event_reset();
        m85D = 0;
        mEvtFlags &= ~2;
        mTalk = 0;
        executeSetMode(0);
        return;
    }
    u8 attn = mEventCut.mbAttention;
    if (mEventCut.cutProc()) {
        if (mEventCut.mbAttention == 0) {
            mEventCut.mbAttention = attn;
        }
    } else {
        privateCut();
        if (mNpcNo == 0) {
            setAnmFromMsgTag();
        }
    }
}
VERIFY(0x022E6700, &daNpcSv_c::eventMove);

/* 022E685C */
BOOL daNpcSv_c::isTalkOK() {
    WWHD_FUNC(0x022E685C, BOOL, this);
    /* !daPy_getPlayerLinkActorClass()->checkPlayerFly-like status bit (play + 0x5CD8, 0x100000) */
    return (gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & 0x100000) == 0;
}
VERIFY(0x022E685C, &daNpcSv_c::isTalkOK);

/* 022E688C */
void daNpcSv_c::eventOrder() {
    WWHD_FUNC(0x022E688C, void, this);
    if (mOrder == 2 || mOrder == 1) {
        if (isTalkOK()) {
            eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
            if (mOrder == 2) {
                fopAcM_orderSpeakEvent(this);
            }
        }
    }
}
VERIFY(0x022E688C, &daNpcSv_c::eventOrder);

/* 022E68F8 */
void daNpcSv_c::playAnm() {
    WWHD_FUNC(0x022E68F8, void, this);
    mDoExt_McaMorf* morf = mpMorf;
    mAnmFlags &= ~1;
    if (morf->play(nullptr, 0, 0)) {
        if (mpAnmTbl != nullptr && mAnmLoop > 0) {
            mAnmLoop = mAnmLoop - 1;
            if (mAnmLoop == 0) {
                mpAnmTbl = gabi::at<sSvAnmDat>(gabi::ea(mpAnmTbl.get()) + 3);
                if (setAnmTbl(gabi::at<sSvAnmDat>(gabi::ea(mpAnmTbl.get())))) {
                    mAnmFlags |= 1;
                }
            } else {
                setAnm(mpAnmTbl->mAnm, 0, 0.0f);
            }
        }
    }
}
VERIFY(0x022E68F8, &daNpcSv_c::playAnm);

/* 022E69C8 */
void daNpcSv_c::lookBack() {
    WWHD_FUNC(0x022E69C8, void, this);
    s8 mode = mLookMode;
    f32 eyeX = eyePos.x;
    s16 defY = current.angle.y;
    u8 headOnly = mHeadOnly;
    s16 vel = mLookSpeed;
    f32 eyeZ = eyePos.z;
    cXyz* target = nullptr;
    f32 eyeY = eyePos.y;
    gabi::Local<cXyz> dst;
    bool turn;
    if (mode == 1) {
        dst->copy(mLookPos);
        target = dst;
        turn = mTalk != 0;
    } else if (mode == 2) {
        turn = mTalk != 0;
        defY = mLookAngleY;
    } else {
        turn = mTalk != 0;
    }
    bool calc;
    if (turn && m857 != 0) {
        m_jnt.mbTrn = 1;
        headOnly = 0;
        calc = true;
    } else {
        calc = m_jnt.mbTrn != 0;
    }
    gabi::Local<cXyz> eye;
    if (!calc) {
        mLookVel = 0;
        eye->x = eyeX;
        eye->y = eyeY;
        eye->z = eyeZ;
        lookAtTarget(&m_jnt, &current.angle.y, target, eye, defY, 0, headOnly);
    } else {
        s16 turnSpeed = mEventCut.mTurnSpeed;
        if (turnSpeed != 0) {
            vel = turnSpeed;
        }
        cLib_addCalcAngleS2(&mLookVel, vel, 4, 0x800);
        s16 lookVel = mLookVel;
        eye->x = eyeX;
        eye->y = eyeY;
        eye->z = eyeZ;
        lookAtTarget(&m_jnt, &current.angle.y, target, eye, defY, lookVel, headOnly);
    }
    shape_angle.x = current.angle.x;
    shape_angle.z = current.angle.z;
    shape_angle.y = current.angle.y;
}
VERIFY(0x022E69C8, &daNpcSv_c::lookBack);

/* 022E6BD4 */
BOOL daNpcSv_c::_execute() {
    WWHD_FUNC(0x022E6BD4, BOOL, this);
    gabi::Local<be<u32>> ship;
    fopAcM_SearchByID2(parentActorID, ship);
    chkAttention();
    checkOrder();
    if (!dComIfGp_event_runCheck()) {
        ptmf_call_r(l_exec_tbl + mMode * 8, this);
        eventOrder();
    } else {
        eventMove();
        eventOrder();
    }
    if (mNpcNo == 0 && mAnm == 5) {
        mpMorf->setFrame((f32)mFrame);
    } else {
        playAnm();
    }
    if (*ship != 0) {
        mFrame = gabi::load<s32>(*ship + 0x1438);
    }
    mObjAcch.CrrPos(dComIfG_Bgsp());
    sSvNpcDat_l* dat = l_npc_dat(mNpcNo);
    gabi::Local<cXyz> pos;
    pos->y = current.pos.y;
    pos->x = current.pos.x;
    pos->z = current.pos.z;
    setCollision(&mCyl, pos, dat->mCylR, dat->mCylH);
    dat = l_npc_dat(mNpcNo);
    f32 z = current.pos.z;
    f32 y = current.pos.y;
    f32 x = current.pos.x;
    cXyz* attnPos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attnPos->x = x;
    attnPos->y = y + dat->mAttnY;
    attnPos->z = z;
    eyePos.x = x;
    eyePos.y = y + dat->mEyeY;
    eyePos.z = z;
    lookBack();
    setMtx();
    return FALSE;
}
VERIFY(0x022E6BD4, &daNpcSv_c::_execute);

/* 022E6E00 */
static BOOL daNpc_PeopleExecute(void* i_this) {
    WWHD_FUNC(0x022E6E00, BOOL, i_this);
    return ((daNpcSv_c*)i_this)->_execute();
}
VERIFY(0x022E6E00, daNpc_PeopleExecute);

/* 022E6E04 */
BOOL daNpcSv_c::_draw() {
    WWHD_FUNC(0x022E6E04, BOOL, this);
    /* HD: no shadow */
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    mDoExt_McaMorf* morf = mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &tevStr);
    mpMorf->updateDL();
    return TRUE;
}
VERIFY(0x022E6E04, &daNpcSv_c::_draw);

/* 022E6E68 */
static BOOL daNpc_PeopleDraw(void* i_this) {
    WWHD_FUNC(0x022E6E68, BOOL, i_this);
    return ((daNpcSv_c*)i_this)->_draw();
}
VERIFY(0x022E6E68, daNpc_PeopleDraw);

/* 022E6E6C */
s32 daNpcSv_c::executeCommon() {
    WWHD_FUNC(0x022E6E6C, s32, this);
    mOrder = mAttn != 0;
    return 0;
}
VERIFY(0x022E6E6C, &daNpcSv_c::executeCommon);

/* 022E6E84 */
s32 daNpcSv_c::executeWaitInit() {
    WWHD_FUNC(0x022E6E84, s32, this);
    setAnmTbl(l_anm_wait);
    sSvNpcDat_l* dat = l_npc_dat(mNpcNo);
    m_jnt.setParam(dat->mMaxBackboneX, dat->mMaxBackboneY, dat->mMinBackboneX, dat->mMinBackboneY, dat->mMaxHeadX,
                   dat->mMaxHeadY, dat->mMinHeadX, dat->mMinHeadY, dat->mMaxTurnStep);
    return 0;
}
VERIFY(0x022E6E84, &daNpcSv_c::executeWaitInit);

/* 022E6F04 */
void daNpcSv_c::executeWait() {
    WWHD_FUNC(0x022E6F04, void, this);
    gabi::Local<be<u32>> shipId;
    fopAcM_SearchByID2(parentActorID, shipId);
    if (*shipId == 0) {
        return;
    }
    fopAc_ac_c* ship = gabi::at<fopAc_ac_c>(*shipId);
    if (mNpcNo == 0) {
        if (m859 != 0) {
            mOrder = 1;
            if (m859 != m85E) {
                m85D = 1;
            }
            gabi::store<u8>(gabi::ea(ship) + 0x14C2, 1);
        } else {
            mOrder = 0;
            m85D = 0;
            gabi::store<u8>(gabi::ea(ship) + 0x14C2, 0);
        }
        if (m85D == 0) {
            ship = gabi::at<fopAc_ac_c>(*shipId);
            mLookAngleY = ship->shape_angle.y;
            current.angle.x = ship->shape_angle.x;
            mHeadOnly = 0;
            current.angle.z = ship->shape_angle.z;
            m_jnt.mbTrn = 1;
            mLookMode = 2;
            setAnmTbl(l_anm_ship);
        } else {
            setAnmTbl(l_anm_wait);
        }
        m85E = m859;
        return;
    }

    m857 = 1;
    s32 shipMode = gabi::load<s32>(gabi::ea(ship) + 0x40C);
    if (shipMode == 8 || shipMode == 9 || shipMode == 5) {
        mHeadOnly = 0;
        mLookAngleY = ship->shape_angle.y;
        m_jnt.mbTrn = 1;
        mLookMode = 2;
    } else if (shipMode == 4 || shipMode == 6) {
        s16 y = ship->shape_angle.y;
        if (gabi::load<u32>(gabi::ea(ship) + 0x14C4) == 0) {
            mHeadOnly = 0;
            mLookAngleY = y + 0x4000;
        } else {
            mHeadOnly = 0;
            mLookAngleY = y - 0x4000;
        }
        m_jnt.mbTrn = 1;
        mLookMode = 2;
    } else {
        s16 target;
        if (gabi::load<u32>(gabi::ea(ship) + 0x14C4) == 0) {
            target = (s16)(ship->shape_angle.y + 0x4000);
        } else {
            target = (s16)(ship->shape_angle.y - 0x4000);
        }
        if (current.angle.y == target) {
            cXyz* p = gabi::at<cXyz>(gabi::ea(ship) + 0x590);
            f32 x = p->x;
            f32 y = p->y;
            f32 z = p->z;
            mHeadOnly = 1;
            mLookPos.y = y;
            mLookPos.x = x;
            mLookPos.z = z;
            mLookMode = 1;
            m857 = 0;
        } else {
            mLookAngleY = target;
            mHeadOnly = 0;
            m_jnt.mbTrn = 1;
            mLookMode = 2;
        }
    }
    if (gabi::load<s32>(gabi::ea(ship) + 0x40C) == 7) {
        if (m85C == 0) {
            m85C = 1;
            setAnmTbl(l_npc_anm_sv1_tbl(1));
        }
    } else if (m85C == 1) {
        m85C = 0;
        setAnmTbl(l_npc_anm_sv1_tbl(0));
    }
}
VERIFY(0x022E6F04, &daNpcSv_c::executeWait);

/* 022E7214 */
u8 daNpcSv_c::executeTalkInit() {
    WWHD_FUNC(0x022E7214, u8, this);
    if (m856 != 0) {
        return 1;
    }
    return mMode;
}
VERIFY(0x022E7214, &daNpcSv_c::executeTalkInit);

/* 022E7230 */
void daNpcSv_c::executeTalk() {
    WWHD_FUNC(0x022E7230, void, this);
    executeCommon();
    if (talk2(1, this) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mTalk = 0;
        m85D = 0;
        executeSetMode(0);
        dComIfGp_event_reset();
    } else {
        setAnmFromMsgTag();
    }
}
VERIFY(0x022E7230, &daNpcSv_c::executeTalk);

/* 022E72B0 */
u16 daNpcSv_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022E72B0, u16, this, pMsgNo);
    if (*pMsgNo == 0) {
        return 0x10; /* fopMsgStts_MSG_ENDS_e */
    }
    if (mpMsgTbl == 0) {
        return 0x10;
    }
    mpMsgTbl = mpMsgTbl + 4;
    u32 next = gabi::load<u32>(mpMsgTbl);
    switch (next) {
    case 0:
        mpMsgTbl = 0;
        return 0x10;
    case 1:
        return 0x10;
    case 2:
    case 3:
    case 4:
    case 5:
        mMsgAnm = (u8)next;
        return 0x10;
    default:
        *pMsgNo = next;
        return 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    }
}
VERIFY(0x022E72B0, &daNpcSv_c::next_msgStatus);

/* 022E7364 */
u32 daNpcSv_c::getMsg() {
    WWHD_FUNC(0x022E7364, u32, this);
    u32 msgNo = 0;
    mpMsgTbl = 0;
    if (dComIfGp_event_chkTalkXY()) {
        dComIfGp_get(); /* HD: a play accessor whose result is unused */
    } else {
        u8 talkNo = mTalkNo;
        u16 flags = mFlags;
        mpMsgTbl = l_msg_tbl(talkNo);
        flags |= l_msg_flag_tbl(talkNo);
        mFlags = flags;
        if (flags & 0x10) {
            dComIfGs_onEventBit(0x2F80);
        }
    }
    if (mpMsgTbl != 0) {
        msgNo = gabi::load<u32>(mpMsgTbl);
        if (msgNo != 0) {
            return msgNo;
        }
    }
    setAnmTbl(l_anm_end);
    return msgNo;
}
VERIFY(0x022E7364, &daNpcSv_c::getMsg);

/* 022E7438 */
static void __sinit_d_a_npc_sv_cpp() {
    WWHD_FUNC(0x022E7438, void, (u32)0);
    sinit_header_statics(0x10468794, 0x101C6798);
}
VERIFY(0x022E7438, __sinit_d_a_npc_sv_cpp);

/* 022E74CC: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x022E74CC, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x022E74CC, SafeString_dt);

/* 022E74E0 */
static BOOL daNpc_PeopleIsDelete(void*) {
    WWHD_FUNC(0x022E74E0, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x022E74E0, daNpc_PeopleIsDelete);

/* 022E74E8: daNpcSv_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpcSv_dt(daNpcSv_c* p, s32 flags) {
    WWHD_FUNC(0x022E74E8, void, p, flags);
    if (p != nullptr) {
        dCcD_Cyl_dt(&p->mCyl, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mAcchCir) + 0x14), 2); /* ~cM3dGCir */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x20, 0x10022448);            /* ~dBgS_ObjAcch */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x14, 0x10022458);
        gabi::call(0x024EFD9C, &p->mObjAcch, 0);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x022E74E8, daNpcSv_dt);

/* 022E7584: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x022E7584, void, (u32)0);
}
VERIFY(0x022E7584, SafeString_assureTerminationImpl);
