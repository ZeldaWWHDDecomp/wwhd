/**
 * d_a_swc00.cpp (WWHD)
 * Switch trigger: sets a switch while the player stands inside a cylinder.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_swc00.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_swc00.h"

// HD: when nonzero, this global byte suppresses cylinder entry; its wider role is unknown.
static u8 l_triggerSuppressed;

/* 024A0F20 */
static BOOL daSwc00_Execute(swc00_class* i_this) {
    fopAc_ac_c* actor = i_this;
    int enable_sw = daSwc00_getSw2No(i_this);
    if (enable_sw == 0xFF || dComIfGs_isSwitch(enable_sw, fopAcM_GetRoomNo(actor))) {
        int swBit = daSwc00_getSw1No(i_this);

        f32 xz_dist2 = fopAcM_searchPlayerDistanceXZ2(actor);
        f32 y_diff = fopAcM_searchPlayerDistanceY(actor);
        if (l_triggerSuppressed == 0 && xz_dist2 < actor->scale.x && (-100.0f < y_diff && y_diff < actor->scale.y)) {
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

/* 024A122C */
static BOOL daSwc00_IsDelete(swc00_class* i_this) {
    return TRUE;
}

/* 024A1064 */
static BOOL daSwc00_Delete(swc00_class* i_this) {
    return TRUE;
}

/* 024A106C */
static cPhs_State daSwc00_Create(fopAc_ac_c* i_this) {
    fopAcM_ct((swc00_class*)i_this, swc00_class);

    swc00_class* a_this = (swc00_class*)i_this;

    u8 swBit = daSwc00_getSw1No(a_this);
    if (dComIfGs_isSwitch(swBit, fopAcM_GetRoomNo(i_this))) {
        if (daSwc00_getType(a_this) == 0) {
            dComIfGs_offSwitch(daSwc00_getSw1No(a_this), fopAcM_GetRoomNo(i_this));
        } else {
            return cPhs_ERROR_e;
        }
    }

    // HD: GHS fuses the radius calculation and stores the height first.
    f32 sy = i_this->scale.y * 100.0f;
    f32 sx = fmadds(i_this->scale.x, 100.0f, 30.0f);
    i_this->scale.y = sy;
    i_this->scale.x = sx * sx;

    return cPhs_COMPLEATE_e;
}

/* inlined into daSwc00_Create (fopAcM_ct) */
swc00_class::swc00_class() {
    // HD: explicit base construction and virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 024A1234 */
// HD: deleting destructor.
static void swc00_class_dt(swc00_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024A1198 */
static void __sinit_d_a_swc00_cpp() {
    sinit_header_statics(0x1046E148, 0x101D1280);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data l_triggerSuppressed 101D5F45
*/
