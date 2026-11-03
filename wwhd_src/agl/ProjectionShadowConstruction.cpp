// Projection-shadow construction, with every SafeText at its original frame offset.
#include "agl/ProjectionShadow.h"
#include "bindings.h"
using namespace agl::projection_shadow;
using sead::SafeText;
static sead::SafeTextVtable sConstructionTextTable;
static u8 sConstructionBaseTable;
static u8 sConstructionObjectTable;
static u8 sConstructionControlTable;
static agl::parameter_tree::ValueVtable sConstructionLinkTable;
static sead::VtableSlot sConstructionPairTable;
static sead::VtableSlot sConstructionIntegerTable;
static sead::VtableSlot sConstructionFloatTable;
static sead::VtableSlot sConstructionBooleanTable;
static u8 sPairTerminator = 0;
static u8 sIntegerTerminator = 0;
static u8 sFloatTerminator = 0;
static u8 sBooleanTerminator = 0;
// Preserve exact floating-point payloads through typed big-endian words.
static u32 readFloatWord(const be<f32>& field) {
    return static_cast<u32>(reinterpret_cast<const be<u32>&>(field));
}
static void writeFloatWord(be<f32>& field, u32 bits) {
    reinterpret_cast<be<u32>&>(field) = bits;
}
static Pair2 sConstructionDefaultScale;
static Pair2 sConstructionDefaultPair;
static void setConstructionText(SafeText*, const char*);
static void initializePairParameter(PairParameter*, SafeText*, SafeText*, SafeText*, agl::ParameterObject*, const Pair2*);
static void initializeIntegerParameter(agl::shadow_parameter::IntegerParameter*, SafeText*, SafeText*, SafeText*, agl::ParameterObject*);
static void initializeFloatParameter(agl::shadow_parameter::FloatParameter*, SafeText*, SafeText*, SafeText*, agl::ParameterObject*);
static void initializeBooleanParameter(agl::shadow_parameter::BooleanParameter*, SafeText*, SafeText*, SafeText*, agl::ParameterObject*);

/* 0279207C */
ProjectionShadow* ProjectionShadow::create(ProjectionShadow* shadow) {
    gabi::AutoFrame frame{0x0279207Cu};
    if (shadow == nullptr) shadow = static_cast<ProjectionShadow*>(operator_new(sizeof(ProjectionShadow)));
    if (shadow == nullptr) return shadow;
    gabi::FrameLocal<SafeText> ioName(8), groupName(0xA0);
    gabi::FrameLocal<SafeText> scaleName(0x10), scaleHelp(0x18), scaleValue(0xA8);
    gabi::FrameLocal<SafeText> velocityName(0x20), velocityHelp(0x28), velocityValue(0xB0);
    gabi::FrameLocal<SafeText> cyclexName(0x30), cyclexHelp(0x38), cyclexValue(0xB8);
    gabi::FrameLocal<SafeText> cycleyName(0x40), cycleyHelp(0x48), cycleyValue(0xC0);
    gabi::FrameLocal<SafeText> amplitudeName(0x50), amplitudeHelp(0x58), amplitudeValue(0xC8);
    gabi::FrameLocal<SafeText> translationName(0x60), translationHelp(0x68), translationValue(0xD0);
    gabi::FrameLocal<SafeText> speedName(0x70), speedHelp(0x78), speedValue(0xD8);
    gabi::FrameLocal<SafeText> rotationName(0x80), rotationHelp(0x88), rotationValue(0xE0);
    gabi::FrameLocal<SafeText> repeatName(0x90), repeatHelp(0x98), repeatValue(0xE8);
    shadow->mObjectTable = &sConstructionBaseTable;
    setConstructionText(ioName.get(), "aglprojsdw");
    agl::ParameterRoot* io = &shadow->mRoot;
    agl::ParameterRoot::create(io, ioName.get(), 0);
    shadow->mObjectTable = &sConstructionObjectTable;
    shadow->mRoot.mTree.mTable = gabi::ea(&sConstructionControlTable);
    agl::TextureSampler::create(&shadow->mTexture);
    if ((gabi::ea(shadow) + static_cast<u32>(offsetof(ProjectionShadow, mBiasMatrix))) == 0)
        operator_new(sizeof(Matrix34));
    if ((gabi::ea(shadow) + static_cast<u32>(offsetof(ProjectionShadow, mAnimationOffset))) == 0)
        operator_new(sizeof(Pair2));
    if ((gabi::ea(shadow) + static_cast<u32>(offsetof(ProjectionShadow, mSwingOffset))) == 0)
        operator_new(sizeof(Pair2));
    if ((gabi::ea(shadow) + static_cast<u32>(offsetof(ProjectionShadow, mTranslationOffset))) == 0)
        operator_new(sizeof(Pair2));
    agl::ParameterObject::create(&shadow->mParameterContainer);
    shadow->mParameterIoOwner = io;
    shadow->mParameterContainer.mVtable = &sConstructionLinkTable;
    setConstructionText(scaleName.get(), "bias_scale");
    setConstructionText(scaleHelp.get(), "テクスチャSRT Scale");
    initializePairParameter(&shadow->mScaleBias, scaleName.get(), scaleHelp.get(), scaleValue.get(), &shadow->mParameterContainer, &sConstructionDefaultScale);
    setConstructionText(velocityName.get(), "anim_trans_vel");
    setConstructionText(velocityHelp.get(), "スクロールアニメーション");
    initializePairParameter(&shadow->mAnimationVelocity, velocityName.get(), velocityHelp.get(), velocityValue.get(), &shadow->mParameterContainer, &sConstructionDefaultPair);
    setConstructionText(cyclexName.get(), "anim_swing_cyc_x");
    setConstructionText(cyclexHelp.get(), "揺れアニメーション周期X");
    initializeIntegerParameter(&shadow->mSwingCycleX, cyclexName.get(), cyclexHelp.get(), cyclexValue.get(), &shadow->mParameterContainer);
    setConstructionText(cycleyName.get(), "anim_swing_cyc_y");
    setConstructionText(cycleyHelp.get(), "揺れアニメーション周期Y");
    initializeIntegerParameter(&shadow->mSwingCycleY, cycleyName.get(), cycleyHelp.get(), cycleyValue.get(), &shadow->mParameterContainer);
    setConstructionText(amplitudeName.get(), "anim_swing_amp");
    setConstructionText(amplitudeHelp.get(), "揺れアニメーション幅");
    initializePairParameter(&shadow->mSwingAmplitude, amplitudeName.get(), amplitudeHelp.get(), amplitudeValue.get(), &shadow->mParameterContainer, &sConstructionDefaultPair);
    setConstructionText(translationName.get(), "bias_trans");
    setConstructionText(translationHelp.get(), "テクスチャSRT Trans");
    initializePairParameter(&shadow->mTranslationBias, translationName.get(), translationHelp.get(), translationValue.get(), &shadow->mParameterContainer, &sConstructionDefaultPair);
    setConstructionText(speedName.get(), "anim_rot_speed");
    setConstructionText(speedHelp.get(), "回転アニメーション");
    initializeFloatParameter(&shadow->mRotationSpeed, speedName.get(), speedHelp.get(), speedValue.get(), &shadow->mParameterContainer);
    setConstructionText(rotationName.get(), "bias_rotate");
    setConstructionText(rotationHelp.get(), "テクスチャSRT Rotate");
    initializeFloatParameter(&shadow->mRotationBias, rotationName.get(), rotationHelp.get(), rotationValue.get(), &shadow->mParameterContainer);
    setConstructionText(repeatName.get(), "repeat");
    setConstructionText(repeatHelp.get(), "Repeat");
    initializeBooleanParameter(&shadow->mRepeat, repeatName.get(), repeatHelp.get(), repeatValue.get(), &shadow->mParameterContainer);
    shadow->mRotationSpeedDegrees = 0.0f;
    shadow->mAnimationEnabled = 0;
    shadow->mRotationBiasDegrees = 0.0f;
    setConstructionText(groupName.get(), "projection_shadow");
    io->mTree.appendValue(&shadow->mParameterContainer, groupName.get());
    u8 repeat = shadow->mRepeat.mValue;
    u8 dirty = shadow->mTexture.dirty;
    u32 addressMode = repeat ? 0 : 2;
    shadow->mTexture.settings.modes[5] = addressMode;
    dirty |= 6;
    shadow->mTexture.settings.modes[4] = addressMode;
    shadow->mTexture.dirty = dirty;
    shadow->mTexture.settings.modes[1] = 1;
    shadow->mTexture.settings.modes[2] = 1;
    shadow->mTexture.settings.modes[0] = 1;
    shadow->mTexture.settings.modes[3] = addressMode;
    shadow->resetAnimation();
    return shadow;
}

// These helpers are the constructor's repeated inlined parameter operations.
static void setConstructionText(SafeText* text, const char* content) {
    text->mVtable = &sConstructionTextTable;
    text->mText.v = gabi::guest_str(content);
}
static void initializePairParameter(PairParameter* parameter, SafeText* name, SafeText* help,
                                    SafeText* value, agl::ParameterObject* parent, const Pair2* defaults) {
    if (parameter == nullptr) parameter = static_cast<PairParameter*>(operator_new(sizeof(PairParameter)));
    if (parameter == nullptr) return;
    agl::ParameterBase::create(&parameter->mBase);
    parameter->mBase.mMethods = &sConstructionPairTable;
    value->mVtable = &sConstructionTextTable;
    value->mText = &sPairTerminator;
    parameter->mBase.initialize(name, help, value, parent);
    writeFloatWord(parameter->mValue.mX, readFloatWord(defaults->mX));
    writeFloatWord(parameter->mValue.mY, readFloatWord(defaults->mY));
}
static void initializeIntegerParameter(agl::shadow_parameter::IntegerParameter* parameter,
                                      SafeText* name, SafeText* help, SafeText* value, agl::ParameterObject* parent) {
    if (parameter == nullptr) parameter = static_cast<agl::shadow_parameter::IntegerParameter*>(operator_new(sizeof(*parameter)));
    if (parameter == nullptr) return;
    agl::ParameterBase::create(&parameter->mBase);
    parameter->mBase.mMethods = &sConstructionIntegerTable;
    value->mVtable = &sConstructionTextTable;
    value->mText = &sIntegerTerminator;
    parameter->mBase.initialize(name, help, value, parent);
    parameter->mValue = 0;
}
static void initializeFloatParameter(agl::shadow_parameter::FloatParameter* parameter,
                                      SafeText* name, SafeText* help, SafeText* value, agl::ParameterObject* parent) {
    if (parameter == nullptr) parameter = static_cast<agl::shadow_parameter::FloatParameter*>(operator_new(sizeof(*parameter)));
    if (parameter == nullptr) return;
    agl::ParameterBase::create(&parameter->mBase);
    parameter->mBase.mMethods = &sConstructionFloatTable;
    value->mVtable = &sConstructionTextTable;
    value->mText = &sFloatTerminator;
    parameter->mBase.initialize(name, help, value, parent);
    parameter->mValue = 0.0f;
}
static void initializeBooleanParameter(agl::shadow_parameter::BooleanParameter* parameter,
                                      SafeText* name, SafeText* help, SafeText* value, agl::ParameterObject* parent) {
    if (parameter == nullptr) parameter = static_cast<agl::shadow_parameter::BooleanParameter*>(operator_new(sizeof(*parameter)));
    if (parameter == nullptr) return;
    agl::ParameterBase::create(&parameter->mBase);
    parameter->mBase.mMethods = &sConstructionBooleanTable;
    value->mVtable = &sConstructionTextTable;
    value->mText = &sBooleanTerminator;
    parameter->mBase.initialize(name, help, value, parent);
    parameter->mValue = 0;
}

/* WWHD glue
data sConstructionTextTable 1016605C
data sConstructionBaseTable 10166084
data sConstructionObjectTable 10166478
data sConstructionControlTable 10166498
data sConstructionLinkTable 101660D4
data sConstructionPairTable 1016627C
data sConstructionIntegerTable 1016618C
data sConstructionFloatTable 10166204
data sConstructionBooleanTable 10166114
data sPairTerminator 1016605B
data sIntegerTerminator 10166059
data sFloatTerminator 1016605A
data sBooleanTerminator 10166058
data sConstructionDefaultScale 104A0920
data sConstructionDefaultPair 104A0908
str 10166380 "aglprojsdw"
str 1016636C "projection_shadow"
str 1016638C "bias_scale"
str 10166338 "テクスチャSRT Scale"
str 10166398 "anim_trans_vel"
str 101663A8 "スクロールアニメーション"
str 101663C4 "anim_swing_cyc_x"
str 10166308 "揺れアニメーション周期X"
str 101663D8 "anim_swing_cyc_y"
str 10166320 "揺れアニメーション周期Y"
str 101663EC "anim_swing_amp"
str 101663FC "揺れアニメーション幅"
str 10166414 "bias_trans"
str 1016634C "テクスチャSRT Trans"
str 10166420 "anim_rot_speed"
str 10166430 "回転アニメーション"
str 10166360 "bias_rotate"
str 10166444 "テクスチャSRT Rotate"
str 101662F8 "repeat"
str 10166300 "Repeat"
*/
