/** Fan actor, verified against the original WWHD functions.
 * GameCube names and layout views; native call/store order retained. */
#include "d/actor/d_a_fan.h"
const gc_tu_data gc_tu = {.safeString = 0x1000E904};
static FanArcTable m_arcname;
static FanShortTable m_bdlidx;
static FanShortTable m_dzbidx;
static FanShortTable m_fan_speed;
static FanFloatTable m_wind_length;
static FanWordTable m_heapsize;
static FanVectorTable m_cull_min;
static FanVectorTable m_cull_max;
static FanFloatTable m_wind_r;
static FanVectorTable m_wind_model_scale;
static dCcD_SrcCps l_cps_src;
static BOOL nodeCallBack(J3DNode *, int);
/* 02132BDC */
BOOL daFan_c::Delete() {
    dComIfG_resDelete(&mPhs, m_arcname.entries[u8(mType)].get());
    dComIfG_resDelete(&mWindPhs, "Yaflw00");
    if (mWindSePId != (u32)-1 /* fpcM_ERROR_PROCESS_ID_e */) {
        void *se = fopKyM_SearchByID(mWindSePId);
        if (se != nullptr)
            fopKyM_Delete(se);
    }
    return TRUE;
}

/* 02132310 */
BOOL daFan_c::CreateHeap() {
    J3DModelData *modelData =
        (J3DModelData *)dComIfG_getObjectRes(m_arcname.entries[u8(mType)].get(), m_bdlidx.entries[u8(mType)]);
    if (modelData == nullptr) /* JUT_ASSERT(0x15e, modelData != NULL) */
        JUT_ASSERT_fail("d_a_fan.cpp", 0x15E, "modelData != NULL");

    mModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (!mModel)
        return FALSE;
    mModel->setUserArea((uintptr_t)this); /* setUserArea */

    modelData = (J3DModelData *)dComIfG_getObjectRes("Yaflw00", dRes_INDEX_YAFLW00_BDL_YAFLW00_e);
    if (modelData == nullptr) /* JUT_ASSERT(0x17f, modelData != NULL) */
        JUT_ASSERT_fail("d_a_fan.cpp", 0x17F, "modelData != NULL");
    mWindModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000222);
    if (!mWindModel)
        return FALSE;

    J3DAnmTextureSRTKey *pbtk =
        (J3DAnmTextureSRTKey *)dComIfG_getObjectRes("Yaflw00", dRes_INDEX_YAFLW00_BTK_YAFLW00_01_e);
    if (pbtk == nullptr) /* JUT_ASSERT(400, pbtk != NULL) */
        JUT_ASSERT_fail("d_a_fan.cpp", 400, "pbtk != NULL");
    if (!mWindBtkAnm0.init(modelData, pbtk, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0))
        return FALSE;

    pbtk = (J3DAnmTextureSRTKey *)dComIfG_getObjectRes("Yaflw00", dRes_INDEX_YAFLW00_BTK_YAFLW00_02_e);
    if (pbtk == nullptr) /* JUT_ASSERT(0x19c, pbtk != NULL) */
        JUT_ASSERT_fail("d_a_fan.cpp", 0x19C, "pbtk != NULL");
    if (!mWindBtkAnm1.init(modelData, pbtk, TRUE, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0))
        return FALSE;

    J3DAnmTransform *pbck =
        (J3DAnmTransform *)dComIfG_getObjectRes("Yaflw00", dRes_INDEX_YAFLW00_BCK_YAFLW00_e);
    if (pbck == nullptr) /* JUT_ASSERT(0x1a9, pbck != NULL) */
        JUT_ASSERT_fail("d_a_fan.cpp", 0x1A9, "pbck != NULL");
    if (!mWindBckAnm.init(modelData, pbck, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false))
        return FALSE;

    return TRUE;
}

/* 02132C4C */
BOOL daFan_c::Create() {
    f32 wind_len = m_wind_length.entries[u8(mType)];
    cullMtx = gabi::ea(mModel->getBaseTRMtx()); /* fopAcM_SetMtx */
    cXyz *cullMin = &m_cull_min.entries[u8(mType)];
    cXyz *cullMax = &m_cull_max.entries[u8(mType)];
    fopAcM_setCullSizeBox(this, cullMin->x, cullMin->y, cullMin->z, cullMax->x, cullMax->y, cullMax->z);
    mStts.Init(0xFF, 0xFF, this);
    Fan_setCps(&mCps, &l_cps_src);
    mCps.SetStts(&mStts); /* mCps.SetStts(&mStts) */
    cXyz *ws = &m_wind_model_scale.entries[u8(mType)];
    mWindScale.x = ws->x;
    mWindScale.y = ws->y;
    mWindScale.z = ws->z;
    set_cps(wind_len);
    Fan_setCpsShape(cps_shape(), &mCpsS);
    /* mCps.CalcAtVec(): at vector = end - start */
    Fan_subtract(&cps_shape()->mEnd, &cps_shape()->mStart, &mCps.mGObjAt.mVec);
    set_mtx();
    mSwitchNo = daFan_prm::getSwitchNo(this);

    /* HD: the joint loop became JUTNameTab::getIndex("puro") */
    auto jointName = mModel->getModelData()->getJointName();
    s32 idx = jointName->getIndex("puro");
    if (idx >= 0) {
        mModel->getModelData()->getJointNodePointer((u16)idx)->setCallBack(nodeCallBack);
    }

    mModel->calc();
    mWindSePId = fopKyM_create(fpcNm_LEVEL_SE_e, JA_SE_OBJ_WIND_TAG, &eyePos, nullptr, 0);
    return TRUE;
}

/* 02131FF8 */
static BOOL nodeCallBack(J3DNode *node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        J3DJoint *joint = J3DNode_toJoint(node);
        auto model = j3dSys.getModel();
        daFan_c *i_this = reinterpret_cast<daFan_c *>(model->getUserArea());
        if (i_this != nullptr) {
            s32 jntNo = reinterpret_cast<FanJointView *>(joint)->mJntNo;
            i_this->mFanAngle = (s16)(i_this->mFanAngle + i_this->mFanSpeed);
            PSMTXCopy(model->getAnmMtx(jntNo), mDoMtx_stack_c::get());
            mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->mFanAngle);
            mtx_copy(model->getAnmMtx(jntNo), mDoMtx_stack_c::get()); /* setAnmMtx */
            PSMTXCopy(mDoMtx_stack_c::get(), J3DSys::mCurrentMtx);
        }
    }
    return TRUE;
}

/* 0213212C */
cPhs_State daFan_c::_create() {
    /* fopAcM_ct(this, daFan_c): base constructor, vtable, inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this)) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = FAN_ACTOR_VTABLE;
            dCcD_Stts_ct(&mStts);
            auto cps = reinterpret_cast<FanCpsStorage *>(&mCps);
            Fan_collisionCtor(&mCps);
            cps->__vtblShape = FAN_SHAPE_BASE_VTABLE;
            cps->__vtblAabb = FAN_AABB_VTABLE;
            Fan_capsuleCtor(&cps->mCps);
            cps->__vtbl_hitinf = FAN_CPS_OBJ_VTABLE;
            cps->__vtblShape = FAN_CPS_SHAPE_VTABLE;
            cps->mCps.__vtbl = FAN_CPS_GEOMETRY_VTABLE;
            mDoExt_btkAnm::ct(&mBtkAnm);
            mDoExt_btkAnm::ct(&mWindBtkAnm0);
            mDoExt_btkAnm::ct(&mWindBtkAnm1);
            auto bck = reinterpret_cast<FanBckStorage *>(&mWindBckAnm);
            mWindBckAnm.mFrameCtrl.init(0);
            bck->__vtbl = FAN_TRANSFORM_BASE_VTABLE;
            Fan_transformCtor(bck->mTransform);
            bck->mpAnmTransform = 0;
            bck->mpCallback1 = 0;
            bck->__vtbl = FAN_TRANSFORM_VTABLE;
            bck->mpQuat = 0;
            bck->__vtblMtxCalc = FAN_MTX_CALC_VTABLE;
            bck->mpTransformInfo = 0;
            bck->mpCallback2 = 0;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    mType = daFan_prm::getType(this);
    cPhs_State rt1 = dComIfG_resLoad(&mPhs, m_arcname.entries[u8(mType)].get());
    if (rt1 != cPhs_COMPLEATE_e)
        return rt1;

    cPhs_State rt2 = dComIfG_resLoad(&mWindPhs, "Yaflw00");
    if (rt2 != cPhs_COMPLEATE_e)
        return rt2;

    /* HD: MoveBGCreate's result is returned directly */
    return MoveBGCreate(m_arcname.entries[u8(mType)].get(), m_dzbidx.entries[u8(mType)], FAN_MOVE_BG_ROT_Y,
                        m_heapsize.entries[u8(mType)]);
}

/* 021326B0 */
void daFan_c::set_mtx() {
    mModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    mModel->setBaseTRMtx(mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &mBgMtx);

    mWindModel->setBaseScale(mWindScale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), (s16)(current.angle.x + 0x4000), current.angle.y, current.angle.z);
    mWindModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 02132698 */
void daFan_c::set_wind_length(f32 h) {
    f32 len = m_wind_length.entries[u8(mType)];
    len *= h;
    set_cps(len);
}

/* 0213258C */
void daFan_c::set_cps(f32 h) {
    mCpsS.mRadius = m_wind_r.entries[u8(mType)];
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), (s16)(current.angle.x + 0x4000), current.angle.y, current.angle.z);
    mDoMtx_stack_push();
    mDoMtx_stack_c::transM(0.0f, mCpsS.mRadius, 0.0f);
    PSMTXMultVec(mDoMtx_stack_c::get(), cXyz_Zero, &mCpsS.mStart);
    mDoMtx_stack_pop();
    mDoMtx_stack_c::transM(0.0f, mCpsS.mRadius + h, 0.0f);
    PSMTXMultVec(mDoMtx_stack_c::get(), cXyz_Zero, &mCpsS.mEnd);
}

/* 0213284C */
BOOL daFan_c::Execute(Mtx34 **mtxP) {
    s16 speed = m_fan_speed.entries[u8(mType)];
    f32 len = (f32)(s16)mFanSpeed / (f32)speed;

    if ((mType == 0 && !fopAcM_isSwitch(this, mSwitchNo)) ||
        (mType == 1 && fopAcM_isSwitch(this, mSwitchNo))) {
        cLib_chaseS(&mFanSpeed, speed, 100);
        mWindBtkAnm1.mFrameCtrl.setRate(1.0f); /* setPlaySpeed */
    } else if (mType == 2) {
        len = 1.0f;
        cLib_chaseS(&mFanSpeed, speed, 100);
        mWindBtkAnm1.mFrameCtrl.setRate(1.0f);
    } else {
        cLib_chaseS(&mFanSpeed, 0, 100);
        mWindBtkAnm1.mFrameCtrl.setRate(-1.0f);
    }

    if (mWindSePId != (u32)-1) {
        auto se = fopKyM_SearchByID(mWindSePId);
        if (se != nullptr) {
            // HD: preserve GHS's unsigned volume conversion, including infinities and NaNs.
            f32 v = len * 100.0f;
            u32 vol;
            if (v >= 2147483648.0f)
                vol = (u32)gabi::ftoi(v - 2147483648.0f) + 0x80000000u;
            else
                vol = (u32)gabi::ftoi(v);
            s32 reverb = dComIfGp_getReverb(current.roomNo);
            se->field_0xf8 = vol;
            se->mReverb = (s8)reverb;
            se->mFlag = u8(se->mFlag) | 4;
        }
    }

    if (len > 0.1f) {
        set_wind_length(len * len);
        Fan_setCpsShape(cps_shape(), &mCpsS);
        cCcS_Set(dComIfG_Ccsp(), &mCps);
        mWindBtkAnm0.play();
        mWindBtkAnm1.play();
        mWindBckAnm.play();
    }

    set_mtx();
    // HD: the output matrix pointer occupies one guest word.
    *reinterpret_cast<gptr<Mtx34> *>(mtxP) = &mBgMtx;
    return TRUE;
}

/* 02132AF4 */
BOOL daFan_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mModel, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mWindModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel);
    dComIfGd_setList();
    mWindBckAnm.entry(mWindModel->getModelData(), mWindBckAnm.getFrame());
    Fan_entryBtkModel(&mWindBtkAnm0, mWindModel, mWindBtkAnm0.getFrame());
    Fan_entryBtkModel(&mWindBtkAnm1, mWindModel, mWindBtkAnm1.getFrame());
    mDoExt_modelUpdateDL(mWindModel);
    return TRUE;
}

/* 021322F4 */
static cPhs_State daFan_Create(void *i_this) { return ((daFan_c *)i_this)->_create(); }

/* 021322F8 */
static BOOL daFan_Delete(void *i_this) { return ((daFan_c *)i_this)->MoveBGDelete(); }

/* 021322FC */
static BOOL daFan_Draw(void *i_this) { return ((daFan_c *)i_this)->Draw_v(); }

/* 0213230C */
static BOOL daFan_Execute(void *i_this) { return ((daFan_c *)i_this)->MoveBGExecute(); }

/* 02132E9C */
static BOOL daFan_IsDelete(void *i_this) { return TRUE; }

/* 02132EB8 */
static BOOL dBgS_MoveBgActor_IsDelete(void *i_this) { return TRUE; }

/* 02132E08 */
static void __sinit_d_a_fan_cpp() { sinit_header_statics(0x10463CF8, 0x101B4B70); }

/* 02132EA4 */
static void deleting_dtor_empty(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02132EC0 */
static void daFan_dtor(daFan_c *p, s32 flags) {
    if (p != nullptr) {
        Fan_transformDtor(&reinterpret_cast<FanBckStorage *>(&p->mWindBckAnm)->__vtbl,
                          0);           /* mWindBckAnm's anm member destructor */
        Fan_collisionDtor(&p->mCps, 2); /* dCcD_GObjInf::~dCcD_GObjInf */
        dCcD_Stts_dt(&p->mStts, 2);
        Fan_actorDtor(p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}

/* 02132F38 */
static void fan_empty_virtual(void *p) {}

/* WWHD glue (tools/verify/autoglue.py): translation unit data
data m_arcname 101B4BC4
data m_bdlidx 1000E9C8
data m_dzbidx 1000E9D0
data m_fan_speed 1000E9D8
data m_wind_length 1000E9E0
data m_heapsize 1000E9EC
data m_cull_min 1000E9F8
data m_cull_max 1000EA1C
data m_wind_r 1000EA40
data m_wind_model_scale 1000EA4C
data l_cps_src 101B4B24
str 1000E9C0 "Yaflw00"
str 1000E958 "d_a_fan.cpp"
str 1000E97C "modelData != NULL"
str 1000E964 "pbtk != NULL"
str 1000E970 "pbck != NULL"
str 1000E9B0 "puro"
ext 025DAC50 FanLevelSe* fopKyM_SearchByID(u32)
ext 025DAD48 BOOL fopKyM_Delete(void*)
ext 025DADA4 u32 fopKyM_create(s16,s32,cXyz*,cXyz*,u32)
ext 0200F564 bool cLib_chaseS(be<s16>*,s16,s16)
ext 025164C0 void Fan_setCps(dCcD_Cps*,const dCcD_SrcCps*)
ext 020181FC void Fan_setCpsShape(FanCapsuleGeometry*,const FanCapsuleShape*)
ext 028E8DAC void Fan_subtract(const cXyz*,const cXyz*,cXyz*)
ext 025E8048 void Fan_entryBtkModel(mDoExt_btkAnm*,J3DModel*,f32)
ext 02515FB8 void Fan_collisionCtor(dCcD_Cps*)
ext 02018150 void Fan_capsuleCtor(FanCapsuleGeometry*)
ext 027DA984 void Fan_transformCtor(void*)
ext 027F3628 void Fan_transformDtor(void*,s32)
ext 02515980 void Fan_collisionDtor(dCcD_Cps*,s32)
ext 025D50BC void Fan_actorDtor(fopAc_ac_c*,s32)
*/
