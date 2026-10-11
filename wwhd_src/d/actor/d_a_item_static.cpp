/**
 * d_a_item_static.cpp (WWHD)
 * Item - Field Item (static part, linked into the main executable on GameCube)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_a_item_static.cpp) to the WWHD layout and code, verified against
 * cking.rpx. HD: the translation unit sits between d_a_item.cpp and d_a_itembase.cpp
 * (02183098..0218338C), with checkActionNow first; it also holds this TU's copy of the
 * daItemBase_c destructor.
 */
#include "d/actor/d_a_item_static.h"

// HD: this translation unit's destructor vtables and common-header statics.
const gc_tu_data gc_tu = {
    .objAcchDt = {0x10012094, 0x100120A4},
    .headerStatics = {0x10464814, 0x101B7A6C, 0x10464820},
};

/* 02183098 */
BOOL daItem_c::checkActionNow() {
    if (std::fabs(speedF) < 0.1f && std::fabs(old.pos.y - current.pos.y) < 1.0f) {
        return FALSE;
    }
    return TRUE;
}

/* 021830E0 */
// HD: not named by the matcher.
BOOL daItem_c::checkControl() {
    if (mItemStatus == STATUS_BRING_NEZUMI) {
        return FALSE;
    }
    if (checkActionNow()) {
        return FALSE;
    }
    if (mItemStatus == STATUS_UNK4) {
        return FALSE;
    }
    if (mItemStatus == STATUS_INIT_NORMAL || mItemStatus == STATUS_MAIN_NORMAL) {
        return FALSE;
    }
    return TRUE;
}

/* 02183150 */
// HD: not named by the matcher.
BOOL daItem_c::startControl() {
    if (!checkControl()) {
        return FALSE;
    }
    mItemStatus = STATUS_BRING_NEZUMI;
    return TRUE;
}

/* 0218319C */
// HD: not named by the matcher.
BOOL daItem_c::endControl() {
    mItemStatus = STATUS_UNK0;
    return TRUE;
}

/* 021831AC */
BOOL daItem_c::checkLock() {
    if (checkActionNow()) {
        return FALSE;
    }
    if (mItemStatus == STATUS_UNK4) {
        return FALSE;
    }
    return mItemStatus == STATUS_UNK0 ? TRUE : FALSE;
}

/* 02183204 */
BOOL daItem_c::setLock() {
    if (!checkLock()) {
        return FALSE;
    }
    mItemStatus = STATUS_WAIT_MAIN;
    return TRUE;
}

/* 02183250 */
// HD: not named by the matcher.
BOOL daItem_c::releaseLock() {
    mItemStatus = STATUS_UNK0;
    return TRUE;
}

// HD: compiler-generated common-header initializer and deleting destructor.

/* 02183260 */
static void __sinit_d_a_item_static_cpp() {
    __sinit_header_statics();
}

/* 021832F4 */
// HD: daItemBase_c::~daItemBase_c, this TU's copy (vtable slot 1).
static void daItemBase_c_dtor(daItemBase_c* i_this, s32 flags) {
    if (i_this != NULL) {
        __dt(&i_this->mCyl);
        __dt(&i_this->mStts);
        __dt(&i_this->mAcchCir);
        __dt(&i_this->mAcch);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* WWHD glue (tools/verify/autoglue.py): no translation-unit tables or strings. */
