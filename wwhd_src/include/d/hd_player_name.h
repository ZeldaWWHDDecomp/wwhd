// HD paired UTF-16 player name and narrow display name. Private source.
#pragma once
#include "bindings.h"
struct hdNameStringVtable_c { u8 mReserved[0x14]; be<u32> mAssureTermination; };
WWHD_OFFSET(hdNameStringVtable_c, mAssureTermination, 0x14);
struct hdNameString_c {
    gptr<u8> mpText;
    gptr<hdNameStringVtable_c> mVtable;
    be<u32> mCapacity;
    be<u8> mBuffer[20];
    void assureTermination() {
        u32 target = mVtable->mAssureTermination;
        gabi::call_ptr<void>(target, this);
    }
};
WWHD_OFFSET(hdNameString_c, mpText, 0);
WWHD_OFFSET(hdNameString_c, mVtable, 4);
WWHD_OFFSET(hdNameString_c, mCapacity, 8);
WWHD_OFFSET(hdNameString_c, mBuffer, 12);
WWHD_SIZE(hdNameString_c, 32);
struct hdPlayerName_c {
    hdNameString_c mWideName;
    hdNameString_c mNarrowName;
    void initialize();
    void setNarrowName(const char* name);
    void setDefault();
    void setWideName(u8* name);
    void copyWideName(hdNameString_c* name);
    void copy(hdPlayerName_c* name);
};
WWHD_OFFSET(hdPlayerName_c, mNarrowName, 0x20);
WWHD_SIZE(hdPlayerName_c, 0x40);
void* playerNameAllocate(u32);
void playerNameFormatWide(hdNameString_c*, const char*);
void playerNameFormat(hdNameString_c*, const char*);
void playerNameFormatText(hdNameString_c*, const char*, const char*);
void playerNameConvertWide(u8*, u32, const char*, s32);
void playerNameConvertNarrow(u8*, u32, u8*, s32);
void playerNameCopy(void*, const void*, u32, u32);
