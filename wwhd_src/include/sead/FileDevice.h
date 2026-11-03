/* WWHD sead file layer: the file-device base, the archive and main devices, the device manager,
 * path helpers and the Cafe FS backend. One translation unit shared with SEAD#9-13 (this header and
 * the TU-wide statics are written by SEAD#8).
 * Layouts come from this game's constructors and instructions only (no external reference). */
#pragma once
#include "sead/Foundation.h"

namespace sead {

/* One entry of a GHS vtable: this-adjustment word and function address. */
struct VtableSlot {
    be<u32> mAdjust;
    be<u32> mFunction;
};

/* ---- text objects: {text pointer, vtable}; the buffered form adds its capacity ---- */
struct SafeTextVtable {
    VtableSlot mHeader;
    VtableSlot mDestroy;
    VtableSlot mAssureTermination;
};
struct SafeText {
    gptr<be<u8>> mText;
    gptr<SafeTextVtable> mVtable;
    void assureTermination() { gabi::call_ptr<void>(mVtable->mAssureTermination.mFunction, this); }
};
struct SafeTextBuffer : SafeText {
    be<s32> mCapacity;
};
template <u32 N> struct FixedSafeText : SafeTextBuffer {
    be<u8> mStorage[N];
};
typedef FixedSafeText<32> DriveNameText;
typedef FixedSafeText<256> PathText;

/* Longest text the inline length and compare loops scan. */
static constexpr s32 kMaxTextLength = 0x40000;

/* ---- disposer and list node bases (IDisposer-like: heap, link, vtable) ---- */
struct DisposerVtable {
    VtableSlot mHeader;
    VtableSlot mDestroy;
};
struct Disposer {
    gptr<void> mHeap;
    Link mLink;
    gptr<DisposerVtable> mVtable;
};
struct FileDevice;
struct DeviceListNode {
    Link mLink;
    gptr<FileDevice> mDevice;
    gptr<LinkList> mList;
};

/* ---- handles ---- */
struct HandleBase {
    Disposer mDisposer;
    gptr<FileDevice> mDevice;          // device that serves the handle
    gptr<FileDevice> mSourceDevice;  // device the handle was opened through
    u8 mBackend[0x20];                 // backend-specific state (see the views below)
};
struct FileHandle : HandleBase {
    be<u32> mReadChunkSize;            // 0: read in one call; else the largest single read
    u32 close();
    u32 tryClose();
    u32 read(void* buffer, u32 size);
    u32 tryRead(be<u32>* readSize, void* buffer, u32 size);
    u32 write(const void* buffer, u32 size);
    u32 tryWrite(be<u32>* written, const void* buffer, u32 size);
    u32 seek(s32 offset, u32 origin);
    u32 getFileSize();
};
struct DirectoryHandle : HandleBase {
    u32 read(struct DirectoryEntry* entries, u32 count);
};
struct DirectoryEntry {
    FixedSafeText<256> mName;
    be<u8> mIsDirectory;
    u8 mPadding[3];
};

/* Load request: the file is read into mBuffer, or into a new buffer from mHeap. */
struct LoadArg {
    SafeText mPath;
    gptr<u8> mBuffer;
    be<u32> mBufferSize;
    gptr<void> mHeap;
    be<s32> mAlignment;
    be<u32> mReadChunkSize;
    be<u32> mReadSize;        // out
    be<u32> mRoundedSize;     // out: size of the buffer used
    be<u8> mNeedsRelease;     // out: the buffer was allocated here
    u8 mPadding[3];
};

enum SeekOrigin { cSeekBegin = 0, cSeekCurrent = 1, cSeekEnd = 2 };

/* ---- the device base ---- */
struct FileDeviceVtable {
    VtableSlot mHeader[3];
    VtableSlot mDestroy;            // 0x1C
    VtableSlot mBaseSlots[4];
    VtableSlot mIsMatchDevice;      // 0x44
    VtableSlot mIsAvailable;        // 0x4C
    VtableSlot mLoad;               // 0x54
    VtableSlot mOpen;               // 0x5C
    VtableSlot mClose;              // 0x64
    VtableSlot mRead;               // 0x6C
    VtableSlot mWrite;              // 0x74
    VtableSlot mSeek;               // 0x7C
    VtableSlot mUnusedSlots80[2];
    VtableSlot mGetFileSize;        // 0x94
    VtableSlot mIsExistFile;        // 0x9C
    VtableSlot mUnusedSlotA0;
    VtableSlot mOpenDirectory;      // 0xAC
    VtableSlot mCloseDirectory;     // 0xB4
    VtableSlot mReadDirectory;      // 0xBC
    VtableSlot mUnusedSlotsC0[4];
    VtableSlot mFormatPath;         // 0xE4 (Cafe FS devices)
};

struct FileDevice {
    DeviceListNode mListNode;
    Disposer mDisposer;
    FixedSafeText<32> mDriveName;
    be<u8> mPermission;
    u8 mPadding[3];
    gptr<FileDeviceVtable> mVtable;

    /* virtual dispatch through the vtable */
    void dispatchDestroy(u32 flags) { gabi::call_ptr<void>(mVtable->mDestroy.mFunction, this, flags); }
    u32 dispatchIsMatchDevice(HandleBase* handle) { return gabi::call_ptr<u32>(mVtable->mIsMatchDevice.mFunction, this, handle); }

    /* non-virtual entry points */
    void destroy(u32 flags);
    u32 isAvailable();
    u8* tryLoad(LoadArg* arg);
    void setHandleReadChunkSize(FileHandle* handle, u32 size);
    void setHandleDevice(HandleBase* handle, FileDevice* device);
    void setHandleSourceDevice(HandleBase* handle, FileDevice* device);
    FileDevice* tryOpen(FileHandle* handle, SafeText* path, u32 mode, u32 readChunkSize);
    u32 tryClose(FileHandle* handle);
    u32 tryRead(be<u32>* readSize, FileHandle* handle, u8* buffer, u32 size);
    u32 tryWrite(be<u32>* written, FileHandle* handle, const u8* buffer, u32 size);
    u32 trySeek(FileHandle* handle, s32 offset, u32 origin);
    u32 tryGetFileSize(be<u32>* size, FileHandle* handle);
    u32 tryIsExistFile(be<u8>* exists, SafeText* path);
    FileDevice* tryOpenDirectory(DirectoryHandle* handle, SafeText* path);
    u32 tryCloseDirectory(DirectoryHandle* handle);
    u32 tryReadDirectory(be<u32>* readCount, DirectoryHandle* handle, DirectoryEntry* entries, u32 count);
    /* default virtual implementations */
    u32 isMatchDevice(HandleBase* handle);
    u8* doLoad(LoadArg* arg);
    u8* getHandleBackend(HandleBase* handle);
};

/* ---- archive device: files of an archive resource in memory ---- */
struct ArchiveFileInfo {
    be<u32> mOffset;
    be<u32> mSize;
};
/* the archive resource itself (Resource -> DirectResource -> ArchiveRes) is in sead/ArchiveRes.h */
struct ArchiveRes;
struct ArchiveFileHandle {
    gptr<u8> mData;
    ArchiveFileInfo mInfo;
    be<u32> mPosition;
};
struct ArchiveFileDevice : FileDevice {
    gptr<ArchiveRes> mArchive;

    void destroy(u32 flags);
    u8* doLoad(LoadArg* arg);
    ArchiveFileHandle* getArchiveHandle(FileHandle* handle);
    FileDevice* doOpen(FileHandle* handle, SafeText* path, u32 mode);
    bool doClose(FileHandle* handle);
    bool doRead(be<u32>* readSize, FileHandle* handle, u8* buffer, u32 size);
    bool doWrite(be<u32>* written, FileHandle* handle, const u8* buffer, u32 size);
    bool doSeek(FileHandle* handle, s32 offset, u32 origin);
    bool doGetFileSize(be<u32>* size, FileHandle* handle);
    bool doIsExistFile(be<u8>* exists, SafeText* path);
    FileDevice* doOpenDirectory(DirectoryHandle* handle, SafeText* path);
    u32 doCloseDirectory(DirectoryHandle* handle);
    bool doReadDirectory(be<u32>* readCount, DirectoryHandle* handle, DirectoryEntry* entries, u32 count);
    bool doIsAvailable();
};

/* ---- Cafe FS devices ---- */
struct CafeFileHandle {
    be<u32> mFSHandle;
    be<u32> mPosition;
};
struct CafeDirectoryHandle {
    be<u32> mFSHandle;
};
struct CafeFileDevice : FileDevice {
    gptr<be<u8>> mMountPoint;
    be<s32> mLastStatus;
    be<u32> mOpenErrorMask;
    be<u32> mCloseErrorMask;
    be<u32> mReadErrorMask;
    gptr<void> mClient;

    static CafeFileDevice* construct(CafeFileDevice* device, SafeText* driveName, SafeText* mountPoint);
    void* getClient();
    CafeFileHandle* getCafeHandle(FileHandle* handle);
    CafeDirectoryHandle* getCafeDirectoryHandle(DirectoryHandle* handle);
    void dispatchFormatPath(SafeTextBuffer* out, SafeText* path) { gabi::call_ptr<void>(mVtable->mFormatPath.mFunction, this, out, path); }
    FileDevice* doOpen(FileHandle* handle, SafeText* path, u32 mode);
    bool doClose(FileHandle* handle);
    bool doRead(be<u32>* readSize, FileHandle* handle, u8* buffer, u32 size);
    bool doWrite(be<u32>* written, FileHandle* handle, const u8* buffer, u32 size);
    bool doSeek(FileHandle* handle, s32 offset, u32 origin);
    bool doGetFileSize(be<u32>* size, FileHandle* handle);
    bool doIsExistFile(be<u8>* exists, SafeText* path);
    FileDevice* doOpenDirectory(DirectoryHandle* handle, SafeText* path);
    bool doCloseDirectory(DirectoryHandle* handle);
    bool doReadDirectory(be<u32>* readCount, DirectoryHandle* handle, DirectoryEntry* entries, u32 count);
    void formatPath(SafeTextBuffer* out, SafeText* path);
    void destroy(u32 flags);   /* 027444C8 (sead/CafeFileDevices.cpp) */
    u32 doIsAvailable();       /* 0274467C */
};
/* The content device ("content" mounted at "/vol/content"). */
struct CafeContentFileDevice : CafeFileDevice {
    static CafeContentFileDevice* construct(CafeContentFileDevice* device);
    void destroy(u32 flags);   /* 02744528 (sead/CafeFileDevices.cpp) */
};

/* ---- main device: the default device, forwards to its content device ---- */
struct MainFileDevice : FileDevice {
    gptr<CafeContentFileDevice> mDevice;

    static MainFileDevice* construct(MainFileDevice* device, void* heap);
    void destroy(u32 flags);
    u32 isAvailable();
    FileDevice* doOpen(FileHandle* handle, SafeText* path, u32 mode);
    u32 doClose(FileHandle* handle);
    u32 doRead(be<u32>* readSize, FileHandle* handle, u8* buffer, u32 size);
    u32 doWrite(be<u32>* written, FileHandle* handle, const u8* buffer, u32 size);
    u32 doSeek(FileHandle* handle, s32 offset, u32 origin);
    u32 doGetFileSize(be<u32>* size, FileHandle* handle);
    u32 doIsExistFile(be<u8>* exists, SafeText* path);
    FileDevice* doOpenDirectory(DirectoryHandle* handle, SafeText* path);
    u32 doCloseDirectory(DirectoryHandle* handle);
    u32 doReadDirectory(be<u32>* readCount, DirectoryHandle* handle, DirectoryEntry* entries, u32 count);
    u32 isMatchDevice(HandleBase* handle);
};

/* ---- device manager (singleton) ---- */
struct FSStateChangeParams {
    be<u32> mCallback;
    gptr<void> mContext;
    gptr<void> mQueue;
};
struct FileDeviceManager {
    Disposer mInstanceDisposer;   // constructed by createInstance before the manager
    LinkList mDevices;
    gptr<FileDevice> mDefaultDevice;
    gptr<MainFileDevice> mMainDevice;
    u8 mClient[0x1700];            // FSClient
    u8 mWorkArea[0x80];            // not used by this unit
    be<u8> mScratchText[0x80];
    be<u32> mScratchState;

    static FileDeviceManager* construct(FileDeviceManager* manager);
    static FileDeviceManager* createInstance(void* heap);
    void destroy(u32 flags);
    void mount(FileDevice* device, SafeText* driveName);
    void unmount(FileDevice* device);
    FileDevice* findDeviceFromDriveName(SafeText* driveName);
    FileDevice* findDevice(SafeText* path, SafeTextBuffer* pathWithoutDrive);
    FileDevice* tryOpen(FileHandle* handle, SafeText* path, u32 mode, u32 readChunkSize);
    u8* tryLoad(LoadArg* arg);
};
struct FileDeviceManagerDisposer : Disposer {
    void destroy(u32 flags);
};

/* ---- path helpers ---- */
namespace path {
bool getDriveName(SafeTextBuffer* out, SafeText* path);
bool getExtension(SafeTextBuffer* out, SafeText* path);
void getPathWithoutDrive(SafeTextBuffer* out, SafeText* path);
}

/* ---- per-TU copies of the text classes' virtual functions ----
 * Each group belongs to the translation unit whose code stores its vtables. A group lists its
 * immutable text, its buffered text and the fixed-size buffers; the sizes of fixed buffers whose
 * vtables are never stored in this binary are unknown, they are named in vtable order. */
namespace file_routes { namespace route_text {   // vtables installed by the file-route TU (SEAD#7)
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : SafeTextBuffer { void destroy(u32 flags); };
struct DriveName : SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : SafeTextBuffer { void destroy(u32 flags); };
struct FullPath : SafeTextBuffer { void destroy(u32 flags); };
} }
namespace archive_device_text {
struct FileHandle : sead::FileHandle { void destroy(u32 flags); };
}
namespace file_device_text {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FileHandle : sead::FileHandle { void destroy(u32 flags); };
struct FixedTextA : SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextC : SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextD : SafeTextBuffer { void destroy(u32 flags); };
}
namespace device_manager_text {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : SafeTextBuffer { void destroy(u32 flags); };
struct PathBuffer : SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : SafeTextBuffer { void destroy(u32 flags); };
struct DriveName : SafeTextBuffer { void destroy(u32 flags); };
}
namespace main_device_text {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : SafeTextBuffer { void destroy(u32 flags); };
struct DriveName : SafeTextBuffer { void destroy(u32 flags); };
}
namespace path_text {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
}
}

WWHD_OFFSET(sead::VtableSlot, mFunction, 4);
WWHD_SIZE(sead::VtableSlot, 8);
WWHD_OFFSET(sead::SafeTextVtable, mAssureTermination, 0x10);
WWHD_OFFSET(sead::SafeText, mVtable, 4);
WWHD_SIZE(sead::SafeText, 8);
WWHD_OFFSET(sead::SafeTextBuffer, mCapacity, 8);
WWHD_SIZE(sead::SafeTextBuffer, 0xC);
WWHD_SIZE(sead::FixedSafeText<32>, 0x2C);
WWHD_SIZE(sead::FixedSafeText<256>, 0x10C);
WWHD_OFFSET(sead::Disposer, mLink, 4);
WWHD_OFFSET(sead::Disposer, mVtable, 0xC);
WWHD_SIZE(sead::Disposer, 0x10);
WWHD_OFFSET(sead::DeviceListNode, mDevice, 8);
WWHD_OFFSET(sead::DeviceListNode, mList, 0xC);
WWHD_SIZE(sead::DeviceListNode, 0x10);
WWHD_OFFSET(sead::HandleBase, mDevice, 0x10);
WWHD_OFFSET(sead::HandleBase, mSourceDevice, 0x14);
WWHD_OFFSET(sead::HandleBase, mBackend, 0x18);
WWHD_SIZE(sead::HandleBase, 0x38);
WWHD_OFFSET(sead::FileHandle, mReadChunkSize, 0x38);
WWHD_SIZE(sead::FileHandle, 0x3C);
WWHD_OFFSET(sead::DirectoryEntry, mIsDirectory, 0x10C);
WWHD_SIZE(sead::DirectoryEntry, 0x110);
WWHD_OFFSET(sead::LoadArg, mBuffer, 8);
WWHD_OFFSET(sead::LoadArg, mBufferSize, 0xC);
WWHD_OFFSET(sead::LoadArg, mHeap, 0x10);
WWHD_OFFSET(sead::LoadArg, mAlignment, 0x14);
WWHD_OFFSET(sead::LoadArg, mReadChunkSize, 0x18);
WWHD_OFFSET(sead::LoadArg, mReadSize, 0x1C);
WWHD_OFFSET(sead::LoadArg, mRoundedSize, 0x20);
WWHD_OFFSET(sead::LoadArg, mNeedsRelease, 0x24);
WWHD_SIZE(sead::LoadArg, 0x28);
WWHD_OFFSET(sead::FileDeviceVtable, mDestroy, 0x18);
WWHD_OFFSET(sead::FileDeviceVtable, mIsMatchDevice, 0x40);
WWHD_OFFSET(sead::FileDeviceVtable, mIsAvailable, 0x48);
WWHD_OFFSET(sead::FileDeviceVtable, mLoad, 0x50);
WWHD_OFFSET(sead::FileDeviceVtable, mOpen, 0x58);
WWHD_OFFSET(sead::FileDeviceVtable, mClose, 0x60);
WWHD_OFFSET(sead::FileDeviceVtable, mRead, 0x68);
WWHD_OFFSET(sead::FileDeviceVtable, mWrite, 0x70);
WWHD_OFFSET(sead::FileDeviceVtable, mSeek, 0x78);
WWHD_OFFSET(sead::FileDeviceVtable, mGetFileSize, 0x90);
WWHD_OFFSET(sead::FileDeviceVtable, mIsExistFile, 0x98);
WWHD_OFFSET(sead::FileDeviceVtable, mOpenDirectory, 0xA8);
WWHD_OFFSET(sead::FileDeviceVtable, mCloseDirectory, 0xB0);
WWHD_OFFSET(sead::FileDeviceVtable, mReadDirectory, 0xB8);
WWHD_OFFSET(sead::FileDeviceVtable, mFormatPath, 0xE0);
WWHD_OFFSET(sead::FileDevice, mDisposer, 0x10);
WWHD_OFFSET(sead::FileDevice, mDriveName, 0x20);
WWHD_OFFSET(sead::FileDevice, mPermission, 0x4C);
WWHD_OFFSET(sead::FileDevice, mVtable, 0x50);
WWHD_SIZE(sead::FileDevice, 0x54);
WWHD_OFFSET(sead::ArchiveFileHandle, mInfo, 4);
WWHD_OFFSET(sead::ArchiveFileHandle, mPosition, 0xC);
WWHD_SIZE(sead::ArchiveFileHandle, 0x10);
WWHD_OFFSET(sead::ArchiveFileDevice, mArchive, 0x54);
WWHD_SIZE(sead::ArchiveFileDevice, 0x58);
WWHD_OFFSET(sead::CafeFileDevice, mMountPoint, 0x54);
WWHD_OFFSET(sead::CafeFileDevice, mLastStatus, 0x58);
WWHD_OFFSET(sead::CafeFileDevice, mOpenErrorMask, 0x5C);
WWHD_OFFSET(sead::CafeFileDevice, mCloseErrorMask, 0x60);
WWHD_OFFSET(sead::CafeFileDevice, mReadErrorMask, 0x64);
WWHD_OFFSET(sead::CafeFileDevice, mClient, 0x68);
WWHD_SIZE(sead::CafeFileDevice, 0x6C);
WWHD_SIZE(sead::CafeContentFileDevice, 0x6C);
WWHD_OFFSET(sead::MainFileDevice, mDevice, 0x54);
WWHD_SIZE(sead::MainFileDevice, 0x58);
WWHD_SIZE(sead::FSStateChangeParams, 0xC);
WWHD_OFFSET(sead::FileDeviceManager, mDevices, 0x10);
WWHD_OFFSET(sead::FileDeviceManager, mDefaultDevice, 0x1C);
WWHD_OFFSET(sead::FileDeviceManager, mMainDevice, 0x20);
WWHD_OFFSET(sead::FileDeviceManager, mClient, 0x24);
WWHD_OFFSET(sead::FileDeviceManager, mScratchText, 0x17A4);
WWHD_OFFSET(sead::FileDeviceManager, mScratchState, 0x1824);
WWHD_SIZE(sead::FileDeviceManager, 0x1828);

// The file-device TU's manager singleton (101F8B08) and empty text (104A0CD8), and the file-device base
// vtables (10143B78, 10143C58), owned by sead/FileDeviceStatics.cpp.
extern gptr<sead::FileDeviceManager>& sFileDeviceManager;
extern sead::SafeText& sFileDeviceEmptyText;
extern sead::FileDeviceVtable& sFileDeviceVtable;
extern sead::DisposerVtable& sFileDeviceDisposerVtable;
