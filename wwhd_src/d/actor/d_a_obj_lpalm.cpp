/**
 * d_a_obj_lpalm.cpp
 * Object - Palm tree
 *
 * WWHD (cking.rpx), written in the style of the GameCube decompilation (zeldaret/tww) and verified
 * function by function against the recompiled original (tools/verify, unit d_a_obj_lpalm). Each
 * function is marked with its WWHD address; the glue block at the end lists this translation
 * unit's data (tools/verify/autoglue.py).
 */

#include "d/actor/d_a_obj_lpalm.h"

/* HD: this translation unit's copies of inline-constructed vtables (GHS emits one per TU) */
const gc_tu_data gc_tu = {
    .safeString = 0x1002C12C,
    .headerStatics = {0x1046A4D0, 0x101CA9F4, 0x1046A4DC},
};

const char daObjLpalm_c::M_arcname[] = "Oyashi";

/* 0236C594 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daObjLpalm_c*)i_this)->CreateHeap();
}

/* 0236C290 */
static BOOL nodeCallBack(J3DNode* joint, int calcTiming) {
    J3DModel* model = j3dSys.getModel();
    s32 jntNo = ((J3DJoint*)joint)->getJntNo();
    daObjLpalm_c* i_this = (daObjLpalm_c*)model->getUserArea();
    if (calcTiming == J3DNodeCBCalcTiming_In && (jntNo == OYASHI_JNT_J_HAPPA1_e || jntNo == OYASHI_JNT_J_HAPPA2_e)) {
        mDoMtx_stack_c::copy(model->getAnmMtx(jntNo));
        mDoMtx_stack_c::ZrotM(-0x4000);
        mDoMtx_stack_c::quatM(&i_this->mBaseQuat);
        if (jntNo == OYASHI_JNT_J_HAPPA1_e)
            mDoMtx_stack_c::quatM(&i_this->mAnmMtxQuat[0]);
        else
            mDoMtx_stack_c::quatM(&i_this->mAnmMtxQuat[1]);
        mDoMtx_stack_c::ZrotM(0x4000);
        model->setAnmMtx(jntNo, mDoMtx_stack_c::get());
    }

    return TRUE;
}

/* 0236C44C */
BOOL daObjLpalm_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_OYASHI_BDL_OYASHI_e);
    JUT_ASSERT(269, modelData != NULL); // HD
    for (u16 i = 0; i < modelData->getJointNum(); i++)
        modelData->getJointNodePointer(i)->setCallBack(nodeCallBack);
    if (modelData == NULL)
        return false;

    mModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
    if (mModel == NULL)
        return false;

    mModel->setUserArea((uintptr_t)this);
    // HD: the collision model follows its own matrix (set up in CreateInit)
    mpBgW = dBgW_NewSet((cBgD_t*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_OYASHI_DZB_OYASHI_e), dBgW::MOVE_BG_e, &mBgMtx);
    if (mpBgW == NULL)
        return false;

    return true;
}

/* 0236C598 */
void daObjLpalm_c::CreateInit() {
    Quaternion q(0.0f, 0.0f, 0.0f, 1.0f);

    mBaseQuat = mBaseQuatTarget = q;
    mAnmMtxQuat[0] = mAnmMtxQuat[1] = q;
    mAnimDir[0] = 0;
    mAnimDir[1] = 0;
    mAnimWave[0] = 0;
    mAnimWave[1] = ftoi(cM_rndFX(0x8000));
    fopAcM_SetMtx(this, mModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -350.0f, -50.0f, -350.0f, 350.0f, 1300.0f, 350.0f);
    fopAcM_setCullSizeFar(this, 2.37f);
    dComIfG_Bgsp()->Regist(mpBgW, this);
    static f32 bgScaleXZ(0.7f); // HD: the collision is 70% of the tree's width
    mModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mModel->setBaseTRMtx(mDoMtx_stack_c::get());
    mDoMtx_stack_c::scaleM(bgScaleXZ, 1.0f, bgScaleXZ);
    MTXCopy(mDoMtx_stack_c::get(), &mBgMtx);
}

/* inlined into daObjLpalmCreate (fopAcM_ct) */
daObjLpalm_c::daObjLpalm_c() {
    // HD: GHS constructs the base class here and stores the class's vtable
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* inlined into daObjLpalmCreate */
cPhs_State daObjLpalm_c::_create() {
    fopAcM_ct(this, daObjLpalm_c);
    cPhs_State ret = dComIfG_resLoad(&mPhs, M_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, CheckCreateHeap, 0xF00) == 0) {
            ret = cPhs_ERROR_e;
        } else {
            CreateInit();
        }
    }

    return ret;
}

/* inlined into daObjLpalmDelete */
bool daObjLpalm_c::_delete() {
    if (heap != NULL && mpBgW->ChkUsed()) {
        dComIfG_Bgsp()->Release(mpBgW);
    }

    dComIfG_resDeleteDemo(&mPhs, M_arcname);
    return true;
}

/* 0236C7C0 */
static cPhs_State daObjLpalmCreate(void* i_this) {
    return ((daObjLpalm_c*)i_this)->_create();
}

/* 0236C884 */
static BOOL daObjLpalmDelete(void* i_this) {
    return ((daObjLpalm_c*)i_this)->_delete();
}

/* 0236C8EC */
static BOOL daObjLpalmExecute(void* i_this) {
    return ((daObjLpalm_c*)i_this)->_execute();
}

/* inlined into daObjLpalmExecute */
bool daObjLpalm_c::_execute() {
    Quaternion q;
    cXyz up(0.0f, 1.0f, 0.0f);
    cXyz windDir;

    cMtx_YrotS(calc_mtx(), -current.angle.y);
    MtxPosition(dKyw_get_wind_vec(), &windDir);
    f32 windPow = dKyw_get_wind_pow();
    s16 angle = ftoi(windPow * 0x600);
    cXyz dir = up.outprod(windDir);
    f64 mag = dir.abs(); // HD: compared in double precision
    if (mag < 8e-09) {
        mBaseQuatTarget.x = 0.0f;
        mBaseQuatTarget.y = 0.0f;
        mBaseQuatTarget.z = 0.0f;
        mBaseQuatTarget.w = 1.0f;
    } else {
        f32 sin = cM_ssin(angle);
        cXyz norm = dir.normZP(); // `dir = dir.normZP()`: GHS goes through a temporary
        dir = norm;
        mBaseQuatTarget.x = sin * dir.x;
        mBaseQuatTarget.y = sin * dir.y;
        mBaseQuatTarget.z = sin * dir.z;
        mBaseQuatTarget.w = cM_scos(angle);
    }

    mDoMtx_quatSlerp(&mBaseQuat, &mBaseQuatTarget, &q, 0.25f);
    mBaseQuat = q;

    for (s32 i = 0; i < 2; i++) {
        s16 target = ftoi(windPow * 0x180);
        if (target > 0x100)
            target = 0x100;
        cLib_addCalcAngleS2(&mAnimDir[i], target, 4, 0x20);
        s32 add = ftoi((windPow * 0x800) + cM_rndFX(128.0f));
        mAnimWave[i] += add;
        f32 w = cM_ssin(mAnimDir[i]);
        mAnmMtxQuat[i].x = w * cM_ssin(mAnimWave[i]);
        mAnmMtxQuat[i].y = 0.0f;
        mAnmMtxQuat[i].z = w * cM_ssin(mAnimWave[i]);
        mAnmMtxQuat[i].w = cM_scos(mAnimDir[i]);
    }

    mpBgW->Move();
    return false;
}

/* inlined into daObjLpalmDraw */
bool daObjLpalm_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mModel, &tevStr);
    dComIfGd_setListBG(); // HD: drawn in the BG lists
    mDoExt_modelUpdateDL(mModel);
    dComIfGd_setList();
    return true;
}

/* 0236CC18 */
static BOOL daObjLpalmDraw(void* i_this) {
    return ((daObjLpalm_c*)i_this)->_draw();
}

/* 0236CD58 */
static BOOL daObjLpalmIsDelete(void*) {
    return TRUE;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0236CCB0 */
// __sinit_d_a_obj_lpalm_cpp: the common header statics
static void __sinit_d_a_obj_lpalm_cpp() {
    __sinit_header_statics();
}

/* 0236CD44 */
// sead::SafeStringBase<char>::~SafeStringBase (deleting; this TU's copy)
static void SafeString_dtor(SafeString* i_this, int flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 0236CD60 */
// the compiler-generated deleting destructor: only the fopAc_ac_c base
static void daObjLpalm_c_dtor(daObjLpalm_c* i_this, int flags) {
    if (i_this != NULL) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 0236CDB4 */
// sead::SafeStringBase<char>::assureTerminationImpl_ (empty; this TU's copy)
static void SafeString_assureTerminationImpl(SafeString* i_this) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str  1002C1DC                               # "Oyashi"
str  1002C154                               # "d_a_obj_lpalm.cpp"
str  1002C168 "modelData != NULL"
data bgScaleXZ 101FDC0C guard 101FDC08
*/
