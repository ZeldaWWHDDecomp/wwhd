/* sead chunk-resource reader: indexes the 16-byte-aligned tagged chunks of a binary resource (header, chunk count,
 * per-chunk tag/size/flags), the byte helpers and allocator hooks its users share (LMS message binaries), and an
 * intrusive parent/child/sibling tree link. Reconstructed from this game's instructions. */
#pragma once
#include "wwhd.h"
namespace sead { namespace chunk_resource {
struct Entry { gptr<const be<u8>> mData; be<u8> mTag[4]; be<u32> mSize; be<u16> mFlags; u8 mReserved[2]; };
struct Index {
    gptr<const be<u8>> mData;
    be<u32> mDeclaredSize;
    be<u8> mFormat;
    u8 mReserved;
    be<u16> mCount;
    gptr<Entry> mEntries;
    void readHeader(); void buildIndex(); void initialize(); s32 findTag(const char* tag);
};
struct TreeLink {
    gptr<TreeLink> mParent; gptr<TreeLink> mChild; gptr<TreeLink> mNext; gptr<TreeLink> mPrevious;
    void clearLinks(); TreeLink* detach(); void resetChildren(); void reset();
    void appendSibling(TreeLink* node); void appendChild(TreeLink* node); void prependChild(TreeLink* node);
};
struct Header { u8 mPrefix[12]; be<u8> mFormat; u8 mReserved; be<u16> mCount; u8 mAlignment[2]; be<u32> mDeclaredSize; };
struct ChunkHeader { be<u8> mTag[4]; be<u32> mSize; be<u16> mFlags; u8 mReserved[6]; };
u32 bytesEqual(const be<u8>* left, const be<u8>* right, s32 size);
void copyBytes(be<u8>* destination, const be<u8>* source, s32 size);
u32 hashName(const be<u8>* name, u32 buckets);
void setMemoryCallbacks(u32 allocate, u32 release); void* allocate(u32 size); void release(void* pointer);
}}
WWHD_OFFSET(sead::chunk_resource::Index,mCount,0xA);
WWHD_OFFSET(sead::chunk_resource::Index,mEntries,0xC);
WWHD_SIZE(sead::chunk_resource::Index,0x10);
WWHD_OFFSET(sead::chunk_resource::Entry,mTag,4);
WWHD_OFFSET(sead::chunk_resource::Entry,mSize,8);
WWHD_OFFSET(sead::chunk_resource::Entry,mFlags,0xC);
WWHD_SIZE(sead::chunk_resource::Entry,0x10);
WWHD_SIZE(sead::chunk_resource::TreeLink,0x10);
WWHD_SIZE(sead::chunk_resource::ChunkHeader,0x10);
void* allocateChunkLink(u32 size);

WWHD_OFFSET(sead::chunk_resource::Header,mDeclaredSize,0x12);

sead::chunk_resource::TreeLink* createChunkTreeLink(sead::chunk_resource::TreeLink*);
