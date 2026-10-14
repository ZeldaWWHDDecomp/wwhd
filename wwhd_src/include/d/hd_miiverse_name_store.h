// HD saved Miiverse text and per-entry metadata. Private decomp-derived source.
#pragma once
#include "bindings.h"
struct hdMiiverseNameStore_c {
    enum { EntryCount = 50, EntrySize = 32 };
    char mNames[EntryCount][EntrySize];
    be<u16> mMetadata[EntryCount];
    void initialize();
    void clear();
    void getName(u32 output, u32 index);
    void setName(u32 input, u32 index);
    u32 metadata(u32 index);
    void setMetadata(u32 index, u32 value);
};
WWHD_OFFSET(hdMiiverseNameStore_c, mMetadata, 0x640);
WWHD_SIZE(hdMiiverseNameStore_c, 0x6A4);
void nameStoreFill(void*, u32, u32);
void nameStoreFormat(u32, const char*, const char*);
void nameStoreCopy(void*, u32, u32, u32);
void* nameStoreAllocate(u32);
