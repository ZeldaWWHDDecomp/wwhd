/** HD save-data layouts; local decomp-derived
 * Field roles are descriptive and provisional where no caller confirms them. */
#pragma once
#include "bindings.h"

struct hdSavePair_c {
    /* 0x00 */ be<u8> mPrimary;
    /* 0x01 */ be<u8> mSecondary;
    /* 0x02 */ be<u8> mReserved[2];
    void init(); void clear(); void copyFrom(hdSavePair_c*);
};
WWHD_OFFSET(hdSavePair_c,mSecondary,1);
WWHD_OFFSET(hdSavePair_c,mReserved,2);
WWHD_SIZE(hdSavePair_c,4);

hdSavePair_c* createSavePair(hdSavePair_c*);
