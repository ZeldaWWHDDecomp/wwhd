/* WWHD sead resources: the resource base, direct resources over a loaded file image, the resource factory
 * base and the factory for direct resources (loading the image through a file device or a decompressor),
 * runtime types, and this translation unit's copies of its text and file-handle classes.
 * Reconstructed from this game's instructions. */
#include "sead/Resource.h"
#include "sead/ResourceMgr.h"

using namespace sead;

static void* allocateObject(u32 size);
static void releaseObject(void* object);
static void releaseArray(void* storage);
static void destroyDisposer(Disposer* disposer, u32 flags);

static constexpr u32 kDestroyRelease = 1;
static constexpr u32 kDestroyAll = 3;   // release with the object

/* vtables of this translation unit */
static ResourceVtable sResourceVtable;
static ResourceVtable sDirectResourceVtable;
static ResourceFactoryVtable sResourceFactoryVtable;
static DisposerVtable sResourceFactoryDisposerVtable;
static DisposerVtable sDirectFactoryBaseDisposerVtable;
static SafeTextVtable sResourceTextVtable;
static DisposerVtable sFileHandleVtable;

/* the runtime types: guarded statics shared with the other resource translation units */
static u32 sResourceTypeGuard;
static RuntimeType sResourceType;
static u32 sResourceTypeTable;
static u32 sDirectResourceTypeGuard;
static RuntimeType sDirectResourceType;
static u32 sDirectResourceTypeTable;

/* the file device manager (the default device) and the resource manager */
static ResourceMgr* sResourceMgr;

/* the storage of a member object: its own address, or a new block when that address is NULL
 * (GHS member constructors allocate when their object pointer is NULL) */
template <class T>
static T* memberStorage(T* member) {
    if (gabi::ea(member) == 0) member = static_cast<T*>(allocateObject(sizeof(T)));
    return member;
}

/* 0275CFD0 */
void sead::ResourceFactory::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sResourceFactoryVtable;
    mDisposer.mVtable = &sResourceFactoryDisposerVtable;
    if (sResourceMgr != NULL) sResourceMgr->unregisterFactory(this);
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275D054 */
void sead::ResourceFactory::destroyFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<ResourceFactory*>(reinterpret_cast<u8*>(disposer) - offsetof(ResourceFactory, mDisposer))
        ->destroy(flags);
}

/* 0275D05C */
void sead::DirectResourceFactoryBase::destroyFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<DirectResourceFactoryBase*>(reinterpret_cast<u8*>(disposer) -
                                                 offsetof(ResourceFactory, mDisposer))
        ->destroy(flags);
}

/* 0275D064 */
Resource* sead::Resource::construct(Resource* resource) {
    if (resource == NULL) {
        resource = static_cast<Resource*>(allocateObject(sizeof(Resource)));
        if (resource == NULL) return NULL;
    }
    TypedListNode* node = memberStorage(&resource->mNode);
    if (node != NULL) {
        node->mData = resource;
        node->mNext = NULL;
        node->mPrevious = NULL;
        node->mList = NULL;
    }
    resource->mVtable = &sResourceVtable;
    return resource;
}

/* 0275D0DC */
void sead::Resource::finishCreate() {
    dispatchPostCreate();
    if (sResourceMgr != NULL) sResourceMgr->removeFromPostCreateList(this);
}

/* 0275D12C */
DirectResource* sead::DirectResource::construct(DirectResource* resource) {
    if (resource == NULL) {
        resource = static_cast<DirectResource*>(allocateObject(sizeof(DirectResource)));
        if (resource == NULL) return NULL;
    }
    Resource::construct(resource);
    resource->mBufferSize = 0;
    resource->mRawSize = 0;
    resource->mVtable = &sDirectResourceVtable;
    resource->mRawData = NULL;
    be<u32>* flags = memberStorage(&resource->mSettingFlags);
    if (flags != NULL) *flags = 0;
    return resource;
}

/* 0275D1B4 */
void sead::DirectResource::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sDirectResourceVtable;
    if (mSettingFlags & cOwnsData) releaseArray(mRawData);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275D21C */
void sead::DirectResource::create(u8* data, u32 size, u32 bufferSize, u32 ownsData, RegistryHeap* heap) {
    if (mRawData != NULL) return;   // created twice
    ResourceVtable* vtable = mVtable;
    mRawSize = size;
    u32 settings = mSettingFlags;
    mBufferSize = bufferSize;
    mRawData = data;
    if (ownsData != 0) mSettingFlags = settings | cOwnsData;
    else mSettingFlags = settings & ~cOwnsData;
    gabi::tail_ptr<void>(vtable->mCreate.mFunction, this, data, size, heap);
}

/* 0275D274 */
Resource* sead::DirectResourceFactoryBase::tryCreateWithDecomp(ResourceLoadArg* arg, Decompressor* decompressor) {
    gabi::AutoFrame frame{gabi::current_function()};
    DirectResource* resource = dispatchNewResource(arg->mInstanceHeap, arg->mInstanceAlignment);
    if (resource == NULL) return NULL;
    gabi::FrameLocal<be<u8>> ownsData(8);
    gabi::FrameLocal<be<u32>> size(0xC);
    gabi::FrameLocal<be<u32>> bufferSize(0x10);
    *size = 0;
    *bufferSize = 0;
    *ownsData = 0;
    u8* data = decompressor->dispatchTryDecompFromDevice(arg, resource, size.get(), bufferSize.get(), ownsData.get());
    if (data == NULL) {
        resource->dispatchDestroy(kDestroyAll);
        return NULL;
    }
    resource->create(data, *size, *bufferSize, *ownsData, arg->mInstanceHeap);
    return resource;
}

/* 0275D360 */
Resource* sead::DirectResourceFactoryBase::tryCreate(ResourceLoadArg* arg) {
    gabi::AutoFrame frame{gabi::current_function()};
    DirectResource* resource = dispatchNewResource(arg->mInstanceHeap, arg->mInstanceAlignment);
    if (resource == NULL) return NULL;
    gabi::FrameLocal<LoadArg> load(8);
    load->mPath.mText = arg->mPath.mText;
    load->mPath.mVtable = &sResourceTextVtable;
    load->mNeedsRelease = 0;
    load->mHeap = arg->mLoadDataHeap.get();
    load->mBufferSize = arg->mLoadDataBufferSize;
    load->mRoundedSize = 0;
    load->mReadChunkSize = arg->mReadChunkSize;
    load->mBuffer = arg->mLoadDataBuffer;
    load->mReadSize = 0;
    if (arg->mLoadDataAlignment == 0) {
        load->mAlignment = 0;
        s32 direction = arg->mInstanceAlignment < 0 ? -1 : 1;
        load->mAlignment = direction * static_cast<s32>(resource->dispatchLoadDataAlignment());
    } else {
        load->mAlignment = arg->mLoadDataAlignment;
    }
    FileDevice* device = arg->mDevice;
    u8* data;
    if (device != NULL) data = device->tryLoad(load.get());
    else data = sFileDeviceManager->tryLoad(load.get());
    if (data == NULL) {
        resource->dispatchDestroy(kDestroyAll);
        return NULL;
    }
    resource->create(data, load->mReadSize, load->mRoundedSize, load->mNeedsRelease, arg->mInstanceHeap);
    return resource;
}

/* 0275D4C4 */
void sead::DirectResourceFactoryBase::destroy(u32 flags) {
    if (this == NULL) return;
    mDisposer.mVtable = &sDirectFactoryBaseDisposerVtable;
    ResourceFactory::destroy(0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275D524 */
void sead::resource_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0275D538 */
void sead::resource_text::Text::assureTerminationImpl() {}

/* inlined: the resource runtime type, initialised on first use */
static bool isResourceType(const RuntimeType* type) {
    if (sResourceTypeGuard == 0) {
        sResourceTypeGuard = 1;
        sResourceType.mTable = &sResourceTypeTable;
    }
    return type == &sResourceType;
}

/* 0275D53C */
bool sead::Resource::isDerivedFrom(const RuntimeType* type) const {
    return isResourceType(type);
}

/* 0275D578 */
void sead::Resource::postCreate() {}

/* 0275D57C */
bool sead::DirectResource::isDerivedFrom(const RuntimeType* type) const {
    if (sDirectResourceTypeGuard == 0) {
        sDirectResourceTypeGuard = 1;
        sDirectResourceType.mTable = &sDirectResourceTypeTable;
    }
    if (type == &sDirectResourceType) return true;
    return isResourceType(type);
}

/* 0275D5F4 */
u32 sead::DirectResource::getLoadDataAlignment() { return 4; }

/* 0275D5FC */
void sead::DirectResource::doCreate(u8* data, u32 size, RegistryHeap* heap) {}

/* 0275D600 */
void sead::resource_text::FileHandle::destroy(u32 flags) {
    if (this == NULL) return;
    FileDevice* device = mSourceDevice;
    mDisposer.mVtable = &sFileHandleVtable;
    if (device != NULL) device->tryClose(this);
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* WWHD glue
 data sResourceVtable 1015DF80 nocheck
 data sDirectResourceVtable 1015DFA8 nocheck
 data sResourceFactoryVtable 1015DFE0 nocheck
 data sResourceFactoryDisposerVtable 1015E018 nocheck
 data sDirectFactoryBaseDisposerVtable 1015E028 nocheck
 data sResourceTextVtable 1015DF48 nocheck
 data sFileHandleVtable 1015DF70 nocheck
 data sResourceTypeGuard 101FD6DC
 data sResourceType 101FD94C
 data sResourceTypeTable 1015DF38 nocheck
 data sDirectResourceTypeGuard 101FD6E0
 data sDirectResourceType 101FD944
 data sDirectResourceTypeTable 1015DF60 nocheck
 data sResourceMgr 101F8B68
 ext 0273AD10 void* allocateObject(u32)
 ext 0273AF40 void releaseObject(void*)
 ext 0273AFC8 void releaseArray(void*)
 ext 02752BEC void destroyDisposer(sead::Disposer*, u32)
 ext 02740AE8 u8* sead::FileDevice::tryLoad(sead::LoadArg*)
 ext 02741DAC u8* sead::FileDeviceManager::tryLoad(sead::LoadArg*)
 ext 02740BE0 u32 sead::FileDevice::tryClose(sead::FileHandle*)
*/
