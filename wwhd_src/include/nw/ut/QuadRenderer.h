#pragma once
#include "wwhd.h"
namespace nw { namespace ut {
struct ShaderAttribute { gptr<const be<u8>> name; be<u32> type; be<u32> count; be<u32> location; };
struct ShaderUniform { gptr<const be<u8>> name; be<u32> type; be<u32> count; be<u32> location; be<u32> block; };
struct ShaderSampler { gptr<const be<u8>> name; be<u32> type; be<u32> location; };
struct VertexShader {
    u8 registers[0xD0]; be<u32> programSize; be<u32> program;
    u8 reserved[0xC]; be<u32> blockCount; gptr<ShaderUniform> blocks;
    u8 uniformInfo[0x18]; be<u32> attributeCount; gptr<ShaderAttribute> attributes;
};
struct PixelShader {
    u8 registers[0xA4]; be<u32> programSize; be<u32> program;
    u8 reserved[0xC]; be<u32> blockCount; gptr<ShaderUniform> blocks;
    u8 uniformInfo[0x10]; be<u32> samplerCount; gptr<ShaderSampler> samplers;
};
struct FetchShader { be<u32> words[8]; };
struct AttributeStream {
    be<u32> location; be<u32> buffer; be<u32> offset; be<u32> format;
    be<u32> indexType; be<u32> divisor; be<u32> mask; be<u32> endian;
};
struct ShaderState {
    gptr<VertexShader> vertex; gptr<PixelShader> pixel; FetchShader fetch;
    be<u32> fetchProgram; be<u32> textureUnit; be<u32> positionLocation;
    be<u32> colorLocation; be<u32> uvLocation; AttributeStream streams[3];
    be<u32> displayList; be<u32> displayCapacity; be<u32> displaySize;
    be<u32> transformUniform; be<u32> interpolateOffset; be<u32> interpolateWidth;
    be<u32> shadowOffset; be<u32> shadowWidth;
    static ShaderState* create(ShaderState*);
    u32 loadVertex(gptr<VertexShader>*,u32,const void*,u32);
    u32 loadPixel(gptr<PixelShader>*,u32,const void*,u32);
    u32 initialize(u32,u32,const gptr<const void>*);
    void apply();
};
struct Texture { u8 surface[0xE]; be<u8> transformed; u8 padding; u8 descriptor[0x90]; };
struct Quad {
    be<f32> width;be<f32> height;be<f32> x;be<f32> y;
    be<u32> topColor;be<u32> bottomColor;
    be<f32> left;be<f32> top;be<f32> right;be<f32> bottom;
    gptr<Texture> texture;be<s16> bearing;be<u8> layer;be<u8> flags;
};
struct DrawGroup { gptr<Texture> texture;be<u32> firstVertex;be<u32> quadCount;be<u32> flags; };
struct Vertex { be<f32> x;be<f32> y;be<f32> z;be<u32> color;be<f32> u;be<f32> v;be<f32> layer; };
struct QuadBatch {
    be<u32> capacity;be<u16> quadCount;be<u8> dirty;u8 reserved;
    be<u32> unused;be<u32> vertexCount;be<u32> reservedWord;
    be<u32> vertexBuffers[2];be<u32> bufferIndex;
    DrawGroup groups[8];be<u32> groupCount;Quad quads[1];
};
struct Matrix4 { be<f32> values[4][4]; };
struct MatrixRows3 { be<u32> values[3][4]; };
struct DrawParameters {
    gptr<QuadBatch> batch;gptr<Matrix4> projection;gptr<MatrixRows3> view;gptr<MatrixRows3> model;
    be<f32> interpolateFrom[4];be<f32> interpolateTo[4];
    be<f32> shadowFrom[4];be<f32> shadowTo[4];be<u32> flags;
};
struct ShadowParameters { be<u32> topColor;be<u32> bottomColor;be<f32> x;be<f32> y;be<f32> bearingScale;be<f32> heightScale;be<f32> advance; };
struct QuadRenderer {
    be<u32> capacity; ShaderState shaders[3];be<u32> sampler[3];be<u32> indices;be<u32> vtable;
    QuadRenderer();
    static u32 requiredSize(u32,const gptr<const void>*);
    static void initializeIndices(be<u16>*,u32);
    void initialize(u32,u32,const gptr<const void>*);
    void group(QuadBatch*,u32);
    void build(QuadBatch*,const ShadowParameters*);
    void draw(const DrawParameters*);
};
WWHD_SIZE(AttributeStream,0x20);WWHD_SIZE(FetchShader,0x20);WWHD_SIZE(ShaderState,0xBC);WWHD_OFFSET(ShaderState,streams,0x3C);WWHD_OFFSET(ShaderState,transformUniform,0xA8);
WWHD_OFFSET(VertexShader,programSize,0xD0);WWHD_OFFSET(VertexShader,attributeCount,0x104);WWHD_OFFSET(PixelShader,blockCount,0xB8);WWHD_OFFSET(PixelShader,samplerCount,0xD0);
WWHD_SIZE(Quad,0x30);WWHD_SIZE(Vertex,0x1C);WWHD_SIZE(DrawGroup,0x10);WWHD_OFFSET(QuadBatch,groups,0x20);WWHD_OFFSET(QuadBatch,quads,0xA4);WWHD_OFFSET(QuadRenderer,sampler,0x238);WWHD_OFFSET(QuadRenderer,indices,0x244);WWHD_SIZE(QuadRenderer,0x24C);
} }
u32 quadFetchSize(u32,u32,u32);
u32 quadVertexCount(const void*);
u32 quadPixelCount(const void*);
u32 quadVertexHeaderSize(u32,const void*);
u32 quadVertexProgramSize(u32,const void*);
u32 quadPixelHeaderSize(u32,const void*);
u32 quadPixelProgramSize(u32,const void*);
u32 quadLoadVertex(nw::ut::VertexShader*,u32,u32,const void*);
u32 quadLoadPixel(nw::ut::PixelShader*,u32,u32,const void*);
void quadInvalidate(u32,u32,u32);
void quadInitializeFetch(nw::ut::FetchShader*,u32,u32,const nw::ut::AttributeStream*,u32,u32);
void quadConstructArray(void*,u32,u32,u32);
void quadInitializeSampler(void*,u32,u32);
void quadSamplerClamping(void*,u32,u32);
void quadBeginList(u32,u32,u32);
u32 quadEndList(u32);
void quadApplyFetch(nw::ut::FetchShader*);
void quadApplyVertex(nw::ut::VertexShader*);
void quadApplyPixel(nw::ut::PixelShader*);
void quadApplySampler(void*,u32);
void quadApplyTexture(void*,u32);
void quadVertexUniform(u32,u32,const void*);
void quadPixelUniform(u32,u32,const void*);
void quadAttributeBuffer(u32,u32,u32,u32);
void quadDrawIndexed(u32,u32,u32,u32,u32,u32);
void quadZeroRange(u32,u32);
void quadCallList(u32,u32);

nw::ut::ShaderState* quadConstructShader(nw::ut::ShaderState*);

void* quadAllocate(u32);
WWHD_SIZE(nw::ut::ShaderAttribute,0x10); WWHD_OFFSET(nw::ut::ShaderAttribute,location,0xC);
WWHD_SIZE(nw::ut::ShaderUniform,0x14); WWHD_OFFSET(nw::ut::ShaderUniform,location,0xC);
WWHD_SIZE(nw::ut::ShaderSampler,0xC); WWHD_OFFSET(nw::ut::ShaderSampler,location,8);
WWHD_SIZE(nw::ut::VertexShader,0x10C); WWHD_SIZE(nw::ut::PixelShader,0xD8);
WWHD_SIZE(nw::ut::Texture,0xA0); WWHD_OFFSET(nw::ut::Texture,descriptor,0x10);
WWHD_SIZE(nw::ut::QuadBatch,0xD4); WWHD_SIZE(nw::ut::Matrix4,0x40); WWHD_SIZE(nw::ut::MatrixRows3,0x30);
WWHD_SIZE(nw::ut::DrawParameters,0x54); WWHD_OFFSET(nw::ut::DrawParameters,flags,0x50);
WWHD_SIZE(nw::ut::ShadowParameters,0x1C); WWHD_OFFSET(nw::ut::ShadowParameters,advance,0x18);
