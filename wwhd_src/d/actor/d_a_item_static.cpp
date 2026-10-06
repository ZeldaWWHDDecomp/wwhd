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
#include "d/actor/d_a_item.h"

/* 02183098 */
BOOL daItem_c::checkActionNow() {
    WWHD_FUNC(0x02183098, BOOL, this);
    if (std::fabs(speedF) < 0.1f && std::fabs(old.pos.y - current.pos.y) < 1.0f) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x02183098, &daItem_c::checkActionNow);

/* 021830E0 (not named by the matcher) */
BOOL daItem_c::checkControl() {
    WWHD_FUNC(0x021830E0, BOOL, this);
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
VERIFY(0x021830E0, &daItem_c::checkControl);

/* 02183150 (not named by the matcher) */
static BOOL daItem_c_startControl(daItem_c* i_this) {
    WWHD_FUNC(0x02183150, BOOL, i_this);
    if (!i_this->checkControl()) {
        return FALSE;
    }
    i_this->mItemStatus = daItem_c::STATUS_BRING_NEZUMI;
    return TRUE;
}
VERIFY(0x02183150, daItem_c_startControl);

/* 0218319C (not named by the matcher) */
static BOOL daItem_c_endControl(daItem_c* i_this) {
    WWHD_FUNC(0x0218319C, BOOL, i_this);
    i_this->mItemStatus = daItem_c::STATUS_UNK0;
    return TRUE;
}
VERIFY(0x0218319C, daItem_c_endControl);

/* 021831AC */
BOOL daItem_c::checkLock() {
    WWHD_FUNC(0x021831AC, BOOL, this);
    if (checkActionNow()) {
        return FALSE;
    }
    if (mItemStatus == STATUS_UNK4) {
        return FALSE;
    }
    return mItemStatus == STATUS_UNK0 ? TRUE : FALSE;
}
VERIFY(0x021831AC, &daItem_c::checkLock);

/* 02183204 */
BOOL daItem_c::setLock() {
    WWHD_FUNC(0x02183204, BOOL, this);
    if (!checkLock()) {
        return FALSE;
    }
    mItemStatus = STATUS_WAIT_MAIN;
    return TRUE;
}
VERIFY(0x02183204, &daItem_c::setLock);

/* 02183250 (not named by the matcher) */
BOOL daItem_c::releaseLock() {
    WWHD_FUNC(0x02183250, BOOL, this);
    mItemStatus = STATUS_UNK0;
    return TRUE;
}
VERIFY(0x02183250, &daItem_c::releaseLock);

/* 02183260 */
static void __sinit_d_a_item_static_cpp() {
    WWHD_FUNC(0x02183260, void, (u32)0);
    sinit_header_statics(0x10464814, 0x101B7A6C);
}
VERIFY(0x02183260, __sinit_d_a_item_static_cpp);

/* 021832F4: daItemBase_c::~daItemBase_c (this TU's copy; slot 1 of the daItemBase_c vtable) */
static void daItemBase_c_dt(daItemBase_c* i_this, s32 flags) {
    WWHD_FUNC(0x021832F4, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, &i_this->mAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir */
        /* dBgS_ObjAcch::~dBgS_ObjAcch (inline): this TU's vtables, then dBgS_Acch::~dBgS_Acch */
        u32 acch = gabi::ea(&i_this->mAcch);
        gabi::store<u32>(acch + 0x20, 0x10012094);
        gabi::store<u32>(acch + 0x14, 0x100120A4);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021832F4, daItemBase_c_dt);
