/* Family lifetime and shared-data construction from assigned WWHD bodies. */
#include "agl/ShaderProgram.h"
#include "agl/ShaderArchiveRelocations.h"
#include "agl/ShaderResourceViews.h"
#include "sead/HeapRegistry.h"
using namespace agl;
struct FamilyDestructorMethods_l {
    u8 prefix[12];
    gfn destroy;
};
struct FamilyMethodIdentity_l {
    u8 byte;
};
static FamilyMethodIdentity_l sFamilyDomainMethods;
static FamilyMethodIdentity_l sFamilyProgramMethods;
static sead::SafeTextVtable sFamilyNameMethods;
typedef be<u8> TextByte;
static TextByte sFamilyDefaultName;
using FamilySelectionConstructor_l = ShaderVariantSelection* (*)(ShaderVariantSelection*);
static void* allocateFamilyObject(u32);
static void* allocateFamilyShared(u32, sead::RegistryHeap*, s32);
static void clearFamilyShared(void*, u32);
static void releaseFamilyObject(void*);
static void destroyFamilyConfiguration(ShaderProgramConfiguration_l*, u32);
static void constructFamilySelections(void*, u32, u32, FamilySelectionConstructor_l);
WWHD_OFFSET(FamilyDestructorMethods_l, destroy, 12);

template <class Record> static void releaseFamilyArray(be<u32>& count, gptr<Record>& records);
static void destroyFamilyProgramElement(ShaderProgram* program);
static void destroyFamilyStringElement(sead::SafeText* text);
/* 027BA410 */
void ShaderVariantDomain::destroy(ShaderVariantDomain* domain, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!domain) return;
    ShaderProgram* programs = domain->programs;
    domain->methods = &sFamilyDomainMethods;
    if (programs) {
        u32 index = 0, offset = 0;
        while (static_cast<s32>(index) < static_cast<s32>(static_cast<u32>(domain->programCount))) {
            programs = domain->programs;
            destroyFamilyProgramElement(gabi::at<ShaderProgram>(gabi::ea(programs) + offset));
            ++index;
            offset += 0x84;
        }
        releaseFamilyArray(domain->programCount, domain->programs);
    }
    u32 count = domain->count, cursor = gabi::ea(domain->radices.get()), end = cursor + count * 0x1Cu;
    while (cursor != end) {
        auto* radix = gabi::at<ShaderVariantRadix>(cursor);
        sead::SafeText* values = radix->values;
        if (values) {
            u32 index = 0, offset = 0;
            while (static_cast<s32>(index) < static_cast<s32>(static_cast<u32>(radix->valueCount))) {
                values = radix->values;
                destroyFamilyStringElement(gabi::at<sead::SafeText>(gabi::ea(values) + offset));
                ++index;
                offset += 8;
            }
            releaseFamilyArray(radix->valueCount, radix->values);
        }
        cursor += 0x1C;
    }
    if (domain->radices) releaseFamilyArray(domain->count, domain->radices);
    if (flags & 1) releaseFamilyObject(domain);
}
/* Shared helpers stay within marked mutation ranges; replay all callers for helper mutations. */
template <class Record> static void releaseFamilyArray(be<u32>& count, gptr<Record>& records) {
    // The initial nonnull guard precedes any element destructor callbacks.
    sead::RegistryHeap* heap = findRegistryHeap(sRegistryManager.get(), records.get());
    u32 release = heap->dispatch()->mRelease.mFunction;
    gabi::call_ptr<void>(release, heap, records.get());
    count = 0;
    records = nullptr;
}
static void destroyFamilyProgramElement(ShaderProgram* program) {
    const auto* methods = static_cast<const FamilyDestructorMethods_l*>(program->methods.get());
    gabi::call_ptr<void>(methods->destroy.get(), program, 2u);
}
static void destroyFamilyStringElement(sead::SafeText* text) {
    const auto* methods = reinterpret_cast<const FamilyDestructorMethods_l*>(text->mVtable.get());
    gabi::call_ptr<void>(methods->destroy.get(), text, 2u);
}

/* 027BA5B4 */
void ShaderProgram::destroy(ShaderProgram* program, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!program) return;
    program->methods = &sFamilyProgramMethods;
    reinterpret_cast<ShaderFlags*>(program)->clearDirty();
    program->releaseAttributes();
    program->releaseUniforms();
    program->releaseSamplers();
    program->releaseUniformBlocks();
    if (!static_cast<u16>(program->variantIndex)) {
        auto* shared = static_cast<ShaderProgramSharedData*>(program->binary.get());
        if (shared) {
            ShaderVariantDomain::destroy(&shared->variation, 2);
            releaseFamilyObject(shared);
        }
        program->binary = nullptr;
    }
    destroyFamilyConfiguration(&program->configuration, 2);
    if (flags & 1) releaseFamilyObject(program);
}
/* 027BA674 */
ShaderVariantDomain* ShaderVariantDomain::create(ShaderVariantDomain* domain) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!domain) domain = static_cast<ShaderVariantDomain*>(allocateFamilyObject(0x18));
    if (domain) {
        domain->root = nullptr;
        domain->methods = &sFamilyDomainMethods;
        domain->programs = nullptr;
        domain->programCount = 0;
        domain->radices = nullptr;
        domain->count = 0;
    }
    return domain;
}
/* 027BA710 */
void ShaderProgram::initializeSharedData(const sead::SafeText* source, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* shared = static_cast<ShaderProgramSharedData*>(allocateFamilyShared(0x44, heap, 4));
    if (shared) {
        clearFamilyShared(shared, 0x44);
        shared->name.mVtable = &sFamilyNameMethods;
        shared->name.mText = &sFamilyDefaultName;
        ShaderVariantDomain::create(&shared->variation);
        shared->reserved[0] = 0;
        shared->reserved[1] = 0;
        constructFamilySelections(shared->selections, 4, 4, ShaderVariantSelection::create);
    }
    binary = shared;
    // The original continues after allocation failure; no new null-return path is added.
    *gabi::at<be<u32>>(gabi::ea(shared) + 0x20) = 0;
    shared = static_cast<ShaderProgramSharedData*>(binary.get());
    shared->name.mText = source->mText;
    shared = static_cast<ShaderProgramSharedData*>(binary.get());
    shared->status = 0;
    shared = static_cast<ShaderProgramSharedData*>(binary.get());
    shared->variation.root = this;
    shared = static_cast<ShaderProgramSharedData*>(binary.get());
    shared->mode = 3;
    shared = static_cast<ShaderProgramSharedData*>(binary.get());
    configuration.archiveName = shared->name.mText;
    for (u32 i = 0; i < 4; ++i)
        shared->selections[i].selection = 0;
    shared = static_cast<ShaderProgramSharedData*>(binary.get());
    shared->reserved[0] = 0;
    shared = static_cast<ShaderProgramSharedData*>(binary.get());
    shared->reserved[1] = 0;
}
/* WWHD glue
data sFamilyDomainMethods 10169CE8
data sFamilyProgramMethods 10169CD8
data sFamilyNameMethods 10169C18
data sFamilyDefaultName 10000160
ext 0273AD10 void* allocateFamilyObject(u32)
ext 0273B050 void* allocateFamilyShared(u32,sead::RegistryHeap*,s32)
ext 028F521C void clearFamilyShared(void*,u32)
ext 0273AF40 void releaseFamilyObject(void*)
ext 027C06C4 void destroyFamilyConfiguration(ShaderProgramConfiguration_l*,u32)
ext 028EFFD0 void constructFamilySelections(void*,u32,u32,FamilySelectionConstructor_l)
*/
