// GX2 shader-file loader, reconstructed from the WWHD binary.
#include "sdk/gfd/gfd.h"
static void GFDReport(const char* text);
static void GFDReport(const char* text, u32 value);
static u32 GX2TempGetGPUVersion();
static void* GFDCopy(void* dest, const void* source, u32 size);

/* 028E81E4 */
u32 GFDReadHeader(be<u32>* major, be<u32>* minor, be<u32>* gpu, const GFDHeader* file) {
    *major = 0;
    *minor = 0;
    *gpu = 0;
    if (file->magic != GFD_FILE_MAGIC || file->headerSize != sizeof(GFDHeader))
        return 0;
    *major = file->majorVersion;
    u32 minorVersion = file->minorVersion;
    u32 gpuVersion = file->gpuVersion;
    *minor = minorVersion;
    *gpu = gpuVersion;
    return 1;
}
/* 028E823C */
u32 GFDCheckHeader(const GFDHeader* file) {
    if (!file)
        return 0;
    gabi::Local<be<u32>> major, minor, gpu;
    if (!GFDReadHeader(major.get(), minor.get(), gpu.get(), file) || *major != GFD_FILE_MAJOR)
        return 0;
    if (*minor > GFD_FILE_MINOR) {
        GFDReport("Warning: File minor version is greater than that of code.");
        return 0;
    }
    u32 gpuVersion = GX2TempGetGPUVersion();
    return gpuVersion == *gpu;
}
/* 028E82E0 */
u32 GFDCheckBlockHeader(const GFDBlockHeader* block) {
    if (block->magic != GFD_BLOCK_MAGIC) {
        GFDReport("Warning: Unknown block format. \n");
        return 0;
    }
    u32 version = block->majorVersion;
    if (version != GFD_BLOCK_MAJOR) {
        GFDReport("Warning: Unsupported block version %d. \n", version);
        return 0;
    }
    return 1;
}
/* 028E8368 */
u32 GFDGetBlockCount(u32 type, const GFDHeader* file) {
    if (!file || !GFDCheckHeader(file))
        return 0;
    const GFDBlockHeader* block = reinterpret_cast<const GFDBlockHeader*>(file + 1);
    u32 count = 0;
    if (!GFDCheckBlockHeader(block))
        return count;
    do {
        const GFDBlockHeader* previous = block;
        u32 blockType = previous->type;
        if (type == blockType)
            ++count;
        block = reinterpret_cast<const GFDBlockHeader*>(reinterpret_cast<const u8*>(block) +
                                                        u32(previous->dataSize) + sizeof(GFDBlockHeader));
        if (blockType == GFD_END)
            break;
        // HD: the binary validates the previous header before advancing the walk.
        if (!GFDCheckBlockHeader(previous))
            break;
    } while (true);
    return count;
}
/* 028E8428 */
u32 GFDGetVertexShaderCount(const GFDHeader* file) { return GFDGetBlockCount(GFD_VERTEX_HEADER, file); }
/* 028E8434 */
u32 GFDGetPixelShaderCount(const GFDHeader* file) { return GFDGetBlockCount(GFD_PIXEL_HEADER, file); }
/* 028E8440 */
u32 GFDGetBlockSize(u32 type, u32 index, const GFDHeader* file) {
    if (!file || !GFDCheckHeader(file))
        return 0;
    const GFDBlockHeader* block = reinterpret_cast<const GFDBlockHeader*>(file + 1);
    u32 count = 0;
    if (!GFDCheckBlockHeader(block))
        return 0;
    do {
        const GFDBlockHeader* previous = block;
        u32 blockType = previous->type;
        if (type == blockType) {
            if (index == count)
                return previous->dataSize;
            ++count;
        }
        block = reinterpret_cast<const GFDBlockHeader*>(reinterpret_cast<const u8*>(block) +
                                                        u32(previous->dataSize) + sizeof(GFDBlockHeader));
        if (blockType == GFD_END)
            break;
        if (!GFDCheckBlockHeader(previous))
            break;
    } while (true);
    return 0;
}
/* 028E8518 */
u32 GFDGetVertexShaderHeaderSize(u32 index, const GFDHeader* file) {
    return GFDGetBlockSize(GFD_VERTEX_HEADER, index, file);
}
/* 028E8528 */
u32 GFDGetPixelShaderHeaderSize(u32 index, const GFDHeader* file) {
    return GFDGetBlockSize(GFD_PIXEL_HEADER, index, file);
}
/* 028E8538 */
u32 GFDGetVertexShaderProgramSize(u32 index, const GFDHeader* file) {
    return GFDGetBlockSize(GFD_VERTEX_PROGRAM, index, file);
}
/* 028E8548 */
u32 GFDGetPixelShaderProgramSize(u32 index, const GFDHeader* file) {
    return GFDGetBlockSize(GFD_PIXEL_PROGRAM, index, file);
}
/* 028E8558 */
u32 GFDGetPatchOffset(u32 value) { return value & GFD_PATCH_OFFSET_MASK; }
/* 028E8560 */
u32 GFDRelocate(GFDRelocationHeader* relocation, u32 oldBase, u32 newBase, u8* data) {
    u32 tableOffset = GFDGetPatchOffset(relocation->patchOffset);
    u32 count = relocation->patchCount;
    if (!data)
        return 0;
    for (u32 i = 0; i < count; ++i) {
        be<u32>* patches = reinterpret_cast<be<u32>*>(data + tableOffset);
        u32 patch = patches[i];
        if (patch) {
            u32 offset = GFDGetPatchOffset(patch);
            be<u32>* target = reinterpret_cast<be<u32>*>(data + offset);
            u32 value = GFDGetPatchOffset(*target);
            *target = value - oldBase + newBase;
        }
    }
    relocation->patchBase = newBase;
    return 1;
}
/* 028E85EC */
u32 GFDRelocateBlock(u32 size, u8* data) {
    if (!data)
        return 0;
    GFDRelocationHeader* relocation =
        reinterpret_cast<GFDRelocationHeader*>(data + size - sizeof(GFDRelocationHeader));
    if (relocation->patchBase)
        return 1;
    return GFDRelocate(relocation, 0, gabi::ea(data), data);
}
/* 028E8624 */
u32 GFDGetVertexShader(GFDVertexShader* shader, void* program, u32 index, const GFDHeader* file) {
    if (!shader || !program || !file || !GFDCheckHeader(file))
        return 0;
    if (gabi::ea(program) & (GFD_SHADER_ALIGNMENT - 1)) {
        GFDReport("Warning: Shader program buffer not aligned correctly. It needs %d byte align buffer.\n",
                  256);
        return 0;
    }
    const GFDBlockHeader* block = reinterpret_cast<const GFDBlockHeader*>(file + 1);
    u32 headerIndex = 0, programIndex = 0;
    if (!GFDCheckBlockHeader(block))
        return 1;
    do {
        const GFDBlockHeader* previous = block;
        const u8* payload = reinterpret_cast<const u8*>(block + 1);
        u32 type = previous->type;
        if (type == GFD_VERTEX_HEADER) {
            if (index == headerIndex) {
                GFDCopy(shader, payload, previous->dataSize);
                u32 size = previous->dataSize;
                if (!GFDRelocateBlock(size, reinterpret_cast<u8*>(shader)))
                    return 0;
            }
            ++headerIndex;
        } else if (type == GFD_VERTEX_PROGRAM) {
            if (index == programIndex) {
                u32 size = previous->dataSize;
                shader->program = program;
                GFDCopy(program, payload, size);
            }
            ++programIndex;
        }
        type = previous->type;
        block = reinterpret_cast<const GFDBlockHeader*>(payload + u32(previous->dataSize));
        if (type == GFD_END)
            break;
        if (!GFDCheckBlockHeader(previous))
            break;
    } while (true);
    return 1;
}
/* 028E87A8 */
u32 GFDGetPixelShader(GFDPixelShader* shader, void* program, u32 index, const GFDHeader* file) {
    if (!shader || !program || !file || !GFDCheckHeader(file))
        return 0;
    if (gabi::ea(program) & (GFD_SHADER_ALIGNMENT - 1)) {
        GFDReport("Warning: Shader program buffer not aligned correctly. It needs %d byte align buffer.\n",
                  256);
        return 0;
    }
    const GFDBlockHeader* block = reinterpret_cast<const GFDBlockHeader*>(file + 1);
    u32 headerIndex = 0, programIndex = 0;
    if (!GFDCheckBlockHeader(block))
        return 1;
    do {
        const GFDBlockHeader* previous = block;
        const u8* payload = reinterpret_cast<const u8*>(block + 1);
        u32 type = previous->type;
        if (type == GFD_PIXEL_HEADER) {
            if (index == headerIndex) {
                GFDCopy(shader, payload, previous->dataSize);
                u32 size = previous->dataSize;
                if (!GFDRelocateBlock(size, reinterpret_cast<u8*>(shader)))
                    return 0;
            }
            ++headerIndex;
        } else if (type == GFD_PIXEL_PROGRAM) {
            if (index == programIndex) {
                u32 size = previous->dataSize;
                shader->program = program;
                GFDCopy(program, payload, size);
            }
            ++programIndex;
        }
        type = previous->type;
        block = reinterpret_cast<const GFDBlockHeader*>(payload + u32(previous->dataSize));
        if (type == GFD_END)
            break;
        if (!GFDCheckBlockHeader(previous))
            break;
    } while (true);
    return 1;
}
/* WWHD glue
ext C0009EE8 void GFDReport(const char* text)
ext C0009EE8 void GFDReport(const char* text, u32 value)
ext C0006AA8 u32 GX2TempGetGPUVersion()
ext C000A848 void* GFDCopy(void* dest, const void* source, u32 size)
str 10188F5C
str 10188F98
str 10188FBC
str 10188FE8 in 028E8624
str 10189040 in 028E87A8
*/
