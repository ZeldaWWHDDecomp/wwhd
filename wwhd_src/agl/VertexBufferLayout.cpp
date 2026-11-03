/* Vertex-buffer storage, attributes and cache publication in WWHD. */
// References: WWHD's own instructions and strings only.
#include "agl/VertexBufferLayout.h"
static void* allocateObject(u32 bytes);
static void releaseObject(void* object);
static void flushRangeNoSync(void* storage, u32 bytes);
static void constructAttributes(void* storage, u32 count, u32 stride, agl::AttributeConstructor constructor);
static sead::DisposerVtable sVertexBufferMethods;
static const agl::FetchFormatTable sFetchFormats;

/* 027B5BD8 */
agl::VertexBufferLayout* agl::VertexBufferLayout::construct() {
    gabi::AutoFrame frame{gabi::current_function()};
    VertexBufferLayout* result = this;
    if (gabi::ea(result) == 0) {
        result = (VertexBufferLayout*)allocateObject(sizeof(VertexBufferLayout));
        if (!result)
            return result;
    }
    result->mVtable = &sVertexBufferMethods;
    constructAttributes(result, 16, 20, BufferAttribute::construct);
    result->mStorageSize = 0;
    result->mStorage = nullptr;
    result->mCount = 0;
    result->mStride = 0;
    return result;
}
/* 027B5C68 */
void agl::VertexBufferLayout::clear() {
    for (u32 index = 0; index < 16; ++index)
        mAttributes[index].mEnabled = 0;
    mStorage = nullptr;
    mStorageSize = 0;
    mStride = 0;
    mCount = 0;
}
/* 027B5CBC */
void agl::VertexBufferLayout::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0)
        return;
    mVtable = &sVertexBufferMethods;
    clear();
    if (flags & 1)
        releaseObject(this);
}
/* 027B5CFC */
void agl::VertexBufferLayout::setStorage(void* storage, u32 stride, u32 bytes) {
    gabi::AutoFrame frame{gabi::current_function()};
    clear();
    mStride = stride;
    mStorage = storage;
    u32 size;
    if (stride == 0) {
        mCount = 1;
        mStorageSize = bytes;
        size = bytes;
    } else {
        u32 count = bytes / stride;
        mCount = count;
        size = stride * count;
        mStorageSize = size;
    }
    flushRangeNoSync(storage, size);
}
/* 027B5D60 */
void agl::VertexBufferLayout::setAttribute(u32 index, u32 format, u32 offset, u32 bufferIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    BufferAttribute* attribute = &mAttributes[index < 16 ? index : 0];
    attribute->mFormat = format;
    attribute->mOffset = offset;
    attribute->mEnabled = 1;
    attribute->mBufferIndex = bufferIndex;
    // GHS stages the image table in its own frame before the indexed load.
    using FetchFormats = be<u32>[20];
    gabi::FrameLocal<FetchFormats> formats(8);
    for (u32 entry = 0; entry < 20; ++entry)
        (*formats.get())[entry] = (u32)sFetchFormats.mValues[entry];
    attribute->mFetchFormat = (*formats.get())[format & 0xFF];
    attribute->mIndexMode = 3;
}
/* 027B5E94 */
void agl::VertexBufferLayout::flush(u32 offset, u32 bytes) {
    auto storage = mStorage.get();
    auto begin = gabi::at<void>(gabi::ea(storage) + offset);
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)flushRangeNoSync), begin, bytes);
}
/* 027B5EA4 */
agl::BufferAttribute* agl::BufferAttribute::construct(BufferAttribute* attribute) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(attribute) == 0) {
        attribute = (BufferAttribute*)allocateObject(sizeof(BufferAttribute));
        if (!attribute)
            return attribute;
    }
    attribute->mBufferIndex = 0;
    attribute->mFormat = 0;
    attribute->mEnabled = 0;
    attribute->mOffset = 0;
    return attribute;
}
/* WWHD glue
ext 0273AD10 void* allocateObject(u32 bytes)
ext 0273AF40 void releaseObject(void* object)
ext C00088A0 void flushRangeNoSync(void* storage, u32 bytes) callback
ext 028EFFD0 void constructAttributes(void* storage, u32 count, u32 stride, agl::AttributeConstructor constructor)
data sVertexBufferMethods 10169940
data sFetchFormats 101698F0
*/
