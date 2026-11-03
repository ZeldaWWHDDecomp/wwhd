/* Variant-key allocation and mixed-radix program generation from assigned WWHD instructions. */
#include "agl/ShaderArchiveRelocations.h"
#include "agl/ShaderProgram.h"
using namespace agl;
typedef be<u8> TextByte;
static TextByte sVariantEmptyText;
static sead::SafeTextVtable sVariantTextMethods;
static void* allocateVariantStorage(sead::RegistryHeap*, u32);
/* 027BA88C */
void ShaderVariantDomain::allocateRadices(s32 requested, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (requested <= 0) return;
    u32 records = static_cast<u32>(requested);
    void* storage = allocateVariantStorage(heap, records * 0x1C);
    u32 base = gabi::ea(storage), offset = 0;
    for (u32 remaining = records; remaining; --remaining, offset += 0x1C) {
        u32 address = base + offset;
        if (!address) continue;
        ShaderVariantRadix* record = gabi::at<ShaderVariantRadix>(address);
        record->name.mVtable = &sVariantTextMethods;
        record->valueCount = 0;
        record->values = nullptr;
        record->defaultValue.mText = &sVariantEmptyText;
        record->defaultValue.mVtable = &sVariantTextMethods;
        record->name.mText = &sVariantEmptyText;
    }
    if (storage) {
        radices = static_cast<ShaderVariantRadix*>(storage);
        count = records;
    }
}
static void* allocateVariantStorage(sead::RegistryHeap* requested, u32 bytes) {
    sead::RegistryHeap* heap = requested;
    if (!heap) heap = registryCurrentHeap(sRegistryManager.get());
    return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction, heap, bytes, 4u);
}
/* 027BA95C */
void ShaderProgram::allocateVariantKeys(s32 count, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* shared = static_cast<ShaderProgramSharedData*>(binary.get());
    shared->variation.allocateRadices(count, heap);
}
/* 027BA968 */
void ShaderVariantDomain::configureRadix(u32 index, const sead::SafeText* name, const sead::SafeText* defaultValue,
    s32 requested, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 records = count;
    u32 address = gabi::ea(radices.get());
    if (index < records) address += index * 0x1C;
    ShaderVariantRadix* record = gabi::at<ShaderVariantRadix>(address);
    record->name.mText = name->mText;
    be<u8>* initial = defaultValue->mText;
    record->divisor = 1;
    record->defaultValue.mText = initial;
    if (requested <= 0) return;
    u32 values = static_cast<u32>(requested);
    void* storage = allocateVariantStorage(heap, values * 8);
    u32 base = gabi::ea(storage), offset = 0;
    for (u32 remaining = values; remaining; --remaining, offset += 8) {
        u32 element = base + offset;
        if (!element) continue;
        sead::SafeText* value = gabi::at<sead::SafeText>(element);
        value->mText = &sVariantEmptyText;
        value->mVtable = &sVariantTextMethods;
    }
    if (storage) {
        record->values = static_cast<sead::SafeText*>(storage);
        record->valueCount = values;
    }
}
/* 027BAA54 */
void ShaderProgram::configureVariantKey(
    u32 index, const sead::SafeText* name, const sead::SafeText* defaultValue, s32 count, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* shared = static_cast<ShaderProgramSharedData*>(binary.get());
    shared->variation.configureRadix(index, name, defaultValue, count, heap);
}
/* 027BAAA8 */
void ShaderVariantDomain::createVariants(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 records = count;
    u32 start = gabi::ea(radices.get()), cursor = start, end = cursor + records * 0x1C;
    u32 combinations = 1;
    while (cursor != end) {
        u32 next = (cursor - start) / 0x1C + 1;
        while (static_cast<s32>(next) < static_cast<s32>(static_cast<u32>(count))) {
            ShaderVariantRadix* current = gabi::at<ShaderVariantRadix>(cursor);
            u32 nextAddress = gabi::ea(radices.get());
            if (next < static_cast<u32>(count)) nextAddress += next * 0x1C;
            const ShaderVariantRadix* following = gabi::at<const ShaderVariantRadix>(nextAddress);
            u32 product =
                static_cast<u32>(current->divisor) * static_cast<u16>(static_cast<u32>(following->valueCount));
            current->divisor = product;
            ++next;
        }
        combinations *= static_cast<u32>(gabi::at<const ShaderVariantRadix>(cursor)->valueCount);
        cursor += 0x1C;
    }
    u32 variants = combinations - 1;
    if (static_cast<s32>(variants) > 0) {
        void* storage = allocateVariantStorage(heap, variants * 0x84);
        u32 base = gabi::ea(storage), offset = 0;
        for (u32 remaining = variants; remaining; --remaining, offset += 0x84) {
            u32 address = base + offset;
            if (address) ShaderProgram::create(gabi::at<ShaderProgram>(address));
        }
        if (storage) {
            programCount = variants;
            programs = static_cast<ShaderProgram*>(storage);
        }
    }
    u32 size = programCount;
    cursor = gabi::ea(programs.get());
    end = cursor + size * 0x84;
    u32 index = 0;
    while (cursor != end) {
        ShaderProgram* program = gabi::at<ShaderProgram>(cursor);
        ShaderProgram* primary = root;
        auto* shared = static_cast<ShaderProgramSharedData*>(primary->binary.get());
        program->binary = shared;
        program->variantIndex = index + 1;
        program->configuration.archiveName = shared->name.mText;
        cursor += 0x84;
        ++index;
    }
}
/* 027BAC4C */
void ShaderProgram::createVariantPrograms(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* shared = static_cast<ShaderProgramSharedData*>(binary.get());
    shared->variation.createVariants(heap);
}
/* WWHD glue
data sVariantEmptyText 10000160
data sVariantTextMethods 10169C18
*/
