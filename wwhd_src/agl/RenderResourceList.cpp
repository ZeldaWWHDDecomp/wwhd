// Render method resource list: construction over the inline storage, ordering, unique insertion and removal.
#include "agl/RenderResourceList.h"
#include "agl/RenderStorageCopies.h"
#include "agl/RenderQueue.h"
#include "agl/RootNode.h"
using namespace agl::render;
static sead::DisposerVtable sRenderResourceListMethods;
static sead::SafeTextVtable sResourceListTextMethods;
typedef be<u8> ResourceListNameByte;
static ResourceListNameByte sResourceListName;
static const u32 kInlineCapacity = 256;

/* 027D56E4 */
agl::render::RenderResourceList* agl::render::createRenderResourceList(RenderResourceList* list) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!list) list = static_cast<RenderResourceList*>(allocateRegistryStorage(sizeof(RenderResourceList)));
    if (!list) return list;
    list->mVtable = &sRenderResourceListMethods;
    sead::PointerBuffer* entries = &list->mEntries;
    if (!gabi::ea(entries)) entries = static_cast<sead::PointerBuffer*>(allocateRegistryStorage(sizeof(list->mEntries) + sizeof(list->mInlineStorage)));
    if (entries) {
        entries->mStorage = nullptr;
        entries->mCount = 0;
        entries->mCapacity = 0;
        entries->setStorage(kInlineCapacity,entries + 1);
    }
    be<u32>* revision = &list->mRevision;
    if (!gabi::ea(revision)) revision = static_cast<be<u32>*>(allocateRegistryStorage(sizeof(be<u32>)));
    if (revision) *revision = 1;
    gabi::FrameLocal<pool_text::Text> name(8);
    name->mVtable = &sResourceListTextMethods;
    name->mText = &sResourceListName;
    agl::detail::RootNode::setNodeMeta(list,name.get());
    return list;
}
/* 027D57A8 */
void agl::render::RenderResourceList::sortEntries() {
    gabi::AutoFrame frame{gabi::current_function()};
    mEntries.cocktailSort(gabi::guest_fn((const void*)agl::compareRenderOrder));
}
/* 027D57B8 */
u32 agl::render::RenderResourceList::addUnique(void* entry) {
    s32 count = mEntries.mCount;
    gptr<void>* storage = static_cast<gptr<void>*>(static_cast<void*>(mEntries.mStorage));
    for (gptr<void>* current = storage; current != storage + count; ++current) {
        if (static_cast<void*>(*current) == entry) return 0;
    }
    if (count < (s32)mEntries.mCapacity) {
        storage[count] = entry;
        mEntries.mCount = (s32)mEntries.mCount + 1;
    }
    return 1;
}
/* 027D5818 */
u32 agl::render::RenderResourceList::removeAll(void* entry) {
    gabi::AutoFrame frame{gabi::current_function()};
    gptr<void>* current = static_cast<gptr<void>*>(static_cast<void*>(mEntries.mStorage));
    gptr<void>* end = current + (s32)mEntries.mCount;
    u32 removed = 0;
    s32 index = 0;
    while (current != end) {
        if (static_cast<void*>(*current) == entry) {
            mEntries.erase(index,1);
            end = static_cast<gptr<void>*>(static_cast<void*>(mEntries.mStorage)) + (s32)mEntries.mCount;
            ++removed;
        } else {
            ++current;
            ++index;
        }
    }
    return removed;
}
/* 027D5934 */
void agl::render::RenderResourceList::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) && (flags & 1u)) releaseRegistryStorage(this);
}
/* WWHD glue
data sRenderResourceListMethods 1016C144
data sResourceListTextMethods 1016C0C4
data sResourceListName 1016C174
ext 0273B560 void sead::PointerBuffer::setStorage(s32 capacity,void* storage)
ext 0273B6AC void sead::PointerBuffer::cocktailSort(u32 compare)
ext 0273B624 void sead::PointerBuffer::erase(s32 first,s32 count)
ext 027C087C u32 agl::compareRenderOrder(const agl::RenderOrderPrefix*,const agl::RenderOrderPrefix*) callback
ext 02785CB4 void agl::detail::RootNode::setNodeMeta(void* node,const sead::SafeText* meta)
*/
