/**
 * d_a_kytag07.cpp (WWHD)
 * Environment tag 07: Tower of the Gods / Hyrule weather (time flow, rain and fog during demos).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kytag07.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define KYTAG07_VTBL 0x10013C6C    /* kytag07_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x10013C54 /* this TU's sead::SafeString vtable */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0255FD48 dKy_change_colpat(u8) */
static inline void dKy_change_colpat(u8 pat) { gabi::call(0x0255FD48, pat); }
/* 0255FDA4 dKy_custom_colset(u8 prev, u8 next, f32 blend) */
static inline void dKy_custom_colset(u8 a, u8 b, f32 blend) { gabi::call(0x0255FDA4, a, b, blend); }
/* dComIfGp_event_runCheck(): play+0x5292 (u8) != 0 */
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
/* 025445B8 dEvent_manager_c::startCheckOld(const char*) */
static inline BOOL dComIfGp_evmng_startCheckOld(const char* name) { return gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), name); }
/* HD: dComIfGp_demo_get()->getFrameNoMsg(): a global (u32 at 0x101D600C), no null check */
static inline u32 dDemo_getFrameNoMsg() { return gabi::load<u32>(0x101D600C); }
/* strcmp(dComIfGp_getStartStageName(), lit) == 0, HD: sead::SafeString operator== (copied from
 * d_a_bk_action): the virtual at vtable +0x14 is called twice on the literal's SafeString and once
 * on the stage name's (play+0x5134), then pointer compare and a bounded strcmp (0x40001 chars) */
static inline void SafeString_vcall(SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
static inline bool dComIfGp_checkStageName(const char* name) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::ea(name);
    a->__vtbl = SAFESTRING_VTBL;
    u32 stage = dComIfGp_ea() + 0x5134;
    gabi::Local<SafeString> b;
    b->__vtbl = SAFESTRING_VTBL;
    b->mStringTop = stage;
    SafeString_vcall(a);
    SafeString_vcall(a);
    u32 pa = a->mStringTop;
    SafeString_vcall(b);
    u32 pb = b->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        u8 cb = gabi::load<u8>(pb + i);
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
    return false;
}
static inline u32 dComIfGs_raw() { return gabi::load<u32>(0x101F84DC); }

/* g_env_light fields (dScnKy_env_light_c, HD offsets) */
enum : u32 {
    ENV_RAINCOUNT = 0xA40,      /* mRainCount (s32) */
    ENV_MOYAMODE = 0xA7D,       /* mMoyaMode (u8) */
    ENV_MOYACOUNT = 0xA80,      /* mMoyaCount (s32) */
    ENV_THUNDER_MODE = 0xAB4,   /* mThunderEff.mMode (s32) */
    ENV_TACTSTOP = 0x10A1,      /* mbDayNightTactStop (u8) */
};
static inline u32 env_ea() { return gabi::ea(dKy_getEnvlight()); }

struct kytag07_class : fopAc_ac_c {};
WWHD_SIZE(kytag07_class, 0x3AC);

/* 021B1004 */
static BOOL daKytag07_Draw(kytag07_class*) {
    WWHD_FUNC(0x021B1004, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B1004, daKytag07_Draw);

/* 021B100C */
static BOOL daKytag07_Execute(kytag07_class* i_this) {
    WWHD_FUNC(0x021B100C, BOOL, i_this);
    u32 envLight = env_ea();
    if (!dComIfGp_checkStageName(STR(0x10013CA0) /* "GTower" */)) {
        u32 save = dComIfGs_raw();
        f32 time = gabi::load<f32>(save + 0x44);
        u16 date = gabi::load<u16>(save + 0x48);
        if (time >= 60.0f && time < 315.0f)
            time += 0.4f;
        gabi::store<f32>(save + 0x44, time);          /* dComIfGs_setTime */
        gabi::store<u16>(dComIfGs_raw() + 0x48, date); /* dComIfGs_setDate */
    } else {
        u8 mode = fopAcM_GetParam(i_this) & 0xFF;
        if (dComIfGp_event_runCheck() == FALSE)
            return TRUE;
        if (mode != 1) {
            /* HD: no dComIfGp_demo_get() null check */
            if (dComIfGp_evmng_startCheckOld(STR(0x10013CA8) /* "g2before" */) && dDemo_getFrameNoMsg() >= 4719) {
                if (dDemo_getFrameNoMsg() == 4719)
                    dKy_change_colpat(1);
                if (gabi::load<s32>(envLight + ENV_RAINCOUNT) < 250)
                    gabi::store<s32>(envLight + ENV_RAINCOUNT, gabi::load<s32>(envLight + ENV_RAINCOUNT) + 1);
                if (gabi::load<s32>(env_ea() + ENV_MOYACOUNT) < 30) { /* HD: 30 (GameCube 100) */
                    gabi::store<u8>(env_ea() + ENV_MOYAMODE, 0);
                    u32 e = env_ea();
                    gabi::store<s32>(e + ENV_MOYACOUNT, gabi::load<s32>(e + ENV_MOYACOUNT) + 1);
                }
                if (gabi::load<s32>(env_ea() + ENV_THUNDER_MODE) == 0)
                    gabi::store<s32>(env_ea() + ENV_THUNDER_MODE, 2);
            }
        } else {
            /* HD: no dComIfGp_demo_get() null check */
            gabi::store<u8>(env_ea() + ENV_MOYAMODE, 0);
            if (dDemo_getFrameNoMsg() >= 2602) {
                gabi::store<s32>(envLight + ENV_RAINCOUNT, 0);
                gabi::store<s32>(envLight + ENV_MOYACOUNT, 0);
                gabi::store<s32>(env_ea() + ENV_THUNDER_MODE, 0);
            } else {
                gabi::store<s32>(envLight + ENV_RAINCOUNT, 250);
                gabi::store<s32>(envLight + ENV_MOYACOUNT, 30); /* HD: 30 (GameCube 100) */
                if (gabi::load<s32>(env_ea() + ENV_THUNDER_MODE) == 0)
                    gabi::store<s32>(env_ea() + ENV_THUNDER_MODE, 2);
            }

            f32 blend = 0.0f;
            u32 frame = dDemo_getFrameNoMsg();
            if (frame >= 2601) {
                blend = (f32)(frame - 2601) / 200.0f;
                if (blend > 1.0f)
                    blend = 1.0f;
            }
            dKy_custom_colset(1, 2, blend);
        }
    }
    return TRUE;
}
VERIFY(0x021B100C, daKytag07_Execute);

/* 021B131C */
static BOOL daKytag07_IsDelete(kytag07_class*) {
    WWHD_FUNC(0x021B131C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021B131C, daKytag07_IsDelete);

/* 021B1324 */
static BOOL daKytag07_Delete(kytag07_class*) {
    WWHD_FUNC(0x021B1324, BOOL, (u32)0);
    gabi::store<u8>(env_ea() + ENV_TACTSTOP, 0);
    return TRUE;
}
VERIFY(0x021B1324, daKytag07_Delete);

/* 021B1350 */
static cPhs_State daKytag07_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021B1350, cPhs_State, i_this);
    u32 env_light = env_ea();
    /* fopAcM_ct_Retail(i_this, kytag07_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = KYTAG07_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    if (!dComIfGp_checkStageName(STR(0x10013CB4) /* "GTower" */))
        gabi::store<u8>(env_light + ENV_TACTSTOP, 1);

    return cPhs_COMPLEATE_e;
}
VERIFY(0x021B1350, daKytag07_Create);

/* 021B1490: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_kytag07_cpp() {
    WWHD_FUNC(0x021B1490, void, (u32)0);
    sinit_header_statics(0x10464D8C, 0x101B8DF4);
}
VERIFY(0x021B1490, __sinit_d_a_kytag07_cpp);

/* 021B1524: sead::SafeString deleting destructor (this TU's copy; vtable 0x10013C54 +0xC) */
static void SafeString_dt(SafeString* s, s32 flags) {
    WWHD_FUNC(0x021B1524, void, s, flags);
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}
VERIFY(0x021B1524, SafeString_dt);

/* 021B1538: kytag07_class deleting destructor (compiler-generated, HD virtual destructor) */
static void kytag07_class_dt(kytag07_class* i_this, s32 flags) {
    WWHD_FUNC(0x021B1538, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021B1538, kytag07_class_dt);

/* 021B158C: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_; empty for this TU's
 * copy) */
static void SafeString_assureTermination(SafeString*) {
    WWHD_FUNC(0x021B158C, void, (u32)0);
}
VERIFY(0x021B158C, SafeString_assureTermination);
