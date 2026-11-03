/* WWHD sead Cafe file devices, second part (companion of the file-device translation unit,
 * whose header and TU-wide classes are in sead/FileDevice.h): the host-I/O device, the save
 * device, the Cafe devices' deleting destructors and the per-TU text copies of both device files.
 * Reconstructed from this game's instructions; Cafe SDK import names and arguments are facts
 * from wut/Cemu. */

#include "sead/CafeFileDevices.h"

using namespace sead;

/* functions of other units and Cafe imports (bound by the glue block) */
void* allocateObject(u32 size);
void releaseObject(void* object);
void constructDisposer(sead::Disposer* disposer);
s32 formatSafeText(sead::SafeTextBuffer* out, const char* format, const be<u8>* argument);
void* OSBlockMove(void* destination, const void* source, u32 size, u32 flush);
s32 ENVGetEnvironmentVariable(const be<u8>* name, be<u8>* buffer, u32 size);
void FSInitCmdBlock(void* block);
s32 FSCloseFile(void* client, void* block, u32 handle, u32 errorMask);
s32 FSReadFile(void* client, void* block, u8* buffer, u32 size, u32 count, u32 handle, u32 flags, u32 errorMask);
s32 FSWriteFile(void* client, void* block, const u8* buffer, u32 size, u32 count, u32 handle, u32 flags, u32 errorMask);
s32 FSSetPosFile(void* client, void* block, u32 handle, u32 position, u32 errorMask);
s32 FSGetStatFile(void* client, void* block, u32 handle, void* stat, u32 errorMask);
s32 FSCloseDir(void* client, void* block, u32 handle, u32 errorMask);
s32 FSReadDir(void* client, void* block, u32 handle, void* entry, u32 errorMask);
s32 SAVEInitSaveDir(u8 accountSlot);
s32 SAVEFlushQuota(void* client, void* block, u8 accountSlot, u32 errorMask);
s32 SAVEOpenFile(void* client, void* block, u8 accountSlot, const be<u8>* path, const char* mode, be<u32>* handle, u32 errorMask);
s32 SAVEGetStat(void* client, void* block, u8 accountSlot, const be<u8>* path, void* stat, u32 errorMask);
s32 SAVEOpenDir(void* client, void* block, u8 accountSlot, const be<u8>* path, be<u32>* handle, u32 errorMask);

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
WWHD_SIZE(FSCmdBlock, 0xA80);
WWHD_OFFSET(FSStat, mSize, 0x10);
WWHD_SIZE(FSStat, 0x64);
WWHD_OFFSET(FSDirEntry, mName, 0x64);
WWHD_SIZE(FSDirEntry, 0x164);

static constexpr u32 kDestroyRelease = 1;
static constexpr s32 kFSStatusEnd = -2;
static constexpr s32 kFSStatusNotFound = -6;
static constexpr u32 kFSStatNotFileMask = 0xC0000000;   // directory and quota flags
static constexpr u32 kFSStatDirectoryShift = 31;
static constexpr u32 kFSOpenErrorMask = 0xDA;
static constexpr u32 kFSGetStatErrorMask = 0x88;
static constexpr u32 kFSOpenDirErrorMask = 0xEA;
static constexpr u32 kHostPathCapacity = 256;

/* the device manager's FS client and the host root path it keeps in its work area */
static const char* sOpenModes[3] = {"w", "r+", "w+"};

static DisposerVtable sCafeDeviceDisposerVtable;
static FileDeviceVtable sHostDeviceVtable;
static DisposerVtable sHostDeviceDisposerVtable;
static FileDeviceVtable sSaveDeviceVtable;
static DisposerVtable sSaveDeviceDisposerVtable;
static SafeTextVtable sCafeTextVtable;
static SafeTextVtable sCafePathVtable;
static SafeTextVtable sSaveTextVtable;
static SafeTextVtable sSaveDriveNameVtable;

/* ---- inline helpers ---- */

/* the storage of a member object: its own address, or a new block when that address is NULL
 * (GHS member constructors allocate when their object pointer is NULL) */
template <class T>
static T* memberStorage(T* member, u32 size = sizeof(T)) {
    if (gabi::ea(member) == 0) member = static_cast<T*>(allocateObject(size));
    return member;
}

/* length of a text, 0 when it runs past kMaxTextLength */
static s32 textLength(const be<u8>* text) {
    s32 length = 0;
    while (text[length] != 0) {
        if (++length > kMaxTextLength) return 0;
    }
    return length;
}

/* ================================================================ host-I/O device */

/* 02743DA0 */
CafeHostFileDevice* sead::CafeHostFileDevice::construct(CafeHostFileDevice* device) {
    device = memberStorage(device);
    if (device == NULL) return NULL;
    gabi::Local<SafeText> driveName;
    gabi::Local<SafeText> mountPoint;
    driveName->mText.v = gabi::guest_str("hostio");
    driveName->mVtable = &sCafeTextVtable;
    mountPoint->mText = reinterpret_cast<be<u8>*>(sFileDeviceManager->mWorkArea);
    mountPoint->mVtable = &sCafeTextVtable;
    CafeFileDevice::construct(device, driveName, mountPoint);
    device->mVtable = &sHostDeviceVtable;
    device->mDisposer.mVtable = &sHostDeviceDisposerVtable;
    return device;
}

/* 02743E34 */
u32 sead::CafeHostFileDevice::toHostPath(be<u8>* out, u32 capacity, const be<u8>* path) {
    // "drive:/rest" becomes "drive/rest" and backslashes become slashes
    u32 in = 0, length = 0, used = 1;
    while (used < capacity) {
        u8 c = path[in];
        if (c == 0) break;
        if (c == ':') {
            out[length++] = '/';
            in += 2;
        } else {
            out[length++] = c == '\\' ? '/' : c;
            ++in;
        }
        ++used;
    }
    out[length] = 0;
    return length;
}

/* formatPath's text buffers for an environment variable name and its value */
struct HostPathText : SafeTextBuffer {
    be<u8> mStorage[kHostPathCapacity];
};

/* 02743EAC */
u32 sead::CafeHostFileDevice::formatPath(SafeTextBuffer* out, SafeText* path) {
    // "<prefix>%NAME%<rest>" becomes "<root>/<prefix><value of NAME><rest>"; without a
    // complete variable the path is appended to "<root>/" as it is
    gabi::Local<HostPathText> name;
    gabi::Local<SafeText> delimiter;
    gabi::Local<SafeText> rest;
    name->mStorage[0] = 0;
    name->mText = name->mStorage;
    name->mStorage[kHostPathCapacity - 1] = 0;
    name->mCapacity = kHostPathCapacity;
    delimiter->mVtable = &sCafeTextVtable;
    delimiter->mText.v = gabi::guest_str("%");
    name->mVtable = &sCafePathVtable;
    s32 prefixLength = -1, restStart = -1;

    // first token: up to and including the first '%'
    path->assureTermination();
    s32 end = textLength(path->mText) + 1;
    s32 position = 0;
    path->assureTermination();
    s32 length = textLength(path->mText);
    if (position >= 0 && position <= length) {
        for (;; ++position) {
            const be<u8>* text = path->mText;
            u8 c = text[position];
            if (c == 0) {
                ++position;
                break;
            }
            delimiter->assureTermination();
            const be<u8>* delimiters = delimiter->mText;
            bool found = false;
            for (u32 i = 0; i < kMaxTextLength + 1 && delimiters[i] != 0; ++i) {
                if ((u8)delimiters[i] == (u8)text[position]) {
                    found = true;
                    break;
                }
            }
            if (found) {
                ++position;
                break;
            }
        }
    }

    // second token: the variable name up to the next '%'
    if (position != end) {
        prefixLength = position - 1;
        s32 start = position;
        path->assureTermination();
        length = textLength(path->mText);
        if (start >= 0 && start <= length) {
            name->assureTermination();
            be<u8>* nameText = name->mText;
            s32 count = 0;
            position = start + 1;
            for (;;) {
                const be<u8>* text = path->mText;
                u8 c = text[start];
                if (c == 0) break;
                delimiter->assureTermination();
                const be<u8>* delimiters = delimiter->mText;
                c = text[start];
                bool found = false;
                for (u32 i = 0; i < kMaxTextLength + 1 && delimiters[i] != 0; ++i) {
                    if ((u8)delimiters[i] == c) {
                        found = true;
                        break;
                    }
                }
                if (found) break;
                nameText[count++] = c;
                ++position;
                ++start;
            }
            nameText[count] = 0;
        }
        if (position != end) restStart = position;
    }

    if (name->mText[0] == 0 || restStart == -1) {
        gabi::cpu->cr[6] = 0;
        s32 offset = formatSafeText(out, "%s/", mMountPoint);
        out->assureTermination();
        be<u8>* outText = out->mText;
        path->assureTermination();
        return toHostPath(outText + offset, (u32)out->mCapacity - offset, path->mText);
    }

    gabi::Local<HostPathText> value;
    value->mStorage[0] = 0;
    value->mText = value->mStorage;
    value->mVtable = &sCafePathVtable;
    value->mCapacity = kHostPathCapacity;
    value->mStorage[kHostPathCapacity - 1] = 0;
    name->assureTermination();
    const be<u8>* variable = name->mText;
    value->assureTermination();
    s32 status = ENVGetEnvironmentVariable(variable, value->mText, value->mCapacity);
    gabi::cpu->cr[6] = 0;
    s32 offset = formatSafeText(out, "%s/", mMountPoint);
    if (status != 0) {
        out->assureTermination();
        be<u8>* outText = out->mText;
        path->assureTermination();
        return toHostPath(outText + offset, (u32)out->mCapacity - offset, path->mText);
    }
    if (prefixLength > 0) {
        out->assureTermination();
        s32 capacity = out->mCapacity;
        be<u8>* outText = out->mText;
        be<u8>* segment = NULL;
        s32 room = 0;
        if (offset >= 0 && offset < capacity) {
            segment = outText + offset;
            room = (u32)out->mCapacity - offset;
            segment[room - 1] = 0;
        }
        if (prefixLength >= room) prefixLength = room - 1;
        path->assureTermination();
        OSBlockMove(segment, path->mText, prefixLength, 0);
        offset += prefixLength;
        segment[prefixLength] = 0;
    }
    out->assureTermination();
    be<u8>* outText = out->mText;
    value->assureTermination();
    offset += toHostPath(outText + offset, (u32)out->mCapacity - offset, value->mText);
    out->assureTermination();
    outText = out->mText;
    path->assureTermination();
    const be<u8>* text = path->mText;
    length = textLength(text);
    rest->mVtable = &sCafeTextVtable;
    rest->mText = restStart >= 0 && restStart <= length ? const_cast<be<u8>*>(text + restStart) : sFileDeviceEmptyText.mText.get();
    reinterpret_cast<cafe_device_text::Text*>(rest.get())->assureTerminationImpl();
    return toHostPath(outText + offset, (u32)out->mCapacity - offset, rest->mText);
}

/* ================================================================ Cafe devices: deleting
 * destructors (the derived parts have nothing to destroy; the device base does the rest) */

/* 027444C8 */
void sead::CafeFileDevice::destroy(u32 flags) {
    if (this == NULL) return;
    mDisposer.mVtable = &sCafeDeviceDisposerVtable;
    FileDevice::destroy(0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02744528 */
void sead::CafeContentFileDevice::destroy(u32 flags) {
    if (this == NULL) return;
    mDisposer.mVtable = &sCafeDeviceDisposerVtable;
    FileDevice::destroy(0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02744588 */
void sead::CafeHostFileDevice::destroy(u32 flags) {
    if (this == NULL) return;
    mDisposer.mVtable = &sCafeDeviceDisposerVtable;
    FileDevice::destroy(0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* ================================================================ per-TU text copies of the
 * Cafe device code */

/* 027445E8 */
void sead::cafe_device_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027445FC */
void sead::cafe_device_text::Text::assureTerminationImpl() {}

/* 02744600 */
void sead::cafe_device_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    text[(s32)mCapacity - 1] = 0;
}

/* 02744618 */
void sead::cafe_device_text::TextBuffer::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0274462C */
void sead::cafe_device_text::FixedTextA::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02744640 */
void sead::cafe_device_text::PathText::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02744654 */
void sead::cafe_device_text::FixedTextB::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02744668 */
void sead::cafe_device_text::DriveName::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0274467C */
u32 sead::CafeFileDevice::doIsAvailable() { return 1; }

/* ================================================================ save device */

/* 02744684 */
static void destroySaveDeviceFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<SaveFileDevice*>(reinterpret_cast<u8*>(disposer) - 0x10)->destroy(flags);
}

/* 0274468C */
SaveFileDevice* sead::SaveFileDevice::construct(SaveFileDevice* device, u8 accountSlot) {
    device = memberStorage(device);
    if (device == NULL) return NULL;
    gabi::Local<SafeText> driveName;
    driveName->mText.v = gabi::guest_str("save");
    driveName->mVtable = &sSaveTextVtable;

    // the device base, inlined
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
    FixedSafeText<32>* text = memberStorage(&device->mDriveName);
    if (text != NULL) {
        text->mStorage[31] = 0;
        text->mStorage[0] = 0;
        text->mText = text->mStorage;
        text->mVtable = &sSaveDriveNameVtable;
        text->mCapacity = 32;
    }
    device->mPermission = 1;

    // drive name "save"
    be<u8>* out = device->mDriveName.mText;
    driveName->assureTermination();
    s32 length = textLength(driveName->mText);
    s32 capacity = device->mDriveName.mCapacity;
    u32 assureTermination = driveName->mVtable->mAssureTermination.mFunction;
    if (length >= capacity) length = capacity - 1;
    gabi::call_ptr<void>(assureTermination, driveName.get());
    OSBlockMove(out, driveName->mText, length, 0);
    out[length] = 0;

    device->mAccountSlot = accountSlot;
    device->mDisposer.mVtable = &sSaveDeviceDisposerVtable;
    device->mOpenErrorMask = kFSOpenErrorMask;
    device->mLastStatus = 0;
    device->mVtable = &sSaveDeviceVtable;
    device->mClient = NULL;
    device->mReadErrorMask = 0;
    device->mCloseErrorMask = 0;
    return device;
}

/* 0274483C */
bool sead::SaveFileDevice::initSaveDirectory() {
    s32 status = SAVEInitSaveDir(mAccountSlot);
    mLastStatus = status;
    return status == 0;
}

/* 02744878 */
void* sead::SaveFileDevice::getClient() {
    void* client = mClient;
    if (client != NULL) return client;
    return sFileDeviceManager->mClient;
}

/* 02744894 */
bool sead::SaveFileDevice::flushQuota() {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    s32 status = SAVEFlushQuota(client, block.get(), mAccountSlot, 0);
    mLastStatus = status;
    return status == 0;
}

/* 027448E8 */
CafeFileHandle* sead::SaveFileDevice::getSaveHandle(FileHandle* handle) {
    return reinterpret_cast<CafeFileHandle*>(getHandleBackend(handle));
}

/* 02744908 */
SaveFileDevice* sead::SaveFileDevice::doOpen(FileHandle* handle, SafeText* path, u32 mode) {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeFileHandle* saveHandle = getSaveHandle(handle);
    const char* modeText = "r";
    if (mode >= 1 && mode <= 3) modeText = sOpenModes[mode - 1];
    path->assureTermination();
    s32 status = SAVEOpenFile(client, block.get(), mAccountSlot, path->mText, modeText, &saveHandle->mFSHandle, mOpenErrorMask);
    saveHandle->mPosition = 0;
    mLastStatus = status;
    if (status == 0) return this;
    saveHandle->mFSHandle = 0;
    return NULL;
}

/* 02744A30 */
bool sead::SaveFileDevice::doClose(FileHandle* handle) {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeFileHandle* saveHandle = getSaveHandle(handle);
    return FSCloseFile(client, block.get(), saveHandle->mFSHandle, mCloseErrorMask) == 0;
}

/* 02744AA8 */
bool sead::SaveFileDevice::doRead(be<u32>* readSize, FileHandle* handle, u8* buffer, u32 size) {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeFileHandle* saveHandle = getSaveHandle(handle);
    s32 status = FSReadFile(client, block.get(), buffer, 1, size, saveHandle->mFSHandle, 0, mReadErrorMask);
    if (status < 0) {
        mLastStatus = status;
        return false;
    }
    mLastStatus = 0;
    saveHandle->mPosition = (u32)saveHandle->mPosition + (u32)status;
    if (readSize != NULL) *readSize = status;
    return true;
}

/* 02744B70 */
bool sead::SaveFileDevice::doWrite(be<u32>* written, FileHandle* handle, const u8* buffer, u32 size) {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeFileHandle* saveHandle = getSaveHandle(handle);
    s32 status = FSWriteFile(client, block.get(), buffer, 1, size, saveHandle->mFSHandle, 0, 0);
    if (status < 0) {
        mLastStatus = status;
        return false;
    }
    mLastStatus = 0;
    saveHandle->mPosition = (u32)saveHandle->mPosition + (u32)status;
    if (written != NULL) *written = status;
    return true;
}

/* 02744C38 */
bool sead::SaveFileDevice::doSeek(FileHandle* handle, s32 offset, u32 origin) {
    gabi::Local<be<u32>> fileSize;
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeFileHandle* saveHandle = getSaveHandle(handle);
    u32 position = offset;
    if (origin == cSeekCurrent) {
        position += (u32)saveHandle->mPosition;
    } else if (origin == cSeekEnd) {
        *fileSize = 0;
        if (!gabi::call_ptr<bool>(mVtable->mGetFileSize.mFunction, this, fileSize.get(), handle)) return false;
        position += (u32)*fileSize;
    } else if (origin != cSeekBegin) {
        return false;
    }
    s32 status = FSSetPosFile(client, block.get(), saveHandle->mFSHandle, position, 0);
    mLastStatus = status;
    if (status != 0) return false;
    saveHandle->mPosition = position;
    return true;
}

/* 02744D54 */
bool sead::SaveFileDevice::doGetFileSize(be<u32>* size, FileHandle* handle) {
    gabi::Local<FSStat> stat;
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeFileHandle* saveHandle = getSaveHandle(handle);
    s32 status = FSGetStatFile(client, block.get(), saveHandle->mFSHandle, stat.get(), 0);
    mLastStatus = status;
    if (status != 0) return false;
    *size = stat->mSize;
    return true;
}

/* 02744E10 */
bool sead::SaveFileDevice::doIsExistFile(be<u8>* exists, SafeText* path) {
    gabi::Local<FSStat> stat;
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    path->assureTermination();
    s32 status = SAVEGetStat(client, block.get(), mAccountSlot, path->mText, stat.get(), kFSGetStatErrorMask);
    mLastStatus = status;
    if (status == kFSStatusNotFound) {
        *exists = 0;
        return true;
    }
    if (status != 0) return false;
    *exists = ((u32)stat->mFlags & kFSStatNotFileMask) == 0;
    return true;
}

/* 02744F18 */
CafeDirectoryHandle* sead::SaveFileDevice::getSaveDirectoryHandle(DirectoryHandle* handle) {
    return reinterpret_cast<CafeDirectoryHandle*>(getHandleBackend(handle));
}

/* 02744F38 */
SaveFileDevice* sead::SaveFileDevice::doOpenDirectory(DirectoryHandle* handle, SafeText* path) {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeDirectoryHandle* saveHandle = getSaveDirectoryHandle(handle);
    path->assureTermination();
    s32 status = SAVEOpenDir(client, block.get(), mAccountSlot, path->mText, &saveHandle->mFSHandle, kFSOpenDirErrorMask);
    mLastStatus = status;
    return status == 0 ? this : NULL;
}

/* 02744FEC */
bool sead::SaveFileDevice::doCloseDirectory(DirectoryHandle* handle) {
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeDirectoryHandle* saveHandle = getSaveDirectoryHandle(handle);
    s32 status = FSCloseDir(client, block.get(), saveHandle->mFSHandle, 0);
    mLastStatus = status;
    return status == 0;
}

/* 02745068 */
bool sead::SaveFileDevice::doReadDirectory(be<u32>* readCount, DirectoryHandle* handle, DirectoryEntry* entries, u32 count) {
    gabi::Local<SafeText> name;
    gabi::Local<FSDirEntry> entry;
    gabi::Local<FSCmdBlock> block;
    FSInitCmdBlock(block.get());
    void* client = getClient();
    CafeDirectoryHandle* saveHandle = getSaveDirectoryHandle(handle);
    u32 done = 0;
    for (u32 remaining = count; remaining != 0; --remaining, ++entries) {
        s32 status = FSReadDir(client, block.get(), saveHandle->mFSHandle, entry.get(), 0);
        mLastStatus = status;
        if (status != 0) {
            if (readCount != NULL) *readCount = done;
            return status == kFSStatusEnd;
        }
        // copy the entry name (the first termination call is direct, the second virtual)
        name->mText = entry->mName;
        name->mVtable = &sSaveTextVtable;
        be<u8>* out = entries->mName.mText;
        reinterpret_cast<save_device_text::Text*>(name.get())->assureTerminationImpl();
        s32 length = textLength(name->mText);
        s32 capacity = entries->mName.mCapacity;
        u32 assureTermination = name->mVtable->mAssureTermination.mFunction;
        if (length >= capacity) length = capacity - 1;
        gabi::call_ptr<void>(assureTermination, name.get());
        OSBlockMove(out, name->mText, length, 0);
        out[length] = 0;
        ++done;
        entries->mIsDirectory = (u32)entry->mStat.mFlags >> kFSStatDirectoryShift;
    }
    if (readCount != NULL) *readCount = count;
    return true;
}

/* 027451EC */
void sead::SaveFileDevice::destroy(u32 flags) {
    if (this == NULL) return;
    mDisposer.mVtable = &sSaveDeviceDisposerVtable;
    FileDevice::destroy(0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* ================================================================ per-TU text copies of the
 * save device code */

/* 0274524C */
void sead::save_device_text::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02745260 */
void sead::save_device_text::Text::assureTerminationImpl() {}

/* 02745264 */
void sead::save_device_text::TextBuffer::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02745278 */
void sead::save_device_text::TextBuffer::assureTerminationImpl() {
    be<u8>* text = mText;
    text[(s32)mCapacity - 1] = 0;
}

/* 02745290 */
void sead::save_device_text::FixedText::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027452A4 */
void sead::save_device_text::DriveName::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027452B8 */
u32 sead::SaveFileDevice::doIsAvailable() { return 1; }

/* WWHD glue
 ext 0273AD10 void* allocateObject(u32)
 ext 0273AF40 void releaseObject(void*)
 ext 02752A84 void constructDisposer(sead::Disposer*)
 ext 02759C28 s32 formatSafeText(sead::SafeTextBuffer*, const char*, const be<u8>*)
 ext 02740A38 void sead::FileDevice::destroy(u32)
 ext 02741428 u8* sead::FileDevice::getHandleBackend(sead::HandleBase*)
 ext 02743128 sead::CafeFileDevice* sead::CafeFileDevice::construct(sead::CafeFileDevice*, sead::SafeText*, sead::SafeText*)
 ext C0009988 void* OSBlockMove(void*, const void*, u32, u32)
 ext C0008980 s32 ENVGetEnvironmentVariable(const be<u8>*, be<u8>*, u32)
 ext C0008E40 void FSInitCmdBlock(void*)
 ext C0008CF8 s32 FSCloseFile(void*, void*, u32, u32)
 ext C0008EC8 s32 FSReadFile(void*, void*, u8*, u32, u32, u32, u32, u32)
 ext C0008F98 s32 FSWriteFile(void*, void*, const u8*, u32, u32, u32, u32, u32)
 ext C0008F48 s32 FSSetPosFile(void*, void*, u32, u32, u32)
 ext C0008E00 s32 FSGetStatFile(void*, void*, u32, void*, u32)
 ext C0008CE8 s32 FSCloseDir(void*, void*, u32, u32)
 ext C0008EB8 s32 FSReadDir(void*, void*, u32, void*, u32)
 ext C0007458 s32 SAVEInitSaveDir(u8)
 ext C00073F8 s32 SAVEFlushQuota(void*, void*, u8, u32)
 ext C0007490 s32 SAVEOpenFile(void*, void*, u8, const be<u8>*, const char*, be<u32>*, u32)
 ext C0007430 s32 SAVEGetStat(void*, void*, u8, const be<u8>*, void*, u32)
 ext C0007470 s32 SAVEOpenDir(void*, void*, u8, const be<u8>*, be<u32>*, u32)
 data sOpenModes 101442EC
 data sCafeDeviceDisposerVtable 10144168 nocheck
 data sHostDeviceVtable 10144178 nocheck
 data sHostDeviceDisposerVtable 10144260 nocheck
 data sSaveDeviceVtable 10144300 nocheck
 data sSaveDeviceDisposerVtable 101443E0 nocheck
 data sCafeTextVtable 10143EA0 nocheck
 data sCafePathVtable 10143EE8 nocheck
 data sSaveTextVtable 10144280 nocheck
 data sSaveDriveNameVtable 101442C8 nocheck
 str 10144070 "hostio"
 str 10144078 "%s/"
 str 1014407C "%"
 str 101442E0 "save"
 str 101442E8 "r"
*/
