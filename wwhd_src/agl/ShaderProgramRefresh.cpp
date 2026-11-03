/* Program variation selection and refresh from assigned WWHD bodies. */
#include "agl/ShaderProgram.h"
#include "agl/ShaderArchiveRelocations.h"
#include "agl/ShaderStageResource.h"
using namespace agl;
struct VariantTextList_l {
    gptr<be<u8>> text[32];
};
struct ProgramRefreshObserverMethods_l {
    u8 prefix[12];
    gfn notify;
};
struct ProgramRefreshObserver_l {
    gptr<ProgramRefreshObserverMethods_l> methods;
};
using ProgramRefreshBinary_l = ShaderProgramSharedData;
struct ProgramRefreshConfiguration_l {
    gptr<void> context;
    u8 reserved[4];
    be<u32> bindingState;
    u8 tail[16];
};
struct ProgramRefreshManager_l {
    gptr<void> value;
};
static ProgramRefreshManager_l sProgramRefreshManager;
WWHD_SIZE(VariantTextList_l, 0x80);
WWHD_OFFSET(ProgramRefreshBinary_l, variation, 8);
WWHD_OFFSET(ProgramRefreshBinary_l, observer, 0x20);
WWHD_OFFSET(ProgramRefreshBinary_l, mode, 0x28);
WWHD_SIZE(ProgramRefreshConfiguration_l, 0x1C);
WWHD_OFFSET(ProgramRefreshConfiguration_l, bindingState, 8);
WWHD_OFFSET(ProgramRefreshObserverMethods_l, notify, 12);
static void resetShaderVariations(void*);
static void selectShaderVariation(void*, const be<u8>*, const be<u8>*);
static u32 refreshShaderStage(ShaderStage_l*, u32, u32);
static void beginProgramRefresh(void*);
static void endProgramRefresh(void*);
static void beginProgramConfiguration(ProgramRefreshConfiguration_l*, u32);
static void endProgramConfiguration(ProgramRefreshConfiguration_l*, u32);
static u32 refreshProgramResources(ShaderProgram*, u32);

static void terminateVariantText(sead::SafeText* text);
static ProgramRefreshBinary_l* refreshBinary(ShaderProgram* p);
/* 027BB630 */
static u32 collectVariantTexts(ShaderVariantDomain* domain, s32 selection, gptr<be<u8>>* names, gptr<be<u8>>* values) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = domain->count, begin = gabi::ea(domain->radices.get());
    u32 cursor = begin, end = begin + count * 0x1Cu, remaining = static_cast<u32>(selection);
    while (cursor != end) {
        auto* radix = gabi::at<ShaderVariantRadix>(cursor);
        u32 divisor = radix->divisor, quotient = ppc_divw(remaining, divisor), index = (cursor - begin) / 0x1C;
        terminateVariantText(&radix->name);
        names[index] = radix->name.mText;
        u32 valueCount = radix->valueCount;
        sead::SafeText* value = radix->values;
        if (quotient < valueCount) value = gabi::at<sead::SafeText>(gabi::ea(value) + quotient * 8);
        terminateVariantText(value);
        values[index] = value->mText;
        divisor = radix->divisor;
        remaining -= quotient * divisor;
        cursor += 0x1C;
    }
    return domain->count;
}
/* Shared helpers stay within marked mutation ranges; replay all callers for helper mutations. */
static void terminateVariantText(sead::SafeText* text) {
    const gfn* methods = gabi::at<const gfn>(gabi::ea(text->mVtable.get()));
    gabi::call_ptr<void>(methods[5].get(), text);
}
static ProgramRefreshBinary_l* refreshBinary(ShaderProgram* p) {
    return static_cast<ProgramRefreshBinary_l*>(p->binary.get());
}

/* 027BB710 */
static void selectProgramVariations(ShaderProgram* program) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<VariantTextList_l> names(8), values(0x88);
    ProgramRefreshBinary_l* binary = refreshBinary(program);
    ShaderProgram* primary = binary->variation.root;
    auto* domain = gabi::at<ShaderVariantDomain>(gabi::ea(binary) + 8);
    u32 count = collectVariantTexts(domain, static_cast<u16>(program->variantIndex), names->text, values->text);
    for (u32 stage = 0; stage < 3; ++stage) {
        const ShaderStage_l* source = static_cast<const ShaderProgram*>(primary)->stage(stage);
        void* variations = source->variations;
        if (!variations) continue;
        resetShaderVariations(variations);
        if (static_cast<s32>(count) > 0)
            for (u32 i = 0; i < count; ++i)
                selectShaderVariation(variations, names->text[i], values->text[i]);
        ShaderStage_l* destination =
            const_cast<ShaderStage_l*>(static_cast<const ShaderProgram*>(program)->stage(stage));
        destination->variations = variations;
    }
}
/* 027BB9E0 */
static u32 refreshProgramResources(ShaderProgram* program, u32 unused) {
    gabi::AutoFrame frame{gabi::current_function()};
    u8 initial = program->flags;
    u32 wasActive = initial & 1, forceRefresh = (initial >> 3) & 1, result = 0;
    selectProgramVariations(program);
    if (refreshShaderStage(&program->vertex, wasActive, forceRefresh) != 0)
        result = 1;
    else if (refreshShaderStage(&program->pixel, wasActive, forceRefresh) != 0)
        result = 2;
    else if (refreshShaderStage(&program->geometry, wasActive, forceRefresh) == 1)
        result = 3;
    if (!result) {
        void* geometry = program->geometry.code;
        ProgramRefreshBinary_l* binary = refreshBinary(program);
        u32 mode = 2;
        if (!geometry) mode = static_cast<const SerializedVertexShader_l*>(program->vertex.code.get())->mode;
        binary->mode = mode;
        auto* configuration = reinterpret_cast<ProgramRefreshConfiguration_l*>(&program->configuration);
        if (configuration->context) {
            if (!wasActive) {
                beginProgramRefresh(sProgramRefreshManager.value);
                beginProgramConfiguration(configuration, 0);
                program->bindStagePrograms();
                endProgramConfiguration(configuration, 0);
                endProgramRefresh(sProgramRefreshManager.value);
            } else
                configuration->bindingState = 0;
        }
        program->flags = (static_cast<u8>(program->flags) | 1) & 0xF7;
    }
    if (static_cast<u8>(program->flags) & 1) {
        program->bindUniforms();
        program->bindUniformBlocks();
        program->bindAttributes();
        program->bindSamplers();
    }
    ProgramRefreshObserver_l* observer = static_cast<ProgramRefreshObserver_l*>(refreshBinary(program)->observer.get());
    if (observer) gabi::call_ptr<void>(observer->methods->notify.get(), observer, program);
    return result;
}
/* 027BBB9C */
static u32 refreshProgramFamily(ShaderProgram* program) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 binaryAddress = gabi::ea(program->binary.get());
    if (binaryAddress + 8u != 0) {
        auto* binary = gabi::at<ProgramRefreshBinary_l>(binaryAddress);
        ShaderProgram* primary = binary->variation.root;
        if (static_cast<u8>(primary->flags) & 2) {
            primary->flags = static_cast<u8>(primary->flags) & ~2;
            u32 result = refreshProgramResources(primary, 0);
            if (result) return result;
            binary = refreshBinary(program);
        }
        u32 count = binary->variation.programCount, cursor = gabi::ea(binary->variation.programs.get()),
            end = cursor + count * 0x84u;
        while (cursor != end) {
            auto* variant = gabi::at<ShaderProgram>(cursor);
            if (static_cast<u8>(variant->flags) & 2) {
                variant->flags = static_cast<u8>(variant->flags) & ~2;
                u32 result = refreshProgramResources(variant, 0);
                if (result) return result;
            }
            cursor += 0x84;
        }
        return 0;
    }
    if (!(static_cast<u8>(program->flags) & 2)) return 0;
    program->flags = static_cast<u8>(program->flags) & ~2;
    return refreshProgramResources(program, 0);
}
/* WWHD glue
data sProgramRefreshManager 101F8E04
ext 027D3FC4 void resetShaderVariations(void*)
ext 027D3FD4 void selectShaderVariation(void*,const be<u8>*,const be<u8>*)
ext 027D4D30 u32 refreshShaderStage(ShaderStage_l*,u32,u32)
ext 027B736C void beginProgramRefresh(void*)
ext 027B737C void endProgramRefresh(void*)
ext 027C073C void beginProgramConfiguration(ProgramRefreshConfiguration_l*,u32)
ext 027C077C void endProgramConfiguration(ProgramRefreshConfiguration_l*,u32)
*/
