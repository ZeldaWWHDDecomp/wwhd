/**
 * d_a_npc_mn.cpp (WWHD)
 * NPC - Manny
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_mn.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * This part: construction, heap, create phases, draw and the small helpers. The other part:
 * d_a_npc_mn_exec.cpp (execute, events, messages).
 */
#include "d/actor/d_a_npc_mn.h"

/* 0229F840 daObj::PrmAbstract<daNpcMn_c::Prm_e>(actor, width, shift) (out of line in this TU) */
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift);

/* 0229B8E0 */
static BOOL daNpc_Mn_nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0229B8E0, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        u32 model = gabi::load<u32>(0x104B462C); /* j3dSys.getModel() */
        daNpcMn_c* i_this = gabi::at<daNpcMn_c>(gabi::load<u32>(model + 0xB8)); /* getUserArea() */
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
VERIFY(0x0229B8E0, daNpc_Mn_nodeCallBack);

/* 0229BA38 */
BOOL daNpcMn_c::initTexPatternAnm(u32 modify) {
    WWHD_FUNC(0x0229BA38, BOOL, this, modify);
    J3DModelData* modelData = J3DModel_getModelData_l(mpMorf->getModel());
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(mn_arcname(), gabi::load<s32>(0x1001E634) /* l_btp_ix_tbl[0] */);
    if (m_head_tex_pattern.get() == nullptr) { /* JUT_ASSERT(0xA44, m_head_tex_pattern != NULL) */
        JUT_ASSERT_fail(STR(0x1001E6B8), 0xA44, STR(0x1001E69C));
    }
    if (!mDoExt_btpAnm_init(mBtpAnm, modelData, m_head_tex_pattern, 1, 2, 1.0f, 0, -1, modify, FALSE)) {
        return FALSE;
    }
    mBtpFrame = 0;
    mBtpTimer = 0;
    return TRUE;
}
VERIFY(0x0229BA38, &daNpcMn_c::initTexPatternAnm);

/* 0229BB3C */
BOOL daNpcMn_c::createHeap() {
    WWHD_FUNC(0x0229BB3C, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(mn_arcname(), gabi::load<s32>(0x1001E62C) /* l_bmd_ix_tbl[0] */);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(mn_arcname(), gabi::load<s32>(MN_l_bck_ix_tbl + mBckIdx * 4));
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1, nullptr,
                                    0x80000, 0x15021222);
    m_jnt.mHeadJntNum = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x1001E6CC) /* "head" */);
    if (!(m_jnt.mHeadJntNum >= 0)) {
        JUT_ASSERT_fail(STR(0x1001E6E0), 0x3ED, STR(0x1001E6F0));
    }
    m_jnt.mBackboneJntNum = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x1001E70C) /* "backbone" */);
    if (!(m_jnt.mBackboneJntNum >= 0)) {
        JUT_ASSERT_fail(STR(0x1001E6E0), 0x3F1, STR(0x1001E718));
    }
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    for (u16 i = 0; i < J3DModelData_getJointNum_l(modelData); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            J3DModelData_setJointCallBack_l(modelData, i, 0x0229B8E0 /* daNpc_Mn_nodeCallBack */);
        }
    }
    if (mpMorf->getModel() != nullptr) { /* HD: setUserArea through a null-checked getModel() */
        gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this));
    }
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    J3DModelData* etc_modelData = (J3DModelData*)dComIfG_getObjectIDRes(mn_arcname(), gabi::load<s32>(0x1001E630) /* l_etc_bmd_ix_tbl[0] */);
    mpModel = mDoExt_J3DModel__create(etc_modelData, 0x80000, 0x11000002);
    if (mpModel.get() == nullptr) {
        return FALSE;
    }
    mShoulderRJoint = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x1001E6D4) /* "shoulderR" */);
    return TRUE;
}
VERIFY(0x0229BB3C, &daNpcMn_c::createHeap);

/* 0229BE28 (tail call) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0229BE28, BOOL, i_this);
    return ((daNpcMn_c*)i_this)->createHeap();
}
VERIFY(0x0229BE28, CheckCreateHeap);

/* 0229BE2C */
u8 daNpcMn_c::chkPosNo() {
    WWHD_FUNC(0x0229BE2C, u8, this);
    u8 posNo = 0;
    /* HD: strcmp is a sead::SafeString comparison; l_room_name[posNo] is the left operand */
    for (int i = 10; i != 0; i--) {
        if (mn_isNextStage(gabi::load<u32>(MN_l_room_name + posNo * 4))) {
            return posNo;
        }
        posNo++;
    }
    return posNo;
}
VERIFY(0x0229BE2C, &daNpcMn_c::chkPosNo);

/* 0229BF38 */
u8 daNpcMn_c::getPrmNpcNo() {
    WWHD_FUNC(0x0229BF38, u8, this);
    return mPosFlag != 0;
}
VERIFY(0x0229BF38, &daNpcMn_c::getPrmNpcNo);

/* 0229BF48 daNpcMn_c::daNpcMn_c (HD: allocates when this == NULL) */
static daNpcMn_c* daNpcMn_c_ct(daNpcMn_c* i_this) {
    WWHD_FUNC(0x0229BF48, daNpcMn_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpcMn_c*)operator_new(0x93C);
        if (i_this == nullptr) {
            return i_this;
        }
    }
    fopNpc_npc_c_ct(i_this);
    i_this->__vtbl = MN_VTBL;
    mDoExt_btpAnm_ct(i_this->mBtpAnm);
    i_this->mHomeYRot = i_this->home.angle.y;
    i_this->mWaitTimer = 0;
    i_this->mResFlag = 0;
    i_this->mMoveState = daNpcMn_c::MOVE_PROC_WAIT;
    i_this->mEtcFlag = 0;
    i_this->mLookMode = daNpcMn_c::LOOK_MODE_NONE;
    i_this->mBckIdx = daNpcMn_c::BCK_WAIT01;
    i_this->mAnmMorfOverride = -1.0f;
    i_this->mHeadOnlyFollow = true;
    i_this->mTargetSpeedF = 0.0f;
    i_this->mPosFlag = i_this->chkPosNo();
    i_this->mNpcNo = i_this->getPrmNpcNo();
    return i_this;
}
VERIFY(0x0229BF48, daNpcMn_c_ct);

/* 0229C000 */
u8 daNpcMn_c::getPrmSwitchBit() {
    WWHD_FUNC(0x0229C000, u8, this);
    return daObj_PrmAbstract(this, 8, 8);
}
VERIFY(0x0229C000, &daNpcMn_c::getPrmSwitchBit);

/* 0229C02C (unnamed by the matcher) */
BOOL daNpcMn_c::isChangePos(u32 i_posNo) {
    WWHD_FUNC(0x0229C02C, BOOL, this, i_posNo);
    return i_posNo == 0;
}
VERIFY(0x0229C02C, &daNpcMn_c::isChangePos);

/* 0229C038 */
int daNpcMn_c::getRand(int i_max) {
    WWHD_FUNC(0x0229C038, int, this, i_max);
    int rnd = gabi::ftoi(cM_rndF((f32)i_max));
    if (rnd == i_max) {
        rnd = 0;
    }
    return rnd;
}
VERIFY(0x0229C038, &daNpcMn_c::getRand);

/* 0229C0A4 */
u8 daNpcMn_c::getPosNo() {
    WWHD_FUNC(0x0229C0A4, u8, this);
    s32 roomCnt[8];
    if (dComIfGs_isEventBit(0x3D08) && !dComIfGs_isEventBit(0x3120)) {
        return 1;
    }
    roomCnt[0] = 1;
    for (int i = 1; i < 8; i++) {
        roomCnt[i] = 0;
    }
    for (int fig = 0; fig < 0x86; fig++) {
        if (fig / 8 < 0x11) {
            int bit = fig % 8;
            u8 reg = dComIfGs_getEventReg(gabi::load<u16>(MN_l_figure_comp + (fig / 8) * 2));
            if (reg & (1 << bit)) {
                int roomId = dSnap_GetFigRoomId(fig);
                if (roomId != 0xFF && roomId < 8) {
                    roomCnt[roomId]++;
                }
            }
        }
    }
    int nonZero = 0;
    for (int i = 0; i < 8; i++) {
        if (roomCnt[i] != 0) {
            nonZero++;
        }
    }
    int rnd = getRand(nonZero);
    for (int i = 0; i < 8; i++) {
        if (roomCnt[i] != 0) {
            if (rnd) {
                rnd--;
            } else {
                return i + 1;
            }
        }
    }
    return 1;
}
VERIFY(0x0229C0A4, &daNpcMn_c::getPosNo);

/* 0229C22C */
static cPhs_State phase_1(daNpcMn_c* i_this) {
    WWHD_FUNC(0x0229C22C, cPhs_State, i_this);
    /* fopAcM_ct(i_this, daNpcMn_c) */
    if (!(i_this->actor_condition & fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            daNpcMn_c_ct(i_this);
        }
        i_this->actor_condition = i_this->actor_condition | fopAcCnd_INIT_e;
    }
    if (i_this->mPosFlag == 0) {
        dComIfGs_setEventReg(0x870F, 0);
        if (fopAcM_isSwitch(i_this, i_this->getPrmSwitchBit())) {
            return cPhs_UNK3_e; /* cPhs_STOP_e */
        }
    } else {
        u8 eventReg = dComIfGs_getEventReg(0x870F);
        if (i_this->isChangePos(eventReg)) {
            if (dComIfGs_isEventBit(0x3A01)) {
                eventReg = i_this->getPosNo();
                if (!(eventReg < 10)) { /* HD: JUT_ASSERT(0x3A1, posNo < 10) */
                    JUT_ASSERT_fail(STR(0x1001E748), 0x3A1, STR(0x1001E758));
                }
                dComIfGs_setEventReg(0x870F, eventReg);
            } else {
                eventReg = 1;
                dComIfGs_setEventReg(0x870F, eventReg);
            }
        }
        if (!(eventReg < 10)) { /* HD: JUT_ASSERT(0x3A7, posNo < 10) */
            JUT_ASSERT_fail(STR(0x1001E748), 0x3A7, STR(0x1001E79C));
        }
        if (eventReg != i_this->mPosFlag) {
            return cPhs_UNK3_e;
        }
    }
    i_this->setResFlag(1);
    return cPhs_NEXT_e;
}
VERIFY(0x0229C22C, phase_1);

/* 0229C3AC */
u8 daNpcMn_c::getPrmRailID() {
    WWHD_FUNC(0x0229C3AC, u8, this);
    return daObj_PrmAbstract(this, 8, 0x18);
}
VERIFY(0x0229C3AC, &daNpcMn_c::getPrmRailID);

/* 0229C3D8 */
void daNpcMn_c::setAnm(u32 i_bckIdx, int i_loopMode, f32 i_morf) {
    WWHD_FUNC(0x0229C3D8, void, this, i_bckIdx, i_loopMode, i_morf);
    if (!(mAnmMorfOverride < 0.0f)) {
        i_morf = mAnmMorfOverride;
        mAnmMorfOverride = -1.0f;
    }
    J3DAnmTransform* pAnm = (J3DAnmTransform*)dComIfG_getObjectIDRes(mn_arcname(), gabi::load<s32>(MN_l_bck_ix_tbl + i_bckIdx * 4));
    mpMorf->setAnm(pAnm, i_loopMode, i_morf, 1.0f, 0.0f, -1.0f, nullptr);
    mBckIdx = i_bckIdx;
}
VERIFY(0x0229C3D8, &daNpcMn_c::setAnm);

/* 0229C4E4 */
bool daNpcMn_c::setAnmTbl(sMnAnmDat* i_anmDat) {
    WWHD_FUNC(0x0229C4E4, bool, this, i_anmDat);
    mAnmFlag = mAnmFlag & 0xFE;
    if (i_anmDat->mBckIdx == BCK_NULL) {
        mpAnmDat = nullptr;
        return true;
    }
    mpAnmDat = i_anmDat;
    mAnmLoopCnt = i_anmDat->mLoopCount;
    if (mAnmLoopCnt > 0) {
        setAnm(i_anmDat->mBckIdx, 0 /* EMode_NONE */, (f32)i_anmDat->mMorf);
    } else if (mBckIdx != i_anmDat->mBckIdx) {
        setAnm(i_anmDat->mBckIdx, 2 /* EMode_LOOP */, (f32)i_anmDat->mMorf);
    }
    return false;
}
VERIFY(0x0229C4E4, &daNpcMn_c::setAnmTbl);

/* 0229C5BC (unnamed by the matcher) */
s16 daNpcMn_c::XyCheckCB(int) {
    WWHD_FUNC(0x0229C5BC, s16, this, 0);
    return false;
}
VERIFY(0x0229C5BC, &daNpcMn_c::XyCheckCB);

/* 0229C5C4 (unnamed by the matcher; tail call) */
static s16 daNpcMn_XyCheckCB(void* i_this, int i_itemBtn) {
    WWHD_FUNC(0x0229C5C4, s16, i_this, i_itemBtn);
    return static_cast<daNpcMn_c*>(i_this)->XyCheckCB(i_itemBtn);
}
VERIFY(0x0229C5C4, daNpcMn_XyCheckCB);

/* 0229C5C8 */
void daNpcMn_c::setMtx() {
    WWHD_FUNC(0x0229C5C8, void, this);
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
VERIFY(0x0229C5C8, &daNpcMn_c::setMtx);

/* 0229C6A8 */
void daNpcMn_c::setCollision(dCcD_Cyl* i_cyl, cXyz* i_center, f32 i_radius, f32 i_height) {
    WWHD_FUNC(0x0229C6A8, void, this, i_cyl, i_center, i_radius, i_height);
    i_cyl->SetC(i_center);
    i_cyl->SetR(i_radius);
    i_cyl->SetH(i_height);
    dComIfG_Ccsp_Set(i_cyl);
}
VERIFY(0x0229C6A8, &daNpcMn_c::setCollision);

/* 0229C738 */
cPhs_State daNpcMn_c::createInit() {
    WWHD_FUNC(0x0229C738, cPhs_State, this);
    int weight = 0xFF;
    u8 railID = getPrmRailID();
    if (railID != 0xFF) {
        dNpc_PathRun_setInf(&mPathRun, railID, fopAcM_GetRoomNo(this), true);
        dPath* path = mPathRun.mPath;
        if (path == nullptr) {
            return cPhs_ERROR_e;
        }
        /* HD: no dPath_GetNextRoomPath */
        actor_status = actor_status & ~0x80; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        u8 currPointIdx = 0;
        while (currPointIdx < dNpc_PathRun_maxPoint(&mPathRun)) {
            if (gabi::load<u8>(gabi::load<u32>(gabi::ea(path) + 8) + currPointIdx * 0x10) == 0) { /* m_points[i].mArg0 */
                break;
            }
            currPointIdx++;
        }
        if (currPointIdx == dNpc_PathRun_maxPoint(&mPathRun)) {
            currPointIdx = 0;
        }
        mPathRun.mIdx = currPointIdx;
        gabi::Local<cXyz> point;
        dNpc_PathRun_getPoint(&mPathRun, point.get(), currPointIdx);
        old.pos.copy(*point);
        current.pos.copy(*point);
        dNpc_PathRun_incIdxLoop(&mPathRun);
        mWaitTimer = 1;
        weight = 0xFE;
    }
    gravity = -9.0f;
    setAnmTbl(gabi::at<sMnAnmDat>(MN_l_npc_anm_wait));
    mHatchEventIdx = dComIfGp_evmng_getEventIdx(STR(0x1001E800) /* "FIGURE_HATCH_OPEN" */, 0xFF);
    gabi::store<u32>(gabi::ea(this) + 0x104, 0x0229C5C4); /* eventInfo.setXyCheckCB(daNpcMn_XyCheckCB) */
    mEventCut.setActorInfo2(gabi::at<const char>(gabi::load<u32>(MN_l_npc_staff_id)), this);
    mTalkOrder = 0;
    mTurnVel = 0;
    mbPlayerAttention = 0;
    mbNearPlayer = 0;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAA); /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAA); /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    MnNpcDat* dat = mn_npc_dat(mNpcNo);
    m_jnt.setParam(dat->mMax_backbone_x, dat->mMax_backbone_y, dat->mMin_backbone_x, dat->mMin_backbone_y,
                   dat->mMax_head_x, dat->mMax_head_y, dat->mMin_head_x, dat->mMin_head_y, dat->mMax_turn_step);
    dat = mn_npc_dat(mNpcNo);
    mbAllowBodyTurn = dat->mbAllowBodyTurn;
    mbLookOnly = dat->mbLookOnly;
    mAttnDist = dat->mAttnDist;
    mAttnAngle = dat->mAttnAngle;
    mObjAcch.CrrPos(dComIfG_Bgsp());
    f32 gnd = gabi::load<f32>(gabi::ea(&mObjAcch) + 0x94); /* GetGroundH() */
    if (-1000000000.0f /* -G_CM3D_F_INF */ != gnd) {
        current.pos.y = gnd;
        home.pos.y = gnd;
    }
    setMtx();
    J3DModel_calc(mpMorf->getModel());
    mStts.Init(weight, 0xFF, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101EA190) /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    gabi::Local<cXyz> center;
    center->x = current.pos.x;
    center->y = current.pos.y;
    center->z = current.pos.z;
    setCollision(&mCyl, center.get(), mn_npc_dat(mNpcNo)->mCylRadius, 150.0f);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0229C738, &daNpcMn_c::createInit);

/* 0229CA5C */
static cPhs_State phase_2(daNpcMn_c* i_this) {
    WWHD_FUNC(0x0229CA5C, cPhs_State, i_this);
    cPhs_State state = dComIfG_resLoad(i_this->getPhaseP(), mn_arcname());
    if (state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, 0x0229BE28 /* CheckCreateHeap */, 0)) {
            return i_this->createInit();
        }
        i_this->mpMorf = nullptr;
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x0229CA5C, phase_2);

/* 0229CAE4 (unnamed by the matcher) */
cPhs_State daNpcMn_c::_create() {
    WWHD_FUNC(0x0229CAE4, cPhs_State, this);
    /* static cPhs__Handler l_method[] = {phase_1, phase_2, NULL} */
    return dComLbG_PhaseHandler(&mPhsMethod, MN_l_method, this);
}
VERIFY(0x0229CAE4, &daNpcMn_c::_create);

/* 0229CAF8 (tail call) */
static cPhs_State daNpc_MnCreate(void* i_this) {
    WWHD_FUNC(0x0229CAF8, cPhs_State, i_this);
    return static_cast<daNpcMn_c*>(i_this)->_create();
}
VERIFY(0x0229CAF8, daNpc_MnCreate);

/* 0229CAFC */
bool daNpcMn_c::_delete() {
    WWHD_FUNC(0x0229CAFC, bool, this);
    dComIfG_resDelete(&mPhs, mn_arcname()); /* HD: dComIfG_resDeleteDemo is resDelete */
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    if (gabi::load<s8>(dComIfGp_ea() + 0x514C) != 0 /* dComIfGp_isEnableNextStage() */ && mn_isNextStage(0x1001E814 /* "sea" */)) {
        dComIfGs_setEventReg(0x870F, 0);
    }
    return true;
}
VERIFY(0x0229CAFC, &daNpcMn_c::_delete);

/* 0229CC30 (tail call) */
static BOOL daNpc_MnDelete(void* i_this) {
    WWHD_FUNC(0x0229CC30, BOOL, i_this);
    return static_cast<daNpcMn_c*>(i_this)->_delete();
}
VERIFY(0x0229CC30, daNpc_MnDelete);

/* 0229CC34 */
void daNpcMn_c::chkAttention() {
    WWHD_FUNC(0x0229CC34, void, this);
    mbNearPlayer = 0;
    if (mEventCut.mbAttention) { /* getAttnFlag() */
        mLookAtPos.x = mEventCut.mPos.x;
        mLookAtPos.z = mEventCut.mPos.z;
        mLookAtPos.y = mEventCut.mPos.y;
        mLookMode = LOOK_MODE_ATTN;
        if (mbAllowBodyTurn) {
            mHeadOnlyFollow = false;
            m_jnt.mbTrn = 1; /* setTrn() */
        } else {
            mHeadOnlyFollow = true;
        }
        if (mbPlayerAttention == 0) {
            mbPlayerAttention = 1;
        }
    } else {
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        gabi::Local<cXyz> ppos;
        ppos->x = player->current.pos.x;
        ppos->y = player->current.pos.y;
        f32 maxDist = mAttnDist;
        ppos->z = player->current.pos.z;
        s32 maxAngle = mAttnAngle;
        gabi::Local<be<f32>> dist;
        gabi::Local<be<s16>> angle;
        dNpc_calc_DisXZ_AngY(pos.get(), ppos.get(), dist.get(), angle.get());
        if (mbPlayerAttention != 0) {
            maxDist += 40.0f;
            maxAngle += 0x71C;
        }
        s16 ang = (s16)(*angle - shape_angle.y);
        *angle = ang;
        f32 d = *dist;
        s32 abs_ang = ang < 0 ? -ang : ang;
        if (maxDist > d && maxAngle > abs_ang) {
            gabi::Local<cXyz> eye;
            dNpc_playerEyePos_l(eye.get(), mn_npc_dat(mNpcNo)->mPlayerEyeOfsY);
            mLookAtPos.copy(*eye);
            mHeadOnlyFollow = mbAllowBodyTurn == 0;
            mLookMode = LOOK_MODE_ATTN;
            if (mbLookOnly == 0) {
                mHeadOnlyFollow = false;
                mTargetYRot = mHomeYRot;
                m_jnt.mbTrn = 1;
                mLookMode = LOOK_MODE_TURN;
            }
            if (mbPlayerAttention == 0) {
                mbPlayerAttention = 1;
            }
        } else {
            if (mbPlayerAttention == 1) {
                mbPlayerAttention = 0;
                mLookResetTimer = mn_npc_dat(mNpcNo)->mLookResetTimer;
            }
            if (mn_npc_dat(mNpcNo)->mNearDist > d) {
                gabi::Local<cXyz> eye;
                dNpc_playerEyePos_l(eye.get(), mn_npc_dat(mNpcNo)->mPlayerEyeOfsY);
                mLookMode = LOOK_MODE_ATTN;
                mLookAtPos.copy(*eye);
                mHeadOnlyFollow = mbAllowBodyTurn == 0;
                if (mbLookOnly == 0) {
                    mHeadOnlyFollow = false;
                    mTargetYRot = mHomeYRot;
                    mLookMode = LOOK_MODE_TURN;
                    m_jnt.mbTrn = 1;
                }
                mbNearPlayer = 1;
            } else {
                mLookMode = LOOK_MODE_NONE;
                if (mPathRun.mPath.get() == nullptr) {
                    if (mLookResetTimer != 0) {
                        mLookResetTimer = mLookResetTimer - 1;
                    } else {
                        mHeadOnlyFollow = false;
                        mTargetYRot = mHomeYRot;
                        mLookMode = LOOK_MODE_TURN;
                        m_jnt.mbTrn = 1;
                    }
                }
            }
        }
    }
    mLookAtMaxVel = mn_npc_dat(mNpcNo)->mLookAtMaxVel;
}
VERIFY(0x0229CC34, &daNpcMn_c::chkAttention);

/* 0229D728 */
u8 daNpcMn_c::getPrmSwitchBit2() {
    WWHD_FUNC(0x0229D728, u8, this);
    return daObj_PrmAbstract(this, 8, 0x10);
}
VERIFY(0x0229D728, &daNpcMn_c::getPrmSwitchBit2);

/* 0229E728 (tail call) */
static BOOL daNpc_MnExecute(void* i_this) {
    WWHD_FUNC(0x0229E728, BOOL, i_this);
    return static_cast<daNpcMn_c*>(i_this)->_execute();
}
VERIFY(0x0229E728, daNpc_MnExecute);

/* 0229E72C */
bool daNpcMn_c::_draw() {
    WWHD_FUNC(0x0229E72C, bool, this);
    if (dComIfGs_isTmpBit(0x0408)) {
        return true;
    }
    J3DModel* morfModel = mpMorf->getModel();
    J3DModelData* morfModelData = J3DModel_getModelData_l(morfModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morfModel, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, morfModelData, mBtpFrame);
    mpMorf->updateDL();
    gabi::store<u32>(gabi::ea(morfModelData) + 0x38, 0); /* mBtpAnm.remove(morfModelData) */
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    /* mpModel->setBaseTRMtx(morfModel->getAnmMtx(mShoulderRJoint)) */
    mtx_copy(gabi::at<Mtx34>(gabi::ea(mpModel.get()) + 0xC8), mn_getAnmMtx(morfModel, mShoulderRJoint));
    mDoExt_modelUpdateDL(mpModel);
    /* HD: no dComIfGd_setShadow */
    dSnap_RegistFig(0xA8 /* DSNAP_TYPE_NPC_MN */, this, 1.0f, 1.0f, 1.0f);
    return true;
}
VERIFY(0x0229E72C, &daNpcMn_c::_draw);

/* 0229E89C (tail call) */
static BOOL daNpc_MnDraw(void* i_this) {
    WWHD_FUNC(0x0229E89C, BOOL, i_this);
    return static_cast<daNpcMn_c*>(i_this)->_draw();
}
VERIFY(0x0229E89C, daNpc_MnDraw);

/* 0229F6F0 __sinit_d_a_npc_mn_cpp (header statics) */
static void __sinit_d_a_npc_mn_cpp() {
    WWHD_FUNC(0x0229F6F0, void);
    sinit_header_statics(0x10467CA4, 0x101C2244);
}
VERIFY(0x0229F6F0, __sinit_d_a_npc_mn_cpp);

/* 0229F784: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0229F784, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x0229F784, SafeString_dt);

/* 0229F798 */
static BOOL daNpc_MnIsDelete(void*) {
    WWHD_FUNC(0x0229F798, BOOL, (void*)nullptr);
    return TRUE;
}
VERIFY(0x0229F798, daNpc_MnIsDelete);

/* 0229F7A0: daNpcMn_c deleting destructor (compiler-generated, HD virtual destructor; the members
 * of daNpcMn_c itself are trivially destructible) */
static void daNpcMn_dt(daNpcMn_c* p, s32 flags) {
    WWHD_FUNC(0x0229F7A0, void, p, flags);
    if (p != nullptr) {
        dCcD_Cyl_dt(&p->mCyl, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mAcchCir) + 0x14), 2); /* ~cM3dGCir */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x20, 0x1001E658);            /* ~dBgS_ObjAcch */
        gabi::store<u32>(gabi::ea(&p->mObjAcch) + 0x14, 0x1001E668);
        gabi::call(0x024EFD9C, &p->mObjAcch, 0);
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(p);
        }
    }
}
VERIFY(0x0229F7A0, daNpcMn_dt);

/* 0229F83C: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x0229F83C, void);
}
VERIFY(0x0229F83C, SafeString_assureTerminationImpl);

/* 0229F840 */
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift) {
    WWHD_FUNC(0x0229F840, u32, i_actor, i_width, i_shift);
    /* PowerPC slw/srw: shift counts 32..63 give 0 */
    u32 mask = ((i_width & 0x20) ? 0u : (1u << (i_width & 0x1F))) - 1;
    u32 prm = i_actor->mParameters;
    return ((i_shift & 0x20) ? 0u : (prm >> (i_shift & 0x1F))) & mask;
}
VERIFY(0x0229F840, daObj_PrmAbstract);
