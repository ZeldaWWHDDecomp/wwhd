#pragma once
#include "d/hd_layout_locale.h"

/** HD locale lookup views of the NW layout pane and its runtime type chain. */
struct LocalePaneType_l {
    gptr<LocalePaneType_l> mpBase;
};
struct LocaleSearchPane_l {
    gptr<LocaleSearchPane_l> mpNext;
    u8 mReserved04[4];
    gptr<gfn> mpVTable;
    u8 mReserved0C[8];
    gptr<LocaleSearchPane_l> mpFirstChild;
    u8 mReserved18[0x68];
    be<u8> mName[1];
    u8 mNamePadding[3];
};
WWHD_SIZE(LocalePaneType_l, 4);
WWHD_OFFSET(LocaleSearchPane_l, mpNext, 0);
WWHD_OFFSET(LocaleSearchPane_l, mpVTable, 8);
WWHD_OFFSET(LocaleSearchPane_l, mpFirstChild, 0x14);
WWHD_OFFSET(LocaleSearchPane_l, mName, 0x80);
WWHD_SIZE(LocaleSearchPane_l, 0x84);
