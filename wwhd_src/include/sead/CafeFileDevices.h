/* WWHD sead Cafe file devices, second part: the host-I/O device ("hostio", paths below the
 * host root with %VARIABLE% expansion), the save-data device ("save", SAVE* calls on one
 * account) and the Cafe devices' deleting destructors with their per-TU text copies.
 * The device base, the Cafe FS device and the text classes are in sead/FileDevice.h (SEAD#8).
 * Layouts come from this game's constructors and instructions. */
#pragma once
#include "sead/FileDevice.h"

namespace sead {

/* Members of the SEAD#8 classes whose bodies are in this unit (local until merged into
 * FileDevice.h): the Cafe devices' deleting destructors and their shared availability slot. */

/* The host-I/O device: a Cafe FS device mounted at the host root path. */
struct CafeHostFileDevice : CafeFileDevice {
    static CafeHostFileDevice* construct(CafeHostFileDevice* device);
    void destroy(u32 flags);
    u32 toHostPath(be<u8>* out, u32 capacity, const be<u8>* path);
    u32 formatPath(SafeTextBuffer* out, SafeText* path);
};

/* The save-data device: SAVE* calls for one account; handles use the Cafe handle views. */
struct SaveFileDevice : FileDevice {
    be<s32> mLastStatus;
    be<u32> mOpenErrorMask;
    be<u32> mCloseErrorMask;
    be<u32> mReadErrorMask;
    gptr<void> mClient;          // NULL: the device manager's client
    be<u8> mAccountSlot;
    u8 mPadding69[3];

    static SaveFileDevice* construct(SaveFileDevice* device, u8 accountSlot);
    void destroy(u32 flags);
    u32 doIsAvailable();
    bool initSaveDirectory();
    void* getClient();
    bool flushQuota();
    CafeFileHandle* getSaveHandle(FileHandle* handle);
    CafeDirectoryHandle* getSaveDirectoryHandle(DirectoryHandle* handle);
    SaveFileDevice* doOpen(FileHandle* handle, SafeText* path, u32 mode);
    bool doClose(FileHandle* handle);
    bool doRead(be<u32>* readSize, FileHandle* handle, u8* buffer, u32 size);
    bool doWrite(be<u32>* written, FileHandle* handle, const u8* buffer, u32 size);
    bool doSeek(FileHandle* handle, s32 offset, u32 origin);
    bool doGetFileSize(be<u32>* size, FileHandle* handle);
    bool doIsExistFile(be<u8>* exists, SafeText* path);
    SaveFileDevice* doOpenDirectory(DirectoryHandle* handle, SafeText* path);
    bool doCloseDirectory(DirectoryHandle* handle);
    bool doReadDirectory(be<u32>* readCount, DirectoryHandle* handle, DirectoryEntry* entries, u32 count);
};

/* ---- per-TU copies of the text classes' virtual functions (see FileDevice.h) ---- */
namespace cafe_device_text {   // vtables stored by the Cafe FS device code
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : SafeTextBuffer { void destroy(u32 flags); };
struct PathText : SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : SafeTextBuffer { void destroy(u32 flags); };
struct DriveName : SafeTextBuffer { void destroy(u32 flags); };
}
namespace save_device_text {   // vtables stored by the save device code
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedText : SafeTextBuffer { void destroy(u32 flags); };
struct DriveName : SafeTextBuffer { void destroy(u32 flags); };
}

}

WWHD_SIZE(sead::CafeFileDevice, 0x6C);
WWHD_SIZE(sead::CafeContentFileDevice, 0x6C);
WWHD_SIZE(sead::CafeHostFileDevice, 0x6C);
WWHD_OFFSET(sead::SaveFileDevice, mLastStatus, 0x54);
WWHD_OFFSET(sead::SaveFileDevice, mOpenErrorMask, 0x58);
WWHD_OFFSET(sead::SaveFileDevice, mCloseErrorMask, 0x5C);
WWHD_OFFSET(sead::SaveFileDevice, mReadErrorMask, 0x60);
WWHD_OFFSET(sead::SaveFileDevice, mClient, 0x64);
WWHD_OFFSET(sead::SaveFileDevice, mAccountSlot, 0x68);
WWHD_SIZE(sead::SaveFileDevice, 0x6C);
