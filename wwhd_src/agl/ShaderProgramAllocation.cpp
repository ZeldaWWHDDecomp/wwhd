/* Resource-array allocation and propagation from assigned WWHD program operations. */
#include "agl/ShaderProgram.h"
#include "sead/HeapRegistry.h"
using namespace agl;
struct ProgramResourceName_l {
    u8 first;
};
struct ProgramResourceMethods_l {
    u8 first;
};
struct AttributeAllocation_l {
    gptr<const void> name, methods;
    be<u16> location;
    u8 padding[2];
};
struct StageResourceAllocation_l {
    gptr<const void> name, methods;
    be<u16> vertex, pixel, geometry, extra;
};
struct ProgramVariants_l {
    u8 prefix[12];
    be<u32> count;
    gptr<ShaderProgram> programs;
};
WWHD_SIZE(AttributeAllocation_l, 12);
WWHD_OFFSET(AttributeAllocation_l, location, 8);
WWHD_SIZE(StageResourceAllocation_l, 16);
WWHD_OFFSET(StageResourceAllocation_l, pixel, 10);
WWHD_OFFSET(StageResourceAllocation_l, geometry, 12);
WWHD_OFFSET(StageResourceAllocation_l, extra, 14);
WWHD_OFFSET(ProgramVariants_l, count, 12);
WWHD_OFFSET(ProgramVariants_l, programs, 16);
/* Image-backed objects are borrowed without initializers. Only their addresses are used. */
static ProgramResourceName_l sAttributeName;
static ProgramResourceName_l sUniformName;
static ProgramResourceName_l sSamplerName;
static ProgramResourceName_l sUniformBlockName;
static ProgramResourceMethods_l sResourceMethods;

enum class ResourceArrayKind { Attribute, Uniform, Sampler, UniformBlock };

static void* allocateResourceStorage(sead::RegistryHeap* requested, u32 bytes);
template <class Record>
static void initializeResourceStorage(void* storage, u32 count, const ProgramResourceName_l* name);
static void storeResourceStorage(ShaderProgram* program, ResourceArrayKind kind, void* storage, u32 count);
template <class Record>
static void allocateOneResourceArray(ShaderProgram* program, u32 count, sead::RegistryHeap* heap,
    ResourceArrayKind kind, const ProgramResourceName_l* name);
template <class Record>
static void allocateProgramResourceArrays(ShaderProgram* program, s32 count, sead::RegistryHeap* heap,
    ResourceArrayKind kind, const ProgramResourceName_l* name);
/* 027BACC0 */
static void allocateProgramAttributes(ShaderProgram* program, s32 count, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    allocateProgramResourceArrays<AttributeAllocation_l>(
        program, count, heap, ResourceArrayKind::Attribute, &sAttributeName);
}
/* Shared helpers stay within marked mutation ranges; replay all callers for helper mutations. */
static void* allocateResourceStorage(sead::RegistryHeap* requested, u32 bytes) {
    sead::RegistryHeap* heap = requested;
    if (!heap) heap = registryCurrentHeap(sRegistryManager.get());
    return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction.get(), heap, bytes, 4u);
}
template <class Record>
static void initializeResourceStorage(void* storage, u32 count, const ProgramResourceName_l* name) {
    u32 base = gabi::ea(storage), offset = 0;
    for (u32 remaining = count; remaining; --remaining, offset += sizeof(Record)) {
        u32 address = base + offset;
        if (!address) continue;
        Record* entry = gabi::at<Record>(address);
        entry->name = name;
        entry->methods = &sResourceMethods;
        if constexpr (sizeof(Record) == 12) {
            entry->location = 0;
        } else {
            entry->pixel = 0xFFFF;
            entry->geometry = 0xFFFF;
            entry->vertex = 0xFFFF;
            entry->extra = 0xFFFF;
        }
    }
}
static void storeResourceStorage(ShaderProgram* program, ResourceArrayKind kind, void* storage, u32 count) {
    if (kind == ResourceArrayKind::Attribute) {
        program->attributes = static_cast<ProgramResource12_l*>(storage);
        program->attributeCount = count;
    } else if (kind == ResourceArrayKind::Uniform) {
        program->uniforms = static_cast<ProgramResource16_l*>(storage);
        program->uniformCount = count;
    } else if (kind == ResourceArrayKind::Sampler) {
        program->samplers = static_cast<ProgramResource16_l*>(storage);
        program->samplerCount = count;
    } else {
        program->uniformBlocks = static_cast<ProgramResource16_l*>(storage);
        program->uniformBlockCount = count;
    }
}
template <class Record>
static void allocateOneResourceArray(ShaderProgram* program, u32 count, sead::RegistryHeap* heap,
    ResourceArrayKind kind, const ProgramResourceName_l* name) {
    void* storage = allocateResourceStorage(heap, count * static_cast<u32>(sizeof(Record)));
    initializeResourceStorage<Record>(storage, count, name);
    if (storage) storeResourceStorage(program, kind, storage, count);
}
template <class Record>
static void allocateProgramResourceArrays(ShaderProgram* program, s32 count, sead::RegistryHeap* heap,
    ResourceArrayKind kind, const ProgramResourceName_l* name) {
    if (count > 0) allocateOneResourceArray<Record>(program, static_cast<u32>(count), heap, kind, name);
    const auto* variants = reinterpret_cast<const ProgramVariants_l*>(program->binary.get());
    u32 variantCount = variants->count;
    u32 cursor = gabi::ea(variants->programs.get()), end = cursor + variantCount * 0x84u;
    while (cursor != end) {
        if (count > 0)
            allocateOneResourceArray<Record>(
                gabi::at<ShaderProgram>(cursor), static_cast<u32>(count), heap, kind, name);
        cursor += 0x84;
    }
}

/* 027BAEF8 */
static void allocateProgramUniforms(ShaderProgram* program, s32 count, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    allocateProgramResourceArrays<StageResourceAllocation_l>(
        program, count, heap, ResourceArrayKind::Uniform, &sUniformName);
}
/* 027BB160 */
static void allocateProgramSamplers(ShaderProgram* program, s32 count, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    allocateProgramResourceArrays<StageResourceAllocation_l>(
        program, count, heap, ResourceArrayKind::Sampler, &sSamplerName);
}
/* 027BB3C8 */
static void allocateProgramUniformBlocks(ShaderProgram* program, s32 count, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    allocateProgramResourceArrays<StageResourceAllocation_l>(
        program, count, heap, ResourceArrayKind::UniformBlock, &sUniformBlockName);
}
/* WWHD glue
data sAttributeName 10169CA8
data sUniformName 10169CB4
data sSamplerName 10169CC0
data sUniformBlockName 10169CCC
data sResourceMethods 10169C18
*/
