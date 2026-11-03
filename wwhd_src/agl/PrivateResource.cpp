/* agl::detail::PrivateResource: the graphics library's private singleton (work heap, shader-compile heap with the
 * shader source text and work buffer, the device over the library's resource archive) and the text instances of
 * its translation unit. Reconstructed from this game's instructions. */
#include "agl/PrivateResource.h"
using namespace sead;
using namespace agl::detail;

static void* allocateObject(u32 size);
static void* allocateArray(u32 size, RegistryHeap* heap, s32 alignment);
static void* allocateAlignedObject(u32 size, RegistryHeap* heap, s32 alignment);
static void releaseObject(void* object);
static void releaseArray(void* object);
static RegisteredObject* constructDisposer(RegisteredObject* object);
static RegisteredObject* constructHeapDisposer(RegisteredObject* object, RegistryHeap* heap, u32 option);
static void clearMemory(void* destination, u32 size);
static void* moveBytes(void* destination, const void* source, u32 size, u32 flags);
static RegistryHeap* currentHeap(void* manager);

static PrivateResource* sInstance;
static PrivateResourceDisposer* sDisposer;
static DisposerVtable sDisposerVtable;
static DisposerVtable sPrivateResourceVtable;
static SafeTextVtable sTextVtable;
static SafeTextVtable sHeapTextVtable;
static SafeTextVtable sDriveNameVtable;
static DisposerVtable sArchiveDeviceDisposerVtable;
static FileDeviceVtable sArchiveDeviceVtable;
static u32 sHeapManager;

/* the inline sead text length: 0 for an empty text or one longer than kMaxTextLength */
static s32 textLength(const be<u8>* text) {
    u32 address = gabi::ea(text);
    if (*gabi::at<const be<u8>>(address) == 0) return 0;
    s32 length = 0;
    do {
        ++length;
        ++address;
        if (length > kMaxTextLength) return 0;
    } while (*gabi::at<const be<u8>>(address) != 0);
    return length;
}

/* 0278540C */
PrivateResource* PrivateResource::construct(PrivateResource* resource) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (resource == nullptr) {
        resource = static_cast<PrivateResource*>(allocateObject(sizeof(PrivateResource)));
        if (resource == nullptr) return nullptr;
    }
    resource->mShaderText = nullptr;
    resource->mShaderBuildHeap = nullptr;
    resource->mVtable = &sPrivateResourceVtable;
    resource->mWorkHeap = nullptr;
    resource->mShaderScratch.mBuffer = nullptr;
    resource->mShaderScratch.mSize = 0;
    resource->mArchiveDevice = nullptr;
    resource->mDebugHeap = nullptr;
    CriticalSection::construct(&resource->mCriticalSection);
    resource->mHook.v = gabi::guest_fn((const void*)defaultResourceHook);
    return resource;
}

/* 027854AC */
void PrivateResource::createInstance(RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (sInstance != nullptr) return;
    PrivateResource* storage = static_cast<PrivateResource*>(allocateArray(sizeof(PrivateResource), heap, 4));
    if (storage != nullptr) {
        constructHeapDisposer(&storage->mDisposer, heap, 3);
        storage->mDisposer.mVtable = &sDisposerVtable;
    }
    sDisposer = static_cast<PrivateResourceDisposer*>(&storage->mDisposer);
    PrivateResource* resource = storage;
    if (resource != nullptr) resource = construct(resource);
    sInstance = resource;
}

/* 02785540 */
void PrivateResource::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    RegistryHeap* compileHeap = mShaderBuildHeap;
    mVtable = &sPrivateResourceVtable;
    if (compileHeap != nullptr) gabi::call_ptr<void>(compileHeap->dispatch()->mDestroyHeap.mFunction, compileHeap);
    mCriticalSection.destroy(2);
    if (flags & 1) releaseObject(this);
}

/* 027855BC */
void PrivateResource::initialize(RegistryHeap* parent, RegistryHeap* debugHeap, u32 workHeapSize) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SafeText> name(8);
    name->mText.v = gabi::guest_str("agl::WorkHeap");
    name->mVtable = &sTextVtable;
    mWorkHeap = FreeRegionHeap::create(workHeapSize, name.get(), parent, 1, 1);
    mDebugHeap = debugHeap;
    if (debugHeap == nullptr) return;
    mShaderBuildHeap = debugHeap;
    // the shader source text: a 1 MB cleared buffer from the shader-compile heap
    ResourceHeapText* text = static_cast<ResourceHeapText*>(allocateAlignedObject(sizeof(SafeTextBuffer), debugHeap, 4));
    if (text != nullptr) {
        be<u8>* buffer = static_cast<be<u8>*>(allocateArray(0x100000, mShaderBuildHeap, 4));
        if (buffer != nullptr) clearMemory(buffer, 0x100000);
        text->mText = buffer;
        text->mCapacity = 0x100000;
        *gabi::at<be<u8>>(gabi::ea(buffer) + 0xFFFFF) = 0;
        be<u8>* first = text->mText;
        text->mVtable = &sHeapTextVtable;
        *first = 0;
    }
    RegistryHeap* workHeap = mShaderBuildHeap;
    mShaderText = text;
    if (workHeap == nullptr) workHeap = currentHeap(gabi::at<void>(sHeapManager));
    void* workBuffer = gabi::call_ptr<void*>(workHeap->dispatch()->mAllocate.mFunction, workHeap, 0x40000, 4);
    if (workBuffer != nullptr) {
        mShaderScratch.mBuffer = static_cast<u8*>(workBuffer);
        mShaderScratch.mSize = 0x40000;
    }
}

/* 02785714 */
void PrivateResource::setArchive(ArchiveRes* archive) {
    gabi::AutoFrame frame{gabi::current_function()};
    mArchive = archive;
    ArchiveFileDevice* device = static_cast<ArchiveFileDevice*>(allocateAlignedObject(sizeof(ArchiveFileDevice), mWorkHeap, 4));
    if (device != nullptr) {
        ArchiveRes* deviceArchive = mArchive;
        gabi::FrameLocal<SafeText> driveName(8);
        driveName->mVtable = &sTextVtable;
        driveName->mText.v = gabi::guest_str("arc");
        // the file-device base constructor (its vtables are replaced by the archive device's below, before any reader)
        DeviceListNode* node = &device->mListNode;
        node->mLink.mNext = nullptr;
        node->mDevice = device;
        node->mLink.mPrevious = nullptr;
        node->mList = nullptr;
        constructDisposer(static_cast<RegisteredObject*>(&device->mDisposer));
        FixedSafeText<32>* deviceName = &device->mDriveName;
        deviceName->mText = &deviceName->mStorage[0];
        deviceName->mCapacity = 32;
        deviceName->mStorage[31] = 0;
        deviceName->mVtable = &sDriveNameVtable;
        deviceName->mStorage[0] = 0;
        device->mPermission = 1;
        // the drive name is copied from the "arc" text
        SafeTextVtable* methods = driveName->mVtable;
        be<u8>* destination = deviceName->mText;
        gabi::call_ptr<void>(methods->mAssureTermination.mFunction, driveName.get());
        s32 length = textLength(driveName->mText);
        s32 capacity = deviceName->mCapacity;
        methods = driveName->mVtable;
        if (length >= capacity) length = capacity - 1;
        gabi::call_ptr<void>(methods->mAssureTermination.mFunction, driveName.get());
        moveBytes(destination, driveName->mText, (u32)length, 0);
        destination[length] = 0;
        device->mArchive = deviceArchive;
        device->mDisposer.mVtable = &sArchiveDeviceDisposerVtable;
        device->mVtable = &sArchiveDeviceVtable;
    }
    mArchiveDevice = device;
}

/* 027858E8 */
void PrivateResourceDisposer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    mVtable = &sDisposerVtable;
    if (this == sDisposer) {
        PrivateResource* resource = sInstance;
        sDisposer = nullptr;
        gabi::call_ptr<void>(resource->mVtable->mDestroy.mFunction, resource, 2);
        sInstance = nullptr;
    }
    RegisteredObject::destroy(0);
    if (flags & 1) releaseObject(this);
}

/* 02785990 */
void agl::detail::defaultResourceHook() {}

/* ---- sead text instances of this translation unit ---- */

/* 02785994 */
void ResourceText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this != nullptr && (flags & 1)) releaseObject(this);
}
/* 027859A8 */
void ResourceDriveNameBase::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this != nullptr && (flags & 1)) releaseObject(this);
}
/* 027859BC */
void ResourceText::assureTermination() {}
/* 027859C0 */
void ResourceTextBuffer::assureTermination() {
    be<u8>* text = mText;
    s32 capacity = mCapacity;
    text[capacity - 1] = 0;
}
/* 027859D8 */
void ResourceTextBuffer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this != nullptr && (flags & 1)) releaseObject(this);
}
/* 027859EC */
void ResourceHeapText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    be<u8>* text = mText;
    mVtable = &sHeapTextVtable;
    if (text != nullptr) releaseArray(text);
    if (flags & 1) releaseObject(this);
}
/* 02785A50 */
void ResourceDriveName::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this != nullptr && (flags & 1)) releaseObject(this);
}

/* WWHD glue
data sInstance 101F8C14
data sDisposer 101F8C18
data sDisposerVtable 10163644 nocheck
data sPrivateResourceVtable 10163654 nocheck
data sTextVtable 101635B8 nocheck
data sHeapTextVtable 101635E8 nocheck
data sDriveNameVtable 10163618 nocheck
data sArchiveDeviceVtable 101439A4 nocheck
data sArchiveDeviceDisposerVtable 10143AA4 nocheck
data sHeapManager 101F8B4C nocheck
str 10163630
str 10163640
ext 0273AD10 void* allocateObject(u32)
ext 0273B0D4 void* allocateArray(u32,sead::RegistryHeap*,s32)
ext 0273B050 void* allocateAlignedObject(u32,sead::RegistryHeap*,s32)
ext 0273AF40 void releaseObject(void*)
ext 0273AFC8 void releaseArray(void*)
ext 02752A84 sead::RegisteredObject* constructDisposer(sead::RegisteredObject*)
ext 02752B0C sead::RegisteredObject* constructHeapDisposer(sead::RegisteredObject*,sead::RegistryHeap*,u32)
ext 02752BEC void sead::RegisteredObject::destroy(u32)
ext 028F521C void clearMemory(void*,u32)
ext C0009988 void* moveBytes(void*,const void*,u32,u32)
ext 02756140 sead::RegistryHeap* currentHeap(void*)
ext 02753004 sead::RegistryHeap* sead::FreeRegionHeap::create(u32,const sead::SafeText*,sead::RegistryHeap*,s32,u32)
ext 02760084 sead::CriticalSection* sead::CriticalSection::construct(sead::CriticalSection*)
ext 02760168 void sead::CriticalSection::destroy(u32)
*/
