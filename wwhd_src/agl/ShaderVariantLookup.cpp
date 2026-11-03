/* Bounded variant-name lookup and leaf lifetime operations from assigned WWHD code. */
#include "agl/ShaderArchiveRelocations.h"
#include "sead/FileDevice.h"
using namespace agl;
struct VariantLookupIndices_l {
    be<s32> values[32];
};
struct VariantArchiveHandle_l {
    gptr<void> resource;
};
static sead::SafeTextVtable sVariantLookupMethods;
static void* clearVariantLookup(void*, u32, u32);
static u32 resolveVariantSelection(const ShaderVariantDomain*, const be<s32>*);
static void releaseVariantLookupObject(void*);
WWHD_SIZE(VariantLookupIndices_l, 0x80);
WWHD_SIZE(VariantArchiveHandle_l, 4);

static void assureVariantLookupText(const sead::SafeText* text);
static bool boundedVariantTextEqual(const be<u8>* a, const be<u8>* b);
static bool equalVariantLookupText(const sead::SafeText* left, const sead::SafeText* right);
/* 027BBD04 */
static u32 queryVariantSelection(const ShaderVariantDomain* domain, s32 requestCount, const gptr<be<u8>>* names,
    const gptr<be<u8>>* values, s32 variant) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::SafeText> name(8), value(0x10);
    gabi::FrameLocal<VariantLookupIndices_l> indices(0x18);
    if (variant == -1)
        clearVariantLookup(indices.get(), 0, static_cast<u32>(domain->count) * 4);
    else
        domain->decompose(variant, indices->values);
    u32 count = domain->count, begin = gabi::ea(domain->radices.get()), cursor = begin, end = begin + count * 0x1Cu;
    while (cursor != end) {
        const auto* radix = gabi::at<const ShaderVariantRadix>(cursor);
        for (s32 request = 0; request < requestCount; ++request) {
            name->mText = names[static_cast<u32>(request)];
            name->mVtable = &sVariantLookupMethods;
            if (!equalVariantLookupText(&radix->name, name.get())) continue;
            u32 choiceCount = radix->valueCount;
            u32 choices = gabi::ea(radix->values.get()), choice = choices, choiceEnd = choices + choiceCount * 8;
            be<u8>* selected = values[static_cast<u32>(request)];
            if (choice != choiceEnd) {
                u32 index = (cursor - begin) / 0x1C;
                be<s32>* output = &indices->values[index];
                while (choice != choiceEnd) {
                    value->mText = selected;
                    value->mVtable = &sVariantLookupMethods;
                    const auto* text = gabi::at<const sead::SafeText>(choice);
                    if (equalVariantLookupText(text, value.get())) {
                        *output = (choice - choices) >> 3;
                        break;
                    }
                    choice += 8;
                }
            }
            // The first matching name ends the selector search, including a missing value.
            break;
        }
        cursor += 0x1C;
    }
    return resolveVariantSelection(domain, indices->values);
}
/* Shared helpers stay within marked mutation ranges; replay all callers for helper mutations. */
static void assureVariantLookupText(const sead::SafeText* text) {
    const gfn* table = gabi::at<const gfn>(gabi::ea(text->mVtable.get()));
    gabi::call_ptr<void>(table[5].get(), const_cast<sead::SafeText*>(text));
}
static bool boundedVariantTextEqual(const be<u8>* a, const be<u8>* b) {
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 x = a[i], y = b[i];
        if (x != y) return false;
        if (!x) return true;
    }
    return false;
}
static bool equalVariantLookupText(const sead::SafeText* left, const sead::SafeText* right) {
    assureVariantLookupText(left);
    assureVariantLookupText(left);
    be<u8>* cached = left->mText;
    assureVariantLookupText(right);
    if (cached == right->mText.get()) return true;
    return boundedVariantTextEqual(left->mText, right->mText);
}

/* 027BBF6C */
ShaderVariantRadix* ShaderVariantDomain::findDefault(sead::SafeText* query) const {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 keys = count, cursor = gabi::ea(radices.get()), end = cursor + keys * 0x1Cu;
    while (cursor != end) {
        auto* radix = gabi::at<ShaderVariantRadix>(cursor);
        if (equalVariantLookupText(query, &radix->defaultValue)) return radix;
        cursor += 0x1C;
    }
    // The fallback is the canonical eight-byte SafeText identity, not a complete radix record.
    return reinterpret_cast<ShaderVariantRadix*>(&sFileDeviceEmptyText);
}
/* 027BC064 */
void ShaderVariantRadix::destroy(ShaderVariantRadix* record, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (record && (flags & 1)) releaseVariantLookupObject(record);
}
/* 027BD29C */
static void destroyVariantArchiveHandle(VariantArchiveHandle_l* handle, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (handle && (flags & 1)) releaseVariantLookupObject(handle);
}
/* 027BD2B0 */
static void variantArchiveNoop(VariantArchiveHandle_l*) {}
/* WWHD glue
data sVariantLookupMethods 10169C18
ext C0009990 void* clearVariantLookup(void*,u32,u32)
ext 027B950C u32 resolveVariantSelection(const ShaderVariantDomain*,const be<s32>*)
ext 0273AF40 void releaseVariantLookupObject(void*)
*/
