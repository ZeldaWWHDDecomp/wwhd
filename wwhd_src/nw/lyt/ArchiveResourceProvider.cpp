/* Archive-provider construction and intrusive-list operations reconstructed from WWHD. */
#include "nw/lyt/ArchiveResourceProvider.h"
using namespace nw::lyt;
static ResourceProviderMethods sArchiveProviderMethods;

static void initializeResourceLinks(ResourceLinks* links) {
    if (gabi::ea(links) == 0) links = static_cast<ResourceLinks*>(providerAllocate(sizeof(ResourceLinks)));
    if (links) {
        links->count = 0;
        links->root.previous = &links->root;
        links->root.next = &links->root;
    }
}

/* 02873470 */
ArchiveResourceProvider* ArchiveResourceProvider::create(ArchiveResourceProvider* provider) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!provider) provider = static_cast<ArchiveResourceProvider*>(providerAllocate(sizeof(ArchiveResourceProvider)));
    if (provider) {
        ResourceProvider::create(provider);
        provider->methods = &sArchiveProviderMethods;
        initializeResourceLinks(&provider->archives);
        initializeResourceLinks(&provider->fonts);
        initializeResourceLinks(&provider->textures);
    }
    return provider;
}

/* 02873548 */
void ArchiveResourceProvider::clearArchives() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<gptr<ResourceLink>> first(8);
    gabi::FrameLocal<gptr<ResourceLink>> last(0xC);
    *first = archives.root.next;
    *last = &archives.root;
    resourceLinksErase(&archives, first.get(), last.get());
}

/* 02873604 */
void ArchiveResourceProvider::appendArchive(ArchiveAttachment* attachment) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<gptr<ResourceLink>> position(8);
    *position = &archives.root;
    resourceLinksInsert(&archives, position.get(), &attachment->link);
}

/* 02873584 */
void ArchiveResourceProvider::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    methods = &sArchiveProviderMethods;
    clearArchives();
    reinterpret_cast<TextureCache*>(&textures)->destroy(2);
    reinterpret_cast<FontCache*>(&fonts)->destroy(2);
    ResourceProvider::destroy(0);
    if (flags & 1) providerDelete(this);
}

struct ProviderLoadMethods {
    u8 prefix[0x34];
    be<u32> texture;
    u8 padding[4];
    be<u32> font;
};

/* 028736B8 */
TextureResourceView* ArchiveResourceProvider::texture(const be<u8>* name) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* cache = reinterpret_cast<TextureCache*>(&textures);
    TextureResourceView* result = cache->find(name);
    if (result) return result;
    result = cache->append(name);
    if (result) {
        auto* dispatch = reinterpret_cast<ProviderLoadMethods*>((ResourceProviderMethods*)methods);
        gabi::call_ptr<void>(dispatch->texture, this, result, name);
    }
    return result;
}

/* 02873738 */
FontInterface* ArchiveResourceProvider::font(const be<u8>* name) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* cache = reinterpret_cast<FontCache*>(&fonts);
    FontInterface* result = cache->find(name);
    if (result) return result;
    auto* dispatch = reinterpret_cast<ProviderLoadMethods*>((ResourceProviderMethods*)methods);
    result = gabi::call_ptr<FontInterface*>(dispatch->font, this, name);
    if (result) cache->append(name, result, 1);
    return result;
}

/* 028733DC */
bool ArchiveAttachment::attach(ArchiveHeader* source, const be<u8>* directory) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!archive.attach(source)) return false;
    auto* destination = path;
    for (u32 index = 0; index < 63; ++index) {
        u8 character = *directory++;
        if (character == 0) break;
        *destination++ = character;
    }
    *destination = 0;
    return true;
}

/* 02873638 */
void* ArchiveResourceProvider::findResource(u32 kind, const be<u8>* name, be<u32>* size) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* attachment = reinterpret_cast<ArchiveAttachment*>((ResourceLink*)archives.root.next);
    auto* sentinel = reinterpret_cast<ArchiveAttachment*>(&archives.root);
    while (gabi::ea(attachment) != gabi::ea(sentinel)) {
        void* resource = attachment->archive.findResource(attachment->path, kind, name, size);
        if (resource) return resource;
        attachment = reinterpret_cast<ArchiveAttachment*>((ResourceLink*)attachment->link.next);
    }
    return nullptr;
}

/* 028736AC */
FontCacheEntry* ArchiveResourceProvider::registerFont(const be<u8>* name, FontInterface* font) {
    return gabi::tail_ptr<FontCacheEntry*>(gabi::guest_fn((const void*)appendProviderFontBinding),
        reinterpret_cast<FontCache*>(&fonts), name, font, 0u);
}

/* WWHD glue
ext 0287430C nw::lyt::FontCacheEntry* appendProviderFontBinding(nw::lyt::FontCache*, const be<u8>*, nw::lyt::FontInterface*, u32) callback
ext 0286DACC void resourceLinksErase(nw::lyt::ResourceLinks*, gptr<nw::lyt::ResourceLink>*, gptr<nw::lyt::ResourceLink>*)
ext 0286DA70 void resourceLinksInsert(nw::lyt::ResourceLinks*, gptr<nw::lyt::ResourceLink>*, nw::lyt::ResourceLink*)
data sArchiveProviderMethods 1018028C nocheck
*/
