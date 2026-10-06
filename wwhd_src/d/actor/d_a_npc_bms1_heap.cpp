/**
 * d_a_npc_bms1_heap.cpp (WWHD)
 * NPC - Bomb-Master Cannon: node callbacks, heap, create, matrices, execute, draw.
 *
 * Written from the WWHD code (the GameCube TU is
 * "Nonmatching"); see d_a_npc_bms1.cpp.
 */
#include "d/actor/d_a_npc_bms1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void PSMTXConcat_l(Mtx34* a, Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline void PSVECSubtract_l(cXyz* a, cXyz* b, cXyz* ab) { gabi::call(0x028E8DAC, a, b, ab); }
static inline void mDoMtx_stack_quatM_l(be<f32>* q) { gabi::call(0x025F25CC, q); }
static inline void daObj_quat_rotVec_l(void* q, cXyz* a, cXyz* b) { gabi::call(0x023127F8, q, a, b); }
static inline void C_QUATSlerp_l(void* p, void* q, void* r, f32 t) { gabi::call(0x028E9BC0, p, q, r, t); }
static inline bool cXyz_isZero_l(cXyz* v) { return gabi::call<bool>(0x0201B4E0, v); }
/* HD dSnap_RegistFig(type, actor, const cXyz& pos, s16 angleY, f32, f32, f32) */
static inline void dSnap_RegistFig_l(s32 type, fopAc_ac_c* a, cXyz* pos, s16 angY, f32 x, f32 y, f32 z) {
    gabi::call(0x025BEBB8, type, a, pos, angY, x, y, z);
}
static inline void dNpc_EventCut_setActorInfo_l(dNpc_EventCut_c* c, const char* name, fopAc_ac_c* a) { gabi::call(0x0259F7D4, c, name, a); }
static inline void STControl_setWaitParm_l(STControl_l* s, s16 a, s16 b, s16 c, s16 d, f32 e, f32 f, s16 g, s16 h) {
    gabi::call(0x025885C4, s, a, b, c, d, e, f, g, h);
}
static inline void STControl_init_l(STControl_l* s) { gabi::call(0x025885E8, s); }
static inline void mDoMtx_stack_copy_to_model(J3DModel* m) { J3DModel_setBaseTRMtx(m, mDoMtx_stack_c::get()); }

/* 022083A4 */
static BOOL nodeCallBack_Bms(J3DNode* i_node, int i_calcTiming) {
    WWHD_FUNC(0x022083A4, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(j3dSys_getModel_l());
        daNpc_Bms1_c* i_this = gabi::at<daNpc_Bms1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea */
        u32 jntNo = gabi::load<u16>(gabi::ea(J3DNode_toJoint(i_node)) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(J3DModel_getAnmMtx_l(model, jntNo), calc_mtx());
            if (jntNo == (u32)(s32)i_this->m_head_jnt_num) {
                gabi::Local<cXyz> offset;
                offset->x = 0.0f;
                offset->y = 0.0f;
                offset->z = 0.0f;
                cMtx_YrotM(calc_mtx(), (s16)-(i_this->mJntCtrl.mAngles[0][1] + i_this->mHeadAnm.field_0x00.y));
                cMtx_ZrotM(calc_mtx(), (s16)-(i_this->mHeadAnm.field_0x00.x + i_this->mJntCtrl.mAngles[0][0]));
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
VERIFY(0x022083A4, nodeCallBack_Bms);

/* the hair joint of nodeCallBack_BmsHead (inline, once per side): the joint's scale, its
 * rotation by a quaternion that follows a damped spring tip, and a stretch factor */
struct Quat_l {
    be<f32> x, y, z, w;
};
static inline void bms1_hair(J3DModel* model, u32 jntNo, f32 sx, f32 sy, f32 sz, be<f32>* stretch, cXyz* tipPos,
                             cXyz* tipVel, be<f32>* quat) {
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
    PSMTXMultVec(stk, gabi::at<cXyz>(0x10466644), tip.get());
    gabi::Local<cXyz> base;
    PSMTXMultVec(stk, gabi::at<cXyz>(0x10466638), base.get());
    mtx_copy(J3DModel_getAnmMtx_l(model, jntNo), stk);
    if (cXyz_isZero_l(tipPos)) {
        tipPos->copy(*tip);
    }
    gabi::Local<cXyz> diff;
    cXyz_mi(tip.get(), diff.get(), tipPos);
    gabi::Local<cXyz> acc;
    cXyz_ml(diff.get(), acc.get(), l_HIO().mHairSpring);
    PSVECAdd(tipVel, acc.get(), tipVel);
    PSVECScale(tipVel, tipVel, l_HIO().mHairDamp);
    PSVECAdd(tipPos, tipVel, tipPos);
    gabi::Local<cXyz> dir;
    dir->copy(*tipPos);
    PSVECSubtract_l(tip.get(), base.get(), tip.get());
    PSVECSubtract_l(dir.get(), base.get(), dir.get());
    gabi::Local<Quat_l> q;
    daObj_quat_rotVec_l(q.get(), tip.get(), dir.get());
    C_QUATSlerp_l(quat, q.get(), quat, l_HIO().mHairSlerp);
    f32 dot = PSVECDotProduct(dir.get(), tipVel);
    f32 s = gabi::fnmsubs(l_HIO().mHairStretch, dot, 1.0f);
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
    WWHD_FUNC(0x02208634, BOOL, i_node, i_calcTiming);
    if (i_calcTiming == 0) {
        J3DModel* model = gabi::at<J3DModel>(j3dSys_getModel_l());
        daNpc_Bms1_c* i_this = gabi::at<daNpc_Bms1_c>(gabi::load<u32>(gabi::ea(model) + 0xB8));
        u32 jntNo = gabi::load<u16>(gabi::ea(J3DNode_toJoint(i_node)) + 4);
        if (i_this != nullptr) {
            /* static cXyz l_base = cXyz::Zero (guard 0x104666B0, object 0x10466638) */
            cXyz* l_base = gabi::at<cXyz>(0x10466638);
            if (gabi::load<u32>(0x104666B0) == 0) {
                f32 x = cXyz_Zero->x, z = cXyz_Zero->z, y = cXyz_Zero->y;
                l_base->x = x;
                l_base->z = z;
                l_base->y = y;
                gabi::store<u32>(0x104666B0, 1);
            }
            /* static cXyz l_tip(40, 0, 0) (guard 0x104666B4, object 0x10466644) */
            cXyz* l_tip = gabi::at<cXyz>(0x10466644);
            if (gabi::load<u32>(0x104666B4) == 0) {
                l_tip->y = 0.0f;
                gabi::store<u32>(0x104666B4, 1);
                l_tip->x = 40.0f;
                l_tip->z = 0.0f;
            }
            PSMTXCopy(J3DModel_getAnmMtx_l(model, jntNo), mDoMtx_stack_c::get());
            if (jntNo == (u32)(s32)i_this->m_hairL_jnt_num) {
                f32 sx = i_this->mHairScaleL.x * i_this->mHairStretchL;
                bms1_hair(model, jntNo, sx, i_this->mHairScaleL.y, i_this->mHairScaleL.z, &i_this->mHairStretchL,
                          &i_this->mHairPosL, &i_this->mHairVelL, i_this->mHairQuatL);
            } else if (jntNo == (u32)(s32)i_this->m_hairR_jnt_num) {
                /* the right side stretches along y */
                f32 sy = i_this->mHairScaleR.y * i_this->mHairStretchR;
                bms1_hair(model, jntNo, i_this->mHairScaleR.x, sy, i_this->mHairScaleR.z, &i_this->mHairStretchR,
                          &i_this->mHairPosR, &i_this->mHairVelR, i_this->mHairQuatR);
            }
        }
    }
    return TRUE;
}
VERIFY(0x02208634, nodeCallBack_BmsHead);

/* 02208CDC */
BOOL daNpc_Bms1_c::CreateHeap() {
    WWHD_FUNC(0x02208CDC, BOOL, this);
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
        morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2, 1.0f, 0, -1, 1, nullptr, 0, 0x11020203);
    } else {
        modelData = (J3DModelData*)bms1_getRes(0x1A);
        J3DAnmTransform* bck = (J3DAnmTransform*)bms1_getRes(0x12);
        mA02 = 3;
        morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, 2, 1.0f, 0, -1, 1, nullptr, 0, 0x11020203);
    }
    if (morf != nullptr) {
        mpMorf = morf;
        if (morf->getModel() != nullptr)
            goto ok;
    }
    mpMorf = nullptr;
    return FALSE;
ok:
    m_head_jnt_num = J3DModelData_getJointIndex_l(modelData, STR(0x10017E1C));
    if (m_head_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10017E58), 0x842, STR(0x10017E44));
    m_backbone_jnt_num = J3DModelData_getJointIndex_l(modelData, STR(0x10017E6C));
    if (m_backbone_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10017E58), 0x845, STR(0x10017E2C));
    if (mType == 0) {
        m_jnt84C = J3DModelData_getJointIndex_l(modelData, STR(0x10017E24));
        if (m_jnt84C < 0)
            JUT_ASSERT_fail(STR(0x10017E58), 0x84A, STR(0x10017E78));
    }
    J3DModelData* headData = (J3DModelData*)bms1_getRes(0x23);
    mpHeadModel = mDoExt_J3DModel__create(headData, 0x80000, 0x11020002);
    if (mpHeadModel.get() == nullptr)
        return FALSE;
    m_hairL_jnt_num = J3DModelData_getJointIndex_l(headData, STR(0x10017E0C));
    if (m_hairL_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10017E58), 0x86C, STR(0x10017E8C));
    m_hairR_jnt_num = J3DModelData_getJointIndex_l(headData, STR(0x10017E14));
    if (m_hairR_jnt_num < 0)
        JUT_ASSERT_fail(STR(0x10017E58), 0x86F, STR(0x10017EA4));
    mTexIdx = 1;
    if (!initTexPatternAnm(0))
        return FALSE;
    s8 type = mType;
    if (type == 0) {
        if (!dComIfGs_isEventBit_l(0xA02)) {
            setTexAnm(0);
        } else {
            setTexAnm(1);
        }
    } else if (type == 1) {
        setTexAnm(1);
    }
    mpModel4C8 = mDoExt_J3DModel__create((J3DModelData*)bms1_getRes(0x1B), 0, 0x11020203);
    if (mpModel4C8.get() == nullptr)
        return FALSE;
    mpModel4CC = mDoExt_J3DModel__create((J3DModelData*)bms1_getRes(0x1C), 0, 0x11020203);
    if (mType == 0) {
        if (dComIfGs_isEventBit_l(0xA02)) {
            mpModel4D0 = mDoExt_J3DModel__create((J3DModelData*)bms1_getRes(0x1F), 0, 0x11020203);
            if (mpModel4D0.get() == nullptr)
                return FALSE;
        } else {
            mpModel4D0 = mDoExt_J3DModel__create((J3DModelData*)bms1_getRes(0x1E), 0, 0x11020203);
            if (mpModel4D0.get() == nullptr)
                return FALSE;
        }
        mpModel4D4 = mDoExt_J3DModel__create((J3DModelData*)bms1_getRes(0x20), 0, 0x11020203);
        if (mpModel4D4.get() == nullptr)
            return FALSE;
    } else {
        mpModel4D0 = nullptr;
    }
    for (u16 i = 0; i < J3DModelData_getJointNum_l(gabi::ea(modelData)); i = (u16)(i + 1)) {
        if (i == (u32)(s32)m_head_jnt_num || i == (u32)(s32)m_backbone_jnt_num) {
            J3DModelData_setJointCallBack_l(J3DModel_modelData_l(mpMorf->getModel()), i, 0x022083A4 /* nodeCallBack_Bms */);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea */
    for (u16 i = 0; i < J3DModelData_getJointNum_l(J3DModel_modelData_l(mpHeadModel)); i = (u16)(i + 1)) {
        if (i == (u32)(s32)m_hairL_jnt_num || i == (u32)(s32)m_hairR_jnt_num) {
            J3DModelData_setJointCallBack_l(J3DModel_modelData_l(mpHeadModel), i, 0x02208634 /* nodeCallBack_BmsHead */);
        }
    }
    gabi::store<u32>(gabi::ea(mpHeadModel.get()) + 0xB8, gabi::ea(this));
    mAcchCir.SetWall(30.0f, 0.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    void* cursorBrk = bms1_getRes(0xB);
    void* cursorMdl = bms1_getRes(8);
    mpShopCursor = ShopCursor_create_l(cursorMdl, cursorBrk, l_HIO().mCursorScale[0]);
    if (mpShopCursor.get() == nullptr)
        return FALSE;
    return TRUE;
}
VERIFY(0x02208CDC, &daNpc_Bms1_c::CreateHeap);

/* 02209438 */
void daNpc_Bms1_c::set_mtx() {
    WWHD_FUNC(0x02209438, void, this);
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
        mDoMtx_stack_copy_to_model(mpModel4C8);
        PSMTXCopy(J3DModel_getAnmMtx_l(model, m_head_jnt_num), mDoMtx_stack_c::get());
        mDoMtx_stack_copy_to_model(mpModel4CC);
    } else if (dComIfGs_isEventBit_l(0xA02)) {
        PSMTXCopy(J3DModel_getAnmMtx_l(model, m_head_jnt_num), mDoMtx_stack_c::get());
        mDoMtx_stack_copy_to_model(mpModel4C8);
    }
    if (mpModel4D0.get() != nullptr) {
        PSMTXCopy(J3DModel_getAnmMtx_l(model, m_jnt84C), mDoMtx_stack_c::get());
        mDoMtx_stack_copy_to_model(mpModel4D0);
    }
    if (mpModel4D4.get() != nullptr) {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_copy_to_model(mpModel4D4);
    }
}
VERIFY(0x02209438, &daNpc_Bms1_c::set_mtx);

/* 022098D0 */
BOOL daNpc_Bms1_c::CreateInit() {
    WWHD_FUNC(0x022098D0, BOOL, this);
    m936.x = current.angle.x;
    m936.y = current.angle.y;
    m936.z = current.angle.z;
    gravity = -30.0f;
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0xAD); /* attention_info.distances[SPEAK] */
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0xAD); /* attention_info.distances[TALK] */
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
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101BD2D4) /* l_cyl_src */);
    mA0A = 0;
    mNextMsgNo = 0;
    mBoughtItemNo = 0xFF;
    mCyl.SetStts(&mStts);
    m918 = 0;
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
    /* quaternions = the identity (.data 0x101E9C38) */
    for (u32 o = 0; o < 0x10; o += 4) gabi::store<u32>(gabi::ea(mHairQuatL) + o, gabi::load<u32>(0x101E9C38 + o));
    for (u32 o = 0; o < 0x10; o += 4) gabi::store<u32>(gabi::ea(mHairQuatR) + o, gabi::load<u32>(0x101E9C38 + o));
    mShopItems.mItemSetListGlobalIdx = shopIdx; /* setItemDataIdx */
    gabi::store<s16>(gabi::ea(mShopCamAction.mAction) + 2, 0); /* setCamAction(NULL) */
    gabi::store<u32>(gabi::ea(mShopCamAction.mAction) + 4, 0);
    gabi::store<s16>(gabi::ea(mShopCamAction.mAction), 0);
    mShopCamAction.mCamDataIdx = shopIdx;
    mShopItems.setItemSetDataList();
    mShopItems.createItem(3, current.roomNo);
    mbDemo = 0;
    m948 = current.pos.y;
    dNpc_EventCut_setActorInfo_l(&mEventCut, STR(0x10017EDC) /* "Bms1" */, this);
    mEventCut.mpJntCtrl = &mJntCtrl;
    mpMorf->calc();
    set_mtx();
    return TRUE;
}
VERIFY(0x022098D0, &daNpc_Bms1_c::CreateInit);

/* 02209DB4 */
cPhs_State daNpc_Bms1_c::_create() {
    WWHD_FUNC(0x02209DB4, cPhs_State, this);
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
            gabi::call(0x025E7820, mBtpAnm); /* mDoExt_btpAnm::mDoExt_btpAnm */
            dBgS_ObjAcch_ct(&mAcch, {0x10017D24, 0x10017D44, 0x10017D34});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x10017D14);
            gabi::call(0x0259DAA0, &mJntCtrl);   /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
            gabi::call(0x0259F740, &mEventCut);  /* dNpc_EventCut_c::dNpc_EventCut_c */
            mStickControl.__vtbl = 0x10050788;
            STControl_setWaitParm_l(&mStickControl, 0xF, 0xF, 0, 0, 0.9f, 0.5f, 0, 0x2000);
            STControl_init_l(&mStickControl);
            gabi::call(0x025BBDF0, &mShopCamAction); /* ShopCam_action_c::ShopCam_action_c */
            gabi::call(0x025BC760, &mShopItems);     /* ShopItems_c::ShopItems_c */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    s8 type = (s8)(mParameters >> 24);
    mType = type;
    BOOL evt = dComIfGs_isEventBit_l(0xA02);
    if (type == 0) {
        if (evt && !checkItemGet(0x69, 1))
            goto err;
    } else {
        if (!evt || checkItemGet(0x69, 1))
            goto err;
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
            daNpc_Bms1_HIO_c& hio = l_HIO();
            if (hio.m8 < 0) {
                hio.mNo = mDoHIO_createChild(STR(0x10017EE4), &hio);
            }
            hio.m8 = hio.m8 + 1;
            if (!CreateInit()) {
                return cPhs_ERROR_e;
            }
        }
        return phase;
    }
err:
    mbCreateErr = 1;
    return cPhs_ERROR_e;
}
VERIFY(0x02209DB4, &daNpc_Bms1_c::_create);

/* 0220A7AC */
BOOL daNpc_Bms1_c::_execute() {
    WWHD_FUNC(0x0220A7AC, BOOL, this);
    daNpc_Bms1_HIO_c& hio = l_HIO();
    mJntCtrl.setParam(hio.mMaxBackboneX, hio.mMaxBackboneY, hio.mMinBackboneX, hio.mMinBackboneY, hio.mMaxHeadX,
                      hio.mMaxHeadY, hio.mMinHeadX, hio.mMinHeadY, hio.mMaxTurnStep);
    playTexPatternAnm();
    m93E = (u8)mpMorf->play(&eyePos, 0, 0);
    mpMorf->calc();
    mDoExt_McaMorf* morf = mpMorf;
    if (morf->getFrame() < mPrevFrame) {
        morf = mpMorf;
        m93E = 1;
    }
    mPrevFrame = morf->getFrame();
    if (!demo_move()) {
        checkOrder();
        callAction();
        mShopCamAction.move();
        mShopItems.Item_Move();
        eventOrder();
    }
    mHeadAnm.move();
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    mAcch.CrrPos(dComIfG_Bgsp());
    tevStr.mRoomNo = (s8)gabi::call<s32>(0x024EF130, dComIfG_Bgsp(), gabi::ea(&mAcch) + 0xD4 + 0x14); /* dBgS::GetRoomId */
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, (u8)gabi::call<s32>(0x024EEEB8, dComIfG_Bgsp(), gabi::ea(&mAcch) + 0xD4 + 0x14));
    set_mtx();
    setCollision();
    return TRUE;
}
VERIFY(0x0220A7AC, &daNpc_Bms1_c::_execute);

/* 0220A954 */
BOOL daNpc_Bms1_c::_draw() {
    WWHD_FUNC(0x0220A954, BOOL, this);
    J3DModel* model = mpMorf->getModel();
    u32 headData = J3DModel_modelData_l(mpHeadModel);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, gabi::at<J3DModelData>(headData), mFrame);
    setLightTevColorType(dKy_getEnvlight(), mpHeadModel, &tevStr);
    mpMorf->entryDL();
    mDoExt_modelUpdateDL(mpHeadModel, 0);
    if (mType == 1) {
        setLightTevColorType(dKy_getEnvlight(), mpModel4C8, &tevStr);
        mDoExt_modelUpdateDL(mpModel4C8, 0);
        setLightTevColorType(dKy_getEnvlight(), mpModel4CC, &tevStr);
        mDoExt_modelUpdateDL(mpModel4CC, 0);
    } else if (dComIfGs_isEventBit_l(0xA02)) {
        setLightTevColorType(dKy_getEnvlight(), mpModel4C8, &tevStr);
        mDoExt_modelUpdateDL(mpModel4C8, 0);
    }
    if (mpModel4D0.get() != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), mpModel4D0, &tevStr);
        mDoExt_modelUpdateDL(mpModel4D0, 0);
    }
    if (mpModel4D4.get() != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), mpModel4D4, &tevStr);
        mDoExt_modelUpdateDL(mpModel4D4, 0);
    }
    gabi::store<u32>(headData + 0x38, 0); /* mBtpAnm.remove(headData) */
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
VERIFY(0x0220A954, &daNpc_Bms1_c::_draw);
