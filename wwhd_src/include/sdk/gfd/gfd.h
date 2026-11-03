// GX2 file-format layouts; field offsets checked against the WWHD binary.
#pragma once
#include "wwhd.h"
struct GFDHeader {
    be<u32> magic, headerSize, majorVersion, minorVersion, gpuVersion, align, reserved[2];
};
struct GFDBlockHeader {
    be<u32> magic, headerSize, majorVersion, minorVersion, type, dataSize, id, index;
};
struct GFDRelocationHeader {
    be<u32> magic, headerSize, reserved, dataSize, dataOffset, textSize, textOffset;
    be<u32> patchBase, patchCount, patchOffset;
};
struct GFDVertexShader {
    u8 header[0xD4];
    gptr<void> program;
};
struct GFDPixelShader {
    u8 header[0xA8];
    gptr<void> program;
};
WWHD_SIZE(GFDHeader, 0x20);
WWHD_OFFSET(GFDHeader, gpuVersion, 0x10);
WWHD_SIZE(GFDBlockHeader, 0x20);
WWHD_OFFSET(GFDBlockHeader, type, 0x10);
WWHD_OFFSET(GFDBlockHeader, dataSize, 0x14);
WWHD_SIZE(GFDRelocationHeader, 0x28);
WWHD_OFFSET(GFDRelocationHeader, patchBase, 0x1C);
WWHD_OFFSET(GFDRelocationHeader, patchCount, 0x20);
WWHD_OFFSET(GFDRelocationHeader, patchOffset, 0x24);
WWHD_OFFSET(GFDVertexShader, program, 0xD4);
WWHD_SIZE(GFDVertexShader, 0xD8);
WWHD_OFFSET(GFDPixelShader, program, 0xA8);
WWHD_SIZE(GFDPixelShader, 0xAC);
enum GFDBlockType {
    GFD_END = 1,
    GFD_VERTEX_HEADER = 3,
    GFD_VERTEX_PROGRAM = 5,
    GFD_PIXEL_HEADER = 6,
    GFD_PIXEL_PROGRAM = 7
};

enum GFDFormat {
    GFD_FILE_MAGIC = 0x47667832,
    GFD_BLOCK_MAGIC = 0x424C4B7B,
    GFD_FILE_MAJOR = 7,
    GFD_FILE_MINOR = 1,
    GFD_BLOCK_MAJOR = 1,
    GFD_PATCH_OFFSET_MASK = 0xFFFFF,
    GFD_SHADER_ALIGNMENT = 256
};
