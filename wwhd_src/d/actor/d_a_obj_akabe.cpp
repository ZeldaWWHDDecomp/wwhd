/**
 * d_a_obj_akabe.cpp (WWHD) - collision-only walls and box.
 * Ported from zeldaret/tww with WWHD layout and verified semantics.
 * "HD:" marks differences from the GameCube source.
 */
#include "d/actor/d_a_obj_akabe.h"
using daObjAkabe::Act_c;
const gc_tu_data gc_tu = { .safeString = 0x10024A60,
    .headerStatics = {0x104690A0, 0x101C7B4C, 0x104690AC} };
static const char* M_arcname[4] = {"Akabe", "AkabeD", "AkabeK", "NBOX"};
static const AkabeCullAttr l_cull_attr = {-51.0f, -1.0f, 51.0f, 101.0f};
static const f32 l_cull_depth = 1.0f;

namespace daObj {
/* 02316A94 */
// HD: preserve the PPC shift behavior for widths and shifts outside 0..31.
u32 PrmAbstract(const fopAc_ac_c* actor, u32 width, u32 shift) {
    u32 parameters = actor->mParameters;
    width &= 63;
    shift &= 63;
    u32 mask = (width < 32 ? 1u << width : 0u) - 1u;
    return (shift < 32 ? parameters >> shift : 0u) & mask;
}
}
using daObj::PrmAbstract;

/* 0231645C */
bool Act_c::chk_appear() {
    if (prm_get_always())
        return true;
    s32 swSaveNo = prm_get_swSave();
    if (swSaveNo == 0xFF)
        return dComIfGs_checkCollect(0) == 0;
    return !fopAcM_isSwitch(this, swSaveNo);
}

/* 0231650C */
void Act_c::init_scale() {
    s32 scl = prm_get_scl();
    if (scl == 1) {
        f32 x = scale.x, y = scale.y;
        x *= 10.0f;
        // HD: the Z store precedes X and Y.
        scale.z = 1.0f;
        y *= 10.0f;
        scale.x = x;
        scale.y = y;
    } else if (scl == 3) {
        scale *= 10.0f;
    } else if (scl != 2) {
        scale.z = 1.0f;
    }
}

/* 023165BC */
void Act_c::init_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
}

/* 02316634 */
u8 Act_c::create_heap() {
    static const s16 dzb[4] = {3, 3, 3, 3};
    bool ret = false;
    mpBgW = (AkabeBgW*)new_dBgW();
    if (mpBgW != NULL) {
        cBgD_t* bgw_data = (cBgD_t*)dComIfG_getObjectRes(M_arcname[mType], dzb[mType]);
        JUT_ASSERT(0x82, bgw_data != NULL);
        if (!mpBgW->Set(bgw_data, dBgW::MOVE_BG_e, &mMtx))
            ret = true;
    }
    if (!ret)
        mpBgW = NULL;
    return ret;
}

/* 02316718 */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    // HD: tail branch preserves the full result register.
    return Akabe_create_heap_result((Act_c*)i_this);
}

/* 0231671C */
cPhs_State Act_c::_create() {
    s32 arg0 = prm_get_arg0();
    mType = (arg0 == 1 || arg0 == 2 || arg0 == 3) ? arg0 : 0;
    fopAcM_ct(this, Act_c);
    // HD: retain the full register, then store its low byte without normalizing it.
    u32 appear = Akabe_chk_appear_result(this);
    mbAppear = u8(appear);
    cPhs_State rt = cPhs_ERROR_e;
    if (appear) {
        rt = dComIfG_resLoad(&mPhs, M_arcname[mType]);
        if (rt == cPhs_COMPLEATE_e) {
            init_scale();
            init_mtx();
            static const u32 heap_size[4] = {0x200, 0x200, 0x200, 0x3E0};
            if (fopAcM_entrySolidHeap(this, solidHeapCB, heap_size[mType])) {
                dComIfG_Bgsp()->Regist(mpBgW, this);
                AkabeBgW* bgw = mpBgW;
                // HD: load cull extents between collision field stores, retaining alias behavior.
                f32 lowX = l_cull_attr.lowX;
                bgw->SetCrrFunc(0);
                bgw = mpBgW;
                f32 highZ = l_cull_attr.highZ;
                bgw->SetPriority(1);
                f32 highY = l_cull_attr.highY;
                s32 kind = mType;
                f32 lowY = l_cull_attr.lowY;
                fopAcM_SetMtx(this, &mMtx);
                if (kind == 3)
                    fopAcM_setCullSizeBox(this, lowX, lowY, lowX, highZ, highY, highZ);
                else
                    fopAcM_setCullSizeBox(this, lowX, lowY, lowY, highZ, highY, l_cull_depth);
            } else {
                rt = cPhs_ERROR_e;
            }
        }
    }
    return rt;
}

/* 023168BC */
bool Act_c::_delete() {
    if (mbAppear) {
        if (mpBgW != NULL && mpBgW->ChkUsed())
            dComIfG_Bgsp()->Release(mpBgW);
        dComIfG_resDelete(&mPhs, M_arcname[mType]);
    }
    return true;
}

/* 02316938 */
bool Act_c::_execute() {
    // HD: test the full result register of chk_appear.
    if (!Akabe_chk_appear_result(this))
        fopAcM_delete(this);
    return true;
}

/* 02316978 */
static cPhs_State Mthd_Create(void* i_this) { return ((Act_c*)i_this)->_create(); }
/* 0231697C */
static bool Mthd_Delete(void* i_this) { return ((Act_c*)i_this)->_delete(); }
/* 02316980 */
static bool Mthd_Execute(void* i_this) { return ((Act_c*)i_this)->_execute(); }
/* 02316984 */
// HD: _draw is folded into the method-table entry.
static BOOL Mthd_Draw(void*) { return TRUE; }
/* 0231698C */
static void __sinit_d_a_obj_akabe_cpp() { __sinit_header_statics(); }
/* 02316A20 */
// HD: compiler-emitted deleting destructor of a header static.
static void header_static_dt(void* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
/* 02316A34 */
// HD: SafeString::assureTerminationImpl_ companion.
static void assureTerminationImpl_() {}
/* 02316A38 */
// HD: actor deleting destructor.
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}
/* 02316A8C */
static BOOL Mthd_IsDelete(void*) { return TRUE; }

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
str 10024A94 # "d_a_obj_akabe.cpp"
str 10024AA8 "bgw_data != NULL" renamed # HD: assertion prints null as 0
data M_arcname 10024AE0
data l_cull_attr 10024AB8
data l_cull_depth 10024A38
data dzb 10024A8C
data heap_size 10024AC8
ext 02316634 u32 Akabe_create_heap_result(Act_c*)
ext 0231645C u32 Akabe_chk_appear_result(Act_c*)
*/
