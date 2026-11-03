/* Local shader archive record views from the assigned WWHD relocation routines. */
#pragma once
#include "wwhd.h"
#include "sead/HeapRegistry.h"
#include "sead/FileDevice.h"
namespace agl {
struct ShaderProgram;
struct BlockNameRecord_l {
    be<u32> relativeName;
    u8 payload[8];
};
struct ValueNameRecord_l {
    be<u32> relativeName;
    u8 payload[16];
};
struct SamplerNameRecord_l {
    be<u32> relativeName;
    u8 payload[8];
};
struct AttributeNameRecord_l {
    be<u32> relativeName;
    u8 payload[12];
};
struct ShaderArchiveRelocations {
    static const char* archiveExtension();
    static const char* binaryExtension();
    static BlockNameRecord_l* relocateBlockNames(void* base, u32 offset, s32 count);
    static ValueNameRecord_l* relocateValueNames(void* base, u32 offset, s32 count);
    static SamplerNameRecord_l* relocateSamplerNames(void* base, u32 offset, s32 count);
    static AttributeNameRecord_l* relocateAttributeNames(void* base, u32 offset, s32 count);
};
struct ShaderVariantSelection {
    be<u32> selection;
    static ShaderVariantSelection* create(ShaderVariantSelection*);
};
struct ShaderVariantRadix {
    sead::SafeText name, defaultValue;
    be<u32> valueCount;
    gptr<sead::SafeText> values;
    be<u16> divisor;
    u8 tail[2];
    static void destroy(ShaderVariantRadix*, u32 flags);
};
struct ShaderVariantDomain {
    gptr<ShaderProgram> root;
    be<u32> programCount;
    gptr<ShaderProgram> programs;
    be<u32> count;
    gptr<ShaderVariantRadix> radices;
    gptr<const void> methods;
    static ShaderVariantDomain* create(ShaderVariantDomain*);
    static void destroy(ShaderVariantDomain*, u32 flags);
    void allocateRadices(s32 count, sead::RegistryHeap*);
    void configureRadix(
        u32 index, const sead::SafeText* name, const sead::SafeText* defaultValue, s32 count, sead::RegistryHeap*);
    void createVariants(sead::RegistryHeap*);
    ShaderVariantRadix* findDefault(sead::SafeText*) const;
    void decompose(s32 index, be<s32>* output) const;
    void setValue(u32 variant, u32 value, const sead::SafeText* source);
};
struct ShaderProgramSharedData {
    sead::SafeText name;
    ShaderVariantDomain variation;
    gptr<void> observer;
    be<u32> status, mode, reserved[2];
    ShaderVariantSelection selections[4];
};
struct ShaderStageTags {
    static void prepare();
    static u32 vertexKind();
    static u32 pixelKind();
    static u32 geometryKind();
};
WWHD_SIZE(BlockNameRecord_l, 12);
WWHD_OFFSET(BlockNameRecord_l, relativeName, 0);
WWHD_SIZE(ValueNameRecord_l, 20);
WWHD_OFFSET(ValueNameRecord_l, relativeName, 0);
WWHD_SIZE(SamplerNameRecord_l, 12);
WWHD_OFFSET(SamplerNameRecord_l, relativeName, 0);
WWHD_SIZE(AttributeNameRecord_l, 16);
WWHD_OFFSET(AttributeNameRecord_l, relativeName, 0);
WWHD_SIZE(ShaderVariantSelection, 4);
WWHD_SIZE(ShaderVariantRadix, 0x1C);
WWHD_OFFSET(ShaderVariantRadix, name, 0);
WWHD_OFFSET(ShaderVariantRadix, defaultValue, 8);
WWHD_OFFSET(ShaderVariantRadix, valueCount, 0x10);
WWHD_OFFSET(ShaderVariantRadix, values, 0x14);
WWHD_OFFSET(ShaderVariantRadix, divisor, 0x18);
WWHD_SIZE(ShaderVariantDomain, 0x18);
WWHD_OFFSET(ShaderVariantDomain, root, 0);
WWHD_OFFSET(ShaderVariantDomain, programCount, 4);
WWHD_OFFSET(ShaderVariantDomain, programs, 8);
WWHD_OFFSET(ShaderVariantDomain, methods, 0x14);
WWHD_SIZE(ShaderProgramSharedData, 0x44);
WWHD_OFFSET(ShaderProgramSharedData, name, 0);
WWHD_OFFSET(ShaderProgramSharedData, variation, 8);
WWHD_OFFSET(ShaderProgramSharedData, observer, 0x20);
WWHD_OFFSET(ShaderProgramSharedData, status, 0x24);
WWHD_OFFSET(ShaderProgramSharedData, mode, 0x28);
WWHD_OFFSET(ShaderProgramSharedData, selections, 0x34);
WWHD_OFFSET(ShaderVariantDomain, count, 0xC);
WWHD_OFFSET(ShaderVariantDomain, radices, 0x10);
}
