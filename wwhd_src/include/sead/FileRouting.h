/* File-route devices reconstructed from WWHD: a prefix device that forwards to a target device under
 * a path prefix, and a multi device that selects the first of its member devices holding a path.
 * Both derive from the file-device base of sead/FileDevice.h (SEAD#8); the text, handle and
 * directory-entry types are the shared ones declared there. */

// prim/seadSafeString.h: interface/member declarations corroborate text layouts only.
// WWHD table words and caller instructions establish slot order and ABI.
#pragma once
#include "sead/FileDevice.h"
#include "sead/ReportListeners.h"
namespace sead { namespace file_routes {
/* Text classes whose vtables this translation unit stores (its own per-TU copies). */
struct Text : SafeText { void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct NameBuffer : FixedSafeText<32> { void destroy(u32 flags); };
struct PrefixBuffer : FixedSafeText<256> { void destroy(u32 flags); };
struct PathBuffer : FixedSafeText<512> { void destroy(u32 flags); };
struct PrefixDevice : FileDevice {
    gptr<FileDevice> mTarget;
    PrefixBuffer mPrefix;
    void destroy(u32 flags);
    u32 doIsAvailable();
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
struct MultiDevice : FileDevice {
    PointerBuffer mDevices;
    gptr<FileDevice> mLastDevice;
    void destroy(u32 flags);
    u32 appendDevice(FileDevice* device);
    FileDevice* findDevice(SafeText* path);
    u32 doIsAvailable();
    u8* doLoad(LoadArg* arg);
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
// Opaque lifetime entries make no field, size, or template-ownership claim.
struct InlineTextRelease { void destroy(u32 flags); };
struct ImmutableTextRelease { void destroy(u32 flags); };
struct OwnedTextRelease { void destroy(u32 flags); };
struct FixedTextRelease { void destroy(u32 flags); };
struct ReportCollection : ReportList { void destroy(u32 flags); };
WWHD_SIZE(NameBuffer,0x2C); WWHD_SIZE(PrefixBuffer,0x10C); WWHD_SIZE(PathBuffer,0x20C);
WWHD_OFFSET(PrefixDevice,mTarget,0x54); WWHD_OFFSET(PrefixDevice,mPrefix,0x58); WWHD_SIZE(PrefixDevice,0x164);
WWHD_OFFSET(MultiDevice,mDevices,0x54); WWHD_OFFSET(MultiDevice,mLastDevice,0x60); WWHD_SIZE(MultiDevice,0x64);
WWHD_SIZE(ReportCollection,sizeof(ReportList));
} }
