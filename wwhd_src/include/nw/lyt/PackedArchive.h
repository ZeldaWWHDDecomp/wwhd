/* Packed archive views from the SARC/SFAT/SFNT checks and indexed resource lookup. */
#pragma once
#include "nw/lyt/FontResource.h"
namespace nw { namespace lyt {
struct ArchiveHeader {
    be<u32> signature;
    be<u16> headerSize, byteOrder;
    be<u32> fileSize, dataOffset;
    be<u16> version, reserved;
};
struct ArchiveHashHeader { be<u32> signature; be<u16> headerSize, count; be<u32> multiplier; };
struct ArchiveNameHeader { be<u32> signature; be<u16> headerSize, reserved; };
struct ArchiveRecord { be<u32> hash, nameInfo, begin, end; };
struct ArchiveRecordSpan { be<u32> count; gptr<ArchiveRecord> entries; };
struct ArchiveRange { be<u32> begin, size; };
struct PackedArchive {
    gptr<ArchiveHeader> header;
    gptr<ArchiveHashHeader> hashHeader;
    gptr<be<u8>> names;
    ArchiveRecordSpan records;
    gptr<be<u8>> data;
    be<u32> reverseByteOrder;
    static PackedArchive* create(PackedArchive* archive);
    void destroy(u32 flags);
    bool attach(ArchiveHeader* source);
    s32 findName(const be<u8>* name) const;
    void* findResource(const be<u8>* directory, u32 kind, const be<u8>* name, be<u32>* size);
    void* resourceAt(s32 index, ArchiveRange* range) const;
};
WWHD_SIZE(ArchiveHeader, 0x14);
WWHD_SIZE(ArchiveHashHeader, 0xC);
WWHD_SIZE(ArchiveRecord, 0x10);
WWHD_SIZE(ArchiveRecordSpan, 8);
WWHD_SIZE(PackedArchive, 0x1C);
WWHD_OFFSET(PackedArchive, records, 0xC);
WWHD_OFFSET(PackedArchive, data, 0x14);
} }
void* packedArchiveAllocate(u32 size);
void packedArchiveDelete(void* storage);
