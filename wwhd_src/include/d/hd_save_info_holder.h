/** HD save-data layouts; local decomp-derived
 * Field roles are descriptive and provisional where no caller confirms them. */
#pragma once
#include "d/hd_save_holders.h"
#include "d/d_save.h"

struct hdSaveInfoHolder_c {
    /* 0x00 */ u8 mBase[0xC];
    /* 0x0C */ be<u32> mVtable;
    /* 0x10 */ u8 mThreadBase[0xC];
    /* 0x1C */ be<u32> mThreadVtable;
    /* 0x20 */ dSv_info_c mSaveInfo;
    /* 0x12C0 */ hdSaveHolders_c mHolders;
    /* 0xF026C0 */ u8 mPictureManager[0x6C0];
    void init();
};
WWHD_OFFSET(hdSaveInfoHolder_c,mVtable,0xC);
WWHD_OFFSET(hdSaveInfoHolder_c,mThreadVtable,0x1C);
WWHD_OFFSET(hdSaveInfoHolder_c,mSaveInfo,0x20);
WWHD_OFFSET(hdSaveInfoHolder_c,mHolders,0x12C0);
WWHD_OFFSET(hdSaveInfoHolder_c,mPictureManager,0xF026C0);
WWHD_SIZE(hdSaveInfoHolder_c,0xF02D80);

dSv_memory_c* createSaveMemory(dSv_memory_c*);
