/* WWHD sead assert texts: the delimiter text object (a vtable and a 128-character text holding the
 * whitespace delimiters " \t\r\n"), the 1024-character assert message buffer with its mutex, and the
 * translation unit's text copies.
 * Layouts from this game's static initialisers and constructor (0x90 bytes) and instructions. */
#pragma once
#include "sead/Thread.h"

namespace sead {

struct DelimiterText {
    gptr<void> mVtable;
    FixedSafeText<128> mText;
    static DelimiterText* construct(DelimiterText* object);
    void destroy(u32 flags);
};

/* a record of the C++ runtime's list of global objects destroyed at exit */
struct GlobalObjectRecord {
    gptr<void> mNext;
    gptr<void> mObject;
    be<u32> mDestructor;
};

namespace assert_text {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
}

}

WWHD_OFFSET(sead::DelimiterText, mText, 4);
WWHD_SIZE(sead::DelimiterText, 0x90);
WWHD_SIZE(sead::GlobalObjectRecord, 0xC);
WWHD_SIZE(sead::FixedSafeText<128>, 0x8C);
WWHD_SIZE(sead::FixedSafeText<1024>, 0x40C);
