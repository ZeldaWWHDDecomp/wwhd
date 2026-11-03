/* Resource-name propagation from the assigned WWHD fixed-end program traversals. */
#include "agl/ShaderProgram.h"
#include "agl/ShaderArchiveRelocations.h"
using namespace agl;
/* 027BAE8C */
void ShaderProgram::setAttributeName(u32 index, const sead::SafeText* source) {
    u32 count = attributeCount;
    u32 destination = gabi::ea(attributes.get());
    be<u8>* name = source->mText;
    if (index < count) destination += index * 12;
    gabi::at<sead::SafeText>(destination)->mText = name;
    const auto* shared = static_cast<const ShaderProgramSharedData*>(binary.get());
    u32 programs = shared->variation.programCount;
    u32 cursor = gabi::ea(shared->variation.programs.get()), end = cursor + programs * 0x84;
    while (cursor != end) {
        const ShaderProgram* program = gabi::at<const ShaderProgram>(cursor);
        u32 records = program->attributeCount;
        u32 target = gabi::ea(program->attributes.get());
        be<u8>* text = source->mText;
        if (index < records) target += index * 12;
        cursor += 0x84;
        gabi::at<sead::SafeText>(target)->mText = text;
    }
}
/* 027BB0EC */
void ShaderProgram::setUniformName(u32 index, const sead::SafeText* source) {
    u32 count = uniformCount;
    u32 destination = gabi::ea(uniforms.get());
    be<u8>* name = source->mText;
    if (index < count) destination += index * 16;
    gabi::at<sead::SafeText>(destination)->mText = name;
    const auto* shared = static_cast<const ShaderProgramSharedData*>(binary.get());
    u32 programs = shared->variation.programCount;
    u32 cursor = gabi::ea(shared->variation.programs.get()), end = cursor + programs * 0x84;
    while (cursor != end) {
        const ShaderProgram* program = gabi::at<const ShaderProgram>(cursor);
        u32 records = program->uniformCount;
        u32 target = gabi::ea(program->uniforms.get());
        be<u8>* text = source->mText;
        if (index < records) target += index * 16;
        cursor += 0x84;
        gabi::at<sead::SafeText>(target)->mText = text;
    }
}
/* 027BB354 */
void ShaderProgram::setSamplerName(u32 index, const sead::SafeText* source) {
    u32 count = samplerCount;
    u32 destination = gabi::ea(samplers.get());
    be<u8>* name = source->mText;
    if (index < count) destination += index * 16;
    gabi::at<sead::SafeText>(destination)->mText = name;
    const auto* shared = static_cast<const ShaderProgramSharedData*>(binary.get());
    u32 programs = shared->variation.programCount;
    u32 cursor = gabi::ea(shared->variation.programs.get()), end = cursor + programs * 0x84;
    while (cursor != end) {
        const ShaderProgram* program = gabi::at<const ShaderProgram>(cursor);
        u32 records = program->samplerCount;
        u32 target = gabi::ea(program->samplers.get());
        be<u8>* text = source->mText;
        if (index < records) target += index * 16;
        cursor += 0x84;
        gabi::at<sead::SafeText>(target)->mText = text;
    }
}
/* 027BB5BC */
void ShaderProgram::setUniformBlockName(u32 index, const sead::SafeText* source) {
    u32 count = uniformBlockCount;
    u32 destination = gabi::ea(uniformBlocks.get());
    be<u8>* name = source->mText;
    if (index < count) destination += index * 16;
    gabi::at<sead::SafeText>(destination)->mText = name;
    const auto* shared = static_cast<const ShaderProgramSharedData*>(binary.get());
    u32 programs = shared->variation.programCount;
    u32 cursor = gabi::ea(shared->variation.programs.get()), end = cursor + programs * 0x84;
    while (cursor != end) {
        const ShaderProgram* program = gabi::at<const ShaderProgram>(cursor);
        u32 records = program->uniformBlockCount;
        u32 target = gabi::ea(program->uniformBlocks.get());
        be<u8>* text = source->mText;
        if (index < records) target += index * 16;
        cursor += 0x84;
        gabi::at<sead::SafeText>(target)->mText = text;
    }
}
/* WWHD glue */
