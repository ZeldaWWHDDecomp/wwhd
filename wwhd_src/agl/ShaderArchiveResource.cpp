/* Archive preparation from the two assigned WWHD resource walkers. */
#include "agl/ShaderStageResource.h"
#include "agl/ShaderResourceViews.h"
#include "agl/ShaderEndianConversions.h"
using namespace agl;
struct ProgramArchiveHeader_l {
    be<u32> tag, version, bytes, byteOrder, sectionOffset;
};
struct BinaryArchiveHeader_l {
    be<u32> tag, version, bytes, byteOrder, namesRelocated, sectionOffset;
};
struct ProgramArchiveResource_l {
    gptr<ProgramArchiveHeader_l> image;
};
struct BinaryArchiveResource_l {
    gptr<BinaryArchiveHeader_l> image;
};
struct ArchiveList_l {
    be<u32> bytes, count;
};
struct ArchiveRecord_l {
    be<u32> bytes, sectionOffset, nameOffset;
};
struct ArchiveWalk_l {
    be<u32> index;
    gptr<void> record;
};
struct ProgramArchiveFrame_l {
    ShaderBinaryCursor program;
    gptr<void> lastConvertedRecord, currentConvertedRecord;
    ShaderStageResource stage;
    ArchiveWalk_l stageNames, nestedNames;
    gptr<void> stageMirror;
    be<u32> stageNameCount;
    u8 pad30[4];
    be<u32> nestedNameCount;
    u8 pad38[4];
    ArchiveWalk_l values, defaults, variants, variantNames;
    be<u32> valueCount;
    u8 pad60[4];
    be<u32> defaultCount;
    u8 pad68[4];
    be<u32> variantCount;
    u8 pad70[4];
    be<u32> variantNameCount;
    u8 pad78[4];
    ArchiveWalk_l programHeaders, followingHeaders, programs, trailingHeaders;
    be<u32> programHeaderCount;
    u8 padA0[4];
    be<u32> followingHeaderCount;
    u8 padA8[4];
    be<u32> programCount;
    u8 padB0[4];
    be<u32> trailingHeaderCount;
};
struct BinaryArchiveFrame_l {
    ShaderStageResource stage;
    ArchiveWalk_l nestedNames;
    ShaderBinaryCursor program;
    gptr<void> stageMirror;
    be<u32> nestedNameCount;
    u8 pad20[4];
    ShaderStageResource descriptor, relocation;
    ArchiveWalk_l values, defaults, variants, variantNames;
    be<u32> valueCount;
    u8 pad50[4];
    be<u32> defaultCount;
    u8 pad58[4];
    be<u32> variantCount;
    u8 pad60[4];
    be<u32> variantNameCount;
    u8 pad68[4];
    ArchiveWalk_l names, descriptors, programHeaders, programs, stageDescriptors;
    be<u32> nameCount;
    u8 pad98[4];
    be<u32> descriptorCount;
    u8 padA0[4];
    be<u32> programHeaderCount;
    u8 padA8[4];
    be<u32> programCount;
    u8 padB0[4];
    be<u32> stageDescriptorCount;
};
WWHD_SIZE(ProgramArchiveHeader_l, 0x14);
WWHD_SIZE(BinaryArchiveHeader_l, 0x18);
WWHD_OFFSET(ProgramArchiveHeader_l, byteOrder, 0xC);
WWHD_OFFSET(BinaryArchiveHeader_l, namesRelocated, 0x10);
WWHD_SIZE(ArchiveList_l, 8);
WWHD_SIZE(ArchiveWalk_l, 8);
WWHD_SIZE(ProgramArchiveFrame_l, 0xB0);
WWHD_OFFSET(ProgramArchiveFrame_l, stage, 0xC);
WWHD_OFFSET(ProgramArchiveFrame_l, stageNames, 0x10);
WWHD_OFFSET(ProgramArchiveFrame_l, variantNames, 0x4C);
WWHD_OFFSET(ProgramArchiveFrame_l, trailingHeaderCount, 0xAC);
WWHD_SIZE(BinaryArchiveFrame_l, 0xB0);
WWHD_OFFSET(BinaryArchiveFrame_l, program, 0xC);
WWHD_OFFSET(BinaryArchiveFrame_l, descriptor, 0x1C);
WWHD_OFFSET(BinaryArchiveFrame_l, relocation, 0x20);
WWHD_OFFSET(BinaryArchiveFrame_l, stageDescriptorCount, 0xAC);
static void* programStageBlock(ShaderBinaryCursor*, u32);
static void* binaryStageBlock(ShaderBinaryCursor*, u32);
static void prepareStageBlock(u32, ShaderStageResource*, u32);

template <class T> static T* archiveAdd(void* base, u32 offset);
static ArchiveList_l* followingArchiveList(ArchiveList_l* list);
static ArchiveRecord_l* firstArchiveRecord(ArchiveList_l* list);
static ArchiveRecord_l* nextArchiveRecord(ArchiveRecord_l* record);
static void beginArchiveWalk(ArchiveWalk_l& walk, be<u32>& savedCount, ArchiveRecord_l* first, u32 count);
static u32 archiveOrder(u32 word);
static u32 programArchiveOrder(ProgramArchiveResource_l* resource);
static u32 binaryArchiveOrder(BinaryArchiveResource_l* resource);
static void convertArchiveRecords(u32 order, ArchiveList_l* list, u32 bytes, ArchiveWalk_l& walk, be<u32>& savedCount);
static ArchiveList_l* programArchiveFirst(ProgramArchiveResource_l* resource);
static ArchiveList_l* binaryArchiveFirst(BinaryArchiveResource_l* resource);
/* 027BC854 */
static bool prepareProgramArchive(ProgramArchiveResource_l* resource) {
    gabi::AutoFrame activation{gabi::current_function()};
    gabi::FrameLocal<ProgramArchiveFrame_l> scratch(8);
    if (!programArchiveOrder(resource)) return true;
    ShaderEndianConversions::convert(1, resource->image.get(), 0x14);
    ArchiveList_l* first = programArchiveFirst(resource);
    convertArchiveRecords(
        programArchiveOrder(resource), first, 0x14, scratch->programHeaders, scratch->programHeaderCount);
    ArchiveRecord_l* program = firstArchiveRecord(first);
    ArchiveList_l* second = followingArchiveList(programArchiveFirst(resource));
    convertArchiveRecords(
        programArchiveOrder(resource), second, 0x10, scratch->followingHeaders, scratch->followingHeaderCount);
    u32 programs = first->count;
    beginArchiveWalk(scratch->programs, scratch->programCount, program, programs);
    for (u32 p = 0; p < programs; ++p) {
        scratch->program.binary = reinterpret_cast<ShaderBinaryHeader_l*>(program);
        for (u32 stage = 0; stage < 3; ++stage) {
            auto* names = static_cast<ArchiveList_l*>(scratch->program.blockAt(static_cast<s32>(stage)));
            u32 order = programArchiveOrder(resource);
            ShaderEndianConversions::convert(order, names, 8);
            ArchiveRecord_l* record = firstArchiveRecord(names);
            u32 count = names->count;
            beginArchiveWalk(scratch->stageNames, scratch->stageNameCount, record, count);
            for (u32 i = 0; i < count; ++i) {
                scratch->currentConvertedRecord = record;
                ShaderEndianConversions::convert(order, record, 12);
                scratch->lastConvertedRecord = record;
                record = nextArchiveRecord(record);
            }
        }
        auto* values = static_cast<ArchiveList_l*>(scratch->program.blockAfterThree());
        convertArchiveRecords(programArchiveOrder(resource), values, 16, scratch->values, scratch->valueCount);
        void* defaultsBase = scratch->program.blockAfterThree();
        auto* defaultsSpan = static_cast<ArchiveList_l*>(scratch->program.blockAfterThree());
        auto* defaults = archiveAdd<ArchiveList_l>(defaultsBase, static_cast<u32>(defaultsSpan->bytes));
        convertArchiveRecords(programArchiveOrder(resource), defaults, 16, scratch->defaults, scratch->defaultCount);
        void* variantBase = scratch->program.blockAfterThree();
        auto* variantSpan = static_cast<ArchiveList_l*>(scratch->program.blockAfterThree());
        void* firstOffset = archiveAdd<void>(variantBase, static_cast<u32>(variantSpan->bytes));
        void* secondBase = scratch->program.blockAfterThree();
        auto* secondSpan = static_cast<ArchiveList_l*>(scratch->program.blockAfterThree());
        auto* secondOffset = archiveAdd<ArchiveList_l>(secondBase, static_cast<u32>(secondSpan->bytes));
        auto* variants = archiveAdd<ArchiveList_l>(firstOffset, static_cast<u32>(secondOffset->bytes));
        convertArchiveRecords(programArchiveOrder(resource), variants, 12, scratch->variants, scratch->variantCount);
        ArchiveRecord_l* variant = firstArchiveRecord(variants);
        u32 variantCount = variants->count;
        beginArchiveWalk(scratch->variantNames, scratch->variantNameCount, variant, variantCount);
        for (u32 v = 0; v < variantCount; ++v) {
            auto* names = archiveAdd<ArchiveList_l>(variant, static_cast<u32>(variant->nameOffset) + 12);
            convertArchiveRecords(
                programArchiveOrder(resource), names, 12, scratch->nestedNames, scratch->nestedNameCount);
            variant = nextArchiveRecord(variant);
        }
        for (u32 stage = 0; stage < 4; ++stage) {
            u32 order = programArchiveOrder(resource);
            void* block = programStageBlock(&scratch->program, stage);
            scratch->stageMirror = block;
            scratch->stage.block = static_cast<ShaderStageBlockPrefix_l*>(block);
            prepareStageBlock(order, &scratch->stage, stage);
        }
        program = nextArchiveRecord(program);
    }
    ArchiveList_l* trailing = followingArchiveList(followingArchiveList(programArchiveFirst(resource)));
    convertArchiveRecords(
        programArchiveOrder(resource), trailing, 16, scratch->trailingHeaders, scratch->trailingHeaderCount);
    ProgramArchiveHeader_l* image = resource->image;
    image->byteOrder = 1 - static_cast<u32>(image->byteOrder);
    return true;
}
/* Shared helpers stay within marked mutation ranges; replay all callers for helper mutations. */
template <class T> static T* archiveAdd(void* base, u32 offset) {
    return gabi::at<T>(gabi::ea(base) + offset);
}
static ArchiveList_l* followingArchiveList(ArchiveList_l* list) {
    return archiveAdd<ArchiveList_l>(list, static_cast<u32>(list->bytes));
}
static ArchiveRecord_l* firstArchiveRecord(ArchiveList_l* list) {
    return archiveAdd<ArchiveRecord_l>(list, 8);
}
static ArchiveRecord_l* nextArchiveRecord(ArchiveRecord_l* record) {
    return archiveAdd<ArchiveRecord_l>(record, static_cast<u32>(record->bytes));
}
static void beginArchiveWalk(ArchiveWalk_l& walk, be<u32>& savedCount, ArchiveRecord_l* first, u32 count) {
    walk.index = 0;
    walk.record = first;
    savedCount = count;
}
static u32 archiveOrder(u32 word) {
    return (word & 0x01000001u) ? 1 : 0;
}
static u32 programArchiveOrder(ProgramArchiveResource_l* resource) {
    return archiveOrder(resource->image->byteOrder);
}
static u32 binaryArchiveOrder(BinaryArchiveResource_l* resource) {
    return archiveOrder(resource->image->byteOrder);
}
static void convertArchiveRecords(u32 order, ArchiveList_l* list, u32 bytes, ArchiveWalk_l& walk, be<u32>& savedCount) {
    ShaderEndianConversions::convert(order, list, 8);
    ArchiveRecord_l* record = firstArchiveRecord(list);
    u32 count = list->count;
    beginArchiveWalk(walk, savedCount, record, count);
    for (u32 i = 0; i < count; ++i) {
        ShaderEndianConversions::convert(order, record, bytes);
        record = nextArchiveRecord(record);
    }
}
static ArchiveList_l* programArchiveFirst(ProgramArchiveResource_l* resource) {
    ProgramArchiveHeader_l* image = resource->image;
    return archiveAdd<ArchiveList_l>(image, static_cast<u32>(image->sectionOffset) + 0x14);
}
static ArchiveList_l* binaryArchiveFirst(BinaryArchiveResource_l* resource) {
    BinaryArchiveHeader_l* image = resource->image;
    return archiveAdd<ArchiveList_l>(image, static_cast<u32>(image->sectionOffset) + 0x18);
}

/* 027BCDA0 */
static bool prepareBinaryArchive(BinaryArchiveResource_l* resource, u32 relocate) {
    gabi::AutoFrame activation{gabi::current_function()};
    gabi::FrameLocal<BinaryArchiveFrame_l> scratch(8);
    bool conversionNeeded = binaryArchiveOrder(resource) != 0;
    if (conversionNeeded) ShaderEndianConversions::convert(1, resource->image.get(), 0x18);
    ArchiveList_l* descriptors = binaryArchiveFirst(resource);
    if (!conversionNeeded) {
        if (static_cast<u32>(resource->image->namesRelocated)) return true;
        ArchiveRecord_l* record = firstArchiveRecord(descriptors);
        u32 count = descriptors->count;
        beginArchiveWalk(scratch->names, scratch->nameCount, record, count);
        for (u32 i = 0; i < count; ++i) {
            scratch->relocation.block = reinterpret_cast<ShaderStageBlockPrefix_l*>(record);
            scratch->relocation.relocateNames();
            record = nextArchiveRecord(record);
        }
        resource->image->namesRelocated = 1;
        return true;
    }
    convertArchiveRecords(
        binaryArchiveOrder(resource), descriptors, 16, scratch->descriptors, scratch->descriptorCount);
    ArchiveRecord_l* descriptor = firstArchiveRecord(descriptors);
    ArchiveList_l* programs = followingArchiveList(binaryArchiveFirst(resource));
    convertArchiveRecords(
        binaryArchiveOrder(resource), programs, 16, scratch->programHeaders, scratch->programHeaderCount);
    ArchiveRecord_l* program = firstArchiveRecord(programs);
    u32 programCount = programs->count;
    beginArchiveWalk(scratch->programs, scratch->programCount, program, programCount);
    for (u32 p = 0; p < programCount; ++p) {
        scratch->program.binary = reinterpret_cast<ShaderBinaryHeader_l*>(program);
        auto* values = archiveAdd<ArchiveList_l>(program, static_cast<u32>(program->sectionOffset) + 16);
        convertArchiveRecords(binaryArchiveOrder(resource), values, 16, scratch->values, scratch->valueCount);
        auto* programView = reinterpret_cast<ArchiveRecord_l*>(scratch->program.binary.get());
        values = archiveAdd<ArchiveList_l>(programView, static_cast<u32>(programView->sectionOffset) + 16);
        auto* defaults = followingArchiveList(values);
        u32 order = binaryArchiveOrder(resource);
        ShaderEndianConversions::convert(order, defaults, 8);
        ArchiveRecord_l* defaultRecord = firstArchiveRecord(defaults);
        u32 defaultCount = defaults->count;
        beginArchiveWalk(scratch->defaults, scratch->defaultCount, defaultRecord, defaultCount);
        ArchiveList_l* variants = followingArchiveList(defaults);
        for (u32 i = 0; i < defaultCount; ++i) {
            ShaderEndianConversions::convert(order, defaultRecord, 16);
            defaultRecord = nextArchiveRecord(defaultRecord);
        }
        if (defaultCount) variants = followingArchiveList(defaults);
        convertArchiveRecords(binaryArchiveOrder(resource), variants, 12, scratch->variants, scratch->variantCount);
        ArchiveRecord_l* variant = firstArchiveRecord(variants);
        u32 variantCount = variants->count;
        beginArchiveWalk(scratch->variantNames, scratch->variantNameCount, variant, variantCount);
        for (u32 v = 0; v < variantCount; ++v) {
            auto* names = archiveAdd<ArchiveList_l>(variant, static_cast<u32>(variant->nameOffset) + 12);
            convertArchiveRecords(
                binaryArchiveOrder(resource), names, 12, scratch->nestedNames, scratch->nestedNameCount);
            variant = nextArchiveRecord(variant);
        }
        for (u32 stage = 0; stage < 4; ++stage) {
            u32 stageOrder = binaryArchiveOrder(resource);
            void* block = binaryStageBlock(&scratch->program, stage);
            scratch->stageMirror = block;
            scratch->stage.block = static_cast<ShaderStageBlockPrefix_l*>(block);
            prepareStageBlock(stageOrder, &scratch->stage, stage);
        }
        program = nextArchiveRecord(program);
    }
    u32 descriptorCount = descriptors->count;
    beginArchiveWalk(scratch->stageDescriptors, scratch->stageDescriptorCount, descriptor, descriptorCount);
    for (u32 i = 0; i < descriptorCount; ++i) {
        scratch->descriptor.block = reinterpret_cast<ShaderStageBlockPrefix_l*>(descriptor);
        scratch->descriptor.reverseWords();
        if (relocate && !static_cast<u32>(resource->image->namesRelocated)) scratch->descriptor.relocateNames();
        descriptor = nextArchiveRecord(descriptor);
    }
    if (relocate) resource->image->namesRelocated = 1;
    BinaryArchiveHeader_l* image = resource->image;
    image->byteOrder = 1 - static_cast<u32>(image->byteOrder);
    return true;
}
/* WWHD glue
ext 027BC6B8 void* programStageBlock(ShaderBinaryCursor*,u32)
ext 027BC73C void* binaryStageBlock(ShaderBinaryCursor*,u32)
ext 027BC790 void prepareStageBlock(u32,ShaderStageResource*,u32)
*/
