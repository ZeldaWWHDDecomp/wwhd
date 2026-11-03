/* Multi-archive provider with three canonical count-and-root link containers. */
#pragma once
#include "nw/lyt/ResourceProvider.h"
#include "nw/lyt/PackedArchive.h"
#include "nw/ut/FrameHeap.h"
#include "nw/lyt/FontCache.h"
#include "nw/lyt/TextureCache.h"
namespace nw { namespace lyt {
// Reuse the canonical merged list shape; its link semantics are identical here.
using ResourceLinks = nw::ut::HeapChildren;
using ResourceLink = nw::ut::HeapLink;
struct ArchiveAttachment {
    ResourceLink link;
    PackedArchive archive;
    be<u8> path[64];
    bool attach(ArchiveHeader* source, const be<u8>* directory);
};
struct ArchiveResourceProvider : ResourceProvider {
    ResourceLinks archives, fonts, textures;
    static ArchiveResourceProvider* create(ArchiveResourceProvider* provider);
    void destroy(u32 flags);
    void clearArchives();
    TextureResourceView* texture(const be<u8>* name);
    FontInterface* font(const be<u8>* name);
    void appendArchive(ArchiveAttachment* attachment);
    void* findResource(u32 kind, const be<u8>* name, be<u32>* size);
    FontCacheEntry* registerFont(const be<u8>* name, FontInterface* font);
};
WWHD_SIZE(ArchiveResourceProvider, 0x28);
WWHD_OFFSET(ArchiveResourceProvider, archives, 4);
WWHD_OFFSET(ArchiveResourceProvider, fonts, 0x10);
WWHD_OFFSET(ArchiveResourceProvider, textures, 0x1C);
WWHD_OFFSET(ArchiveAttachment, archive, 8);
WWHD_OFFSET(ArchiveAttachment, path, 0x24);
} }
void resourceLinksErase(nw::lyt::ResourceLinks*, gptr<nw::lyt::ResourceLink>*, gptr<nw::lyt::ResourceLink>*);
void resourceLinksInsert(nw::lyt::ResourceLinks*, gptr<nw::lyt::ResourceLink>*, nw::lyt::ResourceLink*);
nw::lyt::FontCacheEntry* appendProviderFontBinding(nw::lyt::FontCache*, const be<u8>*, nw::lyt::FontInterface*, u32);
