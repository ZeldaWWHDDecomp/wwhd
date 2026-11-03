/* Local uniform buffer and format prefixes established by assigned WWHD accesses. */
#pragma once
#include "wwhd.h"
namespace agl {
struct UniformField_l {
    be<u8> type;
    u8 reserved;
    be<u16> count, offset;
};
struct UniformFormat_l {
    gptr<UniformField_l> fields;
    be<u16> capacity, count;
};
struct UniformTypeProperties_l {
    be<u8> words, singleAlignment, arrayStride;
};
struct UniformBuffer {
    gptr<UniformFormat_l> format;
    gptr<void> cpuData, gpuData;
    be<u32> stride, size;
    be<u8> flags, bufferCount;
    u8 reserved[2];
    gptr<const void> methods;
    static UniformBuffer* create(UniformBuffer*);
    void appendField(u32 type, u32 count);
    void adoptFormat(const UniformBuffer* source);
    u32 allocationStride(s32 buffers) const;
    void bindStorage(void* data, u32 buffers);
    void clear();
    void finalize();
    static void destroy(UniformBuffer*, u32 flags);
    void allocateFormat(u32 capacity, void* heap);
    void allocateStorage(void* heap, u32 buffers);
    void flush(void* data, u32 invalidateGpu);
    void flushNoSync(void* data, u32 invalidateGpu);
    void writeWords(void* destination, u32 field, const be<u32>* source, u32 first, s32 count) const;
};
WWHD_SIZE(UniformField_l, 6);
WWHD_SIZE(UniformFormat_l, 8);
WWHD_OFFSET(UniformFormat_l, fields, 0);
WWHD_OFFSET(UniformFormat_l, capacity, 4);
WWHD_OFFSET(UniformFormat_l, count, 6);
WWHD_OFFSET(UniformField_l, type, 0);
WWHD_OFFSET(UniformField_l, count, 2);
WWHD_OFFSET(UniformField_l, offset, 4);
WWHD_SIZE(UniformTypeProperties_l, 3);
WWHD_SIZE(UniformBuffer, 0x1C);
WWHD_OFFSET(UniformBuffer, format, 0);
WWHD_OFFSET(UniformBuffer, cpuData, 4);
WWHD_OFFSET(UniformBuffer, gpuData, 8);
WWHD_OFFSET(UniformBuffer, stride, 0xC);
WWHD_OFFSET(UniformBuffer, size, 0x10);
WWHD_OFFSET(UniformBuffer, flags, 0x14);
WWHD_OFFSET(UniformBuffer, bufferCount, 0x15);
WWHD_OFFSET(UniformBuffer, methods, 0x18);
}
