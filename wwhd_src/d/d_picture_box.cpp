// HD twelve-photo storage replacement; all verified functions retained. Local only.
#include "d/d_picture_box.h"
using InitWords = be<u32>[4];
using InitTokens = dPictureInitTokenHD[3];
using PhotoScale = be<f32>[6];
using PhotoConstants = be<f32>[4];
using PhotoManager = gptr<void>;
using InitFlag = be<u8>;
static PhotoManager mPhotoManager;
static InitWords mInitWords;
static InitTokens mInitTokens;
static PhotoScale mPhotoScale;
static const PhotoConstants lPhotoScale;
static InitFlag mInitFlagA;
static InitFlag mInitFlagB;
/* 02725BE8 */
void dPictureStoreHD_c__dt(dPictureStoreHD_c* self, u32 flags) {
    if (self && (flags & 1))
        pictureStoreFree(self);
}

/* 02725BFC */
void pictureStoreNoop() {}

/* 02725C00 */
void dPictureStoreHD_c::initializeSlot(u32 slot) {
    if (slot < PICTURE_SLOT_COUNT) {
        mPhotoBuffer[slot].initialize();
        mPictureSlotSortMap[slot] = PICTURE_SLOT_NONE;
    }
}

/* 02725C60 */
void dPictureStoreHD_c::initialize() {
    for (u32 i = 0; i < PICTURE_SLOT_COUNT; ++i)
        initializeSlot(i);
    mSelectedPhotoSlot = 0;
    for (u32 i = 0; i < 3; ++i)
        mModeState[i] = 0;
}

/* 02725CE0 */
u32 dPictureStoreHD_c__ct(dPictureStoreHD_c* self) {
    u32 p = gabi::ea(self);
    if (!p)
        p = pictureStoreAllocate(sizeof(dPictureStoreHD_c));
    if (p) {
        // HD: the runtime array constructor takes the original guest constructor callback.
        pictureConstructArray(gabi::at<dPictureStoreHD_c>(p), PICTURE_SLOT_COUNT,
                              sizeof(dPictureRecordHD_c), PICTURE_RECORD_CTOR);
        gabi::at<dPictureStoreHD_c>(p)->initialize();
    }
    return p;
}

/* 02725D48 */
void dPictureStoreHD_c::clearSlot(u32 slot) {
    if (slot < PICTURE_SLOT_COUNT) {
        mPhotoBuffer[slot].clear();
        mPictureSlotSortMap[slot] = PICTURE_SLOT_NONE;
    }
}

/* 02725DA8 */
void dPictureStoreHD_c::clear() {
    for (u32 i = 0; i < PICTURE_SLOT_COUNT; ++i)
        clearSlot(i);
    mSelectedPhotoSlot = 0;
}

/* 02725E0C */
void dPictureStoreHD_c::eraseAndNotify(u32 slot) {
    if (slot >= PICTURE_SLOT_COUNT)
        return;
    mPhotoBuffer[slot].erase();
    for (u32 i = 0; i < PICTURE_SLOT_COUNT; ++i) {
        if ((u32)(s32)s8(mPictureSlotSortMap[i]) == slot) {
            for (u32 j = i; j < PICTURE_SLOT_COUNT - 1; ++j)
                mPictureSlotSortMap[j] = mPictureSlotSortMap[j + 1];
            mPictureSlotSortMap[PICTURE_SLOT_COUNT - 1] = PICTURE_SLOT_NONE;
            pictureStoreNotify(mPhotoManager, slot);
            return;
        }
    }
}

/* 02725EE4 */
void dPictureStoreHD_c::erase(u32 slot) {
    if (slot >= PICTURE_SLOT_COUNT)
        return;
    mPhotoBuffer[slot].erase();
    for (u32 i = 0; i < PICTURE_SLOT_COUNT; ++i) {
        if ((u32)(s32)s8(mPictureSlotSortMap[i]) == slot) {
            for (u32 j = i; j < PICTURE_SLOT_COUNT - 1; ++j)
                mPictureSlotSortMap[j] = mPictureSlotSortMap[j + 1];
            mPictureSlotSortMap[PICTURE_SLOT_COUNT - 1] = PICTURE_SLOT_NONE;
            return;
        }
    }
}

/* 02725FAC */
u32 dPictureStoreHD_c::valid(u32 slot) {
    if (slot >= PICTURE_SLOT_COUNT)
        return 0;
    return mPhotoBuffer[slot].valid();
}

/* 027260DC */
u32 dPictureStoreHD_c::label(u32 slot) {
    if (slot >= PICTURE_SLOT_COUNT)
        return PICTURE_LABEL_INVALID;
    return mPhotoBuffer[slot].label();
}

/* 02726120 */
u32 dPictureStoreHD_c::state(u32 slot) {
    if (slot >= PICTURE_SLOT_COUNT)
        return PICTURE_STATE_INVALID;
    return mPhotoBuffer[slot].state();
}

/* 02726164 */
u32 dPictureStoreHD_c::propertyA(u32 slot) {
    if (slot >= PICTURE_SLOT_COUNT)
        return 0;
    return mPhotoBuffer[slot].propertyA();
}

/* 02726188 */
u32 dPictureStoreHD_c::propertyB(u32 slot) {
    if (slot >= PICTURE_SLOT_COUNT)
        return 0;
    return mPhotoBuffer[slot].propertyB();
}

/* 02725FD0 */
void dPictureStoreHD_c::enableAll() {
    for (u32 i = 0; i < PICTURE_SLOT_COUNT; ++i)
        mPhotoBuffer[i].enable();
}

/* 02726044 */
void dPictureStoreHD_c::disableAll() {
    for (u32 i = 0; i < PICTURE_SLOT_COUNT; ++i)
        mPhotoBuffer[i].disable();
}

/* 02726028 */
void dPictureStoreHD_c::disableSlot(u32 slot) {
    if (slot < PICTURE_SLOT_COUNT)
        mPhotoBuffer[slot].disable();
}

/* 0272609C */
u32 dPictureStoreHD_c::record(u32 slot) {
    if (slot >= PICTURE_SLOT_COUNT)
        slot = 0;
    return gabi::ea(&mPhotoBuffer[slot]);
}

/* 027260BC */
u32 dPictureStoreHD_c::constRecord(u32 slot) {
    if (slot >= PICTURE_SLOT_COUNT)
        slot = 0;
    return gabi::ea(&mPhotoBuffer[slot]);
}

/* 02726100 */
void dPictureStoreHD_c::setLabel(u32 slot, u32 value) {
    if (slot < PICTURE_SLOT_COUNT)
        mPhotoBuffer[slot].setLabel(value);
}

/* 02726144 */
void dPictureStoreHD_c::setState(u32 slot, u32 value) {
    if (slot < PICTURE_SLOT_COUNT)
        mPhotoBuffer[slot].setState(value);
}

/* 027261AC */
s32 dPictureStoreHD_c::orderedSlot(u32 slot) {
    if (slot >= PICTURE_SLOT_COUNT)
        return -1;
    return s8(mPictureSlotSortMap[slot]);
}

/* 027261D0 */
void dPictureStoreHD_c::appendOrder(u32 slot) {
    if (slot >= PICTURE_SLOT_COUNT)
        return;
    for (u32 i = 0; i < PICTURE_SLOT_COUNT; ++i) {
        s8 v = s8(mPictureSlotSortMap[i]);
        if ((u32)(s32)v == slot)
            return;
        if (v == -1) {
            mPictureSlotSortMap[i] = slot;
            return;
        }
    }
}

/* 02726218 */
s32 dPictureStoreHD_c::firstEmpty() {
    for (u32 i = 0; i < PICTURE_SLOT_COUNT; ++i)
        if (mPhotoBuffer[i].emptyState() == 0)
            return i;
    return -1;
}

/* 027262B0 */
void dPictureStoreHD_c::setSelection(u32 value) { mSelectedPhotoSlot = value; }

/* 027262BC */
void dPictureStoreHD_c::copy(dPictureStoreHD_c* other) {
    for (u32 i = 0; i < PICTURE_SLOT_COUNT; ++i) {
        u32 src = other->constRecord(i);
        mPhotoBuffer[i].copy(gabi::at<dPictureRecordHD_c>(src));
    }
    for (u32 i = 0; i < PICTURE_SLOT_COUNT; ++i)
        mPictureSlotSortMap[i] = other->mPictureSlotSortMap[i];
    mSelectedPhotoSlot = other->mSelectedPhotoSlot;
}
// HD: compiler-generated initialization for the photo-storage replacement.
/* 02726360 */
static void __sinit_d_picture_box_cpp() {
    for (u32 i = 0; i < 4; ++i)
        mInitWords[i] = 0;
    pictureTokenInit(&mInitTokens[0]);
    mPhotoScale[0] = lPhotoScale[0];
    mPhotoScale[1] = lPhotoScale[1];
    pictureInitFlagA(&mInitFlagA);
    pictureTokenInit(&mInitTokens[1]);
    pictureInitFlagB(&mInitFlagB);
    pictureTokenInit(&mInitTokens[2]);
    mPhotoScale[2] = lPhotoScale[2];
    mPhotoScale[4] = lPhotoScale[3];
    mPhotoScale[3] = lPhotoScale[2];
    mPhotoScale[5] = lPhotoScale[3];
}

// clang-format off
/* WWHD glue
data mPhotoManager 101F8344
data mInitWords 1049F820
data mInitTokens 101F8598
data mPhotoScale 1049F804
data lPhotoScale 10140C28
data mInitFlagA 1049F81C
data mInitFlagB 1049F81D
ext 0273AD10 u32 pictureStoreAllocate(u32)
ext 0273AF40 void pictureStoreFree(dPictureStoreHD_c*)
ext 028EFFD0 void pictureConstructArray(dPictureStoreHD_c*,u32,u32,u32)
ext 027172C8 void pictureStoreNotify(void*,u32)
ext 028F026C void pictureTokenInit(dPictureInitTokenHD*)
ext 028ED6F8 void pictureInitFlagA(be<u8>*)
ext 028EAB2C void pictureInitFlagB(be<u8>*)
ext 027257DC void dPictureRecordHD_c::initialize()
ext 027258F4 void dPictureRecordHD_c::clear()
ext 02725938 void dPictureRecordHD_c::erase()
ext 0272598C u32 dPictureRecordHD_c::valid()
ext 027259B4 u32 dPictureRecordHD_c::label()
ext 027259C4 u32 dPictureRecordHD_c::state()
ext 02725A1C u32 dPictureRecordHD_c::propertyA()
ext 02725A24 u32 dPictureRecordHD_c::propertyB()
ext 0272599C void dPictureRecordHD_c::enable()
ext 027259A8 void dPictureRecordHD_c::disable()
ext 027259BC void dPictureRecordHD_c::setLabel(u32 value)
ext 027259CC void dPictureRecordHD_c::setState(u32 value)
ext 027259D4 u32 dPictureRecordHD_c::emptyState()
ext 0272597C void dPictureRecordHD_c::copy(dPictureRecordHD_c* other)
*/
// clang-format on
