// agl colour correction (parameter root "aglccr", shader programs "color_correction" and its map pass).
// Layouts from the class's own constructor (0x14E4 bytes), destructor and draw routines in WWHD.
#pragma once
#include "agl/ParameterRoot.h"
#include "agl/ParameterValue.h"
#include "agl/CurveParameter.h"
#include "agl/TextureSampler.h"
#include "agl/RenderBuffer.h"
#include "agl/ShaderProgram.h"
#include "agl/ShaderHolder.h"
#include "agl/ShaderProgramActivation.h"
#include "agl/PrimitiveShapes.h"
#include "agl/DynamicTextureCache.h"
#include "agl/TextureResource.h"
#include "sead/RenderState.h"
#include "sead/ColorChannels.h"
#include "sead/Viewport.h"

namespace agl { namespace color_correction {

// The three parameter value kinds the root registers (prefix: name hash, list link, methods).
struct BoolParameter {
    env::ParameterBinding mBinding;
    be<u8> mValue;
    u8 mPadding[3];
    u32 typeCode();
    be<u8>* valueAddress();
    be<u8>* alternateValueAddress();
    u32 byteSize();
};
struct FloatParameter {
    env::ParameterBinding mBinding;
    be<f32> mValue;
    u32 typeCode();
    be<f32>* valueAddress();
    be<f32>* alternateValueAddress();
    u32 byteSize();
};
struct ColorParameter {
    env::ParameterBinding mBinding;
    sead::ColorChannels mValue;
    u32 typeCode();
    sead::ColorChannels* valueAddress();
    sead::ColorChannels* alternateValueAddress();
    u32 byteSize();
};
// Shared no-op slot of the three value kinds above.
struct ParameterValueHooks { void emptyHook(); };

// This translation unit's copies of the parameter object and parameter tree node callbacks.
struct ParameterObjectHooks : parameter_tree::Value {
    u32 constantOneHook();
    void emptyHook();
    u32 alternateConstantOneHook();
    void alternateEmptyHook();
    u32 matchesRecord(const gptr<parameter_tree::MatchRecord>* record);
};
struct ParameterNodeHooks : parameter_tree::Node {
    u32 constantOneHook();
    u32 matchesRecord(const gptr<parameter_tree::MatchRecord>* record);
    void trailingEmptyHook();
    void alternateTrailingEmptyHook();
    void finalEmptyHook();
};
// The SafeString copy of this translation unit (literal text, nothing to terminate).
struct LiteralText : sead::SafeText {
    void destroy(u32 flags);
    void assureTermination();
};

// Four-channel tone curve (one evaluated curve per colour channel), the agl curve parameter instance.
struct ToneCurveParameter {
    u8 mInheritedPrefix[8];
    gptr<ParameterMethods_l> mMethods;
    CurveDescriptor mDescriptors[4];
    CurveValues mCurves[4];
    be<u32> mCurveCount;
    be<u8> mEnabled;
    u8 mPadding[3];
    void initializeCurves();
    f32 evaluate(u32 index, f32 position) const;
    void evaluateAll(sead::ColorChannels* result, f32 position) const;
};

struct LutTransform { be<f32> mScale, mOffset; };

enum Flag : u32 {
    kFlagDirty = 1,              // parameters changed: the map is redrawn
    kFlagVariationDirty = 2,     // the map shader variation is recomputed before drawing
    kFlagHue = 4,
    kFlagSaturation = 8,
    kFlagBrightness = 0x10,
    kFlagGamma = 0x20,
    kFlagToneCurve = 0x40,
    kFlagToycam = 0x80,
    kFlagAlwaysDraw = 0x10000,   // redraw the map even without changes
};
enum : u32 {
    kMapSize = 8,                // the colour map is an 8x8x8 3D texture
    kProgramColorCorrection = 7, // shader holder slots
    kProgramColorCorrectionMap = 8,
};

struct ColorCorrection {
    gptr<const void> mVtable;
    ParameterRoot mRoot;
    parameter_tree::Value mParameters;
    BoolParameter mEnable;
    FloatParameter mHue;
    FloatParameter mSaturation;
    FloatParameter mBrightness;
    FloatParameter mGamma;
    BoolParameter mToycamEnable;
    ColorParameter mToycamOffset1;
    ColorParameter mToycamOffset2;
    ColorParameter mToycamLevel1;
    ColorParameter mToycamLevel2;
    FloatParameter mToycamSaturation1;
    FloatParameter mToycamSaturation2;
    FloatParameter mToycamBrightness;
    FloatParameter mToycamContrast;
    ColorParameter mToycamMulColor;
    sead::CompactRenderState mDrawState;
    LutTransform mLutTransform;
    RenderBuffer mMapBuffer;
    ColorSurface mMapSlices[kMapSize];
    TextureData mMapData;
    TextureSampler mMapSampler;
    gptr<void> mMapImage;
    TextureSampler mSourceSampler;
    be<s32> mMapVariation;
    sead::ColorChannels mToneTable[kMapSize];
    ToneCurveParameter mToneCurve;
    gptr<ResourceRecord> mDebugEntry;   // a resource record: detached, then deleted through its own destructor
    be<u32> mFlags;

    static ColorCorrection* create(ColorCorrection* self);
    void destroy(u32 flags);
    void finalize();
    void resetParameters();
    void updateToneTable();
    void updateFlags();
    void applyParameters();
    void applyParametersFromRoot();
    u32 hueEnabled();
    u32 saturationEnabled();
    u32 brightnessEnabled();
    u32 gammaEnabled();
    void initializeMap(void* heap);
    void updateMapVariation();
    u32 drawMap(u32 shaderMode);
    u32 drawWithSampler(RenderBuffer* target, TextureSampler* source, u32 shaderMode);
    u32 drawWithTexture(RenderBuffer* target, const TextureData* source, u32 shaderMode);
    u32 drawRenderBuffer(RenderBuffer* target, u32 shaderMode);
};

}}

WWHD_OFFSET(agl::color_correction::BoolParameter, mValue, 0xC);
WWHD_SIZE(agl::color_correction::BoolParameter, 0x10);
WWHD_OFFSET(agl::color_correction::FloatParameter, mValue, 0xC);
WWHD_SIZE(agl::color_correction::FloatParameter, 0x10);
WWHD_OFFSET(agl::color_correction::ColorParameter, mValue, 0xC);
WWHD_SIZE(agl::color_correction::ColorParameter, 0x1C);
WWHD_OFFSET(agl::color_correction::ParameterObjectHooks, mNameHash, 0x50);
WWHD_OFFSET(agl::color_correction::ParameterNodeHooks, mNameHash, 0x54);
WWHD_OFFSET(agl::color_correction::ToneCurveParameter, mMethods, 8);
WWHD_OFFSET(agl::color_correction::ToneCurveParameter, mDescriptors, 0xC);
WWHD_OFFSET(agl::color_correction::ToneCurveParameter, mCurves, 0x3C);
WWHD_OFFSET(agl::color_correction::ToneCurveParameter, mCurveCount, 0x23C);
WWHD_SIZE(agl::color_correction::ToneCurveParameter, 0x244);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mRoot, 4);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mParameters, 0x1CC);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mEnable, 0x228);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mHue, 0x238);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mGamma, 0x268);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mToycamEnable, 0x278);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mToycamOffset1, 0x288);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mToycamLevel1, 0x2C0);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mToycamSaturation1, 0x2F8);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mToycamContrast, 0x328);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mToycamMulColor, 0x338);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mDrawState, 0x354);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mLutTransform, 0x3C8);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mMapBuffer, 0x3D0);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mMapSlices, 0x410);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mMapData, 0xE50);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mMapSampler, 0xEE0);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mMapImage, 0x1078);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mSourceSampler, 0x107C);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mMapVariation, 0x1214);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mToneTable, 0x1218);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mToneCurve, 0x1298);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mDebugEntry, 0x14DC);
WWHD_OFFSET(agl::color_correction::ColorCorrection, mFlags, 0x14E0);
WWHD_SIZE(agl::color_correction::ColorCorrection, 0x14E4);
