/* Environment lighting: point/effect lights, time and weather progression, and HD scene-light presets.
 * The complete six-part unit is verified against the original implementation.
 */

#include "d/d_kankyo_4.h"
#include "gc_api/d/d_com_inf_game.h"

namespace d_kankyo_cpp {
void dKy_plight_priority_set(u32 light);
void dKy_plight_cut_hd(u32 light);
void dKy_set_actcol_ratio(f32 ratio);
void dKy_set_bgcol_ratio(f32 ratio);
void dKy_set_fogcol_ratio(f32 ratio);
void dKy_set_vrboxsoracol_ratio(f32 ratio);
void dKy_set_vrboxkumocol_ratio(f32 ratio);
void dKy_set_vrboxcol_ratio(f32 ratio);
void dKy_Itemgetcol_chg_move();
BOOL dKy_Draw();
void dKy_Sound_init();
void dKy_efplight_set_hd(u32 light);
void dKy_efplight_cut_hd(u32 light);
BOOL dKy_daylight_preset_chk_hd();
BOOL dKy_Execute();
BOOL dKy_IsDelete(u32 p);
BOOL dKy_Delete(u32 p);
void dKy_setLight_init();

static FramePresetPointer frameLightPresets;
static FrameScenePointer frameSceneLights;
static FrameHdOptions frameHdOptions;
static FrameWord frameEventFrame;
static FrameEventName frameEventName;
static FrameWeatherChoices frameWeatherChoices;
static FrameWeatherModes frameWeatherModes0;
static FrameWeatherModes frameWeatherModes1;
static FrameWeatherModes frameWeatherModes2;
static FrameWeatherModes frameWeatherModes3;
static FrameWeatherTimes frameWeatherTimes0;
static FrameWeatherTimes frameWeatherTimes1;
static FrameWeatherTimes frameWeatherTimes2;
static FrameFloat frameWeatherTimes3;
static FrameWeatherDelays frameWeatherDelays;
static FrameLightStatus lightStatusBase;
static FrameLightStatuses lightStatusData;
static inline f32 i2f(s32 v) { return f32(v); }
static FrameFloat frameOne;
static FrameFloat frameHalf;
static FrameFloat frameZero;
static FrameFloat frameItemLightOffset;
static FrameFloat frameItemLightPower;
static FrameFloat frameItemFadeInStep;
static FrameFloat frameMinimumStep;
static FrameFloat frameItemFadeOutStep;
static FrameFloat frameSmallStep;
static FrameFloat frameItemFadeEnd;
static FrameFloat frameItemRatio;
static FrameFloat frameArrowFadeInStep;
static FrameFloat frameArrowFadeOutStep;
static FrameFloat frameArrowFadeEnd;
static FrameFloat frameBackgroundAmbientRatio;
static FrameFloat frameBackgroundDiffuseRatio;
static FrameFloat frameEveningTime;
static FrameFloat frameUnsignedFloatBias;
static FrameFloat frameDayLength;
static FrameFloat frameDayStopTime;
static FrameFloat frameNightStopTime;
static FrameFloat frameHalfDay;
static FrameFloat frameSoundFar;
static FrameFloat frameSwordLightPower;
static FrameFloat frameSwordLightFluctuation;
static FrameFloat frameNoBlend;
static FrameFloat frameHoursPerDegree;
static FrameFloat frameMonotoneRange;
static FrameFloat frameMonotoneFrameStep;
static FrameFloat frameMonotoneExtraStep;
static FrameFloat frameTwilightSpan;
static FrameFloat frameMorningTime;
static FrameFloat frameWeatherChoiceRange;
static FrameFloat frameStormSkyBrightness;
static FrameSavePointer frameSave;
static FrameByte frameTimePass;
static FrameFlushTable frameFlushColors;
static FrameFlushTable frameFlushColors2;
static FrameSinCosTable frameSinCos;

void dKy_actor_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_actor_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg1_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg1_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg2_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg2_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg3_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor);
void dKy_bg3_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor);
BOOL toon_proc_check();
void dScnKy_env_light_c_setSunpos(u32 t);
void dScnKy_env_light_c_setLightPos_hd(u32 t);
void dScnKy_env_light_c_SetBaseLight(u32 t);
void dScnKy_env_light_c_setLight(u32 t);

static constexpr u32 SafeString_vtbl = 0x1004EF7C;

// HD: preserve SafeString resolution order and the bounded comparison for unterminated input.
static inline bool stageNameEquals(const char *lit) {
    gabi::Local<FrameString> a;
    a->str = gabi::guest_addr(lit);
    a->vtbl = SafeString_vtbl;
    auto *play = framePlay();
    gabi::Local<FrameString> b;
    b->vtbl = SafeString_vtbl;
    b->str = gabi::ea(play->mStageName);
    gabi::call_ptr<void>(gabi::at<FrameStringVtable>(a->vtbl)->resolve.get(), a.get());
    gabi::call_ptr<void>(gabi::at<FrameStringVtable>(a->vtbl)->resolve.get(), a.get());
    u32 bv = b->vtbl;
    u32 s1 = a->str;
    gabi::call_ptr<void>(gabi::at<FrameStringVtable>(bv)->resolve.get(), b.get());
    u32 s2 = b->str;
    if (s1 == s2)
        return true;
    for (u32 n = 0x40001; n != 0; n--) {
        u8 c1 = u8(*gabi::at<be<u8>>(s1)), c2 = u8(*gabi::at<be<u8>>(s2));
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
        s1++;
        s2++;
    }
    return false;
}

static inline s32 subtractColorByte(s32 color, const be<u8> *channel) { return s16(color - s32(*channel)); }

BOOL dKy_checkEventNightStop();
void dKankyo_DayProc();
s32 dKy_getdaytime_hour();
s32 dKy_getdaytime_minute();
s32 dKy_get_dayofweek();
s32 dKy_daynight_check();

s32 dKy_eflight_influence_id(u32 pos, s32 nth);
s16 u8_data_ratio_set(s32 a, s32 b, f32 ratio);

void dScnKy_env_light_c_setDaytime(u32 t);
void dScnKy_env_light_c_CalcTevColor(u32 t);
void dScnKy_env_light_c_Sndpos(u32 t);
void dScnKy_env_light_c_Eflight_flush_proc(u32 t);
void dScnKy_env_light_c_SetSchbit(u32 t);

static inline void copyEnvironmentu8(be<u8> KankyoEnvLight::*dst, be<u8> KankyoEnvLight::*src) {
    auto *out = frameEnv();
    u8 value = u8(frameEnv()->*src);
    out->*dst = value;
}
static inline void copyEnvironmentf32(be<f32> KankyoEnvLight::*dst, be<f32> KankyoEnvLight::*src) {
    auto *out = frameEnv();
    f32 value = f32(frameEnv()->*src);
    out->*dst = value;
}

u8 phantomship_wether();
void dice_wether_init(u32 mode, f32 weatherTime, f32 curTime);
void dice_wether_execute(u32 mode, f32 weatherTime, f32 curTime);
void dice_rain_minus();
void dScnKy_env_light_c_exeKankyo(u32 t);

void plight_init();

// HD: stopped time may advance in A_umikz and ENDumi; the sea layer stop matches the USA version.
/* 0255B300 */

void d_kankyo_cpp::dScnKy_env_light_c::setDaytime() {

    this->mCurTime = f32(frameSave.value->mTime);
    this->mDayOfWeek = u16(frameSave.value->mDate);
    bool stop = false;
    if (stageNameEquals("sea")) {
        if (dComIfGp_roomControl_getStayNo() == 0xE) {
            if ((s8)u8(framePlay()->mStageLayer) == 2 || (s8)u8(framePlay()->mStageLayer) == 3)
                stop = true;
        }
    }
    bool nightStop = dKy_checkEventNightStop();
    const f32 k270 = f32(frameEveningTime.value);
    bool endumi;
    if (nightStop || !frameIsGetItem(&frameSave.value->mItems, 2, 0) || u8(framePlay()->mEventRunning) != 0 || stop) {
        if (stageNameEquals("A_umikz")) {
            f32 v = f32(this->mCurTime) + f32(this->mTimeAdv);
            if (v < k270)
                this->mCurTime = v;
        }
        endumi = stageNameEquals("ENDumi");
    } else {
        framePlay();
        if (u8(frameTimePass.value) != 0) {
            f32 v = f32(this->mCurTime) + f32(this->mTimeAdv);
            const f32 k2p31 = f32(frameUnsignedFloatBias.value);
            f32 u;
            if (v < k2p31)
                u = (f32)(f64)(u32)gabi::ftoi(v);
            else
                u = (f32)(f64)(u32)((u32)gabi::ftoi(v - k2p31) + 0x80000000u);
            if (!(u < f32(frameDayLength.value))) {
                this->mDayOfWeek = (u16)(u16(this->mDayOfWeek) + 1);
                this->mCurTime = f32(frameZero.value);
                dKankyo_DayProc();
            } else {
                this->mCurTime = v;
            }
        } else {
            s32 dn = dKy_daynight_check();
            f32 cur = f32(this->mCurTime);
            bool run;
            if (dn == 0)
                run = cur < f32(frameDayStopTime.value);
            else
                run = !(cur < k270) && cur < f32(frameNightStopTime.value);
            if (run)
                this->mCurTime = cur + f32(this->mTimeAdv);
        }
        endumi = stageNameEquals("ENDumi");
    }
    if (endumi) {
        f32 cur = f32(this->mCurTime);
        if (cur < f32(frameHalfDay.value))
            this->mCurTime = cur + f32(this->mTimeAdv);
    }
    mDoAud_setHour(dKy_getdaytime_hour());
    mDoAud_setMinute(dKy_getdaytime_minute());
    mDoAud_setWeekday(dKy_get_dayofweek());
    frameSave.value->mTime = f32(this->mCurTime);
    auto *sv = frameSave.value.get();
    sv->mDate = u16(this->mDayOfWeek);
}

/* 0255BD4C */

void d_kankyo_cpp::dScnKy_env_light_c::SetSchbit() {

    auto *info = static_cast<FrameStageInfo *>(dComIfGp_getStageStagInfo());
    u32 start = (u32(info->mSchedule) >> 16) & 0xFF;
    u32 sec = u8(info->mScheduleSeconds);
    if (start == 0)
        return;
    u8 bit = u8(this->mSchbit);
    u32 timer = u32(this->mSchbitTimer);
    if (bit == 0)
        this->mSchbit = 0x80;
    s32 n = (s32)timer + 1;
    if (n < (s32)(sec * 30)) {
        this->mSchbitTimer = (u32)n;
        return;
    }
    u8 b = u8(this->mSchbit);
    this->mSchbitTimer = 0;
    b = (u8)(b >> 1);
    this->mSchbit = b != 0 ? b : 0x80;
}

// HD: only the player effect-light lookup remains; point-light selection was removed.
/* 0255B8D8 */

void d_kankyo_cpp::dScnKy_env_light_c::CalcTevColor() {

    auto *player = framePlay()->mpPlayer.get();
    gabi::Local<cXyz> pos;

    pos->x = f32(player->current.pos.x);
    pos->y = f32(player->current.pos.y);
    pos->z = f32(player->current.pos.z);
    this->mPlayerEflightIdx = (u32)dKy_eflight_influence_id(gabi::ea(pos.get()), 0);
}

/* 0255B9AC */

void d_kankyo_cpp::dScnKy_env_light_c::Sndpos() {

    u32 n = u32(this->mSound.field_0x10);
    if (n == 0)
        return;
    n--;
    this->mSound.field_0x10 = n;
    if (n == 0)
        dKy_Sound_init();
}

// HD: both sword-light types use power700 and fluctuation100.
/* 0255BAF4 */

void d_kankyo_cpp::dScnKy_env_light_c::Eflight_flush_proc() {

    auto *tbl = u32(this->mEfLightProc.mSwordLightType) != 0 ? frameFlushColors2.values : frameFlushColors.values;
    u8 state = u8(this->mEfLightProc.mSwordState);
    switch (state) {
    case 1: {
        this->mEfLightProc.mSwordFrame = 0;
        this->mEfLightProc.mSwordLight.mColor.r = u8(tbl[0].r);
        this->mEfLightProc.mSwordLight.mColor.g = u8(tbl[0].g);
        this->mEfLightProc.mSwordLight.mPower = f32(frameSwordLightPower.value);
        this->mEfLightProc.mSwordLight.mColor.b = u8(tbl[0].b);
        this->mEfLightProc.mSwordLight.mFluctuation = f32(frameSwordLightFluctuation.value);
        dKy_efplight_set_hd(gabi::ea(&this->mEfLightProc.mSwordLight));
        this->mEfLightProc.mSwordState = (u8)(u8(this->mEfLightProc.mSwordState) + 1);
        return;
    }
    case 2: {
        const f32 one = f32(frameOne.value);
        u32 frame = u8(this->mEfLightProc.mSwordFrame);
        u32 last = frame;
        bool found = false;
        for (u32 i = 0; i < 3; i++) {
            auto *cur = &tbl[i];
            auto *next = &tbl[i + 1];
            u32 cf = u8(cur->frame);
            if (frame < cf)
                continue;
            u32 nf = u8(next->frame);
            if (frame > nf)
                continue;
            f32 r = one - i2f((s32)(nf - frame)) / i2f((s32)(nf - cf));
            this->mEfLightProc.mSwordLight.mColor.r = (u16)u8_data_ratio_set(u8(cur->r), u8(next->r), r);
            this->mEfLightProc.mSwordLight.mColor.g = (u16)u8_data_ratio_set(u8(cur->g), u8(next->g), r);
            this->mEfLightProc.mSwordLight.mColor.b = (u16)u8_data_ratio_set(u8(cur->b), u8(next->b), r);
            found = true;
            break;
        }
        u32 endf = u8(tbl[3].frame);
        if (found)
            last = u8(this->mEfLightProc.mSwordFrame);
        if (last > endf) {
            u8 st_ = u8(this->mEfLightProc.mSwordState);
            last = u8(this->mEfLightProc.mSwordFrame);
            this->mEfLightProc.mSwordState = (u8)(st_ + 1);
        }
        this->mEfLightProc.mSwordFrame = (u8)(last + 1);
        return;
    }
    case 3:
        dKy_efplight_cut_hd(gabi::ea(&this->mEfLightProc.mSwordLight));
        this->mEfLightProc.mSwordState = 0;
        return;
    case 4:
        dKy_efplight_cut_hd(gabi::ea(&this->mEfLightProc.mSwordLight));
        this->mEfLightProc.mSwordState = 1;
        return;
    default:
        return;
    }
}

// HD: apply scene-light presets after the GameCube environment updates.
/* 0255C85C */

void d_kankyo_cpp::dScnKy_env_light_c::exeKankyo() {

    copyEnvironmentu8(&KankyoEnvLight::mColPatMode, &KankyoEnvLight::mColPatModeGather);
    f32 zero;
    bool mode;
    if (u8(framePlay()->mEventRunning) == 0 && u8(frameEnv()->mColPatModeGather) != 0) {
        if (u8(frameEnv()->mColPatModeGather) >= 3) {
            frameEnv()->mColPatModeGather = 0;
        } else {
            auto *e = frameEnv();
            e->mColPatModeGather = (u8)(u8(e->mColPatModeGather) + 1);
        }
    }
    zero = f32(frameZero.value);
    mode = u8(frameEnv()->mColPatMode) != 0;
    bool resetBlend = false;
    if (mode) {
        if (u8(frameEnv()->mColpatPrevGather) != 0xFF) {
            copyEnvironmentu8(&KankyoEnvLight::mColpatPrev, &KankyoEnvLight::mColpatPrevGather);
            if (u8(frameEnv()->mColPatModeGather) == 0)
                frameEnv()->mColpatPrevGather = 0xFF;
        }
        if (u8(frameEnv()->mColpatCurrGather) != 0xFF) {
            copyEnvironmentu8(&KankyoEnvLight::mColpatCurr, &KankyoEnvLight::mColpatCurrGather);
            if (u8(frameEnv()->mColPatModeGather) == 0)
                frameEnv()->mColpatCurrGather = 0xFF;
        }
        if (!(f32(frameEnv()->mColPatBlendGather) < zero)) {
            copyEnvironmentf32(&KankyoEnvLight::mColPatBlend, &KankyoEnvLight::mColPatBlendGather);
            if (u8(frameEnv()->mColPatModeGather) == 0)
                resetBlend = true;
        }
    } else {
        auto *e1 = frameEnv();
        u8 cur = u8(frameEnv()->mColpatCurr);
        if (u8(e1->mColpatPrev) == cur) {
            if (u8(frameEnv()->mColpatPrevGather) != 0xFF) {
                copyEnvironmentu8(&KankyoEnvLight::mColpatPrev, &KankyoEnvLight::mColpatPrevGather);
                frameEnv()->mColpatPrevGather = 0xFF;
            }
            if (u8(frameEnv()->mColpatCurrGather) != 0xFF) {
                copyEnvironmentu8(&KankyoEnvLight::mColpatCurr, &KankyoEnvLight::mColpatCurrGather);
                frameEnv()->mColpatCurrGather = 0xFF;
                copyEnvironmentu8(&KankyoEnvLight::mColpatWeather, &KankyoEnvLight::mColpatCurr);
            }
            if (!(f32(frameEnv()->mColPatBlendGather) < zero)) {
                copyEnvironmentf32(&KankyoEnvLight::mColPatBlend, &KankyoEnvLight::mColPatBlendGather);
                resetBlend = true;
            }
        }
    }
    if (resetBlend)
        frameEnv()->mColPatBlendGather = f32(frameNoBlend.value);
    copyEnvironmentf32(&KankyoEnvLight::mAllColRatio, &KankyoEnvLight::mAllColGatherRatio);
    copyEnvironmentf32(&KankyoEnvLight::mActColRatio, &KankyoEnvLight::mActColGatherRatio);
    copyEnvironmentf32(&KankyoEnvLight::mBgColRatio, &KankyoEnvLight::mBgColGatherRatio);
    copyEnvironmentf32(&KankyoEnvLight::mFogColRatio, &KankyoEnvLight::mFogColGatherRatio);
    copyEnvironmentf32(&KankyoEnvLight::mVrboxSoraColRatio, &KankyoEnvLight::mVrboxSoraColGatherRatio);
    copyEnvironmentf32(&KankyoEnvLight::mVrboxKumoColRatio, &KankyoEnvLight::mVrboxKumoColGatherRatio);
    const f32 one = f32(frameOne.value);
    for (auto member : {&KankyoEnvLight::mAllColGatherRatio, &KankyoEnvLight::mActColGatherRatio,
                        &KankyoEnvLight::mBgColGatherRatio, &KankyoEnvLight::mFogColGatherRatio,
                        &KankyoEnvLight::mVrboxSoraColGatherRatio, &KankyoEnvLight::mVrboxKumoColGatherRatio})
        frameEnv()->*member = one;
    this->setDaytime();
    dKyw_wether_proc();
    this->CalcTevColor();
    this->Sndpos();
    this->Eflight_flush_proc();
    this->SetSchbit();

    s32 hour = gabi::ftoi(f32(this->mCurTime) * f32(frameHoursPerDegree.value));
    auto *tbl = frameLightPresets.value.get();
    auto *graphics = frameSceneLights.value.get();
    auto *pp = graphics->mpSets.get();
    if (u32(graphics->mSetCount) > 1)
        ++pp;
    auto *night = tbl->mpNight.get();
    auto *set = pp->get();
    auto *day = tbl->mpDay.get();
    dComIfGp_getStageStagInfo();
    auto apply = [&](void *a, void *b, f32 f) { frameApplyLightPresets(set, a, b, f); };
    if (u8(frameHdOptions.mMonotone) != 0) {
        frameCalcMonotone();
        if (stageNameEquals("Hyroom")) {
            auto *pa = tbl->mpMonotoneA.get();
            s32 v = s16(frameHdOptions.mLevel);
            f32 f13 = i2f(v + 600) / f32(frameMonotoneRange.value);
            s32 w = s16(frameHdOptions.mDirection);
            auto *pb = tbl->mpMonotoneB.get();
            f32 target = one - f13;
            if (w < 0 && v > -250) {
                this->monotoneBlend() = zero;
            } else {
                frameChaseF(&this->monotoneBlend(), target,
                            f32(frameMonotoneFrameStep.value) + f32(frameMonotoneExtraStep.value));
            }

            gabi::Local<FrameString> a;
            a->str = gabi::guest_addr("Demo49");
            a->vtbl = SafeString_vtbl;
            gabi::Local<FrameString> b;
            b->vtbl = SafeString_vtbl;
            b->str = gabi::ea(frameEventName.value);
            frameAssureTermination(a.get());
            gabi::call_ptr<void>(gabi::at<FrameStringVtable>(a->vtbl)->resolve.get(), a.get());
            u32 bv = b->vtbl;
            u32 s1 = a->str;
            gabi::call_ptr<void>(gabi::at<FrameStringVtable>(bv)->resolve.get(), b.get());
            u32 s2 = b->str;
            bool eq = s1 == s2;
            if (!eq) {
                for (u32 n = 0x40001; n != 0; n--) {
                    u8 c1 = u8(*gabi::at<be<u8>>(s1)), c2 = u8(*gabi::at<be<u8>>(s2));
                    if (c1 != c2)
                        break;
                    if (c1 == 0) {
                        eq = true;
                        break;
                    }
                    s1++;
                    s2++;
                }
            }
            if (eq && (s32)u32(frameEventFrame.value) >= 0x262) {
                this->monotoneBlend() = one;
                apply(pa, pb, one);
                return;
            }
            apply(pa, pb, f32(this->monotoneBlend()));
            return;
        }
        apply(tbl->mpMonotone.get(), 0, one);
        return;
    }
    if (frameIsEventBit(&frameSave.value->mTmp, 0x408)) {
        apply(tbl->mpEvent.get(), 0, one);
        return;
    }
    if (u8(frameHdOptions.mOverride) == 0 && f32(framePlay()->mLightOverride) != one) {
        apply(tbl->mpOverride.get(), 0, one);
        return;
    }
    if (dKy_daylight_preset_chk_hd()) {
        apply(day, 0, one);
        return;
    }
    const f32 k30 = f32(frameTwilightSpan.value);
    if ((u32)(hour - 6) < 2) {
        apply(night, day, (f32(this->mCurTime) - f32(frameMorningTime.value)) / k30);
        return;
    }
    if ((u32)(hour - 18) < 2) {
        apply(day, night, (f32(this->mCurTime) - f32(frameEveningTime.value)) / k30);
        return;
    }
    if ((u32)(hour - 7) >= 13) {
        apply(night, 0, one);
        return;
    }
    if ((u32)(hour - 8) < 11)
        apply(day, 0, one);
}

// HD: arrow color changes are inlined here, with revised colors and fade speeds.
/* 0255A924 */

void d_kankyo_cpp::dScnKy_env_light_c::drawKankyo() {

    dScnKy_env_light_c_setSunpos(gabi::ea(this));
    dScnKy_env_light_c_setLightPos_hd(gabi::ea(this));
    dScnKy_env_light_c_SetBaseLight(gabi::ea(this));
    dScnKy_env_light_c_setLight(gabi::ea(this));
    dKy_Itemgetcol_chg_move();
    auto *env = frameEnv();
    u8 flag = u8(env->mColChgFlag);
    gabi::Local<be<s16>[8]> col;
    auto setc = [&](s16 r0, s16 g0, s16 b0, s16 r1, s16 g1, s16 b1) {
        (*col)[0] = r0;
        (*col)[1] = g0;
        (*col)[2] = b0;
        (*col)[4] = r1;
        (*col)[5] = g1;
        (*col)[6] = b1;
    };
    if (flag == 0x10 || flag == 0x40) {
        setc(0xFF, 0x50, 0x14, 0xFF, 0x50, 0x14);
    } else if (flag == 0x20 || flag == 0x50) {
        setc(0x1E, 0xA0, 0xFF, 0x1E, 0xA0, 0xFF);
    } else if (flag == 0x30 || flag == 0x60) {
        if (stageNameEquals("GTower"))
            setc(0x45, 0x1B, 0, 0x74, 0x65, 0x10);
        else
            setc(0xE1, 0xFF, 0x3C, 0xE1, 0xFF, 0x3C);
        flag = u8(env->mColChgFlag);
    }
    if (flag == 0x10 || flag == 0x20 || flag == 0x30) {
        const f32 one = f32(frameOne.value);
        frameAddCalc(&env->field_0xc34, one, f32(frameHalf.value), f32(frameArrowFadeInStep.value),
                     f32(frameMinimumStep.value));
        if (!(f32(env->field_0xc34) < one)) {
            flag = (u8)(u8(env->mColChgFlag) + 0x30);
            env->mColChgFlag = flag;
        } else {
            flag = u8(env->mColChgFlag);
        }
    } else if (flag == 0x40 || flag == 0x50 || flag == 0x60) {
        const f32 zero = f32(frameZero.value);
        frameAddCalc(&env->field_0xc34, zero, f32(frameHalf.value), f32(frameArrowFadeOutStep.value),
                     f32(frameMinimumStep.value));
        if (f32(env->field_0xc34) < f32(frameArrowFadeEnd.value)) {
            env->mColChgFlag = 0;
            env->field_0xc34 = zero;
            return;
        }
        flag = u8(env->mColChgFlag);
    }
    if ((flag & 0xF0) == 0)
        return;
    s32 cr = (*col)[0], cg = (*col)[1], cb = (*col)[2];
    dKy_actor_addcol_amb_set(subtractColorByte(cr, &env->colors().mActorC0.r),
                             subtractColorByte(cg, &env->colors().mActorC0.g),
                             subtractColorByte(cb, &env->colors().mActorC0.b), f32(env->field_0xc34));
    dKy_bg_addcol_amb_set(subtractColorByte(cr, &env->colors().mBG0_C0.r),
                          subtractColorByte(cg, &env->colors().mBG0_C0.g),
                          subtractColorByte(cb, &env->colors().mBG0_C0.b), f32(env->field_0xc34));
    s32 r3, g3, b3;
    if (stageNameEquals("GTower")) {
        dKy_bg2_addcol_amb_set(subtractColorByte(cr, &env->colors().mBG2_C0.r),
                               subtractColorByte(cg, &env->colors().mBG2_C0.g),
                               subtractColorByte(cb, &env->colors().mBG2_C0.b), f32(env->field_0xc34));
    } else {
        dKy_bg1_addcol_amb_set(subtractColorByte(cr, &env->colors().mBG1_C0.r),
                               subtractColorByte(cg, &env->colors().mBG1_C0.g),
                               subtractColorByte(cb, &env->colors().mBG1_C0.b),
                               f32(env->field_0xc34) * f32(frameBackgroundAmbientRatio.value));
        dKy_bg2_addcol_amb_set(subtractColorByte(cr, &env->colors().mBG2_C0.r),
                               subtractColorByte(cg, &env->colors().mBG2_C0.g),
                               subtractColorByte(cb, &env->colors().mBG2_C0.b), f32(env->field_0xc34));
    }
    r3 = subtractColorByte(cr, &env->colors().mBG3_C0.r);
    g3 = subtractColorByte(cg, &env->colors().mBG3_C0.g);
    b3 = subtractColorByte(cb, &env->colors().mBG3_C0.b);
    if (stageNameEquals("GTower")) {
        r3 = subtractColorByte(0xB7, &env->colors().mBG3_C0.r);
        g3 = subtractColorByte(0xB0, &env->colors().mBG3_C0.g);
        b3 = subtractColorByte(0x88, &env->colors().mBG3_C0.b);
    }
    dKy_bg3_addcol_amb_set(r3, g3, b3, f32(env->field_0xc34));
    s32 kr = (*col)[4], kg = (*col)[5], kb = (*col)[6];
    dKy_actor_addcol_dif_set(subtractColorByte(kr, &env->colors().mActorK0.r),
                             subtractColorByte(kg, &env->colors().mActorK0.g),
                             subtractColorByte(kb, &env->colors().mActorK0.b), f32(env->field_0xc34));
    dKy_bg_addcol_dif_set(subtractColorByte(kr, &env->colors().mBG0_K0.r),
                          subtractColorByte(kg, &env->colors().mBG0_K0.g),
                          subtractColorByte(kb, &env->colors().mBG0_K0.b), f32(env->field_0xc34));
    if (stageNameEquals("GTower")) {
        dKy_bg2_addcol_dif_set(subtractColorByte(kr, &env->colors().mBG2_K0.r),
                               subtractColorByte(kg, &env->colors().mBG2_K0.g),
                               subtractColorByte(kb, &env->colors().mBG2_K0.b), f32(env->field_0xc34));
    } else {
        dKy_bg1_addcol_dif_set(subtractColorByte(kr, &env->colors().mBG1_K0.r),
                               subtractColorByte(kg, &env->colors().mBG1_K0.g),
                               subtractColorByte(kb, &env->colors().mBG1_K0.b),
                               f32(env->field_0xc34) * f32(frameBackgroundDiffuseRatio.value));
        dKy_bg2_addcol_dif_set(subtractColorByte(kr, &env->colors().mBG2_K0.r),
                               subtractColorByte(kg, &env->colors().mBG2_K0.g),
                               subtractColorByte(kb, &env->colors().mBG2_K0.b), f32(env->field_0xc34));
    }
    r3 = subtractColorByte(kr, &env->colors().mBG3_K0.r);
    b3 = subtractColorByte(kb, &env->colors().mBG3_K0.b);
    g3 = subtractColorByte(kg, &env->colors().mBG3_K0.g);
    if (stageNameEquals("GTower")) {
        r3 = subtractColorByte(0x8A, &env->colors().mBG3_C0.r);
        g3 = subtractColorByte(0x8A, &env->colors().mBG3_C0.g);
        b3 = subtractColorByte(0x44, &env->colors().mBG3_C0.b);
    }
    dKy_bg3_addcol_dif_set(r3, g3, b3, f32(env->field_0xc34));
}

/* 0255B2D8 */

BOOL dKy_Draw() {

    frameEnv()->drawKankyo();
    return TRUE;
}

// HD: dKy_event_proc is inlined; thunder also drives the sky brightness.
/* 0255D128 */

BOOL dKy_Execute() {

    auto *env0 = frameEnv();
    bool checkColorPatternMode = false, checkColorPatternBlend = false;
    if (stageNameEquals("sea")) {
        u32 weatherColor = 0;
        auto modeChk = [&]() {
            if (u8(frameEnv()->mColPatMode) == 0)
                checkColorPatternBlend = true;
        };
        if (dKy_checkEventNightStop()) {
            u8 w = u8(env0->mColpatWeather);
            s32 rain = (s32)u32(env0->mRainCount);
            if (w != 1) {
                env0->mColpatWeather = 1;
                env0->mColpatCurrGather = 1;
            }
            if (rain < 250) {
                env0->mRainCount = (u32)(rain + 1);
                dKyw_rain_set(rain + 1);
                modeChk();
            } else {
                checkColorPatternMode = true;
            }
        } else if (u8(frameEnv()->mInitAnimTimer) != 0) {
            checkColorPatternMode = true;
        } else {
            framePlay();
            if (u8(frameTimePass.value) != 0 && frameIsGetItem(&frameSave.value->mItems, 2, 0)) {
                if (phantomship_wether()) {
                    frameEnv()->mThunderEff.mMode = 1;
                    dice_rain_minus();
                    if (u8(env0->mColpatWeather) == 1) {
                        checkColorPatternMode = true;
                    } else {
                        frameEnv()->mColpatWeather = 1;
                        frameEnv()->mColpatCurrGather = 1;
                        modeChk();
                    }
                } else {
                    f32 cur = f32(frameSave.value->mTime);
                    f32 chg = f32(env0->mDiceWeatherChangeTime);
                    const f32 k180 = f32(frameHalfDay.value);
                    u8 st_;
                    if (cur > chg && cur - chg < k180) {
                        st_ = u8(env0->mDiceWeatherState);
                        if (st_ == 0) {
                            env0->mDiceWeatherState = 1;
                            st_ = 1;
                        }
                    } else {
                        st_ = u8(env0->mDiceWeatherState);
                    }
                    const f32 k799 = f32(frameWeatherChoiceRange.value);
                    bool checkWeatherColorMode = false;
                    switch (st_) {
                    case DICE_STATE_RESET_e:
                        checkWeatherColorMode = true;
                        break;
                    case DICE_STATE_INIT_e: {
                        f32 r = frameRandom(k799);
                        u8 idx = (u8)gabi::ftoi(r);
                        u8 pat = u8(frameWeatherChoices.values[idx]);
                        env0->mDiceWeatherCurrPattern = pat;
                        env0->mDiceWeatherCounter = 0;
                        if (pat <= 3) {
                            if (pat == 0)
                                dice_wether_init(u8(frameWeatherModes0.values[0]), f32(frameWeatherTimes0.values[0]),
                                                 cur);
                            else if (pat == 1)
                                dice_wether_init(u8(frameWeatherModes1.values[0]), f32(frameWeatherTimes1.values[0]),
                                                 cur);
                            else if (pat == 2)
                                dice_wether_init(u8(frameWeatherModes2.values[0]), f32(frameWeatherTimes2.values[0]),
                                                 cur);
                            else
                                dice_wether_init(u8(frameWeatherModes3.values[0]), f32(frameWeatherTimes3.value), cur);
                            u32 cnt = u32(env0->mDiceWeatherCounter);
                            u8 s2 = u8(env0->mDiceWeatherState);
                            env0->mDiceWeatherCounter = cnt + 1;
                            env0->mDiceWeatherState = (u8)(s2 + 1);
                        } else {
                            u8 s2 = u8(env0->mDiceWeatherState);
                            env0->mDiceWeatherCounter = 1;
                            env0->mDiceWeatherState = (u8)(s2 + 1);
                        }
                        if (u8(frameEnv()->mColPatMode) != 0)
                            checkColorPatternMode = true;
                        break;
                    }
                    case DICE_STATE_EXEC_e: {
                        f32 wt = f32(env0->mDiceWeatherTime);
                        if (cur > wt && cur - wt < k180) {
                            u8 pat = u8(env0->mDiceWeatherCurrPattern);
                            if (pat <= 3) {
                                u32 cnt = u32(env0->mDiceWeatherCounter);
                                if (pat == 0)
                                    dice_wether_execute(u8(frameWeatherModes0.values[cnt]),
                                                        f32(frameWeatherTimes0.values[cnt]), cur);
                                else if (pat == 1)
                                    dice_wether_execute(u8(frameWeatherModes1.values[cnt]),
                                                        f32(frameWeatherTimes1.values[cnt]), cur);
                                else if (pat == 2)
                                    dice_wether_execute(u8(frameWeatherModes2.values[cnt]),
                                                        f32(frameWeatherTimes2.values[cnt]), cur);
                                else
                                    dice_wether_execute(u8(frameWeatherModes3.values[cnt]),
                                                        f32(frameWeatherTimes3.value), cur);
                            }
                            if (u8(frameEnv()->mColPatMode) != 0)
                                checkColorPatternMode = true;
                        } else {
                            checkWeatherColorMode = true;
                        }
                        break;
                    }
                    case DICE_STATE_NEXT_e: {
                        f32 r = frameRandom(k799);
                        u8 idx = (u8)gabi::ftoi(r);
                        f32 t = cur + f32(frameWeatherDelays.values[idx]);
                        const f32 k360 = f32(frameDayLength.value);
                        if (!(t < k360))
                            t = t - k360;
                        env0->mDiceWeatherState = (u8)weatherColor;
                        env0->mDiceWeatherChangeTime = t;
                        checkWeatherColorMode = true;
                        break;
                    }
                    default:
                        if (u8(frameEnv()->mColPatMode) != 0)
                            checkColorPatternMode = true;
                        break;
                    }
                    if (checkWeatherColorMode && u8(frameEnv()->mColPatMode) != 0)
                        checkColorPatternMode = true;
                    if (!checkColorPatternMode) {
                        if (u8(frameEnv()->mColPatModeGather) != 0) {
                            checkColorPatternMode = true;
                        } else {
                            u8 mode = u8(env0->mDiceWeatherMode);
                            u32 w = 0;
                            bool set = false;
                            switch (mode) {
                            case DICE_MODE_SUNNY_e:
                                w = 0;
                                if (u32(frameEnv()->mThunderEff.mMode) == 1)
                                    frameEnv()->mThunderEff.mMode = weatherColor;
                                dice_rain_minus();
                                set = u8(env0->mColpatWeather) != w;
                                break;
                            case DICE_MODE_CLOUDY_e:
                                w = 1;
                                dice_rain_minus();
                                set = u8(env0->mColpatWeather) != 1;
                                break;
                            case DICE_MODE_RAIN_LIGHT_e: {
                                s32 rain = (s32)u32(env0->mRainCount);
                                w = 1;
                                if (rain < 40) {
                                    env0->mRainCount = (u32)(rain + 1);
                                    dKyw_rain_set(rain + 1);
                                } else {
                                    env0->mRainCount = (u32)(rain - 1);
                                    dKyw_rain_set(rain - 1);
                                }
                                set = u8(env0->mColpatWeather) != w;
                                break;
                            }
                            case DICE_MODE_THUNDER_HEAVY_e:
                                frameEnv()->mThunderEff.mMode = 1;

                            case DICE_MODE_RAIN_HEAVY_e: {
                                s32 rain = (s32)u32(env0->mRainCount);
                                w = 1;
                                if (rain < 250) {
                                    env0->mRainCount = (u32)(rain + 1);
                                    dKyw_rain_set(rain + 1);
                                }
                                set = u8(env0->mColpatWeather) != w;
                                break;
                            }
                            case DICE_MODE_THUNDER_LIGHT_e:
                                w = 1;
                                frameEnv()->mThunderEff.mMode = w;
                                dice_rain_minus();
                                set = u8(env0->mColpatWeather) != w;
                                break;
                            default:
                                frameAssert("d_kankyo.cpp", 0x291A, "0");
                                w = u8(env0->mColpatWeather);
                                set = false;
                                break;
                            }
                            if (set) {
                                frameEnv()->mColpatWeather = (u8)w;
                                frameEnv()->mColpatCurrGather = (u8)w;
                                modeChk();
                            } else {
                                checkColorPatternMode = true;
                            }
                        }
                    }
                }
            } else {
                if (phantomship_wether()) {
                    if (u8(frameEnv()->mColpatWeather) != 1) {
                        weatherColor = 1;
                        frameEnv()->mColpatWeather = 1;
                        frameEnv()->mColpatCurrGather = 1;
                    }
                    frameEnv()->mThunderEff.mMode = 1;
                    modeChk();
                } else {
                    if (u8(frameEnv()->mColpatWeather) != 0) {
                        frameEnv()->mColpatWeather = (u8)weatherColor;
                        frameEnv()->mColpatCurrGather = (u8)weatherColor;
                    }
                    if (u32(frameEnv()->mThunderEff.mMode) == 1)
                        frameEnv()->mThunderEff.mMode = weatherColor;
                    dice_rain_minus();
                    checkColorPatternMode = true;
                }
            }
        }
        if (checkColorPatternMode) {
            if (u8(frameEnv()->mColPatMode) == 0)
                checkColorPatternBlend = true;
        }
        if (checkColorPatternBlend) {
            if (u8(frameEnv()->mColPatModeGather) == 0 && u8(frameEnv()->mColpatCurrGather) != 0xFF) {
                auto *e1 = frameEnv();
                u8 g = u8(frameEnv()->mColpatCurrGather);
                if (u8(e1->mColpatCurr) != g)
                    frameEnv()->mColPatBlendGather = f32(frameZero.value);
            }
        }
    }
    frameEnv()->exeKankyo();
    dKyw_wind_set();
    auto *th = &frameEnv()->mThunderEff;
    f32 target = f32(frameOne.value);
    if (u8(frameEnv()->mColpatWeather) != 0) {
        u8 state = u8(th->mState);
        f32 k = f32(frameStormSkyBrightness.value);
        target = k;
        if (state == 0 || state >= 10) {
            f32 v = k * f32(th->mFlashTimer);
            frameEnv()->skyBrightness() = v + k;
        } else {
            f32 v = f32(frameBackgroundAmbientRatio.value) * f32(th->mFlashTimer) * f32(th->field_0xc);
            frameEnv()->skyBrightness() = v + k;
        }
    }
    f32 step = f32(frameSmallStep.value);
    auto *e = frameEnv();
    frameAddCalc(&e->skyBrightness(), target, step, step, f32(frameMinimumStep.value));
    return TRUE;
}

/* 0255DA4C */

BOOL dKy_IsDelete(u32 p) { return TRUE; }

/* 0255DA54 */

BOOL dKy_Delete(u32 p) {

    plight_init();
    return TRUE;
}

/* 0255DA78 */

void dKy_setLight_init() {

    for (u32 i = 0; i < 8; i++) {
        auto &dst = lightStatusData.values[i];
        // HD: integer word copies preserve NaN payloads and the verified store order.
        for (u32 j = 0; j < 0xE8 / 4; ++j)
            dst.words[j] = u32(lightStatusBase.words[j]);
    }
}

/* 0255A2B8 */
void dKy_plight_priority_set(u32 light) {

    for (u32 i = 0; i < 200; i++) {
        if (u32(frameEnv()->mpPLights[i].v) == 0) {
            frameEnv()->mpPLights[i].v = light;
            frameEnv()->mpPLights[i]->mIdx = -(s32)(i + 1);
            return;
        }
    }
    // HD: the guest warning includes leading/trailing newlines; its address remains unchanged.
    frameReportWarning("POINTLIGHT COUNT OVER!!!");
}

/* 0255A374 */
void dKy_plight_cut_hd(u32 light) {

    if (light == 0)
        return;
    s32 idx = (s32)u32(gabi::at<dKankyo::LIGHT_INFLUENCE>(light)->mIdx);
    if (idx <= 0) {
        if (idx == 0)
            return;
        idx = (s32)(0u - (u32)idx);
        gabi::at<dKankyo::LIGHT_INFLUENCE>(light)->mIdx = (u32)idx;
    }
    s32 i = (s32)((u32)idx - 1);
    if (i < 200)
        frameEnv()->mpPLights[i].v = 0;
}

/* 0255B9C8 */

void dKy_efplight_set_hd(u32 light) {

    for (u32 i = 0; i < 10; i++)
        if (u32(frameEnv()->mpEfLights[i].v) == light)
            return;
    for (u32 i = 0; i < 10; i++) {
        if (u32(frameEnv()->mpEfLights[i].v) == 0) {
            frameEnv()->mpEfLights[i].v = light;
            frameEnv()->mpEfLights[i]->mIdx = i + 1;
            return;
        }
    }
}

/* 0255BA9C */
void dKy_efplight_cut_hd(u32 light) {

    if (light == 0)
        return;
    u32 idx = u32(gabi::at<dKankyo::LIGHT_INFLUENCE>(light)->mIdx);
    if (idx == 0)
        return;
    u32 i = idx - 1;
    if (i < 10)
        frameEnv()->mpEfLights[i].v = 0;
}

// HD: RGB rises by eight to255; power200 and gather-ratio fades differ from GameCube.
/* 0255A568 */

void dKy_Itemgetcol_chg_move() {

    auto *cam = framePlay()->mpCamera.get();
    auto *env = frameEnv();
    auto *player = framePlay()->mpPlayer.get();
    const f32 one = f32(frameOne.value), half = f32(frameHalf.value), zero = f32(frameZero.value);
    u8 flag = u8(env->mColChgFlag);
    bool fade = false;
    switch (flag) {
    case 0:
        break;
    case 1:
        env->mColChgLight.mColor.r = 0;
        env->mColChgFlag = 2;
        break;
    case 2:
    case 3:
    case 4:
        if (flag == 2) {
            f32 y = f32(player->current.pos.y), z = f32(player->current.pos.z), x = f32(player->current.pos.x);
            s32 offsAngle, offsY;
            if (toon_proc_check()) {
                offsAngle = 13000;
                offsY = -70;
            } else {
                offsY = 280;
                offsAngle = 23000;
            }
            gabi::Local<cXyz> v;
            frameGetVector(&cam->mEye, &cam->mCenter, v.get());

            f32 vz = f32(v->z), vx = f32(v->x);
            frameSqrt(gabi::fmadds(vx, vx, vz * vz));
            s16 a = frameAtan2s(f32(v->x), f32(v->z));
            u32 ang = (u16)(a - offsAngle);
            auto *tbl = frameSinCos.values;
            f32 c0 = f32(tbl[0].cosine);
            f32 fx = c0 * f32(tbl[ang >> 3].sine);
            f32 fz = c0 * f32(tbl[ang >> 3].cosine);
            v->x = fx;
            v->z = fz;
            v->y = f32(tbl[0].sine);
            const f32 k80 = f32(frameItemLightOffset.value);
            y = y + i2f(offsY);
            env->mColChgLight.mColor.r = 0;
            x = gabi::fmadds(k80, fx, x);
            env->mColChgLight.mColor.g = 0;
            env->mColChgLight.mPos.y = y;
            env->mColChgLight.mPos.x = x;
            env->mColChgLight.mPower = f32(frameItemLightPower.value);
            env->mColChgLight.mFluctuation = zero;
            env->mColChgLight.mColor.b = 0;
            env->mColChgLight.mPos.z = gabi::fmadds(k80, fz, z);
            dKy_plight_priority_set(gabi::ea(&env->mColChgLight.mPos.x));
            env->mColChgFlag = 3;
        }
        if (flag <= 3) {
            s16 r = (s16)(s16(env->mColChgLight.mColor.r) + 8);
            s16 g = (s16)(s16(env->mColChgLight.mColor.g) + 8);
            s16 b = (s16)(s16(env->mColChgLight.mColor.b) + 8);
            env->mColChgLight.mColor.r = (u16)r;
            env->mColChgLight.mColor.g = (u16)g;
            env->mColChgLight.mColor.b = (u16)b;
            if ((s32)r + 8 > 0xFF) {
                env->mColChgFlag = 4;
                env->mColChgLight.mColor.r = 0xFF;
                env->mColChgLight.mColor.g = 0xFF;
                env->mColChgLight.mColor.b = 0xFF;
            }
        }
        frameAddCalc(&env->field_0xc34, one, half, f32(frameItemFadeInStep.value), f32(frameMinimumStep.value));
        break;
    case 5:
    case 6:
        if (flag == 5) {
            dKy_plight_cut_hd(gabi::ea(&env->mColChgLight.mPos.x));
            env->mColChgFlag = 6;
        }
        frameAddCalc(&env->field_0xc34, zero, half, f32(frameItemFadeOutStep.value), f32(frameSmallStep.value));
        if (f32(env->field_0xc34) < f32(frameItemFadeEnd.value)) {
            env->mColChgFlag = 0;
            env->field_0xc34 = zero;
        }
        break;
    default:
        break;
    }
    flag = u8(env->mColChgFlag);
    env->field_0xc98 = 0;
    fade = (flag & 7) != 0;
    if (!fade)
        return;
    f32 s = f32(env->field_0xc34);
    env->field_0xc98 = 1;
    dKy_set_actcol_ratio(gabi::fnmsubs(s, half, one));
    const f32 k = f32(frameItemRatio.value);
    dKy_set_bgcol_ratio(gabi::fnmsubs(f32(env->field_0xc34), k, one));
    dKy_set_fogcol_ratio(gabi::fnmsubs(f32(env->field_0xc34), k, one));
    dKy_set_vrboxcol_ratio(gabi::fnmsubs(f32(env->field_0xc34), k, one));
    dKy_set_vrboxsoracol_ratio(gabi::fnmsubs(f32(env->field_0xc34), k, one));
    dKy_set_vrboxkumocol_ratio(gabi::fnmsubs(f32(env->field_0xc34), k, one));
    s16 nr = (s16)(0 - (s32)u8(env->colors().mActorC0.r)), ng = (s16)(0 - (s32)u8(env->colors().mActorC0.g)),
        nb = (s16)(0 - (s32)u8(env->colors().mActorC0.b));
    dKy_actor_addcol_amb_set(nr, ng, nb, f32(env->field_0xc34));
    dKy_actor_addcol_dif_set(0, 0, 0, f32(env->field_0xc34));
}

/* 0255B930 */

void dKy_Sound_init() {

    const f32 far = f32(frameSoundFar.value);
    frameEnv()->mSound.field_0x0.x = far;
    frameEnv()->mSound.field_0x0.y = far;
    frameEnv()->mSound.field_0x0.z = far;
    frameEnv()->mSound.field_0xc = 0;
    frameEnv()->mSound.field_0x14 = (u32)-1;
    frameEnv()->mSound.field_0x10 = 0;
}

/* 0255A3D8 */
void dKy_set_actcol_ratio(f32 ratio) { frameEnv()->mActColGatherRatio = ratio; }

/* 0255A418 */
void dKy_set_bgcol_ratio(f32 ratio) { frameEnv()->mBgColGatherRatio = ratio; }

/* 0255A458 */

void dKy_set_fogcol_ratio(f32 ratio) {

    frameEnv();
    frameEnv()->mFogColGatherRatio = ratio;
}

/* 0255A49C */
void dKy_set_vrboxsoracol_ratio(f32 ratio) { frameEnv()->mVrboxSoraColGatherRatio = ratio; }

/* 0255A4DC */
void dKy_set_vrboxkumocol_ratio(f32 ratio) { frameEnv()->mVrboxKumoColGatherRatio = ratio; }

/* 0255A51C */
void dKy_set_vrboxcol_ratio(f32 ratio) {

    frameEnv();
    dKy_set_vrboxsoracol_ratio(ratio);
    dKy_set_vrboxkumocol_ratio(ratio);
}

// HD: determine whether the current stage uses the time-of-day preset pair.
/* 0255BE0C */

BOOL dKy_daylight_preset_chk_hd() {

    BOOL ret = FALSE;
    if (dComIfGp_getStageStagInfo() == 0)
        return FALSE;
    auto *info = static_cast<FrameStageInfo *>(dComIfGp_getStageStagInfo());
    u32 type = (u32(info->mParameters) >> 16) & 7;
    bool special = true;
    if (stageNameEquals("ADMumi") || stageNameEquals("Mjtower") || stageNameEquals("M2tower") ||
        stageNameEquals("Ocrogh") || stageNameEquals("ADMumi") || stageNameEquals("ENDumi") ||
        stageNameEquals("M_Dra09")) {
    } else if (stageNameEquals("Hyrule") && dComIfGp_roomControl_getStayNo() == 0) {
    } else if (stageNameEquals("M_NewD2") && dComIfGp_roomControl_getStayNo() == 3) {
    } else if (stageNameEquals("Siren") && dComIfGp_roomControl_getStayNo() == 0x12) {
    } else if (stageNameEquals("kinMB") && dComIfGp_roomControl_getStayNo() == 0xA) {
    } else if (stageNameEquals("kindan")) {
        s32 room = dComIfGp_roomControl_getStayNo();
        if (!(room == 2 || room == 4 || room == 0xD))
            special = false;
    } else {
        special = false;
    }
    if (!special && ((type >= 1 && type <= 4) || type == 6))
        ret = TRUE;
    return ret;
}

// HD: cross-part bridges retain the verified split-TU ABI.
void dScnKy_env_light_c_drawKankyo(u32 t) { gabi::at<dScnKy_env_light_c>(t)->drawKankyo(); }
void dScnKy_env_light_c_setDaytime(u32 t) { gabi::at<dScnKy_env_light_c>(t)->setDaytime(); }
void dScnKy_env_light_c_CalcTevColor(u32 t) { gabi::at<dScnKy_env_light_c>(t)->CalcTevColor(); }
void dScnKy_env_light_c_Sndpos(u32 t) { gabi::at<dScnKy_env_light_c>(t)->Sndpos(); }
void dScnKy_env_light_c_Eflight_flush_proc(u32 t) { gabi::at<dScnKy_env_light_c>(t)->Eflight_flush_proc(); }
void dScnKy_env_light_c_SetSchbit(u32 t) { gabi::at<dScnKy_env_light_c>(t)->SetSchbit(); }
void dScnKy_env_light_c_exeKankyo(u32 t) { gabi::at<dScnKy_env_light_c>(t)->exeKankyo(); }
void dKy_plight_cut(u32 light) { dKy_plight_cut_hd(light); }
void dKy_efplight_set(u32 light) { dKy_efplight_set_hd(light); }
void dKy_efplight_cut(u32 light) { dKy_efplight_cut_hd(light); }
} // namespace d_kankyo_cpp

using namespace d_kankyo_cpp;

/* WWHD glue
data frameLightPresets 101F8664
data frameSceneLights 101F95D0
data frameHdOptions 101F4820
data frameEventFrame 101D6008
data frameEventName 1047E6B8
data frameWeatherChoices 101E8CC0
data frameWeatherModes0 101E8CCC
data frameWeatherModes1 101E8CD0
data frameWeatherModes2 101E8CD4
data frameWeatherModes3 101E8CDE
data frameWeatherTimes0 101E8D24
data frameWeatherTimes1 101E8D30
data frameWeatherTimes2 101E8D3C
data frameWeatherTimes3 101E8CD8
data frameWeatherDelays 101E8CF0
data lightStatusBase 101E8D48
data lightStatusData 10476C74
str 1004F2B0 "Demo49" in 0255C85C
data frameOne 1004F0A4
data frameHalf 1004F1C8
data frameZero 1004F0B0
data frameItemLightOffset 1004F1FC
data frameItemLightPower 1004F0C4
data frameItemFadeInStep 1004F17C
data frameMinimumStep 1004F178
data frameItemFadeOutStep 1004F138
data frameSmallStep 1004F0BC
data frameItemFadeEnd 1004F200
data frameItemRatio 1004F204
data frameArrowFadeInStep 1004F208
data frameArrowFadeOutStep 1004F20C
data frameArrowFadeEnd 1004F210
data frameBackgroundAmbientRatio 1004F214
data frameBackgroundDiffuseRatio 1004F218
data frameEveningTime 1004F21C
data frameUnsignedFloatBias 1004F220
data frameDayLength 1004F108
data frameDayStopTime 1004F12C
data frameNightStopTime 1004F118
data frameHalfDay 1004F10C
data frameSoundFar 1004F238
data frameSwordLightPower 1004F23C
data frameSwordLightFluctuation 1004F240
data frameNoBlend 1004F0B4
data frameHoursPerDegree 1004F1A0
data frameMonotoneRange 1004F29C
data frameMonotoneFrameStep 1047BA08
data frameMonotoneExtraStep 1004F2A0
data frameTwilightSpan 1004F2A4
data frameMorningTime 1004F114
data frameWeatherChoiceRange 1004F2B8
data frameStormSkyBrightness 1004F2BC
str 1004EF74 "GTower" in 0255A924
str 1004F224 "A_umikz" in 0255B300
str 1004F244 "Mjtower" in 0255BE0C
str 1004F24C "M2tower" in 0255BE0C
str 1004F27C "Ocrogh" in 0255BE0C
str 1004F284 "ENDumi" in 0255BE0C
str 1004F254 "M_Dra09" in 0255BE0C
str 1004F28C "Hyrule" in 0255BE0C
str 1004F25C "M_NewD2" in 0255BE0C
str 1004F264 "Siren" in 0255BE0C
str 1004F26C "kinMB" in 0255BE0C
str 1004F294 "kindan" in 0255BE0C
str 1004F2A8 "Hyroom" in 0255C85C
str 1004F22C "sea" in 0255B300
str 1004EF54 "sea" in 0255D128
str 1004F230 "ENDumi" in 0255B300
str 1004F274 "ADMumi" in 0255BE0C
str 1004F1E0 "POINTLIGHT COUNT OVER!!!" renamed in 0255A2B8
str 1004F094 "d_kankyo.cpp" in 0255D128
str 1004EF60 "0" in 0255D128
data frameSave 101F84DC
data frameTimePass 101EBFD8
data frameFlushColors 101E8EA8
data frameFlushColors2 101E8EB8
data frameSinCos 104A44F8
ext 02555D0C d_kankyo_cpp::dScnKy_env_light_c* frameEnv()
ext 025200D4 d_kankyo_cpp::FramePlay* framePlay()
ext 025B8B94 BOOL frameIsEventBit(void*,u32)
ext 0200ECD4 f32 frameAddCalc(be<f32>*,f32,f32,f32,f32)
ext 025F27E8 void frameReportWarning(const char*)
ext 02563F64 void frameGetVector(cXyz*,cXyz*,cXyz*)
ext 028F4384 f32 frameSqrt(f32)
ext 020195B0 s16 frameAtan2s(f32,f32)
ext 025B5D40 BOOL frameIsGetItem(void*,u32,u32)
ext 025E1F80 void mDoAud_setHour(s32)
ext 025E1F90 void mDoAud_setMinute(s32)
ext 025E1FA0 void mDoAud_setWeekday(s32)
ext 0257D208 void dKyw_wether_proc()
ext 0272FEE0 void frameApplyLightPresets(void*,void*,void*,f32)
ext 025F0840 void frameCalcMonotone()
ext 0200F5C8 BOOL frameChaseF(be<f32>*,f32,f32)
ext 025638D4 void frameAssureTermination(d_kankyo_cpp::FrameString*)
ext 020198D8 f32 frameRandom(f32)
ext 0257D398 void dKyw_wind_set()
ext 0257E7C0 void dKyw_rain_set(s32)
ext 0273AA24 void frameAssert(const char*,s32,const char*)
*/
