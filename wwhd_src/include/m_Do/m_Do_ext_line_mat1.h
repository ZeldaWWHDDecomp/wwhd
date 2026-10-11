// Textured 3D line material. Private game-derived source, verified by
// m_Do_ext_13.
#pragma once
#include "bindings.h"
namespace line_mat1 {
// HD: replaces GC's position/texture arrays with double-buffered mesh vertices.
struct Vertex {
    cXyz mPosition, mTangent, mNormal;
    be<f32> mNormalW;
    u8 mAttributePadding[0x14];
    be<f32> mTexY;
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
struct MeshBuffer {
    struct Buffer {
        u8 mHeader[4];
        gptr<VertexTable> mpVertices;
    };
    Buffer mBuffers[2];
    u8 mBufferState[0xC];
    be<u32> mCurArr;
    VertexTable* getVertices() { return mBuffers[mCurArr].mpVertices; }
};
struct mDoExt_3Dline_c {
    gptr<cXyz> mpSegments;
    gptr<be<u8>> mpSize;
    gptr<MeshBuffer> mpMeshBuffer;
    u8 mLineState[4];
};
struct ShaderBinding;
struct RendererManager;
struct TextureManager;
struct DrawContext;
struct TextureHandle {
    u8 mState[0x90];
};
struct mDoExt_3DlineMat1_c {
    // HD: retain the original full-width arguments and GPR/FPR order.
    void update(u32 count, const be<u32>* color, u32 taper, dKy_tevstr_c* lighting, f32 width);
    void updateWidths(u32 count, const be<u32>* color, dKy_tevstr_c* lighting);
    u32 init(u32 count, u32 points, void* texture, u32 optional);
    void setMaterial();
    void draw(DrawContext* arguments);
    /* 0x000 */ gptr<mDoExt_3DlineMat1_c> mpNextLineMat;
    /* 0x004 */ gptr<ShaderBinding> mpShaders;
    /* 0x008 */ gptr<RendererManager> mpRenderer;
    /* 0x00C */ gptr<TextureManager> mpTextures;
    /* 0x010 */ TextureHandle mTexObj;
    /* 0x0A0 */ TextureHandle mToonTexObj;
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 mMaterialCache[0x40];
    be<u32> mColor;
    gptr<dKy_tevstr_c> mpTevStr;
    be<u16> mNumLines, mMaxSegments, mNumSegments;
    be<u8> mCurArr;
    be<u8> mFlags;
    gptr<mDoExt_3Dline_c> mpLines;
};
struct LineWork {
    cXyz cross, tangent, partial, normal, scaled, norm2;
    cXyz averagePlus, averageMinus, temp, eye, out, norm;
};
struct WidthWork {
    cXyz cross, tangent, partial, unused, normal, scaled, norm2;
    cXyz averagePlus, averageMinus, temp, eye, out, norm;
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
WWHD_SIZE(mDoExt_3DlineMat1_c, 0x188);
WWHD_OFFSET(mDoExt_3DlineMat1_c, mColor, 0x174);
WWHD_OFFSET(mDoExt_3DlineMat1_c, mpTevStr, 0x178);
WWHD_OFFSET(mDoExt_3DlineMat1_c, mpLines, 0x184);
WWHD_SIZE(LineWork, 144);
WWHD_SIZE(WidthWork, 156);
WWHD_OFFSET(LineWork, normal, 0x24);
WWHD_OFFSET(WidthWork, normal, 0x30);
} // namespace line_mat1
u32 texturedLineNonzero(f32);
f32 texturedLineSquareRoot(); // HD: consumes the preceding magnitude in guest
                              // FPR1.
void texturedLineNormalize(cXyz*, cXyz*);
void texturedLineScale(cXyz*, cXyz*, f32);
void texturedLineFinalize(line_mat1::MeshBuffer*, u32, u32);
void texturedLineMaterialSetup(line_mat1::mDoExt_3DlineMat1_c*, dKy_tevstr_c*, be<u32>*);

namespace line_mat1 {
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
    u8 mUniformState[0x2C];
};
struct RendererRecord {
    RenderInterface mInterface;
    GraphicsBuffer mBuffers[2];
    u8 mBufferState[4];
    be<u32> mCurArr;
    u8 mRecordState[0x1EC];
};
struct Float4 {
    be<f32> values[4];
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
    u8 mScratch[0x34];
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
WWHD_OFFSET(RendererManager, mScratch, 0x80);
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
} // namespace line_mat1
void texturedLineBase_ct(void*);
void texturedLineBase_dt(void*, u32);
void texturedLineZeroBytes(void*, u32);
void* texturedLineAllocArray(void*, u32, u32, u32);
void texturedLineDestroyArray(void*, s32, u32, u32, u32, u32);
void texturedLineBufferDestroy(line_mat1::GraphicsBuffer*);
void texturedLineUniform_ct(line_mat1::UniformBlock*);
void texturedLineUniform_dt(line_mat1::UniformBlock*, u32);
void texturedLineRenderer_ct(line_mat1::RendererManager*);
void texturedLineRenderer_dt(line_mat1::RendererManager*, u32);
void texturedLineRendererInit(line_mat1::RendererManager*, u32, u32);
void texturedLineRendererPrepare(line_mat1::RendererManager*, line_mat1::DrawPacket*, u32);
line_mat1::TextureManager* texturedLineTextureManager_ct(u32);
void texturedLineTextureManager_dt(line_mat1::TextureManager*, u32);
void texturedLineSetTexture(line_mat1::TextureManager*, u32, line_mat1::TextureHandle*, u32);
void texturedLinePrepareTextures(line_mat1::TextureManager*, line_mat1::DrawPacket*);
line_mat1::TextureBinding* texturedLineGetTexture(line_mat1::TextureManager*, u32);
line_mat1::ShaderRegistry* texturedLineGetShaderRegistry();
u32 texturedLineFindShader(line_mat1::ShaderArchive*, SafeString*);
void texturedLineInitializeShader(line_mat1::ShaderResource*, line_mat1::ShaderArchiveEntry*, u32);
void texturedLineBindShader(line_mat1::ShaderBinding*, line_mat1::ShaderResource*, u32);
u32 texturedLineInitGeometry(line_mat1::mDoExt_3Dline_c*, u32, u32, u32);
void texturedLineInitMesh(line_mat1::MeshBuffer*, u32, u32, u32);
void texturedLineBindMesh(line_mat1::ShaderBindingEntry*, line_mat1::DrawPacket*,
                          line_mat1::MeshBuffer*, u32);
void* texturedLineGetResource(line_mat1::ResourceControl*, SafeString*, SafeString*, u32);
void texturedLineLoadTexture(line_mat1::TextureHandle*, void*, const char*);
void texturedLineUseImage(line_mat1::TextureHandle*, void*);
void texturedLineTextureLock(line_mat1::TextureLock*);
void texturedLineTextureUnlock(line_mat1::TextureLock*);
void texturedLineSetLighting();
void texturedLineSetFog(dKy_tevstr_c*);
void texturedLineSetDefaultFog();
line_mat1::ShaderCache* texturedLineGetShaderCache(line_mat1::J3DSysView*);
void texturedLineInvalidateMaterial(line_mat1::ShaderMaterial*, u32);
void texturedLineUseShader(line_mat1::ShaderProgram*);
void texturedLineUseMaterial(line_mat1::ShaderMaterial*);
void texturedLineCallDisplayList(void*, u32);
void texturedLineBindNormal(s32, void*, void*);
void texturedLineBindPosition(s32, void*, void*);
void texturedLineBindTexCoord(s32, void*, void*);
void texturedLineInitRenderState(line_mat1::RenderState*);
void texturedLineSetRenderState(u32, line_mat1::RenderState*);
void texturedLineApplyRenderState(line_mat1::RenderState*);
void texturedLineUseVertexBuffer(void*);
void texturedLineDrawMesh(line_mat1::MeshBuffer*, line_mat1::DrawPrimitive*, u32, u32);
void texturedLinePrepareContext(line_mat1::DrawContext*, line_mat1::DrawPacket*);
