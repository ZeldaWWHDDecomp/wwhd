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
#include "d/d_path.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1001E640,
    .objAcchDt = {0x1001E658, 0x1001E668},
    .headerStatics = {0x10467CA4, 0x101C2244},
};

static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift);

using MnAnimationPointer = gptr<sMnAnmDat>;
static sMnAnmDat* l_msgAnm[4] = {0x101C2054, 0x101C205A, 0x101C205D, 0x101C2057};
static dCcD_SrcCyl l_cyl_src;
static u32 l_messageManager = 0;
static s32 l_bmd_ix_tbl[1] = {dRes_ID_MN_BDL_MN_e};
static s32 l_etc_bmd_ix_tbl[1] = {dRes_ID_MN_BDL_MN_BAG_e};
static s32 l_btp_ix_tbl[1] = {dRes_ID_MN_BTP_MN_MABA_e};
static s32 l_bck_ix_tbl[8] = {
    dRes_ID_MN_BCK_MN_WAIT01_e,  dRes_ID_MN_BCK_MN_WAIT02_e,  dRes_ID_MN_BCK_MN_TALK01_e,  dRes_ID_MN_BCK_MN_TALK02_e,
     dRes_ID_MN_BCK_MN_WALK_e,  dRes_ID_MN_BCK_MN_BIKKURI_e,  dRes_ID_MN_BCK_MN_JUMP01_e,  dRes_ID_MN_BCK_MN_JUMP02_e,
};
static char* l_arcname_tbl[1] = {"Mn"};
static char* l_npc_staff_id[1] = {"Mn"};
static char* l_room_name[10] = {
    "sea",  "Pfigure",  "figureA",  "figureB",
     "figureC",  "figureD",  "figureE",  "figureF",
     "figureG",  "K_Testb",
};
static char* cut_name_tbl[10] = {
    "MES_SET",  "GET_ITEM",  "WAIT",  "HATCH",
     "BIKKURI",  "TURN",  "WALK",  "LOOK",
     "JUMP",  "SWON",
};
static u16 l_figure_comp[17] = {
    0x95ff,  0x94ff,  0x93ff,  0x92ff,
     0x91ff,  0x90ff,  0x8fff,  0x8eff,
     0x8dff,  0x8cff,  0xb1ff,  0x9cff,
     0x84ff,  0x83ff,  0x82ff,  0x81ff,
     0x80ff,
};
static u32 l_msg_mn_1st_talk[4] = {0x35e9, 0x35ea, 0x35eb, 0x0};
static u32 l_msg_mn_2nd_talk[4] = {0x35ec, 0x35ed, 0x35ee, 0x0};
static u32 l_msg_mn_1st_talk_in[2] = {0x35f1, 0x0};
static u32 l_msg_mn_2nd_talk_in[2] = {0x35f2, 0x0};
static u32 l_msg_mn_3rd_talk_in[2] = {0x35f0, 0x0};
static u32 l_msg_mn_comp_1st[4] = {0x35f3, 0x35f4, 0x35f5, 0x0};
static u32 l_msg_mn_comp_2nd[2] = {0x35f6, 0x0};
static u32 l_msg_mn_figure[10] = {
    0x35f7,  0x35f8,  0x35f9,  0x35fa,
     0x35fb,  0x35fc,  0x35fd,  0x35fe,
     0x35ff,  0x3600,
};
static sMnAnmDat l_npc_anm_wait[1] = {{0, 8, -1}};
static sMnAnmDat l_npc_anm_wait2[1] = {{1, 8, -1}};
static sMnAnmDat l_npc_anm_talk[1] = {{2, 8, -1}};
static sMnAnmDat l_npc_anm_talk2[1] = {{3, 8, -1}};
static sMnAnmDat l_npc_anm_walk[1] = {{4, 8, -1}};
static sMnAnmDat l_npc_anm_bikkuri[2] = {{5, 8, 1}, {0, 8, -1}};
static sMnAnmDat l_npc_anm_jump1[2] = {{6, 8, 1}, {255, 8, 0}};
static NpcDatStruct l_npc_dat[2] = {
    {
        4000,  10000,  0,  6000,
         -2000,  -10000,  -6000,  -4000,
         1500,  0,  0.0f,  150.0f,
         120.0f,  250.0f,  0.0f,  32767,
         1300,  400,  0,  60.0f,
         0.5f,  3.0f,  6,  1024,
         60,  90,  200,  300,
         30,  1,  1,
    },
    {
        4000,  10000,  0,  6000,
         -2000,  -10000,  -6000,  -4000,
         1500,  0,  0.0f,  150.0f,
         120.0f,  250.0f,  0.0f,  32767,
         1300,  400,  0,  60.0f,
         0.5f,  1.5f,  6,  1024,
         60,  90,  200,  300,
         30,  1,  1,
    },
};

/* 0229BF48 */
static daNpcMn_c* daNpcMn_c_ct(daNpcMn_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daNpcMn_c*)operator_new(0x93C);
        if (i_this == nullptr) {
            return i_this;
        }
    }
    fopNpc_npc_c_ct(i_this);
    i_this->__vtbl = MN_VTBL;
    mDoExt_btpAnm_ct(&i_this->mBtpAnm);
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

/* 0229B8E0 */
static BOOL daNpc_Mn_nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == 0) {
        J3DModel* model = j3dSys.getModel();
        daNpcMn_c* i_this = static_cast<daNpcMn_c*>(model->getUserArea());
        u32 jntNo = reinterpret_cast<J3DJoint*>(node)->getJntNo();
        PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
        if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[0][1]);
            mDoMtx_ZrotM(calc_mtx(), -i_this->m_jnt.mAngles[0][0]);
        }
        if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
            mDoMtx_XrotM(calc_mtx(), i_this->m_jnt.mAngles[1][1]);
            mDoMtx_ZrotM(calc_mtx(), -i_this->m_jnt.mAngles[1][0]);
        }

        mtx_copy(model->getAnmMtx(jntNo), calc_mtx());
        PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
    }
    return TRUE;
}

/* 0229BE28 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daNpcMn_c*)i_this)->createHeap();
}

/* 0229C22C */
static cPhs_State phase_1(daNpcMn_c* i_this) {

    if (!(i_this->actor_condition & fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            daNpcMn_c_ct(i_this);
        }
        i_this->actor_condition = i_this->actor_condition | fopAcCnd_INIT_e;
    }
    if (i_this->mPosFlag == 0) {
        dComIfGs_setEventReg(MN_EVENT_ROOM, 0);
        if (fopAcM_isSwitch(i_this, i_this->getPrmSwitchBit())) {
            return cPhs_UNK3_e;
        }
    } else {
        u8 eventReg = dComIfGs_getEventReg(MN_EVENT_ROOM);
        if (i_this->isChangePos(eventReg)) {
            if (dComIfGs_isEventBit(MN_EVENT_ALLOW_ROOM_CHANGE)) {
                eventReg = i_this->getPosNo();
                // HD: JUT_ASSERT(0x3A1, posNo < 10)
                if (!(eventReg < 10)) {
                    JUT_ASSERT_fail("d_a_npc_mn.cpp", 0x3A1, "posNo >= daNpcMn_c::Pos_Flg_Out && posNo < daNpcMn_c::Pos_Flg_Max");
                }
                dComIfGs_setEventReg(MN_EVENT_ROOM, eventReg);
            } else {
                eventReg = 1;
                dComIfGs_setEventReg(MN_EVENT_ROOM, eventReg);
            }
        }
        // HD: JUT_ASSERT(0x3A7, posNo < 10)
        if (!(eventReg < 10)) {
            JUT_ASSERT_fail("d_a_npc_mn.cpp", 0x3A7, "posNo >= 0 && posNo < sizeof(l_room_name)/sizeof(l_room_name[0])");
        }
        if (eventReg != i_this->mPosFlag) {
            return cPhs_UNK3_e;
        }
    }
    i_this->setResFlag(1);
    return cPhs_NEXT_e;
}

/* 0229CA5C */
static cPhs_State phase_2(daNpcMn_c* i_this) {
    cPhs_State state = dComIfG_resLoad(i_this->getPhaseP(), mn_arcname());
    if (state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, 0x0229BE28 , 0)) {
            return i_this->createInit();
        }
        i_this->mpMorf = nullptr;
        return cPhs_ERROR_e;
    }
    return state;
}

/* 0229CAE4 */
cPhs_State daNpcMn_c::_create() {

    static u32 l_method[3] = {0x0229C22C, 0x0229CA5C, 0};
    return dComLbG_PhaseHandler(&mPhsMethod, gabi::ea(l_method), this);
}

/* 0229BB3C */
BOOL daNpcMn_c::createHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(mn_arcname(), l_bmd_ix_tbl[0] );
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(mn_arcname(), l_bck_ix_tbl[mBckIdx]);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2 , 1.0f, 0, -1, 1, nullptr,
                                    0x80000, 0x15021222);
    m_jnt.mHeadJntNum = JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "head" );
    if (!(m_jnt.mHeadJntNum >= 0)) {
        JUT_ASSERT_fail("d_a_npc_mn.cpp", 0x3ED, "m_jnt.getHeadJntNum() >= 0");
    }
    m_jnt.mBackboneJntNum = JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "backbone" );
    if (!(m_jnt.mBackboneJntNum >= 0)) {
        JUT_ASSERT_fail("d_a_npc_mn.cpp", 0x3F1, "m_jnt.getBackboneJntNum() >= 0");
    }
    if (!initTexPatternAnm(false)) {
        return FALSE;
    }
    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            modelData->getJointNodePointer(i)->setCallBack(daNpc_Mn_nodeCallBack);
        }
    }
    // HD: setUserArea through a null-checked getModel()
    if (mpMorf->getModel() != nullptr) {
        mpMorf->getModel()->mUserArea = gabi::ea(this);
    }
    mAcchCir.SetWall(30.0f, 30.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    J3DModelData* etc_modelData = (J3DModelData*)dComIfG_getObjectIDRes(mn_arcname(), l_etc_bmd_ix_tbl[0] );
    mpModel = mDoExt_J3DModel__create(etc_modelData, 0x80000, 0x11000002);
    if (mpModel.get() == nullptr) {
        return FALSE;
    }
    mShoulderRJoint = JUTNameTab_getIndex(gabi::ea(modelData->getJointName()), "shoulderR" );
    return TRUE;
}

/* 0229C5C4 */
static s16 daNpcMn_XyCheckCB(void* i_this, int i_itemBtn) {
    return static_cast<daNpcMn_c*>(i_this)->XyCheckCB(i_itemBtn);
}

/* 0229C738 */
cPhs_State daNpcMn_c::createInit() {
    int weight = 0xFF;
    u8 railID = getPrmRailID();
    if (railID != 0xFF) {
        mPathRun.setInf(railID, fopAcM_GetRoomNo(this), true);
        dPath* path = mPathRun.mPath;
        if (path == nullptr) {
            return cPhs_ERROR_e;
        }
        // HD: no dPath_GetNextRoomPath

        actor_status = actor_status & ~0x80;
        u8 currPointIdx = 0;
        while (currPointIdx < mPathRun.maxPoint()) {
            if (reinterpret_cast<path_layout::dPath*>(path)->m_points[currPointIdx].mArg0 == 0) {
                break;
            }
            currPointIdx++;
        }
        if (currPointIdx == mPathRun.maxPoint()) {
            currPointIdx = 0;
        }
        mPathRun.mIdx = currPointIdx;
        gabi::Local<cXyz> point;
        mPathRun.getPoint(point.get(), currPointIdx);
        old.pos.copy(*point);
        current.pos.copy(*point);
        mPathRun.incIdxLoop();
        mWaitTimer = 1;
        weight = 0xFE;
    }
    gravity = -9.0f;
    setAnmTbl(l_npc_anm_wait);
    mHatchEventIdx = dComIfGp_evmng_getEventIdx("FIGURE_HATCH_OPEN" , 0xFF);
    eventInfo.setXyCheckCB(daNpcMn_XyCheckCB);
    mEventCut.setActorInfo2(l_npc_staff_id[0], this);
    mTalkOrder = 0;
    mTurnVel = 0;
    mbPlayerAttention = 0;
    mbNearPlayer = 0;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel()));
    fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAA;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAA;
    attention_info.flags = fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e;
    NpcDatStruct* dat = mn_npc_dat(mNpcNo);
    m_jnt.setParam(dat->mMax_backbone_x, dat->mMax_backbone_y, dat->mMin_backbone_x, dat->mMin_backbone_y,
                   dat->mMax_head_x, dat->mMax_head_y, dat->mMin_head_x, dat->mMin_head_y, dat->mMax_turn_step);
    dat = mn_npc_dat(mNpcNo);
    mbAllowBodyTurn = dat->mbAllowBodyTurn;
    mbLookOnly = dat->mbLookOnly;
    mAttnDist = dat->mAttnDist;
    mAttnAngle = dat->mAttnAngle;
    mObjAcch.CrrPos(dComIfG_Bgsp());
    f32 gnd = mObjAcch.GetGroundH();
    if (-1000000000.0f  != gnd) {
        current.pos.y = gnd;
        home.pos.y = gnd;
    }
    setMtx();
    J3DModel_calc(mpMorf->getModel());
    mStts.Init(weight, 0xFF, this);
    mCyl.Set(&l_cyl_src );
    mCyl.SetStts(&mStts);
    gabi::Local<cXyz> center;
    center->x = current.pos.x;
    center->y = current.pos.y;
    center->z = current.pos.z;
    setCollision(&mCyl, center.get(), mn_npc_dat(mNpcNo)->mCylRadius, 150.0f);
    return cPhs_COMPLEATE_e;
}

/* 0229CAFC */
bool daNpcMn_c::_delete() {
    // HD: dComIfG_resDeleteDemo is resDelete
    dComIfG_resDelete(&mPhs, mn_arcname());
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    if (reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mNextStage.enable != 0  && mn_isNextStage("sea")) {
        dComIfGs_setEventReg(MN_EVENT_ROOM, 0);
    }
    return true;
}

/* 0229E72C */
bool daNpcMn_c::_draw() {
    if (dComIfGs_isTmpBit(0x0408)) {
        return true;
    }
    J3DModel* morfModel = mpMorf->getModel();
    J3DModelData* morfModelData = morfModel->getModelData();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morfModel, &tevStr);
    mDoExt_btpAnm_entry(&mBtpAnm, morfModelData, mBtpFrame);
    mpMorf->updateDL();
    reinterpret_cast<MnModelDataView*>(morfModelData)->mpTexPattern = 0;
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);

    mtx_copy(&mpModel->mBaseTransformMtx, morfModel->getAnmMtx(mShoulderRJoint));
    mDoExt_modelUpdateDL(mpModel);
    // HD: no dComIfGd_setShadow

    dSnap_RegistFig(DSNAP_TYPE_NPC_MN , this, 1.0f, 1.0f, 1.0f);
    return true;
}

/* 0229BF38 */
u8 daNpcMn_c::getPrmNpcNo() {
    return mPosFlag != 0;
}

/* 0229C3AC */
u8 daNpcMn_c::getPrmRailID() {
    return daObj_PrmAbstract(this, PRM_RAIL_W, PRM_RAIL_S);
}

/* 0229C000 */
u8 daNpcMn_c::getPrmSwitchBit() {
    return daObj_PrmAbstract(this, PRM_SWITCH_W, PRM_SWITCH_S);
}

/* 0229D728 */
u8 daNpcMn_c::getPrmSwitchBit2() {
    return daObj_PrmAbstract(this, PRM_SWITCH2_W, PRM_SWITCH2_S);
}

/* 0229C5C8 */
void daNpcMn_c::setMtx() {
    J3DModel* model = mpMorf->getModel();
    cXyz* base_scale = &model->mBaseScale;
    f32 sx = scale.x, sy = scale.y, sz = scale.z;
    base_scale->x = sx;
    base_scale->y = sy;
    base_scale->z = sz;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
}

/* 0229CC34 */
void daNpcMn_c::chkAttention() {
    mbNearPlayer = 0;
    if (mEventCut.mbAttention) {
        mLookAtPos.x = mEventCut.mPos.x;
        mLookAtPos.z = mEventCut.mPos.z;
        mLookAtPos.y = mEventCut.mPos.y;
        mLookMode = LOOK_MODE_ATTN;
        if (mbAllowBodyTurn) {
            mHeadOnlyFollow = false;
            m_jnt.mbTrn = 1;
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
            mn_playerEyePos(eye.get(), mn_npc_dat(mNpcNo)->mPlayerEyeOfsY);
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
                mn_playerEyePos(eye.get(), mn_npc_dat(mNpcNo)->mPlayerEyeOfsY);
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

/* 0229BA38 */
BOOL daNpcMn_c::initTexPatternAnm(u32 modify) {
    J3DModelData* modelData = mpMorf->getModel()->getModelData();
    m_head_tex_pattern = (J3DAnmTexPattern*)dComIfG_getObjectIDRes(mn_arcname(), l_btp_ix_tbl[0] );
    if (m_head_tex_pattern.get() == nullptr) {
        JUT_ASSERT_fail("d_a_npc_mn.cpp", 0xA44, "m_head_tex_pattern != (0)");
    }
    if (!mDoExt_btpAnm_init(&mBtpAnm, modelData, m_head_tex_pattern, 1, 2, 1.0f, 0, -1, modify, FALSE)) {
        return FALSE;
    }
    mBtpFrame = 0;
    mBtpTimer = 0;
    return TRUE;
}

/* 0229C3D8 */
void daNpcMn_c::setAnm(u32 i_bckIdx, int i_loopMode, f32 i_morf) {
    if (!(mAnmMorfOverride < 0.0f)) {
        i_morf = mAnmMorfOverride;
        mAnmMorfOverride = -1.0f;
    }
    J3DAnmTransform* pAnm = (J3DAnmTransform*)dComIfG_getObjectIDRes(mn_arcname(), l_bck_ix_tbl[i_bckIdx]);
    mpMorf->setAnm(pAnm, i_loopMode, i_morf, 1.0f, 0.0f, -1.0f, nullptr);
    mBckIdx = i_bckIdx;
}

/* 0229C4E4 */
bool daNpcMn_c::setAnmTbl(sMnAnmDat* i_anmDat) {
    mAnmFlag = mAnmFlag & 0xFE;
    if (i_anmDat->mBckIdx == BCK_NULL) {
        mpAnmDat = nullptr;
        return true;
    }
    mpAnmDat = i_anmDat;
    mAnmLoopCnt = i_anmDat->mLoopCount;
    if (mAnmLoopCnt > 0) {
        setAnm(i_anmDat->mBckIdx, 0 , (f32)i_anmDat->mMorf);
    } else if (mBckIdx != i_anmDat->mBckIdx) {
        setAnm(i_anmDat->mBckIdx, 2 , (f32)i_anmDat->mMorf);
    }
    return false;
}

/* 0229C5BC */
s16 daNpcMn_c::XyCheckCB(int) {
    return false;
}

/* 0229C038 */
int daNpcMn_c::getRand(int i_max) {
    int rnd = gabi::ftoi(cM_rndF((f32)i_max));
    if (rnd == i_max) {
        rnd = 0;
    }
    return rnd;
}

/* 0229C6A8 */
void daNpcMn_c::setCollision(dCcD_Cyl* i_cyl, cXyz* i_center, f32 i_radius, f32 i_height) {
    i_cyl->SetC(i_center);
    i_cyl->SetR(i_radius);
    i_cyl->SetH(i_height);
    dComIfG_Ccsp_Set(i_cyl);
}

/* 0229BE2C */
u8 daNpcMn_c::chkPosNo() {
    u8 posNo = 0;
    // HD: strcmp is a sead::SafeString comparison; l_room_name[posNo] is the left operand

    for (int i = 10; i != 0; i--) {
        if (mn_isNextStage(l_room_name[posNo])) {
            return posNo;
        }
        posNo++;
    }
    return posNo;
}

/* 0229C0A4 */
u8 daNpcMn_c::getPosNo() {
    s32 roomCnt[8];
    if (dComIfGs_isEventBit(MN_EVENT_COMPLETE) && !dComIfGs_isEventBit(MN_EVENT_COMPLETE_TALK)) {
        return 1;
    }
    roomCnt[0] = 1;
    for (int i = 1; i < 8; i++) {
        roomCnt[i] = 0;
    }
    for (int fig = 0; fig < 0x86; fig++) {
        if (fig / 8 < 0x11) {
            int bit = fig % 8;
            u8 reg = dComIfGs_getEventReg(l_figure_comp[fig / 8]);
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

/* 0229C02C */
BOOL daNpcMn_c::isChangePos(u32 i_posNo) {
    return i_posNo == 0;
}

/* 0229CAF8 */
static cPhs_State daNpc_MnCreate(void* i_this) {
    return static_cast<daNpcMn_c*>(i_this)->_create();
}

/* 0229CC30 */
static BOOL daNpc_MnDelete(void* i_this) {
    return static_cast<daNpcMn_c*>(i_this)->_delete();
}

/* 0229E728 */
static BOOL daNpc_MnExecute(void* i_this) {
    return static_cast<daNpcMn_c*>(i_this)->_execute();
}

/* 0229E89C */
static BOOL daNpc_MnDraw(void* i_this) {
    return static_cast<daNpcMn_c*>(i_this)->_draw();
}

/* 0229F798 */
static BOOL daNpc_MnIsDelete(void*) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.

/* 0229F6F0 */
static void __sinit_d_a_npc_mn_cpp() {
    sinit_header_statics(0x10467CA4, 0x101C2244);
}

/* 0229F784 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 0229F7A0 */
static void daNpcMn_dt(daNpcMn_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(&p->mCyl);
        __dt(&p->mStts);
        __dt(&p->mAcchCir);
        __dt(&p->mObjAcch);
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1) {
            operator_delete(p);
        }
    }
}

/* 0229F83C */
static void SafeString_assureTerminationImpl(void*) {
}

/* 0229F840 */
static u32 daObj_PrmAbstract(fopAc_ac_c* i_actor, u32 i_width, u32 i_shift) {
    /* PowerPC slw/srw: shift counts 32..63 give 0 */
    u32 mask = ((i_width & 0x20) ? 0u : (1u << (i_width & 0x1F))) - 1;
    u32 prm = i_actor->mParameters;
    return ((i_shift & 0x20) ? 0u : (prm >> (i_shift & 0x1F))) & mask;
}

/* WWHD glue (tools/verify/autoglue.py)
ext 025E789C s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry)
ext 027DF9B0 s8 JUTNameTab_getIndex(u32 tab, const char* name)
ext 02525FE4 cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class* p, u32 tbl, void* self)
ext 025A1458 void fopNpc_npc_c_ct(void* p)
ext 025E7820 void mDoExt_btpAnm_ct(void* p)
ext 025BD64C s32 dSnap_GetFigRoomId(s32 fig)
ext 0259D624 void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* ang)
ext 0259D54C void mn_playerEyePos(cXyz* out, f32 offs)
ext 0259DED0 void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 025D9F38 fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm)
ext 025D7970 BOOL fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind)
ext 025D7B24 BOOL fopAcM_orderPotentialEvent(fopAc_ac_c* a, u16 type, u16 flag, u16 p)
ext 025D7DEC u32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale)
ext 02055B64 s16 cLib_calcTimer(be<s16>* t)
ext 025F795C u32 msgMng_getStatus(u32 mgr)
ext 025F74D0 void msgMng_setStatus(u32 mgr, u32 st)
ext 025F7DB0 u32 msgMng_messageSet(u32 mgr, u32 msgNo, cXyz* pos)
ext 025F7E68 u32 msgMng_scopeMessageSet(u32 mgr, u32 msgNo)

data l_bmd_ix_tbl 1001E62C
data l_etc_bmd_ix_tbl 1001E630
data l_btp_ix_tbl 1001E634
data l_bck_ix_tbl 1001E678
data l_arcname_tbl 101C2040
data l_npc_staff_id 101C203C
data l_room_name 101C20BC
data cut_name_tbl 101C221C
data l_figure_comp 101C2154
data l_msg_mn_1st_talk 101C2064
data l_msg_mn_2nd_talk 101C2074
data l_msg_mn_1st_talk_in 101C201C
data l_msg_mn_2nd_talk_in 101C2024
data l_msg_mn_3rd_talk_in 101C202C
data l_msg_mn_comp_1st 101C2084
data l_msg_mn_comp_2nd 101C2034
data l_msg_mn_figure 101C2094
data l_npc_anm_wait 101C2054
data l_npc_anm_wait2 101C2057
data l_npc_anm_talk 101C205A
data l_npc_anm_talk2 101C205D
data l_npc_anm_walk 101C2060
data l_npc_anm_bikkuri 101C2044
data l_npc_anm_jump1 101C204C
data l_npc_dat 101C2178
str 1001E6B8 in 0229BA38
str 1001E69C in 0229BA38
str 1001E6CC in 0229BB3C
str 1001E6E0 in 0229BB3C
str 1001E6F0 in 0229BB3C
str 1001E70C in 0229BB3C
str 1001E6E0 in 0229BB3C
str 1001E718 in 0229BB3C
str 1001E6D4 in 0229BB3C
str 1001E748 in 0229C22C
str 1001E758 in 0229C22C
str 1001E748 in 0229C22C
str 1001E79C in 0229C22C
str 1001E800 in 0229C738
data l_method 101C2210
data l_msgAnm 1001E8D8
ext 026067F4 void* mn_getResourceWithRegisters(void* control, SafeString* key, s32 id, u32 r6, u32 r7)
data l_messageManager 101F4B5C nocheck
str 1001E814 in 0229CAFC
data l_cyl_src 101EA190 nocheck
*/
