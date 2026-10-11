/* d_kankyo: environment / lighting core (dKy_*, dScnKy_env_light_c), part 1 of 6, WWHD.
 *
 * Translation unit 02554288..025638D4 (one TU), from the image: d_jnt_hit ends with its __sinit
 * (025541F4); d_kankyo starts with dKy_get_dayofweek (02554288); its __sinit is 02563720
 * (header statics 10475A4C.., kankyo statics 104773B8..104773BB), followed by the per-TU inline
 * copies (LIGHT_INFLUENCE constructor 025637D4, constructors 02563814 / 02563840, destructors
 * 0256386C / 02563880, an empty virtual 025638D4); d_kankyo_data starts at 025638D8.
 * The HD function order differs from the GameCube object file.
 * Parts: d_kankyo.cpp 02554288..025560C0 (day change, addcol setters, the env light constructor
 * and accessor, colour ratio helpers), d_kankyo_2.cpp 025560C0..02557228, d_kankyo_3.cpp
 * 02557228..0255A2B8, d_kankyo_4.cpp 0255A2B8..0255DAC0, d_kankyo_5.cpp 0255DAC0..02560E10,
 * d_kankyo_6.cpp 02560E10..025638D8. */
#include "d/d_kankyo.h"

namespace dSv_event_flag_c {
enum : u16 {
    UNK_0B80 = 0xb80,
    UNK_1301 = 0x1301,
    UNK_1302 = 0x1302,
    UNK_1304 = 0x1304,
    UNK_1820 = 0x1820,
    UNK_1F08 = 0x1f08,
    UNK_1F10 = 0x1f10,
    UNK_2002 = 0x2002,
    UNK_2004 = 0x2004,
    UNK_2080 = 0x2080,
    UNK_2680 = 0x2680,
    UNK_2801 = 0x2801,
    UNK_2802 = 0x2802,
    UNK_2804 = 0x2804,
    UNK_2940 = 0x2940,
    UNK_2980 = 0x2980,
    GRANDMA_HEALED = 0x2a20,
    UNK_2F01 = 0x2f01,
    UNK_3080 = 0x3080,
    UNK_3B01 = 0x3b01,
    UNK_3C02 = 0x3c02,
    UNK_3C04 = 0x3c04,
    UNK_3C08 = 0x3c08,
    UNK_3C10 = 0x3c10,
    UNK_3C20 = 0x3c20,
    UNK_3C40 = 0x3c40,
    UNK_3C80 = 0x3c80,
    LETTER_ARYLL = 0x8b03,
    LETTER_GRANDMA = 0x9d03,
    UNK_A60F = 0xa60f,
    UNK_AB03 = 0xab03,
    LETTER_BAITOS_MOM = 0xac03,
    LETTER_HOSKITS_GIRLFRIEND = 0xae03,
    LETTER_GOLD_MEMBERSHIP = 0xaf03,
    LETTER_SILVER_MEMBERSHIP = 0xb003,
    LETTER_TINGLE = 0xb203,
    UNK_B907 = 0xb907,
    UNK_BB07 = 0xbb07,
    UNK_BCFF = 0xbcff,
    UNK_C103 = 0xc103,
    UNK_C407 = 0xc407,
    UNK_C903 = 0xc903,
    UNK_CA03 = 0xca03,
    UNK_CB03 = 0xcb03,
    UNK_CCFF = 0xccff,
    UNK_CF03 = 0xcf03
};
}

namespace dSv_event_tmp_flag_c {
enum : u16 {
    UNK_0001 = 0x1,
    UNK_0002 = 0x2,
    UNK_0004 = 0x4,
    UNK_0008 = 0x8,
    UNK_0010 = 0x10,
    UNK_0020 = 0x20,
    UNK_0040 = 0x40,
    UNK_0120 = 0x120,
    UNK_0180 = 0x180,
    UNK_0140 = 0x140,
    UNK_0580 = 0x580,
    UNK_0104 = 0x104,
    UNK_0208 = 0x208,
    UNK_0302 = 0x302,
    UNK_0301 = 0x301
};
}
enum : u8 { dItemNo_MOBLINS_LETTER_e = 0x9B };

// HD: the environment global is lazy and has a destructor record, unlike GameCube.
static KankyoEnvLight g_env_light;
struct KankyoGuard {
    be<u32> value;
};
static KankyoGuard envLightInitialized;
struct KankyoDestructorRecord {
    be<u32> words[3];
};
static KankyoDestructorRecord envLightDestructor;
struct KankyoFloat {
    be<f32> value;
};
static KankyoFloat kankyoOne;
static KankyoFloat kankyoZero;
static KankyoFloat kankyoNegativeOne;
struct KankyoRoomNo {
    be<s8> value;
};
static KankyoRoomNo kankyoStayRoomNo;
static gabi::HostFn lightConstructorBinding(reinterpret_cast<const void *>(&kankyoConstructLight), 0x025637D4);
static gabi::HostFn record40ConstructorBinding(reinterpret_cast<const void *>(&kankyoConstructRecord40), 0x02563814);
static gabi::HostFn record10ConstructorBinding(reinterpret_cast<const void *>(&kankyoConstructRecord10), 0x02563840);

static KankyoEnvLight *envLightView() {
    // HD: preserve a lazy-accessor call for each component, including its side effects.
    return reinterpret_cast<KankyoEnvLight *>(::dKy_getEnvlight());
}

/* 02554288 */
s32 dKy_get_dayofweek() {
    s32 date = dComIfGs_info()->mSavedata.mPlayer.mPlayerStatusB.mDate;
    return date % 7;
}

static void incEventRegMax(u16 reg, s32 max) {
    s32 value = static_cast<s32>(dComIfGs_getEventReg(reg)) + 1;
    dSv_event_c *event = dComIfGs_getEvent();
    if (value > max)
        value = max;
    event->setEventReg(reg, static_cast<u8>(value));
}

/* 025542BC */
void dKankyo_DayProc() {
    if (dComIfGs_getEvent()->isEventBit(dSv_event_flag_c::UNK_1820))
        dLetter_autoStock(dSv_event_flag_c::LETTER_ARYLL);
    if (dComIfGs_getEvent()->isEventBit(dSv_event_flag_c::UNK_1820) &&
        dComIfGs_getEvent()->isEventBit(dSv_event_flag_c::UNK_0B80))
        dLetter_autoStock(dSv_event_flag_c::LETTER_TINGLE);
    dLetter_delivery(dSv_event_flag_c::LETTER_BAITOS_MOM);
    dLetter_delivery(dSv_event_flag_c::LETTER_HOSKITS_GIRLFRIEND);
    dLetter_delivery(dSv_event_flag_c::LETTER_SILVER_MEMBERSHIP);
    dLetter_delivery(dSv_event_flag_c::LETTER_GOLD_MEMBERSHIP);
    dLetter_delivery(dSv_event_flag_c::LETTER_GRANDMA);
    incEventRegMax(dSv_event_flag_c::UNK_CF03, 3);
    incEventRegMax(dSv_event_flag_c::UNK_AB03, 3);
    if (dComIfGs_isGetItemReserve(0xF) && dComIfGs_checkReserveItem(dItemNo_MOBLINS_LETTER_e) == 0)
        incEventRegMax(dSv_event_flag_c::UNK_CCFF, 2);
    dComIfGs_getEvent()->setEventReg(dSv_event_flag_c::UNK_BCFF, 0);
    dComIfGs_getEvent()->setEventReg(dSv_event_flag_c::UNK_CB03, 0);
    dComIfGs_getEvent()->setEventReg(dSv_event_flag_c::UNK_CA03, 0);
    dComIfGs_getEvent()->setEventReg(dSv_event_flag_c::UNK_C903, 0);
    dComIfGs_offEventBit(dSv_event_flag_c::UNK_1304);
    dComIfGs_offEventBit(dSv_event_flag_c::UNK_1302);
    dComIfGs_offEventBit(dSv_event_flag_c::UNK_1301);
    const u16 temporaryFlags[] = {
        dSv_event_tmp_flag_c::UNK_0120, dSv_event_tmp_flag_c::UNK_0040, dSv_event_tmp_flag_c::UNK_0020,
        dSv_event_tmp_flag_c::UNK_0010, dSv_event_tmp_flag_c::UNK_0008, dSv_event_tmp_flag_c::UNK_0004,
        dSv_event_tmp_flag_c::UNK_0002, dSv_event_tmp_flag_c::UNK_0001, dSv_event_tmp_flag_c::UNK_0180,
        dSv_event_tmp_flag_c::UNK_0140, dSv_event_tmp_flag_c::UNK_0580, dSv_event_tmp_flag_c::UNK_0104};
    for (u16 flag : temporaryFlags)
        dComIfGs_offTmpBit(flag);
    if (dComIfGs_getEvent()->getEventReg(dSv_event_flag_c::UNK_C103) == 1)
        dComIfGs_getEvent()->setEventReg(dSv_event_flag_c::UNK_C103, 2);
    if (dComIfGs_getEvent()->isEventBit(dSv_event_flag_c::UNK_1F10)) {
        u8 reg = dComIfGs_getEvent()->getEventReg(dSv_event_flag_c::UNK_BB07);
        dSv_event_c *event = dComIfGs_getEvent();
        if (reg == 7) {
            event->onEventBit(dSv_event_flag_c::UNK_1F08);
        } else {
            s32 v = (s32)reg + 1;
            if (v > 7)
                v = 7;
            event->setEventReg(dSv_event_flag_c::UNK_BB07, (u8)v);
        }
    }
    if (dComIfGs_getEvent()->getEventReg(dSv_event_flag_c::UNK_C407) == 6)
        dComIfGs_getEvent()->setEventReg(dSv_event_flag_c::UNK_C407, 7);
    u8 reg = dComIfGs_getEvent()->getEventReg(dSv_event_flag_c::UNK_B907);
    if (reg & 1)
        dComIfGs_getEvent()->setEventReg(dSv_event_flag_c::UNK_B907, (u8)(reg + 1));
    dComIfGs_offEventBit(dSv_event_flag_c::UNK_2680);
    if (dComIfGs_getEvent()->isEventBit(dSv_event_flag_c::GRANDMA_HEALED))
        incEventRegMax(dSv_event_flag_c::UNK_A60F, 3);
    if (dKy_get_dayofweek() == 5) {
        const u16 fridayFlags[] = {dSv_event_flag_c::UNK_2080, dSv_event_flag_c::UNK_2004, dSv_event_flag_c::UNK_2002,
                                   dSv_event_flag_c::UNK_2804, dSv_event_flag_c::UNK_2802, dSv_event_flag_c::UNK_2801,
                                   dSv_event_flag_c::UNK_2980, dSv_event_flag_c::UNK_2940, dSv_event_flag_c::UNK_3B01,
                                   dSv_event_flag_c::UNK_3C80, dSv_event_flag_c::UNK_3C40, dSv_event_flag_c::UNK_3C20,
                                   dSv_event_flag_c::UNK_3C10, dSv_event_flag_c::UNK_3C08, dSv_event_flag_c::UNK_3C04,
                                   dSv_event_flag_c::UNK_3C02};
        for (u16 flag : fridayFlags)
            dComIfGs_offEventBit(flag);
    }
    dComIfGs_offTmpBit(dSv_event_tmp_flag_c::UNK_0208);
    if (dComIfGs_getEvent()->isEventBit(dSv_event_flag_c::UNK_2F01))
        dComIfGs_getEvent()->onEventBit(dSv_event_flag_c::UNK_3080);
    dComIfGs_offTmpBit(dSv_event_tmp_flag_c::UNK_0302);
    dComIfGs_offTmpBit(dSv_event_tmp_flag_c::UNK_0301);
}

/* 02554870 */
KankyoEfLightProc *KankyoEfLightProc::construct() {
    KankyoEfLightProc *self = this;
    // HD: allocating construction is valid for a null guest address.
    if (self == nullptr) {
        self = static_cast<KankyoEfLightProc *>(kankyoAllocate(0x2C));
        if (self == nullptr)
            return self;
    }
    self->mSwordLight.mHD20 = static_cast<f32>(kankyoOne.value);
    return self;
}

/* 025548B0 */
KankyoThunder *KankyoThunder::construct() {
    KankyoThunder *self = this;
    // HD: allocating construction is valid for a null guest address.
    if (self == nullptr) {
        self = static_cast<KankyoThunder *>(kankyoAllocate(0x3C));
        if (self == nullptr)
            return self;
    }
    self->mLightInfluence.mHD20 = static_cast<f32>(kankyoOne.value);
    return self;
}

/* GXColorS10 addcol setters: colour * factor, the env light fetched for each component.
 * The s16 arguments are converted from the whole register (GHS does not re-extend). */
static void addcol_set(GXColorS10 KankyoEnvLight::*color, s32 r, s32 g, s32 b, f32 factor) {
    // HD: s16 parameters use the whole incoming register without re-extension.
    (envLightView()->*color).r = static_cast<s16>(gabi::ftoi(static_cast<f32>(r) * factor));
    (envLightView()->*color).g = static_cast<s16>(gabi::ftoi(static_cast<f32>(g) * factor));
    (envLightView()->*color).b = static_cast<s16>(gabi::ftoi(static_cast<f32>(b) * factor));
}
/* 025548F0 */
void dKy_actor_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) {
    addcol_set(&KankyoEnvLight::mActorAddColAmb, r, g, b, factor);
}
/* 02554A04 */
void dKy_actor_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) {
    addcol_set(&KankyoEnvLight::mActorAddColDif, r, g, b, factor);
}
/* 02554B18 */
void dKy_bg_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) {
    addcol_set(&KankyoEnvLight::mBgAddColAmb, r, g, b, factor);
}
/* 02554C2C */
void dKy_bg_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) {
    addcol_set(&KankyoEnvLight::mBgAddColDif, r, g, b, factor);
}
/* 02554D40 */
void dKy_bg1_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) {
    addcol_set(&KankyoEnvLight::mBg1AddColAmb, r, g, b, factor);
}
/* 02554E54 */
void dKy_bg1_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) {
    addcol_set(&KankyoEnvLight::mBg1AddColDif, r, g, b, factor);
}
/* 02554F68 */
void dKy_bg2_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) {
    addcol_set(&KankyoEnvLight::mBg2AddColAmb, r, g, b, factor);
}
/* 0255507C */
void dKy_bg2_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) {
    addcol_set(&KankyoEnvLight::mBg2AddColDif, r, g, b, factor);
}
/* 02555190 */
void dKy_bg3_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) {
    addcol_set(&KankyoEnvLight::mBg3AddColAmb, r, g, b, factor);
}
/* 025552A4 */
void dKy_bg3_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) {
    addcol_set(&KankyoEnvLight::mBg3AddColDif, r, g, b, factor);
}

/* 025553B8 */
void dKy_actor_addcol_set(s32 r, s32 g, s32 b, f32 factor) {
    dKy_actor_addcol_amb_set(r, g, b, factor);
    dKy_actor_addcol_dif_set(r, g, b, factor);
    dKy_bg_addcol_amb_set(r, g, b, factor);
    dKy_bg_addcol_dif_set(r, g, b, factor);
    dKy_bg1_addcol_amb_set(r, g, b, factor);
    dKy_bg1_addcol_dif_set(r, g, b, factor);
    dKy_bg2_addcol_amb_set(r, g, b, factor);
    dKy_bg2_addcol_dif_set(r, g, b, factor);
    dKy_bg3_addcol_amb_set(r, g, b, factor);
    dKy_bg3_addcol_dif_set(r, g, b, factor);
}

/* 025554CC */
// HD: enlarged object, trailing vtable, and explicit zero-fill/array construction.
KankyoEnvLight *KankyoEnvLight::construct() {
    KankyoEnvLight *self = this;
    // HD: null construction allocates the enlarged environment object.
    if (self == nullptr) {
        self = static_cast<KankyoEnvLight *>(kankyoAllocate(sizeof(KankyoEnvLight)));
        if (self == nullptr)
            return self;
    }
    const f32 one = static_cast<f32>(kankyoOne.value);
    self->mpPaletInfo = nullptr;
    self->__vtbl = 0x1004F084;
    self->mpVrboxInfo = nullptr;
    self->mpPselectInfo = nullptr;
    self->mpEnvrInfo = nullptr;
    self->mpSchejule = nullptr;
    kankyoClear(&self->mBaseLightInfluence, sizeof(self->mBaseLightInfluence));
    self->mBaseLightInfluence.mHD20 = one;
    kankyoConstructArray(self->mLightInfluence, 30, sizeof(dKankyo::LIGHT_INFLUENCE), kankyoConstructLight,
                         nullptr); /* LIGHT_INFLUENCE[30] */
    kankyoClear(&self->mpPLights, 0x320);
    kankyoClear(&self->mpEfLights, 0x28);
    kankyoClear(&self->mColChgLight, 0x24);
    self->mColChgLight.mHD20 = one;
    self->field_0x760 = 0;
    kankyoClear(&self->mEfLightProc, 0x2C);
    self->mEfLightProc.construct();
    self->mpPlightInfo = nullptr;
    kankyoClear(&self->mpWindInfluence, 0x78);
    kankyoClear(&self->mWindInfluenceEntity, 0x12C);
    kankyoClear(&self->mSound, 0x18);
    kankyoClear(&self->mWaveChan, 0x30);
    kankyoClear(&self->mWind, 0x34);
    kankyoClear(&self->mbSunInitialized, 0xC);
    kankyoClear(&self->mbRainInitialized, 0xC);
    kankyoClear(&self->mbSnowInitialized, 0xC);
    kankyoClear(&self->mbStarInitialized, 0x10);
    kankyoClear(&self->mbPoisonInitialized, 0xC);
    kankyoClear(&self->mbHousiInitialized, 0xC);
    kankyoClear(&self->mMoyaInitialized, 0xC);
    kankyoClear(&self->mVrkumoStatus, 0x10);
    kankyoClear(&self->mWaveInitialized, 0xC);
    kankyoClear(&self->mbWindlineInitialized, 0xC);
    kankyoClear(&self->mThunderEff, 0x3C);
    self->mThunderEff.construct();
    kankyoClear(&self->mpWaveInfl, 0x28);
    kankyoClear(&self->mLightDir, 0xC);
    kankyoClear(&self->mFogAdjTable, 0x14);
    const f32 zero = static_cast<f32>(kankyoZero.value);
    self->mHdField_0xb64 = 0;
    self->mHdField_0xb81 = 0;
    self->mHdField_0xb73 = 0xFF;
    self->mHdField_0xb74 = 0;
    self->mHdField_0xb6f = 0xFF;
    self->mHdField_0xb6e = 0;
    self->mHdField_0xb6b = 0xFF;
    self->mHdField_0xb89 = 0;
    self->mHdField_0xb67 = 0xFF;
    self->mHdField_0xb80 = 0;
    self->mHdField_0xb83 = 0xFF;
    self->mHdField_0xb6c = 0;
    self->mHdField_0xb87 = 0xFF;
    self->mHdField_0xb69 = 0;
    self->mHdField_0xb7b = 0xFF;
    self->mHdField_0xb70 = 0;
    self->mHdField_0xb7d = 0;
    self->mHdField_0xb8c = 0;
    self->mHdField_0xb79 = 0;
    self->mHdField_0xb75 = 0;
    self->mHdField_0xb7a = 0;
    self->mHdField_0xb7c = 0;
    self->mHdField_0xb82 = 0;
    self->mHdField_0xb72 = 0;
    self->mHdField_0xb8d = 0;
    self->mHdField_0xb77 = 0xFF;
    self->mHdField_0xb71 = 0;
    self->mHdField_0xb78 = 0;
    self->mHdField_0xb8e = 0;
    self->mHdField_0xb7e = 0;
    self->mHdField_0xb65 = 0;
    self->mHdField_0xb86 = 0;
    self->mHdField_0xb8f = 0xFF;
    self->mHdField_0xb8b = 0xFF;
    self->mHdField_0xb85 = 0;
    self->mHdField_0xb76 = 0;
    self->mHdField_0xb90 = 0;
    self->mHdField_0xb6d = 0;
    self->mHdField_0xb68 = 0;
    self->mHdField_0xb66 = 0;
    self->mHdField_0xb91 = 0;
    self->mHdField_0xb6a = 0;
    self->mHdField_0xb84 = 0;
    self->mHdField_0xb8a = 0;
    self->mHdField_0xb92 = 0;
    self->mHdField_0xb7f = 0xFF;
    self->mHdField_0xb88 = 0;
    self->mHdField_0xb93 = 0xFF;
    self->mHdField_0xb94 = 0;
    self->mHdField_0xb95 = 0;
    self->mHdField_0xb96 = 0;
    self->mHdField_0xb97 = 0xFF;
    self->mHdField_0xb98 = 0;
    self->mHdField_0xb99 = 0;
    self->mHdField_0xb9a = 0;
    self->mHdField_0xb9b = 0xFF;
    self->mHdField_0xba2 = 0;
    self->mHdField_0xb9d = 0;
    self->mHdField_0xb9c = 0;
    self->mHdField_0xba4 = 0;
    self->mHdField_0xba3 = 0xFF;
    self->mHdField_0xba7 = 0xFF;
    self->mHdField_0xba1 = 0;
    self->mHdField_0xba0 = 0;
    self->mHdField_0xba6 = 0;
    self->mHdField_0xba5 = 0;
    self->mHdField_0xb9e = 0;
    self->mHdField_0xb9f = 0xFF;
    kankyoClear(&self->mHdField_0xba8, 8);
    self->mHdField_0xbca = 0;
    self->mHdField_0xbce = 0;
    self->mHdField_0xbc7 = 0xFF;
    self->mHdField_0xbb7 = 0xFF;
    self->mHdField_0xbcb = 0xFF;
    self->mHdField_0xbbe = 0;
    self->mHdField_0xbb9 = 0;
    self->mHdField_0xbb3 = 0xFF;
    self->mHdField_0xbbc = 0;
    self->mHdField_0xbba = 0;
    self->mHdField_0xbbf = 0xFF;
    self->mHdField_0xbcc = 0;
    self->mHdField_0xbb6 = 0;
    self->mHdField_0xbb5 = 0;
    self->mHdField_0xbd5 = 0;
    self->mHdField_0xbd4 = 0;
    self->mHdField_0xbb4 = 0;
    self->mHdField_0xbb0 = 0;
    self->mHdField_0xbd6 = 0;
    self->mHdField_0xbd3 = 0xFF;
    self->mHdField_0xbb1 = 0;
    self->mHdField_0xbc0 = 0;
    self->mHdField_0xbd7 = 0xFF;
    self->mHdField_0xbb2 = 0;
    self->mHdField_0xbc6 = 0;
    self->mHdField_0xbd0 = 0;
    self->mHdField_0xbd8 = 0;
    self->mHdField_0xbc3 = 0xFF;
    self->mHdField_0xbcd = 0;
    self->mHdField_0xbc5 = 0;
    self->mHdField_0xbd9 = 0;
    self->mHdField_0xbd2 = 0;
    self->mHdField_0xbbd = 0;
    self->mHdField_0xbb8 = 0;
    self->mHdField_0xbda = 0;
    self->mHdField_0xbc2 = 0;
    self->mHdField_0xbc9 = 0;
    self->mHdField_0xbc1 = 0;
    self->mHdField_0xbdb = 0xFF;
    self->mHdField_0xbbb = 0xFF;
    self->mHdField_0xbcf = 0xFF;
    self->mHdField_0xbd1 = 0;
    self->mHdField_0xbdc = 0;
    self->mHdField_0xbc4 = 0;
    self->mHdField_0xbc8 = 0;
    self->mHdField_0xbdd = 0;
    self->mHdField_0xbde = 0;
    self->mHdField_0xbdf = 0xFF;
    self->mHdField_0xbe0 = zero;
    self->mHdField_0xc04 = zero;
    self->mHdField_0xc23 = 0xFF;
    self->mHdField_0xc22 = 0;
    self->mHdField_0xc00 = zero;
    self->mHdField_0xc08 = zero;
    self->mHdField_0xc24 = 0;
    self->mHdField_0xc21 = 0;
    self->mHdField_0xbfc = zero;
    self->mHdField_0xc0c = zero;
    self->mHdField_0xc3c = zero;
    self->mHdField_0xc20 = 0;
    self->mHdField_0xc38 = zero;
    self->mHdField_0xbf8 = zero;
    self->mHdField_0xc34 = zero;
    self->mHdField_0xc10 = zero;
    self->mHdField_0xc30 = zero;
    self->mHdField_0xc26 = 0;
    self->mHdField_0xc2c = zero;
    self->mHdField_0xc1f = 0xFF;
    self->mHdField_0xc2b = 0xFF;
    self->mHdField_0xbf4 = zero;
    self->mHdField_0xbe4 = zero;
    self->mHdField_0xc14 = zero;
    self->mHdField_0xc1b = 0xFF;
    self->mHdField_0xc27 = 0xFF;
    self->mHdField_0xc2a = 0;
    self->mHdField_0xc1e = 0;
    self->mHdField_0xc1a = 0;
    self->mHdField_0xbf0 = zero;
    self->mHdField_0xbe8 = zero;
    self->mHdField_0xc18 = 0;
    self->mHdField_0xc1c = 0;
    self->mHdField_0xc28 = 0;
    self->mHdField_0xc29 = 0;
    self->mHdField_0xc1d = 0;
    self->mHdField_0xc19 = 0;
    self->mHdField_0xc25 = 0;
    self->mHdField_0xbec = zero;
    kankyoConstructObject74(&self->mHdObject74);
    self->mHdObject74.__vtbl = 0x1016F014;
    kankyoClear(&self->mHdRenderState, 0x294);
    kankyoConstructArrayNoDtor(self->mHdRenderState.mRecords, 4, sizeof(dKankyo::HdRecord40),
                               kankyoConstructRecord40); /* __construct_array */
    // HD: unsigned guest address arithmetic preserves wrap-to-null allocation cases.
    const u16 subs[] = {0x100, 0x110, 0x160, 0x170, 0x180, 0x194, 0x1A4, 0x1B4, 0x1C4, 0x1D4, 0x1E4};
    for (u16 o : subs)
        if (static_cast<u32>(gabi::ea(self) + offsetof(KankyoEnvLight, mHdRenderState) + o) == 0)
            kankyoAllocate(o == 0x110 ? 0x40 : 0x10);
    kankyoConstructArrayNoDtor(&self->mHdRenderState.mTail[0xF4], 10, 0x10, kankyoConstructRecord10);
    // HD: clear the color block in its original ascending order.
    kankyoClear(&self->mBG3_K0, sizeof(GXColorS10));
    kankyoClear(&self->mFogColor, sizeof(GXColorS10));
    kankyoClear(&self->mActorAddColAmb, sizeof(GXColorS10));
    kankyoClear(&self->mActorAddColDif, sizeof(GXColorS10));
    kankyoClear(&self->mBgAddColAmb, sizeof(GXColorS10));
    kankyoClear(&self->mBgAddColDif, sizeof(GXColorS10));
    kankyoClear(&self->mBg1AddColAmb, sizeof(GXColorS10));
    kankyoClear(&self->mBg1AddColDif, sizeof(GXColorS10));
    kankyoClear(&self->mBg2AddColAmb, sizeof(GXColorS10));
    kankyoClear(&self->mBg2AddColDif, sizeof(GXColorS10));
    kankyoClear(&self->mBg3AddColAmb, sizeof(GXColorS10));
    kankyoClear(&self->mBg3AddColDif, sizeof(GXColorS10));
    kankyoClear(&self->mAddColFog, sizeof(GXColorS10));
    kankyoClear(&self->mVrboxAddColSky0, sizeof(GXColorS10));
    kankyoClear(&self->mVrboxAddColKasumi, sizeof(GXColorS10));
    self->mCurTime = zero;
    self->field_0xc44 = zero;
    self->field_0xc2c = zero;
    self->mFogStartZ = zero;
    self->mActColRatio = zero;
    self->mColPatBlendGather = zero;
    self->mDiceWeatherTime = zero;
    self->mBgColGatherRatio = zero;
    self->mBgColRatio = zero;
    self->mVrboxSoraColRatio = zero;
    self->mVrboxKumoColGatherRatio = zero;
    self->field_0xc4c = 0;
    self->mFogEndZ = zero;
    self->mActColGatherRatio = zero;
    self->field_0x103c = zero;
    self->field_0xc48 = 0;
    self->mAllColRatio = zero;
    self->mVrboxKumoColRatio = zero;
    self->field_0x1040 = zero;
    self->field_0xc30 = zero;
    self->field_0xc34 = zero;
    self->mTimeAdv = zero;
    self->mFogRatio = zero;
    self->field_0x1034 = zero;
    self->mColPatBlend = zero;
    self->mNextTime = zero;
    self->mFogColRatio = zero;
    self->mFogStartZ__setLight = zero;
    self->field_0x102c = zero;
    self->field_0x1038 = zero;
    self->field_0x1048 = zero;
    self->mDiceWeatherChangeTime = zero;
    self->field_0x1030 = zero;
    self->mFogEndZ__setLight = zero;
    self->field_0x1044 = zero;
    self->mFogColGatherRatio = zero;
    self->mVrboxSoraColGatherRatio = zero;
    self->mAllColGatherRatio = zero;
    kankyoClear(&self->mMoyaSE, 0x10);
    self->mEnvrIdxPrev = 0;
    self->field_0xc74 = 0;
    self->mEnvrIdxCurr = 0;
    self->mPlayerEflightIdx = 0;
    self->mColpatWeather = 0;
    self->field_0xc78 = 0;
    self->mPlayerPLightIdx = 0;
    self->mColpatCurrGather = 0;
    self->mDiceWeatherMode = 0;
    self->mSchbitTimer = 0;
    self->field_0xc68 = 0;
    self->mColpatPrev = 0;
    self->field_0x10a5 = 0;
    self->field_0xc9a = 0;
    self->mbVrboxInvisible = 0;
    self->field_0x10b8 = zero;
    self->field_0x10a8 = zero;
    self->mDiceWeatherCurrPattern = 0;
    self->mDiceWeatherState = 0;
    self->mColChgFlag = 0;
    self->mColPatMode = 0;
    self->mColPatModeGather = 0;
    self->mInitAnimTimer = 0;
    self->mFogAdjCenter = 0;
    self->mDayOfWeek = 0;
    self->field_0x10ac = zero;
    self->field_0x10bc = zero;
    self->mbContrastFlag = 0;
    self->field_0xc9b = 0;
    self->mbDayNightTactStop = 0;
    self->mSchbit = 0;
    self->field_0x10b4 = zero;
    self->field_0x10a6 = 0;
    self->mColpatCurr = 0;
    self->mColpatPrevGather = 0;
    self->mFogAdjTableType = 0;
    self->field_0xc8e = 0;
    self->mFogAdjEnable = 0;
    self->field_0x10c0 = zero;
    self->mRainCountOrig = 0;
    self->mDiceWeatherCounter = 0;
    self->field_0x10b0 = zero;
    self->field_0x10c4 = zero;
    self->field_0xc98 = 0;
    self->field_0x10a4 = 0;
    self->field_0x10c8 = zero;
    self->field_0x10cc = zero;
    self->field_0x10d0 = zero;
    self->field_0x10d4 = zero;
    self->field_0x10d8 = zero;
    self->field_0x10dc = zero;
    self->field_0x10e0 = zero;
    self->field_0x10e4 = zero;
    self->field_0x10e8 = zero;
    kankyoClear(&self->mHdTailState, 0x1C);
    kankyoClear(&self->mHdTailTable0, 0x80);
    kankyoClear(&self->mHdTailTable1, 0x80);
    dKy_actor_addcol_set(0, 0, 0, zero);
    self->mMoonPos.y = zero;
    self->mSunPos.y = zero;
    f32 m1 = static_cast<f32>(kankyoNegativeOne.value);
    self->mMoonPos.z = zero;
    self->mNextTime = m1;
    self->mMoonPos.x = m1;
    self->mSunPos.z = zero;
    self->mSunPos.x = one;
    return self;
}

/* 02555D0C */
// HD: private name avoids the shared opaque accessor; this implements its lazy global.
KankyoEnvLight *kankyo_getEnvlight() {
    // HD: the guard is set before construction and destructor registration.
    if (envLightInitialized.value == 0) {
        envLightInitialized.value = 1;
        g_env_light.construct();
        kankyoRegisterGlobal(&envLightDestructor);
    }
    return &g_env_light;
}

/* 02555D64 */
BOOL toon_proc_check() {
    s32 roomNo = kankyoStayRoomNo.value;
    if (roomNo >= 0) {
        KankyoRoomData *room =
            kankyoGetRoomData(&reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())->play.mRoomCtrl, roomNo);
        // HD: call the room virtual even though the toon result is always FALSE.
        auto *vtable = gabi::at<KankyoRoomVtable>(room->__vtbl);
        gabi::call_ptr(vtable->getFileListInfo, room);
    }
    return FALSE;
}

/* 02555DBC */
s16 u8_data_ratio_set(s32 a, s32 b, f32 ratio) {
    s16 d = (s16)gabi::ftoi(static_cast<f32>(b - a) * ratio);
    return (s16)(a + d);
}

/* 02555E0C */
s16 s16_data_ratio_set(s32 a, s32 b, f32 ratio) {
    s16 d = (s16)gabi::ftoi(static_cast<f32>(b - a) * ratio);
    return (s16)(a + d);
}

/* 02555E5C */
s16 kankyo_color_ratio_set(u32 b0A, u32 b0B, f32 blendAB0, u32 b1A, u32 b1B, f32 blendAB1, s32 add, f32 blend01) {
    KankyoEnvLight *env = envLightView();
    s16 c0 = s16_data_ratio_set((s32)b0A, (s32)b0B, blendAB0);
    s16 c1 = s16_data_ratio_set((s32)b1A, (s32)b1B, blendAB0);
    s16 c = s16_data_ratio_set(c0, c1, blendAB1);
    s16 v = (s16)(c + add);
    f32 ratio = static_cast<f32>(env->mAllColRatio) * blend01;
    s16 r = (s16)gabi::ftoi(static_cast<f32>(v) * ratio);
    if (r < 0)
        return 0;
    if (r > 0xFF)
        return 0xFF;
    return r;
}

/* 02555F80 */
f32 fl_data_ratio_set(f32 a, f32 b, f32 ratio) { return gabi::fmadds(b - a, ratio, a); }

/* 02555F8C */
f32 float_kankyo_color_ratio_set(f32 p0, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6, f32 p7) {
    KankyoEnvLight *env = envLightView();
    f32 a = fl_data_ratio_set(p0, p1, p2);
    f32 b = fl_data_ratio_set(p3, p4, p2);
    f32 c = fl_data_ratio_set(a, b, p5);
    f32 rt = gabi::fmadds(p6 - c, p7, c) * static_cast<f32>(env->mAllColRatio);
    return rt >= 0.0f ? rt : static_cast<f32>(kankyoZero.value);
}

// HD: compatibility entry points for the other, independently rewritten TU parts.
namespace d_kankyo_cpp {
s32 dKy_get_dayofweek() { return ::dKy_get_dayofweek(); }
void dKankyo_DayProc() { ::dKankyo_DayProc(); }
void dKy_actor_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) { ::dKy_actor_addcol_amb_set(r, g, b, factor); }
void dKy_actor_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) { ::dKy_actor_addcol_dif_set(r, g, b, factor); }
void dKy_bg_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) { ::dKy_bg_addcol_amb_set(r, g, b, factor); }
void dKy_bg_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) { ::dKy_bg_addcol_dif_set(r, g, b, factor); }
void dKy_bg1_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) { ::dKy_bg1_addcol_amb_set(r, g, b, factor); }
void dKy_bg1_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) { ::dKy_bg1_addcol_dif_set(r, g, b, factor); }
void dKy_bg2_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) { ::dKy_bg2_addcol_amb_set(r, g, b, factor); }
void dKy_bg2_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) { ::dKy_bg2_addcol_dif_set(r, g, b, factor); }
void dKy_bg3_addcol_amb_set(s32 r, s32 g, s32 b, f32 factor) { ::dKy_bg3_addcol_amb_set(r, g, b, factor); }
void dKy_bg3_addcol_dif_set(s32 r, s32 g, s32 b, f32 factor) { ::dKy_bg3_addcol_dif_set(r, g, b, factor); }
void dKy_actor_addcol_set(s32 r, s32 g, s32 b, f32 factor) { ::dKy_actor_addcol_set(r, g, b, factor); }
BOOL toon_proc_check() { return ::toon_proc_check(); }
s16 u8_data_ratio_set(s32 a, s32 b, f32 ratio) { return ::u8_data_ratio_set(a, b, ratio); }
s16 s16_data_ratio_set(s32 a, s32 b, f32 ratio) { return ::s16_data_ratio_set(a, b, ratio); }
s16 kankyo_color_ratio_set(u32 b0A, u32 b0B, f32 blendAB0, u32 b1A, u32 b1B, f32 blendAB1, s32 add, f32 blend01) {
    return ::kankyo_color_ratio_set(b0A, b0B, blendAB0, b1A, b1B, blendAB1, add, blend01);
}
f32 fl_data_ratio_set(f32 a, f32 b, f32 ratio) { return ::fl_data_ratio_set(a, b, ratio); }
f32 float_kankyo_color_ratio_set(f32 p0, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6, f32 p7) {
    return ::float_kankyo_color_ratio_set(p0, p1, p2, p3, p4, p5, p6, p7);
}
} // namespace d_kankyo_cpp

/* WWHD glue
 data g_env_light 10475A68
 data envLightInitialized 104773B4
 data envLightDestructor 101E8E9C
 data kankyoOne 1004F0A4
 data kankyoZero 1004F0B0
 data kankyoNegativeOne 1004F0B4
 data kankyoStayRoomNo 1047E6C8
 ext 0273AD10 void* kankyoAllocate(u32)
 ext 028F521C void kankyoClear(void*,u32)
 ext 028F0020 void kankyoConstructArray(void*,u32,u32,KankyoArrayConstructor,void*)
 ext 028EFFD0 void kankyoConstructArrayNoDtor(void*,u32,u32,KankyoArrayConstructor)
 ext 025637D4 void* kankyoConstructLight(void*)
 ext 02563814 void* kankyoConstructRecord40(void*)
 ext 02563840 void* kankyoConstructRecord10(void*)
 ext 028F026C void kankyoRegisterGlobal(void*)
 ext 027FB40C void kankyoConstructObject74(dKankyo::HdObject74*)
 ext 02586E94 void dLetter_autoStock(u32)
 ext 02586E58 void dLetter_delivery(u32)
 ext 025C11DC KankyoRoomData* kankyoGetRoomData(void*,s32)
*/
