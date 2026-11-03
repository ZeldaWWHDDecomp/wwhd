/* Uniform layout masks, storage views and byte-reversed word transfer from WWHD. */
#include "agl/UniformBuffer.h"
using namespace agl;
struct UniformPropertyTable_l {
    UniformTypeProperties_l entries[15];
};
struct UniformMethodTag_l {
    be<u32> first;
};
struct UniformCacheLine_l {
    be<u32> words[8];
};
static UniformPropertyTable_l sUniformProperties;
static UniformMethodTag_l sUniformMethods;
static void* allocateUniformObject(u32);
static void releaseUniformObject(void*);
static void releaseUniformArray(void*);
static void* allocateUniformFormat(u32, void*, s32);
static void* allocateUniformArray(u32, void*, s32);
static void flushUniformRange(void*, u32);
static void flushUniformRangeNoSync(void*, u32);
static void invalidateUniformRange(u32, void*, u32);

/* 027BEB5C */
UniformBuffer* UniformBuffer::create(UniformBuffer* buffer) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!buffer) buffer = static_cast<UniformBuffer*>(allocateUniformObject(sizeof(UniformBuffer)));
    if (buffer) {
        buffer->stride = 0;
        buffer->methods = &sUniformMethods;
        buffer->format = nullptr;
        buffer->size = 0;
        buffer->cpuData = nullptr;
        buffer->gpuData = nullptr;
        buffer->flags = 0;
        buffer->bufferCount = 1;
    }
    return buffer;
}
/* 027BED84 */
void UniformBuffer::appendField(u32 type, u32 count) {
    UniformFormat_l* descriptor = format;
    u32 index = descriptor->count;
    UniformField_l* fields = descriptor->fields;
    UniformField_l* field = &fields[index];
    field->type = type;
    u32 kind = field->type;
    u32 fieldCount = static_cast<u16>(count);
    field->count = fieldCount;
    const UniformTypeProperties_l* properties = &sUniformProperties.entries[kind];
    u32 alignmentWords =
        fieldCount == 1 ? static_cast<u32>(properties->singleAlignment) : static_cast<u32>(properties->arrayStride);
    u32 componentWords =
        fieldCount == 1 ? static_cast<u32>(properties->words) : static_cast<u32>(properties->arrayStride);
    u32 alignmentBytes = alignmentWords * 4;
    u32 aligned = (static_cast<u32>(size) + alignmentBytes - 1) & ~(alignmentBytes - 1);
    size = aligned;
    u32 copiedCount = field->count;
    field->offset = aligned;
    u32 bytes = copiedCount * componentWords * 4;
    u32 currentSize = size;
    descriptor = format;
    size = currentSize + bytes;
    descriptor->count = static_cast<u32>(descriptor->count) + 1;
    descriptor = format;
    if (static_cast<u32>(descriptor->count) == static_cast<u32>(descriptor->capacity))
        size = (static_cast<u32>(size) + 31) & ~31u;
}
/* 027BEE9C */
void UniformBuffer::adoptFormat(const UniformBuffer* source) {
    UniformFormat_l* descriptor = source->format;
    u32 oldFlags = flags;
    format = descriptor;
    u32 bytes = source->size;
    flags = oldFlags & ~1u;
    size = bytes;
}
/* 027BEEBC */
u32 UniformBuffer::allocationStride(s32 buffers) const {
    u32 bytes = size;
    if (buffers > 1) bytes = (bytes + 255) & ~255u;
    return bytes;
}
/* 027BEED4 */
void UniformBuffer::bindStorage(void* data, u32 buffers) {
    gpuData = data;
    cpuData = data;
    bufferCount = buffers;
}
static void clearUniformCacheLine(u32 address) {
    UniformCacheLine_l* line = gabi::at<UniformCacheLine_l>(address & ~31u);
    for (u32 i = 0; i < 8; ++i)
        line->words[i] = 0;
}
/* 027BEF54 */
void UniformBuffer::clear() {
    u32 address = gabi::ea(cpuData.get());
    u32 end = address + (static_cast<u32>(size) & ~31u);
    while (address < end) {
        clearUniformCacheLine(address);
        address += 32;
    }
}
static void writeReversedUniformWord(u32 address, u32 value) {
    u32 reversed = (value << 24) | ((value & 0xFF00) << 8) | ((value >> 8) & 0xFF00) | (value >> 24);
    *gabi::at<be<u32>>(address) = reversed;
}
/* 027BF048 */
void UniformBuffer::writeWords(void* destination, u32 index, const be<u32>* source, u32 first, s32 count) const {
    const UniformFormat_l* descriptor = format;
    const UniformField_l* fields = descriptor->fields;
    const UniformField_l* field = &fields[index];
    const UniformTypeProperties_l* properties = &sUniformProperties.entries[static_cast<u32>(field->type)];
    u32 strideWords = properties->arrayStride;
    u32 offset = field->offset;
    u32 base = gabi::ea(destination) + offset + strideWords * first * 4;
    u32 copiedWords = properties->words;
    for (s32 element = 0; element < count; ++element) {
        for (u32 word = 0; word < copiedWords; ++word) {
            writeReversedUniformWord(base + word * 4, *source);
            ++source;
        }
        base += strideWords * 4;
    }
}
/* 027BEBEC */
void UniformBuffer::finalize() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 currentFlags = flags;
    if (currentFlags & 2) {
        if (gpuData) {
            releaseUniformArray(gpuData);
            currentFlags = flags;
            gpuData = nullptr;
        }
        flags = currentFlags & ~2u;
    }
    u32 formatFlags = flags;
    stride = 0;
    size = 0;
    gpuData = nullptr;
    cpuData = nullptr;
    if (formatFlags & 1) {
        UniformFormat_l* descriptor = format;
        releaseUniformObject(descriptor->fields);
        releaseUniformObject(format);
        u32 remainingFlags = flags;
        format = nullptr;
        flags = remainingFlags & ~1u;
    }
}
/* 027BEC90 */
void UniformBuffer::destroy(UniformBuffer* buffer, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (buffer) {
        buffer->methods = &sUniformMethods;
        buffer->finalize();
        if (flags & 1) releaseUniformObject(buffer);
    }
}
/* 027BECEC */
void UniformBuffer::allocateFormat(u32 capacity, void* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    UniformFormat_l* descriptor = static_cast<UniformFormat_l*>(allocateUniformFormat(8, heap, 4));
    format = descriptor;
    void* fields = allocateUniformArray(capacity * 6, heap, 4);
    descriptor->fields = static_cast<UniformField_l*>(fields);
    descriptor = format;
    descriptor->capacity = capacity;
    descriptor = format;
    descriptor->count = 0;
    u32 oldFlags = flags;
    size = 0;
    flags = oldFlags | 1;
}
/* 027BEEE4 */
void UniformBuffer::allocateStorage(void* heap, u32 buffers) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 bytes = allocationStride(static_cast<s32>(buffers));
    stride = bytes;
    void* memory = allocateUniformArray(bytes * buffers, heap, 0x100);
    bindStorage(memory, buffers);
    flags = static_cast<u32>(flags) | 2;
}
/* 027BEF80 */
void UniformBuffer::flush(void* data, u32 invalidateGpu) {
    gabi::AutoFrame frame{gabi::current_function()};
    flushUniformRange(data, size);
    if (invalidateGpu) invalidateUniformRange(4, data, size);
}
/* 027BEFE4 */
void UniformBuffer::flushNoSync(void* data, u32 invalidateGpu) {
    gabi::AutoFrame frame{gabi::current_function()};
    flushUniformRangeNoSync(data, size);
    if (invalidateGpu) invalidateUniformRange(4, data, size);
}
/* WWHD glue
data sUniformProperties 10169D50
data sUniformMethods 10169D80
ext 0273AD10 void* allocateUniformObject(u32)
ext 0273AF40 void releaseUniformObject(void*)
ext 0273AFC8 void releaseUniformArray(void*)
ext 0273B050 void* allocateUniformFormat(u32,void*,s32)
ext 0273B0D4 void* allocateUniformArray(u32,void*,s32)
ext C0008898 void flushUniformRange(void*,u32)
ext C00088A0 void flushUniformRangeNoSync(void*,u32)
ext C00064A8 void invalidateUniformRange(u32,void*,u32)
*/
