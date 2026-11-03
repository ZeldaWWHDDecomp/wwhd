/* File-route forwarding and multi-device selection, reconstructed from WWHD.
 * Canonical link/pointer-buffer layouts are our merged foundation code. */

// prim/seadSafeString.h: declarations only; no bodies/comments or conditional slot order used.
#include "sead/FileRouting.h"
using namespace sead;
using namespace sead::file_routes;
extern void releaseFileRoute(void*);
static u32 sReportCollectionTable;
static DisposerVtable sPrefixDisposerVtable;
static FileDeviceVtable sMultiDeviceVtable;
static DisposerVtable sMultiDisposerVtable;

/* 0273EEF0 */
void sead::file_routes::ReportCollection::destroy(u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (this == NULL) return;
    sead::Link* node = mNext;
    mTable = gabi::ea(&sReportCollectionTable);
    while (node != this) {
        sead::EmbeddedObject* object = reinterpret_cast<ReportLink*>(node)->mObject;
        u8 registered = object->mRegistered;
        node = node->mNext;
        if (registered != 0) {
            sead::LinkList* owner = object->mOwner;
            if (owner != NULL) {
                object->mOwner = NULL;
                object->mRegistration.detach();
                owner->mCount = (u32)owner->mCount - 1;
            }
            object->mRegistered = 0;
        }
    }
    if (flags & 1) releaseFileRoute(this);
}
/* 0273EF90 */
void sead::file_routes::PrefixDevice::destroy(u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (this == NULL) return;
    mDisposer.mVtable = &sPrefixDisposerVtable;
    FileDevice::destroy(0);
    if (flags & 1) releaseFileRoute(this);
}
/* 0273EFF0 */
static void destroyPrefixDisposer(void* disposer,u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    auto* device = reinterpret_cast<PrefixDevice*>(static_cast<u8*>(disposer)-0x10);
    device->destroy(flags);
}
/* 0273F254 */
u32 sead::file_routes::PrefixDevice::doIsAvailable() {
    gabi::AutoFrame originalFrame{gabi::current_function()}; return mTarget->isAvailable(); }
/* 0273F464 */
u32 sead::file_routes::PrefixDevice::doClose(FileHandle* handle) {
    gabi::AutoFrame originalFrame{gabi::current_function()}; return mTarget->tryClose(handle); }
/* 0273F46C */
u32 sead::file_routes::PrefixDevice::doRead(be<u32>* readSize,FileHandle* handle,u8* buffer,u32 size) {
    gabi::AutoFrame originalFrame{gabi::current_function()}; return mTarget->tryRead(readSize,handle,buffer,size); }
/* 0273F474 */
u32 sead::file_routes::PrefixDevice::doWrite(be<u32>* written,FileHandle* handle,const u8* buffer,u32 size) {
    gabi::AutoFrame originalFrame{gabi::current_function()}; return mTarget->tryWrite(written,handle,buffer,size); }
/* 0273F47C */
u32 sead::file_routes::PrefixDevice::doSeek(FileHandle* handle,s32 offset,u32 origin) {
    gabi::AutoFrame originalFrame{gabi::current_function()}; return mTarget->trySeek(handle,offset,origin); }
/* 0273F484 */
u32 sead::file_routes::PrefixDevice::doGetFileSize(be<u32>* size,FileHandle* handle) {
    gabi::AutoFrame originalFrame{gabi::current_function()}; return mTarget->tryGetFileSize(size,handle); }
/* 0273F884 */
u32 sead::file_routes::PrefixDevice::doCloseDirectory(DirectoryHandle* handle) {
    gabi::AutoFrame originalFrame{gabi::current_function()}; return mTarget->tryCloseDirectory(handle); }
/* 0273F88C */
u32 sead::file_routes::PrefixDevice::doReadDirectory(be<u32>* readCount,DirectoryHandle* handle,DirectoryEntry* entries,u32 count) {
    gabi::AutoFrame originalFrame{gabi::current_function()}; return mTarget->tryReadDirectory(readCount,handle,entries,count); }
/* 0273F894 */
u32 sead::file_routes::PrefixDevice::isMatchDevice(HandleBase* handle) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    FileDevice* target = mTarget;
    return gabi::tail_ptr<u32>(target->mVtable->mIsMatchDevice.mFunction,target,handle);
}
/* 0273F964 */
void sead::file_routes::MultiDevice::destroy(u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (this == NULL) return;
    mVtable = &sMultiDeviceVtable;
    mDisposer.mVtable = &sMultiDisposerVtable;
    mDevices.release();
    FileDevice::destroy(0);
    if (flags & 1) releaseFileRoute(this);
}
/* 0273F9D8 */
static void destroyMultiDisposer(void* disposer,u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    auto* device = reinterpret_cast<MultiDevice*>(static_cast<u8*>(disposer)-0x10);
    device->destroy(flags);
}
/* 0273FBB8 */
u32 sead::file_routes::MultiDevice::appendDevice(FileDevice* device) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    s32 count = mDevices.mCount;
    if (count >= (s32)mDevices.mCapacity) return 0;
    auto* storage = static_cast<gptr<FileDevice>*>(mDevices.mStorage.get());
    storage[count] = device;
    mDevices.mCount = (u32)mDevices.mCount+1;
    return 1;
}
/* 0273FBF0 */
FileDevice* sead::file_routes::MultiDevice::findDevice(SafeText* path) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    auto* cursor = static_cast<gptr<FileDevice>*>(mDevices.mStorage.get());
    auto* end = cursor+(s32)mDevices.mCount;
    if (cursor == end) return NULL;
    gabi::FrameLocal<be<u8>> exists(8);
    do {
        FileDevice* device = *cursor;
        if (device->isAvailable() != 0) {
            *exists = 0;
            device->tryIsExistFile(exists.get(),path);
            if ((u8)*exists != 0) return device;
        }
        auto* storage = static_cast<gptr<FileDevice>*>(mDevices.mStorage.get());
        ++cursor;
        end = storage+(s32)mDevices.mCount;
    } while (cursor != end);
    return NULL;
}
/* 0273FC90 */
u32 sead::file_routes::MultiDevice::doIsAvailable() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    auto* cursor = static_cast<gptr<FileDevice>*>(mDevices.mStorage.get());
    auto* end = cursor+(s32)mDevices.mCount;
    if (cursor == end) return 0;
    do {
        if ((*cursor)->isAvailable() != 0) return 1;
        auto* storage = static_cast<gptr<FileDevice>*>(mDevices.mStorage.get());
        ++cursor;
        end = storage+(s32)mDevices.mCount;
    } while (cursor != end);
    return 0;
}
/* 0273FD24 */
u8* sead::file_routes::MultiDevice::doLoad(LoadArg* arg) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    FileDevice* device = findDevice(&arg->mPath);
    if (device == NULL) return NULL;
    mLastDevice = device;
    return device->tryLoad(arg);
}
/* 0273FD8C */
FileDevice* sead::file_routes::MultiDevice::doOpen(FileHandle* handle,SafeText* path,u32 mode) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    FileDevice* device = findDevice(path);
    if (device == NULL) return NULL;
    mLastDevice = device;
    u32 readChunkSize = handle->mReadChunkSize;
    return device->tryOpen(handle,path,mode,readChunkSize);
}
/* 0273FE24 */
u32 sead::file_routes::MultiDevice::doClose(FileHandle* handle) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    FileDevice* device = handle->mDevice;
    mLastDevice = device;
    return device->tryClose(handle);
}
/* 0273FE34 */
u32 sead::file_routes::MultiDevice::doRead(be<u32>* readSize,FileHandle* handle,u8* buffer,u32 size) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    FileDevice* device = handle->mDevice;
    mLastDevice = device;
    return device->tryRead(readSize,handle,buffer,size);
}
/* 0273FE44 */
u32 sead::file_routes::MultiDevice::doWrite(be<u32>* written,FileHandle* handle,const u8* buffer,u32 size) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    FileDevice* device = handle->mDevice;
    mLastDevice = device;
    return device->tryWrite(written,handle,buffer,size);
}
/* 0273FE54 */
u32 sead::file_routes::MultiDevice::doSeek(FileHandle* handle,s32 offset,u32 origin) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    FileDevice* device = handle->mDevice;
    mLastDevice = device;
    return device->trySeek(handle,offset,origin);
}
/* 0273FE64 */
u32 sead::file_routes::MultiDevice::doGetFileSize(be<u32>* size,FileHandle* handle) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    FileDevice* device = handle->mDevice;
    mLastDevice = device;
    return device->tryGetFileSize(size,handle);
}
/* 0273FE74 */
u32 sead::file_routes::MultiDevice::doIsExistFile(be<u8>* exists,SafeText* path) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    FileDevice* device = findDevice(path);
    mLastDevice = device;
    *exists = device != NULL;
    return 1;
}
/* 027401C4 */
u32 sead::file_routes::MultiDevice::isMatchDevice(HandleBase* handle) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    auto* cursor = static_cast<gptr<FileDevice>*>(mDevices.mStorage.get());
    auto* end = cursor+(s32)mDevices.mCount;
    if (cursor == end) return 0;
    do {
        FileDevice* device = *cursor;
        if (device->dispatchIsMatchDevice(handle) != 0) return 1;
        ++cursor;
        auto* storage = static_cast<gptr<FileDevice>*>(mDevices.mStorage.get());
        end = storage+(s32)mDevices.mCount;
    } while (cursor != end);
    return 0;
}
extern void* allocateFileRoute(u32);
extern void initializeRouteDisposer(sead::Disposer*);
extern void moveRouteBytes(void*,const void*,u32,u32);
extern void registerRouteStorage(void*);
static SafeTextVtable sNameTextMethods;
static SafeTextVtable sPrefixTextMethods;
static SafeTextVtable sPathTextMethods;
static SafeTextVtable sMultiNameTextMethods;
static SafeTextVtable sAnyTextMethods;
static const char sAnyName[4] = {97,110,121,0};
static SafeTextVtable sDirectoryTextMethods;
static FileDeviceVtable sPrefixDeviceVtable;
static PathBuffer sDirectoryPath;
static u32 sDirectoryPathRegistration;

/* inlined text operations retained with original callback/load order */
static void assureRouteText(SafeText* text) {
    text->assureTermination();
}
static s32 routeTextLength(SafeText* text) {
    const be<u8>* cursor = text->mText;
    s32 length = 0;
    if ((u8)*cursor == 0) return 0;
    do {
        ++length;
        ++cursor;
        if (length > 0x40000) return 0;
    } while ((u8)*cursor != 0);
    return length;
}
static SafeTextBuffer* initializeRouteText(SafeTextBuffer* buffer,s32 capacity,SafeTextVtable* vtable) {
    if (buffer == NULL) {
        buffer = static_cast<SafeTextBuffer*>(allocateFileRoute((u32)capacity+sizeof(SafeTextBuffer)));
        if (buffer == NULL) return NULL;
    }
    auto* storage = reinterpret_cast<be<u8>*>(buffer+1);
    storage[capacity-1] = 0;
    buffer->mText = storage;
    storage[0] = 0;
    buffer->mVtable = vtable;
    buffer->mCapacity = capacity;
    return buffer;
}
static void copyRouteText(SafeTextBuffer* buffer,SafeText* source) {
    be<u8>* destination = buffer->mText;
    assureRouteText(source);
    s32 length = routeTextLength(source);
    s32 capacity = buffer->mCapacity;
    if (length >= capacity) length = capacity-1;
    assureRouteText(source);
    moveRouteBytes(destination,source->mText,(u32)length,0);
    destination[length] = 0;
}
/* the file-device base constructor, inlined */
static void initializeRouteBase(FileDevice* device) {
    DeviceListNode* node = &device->mListNode;
    node->mLink.mNext = NULL;
    node->mDevice = device;
    node->mLink.mPrevious = NULL;
    node->mList = NULL;
    initializeRouteDisposer(&device->mDisposer);
    device->mDisposer.mVtable = &sFileDeviceDisposerVtable;
    device->mVtable = &sFileDeviceVtable;
}
static void initializeRoutePath(PathBuffer* path) {
    path->mText = &path->mStorage[0];
    path->mStorage[511] = 0;
    path->mVtable = &sPathTextMethods;
    path->mStorage[0] = 0;
    path->mCapacity = 512;
}
static void joinRoutePath(PathBuffer* path,SafeText* prefix,SafeText* suffix) {
    be<u8>* destination = &path->mStorage[0];
    assureRouteText(prefix);
    s32 length = routeTextLength(prefix);
    s32 capacity = path->mCapacity;
    if (length >= capacity) length = capacity-1;
    assureRouteText(prefix);
    moveRouteBytes(destination,prefix->mText,(u32)length,0);
    destination[length] = 0;
    destination = path->mText;
    assureRouteText(path);
    s32 previous = routeTextLength(path);
    s32 offset = previous;
    if (offset < 0) offset = 0;
    assureRouteText(suffix);
    length = routeTextLength(suffix);
    capacity = (u32)path->mCapacity-(u32)offset;
    if (length >= capacity) length = capacity-1;
    if (length > 0) {
        be<u8>* append = destination+offset;
        assureRouteText(suffix);
        moveRouteBytes(append,suffix->mText,(u32)length,0);
        s32 end = (u32)offset+(u32)length;
        if (end > previous) destination[end] = 0;
    }
}
/* 0273EFF8 */
static PrefixDevice* createPrefixRoute(PrefixDevice* device,SafeText* driveName,FileDevice* target,SafeText* prefix) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (device == NULL) {
        device = static_cast<PrefixDevice*>(allocateFileRoute(sizeof(PrefixDevice)));
        if (device == NULL) return NULL;
    }
    initializeRouteBase(device);
    initializeRouteText(&device->mDriveName,32,&sNameTextMethods);
    device->mPermission = 1;
    copyRouteText(&device->mDriveName,driveName);
    device->mTarget = target;
    device->mDisposer.mVtable = &sPrefixDisposerVtable;
    device->mVtable = &sPrefixDeviceVtable;
    initializeRouteText(&device->mPrefix,256,&sPrefixTextMethods);
    if (target != NULL) copyRouteText(&device->mPrefix,prefix);
    return device;
}
/* 0273F25C */
FileDevice* sead::file_routes::PrefixDevice::doOpen(FileHandle* handle,SafeText* path,u32 mode) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<PathBuffer> combined(8);
    initializeRoutePath(combined.get());
    joinRoutePath(combined.get(),&mPrefix,path);
    FileDevice* target = mTarget;
    u32 readChunkSize = handle->mReadChunkSize;
    return target->tryOpen(handle,combined.get(),mode,readChunkSize);
}
/* 0273F48C */
u32 sead::file_routes::PrefixDevice::doIsExistFile(be<u8>* exists,SafeText* path) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<PathBuffer> combined(8);
    initializeRoutePath(combined.get());
    joinRoutePath(combined.get(),&mPrefix,path);
    return mTarget->tryIsExistFile(exists,combined.get());
}
/* 0273F688 */
FileDevice* sead::file_routes::PrefixDevice::doOpenDirectory(DirectoryHandle* handle,SafeText* path) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<PathBuffer> combined(8);
    initializeRoutePath(combined.get());
    joinRoutePath(combined.get(),&mPrefix,path);
    return mTarget->tryOpenDirectory(handle,combined.get());
}
/* 0273F9E0 */
static MultiDevice* createMultiRoute(MultiDevice* device,s32 capacity,void* heap,s32 alignment) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (device == NULL) {
        device = static_cast<MultiDevice*>(allocateFileRoute(sizeof(MultiDevice)));
        if (device == NULL) return NULL;
    }
    gabi::FrameLocal<SafeText> name(8);
    name->mVtable = &sAnyTextMethods;
    name->mText = const_cast<be<u8>*>(reinterpret_cast<const be<u8>*>(sAnyName));
    initializeRouteBase(device);
    initializeRouteText(&device->mDriveName,32,&sMultiNameTextMethods);
    device->mPermission = 1;
    copyRouteText(&device->mDriveName,name.get());
    device->mVtable = &sMultiDeviceVtable;
    device->mDisposer.mVtable = &sMultiDisposerVtable;
    sead::PointerBuffer* buffer = &device->mDevices;
    if (buffer == NULL) buffer = static_cast<sead::PointerBuffer*>(allocateFileRoute(sizeof(sead::PointerBuffer)));
    if (buffer != NULL) {
        buffer->mCapacity = 0;
        buffer->mCount = 0;
        buffer->mStorage = NULL;
    }
    device->mLastDevice = NULL;
    device->mDevices.allocate(capacity,heap,alignment);
    return device;
}
/* 0273FEC4 */
FileDevice* sead::file_routes::MultiDevice::doOpenDirectory(DirectoryHandle* handle,SafeText* path) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    const be<u8>* savedPath = sDirectoryPath.mText;
    if ((u8)*savedPath != 0) return NULL;
    auto* cursor = static_cast<gptr<FileDevice>*>(mDevices.mStorage.get());
    auto* end = cursor+(s32)mDevices.mCount;
    FileDevice* selected = NULL;
    while (cursor != end) {
        selected = (*cursor)->tryOpenDirectory(handle,path);
        if (selected != NULL) break;
        ++cursor;
    }
    if (selected != NULL) {
        mLastDevice = selected;
        copyRouteText(&sDirectoryPath,path);
    }
    return selected;
}
/* 02740014 */
u32 sead::file_routes::MultiDevice::doCloseDirectory(DirectoryHandle* handle) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    mLastDevice = handle->mDevice;
    be<u8>* path = sDirectoryPath.mText;
    *path = 0;
    return mLastDevice->tryCloseDirectory(handle);
}
/* 02740034 */
u32 sead::file_routes::MultiDevice::doReadDirectory(be<u32>* count,DirectoryHandle* handle,DirectoryEntry* entries,u32 capacity) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> fetched(8);
    *fetched = 0;
    u32 read = handle->mDevice->tryReadDirectory(fetched.get(),handle,entries,capacity);
    if (read == 0) {
        if (count != NULL) *count = *fetched;
        return 0;
    }
    u32 obtained = *fetched;
    if (obtained >= capacity) {
        if (count != NULL) *count = obtained;
        return 1;
    }
    auto* cursor = static_cast<gptr<FileDevice>*>(mDevices.mStorage.get());
    auto* end = cursor+(s32)mDevices.mCount;
    u32 remaining = capacity-obtained;
    entries += obtained;
    u32 total = obtained;
    while (cursor != end) {
        FileDevice* current = handle->mDevice;
        FileDevice* candidate = *cursor;
        ++cursor;
        if (current == candidate) break;
    }
    for (;;) {
        if (cursor == end) {
            if (count != NULL) *count = total;
            return 1;
        }
        FileDevice* opened = (*cursor)->tryOpenDirectory(handle,&sDirectoryPath);
        if (opened == NULL) {
            if (count != NULL) *count = total;
            return 0;
        }
        mLastDevice = opened;
        read = handle->mDevice->tryReadDirectory(fetched.get(),handle,entries,remaining);
        if (read == 0) {
            if (count != NULL) *count = total;
            return 0;
        }
        obtained = *fetched;
        if (obtained >= remaining) {
            if (count != NULL) *count = total+obtained;
            return 1;
        }
        remaining -= obtained;
        ++cursor;
        entries += obtained;
        total += obtained;
        auto* storage = static_cast<gptr<FileDevice>*>(mDevices.mStorage.get());
        end = storage+(s32)mDevices.mCount;
    }
}
/* 02740278 */
static void initializeDirectoryRoutePath() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    sDirectoryPath.mStorage[511] = 0;
    sDirectoryPath.mStorage[0] = 0;
    sDirectoryPath.mText = &sDirectoryPath.mStorage[0];
    sDirectoryPath.mCapacity = 512;
    sDirectoryPath.mVtable = &sDirectoryTextMethods;
    registerRouteStorage(&sDirectoryPathRegistration);
}

/* 0273F8A8 */
void sead::file_routes::InlineTextRelease::destroy(u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (this != NULL && (flags & 1)) releaseFileRoute(this);
}
/* 0273F8BC */
void sead::file_routes::ImmutableTextRelease::destroy(u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (this != NULL && (flags & 1)) releaseFileRoute(this);
}
/* 0273F8D0 */
void sead::file_routes::Text::assureTerminationImpl() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
}
/* 0273F8D4 */
void sead::file_routes::TextBuffer::destroy(u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (this != NULL && (flags & 1)) releaseFileRoute(this);
}
/* 0273F900 */
void sead::file_routes::PrefixBuffer::destroy(u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (this != NULL && (flags & 1)) releaseFileRoute(this);
}
/* 0273F914 */
void sead::file_routes::OwnedTextRelease::destroy(u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (this != NULL && (flags & 1)) releaseFileRoute(this);
}
/* 0273F928 */
void sead::file_routes::NameBuffer::destroy(u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (this != NULL && (flags & 1)) releaseFileRoute(this);
}
/* 0273F93C */
void sead::file_routes::FixedTextRelease::destroy(u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (this != NULL && (flags & 1)) releaseFileRoute(this);
}
/* 0273F950 */
void sead::file_routes::PathBuffer::destroy(u32 flags) {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    if (this != NULL && (flags & 1)) releaseFileRoute(this);
}
/* 0273F8E8 */
void sead::file_routes::TextBuffer::assureTerminationImpl() {
    gabi::AutoFrame originalFrame{gabi::current_function()};
    be<u8>* text = mText;
    u32 capacity = mCapacity;
    text[capacity-1] = 0;
}
/* WWHD glue
 data sNameTextMethods 101436B8
 data sPrefixTextMethods 10143688
 data sPathTextMethods 101436E8
 data sMultiNameTextMethods 10143848
 data sAnyTextMethods 10143800
 data sAnyName 10143890
 data sDirectoryTextMethods 10143878
 data sPrefixDeviceVtable 10143700
 data sDirectoryPath 1049FFC0
 data sDirectoryPathRegistration 101F8AFC
 ext 0273AD10 void* allocateFileRoute(u32)
 ext 02752A84 void initializeRouteDisposer(sead::Disposer*)
 ext C0009988 void moveRouteBytes(void*,const void*,u32,u32)
 ext 028F026C void registerRouteStorage(void*)
 ext 0273B584 void sead::PointerBuffer::allocate(s32,void*,s32)

 ext 0273B310 sead::Link* sead::Link::detach()
 ext 0273B5DC void sead::PointerBuffer::release()
 data sReportCollectionTable 10143620
 data sPrefixDisposerVtable 101437E0
 data sMultiDeviceVtable 10143894
 data sMultiDisposerVtable 10143974
 ext 0273AF40 void releaseFileRoute(void*)
 ext 02740A38 void sead::FileDevice::destroy(u32)
 ext 02740AC4 u32 sead::FileDevice::isAvailable()
 ext 02740AE8 u8* sead::FileDevice::tryLoad(sead::LoadArg*)
 ext 02740B24 sead::FileDevice* sead::FileDevice::tryOpen(sead::FileHandle*,sead::SafeText*,u32,u32)
 ext 02740BE0 u32 sead::FileDevice::tryClose(sead::FileHandle*)
 ext 02740C9C u32 sead::FileDevice::tryRead(be<u32>*,sead::FileHandle*,u8*,u32)
 ext 02740E08 u32 sead::FileDevice::tryWrite(be<u32>*,sead::FileHandle*,const u8*,u32)
 ext 02740EB8 u32 sead::FileDevice::trySeek(sead::FileHandle*,s32,u32)
 ext 02740F7C u32 sead::FileDevice::tryGetFileSize(be<u32>*,sead::FileHandle*)
 ext 02740FB0 u32 sead::FileDevice::tryIsExistFile(be<u8>*,sead::SafeText*)
 ext 02740FDC sead::FileDevice* sead::FileDevice::tryOpenDirectory(sead::DirectoryHandle*,sead::SafeText*)
 ext 02741088 u32 sead::FileDevice::tryCloseDirectory(sead::DirectoryHandle*)
 ext 02741144 u32 sead::FileDevice::tryReadDirectory(be<u32>*,sead::DirectoryHandle*,sead::DirectoryEntry*,u32)
*/
