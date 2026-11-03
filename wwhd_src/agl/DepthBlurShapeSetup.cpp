// Vignetting-shape parameter registration, from this game's names and call sequence.
#include "bindings.h"
#include "agl/DepthBlurValues.h"
#include "agl/DepthOfFieldDraw.h"
using namespace agl::depth_blur_value;

// Text bytes of the labels and descriptions (guest strings bound by the data lines).
typedef be<u8> TextByte;
static sead::SafeTextVtable sShapeStringMethods;
static TextByte sShapeIntegerDescription;
static TextByte sShapeVectorDescription;
static TextByte sShapeTypeLabel;
static TextByte sShapeRangeLabel;
static TextByte sShapeScaleLabel;
static TextByte sShapeTranslationLabel;
// agl::dof::FormatText32::create (AGL#5): a 32-character formatted name; these calls pass one argument.
static agl::dof::FormatText32* formatShapeName(agl::dof::FormatText32* destination, const char* format, const be<u8>* name);
static void registerShapeParameter(void* parameter, sead::SafeText* name, sead::SafeText* label,
    sead::SafeText* description, void* archive);

/* 0277DB24 */
static void registerDepthBlurShape(VignettingShape* shape, sead::SafeText* name, void* archive) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::SafeText> label(8);
    gabi::FrameLocal<sead::SafeText> integerDescription(0x10);
    gabi::FrameLocal<sead::SafeText> rangeDescription(0x18);
    gabi::FrameLocal<sead::SafeText> scaleDescription(0x20);
    gabi::FrameLocal<sead::SafeText> translationDescription(0x28);
    gabi::FrameLocal<agl::dof::FormatText32> formatted(0x30);

    name->assureTermination();
    label->mVtable = &sShapeStringMethods;
    label->mText = &sShapeTypeLabel;
    agl::dof::FormatText32* result = formatShapeName(formatted, "%s_type", name->mText);
    integerDescription->mVtable = &sShapeStringMethods;
    integerDescription->mText = &sShapeIntegerDescription;
    registerShapeParameter(&shape->mType, result, label, integerDescription, archive);
    shape->mType.mValue = 0;

    name->assureTermination();
    label->mVtable = &sShapeStringMethods;
    label->mText = &sShapeRangeLabel;
    result = formatShapeName(formatted, "%s_range", name->mText);
    rangeDescription->mVtable = &sShapeStringMethods;
    rangeDescription->mText = &sShapeVectorDescription;
    registerShapeParameter(&shape->mRange, result, label, rangeDescription, archive);
    shape->mRange.x = 0.25f;
    shape->mRange.y = 1.0f;

    name->assureTermination();
    label->mVtable = &sShapeStringMethods;
    label->mText = &sShapeScaleLabel;
    result = formatShapeName(formatted, "%s_scale", name->mText);
    scaleDescription->mVtable = &sShapeStringMethods;
    scaleDescription->mText = &sShapeVectorDescription;
    registerShapeParameter(&shape->mScale, result, label, scaleDescription, archive);
    shape->mScale.y = 1.0f;
    shape->mScale.x = 1.0f;

    name->assureTermination();
    label->mVtable = &sShapeStringMethods;
    label->mText = &sShapeTranslationLabel;
    result = formatShapeName(formatted, "%s_trans", name->mText);
    translationDescription->mVtable = &sShapeStringMethods;
    translationDescription->mText = &sShapeVectorDescription;
    registerShapeParameter(&shape->mTranslation, result, label, translationDescription, archive);
    shape->mTranslation.x = 0.0f;
    shape->mTranslation.y = 0.0f;
}

WWHD_SIZE(agl::dof::FormatText32, 0x2C);
/* WWHD glue
 data sShapeStringMethods 10162618 nocheck
 data sShapeIntegerDescription 10162612 nocheck
 data sShapeVectorDescription 10162615 nocheck
 data sShapeTypeLabel 101629D8 nocheck
 data sShapeRangeLabel 101629E0 nocheck
 data sShapeScaleLabel 10162A00 nocheck
 data sShapeTranslationLabel 10162A18 nocheck
 str 101629D0 "%s_type"
 str 101629E8 "%s_range"
 str 101629F4 "%s_scale"
 str 10162A0C "%s_trans"
 ext 027828DC agl::dof::FormatText32* formatShapeName(agl::dof::FormatText32* destination, const char* format, const be<u8>* name)
 ext 0279E618 void registerShapeParameter(void* parameter, sead::SafeText* name, sead::SafeText* label, sead::SafeText* description, void* archive)
*/
