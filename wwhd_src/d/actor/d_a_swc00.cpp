/**
 * d_a_swc00.cpp (WWHD)
 * Switch trigger: sets a switch while the player stands inside a cylinder.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_swc00.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

struct swc00_class : fopAc_ac_c {};  /* no members of its own */

static inline int daSwc00_getSw1No(swc00_class* i_this) { return fopAcM_GetParam(i_this) & 0xFF; }
static inline int daSwc00_getSw2No(swc00_class* i_this) { return (fopAcM_GetParam(i_this) >> 8) & 0xFF; }
static inline int daSwc00_getType(swc00_class* i_this) { return (fopAcM_GetParam(i_this) >> 0x10) & 0x3; }

/* HD: a global byte tested before the trigger cylinder (when set, the switch acts as if the
 * player were outside). Not in the GameCube code; role unknown. */
static inline u8 l_HD_101D5F45() { return gabi::load<u8>(0x101D5F45); }

#define SWC00_VTBL 0x1003EAC4 /* swc00_class vtable (HD virtual destructor) */

/* 024A0F20 */
static BOOL daSwc00_Execute(swc00_class* i_this) {
    WWHD_FUNC(0x024A0F20, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    int enable_sw = daSwc00_getSw2No(i_this);
    if (enable_sw == 0xFF || dComIfGs_isSwitch(enable_sw, fopAcM_GetRoomNo(actor))) {
        int swBit = daSwc00_getSw1No(i_this);

        f32 xz_dist2 = fopAcM_searchPlayerDistanceXZ2(actor);
        f32 y_diff = fopAcM_searchPlayerDistanceY(actor);
        if (l_HD_101D5F45() == 0 && xz_dist2 < actor->scale.x && (-100.0f < y_diff && y_diff < actor->scale.y)) {
            dComIfGs_onSwitch(swBit, fopAcM_GetRoomNo(actor));

            if (daSwc00_getType(i_this) != 0) {
                fopAcM_delete(i_this);
            }
        } else if (daSwc00_getType(i_this) == 0) {
            dComIfGs_offSwitch(swBit, fopAcM_GetRoomNo(actor));
        }
    }

    return TRUE;
}
VERIFY(0x024A0F20, daSwc00_Execute);

/* 0x024A122C */
static BOOL daSwc00_IsDelete(swc00_class* i_this) {
    WWHD_FUNC(0x024A122C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024A122C, daSwc00_IsDelete);

/* 024A1064 */
static BOOL daSwc00_Delete(swc00_class* i_this) {
    WWHD_FUNC(0x024A1064, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024A1064, daSwc00_Delete);

/* 024A106C */
static cPhs_State daSwc00_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024A106C, cPhs_State, i_this);
    /* fopAcM_ct(i_this, swc00_class): HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = SWC00_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    swc00_class* a_this = (swc00_class*)i_this;

    u8 swBit = daSwc00_getSw1No(a_this);
    if (dComIfGs_isSwitch(swBit, fopAcM_GetRoomNo(i_this))) {
        if (daSwc00_getType(a_this) == 0) {
            dComIfGs_offSwitch(daSwc00_getSw1No(a_this), fopAcM_GetRoomNo(i_this));
        } else {
            return cPhs_ERROR_e;
        }
    }

    /* scale.x = SQUARE(scale.x * 100 + 30) (contracted to fmadds), scale.y *= 100 */
    f32 sy = i_this->scale.y * 100.0f;
    f32 sx = gabi::fmadds(i_this->scale.x, 100.0f, 30.0f);
    i_this->scale.y = sy;
    i_this->scale.x = sx * sx;

    return cPhs_COMPLEATE_e;
}
VERIFY(0x024A106C, daSwc00_Create);

/* 024A1234: swc00_class deleting destructor (compiler-generated, HD virtual destructor) */
static void swc00_class_dt(swc00_class* i_this, s32 flags) {
    WWHD_FUNC(0x024A1234, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024A1234, swc00_class_dt);

/* 024A1198: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_swc00_cpp() {
    WWHD_FUNC(0x024A1198, void, (u32)0);
    sinit_header_statics(0x1046E148, 0x101D1280);
}
VERIFY(0x024A1198, __sinit_d_a_swc00_cpp);
