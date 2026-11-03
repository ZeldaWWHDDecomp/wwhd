/* Named texture-cache entry lifetime and bounded name copying from WWHD. */
#include "nw/lyt/TextureCache.h"
#include "nw/lyt/ResourceTables.h"
using namespace nw::lyt;

/* 0287555C */
TextureCacheEntry* TextureCacheEntry::create(TextureCacheEntry* entry) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!entry) entry = static_cast<TextureCacheEntry*>(textureCacheAllocate(sizeof(TextureCacheEntry)));
    if (entry) {
        entry->previous = nullptr;
        entry->name[0] = 0;
        entry->resource.width = 0;
        entry->resource.height = 0;
        entry->next = nullptr;
        entry->resource.descriptorWord = 0;
    }
    return entry;
}

/* 028755AC */
void TextureCacheEntry::destroy(u32 flags) {
    if (this && (flags & 1)) {
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)textureCacheDelete), this);
    }
}

/* 028755C0 */
void TextureCacheEntry::setName(const be<u8>* text) {
    auto* destination = name;
    for (u32 count = 0; count < 127; ++count) {
        u8 character = *text++;
        if (character == 0) break;
        *destination++ = character;
    }
    *destination = 0;
}

/* 028755F4 */
void TextureCache::clear() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    while (u32(count) != 0) {
        auto* entry = reinterpret_cast<TextureCacheEntry*>((nw::ut::HeapLink*)root.next);
        textureLinksUnlink(this, reinterpret_cast<nw::ut::HeapLink*>(entry));
        if (entry) {
            entry->destroy(2);
            textureCacheRelease(entry);
        }
    }
}

/* 02875668 */
void TextureCache::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    clear();
    if (flags & 1) textureCacheDelete(this);
}

/* 028756B8 */
TextureResourceView* TextureCache::append(const be<u8>* name) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* storage = static_cast<TextureCacheEntry*>(textureCacheAlignedAllocate(sizeof(TextureCacheEntry), 4));
    if (!storage) return nullptr;
    TextureCacheEntry* entry = TextureCacheEntry::create(storage);
    if (!entry) return nullptr;
    entry->setName(name);
    gabi::FrameLocal<gptr<nw::ut::HeapLink>> position(8);
    *position = &root;
    textureLinksInsert(this, position.get(), reinterpret_cast<nw::ut::HeapLink*>(entry));
    return &entry->resource;
}

/* 02875764 */
TextureResourceView* TextureCache::find(const be<u8>* name) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* entry = reinterpret_cast<TextureCacheEntry*>((nw::ut::HeapLink*)root.next);
    auto* sentinel = reinterpret_cast<TextureCacheEntry*>(&root);
    while (gabi::ea(entry) != gabi::ea(sentinel)) {
        if (layoutTextureNameEqual(name, entry->name) != 0) return &entry->resource;
        entry = entry->next;
    }
    return nullptr;
}

/* WWHD glue
ext 0287F3DC void* textureCacheAlignedAllocate(u32, s32)
ext 0287F3FC void textureCacheRelease(void*)
ext 0286DA9C void textureLinksUnlink(nw::ut::HeapChildren*, nw::ut::HeapLink*)
ext 0286DA70 void textureLinksInsert(nw::ut::HeapChildren*, gptr<nw::ut::HeapLink>*, nw::ut::HeapLink*)
ext 0273AD10 void* textureCacheAllocate(u32)
ext 0273AF40 void textureCacheDelete(void*) callback
*/
