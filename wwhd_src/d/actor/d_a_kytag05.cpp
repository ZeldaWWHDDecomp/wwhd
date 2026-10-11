/** Weather tag 5: Tower of the Gods / Hyrule wind cycle. */
#include "d/actor/d_a_kytag05.h"

const gc_tu_data gc_tu = {
    .headerStatics = {0x10464D54, 0x101B8D0C, 0x10464D60},
};
static const s16 wind_table[] = {0, -32767, -16384, 16384};
static const s16 mufuu_timer[] = {10, 10, 0, 90};
static const s16 fuu_timer[] = {150, 150, 150, 150};
static u32 l_demoFrame;

/* (u32)f, GHS: through 2^31 */
// HD: GHS converts unsigned sound parameters through the signed conversion instruction.
static inline u32 windSoundParameter(f32 f) {
    if (f < 2147483648.0f) return (u32)gabi::ftoi(f);
    return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}

/* 021B0708 */
static BOOL daKytag05_Draw(kytag05_class*) {
    return TRUE;
}

/* 021B0710 */
static BOOL daKytag05_Execute(kytag05_class* a_this) {
    Kytag05Camera* camera = kytag05_getCamera(0);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    f32 windPow = dKyw_get_wind_pow();
    f32 blend = 1.0f;

    if (g_env_light.mWind.mEvtWindSet == 0xFF) {
        return TRUE;
    }

    if (dComIfGp_event_runCheck() && kytag05_startCheckOld("demo41")) {
        // HD: the demo frame is global, without a null check. The unreachable stop branch is absent.
        u32 demoFrame = l_demoFrame;
        if (demoFrame >= 0x186) {
            f32 fVar7 = ((f32)demoFrame - 390.0f) / 100.0f;
            if (fVar7 > 1.0f) {
                fVar7 = 1.0f;
            }
            blend = 1.0f - fVar7;
            g_env_light.mSnowCount = gabi::ftoi(200.0f * blend);
        }
    }
    dKy_custom_colset(0, 7, blend);

    // HD: clamp the table index and select the wind direction before incrementing it.
    s32 idx = a_this->mIndex >> 1;
    if (idx > 3) {
        idx = 0;
    }
    if ((a_this->mIndex & 1) == 0) {
        if (a_this->mTimer >= fuu_timer[idx]) {
            a_this->mTimer = 0;
            a_this->mIndex = a_this->mIndex + 1;
            g_env_light.mWind.mEvtWindSet = 2;
        } else {
            a_this->mTimer = a_this->mTimer + 1;
        }
    } else {
        if (a_this->mTimer >= mufuu_timer[idx]) {
            u8 next = a_this->mIndex + 1;
            if ((next >> 1) >= 4) {
                a_this->mIndex = 0;
            } else {
                a_this->mIndex = next;
            }
            dKyw_evt_wind_set(0, wind_table[idx]);
            a_this->mTimer = 0;
            g_env_light.mWind.mEvtWindSet = 1;
        } else {
            a_this->mTimer = a_this->mTimer + 1;
        }
    }

    f32 camZ = camera->view.mLookat.mEye.z;
    if ((camZ > 1445.0f || player->current.pos.z > 1445.0f) &&
        (camera->view.mLookat.mEye.x > 520.0f || player->current.pos.x > 520.0f)) {
        if (camZ > 2100.0f || player->current.pos.z > 2100.0f) {
            dKyw_evt_wind_set(0, 0x61A8);
        } else if (camZ > 1970.0f || player->current.pos.z > 1970.0f) {
            dKyw_evt_wind_set(0, 0x4E20);
        } else {
            dKyw_evt_wind_set(0, 0x4650);
        }
    } else if (camZ < -4085.0f || player->current.pos.z < -4085.0f) {
        dKyw_evt_wind_set(0, -0x3E80);
    } else if (camZ < -3108.0f || player->current.pos.z < -3108.0f) {
        dKyw_evt_wind_set(0, -0x4B00);
    } else if (camZ < -1412.0f || player->current.pos.z < -1412.0f) {
        dKyw_evt_wind_set(0, -0x32C8);
    }

    mDoAud_seStartHD(JA_SE_ATM_WIND_VAR, nullptr, windSoundParameter(windPow * 100.0f));

    return TRUE;
}

/* 021B0B4C */
static BOOL daKytag05_IsDelete(kytag05_class*) {
    return TRUE;
}

/* 021B0B54 */
static BOOL daKytag05_Delete(kytag05_class*) {
    return TRUE;
}

/* 021B0B5C */
static cPhs_State daKytag05_Create(fopAc_ac_c* i_this) {
    dKy_getEnvlight(); // HD: retain the discarded environment-light accessor call.
    /* fopAcM_ct_Retail(i_this, kytag05_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = kytag05_class::VTABLE;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    kytag05_class* a_this = (kytag05_class*)i_this;
    if (dComIfGs_isSymbol(dSymbol_DIN_e)) {
        return cPhs_STOP_e;
    }

    a_this->mIndex = 0;
    a_this->mTimer = 0;
    a_this->mUnknownParam = fopAcM_GetParam(i_this) & 0xff;
    dKyw_evt_wind_set_go();
    dKyw_evt_wind_set(0, 0);
    g_env_light.mSnowCount = 200;
    g_env_light.mMoyaMode = 0;
    g_env_light.mMoyaCount = 30; // HD: GameCube uses 100.

    return cPhs_COMPLEATE_e;
}

// HD: compiler-generated and template code of this translation unit.
/* 021B0C54 */
static void __sinit_d_a_kytag05_cpp() {
    __sinit_header_statics();
}

// HD: deleting destructor for the virtual actor base.
/* 021B0CE8 */
static void kytag05_class_dt(kytag05_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(i_this);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit tables, frame counter and external calls
 data wind_table 10013BD8
 data mufuu_timer 10013BE0
 data fuu_timer 10013BE8
 data l_demoFrame 101D6008
 str 10013BF0 "demo41"
 ext 0257E684 void dKyw_evt_wind_set(s16, s16)
 ext 0257E6C4 void dKyw_evt_wind_set_go()
 ext 0255FDA4 void dKy_custom_colset(u8, u8, f32)
 ext 025E1A04 void mDoAud_seStartHD(u32, cXyz*, u32)
 ext 025445B8 BOOL dEvent_manager_startCheckOld(dEvent_manager_c*, const char*)
*/
