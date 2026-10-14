/** HD save-data layouts; local decomp-derived
 * Field roles are descriptive and provisional where no caller confirms them. */
#pragma once
#include "bindings.h"

struct hdSaveSlotFlags_c {
    /* 0x00 */ be<u8> mProgress[5];
    /* 0x05 */ be<u8> mCreated;
    /* 0x06 */ be<u8> mLoaded;
    /* 0x07 */ be<u8> mChanged;
    /* 0x08 */ be<u8> mStatus;
    /* 0x09 */ be<u8> mReserved[7];
    void init(); void clear(); BOOL isCreated(); u32 getCreated(); void setCreated(u32);
    BOOL isLoaded(); void setLoaded(u32); BOOL isChanged(); u32 getChanged(); void setChanged(u32);
    u32 getStatus(); void setStatus(u32); void copyFrom(hdSaveSlotFlags_c*);
};
WWHD_OFFSET(hdSaveSlotFlags_c,mCreated,5);
WWHD_OFFSET(hdSaveSlotFlags_c,mLoaded,6);
WWHD_OFFSET(hdSaveSlotFlags_c,mChanged,7);
WWHD_OFFSET(hdSaveSlotFlags_c,mStatus,8);
WWHD_OFFSET(hdSaveSlotFlags_c,mReserved,9);
WWHD_SIZE(hdSaveSlotFlags_c,0x10);

hdSaveSlotFlags_c* createSaveSlotFlags(hdSaveSlotFlags_c*);
