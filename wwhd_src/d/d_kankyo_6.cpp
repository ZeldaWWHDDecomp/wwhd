/** Actor and background environment lighting, with the verified HD material path. */
#include "d/d_kankyo_6.h"

namespace d_kankyo_cpp {
using namespace lighting6;
static f32 l_tevOne;
static f32 l_tevZero;
static f32 l_tevByteColorScale;
static f32 l_tevSeaFogDistance;
static f32 l_tevDirectLightThreshold;
static f32 l_tevFlickerDivisor;
static f32 l_tevAmbientLightScale;
static f32 l_tevPlayerLightDistance;
static f32 l_tevPlayerBaseLightDistance;
static f32 l_tevPlayerLightMinStep;
static f32 l_tevPlayerLightEaseScale;
static f32 l_tevPlayerLightStepScale;
static f32 l_tevHeaderInitialA;
static f32 l_tevHeaderInitialB;
static HeaderStatics l_tevHeaderStatics;
static EnvironmentFlags l_tevEnvironmentFlags;
static HeaderDestructorRecords l_tevHeaderDestructors;

static u32 environmentAddress() { return gabi::ea(dKy_getEnvlight()); }
static u32 playAddress() { return gabi::ea(dComIfGp_get()); }
static u32 allocateObject(u32 size) { return gabi::ea(operator_new(size)); }
static u32 effectLightAddress(u32 env, s32 index) {
    // HD: malformed indices retain unsigned guest-address wrapping.
    u32 base = gabi::ea(gabi::at<KankyoEnvLight>(env)->mpEfLights);
    return *gabi::at<be<u32>>(base + (u32)index * sizeof(be<u32>));
}
template <class Color> static auto &colorComponent(Color &color, u32 index) {
    using Component = decltype(color.r);
    return *gabi::at<Component>(gabi::ea(&color) + index * sizeof(Component));
}

static be<s16> &backgroundAddComponent(u32 env, u32 group, u32 channel) {
    u32 base = gabi::ea(gabi::at<EnvironmentColors>(env)->mBgAddColor);
    return *gabi::at<be<s16>>(base + group * sizeof(GXColorS10) + channel * sizeof(be<s16>));
}
static be<u8> &backgroundPaletteComponent(u32 palette, u32 group, u32 channel) {
    u32 base = gabi::ea(&gabi::at<lighting::PaletteHD>(palette)->mBG0_C0);
    return *gabi::at<be<u8>>(base + group * sizeof(lighting::ColorRGB) + channel);
}
static u32 ld(u32 p) { return gabi::load<u32>(p); }
static u16 ld16(u32 p) { return gabi::load<u16>(p); }
static s16 lds16(u32 p) { return gabi::load<s16>(p); }
static u8 ld8(u32 p) { return gabi::load<u8>(p); }
static f32 ldf(u32 p) { return gabi::load<f32>(p); }
static void st(u32 p, u32 v) { gabi::store<u32>(p, v); }
static void st16(u32 p, u16 v) { gabi::store<u16>(p, v); }
static void st8(u32 p, u8 v) { gabi::store<u8>(p, v); }
static void stf(u32 p, f32 v) { gabi::store<f32>(p, v); }
static f32 i2f(s32 value) { return (f32)value; }

void dScnKy_env_light_c_setLight_actor(u32 t, u32 tev, u32 fog, u32 fogStart, u32 fogEnd);
void dScnKy_env_light_c_settingTevStruct_colget_actor(u32 t, u32 pos, u32 tev, u32 c0, u32 k0, u32 fog,
                                                      u32 fogStart, u32 fogEnd);
void dScnKy_env_light_c_settingTevStruct_colget_player(u32 t, u32 tev);
void dScnKy_env_light_c_settingTevStruct_plightcol_plus(u32 t, u32 pos, u32 tev, u32 c0, u32 k0, u32 timer);
void dScnKy_env_light_c_settingTevStruct_eflightcol_plus(u32 t, u32 pos, u32 tev);
void dScnKy_env_light_c_setLight_bg(u32 t, u32 tev, u32 bg0c0, u32 bg0k0, u32 bg1c0, u32 bg1k0, u32 bg2c0,
                                    u32 bg2k0);
void dScnKy_env_light_c_settingTevStruct(u32 t, s32 type, u32 pos, u32 tev);
void dScnKy_env_light_c_setLightTevColorType(u32 t, u32 model, u32 tev);

s16 kankyo_color_ratio_set(u32 b0A, u32 b0B, f32 blendAB0, u32 b1A, u32 b1B, f32 blendAB1, s32 add,
                           f32 blend01);
f32 float_kankyo_color_ratio_set_noratio(f32 p0, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6, f32 p7);

// HD: actor palettes include fog alpha and the day/night K0 blend.
/* 02560E10 */
void lighting6::dScnKy_env_light_c::setLight_actor(dKy_tevstr_c *i_tevstr, GXColorS10 *i_fogColor,
                                                   be<f32> *i_fogStartZ, be<f32> *i_fogEndZ) {
    u32 t = gabi::ea(this);
    u32 tev = gabi::ea(i_tevstr);
    u32 fog = gabi::ea(i_fogColor);
    u32 fogStart = gabi::ea(i_fogStartZ);
    u32 fogEnd = gabi::ea(i_fogEndZ);

    gabi::at<dKy_tevstr_c>(tev)->mColpatPrev =
        (u8)((u8)gabi::at<KankyoEnvLight>(environmentAddress())->mColpatPrev);
    u8 cur = (u8)gabi::at<KankyoEnvLight>(environmentAddress())->mColpatCurr;
    u8 prev = (u8)gabi::at<dKy_tevstr_c>(tev)->mColpatPrev;
    gabi::at<dKy_tevstr_c>(tev)->mColpatCurr = (u8)(cur);
    if (prev != cur)
        gabi::at<dKy_tevstr_c>(tev)->mColpatBlend =
            (f32)((f32)gabi::at<KankyoEnvLight>(environmentAddress())->mColPatBlend);
    gabi::Local<be<f32>> blendAB;
    gabi::Local<be<u8>> pal0A, pal0B, pal1A, pal1B;
    gabi::Local<be<s32>> psel0, psel1;
    selectLightPalettes(t, tev + 0xB6, tev + 0xB5, tev + 0xB8, tev + 0xB7, gabi::ea(pal0A.get()),
                        gabi::ea(pal0B.get()), gabi::ea(pal1A.get()), gabi::ea(pal1B.get()),
                        gabi::ea(blendAB.get()), gabi::ea(psel0.get()), gabi::ea(psel1.get()), tev + 0xB0,
                        tev + 0xB4);
    u32 palette0a, palette0b, palette1a, palette1b;
    if ((u8)gabi::at<lighting::EnvLight>(environmentAddress())->mPaletteOverride == 0xFF) {
        palette0a = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo +
                    (u8)*pal0A * sizeof(lighting::PaletteHD);
        palette0b = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo +
                    (u8)*pal0B * sizeof(lighting::PaletteHD);
        palette1a = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo +
                    (u8)*pal1A * sizeof(lighting::PaletteHD);
        palette1b = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo +
                    (u8)*pal1B * sizeof(lighting::PaletteHD);
    } else {
        palette0a = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo;
        palette0b = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo;
        palette1a = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo;
        palette1b = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo;
    }
    const u32 bAB = gabi::ea(blendAB.get());
    for (u32 i = 0; i < 3; i++) { /* C0: mActColRatio^2, + mActorAddColAmb */
        u32 e1 = environmentAddress();
        u32 e2 = environmentAddress();
        f32 ratio =
            (f32)gabi::at<KankyoEnvLight>(e1)->mActColRatio * (f32)gabi::at<KankyoEnvLight>(e2)->mActColRatio;
        s16 r = kankyo_color_ratio_set(
            colorComponent(gabi::at<lighting::PaletteHD>(palette0a)->mActor_C0, i).get(),
            colorComponent(gabi::at<lighting::PaletteHD>(palette0b)->mActor_C0, i).get(), ldf(bAB),
            colorComponent(gabi::at<lighting::PaletteHD>(palette1a)->mActor_C0, i).get(),
            colorComponent(gabi::at<lighting::PaletteHD>(palette1b)->mActor_C0, i).get(),
            (f32)gabi::at<dKy_tevstr_c>(tev)->mColpatBlend,
            colorComponent(gabi::at<EnvironmentColors>(t)->mActorAddColAmb, i).get(), ratio);
        colorComponent(gabi::at<dKy_tevstr_c>(tev)->mColorC0, i) = r;
    }
    for (u32 i = 0; i < 3; i++) { /* K0: mActColRatio, + mActorAddColDif */
        u32 e = environmentAddress();
        s16 r = kankyo_color_ratio_set(
            colorComponent(gabi::at<lighting::PaletteHD>(palette0a)->mActor_K0, i).get(),
            colorComponent(gabi::at<lighting::PaletteHD>(palette0b)->mActor_K0, i).get(), ldf(bAB),
            colorComponent(gabi::at<lighting::PaletteHD>(palette1a)->mActor_K0, i).get(),
            colorComponent(gabi::at<lighting::PaletteHD>(palette1b)->mActor_K0, i).get(),
            (f32)gabi::at<dKy_tevstr_c>(tev)->mColpatBlend,
            colorComponent(gabi::at<EnvironmentColors>(t)->mActorAddColDif, i).get(),
            (f32)gabi::at<KankyoEnvLight>(e)->mActColRatio);
        colorComponent(gabi::at<dKy_tevstr_c>(tev)->mColorK0, i) = (u8)r;
    }
    const f32 zero = l_tevZero;
    if ((f32)gabi::at<lighting::EnvLight>(environmentAddress())->mDayNightFade > zero) {
        f32 prevk = (f32)(u8)gabi::at<dKy_tevstr_c>(tev)->mColorK0.r;
        for (u32 i = 0; i < 3; i++) {
            u32 e = environmentAddress();
            f32 c = i2f(colorComponent(gabi::at<dKy_tevstr_c>(tev)->mColorC0, i).get());
            f32 v = gabi::fmadds(c - prevk, (f32)gabi::at<lighting::EnvLight>(e)->mDayNightFade, prevk);
            if (i < 2)
                prevk = (f32)ld8(tev + 0x99 + i);
            colorComponent(gabi::at<dKy_tevstr_c>(tev)->mColorK0, i) = (u8)gabi::ftoi(v);
        }
    }
    for (u32 i = 0; i < 3; i++) { /* fog: mFogColRatio, + mAddColFog */
        u32 e = environmentAddress();
        s16 r = kankyo_color_ratio_set(
            colorComponent(gabi::at<lighting::PaletteHD>(palette0a)->mFog, i).get(),
            colorComponent(gabi::at<lighting::PaletteHD>(palette0b)->mFog, i).get(), ldf(bAB),
            colorComponent(gabi::at<lighting::PaletteHD>(palette1a)->mFog, i).get(),
            colorComponent(gabi::at<lighting::PaletteHD>(palette1b)->mFog, i).get(),
            (f32)gabi::at<dKy_tevstr_c>(tev)->mColpatBlend,
            colorComponent(gabi::at<EnvironmentColors>(t)->mAddColFog, i).get(),
            (f32)gabi::at<KankyoEnvLight>(e)->mFogColRatio);
        colorComponent(*gabi::at<GXColorS10>(fog), i) = r;
    }
    {
        const f32 k255 = l_tevByteColorScale;
        u32 e = environmentAddress();
        u8 a = (u8)gabi::ftoi((f32)gabi::at<lighting::PaletteHD>(palette0a)->mFogAlpha * k255),
           b = (u8)gabi::ftoi((f32)gabi::at<lighting::PaletteHD>(palette0b)->mFogAlpha * k255),
           c = (u8)gabi::ftoi((f32)gabi::at<lighting::PaletteHD>(palette1a)->mFogAlpha * k255),
           d = (u8)gabi::ftoi((f32)gabi::at<lighting::PaletteHD>(palette1b)->mFogAlpha * k255);
        s16 r = kankyo_color_ratio_set(a, b, ldf(bAB), c, d, (f32)gabi::at<dKy_tevstr_c>(tev)->mColpatBlend,
                                       0, (f32)gabi::at<KankyoEnvLight>(e)->mFogColRatio);
        gabi::at<GXColorS10>(fog)->a = r;
    }
    {
        f32 sz = (f32)gabi::at<KankyoEnvLight>(environmentAddress())->mFogStartZ;
        u32 e = environmentAddress();
        f32 v = float_kankyo_color_ratio_set_noratio(
            (f32)gabi::at<lighting::PaletteHD>(palette0a)->mFogStartZ,
            (f32)gabi::at<lighting::PaletteHD>(palette0b)->mFogStartZ, ldf(bAB),
            (f32)gabi::at<lighting::PaletteHD>(palette1a)->mFogStartZ,
            (f32)gabi::at<lighting::PaletteHD>(palette1b)->mFogStartZ,
            (f32)gabi::at<dKy_tevstr_c>(tev)->mColpatBlend, sz, (f32)gabi::at<KankyoEnvLight>(e)->mFogRatio);
        stf(fogStart, v);
        u32 e1 = environmentAddress();
        u32 e2 = environmentAddress();
        f32 ez = float_kankyo_color_ratio_set_noratio(
            (f32)gabi::at<lighting::PaletteHD>(palette0a)->mFogEndZ,
            (f32)gabi::at<lighting::PaletteHD>(palette0b)->mFogEndZ, ldf(bAB),
            (f32)gabi::at<lighting::PaletteHD>(palette1a)->mFogEndZ,
            (f32)gabi::at<lighting::PaletteHD>(palette1b)->mFogEndZ,
            (f32)gabi::at<dKy_tevstr_c>(tev)->mColpatBlend, (f32)gabi::at<KankyoEnvLight>(e1)->mFogEndZ,
            (f32)gabi::at<KankyoEnvLight>(e2)->mFogRatio);
        stf(fogEnd, ez);
        u32 e3 = environmentAddress();
        ez = gabi::fmadds(l_tevSeaFogDistance, (f32)gabi::at<lighting::EnvLight>(e3)->mSeaLightBlend, ez);
        stf(fogEnd, ez);
        if (ldf(fogStart) > ez)
            stf(fogStart, ez);
    }
}

/* dScnKy_env_light_c::settingTevStruct_colget_actor */
/* 02561384 */
void lighting6::dScnKy_env_light_c::settingTevStruct_colget_actor(cXyz *i_pos, dKy_tevstr_c *i_tevstr,
                                                                  GXColorS10 *i_colorC0,
                                                                  GXColorS10 *i_colorK0,
                                                                  GXColorS10 *i_fogColor,
                                                                  be<f32> *i_fogStartZ, be<f32> *i_fogEndZ) {
    u32 t = gabi::ea(this);
    [[maybe_unused]] u32 pos = gabi::ea(i_pos);
    u32 tev = gabi::ea(i_tevstr);
    u32 c0 = gabi::ea(i_colorC0);
    u32 k0 = gabi::ea(i_colorK0);
    u32 fog = gabi::ea(i_fogColor);
    u32 fogStart = gabi::ea(i_fogStartZ);
    u32 fogEnd = gabi::ea(i_fogEndZ);

    u8 ov = (u8)gabi::at<dKy_tevstr_c>(tev)->mEnvrIdxOverride;
    u8 prev = (u8)gabi::at<dKy_tevstr_c>(tev)->mEnvrIdxPrev;
    u8 cur;
    if (ov != 0xFF) {
        cur = ov;
    } else {
        s32 r = (s8)(u8)gabi::at<dKy_tevstr_c>(tev)->mRoomNo;
        if (r < 0)
            r = 0;
        cur = (u8)r;
    }
    gabi::at<dKy_tevstr_c>(tev)->mEnvrIdxCurr = (u8)(cur);
    if (prev != cur) {
        f32 b = (f32)gabi::at<dKy_tevstr_c>(tev)->mColpatBlend;
        const f32 zero = l_tevZero;
        if (!(b < l_tevOne) || !(b > zero))
            gabi::at<dKy_tevstr_c>(tev)->mColpatBlend = (f32)(zero);
    }
    dScnKy_env_light_c_setLight_actor(t, tev, fog, fogStart, fogEnd);
    st16(c0 + 0, (u16)gabi::at<dKy_tevstr_c>(tev)->mColorC0.r);
    st16(c0 + 2, (u16)gabi::at<dKy_tevstr_c>(tev)->mColorC0.g);
    st16(c0 + 4, (u16)gabi::at<dKy_tevstr_c>(tev)->mColorC0.b);
    st16(c0 + 6, (u16)gabi::at<dKy_tevstr_c>(tev)->mColorC0.a);
    st16(k0 + 0, (u8)gabi::at<dKy_tevstr_c>(tev)->mColorK0.r);
    st16(k0 + 2, (u8)gabi::at<dKy_tevstr_c>(tev)->mColorK0.g);
    st16(k0 + 4, (u8)gabi::at<dKy_tevstr_c>(tev)->mColorK0.b);
}

// HD: the room-light override also selects the player environment.
/* 0256146C */
void lighting6::dScnKy_env_light_c::settingTevStruct_colget_player(dKy_tevstr_c *i_tevstr) {
    u32 tev = gabi::ea(i_tevstr);

    u8 ov = (u8)gabi::at<dKy_tevstr_c>(tev)->mEnvrIdxOverride;
    if (ov != 0xFF) {
        gabi::at<dKy_tevstr_c>(tev)->mEnvrIdxCurr = (u8)(ov);
    } else {
        s32 r = (s8)(u8)gabi::at<dKy_tevstr_c>(tev)->mRoomNo;
        if (r >= 0) {
            gabi::at<dKy_tevstr_c>(tev)->mEnvrIdxCurr = (u8)((u8)r);
            if ((u8)gabi::at<lighting::EnvLight>(environmentAddress())->mRoomLightBlendRoom != 0xFF)
                gabi::at<dKy_tevstr_c>(tev)->mEnvrIdxCurr =
                    (u8)((u8)gabi::at<lighting::EnvLight>(environmentAddress())->mRoomLightBlendRoom);
        }
    }
    u32 env = environmentAddress();
    u8 cur = (u8)gabi::at<dKy_tevstr_c>(tev)->mEnvrIdxCurr;
    if ((u8)gabi::at<KankyoEnvLight>(env)->mEnvrIdxCurr == cur)
        return;
    const f32 one = l_tevOne;
    if (cur == (u8)gabi::at<KankyoEnvLight>(environmentAddress())->mEnvrIdxPrev) {
        u32 e1 = environmentAddress();
        gabi::at<KankyoEnvLight>(e1)->mEnvrIdxPrev =
            (u8)((u8)gabi::at<KankyoEnvLight>(environmentAddress())->mEnvrIdxCurr);
        // HD: the lazy accessor must run before loading the current environment index.
        u32 currentEnv = environmentAddress();
        gabi::at<KankyoEnvLight>(currentEnv)->mEnvrIdxCurr = (u8)gabi::at<dKy_tevstr_c>(tev)->mEnvrIdxCurr;
        e1 = environmentAddress();
        f32 b = (f32)gabi::at<KankyoEnvLight>(environmentAddress())->mColPatBlend;
        gabi::at<KankyoEnvLight>(e1)->mColPatBlend = (f32)(one - b);
        gabi::at<dKy_tevstr_c>(tev)->mColpatBlend =
            (f32)(one - (f32)gabi::at<KankyoEnvLight>(environmentAddress())->mColPatBlend);
    } else {
        const f32 zero = l_tevZero;
        bool reset = !((f32)gabi::at<KankyoEnvLight>(environmentAddress())->mColPatBlend < one);
        if (!reset)
            reset = !((f32)gabi::at<KankyoEnvLight>(environmentAddress())->mColPatBlend > zero);
        if (reset) {
            u32 currentEnv = environmentAddress();
            gabi::at<KankyoEnvLight>(currentEnv)->mEnvrIdxCurr =
                (u8)gabi::at<dKy_tevstr_c>(tev)->mEnvrIdxCurr;
            u32 e = environmentAddress();
            gabi::at<KankyoEnvLight>(e)->mColPatBlend = (f32)(zero);
            gabi::at<dKy_tevstr_c>(tev)->mColpatBlend = (f32)(zero);
        }
    }
}

BOOL toon_proc_check();
f32 dKy_eflight_influence_power(s32 idx);
f32 dKy_eflight_influence_distance(u32 pos, s32 idx);
static inline s16 scaleLightColor(s32 v, f32 k) { return (s16)gabi::ftoi(i2f(v) * k); }

// The primary light is mirrored through the actor position for toon shading.
static inline void setTevLightObject(u32 tev, bool toon, u32 pos) {
    gabi::Local<cXyz> wp;
    u32 wa = gabi::ea(wp.get());
    if (!toon) {
        st(wa + 0, ld(gabi::ea(&gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.x)));
        st(wa + 4, ld(gabi::ea(&gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.y)));
        st(wa + 8, ld(gabi::ea(&gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.z)));
    } else {
        /* toon: the light mirrored through the position */
        gabi::Local<cXyz> d;
        subtractLightPosition(tev + 0x84, gabi::ea(d.get()), pos); /* cXyz::operator- */
        gabi::Local<cXyz> d2;
        u32 da = gabi::ea(d.get()), d2a = gabi::ea(d2.get());
        st(d2a + 0, ld(da + 0));
        st(d2a + 4, ld(da + 4));
        st(d2a + 8, ld(da + 8));
        subtractLightPosition(pos, gabi::ea(d.get()), gabi::ea(d2.get()));
        st(wa + 0, ld(da + 0));
        st(wa + 4, ld(da + 4));
        st(wa + 8, ld(da + 8));
    }
    gabi::Local<cXyz> vp;
    u32 va = gabi::ea(vp.get());
    transformLightPosition(0x104B45F8, wa, va); /* PSMTXMultVec(view) */
    st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mPosition.x), ld(va + 0));
    st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mPosition.y), ld(va + 4));
    st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mPosition.z), ld(va + 8));
    u32 env = environmentAddress();
    st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mDirection.x),
       ld(gabi::ea(&gabi::at<KankyoEnvLight>(env)->mLightDir.x)));
    st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mDirection.y),
       ld(gabi::ea(&gabi::at<KankyoEnvLight>(env)->mLightDir.y)));
    u32 dz = ld(gabi::ea(&gabi::at<KankyoEnvLight>(env)->mLightDir.z));
    const f32 one = l_tevOne, zero = l_tevZero;
    gabi::at<TevLightParameters>(tev)->mLightObj.mColor.g = (u8)(0);
    gabi::at<TevLightParameters>(tev)->mLightObj.mColor.b = (u8)(0);
    gabi::at<TevLightParameters>(tev)->mLightObj.mColor.a = (u8)(0xFF);
    gabi::at<TevLightParameters>(tev)->mLightObj.mCosAttenuation[0] = (f32)(one);
    gabi::at<TevLightParameters>(tev)->mLightObj.mCosAttenuation[1] = (f32)(zero);
    gabi::at<TevLightParameters>(tev)->mLightObj.mCosAttenuation[2] = (f32)(zero);
    gabi::at<TevLightParameters>(tev)->mLightObj.mDistanceAttenuation[0] = (f32)(one);
    gabi::at<TevLightParameters>(tev)->mLightObj.mDistanceAttenuation[1] = (f32)(zero);
    gabi::at<TevLightParameters>(tev)->mLightObj.mDistanceAttenuation[2] = (f32)(zero);
    st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mDirection.z), dz);
}

// HD: use the base light; player state three eases its position.
/* 025615B8 */
void lighting6::dScnKy_env_light_c::settingTevStruct_plightcol_plus(cXyz *i_pos, dKy_tevstr_c *i_tevstr,
                                                                    GXColorS10 *i_colorC0,
                                                                    GXColorS10 *i_colorK0, u32 timer) {
    u32 t = gabi::ea(this);
    u32 pos = gabi::ea(i_pos);
    u32 tev = gabi::ea(i_tevstr);
    u32 c0 = gabi::ea(i_colorC0);
    u32 k0 = gabi::ea(i_colorK0);

    const u32 env = environmentAddress();
    gabi::Local<cXyz> bl;
    u32 ba = gabi::ea(bl.get());
    st(ba + 0, ld(gabi::ea(&gabi::at<KankyoEnvLight>(env)->mBaseLightInfluence.mPos.x)));
    st(ba + 4, ld(gabi::ea(&gabi::at<KankyoEnvLight>(env)->mBaseLightInfluence.mPos.y)));
    st(ba + 8, ld(gabi::ea(&gabi::at<KankyoEnvLight>(env)->mBaseLightInfluence.mPos.z)));
    f32 d = std_sqrtf(squaredDistance(env + 0x14, pos));
    f32 fl = (f32)gabi::at<KankyoEnvLight>(env)->mBaseLightInfluence.mFluctuation;
    const f32 zero = l_tevZero;
    f32 pw = (f32)gabi::at<KankyoEnvLight>(env)->mBaseLightInfluence.mPower;
    gabi::at<EnvironmentColors>(t)->mColorK0.r = (s16)(0);
    gabi::at<EnvironmentColors>(t)->mColorK0.g = (s16)(0);
    gabi::at<EnvironmentColors>(t)->mColorK0.b = (s16)(0);
    const f32 one = l_tevOne;
    f32 v = one;
    if (pw > zero && timer == 0) {
        v = d / pw;
        if (v > one)
            v = one;
    }
    f32 k = gabi::fnmsubs(v, v, one);
    s32 i;
    if (!(fl < l_tevDirectLightThreshold)) {
        i = gabi::ftoi(fl - l_tevDirectLightThreshold);
    } else {
        f32 k255 = l_tevByteColorScale;
        f32 w = gabi::fnmsubs(fl / l_tevFlickerDivisor, k, k255);
        f32 range = k255 - w;
        f32 r = cM_rndF(one); /* cM_rndF */
        i = gabi::ftoi(gabi::fmadds(range, r, w));
    }
    gabi::at<TevLightParameters>(tev)->mLightObj.mColor.r = (u8)((u8)i);
    {
        f32 kc = k * l_tevAmbientLightScale;
        s16 a = scaleLightColor((s16)gabi::at<EnvironmentColors>(t)->mColorK0.r, kc);
        s16 b = scaleLightColor((s16)gabi::at<EnvironmentColors>(t)->mColorK0.g, kc);
        s16 c = scaleLightColor((s16)gabi::at<EnvironmentColors>(t)->mColorK0.b, kc);
        s16 r0 = (s16)((s16)gabi::at<GXColorS10>(c0)->r + a);
        gabi::at<EnvironmentColors>(t)->mColorC0.r = (s16)((u16)r0);
        s16 r1 = (s16)((s16)gabi::at<GXColorS10>(c0)->g + b);
        gabi::at<EnvironmentColors>(t)->mColorC0.g = (s16)((u16)r1);
        s16 r1r = (s16)gabi::at<EnvironmentColors>(t)->mColorC0.g;
        s16 r2 = (s16)((s16)gabi::at<GXColorS10>(c0)->b + c);
        gabi::at<EnvironmentColors>(t)->mColorC0.b = (s16)((u16)r2);
        if (r0 > 0xFF)
            gabi::at<EnvironmentColors>(t)->mColorC0.r = (s16)(0xFF);
        s16 r2r = (s16)gabi::at<EnvironmentColors>(t)->mColorC0.b;
        if (r1r > 0xFF)
            gabi::at<EnvironmentColors>(t)->mColorC0.g = (s16)(0xFF);
        if (r2r > 0xFF)
            gabi::at<EnvironmentColors>(t)->mColorC0.b = (s16)(0xFF);
    }
    {
        s16 a = scaleLightColor((s16)gabi::at<EnvironmentColors>(t)->mColorK0.r, k);
        s16 b = scaleLightColor((s16)gabi::at<EnvironmentColors>(t)->mColorK0.g, k);
        s16 c = scaleLightColor((s16)gabi::at<EnvironmentColors>(t)->mColorK0.b, k);
        s16 r0 = (s16)((s16)gabi::at<GXColorS10>(k0)->r + a);
        gabi::at<EnvironmentColors>(t)->mColorK0.r = (s16)((u16)r0);
        s16 r1 = (s16)((s16)gabi::at<GXColorS10>(k0)->g + b);
        gabi::at<EnvironmentColors>(t)->mColorK0.g = (s16)((u16)r1);
        s16 r2 = (s16)((s16)gabi::at<GXColorS10>(k0)->b + c);
        gabi::at<EnvironmentColors>(t)->mColorK0.b = (s16)((u16)r2);
        if (r0 > 0xFF)
            gabi::at<EnvironmentColors>(t)->mColorK0.r = (s16)(0xFF);
        s16 r2r = (s16)gabi::at<EnvironmentColors>(t)->mColorK0.b;
        if (r1 > 0xFF)
            gabi::at<EnvironmentColors>(t)->mColorK0.g = (s16)(0xFF);
        if (r2r > 0xFF)
            gabi::at<EnvironmentColors>(t)->mColorK0.b = (s16)(0xFF);
    }
    u32 player = (u32)gabi::at<lighting::Play>(playAddress())->mpPlayer;
    if (player != 0 && (u16)gabi::at<lighting::Player>(player)->mRoomLightState == 3 && timer == 0) {
        f32 d2 = std_sqrtf(squaredDistance(pos, tev + 0x84));
        f32 k1e4 = l_tevPlayerLightDistance;
        f32 a = d2 / k1e4;
        if (a > one)
            a = one;
        a = a * a;
        f32 d3 = std_sqrtf(squaredDistance(pos, ba));
        f32 b = d3 / l_tevPlayerBaseLightDistance;
        if (b > one)
            b = one;
        b = one - b;
        f32 b2 = b * b;
        f32 base = gabi::fmadds(k1e4, a, l_tevPlayerLightMinStep);
        f32 b3 = b * b2;
        f32 step = gabi::fmadds(l_tevPlayerLightEaseScale, b3, base) * l_tevPlayerLightStepScale;
        approachLightCoordinate(tev + 0x84, ldf(ba + 0), one, step, one);
        approachLightCoordinate(tev + 0x88, ldf(ba + 4), one, step, one);
        approachLightCoordinate(tev + 0x8C, ldf(ba + 8), one, step, one);
    } else {
        st(gabi::ea(&gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.x), ld(ba + 0));
        st(gabi::ea(&gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.y), ld(ba + 4));
        st(gabi::ea(&gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.z), ld(ba + 8));
    }
    setTevLightObject(tev, toon_proc_check(), pos);
}

// Add the selected effect light to K1.
/* 02561BA4 */
void lighting6::dScnKy_env_light_c::settingTevStruct_eflightcol_plus(cXyz *i_pos, dKy_tevstr_c *i_tevstr) {
    u32 t = gabi::ea(this);
    u32 pos = gabi::ea(i_pos);
    u32 tev = gabi::ea(i_tevstr);

    if ((s32)(u32)gabi::at<KankyoEnvLight>(t)->mPlayerEflightIdx < 0)
        return;
    u32 env = environmentAddress();
    s32 efi = (s32)(u32)gabi::at<KankyoEnvLight>(t)->mPlayerEflightIdx;
    if (effectLightAddress(env, efi) == 0)
        return;
    f32 pw = dKy_eflight_influence_power(efi);
    if (!(pw > l_tevZero))
        return;
    gabi::Local<cXyz> lp;
    u32 la = gabi::ea(lp.get());
    stf(la + 8, ldf(pos + 8));
    stf(la + 4, ldf(pos + 4));
    stf(la + 0, ldf(pos + 0));
    f32 b = dKy_eflight_influence_distance(la, efi) / pw;
    const f32 one = l_tevOne;
    if (!(b < one))
        return;
    gabi::at<dKy_tevstr_c>(tev)->mColorK1.a = (u8)(1);
    if (efi < 0)
        efi = 0;
    u32 l = effectLightAddress(environmentAddress(), efi);
    f32 k = one - b;
    s32 r = scaleLightColor(lds16(l + 0xC), k);
    s32 g = scaleLightColor(lds16(l + 0xE), k);
    s32 bb = scaleLightColor(lds16(l + 0x10), k);
    if (r > 0xFF)
        r = 0xFF;
    if (g > 0xFF)
        g = 0xFF;
    if (bb > 0xFF)
        bb = 0xFF;
    u8 rr = (u8)gabi::ftoi(i2f(r) * k), gg = (u8)gabi::ftoi(i2f(g) * k), b2 = (u8)gabi::ftoi(i2f(bb) * k);
    gabi::at<dKy_tevstr_c>(tev)->mColorK1.r = (u8)(rr);
    gabi::at<dKy_tevstr_c>(tev)->mColorK1.g = (u8)(gg);
    gabi::at<dKy_tevstr_c>(tev)->mColorK1.b = (u8)(b2);
}

// HD: four background palette pairs share the fog alpha and distance extensions.
/* 02561E14 */
void lighting6::dScnKy_env_light_c::setLight_bg(dKy_tevstr_c *i_tev, GXColorS10 *i_bg0c0, GXColorS10 *i_bg0k0,
                                                GXColorS10 *i_bg1c0, GXColorS10 *i_bg1k0, GXColorS10 *i_bg2c0,
                                                GXColorS10 *i_bg2k0) {
    u32 t = gabi::ea(this);
    u32 tev = gabi::ea(i_tev);
    u32 bg0c0 = gabi::ea(i_bg0c0);
    u32 bg0k0 = gabi::ea(i_bg0k0);
    u32 bg1c0 = gabi::ea(i_bg1c0);
    u32 bg1k0 = gabi::ea(i_bg1k0);
    u32 bg2c0 = gabi::ea(i_bg2c0);
    u32 bg2k0 = gabi::ea(i_bg2k0);

    const u32 sp = gabi::cpu->r[1];
    const u32 bg3c0 = (u32)gabi::at<BackgroundStackArguments>(sp)->bg3c0,
              bg3k0 = (u32)gabi::at<BackgroundStackArguments>(sp)->bg3k0,
              fog = (u32)gabi::at<BackgroundStackArguments>(sp)->fog,
              fogStart = (u32)gabi::at<BackgroundStackArguments>(sp)->fogStart,
              fogEnd = (u32)gabi::at<BackgroundStackArguments>(sp)->fogEnd;
    gabi::at<dKy_tevstr_c>(tev)->mColpatPrev =
        (u8)((u8)gabi::at<KankyoEnvLight>(environmentAddress())->mColpatPrev);
    u8 cur = (u8)gabi::at<KankyoEnvLight>(environmentAddress())->mColpatCurr;
    u8 prev = (u8)gabi::at<dKy_tevstr_c>(tev)->mColpatPrev;
    gabi::at<dKy_tevstr_c>(tev)->mColpatCurr = (u8)(cur);
    if (prev != cur)
        gabi::at<dKy_tevstr_c>(tev)->mColpatBlend =
            (f32)((f32)gabi::at<KankyoEnvLight>(environmentAddress())->mColPatBlend);
    gabi::Local<be<f32>> blendAB;
    gabi::Local<be<u8>> pal0A, pal0B, pal1A, pal1B;
    gabi::Local<be<s32>> psel0, psel1;
    selectLightPalettes(t, tev + 0xB6, tev + 0xB5, tev + 0xB8, tev + 0xB7, gabi::ea(pal0A.get()),
                        gabi::ea(pal0B.get()), gabi::ea(pal1A.get()), gabi::ea(pal1B.get()),
                        gabi::ea(blendAB.get()), gabi::ea(psel0.get()), gabi::ea(psel1.get()), tev + 0xB0,
                        tev + 0xB4);
    u32 palette0a, palette0b, palette1a, palette1b;
    if ((u8)gabi::at<lighting::EnvLight>(environmentAddress())->mPaletteOverride == 0xFF) {
        palette0a = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo +
                    (u8)*pal0A * sizeof(lighting::PaletteHD);
        palette0b = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo +
                    (u8)*pal0B * sizeof(lighting::PaletteHD);
        palette1a = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo +
                    (u8)*pal1A * sizeof(lighting::PaletteHD);
        palette1b = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo +
                    (u8)*pal1B * sizeof(lighting::PaletteHD);
    } else {
        palette0a = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo;
        palette0b = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo;
        palette1a = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo;
        palette1b = (u32)gabi::at<lighting::EnvLight>(environmentAddress())->mpPaletInfo;
    }
    const u32 bAB = gabi::ea(blendAB.get());
    const u32 outs[8] = {bg0c0, bg0k0, bg1c0, bg1k0, bg2c0, bg2k0, bg3c0, bg3k0};
    for (u32 g = 0; g < 8; g++) {
        for (u32 i = 0; i < 3; i++) {
            u32 e = environmentAddress();
            s16 r = kankyo_color_ratio_set(backgroundPaletteComponent(palette0a, g, i).get(),
                                           backgroundPaletteComponent(palette0b, g, i).get(), ldf(bAB),
                                           backgroundPaletteComponent(palette1a, g, i).get(),
                                           backgroundPaletteComponent(palette1b, g, i).get(),
                                           (f32)gabi::at<dKy_tevstr_c>(tev)->mColpatBlend,
                                           backgroundAddComponent(t, g, i).get(),
                                           (f32)gabi::at<KankyoEnvLight>(e)->mBgColRatio);
            st16(outs[g] + i * 2, (u16)r);
        }
    }
    for (s32 g = 7; g >= 0; g--)
        st16(outs[g] + 6, 0xFF);
    const f32 zero = l_tevZero;
    if ((f32)gabi::at<lighting::EnvLight>(environmentAddress())->mDayNightFade > zero) {
        f32 prevk = i2f((s16)gabi::at<GXColorS10>(bg0k0)->r);
        for (u32 i = 0; i < 3; i++) {
            u32 e = environmentAddress();
            f32 c = i2f(colorComponent(*gabi::at<GXColorS10>(bg0c0), i).get());
            f32 v = gabi::fmadds(c - prevk, (f32)gabi::at<lighting::EnvLight>(e)->mDayNightFade, prevk);
            if (i < 2)
                prevk = i2f(lds16(bg0k0 + (i + 1) * 2));
            st16(bg0k0 + i * 2, (u8)gabi::ftoi(v));
        }
    }
    for (u32 i = 0; i < 3; i++) {
        u32 e = environmentAddress();
        s16 r = kankyo_color_ratio_set(
            colorComponent(gabi::at<lighting::PaletteHD>(palette0a)->mFog, i).get(),
            colorComponent(gabi::at<lighting::PaletteHD>(palette0b)->mFog, i).get(), ldf(bAB),
            colorComponent(gabi::at<lighting::PaletteHD>(palette1a)->mFog, i).get(),
            colorComponent(gabi::at<lighting::PaletteHD>(palette1b)->mFog, i).get(),
            (f32)gabi::at<dKy_tevstr_c>(tev)->mColpatBlend,
            colorComponent(gabi::at<EnvironmentColors>(t)->mAddColFog, i).get(),
            (f32)gabi::at<KankyoEnvLight>(e)->mFogColRatio);
        colorComponent(*gabi::at<GXColorS10>(fog), i) = r;
    }
    {
        const f32 k255 = l_tevByteColorScale;
        u32 e = environmentAddress();
        u8 a = (u8)gabi::ftoi((f32)gabi::at<lighting::PaletteHD>(palette0a)->mFogAlpha * k255),
           b = (u8)gabi::ftoi((f32)gabi::at<lighting::PaletteHD>(palette0b)->mFogAlpha * k255),
           c = (u8)gabi::ftoi((f32)gabi::at<lighting::PaletteHD>(palette1a)->mFogAlpha * k255),
           d = (u8)gabi::ftoi((f32)gabi::at<lighting::PaletteHD>(palette1b)->mFogAlpha * k255);
        s16 r = kankyo_color_ratio_set(a, b, ldf(bAB), c, d, (f32)gabi::at<dKy_tevstr_c>(tev)->mColpatBlend,
                                       0, (f32)gabi::at<KankyoEnvLight>(e)->mFogColRatio);
        gabi::at<GXColorS10>(fog)->a = r;
    }
    {
        f32 sz = (f32)gabi::at<KankyoEnvLight>(environmentAddress())->mFogStartZ;
        u32 e = environmentAddress();
        f32 v = float_kankyo_color_ratio_set_noratio(
            (f32)gabi::at<lighting::PaletteHD>(palette0a)->mFogStartZ,
            (f32)gabi::at<lighting::PaletteHD>(palette0b)->mFogStartZ, ldf(bAB),
            (f32)gabi::at<lighting::PaletteHD>(palette1a)->mFogStartZ,
            (f32)gabi::at<lighting::PaletteHD>(palette1b)->mFogStartZ,
            (f32)gabi::at<dKy_tevstr_c>(tev)->mColpatBlend, sz, (f32)gabi::at<KankyoEnvLight>(e)->mFogRatio);
        stf(fogStart, v);
        u32 e1 = environmentAddress();
        u32 e2 = environmentAddress();
        f32 ez = float_kankyo_color_ratio_set_noratio(
            (f32)gabi::at<lighting::PaletteHD>(palette0a)->mFogEndZ,
            (f32)gabi::at<lighting::PaletteHD>(palette0b)->mFogEndZ, ldf(bAB),
            (f32)gabi::at<lighting::PaletteHD>(palette1a)->mFogEndZ,
            (f32)gabi::at<lighting::PaletteHD>(palette1b)->mFogEndZ,
            (f32)gabi::at<dKy_tevstr_c>(tev)->mColpatBlend, (f32)gabi::at<KankyoEnvLight>(e1)->mFogEndZ,
            (f32)gabi::at<KankyoEnvLight>(e2)->mFogRatio);
        stf(fogEnd, ez);
        u32 e3 = environmentAddress();
        ez = gabi::fmadds(l_tevSeaFogDistance, (f32)gabi::at<lighting::EnvLight>(e3)->mSeaLightBlend, ez);
        stf(fogEnd, ez);
        if (ldf(fogStart) > ez)
            stf(fogStart, ez);
    }
}

void dKy_tevstr_init(u32 tev, s32 roomNo, u32 p2);

// Copy the environment work registers into the actor lighting state.
static inline void copyTevWorkColors(u32 t, u32 tev) {
    gabi::at<EnvironmentColors>(t)->mColorC0.a = (s16)(0xFF);
    gabi::at<dKy_tevstr_c>(tev)->mColorC0.r = (s16)((u16)gabi::at<EnvironmentColors>(t)->mColorC0.r);
    gabi::at<dKy_tevstr_c>(tev)->mColorC0.g = (s16)((u16)gabi::at<EnvironmentColors>(t)->mColorC0.g);
    gabi::at<dKy_tevstr_c>(tev)->mColorC0.b = (s16)((u16)gabi::at<EnvironmentColors>(t)->mColorC0.b);
    gabi::at<dKy_tevstr_c>(tev)->mColorC0.a = (s16)(0xFF);
    gabi::at<dKy_tevstr_c>(tev)->mColorK0.r = (u8)((u8)(u16)gabi::at<EnvironmentColors>(t)->mColorK0.r);
    gabi::at<dKy_tevstr_c>(tev)->mColorK0.g = (u8)((u8)(u16)gabi::at<EnvironmentColors>(t)->mColorK0.g);
    gabi::at<dKy_tevstr_c>(tev)->mColorK0.b = (u8)((u8)(u16)gabi::at<EnvironmentColors>(t)->mColorK0.b);
}

// HD: each background type selects its own light intensities; sea uses skybox fog.
/* 025626A4 */
void lighting6::dScnKy_env_light_c::settingTevStruct(s32 i_lightType, cXyz *i_pos, dKy_tevstr_c *i_tevstr) {
    u32 t = gabi::ea(this);
    s32 type = i_lightType;
    u32 pos = gabi::ea(i_pos);
    u32 tev = gabi::ea(i_tevstr);

    playAddress();
    s32 bgIdx = -1;
    u32 timer = (u8)gabi::at<dKy_tevstr_c>(tev)->mInitTimer;
    bool full = false;
    if ((u8)gabi::at<dKy_tevstr_c>(tev)->mInitType != TevInitialized &&
        (u8)gabi::at<dKy_tevstr_c>(tev)->mInitType != TevLightingSet)
        dKy_tevstr_init(tev, (s8)ld8(0x1047E6C8), 0xFF);
    gabi::at<dKy_tevstr_c>(tev)->mInitType = (u8)(TevLightingSet);
    gabi::at<dKy_tevstr_c>(tev)->mColorK1.a = (u8)(0);
    gabi::at<EnvironmentColors>(t)->mActorK0.a = (u8)(0xFF);
    gabi::at<EnvironmentColors>(t)->mActorC0.a = (u8)(0xFF);
    gabi::Local<GXColorS10> fog;
    gabi::Local<be<f32>> fogStart, fogEnd;
    const u32 fa = gabi::ea(fog.get());
    if (type == TEV_TYPE_ACTOR || type == TEV_TYPE_PLAYER || type == TEV_TYPE_UNK99) {
        gabi::at<dKy_tevstr_c>(tev)->mLightMode = (u8)(1);
        gabi::Local<GXColorS10> c0, k0;
        const u32 ca = gabi::ea(c0.get()), ka = gabi::ea(k0.get());
        st16(ca + 0, (u8)gabi::at<EnvironmentColors>(t)->mActorC0.r);
        st16(ca + 2, (u8)gabi::at<EnvironmentColors>(t)->mActorC0.g);
        st16(ca + 4, (u8)gabi::at<EnvironmentColors>(t)->mActorC0.b);
        st16(ka + 0, (u8)gabi::at<EnvironmentColors>(t)->mActorK0.r);
        st16(ka + 2, (u8)gabi::at<EnvironmentColors>(t)->mActorK0.g);
        st16(ka + 4, (u8)gabi::at<EnvironmentColors>(t)->mActorK0.b);
        st16(fa + 0, (u8)gabi::at<EnvironmentColors>(t)->mFogColor.r);
        st16(fa + 2, (u8)gabi::at<EnvironmentColors>(t)->mFogColor.g);
        st16(fa + 4, (u8)gabi::at<EnvironmentColors>(t)->mFogColor.b);
        st16(fa + 6, (u8)gabi::at<EnvironmentColors>(t)->mFogColor.a);
        stf(gabi::ea(fogStart.get()), (f32)gabi::at<KankyoEnvLight>(t)->mFogStartZ__setLight);
        stf(gabi::ea(fogEnd.get()), (f32)gabi::at<KankyoEnvLight>(t)->mFogEndZ__setLight);
        if (type == TEV_TYPE_PLAYER) {
            timer = (u8)gabi::at<KankyoEnvLight>(environmentAddress())->mInitAnimTimer;
            dScnKy_env_light_c_settingTevStruct_colget_player(t, tev);
        }
        dScnKy_env_light_c_settingTevStruct_colget_actor(t, pos, tev, ca, ka, fa, gabi::ea(fogStart.get()),
                                                         gabi::ea(fogEnd.get()));
        gabi::at<EnvironmentColors>(t)->mColorC0.r = (s16)(ld16(ca + 0));
        gabi::at<EnvironmentColors>(t)->mColorC0.g = (s16)(ld16(ca + 2));
        gabi::at<EnvironmentColors>(t)->mColorC0.b = (s16)(ld16(ca + 4));
        gabi::at<EnvironmentColors>(t)->mColorC0.a = (s16)(0xFF);
        gabi::at<EnvironmentColors>(t)->mColorK0.r = (s16)(ld16(ka + 0));
        gabi::at<EnvironmentColors>(t)->mColorK0.g = (s16)(ld16(ka + 2));
        gabi::at<EnvironmentColors>(t)->mColorK0.b = (s16)(ld16(ka + 4));
        gabi::at<EnvironmentColors>(t)->mColorK0.a = (s16)(0xFF);
        if (type != TEV_TYPE_UNK99) {
            gabi::Local<GXColorS10> c0c, k0c;
            u32 cca = gabi::ea(c0c.get()), kca = gabi::ea(k0c.get());
            st(kca + 0, ld(ka + 0));
            st(cca + 4, ld(ca + 4));
            st(kca + 4, ld(ka + 4));
            st(cca + 0, ld(ca + 0));
            dScnKy_env_light_c_settingTevStruct_plightcol_plus(t, pos, tev, cca, kca, timer);
            dScnKy_env_light_c_settingTevStruct_eflightcol_plus(t, pos, tev);
            if (type == TEV_TYPE_PLAYER) {
                u32 env = environmentAddress();
                st(gabi::ea(&gabi::at<KankyoEnvLight>(env)->mPLightNearPlayer.x),
                   ld(gabi::ea(&gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.x)));
                st(gabi::ea(&gabi::at<KankyoEnvLight>(env)->mPLightNearPlayer.y),
                   ld(gabi::ea(&gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.y)));
                st(gabi::ea(&gabi::at<KankyoEnvLight>(env)->mPLightNearPlayer.z),
                   ld(gabi::ea(&gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.z)));
            }
        }
    } else {
        gabi::at<dKy_tevstr_c>(tev)->mLightMode = (u8)(0);
        gabi::at<dKy_tevstr_c>(tev)->mEnvrIdxCurr = (u8)((u8)gabi::at<dKy_tevstr_c>(tev)->mRoomNo);
        gabi::Local<GXColorS10> bg[8]; /* C0/K0 of BG0..BG3 */
        u32 c0[4], k0[4];
        for (u32 i = 0; i < 4; i++) {
            c0[i] = gabi::ea(bg[i * 2].get());
            k0[i] = gabi::ea(bg[i * 2 + 1].get());
        }
        setBackgroundLight(t, tev, c0[0], k0[0], c0[1], k0[1], c0[2], k0[2], c0[3], k0[3], fa,
                           gabi::ea(fogStart.get()), gabi::ea(fogEnd.get())); /* setLight_bg */
        if ((u32)(type - TEV_TYPE_BG0_FULL) < 4) {
            type -= 4;
            full = true;
        }
        u32 pl = (u32)(type - TEV_TYPE_BG0_PLIGHT);
        bgIdx = type - (pl < 4 ? (s32)TEV_TYPE_BG0_PLIGHT : (s32)TEV_TYPE_BG0);
        u32 g = (u32)bgIdx < 3 ? (u32)bgIdx : 3;
        gabi::at<EnvironmentColors>(t)->mColorC0.r = (s16)(ld16(c0[g] + 0));
        gabi::at<EnvironmentColors>(t)->mColorC0.g = (s16)(ld16(c0[g] + 2));
        gabi::at<EnvironmentColors>(t)->mColorC0.b = (s16)(ld16(c0[g] + 4));
        gabi::at<EnvironmentColors>(t)->mColorC0.a = (s16)(ld16(c0[g] + 6));
        s16 kr = lds16(k0[g] + 0), kg = lds16(k0[g] + 2), kb = lds16(k0[g] + 4);
        gabi::at<EnvironmentColors>(t)->mColorK0.r = (s16)((u16)kr);
        gabi::at<EnvironmentColors>(t)->mColorK0.g = (s16)((u16)kg);
        gabi::at<EnvironmentColors>(t)->mColorK0.b = (s16)((u16)kb);
        gabi::at<EnvironmentColors>(t)->mColorK0.a = (s16)(ld16(k0[g] + 6));
        if (pl < 3) {
            gabi::Local<GXColorS10> c0c, k0c;
            u32 cca = gabi::ea(c0c.get()), kca = gabi::ea(k0c.get());
            st16(cca + 0, ld8(t + 0xF49));
            st16(cca + 2, ld8(t + 0xF4B));
            st16(cca + 4, ld8(t + 0xF4D));
            st16(kca + 0, (u8)kr);
            st16(kca + 2, (u8)kg);
            st16(kca + 4, (u8)kb);
            gabi::Local<GXColorS10> c0d, k0d;
            u32 cda = gabi::ea(c0d.get()), kda = gabi::ea(k0d.get());
            st(cda + 0, ld(cca + 0));
            st(cda + 4, ld(cca + 4));
            gabi::at<dKy_tevstr_c>(tev)->mLightMode = (u8)(2);
            st(kda + 0, ld(kca + 0));
            st(kda + 4, ld(kca + 4));
            dScnKy_env_light_c_settingTevStruct_plightcol_plus(t, pos, tev, cda, kda, timer);
        } else {
            gabi::Local<cXyz> vp;
            u32 va = gabi::ea(vp.get());
            transformLightPosition(0x104B45F8, 0x10476C74,
                                   va); /* PSMTXMultVec(view, lightStatusData[0].mPos) */
            st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mPosition.x), ld(va + 0));
            st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mPosition.y), ld(va + 4));
            st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mPosition.z), ld(va + 8));
            gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.x = (f32)(ldf(0x10476C74));
            gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.y = (f32)(ldf(0x10476C78));
            gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.z = (f32)(ldf(0x10476C7C));
            u32 env = environmentAddress();
            st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mDirection.x),
               ld(gabi::ea(&gabi::at<KankyoEnvLight>(env)->mLightDir.x)));
            st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mDirection.y),
               ld(gabi::ea(&gabi::at<KankyoEnvLight>(env)->mLightDir.y)));
            u32 dz = ld(gabi::ea(&gabi::at<KankyoEnvLight>(env)->mLightDir.z));
            gabi::at<TevLightParameters>(tev)->mLightObj.mColor.r = (u8)(0xFF);
            st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mDirection.z), dz);
            const f32 one = l_tevOne, zero = l_tevZero;
            u8 c = full ? 0xFF : 0;
            gabi::at<TevLightParameters>(tev)->mLightObj.mColor.g = (u8)(c);
            gabi::at<TevLightParameters>(tev)->mLightObj.mColor.b = (u8)(c);
            gabi::at<TevLightParameters>(tev)->mLightObj.mColor.a = (u8)(0xFF);
            gabi::at<TevLightParameters>(tev)->mLightObj.mCosAttenuation[0] = (f32)(one);
            gabi::at<TevLightParameters>(tev)->mLightObj.mCosAttenuation[1] = (f32)(zero);
            gabi::at<TevLightParameters>(tev)->mLightObj.mCosAttenuation[2] = (f32)(zero);
            gabi::at<TevLightParameters>(tev)->mLightObj.mDistanceAttenuation[0] = (f32)(one);
            gabi::at<TevLightParameters>(tev)->mLightObj.mDistanceAttenuation[1] = (f32)(zero);
            gabi::at<TevLightParameters>(tev)->mLightObj.mDistanceAttenuation[2] = (f32)(zero);
        }
    }
    copyTevWorkColors(t, tev);
    if (bgIdx == 1) {
        gabi::at<dKy_tevstr_c>(tev)->mFogColor.r =
            (s16)((u8)gabi::at<EnvironmentColors>(environmentAddress())->mVrUsoUmiColor.r);
        gabi::at<dKy_tevstr_c>(tev)->mFogColor.g =
            (s16)((u8)gabi::at<EnvironmentColors>(environmentAddress())->mVrUsoUmiColor.g);
        gabi::at<dKy_tevstr_c>(tev)->mFogColor.b =
            (s16)((u8)gabi::at<EnvironmentColors>(environmentAddress())->mVrUsoUmiColor.b);
        u8 a = (u8)gabi::at<EnvironmentColors>(environmentAddress())->mVrUsoUmiColor.a;
        gabi::at<dKy_tevstr_c>(tev)->mFogStartZ = (f32)(ldf(gabi::ea(fogStart.get())));
        gabi::at<dKy_tevstr_c>(tev)->mFogColor.a = (s16)(a);
        gabi::at<dKy_tevstr_c>(tev)->mFogEndZ = (f32)(ldf(gabi::ea(fogEnd.get())));
    } else {
        u16 f0 = ld16(fa + 0), f3 = ld16(fa + 6);
        gabi::at<dKy_tevstr_c>(tev)->mFogColor.r = (s16)(f0);
        gabi::at<dKy_tevstr_c>(tev)->mFogColor.g = (s16)(ld16(fa + 2));
        gabi::at<dKy_tevstr_c>(tev)->mFogColor.b = (s16)(ld16(fa + 4));
        gabi::at<dKy_tevstr_c>(tev)->mFogStartZ = (f32)(ldf(gabi::ea(fogStart.get())));
        gabi::at<dKy_tevstr_c>(tev)->mFogEndZ = (f32)(ldf(gabi::ea(fogEnd.get())));
        gabi::at<dKy_tevstr_c>(tev)->mFogColor.a = (s16)(f3);
    }
    u32 hp = 0;
    switch ((u32)type) {
    case TEV_TYPE_ACTOR:
    case TEV_TYPE_PLAYER:
    case TEV_TYPE_UNK99:
        hp = offsetof(lighting::EnvLight, mActorC0Intensity);
        break;
    case TEV_TYPE_BG0:
        hp = offsetof(lighting::EnvLight, mBG0C0Intensity);
        break;
    case TEV_TYPE_BG1:
        hp = offsetof(lighting::EnvLight, mLightSpread);
        break;
    case TEV_TYPE_BG2:
        hp = offsetof(lighting::EnvLight, mLightFalloff);
        break;
    case TEV_TYPE_BG3:
        hp = offsetof(lighting::EnvLight, mLightIndirectScale);
        break;
    default:
        break;
    }
    if (hp != 0) {
        gabi::at<TevLightParameters>(tev)->mLightObj.mK0Intensity = (f32)(ldf(t + hp + 4));
        gabi::at<TevLightParameters>(tev)->mLightObj.mC0Intensity = (f32)(ldf(t + hp));
    }
    for (u32 i = 0; i < 3; i++) {
        f32 base =
            (f32)colorComponent(gabi::at<EnvironmentColors>(environmentAddress())->mBgAmbientColor, i).get();
        s32 c = colorComponent(gabi::at<dKy_tevstr_c>(tev)->mColorC0, i).get();
        f32 d = i2f(
            c -
            (s32)colorComponent(gabi::at<EnvironmentColors>(environmentAddress())->mBgAmbientColor, i).get());
        f32 v = gabi::fmadds(d, (f32)gabi::at<lighting::EnvLight>(environmentAddress())->mDayNightFade, base);
        colorComponent(gabi::at<TevLightParameters>(tev)->mAmbientLight.mAmbientColor, i) = (u8)gabi::ftoi(v);
    }
    gabi::at<TevLightParameters>(tev)->mAmbientLight.mC0Intensity =
        (f32)((f32)gabi::at<lighting::EnvLight>(environmentAddress())->mFogDensity);
}

void copy_f32x12(u32 dst, u32 src);
void GXColorS10_to_f32(u32 dst, u32 src);
void GXColor_to_f32(u32 dst, u32 src);

// HD: material blocks expose virtual colour/fog accessors and float parameter slots.
// HD: set the expanded light objects and matching float material parameters.
/* 02562F5C */
void lighting6::dScnKy_env_light_c::setLightTevColorType(void *i_model, dKy_tevstr_c *i_tevstr) {
    u32 t = gabi::ea(this);
    u32 model = gabi::ea(i_model);
    u32 tev = gabi::ea(i_tevstr);

    /* stack objects at the original's sp offsets (frame 0x158), as in the disassembly */
    if ((u8)gabi::at<dKy_tevstr_c>(tev)->mInitType != TevLightingSet) {
        if ((u8)gabi::at<dKy_tevstr_c>(tev)->mInitType != TevInitialized)
            dKy_tevstr_init(tev, (s8)ld8(0x1047E6C8), 0xFF);
        dScnKy_env_light_c_settingTevStruct(t, TEV_TYPE_UNK99, 0, tev);
    }
    u32 n = ld16(getMaterialTable((u32)gabi::at<Model>(model)->mpModelData) + 0x24);
    s32 idx = (s32)n - 1;
    if (idx < 0)
        return;
    const f32 one = l_tevOne, k255 = l_tevByteColorScale;
    const u32 j3d = 0x104B45C0;
    for (u32 cnt = n; cnt != 0; cnt--, idx--) {
        u32 md = (u32)gabi::at<Model>(model)->mpModelData;
        u32 i16 = (u32)idx & 0xFFFF;
        u32 mat = (u32)gabi::at<ModelData>(md)->mpMaterials;
        if (i16 < (u32)gabi::at<ModelData>(md)->mMaterialNum)
            mat += i16 * 0x39C;
        environmentAddress();
        if ((u8)gabi::at<dKy_tevstr_c>(tev)->mLightMode != 0) {
            u32 tb = (u32)gabi::at<Material>(mat)->mpTevBlock;
            gabi::call_ptr(gabi::at<TevVtable>(gabi::at<TevBlock>(tb)->mpVtable)->getTevColor.get(), tb, 3);
        }
        gabi::FrameLocal<be<f32>[12]> view(0xAC);
        copy_f32x12(gabi::ea(view.get()), j3d + 0x38);
        gabi::FrameLocal<cXyz> lp(0x14);
        u32 la = gabi::ea(lp.get());
        stf(la + 0, (f32)gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.x);
        stf(la + 4, (f32)gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.y);
        stf(la + 8, (f32)gabi::at<dKy_tevstr_c>(tev)->mLightPosWorld.z);
        transformLightPosition(gabi::ea(view.get()), la, la); /* PSMTXMultVec */
        gabi::FrameLocal<cXyz> vp(0x20);
        u32 va = gabi::ea(vp.get());
        transformLightPosition(j3d + 0x38, tev + 0x84, va);
        st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mPosition.x), ld(va + 0));
        st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mPosition.y), ld(va + 4));
        st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mLightObj.mPosition.z), ld(va + 8));
        st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mMaterialLight.mPosition.x), ld(va + 0));
        st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mMaterialLight.mPosition.y), ld(va + 4));
        st(gabi::ea(&gabi::at<TevLightParameters>(tev)->mMaterialLight.mPosition.z), ld(va + 8));
        st(tev + 0xD8, ld(tev + 0x18));
        {
            u32 cb = (u32)gabi::at<Material>(mat)->mpColorBlock;
            gabi::call_ptr(gabi::at<PrimaryVtable>(gabi::at<PrimaryBlock>(cb)->mpVtable)->setLight.get(), cb,
                           0, tev + 0xC0); /* setLight(0, light) */
        }
        bool hasFog;
        if (toon_proc_check() && (u8)gabi::at<dKy_tevstr_c>(tev)->mLightMode != 0) {
            u32 tb = (u32)gabi::at<Material>(mat)->mpTevBlock;
            u32 kc =
                gabi::call_ptr<u32>(gabi::at<TevVtable>(gabi::at<TevBlock>(tb)->mpVtable)->getTevKColor.get(),
                                    tb, 0); /* getTevKColor(0) */
            if (kc != 0) {
                gabi::FrameLocal<be<u8>[4]> k(0x10);
                u32 ka = gabi::ea(k.get());
                u8 g = (u8)(s16)gabi::at<dKy_tevstr_c>(tev)->mColorC0.g,
                   r = (u8)(s16)gabi::at<dKy_tevstr_c>(tev)->mColorC0.r, a = ld8(kc + 3);
                st8(ka + 0, r);
                u8 b = (u8)(s16)gabi::at<dKy_tevstr_c>(tev)->mColorC0.b;
                st8(ka + 1, g);
                st8(ka + 2, b);
                st8(ka + 3, a);
                tb = (u32)gabi::at<Material>(mat)->mpTevBlock;
                gabi::call_ptr(gabi::at<TevVtable>(gabi::at<TevBlock>(tb)->mpVtable)->setTevKColor.get(), tb,
                               0, ka); /* setTevKColor(0) */
                gabi::FrameLocal<be<f32>[4]> f(0x7C);
                GXColor_to_f32(gabi::ea(f.get()), ka);
                gabi::FrameLocal<be<f32>[4]> sc(0x3C);
                u32 sa = gabi::ea(sc.get());
                scaleMaterialColor(sa, gabi::ea(f.get()), one);
                gabi::at<Material>(mat)->mParameterFlags =
                    (u32)((u32)gabi::at<Material>(mat)->mParameterFlags | MaterialKColorDirty);
                u32 prm = getMaterialParameter(gabi::ea(&gabi::at<Material>(mat)->mParameterFlags),
                                               MaterialKColorParameter);
                f32 al = (f32)ld8(ka + 3);
                f32 x = ldf(sa + 0), y = ldf(sa + 4), z = ldf(sa + 8);
                gabi::at<MaterialParameter>(prm)->g = (f32)(y);
                gabi::at<MaterialParameter>(prm)->b = (f32)(z);
                gabi::at<MaterialParameter>(prm)->r = (f32)(x);
                gabi::at<MaterialParameter>(prm)->a = (f32)(al / k255);
            }
            tb = (u32)gabi::at<Material>(mat)->mpTevBlock;
            u32 c =
                gabi::call_ptr<u32>(gabi::at<TevVtable>(gabi::at<TevBlock>(tb)->mpVtable)->getTevColor.get(),
                                    tb, 0); /* getTevColor(0) */
            if (c != 0) {
                gabi::FrameLocal<GXColorS10> cl(0x14);
                u32 cla = gabi::ea(cl.get());
                u8 g = (u8)gabi::at<dKy_tevstr_c>(tev)->mColorK0.g,
                   r = (u8)gabi::at<dKy_tevstr_c>(tev)->mColorK0.r;
                s16 a = lds16(c + 6);
                st16(cla + 0, r);
                u8 b = (u8)gabi::at<dKy_tevstr_c>(tev)->mColorK0.b;
                st16(cla + 2, g);
                st16(cla + 4, b);
                st16(cla + 6, (u16)a);
                tb = (u32)gabi::at<Material>(mat)->mpTevBlock;
                gabi::call_ptr(gabi::at<TevVtable>(gabi::at<TevBlock>(tb)->mpVtable)->setTevColor.get(), tb,
                               0, cla); /* setTevColor(0) */
                gabi::FrameLocal<be<f32>[4]> f(0x6C);
                GXColorS10_to_f32(gabi::ea(f.get()), cla);
                gabi::FrameLocal<be<f32>[4]> sc(0x2C);
                u32 sa = gabi::ea(sc.get());
                scaleMaterialColor(sa, gabi::ea(f.get()), one);
                gabi::at<Material>(mat)->mParameterFlags =
                    (u32)((u32)gabi::at<Material>(mat)->mParameterFlags | MaterialColorDirty);
                u32 prm = getMaterialParameter(gabi::ea(&gabi::at<Material>(mat)->mParameterFlags),
                                               MaterialColorParameter);
                f32 al = i2f(lds16(cla + 6));
                f32 x = ldf(sa + 0), y = ldf(sa + 4), z = ldf(sa + 8);
                gabi::at<MaterialParameter>(prm)->g = (f32)(y);
                gabi::at<MaterialParameter>(prm)->b = (f32)(z);
                gabi::at<MaterialParameter>(prm)->r = (f32)(x);
                gabi::at<MaterialParameter>(prm)->a = (f32)(al / k255);
            }
            u32 fb = (u32)gabi::at<Material>(mat)->mpPEBlock;
            hasFog =
                gabi::call_ptr<u32>(
                    gabi::at<PrimaryVtable>(gabi::at<PrimaryBlock>(fb)->mpVtable)->getFogInfo.get(), fb) != 0;
        } else {
            u32 tb = (u32)gabi::at<Material>(mat)->mpTevBlock;
            u32 c = gabi::call_ptr<u32>(
                gabi::at<TevVtable>(gabi::at<TevBlock>(tb)->mpVtable)->getTevColor.get(), tb, 0);
            if (c != 0) {
                gabi::at<dKy_tevstr_c>(tev)->mColorC0.a = (s16)(ld16(c + 6));
                tb = (u32)gabi::at<Material>(mat)->mpTevBlock;
                gabi::call_ptr(gabi::at<TevVtable>(gabi::at<TevBlock>(tb)->mpVtable)->setTevColor.get(), tb,
                               0, tev + 0x90);
                gabi::FrameLocal<be<f32>[4]> f(0x9C);
                GXColorS10_to_f32(gabi::ea(f.get()), tev + 0x90);
                gabi::FrameLocal<be<f32>[4]> sc(0x5C);
                u32 sa = gabi::ea(sc.get());
                scaleMaterialColor(sa, gabi::ea(f.get()), one);
                gabi::at<Material>(mat)->mParameterFlags =
                    (u32)((u32)gabi::at<Material>(mat)->mParameterFlags | MaterialColorDirty);
                u32 prm = getMaterialParameter(gabi::ea(&gabi::at<Material>(mat)->mParameterFlags),
                                               MaterialColorParameter);
                f32 al = i2f((s16)gabi::at<dKy_tevstr_c>(tev)->mColorC0.a);
                f32 x = ldf(sa + 0), y = ldf(sa + 4), z = ldf(sa + 8);
                gabi::at<MaterialParameter>(prm)->g = (f32)(y);
                gabi::at<MaterialParameter>(prm)->b = (f32)(z);
                gabi::at<MaterialParameter>(prm)->r = (f32)(x);
                gabi::at<MaterialParameter>(prm)->a = (f32)(al / k255);
                u16 b = (u16)gabi::at<dKy_tevstr_c>(tev)->mColorC0.b,
                    r = (u16)gabi::at<dKy_tevstr_c>(tev)->mColorC0.r,
                    g = (u16)gabi::at<dKy_tevstr_c>(tev)->mColorC0.g;
                st16(tev + 0xDC, r);
                st16(tev + 0xDE, g);
                st16(tev + 0xE0, b);
                f32 w = (f32)gabi::at<TevLightParameters>(tev)->mLightObj.mC0Intensity;
                u16 a = (u16)gabi::at<dKy_tevstr_c>(tev)->mColorC0.a;
                gabi::at<TevLightParameters>(tev)->mMaterialLight.mC0Intensity = (f32)(w);
                st16(tev + 0xE2, a);
                for (u32 i = 0; i < 3; i++) {
                    f32 base = (f32)colorComponent(
                                   gabi::at<EnvironmentColors>(environmentAddress())->mBgAmbientColor, i)
                                   .get();
                    s32 cc = colorComponent(gabi::at<dKy_tevstr_c>(tev)->mColorC0, i).get();
                    f32 d =
                        i2f(cc - (s32)colorComponent(
                                     gabi::at<EnvironmentColors>(environmentAddress())->mBgAmbientColor, i)
                                     .get());
                    f32 v = gabi::fmadds(
                        d, (f32)gabi::at<lighting::EnvLight>(environmentAddress())->mDayNightFade, base);
                    colorComponent(gabi::at<TevLightParameters>(tev)->mAmbientLight.mAmbientColor, i) =
                        (u8)gabi::ftoi(v);
                }
                gabi::at<TevLightParameters>(tev)->mAmbientLight.mC0Intensity =
                    (f32)((f32)gabi::at<lighting::EnvLight>(environmentAddress())->mFogDensity);
            }
            tb = (u32)gabi::at<Material>(mat)->mpTevBlock;
            u32 kc = gabi::call_ptr<u32>(
                gabi::at<TevVtable>(gabi::at<TevBlock>(tb)->mpVtable)->getTevKColor.get(), tb, 0);
            if (kc != 0) {
                gabi::at<dKy_tevstr_c>(tev)->mColorK0.a = (u8)(ld8(kc + 3));
                tb = (u32)gabi::at<Material>(mat)->mpTevBlock;
                f32 scale = (f32)gabi::at<TevLightParameters>(tev)->mLightObj.mK0Intensity;
                gabi::call_ptr(gabi::at<TevVtable>(gabi::at<TevBlock>(tb)->mpVtable)->setTevKColor.get(), tb,
                               0, tev + 0x98);
                gabi::FrameLocal<be<f32>[4]> f(0x8C);
                GXColor_to_f32(gabi::ea(f.get()), tev + 0x98);
                gabi::FrameLocal<be<f32>[4]> sc(0x4C);
                u32 sa = gabi::ea(sc.get());
                scaleMaterialColor(sa, gabi::ea(f.get()), scale);
                gabi::at<Material>(mat)->mParameterFlags =
                    (u32)((u32)gabi::at<Material>(mat)->mParameterFlags | MaterialKColorDirty);
                u32 prm = getMaterialParameter(gabi::ea(&gabi::at<Material>(mat)->mParameterFlags),
                                               MaterialKColorParameter);
                f32 al = (f32)(u8)gabi::at<dKy_tevstr_c>(tev)->mColorK0.a;
                f32 x = ldf(sa + 0), y = ldf(sa + 4), z = ldf(sa + 8);
                gabi::at<MaterialParameter>(prm)->g = (f32)(y);
                gabi::at<MaterialParameter>(prm)->b = (f32)(z);
                gabi::at<MaterialParameter>(prm)->r = (f32)(x);
                gabi::at<MaterialParameter>(prm)->a = (f32)(al / k255);
                gabi::at<TevLightParameters>(tev)->mAmbientLight.mColor.r = (u8)(0);
                gabi::at<TevLightParameters>(tev)->mAmbientLight.mColor.g = (u8)(0);
                gabi::at<TevLightParameters>(tev)->mAmbientLight.mColor.b = (u8)(0);
                gabi::at<TevLightParameters>(tev)->mAmbientLight.mColor.a = (u8)(0);
            }
            u32 cb = (u32)gabi::at<Material>(mat)->mpColorBlock;
            gabi::call_ptr(gabi::at<PrimaryVtable>(gabi::at<PrimaryBlock>(cb)->mpVtable)->setLight.get(), cb,
                           1, tev + 0x144); /* setLight(1, HD second light) */
            u32 fb = (u32)gabi::at<Material>(mat)->mpPEBlock;
            hasFog =
                gabi::call_ptr<u32>(
                    gabi::at<PrimaryVtable>(gabi::at<PrimaryBlock>(fb)->mpVtable)->getFogInfo.get(), fb) != 0;
        }
        if (!hasFog)
            continue;
        u32 fb = (u32)gabi::at<Material>(mat)->mpPEBlock;
        u32 fi = gabi::call_ptr<u32>(
            gabi::at<PrimaryVtable>(gabi::at<PrimaryBlock>(fb)->mpVtable)->getFogInfo.get(),
            fb); /* getFogInfo */
        u8 ty = (u8)gabi::at<FogInfo>(fi)->mType;
        if (!(ty == 0 || ty == 2))
            continue;
        gabi::at<FogInfo>(fi)->mType = (u8)(2);
        /* lfs/stfs pairs keep the bits (a signalling NaN is not quietened by the recompiler here) */
        u32 szb = ld(gabi::ea(&gabi::at<dKy_tevstr_c>(tev)->mFogStartZ));
        st(gabi::ea(&gabi::at<FogInfo>(fi)->mStartZ), szb);
        u32 ezb = ld(gabi::ea(&gabi::at<dKy_tevstr_c>(tev)->mFogEndZ));
        st(gabi::ea(&gabi::at<FogInfo>(fi)->mEndZ), ezb);
        if (gabi::f32_from_bits(szb) > gabi::f32_from_bits(ezb))
            st(gabi::ea(&gabi::at<FogInfo>(fi)->mStartZ), ezb);
        gabi::at<FogInfo>(fi)->mNearZ = (f32)(ldf(ld(playAddress() + 0x5FA4) + 0xCC));
        gabi::at<FogInfo>(fi)->mFarZ = (f32)(ldf(ld(playAddress() + 0x5FA4) + 0xD0));
        gabi::at<FogInfo>(fi)->mColor.r = (u8)((u8)(s16)gabi::at<dKy_tevstr_c>(tev)->mFogColor.r);
        gabi::at<FogInfo>(fi)->mColor.g = (u8)((u8)(s16)gabi::at<dKy_tevstr_c>(tev)->mFogColor.g);
        gabi::at<FogInfo>(fi)->mColor.b = (u8)((u8)(s16)gabi::at<dKy_tevstr_c>(tev)->mFogColor.b);
        gabi::at<FogInfo>(fi)->mColor.a = (u8)((u8)(s16)gabi::at<dKy_tevstr_c>(tev)->mFogColor.a);
    }
}

// HD: compiler-generated and template code of this translation unit.
/* 02563720 */
void d_kankyo_sinit_hd() {
    l_tevHeaderStatics.mWords[2] = 0;
    l_tevHeaderStatics.mWords[0] = 0;
    l_tevHeaderStatics.mWords[3] = 0;
    l_tevHeaderStatics.mWords[1] = 0;
    __register_global_object(gabi::ea(l_tevHeaderDestructors.mRecords[0]));
    f32 a = l_tevHeaderInitialA, b = l_tevHeaderInitialB;
    l_tevHeaderStatics.mInitialA = a;
    l_tevHeaderStatics.mInitialB = b;
    initializeHeaderByte(gabi::ea(&l_tevHeaderStatics.mByte));
    __register_global_object(gabi::ea(l_tevHeaderDestructors.mRecords[1]));
    initializeHeaderFlag(gabi::ea(&l_tevHeaderStatics.mFlag));
    __register_global_object(gabi::ea(l_tevHeaderDestructors.mRecords[2]));
    l_tevEnvironmentFlags.mInitialized = 0;
    l_tevEnvironmentFlags.mRoomOverride = 0xFF;
    l_tevEnvironmentFlags.mLightState = 0;
    l_tevEnvironmentFlags.mPaletteState = 0;
}

// HD: this constructor initializes the appended light influence factor.
/* 025637D4 */
u32 LIGHT_INFLUENCE_ct(u32 p) {
    if (p == 0) {
        p = allocateObject(sizeof(dKankyo::LIGHT_INFLUENCE));
        if (p == 0)
            return p;
    }
    gabi::at<dKankyo::LIGHT_INFLUENCE>(p)->mHD20 = l_tevOne;
    return p;
}

/* inline constructor of a 0x40-byte element (nothing to initialise) */
/* 02563814 */
u32 constructRenderRecord(u32 p) {
    if (p == 0)
        p = allocateObject(0x40);
    return p;
}

/* inline constructor of a 0x10-byte element (nothing to initialise) */
/* 02563840 */
u32 constructCompactRecord(u32 p) {
    if (p == 0)
        p = allocateObject(0x10);
    return p;
}

/* deleting destructor of a trivially destructible class (SafeString vtable +0xC) */
/* 0256386C */
void destroyTrivialObject(u32 p, u32 flags) {
    if (p == 0)
        return;
    if (flags & 1)
        operator_delete(gabi::at<void>(p)); /* __dl */
}

// HD: destroy the expanded render-state member before releasing the environment.
/* 02563880 */
void dScnKy_env_light_c_dt(u32 p, u32 flags) {
    if (p == 0)
        return;
    destroyEnvironmentRenderState(gabi::ea(&gabi::at<KankyoEnvLight>(p)->mHdObject74), 0);
    if (flags & 1)
        operator_delete(gabi::at<void>(p));
}

/* sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
/* 025638D4 */
void SafeString_assureTerminationImpl(u32 p) {}

// HD: ABI adapters for the other pieces of the original translation unit.
void dScnKy_env_light_c_setLight_actor(u32 t, u32 tev, u32 fog, u32 fogStart, u32 fogEnd) {
    gabi::at<lighting6::dScnKy_env_light_c>(t)->setLight_actor(
        gabi::at<dKy_tevstr_c>(tev), gabi::at<GXColorS10>(fog), gabi::at<be<f32>>(fogStart),
        gabi::at<be<f32>>(fogEnd));
}
void dScnKy_env_light_c_settingTevStruct_colget_actor(u32 t, u32 pos, u32 tev, u32 c0, u32 k0, u32 fog,
                                                      u32 fogStart, u32 fogEnd) {
    gabi::at<lighting6::dScnKy_env_light_c>(t)->settingTevStruct_colget_actor(
        gabi::at<cXyz>(pos), gabi::at<dKy_tevstr_c>(tev), gabi::at<GXColorS10>(c0), gabi::at<GXColorS10>(k0),
        gabi::at<GXColorS10>(fog), gabi::at<be<f32>>(fogStart), gabi::at<be<f32>>(fogEnd));
}
void dScnKy_env_light_c_settingTevStruct_colget_player(u32 t, u32 tev) {
    gabi::at<lighting6::dScnKy_env_light_c>(t)->settingTevStruct_colget_player(gabi::at<dKy_tevstr_c>(tev));
}
void dScnKy_env_light_c_settingTevStruct_plightcol_plus(u32 t, u32 pos, u32 tev, u32 c0, u32 k0, u32 timer) {
    gabi::at<lighting6::dScnKy_env_light_c>(t)->settingTevStruct_plightcol_plus(
        gabi::at<cXyz>(pos), gabi::at<dKy_tevstr_c>(tev), gabi::at<GXColorS10>(c0), gabi::at<GXColorS10>(k0),
        timer);
}
void dScnKy_env_light_c_settingTevStruct_eflightcol_plus(u32 t, u32 pos, u32 tev) {
    gabi::at<lighting6::dScnKy_env_light_c>(t)->settingTevStruct_eflightcol_plus(gabi::at<cXyz>(pos),
                                                                                 gabi::at<dKy_tevstr_c>(tev));
}
void dScnKy_env_light_c_setLight_bg(u32 t, u32 tev, u32 bg0c0, u32 bg0k0, u32 bg1c0, u32 bg1k0, u32 bg2c0,
                                    u32 bg2k0) {
    gabi::at<lighting6::dScnKy_env_light_c>(t)->setLight_bg(
        gabi::at<dKy_tevstr_c>(tev), gabi::at<GXColorS10>(bg0c0), gabi::at<GXColorS10>(bg0k0),
        gabi::at<GXColorS10>(bg1c0), gabi::at<GXColorS10>(bg1k0), gabi::at<GXColorS10>(bg2c0),
        gabi::at<GXColorS10>(bg2k0));
}
void dScnKy_env_light_c_settingTevStruct(u32 t, s32 type, u32 pos, u32 tev) {
    gabi::at<lighting6::dScnKy_env_light_c>(t)->settingTevStruct(type, gabi::at<cXyz>(pos),
                                                                 gabi::at<dKy_tevstr_c>(tev));
}
void dScnKy_env_light_c_setLightTevColorType(u32 t, u32 model, u32 tev) {
    gabi::at<lighting6::dScnKy_env_light_c>(t)->setLightTevColorType(gabi::at<void>(model),
                                                                     gabi::at<dKy_tevstr_c>(tev));
}
} // namespace d_kankyo_cpp

using namespace d_kankyo_cpp;
/* WWHD glue
data l_tevHeaderStatics 10475A4C
data l_tevEnvironmentFlags 104773B8
data l_tevHeaderDestructors 101E8ED0
data l_tevOne 1004F0A4
data l_tevZero 1004F0B0
data l_tevByteColorScale 1004F104
data l_tevSeaFogDistance 1004F1C4
data l_tevDirectLightThreshold 1004F29C
data l_tevFlickerDivisor 1004F2D0
data l_tevAmbientLightScale 1004F218
data l_tevPlayerLightDistance 1004F338
data l_tevPlayerBaseLightDistance 1004F180
data l_tevPlayerLightMinStep 1004F340
data l_tevPlayerLightEaseScale 1004F240
data l_tevPlayerLightStepScale 1004F344
data l_tevHeaderInitialA 1004F348
data l_tevHeaderInitialB 1004F34C
ext 025580FC void selectLightPalettes(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32)
ext 028E8DE8 f32 squaredDistance(u32, u32)
ext 0200ECD4 f32 approachLightCoordinate(u32, f32, f32, f32, f32)
ext 0201ADE0 void subtractLightPosition(u32, u32, u32)
ext 028E8F64 void transformLightPosition(u32, u32, u32)
ext 02561E14 void setBackgroundLight(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32)
ext 027F3F8C u32 getMaterialTable(u32)
ext 0274D458 void scaleMaterialColor(u32, u32, f32)
ext 027F9F0C u32 getMaterialParameter(u32, u32)
ext 028ED6F8 void initializeHeaderByte(u32)
ext 028EAB2C void initializeHeaderFlag(u32)
ext 027FB528 void destroyEnvironmentRenderState(u32, u32)
*/
