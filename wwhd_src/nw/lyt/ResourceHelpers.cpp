/* Binary resource-header checks and character reads reconstructed from WWHD.
 * Uses merged NW-LYT#1 layouts unchanged; no external reference used. */
#include "nw/lyt/ResourceHelpers.h"
using namespace nw::lyt;

/* 02871158 */
u32 layoutValidateResourceHeader(FontFile* file, u32 expectedMagic, u32 supportedVersion, u32 minimumBlocks) {
    if ((u32)file->magic != expectedMagic) return 0;
    if ((u16)file->byteOrder != 0xFEFF) return 0;
    u32 version = file->version;
    if (((supportedVersion ^ version) & 0xFF000000) != 0) return 0;
    if (((supportedVersion >> 16) & 0xFF) < ((version >> 16) & 0xFF)) return 0;
    if ((supportedVersion & 0xFF) > (version & 0xFF)) return 0;
    if ((u32)file->size < minimumBlocks * 8 + 0x14) return 0;
    return (u16)file->blockCount >= minimumBlocks;
}

/* 028711D0 */
u32 layoutByteOrderReversed(const FontFile* file) {
    return (u16)file->byteOrder != 0xFEFF;
}

static u32 reverseWord(u32 value) {
    return (value << 24) | ((value << 8) & 0x00FF0000) |
           ((value >> 8) & 0x0000FF00) | (value >> 24);
}

/* 028711E4 */
FontBlock* layoutNextResourceBlock(FontFile* file, FontBlock* previous) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 reversed = layoutByteOrderReversed(file);
    u32 candidate;
    u32 end;
    if (!reversed) {
        if (!previous) {
            if (!(u16)file->blockCount) return nullptr;
            candidate = gabi::ea(file) + (u16)file->headerSize;
        } else {
            candidate = gabi::ea(previous) + (u32)previous->size;
        }
        end = gabi::ea(file) + (u32)file->size;
    } else {
        if (!previous) {
            if (!(u16)file->blockCount) return nullptr;
            u32 size = file->size;
            u16 offset = file->headerSize;
            candidate = gabi::ea(file) + (u16)((offset << 8) | (offset >> 8));
            end = gabi::ea(file) + reverseWord(size);
        } else {
            u32 size = previous->size;
            u32 total = file->size;
            candidate = gabi::ea(previous) + reverseWord(size);
            end = gabi::ea(file) + reverseWord(total);
        }
    }
    return candidate < end ? gabi::at<FontBlock>(candidate) : nullptr;
}

/* 02871328 */
u32 layoutReadUtf16Character(CharacterCursor* cursor) {
    u32 address = cursor->text;
    u32 character = *gabi::at<be<u16>>(address);
    cursor->text = address + 2;
    return character;
}

/* 02872920 */
u32 layoutShortNameEqual(const be<u8>* left, const be<u8>* right) {
    for (u32 index = 0; index < 24; ++index) {
        u32 character = left[index];
        if (character != (u8)right[index]) return 0;
        if (character == 0) return 1;
    }
    return 1;
}

/* 0287295C */
u32 layoutLongNameEqual(const be<u8>* left, const be<u8>* right) {
    for (u32 index = 0; index < 28; ++index) {
        u32 character = left[index];
        if (character != (u8)right[index]) return 0;
        if (character == 0) return 1;
    }
    return 1;
}

/* 02871708 */
u32 layoutZeroResult() { return 0; }

/* 0287291C */
void layoutEmptyCallback() {}

/* 02874890 */
void setLayoutStorageAllocator(void* allocator) {
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)layoutStorageAllocatorSink), allocator);
}

/* WWHD glue
ext 0287F3D0 void layoutStorageAllocatorSink(void*) callback
*/
