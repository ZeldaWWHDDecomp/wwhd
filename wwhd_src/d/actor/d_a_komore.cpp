/**
 * d_a_komore.cpp (WWHD)
 * Komore: light shafts through foliage (scrolling-texture model "frLt").
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_komore.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_komore.h"
using daKomore::Act_c;

const gc_tu_data gc_tu = {
    .safeString = 0x100135E8,
    .headerStatics = {0x10464C58, 0x101B87C8, 0},
};

const char daKomore::Act_c::M_arcname[] = "frLt";
// HD: these tail calls pass the full result register without bool conversion.
u32 Komore_create_heap_result(Act_c*);
u32 Komore_delete_result(Act_c*);
u32 Komore_execute_result(Act_c*);
u32 Komore_draw_result(Act_c*);

/* 021A6B90 */
static u32 solidHeapCB(fopAc_ac_c* i_this) {
    /* tail call: r3 is passed through unchanged */
    return Komore_create_heap_result((Act_c*)i_this);
}

/* 021A6A6C */
BOOL Act_c::create_heap() {
    J3DModelData* mdl_data;
    J3DAnmTextureSRTKey* btk_data;

    mdl_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_FRLT_BDL_YFRLT00_e);
    JUT_ASSERT(0x66, mdl_data != NULL);
    if (mdl_data != NULL)
        mpModel = mDoExt_J3DModel__create(mdl_data, 0, 0x11020203);

    btk_data = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_FRLT_BTK_YFRLT00_e);
    JUT_ASSERT(0x6D, btk_data != NULL);

    s32 btkRet = mBtkAnm.init(mdl_data, btk_data, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);
    bool ret = false;
    if (mdl_data != nullptr && mpModel != nullptr && btkRet != 0) {
        ret = true;
    }
    return ret;
}

/* 021A6C88 */
cPhs_State Act_c::_create() {
    fopAcM_ct(this, Act_c);

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)solidHeapCB), 0x0)) {
            set_mtx();
            fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
            fopAcM_setCullSizeBox(this, -100.0f, -1000.0f, -100.0f, 100.0f, 100.0f, 100.0f);
        } else {
            phase_state = cPhs_ERROR_e;
        }
    }
    return phase_state;
}

/* 021A6D84 */
bool Act_c::_delete() {
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}

/* 021A6B94 */
void Act_c::set_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    MTXCopy(mDoMtx_stack_c::get(), &mMtx);
    J3DModel_calc(mpModel);
}

/* 021A6DB4 */
bool Act_c::_execute() {
    mBtkAnm.play();
    return true;
}

/* 021A6DDC */
bool Act_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG2, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mBtkAnm.entry(mpModel->getModelData(), mBtkAnm.getFrame());
    mDoExt_modelUpdateDL(mpModel);
    return true;
}

/* 021A6E4C */
static cPhs_State Mthd_Create(void* i_this) {
    return static_cast<Act_c*>(i_this)->_create();
}

/* 021A6E50 */
static u32 Mthd_Delete(void* i_this) {
    return Komore_delete_result((Act_c*)i_this);
}

/* 021A6E54 */
static u32 Mthd_Execute(void* i_this) {
    return Komore_execute_result((Act_c*)i_this);
}

/* 021A6E58 */
static u32 Mthd_Draw(void* i_this) {
    return Komore_draw_result((Act_c*)i_this);
}

/* 021A6F5C */
static BOOL Mthd_IsDelete(void* i_this) {
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 021A6E5C */
static void __sinit_d_a_komore_cpp() {
    sinit_header_statics(0x10464C58, 0x101B87C8);
}

/* 021A6EF0 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 021A6F04 */
static void SafeString_assureTerminationImpl(void*) {
}

/* 021A6F08 */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 10013658 # "frLt"
str 10013630 # "d_a_komore.cpp"
str 10013610 "mdl_data != NULL" renamed # HD: "mdl_data != 0"
str 10013620 "btk_data != NULL" renamed # HD: "btk_data != 0"
ext 021A6A6C u32 Komore_create_heap_result(Act_c*)
ext 021A6D84 u32 Komore_delete_result(Act_c*)
ext 021A6DB4 u32 Komore_execute_result(Act_c*)
ext 021A6DDC u32 Komore_draw_result(Act_c*)
*/
