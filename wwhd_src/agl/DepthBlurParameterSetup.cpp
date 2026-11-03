// Depth-blur parameter registration, reconstructed from the game's own strings and stores.
#include "bindings.h"
#include "agl/DepthBlurValues.h"
using namespace agl::depth_blur_value;
using agl::env::ParameterColor;
struct SetupFrameStrings {
    sead::SafeText mNameOrDescription, mLabel;
    ParameterColor mColor;
    sead::SafeText mLevelNameOrMaximum;
    sead::SafeText mDescriptions[28];
};
// Text bytes of the parameter labels and descriptions (guest strings bound by the data lines).
typedef be<u8> TextByte;
static sead::RenderVector2 sSetupDefaultTranslation;
static sead::RenderVector2 sSetupDefaultScale;
static TextByte sSetupBooleanDescription;
static TextByte sSetupIntegerDescription;
static TextByte sSetupFloatDescription;
static TextByte sSetupVectorDescription;
static TextByte sSetupDepthColorDescription;
static TextByte sSetupColorDescription;
static TextByte sSetupLevelRange;
static TextByte sSetupFarMaximumRange;
static sead::SafeTextVtable sSetupStringMethods;
static TextByte sSetupFirstShapeName;
static TextByte sSetupSecondShapeName;
static TextByte sSetupLevelLabel;
static TextByte sSetupStartLabel;
static TextByte sSetupEndLabel;
static TextByte sSetupNearLabel;
static TextByte sSetupFarLabel;
static TextByte sSetupDepthBlurLabel;
static TextByte sSetupVignettingColorLabel;
static TextByte sSetupVignettingBlurLabel;
static TextByte sSetupVignettingTwoShapesLabel;
static TextByte sSetupColorControlLabel;
static TextByte sSetupColorReverseLabel;
static TextByte sSetupIndirectLabel;
static TextByte sSetupIndirectDepthCancelLabel;
static TextByte sSetupReduceDrawLabel;
static TextByte sSetupFarStartLabel;
static TextByte sSetupFarEndLabel;
static TextByte sSetupDepthBlurAddLabel;
static TextByte sSetupSaturationMinimumLabel;
static TextByte sSetupColorControlDepthLabel;
static TextByte sSetupIndirectTranslationLabel;
static TextByte sSetupIndirectRotationLabel;
static TextByte sSetupIndirectScaleLabel;
static TextByte sSetupIndirectStrengthLabel;
static TextByte sSetupVignettingBlurStrengthLabel;
static TextByte sSetupVignettingBlendModeLabel;
static TextByte sSetupVignettingColorParameterLabel;
static TextByte sSetupFarMultiplyColorParameterLabel;
static TextByte sSetupFarMaximumLabel;
static TextByte sSetupFarMaximumDepthLabel;
static TextByte sSetupIndirectFromFullLabel;
static void registerDepthParameter(void* parameter, sead::SafeText* name, sead::SafeText* label,
    sead::SafeText* description, void* archive);
static void setupVignettingShape(VignettingShape* shape, sead::SafeText* name, void* archive, u32 owner);
static void registerDepthParameterText(SetupFrameStrings* frame, void* parameter, const char* text, TextByte* labelText,
    TextByte* descriptionText, u32 descriptionOffset, void* archive, u32 nameOffset = 8) {
    sead::SafeText* name = nameOffset == 8 ? &frame->mNameOrDescription : &frame->mLevelNameOrMaximum;
    sead::SafeText* label = &frame->mLabel;
    sead::SafeText* description = descriptionOffset == 8 ? &frame->mNameOrDescription
        : (descriptionOffset == 0x28 ? &frame->mLevelNameOrMaximum
           : &frame->mDescriptions[(descriptionOffset - 0x30) / sizeof(sead::SafeText)]);
    name->mText = gabi::at<be<u8>>(gabi::guest_addr(text));
    name->mVtable = &sSetupStringMethods;
    label->mText = labelText;
    label->mVtable = &sSetupStringMethods;
    description->mText = descriptionText;
    description->mVtable = &sSetupStringMethods;
    registerDepthParameter(parameter, name, label, description, archive);
}

/* 0277DD2C */
static void setupDepthBlurParameters(Parameters* parameters, void* archive, u32 owner) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<SetupFrameStrings> stack(8);
    ParameterColor* color = &stack->mColor;
    registerDepthParameterText(stack, &parameters->mLevel, "level",
        &sSetupLevelLabel, &sSetupLevelRange, 0x8, archive, 0x28);
    parameters->mLevel.mValue = 2.0f;
    registerDepthParameterText(stack, &parameters->mStart, "start",
        &sSetupStartLabel, &sSetupFloatDescription, 0x30, archive);
    parameters->mStart.mValue = 100.0f;
    registerDepthParameterText(stack, &parameters->mEnd, "end",
        &sSetupEndLabel, &sSetupFloatDescription, 0x38, archive);
    parameters->mEnd.mValue = 200.0f;
    registerDepthParameterText(stack, &parameters->mNear, "near_enable",
        &sSetupNearLabel, &sSetupBooleanDescription, 0x40, archive);
    parameters->mNear.mValue = 0;
    registerDepthParameterText(stack, &parameters->mFar, "far_enable",
        &sSetupFarLabel, &sSetupBooleanDescription, 0x48, archive);
    parameters->mFar.mValue = 1;
    registerDepthParameterText(stack, &parameters->mDepthBlur, "depth_blur",
        &sSetupDepthBlurLabel, &sSetupBooleanDescription, 0x50, archive);
    parameters->mDepthBlur.mValue = 0;
    registerDepthParameterText(stack, &parameters->mVignettingColor, "enable_vignetting_color",
        &sSetupVignettingColorLabel, &sSetupBooleanDescription, 0x58, archive);
    parameters->mVignettingColor.mValue = 0;
    registerDepthParameterText(stack, &parameters->mVignettingBlur, "enable_vignetting_blur",
        &sSetupVignettingBlurLabel, &sSetupBooleanDescription, 0x60, archive);
    parameters->mVignettingBlur.mValue = 0;
    registerDepthParameterText(stack, &parameters->mVignettingTwoShapes, "enable_vignetting_2_shape",
        &sSetupVignettingTwoShapesLabel, &sSetupBooleanDescription, 0x68, archive);
    parameters->mVignettingTwoShapes.mValue = 0;
    registerDepthParameterText(stack, &parameters->mColorControl, "enable_color_control",
        &sSetupColorControlLabel, &sSetupBooleanDescription, 0x70, archive);
    parameters->mColorControl.mValue = 0;
    registerDepthParameterText(stack, &parameters->mColorReverse, "enable_color_reverse",
        &sSetupColorReverseLabel, &sSetupBooleanDescription, 0x78, archive);
    parameters->mColorReverse.mValue = 1;
    registerDepthParameterText(stack, &parameters->mIndirect, "indirect_enable",
        &sSetupIndirectLabel, &sSetupBooleanDescription, 0x80, archive);
    parameters->mIndirect.mValue = 0;
    registerDepthParameterText(stack, &parameters->mIndirectDepthCancel, "indirect_depth_cancelenable",
        &sSetupIndirectDepthCancelLabel, &sSetupBooleanDescription, 0x88, archive);
    parameters->mIndirectDepthCancel.mValue = 0;
    registerDepthParameterText(stack, &parameters->mReduceDraw, "enable_reduce_draw",
        &sSetupReduceDrawLabel, &sSetupBooleanDescription, 0x90, archive);
    parameters->mReduceDraw.mValue = 0;
    registerDepthParameterText(stack, &parameters->mFarStart, "far_start",
        &sSetupFarStartLabel, &sSetupFloatDescription, 0x98, archive);
    parameters->mFarStart.mValue = 120.0f;
    registerDepthParameterText(stack, &parameters->mFarEnd, "far_end",
        &sSetupFarEndLabel, &sSetupFloatDescription, 0xA0, archive);
    parameters->mFarEnd.mValue = 0.0f;
    registerDepthParameterText(stack, &parameters->mDepthBlurAdd, "depth_blur_add",
        &sSetupDepthBlurAddLabel, &sSetupFloatDescription, 0xA8, archive);
    parameters->mDepthBlurAdd.mValue = 0.0f;
    registerDepthParameterText(stack, &parameters->mSaturationMinimum, "saturate_min",
        &sSetupSaturationMinimumLabel, &sSetupFloatDescription, 0xB0, archive);
    parameters->mSaturationMinimum.mValue = 1.0f;
    registerDepthParameterText(stack, &parameters->mColorControlDepth, "color_ctrl_depth",
        &sSetupColorControlDepthLabel, &sSetupDepthColorDescription, 0xB8, archive);
    parameters->mColorControlDepth.mValue.g = 2000.0f;
    parameters->mColorControlDepth.mValue.r = 1000.0f;
    parameters->mColorControlDepth.mValue.a = 4000.0f;
    parameters->mColorControlDepth.mValue.b = 3000.0f;
    registerDepthParameterText(stack, &parameters->mIndirectTranslation, "indirect_tex_trans",
        &sSetupIndirectTranslationLabel, &sSetupVectorDescription, 0xC0, archive);
    parameters->mIndirectTranslation.x = sSetupDefaultTranslation.x;
    parameters->mIndirectTranslation.y = sSetupDefaultTranslation.y;
    registerDepthParameterText(stack, &parameters->mIndirectRotation, "indirect_tex_rotate",
        &sSetupIndirectRotationLabel, &sSetupFloatDescription, 0xC8, archive);
    parameters->mIndirectRotation.mValue = 0.0f;
    registerDepthParameterText(stack, &parameters->mIndirectScale, "indirect_tex_scale",
        &sSetupIndirectScaleLabel, &sSetupVectorDescription, 0xD0, archive);
    parameters->mIndirectScale.x = sSetupDefaultScale.x;
    parameters->mIndirectScale.y = sSetupDefaultScale.y;
    registerDepthParameterText(stack, &parameters->mIndirectStrength, "indirect_scale",
        &sSetupIndirectStrengthLabel, &sSetupFloatDescription, 0xD8, archive);
    parameters->mIndirectStrength.mValue = 0.2f;
    registerDepthParameterText(stack, &parameters->mVignettingBlurStrength, "vignetting_blur",
        &sSetupVignettingBlurStrengthLabel, &sSetupFloatDescription, 0xE0, archive);
    parameters->mVignettingBlurStrength.mValue = 1.0f;
    registerDepthParameterText(stack, &parameters->mVignettingBlendMode, "vignetting_blend",
        &sSetupVignettingBlendModeLabel, &sSetupIntegerDescription, 0xE8, archive);
    parameters->mVignettingBlendMode.mValue = 0;
    color->r = 0.0f;
    color->b = 0.0f;
    color->g = 0.0f;
    color->a = 0.75f;
    registerDepthParameterText(stack, &parameters->mVignettingColorParameter, "vignetting_color",
        &sSetupVignettingColorParameterLabel, &sSetupColorDescription, 0xF0, archive);
    const f32 alpha = color->a;
    const f32 red = color->r;
    parameters->mVignettingColorParameter.mValue.a = alpha;
    parameters->mVignettingColorParameter.mValue.r = red;
    const f32 blue = color->b;
    color->r = 1.0f;
    parameters->mVignettingColorParameter.mValue.b = blue;
    const f32 green = color->g;
    color->g = 1.0f;
    color->b = 1.0f;
    color->a = 1.0f;
    parameters->mVignettingColorParameter.mValue.g = green;
    registerDepthParameterText(stack, &parameters->mFarMultiplyColorParameter, "far_mul_color",
        &sSetupFarMultiplyColorParameterLabel, &sSetupColorDescription, 0xF8, archive);
    const f32 farAlpha = color->a;
    const f32 farBlue = color->b;
    const f32 farGreen = color->g;
    parameters->mFarMultiplyColorParameter.mValue.b = farBlue;
    parameters->mFarMultiplyColorParameter.mValue.g = farGreen;
    const f32 farRed = color->r;
    parameters->mFarMultiplyColorParameter.mValue.a = farAlpha;
    parameters->mFarMultiplyColorParameter.mValue.r = farRed;
    sead::SafeText* shapeName = &stack->mNameOrDescription;
    shapeName->mText = &sSetupFirstShapeName;
    shapeName->mVtable = &sSetupStringMethods;
    setupVignettingShape(&parameters->mShapes[0], shapeName, archive, owner);
    shapeName->mVtable = &sSetupStringMethods;
    shapeName->mText = &sSetupSecondShapeName;
    setupVignettingShape(&parameters->mShapes[1], shapeName, archive, owner);
    registerDepthParameterText(stack, &parameters->mFarMaximum, "enable_dof_far_max",
        &sSetupFarMaximumLabel, &sSetupBooleanDescription, 0x100, archive);
    parameters->mFarMaximum.mValue = 0;
    registerDepthParameterText(stack, &parameters->mFarMaximumDepth, "dof_far_max",
        &sSetupFarMaximumDepthLabel, &sSetupFarMaximumRange, 0x28, archive);
    parameters->mFarMaximumDepth.mValue = 200.0f;
    registerDepthParameterText(stack, &parameters->mIndirectFromFull, "enable_indirect_from_full",
        &sSetupIndirectFromFullLabel, &sSetupBooleanDescription, 0x108, archive);
    parameters->mIndirectFromFull.mValue = 0;
}

WWHD_SIZE(SetupFrameStrings, 0x108);
WWHD_OFFSET(SetupFrameStrings, mColor, 0x10);
WWHD_OFFSET(SetupFrameStrings, mDescriptions, 0x28);

/* WWHD glue
 data sSetupBooleanDescription 10162610 nocheck
 data sSetupIntegerDescription 10162612 nocheck
 data sSetupFloatDescription 10162613 nocheck
 data sSetupVectorDescription 10162615 nocheck
 data sSetupDepthColorDescription 10162616 nocheck
 data sSetupColorDescription 10162617 nocheck
 data sSetupLevelRange 10162BAC nocheck
 data sSetupFarMaximumRange 10162E98 nocheck
 data sSetupStringMethods 10162618 nocheck
 data sSetupFirstShapeName 10162E30 nocheck
 data sSetupSecondShapeName 10162E44 nocheck
 data sSetupLevelLabel 10162B9C nocheck
 data sSetupStartLabel 10162BBC nocheck
 data sSetupEndLabel 10162BCC nocheck
 data sSetupNearLabel 10162BDC nocheck
 data sSetupFarLabel 10162BF8 nocheck
 data sSetupDepthBlurLabel 10162C14 nocheck
 data sSetupVignettingColorLabel 10162C2C nocheck
 data sSetupVignettingBlurLabel 10162C54 nocheck
 data sSetupVignettingTwoShapesLabel 10162C64 nocheck
 data sSetupColorControlLabel 10162C90 nocheck
 data sSetupColorReverseLabel 10162CBC nocheck
 data sSetupIndirectLabel 10162CCC nocheck
 data sSetupIndirectDepthCancelLabel 10162B3C nocheck
 data sSetupReduceDrawLabel 10162CF8 nocheck
 data sSetupFarStartLabel 10162D0C nocheck
 data sSetupFarEndLabel 10162D20 nocheck
 data sSetupDepthBlurAddLabel 10162D44 nocheck
 data sSetupSaturationMinimumLabel 10162A68 nocheck
 data sSetupColorControlDepthLabel 10162D78 nocheck
 data sSetupIndirectTranslationLabel 10162AD8 nocheck
 data sSetupIndirectRotationLabel 10162D98 nocheck
 data sSetupIndirectScaleLabel 10162B00 nocheck
 data sSetupIndirectStrengthLabel 10162DD4 nocheck
 data sSetupVignettingBlurStrengthLabel 10162DE0 nocheck
 data sSetupVignettingBlendModeLabel 10162E04 nocheck
 data sSetupVignettingColorParameterLabel 10162E24 nocheck
 data sSetupFarMultiplyColorParameterLabel 10162A70 nocheck
 data sSetupFarMaximumLabel 10162E6C nocheck
 data sSetupFarMaximumDepthLabel 10162E88 nocheck
 data sSetupIndirectFromFullLabel 10162EA4 nocheck
 data sSetupDefaultTranslation 104A0908
 data sSetupDefaultScale 104A0920
 str 10162A58 "level"
 str 10162A60 "start"
 str 10162A54 "end"
 str 10162AB0 "near_enable"
 str 10162BEC "far_enable"
 str 10162C08 "depth_blur"
 str 10162A78 "enable_vignetting_color"
 str 10162C3C "enable_vignetting_blur"
 str 10162B20 "enable_vignetting_2_shape"
 str 10162C78 "enable_color_control"
 str 10162CA4 "enable_color_reverse"
 str 10162A90 "indirect_enable"
 str 10162ABC "indirect_depth_cancelenable"
 str 10162CE4 "enable_reduce_draw"
 str 10162B64 "far_start"
 str 10162A4C "far_end"
 str 10162D34 "depth_blur_add"
 str 10162D54 "saturate_min"
 str 10162D64 "color_ctrl_depth"
 str 10162D84 "indirect_tex_trans"
 str 10162AEC "indirect_tex_rotate"
 str 10162DB0 "indirect_tex_scale"
 str 10162DC4 "indirect_scale"
 str 10162AA0 "vignetting_blur"
 str 10162DF0 "vignetting_blend"
 str 10162E10 "vignetting_color"
 str 10162B70 "far_mul_color"
 str 10162E58 "enable_dof_far_max"
 str 10162B14 "dof_far_max"
 str 10162B80 "enable_indirect_from_full"
 ext 0279E618 void registerDepthParameter(void* parameter, sead::SafeText* name, sead::SafeText* label, sead::SafeText* description, void* archive)
 ext 0277DB24 void setupVignettingShape(VignettingShape* shape, sead::SafeText* name, void* archive, u32 owner)
*/
