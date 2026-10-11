/**
 * d_a_obj_gong.cpp (WWHD)
 * Object - Gong (Tetra's Ship)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_gong.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_gong.h"
using daObjGong::Act_c;
const gc_tu_data gc_tu = { .safeString = 0x10029880, .headerStatics = {0x10469CAC, 0x101C99B4, 0x10469CB8} };
const char Act_c::M_arcname[] = "Vdora";
namespace daObjGong {
struct Attr_c { f32 spec; f32 offsetY; };
static const Attr_c L_attr = {0.75f, 125.0f};
static inline const Attr_c& attr() { return L_attr; }
}
using daObjGong::attr;
u32 Gong_create_heap_result(Act_c*);

/* 0234BEB8 */
// HD: skin deformation creation and attachment were removed.
bool Act_c::create_heap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_VDORA_BDL_VDORA_e);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_VDORA_BCK_05_VDORA_CUT02_HIT_e);
    JUT_ASSERT(0xbd, (modelData != NULL) && (bck != NULL));

    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0,
                                    nullptr, 0x00000000, 0x11020203);
    mDoExt_McaMorf* morf = mpMorf;
    J3DModel* model = morf != nullptr ? morf->getModel() : nullptr;
    return model != nullptr;
}

/* 0234BFC0 */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    // HD: tail branch preserves the full result register.
    return Gong_create_heap_result((Act_c*)i_this);
}

/* 0234C0B0 */
cPhs_State Act_c::_create() {
    fopAcM_ct(this, Act_c);

    cPhs_State ret = dComIfG_resLoad(&mPhs, M_arcname);

    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, solidHeapCB, 0x0)) {
            fopAcM_SetMtx(this, mpMorf->getModel()->getBaseTRMtx());
            init_mtx();
            fopAcM_setCullSizeBox(this, -100.0f, -1.0f, -50.0f, 100.0f, 230.0f, 50.0f);
            eyePos.y += attr().offsetY;
        } else {
            ret = cPhs_ERROR_e;
        }
    }

    return ret;
}

/* 0234C1C4 */
bool Act_c::_delete() {
    dComIfG_resDeleteDemo(&mPhs, M_arcname);
    return true;
}

/* 0234BFC4 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mpMorf->getModel()->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 0234C08C */
void Act_c::init_mtx() {
    mpMorf->getModel()->setBaseScale(scale);
    set_mtx();
}

/* 0234C1F4 */
bool Act_c::demo_move() {
    return dDemo_setDemoData(this, DEMO_ENABLE_TRANS_e | DEMO_ENABLE_ROTATE_e |
        DEMO_ENABLE_ANM_e | DEMO_ENABLE_ANM_FRAME_e, mpMorf, "Vdora", 0, nullptr, 0, 0) != 0;
}

/* 0234C23C */
bool Act_c::_execute() {
    demo_move();
    mpMorf->play(nullptr, 0, 0);
    set_mtx();
    return true;
}

/* 0234C288 */
bool Act_c::_draw() {
    J3DModel* model = mpMorf->getModel();
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(model, &tevStr);
    dDlst_texSpecmapST(&eyePos, &tevStr, model, attr().spec); // HD: passes the model instead of model data.
    mpMorf->updateDL();
    return true;
}

/* method table (HD: tail branches) */
/* 0234C308 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->_create();
}
/* 0234C30C */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->_delete();
}
/* 0234C310 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->_execute();
}
/* 0234C314 */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->_draw();
}
/* 0234C418 */
static BOOL Mthd_IsDelete(void* i_this) {
    return TRUE;
}

/* 0234C318 */
static void __sinit_d_a_obj_gong_cpp() {
    __sinit_header_statics();
}

/* 0234C3AC */
// HD: sead::SafeString deleting destructor (this TU's SafeString vtable)
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0234C3C0 */
// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty)
static void SafeString_assureTerminationImpl(void*) {}

/* 0234C3C4 */
// HD: daObjGong::Act_c deleting destructor (vtable +0xC)
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
str 10029910 # "Vdora"
str 100298F8 in 0234C1F4 # "Vdora"
str 100298A8 # "d_a_obj_gong.cpp"
str 100298BC "(modelData != NULL) && (bck != NULL)" renamed # HD: null printed as 0
ext 0234BEB8 u32 Gong_create_heap_result(Act_c*)
ext 0252CA4C void dDlst_texSpecmapST(cXyz*, dKy_tevstr_c*, J3DModel*, f32)
ext 02527028 BOOL dDemo_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, u16*, u32, s8)
*/
