// HD: twelve-photo storage replaces the GameCube picture UI/ARAM storage; no matching tww class.
#pragma once
#include "wwhd.h"
enum {
    PICTURE_SLOT_COUNT = 12,
    PICTURE_SLOT_NONE = 255,
    PICTURE_LABEL_INVALID = 208,
    PICTURE_STATE_INVALID = 3,
    // HD: runtime array construction uses a guest code address, not a host callback.
    PICTURE_RECORD_CTOR = 0x02725780,
};
// Only the record stride and storage operations are verified here; internal record fields stay
// opaque.
struct dPictureRecordHD_c {
    u8 mRecordData[0x50040];
    void initialize();
    void clear();
    void erase();
    u32 valid();
    u32 label();
    u32 state();
    // The two independent property accessors have no confirmed GameCube equivalents.
    u32 propertyA();
    u32 propertyB();
    void enable();
    void disable();
    void setLabel(u32 value);
    void setState(u32 value);
    u32 emptyState();
    void copy(dPictureRecordHD_c* other);
};
WWHD_SIZE(dPictureRecordHD_c, 0x50040);
struct dPictureStoreHD_c {
    /* 0x000000 */ dPictureRecordHD_c mPhotoBuffer[PICTURE_SLOT_COUNT];
    /* 0x3C0300 */ be<u8> mPictureSlotSortMap[PICTURE_SLOT_COUNT];
    /* 0x3C030C */ be<u8> mSelectedPhotoSlot;
    /* 0x3C030D */ be<u8> mModeState[3];
    /* 0x3C0310 */ u8 mReserved[0x30];
    void initializeSlot(u32 slot);
    void initialize();
    void clearSlot(u32 slot);
    void clear();
    void eraseAndNotify(u32 slot);
    void erase(u32 slot);
    u32 valid(u32 slot);
    u32 label(u32 slot);
    u32 state(u32 slot);
    u32 propertyA(u32 slot);
    u32 propertyB(u32 slot);
    void enableAll();
    void disableAll();
    void disableSlot(u32 slot);
    u32 record(u32 slot);
    u32 constRecord(u32 slot);
    void setLabel(u32 slot, u32 value);
    void setState(u32 slot, u32 value);
    s32 orderedSlot(u32 slot);
    void appendOrder(u32 slot);
    s32 firstEmpty();
    void setSelection(u32 value);
    void copy(dPictureStoreHD_c* other);
};
WWHD_OFFSET(dPictureStoreHD_c, mPictureSlotSortMap, 0x3C0300);
WWHD_OFFSET(dPictureStoreHD_c, mSelectedPhotoSlot, 0x3C030C);
WWHD_OFFSET(dPictureStoreHD_c, mModeState, 0x3C030D);
WWHD_SIZE(dPictureStoreHD_c, 0x3C0340);
u32 pictureStoreAllocate(u32 size);
void pictureStoreFree(dPictureStoreHD_c* self);
void pictureConstructArray(dPictureStoreHD_c*, u32 count, u32 size, u32 ctor);
void pictureStoreNotify(void*, u32 slot);
// HD: the three global initializer objects occupy consecutive twelve-byte slots.
struct dPictureInitTokenHD {
    u8 mData[12];
};
void pictureTokenInit(dPictureInitTokenHD*);
void pictureInitFlagA(be<u8>*);
void pictureInitFlagB(be<u8>*);
