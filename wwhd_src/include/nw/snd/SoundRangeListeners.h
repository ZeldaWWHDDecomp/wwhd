/* Intrusive sound-memory range notifications, from WWHD's binary only. */
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
struct SoundRangeListener;
struct SoundRangeListenerTable {
    u8 prefix[0x14];
    be<u32> invalidateRange;
};
WWHD_OFFSET(SoundRangeListenerTable, invalidateRange, 0x14);
struct SoundRangeLink {
    gptr<SoundRangeLink> next;
    gptr<SoundRangeLink> previous;
};
WWHD_SIZE(SoundRangeLink, 8);
struct SoundRangeListener : SoundRangeLink {
    gptr<SoundRangeListenerTable> table;
};
WWHD_OFFSET(SoundRangeListener, table, 8);
WWHD_SIZE(SoundRangeListener, 12);
struct SoundRangeListeners {
    be<u32> count;
    SoundRangeLink root;
    static SoundRangeListeners* construct(SoundRangeListeners* list);
    static SoundRangeListeners* instance();
    static void notify(void* unused, const void* start, u32 size);
    SoundRangeLink* append(SoundRangeLink* listener);
    SoundRangeLink* remove(SoundRangeLink* listener);
};
WWHD_OFFSET(SoundRangeListeners, root, 4);
WWHD_SIZE(SoundRangeListeners, 12);
} }
