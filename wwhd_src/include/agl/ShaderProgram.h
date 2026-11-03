/* Local program/resource prefixes from the assigned WWHD program operations. */
#pragma once
#include "wwhd.h"
#include "sead/HeapRegistry.h"
#include "sead/FileDevice.h"
namespace agl {
void setProgramShaderMode(u32 mode);
struct ShaderProgramConfiguration_l {
    u8 prefix[0xC];
    gptr<be<u8>> archiveName;
    u8 reserved[0xC];
};
WWHD_SIZE(ShaderProgramConfiguration_l, 0x1C);
WWHD_OFFSET(ShaderProgramConfiguration_l, archiveName, 0xC);
struct ShaderStage_l {
    gptr<void> code, variations;
    u8 reserved[8];
    gptr<const void> methods;
};
struct ProgramResource12_l {
    u8 bytes[12];
};
struct ProgramResource16_l {
    u8 bytes[16];
};
struct ShaderProgram {
    be<u8> flags;
    u8 reserved;
    be<u16> variantIndex;
    ShaderProgramConfiguration_l configuration;
    be<u32> attributeCount;
    gptr<ProgramResource12_l> attributes;
    be<u32> uniformCount;
    gptr<ProgramResource16_l> uniforms;
    be<u32> uniformBlockCount;
    gptr<ProgramResource16_l> uniformBlocks;
    be<u32> samplerCount;
    gptr<ProgramResource16_l> samplers;
    ShaderStage_l vertex, pixel, geometry;
    gptr<void> binary;
    gptr<const void> methods;
    ShaderStage_l* stage(u32 kind);
    const ShaderStage_l* stage(u32 kind) const;
    static ShaderProgram* create(ShaderProgram*);
    static void destroy(ShaderProgram*, u32 flags);
    void initializeSharedData(const sead::SafeText*, sead::RegistryHeap*);
    void allocateVariantKeys(s32 count, sead::RegistryHeap*);
    void configureVariantKey(
        u32 index, const sead::SafeText* name, const sead::SafeText* defaultValue, s32 count, sead::RegistryHeap*);
    void createVariantPrograms(sead::RegistryHeap*);
    void releaseAttributes();
    void releaseUniforms();
    void releaseSamplers();
    void releaseUniformBlocks();
    void markResourceDirty();
    void setAttributeName(u32 index, const sead::SafeText*);
    void setUniformName(u32 index, const sead::SafeText*);
    void setSamplerName(u32 index, const sead::SafeText*);
    void setUniformBlockName(u32 index, const sead::SafeText*);
    void bindStagePrograms();
    void setVariantValue(u32 variant, u32 value, const sead::SafeText* source);
    void bindUniforms();
    void bindUniformBlocks();
    void bindAttributes();
    void bindSamplers();
};
WWHD_SIZE(ShaderStage_l, 0x14);
WWHD_OFFSET(ShaderStage_l, code, 0);
WWHD_OFFSET(ShaderStage_l, variations, 4);
WWHD_OFFSET(ShaderStage_l, methods, 0x10);
WWHD_SIZE(ProgramResource12_l, 12);
WWHD_SIZE(ProgramResource16_l, 16);
WWHD_SIZE(ShaderProgram, 0x84);
WWHD_OFFSET(ShaderProgram, flags, 0);
WWHD_OFFSET(ShaderProgram, variantIndex, 2);
WWHD_OFFSET(ShaderProgram, configuration, 4);
WWHD_OFFSET(ShaderProgram, attributeCount, 0x20);
WWHD_OFFSET(ShaderProgram, attributes, 0x24);
WWHD_OFFSET(ShaderProgram, uniformCount, 0x28);
WWHD_OFFSET(ShaderProgram, uniforms, 0x2C);
WWHD_OFFSET(ShaderProgram, uniformBlockCount, 0x30);
WWHD_OFFSET(ShaderProgram, uniformBlocks, 0x34);
WWHD_OFFSET(ShaderProgram, samplerCount, 0x38);
WWHD_OFFSET(ShaderProgram, samplers, 0x3C);
WWHD_OFFSET(ShaderProgram, vertex, 0x40);
WWHD_OFFSET(ShaderProgram, pixel, 0x54);
WWHD_OFFSET(ShaderProgram, geometry, 0x68);
WWHD_OFFSET(ShaderProgram, binary, 0x7C);
WWHD_OFFSET(ShaderProgram, methods, 0x80);
}
