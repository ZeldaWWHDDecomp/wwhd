/* Named font cache ownership, using the merged external font interface and link layout. */
#pragma once
#include "nw/lyt/FontResource.h"
#include "nw/ut/FrameHeap.h"
namespace nw { namespace lyt {
struct FontCacheEntry {
    nw::ut::HeapLink link;
    be<u8> name[128];
    gptr<FontInterface> font;
    be<u8> ownsFont;
    u8 padding[3];
    static FontCacheEntry* create(FontCacheEntry* entry);
    void destroy(u32 flags);
    void assign(const be<u8>* name, FontInterface* font, u32 owned);
};
struct FontCache : nw::ut::HeapChildren {
    void clear();
    void destroy(u32 flags);
    FontInterface* find(const be<u8>* name);
    FontCacheEntry* append(const be<u8>* name, FontInterface* font, u32 owned);
};
struct FontDisposeMethods { u8 prefix[0x14]; be<u32> destroy; };
WWHD_SIZE(FontCacheEntry, 0x90);
WWHD_OFFSET(FontCacheEntry, name, 8);
WWHD_OFFSET(FontCacheEntry, font, 0x88);
WWHD_OFFSET(FontCacheEntry, ownsFont, 0x8C);
} }
void* fontCacheAllocate(u32 size);
void fontCacheDelete(void* object);
void fontCacheRelease(void* storage);
void* fontCacheAlignedAllocate(u32 size, s32 alignment);
void fontLinksUnlink(nw::ut::HeapChildren*, nw::ut::HeapLink*);
void fontLinksInsert(nw::ut::HeapChildren*, gptr<nw::ut::HeapLink>*, nw::ut::HeapLink*);
