/**
 * d_a_tag_kb_item.cpp (WWHD)
 * Tag - item dug up by a Kargaroc / enemy (switch and item-bit guard).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_kb_item.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define TAGKBITEM_VTBL 0x1003F54C /* HD: daTagKbItem_c vtable */

struct daTagKbItem_c : fopAc_ac_c {
    bool _delete();
    void CreateInit();
    cPhs_State _create();

    /* 0x3AC */ request_of_phase_process_class mPhase; /* unused */
    /* 0x3B4 */ be<u8> field_0x298;
    /* 0x3B5 */ be<u8> field_0x299;
    /* 0x3B6 */ u8 _3B6[2];
    /* 0x3B8 */ be<s32> mItemBitNo;
    /* 0x3BC */ be<u8> mItemNo;
    /* 0x3BD */ be<u8> mEnemyKind;
    /* 0x3BE */ u8 _3BE[2];
    /* 0x3C0 */ be<s32> mSwBitNo;
    /* 0x3C4 */ gptr<fopAc_ac_c> mpActor;
};
WWHD_OFFSET(daTagKbItem_c, mItemBitNo, 0x3B8);
WWHD_OFFSET(daTagKbItem_c, mpActor, 0x3C4);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* fopAcM_offSwitch(a, sw): dSv_info_c::offSwitch(sw, home.roomNo) */
static inline void fopAcM_offSwitch(fopAc_ac_c* a, s32 sw) { dComIfGs_offSwitch(sw, a->home.roomNo); }

/* 024ABAA0 */
bool daTagKbItem_c::_delete() {
    WWHD_FUNC(0x024ABAA0, bool, this);
    if (mItemNo != 0xFF /* dItemNo_NONE_e */ && mSwBitNo != 0xFF) {
        fopAcM_offSwitch(this, mSwBitNo);
    }
    return true;
}
VERIFY(0x024ABAA0, &daTagKbItem_c::_delete);

/* 024AB984 (not named by the matcher) */
void daTagKbItem_c::CreateInit() {
    WWHD_FUNC(0x024AB984, void, this);
    u32 prm = fopAcM_GetParam(this);
    mItemNo = prm & 0xFF;
    mItemBitNo = (s8)((prm >> 8) & 0xFF);
    mEnemyKind = (prm >> 16) & 0xFF;
    mSwBitNo = (prm >> 24) & 0xFF;
    field_0x298 = false;
    field_0x299 = 0;
    mpActor = nullptr;
}
VERIFY(0x024AB984, &daTagKbItem_c::CreateInit);

/* 024AB9BC */
cPhs_State daTagKbItem_c::_create() {
    WWHD_FUNC(0x024AB9BC, cPhs_State, this);
    /* fopAcM_ct(this, daTagKbItem_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = TAGKBITEM_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    CreateInit();
    if ((mItemBitNo != 0x1f && fopAcM_isItem(this, mItemBitNo)) ||
        (mSwBitNo != 0xff && fopAcM_isSwitch(this, mSwBitNo)))
    {
        return cPhs_ERROR_e;
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024AB9BC, &daTagKbItem_c::_create);

/* 024ABA9C */
static cPhs_State daTagKbItem_Create(void* i_this) {
    WWHD_FUNC(0x024ABA9C, cPhs_State, i_this);
    return static_cast<daTagKbItem_c*>(i_this)->_create();
}
VERIFY(0x024ABA9C, daTagKbItem_Create);

/* 024ABAF0 */
static BOOL daTagKbItem_Delete(void* i_this) {
    WWHD_FUNC(0x024ABAF0, BOOL, i_this);
    return static_cast<daTagKbItem_c*>(i_this)->_delete();
}
VERIFY(0x024ABAF0, daTagKbItem_Delete);

/* 024ABAF4: _draw() inlined */
static BOOL daTagKbItem_Draw(void* i_this) {
    WWHD_FUNC(0x024ABAF4, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024ABAF4, daTagKbItem_Draw);

/* 024ABAFC: _execute() inlined (USA/HD: empty) */
static BOOL daTagKbItem_Execute(void* i_this) {
    WWHD_FUNC(0x024ABAFC, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024ABAFC, daTagKbItem_Execute);

/* 024ABB98 */
static BOOL daTagKbItem_IsDelete(void* i_this) {
    WWHD_FUNC(0x024ABB98, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024ABB98, daTagKbItem_IsDelete);

/* 024ABB04 */
static void __sinit_d_a_tag_kb_item_cpp() {
    WWHD_FUNC(0x024ABB04, void, (u32)0);
    sinit_header_statics(0x1046E36C, 0x101D1B84);
}
VERIFY(0x024ABB04, __sinit_d_a_tag_kb_item_cpp);

/* 024ABBA0: daTagKbItem_c deleting destructor (compiler-generated, vtable +0xC) */
static void daTagKbItem_c_dt(daTagKbItem_c* i_this, s32 flags) {
    WWHD_FUNC(0x024ABBA0, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024ABBA0, daTagKbItem_c_dt);
