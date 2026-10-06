/**
 * d_a_kytag00.cpp (WWHD)
 * Environment tag 00: weather area (colour pattern blend and rain/snow/fog/thunder effects
 * within a cylinder around the tag).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kytag00.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define KYTAG00_VTBL 0x100139D8 /* kytag00_class vtable (HD virtual destructor) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dStage_roomControl_c::mStayNo (s8 at 0x1047E6C8) */
static inline s8 dComIfGp_roomControl_getStayNo() { return gabi::load<s8>(0x1047E6C8); }
/* dComIfGp_event_runCheck(): play+0x5292 (u8) != 0 */
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
/* dComIfGp_getCamera(0): camera_class* at play+0x5AF8; view.mLookat.mEye at +0xDC */
static inline u32 dComIfGp_getCamera0() { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8); }
/* 028E8DE8 PSVECSquareDistance */
static inline f32 PSVECSquareDistance(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8DE8, a, b); }
/* cXyz::abs(other) = sqrtf(PSVECSquareDistance) */
static inline f32 cXyz_abs(const cXyz* a, const cXyz* b) { return std_sqrtf(PSVECSquareDistance(a, b)); }
/* 02556BC0 dKy_checkEventNightStop() */
static inline BOOL dKy_checkEventNightStop() { return gabi::call<BOOL>(0x02556BC0); }
/* 0200ECD4 cLib_addCalc(f32*, target, scale, maxStep, minStep) */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* HD: the switch room is the home room, or the stay room when it is negative */
static inline s32 home_or_stay_room(fopAc_ac_c* a) {
    s32 room = a->home.roomNo;
    if (room < 0) room = dComIfGp_roomControl_getStayNo();
    return room;
}

/* g_env_light (dScnKy_env_light_c, HD offsets). HD: every access goes through the accessor */
static inline u32 env_ea() { return gabi::ea(dKy_getEnvlight()); }
enum : u32 {
    ENV_RAINCOUNT = 0xA40,          /* mRainCount (s32) */
    ENV_SNOWCOUNT = 0xA4C,          /* mSnowCount (s32) */
    ENV_HOUSICOUNT = 0xA74,         /* mHousiCount (s32) */
    ENV_MOYAMODE = 0xA7D,           /* mMoyaMode (u8) */
    ENV_MOYACOUNT = 0xA80,          /* mMoyaCount (s32) */
    ENV_THUNDER_MODE = 0xAB4,       /* mThunderEff.mMode (s32) */
    ENV_COLPATBLENDGATHER = 0xFCC,  /* mColPatBlendGather (f32) */
    ENV_RAINCOUNTORIG = 0x106C,     /* mRainCountOrig (s32) */
    ENV_COLPATPREVGATHER = 0x108E,  /* mColpatPrevGather (u8) */
    ENV_COLPATCURRGATHER = 0x108F,  /* mColpatCurrGather (u8) */
    ENV_ENVRIDXPREV = 0x1090,       /* mEnvrIdxPrev (u8) */
    ENV_ENVRIDXCURR = 0x1091,       /* mEnvrIdxCurr (u8) */
    ENV_COLPATWEATHER = 0x1092,     /* mColpatWeather (u8) */
    ENV_COLPATMODEGATHER = 0x1098,  /* mColPatModeGather (u8) */
};
template <class T> static inline void env_set(u32 off, T v) { gabi::store<T>(env_ea() + off, v); }
template <class T> static inline T env_get(u32 off) { return gabi::load<T>(env_ea() + off); }

struct kytag00_class : fopAc_ac_c {
    /* 0x3AC */ be<u32> field_0x290;
    /* 0x3B0 */ be<u8> mbEfSet;
    /* 0x3B1 */ be<u8> mbPselSet;
    /* 0x3B2 */ be<u8> field_0x296;
    /* 0x3B3 */ be<u8> mPselIdx;
    /* 0x3B4 */ be<u8> mEfMode;
    /* 0x3B5 */ u8 _3B5[3];
    /* 0x3B8 */ be<s32> mThickness;
    /* 0x3BC */ be<s32> mInnerFadeY;
    /* 0x3C0 */ be<f32> mInnerRadius;
    /* 0x3C4 */ be<f32> mOuterRadius;
    /* 0x3C8 */ be<f32> mTarget;
    /* 0x3CC */ be<u8> mSwitchNo;
    /* 0x3CD */ be<u8> mbInvert;
    /* 0x3CE */ be<u8> mMode;
    /* 0x3CF */ u8 _3CF;
};
WWHD_SIZE(kytag00_class, 0x3D0);

/* 021AE1EC: cXyz get_check_pos(kytag00_class*). HD/GHS: the result is returned through a hidden
 * pointer (r3), allocated (operator new, 12 bytes) when it is NULL; r3 is not set on return */
static void get_check_pos(cXyz* ret, kytag00_class* i_this) {
    WWHD_FUNC(0x021AE1EC, void, ret, i_this);
    fopAc_ac_c* actor = i_this;
    u32 pCamera = dComIfGp_getCamera0();
    fopAc_ac_c* pPlayer = dComIfGp_getPlayer(0);

    f32 cameraDist = cXyz_abs(&actor->current.pos, gabi::at<cXyz>(pCamera + 0xDC));
    f32 playerDist = cXyz_abs(&actor->current.pos, &pPlayer->current.pos);

    f32 x, y, z;
    if (dComIfGp_event_runCheck() && i_this->mMode == 0 && cameraDist < playerDist) {
        y = gabi::load<f32>(pCamera + 0xE0);
        z = gabi::load<f32>(pCamera + 0xE4);
        x = gabi::load<f32>(pCamera + 0xDC);
    } else {
        y = pPlayer->current.pos.y;
        z = pPlayer->current.pos.z;
        x = pPlayer->current.pos.x;
    }
    if (ret == nullptr) {
        ret = (cXyz*)operator_new(0xC);
        if (ret == nullptr)
            return;
    }
    ret->y = y;
    ret->x = x;
    ret->z = z;
}
VERIFY(0x021AE1EC, get_check_pos);

/* 021AE314 */
static void raincnt_set(f32 count) {
    WWHD_FUNC(0x021AE314, void, count);
    s32 newCount = 0;

    BOOL nightStop = dKy_checkEventNightStop();
    s32 c = gabi::ftoi(250.0f * (count * count * count));
    if (nightStop) {
        if (env_get<s32>(ENV_RAINCOUNT) < c)
            newCount = c;
    } else {
        newCount = c;
    }

    if (newCount > env_get<s32>(ENV_RAINCOUNTORIG))
        env_set<s32>(ENV_RAINCOUNT, newCount);
}
VERIFY(0x021AE314, raincnt_set);

/* 021AE3C4 */
static void raincnt_cut() {
    WWHD_FUNC(0x021AE3C4, void, (u32)0);
    if (!dKy_checkEventNightStop()) {
        u32 dst = env_ea();
        gabi::store<s32>(dst + ENV_RAINCOUNT, env_get<s32>(ENV_RAINCOUNTORIG));
    }
}
VERIFY(0x021AE3C4, raincnt_cut);

/* The area test and blend shared by wether_tag_move and wether_tag_efect_move. GHS tests
 * `chk.y >= bottom` as !(chk.y < bottom), `chk.y <= y` as !(chk.y > y), and clamps with fsel. */
static inline bool area_blend(kytag00_class* i_this, const cXyz* chk_pos, f32 fade_y, f32 dist_xz, f32* o_blend, bool efect) {
    fopAc_ac_c* actor = i_this;
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

/* 021AE408 */
static void wether_tag_efect_move(kytag00_class* i_this) {
    WWHD_FUNC(0x021AE408, void, i_this);
    fopAc_ac_c* actor = i_this;
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
        case 0x1:
            raincnt_set(blend);
            break;
        case 0x2:
            env_set<s32>(ENV_SNOWCOUNT, gabi::ftoi(250.0f * blend));
            env_set<u8>(ENV_MOYAMODE, 2);
            env_set<s32>(ENV_MOYACOUNT, gabi::ftoi(30.0f * blend));
            break;
        case 0x3:
            env_set<u8>(ENV_MOYAMODE, 0);
            env_set<s32>(ENV_MOYACOUNT, gabi::ftoi(30.0f * blend));
            break;
        case 0x4:
            env_set<u8>(ENV_MOYAMODE, 1);
            env_set<s32>(ENV_MOYACOUNT, gabi::ftoi(30.0f * blend));
            break;
        case 0x5:
            env_set<u8>(ENV_MOYAMODE, 2);
            env_set<s32>(ENV_MOYACOUNT, gabi::ftoi(30.0f * blend));
            break;
        case 0x6:
            if (actor->home.roomNo == dComIfGp_roomControl_getStayNo()) {
                env_set<s32>(ENV_HOUSICOUNT, gabi::ftoi(300.0f * blend));
            } else {
                env_set<s32>(ENV_HOUSICOUNT, 0);
            }
            break;
        case 0x7:
            if (env_get<s32>(ENV_THUNDER_MODE) == 0)
                env_set<s32>(ENV_THUNDER_MODE, 2);
            break;
        case 0x8:
            if (env_get<s32>(ENV_THUNDER_MODE) == 0)
                env_set<s32>(ENV_THUNDER_MODE, 2);
            raincnt_set(blend);
            break;
        case 0x9:
            env_set<u8>(ENV_MOYAMODE, 0);
            env_set<s32>(ENV_MOYACOUNT, gabi::ftoi(30.0f * blend));
            raincnt_set(blend);
            if (env_get<s32>(ENV_THUNDER_MODE) == 0)
                env_set<s32>(ENV_THUNDER_MODE, 2);
            break;
        case 0xA:
            env_set<u8>(ENV_MOYAMODE, 3);
            env_set<s32>(ENV_MOYACOUNT, gabi::ftoi(30.0f * blend));
            break;
        case 0xB:
            env_set<u8>(ENV_MOYAMODE, 4);
            env_set<s32>(ENV_MOYACOUNT, gabi::ftoi(30.0f * blend));
            break;
        }
    } else if (i_this->mbEfSet != 0) {
        u8 mode = i_this->mEfMode;
        i_this->mbEfSet = 0;

        switch (mode) {
        case 0x1:
            raincnt_cut();
            break;
        case 0x2:
            env_set<s32>(ENV_SNOWCOUNT, 0);
            env_set<s32>(ENV_MOYACOUNT, 0);
            break;
        case 0x3:
        case 0x4:
        case 0x5:
        case 0xA:
            env_set<s32>(ENV_MOYACOUNT, 0);
            break;
        case 0x6:
            env_set<s32>(ENV_HOUSICOUNT, 0);
            break;
        case 0x7:
        case 0x8:
        case 0x9:
            if (env_get<s32>(ENV_THUNDER_MODE) == 2)
                env_set<s32>(ENV_THUNDER_MODE, 0);
            if (i_this->mEfMode == 8 || i_this->mEfMode == 9)
                raincnt_cut();
            if (i_this->mEfMode == 9)
                env_set<s32>(ENV_MOYACOUNT, 0);
            break;
        }
    }
}
VERIFY(0x021AE408, wether_tag_efect_move);

/* 021AEB34 */
static BOOL daKytag00_Draw(kytag00_class* i_this) {
    WWHD_FUNC(0x021AEB34, BOOL, i_this);
    wether_tag_efect_move(i_this);
    return TRUE;
}
VERIFY(0x021AEB34, daKytag00_Draw);

/* colour pattern gather: prev <- weather (destination address taken before the source's) */
static inline void set_prev_from_weather() {
    u32 dst = env_ea();
    gabi::store<u8>(dst + ENV_COLPATPREVGATHER, env_get<u8>(ENV_COLPATWEATHER));
}
static inline void set_curr_from_weather() {
    u32 dst = env_ea();
    gabi::store<u8>(dst + ENV_COLPATCURRGATHER, env_get<u8>(ENV_COLPATWEATHER));
}

/* wether_tag_move (inlined into Execute) */
static inline void wether_tag_move(kytag00_class* i_this) {
    fopAc_ac_c* actor = i_this;
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
        u32 e1 = env_ea();
        u32 e2 = env_ea();
        if (gabi::load<u8>(e1 + ENV_ENVRIDXPREV) != gabi::load<u8>(e2 + ENV_ENVRIDXCURR))
            return;
        i_this->mbPselSet = true;

        u8 idx = i_this->mPselIdx;
        if (idx == 0) {
            set_prev_from_weather();
            env_set<u8>(ENV_COLPATCURRGATHER, 0);
            env_set<f32>(ENV_COLPATBLENDGATHER, blend);
            env_set<u8>(ENV_COLPATMODEGATHER, 1);
        } else if (idx <= 3) {
            if (blend > 0.5f) {
                set_prev_from_weather();
                env_set<u8>(ENV_COLPATCURRGATHER, idx);
                env_set<f32>(ENV_COLPATBLENDGATHER, blend);
            } else {
                blend = 1.0f - blend;
                env_set<u8>(ENV_COLPATPREVGATHER, idx);
                set_curr_from_weather();
                env_set<f32>(ENV_COLPATBLENDGATHER, blend);
            }
            env_set<u8>(ENV_COLPATMODEGATHER, 1);
        } else {
            i_this->mbPselSet = false;
        }
    } else {
        if (i_this->mbPselSet) {
            i_this->mbPselSet = false;
            set_prev_from_weather();
            set_curr_from_weather();
            env_set<f32>(ENV_COLPATBLENDGATHER, 0.0f);
            env_set<u8>(ENV_COLPATMODEGATHER, 1);
        }
    }
}

/* 021AEB58 */
static BOOL daKytag00_Execute(kytag00_class* i_this) {
    WWHD_FUNC(0x021AEB58, BOOL, i_this);
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
VERIFY(0x021AEB58, daKytag00_Execute);

/* 021AF0B0 */
static BOOL daKytag00_IsDelete(kytag00_class* i_this) {
    WWHD_FUNC(0x021AF0B0, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021AF0B0, daKytag00_IsDelete);

/* 021AF0B8 */
static BOOL daKytag00_Delete(kytag00_class* i_this) {
    WWHD_FUNC(0x021AF0B8, BOOL, (u32)0);
    env_set<s32>(ENV_MOYACOUNT, 0);
    return TRUE;
}
VERIFY(0x021AF0B8, daKytag00_Delete);

/* 021AF0E4 */
static cPhs_State daKytag00_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x021AF0E4, cPhs_State, i_ac);
    kytag00_class* i_this = (kytag00_class*)i_ac;

    /* fopAcM_ct(i_ac, kytag00_class) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr) {
            fopAc_ac_c_ct(i_ac);
            i_ac->__vtbl = KYTAG00_VTBL;
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
        i_this->mOuterRadius = gabi::fmadds(i_ac->scale.x, 5000.0f, (f32)i_this->mThickness * 100.0f);
    } else {
        i_this->mInnerRadius = i_ac->scale.x * 500.0f;
        i_this->mOuterRadius = gabi::fmadds(i_ac->scale.x, 500.0f, (f32)i_this->mThickness * 10.0f);
    }

    i_this->mbEfSet = false;
    i_this->mbPselSet = false;
    env_set<s32>(ENV_MOYACOUNT, 0);
    wether_tag_efect_move(i_this);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x021AF0E4, daKytag00_Create);

/* 021AF34C: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_kytag00_cpp() {
    WWHD_FUNC(0x021AF34C, void, (u32)0);
    sinit_header_statics(0x10464CC8, 0x101B8AC8);
}
VERIFY(0x021AF34C, __sinit_d_a_kytag00_cpp);

/* 021AF3E0: kytag00_class deleting destructor (compiler-generated, HD virtual destructor) */
static void kytag00_class_dt(kytag00_class* i_this, s32 flags) {
    WWHD_FUNC(0x021AF3E0, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021AF3E0, kytag00_class_dt);
