// Actor/background environment lighting.
#pragma once
#include "d/d_kankyo.h"
#include "d/d_kankyo_3.h"
namespace d_kankyo_cpp::lighting6 {
// HD: byte palette colours and the temporary signed work registers occupy separate blocks.
struct EnvironmentColors {
    u8 padding[0xB64];
    GXColor mActorC0, mActorK0;
    u8 paddingBG[0xB8C - 0xB6C];
    GXColor mFogColor;
    u8 paddingSky[4];
    GXColor mVrUsoUmiColor;
    u8 paddingCloud[0xBA4 - 0xB98];
    GXColor mBgAmbientColor;
    u8 paddingWork[0xF48 - 0xBA8];
    GXColorS10 mColorC0, mColorK0, mActorAddColAmb, mActorAddColDif;
    GXColorS10 mBgAddColor[8];
    GXColorS10 mAddColFog;
};
WWHD_OFFSET(EnvironmentColors, mActorC0, 0xB64);
WWHD_OFFSET(EnvironmentColors, mActorK0, 0xB68);
WWHD_OFFSET(EnvironmentColors, mFogColor, 0xB8C);
WWHD_OFFSET(EnvironmentColors, mVrUsoUmiColor, 0xB94);
WWHD_OFFSET(EnvironmentColors, mBgAmbientColor, 0xBA4);
WWHD_OFFSET(EnvironmentColors, mColorC0, 0xF48);
WWHD_OFFSET(EnvironmentColors, mColorK0, 0xF50);
WWHD_OFFSET(EnvironmentColors, mActorAddColAmb, 0xF58);
WWHD_OFFSET(EnvironmentColors, mActorAddColDif, 0xF60);
WWHD_OFFSET(EnvironmentColors, mBgAddColor, 0xF68);
WWHD_OFFSET(EnvironmentColors, mAddColFog, 0xFA8);
WWHD_SIZE(EnvironmentColors, 0xFB0);
// HD: the shared full environment layout is reused; only this part's methods are declared here.
struct dScnKy_env_light_c : KankyoEnvLight {
    void setLight_actor(dKy_tevstr_c *i_tevstr, GXColorS10 *i_fogColor, be<f32> *i_fogStartZ,
                        be<f32> *i_fogEndZ);
    void settingTevStruct_colget_actor(cXyz *i_pos, dKy_tevstr_c *i_tevstr, GXColorS10 *i_colorC0,
                                       GXColorS10 *i_colorK0, GXColorS10 *i_fogColor, be<f32> *i_fogStartZ,
                                       be<f32> *i_fogEndZ);
    void settingTevStruct_colget_player(dKy_tevstr_c *i_tevstr);
    void settingTevStruct_plightcol_plus(cXyz *i_pos, dKy_tevstr_c *i_tevstr, GXColorS10 *i_colorC0,
                                         GXColorS10 *i_colorK0, u32 timer);
    void settingTevStruct_eflightcol_plus(cXyz *i_pos, dKy_tevstr_c *i_tevstr);
    void setLight_bg(dKy_tevstr_c *i_tev, GXColorS10 *i_bg0c0, GXColorS10 *i_bg0k0, GXColorS10 *i_bg1c0,
                     GXColorS10 *i_bg1k0, GXColorS10 *i_bg2c0, GXColorS10 *i_bg2k0);
    void settingTevStruct(s32 i_lightType, cXyz *i_pos, dKy_tevstr_c *i_tevstr);
    void setLightTevColorType(void *i_model, dKy_tevstr_c *i_tevstr);
};
WWHD_SIZE(dScnKy_env_light_c, 0x120C);

// HD: light objects carry two material intensity factors before attenuation coefficients.
struct LightObject {
    cXyz mPosition, mDirection;
    GXColor mColor;
    GXColorS10 mAmbientColor;
    be<f32> mK0Intensity, mC0Intensity;
    be<f32> mCosAttenuation[3], mDistanceAttenuation[3];
    u8 tail[0x84 - 0x44];
};
WWHD_OFFSET(LightObject, mPosition, 0);
WWHD_OFFSET(LightObject, mDirection, 0xC);
WWHD_OFFSET(LightObject, mColor, 0x18);
WWHD_OFFSET(LightObject, mAmbientColor, 0x1C);
WWHD_OFFSET(LightObject, mK0Intensity, 0x24);
WWHD_OFFSET(LightObject, mC0Intensity, 0x28);
WWHD_OFFSET(LightObject, mCosAttenuation, 0x2C);
WWHD_OFFSET(LightObject, mDistanceAttenuation, 0x38);
WWHD_SIZE(LightObject, 0x84);
struct TevLightParameters {
    LightObject mLightObj;
    u8 paddingTev[0xC0 - 0x84];
    LightObject mMaterialLight;
    LightObject mAmbientLight;
};
WWHD_OFFSET(TevLightParameters, mMaterialLight, 0xC0);
WWHD_OFFSET(TevLightParameters, mAmbientLight, 0x144);
WWHD_SIZE(TevLightParameters, 0x1C8);
struct Material {
    u8 padding[0x10];
    be<u32> mpColorBlock;
    u8 paddingColor[4];
    be<u32> mpTevBlock;
    u8 paddingTev[4];
    be<u32> mpPEBlock;
    u8 paddingParameters[0xA0 - 0x24];
    be<u32> mParameterFlags;
    u8 tail[0x39C - 0xA4];
};
WWHD_OFFSET(Material, mpColorBlock, 0x10);
WWHD_OFFSET(Material, mpTevBlock, 0x18);
WWHD_OFFSET(Material, mpPEBlock, 0x20);
WWHD_OFFSET(Material, mParameterFlags, 0xA0);
WWHD_SIZE(Material, 0x39C);
struct Model {
    u8 padding[0xAC];
    be<u32> mpModelData;
};
WWHD_OFFSET(Model, mpModelData, 0xAC);
WWHD_SIZE(Model, 0xB0);
struct ModelData {
    u8 padding[0xC];
    be<u32> mMaterialNum, mpMaterials;
};
WWHD_OFFSET(ModelData, mMaterialNum, 0xC);
WWHD_OFFSET(ModelData, mpMaterials, 0x10);
WWHD_SIZE(ModelData, 0x14);
struct MaterialTable {
    u8 padding[0x24];
    be<u16> mMaterialNum;
};
WWHD_OFFSET(MaterialTable, mMaterialNum, 0x24);
WWHD_SIZE(MaterialTable, 0x26);
struct FogInfo {
    be<u8> mType;
    u8 padding[3];
    be<f32> mStartZ, mEndZ, mNearZ, mFarZ;
    GXColor mColor;
};
WWHD_OFFSET(FogInfo, mType, 0);
WWHD_OFFSET(FogInfo, mStartZ, 4);
WWHD_OFFSET(FogInfo, mEndZ, 8);
WWHD_OFFSET(FogInfo, mNearZ, 0xC);
WWHD_OFFSET(FogInfo, mFarZ, 0x10);
WWHD_OFFSET(FogInfo, mColor, 0x14);
WWHD_SIZE(FogInfo, 0x18);
struct MaterialParameter {
    be<f32> r, g, b, a;
};
WWHD_OFFSET(MaterialParameter, r, 0);
WWHD_OFFSET(MaterialParameter, g, 4);
WWHD_OFFSET(MaterialParameter, b, 8);
WWHD_OFFSET(MaterialParameter, a, 12);
WWHD_SIZE(MaterialParameter, 16);
// HD: preserve the complete GHS argument slots for setLight_bg, including the stack pointers.
struct BackgroundStackArguments {
    u8 padding[8];
    be<u32> bg3c0, bg3k0, fog, fogStart, fogEnd;
};
WWHD_OFFSET(BackgroundStackArguments, bg3c0, 8);
WWHD_OFFSET(BackgroundStackArguments, bg3k0, 12);
WWHD_OFFSET(BackgroundStackArguments, fog, 16);
WWHD_OFFSET(BackgroundStackArguments, fogStart, 20);
WWHD_OFFSET(BackgroundStackArguments, fogEnd, 24);
WWHD_SIZE(BackgroundStackArguments, 28);
// The first two names are already supplied by the common bindings.
enum TevType {
    TEV_TYPE_BG1 = 2,
    TEV_TYPE_BG2 = 3,
    TEV_TYPE_BG3 = 4,
    TEV_TYPE_BG0_FULL = 5,
    TEV_TYPE_BG1_FULL = 6,
    TEV_TYPE_BG2_FULL = 7,
    TEV_TYPE_BG3_FULL = 8,
    TEV_TYPE_PLAYER = 9,
    TEV_TYPE_BG0_PLIGHT = 91,
    TEV_TYPE_BG1_PLIGHT = 92,
    TEV_TYPE_BG2_PLIGHT = 93,
    TEV_TYPE_ACTOR_NOLIGHT = 94,
    TEV_TYPE_UNK99 = 99
};
enum {
    TevInitialized = 123,
    TevLightingSet = 124,
    ColorMaximum = 255,
    MaterialColorDirty = 0x10,
    MaterialKColorDirty = 0x80,
    MaterialColorParameter = 4,
    MaterialKColorParameter = 7
};

struct PrimaryBlock {
    be<u32> mpVtable;
};
struct TevBlock {
    be<u32> mType, mpVtable;
};
struct PrimaryVtable {
    u8 padding[0x14];
    be<u32> getFogInfo;
    u8 paddingLight[0x3C - 0x18];
    be<u32> setLight;
};
struct TevVtable {
    u8 padding[0x24];
    be<u32> setTevColor;
    u8 paddingGet[0x34 - 0x28];
    be<u32> getTevColor;
    u8 paddingK[4];
    be<u32> setTevKColor;
    u8 paddingGetK[0x4C - 0x40];
    be<u32> getTevKColor;
};
WWHD_SIZE(PrimaryBlock, 4);
WWHD_OFFSET(TevBlock, mpVtable, 4);
WWHD_SIZE(TevBlock, 8);
WWHD_OFFSET(PrimaryVtable, getFogInfo, 0x14);
WWHD_OFFSET(PrimaryVtable, setLight, 0x3C);
WWHD_SIZE(PrimaryVtable, 0x40);
WWHD_OFFSET(TevVtable, setTevColor, 0x24);
WWHD_OFFSET(TevVtable, getTevColor, 0x34);
WWHD_OFFSET(TevVtable, setTevKColor, 0x3C);
WWHD_OFFSET(TevVtable, getTevKColor, 0x4C);
WWHD_SIZE(TevVtable, 0x50);
// HD: per-translation-unit header state and the environment helper flags.
struct HeaderStatics {
    be<f32> mInitialA, mInitialB;
    be<u8> mByte, mFlag;
    u8 padding[2];
    be<u32> mWords[4];
};
WWHD_OFFSET(HeaderStatics, mInitialA, 0);
WWHD_OFFSET(HeaderStatics, mInitialB, 4);
WWHD_OFFSET(HeaderStatics, mByte, 8);
WWHD_OFFSET(HeaderStatics, mFlag, 9);
WWHD_OFFSET(HeaderStatics, mWords, 12);
WWHD_SIZE(HeaderStatics, 28);
struct EnvironmentFlags {
    be<u8> mInitialized, mPaletteState, mLightState, mRoomOverride;
};
WWHD_OFFSET(EnvironmentFlags, mInitialized, 0);
WWHD_OFFSET(EnvironmentFlags, mPaletteState, 1);
WWHD_OFFSET(EnvironmentFlags, mLightState, 2);
WWHD_OFFSET(EnvironmentFlags, mRoomOverride, 3);
WWHD_SIZE(EnvironmentFlags, 4);
struct HeaderDestructorRecords {
    u8 mRecords[3][12];
};
WWHD_SIZE(HeaderDestructorRecords, 36);
} // namespace d_kankyo_cpp::lighting6

void selectLightPalettes(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
f32 squaredDistance(u32, u32);
f32 approachLightCoordinate(u32, f32, f32, f32, f32);
void subtractLightPosition(u32, u32, u32);
void transformLightPosition(u32, u32, u32);
void setBackgroundLight(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
u32 getMaterialTable(u32);
void scaleMaterialColor(u32, u32, f32);
u32 getMaterialParameter(u32, u32);
void initializeHeaderByte(u32);
void initializeHeaderFlag(u32);
void destroyEnvironmentRenderState(u32, u32);
