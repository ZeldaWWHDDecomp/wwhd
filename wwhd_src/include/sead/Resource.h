/* WWHD sead resources: the resource base (list node + runtime type), direct resources that own a loaded
 * file image, resource factories and decompressors (both registered with the resource manager under a
 * file extension), and the load arguments the manager passes them.
 * Layouts from this game's constructors (Resource 0x14, DirectResource 0x24, factory and decompressor
 * 0x50) and instructions. */
#pragma once
#include "sead/FileDevice.h"
#include "sead/HeapRegistry.h"
#include "sead/RuntimeType.h"

namespace sead {

/* a node of a typed intrusive list: the link, the element it carries and the list it is in */
struct TypedListNode : Link {
    gptr<void> mData;
    gptr<LinkList> mList;
};

/* what the manager passes a factory or decompressor to load one file */
struct ResourceLoadArg {
    SafeText mPath;
    gptr<RegistryHeap> mInstanceHeap;      // heap of the resource object
    gptr<RegistryHeap> mLoadDataHeap;      // heap of the file image (NULL: the current heap)
    be<s32> mInstanceAlignment;            // its sign is the allocation direction
    be<s32> mLoadDataAlignment;            // 0: from the resource
    gptr<u8> mLoadDataBuffer;              // caller's buffer for the file image, or NULL
    be<u32> mLoadDataBufferSize;
    gptr<struct ResourceFactory> mFactory; // NULL: chosen by the file extension
    gptr<FileDevice> mDevice;              // NULL: the device manager
    be<u32> mReadChunkSize;
};

struct Resource;
struct ResourceVtable {
    VtableSlot mHeader;
    VtableSlot mIsDerivedFrom;          // 0x08 (type) -> bool
    VtableSlot mRuntimeType;            // 0x10
    VtableSlot mDestroy;                // 0x18 (flags)
    VtableSlot mPostCreate;             // 0x20
    VtableSlot mLoadDataAlignment;      // 0x28 -> u32
    VtableSlot mCreate;                 // 0x30 (data, size, heap)
};

/* list node + vtable; the manager keeps resources that still await post-creation in a list */
struct Resource {
    TypedListNode mNode;
    gptr<ResourceVtable> mVtable;

    static Resource* construct(Resource* resource);
    void finishCreate();
    bool isDerivedFrom(const RuntimeType* type) const;
    void postCreate();

    /* virtual dispatch */
    void dispatchDestroy(u32 flags) { gabi::call_ptr<void>(mVtable->mDestroy.mFunction, this, flags); }
    void dispatchPostCreate() { gabi::call_ptr<void>(mVtable->mPostCreate.mFunction, this); }
    u32 dispatchLoadDataAlignment() { return gabi::call_ptr<u32>(mVtable->mLoadDataAlignment.mFunction, this); }
    bool dispatchIsDerivedFrom(const RuntimeType* type) {
        return gabi::call_ptr<u32>(mVtable->mIsDerivedFrom.mFunction, this, type) != 0;   // the caller tests the whole register
    }
};

/* a resource made from one file image */
struct DirectResource : Resource {
    gptr<u8> mRawData;
    be<u32> mRawSize;
    be<u32> mBufferSize;
    be<u32> mSettingFlags;   // bit 0: the image is released with the resource

    enum { cOwnsData = 1 };

    static DirectResource* construct(DirectResource* resource);
    void destroy(u32 flags);
    void create(u8* data, u32 size, u32 bufferSize, u32 ownsData, RegistryHeap* heap);
    bool isDerivedFrom(const RuntimeType* type) const;
    u32 getLoadDataAlignment();
    void doCreate(u8* data, u32 size, RegistryHeap* heap);
};

struct Decompressor;
struct ResourceFactoryVtable {
    VtableSlot mHeader[3];
    VtableSlot mDestroy;                // 0x18 (flags)
    VtableSlot mUnused20;
    VtableSlot mTryCreate;              // 0x28 (arg) -> Resource*
    VtableSlot mTryCreateWithDecomp;    // 0x30 (arg, decompressor) -> Resource*
    VtableSlot mNewResource;            // 0x38 (heap, alignment) -> DirectResource*
};

/* creates resources for one file extension */
struct ResourceFactory {
    TypedListNode mNode;
    Disposer mDisposer;
    FixedSafeText<32> mExtension;
    gptr<ResourceFactoryVtable> mVtable;

    void destroy(u32 flags);
    static void destroyFromDisposer(Disposer* disposer, u32 flags);

    void dispatchDestroy(u32 flags) { gabi::call_ptr<void>(mVtable->mDestroy.mFunction, this, flags); }
    Resource* dispatchTryCreate(ResourceLoadArg* arg) {
        return gabi::call_ptr<Resource*>(mVtable->mTryCreate.mFunction, this, arg);
    }
    Resource* dispatchTryCreateWithDecomp(ResourceLoadArg* arg, Decompressor* decompressor) {
        return gabi::call_ptr<Resource*>(mVtable->mTryCreateWithDecomp.mFunction, this, arg, decompressor);
    }
    DirectResource* dispatchNewResource(RegistryHeap* heap, s32 alignment) {
        return gabi::call_ptr<DirectResource*>(mVtable->mNewResource.mFunction, this, heap, alignment);
    }
};

/* the factory base for direct resources: load the file image, then create the resource on it */
struct DirectResourceFactoryBase : ResourceFactory {
    void destroy(u32 flags);
    static void destroyFromDisposer(Disposer* disposer, u32 flags);
    Resource* tryCreate(ResourceLoadArg* arg);
    Resource* tryCreateWithDecomp(ResourceLoadArg* arg, Decompressor* decompressor);
};

struct DecompressorVtable {
    VtableSlot mHeader;
    VtableSlot mDestroy;                // 0x08 (flags)
    VtableSlot mTryDecompFromDevice;    // 0x10 (arg, resource, size, bufferSize, ownsData) -> u8*
};

/* turns a compressed file into its image for one file extension */
struct Decompressor {
    TypedListNode mNode;
    Disposer mDisposer;
    FixedSafeText<32> mExtension;
    gptr<DecompressorVtable> mVtable;

    u8* dispatchTryDecompFromDevice(ResourceLoadArg* arg, Resource* resource, be<u32>* size, be<u32>* bufferSize,
                                    be<u8>* ownsData) {
        return gabi::call_ptr<u8*>(mVtable->mTryDecompFromDevice.mFunction, this, arg, resource, size, bufferSize,
                                   ownsData);
    }
};

/* per-TU copies of the resource translation unit: its text and its file handle */
namespace resource_text {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct FileHandle : sead::FileHandle { void destroy(u32 flags); };
}

}

WWHD_OFFSET(sead::TypedListNode, mData, 8);
WWHD_OFFSET(sead::TypedListNode, mList, 0xC);
WWHD_SIZE(sead::TypedListNode, 0x10);
WWHD_OFFSET(sead::ResourceLoadArg, mInstanceHeap, 8);
WWHD_OFFSET(sead::ResourceLoadArg, mLoadDataHeap, 0xC);
WWHD_OFFSET(sead::ResourceLoadArg, mInstanceAlignment, 0x10);
WWHD_OFFSET(sead::ResourceLoadArg, mLoadDataAlignment, 0x14);
WWHD_OFFSET(sead::ResourceLoadArg, mLoadDataBuffer, 0x18);
WWHD_OFFSET(sead::ResourceLoadArg, mLoadDataBufferSize, 0x1C);
WWHD_OFFSET(sead::ResourceLoadArg, mFactory, 0x20);
WWHD_OFFSET(sead::ResourceLoadArg, mDevice, 0x24);
WWHD_OFFSET(sead::ResourceLoadArg, mReadChunkSize, 0x28);
WWHD_SIZE(sead::ResourceLoadArg, 0x2C);
WWHD_OFFSET(sead::ResourceVtable, mIsDerivedFrom, 0x08);
WWHD_OFFSET(sead::ResourceVtable, mDestroy, 0x18);
WWHD_OFFSET(sead::ResourceVtable, mPostCreate, 0x20);
WWHD_OFFSET(sead::ResourceVtable, mLoadDataAlignment, 0x28);
WWHD_OFFSET(sead::ResourceVtable, mCreate, 0x30);
WWHD_OFFSET(sead::Resource, mVtable, 0x10);
WWHD_SIZE(sead::Resource, 0x14);
WWHD_OFFSET(sead::DirectResource, mRawData, 0x14);
WWHD_OFFSET(sead::DirectResource, mRawSize, 0x18);
WWHD_OFFSET(sead::DirectResource, mBufferSize, 0x1C);
WWHD_OFFSET(sead::DirectResource, mSettingFlags, 0x20);
WWHD_SIZE(sead::DirectResource, 0x24);
WWHD_OFFSET(sead::ResourceFactoryVtable, mDestroy, 0x18);
WWHD_OFFSET(sead::ResourceFactoryVtable, mTryCreate, 0x28);
WWHD_OFFSET(sead::ResourceFactoryVtable, mTryCreateWithDecomp, 0x30);
WWHD_OFFSET(sead::ResourceFactoryVtable, mNewResource, 0x38);
WWHD_OFFSET(sead::ResourceFactory, mDisposer, 0x10);
WWHD_OFFSET(sead::ResourceFactory, mExtension, 0x20);
WWHD_OFFSET(sead::ResourceFactory, mVtable, 0x4C);
WWHD_SIZE(sead::ResourceFactory, 0x50);
WWHD_OFFSET(sead::DecompressorVtable, mTryDecompFromDevice, 0x10);
WWHD_OFFSET(sead::Decompressor, mDisposer, 0x10);
WWHD_OFFSET(sead::Decompressor, mExtension, 0x20);
WWHD_OFFSET(sead::Decompressor, mVtable, 0x4C);
WWHD_SIZE(sead::Decompressor, 0x50);
