/**
 * d_a_npc_bms1_heap.cpp (WWHD)
 * NPC - Bomb-Master Cannon: node callbacks, heap, create, matrices, execute, draw.
 *
 * Written from the WWHD code (the GameCube TU is
 * "Nonmatching"); see d_a_npc_bms1.cpp.
 */
#include "d/actor/d_a_npc_bms1.h"

/* 022083A4 */
static BOOL nodeCallBack_Bms(J3DNode* i_node, int i_calcTiming) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(j3dSys_getModel_l());
        daNpc_Bms1_c* i_this = gabi::at<daNpc_Bms1_c>((u32)model->mUserArea); /* getUserArea */
        u32 jntNo = ((J3DJoint*)i_node)->getJntNo();
        if (i_this != nullptr) {
            PSMTXCopy(J3DModel_getAnmMtx_l(model, jntNo), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_head_jnt_num) {
                gabi::Local<cXyz> offset;
                offset->x = 0.0f;
                offset->y = 0.0f;
                offset->z = 0.0f;
                cMtx_YrotM(calc_mtx(),
                           (s16) - (i_this->mJntCtrl.mAngles[0][1] + i_this->mHeadAnm.field_0x00.y));
                cMtx_ZrotM(calc_mtx(),
                           (s16) - (i_this->mHeadAnm.field_0x00.x + i_this->mJntCtrl.mAngles[0][0]));
                gabi::Local<cXyz> pos;
                MtxPosition(offset.get(), pos.get());
                i_this->mAttnBasePos.set(pos->x, pos->y, pos->z); /* setAttentionBasePos */
                offset->x = 28.0f;
                offset->y = -20.0f;
                offset->z = 0.0f;
                MtxPosition(offset.get(), pos.get());
                i_this->eyePos.set(pos->x, pos->y, pos->z); /* setEyePos */
            } else if (jntNo == (u32)(s32)i_this->m_backbone_jnt_num) {
                cMtx_XrotM(calc_mtx(), i_this->mJntCtrl.mAngles[1][1]);
                cMtx_ZrotM(calc_mtx(), i_this->mJntCtrl.mAngles[1][0]);
            }
            PSMTXCopy(calc_mtx(), j3dSys_mCurrentMtx_l());
            mtx_copy(J3DModel_getAnmMtx_l(model, jntNo), calc_mtx()); /* model->setAnmMtx(jntNo, *calc_mtx) */
        }
    }
    return TRUE;
}

/* the hair joint of nodeCallBack_BmsHead (inline, once per side): the joint's scale, its
 * rotation by a quaternion that follows a damped spring tip, and a stretch factor */
struct Quat_l {
    be<f32> x, y, z, w;
};
static inline void bms1_hair(J3DModel* model, u32 jntNo, f32 sx, f32 sy, f32 sz, be<f32>* stretch,
                             cXyz* tipPos, cXyz* tipVel, be<f32>* quat) {
    Mtx34* stk = mDoMtx_stack_c::get();
    mDoMtx_stack_scaleM(sx, sy, sz);
    gabi::Local<Mtx34> rot;
    PSMTXCopy(stk, rot.get());
    f32 ty = rot->m[1][3];
    f32 tz = rot->m[2][3];
    f32 tx = rot->m[0][3];
    rot->m[2][3] = 0.0f;
    rot->m[0][3] = 0.0f;
    rot->m[1][3] = 0.0f;
    PSMTXTrans(stk, tx, ty, tz);
    mDoMtx_stack_quatM_l(quat);
    PSMTXConcat_l(stk, rot.get(), stk);
    gabi::Local<cXyz> tip;
    PSMTXMultVec(stk, bms1HairTip(), tip.get());
    gabi::Local<cXyz> base;
    PSMTXMultVec(stk, bms1HairBase(), base.get());
    mtx_copy(J3DModel_getAnmMtx_l(model, jntNo), stk);
    if (cXyz_isZero_l(tipPos)) {
        tipPos->copy(*tip);
    }
    gabi::Local<cXyz> diff;
    cXyz_mi(tip.get(), diff.get(), tipPos);
    gabi::Local<cXyz> acc;
    cXyz_ml(diff.get(), acc.get(), bms1HIO().mHairSpring);
    PSVECAdd(tipVel, acc.get(), tipVel);
    PSVECScale(tipVel, tipVel, bms1HIO().mHairDamp);
    PSVECAdd(tipPos, tipVel, tipPos);
    gabi::Local<cXyz> dir;
    dir->copy(*tipPos);
    PSVECSubtract_l(tip.get(), base.get(), tip.get());
    PSVECSubtract_l(dir.get(), base.get(), dir.get());
    gabi::Local<Quat_l> q;
    daObj_quat_rotVec_l(q.get(), tip.get(), dir.get());
    C_QUATSlerp_l(quat, q.get(), quat, bms1HIO().mHairSlerp);
    f32 dot = PSVECDotProduct(dir.get(), tipVel);
    f32 s = gabi::fnmsubs(bms1HIO().mHairStretch, dot, 1.0f);
    f32 r;
    if (s < 0.5f) {
        r = 0.5f;
    } else {
        r = (s - 1.5f >= 0.0f) ? 1.5f : s;
    }
    *stretch = r;
}

/* 02208634 */
static BOOL nodeCallBack_BmsHead(J3DNode* i_node, int i_calcTiming) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(j3dSys_getModel_l());
        daNpc_Bms1_c* i_this = gabi::at<daNpc_Bms1_c>((u32)model->mUserArea);
        u32 jntNo = ((J3DJoint*)i_node)->getJntNo();
        if (i_this != nullptr) {
            // HD: preserve guarded initialization and the original component store order.
            cXyz* l_base = bms1HairBase();
            if (bms1HairBaseGuard() == 0) {
                f32 x = cXyz_Zero->x, z = cXyz_Zero->z, y = cXyz_Zero->y;
                l_base->x = x;
                l_base->z = z;
                l_base->y = y;
                bms1HairBaseGuard() = 1;
            }
            // HD: the tip guard is stored between its y and x components.
            cXyz* l_tip = bms1HairTip();
            if (bms1HairTipGuard() == 0) {
                l_tip->y = 0.0f;
                bms1HairTipGuard() = 1;
                l_tip->x = 40.0f;
                l_tip->z = 0.0f;
            }
            PSMTXCopy(J3DModel_getAnmMtx_l(model, jntNo), mDoMtx_stack_c::get());
            if (jntNo == (u32)(s32)i_this->m_hairL_jnt_num) {
                f32 sx = i_this->mHairScaleL.x * i_this->mHairStretchL;
                bms1_hair(model, jntNo, sx, i_this->mHairScaleL.y, i_this->mHairScaleL.z,
                          &i_this->mHairStretchL, &i_this->mHairPosL, &i_this->mHairVelL, i_this->mHairQuatL);
            } else if (jntNo == (u32)(s32)i_this->m_hairR_jnt_num) {
                /* the right side stretches along y */
                f32 sy = i_this->mHairScaleR.y * i_this->mHairStretchR;
                bms1_hair(model, jntNo, i_this->mHairScaleR.x, sy, i_this->mHairScaleR.z,
                          &i_this->mHairStretchR, &i_this->mHairPosR, &i_this->mHairVelR, i_this->mHairQuatR);
            }
        }
    }
    return TRUE;
}

/* 02209438 */
void daNpc_Bms1_c::set_mtx() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 type = mType;
    J3DModel* model = mpMorf->getModel();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    if (type == 0) {
        mDoMtx_stack_c::transM(0.0f, 100.0f, 35.0f);
    }
    mDoMtx_stack_copy_to_model(model);
    PSMTXCopy(J3DModel_getAnmMtx_l(model, m_head_jnt_num), mDoMtx_stack_c::get());
    mDoMtx_stack_copy_to_model(mpHeadModel);
    if (mType == 1) {
        PSMTXCopy(J3DModel_getAnmMtx_l(model, m_head_jnt_num), mDoMtx_stack_c::get());
        mDoMtx_stack_copy_to_model(mpHeadAccessory);
        PSMTXCopy(J3DModel_getAnmMtx_l(model, m_head_jnt_num), mDoMtx_stack_c::get());
        mDoMtx_stack_copy_to_model(mpHeadAccessory2);
    } else if (dComIfGs_isEventBit(0xA02)) {
        PSMTXCopy(J3DModel_getAnmMtx_l(model, m_head_jnt_num), mDoMtx_stack_c::get());
        mDoMtx_stack_copy_to_model(mpHeadAccessory);
    }
    if (mpBodyAccessory.get() != nullptr) {
        PSMTXCopy(J3DModel_getAnmMtx_l(model, mAccessoryJoint), mDoMtx_stack_c::get());
        mDoMtx_stack_copy_to_model(mpBodyAccessory);
    }
    if (mpShopModel.get() != nullptr) {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_copy_to_model(mpShopModel);
    }
}

/* 022098D0 */
BOOL daNpc_Bms1_c::CreateInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    mInitialAngle.x = current.angle.x;
    mInitialAngle.y = current.angle.y;
    mInitialAngle.z = current.angle.z;
    gravity = -30.0f;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAD; /* attention_info.distances[SPEAK] */
    attention_info.flags = 0xA;                               /* attention_info.flags */
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAD;  /* attention_info.distances[TALK] */
    switch (mType) {
    case 0:
        setAction(PMF_wait_action, nullptr);
        break;
    case 1:
        setAction(PMF_wait_action, nullptr);
        break;
    }
    mAttnBasePos.copy(current.pos);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(bms1CylinderSource());
    mShopInitialized = 0;
    mNextMsgNo = 0;
    mBoughtItemNo = 0xFF;
    mCyl.SetStts(&mStts);
    mTriggerHandled = 0;
    s16 shopIdx = checkItemGet(0x69, 1) ? 2 : 1;
    f32 r0;
    if (mType != 1 && checkItemGet(0x69, 1)) {
        mHairScaleL.x = 0.4f;
        mHairScaleL.z = 0.4f;
        mHairScaleR.x = 0.4f;
        mHairScaleL.y = 0.4f;
        r0 = 0.4f;
        mHairScaleR.y = 0.4f;
        mHairScaleR.z = r0;
    } else {
        mHairScaleL.y = 0.9f;
        mHairScaleL.x = 1.2f;
        mHairScaleR.x = 0.7f;
        mHairScaleL.z = 0.9f;
        mHairScaleR.y = 0.6f;
        mHairScaleR.z = 0.6f;
    }
    mHairStretchR = 1.0f;
    mHairStretchL = 1.0f;
    // HD: copy each identity word immediately after loading it; do not batch the loads.
    bms1_cpp::QuaternionWords* leftWords = reinterpret_cast<bms1_cpp::QuaternionWords*>(mHairQuatL);
    for (u32 i = 0; i < 4; ++i) {
        leftWords->mWords[i] = bms1IdentityQuaternion()->mWords[i];
    }
    bms1_cpp::QuaternionWords* rightWords = reinterpret_cast<bms1_cpp::QuaternionWords*>(mHairQuatR);
    for (u32 i = 0; i < 4; ++i) {
        rightWords->mWords[i] = bms1IdentityQuaternion()->mWords[i];
    }
    mShopItems.mItemSetListGlobalIdx = shopIdx; /* setItemDataIdx */
    mShopCam.mCurrActionFunc.mVtableIndex = 0;  /* setCamAction(NULL) */
    mShopCam.mCurrActionFunc.mFunction = 0;
    mShopCam.mCurrActionFunc.mThisDelta = 0;
    mShopCam.mCamDataIdx = shopIdx;
    mShopItems.setItemSetDataList();
    mShopItems.createItem(3, current.roomNo);
    mbDemo = 0;
    mPlayerHeight = current.pos.y;
    dNpc_EventCut_setActorInfo_l(&mEventCut, "Bms1" /* "Bms1" */, this);
    mEventCut.mpJntCtrl = &mJntCtrl;
    mpMorf->calc();
    set_mtx();
    return TRUE;
}

/* 0220A954 */
BOOL daNpc_Bms1_c::_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModel* model = mpMorf->getModel();
    u32 headData = J3DModel_modelData_l(mpHeadModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    mDoExt_btpAnm_entry(&mBtpAnm, gabi::at<J3DModelData>(headData), mFrame);
    setLightTevColorType(dKy_getEnvlight(), mpHeadModel, &tevStr);
    mpMorf->entryDL();
    mDoExt_modelUpdateDL(mpHeadModel, 0);
    if (mType == 1) {
        setLightTevColorType(dKy_getEnvlight(), mpHeadAccessory, &tevStr);
        mDoExt_modelUpdateDL(mpHeadAccessory, 0);
        setLightTevColorType(dKy_getEnvlight(), mpHeadAccessory2, &tevStr);
        mDoExt_modelUpdateDL(mpHeadAccessory2, 0);
    } else if (dComIfGs_isEventBit(0xA02)) {
        setLightTevColorType(dKy_getEnvlight(), mpHeadAccessory, &tevStr);
        mDoExt_modelUpdateDL(mpHeadAccessory, 0);
    }
    if (mpBodyAccessory.get() != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), mpBodyAccessory, &tevStr);
        mDoExt_modelUpdateDL(mpBodyAccessory, 0);
    }
    if (mpShopModel.get() != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), mpShopModel, &tevStr);
        mDoExt_modelUpdateDL(mpShopModel, 0);
    }
    gabi::at<bms1_cpp::ModelMaterialAnimation>(headData)->mpTexturePattern = 0; /* mBtpAnm.remove(headData) */
    if (mShopItems.mSelectedItemIdx >= 0) {
        mpShopCursor->draw();
    }
    gabi::Local<cXyz> pos;
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    pos->x = x;
    pos->z = z;
    if (mType != 1) {
        pos->y = y;
        dSnap_RegistFig_l(0x5C /* DSNAP_TYPE_NPC_BMS1 */, this, pos.get(), current.angle.y, 1.0f, 1.0f, 1.0f);
    } else {
        pos->y = y - 90.0f;
        dSnap_RegistFig_l(0x5C, this, pos.get(), current.angle.y, 1.0f, 1.0f, 1.0f);
    }
    return TRUE;
}

/* 0220A7AC */
BOOL daNpc_Bms1_c::_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    daNpc_Bms1_HIO_c& hio = bms1HIO();
    mJntCtrl.setParam(hio.mMaxBackboneX, hio.mMaxBackboneY, hio.mMinBackboneX, hio.mMinBackboneY,
                      hio.mMaxHeadX, hio.mMaxHeadY, hio.mMinHeadX, hio.mMinHeadY, hio.mMaxTurnStep);
    playTexPatternAnm();
    mAnimationLooped = (u8)mpMorf->play(&eyePos, 0, 0);
    mpMorf->calc();
    mDoExt_McaMorf* morf = mpMorf;
    if (morf->getFrame() < mPrevFrame) {
        morf = mpMorf;
        mAnimationLooped = 1;
    }
    mPrevFrame = morf->getFrame();
    if (!demo_move()) {
        checkOrder();
        callAction();
        mShopCam.move();
        mShopItems.Item_Move();
        eventOrder();
    }
    mHeadAnm.move();
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    mAcch.CrrPos(dComIfG_Bgsp());
    tevStr.mRoomNo = (s8)bms1BackgroundRoom(&mAcch); /* dBgS::GetRoomId */
    tevStr.mEnvrIdxOverride = (u8)bms1BackgroundEnvironment(&mAcch);
    set_mtx();
    setCollision();
    return TRUE;
}

/* 02209DB4 */
cPhs_State daNpc_Bms1_c::_create() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* fopAcM_ct(this, daNpc_Bms1_c): the HD constructor, inline */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            mHeadAnm.field_0x00.x = 0;
            mHeadAnm.field_0x1C = 0;
            mHeadAnm.field_0x18 = 0.0f;
            mHeadAnm.field_0x1E = 0;
            mHeadAnm.field_0x20 = 0;
            __vtbl = BMS1_VTBL;
            mHeadAnm.field_0x00.y = 0;
            mHeadAnm.field_0x00.z = 0;
            mHeadAnm.field_0x14 = 0.0f;
            mDoExt_btpAnm::ct(&mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mAcch, {0x10017D24, 0x10017D44, 0x10017D34});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x10017D14);
            bms1_jointControlConstruct(&mJntCtrl); /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            bms1_eventCutConstruct(&mEventCut);    /* dNpc_EventCut_c::dNpc_EventCut_c */
            mStickControl.__vtbl = 0x10050788;
            STControl_setWaitParm_l(&mStickControl, 0xF, 0xF, 0, 0, 0.9f, 0.5f, 0, 0x2000);
            STControl_init_l(&mStickControl);
            bms1_shopCamConstruct(&mShopCam);     /* ShopCam_action_c::ShopCam_action_c */
            bms1_shopItemsConstruct(&mShopItems); /* ShopItems_c::ShopItems_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    s8 type = (s8)(mParameters >> 24);
    mType = type;
    BOOL evt = dComIfGs_isEventBit(0xA02);
    bool createError;
    if (type == 0) {
        createError = evt && !checkItemGet(0x69, 1);
    } else {
        createError = !evt || checkItemGet(0x69, 1);
    }
    if (createError) {
        mbCreateErr = 1;
        return cPhs_ERROR_e;
    }
    {
        mbCreateErr = 0;
        cPhs_State phase = dComIfG_resLoad(&mPhs, BMS1_ARC);
        if (phase == cPhs_COMPLEATE_e) {
            mType = (mType != 0) ? 1 : 0;
            if (!fopAcM_entrySolidHeap(this, 0x02209434 /* CheckCreateHeap */, 0x18000)) {
                return cPhs_ERROR_e;
            }
            cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
            daNpc_Bms1_HIO_c& hio = bms1HIO();
            if (hio.m8 < 0) {
                hio.mNo = mDoHIO_createChild("\203{\203\200\211\256\223X\210\365", &hio);
            }
            hio.m8 = hio.m8 + 1;
            if (!CreateInit()) {
                return cPhs_ERROR_e;
            }
        }
        return phase;
    }
}

/* 02208CDC */
BOOL daNpc_Bms1_c::CreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModelData* modelData;
    mDoExt_McaMorf* morf;
    if (mType == 0) {
        modelData = (J3DModelData*)bms1_getRes(0x1D);
        J3DAnmTransform* bck;
        if (!checkItemGet(0x69, 1)) {
            bck = (J3DAnmTransform*)bms1_getRes(0x13);
        } else {
            bck = (J3DAnmTransform*)bms1_getRes(0x14);
        }
        morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2, 1.0f, 0, -1, 1, nullptr,
                                      0, 0x11020203);
    } else {
        modelData = (J3DModelData*)bms1_getRes(0x1A);
        J3DAnmTransform* bck = (J3DAnmTransform*)bms1_getRes(0x12);
        mAnimationRepeats = 3;
        morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2, 1.0f, 0, -1, 1, nullptr,
                                      0, 0x11020203);
    }
    if (morf != nullptr) {
        mpMorf = morf;
    }
    if (morf == nullptr || morf->getModel() == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    m_head_jnt_num = J3DModelData_getJointIndex_l(modelData, "head");
    if (m_head_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bms1.cpp", 0x842, "m_head_jnt_num >= 0");
    m_backbone_jnt_num = J3DModelData_getJointIndex_l(modelData, "backbone");
    if (m_backbone_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bms1.cpp", 0x845, "m_backbone_jnt_num >= 0");
    if (mType == 0) {
        mAccessoryJoint = J3DModelData_getJointIndex_l(modelData, "center");
        if (mAccessoryJoint < 0)
            JUT_ASSERT_fail("d_a_npc_bms1.cpp", 0x84A, "m_leg_jnt_num >= 0");
    }
    J3DModelData* headData = (J3DModelData*)bms1_getRes(0x23);
    mpHeadModel = mDoExt_J3DModel__create(headData, 0x80000, 0x11020002);
    if (mpHeadModel.get() == nullptr)
        return FALSE;
    m_hairL_jnt_num = J3DModelData_getJointIndex_l(headData, "hairL");
    if (m_hairL_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bms1.cpp", 0x86C, "m_hairL_jnt_num >= 0");
    m_hairR_jnt_num = J3DModelData_getJointIndex_l(headData, "hairR");
    if (m_hairR_jnt_num < 0)
        JUT_ASSERT_fail("d_a_npc_bms1.cpp", 0x86F, "m_hairR_jnt_num >= 0");
    mTexIdx = 1;
    if (!initTexPatternAnm(0))
        return FALSE;
    s8 type = mType;
    if (type == 0) {
        if (!dComIfGs_isEventBit(0xA02)) {
            setTexAnm(0);
        } else {
            setTexAnm(1);
        }
    } else if (type == 1) {
        setTexAnm(1);
    }
    mpHeadAccessory = mDoExt_J3DModel__create((J3DModelData*)bms1_getRes(0x1B), 0, 0x11020203);
    if (mpHeadAccessory.get() == nullptr)
        return FALSE;
    mpHeadAccessory2 = mDoExt_J3DModel__create((J3DModelData*)bms1_getRes(0x1C), 0, 0x11020203);
    if (mType == 0) {
        if (dComIfGs_isEventBit(0xA02)) {
            mpBodyAccessory = mDoExt_J3DModel__create((J3DModelData*)bms1_getRes(0x1F), 0, 0x11020203);
            if (mpBodyAccessory.get() == nullptr)
                return FALSE;
        } else {
            mpBodyAccessory = mDoExt_J3DModel__create((J3DModelData*)bms1_getRes(0x1E), 0, 0x11020203);
            if (mpBodyAccessory.get() == nullptr)
                return FALSE;
        }
        mpShopModel = mDoExt_J3DModel__create((J3DModelData*)bms1_getRes(0x20), 0, 0x11020203);
        if (mpShopModel.get() == nullptr)
            return FALSE;
    } else {
        mpBodyAccessory = nullptr;
    }
    for (u16 i = 0; i < J3DModelData_getJointNum_l(gabi::ea(modelData)); i = (u16)(i + 1)) {
        if (i == (u32)(s32)m_head_jnt_num || i == (u32)(s32)m_backbone_jnt_num) {
            J3DModelData_setJointCallBack_l(J3DModel_modelData_l(mpMorf->getModel()), i,
                                            0x022083A4 /* nodeCallBack_Bms */);
        }
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea */
    for (u16 i = 0; i < J3DModelData_getJointNum_l(J3DModel_modelData_l(mpHeadModel)); i = (u16)(i + 1)) {
        if (i == (u32)(s32)m_hairL_jnt_num || i == (u32)(s32)m_hairR_jnt_num) {
            J3DModelData_setJointCallBack_l(J3DModel_modelData_l(mpHeadModel), i,
                                            0x02208634 /* nodeCallBack_BmsHead */);
        }
    }
    mpHeadModel->mUserArea = gabi::ea(this);
    mAcchCir.SetWall(30.0f, 0.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    void* cursorBrk = bms1_getRes(0xB);
    void* cursorMdl = bms1_getRes(8);
    mpShopCursor = ShopCursor_create_l(cursorMdl, cursorBrk, bms1HIO().mCursorScale[0]);
    if (mpShopCursor.get() == nullptr)
        return FALSE;
    return TRUE;
}

// clang-format off
/* WWHD glue
str 101BD3A0 "Bms"
str 10017EE4 "\203{\203\200\211\256\223X\210\365" in 02209DB4
str 10017EDC "Bms1" in 022098D0
str 10017EA4 "m_hairR_jnt_num >= 0" in 02208CDC
str 10017E58 "d_a_npc_bms1.cpp" in 02208CDC
str 10017E14 "hairR" in 02208CDC
str 10017E8C "m_hairL_jnt_num >= 0" in 02208CDC
str 10017E0C "hairL" in 02208CDC
str 10017E78 "m_leg_jnt_num >= 0" in 02208CDC
str 10017E24 "center" in 02208CDC
str 10017E2C "m_backbone_jnt_num >= 0" in 02208CDC
str 10017E6C "backbone" in 02208CDC
str 10017E44 "m_head_jnt_num >= 0" in 02208CDC
str 10017E1C "head" in 02208CDC
*/
// clang-format on
