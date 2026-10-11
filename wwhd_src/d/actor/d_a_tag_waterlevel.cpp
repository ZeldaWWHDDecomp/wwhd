/**
 * d_a_tag_waterlevel.cpp (WWHD)
 * Tag - Water level (Forbidden Woods / Earth Temple water level by schedule bit).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_waterlevel.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_tag_waterlevel.h"
const gc_tu_data gc_tu = { .headerStatics={0x1046E698, 0x101D21F8, 0x1046E6A4} };
namespace daTagWaterlevel {
// HD: file-scope aliases for the GameCube static class members (autoglue data mapping).
static u32 M_state;
static f32 M_now;
namespace {
struct Attr_c { f32 field_0x00, field_0x04, field_0x08, field_0x0c; };
static const Attr_c L_attr = {0.005f, 0.05f, 0.015f, 0.005f};
inline const Attr_c& attr() { return L_attr; }
}

/* 024B3DC4 */
// HD: daObj::PrmAbstract<daTagWaterlevel::Act_c::Prm_e> (per-TU copy)
static u32 PrmAbstract(Act_c* actor, u32 width, u32 shift) {
    u32 param = fopAcM_GetParam(actor);
    width &= 63;
    shift &= 63;
    u32 mask = (width < 32 ? 1u << width : 0u) - 1u;
    return (shift < 32 ? param >> shift : 0u) & mask;
}

u8 Act_c::prm_get_sch() { return PrmAbstract(this, PRM_SCH_W, PRM_SCH_S); }

/* 024B398C */
cPhs_State Act_c::_create() {
    fopAcM_ct(this, Act_c);
    u8 sch = prm_get_sch();
    if (sch & dKy_get_schbit() & 0xFF) {
        M_now = 1.0f;
    } else {
        M_now = 0.0f;
    }
    field_0x290 = 0.0f;
    mAction = 0;
    return cPhs_COMPLEATE_e;
}

/* 024B3A58 */
void Act_c::bgm_proc() {
    if (M_state & STATE_1) {
        if (M_now > 0.95f && mAction != 1) {
            mAction = 1;
            mDoAud_stWaterLevelUp();
        }
    } else {
        if (M_now < 0.05f && mAction != 2) {
            mAction = 2;
            mDoAud_stWaterLevelDown();
        }
    }
}

/* 024B3AC0 */
// HD: the step clamp is computed before the single store of field_0x290
bool Act_c::_execute() {
    f32 target;
    u8 prm_sch = prm_get_sch();
    u8 sch_bit = dKy_get_schbit();
    M_state = M_state & ~STATE_2;

    if (prm_sch & sch_bit & 0xFF) {
        target = 1.0f;
        if (!(M_state & STATE_1)) {
            M_state = M_state | STATE_2 | STATE_1;
        }
    } else {
        target = 0.0f;
        if (M_state & STATE_1) {
            M_state = (M_state | STATE_2) & ~STATE_1;
        }
    }

    if (dComIfGp_event_runCheck() && dComIfGp_event_chkEventFlag(2)) {
        if (M_now < 0.5f) {
            M_now = 0.0f;
        } else {
            M_now = 1.0f;
        }
    } else {
        if (std::fabs(target - M_now) < attr().field_0x0c) {
            M_now = target;
            field_0x290 = 0.0f;
        } else {
            f32 step = field_0x290 + attr().field_0x00;
            if (step > attr().field_0x08)
                step = attr().field_0x08;
            else if (step < attr().field_0x0c)
                step = attr().field_0x0c;
            field_0x290 = step;
            cLib_addCalc(&M_now, target, attr().field_0x04, step, attr().field_0x0c);
        }
    }

    bgm_proc();
    return true;
}

/* 024B3CBC */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->_create();
}

/* 024B3CC0 */
// HD: _delete inlined
static BOOL Mthd_Delete(void* i_this) {
    return TRUE;
}

/* 024B3CC8 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->_execute();
}

/* 024B3CCC */
// HD: _draw inlined
static BOOL Mthd_Draw(void* i_this) {
    return TRUE;
}

/* 024B3DBC */
static BOOL Mthd_IsDelete(void* i_this) {
    return TRUE;
}

/* 024B3CD4 */
static void __sinit_d_a_tag_waterlevel_cpp() {
    __sinit_header_statics();
}

/* 024B3D68 */
// HD: Act_c deleting destructor
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

} // namespace daTagWaterlevel
using namespace daTagWaterlevel;

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
data M_state 101D5F2C
data M_now 101D5F28
ext 025602A8 u32 dKy_get_schbit()
ext 0200ECD4 f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32)
ext 025E1970 void mDoAud_stWaterLevelUp()
ext 025E197C void mDoAud_stWaterLevelDown()
*/
