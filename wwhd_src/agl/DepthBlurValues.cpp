// Depth-blur matrix and feature helpers, derived from this game's binary only.
#include "bindings.h"
#include "agl/DepthBlurValues.h"
#include "agl/TextureResource.h"
#include "agl/DepthOfFieldDraw.h"
using namespace agl::depth_blur_value;

static sead::DisposerVtable sParameterArchiveMethods;
static u32 sParameterCategoryMethods;
static void releaseArchiveEntries(agl::ResourceRecord* archive);
static void destroyArchiveBase(agl::ResourceRecord* archive, u32 flags);
static ShaderHolderPointer sDepthBlurShaderHolder; // shader program holder singleton
static u8 sDepthBlurDoubleBuffer;
static u32 getShaderProgramAddress(u32 programIndex) {
    agl::detail::ShaderHolder* holder = sDepthBlurShaderHolder;
    u32 programAddress = 0;
    if (u32(holder->mPrograms.mCount) > programIndex)
        programAddress = gabi::ea(holder->programSlots()[programIndex].get());
    return programAddress;
}
static agl::ShaderVariantDomain* shaderVariantsAt(u32 programAddress) {
    // The original still reads the shared-data word at guest 0x7C when lookup fails.
    // Load through its guest address so C++ cannot eliminate the null-program branch.
    const u32 sharedAddress = gabi::load<u32>(programAddress + offsetof(agl::ShaderProgram, binary));
    return gabi::at<agl::ShaderVariantDomain>(sharedAddress + offsetof(agl::ShaderProgramSharedData, variation));
}
static u32 macroStride(const agl::ShaderVariantDomain* variants, u32 macro) {
    const agl::ShaderVariantRadix* definition = variants->radices;
    if (variants->count > macro) definition += macro;
    return definition->divisor;
}
static agl::ShaderProgram* selectShaderVariant(const agl::ShaderVariantDomain* variants, s32 index, bool zeroOnly = false) {
    if (zeroOnly ? index == 0 : index <= 0) return variants->root;
    agl::ShaderProgram* entry = variants->programs;
    if (u32(index) - 1 < variants->programCount)
        entry += u32(index) - 1;
    return entry;
}

/* 0277C7E8 */
static void destroyParameterArchiveStorage(agl::ResourceRecord* archive, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!archive) return;
    archive->mVtable = &sParameterArchiveMethods;
    archive->mInterfaceMethods = &sParameterCategoryMethods;
    releaseArchiveEntries(archive);
    destroyArchiveBase(archive, 0);
    if (flags & 1) operator_delete(archive);
}

/* 0277C85C */
void Matrix4::setColumn(u32 column, const Vector4* value) {
    gabi::AutoFrame frame{gabi::current_function()};
    mRows[0][column] = value->x;
    mRows[1][column] = value->y;
    mRows[2][column] = value->z;
    mRows[3][column] = value->w;
}

/* 0277C888 */
void Matrix4::setTranslation(const sead::RenderVector3* value) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<Vector4> column(8);
    column->z = value->z;
    column->y = value->y;
    column->x = value->x;
    column->w = 1.0f;
    setColumn(3, column);
}

/* 0277C8D4 */
u32 Parameters::usesNearDepthBlur() const {
    gabi::AutoFrame frame{gabi::current_function()};
    // PPC bge tests the absence of LT, so unordered levels are rejected too.
    return mNear.mValue && mDepthBlur.mValue && 0.0f < f32(mLevel.mValue);
}

/* 0277C910 */
u32 Parameters::usesTwoVignettingShapes() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return mVignettingColor.mValue && mVignettingBlur.mValue && mVignettingTwoShapes.mValue;
}

/* 0277C944 */
u32 Parameters::usesVignettingColorOnly() const {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!mVignettingColor.mValue) return 0;
    if (usesTwoVignettingShapes() || mVignettingBlendMode.mValue != 0) return 1;
    if (!mVignettingBlur.mValue) return 1;
    return !(f32(mLevel.mValue) > 0.0f);
}

/* 0277C9C4 */
u32 Parameters::usesIndirectTexture() const {
    gabi::AutoFrame frame{gabi::current_function()};
    return mIndirect.mValue && mIndirectTexture && mFar.mValue;
}

/* 0277C9F8 */
u32 Parameters::usesIndirectFromFull() const {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!usesIndirectTexture()) return 0;
    const f32 level = mLevel.mValue;
    s32 integralLevel = gabi::ftoi(level);
    // The integer-to-double bias pair is also the original frame's observable scratch.
    struct ConversionBias { be<u32> exponent, mantissa; };
    gabi::FrameLocal<ConversionBias> conversion(8);
    conversion->exponent = 0x43300000;
    conversion->mantissa = u32(integralLevel) ^ 0x80000000;
    if (f32(integralLevel) != level && level < 0.0f)
        integralLevel = s32(u32(integralLevel) - 1);
    return integralLevel == 0 || mIndirectFromFull.mValue;
}

/* 0277E714 */
u32 Parameters::usesDepthPass() const {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!mNear.mValue && !mFar.mValue) return 0;
    if (0.0f < f32(mLevel.mValue)) return 1;
    return usesIndirectFromFull() != 0;
}

/* 0277E774 */
u32 Parameters::usesBlurPass() const {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!usesDepthPass() && !mVignettingBlur.mValue) return 0;
    if (0.0f < f32(mLevel.mValue)) return 1;
    return usesIndirectFromFull() != 0;
}

/* 0277D168 */
void Parameters::updateIndirectTransform() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::RenderVector3> argument(8);
    gabi::FrameLocal<sead::RenderMatrix34> matrix(0x14);
    argument->z = mIndirectRotation.mValue;
    argument->x = 0.0f;
    argument->y = 0.0f;
    agl::dof::makeRotation(matrix, argument);
    argument->z = 0.0f;
    argument->x = mIndirectScale.x;
    argument->y = mIndirectScale.y;
    agl::dof::scaleColumns(matrix, matrix, argument);
    argument->z = 0.0f;
    argument->x = mIndirectTranslation.x;
    argument->y = mIndirectTranslation.y;
    agl::dof::translateLocal(matrix, matrix, argument);
    mIndirectTransformRow0.y = matrix->mRows[0][1];
    mIndirectTransformRow0.z = matrix->mRows[0][3];
    mIndirectTransformRow1.z = matrix->mRows[1][3];
    mIndirectTransformRow1.y = matrix->mRows[1][1];
    mIndirectTransformRow0.x = matrix->mRows[0][0];
    mIndirectTransformRow1.x = matrix->mRows[1][0];
}

/* 0277CA98 */
void Parameters::updateShaderPrograms() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 programAddress = getShaderProgramAddress(46);
    agl::ShaderVariantDomain* variants = shaderVariantsAt(programAddress);
    mShaderPrograms[0] = selectShaderVariant(variants,
        s32(macroStride(variants, 0) * (sDepthBlurDoubleBuffer ? 2 : 1)));
    variants = shaderVariantsAt(programAddress);
    mShaderPrograms[1] = selectShaderVariant(variants, s32(macroStride(variants, 1)), true);

    const u32 nearDepth = usesNearDepthBlur();
    u32 nearMode = 0;
    if (mNear.mValue && 0.0f < f32(mLevel.mValue)) nearMode = nearDepth == 1 ? 2 : 1;

    const u32 vignettingBlur = mVignettingBlur.mValue;
    u32 blurMode = 0;
    if (vignettingBlur && 0.0f < f32(mLevel.mValue)) {
        blurMode = 1;
        if (mVignettingColor.mValue && !usesVignettingColorOnly()) {
            const bool origin = mVignettingColorParameter.mValue.r == 0.0f
                && mVignettingColorParameter.mValue.g == 0.0f && mVignettingColorParameter.mValue.b == 0.0f;
            blurMode = origin ? 2 : 3;
        }
    }

    u32 farMode = 0;
    if (mFar.mValue && (0.0f < f32(mLevel.mValue) || usesIndirectFromFull())) {
        farMode = mFarMaximum.mValue ? 2 : 1;
        if (mColorControl.mValue) {
            if (mSaturationMinimum.mValue != 1.0f) farMode += 2;
            if (mFarMultiplyColorParameter.mValue.r != 1.0f || mFarMultiplyColorParameter.mValue.g != 1.0f
                || mFarMultiplyColorParameter.mValue.b != 1.0f) farMode += 4;
        }
    }

    u32 indirectMode = 0;
    if (usesIndirectTexture()) indirectMode = mIndirectDepthCancel.mValue ? 2 : 1;

    programAddress = getShaderProgramAddress(47);
    variants = shaderVariantsAt(programAddress);
    u32 index = (farMode ? macroStride(variants, 0) * (mFarMaximum.mValue ? 2 : 1) : 0)
        + macroStride(variants, 1) * nearMode
        + macroStride(variants, 2) * vignettingBlur;
    mShaderPrograms[2] = selectShaderVariant(variants, s32(index));
    variants = shaderVariantsAt(programAddress);
    mShaderPrograms[3] = selectShaderVariant(variants, s32(index | 1));

    programAddress = getShaderProgramAddress(48);
    variants = shaderVariantsAt(programAddress);
    index = macroStride(variants, 0) * farMode + macroStride(variants, 1) * nearMode
        + macroStride(variants, 2) * blurMode + macroStride(variants, 3) * indirectMode;
    mShaderPrograms[4] = selectShaderVariant(variants, s32(index));
    variants = shaderVariantsAt(programAddress);
    mShaderPrograms[5] = selectShaderVariant(variants, s32(index | 1));

    variants = shaderVariantsAt(getShaderProgramAddress(49));
    const bool origin = mVignettingColorParameter.mValue.r == 0.0f
        && mVignettingColorParameter.mValue.g == 0.0f && mVignettingColorParameter.mValue.b == 0.0f;
    const u32 shapeMode = mVignettingBlendMode.mValue == 2 ? 1 : (mVignettingBlendMode.mValue == 3 ? 2 : 0);
    index = macroStride(variants, 0) * u32(!origin) + macroStride(variants, 1) * shapeMode;
    mShaderPrograms[6] = selectShaderVariant(variants, s32(index));

    variants = shaderVariantsAt(getShaderProgramAddress(50));
    const u32 farOnly = !mVignettingBlur.mValue && mFar.mValue;
    const u32 nearOnly = !mVignettingBlur.mValue && mNear.mValue && !mDepthBlur.mValue;
    index = macroStride(variants, 0) * farOnly + macroStride(variants, 1) * nearOnly;
    mShaderPrograms[7] = selectShaderVariant(variants, s32(index));
}

/* WWHD glue
 data sParameterArchiveMethods 10161F94 nocheck
 data sParameterCategoryMethods 10161FA4 nocheck
 ext 027A0998 void releaseArchiveEntries(agl::ResourceRecord* archive)
 ext 02752BEC void destroyArchiveBase(agl::ResourceRecord* archive, u32 flags)
 ext 02782C20 void agl::dof::makeRotation(sead::RenderMatrix34* matrix, const sead::RenderVector3* rotation)
 ext 02782D70 void agl::dof::scaleColumns(sead::RenderMatrix34* destination, const sead::RenderMatrix34* source, const sead::RenderVector3* scale)
 ext 02782E1C void agl::dof::translateLocal(sead::RenderMatrix34* destination, const sead::RenderMatrix34* source, const sead::RenderVector3* translation)
 data sDepthBlurShaderHolder 101F8D6C
 data sDepthBlurDoubleBuffer 101F8C11
*/
