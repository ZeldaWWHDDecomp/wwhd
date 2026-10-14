/* HD line wrapping over wide SafeStrings; private game-derived source. */
#pragma once
#include "wwhd.h"
struct hdBreakStringVtable_c {
    u8 mReserved[0x14];
    be<u32> mAssureTermination;
};
struct hdBreakString_c {
    gptr<be<u16>> mText;
    gptr<hdBreakStringVtable_c> mVtable;
    void assureTermination() {
        gabi::call_ptr<void>(mVtable->mAssureTermination, this);
    }
};
struct hdBreakBuffer_c : hdBreakString_c {
    be<s32> mCapacity;
};
struct hdBreakIterator_c {
    gptr<hdBreakString_c> mString;
    be<s32> mIndex;
    be<u32> mVtable;
};
struct hdBreakMeasureVtable_c {
    u8 mReserved[0xC];
    be<u32> mMeasure;
};
struct hdBreakMeasure_c {
    gptr<hdBreakMeasureVtable_c> mVtable;
};
struct hdBreakOptions_c {
    be<s32> mMaxLines;
    be<u8> mSkipSpaces;
    u8 mReserved[3];
    be<u32> mClassOptions;
};
WWHD_SIZE(hdBreakString_c, 8);
WWHD_OFFSET(hdBreakString_c, mVtable, 4);
WWHD_SIZE(hdBreakBuffer_c, 12);
WWHD_OFFSET(hdBreakBuffer_c, mCapacity, 8);
WWHD_SIZE(hdBreakIterator_c, 12);
WWHD_OFFSET(hdBreakIterator_c, mIndex, 4);
WWHD_OFFSET(hdBreakIterator_c, mVtable, 8);
WWHD_SIZE(hdBreakOptions_c, 12);
u32 hdTextBreak_getClass(u32 code, void *options);
hdBreakIterator_c *hdTextBreak_find(hdBreakIterator_c *result, hdBreakIterator_c *begin,
                                    hdBreakIterator_c *end, hdBreakMeasure_c *measure, void *options);
u32 hdTextBreak_wrap(hdBreakBuffer_c *output, hdBreakString_c *input, hdBreakMeasure_c *measure,
                     hdBreakOptions_c *options);
