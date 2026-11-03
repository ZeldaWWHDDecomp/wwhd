/* Program stage selection and fixed-end resource traversals from WWHD. */
#include "agl/ShaderProgram.h"
#include "agl/ShaderArchiveRelocations.h"
using namespace agl;
struct ProgramEnginePointer_l {
    gptr<void> value;
};
static ProgramEnginePointer_l sProgramEngine;
static void synchronizeProgramEngine(void*);
static void selectProgramMode(void*, u32);
static void bindVertexShader(void*);
static void bindPixelShader(void*);
static void bindGeometryShader(void*);
static void bindProgramUniform(ProgramResource16_l*, ShaderProgram*);
static void bindProgramUniformBlock(ProgramResource16_l*, ShaderProgram*);
static void bindProgramAttribute(ProgramResource12_l*, ShaderProgram*);
static void bindProgramSampler(ProgramResource16_l*, ShaderProgram*);
/* 027BAC58 */
ShaderStage_l* ShaderProgram::stage(u32 kind) {
    if (kind == 0) return &vertex;
    if (kind == 1) return &pixel;
    if (kind == 2) return &geometry;
    return nullptr;
}
/* 027BAC8C */
const ShaderStage_l* ShaderProgram::stage(u32 kind) const {
    if (kind == 0) return &vertex;
    if (kind == 1) return &pixel;
    if (kind == 2) return &geometry;
    return nullptr;
}
/* 027BB7CC */
void ShaderProgram::bindStagePrograms() {
    gabi::AutoFrame frame{gabi::current_function()};
    const auto* shared = static_cast<const ShaderProgramSharedData*>(binary.get());
    if (static_cast<u32>(shared->mode) == 2) synchronizeProgramEngine(sProgramEngine.value);
    void* code = vertex.code;
    if (code) bindVertexShader(code);
    code = pixel.code;
    if (code) bindPixelShader(code);
    code = geometry.code;
    if (code) bindGeometryShader(code);
}
/* 027BB840 */
void ShaderProgram::bindUniforms() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = uniformCount;
    u32 cursor = gabi::ea(uniforms.get()), end = cursor + count * 16;
    while (cursor != end) {
        bindProgramUniform(gabi::at<ProgramResource16_l>(cursor), this);
        cursor += 16;
    }
}
/* 027BB8A8 */
void ShaderProgram::bindUniformBlocks() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = uniformBlockCount;
    u32 cursor = gabi::ea(uniformBlocks.get()), end = cursor + count * 16;
    while (cursor != end) {
        bindProgramUniformBlock(gabi::at<ProgramResource16_l>(cursor), this);
        cursor += 16;
    }
}
/* 027BB910 */
void ShaderProgram::bindAttributes() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = attributeCount;
    u32 cursor = gabi::ea(attributes.get()), end = cursor + count * 12;
    while (cursor != end) {
        bindProgramAttribute(gabi::at<ProgramResource12_l>(cursor), this);
        cursor += 12;
    }
}
/* 027BB978 */
void ShaderProgram::bindSamplers() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = samplerCount;
    u32 cursor = gabi::ea(samplers.get()), end = cursor + count * 16;
    while (cursor != end) {
        bindProgramSampler(gabi::at<ProgramResource16_l>(cursor), this);
        cursor += 16;
    }
}
/* 027BAA9C */
void ShaderProgram::setVariantValue(u32 variant, u32 value, const sead::SafeText* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto* shared = static_cast<ShaderProgramSharedData*>(binary.get());
    shared->variation.setValue(variant, value, source);
}
/* 027B9F68 */
void agl::setProgramShaderMode(u32 mode) {
    gabi::AutoFrame frame{gabi::current_function()};
    selectProgramMode(sProgramEngine.value, mode);
}
/* WWHD glue
data sProgramEngine 101F8BD8
ext 0276B0D4 void synchronizeProgramEngine(void*)
ext 0276B0CC void selectProgramMode(void*,u32)
ext C0006A28 void bindVertexShader(void*)
ext C00068F0 void bindPixelShader(void*)
ext C0006888 void bindGeometryShader(void*)
ext 027BD3CC void bindProgramUniform(ProgramResource16_l*,ShaderProgram*)
ext 027BDA2C void bindProgramUniformBlock(ProgramResource16_l*,ShaderProgram*)
ext 027BDD2C void bindProgramAttribute(ProgramResource12_l*,ShaderProgram*)
ext 027BD72C void bindProgramSampler(ProgramResource16_l*,ShaderProgram*)
*/
