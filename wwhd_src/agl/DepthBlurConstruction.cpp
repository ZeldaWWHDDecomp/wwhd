// Depth-blur parameter construction, from this game's allocation and field-store sequence.
#include "bindings.h"
#include "agl/DepthBlurValues.h"
using namespace agl::depth_blur_value;
using namespace agl::parameter_value;

static u32 sDepthBooleanMethods;
static u32 sDepthFloatMethods;
static u32 sDepthIntegerMethods;
static u32 sDepthVector2Methods;
static u32 sDepthColorMethods;
static u32 sDepthVector4Methods;
static agl::env::ParameterColor sDepthDefaultIndirectColor;
static void* allocateDepthParameter(u32 size);
static void constructDepthParameterBase(void* parameter);

template<class T> static T* constructDepthParameter(T* storage, const void* methods) {
    // A wrapped guest member address can be zero even though its host expression is nonnull.
    if (gabi::ea(storage) == 0) storage = static_cast<T*>(allocateDepthParameter(sizeof(T)));
    if (storage) {
        constructDepthParameterBase(storage);
        reinterpret_cast<agl::env::ParameterBinding*>(storage)->mMethods
            = reinterpret_cast<sead::VtableSlot*>(const_cast<void*>(methods));
    }
    return storage;
}
static void constructDepthColor(ColorParameter* storage) {
    storage = constructDepthParameter(storage, &sDepthVector4Methods);
    if (!storage) return;
    storage->mValue.r = 0.0f;
    storage->mValue.g = 0.0f;
    storage->mValue.b = 0.0f;
    storage->mValue.a = 1.0f;
}
static void constructDepthShape(VignettingShape* shape) {
    constructDepthParameter(&shape->mType, &sDepthIntegerMethods);
    constructDepthParameter(&shape->mRange, &sDepthVector2Methods);
    constructDepthParameter(&shape->mScale, &sDepthVector2Methods);
    constructDepthParameter(&shape->mTranslation, &sDepthVector2Methods);
}
static sead::RenderVector3* allocateDepthVectorIfNeeded(sead::RenderVector3* storage) {
    if (gabi::ea(storage) == 0) storage = static_cast<sead::RenderVector3*>(allocateDepthParameter(sizeof(sead::RenderVector3)));
    return storage;
}

/* 0277D354 */
static Parameters* constructDepthBlurParameters(Parameters* parameters) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!parameters) parameters = static_cast<Parameters*>(allocateDepthParameter(sizeof(Parameters)));
    if (!parameters) return nullptr;
    BooleanParameter* flags[] = {
        &parameters->mNear, &parameters->mFar, &parameters->mDepthBlur,
        &parameters->mVignettingColor, &parameters->mVignettingBlur, &parameters->mVignettingTwoShapes,
        &parameters->mColorControl, &parameters->mColorReverse, &parameters->mIndirect,
        &parameters->mIndirectDepthCancel, &parameters->mReduceDraw,
        &parameters->mFarMaximum, &parameters->mIndirectFromFull,
    };
    for (auto* flag : flags) constructDepthParameter(flag, &sDepthBooleanMethods);
    FloatParameter* ranges[] = {
        &parameters->mStart, &parameters->mEnd, &parameters->mFarStart, &parameters->mFarEnd,
        &parameters->mLevel, &parameters->mDepthBlurAdd, &parameters->mFarMaximumDepth,
        &parameters->mSaturationMinimum,
    };
    for (auto* range : ranges) constructDepthParameter(range, &sDepthFloatMethods);
    constructDepthParameter(&parameters->mColorControlDepth, &sDepthColorMethods);
    constructDepthParameter(&parameters->mIndirectTranslation, &sDepthVector2Methods);
    constructDepthParameter(&parameters->mIndirectScale, &sDepthVector2Methods);
    constructDepthParameter(&parameters->mIndirectRotation, &sDepthFloatMethods);
    constructDepthParameter(&parameters->mIndirectStrength, &sDepthFloatMethods);
    constructDepthParameter(&parameters->mVignettingBlurStrength, &sDepthFloatMethods);
    constructDepthParameter(&parameters->mVignettingBlendMode, &sDepthIntegerMethods);
    constructDepthColor(&parameters->mVignettingColorParameter);
    constructDepthShape(&parameters->mShapes[0]);
    constructDepthShape(&parameters->mShapes[1]);
    constructDepthColor(&parameters->mFarMultiplyColorParameter);

    // These four eight-byte shader handles have empty constructors, with allocation on zero storage.
    void* handles[] = {parameters->mResourceHeader, &parameters->mShaderPrograms[0],
        &parameters->mShaderPrograms[2], &parameters->mShaderPrograms[4]};
    for (void* handle : handles) if (gabi::ea(handle) == 0) allocateDepthParameter(8);
    parameters->mIndirectTexture = nullptr;
    parameters->mResourceScale = 1.0f;
    parameters->mIndirectColor.r = sDepthDefaultIndirectColor.r;
    parameters->mIndirectColor.g = sDepthDefaultIndirectColor.g;
    parameters->mIndirectColor.b = sDepthDefaultIndirectColor.b;
    parameters->mIndirectColor.a = sDepthDefaultIndirectColor.a;
    if (sead::RenderVector3* row = allocateDepthVectorIfNeeded(&parameters->mIndirectTransformRow0)) {
        row->y = 0.0f;
        row->x = 1.0f;
        row->z = 0.0f;
    }
    if (sead::RenderVector3* row = allocateDepthVectorIfNeeded(&parameters->mIndirectTransformRow1)) {
        row->y = 1.0f;
        row->x = 0.0f;
        row->z = 0.0f;
    }
    return parameters;
}

/* WWHD glue
 data sDepthBooleanMethods 101626F8 nocheck
 data sDepthIntegerMethods 10162770 nocheck
 data sDepthFloatMethods 101627E8 nocheck
 data sDepthVector2Methods 10162860 nocheck
 data sDepthColorMethods 101628D8 nocheck
 data sDepthVector4Methods 10162950 nocheck
 data sDepthDefaultIndirectColor 104A0964
 ext 0273AD10 void* allocateDepthParameter(u32 size)
 ext 0279E674 void constructDepthParameterBase(void* parameter)
*/
