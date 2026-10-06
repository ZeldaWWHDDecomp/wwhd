/**
 * d_a_tag_waterlevel.cpp (WWHD)
 * Tag - Water level (Forbidden Woods / Earth Temple water level by schedule bit).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_waterlevel.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define WATERLEVEL_VTBL 0x1003FD28

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025602A8 dKy_get_schbit() */
static inline u32 dKy_get_schbit() { return gabi::call<u32>(0x025602A8); }
/* 0200ECD4 cLib_addCalc(value, target, scale, maxStep, minStep) */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* 025E1970 / 025E197C mDoAud_stWaterLevelUp / Down (HD: static) */
static inline void mDoAud_stWaterLevelUp() { gabi::call(0x025E1970); }
static inline void mDoAud_stWaterLevelDown() { gabi::call(0x025E197C); }
/* dComIfGp_event_runCheck(): play+0x5292; dComIfGp_event_chkEventFlag(flag): play+0x52B8 */
static inline BOOL dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
static inline u16 dComIfGp_event_chkEventFlag(u16 flag) { return gabi::load<u16>(dComIfGp_ea() + 0x52B8) & flag; }

namespace daTagWaterlevel {

enum State_e { STATE_1 = 1, STATE_2 = 2, STATE_4 = 4 };

/* static members (.data) */
static be<u32>& M_state() { return *gabi::at<be<u32>>(0x101D5F2C); }
static be<f32>& M_now() { return *gabi::at<be<f32>>(0x101D5F28); }

struct Act_c : fopAc_ac_c {
    enum Prm_e { PRM_SCH_W = 8, PRM_SCH_S = 0 };
    u8 prm_get_sch();
    cPhs_State _create();
    void bgm_proc();
    bool _execute();

    /* 0x3AC */ be<f32> field_0x290;
    /* 0x3B0 */ be<s32> mAction;
};
WWHD_OFFSET(Act_c, field_0x290, 0x3AC);
WWHD_SIZE(Act_c, 0x3B4);

/* 024B3DC4: daObj::PrmAbstract<daTagWaterlevel::Act_c::Prm_e> (per-TU copy) */
static u32 PrmAbstract(Act_c* actor, u32 width, u32 shift) {
    WWHD_FUNC(0x024B3DC4, u32, actor, width, shift);
    u32 param = fopAcM_GetParam(actor);
    width &= 63;
    shift &= 63;
    u32 mask = (width < 32 ? 1u << width : 0u) - 1u;
    return (shift < 32 ? param >> shift : 0u) & mask;
}
VERIFY(0x024B3DC4, PrmAbstract);

u8 Act_c::prm_get_sch() { return PrmAbstract(this, PRM_SCH_W, PRM_SCH_S); }

/* 024B398C */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x024B398C, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = WATERLEVEL_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    u8 sch = prm_get_sch();
    if (sch & dKy_get_schbit() & 0xFF) {
        M_now() = 1.0f;
    } else {
        M_now() = 0.0f;
    }
    field_0x290 = 0.0f;
    mAction = 0;
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024B398C, &Act_c::_create);

/* 024B3A58 */
void Act_c::bgm_proc() {
    WWHD_FUNC(0x024B3A58, void, this);
    if (M_state() & STATE_1) {
        if (M_now() > 0.95f && mAction != 1) {
            mAction = 1;
            mDoAud_stWaterLevelUp();
        }
    } else {
        if (M_now() < 0.05f && mAction != 2) {
            mAction = 2;
            mDoAud_stWaterLevelDown();
        }
    }
}
VERIFY(0x024B3A58, &Act_c::bgm_proc);

/* 024B3AC0: HD: the step clamp is computed before the single store of field_0x290 */
bool Act_c::_execute() {
    WWHD_FUNC(0x024B3AC0, bool, this);
    f32 target;
    u8 prm_sch = prm_get_sch();
    u8 sch_bit = dKy_get_schbit();
    M_state() = M_state() & ~STATE_2;

    if (prm_sch & sch_bit & 0xFF) {
        target = 1.0f;
        if (!(M_state() & STATE_1)) {
            M_state() = M_state() | STATE_2 | STATE_1;
        }
    } else {
        target = 0.0f;
        if (M_state() & STATE_1) {
            M_state() = (M_state() | STATE_2) & ~STATE_1;
        }
    }

    if (dComIfGp_event_runCheck() && dComIfGp_event_chkEventFlag(2)) {
        if (M_now() < 0.5f) {
            M_now() = 0.0f;
        } else {
            M_now() = 1.0f;
        }
    } else {
        if (std::fabs(target - M_now()) < 0.005f) {
            M_now() = target;
            field_0x290 = 0.0f;
        } else {
            f32 step = field_0x290 + 0.005f;
            if (step > 0.015f)
                step = 0.015f;
            else if (step < 0.005f)
                step = 0.005f;
            field_0x290 = step;
            cLib_addCalc(&M_now(), target, 0.05f, step, 0.005f);
        }
    }

    bgm_proc();
    return true;
}
VERIFY(0x024B3AC0, &Act_c::_execute);

/* 024B3CBC */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x024B3CBC, cPhs_State, i_this);
    return ((Act_c*)i_this)->_create();
}
VERIFY(0x024B3CBC, Mthd_Create);

/* 024B3CC0: _delete inlined */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x024B3CC0, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024B3CC0, Mthd_Delete);

/* 024B3CC8 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x024B3CC8, BOOL, i_this);
    return ((Act_c*)i_this)->_execute();
}
VERIFY(0x024B3CC8, Mthd_Execute);

/* 024B3CCC: _draw inlined */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x024B3CCC, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024B3CCC, Mthd_Draw);

/* 024B3DBC */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x024B3DBC, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024B3DBC, Mthd_IsDelete);

/* 024B3CD4 */
static void __sinit_d_a_tag_waterlevel_cpp() {
    WWHD_FUNC(0x024B3CD4, void, (u32)0);
    sinit_header_statics(0x1046E698, 0x101D21F8);
}
VERIFY(0x024B3CD4, __sinit_d_a_tag_waterlevel_cpp);

/* 024B3D68: Act_c deleting destructor */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x024B3D68, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024B3D68, Act_c_dt);

} // namespace daTagWaterlevel
