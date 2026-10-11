// Weather controls and HD weather packets.
// Verified wind/weather controls, packet geometry, rendering and GHS lifetimes.
// HD: explicit render resources and double-buffered geometry extend the GC packets.
#include "d/d_kankyo_wether.h"
namespace d_kankyo_wether_cpp {

// HD: GHS array constructor and destructor callbacks.
static void sunGeometryDestroy(u32 object, u32 flags);
static void sunlenzGeometryDestroy(u32 object, u32 flags);
static void rainGeometryDestroy(u32 object, u32 flags);
static void snowGeometryDestroy(u32 object, u32 flags);
static void starGeometryDestroy(u32 object, u32 flags);
static void poisonGeometryDestroy(u32 object, u32 flags);
static void housiGeometryDestroy(u32 object, u32 flags);
static void cloudGeometryDestroy(u32 object, u32 flags);
static void vrkumoGeometryDestroy(u32 object, u32 flags);
static void waveGeometryDestroy(u32 object, u32 flags);
static u32 snowGeometryConstruct(u32 object);
static u32 starGeometryConstruct(u32 object);
static u32 poisonGeometryConstruct(u32 object);
static void sunGeometryPairDestroy(u32 object, u32 flags);
static void rainGeometryPairDestroy(u32 object, u32 flags);
static void snowGeometryPairDestroy(u32 object, u32 flags);
static void poisonGeometryPairDestroy(u32 object, u32 flags);
static void housiGeometryPairDestroy(u32 object, u32 flags);
static void cloudGeometryPairDestroy(u32 object, u32 flags);
static void waveGeometryPairDestroy(u32 object, u32 flags);
static u32 snowGeometryPairConstruct(u32 object);
static u32 poisonGeometryPairConstruct(u32 object);
static void texture_pair_destructor(u32 object, u32 flags);
static void texture_destructor(u32 object, u32 flags);
static u32 four_geometry_constructor(u32 object);
static void four_geometry_destructor(u32 object, u32 flags);
static void precipitation_geometry_destructor(u32 object, u32 flags);
static u32 sunGeometryConstruct(u32 object);
static u32 sunlenzGeometryConstruct(u32 object);
static u32 rainGeometryConstruct(u32 object);
static u32 housiGeometryConstruct(u32 object);
static u32 cloudGeometryConstruct(u32 object);
static u32 vrkumoGeometryConstruct(u32 object);
static u32 waveGeometryConstruct(u32 object);
static u32 sunGeometryPairConstruct(u32 object);
static u32 rainGeometryPairConstruct(u32 object);
static u32 housiGeometryPairConstruct(u32 object);
static u32 cloudGeometryPairConstruct(u32 object);
static u32 waveGeometryPairConstruct(u32 object);
static u32 precipitation_geometry_constructor(u32 object);
static u32 allocate_empty16(u32 object);
static u32 dual_texture_constructor(u32 object);
static u32 texture_with_constants_constructor(u32 object);

// HD: distinct copies of archive and stage names are used within one entry
// point. HD: heap-label literals below name the packet; the original labels
// include Shift-JIS descriptions.
static WeatherOpaqueObject weatherRainAttributes;
static WeatherOpaqueObject weatherSnowAttributes;
static WeatherOpaqueObject weatherPoisonAttributes;
struct WeatherInitialRange {
    WeatherFloatBits values[2];
};
WWHD_SIZE(WeatherInitialRange, 8);
static WeatherInitialRange weatherInitialRange;
struct WeatherRange {
    be<f32> values[2];
};
WWHD_SIZE(WeatherRange, 8);
static WeatherRange weatherRange;
struct WeatherRuntimeHandles {
    be<u32> values[4];
};
WWHD_SIZE(WeatherRuntimeHandles, 16);
static WeatherRuntimeHandles weatherRuntimeHandles;
static u8 weatherRuntimeHeader;
static u8 weatherRuntimeArchive;
static WeatherRegistration weatherRangeRegistration;
static WeatherRegistration weatherHeaderRegistration;
static WeatherRegistration weatherArchiveRegistration;
static WeatherRenderState weatherDefaultRenderState;
static WeatherOpaqueObject weatherSnowVtable;
static WeatherOpaqueObject weatherStarVtable;
static WeatherOpaqueObject weatherPoisonVtable;
static WeatherOpaqueObject weatherStringVtable;
static WeatherOpaqueObject weatherHousiAttributes;
static WeatherOpaqueObject weatherCloudAttributes;
static WeatherOpaqueObject weatherVrkumoAttributes;
static WeatherOpaqueObject weatherWaveAttributes;
static WeatherOpaqueObject weatherStarAttributes;
static WeatherOpaqueObject weatherSunVtable;
static WeatherOpaqueObject weatherSunlenzVtable;
static WeatherOpaqueObject weatherRainVtable;
static WeatherOpaqueObject weatherHousiVtable;
static WeatherOpaqueObject weatherCloudVtable;
static WeatherOpaqueObject weatherWaveVtable;
static WeatherOpaqueObject weatherVrkumoVtable;
static WeatherOpaqueObject weatherSmallTextureVtable;
static WeatherOpaqueObject weatherTextureVtable;
static WeatherOpaqueObject weatherBufferedTextureVtable;
static WeatherVector weatherZeroVector;
static WeatherOpaqueObject weatherShaderContext;
static WeatherMatrix weatherViewMatrix;
static WeatherProjection weatherProjectionMatrix;
static WeatherStarPalette weatherStarPalette;
static WeatherOpaqueObject weatherSunGanonStage;    // "GanonK"
static WeatherOpaqueObject weatherSunTowerStage;    // "GTower"
static WeatherOpaqueObject weatherSunArchiveName;   // "Always"
static WeatherOpaqueObject weatherStarGanonStage;   // "GanonK"
static WeatherOpaqueObject weatherStarTowerStage;   // "GTower"
static WeatherOpaqueObject weatherStarNameStage;    // "Name"
static WeatherOpaqueObject weatherHousiArchiveName; // "Always"
static WeatherOpaqueObject weatherSunArchiveFile;   // "Always.bfres"
static WeatherOpaqueObject weatherHousiArchiveFile; // "Always.bfres"

// HD: the constants are guest views so recorded inputs retain their original
// bits.
static WindDirectionTable wind_table;
static WeatherSinCosTable weatherSinCosTable;
static WeatherFloatBits weatherNegativeOne;
static WeatherFloatBits weatherZero;
static WeatherFloatBits weatherWindNoticeDisabled;
static WeatherFloatBits weatherWindlineScale;
static WeatherFloatBits weatherColorScale;
static WeatherFloatBits weatherSunlenzDepthLimit;
static WeatherFloatBits weatherHeavyRainThreshold;
static WeatherFloatBits weatherOne;
static WeatherFloatBits weatherStarDawnScale;
static WeatherFloatBits weatherNameNightStart;
static WeatherFloatBits weatherNameNightEnd;
static WeatherFloatBits weatherNameDuskStart;
static WeatherFloatBits weatherNameDuskScale;
static WeatherFloatBits weatherNameDawnEnd;
static WeatherFloatBits weatherNightStart;
static WeatherFloatBits weatherNightEnd;
static WeatherFloatBits weatherDuskStart;
static WeatherFloatBits weatherDuskScale;
static WeatherFloatBits weatherDawnEnd;
static WeatherFloatBits weatherStarFadeThreshold;
static WeatherFloatBits weatherWindDamping;
static WeatherFloatBits weatherStarStep;
static WeatherFloatBits weatherStarMinimumStep;
static WeatherFloatBits weatherStarCountScale;
static WeatherFloatBits weatherSeaCloudCenterX;
static WeatherFloatBits weatherSeaCloudCenterY;
static WeatherFloatBits weatherSeaCloudCenterZ;
static WeatherFloatBits weatherCloudGrowStep;
static WeatherFloatBits weatherCloudGrowMinimumStep;
static WeatherFloatBits weatherCloudFadeDamping;
static WeatherFloatBits weatherCloudFadeStep;
static WeatherFloatBits weatherCloudFadeMinimumStep;
static WeatherFloatBits weatherSeaCloudRadius;
static WeatherFloatBits weatherCloudCountScale;
static WeatherFloatBits weatherSquallRadiusScale;
static WeatherFloatBits weatherWindMinimumStep;
static WeatherFloatBits weatherSquallDamping;
static WeatherFloatBits weatherWindLevelLow;
static WeatherFloatBits weatherWindLevelMedium;
static WeatherFloatBits weatherWindLevelHigh;
static WeatherFloatBits weatherWindDirectionStep;
static WeatherFloatBits weatherWindPowerMinimumStep;
static WeatherFloatBits weatherPointWindScale;
static WeatherFloatBits weatherTextureOne;
static WeatherFloatBits weatherTextureZero;
static u32 weatherArchiveManager;
static WeatherSaveLink weatherSavePointer;
static u32 weatherTextureManager;
static u32 weatherVertexAllocator;
static u32 weatherSeaCloudCenterInitialized;
static f32 weatherSeaCloudX;
static f32 weatherSeaCloudY;
static f32 weatherSeaCloudZ;
static s8 weatherStayRoomNo;
static u32 weatherOpaqueDrawBuffer;
static u32 weatherTranslucentDrawBuffer;
static u32 weatherCamera;
static u32 weatherTriangleIndexType;
static u32 weatherTrianglePrimitive;
static u32 weatherTriangleIndices;
static u32 weatherQuadIndexType;
static u32 weatherQuadPrimitive;
static u32 weatherQuadIndices;

static u32 environment() { return weatherGetEnvironment(); }
static dKankyo::dScnKy_env_light_c* dKy_getWeatherEnvlight() {
    return gabi::at<dKankyo::dScnKy_env_light_c>(weatherGetEnvironment());
}
static WeatherPlay* weatherGetPlay() { return gabi::at<WeatherPlay>(weatherGetPlayRegister()); }
/* 02578348 */
static f32 dKyw_get_wind_pow() {

    return static_cast<f32>(dKy_getWeatherEnvlight()->mWind.mWindPower);
}
/* 0257D174 */
static void dKyw_wether_draw2() {

    if (static_cast<u8>(dKy_getWeatherEnvlight()->mVrkumoStatus) != 0)
        if (static_cast<u8>(dKy_getWeatherEnvlight()->mVrkumoStatus) < 99)
            weatherCall_dKyw_drawVrkumo(1u);
}
/* 0257D1B8 */
static void dKyw_pntwind_cut(WeatherWindInfluence *i_influence) {
    if (i_influence && static_cast<u32>(i_influence->mRegistIdx) < 30) {
        auto *env = dKy_getWeatherEnvlight();
        u32 index = i_influence->mRegistIdx;
        env->mpWindInfluence[index] = nullptr;
    }
}

/* 0257DAA8 */
static WeatherVector *dKyw_get_wind_vec() {
    return reinterpret_cast<WeatherVector *>(&dKy_getWeatherEnvlight()->mWind.mWindVec);
}

/* 0257DACC */
static u32 dKyw_get_wind_angle() {

    u32 vec = weatherdKyw_get_wind_vec();
    s32 angle = weatherAtan2Signed(static_cast<f32>(gabi::at<WeatherVector>(vec)->x),
                                   static_cast<f32>(gabi::at<WeatherVector>(vec)->z));
    return (u16)((u32)angle + 0x8000u);
}
/* 0257DB04 */
static be<f32> *dKyw_get_wind_power() { return &dKy_getWeatherEnvlight()->mWind.mWindPower; }

/* 0257DBDC */
static void pntwind_set(WeatherWindInfluence *i_influence) {
    for (u32 i = 0; i < 30; ++i) {
        if (!dKy_getWeatherEnvlight()->mpWindInfluence[i]) {
            dKy_getWeatherEnvlight()->mpWindInfluence[i] = i_influence;
            auto *stored = gabi::at<WeatherWindInfluence>(
                gabi::ea(dKy_getWeatherEnvlight()->mpWindInfluence[i]));
            stored->mRegistIdx = i;
            return;
        }
    }
    i_influence->mRegistIdx = 9999;
}

/* 0257DC90 */
static void dKyw_pntwind_set(WeatherWindInfluence *i_influence) {
    i_influence->mbConstant = 0;
    weatherpntwind_set(gabi::ea(i_influence));
}

/* 0257DE5C */
static void dKyw_pwind_cylinder_set(WeatherWindInfluence *i_influence) {
    // HD: this wrapper clears the constant-wind byte, like the point-wind setter.
    i_influence->mbConstant = 0;
    weatherpntwind_set(gabi::ea(i_influence));
}

/* 0257E490 */
static void dKyw_tact_wind_set(u32 windX, u32 windY) {

    auto *env = dKy_getWeatherEnvlight();
    u32 vec = weatherdKyw_get_wind_vec();
    env->mWind.mTactWindAngleX = windX;
    env->mWind.mTactWindAngleY = windY;
    (weatherSavePointer->mWindX = windX);
    (weatherSavePointer->mWindY = static_cast<u16>(env->mWind.mTactWindAngleY));
    u32 angle = weatherAtan2Register(static_cast<f32>(gabi::at<WeatherVector>(vec)->x),
                                     static_cast<f32>(gabi::at<WeatherVector>(vec)->z));
    s16 delta = (s16)(0u - (angle + 0x4000u) - (s32) static_cast<s16>(env->mWind.mTactWindAngleY));
    env->mWind.mTactWindAngleFlags = 1;
    if (delta < 0)
        env->mWind.mTactWindAngleFlags = 0x81;
}
/* 0257E52C */
static void dKyw_tact_wind_set_go() {

    auto *env = dKy_getWeatherEnvlight();
    u8 flags = static_cast<u8>(env->mWind.mTactWindAngleFlags);
    env->mWind.mEvtWindSet = 0xFF;
    env->mWind.mTactWindAngleFlags = flags | 1;
}
/* 0257E560 */
static s32 dKyw_get_tactwind_dir() {

    return (static_cast<u8>(dKy_getWeatherEnvlight()->mWind.mTactWindAngleFlags) & 0x80) ? 1 : 0;
}
/* 0257E594 */
static void dKyw_custom_windpower(f32 power) {

    dKy_getWeatherEnvlight()->mWind.mCustomWindPower = power;
}
/* 0257E5D4 */
static s32 dKyw_get_windsdir() {
    auto *wind_vec = gabi::at<WeatherVector>(weatherdKyw_get_wind_vec());
    u32 angle = weatherAtan2Register(wind_vec->x, wind_vec->z);
    u16 t = 0u - (angle + 0x4000u);
    s32 dir = 0;
    for (u32 i = 0; i < 9; ++i) {
        if (t < static_cast<u16>(wind_table.mRanges[i].max_angle) &&
            t >= static_cast<u16>(wind_table.mRanges[i].min_angle))
            dir = wind_table.mRanges[i].direction;
    }
    return dir;
}

/* 0257E684 */
static void dKyw_evt_wind_set(u32 windX, u32 windY) {

    auto *env = dKy_getWeatherEnvlight();
    env->mWind.mEvtWindAngleX = windX;
    env->mWind.mEvtWindAngleY = windY;
}
/* 0257E6C4 */
static void dKyw_evt_wind_set_go() { dKy_getWeatherEnvlight()->mWind.mEvtWindSet = 1; }
/* 0257E77C */
static void dKyw_tornado_Notice(WeatherVector *i_pos) {
    auto *env = dKy_getWeatherEnvlight();
    auto *notice = reinterpret_cast<WeatherVector *>(&env->mWind.field_0x8);
    for (u32 i = 0; i < 3; ++i)
        notice->words[i] = i_pos->words[i];
}

/* 0257E7C0 */
static void dKyw_rain_set(u32 i_count) {
    auto *env = dKy_getWeatherEnvlight();
    env->mRainCount = i_count;
    env->mRainCountOrig = i_count;
}

/* 02582500 */
void dKankyo_wave_Packet::draw(u32 context) {
    auto* packetView = this;

    weatherDrawWave(gabi::ea(&weatherViewMatrix), gabi::ea(&packetView->mpTextures), context);
}
struct Vector {
    gabi::be<f32> x, y, z;
};
static void copyVectorBits(u32 destination, u32 source) {
    auto* out = gabi::at<WeatherVector>(destination);
    auto* in = gabi::at<WeatherVector>(source);
    for (u32 i = 0; i < 3; ++i)
        out->words[i] = in->words[i];
}
/* 0257DB28 */
static void dKyw_get_wind_vecpow(WeatherVector *i_destination) {
    u32 destination = gabi::ea(i_destination);

    f32 power = static_cast<f32>(dKy_getWeatherEnvlight()->mWind.mWindPower);
    u32 vec = environment() + 0x9FC;
    gabi::Local<Vector> result;
    weatherScaleVector(vec, result.a, power);
    f32 y = static_cast<f32>(gabi::at<WeatherVector>(result.a)->y);
    f32 z = static_cast<f32>(gabi::at<WeatherVector>(result.a)->z);
    f32 x = static_cast<f32>(gabi::at<WeatherVector>(result.a)->x);
    if (!destination)
        destination = weatherAllocateRegister(12u);
    if (destination) {
        (gabi::at<WeatherVector>(destination)->y = y);
        (gabi::at<WeatherVector>(destination)->x = x);
        (gabi::at<WeatherVector>(destination)->z = z);
    }
}
/* 0257E128 */
static void dKyw_pntwind_get_vecpow(WeatherVector *i_destination, u32 position) {
    u32 destination = gabi::ea(i_destination);

    gabi::Local<Vector> direction, result;
    gabi::Local<gabi::be<f32>> power;
    weatherdKyw_pntwind_get_info(position, direction.a, power.a);
    weatherScaleVector(direction.a, result.a, static_cast<f32>(*gabi::at<gabi::be<f32>>(power.a)));
    f32 x = static_cast<f32>(gabi::at<WeatherVector>(result.a)->x),
        y = static_cast<f32>(gabi::at<WeatherVector>(result.a)->y),
        z = static_cast<f32>(gabi::at<WeatherVector>(result.a)->z);
    (gabi::at<WeatherVector>(direction.a)->y = y);
    (gabi::at<WeatherVector>(direction.a)->z = z);
    (gabi::at<WeatherVector>(direction.a)->x = x);
    if (!destination) {
        destination = weatherAllocateRegister(12u);
        if (!destination)
            return;
        x = static_cast<f32>(gabi::at<WeatherVector>(direction.a)->x);
    }
    (gabi::at<WeatherVector>(destination)->x = x);
    (gabi::at<WeatherVector>(destination)->y =
         static_cast<f32>(gabi::at<WeatherVector>(direction.a)->y));
    (gabi::at<WeatherVector>(destination)->z =
         static_cast<f32>(gabi::at<WeatherVector>(direction.a)->z));
}
/* 0257E34C */
static void dKyw_get_AllWind_vecpow(WeatherVector *i_destination, u32 position) {
    u32 destination = gabi::ea(i_destination);

    auto *env = dKy_getWeatherEnvlight();
    gabi::Local<gabi::be<f32>> power;
    gabi::Local<Vector> direction, temporary, globalPart, pointPart;
    weatherdKyw_pntwind_get_info(position, direction.a, power.a);
    f32 complement = gabi::fsubs_ppc(static_cast<f32>(weatherOne.value),
                                     static_cast<f32>(*gabi::at<gabi::be<f32>>(power.a)));
    f32 strength = gabi::fmuls_ppc(static_cast<f32>(env->mWind.mWindPower), complement);
    weatherScaleVector(gabi::ea(&env->mWind.mWindVec), temporary.a, strength);
    copyVectorBits(globalPart.a, temporary.a);
    strength = gabi::fmuls_ppc(static_cast<f32>(*gabi::at<gabi::be<f32>>(power.a)),
                               static_cast<f32>(weatherPointWindScale.value));
    weatherScaleVector(direction.a, temporary.a, strength);
    copyVectorBits(pointPart.a, temporary.a);
    weatherAddVectors(globalPart.a, temporary.a, pointPart.a);
    f32 x = static_cast<f32>(gabi::at<WeatherVector>(temporary.a)->x),
        y = static_cast<f32>(gabi::at<WeatherVector>(temporary.a)->y),
        z = static_cast<f32>(gabi::at<WeatherVector>(temporary.a)->z);
    if (!destination)
        destination = weatherAllocateRegister(12u);
    if (destination) {
        (gabi::at<WeatherVector>(destination)->y = y);
        (gabi::at<WeatherVector>(destination)->x = x);
        (gabi::at<WeatherVector>(destination)->z = z);
    }
}
/* 0257E6EC */
static s32 dKyw_gbwind_use_check() {

    if (static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mWind.mpWindVecOverride)) != 0)
        return 0;
    s32 room = static_cast<s8>(weatherStayRoomNo);
    if (room < 0)
        return 0;
    u32 game = weatherGetPlayRegister();
    u32 status = weatherdStage_roomControl_c__getStatusRoomDtRegister(game + 0x51CC, room);
    u32 target = static_cast<u32>(gabi::at<WeatherRoomVtable>(gabi::at<WeatherRoomStatus>(status)->mpVtable)->mpGetFileList);
    u32 information = gabi::call_ptr<u32>(target, status);
    if (!information)
        return 0;
    return ((static_cast<u32>(gabi::at<WeatherRoomFileList>(information)->mFlags) >> 18) & 3) <= 2;
}

// The HD renderer stores texture data and vertex state in separate subobjects.
static void release_texture_geometry(u32 object, u32 flags) {
    if (!object)
        return;
    auto* geometry = gabi::at<WeatherGeometry>(object);
    weatherDestroyTextureBinding(gabi::ea(&geometry->mMaterialBinding), 2u);
    weatherDestroyVertexBuffer(gabi::ea(&geometry->mVertexBuffer), 2u);
    if (flags & 1)
        weatherFree(object);
}
static u32 construct_texture_geometry(u32 object) {
    if (!object) {
        object = weatherAllocateRegister(sizeof(WeatherGeometry));
        if (!object)
            return 0;
    }
    auto* geometry = gabi::at<WeatherGeometry>(object);
    weatherConstructVertexBuffer(gabi::ea(&geometry->mVertexBuffer));
    weatherConstructTextureBinding(gabi::ea(&geometry->mMaterialBinding));
    geometry->mpAllocation = 0;
    geometry->mVertexCount = 0;
    return object;
}
static void release_geometry_element(u32 object, u32 base) {
    auto* geometry = gabi::at<WeatherGeometry>(object + base);
    weatherInitializeTextureBinding(gabi::ea(&geometry->mMaterialBinding));
    u32 pointer = geometry->mpAllocation;
    geometry->mpVertices = 0;
    if (pointer) {
        (void)static_cast<u32>(geometry->mVertexCount);
        u32 allocator = weatherAllocateVertices(static_cast<u32>(weatherVertexAllocator), pointer);
        auto* heap = gabi::at<WeatherHeap>(allocator);
        u32 target = heap->mpVtable->mpFree;
        gabi::call_ptr<void>(target, allocator, static_cast<u32>(geometry->mpAllocation));
        geometry->mVertexCount = 0;
        geometry->mpAllocation = 0;
    }
}
static void release_geometry_pair(u32 object, u32 flags, u32 destructor) {
    if (object) {
        release_geometry_element(object, 0);
        release_geometry_element(object, 0x254);
        gabi::at<WeatherGeometryPair>(object)->mpMaterial = 0;
        weatherDestroyArray(object, 2u, 0x254u, destructor, 0u, 0u);
        if (flags & 1)
            weatherFree(object);
    }
}
static u32 construct_geometry_pair(u32 object, u32 constructor, u32 count = 20) {
    if (!object) {
        object = weatherAllocateRegister(0x4C0u);
        if (!object)
            return 0;
    }
    weatherConstructArray(object, 2u, 0x254u, constructor);
    gabi::at<WeatherGeometryPair>(object)->mAttributeCount = count;
    gabi::at<WeatherGeometryPair>(object)->mbInitialized = 0;
    gabi::at<WeatherGeometryPair>(object)->mPrimitive = 0;
    gabi::at<WeatherGeometryPair>(object)->mpMaterial = 0;
    gabi::at<WeatherGeometryPair>(object)->mBufferIndex = 0;
    gabi::at<WeatherGeometryPair>(object)->mGeometry[0].mpVertices = 0;
    gabi::at<WeatherGeometryPair>(object)->mGeometry[1].mpVertices = 0;
    return object;
}
/* 02583FBC */
static void sunGeometryDestroy(u32 object, u32 flags) { release_texture_geometry(object, flags); }
/* 02584194 */
static void sunlenzGeometryDestroy(u32 object, u32 flags) {
    release_texture_geometry(object, flags);
}
/* 02584248 */
static void rainGeometryDestroy(u32 object, u32 flags) { release_texture_geometry(object, flags); }
/* 02584498 */
static void snowGeometryDestroy(u32 object, u32 flags) { release_texture_geometry(object, flags); }
/* 02584700 */
static void starGeometryDestroy(u32 object, u32 flags) { release_texture_geometry(object, flags); }
/* 02584960 */
static void poisonGeometryDestroy(u32 object, u32 flags) {
    release_texture_geometry(object, flags);
}
/* 02584ACC */
static void housiGeometryDestroy(u32 object, u32 flags) { release_texture_geometry(object, flags); }
/* 02584C38 */
static void cloudGeometryDestroy(u32 object, u32 flags) { release_texture_geometry(object, flags); }
/* 02584DA4 */
static void vrkumoGeometryDestroy(u32 object, u32 flags) {
    release_texture_geometry(object, flags);
}
/* 02584F28 */
static void waveGeometryDestroy(u32 object, u32 flags) { release_texture_geometry(object, flags); }
/* 025843B4 */
static u32 snowGeometryConstruct(u32 object) { return construct_texture_geometry(object); }
/* 02584604 */
static u32 starGeometryConstruct(u32 object) { return construct_texture_geometry(object); }
/* 0258487C */
static u32 poisonGeometryConstruct(u32 object) { return construct_texture_geometry(object); }
/* 0258401C */
static void sunGeometryPairDestroy(u32 object, u32 flags) {
    release_geometry_pair(object, flags, gabi::guest_fn((const void*)sunGeometryDestroy));
}
/* 025842A8 */
static void rainGeometryPairDestroy(u32 object, u32 flags) {
    release_geometry_pair(object, flags, gabi::guest_fn((const void*)rainGeometryDestroy));
}
/* 025844F8 */
static void snowGeometryPairDestroy(u32 object, u32 flags) {
    release_geometry_pair(object, flags, gabi::guest_fn((const void*)snowGeometryDestroy));
}
/* 025849C0 */
static void poisonGeometryPairDestroy(u32 object, u32 flags) {
    release_geometry_pair(object, flags, gabi::guest_fn((const void*)poisonGeometryDestroy));
}
/* 02584B2C */
static void housiGeometryPairDestroy(u32 object, u32 flags) {
    release_geometry_pair(object, flags, gabi::guest_fn((const void*)housiGeometryDestroy));
}
/* 02584C98 */
static void cloudGeometryPairDestroy(u32 object, u32 flags) {
    release_geometry_pair(object, flags, gabi::guest_fn((const void*)cloudGeometryDestroy));
}
/* 02584F88 */
static void waveGeometryPairDestroy(u32 object, u32 flags) {
    release_geometry_pair(object, flags, gabi::guest_fn((const void*)waveGeometryDestroy));
}
/* 02584410 */
static u32 snowGeometryPairConstruct(u32 object) {
    return construct_geometry_pair(object, gabi::guest_fn((const void*)snowGeometryConstruct));
}
/* 025848D8 */
static u32 poisonGeometryPairConstruct(u32 object) {
    return construct_geometry_pair(object, gabi::guest_fn((const void*)poisonGeometryConstruct));
}
/* 02584F24 */
static void waveEffectConstruct() {}
/* 02585094 */
static void waveEffectDestroy() {}
/* 02584128 */
static void texture_pair_destructor(u32 object, u32 flags) {
    if (object) {
        weatherDestroyTexture(gabi::ea(&gabi::at<WeatherRenderer>(object)->mExtendedTexture), 0u);
        weatherDestroyTexture(gabi::ea(&gabi::at<WeatherRenderer>(object)->mPrefix.mTexture), 0u);
        weatherDestroyRenderer(object, 2u);
        if (flags & 1)
            weatherFree(object);
    }
}
/* 025841F4 */
static void texture_destructor(u32 object, u32 flags) {
    if (object) {
        weatherDestroyTexture(object, 0u);
        if (flags & 1)
            weatherFree(object);
    }
}
/* 02584660 */
static u32 four_geometry_constructor(u32 object) {
    if (!object) {
        object = weatherAllocateRegister(0x968u);
        if (!object)
            return 0;
    }
    weatherConstructArray(object, 4u, 0x254u, gabi::guest_fn((const void*)starGeometryConstruct));
    (gabi::at<WeatherStarGeometry>(object)->mBufferIndex = 0);
    (gabi::at<WeatherStarGeometry>(object)->mAttributeCount = 12);
    (gabi::at<WeatherStarGeometry>(object)->mbInitialized = 0);
    (gabi::at<WeatherStarGeometry>(object)->mPrimitive = 0);
    (gabi::at<WeatherStarGeometry>(object)->mpMaterial = 0);
    for (u32 pair = 0; pair < 2; ++pair)
        for (u32 side = 0; side < 2; ++side)
            gabi::at<WeatherStarGeometry>(object)->mGeometry[pair + side * 2].mpVertices = 0;
    return object;
}
template <typename GeometrySet>
static void release_geometry_array(u32 object, u32 flags, u32 pairs, u32 arrayCount,
                                   u32 destructor) {
    if (!object)
        return;
    auto* geometry = gabi::at<GeometrySet>(object);
    for (u32 pair = 0; pair < pairs; ++pair) {
        release_geometry_element(gabi::ea(&geometry->mGeometry[pair * 2]), 0);
        release_geometry_element(gabi::ea(&geometry->mGeometry[pair * 2 + 1]), 0);
    }
    geometry->mpMaterial = 0;
    weatherDestroyArray(object, arrayCount, sizeof(WeatherGeometry), destructor, 0u, 0u);
    if (flags & 1)
        weatherFree(object);
}

/* 02584760 */
static void four_geometry_destructor(u32 object, u32 flags) {
    release_geometry_array<WeatherStarGeometry>(object, flags, 2, 4,
                                                gabi::guest_fn((const void*)starGeometryDestroy));
}
/* 02584E04 */
static void precipitation_geometry_destructor(u32 object, u32 flags) {
    release_geometry_array<WeatherVrkumoGeometry>(
        object, flags, 300, 600, gabi::guest_fn((const void*)vrkumoGeometryDestroy));
}
/* 02582CFC */
static u32 sunGeometryConstruct(u32 object) { return construct_texture_geometry(object); }
/* 025830D0 */
static u32 sunlenzGeometryConstruct(u32 object) { return construct_texture_geometry(object); }
/* 0258312C */
static u32 rainGeometryConstruct(u32 object) { return construct_texture_geometry(object); }
/* 025834B0 */
static u32 housiGeometryConstruct(u32 object) { return construct_texture_geometry(object); }
/* 02583594 */
static u32 cloudGeometryConstruct(u32 object) { return construct_texture_geometry(object); }
/* 02583B18 */
static u32 vrkumoGeometryConstruct(u32 object) { return construct_texture_geometry(object); }
/* 02583C1C */
static u32 waveGeometryConstruct(u32 object) { return construct_texture_geometry(object); }
/* 02582D58 */
static u32 sunGeometryPairConstruct(u32 object) {
    return construct_geometry_pair(object, gabi::guest_fn((const void*)sunGeometryConstruct), 152);
}
/* 02583188 */
static u32 rainGeometryPairConstruct(u32 object) {
    return construct_geometry_pair(object, gabi::guest_fn((const void*)rainGeometryConstruct), 20);
}
/* 0258350C */
static u32 housiGeometryPairConstruct(u32 object) {
    return construct_geometry_pair(object, gabi::guest_fn((const void*)housiGeometryConstruct), 20);
}
/* 025835F0 */
static u32 cloudGeometryPairConstruct(u32 object) {
    return construct_geometry_pair(object, gabi::guest_fn((const void*)cloudGeometryConstruct), 20);
}
/* 02583C78 */
static u32 waveGeometryPairConstruct(u32 object) {
    return construct_geometry_pair(object, gabi::guest_fn((const void*)waveGeometryConstruct), 20);
}
/* 02583B74 */
static u32 precipitation_geometry_constructor(u32 object) {
    if (!object) {
        object = weatherAllocateRegister(0x574F8u);
        if (!object)
            return 0;
    }
    weatherConstructArray(object, 600u, 0x254u,
                          gabi::guest_fn((const void*)vrkumoGeometryConstruct));
    (gabi::at<WeatherVrkumoGeometry>(object)->mBufferIndex = 0);
    (gabi::at<WeatherVrkumoGeometry>(object)->mpMaterial = 0);
    (gabi::at<WeatherVrkumoGeometry>(object)->mAttributeCount = 20);
    (gabi::at<WeatherVrkumoGeometry>(object)->mPrimitive = 0);
    (gabi::at<WeatherVrkumoGeometry>(object)->mbInitialized = 0);
    for (u32 side = 0; side < 2; ++side)
        for (u32 index = 0; index < 300; ++index)
            gabi::at<WeatherVrkumoGeometry>(object)->mGeometry[side + index * 2].mpVertices = 0;
    return object;
}
/* 02582DE0 */
static u32 allocate_empty16(u32 object) { return object ? object : weatherAllocateRegister(16u); }
/* 02582BB8 */
static u32 safe_string_constructor(u32 object, u32 text) {
    if (!object) {
        object = weatherAllocateRegister(8u);
        if (!object)
            return 0;
    }
    gabi::at<SafeString>(object)->text = text;
    gabi::at<SafeString>(object)->vtable = gabi::ea(&weatherStringVtable);
    return object;
}
/* 02582CE8 */
static void safe_string_destructor(u32 object, u32 flags) {
    if (object && (flags & 1))
        weatherFree(object);
}
/* 02582B24 */
static void __sinit_d_kankyo_wether_cpp() {
    weatherRuntimeHandles.values[2] = 0;
    weatherRuntimeHandles.values[0] = 0;
    weatherRuntimeHandles.values[3] = 0;
    weatherRuntimeHandles.values[1] = 0;
    weatherRegisterGlobalObject(gabi::ea(&weatherRangeRegistration));
    f32 first = static_cast<f32>(weatherInitialRange.values[0].value);
    f32 second = static_cast<f32>(weatherInitialRange.values[1].value);
    weatherRange.values[0] = first;
    weatherRange.values[1] = second;
    weatherConstructHeaderFlag(gabi::ea(&weatherRuntimeHeader));
    weatherRegisterGlobalObject(gabi::ea(&weatherHeaderRegistration));
    weatherConstructArchiveFlag(gabi::ea(&weatherRuntimeArchive));
    weatherRegisterGlobalObject(gabi::ea(&weatherArchiveRegistration));
}
/* 025776F8 */
static void effect_destructor(u32 object, u32 flags) {

    if (object && (flags & 1))
        weatherFree(object);
}
/* 0257770C */
static void empty_effect_constructor() {}
/* 02577710 */
static u32 dKyw_setDrawPacketList(u32 packet, u32 type) {

    if (packet) {
        u32 game = weatherGetPlayRegister();
        weatherJ3DDrawBuffer__entryImm(static_cast<u32>(gabi::at<WeatherPlay>(game)->mpFilter),
                                       packet, 0u);
    }
    return packet;
}
/* 02577754 */
static u32 dKyw_setDrawPacketListSky(u32 packet, u32 type) {

    if (packet) {
        u32 first = static_cast<u32>(weatherGetPlay()->mpSkyOpaque);
        (weatherOpaqueDrawBuffer = first);
        u32 second = static_cast<u32>(weatherGetPlay()->mpSkyTranslucent);
        (weatherTranslucentDrawBuffer = second);
        u32 buffer = gabi::load<u32>(0x104B4634 + (type < 2 ? type * 4 : 0));
        weatherJ3DDrawBuffer__entryImm(buffer, packet, 0u);
        (weatherOpaqueDrawBuffer = static_cast<u32>(weatherGetPlay()->mpOpaque));
        (weatherTranslucentDrawBuffer = static_cast<u32>(weatherGetPlay()->mpTranslucent));
    }
    return packet;
}
/* 025777F0 */
static void dKyw_drawSun(u32 type) {

    weatherCall_dKyw_setDrawPacketListSky(
        static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpSunPacket)), type);
}
/* 02577864 */
static void dKyw_drawRain(u32 type) {

    weatherCall_dKyw_setDrawPacketList(
        static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpRainPacket)), type);
}
/* 0257789C */
static void dKyw_drawSnow(u32 type) {

    weatherCall_dKyw_setDrawPacketList(
        static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpSnowPacket)), type);
}
/* 025778D4 */
static void dKyw_drawStar(u32 type) {

    weatherCall_dKyw_setDrawPacketListSky(
        static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpStarPacket)), type);
}
/* 02577974 */
static void dKyw_drawHousi(u32 type) {

    weatherCall_dKyw_setDrawPacketList(
        static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpHousiPacket)), type);
}
/* 025779AC */
static void dKyw_drawCloud(u32 type) {

    weatherCall_dKyw_setDrawPacketList(
        static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpMoyaPacket)), type);
}
/* 02577828 */
static void dKyw_drawSunlenz(u32 type) {

    (void)environment();
    weatherCall_dKyw_setDrawPacketList(
        static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpSunlenzPacket)), type);
}
/* 0257790C */
static u32 dKyw_drawPoison() {

    u32 packet = static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpPoisonPacket));
    u32 position = static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpPoisonPacket)) + 0xBC24;
    if (packet) {
        u32 game = weatherGetPlayRegister();
        weatherdDlst_list_c__entryZSortXluDrawList(
            game + 0x5D30, static_cast<u32>(gabi::at<WeatherPlay>(game)->mpTranslucent), packet,
            position);
    }
    return packet;
}
static void delete_weather_packet(u32 owner) {
    u32 packet = gabi::load<u32>(owner);
    if (packet) {
        u32 target = gabi::load<u32>(gabi::load<u32>(packet + 0xC) + 0xC);
        gabi::call_ptr<void>(target, packet, 2u);
        gabi::store<u32>(owner, 0);
        weatherDelete(packet);
    }
}
/* 02577E38 */
static void weatherDeleteRain(u32 owner) { delete_weather_packet(owner); }
/* 02577E9C */
static void weatherDeleteStar(u32 owner) { delete_weather_packet(owner); }
/* 02577F00 */
static void weatherDeleteHousi(u32 owner) { delete_weather_packet(owner); }
/* 02577F64 */
static void weatherDeleteCloud(u32 owner) { delete_weather_packet(owner); }
/* 02577FC8 */
static void weatherDeleteWave(u32 owner) { delete_weather_packet(owner); }
/* 02577E00 */
static void dKyw_wether_init2() {

    dKy_getWeatherEnvlight()->mVrkumoStatus = 0;
    dKy_getWeatherEnvlight()->mVrkumoCount = 0;
}
/* 025782D0 */
static void dKyw_wether_delete2() {

    if (static_cast<u8>(dKy_getWeatherEnvlight()->mVrkumoStatus)) {
        auto *env = dKy_getWeatherEnvlight();
        delete_weather_packet(gabi::ea(env) + 0xA94);
    }
}

static u32 draw_sky_packet(u32 packet, u32 type, u32 gameOffset) {
    if (packet) {
        u32 game = weatherGetPlayRegister();
        (weatherTranslucentDrawBuffer = gabi::load<u32>(game + gameOffset));
        u32 buffer = gabi::load<u32>(0x104B4634 + (type < 2 ? type * 4 : 0));
        weatherJ3DDrawBuffer__entryImm(buffer, packet, (u32)0);
        game = weatherGetPlayRegister();
        (weatherOpaqueDrawBuffer = static_cast<u32>(gabi::at<WeatherPlay>(game)->mpOpaque));
        game = weatherGetPlayRegister();
        (weatherTranslucentDrawBuffer =
             static_cast<u32>(gabi::at<WeatherPlay>(game)->mpTranslucent));
    }
    return packet;
}
/* 025779E4 */
static u32 dKyw_drawVrkumo(u32 type) {

    return draw_sky_packet(static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpVrkumoPacket)),
                           type, 0x5DA4);
}
/* 02577A80 */
static u32 dKyw_drawWave(u32 type) {

    if (static_cast<s16>(dKy_getWeatherEnvlight()->mWaveChan.mWaveCount) <= 0)
        return 0;
    if (!static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpWavePacket)))
        return 0;
    return draw_sky_packet(static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpWavePacket)), type,
                           0x5D74);
}
/* 02577B5C */
static void dKyw_wind_init() {

    auto *env = dKy_getWeatherEnvlight();
    float x = static_cast<float>(weatherNegativeOne.value);
    float zero = static_cast<float>(weatherZero.value);
    env->mWind.mWindVec.x = x;
    dKy_getWeatherEnvlight()->mWind.mWindVec.y = zero;
    dKy_getWeatherEnvlight()->mWind.mWindVec.z = zero;
    dKy_getWeatherEnvlight()->mWind.mWindPower = zero;
    auto *state = static_cast<WeatherSave *>(weatherSavePointer);
    bool unset = static_cast<s16>(state->mWindX) == -1 && static_cast<s16>(state->mWindY) == -1;
    if (unset) {
        dKy_getWeatherEnvlight()->mWind.mTactWindAngleFlags = 0;
        dKy_getWeatherEnvlight()->mWind.mTactWindAngleX = 0;
        dKy_getWeatherEnvlight()->mWind.mTactWindAngleY = 0;
    } else {
        dKy_getWeatherEnvlight()->mWind.mTactWindAngleFlags = 1;
        env = dKy_getWeatherEnvlight();
        env->mWind.mTactWindAngleX = static_cast<u16>(weatherSavePointer->mWindX);
        u16 angle = static_cast<u16>(weatherSavePointer->mWindY);
        dKy_getWeatherEnvlight()->mWind.mTactWindAngleY = angle;
    }
    dKy_getWeatherEnvlight()->mWind.mpWindVecOverride = nullptr;
    dKy_getWeatherEnvlight()->mWind.mWindStrengthOverride = zero;
    env = dKy_getWeatherEnvlight();
    float distant = static_cast<float>(weatherWindNoticeDisabled.value);
    env->mWind.mEvtWindSet = 0;
    dKy_getWeatherEnvlight()->mWind.field_0x8.x = distant;
    dKy_getWeatherEnvlight()->mWind.field_0x8.y = distant;
    dKy_getWeatherEnvlight()->mWind.field_0x8.z = distant;
}

/* 02582A50 */
void dKankyo_wave_Packet::destroy(u32 flags) {
    u32 object = gabi::ea(this);
    auto* packetView = this;

    if (!object)
        return;
    weatherDestroyTextureSet(gabi::ea(&packetView->mTextureSet), (u32)2);
    weatherDestroyTexture(gabi::ea(&packetView->mRenderer.mTexture), (u32)0);
    weatherDestroyRenderer(gabi::ea(&packetView->mRenderer), (u32)2);
    weatherDestroyArray(gabi::ea(&packetView->mTextures), (u32)300, (u32)0x370,
                        (u32)gabi::guest_fn((const void*)texture_destructor), (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&packetView->mGeometry), (u32)300, (u32)0x4C0,
                        (u32)gabi::guest_fn((const void*)waveGeometryPairDestroy), (u32)0, (u32)0);
    weatherDestroyPacket(object, (u32)0);
    if (flags & 1)
        weatherFree(object);
}

/* 02577CB0 */
static void dKyw_wether_init() {

    dKy_getWeatherEnvlight()->mbSunInitialized = 0;
    dKy_getWeatherEnvlight()->mThunderEff.mStatus = 0;
    dKy_getWeatherEnvlight()->mThunderEff.mMode = 0;
    dKy_getWeatherEnvlight()->mbRainInitialized = 0;
    dKy_getWeatherEnvlight()->mRainCount = 0;
    dKy_getWeatherEnvlight()->mbSnowInitialized = 0;
    dKy_getWeatherEnvlight()->mSnowCount = 0;
    dKy_getWeatherEnvlight()->mbStarInitialized = 0;
    auto *env = dKy_getWeatherEnvlight();
    float zero = static_cast<float>(weatherZero.value);
    env->mStarCount = 0;
    dKy_getWeatherEnvlight()->mStarCountTarget = zero;
    dKy_getWeatherEnvlight()->mMoyaInitialized = 0;
    dKy_getWeatherEnvlight()->mMoyaCount = 0;
    dKy_getWeatherEnvlight()->mMoyaMode = 0;
    dKy_getWeatherEnvlight()->mbHousiInitialized = 0;
    dKy_getWeatherEnvlight()->mHousiCount = 0;
    dKy_getWeatherEnvlight()->mbPoisonInitialized = 0;
    dKy_getWeatherEnvlight()->mPoisonCount = 0;
    dKy_getWeatherEnvlight()->mWind.mCustomWindPower = zero;
    dKy_getWeatherEnvlight()->mWaveInitialized = 0;
    dKy_getWeatherEnvlight()->field_0xa1c = 0;
    dKy_getWeatherEnvlight()->mbWindlineInitialized = 0;
    dKy_getWeatherEnvlight()->mWindlineCount = 0;
    weatherCall_dKyw_wind_init();
    for (u32 i = 0; i < 30; ++i)
        (dKy_getWeatherEnvlight()->mpWindInfluence[i] = gabi::at<void>(0));
    for (u32 i = 0; i < 5; ++i)
        (dKy_getWeatherEnvlight()->mWindInfluenceEntity[i].mbInUse = 0);
}

static void retire_particle_slot(u32 slot) {
    u32 particle = gabi::load<u32>(slot);
    if (particle) {
        weatherJPABaseEmitter__deleteAllParticle(particle, (u32)1);
        particle = gabi::load<u32>(slot);
        u32 flags = gabi::load<u32>(particle + 0x254);
        gabi::store<u32>(particle + 0x5C, 0xFFFFFFFF);
        gabi::store<u32>(particle + 0x254, flags | 1);
        gabi::store<u32>(slot, 0);
    }
}
/* 0257802C */
static void dKyw_wether_delete() {

    if (static_cast<u8>(dKy_getWeatherEnvlight()->mbSunInitialized)) {
        if (static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpSunlenzPacket))) {
            u32 packet = static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpSunlenzPacket));
            if (packet) {
                u32 target = gabi::load<u32>(gabi::load<u32>(packet + 0xC) + 0xC);
                gabi::call_ptr<void>(target, packet, (u32)3);
            }
            dKy_getWeatherEnvlight()->mpSunlenzPacket = gabi::at<void>(0);
        }
        delete_weather_packet(gabi::ea(&dKy_getWeatherEnvlight()->mpSunPacket));
    }
    if (static_cast<u8>(dKy_getWeatherEnvlight()->mbRainInitialized))
        weatherDeleteRainPacket(gabi::ea(&dKy_getWeatherEnvlight()->mpRainPacket));
    if (static_cast<u8>(dKy_getWeatherEnvlight()->mbSnowInitialized))
        delete_weather_packet(gabi::ea(&dKy_getWeatherEnvlight()->mpSnowPacket));
    if (static_cast<u8>(dKy_getWeatherEnvlight()->mbStarInitialized))
        weatherDeleteStarPacket(gabi::ea(&dKy_getWeatherEnvlight()->mpStarPacket));
    if (static_cast<u8>(dKy_getWeatherEnvlight()->mbPoisonInitialized))
        delete_weather_packet(gabi::ea(&dKy_getWeatherEnvlight()->mpPoisonPacket));
    if (static_cast<u8>(dKy_getWeatherEnvlight()->mbHousiInitialized))
        weatherDeleteHousiPacket(gabi::ea(&dKy_getWeatherEnvlight()->mpHousiPacket));
    if (static_cast<u8>(dKy_getWeatherEnvlight()->mMoyaInitialized))
        weatherDeleteCloudPacket(gabi::ea(&dKy_getWeatherEnvlight()->mpMoyaPacket));
    if (static_cast<u8>(dKy_getWeatherEnvlight()->mWaveInitialized))
        weatherDeleteWavePacket(gabi::ea(&dKy_getWeatherEnvlight()->mpWavePacket));
    if (static_cast<u8>(dKy_getWeatherEnvlight()->mbWindlineInitialized)) {
        u32 packet = static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpWind));
        for (u32 i = 0; i < 30; ++i)
            retire_particle_slot(packet + 0x34 * i);
        for (u32 i = 0; i < 2; ++i)
            retire_particle_slot(packet + 0x618 + 0x20 * i);
        if (static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpWind))) {
            auto *env = dKy_getWeatherEnvlight();
            u32 saved = static_cast<u32>(gabi::ea(env->mpWind));
            if (saved) {
                env->mpWind = gabi::at<void>(0);
                weatherDelete(saved);
            }
        }
    }
}

/* 02580FC8 */
void dKankyo_star_Packet::destroy(u32 flags) {
    u32 object = gabi::ea(this);
    auto* packetView = this;

    if (!object)
        return;
    weatherDestroyArray(gabi::ea(&packetView->mRenderer), (u32)8, (u32)0x41C,
                        (u32)gabi::guest_fn((const void*)texture_pair_destructor), (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&packetView->mGeometry), (u32)2000, (u32)0x968,
                        (u32)gabi::guest_fn((const void*)four_geometry_destructor), (u32)0, (u32)0);
    weatherDestroyPacket(object, (u32)0);
    if (flags & 1)
        weatherFree(object);
}

/* 02581440 */
void dKankyo_poison_Packet::destroy(u32 flags) {
    u32 object = gabi::ea(this);
    auto* packetView = this;

    if (!object)
        return;
    weatherDestroyTextureSet(gabi::ea(&packetView->mTextureSet), (u32)2);
    weatherDestroyTexture(gabi::ea(&packetView->mRenderer.mTexture), (u32)0);
    weatherDestroyRenderer(gabi::ea(&packetView->mRenderer), (u32)2);
    weatherDestroyArray(gabi::ea(&packetView->mTextures), (u32)1000, (u32)0x370,
                        (u32)gabi::guest_fn((const void*)texture_destructor), (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&packetView->mGeometry), (u32)1000, (u32)0x4C0,
                        (u32)gabi::guest_fn((const void*)poisonGeometryPairDestroy), (u32)0,
                        (u32)0);
    weatherDestroyPacket(object, (u32)0);
    if (flags & 1)
        weatherFree(object);
}

/* 02581B60 */
void dKankyo_housi_Packet::destroy(u32 flags) {
    u32 object = gabi::ea(this);
    auto* packetView = this;

    if (!object)
        return;
    weatherDestroyTextureSet(gabi::ea(&packetView->mTextureSet), (u32)2);
    weatherDestroyTexture(gabi::ea(&packetView->mRenderer.mTexture), (u32)0);
    weatherDestroyRenderer(gabi::ea(&packetView->mRenderer), (u32)2);
    weatherDestroyArray(gabi::ea(&packetView->mTextures), (u32)600, (u32)0x370,
                        (u32)gabi::guest_fn((const void*)texture_destructor), (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&packetView->mGeometry), (u32)600, (u32)0x4C0,
                        (u32)gabi::guest_fn((const void*)housiGeometryPairDestroy), (u32)0, (u32)0);
    weatherDestroyPacket(object, (u32)0);
    if (flags & 1)
        weatherFree(object);
}

/* 02582020 */
void dKankyo_cloud_Packet::destroy(u32 flags) {
    u32 object = gabi::ea(this);
    auto* packetView = this;

    if (!object)
        return;
    weatherDestroyTextureSet(gabi::ea(&packetView->mTextureSet), (u32)2);
    weatherDestroyTexture(gabi::ea(&packetView->mRenderer.mTexture), (u32)0);
    weatherDestroyRenderer(gabi::ea(&packetView->mRenderer), (u32)2);
    weatherDestroyArray(gabi::ea(&packetView->mTextures), (u32)30, (u32)0x370,
                        (u32)gabi::guest_fn((const void*)texture_destructor), (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&packetView->mGeometry), (u32)30, (u32)0x4C0,
                        (u32)gabi::guest_fn((const void*)cloudGeometryPairDestroy), (u32)0, (u32)0);
    weatherDestroyPacket(object, (u32)0);
    if (flags & 1)
        weatherFree(object);
}

/* 0257E1B8 */
static void dKyw_get_AllWind_vec(u32 position, u32 direction, u32 power) {
    auto *windPower = gabi::at<be<f32>>(power);

    auto *env = dKy_getWeatherEnvlight();
    gabi::Local<Vector> temporary, globalPart, pointPart, normalized;
    weatherdKyw_pntwind_get_info(position, direction, power);
    float complement =
        gabi::fsubs_ppc(static_cast<float>(weatherOne.value), static_cast<f32>(*windPower));
    float scale = gabi::fmuls_ppc(static_cast<float>(env->mWind.mWindPower), complement);
    weatherScaleVector(gabi::ea(&env->mWind.mWindVec), temporary.a, scale);
    copyVectorBits(globalPart.a, temporary.a);
    scale = gabi::fmuls_ppc(static_cast<f32>(*windPower),
                            static_cast<float>(weatherPointWindScale.value));
    weatherScaleVector(direction, temporary.a, scale);
    copyVectorBits(pointPart.a, temporary.a);
    weatherAddVectors(globalPart.a, temporary.a, pointPart.a);
    copyVectorBits(normalized.a, temporary.a);
    float lengthSquared = weatherVectorSquaredLength(normalized.a);
    float length = weatherSqrt(lengthSquared);
    *windPower = length;
    weatherNormalizeVector(normalized.a, temporary.a);
    copyVectorBits(normalized.a, temporary.a);
    u32 nonzero = weatherVectorNotEqual(normalized.a, gabi::ea(&weatherZeroVector));
    if (nonzero)
        copyVectorBits(direction, normalized.a);
    else
        copyVectorBits(direction, gabi::ea(&env->mWind.mWindVec));
}
/* 0257963C */
static void weatherNormalizeRgba(u32 output, u32 rgba) {
    // HD: returns a four-float color through the EABI result buffer.
    auto *source = gabi::at<WeatherColorBytes>(rgba);
    auto *destination = gabi::at<ColorFloat>(output);
    float red = static_cast<u8>(source->red);
    float green = static_cast<u8>(source->green);
    float denominator = weatherColorScale.value;
    float blue = static_cast<u8>(source->blue);
    float alpha = static_cast<u8>(source->alpha);
    float values[4] = {red / denominator, green / denominator, blue / denominator,
                       alpha / denominator};
    for (u32 i = 0; i < 4; ++i)
        destination->channels[i] = values[i];
}

/* 0257E93C */
static void weatherCopyViewMatrix(u32 output, u32 input) {
    auto *source = gabi::at<WeatherMatrix>(input);
    auto *destination = gabi::at<WeatherMatrix>(output);
    float matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = source->values[i];
    for (u32 i = 0; i < 12; ++i)
        destination->values[i] = matrix[i];
}

static float divide_single(float numerator, float denominator) {
    if (numerator != numerator)
        return gabi::ppc_qnan(numerator);
    if (denominator != denominator)
        return gabi::ppc_qnan(denominator);
    return (float)((double)numerator / (double)denominator);
}
static float subtract_product(float first, float multiplier, float from) {
    if (first != first)
        return gabi::ppc_qnan(first);
    if (multiplier != multiplier)
        return gabi::ppc_qnan(multiplier);
    if (from != from)
        return gabi::ppc_qnan(from);
    return gabi::fnmsubs(first, multiplier, from);
}
/* 0257DE68 */
static void dKyw_pntwind_get_info(u32 position, WeatherVector *i_direction, u32 power) {
    auto *windPower = gabi::at<be<f32>>(power);
    u32 direction = gabi::ea(i_direction);

    gabi::Local<Vector> origin;
    float zero = static_cast<float>(weatherZero.value);
    (i_direction->y = zero);
    (i_direction->x = zero);
    (i_direction->z = zero);
    *windPower = zero;
    for (u32 i = 0; i < 30; ++i) {
        if (!gabi::ea(dKy_getWeatherEnvlight()->mpWindInfluence[i]))
            continue;
        u32 influence = gabi::ea(dKy_getWeatherEnvlight()->mpWindInfluence[i]);
        auto *influenceView = gabi::at<WeatherWindInfluence>(influence);
        float squared = weatherVectorSquaredDistance(position, influence);
        float distance = weatherSqrt(squared);
        float radius = static_cast<float>(influenceView->mRadius);
        if (!(distance < radius))
            continue;
        float strength = static_cast<float>(influenceView->mStrength);
        if (!(strength > zero) || distance == zero)
            continue;
        bool cylinder = static_cast<u8>(influenceView->mbConstant) != 0;
        float one = static_cast<float>(weatherOne.value);
        if (!cylinder) {
            float attenuation = one;
            if (radius > zero) {
                float relative = divide_single(distance, radius);
                attenuation = subtract_product(relative, relative, one);
            }
            *windPower = gabi::fmuls_ppc(strength, attenuation);
            for (u32 axis = 0; axis < 3; ++axis) {
                float vector = static_cast<f32>(influenceView->mDir.channels[axis]);
                float currentRadius = static_cast<float>(influenceView->mRadius);
                float center = static_cast<f32>(influenceView->mPos.channels[axis]);
                gabi::store<float>(origin.a + 4 * axis,
                                   subtract_product(vector, currentRadius, center));
            }
            weatherdKyr_get_vectle_calc(origin.a, position, direction);
            squared = weatherVectorSquaredDistance(position, origin.a);
            distance = weatherSqrt(squared);
            radius = static_cast<float>(influenceView->mRadius);
            if (distance < radius) {
                (i_direction->y = zero);
                (i_direction->x = zero);
                (i_direction->z = zero);
            }
            return;
        }
        u32 x = static_cast<u32>(influenceView->mDir.words[0]);
        float relative = radius > zero ? divide_single(distance, radius) : zero;
        (i_direction->words[0] = x);
        (i_direction->words[1] = static_cast<u32>(influenceView->mDir.words[1]));
        (i_direction->words[2] = static_cast<u32>(influenceView->mDir.words[2]));
        strength = static_cast<float>(influenceView->mStrength);
        float attenuation = radius > zero ? gabi::fsubs_ppc(one, relative) : one;
        *windPower = gabi::fmuls_ppc(strength, attenuation);
        return;
    }
}

static float add_product(float first, float multiplier, float to) {
    if (first != first)
        return gabi::ppc_qnan(first);
    if (multiplier != multiplier)
        return gabi::ppc_qnan(multiplier);
    if (to != to)
        return gabi::ppc_qnan(to);
    return gabi::fmadds(first, multiplier, to);
}
/* 0257D208 */
static void squal_proc() {
    auto *env = dKy_getWeatherEnvlight();
    f32 one = weatherOne.value;
    f32 damping = weatherSquallDamping.value;
    f32 cutoff = weatherStarStep.value;
    f32 zero = weatherZero.value;
    f32 speed = weatherSquallRadiusScale.value;
    f32 minimum = weatherWindMinimumStep.value;
    for (u32 i = 0; i < 5; ++i) {
        auto *squall = &env->mWindInfluenceEntity[i];
        auto *influence = gabi::at<WeatherWindInfluence>(gabi::ea(&squall->mInfluence));
        if (squall->mbInUse != 1)
            continue;
        f32 rate = squall->mSpeed;
        f32 x = influence->mPos.x;
        f32 dx = influence->mDir.x;
        f32 y = influence->mPos.y;
        influence->mPos.x = add_product(dx, rate, x);
        rate = squall->mSpeed;
        f32 dy = influence->mDir.y;
        f32 dz = influence->mDir.z;
        influence->mPos.y = add_product(dy, rate, y);
        f32 z = influence->mPos.z;
        rate = squall->mSpeed;
        influence->mPos.z = add_product(dz, rate, z);
        f32 step = squall->mStrengthMaxVel;
        weatherAddCalc(gabi::ea(&influence->mStrength), zero, damping, step, minimum);
        f32 current = influence->mStrength;
        f32 amplitude = squall->mMinRadius;
        f32 target = gabi::fsubs_ppc(one, current);
        f32 maxStep = gabi::fmuls_ppc(gabi::fmuls_ppc(target, amplitude), speed);
        weatherAddCalc(gabi::ea(&influence->mRadius), amplitude, target, maxStep, cutoff);
        if (influence->mStrength < cutoff) {
            weatherdKyw_pntwind_cut(gabi::ea(influence));
            squall->mbInUse = 0;
        }
    }
}

/* 0257DC9C */
static void dKyw_squal_set(WeatherVector *i_pos, u32 i_x, u32 i_y, f32 i_radius, f32 i_minRadius,
                           f32 i_strength, f32 i_speed, f32 i_maxStrength) {
    auto *env = dKy_getWeatherEnvlight();
    auto *tableX = &weatherSinCosTable.mEntries[(i_x & 0xFFFF) >> 3];
    auto *tableY = &weatherSinCosTable.mEntries[(i_y & 0xFFFF) >> 3];
    for (u32 i = 0; i < 5; ++i) {
        auto *squall = &env->mWindInfluenceEntity[i];
        if (squall->mbInUse)
            continue;
        squall->mStrengthMaxVel = i_maxStrength;
        squall->mMinRadius = i_minRadius;
        squall->mSpeed = i_speed;
        squall->mbInUse = 1;
        auto *influence = gabi::at<WeatherWindInfluence>(gabi::ea(&squall->mInfluence));
        copyVectorBits(gabi::ea(&influence->mPos), gabi::ea(i_pos));
        f32 cosine = tableX->cosine.value;
        f32 sine = tableY->sine.value;
        influence->mDir.x = gabi::fmuls_ppc(cosine, sine);
        // HD: the y channel is a word copy, preserving signaling NaNs.
        influence->mDir.words[1] = tableX->sine.bits;
        cosine = tableX->cosine.value;
        f32 cosineY = tableY->cosine.value;
        influence->mStrength = i_strength;
        influence->mRadius = i_radius;
        influence->mInitialRadius = i_radius;
        influence->mDir.z = gabi::fmuls_ppc(cosine, cosineY);
        weatherdKyw_pntwind_set(gabi::ea(influence));
        return;
    }
}

// HD: GHS expands construction of parameter vectors and matrix storage in
// native order.
static void construct_texture_constants(WeatherTextureParameters* parameters) {
    float zero = weatherTextureZero.value;
    float one = weatherTextureOne.value;
    for (u32 i = 0; i < 44; ++i)
        parameters->mValues[i] = i % 4 == 3 ? one : zero;
    for (u32 i = 0; i < 3; ++i)
        weatherConstructArray(gabi::ea(&parameters->mColorPairs[i]), 2u, sizeof(ColorFloat),
                              gabi::guest_fn((const void*)allocate_empty16));
    for (u32 i = 0; i < 8; ++i)
        if (gabi::ea(&parameters->mMatrixStorage[i]) == 0)
            weatherAllocateDiscard(0x30u);
    for (u32 i = 0; i < 6; ++i)
        if (gabi::ea(&parameters->mVectorStorage[i]) == 0)
            weatherAllocateDiscard(sizeof(ColorFloat));
}

// HD: earlier packet setup passes its embedded parameter block by offset.
static void construct_texture_constants(u32 owner, u32 offset) {
    construct_texture_constants(gabi::at<WeatherTextureParameters>(owner + offset));
}

/* 02582E0C */
static u32 dual_texture_constructor(u32 object) {
    if (!object)
        object = weatherAllocateRegister((u32)0x41C);
    if (!object)
        return 0;
    weatherConstructRenderer(object);
    weatherConstructTexture(gabi::ea(&gabi::at<WeatherRenderer>(object)->mPrefix.mTexture));
    gabi::at<WeatherRenderer>(object)->mPrefix.mTexture.__vtbl =
        gabi::ea(&weatherSmallTextureVtable);
    weatherClear(gabi::ea(&gabi::at<WeatherRenderer>(object)->mPrefix.mTexture.mParameters),
                 (u32)0x34);
    if (gabi::ea(&gabi::at<WeatherRenderer>(object)->mPrefix.mTexture.mParameters) == 0)
        weatherAllocateDiscard((u32)0x30);
    weatherConstructTexture(gabi::ea(&gabi::at<WeatherRenderer>(object)->mExtendedTexture));
    gabi::at<WeatherRenderer>(object)->mExtendedTexture.__vtbl = gabi::ea(&weatherTextureVtable);
    weatherClear(gabi::ea(&gabi::at<WeatherRenderer>(object)->mExtendedTexture.mParameters),
                 (u32)0x2F0);
    construct_texture_constants(&gabi::at<WeatherRenderer>(object)->mExtendedTexture.mParameters);
    gabi::at<WeatherRenderer>(object)->mStatus = 0;
    return object;
}
/* 02583210 */
static u32 texture_with_constants_constructor(u32 object) {
    if (!object)
        object = weatherAllocateRegister((u32)0x370);
    if (!object)
        return 0;
    weatherConstructTexture(object);
    gabi::at<WeatherTexture>(object)->__vtbl = gabi::ea(&weatherTextureVtable);
    weatherClear(gabi::ea(&gabi::at<WeatherTexture>(object)->mParameters), (u32)0x2F0);
    construct_texture_constants(&gabi::at<WeatherTexture>(object)->mParameters);
    gabi::at<WeatherTexture>(object)->mFlags = 0;
    gabi::at<WeatherTexture>(object)->__vtbl = gabi::ea(&weatherBufferedTextureVtable);
    gabi::at<WeatherTexture>(object)->mBufferIndex = 0;
    return object;
}

/* 02582410 */
void dKankyo_vrkumo_Packet::destroy(u32 flags) {
    u32 object = gabi::ea(this);
    auto* packetView = this;

    if (!object)
        return;
    weatherDestroyArray(gabi::ea(&packetView->mTextureBindings), (u32)3, (u32)0x198,
                        gabi::guest_fn((const void*)weatherDestroySampler), (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&packetView->mTextures), (u32)100, (u32)0x370,
                        (u32)gabi::guest_fn((const void*)texture_destructor), (u32)0, (u32)0);
    weatherDestroyTexture(gabi::ea(&packetView->mRenderer.mExtendedTexture), (u32)0);
    weatherDestroyTexture(gabi::ea(&packetView->mRenderer.mPrefix.mTexture), (u32)0);
    weatherDestroyRenderer(gabi::ea(&packetView->mRenderer), (u32)2);
    weatherDestroyArray(gabi::ea(&packetView->mGeometry), (u32)2, (u32)0x574F8,
                        (u32)gabi::guest_fn((const void*)precipitation_geometry_destructor), (u32)0,
                        (u32)0);
    weatherDestroyPacket(object, (u32)0);
    if (flags & 1)
        weatherFree(object);
}

/* 02580204 */
void dKankyo_snow_Packet::draw() {
    auto* packetView = this;

    weatherDrawKazanbai(gabi::ea(&weatherViewMatrix), gabi::ea(&packetView->mpTexture));
}

/* 02580FB8 */
void dKankyo_star_Packet::draw() {
    auto* packetView = this;

    weatherDrawStar(gabi::ea(&weatherViewMatrix), gabi::ea(&packetView->mpTexture));
}

/* 0258142C */
void dKankyo_poison_Packet::draw() {
    auto* packetView = this;

    weatherDrawPoison(gabi::ea(&weatherViewMatrix), gabi::ea(&packetView->mpTexture));
}

/* 02581B50 */
void dKankyo_housi_Packet::draw() {
    auto* packetView = this;

    weatherDrawHousi(gabi::ea(&weatherViewMatrix), gabi::ea(&packetView->mpTex));
}

/* 02582010 */
void dKankyo_cloud_Packet::draw() {
    auto* packetView = this;

    weatherDrawCloudShadow(gabi::ea(&weatherViewMatrix), gabi::ea(&packetView->mpTexture));
}

/* 02582400 */
void dKankyo_vrkumo_Packet::draw() {
    auto* packetView = this;

    weatherDrawVrkumo(gabi::ea(&weatherViewMatrix), gabi::ea(&packetView->mpCloudTextures));
}

/* 02580214 */
void dKankyo_snow_Packet::destroy(u32 flags) {
    u32 object = gabi::ea(this);
    auto* packetView = this;

    if (!object)
        return;
    weatherDestroyTextureSet(gabi::ea(&packetView->mTextureSet), (u32)2);
    weatherDestroyTexture(gabi::ea(&packetView->mRenderer.mTexture), (u32)0);
    weatherDestroyRenderer(gabi::ea(&packetView->mRenderer), (u32)2);
    weatherDestroyArray(gabi::ea(&packetView->mTextures), (u32)4000, (u32)0x370,
                        (u32)gabi::guest_fn((const void*)texture_destructor), (u32)0, (u32)0);
    weatherDestroyTexture(gabi::ea(&packetView->mSnowTexture), (u32)0);
    weatherDestroyArray(gabi::ea(&packetView->mSnowGeometry), (u32)4000, (u32)0x4C0,
                        (u32)gabi::guest_fn((const void*)snowGeometryPairDestroy), (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&packetView->mSplashGeometry), (u32)1000, (u32)0x4C0,
                        (u32)gabi::guest_fn((const void*)snowGeometryPairDestroy), (u32)0, (u32)0);
    weatherDestroyPacket(object, (u32)0);
    if (flags & 1)
        weatherFree(object);
}

static void string_prepare(u32 object) {
    auto* string = gabi::at<SafeString>(object);
    auto* table = gabi::at<WeatherStringVtable>(string->vtable);
    gabi::call_ptr<void>(static_cast<u32>(table->mpPrepare), object);
}
static bool same_string_bytes(u32 first, u32 second) {
    if (first == second)
        return true;
    auto* firstText = gabi::at<be<u8>>(first);
    auto* secondText = gabi::at<be<u8>>(second);
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = firstText[i], b = secondText[i];
        if (a != b)
            return false;
        if (a == 0)
            return true;
    }
    return false;
}
/* 0257836C */
static void wether_move_windline() {

    u8 phase = static_cast<u8>(dKy_getWeatherEnvlight()->mThunderEff.mStatus);
    if (phase == 0) {
        if (static_cast<u32>(dKy_getWeatherEnvlight()->mThunderEff.mMode)) {
            weatherCall_dKyr_thunder_init();
            dKy_getWeatherEnvlight()->mThunderEff.mStatus = 1;
        }
    } else if (phase == 1)
        weatherdKyr_thunder_move();
    weatherGetPlayDiscard();
    s32 room = static_cast<s8>(weatherStayRoomNo);
    u32 roomInfo = 0;
    if (room >= 0) {
        u32 game = weatherGetPlayRegister();
        u32 status = weatherdStage_roomControl_c__getStatusRoomDtRegister(game + 0x51CC, room);
        u32 target = static_cast<u32>(gabi::at<WeatherRoomVtable>(gabi::at<WeatherRoomStatus>(status)->mpVtable)->mpGetFileList);
        roomInfo = gabi::call_ptr<u32>(target, status);
    }
    dKy_getWeatherEnvlight()->mWindlineCount = 0;
    if (roomInfo && (static_cast<u32>(gabi::at<WeatherRoomFileList>(roomInfo)->mFlags) & 0x100000)) {
        gabi::Local<SafeString> stage, gameStage;
        gabi::at<SafeString>(stage.a)->text = gabi::guest_addr("GTower");
        gabi::at<SafeString>(stage.a)->vtable = gabi::ea(&weatherStringVtable);
        u32 game = weatherGetPlayRegister();
        gabi::at<SafeString>(gameStage.a)->text = gabi::ea(&gabi::at<WeatherPlay>(game)->mStageName);
        gabi::at<SafeString>(gameStage.a)->vtable = gabi::ea(&weatherStringVtable);
        string_prepare(stage.a);
        string_prepare(stage.a);
        u32 stageText = static_cast<u32>(gabi::at<SafeString>(stage.a)->text);
        string_prepare(gameStage.a);
        u32 gameText = static_cast<u32>(gabi::at<SafeString>(gameStage.a)->text);
        bool same = stageText == gameText;
        if (!same)
            same = same_string_bytes(static_cast<u32>(gabi::at<SafeString>(stage.a)->text), static_cast<u32>(gabi::at<SafeString>(gameStage.a)->text));
        if (!same) {
            auto *env = dKy_getWeatherEnvlight();
            float power = weatherdKyw_get_wind_pow();
            float scaled = gabi::fmuls_ppc(power, static_cast<float>(weatherWindlineScale.value));
            env->mWindlineCount = gabi::ftoi(scaled);
        }
    }
    float disabled = static_cast<float>(weatherWindNoticeDisabled.value);
    if (static_cast<float>(dKy_getWeatherEnvlight()->mWind.field_0x8.y) < disabled) {
        dKy_getWeatherEnvlight()->mWindlineCount = 0;
        dKy_getWeatherEnvlight()->mWind.field_0x8.y = disabled;
    }
    phase = static_cast<u8>(dKy_getWeatherEnvlight()->mbWindlineInitialized);
    if (phase == 0) {
        if (!static_cast<u32>(dKy_getWeatherEnvlight()->mWindlineCount))
            return;
        // The native scope constructor allocates/writes exactly one guest pointer (4 bytes).
        gabi::Local<gabi::be<u32>> heapScope;
        weatherBeginHeap(heapScope.a, gabi::guest_addr("WINDEFF_SET"), (u32)0x82C, (u32)0);
        auto *env = dKy_getWeatherEnvlight();
        u32 packet = weatherAllocateAligned((u32)0x768, (u32)32);
        env->mpWind = gabi::at<void>(packet);
        if (static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpWind))) {
            for (u32 i = 0; i < 30; ++i) {
                packet = static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpWind));
                gabi::at<WeatherWindlinePacket>(packet)->mEffects[i].mActive = 0;
                packet = static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpWind));
                gabi::at<WeatherWindlinePacket>(packet)->mEffects[i].mStatus = 0;
            }
            weatherdKyr_wind_init();
            weatherdKyr_wind_move();
            dKy_getWeatherEnvlight()->mbWindlineInitialized = 1;
        } else
            weatherAbortHeap(heapScope.a);
        weatherEndHeap(heapScope.a, (u32)2);
    } else if (phase == 1)
        weatherdKyr_wind_move();
}

/* 02582C08 */
static u32 safe_string_equal(u32 first, u32 second) {
    auto* left = gabi::at<SafeString>(first);
    auto* right = gabi::at<SafeString>(second);
    string_prepare(first);
    string_prepare(first);
    u32 cached = left->text;
    string_prepare(second);
    if (cached == static_cast<u32>(right->text))
        return 1;
    return same_string_bytes(static_cast<u32>(left->text), static_cast<u32>(right->text));
}

static u32 archive_resource_array(u32 archive) {
    auto* header = gabi::at<WeatherArchiveHeader>(archive);
    u32 offset = header->mTextureDictionaryOffset;
    return offset ? gabi::ea(&header->mTextureDictionaryOffset) + offset : 0;
}
/* 02583D00 */
static void wether_move_wave() {
    u8 phase = dKy_getWeatherEnvlight()->mWaveInitialized;
    switch (phase) {
    case WEATHER_INITIALIZE: {
        if (dKy_getWeatherEnvlight()->mWaveChan.mWaveCount == 0)
            return;
        gabi::Local<be<u32>> heapScope;
        // HD: the original heap label includes the Japanese description for waves.
        weatherBeginHeap(heapScope.a, gabi::guest_addr("dKankyo_wave_Packet"), 0x17FA10u, 0u);
        u32 packet = weatherAllocateAligned(0x9DCC0u, 32u);
        if (packet)
            packet = weatherConstructWavePacket(packet);
        dKy_getWeatherEnvlight()->mpWavePacket = gabi::at<void>(packet);
        if (gabi::ea(dKy_getWeatherEnvlight()->mpWavePacket)) {
            gabi::Local<SafeString> package, entry;
            package.get()->text = gabi::guest_addr("Always");
            package.get()->vtable = gabi::ea(&weatherStringVtable);
            entry.get()->text = gabi::guest_addr("Always.bfres");
            entry.get()->vtable = gabi::ea(&weatherStringVtable);
            u32 resource = weatherGetArchiveResource(static_cast<u32>(weatherArchiveManager),
                                                     package.a, entry.a, 0u);
            u32 archive = weatherGetArchive(resource);
            u32 texture = weatherGetArchiveTexture(archive_resource_array(archive),
                                                   gabi::guest_addr("txa_usonami_256_64mip"));
            packet = gabi::ea(dKy_getWeatherEnvlight()->mpWavePacket);
            gabi::at<dKankyo_wave_Packet>(packet)->mpTextures[0] = texture;
            texture = weatherGetArchiveTexture(archive_resource_array(archive),
                                               gabi::guest_addr("txa_usonami_m_256_64mip"));
            packet = gabi::ea(dKy_getWeatherEnvlight()->mpWavePacket);
            gabi::at<dKankyo_wave_Packet>(packet)->mpTextures[1] = texture;
            weatherInitializeWaveGeometry(gabi::ea(dKy_getWeatherEnvlight()->mpWavePacket));
            for (u32 i = 0; i < 300; ++i) {
                packet = gabi::ea(dKy_getWeatherEnvlight()->mpWavePacket);
                gabi::at<dKankyo_wave_Packet>(packet)->mEff[i].mStatus = 0;
            }
            weatherMoveWave();
            dKy_getWeatherEnvlight()->mWaveInitialized = WEATHER_MOVE;
        } else {
            weatherAbortHeap(heapScope.a);
        }
        weatherEndHeap(heapScope.a, 2u);
        break;
    }
    case WEATHER_MOVE:
        if (dKy_getWeatherEnvlight()->mWaveChan.mWaveCount == 0)
            dKy_getWeatherEnvlight()->mWaveInitialized = WEATHER_DRAIN;
        else
            weatherMoveWave();
        break;
    case WEATHER_DRAIN:
        dKy_getWeatherEnvlight()->mWaveInitialized = WEATHER_DELETE;
        break;
    case WEATHER_DELETE:
        dKy_getWeatherEnvlight()->mWaveInitialized = WEATHER_INITIALIZE;
        weatherDeleteWavePacket(gabi::ea(&dKy_getWeatherEnvlight()->mpWavePacket));
        break;
    }
}

/* 02582518 */
void dKankyo_wave_Packet::setColors(u32 index, u32 colorA, u32 colorB) {
    gabi::Local<ColorFloat> normalized, secondary;
    weatherNormalizeColor(normalized.a, colorA);
    auto* texture = &mTextures[index];
    u32 environment = weatherGetEnvironment();
    weatherScaleColor(
        gabi::ea(&texture->mParameters.mPrimaryColor), normalized.a,
        static_cast<f32>(gabi::at<dKankyo::dScnKy_env_light_c>(environment)->field_0x10bc));
    float denominator = weatherColorScale.value;
    texture->mParameters.mPrimaryColor.alpha =
        divide_single((float)gabi::at<WeatherColorBytes>(colorA)->alpha, denominator);
    weatherSetPrimaryColor(gabi::ea(texture), gabi::ea(&texture->mParameters.mPrimaryColor), 0u);
    weatherNormalizeColor(normalized.a, colorB);
    environment = weatherGetEnvironment();
    weatherScaleColor(
        secondary.a, normalized.a,
        static_cast<f32>(gabi::at<dKankyo::dScnKy_env_light_c>(environment)->field_0x10b8));
    secondary.get()->alpha =
        divide_single((float)gabi::at<WeatherColorBytes>(colorB)->alpha, denominator);
    weatherSetSecondaryColor(gabi::ea(texture), secondary.a, 0u);
    weatherSetSecondaryColor(gabi::ea(texture), secondary.a, 3u);
}

/* 0257FE7C */
u32 dKankyo_snow_Packet::construct() {
    u32 object = gabi::ea(this);

    if (!object)
        object = weatherAllocateRegister((u32)0x92B7E0);
    if (!object)
        return 0;
    auto* packetView = gabi::at<dKankyo_snow_Packet>(object);
    weatherConstructPacket(object);
    packetView->mPacket.__vtbl = gabi::ea(&weatherSnowVtable);
    weatherClear(gabi::ea(&packetView->mpTexture), (u32)4);
    weatherClear(gabi::ea(&packetView->mEff), (u32)0x36B0);
    float zero = static_cast<float>(weatherZero.value);
    packetView->field_0x36d4 = zero;
    packetView->mTamariStart = 0;
    packetView->field_0x36d0 = zero;
    packetView->mpMaterial = 0;
    packetView->field_0x36dc = 0;
    packetView->mEffCount = 0;
    weatherConstructArray(gabi::ea(&packetView->mSplashGeometry), (u32)1000, (u32)0x4C0,
                          (u32)gabi::guest_fn((const void*)snowGeometryPairConstruct));
    weatherConstructArray(gabi::ea(&packetView->mSnowGeometry), (u32)4000, (u32)0x4C0,
                          (u32)gabi::guest_fn((const void*)snowGeometryPairConstruct));
    u32 texture = gabi::ea(&packetView->mSnowTexture);
    weatherConstructTexture(texture);
    gabi::at<WeatherTexture>(texture)->__vtbl = gabi::ea(&weatherTextureVtable);
    weatherClear(texture + 0x74, (u32)0x2F0);
    construct_texture_constants(&gabi::at<WeatherTexture>(texture)->mParameters);
    gabi::at<WeatherTexture>(texture)->__vtbl = gabi::ea(&weatherBufferedTextureVtable);
    gabi::at<WeatherTexture>(texture)->mFlags = 0;
    gabi::at<WeatherTexture>(texture)->mBufferIndex = 0;
    weatherConstructArray(gabi::ea(&packetView->mTextures), (u32)4000, (u32)0x370,
                          (u32)gabi::guest_fn((const void*)texture_with_constants_constructor));
    u32 renderer = gabi::ea(&packetView->mRenderer);
    weatherConstructRenderer(renderer);
    weatherConstructTexture(gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture));
    gabi::at<WeatherRendererPrefix>(renderer)->mTexture.__vtbl =
        gabi::ea(&weatherSmallTextureVtable);
    weatherClear(gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture.mParameters),
                 (u32)0x34);
    if (gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture.mParameters) == 0)
        weatherAllocateDiscard((u32)0x30);
    weatherConstructTextureSet(gabi::ea(&packetView->mTextureSet));
    return object;
}
static u32 lookup_weather_material(u32 packet, u32 name) {
    gabi::Local<SafeString> text;
    text.get()->text = name;
    text.get()->vtable = gabi::ea(&weatherStringVtable);
    auto* state = gabi::at<WeatherMaterialRegistry>(weatherGetMaterialState(packet));
    u32 index = weatherFindMaterial(gabi::ea(state->mpMaterials), text.a);
    if ((s32)index < 0)
        return 0;
    u32 count = state->mCount;
    auto* table = static_cast<WeatherMaterialEntry*>(state->mpEntries);
    auto* entry = &table[index < count ? index : 0];
    if (!static_cast<u8>(entry->mbInitialized)) {
        auto* materials = static_cast<WeatherMaterialResources*>(state->mpMaterials);
        u32 materialCount = materials->mCount;
        u32 material = index < materialCount ? gabi::ea(&materials->mpResources[index]) : 0;
        weatherInitializeMaterial(gabi::ea(entry), material, 0u);
        count = state->mCount;
        table = state->mpEntries;
    }
    return gabi::ea(&table[index < count ? index : 0]);
}
static void prepare_weather_pair(u32 pair, u32 material, u32 attributes) {
    auto* i_pair = gabi::at<WeatherGeometryPair>(pair);
    (i_pair->mPrimitive = 17);
    (i_pair->mpAttributes = attributes);
    for (u32 side = 0; side < 2; ++side) {
        u32 element = pair + side * 0x254;
        auto* elementView = gabi::at<WeatherGeometry>(element);
        u32 data = static_cast<u32>(elementView->mpVertices);
        if (!data) {
            u32 allocator = weatherGetAllocator(static_cast<u32>(weatherVertexAllocator), data);
            auto* table =
                static_cast<WeatherHeapVtable*>(gabi::at<WeatherHeap>(allocator)->mpVtable);
            u32 allocation = gabi::call_ptr<u32>(static_cast<u32>(table->mpAllocate), allocator,
                                                 (u32)0x50, (u32)0x40);
            if (allocation) {
                (elementView->mpAllocation = allocation);
                (elementView->mVertexCount = 4);
            }
            data = static_cast<u32>(elementView->mpAllocation);
            (elementView->mpVertices = data);
        }
        weatherInitializeVertices(element + 4, data, (u32)4, gabi::ea(&i_pair->mPrimitive));
        if (material && material != static_cast<u32>(i_pair->mpMaterial))
            weatherInitializeGeometryMaterial(material, gabi::ea(&elementView->mMaterialBinding[0]),
                                              element + 4, gabi::ea(&i_pair->mPrimitive), (u32)0);
    }
    (i_pair->mpMaterial = material);
    (i_pair->mbInitialized = 1);
}

static bool weather_stage_equals(u32 name);
/* 02580318 */
void dKankyo_snow_Packet::initializeGeometry() {
    u32 packet = gabi::ea(this);
    auto* packetView = this;

    packetView->mpMaterial = lookup_weather_material(packet, gabi::guest_addr("housi"));
    bool special = weather_stage_equals(gabi::guest_addr("Adanmae"));
    if (special) {
        for (u32 i = 0; i < 4000; ++i) {
            prepare_weather_pair(gabi::ea(&packetView->mSnowGeometry[i]),
                                 static_cast<u32>(packetView->mpMaterial),
                                 gabi::ea(&weatherSnowAttributes));
            weatherSetTextureMode(gabi::ea(&packetView->mTextures[i]), (u32)0);
        }
    } else {
        for (u32 i = 0; i < 250; ++i)
            for (u32 quad = 0; quad < 4; ++quad)
                prepare_weather_pair(gabi::ea(&packetView->mSplashGeometry[i * 4 + quad]),
                                     static_cast<u32>(packetView->mpMaterial),
                                     gabi::ea(&weatherSnowAttributes));
    }
    gabi::Local<WeatherTextureDescriptor> descriptor;
    weatherConstructTextureDescriptor(descriptor.a);
    u32 resource = static_cast<u32>(packetView->mpTexture);
    weatherLockTextures(static_cast<u32>(weatherTextureManager));
    weatherImportTexture(descriptor.a, resource);
    weatherUnlockTextures(static_cast<u32>(weatherTextureManager));
    u32 renderer = gabi::ea(&packetView->mRenderer);
    weatherSetTexture(gabi::ea(&packetView->mTextureSet), (u32)0, descriptor.a, (u32)0);
    u32 textureState = weatherGetTexture(gabi::ea(&packetView->mTextureSet), (u32)0);
    auto* binding = gabi::at<WeatherTextureBinding>(textureState);
    u8 flags = binding->mFlags;
    binding->mSamplerStateDirty[0] = 1;
    binding->mSamplerStateDirty[1] = 1;
    binding->mSamplerStateDirty[2] = 1;
    binding->mFlags = flags | 2;
    weatherSetTextureMode(gabi::ea(&packetView->mSnowTexture), (u32)0);
    weatherSetRendererMode(renderer, (u32)1, (u32)0);
    weatherSetTextureMode(gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture), (u32)0);
}

static void prepare_weather_element(u32 element, u32 attributes, u32 previous, u32 material,
                                    u32 vertexCount = 4, u32 allocationSize = 0x50) {
    auto* i_element = gabi::at<WeatherGeometry>(element);
    u32 data = static_cast<u32>(i_element->mpVertices);
    if (!data) {
        u32 allocator = weatherGetAllocator(static_cast<u32>(weatherVertexAllocator), data);
        auto* table = static_cast<WeatherHeapVtable*>(gabi::at<WeatherHeap>(allocator)->mpVtable);
        u32 allocation = gabi::call_ptr<u32>(static_cast<u32>(table->mpAllocate), allocator,
                                             allocationSize, (u32)0x40);
        if (allocation) {
            (i_element->mpAllocation = allocation);
            (i_element->mVertexCount = vertexCount);
        }
        data = static_cast<u32>(i_element->mpAllocation);
        (i_element->mpVertices = data);
    }
    weatherInitializeVertices(element + 4, data, vertexCount, attributes);
    if (material && material != static_cast<u32>(*gabi::at<be<u32>>(previous)))
        weatherInitializeGeometryMaterial(material, gabi::ea(&i_element->mMaterialBinding[0]),
                                          element + 4, attributes, (u32)0);
}

static bool weather_stage_equals(u32 name) {
    gabi::Local<SafeString> expected, actual;
    expected.get()->text = name;
    expected.get()->vtable = gabi::ea(&weatherStringVtable);
    u32 game = weatherGetPlayRegister();
    actual.get()->text = game + 0x5134;
    actual.get()->vtable = gabi::ea(&weatherStringVtable);
    string_prepare(expected.a);
    string_prepare(expected.a);
    u32 first = static_cast<u32>(expected.get()->text);
    string_prepare(actual.a);
    u32 second = static_cast<u32>(actual.get()->text);
    if (first == second)
        return true;
    return same_string_bytes(static_cast<u32>(expected.get()->text), second);
}
/* 0257CE00 */
static void dKyw_wether_draw() {

    if (!weather_stage_equals(gabi::guest_addr("Name"))) {
        auto *environment = dKy_getWeatherEnvlight();
        if (static_cast<u8>(environment->mbPoisonInitialized))
            weatherCall_dKyw_drawPoison((u32)1);
        environment = dKy_getWeatherEnvlight();
        if (static_cast<u8>(environment->mWaveInitialized) == 1)
            weatherCall_dKyw_drawWave((u32)1);
        environment = dKy_getWeatherEnvlight();
        if (static_cast<u8>(environment->mMoyaInitialized) == 1)
            weatherCall_dKyw_drawCloud((u32)1);
    }
    if (!weather_stage_equals(gabi::guest_addr("Name"))) {
        auto *environment = dKy_getWeatherEnvlight();
        if (static_cast<u8>(environment->mbSunInitialized)) {
            weatherCall_dKyw_drawSun((u32)1);
            weatherCall_dKyw_drawSunlenz((u32)1);
        }
    }
    auto *environment = dKy_getWeatherEnvlight();
    if (static_cast<u8>(environment->mbStarInitialized) == 1)
        weatherCall_dKyw_drawStar((u32)1);
    if (weather_stage_equals(gabi::guest_addr("Name")))
        return;
    environment = dKy_getWeatherEnvlight();
    if (static_cast<u8>(environment->mbRainInitialized) == 1)
        weatherCall_dKyw_drawRain((u32)1);
    environment = dKy_getWeatherEnvlight();
    if (static_cast<u8>(environment->mbSnowInitialized))
        weatherCall_dKyw_drawSnow((u32)1);
    environment = dKy_getWeatherEnvlight();
    if (static_cast<u8>(environment->mbHousiInitialized) == 1)
        weatherCall_dKyw_drawHousi((u32)1);
}
/* 02580ED8 */
u32 dKankyo_star_Packet::construct() {
    u32 object = gabi::ea(this);

    if (!object) {
        object = weatherAllocateRegister((u32)0x49ADD8);
        if (!object)
            return 0;
    }
    auto* packetView = gabi::at<dKankyo_star_Packet>(object);
    weatherConstructPacket(object);
    packetView->mPacket.__vtbl = gabi::ea(&weatherStarVtable);
    weatherClear(gabi::ea(&packetView->mpTexture), (u32)4);
    weatherClear(gabi::ea(&packetView->mEffect), (u32)0x30);
    packetView->mEffectNum = 0;
    packetView->mFrame = 0;
    weatherClear(gabi::ea(&packetView->mActive), (u32)2000);
    packetView->mpMaterial = 0;
    weatherConstructArray(gabi::ea(&packetView->mGeometry), (u32)2000, (u32)0x968,
                          (u32)gabi::guest_fn((const void*)four_geometry_constructor));
    weatherConstructArray(gabi::ea(&packetView->mRenderer), (u32)8, (u32)0x41C,
                          (u32)gabi::guest_fn((const void*)dual_texture_constructor));
    weatherClear(gabi::ea(&packetView->mColorIndex), (u32)2000);
    return object;
}
/* 025812EC */
u32 dKankyo_poison_Packet::construct() {
    u32 object = gabi::ea(this);

    if (!object) {
        object = weatherAllocateRegister((u32)0x20BCA8);
        if (!object)
            return 0;
    }
    auto* packetView = gabi::at<dKankyo_poison_Packet>(object);
    weatherConstructPacket(object);
    packetView->mPacket.__vtbl = gabi::ea(&weatherPoisonVtable);
    weatherClear(gabi::ea(&packetView->mEff), (u32)0xBB80);
    packetView->mCount = 0;
    weatherClear(gabi::ea(&packetView->mpTexture), 4u);
    packetView->mAlpha = weatherZero.value;
    weatherClear(gabi::ea(&packetView->mActive), 1000u);
    packetView->mpMaterial = 0;
    weatherConstructArray(gabi::ea(&packetView->mGeometry), (u32)1000, (u32)0x4C0,
                          (u32)gabi::guest_fn((const void*)poisonGeometryPairConstruct));
    weatherConstructArray(gabi::ea(&packetView->mTextures), (u32)1000, (u32)0x370,
                          (u32)gabi::guest_fn((const void*)texture_with_constants_constructor));
    u32 renderer = gabi::ea(&packetView->mRenderer);
    weatherConstructRenderer(renderer);
    weatherConstructTexture(gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture));
    gabi::at<WeatherRendererPrefix>(renderer)->mTexture.__vtbl =
        gabi::ea(&weatherSmallTextureVtable);
    weatherClear(gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture.mParameters),
                 (u32)0x34);
    if (!(gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture.mParameters)))
        weatherAllocateRegister((u32)0x30);
    weatherConstructTextureSet(gabi::ea(&packetView->mTextureSet));
    return object;
}
/* 02581060 */
void dKankyo_star_Packet::setColor(u32 colorIndex) {
    auto* packetView = this;

    weatherBindRenderer(gabi::ea(&packetView->mRenderer[colorIndex]),
                        static_cast<u32>(packetView->mpMaterial), (u32)0);
    s32 count = (s16) static_cast<u16>(packetView->mEffectNum);
    for (u32 i = 0; (s32)i < count; ++i) {
        if ((u32)(s32)(s8) static_cast<u8>(packetView->mColorIndex[i]) == colorIndex &&
            static_cast<u8>(packetView->mActive[i])) {
            auto* geometry = &packetView->mGeometry[i];
            u32 side = static_cast<u32>(geometry->mBufferIndex) == 0 ? 1 : 0;
            u32 element = gabi::ea(&geometry->mGeometry[side]);
            for (u32 half = 0; half < 2; ++half) {
                if (static_cast<u32>(geometry->mpMaterial))
                    weatherBindGeometryMaterial(element + 0x158);
                weatherDrawIndexed(static_cast<u32>(weatherTrianglePrimitive), (u32)3,
                                   static_cast<u32>(weatherTriangleIndexType),
                                   static_cast<u32>(weatherTriangleIndices), (u32)0, (u32)1);
                element += 0x4A8;
            }
            count = (s16) static_cast<u16>(packetView->mEffectNum);
        }
    }
}

static u32 bind_weather_shader(u32 materialField) {
    auto* cache =
        gabi::at<WeatherShaderCache>(weatherGetShaderCache(gabi::ea(&weatherShaderContext)));
    auto* field = gabi::at<gptr<WeatherMaterial>>(materialField);
    auto* material = static_cast<WeatherMaterial*>(*field);
    auto* shader = static_cast<WeatherShader*>(material->mpShader);
    if (gabi::ea(shader) == gabi::ea(cache->mpShader))
        return gabi::ea(material);
    u8 flags = shader->mFlags;
    u32 oldProgram = cache->mpProgram;
    if (flags & 2) {
        shader->mFlags = flags & ~2u;
        weatherPrepareShader(gabi::ea(shader), 0u);
    }
    auto* handle = static_cast<WeatherShaderProgram*>(shader->mpProgram);
    u32 program = handle->mpProgram;
    if (oldProgram != program)
        weatherBindProgram(program);
    u32 size = shader->mBinarySize;
    if (size) {
        weatherBindShader(static_cast<u32>(shader->mpBinary), size);
        cache->mpProgram = program;
        cache->mpShader = shader;
    } else {
        weatherUploadShader(gabi::ea(shader));
        cache->mpShader = shader;
        cache->mpProgram = program;
    }
    return gabi::ea(*field);
}
/* 0258115C */
void dKankyo_star_Packet::render() {
    u32 packet = gabi::ea(this);
    auto* packetView = this;

    gabi::Local<WeatherRenderState> state;
    weatherConstructRenderState(state.a);
    (state.get()->mDepthBias = static_cast<float>(weatherZero.value));
    (state.get()->mDepthFunction = 4);
    (state.get()->mDepthCompare = 2);
    u32 bits = static_cast<u32>(state.get()->mBlendFactors);
    (state.get()->mDepthEnabled = 0);
    (state.get()->mBlendEnabled = 1);
    (state.get()->mAlphaMode = 3);
    (state.get()->mBlendFactors = (((bits & ~15u) + 7) & ~0xF0u) + 16);
    weatherApplyRenderState(state.a);
    gabi::Local<WeatherMatrix> matrix;
    for (u32 i = 0; i < 8; ++i) {
        weatherCopyMatrix(matrix.a, gabi::ea(&weatherViewMatrix));
        u32 renderer = gabi::ea(&packetView->mRenderer[i]);
        u32 camera = static_cast<u32>(weatherCamera);
        weatherSetMatrices(renderer, (u32)0, matrix.a, gabi::ea(&weatherProjectionMatrix),
                           gabi::ea(&gabi::at<WeatherCameraView>(camera)->mRenderParameters));
        weatherPrepareRenderer(renderer, (u32)0);
    }
    bind_weather_shader(gabi::ea(&packetView->mpMaterial));
    for (u32 i = 0; i < 8; ++i)
        weatherDrawStarColor(packet, i);
    weatherApplyRenderState(gabi::ea(&weatherDefaultRenderState));
}
/* 02581518 */
void dKankyo_poison_Packet::initializeGeometry() {
    u32 packet = gabi::ea(this);
    auto* packetView = this;

    packetView->mpMaterial = lookup_weather_material(packet, gabi::guest_addr("housi"));
    for (u32 i = 0; i < 1000; ++i) {
        u32 material = static_cast<u32>(packetView->mpMaterial);
        prepare_weather_pair(gabi::ea(&packetView->mGeometry[i]), material,
                             gabi::ea(&weatherPoisonAttributes));
    }
    gabi::Local<WeatherTextureDescriptor> texture;
    weatherConstructTextureDescriptor(texture.a);
    u32 resource = static_cast<u32>(packetView->mpTexture);
    weatherLockTextures(static_cast<u32>(weatherTextureManager));
    weatherImportTexture(texture.a, resource);
    weatherUnlockTextures(static_cast<u32>(weatherTextureManager));
    weatherSetTexture(gabi::ea(&packetView->mTextureSet), (u32)0, texture.a, (u32)0);
    u32 target = weatherGetTexture(gabi::ea(&packetView->mTextureSet), (u32)0);
    auto* binding = gabi::at<WeatherTextureBinding>(target);
    u8 flags = binding->mFlags;
    binding->mSamplerStateDirty[1] = 1;
    binding->mSamplerStateDirty[0] = 1;
    binding->mSamplerStateDirty[2] = 1;
    binding->mFlags = flags | 2;
    for (u32 i = 0; i < 1000; ++i)
        weatherSetTextureMode(gabi::ea(&packetView->mTextures[i]), (u32)0);
    weatherSetRendererMode(gabi::ea(&packetView->mRenderer), (u32)1, (u32)0);
    weatherSetTextureMode(gabi::ea(&packetView->mRenderer.mTexture), (u32)0);
}

static void bind_weather_texture(u32 texture, u32 material) {
    auto* table = gabi::at<WeatherTextureVtable>(gabi::at<WeatherTextureSmall>(texture)->__vtbl);
    gabi::call_ptr<void>(table->mpBind, texture, material);
}
static void bind_weather_uniforms(u32 renderer, u32 materialField) {
    auto* info = static_cast<WeatherUniformInfo*>(gabi::at<WeatherUniformOwner>(renderer)->mpInfo);
    u32 slot = info->mBufferIndex;
    auto* uniform = &info->mUniforms[slot];
    auto* material = gabi::at<WeatherMaterial>(*gabi::at<be<u32>>(materialField));
    auto* descriptor = gabi::at<WeatherUniformDescriptor>(
        material->mDescriptorCount ? gabi::ea(material->mpDescriptors) : 0);
    s32 vertex = descriptor->mVertexSlot;
    u32 size = uniform->mSize;
    s32 pixel = descriptor->mPixelSlot;
    u32 data = uniform->mpData;
    s32 geometry = descriptor->mGeometrySlot;
    if (pixel != -1)
        weatherSetPixelUniforms((u32)pixel, size, data);
    if (vertex != -1)
        weatherSetVertexUniforms((u32)vertex, size, data);
    if (geometry != -1)
        weatherSetGeometryUniforms((u32)geometry, size, data);
}
/* 025817B0 */
void dKankyo_poison_Packet::render() {
    auto* packetView = this;

    gabi::Local<WeatherRenderState> state;
    weatherConstructRenderState(state.a);
    u32 bits = static_cast<u32>(state.get()->mBlendFactors);
    (state.get()->mDepthCompare = 2);
    (state.get()->mDepthBias = static_cast<float>(weatherZero.value));
    (state.get()->mDepthFunction = 4);
    (state.get()->mDepthEnabled = 0);
    (state.get()->mAlphaMode = 3);
    (state.get()->mBlendEnabled = 1);
    (state.get()->mBlendFactors = (((bits & ~15u) + 7) & ~0xF0u) + 16);
    gabi::Local<WeatherMatrix> matrix;
    gabi::Local<WeatherProjection> projection;
    weatherCopyMatrix(matrix.a, gabi::ea(&weatherViewMatrix));
    for (u32 i = 0; i < 16; ++i)
        (projection.get()->words[i] = static_cast<u32>(weatherProjectionMatrix.words[i]));
    u32 renderer = gabi::ea(&packetView->mRenderer);
    weatherSetMatrices(renderer, (u32)0, matrix.a, projection.a,
                       gabi::ea(&gabi::at<WeatherCameraView>(weatherCamera)->mRenderParameters));
    weatherUpdateRendererUniforms(renderer);
    u32 texture = gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture);
    weatherUpdateTextureUniforms(texture);
    s32 count = (s32) static_cast<u32>(dKy_getWeatherEnvlight()->mPoisonCount);
    for (u32 i = 0; (s32)i < count; ++i) {
        if (static_cast<u8>(packetView->mActive[i]))
            weatherUpdateTextureUniforms(gabi::ea(&packetView->mTextures[i]));
        count = (s32) static_cast<u32>(dKy_getWeatherEnvlight()->mPoisonCount);
    }
    u32 material = bind_weather_shader(gabi::ea(&packetView->mpMaterial));
    weatherBindTextureSet(gabi::ea(&packetView->mTextureSet), material);
    bind_weather_texture(texture, static_cast<u32>(packetView->mpMaterial));
    bind_weather_uniforms(renderer, gabi::ea(&packetView->mpMaterial));
    weatherApplyRenderState(state.a);
    count = (s32) static_cast<u32>(dKy_getWeatherEnvlight()->mPoisonCount);
    for (u32 i = 0; (s32)i < count; ++i) {
        if (static_cast<u8>(packetView->mActive[i])) {
            bind_weather_texture(gabi::ea(&packetView->mTextures[i]),
                                 static_cast<u32>(packetView->mpMaterial));
            auto* pair = &packetView->mGeometry[i];
            u32 side = static_cast<u32>(pair->mBufferIndex) == 0 ? 1 : 0;
            if (static_cast<u32>(pair->mpMaterial))
                weatherBindGeometryMaterial(gabi::ea(&pair->mGeometry[side].mMaterialBinding));
            weatherDrawIndexed(static_cast<u32>(weatherQuadPrimitive), (u32)4,
                               static_cast<u32>(weatherQuadIndexType),
                               static_cast<u32>(weatherQuadIndices), (u32)0, (u32)1);
        }
        count = (s32) static_cast<u32>(dKy_getWeatherEnvlight()->mPoisonCount);
    }
    weatherApplyRenderState(gabi::ea(&weatherDefaultRenderState));
}
/* 025820F4 */
void dKankyo_cloud_Packet::render(s32 count) {
    auto* packetView = this;

    gabi::Local<WeatherRenderState> state;
    weatherConstructRenderState(state.a);
    u32 bits = static_cast<u32>(state.get()->mBlendFactors);
    (state.get()->mBlendEnabled = 1);
    (state.get()->mDepthCompare = 1);
    (state.get()->mDepthFunction = 4);
    (state.get()->mBlendMode = 3);
    (state.get()->mAlphaMode = 3);
    (state.get()->mDepthBias = static_cast<float>(weatherZero.value));
    (state.get()->mBlendFactors = (((bits & ~15u) + 7) & ~0xF0u) + 16);
    (state.get()->mDepthEnabled = 0);
    (state.get()->mCullEnabled = 0);
    weatherApplyRenderState(state.a);
    gabi::Local<WeatherMatrix> matrix;
    gabi::Local<WeatherProjection> projection;
    weatherCopyMatrix(matrix.a, gabi::ea(&weatherViewMatrix));
    for (u32 i = 0; i < 16; ++i)
        (projection.get()->words[i] = static_cast<u32>(weatherProjectionMatrix.words[i]));
    u32 renderer = gabi::ea(&packetView->mRenderer),
        texture = gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture);
    weatherSetMatrices(renderer, (u32)0, matrix.a, projection.a,
                       gabi::ea(&gabi::at<WeatherCameraView>(weatherCamera)->mRenderParameters));
    weatherUpdateRendererUniforms(renderer);
    weatherUpdateTextureUniforms(texture);
    u32 material = bind_weather_shader(gabi::ea(&packetView->mpMaterial));
    weatherBindTextureSet(gabi::ea(&packetView->mTextureSet), material);
    bind_weather_texture(texture, static_cast<u32>(packetView->mpMaterial));
    bind_weather_uniforms(renderer, gabi::ea(&packetView->mpMaterial));
    if (count > 0)
        for (u32 i = 0; i < (u32)count; ++i) {
            u32 particle = gabi::ea(&packetView->mTextures[i]);
            weatherUpdateTextureUniforms(particle);
            bind_weather_texture(particle, static_cast<u32>(packetView->mpMaterial));
            auto* pair = &packetView->mGeometry[i];
            u32 side = static_cast<u32>(pair->mBufferIndex) == 0 ? 1 : 0;
            if (static_cast<u32>(pair->mpMaterial))
                weatherBindGeometryMaterial(gabi::ea(&pair->mGeometry[side].mMaterialBinding));
            weatherDrawIndexed(static_cast<u32>(weatherQuadPrimitive), (u32)4,
                               static_cast<u32>(weatherQuadIndexType),
                               static_cast<u32>(weatherQuadIndices), (u32)0, (u32)1);
        }
    weatherApplyRenderState(gabi::ea(&weatherDefaultRenderState));
}
/* 02581C34 */
void dKankyo_housi_Packet::render() {
    auto* packetView = this;

    gabi::Local<WeatherRenderState> state;
    weatherConstructRenderState(state.a);
    u32 bits = static_cast<u32>(state.get()->mBlendFactors);
    (state.get()->mDepthCompare = 2);
    (state.get()->mDepthBias = static_cast<float>(weatherZero.value));
    (state.get()->mDepthFunction = 4);
    (state.get()->mDepthEnabled = 0);
    (state.get()->mAlphaMode = 3);
    (state.get()->mBlendEnabled = 1);
    (state.get()->mBlendFactors = (((bits & ~15u) + 7) & ~0xF0u) + 16);
    gabi::Local<WeatherMatrix> matrix;
    gabi::Local<WeatherProjection> projection;
    weatherCopyMatrix(matrix.a, gabi::ea(&weatherViewMatrix));
    for (u32 i = 0; i < 16; ++i)
        (projection.get()->words[i] = static_cast<u32>(weatherProjectionMatrix.words[i]));
    u32 renderer = gabi::ea(&packetView->mRenderer);
    weatherSetMatrices(renderer, (u32)0, matrix.a, projection.a,
                       gabi::ea(&gabi::at<WeatherCameraView>(weatherCamera)->mRenderParameters));
    weatherUpdateRendererUniforms(renderer);
    u32 texture = gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture);
    weatherUpdateTextureUniforms(texture);
    s32 count;
    for (u32 group = 0; group < 2; ++group) {
        count = (s32) static_cast<u32>(dKy_getWeatherEnvlight()->mHousiCount);
        for (u32 i = 0; (s32)i < count; ++i) {
            u32 index = group * 300 + i;
            if (static_cast<u8>(packetView->mActive[index]))
                weatherUpdateTextureUniforms(gabi::ea(&packetView->mTextures[index]));
            count = (s32) static_cast<u32>(dKy_getWeatherEnvlight()->mHousiCount);
        }
    }
    u32 material = bind_weather_shader(gabi::ea(&packetView->mpMaterial));
    weatherBindTextureSet(gabi::ea(&packetView->mTextureSet), material);
    bind_weather_texture(texture, static_cast<u32>(packetView->mpMaterial));
    bind_weather_uniforms(renderer, gabi::ea(&packetView->mpMaterial));
    for (u32 group = 0; group < 2; ++group) {
        (state.get()->mBlendMode = group ? 6 : 3);
        weatherApplyRenderState(state.a);
        count = (s32) static_cast<u32>(dKy_getWeatherEnvlight()->mHousiCount);
        for (u32 i = 0; (s32)i < count; ++i) {
            u32 index = group * 300 + i;
            if (static_cast<u8>(packetView->mActive[index])) {
                bind_weather_texture(gabi::ea(&packetView->mTextures[index]),
                                     static_cast<u32>(packetView->mpMaterial));
                auto* pair = &packetView->mGeometry[index];
                u32 side = static_cast<u32>(pair->mBufferIndex) == 0 ? 1 : 0;
                if (static_cast<u32>(pair->mpMaterial))
                    weatherBindGeometryMaterial(gabi::ea(&pair->mGeometry[side].mMaterialBinding));
                weatherDrawIndexed(static_cast<u32>(weatherQuadPrimitive), (u32)4,
                                   static_cast<u32>(weatherQuadIndexType),
                                   static_cast<u32>(weatherQuadIndices), (u32)0, (u32)1);
            }
            count = (s32) static_cast<u32>(dKy_getWeatherEnvlight()->mHousiCount);
        }
    }
    weatherApplyRenderState(gabi::ea(&weatherDefaultRenderState));
}
static void emit_weather_uniforms(u32 infoAddress, u32 descriptorAddress) {
    auto* info = gabi::at<WeatherUniformInfo>(infoAddress);
    auto* uniform = &info->mUniforms[static_cast<u32>(info->mBufferIndex)];
    auto* descriptor = gabi::at<WeatherUniformDescriptor>(descriptorAddress);
    s32 vertex = descriptor->mVertexSlot;
    u32 data = uniform->mpData;
    s32 pixel = descriptor->mPixelSlot;
    u32 size = uniform->mSize;
    s32 geometry = descriptor->mGeometrySlot;
    if (pixel != -1)
        weatherSetPixelUniforms((u32)pixel, size, data);
    if (vertex != -1)
        weatherSetVertexUniforms((u32)vertex, size, data);
    if (geometry != -1)
        weatherSetGeometryUniforms((u32)geometry, size, data);
}
/* 02582650 */
void dKankyo_wave_Packet::render(u32 context) {
    auto* packetView = this;

    gabi::Local<WeatherMatrix> matrix;
    gabi::Local<WeatherProjection> projection;
    weatherCopyMatrix(matrix.a, gabi::ea(&weatherViewMatrix));
    for (u32 i = 0; i < 16; ++i)
        (projection.get()->words[i] = static_cast<u32>(weatherProjectionMatrix.words[i]));
    u32 renderer = gabi::ea(&packetView->mRenderer),
        texture = gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture);
    weatherSetMatrices(renderer, (u32)0, matrix.a, projection.a,
                       gabi::ea(&gabi::at<WeatherCameraView>(weatherCamera)->mRenderParameters));
    weatherUpdateRendererUniforms(renderer);
    weatherUpdateTextureUniforms(texture);
    u32 material = bind_weather_shader(gabi::ea(&packetView->mpMaterial));
    weatherBindTextureSet(gabi::ea(&packetView->mTextureSet), material);
    bind_weather_texture(texture, static_cast<u32>(packetView->mpMaterial));
    bind_weather_uniforms(renderer, gabi::ea(&packetView->mpMaterial));
    gabi::Local<WeatherRenderState> state;
    weatherConstructRenderState(state.a);
    u32 bits = static_cast<u32>(state.get()->mBlendFactors);
    (state.get()->mDepthFunction = 4);
    (state.get()->mBlendEnabled = 1);
    (state.get()->mDepthCompare = 2);
    (state.get()->mAlphaMode = 3);
    (state.get()->mDepthBias = static_cast<float>(weatherWindDamping.value));
    (state.get()->mBlendFactors = (((bits & ~15u) + 7) & ~0xF0u) + 16);
    auto* drawContext = gabi::at<WeatherDrawContext>(context);
    if (static_cast<u32>(drawContext->mType) == 2) {
        u32 info = drawContext->mpInfo;
        if (info) {
            u32 material = static_cast<u32>(packetView->mpMaterial);
            auto* materialView = gabi::at<WeatherMaterial>(material);
            u32 descriptor = static_cast<u32>(materialView->mDescriptorCount) > 4
                                 ? gabi::ea(&materialView->mpDescriptors[4])
                                 : 0;
            emit_weather_uniforms(info, descriptor);
        }
        weatherApplyContextMaterial(context, static_cast<u32>(packetView->mpMaterial));
    }
    weatherApplyRenderState(state.a);
    s32 count = (s16) static_cast<u16>(dKy_getWeatherEnvlight()->mWaveChan.mWaveCount);
    for (u32 i = 0; (s32)i < count; ++i) {
        if (static_cast<u8>(packetView->mActive[i])) {
            u32 particle = gabi::ea(&packetView->mTextures[i]);
            weatherUpdateTextureUniforms(particle);
            bind_weather_texture(particle, static_cast<u32>(packetView->mpMaterial));
            auto* pair = &packetView->mGeometry[i];
            u32 side = static_cast<u32>(pair->mBufferIndex) == 0 ? 1 : 0;
            if (static_cast<u32>(pair->mpMaterial))
                weatherBindGeometryMaterial(gabi::ea(&pair->mGeometry[side].mMaterialBinding));
            weatherDrawIndexed(static_cast<u32>(weatherQuadPrimitive), (u32)4,
                               static_cast<u32>(weatherQuadIndexType),
                               static_cast<u32>(weatherQuadIndices), (u32)0, (u32)1);
        }
        count = (s16) static_cast<u16>(dKy_getWeatherEnvlight()->mWaveChan.mWaveCount);
    }
    weatherSetFog();
}
/* 02583678 */
static void wether_move_moya() {
    u8 phase = dKy_getWeatherEnvlight()->mMoyaInitialized;
    switch (phase) {
    case WEATHER_INITIALIZE: {
        if (!dKy_getWeatherEnvlight()->mMoyaCount)
            return;
        gabi::Local<be<u32>> heapScope;
        // HD: the original heap label includes the Japanese description for haze.
        weatherBeginHeap(heapScope.a, gabi::guest_addr("dKankyo_cloud_Packet"), 0x27510u, 0u);
        u32 packet = weatherAllocateAligned(sizeof(dKankyo_cloud_Packet), 32u);
        if (packet)
            packet = weatherConstructCloudPacket(packet);
        dKy_getWeatherEnvlight()->mpMoyaPacket = gabi::at<void>(packet);
        packet = gabi::ea(dKy_getWeatherEnvlight()->mpMoyaPacket);
        if (packet) {
            gabi::Local<SafeString> package, entry;
            package.get()->text = gabi::guest_addr("Always");
            package.get()->vtable = gabi::ea(&weatherStringVtable);
            entry.get()->text = gabi::guest_addr("Always.bfres");
            entry.get()->vtable = gabi::ea(&weatherStringVtable);
            u32 resource = weatherGetArchiveResource(static_cast<u32>(weatherArchiveManager),
                                                     package.a, entry.a, 0u);
            u32 archive = weatherGetArchive(resource);
            u32 texture = weatherGetArchiveTexture(archive_resource_array(archive),
                                                   gabi::guest_addr("txa_moya_b_64"));
            packet = gabi::ea(dKy_getWeatherEnvlight()->mpMoyaPacket);
            gabi::at<dKankyo_cloud_Packet>(packet)->mpTexture = texture;
            weatherInitializeCloudGeometry(gabi::ea(dKy_getWeatherEnvlight()->mpMoyaPacket));
            for (u32 i = 0; i < 30; ++i)
                gabi::at<dKankyo_cloud_Packet>(gabi::ea(dKy_getWeatherEnvlight()->mpMoyaPacket))
                    ->mEff[i]
                    .mStatus = 0;
            gabi::at<dKankyo_cloud_Packet>(gabi::ea(dKy_getWeatherEnvlight()->mpMoyaPacket))
                ->mCount = 0;
            weatherMoveCloudShadow();
            auto* env = dKy_getWeatherEnvlight();
            env->mMoyaInitialized = static_cast<u8>(env->mMoyaInitialized) + 1;
        } else {
            weatherAbortHeap(heapScope.a);
        }
        weatherEndHeap(heapScope.a, 2u);
        break;
    }
    case WEATHER_MOVE: {
        bool special = false;
        if (dKy_getWeatherEnvlight()->mMoyaMode == 0) {
            float power = weatherdKyw_get_wind_pow();
            if (power > static_cast<float>(weatherSquallDamping.value) &&
                dKy_getWeatherEnvlight()->mMoyaCount) {
                if (weather_stage_equals(gabi::guest_addr("M_NewD2"))) {
                    if (dKy_getWeatherEnvlight()->mbVrboxInvisible)
                        special = true;
                    else
                        dKy_getWeatherEnvlight()->mMoyaSE = JA_SE_ATM_WIND_1;
                }
                if (special)
                    dKy_getWeatherEnvlight()->mMoyaSE = JA_SE_ATM_WIND_HOT;
                weatherMoveCloudShadow();
                if (!dKy_getWeatherEnvlight()->mMoyaCount &&
                    !gabi::at<dKankyo_cloud_Packet>(
                         gabi::ea(dKy_getWeatherEnvlight()->mpMoyaPacket))
                         ->mCount) {
                    auto* env = dKy_getWeatherEnvlight();
                    env->mMoyaInitialized = static_cast<u8>(env->mMoyaInitialized) + 1;
                }
                return;
            }
        }
        u8 mode = dKy_getWeatherEnvlight()->mMoyaMode;
        if (mode == 1)
            dKy_getWeatherEnvlight()->mMoyaSE = JA_SE_ATM_WIND_HOT;
        else if (dKy_getWeatherEnvlight()->mMoyaMode == 2 &&
                 weather_stage_equals(gabi::guest_addr("A_mori")))
            dKy_getWeatherEnvlight()->mMoyaSE = JA_SE_ATM_WIND_GRASS;
        weatherMoveCloudShadow();
        if (!dKy_getWeatherEnvlight()->mMoyaCount &&
            !gabi::at<dKankyo_cloud_Packet>(gabi::ea(dKy_getWeatherEnvlight()->mpMoyaPacket))
                 ->mCount) {
            auto* env = dKy_getWeatherEnvlight();
            env->mMoyaInitialized = static_cast<u8>(env->mMoyaInitialized) + 1;
        }
        break;
    }
    case WEATHER_DRAIN: {
        auto* env = dKy_getWeatherEnvlight();
        env->mMoyaInitialized = static_cast<u8>(env->mMoyaInitialized) + 1;
        break;
    }
    case WEATHER_DELETE:
        dKy_getWeatherEnvlight()->mMoyaInitialized = WEATHER_INITIALIZE;
        weatherDeleteCloudPacket(gabi::ea(&dKy_getWeatherEnvlight()->mpMoyaPacket));
        break;
    }
}
/* 0257F804 */
void dKankyo_rain_Packet::renderSplashes() {
    auto* packetView = this;

    gabi::Local<WeatherRenderState> state;
    weatherConstructRenderState(state.a);
    u32 bits = static_cast<u32>(state.get()->mBlendFactors);
    (state.get()->mBlendEnabled = 1);
    (state.get()->mDepthCompare = 2);
    (state.get()->mDepthFunction = 4);
    (state.get()->mAlphaMode = 3);
    (state.get()->mDepthBias = static_cast<float>(weatherZero.value));
    (state.get()->mBlendFactors = (((bits & ~15u) + 7) & ~0xF0u) + 16);
    (state.get()->mDepthEnabled = 0);
    weatherApplyRenderState(state.a);
    gabi::Local<WeatherMatrix> matrix;
    gabi::Local<WeatherProjection> projection;
    weatherCopyMatrix(matrix.a, gabi::ea(&weatherViewMatrix));
    for (u32 i = 0; i < 16; ++i)
        (projection.get()->words[i] = static_cast<u32>(weatherProjectionMatrix.words[i]));
    u32 renderer = gabi::ea(&packetView->mRenderer),
        texture = gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture);
    weatherSetMatrices(renderer, (u32)0, matrix.a, projection.a,
                       gabi::ea(&gabi::at<WeatherCameraView>(weatherCamera)->mRenderParameters));
    weatherUpdateRendererUniforms(renderer);
    weatherUpdateTextureUniforms(texture);
    u32 material = bind_weather_shader(gabi::ea(&packetView->mpMaterial));
    weatherBindTextureSet(gabi::ea(&packetView->mTextureSet), material);
    bind_weather_texture(texture, static_cast<u32>(packetView->mpMaterial));
    bind_weather_uniforms(renderer, gabi::ea(&packetView->mpMaterial));
    s32 count = (s16) static_cast<u16>(packetView->mRainCount);
    for (u32 i = 0; (s32)i < count; ++i) {
        if (static_cast<u8>(packetView->mSplashActive[i])) {
            u32 particle = gabi::ea(&packetView->mSplashTextures[i]);
            weatherUpdateTextureUniforms(particle);
            bind_weather_texture(particle, static_cast<u32>(packetView->mpMaterial));
            for (u32 quad = 0; quad < 4; ++quad) {
                auto* pair = &packetView->mSplashGeometry[i * 4 + quad];
                u32 side = static_cast<u32>(pair->mBufferIndex) == 0 ? 1 : 0;
                if (static_cast<u32>(pair->mpMaterial))
                    weatherBindGeometryMaterial(gabi::ea(&pair->mGeometry[side].mMaterialBinding));
                weatherDrawIndexed(static_cast<u32>(weatherQuadPrimitive), (u32)4,
                                   static_cast<u32>(weatherQuadIndexType),
                                   static_cast<u32>(weatherQuadIndices), (u32)0, (u32)1);
            }
            count = (s16) static_cast<u16>(packetView->mRainCount);
        }
    }
    weatherApplyRenderState(gabi::ea(&weatherDefaultRenderState));
}
/* 0257FB4C */
void dKankyo_rain_Packet::renderStreaks() {
    auto* packetView = this;

    gabi::Local<WeatherRenderState> state;
    weatherConstructRenderState(state.a);
    (state.get()->mDepthEnabled = 0);
    float zero = static_cast<float>(weatherZero.value);
    u32 bits = static_cast<u32>(state.get()->mBlendFactors);
    (state.get()->mDepthCompare = 2);
    (state.get()->mBlendMode = 6);
    (state.get()->mDepthFunction = 4);
    (state.get()->mAlphaMode = 3);
    (state.get()->mBlendFactors = (((bits & ~15u) + 7) & ~0xF0u) + 16);
    (state.get()->mBlendEnabled = 1);
    (state.get()->mDepthBias = zero);
    weatherApplyRenderState(state.a);
    gabi::Local<WeatherMatrix> matrix;
    gabi::Local<WeatherProjection> projection;
    weatherCopyMatrix(matrix.a, gabi::ea(&weatherViewMatrix));
    for (u32 i = 0; i < 16; ++i)
        (projection.get()->words[i] = static_cast<u32>(weatherProjectionMatrix.words[i]));
    u32 renderer = gabi::ea(&packetView->mRenderer),
        texture = gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture),
        extraTexture = gabi::ea(&packetView->mRainTexture);
    weatherSetMatrices(renderer, (u32)0, matrix.a, projection.a,
                       gabi::ea(&gabi::at<WeatherCameraView>(weatherCamera)->mRenderParameters));
    weatherUpdateRendererUniforms(renderer);
    weatherUpdateTextureUniforms(texture);
    weatherUpdateTextureUniforms(extraTexture);
    u32 material = bind_weather_shader(gabi::ea(&packetView->mpMaterial));
    weatherBindTextureSet(gabi::ea(&packetView->mTextureSet[1]), material);
    bind_weather_texture(texture, static_cast<u32>(packetView->mpMaterial));
    bind_weather_uniforms(renderer, gabi::ea(&packetView->mpMaterial));
    bind_weather_texture(extraTexture, static_cast<u32>(packetView->mpMaterial));
    s32 count = static_cast<s32>(dKy_getWeatherEnvlight()->mRainCount) >> 1;
    for (u32 i = 0; (s32)i < count; ++i) {
        auto* pair = &packetView->mRainGeometry[i];
        u32 side = static_cast<u32>(pair->mBufferIndex) == 0 ? 1 : 0;
        if (static_cast<u32>(pair->mpMaterial))
            weatherBindGeometryMaterial(gabi::ea(&pair->mGeometry[side].mMaterialBinding));
        weatherDrawIndexed(static_cast<u32>(weatherQuadPrimitive), (u32)4,
                           static_cast<u32>(weatherQuadIndexType),
                           static_cast<u32>(weatherQuadIndices), (u32)0, (u32)1);
        count = static_cast<s32>(dKy_getWeatherEnvlight()->mRainCount) >> 1;
    }
    weatherApplyRenderState(gabi::ea(&weatherDefaultRenderState));
}
/* 02580870 */
void dKankyo_snow_Packet::renderSplashes() {
    auto* packetView = this;

    gabi::Local<WeatherRenderState> state;
    weatherConstructRenderState(state.a);
    u32 bits = static_cast<u32>(state.get()->mBlendFactors);
    float zero = static_cast<float>(weatherZero.value);
    (state.get()->mDepthCompare = 2);
    (state.get()->mDepthBias = zero);
    (state.get()->mDepthFunction = 4);
    (state.get()->mDepthEnabled = 0);
    (state.get()->mBlendEnabled = 1);
    (state.get()->mBlendFactors = (((bits & ~15u) + 7) & ~0xF0u) + 16);
    (state.get()->mAlphaMode = 3);
    gabi::Local<WeatherMatrix> matrix;
    gabi::Local<WeatherProjection> projection;
    weatherCopyMatrix(matrix.a, gabi::ea(&weatherViewMatrix));
    for (u32 i = 0; i < 16; ++i)
        (projection.get()->words[i] = static_cast<u32>(weatherProjectionMatrix.words[i]));
    u32 renderer = gabi::ea(&packetView->mRenderer),
        texture = gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture),
        extra = gabi::ea(&packetView->mSnowTexture);
    weatherSetMatrices(renderer, (u32)0, matrix.a, projection.a,
                       gabi::ea(&gabi::at<WeatherCameraView>(weatherCamera)->mRenderParameters));
    weatherUpdateRendererUniforms(renderer);
    weatherUpdateTextureUniforms(texture);
    weatherUpdateTextureUniforms(extra);
    u32 material = bind_weather_shader(gabi::ea(&packetView->mpMaterial));
    weatherBindTextureSet(gabi::ea(&packetView->mTextureSet), material);
    bind_weather_texture(texture, static_cast<u32>(packetView->mpMaterial));
    bind_weather_uniforms(renderer, gabi::ea(&packetView->mpMaterial));
    bind_weather_texture(extra, static_cast<u32>(packetView->mpMaterial));
    weatherApplyRenderState(state.a);
    s32 count = static_cast<s16>(packetView->mEffCount);
    for (u32 i = 0; (s32)i < count; ++i) {
        for (u32 q = 0; q < 4; ++q) {
            auto* pair = &packetView->mSplashGeometry[i * 4 + q];
            u32 side = static_cast<u32>(pair->mBufferIndex) == 0 ? 1 : 0;
            if (static_cast<u32>(pair->mpMaterial))
                weatherBindGeometryMaterial(gabi::ea(&pair->mGeometry[side].mMaterialBinding));
            weatherDrawIndexed(static_cast<u32>(weatherQuadPrimitive), (u32)4,
                               static_cast<u32>(weatherQuadIndexType),
                               static_cast<u32>(weatherQuadIndices), (u32)0, (u32)1);
        }
        count = static_cast<s16>(packetView->mEffCount);
    }
    weatherApplyRenderState(gabi::ea(&weatherDefaultRenderState));
}
/* 02580B98 */
void dKankyo_snow_Packet::renderParticles(u32 count, u32 blendStart) {
    auto* packetView = this;

    gabi::Local<WeatherRenderState> state;
    weatherConstructRenderState(state.a);
    u32 bits = static_cast<u32>(state.get()->mBlendFactors);
    (state.get()->mDepthCompare = 2);
    (state.get()->mDepthEnabled = 0);
    (state.get()->mDepthFunction = 4);
    (state.get()->mAlphaMode = 3);
    float zero = static_cast<float>(weatherZero.value);
    (state.get()->mBlendEnabled = 1);
    (state.get()->mDepthBias = zero);
    (state.get()->mBlendFactors = (((bits & ~15u) + 7) & ~0xF0u) + 16);
    (state.get()->mBlendMode = 3);
    weatherApplyRenderState(state.a);
    gabi::Local<WeatherMatrix> matrix;
    gabi::Local<WeatherProjection> projection;
    weatherCopyMatrix(matrix.a, gabi::ea(&weatherViewMatrix));
    for (u32 i = 0; i < 16; ++i)
        (projection.get()->words[i] = static_cast<u32>(weatherProjectionMatrix.words[i]));
    u32 renderer = gabi::ea(&packetView->mRenderer),
        texture = gabi::ea(&gabi::at<WeatherRendererPrefix>(renderer)->mTexture);
    weatherSetMatrices(renderer, (u32)0, matrix.a, projection.a,
                       gabi::ea(&gabi::at<WeatherCameraView>(weatherCamera)->mRenderParameters));
    weatherUpdateRendererUniforms(renderer);
    weatherUpdateTextureUniforms(texture);
    u32 material = bind_weather_shader(gabi::ea(&packetView->mpMaterial));
    weatherBindTextureSet(gabi::ea(&packetView->mTextureSet), material);
    bind_weather_texture(texture, static_cast<u32>(packetView->mpMaterial));
    bind_weather_uniforms(renderer, gabi::ea(&packetView->mpMaterial));
    s32 limit = (s32)count;
    if (limit > 4000)
        limit = 4000;
    for (u32 i = 0; (s32)i < limit; ++i) {
        if (i == blendStart) {
            (state.get()->mBlendMode = 6);
            weatherApplyBlendState(state.a);
        }
        u32 particle = gabi::ea(&packetView->mTextures[i]);
        weatherUpdateTextureUniforms(particle);
        u32 table = packetView->mTextures[i].__vtbl;
        u32 target = gabi::at<WeatherTextureVtable>(table)->mpBind;
        // HD: Native uses r5/r7 for the vtable and target; resolved callees can read
        // these registers.
        gabi::cpu->r[5] = table;
        gabi::cpu->r[7] = target;
        gabi::call_ptr<void>(target, particle, static_cast<u32>(packetView->mpMaterial));
        auto* pair = &packetView->mSnowGeometry[i];
        u32 side = static_cast<u32>(pair->mBufferIndex) == 0 ? 1 : 0;
        if (static_cast<u32>(pair->mpMaterial))
            weatherBindGeometryMaterial(gabi::ea(&pair->mGeometry[side].mMaterialBinding));
        weatherDrawIndexed(static_cast<u32>(weatherQuadPrimitive), (u32)4,
                           static_cast<u32>(weatherQuadIndexType),
                           static_cast<u32>(weatherQuadIndices), (u32)0, (u32)1);
    }
    weatherApplyRenderState(gabi::ea(&weatherDefaultRenderState));
}
/* 0257D398 */
static void dKyw_wind_set() {

    auto *env = dKy_getWeatherEnvlight();
    float zero = static_cast<float>(weatherZero.value), x, y, z, power;
    gabi::Local<Vector> direction;
    if (static_cast<u32>(gabi::ea(env->mWind.mpWindVecOverride))) {
        u32 custom = static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mWind.mpWindVecOverride));
        for (u32 i = 0; i < 3; ++i)
            gabi::at<WeatherVector>(direction.a)->words[i] = gabi::at<WeatherVector>(custom)->words[i];
        env = dKy_getWeatherEnvlight();
        x = static_cast<float>(gabi::at<WeatherVector>(direction.a)->x);
        z = static_cast<float>(gabi::at<WeatherVector>(direction.a)->z);
        power = static_cast<float>(env->mWind.mWindStrengthOverride);
        weatherAtan2Register(x, z);
        gabi::Local<Vector> horizontal;
        (gabi::at<WeatherVector>(horizontal.a)->x = x);
        (gabi::at<WeatherVector>(horizontal.a)->y = zero);
        (gabi::at<WeatherVector>(horizontal.a)->z = z);
        float squared = weatherVectorSquaredLength(horizontal.a);
        float length = weatherSqrt(squared);
        y = static_cast<float>(gabi::at<WeatherVector>(direction.a)->y);
        weatherAtan2Register(length, y);
    } else {
        s32 room = static_cast<s8>(weatherStayRoomNo);
        u32 roomWind = 0;
        if (room >= 0) {
            u32 game = weatherGetPlayRegister();
            u32 info =
                weatherdStage_roomControl_c__getStatusRoomDtRegister78(game + 0x51CC, (u32)room);
            roomWind = gabi::call_ptr<u32>(static_cast<u32>(gabi::at<WeatherRoomVtable>(gabi::at<WeatherRoomStatus>(info)->mpVtable)->mpGetFileList), info);
        }
        u32 first = 0, second = 0;
        if (static_cast<u8>(dKy_getWeatherEnvlight()->mWind.mEvtWindSet) &&
            static_cast<u8>(dKy_getWeatherEnvlight()->mWind.mEvtWindSet) != 255) {
            first = static_cast<u16>(dKy_getWeatherEnvlight()->mWind.mEvtWindAngleX);
            second = static_cast<u16>(dKy_getWeatherEnvlight()->mWind.mEvtWindAngleY);
        } else if (static_cast<u8>(dKy_getWeatherEnvlight()->mWind.mTactWindAngleFlags)) {
            first = static_cast<u16>(dKy_getWeatherEnvlight()->mWind.mTactWindAngleX);
            second = static_cast<u16>(dKy_getWeatherEnvlight()->mWind.mTactWindAngleY);
        }
        auto* firstAngle = &weatherSinCosTable.mEntries[(first & ~7u) / 8];
        auto* secondAngle = &weatherSinCosTable.mEntries[(second & ~7u) / 8];
        float cosine = firstAngle->cosine.value, otherCosine = secondAngle->cosine.value;
        y = firstAngle->sine.value;
        u32 yBits = firstAngle->sine.bits;
        x = gabi::fmuls_ppc(cosine, otherCosine);
        z = gabi::fmuls_ppc(cosine, static_cast<f32>(secondAngle->sine.value));
        u32 mode = roomWind ? (static_cast<u32>(gabi::at<WeatherRoomFileList>(roomWind)->mFlags) >> 18) & 3 : 0;
        power = mode == 0   ? static_cast<float>(weatherWindLevelLow.value)
                : mode == 1 ? static_cast<float>(weatherWindLevelMedium.value)
                : mode == 2 ? static_cast<float>(weatherWindLevelHigh.value)
                            : zero;
        if (weather_stage_equals(gabi::guest_addr("Hyrule"))) {
            y = zero;
            yBits = static_cast<u32>(weatherZero.bits);
            x = static_cast<float>(weatherNegativeOne.value);
            power = static_cast<float>(weatherWindLevelLow.value);
            z = x;
        }
        // y reaches the stack vector by lfs/stfs only (no arithmetic): store its bits, as the
        // native register copy does.
        (gabi::at<WeatherVector>(direction.a)->x = x);
        (gabi::at<WeatherVector>(direction.a)->words[1] = yBits);
        (gabi::at<WeatherVector>(direction.a)->z = z);
    }
    if (static_cast<float>(dKy_getWeatherEnvlight()->mWind.mCustomWindPower) > zero) {
        power = static_cast<float>(dKy_getWeatherEnvlight()->mWind.mCustomWindPower);
        dKy_getWeatherEnvlight()->mWind.mWindPower = power;
    }
    if (static_cast<u8>(dKy_getWeatherEnvlight()->mWind.mEvtWindSet) == 2)
        power = zero;
    if (weather_stage_equals(gabi::guest_addr("ADMumi")))
        power = static_cast<float>(weatherWindLevelMedium.value);
    else if (power > static_cast<float>(weatherOne.value))
        power = static_cast<float>(weatherOne.value);
    if (static_cast<u8>(dKy_getWeatherEnvlight()->mInitAnimTimer)) {
        env = dKy_getWeatherEnvlight();
        for (u32 i = 0; i < 3; ++i)
            reinterpret_cast<WeatherVector*>(&env->mWind.mWindVec)->words[i] = gabi::at<WeatherVector>(direction.a)->words[i];
        dKy_getWeatherEnvlight()->mWind.mWindPower = power;
    } else {
        float damping = static_cast<float>(weatherWindDamping.value),
              minimum = static_cast<float>(weatherWindMinimumStep.value),
              step = static_cast<float>(weatherWindDirectionStep.value);
        weatherAddCalc(environment() + 0x9FC, x, damping, step, minimum);
        weatherAddCalc(environment() + 0xA00, y, damping, step, minimum);
        weatherAddCalc(environment() + 0xA04, z, damping, step, minimum);
        env = dKy_getWeatherEnvlight();
        float maxStep = static_cast<float>(weatherOne.value),
              cutoff = static_cast<float>(weatherWindPowerMinimumStep.value);
        weatherAddCalc(gabi::ea(env) + 0xA18, power, damping, maxStep, cutoff);
    }
}
/* 0257C5B0 */
static void wether_move_vrkumo() {

    u32 game = weatherGetPlayRegister(),
        camera = static_cast<u32>(gabi::at<WeatherPlay>(game)->mpCamera);
    if (!static_cast<u32>(weatherSeaCloudCenterInitialized)) {
        float x = static_cast<float>(weatherSeaCloudCenterX.value);
        (weatherSeaCloudCenterInitialized = 1);
        (weatherSeaCloudX = x);
        float y = static_cast<float>(weatherSeaCloudCenterY.value),
              z = static_cast<float>(weatherSeaCloudCenterZ.value);
        (weatherSeaCloudY = y);
        (weatherSeaCloudZ = z);
    }
    if (weather_stage_equals(gabi::guest_addr("Name")))
        dKy_getWeatherEnvlight()->mVrkumoCount = 70;
    else {
        game = weatherGetPlayRegister();
        if ((static_cast<u16>(gabi::at<WeatherPlay>(game)->mStatus) & 1) &&
            !static_cast<u8>(dKy_getWeatherEnvlight()->mbVrboxInvisible)) {
            auto *env = dKy_getWeatherEnvlight();
            float one = static_cast<float>(weatherOne.value),
                  zero = static_cast<float>(weatherZero.value);
            env->mVrkumoCount = 50;
            bool growing = false;
            if (static_cast<u8>(dKy_getWeatherEnvlight()->mColpatCurr) == 1 &&
                static_cast<float>(dKy_getWeatherEnvlight()->mColPatBlend) > zero)
                growing = true;
            else if (static_cast<u8>(dKy_getWeatherEnvlight()->mColpatPrev) == 1 &&
                     static_cast<float>(dKy_getWeatherEnvlight()->mColPatBlend) < one)
                growing = true;
            else if (static_cast<u8>(dKy_getWeatherEnvlight()->mColpatCurr) == 2 &&
                     static_cast<float>(dKy_getWeatherEnvlight()->mColPatBlend) > zero)
                growing = true;
            else if (static_cast<u8>(dKy_getWeatherEnvlight()->mColpatPrev) == 2 &&
                     static_cast<float>(dKy_getWeatherEnvlight()->mColPatBlend) < one)
                growing = true;
            float step = static_cast<float>(growing ? weatherCloudGrowStep.value : weatherCloudFadeStep.value);
            float minimum = static_cast<float>(growing ? weatherCloudGrowMinimumStep.value : weatherCloudFadeMinimumStep.value);
            float damping = static_cast<float>(growing ? weatherWindDamping.value : weatherCloudFadeDamping.value);
            weatherAddCalc(environment() + 0xA90, growing ? one : zero, damping, step, minimum);
            if (weather_stage_equals(gabi::guest_addr("sea")) &&
                static_cast<s8>(weatherStayRoomNo) == 9) {
                float squared = weatherVectorSquaredDistance(camera + 0xDC, (u32)0x104775B0);
                float distance = weatherSqrt(squared);
                if (distance < static_cast<float>(weatherSeaCloudRadius.value))
                    dKy_getWeatherEnvlight()->mVrkumoStrength = one;
            }
            float fifty = static_cast<float>(weatherCloudCountScale.value);
            env = dKy_getWeatherEnvlight();
            float strength = static_cast<float>(dKy_getWeatherEnvlight()->mVrkumoStrength);
            env->mVrkumoCount = (s16)gabi::ftoi(add_product(fifty, strength, fifty));
            if (weather_stage_equals(gabi::guest_addr("GTower")))
                dKy_getWeatherEnvlight()->mVrkumoCount = 0;
        } else
            dKy_getWeatherEnvlight()->mVrkumoCount = 0;
    }
    u8 status = static_cast<u8>(dKy_getWeatherEnvlight()->mVrkumoStatus);
    if (status == 1) {
        weathervrkumo_move();
        return;
    }
    if (status != 0)
        return;
    if (!static_cast<u32>(dKy_getWeatherEnvlight()->mVrkumoCount))
        return;
    gabi::Local<gabi::be<u32>> heapScope;
    weatherBeginHeap(heapScope.a, gabi::guest_addr("dKankyo_vrkumo_Packet"), (u32)0x18F0D0, (u32)0);
    u32 packet = weatherAllocateAligned((u32)0xC5E20, (u32)0x20);
    if (packet)
        packet = weatherConstructVrkumoPacket(packet);
    dKy_getWeatherEnvlight()->mpVrkumoPacket = gabi::at<void>(packet);
    if (!static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpVrkumoPacket))) {
        weatherAbortHeap(heapScope.a);
        weatherEndHeap(heapScope.a, (u32)2);
        return;
    }
    bool model =
        weatherGetStageResource(gabi::guest_addr("Stage"), gabi::guest_addr("vr_box.bmd")) != 0;
    packet = static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpVrkumoPacket));
    u32 resource = weatherGetStageResource(gabi::guest_addr("Stage"),
                                           model ? gabi::guest_addr("cloud_01.bti")
                                                 : gabi::guest_addr("cloudtx_01.bti"));
    gabi::at<dKankyo_vrkumo_Packet>(packet)->mpCloudTextures[0] = resource;
    packet = static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpVrkumoPacket));
    resource = weatherGetStageResource(gabi::guest_addr("Stage"),
                                       model ? gabi::guest_addr("cloud_02.bti")
                                             : gabi::guest_addr("cloudtx_02.bti"));
    gabi::at<dKankyo_vrkumo_Packet>(packet)->mpCloudTextures[1] = resource;
    resource = weatherGetStageResource(gabi::guest_addr("Stage"),
                                       model ? gabi::guest_addr("cloud_03.bti")
                                             : gabi::guest_addr("cloudtx_03.bti"));
    gabi::store<u32>(static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpVrkumoPacket)) + 0xA0,
                     resource);
    if (!gabi::load<u32>(static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpVrkumoPacket)) +
                         0x98))
        dKy_getWeatherEnvlight()->mVrkumoStatus = 99;
    for (u32 i = 0; i < 100; ++i)
        gabi::store<u8>(static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpVrkumoPacket)) +
                            0xA4 + i * 0x2C,
                        0);
    weathervrkumo_move();
    auto *env = dKy_getWeatherEnvlight();
    env->mVrkumoStatus = static_cast<u8>(env->mVrkumoStatus) + 1;
    weatherRefreshVrkumoTextures(
        static_cast<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpVrkumoPacket)));
    weatherEndHeap(heapScope.a, (u32)2);
}
static u32 weather_packet(u32 offset) { return gabi::load<u32>(environment() + offset); }
static void start_sun_weather(float zero) {
    gabi::Local<gabi::be<u32>> heapScope;
    weatherBeginHeap(heapScope.a, gabi::guest_addr("dKankyo_sun_Packet"), (u32)0x2B9D0, (u32)0);
    u32 packet = weatherAllocateAligned((u32)0x2628, (u32)0x20);
    if (packet)
        packet = weatherdKyw_wether_init(packet);
    gabi::store<u32>(gabi::ea(&dKy_getWeatherEnvlight()->mpSunPacket), packet);
    packet = weatherAllocateAligned((u32)0xA088, (u32)0x20);
    if (packet)
        packet = weatherConstructSunlenzPacket(packet);
    gabi::store<u32>(gabi::ea(&dKy_getWeatherEnvlight()->mpSunlenzPacket), packet);
    if (!gabi::ea(dKy_getWeatherEnvlight()->mpSunPacket) ||
        !gabi::ea(dKy_getWeatherEnvlight()->mpSunlenzPacket))
        weatherAbortHeap(heapScope.a);
    else {
        gabi::Local<SafeString> archiveName, resourceName;
        gabi::store<u32>(archiveName.a, gabi::ea(&weatherSunArchiveName));
        gabi::store<u32>(archiveName.a + 4, gabi::ea(&weatherStringVtable));
        gabi::store<u32>(resourceName.a, gabi::ea(&weatherSunArchiveFile));
        gabi::store<u32>(resourceName.a + 4, gabi::ea(&weatherStringVtable));
        u32 entry = weatherGetArchiveResource(static_cast<u32>(weatherArchiveManager),
                                              archiveName.a, resourceName.a, (u32)0);
        u32 archive = weatherGetArchive(entry);
        const u32 names[] = {gabi::guest_addr("txa_tuki_as"), gabi::guest_addr("txa_tuki_bs"),
                             gabi::guest_addr("txa_tuki_cs"), gabi::guest_addr("txa_tuki_ds"),
                             gabi::guest_addr("txa_taiyo")};
        for (u32 i = 0; i < 5; ++i) {
            u32 resource = weatherGetArchiveTexture(archive_resource_array(archive), names[i]);
            gabi::store<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpSunPacket) + 0xD4 + 4 * i,
                             resource);
        }
        gabi::store<u8>(gabi::ea(dKy_getWeatherEnvlight()->mpSunPacket) + 0xC4, 0);
        gabi::store<u8>(gabi::ea(dKy_getWeatherEnvlight()->mpSunPacket) + 0xC5, 0);
        for (u32 offset = 0xC8; offset <= 0xD0; offset += 4)
            gabi::store<float>(gabi::ea(dKy_getWeatherEnvlight()->mpSunPacket) + offset, zero);
        for (u32 offset = 0xB0; offset <= 0xBC; offset += 4)
            gabi::store<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpSunPacket) + offset, 0);
        const u32 cloudNames[] = {gabi::guest_addr("tx_snow01"),
                                  gabi::guest_addr("txa_lens_32half"),
                                  gabi::guest_addr("txa_ring_a_32hafe")};
        for (u32 i = 0; i < 3; ++i) {
            u32 resource = weatherGetArchiveTexture(archive_resource_array(archive), cloudNames[i]);
            gabi::store<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpSunlenzPacket) + 0x98 + 4 * i,
                             resource);
        }
        u32 cloud = gabi::ea(dKy_getWeatherEnvlight()->mpSunlenzPacket);
        gabi::store<float>(cloud + 0x118, static_cast<float>(weatherSunlenzDepthLimit.value));
        gabi::store<float>(gabi::ea(dKy_getWeatherEnvlight()->mpSunlenzPacket) + 0x11C, zero);
        gabi::store<float>(gabi::ea(dKy_getWeatherEnvlight()->mpSunlenzPacket) + 0x124, zero);
        gabi::store<u8>(gabi::ea(dKy_getWeatherEnvlight()->mpSunlenzPacket) + 0x12A, 0);
        u32 texture = gabi::load<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpSunlenzPacket) + 0x98);
        weatherInitializeSunGeometry(gabi::ea(dKy_getWeatherEnvlight()->mpSunPacket), texture);
        weatherInitializeSunlenzGeometry(gabi::ea(dKy_getWeatherEnvlight()->mpSunlenzPacket));
        weatherdKyr_sun_move();
        weatherdKyr_lenzflare_move();
        (dKy_getWeatherEnvlight()->mbSunInitialized = 1);
    }
    weatherEndHeap(heapScope.a, (u32)2);
}
static void move_rain_weather() {
    u8 status = static_cast<u8>(dKy_getWeatherEnvlight()->mbRainInitialized);
    switch (status) {
    case 0:
        if (static_cast<u32>(dKy_getWeatherEnvlight()->mRainCount)) {
            gabi::Local<gabi::be<u32>> heapScope;
            weatherBeginHeap(heapScope.a, gabi::guest_addr("dKankyo_rain_Packet"), (u32)0x3637D0,
                             (u32)0);
            u32 packet = weatherAllocateAligned((u32)0x1ACA40, (u32)0x20);
            if (packet)
                packet = weatherConstructRainPacket(packet);
            gabi::store<u32>(gabi::ea(&dKy_getWeatherEnvlight()->mpRainPacket), packet);
            if (!gabi::ea(dKy_getWeatherEnvlight()->mpRainPacket))
                weatherAbortHeap(heapScope.a);
            else {
                weatherdKyr_rain_init();
                weatherdKyr_rain_move();
                (dKy_getWeatherEnvlight()->mbRainInitialized = 1);
                if (static_cast<u32>(dKy_getWeatherEnvlight()->mRainCount) != 250)
                    weatherStartSound((u32)0x185C);
            }
            weatherEndHeap(heapScope.a, (u32)2);
        }
        break;
    case 1:
        weatherdKyr_rain_move();
        if (!static_cast<u32>(dKy_getWeatherEnvlight()->mSnowCount)) {
            float count = (float)static_cast<s32>(dKy_getWeatherEnvlight()->mRainCount);
            weatherPlayRainSound(
                (u32)(count < static_cast<float>(weatherHeavyRainThreshold.value) ? 0 : 1));
        }
        if (!static_cast<u32>(dKy_getWeatherEnvlight()->mRainCount)) {
            weatherStartSound((u32)0x185D);
            auto* env = dKy_getWeatherEnvlight();
            (env->mbRainInitialized = static_cast<u8>(env->mbRainInitialized) + 1);
        }
        break;
    case 2: {
        auto* env = dKy_getWeatherEnvlight();
        (env->mbRainInitialized = static_cast<u8>(env->mbRainInitialized) + 1);
        break;
    }
    case 3:
        (dKy_getWeatherEnvlight()->mbRainInitialized = 0);
        weatherDeleteRainPacket(gabi::ea(&dKy_getWeatherEnvlight()->mpRainPacket));
        break;
    }
}
static void move_snow_weather() {
    u8 status = static_cast<u8>(dKy_getWeatherEnvlight()->mbSnowInitialized);
    if (status == 0) {
        if (static_cast<u32>(dKy_getWeatherEnvlight()->mSnowCount)) {
            weatherCall_dKyr_snow_init();
            if (gabi::ea(dKy_getWeatherEnvlight()->mpSnowPacket)) {
                if (weather_stage_equals(gabi::guest_addr("Adanmae")))
                    weatherdKyr_kazanbai_move();
                else
                    weatherdKyr_snow_move();
                (dKy_getWeatherEnvlight()->mbSnowInitialized = 1);
            }
        }
    } else if (status == 1) {
        if (weather_stage_equals(gabi::guest_addr("Adanmae")))
            weatherdKyr_kazanbai_move();
        else
            weatherdKyr_snow_move();
    }
}
static void update_star_amount(float zero) {
    float time = weatherGetDaytime(environment());
    bool special = weather_stage_equals(gabi::ea(&weatherStarNameStage));
    float target = static_cast<float>(weatherOne.value),
          rate = static_cast<float>(weatherStarDawnScale.value);
    if (special) {
        if (!(time < static_cast<float>(weatherNameNightStart.value))) {
        } else if (time < static_cast<float>(weatherNameNightEnd.value)) {
        } else if (time > static_cast<float>(weatherNameDuskStart.value))
            target = subtract_product(
                gabi::fsubs_ppc(static_cast<float>(weatherNameNightStart.value), time),
                static_cast<float>(weatherNameDuskScale.value), target);
        else if (time < static_cast<float>(weatherNameDawnEnd.value))
            target = gabi::fmuls_ppc(
                gabi::fsubs_ppc(static_cast<float>(weatherNameDawnEnd.value), time), rate);
        else
            target = zero;
    } else {
        if (!(time < static_cast<float>(weatherNightStart.value))) {
        } else if (time < static_cast<float>(weatherNightEnd.value)) {
        } else if (time > static_cast<float>(weatherDuskStart.value))
            target =
                subtract_product(gabi::fsubs_ppc(static_cast<float>(weatherNightStart.value), time),
                                 static_cast<float>(weatherDuskScale.value), target);
        else if (time < static_cast<float>(weatherDawnEnd.value))
            target = gabi::fmuls_ppc(
                gabi::fsubs_ppc(static_cast<float>(weatherDawnEnd.value), time), rate);
        else
            target = zero;
    }
    if (static_cast<u8>(dKy_getWeatherEnvlight()->mColpatWeather))
        target = zero;
    if (static_cast<u8>(dKy_getWeatherEnvlight()->mColpatCurr) &&
        static_cast<float>(dKy_getWeatherEnvlight()->mColPatBlend) >
            static_cast<float>(weatherStarFadeThreshold.value))
        target = zero;
    auto* env = dKy_getWeatherEnvlight();
    weatherAddCalc(gabi::ea(env) + 0xA5C, target, static_cast<float>(weatherWindDamping.value),
                   static_cast<float>(weatherStarStep.value),
                   static_cast<float>(weatherStarMinimumStep.value));
    env = dKy_getWeatherEnvlight();
    float strength = static_cast<float>(dKy_getWeatherEnvlight()->mStarCountTarget);
    (env->mStarCount = (s16)gabi::ftoi(
         gabi::fmuls_ppc(static_cast<float>(weatherStarCountScale.value), strength)));
}
static void move_star_weather() {
    u8 status = static_cast<u8>(dKy_getWeatherEnvlight()->mbStarInitialized);
    switch (status) {
    case 0:
        if (static_cast<u32>(dKy_getWeatherEnvlight()->mStarCount)) {
            weatherdKyr_star_init();
            if (gabi::ea(dKy_getWeatherEnvlight()->mpStarPacket)) {
                weatherInitializeStarGeometry(gabi::ea(dKy_getWeatherEnvlight()->mpStarPacket));
                weatherCall_dKyr_star_move();
                (dKy_getWeatherEnvlight()->mbStarInitialized = 1);
            }
        }
        break;
    case 1:
        weatherCall_dKyr_star_move();
        if (static_cast<u32>(dKy_getWeatherEnvlight()->mStarCount))
            break;
        [[fallthrough]];
    case 2: {
        auto* env = dKy_getWeatherEnvlight();
        (env->mbStarInitialized = static_cast<u8>(env->mbStarInitialized) + 1);
        break;
    }
    case 3:
        (dKy_getWeatherEnvlight()->mbStarInitialized = 0);
        weatherDeleteStarPacket(gabi::ea(&dKy_getWeatherEnvlight()->mpStarPacket));
        break;
    }
}
static void start_mud_weather(float zero) {
    gabi::Local<gabi::be<u32>> heapScope;
    weatherBeginHeap(heapScope.a, gabi::guest_addr("dKankyo_housi_Packet"), (u32)0x2FBF10, (u32)0);
    u32 packet = weatherAllocateAligned((u32)0x139250, (u32)0x20);
    if (packet)
        packet = weatherConstructHousiPacket(packet);
    gabi::store<u32>(gabi::ea(&dKy_getWeatherEnvlight()->mpHousiPacket), packet);
    if (!gabi::ea(dKy_getWeatherEnvlight()->mpHousiPacket))
        weatherAbortHeap(heapScope.a);
    else {
        gabi::Local<SafeString> archiveName, resourceName;
        gabi::store<u32>(archiveName.a, gabi::ea(&weatherHousiArchiveName));
        gabi::store<u32>(archiveName.a + 4, gabi::ea(&weatherStringVtable));
        gabi::store<u32>(resourceName.a, gabi::ea(&weatherHousiArchiveFile));
        gabi::store<u32>(resourceName.a + 4, gabi::ea(&weatherStringVtable));
        u32 entry = weatherGetArchiveResource(static_cast<u32>(weatherArchiveManager),
                                              archiveName.a, resourceName.a, (u32)0);
        u32 archive = weatherGetArchive(entry);
        u32 resource = weatherGetArchiveTexture(archive_resource_array(archive),
                                                gabi::guest_addr("ak_houshi00"));
        gabi::store<u32>(gabi::ea(dKy_getWeatherEnvlight()->mpHousiPacket) + 0x98, resource);
        gabi::store<float>(gabi::ea(dKy_getWeatherEnvlight()->mpHousiPacket) + 0x5E64, zero);
        for (u32 i = 0; i < 300; ++i)
            gabi::store<u8>(gabi::ea(dKy_getWeatherEnvlight()->mpHousiPacket) + 0x9C + i * 0x50, 0);
        weatherInitializeHousiGeometry(gabi::ea(dKy_getWeatherEnvlight()->mpHousiPacket));
        weatherdKyr_housi_move();
        (dKy_getWeatherEnvlight()->mbHousiInitialized = 1);
    }
    weatherEndHeap(heapScope.a, (u32)2);
}
static void move_mud_weather(float zero) {
    u8 status = static_cast<u8>(dKy_getWeatherEnvlight()->mbHousiInitialized);
    switch (status) {
    case 0:
        if (static_cast<u32>(dKy_getWeatherEnvlight()->mHousiCount))
            start_mud_weather(zero);
        break;
    case 1:
        if (static_cast<u32>(dKy_getWeatherEnvlight()->mHousiCount) ||
            gabi::load<float>(gabi::ea(dKy_getWeatherEnvlight()->mpHousiPacket) + 0x5E64) > zero) {
            weatherdKyr_housi_move();
            (dKy_getWeatherEnvlight()->mHousiCount = 0);
        } else {
            auto* env = dKy_getWeatherEnvlight();
            (env->mbHousiInitialized = static_cast<u8>(env->mbHousiInitialized) + 1);
        }
        break;
    case 2: {
        (dKy_getWeatherEnvlight()->mHousiCount = 0);
        auto* env = dKy_getWeatherEnvlight();
        (env->mbHousiInitialized = static_cast<u8>(env->mbHousiInitialized) + 1);
        break;
    }
    case 3:
        (dKy_getWeatherEnvlight()->mHousiCount = 0);
        (dKy_getWeatherEnvlight()->mbHousiInitialized = 0);
        weatherDeleteHousiPacket(gabi::ea(&dKy_getWeatherEnvlight()->mpHousiPacket));
        break;
    }
    weatherwether_move_moya();
}
static bool weather_stage_wrapped_equal(u32 name) {
    gabi::Local<SafeString> expected, actual;
    u32 first = weatherConstructSafeString(expected.a, name);
    u32 game = weatherGetPlayRegister();
    u32 second = weatherConstructSafeString(actual.a, game + 0x5134);
    u32 result = weatherSafeStringEqual(first, second);
    weatherDestroySafeString(actual.a, (u32)2);
    weatherDestroySafeString(expected.a, (u32)2);
    return result == 1;
}
/* 0257ACAC */
static void dKyw_wether_move_draw() {

    (dKy_getWeatherEnvlight()->mMoyaSE = 0);
    bool special = weather_stage_equals(gabi::guest_addr("Name"));
    float zero = static_cast<float>(weatherZero.value);
    if (!special) {
        u32 game = weatherGetPlayRegister();
        bool sun = (static_cast<u16>(gabi::at<WeatherPlay>(game)->mStatus) & 1) &&
                   !static_cast<u8>(dKy_getWeatherEnvlight()->mbVrboxInvisible) &&
                   !weather_stage_equals(gabi::guest_addr("Hyrule")) &&
                   !weather_stage_equals(gabi::ea(&weatherSunGanonStage)) &&
                   !weather_stage_equals(gabi::ea(&weatherSunTowerStage));
        if (sun) {
            u8 state = static_cast<u8>(dKy_getWeatherEnvlight()->mbSunInitialized);
            if (state == 0)
                start_sun_weather(zero);
            else if (state == 1) {
                weatherdKyr_sun_move();
                weatherdKyr_lenzflare_move();
            }
        }
        move_rain_weather();
        move_snow_weather();
    }
    u32 game = weatherGetPlayRegister();
    bool stars = (static_cast<u16>(gabi::at<WeatherPlay>(game)->mStatus) & 1) &&
                 !static_cast<u8>(dKy_getWeatherEnvlight()->mbVrboxInvisible) &&
                 !weather_stage_equals(gabi::guest_addr("M_DragB")) &&
                 !weather_stage_equals(gabi::ea(&weatherStarGanonStage)) &&
                 !weather_stage_equals(gabi::ea(&weatherStarTowerStage));
    if (!stars)
        stars = weather_stage_equals(gabi::ea(&weatherStarNameStage));
    if (stars) {
        update_star_amount(zero);
        move_star_weather();
    }
    if (weather_stage_equals(gabi::guest_addr("Name"))) {
        if (!weather_stage_wrapped_equal(gabi::guest_addr("Name")))
            weatherwether_move_wave();
        return;
    }
    if (weather_stage_equals(gabi::guest_addr("M_Dai")))
        dKy_getWeatherEnvlight()->mPoisonCount = 500;
    u8 poison = static_cast<u8>(dKy_getWeatherEnvlight()->mbPoisonInitialized);
    if (poison == 0) {
        if (static_cast<u32>(dKy_getWeatherEnvlight()->mPoisonCount)) {
            weatherpoison_init();
            dKy_getWeatherEnvlight()->mbPoisonInitialized = 1;
        }
    } else if (poison == 1)
        weatherpoison_move();
    move_mud_weather(zero);
    if (!weather_stage_wrapped_equal(gabi::guest_addr("Name")))
        weatherwether_move_wave();
}
static void configure_cloud_render_state(u32 address, float zero, bool clearHead = true) {
    auto* state = gabi::at<WeatherRenderState>(address);
    state->mDepthEnabled = 0;
    u32 bits = state->mBlendFactors;
    state->mDepthFunction = 4;
    state->mDepthCompare = 2;
    state->mDepthBias = zero;
    state->mBlendEnabled = 1;
    if (clearHead)
        state->mCullEnabled = 0;
    state->mBlendMode = 3;
    state->mAlphaMode = 3;
    state->mBlendFactors = (((bits & ~15u) + 7) & ~0xF0u) + 16;
}

/* 0257F490 */
void dKankyo_rain_Packet::initializeGeometry() {
    u32 packet = gabi::ea(this);
    auto* packetView = this;

    packetView->mpMaterial = lookup_weather_material(packet, gabi::guest_addr("housi"));
    for (u32 i = 0; i < 250; ++i) {
        for (u32 quad = 0; quad < 4; ++quad)
            prepare_weather_pair(gabi::ea(&packetView->mSplashGeometry[i * 4 + quad]),
                                 static_cast<u32>(packetView->mpMaterial),
                                 gabi::ea(&weatherRainAttributes));
        weatherSetTextureMode(gabi::ea(&packetView->mSplashTextures[i]), (u32)0);
        prepare_weather_pair(gabi::ea(&packetView->mRainGeometry[i]),
                             static_cast<u32>(packetView->mpMaterial),
                             gabi::ea(&weatherRainAttributes));
    }
    gabi::Local<WeatherTextureDescriptor> texture;
    weatherConstructTextureDescriptor(texture.a);
    for (u32 i = 0; i < 2; ++i) {
        u32 manager = static_cast<u32>(weatherTextureManager),
            resource = static_cast<u32>(packetView->mpTextures[i]);
        weatherLockTextures(manager);
        weatherImportTexture(texture.a, resource);
        weatherUnlockTextures(static_cast<u32>(weatherTextureManager));
        u32 target = gabi::ea(&packetView->mTextureSet) + i * 0x4C;
        weatherSetTexture(target, (u32)0, texture.a, (u32)0);
        target = weatherGetTexture(target, (u32)0);
        auto* binding = gabi::at<WeatherTextureBinding>(target);
        u8 flags = binding->mFlags;
        binding->mSamplerStateDirty[2] = 1;
        binding->mSamplerStateDirty[0] = 1;
        binding->mSamplerStateDirty[1] = 1;
        binding->mFlags = flags | 2;
    }
    weatherSetRendererMode(gabi::ea(&packetView->mRenderer), (u32)1, (u32)0);
    weatherSetTextureMode(gabi::ea(&packetView->mRenderer.mTexture), (u32)0);
    weatherSetTextureMode(gabi::ea(&packetView->mRainTexture), (u32)0);
}
} // namespace d_kankyo_wether_cpp

// HD: packet renderer storage and its compiler-generated lifetime code.
/* 025786F0 */
u32 d_kankyo_wether_cpp::dKankyo_sun_Packet::construct() {
    u32 object = gabi::ea(this);

    if (!object)
        object = weatherAllocateRegister((u32)0x2628);
    if (!object)
        return 0;
    auto *packetView = gabi::at<dKankyo_sun_Packet>(object);
    weatherConstructPacket(object);
    (packetView->mPacket.__vtbl = gabi::ea(&weatherSunVtable));
    weatherClear(gabi::ea(&packetView->mVizChkData[0]), (u32)0x14);
    float zero = static_cast<float>(weatherZero.value);
    (packetView->mOcclusionDelayHD = 0);
    (packetView->mVisibility = zero);
    (packetView->mMoonAlpha = zero);
    (packetView->mHasVisiblePointHD = 0);
    (packetView->mSunAlpha = zero);
    weatherClear(gabi::ea(&packetView->mpTextureData[0]), (u32)0x14);
    (packetView->mFullyVisibleDelayHD = 0);
    (packetView->mpMaterial = 0);
    weatherConstructArray(gabi::ea(&packetView->mGeometry[0].mGeometry[0].mpVertices), (u32)4,
                          (u32)0x4C0, (u32)0x02582D58);
    weatherConstructArray(gabi::ea(&packetView->mRenderer[0].mPrefix), (u32)4, (u32)0x41C,
                          (u32)0x02582E0C);
    weatherConstructArray(gabi::ea(&packetView->mTextureSet[0].mState), (u32)6, (u32)0x4C,
                          (u32)0x027FE344);
    return object;
}

/* 025787D4 */
u32 d_kankyo_wether_cpp::dKankyo_sunlenz_Packet::construct() {
    u32 object = gabi::ea(this);

    if (!object)
        object = weatherAllocateRegister((u32)0xA088);
    if (!object)
        return 0;
    auto *packetView = gabi::at<dKankyo_sunlenz_Packet>(object);
    weatherConstructPacket(object);
    (packetView->mPacket.__vtbl = gabi::ea(&weatherSunlenzVtable));
    weatherClear(gabi::ea(&packetView->mpTextures[0]), (u32)12);
    float zero = static_cast<float>(weatherZero.value);
    (packetView->mRotation = 0);
    (packetView->mbDrawLenzInSky = 0);
    (packetView->mpMaterial = 0);
    packetView->mPositionScale = zero;
    packetView->mPositionOffset = zero;
    packetView->mDepthLimit = zero;
    packetView->mVisibility = zero;
    packetView->mAngleDeg = zero;
    packetView->mDistFalloff = zero;
    weatherConstructArray(gabi::ea(&packetView->mGeometry[0].mGeometry[0].mpVertices), (u32)9,
                          (u32)0x4C0, (u32)0x02582D58);
    weatherConstructArray(gabi::ea(&packetView->mRenderer[0].mPrefix), (u32)9, (u32)0x41C,
                          (u32)0x02582E0C);
    weatherConstructArray(gabi::ea(&packetView->mTextureSet[0].mState), (u32)3, (u32)0x4C,
                          (u32)0x027FE344);
    u32 geometry = gabi::ea(&packetView->mCloudGeometry.mGeometry[0].mpVertices);
    (packetView->mpCloudMaterial = 0);
    if (!geometry)
        geometry = weatherAllocateRegister((u32)0x4A98);
    if (geometry) {
        weatherConstructArray(geometry, (u32)32, (u32)0x254, (u32)0x025830D0);
        (gabi::at<WeatherCloudGeometry>(geometry)->mBufferIndex = 0);
        (gabi::at<WeatherCloudGeometry>(geometry)->mpMaterial = 0);
        (gabi::at<WeatherCloudGeometry>(geometry)->mAttributeCount = 0x98);
        (gabi::at<WeatherCloudGeometry>(geometry)->mbInitialized = 0);
        (gabi::at<WeatherCloudGeometry>(geometry)->mPrimitive = 0);
        for (u32 side = 0; side < 2; ++side)
            for (u32 i = 0; i < 16; ++i)
                (gabi::at<WeatherCloudGeometry>(geometry)->mGeometry[side + 2 * i].mpVertices = 0);
    }
    u32 renderer = gabi::ea(&packetView->mCloudRenderer.mPrefix);
    auto *rendererView = gabi::at<WeatherRenderer>(renderer);
    weatherConstructRenderer(renderer);
    weatherConstructTexture(renderer + 12);
    (rendererView->mPrefix.mTexture.__vtbl = gabi::ea(&weatherSmallTextureVtable));
    weatherClear(renderer + 0x80, (u32)0x34);
    if (renderer + 0x80 == 0)
        weatherAllocateDiscard((u32)0x30);
    weatherConstructTexture(renderer + 0xB4);
    (rendererView->mExtendedTexture.__vtbl = gabi::ea(&weatherTextureVtable));
    weatherClear(renderer + 0x128, (u32)0x2F0);
    construct_texture_constants(renderer, 0x128);
    (rendererView->mStatus = 0);
    return object;
}

/* 02578BC0 */
void d_kankyo_wether_cpp::dKankyo_sun_Packet::initializeGeometry(u32 alternateTexture) {
    u32 packet = gabi::ea(this);

    u32 material = lookup_weather_material(packet, gabi::guest_addr("sun"));
    (this->mpMaterial = material);
    for (u32 i = 0; i < 4; ++i) {
        u32 pair = gabi::ea(&this->mGeometry[i].mGeometry[0].mpVertices);
        auto *pairView = gabi::at<WeatherGeometryPair>(pair);
        for (u32 side = 0; side < 2; ++side)
            prepare_weather_element(pair + side * 0x254, gabi::ea(&pairView->mPrimitive),
                                    gabi::ea(&pairView->mpMaterial), material, 4, 0x260);
        (pairView->mpMaterial = material);
        (pairView->mbInitialized = 1);
        weatherInitializeRenderer(gabi::ea(&this->mRenderer[i].mPrefix), (u32)1, (u32)0);
        if (i != 3)
            material = static_cast<u32>(this->mpMaterial);
    }
    for (u32 i = 0; i < 6; ++i) {
        u32 renderer = gabi::ea(&this->mTextureSet[i].mState);
        weatherInitializeTextureSet(renderer);
        gabi::Local<WeatherTextureDescriptor> texture;
        weatherConstructTextureDescriptor(texture.a);
        u32 manager = static_cast<u32>(weatherTextureManager);
        u32 resource = i == 5 ? alternateTexture : static_cast<u32>(this->mpTextureData[i]);
        weatherLockTextures(manager);
        weatherImportTexture(texture.a, resource);
        weatherUnlockTextures(static_cast<u32>(weatherTextureManager));
        weatherSetTexture(renderer, (u32)0, texture.a, (u32)0);
    }
    (this->mFullyVisibleDelayHD = 0);
}

/* 02578E60 */
void d_kankyo_wether_cpp::dKankyo_sunlenz_Packet::initializeGeometry() {
    u32 packet = gabi::ea(this);

    u32 first = lookup_weather_material(packet, gabi::guest_addr("sunlight"));
    (this->mpCloudMaterial = first);
    u32 material = lookup_weather_material(first, gabi::guest_addr("lenzflare"));
    (this->mpMaterial = material);
    for (u32 i = 0; i < 9; ++i) {
        u32 pair = gabi::ea(&this->mGeometry[i].mGeometry[0].mpVertices);
        auto *pairView = gabi::at<WeatherGeometryPair>(pair);
        for (u32 side = 0; side < 2; ++side)
            prepare_weather_element(pair + side * 0x254, gabi::ea(&pairView->mPrimitive),
                                    gabi::ea(&pairView->mpMaterial), material, 4, 0x260);
        (pairView->mpMaterial = material);
        (pairView->mbInitialized = 1);
        weatherInitializeRenderer(gabi::ea(&this->mRenderer[i].mPrefix), (u32)1, (u32)0);
        if (i != 8)
            material = static_cast<u32>(this->mpMaterial);
    }
    gabi::Local<WeatherTextureDescriptor> texture;
    weatherConstructTextureDescriptor(texture.a);
    for (u32 i = 0; i < 3; ++i) {
        u32 manager = static_cast<u32>(weatherTextureManager);
        u32 resource = static_cast<u32>(this->mpTextures[i]);
        weatherLockTextures(manager);
        weatherImportTexture(texture.a, resource);
        weatherUnlockTextures(static_cast<u32>(weatherTextureManager));
        u32 renderer = gabi::ea(&this->mTextureSet[i].mState);
        weatherSetTexture(renderer, (u32)0, texture.a, (u32)0);
        if (i) {
            u32 target = weatherGetTexture(renderer, (u32)0);
            auto* binding = gabi::at<WeatherTextureBinding>(target);
            u8 flags = binding->mFlags;
            binding->mSamplerStateDirty[1] = 1;
            binding->mSamplerStateDirty[0] = 1;
            binding->mFlags = flags | 2;
            binding->mSamplerStateDirty[2] = 1;
        }
    }
    material = static_cast<u32>(this->mpCloudMaterial);
    u32 geometry = gabi::ea(&this->mCloudGeometry.mGeometry[0].mpVertices);
    for (u32 side = 0; side < 2; ++side)
        for (u32 i = 0; i < 16; ++i)
            prepare_weather_element(geometry + side * 0x254 + i * 0x4A8, geometry + 0x4A84,
                                    geometry + 0x4A90, material, 3, 0x1C8);
    (gabi::at<WeatherCloudGeometry>(geometry)->mpMaterial = material);
    (gabi::at<WeatherCloudGeometry>(geometry)->mbInitialized = 1);
    weatherInitializeRenderer(gabi::ea(&this->mCloudRenderer.mPrefix), (u32)1, (u32)0);
}

/* 02579280 */
u32 d_kankyo_wether_cpp::dKankyo_rain_Packet::construct() {
    u32 object = gabi::ea(this);

    if (!object)
        object = weatherAllocateRegister((u32)0x1ACA40);
    if (!object)
        return 0;
    auto *packetView = gabi::at<dKankyo_rain_Packet>(object);
    weatherConstructPacket(object);
    (packetView->mPacket.__vtbl = gabi::ea(&weatherRainVtable));
    weatherClear(gabi::ea(&packetView->mpTextures[0]), (u32)8);
    weatherClear(gabi::ea(&packetView->mEff[0].mStatus), (u32)0x36B0);
    float zero = static_cast<float>(weatherZero.value);
    (packetView->mRainCount = 0);
    (packetView->mStatus = 0);
    (packetView->mFrame = 0);
    packetView->mOverheadFade = zero;
    packetView->mFwdFade1 = zero;
    packetView->mCenterDeltaMul = zero;
    packetView->mSibukiAlpha = zero;
    packetView->mWindScale = zero;
    packetView->mFwdFade2 = zero;
    packetView->mSplashSpeed = zero;
    packetView->mWindSpeed = zero;
    weatherClear(gabi::ea(&packetView->mSplashActive[0]), (u32)250);
    (packetView->mpMaterial = 0);
    weatherConstructArray(gabi::ea(&packetView->mSplashGeometry[0].mGeometry[0].mpVertices),
                          (u32)1000, (u32)0x4C0, (u32)0x02583188);
    weatherConstructArray(gabi::ea(&packetView->mRainGeometry[0].mGeometry[0].mpVertices), (u32)250,
                          (u32)0x4C0, (u32)0x02583188);
    weatherConstructArray(gabi::ea(&packetView->mSplashTextures[0]), (u32)250, (u32)0x370,
                          (u32)0x02583210);
    u32 texture = gabi::ea(&packetView->mRainTexture);
    auto *textureView = gabi::at<WeatherTexture>(texture);
    weatherConstructTexture(texture);
    (textureView->__vtbl = gabi::ea(&weatherTextureVtable));
    weatherClear(texture + 0x74, (u32)0x2F0);
    construct_texture_constants(texture, 0x74);
    (textureView->__vtbl = gabi::ea(&weatherBufferedTextureVtable));
    (textureView->mBufferIndex = 0);
    (textureView->mFlags = 0);
    u32 renderer = gabi::ea(&packetView->mRenderer);
    auto *rendererView = gabi::at<WeatherRenderer>(renderer);
    weatherConstructRenderer(renderer);
    weatherConstructTexture(renderer + 12);
    (rendererView->mPrefix.mTexture.__vtbl = gabi::ea(&weatherSmallTextureVtable));
    weatherClear(renderer + 0x80, (u32)0x34);
    if (renderer + 0x80 == 0)
        weatherAllocateDiscard((u32)0x30);
    weatherConstructArray(gabi::ea(&packetView->mTextureSet[0].mState), (u32)2, (u32)0x4C,
                          (u32)0x027FE344);
    return object;
}

/* 025796F0 */
void d_kankyo_wether_cpp::dKankyo_star_Packet::initializeGeometry() {
    u32 packet = gabi::ea(this);

    (this->mpMaterial = lookup_weather_material(packet, gabi::guest_addr("star")));
    for (u32 i = 0; i < 2000; ++i) {
        u32 geometry = gabi::ea(&this->mGeometry[i]);
        auto *geometryView = gabi::at<WeatherStarGeometry>(geometry);
        geometryView->mpAttributes = gabi::ea(&weatherStarAttributes);
        geometryView->mPrimitive = 1;
        u32 material = static_cast<u32>(this->mpMaterial);
        for (u32 side = 0; side < 2; ++side)
            for (u32 half = 0; half < 2; ++half)
                prepare_weather_element(gabi::ea(&geometryView->mGeometry[side + 2 * half]),
                                        gabi::ea(&geometryView->mPrimitive),
                                        gabi::ea(&geometryView->mpMaterial), material, 3, 0x24);
        geometryView->mpMaterial = material;
        geometryView->mbInitialized = 1;
    }
    gabi::Local<ColorFloat> color;
    for (u32 i = 0; i < 8; ++i) {
        u32 renderer = gabi::ea(&this->mRenderer[i].mPrefix);
        auto *rendererView = gabi::at<WeatherRenderer>(renderer);
        weatherInitializeRenderer(renderer, (u32)1, (u32)0);
        weatherNormalizeColor(color.a, (u32)(gabi::ea(&weatherStarPalette.mColors[i])));
        for (u32 j = 0; j < 4; ++j)
            rendererView->mExtendedTexture.mParameters.mPrimaryColor.words[j] =
                gabi::at<ColorFloat>(color.a)->words[j];
    }
}

/* 02579934 */
u32 d_kankyo_wether_cpp::dKankyo_housi_Packet::construct() {
    u32 object = gabi::ea(this);

    if (!object) {
        object = weatherAllocateRegister((u32)0x139250);
        if (!object)
            return 0;
    }
    auto *packetView = gabi::at<dKankyo_housi_Packet>(object);
    weatherConstructPacket(object);
    (packetView->mPacket.__vtbl = gabi::ea(&weatherHousiVtable));
    weatherClear(gabi::ea(&packetView->mpTex), (u32)4);
    weatherClear(gabi::ea(&packetView->mEffect[0][0]), (u32)0x5DC0);
    float zero = static_cast<float>(weatherZero.value);
    (packetView->mWindScale = zero);
    (packetView->mWindSpeed = zero);
    (packetView->mAlpha = zero);
    (packetView->mCount = 0);
    (packetView->mFrame = 0);
    weatherClear(gabi::ea(&packetView->mActive[0]), (u32)0x258);
    (packetView->mpMaterial = 0);
    weatherConstructArray(gabi::ea(&packetView->mGeometry[0].mGeometry[0].mpVertices), (u32)600,
                          (u32)0x4C0, (u32)0x0258350C);
    weatherConstructArray(gabi::ea(&packetView->mTextures[0]), (u32)600, (u32)0x370,
                          (u32)0x02583210);
    u32 renderer = gabi::ea(&packetView->mRenderer);
    auto *rendererView = gabi::at<WeatherRenderer>(renderer);
    weatherConstructRenderer(renderer);
    weatherConstructTexture(renderer + 0xC);
    (rendererView->mPrefix.mTexture.__vtbl = gabi::ea(&weatherSmallTextureVtable));
    weatherClear(renderer + 0x80, (u32)0x34);
    if (!(renderer + 0x80))
        weatherAllocateRegister((u32)0x30);
    weatherConstructTextureSet(renderer + 0xB4);
    return object;
}

/* 02579A68 */
void d_kankyo_wether_cpp::dKankyo_housi_Packet::initializeGeometry() {
    u32 packet = gabi::ea(this);

    (this->mpMaterial = lookup_weather_material(packet, gabi::guest_addr("housi_bloom")));
    for (u32 i = 0; i < 600; ++i)
        prepare_weather_pair(gabi::ea(&this->mGeometry[i].mGeometry[0].mpVertices),
                             static_cast<u32>(this->mpMaterial), gabi::ea(&weatherHousiAttributes));
    gabi::Local<WeatherTextureDescriptor> texture;
    weatherConstructTextureDescriptor(texture.a);
    u32 resource = static_cast<u32>(this->mpTex);
    weatherLockTextures(static_cast<u32>(weatherTextureManager));
    weatherImportTexture(texture.a, resource);
    weatherUnlockTextures(static_cast<u32>(weatherTextureManager));
    weatherSetTexture(gabi::ea(&this->mTextureSet.mState), (u32)0, texture.a, (u32)0);
    u32 target = weatherGetTexture(gabi::ea(&this->mTextureSet.mState), (u32)0);
    auto* binding = gabi::at<WeatherTextureBinding>(target);
            u8 flags = binding->mFlags;
    binding->mSamplerStateDirty[2] = 1;
    binding->mSamplerStateDirty[0] = 1;
    binding->mFlags = flags | 2;
    binding->mSamplerStateDirty[1] = 1;
    for (u32 i = 0; i < 600; ++i)
        weatherSetTextureMode(gabi::ea(&this->mTextures[i]), (u32)0);
    weatherSetRendererMode(gabi::ea(&this->mRenderer), (u32)1, (u32)0);
    weatherSetTextureMode(gabi::ea(&this->mRenderer.mTexture), (u32)0);
}

/* 02579D2C */
u32 d_kankyo_wether_cpp::dKankyo_cloud_Packet::construct() {
    u32 object = gabi::ea(this);

    if (!object)
        object = weatherAllocateRegister((u32)0x10038);
    if (!object)
        return 0;
    auto *packetView = gabi::at<dKankyo_cloud_Packet>(object);
    weatherConstructPacket(object);
    (packetView->mPacket.__vtbl = gabi::ea(&weatherCloudVtable));
    weatherClear(gabi::ea(&packetView->mpTexture), (u32)4);
    (packetView->mCount = 0);
    weatherClear(gabi::ea(&packetView->mEff[0]), (u32)0x8E8);
    float zero = static_cast<float>(weatherZero.value);
    (packetView->mpMaterial = 0);
    (packetView->mAlpha = zero);
    (packetView->mActiveCount = 0);
    weatherConstructArray(gabi::ea(&packetView->mGeometry[0].mGeometry[0].mpVertices), (u32)30,
                          (u32)0x4C0, (u32)0x025835F0);
    weatherConstructArray(gabi::ea(&packetView->mTextures[0]), (u32)30, (u32)0x370,
                          (u32)0x02583210);
    weatherConstructRenderer(gabi::ea(&packetView->mRenderer));
    weatherConstructTexture(gabi::ea(&packetView->mRenderer.mTexture));
    (packetView->mRenderer.mTexture.__vtbl = gabi::ea(&weatherSmallTextureVtable));
    weatherClear(gabi::ea(&packetView->mRenderer.mTexture.mParameters), (u32)0x34);
    if (gabi::ea(&packetView->mRenderer.mTexture.mParameters) == 0)
        weatherAllocateDiscard((u32)0x30);
    weatherConstructTextureSet(gabi::ea(&packetView->mTextureSet.mState));
    return object;
}

/* 02579E4C */
void d_kankyo_wether_cpp::dKankyo_cloud_Packet::initializeGeometry() {
    u32 packet = gabi::ea(this);

    (this->mActiveCount = 0);
    (this->mAlpha = static_cast<float>(weatherZero.value));
    (this->mpMaterial = lookup_weather_material(packet, gabi::guest_addr("cloud_shadow")));
    for (u32 i = 0; i < 30; ++i) {
        u32 material = static_cast<u32>(this->mpMaterial);
        prepare_weather_pair(gabi::ea(&this->mGeometry[i].mGeometry[0].mpVertices), material,
                             gabi::ea(&weatherCloudAttributes));
    }
    gabi::Local<WeatherTextureDescriptor> texture;
    weatherConstructTextureDescriptor(texture.a);
    u32 resource = static_cast<u32>(this->mpTexture);
    weatherLockTextures(static_cast<u32>(weatherTextureManager));
    weatherImportTexture(texture.a, resource);
    weatherUnlockTextures(static_cast<u32>(weatherTextureManager));
    weatherSetTexture(gabi::ea(&this->mTextureSet.mState), (u32)0, texture.a, (u32)0);
    u32 target = weatherGetTexture(gabi::ea(&this->mTextureSet.mState), (u32)0);
    auto* binding = gabi::at<WeatherTextureBinding>(target);
            u8 flags = binding->mFlags;
    binding->mSamplerStateDirty[0] = 1;
    binding->mSamplerStateDirty[2] = 1;
    binding->mFlags = flags | 2;
    binding->mSamplerStateDirty[1] = 1;
    for (u32 i = 0; i < 30; ++i)
        weatherSetTextureMode(gabi::ea(&this->mTextures[i]), (u32)0);
    weatherSetRendererMode(gabi::ea(&this->mRenderer), (u32)1, (u32)0);
    weatherSetTextureMode(gabi::ea(&this->mRenderer.mTexture), (u32)0);
}

/* 0257A0E8 */
void d_kankyo_wether_cpp::dKankyo_vrkumo_Packet::initializeGeometry() {
    u32 packet = gabi::ea(this);

    (this->mpMaterial = lookup_weather_material(packet, gabi::guest_addr("cloud_draw")));
    for (u32 group = 0; group < 2; ++group) {
        u32 geometry = gabi::ea(&this->mGeometry[group]);
        auto *geometryView = gabi::at<WeatherVrkumoGeometry>(geometry);
        geometryView->mPrimitive = 17;
        geometryView->mpAttributes = gabi::ea(&weatherVrkumoAttributes);
        u32 material = static_cast<u32>(this->mpMaterial);
        for (u32 side = 0; side < 2; ++side)
            for (u32 i = 0; i < 300; ++i)
                prepare_weather_element(gabi::ea(&geometryView->mGeometry[side + 2 * i]),
                                        gabi::ea(&geometryView->mPrimitive),
                                        gabi::ea(&geometryView->mpMaterial), material);
        geometryView->mpMaterial = material;
        geometryView->mbInitialized = 1;
    }
    weatherInitializeRenderer(gabi::ea(&this->mRenderer.mPrefix), (u32)1, (u32)0);
    for (u32 i = 0; i < 100; ++i)
        weatherSetTextureMode(gabi::ea(&this->mTextures[i]), (u32)0);
    (this->mCloudPhase = static_cast<float>(weatherZero.value));
}

/* 0257A42C */
u32 d_kankyo_wether_cpp::dKankyo_vrkumo_Packet::construct() {
    u32 object = gabi::ea(this);

    if (!object)
        object = weatherAllocateRegister((u32)0xC5E20);
    if (!object)
        return 0;
    auto *packetView = gabi::at<dKankyo_vrkumo_Packet>(object);
    weatherConstructPacket(object);
    (packetView->mPacket.__vtbl = gabi::ea(&weatherVrkumoVtable));
    weatherClear(gabi::ea(&packetView->mpCloudTextures[0]), (u32)12);
    weatherClear(gabi::ea(&packetView->mInst[0][0]), (u32)0x1130);
    float zero = static_cast<float>(weatherZero.value);
    (packetView->mpMaterial = 0);
    (packetView->mCloudPhase = zero);
    weatherConstructArray(gabi::ea(&packetView->mGeometry[0]), (u32)2, (u32)0x574F8,
                          (u32)0x02583B74);
    u32 renderer = gabi::ea(&packetView->mRenderer.mPrefix);
    auto *rendererView = gabi::at<WeatherRenderer>(renderer);
    weatherConstructRenderer(renderer);
    weatherConstructTexture(renderer + 0xC);
    (rendererView->mPrefix.mTexture.__vtbl = gabi::ea(&weatherSmallTextureVtable));
    weatherClear(renderer + 0x80, (u32)0x34);
    if (renderer + 0x80 == 0)
        weatherAllocateDiscard((u32)0x30);
    weatherConstructTexture(renderer + 0xB4);
    (rendererView->mExtendedTexture.__vtbl = gabi::ea(&weatherTextureVtable));
    weatherClear(renderer + 0x128, (u32)0x2F0);
    construct_texture_constants(renderer, 0x128);
    (rendererView->mStatus = 0);
    weatherConstructArray(gabi::ea(&packetView->mTextures[0]), (u32)100, (u32)0x370,
                          (u32)0x02583210);
    weatherConstructArray(gabi::ea(&packetView->mTextureDescriptors[0][0]), (u32)3, (u32)0x90,
                          (u32)0x027BE6B8);
    weatherConstructArray(gabi::ea(&packetView->mTextureBindings[0][0]), (u32)3, (u32)0x198,
                          (u32)0x027BDF7C);
    weatherInitializeVrkumoGeometry(object);
    return object;
}

/* 0257A7A8 */
void d_kankyo_wether_cpp::dKankyo_vrkumo_Packet::refreshTextures() {

    weatherLockTextures(static_cast<u32>(weatherTextureManager));
    for (u32 i = 0; i < 3; ++i) {
        u32 resource = static_cast<u32>(this->mpCloudTextures[i]);
        if (!resource)
            continue;
        u32 source = gabi::ea(&this->mTextureDescriptors[i][0]);
        u32 target = gabi::ea(&this->mTextureBindings[i][0]);
        auto *sourceView = gabi::at<WeatherTextureDescriptor>(source);
        auto *targetView = gabi::at<WeatherTextureBinding>(target);
        weatherImportTexture(source, static_cast<u32>(gabi::at<WeatherTextureResource>(resource)->mpTexture));
        bool same = static_cast<u32>(targetView->mDescriptor.mWidth) ==
                        static_cast<u32>(sourceView->mWidth) &&
                    static_cast<u32>(targetView->mDescriptor.mHeight) ==
                        static_cast<u32>(sourceView->mHeight) &&
                    static_cast<u32>(targetView->mDescriptor.mDepth) ==
                        static_cast<u32>(sourceView->mDepth) &&
                    static_cast<u32>(targetView->mDescriptor.mMipCount) ==
                        static_cast<u32>(sourceView->mMipCount) &&
                    static_cast<u32>(targetView->mDescriptor.mFormat) ==
                        static_cast<u32>(sourceView->mFormat) &&
                    static_cast<u32>(targetView->mDescriptor.mSampleMode) ==
                        static_cast<u32>(sourceView->mSampleMode) &&
                    static_cast<u32>(targetView->mDescriptor.mAlignment) ==
                        static_cast<u32>(sourceView->mAlignment) &&
                    static_cast<u32>(targetView->mDescriptor.mTileMode) ==
                        static_cast<u32>(sourceView->mTileMode) &&
                    static_cast<u32>(targetView->mDescriptor.mUsage) ==
                        static_cast<u32>(sourceView->mUsage);
        if (!same) {
            weatherCopyTextureDescriptor(target, source);
            u8 flags = static_cast<u8>(targetView->mFlags);
            (targetView->mPixelStateDirty[0] = 1);
            (targetView->mPixelStateDirty[1] = 1);
            (targetView->mPixelStateDirty[2] = 1);
            (targetView->mFlags = flags | 4);
        } else {
            u8 flags = static_cast<u8>(targetView->mFlags);
            u32 data = static_cast<u32>(sourceView->mpPixelData),
                size = static_cast<u32>(sourceView->mPixelBytes);
            (targetView->mDescriptor.mpPixelData = data);
            (targetView->mFlags = flags | 4);
            (targetView->mCachedPixelBytes = size);
            (targetView->mDescriptor.mPixelBytes = size);
            (targetView->mPixelStateDirty[1] = 1);
            (targetView->mpCachedPixelData = data);
            (targetView->mPixelStateDirty[2] = 1);
            (targetView->mPixelStateDirty[0] = 1);
        }
    }
    weatherUnlockTextures(static_cast<u32>(weatherTextureManager));
}

/* 0257A920 */
u32 d_kankyo_wether_cpp::dKankyo_wave_Packet::construct() {
    u32 object = gabi::ea(this);

    if (!object)
        object = weatherAllocateRegister((u32)0x9DCC0);
    if (!object)
        return 0;
    auto *packetView = gabi::at<dKankyo_wave_Packet>(object);
    weatherConstructPacket(object);
    (packetView->mPacket.__vtbl = gabi::ea(&weatherWaveVtable));
    weatherClear(gabi::ea(&packetView->mpTextures[0]), (u32)8);
    weatherClear(gabi::ea(&packetView->mEff[0]), (u32)0x41A0);
    float zero = static_cast<float>(weatherZero.value);
    (packetView->mSkewWidth = zero);
    (packetView->mpMaterial = 0);
    (packetView->mSkewDir = zero);
    weatherConstructArray(gabi::ea(&packetView->mGeometry[0].mGeometry[0].mpVertices), (u32)300,
                          (u32)0x4C0, (u32)0x02583C78);
    weatherConstructArray(gabi::ea(&packetView->mTextures[0]), (u32)300, (u32)0x370,
                          (u32)0x02583210);
    weatherConstructRenderer(gabi::ea(&packetView->mRenderer));
    weatherConstructTexture(gabi::ea(&packetView->mRenderer.mTexture));
    (packetView->mRenderer.mTexture.__vtbl = gabi::ea(&weatherSmallTextureVtable));
    weatherClear(gabi::ea(&packetView->mRenderer.mTexture.mParameters), (u32)0x34);
    if (gabi::ea(&packetView->mRenderer.mTexture.mParameters) == 0)
        weatherAllocateDiscard((u32)0x30);
    weatherConstructTextureSet(gabi::ea(&packetView->mTextureSet.mState));
    weatherClear(gabi::ea(&packetView->mActive[0]), (u32)300);
    return object;
}

/* 0257AA4C */
void d_kankyo_wether_cpp::dKankyo_wave_Packet::initializeGeometry() {
    u32 packet = gabi::ea(this);

    (this->mpMaterial = lookup_weather_material(packet, gabi::guest_addr("usonami")));
    for (u32 i = 0; i < 300; ++i)
        prepare_weather_pair(gabi::ea(&this->mGeometry[i].mGeometry[0].mpVertices),
                             static_cast<u32>(this->mpMaterial), gabi::ea(&weatherWaveAttributes));
    gabi::Local<WeatherTextureDescriptor> texture;
    weatherConstructTextureDescriptor(texture.a);
    u32 resource = static_cast<u32>(this->mpTextures[0]);
    weatherLockTextures(static_cast<u32>(weatherTextureManager));
    weatherImportTexture(texture.a, resource);
    weatherUnlockTextures(static_cast<u32>(weatherTextureManager));
    weatherSetTexture(gabi::ea(&this->mTextureSet.mState), (u32)0, texture.a, (u32)0);
    for (u32 i = 0; i < 300; ++i)
        weatherSetTextureMode(gabi::ea(&this->mTextures[i]), (u32)0);
    weatherSetRendererMode(gabi::ea(&this->mRenderer), (u32)1, (u32)0);
    weatherSetTextureMode(gabi::ea(&this->mRenderer.mTexture), (u32)0);
}

/* 0257E7F4 */
void d_kankyo_wether_cpp::dKankyo_sun_Packet::draw() {
    u32 packet = gabi::ea(this);

    weatherdKyr_drawSun(gabi::ea(&weatherViewMatrix), packet + 0x98, packet + 0xD4);
}

/* 0257E80C */
void d_kankyo_wether_cpp::dKankyo_sun_Packet::destroy(u32 flags) {
    u32 object = gabi::ea(this);

    if (!object)
        return;
    weatherDestroyArray(gabi::ea(&this->mTextureSet[0].mState), (u32)6, (u32)0x4C, (u32)0x027FE4B0,
                        (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&this->mRenderer[0].mPrefix), (u32)4, (u32)0x41C, (u32)0x02584128,
                        (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&this->mGeometry[0].mGeometry[0].mpVertices), (u32)4, (u32)0x4C0,
                        (u32)0x0258401C, (u32)0, (u32)0);
    weatherDestroyPacket(object, (u32)0);
    if (flags & 1)
        weatherFree(object);
}

/* 0257E8C0 */
void d_kankyo_wether_cpp::dKankyo_sun_Packet::setColors(u32 index, u32 colorA, u32 colorB) {
    gabi::Local<ColorFloat> first, second;
    u32 destination = gabi::ea(&this->mRenderer[index].mExtendedTexture.mParameters.mPrimaryColor);
    auto *primary = gabi::at<ColorFloat>(destination);
    auto *secondary = gabi::at<ColorFloat>(destination + sizeof(ColorFloat));
    weatherNormalizeColor(first.a, colorA);
    for (u32 i = 0; i < 4; ++i)
        primary->words[i] = gabi::at<ColorFloat>(first.a)->words[i];
    weatherNormalizeColor(second.a, colorB);
    for (u32 i = 0; i < 4; ++i)
        secondary->words[i] = gabi::at<ColorFloat>(second.a)->words[i];
}

/* 0257E9DC */
void d_kankyo_wether_cpp::dKankyo_sun_Packet::render(u32 firstTexture, u32 enabled, u32 allRows) {

    gabi::Local<WeatherMatrix> matrix;
    for (u32 i = 0; i < 4; ++i) {
        weatherCopyMatrix(matrix.a, gabi::ea(&weatherViewMatrix));
        u32 renderer = gabi::ea(&this->mRenderer[i].mPrefix);
        weatherSetMatrices(renderer, (u32)0, matrix.a, gabi::ea(&weatherProjectionMatrix),
                           static_cast<u32>(weatherCamera) + 0x240);
        weatherPrepareRenderer(renderer, (u32)0);
        u32 pair = gabi::ea(&this->mGeometry[i].mGeometry[0].mpVertices);
        u32 element =
            pair + static_cast<u32>(gabi::at<WeatherGeometryPair>(pair)->mBufferIndex) * 0x254;
        auto *elementView = gabi::at<WeatherGeometry>(element);
        weatherUploadVertices(element + 4, (u32)0,
                              static_cast<u32>(elementView->mVertexBuffer.mUploadSize));
        (gabi::at<WeatherGeometryPair>(pair)->mBufferIndex =
             static_cast<u32>(gabi::at<WeatherGeometryPair>(pair)->mBufferIndex) == 0 ? 1 : 0);
    }
    bind_weather_shader(gabi::ea(&this->mpMaterial));
    gabi::Local<WeatherRenderState> state;
    weatherConstructRenderState(state.a);
    configure_cloud_render_state(state.a, static_cast<float>(weatherZero.value), false);
    weatherApplyRenderState(state.a);
    for (u32 i = 0; i < 4; ++i) {
        if (i < 2 ? !enabled : !allRows)
            continue;
        weatherBindRenderer(gabi::ea(&this->mRenderer[i].mPrefix),
                            static_cast<u32>(this->mpMaterial), (u32)0);
        u32 texture = i == 0 ? firstTexture : i == 2 ? 4 : 5;
        weatherBindTextureSet(gabi::ea(&this->mTextureSet[texture].mState),
                              static_cast<u32>(this->mpMaterial));
        u32 pair = gabi::ea(&this->mGeometry[i].mGeometry[0].mpVertices);
        u32 side = static_cast<u32>(gabi::at<WeatherGeometryPair>(pair)->mBufferIndex) == 0 ? 1 : 0;
        if (static_cast<u32>(gabi::at<WeatherGeometryPair>(pair)->mpMaterial))
            weatherBindGeometryMaterial(pair + side * 0x254 + 0x158);
        weatherDrawIndexed(static_cast<u32>(weatherQuadPrimitive), (u32)4,
                           static_cast<u32>(weatherQuadIndexType),
                           static_cast<u32>(weatherQuadIndices), (u32)0, (u32)1);
    }
    weatherApplyRenderState(gabi::ea(&weatherDefaultRenderState));
}

/* 0257ED2C */
void d_kankyo_wether_cpp::dKankyo_sunlenz_Packet::draw() {
    u32 packet = gabi::ea(this);

    weatherdKyr_drawLenzflare(gabi::ea(&weatherViewMatrix), packet + 0xA4, packet + 0x98);
}

/* 0257ED44 */
void d_kankyo_wether_cpp::dKankyo_sunlenz_Packet::destroy(u32 flags) {
    u32 object = gabi::ea(this);

    if (!object)
        return;
    weatherDestroyTexture(gabi::ea(&this->mCloudRenderer.mExtendedTexture), (u32)0);
    weatherDestroyTexture(gabi::ea(&this->mCloudRenderer.mPrefix.mTexture), (u32)0);
    weatherDestroyRenderer(gabi::ea(&this->mCloudRenderer.mPrefix), (u32)2);
    u32 geometry = gabi::ea(&this->mCloudGeometry.mGeometry[0].mpVertices);
    if (geometry) {
        for (u32 i = 0; i < 16; ++i) {
            release_geometry_element(geometry + i * 0x4A8, 0);
            release_geometry_element(geometry + i * 0x4A8, 0x254);
        }
        (gabi::at<WeatherCloudGeometry>(geometry)->mpMaterial = 0);
        weatherDestroyArray(geometry, (u32)32, (u32)0x254, (u32)0x02584194, (u32)0, (u32)0);
    }
    weatherDestroyArray(gabi::ea(&this->mTextureSet[0].mState), (u32)3, (u32)0x4C, (u32)0x027FE4B0,
                        (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&this->mRenderer[0].mPrefix), (u32)9, (u32)0x41C, (u32)0x02584128,
                        (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&this->mGeometry[0].mGeometry[0].mpVertices), (u32)9, (u32)0x4C0,
                        (u32)0x0258401C, (u32)0, (u32)0);
    weatherDestroyPacket(object, (u32)0);
    if (flags & 1)
        weatherFree(object);
}

/* 0257EF00 */
void d_kankyo_wether_cpp::dKankyo_sunlenz_Packet::render() {

    gabi::Local<WeatherRenderState> state;
    weatherConstructRenderState(state.a);
    float zero = static_cast<float>(weatherZero.value);
    configure_cloud_render_state(state.a, zero);
    weatherApplyRenderState(state.a);
    gabi::Local<WeatherMatrix> matrix;
    for (u32 i = 0; i < 9; ++i) {
        weatherCopyMatrix(matrix.a, gabi::ea(&weatherViewMatrix));
        u32 renderer = gabi::ea(&this->mRenderer[i].mPrefix);
        weatherSetMatrices(renderer, (u32)0, matrix.a, gabi::ea(&weatherProjectionMatrix),
                           static_cast<u32>(weatherCamera) + 0x240);
        weatherPrepareRenderer(renderer, (u32)0);
    }
    u32 material = bind_weather_shader(gabi::ea(&this->mpMaterial));
    for (u32 i = 0; i < 9; ++i) {
        if (i)
            material = static_cast<u32>(this->mpMaterial);
        u32 texture = i < 2 ? 0 : i == 2 ? 2 : 1;
        weatherBindTextureSet(gabi::ea(&this->mTextureSet[texture].mState), material);
        weatherBindRenderer(gabi::ea(&this->mRenderer[i].mPrefix),
                            static_cast<u32>(this->mpMaterial), (u32)0);
        u32 pair = gabi::ea(&this->mGeometry[i].mGeometry[0].mpVertices);
        auto *pairView = gabi::at<WeatherGeometryPair>(pair);
        u32 side = static_cast<u32>(pairView->mBufferIndex) == 0 ? 1 : 0;
        if (static_cast<u32>(pairView->mpMaterial))
            weatherBindGeometryMaterial(pair + side * 0x254 + 0x158);
        weatherDrawIndexed(static_cast<u32>(weatherQuadPrimitive), (u32)4,
                           static_cast<u32>(weatherQuadIndexType),
                           static_cast<u32>(weatherQuadIndices), (u32)0, (u32)1);
    }
    weatherConstructRenderState(state.a);
    configure_cloud_render_state(state.a, zero);
    weatherApplyRenderState(state.a);
    gabi::Local<WeatherMatrix> secondMatrix;
    weatherCopyMatrix(secondMatrix.a, gabi::ea(&weatherViewMatrix));
    weatherSetMatrices(gabi::ea(&this->mCloudRenderer.mPrefix), (u32)0, secondMatrix.a,
                       gabi::ea(&weatherProjectionMatrix), static_cast<u32>(weatherCamera) + 0x240);
    weatherPrepareRenderer(gabi::ea(&this->mCloudRenderer.mPrefix), (u32)0);
    u32 geometry = gabi::ea(&this->mCloudGeometry.mGeometry[0].mpVertices);
    u32 head = static_cast<u32>(gabi::at<WeatherCloudGeometry>(geometry)->mBufferIndex);
    for (u32 i = 0; i < 16; ++i) {
        u32 element = geometry + head * 0x254 + i * 0x4A8;
        auto *elementView = gabi::at<WeatherGeometry>(element);
        weatherUploadVertices(element + 4, (u32)0,
                              static_cast<u32>(elementView->mVertexBuffer.mUploadSize));
    }
    (gabi::at<WeatherCloudGeometry>(geometry)->mBufferIndex =
         static_cast<u32>(gabi::at<WeatherCloudGeometry>(geometry)->mBufferIndex) == 0 ? 1 : 0);
    material = bind_weather_shader(gabi::ea(&this->mpCloudMaterial));
    weatherBindRenderer(gabi::ea(&this->mCloudRenderer.mPrefix), material, (u32)0);
    u32 side =
        static_cast<u32>(gabi::at<WeatherCloudGeometry>(geometry)->mBufferIndex) == 0 ? 1 : 0;
    for (u32 i = 0; i < 16; ++i) {
        if (static_cast<u32>(gabi::at<WeatherCloudGeometry>(geometry)->mpMaterial))
            weatherBindGeometryMaterial(geometry + side * 0x254 + i * 0x4A8 + 0x158);
        weatherDrawIndexed(static_cast<u32>(weatherTrianglePrimitive), (u32)3,
                           static_cast<u32>(weatherTriangleIndexType),
                           static_cast<u32>(weatherTriangleIndices), (u32)0, (u32)1);
    }
    weatherApplyRenderState(gabi::ea(&weatherDefaultRenderState));
}

/* 0257F32C */
void d_kankyo_wether_cpp::dKankyo_rain_Packet::draw() {
    u32 packet = gabi::ea(this);

    weatherdKyr_drawSibuki(gabi::ea(&weatherViewMatrix), packet + 0x98);
    weatherdKyr_drawRain(gabi::ea(&weatherViewMatrix), packet + 0x98);
}

/* 0257F378 */
void d_kankyo_wether_cpp::dKankyo_rain_Packet::destroy(u32 flags) {
    u32 object = gabi::ea(this);

    if (!object)
        return;
    weatherDestroyArray(gabi::ea(&this->mTextureSet[0].mState), (u32)2, (u32)0x4C, (u32)0x027FE4B0,
                        (u32)0, (u32)0);
    weatherDestroyTexture(gabi::ea(&this->mRenderer.mTexture), (u32)0);
    weatherDestroyRenderer(gabi::ea(&this->mRenderer), (u32)2);
    weatherDestroyTexture(gabi::ea(&this->mRainTexture), (u32)0);
    weatherDestroyArray(gabi::ea(&this->mSplashTextures[0]), (u32)250, (u32)0x370, (u32)0x025841F4,
                        (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&this->mRainGeometry[0].mGeometry[0].mpVertices), (u32)250,
                        (u32)0x4C0, (u32)0x025842A8, (u32)0, (u32)0);
    weatherDestroyArray(gabi::ea(&this->mSplashGeometry[0].mGeometry[0].mpVertices), (u32)1000,
                        (u32)0x4C0, (u32)0x025842A8, (u32)0, (u32)0);
    weatherDestroyPacket(object, (u32)0);
    if (flags & 1)
        weatherFree(object);
}

using namespace d_kankyo_wether_cpp;

// clang-format off
/* WWHD glue
ext 027BE2B0 void weatherDestroySampler(u32,u32) callback
data weatherDefaultRenderState 104B474C
data weatherSnowVtable 100502B4
data weatherStarVtable 100502E4
data weatherPoisonVtable 10050314
data weatherStringVtable 1004FDD0
data weatherHousiAttributes 10050200
data weatherCloudAttributes 10050208
data weatherVrkumoAttributes 10050210
data weatherWaveAttributes 10050218
data weatherStarAttributes 10050220
data weatherSunVtable 10050224
data weatherSunlenzVtable 10050254
data weatherRainVtable 10050284
data weatherHousiVtable 10050344
data weatherCloudVtable 10050374
data weatherWaveVtable 100503A4
data weatherVrkumoVtable 100503D4
data weatherSmallTextureVtable 1016EF84
data weatherTextureVtable 1016EFB4
data weatherBufferedTextureVtable 1016EFE4
data weatherZeroVector 101FFBA8
data weatherShaderContext 104B45C0
data weatherViewMatrix 104B45F8
data weatherProjectionMatrix 104B470C
data weatherStarPalette 10050060
data weatherSunGanonStage 1004FD70
data weatherSunTowerStage 1004FD78
data weatherSunArchiveName 1004FD80
data weatherStarGanonStage 1004FD88
data weatherStarTowerStage 1004FD90
data weatherStarNameStage 1004FD98
data weatherHousiArchiveName 1004FDA0
data weatherSunArchiveFile 1004FF30
data weatherHousiArchiveFile 1004FF60
str 10050080 "housi_bloom"
str 1004FE84 "WINDEFF_SET" renamed
str 10050108 "Name" in 0257ACAC
str 1005008C "cloud_shadow"
str 1004FE10 "txa_tuki_as"
str 1004FF10 "dKankyo_sun_Packet" renamed
str 1004FE9C "txa_taiyo"
str 1004FE1C "txa_tuki_bs"
str 1005009C "cloud_draw"
str 1004FFA0 "vr_box.bmd"
str 1004FE28 "txa_tuki_cs"
str 1004FEA8 "tx_snow01"
str 100500A8 "usonami"
str 1004FFAC "cloud_01.bti"
str 1005002C "sun"
str 10050030 "lenzflare"
str 1004FD30 "Adanmae" in 0257ACAC
str 1004FE34 "txa_tuki_ds"
str 1004FEB4 "txa_ring_a_32hafe"
str 10050138 "Name" in 0257CE00
str 1004FDB8 "Name" in 0257C5B0
str 1004FD38 "M_DragB" in 0257ACAC
str 1005003C "sunlight"
str 1004FFBC "cloud_02.bti"
str 1004FE40 "dKankyo_rain_Packet" renamed
str 1004FF40 "dKankyo_housi_Packet" renamed
str 1004FDC0 "GTower" in 0257C5B0
str 1004FFCC "cloud_03.bti"
str 1004FD4C "sea" in 0257C5B0
str 1004FD50 "M_Dai" in 0257ACAC
str 1004FED8 "dKankyo_vrkumo_Packet" renamed
str 10050058 "star"
str 1004FD58 "Stage" in 0257C5B0
str 1004FFDC "cloudtx_01.bti"
str 1004FE5C "ak_houshi00"
str 1004FD60 "GTower" in 0257836C
str 10050160 "Hyrule" in 0257D398
str 1004FDE8 "txa_lens_32half"
str 1004FD68 "Hyrule" in 0257ACAC
str 10050168 "ADMumi" in 0257D398
str 1004FFEC "cloudtx_02.bti"
str 1004FFFC "cloudtx_03.bti"
data wind_table 10050174
data weatherSinCosTable 104A44F8
data weatherNegativeOne 1005001C
data weatherZero 10050020
data weatherWindNoticeDisabled 10050024
data weatherWindlineScale 10050028
data weatherColorScale 10050048
data weatherSunlenzDepthLimit 100500B0
data weatherHeavyRainThreshold 100500C0
data weatherOne 100500C4
data weatherStarDawnScale 100500C8
data weatherNameNightStart 100500CC
data weatherNameNightEnd 100500D0
data weatherNameDuskStart 100500D4
data weatherNameDuskScale 100500D8
data weatherNameDawnEnd 100500DC
data weatherNightStart 100500E0
data weatherNightEnd 100500E4
data weatherDuskStart 100500E8
data weatherDuskScale 100500EC
data weatherDawnEnd 100500F0
data weatherStarFadeThreshold 100500F4
data weatherWindDamping 100500F8
data weatherStarStep 100500FC
data weatherStarMinimumStep 10050100
data weatherStarCountScale 10050104
data weatherSeaCloudCenterX 10050110
data weatherSeaCloudCenterY 10050114
data weatherSeaCloudCenterZ 10050118
data weatherCloudGrowStep 1005011C
data weatherCloudGrowMinimumStep 10050120
data weatherCloudFadeDamping 10050124
data weatherCloudFadeStep 10050128
data weatherCloudFadeMinimumStep 1005012C
data weatherSeaCloudRadius 10050130
data weatherCloudCountScale 10050134
data weatherSquallRadiusScale 10050140
data weatherWindMinimumStep 10050144
data weatherSquallDamping 10050148
data weatherWindLevelLow 1005014C
data weatherWindLevelMedium 10050150
data weatherWindLevelHigh 10050154
data weatherWindDirectionStep 10050158
data weatherWindPowerMinimumStep 1005015C
data weatherPointWindScale 10050170
data weatherTextureOne 1014517C
data weatherTextureZero 10145180
data weatherArchiveManager 101F4F7C
data weatherSavePointer 101F84DC
data weatherTextureManager 101F8B18
data weatherVertexAllocator 101F8B4C
data weatherSeaCloudCenterInitialized 10477598
data weatherSeaCloudX 104775B0
data weatherSeaCloudY 104775B4
data weatherSeaCloudZ 104775B8
data weatherStayRoomNo 1047E6C8
data weatherOpaqueDrawBuffer 104B4634
data weatherTranslucentDrawBuffer 104B4638
data weatherCamera 104B4708
data weatherTriangleIndexType 104B4A84
data weatherTrianglePrimitive 104B4A88
data weatherTriangleIndices 104B4A8C
data weatherQuadIndexType 104B4CFC
data weatherQuadPrimitive 104B4D00
data weatherQuadIndices 104B4D04
ext 0200ECD4 void weatherAddCalc(u32,f32,f32,f32,f32)
ext 020195B0 s32 weatherAtan2Signed(f32,f32)
ext 020195B0 u32 weatherAtan2Register(f32,f32)
ext 0201AD78 void weatherAddVectors(u32,u32,u32)
ext 0201AE48 void weatherScaleVector(u32,u32,f32)
ext 0201AFD8 u32 weatherVectorNotEqual(u32,u32)
ext 0201B12C void weatherNormalizeVector(u32,u32)
ext 025200D4 u32 weatherGetPlayRegister()
ext 025200D4 void weatherGetPlayDiscard()
ext 0252447C u32 weatherGetStageResource(u32,u32)
ext 0252F3B0 void weatherdDlst_list_c__entryZSortXluDrawList(u32,u32,u32,u32)
ext 02555D0C u32 weatherGetEnvironment()
ext 02560E08 f32 weatherGetDaytime(u32)
ext 02563F64 void weatherdKyr_get_vectle_calc(u32,u32,u32)
ext 02564184 void weatherdKyr_wind_init()
ext 025642AC void weatherdKyr_wind_move()
ext 02565B70 void weatherdKyr_lenzflare_move()
ext 02565E0C void weatherdKyr_sun_move()
ext 0256670C void weatherdKyr_rain_init()
ext 02566B88 void weatherdKyr_rain_move()
ext 02567F68 void weatherdKyr_housi_move()
ext 025688C4 void weatherCall_dKyr_snow_init()
ext 02568BD4 void weatherdKyr_snow_move()
ext 02569408 void weatherdKyr_kazanbai_move()
ext 0256A25C void weatherdKyr_star_init()
ext 0256A388 void weatherCall_dKyr_star_move()
ext 0256CA54 void weatherpoison_move()
ext 0256DC04 void weatherpoison_init()
ext 0256DDF8 void weathervrkumo_move()
ext 0256F1C4 void weatherdKyr_drawSun(u32,u32,u32)
ext 02570210 void weatherdKyr_drawLenzflare(u32,u32,u32)
ext 025712A8 void weatherdKyr_drawRain(u32,u32)
ext 02571B1C void weatherdKyr_drawSibuki(u32,u32)
ext 02577110 void weatherCall_dKyr_thunder_init()
ext 02577138 void weatherdKyr_thunder_move()
ext 02577710 void weatherCall_dKyw_setDrawPacketList(u32,u32)
ext 02577754 void weatherCall_dKyw_setDrawPacketListSky(u32,u32)
ext 025777F0 void weatherCall_dKyw_drawSun(u32)
ext 02577828 void weatherCall_dKyw_drawSunlenz(u32)
ext 02577864 void weatherCall_dKyw_drawRain(u32)
ext 0257789C void weatherCall_dKyw_drawSnow(u32)
ext 025778D4 void weatherCall_dKyw_drawStar(u32)
ext 0257790C void weatherCall_dKyw_drawPoison(u32)
ext 02577974 void weatherCall_dKyw_drawHousi(u32)
ext 025779AC void weatherCall_dKyw_drawCloud(u32)
ext 025779E4 void weatherCall_dKyw_drawVrkumo(u32)
ext 02577A80 void weatherCall_dKyw_drawWave(u32)
ext 02577B5C void weatherCall_dKyw_wind_init()
ext 02577E38 void weatherDeleteRainPacket(u32)
ext 02577E9C void weatherDeleteStarPacket(u32)
ext 02577F00 void weatherDeleteHousiPacket(u32)
ext 02577F64 void weatherDeleteCloudPacket(u32)
ext 02577FC8 void weatherDeleteWavePacket(u32)
ext 02578348 f32 weatherdKyw_get_wind_pow()
ext 025786F0 u32 weatherdKyw_wether_init(u32)
ext 025787D4 u32 weatherConstructSunlenzPacket(u32)
ext 02578BC0 void weatherInitializeSunGeometry(u32,u32)
ext 02578E60 void weatherInitializeSunlenzGeometry(u32)
ext 02579280 u32 weatherConstructRainPacket(u32)
ext 0257963C void weatherNormalizeColor(u32,u32)
ext 025796F0 void weatherInitializeStarGeometry(u32)
ext 02579934 u32 weatherConstructHousiPacket(u32)
ext 02579A68 void weatherInitializeHousiGeometry(u32)
ext 0257A0E8 void weatherInitializeVrkumoGeometry(u32)
ext 0257A42C u32 weatherConstructVrkumoPacket(u32)
ext 0257A7A8 void weatherRefreshVrkumoTextures(u32)
ext 0257D1B8 void weatherdKyw_pntwind_cut(u32)
ext 0257DAA8 u32 weatherdKyw_get_wind_vec()
ext 0257DBDC void weatherpntwind_set(u32)
ext 0257DC90 void weatherdKyw_pntwind_set(u32)
ext 0257DE68 void weatherdKyw_pntwind_get_info(u32,u32,u32)
ext 0257E93C void weatherCopyMatrix(u32,u32)
ext 02582BB8 u32 weatherConstructSafeString(u32,u32)
ext 02582C08 u32 weatherSafeStringEqual(u32,u32)
ext 02582CE8 void weatherDestroySafeString(u32,u32)
ext 02583678 void weatherwether_move_moya()
ext 02583D00 void weatherwether_move_wave()
ext 025C11DC u32 weatherdStage_roomControl_c__getStatusRoomDtRegister(u32,s32)
ext 025C11DC u32 weatherdStage_roomControl_c__getStatusRoomDtRegister78(u32,u32)
ext 025E1988 void weatherStartSound(u32)
ext 025E200C void weatherPlayRainSound(u32)
ext 025F0148 void weatherDelete(u32)
ext 025F01D8 void weatherBeginHeap(u32,u32,u32,u32)
ext 025F0270 void weatherEndHeap(u32,u32)
ext 025F02D0 void weatherAbortHeap(u32)
ext 026124B0 u32 weatherGetArchiveResource(u32,u32,u32,u32)
ext 0273AD10 u32 weatherAllocateRegister(u32)
ext 0273AD10 void weatherAllocateDiscard(u32)
ext 0273AE48 u32 weatherAllocateAligned(u32,u32)
ext 0273AF40 void weatherFree(u32)
ext 0274FBF8 void weatherLockTextures(u32)
ext 0274FCCC void weatherUnlockTextures(u32)
ext 02750250 void weatherConstructRenderState(u32)
ext 02750370 void weatherApplyRenderState(u32)
ext 02755FEC u32 weatherAllocateVertices(u32,u32)
ext 02756140 u32 weatherGetAllocator(u32,u32)
ext 02773798 void weatherImportTexture(u32,u32)
ext 027B5BD8 void weatherConstructVertexBuffer(u32)
ext 027B5CBC void weatherDestroyVertexBuffer(u32,u32)
ext 027B5E94 void weatherUploadVertices(u32,u32,u32)
ext 027B90AC u32 weatherFindMaterial(u32,u32)
ext 027B9F68 void weatherBindProgram(u32)
ext 027BB7CC void weatherUploadShader(u32)
ext 027BB9E0 void weatherPrepareShader(u32,u32)
ext 027BDEB4 void weatherCopyTextureDescriptor(u32,u32)
ext 027BE6B8 void weatherConstructTextureDescriptor(u32)
ext 027BF734 void weatherConstructTextureBinding(u32)
ext 027BF7E8 void weatherInitializeTextureBinding(u32)
ext 027BF880 void weatherDestroyTextureBinding(u32,u32)
ext 027BFE5C void weatherBindGeometryMaterial(u32)
ext 027DFA24 u32 weatherGetArchiveTexture(u32,u32)
ext 027E2DC0 u32 weatherGetArchive(u32)
ext 027F0E04 void weatherJ3DDrawBuffer__entryImm(u32,u32,u32)
ext 027F1278 void weatherConstructPacket(u32)
ext 027F13DC void weatherDestroyPacket(u32,u32)
ext 027F29D4 u32 weatherGetShaderCache(u32)
ext 027FB40C void weatherConstructTexture(u32)
ext 027FB528 void weatherDestroyTexture(u32,u32)
ext 027FB5D4 void weatherSetTextureMode(u32,u32)
ext 027FD6F4 void weatherConstructRenderer(u32)
ext 027FD764 void weatherDestroyRenderer(u32,u32)
ext 027FD838 void weatherSetRendererMode(u32,u32,u32)
ext 027FDA54 void weatherSetMatrices(u32,u32,u32,u32,u32)
ext 027FE084 void weatherInitializeRenderer(u32,u32,u32)
ext 027FE0DC void weatherPrepareRenderer(u32,u32)
ext 027FE118 void weatherBindRenderer(u32,u32,u32)
ext 027FE344 void weatherConstructTextureSet(u32)
ext 027FE458 void weatherInitializeTextureSet(u32)
ext 027FE500 void weatherSetTexture(u32,u32,u32,u32)
ext 027FE5B8 void weatherBindTextureSet(u32,u32)
ext 027FE640 u32 weatherGetTexture(u32,u32)
ext 027FF478 void weatherInitializeVertices(u32,u32,u32,u32)
ext 027FF530 void weatherInitializeGeometryMaterial(u32,u32,u32,u32,u32)
ext 027FFCBC u32 weatherGetMaterialState(u32)
ext 02800B0C void weatherInitializeMaterial(u32,u32,u32)
ext 0281DE68 void weatherJPABaseEmitter__deleteAllParticle(u32,u32)
ext 028E8DD0 f32 weatherVectorSquaredLength(u32)
ext 028E8DE8 f32 weatherVectorSquaredDistance(u32,u32)
ext 028EFFD0 void weatherConstructArray(u32,u32,u32,u32)
ext 028F0164 void weatherDestroyArray(u32,u32,u32,u32,u32,u32)
ext 028F4384 f32 weatherSqrt(f32)
ext 028F521C void weatherClear(u32,u32)
ext C00060E0 void weatherBindShader(u32,u32)
ext C0006178 void weatherDrawIndexed(u32,u32,u32,u32,u32,u32)
ext C00068A8 void weatherSetGeometryUniforms(u32,u32,u32)
ext C0006900 void weatherSetPixelUniforms(u32,u32,u32)
ext C0006A38 void weatherSetVertexUniforms(u32,u32,u32)
ext 0255F84C void weatherSetFog()
ext 0256A448 void weatherMoveWave()
ext 0256BB6C void weatherMoveCloudShadow()
ext 025720D8 void weatherDrawPoison(u32,u32)
ext 025727BC void weatherDrawHousi(u32,u32)
ext 02572FC4 void weatherDrawKazanbai(u32,u32)
ext 02574144 void weatherDrawStar(u32,u32)
ext 02574D38 void weatherDrawWave(u32,u32,u32)
ext 0257556C void weatherDrawCloudShadow(u32,u32)
ext 02575B6C void weatherDrawVrkumo(u32,u32)
ext 02579D2C u32 weatherConstructCloudPacket(u32)
ext 02579E4C void weatherInitializeCloudGeometry(u32)
ext 0257A920 u32 weatherConstructWavePacket(u32)
ext 0257AA4C void weatherInitializeWaveGeometry(u32)
ext 02581060 void weatherDrawStarColor(u32,u32)
ext 0274D458 void weatherScaleColor(u32,u32,f32)
ext 02750534 void weatherApplyBlendState(u32)
ext 027FB678 void weatherUpdateTextureUniforms(u32)
ext 027FC500 void weatherSetPrimaryColor(u32,u32,u32)
ext 027FC650 void weatherSetSecondaryColor(u32,u32,u32)
ext 027FE010 void weatherUpdateRendererUniforms(u32)
ext 027FE4B0 void weatherDestroyTextureSet(u32,u32)
ext 027FFE54 void weatherApplyContextMaterial(u32,u32)
ext 028EAB2C void weatherConstructArchiveFlag(u32)
ext 028ED6F8 void weatherConstructHeaderFlag(u32)
ext 028F026C void weatherRegisterGlobalObject(u32)
str 1004FE68 "dKankyo_wave_Packet" renamed in 02583D00
str 1004FDC8 "Always" in 02583D00
str 1005000C "Always.bfres" in 02583D00
str 1004FEF8 "txa_usonami_256_64mip" in 02583D00
str 1004FDF8 "txa_usonami_m_256_64mip" in 02583D00
str 1004FF70 "dKankyo_cloud_Packet" renamed in 02583678
str 1004FDA8 "Always" in 02583678
str 1004FF90 "Always.bfres" in 02583678
str 1004FEC8 "txa_moya_b_64" in 02583678
str 1004FD40 "M_NewD2" in 02583678
str 1004FDB0 "A_mori" in 02583678
str 100501BC "housi" in 0257F490
str 100501CC "housi" in 02580318
str 100501D4 "housi" in 02581518
str 100501C4 "Adanmae" in 02580318
data weatherRainAttributes 100501E8
data weatherSnowAttributes 100501F0
data weatherPoisonAttributes 100501F8
data weatherInitialRange 100501E0
data weatherRange 10477590
data weatherRuntimeHeader 1047759C
data weatherRuntimeArchive 1047759D
data weatherRuntimeHandles 104775A0
data weatherRangeRegistration 101E9A4C
data weatherHeaderRegistration 101E9A58
data weatherArchiveRegistration 101E9A64
*/
// clang-format on
