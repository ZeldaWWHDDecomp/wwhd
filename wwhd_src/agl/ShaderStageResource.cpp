/* Stage descriptor word conversion and name relocation from assigned WWHD instructions. */
#include "agl/ShaderStageResource.h"
#include "agl/ShaderArchiveRelocations.h"
#include "agl/ShaderResourceViews.h"
using namespace agl;
struct StageWordConversion_l {
    gfn operation;
};
static StageWordConversion_l sStageWordConversion;
/* 027BC138 */
void ShaderStageResource::reverseWords() {
    gabi::AutoFrame frame{gabi::current_function()};
    const ShaderStageBlockPrefix_l* header = block;
    u32 kind = header->stageKind;
    if (kind > 2) return;
    u32 descriptor = gabi::ea(header) + 0x10;
    u32 count = kind == 0 ? 77 : (kind == 1 ? 58 : 48);
    u32 operation = sStageWordConversion.operation.get();
    for (u32 i = 0; i < count; ++i) {
        be<u32>* word = gabi::at<be<u32>>(descriptor + i * 4);
        u32 result = gabi::call_ptr<u32>(operation, static_cast<u32>(*word));
        *word = result;
    }
    u32 bytes, payload;
    if (kind == 0) {
        const SerializedVertexShader_l* shader = gabi::at<const SerializedVertexShader_l>(descriptor);
        u32 blocks = shader->blockCount, samplers = shader->samplerCount, uniforms = shader->uniformCount;
        u32 attributes = shader->attributeCount, initials = shader->initialCount;
        bytes = (blocks + samplers) * 12 + uniforms * 20 + attributes * 16 + initials * 20;
        payload = descriptor + 0x134;
    } else if (kind == 1) {
        const SerializedPixelShader_l* shader = gabi::at<const SerializedPixelShader_l>(descriptor);
        u32 uniforms = shader->uniformCount, initials = shader->initialCount, blocks = shader->blockCount,
            samplers = shader->samplerCount;
        bytes = (uniforms + initials) * 20 + (blocks + samplers) * 12;
        payload = descriptor + 0xE8;
    } else {
        const SerializedGeometryShader_l* shader = gabi::at<const SerializedGeometryShader_l>(descriptor);
        u32 uniforms = shader->uniformCount, initials = shader->initialCount, samplers = shader->samplerCount,
            blocks = shader->blockCount;
        bytes = (uniforms + initials) * 20 + (blocks + samplers) * 12;
        payload = descriptor + 0xC0;
    }
    u32 words = bytes >> 2;
    if (!words) return;
    operation = sStageWordConversion.operation.get();
    for (u32 remaining = words; remaining; --remaining, payload += 4) {
        be<u32>* word = gabi::at<be<u32>>(payload);
        u32 result = gabi::call_ptr<u32>(operation, static_cast<u32>(*word));
        *word = result;
    }
}
/* 027BC2D8 */
void ShaderStageResource::relocateNames() {
    gabi::AutoFrame frame{gabi::current_function()};
    const ShaderStageBlockPrefix_l* header = block;
    u32 kind = header->stageKind;
    u32 descriptor = gabi::ea(header) + 0x10;
    void* base = gabi::at<void>(descriptor);
    if (kind == 0) {
        SerializedVertexShader_l* shader = gabi::at<SerializedVertexShader_l>(descriptor);
        BlockNameRecord_l* blocks = ShaderArchiveRelocations::relocateBlockNames(
            base, shader->blockOffset, static_cast<s32>(shader->blockCount));
        u32 offset = shader->uniformOffset;
        s32 count = static_cast<s32>(shader->uniformCount);
        shader->blockOffset = gabi::ea(blocks);
        ValueNameRecord_l* uniforms = ShaderArchiveRelocations::relocateValueNames(base, offset, count);
        offset = shader->samplerOffset;
        count = static_cast<s32>(shader->samplerCount);
        shader->uniformOffset = gabi::ea(uniforms);
        SamplerNameRecord_l* samplers = ShaderArchiveRelocations::relocateSamplerNames(base, offset, count);
        count = static_cast<s32>(shader->attributeCount);
        offset = shader->attributeOffset;
        shader->samplerOffset = gabi::ea(samplers);
        AttributeNameRecord_l* attributes = ShaderArchiveRelocations::relocateAttributeNames(base, offset, count);
        offset = shader->loopOffset;
        shader->attributeOffset = gabi::ea(attributes);
        void* loops = ShaderRelativeBase::add(base, offset);
        offset = shader->programOffset;
        shader->loopOffset = gabi::ea(loops);
        shader->programOffset = gabi::ea(ShaderRelativeBase::add(base, offset));
    } else if (kind == 1) {
        SerializedPixelShader_l* shader = gabi::at<SerializedPixelShader_l>(descriptor);
        BlockNameRecord_l* blocks = ShaderArchiveRelocations::relocateBlockNames(
            base, shader->blockOffset, static_cast<s32>(shader->blockCount));
        u32 offset = shader->uniformOffset;
        s32 count = static_cast<s32>(shader->uniformCount);
        shader->blockOffset = gabi::ea(blocks);
        ValueNameRecord_l* uniforms = ShaderArchiveRelocations::relocateValueNames(base, offset, count);
        offset = shader->samplerOffset;
        count = static_cast<s32>(shader->samplerCount);
        shader->uniformOffset = gabi::ea(uniforms);
        SamplerNameRecord_l* samplers = ShaderArchiveRelocations::relocateSamplerNames(base, offset, count);
        shader->samplerOffset = gabi::ea(samplers);
        offset = shader->loopOffset;
        void* loops = ShaderRelativeBase::add(base, offset);
        offset = shader->programOffset;
        shader->loopOffset = gabi::ea(loops);
        shader->programOffset = gabi::ea(ShaderRelativeBase::add(base, offset));
    } else if (kind == 2) {
        SerializedGeometryShader_l* shader = gabi::at<SerializedGeometryShader_l>(descriptor);
        BlockNameRecord_l* blocks = ShaderArchiveRelocations::relocateBlockNames(
            base, shader->blockOffset, static_cast<s32>(shader->blockCount));
        u32 offset = shader->uniformOffset;
        s32 count = static_cast<s32>(shader->uniformCount);
        shader->blockOffset = gabi::ea(blocks);
        ValueNameRecord_l* uniforms = ShaderArchiveRelocations::relocateValueNames(base, offset, count);
        offset = shader->samplerOffset;
        count = static_cast<s32>(shader->samplerCount);
        shader->uniformOffset = gabi::ea(uniforms);
        SamplerNameRecord_l* samplers = ShaderArchiveRelocations::relocateSamplerNames(base, offset, count);
        offset = shader->loopOffset;
        shader->samplerOffset = gabi::ea(samplers);
        void* loops = ShaderRelativeBase::add(base, offset);
        offset = shader->programOffset;
        shader->loopOffset = gabi::ea(loops);
        void* program = ShaderRelativeBase::add(base, offset);
        offset = shader->vertexProgramOffset;
        shader->programOffset = gabi::ea(program);
        shader->vertexProgramOffset = gabi::ea(ShaderRelativeBase::add(base, offset));
    }
}
/* WWHD glue
data sStageWordConversion 10147624
*/
