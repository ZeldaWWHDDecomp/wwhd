/** HD save-data layouts; local decomp-derived
 * Field roles are descriptive and provisional where no caller confirms them. */
#pragma once
#include "bindings.h"

struct hdSaveInventory_c {
    enum { CAPACITY = 12 };
    /* 0x00 */ be<u32> mFlags;
    /* 0x04 */ be<u8> mEntries[CAPACITY];
    /* 0x10 */ be<u8> mCount;
    /* 0x11 */ be<u8> mSelection;
    /* 0x12 */ be<u8> mReserved[2];
    void init(); void clear(); void copyFrom(hdSaveInventory_c*);
    BOOL hasFlags(u32); void setFlags(u32); u32 getEntry(u32); void setEntry(u32,u32); void addCount(u32);
};
WWHD_OFFSET(hdSaveInventory_c, mEntries, 4);
WWHD_OFFSET(hdSaveInventory_c, mCount, 0x10);
WWHD_OFFSET(hdSaveInventory_c, mSelection, 0x11);
WWHD_OFFSET(hdSaveInventory_c, mReserved, 0x12);
WWHD_SIZE(hdSaveInventory_c, 0x14);

hdSaveInventory_c* createSaveInventory(hdSaveInventory_c*);
