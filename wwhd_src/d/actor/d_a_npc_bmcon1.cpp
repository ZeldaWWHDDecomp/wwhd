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

static u32 daObj_PrmAbstract(fopAc_ac_c*, u32, u32);
static BOOL CheckCreateHeap(fopAc_ac_c*);
static s16 daNpcBmcon_XyCheckCB(void*, int);
static daNpcBmcon_c* daNpcBmcon_c_ct(daNpcBmcon_c*);
const gc_tu_data gc_tu = {.safeString = 0x10017A70};
static auto& l_arcname_tbl = gabi::object_at<gptr<char>[2]>(0x101BD050);
static auto& l_npc_staff_id = gabi::object_at<gptr<char>[2]>(0x101BD048);
static auto& l_npc_anm_tbl = gabi::object_at<gptr<sBmconAnmDat>[8]>(0x101BD0B8);
static auto& l_npc_dat = gabi::object_at<daNpcBmcon_c__l_npc_dat[2]>(0x101BD1B4);
static auto& l_method = gabi::object_at<be<u32>[3]>(0x101BD24C);
static auto& l_bmd_ix_tbl = gabi::object_at<be<s32>[2]>(0x10017A40);
static auto& l_etc_bmd_ix_tbl = gabi::object_at<be<s32>[2]>(0x10017A48);
static auto& l_arm_bmd_ix_tbl = gabi::object_at<be<s32>[2]>(0x10017A50);
static auto& l_bck_ix_tbl = gabi::object_at<be<s32>[8]>(0x10017AA8);
static auto& l_arm_bck_ix_tbl = gabi::object_at<be<s32>[8]>(0x10017AC8);
static auto& l_execute_init = gabi::object_at<BmconAction[4]>(0x101BD100);
static auto& l_execute = gabi::object_at<BmconAction[4]>(0x101BD120);
static auto& l_msg_result_cleared = gabi::object_at<be<u32>[1]>(0x101BD1A0);
static auto& l_msg_result_first = gabi::object_at<be<u32>[1]>(0x101BD0D8);
static auto& l_msg_dist = gabi::object_at<be<u32>[1]>(0x101BD18C);
static auto& l_msg_decline1 = gabi::object_at<be<u32>[1]>(0x101BD058);
static auto& l_msg_no_rupee = gabi::object_at<be<u32>[1]>(0x101BD060);
static auto& l_msg_cleared0 = gabi::object_at<be<u32>[1]>(0x101BD068);
static auto& l_msg_pay2 = gabi::object_at<be<u32>[1]>(0x101BD070);
static auto& l_msg_decline2 = gabi::object_at<be<u32>[1]>(0x101BD078);
static auto& l_msg_after0 = gabi::object_at<be<u32>[1]>(0x101BD080);
static auto& l_msg_first1 = gabi::object_at<be<u32>[1]>(0x101BD088);
static auto& l_msg_cleared1 = gabi::object_at<be<u32>[1]>(0x101BD090);
static auto& l_msg_default1 = gabi::object_at<be<u32>[1]>(0x101BD098);
static auto& l_msg_played1 = gabi::object_at<be<u32>[1]>(0x101BD0F0);
static auto& l_msg_first0 = gabi::object_at<be<u32>[1]>(0x101BD160);
static auto& l_msg_pay1 = gabi::object_at<be<u32>[1]>(0x101BD174);
static auto& l_msg_played0 = gabi::object_at<be<u32>[1]>(0x101BD180);
static auto& l_message = gabi::object_at<gptr<void>>(0x101F4B5C);
static auto& dNpc_cyl_src = gabi::object_at<dCcD_SrcCyl>(0x101EA190);
static auto& cut_name_tbl = gabi::object_at<gptr<char>[2]>(0x101BD258);
static auto& l_startPos = gabi::object_at<cXyz>(0x104665F0);
static auto& l_coursePos = gabi::object_at<cXyz>(0x104665FC);
static auto& l_anm_tag_tbl = gabi::object_at<sBmconAnmDat[8]>(0x101BD0A0);

/* inlined into eventMesSetInit */
static inline void bmcon_startShock(s32 strength, s32 flags) {
    cXyz pos;
    dVibration_c* vib = dComIfGp_getVibration();
    pos.x = 0;
    pos.y = 1;
    pos.z = 0;
    Bmcon_shock(vib, strength, flags, &pos);
}
/* inlined into next_msgStatus */
static inline void bmcon_pay(daNpcBmcon_c* i_this, be<u32>* list) {
    i_this->mpMessageCursor = list;
    s16 cost = dComIfGp_getMessageRupee();
    dComIfGp_setItemRupeeCount(-cost);
}

/* 0220435C */
static BOOL daNpc_Bmcon_nodeCallBack(J3DNode* node, int calcTiming) {

    if (calcTiming == 0) {
        J3DModel* model = j3dSys.getModel(); /* j3dSys.getModel() */
        daNpcBmcon_c* i_this = static_cast<daNpcBmcon_c*>(model->getUserArea()); /* getUserArea() */
        u32 jntNo = reinterpret_cast<J3DJoint*>(node)->getJntNo();
        PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
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
        mtx_copy(model->getAnmMtx(jntNo), calc_mtx());
        PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
    }
    return TRUE;
}

/* 02204678 */
static BOOL daNpc_Arm_nodeCallBack(J3DNode* node, int calcTiming) {

    if (calcTiming == 0) {
        J3DModel* model = j3dSys.getModel(); /* j3dSys.getModel() */
        daNpcBmcon_c* i_this = static_cast<daNpcBmcon_c*>(model->getUserArea());
        i_this->nodeArmControl(node, model);
    }
    return TRUE;
}

/* 022044EC */
void daNpcBmcon_c::nodeArmControl(J3DNode* node, J3DModel* model) {

    u32 jntNo = reinterpret_cast<J3DJoint*>(node)->getJntNo();
    PSMTXCopy(model->getAnmMtx(jntNo), mDoMtx_stack_c::get());
    if (jntNo == (u32)(s32)m_armLloc_jnt_num) {
        PSMTXCopy(&mArmLMtx, J3DSys::mCurrentMtx);
        mtx_copy(model->getAnmMtx(jntNo), &mArmLMtx); /* setAnmMtx */
    }
    if (jntNo == (u32)(s32)m_armRloc_jnt_num) {
        PSMTXCopy(&mArmRMtx, J3DSys::mCurrentMtx);
        mtx_copy(model->getAnmMtx(jntNo), &mArmRMtx);
    }
}

/* 02204CA0 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {

    return static_cast<daNpcBmcon_c*>(i_this)->createHeap();
}

/* 02204D74 */
static cPhs_State phase_1(daNpcBmcon_c* i_this) {

    /* fopAcM_ct(i_this, daNpcBmcon_c) */
    if (!(i_this->actor_condition & fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            daNpcBmcon_c_ct(i_this);
        }
        i_this->actor_condition = i_this->actor_condition | fopAcCnd_INIT_e;
    }
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    if (link == nullptr || Bmcon_isCreating(fopAcM_GetID(link))) {
        return cPhs_INIT_e;
    }
    i_this->mResFlag = 1;
    return cPhs_NEXT_e;
}

/* 02205630 */
static cPhs_State phase_2(daNpcBmcon_c* i_this) {

    cPhs_State rt = dComIfG_resLoad(i_this->getPhaseP(), l_arcname_tbl[i_this->getNpcNo()].get());
    if (rt == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, CheckCreateHeap, 0)) {
            return i_this->createInit();
        }
        i_this->mpMorf = nullptr;
        return cPhs_ERROR_e;
    }
    return rt;
}

/* 022056C4 */
cPhs_State daNpcBmcon_c::_create() {

    /* static cPhs__Handler l_method[] = {phase_1, phase_2, NULL} */
    return Bmcon_phaseHandler(&mPhase2, l_method, this);
}

/* 022046B4 */
BOOL daNpcBmcon_c::createHeap() {

    u32 npc = mNpcNo;
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectIDRes(l_arcname_tbl[npc].get(), l_bmd_ix_tbl[npc]);
    u32 anm = mAnmIdx;
    J3DAnmTransform* pAnm =
        (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname_tbl[mNpcNo].get(), l_bck_ix_tbl[anm]);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, pAnm, 2 /* EMode_LOOP */,
                                    1.0f, 0, -1, 1, nullptr, 0x00080000, 0x11000002);
    npc = mNpcNo;
    J3DModelData* armModelData =
        (J3DModelData*)dComIfG_getObjectIDRes(l_arcname_tbl[npc].get(), l_arm_bmd_ix_tbl[npc]);
    anm = mAnmIdx;
    J3DAnmTransform* pArmAnm = (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname_tbl[mNpcNo].get(),
                                                                        l_arm_bck_ix_tbl[anm]);
    mpArmMorf = mDoExt_McaMorf::create(nullptr, armModelData, nullptr, nullptr, pArmAnm, 2, 1.0f, 0,
                                       -1, 1, nullptr, 0x00080000, 0x11000002);
    if (mpArmMorf.get() == nullptr || mpArmMorf->getModel() == nullptr) {
        return false;
    }
    npc = mNpcNo;
    J3DModelData* etcModelData =
        (J3DModelData*)dComIfG_getObjectIDRes(l_arcname_tbl[npc].get(), l_etc_bmd_ix_tbl[npc]);
    mpEtcModel = mDoExt_J3DModel__create(etcModelData, 0x80000, 0x37441422);
    if (mpEtcModel.get() == nullptr) {
        return false;
    }
    m_jnt.mHeadJntNum = (s8)modelData->getJointName()->getIndex("head");
    if (!(m_jnt.mHeadJntNum >= 0)) {
        JUT_ASSERT_fail("d_a_npc_bmcon1.cpp", 0x3F4, "m_jnt.getHeadJntNum() >= 0");
    }
    m_jnt.mBackboneJntNum = (s8)modelData->getJointName()->getIndex("backbone");
    if (!(m_jnt.mBackboneJntNum >= 0)) {
        JUT_ASSERT_fail("d_a_npc_bmcon1.cpp", 0x3F8, "m_jnt.getBackboneJntNum() >= 0");
    }
    m_nec_jnt_num = (s8)modelData->getJointName()->getIndex("neck");
    if (!(m_nec_jnt_num >= 0)) {
        JUT_ASSERT_fail("d_a_npc_bmcon1.cpp", 0x3FF, "m_nec_jnt_num >= 0");
    }
    m_arm_L_jnt_num = (s8)modelData->getJointName()->getIndex("armL");
    if (!(m_arm_L_jnt_num >= 0)) {
        JUT_ASSERT_fail("d_a_npc_bmcon1.cpp", 0x403, "m_arm_L_jnt_num >= 0");
    }
    m_arm_R_jnt_num = (s8)modelData->getJointName()->getIndex("armR");
    if (!(m_arm_R_jnt_num >= 0)) {
        JUT_ASSERT_fail("d_a_npc_bmcon1.cpp", 0x405, "m_arm_R_jnt_num >= 0");
    }
    m_armLloc_jnt_num = (s8)armModelData->getJointName()->getIndex("armLloc");
    if (!(m_armLloc_jnt_num >= 0)) {
        JUT_ASSERT_fail("d_a_npc_bmcon1.cpp", 0x409, "m_armL1_jnt_num >= 0");
    }
    m_armRloc_jnt_num = (s8)armModelData->getJointName()->getIndex("armRloc");
    if (!(m_armRloc_jnt_num >= 0)) {
        JUT_ASSERT_fail("d_a_npc_bmcon1.cpp", 0x40B, "m_armR1_jnt_num >= 0");
    }
    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum ||
            i == (u32)(s32)m_nec_jnt_num || i == (u32)(s32)m_arm_L_jnt_num ||
            i == (u32)(s32)m_arm_R_jnt_num) {
            modelData->getJointNodePointer(i)->setCallBack(daNpc_Bmcon_nodeCallBack);
        }
    }
    if (mpMorf->getModel() != nullptr) {
        mpMorf->getModel()->setUserArea((uintptr_t)this); /* setUserArea(this) */
    }
    for (u16 i = 0; i < armModelData->getJointNum(); i++) {
        if (i == (u32)(s32)m_armLloc_jnt_num || i == (u32)(s32)m_armRloc_jnt_num) {
            armModelData->getJointNodePointer(i)->setCallBack(daNpc_Arm_nodeCallBack);
        }
    }
    mpArmMorf->getModel()->setUserArea((uintptr_t)this);
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    return true;
}

/* 0220507C */
static s16 daNpcBmcon_XyCheckCB(void* i_this, int i_itemBtn) {

    return static_cast<daNpcBmcon_c*>(i_this)->XyCheckCB(i_itemBtn);
}

/* 022051F0 */
cPhs_State daNpcBmcon_c::createInit() {

    int temp = 0xFF;
    u8 pathIndex = getPrmRailID();
    if (pathIndex != 0xFF) {
        mPathRun.setInf(pathIndex, fopAcM_GetRoomNo(this), true);
        if (mPathRun.mPath.get() == nullptr) {
            return cPhs_ERROR_e;
        }
        actor_status = actor_status & ~0x80; /* fopAcM_OffStatus(this, fopAcStts_NOCULLEXEC_e) */
        cXyz point;
        mPathRun.getPoint(&point, mPathRun.mIdx);
        current.pos.copy(point);
        old.pos.copy(point);
        mPathRun.incIdxLoop();
        mTurnWaitTimer = 1;
        temp = 0xFE;
    }
    gravity = -9.0f;
    setAnmTbl(l_npc_anm_tbl[mAnmIdx].get());
    switch (mNpcNo) {
    case NPC_BMCON1:
        mEventIdx[0] = dComIfGp_evmng_getEventIdx("BMCON_RESULT", 0xFF);   /* "BMCON_RESULT" */
        mEventIdx[1] = dComIfGp_evmng_getEventIdx("BMCON_GET_ITEM", 0xFF); /* "BMCON_GET_ITEM" */
        if (dComIfGs_isTmpBit(0x0210)) {
            dComIfGs_offTmpBit(0x0210);
            fopAc_ac_c* link = dComIfGp_getLinkPlayer();
            Bmcon_orderChangeEvent(link, this, mEventIdx[0], 0, 0xFFFF);
            Bmcon_view<BmconPlayerFlag>(link)->mStatus |= 0x80000; /* HD: player flag */
        }
        break;
    case NPC_BMCON2:
        mEventIdx[0] = dComIfGp_evmng_getEventIdx("BMCON_END", 0xFF);  /* "BMCON_END" */
        mEventIdx[1] = dComIfGp_evmng_getEventIdx("BMCON_END2", 0xFF); /* "BMCON_END2" */
        break;
    default:
        break;
    }
    eventInfo.setXyCheckCB(daNpcBmcon_XyCheckCB); /* eventInfo.setXyCheckCB(daNpcBmcon_XyCheckCB) */
    mEventCut.setActorInfo2(l_npc_staff_id[mNpcNo].get(), this);
    mTurnStep = 0;
    mbAttention = 0;
    mTalk = 0;
    mbFar = 0;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] =
        0xA9; /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] =
        0xA9;                   /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
    attention_info.flags = 0xA; /* attention_info.flags */
    daNpcBmcon_c__l_npc_dat* dat = &l_npc_dat[mNpcNo];
    m_jnt.setParam(dat->mMaxBackBoneX, dat->mMaxBackBoneY, dat->mMinBackBoneX, dat->mMinBackBoneY,
                   dat->mMaxHeadX, dat->mMaxHeadY, dat->mMinHeadX, dat->mMinHeadY,
                   dat->mMaxTurnStep);
    dat = &l_npc_dat[mNpcNo];
    mbTurnOnLook = dat->mbTurnOnLook;
    mbLookPlayer = dat->mbLookPlayer;
    mAttDist = dat->mAttDist;
    mAttAngle = dat->mAttAngle;
    mObjAcch.CrrPos(dComIfG_Bgsp());
    f32 gnd = mObjAcch.m_ground_h; /* GetGroundH() */
    if (-1000000000.0f /* -G_CM3D_F_INF */ != gnd) {
        current.pos.y = gnd;
        home.pos.y = gnd;
    }
    setMtx();
    J3DModel_calc(mpMorf->getModel());
    mStts.Init(temp, 0xFF, this);
    mCyl.Set(&dNpc_cyl_src /* dNpc_cyl_src */);
    mCyl.SetStts(&mStts);
    cXyz center;
    center.x = current.pos.x;
    center.y = current.pos.y;
    center.z = current.pos.z;
    setCollision(&mCyl, &center, l_npc_dat[mNpcNo].mCollisionRadius, 150.0f);
    return cPhs_COMPLEATE_e;
}

/* 022056DC */
bool daNpcBmcon_c::_delete() {

    if (mResFlag) {
        dComIfG_resDelete(&mPhase, l_arcname_tbl[mNpcNo].get());
    }
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    return true;
}

/* 02207058 */
bool daNpcBmcon_c::_draw() {

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
    mtx_copy(&Bmcon_view<BmconModelMatrix>(etcModel)->mBaseMatrix, model->getAnmMtx(head));
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

/* 02206D64 */
bool daNpcBmcon_c::_execute() {

    chkAttention();
    checkOrder();
    if (!dComIfGp_event_runCheck() || eventInfo.mCommand == 1 /* eventInfo.checkCommandTalk() */) {
        Bmcon_callMode(this, &l_execute[mExeMode]); /* (this->*l_execute[mExeMode])() */
    } else {
        eventMove();
    }
    eventOrder();
    playAnm();
    if (mAnmIdx == 5) {
        cLib_chaseF(&speedF, mTargetSpeed, 0.3f);
        f32 rate = speedF * l_npc_dat[mNpcNo].mWalkAnmRate;
        if (!(rate >= 0.5f)) {
            rate = 0.5f;
        }
        mpMorf->setPlaySpeed(rate);
        mpArmMorf->setPlaySpeed(rate);
    } else {
        cLib_chaseF(&speedF, mTargetSpeed, 0.1f);
    }
    fopAcM_posMoveF(this, &mStts.m_cc_move /* mStts.GetCCMoveP() */);
    mObjAcch.CrrPos(dComIfG_Bgsp());
    cXyz center;
    center.x = current.pos.x;
    center.y = current.pos.y;
    center.z = current.pos.z;
    setCollision(&mCyl, &center, l_npc_dat[mNpcNo].mCollisionRadius, 150.0f);
    daNpcBmcon_c__l_npc_dat* dat = &l_npc_dat[mNpcNo];
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    cXyz* attn_pos = &attention_info.position; /* attention_info.position */
    attn_pos->x = x;
    attn_pos->y = y + dat->mAttOffsetY;
    attn_pos->z = z;
    eyePos.x = x;
    eyePos.y = y + dat->mEyeHeight;
    eyePos.z = z;
    lookBack();
    setMtx();
    return false;
}

/* 02207410 */
u8 daNpcBmcon_c::executeCommon() {

    if (mbAttention && !mbChangingStage) {
        mOrderEventNum = 1;
    } else {
        mOrderEventNum = 0;
    }
    if (mTalk == 1 && mExeMode != 1) {
        executeSetMode(1);
    }
    if (mNpcNo == NPC_BMCON2 &&
        (dComIfGp_getStartStageLayer() == 2 || dComIfGp_getStartStageLayer() == 3)) {
        cXyz dist;
        calcFlyDist(&dist);
        u8 flag = mFlightFlags;
        if (!(flag & 1) && dist.x > 10.0f) {
            mFlightFlags = flag | 1;
            Bmcon_bgmStart(0x80000050);
            Bmcon_sound(0x900);
        }
        if (mFlightResult == 1) {
            f32 x = dist.x;
            if (x > 25710.0f && dist.z < 1131.0f && dist.z > -1131.0f) {
                mFlightResult = 0;
                // HD: flight drains or restores the maximum magic amount.
                u8 magic = dComIfGs_getMaxMagic();
                dComIfGp_setItemMagicCount((s16) - (s32)magic);
            } else if (x < -1948.0f || x > 25710.0f || dist.z < -3534.0f || dist.z > 3534.0f) {
                mFlightResult = 2;
                // HD: flight drains or restores the maximum magic amount.
                u8 magic = dComIfGs_getMaxMagic();
                dComIfGp_setItemMagicCount((s16) - (s32)magic);
            }
        }
        if (dComIfGp_getPlayerStatus0() & 0x100000) {
            s16 d = (s16)gabi::ftoi(dist.x / 100.0f);
            mFlyDist = d;
            if (d < 0) {
                mFlightResult = 3;
            }
            dComIfGs_setTmpReg(0xF903, mFlightResult);
            if (mFlightResult == 0 || mFlightResult == 1) {
                mOrderEventNum = 3;
                setFlyDistNow(mFlyDist);
            } else {
                mOrderEventNum = 4;
                setFlyDistNow(mFlyDist);
            }
            if (mFlightResult != 2) {
                s16 max = getFlyDistMax();
                if (mFlyDist > max) {
                    setFlyDistMax(mFlyDist);
                }
            }
            cXyz pt;
            mPathRun.getPoint(&pt, mPathRun.mIdx);
            old.pos.copy(pt);
            mHomeAngleY = -0x6000;
            current.pos.copy(pt);
            current.angle.y = -0x6000;
            u8 flag2 = mFlightFlags;
            if (!(flag2 & 2)) {
                mFlightFlags = flag2 | 2;
                Bmcon_bgmStop(0x5A);
            }
        }
    }
    return mTalk;
}

/* 02205A6C */
void daNpcBmcon_c::executeSetMode(u32 proc) {

    mTargetSpeed = 0.0f;
    mExeMode = Bmcon_callMode(this, &l_execute_init[proc]); /* (this->*l_execute_init[proc])() */
}

/* 02207750 */
s32 daNpcBmcon_c::executeWaitInit() {

    u8 npc = mNpcNo;
    speedF = 0.0f;
    setAnmTbl(&l_anm_tag_tbl[npc == NPC_BMCON1 ? 0 : 1]);
    daNpcBmcon_c__l_npc_dat* dat = &l_npc_dat[mNpcNo];
    m_jnt.setParam(dat->mMaxBackBoneX, dat->mMaxBackBoneY, dat->mMinBackBoneX, dat->mMinBackBoneY,
                   dat->mMaxHeadX, dat->mMaxHeadY, dat->mMinHeadX, dat->mMinHeadY,
                   dat->mMaxTurnStep);
    return 0;
}

/* 022077F4 */
void daNpcBmcon_c::executeWait() {

    if (!executeCommon()) {
        if (mNpcNo == NPC_BMCON2 && mbAttention && !dComIfGs_isEventBit(0x2A40)) {
            mOrderEventNum = 2;
        }
    }
}

/* 02207864 */
s32 daNpcBmcon_c::executeTalkInit() {

    mbTurnOnLook = 1;
    return 1;
}

/* 02207874 */
void daNpcBmcon_c::executeTalk() {

    executeCommon();
    if (talk2(1) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mTalk = 0;
        executeSetMode(0);
        daNpcBmcon_c__l_npc_dat* dat = &l_npc_dat[mNpcNo];
        u8 next = mbStartFlight;
        mbTurnOnLook = dat->mbTurnOnLook;
        mbLookPlayer = dat->mbLookPlayer;
        if (next) {
            mOrderEventNum = 0;
            mbChangingStage = 1;
            Bmcon_nextStage("sea" /* "sea" */, 1, 0xE, 2, 0.0f, 0, 1, 0);
        } else {
            dComIfGp_event_reset();
        }
    } else {
        setAnmFromMsgTag();
    }
}

/* 02207974 */
s32 daNpcBmcon_c::executeWalkInit() {

    setAnmTbl(&l_anm_tag_tbl[5]);
    return 2;
}

/* 022079A0 */
void daNpcBmcon_c::executeWalk() {

    if (executeCommon()) {
        return;
    }
    cXyz pos;
    pos.x = current.pos.x;
    pos.y = current.pos.y;
    pos.z = current.pos.z;
    if (!mPathRun.chkPointPass(&pos, mPathRun.mbDir != 0) || mPathRun.nextIdxAuto()) {
        cXyz pt;
        mPathRun.getPoint(&pt, mPathRun.mIdx);
        cXyz target;
        target.x = pt.x;
        target.y = pt.y;
        target.z = pt.z;
        cXyz own;
        own.y = current.pos.y;
        own.x = current.pos.x;
        own.z = current.pos.z;
        BmconAngle ang;
        Bmcon_distanceAngle(&own, &target, nullptr, &ang);
        s16 a = ang;
        mTargetAngleY = a;
        mbHeadOnly = 0;
        mHomeAngleY = a;
        mLookMode = 2;
        daNpcBmcon_c__l_npc_dat* dat = &l_npc_dat[mNpcNo];
        mTurnSpeed = dat->mWalkTurnSpeed;
        m_jnt.mbTrn = 1;
        mTargetSpeed = dat->mWalkSpeed;
        return;
    }
    /* end of the path: turn round */
    mPathRun.mbDir = mPathRun.mbDir ^ 1;
    cXyz pt;
    mPathRun.getPoint(&pt, mPathRun.mIdx);
    cXyz own;
    own.y = current.pos.y;
    own.x = current.pos.x;
    cXyz target;
    target.y = pt.y;
    target.x = pt.x;
    target.z = pt.z;
    own.z = current.pos.z;
    BmconAngle ang;
    Bmcon_distanceAngle(&own, &target, nullptr, &ang);
    mHomeAngleY = ang;
    mPathRun.setInf(0xFF, fopAcM_GetRoomNo(this), true);
    executeSetMode(0);
}

/* 02207B54 */
s32 daNpcBmcon_c::executeTurnInit() {

    cXyz pt;
    mPathRun.getPoint(&pt, mPathRun.mIdx);
    cXyz own;
    own.y = current.pos.y;
    own.x = current.pos.x;
    cXyz target;
    target.y = pt.y;
    target.z = pt.z;
    target.x = pt.x;
    own.z = current.pos.z;
    BmconAngle ang;
    Bmcon_distanceAngle(&own, &target, nullptr, &ang);
    if (ang != current.angle.y) {
        return 3;
    }
    setAnmTbl(&l_anm_tag_tbl[5]);
    daNpcBmcon_c__l_npc_dat* dat = &l_npc_dat[mNpcNo];
    f32 rnd = cM_rndF((f32)(dat->mTurnWaitMax - dat->mTurnWaitMin));
    dat = &l_npc_dat[mNpcNo];
    mTurnWaitTimer = (s16)gabi::ftoi(rnd + (f32)dat->mTurnWaitMin);
    return 2;
}

/* 02207CB4 */
void daNpcBmcon_c::executeTurn() {

    if (executeCommon()) {
        return;
    }
    cXyz pt;
    mPathRun.getPoint(&pt, mPathRun.mIdx);
    cXyz own;
    own.z = current.pos.z;
    cXyz target;
    target.x = pt.x;
    own.x = current.pos.x;
    target.y = pt.y;
    target.z = pt.z;
    own.y = current.pos.y;
    BmconAngle ang;
    Bmcon_distanceAngle(&own, &target, nullptr, &ang);
    s16 a = ang;
    s16 cur = current.angle.y;
    mLookMode = 2;
    mTargetAngleY = a;
    mbHeadOnly = 0;
    m_jnt.mbTrn = 1;
    if (cur == a) {
        executeSetMode(2);
    }
}

/* 02205AF4 */
void daNpcBmcon_c::checkOrder() {

    u16 cmd = eventInfo.mCommand; /* eventInfo.mCommand */
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

/* 02206954 */
void daNpcBmcon_c::eventOrder() {

    u8 num = mOrderEventNum;
    if (num == 2 || num == 1) {
        eventInfo_onCondition(this, 0x21); /* dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e */
        if (mOrderEventNum == 2) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (num == 3) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        s16 ev = mEventIdx[0];
        Bmcon_orderChangeEvent(player, this, ev, 0, 0xFFFF);
    } else if (num == 4) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        s16 ev = mEventIdx[1];
        Bmcon_orderChangeEvent(player, this, ev, 0, 0xFFFF);
    } else if (num == 6) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        s16 ev = mEventIdx[1];
        Bmcon_orderChangeEvent(player, this, ev, 0, 0xFFFF);
    }
}

/* 022068A8 */
void daNpcBmcon_c::eventMove() {

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

/* 02206650 */
void daNpcBmcon_c::privateCut() {

    const char* staff = l_npc_staff_id[mNpcNo].get();
    s32 staffIdx = dComIfGp_evmng_getMyStaffId(staff, nullptr, 0);
    if (staffIdx == -1) {
        return;
    }
    /* Cut names: MES_SET and GET_ITEM. */
    s8 act = (s8)dComIfGp_evmng_getMyActIdx(staffIdx, cut_name_tbl, 2, TRUE, 0);
    mCutIndex = act;
    dEvent_manager_c* mgr = dComIfGp_getPEvtManager();
    if (act == -1) {
        Bmcon_cutEnd(mgr, staffIdx); /* cutEnd(staffIdx) */
        return;
    }
    if (Bmcon_cutAdvance(mgr, staffIdx) /* getIsAddvance(staffIdx) */) {
        switch ((s8)mCutIndex) {
        case 0:
            eventMesSetInit(staffIdx);
            break;
        case 1:
            eventGetItemInit();
            break;
        }
    }
    switch ((s8)mCutIndex) {
    case 0:
        if (!eventMesSet()) {
            return;
        }
        break;
    }
    dComIfGp_evmng_cutEnd(staffIdx);
}

/* 02205E1C */
void daNpcBmcon_c::eventMesSetInit(int staffIdx) {

    be<u32>* pMsgNo = (be<u32>*)dComIfGp_evmng_getMyIntegerP(staffIdx, "MsgNo" /* "MsgNo" */);
    if (pMsgNo == nullptr) {
        mpMessageCursor = mpMessageCursor.get() + 1;
        setMessage(*mpMessageCursor.get());
        return;
    }
    mpMessageCursor = nullptr;
    switch (*pMsgNo) {
    case 0:
        setMessage(Bmcon_getMessage(this));
        break;
    case 1:
        switch (mFlightResult) {
        case 0: {
            s16 dist = mFlyDist;
            dComIfGp_setMessageCountNumber(dist);
            setMessage(0x2AB2);
            bmcon_startShock(8, 1);
            bmcon_startShock(4, 0x1E);
            Bmcon_sound(0x902);
            break;
        }
        case 1: {
            s16 dist = mFlyDist;
            dComIfGp_setMessageCountNumber(dist);
            setMessage(0x2AB3);
            bmcon_startShock(8, 1);
            bmcon_startShock(4, 0x1E);
            Bmcon_sound(0x901);
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
        switch (mFlightResult) {
        case 0:
            setMessage(0x2AB4);
            break;
        case 1: {
            s16 dist = mFlyDist;
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
                mpMessageCursor = l_msg_result_cleared;
                setMessage(l_msg_result_cleared[0]);
            } else {
                dComIfGs_onEventBit(0x2B40);
                mpMessageCursor = l_msg_result_first;
                setMessage(l_msg_result_first[0]);
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
                setMessage(l_msg_dist[idx]);
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
    if (mpMessageCursor.get() != nullptr) {
        setMessage(*mpMessageCursor.get());
    }
}

/* 022065C4 */
bool daNpcBmcon_c::eventMesSet() {

    u16 status = talk2(0);
    if (status == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        u8 flag = mPresentFlags;
        if (flag & 1) {
            mPresentFlags = flag & ~1;
            mPresentItem = 7;
            mOrderEventNum = 6;
        } else if (flag & 2) {
            mPresentFlags = flag & ~2;
            mPresentItem = 5;
            mOrderEventNum = 6;
        }
    }
    return status == 0x12;
}

/* 02206340 */
void daNpcBmcon_c::eventGetItemInit() {

    u32 id = Bmcon_presentItem(&current.pos, mPresentItem, 0, -1, current.roomNo, nullptr, nullptr);
    if (id != fpcM_ERROR_PROCESS_ID_e) {
        dComIfGp_event_setItemPartnerId(id);
    }
}

/* 022063CC */
u16 daNpcBmcon_c::talk2(int param_1) {

    u16 status = 0xFF;
    /* HD: the message (GameCube mpCurrMsg) is the message manager, read once */
    void* mgr = l_message.get();
    if (mCurrMsgBsPcId == fpcM_ERROR_PROCESS_ID_e) {
        if (param_1 == 1) {
            mCurrMsgNo = Bmcon_getMessage(this);
        }
        mCurrMsgBsPcId = Bmcon_messageSet(mgr, mCurrMsgNo, &eyePos);
        if (mCurrMsgBsPcId != fpcM_ERROR_PROCESS_ID_e) {
            mbHasMsg = 0;
            mPrevMsgStatus = 0xFFFF;
        }
    } else if (mbHasMsg) {
        status = (u16)Bmcon_messageStatus(mgr);
        switch (status) {
        case 0xE: /* fopMsgStts_MSG_DISPLAYED_e */
            Bmcon_setMessageStatus(mgr, Bmcon_nextMessage(this, &mCurrMsgNo));
            if (Bmcon_messageStatus(mgr) == 0xF /* fopMsgStts_MSG_CONTINUES_e */) {
                Bmcon_messageSet(mgr, mCurrMsgNo, nullptr);
            }
            break;
        case 6: /* fopMsgStts_MSG_TYPING_e */
            if (mPrevMsgStatus == 0xF || mPrevMsgStatus == 2 /* fopMsgStts_BOX_OPENING_e */) {
                chkMsg();
            }
            break;
        case 0x12: /* fopMsgStts_BOX_CLOSED_e */
            Bmcon_setMessageStatus(mgr, 0x13 /* fopMsgStts_MSG_DESTROYED_e */);
            mCurrMsgBsPcId = fpcM_ERROR_PROCESS_ID_e;
            break;
        }
        mPrevMsgStatus = status;
        Bmcon_messageAnimation(this, status);
    } else {
        /* HD: mpCurrMsg = fopMsgM_SearchByID(mCurrMsgBsPcId) is a flag */
        mbHasMsg = 1;
    }
    return status;
}

/* 02207D74 */
u16 daNpcBmcon_c::next_msgStatus(be<u32>* pMsgNo) {

    u16 status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    /* HD: mpCurrMsg is the message manager (read once); mSelectNum at +0x948 */
    void* msg = l_message.get();
    if (*pMsgNo == 0x2AB4) {
        *pMsgNo = 0x2AFF;
        return status;
    }
    if (mpMessageCursor.get() == nullptr) {
        return 0x10; /* fopMsgStts_MSG_ENDS_e */
    }
    mpMessageCursor = mpMessageCursor.get() + 1;
    u32 v = *mpMessageCursor.get();
    switch (v) {
    case 0:
        mpMessageCursor = nullptr;
        return 0x10;
    case 1:
        if (Bmcon_view<BmconMessage>(msg)->mSelectNum != 0) {
            mpMessageCursor = l_msg_decline1;
        } else {
            s16 cost = dComIfGp_getMessageRupee();
            if ((s32)dComIfGs_getRupee() < cost) {
                mpMessageCursor = l_msg_no_rupee;
            } else {
                bmcon_pay(this, l_msg_pay1);
            }
        }
        *pMsgNo = *mpMessageCursor.get();
        break;
    case 2:
        if (Bmcon_view<BmconMessage>(msg)->mSelectNum != 0) {
            mpMessageCursor = l_msg_decline2;
        } else {
            s16 cost = dComIfGp_getMessageRupee();
            if ((s32)dComIfGs_getRupee() < cost) {
                mpMessageCursor = l_msg_no_rupee;
            } else {
                bmcon_pay(this, l_msg_pay2);
            }
        }
        *pMsgNo = *mpMessageCursor.get();
        break;
    case 3: {
        // HD: flight drains or restores the maximum magic amount.
        u8 magic = dComIfGs_getMaxMagic();
        dComIfGp_setItemMagicCount(magic);
        mpMessageCursor = nullptr;
        mbStartFlight = 1;
        return 0x10;
    }
    case 4:
        mPresentFlags = mPresentFlags | 1;
        return 0x10;
    case 5:
        mPresentFlags = mPresentFlags | 2;
        return 0x10;
    default:
        *pMsgNo = v;
        break;
    }
    return status;
}

/* 02207FA4 */
u32 daNpcBmcon_c::getMsg() {

    u32 msgNo = 0;
    mpMessageCursor = nullptr;
    if (!dComIfGp_event_chkTalkXY() /* !dComIfGp_event_chkTalkXY() */) {
        switch (mNpcNo) {
        case NPC_BMCON1:
            if (mbStartFlight) {
                mpMessageCursor = l_msg_after0;
            } else if (!dComIfGs_isEventBit(0x2901)) {
                mpMessageCursor = l_msg_first0;
            } else if (isClear()) {
                mpMessageCursor = l_msg_cleared0;
                s16 max = getFlyDistMax();
                dComIfGp_setMessageCountNumber(max);
            } else {
                s16 max = getFlyDistMax();
                dComIfGp_setMessageCountNumber(max);
                mpMessageCursor = l_msg_played0;
            }
            break;
        case NPC_BMCON2:
            if (!dComIfGs_isEventBit(0x2A40)) {
                mpMessageCursor = l_msg_first1;
                dComIfGs_onEventBit(0x2A40);
            } else if (isClear()) {
                mpMessageCursor = l_msg_cleared1;
            } else if (dComIfGs_isEventBit(0x2901)) {
                s16 max = getFlyDistMax();
                dComIfGp_setMessageCountNumber(max);
                mpMessageCursor = l_msg_played1;
            } else {
                mpMessageCursor = l_msg_default1;
            }
            break;
        }
    }
    if (mpMessageCursor.get() != nullptr) {
        msgNo = *mpMessageCursor.get();
    }
    return msgNo;
}

/* 0220639C */
void daNpcBmcon_c::chkMsg() {

    switch (mCurrMsgNo) {
    case 0x2AB2:
        Bmcon_bgmStart(0x80000051);
        break;
    case 0x2AB3:
        Bmcon_bgmStart(0x80000052);
        break;
    }
}

/* 02205E14 */
void daNpcBmcon_c::setMessage(u32 msgNo) { mCurrMsgNo = msgNo; }

/* 0220677C */
void daNpcBmcon_c::setAnmFromMsgTag() {

    switch (dComIfGp_getMesgAnimeAttrInfo() /* dComIfGp_getMesgAnimeTagInfo() */) {
    case 0:
        setAnmTbl(&l_anm_tag_tbl[0]);
        break;
    case 5:
        setAnmTbl(&l_anm_tag_tbl[1]);
        break;
    case 6:
        setAnmTbl(&l_anm_tag_tbl[2]);
        break;
    case 9:
        setAnmTbl(&l_anm_tag_tbl[3]);
        break;
    case 0xE:
        mbLookPlayer = 0;
        setAnmTbl(&l_anm_tag_tbl[4]);
        break;
    case 0x12:
        setAnmTbl(&l_anm_tag_tbl[6]);
        break;
    case 0x13:
        setAnmTbl(&l_anm_tag_tbl[7]);
        break;
    }
    dComIfGp_clearMesgAnimeAttrInfo(); /* dComIfGp_clearMesgAnimeTagInfo() */
}

/* 02204CA4 */
u8 daNpcBmcon_c::getPrmNpcNo() {

    if (0 <= argument && argument < 2) {
        return argument;
    }
    return 0;
}

/* 02204E14 */
u8 daNpcBmcon_c::getPrmRailID() { return daObj_PrmAbstract(this, PRM_RAIL_ID_W, PRM_RAIL_ID_S); }

/* 02205080 */
void daNpcBmcon_c::setMtx() {

    J3DModel* model = mpMorf->getModel();
    cXyz* base_scale = &Bmcon_view<BmconModelScale>(model)->mScale; /* setBaseScale(scale) */
    f32 sz = scale.z, sx = scale.x, sy = scale.y;
    base_scale->x = sx;
    base_scale->y = sy;
    base_scale->z = sz;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}

/* 02205750 */
void daNpcBmcon_c::chkAttention() {

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
        cXyz pos;
        pos.z = current.pos.z;
        pos.y = current.pos.y;
        pos.x = current.pos.x;
        cXyz ppos;
        ppos.x = player->current.pos.x;
        ppos.y = player->current.pos.y;
        f32 dist_max = mAttDist;
        ppos.z = player->current.pos.z;
        s32 ang_max = mAttAngle;
        BmconFloat dist_p;
        BmconAngle ang_p;
        Bmcon_distanceAngle(&pos, &ppos, &dist_p, &ang_p);
        u8 att = mbAttention;
        if (att != 0) {
            dist_max += 40.0f;
            ang_max += 0x71C;
        }
        s16 ang = (s16)(ang_p - shape_angle.y);
        ang_p = ang;
        f32 dist = dist_p;
        s32 abs_ang = ang < 0 ? -ang : ang;
        if (dist_max > dist && ang_max > abs_ang &&
            player->current.pos.y > current.pos.y - 200.0f) {
            cXyz eye;
            Bmcon_playerEyePos(&eye, l_npc_dat[mNpcNo].mPlayerEyeOffset);
            mbHeadOnly = (mbTurnOnLook == 0);
            mLookAtPos.copy(eye);
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
                mLookTimer = l_npc_dat[mNpcNo].mLookTimer;
            }
            daNpcBmcon_c__l_npc_dat* dat = &l_npc_dat[npc];
            if (dat->mLookDist > dist) {
                cXyz eye;
                Bmcon_playerEyePos(&eye, dat->mPlayerEyeOffset);
                mbHeadOnly = (mbTurnOnLook == 0);
                mLookAtPos.copy(eye);
                mLookMode = 1;
                if (mbLookPlayer == 0) {
                    mbHeadOnly = false;
                    mTargetAngleY = mHomeAngleY;
                    m_jnt.mbTrn = 1;
                    mLookMode = 2;
                }
                mbFar = 1;
                mTurnSpeed = l_npc_dat[mNpcNo].mTurnSpeed;
                return;
            }
            s16 t = mLookTimer;
            mLookMode = 0;
            if (t != 0) {
                mLookTimer = t - 1;
                mTurnSpeed = l_npc_dat[mNpcNo].mTurnSpeed;
                return;
            }
            mbHeadOnly = false;
            mTargetAngleY = mHomeAngleY;
            mLookMode = 2;
            m_jnt.mbTrn = 1;
            mTurnSpeed = l_npc_dat[mNpcNo].mTurnSpeed;
            return;
        }
    }
    if (mbAttention == 0) {
        mbAttention = 1;
    }
    mTurnSpeed = l_npc_dat[mNpcNo].mTurnSpeed;
}

/* 02206B58 */
void daNpcBmcon_c::lookBack() {

    s16 targetY = current.angle.y;
    u8 headOnly = mbHeadOnly;
    s16 vel = mTurnSpeed;
    cXyz* dstTemp = nullptr;
    cXyz temp2;
    /* cXyz dstPos = eyePos (through FPRs) */
    f32 eye_x = eyePos.x;
    f32 eye_y = eyePos.y;
    f32 eye_z = eyePos.z;
    switch ((s8)mLookMode) {
    case 1:
        temp2.copy(mLookAtPos);
        dstTemp = &temp2;
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
    cXyz dstPos;
    dstPos.x = eye_x;
    dstPos.y = eye_y;
    dstPos.z = eye_z;
    Bmcon_lookAtTarget(&m_jnt, &current.angle.y, dstTemp, &dstPos, targetY, mTurnStep, headOnly);
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
}

/* 02206A74 */
void daNpcBmcon_c::playAnm() {

    mAnmFlags = mAnmFlags & ~1;
    mpArmMorf->play(nullptr, 0, 0);
    if (mpMorf->play(nullptr, 0, 0)) {
        if (mpAnmDat.get() != nullptr) {
            if (mAnmLoop > 0) {
                mAnmLoop = mAnmLoop - 1;
                if (mAnmLoop == 0) {
                    mpAnmDat = mpAnmDat.get() + 1;
                    if (setAnmTbl(mpAnmDat)) {
                        mAnmFlags = mAnmFlags | 1;
                    }
                } else {
                    setAnm(mpAnmDat->mAnmIndex, 0, 0.0f);
                }
            }
        }
    }
}

/* 02204E40 */
void daNpcBmcon_c::setAnm(u32 anmIdx, int loopMode, f32 morf) {

    f32 ov = mMorfOverride;
    u32 npc = mNpcNo;
    if (!(ov < 0.0f)) {
        morf = ov;
        mMorfOverride = -1.0f;
    }
    J3DAnmTransform* pAnm =
        (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname_tbl[npc].get(), l_bck_ix_tbl[anmIdx]);
    mpMorf->setAnm(pAnm, loopMode, morf, 1.0f, 0.0f, -1.0f, nullptr);
    J3DAnmTransform* pArmAnm = (J3DAnmTransform*)dComIfG_getObjectIDRes(l_arcname_tbl[mNpcNo].get(),
                                                                        l_arm_bck_ix_tbl[anmIdx]);
    mpArmMorf->setAnm(pArmAnm, loopMode, morf, 1.0f, 0.0f, -1.0f, nullptr);
    mAnmIdx = anmIdx;
}

/* 02204FA8 */
bool daNpcBmcon_c::setAnmTbl(sBmconAnmDat* dat) {

    if (dat->mAnmIndex == 0xFF) {
        mpAnmDat = nullptr;
        return true;
    }
    mpAnmDat = dat;
    mAnmLoop = dat->mLoopCount;
    if (mAnmLoop > 0) {
        setAnm(dat->mAnmIndex, 0, (f32)dat->mMorf);
    } else if (mAnmIdx != dat->mAnmIndex) {
        setAnm(dat->mAnmIndex, 2, (f32)dat->mMorf);
    }
    return false;
}

/* 02205074 */
s16 daNpcBmcon_c::XyCheckCB(int) { return 0; }

/* 02205160 */
void daNpcBmcon_c::setCollision(dCcD_Cyl* cyl, cXyz* center, f32 radius, f32 height) {

    cyl->SetC(center);
    cyl->SetR(radius);
    cyl->SetH(height);
    dComIfG_Ccsp_Set(cyl);
}

/* 022071EC */
void daNpcBmcon_c::calcFlyDist(cXyz* result) {

    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    cXyz dist;
    dist.x = link->current.pos.x;
    dist.y = link->current.pos.y;
    dist.z = link->current.pos.z;
    Bmcon_subtract(&dist, &l_startPos /* the platform's start position (static) */, &dist);
    mDoMtx_YrotS(mDoMtx_stack_c::get(), -0x6000);
    cXyz out;
    PSMTXMultVec(mDoMtx_stack_c::get(), &dist, &out);
    /* return out: GHS's copy constructor allocates when the result pointer is NULL */
    if (result == nullptr) {
        result = (cXyz*)operator_new(0xC);
        if (result == nullptr) {
            return;
        }
    }
    /* lfs/stfs pairs: the recompiled code copies the bits (a signalling NaN is not quieted) */
    Bmcon_copyWords(result, &out);
}

/* 02207324 */
s16 daNpcBmcon_c::getFlyDistMax() {

    u8 lo = dSv_event_getEventReg(dComIfGs_getEvent(), 0xA7FF);
    return (s16)(lo + (dSv_event_getEventReg(dComIfGs_getEvent(), 0xA8FF) << 8));
}

/* 0220738C */
void daNpcBmcon_c::setFlyDistMax(s32 dist) {

    u8 hi = dist / 256;
    dSv_event_setEventReg(dComIfGs_getEvent(), 0xA7FF, dist % 256);
    dSv_event_setEventReg(dComIfGs_getEvent(), 0xA8FF, hi);
}

/* 02205DAC */
s16 daNpcBmcon_c::getFlyDistNow() {

    u8 lo = dComIfGs_getTmpReg(0xFBFF);
    return (s16)(lo + (dComIfGs_getTmpReg(0xFAFF) << 8));
}

/* 022072A0 */
void daNpcBmcon_c::setFlyDistNow(s32 dist) {

    u8 hi = dist / 256;
    dComIfGs_setTmpReg(0xFBFF, dist % 256);
    dComIfGs_setTmpReg(0xFAFF, hi);
}

/* 02205BF4 */
BOOL daNpcBmcon_c::chkEndEvent() {

    switch (mNpcNo) {
    case NPC_BMCON1:
        if (dComIfGp_evmng_endCheck(mEventIdx[0]) && dComIfGs_getTmpReg(0xF903) != 0) {
            dComIfGp_event_reset();
            mbLookPlayer = l_npc_dat[mNpcNo].mbLookPlayer;
            return TRUE;
        }
        if (dComIfGp_evmng_endCheck(mEventIdx[1])) {
            dComIfGp_event_reset();
            mbLookPlayer = l_npc_dat[mNpcNo].mbLookPlayer;
            return TRUE;
        }
        break;
    case NPC_BMCON2:
        if (dComIfGp_evmng_endCheck(mEventIdx[0]) || dComIfGp_evmng_endCheck(mEventIdx[1])) {
            dComIfGp_event_reset();
            mbLookPlayer = l_npc_dat[mNpcNo].mbLookPlayer;
            return TRUE;
        }
        break;
    default:
        return FALSE;
    }
    return FALSE;
}

/* 02207F6C */
BOOL daNpcBmcon_c::isClear() { return dComIfGs_isEventBit(0x2B40) ? TRUE : FALSE; }

/* 022056D8 */
static cPhs_State daNpc_BmconCreate(void* i_this) {

    return static_cast<daNpcBmcon_c*>(i_this)->_create();
}

/* 0220574C */
static BOOL daNpc_BmconDelete(void* i_this) {

    return static_cast<daNpcBmcon_c*>(i_this)->_delete();
}

/* 02207054 */
static BOOL daNpc_BmconExecute(void* i_this) {

    return static_cast<daNpcBmcon_c*>(i_this)->_execute();
}

/* 022071E8 */
static BOOL daNpc_BmconDraw(void* i_this) { return static_cast<daNpcBmcon_c*>(i_this)->_draw(); }

/* 02208270 */
static BOOL daNpc_BmconIsDelete(void* i_this) { return true; }

/* 02204CC0 */
static daNpcBmcon_c* daNpcBmcon_c_ct(daNpcBmcon_c* i_this) {

    if (i_this == nullptr) {
        i_this = (daNpcBmcon_c*)operator_new(0x93C);
        if (i_this == nullptr) {
            return i_this;
        }
    }
    Bmcon_npcCtor(i_this);
    i_this->__vtbl = 0x10017CC8;
    Bmcon_btpCtor(i_this->mBtpAnm);
    i_this->mNpcNo = i_this->getPrmNpcNo();
    i_this->mbHeadOnly = 1;
    i_this->mMorfOverride = -1.0f;
    i_this->mUnusedTimer = 0;
    i_this->mTargetSpeed = 0.0f;
    i_this->mLookMode = 0;
    i_this->mExeMode = 0;
    i_this->mAnmIdx = 1;
    i_this->mResFlag = 0;
    i_this->mHomeAngleY = i_this->home.angle.y;
    i_this->mTurnWaitTimer = 0;
    i_this->mbChangingStage = 0;
    i_this->mFlightResult = 1;
    return i_this;
}

/* 02208184 */

static void __sinit_d_a_npc_bmcon1_cpp() {

    sinit_header_statics_z(0x104665E4, 0x101BD260, 0x10466608);
    cXyz* start = &l_startPos; /* the platform's start position (calcFlyDist) */
    start->x = 297080.0f;
    start->y = 1100.0f;
    start->z = -202920.0f;
    cXyz* p2 = &l_coursePos;
    p2->x = 278900.0f;
    p2->y = 1100.0f;
    p2->z = -221100.0f;
}

/* 0220825C */
static void SafeString_dt(void* p, s32 flags) {

    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 02208278 */

static void daNpcBmcon_dt(daNpcBmcon_c* p, s32 flags) {

    if (p != nullptr) {
        dCcD_Cyl_dt(&p->mCyl, 2);                                             /* ~dCcD_Cyl */
        dCcD_Stts_dt(&p->mStts, 2);                                           /* ~dCcD_Stts */
        Bmcon_circleDestroy(&p->mAcchCir.m_cir, 2);                           /* ~cM3dGCir */
        Bmcon_view<BmconAcchVtables>(&p->mObjAcch)->mPolyVtable = 0x10017A88; /* ~dBgS_ObjAcch */
        Bmcon_view<BmconAcchVtables>(&p->mObjAcch)->mCheckVtable = 0x10017A98;
        Bmcon_acchDestroy(&p->mObjAcch, 0);
        Bmcon_actorDestroy(p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(p);
        }
    }
}

/* 02208314 */
static void SafeString_assureTerminationImpl(void*) {}

/* 02208318 */
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift) {

    /* PowerPC slw/srw: shift counts 32..63 give 0 */
    u32 mask = ((i_width & 0x20) ? 0u : (1u << (i_width & 0x1F))) - 1;
    u32 prm = i_actor->mParameters;
    return ((i_shift & 0x20) ? 0u : (prm >> (i_shift & 0x1F))) & mask;
}

/* WWHD glue
str 10017B00 "head" in 022046B4
str 10017B20 "d_a_npc_bmcon1.cpp" in 022046B4
str 10017B34 "m_jnt.getHeadJntNum() >= 0" in 022046B4
str 10017B50 "backbone" in 022046B4
str 10017B5C "m_jnt.getBackboneJntNum() >= 0" in 022046B4
str 10017B08 "neck" in 022046B4
str 10017B7C "m_nec_jnt_num >= 0" in 022046B4
str 10017B10 "armL" in 022046B4
str 10017B90 "m_arm_L_jnt_num >= 0" in 022046B4
str 10017B18 "armR" in 022046B4
str 10017BA8 "m_arm_R_jnt_num >= 0" in 022046B4
str 10017AF0 "armLloc" in 022046B4
str 10017BC0 "m_armL1_jnt_num >= 0" in 022046B4
str 10017AF8 "armRloc" in 022046B4
str 10017BD8 "m_armR1_jnt_num >= 0" in 022046B4
str 10017C24 "BMCON_RESULT" in 022051F0
str 10017C34 "BMCON_GET_ITEM" in 022051F0
str 10017C18 "BMCON_END" in 022051F0
str 10017C44 "BMCON_END2" in 022051F0
str 10017C54 "MsgNo" in 02205E1C
str 10017C9C "sea" in 02207874
ext 02055B64 s16 Bmcon_calcTimer(be<s16>*)
ext 0259D54C void Bmcon_playerEyePos(cXyz*,f32)
ext 0259DED0 void Bmcon_lookAtTarget(dNpc_JntCtrl_c*,be<s16>*,cXyz*,cXyz*,s16,s16,u8)
ext 0259D624 void Bmcon_distanceAngle(cXyz*,cXyz*,be<f32>*,be<s16>*)
ext 02525FE4 cPhs_State Bmcon_phaseHandler(request_of_phase_process_class*,void*,void*)
ext 025A1458 void Bmcon_npcCtor(void*)
ext 025E7820 void Bmcon_btpCtor(void*)
ext 025D7970 BOOL Bmcon_orderChangeEvent(fopAc_ac_c*,fopAc_ac_c*,s16,u16,u16)
ext 025DD868 BOOL Bmcon_isCreating(u32)
ext 025F795C u32 Bmcon_messageStatus(void*)
ext 025F74D0 void Bmcon_setMessageStatus(void*,u32)
ext 025F7DB0 u32 Bmcon_messageSet(void*,u32,cXyz*)
ext 025E1988 void Bmcon_sound(u32)
ext 025E18EC void Bmcon_bgmStart(u32)
ext 025E1904 void Bmcon_bgmStop(u32)
ext 025D7DEC u32 Bmcon_presentItem(cXyz*,s32,u8,s32,s32,csXyz*,cXyz*)
ext 02543280 void Bmcon_cutEnd(dEvent_manager_c*,s32)
ext 025447C8 BOOL Bmcon_cutAdvance(dEvent_manager_c*,s32)
ext 028E8DAC void Bmcon_subtract(const cXyz*,const cXyz*,cXyz*)
ext 0252012C void Bmcon_nextStage(const char*,s16,s8,s8,f32,u32,s32,s8)
ext 02018034 void Bmcon_circleDestroy(void*,s32)
ext 024EFD9C void Bmcon_acchDestroy(dBgS_Acch*,s32)
ext 025D50BC void Bmcon_actorDestroy(daNpcBmcon_c*,s32)
ext 025CB374 void Bmcon_shock(dVibration_c*,s32,s32,cXyz*)
*/
