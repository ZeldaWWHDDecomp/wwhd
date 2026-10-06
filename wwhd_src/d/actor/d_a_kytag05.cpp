/**
 * d_a_kytag05.cpp (WWHD)
 * Environment tag 05: Tower of the Gods / Hyrule wind cycle (event wind direction and strength).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kytag05.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define KYTAG05_VTBL 0x10013B94 /* kytag05_class vtable (HD virtual destructor) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02578348 dKyw_get_wind_pow() */
static inline f32 dKyw_get_wind_pow() { return gabi::call<f32>(0x02578348); }
/* 0257E684 dKyw_evt_wind_set(s16 x, s16 y) */
static inline void dKyw_evt_wind_set(s16 x, s16 y) { gabi::call(0x0257E684, x, y); }
/* 0257E6C4 dKyw_evt_wind_set_go() */
static inline void dKyw_evt_wind_set_go() { gabi::call(0x0257E6C4); }
/* 0255FDA4 dKy_custom_colset(u8 prev, u8 next, f32 blend) */
static inline void dKy_custom_colset(u8 a, u8 b, f32 blend) { gabi::call(0x0255FDA4, a, b, blend); }
/* 025E1A04 mDoAud_seStart(id, pos, param) (HD: no reverb argument) */
static inline void mDoAud_seStart3(u32 id, cXyz* pos, u32 param) { gabi::call(0x025E1A04, id, pos, param); }
/* 025B7D90 dSv_player_collect_c::isSymbol (save + 0xD4) */
static inline BOOL dComIfGs_isSymbol(u8 i) { return gabi::call<BOOL>(0x025B7D90, gabi::load<u32>(0x101F84DC) + 0xD4, i); }
/* dComIfGp_event_runCheck(): play+0x5292 (u8) != 0 */
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
/* 025445B8 dEvent_manager_c::startCheckOld(const char*) */
static inline BOOL dComIfGp_evmng_startCheckOld(const char* name) { return gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), name); }
/* HD: dComIfGp_demo_get()->getFrame(): the demo frame counter is a global (u32 at 0x101D6008), no null check */
static inline u32 dDemo_getFrame() { return gabi::load<u32>(0x101D6008); }
/* dComIfGp_getCamera(0): camera_class* at play+0x5AF8; view.mLookat.mEye at +0xDC */
static inline u32 dComIfGp_getCamera0() { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8); }
static inline u32 dComIfGp_getPlayer0_ea() { return gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER); }

/* g_env_light fields (dScnKy_env_light_c, HD offsets) */
static inline u32 env_ea() { return gabi::ea(dKy_getEnvlight()); }
enum : u32 {
    ENV_EVTWINDSET = 0xA2D, /* mWind.mEvtWindSet (u8) */
    ENV_SNOWCOUNT = 0xA4C,  /* mSnowCount (s32) */
    ENV_MOYAMODE = 0xA7D,   /* mMoyaMode (u8) */
    ENV_MOYACOUNT = 0xA80,  /* mMoyaCount (s32) */
};

enum { JA_SE_ATM_WIND_VAR = 0x106A };

struct kytag05_class : fopAc_ac_c {
    /* 0x3AC */ be<u8> mIndex;
    /* 0x3AD */ u8 _3AD[3];
    /* 0x3B0 */ be<s32> mTimer;
    /* 0x3B4 */ be<s32> mUnknownParam;
};
WWHD_SIZE(kytag05_class, 0x3B8);

/* static const s16 wind_table[] = {0x0000, 0x8001, 0xC000, 0x4000}; mufuu_timer[] = {10, 10, 0, 90};
 * fuu_timer[] = {150, 150, 150, 150} (.rodata) */
static inline s16 wind_table(s32 i) { return gabi::load<s16>(0x10013BD8 + 2 * i); }
static inline s16 mufuu_timer(s32 i) { return gabi::load<s16>(0x10013BE0 + 2 * i); }
static inline s16 fuu_timer(s32 i) { return gabi::load<s16>(0x10013BE8 + 2 * i); }

/* (u32)f, GHS: through 2^31 */
static inline u32 f2u(f32 f) {
    if (f < 2147483648.0f) return (u32)gabi::ftoi(f);
    return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}

/* 021B0708 */
static BOOL daKytag05_Draw(kytag05_class*) {
    WWHD_FUNC(0x021B0708, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B0708, daKytag05_Draw);

/* 021B0710 */
static BOOL daKytag05_Execute(kytag05_class* a_this) {
    WWHD_FUNC(0x021B0710, BOOL, a_this);
    u32 camera = dComIfGp_getCamera0();
    u32 player = dComIfGp_getPlayer0_ea();
    f32 windPow = dKyw_get_wind_pow();
    f32 blend = 1.0f;

    if (gabi::load<u8>(env_ea() + ENV_EVTWINDSET) == 0xFF) {
        return TRUE;
    }

    if (dComIfGp_event_runCheck() && dComIfGp_evmng_startCheckOld(STR(0x10013BF0) /* "demo41" */)) {
        /* HD: no dComIfGp_demo_get() null check; the `demoFrame == 0x187` (daYkgr_c::stop) branch
         * is unreachable after `>= 0x186` and was dropped */
        u32 demoFrame = dDemo_getFrame();
        if (demoFrame >= 0x186) {
            f32 fVar7 = ((f32)demoFrame - 390.0f) / 100.0f;
            if (fVar7 > 1.0f) {
                fVar7 = 1.0f;
            }
            blend = 1.0f - fVar7;
            gabi::store<s32>(env_ea() + ENV_SNOWCOUNT, gabi::ftoi(200.0f * blend));
        }
    }
    dKy_custom_colset(0, 7, blend);

    /* HD: the table index is clamped, and the new wind direction is taken at the index before
     * the increment */
    s32 idx = a_this->mIndex >> 1;
    if (idx > 3) {
        idx = 0;
    }
    if ((a_this->mIndex & 1) == 0) {
        if (a_this->mTimer >= fuu_timer(idx)) {
            a_this->mTimer = 0;
            a_this->mIndex = a_this->mIndex + 1;
            gabi::store<u8>(env_ea() + ENV_EVTWINDSET, 2);
        } else {
            a_this->mTimer = a_this->mTimer + 1;
        }
    } else {
        if (a_this->mTimer >= mufuu_timer(idx)) {
            u8 next = a_this->mIndex + 1;
            if ((next >> 1) >= 4) {
                a_this->mIndex = 0;
            } else {
                a_this->mIndex = next;
            }
            dKyw_evt_wind_set(0, wind_table(idx));
            a_this->mTimer = 0;
            gabi::store<u8>(env_ea() + ENV_EVTWINDSET, 1);
        } else {
            a_this->mTimer = a_this->mTimer + 1;
        }
    }

    f32 camZ = gabi::load<f32>(camera + 0xE4);
    if ((camZ > 1445.0f || gabi::load<f32>(player + 0x31C) > 1445.0f) &&
        (gabi::load<f32>(camera + 0xDC) > 520.0f || gabi::load<f32>(player + 0x314) > 520.0f)) {
        if (camZ > 2100.0f || gabi::load<f32>(player + 0x31C) > 2100.0f) {
            dKyw_evt_wind_set(0, 0x61A8);
        } else if (camZ > 1970.0f || gabi::load<f32>(player + 0x31C) > 1970.0f) {
            dKyw_evt_wind_set(0, 0x4E20);
        } else {
            dKyw_evt_wind_set(0, 0x4650);
        }
    } else if (camZ < -4085.0f || gabi::load<f32>(player + 0x31C) < -4085.0f) {
        dKyw_evt_wind_set(0, -0x3E80);
    } else if (camZ < -3108.0f || gabi::load<f32>(player + 0x31C) < -3108.0f) {
        dKyw_evt_wind_set(0, -0x4B00);
    } else if (camZ < -1412.0f || gabi::load<f32>(player + 0x31C) < -1412.0f) {
        dKyw_evt_wind_set(0, -0x32C8);
    }

    mDoAud_seStart3(JA_SE_ATM_WIND_VAR, nullptr, f2u(windPow * 100.0f));

    return TRUE;
}
VERIFY(0x021B0710, daKytag05_Execute);

/* 021B0B4C */
static BOOL daKytag05_IsDelete(kytag05_class*) {
    WWHD_FUNC(0x021B0B4C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B0B4C, daKytag05_IsDelete);

/* 021B0B54 */
static BOOL daKytag05_Delete(kytag05_class*) {
    WWHD_FUNC(0x021B0B54, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B0B54, daKytag05_Delete);

/* 021B0B5C */
static cPhs_State daKytag05_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021B0B5C, cPhs_State, i_this);
    dKy_getEnvlight(); /* HD: a discarded env-light accessor call */
    /* fopAcM_ct_Retail(i_this, kytag05_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = KYTAG05_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    kytag05_class* a_this = (kytag05_class*)i_this;
    if (dComIfGs_isSymbol(1 /* dSymbol_DIN_e */)) {
        return 3; /* cPhs_STOP_e */
    }

    a_this->mIndex = 0;
    a_this->mTimer = 0;
    a_this->mUnknownParam = fopAcM_GetParam(i_this) & 0xff;
    dKyw_evt_wind_set_go();
    dKyw_evt_wind_set(0, 0);
    gabi::store<s32>(env_ea() + ENV_SNOWCOUNT, 200);
    gabi::store<u8>(env_ea() + ENV_MOYAMODE, 0);
    gabi::store<s32>(env_ea() + ENV_MOYACOUNT, 30); /* HD: 30 (GameCube 100) */

    return cPhs_COMPLEATE_e;
}
VERIFY(0x021B0B5C, daKytag05_Create);

/* 021B0C54: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_kytag05_cpp() {
    WWHD_FUNC(0x021B0C54, void, (u32)0);
    sinit_header_statics(0x10464D54, 0x101B8D0C);
}
VERIFY(0x021B0C54, __sinit_d_a_kytag05_cpp);

/* 021B0CE8: kytag05_class deleting destructor (compiler-generated, HD virtual destructor) */
static void kytag05_class_dt(kytag05_class* i_this, s32 flags) {
    WWHD_FUNC(0x021B0CE8, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021B0CE8, kytag05_class_dt);
