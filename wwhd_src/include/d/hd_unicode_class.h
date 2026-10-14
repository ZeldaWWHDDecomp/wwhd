/* HD Unicode line-break classification; private game-derived source. */
#pragma once
#include "wwhd.h"
struct hdUnicodeEntry_c {
    be<u16> mCode;
    u8 mPadding[2];
    be<u32> mClass;
};
WWHD_OFFSET(hdUnicodeEntry_c, mClass, 4);
WWHD_SIZE(hdUnicodeEntry_c, 8);
u32 hdUnicode_findClass(u32 code, hdUnicodeEntry_c *entries, s32 first, s32 last);
u32 hdUnicode_getClass(u32 code);
u32 hdUnicode_getBreakRule(u32 previous, be<u32> *next);
