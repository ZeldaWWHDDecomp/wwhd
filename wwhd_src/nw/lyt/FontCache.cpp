/* Font cache entry lifetime only; font implementation stays in the merged external unit. */
#include "nw/lyt/FontCache.h"
#include "nw/lyt/ResourceTables.h"
using namespace nw::lyt;

/* 02874134 */
FontCacheEntry* FontCacheEntry::create(FontCacheEntry* entry) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!entry) entry = static_cast<FontCacheEntry*>(fontCacheAllocate(sizeof(FontCacheEntry)));
    if (entry) {
        entry->ownsFont = 0;
        entry->link.next = nullptr;
        entry->font = nullptr;
        entry->link.previous = nullptr;
    }
    return entry;
}

/* 0287417C */
void FontCacheEntry::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    if (u8(ownsFont) != 0) {
        FontInterface* owned = font;
        if (owned) {
            auto* methods = reinterpret_cast<FontDisposeMethods*>((FontVtable*)owned->vtable);
            gabi::call_ptr<void>(methods->destroy, owned, 2u);
            fontCacheRelease(owned);
        }
        font = nullptr;
    }
    if (flags & 1) fontCacheDelete(this);
}

/* 0287420C */
void FontCacheEntry::assign(const be<u8>* text, FontInterface* value, u32 owned) {
    auto* destination = name;
    for (u32 count = 0; count < 127; ++count) {
        u8 character = *text++;
        if (character == 0) break;
        *destination++ = character;
    }
    *destination = 0;
    font = value;
    ownsFont = owned;
}

/* 02874248 */
void FontCache::clear() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    while (u32(count) != 0) {
        auto* entry = reinterpret_cast<FontCacheEntry*>((nw::ut::HeapLink*)root.next);
        fontLinksUnlink(this, &entry->link);
        if (entry) {
            entry->destroy(2);
            fontCacheRelease(entry);
        }
    }
}

/* 028742BC */
void FontCache::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    clear();
    if (flags & 1) fontCacheDelete(this);
}

/* 0287430C */
FontCacheEntry* FontCache::append(const be<u8>* name, FontInterface* font, u32 owned) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* storage = static_cast<FontCacheEntry*>(fontCacheAlignedAllocate(sizeof(FontCacheEntry), 4));
    if (!storage) return nullptr;
    FontCacheEntry* entry = FontCacheEntry::create(storage);
    if (!entry) return nullptr;
    entry->assign(name, font, owned);
    gabi::FrameLocal<gptr<nw::ut::HeapLink>> position(8);
    *position = &root;
    fontLinksInsert(this, position.get(), &entry->link);
    return entry;
}

/* 028743B0 */
FontInterface* FontCache::find(const be<u8>* name) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* entry = reinterpret_cast<FontCacheEntry*>((nw::ut::HeapLink*)root.next);
    auto* sentinel = reinterpret_cast<FontCacheEntry*>(&root);
    while (gabi::ea(entry) != gabi::ea(sentinel)) {
        if (layoutTextureNameEqual(name, entry->name) != 0) return entry->font;
        entry = reinterpret_cast<FontCacheEntry*>((nw::ut::HeapLink*)entry->link.next);
    }
    return nullptr;
}

/* WWHD glue
ext 0287F3DC void* fontCacheAlignedAllocate(u32, s32)
ext 0286DA9C void fontLinksUnlink(nw::ut::HeapChildren*, nw::ut::HeapLink*)
ext 0286DA70 void fontLinksInsert(nw::ut::HeapChildren*, gptr<nw::ut::HeapLink>*, nw::ut::HeapLink*)
ext 0273AD10 void* fontCacheAllocate(u32)
ext 0273AF40 void fontCacheDelete(void*)
ext 0287F3FC void fontCacheRelease(void*)
*/
