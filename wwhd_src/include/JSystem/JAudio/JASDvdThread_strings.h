#ifndef WWHD_JASDVDTHREAD_STRINGS_H
#define WWHD_JASDVDTHREAD_STRINGS_H
#include "wwhd.h"
// HD: per-TU string helpers emitted beside the JAudio file adapter.
struct AudioString {
    gptr<be<u8>> mText;
    gptr<void> mVtable;
    void assureTermination() const;
};
struct AudioBoundedString {
    AudioString mString;
    be<u32> mSize;
    void assureTermination() const;
};
WWHD_OFFSET(AudioString, mVtable, 4);
WWHD_SIZE(AudioString, 8);
WWHD_OFFSET(AudioBoundedString, mSize, 8);
WWHD_SIZE(AudioBoundedString, 0xC);
#endif
