/* HD tag-processing callbacks and owned handle, companion layouts. */
#pragma once
#include "wwhd.h"
struct hdTextTagOwnerTable_c {
    u8 mPrefix[0x24]; gfn mBegin; u8 mBeginPadding[4]; gfn mEnd; u8 mEndPadding[4]; gfn mRelease;
};
struct hdTextTagOwner_c { u8 mPrefix[8]; gptr<hdTextTagOwnerTable_c> mpVtable; };
struct hdTextTagHandle_c {
    gptr<void> mpVtable;
    be<u32> mHandle;
    gptr<hdTextTagOwner_c> mpOwner;
    be<u32> mContext;
    void* getVtable();
    be<u32>* setContext(u32 context);
    void clearOutput(be<u32>* output);
    hdTextTagOwner_c* getOwner();
    u32 begin();
    u32 end();
    void release();
};
WWHD_OFFSET(hdTextTagHandle_c, mpOwner, 8);
WWHD_OFFSET(hdTextTagHandle_c, mContext, 12);
WWHD_SIZE(hdTextTagHandle_c, 16);
WWHD_OFFSET(hdTextTagOwner_c, mpVtable, 8);
WWHD_SIZE(hdTextTagOwner_c, 12);
WWHD_OFFSET(hdTextTagOwnerTable_c, mBegin, 0x24);
WWHD_OFFSET(hdTextTagOwnerTable_c, mEnd, 0x2C);
WWHD_OFFSET(hdTextTagOwnerTable_c, mRelease, 0x34);
WWHD_SIZE(hdTextTagOwnerTable_c, 0x38);
