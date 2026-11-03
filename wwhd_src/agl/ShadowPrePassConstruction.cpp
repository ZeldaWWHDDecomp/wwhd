// Main shadow pre-pass construction; names and labels come from this game's own strings.
#include "agl/ShadowPrePass.h"
#include "agl/ShadowTextInstances.h"
#include "agl/RootNode.h"
#include "bindings.h"
using namespace agl::shadow_prepass;
using sead::SafeText;
static sead::SafeTextVtable sPrePassTextTable;
static u8 sPrePassBaseObjectTable;
static u8 sPrePassFinalObjectTable;
static u8 sPrePassFinalControlTable;
static sead::VtableSlot sPrePassBooleanTable;
static sead::VtableSlot sPrePassIntegerTable;
static sead::VtableSlot sPrePassFloatTable;
static u8 sPrePassBooleanTerminator = 0;
static u8 sPrePassIntegerTerminator = 0;
static u8 sPrePassFloatTerminator = 0;
static void setPrePassText(SafeText*, const char*);
static void initializePrePassBoolean(agl::shadow_parameter::BooleanParameter*, SafeText*, SafeText*, SafeText*, agl::ParameterObject*, u8);
static void initializePrePassInteger(agl::shadow_parameter::IntegerParameter*, SafeText*, SafeText*, SafeText*, agl::ParameterObject*, u32);
static void initializePrePassFloat(agl::shadow_parameter::FloatParameter*, SafeText*, SafeText*, SafeText*, agl::ParameterObject*, f32);
/* 0278DAC0 */
ShadowPrePass* ShadowPrePass::create(ShadowPrePass* shadow) {
    gabi::AutoFrame frame{0x0278DAC0u};
    if (shadow == nullptr) shadow = static_cast<ShadowPrePass*>(operator_new(sizeof(ShadowPrePass)));
    if (shadow == nullptr) return shadow;
    gabi::FrameLocal<SafeText> groupName(8), ioName(0x10);
    gabi::FrameLocal<SafeText> EnabledName(0x18), EnabledHelp(0x20), EnabledValue(0x1A8);
    gabi::FrameLocal<SafeText> LinearOutputName(0x28), LinearOutputHelp(0x30), LinearOutputValue(0x1B0);
    gabi::FrameLocal<SafeText> ResolutionName(0x38), ResolutionHelp(0x40), ResolutionValue(0x1B8);
    gabi::FrameLocal<SafeText> BlurTypeName(0x48), BlurTypeHelp(0x50), BlurTypeValue(0x1C0);
    gabi::FrameLocal<SafeText> BlurWidthName(0x58), BlurWidthHelp(0x60), BlurWidthValue(0x1C8);
    gabi::FrameLocal<SafeText> BlurRepeatName(0x68), BlurRepeatHelp(0x70), BlurRepeatValue(0x1D0);
    gabi::FrameLocal<SafeText> NoiseWidthName(0x78), NoiseWidthHelp(0x80), NoiseWidthValue(0x1D8);
    gabi::FrameLocal<SafeText> EdgeCutoffName(0x88), EdgeCutoffHelp(0x90), EdgeCutoffValue(0x1E0);
    gabi::FrameLocal<SafeText> EdgeThresholdName(0x98), EdgeThresholdHelp(0xA0), EdgeThresholdValue(0x1E8);
    gabi::FrameLocal<SafeText> EdgeRangeName(0xA8), EdgeRangeHelp(0xB0), EdgeRangeValue(0x1F0);
    gabi::FrameLocal<SafeText> StaticDepthName(0xB8), StaticDepthHelp(0xC0), StaticDepthValue(0x1F8);
    gabi::FrameLocal<SafeText> DecalAoName(0xC8), DecalAoHelp(0xD0), DecalAoValue(0x200);
    gabi::FrameLocal<SafeText> FarFadeName(0xD8), FarFadeHelp(0xE0), FarFadeValue(0x208);
    gabi::FrameLocal<SafeText> FarFadeStartName(0xE8), FarFadeStartHelp(0xF0), FarFadeStartValue(0x210);
    gabi::FrameLocal<SafeText> FarFadeEndName(0xF8), FarFadeEndHelp(0x100), FarFadeEndValue(0x218);
    gabi::FrameLocal<SafeText> GenerateNormalsName(0x108), GenerateNormalsHelp(0x110), GenerateNormalsValue(0x220);
    gabi::FrameLocal<SafeText> DepthToNormalBlurName(0x118), DepthToNormalBlurHelp(0x120), DepthToNormalBlurValue(0x228);
    gabi::FrameLocal<SafeText> NormalShadowRatioName(0x128), NormalShadowRatioHelp(0x130), NormalShadowRatioValue(0x230);
    gabi::FrameLocal<SafeText> NormalShadowMultiplierName(0x138), NormalShadowMultiplierHelp(0x140), NormalShadowMultiplierValue(0x238);
    gabi::FrameLocal<SafeText> MipBlurName(0x148), MipBlurHelp(0x150), MipBlurValue(0x240);
    gabi::FrameLocal<SafeText> MipBlurReduceName(0x158), MipBlurReduceHelp(0x160), MipBlurReduceValue(0x248);
    gabi::FrameLocal<SafeText> MipBlurWidthName(0x168), MipBlurWidthHelp(0x170), MipBlurWidthValue(0x250);
    gabi::FrameLocal<SafeText> MipBlurRepeatName(0x178), MipBlurRepeatHelp(0x180), MipBlurRepeatValue(0x258);
    gabi::FrameLocal<SafeText> CombineSsaoName(0x188), CombineSsaoHelp(0x190), CombineSsaoValue(0x260);
    gabi::FrameLocal<SafeText> SsaoDirectDrawName(0x198), SsaoDirectDrawHelp(0x1A0), SsaoDirectDrawValue(0x268);
    shadow->mObjectTable = &sPrePassBaseObjectTable;
    setPrePassText(ioName.get(), "aglshpp");
    agl::ParameterRoot* io = &shadow->mRoot;
    agl::ParameterRoot::create(io, ioName.get(), 0);
    shadow->mObjectTable = &sPrePassFinalObjectTable;
    shadow->mRoot.mTree.mTable = gabi::ea(&sPrePassFinalControlTable);
    shadow->mFrames = nullptr;
    shadow->mFrameCount = 0;
    shadow->mRegistryQuery.mValue = 0;
    agl::ResourceRecord::create(&shadow->mParameterState.mRecord);
    agl::ParameterObject* parameters = &shadow->mParameterState.mObject;
    agl::ParameterObject::create(parameters);
    setPrePassText(EnabledName.get(), "is_enable");
    setPrePassText(EnabledHelp.get(), "有効");
    initializePrePassBoolean(&shadow->mParameters.mEnabled, EnabledName.get(), EnabledHelp.get(), EnabledValue.get(), parameters, 1);
    setPrePassText(LinearOutputName.get(), "is_outputLinearSampler");
    setPrePassText(LinearOutputHelp.get(), "バイリニア出力");
    initializePrePassBoolean(&shadow->mParameters.mLinearOutput, LinearOutputName.get(), LinearOutputHelp.get(), LinearOutputValue.get(), parameters, 1);
    setPrePassText(ResolutionName.get(), "resolutionMode");
    setPrePassText(ResolutionHelp.get(), "バッファ解像度");
    initializePrePassInteger(&shadow->mParameters.mResolution, ResolutionName.get(), ResolutionHelp.get(), ResolutionValue.get(), parameters, 0);
    setPrePassText(BlurTypeName.get(), "screenSpaceBlurType");
    setPrePassText(BlurTypeHelp.get(), "ブラータイプ");
    initializePrePassInteger(&shadow->mParameters.mBlurType, BlurTypeName.get(), BlurTypeHelp.get(), BlurTypeValue.get(), parameters, 0);
    setPrePassText(BlurWidthName.get(), "screenSpaceBlurWidth");
    setPrePassText(BlurWidthHelp.get(), "ブラーぼかし幅");
    initializePrePassInteger(&shadow->mParameters.mBlurWidth, BlurWidthName.get(), BlurWidthHelp.get(), BlurWidthValue.get(), parameters, 0);
    setPrePassText(BlurRepeatName.get(), "mScreenSpaceBlurRepNum");
    setPrePassText(BlurRepeatHelp.get(), "ぼかし回数");
    initializePrePassInteger(&shadow->mParameters.mBlurRepeat, BlurRepeatName.get(), BlurRepeatHelp.get(), BlurRepeatValue.get(), parameters, 1);
    setPrePassText(NoiseWidthName.get(), "noiseWidth");
    setPrePassText(NoiseWidthHelp.get(), "ぼかしノイズ幅");
    initializePrePassFloat(&shadow->mParameters.mNoiseWidth, NoiseWidthName.get(), NoiseWidthHelp.get(), NoiseWidthValue.get(), parameters, 0.0f);
    setPrePassText(EdgeCutoffName.get(), "is_edgeCutoff");
    setPrePassText(EdgeCutoffHelp.get(), "エッジ2値化を使用");
    initializePrePassBoolean(&shadow->mParameters.mEdgeCutoff, EdgeCutoffName.get(), EdgeCutoffHelp.get(), EdgeCutoffValue.get(), parameters, 0);
    setPrePassText(EdgeThresholdName.get(), "edgeCutoffThreshold");
    setPrePassText(EdgeThresholdHelp.get(), "エッジ2値化閾値");
    initializePrePassFloat(&shadow->mParameters.mEdgeThreshold, EdgeThresholdName.get(), EdgeThresholdHelp.get(), EdgeThresholdValue.get(), parameters, 0.5f);
    setPrePassText(EdgeRangeName.get(), "edgeCutoffRange");
    setPrePassText(EdgeRangeHelp.get(), "エッジ2値化ぼけ範囲");
    initializePrePassFloat(&shadow->mParameters.mEdgeRange, EdgeRangeName.get(), EdgeRangeHelp.get(), EdgeRangeValue.get(), parameters, 0.1f);
    setPrePassText(StaticDepthName.get(), "is_useStaticDepthShadow");
    setPrePassText(StaticDepthHelp.get(), "静的デプスシャドウを使用");
    initializePrePassBoolean(&shadow->mParameters.mStaticDepth, StaticDepthName.get(), StaticDepthHelp.get(), StaticDepthValue.get(), parameters, 0);
    setPrePassText(DecalAoName.get(), "is_useDecalAo");
    setPrePassText(DecalAoHelp.get(), "デカールAOを使用");
    initializePrePassBoolean(&shadow->mParameters.mDecalAo, DecalAoName.get(), DecalAoHelp.get(), DecalAoValue.get(), parameters, 0);
    setPrePassText(FarFadeName.get(), "is_useFarFade");
    setPrePassText(FarFadeHelp.get(), "遠距離フェードを使用");
    initializePrePassBoolean(&shadow->mParameters.mFarFade, FarFadeName.get(), FarFadeHelp.get(), FarFadeValue.get(), parameters, 0);
    setPrePassText(FarFadeStartName.get(), "farFadeStart");
    setPrePassText(FarFadeStartHelp.get(), "遠距離フェード開始位置");
    initializePrePassFloat(&shadow->mParameters.mFarFadeStart, FarFadeStartName.get(), FarFadeStartHelp.get(), FarFadeStartValue.get(), parameters, 100.0f);
    setPrePassText(FarFadeEndName.get(), "farFadeEnd");
    setPrePassText(FarFadeEndHelp.get(), "遠距離フェード終了位置");
    initializePrePassFloat(&shadow->mParameters.mFarFadeEnd, FarFadeEndName.get(), FarFadeEndHelp.get(), FarFadeEndValue.get(), parameters, 1000.0f);
    setPrePassText(GenerateNormalsName.get(), "is_useDepth2Normal");
    setPrePassText(GenerateNormalsHelp.get(), "デプスから法線を生成");
    initializePrePassBoolean(&shadow->mParameters.mGenerateNormals, GenerateNormalsName.get(), GenerateNormalsHelp.get(), GenerateNormalsValue.get(), parameters, 0);
    setPrePassText(DepthToNormalBlurName.get(), "is_useDepth2NormalBlur");
    setPrePassText(DepthToNormalBlurHelp.get(), "生成した法線を一緒にぼかす");
    initializePrePassBoolean(&shadow->mParameters.mDepthToNormalBlur, DepthToNormalBlurName.get(), DepthToNormalBlurHelp.get(), DepthToNormalBlurValue.get(), parameters, 0);
    setPrePassText(NormalShadowRatioName.get(), "normal2ShadowRatio");
    setPrePassText(NormalShadowRatioHelp.get(), "法線をシャドウに反映する角度");
    initializePrePassFloat(&shadow->mParameters.mNormalShadowRatio, NormalShadowRatioName.get(), NormalShadowRatioHelp.get(), NormalShadowRatioValue.get(), parameters, 0.0f);
    setPrePassText(NormalShadowMultiplierName.get(), "normal2ShadowMul");
    setPrePassText(NormalShadowMultiplierHelp.get(), "法線をシャドウに反映する影濃度");
    initializePrePassFloat(&shadow->mParameters.mNormalShadowMultiplier, NormalShadowMultiplierName.get(), NormalShadowMultiplierHelp.get(), NormalShadowMultiplierValue.get(), parameters, 1.0f);
    setPrePassText(MipBlurName.get(), "is_useMipLevelBlur");
    setPrePassText(MipBlurHelp.get(), "ミップマップにぼけた絵を生成");
    initializePrePassBoolean(&shadow->mParameters.mMipBlur, MipBlurName.get(), MipBlurHelp.get(), MipBlurValue.get(), parameters, 0);
    setPrePassText(MipBlurReduceName.get(), "is_useMipLevelBlurReduce");
    setPrePassText(MipBlurReduceHelp.get(), "ミップマップのぼけを縮小して生成");
    initializePrePassBoolean(&shadow->mParameters.mMipBlurReduce, MipBlurReduceName.get(), MipBlurReduceHelp.get(), MipBlurReduceValue.get(), parameters, 0);
    setPrePassText(MipBlurWidthName.get(), "mipBlurWidth");
    setPrePassText(MipBlurWidthHelp.get(), "ミップマップのぼかし幅");
    initializePrePassInteger(&shadow->mParameters.mMipBlurWidth, MipBlurWidthName.get(), MipBlurWidthHelp.get(), MipBlurWidthValue.get(), parameters, 0);
    setPrePassText(MipBlurRepeatName.get(), "mipBlurRepNum");
    setPrePassText(MipBlurRepeatHelp.get(), "ミップマップのぼかし回数");
    initializePrePassInteger(&shadow->mParameters.mMipBlurRepeat, MipBlurRepeatName.get(), MipBlurRepeatHelp.get(), MipBlurRepeatValue.get(), parameters, 1);
    setPrePassText(CombineSsaoName.get(), "is_usePreCombSsao");
    setPrePassText(CombineSsaoHelp.get(), "SSAOを先に統合して一緒にぼかす");
    initializePrePassBoolean(&shadow->mParameters.mCombineSsao, CombineSsaoName.get(), CombineSsaoHelp.get(), CombineSsaoValue.get(), parameters, 0);
    setPrePassText(SsaoDirectDrawName.get(), "is_useSsaoDirectDraw");
    setPrePassText(SsaoDirectDrawHelp.get(), "SSAOを外部でバッファに直接描画");
    initializePrePassBoolean(&shadow->mParameters.mSsaoDirectDraw, SsaoDirectDrawName.get(), SsaoDirectDrawHelp.get(), SsaoDirectDrawValue.get(), parameters, 0);
    if ((gabi::ea(shadow) + static_cast<u32>(offsetof(ShadowPrePass, mColor))) == 0)
        operator_new(sizeof(sead::RenderQuaternion));
    setPrePassText(groupName.get(), "ShadowPrePass");
    io->mTree.appendValue(parameters, groupName.get());
    setPrePassText(groupName.get(), "シャドウプリパス");
    auto* text = reinterpret_cast<agl::shadow_prepass_text::Text*>(groupName.get());
    text->destroy(2);
    text = agl::shadow_prepass_text::Text::create(text, gabi::guest_str("Icon=EFFECT"));
    agl::detail::RootNode::setNodeMeta(shadow, text);
    reinterpret_cast<agl::shadow_prepass_text::Text*>(groupName.get())->destroy(2);
    void* enabled = shadow->mParameters.mEnabled.mBase.valueAddress();
    *static_cast<be<u8>*>(enabled) = 1;
    shadow->mFrameSequence = 0;
    shadow->mSwapIndex = 0;
    shadow->mFrameToken = 0;
    return shadow;
}

// Repeated inlined parameter operations.
static void setPrePassText(SafeText* text, const char* content) {
    text->mVtable = &sPrePassTextTable;
    text->mText.v = gabi::guest_str(content);
}
static void initializePrePassBoolean(agl::shadow_parameter::BooleanParameter* parameter, SafeText* name,
                                    SafeText* help, SafeText* value, agl::ParameterObject* parent, u8 initial) {
    if (parameter == nullptr) parameter = static_cast<agl::shadow_parameter::BooleanParameter*>(operator_new(sizeof(*parameter)));
    if (parameter == nullptr) return;
    agl::ParameterBase::create(&parameter->mBase);
    parameter->mBase.mMethods = &sPrePassBooleanTable;
    value->mVtable = &sPrePassTextTable;
    value->mText = &sPrePassBooleanTerminator;
    parameter->mBase.initialize(name, help, value, parent);
    parameter->mValue = initial;
}
static void initializePrePassInteger(agl::shadow_parameter::IntegerParameter* parameter, SafeText* name,
                                    SafeText* help, SafeText* value, agl::ParameterObject* parent, u32 initial) {
    if (parameter == nullptr) parameter = static_cast<agl::shadow_parameter::IntegerParameter*>(operator_new(sizeof(*parameter)));
    if (parameter == nullptr) return;
    agl::ParameterBase::create(&parameter->mBase);
    parameter->mBase.mMethods = &sPrePassIntegerTable;
    value->mVtable = &sPrePassTextTable;
    value->mText = &sPrePassIntegerTerminator;
    parameter->mBase.initialize(name, help, value, parent);
    parameter->mValue = initial;
}
static void initializePrePassFloat(agl::shadow_parameter::FloatParameter* parameter, SafeText* name,
                                    SafeText* help, SafeText* value, agl::ParameterObject* parent, f32 initial) {
    if (parameter == nullptr) parameter = static_cast<agl::shadow_parameter::FloatParameter*>(operator_new(sizeof(*parameter)));
    if (parameter == nullptr) return;
    agl::ParameterBase::create(&parameter->mBase);
    parameter->mBase.mMethods = &sPrePassFloatTable;
    value->mVtable = &sPrePassTextTable;
    value->mText = &sPrePassFloatTerminator;
    parameter->mBase.initialize(name, help, value, parent);
    parameter->mValue = initial;
}

/* WWHD glue
data sPrePassTextTable 101658B4
data sPrePassBaseObjectTable 101658DC
data sPrePassFinalObjectTable 10165FC4
data sPrePassFinalControlTable 10165FDC
data sPrePassBooleanTable 1016594C
data sPrePassIntegerTable 101659C4
data sPrePassFloatTable 10165A3C
data sPrePassBooleanTerminator 101658B0
data sPrePassIntegerTerminator 101658B1
data sPrePassFloatTerminator 101658B2
str 10165ACC "aglshpp"
str 10165BD0 "ShadowPrePass"
str 10165F0C "シャドウプリパス"
str 10165B50 "Icon=EFFECT"
str 10165B5C "is_enable"
str 10165AD4 "有効"
str 10165BE0 "is_outputLinearSampler"
str 10165BF8 "バイリニア出力"
str 10165C08 "resolutionMode"
str 10165C18 "バッファ解像度"
str 10165B14 "screenSpaceBlurType"
str 10165C28 "ブラータイプ"
str 10165C38 "screenSpaceBlurWidth"
str 10165C50 "ブラーぼかし幅"
str 10165C60 "mScreenSpaceBlurRepNum"
str 10165C78 "ぼかし回数"
str 10165C84 "noiseWidth"
str 10165C90 "ぼかしノイズ幅"
str 10165B68 "is_edgeCutoff"
str 10165B78 "エッジ2値化を使用"
str 10165B28 "edgeCutoffThreshold"
str 10165ADC "エッジ2値化閾値"
str 10165AEC "edgeCutoffRange"
str 10165B3C "エッジ2値化ぼけ範囲"
str 10165AFC "is_useStaticDepthShadow"
str 10165CA0 "静的デプスシャドウを使用"
str 10165B8C "is_useDecalAo"
str 10165CBC "デカールAOを使用"
str 10165B9C "is_useFarFade"
str 10165CD0 "遠距離フェードを使用"
str 10165CE8 "farFadeStart"
str 10165CF8 "遠距離フェード開始位置"
str 10165D10 "farFadeEnd"
str 10165D1C "遠距離フェード終了位置"
str 10165D34 "is_useDepth2Normal"
str 10165D48 "デプスから法線を生成"
str 10165D60 "is_useDepth2NormalBlur"
str 10165D78 "生成した法線を一緒にぼかす"
str 10165D94 "normal2ShadowRatio"
str 10165DA8 "法線をシャドウに反映する角度"
str 10165DC8 "normal2ShadowMul"
str 10165DDC "法線をシャドウに反映する影濃度"
str 10165DFC "is_useMipLevelBlur"
str 10165E10 "ミップマップにぼけた絵を生成"
str 10165E30 "is_useMipLevelBlurReduce"
str 10165E4C "ミップマップのぼけを縮小して生成"
str 10165E70 "mipBlurWidth"
str 10165E80 "ミップマップのぼかし幅"
str 10165BAC "mipBlurRepNum"
str 10165E98 "ミップマップのぼかし回数"
str 10165BBC "is_usePreCombSsao"
str 10165EB4 "SSAOを先に統合して一緒にぼかす"
str 10165ED4 "is_useSsaoDirectDraw"
str 10165EEC "SSAOを外部でバッファに直接描画"
*/
