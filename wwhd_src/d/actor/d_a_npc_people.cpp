/**
 * d_a_npc_people.cpp (WWHD)
 * NPC - Windfall townspeople (Uo, Ub, Uw, Um, Sa, Ug)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_people.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * This part: construction, heap, create phases, draw-side helpers and the small accessors.
 * The other parts: d_a_npc_people_*.cpp.
 */
#include "d/actor/d_a_npc_people.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 022C8860 daObj::PrmAbstract<daNpcPeople_c::Prm_e>(actor, width, shift) (out of line in this TU) */
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift);
/* 02525FE4 dComLbG_PhaseHandler(phase, table, this) */
static inline cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class* p, u32 tbl, void* self) {
    return gabi::call<cPhs_State>(0x02525FE4, p, tbl, self);
}
/* 025A1458 fopNpc_npc_c::fopNpc_npc_c, 025E7820 mDoExt_btpAnm::mDoExt_btpAnm */
static inline void fopNpc_npc_c_ct(void* p) { gabi::call(0x025A1458, p); }
static inline void mDoExt_btpAnm_ct(void* p) { gabi::call(0x025E7820, p); }
/* 0259E778 dNpc_PathRun_c::getPoint(u8): cXyz through a hidden result pointer (r4) */
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_l* r, cXyz* out, u8 idx) { gabi::call(0x0259E778, r, out, idx); }
static inline BOOL dNpc_PathRun_incIdxLoop(dNpc_PathRun_l* r) { return gabi::call<BOOL>(0x0259EB60, r); }
/* 02520C0C dComIfGs_checkGetItem(u8) */
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
static inline u8 dComIfGp_getPictureResult() { return gabi::load<u8>(dComIfGp_ea() + 0x5BE6); }

/* 022BE034 */
static BOOL daNpc_People_nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022BE034, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        u32 model = gabi::load<u32>(0x104B462C); /* j3dSys.getModel() */
        daNpcPeople_c* i_this = gabi::at<daNpcPeople_c>(gabi::load<u32>(model + 0xB8)); /* getUserArea() */
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(model + 0x2C));
        blk->mFlags |= 0x10;
        PSMTXCopy(gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30), calc_mtx());
        if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
            mDoMtx_ZrotM(calc_mtx(), -i_this->m_jnt.mAngles[0][0]);
        }
        if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            mDoMtx_ZrotM(calc_mtx(), -i_this->m_jnt.mAngles[1][0]);
        }
        /* model->setAnmMtx(jntNo, *calc_mtx) */
        blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(model + 0x2C));
        blk->mFlags |= 0x10;
        mtx_copy(gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30), calc_mtx());
        PSMTXCopy(calc_mtx(), j3dSys_mCurrentMtx());
    }
    return TRUE;
}
VERIFY(0x022BE034, daNpc_People_nodeCallBack);

/* 022BE18C */
int daNpcPeople_c::getBck(int param_1) {
    WWHD_FUNC(0x022BE18C, int, this, param_1);
    u32 tbl = gabi::load<u32>(PEOPLE_l_bck_ix_tbl + (mNpcNo * 2 + mbIsNight) * 4);
    return gabi::load<s32>(tbl + param_1 * 4);
}
VERIFY(0x022BE18C, &daNpcPeople_c::getBck);

/* 022BE1B4 */
int daNpcPeople_c::getHeadBck(int param_1) {
    WWHD_FUNC(0x022BE1B4, int, this, param_1);
    u32 tbl = gabi::load<u32>(PEOPLE_l_head_bck_ix_tbl + (mNpcNo * 2 + mbIsNight) * 4);
    if (tbl == 0) {
        return -1;
    }
    return gabi::load<s32>(tbl + param_1 * 4);
}
VERIFY(0x022BE1B4, &daNpcPeople_c::getHeadBck);

/* 022BE1E8 */
BOOL daNpcPeople_c::initTexPatternAnm(u32 param_1) {
    WWHD_FUNC(0x022BE1E8, BOOL, this, param_1);
    m_head_tex_pattern = nullptr;
    s32 btp = gabi::load<s32>(0x10020204 + mNpcNo * 4); /* l_btp_ix_tbl */
    if (btp >= 0) {
        J3DModelData* modelData;
        if (mpHeadModel.get() != nullptr) {
            modelData = J3DModel_getModelData_l(mpHeadModel);
        } else {
            modelData = J3DModel_getModelData_l(mpHeadMorf->getModel());
        }
        const char* arc = people_arcname(mNpcNo);
        m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes_r67(arc, btp, gabi::ea(arc), PEOPLE_l_arcname_tbl);
        if (m_head_tex_pattern.get() == nullptr) {
            JUT_ASSERT_fail(STR(0x10020354), 0x240D, STR(0x10020338));
        }
        if (!mDoExt_btpAnm_init(mBtpAnm, modelData, m_head_tex_pattern, TRUE, 2 /* EMode_LOOP */, 1.0f, 0, -1, param_1, FALSE)) {
            return false;
        }
    }
    m78E = 0;
    m780 = 0;
    return true;
}
VERIFY(0x022BE1E8, &daNpcPeople_c::initTexPatternAnm);

/* 022BE780 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022BE780, BOOL, i_this);
    return static_cast<daNpcPeople_c*>(i_this)->createHeap();
}
VERIFY(0x022BE780, CheckCreateHeap);

/* 022BE784 */
u8 daNpcPeople_c::getPrmArg0() {
    WWHD_FUNC(0x022BE784, u8, this);
    return daObj_PrmAbstract(this, PRM_ARG0_W, PRM_ARG0_S);
}
VERIFY(0x022BE784, &daNpcPeople_c::getPrmArg0);

/* 022BE7B0 */
u8 daNpcPeople_c::getPrmNpcNo() {
    WWHD_FUNC(0x022BE7B0, u8, this);
    if (0 <= argument && argument < 0x13) {
        return argument;
    }
    return 0;
}
VERIFY(0x022BE7B0, &daNpcPeople_c::getPrmNpcNo);

/* 022BE7CC daNpcPeople_c::daNpcPeople_c (HD: allocates when this == NULL) */
static daNpcPeople_c* daNpcPeople_c_ct(daNpcPeople_c* i_this) {
    WWHD_FUNC(0x022BE7CC, daNpcPeople_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpcPeople_c*)operator_new(0x920);
        if (i_this == nullptr) {
            return i_this;
        }
    }
    fopNpc_npc_c_ct(i_this);
    i_this->__vtbl = PEOPLE_VTBL;
    mDoExt_btpAnm_ct(i_this->mBtpAnm);
    i_this->mNpcNo = i_this->getPrmNpcNo();
    i_this->mResFlag = 0;
    i_this->m78F = 0;
    i_this->m740 = 0.0f;
    i_this->m76E = 0;
    i_this->m744 = -1.0f;
    i_this->m799 = 0;
    i_this->m764 = true;
    i_this->m79C = 1;
    i_this->m738 = nullptr;
    i_this->m77A = i_this->home.angle.y;
    i_this->m793 = 0;
    i_this->m760 = 0;
    i_this->m794 = 0;
    i_this->mEtcFlag = 0;
    i_this->m7A1 = 0;
    i_this->mbIsNight = dKy_daynight_check() & 1;
    i_this->mpNpcDat = gabi::at<daNpcPeople_c__l_npc_dat>(
        gabi::load<u32>(PEOPLE_l_npc_dat + (i_this->mNpcNo * 2 + i_this->mbIsNight) * 4));
    i_this->m730 = nullptr;
    return i_this;
}
VERIFY(0x022BE7CC, daNpcPeople_c_ct);

/* 022BEB50 */
u8 daNpcPeople_c::getPrmRailID() {
    WWHD_FUNC(0x022BEB50, u8, this);
    return daObj_PrmAbstract(this, PRM_RAIL_ID_W, PRM_RAIL_ID_S);
}
VERIFY(0x022BEB50, &daNpcPeople_c::getPrmRailID);

/* 022BEFF4 */
BOOL daNpcPeople_c::isPhoto(u8 param_1) {
    WWHD_FUNC(0x022BEFF4, BOOL, this, param_1);
    /* HD: dComIfGs_getPictureNum() is a byte of the picture save data (02720144 returns it) */
    if ((param_1 == 0x23 || param_1 == 0x26) &&
        gabi::load<u8>(gabi::call<u32>(0x02720144, dComIfGs_save() + 0x12C0) + 0x3C030C) != 0) {
        return true;
    }
    return false;
}
VERIFY(0x022BEFF4, &daNpcPeople_c::isPhoto);

/* 022BF21C */
static s16 daNpcPeople_XyCheckCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x022BF21C, s16, i_this, i_itemBtn);
    return static_cast<daNpcPeople_c*>(i_this)->XyCheckCB(i_itemBtn);
}
VERIFY(0x022BF21C, daNpcPeople_XyCheckCB);

/* 022BF300 */
static s16 daNpcPeople_XyEventCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x022BF300, s16, i_this, i_itemBtn);
    return static_cast<daNpcPeople_c*>(i_this)->XyEventCB(i_itemBtn);
}
VERIFY(0x022BF300, daNpcPeople_XyEventCB);

/* 022BF304 */
BOOL daNpcPeople_c::is1GetMap20() {
    WWHD_FUNC(0x022BF304, BOOL, this);
    return dComIfGs_checkGetItem(0xEB /* dItemNo_COLLECT_MAP_20_e */) ? TRUE : FALSE;
}
VERIFY(0x022BF304, &daNpcPeople_c::is1GetMap20);

/* 022BF330 */
s16 daNpcPeople_c::photoCB(int param_1) {
    WWHD_FUNC(0x022BF330, s16, this, param_1);
    s16 ret = -1;
    switch (mNpcNo) {
    case NPC_UB1:
    case NPC_UB2:
        if (dComIfGp_getPictureResult() == 5 && !is1GetMap20()) {
            ret = m766[3];
        } else {
            ret = m766[1];
        }
    }
    return ret;
}
VERIFY(0x022BF330, &daNpcPeople_c::photoCB);

/* 022BF39C */
static s16 daNpcPeople_photoCB(void* i_this, int param_1) {
    WWHD_FUNC(0x022BF39C, s16, i_this, param_1);
    return static_cast<daNpcPeople_c*>(i_this)->photoCB(param_1);
}
VERIFY(0x022BF39C, daNpcPeople_photoCB);

/* 022BF3A0 */
void daNpcPeople_c::setMtx() {
    WWHD_FUNC(0x022BF3A0, void, this);
    J3DModel* model = mpMorf->getModel();
    cXyz* base_scale = gabi::at<cXyz>(gabi::ea(model) + 0xBC); /* setBaseScale(scale) */
    f32 sx = scale.x, sy = scale.y, sz = scale.z;
    base_scale->x = sx;
    base_scale->y = sy;
    base_scale->z = sz;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}
VERIFY(0x022BF3A0, &daNpcPeople_c::setMtx);

/* 022BF480 */
void daNpcPeople_c::setCollision(dCcD_Cyl* cyl, cXyz* center, f32 radius, f32 height) {
    WWHD_FUNC(0x022BF480, void, this, cyl, center, radius, height);
    cyl->SetC(center);
    cyl->SetR(radius);
    cyl->SetH(height);
    dComIfG_Ccsp_Set(cyl);
}
VERIFY(0x022BF480, &daNpcPeople_c::setCollision);

/* 022BFE54 */
static cPhs_State phase_2(daNpcPeople_c* i_this) {
    WWHD_FUNC(0x022BFE54, cPhs_State, i_this);
    cPhs_State rt = dComIfG_resLoad(i_this->getPhaseP(), people_arcname(i_this->getNpcNo()));
    if (rt == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, 0x022BE780 /* CheckCreateHeap */, 0x3300)) {
            return i_this->createInit();
        }
        i_this->mpMorf = nullptr;
        return cPhs_ERROR_e;
    }
    return rt;
}
VERIFY(0x022BFE54, phase_2);

/* 022BFEE8 */
cPhs_State daNpcPeople_c::_create() {
    WWHD_FUNC(0x022BFEE8, cPhs_State, this);
    /* static cPhs__Handler l_method[] = {phase_1, phase_2, NULL} (.data 0x101C4C5C) */
    return dComLbG_PhaseHandler(&mPhase2, PEOPLE_l_method, this);
}
VERIFY(0x022BFEE8, &daNpcPeople_c::_create);

/* 022BFEFC */
static cPhs_State daNpc_PeopleCreate(void* i_this) {
    WWHD_FUNC(0x022BFEFC, cPhs_State, i_this);
    return static_cast<daNpcPeople_c*>(i_this)->_create();
}
VERIFY(0x022BFEFC, daNpc_PeopleCreate);

/* 022BFF00 */
bool daNpcPeople_c::_delete() {
    WWHD_FUNC(0x022BFF00, bool, this);
    dComIfG_resDelete(&mPhase, people_arcname(mNpcNo)); /* HD: dComIfG_resDeleteDemo is resDelete */
    if (heap.get() != nullptr) {
        if (mpMorf.get() != nullptr) {
            mpMorf->stopZelAnime();
        }
        if (mpHeadMorf.get() != nullptr) {
            mpHeadMorf->stopZelAnime();
        }
    }
    return true;
}
VERIFY(0x022BFF00, &daNpcPeople_c::_delete);

/* 022BFF74 */
static BOOL daNpc_PeopleDelete(void* i_this) {
    WWHD_FUNC(0x022BFF74, BOOL, i_this);
    return static_cast<daNpcPeople_c*>(i_this)->_delete();
}
VERIFY(0x022BFF74, daNpc_PeopleDelete);

/* 022BFF78 */
int daNpcPeople_c::getRand(int max) {
    WWHD_FUNC(0x022BFF78, int, this, max);
    int rnd = gabi::ftoi(cM_rndF((f32)max));
    if (rnd == max) {
        rnd = 0;
    }
    return rnd;
}
VERIFY(0x022BFF78, &daNpcPeople_c::getRand);

/* 022BFFE4 */
void daNpcPeople_c::warp() {
    WWHD_FUNC(0x022BFFE4, void, this);
    if (fopAcM_GetParam(this) & 0x80000000) {
        mParameters = fopAcM_GetParam(this) & ~0x80000000;
        if (mPathRun.mPath.get() != nullptr) {
            mPathRun.mbDir = true;
            mPathRun.mIdx = 0;
            gabi::Local<cXyz> pt;
            dNpc_PathRun_getPoint(&mPathRun, pt.get(), 0);
            old.pos.copy(*pt);
            current.pos.copy(old.pos);
            dNpc_PathRun_incIdxLoop(&mPathRun);
        }
        speedF = 0.0f;
        m740 = 0.0f;
    }
}
VERIFY(0x022BFFE4, &daNpcPeople_c::warp);

/* 022C1108 */
BOOL daNpcPeople_c::is1DayGetMap20() {
    WWHD_FUNC(0x022C1108, BOOL, this);
    return dComIfGs_getEventReg(0xC103 /* UNK_C103 */) == 2 ? TRUE : FALSE;
}
VERIFY(0x022C1108, &daNpcPeople_c::is1DayGetMap20);

/* 022C1890 (an out-of-line thunk: GHS's copy of eventMesSetPoInit, which is eventMesSetInit) */
void daNpcPeople_c::eventMesSetPoInit(int param_1) {
    WWHD_FUNC(0x022C1890, void, this, param_1);
    eventMesSetInit(param_1);
}
VERIFY(0x022C1890, &daNpcPeople_c::eventMesSetPoInit);

/* 022C36D0 */
static BOOL daNpc_PeopleExecute(void* i_this) {
    WWHD_FUNC(0x022C36D0, BOOL, i_this);
    return static_cast<daNpcPeople_c*>(i_this)->_execute();
}
VERIFY(0x022C36D0, daNpc_PeopleExecute);

/* 022C39EC */
static BOOL daNpc_PeopleDraw(void* i_this) {
    WWHD_FUNC(0x022C39EC, BOOL, i_this);
    return static_cast<daNpcPeople_c*>(i_this)->_draw();
}
VERIFY(0x022C39EC, daNpc_PeopleDraw);

/* 022C6BDC */
s16 daNpcPeople_c::getPigTimer() {
    WWHD_FUNC(0x022C6BDC, s16, this);
    return gabi::load<s16>(dComIfGp_ea() + 0x5BAA); /* dComIfGp_getItemTimer() */
}
VERIFY(0x022C6BDC, &daNpcPeople_c::getPigTimer);

/* 022C6C00 */
BOOL daNpcPeople_c::isPigOk() {
    WWHD_FUNC(0x022C6C00, BOOL, this);
    u8 a = dComIfGs_getTmpReg(0xFC03 /* UNK_FC03 */);
    return a == dComIfGs_getTmpReg(0xFF03 /* UNK_FF03 */) ? TRUE : FALSE;
}
VERIFY(0x022C6C00, &daNpcPeople_c::isPigOk);

/* 022C87B8 */
static BOOL daNpc_PeopleIsDelete(void* i_this) {
    WWHD_FUNC(0x022C87B8, BOOL, i_this);
    return true;
}
VERIFY(0x022C87B8, daNpc_PeopleIsDelete);

/* 022C8860 */
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift) {
    WWHD_FUNC(0x022C8860, u32, i_actor, i_width, i_shift);
    /* PowerPC slw/srw: shift counts 32..63 give 0 */
    u32 mask = ((i_width & 0x20) ? 0u : (1u << (i_width & 0x1F))) - 1;
    u32 prm = i_actor->mParameters;
    return ((i_shift & 0x20) ? 0u : (prm >> (i_shift & 0x1F))) & mask;
}
VERIFY(0x022C8860, daObj_PrmAbstract);

/* 022BEB7C */
void daNpcPeople_c::setAnm(u32 param_1, int param_2, f32 morf, f32 param_4) {
    WWHD_FUNC(0x022BEB7C, void, this, param_1, param_2, morf, param_4);
    if (!(mNpcNo < 0x13)) {
        JUT_ASSERT_fail(STR(0x10020420), 0x246A, STR(0x100203EC)); /* HD: l_arcname_tbl bound check */
    }
    if (mNpcNo == NPC_UW1) {
        if (m793 == 4 && param_1 == 0) {
            morf = 12.0f;
        } else if (m793 == 3 && param_1 == 1) {
            morf = 12.0f;
        }
    }
    if (!(m744 < 0.0f)) {
        morf = m744;
        m744 = -1.0f;
    }
    const char* arc = people_arcname(mNpcNo);
    s32 bck = getBck(param_1);
    /* r6/r7: the high halves GHS left from loading the -1.0f/0.0f constants */
    J3DAnmTransform* pAnm = (J3DAnmTransform*)dComIfG_getObjectIDRes_r67(arc, bck, 0x10020000, 0x10020000);
    mpMorf->setAnm(pAnm, param_2, morf, param_4, 0.0f, -1.0f, nullptr);
    if (mpHeadMorf.get() != nullptr && getHeadBck(0) >= 0) {
        const char* arc2 = people_arcname(mNpcNo);
        s32 head_bck = getHeadBck(param_1);
        J3DAnmTransform* pHeadAnm = (J3DAnmTransform*)dComIfG_getObjectIDRes(arc2, head_bck);
        mpHeadMorf->setAnm(pHeadAnm, param_2, morf, param_4, 0.0f, -1.0f, nullptr);
    }
    m793 = param_1;
    m760 = param_2;
}
VERIFY(0x022BEB7C, &daNpcPeople_c::setAnm);

/* 022BED70 */
bool daNpcPeople_c::setAnmTbl(sPeopleAnmDat* param_1, int param_2) {
    WWHD_FUNC(0x022BED70, bool, this, param_1, param_2);
    if (mNpcNo == NPC_SA5) {
        if (m793 == 1) {
            m744 = 16.0f;
        }
    } else if (mNpcNo == NPC_SA4) {
        if (m793 == 1) {
            m744 = 13.0f;
        }
    } else if (mNpcNo == NPC_UM3 && m793 == 5) {
        m744 = 13.0f;
        if (param_1->field_0x00 == 1) {
            m744 = 6.0f;
        }
    }
    mAnmFlag = mAnmFlag & ~0x1;
    if (param_1->field_0x00 == 0xFF && param_2 != 0) {
        m728 = nullptr;
        return true;
    }
    m728 = param_1;
    m796 = m728->field_0x02;
    m750 = 1.0f;
    int temp = 2;
    if (m796 != 0) {
        temp = 0;
    }
    if (m796 < 0) {
        m796 = -m796;
        m750 = -1.0f;
    }
    if (m793 != m728->field_0x00 || temp == 0 || m760 == 0) {
        setAnm(m728->field_0x00, temp, (f32)m728->field_0x01, m750);
    }
    return false;
}
VERIFY(0x022BED70, &daNpcPeople_c::setAnmTbl);

/* 022BEEDC */
void daNpcPeople_c::setWaitAnm() {
    WWHD_FUNC(0x022BEEDC, void, this);
    if (mNpcNo == NPC_UB3 && m789 != 0) {
        setAnmTbl(gabi::at<sPeopleAnmDat>(0x101C31B4) /* l_npc_anm_wait */, 0);
    } else if (mNpcNo == NPC_UM3 && !mbIsNight && dComIfGs_checkGetItem(0xF0 /* dItemNo_COLLECT_MAP_15_e */)) {
        setAnmTbl(gabi::at<sPeopleAnmDat>(0x101C31D8) /* l_npc_anm_um3_wait3 */, 1);
    } else if (mNpcNo == NPC_UW2 && m793 == 1) {
        setAnmTbl(gabi::at<sPeopleAnmDat>(0x101C318C) /* l_npc_anm_talkH2 */, 1);
    } else {
        if (!(mNpcNo < 0x13)) {
            JUT_ASSERT_fail(STR(0x1002047C), 0x24CB, STR(0x10020448));
        }
        /* l_npc_anm_wait_tbl[mNpcNo][mbIsNight] (.data 0x101C4044) */
        setAnmTbl(gabi::at<sPeopleAnmDat>(gabi::load<u32>(0x101C4044 + (mNpcNo * 2 + mbIsNight) * 4)), 1);
    }
}
VERIFY(0x022BEEDC, &daNpcPeople_c::setWaitAnm);

/* 022BF05C */
s16 daNpcPeople_c::XyCheckCB(int i_itemBtn) {
    WWHD_FUNC(0x022BF05C, s16, this, i_itemBtn);
    u8 itemNo = dComIfGp_getSelectItem(i_itemBtn);
    switch (mNpcNo) {
    case NPC_UO3:
    case NPC_UM2:
        if (isPhoto(itemNo)) {
            return true;
        }
        break;
    case NPC_UB1:
    case NPC_UB2:
        if (dComIfGs_isEventBit(0x2102) && isPhoto(itemNo)) {
            return true;
        }
        if (itemNo == 0x1F /* dItemNo_JOY_PENDANT_e */) {
            return true;
        }
        break;
    case NPC_UB3:
        if (itemNo == 0x1F) {
            return true;
        }
        break;
    case NPC_UB4:
        if (!(mEtcFlag & 0x40) && isPhoto(itemNo)) {
            return true;
        }
        if (itemNo == 0x1F) {
            return true;
        }
        break;
    case NPC_UM3:
        if (!mbIsNight && isPhoto(itemNo) && dComIfGs_isEventBit(0x2310) && !dComIfGs_checkGetItem(0xF0)) {
            return true;
        }
        break;
    case NPC_SA5:
        if (isPhoto(itemNo)) {
            return false;
        }
        if (!mbIsNight && dComIfGs_isEventBit(0x2440) && !dComIfGs_isTmpBit(0x0280)) {
            return true;
        }
        break;
    }
    return false;
}
VERIFY(0x022BF05C, &daNpcPeople_c::XyCheckCB);

/* 022BF220 */
s16 daNpcPeople_c::XyEventCB(int i_itemBtn) {
    WWHD_FUNC(0x022BF220, s16, this, i_itemBtn);
    s16 ret = -1;
    u8 itemNo = dComIfGp_getSelectItem(i_itemBtn);
    switch (mNpcNo) {
    case NPC_UB1:
    case NPC_UB2:
        if (itemNo != 0x1F /* dItemNo_JOY_PENDANT_e */) {
            ret = m766[1];
            m79C = 0;
            break;
        }
        m79C = 1;
        break;
    case NPC_SA5:
        if (itemNo == 0x45 /* dItemNo_SKULL_NECKLACE_e */) {
            ret = m766[0];
            m79C = 0;
            break;
        }
    default:
        ret = dComIfGp_evmng_getEventIdx(STR(0x10020490) /* "DEFAULT_TALK_XY" */, 0xFF);
    }
    return ret;
}
VERIFY(0x022BF220, &daNpcPeople_c::XyEventCB);

/* PsoData (photo object data, l_pso_tbl at .data 0x101C39B0) */
struct PsoData_l {
    /* 0x00 */ be<f32> field_0x00;
    /* 0x04 */ be<f32> field_0x04;
    /* 0x08 */ be<f32> field_0x08;
    /* 0x0C */ be<f32> field_0x0C;
    /* 0x10 */ be<f32> field_0x10;
    /* 0x14 */ be<s16> field_0x14;
    /* 0x16 */ be<u8> field_0x16;
    /* 0x17 */ be<u8> field_0x17;
    /* 0x18 */ be<u8> photoNo;
};
/* dSnap_Obj (0x34, out-of-line constructor) */
struct dSnap_Obj_l {
    u8 _00[0x34];
};
static inline void dSnap_Obj_ct(dSnap_Obj_l* o) { gabi::call(0x025BD71C, o); }
static inline void dSnap_Obj_SetInf(dSnap_Obj_l* o, u8 photoNo, fopAc_ac_c* a, u8 b, u8 c, s16 cull) {
    gabi::call(0x025BEB90, o, photoNo, a, b, c, cull);
}
static inline void dSnap_Obj_SetGeo(dSnap_Obj_l* o, cXyz* c, f32 r, f32 h, s16 ang) { gabi::call(0x025BEB5C, o, c, r, h, ang); }
static inline void dSnap_RegistSnapObj(dSnap_Obj_l* o) { gabi::call(0x025BEB4C, o); }

/* J3DModel::getAnmMtx(jnt) with the HD matrix block's dirty flag, then setBaseTRMtx (copy) */
static inline void people_setBaseTRMtx_anm(J3DModel* dst, J3DModel* src, s32 jnt) {
    J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(gabi::ea(src) + 0x2C));
    blk->mFlags |= 0x10;
    mtx_copy(gabi::at<Mtx34>(gabi::ea(dst) + 0xC8), gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30));
}

/* 022C36D4 */
bool daNpcPeople_c::_draw() {
    WWHD_FUNC(0x022C36D4, bool, this);
    J3DModel* bodyModel = mpMorf->getModel();
    J3DModel* headModel;
    if (mpHeadModel.get() != nullptr) {
        headModel = mpHeadModel;
    } else {
        headModel = mpHeadMorf->getModel();
    }
    J3DModelData* headModelData = J3DModel_getModelData_l(headModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    mDoExt_McaMorf* morf = mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &tevStr);
    setLightTevColorType(dKy_getEnvlight(), headModel, &tevStr);
    u32 btp_tbl = 0x10020204; /* l_btp_ix_tbl */
    if (gabi::load<s32>(btp_tbl + mNpcNo * 4) >= 0) {
        mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, headModelData, m78E);
    }
    /* HD: no l_bmt_ix_tbl material table */
    mpMorf->updateDL();
    people_setBaseTRMtx_anm(headModel, bodyModel, m_jnt.mHeadJntNum);
    if (mpHeadModel.get() != nullptr) {
        mDoExt_modelUpdateDL(mpHeadModel);
    } else {
        mpHeadMorf->updateDL();
    }
    if (gabi::load<s32>(btp_tbl + mNpcNo * 4) >= 0) {
        gabi::store<u32>(gabi::ea(headModelData) + 0x38, 0); /* mBtpAnm.remove(headModelData) */
    }
    if (mpEtcModel.get() != nullptr && (mEtcFlag & 0x40000001)) {
        setLightTevColorType(dKy_getEnvlight(), mpEtcModel, &tevStr);
        people_setBaseTRMtx_anm(mpEtcModel, bodyModel, m7A3);
        mDoExt_modelUpdateDL(mpEtcModel);
    }
    /* HD: no dComIfGd_setShadow / addRealShadow */
    if (m7A6 != 0xFF) {
        gabi::Local<dSnap_Obj_l> obj;
        dSnap_Obj_ct(obj.get());
        PsoData_l* pso = gabi::at<PsoData_l>(gabi::load<u32>(0x101C39B0 + m7A6 * 4)); /* l_pso_tbl */
        gabi::Local<cXyz> temp;
        temp->x = pso->field_0x00;
        temp->y = pso->field_0x04;
        temp->z = pso->field_0x08;
        PSVECAdd(temp.get(), &current.pos, temp.get());
        dSnap_Obj_SetInf(obj.get(), pso->photoNo, this, pso->field_0x16, pso->field_0x17, 0x7FFF);
        dSnap_Obj_SetGeo(obj.get(), temp.get(), pso->field_0x0C, pso->field_0x10, pso->field_0x14 + current.angle.y);
        dSnap_RegistSnapObj(obj.get());
        /* HD: m7A6 is not reset to 0xFF */
    } else {
        dSnap_RegistFig(gabi::load<u8>(0x10020158 + mNpcNo) /* l_photo_no */, this, 1.0f, 1.0f, 1.0f);
    }
    return true;
}
VERIFY(0x022C36D4, &daNpcPeople_c::_draw);

/* J3DModelData (HD): 027F3F94 (the matcher calls it __nw) returns the joint tree header (joint
 * count u16 at +8); the joint nodes are an array of 0x1C bytes at +8 (count at +4; an index past
 * the end gives element 0), callback at +8 */
static inline u16 J3DModelData_getJointNum_l(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline void J3DModelData_setJointCallBack_l(J3DModelData* d, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(gabi::ea(d) + 4);
    u32 p = gabi::load<u32>(gabi::ea(d) + 8);
    if (i < n) {
        p += i * 0x1C;
    }
    gabi::store<u32>(p + 8, cb);
}
static inline J3DModelData* people_getRes(u32 npcNo, s32 id) { return (J3DModelData*)dComIfG_getObjectIDRes(people_arcname(npcNo), id); }

/* 022BE334 */
BOOL daNpcPeople_c::createHeap() {
    WWHD_FUNC(0x022BE334, BOOL, this);
    /* r6/r7: what GHS leaves there (compared through getIDRes's GameCube signature) */
    u32 npc = mNpcNo;
    people_set_r(7, npc * 4);
    J3DModelData* bodyModelData = people_getRes(npc, gabi::load<s32>(0x1002016C + npc * 4) /* l_bmd_ix_tbl */);
    u32 npc2 = mNpcNo;
    s32 bck = getBck(m793);
    people_set_r(7, npc2 * 4);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(people_arcname(npc2), bck);
    /* HD: l_diff_flag_tbl is 0x11020203 for every NPC (a constant) */
    mpMorf = mDoExt_McaMorf::create(nullptr, bodyModelData, nullptr, nullptr, anm, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1,
                                    nullptr, 0x00080000, 0x11020203);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return false;
    }
    u32 npc3 = mNpcNo;
    people_set_r(7, npc3 * 4);
    J3DModelData* headModelData = people_getRes(npc3, gabi::load<s32>(0x100201B8 + npc3 * 4) /* l_head_bmd_ix_tbl */);
    int headBck = getHeadBck(0);
    if (headBck < 0) {
        mpHeadModel = mDoExt_J3DModel__create(headModelData, 0x80000, gabi::load<u32>(0x10020250 + mNpcNo * 4) /* l_head_diff_flag_tbl */);
        if (mpHeadModel.get() == nullptr) {
            return false;
        }
    } else {
        u32 npc4 = mNpcNo;
        s32 hbck = getHeadBck(m793);
        const char* arc = people_arcname(npc4);
        people_set_r(6, gabi::ea(arc));
        J3DAnmTransform* hanm = (J3DAnmTransform*)dComIfG_getObjectIDRes(arc, hbck);
        mpHeadMorf = mDoExt_McaMorf::create(nullptr, headModelData, nullptr, nullptr, hanm, 2, 1.0f, 0, -1, 1, nullptr,
                                            0x00080000, gabi::load<u32>(0x10020250 + mNpcNo * 4));
        if (mpHeadMorf.get() == nullptr || mpHeadMorf->getModel() == nullptr) {
            return false;
        }
    }
    m_jnt.mHeadJntNum = JUTNameTab_getIndex(J3DModelData_getJointName(bodyModelData), STR(0x10020374) /* "head" */);
    if (!(m_jnt.mHeadJntNum >= 0)) {
        JUT_ASSERT_fail(STR(0x1002037C), 0x128C, STR(0x10020390));
    }
    m_jnt.mBackboneJntNum = JUTNameTab_getIndex(J3DModelData_getJointName(bodyModelData), STR(0x100203AC) /* "backbone" */);
    if (!(m_jnt.mBackboneJntNum >= 0)) {
        JUT_ASSERT_fail(STR(0x1002037C), 0x1290, STR(0x100203B8));
    }
    if (!initTexPatternAnm(false)) {
        return false;
    }
    for (u16 i = 0; i < J3DModelData_getJointNum_l(bodyModelData); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            J3DModelData_setJointCallBack_l(bodyModelData, i, 0x022BE034 /* daNpc_People_nodeCallBack */);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    u32 npc5 = mNpcNo;
    s32 etc = gabi::load<s32>(0x1002029C + (npc5 * 2 + mbIsNight) * 4); /* l_etc_bmd_ix_tbl */
    if (etc >= 0) {
        people_set_r(6, npc5 * 4);
        J3DModelData* etcModelData = people_getRes(npc5, etc);
        mpEtcModel = mDoExt_J3DModel__create(etcModelData, 0x80000, 0x11000002);
        if (mpEtcModel.get() == nullptr) {
            return false;
        }
        m7A3 = JUTNameTab_getIndex(J3DModelData_getJointName(bodyModelData), STR(0x1002036C) /* "handR" */);
    }
    return true;
}
VERIFY(0x022BE334, &daNpcPeople_c::createHeap);

/* sead::SafeString equality (HD form of strcmp(a, b) == 0): both strings are terminated through
 * their vtable (slot +0x14; the left one twice), then compared by pointer, then byte by byte (at
 * most 0x40001 bytes) */
static inline bool people_SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b);
    u32 pa = a->mStringTop;
    u32 pb = b->mStringTop;
    if (pa == pb) {
        return true;
    }
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = gabi::load<u8>(pa + n);
        if (ca != gabi::load<u8>(pb + n)) {
            return false;
        }
        if (ca == 0) {
            return true;
        }
    }
    return false;
}
/* 02554288 dKy_get_dayofweek */
static inline s32 dKy_get_dayofweek() { return gabi::call<s32>(0x02554288); }

/* 022BE8C0 */
static cPhs_State phase_1(daNpcPeople_c* i_this) {
    WWHD_FUNC(0x022BE8C0, cPhs_State, i_this);
    /* fopAcM_ct(i_this, daNpcPeople_c) */
    if (!(i_this->actor_condition & fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            daNpcPeople_c_ct(i_this);
        }
        i_this->actor_condition = i_this->actor_condition | fopAcCnd_INIT_e;
    }
    u32 arg0 = i_this->getPrmArg0();
    switch (i_this->getNpcNo()) {
    case NPC_UB4: {
        gabi::Local<SafeString> name;
        name->mStringTop = 0x100203E0; /* "Ocmera" */
        name->__vtbl = PEOPLE_SAFESTRING_VTBL;
        u32 play = dComIfGp_ea();
        gabi::Local<SafeString> start; /* dComIfGp_getStartStageName() */
        start->__vtbl = PEOPLE_SAFESTRING_VTBL;
        start->mStringTop = play + 0x5134;
        if (people_SafeString_eq(name.get(), start.get())) {
            if (dComIfGs_checkGetItem(0xEB /* dItemNo_COLLECT_MAP_20_e */) ||
                arg0 != (u32)(s32)gabi::load<s16>(dComIfGp_ea() + 0x513C) /* dComIfGp_getStartStagePoint() */) {
                return cPhs_UNK3_e; /* cPhs_STOP_e */
            }
            i_this->setResFlag(1);
            i_this->setEtcFlag(0x40);
            return cPhs_NEXT_e;
        }
        break;
    }
    case NPC_UW2:
    case NPC_UM2:
        if (arg0 != 0xFF && (u32)((arg0 << 1) & 0xFE) != (u32)(dComIfGs_getEventReg(0xB907) & 6)) {
            return cPhs_UNK3_e;
        }
        break;
    case NPC_UG1:
    case NPC_UG2: {
        int day = dKy_get_dayofweek();
        switch (arg0) {
        case 0:
            if (day == 5 || day == 6) {
                return cPhs_UNK3_e;
            }
            break;
        case 1:
            if (day != 5 && day != 6) {
                return cPhs_UNK3_e;
            }
            break;
        }
        break;
    }
    default:
        break;
    }
    i_this->setResFlag(1);
    return cPhs_NEXT_e;
}
VERIFY(0x022BE8C0, phase_1);

/* 0259E6D0 dNpc_PathRun_c::setInf(u8 pathIdx, s8 roomNo, u8 forwards) */
static inline bool dNpc_PathRun_setInf(dNpc_PathRun_l* r, u8 idx, s8 room, u8 fwd) { return gabi::call<bool>(0x0259E6D0, r, idx, room, fwd); }
/* 025D9F38 fopAcM_searchFromName(name, param mask, param) */
static inline fopAc_ac_c* fopAcM_searchFromName_l(const char* name, u32 mask, u32 prm) { return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm); }
static inline s16 people_getEventIdx(u32 name) { return dComIfGp_evmng_getEventIdx(STR(name), 0xFF); }

/* 022BF510 */
cPhs_State daNpcPeople_c::createInit() {
    WWHD_FUNC(0x022BF510, cPhs_State, this);
    int temp = 0xFF;
    u8 pathIndex = getPrmRailID();
    if (pathIndex != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, pathIndex, fopAcM_GetRoomNo(this), true);
        if (mPathRun.mPath.get() == nullptr) {
            return cPhs_ERROR_e;
        }
        actor_status = actor_status & ~0x80; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        gabi::Local<cXyz> point;
        dNpc_PathRun_getPoint(&mPathRun, point.get(), mPathRun.mIdx);
        old.pos.copy(*point);
        current.pos.copy(*point);
        dNpc_PathRun_incIdxLoop(&mPathRun);
        m76E = 1;
        temp = 0xFE;
        m7A1 = 1;
    }
    gravity = -9.0f;
    setWaitAnm();
    for (int i = 0; i < 4; i++) {
        m766[i] = -1;
    }
    switch (mNpcNo) {
    case NPC_UO1:
        m766[0] = people_getEventIdx(0x100204F4); /* "UO1_GET_ITEM" */
        break;
    case NPC_UB1:
    case NPC_UB2:
        m766[0] = people_getEventIdx(0x10020504); /* "UB1_TALK" */
        m766[1] = people_getEventIdx(0x100204DC); /* "UB1_TALK_XY" */
        m766[1] = people_getEventIdx(0x10020510); /* "UB1_TALK_PHOTO" */
        {
            s16 idx = people_getEventIdx(0x100204C4); /* "UB1_TALK_PHOTO_GET_ITEM" */
            m79C = 0;
            m766[3] = idx;
        }
        gabi::store<s16>(gabi::ea(this) + 0xFC, m766[0]); /* eventInfo.setEventId(m766[0]) */
        break;
    case NPC_UB4:
        m766[0] = people_getEventIdx(0x10020520); /* "UB4_GET_ITEM" */
        break;
    case NPC_UW2:
        m766[0] = people_getEventIdx(0x10020530); /* "UW2_GET_ITEM" */
        break;
    case NPC_UM1:
        m766[0] = people_getEventIdx(0x10020540); /* "UM1_GET_ITEM" */
        m766[1] = people_getEventIdx(0x10020550); /* "UM1_TALK" */
        break;
    case NPC_UM3:
        m766[0] = people_getEventIdx(0x1002055C); /* "UM3_TELESCOPE_TALK" */
        m766[1] = people_getEventIdx(0x10020570); /* "UM3_GET_ITEM" */
        if (mbIsNight) {
            mEtcFlag = mEtcFlag | 0x40000000;
        }
        break;
    case NPC_SA3:
        m766[0] = people_getEventIdx(0x10020580); /* "SA3_GET_ITEM" */
        break;
    case NPC_SA5: {
        m766[0] = people_getEventIdx(0x100204E8); /* "SA5_TALK_XY" */
        m766[1] = people_getEventIdx(0x10020590); /* "SA5_GET_ITEM" */
        u8 reg = dComIfGs_getTmpReg(0xFD07 /* UNK_FD07 */);
        for (int i = 0; i < 3; i++) {
            if (reg & (1 << i)) {
                u32 pig = gabi::ea(fopAcM_searchFromName_l(STR(0x100204C0) /* "Pig" */, 0xF00,
                                                           gabi::load<u32>(0x1002014C + i * 4) /* l_pig_para */));
                if (pig != 0) {
                    /* kb_class::taura_pos_set(current.pos) */
                    f32 y = current.pos.y, x = current.pos.x, z = current.pos.z;
                    gabi::store<f32>(pig + 0x798, x);
                    gabi::store<f32>(pig + 0x79C, y);
                    gabi::store<u8>(pig + 0x750, 1);
                    gabi::store<f32>(pig + 0x7A0, z);
                    gabi::at<fopAc_ac_c>(pig)->current.pos.copy(current.pos);
                }
            }
        }
    }
        /* fall through */
    case NPC_SA4:
        if (mbIsNight) {
            mEtcFlag = mEtcFlag | 0x40000000;
        }
        break;
    case NPC_UG1:
        /* HD: an UG1 placed at x = -1984 is moved to (-2002, 284, -200328) */
        if (gabi::ftoi(home.pos.x) == -0x7C0) {
            current.pos.y = 284.0f;
            current.pos.z = -200328.0f;
            old.pos.y = 284.0f;
            old.pos.z = -200328.0f;
            current.pos.x = -2002.0f;
            old.pos.x = -2002.0f;
            home.pos.x = -2002.0f;
            home.pos.y = 284.0f;
            home.pos.z = -200328.0f;
        }
        /* fall through */
    case NPC_UG2:
        if (getPrmArg0() == 0) {
            temp = 0x50;
            m76E = 1;
            mEtcFlag = 0x1000;
        }
        m766[0] = people_getEventIdx(0x100205A0); /* "UG1_TALK" */
        m7A1 = 1;
        break;
    default:
        break;
    }
    gabi::store<u32>(gabi::ea(this) + 0x104, 0x022BF21C); /* eventInfo.setXyCheckCB(daNpcPeople_XyCheckCB) */
    gabi::store<u32>(gabi::ea(this) + 0x100, 0x022BF300); /* eventInfo.setXyEventCB(daNpcPeople_XyEventCB) */
    gabi::store<u32>(gabi::ea(this) + 0x108, 0x022BF39C); /* eventInfo.setPhotoEventCB(daNpcPeople_photoCB) */
    mEventCut.setActorInfo2(gabi::at<const char>(gabi::load<u32>(0x101C3E7C + mNpcNo * 4)) /* l_npc_staff_id */, this);
    m784 = 0;
    mTalk = 0;
    m789 = 0;
    m78A = 0;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
    cullSizeFar = mpNpcDat->field_0x30 / gabi::load<f32>(0x1048D04C) /* mDoLib_clipper::getFar() */;
    u32 dist = 0x101C3E28 + mNpcNo * 2 + mbIsNight; /* l_npc_dist_tbl[mNpcNo][mbIsNight] */
    gabi::store<u8>(gabi::ea(this) + 0x389, gabi::load<u8>(dist)); /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0x0100000A); /* attention_info.flags */
    gabi::store<u8>(gabi::ea(this) + 0x38B, gabi::load<u8>(dist)); /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
    daNpcPeople_c__l_npc_dat* dat = mpNpcDat;
    m_jnt.setParam(dat->field_0x04, dat->field_0x06, dat->field_0x0C, dat->field_0x0E, dat->field_0x00, dat->field_0x02,
                   dat->field_0x08, dat->field_0x0A, dat->field_0x10);
    if (mNpcNo == NPC_UM3 && !mbIsNight) {
        dComIfGs_checkGetItem(0xF0 /* dItemNo_COLLECT_MAP_15_e */);
    }
    m79D = mpNpcDat->field_0x5A;
    m79E = mpNpcDat->field_0x5C;
    m79F = mpNpcDat->field_0x5D;
    m748 = mpNpcDat->field_0x28;
    m776 = mpNpcDat->field_0x34;
    m778 = mpNpcDat->field_0x36;
    m74C = mpNpcDat->field_0x3C;
    if (mNpcNo != NPC_UM3) {
        mObjAcch.CrrPos(dComIfG_Bgsp());
        f32 gnd = gabi::load<f32>(gabi::ea(&mObjAcch) + 0x94); /* GetGroundH() */
        if (-1000000000.0f /* -G_CM3D_F_INF */ != gnd) {
            home.pos.y = gnd;
            current.pos.y = gnd;
        }
    }
    setMtx();
    J3DModel_calc(mpMorf->getModel());
    mStts.Init(temp, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    gabi::Local<cXyz> center;
    center->x = current.pos.x;
    center->y = current.pos.y;
    center->z = current.pos.z;
    setCollision(&mCyl, center.get(), m74C, mpNpcDat->field_0x40);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x022BF510, &daNpcPeople_c::createInit);

/* 022C8710 __sinit_d_a_npc_people_cpp (header statics: the objects at P+8 / P+9, P = 0x10468314) */
static void __sinit_d_a_npc_people_cpp() {
    WWHD_FUNC(0x022C8710, void);
    for (int i = 0; i < 4; i++) {
        gabi::store<u32>(0x10468320 + 4 * i, 0);
    }
    __register_global_object(0x101C4CAC);
    gabi::store<f32>(0x10468314, -3.1415927f);
    gabi::store<f32>(0x10468318, 3.1415927f);
    gabi::call(0x028ED6F8, 0x1046831Cu);
    __register_global_object(0x101C4CB8);
    gabi::call(0x028EAB2C, 0x1046831Du);
    __register_global_object(0x101C4CC4);
}
VERIFY(0x022C8710, __sinit_d_a_npc_people_cpp);

/* 022C87A4: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x022C87A4, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x022C87A4, SafeString_dt);

/* 022C87C0: daNpcPeople_c deleting destructor (compiler-generated, HD virtual destructor; the
 * members of daNpcPeople_c itself are trivially destructible) */
static void daNpcPeople_dt(daNpcPeople_c* p, s32 flags) {
    WWHD_FUNC(0x022C87C0, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x02515A70, &p->mCyl, 2);                                    /* ~dCcD_Cyl */
        gabi::call(0x02515860, &p->mStts, 2);                                   /* ~dCcD_Stts */
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mAcchCir) + 0x14), 2); /* ~cM3dGCir */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x20, 0x1002012C);            /* ~dBgS_ObjAcch */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x14, 0x1002013C);
        gabi::call(0x024EFD9C, &p->mObjAcch, 0);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(p);
        }
    }
}
VERIFY(0x022C87C0, daNpcPeople_dt);

/* 022C885C: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x022C885C, void);
}
VERIFY(0x022C885C, SafeString_assureTerminationImpl);
