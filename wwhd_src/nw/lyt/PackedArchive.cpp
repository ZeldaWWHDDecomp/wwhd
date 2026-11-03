/* Packed archive lifetime and indexed resource spans reconstructed from WWHD. */
#include "nw/lyt/PackedArchive.h"
#include "nw/lyt/ResourceHelpers.h"
#include "nw/lyt/ArchiveFormatting.h"
using namespace nw::lyt;

/* 02873A40 */
PackedArchive* PackedArchive::create(PackedArchive* archive) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!archive) archive = static_cast<PackedArchive*>(packedArchiveAllocate(sizeof(PackedArchive)));
    if (archive) {
        archive->names = nullptr;
        archive->header = nullptr;
        ArchiveRecordSpan* span = &archive->records;
        archive->hashHeader = nullptr;
        if (gabi::ea(span) == 0) span = static_cast<ArchiveRecordSpan*>(packedArchiveAllocate(sizeof(ArchiveRecordSpan)));
        if (span) {
            span->entries = nullptr;
            span->count = 0;
        }
        archive->reverseByteOrder = 0;
        archive->data = nullptr;
    }
    return archive;
}

/* 02873AC0 */
void PackedArchive::destroy(u32 flags) {
    if (!this) return;
    if (gabi::ea(&records) != 0) {
        records.entries = nullptr;
        records.count = 0;
    }
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)packedArchiveDelete), this);
}

/* 02873AE8 */
void* PackedArchive::resourceAt(s32 index, ArchiveRange* range) const {
    if (index < 0) return nullptr;
    u32 count = records.count;
    if (index >= s32(count)) return nullptr;
    ArchiveRecord* base = records.entries;
    ArchiveRecord* entry = base;
    if (u32(index) < count) entry = layoutAtOffset<ArchiveRecord>(base, u32(index) * 16);
    u32 begin = entry->begin;
    if (range) {
        u32 end = entry->end;
        if (begin > end) return nullptr;
        range->begin = begin;
        range->size = end - begin;
    }
    be<u8>* storage = data;
    return layoutAtOffset<u8>(storage, begin);
}

/* 02873880 */
bool PackedArchive::attach(ArchiveHeader* source) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!source) return false;
    header = source;
    auto* signature = reinterpret_cast<be<u8>*>(source);
    const u8 archiveMagic[4] = {'S', 'A', 'R', 'C'};
    for (u32 index = 0; index < 4; ++index) {
        if (u8(signature[index]) != archiveMagic[index]) return false;
    }
    gabi::FrameLocal<be<u16>> byteOrder(8);
    *byteOrder = source->byteOrder;
    auto* orderBytes = reinterpret_cast<be<u8>*>(byteOrder.get());
    u8 first = orderBytes[0];
    if (first != 0xFE || u8(orderBytes[1]) != 0xFF) {
        reverseByteOrder = 1;
        return false;
    }
    ArchiveHeader* currentHeader = header;
    reverseByteOrder = 0;
    if (u16(currentHeader->version) != 0x100) return false;
    if (u16(currentHeader->headerSize) != 0x14) return false;
    auto* hash = layoutAtOffset<ArchiveHashHeader>(source, 0x14);
    hashHeader = hash;
    signature = reinterpret_cast<be<u8>*>(hash);
    const u8 hashMagic[4] = {'S', 'F', 'A', 'T'};
    for (u32 index = 0; index < 4; ++index) {
        if (u8(signature[index]) != hashMagic[index]) return false;
    }
    u16 hashSize = hash->headerSize;
    if (hashSize != 0xC) return false;
    u16 count = hash->count;
    if (count > 0x3FFF) return false;
    currentHeader = header;
    u16 headerSize = currentHeader->headerSize;
    u32 hashAddress = gabi::ea(source) + headerSize;
    auto* entries = gabi::at<ArchiveRecord>(hashAddress + 0xC);
    if (count != 0 && gabi::ea(entries) != 0) {
        currentHeader = header;
        records.entries = entries;
        hash = hashHeader;
        records.count = count;
        headerSize = currentHeader->headerSize;
        hashSize = hash->headerSize;
        count = hash->count;
        hashAddress = gabi::ea(source) + headerSize;
    }
    auto* nameHeader = gabi::at<ArchiveNameHeader>(hashAddress + hashSize + (u32(count) << 4));
    signature = reinterpret_cast<be<u8>*>(nameHeader);
    const u8 nameMagic[4] = {'S', 'F', 'N', 'T'};
    for (u32 index = 0; index < 4; ++index) {
        if (u8(signature[index]) != nameMagic[index]) return false;
    }
    if (u16(nameHeader->headerSize) != 8) return false;
    currentHeader = header;
    auto* nameBytes = layoutAtOffset<be<u8>>(nameHeader, 8);
    names = nameBytes;
    u32 dataOffset = currentHeader->dataOffset;
    u32 namesOffset = gabi::ea(nameBytes) - gabi::ea(currentHeader);
    if (s32(dataOffset) < s32(namesOffset)) return false;
    data = layoutAtOffset<be<u8>>(source, dataOffset);
    return true;
}

/* 02873B68 */
s32 PackedArchive::findName(const be<u8>* name) const {
    auto frame = gabi::AutoFrame(gabi::current_function());
    ArchiveHashHeader* hashInfo = hashHeader;
    u8 character = name[0];
    u32 multiplier = hashInfo->multiplier;
    u32 hash = 0;
    u32 offset = 0;
    while (character != 0) {
        hash = hash * multiplier + character;
        character = name[++offset];
    }
    u32 count = records.count;
    u32 lower = 0;
    u32 upper = count;
    ArchiveRecord* entries = records.entries;
    u32 index;
    for (;;) {
        index = u32(s32(lower + upper) / 2);
        auto* entry = layoutAtOffset<ArchiveRecord>(entries, index * 16);
        u32 value = entry->hash;
        if (value > hash) {
            if (upper == index) return -1;
            upper = index;
        } else if (value < hash) {
            if (lower == index) return -1;
            lower = index;
        } else break;
    }
    if (index == 0xFFFFFFFF) return -1;
    ArchiveRecord* entry = entries;
    if (index < count) entry = layoutAtOffset<ArchiveRecord>(entries, index * 16);
    u32 information = entry->nameInfo;
    if (information == 0) return s32(index);
    index -= (information >> 24) - 1;
    if (s32(index) >= s32(count)) return s32(index);
    u32 remaining = count - index;
    do {
        entry = nullptr;
        if (index < count) entry = layoutAtOffset<ArchiveRecord>(entries, index * 16);
        if (u32(entry->hash) != hash) return -1;
        information = entry->nameInfo;
        be<u8>* nameStorage = names;
        u32 wordOffset = information & 0xFFFFFF;
        be<u8>* dataStorage = data;
        if (gabi::ea(nameStorage) + wordOffset > gabi::ea(dataStorage)) return -1;
        auto* storedName = layoutAtOffset<be<u8>>(nameStorage, (wordOffset << 2));
        u32 cursor = 0;
        bool equal;
        for (;;) {
            u8 a = name[cursor];
            u8 b = storedName[cursor++];
            if (a != b) { equal = false; break; }
            if (a == 0) { equal = true; break; }
        }
        if (equal) return s32(index);
        --remaining;
        ++index;
    } while (remaining != 0);
    return s32(index);
}

struct ArchiveKindText { be<u8> bytes[5]; };
struct ArchivePathText { be<u8> bytes[256]; };

/* 028732A4 */
void* PackedArchive::findResource(const be<u8>* directory, u32 kind, const be<u8>* name, be<u32>* size) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<ArchiveKindText> kindText(8);
    kindText->bytes[2] = u8(kind >> 8);
    kindText->bytes[1] = u8(kind >> 16);
    kindText->bytes[0] = u8(kind >> 24);
    kindText->bytes[4] = 0;
    u8 first = directory[0];
    kindText->bytes[3] = u8(kind);
    gabi::FrameLocal<ArchivePathText> path(0x18);
    if (first == '.' && u8(directory[1]) == 0) {
        // GHS variadic calls mark the absence of floating arguments in CR1.
        gabi::cpu->cr[6] = 0;
        formatArchiveLocalName(path->bytes, 256, 255, "%s/%s", kindText->bytes, name);
    } else {
        gabi::cpu->cr[6] = 0;
        formatArchiveQualifiedName(path->bytes, 256, 255, "%s/%s/%s", directory, kindText->bytes, name);
    }
    s32 index = findName(path->bytes);
    if (index == -1) return nullptr;
    gabi::FrameLocal<ArchiveRange> range(0x10);
    range->begin = 0;
    range->size = 0;
    void* resource = resourceAt(index, range.get());
    if (size) *size = range->size;
    return resource;
}

/* WWHD glue
ext 028737BC s32 formatArchiveLocalName(be<u8>*, u32, u32, const char*, const be<u8>*, const be<u8>*)
ext 028737BC s32 formatArchiveQualifiedName(be<u8>*, u32, u32, const char*, const be<u8>*, const be<u8>*, const be<u8>*)
str 10180278
str 10180280
ext 0273AD10 void* packedArchiveAllocate(u32)
ext 0273AF40 void packedArchiveDelete(void*) callback
*/
