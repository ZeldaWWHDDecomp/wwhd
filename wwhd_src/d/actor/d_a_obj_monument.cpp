/**
 * d_a_obj_monument.cpp (WWHD)
 * Object - Monument ("Esekh" stone tablet with its own background collision)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_monument.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_monument.h"
using daObjMonument::Act_c;

const gc_tu_data gc_tu = { .safeString = 0x1002CEF0, .headerStatics = {0x1046A664, 0x101CB144, 0x1046A670} };
const char Act_c::M_arcname[] = "Esekh";
namespace daObjMonument {
struct Attr_c {
    be<s16> mModelId;
    be<s16> mBgWId;
};
}
// ESEKH and ESEKH2 model/collision resource pairs.
static daObjMonument::Attr_c L_attr[2] = {{4, 8}, {5, 9}};
static inline const daObjMonument::Attr_c& attr(s32 type) { return L_attr[type]; }

static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift);
s32 Act_c::prm_get_type() { return (s32)PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S); }
u32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 02375E18 */
// HD: tail branch preserves the full result register.
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    return ((Act_c*)i_this)->create_heap();
}

/* 02375C64 */
bool Act_c::create_heap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, attr(mType).mModelId);
    JUT_ASSERT(0x81, modelData != NULL);
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x00, 0x11020203);
    mpModel = model;
    if (model == nullptr)
        return false;

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    PSMTXCopy(mDoMtx_stack_c::get(), &mtx);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());

    dBgW* bgw = new_dBgW();
    mpBgW = bgw;
    if (bgw != nullptr) {
        cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(M_arcname, attr(mType).mBgWId);
        if (mpBgW->Set(dzb, cBgW_MOVE_BG_e, &mtx))
            return false;
    }
    return true;
}

/* 02375F00 */
cPhs_State Act_c::_create() {
    fopAcM_ct(this, Act_c);

    cPhs_State ret = dComIfG_resLoad(&mPhs, M_arcname);

    mpBgW = nullptr;
    if (ret == cPhs_COMPLEATE_e) {
        mType = prm_get_type();

        if (fopAcM_entrySolidHeap(this, solidHeapCB, 0xD20)) {
            fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
            dBgS* bgs = dComIfG_Bgsp();
            bgs->Regist(mpBgW, this);
            init_mtx();
        } else {
            ret = cPhs_ERROR_e;
        }
    }
    return ret;
}

/* 02375FF4 */
bool Act_c::_delete() {
    if (mpBgW != nullptr && mpBgW->ChkUsed()) {
        dBgS* bgs = dComIfG_Bgsp();
        bgs->Release(mpBgW);
    }
    dComIfG_resDeleteDemo(&mPhs, M_arcname);
    return true;
}

/* 02375E1C */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 02375EE0 */
// HD: (matcher: daObj::PrmAbstract)
void Act_c::init_mtx() {
    mpModel->setBaseScale(scale);
    set_mtx();
}

/* 02376058 */
bool Act_c::_execute() {
    set_mtx();
    return true;
}

/* 0237607C */
// HD:  HD: tev type 1 (TEV_TYPE_BG0 in the bindings' numbering; GameCube source: TEV_TYPE_BG1)
bool Act_c::_draw() {
    u32 swSave = prm_get_swSave();
    if (dComIfGs_isSwitch(swSave, home.roomNo))
        return true;

    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return true;
}

/* method table entries (HD: tail branches) */
/* 02376144 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->_create();
}
/* 02376148 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->_delete();
}
/* 0237614C */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->_execute();
}
/* 02376150 */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->_draw();
}
/* 02376254 */
static BOOL Mthd_IsDelete(void*) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.
/* 02376154 */
static void __sinit_d_a_obj_monument_cpp() {
    __sinit_header_statics();
}

/* 023761E8 */
// HD:  this TU's sead::SafeString deleting destructor (trivial; SafeString vtable +8)
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 023761FC */
// HD:  this TU's empty sead::SafeString virtual (SafeString vtable +0x10, assureTerminationImpl_)
static void SafeString_empty(void*) {
}

/* 02376200 */
// HD:  Act_c deleting destructor (HD: virtual; ~fopAc_ac_c)
static void Act_c_dt(Act_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}

/* 0237625C */
// HD:  daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU)
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
data L_attr 101CB168
str 1002CF48 # "Esekh"
str 1002CF18 # "d_a_obj_monument.cpp"
str 1002CF30 "modelData != NULL" renamed # HD: modelData != 0
*/
