/* Layout GPU program storage, reconstructed from WWHD instructions. */
#pragma once
#include "nw/lyt/WindowPane.h"
#include "nw/lyt/ResourceProvider.h"
namespace nw { namespace lyt {
struct WindowDrawMethods { u8 prefix[0x9C]; be<u32> prepare; };
struct WindowDrawDispatch : WindowPane { void draw(LayoutDrawContext* context); };
struct ShaderLocation { be<s32> location; static ShaderLocation* create(ShaderLocation*); };
struct ShaderPoint { be<f32> x,y; static ShaderPoint* create(ShaderPoint*); };
struct ShaderAttribute { be<u32> words[8]; };
using ShaderFetch = nw::ut::FetchShader;
struct ShaderProgram {
    gptr<void> vertexShader,pixelShader;
    ShaderFetch fetch;
    be<u32> fetchReserved;
    gptr<ShaderAttribute> attributes;
    gptr<be<s32>> locations;
    gptr<void> displayList;
    be<u32> displayListCapacity,displayListSize;
    be<u8> usesVertexColor; u8 padding[3];
};
struct ShaderProgramConfig {
    be<u32> storageSize,attributeCount,locationCount,displayListCapacity,programCount;
    be<u8> enabled[30]; u8 padding[2];
    u32 calculateStorageSize();
};
struct ShaderProgramCache {
    gptr<void> storage;
    ShaderProgramConfig config;
    gptr<ShaderProgram> programs[30];
    void initialize(ShaderProgramConfig* config,void* storage);
    ShaderProgram* configureProgram(u32 index,void* vertexShader,void* pixelShader);
    void recordProgram(ShaderProgram* program);
    ShaderProgram* program(u32 index);
    void* vertexShader(ShaderProgram* program);
    void* pixelShader(ShaderProgram* program);
    ShaderFetch* fetchShader(ShaderProgram* program);
    ShaderAttribute* attributes(ShaderProgram* program);
    ShaderAttribute* attribute(ShaderProgram* program,u32 index);
    be<s32>* locations(ShaderProgram* program);
    s32 location(ShaderProgram* program,u32 index);
    void bindProgram(ShaderProgram* program);
    u8 vertexColorEnabled(ShaderProgram* program);
};
struct ShaderProgramSlot { gptr<nw::ut::VertexShader> vertexShader; gptr<nw::ut::PixelShader> pixelShader; gptr<void> fetchProgram; static ShaderProgramSlot* create(ShaderProgramSlot*); };
using ShaderSlotConstructor = ShaderProgramSlot*(*)(ShaderProgramSlot*);
struct LayoutRenderStorage;
struct LayoutShaderSet { ShaderProgramSlot slots[30]; static LayoutShaderSet* create(LayoutShaderSet*); void initializePrograms(LayoutRenderStorage* device,const gptr<void>* archives); };
struct LayoutAllocatorMethods { u8 prefix[0x1C]; be<u32> allocate; u8 gap[4]; be<u32> release; };
struct LayoutAllocator { gptr<LayoutAllocatorMethods> methods; };
struct LayoutAllocatorSlot { gptr<LayoutAllocator> value; };
void setLayoutAllocator(LayoutAllocator* allocator);
void* allocateLayoutStorage(u32 size,s32 alignment);
void releaseLayoutStorage(void* storage);
u32 decodeTextureFormat(be<u32>* output,u32 format,u32 alpha);
bool containsLayoutPoint(const TextBounds* bounds,const PaneSize* point);
WWHD_SIZE(ShaderLocation,4); WWHD_SIZE(ShaderPoint,8); WWHD_SIZE(ShaderAttribute,0x20);
WWHD_SIZE(ShaderFetch,0x20); WWHD_SIZE(ShaderProgram,0x44); WWHD_SIZE(ShaderProgramConfig,0x34);
WWHD_SIZE(ShaderProgramCache,0xB0); WWHD_SIZE(ShaderProgramSlot,0xC); WWHD_SIZE(LayoutShaderSet,0x168);
WWHD_OFFSET(ShaderProgram,attributes,0x2C); WWHD_OFFSET(ShaderProgram,locations,0x30);
WWHD_OFFSET(ShaderProgram,displayList,0x34); WWHD_OFFSET(ShaderProgram,usesVertexColor,0x40);
WWHD_OFFSET(ShaderProgramCache,programs,0x38); WWHD_OFFSET(ShaderProgramConfig,enabled,0x14);
WWHD_OFFSET(LayoutAllocatorMethods,allocate,0x1C); WWHD_OFFSET(LayoutAllocatorMethods,release,0x24);
} }
void* gpuResourceAllocate(u32 size);
void gpuResourceConstructArray(void*,u32 count,u32 stride,nw::lyt::ShaderSlotConstructor constructor);
void gpuBeginDisplayList(void*,u32,u32);
u32 gpuEndDisplayList(void*);
void gpuCallDisplayList(void*,u32);
void gpuSetFetchShader(nw::lyt::ShaderFetch*);
void gpuSetVertexShader(void*);
void gpuSetPixelShader(void*);

void* gpuCreateFrameHeap(void* storage,u32 size,u32 options);
void* gpuAllocateFrameHeap(void* heap,u32 size,s32 alignment);
void gpuDestroyFrameHeap(void* heap);
void gpuZeroMemory(void* storage,u32 size);
void gpuInvalidate(u32 mode,void* storage,u32 size);
