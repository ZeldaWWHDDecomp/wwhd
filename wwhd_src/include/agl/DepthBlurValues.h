// Depth-blur parameter object and shader-program selection, reconstructed from the game's
// instructions and parameter strings. No external headers or implementation were used.
// Vectors and matrices are sead render-math types; programs are agl::ShaderProgram with their shared data.
#pragma once
#include "agl/ParameterValues.h"
#include "agl/ShaderProgramActivation.h"
#include "agl/ShaderHolder.h"
#include "sead/RenderMath.h"

namespace agl { namespace depth_blur_value {
// Four floats passed as one shader uniform or matrix column (sead has no four-float vector type).
struct Vector4 { be<f32> x, y, z, w; };
struct Vector2Parameter { agl::env::ParameterBinding mBinding; be<f32> x, y; };
struct VignettingShape {
    parameter_value::IntegerParameter mType;
    Vector2Parameter mRange, mScale, mTranslation;
};
// This unit's column helpers on sead's 4x4 matrix.
struct Matrix4 : sead::RenderMatrix44 {
    void setColumn(u32 column, const Vector4* value);
    void setTranslation(const sead::RenderVector3* value);
};
struct Parameters {
    // Program slots of mShaderPrograms the draw passes use, and the vignetting blend modes.
    enum ProgramSlot { cProgram_DepthMask = 2, cProgram_Final = 4, cProgram_Vignetting = 6, cProgram_Reduce = 7 };
    enum VignettingBlend { cBlend_Normal, cBlend_Add, cBlend_Multiply, cBlend_Screen };
    parameter_value::BooleanParameter mNear;
    parameter_value::BooleanParameter mFar;
    parameter_value::BooleanParameter mDepthBlur;
    parameter_value::BooleanParameter mVignettingColor;
    parameter_value::BooleanParameter mVignettingBlur;
    parameter_value::BooleanParameter mVignettingTwoShapes;
    parameter_value::BooleanParameter mColorControl;
    parameter_value::BooleanParameter mColorReverse;
    parameter_value::BooleanParameter mIndirect;
    parameter_value::BooleanParameter mIndirectDepthCancel;
    parameter_value::BooleanParameter mReduceDraw;
    parameter_value::BooleanParameter mFarMaximum;
    parameter_value::BooleanParameter mIndirectFromFull;
    parameter_value::FloatParameter mStart, mEnd, mFarStart, mFarEnd;
    parameter_value::FloatParameter mLevel;
    parameter_value::FloatParameter mDepthBlurAdd, mFarMaximumDepth;
    parameter_value::FloatParameter mSaturationMinimum;
    parameter_value::ColorParameter mColorControlDepth;
    Vector2Parameter mIndirectTranslation;
    Vector2Parameter mIndirectScale;
    parameter_value::FloatParameter mIndirectRotation;
    parameter_value::FloatParameter mIndirectStrength, mVignettingBlurStrength;
    parameter_value::IntegerParameter mVignettingBlendMode;
    parameter_value::ColorParameter mVignettingColorParameter;
    VignettingShape mShapes[2];
    parameter_value::ColorParameter mFarMultiplyColorParameter;
    u8 mResourceHeader[8];
    gptr<agl::ShaderProgram> mShaderPrograms[8];
    be<f32> mResourceScale;
    gptr<void> mIndirectTexture;
    agl::env::ParameterColor mIndirectColor;
    sead::RenderVector3 mIndirectTransformRow0;
    sead::RenderVector3 mIndirectTransformRow1;
    // Bounds-checked pair lookup (depth mask / final): an index of 2 or more takes the first of the pair.
    agl::ShaderProgram* program(u32 first, u32 index) const {
        const gptr<agl::ShaderProgram>* slot = &mShaderPrograms[first];
        if (index < 2) slot += index;
        return *slot;
    }
    u32 usesNearDepthBlur() const;
    u32 usesTwoVignettingShapes() const;
    u32 usesVignettingColorOnly() const;
    u32 usesIndirectTexture() const;
    u32 usesIndirectFromFull() const;
    u32 usesDepthPass() const;
    u32 usesBlurPass() const;
    void updateIndirectTransform();
    void updateShaderPrograms();
};
WWHD_SIZE(Vector4, 0x10);
WWHD_SIZE(Matrix4, 0x40);
WWHD_OFFSET(Parameters, mLevel, 0x110);
WWHD_OFFSET(Parameters, mVignettingBlendMode, 0x1C4);
WWHD_OFFSET(Parameters, mShapes, 0x1F0);
WWHD_OFFSET(Parameters, mIndirectTexture, 0x2D0);
WWHD_OFFSET(Parameters, mIndirectTranslation, 0x16C);
WWHD_OFFSET(Parameters, mIndirectScale, 0x180);
WWHD_OFFSET(Parameters, mIndirectRotation, 0x194);
WWHD_OFFSET(Parameters, mIndirectTransformRow0, 0x2E4);
WWHD_OFFSET(Parameters, mVignettingColorParameter, 0x1D4);
WWHD_OFFSET(Parameters, mFarMultiplyColorParameter, 0x288);
WWHD_OFFSET(Parameters, mShaderPrograms, 0x2AC);
WWHD_SIZE(Parameters, 0x2FC);
} }

// The library's shader-program holder singleton word (101F8D6C; owned by agl/TimingStatics.cpp,
// declared in agl/ShaderHolder.h); these sources keep their own `data` binding of it.
using ShaderHolderPointer = gptr<agl::detail::ShaderHolder>;
