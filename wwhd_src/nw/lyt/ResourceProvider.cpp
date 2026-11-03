/* Resource-provider lifetime and picture color/type instances from WWHD.
 * Picture's constructor supplies the color-array callback and its table supplies the type getter.
 * No external reference used. */
#include "nw/lyt/ResourceProvider.h"
#include "nw/lyt/FontCache.h"
using namespace nw::lyt;
static u8 sProviderMethods;
static RuntimeTypeGuard sPictureTypeGuard;
static RuntimeTypeNode sPictureType;

/* 02875148 */
VertexColor* VertexColor::create(VertexColor* color) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!color) color = static_cast<VertexColor*>(providerAllocate(sizeof(VertexColor)));
    if (color) color->value = 0xFFFFFFFF;
    return color;
}

/* 02875184 */
RuntimeTypeNode* nw::lyt::pictureRuntimeType() {
    if (!(u32)sPictureTypeGuard.ready) {
        u32 baseReady = sPaneTypeGuard.ready;
        sPictureTypeGuard.ready = 1;
        if (!baseReady) {
            sPaneTypeGuard.ready = 1;
            sPaneType.parent = nullptr;
        }
        sPictureType.parent = &sPaneType;
    }
    return &sPictureType;
}

/* 028751D4 */
ResourceProvider* ResourceProvider::create(ResourceProvider* provider) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!provider) provider = static_cast<ResourceProvider*>(providerAllocate(sizeof(ResourceProvider)));
    if (provider) provider->methods = reinterpret_cast<ResourceProviderMethods*>(&sProviderMethods);
    return provider;
}

/* 02875214 */
void ResourceProvider::destroy(u32 flags) {
    if (this && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)providerDelete), this);
}

/* 02875228 */
bool ResourceProvider::loadTexture(TextureResourceView* texture, const be<u8>* name) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ResourceProviderMethods* dispatch = methods;
    gabi::FrameLocal<be<u32>> size(8);
    *size = 0;
    void* resource = gabi::call_ptr<void*>(dispatch->findResource, this, 0x74696D67u, name, size.get());
    if (!resource) return false;
    u32 length = *size;
    if (length == 0) return false;
    return providerDecodeTexture(texture, resource, length) != 0;
}

/* 028752B4 */
FontInterface* ResourceProvider::loadFont(const be<u8>* name) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ResourceProviderMethods* dispatch = methods;
    gabi::FrameLocal<be<u32>> size(8);
    *size = 0;
    auto* resource = gabi::call_ptr<FontFile*>(dispatch->findResource, this, 0x666F6E74u, name, size.get());
    if (!resource || u32(*size) == 0) return nullptr;
    auto* storage = static_cast<ResourceFont*>(providerAlignedAllocate(sizeof(ResourceFont), 4));
    if (!storage) return nullptr;
    ResourceFont* font = providerCreateResourceFont(storage);
    if (!font) return nullptr;
    if (providerAttachResourceFont(font, resource) != 0) return font;
    auto* disposal = reinterpret_cast<FontDisposeMethods*>((FontVtable*)font->vtable);
    gabi::call_ptr<void>(disposal->destroy, font, 2u);
    providerRelease(font);
    return nullptr;
}

/* WWHD glue
ext 0287F3DC void* providerAlignedAllocate(u32, s32)
ext 0287F3FC void providerRelease(void*)
ext 0287EEC4 u32 providerDecodeTexture(nw::lyt::TextureResourceView*, const void*, u32)
ext 0286ED48 nw::lyt::ResourceFont* providerCreateResourceFont(nw::lyt::ResourceFont*)
ext 0286EFEC u32 providerAttachResourceFont(nw::lyt::ResourceFont*, nw::lyt::FontFile*)
ext 0273AD10 void* providerAllocate(u32)
ext 0273AF40 void providerDelete(void*) callback
data sProviderMethods 10180488 nocheck
data sPictureTypeGuard 101FD898 nocheck
data sPictureType 101FD910 nocheck
*/
