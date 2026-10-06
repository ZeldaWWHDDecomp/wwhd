/**
 * d_a_npc_bmcon1.cpp (WWHD)
 * NPC - Willi & Obli (Flight Control Platform)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_bmcon1.cpp) has only "Nonmatching" stubs for this unit: every function
 * here is written from the WWHD code (cking.rpx) and verified against it. Names follow the
 * GameCube symbols; the structure follows d_a_npc_people (the same author's townspeople), which
 * this actor closely resembles.
 */
#include "d/actor/d_a_npc_bmcon1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* set one argument register before a guest call that leaves it alone (for a register the harness
 * compares at a call, e.g. r6/r7 of getIDRes, whose value the original happens to set) */
static inline void bmcon_set_r(int n, u32 v) { gabi::cpu->r[n] = v; }
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift);

/* 0220435C */
static BOOL daNpc_Bmcon_nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0220435C, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        u32 model = gabi::load<u32>(0x104B462C); /* j3dSys.getModel() */
        daNpcBmcon_c* i_this = gabi::at<daNpcBmcon_c>(gabi::load<u32>(model + 0xB8)); /* getUserArea() */
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        J3DMtxBlock_bmcon* blk = gabi::at<J3DMtxBlock_bmcon>(gabi::load<u32>(model + 0x2C));
        blk->mFlags |= 0x10;
        PSMTXCopy(gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30), calc_mtx());
        if (jntNo == (u32)(s32)i_this->m_nec_jnt_num) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
            mDoMtx_ZrotM(calc_mtx(), -i_this->m_jnt.mAngles[0][0]);
        }
        if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            mDoMtx_ZrotM(calc_mtx(), -i_this->m_jnt.mAngles[1][0]);
        }
        if (jntNo == (u32)(s32)i_this->m_arm_L_jnt_num) {
            PSMTXCopy(calc_mtx(), &i_this->mArmLMtx);
        }
        if (jntNo == (u32)(s32)i_this->m_arm_R_jnt_num) {
            PSMTXCopy(calc_mtx(), &i_this->mArmRMtx);
        }
        /* model->setAnmMtx(jntNo, *calc_mtx) */
        blk = gabi::at<J3DMtxBlock_bmcon>(gabi::load<u32>(model + 0x2C));
        blk->mFlags |= 0x10;
        mtx_copy(gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30), calc_mtx());
        PSMTXCopy(calc_mtx(), j3dSys_mCurrentMtx());
    }
    return TRUE;
}
VERIFY(0x0220435C, daNpc_Bmcon_nodeCallBack);

/* 022044EC */
void daNpcBmcon_c::nodeArmControl(J3DNode* node, J3DModel* model) {
    WWHD_FUNC(0x022044EC, void, this, node, model);
    J3DJoint* joint = J3DNode_toJoint(node);
    u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
    J3DMtxBlock_bmcon* blk = gabi::at<J3DMtxBlock_bmcon>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10;
    PSMTXCopy(gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30), mDoMtx_stack_c::get());
    if (jntNo == (u32)(s32)m_armLloc_jnt_num) {
        PSMTXCopy(&mArmLMtx, j3dSys_mCurrentMtx());
        blk = gabi::at<J3DMtxBlock_bmcon>(gabi::load<u32>(gabi::ea(model) + 0x2C));
        blk->mFlags |= 0x10;
        mtx_copy(gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30), &mArmLMtx); /* setAnmMtx */
    }
    if (jntNo == (u32)(s32)m_armRloc_jnt_num) {
        PSMTXCopy(&mArmRMtx, j3dSys_mCurrentMtx());
        blk = gabi::at<J3DMtxBlock_bmcon>(gabi::load<u32>(gabi::ea(model) + 0x2C));
        blk->mFlags |= 0x10;
        mtx_copy(gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30), &mArmRMtx);
    }
}
VERIFY(0x022044EC, &daNpcBmcon_c::nodeArmControl);

/* 02204678 */
static BOOL daNpc_Arm_nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02204678, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        u32 model = gabi::load<u32>(0x104B462C); /* j3dSys.getModel() */
        daNpcBmcon_c* i_this = gabi::at<daNpcBmcon_c>(gabi::load<u32>(model + 0xB8));
        i_this->nodeArmControl(node, gabi::at<J3DModel>(model));
    }
    return TRUE;
}
VERIFY(0x02204678, daNpc_Arm_nodeCallBack);

/* 022046B4 */
BOOL daNpcBmcon_c::createHeap() {
    WWHD_FUNC(0x022046B4, BOOL, this);
    u32 npc = mNpcNo;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(bmcon_arcname(npc), gabi::load<s32>(BMCON_l_bmd_ix_tbl + npc * 4));
    u32 anm = mAnmIdx;
    J3DAnmTransform* pAnm = (J3DAnmTransform*)dComIfG_getObjectIDRes(bmcon_arcname(mNpcNo), gabi::load<s32>(BMCON_l_bck_ix_tbl + anm * 4));
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, pAnm, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr,
                                    0x00080000, 0x11000002);
    npc = mNpcNo;
    J3DModelData* armModelData = (J3DModelData*)dComIfG_getObjectIDRes(bmcon_arcname(npc), gabi::load<s32>(BMCON_l_arm_bmd_ix_tbl + npc * 4));
    anm = mAnmIdx;
    J3DAnmTransform* pArmAnm = (J3DAnmTransform*)dComIfG_getObjectIDRes(bmcon_arcname(mNpcNo), gabi::load<s32>(BMCON_l_arm_bck_ix_tbl + anm * 4));
    mpArmMorf = mDoExt_McaMorf::create(nullptr, armModelData, nullptr, nullptr, pArmAnm, 2, 1.0f, 0, -1, 1, nullptr,
                                       0x00080000, 0x11000002);
    if (mpArmMorf.get() == nullptr || mpArmMorf->getModel() == nullptr) {
        return false;
    }
    npc = mNpcNo;
    J3DModelData* etcModelData = (J3DModelData*)dComIfG_getObjectIDRes(bmcon_arcname(npc), gabi::load<s32>(BMCON_l_etc_bmd_ix_tbl + npc * 4));
    mpEtcModel = mDoExt_J3DModel__create(etcModelData, 0x80000, 0x37441422);
    if (mpEtcModel.get() == nullptr) {
        return false;
    }
    m_jnt.mHeadJntNum = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10017B00) /* "head" */);
    if (!(m_jnt.mHeadJntNum >= 0)) {
        JUT_ASSERT_fail(STR(0x10017B20), 0x3F4, STR(0x10017B34));
    }
    m_jnt.mBackboneJntNum = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10017B50) /* "backbone" */);
    if (!(m_jnt.mBackboneJntNum >= 0)) {
        JUT_ASSERT_fail(STR(0x10017B20), 0x3F8, STR(0x10017B5C));
    }
    m_nec_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10017B08) /* "neck" */);
    if (!(m_nec_jnt_num >= 0)) {
        JUT_ASSERT_fail(STR(0x10017B20), 0x3FF, STR(0x10017B7C));
    }
    m_arm_L_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10017B10) /* "armL" */);
    if (!(m_arm_L_jnt_num >= 0)) {
        JUT_ASSERT_fail(STR(0x10017B20), 0x403, STR(0x10017B90));
    }
    m_arm_R_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10017B18) /* "armR" */);
    if (!(m_arm_R_jnt_num >= 0)) {
        JUT_ASSERT_fail(STR(0x10017B20), 0x405, STR(0x10017BA8));
    }
    m_armLloc_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(armModelData), STR(0x10017AF0) /* "armLloc" */);
    if (!(m_armLloc_jnt_num >= 0)) {
        JUT_ASSERT_fail(STR(0x10017B20), 0x409, STR(0x10017BC0));
    }
    m_armRloc_jnt_num = JUTNameTab_getIndex(J3DModelData_getJointName(armModelData), STR(0x10017AF8) /* "armRloc" */);
    if (!(m_armRloc_jnt_num >= 0)) {
        JUT_ASSERT_fail(STR(0x10017B20), 0x40B, STR(0x10017BD8));
    }
    for (u16 i = 0; i < J3DModelData_getJointNum_l(modelData); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum || i == (u32)(s32)m_nec_jnt_num ||
            i == (u32)(s32)m_arm_L_jnt_num || i == (u32)(s32)m_arm_R_jnt_num) {
            J3DModelData_setJointCallBack_l(modelData, i, 0x0220435C /* daNpc_Bmcon_nodeCallBack */);
        }
    }
    if (mpMorf->getModel() != nullptr) {
        gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    }
    for (u16 i = 0; i < J3DModelData_getJointNum_l(armModelData); i++) {
        if (i == (u32)(s32)m_armLloc_jnt_num || i == (u32)(s32)m_armRloc_jnt_num) {
            J3DModelData_setJointCallBack_l(armModelData, i, 0x02204678 /* daNpc_Arm_nodeCallBack */);
        }
    }
    gabi::store<u32>(gabi::ea(mpArmMorf->getModel()) + 0xB8, gabi::ea(this));
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    return true;
}
VERIFY(0x022046B4, &daNpcBmcon_c::createHeap);

/* 02204CA0 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02204CA0, BOOL, i_this);
    return static_cast<daNpcBmcon_c*>(i_this)->createHeap();
}
VERIFY(0x02204CA0, CheckCreateHeap);

/* 02204CA4 */
u8 daNpcBmcon_c::getPrmNpcNo() {
    WWHD_FUNC(0x02204CA4, u8, this);
    if (0 <= argument && argument < 2) {
        return argument;
    }
    return 0;
}
VERIFY(0x02204CA4, &daNpcBmcon_c::getPrmNpcNo);

/* 02204CC0 daNpcBmcon_c::daNpcBmcon_c (HD: allocates when this == NULL) */
static daNpcBmcon_c* daNpcBmcon_c_ct(daNpcBmcon_c* i_this) {
    WWHD_FUNC(0x02204CC0, daNpcBmcon_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpcBmcon_c*)operator_new(0x93C);
        if (i_this == nullptr) {
            return i_this;
        }
    }
    fopNpc_npc_c_ct(i_this);
    i_this->__vtbl = BMCON_VTBL;
    mDoExt_btpAnm_ct(i_this->mBtpAnm);
    i_this->mNpcNo = i_this->getPrmNpcNo();
    i_this->mbHeadOnly = 1;
    i_this->mMorfOverride = -1.0f;
    i_this->field_0x914 = 0;
    i_this->field_0x8F4 = 0.0f;
    i_this->mLookMode = 0;
    i_this->mExeMode = 0;
    i_this->mAnmIdx = 1;
    i_this->mResFlag = 0;
    i_this->mHomeAngleY = i_this->home.angle.y;
    i_this->field_0x90A = 0;
    i_this->field_0x937 = 0;
    i_this->field_0x938 = 1;
    return i_this;
}
VERIFY(0x02204CC0, daNpcBmcon_c_ct);

/* 02204D74 */
static cPhs_State phase_1(daNpcBmcon_c* i_this) {
    WWHD_FUNC(0x02204D74, cPhs_State, i_this);
    /* fopAcM_ct(i_this, daNpcBmcon_c) */
    if (!(i_this->actor_condition & fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            daNpcBmcon_c_ct(i_this);
        }
        i_this->actor_condition = i_this->actor_condition | fopAcCnd_INIT_e;
    }
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    if (link == nullptr || fpcM_IsCreating(fopAcM_GetID(link))) {
        return cPhs_INIT_e;
    }
    i_this->mResFlag = 1;
    return cPhs_NEXT_e;
}
VERIFY(0x02204D74, phase_1);

/* 02204E14 */
u8 daNpcBmcon_c::getPrmRailID() {
    WWHD_FUNC(0x02204E14, u8, this);
    return daObj_PrmAbstract(this, PRM_RAIL_ID_W, PRM_RAIL_ID_S);
}
VERIFY(0x02204E14, &daNpcBmcon_c::getPrmRailID);

/* 02204E40 */
void daNpcBmcon_c::setAnm(u32 anmIdx, int loopMode, f32 morf) {
    WWHD_FUNC(0x02204E40, void, this, anmIdx, loopMode, morf);
    f32 ov = mMorfOverride;
    u32 npc = mNpcNo;
    if (!(ov < 0.0f)) {
        morf = ov;
        mMorfOverride = -1.0f;
    }
    J3DAnmTransform* pAnm = (J3DAnmTransform*)dComIfG_getObjectIDRes(bmcon_arcname(npc), gabi::load<s32>(BMCON_l_bck_ix_tbl + anmIdx * 4));
    mpMorf->setAnm(pAnm, loopMode, morf, 1.0f, 0.0f, -1.0f, nullptr);
    J3DAnmTransform* pArmAnm =
        (J3DAnmTransform*)dComIfG_getObjectIDRes(bmcon_arcname(mNpcNo), gabi::load<s32>(BMCON_l_arm_bck_ix_tbl + anmIdx * 4));
    mpArmMorf->setAnm(pArmAnm, loopMode, morf, 1.0f, 0.0f, -1.0f, nullptr);
    mAnmIdx = anmIdx;
}
VERIFY(0x02204E40, &daNpcBmcon_c::setAnm);

/* 02204FA8 */
bool daNpcBmcon_c::setAnmTbl(sBmconAnmDat* dat) {
    WWHD_FUNC(0x02204FA8, bool, this, dat);
    if (dat->field_0x00 == 0xFF) {
        mpAnmDat = nullptr;
        return true;
    }
    mpAnmDat = dat;
    mAnmLoop = dat->field_0x02;
    if (mAnmLoop > 0) {
        setAnm(dat->field_0x00, 0, (f32)dat->field_0x01);
    } else if (mAnmIdx != dat->field_0x00) {
        setAnm(dat->field_0x00, 2, (f32)dat->field_0x01);
    }
    return false;
}
VERIFY(0x02204FA8, &daNpcBmcon_c::setAnmTbl);

/* 02205074 */
s16 daNpcBmcon_c::XyCheckCB(int) {
    WWHD_FUNC(0x02205074, s16, this, 0);
    return 0;
}
VERIFY(0x02205074, &daNpcBmcon_c::XyCheckCB);

/* 0220507C */
static s16 daNpcBmcon_XyCheckCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x0220507C, s16, i_this, i_itemBtn);
    return static_cast<daNpcBmcon_c*>(i_this)->XyCheckCB(i_itemBtn);
}
VERIFY(0x0220507C, daNpcBmcon_XyCheckCB);

/* 02205080 */
void daNpcBmcon_c::setMtx() {
    WWHD_FUNC(0x02205080, void, this);
    J3DModel* model = mpMorf->getModel();
    cXyz* base_scale = gabi::at<cXyz>(gabi::ea(model) + 0xBC); /* setBaseScale(scale) */
    f32 sz = scale.z, sx = scale.x, sy = scale.y;
    base_scale->x = sx;
    base_scale->y = sy;
    base_scale->z = sz;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}
VERIFY(0x02205080, &daNpcBmcon_c::setMtx);

/* 02205160 */
void daNpcBmcon_c::setCollision(dCcD_Cyl* cyl, cXyz* center, f32 radius, f32 height) {
    WWHD_FUNC(0x02205160, void, this, cyl, center, radius, height);
    cyl->SetC(center);
    cyl->SetR(radius);
    cyl->SetH(height);
    dComIfG_Ccsp_Set(cyl);
}
VERIFY(0x02205160, &daNpcBmcon_c::setCollision);

static inline s16 bmcon_getEventIdx(u32 name) { return dComIfGp_evmng_getEventIdx(STR(name), 0xFF); }

/* 022051F0 */
cPhs_State daNpcBmcon_c::createInit() {
    WWHD_FUNC(0x022051F0, cPhs_State, this);
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
        current.pos.copy(*point);
        old.pos.copy(*point);
        dNpc_PathRun_incIdxLoop(&mPathRun);
        field_0x90A = 1;
        temp = 0xFE;
    }
    gravity = -9.0f;
    setAnmTbl(gabi::at<sBmconAnmDat>(gabi::load<u32>(BMCON_l_npc_anm_tbl + mAnmIdx * 4)));
    switch (mNpcNo) {
    case NPC_BMCON1:
        mEventIdx[0] = bmcon_getEventIdx(0x10017C24); /* "BMCON_RESULT" */
        mEventIdx[1] = bmcon_getEventIdx(0x10017C34); /* "BMCON_GET_ITEM" */
        if (dComIfGs_isTmpBit(0x0210)) {
            dComIfGs_offTmpBit(0x0210);
            fopAc_ac_c* link = dComIfGp_getLinkPlayer();
            fopAcM_orderChangeEventId(link, this, mEventIdx[0], 0, 0xFFFF);
            gabi::store<u32>(gabi::ea(link) + 0x3BC, gabi::load<u32>(gabi::ea(link) + 0x3BC) | 0x80000); /* HD: player flag */
        }
        break;
    case NPC_BMCON2:
        mEventIdx[0] = bmcon_getEventIdx(0x10017C18); /* "BMCON_END" */
        mEventIdx[1] = bmcon_getEventIdx(0x10017C44); /* "BMCON_END2" */
        break;
    default:
        break;
    }
    gabi::store<u32>(gabi::ea(this) + 0x104, 0x0220507C); /* eventInfo.setXyCheckCB(daNpcBmcon_XyCheckCB) */
    mEventCut.setActorInfo2(gabi::at<const char>(gabi::load<u32>(BMCON_l_npc_staff_id + mNpcNo * 4)), this);
    mTurnStep = 0;
    mbAttention = 0;
    mTalk = 0;
    mbFar = 0;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xA9); /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xA9); /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    daNpcBmcon_c__l_npc_dat* dat = bmcon_npc_dat(mNpcNo);
    m_jnt.setParam(dat->field_0x04, dat->field_0x06, dat->field_0x0C, dat->field_0x0E, dat->field_0x00, dat->field_0x02,
                   dat->field_0x08, dat->field_0x0A, dat->field_0x10);
    dat = bmcon_npc_dat(mNpcNo);
    mbTurnOnLook = dat->field_0x4A;
    mbLookPlayer = dat->field_0x4B;
    mAttDist = dat->field_0x20;
    mAttAngle = dat->field_0x28;
    mObjAcch.CrrPos(dComIfG_Bgsp());
    f32 gnd = gabi::load<f32>(gabi::ea(&mObjAcch) + 0x94); /* GetGroundH() */
    if (-1000000000.0f /* -G_CM3D_F_INF */ != gnd) {
        current.pos.y = gnd;
        home.pos.y = gnd;
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
    setCollision(&mCyl, center.get(), bmcon_npc_dat(mNpcNo)->field_0x30, 150.0f);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x022051F0, &daNpcBmcon_c::createInit);

/* 02205630 */
static cPhs_State phase_2(daNpcBmcon_c* i_this) {
    WWHD_FUNC(0x02205630, cPhs_State, i_this);
    cPhs_State rt = dComIfG_resLoad(i_this->getPhaseP(), bmcon_arcname(i_this->getNpcNo()));
    if (rt == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, 0x02204CA0 /* CheckCreateHeap */, 0)) {
            return i_this->createInit();
        }
        i_this->mpMorf = nullptr;
        return cPhs_ERROR_e;
    }
    return rt;
}
VERIFY(0x02205630, phase_2);

/* 022056C4 */
cPhs_State daNpcBmcon_c::_create() {
    WWHD_FUNC(0x022056C4, cPhs_State, this);
    /* static cPhs__Handler l_method[] = {phase_1, phase_2, NULL} */
    return dComLbG_PhaseHandler(&mPhase2, BMCON_l_method, this);
}
VERIFY(0x022056C4, &daNpcBmcon_c::_create);

/* 022056D8 */
static cPhs_State daNpc_BmconCreate(void* i_this) {
    WWHD_FUNC(0x022056D8, cPhs_State, i_this);
    return static_cast<daNpcBmcon_c*>(i_this)->_create();
}
VERIFY(0x022056D8, daNpc_BmconCreate);

/* 022056DC */
bool daNpcBmcon_c::_delete() {
    WWHD_FUNC(0x022056DC, bool, this);
    if (mResFlag) {
        dComIfG_resDelete(&mPhase, bmcon_arcname(mNpcNo));
    }
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return true;
}
VERIFY(0x022056DC, &daNpcBmcon_c::_delete);

/* 0220574C */
static BOOL daNpc_BmconDelete(void* i_this) {
    WWHD_FUNC(0x0220574C, BOOL, i_this);
    return static_cast<daNpcBmcon_c*>(i_this)->_delete();
}
VERIFY(0x0220574C, daNpc_BmconDelete);

/* 02205750 */
void daNpcBmcon_c::chkAttention() {
    WWHD_FUNC(0x02205750, void, this);
    mbFar = 0;
    if (mEventCut.mbAttention) { /* getAttnFlag() */
        mLookAtPos.x = mEventCut.mPos.x;
        mLookAtPos.z = mEventCut.mPos.z;
        mLookAtPos.y = mEventCut.mPos.y;
        mLookMode = 1;
        if (mbTurnOnLook != 0) {
            mbHeadOnly = false;
            m_jnt.mbTrn = 1;
        } else {
            mbHeadOnly = true;
        }
    } else {
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        gabi::Local<cXyz> pos;
        pos->z = current.pos.z;
        pos->y = current.pos.y;
        pos->x = current.pos.x;
        gabi::Local<cXyz> ppos;
        ppos->x = player->current.pos.x;
        ppos->y = player->current.pos.y;
        f32 dist_max = mAttDist;
        ppos->z = player->current.pos.z;
        s32 ang_max = mAttAngle;
        gabi::Local<be<f32>> dist_p;
        gabi::Local<be<s16>> ang_p;
        dNpc_calc_DisXZ_AngY(pos.get(), ppos.get(), dist_p.get(), ang_p.get());
        u8 att = mbAttention;
        if (att != 0) {
            dist_max += 40.0f;
            ang_max += 0x71C;
        }
        s16 ang = (s16)(*ang_p - shape_angle.y);
        *ang_p = ang;
        f32 dist = *dist_p;
        s32 abs_ang = ang < 0 ? -ang : ang;
        if (dist_max > dist && ang_max > abs_ang && player->current.pos.y > current.pos.y - 200.0f) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos_l(eye.get(), bmcon_npc_dat(mNpcNo)->field_0x14);
            mbHeadOnly = (mbTurnOnLook == 0);
            mLookAtPos.copy(*eye);
            mLookMode = 1;
            if (mbLookPlayer == 0) {
                mbHeadOnly = false;
                mTargetAngleY = mHomeAngleY;
                m_jnt.mbTrn = 1;
                mLookMode = 2;
            }
        } else {
            u32 npc = mNpcNo;
            if (att == 1) {
                mbAttention = 0;
                mLookTimer = bmcon_npc_dat(mNpcNo)->field_0x48;
            }
            daNpcBmcon_c__l_npc_dat* dat = bmcon_npc_dat(npc);
            if (dat->field_0x24 > dist) {
                gabi::Local<cXyz> eye;
                dNpc_playerEyePos_l(eye.get(), dat->field_0x14);
                mbHeadOnly = (mbTurnOnLook == 0);
                mLookAtPos.copy(*eye);
                mLookMode = 1;
                if (mbLookPlayer == 0) {
                    mbHeadOnly = false;
                    mTargetAngleY = mHomeAngleY;
                    m_jnt.mbTrn = 1;
                    mLookMode = 2;
                }
                mbFar = 1;
                mTurnSpeed = bmcon_npc_dat(mNpcNo)->field_0x2A;
                return;
            }
            s16 t = mLookTimer;
            mLookMode = 0;
            if (t != 0) {
                mLookTimer = t - 1;
                mTurnSpeed = bmcon_npc_dat(mNpcNo)->field_0x2A;
                return;
            }
            mbHeadOnly = false;
            mTargetAngleY = mHomeAngleY;
            mLookMode = 2;
            m_jnt.mbTrn = 1;
            mTurnSpeed = bmcon_npc_dat(mNpcNo)->field_0x2A;
            return;
        }
    }
    if (mbAttention == 0) {
        mbAttention = 1;
    }
    mTurnSpeed = bmcon_npc_dat(mNpcNo)->field_0x2A;
}
VERIFY(0x02205750, &daNpcBmcon_c::chkAttention);

/* GHS pointer-to-member call without arguments, returning r3 */
static inline s32 bmcon_pmf_call0(void* self, u32 pmf) {
    s16 i = gabi::load<s16>(pmf + 2);
    u32 thisp = gabi::ea(self) + (s32)gabi::load<s16>(pmf);
    if (i < 0) {
        return gabi::call_ptr<s32>(gabi::load<u32>(pmf + 4), thisp);
    }
    s16 voff = gabi::load<s16>(pmf + 6);
    u32 vt = gabi::load<u32>(thisp + voff);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + i * 8 + 4), thisp);
}
enum : u32 { BMCON_l_execute_init = 0x101BD100, BMCON_l_execute = 0x101BD120 };

/* 02205A6C */
void daNpcBmcon_c::executeSetMode(u32 proc) {
    WWHD_FUNC(0x02205A6C, void, this, proc);
    field_0x8F4 = 0.0f;
    mExeMode = bmcon_pmf_call0(this, BMCON_l_execute_init + proc * 8); /* (this->*l_execute_init[proc])() */
}
VERIFY(0x02205A6C, &daNpcBmcon_c::executeSetMode);

/* 02205AF4 */
void daNpcBmcon_c::checkOrder() {
    WWHD_FUNC(0x02205AF4, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd == 2 /* dEvtCmd_INDEMO_e */) {
        if ((dComIfGp_evmng_startCheck(mEventIdx[0]) && mOrderEventNum == 3) ||
            (dComIfGp_evmng_startCheck(mEventIdx[1]) && mOrderEventNum == 4) ||
            (dComIfGp_evmng_startCheck(mEventIdx[1]) && mOrderEventNum == 6)) {
            mOrderEventNum = 0;
        }
    } else if (cmd == 1 /* dEvtCmd_INTALK_e */) {
        if (mOrderEventNum == 2 || mOrderEventNum == 1) {
            mTalk = 1;
            executeSetMode(1);
        }
    }
}
VERIFY(0x02205AF4, &daNpcBmcon_c::checkOrder);

/* 02205BF4 */
BOOL daNpcBmcon_c::chkEndEvent() {
    WWHD_FUNC(0x02205BF4, BOOL, this);
    switch (mNpcNo) {
    case NPC_BMCON1:
        if (dComIfGp_evmng_endCheck(mEventIdx[0]) && dComIfGs_getTmpReg(0xF903) != 0) {
            dComIfGp_event_reset();
            mbLookPlayer = bmcon_npc_dat(mNpcNo)->field_0x4B;
            return TRUE;
        }
        if (dComIfGp_evmng_endCheck(mEventIdx[1])) {
            dComIfGp_event_reset();
            mbLookPlayer = bmcon_npc_dat(mNpcNo)->field_0x4B;
            return TRUE;
        }
        break;
    case NPC_BMCON2:
        if (dComIfGp_evmng_endCheck(mEventIdx[0]) || dComIfGp_evmng_endCheck(mEventIdx[1])) {
            dComIfGp_event_reset();
            mbLookPlayer = bmcon_npc_dat(mNpcNo)->field_0x4B;
            return TRUE;
        }
        break;
    default:
        return FALSE;
    }
    return FALSE;
}
VERIFY(0x02205BF4, &daNpcBmcon_c::chkEndEvent);

/* 02205DAC */
s16 daNpcBmcon_c::getFlyDistNow() {
    WWHD_FUNC(0x02205DAC, s16, this);
    u8 lo = dComIfGs_getTmpReg(0xFBFF);
    return (s16)(lo + (dComIfGs_getTmpReg(0xFAFF) << 8));
}
VERIFY(0x02205DAC, &daNpcBmcon_c::getFlyDistNow);

/* 02205E14 */
void daNpcBmcon_c::setMessage(u32 msgNo) {
    WWHD_FUNC(0x02205E14, void, this, msgNo);
    mCurrMsgNo = msgNo;
}
VERIFY(0x02205E14, &daNpcBmcon_c::setMessage);

/* HD: daNpcBmcon_c's message functions are virtual (vtable at +0xB4): slot 0x14
 * next_msgStatus, 0x1C getMsg, 0x24 anmAtr(u16) */
static inline u32 bmcon_vfn(void* self, u32 slot) { return gabi::load<u32>(gabi::load<u32>(gabi::ea(self) + 0xB4) + slot); }
static inline u32 bmcon_v_next_msgStatus(daNpcBmcon_c* self, be<u32>* msgNo) { return gabi::call_ptr<u32>(bmcon_vfn(self, 0x14), self, msgNo); }
static inline u32 bmcon_v_getMsg(daNpcBmcon_c* self) { return gabi::call_ptr<u32>(bmcon_vfn(self, 0x1C), self); }
static inline void bmcon_v_anmAtr(daNpcBmcon_c* self, u16 status) { gabi::call_ptr(bmcon_vfn(self, 0x24), self, status); }
/* HD message manager (*(0x101F4B5C)) methods: 025F795C status, 025F74D0 setStatus,
 * 025F7DB0 messageSet(msgNo, cXyz* pos) */
static inline u32 msgMng_getStatus(u32 mgr) { return gabi::call<u32>(0x025F795C, mgr); }
static inline void msgMng_setStatus(u32 mgr, u32 st) { gabi::call(0x025F74D0, mgr, st); }
static inline u32 msgMng_messageSet(u32 mgr, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mgr, msgNo, pos); }
/* 025E1988 HD: mDoAud_seStart(id) without a position; 025E18EC HD: an audio request by id (BGM) */
static inline void mDoAud_seStart_id(u32 id) { gabi::call(0x025E1988, id); }
static inline void mDoAud_bgmStart_id(u32 id) { gabi::call(0x025E18EC, id); }
/* 025E1904 mDoAud_bgmStop(frames) */
static inline void mDoAud_bgmStop(u32 frames) { gabi::call(0x025E1904, frames); }
/* 025D7DEC fopAcM_createItemForPresentDemo(pos, itemNo, argFlag, itemBitNo, roomNo, angle, scale) */
static inline u32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return gabi::call<u32>(0x025D7DEC, pos, itemNo, flag, bitNo, roomNo, angle, scale);
}
static inline void dComIfGp_event_setItemPartnerId(u32 id) { gabi::store<u32>(dComIfGp_ea() + 0x52A0, id); }
/* play + 0x5BA0: a message number argument (the fly distance) */
static inline void dComIfGp_setMessageCountNumber(s16 v) { gabi::store<s16>(dComIfGp_ea() + 0x5BA0, v); }
static inline void bmcon_startShock(s32 strength, s32 flags) {
    gabi::Local<cXyz> pos;
    dVibration_c* vib = dComIfGp_getVibration();
    pos->x = 0.0f;
    pos->y = 1.0f;
    pos->z = 0.0f;
    gabi::call(0x025CB374, vib, strength, flags, pos.get()); /* StartShock(strength, flags, cXyz(0, 1, 0)) */
}
enum : u32 {
    BMCON_l_msg_result_cleared = 0x101BD1A0, /* u32[] message list */
    BMCON_l_msg_result_first = 0x101BD0D8,   /* u32[] message list */
    BMCON_l_msg_dist = 0x101BD18C,           /* u32[5] by fly distance */
};

/* 02205E1C */
void daNpcBmcon_c::eventMesSetInit(int staffIdx) {
    WWHD_FUNC(0x02205E1C, void, this, staffIdx);
    be<u32>* pMsgNo = (be<u32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, STR(0x10017C54) /* "MsgNo" */);
    if (pMsgNo == nullptr) {
        field_0x8F0 = field_0x8F0 + 4;
        setMessage(gabi::load<u32>(field_0x8F0));
        return;
    }
    field_0x8F0 = 0;
    switch (*pMsgNo) {
    case 0:
        setMessage(bmcon_v_getMsg(this));
        break;
    case 1:
        switch (field_0x938) {
        case 0: {
            s16 dist = field_0x916;
            dComIfGp_setMessageCountNumber(dist);
            setMessage(0x2AB2);
            bmcon_startShock(8, 1);
            bmcon_startShock(4, 0x1E);
            mDoAud_seStart_id(0x902);
            break;
        }
        case 1: {
            s16 dist = field_0x916;
            dComIfGp_setMessageCountNumber(dist);
            setMessage(0x2AB3);
            bmcon_startShock(8, 1);
            bmcon_startShock(4, 0x1E);
            mDoAud_seStart_id(0x901);
            break;
        }
        case 2:
            setMessage(0x2AB0);
            break;
        case 3:
            setMessage(0x2AB5);
            break;
        }
        dComIfGs_onEventBit(0x2901);
        dComIfGs_onTmpBit(0x0210);
        break;
    case 2:
        switch (field_0x938) {
        case 0:
            setMessage(0x2AB4);
            break;
        case 1: {
            s16 dist = field_0x916;
            dComIfGp_setMessageCountNumber(dist);
            setMessage(0x2AA6);
            break;
        }
        case 2:
            setMessage(0x2AB0);
            break;
        case 3:
            setMessage(0x2AB5);
            break;
        default:
            return;
        }
        break;
    case 10:
        switch (dComIfGs_getTmpReg(0xF903)) {
        case 0:
            if (dComIfGs_isEventBit(0x2B40)) {
                field_0x8F0 = BMCON_l_msg_result_cleared;
                setMessage(gabi::load<u32>(BMCON_l_msg_result_cleared));
            } else {
                dComIfGs_onEventBit(0x2B40);
                field_0x8F0 = BMCON_l_msg_result_first;
                setMessage(gabi::load<u32>(BMCON_l_msg_result_first));
            }
            return;
        case 1:
            if (dComIfGs_isEventBit(0x2B40)) {
                setMessage(0x2AAF);
            } else {
                int idx = getFlyDistNow() * 6 / 256;
                if (idx > 4) {
                    idx = 4;
                }
                setMessage(gabi::load<u32>(BMCON_l_msg_dist + idx * 4));
            }
            break;
        case 2:
            setMessage(0x2AB1);
            break;
        case 3:
            setMessage(0x2AB6);
            break;
        }
        break;
    default:
        setMessage(*pMsgNo);
        break;
    }
    if (field_0x8F0 != 0) {
        setMessage(gabi::load<u32>(field_0x8F0));
    }
}
VERIFY(0x02205E1C, &daNpcBmcon_c::eventMesSetInit);

/* 02206340 */
void daNpcBmcon_c::eventGetItemInit() {
    WWHD_FUNC(0x02206340, void, this);
    u32 id = fopAcM_createItemForPresentDemo(&current.pos, field_0x900, 0, -1, current.roomNo, nullptr, nullptr);
    if (id != fpcM_ERROR_PROCESS_ID_e) {
        dComIfGp_event_setItemPartnerId(id);
    }
}
VERIFY(0x02206340, &daNpcBmcon_c::eventGetItemInit);

/* 0220639C */
void daNpcBmcon_c::chkMsg() {
    WWHD_FUNC(0x0220639C, void, this);
    switch (mCurrMsgNo) {
    case 0x2AB2:
        mDoAud_bgmStart_id(0x80000051);
        break;
    case 0x2AB3:
        mDoAud_bgmStart_id(0x80000052);
        break;
    }
}
VERIFY(0x0220639C, &daNpcBmcon_c::chkMsg);

/* 022063CC */
u16 daNpcBmcon_c::talk2(int param_1) {
    WWHD_FUNC(0x022063CC, u16, this, param_1);
    u16 status = 0xFF;
    /* HD: the message (GameCube mpCurrMsg) is the message manager, read once */
    u32 mgr = bmcon_msgManager();
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        if (param_1 == 1) {
            mCurrMsgNo = bmcon_v_getMsg(this);
        }
        mCurrMsgBsPcId = msgMng_messageSet(mgr, mCurrMsgNo, &eyePos);
        if (mCurrMsgBsPcId != fpcM_ERROR_PROCESS_ID_e) {
            mbHasMsg = 0;
            field_0x912 = 0xFFFF;
        }
    } else if (mbHasMsg) {
        status = (u16)msgMng_getStatus(mgr);
        switch (status) {
        case 0xE: /* fopMsgStts_MSG_DISPLAYED_e */
            msgMng_setStatus(mgr, bmcon_v_next_msgStatus(this, &mCurrMsgNo));
            if (msgMng_getStatus(mgr) == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
                msgMng_messageSet(mgr, mCurrMsgNo, nullptr);
            }
            break;
        case 6: /* fopMsgStts_MSG_TYPING_e */
            if (field_0x912 == 0xF || field_0x912 == 2 /* fopMsgStts_BOX_OPENING_e */) {
                chkMsg();
            }
            break;
        case 0x12: /* fopMsgStts_BOX_CLOSED_e */
            msgMng_setStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
            mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
            break;
        }
        field_0x912 = status;
        bmcon_v_anmAtr(this, status);
    } else {
        /* HD: mpCurrMsg = fopMsgM_SearchByID(mCurrMsgBsPcId) is a flag */
        mbHasMsg = 1;
    }
    return status;
}
VERIFY(0x022063CC, &daNpcBmcon_c::talk2);

/* 022065C4 */
bool daNpcBmcon_c::eventMesSet() {
    WWHD_FUNC(0x022065C4, bool, this);
    u16 status = talk2(0);
    if (status == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        u8 flag = field_0x92C;
        if (flag & 1) {
            field_0x92C = flag & ~1;
            field_0x900 = 7;
            mOrderEventNum = 6;
        } else if (flag & 2) {
            field_0x92C = flag & ~2;
            field_0x900 = 5;
            mOrderEventNum = 6;
        }
    }
    return status == 0x12;
}
VERIFY(0x022065C4, &daNpcBmcon_c::eventMesSet);

/* 02206650 */
void daNpcBmcon_c::privateCut() {
    WWHD_FUNC(0x02206650, void, this);
    const char* staff = gabi::at<const char>(gabi::load<u32>(BMCON_l_npc_staff_id + mNpcNo * 4));
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(staff, nullptr, 0);
    if (staffIdx == -1) {
        return;
    }
    /* static char* cut_name_tbl[] = {"MES_SET", "GET_ITEM"} (.data 0x101BD258) */
    s8 act = dComIfGp_evmng_getMyActIdx(staffIdx, 0x101BD258, 2, TRUE, 0);
    field_0x930 = act;
    dEvent_manager_c* mgr = dComIfGp_getPEvtManager();
    if (act == -1) {
        gabi::call(0x02543280, mgr, staffIdx); /* cutEnd(staffIdx) */
        return;
    }
    if (gabi::call<BOOL>(0x025447C8, mgr, staffIdx) /* getIsAddvance(staffIdx) */) {
        switch ((s8)field_0x930) {
        case 0:
            eventMesSetInit(staffIdx);
            break;
        case 1:
            eventGetItemInit();
            break;
        }
    }
    switch ((s8)field_0x930) {
    case 0:
        if (!eventMesSet()) {
            return;
        }
        break;
    }
    dComIfGp_evmng_cutEnd(staffIdx);
}
VERIFY(0x02206650, &daNpcBmcon_c::privateCut);

/* 0220677C */
void daNpcBmcon_c::setAnmFromMsgTag() {
    WWHD_FUNC(0x0220677C, void, this);
    switch (gabi::load<u8>(dComIfGp_ea() + 0x5BC5) /* dComIfGp_getMesgAnimeTagInfo() */) {
    case 0:
        setAnmTbl(gabi::at<sBmconAnmDat>(0x101BD0A0));
        break;
    case 5:
        setAnmTbl(gabi::at<sBmconAnmDat>(0x101BD0A3));
        break;
    case 6:
        setAnmTbl(gabi::at<sBmconAnmDat>(0x101BD0A6));
        break;
    case 9:
        setAnmTbl(gabi::at<sBmconAnmDat>(0x101BD0A9));
        break;
    case 0xE:
        mbLookPlayer = 0;
        setAnmTbl(gabi::at<sBmconAnmDat>(0x101BD0AC));
        break;
    case 0x12:
        setAnmTbl(gabi::at<sBmconAnmDat>(0x101BD0B2));
        break;
    case 0x13:
        setAnmTbl(gabi::at<sBmconAnmDat>(0x101BD0B5));
        break;
    }
    gabi::store<u8>(dComIfGp_ea() + 0x5BC5, 0xFF); /* dComIfGp_clearMesgAnimeTagInfo() */
}
VERIFY(0x0220677C, &daNpcBmcon_c::setAnmFromMsgTag);

/* 022068A8 */
void daNpcBmcon_c::eventMove() {
    WWHD_FUNC(0x022068A8, void, this);
    if (chkEndEvent()) {
        executeSetMode(0);
        return;
    }
    u8 attn = mEventCut.mbAttention;
    if (mEventCut.cutProc()) {
        if (!mEventCut.mbAttention) {
            mEventCut.mbAttention = attn;
        }
    } else {
        privateCut();
        setAnmFromMsgTag();
    }
}
VERIFY(0x022068A8, &daNpcBmcon_c::eventMove);

/* 02206954 */
void daNpcBmcon_c::eventOrder() {
    WWHD_FUNC(0x02206954, void, this);
    u8 num = mOrderEventNum;
    if (num == 2 || num == 1) {
        eventInfo_onCondition(this, 0x21); /* dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e */
        if (mOrderEventNum == 2) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (num == 3) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        s16 ev = mEventIdx[0];
        fopAcM_orderChangeEventId(player, this, ev, 0, 0xFFFF);
    } else if (num == 4) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        s16 ev = mEventIdx[1];
        fopAcM_orderChangeEventId(player, this, ev, 0, 0xFFFF);
    } else if (num == 6) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        s16 ev = mEventIdx[1];
        fopAcM_orderChangeEventId(player, this, ev, 0, 0xFFFF);
    }
}
VERIFY(0x02206954, &daNpcBmcon_c::eventOrder);

/* 02206A74 */
void daNpcBmcon_c::playAnm() {
    WWHD_FUNC(0x02206A74, void, this);
    field_0x92E = field_0x92E & ~1;
    mpArmMorf->play(nullptr, 0, 0);
    if (mpMorf->play(nullptr, 0, 0)) {
        if (mpAnmDat.get() != nullptr) {
            if (mAnmLoop > 0) {
                mAnmLoop = mAnmLoop - 1;
                if (mAnmLoop == 0) {
                    mpAnmDat = gabi::at<sBmconAnmDat>(gabi::ea(mpAnmDat.get()) + 3);
                    if (setAnmTbl(mpAnmDat)) {
                        field_0x92E = field_0x92E | 1;
                    }
                } else {
                    setAnm(mpAnmDat->field_0x00, 0, 0.0f);
                }
            }
        }
    }
}
VERIFY(0x02206A74, &daNpcBmcon_c::playAnm);

/* 02206B58 */
void daNpcBmcon_c::lookBack() {
    WWHD_FUNC(0x02206B58, void, this);
    s16 targetY = current.angle.y;
    u8 headOnly = mbHeadOnly;
    s16 vel = mTurnSpeed;
    cXyz* dstTemp = nullptr;
    gabi::Local<cXyz> temp2;
    /* cXyz dstPos = eyePos (through FPRs) */
    f32 eye_x = eyePos.x;
    f32 eye_y = eyePos.y;
    f32 eye_z = eyePos.z;
    switch ((s8)mLookMode) {
    case 1:
        temp2->copy(mLookAtPos);
        dstTemp = temp2.get();
        break;
    case 2:
        targetY = mTargetAngleY;
        break;
    }
    if (mTalk != 0 && mbTurnOnLook != 0) {
        headOnly = false;
        m_jnt.mbTrn = 1; /* setTrn() */
    }
    if (m_jnt.mbTrn != 0) { /* trnChk() */
        if (mEventCut.mTurnSpeed != 0) {
            vel = mEventCut.mTurnSpeed;
        }
        cLib_addCalcAngleS2(&mTurnStep, vel, 4, 0x800);
    } else {
        mTurnStep = 0;
    }
    gabi::Local<cXyz> dstPos;
    dstPos->x = eye_x;
    dstPos->y = eye_y;
    dstPos->z = eye_z;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstTemp, dstPos.get(), targetY, mTurnStep, headOnly);
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
}
VERIFY(0x02206B58, &daNpcBmcon_c::lookBack);

/* 02206D64 */
bool daNpcBmcon_c::_execute() {
    WWHD_FUNC(0x02206D64, bool, this);
    chkAttention();
    checkOrder();
    if (!dComIfGp_event_runCheck() || gabi::load<u16>(gabi::ea(this) + 0xF8) == 1 /* eventInfo.checkCommandTalk() */) {
        bmcon_pmf_call0(this, BMCON_l_execute + mExeMode * 8); /* (this->*l_execute[mExeMode])() */
    } else {
        eventMove();
    }
    eventOrder();
    playAnm();
    if (mAnmIdx == 5) {
        cLib_chaseF(&speedF, field_0x8F4, 0.3f);
        f32 rate = speedF * bmcon_npc_dat(mNpcNo)->field_0x34;
        if (!(rate >= 0.5f)) {
            rate = 0.5f;
        }
        mpMorf->setPlaySpeed(rate);
        mpArmMorf->setPlaySpeed(rate);
    } else {
        cLib_chaseF(&speedF, field_0x8F4, 0.1f);
    }
    fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts)) /* mStts.GetCCMoveP() */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    gabi::Local<cXyz> center;
    center->x = current.pos.x;
    center->y = current.pos.y;
    center->z = current.pos.z;
    setCollision(&mCyl, center.get(), bmcon_npc_dat(mNpcNo)->field_0x30, 150.0f);
    daNpcBmcon_c__l_npc_dat* dat = bmcon_npc_dat(mNpcNo);
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    cXyz* attn_pos = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
    attn_pos->x = x;
    attn_pos->y = y + dat->field_0x18;
    attn_pos->z = z;
    eyePos.x = x;
    eyePos.y = y + dat->field_0x1C;
    eyePos.z = z;
    lookBack();
    setMtx();
    return false;
}
VERIFY(0x02206D64, &daNpcBmcon_c::_execute);

/* 02207054 */
static BOOL daNpc_BmconExecute(void* i_this) {
    WWHD_FUNC(0x02207054, BOOL, i_this);
    return static_cast<daNpcBmcon_c*>(i_this)->_execute();
}
VERIFY(0x02207054, daNpc_BmconExecute);

/* 02207058 */
bool daNpcBmcon_c::_draw() {
    WWHD_FUNC(0x02207058, bool, this);
    J3DModel* model = mpMorf->getModel();
    J3DModel* armModel = mpArmMorf->getModel();
    J3DModel* etcModel = mpEtcModel;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), armModel, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), etcModel, &tevStr);
    mpMorf->updateDL();
    mpArmMorf->updateDL();
    /* etcModel->setBaseTRMtx(model->getAnmMtx(m_jnt.getHeadJntNum())) */
    s32 head = m_jnt.mHeadJntNum;
    J3DMtxBlock_bmcon* blk = gabi::at<J3DMtxBlock_bmcon>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10;
    mtx_copy(gabi::at<Mtx34>(gabi::ea(etcModel) + 0xC8), gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + head * 0x30));
    mDoExt_modelUpdateDL(mpEtcModel);
    switch (mNpcNo) {
    case NPC_BMCON1:
        dSnap_RegistFig(0x95, this, 1.0f, 1.0f, 1.0f);
        break;
    case NPC_BMCON2:
        dSnap_RegistFig(0x94, this, 1.0f, 1.0f, 1.0f);
        break;
    }
    return true;
}
VERIFY(0x02207058, &daNpcBmcon_c::_draw);

/* 022071E8 */
static BOOL daNpc_BmconDraw(void* i_this) {
    WWHD_FUNC(0x022071E8, BOOL, i_this);
    return static_cast<daNpcBmcon_c*>(i_this)->_draw();
}
VERIFY(0x022071E8, daNpc_BmconDraw);

/* 028E8DAC PSVECSubtract(a, b, out) */
static inline void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out) { gabi::call(0x028E8DAC, a, b, out); }

/* 022071EC */
void daNpcBmcon_c::calcFlyDist(cXyz* result) {
    WWHD_FUNC(0x022071EC, void, this, result);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> dist;
    dist->x = link->current.pos.x;
    dist->y = link->current.pos.y;
    dist->z = link->current.pos.z;
    PSVECSubtract(dist.get(), gabi::at<cXyz>(0x104665F0) /* the platform's start position (static) */, dist.get());
    mDoMtx_YrotS(mDoMtx_stack_c::get(), -0x6000);
    gabi::Local<cXyz> out;
    PSMTXMultVec(mDoMtx_stack_c::get(), dist.get(), out.get());
    /* return out: GHS's copy constructor allocates when the result pointer is NULL */
    if (result == nullptr) {
        result = (cXyz*)operator_new(0xC);
        if (result == nullptr) {
            return;
        }
    }
    /* lfs/stfs pairs: the recompiled code copies the bits (a signalling NaN is not quieted) */
    gabi::store<u32>(gabi::ea(result), gabi::load<u32>(gabi::ea(out.get())));
    gabi::store<u32>(gabi::ea(result) + 4, gabi::load<u32>(gabi::ea(out.get()) + 4));
    gabi::store<u32>(gabi::ea(result) + 8, gabi::load<u32>(gabi::ea(out.get()) + 8));
}
VERIFY(0x022071EC, &daNpcBmcon_c::calcFlyDist);

/* 022072A0 */
void daNpcBmcon_c::setFlyDistNow(s32 dist) {
    WWHD_FUNC(0x022072A0, void, this, dist);
    u8 hi = dist / 256;
    dComIfGs_setTmpReg(0xFBFF, dist % 256);
    dComIfGs_setTmpReg(0xFAFF, hi);
}
VERIFY(0x022072A0, &daNpcBmcon_c::setFlyDistNow);

/* 02207324 */
s16 daNpcBmcon_c::getFlyDistMax() {
    WWHD_FUNC(0x02207324, s16, this);
    u8 lo = dSv_event_getEventReg(dComIfGs_event(), 0xA7FF);
    return (s16)(lo + (dSv_event_getEventReg(dComIfGs_event(), 0xA8FF) << 8));
}
VERIFY(0x02207324, &daNpcBmcon_c::getFlyDistMax);

/* 0220738C */
void daNpcBmcon_c::setFlyDistMax(s32 dist) {
    WWHD_FUNC(0x0220738C, void, this, dist);
    u8 hi = dist / 256;
    dSv_event_setEventReg(dComIfGs_event(), 0xA7FF, dist % 256);
    dSv_event_setEventReg(dComIfGs_event(), 0xA8FF, hi);
}
VERIFY(0x0220738C, &daNpcBmcon_c::setFlyDistMax);

/* 0252012C dComIfGp_setNextStage(stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe) */
static inline void dComIfGp_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe) {
    gabi::call(0x0252012C, stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe);
}
/* play + 0x5134: dStage_startStage_c (name[8], point, roomNo, layer at +0xB) */
static inline s8 dComIfGp_getStartStageLayer() { return gabi::load<s8>(dComIfGp_ea() + 0x513F); }

/* 02207410 */
u8 daNpcBmcon_c::executeCommon() {
    WWHD_FUNC(0x02207410, u8, this);
    if (mbAttention && !field_0x937) {
        mOrderEventNum = 1;
    } else {
        mOrderEventNum = 0;
    }
    if (mTalk == 1 && mExeMode != 1) {
        executeSetMode(1);
    }
    if (mNpcNo == NPC_BMCON2 && (dComIfGp_getStartStageLayer() == 2 || dComIfGp_getStartStageLayer() == 3)) {
        gabi::Local<cXyz> dist;
        calcFlyDist(dist.get());
        u8 flag = field_0x939;
        if (!(flag & 1) && dist->x > 10.0f) {
            field_0x939 = flag | 1;
            mDoAud_bgmStart_id(0x80000050);
            mDoAud_seStart_id(0x900);
        }
        if (field_0x938 == 1) {
            f32 x = dist->x;
            if (x > 25710.0f && dist->z < 1131.0f && dist->z > -1131.0f) {
                field_0x938 = 0;
                u8 magic = gabi::load<u8>(dComIfGs_save() + 0x33);
                u32 a = dComIfGp_ea() + 0x5B60;
                gabi::store<s16>(a, (s16)(gabi::load<s16>(a) - magic));
            } else if (x < -1948.0f || x > 25710.0f || dist->z < -3534.0f || dist->z > 3534.0f) {
                field_0x938 = 2;
                u8 magic = gabi::load<u8>(dComIfGs_save() + 0x33);
                u32 a = dComIfGp_ea() + 0x5B60;
                gabi::store<s16>(a, (s16)(gabi::load<s16>(a) - magic));
            }
        }
        if (gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & 0x100000) {
            s16 d = (s16)gabi::ftoi(dist->x / 100.0f);
            field_0x916 = d;
            if (d < 0) {
                field_0x938 = 3;
            }
            dComIfGs_setTmpReg(0xF903, field_0x938);
            if (field_0x938 == 0 || field_0x938 == 1) {
                mOrderEventNum = 3;
                setFlyDistNow(field_0x916);
            } else {
                mOrderEventNum = 4;
                setFlyDistNow(field_0x916);
            }
            if (field_0x938 != 2) {
                s16 max = getFlyDistMax();
                if (field_0x916 > max) {
                    setFlyDistMax(field_0x916);
                }
            }
            gabi::Local<cXyz> pt;
            dNpc_PathRun_getPoint(&mPathRun, pt.get(), mPathRun.mIdx);
            old.pos.copy(*pt);
            mHomeAngleY = -0x6000;
            current.pos.copy(*pt);
            current.angle.y = -0x6000;
            u8 flag2 = field_0x939;
            if (!(flag2 & 2)) {
                field_0x939 = flag2 | 2;
                mDoAud_bgmStop(0x5A);
            }
        }
    }
    return mTalk;
}
VERIFY(0x02207410, &daNpcBmcon_c::executeCommon);

/* 02207750 */
s32 daNpcBmcon_c::executeWaitInit() {
    WWHD_FUNC(0x02207750, s32, this);
    u8 npc = mNpcNo;
    speedF = 0.0f;
    setAnmTbl(gabi::at<sBmconAnmDat>(npc == NPC_BMCON1 ? 0x101BD0A0 : 0x101BD0A3));
    daNpcBmcon_c__l_npc_dat* dat = bmcon_npc_dat(mNpcNo);
    m_jnt.setParam(dat->field_0x04, dat->field_0x06, dat->field_0x0C, dat->field_0x0E, dat->field_0x00, dat->field_0x02,
                   dat->field_0x08, dat->field_0x0A, dat->field_0x10);
    return 0;
}
VERIFY(0x02207750, &daNpcBmcon_c::executeWaitInit);

/* 022077F4 */
void daNpcBmcon_c::executeWait() {
    WWHD_FUNC(0x022077F4, void, this);
    if (!executeCommon()) {
        if (mNpcNo == NPC_BMCON2 && mbAttention && !dComIfGs_isEventBit(0x2A40)) {
            mOrderEventNum = 2;
        }
    }
}
VERIFY(0x022077F4, &daNpcBmcon_c::executeWait);

/* 02207864 */
s32 daNpcBmcon_c::executeTalkInit() {
    WWHD_FUNC(0x02207864, s32, this);
    mbTurnOnLook = 1;
    return 1;
}
VERIFY(0x02207864, &daNpcBmcon_c::executeTalkInit);

/* 02207874 */
void daNpcBmcon_c::executeTalk() {
    WWHD_FUNC(0x02207874, void, this);
    executeCommon();
    if (talk2(1) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mTalk = 0;
        executeSetMode(0);
        daNpcBmcon_c__l_npc_dat* dat = bmcon_npc_dat(mNpcNo);
        u8 next = field_0x936;
        mbTurnOnLook = dat->field_0x4A;
        mbLookPlayer = dat->field_0x4B;
        if (next) {
            mOrderEventNum = 0;
            field_0x937 = 1;
            dComIfGp_setNextStage(STR(0x10017C9C) /* "sea" */, 1, 0xE, 2, 0.0f, 0, 1, 0);
        } else {
            dComIfGp_event_reset();
        }
    } else {
        setAnmFromMsgTag();
    }
}
VERIFY(0x02207874, &daNpcBmcon_c::executeTalk);

/* 02207974 */
s32 daNpcBmcon_c::executeWalkInit() {
    WWHD_FUNC(0x02207974, s32, this);
    setAnmTbl(gabi::at<sBmconAnmDat>(0x101BD0AF));
    return 2;
}
VERIFY(0x02207974, &daNpcBmcon_c::executeWalkInit);

/* 022079A0 */
void daNpcBmcon_c::executeWalk() {
    WWHD_FUNC(0x022079A0, void, this);
    if (executeCommon()) {
        return;
    }
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    if (!dNpc_PathRun_chkPointPass(&mPathRun, pos.get(), mPathRun.mbDir != 0) || dNpc_PathRun_nextIdxAuto(&mPathRun)) {
        gabi::Local<cXyz> pt;
        dNpc_PathRun_getPoint(&mPathRun, pt.get(), mPathRun.mIdx);
        gabi::Local<cXyz> target;
        target->x = pt->x;
        target->y = pt->y;
        target->z = pt->z;
        gabi::Local<cXyz> own;
        own->y = current.pos.y;
        own->x = current.pos.x;
        own->z = current.pos.z;
        gabi::Local<be<s16>> ang;
        dNpc_calc_DisXZ_AngY(own.get(), target.get(), nullptr, ang.get());
        s16 a = *ang;
        mTargetAngleY = a;
        mbHeadOnly = 0;
        mHomeAngleY = a;
        mLookMode = 2;
        daNpcBmcon_c__l_npc_dat* dat = bmcon_npc_dat(mNpcNo);
        mTurnSpeed = dat->field_0x2C;
        m_jnt.mbTrn = 1;
        field_0x8F4 = dat->field_0x38;
        return;
    }
    /* end of the path: turn round */
    mPathRun.mbDir = mPathRun.mbDir ^ 1;
    gabi::Local<cXyz> pt;
    dNpc_PathRun_getPoint(&mPathRun, pt.get(), mPathRun.mIdx);
    gabi::Local<cXyz> own;
    own->y = current.pos.y;
    own->x = current.pos.x;
    gabi::Local<cXyz> target;
    target->y = pt->y;
    target->x = pt->x;
    target->z = pt->z;
    own->z = current.pos.z;
    gabi::Local<be<s16>> ang;
    dNpc_calc_DisXZ_AngY(own.get(), target.get(), nullptr, ang.get());
    mHomeAngleY = *ang;
    dNpc_PathRun_setInf(&mPathRun, 0xFF, fopAcM_GetRoomNo(this), true);
    executeSetMode(0);
}
VERIFY(0x022079A0, &daNpcBmcon_c::executeWalk);

/* 02207B54 */
s32 daNpcBmcon_c::executeTurnInit() {
    WWHD_FUNC(0x02207B54, s32, this);
    gabi::Local<cXyz> pt;
    dNpc_PathRun_getPoint(&mPathRun, pt.get(), mPathRun.mIdx);
    gabi::Local<cXyz> own;
    own->y = current.pos.y;
    own->x = current.pos.x;
    gabi::Local<cXyz> target;
    target->y = pt->y;
    target->z = pt->z;
    target->x = pt->x;
    own->z = current.pos.z;
    gabi::Local<be<s16>> ang;
    dNpc_calc_DisXZ_AngY(own.get(), target.get(), nullptr, ang.get());
    if (*ang != current.angle.y) {
        return 3;
    }
    setAnmTbl(gabi::at<sBmconAnmDat>(0x101BD0AF));
    daNpcBmcon_c__l_npc_dat* dat = bmcon_npc_dat(mNpcNo);
    f32 rnd = cM_rndF((f32)(dat->field_0x46 - dat->field_0x44));
    dat = bmcon_npc_dat(mNpcNo);
    field_0x90A = (s16)gabi::ftoi(rnd + (f32)dat->field_0x44);
    return 2;
}
VERIFY(0x02207B54, &daNpcBmcon_c::executeTurnInit);

/* 02207CB4 */
void daNpcBmcon_c::executeTurn() {
    WWHD_FUNC(0x02207CB4, void, this);
    if (executeCommon()) {
        return;
    }
    gabi::Local<cXyz> pt;
    dNpc_PathRun_getPoint(&mPathRun, pt.get(), mPathRun.mIdx);
    gabi::Local<cXyz> own;
    own->z = current.pos.z;
    gabi::Local<cXyz> target;
    target->x = pt->x;
    own->x = current.pos.x;
    target->y = pt->y;
    target->z = pt->z;
    own->y = current.pos.y;
    gabi::Local<be<s16>> ang;
    dNpc_calc_DisXZ_AngY(own.get(), target.get(), nullptr, ang.get());
    s16 a = *ang;
    s16 cur = current.angle.y;
    mLookMode = 2;
    mTargetAngleY = a;
    mbHeadOnly = 0;
    m_jnt.mbTrn = 1;
    if (cur == a) {
        executeSetMode(2);
    }
}
VERIFY(0x02207CB4, &daNpcBmcon_c::executeTurn);

/* message lists (.data, u32[]: message numbers, then 0 end / 1-2 rupee choice / 3 start / 4-5 item) */
enum : u32 {
    BMCON_l_msg_decline1 = 0x101BD058,
    BMCON_l_msg_no_rupee = 0x101BD060,
    BMCON_l_msg_cleared0 = 0x101BD068,
    BMCON_l_msg_pay2 = 0x101BD070,
    BMCON_l_msg_decline2 = 0x101BD078,
    BMCON_l_msg_after0 = 0x101BD080,
    BMCON_l_msg_first1 = 0x101BD088,
    BMCON_l_msg_cleared1 = 0x101BD090,
    BMCON_l_msg_default1 = 0x101BD098,
    BMCON_l_msg_played1 = 0x101BD0F0,
    BMCON_l_msg_first0 = 0x101BD160,
    BMCON_l_msg_pay1 = 0x101BD174,
    BMCON_l_msg_played0 = 0x101BD180,
};
/* save info: rupees (u16 at +0x24), magic (u8 at +0x33); play: rupee cost of the message
 * (s16 at +0x5BA4), rupee count change (s32 at +0x5B48), magic change (s16 at +0x5B60) */
static inline u16 dComIfGs_getRupee() { return gabi::load<u16>(dComIfGs_save() + 0x24); }
static inline u8 dComIfGs_getMagic() { return gabi::load<u8>(dComIfGs_save() + 0x33); }
static inline s16 dComIfGp_getMessageRupee() { return gabi::load<s16>(dComIfGp_ea() + 0x5BA4); }
static inline void dComIfGp_setItemRupeeCount(s32 v) {
    u32 a = dComIfGp_ea() + 0x5B48;
    gabi::store<s32>(a, gabi::load<s32>(a) + v);
}
/* a list entry pays the message's rupee cost, or goes to the "not enough rupees" list */
static inline void bmcon_pay(daNpcBmcon_c* i_this, u32 list) {
    i_this->field_0x8F0 = list;
    s16 cost = dComIfGp_getMessageRupee();
    dComIfGp_setItemRupeeCount(-cost);
}

/* 02207D74 */
u16 daNpcBmcon_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x02207D74, u16, this, pMsgNo);
    u16 status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    /* HD: mpCurrMsg is the message manager (read once); mSelectNum at +0x948 */
    u32 msg = bmcon_msgManager();
    if (*pMsgNo == 0x2AB4) {
        *pMsgNo = 0x2AFF;
        return status;
    }
    if (field_0x8F0 == 0) {
        return 0x10; /* fopMsgStts_MSG_ENDS_e */
    }
    field_0x8F0 = field_0x8F0 + 4;
    u32 v = gabi::load<u32>(field_0x8F0);
    switch (v) {
    case 0:
        field_0x8F0 = 0;
        return 0x10;
    case 1:
        if (gabi::load<u32>(msg + 0x948) != 0) {
            field_0x8F0 = BMCON_l_msg_decline1;
        } else {
            s16 cost = dComIfGp_getMessageRupee();
            if ((s32)dComIfGs_getRupee() < cost) {
                field_0x8F0 = BMCON_l_msg_no_rupee;
            } else {
                bmcon_pay(this, BMCON_l_msg_pay1);
            }
        }
        *pMsgNo = gabi::load<u32>(field_0x8F0);
        break;
    case 2:
        if (gabi::load<u32>(msg + 0x948) != 0) {
            field_0x8F0 = BMCON_l_msg_decline2;
        } else {
            s16 cost = dComIfGp_getMessageRupee();
            if ((s32)dComIfGs_getRupee() < cost) {
                field_0x8F0 = BMCON_l_msg_no_rupee;
            } else {
                bmcon_pay(this, BMCON_l_msg_pay2);
            }
        }
        *pMsgNo = gabi::load<u32>(field_0x8F0);
        break;
    case 3: {
        u8 magic = dComIfGs_getMagic();
        u32 a = dComIfGp_ea() + 0x5B60;
        gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + magic));
        field_0x8F0 = 0;
        field_0x936 = 1;
        return 0x10;
    }
    case 4:
        field_0x92C = field_0x92C | 1;
        return 0x10;
    case 5:
        field_0x92C = field_0x92C | 2;
        return 0x10;
    default:
        *pMsgNo = v;
        break;
    }
    return status;
}
VERIFY(0x02207D74, &daNpcBmcon_c::next_msgStatus);

/* 02207F6C */
BOOL daNpcBmcon_c::isClear() {
    WWHD_FUNC(0x02207F6C, BOOL, this);
    return dComIfGs_isEventBit(0x2B40) ? TRUE : FALSE;
}
VERIFY(0x02207F6C, &daNpcBmcon_c::isClear);

/* 02207FA4 */
u32 daNpcBmcon_c::getMsg() {
    WWHD_FUNC(0x02207FA4, u32, this);
    u32 msgNo = 0;
    field_0x8F0 = 0;
    if (!((u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3) /* !dComIfGp_event_chkTalkXY() */) {
        switch (mNpcNo) {
        case NPC_BMCON1:
            if (field_0x936) {
                field_0x8F0 = BMCON_l_msg_after0;
            } else if (!dComIfGs_isEventBit(0x2901)) {
                field_0x8F0 = BMCON_l_msg_first0;
            } else if (isClear()) {
                field_0x8F0 = BMCON_l_msg_cleared0;
                s16 max = getFlyDistMax();
                dComIfGp_setMessageCountNumber(max);
            } else {
                s16 max = getFlyDistMax();
                dComIfGp_setMessageCountNumber(max);
                field_0x8F0 = BMCON_l_msg_played0;
            }
            break;
        case NPC_BMCON2:
            if (!dComIfGs_isEventBit(0x2A40)) {
                field_0x8F0 = BMCON_l_msg_first1;
                dComIfGs_onEventBit(0x2A40);
            } else if (isClear()) {
                field_0x8F0 = BMCON_l_msg_cleared1;
            } else if (dComIfGs_isEventBit(0x2901)) {
                s16 max = getFlyDistMax();
                dComIfGp_setMessageCountNumber(max);
                field_0x8F0 = BMCON_l_msg_played1;
            } else {
                field_0x8F0 = BMCON_l_msg_default1;
            }
            break;
        }
    }
    if (field_0x8F0 != 0) {
        msgNo = gabi::load<u32>(field_0x8F0);
    }
    return msgNo;
}
VERIFY(0x02207FA4, &daNpcBmcon_c::getMsg);

/* 02208184 __sinit_d_a_npc_bmcon1_cpp (header statics at P = 0x104665E4, the zeroed object at
 * 0x10466608), then the two static cXyz of the flight course */
static void __sinit_d_a_npc_bmcon1_cpp() {
    WWHD_FUNC(0x02208184, void);
    sinit_header_statics_z(0x104665E4, 0x101BD260, 0x10466608);
    cXyz* start = gabi::at<cXyz>(0x104665F0); /* the platform's start position (calcFlyDist) */
    start->x = 297080.0f;
    start->y = 1100.0f;
    start->z = -202920.0f;
    cXyz* p2 = gabi::at<cXyz>(0x104665FC);
    p2->x = 278900.0f;
    p2->y = 1100.0f;
    p2->z = -221100.0f;
}
VERIFY(0x02208184, __sinit_d_a_npc_bmcon1_cpp);

/* 0220825C: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0220825C, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x0220825C, SafeString_dt);

/* 02208270 */
static BOOL daNpc_BmconIsDelete(void* i_this) {
    WWHD_FUNC(0x02208270, BOOL, i_this);
    return true;
}
VERIFY(0x02208270, daNpc_BmconIsDelete);

/* 02208278: daNpcBmcon_c deleting destructor (compiler-generated, HD virtual destructor; the
 * members of daNpcBmcon_c itself are trivially destructible) */
static void daNpcBmcon_dt(daNpcBmcon_c* p, s32 flags) {
    WWHD_FUNC(0x02208278, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x02515A70, &p->mCyl, 2);                                    /* ~dCcD_Cyl */
        gabi::call(0x02515860, &p->mStts, 2);                                   /* ~dCcD_Stts */
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mAcchCir) + 0x14), 2); /* ~cM3dGCir */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x20, 0x10017A88);            /* ~dBgS_ObjAcch */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x14, 0x10017A98);
        gabi::call(0x024EFD9C, &p->mObjAcch, 0);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(p);
        }
    }
}
VERIFY(0x02208278, daNpcBmcon_dt);

/* 02208314: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x02208314, void);
}
VERIFY(0x02208314, SafeString_assureTerminationImpl);

/* 02208318 */
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift) {
    WWHD_FUNC(0x02208318, u32, i_actor, i_width, i_shift);
    /* PowerPC slw/srw: shift counts 32..63 give 0 */
    u32 mask = ((i_width & 0x20) ? 0u : (1u << (i_width & 0x1F))) - 1;
    u32 prm = i_actor->mParameters;
    return ((i_shift & 0x20) ? 0u : (prm >> (i_shift & 0x1F))) & mask;
}
VERIFY(0x02208318, daObj_PrmAbstract);
