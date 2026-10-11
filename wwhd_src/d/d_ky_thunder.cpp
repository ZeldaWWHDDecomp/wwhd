/**
 * d_ky_thunder.cpp
 * Environment - Thunder flash
 * WWHD cking.rpx, GameCube names and structure, verified by tools/verify/d_ky_thunder.
 */
#include "d/d_ky_thunder.h"
#include "gc_api/JSystem/JKernel/JKRSolidHeap.h"
#include "gc_api/JSystem/JUTAssert.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_kankyo_rain.h"
#include "gc_api/dolphin/os.h"
#include "gc_api/f_op/f_op_kankyo_mng.h"
#include "gc_api/f_pc/f_pc_base.h"
#include "gc_api/m_Do/m_Do_audio.h"
#include "gc_api/m_Do/m_Do_ext.h"
#include "gc_api/m_Do/m_Do_mtx.h"

const gc_tu_data gc_tu = {.safeString = 0x10050414};
enum {
    JA_SE_OBJ_THUNDER_FAR = 0x69F6,
    dRes_INDEX_ALWAYS_BDL_YTHDR00_e = 0x3E,
    dRes_INDEX_ALWAYS_BTK_YTHDR00_e = 0x60,
    dRes_INDEX_ALWAYS_BRK_YTHDR00_e = 0x52
};

/* 02585098 */
u32 dThunder_c::draw() {
    static cXyz l_offsetPos(0.0f, 40.0f, -250.0f);
    (void)l_offsetPos;
    // HD: the offset used at creation remains camera-relative when drawing.
    dThunder_camera_c *pCamera = (dThunder_camera_c *)dComIfGp_getCamera(0);
    cXyz pos = pCamera->mEye + mOffsetPos;
    mDoMtx_stack_c::transS(pos);
    mDoMtx_stack_c::ZrotM((s16)gabi::ftoi(mRot));
    mDoMtx_stack_c::XrotM((s16)gabi::ftoi(mRot));
    Mtx34 m;
    MTXCopy(mDoMtx_stack_c::get(), &m);
    J3DModel *model = mModelInfo.mpModel;
    f32 sz = mScale.z, sx = mScale.x, sy = mScale.y;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    model->mBaseScale.z = sz;
    model = mModelInfo.mpModel;
    model->setBaseTRMtx(&m);
    mModelInfo.mBtk.entry(mModelInfo.mpModel->getModelData(), mBtkTime);
    mModelInfo.mBrk.entry(mModelInfo.mpModel->getModelData(), mModelInfo.mBrk.getFrame());
    dComIfGd_setList();
    mDoExt_modelUpdateDL(mModelInfo.mpModel);
    mDoExt_invisibleModel_entryMaskOff(mModelInfo.mInvisModel);
    // HD: removal is inlined as clearing the two animation pointers on model data.
    J3DModelData *data = mModelInfo.mpModel->getModelData();
    mDoExt_btkAnm_remove(data);
    data = mModelInfo.mpModel->getModelData();
    mDoExt_brkAnm_remove(data);
    return TRUE;
}

/* 02585278 */
u32 dThunder_c::execute() {
    // HD: the play-object accessor is retained although its result is unused.
    dComIfGp_ea();
    mModelInfo.mBrk.setPlaySpeed(1.0f);
    if (mModelInfo.mBrk.play()) {
        mDoAud_seStart(JA_SE_OBJ_THUNDER_FAR, &mPos);
        fopKyM_Delete(this);
    }
    return TRUE;
}

/* 025852D8 */
static u32 dThunder_Delete(dThunder_c *i_this) {
    mDoAud_seDeleteObject(&i_this->mPos);
    mDoAud_seDeleteObject(&i_this->mPosNeg);
    mDoExt_destroySolidHeap(i_this->solid_heap);
    // HD: GHS destroys the invisibility wrapper and process base explicitly.
    mDoExt_invisibleModel_dt(i_this->mModelInfo.mInvisModel);
    __dt(&i_this->base);
    return TRUE;
}

/* 02585334 */
u32 dThunder_c::createHeap() {
    if (solid_heap != NULL)
        return TRUE;
    // HD: try the selected game heap, then game heaps zero and one, with a larger allocation.
    JKRHeap *parent = mDoExt_getCurrentGameHeap();
    JKRSolidHeap *heap = NULL;
    if (parent != NULL) {
        heap = mDoExt_createSolidHeapToCurrent(0x3804, parent, 0x20);
        solid_heap = heap;
    }
    if (heap == NULL) {
        parent = mDoExt_getGameHeap(0);
        heap = mDoExt_createSolidHeapToCurrent(0x3804, parent, 0x20);
        solid_heap = heap;
        if (heap == NULL) {
            parent = mDoExt_getGameHeap(1);
            heap = mDoExt_createSolidHeapToCurrent(0x3804, parent, 0x20);
            solid_heap = heap;
            if (heap == NULL)
                return FALSE;
        }
    }
    // HD: source spelling is ASCII; glue preserves the original Japanese guest bytes.
    JKRSolidHeap_setDebugName(heap, "Thunder");
    return TRUE;
}

/* 02585400 */
void dThunder_c::adjustHeap() {
    mDoExt_restoreCurrentHeap();
    if (mDoExt_adjustSolidHeap(solid_heap) >= 0) {
        JKRSolidHeap *heap = solid_heap;
        JKRSolidHeap_vtable *vt = JKRSolidHeap_getVirtualTable(heap);
        u32 size = JKRSolidHeap_getHeapSize(heap, vt);
        void *start = JKRSolidHeap_getStartAddr(heap, vt);
        DCStoreRangeNoSync(start, size);
    }
}

/* 02585478 */
static u32 dThunder_Create(dThunder_c *i_this) {
    if (!i_this->createHeap())
        return cPhs_ERROR_e;
    dThunder_env_c *envLight = (dThunder_env_c *)dKy_getEnvlight();
    dThunder_camera_c *pCamera = (dThunder_camera_c *)dComIfGp_getCamera(0);
    // HD: GHS expands placement construction and each animation constructor here.
    if (i_this != NULL) {
        __ct(&i_this->base);
        i_this->base.__vtbl = dThunder_c::__vt;
        mDoExt_invisibleModel_ct(i_this->mModelInfo.mInvisModel);
        mDoExt_btkAnm::ct(&i_this->mModelInfo.mBtk);
        mDoExt_brkAnm::ct(&i_this->mModelInfo.mBrk);
    }
    J3DModelData *modelData =
        (J3DModelData *)dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BDL_YTHDR00_e);
    JUT_ASSERT(0x8B, modelData != NULL);
    J3DModel *model = mDoExt_J3DModel__create(modelData, 0x80000, 0x01000200);
    i_this->mModelInfo.mpModel = model;
    if (model == NULL || !mDoExt_invisibleModel_create(i_this->mModelInfo.mInvisModel, model)) {
        i_this->adjustHeap();
        return cPhs_ERROR_e;
    }
    void *anm = dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BTK_YTHDR00_e);
    JUT_ASSERT(0x9A, anm != NULL);
    if (!i_this->mModelInfo.mBtk.init(modelData, (J3DAnmTextureSRTKey *)anm, false,
                                      J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
        i_this->adjustHeap();
        return cPhs_ERROR_e;
    }
    void *canm = dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BRK_YTHDR00_e);
    JUT_ASSERT(0xA9, canm != NULL);
    if (!i_this->mModelInfo.mBrk.init(modelData, (J3DAnmTevRegKey *)canm, true,
                                      J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        i_this->adjustHeap();
        return cPhs_ERROR_e;
    }
    i_this->mBtkTime = cM_rndF(1.0f);
    bool close = envLight->mThunderEff.mState < 10;
    f32 factor = close ? 1.0f : 0.5f;
    // HD: rotation is randomized from zero and both scale components use the same size.
    i_this->mRot = 0.0f;
    f32 r = cM_rndFX(0.0f);
    i_this->mRot = gabi::fmuls_ppc(r, factor);
    r = cM_rndF(60.0f);
    f32 size = gabi::fmuls_ppc(gabi::fadds_ppc(20.0f, r), factor);
    i_this->mScale.x = size;
    r = cM_rndFX(1.0f);
    // HD: retain GHS's unordered-NaN behavior for the distant branch.
    if (close ? (r >= 0.5) : !(r < 0.5))
        i_this->mScale.x = -i_this->mScale.x;
    i_this->mScale.y = size;
    i_this->mScale.z = 1.0f;
    cXyz fwd;
    dKyr_get_vectle_calc(&pCamera->mEye, &pCamera->mCenter, &fwd);
    f32 dx = fwd.x, dz = fwd.z;
    f32 distXZ = std_sqrtf(gabi::fmadds(dx, dx, gabi::fmuls_ppc(dz, dz)));
    s16 rotX = cM_atan2s(fwd.x, fwd.z);
    s16 rotY = cM_atan2s(fwd.y, distXZ);
    bool right = !(cM_rndFX(1.0f) < 0.0f);
    u16 yaw = (u16)((s32)rotX + (right ? 0x4000 : -0x4000));
    f32 cs = cM_scos(rotY), sn = cM_ssin(yaw), cy = cM_scos(yaw);
    f32 sideX = gabi::fmuls_ppc(cs, sn), sideZ = gabi::fmuls_ppc(cs, cy);
    f32 baseXZ = cM_rndF(120000.0f);
    f32 offX = gabi::fmadds(fwd.x, 100000.0f, gabi::fmuls_ppc(sideX, baseXZ));
    i_this->mOffsetPos.x = offX;
    f32 offY = cM_rndFX(2000.0f);
    f32 offZ = gabi::fmadds(fwd.z, 100000.0f, gabi::fmuls_ppc(sideZ, baseXZ));
    f32 currentX = i_this->mOffsetPos.x;
    i_this->mOffsetPos.y = offY;
    i_this->mOffsetPos.z = offZ;
    i_this->mPos.x = gabi::fadds_ppc(pCamera->mEye.x, currentX);
    i_this->mPos.y = gabi::fadds_ppc(pCamera->mEye.y, offY);
    i_this->mPos.z = gabi::fadds_ppc(pCamera->mEye.z, offZ);
    r = cM_rndF(1.0f);
    // HD: the left-hand branch also preserves GHS's unordered comparison.
    if (right ? (r < 0.3f) : !(r >= 0.3f)) {
        f32 x = i_this->mPos.x, y = i_this->mPos.y, z = i_this->mPos.z;
        i_this->mPosNeg.x = -x;
        i_this->mPosNeg.y = -y;
        i_this->mPosNeg.z = -z;
        mDoAud_seStart(JA_SE_OBJ_THUNDER_FAR, &i_this->mPosNeg);
    }
    i_this->adjustHeap();
    return cPhs_COMPLEATE_e;
}

/* 02585AA0 */
static u32 dThunder_IsDelete(dThunder_c *i_this) {
    return TRUE;
}

/* ---------------- HD: compiler-generated and template code ---------------- */

/* 02585A0C */
static void __sinit_d_ky_thunder_cpp() {
    sinit_header_statics(0x104775BC, 0x101E9A84);
}

/* 02585AA8 */
static void SafeString__dt(void *i_this, u32 flags) {
    if (i_this != NULL && (flags & 1))
        operator_delete(i_this);
}

/* 02585ABC */
static void SafeString__assureTerminationImpl(void *i_this) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
ext 025E37D8 void mDoExt_restoreCurrentHeap()
str 1005040C
str 10050448
str 1005045C "modelData != NULL"
str 10050470 "anm != NULL"
str 1005043C "canm != NULL"
str 1005048C "Thunder" renamed
data l_offsetPos 101FDD04 guard 101FDD00
*/
