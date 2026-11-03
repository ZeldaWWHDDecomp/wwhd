/* Archive-list lookup and endian preparation from four assigned WWHD bodies. */
#include "agl/ShaderResourceViews.h"
#include "agl/ShaderStageResource.h"
#include "agl/ShaderArchiveRelocations.h"
#include "agl/ShaderEndianConversions.h"
using namespace agl;
struct ArchiveHelperList_l {
    be<u32> bytes, count;
};
struct ArchiveHelperRecord_l {
    be<u32> bytes, reserved4, sectionOffset, dataOffset, dataBytes, reserved14;
};
static sead::SafeTextVtable sArchiveHelperNameMethods;
WWHD_SIZE(ArchiveHelperList_l, 8);
WWHD_SIZE(ArchiveHelperRecord_l, 0x18);
WWHD_OFFSET(ArchiveHelperRecord_l, sectionOffset, 8);
WWHD_OFFSET(ArchiveHelperRecord_l, dataOffset, 12);
WWHD_OFFSET(ArchiveHelperRecord_l, dataBytes, 16);

template <class T> static T* addArchiveHelperOffset(void* base, u32 offset);
static ArchiveHelperList_l* nextArchiveHelperList(ArchiveHelperList_l* list);
static ArchiveHelperRecord_l* nextArchiveHelperRecord(ArchiveHelperRecord_l* record);
static void assureArchiveHelperName(const sead::SafeText* text);
static bool compareArchiveHelperName(sead::SafeText* query, sead::SafeText* candidate);
/* 027BC540 */
static void* findArchiveRecordByName(const ShaderBinaryCursor* cursor, sead::SafeText* query) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::SafeText> name(8);
    auto* list = reinterpret_cast<ArchiveHelperList_l*>(cursor->binary.get());
    u32 count = list->count;
    auto* record = addArchiveHelperOffset<ArchiveHelperRecord_l>(list, 8);
    for (u32 i = 0; i < count; ++i) {
        name->mVtable = &sArchiveHelperNameMethods;
        name->mText = addArchiveHelperOffset<be<u8>>(record, static_cast<u32>(record->sectionOffset) + 0x18);
        if (compareArchiveHelperName(query, name.get())) return record;
        record = nextArchiveHelperRecord(record);
    }
    return nullptr;
}
/* Shared helpers stay within marked mutation ranges; replay all callers for helper mutations. */
template <class T> static T* addArchiveHelperOffset(void* base, u32 offset) {
    return gabi::at<T>(gabi::ea(base) + offset);
}
static ArchiveHelperList_l* nextArchiveHelperList(ArchiveHelperList_l* list) {
    return addArchiveHelperOffset<ArchiveHelperList_l>(list, static_cast<u32>(list->bytes));
}
static ArchiveHelperRecord_l* nextArchiveHelperRecord(ArchiveHelperRecord_l* record) {
    return addArchiveHelperOffset<ArchiveHelperRecord_l>(record, static_cast<u32>(record->bytes));
}
static void assureArchiveHelperName(const sead::SafeText* text) {
    const gfn* table = gabi::at<const gfn>(gabi::ea(text->mVtable.get()));
    gabi::call_ptr<void>(table[5].get(), const_cast<sead::SafeText*>(text));
}
static bool compareArchiveHelperName(sead::SafeText* query, sead::SafeText* candidate) {
    assureArchiveHelperName(query);
    assureArchiveHelperName(query);
    be<u8>* cached = query->mText;
    assureArchiveHelperName(candidate);
    if (cached == candidate->mText.get()) return true;
    const auto* a = reinterpret_cast<const be<u8>*>(query->mText.get());
    const auto* b = reinterpret_cast<const be<u8>*>(candidate->mText.get());
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 x = a[i], y = b[i];
        if (x != y) return false;
        if (!x) return true;
    }
    return false;
}

/* 027BC6B8 */
static void* programStageBlock(ShaderBinaryCursor* cursor, u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* firstBase = cursor->blockAfterThree();
    auto* firstSpan = static_cast<ArchiveHelperList_l*>(cursor->blockAfterThree());
    void* following = addArchiveHelperOffset<void>(firstBase, static_cast<u32>(firstSpan->bytes));
    void* secondBase = cursor->blockAfterThree();
    auto* secondSpan = static_cast<ArchiveHelperList_l*>(cursor->blockAfterThree());
    auto* secondList = addArchiveHelperOffset<ArchiveHelperList_l>(secondBase, static_cast<u32>(secondSpan->bytes));
    auto* variants = addArchiveHelperOffset<ArchiveHelperList_l>(following, static_cast<u32>(secondList->bytes));
    ArchiveHelperList_l* block = nextArchiveHelperList(variants);
    for (s32 i = 0; i < static_cast<s32>(index); ++i)
        block = nextArchiveHelperList(block);
    return block;
}
/* 027BC73C */
static void* binaryStageBlock(ShaderBinaryCursor* cursor, u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    ShaderBinaryHeader_l* program = cursor->binary;
    auto* block = addArchiveHelperOffset<ArchiveHelperList_l>(program, static_cast<u32>(program->sectionOffset) + 0x10);
    block = nextArchiveHelperList(block);
    block = nextArchiveHelperList(block);
    block = nextArchiveHelperList(block);
    for (s32 i = 0; i < static_cast<s32>(index); ++i)
        block = nextArchiveHelperList(block);
    return block;
}
/* 027BC790 */
static void prepareStageBlock(u32 order, ShaderStageResource* resource, u32 kind) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* list = reinterpret_cast<ArchiveHelperList_l*>(resource->block.get());
    ShaderEndianConversions::convert(order, list, 8);
    u32 count = list->count;
    auto* first = addArchiveHelperOffset<ArchiveHelperRecord_l>(list, 8);
    ArchiveHelperRecord_l* record = first;
    for (u32 i = 0; i < count; ++i) {
        ShaderEndianConversions::convert(order, record, 0x18);
        record = nextArchiveHelperRecord(record);
    }
    if (kind == 1) return;
    count = list->count;
    record = first;
    for (u32 i = 0; i < count; ++i) {
        void* data = addArchiveHelperOffset<void>(
            record, static_cast<u32>(record->sectionOffset) + 0x18 + static_cast<u32>(record->dataOffset));
        u32 bytes = record->dataBytes;
        ShaderEndianConversions::convert(order, data, bytes);
        record = nextArchiveHelperRecord(record);
    }
}
/* WWHD glue
data sArchiveHelperNameMethods 10169D08
*/
