/* Intrusive named texture entry, from allocation, name and resource-return instructions. */
#pragma once
#include "nw/lyt/TextureBinding.h"
#include "nw/ut/FrameHeap.h"
namespace nw { namespace lyt {
struct TextureCacheEntry {
    gptr<TextureCacheEntry> next, previous;
    be<u8> name[128];
    TextureResourceView resource;
    static TextureCacheEntry* create(TextureCacheEntry* entry);
    void destroy(u32 flags);
    void setName(const be<u8>* text);
};
struct TextureCache : nw::ut::HeapChildren {
    void clear();
    void destroy(u32 flags);
    TextureResourceView* append(const be<u8>* name);
    TextureResourceView* find(const be<u8>* name);
};
WWHD_OFFSET(TextureCacheEntry, name, 8);
WWHD_OFFSET(TextureCacheEntry, resource, 0x88);
WWHD_SIZE(TextureCacheEntry, 0x130);
} }
void* textureCacheAllocate(u32 size);
void textureCacheDelete(void* storage);
void* textureCacheAlignedAllocate(u32 size, s32 alignment);
void textureCacheRelease(void* storage);
void textureLinksUnlink(nw::ut::HeapChildren*, nw::ut::HeapLink*);
void textureLinksInsert(nw::ut::HeapChildren*, gptr<nw::ut::HeapLink>*, nw::ut::HeapLink*);
