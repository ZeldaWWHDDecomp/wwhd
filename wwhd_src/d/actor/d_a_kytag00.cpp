/**
 * d_a_kytag00.cpp (WWHD)
 * Environment tag 00: weather area (colour pattern blend and rain/snow/fog/thunder effects
 * within a cylinder around the tag).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kytag00.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_kytag00.h"
// HD: this unit compares the rounded sqrtf result, rather than cXyz::abs's double return.
static f32 cXyz_abs(const cXyz *a, const cXyz *b) {
    return std_sqrtf(PSVECSquareDistance(a, b));
}
// HD: the switch room is the home room, or the stay room when it is negative.
static s32 home_or_stay_room(fopAc_ac_c *actor) {
    s32 room = actor->home.roomNo;
    if (room < 0)
        room = dComIfGp_roomControl_getStayNo();
    return room;
}
// HD: GHS hidden result may be null; allocate 12 bytes and store y/x/z, leaving r3 unchanged.
/* 021AE1EC */
static void get_check_pos(cXyz *ret, kytag00_class *i_this) {
    fopAc_ac_c *actor = i_this;
    kytag00_camera_c *pCamera = kytag00_getCamera();
    fopAc_ac_c *pPlayer = dComIfGp_getPlayer(0);

    f32 cameraDist = cXyz_abs(&actor->current.pos, &pCamera->mEye);
    f32 playerDist = cXyz_abs(&actor->current.pos, &pPlayer->current.pos);

    f32 x, y, z;
    if (dComIfGp_event_runCheck() && i_this->mMode == 0 && cameraDist < playerDist) {
        y = (f32)pCamera->mEye.y;
        z = (f32)pCamera->mEye.z;
        x = (f32)pCamera->mEye.x;
    } else {
        y = pPlayer->current.pos.y;
        z = pPlayer->current.pos.z;
        x = pPlayer->current.pos.x;
    }
    if (ret == nullptr) {
        ret = (cXyz *)operator_new(0xC);
        if (ret == nullptr)
            return;
    }
    ret->y = y;
    ret->x = x;
    ret->z = z;
}

/* The area test and blend shared by wether_tag_move and wether_tag_efect_move. GHS tests
 * `chk.y >= bottom` as !(chk.y < bottom), `chk.y <= y` as !(chk.y > y), and clamps with fsel. */
static inline bool area_blend(kytag00_class *i_this, const cXyz *chk_pos, f32 fade_y, f32 dist_xz,
                              f32 *o_blend, bool efect) {
    fopAc_ac_c *actor = i_this;
    if (!(dist_xz < i_this->mOuterRadius))
        return false;
    if (chk_pos->y < actor->current.pos.y - fade_y)
        return false;
    if (!(chk_pos->y < gabi::fmadds(actor->scale.y, 5000.0f, actor->current.pos.y) + fade_y))
        return false;
    if (!(i_this->mTarget > 0.0f))
        return false;

    f32 blend = 1.0f;
    f32 f9 = 1.0f;
    if (efect)
        i_this->mbEfSet = 1;
    f32 fade_radius = i_this->mOuterRadius - i_this->mInnerRadius;
    if (fade_radius != 0.0f) {
        blend = (i_this->mOuterRadius - dist_xz) / fade_radius;
        if (blend > 1.0f)
            blend = 1.0f;
    }
    if (fade_y != 0.0f) {
        f32 cy = actor->current.pos.y;
        if (!(chk_pos->y > cy)) {
            f9 = (cy - chk_pos->y) / fade_y;
        } else {
            f9 = (chk_pos->y - gabi::fmadds(actor->scale.y, 5000.0f, cy)) / fade_y;
        }
        if (!(f9 >= 0.0f)) /* fsel */
            f9 = 0.0f;
        f9 = 1.0f - f9;
    }
    blend = blend * (f9 * i_this->mTarget);
    *o_blend = blend;
    return true;
}

/* colour pattern gather: prev <- weather (destination address taken before the source's) */
static inline void set_prev_from_weather() {
    kytag00_env_c *dst = kytag00_env();
    dst->mColpatPrevGather = kytag00_env()->mColpatWeather;
}
static inline void set_curr_from_weather() {
    kytag00_env_c *dst = kytag00_env();
    dst->mColpatCurrGather = kytag00_env()->mColpatWeather;
}

/* inlined into daKytag00_Execute */
static inline void wether_tag_move(kytag00_class *i_this) {
    fopAc_ac_c *actor = i_this;
    gabi::Local<cXyz> chk_pos;
    get_check_pos(chk_pos, i_this);
    f32 fade_y = (f32)i_this->mInnerFadeY * 100.0f;
    gabi::Local<cXyz> chk_pos_xz;
    chk_pos_xz->x = chk_pos->x;
    chk_pos_xz->y = actor->current.pos.y;
    chk_pos_xz->z = chk_pos->z;
    f32 dist_xz = cXyz_abs(&actor->current.pos, chk_pos_xz);

    f32 blend;
    if (area_blend(i_this, chk_pos, fade_y, dist_xz, &blend, false)) {
        kytag00_env_c *e1 = kytag00_env();
        kytag00_env_c *e2 = kytag00_env();
        if ((u8)e1->mEnvrIdxPrev != (u8)e2->mEnvrIdxCurr)
            return;
        i_this->mbPselSet = true;

        u8 idx = i_this->mPselIdx;
        if (idx == 0) {
            set_prev_from_weather();
            kytag00_env()->mColpatCurrGather = 0;
            kytag00_env()->mColPatBlendGather = blend;
            kytag00_env()->mColPatModeGather = 1;
        } else if (idx <= 3) {
            if (blend > 0.5f) {
                set_prev_from_weather();
                kytag00_env()->mColpatCurrGather = idx;
                kytag00_env()->mColPatBlendGather = blend;
            } else {
                blend = 1.0f - blend;
                kytag00_env()->mColpatPrevGather = idx;
                set_curr_from_weather();
                kytag00_env()->mColPatBlendGather = blend;
            }
            kytag00_env()->mColPatModeGather = 1;
        } else {
            i_this->mbPselSet = false;
        }
    } else {
        if (i_this->mbPselSet) {
            i_this->mbPselSet = false;
            set_prev_from_weather();
            set_curr_from_weather();
            kytag00_env()->mColPatBlendGather = 0.0f;
            kytag00_env()->mColPatModeGather = 1;
        }
    }
}

/* 021AE314 */
static void raincnt_set(f32 count) {
    s32 newCount = 0;

    BOOL nightStop = dKy_checkEventNightStop();
    s32 c = gabi::ftoi(250.0f * (count * count * count));
    if (nightStop) {
        if (kytag00_env()->mRainCount < c)
            newCount = c;
    } else {
        newCount = c;
    }

    if (newCount > kytag00_env()->mRainCountOrig)
        kytag00_env()->mRainCount = newCount;
}

/* 021AE3C4 */
static void raincnt_cut() {
    if (!dKy_checkEventNightStop()) {
        kytag00_env_c *dst = kytag00_env();
        dst->mRainCount = kytag00_env()->mRainCountOrig;
    }
}

/* 021AE408 */
static void wether_tag_efect_move(kytag00_class *i_this) {
    fopAc_ac_c *actor = i_this;
    gabi::Local<cXyz> chk_pos;
    get_check_pos(chk_pos, i_this);
    f32 fade_y = (f32)i_this->mInnerFadeY * 100.0f;
    gabi::Local<cXyz> chk_pos_xz;
    chk_pos_xz->x = chk_pos->x;
    chk_pos_xz->y = actor->current.pos.y;
    chk_pos_xz->z = chk_pos->z;
    f32 dist_xz = cXyz_abs(&actor->current.pos, chk_pos_xz);

    f32 blend;
    if (area_blend(i_this, chk_pos, fade_y, dist_xz, &blend, true)) {
        /* HD: fog (moya) counts scale by 30 (GameCube 100) */
        switch (i_this->mEfMode) {
        case KYTAG00_RAIN:
            raincnt_set(blend);
            break;
        case KYTAG00_SNOW:
            kytag00_env()->mSnowCount = gabi::ftoi(250.0f * blend);
            kytag00_env()->mMoyaMode = 2;
            kytag00_env()->mMoyaCount = gabi::ftoi(30.0f * blend);
            break;
        case KYTAG00_FOG:
            kytag00_env()->mMoyaMode = 0;
            kytag00_env()->mMoyaCount = gabi::ftoi(30.0f * blend);
            break;
        case KYTAG00_FOG_1:
            kytag00_env()->mMoyaMode = 1;
            kytag00_env()->mMoyaCount = gabi::ftoi(30.0f * blend);
            break;
        case KYTAG00_FOG_2:
            kytag00_env()->mMoyaMode = 2;
            kytag00_env()->mMoyaCount = gabi::ftoi(30.0f * blend);
            break;
        case KYTAG00_HOUSI:
            if (actor->home.roomNo == dComIfGp_roomControl_getStayNo()) {
                kytag00_env()->mHousiCount = gabi::ftoi(300.0f * blend);
            } else {
                kytag00_env()->mHousiCount = 0;
            }
            break;
        case KYTAG00_THUNDER:
            if (kytag00_env()->mThunderEff.mMode == 0)
                kytag00_env()->mThunderEff.mMode = 2;
            break;
        case KYTAG00_THUNDER_RAIN:
            if (kytag00_env()->mThunderEff.mMode == 0)
                kytag00_env()->mThunderEff.mMode = 2;
            raincnt_set(blend);
            break;
        case KYTAG00_THUNDER_RAIN_FOG:
            kytag00_env()->mMoyaMode = 0;
            kytag00_env()->mMoyaCount = gabi::ftoi(30.0f * blend);
            raincnt_set(blend);
            if (kytag00_env()->mThunderEff.mMode == 0)
                kytag00_env()->mThunderEff.mMode = 2;
            break;
        case KYTAG00_FOG_3:
            kytag00_env()->mMoyaMode = 3;
            kytag00_env()->mMoyaCount = gabi::ftoi(30.0f * blend);
            break;
        case KYTAG00_FOG_4:
            kytag00_env()->mMoyaMode = 4;
            kytag00_env()->mMoyaCount = gabi::ftoi(30.0f * blend);
            break;
        }
    } else if (i_this->mbEfSet != 0) {
        u8 mode = i_this->mEfMode;
        i_this->mbEfSet = 0;

        switch (mode) {
        case KYTAG00_RAIN:
            raincnt_cut();
            break;
        case KYTAG00_SNOW:
            kytag00_env()->mSnowCount = 0;
            kytag00_env()->mMoyaCount = 0;
            break;
        case KYTAG00_FOG:
        case KYTAG00_FOG_1:
        case KYTAG00_FOG_2:
        case KYTAG00_FOG_3:
            kytag00_env()->mMoyaCount = 0;
            break;
        case KYTAG00_HOUSI:
            kytag00_env()->mHousiCount = 0;
            break;
        case KYTAG00_THUNDER:
        case KYTAG00_THUNDER_RAIN:
        case KYTAG00_THUNDER_RAIN_FOG:
            if (kytag00_env()->mThunderEff.mMode == 2)
                kytag00_env()->mThunderEff.mMode = 0;
            if (i_this->mEfMode == KYTAG00_THUNDER_RAIN ||
                i_this->mEfMode == KYTAG00_THUNDER_RAIN_FOG)
                raincnt_cut();
            if (i_this->mEfMode == KYTAG00_THUNDER_RAIN_FOG)
                kytag00_env()->mMoyaCount = 0;
            break;
        }
    }
}

/* 021AEB34 */
static BOOL daKytag00_Draw(kytag00_class *i_this) {
    wether_tag_efect_move(i_this);
    return TRUE;
}

/* 021AEB58 */
static BOOL daKytag00_Execute(kytag00_class *i_this) {
    s32 room = home_or_stay_room(i_this); /* HD (GameCube: the stay room) */
    if (!i_this->mbInvert) {
        if (i_this->mSwitchNo != 0xFF && dComIfGs_isSwitch(i_this->mSwitchNo, room)) {
            cLib_addCalc(&i_this->mTarget, 0.0f, 0.1f, 0.01f, 0.0001f);
        } else {
            cLib_addCalc(&i_this->mTarget, 1.0f, 0.1f, 0.01f, 0.0001f);
        }
    } else {
        if (i_this->mSwitchNo != 0xFF && dComIfGs_isSwitch(i_this->mSwitchNo, room)) {
            cLib_addCalc(&i_this->mTarget, 1.0f, 0.1f, 0.01f, 0.0001f);
        } else {
            cLib_addCalc(&i_this->mTarget, 0.0f, 0.1f, 0.01f, 0.0001f);
        }
    }

    wether_tag_move(i_this);
    return TRUE;
}

/* 021AF0B0 */
static BOOL daKytag00_IsDelete(kytag00_class *i_this) {
    return TRUE;
}

/* 021AF0B8 */
static BOOL daKytag00_Delete(kytag00_class *i_this) {
    kytag00_env()->mMoyaCount = 0;
    return TRUE;
}

/* 021AF0E4 */
static cPhs_State daKytag00_Create(fopAc_ac_c *i_ac) {
    kytag00_class *i_this = (kytag00_class *)i_ac;

    /* fopAcM_ct(i_ac, kytag00_class) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr) {
            fopAc_ac_c_ct(i_ac);
            i_ac->__vtbl = kytag00_class::__vt;
        }
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }
    s32 room = home_or_stay_room(i_this); /* HD (GameCube: the stay room) */

    i_this->field_0x296 = 0;
    i_this->mPselIdx = (fopAcM_GetParam(i_ac) >> 0) & 0xFF;
    i_this->mEfMode = (fopAcM_GetParam(i_ac) >> 8) & 0xFF;
    i_this->mThickness = (fopAcM_GetParam(i_ac) >> 16) & 0xFF;
    i_this->mInnerFadeY = (fopAcM_GetParam(i_ac) >> 24) & 0xFF;
    i_this->mSwitchNo = (i_ac->current.angle.x >> 0) & 0xFF;
    i_this->mbInvert = (i_ac->current.angle.x >> 8) & 0xFF;
    i_this->mMode = (i_ac->current.angle.z >> 0) & 0xFF;

    if (!i_this->mbInvert) {
        if (i_this->mSwitchNo != 0xFF && dComIfGs_isSwitch(i_this->mSwitchNo, room)) {
            i_this->mTarget = 0.0f;
        } else {
            i_this->mTarget = 1.0f;
        }
    } else {
        if (i_this->mSwitchNo != 0xFF && dComIfGs_isSwitch(i_this->mSwitchNo, room)) {
            i_this->mTarget = 1.0f;
        } else {
            i_this->mTarget = 0.0f;
        }
    }

    if (i_this->mThickness == 0xFF)
        i_this->mThickness = 10;

    if (i_this->mInnerFadeY == 0xFF)
        i_this->mInnerFadeY = 10;

    if (i_this->mMode == 0) {
        i_this->mInnerRadius = i_ac->scale.x * 5000.0f;
        i_this->mOuterRadius =
            gabi::fmadds(i_ac->scale.x, 5000.0f, (f32)i_this->mThickness * 100.0f);
    } else {
        i_this->mInnerRadius = i_ac->scale.x * 500.0f;
        i_this->mOuterRadius = gabi::fmadds(i_ac->scale.x, 500.0f, (f32)i_this->mThickness * 10.0f);
    }

    i_this->mbEfSet = false;
    i_this->mbPselSet = false;
    kytag00_env()->mMoyaCount = 0;
    wether_tag_efect_move(i_this);
    return cPhs_COMPLEATE_e;
}

// HD: compiler-generated and template code of this translation unit
/* 021AF34C */
static void __sinit_d_a_kytag00_cpp() {
    sinit_header_statics(0x10464CC8, 0x101B8AC8);
}

/* 021AF3E0 */
static void kytag00_class_dt(kytag00_class *i_this, s32 flags) {
    if (i_this != nullptr) {
        kytag00_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

// clang-format off
/* WWHD glue: private bindings
ext 028E8DE8 f32 PSVECSquareDistance(const cXyz*,const cXyz*)
ext 02556BC0 BOOL dKy_checkEventNightStop()
ext 0200ECD4 f32 cLib_addCalc(be<f32>*,f32,f32,f32,f32)
ext 025D50BC void kytag00_actorDestroy(void*,s32)
*/
// clang-format on
