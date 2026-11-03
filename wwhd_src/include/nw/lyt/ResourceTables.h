/* Binary-proven relative table view; no unproven resource-owner attribution. */
#pragma once
#include "nw/lyt/FontResource.h"
namespace nw { namespace lyt {
struct RelativeTableHeader {
    u8 prefix[8];
    be<u32> tableOffset;
    be<u16> recordCount, offsetCount;
    be<u8> payload[1];
};
struct ResourceTableRecord { u8 bytes[0x18]; };
struct RelativeResourceTables {
    gptr<be<u8>> payload;
    be<u16> recordCount, offsetCount;
    gptr<ResourceTableRecord> records;
    gptr<be<u32>> offsets;
    void attach(RelativeTableHeader* header);
    ResourceTableRecord* record(u32 index) const;
    void* offsetTarget(u32 index) const;
};
WWHD_OFFSET(RelativeTableHeader, tableOffset, 8);
WWHD_OFFSET(RelativeTableHeader, payload, 0x10);
WWHD_SIZE(ResourceTableRecord, 0x18);
WWHD_SIZE(RelativeResourceTables, 0x10);
u32 layoutTextureNameEqual(const be<u8>* first, const be<u8>* second);
} }
