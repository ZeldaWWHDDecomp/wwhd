/* Relative resource table arithmetic reconstructed from WWHD. */
#include "nw/lyt/ResourceTables.h"
#include "nw/lyt/ResourceHelpers.h"
using namespace nw::lyt;

/* 02873CE8 */
void RelativeResourceTables::attach(RelativeTableHeader* header) {
    payload = header->payload;
    u16 count = header->recordCount;
    recordCount = count;
    offsetCount = header->offsetCount;
    u32 offset = header->tableOffset;
    records = layoutAtOffset<ResourceTableRecord>(header, offset);
    offset = header->tableOffset;
    offsets = layoutAtOffset<be<u32>>(header, offset + u32(count) * 0x18);
}

/* 02873D24 */
ResourceTableRecord* RelativeResourceTables::record(u32 index) const {
    if (index >= u16(recordCount)) return nullptr;
    ResourceTableRecord* base = records;
    return layoutAtOffset<ResourceTableRecord>(base, index * 0x18);
}

/* 02873D48 */
void* RelativeResourceTables::offsetTarget(u32 index) const {
    if (index >= u16(offsetCount)) return nullptr;
    be<u32>* base = offsets;
    u32 displacement = base[index];
    return layoutAtOffset<u8>(base, displacement);
}

/* 0287443C */
u32 nw::lyt::layoutTextureNameEqual(const be<u8>* first, const be<u8>* second) {
    for (u32 index = 0; index < 128; ++index) {
        u8 a = first[index];
        u8 b = second[index];
        if (a != b) return 0;
        if (a == 0) return 1;
    }
    return 1;
}

/* WWHD glue
*/
