// 3D line geometry, lighting and untextured material. Private game-derived source, verified by
// m_Do_ext_11.
#pragma once
#include "bindings.h"
namespace line_mat0 {
// HD: replaces GC's position/texture arrays with double-buffered mesh vertices.
struct Vertex {
    cXyz mPosition, mTangent, mNormal;
    be<f32> mNormalW;
    u8 mAttributePadding[0x10];
    be<f32> mTexX, mTexY;
    u8 mVertexTail[0x58];
};
struct VertexTable {
    be<u32> mCount;
    gptr<Vertex> mpVertices;
    Vertex* getVertex(u32 index) {
        u32 count = mCount;
        Vertex* vertices = mpVertices;
        return index < count ? &vertices[index] : vertices;
    }
};
// HD: each mesh record owns a vertex table and two graphics resource subobjects.
struct VertexResource {
    u8 mState[0x154];
};
struct BufferState {
    u8 mState[0xF4];
};
struct MeshRecord {
    VertexTable mVertices;
    VertexResource mResource;
    BufferState mBuffer;
};
struct MeshArray {
    be<u32> mCount;
    gptr<MeshRecord> mpEntries;
    MeshRecord* getEntry(u32 index) {
        u32 count = mCount;
        MeshRecord* entries = mpEntries;
        return index < count ? &entries[index] : entries;
    }
};
struct MeshBuffer {
    MeshArray mBuffers[2];
    be<u32> mNumBuffers, mCapacity;
    u8 mState;
    be<u8> mReady;
    u8 mPadding[2];
    be<u32> mCurArr;
    VertexTable* getVertices() { return &mBuffers[mCurArr].mpEntries->mVertices; }
};
struct mDoExt_3Dline_c {
    gptr<cXyz> mpSegments;
    gptr<be<u8>> mpSize;
    gptr<MeshBuffer> mpMeshBuffer;
    u32 init(u32 count, u32 optional);
    be<u32> __vtbl;
};
struct ShaderBinding;
struct RendererManager;
struct TextureManager;
struct DrawContext;
struct TextureHandle {
    u8 mState[0x90];
};
struct mDoExt_3DlineMat0_c {
    u32 init(u32 count, u32 points, u32 optional);
    void setMaterial();
    void draw(DrawContext* arguments);
    /* 0x000 */ gptr<mDoExt_3DlineMat0_c> mpNextLineMat;
    /* 0x004 */ gptr<ShaderBinding> mpShaders;
    /* 0x008 */ gptr<RendererManager> mpRenderer;
    /* 0x00C */ gptr<TextureManager> mpTextures;
    /* 0x010 */ TextureHandle mTexObj;
    /* 0x0A0 */ TextureHandle mToonTexObj;
    /* 0x130 */ be<u32> __vtbl;
    be<u32> mColor;
    gptr<dKy_tevstr_c> mpTevStr;
    be<u16> mNumLines, mMaxSegments, mNumSegments;
    be<u8> mCurArr;
    be<u8> mFlags;
    gptr<mDoExt_3Dline_c> mpLines;
};
struct CameraView {
    u8 mCameraState[0xDC];
    cXyz mEye;
};
struct PlayView {
    u8 mPlayState[0x5FA4];
    gptr<CameraView> mView;
};
WWHD_SIZE(Vertex, 0x98);
WWHD_OFFSET(Vertex, mTexY, 0x3C);
WWHD_SIZE(VertexTable, 8);
WWHD_SIZE(MeshBuffer, 0x20);
WWHD_OFFSET(MeshBuffer, mCurArr, 0x1C);
WWHD_SIZE(mDoExt_3Dline_c, 0x10);
WWHD_SIZE(mDoExt_3DlineMat0_c, 0x148);
WWHD_OFFSET(mDoExt_3DlineMat0_c, mColor, 0x134);
WWHD_OFFSET(mDoExt_3DlineMat0_c, mpTevStr, 0x138);
WWHD_OFFSET(mDoExt_3DlineMat0_c, mpLines, 0x144);
} // namespace line_mat0
u32 texturedLineNonzero(f32);
f32 texturedLineSquareRoot(); // HD: consumes the preceding magnitude in guest
                              // FPR1.
void texturedLineNormalize(cXyz*, cXyz*);
void texturedLineScale(cXyz*, cXyz*, f32);
void texturedLineFinalize(line_mat0::MeshBuffer*, u32, u32);
void texturedLineMaterialSetup(line_mat0::mDoExt_3DlineMat0_c*, dKy_tevstr_c*, be<u32>*);

namespace line_mat0 {
// HD: graphics views inferred from the verified field accesses; untouched state
// stays opaque.
struct ShaderProgram {
    u8 mOpaque;
};
struct MaterialParameters {
    u8 mHeader[0x28];
    gptr<ShaderProgram> mpShaderProgram;
};
struct ShaderMaterial {
    be<u8> mFlags;
    u8 mFlagPadding[3];
    gptr<void> mpDisplayList;
    u8 mDisplayListState[4];
    be<u32> mDisplayListSize;
    u8 mMaterialState[0x6C];
    gptr<MaterialParameters> mpParameters;
};
struct ShaderInfo {
    u8 mShaderState[0xC];
    be<s16> mPositionIndex, mNormalIndex, mTexCoordIndex;
};
struct DrawPacket {
    gptr<ShaderMaterial> mpMaterial;
    u8 mPacketState[8];
    be<u32> mShaderInfoCount;
    gptr<ShaderInfo> mpShaderInfo;
    ShaderInfo* getShaderInfo() { return mShaderInfoCount ? mpShaderInfo.get() : nullptr; }
};
struct ShaderBindingEntry {
    gptr<DrawPacket> mpPacket;
    u8 mPacketState[4];
    // Only slots 0 and 2 are read; the middle slot's role is unconfirmed.
    gptr<void> mVertexBindings[3];
};
struct ShaderBinding {
    be<u32> mCount, mCapacity;
    gptr<ShaderBindingEntry> mpEntries;
    ShaderBindingEntry* getEntry(u32 index) {
        u32 count = mCapacity;
        ShaderBindingEntry* entries = mpEntries;
        return index < count ? &entries[index] : entries;
    }
};
struct GraphicsBuffer {
    u8 mBufferState[4];
    gptr<void> mpVertices;
    u8 mVertexState[4];
    gptr<void> mpData;
    u8 mBindingState[0xC];
};
struct RenderVtable {
    u8 mMethods[0x2C];
    be<u32> apply;
};
struct RenderInterface {
    u8 mInterfaceState[0xC];
    gptr<RenderVtable> __vtbl;
    void apply(DrawPacket* packet) { gabi::call_ptr(__vtbl->apply, this, packet); }
};
struct UniformBlock : RenderInterface {
    GraphicsBuffer mBuffers[2];
    u8 mUniformState[0x28];
    be<u32> mDirty;
};
struct Float4 {
    be<f32> values[4];
};
struct RendererRecord {
    RenderInterface mInterface;
    GraphicsBuffer mBuffers[2];
    u8 mBufferState[4];
    be<u32> mCurArr;
    u8 mRecordState[0x174];
    Float4 mAmbient, mFog;
    u8 mRecordTail[0x58];
};

struct RendererTransforms {
    Mtx34 mBaseMatrices[3];
    Float4 mDefaultVectors[2];
    Float4 mParameterPairs[3][2];
    Mtx34 mExtraMatrices[8];
    Float4 mExtraVectors[6];
};
struct RendererManager {
    be<u32> mCount;
    gptr<RendererRecord> mpEntries;
    u8 mManagerState[4];
    UniformBlock mPrimary;
    Mtx34 mScratchMatrix;
    be<u32> mMatrixDirty;
    UniformBlock mSecondary;
    RendererTransforms mTransforms;
    be<u8> mFlags;
    u8 mFlagPadding[3];
    RendererRecord* getEntry(u32 index) {
        u32 count = mCount;
        RendererRecord* entries = mpEntries;
        return index < count ? &entries[index] : entries;
    }
};
struct DrawContext {
    u8 mContextState[4];
    be<u32> mMode;
    u8 mModeState[4];
    be<u32> mPass;
    u8 mPassState[4];
    gptr<RendererManager> mpController;
    u8 mControllerState[0x18];
    gptr<RenderInterface> mpRenderer;
};
struct RenderState {
    u8 mStateHeader[8];
    be<u32> mDrawMode,
        mPassMode; // Exact graphics enum identities are unconfirmed.
    u8 mStateBody[0xDC];
    be<u32> mFlags;
    u8 mStateTail[0x2C];
};
struct ShaderCache {
    gptr<ShaderProgram> mpShaderProgram;
    gptr<ShaderMaterial> mpMaterial;
};
struct ShaderArchiveEntry {
    u8 mResourceState[0x84];
};
struct ShaderArchive {
    u8 mArchiveState[0x1C];
    be<u32> mCount;
    gptr<ShaderArchiveEntry> mpEntries;
};
struct ShaderResource {
    u8 mResourceState[0x20];
    be<u8> mInitialized;
    u8 mFlagPadding[3];
};
struct ShaderRegistry {
    u8 mRegistryState[4];
    gptr<ShaderArchive> mpArchive;
    be<u32> mCount;
    gptr<ShaderResource> mpResources;
};
struct TextureResource {
    u8 mResourceState[0x20];
    gptr<void> mpImage;
};
struct TextureManager {
    u8 mManagerState;
};
struct TextureBinding {
    u8 mBindingHeader[0x15C];
    be<u32> mWrapS, mWrapT;
    u8 mWrapState[0x2C];
    be<u8> mFlags;
};
struct ShaderNames {
    SafeString names[2];
};
struct ResourceNames {
    SafeString archive, resource;
};
struct DestructorRecord {
    u8 mRecord[12];
};
struct J3DSysView {
    u8 mOpaque;
};
struct ResourceControl {
    u8 mOpaque;
};
struct TextureLock {
    u8 mOpaque;
};
struct DrawPrimitive {
    u8 mOpaque;
};
WWHD_SIZE(TextureHandle, 0x90);
WWHD_SIZE(ShaderMaterial, 0x80);
WWHD_OFFSET(ShaderMaterial, mpParameters, 0x7C);
WWHD_SIZE(ShaderBindingEntry, 0x14);
WWHD_SIZE(ShaderBinding, 12);
WWHD_SIZE(GraphicsBuffer, 0x1C);
WWHD_SIZE(RenderInterface, 0x10);
WWHD_SIZE(UniformBlock, 0x74);
WWHD_SIZE(RendererRecord, 0x23C);
WWHD_OFFSET(RendererRecord, mCurArr, 0x4C);
WWHD_SIZE(RendererTransforms, 0x2F0);
WWHD_SIZE(RendererManager, 0x41C);
WWHD_OFFSET(RendererManager, mPrimary, 0xC);
WWHD_OFFSET(RendererManager, mScratchMatrix, 0x80);
WWHD_OFFSET(RendererManager, mSecondary, 0xB4);
WWHD_OFFSET(RendererManager, mTransforms, 0x128);
WWHD_OFFSET(RendererManager, mFlags, 0x418);
WWHD_SIZE(DrawContext, 0x34);
WWHD_OFFSET(DrawContext, mpRenderer, 0x30);
WWHD_SIZE(RenderState, 284);
WWHD_OFFSET(RenderState, mFlags, 0xEC);
WWHD_SIZE(ShaderCache, 8);
WWHD_SIZE(ShaderResource, 0x24);
WWHD_SIZE(ShaderArchiveEntry, 0x84);
WWHD_SIZE(ShaderRegistry, 16);
WWHD_OFFSET(TextureBinding, mFlags, 0x190);
WWHD_SIZE(ShaderNames, 16);
WWHD_SIZE(ResourceNames, 16);
WWHD_SIZE(DestructorRecord, 12);
} // namespace line_mat0
void texturedLineBase_ct(void*);
void texturedLineBase_dt(void*, u32);
void texturedLineZeroBytes(void*, u32);
void* texturedLineAllocArray(void*, u32, u32, u32);
void texturedLineDestroyArray(void*, s32, u32, u32, u32, u32);
void texturedLineBufferDestroy(line_mat0::GraphicsBuffer*);
void texturedLineUniform_ct(line_mat0::UniformBlock*);
void texturedLineUniform_dt(line_mat0::UniformBlock*, u32);
void texturedLineRenderer_ct(line_mat0::RendererManager*);
void texturedLineRenderer_dt(line_mat0::RendererManager*, u32);
void texturedLineRendererInit(line_mat0::RendererManager*, u32, u32);
void texturedLineRendererPrepare(line_mat0::RendererManager*, line_mat0::DrawPacket*, u32);
line_mat0::TextureManager* texturedLineTextureManager_ct(u32);
void texturedLineTextureManager_dt(line_mat0::TextureManager*, u32);
void texturedLineSetTexture(line_mat0::TextureManager*, u32, line_mat0::TextureHandle*, u32);
void texturedLinePrepareTextures(line_mat0::TextureManager*, line_mat0::DrawPacket*);
line_mat0::TextureBinding* texturedLineGetTexture(line_mat0::TextureManager*, u32);
line_mat0::ShaderRegistry* texturedLineGetShaderRegistry();
u32 texturedLineFindShader(line_mat0::ShaderArchive*, SafeString*);
void texturedLineInitializeShader(line_mat0::ShaderResource*, line_mat0::ShaderArchiveEntry*, u32);
void texturedLineBindShader(line_mat0::ShaderBinding*, line_mat0::ShaderResource*, u32);
u32 texturedLineInitGeometry(line_mat0::mDoExt_3Dline_c*, u32, u32, u32);
void texturedLineInitMesh(line_mat0::MeshBuffer*, u32, u32, u32);
void texturedLineBindMesh(line_mat0::ShaderBindingEntry*, line_mat0::DrawPacket*,
                          line_mat0::MeshBuffer*, u32);
void* texturedLineGetResource(line_mat0::ResourceControl*, SafeString*, SafeString*, u32);
void texturedLineLoadTexture(line_mat0::TextureHandle*, void*, const char*);
void texturedLineUseImage(line_mat0::TextureHandle*, void*);
void texturedLineTextureLock(line_mat0::TextureLock*);
void texturedLineTextureUnlock(line_mat0::TextureLock*);
void texturedLineSetLighting();
void texturedLineSetFog(dKy_tevstr_c*);
void texturedLineSetDefaultFog();
line_mat0::ShaderCache* texturedLineGetShaderCache(line_mat0::J3DSysView*);
void texturedLineInvalidateMaterial(line_mat0::ShaderMaterial*, u32);
void texturedLineUseShader(line_mat0::ShaderProgram*);
void texturedLineUseMaterial(line_mat0::ShaderMaterial*);
void texturedLineCallDisplayList(void*, u32);
void texturedLineBindNormal(s32, void*, void*);
void texturedLineBindPosition(s32, void*, void*);
void texturedLineBindTexCoord(s32, void*, void*);
void texturedLineInitRenderState(line_mat0::RenderState*);
void texturedLineSetRenderState(u32, line_mat0::RenderState*);
void texturedLineApplyRenderState(line_mat0::RenderState*);
void texturedLineUseVertexBuffer(void*);
void texturedLineDrawMesh(line_mat0::MeshBuffer*, line_mat0::DrawPrimitive*, u32, u32);
void texturedLinePrepareContext(line_mat0::DrawContext*, line_mat0::DrawPacket*);

namespace line_mat0 {
struct MaterialUniforms {
    Float4 mRows[4];
    Float4 mColorC0, mColorK0, mColor;
};
struct LightWords {
    be<u32> words[16];
};
struct LightingWork {
    Float4 color;
    Mtx34 matrix;
    LightWords lights;
};
struct LightExtra {
    u8 mHeader[0x240];
    u8 mExtra;
};
struct LightingSource {
    u8 mState[0x148];
    gptr<LightExtra> mpExtra;
    LightWords mLights;
};
struct LightingView {
    u8 mHeader[0x24];
    be<f32> mColorK0Scale, mAmbientScale;
    u8 mLightState[0x64];
    GXColorS10 mColorC0;
    GXColor mColorK0;
    u8 mTail[0xC4];
    GXColorS10 mAmbientColor;
    u8 mAmbientState[4];
    be<f32> mFogScale;
};
struct HeapVtable {
    u8 mMethods[0x3C];
    be<u32> free;
};
struct HeapManager {
    u8 mState[0xC];
    gptr<HeapVtable> __vtbl;
    void free(void* allocation) { gabi::call_ptr(__vtbl->free, this, allocation); }
};
struct HeapRegistry {
    u8 mState;
};
WWHD_SIZE(MeshRecord, 0x250);
WWHD_OFFSET(MeshRecord, mResource, 8);
WWHD_OFFSET(MeshRecord, mBuffer, 0x15C);
WWHD_SIZE(LightingWork, 128);
WWHD_OFFSET(LightingView, mColorC0, 0x90);
WWHD_OFFSET(LightingView, mColorK0, 0x98);
WWHD_OFFSET(LightingView, mAmbientColor, 0x160);
WWHD_OFFSET(LightingView, mFogScale, 0x16C);
WWHD_OFFSET(RendererRecord, mAmbient, 0x1C4);
WWHD_OFFSET(RendererRecord, mFog, 0x1D4);
} // namespace line_mat0

void* lineAllocateBytes(u32);
void lineFreeBytes(void*);
line_mat0::MeshBuffer* lineCreateMesh(u32);
void lineAllocateMesh(line_mat0::MeshBuffer*, u32, u32, u32);
void lineUploadMesh(line_mat0::MeshBuffer*);
void lineReleaseBuffer(line_mat0::BufferState*);
void lineBuffer_dt(line_mat0::BufferState*, u32);
void lineResource_dt(line_mat0::VertexResource*, u32);
line_mat0::HeapManager* lineFindHeap(line_mat0::HeapRegistry*, void*);
void lineMesh_dt(line_mat0::MeshBuffer*, u32);
void lineTexture_ct(line_mat0::TextureHandle*);
void lineAssert(const char*, u32, const char*);
void lineMatrixIdentity(Mtx34*);
void lineDivideColor(line_mat0::Float4*, f32);
void lineScaleColor(line_mat0::Float4*, f32);
void lineBuildLightingMatrix(Mtx34*, Mtx34*, line_mat0::J3DSysView*);
void lineApplyLighting(line_mat0::RendererManager*, u32, Mtx34*, line_mat0::LightWords*, void*);
void lineMixColor(line_mat0::Float4*, line_mat0::Float4*, f32);
void lineCommitRenderer(line_mat0::RendererManager*, u32);

WWHD_OFFSET(line_mat0::Vertex, mTexX, 0x38);
WWHD_OFFSET(line_mat0::RendererManager, mMatrixDirty, 0xB0);
