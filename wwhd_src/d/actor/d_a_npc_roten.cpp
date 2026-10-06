/**
 * d_a_npc_roten.cpp (WWHD)
 * NPC - Traveling Merchants: construction, heap, draw, animation, collision, messages.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_roten.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_roten.h"

/* 022D30F4 daNpcRoten_c::daNpcRoten_c (the matcher calls it daNpcRoten_c::setAnm; HD: allocates
 * 0xB40 when this == NULL) */
static daNpcRoten_c* daNpcRoten_c_ct(daNpcRoten_c* i_this) {
    WWHD_FUNC(0x022D30F4, daNpcRoten_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpcRoten_c*)operator_new(0xB40);
        if (i_this == nullptr)
            return i_this;
    }
    gabi::call(0x025A1458, i_this); /* fopNpc_npc_c::fopNpc_npc_c */
    i_this->__vtbl = ROTEN_VTBL;
    gabi::call(0x025E7820, i_this->mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
    dCcD_Cyl_ct(&i_this->mCyl2, 0x10021290);
    gabi::call(0x025166F0, &i_this->mSph); /* dCcD_Sph::dCcD_Sph */
    i_this->field_0x9C8 = 1;
    i_this->field_0x9C7 = 0;
    i_this->field_0x9CA = 0;
    i_this->mShownItemBtn = 3; /* dItemBtn_NONE_e */
    i_this->field_0x6FC = 0xFFFFFFFF;
    i_this->field_0x98C = nullptr;
    i_this->field_0x990 = 0.0f;
    i_this->field_0x9C9 = 0;
    i_this->field_0x994 = -1.0f;
    i_this->field_0x9C6 = 0;
    i_this->field_0x9C4 = 0;
    i_this->field_0x9BB = 0;
    i_this->field_0x9BC = 0;
    i_this->field_0x9C1 = 0;
    i_this->field_0x99C = 1;
    i_this->field_0x9B2 = 0;
    i_this->field_0x9C0 = 0;
    return i_this;
}
VERIFY(0x022D30F4, daNpcRoten_c_ct);

/* 022D2A60 daNpc_Roten_nodeCallBack (unnamed by the matcher) */
static BOOL daNpc_Roten_nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x022D2A60, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        daNpcRoten_c* i_this = gabi::at<daNpcRoten_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        u32 jntNo = gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4);
        PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
        if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
            mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[0][0]);
        }
        if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            mDoMtx_ZrotM(calc_mtx(), (s16)-i_this->m_jnt.mAngles[1][0]);
        }
        Mtx34* dst = getAnmMtx(model, jntNo);
        mtx_copy(dst, calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868)); /* J3DSys::mCurrentMtx */
    }
    return TRUE;
}
VERIFY(0x022D2A60, daNpc_Roten_nodeCallBack);

/* 022D2BB8 */
BOOL daNpcRoten_c::initTexPatternAnm(u8 modify) {
    WWHD_FUNC(0x022D2BB8, BOOL, this, modify);
    J3DModelData* modelData = J3DModel_getModelData_r(mpMorf->getModel());
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(arcname(mNpcNo), gabi::load<s32>(l_btp_ix_tbl + mNpcNo * 4));
    if (m_head_tex_pattern.get() == nullptr) /* JUT_ASSERT(3117, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail(STR(0x100213A8), 0xC2D, STR(0x100213BC));
    if (gabi::call<s32>(0x025E789C, mBtpAnm, modelData, m_head_tex_pattern.get(), 1, 2 /* EMode_LOOP */, 1.0f, 0, -1,
                        (u32)modify, 0) == 0) { /* mBtpAnm.init */
        return FALSE;
    }
    field_0x9B8 = 0;
    field_0x9AA = 0;
    return TRUE;
}
VERIFY(0x022D2BB8, &daNpcRoten_c::initTexPatternAnm);

static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }

/* 022D2CCC */
BOOL daNpcRoten_c::createHeap() {
    WWHD_FUNC(0x022D2CCC, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(arcname(mNpcNo), gabi::load<s32>(l_bmd_ix_tbl + mNpcNo * 4));
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(
        arcname(mNpcNo), gabi::load<s32>(l_bck_ix_tbl + (mNpcNo * 10 + field_0x9C0) * 4));
    /* HD: the same differed display list flags for all three merchants (GameCube l_diff_flag_tbl) */
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1,
                                    nullptr, 0x80000, 0x11020203);
    if (mpMorf.get() == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    J3DModelData* headModelData =
        (J3DModelData*)dComIfG_getObjectIDRes(arcname(mNpcNo), gabi::load<s32>(l_head_bmd_ix_tbl + mNpcNo * 4));
    J3DAnmTransform* headBck =
        (J3DAnmTransform*)dComIfG_getObjectIDRes(arcname(mNpcNo), gabi::load<s32>(l_head_bck_ix_tbl + mNpcNo * 4));
    field_0x6D8 = mDoExt_McaMorf::create(nullptr, headModelData, nullptr, nullptr, headBck, 2, 1.0f, 0, -1, 1, nullptr,
                                         0x80000, 0x37441422);
    if (field_0x6D8.get() == nullptr || field_0x6D8->getModel() == nullptr) {
        return FALSE;
    }
    field_0x6D4 = field_0x6D8->getModel();

    m_jnt.mHeadJntNum = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x100213E4) /* "head" */);
    if (m_jnt.mHeadJntNum < 0) /* JUT_ASSERT(1605, m_jnt.getHeadJntNum() >= 0) */
        JUT_ASSERT_fail(STR(0x100213F4), 0x645, STR(0x1002142C));
    m_jnt.mBackboneJntNum = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10021408) /* "backbone1" */);
    if (m_jnt.mBackboneJntNum < 0)
        JUT_ASSERT_fail(STR(0x100213F4), 0x64A, STR(0x10021448));
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    m_hand_L_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x100213DC) /* "handL" */);
    if (m_hand_L_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100213F4), 0x65C, STR(0x10021414));
    m_bag_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x100213EC) /* "Bag1" */);
    if (m_bag_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x100213F4), 0x660, STR(0x10021468));

    for (u16 i = 0; i < J3DModelData_getJointNum(modelData); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            /* modelData->getJointNodePointer(i)->setCallBack(daNpc_Roten_nodeCallBack) */
            u32 n = gabi::load<u32>(gabi::ea(modelData) + 4);
            u32 joint = gabi::load<u32>(gabi::ea(modelData) + 8);
            if ((u32)i < n)
                joint += i * 0x1C;
            gabi::store<u32>(joint + 8, 0x022D2A60);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    return TRUE;
}
VERIFY(0x022D2CCC, &daNpcRoten_c::createHeap);

/* 022D30F0 CheckCreateHeap (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x022D30F0, BOOL, i_this);
    return ((daNpcRoten_c*)i_this)->createHeap();
}
VERIFY(0x022D30F0, CheckCreateHeap);

/* 022D680C daObj::PrmAbstract<daNpcRoten_c::Prm_e> (PPC shift semantics: amounts 32..63 give 0) */
static u32 daObj_PrmAbstract(fopAc_ac_c* actor, s32 width, s32 shift) {
    WWHD_FUNC(0x022D680C, u32, actor, width, shift);
    u32 prm = actor->mParameters;
    u32 bit = (width & 0x20) ? 0 : (1u << (width & 0x1F));
    u32 v = (shift & 0x20) ? 0 : (prm >> (shift & 0x1F));
    return v & (bit - 1);
}
VERIFY(0x022D680C, daObj_PrmAbstract);

/* 022D3208 */
u8 daNpcRoten_c::getPrmNpcNo() {
    WWHD_FUNC(0x022D3208, u8, this);
    return (u8)daObj_PrmAbstract(this, PRM_NPC_NO_W, PRM_NPC_NO_S);
}
VERIFY(0x022D3208, &daNpcRoten_c::getPrmNpcNo);

/* 022D32D0 */
u8 daNpcRoten_c::getPrmRailID() {
    WWHD_FUNC(0x022D32D0, u8, this);
    return (u8)daObj_PrmAbstract(this, PRM_RAIL_ID_W, PRM_RAIL_ID_S);
}
VERIFY(0x022D32D0, &daNpcRoten_c::getPrmRailID);

/* 022D3234 */
static cPhs_State phase_1(daNpcRoten_c* i_this) {
    WWHD_FUNC(0x022D3234, cPhs_State, i_this);
    /* fopAcM_ct(i_this, daNpcRoten_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            daNpcRoten_c_ct(i_this);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    i_this->mNpcNo = i_this->getPrmNpcNo();
    if (!dComIfGs_isEventBit(0x1108)) {
        return 3; /* cPhs_STOP_e */
    }
    i_this->field_0x9BC = 1; /* setResFlag(1) */
    return 2; /* cPhs_NEXT_e */
}
VERIFY(0x022D3234, phase_1);

/* 022D3A44 */
static cPhs_State phase_2(daNpcRoten_c* i_this) {
    WWHD_FUNC(0x022D3A44, cPhs_State, i_this);
    u8 npcNo = i_this->getPrmNpcNo();
    /* HD: l_arcname_tbl is indexed through a range-checked accessor */
    if (npcNo >= 3) {
        JUT_ASSERT_fail(STR(0x100214F4), 0x5E7, STR(0x10021508));
        i_this->mpMorf = nullptr;
        return cPhs_ERROR_e;
    }
    cPhs_State result = dComIfG_resLoad(&i_this->mPhs, arcname(npcNo));
    if (result == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, 0x022D30F0 /* CheckCreateHeap */, 0x4620)) {
            result = i_this->createInit();
        } else {
            i_this->mpMorf = nullptr;
            return cPhs_ERROR_e;
        }
    }
    return result;
}
VERIFY(0x022D3A44, phase_2);

/* 022D3B1C (unnamed by the matcher; daNpc_RotenCreate 022D3B30 branches to it) */
cPhs_State daNpcRoten_c::_create() {
    WWHD_FUNC(0x022D3B1C, cPhs_State, this);
    /* static cPhs__Handler l_method[] = {phase_1, phase_2, NULL} (.data 0x101C6000) */
    return gabi::call<cPhs_State>(0x02525FE4, &mPhs2, 0x101C6000, this); /* dComLbG_PhaseHandler */
}
VERIFY(0x022D3B1C, &daNpcRoten_c::_create);

/* 022D3B30 (tail call) */
static cPhs_State daNpc_RotenCreate(void* i_this) {
    WWHD_FUNC(0x022D3B30, cPhs_State, i_this);
    return ((daNpcRoten_c*)i_this)->_create();
}
VERIFY(0x022D3B30, daNpc_RotenCreate);

/* 022D3710 */
cPhs_State daNpcRoten_c::createInit() {
    WWHD_FUNC(0x022D3710, cPhs_State, this);
    s32 weight = 0xFF;
    u8 railId = getPrmRailID();
    if (railId != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, railId, current.roomNo, 1);
        if (!mPathRun.isPath()) {
            return cPhs_ERROR_e;
        }
        actor_status = actor_status & ~0x80u; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        gabi::Local<cXyz> point;
        dNpc_PathRun_getPoint(&mPathRun, point, mPathRun.mIdx);
        old.pos.copy(*point);
        current.pos.copy(old.pos);
        dNpc_PathRun_incIdxLoop(&mPathRun);
        dNpc_PathRun_getPoint(&mPathRun, point, mPathRun.mIdx);
        gabi::Local<cXyz> a, b;
        b->x = point->x;
        b->y = point->y;
        b->z = point->z;
        a->x = current.pos.x;
        a->y = current.pos.y;
        a->z = current.pos.z;
        dNpc_calc_DisXZ_AngY(a, b, nullptr, &current.angle.y);
        field_0x9A6 = 1;
        weight = 0xFE;
    }
    gravity = -9.0f;
    field_0x99E = dComIfGp_evmng_getEventIdx_r(0x100214CC); /* "ROTEN_EXCHANGE_1ST" */
    field_0x9A0 = dComIfGp_evmng_getEventIdx_r(0x100214E0); /* "ROTEN_EXCHANGE_2ND" */
    field_0x9A2 = dComIfGp_evmng_getEventIdx_r(0x100214A8); /* "ROTEN_CHANGE_ITEM" */
    field_0x9A4 = dComIfGp_evmng_getEventIdx_r(0x100214BC); /* "ROTEN_GET_MAP" */
    mEventCut.setActorInfo2(gabi::at<const char>(gabi::load<u32>(l_npc_staff_id + mNpcNo * 4)), this);
    field_0x9AC = 0;
    field_0x9AE = 0;
    field_0x9B4 = 0;
    field_0x9B5 = 0;
    gabi::store<u32>(gabi::ea(this) + 0x100, 0x022D34DC); /* eventInfo.setXyEventCB(daNpcRoten_XyEventCB) */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAA);        /* attention_info.distances[TALK] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAA);        /* attention_info.distances[SPEAK] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA);        /* LOCKON_TALK | ACTION_SPEAK */
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -200.0f, 0.0f, -200.0f, 200.0f, 300.0f, 200.0f);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    f32 groundH = mObjAcch.GetGroundH();
    if (!(groundH == -1000000000.0f)) { /* -G_CM3D_F_INF */
        current.pos.y = groundH;
        home.pos.y = groundH;
    }
    setMtx();
    J3DModel_calc(mpMorf->getModel());
    mStts.Init(weight, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(dNpc_cyl_src));
    mCyl.SetStts(&mStts);
    mCyl2.Set(gabi::at<dCcD_SrcCyl>(dNpc_cyl_src));
    mCyl2.SetStts(&mStts);
    setCollision(npc_dat(mNpcNo).field_0x2C, 200.0f);
    setCollisionB();
    mSph.Set(gabi::at<dCcD_SrcSph>(l_sph_src));
    mSph.SetStts(&mStts);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x022D3710, &daNpcRoten_c::createInit);

/* 022D3B34 */
bool daNpcRoten_c::_delete() {
    WWHD_FUNC(0x022D3B34, bool, this);
    /* HD: dComIfG_resDelete (GameCube resDeleteDemo) */
    dComIfG_resDelete(&mPhs, arcname(mNpcNo));
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return true;
}
VERIFY(0x022D3B34, &daNpcRoten_c::_delete);

/* 022D3B98 (tail call) */
static BOOL daNpc_RotenDelete(void* i_this) {
    WWHD_FUNC(0x022D3B98, BOOL, i_this);
    return ((daNpcRoten_c*)i_this)->_delete();
}
VERIFY(0x022D3B98, daNpc_RotenDelete);

/* 022D5790 */
bool daNpcRoten_c::_draw() {
    WWHD_FUNC(0x022D5790, bool, this);
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* modelData = J3DModel_getModelData_r(pModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    mDoExt_McaMorf* morf = mpMorf;
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, morf->getModel(), &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, field_0x6D4, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, modelData, field_0x9B8);
    /* HD: no l_bmt_ix_tbl material table */
    mpMorf->updateDL();
    if (field_0x9C8 != 0) {
        Mtx34* headMtx = getAnmMtx(pModel, m_jnt.mHeadJntNum);
        J3DModel_setBaseTRMtx(field_0x6D4, headMtx);
        field_0x6D8->updateDL();
    }
    gabi::store<u32>(gabi::ea(modelData) + 0x38, 0); /* mBtpAnm.remove(modelData) */
    /* HD: no shadow */
    dSnap_RegistFig(0x7E /* DSNAP_TYPE_NPC_ROTEN */, this, 1.0f, 1.0f, 1.0f);
    return true;
}
VERIFY(0x022D5790, &daNpcRoten_c::_draw);

/* 022D58F0 (tail call) */
static BOOL daNpc_RotenDraw(void* i_this) {
    WWHD_FUNC(0x022D58F0, BOOL, i_this);
    return ((daNpcRoten_c*)i_this)->_draw();
}
VERIFY(0x022D58F0, daNpc_RotenDraw);

/* 022D578C (tail call) */
static BOOL daNpc_RotenExecute(void* i_this) {
    WWHD_FUNC(0x022D578C, BOOL, i_this);
    return ((daNpcRoten_c*)i_this)->_execute();
}
VERIFY(0x022D578C, daNpc_RotenExecute);

/* 022D6744 */
static BOOL daNpc_RotenIsDelete(void*) {
    WWHD_FUNC(0x022D6744, BOOL, (void*)nullptr);
    return TRUE;
}
VERIFY(0x022D6744, daNpc_RotenIsDelete);

/* 022D34E0 */
void daNpcRoten_c::setMtx() {
    WWHD_FUNC(0x022D34E0, void, this);
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}
VERIFY(0x022D34E0, &daNpcRoten_c::setMtx);

/* 022D35C0 */
void daNpcRoten_c::setCollisionB() {
    WWHD_FUNC(0x022D35C0, void, this);
    Mtx34* pMtx = getAnmMtx(mpMorf->getModel(), m_bag_jnt_num);
    gabi::Local<cXyz> temp;
    temp->x = pMtx->m[0][3]; /* mDoMtx_multVecZero */
    temp->y = pMtx->m[1][3];
    temp->z = pMtx->m[2][3];
    gabi::Local<cXyz> a, b;
    gabi::Local<be<s16>> angle;
    a->x = current.pos.x;
    a->y = current.pos.y;
    a->z = current.pos.z;
    b->x = temp->x;
    b->y = temp->y;
    b->z = temp->z;
    dNpc_calc_DisXZ_AngY(a, b, nullptr, angle);
    s16 ang = *angle;
    RotenNpcDat& dat = npc_dat(mNpcNo);
    temp->x = gabi::fmadds(cM_ssin(ang), dat.field_0x34, temp->x);
    temp->y = current.pos.y;
    temp->z = gabi::fmadds(cM_scos(ang), dat.field_0x34, temp->z);
    mCyl2.SetC(temp);
    mCyl2.SetR(npc_dat(mNpcNo).field_0x30);
    mCyl2.SetH(200.0f);
    dComIfG_Ccsp_Set(&mCyl2);
}
VERIFY(0x022D35C0, &daNpcRoten_c::setCollisionB);

/* 022D5238 */
void daNpcRoten_c::setCollisionH() {
    WWHD_FUNC(0x022D5238, void, this);
    Mtx34* mtx = getAnmMtx(mpMorf->getModel(), m_jnt.mHeadJntNum);
    PSMTXCopy(mtx, calc_mtx());
    mDoMtx_stack_c::transS(npc_dat(mNpcNo).field_0x3C, 0.0f, 0.0f);
    PSMTXConcat(calc_mtx(), mDoMtx_stack_c::get(), calc_mtx());
    gabi::Local<cXyz> temp;
    Mtx34* m = calc_mtx();
    temp->x = m->m[0][3];
    temp->y = m->m[1][3];
    temp->z = m->m[2][3];
    mSph.SetC(temp);
    mSph.SetR(npc_dat(mNpcNo).field_0x38);
    dComIfG_Ccsp_Set(&mSph);
    if (field_0x9CA == 0 && mSph.ChkTgHit()) {
        executeSetMode(4);
        field_0x9CA = 1;
    }
}
VERIFY(0x022D5238, &daNpcRoten_c::setCollisionH);

/* 022D50D4 */
void daNpcRoten_c::playTexPatternAnm() {
    WWHD_FUNC(0x022D50D4, void, this);
    if (cLib_calcTimer(&field_0x9AA) == 0) {
        s32 frameMax = J3DAnm_getFrameMax(m_head_tex_pattern);
        if ((s32)field_0x9B8 >= frameMax) {
            frameMax = J3DAnm_getFrameMax(m_head_tex_pattern);
            u8 f = field_0x9B8;
            field_0x9AA = 0x78;
            field_0x9B8 = (u8)(f - frameMax);
        } else {
            field_0x9B8 = (u8)(field_0x9B8 + 1);
        }
    }
}
VERIFY(0x022D50D4, &daNpcRoten_c::playTexPatternAnm);

/* 022D5174 */
void daNpcRoten_c::playAnm() {
    WWHD_FUNC(0x022D5174, void, this);
    if (mpMorf->play(nullptr, 0, 0) && field_0x988.get() != nullptr && field_0x9C2 > 0) {
        s8 n = (s8)(field_0x9C2 - 1);
        field_0x9C2 = n;
        if (n == 0) {
            field_0x988 = gabi::at<sRotenAnmDat>(gabi::ea(field_0x988.get()) + 3);
            if (setAnmTbl(field_0x988)) {
                field_0x9C1 = field_0x9C1 | 1;
            }
        } else {
            setAnm(field_0x988->field_0x00, 0, 0.0f);
        }
    }
}
VERIFY(0x022D5174, &daNpcRoten_c::playAnm);

/* 022D43B0 (unnamed by the matcher; the name daNpcRoten_c::setAnm is on the constructor) */
void daNpcRoten_c::setAnm(u8 param_1, int param_2, f32 param_3) {
    WWHD_FUNC(0x022D43B0, void, this, param_1, param_2, param_3);
    f32 f = field_0x994;
    if (!(f < 0.0f)) {
        param_3 = f;
        field_0x994 = -1.0f;
    }
    J3DAnmTransform* pAnmRes = (J3DAnmTransform*)dComIfG_getObjectIDRes(
        arcname(mNpcNo), gabi::load<s32>(l_bck_ix_tbl + (mNpcNo * 10 + param_1) * 4));
    mpMorf->setAnm(pAnmRes, param_2, param_3, 1.0f, 0.0f, -1.0f, nullptr);
    field_0x9C0 = param_1;
    field_0x998 = param_2;
}
VERIFY(0x022D43B0, &daNpcRoten_c::setAnm);

/* 022D44D0 */
bool daNpcRoten_c::setAnmTbl(sRotenAnmDat* param_1) {
    WWHD_FUNC(0x022D44D0, bool, this, param_1);
    field_0x9C1 = field_0x9C1 & ~1;
    if (param_1->field_0x00 == 0xFF) {
        field_0x988 = nullptr;
        return true;
    }
    field_0x988 = param_1;
    s8 cnt = (s8)param_1->field_0x02;
    field_0x9C2 = cnt;
    u8 anm = param_1->field_0x00;
    if (cnt > 0) {
        setAnm(anm, 0, (f32)param_1->field_0x01);
    } else if (field_0x9C0 != anm || field_0x998 == 0) {
        setAnm(anm, 2, (f32)param_1->field_0x01);
    }
    return false;
}
VERIFY(0x022D44D0, &daNpcRoten_c::setAnmTbl);

/* 022D32FC */
BOOL daNpcRoten_c::isKoukanItem(u8 itemNo) {
    WWHD_FUNC(0x022D32FC, BOOL, this, itemNo);
    return 0x8C /* dItemNo_TOWN_FLOWER_e */ <= itemNo && itemNo <= 0x97 /* dItemNo_SHOP_GURU_STATUE_e */;
}
VERIFY(0x022D32FC, &daNpcRoten_c::isKoukanItem);

/* 022D62EC (unnamed by the matcher) */
BOOL daNpcRoten_c::isHaitatuItem(u8 itemNo) {
    WWHD_FUNC(0x022D62EC, BOOL, this, itemNo);
    return 0x8C /* dItemNo_TOWN_FLOWER_e */ <= itemNo && itemNo <= 0xA2 /* dItemNo_XXX_039_e */;
}
VERIFY(0x022D62EC, &daNpcRoten_c::isHaitatuItem);

/* 022D3314 */
BOOL daNpcRoten_c::isGetMap(u8 itemNo) {
    WWHD_FUNC(0x022D3314, BOOL, this, itemNo);
    if (mNpcNo == 1 && !dComIfGs_isEventBit(0x3E04) && itemNo == 0x97 /* dItemNo_SHOP_GURU_STATUE_e */) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022D3314, &daNpcRoten_c::isGetMap);

/* 022D3388 */
s16 daNpcRoten_c::XyEventCB(int i_itemBtn) {
    WWHD_FUNC(0x022D3388, s16, this, i_itemBtn);
    u8 itemNo = dComIfGp_getSelectItem(i_itemBtn);
    field_0x9BE = (u8)(itemNo - 0x8C);
    if (isKoukanItem(itemNo) && dComIfGs_getEventReg(save_dat(mNpcNo, 2)) < 3 && !isGetMap(itemNo)) {
        u8 btn = dComIfGp_event_getTalkXYBtn();
        if (btn == 1 /* dTalkBtn_X_e */) {
            mShownItemBtn = 0;
        } else if (btn == 2 /* dTalkBtn_Y_e */) {
            mShownItemBtn = 1;
        } else if (btn == 3 /* dTalkBtn_Z_e */) {
            mShownItemBtn = 2;
        } else {
            mShownItemBtn = 3;
        }
        field_0x9B2 = field_0x9B2 | 0xC000;
        return field_0x99E;
    }
    if (isGetMap(itemNo)) {
        field_0x9B2 = field_0x9B2 | 0xC000;
        return field_0x9A4;
    }
    return dComIfGp_evmng_getEventIdx_r(0x10021484); /* "DEFAULT_TALK_XY" */
}
VERIFY(0x022D3388, &daNpcRoten_c::XyEventCB);

/* 022D34DC daNpcRoten_XyEventCB (tail call) */
static s16 daNpcRoten_XyEventCB(void* i_this, int param_1) {
    WWHD_FUNC(0x022D34DC, s16, i_this, param_1);
    return ((daNpcRoten_c*)i_this)->XyEventCB(param_1);
}
VERIFY(0x022D34DC, daNpcRoten_XyEventCB);

/* 022D60DC */
u16 daNpcRoten_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022D60DC, u16, this, pMsgNo);
    u16 status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    u32 msg = msgMgr(); /* HD: the message manager (GameCube mpCurrMsg) */
    switch ((u32)*pMsgNo) {
    case 0x293D:
    case 0x29A1:
    case 0x2A05:
        status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    default:
        if (field_0x98C.get() != nullptr) {
            field_0x98C = gabi::at<be<u32>>(gabi::ea(field_0x98C.get()) + 4);
            u32 v = *field_0x98C;
            switch (v) {
            case 0:
                field_0x98C = nullptr;
                status = 0x10;
                break;
            case 1:
                if (gabi::load<u32>(msg + 0x948) != 0) { /* mSelectNum */
                    field_0x9B2 = field_0x9B2 | 8;
                }
                field_0x98C = nullptr;
                status = 0x10;
                break;
            case 2:
                if (gabi::load<u32>(msg + 0x948) == 0) {
                    u32 play = dComIfGp_ea();
                    if ((s32)dComIfGs_getRupee() < (s32)gabi::load<s16>(play + 0x5BA4) /* getMessageRupee */) {
                        field_0x9B2 = field_0x9B2 | 0x20;
                        field_0x98C = nullptr;
                        status = 0x10;
                        break;
                    }
                    s16 rupee = dComIfGp_getMessageRupee();
                    u32 play2 = dComIfGp_ea(); /* dComIfGp_setItemRupeeCount(-rupee) */
                    gabi::store<s32>(play2 + 0x5B48, gabi::load<s32>(play2 + 0x5B48) - rupee);
                    u8 temp = item_dat(mNpcNo, field_0x9BE);
                    u8 reg = dComIfGs_getEventReg(save_dat(mNpcNo, 2));
                    dComIfGs_setEventReg(save_dat(mNpcNo, 2), (u8)(reg + 1));
                    if (dComIfGs_isGetItemReserve(temp)) {
                        *pMsgNo = gabi::load<u32>(0x101C5CCC + mNpcNo * 4); /* l_msg_xy_koukan_end */
                        field_0x9B2 = field_0x9B2 | 0x40;
                    } else {
                        *pMsgNo = gabi::load<u32>(0x101C5CD8 + mNpcNo * 4); /* l_msg_xy_koukan_first */
                    }
                    dComIfGs_setReserveItemChange(mShownItemBtn, (u8)(temp + 0x8C));
                    field_0x98C = nullptr;
                } else {
                    field_0x9B2 = field_0x9B2 | 0x10;
                    field_0x98C = nullptr;
                    status = 0x10;
                }
                break;
            case 3:
                status = 0x10;
                break;
            default:
                *pMsgNo = v;
                break;
            }
        } else {
            status = 0x10;
        }
        break;
    }
    return status;
}
VERIFY(0x022D60DC, &daNpcRoten_c::next_msgStatus);

/* 022D6304 */
u32 daNpcRoten_c::getMsg() {
    WWHD_FUNC(0x022D6304, u32, this);
    u32 msgNo = 0;
    field_0x98C = nullptr;
    u16 flags = field_0x9B2;
    if (flags & 8) {
        field_0x9B2 = flags & ~8;
        msgNo = gabi::load<u32>(0x101C5CB4 + mNpcNo * 4); /* l_msg_xy_koukan_no */
    } else if (flags & 0x10) {
        field_0x9B2 = flags & ~0x10;
        msgNo = gabi::load<u32>(0x101C5CB4 + mNpcNo * 4); /* l_msg_xy_koukan_no */
    } else if (flags & 0x20) {
        field_0x9B2 = flags & ~0x20;
        msgNo = gabi::load<u32>(0x101C5CC0 + mNpcNo * 4); /* l_msg_xy_koukan_rupee */
    } else if (dComIfGp_event_chkTalkXY()) {
        u8 itemNo = dComIfGp_event_getPreItemNo();
        if (isGetMap(itemNo)) {
            field_0x98C = gabi::at<be<u32>>(0x101C5D20); /* l_msg_try_force */
            dComIfGs_setReserveItemEmpty();
            dComIfGs_onEventBit(0x3E04);
        } else if (dComIfGs_getEventReg(save_dat(mNpcNo, 2)) >= 3) {
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5AE0 + mNpcNo * 4)); /* l_msg_xy_exchange3 */
        } else if (!isHaitatuItem(itemNo)) {
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5AEC + mNpcNo * 4)); /* l_msg_xy_no_roten_item */
        } else if (!isKoukanItem(itemNo)) {
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5AF8 + mNpcNo * 4)); /* l_msg_xy_invalid_item */
        } else {
            u8 npcNo = mNpcNo;
            u8 idx = (u8)(itemNo - 0x8C);
            field_0x9BE = idx;
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5E50 + (npcNo * 12 + idx) * 4)); /* l_msg_xy_koukan_item */
            dComIfGs_onEventBit(save_dat(npcNo, 6));
        }
    } else {
        if (!dComIfGs_isEventBit(save_dat(mNpcNo, 0))) {
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5A44 + mNpcNo * 4)); /* l_msg_1st_talk */
            dComIfGs_onEventBit(save_dat(mNpcNo, 0));
        } else if (dComIfGs_getEventReg(save_dat(mNpcNo, 2)) >= 3) {
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5A50 + mNpcNo * 4)); /* l_msg_exchange3 */
        } else if (mNpcNo == 1 && dComIfGs_isEventBit(0x3E04)) {
            field_0x98C = gabi::at<be<u32>>(0x101C5464); /* l_msg_collect_map */
        } else if (dComIfGs_isEventBit(save_dat(mNpcNo, 6))) {
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5A80 + mNpcNo * 4)); /* l_msg_exchange */
        } else {
            field_0x98C = gabi::at<be<u32>>(gabi::load<u32>(0x101C5AB0 + mNpcNo * 4)); /* l_msg_etc */
        }
    }
    if (field_0x98C.get() != nullptr) {
        msgNo = *field_0x98C;
    }
    return msgNo;
}
VERIFY(0x022D6304, &daNpcRoten_c::getMsg);

/* 022D4094 (unnamed by the matcher) */
void daNpcRoten_c::setMessage(u32 msgNo) {
    WWHD_FUNC(0x022D4094, void, this, msgNo);
    mCurrMsgBsPcId = 0xFFFFFFFF;
    mCurrMsgNo = msgNo;
}
VERIFY(0x022D4094, &daNpcRoten_c::setMessage);

/* 022D4BE4 */
void daNpcRoten_c::setAnmFromMsgTag() {
    WWHD_FUNC(0x022D4BE4, void, this);
    u8 attr = dComIfGp_getMesgAnimeAttrInfo();
    /* HD: a table of the nine animation lists (.data 0x100215D0) */
    if (attr <= 8) {
        setAnmTbl(gabi::at<sRotenAnmDat>(gabi::load<u32>(0x100215D0 + attr * 4)));
    }
    dComIfGp_clearMesgAnimeAttrInfo();
}
VERIFY(0x022D4BE4, &daNpcRoten_c::setAnmFromMsgTag);

/* 022D669C: static initialisation of the translation unit */
static void __sinit_d_a_npc_roten_cpp() {
    WWHD_FUNC(0x022D669C, void);
    sinit_header_statics(0x104684A0, 0x101C602C);
}
VERIFY(0x022D669C, __sinit_d_a_npc_roten_cpp);

/* 022D6730: sead::SafeString deleting destructor (this TU's copy; vtable 0x10021278 slot +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x022D6730, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x022D6730, SafeString_dt);

/* 022D6754: daNpcRoten_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpcRoten_c_dt(daNpcRoten_c* i_this, s32 flags) {
    WWHD_FUNC(0x022D6754, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Cyl_dt(&i_this->mCyl2, 2);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x100212A0);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x100212B0);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022D6754, daNpcRoten_c_dt);

/* 022D6808: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x022D6808, void, (SafeString*)nullptr);
}
VERIFY(0x022D6808, SafeString_assureTerminationImpl);
