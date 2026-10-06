/**
 * d_a_tag_evsw.cpp (WWHD)
 * Tag - mirrors an event bit into a switch.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_evsw.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ACT_VTBL 0x1003F0C0     /* HD: daTagEvsw::Act_c vtable */
#define TU_AAB_VTBL 0x1003F0B0  /* this TU's cM3dGAab vtable copy */

/* ---- local bindings (SHARED-CANDIDATE: same as d_a_npc_kf1.h) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }

namespace daTagEvsw {
struct Act_c : fopAc_ac_c {
    enum Prm_e {
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 0,
        PRM_EVENTBITID_W = 16,
        PRM_EVENTBITID_S = 8,
        PRM_TYPE_W = 2,
        PRM_TYPE_S = 24,
    };
    int prm_get_swSave();
    u16 prm_get_eventbitID();
    u16 prm_get_Type();

    cPhs_State _create();
    bool _delete() { return true; }
    bool _execute();
    bool _draw() { return true; }

    /* 0x3AC */ u8 m290[0xC];  /* GameCube 0x290 */
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
};
WWHD_OFFSET(Act_c, mStts, 0x3B8);
WWHD_OFFSET(Act_c, mCyl, 0x3F4);
WWHD_SIZE(Act_c, 0x524);
}  // namespace daTagEvsw
using daTagEvsw::Act_c;

/* 024A85BC: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x024A85BC, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x024A85BC, PrmAbstract);
int Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }
u16 Act_c::prm_get_eventbitID() { return PrmAbstract(this, PRM_EVENTBITID_W, PRM_EVENTBITID_S); }
u16 Act_c::prm_get_Type() { return PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S); }

/* 024A8258 */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x024A8258, cPhs_State, this);
    /* fopAcM_ct(this, Act_c): HD: vtable, inline dCcD_Stts / dCcD_Cyl constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, TU_AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

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
VERIFY(0x024A8258, &Act_c::_create);

/* 024A83D0 */
bool Act_c::_execute() {
    WWHD_FUNC(0x024A83D0, bool, this);
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
VERIFY(0x024A83D0, &Act_c::_execute);

/* method table entries (HD: tail branches / inlined bodies) */
/* 024A849C */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x024A849C, cPhs_State, i_this);
    return ((Act_c*)i_this)->_create();
}
VERIFY(0x024A849C, Mthd_Create);
/* 024A84A0 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x024A84A0, BOOL, i_this);
    return ((Act_c*)i_this)->_delete();
}
VERIFY(0x024A84A0, Mthd_Delete);
/* 024A84A8 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x024A84A8, BOOL, i_this);
    return ((Act_c*)i_this)->_execute();
}
VERIFY(0x024A84A8, Mthd_Execute);
/* 024A84AC */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x024A84AC, BOOL, i_this);
    return ((Act_c*)i_this)->_draw();
}
VERIFY(0x024A84AC, Mthd_Draw);
/* 024A85B4 */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x024A85B4, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024A85B4, Mthd_IsDelete);

/* 024A84B4: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_tag_evsw_cpp() {
    WWHD_FUNC(0x024A84B4, void, (u32)0);
    sinit_header_statics(0x1046E29C, 0x101D194C);
}
VERIFY(0x024A84B4, __sinit_d_a_tag_evsw_cpp);

/* 024A8548: Act_c deleting destructor (compiler-generated, HD virtual destructor) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x024A8548, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024A8548, Act_c_dt);
