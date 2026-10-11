/**
 * d_a_tag_kb_item.cpp (WWHD)
 * Tag - item dug up by a Kargaroc / enemy (switch and item-bit guard).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_kb_item.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_tag_kb_item.h"

/* fopAcM_offSwitch(a, sw): dSv_info_c::offSwitch(sw, home.roomNo) */
static inline void fopAcM_offSwitch(fopAc_ac_c* a, s32 sw) { dComIfGs_offSwitch(sw, a->home.roomNo); }

/* 024ABAA0 */
bool daTagKbItem_c::_delete() {
    if (mItemNo != dItemNo_NONE_e && mSwBitNo != 0xFF) {
        fopAcM_offSwitch(this, mSwBitNo);
    }
    return true;
}

/* 024AB984 */
void daTagKbItem_c::CreateInit() {
    u32 prm = fopAcM_GetParam(this);
    mItemNo = prm & 0xFF;
    mItemBitNo = (s8)((prm >> 8) & 0xFF);
    mEnemyKind = (prm >> 16) & 0xFF;
    mSwBitNo = (prm >> 24) & 0xFF;
    field_0x298 = false;
    field_0x299 = 0;
    mpActor = nullptr;
}

/* 024AB9BC */
cPhs_State daTagKbItem_c::_create() {
    fopAcM_ct(this, daTagKbItem_c);

    CreateInit();
    if ((mItemBitNo != 0x1f && fopAcM_isItem(this, mItemBitNo)) ||
        (mSwBitNo != 0xff && fopAcM_isSwitch(this, mSwBitNo)))
    {
        return cPhs_ERROR_e;
    }
    return cPhs_COMPLEATE_e;
}

/* 024ABA9C */
static cPhs_State daTagKbItem_Create(void* i_this) {
    return static_cast<daTagKbItem_c*>(i_this)->_create();
}

/* 024ABAF0 */
static BOOL daTagKbItem_Delete(void* i_this) {
    return static_cast<daTagKbItem_c*>(i_this)->_delete();
}

/* 024ABAF4 */
static BOOL daTagKbItem_Draw(void* i_this) {
    return TRUE;
}

/* 024ABAFC */
// HD: execution is empty, matching the GameCube USA version.
static BOOL daTagKbItem_Execute(void* i_this) {
    return TRUE;
}

/* 024ABB98 */
static BOOL daTagKbItem_IsDelete(void* i_this) {
    return TRUE;
}

/* inlined into _create (fopAcM_ct) */
daTagKbItem_c::daTagKbItem_c() {
    // HD: explicit base construction and virtual destructor table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 024ABB04 */
static void __sinit_d_a_tag_kb_item_cpp() {
    sinit_header_statics(0x1046E36C, 0x101D1B84);
}

/* 024ABBA0 */
// HD: deleting destructor.
static void daTagKbItem_c_dt(daTagKbItem_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
