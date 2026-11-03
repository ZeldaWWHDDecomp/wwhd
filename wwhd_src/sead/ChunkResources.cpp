// sead chunk-resource reader: chunk index, byte helpers, allocator hooks and intrusive tree links.
// Reconstructed from this game's instructions.
#include "sead/ChunkResources.h"
static u32 sChunkAllocate;
static u32 sChunkRelease;

/* 02767098 */
void sead::chunk_resource::Index::readHeader() {
    gabi::AutoFrame frame{gabi::current_function()};
    const Header* header = reinterpret_cast<const Header*>(mData.get());
    mFormat = header->mFormat;
    u32 count = header->mCount;
    mCount = count;
    Entry* entries = NULL;
    if (count != 0) entries = (Entry*)allocate(count * sizeof(Entry));
    mEntries = entries;
    header = reinterpret_cast<const Header*>(mData.get());
    mDeclaredSize = header->mDeclaredSize;
}
/* 0276711C */
void sead::chunk_resource::Index::buildIndex() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 offset = 0x20;
    for (s32 i = 0; i < (s32)(u16)mCount; ++i) {
        const ChunkHeader* chunk = gabi::at<const ChunkHeader>(gabi::ea(mData.get()) + offset);
        Entry* entry = &mEntries.get()[i];
        entry->mData = gabi::at<const be<u8>>(gabi::ea(chunk) + sizeof(ChunkHeader));
        for (u32 byte = 0; byte < 4; ++byte) {
            const be<u8>* data = mData;
            entry->mTag[byte] = data[offset + byte];
        }
        chunk = gabi::at<const ChunkHeader>(gabi::ea(mData.get()) + offset);
        u32 size = chunk->mSize;
        entry->mSize = size;
        chunk = gabi::at<const ChunkHeader>(gabi::ea(mData.get()) + offset);
        entry->mFlags = chunk->mFlags;
        offset = (offset + 0x10 + size + 0xF) & ~0xFu;
    }
}
/* 027671BC */
void sead::chunk_resource::Index::initialize() {
    gabi::AutoFrame frame{gabi::current_function()};
    readHeader();
    buildIndex();
}
/* 027671F0 */
s32 sead::chunk_resource::Index::findTag(const char* tag) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 index = 0;
    while (index < (u16)mCount) {
        Entry* entries = mEntries;
        if (bytesEqual(entries[index].mTag, reinterpret_cast<const be<u8>*>(tag), 4)) return index;
        index = (index + 1) & 0xFFFF;
    }
    return -1;
}
/* 0276727C */
u32 sead::chunk_resource::hashName(const be<u8>* name, u32 buckets) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 hash = 0;
    for (u32 i = 0; (u8)name[i] != 0; ++i) hash = hash * 0x492 + (u8)name[i];
    u32 quotient = buckets != 0 ? hash / buckets : 0;
    return hash - quotient * buckets;
}
/* 027672B8 */
void sead::chunk_resource::setMemoryCallbacks(u32 allocate, u32 release) {
    gabi::AutoFrame frame{gabi::current_function()};
    sChunkAllocate = allocate;
    sChunkRelease = release;
}
/* 027672CC */
void* sead::chunk_resource::allocate(u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    return gabi::tail_ptr<void*>(sChunkAllocate, size);
}
/* 027672DC */
void sead::chunk_resource::release(void* pointer) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::tail_ptr<void>(sChunkRelease, pointer);
}
/* 027672EC */
u32 sead::chunk_resource::bytesEqual(const be<u8>* left, const be<u8>* right, s32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    for (s32 i = 0; i < size; ++i) if ((u8)left[i] != (u8)right[i]) return 0;
    return 1;
}
/* 02767324 */
void sead::chunk_resource::copyBytes(be<u8>* destination, const be<u8>* source, s32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    for (s32 i = 0; i < size; ++i) destination[i] = (u8)source[i];
}
/* 02767348 */
void sead::chunk_resource::TreeLink::clearLinks() {
    gabi::AutoFrame frame{gabi::current_function()};
    mPrevious = NULL;
    mParent = NULL;
    mNext = NULL;
    mChild = NULL;
}
/* 02767360 */
sead::chunk_resource::TreeLink* createChunkTreeLink(sead::chunk_resource::TreeLink* link) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (link == NULL) link = (sead::chunk_resource::TreeLink*)allocateChunkLink(0x10);
    if (link != NULL) link->clearLinks();
    return link;
}
/* 02767398 */
sead::chunk_resource::TreeLink* sead::chunk_resource::TreeLink::detach() {
    gabi::AutoFrame frame{gabi::current_function()};
    TreeLink* previous = mPrevious;
    if (previous != NULL) {
        previous->mNext = mNext;
        TreeLink* next = mNext;
        if (next != NULL) {
            next->mPrevious = mPrevious;
            mNext = NULL;
        }
        mPrevious = NULL;
        mParent = NULL;
    } else {
        TreeLink* parent = mParent;
        if (parent != NULL) {
            parent->mChild = mNext;
            mParent = NULL;
        }
        TreeLink* next = mNext;
        if (next != NULL) {
            next->mPrevious = mPrevious;
            mNext = NULL;
        }
    }
    return this;
}
/* 0276740C */
void sead::chunk_resource::TreeLink::resetChildren() {
    gabi::AutoFrame frame{gabi::current_function()};
    TreeLink* child = mChild;
    while (child != NULL) {
        TreeLink* node = child;
        child = child->mNext;
        node->resetChildren();
        node->clearLinks();
    }
}
/* 02767464 */
void sead::chunk_resource::TreeLink::reset() {
    gabi::AutoFrame frame{gabi::current_function()};
    TreeLink* node = detach();
    node->resetChildren();
    clearLinks();
}
/* 0276749C */
void sead::chunk_resource::TreeLink::appendSibling(TreeLink* node) {
    gabi::AutoFrame frame{gabi::current_function()};
    node->detach();
    TreeLink* tail = this;
    while (tail->mNext != NULL) tail = tail->mNext;
    tail->mNext = node;
    node->mPrevious = tail;
    node->mParent = tail->mParent;
}
/* 027674F0 */
void sead::chunk_resource::TreeLink::appendChild(TreeLink* node) {
    gabi::AutoFrame frame{gabi::current_function()};
    node->detach();
    TreeLink* child = mChild;
    if (child != NULL) child->appendSibling(node);
    else {
        mChild = node;
        node->mParent = this;
    }
}
/* 02767540 */
void sead::chunk_resource::TreeLink::prependChild(TreeLink* node) {
    gabi::AutoFrame frame{gabi::current_function()};
    node = node->detach();
    TreeLink* child = mChild;
    if (child != NULL) {
        node->mNext = child;
        mChild->mPrevious = node;
    }
    mChild = node;
    node->mParent = this;
}
/* WWHD glue
 data sChunkAllocate 101F8BC4
 data sChunkRelease 101F8BC8
 ext 0273AD10 void* allocateChunkLink(u32)
*/
