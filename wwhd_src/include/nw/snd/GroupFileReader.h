/* Group resource readers. Layout and names reconstructed from WWHD's accesses. */
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
struct GroupLoadStatus {
    u8 mPrefix[0x2C];
    be<u8> mOpen;
    void destroy(u32 flags);
};
struct GroupFileHeader {
    be<u32> mMagic;
    be<u16> mByteOrder;
    be<u16> mHeaderSize;
    be<u32> mVersion;
};
struct GroupBlockHeader { be<u32> mMagic, mSize; };
struct GroupReference { be<u16> mKind, mReserved; be<s32> mOffset; };
struct GroupReferenceTable { be<u32> mCount; GroupReference mEntries[1]; };
struct GroupFileEntry { be<u32> mFileId, mReserved; be<s32> mFileOffset; };
struct GroupFileLocation { be<u32> mFileId; gptr<const be<u8>> mAddress; };
struct GroupExtraEntry { be<u32> mFileId, mFlags; };
struct GroupFileReader {
    gptr<GroupReferenceTable> mInfo;
    gptr<const be<u8>> mFileData;
    gptr<GroupReferenceTable> mExtraInfo;
    static u32 isSupported(const GroupFileHeader* header);
    static GroupFileReader* construct(GroupFileReader* reader, const GroupFileHeader* header);
    u32 readFile(GroupFileLocation* output, u32 index) const;
    u32 extraCount() const;
    u32 readExtra(GroupExtraEntry* output, u32 index) const;
};
WWHD_OFFSET(GroupLoadStatus,mOpen,0x2C);
WWHD_OFFSET(GroupFileHeader,mVersion,8);
WWHD_SIZE(GroupFileHeader,0xC);
WWHD_SIZE(GroupReference,8);
WWHD_OFFSET(GroupReferenceTable,mEntries,4);
WWHD_OFFSET(GroupFileEntry,mFileOffset,8);
WWHD_SIZE(GroupFileLocation,8);
WWHD_SIZE(GroupExtraEntry,8);
WWHD_OFFSET(GroupFileReader,mFileData,4);
WWHD_OFFSET(GroupFileReader,mExtraInfo,8);
WWHD_SIZE(GroupFileReader,0xC);
} }
