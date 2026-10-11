/* d_kankyo: environment / lighting core, part 5 of 6, WWHD.
 * envcolor_init, dKy_setLight(_again), the colour getters/setters, fog parameter upload (HD
 * shader blocks), sound/sword flush/arrow colour hooks, tevstr init, wave/time/moon/stars
 * helpers and dKyr_player_overhead_bg_chk. See d_kankyo.cpp for the TU. */
#include "d/d_kankyo_5.h"

namespace d_kankyo_cpp {
static ColorFogFloatRegisters colorFogFloatRegisters;
static ColorFogShortRegisters colorFogShortRegisters;

static ColorFogStringVtable colorFogStringVtable;
static ColorFogWord colorFogGndCheckVtable;
static ColorFogWord colorFogGndPolyInfoVtable;
static ColorFogWord colorFogGroupPassVtable;
static ColorFogWord colorFogPolyPassVtable;
static ColorFogWord colorFogBasePolyInfoVtable;
static ColorFogWord colorFogBasePolyPassVtable;
static ColorFogWord colorFogBaseGroupPassVtable;

static ColorFogJ3DSystem colorFogJ3D;
static ColorFogLightStatuses colorFogStatuses;
static ColorFogLightStatusPointer colorFogStatusPointer;
static ColorFogLightMask colorFogLightMask;
static ColorFogLightMasks colorFogLightMasks;
static ColorFogFloat colorFogBaseTarget;
static ColorFogFloat colorFogEffectTarget;
static ColorFogVoidPointer colorFogLightManager;
static ColorFogWord colorFogLightFlags;

static ColorFogFloat colorFogOrionX;
static ColorFogFloat colorFogOrionY;
static ColorFogFloat colorFogOrionZ;
static ColorFogFloat colorFogHokutoX;
static ColorFogFloat colorFogHokutoY;
static ColorFogFloat colorFogHokutoZ;
static ColorFogSavePointer colorFogSave;

static LightParameterTablePointer colorFogParameterTable;
static LightParameterIndex colorFogFogIndex;
static LightParameterIndex colorFogLightIndex;
static ColorFogWord colorFogFogGuard;
static ColorFogWord colorFogFogRtti;
static ColorFogWord colorFogLightGuard;
static ColorFogWord colorFogLightRtti;
static ColorFogWord colorFogTypeInfoVtable;

static f32 colorFogOne = 1.0f;
static f32 colorFogZero = 0.0f;
static f32 colorFogNoNextTime = -1.0f;
static f32 colorFogMinimumLightPower = 0.01f;
static f32 colorFogDegreesPerHour = 15.0f;
static f32 colorFogColorMaximum = 255.0f;
static f32 colorFogDayLength = 360.0f;
static f32 colorFogHalfDay = 180.0f;
static f32 colorFogSeaStartTime = 165.0f;
static f32 colorFogDayStart = 97.5f;
static f32 colorFogDayEnd = 292.5f;
static f32 colorFogMinimumWaveScale = 0.001f;
static f32 colorFogWaveSpawnDistance = 20000.0f;
static f32 colorFogEffectFlickerScale = 0.5f;
static f32 colorFogStarLookDistance = 100.0f;
static f32 colorFogFluctuationThreshold = 1000.0f;
static f32 colorFogWaveSpeed = 30.0f;
static f32 colorFogBaseFlickerScale = 0.4f;
static f32 colorFogWeatherChangeDelay = 45.0f;
static f32 colorFogTimeAdvance = 0.02f;
static f32 colorFogEveningStageTime = 240.0f;
static f32 colorFogEndingStageTime = 105.0f;
static f32 colorFogFlickerDivisor = 3.0f;
static f32 colorFogLightPowerDefault = 2.3f;
static f32 colorFogShopLightPower = 1.25f;
static f32 colorFogBaseLightMaxStep = 800000.0f;
static f32 colorFogLookMinimumFov = 20.0f;
static f32 colorFogSoundNearDistance = 1500.0f;
static f32 colorFogWaveSpawnRadius = 22000.0f;
static f32 colorFogWaveScale = 300.0f;
static f32 colorFogWaveCounterSpeed = 1.2f;
static f32 colorFogWaveScaleBottom = 6.0f;
static f32 colorFogOverheadCastHeight = 10000.0f;
static f32 colorFogOverheadClearance = 50.0f;
static f32 colorFogPointLightRadiusScale = 1.5f;
static f32 colorFogPointLightColorScale = 1.0f;
static f32 colorFogPointLightPowerScale = 2.3f;

void dKankyo_DayProc();
s32 dKy_get_dayofweek();
BOOL dKy_checkEventNightStop();

// HD: sead SafeString resolves both operands through this translation unit's vtable.
static inline bool stageNameEquals(const char *literal) {
    gabi::Local<ColorFogString> a;
    a->str.v = gabi::guest_addr(literal);
    a->vtbl = gabi::ea(&colorFogStringVtable);
    ColorFogPlay *play = colorFogPlay();
    gabi::Local<ColorFogString> b;
    b->vtbl = gabi::ea(&colorFogStringVtable);
    b->str = play->mStageName;
    gabi::call_ptr(gabi::at<ColorFogStringVtable>(a->vtbl)->assureTermination, a.get());
    gabi::call_ptr(gabi::at<ColorFogStringVtable>(a->vtbl)->assureTermination, a.get());
    u32 vtable = b->vtbl;
    const char *lhs = a->str;
    gabi::call_ptr(gabi::at<ColorFogStringVtable>(vtable)->assureTermination, b.get());
    const char *rhs = b->str;
    if (lhs == rhs)
        return true;
    for (u32 remaining = 0x40001; remaining != 0; remaining--) {
        u8 left = *reinterpret_cast<const be<u8> *>(lhs);
        u8 right = *reinterpret_cast<const be<u8> *>(rhs);
        if (left != right)
            return false;
        if (left == 0)
            return true;
        lhs++;
        rhs++;
    }
    return false;
}

// HD: the material index selects a shader parameter object; its guarded RTTI initializes lazily.
static inline LightParameterObject *findLightParameter(LightParameterIndex &index, ColorFogWord &guard,
                                                       ColorFogWord &rtti) {
    s32 idx = index.value->get();
    LightParameterTable *table = colorFogParameterTable.value;
    u32 count = table->mEntryCount;
    LightParameterEntry *entries = table->mpEntries;
    bool hasObject;
    if (u32(idx) < count)
        hasObject = entries[u32(idx)].mObjectCount != 0;
    else
        hasObject = entries[0].mObjectCount != 0;
    LightParameterObject *object;
    u32 guardValue;
    if (hasObject) {
        LightParameterEntry *entry = u32(idx) < count ? &entries[u32(idx)] : entries;
        u32 objectCount = table->mObjectCount;
        u32 firstObject = entry->mFirstObject;
        gptr<LightParameterObject> *slot = nullptr;
        if (firstObject < objectCount)
            slot = &table->mpObjects.get()[firstObject];
        guardValue = guard.value;
        // HD: the original reads address zero when an invalid object index selects no slot.
        object = gabi::at<LightParameterObject>(gabi::load<u32>(gabi::ea(slot)));
    } else {
        guardValue = guard.value;
        object = nullptr;
    }
    if (guardValue == 0) {
        guard.value = 1;
        rtti.value = gabi::ea(&colorFogTypeInfoVtable);
    }
    if (object != nullptr) {
        auto *vtable = gabi::at<LightParameterVtable>(object->__vtbl);
        if (!gabi::call_ptr<BOOL>(vtable->checkDerivedRuntimeTypeInfo, object, &rtti))
            object = nullptr;
    }
    return object;
}

static inline void checkFogView() {
    if (colorFogPlay()->mpView.v == 0)
        return;
    f32 zero = colorFogZero;
    if (colorFogPlay()->mpView->mNear < zero)
        return;
    if (colorFogPlay()->mpView->mFar < zero)
        return;
    ColorFogView *nearView = colorFogPlay()->mpView;
    ColorFogView *farView = colorFogPlay()->mpView;
    if (!(nearView->mNear < farView->mFar))
        return;
    // HD retains these accessor calls even though the shader upload does not use near/far.
    colorFogPlay();
    colorFogPlay();
}

// HD: both TEV fog entry points upload the same shader parameter block.
/* inlined into dKy_GxFog_tevstr_set and dKy_GfFog_tevstr_set */
static inline void uploadTevFog(ColorFogTevStr *tev) {
    GXColor fogColor;
    u8 blue = u8(s16(tev->mFogColor.b)), green = u8(s16(tev->mFogColor.g));
    fogColor.b = blue;
    fogColor.g = green;
    u8 red = u8(s16(tev->mFogColor.r)), alpha = u8(s16(tev->mFogColor.a));
    fogColor.r = red;
    fogColor.a = alpha;
    checkFogView();
    LightParameterObject *object = findLightParameter(colorFogFogIndex, colorFogFogGuard, colorFogFogRtti);
    ColorFogRGBA color;
    colorFogConvertColor(&color, &fogColor);
    colorFogWordBits(object->mFogColor[0]) = colorFogWordBits(color.r);
    colorFogWordBits(object->mFogColor[1]) = colorFogWordBits(color.g);
    colorFogWordBits(object->mFogColor[2]) = colorFogWordBits(color.b);
    colorFogWordBits(object->mFogColor[3]) = colorFogWordBits(color.a);
    object->mFogStart = f32(tev->mFogStartZ);
    object->mFogEnd = f32(tev->mFogEndZ);
}

static inline s32 clampColorChannel(s32 v) {
    if (v < 0)
        v = 0;
    if (v > 0xFF)
        v = 0xFF;
    return v;
}

// HD: explicit result storage retains the verified hidden-result allocation and float-copy order.
static inline void constellationPosition(cXyz *result, ColorFogFloat &offsetX, ColorFogFloat &offsetY,
                                         ColorFogFloat &offsetZ, bool subtractZ) {
    ColorFogCamera *camera = colorFogPlay()->mpCamera;
    f32 x = f32(camera->mEye.x) + f32(offsetX.value);
    f32 y = f32(camera->mEye.y) + f32(offsetY.value);
    f32 z = subtractZ ? f32(camera->mEye.z) - f32(offsetZ.value) : f32(camera->mEye.z) + f32(offsetZ.value);
    if (result == nullptr) {
        result = static_cast<cXyz *>(colorFogAllocate(sizeof(cXyz)));
        if (result == nullptr)
            return;
    }
    result->y = y;
    result->x = x;
    result->z = z;
}

void dKy_actor_addcol_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_vrbox_addcol_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_fog_startendz_set(f32 startZ, f32 endZ, f32 ratio);
void plight_init();
void plight_set();
void dKy_setLight_init();
void dKy_Sound_init();
static inline void *stageResource(be<u32> ColorFogStageVtable::*getter) {
    ColorFogStageObject *stage = &colorFogPlay()->mStage;
    ColorFogStageVtable *vtable = gabi::at<ColorFogStageVtable>(stage->__vtbl);
    return gabi::call_ptr<void *>(vtable->*getter, stage);
}

s32 dKy_eflight_influence_id(u32 pos, s32 nth);
void dKy_eflight_influence_pos(u32 ret, s32 idx);
f32 dKy_eflight_influence_power(s32 idx);
f32 dKy_eflight_influence_yuragi(s32 idx);
f32 dKy_eflight_influence_distance(u32 pos, s32 idx);
// The base and effect lights keep separate flicker targets.
static inline void lightFlicker(ColorFogFloat &target, f32 v, f32 fluc, f32 k255, f32 k20, f32 one, f32 scale,
                                bool divideFluctuation) {
    if (v > one)
        v = one;
    v = (one - v) * k20;
    if (v > one)
        v = one;
    f32 f = divideFluctuation ? fluc / f32(colorFogFlickerDivisor) : fluc;
    f32 wave = gabi::fnmsubs(f, v, k255);
    f32 range = k255 - wave;
    f32 random = colorFogRandom(one);
    colorFogAddCalc2(&target.value, gabi::fmadds(range, random, wave), scale, k20);
}

/* 0255DAC0 */
s32 dKy_Create(void *i_this) {
    ColorFogEnvironment *const env0 = colorFogEnv();
    void *palette = stageResource(&ColorFogStageVtable::getPalette);
    void *pselect = stageResource(&ColorFogStageVtable::getPselect);
    void *envr = stageResource(&ColorFogStageVtable::getEnvr);
    void *vrbox = stageResource(&ColorFogStageVtable::getVrbox);
    const f32 zero = colorFogZero;
    for (u32 i = 0; i < 20; i++)
        colorFogFloatRegisters.values[i] = zero; /* REG3_F */
    for (u32 i = 0; i < 10; i++)
        colorFogShortRegisters.values[i] = 0; /* REG3_S */
    colorFogInitWindowPos();                  /* mDoAud_initWindowPos */
    colorFogInitSeaEnvPos();                  /* mDoAud_initSeaEnvPos */
    colorFogInitRiverPos();                   /* mDoAud_initRiverPos */
    dKy_actor_addcol_set(0, 0, 0, zero);
    dKy_fog_startendz_set(zero, zero, zero);
    dKy_vrbox_addcol_set(0, 0, 0, zero);
    colorFogEnv()->field_0xc48 = 0;
    colorFogEnv()->mInitAnimTimer = 1;
    colorFogEnv()->mSchbit = 0;
    colorFogEnv()->mSchbitTimer = 0;
    colorFogEnv()->mbVrboxInvisible = 0;
    colorFogEnv()->mbContrastFlag = 0;
    colorFogEnv()->mFogAdjEnable = 1;
    colorFogEnv()->mFogAdjTableType = 0;
    colorFogEnv()->mFogAdjCenter = 0x140;
    colorFogXFogTableSet((u32)colorFogEnv()->mFogAdjTableType); /* dKyd_xfog_table_set */
    const f32 one = colorFogOne;
    for (u32 i = 0; i < 12; i++)
        colorFogEnv()->colorRatios()[i] = one;
    colorFogEnv()->mColChgFlag = 0;
    colorFogEnv()->field_0xc34 = zero;
    colorFogEnv()->field_0xc2c = zero;
    colorFogEnv()->field_0xc30 = zero;
    colorFogEnv()->field_0xc98 = 0;
    colorFogEnv()->mbDayNightTactStop = 0;
    ColorFogStageInfo *info = static_cast<ColorFogStageInfo *>(stageResource(&ColorFogStageVtable::getStagInfo));
    void *sched;
    if (((u32(info->mParameters) >> 16) & 7) == 3)
        sched = colorFogBossSchedule(); /* dKyd_schejule_boss_getp */
    else
        sched = colorFogSchedule(); /* dKyd_schejule_getp */
    colorFogEnv()->mpSchejule = sched;
    colorFogEnv()->mEnvrIdxPrev = 0;
    colorFogEnv()->mEnvrIdxCurr = 0;
    colorFogEnv()->mColPatBlend = one;
    colorFogEnv()->field_0xc78 = 0;
    if (dKy_checkEventNightStop()) {
        colorFogEnv()->mColpatWeather = 1;
        colorFogEnv()->mVrkumoStrength = one;
    } else {
        colorFogEnv()->mColpatWeather = 0;
        colorFogEnv()->mVrkumoStrength = zero;
    }
    colorFogEnv()->mRainCountOrig = 0;
    colorFogEnv()->field_0xc68 = 0;
    colorFogEnv()->mDiceWeatherMode = 0;
    {
        ColorFogEnvironment *env = colorFogEnv();
        env->mDiceWeatherChangeTime = f32(colorFogSave.value->mTime) + colorFogWeatherChangeDelay;
        env = colorFogEnv();
        if (!(env->mDiceWeatherChangeTime < colorFogDayLength)) {
            env = colorFogEnv();
            env->mDiceWeatherChangeTime = env->mDiceWeatherChangeTime - colorFogDayLength;
        }
    }
    colorFogEnv()->mDiceWeatherState = 0;
    colorFogEnv()->mDiceWeatherCurrPattern = 0;
    colorFogEnv()->mDiceWeatherCounter = 0;
    colorFogEnv()->mDiceWeatherTime = zero;
    colorFogEnv()->field_0xc8e = 0;
    {
        ColorFogEnvironment *e1 = colorFogEnv();
        u8 w = colorFogEnv()->mColpatWeather;
        e1->mColpatPrev = w;
        e1 = colorFogEnv();
        w = colorFogEnv()->mColpatWeather;
        e1->mColpatCurr = w;
    }
    colorFogEnv()->mColpatPrevGather = 0xFF;
    const f32 m1 = colorFogNoNextTime;
    colorFogEnv()->mColpatCurrGather = 0xFF;
    colorFogEnv()->mColPatBlendGather = m1;
    colorFogEnv()->mColPatMode = 0;
    colorFogEnv()->mColPatModeGather = 0;
    if (envr == 0)
        envr = colorFogDefaultEnvr(); /* dKyd_dmenvr_getp */
    colorFogEnv()->mpEnvrInfo = envr;
    if (palette == 0)
        palette = colorFogDefaultPalette(); /* dKyd_dmpalet_getp */
    colorFogEnv()->mpPaletInfo = palette;
    if (pselect == 0)
        pselect = colorFogDefaultPselect(); /* dKyd_dmpselect_getp */
    colorFogEnv()->mpPselectInfo = pselect;
    if (vrbox == 0)
        vrbox = colorFogDefaultVrbox(); /* dKyd_dmvrbox_getp */
    colorFogEnv()->mpVrboxInfo = vrbox;
    plight_init();
    plight_set();
    if (colorFogEnv()->mNextTime != m1) {
        f32 nt = colorFogEnv()->mNextTime;
        colorFogSave.value->mTime = nt;
        colorFogEnv()->mNextTime = m1;
    }
    // HD: apply the stage-specific start times.
    bool umikz = stageNameEquals("A_umikz"); /* "A_umikz" */
    if (!umikz) {
        ColorFogStageInfo *inf = static_cast<ColorFogStageInfo *>(stageResource(&ColorFogStageVtable::getStagInfo));
        s32 h = (s8)((u32(inf->mParameters) >> 8) & 0xFF); /* dStage_stagInfo_GetTimeH */
        if (h >= 0) {
            f32 t = colorFogDegreesPerHour * f32(h);
            colorFogSave.value->mTime = t;
        }
    }
    colorFogEnv()->mTimeAdv = colorFogTimeAdvance;
    const f32 k240 = colorFogEveningStageTime;
    if (stageNameEquals("A_umikz")) {
        ColorFogSave *sv = colorFogSave.value;
        if (f32(sv->mTime) < k240)
            sv->mTime = k240;
    } else if (stageNameEquals("sea_E")) { /* "sea_E" */
        colorFogSave.value->mTime = k240;
    }
    bool sea44 = false;
    if (stageNameEquals("sea") && dComIfGp_roomControl_getStayNo() == 0x2C)
        sea44 = true; /* "sea" */
    if (sea44 && !colorFogIsGetItem(colorFogSave.value->mItems, 2, 0)) {
        ColorFogSave *sv = colorFogSave.value;
        if (!(f32(sv->mTime) < colorFogHalfDay))
            sv->mTime = colorFogSeaStartTime;
    }
    if (stageNameEquals("ENDumi"))
        colorFogSave.value->mTime = colorFogEndingStageTime; /* "ENDumi" */
    if (dKy_checkEventNightStop())
        colorFogSave.value->mTime = zero;
    colorFogMemset(&colorFogEnv()->mMoyaSE, 0, 0x10);
    colorFogEnv()->field_0x10a8 = one;
    const f32 k3 = colorFogFlickerDivisor;
    colorFogEnv()->field_0x10ac = k3;
    colorFogEnv()->field_0x10b0 = one;
    colorFogEnv()->field_0x10b4 = k3;
    colorFogEnv()->field_0x10b8 = k3;
    colorFogEnv()->field_0x10bc = k3;
    for (u32 i = 0; i < 4; i++)
        colorFogEnv()->shaderLightRatios()[i] = one;
    for (u32 i = 0; i < 5; i++)
        colorFogEnv()->shaderLightFactors()[i] = one;
    colorFogEnv()->field_0x1030 = zero;
    colorFogEnv()->field_0x1034 = zero;
    colorFogEnv()->field_0x1044 = zero;
    colorFogEnv()->field_0x1048 = zero;
    colorFogEnv()->field_0x1038 = one;
    colorFogEnv()->field_0x103c = one;
    f32 t = f32(colorFogSave.value->mTime);
    u8 night = (t > colorFogDayStart && t < colorFogDayEnd) ? 0 : 1;
    colorFogEnv()->field_0x10a6 = night;
    colorFogEnv()->field_0x10e8 = zero;
    colorFogEnv()->field_0x1040 = zero;
    colorFogEnv()->field_0xc9a = 0xFF;
    colorFogEnv()->field_0xc9b = 0xFF;
    colorFogEnv()->field_0x10a4 = 0;
    colorFogEnv()->field_0xc44 = one;
    colorFogPointLightPowerScale = colorFogLightPowerDefault;
    if (stageNameEquals("Obshop"))
        colorFogPointLightPowerScale = colorFogShopLightPower; /* "Obshop" */
    colorFogEnv()->field_0xc78 = 0;
    colorFogEnv()->field_0x102c = one;
    colorFogEnv()->mPLightNearPlayer.x = zero;
    colorFogEnv()->mPLightNearPlayer.y = zero;
    colorFogEnv()->mPLightNearPlayer.z = zero;
    dKy_setLight_init();
    colorFogWaveInit(); /* dKy_wave_chan_init */
    dKy_Sound_init();
    colorFogWindSet(); /* dKyw_wind_set */
    // HD: reset the room-light record and load its pitch/yaw tables.
    env0->roomLighting().mParameters[0] = zero;
    env0->roomLighting().mParameters[1] = zero;
    env0->roomLighting().mParameters[2] = zero;
    env0->roomLighting().mParameters[3] = zero;
    env0->roomLighting().mFlags[0] = 0;
    env0->roomLighting().mParameters[4] = zero;
    env0->roomLighting().mFlags[1] = 0;
    env0->roomLighting().mFlags[2] = 0;
    env0->roomLighting().mParameters[5] = zero;
    env0->roomLighting().mFlags[3] = 0xFF;
    for (u32 i = 0; i < 0x40; i++) {
        env0->lightPitchTable()[i] = 0;
        env0->lightYawTable()[i] = 0;
    }
    u8 *res = static_cast<u8 *>(colorFogObjectResource("Stage", "baseLight.bin")); /* ("Stage", "baseLight...") */
    if (res != 0) {
        colorFogMemcpy(env0->lightPitchTable(), res, 0x80);
        colorFogMemcpy(env0->lightYawTable(), res + 0x80, 0x80);
    }
    return cPhs_COMPLEATE_e;
}

/* 0255E854 */
void dKy_setLight() {
    ColorFogEnvironment *envLight = colorFogEnv();
    ColorFogCamera *pCamera = colorFogPlay()->mpCamera;
    fopAc_ac_c *pPlayer = colorFogPlay()->mpPlayer;
    if (pCamera == nullptr)
        return;
    cXyz camfwd;
    colorFogGetVector(&pCamera->mEye, &pCamera->mCenter, &camfwd);
    {
        u32 x = colorFogWordBits(pCamera->mEye.x);
        ColorFogLightStatus *status = colorFogStatusPointer.value;
        colorFogWordBits(status->mPos2.x) = x;
        colorFogWordBits(status->mPos2.y) = colorFogWordBits(pCamera->mEye.y);
        colorFogWordBits(status->mPos2.z) = colorFogWordBits(pCamera->mEye.z);
    }
    f32 bx = envLight->mBaseLightInfluence.mPos.x;
    f32 bz = envLight->mBaseLightInfluence.mPos.z;
    f32 by = envLight->mBaseLightInfluence.mPos.y;
    fopAc_ac_c *player = colorFogPlay()->mpPlayer;
    const f32 one = colorFogOne;
    // HD: accept the base-light target immediately except while the player is entering a door.
    if (player != nullptr && player->eventInfo.mCommand == dEvtCmd_INDOOR_e) {
        colorFogAddCalc(&colorFogStatusPointer.value->mPos.x, bx, one, colorFogBaseLightMaxStep, one);
        colorFogAddCalc(&colorFogStatusPointer.value->mPos.y, by, one, colorFogBaseLightMaxStep, one);
        colorFogAddCalc(&colorFogStatusPointer.value->mPos.z, bz, one, colorFogBaseLightMaxStep, one);
    } else {
        ColorFogLightStatus *status = colorFogStatusPointer.value;
        status->mPos.x = bx;
        status->mPos.y = by;
        status->mPos.z = bz;
    }
    const f32 zero = colorFogZero;
    f32 power = envLight->mBaseLightInfluence.mPower;
    f32 v, fluc;
    if (pPlayer != nullptr) {
        if (power > zero) {
            f32 distance =
                colorFogSqrt(colorFogSquareDistance(&envLight->mBaseLightInfluence.mPos, &pPlayer->current.pos));
            f32 power = envLight->mBaseLightInfluence.mPower;
            fluc = envLight->mBaseLightInfluence.mFluctuation;
            v = distance / power;
        } else {
            fluc = envLight->mBaseLightInfluence.mFluctuation;
            v = zero;
        }
    } else {
        fluc = envLight->mBaseLightInfluence.mFluctuation;
        v = one;
    }
    const f32 k1000 = colorFogFluctuationThreshold;
    const f32 k20 = colorFogLookMinimumFov;
    const f32 k255 = colorFogColorMaximum;
    if (fluc < k1000) {
        lightFlicker(colorFogBaseTarget, v, fluc, k255, k20, one, colorFogBaseFlickerScale, true);
    } else {
        colorFogBaseTarget.value = fluc - k1000;
    }
    {
        ColorFogLightStatus *status = colorFogStatusPointer.value;
        status->mColor.r = u8(gabi::ftoi(f32(colorFogBaseTarget.value)));
        envLight->mPLightNearPlayer.x = f32(colorFogStatusPointer.value->mPos.x);
        envLight->mPLightNearPlayer.y = f32(colorFogStatusPointer.value->mPos.y);
        envLight->mPLightNearPlayer.z = f32(colorFogStatusPointer.value->mPos.z);
    }
    s32 eflight = -1;
    if (pPlayer != nullptr) {
        cXyz pos;
        pos.x = f32(pPlayer->current.pos.x);
        pos.y = f32(pPlayer->current.pos.y);
        pos.z = f32(pPlayer->current.pos.z);
        eflight = dKy_eflight_influence_id(gabi::ea(&pos), 0);
    }
    if (eflight < 0) {
        colorFogLightMask.value = 1;
    } else {
        colorFogLightMask.value = 3;
        cXyz pos;
        dKy_eflight_influence_pos(gabi::ea(&pos), eflight);
        ColorFogLightStatus *status = colorFogStatusPointer.value;
        colorFogWordBits(status[1].mPos.x) = colorFogWordBits(pos.x);
        colorFogWordBits(status[1].mPos.y) = colorFogWordBits(pos.y);
        colorFogWordBits(status[1].mPos.z) = colorFogWordBits(pos.z);
        f32 power = dKy_eflight_influence_power(eflight);
        f32 v, fluc;
        if (power > zero) {
            cXyz eye;
            eye.x = f32(pCamera->mEye.x);
            eye.y = f32(pCamera->mEye.y);
            eye.z = f32(pCamera->mEye.z);
            f32 distance = dKy_eflight_influence_distance(gabi::ea(&eye), eflight);
            v = distance / power;
            fluc = dKy_eflight_influence_yuragi(eflight);
        } else {
            v = one;
            fluc = dKy_eflight_influence_yuragi(eflight);
        }
        if (!(fluc < colorFogFluctuationThreshold)) {
            f32 target = fluc - f32(colorFogFluctuationThreshold);
            ColorFogLightStatus *status = colorFogStatusPointer.value;
            colorFogEffectTarget.value = target;
            status[1].mColor.g = u8(gabi::ftoi(target));
        } else {
            lightFlicker(colorFogEffectTarget, v, fluc, k255, k20, one, colorFogEffectFlickerScale, false);
            ColorFogLightStatus *status = colorFogStatusPointer.value;
            status[1].mColor.g = u8(gabi::ftoi(f32(colorFogEffectTarget.value)));
        }
        colorFogStatusPointer.value.get()[1].mColor.r = 0;
        colorFogStatusPointer.value.get()[1].mColor.b = 0;
    }
    // HD: submit point and effect lights to the shader light manager.
    const f32 minPower = colorFogMinimumLightPower;
    for (u32 i = 0; i < 200; i++) {
        if (colorFogEnv()->mpPLights[i].v == 0)
            continue;
        if (!(colorFogEnv()->mpPLights[i]->mPower > minPower))
            continue;
        dKankyo::LIGHT_INFLUENCE *light = colorFogEnv()->mpPLights[i];
        f32 x = light->mPos.x, y = light->mPos.y, z = light->mPos.z;
        f32 power = colorFogEnv()->mpPLights[i]->mPower;
        ColorFogRGBA color;
        colorFogConvertColorS10(&color, &colorFogEnv()->mpPLights[i]->mColor);
        ColorFogEnvironment *envLight = colorFogEnv();
        f32 powerScale = colorFogPointLightPowerScale;
        dKankyo::LIGHT_INFLUENCE *lightAgain = envLight->mpPLights[i];
        f32 radiusScale = colorFogPointLightRadiusScale;
        power = power * powerScale;
        f32 radius = radiusScale * f32(lightAgain->mHD20);
        colorFogScaleColor(&color, colorFogPointLightColorScale);
        cXyz pos;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        colorFogSubmitLight(colorFogLightManager.value, &pos, &color, 1, &colorFogLightFlags, 0, 0, 0, power, radius,
                            zero);
    }
    for (u32 i = 0; i < 10; i++) {
        if (colorFogEnv()->mpEfLights[i].v == 0)
            continue;
        if (!(colorFogEnv()->mpEfLights[i]->mPower > minPower))
            continue;
        dKankyo::LIGHT_INFLUENCE *light = colorFogEnv()->mpEfLights[i];
        f32 x = light->mPos.x, z = light->mPos.z, y = light->mPos.y;
        f32 power = colorFogEnv()->mpEfLights[i]->mPower;
        ColorFogRGBA color;
        colorFogConvertColorS10(&color, &colorFogEnv()->mpEfLights[i]->mColor);
        f32 radius = colorFogPointLightRadiusScale;
        cXyz pos;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        colorFogSubmitLight(colorFogLightManager.value, &pos, &color, 1, &colorFogLightFlags, 0, 0, 0, power, radius,
                            zero);
    }
    ColorFogMatrix invView;
    colorFogInverseTranspose(&colorFogJ3D.mViewMtx, &invView);
    LightParameterTable *table = colorFogJ3D.mpParameters;
    for (u32 i = 0; i < 8; i++) {
        if ((colorFogLightMask.value & colorFogLightMasks.values[i]) == 0)
            continue;
        s32 index = colorFogLightIndex.value->get();
        u32 count = table->mEntryCount;
        LightParameterEntry *entries = table->mpEntries;
        u32 objectCount = u32(index) < count ? u16(entries[u32(index)].mObjectCount) : u16(entries[0].mObjectCount);
        LightParameterObject *object;
        u32 guard;
        if (s32(i) < s32(objectCount)) {
            LightParameterEntry *entry = u32(index) < count ? &entries[u32(index)] : entries;
            u32 objectIndex = u16(entry->mFirstObject) + i;
            u32 objectCount = table->mObjectCount;
            gptr<LightParameterObject> *slot = nullptr;
            if (objectIndex < objectCount)
                slot = &table->mpObjects.get()[objectIndex];
            guard = colorFogLightGuard.value;
            object = gabi::at<LightParameterObject>(gabi::load<u32>(gabi::ea(slot)));
        } else {
            guard = colorFogLightGuard.value;
            object = nullptr;
        }
        if (guard == 0) {
            colorFogLightGuard.value = 1;
            colorFogLightRtti.value = gabi::ea(&colorFogTypeInfoVtable);
        }
        // HD: r5 retains the guard word at this virtual RTTI check.
        if (object != nullptr) {
            auto *vtable = gabi::at<LightParameterVtable>(object->__vtbl);
            if (!gabi::call_ptr<BOOL>(vtable->checkDerivedRuntimeTypeInfo, object, &colorFogLightRtti, guard))
                object = nullptr;
        }
        ColorFogLightStatus *status = &colorFogStatuses.values[i];
        cXyz tmp;
        colorFogMatrixVector(&colorFogJ3D.mViewMtx, &status->mPos, &tmp);
        f32 x = tmp.x, y = tmp.y, z = tmp.z;
        object->mLightPosition.y = y;
        object->mLightPosition.x = x;
        object->mLightPosition.z = z;
        colorFogMatrixVector(&invView, &status->mLightDir, i == 0 ? &envLight->mLightDir : &tmp);
        ColorFogRGBA color;
        colorFogConvertColor(&color, &status->mColor);
        colorFogWordBits(object->mLightColor[0]) = colorFogWordBits(color.r);
        colorFogWordBits(object->mLightColor[1]) = colorFogWordBits(color.g);
        colorFogWordBits(object->mLightColor[2]) = colorFogWordBits(color.b);
        colorFogWordBits(object->mLightColor[3]) = colorFogWordBits(color.a);
    }
}

/* 0255F160 */
void dKy_setLight_again() {
    ColorFogMatrix invView;
    colorFogInverseTranspose(&colorFogJ3D.mViewMtx, &invView);
    cXyz tmp;
    colorFogMatrixVector(&colorFogJ3D.mViewMtx, &colorFogStatuses.values[0].mPos, &tmp);
    LightParameterObject *object = findLightParameter(colorFogLightIndex, colorFogLightGuard, colorFogLightRtti);
    ColorFogRGBA color;
    colorFogConvertColor(&color, &colorFogStatuses.values[0].mColor);
    colorFogWordBits(object->mLightColor[0]) = colorFogWordBits(color.r);
    colorFogWordBits(object->mLightColor[1]) = colorFogWordBits(color.g);
    colorFogWordBits(object->mLightColor[2]) = colorFogWordBits(color.b);
    colorFogWordBits(object->mLightColor[3]) = colorFogWordBits(color.a);
    f32 z = tmp.z, x = tmp.x, y = tmp.y;
    object->mLightPosition.x = x;
    object->mLightPosition.z = z;
    object->mLightPosition.y = y;
    colorFogMatrixVector(&invView, &colorFogStatuses.values[0].mLightDir, &tmp);
}

/* 0255F2F8 */
GXColorS10 *dKy_Get_DifCol() {
    // HD: widen the packed actor K0 channels into the separate signed color result.
    for (u32 i = 0; i < 3; i++) {
        ColorFogEnvironment *e1 = colorFogEnv();
        u8 v = colorFogChannels(colorFogEnv()->colors().mActorK0)[i];
        colorFogChannels(e1->colors().mActorDifColor)[i] = v;
    }
    return &colorFogEnv()->colors().mActorDifColor;
}

/* 0255F360 */
f32 dKy_yuragi_ratio_set(f32 v) {
    f32 k1000 = colorFogFluctuationThreshold;
    return gabi::fmadds(v, colorFogColorMaximum, k1000);
}

/* 0255F378 */
void dKy_Itemgetcol_chg_on() {
    ColorFogEnvironment *env = colorFogEnv();
    u8 f = env->mColChgFlag;
    if (f == COLOR_CHANGE_NONE_e || f == COLOR_CHANGE_ITEM_REINIT_e)
        env->mColChgFlag = COLOR_CHANGE_ITEM_INIT_e;
}

/* 0255F3B4 */
void dKy_Itemgetcol_chg_off() {
    ColorFogEnvironment *env = colorFogEnv();
    if (env->mColChgFlag == COLOR_CHANGE_ITEM_WAIT_e)
        env->mColChgFlag = COLOR_CHANGE_ITEM_END_e;
}

/* 0255F3E8 */
void dKy_arrowcol_chg_on(cXyz *pos, s32 mode) {
    // HD: arrow pulses occupy the high nibble of the shared color-change state.
    ColorFogEnvironment *env = colorFogEnv();
    if (env->mColChgFlag != COLOR_CHANGE_NONE_e)
        return;
    if (mode == 1)
        env->mColChgFlag = COLOR_CHANGE_ARROW_MODE1_e;
    else if (mode == 2)
        env->mColChgFlag = COLOR_CHANGE_ARROW_MODE2_e;
    else
        env->mColChgFlag = COLOR_CHANGE_ARROW_DEFAULT_e;
}

/* 0255F458 */
void dKy_Sound_set(cXyz *pos, s32 p2, u32 p3, u32 p4) {
    ColorFogCamera *camera = colorFogPlay()->mpCamera;
    f32 newDist = colorFogSqrt(colorFogSquareDistance(pos, &camera->mEye));
    ColorFogEnvironment *envLight = colorFogEnv();
    f32 curDist = colorFogSqrt(colorFogSquareDistance(&envLight->mSound.field_0x0, &camera->mEye));
    if (!(newDist < curDist))
        return;
    if (!(curDist < colorFogSoundNearDistance)) {
        if (!(s32(colorFogEnv()->mSound.field_0xc) < p2))
            return;
    }
    envLight = colorFogEnv();
    colorFogWordBits(envLight->mSound.field_0x0.x) = colorFogWordBits(pos->x);
    colorFogWordBits(envLight->mSound.field_0x0.y) = colorFogWordBits(pos->y);
    colorFogWordBits(envLight->mSound.field_0x0.z) = colorFogWordBits(pos->z);
    colorFogEnv()->mSound.field_0xc = p2;
    colorFogEnv()->mSound.field_0x14 = p3;
    colorFogEnv()->mSound.field_0x10 = p4;
}

/* 0255F530 */
dKankyo::SND_INFLUENCE *dKy_Sound_get() { return &colorFogEnv()->mSound; }

/* 0255F554 */
void dKy_SordFlush_set(cXyz *pos, u32 type) {
    if (colorFogEnv()->mEfLightProc.mSwordState == 0) {
        colorFogEnv()->mEfLightProc.mSwordState = 1;
        colorFogEnv()->mEfLightProc.mSwordLightType = type;
        ColorFogEnvironment *env = colorFogEnv();
        colorFogWordBits(env->mEfLightProc.mSwordLight.mPos.x) = colorFogWordBits(pos->x);
        colorFogWordBits(env->mEfLightProc.mSwordLight.mPos.y) = colorFogWordBits(pos->y);
        colorFogWordBits(env->mEfLightProc.mSwordLight.mPos.z) = colorFogWordBits(pos->z);
    } else if (colorFogEnv()->mEfLightProc.mSwordState == 2) {
        colorFogEnv()->mEfLightProc.mSwordState = 4;
    }
}

/* 0255F5FC */
void dKy_FirstlightVec_get(cXyz *result, cXyz *pDst) {
    f32 zero = colorFogZero;
    pDst->z = zero;
    pDst->y = zero;
    pDst->x = zero;
    // HD: preserve the explicit hidden-result allocation and float copies.
    if (result == nullptr) {
        result = static_cast<cXyz *>(colorFogAllocate(sizeof(cXyz)));
        if (result == nullptr)
            return;
    }
    result->x = f32(pDst->x);
    result->y = f32(pDst->y);
    result->z = f32(pDst->z);
}

/* 0255F668 */
void GxFogSet_Sub(GXColor *pFogColor) {
    GXColor fogColor;
    colorFogColorWord(fogColor) = colorFogColorWord(*pFogColor);
    checkFogView();
    LightParameterObject *object = findLightParameter(colorFogFogIndex, colorFogFogGuard, colorFogFogRtti);
    ColorFogRGBA color;
    colorFogConvertColor(&color, &fogColor);
    // HD: upload the converted color and range into the shader parameter object.
    colorFogWordBits(object->mFogColor[0]) = colorFogWordBits(color.r);
    colorFogWordBits(object->mFogColor[1]) = colorFogWordBits(color.g);
    colorFogWordBits(object->mFogColor[2]) = colorFogWordBits(color.b);
    colorFogWordBits(object->mFogColor[3]) = colorFogWordBits(color.a);
    object->mFogStart = f32(colorFogEnv()->mFogStartZ__setLight);
    object->mFogEnd = f32(colorFogEnv()->mFogEndZ__setLight);
}

/* 0255F84C */
void GxFog_set() {
    GXColor fogColor;
    for (u32 i = 0; i < 4; i++)
        colorFogChannels(fogColor)[i] = colorFogChannels(colorFogEnv()->colors().mFogColor)[i];
    GxFogSet_Sub(&fogColor);
}

/* 0255F8A0 */
void GxFog_sea_set() {
    GXColor fogColor;
    for (u32 i = 0; i < 4; i++)
        colorFogChannels(fogColor)[i] = colorFogChannels(colorFogEnv()->colors().mVrUsoUmiColor)[i];
    GxFogSet_Sub(&fogColor);
}

/* 0255F8F4 */
void dKy_GxFog_tevstr_set(ColorFogTevStr *tev) { uploadTevFog(tev); }

/* 0255FAF0 */
void dKy_GfFog_tevstr_set(ColorFogTevStr *tev) { uploadTevFog(tev); }

/* 0255FCEC */
void dKy_change_colset(u32 p0, u32 p1, f32 p2) {
    colorFogEnv()->mColpatCurrGather = (u8)p1;
    ColorFogEnvironment *e1 = colorFogEnv();
    u8 g = colorFogEnv()->mColpatCurrGather;
    if (e1->mColpatCurr != g)
        colorFogEnv()->mColPatBlendGather = colorFogZero;
}

/* 0255FD48 */
void dKy_change_colpat(u32 p0) {
    colorFogEnv()->mColpatCurrGather = (u8)p0;
    ColorFogEnvironment *e1 = colorFogEnv();
    u8 g = colorFogEnv()->mColpatCurrGather;
    if (e1->mColpatCurr != g)
        colorFogEnv()->mColPatBlendGather = colorFogZero;
}

/* 0255FDA4 */
void dKy_custom_colset(u32 p0, u32 p1, f32 blend) {
    if (blend < colorFogOne)
        colorFogEnv()->mColpatPrevGather = (u8)p0;
    else
        colorFogEnv()->mColpatPrevGather = (u8)p1;
    colorFogEnv()->mColpatCurrGather = (u8)p1;
    colorFogEnv()->mColPatBlendGather = blend;
    colorFogEnv()->mColPatModeGather = 1;
}

/* 0255FE50 */
void dKy_custom_timeset(f32 speed) { colorFogEnv()->mTimeAdv = speed; }

/* 0255FE90 */
void dKy_setLight_mine(ColorFogTevStr *tev) {
    LightParameterObject *object = findLightParameter(colorFogLightIndex, colorFogLightGuard, colorFogLightRtti);
    ColorFogRGBA color;
    colorFogConvertColor(&color, &tev->mLightObj.mColor);
    colorFogWordBits(object->mLightColor[0]) = colorFogWordBits(color.r);
    colorFogWordBits(object->mLightColor[1]) = colorFogWordBits(color.g);
    colorFogWordBits(object->mLightColor[2]) = colorFogWordBits(color.b);
    colorFogWordBits(object->mLightColor[3]) = colorFogWordBits(color.a);
    f32 y = tev->mLightObj.mLightPosition.y, x = tev->mLightObj.mLightPosition.x, z = tev->mLightObj.mLightPosition.z;
    object->mLightPosition.x = x;
    object->mLightPosition.y = y;
    object->mLightPosition.z = z;
}

/* 0255FFF4 */
void dKy_tevstr_init_hd(ColorFogTevStr *tev, s32 roomNo, u32 param_2) {
    // HD: zero the expanded TEV object, then populate both copied shader-light blocks.
    colorFogClearWords(tev, sizeof(ColorFogTevStr) / sizeof(u32));
    s32 room = roomNo;
    if (roomNo == -1)
        room = dComIfGp_roomControl_getStayNo();
    tev->mRoomNo = (u8)room;
    u32 overrideIndex = param_2;
    s16 greenS10 = tev->mLightObj.mColorS10.g;
    f32 one = colorFogOne;
    f32 zero = colorFogZero;
    u8 red = tev->mLightObj.mColor.r;
    tev->mLightObj.mDistAtten.z = one;
    tev->mHdLightParam[0].mColor.r = (u8)red;
    tev->mLightObj.mRefBrightness = zero;
    f32 positionY = f32(tev->mLightObj.mLightPosition.y);
    tev->mLightObj.mRefDistance = zero;
    tev->mHdLightParam[0].mLightPosition.y = positionY;
    tev->mLightObj.mDistAtten.y = zero;
    tev->mLightObj.mCosAtten.x = one;
    tev->mLightObj.mCosAtten.z = one;
    s16 redS10 = tev->mLightObj.mColorS10.r;
    tev->mEnvrIdxCurr = (u8)room;
    tev->mHdLightParam[0].mColorS10.r = (u16)redS10;
    tev->mEnvrIdxPrev = (u8)room;
    u32 opaqueAlpha = 0xff;
    f32 directionZ = tev->mLightObj.mLightDirection.z;
    tev->mColorK0.a = (u8)opaqueAlpha;
    tev->mHdLightParam[0].mLightDirection.z = directionZ;
    tev->mLightObj.mDistAtten.x = zero;
    tev->mLightObj.mCosAtten.y = one;
    f32 directionX = tev->mLightObj.mLightDirection.x;
    tev->mFogColor.a = (u16)opaqueAlpha;
    tev->mHdLightParam[0].mLightDirection.x = directionX;
    u32 black = 0;
    tev->mColorC0.a = (u16)opaqueAlpha;
    tev->mLightObj.mColor.a = (u8)opaqueAlpha;
    tev->mHdLightParam[0].mColorS10.g = (u16)greenS10;
    u32 initTimer = 1;
    f32 positionZ = f32(tev->mLightObj.mLightPosition.z);
    f32 directionY = tev->mLightObj.mLightDirection.y;
    f32 positionX = tev->mLightObj.mLightPosition.x;
    tev->mHdLightParam[1].mLightDirection.y = directionY;
    tev->mHdLightParam[0].mLightPosition.x = positionX;
    tev->mInitTimer = (u8)initTimer;
    tev->mHdLightParam[0].mColor.a = (u8)opaqueAlpha;
    tev->mLightObj.mColor.g = (u8)black;
    tev->mHdLightParam[0].mColor.g = (u8)black;
    s16 alphaS10 = tev->mLightObj.mColorS10.a;
    tev->mHdLightParam[0].mColor.b = (u8)black;
    tev->mHdLightParam[0].mColorS10.a = (u16)alphaS10;
    tev->mHdLightParam[0].mCosAtten.x = one;
    tev->mHdLightParam[0].mCosAtten.y = one;
    tev->mHdLightParam[0].mCosAtten.z = one;
    tev->mHdLightParam[0].mDistAtten.x = zero;
    tev->mHdLightParam[0].mDistAtten.y = zero;
    tev->mHdLightParam[0].mDistAtten.z = one;
    tev->mHdLightParam[1].mLightDirection.z = directionZ;
    u32 initType = 0x7b;
    tev->mHdLightParam[0].mRefBrightness = zero;
    tev->mInitType = (u8)initType;
    s16 blueS10 = tev->mLightObj.mColorS10.b;
    tev->mHdLightParam[1].mColor.a = (u8)black;
    tev->mHdLightParam[1].mColorS10.b = (u16)blueS10;
    tev->mHdLightParam[1].mColorS10.g = (u16)greenS10;
    tev->mHdLightParam[1].mLightPosition.x = positionX;
    tev->mHdLightParam[1].mCosAtten.x = one;
    tev->mHdLightParam[1].mColor.r = (u8)black;
    tev->mHdLightParam[1].mColorS10.r = (u16)redS10;
    tev->mHdLightParam[1].mLightPosition.y = positionY;
    tev->mHdLightParam[1].mCosAtten.y = one;
    tev->mHdLightParam[1].mRefBrightness = zero;
    tev->mHdLightParam[1].mColor.b = (u8)black;
    tev->mHdLightParam[1].mDistAtten.x = zero;
    tev->mHdLightParam[0].mLightPosition.z = positionZ;
    tev->mHdLightParam[0].mLightDirection.y = directionY;
    tev->mHdLightParam[1].mColorS10.a = (u16)alphaS10;
    tev->mHdLightParam[0].mRefDistance = zero;
    tev->mHdLightParam[1].mRefDistance = zero;
    tev->mHdLightParam[1].mLightDirection.x = directionX;
    tev->mHdLightParam[1].mDistAtten.z = one;
    tev->mLightObj.mColor.b = (u8)black;
    tev->mHdLightParam[1].mCosAtten.z = one;
    tev->mEnvrIdxOverride = (u8)overrideIndex;
    tev->mHdLightParam[0].mColorS10.b = (u16)blueS10;
    tev->mHdLightParam[1].mDistAtten.y = zero;
    tev->mHdLightParam[1].mLightPosition.z = positionZ;
    tev->mHdLightParam[1].mColor.g = (u8)black;
}

/* 0256019C */
s32 dKy_rain_check_hd() { return s32(colorFogEnv()->mRainCount); }

/* 025601C0 */
void dKy_usonami_set(f32 v) {
    if (colorFogEnv()->mWaveChan.mWaveCount < 200) {
        colorFogEnv()->mWaveChan.mWaveSpawnDist = colorFogWaveSpawnDistance;
        colorFogEnv()->mWaveChan.mWaveSpawnRadius = colorFogWaveSpawnRadius;
        colorFogEnv()->mWaveChan.mWaveReset = 0;
        colorFogEnv()->mWaveChan.mWaveScale = colorFogWaveScale;
        colorFogEnv()->mWaveChan.mWaveScaleRand = colorFogMinimumWaveScale;
        colorFogEnv()->mWaveChan.mWaveCounterSpeedScale = colorFogWaveCounterSpeed;
        colorFogEnv()->mWaveChan.field_0x2f = 0;
        colorFogEnv()->mWaveChan.mWaveScaleBottom = colorFogWaveScaleBottom;
        colorFogEnv()->mWaveChan.mWaveCount = 300;
        colorFogEnv()->mWaveChan.mWaveSpeed = colorFogWaveSpeed;
    }
    colorFogEnv()->mWaveChan.mWaveFlatInter = v;
}

/* 025602A8 */
u8 dKy_get_schbit() { return colorFogEnv()->mSchbit; }

/* 025602CC */
s32 dKy_get_schbit_timer() { return (s32)colorFogEnv()->mSchbitTimer; }

/* 025602F0 */
void dKy_get_seacolor(GXColor *amb, GXColor *dif) {
    ColorFogEnvironment *e = colorFogEnv();
    s32 r;
    {
        ColorFogEnvironment *e2 = colorFogEnv();
        r = (s16)(e->colors().mBG1_C0.r + e2->mBg1AddColAmb.r);
    }
    s32 g;
    {
        ColorFogEnvironment *e2 = colorFogEnv();
        g = (s16)(e->colors().mBG1_C0.g + e2->mBg1AddColAmb.g);
    }
    s32 b;
    {
        ColorFogEnvironment *e2 = colorFogEnv();
        b = (s16)(e->colors().mBG1_C0.b + e2->mBg1AddColAmb.b);
    }
    r = clampColorChannel(r);
    g = clampColorChannel(g);
    b = clampColorChannel(b);
    amb->g = (u8)g;
    amb->b = (u8)b;
    amb->r = (u8)r;
    {
        ColorFogEnvironment *e2 = colorFogEnv();
        r = (s16)(e->colors().mBG1_K0.r + e2->mBg1AddColDif.r);
    }
    {
        ColorFogEnvironment *e2 = colorFogEnv();
        g = (s16)(e->colors().mBG1_K0.g + e2->mBg1AddColDif.g);
    }
    {
        ColorFogEnvironment *e2 = colorFogEnv();
        b = (s16)(e->colors().mBG1_K0.b + e2->mBg1AddColDif.b);
    }
    r = clampColorChannel(r);
    g = clampColorChannel(g);
    b = clampColorChannel(b);
    dif->g = (u8)g;
    dif->b = (u8)b;
    dif->r = (u8)r;
}

/* 02560444 */
void dKy_set_allcol_ratio(f32 ratio) { colorFogEnv()->mAllColGatherRatio = ratio; }

/* 02560484 */
void dKy_itudemo_se() {
    u32 se = colorFogEnv()->mMoyaSE;
    s32 roomNo = dComIfGp_roomControl_getStayNo();
    if (se != 0)
        colorFogSoundStart(se); /* mDoAud_seStart */
    if (stageNameEquals("M_NewD2") && roomNo == 3)
        colorFogMuteMtDragon(); /* "M_NewD2": mDoAud_bgmMuteMtDragon */
    if (stageNameEquals("Siren") && roomNo == 0x12)
        colorFogSkyCloisters(); /* "Siren": mDoAud_stSkyCloisters */
}

/* 025606D4 */
void dKy_contrast_flg_set(u32 f) { colorFogEnv()->mbContrastFlag = (u8)f; }

/* 02560704 */
u8 dKy_contrast_flg_get() { return colorFogEnv()->mbContrastFlag; }

/* 02560728 */
void dKy_set_nexttime(f32 t) { colorFogEnv()->mNextTime = t; }

/* 02560768 */
void dKy_DayProc() { dKankyo_DayProc(); }

/* 0256076C */
void dKy_instant_timechg(f32 t) {
    ColorFogSave *sv = colorFogSave.value;
    if (t < f32(sv->mTime)) {
        sv->mDate = u16(u16(sv->mDate) + 1);
        dKankyo_DayProc();
        sv = colorFogSave.value;
    }
    sv->mTime = t;
}

/* 025607E0 */
void dKy_instant_rainchg() {
    colorFogRainSet(250);
    colorFogEnv()->mColpatWeather = 1;
    colorFogEnv()->mColpatPrev = 1;
    colorFogEnv()->mColpatCurr = 1;
}

/* 02560828 */
s32 dKy_moon_type_chk() {
    s32 weekday = dKy_get_dayofweek();
    if (f32(colorFogSave.value->mTime) < colorFogHalfDay) {
        if (weekday != 0)
            weekday--;
        else
            weekday = 6;
    }
    return weekday;
}

/* 02560878 */
BOOL dKy_telescope_lookin_chk(cXyz *pPos, f32 maxDist, f32 minFov) {
    BOOL ret = FALSE;
    if (colorFogPlay()->mScopeMesgStatus == 0)
        return ret;
    if (!(colorFogPlay()->mpView->mFovy > minFov))
        return ret;
    cXyz proj;
    colorFogProject(pPos, &proj);
    cXyz center;
    f32 zero = colorFogZero;
    // HD: the projected screen center is the origin.
    center.y = zero;
    center.z = zero;
    center.x = zero;
    if (colorFogSqrt(colorFogSquareDistance(&center, &proj)) < maxDist)
        ret = TRUE;
    return ret;
}

/* 02560944 */
BOOL dKy_moon_look_chk() {
    BOOL rt = FALSE;
    if (colorFogMoonArrivalCheck()) {
        ColorFogEnvironment *env = colorFogEnv();
        rt = dKy_telescope_lookin_chk(&env->mMoonPos, colorFogStarLookDistance, colorFogLookMinimumFov);
    }
    return rt;
}

/* 0256099C */
void dKy_get_orion_pos(cXyz *result) {
    constellationPosition(result, colorFogOrionX, colorFogOrionY, colorFogOrionZ, false);
}

/* 02560A60 */
BOOL dKy_orion_look_chk() {
    cXyz pos;
    dKy_get_orion_pos(&pos);
    cXyz copy;
    // GHS copies words in x/z/y order.
    colorFogWordBits(copy.x) = colorFogWordBits(pos.x);
    colorFogWordBits(copy.z) = colorFogWordBits(pos.z);
    colorFogWordBits(copy.y) = colorFogWordBits(pos.y);
    return dKy_telescope_lookin_chk(&copy, colorFogStarLookDistance, colorFogLookMinimumFov);
}

/* 02560AB4 */
void dKy_get_hokuto_pos(cXyz *result) {
    constellationPosition(result, colorFogHokutoX, colorFogHokutoY, colorFogHokutoZ, true);
}

/* 02560B78 */
BOOL dKy_hokuto_look_chk() {
    cXyz pos;
    dKy_get_hokuto_pos(&pos);
    cXyz copy;
    colorFogWordBits(copy.x) = colorFogWordBits(pos.x);
    colorFogWordBits(copy.z) = colorFogWordBits(pos.z);
    colorFogWordBits(copy.y) = colorFogWordBits(pos.y);
    return dKy_telescope_lookin_chk(&copy, colorFogStarLookDistance, colorFogLookMinimumFov);
}

/* 02560BCC */
void dKy_get_moon_pos(cXyz *result) {
    cXyz *src = &colorFogEnv()->mMoonPos;
    if (result == nullptr) {
        result = static_cast<cXyz *>(colorFogAllocate(sizeof(cXyz)));
        if (result == nullptr)
            return;
    }
    result->x = f32(src->x);
    result->y = f32(src->y);
    result->z = f32(src->z);
}

/* 02560C34 */
void dKy_pship_existense_set() { colorFogEnv()->field_0xc8e = 1; }

/* 02560C5C */
void dKy_pship_existense_cut() { colorFogEnv()->field_0xc8e = 0; }

/* 02560C84 */
BOOL dKy_daynighttact_stop_chk() {
    BOOL rt = FALSE;
    if (dKy_checkEventNightStop())
        rt = TRUE;
    else if (colorFogEnv()->mbDayNightTactStop != 0)
        rt = TRUE;
    return rt;
}

/* 02560CD0 */
BOOL dKyr_player_overhead_bg_chk() {
    fopAc_ac_c *player_p = colorFogPlay()->mpPlayer;
    BOOL ret = FALSE;
    ColorFogGndCheck gnd_chk;
    colorFogConstructGround(&gnd_chk);
    gnd_chk.mbBombThrough = 0;
    gnd_chk.mbBoomerangThrough = 0;
    gnd_chk.mbRopeThrough = 0;
    gnd_chk.mPolyPassChk = gabi::ea(&gnd_chk.mPolyPassVtable);
    gnd_chk.mCheckVtable = gabi::ea(&colorFogGndCheckVtable);
    gnd_chk.mPolyInfoVtable = gabi::ea(&colorFogGndPolyInfoVtable);
    gnd_chk.mbArrowThrough = 0;
    gnd_chk.mGroupPassVtable = gabi::ea(&colorFogGroupPassVtable);
    gnd_chk.mGrpPassChk = gabi::ea(&gnd_chk.mGroupPassVtable);
    gnd_chk.mbLinkThrough = 0;
    gnd_chk.mbObjThrough = 1;
    gnd_chk.mGrp = ColorFogGndCheck::FULL_GRP;
    gnd_chk.mPolyPassVtable = gabi::ea(&colorFogPolyPassVtable);
    gnd_chk.mbCamThrough = 0;
    if (colorFogRainCheck() > 200) {
        f32 x = player_p->current.pos.x;
        f32 height = colorFogOverheadCastHeight;
        f32 y = player_p->current.pos.y, z = player_p->current.pos.z;
        gnd_chk.m_pos.x = x;
        gnd_chk.m_pos.z = z;
        gnd_chk.m_pos.y = y + height;
        void *bgs = colorFogPlay()->mBgS;
        f32 ground = colorFogGroundCross(bgs, &gnd_chk);
        if (ground < f32(player_p->current.pos.y) + f32(colorFogOverheadClearance))
            ret = TRUE;
    }
    gnd_chk.mPolyInfoVtable = gabi::ea(&colorFogBasePolyInfoVtable);
    gnd_chk.mPolyPassVtable = gabi::ea(&colorFogBasePolyPassVtable);
    gnd_chk.mGroupPassVtable = gabi::ea(&colorFogBaseGroupPassVtable);
    colorFogDestroyCheck(&gnd_chk, 0);
    return ret;
}

/* 02560E08 */
f32 d_kankyo_cpp::ColorFogEnvironment::getDaytime() { return mCurTime; }

void dKy_tevstr_init(u32 tev, s32 roomNo, u32 overrideIndex) {
    dKy_tevstr_init_hd(gabi::at<ColorFogTevStr>(tev), roomNo, overrideIndex);
}
} // namespace d_kankyo_cpp
using namespace d_kankyo_cpp;

/* WWHD glue
data colorFogFloatRegisters 1047B7C0
data colorFogShortRegisters 1047B838
ext 025E1C20 void colorFogInitWindowPos()
ext 025E1B70 void colorFogInitSeaEnvPos()
ext 025E1BF8 void colorFogInitRiverPos()
ext 025638D8 void colorFogXFogTableSet(u32)
ext 02563A10 void* colorFogBossSchedule()
ext 02563A04 void* colorFogSchedule()
ext 025639EC void* colorFogDefaultEnvr()
ext 025639D4 void* colorFogDefaultPalette()
ext 025639E0 void* colorFogDefaultPselect()
ext 025639F8 void* colorFogDefaultVrbox()
ext 025B5D40 BOOL colorFogIsGetItem(void*,u32,u32)
ext C000A858 void colorFogMemset(void*,s32,u32)
ext C000A848 void colorFogMemcpy(void*,const void*,u32)
ext 0256EF7C void colorFogWaveInit()
ext 0257D398 void colorFogWindSet()
ext 0252447C void* colorFogObjectResource(const char*,const char*)
ext 025E1988 void colorFogSoundStart(u32)
ext 025E1D18 void colorFogMuteMtDragon()
ext 025E1D24 void colorFogSkyCloisters()
str 1004F2DC "Stage" in 0255DAC0
str 1004F2E4 "baseLight.bin" in 0255DAC0
str 1004F310 "M_NewD2" in 02560484
str 1004F318 "Siren" in 02560484
str 1004EF40 "A_umikz" in 0255DAC0
str 1004EF58 "sea_E" in 0255DAC0
str 1004EF50 "sea" in 0255DAC0
str 1004EF64 "ENDumi" in 0255DAC0
str 1004EF6C "Obshop" in 0255DAC0
data colorFogStringVtable 1004EF7C
data colorFogGndCheckVtable 1004F044
data colorFogGndPolyInfoVtable 1004F054
data colorFogGroupPassVtable 1004F064
data colorFogPolyPassVtable 1004F074
data colorFogBasePolyInfoVtable 1004EFD4
data colorFogBasePolyPassVtable 1004EFF4
data colorFogBaseGroupPassVtable 1004EFB4
ext 02008E0C void colorFogConstructGround(d_kankyo_cpp::ColorFogGndCheck*)
ext 02008974 f32 colorFogGroundCross(void*,d_kankyo_cpp::ColorFogGndCheck*)
ext 02008DAC void colorFogDestroyCheck(d_kankyo_cpp::ColorFogGndCheck*,s32)
ext 0256019C s32 colorFogRainCheck()
data colorFogJ3D 104B45C0
data colorFogStatuses 10476C74
data colorFogStatusPointer 101E8CC8
data colorFogLightMask 101E8CDC
data colorFogLightMasks 101E8CE0
data colorFogBaseTarget 101E8EC8
data colorFogEffectTarget 101E8ECC
data colorFogLightManager 101F8A20
data colorFogLightFlags 104A01CC
ext 02563F64 void colorFogGetVector(cXyz*,cXyz*,cXyz*)
ext 0200ECD4 f32 colorFogAddCalc(be<f32>*,f32,f32,f32,f32)
ext 0200ED84 f32 colorFogAddCalc2(be<f32>*,f32,f32,f32)
ext 020198D8 f32 colorFogRandom(f32)
ext 0274D2AC void colorFogScaleColor(d_kankyo_cpp::ColorFogRGBA*,f32)
ext 0273862C void colorFogSubmitLight(void*,cXyz*,d_kankyo_cpp::ColorFogRGBA*,s32,void*,s32,s32,s32,f32,f32,f32)
ext 025F20B0 void colorFogInverseTranspose(d_kankyo_cpp::ColorFogMatrix*,d_kankyo_cpp::ColorFogMatrix*)
ext 028E8F64 void colorFogMatrixVector(d_kankyo_cpp::ColorFogMatrix*,cXyz*,cXyz*)
ext 02556DE4 void colorFogConvertColor(d_kankyo_cpp::ColorFogRGBA*,GXColor*)
ext 02556E98 void colorFogConvertColorS10(d_kankyo_cpp::ColorFogRGBA*,GXColorS10*)
ext 028F5914 void colorFogClearWords(d_kankyo_cpp::ColorFogTevStr*,u32)
data colorFogSave 101F84DC
data colorFogOrionX 1004F320
data colorFogOrionY 1004F324
data colorFogOrionZ 1004F328
data colorFogHokutoX 1004F32C
data colorFogHokutoY 1004F330
data colorFogHokutoZ 1004F334
ext 025F0EA4 void colorFogProject(cXyz*,cXyz*)
ext 02565DAC BOOL colorFogMoonArrivalCheck()
ext 0273AD10 void* colorFogAllocate(u32)
data colorFogParameterTable 104B4708
data colorFogFogIndex 104A1460
data colorFogLightIndex 104A1F70
data colorFogFogGuard 101FD7DC
data colorFogFogRtti 101FDCE0
data colorFogLightGuard 101FD7D4
data colorFogLightRtti 101FDCCC
data colorFogTypeInfoVtable 1004EFA4
data colorFogOne 1004F0A4
data colorFogZero 1004F0B0
data colorFogNoNextTime 1004F0B4
data colorFogMinimumLightPower 1004F0BC
data colorFogDegreesPerHour 1004F0FC
data colorFogColorMaximum 1004F104
data colorFogDayLength 1004F108
data colorFogHalfDay 1004F10C
data colorFogSeaStartTime 1004F12C
data colorFogDayStart 1004F14C
data colorFogDayEnd 1004F150
data colorFogMinimumWaveScale 1004F178
data colorFogWaveSpawnDistance 1004F1C4
data colorFogEffectFlickerScale 1004F1C8
data colorFogStarLookDistance 1004F240
data colorFogFluctuationThreshold 1004F29C
data colorFogWaveSpeed 1004F2A4
data colorFogBaseFlickerScale 1004F2BC
data colorFogWeatherChangeDelay 1004F2C0
data colorFogTimeAdvance 1004F2C4
data colorFogEveningStageTime 1004F2C8
data colorFogEndingStageTime 1004F2CC
data colorFogFlickerDivisor 1004F2D0
data colorFogLightPowerDefault 1004F2D4
data colorFogShopLightPower 1004F2D8
data colorFogBaseLightMaxStep 1004F2F4
data colorFogLookMinimumFov 1004F2F8
data colorFogSoundNearDistance 1004F2FC
data colorFogWaveSpawnRadius 1004F300
data colorFogWaveScale 1004F304
data colorFogWaveCounterSpeed 1004F308
data colorFogWaveScaleBottom 1004F30C
data colorFogOverheadCastHeight 1004F338
data colorFogOverheadClearance 1004F33C
data colorFogPointLightRadiusScale 101E8EF4
data colorFogPointLightColorScale 101E8EF8
data colorFogPointLightPowerScale 101E8EFC
ext 02555D0C d_kankyo_cpp::ColorFogEnvironment* colorFogEnv()
ext 025200D4 d_kankyo_cpp::ColorFogPlay* colorFogPlay()
ext 028E8DE8 f32 colorFogSquareDistance(const cXyz*,const cXyz*)
ext 028F4384 f32 colorFogSqrt(f32)
ext 0257E7C0 void colorFogRainSet(s32)
*/
