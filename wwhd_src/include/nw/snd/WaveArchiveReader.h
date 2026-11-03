/* Wave archive (FWAR) reader: header blocks, file table and the optional table of loaded file
 * addresses, reconstructed from WWHD instructions. */
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
enum : u32 {
    WaveArchiveMagic = 0x46574152,          // "FWAR"
    WaveArchiveVersion = 0x10000,
    WaveArchiveTableBlock = 0x6800,         // the file table block
    WaveArchiveDataBlock = 0x6801,          // the file data block
    WaveArchiveAddressMagic = 0x46574154,   // "FWAT": the data block starts with a loaded-address table
};
struct WaveArchiveBlockEntry { be<u16> mKind, mReserved; be<u32> mOffset, mSize; };
struct WaveArchiveHeader {
    be<u32> mMagic;
    be<u16> mByteOrder, mHeaderSize;
    be<u32> mVersion;
    be<u32> mFileSize;
    be<u16> mBlockCount, mReserved;
    WaveArchiveBlockEntry mBlocks[2];
    const WaveArchiveBlockEntry* findBlock(u32 kind) const;
    u32 tableOffset() const;
    const be<u8>* tableAddress() const;
    u32 dataOffset() const;
    const be<u8>* dataAddress() const;
    u32 isSupported() const;
};
struct WaveArchiveTable {
    be<u32> mCount;
    WaveArchiveBlockEntry mEntries[1];
    const WaveArchiveBlockEntry* entry(u32 index) const;
};
struct WaveArchiveReader {
    gptr<const WaveArchiveHeader> mHeader;
    gptr<const WaveArchiveTable> mTable;
    gptr<be<u32>> mCachedAddresses;
    be<u8> mValid;
    u8 mPadding[3];
    u32 hasAddressCache() const;
    void attach(const WaveArchiveHeader* header, u32 useCache);
    static WaveArchiveReader* construct(WaveArchiveReader* reader, const WaveArchiveHeader* header, u32 useCache);
    static WaveArchiveReader* constructEmpty(WaveArchiveReader* reader);
    void detach();
    u32 count() const;
    void clearCachedAddresses();
    u32 fileSize(u32 index) const;
    u32 fileDataOffset(u32 index) const;
    const void* loadedAddress(u32 index);                         // WaveFileReader.cpp (NW-SND#9)
    const void* exchangeAddress(u32 index, const void* address);  // WaveFileReader.cpp (NW-SND#9)
};
WWHD_SIZE(WaveArchiveBlockEntry,0xC);
WWHD_OFFSET(WaveArchiveHeader,mBlocks,0x14);
WWHD_SIZE(WaveArchiveHeader,0x2C);
WWHD_OFFSET(WaveArchiveTable,mEntries,4);
WWHD_OFFSET(WaveArchiveReader,mTable,4);
WWHD_OFFSET(WaveArchiveReader,mCachedAddresses,8);
WWHD_OFFSET(WaveArchiveReader,mValid,0xC);
WWHD_SIZE(WaveArchiveReader,0x10);
} }
