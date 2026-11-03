/* Relative archive names and shader stage tags recovered from WWHD instructions. */
#include "agl/ShaderArchiveRelocations.h"
using namespace agl;
static void* allocateShaderVariant(u32);

template <class Record> static Record* relocateNames(void* base, u32 offset, s32 count);
static const char* archiveLiteral(const char* text);
/* 027BD2B4 */
const char* ShaderArchiveRelocations::archiveExtension() {
    return archiveLiteral("sharc");
}
/* Shared helpers stay within marked mutation ranges; replay all callers for helper mutations. */
static const char* archiveLiteral(const char* text) {
    return gabi::at<const char>(gabi::guest_addr(text));
}

/* 027BD2C0 */
const char* ShaderArchiveRelocations::binaryExtension() {
    return archiveLiteral("sharcfb");
}
/* 027BD2CC */
BlockNameRecord_l* ShaderArchiveRelocations::relocateBlockNames(void* base, u32 offset, s32 count) {
    return relocateNames<BlockNameRecord_l>(base, offset, count);
}
/* The same relative-name conversion is inlined in the four record specializations. */
template <class Record> static Record* relocateNames(void* base, u32 offset, s32 count) {
    if (!offset) return nullptr;
    Record* records = gabi::at<Record>(gabi::ea(base) + offset);
    for (s32 i = 0; i < count; ++i)
        records[i].relativeName = static_cast<u32>(records[i].relativeName) + gabi::ea(base);
    return records;
}
/* 027BD30C */
ValueNameRecord_l* ShaderArchiveRelocations::relocateValueNames(void* base, u32 offset, s32 count) {
    return relocateNames<ValueNameRecord_l>(base, offset, count);
}
/* 027BD34C */
SamplerNameRecord_l* ShaderArchiveRelocations::relocateSamplerNames(void* base, u32 offset, s32 count) {
    return relocateNames<SamplerNameRecord_l>(base, offset, count);
}
/* 027BD38C */
AttributeNameRecord_l* ShaderArchiveRelocations::relocateAttributeNames(void* base, u32 offset, s32 count) {
    return relocateNames<AttributeNameRecord_l>(base, offset, count);
}
/* 027BC094 */
ShaderVariantSelection* ShaderVariantSelection::create(ShaderVariantSelection* selection) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!selection)
        selection = static_cast<ShaderVariantSelection*>(allocateShaderVariant(sizeof(ShaderVariantSelection)));
    if (selection) selection->selection = 0;
    return selection;
}
/* 027BC078 */
void ShaderStageTags::prepare() {}
/* 027BC07C */
u32 ShaderStageTags::vertexKind() {
    return 0;
}
/* 027BC084 */
u32 ShaderStageTags::pixelKind() {
    return 1;
}
/* 027BC08C */
u32 ShaderStageTags::geometryKind() {
    return 2;
}
/* 027BC0F4 */
void ShaderVariantDomain::decompose(s32 index, be<s32>* output) const {
    u32 cursor = gabi::ea(radices.get());
    u32 end = cursor + static_cast<u32>(count) * 0x1C;
    while (cursor != end) {
        const ShaderVariantRadix* radix = gabi::at<ShaderVariantRadix>(cursor);
        s32 divisor = radix->divisor;
        s32 quotient = divisor ? index / divisor : (index < 0 ? -1 : 0);
        *output++ = quotient;
        u32 reloadedDivisor = radix->divisor;
        cursor += 0x1C;
        index = static_cast<s32>(static_cast<u32>(index) - static_cast<u32>(quotient) * reloadedDivisor);
    }
}
/* 027BAA60 */
void ShaderVariantDomain::setValue(u32 variant, u32 value, const sead::SafeText* source) {
    u32 variants = count;
    u32 record = gabi::ea(radices.get());
    if (variant < variants) record += variant * 0x1C;
    ShaderVariantRadix* radix = gabi::at<ShaderVariantRadix>(record);
    u32 values = radix->valueCount;
    u32 destination = gabi::ea(radix->values.get());
    if (value < values) destination += value * 8;
    gabi::at<sead::SafeText>(destination)->mText = source->mText;
}
/* WWHD glue
str 10169D00 "sharc" in 027BD2B4
str 10169CF8 "sharcfb" in 027BD2C0
ext 0273AD10 void* allocateShaderVariant(u32)
*/
