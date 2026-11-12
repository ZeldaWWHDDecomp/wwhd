// WWHD anti-alias parameter helpers, reconstructed from the unit's own code.

// Behavior is from WWHD; names are ours or grounded in WWHD strings and use.
#include "agl/FilterAaParameters.h"
#include "bindings.h"
#include "sead/Reports.h"
#include "sead/ElementAt.h"
#include <atomic>
#include "sead/HeapRegistry.h"
#include "sead/GraphicsSurface.h"
#include "sead/GraphicsRuntime.h"
#include "port_native_draw.h"
using namespace agl::filter_aa;
using namespace agl::filter_aa_text;
void initializeEffectParameter(ParameterBase_l* storage);
void releaseEffectControl(ControlPrefix_l* control);
void destroyEffectControlBase(ControlPrefix_l* control,u32 flags);
void destroyEffectSurface(SurfacePrefix_l* surface,u32 flags);
sead::RegistryHeap* findRegistryHeap(void* manager,void* memory);
void destroySurfaceState(void* storage,u32 flags);
void destroySurfaceReleaseContext(void* storage,u32 flags);
void destroyRuntimeBase(EffectRuntimePrefix_l* storage,u32 flags);
using PairDestroyCallback=void (*)(SurfaceOwnerPrefix_l*,u32);
void destroySurfaceArray(void* storage,u32 count,u32 stride,PairDestroyCallback destroy,u32 mode,u32 extra);
using SurfaceConstructCallback=void (*)(void*);
void constructFilterSurfaceArray(void* storage,u32 count,u32 stride,SurfaceConstructCallback construct);
void constructFilterTextureData(void* storage);
void* allocateFilterEntryStorage(u32 bytes,void* heap,u32 alignment);
void initializeFilterAttribute(void* storage);
void initializeFilterIndices(void* storage);
s32 findShaderVariant(ShaderVariantList_l* variants,u32 count,const be<u32>* names,const be<u32>* values,s32 selection);
void noOpFilterAaControlHook(ControlPrefix_l* storage,s32 count,const Text* name,void* heap);
static ShaderArchiveRegistry_l* sShaderArchiveRegistry;
struct FilterQualityTable_l { gptr<const char> mValues[7]; };
static const FilterQualityTable_l sFilterQualityValues;
static const FilterQualityTable_l sFilterDepthQualityValues;
static u8 sFilterFetchName;
static u8 sFilterAlphaName;
static u8 sFilterDepthName;
static u8 sFilterDebugName;
static u8 sFilterZeroToken;
static u8 sFilterFiveToken;
static u8 sFilterTextMethods;
static u8 sFilterName;
static ShaderArchive_l* filterArchive(u32 index) {
    ShaderArchiveRegistry_l* registry=sShaderArchiveRegistry;
    if (index>=u32(registry->mCount)) return nullptr;
    return registry->mArchives.get()[index];
}
static ShaderVariant_l* selectedVariant(ShaderArchive_l* archive,s32 index) {
    ShaderVariantList_l* variants=&archive->mResource->mVariants;
    if (index<=0) return variants->mDefault;
    ShaderVariant_l* selected=variants->mVariants;
    if (u32(index-1)<u32(variants->mCount)) selected+=u32(index-1);
    return selected;
}
struct UniformBinding_l {
    u8 mPrefix[8];
    be<s16> mVertexRegister, mPixelRegister;
    u8 mTail[4];
};
struct TextureBinding_l { u8 mStorage[0x10]; };
WWHD_SIZE(TextureBinding_l,0x10);
struct ShaderProgramData_l { u8 mPrefix[8]; ShaderVariantList_l mVariants; u8 mBeforeMode[0xC]; be<u32> mMode; };
struct ColorProgramState_l {
    be<u8> mFlags;
    u8 mBeforeDisplayList[3];
    gptr<void> mDisplayList;
    u8 mBeforeDisplayListBytes[4];
    be<u32> mDisplayListBytes;
    u8 mBeforeUniformCount[0x18];
    be<u32> mUniformCount;
    gptr<UniformBinding_l> mUniforms;
    u8 mBeforeTextureCount[8];
    be<u32> mTextureCount;
    gptr<TextureBinding_l> mTextures;
    u8 mBeforePrograms[0x3C];
    gptr<ShaderProgramData_l> mData;
};
using ColorRenderState_l=FilterRenderState_l;
void updateColorProgram(ColorProgramState_l* program,u32 mode);
void applyShaderMode(u32 mode);
void prepareColorProgram(ColorProgramState_l* program);
void callColorDisplayList(const void* list,u32 bytes);
void setColorVertexUniform(s32 location,u32 words,const void* color);
void setColorPixelUniform(s32 location,u32 words,const void* color);
void drawRuntimeProgram(EffectRuntimePrefix_l* effect,ColorProgramState_l* program,void* context);
WWHD_OFFSET(UniformBinding_l,mVertexRegister,8);
WWHD_OFFSET(UniformBinding_l,mPixelRegister,10);
WWHD_SIZE(UniformBinding_l,0x10);
WWHD_OFFSET(ColorProgramState_l,mData,0x7C);
WWHD_OFFSET(ShaderProgramData_l,mMode,0x28);
WWHD_OFFSET(ColorRenderState_l,mBlendRed,0x28);
WWHD_SIZE(ColorRenderState_l,0x74);
struct SliceValues_l { be<f32> mFirst, mSecond; };
struct ColorWords_l { be<u32> mWords[4]; };
static ColorWords_l sSliceColorFirst;
static ColorWords_l sSliceColorSecond;
u32 submitSliceColor(void* receiver,u32 count,const SliceValues_l* values,const ColorWords_l* colors,u32 initial,f32 scaleX,f32 scaleY);
void constructFilterBase(FilterAaObject_l* storage,const Text* name,u32 flags);
void constructFilterParameterContainer(FilterParameterContainer_l* storage);
void bindFilterParameter(ParameterBase_l* parameter,const Text* name,const Text* description,const Text* range,FilterParameterContainer_l* container);
void constructFilterControl(ControlPrefix_l* storage);
void attachFilterParameters(FilterAaObject_l* storage,FilterParameterContainer_l* container,const Text* name);
void setFilterIcon(void* interface,const Text* icon);
static ColorWords_l sFilterLumaDefault;
static u8 sFilterBaseName;
static u8 sFilterContainerName;
static u8 sFilterIcon;
static u8 sFilterBaseTextMethods;
static u8 sFilterEmptyType;
static u8 sFilterEmptyEnable;
static u8 sFilterEmptyColor;
static u8 sFilterIntegerMethods;
static u8 sFilterByteMethods;
static u8 sFilterScalarMethods;
static u8 sFilterColorMethods;
static u8 sFilterTypeName;
static u8 sFilterTypeDescription;
static u8 sFilterEnableName;
static u8 sFilterEnableDescription;
static u8 sFilterAlphaOutputName;
static u8 sFilterAlphaOutputDescription;
static u8 sFilterAlphaOutputRange;
static u8 sFilterEdgeQualityName;
static u8 sFilterEdgeQualityDescription;
static u8 sFilterFetchQualityName;
static u8 sFilterFetchQualityDescription;
static u8 sFilterEdgeThresholdScaleName;
static u8 sFilterEdgeThresholdScaleDescription;
static u8 sFilterEdgeThresholdScaleRange;
static u8 sFilterLumaName;
static u8 sFilterLumaDescription;
static u8 sFilterSubpixelName;
static u8 sFilterSubpixelDescription;
static u8 sFilterSubpixelRange;
static u8 sFilterMaximumSpanName;
static u8 sFilterMaximumSpanDescription;
static u8 sFilterMaximumSpanRange;
static u8 sFilterSpanMultiplyName;
static u8 sFilterSpanMultiplyDescription;
static u8 sFilterSpanMultiplyRange;
static u8 sFilterMinimumSpanName;
static u8 sFilterMinimumSpanDescription;
static u8 sFilterMinimumSpanRange;
static u8 sFilterReprojectionName;
static u8 sFilterReprojectionDescription;
static u8 sFilterMoveLimitName;
static u8 sFilterMoveLimitDescription;
static u8 sFilterMoveLimitRange;
static f32 sFilterDefaultAlpha;
static f32 sFilterDefaultSubpixel;
static f32 sFilterDefaultMaximumSpan;
static f32 sFilterDefaultMinimumSpan;
static f32 sFilterDefaultMoveLimit;
static f32 sColorZero;
static f32 sColorOne;
template<class T> static void configureFilterParameter(T* parameter,const void* methods,const void* nameToken,const void* descriptionToken,const void* rangeToken,FilterParameterContainer_l* container,Text* name,Text* description,Text* range) {
    name->mMethods=&sFilterBaseTextMethods;
    name->mText=reinterpret_cast<char*>(const_cast<void*>(nameToken));
    description->mMethods=&sFilterBaseTextMethods;
    description->mText=reinterpret_cast<char*>(const_cast<void*>(descriptionToken));
    initializeEffectParameter(parameter);
    parameter->setMethods(methods);
    if constexpr (std::is_same_v<T,ColorParameter>) {
        f32 zero=sColorZero;
        parameter->mValue.mRed=zero;
        parameter->mValue.mBlue=zero;
        parameter->mValue.mGreen=zero;
        parameter->mValue.mAlpha=sColorOne;
    }
    range->mMethods=&sFilterBaseTextMethods;
    range->mText=reinterpret_cast<char*>(const_cast<void*>(rangeToken));
    bindFilterParameter(parameter,name,description,range,container);
}
struct SurfaceImagePrefix_l {
    be<u32> mHeader;
    sead::GraphicsSurface mGraphics;
    be<u8> mMinimumWidth, mMinimumHeight;
    u8 mBeforeMipDirty[0x16];
    be<u8> mMipDirty, mSecondaryFlag;
    u8 mBeforeMipIndex[2];
    be<u32> mMipIndex;
    u8 mBeforeMirror[0x3C];
    gptr<void> mMirrorImage;
    u8 mBeforeMirrorMip[4];
    gptr<void> mMirrorMip;
    u8 mTail[0x68];
};
struct TextureImage_l : SurfaceImagePrefix_l {
    u8 mBeforeLod[0x30];
    be<f32> mLodMinimum, mLodMaximum, mLodBias;
    u8 mBeforeFlags[12];
    be<u8> mFlags;
    u8 mTail[7];
};
// The caller buffer contains a RenderSurface prefix and a target pointer.
struct BlurViewport_l : sead::RenderSurface { u8 mBeforeTarget[4]; gptr<void> mTarget; u8 mTail[0x1C]; be<u32> mLastWord; };
using ViewportRecord_l=sead::Viewport;
// ABI adapter for the canonical namespace free function; autoglue misclassifies namespace-qualified free functions.
void activateFilterFrame(sead::DisplayFrame* frame);
static ViewportRecord_l* constructBlurViewport(ViewportRecord_l* record,const sead::RenderSurface* surface) {
    return sead::Viewport::createSurface(record,reinterpret_cast<const sead::RenderVector2*>(surface));
}
static void applyBlurViewport(ViewportRecord_l* record,const sead::RenderSurface* surface) { record->apply(surface); }
void flushBlurSurface(SurfaceImagePrefix_l* surface);
void finishBlurSurface(SurfaceImagePrefix_l* surface);
void copyBlurDescriptor(SurfaceImagePrefix_l* destination,const SurfaceImagePrefix_l* source);
SurfaceImagePrefix_l* allocateBlurSurface(void* context,const Text* name,u32 format,u32 width,u32 height,u32 layers,u32 firstMode,u32 secondMode,u32 lastMode);
void bindBlurTarget(SurfaceImagePrefix_l* target,SurfaceImagePrefix_l* storage);
void copyBlurLevel(SurfaceImagePrefix_l* target,TextureImage_l* texture,u32 level,u32 flags);
static f32 sBlurZero;
static f32 sBlurOne;
static f32 sBlurUnsignedLimit;
static u8 sBlurName;
static u8 sBlurTextMethods;
WWHD_SIZE(SurfaceImagePrefix_l,0x148);
WWHD_OFFSET(SurfaceImagePrefix_l,mGraphics,4);
WWHD_OFFSET(SurfaceImagePrefix_l,mMinimumWidth,0x78);
WWHD_OFFSET(SurfaceImagePrefix_l,mMipIndex,0x94);
WWHD_OFFSET(SurfaceImagePrefix_l,mMirrorImage,0xD4);
WWHD_SIZE(TextureImage_l,0x198);
WWHD_OFFSET(TextureImage_l,mFlags,0x190);
WWHD_SIZE(BlurViewport_l,0x40);
static bool matchingBlurDescriptor(const SurfaceImagePrefix_l* first,const SurfaceImagePrefix_l* second) {
    return u32(first->mGraphics.mDimension)==u32(second->mGraphics.mDimension) && u32(first->mGraphics.mWidth)==u32(second->mGraphics.mWidth) &&
        u32(first->mGraphics.mHeight)==u32(second->mGraphics.mHeight) && u32(first->mGraphics.mDepth)==u32(second->mGraphics.mDepth) &&
        u32(first->mGraphics.mMipCount)==u32(second->mGraphics.mMipCount) && u32(first->mGraphics.mFormat)==u32(second->mGraphics.mFormat) &&
        u32(first->mGraphics.mSwizzle)==u32(second->mGraphics.mSwizzle) && u32(first->mGraphics.mTileMode)==u32(second->mGraphics.mTileMode) &&
        u32(first->mGraphics.mSamples)==u32(second->mGraphics.mSamples);
}
static void copyBlurStorage(SurfaceImagePrefix_l* destination,const SurfaceImagePrefix_l* source) {
    void* image=source->mGraphics.mImage;
    void* mip=source->mGraphics.mMipImage;
    destination->mMirrorImage=image;
    destination->mGraphics.mImage=image;
    destination->mGraphics.mMipImage=mip;
    destination->mMirrorMip=mip;
}
static void setBlurLod(TextureImage_l* texture,f32 level,f32 zero) {
    texture->mFlags=u8(texture->mFlags) | 8;
    texture->mLodMinimum=level;
    texture->mLodMaximum=level;
    texture->mLodBias=zero;
}
static u32 activateEffectProgram(ColorProgramState_l* program,u32 previous) {
    if ((u8(program->mFlags) & 2)!=0) {
        program->mFlags=u8(program->mFlags) & ~2u;
        updateColorProgram(program,0);
    }
    u32 selected=program->mData->mMode;
    if (previous!=selected) applyShaderMode(selected);
    u32 bytes=program->mDisplayListBytes;
    if (bytes!=0) callColorDisplayList(program->mDisplayList,bytes);
    else prepareColorProgram(program);
    return selected;
}
static void uploadBlurOffsets(ColorProgramState_l* program,const Color4* offsets) {
    u32 uniformCount=program->mUniformCount;
    UniformBinding_l* entry=program->mUniforms;
    if (uniformCount>6) entry+=6;
    s32 vertex=entry->mVertexRegister;
    if (vertex!=-1) setColorVertexUniform(vertex,4,offsets);
    s32 pixel=entry->mPixelRegister;
    if (pixel!=-1) setColorPixelUniform(pixel,4,offsets);
}
static u32 blurDimension(f32 value) {
    f32 limit=sBlurUnsignedLimit;
    if (value<limit) return u32(gabi::ftoi(gabi::fpr_from_f32(value)));
    return u32(gabi::ftoi(gabi::fpr_from_f32(gabi::fsubs_ppc(value,limit))))+0x80000000u;
}
struct BloomLevelParameters_l {
    u8 mPrefix[0x98];
    Color4 mTint;
    u8 mBeforeMode[0x19C];
    be<u32> mMode;
    u8 mBeforeIterations[12];
    be<s32> mIterations;
};
static f32 sBloomHalf;
static u8 sBloomFirstTargetName;
static u8 sBloomSecondTargetName;
WWHD_OFFSET(BloomLevelParameters_l,mMode,0x244);
WWHD_OFFSET(BloomLevelParameters_l,mIterations,0x254);
WWHD_OFFSET(BlurViewport_l,mTarget,0x1C);
static void setBloomViewport(BlurViewport_l* viewport,Color4* bounds,f32 width,f32 height,f32 zero) {
    viewport->mHeight=height;
    viewport->mWidth=width;
    bounds->mBlue=zero<width?width:zero;
    bounds->mRed=zero<width?zero:width;
    bounds->mAlpha=zero<height?height:zero;
    bounds->mGreen=zero<height?zero:height;
    viewport->mLeft=bounds->mRed;
    viewport->mBottom=bounds->mGreen;
    viewport->mRight=bounds->mBlue;
    viewport->mTop=bounds->mAlpha;
}
static void uploadBloomZeros(ColorProgramState_l* program,const Color4* values) {
    u32 mode=program->mUniformCount;
    UniformBinding_l* uniforms=static_cast<UniformBinding_l*>(program->mUniforms.get());
    if (mode>5) uniforms+=5;
    s32 vertex=uniforms->mVertexRegister;
    if (vertex!=-1) setColorVertexUniform(vertex,4,values);
    s32 pixel=uniforms->mPixelRegister;
    if (pixel!=-1) setColorPixelUniform(pixel,4,values);
}
using FilterMatrix44_l=sead::RenderMatrix44;
using FilterVector3_l=sead::RenderVector3;
struct FilterAttributeOwnerPrefix_l { u8 mPrefix[0x6BC]; u8 mBindingArgument[1]; };
struct FilterIndexDraw_l { be<u32> mFormat, mPrimitive; gptr<const void> mIndices; be<u32> mCount; };
struct FilterGeometryOwnerPrefix_l { u8 mPrefix[0x31C]; FilterIndexDraw_l mDraw; };
u32 copyFilterOutput(void* output,void* view,const void* firstSettings,const void* secondSettings,u32 previousMode);
SurfaceImagePrefix_l* allocateFilterTarget(void* pool,const Text* name,u32 format,u32 width,u32 height,u32 layers,u32 firstMode,u32 secondMode,u32 lastMode,u32 extra);
void releaseFilterTarget(void* pool,SurfaceImagePrefix_l* target);
void resetFilterViewport(BlurViewport_l* viewport);
void bindFilterViewport(void* frameContext,u32 mask);
void bindFilterTexture(SurfaceImagePrefix_l* texture,const TextureBinding_l* binding,s32 location,u32 flags);
void bindFilterAttributes(const void* binding);
void drawFilterIndices(u32 primitive,u32 count,u32 format,const void* indices,u32 first,u32 instances);
void constructFilterMatrix(FilterMatrix44_l* matrix,const void* attribute,const void* indices);
void initializeFilterMatrix(FilterMatrix44_l* matrix,f32 x,f32 y,f32 z);
void applyFilterMatrixVector(FilterMatrix44_l* matrix,const FilterVector3_l* vector);
void multiplyFilterMatrices(const FilterMatrix44_l* first,const FilterMatrix44_l* second,FilterMatrix44_l* output);
// Borrowed pointer words from AGL#10; no local ownership or initializer.
static void* sFilterTargetPool;
static FilterAttributeOwnerPrefix_l* sFilterAttributeOwner;
// Borrowed from AGL#11; only the assigned draw-packet read prefix is modeled.
static FilterGeometryOwnerPrefix_l* sFilterGeometryOwner;
static const u8 sFilterCopyFirstSettings;
static const u8 sFilterCopySecondSettings;
static u8 sFilterRenderLumaName;
static f32 sFilterRenderZero;
static f32 sFilterRenderUnsignedLimit;
static f32 sFilterRenderHalf;
static f32 sFilterThresholdSmall;
static f32 sFilterThresholdLarge;
static f32 sFilterThresholdMediumSmall;
static f32 sFilterThresholdMediumLarge;
static f32 sFilterThresholdFine;
static f32 sFilterThresholdMultiplier;
static f32 sFilterReprojectionScale;
static f32 sFilterNegativeHalf;
static f32 sFilterAlternateMode;
WWHD_SIZE(FilterMatrix44_l,0x40);
WWHD_OFFSET(ColorProgramState_l,mUniformCount,0x28);
WWHD_OFFSET(ColorProgramState_l,mTextureCount,0x38);
WWHD_OFFSET(ColorProgramState_l,mTextures,0x3C);
static void uploadFilterUniform(ColorProgramState_l* program,u32 index,const void* values,u32 words=4) {
    UniformBinding_l* binding=program->mUniforms;
    if (index!=0 && u32(program->mUniformCount)>index) binding+=index;
    s32 vertex=binding->mVertexRegister;
    if (vertex!=-1) setColorVertexUniform(vertex,words,values);
    s32 pixel=binding->mPixelRegister;
    if (pixel!=-1) setColorPixelUniform(pixel,words,values);
}
static void uploadFilterCommon(ColorProgramState_l* program,const Color4* steps,const Color4* offsets,const Color4* luma,const Color4* thresholds,const Color4* spans) {
    uploadFilterUniform(program,1,steps);
    uploadFilterUniform(program,2,offsets);
    uploadFilterUniform(program,3,luma);
    uploadFilterUniform(program,4,thresholds);
    uploadFilterUniform(program,5,spans);
}
static const TextureBinding_l* filterTextureBinding(ColorProgramState_l* program,u32 index) {
    TextureBinding_l* binding=program->mTextures;
    if (index!=0 && u32(program->mTextureCount)>index) binding+=index;
    return binding;
}
static void drawFilterQuad() {
    bindFilterAttributes(sFilterAttributeOwner->mBindingArgument);
    FilterIndexDraw_l* draw=&sFilterGeometryOwner->mDraw;
    u32 count=draw->mCount;
    if (count!=0) {
        u32 primitive=draw->mPrimitive;
        const void* indices=draw->mIndices;
        u32 format=draw->mFormat;
#if PORT_NATIVE_DRAW
        // the anti-aliasing filter's draw as a packet of its own (port_native_draw.h)
        if (port_native_draw_single(PORT_NATIVE_POST,PORT_NATIVE_POST_FILTER_AA,gabi::ea(draw),0,0,primitive,count,format,
                                    gabi::ea(indices),0,1)) return;
#endif
        drawFilterIndices(primitive,count,format,indices,0,1);
    }
}
struct BloomRenderOwner_l : sead::RenderSurface {
    u8 mBeforeColor[4]; gptr<SurfaceImagePrefix_l> mColor;
    u8 mBeforeDepth[0x1C]; gptr<SurfaceImagePrefix_l> mDepth;
};
struct BloomRenderContext_l {
    gptr<BloomRenderOwner_l> mOwner; gptr<ViewportRecord_l> mView;
    gptr<SurfaceImagePrefix_l> mColor,mDepth;
    be<u8> mDepthKind; u8 mBeforeExtra[3]; gptr<SurfaceImagePrefix_l> mExtra;
};
struct BloomEffectControls_l {
    u8 mPrefix[0x228]; be<u32> mColorMode;
    u8 mBeforeSlices[0x90]; be<u16> mSliceFlags;
    u8 mBeforeEnabled[0x1FE]; be<u8> mEnabled;
    u8 mBeforeFlags[3]; be<u16> mFlags;
};
struct BloomCoreParameters_l {
    u8 mPrefix[0x3C]; Color4 mBlend;
    u8 mBeforeMix[12]; be<f32> mMix;
    u8 mBeforeColors[0x14C];
    struct ColorRecord_l { u8 mPrefix[12]; Color4 mColor; } mColors[3];
    u8 mBeforeBlendMode[0x1C]; be<u32> mBlendMode;
};
struct BloomViewportTargets_l { u8 mPrefix[0x1C]; gptr<void> mTargets[8]; be<u32> mLastWord; };
struct BloomColors_l { Color4 mColors[3]; };
struct BloomWeightValues_l { be<f32> mValues[3]; };
struct BloomPairCursor_l { gptr<SurfacePair> mBegin; u8 mUnknown[4]; gptr<SurfacePair> mEnd; };
struct BloomEntryTail_l { gptr<SurfaceImagePrefix_l> mExtraTarget; u8 mUnknown[4]; be<f32> mScaleX,mScaleY,mRenderScale; };
s32 prepareBloomContext(void* context);
u32 detectBloomInput(EffectRuntimePrefix_l* effect,u32 index,u32 hasColor,u32 depthKind,u32 previous);
void finishBloomContext(void* context);
void multiplyFilterColorChannels(sead::ColorChannels*,const sead::ColorChannels*,const sead::ColorChannels*);
static void multiplyBloomColor(Color4* output,const Color4* first,const Color4* second) {
    // AGL parameter colors stay their own type; the canonical API consumes RGBA scalar views.
    multiplyFilterColorChannels(reinterpret_cast<sead::ColorChannels*>(output),reinterpret_cast<const sead::ColorChannels*>(first),reinterpret_cast<const sead::ColorChannels*>(second));
}
static BloomRenderOwner_l* sBloomFallbackOwner;
static f32 sBloomMinimumDimension;
static f32 sBloomRenderScale;
static u8 sBloomDetectFirstName;
static u8 sBloomDetectSecondName;
static u32 sBloomMixInitialized;
static f32 sBloomMixHalf;
WWHD_OFFSET(BloomEffectControls_l,mSliceFlags,0x2BC);
WWHD_OFFSET(BloomEffectControls_l,mEnabled,0x4BC);
WWHD_OFFSET(BloomEffectControls_l,mFlags,0x4C0);
WWHD_OFFSET(BloomCoreParameters_l,mMix,0x58);
WWHD_OFFSET(BloomCoreParameters_l,mColors,0x1A8);
WWHD_OFFSET(BloomCoreParameters_l,mBlendMode,0x218);
WWHD_OFFSET(BloomRenderOwner_l,mDepth,0x3C);
WWHD_SIZE(BloomViewportTargets_l,0x40);
static void copyBloomInput(SurfaceImagePrefix_l* target,const SurfaceImagePrefix_l* source) {
    if (!matchingBlurDescriptor(target,source)) copyBlurDescriptor(target,source);
    else copyBlurStorage(target,source);
}
static ColorProgramState_l* bloomCopyProgram() {
    return reinterpret_cast<ColorProgramState_l*>(filterArchive(43)->mResource->mVariants.mVariants.get());
}
struct VariadicSaveArea_l { be<u32> mGeneral[8]; be<f64> mFloating[8]; };
struct VariadicArguments_l {
    be<u8> mGeneralIndex, mFloatingIndex;
    u8 mPadding[2];
    gptr<void> mOverflow;
    gptr<VariadicSaveArea_l> mRegisterSave;
};
// GHS register-bank ABI storage. The construct entry transports r5-r10 and
// f1-f8 in fixed adapter parameters, without claiming source-level vararg types.
WWHD_SIZE(VariadicSaveArea_l,96);
WWHD_OFFSET(VariadicSaveArea_l,mFloating,32);
WWHD_SIZE(VariadicArguments_l,12);
static_assert(sizeof(VariadicArguments_l)==sizeof(ReportArguments));
static_assert(offsetof(VariadicArguments_l,mGeneralIndex)==offsetof(ReportArguments,mIntegerIndex));
static_assert(offsetof(VariadicArguments_l,mFloatingIndex)==offsetof(ReportArguments,mFloatIndex));
static_assert(offsetof(VariadicArguments_l,mOverflow)==offsetof(ReportArguments,mOverflow));
static_assert(offsetof(VariadicArguments_l,mRegisterSave)==offsetof(ReportArguments,mSavedRegisters));
static_assert(sizeof(VariadicSaveArea_l)==sizeof(ReportSave)-offsetof(ReportSave,mIntegers));
static_assert(offsetof(VariadicSaveArea_l,mFloating)==offsetof(ReportSave,mFloatWords)-offsetof(ReportSave,mIntegers));
void formatTextArguments(FormattedText128* storage,const char* format,VariadicArguments_l* arguments);
static u8 sTextBufferMethods;
static u8 sTextBaseMethods;
static u8 sFormattedTextMethods;
u32 renderBloomEffect(EffectRuntimePrefix_l* effect,u32 surfaceIndex,void* context,u32 flags,u32 fallback);
void initializeRuntimeBase(EffectRuntimePrefix_l* storage,s32 surfaceCount,u32 sharedParameters,void* heap);
void initializeSurfaceState(void* storage);
void initializeSurfaceReleaseContext(void* storage);
using PairConstructCallback=SurfacePair* (*)(SurfacePair*);
void constructPairArray(void* storage,u32 count,u32 stride,PairConstructCallback construct);
void initializeRuntimeSurface(EffectRuntimePrefix_l* storage,SurfaceBatchEntry_l* surface,void* heap);
void initializeRuntimeValues(ParameterBlock_l* parameters,FilterParameterContainer_l* values,void* heap);
using SurfaceDestroyCallback=void (*)(SurfacePrefix_l*,u32);
void destroyFilterSurfaceArray(void* storage,u32 count,u32 stride,SurfaceDestroyCallback destroy,u32 mode,u32 extra);
void releaseFilterEntryStorage(void* memory);
static u8 sFilterPrimaryMethods;
static u8 sFilterSecondaryMethods;
static u8 sFilterControlMethods;
static u8 sFilterControlPrimaryMethods;
static u8 sFilterControlSecondaryMethods;
static u8 sRuntimeMethods;
void releaseSurfaceHandle(void* context,u32 handle);
void initializeSurface(void* storage);
void initializeSecondSurface(void* storage);
void initializeEffectRuntime(EffectRuntimePrefix_l* storage);
// Opaque AGL#2 dependency: no foreign implementation is included or executed.
void initializeEffectParameterBlock(void* storage);
static u8 sIntegerParameterMethods;
static u8 sByteParameterMethods;
static u8 sControlPrimaryMethods;
static u8 sControlSecondaryMethods;
static u8 sColorParameterMethods;
/* 0277912C */
void Text::destroy(Text* text,u32 flags) {
    gabi::AutoFrame frame{0x0277912Cu};
    if (text != nullptr && (flags & 1)) operator_delete(text);
}
/* 02779140 */
void TextBufferB::destroy(TextBufferB* text,u32 flags) {
    gabi::AutoFrame frame{0x02779140u};
    if (text != nullptr && (flags & 1)) operator_delete(text);
}
/* 02779154 */
void SurfaceOwnerPrefix_l::destroy(SurfaceOwnerPrefix_l* owner,u32 flags) {
    gabi::AutoFrame frame{0x02779154u};
    if (owner == nullptr) return;
    destroyEffectSurface(&owner->mSurface,2);
    if (flags & 1) operator_delete(owner);
}
/* 027791A8 */
void ControlPrefix_l::emptyHook() {}
/* 027791AC */
void Text::assureTermination() {}
/* 027791B0 */
void TextBufferPrefix_l::assureTermination() {
    u32 capacity=mCapacity;
    gabi::object_at<be<u8>>(mText.v.get()+capacity-1)=0;
}
/* 027791C8 */
void TextBufferA::destroy(TextBufferA* text,u32 flags) {
    gabi::AutoFrame frame{0x027791C8u};
    if (text != nullptr && (flags & 1)) operator_delete(text);
}
/* 027791DC */
Color4* Color4::construct(Color4* storage) {
    gabi::AutoFrame frame{0x027791DCu};
    if (storage==nullptr) storage=static_cast<Color4*>(operator_new(sizeof(Color4)));
    if (storage!=nullptr) {
        f32 zero=sColorZero;
        storage->mGreen=zero;
        storage->mRed=zero;
        storage->mBlue=zero;
        storage->mAlpha=sColorOne;
    }
    return storage;
}
/* 02779230 */
void TextBufferC::destroy(TextBufferC* text,u32 flags) {
    gabi::AutoFrame frame{0x02779230u};
    if (text != nullptr && (flags & 1)) operator_delete(text);
}
/* 02779364 */
void TextBufferD::destroy(TextBufferD* text,u32 flags) {
    gabi::AutoFrame frame{0x02779364u};
    if (text != nullptr && (flags & 1)) operator_delete(text);
}
/* 02779378 */
void TextBufferE::destroy(TextBufferE* text,u32 flags) {
    gabi::AutoFrame frame{0x02779378u};
    if (text != nullptr && (flags & 1)) operator_delete(text);
}
/* 0277938C */
void TextBufferF::destroy(TextBufferF* text,u32 flags) {
    gabi::AutoFrame frame{0x0277938Cu};
    if (text != nullptr && (flags & 1)) operator_delete(text);
}
/* 027793A0 */
void TextBufferG::destroy(TextBufferG* text,u32 flags) {
    gabi::AutoFrame frame{0x027793A0u};
    if (text != nullptr && (flags & 1)) operator_delete(text);
}
/* 027793B4 */
void ParameterBase_l::emptyHook() {}
/* 027793B8 */
u32 ByteParameter::kind() const { return kByteValue; }
/* 027793C0 */
const void* ByteParameter::readData() const { return &mValue; }
/* 027793C8 */
void* ByteParameter::writeData() { return &mValue; }
/* 027793D0 */
u32 ByteParameter::byteSize() const { return sizeof(mValue); }
/* 027793D8 */
u32 ScalarParameter::kind() const { return kScalarValue; }
/* 027793E0 */
const void* ScalarParameter::readData() const { return &mValue; }
/* 027793E8 */
void* ScalarParameter::writeData() { return &mValue; }
/* 027793F0 */
u32 ScalarParameter::byteSize() const { return sizeof(mValue); }
/* 027793F8 */
u32 IntegerParameter::kind() const { return kIntegerValue; }
/* 02779400 */
const void* IntegerParameter::readData() const { return &mValue; }
/* 02779408 */
void* IntegerParameter::writeData() { return &mValue; }
/* 02779410 */
u32 IntegerParameter::byteSize() const { return sizeof(mValue); }
/* 02779418 */
u32 ColorParameter::kind() const { return kColorValue; }
/* 02779420 */
const void* ColorParameter::readData() const { return &mValue; }
/* 02779428 */
void* ColorParameter::writeData() { return &mValue; }
/* 02779430 */
u32 ColorParameter::byteSize() const { return sizeof(mValue); }
// Hook distinctions identify table slots, not policy semantics.
/* 02779438 */
u32 ControlPrefix_l::constantOneHook() const { return 1; }
/* 02779440 */
u32 ControlPrefix_l::matchesValue(const ControlValueReference* value) const {
    const ControlValueRecord* record=value->mRecord;
    u32 token=mValueToken;
    return u32(record->mValueToken)==token;
}
/* 0277945C */
void ControlPrefix_l::alternateEmptyHook() {}
/* 02779460 */
void ControlPrefix_l::destroy(ControlPrefix_l* control,u32 flags) {
    gabi::AutoFrame frame{0x02779460u};
    if (control==nullptr) return;
    control->mPrimaryMethods=&sControlPrimaryMethods;
    control->mSecondaryMethods=&sControlSecondaryMethods;
    releaseEffectControl(control);
    destroyEffectControlBase(control,0);
    if (flags & 1) operator_delete(control);
}
/* 027794D4 */
u32 ControlOwnerPrefix_l::constantOneHook() const { return 1; }
/* 027794DC */
void ControlOwnerPrefix_l::emptyHook() {}
/* 02779628 */
ColorParameter* ColorParameter::construct(ColorParameter* storage) {
    gabi::AutoFrame frame{0x02779628u};
    if (storage==nullptr) storage=static_cast<ColorParameter*>(operator_new(sizeof(ColorParameter)));
    if (storage!=nullptr) {
        initializeEffectParameter(storage);
        storage->setMethods(&sColorParameterMethods);
        f32 zero=sColorZero;
        storage->mValue.mGreen=zero;
        storage->mValue.mRed=zero;
        storage->mValue.mBlue=zero;
        storage->mValue.mAlpha=sColorOne;
    }
    return storage;
}
/* 027794E0 */
ParameterGroupA* ParameterGroupA::construct(ParameterGroupA* storage) {
    gabi::AutoFrame frame{0x027794E0u};
    if (storage==nullptr) storage=static_cast<ParameterGroupA*>(operator_new(sizeof(ParameterGroupA)));
    if (storage!=nullptr) {
        initializeEffectParameter(&storage->mFirst);
        storage->mFirst.setMethods(&sIntegerParameterMethods);
        initializeEffectParameter(&storage->mSecond);
        storage->mSecond.setMethods(&sIntegerParameterMethods);
        initializeEffectParameter(&storage->mThird);
        storage->mThird.setMethods(&sIntegerParameterMethods);
        initializeEffectParameter(&storage->mColor);
        storage->mColor.setMethods(&sColorParameterMethods);
        f32 zero=sColorZero;
        storage->mColor.mValue.mBlue=zero;
        storage->mColor.mValue.mGreen=zero;
        storage->mColor.mValue.mRed=zero;
        storage->mColor.mValue.mAlpha=sColorOne;
        initializeEffectParameter(&storage->mLast);
        storage->mLast.setMethods(&sIntegerParameterMethods);
    }
    return storage;
}
/* 02779594 */
ParameterGroupB* ParameterGroupB::construct(ParameterGroupB* storage) {
    gabi::AutoFrame frame{0x02779594u};
    if (storage==nullptr) storage=static_cast<ParameterGroupB*>(operator_new(sizeof(ParameterGroupB)));
    if (storage!=nullptr) {
        initializeEffectParameter(&storage->mFirst);
        storage->mFirst.setMethods(&sByteParameterMethods);
        initializeEffectParameter(&storage->mSecond);
        storage->mSecond.setMethods(&sIntegerParameterMethods);
        initializeEffectParameter(&storage->mThird);
        storage->mThird.setMethods(&sIntegerParameterMethods);
        initializeEffectParameter(&storage->mFourth);
        storage->mFourth.setMethods(&sIntegerParameterMethods);
        initializeEffectParameter(&storage->mFifth);
        storage->mFifth.setMethods(&sIntegerParameterMethods);
    }
    return storage;
}
/* 0277969C */
SurfacePair* SurfacePair::construct(SurfacePair* storage) {
    gabi::AutoFrame frame{0x0277969Cu};
    if (storage==nullptr) storage=static_cast<SurfacePair*>(operator_new(sizeof(SurfacePair)));
    if (storage!=nullptr) {
        initializeSurface(storage->mSurface);
        initializeSecondSurface(storage->mSecondSurface);
    }
    return storage;
}
/* 02779888 */
EffectRuntimePrefix_l* EffectRuntimePrefix_l::construct(EffectRuntimePrefix_l* storage) {
    gabi::AutoFrame frame{0x02779888u};
    if (storage==nullptr) storage=static_cast<EffectRuntimePrefix_l*>(operator_new(sizeof(EffectRuntimePrefix_l)));
    if (storage!=nullptr) {
        initializeEffectRuntime(storage);
        initializeEffectParameterBlock(storage->mParameters);
        storage->mSurfaces=nullptr;
        storage->mThirdStorage=nullptr;
        storage->mSecondStorage=nullptr;
        storage->mThirdCount=0;
        storage->mSurfaceCount=0;
        storage->mSecondCount=0;
    }
    return storage;
}
/* 02777CB0 */
void EffectRuntimePrefix_l::releasePairHandles(u32 index) {
    gabi::AutoFrame frame{0x02777CB0u};
    SurfaceBatchEntry_l* entry=mSurfaces;
    if (index < u32(mSurfaceCount)) entry=&sead::elementAt(entry,index);
    for (SurfacePair& pair : entry->mPairs) {
        u32 handle=pair.mHeader.v;
        if (handle!=0) {
            releaseSurfaceHandle(entry->mReleaseContext,handle);
            pair.mHeader.v=0;
        }
    }
}
/* 027796EC */
void EffectRuntimePrefix_l::destroy(EffectRuntimePrefix_l* storage,u32 flags) {
    gabi::AutoFrame frame{0x027796ECu};
    if (storage==nullptr) return;
    storage->mRuntimeMethods=&sRuntimeMethods;
    if (storage->mSurfaces!=nullptr) {
        u32 index=0;
        while (s32(index)<s32(storage->mSurfaceCount)) {
            SurfaceBatchEntry_l* entry=storage->mSurfaces.get()+index;
            if (entry!=nullptr) {
                destroySurfaceReleaseContext(entry->mReleaseContext,2);
                destroySurfaceArray(entry->mPairs,2,sizeof(SurfacePair),&SurfaceOwnerPrefix_l::destroy,0,0);
                destroySurfaceState(entry->mSurfaceState,2);
                destroyEffectSurface(reinterpret_cast<SurfacePrefix_l*>(entry->mSurfaces[3]),2);
                destroyEffectSurface(reinterpret_cast<SurfacePrefix_l*>(entry->mSurfaces[2]),2);
                destroyEffectSurface(reinterpret_cast<SurfacePrefix_l*>(entry->mSurfaces[1]),2);
                destroyEffectSurface(reinterpret_cast<SurfacePrefix_l*>(entry->mSurfaces[0]),2);
            }
            ++index;
        }
        sead::RegistryHeap* heap=findRegistryHeap(sRegistryManager,storage->mSurfaces);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,storage->mSurfaces.get());
        storage->mSurfaceCount=0;
        storage->mSurfaces=nullptr;
    }
    if (storage->mSecondStorage!=nullptr) {
        sead::RegistryHeap* heap=findRegistryHeap(sRegistryManager,storage->mSecondStorage);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,storage->mSecondStorage.get());
        storage->mSecondCount=0;
        storage->mSecondStorage=nullptr;
    }
    if (storage->mThirdStorage!=nullptr) {
        sead::RegistryHeap* heap=findRegistryHeap(sRegistryManager,storage->mThirdStorage);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,storage->mThirdStorage.get());
        storage->mThirdCount=0;
        storage->mThirdStorage=nullptr;
    }
    destroyRuntimeBase(storage,0);
    if (flags & 1) operator_delete(storage);
}
/* 02779244 */
FormattedText128* FormattedText128::construct(FormattedText128* storage,const char* format,
    u32 first,u32 second,u32 third,u32 fourth,u32 fifth,u32 sixth,
    f64 float1,f64 float2,f64 float3,f64 float4,f64 float5,f64 float6,f64 float7,f64 float8) {
    bool floatingArguments=gabi::cpu->cr[6];
    gabi::AutoFrame frame{0x02779244u};
    gabi::FrameLocal<VariadicSaveArea_l> saved(0x18);
    saved->mGeneral[3]=second;
    saved->mGeneral[6]=fifth;
    saved->mGeneral[5]=fourth;
    saved->mGeneral[0]=gabi::ea(storage);
    saved->mGeneral[7]=sixth;
    saved->mGeneral[4]=third;
    saved->mGeneral[1]=gabi::ea(format);
    saved->mGeneral[2]=first;
    if (floatingArguments) {
        saved->mFloating[2]=float3;
        saved->mFloating[6]=float7;
        saved->mFloating[5]=float6;
        saved->mFloating[1]=float2;
        saved->mFloating[3]=float4;
        saved->mFloating[7]=float8;
        saved->mFloating[4]=float5;
        saved->mFloating[0]=float1;
    }
    if (storage==nullptr) storage=static_cast<FormattedText128*>(operator_new(sizeof(FormattedText128)));
    if (storage!=nullptr) {
        storage->mText=reinterpret_cast<char*>(storage->mBuffer);
        storage->mMethods=&sTextBufferMethods;
        storage->mCapacity=sizeof(storage->mBuffer);
        storage->mBuffer[127]=0;
        char* text=storage->mText;
        storage->mMethods=&sTextBaseMethods;
        storage->mBuffer[0]=0;
        storage->mMethods=&sFormattedTextMethods;
        gabi::FrameLocal<VariadicArguments_l> arguments(8);
        arguments->mGeneralIndex=2;
        arguments->mOverflow.v=gabi::native_sp()+0x90;
        arguments->mRegisterSave=saved.get();
        arguments->mFloatingIndex=0;
        formatTextArguments(storage,format,arguments.get());
    }
    return storage;
}
/* 02777BA8 */
u32 EffectRuntimePrefix_l::applySliceColor(u32 surfaceIndex,u32 sliceIndex,void* receiver,u32 fallback) {
    gabi::AutoFrame frame{0x02777BA8u};
    SurfaceBatchEntry_l* surface=mSurfaces;
    if (surfaceIndex<u32(mSurfaceCount)) surface=&sead::elementAt(surface,surfaceIndex);
    u32 parameterIndex=surfaceIndex & u32(mParameterIndexMask);
    gptr<ParameterBlock_l>* slot=mSecondStorage;
    if (parameterIndex<u32(mSecondCount)) slot=&sead::elementAt(slot,parameterIndex);
    ParameterBlock_l* block=*slot;
    const ParameterSlice_l* slice=&sead::elementAt(block->mSlices,sliceIndex);
    if (slice->mEnabled==0) return fallback;
    gabi::FrameLocal<SliceValues_l> values(8);
    gabi::FrameLocal<ColorWords_l[2]> colors(0x10);
    values->mFirst=slice->mFirst;
    (*colors)[1].mWords[3]=sSliceColorSecond.mWords[3];
    (*colors)[0].mWords[3]=sSliceColorFirst.mWords[3];
    (*colors)[0].mWords[1]=sSliceColorFirst.mWords[1];
    (*colors)[0].mWords[0]=sSliceColorFirst.mWords[0];
    (*colors)[1].mWords[2]=sSliceColorSecond.mWords[2];
    (*colors)[1].mWords[1]=sSliceColorSecond.mWords[1];
    values->mSecond=slice->mSecond;
    (*colors)[0].mWords[2]=sSliceColorFirst.mWords[2];
    (*colors)[1].mWords[0]=sSliceColorSecond.mWords[0];
    f32 scaleX=surface->mScaleX;
    f32 scaleY=surface->mScaleY;
    return submitSliceColor(receiver,2,values.get(),&(*colors)[0],fallback,scaleX,scaleY);
}
/* 0277A488 */
void FilterAaObject_l::destroy(FilterAaObject_l* storage,u32 flags) {
    gabi::AutoFrame frame{0x0277A488u};
    if (storage==nullptr) return;
    storage->mSecondaryMethods=&sFilterSecondaryMethods;
    storage->mPrimaryMethods=&sFilterPrimaryMethods;
    u32 index=0;
    while (s32(index)<s32(storage->mEntryCount)) {
        FilterSurfaceEntry_l* entry=storage->mEntries;
        if (index<u32(storage->mEntryCount)) entry=&sead::elementAt(entry,index);
        void* memory=entry->mStorage;
        if (memory!=nullptr) releaseFilterEntryStorage(memory);
        ++index;
    }
    if (storage->mEntries!=nullptr) {
        index=0;
        while (s32(index)<s32(storage->mEntryCount)) {
            FilterSurfaceEntry_l* entry=storage->mEntries.get()+index;
            if (entry!=nullptr) {
                destroyEffectSurface(reinterpret_cast<SurfacePrefix_l*>(entry->mThirdSurface),2);
                destroyFilterSurfaceArray(entry->mSurfaces,2,0x198,&destroyEffectSurface,0,0);
                destroySurfaceState(entry->mSurfaceState,2);
            }
            ++index;
        }
        sead::RegistryHeap* heap=findRegistryHeap(sRegistryManager,storage->mEntries);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,storage->mEntries.get());
        storage->mEntryCount=0;
        storage->mEntries=nullptr;
    }
    releaseEffectControl(&storage->mControl);
    storage->mControl.mPrimaryMethods=&sFilterControlPrimaryMethods;
    storage->mControl.mSecondaryMethods=&sFilterControlSecondaryMethods;
    releaseEffectControl(&storage->mControl);
    destroyEffectControlBase(&storage->mControl,0);
    storage->mParameterContainer.mMethods=&sFilterControlMethods;
    if (flags & 1) operator_delete(storage);
}
/* 027798F4 */
void EffectRuntimePrefix_l::initializeArrays(s32 surfaceCount,u32 sharedParameters,void* heap) {
    gabi::AutoFrame frame{0x027798F4u};
    if (sharedParameters==0 && surfaceCount<=1) sharedParameters=1;
    initializeRuntimeBase(this,surfaceCount,sharedParameters,heap);
    s32 requested=mRequestedSurfaceCount;
    SurfaceBatchEntry_l* created=nullptr;
    if (requested>0) {
        sead::RegistryHeap* allocator=static_cast<sead::RegistryHeap*>(heap);
        if (allocator==nullptr) allocator=registryCurrentHeap(sRegistryManager);
        created=gabi::call_ptr<SurfaceBatchEntry_l*>(allocator->dispatch()->mAllocate.mFunction,allocator,u32(requested)*sizeof(SurfaceBatchEntry_l),4);
        for (s32 index=0; index<requested; ++index) {
            SurfaceBatchEntry_l* entry=created+index;
            if (entry!=nullptr) {
                initializeSurface(entry->mSurfaces[0]);
                initializeSurface(entry->mSurfaces[1]);
                initializeSurface(entry->mSurfaces[2]);
                initializeSurface(entry->mSurfaces[3]);
                initializeSurfaceState(entry->mSurfaceState);
                constructPairArray(entry->mPairs,2,sizeof(SurfacePair),&SurfacePair::construct);
                initializeSurfaceReleaseContext(entry->mReleaseContext);
            }
        }
    }
    if (created!=nullptr) {
        mSurfaceCount=requested;
        mSurfaces=created;
    }
    /* the original walks a pointer to an end pointer computed in 32 bits (027799EC..02779A38: end = first +
     * count*0xC98, `cmplw p,end; bne`), so a count whose byte size wraps 2^32 ends early; guest EA arithmetic keeps that */
    u32 surfaceTotal=mSurfaceCount;
    u32 surfaceAt=u32(mSurfaces.v);
    u32 surfaceEnd=surfaceAt+surfaceTotal*u32(sizeof(SurfaceBatchEntry_l));
    for (;surfaceAt!=surfaceEnd;surfaceAt+=u32(sizeof(SurfaceBatchEntry_l)))
        initializeRuntimeSurface(this,gabi::at<SurfaceBatchEntry_l>(surfaceAt),heap);
    s32 parameterTotal=mParameterCount;
    if (sharedParameters==0) {
        s32 extraCount=s32(u32(parameterTotal)-1);
        if (extraCount>0) {
            sead::RegistryHeap* allocator=static_cast<sead::RegistryHeap*>(heap);
            if (allocator==nullptr) allocator=registryCurrentHeap(sRegistryManager);
            ParameterBlock_l* blocks=gabi::call_ptr<ParameterBlock_l*>(allocator->dispatch()->mAllocate.mFunction,allocator,u32(extraCount)*sizeof(ParameterBlock_l),4);
            for (s32 index=0;index<extraCount;++index) {
                ParameterBlock_l* block=blocks+index;
                if (block!=nullptr) initializeEffectParameterBlock(block);
            }
            if (blocks!=nullptr) {
                mThirdStorage=blocks;
                mThirdCount=extraCount;
            }
        }
    }
    parameterTotal=mParameterCount;
    if (parameterTotal>0) {
        sead::RegistryHeap* allocator=static_cast<sead::RegistryHeap*>(heap);
        if (allocator==nullptr) allocator=registryCurrentHeap(sRegistryManager);
        gptr<ParameterBlock_l>* slots=gabi::call_ptr<gptr<ParameterBlock_l>*>(allocator->dispatch()->mAllocate.mFunction,allocator,u32(parameterTotal)*4,4);
        if (slots!=nullptr) {
            mSecondStorage=slots;
            mSecondCount=parameterTotal;
        }
    }
    for (u32 index=0;s32(index)<s32(mParameterCount);++index) {
        gptr<ParameterBlock_l>* slot=mSecondStorage;
        if (index<u32(mSecondCount)) slot+=index;
        ParameterBlock_l* block;
        if (index==0) block=reinterpret_cast<ParameterBlock_l*>(mParameters);
        else {
            block=mThirdStorage;
            u32 extraIndex=index-1;
            if (extraIndex<u32(mThirdCount)) block+=extraIndex;
        }
        *slot=block;
        gptr<ParameterBlock_l>* selected=mSecondStorage;
        if (index<u32(mSecondCount)) selected+=index;
        FilterParameterContainer_l* values=mValueRecords;
        if (index<u32(mValueRecordCount)) values+=index;
        initializeRuntimeValues(*selected,values,heap);
    }
}
/* 0277A634 */
void FilterAaObject_l::initialize(const FilterSetupArguments_l* arguments,void* heap) {
    gabi::AutoFrame frame{0x0277A634u};
    s32 requested=arguments->mEntryCount;
    if (requested>0) {
        sead::RegistryHeap* allocator=static_cast<sead::RegistryHeap*>(heap);
        if (allocator==nullptr) allocator=registryCurrentHeap(sRegistryManager);
        FilterSurfaceEntry_l* entries=gabi::call_ptr<FilterSurfaceEntry_l*>(allocator->dispatch()->mAllocate.mFunction,allocator,u32(requested)*sizeof(FilterSurfaceEntry_l),4);
        for (s32 index=0;index<requested;++index) {
            FilterSurfaceEntry_l* entry=entries+index;
            if (entry!=nullptr) {
                initializeSurfaceState(entry->mSurfaceState);
                initializeSecondSurface(entry->mSecondSurfaceState);
                constructFilterSurfaceArray(entry->mSurfaces,2,0x198,&initializeSurface);
                initializeSurface(entry->mThirdSurface);
                constructFilterTextureData(entry->mTextureData);
            }
        }
        if (entries!=nullptr) {
            mEntries=entries;
            mEntryCount=requested;
        }
    }
    for (u32 index=0;s32(index)<s32(mEntryCount);++index) {
        FilterSurfaceEntry_l* entry=mEntries;
        if (index<u32(mEntryCount)) entry+=index;
        *reinterpret_cast<be<u8>*>(&entry->mHeader)=1;
        entry=mEntries;
        if (index<u32(mEntryCount)) entry+=index;
        u32 bytes=arguments->mStorageBytes;
        void* memory=nullptr;
        if (bytes!=0) memory=allocateFilterEntryStorage(bytes,heap,0x2000);
        entry->mStorage=memory;
        entry=mEntries;
        if (index<u32(mEntryCount)) entry+=index;
        initializeFilterAttribute(entry->mFirstAttribute);
        entry=mEntries;
        if (index<u32(mEntryCount)) entry+=index;
        initializeFilterIndices(entry->mFirstIndices);
        entry=mEntries;
        if (index<u32(mEntryCount)) entry+=index;
        initializeFilterAttribute(entry->mSecondAttribute);
        entry=mEntries;
        if (index<u32(mEntryCount)) entry+=index;
        initializeFilterIndices(entry->mSecondIndices);
    }
    gabi::FrameLocal<be<u32>> fetchName(0x64);
    gabi::FrameLocal<be<u32>> fetchValue(0x68);
    *fetchName.get()=gabi::ea(&sFilterFetchName);
    ShaderArchive_l* archive=filterArchive(53);
    for (u32 fetch=0;fetch<2;++fetch) {
        *fetchValue.get()=gabi::ea(fetch==0?&sFilterZeroToken:&sFilterFiveToken);
        ShaderVariantList_l* variants=&archive->mResource->mVariants;
        u32 selection=archive->mQuerySelection;
        s32 selected=findShaderVariant(variants,1,fetchName.get(),fetchValue.get(),selection);
        mLumaPrograms[fetch]=selectedVariant(archive,selected);
    }
    gabi::FrameLocal<be<u32>[4]> names(0x38);
    gabi::FrameLocal<be<u32>[3]> values(0x2C);
    gabi::FrameLocal<be<u32>[8]> choices(0x44);
    (*names)[1]=gabi::ea(&sFilterFetchName);
    (*names)[0]=gabi::ea(&sFilterAlphaName);
    (*names)[2]=gabi::ea(&sFilterDebugName);
    for (u32 choice=0;choice<7;++choice) (*choices)[choice]=gabi::ea(sFilterQualityValues.mValues[choice]);
    archive=filterArchive(51);
    for (u32 alpha=0;alpha<2;++alpha) {
        for (u32 quality=0;quality<7;++quality) {
            for (u32 debug=0;debug<3;++debug) {
                (*values)[0]=(*choices)[alpha];
                (*values)[2]=(*choices)[debug];
                (*values)[1]=(*choices)[quality];
                ShaderVariantList_l* variants=&archive->mResource->mVariants;
                u32 selection=archive->mQuerySelection;
                s32 selected=findShaderVariant(variants,3,&(*names)[0],&(*values)[0],selection);
                mPrograms[alpha][quality][debug]=selectedVariant(archive,selected);
            }
        }
    }
    (*names)[1]=gabi::ea(&sFilterAlphaName);
    (*names)[0]=gabi::ea(&sFilterDepthName);
    (*names)[3]=gabi::ea(&sFilterDebugName);
    (*names)[2]=gabi::ea(&sFilterFetchName);
    // The second choice table starts four bytes later; names[3] reuses its old first word.
    for (u32 choice=0;choice<7;++choice) (*choices)[choice+1]=gabi::ea(sFilterDepthQualityValues.mValues[choice]);
    archive=filterArchive(54);
    gabi::FrameLocal<be<u32>[4]> depthValues(0x1C);
    for (u32 depth=0;depth<2;++depth) {
        for (u32 alpha=0;alpha<2;++alpha) {
            for (u32 quality=0;quality<7;++quality) {
                for (u32 debug=0;debug<3;++debug) {
                    (*depthValues)[2]=(*choices)[quality+1];
                    (*depthValues)[0]=(*choices)[depth+1];
                    (*depthValues)[1]=(*choices)[alpha+1];
                    (*depthValues)[3]=(*choices)[debug+1];
                    ShaderVariantList_l* variants=&archive->mResource->mVariants;
                    u32 selection=archive->mQuerySelection;
                    s32 selected=findShaderVariant(variants,4,&(*names)[0],&(*depthValues)[0],selection);
                    mDepthPrograms[depth][alpha][quality][debug]=selectedVariant(archive,selected);
                }
            }
        }
    }
    gabi::FrameLocal<Text> name(0x6C);
    name->mMethods=&sFilterTextMethods;
    name->mText=reinterpret_cast<char*>(&sFilterName);
    s32 count=arguments->mEntryCount;
    noOpFilterAaControlHook(&mControl,count,name.get(),heap);
}
/* 02779120 */
u32 EffectRuntimePrefix_l::renderDefault(u32 surfaceIndex,void* context,u32 fallback) {
    gabi::AutoFrame frame{0x02779120u};
    return renderBloomEffect(this,surfaceIndex,context,0,fallback);
}
/* 02776E40 */
u32 EffectRuntimePrefix_l::drawColor(void* context,const Color4* uniformColor,const Color4* blendColor,u32 previousMode) {
    gabi::AutoFrame frame{0x02776E40u};
    ShaderArchive_l* archive=filterArchive(43);
    ColorProgramState_l* program=reinterpret_cast<ColorProgramState_l*>(archive->mResource->mVariants.mDefault.get());
    if ((u8(program->mFlags) & 2)!=0) {
        program->mFlags=u8(program->mFlags) & ~2u;
        updateColorProgram(program,0);
    }
    u32 selected=program->mData->mMode;
    if (previousMode!=selected) applyShaderMode(selected);
    u32 displayBytes=program->mDisplayListBytes;
    if (displayBytes!=0) callColorDisplayList(program->mDisplayList,displayBytes);
    else prepareColorProgram(program);
    gabi::FrameLocal<Color4> color(8);
    f32 red=uniformColor->mRed;
    f32 alpha=uniformColor->mAlpha;
    f32 green=uniformColor->mGreen;
    f32 scaledRed=gabi::fmuls_ppc(red,alpha);
    f32 blue=uniformColor->mBlue;
    f32 scaledGreen=gabi::fmuls_ppc(green,alpha);
    color->mRed=scaledRed;
    f32 scaledBlue=gabi::fmuls_ppc(blue,alpha);
    color->mGreen=scaledGreen;
    color->mAlpha=alpha;
    color->mBlue=scaledBlue;
    u32 mode=program->mUniformCount;
    UniformBinding_l* uniforms=static_cast<UniformBinding_l*>(program->mUniforms.get());
    if (mode>5) uniforms+=5;
    s32 vertexRegister=uniforms->mVertexRegister;
    if (vertexRegister!=-1) setColorVertexUniform(vertexRegister,4,color.get());
    s32 pixelRegister=uniforms->mPixelRegister;
    if (pixelRegister!=-1) setColorPixelUniform(pixelRegister,4,color.get());
    gabi::FrameLocal<ColorRenderState_l> state(0x18);
    sead::CompactRenderState::create(state.get());
    alpha=blendColor->mAlpha;
    red=blendColor->mRed;
    state->mBlend.mColorSource=1;
    state->mBlend.mAlphaSource=1;
    scaledRed=gabi::fmuls_ppc(red,alpha);
    blue=blendColor->mBlue;
    state->mBlend.mColorDestination=13;
    color->mRed=scaledRed;
    green=blendColor->mGreen;
    scaledBlue=gabi::fmuls_ppc(blue,alpha);
    state->mBlend.mAlphaDestination=13;
    color->mBlue=scaledBlue;
    color->mAlpha=alpha;
    scaledGreen=gabi::fmuls_ppc(green,alpha);
    state->mDepthTest=0;
    state->mDepthWrite=0;
    color->mGreen=scaledGreen;
    state->mBlendAlpha=alpha;
    state->mBlendRed=scaledRed;
    state->mBlendBlue=scaledBlue;
    state->mBlendGreen=scaledGreen;
    state->apply();
    drawRuntimeProgram(this,program,context);
    return selected;
}
/* 02779BFC */
FilterAaObject_l* FilterAaObject_l::construct(FilterAaObject_l* storage) {
    gabi::AutoFrame frame{0x02779BFCu};
    if (storage==nullptr) storage=static_cast<FilterAaObject_l*>(operator_new(sizeof(FilterAaObject_l)));
    if (storage==nullptr) {
        std::atomic_thread_fence(std::memory_order_acquire);
        return nullptr;
    }
    gabi::FrameLocal<Text> baseName(0x10);
    baseName->mMethods=&sFilterBaseTextMethods;
    baseName->mText=reinterpret_cast<char*>(&sFilterBaseName);
    constructFilterBase(storage,baseName.get(),0);
    storage->mPrimaryMethods=&sFilterPrimaryMethods;
    storage->mSecondaryMethods=&sFilterSecondaryMethods;
    sead::CompactRenderState::create(&storage->mRenderState);
    constructFilterParameterContainer(&storage->mParameterContainer);
    {
        gabi::FrameLocal<Text> name(0x18);
        gabi::FrameLocal<Text> description(0x20);
        gabi::FrameLocal<Text> range(0x118);
        configureFilterParameter(&storage->mAntialiasType,&sFilterIntegerMethods,&sFilterTypeName,&sFilterTypeDescription,&sFilterEmptyType,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    storage->mAntialiasType.mValue=0;
    {
        gabi::FrameLocal<Text> name(0x28);
        gabi::FrameLocal<Text> description(0x30);
        gabi::FrameLocal<Text> range(0x120);
        configureFilterParameter(&storage->mEnabled,&sFilterByteMethods,&sFilterEnableName,&sFilterEnableDescription,&sFilterEmptyEnable,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    storage->mEnabled.mValue=1;
    f32 defaultAlpha=sFilterDefaultAlpha;
    {
        gabi::FrameLocal<Text> name(0x38);
        gabi::FrameLocal<Text> description(0x40);
        gabi::FrameLocal<Text> range(0x48);
        configureFilterParameter(&storage->mAlphaOutput,&sFilterScalarMethods,&sFilterAlphaOutputName,&sFilterAlphaOutputDescription,&sFilterAlphaOutputRange,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    storage->mAlphaOutput.mValue=defaultAlpha;
    {
        gabi::FrameLocal<Text> name(0x50);
        gabi::FrameLocal<Text> description(0x58);
        gabi::FrameLocal<Text> range(0x128);
        configureFilterParameter(&storage->mEdgeQuality,&sFilterIntegerMethods,&sFilterEdgeQualityName,&sFilterEdgeQualityDescription,&sFilterEmptyType,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    storage->mEdgeQuality.mValue=0;
    {
        gabi::FrameLocal<Text> name(0x60);
        gabi::FrameLocal<Text> description(0x68);
        gabi::FrameLocal<Text> range(0x130);
        configureFilterParameter(&storage->mFetchQuality,&sFilterIntegerMethods,&sFilterFetchQualityName,&sFilterFetchQualityDescription,&sFilterEmptyType,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    storage->mFetchQuality.mValue=1;
    {
        gabi::FrameLocal<Text> name(0x70);
        gabi::FrameLocal<Text> description(0x78);
        gabi::FrameLocal<Text> range(0x80);
        configureFilterParameter(&storage->mEdgeThresholdScale,&sFilterScalarMethods,&sFilterEdgeThresholdScaleName,&sFilterEdgeThresholdScaleDescription,&sFilterEdgeThresholdScaleRange,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    storage->mEdgeThresholdScale.mValue=defaultAlpha;
    {
        gabi::FrameLocal<Text> name(0x88);
        gabi::FrameLocal<Text> description(0x90);
        gabi::FrameLocal<Text> range(0x138);
        configureFilterParameter(&storage->mLumaCoefficients,&sFilterColorMethods,&sFilterLumaName,&sFilterLumaDescription,&sFilterEmptyColor,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    ColorWords_l* initialLuma=reinterpret_cast<ColorWords_l*>(&storage->mLumaCoefficients.mValue);
    initialLuma->mWords[0]=sFilterLumaDefault.mWords[0];
    initialLuma->mWords[1]=sFilterLumaDefault.mWords[1];
    initialLuma->mWords[2]=sFilterLumaDefault.mWords[2];
    initialLuma->mWords[3]=sFilterLumaDefault.mWords[3];
    f32 defaultSubpixel=sFilterDefaultSubpixel;
    {
        gabi::FrameLocal<Text> name(0x98);
        gabi::FrameLocal<Text> description(0xA0);
        gabi::FrameLocal<Text> range(0xA8);
        configureFilterParameter(&storage->mSubpixel,&sFilterScalarMethods,&sFilterSubpixelName,&sFilterSubpixelDescription,&sFilterSubpixelRange,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    storage->mSubpixel.mValue=defaultSubpixel;
    f32 defaultMaximumSpan=sFilterDefaultMaximumSpan;
    {
        gabi::FrameLocal<Text> name(0xB0);
        gabi::FrameLocal<Text> description(0xB8);
        gabi::FrameLocal<Text> range(0xC0);
        configureFilterParameter(&storage->mMaximumSpan,&sFilterScalarMethods,&sFilterMaximumSpanName,&sFilterMaximumSpanDescription,&sFilterMaximumSpanRange,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    storage->mMaximumSpan.mValue=defaultMaximumSpan;
    {
        gabi::FrameLocal<Text> name(0xC8);
        gabi::FrameLocal<Text> description(0xD0);
        gabi::FrameLocal<Text> range(0xD8);
        configureFilterParameter(&storage->mSpanMultiply,&sFilterScalarMethods,&sFilterSpanMultiplyName,&sFilterSpanMultiplyDescription,&sFilterSpanMultiplyRange,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    storage->mSpanMultiply.mValue=defaultAlpha;
    f32 defaultMinimumSpan=sFilterDefaultMinimumSpan;
    {
        gabi::FrameLocal<Text> name(0xE0);
        gabi::FrameLocal<Text> description(0xE8);
        gabi::FrameLocal<Text> range(0xF0);
        configureFilterParameter(&storage->mMinimumSpan,&sFilterScalarMethods,&sFilterMinimumSpanName,&sFilterMinimumSpanDescription,&sFilterMinimumSpanRange,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    storage->mMinimumSpan.mValue=defaultMinimumSpan;
    {
        gabi::FrameLocal<Text> name(0xF8);
        gabi::FrameLocal<Text> description(0x100);
        gabi::FrameLocal<Text> range(0x140);
        configureFilterParameter(&storage->mReprojection,&sFilterByteMethods,&sFilterReprojectionName,&sFilterReprojectionDescription,&sFilterEmptyEnable,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    storage->mReprojection.mValue=1;
    {
        gabi::FrameLocal<Text> name(0x108);
        gabi::FrameLocal<Text> description(0x110);
        gabi::FrameLocal<Text> range(0x8);
        configureFilterParameter(&storage->mReprojectionMoveLimit,&sFilterScalarMethods,&sFilterMoveLimitName,&sFilterMoveLimitDescription,&sFilterMoveLimitRange,&storage->mParameterContainer,name.get(),description.get(),range.get());
    }
    storage->mReprojectionMoveLimit.mValue=sFilterDefaultMoveLimit;
    storage->mRuntimeWord=0;
    storage->mRuntimeCounter=0;
    storage->mRuntimeFlag=0;
    storage->mEntries=nullptr;
    storage->mEntryCount=0;
    constructFilterControl(&storage->mControl);
    storage->mRenderState.mAlphaTest=0;
    storage->mRenderState.mDepthTest=0;
    storage->mRenderState.mWriteAlpha=1;
    storage->mRenderState.mWriteRed=1;
    storage->mRenderState.mWriteBlue=1;
    storage->mRenderState.mDepthWrite=0;
    storage->mRenderState.mCullMode=2;
    storage->mRenderState.mWriteGreen=1;
    storage->mRenderState.mBlendEnable=0;
    gabi::FrameLocal<Text> name(8);
    name->mText=reinterpret_cast<char*>(&sFilterContainerName);
    name->mMethods=&sFilterBaseTextMethods;
    attachFilterParameters(storage,&storage->mParameterContainer,name.get());
    name->mMethods=&sFilterBaseTextMethods;
    name->mText=reinterpret_cast<char*>(&sFilterIcon);
    setFilterIcon(&storage->mSecondaryMethods,name.get());
    gabi::FrameLocal<ColorWords_l> defaults(0x154);
    defaults->mWords[2]=sFilterLumaDefault.mWords[2];
    defaults->mWords[0]=sFilterLumaDefault.mWords[0];
    defaults->mWords[1]=sFilterLumaDefault.mWords[1];
    defaults->mWords[3]=sFilterLumaDefault.mWords[3];
    storage->mFetchQuality.mValue=1;
    storage->mEdgeQuality.mValue=0;
    storage->mEdgeThresholdScale.mValue=defaultAlpha;
    ColorWords_l* luma=reinterpret_cast<ColorWords_l*>(&storage->mLumaCoefficients.mValue);
    luma->mWords[0]=defaults->mWords[0];
    luma->mWords[1]=defaults->mWords[1];
    luma->mWords[2]=defaults->mWords[2];
    storage->mAlphaOutput.mValue=defaultAlpha;
    luma->mWords[3]=defaults->mWords[3];
    storage->mSpanMultiply.mValue=defaultAlpha;
    storage->mRuntimeCounter=0;
    storage->mMaximumSpan.mValue=defaultMaximumSpan;
    storage->mSubpixel.mValue=defaultSubpixel;
    storage->mMinimumSpan.mValue=defaultMinimumSpan;
    storage->mReprojectionMoveLimit.mValue=sFilterDefaultMoveLimit;
    storage->mReprojection.mValue=1;
    std::atomic_thread_fence(std::memory_order_acquire);
    return storage;
}
/* 02776524 */
u32 EffectRuntimePrefix_l::drawMipBlur(u32 surfaceIndex,u32 level,u32 previousMode,f64 strength) {
    gabi::AutoFrame frame{0x02776524u};
    SurfaceBatchEntry_l* entry=mSurfaces;
    if (surfaceIndex<u32(mSurfaceCount)) entry=&sead::elementAt(entry,surfaceIndex);
    TextureImage_l* texture=reinterpret_cast<TextureImage_l*>(entry->mPairs[0].mSurface);
    SurfaceImagePrefix_l* target=reinterpret_cast<SurfaceImagePrefix_l*>(entry->mPairs[0].mSecondSurface);
    BlurViewport_l* viewport=reinterpret_cast<BlurViewport_l*>(entry->mSurfaceState);
    gabi::FrameLocal<ColorRenderState_l> state(0x70);
    sead::CompactRenderState::create(state.get());
    state->mBlendEnable=0;
    state->mDepthTest=0;
    state->mDepthWrite=0;
    state->apply();
    u32 nextLevel=level+1;
    u32 widthWord=target->mGraphics.mWidth;
    u32 heightWord=target->mGraphics.mHeight;
    u32 width=(nextLevel & 32)?0:widthWord>>(nextLevel & 31);
    u32 height=(nextLevel & 32)?0:heightWord>>(nextLevel & 31);
    u32 minimumWidth=target->mMinimumWidth;
    u32 minimumHeight=target->mMinimumHeight;
    if (s32(width)<=s32(minimumWidth)) width=minimumWidth;
    if (s32(minimumHeight)>s32(height)) height=minimumHeight;
    gabi::FrameLocal<SliceValues_l> dimensions(0x18);
    dimensions->mFirst=f32(width);
    dimensions->mSecond=f32(height);
    f32 zero=sBlurZero;
    f32 levelValue=f32(s32(level));
    texture->mLodMinimum=levelValue;
    texture->mLodMaximum=levelValue;
    texture->mFlags=u8(texture->mFlags) | 8;
    texture->mLodBias=zero;
    if (u32(target->mMipIndex)!=nextLevel) {
        target->mMipIndex=nextLevel;
        target->mMipDirty=1;
    }
    f32 extentX=dimensions->mFirst;
    f32 extentY=dimensions->mSecond;
    viewport->mWidth=extentX;
    viewport->mHeight=extentY;
    gabi::FrameLocal<Color4> bounds(0x20);
    bounds->mBlue=zero<extentX?extentX:zero;
    bounds->mRed=zero<extentX?zero:extentX;
    bounds->mAlpha=zero<extentY?extentY:zero;
    bounds->mGreen=zero<extentY?zero:extentY;
    viewport->mLeft=bounds->mRed;
    viewport->mBottom=bounds->mGreen;
    viewport->mRight=bounds->mBlue;
    viewport->mTop=bounds->mAlpha;
    activateFilterFrame(reinterpret_cast<sead::DisplayFrame*>(viewport));
    gabi::FrameLocal<ViewportRecord_l> viewportRecord(0x58);
    constructBlurViewport(viewportRecord.get(),viewport);
    applyBlurViewport(viewportRecord.get(),viewport);
    ShaderArchive_l* archive=filterArchive(43);
    ColorProgramState_l* program=reinterpret_cast<ColorProgramState_l*>(archive->mResource->mVariants.mVariants.get());
    previousMode=activateEffectProgram(program,previousMode);
    drawRuntimeProgram(this,program,texture);
    flushBlurSurface(target);
    f32 inverseX=gabi::fdivs_ppc(sBlurOne,f32(dimensions->mFirst));
    f32 inverseY=gabi::fdivs_ppc(sBlurOne,f32(dimensions->mSecond));
    archive=filterArchive(42);
    gabi::FrameLocal<Text> name(0x50);
    name->mMethods=&sBlurTextMethods;
    name->mText=reinterpret_cast<char*>(&sBlurName);
    u32 targetWidth=blurDimension(dimensions->mFirst);
    u32 targetHeight=blurDimension(dimensions->mSecond);
    SurfaceImagePrefix_l* temporary=allocateBlurSurface(entry->mReleaseContext,name.get(),26,targetWidth,targetHeight,1,0,0,1);
    if (!matchingBlurDescriptor(texture,target)) copyBlurDescriptor(texture,target);
    else copyBlurStorage(texture,target);
    f32 nextLevelValue=f32(s32(nextLevel));
    setBlurLod(texture,nextLevelValue,zero);
    if (u32(target->mMipIndex)!=0) {
        target->mMipIndex=0;
        target->mMipDirty=1;
    }
    bindBlurTarget(target,temporary);
    activateFilterFrame(reinterpret_cast<sead::DisplayFrame*>(viewport));
    program=reinterpret_cast<ColorProgramState_l*>(archive->mResource->mVariants.mDefault.get());
    previousMode=activateEffectProgram(program,previousMode);
    gabi::FrameLocal<Color4> horizontal(0x30);
    horizontal->mGreen=zero;
    horizontal->mAlpha=zero;
    horizontal->mBlue=zero;
    horizontal->mRed=gabi::fmuls_fpr(strength,gabi::fpr_from_f32(inverseX));
    uploadBlurOffsets(program,horizontal.get());
    drawRuntimeProgram(this,program,texture);
    flushBlurSurface(target);
    finishBlurSurface(target);
    copyBlurLevel(target,texture,nextLevel,0);
    texture->mLodMinimum=zero;
    texture->mLodMaximum=zero;
    texture->mLodBias=zero;
    texture->mFlags=u8(texture->mFlags) | 8;
    if (!matchingBlurDescriptor(texture,temporary)) copyBlurDescriptor(texture,temporary);
    else copyBlurStorage(texture,temporary);
    activateFilterFrame(reinterpret_cast<sead::DisplayFrame*>(viewport));
    program=reinterpret_cast<ColorProgramState_l*>(program->mData->mVariants.mVariants.get());
    previousMode=activateEffectProgram(program,previousMode);
    gabi::FrameLocal<Color4> vertical(0x40);
    vertical->mGreen=gabi::fmuls_fpr(strength,gabi::fpr_from_f32(inverseY));
    vertical->mAlpha=zero;
    vertical->mBlue=zero;
    vertical->mRed=zero;
    uploadBlurOffsets(program,vertical.get());
    drawRuntimeProgram(this,program,texture);
    flushBlurSurface(target);
    finishBlurSurface(target);
    if (!matchingBlurDescriptor(texture,target)) copyBlurDescriptor(texture,target);
    else copyBlurStorage(texture,target);
    releaseSurfaceHandle(entry->mReleaseContext,gabi::ea(temporary));
    std::atomic_thread_fence(std::memory_order_acquire);
    return previousMode;
}
/* 02777048 */
u32 EffectRuntimePrefix_l::drawBloomLevels(u32 surfaceIndex,u32 previousMode) {
    // The compiler leaves these registers live for an out-of-domain mode.
    f64 offsetX=gabi::cpu->f[22].ps1;
    f64 offsetY=gabi::cpu->f[24].ps1;
    gabi::AutoFrame frame{0x02777048u};
    u32 parameterIndex=surfaceIndex & u32(mParameterIndexMask);
    gptr<ParameterBlock_l>* slot=mSecondStorage;
    if (parameterIndex<u32(mSecondCount)) slot=&sead::elementAt(slot,parameterIndex);
    BloomLevelParameters_l* parameters=reinterpret_cast<BloomLevelParameters_l*>((*slot).get());
    if (u32(parameters->mMode)==0) {
        std::atomic_thread_fence(std::memory_order_acquire);
        return previousMode;
    }
    SurfaceBatchEntry_l* entry=mSurfaces;
    if (surfaceIndex<u32(mSurfaceCount)) entry=&sead::elementAt(entry,surfaceIndex);
    SurfaceImagePrefix_l* secondTarget=reinterpret_cast<SurfaceImagePrefix_l*>(entry->mPairs[1].mSecondSurface);
    TextureImage_l* firstTexture=reinterpret_cast<TextureImage_l*>(entry->mPairs[0].mSurface);
    SurfaceImagePrefix_l* firstTarget=reinterpret_cast<SurfaceImagePrefix_l*>(entry->mPairs[0].mSecondSurface);
    TextureImage_l* secondTexture=reinterpret_cast<TextureImage_l*>(entry->mPairs[1].mSurface);
    TextureImage_l* originalTexture=reinterpret_cast<TextureImage_l*>(entry->mSurfaces[0]);
    BlurViewport_l* viewport=reinterpret_cast<BlurViewport_l*>(entry->mSurfaceState);
    u32 widthWord=secondTarget->mGraphics.mWidth;
    u32 heightWord=secondTarget->mGraphics.mHeight;
    u32 minimumWidth=secondTarget->mMinimumWidth;
    u32 minimumHeight=secondTarget->mMinimumHeight;
    if (s32(widthWord)<=s32(minimumWidth)) widthWord=minimumWidth;
    if (s32(heightWord)<=s32(minimumHeight)) heightWord=minimumHeight;
    f32 width=f32(widthWord), height=f32(heightWord);
    s32 iterations=parameters->mIterations;
    gabi::FrameLocal<ColorRenderState_l> state(0x78);
    sead::CompactRenderState::create(state.get());
    state->mDepthWrite=0;
    state->mBlendEnable=0;
    state->mDepthTest=0;
    state->apply();
    f32 zero=sBlurZero;
    SurfaceImagePrefix_l* previousStorage=nullptr;
    gabi::FrameLocal<Text> name(0x18);
    gabi::FrameLocal<Color4> bounds(0x20);
    gabi::FrameLocal<Color4> firstValues(0x30);
    gabi::FrameLocal<Color4> secondValues(0x40);
    gabi::FrameLocal<ViewportRecord_l> viewportRecord(0x60);
    if (iterations>0) {
        f32 one=sBlurOne;
        f32 half=sBloomHalf;
        do {
            u32 mode=parameters->mMode;
            if (mode==1) {
                width=gabi::fmuls_ppc(width,half);
                offsetY=gabi::fpr_from_f32(zero);
                offsetX=gabi::fpr_from_f32(gabi::fdivs_ppc(one,width));
            } else if (mode==2) {
                height=gabi::fmuls_ppc(height,half);
                offsetX=gabi::fpr_from_f32(zero);
                offsetY=gabi::fpr_from_f32(gabi::fdivs_ppc(one,height));
            }
            if (!(width<one) && !(height<one)) {
                name->mMethods=&sBlurTextMethods;
                name->mText=reinterpret_cast<char*>(&sBloomFirstTargetName);
                SurfaceImagePrefix_l* firstStorage=allocateBlurSurface(entry->mReleaseContext,name.get(),26,blurDimension(width),blurDimension(height),1,0,0,1);
                if (!matchingBlurDescriptor(firstTexture,firstStorage)) copyBlurDescriptor(firstTexture,firstStorage);
                else copyBlurStorage(firstTexture,firstStorage);
                bindBlurTarget(firstTarget,firstStorage);
                setBloomViewport(viewport,bounds.get(),width,height,zero);
                constructBlurViewport(viewportRecord.get(),viewport);
                applyBlurViewport(viewportRecord.get(),viewport);
                ShaderArchive_l* archive=filterArchive(43);
                ColorProgramState_l* program=reinterpret_cast<ColorProgramState_l*>(archive->mResource->mVariants.mVariants.get());
                previousMode=activateEffectProgram(program,previousMode);
                firstValues->mAlpha=zero;
                firstValues->mGreen=zero;
                firstValues->mBlue=zero;
                firstValues->mRed=zero;
                uploadBloomZeros(program,firstValues.get());
                viewport->mTarget=firstTarget;
                activateFilterFrame(reinterpret_cast<sead::DisplayFrame*>(viewport));
                drawRuntimeProgram(this,program,secondTexture);
                flushBlurSurface(firstTarget);
                if (previousStorage!=nullptr) releaseSurfaceHandle(entry->mReleaseContext,gabi::ea(previousStorage));
                name->mMethods=&sBlurTextMethods;
                name->mText=reinterpret_cast<char*>(&sBloomSecondTargetName);
                previousStorage=allocateBlurSurface(entry->mReleaseContext,name.get(),26,blurDimension(width),blurDimension(height),1,0,0,1);
                bindBlurTarget(secondTarget,previousStorage);
                if (!matchingBlurDescriptor(secondTexture,previousStorage)) copyBlurDescriptor(secondTexture,previousStorage);
                else copyBlurStorage(secondTexture,previousStorage);
                archive=filterArchive(42);
                u32 secondMode=parameters->mMode;
                ShaderVariant_l* selected=secondMode==1?archive->mResource->mVariants.mDefault.get():archive->mResource->mVariants.mVariants.get();
                program=reinterpret_cast<ColorProgramState_l*>(selected);
                previousMode=activateEffectProgram(program,previousMode);
                secondValues->mAlpha=zero;
                secondValues->mGreen=gabi::f32_from_fpr(offsetY);
                secondValues->mBlue=zero;
                secondValues->mRed=gabi::f32_from_fpr(offsetX);
                uploadBlurOffsets(program,secondValues.get());
                viewport->mTarget=secondTarget;
                activateFilterFrame(reinterpret_cast<sead::DisplayFrame*>(viewport));
                drawRuntimeProgram(this,program,firstTexture);
                flushBlurSurface(secondTarget);
                releaseSurfaceHandle(entry->mReleaseContext,gabi::ea(firstStorage));
            }
        } while (--iterations!=0);
    }
    f32 lod=originalTexture->mLodMinimum;
    u32 level=blurDimension(lod);
    u32 originalWidth=originalTexture->mGraphics.mWidth;
    u32 originalHeight=originalTexture->mGraphics.mHeight;
    u32 scaledWidth=(level & 32)?0:originalWidth>>(level & 31);
    u32 scaledHeight=(level & 32)?0:originalHeight>>(level & 31);
    u32 originalMinimumWidth=originalTexture->mMinimumWidth;
    u32 originalMinimumHeight=originalTexture->mMinimumHeight;
    if (s32(scaledWidth)<=s32(originalMinimumWidth)) scaledWidth=originalMinimumWidth;
    if (s32(scaledHeight)<=s32(originalMinimumHeight)) scaledHeight=originalMinimumHeight;
    gabi::FrameLocal<Color4> finalBounds(0x50);
    setBloomViewport(viewport,finalBounds.get(),f32(scaledWidth),f32(scaledHeight),zero);
    copyBlurLevel(firstTarget,originalTexture,level,0);
    viewport->mTarget=firstTarget;
    activateFilterFrame(reinterpret_cast<sead::DisplayFrame*>(viewport));
    constructBlurViewport(viewportRecord.get(),viewport);
    applyBlurViewport(viewportRecord.get(),viewport);
    previousMode=drawColor(secondTexture,&parameters->mTint,reinterpret_cast<const Color4*>(&sSliceColorSecond),previousMode);
    releaseSurfaceHandle(entry->mReleaseContext,gabi::ea(previousStorage));
    std::atomic_thread_fence(std::memory_order_acquire);
    return previousMode;
}
/* 0277AD60 */
u32 FilterAaObject_l::render(u32 surfaceIndex,sead::RenderSurface* renderSurface,sead::Viewport* viewContext,void* output,void* depth,u32 depthKind,void* previousDepth,u32 previousMode) {
    gabi::AutoFrame frame{0x0277AD60u};
    f32 zero=sFilterRenderZero;
    f32 alpha=mAlphaOutput.mValue;
    gabi::cpu->f[13].ps0=gabi::cpu->f[13].ps1=gabi::fpr_from_f32(alpha);
    if (!(alpha>zero)) {
        u32 selected=copyFilterOutput(output,viewContext,&sFilterCopyFirstSettings,&sFilterCopySecondSettings,previousMode);
        std::atomic_thread_fence(std::memory_order_acquire);
        return selected;
    }
    FilterSurfaceEntry_l* entry=mEntries;
    if (surfaceIndex<u32(mEntryCount)) entry=&sead::elementAt(entry,surfaceIndex);
    bool reproject=depth!=nullptr && previousDepth!=nullptr && u8(mReprojection.mValue)!=0;
    gabi::FrameLocal<SliceValues_l> dimensions(0x18);
    viewContext->mapExtent(reinterpret_cast<sead::RenderVector2*>(dimensions.get()),renderSurface);
    gabi::FrameLocal<Text> name(0x90);
    name->mMethods=&sFilterBaseTextMethods;
    name->mText=reinterpret_cast<char*>(&sFilterRenderLumaName);
    f32 width=dimensions->mFirst, height=dimensions->mSecond;
    f32 limit=sFilterRenderUnsignedLimit;
    f32 qualityFallback=alpha;
    u32 widthWord, heightWord;
    if (width<limit) widthWord=u32(gabi::ftoi(gabi::fpr_from_f32(width)));
    else {
        qualityFallback=gabi::fsubs_ppc(width,limit);
        gabi::cpu->f[13].ps0=gabi::cpu->f[13].ps1=gabi::fpr_from_f32(qualityFallback);
        widthWord=u32(gabi::ftoi(gabi::fpr_from_f32(qualityFallback)))+0x80000000u;
    }
    if (height<limit) heightWord=u32(gabi::ftoi(gabi::fpr_from_f32(height)));
    else {
        qualityFallback=gabi::fsubs_ppc(height,limit);
        gabi::cpu->f[13].ps0=gabi::cpu->f[13].ps1=gabi::fpr_from_f32(qualityFallback);
        heightWord=u32(gabi::ftoi(gabi::fpr_from_f32(qualityFallback)))+0x80000000u;
    }
    SurfaceImagePrefix_l* lumaStorage=allocateFilterTarget(sFilterTargetPool,name.get(),1,widthWord,heightWord,1,0,0,1,0);
    f32 one=sFilterDefaultAlpha;
    width=dimensions->mFirst;
    height=dimensions->mSecond;
    f32 inverseX=gabi::fdivs_ppc(one,width);
    f32 subpixelConstant=sFilterDefaultSubpixel;
    f32 smallX=gabi::fmuls_ppc(inverseX,subpixelConstant);
    f32 inverseY=gabi::fdivs_ppc(one,height);
    f32 half=sFilterRenderHalf;
    f32 halfY=gabi::fmuls_ppc(inverseY,half);
    f32 halfX=gabi::fmuls_ppc(inverseX,half);
    f32 smallY=gabi::fmuls_ppc(inverseY,subpixelConstant);
    gabi::FrameLocal<Color4> steps(0x30), offsets(0x40), luma(0x50), thresholds(0x60), spans(0x70);
    steps->mRed=inverseX; steps->mGreen=inverseY; steps->mBlue=halfX; steps->mAlpha=halfY;
    offsets->mRed=halfX; offsets->mGreen=halfY; offsets->mBlue=smallX; offsets->mAlpha=smallY;
    u32 quality=mEdgeQuality.mValue;
    f64 firstThreshold=gabi::fpr_from_f32(inverseX), secondThreshold=gabi::cpu->f[13].ps0;
    if (quality==0) { firstThreshold=gabi::fpr_from_f32(sFilterThresholdSmall); secondThreshold=gabi::fpr_from_f32(sFilterThresholdLarge); }
    else if (quality==1) { firstThreshold=gabi::fpr_from_f32(sFilterThresholdMediumSmall); secondThreshold=gabi::fpr_from_f32(sFilterThresholdMediumLarge); }
    else if (quality==2) { firstThreshold=gabi::fpr_from_f32(sFilterThresholdFine); secondThreshold=gabi::fpr_from_f32(sFilterThresholdSmall); }
    s32 fetchQuality=mFetchQuality.mValue;
    f32 thresholdScale=mEdgeThresholdScale.mValue;
    if (fetchQuality>=5) {
        f32 multiplier=sFilterThresholdMultiplier;
        secondThreshold=gabi::fpr_from_f32(gabi::fmuls_fpr(secondThreshold,gabi::fpr_from_f32(multiplier)));
        firstThreshold=gabi::fpr_from_f32(gabi::fmuls_fpr(firstThreshold,gabi::fpr_from_f32(multiplier)));
    }
    thresholds->mRed=gabi::fdivs_fpr(secondThreshold,gabi::fpr_from_f32(thresholdScale));
    f32 red=mLumaCoefficients.mValue.mRed;
    f32 green=mLumaCoefficients.mValue.mGreen;
    f32 denominator=gabi::fadds_ppc(red,green);
    f32 spanMultiplier=mSpanMultiply.mValue;
    thresholds->mGreen=gabi::fdivs_fpr(firstThreshold,gabi::fpr_from_f32(thresholdScale));
    thresholds->mBlue=thresholdScale;
    thresholds->mAlpha=gabi::fdivs_ppc(one,thresholdScale);
    spans->mRed=mSubpixel.mValue;
    spans->mGreen=mMaximumSpan.mValue;
    f32 spanScale=gabi::fmuls_ppc(sFilterDefaultMoveLimit,spanMultiplier);
    spans->mBlue=gabi::fdivs_ppc(one,spanScale);
    spans->mAlpha=gabi::fmuls_ppc(sFilterThresholdSmall,f32(mMinimumSpan.mValue));
    red=mLumaCoefficients.mValue.mRed;
    if (denominator==zero) denominator=one;
    luma->mRed=gabi::fdivs_ppc(red,denominator);
    f32 currentAlpha=mAlphaOutput.mValue;
    green=mLumaCoefficients.mValue.mGreen;
    luma->mGreen=gabi::fdivs_ppc(green,denominator);
    u32 alphaVariant=!(currentAlpha==one);
    luma->mAlpha=zero;
    luma->mBlue=gabi::fdivs_ppc(f32(mLumaCoefficients.mValue.mBlue),denominator);
    f32 remainingAlpha=gabi::fsubs_ppc(one,currentAlpha);
    TextureImage_l* firstTexture=reinterpret_cast<TextureImage_l*>(entry->mSurfaces[0]);
    TextureImage_l* secondTexture=reinterpret_cast<TextureImage_l*>(entry->mSurfaces[1]);
    SurfaceImagePrefix_l* outputImage=static_cast<SurfaceImagePrefix_l*>(output);
    if (!matchingBlurDescriptor(firstTexture,outputImage)) copyBlurDescriptor(firstTexture,outputImage);
    else copyBlurStorage(firstTexture,outputImage);
    if (!matchingBlurDescriptor(secondTexture,lumaStorage)) copyBlurDescriptor(secondTexture,lumaStorage);
    else copyBlurStorage(secondTexture,lumaStorage);
    secondTexture->mSecondaryFlag=1;
    firstTexture->mSecondaryFlag=1;
    SurfaceImagePrefix_l* target=reinterpret_cast<SurfaceImagePrefix_l*>(entry->mSecondSurfaceState);
    bindBlurTarget(target,lumaStorage);
    BlurViewport_l* viewport=reinterpret_cast<BlurViewport_l*>(entry->mSurfaceState);
    resetFilterViewport(viewport);
    viewport->mLastWord=0;
    gabi::FrameLocal<Color4> bounds(0x20);
    width=dimensions->mFirst;
    height=dimensions->mSecond;
    setBloomViewport(viewport,bounds.get(),width,height,zero);
    viewport->mTarget=target;
    activateFilterFrame(reinterpret_cast<sead::DisplayFrame*>(viewport));
    flushBlurSurface(target);
    gabi::FrameLocal<ViewportRecord_l> viewportRecord(0x194);
    ViewportRecord_l* prepared=constructBlurViewport(viewportRecord.get(),viewport);
    applyBlurViewport(prepared,viewport);
    u32 fetchVariant=s32(mFetchQuality.mValue)>=5;
    ColorProgramState_l* program=reinterpret_cast<ColorProgramState_l*>(mLumaPrograms[fetchVariant].get());
    previousMode=activateEffectProgram(program,previousMode);
    uploadFilterCommon(program,steps.get(),offsets.get(),luma.get(),thresholds.get(),spans.get());
    bindFilterTexture(firstTexture,program->mTextures,-1,0);
    finishBlurSurface(firstTexture);
    drawFilterQuad();
    flushBlurSurface(target);
    if (u8(mRuntimeFlag)!=0) bindFilterViewport(renderSurface,0xFF);
    else activateFilterFrame(reinterpret_cast<sead::DisplayFrame*>(renderSurface));
    viewContext->apply(renderSurface);
    if (reproject) {
        u32 selectedDepth=depthKind<2?depthKind:0;
        u32 selectedFetch=u32(mFetchQuality.mValue);
        if (selectedFetch>=7) selectedFetch=0;
        u32 selectedDebug=mRuntimeWord;
        if (selectedDebug>=3) selectedDebug=0;
        program=reinterpret_cast<ColorProgramState_l*>(mDepthPrograms[selectedDepth][alphaVariant][selectedFetch][selectedDebug].get());
        f32 movement=mReprojectionMoveLimit.mValue;
        f32 distanceScale=gabi::fdivs_ppc(sFilterReprojectionScale,movement);
        SurfaceImagePrefix_l* history=static_cast<SurfaceImagePrefix_l*>(previousDepth);
        u32 historyWidth=history->mGraphics.mWidth;
        u32 minimumHistoryWidth=history->mMinimumWidth;
        u32 historyHeight=history->mGraphics.mHeight;
        u32 minimumHistoryHeight=history->mMinimumHeight;
        if (s32(historyWidth)<=s32(minimumHistoryWidth)) historyWidth=minimumHistoryWidth;
        if (s32(historyHeight)<=s32(minimumHistoryHeight)) historyHeight=minimumHistoryHeight;
        gabi::FrameLocal<Color4> historySize(0x98);
        historySize->mRed=f32(historyWidth);
        historySize->mGreen=f32(historyHeight);
        historySize->mBlue=gabi::fmuls_ppc(f32(historyWidth),distanceScale);
        historySize->mAlpha=gabi::fmuls_ppc(f32(historyHeight),distanceScale);
        gabi::FrameLocal<FilterMatrix44_l> matrix(0xD4), derived(0x1AC), transform(0x154), product(0x1EC), reprojection(0x114);
        constructFilterMatrix(matrix.get(),entry->mFirstAttribute,entry->mFirstIndices);
        derived->invert(matrix.get());
        initializeFilterMatrix(transform.get(),half,sFilterNegativeHalf,one);
        gabi::FrameLocal<FilterVector3_l> position(0xA8);
        position->x=half; position->y=half; position->z=zero;
        applyFilterMatrixVector(transform.get(),position.get());
        constructFilterMatrix(matrix.get(),entry->mSecondAttribute,entry->mSecondIndices);
        multiplyFilterMatrices(transform.get(),matrix.get(),product.get());
        multiplyFilterMatrices(product.get(),derived.get(),reprojection.get());
        previousMode=activateEffectProgram(program,previousMode);
        uploadFilterCommon(program,steps.get(),offsets.get(),luma.get(),thresholds.get(),spans.get());
        gabi::FrameLocal<be<f32>> opacity(0x80);
        *opacity.get()=remainingAlpha;
        uploadFilterUniform(program,6,opacity.get());
        f32 modeValue=one;
        if (u8(mRuntimeFlag)!=0) modeValue=sFilterAlternateMode;
        gabi::FrameLocal<be<f32>> mode(0x84);
        *mode.get()=modeValue;
        uploadFilterUniform(program,7,mode.get());
        uploadFilterUniform(program,8,reprojection.get(),16);
        uploadFilterUniform(program,0,historySize.get());
        gabi::FrameLocal<ColorWords_l> firstScratch(0xB4), secondScratch(0xC4);
        uploadFilterUniform(program,9,firstScratch.get());
        uploadFilterUniform(program,10,secondScratch.get());
        bindFilterTexture(firstTexture,program->mTextures,-1,0);
        bindFilterTexture(secondTexture,filterTextureBinding(program,1),-1,0);
        bindFilterTexture(history,filterTextureBinding(program,4),-1,0);
        bindFilterTexture(static_cast<SurfaceImagePrefix_l*>(depth),filterTextureBinding(program,3),-1,0);
    } else {
        u32 selectedFetch=u32(mFetchQuality.mValue);
        if (selectedFetch>=7) selectedFetch=0;
        u32 selectedDebug=mRuntimeWord;
        if (selectedDebug>=3) selectedDebug=0;
        program=reinterpret_cast<ColorProgramState_l*>(mPrograms[alphaVariant][selectedFetch][selectedDebug].get());
        previousMode=activateEffectProgram(program,previousMode);
        uploadFilterCommon(program,steps.get(),offsets.get(),luma.get(),thresholds.get(),spans.get());
        gabi::FrameLocal<be<f32>> opacity(0x88);
        *opacity.get()=remainingAlpha;
        uploadFilterUniform(program,6,opacity.get());
        f32 modeValue=one;
        if (u8(mRuntimeFlag)!=0) modeValue=sFilterAlternateMode;
        gabi::FrameLocal<be<f32>> mode(0x8C);
        *mode.get()=modeValue;
        uploadFilterUniform(program,7,mode.get());
        bindFilterTexture(firstTexture,program->mTextures,-1,0);
        bindFilterTexture(secondTexture,filterTextureBinding(program,1),-1,0);
    }
    finishBlurSurface(secondTexture);
    drawFilterQuad();
    flushBlurSurface(target);
    name->mMethods=&sFilterBaseTextMethods;
    name->mText=reinterpret_cast<char*>(&sFilterRenderLumaName);
    releaseFilterTarget(sFilterTargetPool,lumaStorage);
    std::atomic_thread_fence(std::memory_order_acquire);
    return previousMode;
}
/* 02777D3C */
u32 renderBloomEffect(EffectRuntimePrefix_l* effect,u32 surfaceIndex,void* rawContext,u32 flags,u32 fallback) {
    gabi::AutoFrame frame{0x02777D3Cu};
    gabi::FrameLocal<be<u8>> savedFlags(0xC),depthKind(0xD);
    *savedFlags.get()=u8(flags);
    BloomEffectControls_l* controls=reinterpret_cast<BloomEffectControls_l*>(effect);
    u32 parameterIndex=surfaceIndex & u32(effect->mParameterIndexMask);
    gptr<ParameterBlock_l>* slot=effect->mSecondStorage;
    if (parameterIndex<u32(effect->mSecondCount)) slot=&sead::elementAt(slot,parameterIndex);
    BloomCoreParameters_l* parameters=reinterpret_cast<BloomCoreParameters_l*>((*slot).get());
    auto finish=[&](u32 value) { std::atomic_thread_fence(std::memory_order_acquire); return value; };
    if (u8(controls->mEnabled)==0) return finish(fallback);
    u32 enabled=effect->mBaseWord;
    if (surfaceIndex==0xFFFFFFFFu) { if (enabled==0) return finish(fallback); }
    else if ((enabled & ((surfaceIndex & 32)?0u:1u<<(surfaceIndex & 31)))==0) return finish(fallback);
    SurfaceBatchEntry_l* entry=effect->mSurfaces;
    if (surfaceIndex<u32(effect->mSurfaceCount)) entry=&sead::elementAt(entry,surfaceIndex);
    if (prepareBloomContext(entry->mReleaseContext)==0) return finish(fallback);
    BloomRenderContext_l* context=static_cast<BloomRenderContext_l*>(rawContext);
    gabi::FrameLocal<SliceValues_l> dimensions(0x64);
    context->mView->mapExtent(reinterpret_cast<sead::RenderVector2*>(dimensions.get()),context->mOwner.get());
    TextureImage_l* mainTexture=reinterpret_cast<TextureImage_l*>(entry->mSurfaces[0]);
    if (f32(dimensions->mFirst)<f32(sBloomMinimumDimension) || f32(dimensions->mSecond)<f32(sBloomMinimumDimension)) {
        copyBloomInput(mainTexture,sBloomFallbackOwner->mColor);
        return finish(fallback);
    }
    BloomEntryTail_l* tail=reinterpret_cast<BloomEntryTail_l*>(entry->mUnknownTail);
    f32 scale=gabi::fmuls_ppc(f32(sBloomRenderScale),f32(tail->mRenderScale));
    SurfaceImagePrefix_l* color=context->mColor;
    if (color==nullptr) color=context->mOwner->mColor;
    copyBloomInput(reinterpret_cast<SurfaceImagePrefix_l*>(entry->mSurfaces[2]),color);
    SurfaceImagePrefix_l* depth=context->mDepth;
    *depthKind.get()=context->mDepthKind;
    if (depth!=nullptr && (u16(controls->mFlags) & 4)==0) {
        copyBloomInput(reinterpret_cast<SurfaceImagePrefix_l*>(entry->mSurfaces[1]),depth);
    } else {
        depth=context->mOwner->mDepth;
        if (depth!=nullptr) {
            copyBloomInput(reinterpret_cast<SurfaceImagePrefix_l*>(entry->mSurfaces[1]),depth);
            *depthKind.get()=0;
        }
    }
    if (context->mExtra!=nullptr) copyBloomInput(reinterpret_cast<SurfaceImagePrefix_l*>(entry->mSurfaces[3]),context->mExtra);
    u32 width=blurDimension(gabi::fmuls_ppc(f32(dimensions->mFirst),scale));
    u32 height=blurDimension(gabi::fmuls_ppc(f32(dimensions->mSecond),scale));
    f32 zero=sBlurZero;
    BlurViewport_l* viewport=reinterpret_cast<BlurViewport_l*>(entry->mSurfaceState);
    BloomViewportTargets_l* targets=reinterpret_cast<BloomViewportTargets_l*>(viewport);
    gabi::FrameLocal<Text> name(0x30);
    for (u32 index=0;index<2;++index) {
        SurfacePair* pair=&entry->mPairs[index];
        name->mMethods=&sBlurTextMethods;
        name->mText=reinterpret_cast<char*>(index==0?&sBloomDetectFirstName:&sBloomDetectSecondName);
        SurfaceImagePrefix_l* storage=allocateBlurSurface(entry->mReleaseContext,name.get(),26,width,height,4,0,0,1);
        pair->mHeader=storage;
        SurfaceImagePrefix_l* target=reinterpret_cast<SurfaceImagePrefix_l*>(pair->mSecondSurface);
        if (u32(target->mMipIndex)!=0) { target->mMipDirty=1; target->mMipIndex=0; }
        bindBlurTarget(target,storage);
        TextureImage_l* texture=reinterpret_cast<TextureImage_l*>(pair->mSurface);
        texture->mLodMinimum=zero; texture->mLodMaximum=zero; texture->mLodBias=zero;
        texture->mFlags=u8(texture->mFlags) | 8;
        copyBloomInput(texture,target);
        targets->mTargets[index]=target;
    }
    bindFilterAttributes(sFilterAttributeOwner->mBindingArgument);
    fallback=detectBloomInput(effect,surfaceIndex,context->mColor!=nullptr,*depthKind.get(),fallback);
    targets->mTargets[1]=nullptr;
    f32 one=sBlurOne;
    if ((u16(controls->mFlags) & 0x80)==0) {
        for (u32 level=0;level<3;++level) fallback=effect->drawMipBlur(surfaceIndex,level,fallback,gabi::fpr_from_f32(one));
    }
    SurfacePair* firstPair=&entry->mPairs[0];
    TextureImage_l* texture=reinterpret_cast<TextureImage_l*>(firstPair->mSurface);
    SurfaceImagePrefix_l* target=reinterpret_cast<SurfaceImagePrefix_l*>(firstPair->mSecondSurface);
    gabi::FrameLocal<Color4> firstBounds(0x20),secondBounds(0x48),multiplied(0x6C);
    gabi::FrameLocal<ViewportRecord_l> firstViewport(0x7C),secondViewport(0x100);
    gabi::FrameLocal<BloomColors_l> colors(0x94);
    gabi::FrameLocal<BloomWeightValues_l> weights(0x58);
    if ((u16(controls->mFlags) & 0x100)==0) {
        constructPairArray(colors.get(),3,16,reinterpret_cast<PairConstructCallback>(&Color4::construct));
        u32 mode=controls->mColorMode;
        if (mode==0) {
            for (u32 index=0;index<3;++index) colors->mColors[index]=parameters->mColors[index].mColor;
        } else if (mode==1) {
            weights->mValues[0]=0.0f;
            weights->mValues[1]=0.0f;
            weights->mValues[2]=0.0f;
            if (sBloomMixInitialized==0) { sBloomMixInitialized=1; sBloomMixHalf=sBloomHalf; }
            f32 factor=parameters->mMix;
            f32 doubled=gabi::fadds_ppc(factor,factor);
            if (!(factor<f32(sBloomMixHalf))) {
                doubled=gabi::fsubs_ppc(doubled,one);
                f32 denominator=gabi::fadds_ppc(gabi::fadds_ppc(one,one),doubled);
                f32 inverse=gabi::fdivs_ppc(one,denominator);
                weights->mValues[0]=inverse;
                weights->mValues[1]=gabi::fmuls_ppc(one,inverse);
                weights->mValues[2]=gabi::fmuls_ppc(doubled,inverse);
            } else {
                f32 inverse=gabi::fdivs_ppc(one,gabi::fadds_ppc(gabi::fadds_ppc(one,doubled),zero));
                weights->mValues[0]=inverse;
                weights->mValues[1]=gabi::fmuls_ppc(doubled,inverse);
                weights->mValues[2]=gabi::fmuls_ppc(zero,inverse);
            }
            for (u32 index=0;index<3;++index) {
                Color4* value=&colors->mColors[index];
                value->mRed=weights->mValues[index]; value->mGreen=weights->mValues[index];
                value->mBlue=weights->mValues[index]; value->mAlpha=one;
            }
        }
        for (u32 level=2;level!=0;--level) {
            u32 widthWord=u32(target->mGraphics.mWidth)>>level,heightWord=u32(target->mGraphics.mHeight)>>level;
            u32 minWidth=target->mMinimumWidth,minHeight=target->mMinimumHeight;
            if (s32(widthWord)<=s32(minWidth)) widthWord=minWidth;
            if (s32(heightWord)<=s32(minHeight)) heightWord=minHeight;
            f32 levelWidth=f32(widthWord),levelHeight=f32(heightWord);
            if (u32(target->mMipIndex)!=level) { target->mMipIndex=level; target->mMipDirty=1; }
            setBloomViewport(viewport,firstBounds.get(),levelWidth,levelHeight,zero);
            activateFilterFrame(reinterpret_cast<sead::DisplayFrame*>(viewport));
            constructBlurViewport(firstViewport.get(),viewport);
            applyBlurViewport(firstViewport.get(),viewport);
            setBlurLod(texture,gabi::fadds_ppc(f32(s32(level)),one),zero);
            if (level==2) fallback=effect->drawColor(texture,&colors->mColors[2],&colors->mColors[1],fallback);
            else {
                multiplyBloomColor(multiplied.get(),&parameters->mBlend,&colors->mColors[0]);
                fallback=effect->drawColor(texture,&parameters->mBlend,multiplied.get(),fallback);
            }
            flushBlurSurface(target); finishBlurSurface(target);
        }
    }
    gabi::FrameLocal<BloomPairCursor_l> cursor(0xF4);
    cursor->mBegin=firstPair; cursor->mEnd=firstPair+2;
    for (SurfacePair* pair=firstPair;pair!=firstPair+2;++pair) {
        setBlurLod(reinterpret_cast<TextureImage_l*>(pair->mSurface),zero,zero);
        SurfaceImagePrefix_l* pairTarget=reinterpret_cast<SurfaceImagePrefix_l*>(pair->mSecondSurface);
        if (u32(pairTarget->mMipIndex)!=0) { pairTarget->mMipIndex=0; pairTarget->mMipDirty=1; }
    }
    fallback=effect->drawBloomLevels(surfaceIndex,fallback);
    releaseSurfaceHandle(entry->mReleaseContext,entry->mPairs[1].mHeader.v);
    entry->mPairs[1].mHeader.v=0;
    SurfaceImagePrefix_l* firstStorage=firstPair->mHeader.get();
    copyBloomInput(mainTexture,firstStorage);
    setBlurLod(mainTexture,one,zero);
    SurfaceImagePrefix_l* extra=tail->mExtraTarget;
    gabi::FrameLocal<ColorRenderState_l> state(0x7C);
    if (extra!=nullptr) {
        u32 widthWord=extra->mGraphics.mWidth,heightWord=extra->mGraphics.mHeight;
        u32 minWidth=extra->mMinimumWidth,minHeight=extra->mMinimumHeight;
        if (s32(widthWord)<=s32(minWidth)) widthWord=minWidth;
        if (s32(heightWord)<=s32(minHeight)) heightWord=minHeight;
        bindBlurTarget(target,extra);
        viewport->mTarget=target;
        setBloomViewport(viewport,secondBounds.get(),f32(widthWord),f32(heightWord),zero);
        activateFilterFrame(reinterpret_cast<sead::DisplayFrame*>(viewport));
        constructBlurViewport(secondViewport.get(),viewport);
        applyBlurViewport(secondViewport.get(),viewport);
        sead::CompactRenderState::create(state.get());
        state->mDepthTest=0; state->mDepthWrite=0; state->mBlendEnable=1;
        state->mBlend.mColorSource=1; state->mBlend.mAlphaSource=1; state->mBlend.mColorDestination=1; state->mBlend.mAlphaDestination=1;
        state->apply();
        ColorProgramState_l* program=bloomCopyProgram();
        u32 selected=activateEffectProgram(program,fallback);
        fallback=selected;
        drawRuntimeProgram(effect,program,entry);
        copyBloomInput(mainTexture,tail->mExtraTarget);
        setBlurLod(mainTexture,zero,zero);
    }
    activateFilterFrame(reinterpret_cast<sead::DisplayFrame*>(context->mOwner.get()));
    context->mView->apply(context->mOwner.get());
    if (u8(*savedFlags.get())==0 && (u16(controls->mFlags) & 1)==0) {
        ColorProgramState_l* program=bloomCopyProgram();
        sead::CompactRenderState::create(state.get());
        state->mWriteRed=1; state->mWriteGreen=1;
        state->mWriteBlue=1; state->mWriteAlpha=0;
        state->mDepthTest=0; state->mDepthWrite=0;
        u32 blendMode=parameters->mBlendMode;
        state->mBlendEnable=0;
        if ((u16(controls->mFlags) & 8)==0 && (blendMode==1 || blendMode==2)) {
            state->mBlendEnable=1;
            state->mBlend.mColorSource=blendMode==1?1:9; state->mBlend.mAlphaSource=blendMode==1?1:9;
            state->mBlend.mColorDestination=1; state->mBlend.mAlphaDestination=1;
        }
        state->apply();
        u32 selected=activateEffectProgram(program,fallback);
        fallback=selected;
        drawRuntimeProgram(effect,program,entry);
    }
    for (u32 slice=0;slice<3;++slice) {
        if ((u16(controls->mSliceFlags) & (0x10u<<slice))!=0)
            fallback=effect->applySliceColor(surfaceIndex,slice,context->mOwner,fallback);
    }
    if (u8(*savedFlags.get())==0) effect->releasePairHandles(surfaceIndex);
    finishBloomContext(entry->mReleaseContext);
    return finish(fallback);
}
/* WWHD glue
 data sFilterRenderZero 10161F10
 data sFilterRenderUnsignedLimit 1016252C
 data sFilterRenderHalf 10162530
 data sFilterThresholdSmall 10162534
 data sFilterThresholdLarge 10162538
 data sFilterThresholdMediumSmall 1016253C
 data sFilterThresholdMediumLarge 10162540
 data sFilterThresholdFine 10162544
 data sFilterThresholdMultiplier 10162548
 data sFilterReprojectionScale 1016254C
 data sFilterNegativeHalf 10162558
 data sFilterAlternateMode 1016255C
 data sFilterTargetPool 101F8DD4
 data sFilterAttributeOwner 101F8DDC
 data sFilterGeometryOwner 101F8DE4
 data sFilterCopyFirstSettings 104A0920
 data sFilterCopySecondSettings 104A0908
 data sFilterRenderLumaName 10162560
 ext 027A90AC u32 copyFilterOutput(void*,void*,const void*,const void*,u32)
 ext 0274F8D8 void sead::Viewport::mapExtent(sead::RenderVector2*,const sead::RenderSurface*) const
 ext 0279FC6C SurfaceImagePrefix_l* allocateFilterTarget(void*,const agl::filter_aa_text::Text*,u32,u32,u32,u32,u32,u32,u32,u32)
 ext 027A0040 void releaseFilterTarget(void*,SurfaceImagePrefix_l*)
 ext 027B954C void resetFilterViewport(BlurViewport_l*)
 ext 027B9D00 void bindFilterViewport(void*,u32)
 ext 027BE53C void bindFilterTexture(SurfaceImagePrefix_l*,const TextureBinding_l*,s32,u32)
 ext 027BFE5C void bindFilterAttributes(const void*)
 ext C0006178 void drawFilterIndices(u32,u32,u32,const void*,u32,u32)
 ext 0272D680 void constructFilterMatrix(FilterMatrix44_l*,const void*,const void*)
 ext 0274EDD4 void sead::RenderMatrix44::invert(const sead::RenderMatrix44*)
 ext 028E8B04 void initializeFilterMatrix(FilterMatrix44_l*,f32,f32,f32)
 ext 0277C888 void applyFilterMatrixVector(FilterMatrix44_l*,const FilterVector3_l*)
 ext 028E89B4 void multiplyFilterMatrices(const FilterMatrix44_l*,const FilterMatrix44_l*,FilterMatrix44_l*)
 ext 0279ABB4 s32 prepareBloomContext(void*)
 ext 02775C3C u32 detectBloomInput(agl::filter_aa::EffectRuntimePrefix_l*,u32,u32,u32,u32)
 ext 0279AE88 void finishBloomContext(void*)
 ext 0274D3EC void multiplyFilterColorChannels(sead::ColorChannels*,const sead::ColorChannels*,const sead::ColorChannels*)
 data sBloomFallbackOwner 101F8DEC
 data sBloomMinimumDimension 10161E3C
 data sBloomRenderScale 10161E40
 data sBloomDetectFirstName 10161E48
 data sBloomDetectSecondName 10161E50
 data sBloomMixInitialized 104A1F78
 data sBloomMixHalf 104A1F7C
 data sBloomHalf 10161E1C
 data sBloomFirstTargetName 10161E20
 data sBloomSecondTargetName 10161E30
 data sBlurZero 1016180C
 data sBlurOne 10161810
 data sBlurUnsignedLimit 10161E10
 data sBlurName 10161E14
 data sBlurTextMethods 10161818
 ext 0274D690 void activateFilterFrame(sead::DisplayFrame*)
 ext 0274F5F0 sead::Viewport* sead::Viewport::createSurface(sead::Viewport*,const sead::RenderVector2*)
 ext 0274F964 void sead::Viewport::apply(const sead::RenderSurface*) const
 ext 027B6D64 void flushBlurSurface(SurfaceImagePrefix_l*)
 ext 027BE794 void finishBlurSurface(SurfaceImagePrefix_l*)
 ext 027BDEB4 void copyBlurDescriptor(SurfaceImagePrefix_l*,const SurfaceImagePrefix_l*)
 ext 0279ADD4 SurfaceImagePrefix_l* allocateBlurSurface(void*,const agl::filter_aa_text::Text*,u32,u32,u32,u32,u32,u32,u32)
 ext 025C0928 void bindBlurTarget(SurfaceImagePrefix_l*,SurfaceImagePrefix_l*)
 ext 025C0848 void copyBlurLevel(SurfaceImagePrefix_l*,TextureImage_l*,u32,u32)
 data sFilterBaseName 101621A4
 data sFilterContainerName 101622AC
 data sFilterIcon 101622B8
 data sFilterBaseTextMethods 10161F1C
 data sFilterEmptyType 10161F19
 data sFilterEmptyEnable 10161F18
 data sFilterEmptyColor 10161F1A
 data sFilterIntegerMethods 1016202C
 data sFilterByteMethods 10161FB4
 data sFilterScalarMethods 101620A4
 data sFilterColorMethods 1016211C
 data sFilterTypeName 101622CC
 data sFilterTypeDescription 101621AC
 data sFilterEnableName 101621B4
 data sFilterEnableDescription 101621BC
 data sFilterAlphaOutputName 101622DC
 data sFilterAlphaOutputDescription 101622EC
 data sFilterAlphaOutputRange 101621FC
 data sFilterEdgeQualityName 10162220
 data sFilterEdgeQualityDescription 101622F8
 data sFilterFetchQualityName 10162310
 data sFilterFetchQualityDescription 10162320
 data sFilterEdgeThresholdScaleName 10162234
 data sFilterEdgeThresholdScaleDescription 10162340
 data sFilterEdgeThresholdScaleRange 10162354
 data sFilterLumaName 101621C4
 data sFilterLumaDescription 10162368
 data sFilterSubpixelName 1016237C
 data sFilterSubpixelDescription 1016238C
 data sFilterSubpixelRange 101621D4
 data sFilterMaximumSpanName 101623AC
 data sFilterMaximumSpanDescription 101623B8
 data sFilterMaximumSpanRange 101623C8
 data sFilterSpanMultiplyName 10162250
 data sFilterSpanMultiplyDescription 101623E0
 data sFilterSpanMultiplyRange 101623F0
 data sFilterMinimumSpanName 10162408
 data sFilterMinimumSpanDescription 10162260
 data sFilterMinimumSpanRange 10162418
 data sFilterReprojectionName 10162284
 data sFilterReprojectionDescription 10162430
 data sFilterMoveLimitName 10162444
 data sFilterMoveLimitDescription 10162464
 data sFilterMoveLimitRange 10162298
 data sFilterDefaultAlpha 10161F14
 data sFilterDefaultSubpixel 10162194
 data sFilterDefaultMaximumSpan 10162198
 data sFilterDefaultMinimumSpan 1016219C
 data sFilterDefaultMoveLimit 101621A0
 data sFilterLumaDefault 104A1F80
 ext 0279A07C void constructFilterBase(agl::filter_aa::FilterAaObject_l*,const agl::filter_aa_text::Text*,u32)
 ext 027A84B8 void constructFilterParameterContainer(agl::filter_aa::FilterParameterContainer_l*)
 ext 0279E618 void bindFilterParameter(agl::filter_aa::ParameterBase_l*,const agl::filter_aa_text::Text*,const agl::filter_aa_text::Text*,const agl::filter_aa_text::Text*,agl::filter_aa::FilterParameterContainer_l*)
 ext 027A03A0 void constructFilterControl(agl::filter_aa::ControlPrefix_l*)
 ext 0279CB9C void attachFilterParameters(agl::filter_aa::FilterAaObject_l*,agl::filter_aa::FilterParameterContainer_l*,const agl::filter_aa_text::Text*)
 ext 02785CB4 void setFilterIcon(void*,const agl::filter_aa_text::Text*)
 ext 0279A86C void initializeRuntimeBase(agl::filter_aa::EffectRuntimePrefix_l*,s32,u32,void*)
 ext 02756140 sead::RegistryHeap* registryCurrentHeap(void*)
 ext 027B95B4 void initializeSurfaceState(void*)
 ext 0279A95C void initializeSurfaceReleaseContext(void*)
 ext 028EFFD0 void constructPairArray(void*,u32,u32,PairConstructCallback)
 ext 02774FD0 void initializeRuntimeSurface(agl::filter_aa::EffectRuntimePrefix_l*,agl::filter_aa::SurfaceBatchEntry_l*,void*)
 ext 02775090 void initializeRuntimeValues(agl::filter_aa::ParameterBlock_l*,agl::filter_aa::FilterParameterContainer_l*,void*)
 ext 027BB9E0 void updateColorProgram(ColorProgramState_l*,u32)
 ext 027B9F68 void applyShaderMode(u32)
 ext 027BB7CC void prepareColorProgram(ColorProgramState_l*)
 ext C00060E0 void callColorDisplayList(const void*,u32)
 ext C0006A40 void setColorVertexUniform(s32,u32,const void*)
 ext C0006908 void setColorPixelUniform(s32,u32,const void*)
 ext 0274FDBC sead::CompactRenderState* sead::CompactRenderState::create(sead::CompactRenderState*)
 ext 0274FEB0 void sead::CompactRenderState::apply()
 ext 02775BDC void drawRuntimeProgram(agl::filter_aa::EffectRuntimePrefix_l*,ColorProgramState_l*,void*)
 data sShaderArchiveRegistry 101F8D6C
 data sFilterQualityValues 10162494
 data sFilterDepthQualityValues 101624CC
 data sFilterFetchName 101624E8
 data sFilterAlphaName 101624F4
 data sFilterDepthName 101624B0
 data sFilterDebugName 10162508
 data sFilterZeroToken 10162490
 data sFilterFiveToken 10162492
 data sFilterTextMethods 10161F1C
 data sFilterName 10162514
 ext 028EFFD0 void constructFilterSurfaceArray(void*,u32,u32,SurfaceConstructCallback)
 ext 027BE6B8 void constructFilterTextureData(void*)
 ext 0273B0D4 void* allocateFilterEntryStorage(u32,void*,u32)
 ext 028E892C void initializeFilterAttribute(void*)
 ext 028E9098 void initializeFilterIndices(void*)
 ext 027BBD04 s32 findShaderVariant(agl::filter_aa::ShaderVariantList_l*,u32,const be<u32>*,const be<u32>*,s32)
 ext 027A063C void noOpFilterAaControlHook(agl::filter_aa::ControlPrefix_l*,s32,const agl::filter_aa_text::Text*,void*)
 data sFilterPrimaryMethods 1016257C
 data sFilterSecondaryMethods 101625E4
 data sFilterControlMethods 10161F54
 data sFilterControlPrimaryMethods 10161F94
 data sFilterControlSecondaryMethods 10161FA4
 ext 028F0164 void destroyFilterSurfaceArray(void*,u32,u32,SurfaceDestroyCallback,u32,u32)
 ext 0273AFC8 void releaseFilterEntryStorage(void*)
 data sSliceColorFirst 104A01CC
 data sSliceColorSecond 104A01EC
 ext 027A9E28 u32 submitSliceColor(void*,u32,const SliceValues_l*,const ColorWords_l*,u32,f32,f32)
 data sTextBufferMethods 10161830
 data sTextBaseMethods 10161868
 data sFormattedTextMethods 101618D0
 ext 02759C10 void formatTextArguments(agl::filter_aa_text::FormattedText128*,const char*,VariadicArguments_l*)
 data sRuntimeMethods 101618B8
 ext 02755FEC sead::RegistryHeap* findRegistryHeap(void*,void*)
 ext 027B9694 void destroySurfaceState(void*,u32)
 ext 0279AA14 void destroySurfaceReleaseContext(void*,u32)
 ext 0279A7D8 void destroyRuntimeBase(agl::filter_aa::EffectRuntimePrefix_l*,u32)
 ext 028F0164 void destroySurfaceArray(void*,u32,u32,PairDestroyCallback,u32,u32)
 ext 0279AE68 void releaseSurfaceHandle(void*,u32)
 data sControlPrimaryMethods 10161898
 data sControlSecondaryMethods 101618A8
 data sColorParameterMethods 10161A98
 data sColorZero 10145180
 data sColorOne 1014517C
 data sIntegerParameterMethods 10161A20
 data sByteParameterMethods 10161930
 ext 027BDF7C void initializeSurface(void*) callback
 ext 027B6CB8 void initializeSecondSurface(void*)
 ext 0279A730 void initializeEffectRuntime(agl::filter_aa::EffectRuntimePrefix_l*)
 ext 02774D44 void initializeEffectParameterBlock(void*)
 ext 0279E674 void initializeEffectParameter(agl::filter_aa::ParameterBase_l*)
 ext 027A0998 void releaseEffectControl(agl::filter_aa::ControlPrefix_l*)
 ext 02752BEC void destroyEffectControlBase(agl::filter_aa::ControlPrefix_l*,u32)
 ext 027BE2B0 void destroyEffectSurface(agl::filter_aa::SurfacePrefix_l*,u32) callback
*/
// 027A063C returns immediately without reading or writing arguments or state.
