// agl depth of field, draw half: the draw entry points, the compose / vignetting / reduced passes with their
// uniform blocks, buffer release, the vignetting-shape edits and the indirect texture-matrix helpers.
// Written from WWHD's instructions; GHS inlined agl::ShaderProgram::activate and the uniform setter into
// every pass, which stay helpers here.
#include "bindings.h"
#include "agl/DepthOfFieldDraw.h"

using namespace agl::dof;


static void updateProgram(agl::ShaderProgram* program, u32 flags);
static void setShaderMode(u32 mode);
static void bindProgram(agl::ShaderProgram* program);
static void GX2CallDisplayList(void* list, u32 size);
static void GX2SetVertexUniformReg(s32 location, u32 count, const void* values);
static void GX2SetPixelUniformReg(s32 location, u32 count, const void* values);
static void activateSampler(agl::TextureSampler* sampler, agl::ProgramResource16_l* location, s32 vertexLocation, u32 geometryLocation);
static void applyTargetTexture(agl::ColorSurface* target, agl::TextureData* texture, u32 mipLevel, u32 slice);
static void copySamplerTexture(agl::TextureSampler* sampler, const agl::TextureSampler* source);
static void invalidateColorTarget(agl::ColorSurface* target);
static void invalidateTargetTexture(agl::ColorSurface* target);
static void bindRenderSurface(agl::RenderBuffer* renderBuffer);
static void freeTexture(agl::DynamicTextureCache* allocator, agl::TextureCacheEntry* texture);
// AGL#4's half of this translation unit (agl/DepthBlurRuntimeValues.cpp, DepthBlurContext.cpp,
// DepthBlurCallbacks.cpp, DepthBlurDrawState.cpp, DepthBlurValues.cpp), bound by address.
static void constructDepthBlurContext(DepthBlurContext* context, s32 view, ContextTargets* targets,
    Parameters* parameters, agl::RenderBuffer* output, const agl::TextureData* secondary, u32 flags, f32 nearDepth,
    f32 farDepth);
static void prepareDepthBlurTexture(DepthOfField* runtime, s32 view, agl::RenderBuffer* buffer);
static u32 drawDepthBlurMipChain(DepthOfField* runtime, const DepthBlurContext* context, u32 mode);
static u32 drawDepthBlurNearMipChain(DepthOfField* runtime, const DepthBlurContext* context, u32 mode);
static u32 drawDepthBlurFullSurface(DepthOfField* runtime, const DepthBlurContext* context, u32 mode);
static void applyDepthBlurOutputMip(DepthOfField* runtime, agl::RenderBuffer* output, u32 mip, u32 secondaryOffset);
static void drawDepthBlurShape(DepthOfField* runtime, const DepthBlurContext* context);
static void uploadDepthBlurComposite(DepthOfField* runtime, const DepthBlurContext* context, const agl::ShaderProgram* program);
// Parameters::updateShaderPrograms (0277CA98); these callers return the r3 it leaves.
static u32 updateShaderPrograms(Parameters* parameters);
static f64 sine(f64 angle);
static f64 cosine(f64 angle);
static TextureCachePointer sTextureAllocator;
static u8 sDepthTestEnabled;
static u32 sReduceStateMethods;

// agl::ShaderProgram::activate as GHS inlines it: refresh a pending update, switch the shader mode only on a
// change, then replay the display list (or bind directly when there is none). Returns the new mode.
static u32 activateProgram(agl::ShaderProgram* program, u32 currentMode) {
    u8 flags = program->flags;
    if (flags & ShaderProgramFlag_UpdatePending) {
        program->flags = flags & ~ShaderProgramFlag_UpdatePending;
        updateProgram(program, 0);
    }
    u32 mode = programSharedData(program)->mode;
    if (currentMode != mode) setShaderMode(mode);
    u32 size = programDisplayList(program)->mUsed;
    if (size != 0) GX2CallDisplayList(programDisplayList(program)->mData, size);
    else bindProgram(program);
    return mode;
}

// agl::ShaderLocation::setVec4 on both stages.
static void setUniform(const UniformLocations_l* uniform, const void* values) {
    if (uniform->mVertexRegister != -1) GX2SetVertexUniformReg(uniform->mVertexRegister, 4, values);
    if (uniform->mPixelRegister != -1) GX2SetPixelUniformReg(uniform->mPixelRegister, 4, values);
}

// sead::Mathf::sqrt: frsqrte with one Newton step, 0 for inputs that are not positive.
static f32 squareRoot(f32 value) {
    f32 inverse = 0.0f;
    if (value > 0.0f) {
        f64 estimate = gabi::frsqrte(value);
        f32 square = (f32)(estimate * round25(estimate));
        f32 half = (f32)(estimate * 0.5);
        inverse = gabi::fmuls_ppc(gabi::fnmsubs(square, value, 3.0f), half);
    }
    return gabi::fmuls_ppc(inverse, value);
}

// slw: a shift count of 32 to 63 gives 0.
static u32 shiftLeft(u32 value, u32 count) {
    count &= 0x3F;
    return count > 31 ? 0 : value << count;
}

// A pure float copy (lfs/stfs) keeps signalling NaNs bit-exact.
static void copyFloat(be<f32>* destination, const be<f32>* source) {
    reinterpret_cast<be<u32>&>(*destination) = (u32)reinterpret_cast<const be<u32>&>(*source);
}

/* 02781570 */
void DepthOfField::setVignettingUniforms(const DepthBlurContext* arg, const agl::ShaderProgram* program) {
    enum { cUniform_Radius = 8, cUniform_Param = 9, cUniform_Translation = 10, cUniform_Color = 11 };
    s32 pass = arg->mDrawMode;
    Parameters* settings = arg->mParameters;
    u8 composeDepthOfField = 0;
    if (pass == DepthBlurContext::cPass_Compose && settings->usesDepthPass()) composeDepthOfField = 1;
    const VignettingShape* shape = &settings->mShapes[0];
    if (pass == DepthBlurContext::cPass_Vignetting && settings->usesTwoVignettingShapes()) shape = &settings->mShapes[1];

    f32 scaleX = shape->mScale.x, scaleY = shape->mScale.y;
    scaleX = scaleX - 0.001f >= 0.0f ? scaleX : 0.001f;
    scaleY = scaleY - 0.001f >= 0.0f ? scaleY : 0.001f;

    f32 inner = 1.0f - shape->mRange.x;
    if (inner < 0.0f) inner = 0.0f;
    else if (inner > 1.0f) inner = 1.0f;
    f32 shift = fabsf(shape->mTranslation.y) - fabsf(shape->mTranslation.x) >= 0.0f
        ? fabsf(shape->mTranslation.y) : fabsf(shape->mTranslation.x);
    f32 smaller = scaleY - scaleX >= 0.0f ? scaleX : scaleY;
    f32 invScale = 1.0f / smaller;

    gabi::Local<agl::depth_blur_value::Vector4> values;
    values->x = composeDepthOfField ? 0.0f : inner;
    values->y = inner;
    values->z = gabi::fmadds(1.0f - inner, shape->mRange.y, inner);
    values->w = gabi::fmadds(1.1f, invScale, shift);
    setUniform(programUniform(program, cUniform_Radius), values);

    f32 scaleWidth = 1.0f, scaleHeight = 1.0f;
    if (shape->mType.mValue == 0) {
        s32 width = (s32)(u32)arg->mWidth, height = (s32)(u32)arg->mHeight;
        f32 diagonal = squareRoot((f32)(width * width + height * height));
        scaleWidth = diagonal / (f32)(s32)(u32)arg->mWidth;
        scaleHeight = diagonal / (f32)(s32)(u32)arg->mHeight;
    }
    f32 blur = arg->mParameters->mVignettingBlurStrength.mValue;
    scaleWidth = scaleWidth * scaleX;
    scaleHeight = scaleHeight * scaleY;
    if (blur < 0.0f) blur = 0.0f;
    else if (blur > 1.0f) blur = 1.0f;
    values->x = scaleWidth;
    values->y = scaleHeight;
    values->z = blur;
    values->w = 0.0f;
    setUniform(programUniform(program, cUniform_Param), values);

    setUniform(programUniform(program, cUniform_Color), &arg->mParameters->mVignettingColorParameter.mValue.r);

    copyFloat(&values->x, &shape->mTranslation.x);
    copyFloat(&values->y, &shape->mTranslation.y);
    values->z = 0.0f;
    values->w = 0.0f;
    setUniform(programUniform(program, cUniform_Translation), values);
}

/* 02781A58 */
// WWHD's reduced pass: the near/far depth reconstruction terms of the in-focus range, then the reduced
// colour sampler.
void DepthOfField::setReduceUniforms(const DepthBlurContext* arg, const agl::ShaderProgram* program) {
    enum { cUniform_DepthParam = 6 };
    f32 nearZ = arg->mNearDepth;
    Parameters* settings = arg->mParameters;
    f32 farZ = arg->mFarDepth;
    f32 doubleNearSquareFar = (nearZ + nearZ) * nearZ * farZ;
    f32 start = settings->mStart.mValue, end = settings->mEnd.mValue;
    f32 focus = end - start >= 0.0f ? start : end;
    f32 sum = farZ + nearZ, range = farZ - nearZ;
    f32 nearFocus = nearZ * focus;
    f32 nearTerm = gabi::fmsubs(nearFocus, sum, doubleNearSquareFar) / (nearFocus * range);
    f32 farTerm = 0.0f;
    if (settings->mNear.mValue) {
        f32 farStart = settings->mFarStart.mValue, farEnd = settings->mFarEnd.mValue;
        f32 farFocus = nearZ * (farEnd - farStart >= 0.0f ? farEnd : farStart);
        farTerm = gabi::fmsubs(farFocus, sum, doubleNearSquareFar) / (farFocus * range);
    }
    gabi::Local<agl::depth_blur_value::Vector4> values;
    values->x = 0.0f;
    values->y = 0.0f;
    values->z = nearTerm;
    values->w = farTerm;
    setUniform(programUniform(program, cUniform_DepthParam), values);
    activateSampler(&arg->mTargets->mCombined, program->samplers, -1, 0);
}

/* 02781B7C */
u32 DepthOfField::drawCompose(const DepthBlurContext* arg, u32 mode) {
    Parameters* settings = arg->mParameters;
    u8 farOnly = 0;
    if (settings->mFar.mValue && !settings->mNear.mValue && sDepthTestEnabled
        && !settings->mVignettingBlur.mValue)
        farOnly = 1;
    u8 nearOnly = 0;
    if (!settings->mFar.mValue && settings->mNear.mValue && !settings->mDepthBlur.mValue
        && sDepthTestEnabled && !settings->mVignettingBlur.mValue)
        nearOnly = 1;

    gabi::Local<sead::CompactRenderState> state;
    sead::CompactRenderState::create(state);
    state->mDepthWrite = 0;
    if (farOnly) {
        state->mDepthTest = 1;
        state->mDepthComparison = 1;   // less
    } else if (nearOnly) {
        state->mDepthTest = 1;
        state->mDepthComparison = 4;   // greater
    } else {
        state->mDepthTest = 0;
    }
    state->mWriteRed = 1;
    state->mWriteGreen = 1;
    state->mWriteBlue = 1;
    state->mWriteAlpha = 0;

    if (mSettings.mReduceDraw.mValue) {
        // Draw into the half-resolution blur target instead, with the reduced sampler following it.
        ContextTargets* context = arg->mTargets;
        TextureSampler* blur = &context->mBlurTarget;
        context->mMipOutput.mColors[0] = &context->mMipColor;
        ContextTargets* current = arg->mTargets;
        applyTargetTexture(&current->mMipColor, &current->mBlurTarget.texture, 0, 0);
        s32 width = (s32)(u32)blur->texture.surface.mWidth;
        s32 minimumWidth = blur->texture.minimumWidth;
        if (width > minimumWidth) minimumWidth = width;
        u8 fullSize = 0;
        ContextTargets* target = arg->mTargets;
        if ((u32)minimumWidth == arg->mWidth) {
            s32 height = (s32)(u32)blur->texture.surface.mHeight;
            s32 minimumHeight = blur->texture.minimumHeight;
            if (height > minimumHeight) minimumHeight = height;
            if ((u32)minimumHeight == arg->mHeight) fullSize = 1;
        }
        TextureSampler* reduce = &target->mCombined;
        const sead::GraphicsSurface* from = &blur->texture.surface;
        sead::GraphicsSurface* to = &reduce->texture.surface;
        if (to->mDimension == from->mDimension && to->mWidth == (u32)width && to->mHeight == from->mHeight
            && to->mDepth == from->mDepth && to->mMipCount == from->mMipCount && to->mFormat == from->mFormat
            && to->mSwizzle == from->mSwizzle && to->mTileMode == from->mTileMode
            && to->mSamples == from->mSamples) {
            // Same surface: only the image pointers move (texture and its GX2 copy).
            void* image = from->mImage;
            void* mipImage = from->mMipImage;
            sead::GraphicsSurface* native = nativeSurface(reduce);
            native->mImage = image;
            native->mMipImage = mipImage;
            to->mImage = image;
            to->mMipImage = mipImage;
        } else {
            copySamplerTexture(reduce, blur);
        }
        ContextTargets* filtered = arg->mTargets;
        u8 dirty = filtered->mCombined.dirty;
        filtered->mCombined.settings.modes[SamplerMode_MagnificationFilter] = 1;
        filtered->mCombined.settings.modes[SamplerMode_MinificationFilter] = 1;
        filtered->mCombined.settings.modes[SamplerMode_MipFilter] = 1;
        filtered->mCombined.dirty = dirty | SamplerDirty_Filters;
        ContextTargets* lod = arg->mTargets;
        lod->mCombined.dirty = lod->mCombined.dirty | SamplerDirty_Lod;
        f32 level = (f32)(f64)(s32)fullSize;
        lod->mCombined.settings.scalarParameters[SamplerScalar_MinimumLod] = level;
        lod->mCombined.settings.scalarParameters[SamplerScalar_MaximumLod] = level;
        lod->mCombined.settings.scalarParameters[SamplerScalar_LodBias] = 0.0f;
        applyDepthBlurOutputMip(this, &arg->mTargets->mMipOutput, fullSize, 0);

        gabi::Local<sead::CompactRenderState> reduceState;
        reduceState->mDepthComparison = (u32)state->mDepthComparison;
        reduceState->mCullMode = (u32)state->mCullMode;
        reduceState->mBlend.mColorSource = (u32)state->mBlend.mColorSource;
        reduceState->mBlend.mAlphaSource = (u32)state->mBlend.mAlphaSource;
        reduceState->mBlend.mColorDestination = (u32)state->mBlend.mColorDestination;
        reduceState->mBlend.mAlphaDestination = (u32)state->mBlend.mAlphaDestination;
        reduceState->mBlend.mColorEquation = (u32)state->mBlend.mColorEquation;
        reduceState->mBlend.mAlphaEquation = (u32)state->mBlend.mAlphaEquation;
        copyFloat(&reduceState->mBlendRed, &state->mBlendRed);
        copyFloat(&reduceState->mBlendGreen, &state->mBlendGreen);
        copyFloat(&reduceState->mBlendBlue, &state->mBlendBlue);
        copyFloat(&reduceState->mBlendAlpha, &state->mBlendAlpha);
        reduceState->mAlphaTest = (u8)state->mAlphaTest;
        reduceState->mAlphaComparison = (u32)state->mAlphaComparison;
        copyFloat(&reduceState->mAlphaReference, &state->mAlphaReference);
        reduceState->mStencilEnable = (u8)state->mStencilEnable;
        reduceState->mStencilComparison = (u32)state->mStencilComparison;
        reduceState->mStencilReference = (u32)state->mStencilReference;
        reduceState->mStencilMask = (u32)state->mStencilMask;
        reduceState->mStencilFail = (u32)state->mStencilFail;
        reduceState->mStencilDepthFail = (u32)state->mStencilDepthFail;
        reduceState->mStencilPass = (u32)state->mStencilPass;
        reduceState->mFrontPolygon = (u32)state->mFrontPolygon;
        reduceState->mBackPolygon = (u32)state->mBackPolygon;
        reduceState->mPolygonOffset = (u8)state->mPolygonOffset;
        reduceState->mLineOffset = (u8)state->mLineOffset;
        reduceState->mPointOffset = (u8)state->mPointOffset;
        reduceState->mTable = gabi::at<void>(gabi::ea(&sReduceStateMethods));
        reduceState->mDepthTest = 0;
        reduceState->mDepthWrite = 0;
        reduceState->mBlendEnable = 0;
        reduceState->mWriteRed = 1;
        reduceState->mWriteGreen = 1;
        reduceState->mWriteBlue = 1;
        reduceState->mWriteAlpha = 1;
        reduceState->apply();
    } else {
        gabi::Local<sead::Viewport> viewport;
        sead::Viewport::createSurface(viewport, (const sead::RenderVector2*)arg->mOutput.get());
        viewport->apply((const sead::RenderSurface*)arg->mOutput.get());
        bindRenderSurface(arg->mOutput);
        state->apply();
    }

    u32 slot = mComposeMode == cCompose_DepthMask ? Parameters::cProgram_DepthMask : Parameters::cProgram_Final;
    agl::ShaderProgram* program = arg->mParameters->program(slot, arg->mFlags);
    mode = activateProgram(program, mode);
    uploadDepthBlurComposite(this, arg, program);
    if (arg->mParameters->mVignettingBlur.mValue) setVignettingUniforms(arg, program);
    drawDepthBlurShape(this, arg);

    if (mSettings.mReduceDraw.mValue) {
        // Second pass: the reduced target back onto the frame with the reduce program.
        gabi::Local<DepthBlurContext> reduceArg;
        for (u32 word = 0; word < sizeof(DepthBlurContext) / 4; ++word)
            gabi::store<u32>(gabi::ea(reduceArg) + word * 4, gabi::load<u32>(gabi::ea(arg) + word * 4));
        reduceArg->mDrawMode = DepthBlurContext::cPass_Reduce;
        state->applyDepthStencil();
        state->applyBlend();
        state->applyChannelMask();
        invalidateColorTarget(arg->mTargets->mMipOutput.mColors[0]);
        invalidateTargetTexture(arg->mTargets->mMipOutput.mColors[0]);
        gabi::Local<sead::Viewport> reduceViewport;
        sead::Viewport::createSurface(reduceViewport, (const sead::RenderVector2*)&reduceArg->mTargets->mMipOutput);
        gabi::Local<sead::Viewport> viewport;
        sead::Viewport::createSurface(viewport, (const sead::RenderVector2*)reduceArg->mOutput.get());
        viewport->apply((const sead::RenderSurface*)reduceArg->mOutput.get());
        bindRenderSurface(reduceArg->mOutput);
        agl::ShaderProgram* reduceProgram = reduceArg->mParameters->mShaderPrograms[Parameters::cProgram_Reduce];
        mode = activateProgram(reduceProgram, mode);
        setReduceUniforms(reduceArg, reduceProgram);
        drawDepthBlurShape(this, reduceArg);
    }
    return mode;
}

/* 02782214 */
void DepthOfField::freeBuffer(s32 contextIndex) {
    ContextTargets* context = this->context(contextIndex);
    agl::DynamicTextureCache* allocator = sTextureAllocator;
    if (context->mBlurSource) {
        freeTexture(allocator, context->mBlurSource);
        context->mBlurSource = NULL;
    }
    if (context->mDepthSource) {
        freeTexture(allocator, context->mDepthSource);
        context->mDepthSource = NULL;
    }
}

/* 0278229C */
u32 DepthOfField::drawVignetting(const DepthBlurContext* arg, u32 mode) {
    gabi::Local<sead::CompactRenderState> state;
    sead::CompactRenderState::create(state);
    state->mWriteAlpha = 0;
    state->mWriteRed = 1;
    state->mDepthWrite = 0;
    state->mWriteGreen = 1;
    state->mDepthTest = 0;
    state->mWriteBlue = 1;
    switch ((u32)arg->mParameters->mVignettingBlendMode.mValue) {
    case Parameters::cBlend_Normal:     // src alpha, inverse src alpha
        state->mBlend.mColorSource = 4; state->mBlend.mAlphaSource = 4;
        state->mBlend.mColorDestination = 5; state->mBlend.mAlphaDestination = 5;
        state->mBlend.mColorEquation = 0; state->mBlend.mAlphaEquation = 0;
        break;
    case Parameters::cBlend_Add:        // src alpha, one
        state->mBlend.mColorSource = 4; state->mBlend.mAlphaSource = 4;
        state->mBlend.mColorDestination = 1; state->mBlend.mAlphaDestination = 1;
        state->mBlend.mColorEquation = 0; state->mBlend.mAlphaEquation = 0;
        break;
    case Parameters::cBlend_Multiply:   // zero, src colour
        state->mBlend.mColorSource = 0; state->mBlend.mAlphaSource = 0;
        state->mBlend.mColorDestination = 2; state->mBlend.mAlphaDestination = 2;
        state->mBlend.mColorEquation = 0; state->mBlend.mAlphaEquation = 0;
        break;
    case Parameters::cBlend_Screen:     // inverse dst colour, one
        state->mBlend.mColorSource = 9; state->mBlend.mAlphaSource = 9;
        state->mBlend.mColorDestination = 1; state->mBlend.mAlphaDestination = 1;
        state->mBlend.mColorEquation = 0; state->mBlend.mAlphaEquation = 0;
        break;
    }
    state->apply();
    gabi::Local<sead::Viewport> viewport;
    sead::Viewport::createSurface(viewport, (const sead::RenderVector2*)arg->mOutput.get());
    viewport->apply((const sead::RenderSurface*)arg->mOutput.get());
    bindRenderSurface(arg->mOutput);
    agl::ShaderProgram* program = arg->mParameters->mShaderPrograms[Parameters::cProgram_Vignetting];
    mode = activateProgram(program, mode);
    setVignettingUniforms(arg, arg->mParameters->mShaderPrograms[Parameters::cProgram_Vignetting]);
    drawDepthBlurShape(this, arg);
    return mode;
}

/* 027824A8 */
u32 DepthOfField::draw(s32 contextIndex, agl::RenderBuffer* renderBuffer, void* depth, u32 viewDepth, u32 mode,
                       f32 nearZ, f32 farZ) {
    if (!mEnableParameter.mValue) return mode;
    if ((mViewMask & shiftLeft(1, contextIndex)) == 0) return mode;
    gabi::Local<DepthBlurContext> arg;
    constructDepthBlurContext(arg, contextIndex, context(contextIndex), viewSettings(contextIndex), renderBuffer,
                              static_cast<const agl::TextureData*>(depth), viewDepth, nearZ, farZ);
    Parameters* settings = viewSettings(contextIndex);
    if (settings->usesBlurPass()) {
        prepareDepthBlurTexture(this, contextIndex, renderBuffer);
        arg->mDrawMode = DepthBlurContext::cPass_ColorMipMap;
        mode = drawDepthBlurMipChain(this, arg, mode);
        if (settings->usesNearDepthBlur()) {
            arg->mDrawMode = DepthBlurContext::cPass_DepthMipMap;
            mode = drawDepthBlurNearMipChain(this, arg, mode);
        }
        if (mComposeMode == cCompose_Combined) {
            mode = drawDepthBlurFullSurface(this, arg, mode);
        } else {
            arg->mDrawMode = DepthBlurContext::cPass_Compose;
            mode = drawCompose(arg, mode);
        }
        freeBuffer(contextIndex);
    }
    if (settings->usesVignettingColorOnly()) {
        arg->mDrawMode = DepthBlurContext::cPass_Vignetting;
        mode = drawVignetting(arg, mode);
    }
    return mode;
}

/* 02782660 */
u32 DepthOfField::draw(s32 contextIndex, agl::RenderBuffer* renderBuffer, u32 mode, f32 nearZ, f32 farZ) {
    return draw(contextIndex, renderBuffer, renderBuffer->mDepth, 0, mode, nearZ, farZ);
}

/* 02782670 */
// A vignetting-shape parameter object changed: copy its values into the first view's settings.
u32 DepthOfField::applyVignettingShape(u32 index, const VignettingShapeParameter* shape) {
    u32 which = index == 0 ? 0 : 1;
    VignettingShape* target = &firstViewSettings()->mShapes[which];
    target->mType.mValue = (s32)shape->mShape.mType.mValue;
    target = &firstViewSettings()->mShapes[which];
    copyFloat(&target->mScale.x, &shape->mShape.mScale.x);
    copyFloat(&target->mScale.y, &shape->mShape.mScale.y);
    target = &firstViewSettings()->mShapes[which];
    copyFloat(&target->mTranslation.x, &shape->mShape.mTranslation.x);
    copyFloat(&target->mTranslation.y, &shape->mShape.mTranslation.y);
    target = &firstViewSettings()->mShapes[which];
    copyFloat(&target->mRange.x, &shape->mShape.mRange.x);
    copyFloat(&target->mRange.y, &shape->mShape.mRange.y);
    return updateShaderPrograms(&mSettings);
}

/* 02782770 */
void DepthOfField::refreshViewSettings(u32 contextIndex) {
    updateShaderPrograms(viewSettings(contextIndex));
    viewSettings(contextIndex)->updateIndirectTransform();
}

/* 027827F4 */
u32 VignettingShapeParameter::postRead() {
    return mOwner->applyVignettingShape(mIndex, this);
}

/* 02782C20 */
// sead::Matrix34f::makeR (x, then y, then z), no translation. The angles' sines and cosines are doubles.
void agl::dof::makeRotation(sead::RenderMatrix34* matrix, const sead::RenderVector3* rotation) {
    f64 sinX = sine(rotation->x), sinY = sine(rotation->y), sinZ = sine(rotation->z);
    f64 cosX = cosine(rotation->x), cosY = cosine(rotation->y), cosZ = cosine(rotation->z);
    f32 sinXsinY = gabi::fmuls_fpr(sinX, sinY);
    f32 cosXsinZ = gabi::fmuls_fpr(cosX, sinZ);
    f32 cosXcosZ = gabi::fmuls_fpr(cosX, cosZ);
    matrix->mRows[2][3] = 0.0f;
    matrix->mRows[0][0] = gabi::fmuls_fpr(cosY, cosZ);
    matrix->mRows[0][1] = (f32)gabi::ppc_nan3<f64>(sinXsinY, cosXsinZ, cosZ,
        (f64)sinXsinY * round25(cosZ) - (f64)cosXsinZ, gabi::ppc_dnan64());
    matrix->mRows[0][2] = (f32)gabi::ppc_nan3<f64>(cosXcosZ, gabi::fmuls_fpr(sinX, sinZ), sinY,
        (f64)cosXcosZ * round25(sinY) + (f64)gabi::fmuls_fpr(sinX, sinZ), gabi::ppc_dnan64());
    matrix->mRows[0][3] = 0.0f;
    matrix->mRows[1][0] = gabi::fmuls_fpr(cosY, sinZ);
    matrix->mRows[1][1] = (f32)gabi::ppc_nan3<f64>(sinXsinY, cosXcosZ, sinZ,
        (f64)sinXsinY * round25(sinZ) + (f64)cosXcosZ, gabi::ppc_dnan64());
    matrix->mRows[1][2] = (f32)gabi::ppc_nan3<f64>(cosXsinZ, gabi::fmuls_fpr(sinX, cosZ), sinY,
        (f64)cosXsinZ * round25(sinY) - (f64)gabi::fmuls_fpr(sinX, cosZ), gabi::ppc_dnan64());
    matrix->mRows[1][3] = 0.0f;
    matrix->mRows[2][0] = (f32)-sinY;
    matrix->mRows[2][1] = gabi::fmuls_fpr(sinX, cosY);
    matrix->mRows[2][2] = gabi::fmuls_fpr(cosX, cosY);
}

/* 02782D70 */
// sead::Matrix34f::setMul(source, scale matrix): each rotation column times its scale; translation kept.
void agl::dof::scaleColumns(sead::RenderMatrix34* destination, const sead::RenderMatrix34* source, const sead::RenderVector3* scale) {
    destination->mRows[0][0] = source->mRows[0][0] * scale->x;
    destination->mRows[1][0] = source->mRows[1][0] * scale->x;
    destination->mRows[2][0] = source->mRows[2][0] * scale->x;
    destination->mRows[0][1] = source->mRows[0][1] * scale->y;
    destination->mRows[1][1] = source->mRows[1][1] * scale->y;
    destination->mRows[2][1] = source->mRows[2][1] * scale->y;
    destination->mRows[0][2] = source->mRows[0][2] * scale->z;
    destination->mRows[1][2] = source->mRows[1][2] * scale->z;
    destination->mRows[2][2] = source->mRows[2][2] * scale->z;
    copyFloat(&destination->mRows[0][3], &source->mRows[0][3]);
    copyFloat(&destination->mRows[1][3], &source->mRows[1][3]);
    copyFloat(&destination->mRows[2][3], &source->mRows[2][3]);
}

/* 02782E1C */
// sead::Matrix34f::setMul(source, translation matrix): rotation rows kept, translation moved along them.
void agl::dof::translateLocal(sead::RenderMatrix34* destination, const sead::RenderMatrix34* source, const sead::RenderVector3* translation) {
    for (u32 row = 0; row < 3; ++row) {
        copyFloat(&destination->mRows[row][0], &source->mRows[row][0]);
        copyFloat(&destination->mRows[row][1], &source->mRows[row][1]);
        copyFloat(&destination->mRows[row][2], &source->mRows[row][2]);
        f32 y = translation->y;
        f32 partial = source->mRows[row][1] * y;
        f32 x = translation->x;
        partial = gabi::fmadds(source->mRows[row][0], x, partial);
        f32 z = translation->z;
        partial = gabi::fmadds(source->mRows[row][2], z, partial);
        destination->mRows[row][3] = partial + source->mRows[row][3];
    }
}

/* WWHD glue
 data sTextureAllocator 101F8DD4
 data sDepthTestEnabled 101F8C10
 data sReduceStateMethods 10162698
 ext 027BB9E0 void updateProgram(agl::ShaderProgram* program, u32 flags)
 ext 027B9F68 void setShaderMode(u32)
 ext 027BB7CC void bindProgram(agl::ShaderProgram* program)
 ext C00060E0 void GX2CallDisplayList(void*, u32)
 ext C0006A40 void GX2SetVertexUniformReg(s32, u32, const void*)
 ext C0006908 void GX2SetPixelUniformReg(s32, u32, const void*)
 ext 027BE53C void activateSampler(agl::TextureSampler*, agl::ProgramResource16_l*, s32, u32)
 ext 025C0848 void applyTargetTexture(agl::ColorSurface*, agl::TextureData*, u32, u32)
 ext 027BDEB4 void copySamplerTexture(agl::TextureSampler*, const agl::TextureSampler*)
 ext 027B6D64 void invalidateColorTarget(agl::ColorSurface*)
 ext 027BE794 void invalidateTargetTexture(agl::ColorSurface*)
 ext 0274D690 void bindRenderSurface(agl::RenderBuffer*)
 ext 027A0040 void freeTexture(agl::DynamicTextureCache* allocator, agl::TextureCacheEntry* texture)
 ext 028F43F8 f64 sine(f64)
 ext 028F4BE0 f64 cosine(f64)
 ext 0274FDBC sead::CompactRenderState* sead::CompactRenderState::create(sead::CompactRenderState*)
 ext 0274FEB0 void sead::CompactRenderState::apply()
 ext 02750070 void sead::CompactRenderState::applyDepthStencil()
 ext 027500F0 void sead::CompactRenderState::applyChannelMask()
 ext 02750138 void sead::CompactRenderState::applyBlend()
 ext 0274F5F0 sead::Viewport* sead::Viewport::createSurface(sead::Viewport*, const sead::RenderVector2*)
 ext 0274F964 void sead::Viewport::apply(const sead::RenderSurface*) const
 ext 0277F450 void constructDepthBlurContext(DepthBlurContext* context, s32 view, ContextTargets* targets, Parameters* parameters, agl::RenderBuffer* output, const agl::TextureData* secondary, u32 flags, f32 nearDepth, f32 farDepth)
 ext 0277FBA8 void prepareDepthBlurTexture(DepthOfField* runtime, s32 view, agl::RenderBuffer* buffer)
 ext 0277FF88 u32 drawDepthBlurMipChain(DepthOfField* runtime, const DepthBlurContext* context, u32 mode)
 ext 027805A0 u32 drawDepthBlurNearMipChain(DepthOfField* runtime, const DepthBlurContext* context, u32 mode)
 ext 02780C34 u32 drawDepthBlurFullSurface(DepthOfField* runtime, const DepthBlurContext* context, u32 mode)
 ext 0277FBDC void applyDepthBlurOutputMip(DepthOfField* runtime, agl::RenderBuffer* output, u32 mip, u32 secondaryOffset)
 ext 0277FD9C void drawDepthBlurShape(DepthOfField* runtime, const DepthBlurContext* context)
 ext 02780D1C void uploadDepthBlurComposite(DepthOfField* runtime, const DepthBlurContext* context, const agl::ShaderProgram* program)
 ext 0277CA98 u32 updateShaderPrograms(Parameters* parameters)
 ext 0277E714 u32 agl::depth_blur_value::Parameters::usesDepthPass() const
 ext 0277E774 u32 agl::depth_blur_value::Parameters::usesBlurPass() const
 ext 0277C8D4 u32 agl::depth_blur_value::Parameters::usesNearDepthBlur() const
 ext 0277C910 u32 agl::depth_blur_value::Parameters::usesTwoVignettingShapes() const
 ext 0277C944 u32 agl::depth_blur_value::Parameters::usesVignettingColorOnly() const
 ext 0277D168 void agl::depth_blur_value::Parameters::updateIndirectTransform()
*/
