/* WWHD sead file layer (first part of the translation unit shared with SEAD#9-13): the per-TU
 * text copies, the archive device, the device base and its handles, the device manager, the main
 * device, the path helpers and the Cafe FS devices' file and directory operations.
 * Reconstructed from this game's instructions only; no external reference was used. */
#include "sead/FileDevice.h"
#include "sead/CafeFileDevices.h"
#include "sead/ArchiveRes.h"

using namespace sead;

/* functions of other units and Cafe imports (bound by the glue block) */
void* allocateObject(u32 size);
void* allocateFromHeap(u32 size, void* heap, s32 alignment);
void* allocateArrayFromHeap(u32 size, void* heap, s32 alignment);
void releaseObject(void* object);
void releaseArray(void* object);
void constructDisposer(sead::Disposer* disposer);
void constructHeapDisposer(sead::Disposer* disposer, void* heap, u32 option);
void destroyDisposer(sead::Disposer* disposer, u32 flags);
void* findContainingHeap(void* heapManager, const void* address);
s32 formatSafeText(sead::SafeTextBuffer* out, const char* format, const be<u8>* first, const be<u8>* second);
void assureCafeTextTermination(sead::SafeText* text);
void* OSBlockMove(void* destination, const void* source, u32 size, u32 flush);
s32 FSGetLastError(void* client);
void FSInit();
s32 FSAddClient(void* client, u32 errorMask);
void FSSetStateChangeNotification(void* client, sead::FSStateChangeParams* params);
s32 SAVEInit();
s32 FSDelClient(void* client, u32 errorMask);
void SAVEShutdown();
void FSShutdown();
void FSInitCmdBlock(void* block);
s32 FSOpenFile(void* client, void* block, const be<u8>* path, const char* mode, be<u32>* handle, u32 errorMask);
s32 FSCloseFile(void* client, void* block, u32 handle, u32 errorMask);
s32 FSReadFile(void* client, void* block, u8* buffer, u32 size, u32 count, u32 handle, u32 flags, u32 errorMask);
s32 FSWriteFile(void* client, void* block, const u8* buffer, u32 size, u32 count, u32 handle, u32 flags, u32 errorMask);
s32 FSSetPosFile(void* client, void* block, u32 handle, u32 position, u32 errorMask);
s32 FSGetStatFile(void* client, void* block, u32 handle, void* stat, u32 errorMask);
s32 FSGetStat(void* client, void* block, const be<u8>* path, void* stat, u32 errorMask);
s32 FSOpenDir(void* client, void* block, const be<u8>* path, be<u32>* handle, u32 errorMask);
s32 FSCloseDir(void* client, void* block, u32 handle, u32 errorMask);
s32 FSReadDir(void* client, void* block, u32 handle, void* entry, u32 errorMask);

struct FSCmdBlock { u8 mData[0xA80]; };
struct FSStat {
    be<u32> mFlags;
    u8 mUnknown04[0xC];
    be<u32> mSize;
    u8 mUnknown14[0x50];
};
struct FSDirEntry {
    FSStat mStat;
    be<u8> mName[0x100];
};
WWHD_OFFSET(FSStat, mSize, 0x10);
WWHD_SIZE(FSStat, 0x64);
WWHD_OFFSET(FSDirEntry, mName, 0x64);
WWHD_SIZE(FSDirEntry, 0x164);

static constexpr u32 kDestroyRelease = 1;
static constexpr u32 kDestroyMember = 3;     // flags the owners pass when they delete a member device
static constexpr u32 kDestroyInstance = 2;
static constexpr u32 kFSErrorNone = 0;
static constexpr s32 kFSStatusEnd = -2;
static constexpr s32 kFSStatusNotFound = -6;
static constexpr u32 kFSStatNotFileMask = 0xC0000000;   // directory and quota flags
static constexpr u32 kFSOpenErrorMask = 0xDA;
static constexpr u32 kFSWriteErrorMask = 0x300;
static constexpr u32 kFSGetStatErrorMask = 0x88;
static constexpr u32 kFSOpenDirErrorMask = 0xEA;
static constexpr u32 kMinimumLoadAlign = 0x40;
static constexpr u32 kLoadSizeRounding = 0x40;

static FileDeviceManagerDisposer* sManagerDisposer;
static void* sHeapManager;
static const char* sOpenModes[3] = {"w", "r+", "w+"};
static DisposerVtable sArchiveDeviceDisposerVtable;
static DisposerVtable sArchiveHandleVtable;
static DisposerVtable sFileHandleVtable;
static FileDeviceVtable sMainDeviceVtable;
static DisposerVtable sMainDeviceDisposerVtable;
static FileDeviceVtable sCafeDeviceVtable;
static DisposerVtable sCafeDeviceDisposerVtable;
static FileDeviceVtable sCafeContentDeviceVtable;
static DisposerVtable sCafeContentDeviceDisposerVtable;
static DisposerVtable sManagerDisposerVtable;
static SafeTextVtable sManagerTextVtable;
static SafeTextVtable sManagerPathVtable;
static SafeTextVtable sManagerDriveNameVtable;
static SafeTextVtable sMainTextVtable;
static SafeTextVtable sMainDriveNameVtable;
static SafeTextVtable sPathTextVtable;
static SafeTextVtable sCafeTextVtable;
static SafeTextVtable sCafePathVtable;
static SafeTextVtable sCafeDriveNameVtable;

/* ---- inline text helpers (GHS inlined the text classes' members into every caller) ---- */

/* the storage of a member object: its own address, or a new block when that address is NULL
 * (GHS member constructors allocate when their object pointer is NULL) */
template <class T>
static T* memberStorage(T* member) {
    if (gabi::ea(member) == 0) member = static_cast<T*>(allocateObject(sizeof(T)));
    return member;
}

template <u32 N>
static void initFixedText(FixedSafeText<N>* text, SafeTextVtable* vtable) {
    text->mStorage[N - 1] = 0;
    text->mText = &text->mStorage[0];
    text->mStorage[0] = 0;
    text->mVtable = vtable;
    text->mCapacity = N;
}

/* length of a text, 0 when it runs past kMaxTextLength */
static s32 textLength(const be<u8>* text) {
    s32 length = 0;
    if (text[0] != 0) {
        do {
            ++length;
            if (length > kMaxTextLength) return 0;
        } while (text[length] != 0);
    }
    return length;
}

static bool textEquals(SafeText* text, SafeText* other) {
    text->assureTermination();
    text->assureTermination();
    be<u8>* left = text->mText;
    other->assureTermination();
    if (left == other->mText) return true;
    const be<u8>* a = text->mText;
    const be<u8>* b = other->mText;
    for (s32 i = 0; i <= kMaxTextLength; ++i) {
        u8 ca = a[i];
        u8 cb = b[i];
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
    return false;
}

static void copyText(SafeTextBuffer* destination, SafeText* source) {
    be<u8>* out = destination->mText;
    source->assureTermination();
    s32 length = textLength(source->mText);
    if (length >= destination->mCapacity) length = destination->mCapacity - 1;
    source->assureTermination();
    OSBlockMove(out, source->mText, length, 0);
    out[length] = 0;
}

static void clearText(SafeTextBuffer* text) {
    if (text->mCapacity > 0) {
        text->mText[0] = 0;
        return;
    }
    text->assureTermination();
    (void)textLength(text->mText);   // GHS kept the length scan of an assertion
}

/* the first n characters of source, or all of it for a negative n */
static void copyTextPrefix(SafeTextBuffer* destination, SafeText* source, s32 n) {
    be<u8>* out = destination->mText;
    if (n < 0) {
        source->assureTermination();
        n = textLength(source->mText);
    }
    if (n >= destination->mCapacity) n = destination->mCapacity - 1;
    source->assureTermination();
    OSBlockMove(out, source->mText, n, 0);
    out[n] = 0;
}

/* overwrite the start of destination with source; the old end stays when it is longer */
static void copyTextAtStart(SafeTextBuffer* destination, SafeText* source) {
    be<u8>* out = destination->mText;
    destination->assureTermination();
    s32 currentLength = textLength(destination->mText);
    source->assureTermination();
    s32 n = textLength(source->mText);
    if (n >= destination->mCapacity) n = destination->mCapacity - 1;
    if (n <= 0) return;
    source->assureTermination();
    OSBlockMove(out, source->mText, n, 0);
    if (n > currentLength) out[n] = 0;
}

/* does the text at part start with pattern (patternLength characters)? */
static bool textStartsWith(SafeText* part, SafeText* pattern, s32 patternLength) {
    part->assureTermination();
    be<u8>* partText = part->mText;
    pattern->assureTermination();
    if (partText == pattern->mText) return true;
    if (patternLength <= 0) return true;
    const be<u8>* expected = pattern->mText;
    const be<u8>* actual = part->mText;
    for (s32 i = 0; i < patternLength; ++i) {
        u8 c = actual[i];
        if (c == 0) return expected[i] == 0;
        u8 e = expected[i];
        if (e == 0 || e != c) return false;
    }
    return true;
}

/* inlined into the path helpers */
static s32 findText(SafeText* text, SafeText* pattern) {
    text->assureTermination();
    s32 length = textLength(text->mText);
    pattern->assureTermination();
    s32 patternLength = textLength(pattern->mText);
    s32 last = length - patternLength;
    path_text::Text part;
    for (s32 i = 0; i <= last; ++i) {
        part.mText = text->mText + i;
        part.mVtable = &sPathTextVtable;
        part.assureTerminationImpl();
        if (textStartsWith(&part, pattern, patternLength)) return i;
    }
    return -1;
}

/* inlined into the path helpers */
static s32 findLastText(SafeText* text, SafeText* pattern) {
    text->assureTermination();
    s32 length = textLength(text->mText);
    pattern->assureTermination();
    s32 patternLength = textLength(pattern->mText);
    path_text::Text part;
    for (s32 i = length - patternLength; i >= 0; --i) {
        part.mText = text->mText + i;
        part.mVtable = &sPathTextVtable;
        part.assureTerminationImpl();
        if (textStartsWith(&part, pattern, patternLength)) return i;
    }
    return -1;
}

/* ================================================================ per-TU text copies of the
 * file-route translation unit (its static initializer stores these vtables) */

/* 027402B4 */
void sead::file_routes::route_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027402C8 */
void sead::file_routes::route_text::Text::assureTerminationImpl() {}

/* 027402CC */
void sead::file_routes::route_text::TextBuffer::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027402E0 */
void sead::file_routes::route_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    text[(u32)mCapacity - 1] = 0;
}

/* 027402F8 */
void sead::file_routes::route_text::FixedTextA::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0274030C */
void sead::file_routes::route_text::DriveName::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02740320 */
void sead::file_routes::route_text::FixedTextB::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02740334 */
void sead::file_routes::route_text::FullPath::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* ================================================================ archive device */

/* 02740348 */
static void destroyArchiveDeviceFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<ArchiveFileDevice*>(reinterpret_cast<u8*>(disposer) - 0x10)->destroy(flags);
}

/* 02740350 */
u8* sead::ArchiveFileDevice::doLoad(LoadArg* arg) {
    ArchiveRes* archive = mArchive;
    if (archive == NULL) return NULL;
    // a caller buffer or heap goes through the generic open/read path
    if (arg->mBuffer != NULL || arg->mHeap != NULL) return FileDevice::doLoad(arg);
    ArchiveFileInfo info;
    info.mSize = 0;
    info.mOffset = 0;
    u8* data = archive->getFile(&arg->mPath, &info);
    if (data == NULL) return NULL;
    arg->mNeedsRelease = 0;
    arg->mReadSize = (u32)info.mSize;
    arg->mRoundedSize = (u32)info.mSize;
    return data;
}

/* 02740420 */
ArchiveFileHandle* sead::ArchiveFileDevice::getArchiveHandle(FileHandle* handle) {
    return reinterpret_cast<ArchiveFileHandle*>(getHandleBackend(handle));
}

/* 02740440 */
FileDevice* sead::ArchiveFileDevice::doOpen(FileHandle* handle, SafeText* path, u32 mode) {
    if (handle == NULL) return NULL;
    if (mArchive == NULL) return NULL;
    path->assureTermination();
    if (path->mText == NULL) return NULL;
    ArchiveFileHandle* archiveHandle = getArchiveHandle(handle);
    u8* data = mArchive->getFile(path, &archiveHandle->mInfo);
    if (data == NULL) return NULL;
    archiveHandle->mData = data;
    archiveHandle->mPosition = 0;
    return this;
}

/* 02740514 */
bool sead::ArchiveFileDevice::doRead(be<u32>* readSize, FileHandle* handle, u8* buffer, u32 size) {
    ArchiveFileHandle* archiveHandle = getArchiveHandle(handle);
    u32 position = archiveHandle->mPosition;
    if (size + position > (u32)archiveHandle->mInfo.mSize) size = archiveHandle->mInfo.mSize - position;
    OSBlockMove(buffer, archiveHandle->mData.get() + position, size, 0);
    archiveHandle->mPosition = archiveHandle->mPosition + size;
    if (readSize != NULL) *readSize = size;
    return true;
}

/* 027405B4 */
bool sead::ArchiveFileDevice::doSeek(FileHandle* handle, s32 offset, u32 origin) {
    ArchiveFileHandle* archiveHandle = getArchiveHandle(handle);
    u32 position = archiveHandle->mPosition;
    switch (origin) {
    case cSeekBegin:
        position = offset;
        break;
    case cSeekCurrent:
        position += offset;
        break;
    case cSeekEnd:
        position = archiveHandle->mInfo.mSize + offset;
        break;
    default:
        return false;
    }
    if (position > (u32)archiveHandle->mInfo.mSize) return false;
    archiveHandle->mPosition = position;
    return true;
}

/* 02740678 */
bool sead::ArchiveFileDevice::doGetFileSize(be<u32>* size, FileHandle* handle) {
    if (handle == NULL) return false;
    if (mArchive == NULL) return false;
    *size = (u32)getArchiveHandle(handle)->mInfo.mSize;
    return true;
}

/* 027406DC */
bool sead::ArchiveFileDevice::doIsExistFile(be<u8>* exists, SafeText* path) {
    if (mArchive == NULL) return false;
    path->assureTermination();
    if (path->mText == NULL) return false;
    u8* data = mArchive->getFile(path, NULL);
    *exists = data != NULL;
    return true;
}

/* 02740794 */
FileDevice* sead::ArchiveFileDevice::doOpenDirectory(DirectoryHandle* handle, SafeText* path) {
    if (handle == NULL) return NULL;
    ArchiveRes* archive = mArchive;
    if (archive == NULL) return NULL;
    u8* backend = getHandleBackend(handle);
    if (gabi::call_ptr<u32>(archive->archiveVtable()->mOpenDirectory.mFunction, archive, backend, path) == 0) return NULL;
    return this;
}

/* 02740838 */
u32 sead::ArchiveFileDevice::doCloseDirectory(DirectoryHandle* handle) {
    ArchiveRes* archive = mArchive;
    if (archive == NULL) return 0;
    u8* backend = getHandleBackend(handle);
    return gabi::call_ptr<u32>(archive->archiveVtable()->mCloseDirectory.mFunction, archive, backend);
}

/* 0274089C */
bool sead::ArchiveFileDevice::doReadDirectory(be<u32>* readCount, DirectoryHandle* handle, DirectoryEntry* entries, u32 count) {
    ArchiveRes* archive = mArchive;
    if (archive == NULL) return false;
    u8* backend = getHandleBackend(handle);
    u32 read = gabi::call_ptr<u32>(archive->archiveVtable()->mReadDirectory.mFunction, archive, backend, entries, count);
    if (readCount != NULL) *readCount = read;
    return true;
}

/* 0274094C */
void sead::ArchiveFileDevice::destroy(u32 flags) {
    if (this == NULL) return;
    mDisposer.mVtable = &sArchiveDeviceDisposerVtable;
    FileDevice::destroy(0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027409AC */
void sead::archive_device_text::FileHandle::destroy(u32 flags) {
    if (this == NULL) return;
    FileDevice* device = mSourceDevice;
    mDisposer.mVtable = &sArchiveHandleVtable;
    if (device != NULL) device->tryClose(this);
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02740A20 */
bool sead::ArchiveFileDevice::doIsAvailable() {
    return true;
}

/* 02740A28 */
bool sead::ArchiveFileDevice::doClose(FileHandle* handle) {
    return true;
}

/* 02740A30 */
bool sead::ArchiveFileDevice::doWrite(be<u32>* written, FileHandle* handle, const u8* buffer, u32 size) {
    return false;
}

/* ================================================================ device base */

/* 02740A38 */
void sead::FileDevice::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sFileDeviceVtable;
    mDisposer.mVtable = &sFileDeviceDisposerVtable;
    if (sFileDeviceManager != NULL) sFileDeviceManager->unmount(this);
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02740ABC */
static void destroyFileDeviceFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<FileDevice*>(reinterpret_cast<u8*>(disposer) - 0x10)->destroy(flags);
}

/* 02740AC4 */
u32 sead::FileDevice::isAvailable() {
    if (mPermission == 0) return 0;
    return gabi::tail_ptr<u32>(mVtable->mIsAvailable.mFunction, this);
}

/* 02740AE8 */
u8* sead::FileDevice::tryLoad(LoadArg* arg) {
    if (mPermission == 0) return NULL;
    return gabi::tail_ptr<u8*>(mVtable->mLoad.mFunction, this, arg);
}

/* 02740B0C */
void sead::FileDevice::setHandleReadChunkSize(FileHandle* handle, u32 size) {
    handle->mReadChunkSize = size;
}

/* 02740B14 */
void sead::FileDevice::setHandleDevice(HandleBase* handle, FileDevice* device) {
    handle->mDevice = device;
}

/* 02740B1C */
void sead::FileDevice::setHandleSourceDevice(HandleBase* handle, FileDevice* device) {
    handle->mSourceDevice = device;
}

/* 02740B24 */
FileDevice* sead::FileDevice::tryOpen(FileHandle* handle, SafeText* path, u32 mode, u32 readChunkSize) {
    if (mPermission == 0 || handle == NULL) return NULL;
    setHandleReadChunkSize(handle, readChunkSize);
    // r7 still holds the chunk size at the virtual call
    FileDevice* device = gabi::call_ptr<FileDevice*>(mVtable->mOpen.mFunction, this, handle, path, mode, readChunkSize);
    setHandleDevice(handle, device);
    if (device != NULL) setHandleSourceDevice(handle, this);
    return device;
}

/* 02740BE0 */
u32 sead::FileDevice::tryClose(FileHandle* handle) {
    if (mPermission == 0 || handle == NULL) return 0;
    if (dispatchIsMatchDevice(handle) == 0) return 0;
    u32 closed = gabi::call_ptr<u32>(mVtable->mClose.mFunction, this, handle);
    if (closed != 0) {
        setHandleDevice(handle, NULL);
        setHandleSourceDevice(handle, NULL);
    }
    return closed;
}

/* 02740C9C */
u32 sead::FileDevice::tryRead(be<u32>* readSize, FileHandle* handle, u8* buffer, u32 size) {
    if (mPermission == 0 || handle == NULL) return 0;
    if (dispatchIsMatchDevice(handle) == 0) return 0;
    if (buffer == NULL) return 0;
    u32 chunkSize = handle->mReadChunkSize;
    if (chunkSize == 0) return gabi::call_ptr<u32>(mVtable->mRead.mFunction, this, readSize, handle, buffer, size);

    gabi::Local<be<u32>> chunkRead;
    u32 total = 0;
    for (;;) {
        *chunkRead = 0;
        if ((s32)size < (s32)chunkSize) chunkSize = size;
        if (gabi::call_ptr<u32>(mVtable->mRead.mFunction, this, chunkRead.get(), handle, buffer, chunkSize) == 0) {
            if (readSize != NULL) *readSize = total;
            return 0;
        }
        u32 read = *chunkRead;
        total += read;
        if (read < chunkSize) break;
        size -= chunkSize;
        buffer += read;
        if (size == 0) break;
        chunkSize = handle->mReadChunkSize;
    }
    if (readSize != NULL) *readSize = total;
    return 1;
}

/* 02740E08 */
u32 sead::FileDevice::tryWrite(be<u32>* written, FileHandle* handle, const u8* buffer, u32 size) {
    if (mPermission == 0 || handle == NULL || buffer == NULL) return 0;
    if (dispatchIsMatchDevice(handle) == 0) return 0;
    return gabi::call_ptr<u32>(mVtable->mWrite.mFunction, this, written, handle, buffer, size);
}

/* 02740EB8 */
u32 sead::FileDevice::trySeek(FileHandle* handle, s32 offset, u32 origin) {
    if (mPermission == 0 || handle == NULL) return 0;
    if (dispatchIsMatchDevice(handle) == 0) return 0;
    return gabi::call_ptr<u32>(mVtable->mSeek.mFunction, this, handle, offset, origin);
}

/* 02740F7C */
u32 sead::FileDevice::tryGetFileSize(be<u32>* size, FileHandle* handle) {
    if (mPermission == 0 || handle == NULL || size == NULL) return 0;
    return gabi::tail_ptr<u32>(mVtable->mGetFileSize.mFunction, this, size, handle);
}

/* 02740FB0 */
u32 sead::FileDevice::tryIsExistFile(be<u8>* exists, SafeText* path) {
    if (mPermission == 0 || exists == NULL) return 0;
    return gabi::tail_ptr<u32>(mVtable->mIsExistFile.mFunction, this, exists, path);
}

/* 02740FDC */
FileDevice* sead::FileDevice::tryOpenDirectory(DirectoryHandle* handle, SafeText* path) {
    if (mPermission == 0 || handle == NULL) return NULL;
    FileDevice* device = gabi::call_ptr<FileDevice*>(mVtable->mOpenDirectory.mFunction, this, handle, path);
    setHandleDevice(handle, device);
    if (device != NULL) setHandleSourceDevice(handle, this);
    return device;
}

/* 02741088 */
u32 sead::FileDevice::tryCloseDirectory(DirectoryHandle* handle) {
    if (mPermission == 0 || handle == NULL) return 0;
    if (dispatchIsMatchDevice(handle) == 0) return 0;
    u32 closed = gabi::call_ptr<u32>(mVtable->mCloseDirectory.mFunction, this, handle);
    if (closed != 0) {
        setHandleDevice(handle, NULL);
        setHandleSourceDevice(handle, NULL);
    }
    return closed;
}

/* 02741144 */
u32 sead::FileDevice::tryReadDirectory(be<u32>* readCount, DirectoryHandle* handle, DirectoryEntry* entries, u32 count) {
    if (mPermission == 0 || handle == NULL) return 0;
    if (dispatchIsMatchDevice(handle) == 0) return 0;
    gabi::Local<be<u32>> read;
    *read = 0;
    u32 result = gabi::call_ptr<u32>(mVtable->mReadDirectory.mFunction, this, read.get(), handle, entries, count);
    u32 n = *read;
    if (readCount != NULL) *readCount = n;
    if (n > count) return 0;
    return result;
}

/* 0274120C */
u32 sead::FileDevice::isMatchDevice(HandleBase* handle) {
    return handle->mDevice == this;
}

/* 02741220 */
u8* sead::FileDevice::doLoad(LoadArg* arg) {
    if (arg->mBuffer != NULL && arg->mBufferSize == 0) return NULL;

    FileHandle handle;
    constructDisposer(&handle.mDisposer);
    handle.mDevice = NULL;
    handle.mSourceDevice = NULL;
    handle.mDisposer.mVtable = &sFileHandleVtable;
    handle.mReadChunkSize = 0;

    u8* buffer = NULL;
    if (tryOpen(&handle, &arg->mPath, 0, arg->mReadChunkSize) != NULL) {
        gabi::Local<be<u32>> fileSize;
        *fileSize = 0;
        if (tryGetFileSize(fileSize.get(), &handle) != 0 && *fileSize != 0) {
            u32 bufferSize = arg->mBufferSize;
            bool sizeOk = true;
            if (bufferSize == 0) {
                bufferSize = (*fileSize + kLoadSizeRounding - 1) & ~(kLoadSizeRounding - 1);
            } else if (bufferSize < *fileSize) {
                sizeOk = false;
            }
            if (sizeOk) {
                buffer = arg->mBuffer;
                bool allocated = false;
                if (buffer == NULL) {
                    s32 alignment = arg->mAlignment;
                    s32 sign = alignment < 0 ? -1 : 1;
                    u32 magnitude = alignment < 0 ? 0u - (u32)alignment : (u32)alignment;
                    if ((s32)magnitude < (s32)kMinimumLoadAlign) magnitude = kMinimumLoadAlign;
                    buffer = static_cast<u8*>(allocateArrayFromHeap(bufferSize, arg->mHeap, (s32)((u32)sign * magnitude)));
                    allocated = true;
                }
                gabi::Local<be<u32>> readSize;
                *readSize = 0;
                if (tryRead(readSize.get(), &handle, buffer, bufferSize) != 0 && tryClose(&handle) != 0) {
                    FileDevice* original = handle.mSourceDevice;
                    handle.mDisposer.mVtable = &sFileHandleVtable;
                    arg->mRoundedSize = bufferSize;
                    arg->mNeedsRelease = allocated;
                    arg->mReadSize = (u32)*readSize;
                    if (original != NULL) original->tryClose(&handle);
                    destroyDisposer(&handle.mDisposer, 0);
                    return buffer;
                }
                if (allocated) releaseArray(buffer);
            }
        }
    }
    // the handle's destructor
    FileDevice* original = handle.mSourceDevice;
    handle.mDisposer.mVtable = &sFileHandleVtable;
    if (original != NULL) original->tryClose(&handle);
    destroyDisposer(&handle.mDisposer, 0);
    return NULL;
}

/* 02741428 */
u8* sead::FileDevice::getHandleBackend(HandleBase* handle) {
    return handle->mBackend;
}

/* ---- handles ---- */

/* 02741430 */
u32 sead::FileHandle::close() {
    FileDevice* device = mSourceDevice;
    if (device == NULL) return 0;
    return device->tryClose(this);
}

/* 02741450 */
u32 sead::FileHandle::tryClose() {
    FileDevice* device = mSourceDevice;
    if (device == NULL) return 0;
    return device->tryClose(this);
}

/* 0274146C */
u32 sead::FileHandle::read(void* buffer, u32 size) {
    FileDevice* device = mDevice;
    if (device == NULL) return 0;
    gabi::Local<be<u32>> readSize;
    *readSize = 0;
    device->tryRead(readSize.get(), this, static_cast<u8*>(buffer), size);
    return *readSize;
}

/* 027414CC */
u32 sead::FileHandle::tryRead(be<u32>* readSize, void* buffer, u32 size) {
    FileDevice* device = mDevice;
    if (device == NULL) return 0;
    return device->tryRead(readSize, this, static_cast<u8*>(buffer), size);
}

/* 027414F8 */
u32 sead::FileHandle::write(const void* buffer, u32 size) {
    FileDevice* device = mDevice;
    if (device == NULL) return 0;
    gabi::Local<be<u32>> written;
    *written = 0;
    device->tryWrite(written.get(), this, static_cast<const u8*>(buffer), size);
    return *written;
}

/* 02741558 */
u32 sead::FileHandle::tryWrite(be<u32>* written, const void* buffer, u32 size) {
    FileDevice* device = mDevice;
    if (device == NULL) return 0;
    return device->tryWrite(written, this, static_cast<const u8*>(buffer), size);
}

/* 02741584 */
u32 sead::FileHandle::seek(s32 offset, u32 origin) {
    FileDevice* device = mDevice;
    if (device == NULL) return 0;
    return device->trySeek(this, offset, origin);
}

/* 027415B0 */
u32 sead::FileHandle::getFileSize() {
    FileDevice* device = mDevice;
    if (device == NULL) return 0;
    gabi::Local<be<u32>> size;
    *size = 0;
    device->tryGetFileSize(size.get(), this);
    return *size;
}

/* 02741604 */
u32 sead::DirectoryHandle::read(DirectoryEntry* entries, u32 count) {
    FileDevice* device = mDevice;
    if (device == NULL) return 0;
    gabi::Local<be<u32>> readCount;
    *readCount = 0;
    device->tryReadDirectory(readCount.get(), this, entries, count);
    return *readCount;
}

/* ---- per-TU text copies of the device base ---- */

/* 02741664 */
void sead::file_device_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02741678 */
void sead::file_device_text::Text::assureTerminationImpl() {}

/* 0274167C */
void sead::file_device_text::TextBuffer::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02741690 */
void sead::file_device_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    text[(u32)mCapacity - 1] = 0;
}

/* 027416A8 */
void sead::file_device_text::FixedTextA::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027416BC */
void sead::file_device_text::FixedTextC::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027416D0 */
void sead::file_device_text::FileHandle::destroy(u32 flags) {
    if (this == NULL) return;
    FileDevice* device = mSourceDevice;
    mDisposer.mVtable = &sFileHandleVtable;
    if (device != NULL) device->tryClose(this);
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02741744 */
void sead::file_device_text::FixedTextB::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02741758 */
void sead::file_device_text::FixedTextD::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* ================================================================ device manager */

/* 0274176C */
static s32 handleFSStateChange(void* client, u32 state, void* context) {
    return FSGetLastError(client);
}

/* 02741770 */
void sead::FileDeviceManager::mount(FileDevice* device, SafeText* driveName) {
    if (!textEquals(driveName, &sFileDeviceEmptyText)) copyText(&device->mDriveName, driveName);
    LinkList* list = device->mListNode.mList;
    if (list != NULL) {
        device->mListNode.mList = NULL;
        device->mListNode.mLink.detach();
        list->mCount = list->mCount - 1;
    }
    device->mListNode.mList = &mDevices;
    mDevices.insertBefore(&device->mListNode.mLink);
    mDevices.mCount = mDevices.mCount + 1;
}

/* 02741900 */
FileDeviceManager* sead::FileDeviceManager::construct(FileDeviceManager* manager) {
    if (manager == NULL) {
        manager = static_cast<FileDeviceManager*>(allocateObject(sizeof(FileDeviceManager)));
        if (manager == NULL) return NULL;
    }
    LinkList* devices = memberStorage(&manager->mDevices);
    if (devices != NULL) {
        devices->mNext = devices;
        devices->mCount = 0;
        devices->mPrevious = devices;
    }
    manager->mMainDevice = NULL;
    manager->mDefaultDevice = NULL;
    if (sHeapManager == NULL) return manager;

    void* heap = findContainingHeap(sHeapManager, manager);
    FSInit();
    FSAddClient(manager->mClient, kFSErrorNone);
    FSStateChangeParams params;
    params.mContext = NULL;
    params.mQueue = NULL;
    params.mCallback = gabi::guest_fn(reinterpret_cast<const void*>(&handleFSStateChange));
    FSSetStateChangeNotification(manager->mClient, &params);
    SAVEInit();
    manager->mScratchText[0] = 0;
    manager->mScratchState = 0;

    MainFileDevice* mainDevice = static_cast<MainFileDevice*>(allocateFromHeap(sizeof(MainFileDevice), heap, 4));
    if (mainDevice != NULL) mainDevice = MainFileDevice::construct(mainDevice, heap);
    manager->mMainDevice = mainDevice;
    manager->mount(mainDevice, &sFileDeviceEmptyText);
    manager->mDefaultDevice = manager->mMainDevice;
    return manager;
}

/* 02741A20 */
FileDeviceManager* sead::FileDeviceManager::createInstance(void* heap) {
    if (sFileDeviceManager != NULL) return sFileDeviceManager;
    FileDeviceManager* manager = static_cast<FileDeviceManager*>(allocateArrayFromHeap(sizeof(FileDeviceManager), heap, 4));
    if (manager != NULL) {
        constructHeapDisposer(&manager->mInstanceDisposer, heap, 3);
        manager->mInstanceDisposer.mVtable = &sManagerDisposerVtable;
    }
    sManagerDisposer = reinterpret_cast<FileDeviceManagerDisposer*>(manager);
    if (manager != NULL) manager = construct(manager);
    sFileDeviceManager = manager;
    return manager;
}

/* 02741AB4 */
void sead::FileDeviceManager::destroy(u32 flags) {
    if (this == NULL) return;
    MainFileDevice* mainDevice = mMainDevice;
    if (mainDevice != NULL) {
        mainDevice->dispatchDestroy(kDestroyMember);
        mMainDevice = NULL;
    }
    FSDelClient(mClient, kFSErrorNone);
    SAVEShutdown();
    FSShutdown();
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02741B38 */
FileDevice* sead::FileDeviceManager::findDeviceFromDriveName(SafeText* driveName) {
    for (Link* link = mDevices.mNext; link != &mDevices; link = link->mNext) {
        DeviceListNode* node = reinterpret_cast<DeviceListNode*>(link);
        FileDevice* device = node->mDevice;
        if (textEquals(&device->mDriveName, driveName)) return node->mDevice;
    }
    return NULL;
}

/* 02741C24 */
FileDevice* sead::FileDeviceManager::findDevice(SafeText* path, SafeTextBuffer* pathWithoutDrive) {
    DriveNameText driveName;
    initFixedText(&driveName, &sManagerDriveNameVtable);
    FileDevice* device;
    if (path::getDriveName(&driveName, path)) {
        device = findDeviceFromDriveName(&driveName);
    } else {
        device = mDefaultDevice;
        if (device == NULL) return NULL;
    }
    if (pathWithoutDrive != NULL) path::getPathWithoutDrive(pathWithoutDrive, path);
    return device;
}

/* 02741CFC */
FileDevice* sead::FileDeviceManager::tryOpen(FileHandle* handle, SafeText* path, u32 mode, u32 readChunkSize) {
    PathText pathWithoutDrive;
    initFixedText(&pathWithoutDrive, &sManagerPathVtable);
    FileDevice* device = findDevice(path, &pathWithoutDrive);
    if (device == NULL) return NULL;
    return device->tryOpen(handle, &pathWithoutDrive, mode, readChunkSize);
}

/* 02741DAC */
u8* sead::FileDeviceManager::tryLoad(LoadArg* arg) {
    // the original's frame layout: the device's request points into the path buffer next to it
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<LoadArg> deviceArg(0x8);
    gabi::FrameLocal<PathText> pathWithoutDrive(0x30);
    initFixedText(pathWithoutDrive.get(), &sManagerPathVtable);
    FileDevice* device = findDevice(&arg->mPath, pathWithoutDrive.get());
    if (device == NULL) return NULL;

    deviceArg->mBuffer = arg->mBuffer;
    deviceArg->mBufferSize = (u32)arg->mBufferSize;
    deviceArg->mReadSize = (u32)arg->mReadSize;
    deviceArg->mNeedsRelease = (u8)arg->mNeedsRelease;
    deviceArg->mHeap = arg->mHeap;
    deviceArg->mReadChunkSize = (u32)arg->mReadChunkSize;
    deviceArg->mPath.mText = arg->mPath.mText;
    deviceArg->mAlignment = (s32)arg->mAlignment;
    deviceArg->mRoundedSize = (u32)arg->mRoundedSize;
    deviceArg->mPath.mVtable = &sManagerTextVtable;
    pathWithoutDrive->assureTermination();
    deviceArg->mPath.mText = pathWithoutDrive->mText;

    u8* data = device->tryLoad(deviceArg.get());
    arg->mNeedsRelease = (u8)deviceArg->mNeedsRelease;
    arg->mReadSize = (u32)deviceArg->mReadSize;
    arg->mRoundedSize = (u32)deviceArg->mRoundedSize;
    return data;
}

/* 02741ED0 */
void sead::FileDeviceManager::unmount(FileDevice* device) {
    if (device->mListNode.mList != NULL) {
        device->mListNode.mList = NULL;
        device->mListNode.mLink.detach();
        mDevices.mCount = mDevices.mCount - 1;
    }
    if (device == mDefaultDevice) mDefaultDevice = NULL;
}

/* 02741F44 */
void sead::FileDeviceManagerDisposer::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sManagerDisposerVtable;
    if (this == sManagerDisposer) {
        FileDeviceManager* manager = sFileDeviceManager;
        sManagerDisposer = NULL;
        manager->destroy(kDestroyInstance);
        sFileDeviceManager = NULL;
    }
    destroyDisposer(this, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* ---- per-TU text copies of the device manager ---- */

/* 02741FE0 */
void sead::device_manager_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02741FF4 */
void sead::device_manager_text::Text::assureTerminationImpl() {}

/* 02741FF8 */
void sead::device_manager_text::TextBuffer::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0274200C */
void sead::device_manager_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    text[(u32)mCapacity - 1] = 0;
}

/* 02742024 */
void sead::device_manager_text::FixedTextA::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02742038 */
void sead::device_manager_text::PathBuffer::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0274204C */
void sead::device_manager_text::FixedTextB::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02742060 */
void sead::device_manager_text::DriveName::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* ================================================================ main device */

/* 02742074 */
void sead::MainFileDevice::destroy(u32 flags) {
    if (this == NULL) return;
    CafeContentFileDevice* device = mDevice;
    mVtable = &sMainDeviceVtable;
    mDisposer.mVtable = &sMainDeviceDisposerVtable;
    if (device != NULL) {
        device->dispatchDestroy(kDestroyMember);
        mDevice = NULL;
    }
    FileDevice::destroy(0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02742108 */
static void destroyMainDeviceFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<MainFileDevice*>(reinterpret_cast<u8*>(disposer) - 0x10)->destroy(flags);
}

/* inlined into MainFileDevice::construct and CafeFileDevice::construct */
static void constructFileDeviceBase(FileDevice* device, SafeTextVtable* driveNameVtable) {
    DeviceListNode* node = memberStorage(&device->mListNode);
    if (node != NULL) {
        node->mLink.mNext = NULL;
        node->mDevice = device;
        node->mLink.mPrevious = NULL;
        node->mList = NULL;
    }
    constructDisposer(&device->mDisposer);
    device->mVtable = &sFileDeviceVtable;
    device->mDisposer.mVtable = &sFileDeviceDisposerVtable;
    FixedSafeText<32>* driveName = memberStorage(&device->mDriveName);
    if (driveName != NULL) initFixedText(driveName, driveNameVtable);
    device->mPermission = 1;
}

/* 02742110 */
MainFileDevice* sead::MainFileDevice::construct(MainFileDevice* device, void* heap) {
    if (device == NULL) {
        device = static_cast<MainFileDevice*>(allocateObject(sizeof(MainFileDevice)));
        if (device == NULL) return NULL;
    }
    SafeText name;
    name.mVtable = &sMainTextVtable;
    name.mText.v = gabi::guest_str("main");
    constructFileDeviceBase(device, &sMainDriveNameVtable);
    copyText(&device->mDriveName, &name);
    device->mDevice = NULL;
    device->mVtable = &sMainDeviceVtable;
    device->mDisposer.mVtable = &sMainDeviceDisposerVtable;
    CafeContentFileDevice* content = static_cast<CafeContentFileDevice*>(allocateFromHeap(sizeof(CafeContentFileDevice), heap, 4));
    if (content != NULL) content = CafeContentFileDevice::construct(content);
    device->mDevice = content;
    return device;
}

/* ---- per-TU text copies of the main device ---- */

/* 027422C8 */
void sead::main_device_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027422DC */
void sead::main_device_text::Text::assureTerminationImpl() {}

/* 027422E0 */
void sead::main_device_text::TextBuffer::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027422F4 */
void sead::main_device_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    text[(u32)mCapacity - 1] = 0;
}

/* 0274230C */
void sead::main_device_text::FixedTextA::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02742320 */
void sead::main_device_text::DriveName::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* ---- main device forwarding to its content device ---- */

/* 02742334 */
u32 sead::MainFileDevice::isAvailable() {
    return mDevice->isAvailable();
}

/* 0274233C */
FileDevice* sead::MainFileDevice::doOpen(FileHandle* handle, SafeText* path, u32 mode) {
    u32 readChunkSize = handle->mReadChunkSize;
    return mDevice->tryOpen(handle, path, mode, readChunkSize);
}

/* 02742348 */
u32 sead::MainFileDevice::doClose(FileHandle* handle) {
    return mDevice->tryClose(handle);
}

/* 02742350 */
u32 sead::MainFileDevice::doRead(be<u32>* readSize, FileHandle* handle, u8* buffer, u32 size) {
    return mDevice->tryRead(readSize, handle, buffer, size);
}

/* 02742358 */
u32 sead::MainFileDevice::doWrite(be<u32>* written, FileHandle* handle, const u8* buffer, u32 size) {
    return mDevice->tryWrite(written, handle, buffer, size);
}

/* 02742360 */
u32 sead::MainFileDevice::doSeek(FileHandle* handle, s32 offset, u32 origin) {
    return mDevice->trySeek(handle, offset, origin);
}

/* 02742368 */
u32 sead::MainFileDevice::doGetFileSize(be<u32>* size, FileHandle* handle) {
    return mDevice->tryGetFileSize(size, handle);
}

/* 02742370 */
u32 sead::MainFileDevice::doIsExistFile(be<u8>* exists, SafeText* path) {
    return mDevice->tryIsExistFile(exists, path);
}

/* 02742378 */
FileDevice* sead::MainFileDevice::doOpenDirectory(DirectoryHandle* handle, SafeText* path) {
    return mDevice->tryOpenDirectory(handle, path);
}

/* 02742380 */
u32 sead::MainFileDevice::doCloseDirectory(DirectoryHandle* handle) {
    return mDevice->tryCloseDirectory(handle);
}

/* 02742388 */
u32 sead::MainFileDevice::doReadDirectory(be<u32>* readCount, DirectoryHandle* handle, DirectoryEntry* entries, u32 count) {
    return mDevice->tryReadDirectory(readCount, handle, entries, count);
}

/* 02742390 */
u32 sead::MainFileDevice::isMatchDevice(HandleBase* handle) {
    FileDevice* device = mDevice;
    return gabi::tail_ptr<u32>(device->mVtable->mIsMatchDevice.mFunction, device, handle);
}

/* ================================================================ path helpers */

/* 027423A4 */
bool sead::path::getDriveName(SafeTextBuffer* out, SafeText* path) {
    clearText(out);
    SafeText separator;
    separator.mVtable = &sPathTextVtable;
    separator.mText.v = gabi::guest_str(":");
    s32 index = findText(path, &separator);
    if (index == -1) return false;
    copyTextPrefix(out, path, index);
    return true;
}

/* 027426B0 */
bool sead::path::getExtension(SafeTextBuffer* out, SafeText* path) {
    clearText(out);
    SafeText dot;
    dot.mVtable = &sPathTextVtable;
    dot.mText.v = gabi::guest_str(".");
    s32 index = findLastText(path, &dot);
    if (index < 0) return false;

    // no extension when a directory separator follows the last dot
    path->assureTermination();
    be<u8>* text = path->mText;
    s32 length = textLength(text);
    path_text::Text suffix;
    suffix.mVtable = &sPathTextVtable;
    suffix.mText = index > length ? sFileDeviceEmptyText.mText.get() : text + index;
    suffix.assureTerminationImpl();
    const be<u8>* scan = suffix.mText;
    for (s32 i = 0; i <= kMaxTextLength; ++i) {
        u8 c = scan[i];
        if (c == 0) break;
        if (c == '/') return false;
    }

    path->assureTermination();
    text = path->mText;
    length = textLength(text);
    s32 start = index + 1;
    path_text::Text extension;
    if (start < 0 || start > length) extension.mText = sFileDeviceEmptyText.mText;
    else extension.mText = text + start;
    be<u8>* destination = out->mText;
    extension.mVtable = &sPathTextVtable;
    extension.assureTerminationImpl();
    s32 n = textLength(extension.mText);
    if (n >= out->mCapacity) n = out->mCapacity - 1;
    extension.assureTermination();
    OSBlockMove(destination, extension.mText, n, 0);
    destination[n] = 0;
    return true;
}

/* 02742B8C */
void sead::path::getPathWithoutDrive(SafeTextBuffer* out, SafeText* path) {
    clearText(out);
    SafeText separator;
    separator.mVtable = &sPathTextVtable;
    separator.mText.v = gabi::guest_str("://");
    s32 index = findText(path, &separator);
    if (index == -1) {
        copyTextAtStart(out, path);
        return;
    }
    index += 3;
    path->assureTermination();
    be<u8>* text = path->mText;
    s32 length = textLength(text);
    SafeText rest;
    rest.mVtable = &sPathTextVtable;
    if (index < 0 || index > length) rest.mText = sFileDeviceEmptyText.mText;
    else rest.mText = text + index;
    copyTextAtStart(out, &rest);
}

/* 027430F8 */
void sead::path_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0274310C */
void sead::path_text::Text::assureTerminationImpl() {}

/* ================================================================ Cafe FS devices */

/* 02743110 */
static void destroyCafeDeviceFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<CafeFileDevice*>(reinterpret_cast<u8*>(disposer) - 0x10)->destroy(flags);
}

/* 02743118 */
static void destroyCafeContentDeviceFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<CafeContentFileDevice*>(reinterpret_cast<u8*>(disposer) - 0x10)->destroy(flags);
}

/* 02743120 */
static void destroyCafeHostDeviceFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<CafeHostFileDevice*>(reinterpret_cast<u8*>(disposer) - 0x10)->destroy(flags);
}

/* 02743128 */
CafeFileDevice* sead::CafeFileDevice::construct(CafeFileDevice* device, SafeText* driveName, SafeText* mountPoint) {
    if (device == NULL) {
        device = static_cast<CafeFileDevice*>(allocateObject(sizeof(CafeFileDevice)));
        if (device == NULL) return NULL;
    }
    constructFileDeviceBase(device, &sCafeDriveNameVtable);
    copyText(&device->mDriveName, driveName);
    device->mVtable = &sCafeDeviceVtable;
    device->mDisposer.mVtable = &sCafeDeviceDisposerVtable;
    mountPoint->assureTermination();
    be<u8>* mountText = mountPoint->mText;
    device->mClient = NULL;
    device->mOpenErrorMask = kFSOpenErrorMask;
    device->mReadErrorMask = 0;
    device->mMountPoint = mountText;
    device->mCloseErrorMask = 0;
    device->mLastStatus = 0;
    return device;
}

/* 027432DC */
void* sead::CafeFileDevice::getClient() {
    void* client = mClient;
    if (client != NULL) return client;
    return sFileDeviceManager->mClient;
}

/* 027432F8 */
CafeFileHandle* sead::CafeFileDevice::getCafeHandle(FileHandle* handle) {
    return reinterpret_cast<CafeFileHandle*>(getHandleBackend(handle));
}

/* 02743318 */
FileDevice* sead::CafeFileDevice::doOpen(FileHandle* handle, SafeText* path, u32 mode) {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeFileHandle* cafeHandle = getCafeHandle(handle);
    const char* modeText = mode >= 1 && mode <= 3 ? (const char*)sOpenModes[mode - 1] : "r";
    PathText fsPath;
    initFixedText(&fsPath, &sCafePathVtable);
    dispatchFormatPath(&fsPath, path);
    fsPath.assureTermination();
    s32 status = FSOpenFile(client, block.get(), fsPath.mText, modeText, &cafeHandle->mFSHandle, mOpenErrorMask);
    cafeHandle->mPosition = 0;
    mLastStatus = status;
    if (status != 0) {
        cafeHandle->mFSHandle = 0;
        return NULL;
    }
    return this;
}

/* 02743468 */
bool sead::CafeFileDevice::doClose(FileHandle* handle) {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeFileHandle* cafeHandle = getCafeHandle(handle);
    return FSCloseFile(client, block.get(), cafeHandle->mFSHandle, mCloseErrorMask) == 0;
}

/* 027434E0 */
bool sead::CafeFileDevice::doRead(be<u32>* readSize, FileHandle* handle, u8* buffer, u32 size) {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeFileHandle* cafeHandle = getCafeHandle(handle);
    s32 result = FSReadFile(client, block.get(), buffer, 1, size, cafeHandle->mFSHandle, 0, mReadErrorMask);
    if (result < 0) {
        mLastStatus = result;
        return false;
    }
    mLastStatus = 0;
    cafeHandle->mPosition = cafeHandle->mPosition + result;
    if (readSize != NULL) *readSize = result;
    return true;
}

/* 027435A8 */
bool sead::CafeFileDevice::doWrite(be<u32>* written, FileHandle* handle, const u8* buffer, u32 size) {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeFileHandle* cafeHandle = getCafeHandle(handle);
    s32 result = FSWriteFile(client, block.get(), buffer, 1, size, cafeHandle->mFSHandle, 0, kFSWriteErrorMask);
    if (result < 0) {
        mLastStatus = result;
        return false;
    }
    mLastStatus = 0;
    cafeHandle->mPosition = cafeHandle->mPosition + result;
    if (written != NULL) *written = result;
    return true;
}

/* 02743670 */
bool sead::CafeFileDevice::doSeek(FileHandle* handle, s32 offset, u32 origin) {
    gabi::Local<be<u32>> fileSize;
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeFileHandle* cafeHandle = getCafeHandle(handle);
    u32 position = offset;
    switch (origin) {
    case cSeekBegin:
        break;
    case cSeekCurrent:
        position += cafeHandle->mPosition;
        break;
    case cSeekEnd:
        *fileSize = 0;
        if (gabi::call_ptr<u32>(mVtable->mGetFileSize.mFunction, this, fileSize.get(), handle) == 0) return false;
        position += *fileSize;
        break;
    default:
        return false;
    }
    s32 status = FSSetPosFile(client, block.get(), cafeHandle->mFSHandle, position, 0);
    mLastStatus = status;
    if (status != 0) return false;
    cafeHandle->mPosition = position;
    return true;
}

/* 0274378C */
bool sead::CafeFileDevice::doGetFileSize(be<u32>* size, FileHandle* handle) {
    gabi::Local<FSStat> stat;
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeFileHandle* cafeHandle = getCafeHandle(handle);
    s32 status = FSGetStatFile(client, block.get(), cafeHandle->mFSHandle, stat.get(), 0);
    mLastStatus = status;
    if (status != 0) return false;
    *size = (u32)stat->mSize;
    return true;
}

/* 02743848 */
bool sead::CafeFileDevice::doIsExistFile(be<u8>* exists, SafeText* path) {
    gabi::Local<FSStat> stat;
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    PathText fsPath;
    initFixedText(&fsPath, &sCafePathVtable);
    dispatchFormatPath(&fsPath, path);
    fsPath.assureTermination();
    s32 status = FSGetStat(client, block.get(), fsPath.mText, stat.get(), kFSGetStatErrorMask);
    mLastStatus = status;
    if (status == 0) {
        *exists = ((u32)stat->mFlags & kFSStatNotFileMask) == 0;
    } else if (status == kFSStatusNotFound) {
        *exists = 0;
    } else {
        return false;
    }
    return true;
}

/* 02743970 */
CafeDirectoryHandle* sead::CafeFileDevice::getCafeDirectoryHandle(DirectoryHandle* handle) {
    return reinterpret_cast<CafeDirectoryHandle*>(getHandleBackend(handle));
}

/* 02743990 */
FileDevice* sead::CafeFileDevice::doOpenDirectory(DirectoryHandle* handle, SafeText* path) {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeDirectoryHandle* cafeHandle = getCafeDirectoryHandle(handle);
    PathText fsPath;
    initFixedText(&fsPath, &sCafePathVtable);
    dispatchFormatPath(&fsPath, path);
    fsPath.assureTermination();
    s32 status = FSOpenDir(client, block.get(), fsPath.mText, &cafeHandle->mFSHandle, kFSOpenDirErrorMask);
    mLastStatus = status;
    if (status != 0) return NULL;
    return this;
}

/* 02743AA0 */
bool sead::CafeFileDevice::doCloseDirectory(DirectoryHandle* handle) {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeDirectoryHandle* cafeHandle = getCafeDirectoryHandle(handle);
    s32 status = FSCloseDir(client, block.get(), cafeHandle->mFSHandle, 0);
    mLastStatus = status;
    return status == 0;
}

/* 02743B1C */
bool sead::CafeFileDevice::doReadDirectory(be<u32>* readCount, DirectoryHandle* handle, DirectoryEntry* entries, u32 count) {
    // the original's frame layout: an unterminated entry name is scanned on into the command block
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SafeText> name(0x8);
    gabi::FrameLocal<FSDirEntry> entry(0x10);
    gabi::FrameLocal<FSCmdBlock> block(0x174);
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeDirectoryHandle* cafeHandle = getCafeDirectoryHandle(handle);
    u32 read = 0;
    for (u32 remaining = count; remaining != 0; --remaining) {
        s32 status = FSReadDir(client, block.get(), cafeHandle->mFSHandle, entry.get(), 0);
        mLastStatus = status;
        if (status != 0) {
            if (readCount != NULL) *readCount = read;
            return status == kFSStatusEnd;
        }
        // entries->mName = entry.mName
        name->mText = &entry->mName[0];
        name->mVtable = &sCafeTextVtable;
        be<u8>* destination = entries->mName.mText;
        assureCafeTextTermination(name.get());
        s32 n = textLength(name->mText);
        if (n >= entries->mName.mCapacity) n = entries->mName.mCapacity - 1;
        name->assureTermination();
        OSBlockMove(destination, name->mText, n, 0);
        destination[n] = 0;
        ++read;
        entries->mIsDirectory = (u32)entry->mStat.mFlags >> 31;
        ++entries;
    }
    if (readCount != NULL) *readCount = count;
    return true;
}

/* 02743CA0 */
void sead::CafeFileDevice::formatPath(SafeTextBuffer* out, SafeText* path) {
    path->assureTermination();
    be<u8>* text = path->mText;
    formatSafeText(out, "%s/%s", mMountPoint.get(), text);
}

/* 02743D10 */
CafeContentFileDevice* sead::CafeContentFileDevice::construct(CafeContentFileDevice* device) {
    if (device == NULL) {
        device = static_cast<CafeContentFileDevice*>(allocateObject(sizeof(CafeContentFileDevice)));
        if (device == NULL) return NULL;
    }
    SafeText driveName;
    SafeText mountPoint;
    driveName.mVtable = &sCafeTextVtable;
    mountPoint.mVtable = &sCafeTextVtable;
    driveName.mText.v = gabi::guest_str("content");
    mountPoint.mText.v = gabi::guest_str("/vol/content");
    CafeFileDevice::construct(device, &driveName, &mountPoint);
    device->mVtable = &sCafeContentDeviceVtable;
    device->mDisposer.mVtable = &sCafeContentDeviceDisposerVtable;
    return device;
}

/* WWHD glue
 str 10143D78
 str 10143E88
 str 10143E8A
 str 10143E8C
 str 10144038
 str 10144050
 str 10144058
 str 10144060
 data sManagerDisposer 101F8B0C
 data sHeapManager 101F8B4C
 data sOpenModes 1014403C
 data sArchiveDeviceDisposerVtable 10143AA4 nocheck
 data sArchiveHandleVtable 10143994 nocheck
 data sFileHandleVtable 10143AF4 nocheck
 data sMainDeviceVtable 10143D80 nocheck
 data sMainDeviceDisposerVtable 10143E60 nocheck
 data sCafeDeviceVtable 10144080 nocheck
 data sCafeDeviceDisposerVtable 10144168 nocheck
 data sCafeContentDeviceVtable 10143F50 nocheck
 data sCafeContentDeviceDisposerVtable 10143F30 nocheck
 data sManagerDisposerVtable 10143CF8 nocheck
 data sManagerTextVtable 10143C68 nocheck
 data sManagerPathVtable 10143CB0 nocheck
 data sManagerDriveNameVtable 10143CE0 nocheck
 data sMainTextVtable 10143D18 nocheck
 data sMainDriveNameVtable 10143D60 nocheck
 data sPathTextVtable 10143E70 nocheck
 data sCafeTextVtable 10143EA0 nocheck
 data sCafePathVtable 10143EE8 nocheck
 data sCafeDriveNameVtable 10143F18 nocheck
 ext 0273AD10 void* allocateObject(u32)
 ext 0273B050 void* allocateFromHeap(u32, void*, s32)
 ext 0273B0D4 void* allocateArrayFromHeap(u32, void*, s32)
 ext 0273AF40 void releaseObject(void*)
 ext 0273AFC8 void releaseArray(void*)
 ext 02752A84 void constructDisposer(sead::Disposer*)
 ext 02752B0C void constructHeapDisposer(sead::Disposer*, void*, u32)
 ext 02752BEC void destroyDisposer(sead::Disposer*, u32)
 ext 02755FEC void* findContainingHeap(void*, const void*)
 ext 0273B2F0 sead::Link* sead::Link::insertBefore(sead::Link*)
 ext 0273B310 sead::Link* sead::Link::detach()
 ext 02759C28 s32 formatSafeText(sead::SafeTextBuffer*, const char*, const be<u8>*, const be<u8>*)
 ext 027445FC void assureCafeTextTermination(sead::SafeText*)
 ext 027444C8 void sead::CafeFileDevice::destroy(u32)
 ext 02744528 void sead::CafeContentFileDevice::destroy(u32)
 ext 02744588 void sead::CafeHostFileDevice::destroy(u32)
 ext C0009988 void* OSBlockMove(void*, const void*, u32, u32)
 ext C0008DB0 s32 FSGetLastError(void*)
 ext C0008E38 void FSInit()
 ext C0008C78 s32 FSAddClient(void*, u32)
 ext C0008F58 void FSSetStateChangeNotification(void*, sead::FSStateChangeParams*)
 ext C0007450 s32 SAVEInit()
 ext C0008D08 s32 FSDelClient(void*, u32)
 ext C00074E0 void SAVEShutdown()
 ext C0008F68 void FSShutdown()
 ext C0008E40 void FSInitCmdBlock(void*)
 ext C0008E98 s32 FSOpenFile(void*, void*, const be<u8>*, const char*, be<u32>*, u32)
 ext C0008CF8 s32 FSCloseFile(void*, void*, u32, u32)
 ext C0008EC8 s32 FSReadFile(void*, void*, u8*, u32, u32, u32, u32, u32)
 ext C0008F98 s32 FSWriteFile(void*, void*, const u8*, u32, u32, u32, u32, u32)
 ext C0008F48 s32 FSSetPosFile(void*, void*, u32, u32, u32)
 ext C0008E00 s32 FSGetStatFile(void*, void*, u32, void*, u32)
 ext C0008DF0 s32 FSGetStat(void*, void*, const be<u8>*, void*, u32)
 ext C0008E88 s32 FSOpenDir(void*, void*, const be<u8>*, be<u32>*, u32)
 ext C0008CE8 s32 FSCloseDir(void*, void*, u32, u32)
 ext C0008EB8 s32 FSReadDir(void*, void*, u32, void*, u32)
*/
