/* WWHD sead archive resources: a direct resource whose image is an archive of files, with the archive
 * format's functions as virtual functions, and the SARC archive (header, file allocation table sorted by
 * name hash, file name table, data block).
 * Layouts from this game's constructor (SharcArchiveRes 0x44) and instructions; SARC block layout from
 * the public SARC format description (facts only). */
#pragma once
#include "sead/Resource.h"

namespace sead {

struct ArchiveResVtable : ResourceVtable {
    VtableSlot mGetFile;                // 0x38 (path, info) -> data
    VtableSlot mGetFileFast;            // 0x40 (entry, info) -> data
    VtableSlot mConvertPathToEntryId;   // 0x48 (path) -> entry
    VtableSlot mSetCurrentDirectory;    // 0x50
    VtableSlot mOpenDirectory;          // 0x58
    VtableSlot mCloseDirectory;         // 0x60
    VtableSlot mReadDirectory;          // 0x68
    VtableSlot mPrepareArchive;         // 0x70 (image) -> bool
};

/* an archive image: valid once prepareArchive accepted it */
struct ArchiveRes : DirectResource {
    be<u8> mEnable;
    u8 mPadding[3];

    ArchiveResVtable* archiveVtable() { return reinterpret_cast<ArchiveResVtable*>(mVtable.get()); }
    /* virtual dispatch (the archive file device's calls) */
    u8* getFile(SafeText* path, ArchiveFileInfo* info) {
        return gabi::call_ptr<u8*>(archiveVtable()->mGetFile.mFunction, this, path, info);
    }
    void doCreate(u8* data, u32 size, RegistryHeap* heap);
};

/* per-TU copies of the archive translation unit's text classes */
namespace archive_res_text {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextA : SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : SafeTextBuffer { void destroy(u32 flags); };
}

/* SARC blocks */
struct SarcHeader {
    be<u8> mSignature[4];       // "SARC"
    be<u16> mHeaderSize;        // 0x14
    be<u16> mByteOrderMark;     // FE FF: big endian
    be<u32> mFileSize;
    be<u32> mDataOffset;
    be<u16> mVersion;           // 0x100
    be<u16> mReserved;
};
struct SarcFatHeader {
    be<u8> mSignature[4];       // "SFAT"
    be<u16> mHeaderSize;        // 0xC
    be<u16> mFileCount;
    be<u32> mHashKey;
};
struct SarcFatEntry {
    be<u32> mHash;
    be<u32> mNameOffset;        // bits 24..31: index among equal hashes (1-based); bits 0..23: name, in 4-byte units
    be<u32> mDataStart;
    be<u32> mDataEnd;
};
struct SarcFntHeader {
    be<u8> mSignature[4];       // "SFNT"
    be<u16> mHeaderSize;        // 8
    be<u16> mReserved;
};

/* an array view: element count and storage */
struct SarcFatEntryBuffer {
    be<s32> mSize;
    gptr<SarcFatEntry> mBuffer;
};

struct SharcArchiveRes : ArchiveRes {
    gptr<SarcHeader> mSarcHeader;
    gptr<SarcFatHeader> mFatBlockHeader;
    gptr<u8> mFntBlock;
    SarcFatEntryBuffer mFatEntries;
    gptr<u8> mDataBlock;
    be<u32> mEndian;            // byte order of the archive

    enum { cArchiveVersion = 0x100, cArchiveEntryMax = 0x3FFF };

    static SharcArchiveRes* construct(SharcArchiveRes* archive);
    void destroy(u32 flags);
    u8* getFileImpl(SafeText* path, ArchiveFileInfo* info);
    u8* getFileFastImpl(s32 entry, ArchiveFileInfo* info);
    s32 convertPathToEntryIdImpl(SafeText* path);
    bool openDirectoryImpl(be<u32>* handle, SafeText* path);
    bool closeDirectoryImpl(be<u32>* handle);
    u32 readDirectoryImpl(be<u32>* handle, DirectoryEntry* entries, u32 count);
    bool prepareArchive(const u8* image);
    bool isDerivedFrom(const RuntimeType* type) const;
    void postCreate();
    u32 getLoadDataAlignment();
};

namespace sharc_text {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
}

}

WWHD_OFFSET(sead::ArchiveResVtable, mGetFile, 0x38);
WWHD_OFFSET(sead::ArchiveResVtable, mConvertPathToEntryId, 0x48);
WWHD_OFFSET(sead::ArchiveResVtable, mPrepareArchive, 0x70);
WWHD_OFFSET(sead::ArchiveRes, mEnable, 0x24);
WWHD_SIZE(sead::ArchiveRes, 0x28);
WWHD_OFFSET(sead::SarcHeader, mByteOrderMark, 6);
WWHD_OFFSET(sead::SarcHeader, mDataOffset, 0xC);
WWHD_OFFSET(sead::SarcHeader, mVersion, 0x10);
WWHD_SIZE(sead::SarcHeader, 0x14);
WWHD_OFFSET(sead::SarcFatHeader, mFileCount, 6);
WWHD_OFFSET(sead::SarcFatHeader, mHashKey, 8);
WWHD_SIZE(sead::SarcFatHeader, 0xC);
WWHD_SIZE(sead::SarcFatEntry, 0x10);
WWHD_SIZE(sead::SarcFntHeader, 8);
WWHD_SIZE(sead::ArchiveFileInfo, 8);
WWHD_OFFSET(sead::ArchiveFileInfo, mSize, 4);
WWHD_OFFSET(sead::SharcArchiveRes, mSarcHeader, 0x28);
WWHD_OFFSET(sead::SharcArchiveRes, mFatBlockHeader, 0x2C);
WWHD_OFFSET(sead::SharcArchiveRes, mFntBlock, 0x30);
WWHD_OFFSET(sead::SharcArchiveRes, mFatEntries, 0x34);
WWHD_OFFSET(sead::SharcArchiveRes, mDataBlock, 0x3C);
WWHD_OFFSET(sead::SharcArchiveRes, mEndian, 0x40);
WWHD_SIZE(sead::SharcArchiveRes, 0x44);
