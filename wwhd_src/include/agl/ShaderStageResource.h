/* Serialized descriptor views from the assigned WWHD conversion and relocation code. */
#pragma once
#include "wwhd.h"
namespace agl {
struct ShaderStageBlockPrefix_l {
    be<u32> bytes, stageKind;
    u8 reserved[8];
};
struct ShaderStageResource {
    gptr<ShaderStageBlockPrefix_l> block;
    void reverseWords();
    void relocateNames();
};
WWHD_SIZE(ShaderStageBlockPrefix_l, 0x10);
WWHD_OFFSET(ShaderStageBlockPrefix_l, stageKind, 4);
WWHD_SIZE(ShaderStageResource, 4);
WWHD_OFFSET(ShaderStageResource, block, 0);
struct SerializedVertexShader_l {
    be<u32> prefix[0xD0 / 4];
    be<u32> programSize, programOffset, mode, blockCount, blockOffset, uniformCount, uniformOffset, initialCount,
        initialOffset, loopCount, loopOffset, samplerCount, samplerOffset, attributeCount, attributeOffset;
    be<u32> tail[10];
};
WWHD_SIZE(SerializedVertexShader_l, 0x134);
WWHD_OFFSET(SerializedVertexShader_l, programSize, 0xD0);
WWHD_OFFSET(SerializedVertexShader_l, programOffset, 0xD4);
WWHD_OFFSET(SerializedVertexShader_l, mode, 0xD8);
WWHD_OFFSET(SerializedVertexShader_l, blockCount, 0xDC);
WWHD_OFFSET(SerializedVertexShader_l, blockOffset, 0xE0);
WWHD_OFFSET(SerializedVertexShader_l, uniformCount, 0xE4);
WWHD_OFFSET(SerializedVertexShader_l, uniformOffset, 0xE8);
WWHD_OFFSET(SerializedVertexShader_l, initialCount, 0xEC);
WWHD_OFFSET(SerializedVertexShader_l, initialOffset, 0xF0);
WWHD_OFFSET(SerializedVertexShader_l, loopCount, 0xF4);
WWHD_OFFSET(SerializedVertexShader_l, loopOffset, 0xF8);
WWHD_OFFSET(SerializedVertexShader_l, samplerCount, 0xFC);
WWHD_OFFSET(SerializedVertexShader_l, samplerOffset, 0x100);
WWHD_OFFSET(SerializedVertexShader_l, attributeCount, 0x104);
WWHD_OFFSET(SerializedVertexShader_l, attributeOffset, 0x108);
struct SerializedPixelShader_l {
    be<u32> prefix[0xA4 / 4];
    be<u32> programSize, programOffset, mode, blockCount, blockOffset, uniformCount, uniformOffset, initialCount,
        initialOffset, loopCount, loopOffset, samplerCount, samplerOffset;
    be<u32> tail[4];
};
WWHD_SIZE(SerializedPixelShader_l, 0xE8);
WWHD_OFFSET(SerializedPixelShader_l, programSize, 0xA4);
WWHD_OFFSET(SerializedPixelShader_l, programOffset, 0xA8);
WWHD_OFFSET(SerializedPixelShader_l, mode, 0xAC);
WWHD_OFFSET(SerializedPixelShader_l, blockCount, 0xB0);
WWHD_OFFSET(SerializedPixelShader_l, blockOffset, 0xB4);
WWHD_OFFSET(SerializedPixelShader_l, uniformCount, 0xB8);
WWHD_OFFSET(SerializedPixelShader_l, uniformOffset, 0xBC);
WWHD_OFFSET(SerializedPixelShader_l, initialCount, 0xC0);
WWHD_OFFSET(SerializedPixelShader_l, initialOffset, 0xC4);
WWHD_OFFSET(SerializedPixelShader_l, loopCount, 0xC8);
WWHD_OFFSET(SerializedPixelShader_l, loopOffset, 0xCC);
WWHD_OFFSET(SerializedPixelShader_l, samplerCount, 0xD0);
WWHD_OFFSET(SerializedPixelShader_l, samplerOffset, 0xD4);
struct SerializedGeometryShader_l {
    be<u32> prefix[0x4C / 4];
    be<u32> programSize, programOffset, vertexProgramSize, vertexProgramOffset, mode, blockCount, blockOffset,
        uniformCount, uniformOffset, initialCount, initialOffset, loopCount, loopOffset, samplerCount, samplerOffset;
    be<u32> tail[14];
};
WWHD_SIZE(SerializedGeometryShader_l, 0xC0);
WWHD_OFFSET(SerializedGeometryShader_l, programSize, 0x4C);
WWHD_OFFSET(SerializedGeometryShader_l, programOffset, 0x50);
WWHD_OFFSET(SerializedGeometryShader_l, vertexProgramSize, 0x54);
WWHD_OFFSET(SerializedGeometryShader_l, vertexProgramOffset, 0x58);
WWHD_OFFSET(SerializedGeometryShader_l, mode, 0x5C);
WWHD_OFFSET(SerializedGeometryShader_l, blockCount, 0x60);
WWHD_OFFSET(SerializedGeometryShader_l, blockOffset, 0x64);
WWHD_OFFSET(SerializedGeometryShader_l, uniformCount, 0x68);
WWHD_OFFSET(SerializedGeometryShader_l, uniformOffset, 0x6C);
WWHD_OFFSET(SerializedGeometryShader_l, initialCount, 0x70);
WWHD_OFFSET(SerializedGeometryShader_l, initialOffset, 0x74);
WWHD_OFFSET(SerializedGeometryShader_l, loopCount, 0x78);
WWHD_OFFSET(SerializedGeometryShader_l, loopOffset, 0x7C);
WWHD_OFFSET(SerializedGeometryShader_l, samplerCount, 0x80);
WWHD_OFFSET(SerializedGeometryShader_l, samplerOffset, 0x84);
// The native geometry serialization converts0xC0 bytes, including its trailing words.
}
