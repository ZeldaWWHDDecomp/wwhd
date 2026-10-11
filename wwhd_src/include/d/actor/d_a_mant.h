#pragma once
#include "bindings.h"

// HD: GX2 resource views are private to the cape translation unit.
namespace mantHD {
struct SinCosTable {
    struct Entry {
        be<f32> mSin, mCos;
    };
    Entry value[8192];
};
struct FloatTable {
    be<f32> value[9];
};
struct VertexIndices {
    struct Entry {
        be<u8> position, normal, texture;
    };
    Entry value[144];
};
struct TextureCoordinates {
    struct Entry {
        be<f32> u, v;
    };
    Entry value[81];
};
struct AttackInfo {
    be<u32> mpObject;
    u8 padding[6];
    be<u8> mHitType;
    u8 padding2[0x11];
};
WWHD_SIZE(AttackInfo, 0x1C);
WWHD_OFFSET(AttackInfo, mHitType, 0xA);

struct AttributeBuffer {
    u8 data[0xF0];
    be<u32> mVtable;
};
struct ObjectBuffer {
    be<s32> mCount;
    gptr<AttributeBuffer> mpObjects;
};
struct ShaderMode {
    u8 padding[0x28];
    be<u32> mMode;
};
struct Shader {
    be<u8> mFlags;
    u8 padding[3];
    gptr<void> mpDisplayList;
    u8 padding2[4];
    be<u32> mDisplayListSize;
    u8 padding3[0x7C - 0x10];
    gptr<ShaderMode> mpMode;
};
struct ShaderBinding {
    u8 padding[0xC];
    be<s16> mVertex, mPixel, mGeometry;
};
struct Program {
    gptr<Shader> mpShader;
    u8 padding[8];
    be<u32> mUniformCount;
    gptr<ShaderBinding> mpUniforms;
    be<u32> mSamplerCount;
    gptr<void> mpSamplers;
};
struct ShaderCache {
    be<u32> mMode;
    gptr<Shader> mpShader;
};
struct SafeString {
    gptr<char> mpString;
    be<u32> mVtable;
};
struct TextureNames {
    SafeString value[7];
};
struct ShaderResource {
    u8 data[0x84];
};
struct ShaderArchive {
    u8 padding[0x1C];
    be<u32> mCount;
    gptr<ShaderResource> mpResources;
};
struct ShaderRecord {
    u8 data[0x20];
    be<u8> mReady;
    u8 padding[3];
};
struct ShaderManager {
    u8 padding[4];
    gptr<ShaderArchive> mpArchive;
    be<u32> mCount;
    gptr<ShaderRecord> mpRecords;
};
struct VirtualObject {
    u8 padding[0xC];
    be<u32> mVtable;
};
struct HeapVtable {
    u8 padding[0x34];
    be<u32> mAllocate;
    u8 padding2[4];
    be<u32> mFree;
};
struct AttributeVtable {
    u8 padding[0xC];
    be<u32> mDestroy;
};
struct UniformVtable {
    u8 padding[0x2C];
    be<u32> mBind;
};
struct StringVtable {
    u8 padding[0x14];
    be<u32> mAssureTermination;
};
struct InitialMaterial {
    be<f32> mValues[44];
};
WWHD_SIZE(SafeString, 8);
WWHD_SIZE(ShaderRecord, 0x24);
WWHD_SIZE(ShaderResource, 0x84);
struct ProgramEntry {
    gptr<Program> mpProgram;
    ObjectBuffer mAttributes[2];
};
struct EntryBuffer {
    be<s32> mCount;
    gptr<ProgramEntry> mpEntries;
};
struct ProgramList {
    be<u32> mSize;
    be<s32> mCount;
    gptr<ProgramEntry> mpEntries;
};
struct GpuVertexBuffer {
    u8 data[0x14C];
    be<u32> mDirty;
    u8 padding[4];
};
struct VertexBuffer {
    gptr<void> mpVertices;
    GpuVertexBuffer mBuffer;
    AttributeBuffer mAttributes;
    be<u32> mCapacity;
    gptr<void> mpAllocation;
};
struct DoubleBuffer {
    VertexBuffer mBuffer[2];
    be<u32> mActive;
    be<u32> mFormat;
    be<u32> mAlignment;
    gptr<void> mpAttributes;
    be<u32> mCount;
    be<u8> mReady;
    u8 padding[3];
};
struct UniformEntry {
    be<u32> mMode;
    be<u32> mSize;
    be<u32> mAllocation;
    gptr<void> mpData;
    u8 padding[0xC];
};
struct Color {
    be<f32> component[4];
};
struct TevLightParameters {
    u8 padding[0x24];
    be<f32> mMaterialScale;
    be<f32> mLightScale;
    u8 padding2[0x160 - 0x2C];
    GXColorS10 mSecondColor;
    u8 padding3[4];
    be<f32> mSecondLightScale;
};
WWHD_OFFSET(TevLightParameters, mSecondColor, 0x160);
WWHD_OFFSET(TevLightParameters, mSecondLightScale, 0x16C);
struct UniformBuffer {
    u8 padding[0x10];
    UniformEntry mBuffer[2];
    be<u32> mReserved;
    be<u32> mActive;
    u8 padding2[0x1C4 - 0x50];
    Color mColor[2];
    u8 padding3[0x23C - 0x1E4];
};
struct LightBuffers {
    be<s32> mCount;
    gptr<UniformBuffer> mpBuffers;
    u8 padding[4];
};
struct MatrixBlock {
    u8 padding[0xC];
    be<u32> mVtable;
    UniformEntry mBuffer[2];
    u8 padding2[0x74 - 0x48];
    Mtx34 mMtx;
    be<u32> mEnabled;
};
struct MaterialBlock {
    u8 padding[0xC];
    be<u32> mVtable;
    UniformEntry mBuffer[2];
    u8 padding2[0x74 - 0x48];
    be<f32> mMatrices[16];
    Color mColor[3];
    u8 padding3[0x364 - 0xE4];
};
struct IndexBuffer {
    be<u32> mPrimitive;
    be<u32> mFormat;
    gptr<void> mpIndices;
    be<u32> mCount;
    u8 padding[8];
};
struct TextureShape {
    be<u32> mMode, mDimension, mWidth, mHeight, mDepth, mMipLevels, mFormat, mAA;
    be<u32> mUsage, mImageSize;
    gptr<void> mpImage;
    be<u32> mMipSize;
    gptr<void> mpMipmaps;
    be<u32> mTileMode, mSwizzle;
};
struct Texture {
    TextureShape mShape;
    u8 padding[0x90 - 0x3C];
};
struct Sampler {
    TextureShape mShape;
    u8 padding[0xD4 - 0x3C];
    gptr<void> mpImage;
    be<u32> mReserved;
    gptr<void> mpMipmaps;
    u8 padding2[0x198 - 0xE0];
};
struct Vertex {
    cXyz mPos;
    cXyz mNrm;
    be<f32> mU, mV;
};
struct RenderPass {
    u8 padding[0xC];
    be<s32> mStage;
    u8 padding2[4];
    gptr<LightBuffers> mpLights;
    u8 padding3[0x30 - 0x18];
    gptr<MatrixBlock> mpMatrixBlock;
};
struct RenderState {
    u8 padding[8];
    be<u32> mDepthMode;
    be<u32> mCullMode;
    u8 padding2[0xE0 - 0x10];
    be<u8> mAlphaTest;
    u8 padding3[3];
    be<u32> mAlphaFunction;
    be<f32> mAlphaReference;
    be<u32> mMask;
    u8 padding4[0x120 - 0xF0];
};
WWHD_SIZE(RenderState, 0x120);
WWHD_OFFSET(RenderPass, mpMatrixBlock, 0x30);

WWHD_SIZE(SinCosTable, 0x10000);
WWHD_SIZE(FloatTable, 0x24);
WWHD_SIZE(VertexIndices, 0x1B0);
WWHD_SIZE(TextureCoordinates, 0x288);
WWHD_SIZE(ShaderMode, 0x2C);
WWHD_SIZE(Shader, 0x80);
WWHD_SIZE(ShaderBinding, 0x12);
WWHD_SIZE(Program, 0x1C);
WWHD_SIZE(ShaderCache, 8);
WWHD_SIZE(TextureNames, 0x38);
WWHD_SIZE(ShaderArchive, 0x24);
WWHD_SIZE(ShaderManager, 0x10);
WWHD_SIZE(VirtualObject, 0x10);
WWHD_SIZE(HeapVtable, 0x40);
WWHD_SIZE(AttributeVtable, 0x10);
WWHD_SIZE(UniformVtable, 0x30);
WWHD_SIZE(StringVtable, 0x18);
WWHD_SIZE(InitialMaterial, 0xB0);
WWHD_SIZE(EntryBuffer, 8);
WWHD_SIZE(GpuVertexBuffer, 0x154);
WWHD_SIZE(Color, 0x10);
WWHD_SIZE(TevLightParameters, 0x170);
WWHD_SIZE(RenderPass, 0x34);
WWHD_SIZE(AttributeBuffer, 0xF4);
WWHD_SIZE(ObjectBuffer, 8);
WWHD_SIZE(ProgramEntry, 0x14);
WWHD_SIZE(ProgramList, 0xC);
WWHD_OFFSET(VertexBuffer, mBuffer, 4);
WWHD_OFFSET(VertexBuffer, mAttributes, 0x158);
WWHD_OFFSET(VertexBuffer, mCapacity, 0x24C);
WWHD_OFFSET(VertexBuffer, mpAllocation, 0x250);
WWHD_SIZE(VertexBuffer, 0x254);
WWHD_OFFSET(DoubleBuffer, mActive, 0x4A8);
WWHD_SIZE(DoubleBuffer, 0x4C0);
WWHD_SIZE(UniformEntry, 0x1C);
WWHD_OFFSET(UniformBuffer, mActive, 0x4C);
WWHD_OFFSET(UniformBuffer, mColor, 0x1C4);
WWHD_SIZE(UniformBuffer, 0x23C);
WWHD_SIZE(LightBuffers, 0xC);
WWHD_OFFSET(MatrixBlock, mMtx, 0x74);
WWHD_OFFSET(MatrixBlock, mEnabled, 0xA4);
WWHD_SIZE(MatrixBlock, 0xA8);
WWHD_OFFSET(MaterialBlock, mColor, 0xB4);
WWHD_SIZE(MaterialBlock, 0x364);
WWHD_SIZE(IndexBuffer, 0x18);
WWHD_SIZE(TextureShape, 0x3C);
WWHD_SIZE(Texture, 0x90);
WWHD_OFFSET(Sampler, mpImage, 0xD4);
WWHD_SIZE(Sampler, 0x198);
WWHD_SIZE(Vertex, 0x20);
} // namespace mantHD

// HD: The packet retains cape positions/normals and adds GX2 shader resources.
struct daMant_packet_c {
    /* 0x000 */ u8 padding[0xC];
    /* 0x00C */ be<u32> mVtable;
    /* 0x010 */ u8 paddingBase[0x98 - 0x10];
    /* 0x098 */ Mtx34 mMtx;
    /* 0x0C8 */ Mtx34 mMtx2;
    /* 0x0F8 */ gptr<dKy_tevstr_c> mpTevStr;
    /* 0x0FC */ cXyz mPos[81];
    /* 0x4C8 */ cXyz mNrm[81];
    /* 0x894 */ be<u8> mTexNo;
    /* 0x895 */ be<u8> mType;
    /* 0x896 */ u8 paddingType[2];
    /* 0x898 */ mantHD::ProgramList mPrograms[3];
    /* 0x8BC */ mantHD::DoubleBuffer mVertices;
    /* 0xD7C */ mantHD::LightBuffers mLightBuffers;
    /* 0xD88 */ mantHD::MatrixBlock mMatrixBlock[3];
    /* 0xF80 */ mantHD::MaterialBlock mMaterialBlock[3];
    /* 0x19AC */ mantHD::IndexBuffer mIndices;
    /* 0x19C4 */ mantHD::Sampler mSampler[7];
    /* 0x24EC */ mantHD::Texture mTexture[7];
    void draw();
    void setupUniforms(void*, void*, u32);
    void setUniforms(void*);
};
WWHD_OFFSET(daMant_packet_c, mPos, 0xFC);
WWHD_OFFSET(daMant_packet_c, mNrm, 0x4C8);
WWHD_OFFSET(daMant_packet_c, mTexNo, 0x894);
WWHD_OFFSET(daMant_packet_c, mPrograms, 0x898);
WWHD_OFFSET(daMant_packet_c, mVertices, 0x8BC);
WWHD_OFFSET(daMant_packet_c, mLightBuffers, 0xD7C);
WWHD_OFFSET(daMant_packet_c, mMatrixBlock, 0xD88);
WWHD_OFFSET(daMant_packet_c, mMaterialBlock, 0xF80);
WWHD_OFFSET(daMant_packet_c, mIndices, 0x19AC);
WWHD_OFFSET(daMant_packet_c, mSampler, 0x19C4);
WWHD_OFFSET(daMant_packet_c, mTexture, 0x24EC);
WWHD_SIZE(daMant_packet_c, 0x28DC);

struct mant_j_s {
    /* 0x000 */ cXyz mPos[9];
    /* 0x06C */ cXyz mVelocity[9];
    /* 0x0D8 */ csXyz mAngle;
    /* 0x0DE */ u8 padding[0x110 - 0xDE];
};
WWHD_OFFSET(mant_j_s, mVelocity, 0x6C);
WWHD_OFFSET(mant_j_s, mAngle, 0xD8);
WWHD_SIZE(mant_j_s, 0x110);

// HD: Expanded actor/packet storage places the original tail at its HD offsets.
struct mant_class : fopAc_ac_c {
    enum CapeType { Type_DARKNUT_e, Type_PHANTOM_GANON_e };
    /* 0x03AC */ u8 padding[8];
    /* 0x03B4 */ daMant_packet_c mPacket;
    /* 0x2C90 */ be<u8> mType;
    /* 0x2C91 */ u8 paddingType[3];
    /* 0x2C94 */ be<u32> mTick;
    /* 0x2C98 */ u8 paddingTick[4];
    /* 0x2C9C */ mant_j_s mJoint[9];
    /* 0x362C */ cXyz m1BE0;
    /* 0x3638 */ cXyz m1BEC;
    /* 0x3644 */ be<f32> m1BF8;
    /* 0x3648 */ be<f32> m1BFC;
    /* 0x364C */ be<f32> mWindAmplitude;
    /* 0x3650 */ be<f32> mWindGust;
    /* 0x3654 */ be<f32> mGravity;
    /* 0x3658 */ be<s16> mWindHeading;
    /* 0x365A */ be<s16> m1C0E;
    /* 0x365C */ dCcD_Stts mStts;
    /* 0x3698 */ dCcD_Sph mWindSph;
    /* 0x37C4 */ dCcD_Sph mMeshSph[9];
    /* 0x4250 */ Mtx34 mMtx;
    /* 0x4280 */ be<s8> m2834;
    /* 0x4281 */ u8 paddingState;
    /* 0x4282 */ be<s16> m2836;
    /* 0x4284 */ be<s16> mBurnTimer;
    /* 0x4286 */ u8 paddingEnd[2];
};
WWHD_OFFSET(mant_class, mPacket, 0x3B4);
WWHD_OFFSET(mant_class, mType, 0x2C90);
WWHD_OFFSET(mant_class, mJoint, 0x2C9C);
WWHD_OFFSET(mant_class, m1BE0, 0x362C);
WWHD_OFFSET(mant_class, m1BF8, 0x3644);
WWHD_OFFSET(mant_class, mStts, 0x365C);
WWHD_OFFSET(mant_class, mMeshSph, 0x37C4);
WWHD_OFFSET(mant_class, mMtx, 0x4250);
WWHD_OFFSET(mant_class, m2834, 0x4280);
WWHD_OFFSET(mant_class, mBurnTimer, 0x4284);
WWHD_SIZE(mant_class, 0x4288);
