/* GPU render-state registers, file resources (textures, fetch shaders, buffers, meshes) and the
   animation runtime of the graphics utility library, reconstructed from the WWHD binary.
   Runtime objects shared with the buffer/pose/model code (PaletteBuffer, ModelRuntime, UniformBuffer,
   StridedBuffer, MatrixUtility, AnimationClock, BindingTable, BufferView) are the owner's types in
   BufferRuntime.h / RuntimeUtility.h / ResourceLayout.h / PoseRecords.h; the file records shared with them
   (NameLink, ResourceDictionary, UniformBinding, ParameterDescriptor, ModelDescriptor) are in
   ResourceLayout.h / ModelRuntime.h. */
#pragma once
#include "wwhd.h"
#include "agl/gx2/BufferRuntime.h"
#include "agl/gx2/ModelRuntime.h"
#include "agl/gx2/RuntimeUtility.h"
#include "agl/gx2/PoseRecords.h"
#include "agl/gx2/PoseTrigTable.h"
namespace agl { namespace gx2util {

/* ---- render-state registers ---- */
struct PolygonState {
    be<u32> mControl;
    u32 isFrontCulled() const;
    u32 isBackCulled() const;
    void setFrontOffset(u32 enabled);
    void setBackOffset(u32 enabled);
    void bind();
};
struct DepthState {
    be<u32> mControl;
    void bind();
    void setDepthTest(u32 value);
    void setDepthWrite(u32 value);
    void setDepthCompare(u32 value);
    u32 isDepthTestEnabled() const;
};
struct AlphaState { be<u32> mControl; be<u32> mReference; void bind(); void setEnabled(u32 enabled); };
struct ColorState {
    be<u32> mControl;
    void bind(); void setLogicOperation(u32 operation); void setBlendMask(u32 mask);
    void setColorWrite(u32 enabled); u32 getBlendMask() const;
};
struct BlendState {
    be<u32> mTarget; be<u32> mControl;
    void bind(); void bindTarget(u32 target);
    void setColorSource(u32 value); void setColorDestination(u32 value);
    void setSeparateAlpha(u32 value); void setAlphaSource(u32 value); void setAlphaDestination(u32 value);
};
struct BlendColor { be<u32> mComponents[4]; void bind(); };
struct PolygonOffset {
    be<f32> mFrontScale; be<f32> mFrontOffset; be<f32> mBackScale; be<f32> mBackOffset; be<u32> mClamp;
    void clear(); void bind();
    void setFrontScale(f32 scale); void setFrontOffset(f32 offset); void setBackScale(f32 scale); void setBackOffset(f32 offset);
};
WWHD_OFFSET(BlendState, mControl, 4);
WWHD_OFFSET(PolygonOffset, mBackOffset, 12);
WWHD_SIZE(PolygonState, 4); WWHD_SIZE(DepthState, 4); WWHD_SIZE(AlphaState, 8);
WWHD_SIZE(ColorState, 4); WWHD_SIZE(BlendState, 8); WWHD_SIZE(BlendColor, 16); WWHD_SIZE(PolygonOffset, 20);

struct RenderPipelineState {
    be<u32> mFlags; PolygonState mPolygon; DepthState mDepth; AlphaState mAlpha;
    ColorState mColor; BlendState mBlend; BlendColor mBlendColor;
    void setBlendMode(u32 mode); void setMaterialMode(u32 mode); void bind();
};
WWHD_OFFSET(RenderPipelineState, mColor, 0x14); WWHD_OFFSET(RenderPipelineState, mBlendColor, 0x20);
WWHD_SIZE(RenderPipelineState, 0x30);

/* ---- timers ---- */
struct SystemTimingInfo { be<u32> mBusClock; };
struct TimerClock { be<u32> mTicks; be<u32> mFrequency; void initialize(); };
struct TimerCounter { be<u32> mHigh; be<u32> mLow; };
WWHD_SIZE(TimerClock, 8); WWHD_SIZE(TimerCounter, 8);

/* ---- textures, samplers, fetch shaders ---- */
struct TextureResource {
    u8 mSurfacePrefix[0x10]; be<u32> mMipCount; u8 mSurfaceDetails[0xC];
    be<u32> mImageSize; gptr<void> mImage; be<u32> mMipSize; gptr<void> mMipImage;
    u8 mLayoutDetails[0x10]; be<u32> mMipOffset;
    void initializeRegisters(); void updateRegisters(); void calculateLayout();
    void* getImage() const; void* getMipImage() const; void flush();
    void bindVertex(u32 slot); void bindGeometry(u32 slot); void bindPixel(u32 slot);
    void setImages(void* image, void* mip);
};
WWHD_OFFSET(TextureResource, mImageSize, 0x20); WWHD_OFFSET(TextureResource, mMipOffset, 0x40);
WWHD_SIZE(TextureResource, 0x44);

/* The name text a NameLink (ResourceLayout.h) points at: the 32-bit byte count, then the text. */
struct KeyBits {
    be<u32> mByteCount; be<u8> mBytes[1];
    u32 test(u32 bit) const;
};
WWHD_OFFSET(KeyBits, mBytes, 4);

struct TextureFileResource {
    be<u32> mTag; TextureResource mTexture; u8 mSurfaceDetails[0x68];
    ResourceLink<void> mImage; ResourceLink<void> mMip;
    void initialize();
};
WWHD_OFFSET(TextureFileResource, mImage, 0xB0); WWHD_OFFSET(TextureFileResource, mMip, 0xB4);

struct SamplerState {
    be<u32> mFilter; be<u32> mLod;
    void bindVertex(u32 slot); void bindGeometry(u32 slot); void bindPixel(u32 slot); f32 getLodBias() const;
};
struct FetchStream { be<u32> mInstruction; be<u32> mFormat; be<u32> mDivisor; be<u32> mReserved; };
struct AttributeEncoding { be<u8> mCategory; be<u8> mChannels; be<u8> mFetchKind; be<u8> mSignMode; be<u32> mSelectors; };
WWHD_SIZE(FetchStream, 16); WWHD_SIZE(AttributeEncoding, 8); WWHD_SIZE(SamplerState, 8);
struct FetchResource {
    u8 mShaderPrefix[8]; be<u32> mSize; gptr<void> mProgram;
    be<u32> mStreamCount; u8 mShaderDetails[12]; be<u32> mProgramHeaderSize; be<u32> mProgramFlags;
    void* getProgram() const; void flush(); void calculateSize(); void bind();
    void setProgram(void* program); FetchStream* getStream(void* program, u32 stream); void clearStream(u32 stream);
    void initializeProgram(be<u32>* program); void setStreamBuffer(void* program, u32 stream, u32 buffer);
    void setStreamLocation(void* program, u32 stream, u32 location);
    void setStreamFormat(void* program, u32 stream, u32 format); void setStreamDivisor(void* program, u32 stream, u32 divisor);
};
WWHD_OFFSET(FetchResource, mProgram, 0xC); WWHD_OFFSET(FetchResource, mProgramHeaderSize, 0x20); WWHD_SIZE(FetchResource, 0x28);
FetchResource* createFetchResource(FetchResource* resource);

/* ---- buffers and meshes of a model file ---- */
struct FileBufferResource { StridedBuffer mGpu; be<u32> mFlags; ResourceLink<void> mData; void initialize(); };
struct VertexResource {
    u8 mHeader[5]; be<u8> mBufferCount; u8 mVertexHeader[0x12]; ResourceLink<FileBufferResource> mBuffers; be<u32> mReserved;
    void initializeBuffers();
};
struct MeshBufferRecord { u8 mHeader[0x14]; ResourceLink<FileBufferResource> mBuffer; u8 mTail[4]; };
struct IndexRange { be<u32> mByteOffset; be<u32> mCount; };
struct IndexedMesh {
    be<u32> mPrimitive; be<u32> mIndexFormat; u8 mHeader[8];
    ResourceLink<IndexRange> mRanges; ResourceLink<StridedBuffer> mBuffer; be<u32> mBaseVertex;
    void draw(u32 first, u32 count, u32 instances);
};
WWHD_SIZE(FileBufferResource, 24); WWHD_OFFSET(VertexResource, mBuffers, 0x18); WWHD_SIZE(VertexResource, 32);
WWHD_SIZE(MeshBufferRecord, 28);
WWHD_OFFSET(IndexedMesh, mRanges, 0x10); WWHD_SIZE(IndexedMesh, 28);

u32 readKeyBit(const be<u8>* bytes, u32 byteCount, u32 bit);

/* ---- model, texture-animation and archive file resources ---- */
struct NameLookupResource { u8 mHeader[0x10]; ResourceLink<ResourceDictionary> mNames; };
struct AnimationCurve;
struct TextureAnimationTrack {
    be<u32> mFlags; NameLink mName; u8 mHeader[2]; be<u8> mCurveCount; u8 mPadding[5];
    ResourceLink<AnimationCurve> mCurves; be<u32> mReserved;
};
struct TextureAnimationResource {
    u8 mHeader[0xC]; be<u32> mFlags; u8 mCountsHeader[4]; be<u16> mEntryCount; u8 mLinksHeader[0xA];
    ResourceLink<TextureAnimationTrack> mTracks; be<s32> mBoundModelRelative; ResourceLink<be<u16>> mBindings;
    be<u32> mReserved; ResourceLink<UniformBinding> mTextures;
    u32 bindModel(NameLookupResource* model); void* restoreCurves(); void* clearBindings();
    u32 resolveTextures(ResourceArchive* archive);
};
WWHD_SIZE(TextureAnimationTrack, 24); WWHD_OFFSET(TextureAnimationResource, mEntryCount, 0x14);
WWHD_OFFSET(TextureAnimationResource, mTracks, 0x20); WWHD_OFFSET(TextureAnimationResource, mTextures, 0x30);
struct ResourceArchive {
    u8 mArchiveHeader[0x20]; ResourceLink<ResourceDictionary> mModels; ResourceLink<ResourceDictionary> mTextures;
    u8 mCollections[0x10]; ResourceLink<ResourceDictionary> mTextureAnimations; u8 mReserved[0x14];
    be<u16> mModelCount; be<u16> mTextureCount; u8 mOtherCounts[8]; be<u16> mTextureAnimationCount;
    void initialize(); u32 resolveTextures(ResourceArchive* textures, u32 flags);
};
WWHD_OFFSET(ResourceArchive, mModels, 0x20); WWHD_OFFSET(ResourceArchive, mTextures, 0x24);
WWHD_OFFSET(ResourceArchive, mTextureAnimations, 0x38);
WWHD_OFFSET(ResourceArchive, mModelCount, 0x50); WWHD_OFFSET(ResourceArchive, mTextureAnimationCount, 0x5C);

/* ---- texture and bone transforms ---- */
using RotationSample = PoseTrigEntry;
WWHD_OFFSET(RotationSample, mSin, 0); WWHD_OFFSET(RotationSample, mCos, 4);
WWHD_OFFSET(RotationSample, mSinSlope, 8); WWHD_OFFSET(RotationSample, mCosSlope, 12);
struct AngleScratch { be<u32> mExponent; be<u32> mFraction; };
struct PlanarTransform { be<f32> mScaleX; be<f32> mScaleY; be<f32> mAngle; be<u32> mTranslationXBits; be<u32> mTranslationYBits; };
struct EulerTransform { be<f32> mScale[3]; be<f32> mAngles[3]; be<u32> mTranslationBits[3]; };
struct TextureTransform {
    be<u32> mMode; be<f32> mScaleX; be<f32> mScaleY; be<f32> mAngle; be<f32> mTranslationX; be<f32> mTranslationY;
    gptr<MatrixUtility> mAdditionalMatrix;
};
struct AffineMatrix {
    MatrixScalar mValues[6];
    u32 fromPlanarTransform(const PlanarTransform* input);
    u32 fromCenteredTexture(const TextureTransform* input); u32 fromTranslatedTexture(const TextureTransform* input);
    u32 fromInverseTexture(const TextureTransform* input);
};
WWHD_SIZE(RotationSample, 16); WWHD_SIZE(AngleScratch, 8); WWHD_SIZE(PlanarTransform, 20); WWHD_SIZE(AffineMatrix, 24);
WWHD_OFFSET(TextureTransform, mAdditionalMatrix, 0x18);
u32 buildEulerMatrix(MatrixUtility* output, const EulerTransform* input);
u32 buildTextureAffine(AffineMatrix* output, const TextureTransform* input);
u32 buildTextureMatrix(MatrixUtility* output, const TextureTransform* input);
u32 buildCenteredTextureMatrix(MatrixUtility* output, const TextureTransform* input);
u32 buildTranslatedTextureMatrix(MatrixUtility* output, const TextureTransform* input);
u32 buildInverseTextureMatrix(MatrixUtility* output, const TextureTransform* input);
/* Out-of-line forwarders of this unit into the matrix builders. */
void fromTransformedPlanarColumn(MatrixUtility* output, const MatrixUtility* left, const MatrixUtility* right);
void faceTransformedPlanarColumn(MatrixUtility* output, const MatrixUtility* left, const MatrixUtility* right);
void fromAlternatePlanarColumn(MatrixUtility* output, void*, void*, const MatrixUtility* input);
void faceAlternatePlanarColumn(MatrixUtility* output, void*, void*, const MatrixUtility* input);
void fromTransformedUp(MatrixUtility* output, const MatrixUtility* left, const MatrixUtility* right);
void faceTransformedUp(MatrixUtility* output, const MatrixUtility* left, const MatrixUtility* right);

/* Per-bone scale accumulated by PaletteBuffer::updateAccumulatedWorld (PaletteBuffer::mScale). */
struct PoseScale { MatrixScalar mValues[3]; };
WWHD_SIZE(PoseScale, 12);

/* ---- allocation plans of the animation runtimes ---- */
struct AllocationSegment { be<u32> mSize; be<u32> mOffset; };
struct ChannelBuildCounts {
    be<s32> mObjectCount; be<s32> mChannelCount; be<u32> mBindingCount; be<u32> mCurveCount;
    be<u8> mAllowCache; be<u8> mUseCache;
};
struct TransformBuildCounts {
    be<s32> mObjectCount; be<s32> mBoneCount; be<u32> mCurveCount; be<u8> mAllowCache; be<u8> mUseCache;
};
struct ChannelAllocation {
    gptr<AllocationSegment> mSegments; be<u32> mSize; AllocationSegment mStorage[4];
    void calculate(const ChannelBuildCounts* counts);
};
struct TransformAllocation {
    gptr<AllocationSegment> mSegments; be<u32> mSize; AllocationSegment mStorage[3];
    void calculate(const TransformBuildCounts* counts);
};
struct ChannelBuildPlan { ChannelBuildCounts mCounts; u8 mPadding[2]; ChannelAllocation mAllocation; u32 allocationSize(); };
struct TransformBuildPlan { TransformBuildCounts mCounts; u8 mPadding[2]; TransformAllocation mAllocation; u32 allocationSize(); };
struct ShapeBuildPlan;
WWHD_SIZE(AllocationSegment, 8); WWHD_SIZE(ChannelAllocation, 40); WWHD_SIZE(TransformAllocation, 32);
WWHD_OFFSET(ChannelBuildPlan, mAllocation, 0x14); WWHD_OFFSET(TransformBuildPlan, mAllocation, 0x10);
struct ShapeAllocation { gptr<AllocationSegment> mSegments; be<u32> mSize; AllocationSegment mStorage; void calculate(const ShapeBuildPlan* plan); };
WWHD_SIZE(ShapeAllocation, 16);
struct ShapeBuildPlan { be<u32> mTargetCount; ShapeAllocation mAllocation; u32 allocationSize(); };
struct VisibilityAllocation {
    gptr<AllocationSegment> mSegments; be<u32> mSize; AllocationSegment mStorage[4];
    void calculate(const ChannelBuildCounts* counts);
};
struct VisibilityBuildPlan { ChannelBuildCounts mCounts; u8 mPadding[2]; VisibilityAllocation mAllocation; u32 allocationSize(); };
struct AllocationCounts { be<s32> mObjects; be<s32> mChannels; };
WWHD_SIZE(VisibilityAllocation, 40); WWHD_OFFSET(VisibilityBuildPlan, mAllocation, 0x14); WWHD_SIZE(AllocationCounts, 8);

/* GHS member-function descriptor: receiver adjustment, then a vtable slot (>= 0, with the vtable's
   offset in the low half of mEntry) or a direct entry (< 0). */
struct RuntimeMethod { be<s16> mThisAdjustment; be<s16> mVirtualSlot; be<u32> mEntry; };
struct RuntimeVtableEntry { be<u32> mAdjustment; be<u32> mFunction; };
WWHD_SIZE(RuntimeMethod, 8); WWHD_SIZE(RuntimeVtableEntry, 8);

/* ---- animation curves ---- */
struct SampleCursor { be<u32> mStartBits; be<u32> mEndBits; be<u32> mIndex; };
WWHD_SIZE(SampleCursor, 12);
struct FloatKeyTrailer { be<u16> mFlags; be<u16> mKeyCount; be<u32> mScaleBits; be<u32> mOffsetBits; be<u32> mKeyAddress; };
struct IntegerKeyTrailer { be<u16> mFlags; be<u16> mKeyCount; be<u32> mKeyAddress; };
WWHD_SIZE(FloatKeyTrailer, 16); WWHD_SIZE(IntegerKeyTrailer, 8);
struct AnimationCurve {
    be<u16> mFlags; be<u16> mKeyCount; be<u32> mTargetOffset;
    be<f32> mStartFrame; be<f32> mEndFrame; MatrixScalar mScale;
    be<u32> mOffsetBits; be<f32> mRepeatOffset; be<s32> mFramesRelative; ResourceLink<be<u32>> mKeys;
    void updateCursor(SampleCursor* cursor, f32 frame); u32 sampleBooleanCached(SampleCursor* cursor, f32 frame);
    f32 evaluateFloat(SampleCursor* cursor, f32 frame); u32 evaluateInteger(SampleCursor* cursor, f32 frame);
    void restoreFloatKeys(); void restoreIntegerKeys();
    f32 wrapFrame(be<u32>* offset, f32 frame); u32 getFloatStorageSize(); u32 getIntegerStorageSize();
    u32 sampleBoolean(f32 frame) const;
};
WWHD_OFFSET(AnimationCurve, mStartFrame, 8); WWHD_OFFSET(AnimationCurve, mFramesRelative, 0x1C);
WWHD_OFFSET(AnimationCurve, mKeys, 0x20); WWHD_SIZE(AnimationCurve, 0x24);
struct FloatAnimationTrack {
    u8 mTrackHeader[10]; be<u8> mCurveCount; u8 mPadding[5]; ResourceLink<AnimationCurve> mCurves;
    void evaluateCached(u8* output, SampleCursor* cursors, f32 frame); void evaluateUncached(u8* output, f32 frame);
};
WWHD_OFFSET(FloatAnimationTrack, mCurves, 16);

/* ---- animation tracks and resources ---- */
struct TransformResult { be<u32> mFlags; be<u32> mScale[3]; be<u32> mTranslation[3]; be<u32> mReserved; be<u32> mRotation[4]; };
struct TransformTrack {
    be<u32> mFlags; ResourceLink<be<u8>> mName; u8 mTrackHeader[2]; be<u8> mCurveCount; u8 mTrackPadding[9];
    ResourceLink<be<u32>> mDefaults;
    void copyDefaults(TransformResult* output, const PoseBoneDescriptor* rest);
};
WWHD_SIZE(TransformResult, 48); WWHD_SIZE(TransformTrack, 24); WWHD_OFFSET(TransformTrack, mCurveCount, 0xA);
/* Per-target record of a visibility track: the curve driving it, and its name. */
struct VisibilityBinding { be<s8> mCurveIndex; u8 mPadding[3]; NameLink mName; };
struct VisibilityTrack {
    be<u16> mCount; be<u16> mFlags; be<u32> mFirstCurve; be<u32> mResultOffset;
    NameLink mName; ResourceLink<VisibilityBinding> mBindings; ResourceLink<AnimationCurve> mCurves; ResourceLink<be<u16>> mIndices;
    void copyDefaults(be<u16>* output) const;
    void evaluateCached(be<u16>* output, const be<s8>* bindings, SampleCursor* cursors, f32 frame);
    void evaluateUncached(be<u16>* output, const be<s8>* bindings, f32 frame);
};
WWHD_SIZE(VisibilityBinding, 8); WWHD_SIZE(VisibilityTrack, 28); WWHD_OFFSET(VisibilityTrack, mIndices, 24);
struct ChannelBindingInfo {
    be<u16> mFirstCurve; be<u16> mFloatCount; be<u16> mIntegerCount; be<u16> mFirstDefault; be<u16> mDefaultCount;
    be<u16> mReserved; NameLink mName;
};
struct ChannelDefault { be<u32> mOffset; be<u32> mValue; };
struct ChannelTrack {
    be<u16> mTargetCount; be<u16> mFlags; be<u32> mReserved; be<u32> mResultOffset; be<u32> mFirstBinding;
    NameLink mName; ResourceLink<ChannelBindingInfo> mBindings; ResourceLink<AnimationCurve> mCurves;
    ResourceLink<ChannelDefault> mDefaults;
};
WWHD_SIZE(ChannelDefault, 8); WWHD_SIZE(ChannelTrack, 32); WWHD_SIZE(ChannelBindingInfo, 16);
struct AnimationResourceHeader {
    u8 mHeader[0xC]; be<u32> mFlags; be<u32> mFrameCount; be<u16> mTrackCount;
    be<u16> mVisibilityTrackCount; be<s32> mTransformCurveCount; be<s32> mCurveCount;
    ResourceLink<TransformTrack> mTransformTracks; be<u32> mReserved;
    ResourceLink<be<u16>> mBindings; ResourceLink<void> mTracks; ResourceLink<UniformBinding> mTextures;
};
WWHD_OFFSET(AnimationResourceHeader, mTracks, 0x2C); WWHD_OFFSET(AnimationResourceHeader, mTextures, 0x30);

/* The animation runtime: the clock (frame controller, cursor cache, results, dispatch vtable), then the
   bound resource, its tracks, the source-to-target binding table and two dispatch descriptors. */
struct AnimationVtable { u8 mHeader[0x1C]; be<u32> mEvaluate; };
struct AnimationRuntime : AnimationClock {
    BindingTable mBindings; gptr<AnimationResourceHeader> mResource; gptr<void> mTracks;
    be<u32> mCapacity; be<u32> mState; gptr<be<u16>> mTargets;
    RuntimeMethod mEvaluate; RuntimeMethod mApply;
    void resetTransformResults(const PoseResource* skeleton); u32 bindTransform(const PoseResource* skeleton);
    void bindIndexedTransform(const PoseResource* skeleton);
    u32 bindChannelTargets(const ChannelTrack* track, const UniformDescriptor* material);
    u32 bindVisibilityTargets(const VisibilityTrack* track, const UniformDescriptor* material);
    u32 bindChannels(const ModelDescriptor* model); u32 bindVisibility(const ModelDescriptor* model);
    void applyChannelTrack(UniformBuffer* material, u32 index); void applyChannels(ModelRuntime* model);
    void evaluateChannels(); void evaluateVisibility();
    void applyVisibilityTrack(UniformBuffer* material, u32 index); void applyVisibility(ModelRuntime* model);
    void setChannelResource(AnimationResourceHeader* resource); void setTransformResource(AnimationResourceHeader* resource);
    void setVisibilityResource(AnimationResourceHeader* resource);
    u32 initializeChannels(ChannelBuildPlan* plan, void* storage, u32 capacity);
    u32 initializeTransforms(TransformBuildPlan* plan, void* storage, u32 capacity);
    u32 initializeVisibility(VisibilityBuildPlan* plan, void* storage, u32 capacity);
    void resetVisibilityResults();
    void setSubtreeFlags(const PoseResource* skeleton, s32 bone, u32 flags);
    void evaluate(); void apply(void* target);
};
WWHD_OFFSET(AnimationRuntime, mBindings, 0x38); WWHD_OFFSET(AnimationRuntime, mResource, 0x44);
WWHD_OFFSET(AnimationRuntime, mTargets, 0x54); WWHD_OFFSET(AnimationRuntime, mEvaluate, 0x58);
WWHD_OFFSET(AnimationRuntime, mApply, 0x60); WWHD_SIZE(AnimationRuntime, 0x68);
void applyBoundAnimation(AnimationRuntime* animation, const ModelRuntime* owner);

struct BlendRuntime {
    gptr<void> mData; be<u16> mTargetCount; be<u16> mCapacity; be<u32> mState;
    gptr<void> mWork; gptr<void> mStorage;
    u32 initialize(ShapeBuildPlan* plan, void* storage, u32 capacity); void invalidate();
    void blend(AnimationRuntime* animation, f32 weight); void finalize(void* target);
};
WWHD_OFFSET(BlendRuntime, mState, 8); WWHD_SIZE(BlendRuntime, 20);

u32 finishDrawCommands(u32 value); void onPaletteRebound(); void onUniformRebound();
} }

/* Cafe SDK imports and out-of-line helpers this unit calls. */
void gx2SetVertexSampler(const agl::gx2util::SamplerState*, u32);
void gx2SetGeometrySampler(const agl::gx2util::SamplerState*, u32);
void gx2SetPixelSampler(const agl::gx2util::SamplerState*, u32);
void gx2InitializeTexture(agl::gx2util::TextureResource*);
void gx2CalculateSurface(agl::gx2util::TextureResource*);
void gx2SetVertexTexture(const agl::gx2util::TextureResource*, u32);
void gx2SetGeometryTexture(const agl::gx2util::TextureResource*, u32);
void gx2SetPixelTexture(const agl::gx2util::TextureResource*, u32);
void gx2FlushRange(void*, u32);
void gx2SetFetchShader(const agl::gx2util::FetchResource*);
void gx2SetAttributeBuffer(u32, u32, u32, void*);
void gx2SetVertexUniform(u32, u32, void*);
void gx2SetGeometryUniform(u32, u32, void*);
void gx2SetPixelUniform(u32, u32, void*);
f32 gx2FloorFrame(f32);
void gx2SubmitPolygon(const agl::gx2util::PolygonState*);
void gx2SubmitDepth(const agl::gx2util::DepthState*);
void gx2SubmitAlpha(const agl::gx2util::AlphaState*);
void gx2SubmitColor(const agl::gx2util::ColorState*);
void gx2SubmitBlend(const agl::gx2util::BlendState*);
void gx2SubmitBlendColor(const agl::gx2util::BlendColor*);
void gx2SubmitOffset(const agl::gx2util::PolygonOffset*);
void gx2ClearWords(void*, u32);
void* gx2Allocate(u32);
agl::gx2util::SystemTimingInfo* gx2GetSystemTimingInfo();
void gx2DrawIndices(u32, u32, u32, void*, u32, u32);
Pair32 gx2EncodeAngle(f32);
void* gx2CopyMemory(void*, const void*, u32);
void clearGraphicsRange(void*, u32);
void gx2EvaluateChannelCached(agl::gx2util::ChannelTrack*, be<u32>*, be<u16>*, agl::gx2util::SampleCursor*, f32);
void gx2EvaluateChannelUncached(agl::gx2util::ChannelTrack*, be<u32>*, be<u16>*, void*, f32);
