/* Named shader-resource lookup and stage binding from the assigned WWHD operations. */
#include "agl/ShaderProgram.h"
using namespace agl;

struct ResourceMethods_l {
    u8 reserved[0x14];
    gfn prepare;
};
struct ResourceBindings_l {
    gptr<const be<u8>> name;
    gptr<ResourceMethods_l> methods;
    be<u16> locations[3];
    be<u16> reserved;
};
struct AttributeBinding_l {
    gptr<const be<u8>> name;
    gptr<ResourceMethods_l> methods;
    be<u16> location;
    be<u16> reserved;
};
struct UniformVariable_l {
    gptr<const be<u8>> name;
    be<u32> type, count, offset, block;
};
struct SamplerVariable_l {
    gptr<const be<u8>> name;
    be<u32> type, location;
};
struct UniformBlock_l {
    gptr<const be<u8>> name;
    be<u32> offset, size;
};
struct AttributeVariable_l {
    gptr<const be<u8>> name;
    be<u32> type, count, location;
};
/* The table prefixes stop immediately after the count and guest table pointer. */
template <class Entry, u32 Offset> struct ShaderTablePrefix_l {
    u8 reserved[Offset];
    be<u32> count;
    gptr<Entry> entries;
};
WWHD_SIZE(AttributeBinding_l, 12);
WWHD_OFFSET(AttributeBinding_l, location, 8);
WWHD_SIZE(ResourceBindings_l, 16);
WWHD_OFFSET(ResourceBindings_l, name, 0);
WWHD_OFFSET(ResourceBindings_l, methods, 4);
WWHD_OFFSET(ResourceBindings_l, locations, 8);
WWHD_OFFSET(ResourceMethods_l, prepare, 0x14);
WWHD_SIZE(UniformVariable_l, 20);
WWHD_OFFSET(UniformVariable_l, offset, 12);
WWHD_SIZE(SamplerVariable_l, 12);
WWHD_OFFSET(SamplerVariable_l, location, 8);
WWHD_SIZE(UniformBlock_l, 12);
WWHD_OFFSET(UniformBlock_l, offset, 4);
WWHD_SIZE(AttributeVariable_l, 16);
WWHD_OFFSET(AttributeVariable_l, location, 12);
static u32 refreshProgramResources(ShaderProgram*, u32);

static bool refreshDirtyResources(ShaderProgram* program);
template <class Resource> static void prepareResource(Resource* resource);
static bool sameResourceName(const be<u8>* entry, const be<u8>* requested);
template <class Entry, u32 Offset> static const Entry* findShaderResource(void* shader, const be<u8>* name);
static void* shaderForStage(ShaderProgram* program, u32 stage);
static const UniformVariable_l* findUniform(void* shader, u32 stage, const be<u8>* name);
static const SamplerVariable_l* findSampler(void* shader, u32 stage, const be<u8>* name);
static const UniformBlock_l* findUniformBlock(void* shader, u32 stage, const be<u8>* name);
/* 027BD3CC */
static void bindProgramUniform(ProgramResource16_l* storage, ShaderProgram* program) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* resource = reinterpret_cast<ResourceBindings_l*>(storage);
    for (u32 stage = 0; stage < 3; ++stage) {
        refreshDirtyResources(program);
        void* shader = shaderForStage(program, stage);
        const UniformVariable_l* entry = nullptr;
        if (shader) {
            if (refreshDirtyResources(program)) shader = shaderForStage(program, stage);
            prepareResource(resource);
            entry = findUniform(shader, stage, resource->name);
        }
        resource->locations[stage] = entry ? static_cast<u32>(entry->offset) : 0xFFFF;
    }
}
/* Shared helpers stay within marked mutation ranges; replay all callers for helper mutations. */
static bool refreshDirtyResources(ShaderProgram* program) {
    u8 flags = program->flags;
    if (!(flags & 2)) return false;
    program->flags = flags & ~2;
    refreshProgramResources(program, 0);
    return true;
}
template <class Resource> static void prepareResource(Resource* resource) {
    gabi::call_ptr<void>(resource->methods->prepare.get(), resource);
}
static bool sameResourceName(const be<u8>* entry, const be<u8>* requested) {
    for (;;) {
        u8 a = *entry++, b = *requested++;
        if (a != b) return false;
        if (!a) return true;
    }
}
template <class Entry, u32 Offset> static const Entry* findShaderResource(void* shader, const be<u8>* name) {
    const auto* table = reinterpret_cast<const ShaderTablePrefix_l<Entry, Offset>*>(shader);
    u32 count = table->count;
    if (!count) return nullptr;
    const Entry* entries = table->entries;
    for (u32 i = 0; i < count; ++i) {
        const Entry* entry = entries + i;
        if (sameResourceName(entry->name, name)) return entry;
    }
    return nullptr;
}
static void* shaderForStage(ShaderProgram* program, u32 stage) {
    if (stage == 0) return program->vertex.code;
    if (stage == 1) return program->pixel.code;
    return program->geometry.code;
}
static const UniformVariable_l* findUniform(void* shader, u32 stage, const be<u8>* name) {
    if (stage == 0) return findShaderResource<UniformVariable_l, 0xE4>(shader, name);
    if (stage == 1) return findShaderResource<UniformVariable_l, 0xB8>(shader, name);
    return findShaderResource<UniformVariable_l, 0x68>(shader, name);
}
static const SamplerVariable_l* findSampler(void* shader, u32 stage, const be<u8>* name) {
    if (stage == 0) return findShaderResource<SamplerVariable_l, 0xFC>(shader, name);
    if (stage == 1) return findShaderResource<SamplerVariable_l, 0xD0>(shader, name);
    return findShaderResource<SamplerVariable_l, 0x80>(shader, name);
}
static const UniformBlock_l* findUniformBlock(void* shader, u32 stage, const be<u8>* name) {
    if (stage == 0) return findShaderResource<UniformBlock_l, 0xDC>(shader, name);
    if (stage == 1) return findShaderResource<UniformBlock_l, 0xB0>(shader, name);
    return findShaderResource<UniformBlock_l, 0x60>(shader, name);
}

/* 027BD72C */
static void bindProgramSampler(ProgramResource16_l* storage, ShaderProgram* program) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* resource = reinterpret_cast<ResourceBindings_l*>(storage);
    resource->locations[2] = 0xFFFF;
    resource->locations[1] = 0xFFFF;
    resource->locations[0] = 0xFFFF;
    for (u32 stage = 0; stage < 3; ++stage) {
        refreshDirtyResources(program);
        void* shader = shaderForStage(program, stage);
        if (shader) {
            if (refreshDirtyResources(program)) shader = shaderForStage(program, stage);
            prepareResource(resource);
            const SamplerVariable_l* entry = findSampler(shader, stage, resource->name);
            if (entry) resource->locations[stage] = static_cast<u32>(entry->location);
        }
    }
}
/* 027BDA2C */
static void bindProgramUniformBlock(ProgramResource16_l* storage, ShaderProgram* program) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* resource = reinterpret_cast<ResourceBindings_l*>(storage);
    resource->locations[2] = 0xFFFF;
    resource->locations[1] = 0xFFFF;
    resource->locations[0] = 0xFFFF;
    for (u32 stage = 0; stage < 3; ++stage) {
        refreshDirtyResources(program);
        void* shader = shaderForStage(program, stage);
        if (shader) {
            if (refreshDirtyResources(program)) shader = shaderForStage(program, stage);
            prepareResource(resource);
            const UniformBlock_l* entry = findUniformBlock(shader, stage, resource->name);
            if (entry) resource->locations[stage] = static_cast<u32>(entry->offset);
        }
    }
}
/* 027BDD2C */
static void bindProgramAttribute(ProgramResource12_l* storage, ShaderProgram* program) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* resource = reinterpret_cast<AttributeBinding_l*>(storage);
    refreshDirtyResources(program);
    void* shader = program->vertex.code;
    prepareResource(resource);
    const AttributeVariable_l* entry = findShaderResource<AttributeVariable_l, 0x104>(shader, resource->name);
    resource->location = entry ? static_cast<u32>(entry->location) : 0xFFFF;
}
/* WWHD glue
ext 027BB9E0 u32 refreshProgramResources(ShaderProgram*,u32)
*/
