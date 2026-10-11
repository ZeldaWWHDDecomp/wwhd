/**
 * d_a_tag_evsw.cpp (WWHD)
 * Tag - mirrors an event bit into a switch.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_evsw.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_tag_evsw.h"

const gc_tu_data gc_tu = { .headerStatics = {0x1046E29C, 0x101D194C, 0x1046E2A8} };

using daTagEvsw::Act_c;

static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift);

int Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }
u16 Act_c::prm_get_eventbitID() { return PrmAbstract(this, PRM_EVENTBITID_W, PRM_EVENTBITID_S); }
u16 Act_c::prm_get_Type() { return PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S); }

/* 024A8258 */
cPhs_State Act_c::_create() {
    fopAcM_ct(this, Act_c);

    if (dComIfGs_isEventBit(prm_get_eventbitID())) {
        dComIfGs_onSwitch(prm_get_swSave(), home.roomNo); /* fopAcM_onSwitch */
    } else {
        switch (prm_get_Type()) {
        case 1:
            break;
        default:
            dComIfGs_offSwitch(prm_get_swSave(), home.roomNo); /* fopAcM_offSwitch */
            break;
        }
    }
    return cPhs_COMPLEATE_e;
}

/* 024A83D0 */
bool Act_c::_execute() {
    if (dComIfGs_isEventBit(prm_get_eventbitID())) {
        dComIfGs_onSwitch(prm_get_swSave(), home.roomNo);
    } else {
        switch (prm_get_Type()) {
        case 1:
            break;
        default:
            dComIfGs_offSwitch(prm_get_swSave(), home.roomNo);
            break;
        }
    }
    return true;
}

// HD: method table entries use tail branches and inline trivial bodies.
/* 024A849C */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->_create();
}
/* 024A84A0 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->_delete();
}
/* 024A84A8 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->_execute();
}
/* 024A84AC */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->_draw();
}
/* 024A85B4 */
static BOOL Mthd_IsDelete(void* i_this) {
    return TRUE;
}

// HD: compiler-generated code of this translation unit.
/* 024A84B4 */
// HD: static initialisation of the translation unit (header statics only)
static void __sinit_d_a_tag_evsw_cpp() {
    __sinit_header_statics();
}

/* 024A8548 */
// HD: Act_c deleting destructor (compiler-generated, HD virtual destructor)
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024A85BC */
// HD: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU)
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}

/* WWHD glue (tools/verify/autoglue.py): no additional translation-unit data */
