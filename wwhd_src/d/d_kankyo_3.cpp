/** Sun/moon placement and palette-based environment lighting. Verified HD behaviour. */
#include "d/d_kankyo_3.h"

namespace d_kankyo_cpp {
using namespace lighting;
static f32 l_lightingOne;
static f32 l_lightingZero;
static f32 l_lightingFadeStep;
static f32 l_lightingBaseLightRadiusScale;
static f32 l_lightingSeaLightMinStep;
static f32 l_lightingSunTimeOffset;
static f32 l_lightingByteColorScale;
static f32 l_lightingSunTimeWrap;
static f32 l_lightingDegreesToRadians;
static f32 l_lightingSunRadius;
static f32 l_lightingSunDepth;
static f32 l_lightingSeaSunCameraHeight;
static f32 l_lightingSeaSunDegrees;
static f32 l_lightingSeaSunRadians;
static f32 l_lightingSunHeightScale;
static f32 l_lightingSunHeightMinimum;
static f32 l_lightingRoomLightRadius;
static f32 l_lightingDayStart;
static f32 l_lightingDayEnd;
static f32 l_lightingDayLowerClamp;
static f32 l_lightingDayClampRadians;
static f32 l_lightingNightLowerClamp;
static f32 l_lightingNightUpperClamp;
static f32 l_lightingNightClampRadians;
static f32 l_lightingFadeScale;
static f32 l_lightingRoomBlendThreshold;
static f32 l_lightingRoomBlendReturn;
static f32 l_lightingRoomBlendOutMaxStep;
static f32 l_lightingRoomBlendOutMinStep;
static f32 l_lightingRoomBlendInMaxStep;
static f32 l_lightingTowerLightXZ;
static f32 l_lightingTowerLightY;
static f32 l_lightingSeaBlendScale;
static f32 l_lightingPaletteMinRate;
static f32 l_lightingSeaPaletteStep;
static f32 l_lightingRoomPaletteStep;
static f32 l_lightingSeaFogDistance;
static f32 l_lightingDefaultAttenuationScale;
static f32 l_lightingColorPatternAttenuationScale;
static SafeStringVtable l_lightingSafeStringVtable;
static u8 l_lightingStayRoom;
static SinCosTable l_lightingSinCos;
static OverridePalette l_lightingPaletteOverride;

static EnvLight& environment() { return *reinterpret_cast<EnvLight*>(dKy_getEnvlight()); }
static EnvLight& environmentAt(u32 address) { return *gabi::at<EnvLight>(address); }
static Play& play() { return *gabi::at<Play>(kankyo3_getPlay()); }

template <class Color> static be<u8>& component(Color& color, u32 index) {
    return *gabi::at<be<u8>>(gabi::ea(&color) + index);
}
template <class Record, class Field> static Field& entryField(u32 entry, Field Record::* member) {
    return gabi::at<Record>(entry)->*member;
}
static be<s16>& roomAngle(EnvLight& env, u32 room, bool azimuth) {
    // HD: Room indices retain unsigned guest-address arithmetic, including malformed inputs.
    u32 base = azimuth ? gabi::ea(env.mRoomLightAzimuth) : gabi::ea(env.mRoomLightElevation);
    return *gabi::at<be<s16>>(base + room * sizeof(be<s16>));
}

// HD: SafeString equality assures termination twice on the literal and once on the stage name.

static inline bool stageNameEquals(const char* lit) {
    gabi::Local<SafeString> a;
    a->str = gabi::guest_addr(lit);
    a->vtbl = gabi::ea(&l_lightingSafeStringVtable);
    u32 playAddress = kankyo3_getPlay();
    gabi::Local<SafeString> b;
    b->vtbl = gabi::ea(&l_lightingSafeStringVtable);
    b->str = gabi::ea(gabi::at<Play>(playAddress)->mStartStageName);
    gabi::call_ptr(gabi::at<SafeStringVtable>(a->vtbl)->assureTermination.get(), a.get());
    gabi::call_ptr(gabi::at<SafeStringVtable>(a->vtbl)->assureTermination.get(), a.get());
    u32 bv = b->vtbl;
    u32 s1 = a->str;
    gabi::call_ptr(gabi::at<SafeStringVtable>(bv)->assureTermination.get(), b.get());
    u32 s2 = b->str;
    if (s1 == s2)
        return true;
    for (u32 n = 0x40001; n != 0; n--) {
        u8 c1 = gabi::at<be<u8>>(s1)->get(), c2 = gabi::at<be<u8>>(s2)->get();
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
        s1++;
        s2++;
    }
    return false;
}

// HD: The sea-room event uses a fixed sun direction above the camera-height threshold.
// HD: The sun-height factor is retained and clamped after position updates.
/* 02557228 */
void EnvLight::setSunpos() {
    u32 cam = play().mpCamera.get();
    f32 ang, x, y, z;
    const f32 r80000 = l_lightingSunRadius;
    if (environment().mCurTime.get() > l_lightingSunTimeOffset) {
        f32 cur = environment().mCurTime.get();
        ang = (cur - l_lightingSunTimeOffset) * l_lightingDegreesToRadians;
    } else {
        f32 cur = environment().mCurTime.get();
        ang = (cur + l_lightingSunTimeWrap) * l_lightingDegreesToRadians;
    }
    x = kankyo3_sin(ang) * r80000;
    y = kankyo3_cos(ang) * r80000;
    f32 m48000 = l_lightingSunDepth;
    z = kankyo3_cos(ang) * m48000;
    bool set = play().mEventRunning.get() == 0; // event not running
    if (!set && environment().mInitAnimTimer.get() != 0)
        set = true; // mInitAnimTimer
    if (set) {
        mSunPos.x = gabi::at<Camera>(cam)->mEye.x.get() + x;
        mSunPos.y = gabi::at<Camera>(cam)->mEye.y.get() - y;
        mSunPos.z = gabi::at<Camera>(cam)->mEye.z.get() + z;
        mMoonPos.x = gabi::at<Camera>(cam)->mEye.x.get() - x;
        mMoonPos.y = gabi::at<Camera>(cam)->mEye.y.get() + y;
        mMoonPos.z = gabi::at<Camera>(cam)->mEye.z.get() - z;
    } else {
        bool special = false;
        if (stageNameEquals("sea")) { // "sea"
            if (play().mEventRunning.get() != 0 && (s8)l_lightingStayRoom == 44 &&
                gabi::at<Camera>(cam)->mEye.y.get() > l_lightingSeaSunCameraHeight)
                special = true;
        }
        if (special) {
            f32 a2 = l_lightingSeaSunRadians;
            f32 deg = l_lightingSeaSunDegrees;
            x = kankyo3_sin(a2) * r80000;
            y = kankyo3_cos(a2) * r80000;
            f32 c = kankyo3_cos(a2);
            mSunPos.x = gabi::at<Camera>(cam)->mEye.x.get() + x;
            mSunPos.y = gabi::at<Camera>(cam)->mEye.y.get() - y;
            f32 cz = gabi::at<Camera>(cam)->mEye.z.get();
            ang = deg * l_lightingDegreesToRadians;
            mSunPos.z = gabi::fmadds(c, m48000, cz);
        }
    }
    u32 env = gabi::ea(&environment());
    f32 c = kankyo3_cos(ang);
    f32 k = l_lightingSunHeightScale;
    f32 lo = l_lightingSunHeightMinimum;
    environmentAt(env).mSunHeightFactor = -c * k;
    if (environment().mSunHeightFactor.get() < lo)
        environment().mSunHeightFactor = lo;
}

// palette index by selector 0..5 (no store for other values)
static inline void pickPalette(u32 dst, u32 psel, u32 sel) {
    if (sel < PaletteCount)
        (*gabi::at<be<u8>>(dst)) = gabi::at<PaletteSelection>(psel)->palette_id[sel].get();
}

// HD: Missing palettes skip stores; sea changes blend slowly and ma2room blends in 15 frames.
/* 025580FC */
void EnvLight::setLight_palno_get(u32 i_envrSel0, u32 i_envrSel1, u32 i_pSelIdx0, u32 i_pSelIdx1,
                                  u32 i_palIdx0A, u32 i_palIdx0B, u32 i_palIdx1A) {
    const auto* stack = gabi::at<PaletteStackArgs>(gabi::cpu->r[1]);
    const u32 i_palIdx1B = stack->palIdx1B.get(), i_blendPalAB = stack->blendPalAB.get(),
              i_pSelPalIdx0 = stack->pSelPalIdx0.get(), i_pSelPalIdx1 = stack->pSelPalIdx1.get(),
              i_blendPal01 = stack->blendPal01.get(), i_initTimer = stack->initTimer.get();
    const f32 one = l_lightingOne;
    u8 timer = (*gabi::at<be<u8>>(i_initTimer)).get();
    if (timer != 0) {
        u8 t1 = (u8)(timer + 1);
        (*gabi::at<be<u8>>(i_initTimer)) = t1 > 20 ? 0 : t1;
        if (environment().mColPatMode.get() == 0 && environment().mColPatModeGather.get() == 0) {
            f32 b = environment().mColPatBlend.get();
            (*gabi::at<be<f32>>(i_blendPal01)) = b;
            if (!(b < one)) {
                (*gabi::at<be<u8>>(i_envrSel0)) = (*gabi::at<be<u8>>(i_envrSel1)).get();
                (*gabi::at<be<u8>>(i_pSelIdx0)) = (*gabi::at<be<u8>>(i_pSelIdx1)).get();
            }
        }
    }
    u32 sched = mpSchejule.get();
    f32 cur = mCurTime.get();
    u32 e = sched;
    for (s32 i = 0; i < 11; i++, e += sizeof(Schedule)) {
        if (cur < gabi::at<Schedule>(e)->mTimeEnd.get())
            continue;
        if (cur > gabi::at<Schedule>(e)->mTimeBegin.get())
            continue;
        u32 off = i * sizeof(Schedule);
        (*gabi::at<be<u32>>(i_pSelPalIdx0)) = gabi::at<Schedule>(sched + off)->mPalIdx0.get();
        (*gabi::at<be<u32>>(i_pSelPalIdx1)) =
            gabi::at<Schedule>(mpSchejule.get() + off)->mPalIdx1.get();
        u32 sc = mpSchejule.get();
        f32 tEnd = gabi::at<Schedule>(sc + off)->mTimeEnd.get(),
            tBegin = gabi::at<Schedule>(sc + off)->mTimeBegin.get();
        f32 d = tBegin - tEnd;
        const f32 zero = l_lightingZero;
        f32 c = mCurTime.get();
        f32 pct = one;
        if (d != zero) {
            f32 r = one - (tBegin - c) / d;
            if (r < one)
                pct = r;
        }
        (*gabi::at<be<f32>>(i_blendPalAB)) = pct;
        u32 env = gabi::ea(&environment());
        u32 sel = (*gabi::at<be<u8>>(i_pSelIdx0)).get();
        u32 envr = environmentAt(env).mpEnvrInfo.get() +
                   (*gabi::at<be<u8>>(i_envrSel0)).get() * sizeof(EnvironmentSelection);
        u32 psel;
        if (sel > 7) {
            JUT_ASSERT_fail("d_kankyo.cpp", 0xA99, "0");
        } else {
            env = gabi::ea(&environment());
            psel = environmentAt(env).mpPselectInfo.get() +
                   gabi::at<EnvironmentSelection>(envr)->pselect_id[sel].get() *
                       sizeof(PaletteSelection);
            if (psel != 0) {
                pickPalette(i_palIdx0A, psel, (*gabi::at<be<u32>>(i_pSelPalIdx0)).get());
                pickPalette(i_palIdx0B, psel, (*gabi::at<be<u32>>(i_pSelPalIdx1)).get());
            }
        }
        env = gabi::ea(&environment());
        u32 sel1 = (*gabi::at<be<u8>>(i_pSelIdx1)).get();
        envr = environmentAt(env).mpEnvrInfo.get() +
               (*gabi::at<be<u8>>(i_envrSel1)).get() * sizeof(EnvironmentSelection);
        if (sel1 > 7) {
            JUT_ASSERT_fail("d_kankyo.cpp", 0xB3F, "0");
            return;
        }
        env = gabi::ea(&environment());
        psel =
            environmentAt(env).mpPselectInfo.get() +
            gabi::at<EnvironmentSelection>(envr)->pselect_id[sel1].get() * sizeof(PaletteSelection);
        if (psel == 0)
            return;
        if ((*gabi::at<be<u8>>(i_envrSel0)).get() != (*gabi::at<be<u8>>(i_envrSel1)).get() ||
            (*gabi::at<be<u8>>(i_pSelIdx0)).get() != (*gabi::at<be<u8>>(i_pSelIdx1)).get()) {
            const f32 rate = l_lightingPaletteMinRate;
            if (gabi::at<PaletteSelection>(psel)->change_rate.get() < rate)
                gabi::at<PaletteSelection>(psel)->change_rate = rate;
            if (environment().mColPatMode.get() == 0) {
                bool done;
                bool slow = false;
                if (stageNameEquals("sea") &&
                    (*gabi::at<be<u8>>(i_pSelIdx0)).get() != (*gabi::at<be<u8>>(i_pSelIdx1)).get())
                    slow = true; // "sea"
                if (slow) {
                    f32 b = (*gabi::at<be<f32>>(i_blendPal01)).get() + l_lightingSeaPaletteStep;
                    (*gabi::at<be<f32>>(i_blendPal01)) = b;
                    done = !(b < one);
                } else if (!(gabi::at<PaletteSelection>(psel)->change_rate.get() > zero)) {
                    done = !((*gabi::at<be<f32>>(i_blendPal01)).get() < one);
                } else if (stageNameEquals("ma2room")) { // "ma2room"
                    f32 b = (*gabi::at<be<f32>>(i_blendPal01)).get() + l_lightingRoomPaletteStep;
                    (*gabi::at<be<f32>>(i_blendPal01)) = b;
                    done = !(b < one);
                } else {
                    f32 step = rate / gabi::at<PaletteSelection>(psel)->change_rate.get();
                    f32 b = (*gabi::at<be<f32>>(i_blendPal01)).get() + step;
                    (*gabi::at<be<f32>>(i_blendPal01)) = b;
                    done = !(b < one);
                }
                if (done) {
                    (*gabi::at<be<u8>>(i_envrSel0)) = (*gabi::at<be<u8>>(i_envrSel1)).get();
                    (*gabi::at<be<u8>>(i_pSelIdx0)) = (*gabi::at<be<u8>>(i_pSelIdx1)).get();
                    (*gabi::at<be<f32>>(i_blendPal01)) = one;
                }
            }
        }
        pickPalette(i_palIdx1A, psel, (*gabi::at<be<u32>>(i_pSelPalIdx0)).get());
        pickPalette(i_palIdx1B, psel, (*gabi::at<be<u32>>(i_pSelPalIdx1)).get());
        return;
    }
}

s16 kankyo_color_ratio_set(u32 b0A, u32 b0B, f32 blendAB0, u32 b1A, u32 b1B, f32 blendAB1, s32 add,
                           f32 blend01);
f32 float_kankyo_color_ratio_set(f32 p0, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6, f32 p7);
f32 float_kankyo_color_ratio_set_noratio(f32 p0, f32 p1, f32 p2, f32 p3, f32 p4, f32 p5, f32 p6,
                                         f32 p7);

// setLight: the four blended palette (or vrbox) entries palIdx0A/0B/1A/1B as passed to kankyo_color_ratio_set (a, b, blendAB, c, d)
struct BlendEntries {
    u32 a, b, c, d;
};

// HD: Each K0 byte fades toward C0; preload the next byte before storing the current one.
static void fadeK0(GXColor& c0, GXColor& k0, f32 zero) {
    if (!(environment().mDayNightFade.get() > zero))
        return;
    f32 prev = component(k0, 0).get();
    for (u32 i = 0; i < 3; ++i) {
        u32 env = gabi::ea(&environment());
        f32 c = component(c0, i).get();
        f32 value = gabi::fmadds(c - prev, environmentAt(env).mDayNightFade.get(), prev);
        if (i < 2)
            prev = component(k0, i + 1).get();
        component(k0, i) = (u8)gabi::ftoi(value);
    }
}

// HD: A fixed palette override is copied by words; material lighting and fog gain parameters.
/* 02558C40 */
void EnvLight::setLight() {
    u32 initTimer = gabi::ea(&environment().mInitAnimTimer);
    if (environment().mPaletteOverride.get() != NoPaletteOverride) {
        u32 dst = environment().mpPaletInfo.get();
        for (u32 i = 0; i < 27; ++i)
            gabi::at<OverridePalette>(dst)->words[i] = l_lightingPaletteOverride.words[i].get();
    }
    u32 envrPrev = gabi::ea(&environment().mEnvrIdxPrev);
    u32 envrCurr = gabi::ea(&environment().mEnvrIdxCurr);
    u32 colPrev = gabi::ea(&environment().mColpatPrev);
    u32 colCurr = gabi::ea(&environment().mColpatCurr);
    u32 blend01 = gabi::ea(&environment().mColPatBlend);
    gabi::Local<be<f32>> blendAB;
    gabi::Local<be<u8>> pal0A, pal0B, pal1A, pal1B;
    gabi::Local<be<s32>> psel0, psel1;
    kankyo3_selectPalettes(gabi::ea(this), envrPrev, envrCurr, colPrev, colCurr, pal0A.a, pal0B.a,
                           pal1A.a, pal1B.a, blendAB.a, psel0.a, psel1.a, blend01, initTimer);
    // These four addresses are native bookkeeping, not an additional guest stack record.
    auto p = BlendEntries{};
    if (environment().mPaletteOverride.get() == NoPaletteOverride) {
        p.a = environment().mpPaletInfo.get() + (u8)*pal0A * sizeof(PaletteHD);
        p.b = environment().mpPaletInfo.get() + (u8)*pal0B * sizeof(PaletteHD);
        p.c = environment().mpPaletInfo.get() + (u8)*pal1A * sizeof(PaletteHD);
        p.d = environment().mpPaletInfo.get() + (u8)*pal1B * sizeof(PaletteHD);
    } else {
        p.a = environment().mpPaletInfo.get();
        p.b = environment().mpPaletInfo.get();
        p.c = environment().mpPaletInfo.get();
        p.d = environment().mpPaletInfo.get();
    }
    // Accessor calls precede all colour loads; a fresh accessor provides the component ratio.
    auto blendColor = [&](const BlendEntries& q, auto color, u32 index,
                          be<f32> EnvLight::* ratioMember, const be<s16>* add,
                          GXColor& destination) {
        f32 blend = environment().mColPatBlend.get();
        f32 ratio = (environment().*ratioMember).get();
        s32 bias = add ? add->get() : 0;
        s16 value = kankyo_color_ratio_set(
            component(entryField(q.a, color), index).get(),
            component(entryField(q.b, color), index).get(), blendAB->get(),
            component(entryField(q.c, color), index).get(),
            component(entryField(q.d, color), index).get(), blend, bias, ratio);
        component(destination, index) = (u8)value;
    };
    // actor C0: ratio = mActColRatio^2 (two accessor calls)
    for (u32 i = 0; i < 3; i++) {
        f32 blend = environment().mColPatBlend.get();
        u32 e1 = gabi::ea(&environment());
        u32 e2 = gabi::ea(&environment());
        f32 ratio = environmentAt(e1).mActColRatio.get() * environmentAt(e2).mActColRatio.get();
        s16 r = kankyo_color_ratio_set(
            component(gabi::at<PaletteHD>(p.a)->mActor_C0, i).get(),
            component(gabi::at<PaletteHD>(p.b)->mActor_C0, i).get(), blendAB->get(),
            component(gabi::at<PaletteHD>(p.c)->mActor_C0, i).get(),
            component(gabi::at<PaletteHD>(p.d)->mActor_C0, i).get(), blend, 0, ratio);
        component(mActorC0, i) = (u8)r;
    }
    for (u32 i = 0; i < 3; ++i)
        blendColor(p, &PaletteHD::mActor_K0, i, &EnvLight::mActColRatio, nullptr, mActorK0);
    const f32 zero = l_lightingZero;
    fadeK0(mActorC0, mActorK0, zero);
    for (u32 i = 0; i < 3; ++i)
        blendColor(p, &PaletteHD::mBG0_C0, i, &EnvLight::mBgColRatio, nullptr, mBG0_C0);
    for (u32 i = 0; i < 3; ++i)
        blendColor(p, &PaletteHD::mBG0_K0, i, &EnvLight::mBgColRatio, nullptr, mBG0_K0);
    fadeK0(mBG0_C0, mBG0_K0, zero);
    for (u32 i = 0; i < 3; ++i)
        blendColor(p, &PaletteHD::mBG1_C0, i, &EnvLight::mBgColRatio, nullptr, mBG1_C0);
    for (u32 i = 0; i < 3; ++i)
        blendColor(p, &PaletteHD::mBG1_K0, i, &EnvLight::mBgColRatio, nullptr, mBG1_K0);
    for (u32 i = 0; i < 3; ++i)
        blendColor(p, &PaletteHD::mBG2_C0, i, &EnvLight::mBgColRatio, nullptr, mBG2_C0);
    for (u32 i = 0; i < 3; ++i)
        blendColor(p, &PaletteHD::mBG2_K0, i, &EnvLight::mBgColRatio, nullptr, mBG2_K0);
    for (u32 i = 0; i < 3; ++i)
        blendColor(p, &PaletteHD::mBG3_C0, i, &EnvLight::mBgColRatio, nullptr, mBG3_C0);
    for (u32 i = 0; i < 3; ++i)
        blendColor(p, &PaletteHD::mBG3_K0, i, &EnvLight::mBgColRatio, nullptr, mBG3_K0);
    for (u32 i = 0; i < 3; ++i)
        blendColor(p, &PaletteHD::mFog, i, &EnvLight::mFogColRatio,
                   gabi::at<be<s16>>(gabi::ea(&mAddColFog) + i * sizeof(be<s16>)), mFogColor);
    {
        // HD: Fog alpha is stored as a float in palettes and converted to a byte.
        const f32 k255 = l_lightingByteColorScale;
        f32 blend = environment().mColPatBlend.get();
        f32 ratio = environment().mFogColRatio.get();
        u8 a = (u8)gabi::ftoi(gabi::at<PaletteHD>(p.a)->mFogAlpha.get() * k255),
           b = (u8)gabi::ftoi(gabi::at<PaletteHD>(p.b)->mFogAlpha.get() * k255),
           c = (u8)gabi::ftoi(gabi::at<PaletteHD>(p.c)->mFogAlpha.get() * k255),
           d = (u8)gabi::ftoi(gabi::at<PaletteHD>(p.d)->mFogAlpha.get() * k255);
        s16 r = kankyo_color_ratio_set(a, b, blendAB->get(), c, d, blend, 0, ratio);
        mFogColor.a = (u8)r;
    }
    {
        // Fog distances exclude the all-colour ratio.
        f32 blend = environment().mColPatBlend.get();
        f32 sz = environment().mFogStartZ.get();
        f32 ratio = environment().mFogRatio.get();
        f32 v = float_kankyo_color_ratio_set_noratio(
            gabi::at<PaletteHD>(p.a)->mFogStartZ.get(), gabi::at<PaletteHD>(p.b)->mFogStartZ.get(),
            blendAB->get(), gabi::at<PaletteHD>(p.c)->mFogStartZ.get(),
            gabi::at<PaletteHD>(p.d)->mFogStartZ.get(), blend, sz, ratio);
        mFogStartZ__setLight = v;
        u32 e1 = gabi::ea(&environment());
        u32 e2 = gabi::ea(&environment());
        u32 e3 = gabi::ea(&environment());
        f32 ez = float_kankyo_color_ratio_set_noratio(
            gabi::at<PaletteHD>(p.a)->mFogEndZ.get(), gabi::at<PaletteHD>(p.b)->mFogEndZ.get(),
            blendAB->get(), gabi::at<PaletteHD>(p.c)->mFogEndZ.get(),
            gabi::at<PaletteHD>(p.d)->mFogEndZ.get(), environmentAt(e1).mColPatBlend.get(),
            environmentAt(e2).mFogEndZ.get(), environmentAt(e3).mFogRatio.get());
        mFogEndZ__setLight = ez;
        u32 e = gabi::ea(&environment());
        ez = gabi::fmadds(l_lightingSeaFogDistance, environmentAt(e).mSeaLightBlend.get(), ez);
        f32 szv = mFogStartZ__setLight.get();
        mFogEndZ__setLight = ez;
        if (szv > ez)
            mFogStartZ__setLight = ez;
    }
    // HD: An additional ambient colour and scalar material parameters follow the GC palette.
    for (u32 i = 0; i < 3; ++i)
        blendColor(p, &PaletteHD::mBgAmbientColor, i, &EnvLight::mBgColRatio,
                   gabi::at<be<s16>>(gabi::ea(&mBgAddColAmb) + i * sizeof(be<s16>)),
                   mBgAmbientColorHD);
    auto blendFloatParameter = [&](auto member, be<f32>& destination) {
        u32 env = gabi::ea(&environment());
        f32 value = float_kankyo_color_ratio_set(
            entryField(p.a, member).get(), entryField(p.b, member).get(), blendAB->get(),
            entryField(p.c, member).get(), entryField(p.d, member).get(),
            environmentAt(env).mColPatBlend.get(), zero, zero);
        destination = value;
    };
    blendFloatParameter(&PaletteHD::mFogDensity, mFogDensity);
    blendFloatParameter(&PaletteHD::mActorC0Intensity, mActorC0Intensity);
    blendFloatParameter(&PaletteHD::mActorK0Intensity, mActorK0Intensity);
    blendFloatParameter(&PaletteHD::mBG0C0Intensity, mBG0C0Intensity);
    blendFloatParameter(&PaletteHD::mBG0K0Intensity, mBG0K0Intensity);
    blendFloatParameter(&PaletteHD::mLightSpread, mLightSpread);
    {
        // HD: Colour patterns select the attenuation multiplier independently for each pair.
        f32 b = gabi::at<PaletteHD>(p.b)->mLightAttenuation.get();
        const f32 one = l_lightingOne;
        f32 a = gabi::at<PaletteHD>(p.a)->mLightAttenuation.get(),
            d = gabi::at<PaletteHD>(p.d)->mLightAttenuation.get(),
            c = gabi::at<PaletteHD>(p.c)->mLightAttenuation.get();
        if (environment().mPaletteAttenuation.get() != zero) {
            f32 k = l_lightingDefaultAttenuationScale;
            if (environment().mColpatPrev.get() == 0) {
                f32 e1 = environment().mPaletteAttenuation.get();
                f32 aa = gabi::at<PaletteHD>(p.a)->mLightAttenuation.get();
                f32 fa = gabi::fnmsubs(k, e1, one);
                f32 e2 = environment().mPaletteAttenuation.get();
                f32 fb = gabi::fnmsubs(k, e2, one);
                a = aa * fa;
                b = gabi::at<PaletteHD>(p.b)->mLightAttenuation.get() * fb;
            } else {
                f32 e1 = environment().mPaletteAttenuation.get();
                f32 aa = gabi::at<PaletteHD>(p.a)->mLightAttenuation.get();
                f32 fa = gabi::fnmsubs(l_lightingColorPatternAttenuationScale, e1, one);
                f32 e2 = environment().mPaletteAttenuation.get();
                f32 fb = gabi::fnmsubs(l_lightingColorPatternAttenuationScale, e2, one);
                a = aa * fa;
                b = gabi::at<PaletteHD>(p.b)->mLightAttenuation.get() * fb;
            }
            if (environment().mColpatCurr.get() != 0) {
                f32 e1 = environment().mPaletteAttenuation.get();
                c = gabi::at<PaletteHD>(p.c)->mLightAttenuation.get() *
                    gabi::fnmsubs(l_lightingColorPatternAttenuationScale, e1, one);
                f32 e2 = environment().mPaletteAttenuation.get();
                d = gabi::at<PaletteHD>(p.d)->mLightAttenuation.get() *
                    gabi::fnmsubs(l_lightingColorPatternAttenuationScale, e2, one);
            } else {
                f32 e1 = environment().mPaletteAttenuation.get();
                c = gabi::at<PaletteHD>(p.c)->mLightAttenuation.get() * gabi::fnmsubs(k, e1, one);
                f32 e2 = environment().mPaletteAttenuation.get();
                d = gabi::at<PaletteHD>(p.d)->mLightAttenuation.get() * gabi::fnmsubs(k, e2, one);
            }
        }
        u32 env = gabi::ea(&environment());
        (mLightAttenuation = float_kankyo_color_ratio_set(
             a, b, blendAB->get(), c, d, environmentAt(env).mColPatBlend.get(), zero, zero));
    }
    blendFloatParameter(&PaletteHD::mLightFalloff, mLightFalloff);
    blendFloatParameter(&PaletteHD::mLightRange, mLightRange);
    blendFloatParameter(&PaletteHD::mLightIndirectScale, mLightIndirectScale);
    blendFloatParameter(&PaletteHD::mLightIndirectBias, mLightIndirectBias);
    if (environment().mDayNightFade.get() > zero) {
        f32 prev = mActorK0Intensity.get();
        u32 env = gabi::ea(&environment());
        f32 v = gabi::fmadds(mActorC0Intensity.get() - prev, environmentAt(env).mDayNightFade.get(),
                             prev);
        prev = mBG0K0Intensity.get();
        mActorK0Intensity = v;
        env = gabi::ea(&environment());
        v = gabi::fmadds(mBG0C0Intensity.get() - prev, environmentAt(env).mDayNightFade.get(),
                         prev);
        mBG0K0Intensity = v;
    }
    // Palette virtual indices select the four skybox records.
    {
        u32 ib = gabi::at<PaletteHD>(p.b)->mVirtIdx.get(),
            ia = gabi::at<PaletteHD>(p.a)->mVirtIdx.get(),
            ic = gabi::at<PaletteHD>(p.c)->mVirtIdx.get(),
            id = gabi::at<PaletteHD>(p.d)->mVirtIdx.get();
        p.a = environment().mpVrboxInfo.get() + ia * sizeof(SkyboxHD);
        p.b = environment().mpVrboxInfo.get() + ib * sizeof(SkyboxHD);
        p.c = environment().mpVrboxInfo.get() + ic * sizeof(SkyboxHD);
        p.d = environment().mpVrboxInfo.get() + id * sizeof(SkyboxHD);
    }
    for (u32 i = 0; i < 3; i++)
        blendColor(p, &SkyboxHD::mSkyColor, i, &EnvLight::mVrboxSoraColRatio,
                   gabi::at<be<s16>>(gabi::ea(&mVrboxAddColSky0) + i * sizeof(be<s16>)),
                   mVrSkyColor); // sky + mVrboxAddColSky0
    mVrSkyColor.a = 0xFF;
    for (u32 i = 0; i < 3; i++)
        blendColor(p, &SkyboxHD::mUsoUmiColor, i, &EnvLight::mVrboxSoraColRatio,
                   gabi::at<be<s16>>(gabi::ea(&mVrboxAddColSky0) + i * sizeof(be<s16>)),
                   mVrUsoUmiColor); // uso umi
    {
        const f32 k255 = l_lightingByteColorScale;
        f32 blend = environment().mColPatBlend.get();
        f32 ratio = environment().mVrboxSoraColRatio.get();
        u8 a = (u8)gabi::ftoi(gabi::at<SkyboxHD>(p.a)->mUsoUmiAlpha.get() * k255),
           b = (u8)gabi::ftoi(gabi::at<SkyboxHD>(p.b)->mUsoUmiAlpha.get() * k255),
           c = (u8)gabi::ftoi(gabi::at<SkyboxHD>(p.c)->mUsoUmiAlpha.get() * k255),
           d = (u8)gabi::ftoi(gabi::at<SkyboxHD>(p.d)->mUsoUmiAlpha.get() * k255);
        s16 r = kankyo_color_ratio_set(a, b, blendAB->get(), c, d, blend, mVrboxAddColSky0.b.get(),
                                       ratio);
        mVrUsoUmiColor.a = (u8)r;
    }
    for (u32 i = 0; i < 3; i++)
        blendColor(p, &SkyboxHD::mKumoColor, i, &EnvLight::mVrboxKumoColRatio,
                   gabi::at<be<s16>>(gabi::ea(&mVrboxAddColSky0) + i * sizeof(be<s16>)),
                   mVrkumoColor); // kumo
    {
        u32 env = gabi::ea(&environment());
        f32 one = l_lightingOne;
        s16 r = kankyo_color_ratio_set(gabi::at<SkyboxHD>(p.a)->mKumoColor.a.get(),
                                       gabi::at<SkyboxHD>(p.b)->mKumoColor.a.get(), blendAB->get(),
                                       gabi::at<SkyboxHD>(p.c)->mKumoColor.a.get(),
                                       gabi::at<SkyboxHD>(p.d)->mKumoColor.a.get(),
                                       environmentAt(env).mColPatBlend.get(), 0, one);
        mVrkumoColor.a = (u8)r;
    }
    for (u32 i = 0; i < 3; i++)
        blendColor(p, &SkyboxHD::mKumoCenterColor, i, &EnvLight::mVrboxKumoColRatio,
                   gabi::at<be<s16>>(gabi::ea(&mVrboxAddColSky0) + i * sizeof(be<s16>)),
                   mVrkumoCenterColor); // kumo center
    for (u32 i = 0; i < 3; i++)
        blendColor(p, &SkyboxHD::mKasumiMaeColor, i, &EnvLight::mVrboxSoraColRatio,
                   gabi::at<be<s16>>(gabi::ea(&mVrboxAddColKasumi) + i * sizeof(be<s16>)),
                   mVrKasumiMaeColor); // kasumi mae + mVrboxAddColKasumi
    mVrKasumiMaeColor.a = 0xFF;
    blendFloatParameter(&SkyboxHD::mLightScale, mVrLightScale);
    blendFloatParameter(&SkyboxHD::mLightNear, mVrLightNear);
    blendFloatParameter(&SkyboxHD::mLightFar, mVrLightFar);
    blendFloatParameter(&SkyboxHD::mLightBias, mVrLightBias);
    if (stageNameEquals("kenroom") || stageNameEquals("ShipD")) { // "kenroom", "ShipD"
        u8 r = mFogColor.r.get(), g = mFogColor.g.get();
        mVrUsoUmiColor.r = r;
        u8 b = mFogColor.b.get();
        mVrUsoUmiColor.g = g;
        mVrUsoUmiColor.b = b;
    }
}

// HD: Base light follows the room direction or HD sun position; sea rooms also blend fog.
/* 02557CF4 */
void EnvLight::SetBaseLight() {
    u32 env = gabi::ea(&environment());
    u32 dir = kankyo3_roomLight(gabi::ea(this));
    f32 zero = l_lightingZero;
    bool sea;
    if (dir != 0) {
        mBaseLightInfluence.mPos.x = gabi::at<RoomLight>(dir)->position.x.get();
        mBaseLightInfluence.mPos.y = gabi::at<RoomLight>(dir)->position.y.get();
        f32 dz = gabi::at<RoomLight>(dir)->position.z.get();
        mBaseLightInfluence.mColor.r = 0;
        mBaseLightInfluence.mPos.z = dz;
        mBaseLightInfluence.mColor.g = 0;
        mBaseLightInfluence.mColor.b = 0;
        mBaseLightInfluence.mColor.a = 0;
        (mBaseLightInfluence.mPower =
             gabi::at<RoomLight>(dir)->radius.get() * l_lightingBaseLightRadiusScale);
        (mBaseLightInfluence.mFluctuation = (f32)gabi::at<RoomLight>(dir)->fluctuation.get());
        sea = stageNameEquals("sea");
    } else {
        if (stageNameEquals("M2tower")) { // "M2tower"
            f32 a = l_lightingTowerLightXZ, b = l_lightingTowerLightY;
            environmentAt(env).mSunPos2.x = a;
            environmentAt(env).mSunPos2.y = b;
            environmentAt(env).mSunPos2.z = a;
            mBaseLightInfluence.mPos.x = a;
            f32 y = environmentAt(env).mSunPos2.y.get();
            mBaseLightInfluence.mPower = zero;
            mBaseLightInfluence.mPos.y = y;
            mBaseLightInfluence.mColor.r = 0xFF;
            mBaseLightInfluence.mColor.g = 0xFF;
            mBaseLightInfluence.mColor.b = 0xFF;
            mBaseLightInfluence.mColor.a = 0xFF;
            mBaseLightInfluence.mPos.z = a;
        } else {
            f32 x = environmentAt(env).mSunPos2.x.get(), z = environmentAt(env).mSunPos2.z.get();
            mBaseLightInfluence.mPos.x = x;
            f32 y = environmentAt(env).mSunPos2.y.get();
            mBaseLightInfluence.mPower = zero;
            mBaseLightInfluence.mPos.y = y;
            mBaseLightInfluence.mColor.r = 0xFF;
            mBaseLightInfluence.mPos.z = z;
            mBaseLightInfluence.mColor.g = 0xFF;
            mBaseLightInfluence.mColor.b = 0xFF;
            mBaseLightInfluence.mColor.a = 0xFF;
        }
        mBaseLightInfluence.mFluctuation = zero;
        sea = stageNameEquals("sea");
    }
    if (!sea)
        return;
    bool lit = play().mSeaLightState.get() == 3;
    if (!lit) {
        s32 room = (s8)l_lightingStayRoom;
        lit = room == 0 || room == 13 || room == 26;
    }
    f32 target = zero;
    if (lit)
        target = l_lightingOne;
    u32 e = gabi::ea(&environment());
    kankyo3_addCalc(gabi::ea(&environmentAt(e).mSeaLightBlend), target, l_lightingSeaBlendScale,
                    l_lightingFadeStep, l_lightingSeaLightMinStep);
}

// HD: Additional room direction and light-position routines.
// HD: Room angle tables select a direction, with a deferred room during transitions.
/* 02557614 */
u32 dKy_room_light_get_hd() {
    u32 env0 = gabi::ea(&environment());
    u32 room = l_lightingStayRoom;
    u32 ret = 0;
    if (environment().mRoomLightBlendRoom.get() != NoBlendRoom &&
        environment().mRoomLightBlendTarget.get() > l_lightingOne) {
        if (environment().mRoomLightBlendRoom.get() == room) {
            environment().mRoomLightBlendRoom = NoBlendRoom;
        } else {
            room = environment().mRoomLightBlendRoom.get();
        }
    }
    s16 a = (s16)roomAngle(environmentAt(env0), room, false).get(),
        b = (s16)roomAngle(environmentAt(env0), room, true).get();
    if ((a | b) != 0) {
        const u32 tbl = gabi::ea(&l_lightingSinCos); // {sin, cos} per 8 angle units
        f32 v0 = gabi::at<SinCos>(tbl + ((u16)a >> 3) * sizeof(SinCos))->cos.get() *
                 gabi::at<SinCos>(tbl + ((u16)b >> 3) * sizeof(SinCos))->sin.get();
        environmentAt(env0).mRoomLight.position.x = v0;
        f32 k = l_lightingRoomLightRadius;
        f32 v1 =
            gabi::at<SinCos>(tbl + ((u16)roomAngle(environmentAt(env0), room, false).get() >> 3) *
                                       sizeof(SinCos))
                ->sin.get();
        environmentAt(env0).mRoomLight.position.y = v1;
        u32 ia = (u16)roomAngle(environmentAt(env0), room, false).get() >> 3,
            ib = (u16)roomAngle(environmentAt(env0), room, true).get() >> 3;
        f32 v2 = gabi::at<SinCos>(tbl + ia * sizeof(SinCos))->cos.get() *
                 gabi::at<SinCos>(tbl + ib * sizeof(SinCos))->cos.get();
        environmentAt(env0).mRoomLight.position.x = v0 * k;
        environmentAt(env0).mRoomLight.position.y = v1 * k;
        ret = gabi::ea(&environmentAt(env0).mRoomLight.position.x);
        environmentAt(env0).mRoomLight.position.z = v2 * k;
    }
    if (stageNameEquals("M2tower"))
        ret = 0; // "M2tower"
    return ret;
}

// HD: Day/night lighting fades and dungeon room-light transitions are additional state.
/* 02557834 */
void EnvLight::setLightPosHD() {
    u32 cam = play().mpCamera.get();
    u32 dir = kankyo3_roomLight(gabi::ea(this));
    f32 ang;
    if (environment().mCurTime.get() > l_lightingSunTimeOffset) {
        f32 cur = environment().mCurTime.get();
        ang = (cur - l_lightingSunTimeOffset) * l_lightingDegreesToRadians;
    } else {
        f32 cur = environment().mCurTime.get();
        ang = (cur + l_lightingSunTimeWrap) * l_lightingDegreesToRadians;
    }
    f32 r80000 = l_lightingSunRadius;
    f32 x = kankyo3_sin(ang) * r80000;
    f32 y = kankyo3_cos(ang) * r80000;
    f32 c = kankyo3_cos(ang);
    const f32 zero = l_lightingZero;
    f32 m48000 = l_lightingSunDepth;
    const f32 one = l_lightingOne;
    f32 z = c * m48000;
    f32 f28 = l_lightingFadeStep;
    if (dir == 0) {
        f32 cur = mCurTime.get();
        if (cur > l_lightingDayStart && cur < l_lightingDayEnd) {
            bool alt = true;
            if (!(environment().mCurTime.get() < l_lightingDayLowerClamp)) {
                if (!(environment().mCurTime.get() > l_lightingByteColorScale))
                    alt = false;
            }
            if (alt)
                y = kankyo3_cos(l_lightingDayClampRadians) * r80000;
            if (environment().mNightLightSide.get() != 0) {
                environment().mNightLightSide = 0;
                environment().mDayNightFadeTarget = one;
            }
            if (!(environment().mDayNightFadeTarget.get() > zero)) {
                mSunPos2.x = gabi::at<Camera>(cam)->mEye.x.get() + x;
                mSunPos2.y = gabi::at<Camera>(cam)->mEye.y.get() - y;
                mSunPos2.z = gabi::at<Camera>(cam)->mEye.z.get() + z;
            }
        } else {
            bool alt = true;
            if (!(environment().mCurTime.get() < l_lightingNightLowerClamp)) {
                if (!(environment().mCurTime.get() > l_lightingNightUpperClamp))
                    alt = false;
            }
            if (alt)
                y = kankyo3_cos(l_lightingNightClampRadians) * r80000;
            if (environment().mNightLightSide.get() == 0) {
                environment().mNightLightSide = 1;
                environment().mDayNightFadeTarget = one;
            }
            if (!(environment().mDayNightFadeTarget.get() > zero)) {
                mSunPos2.x = gabi::at<Camera>(cam)->mEye.x.get() - x;
                mSunPos2.y = gabi::at<Camera>(cam)->mEye.y.get() + y;
                mSunPos2.z = gabi::at<Camera>(cam)->mEye.z.get() - z;
            }
        }
        f32 target = environment().mDayNightFadeTarget.get();
        kankyo3_addCalc(gabi::ea(&environment().mDayNightFade), target, l_lightingFadeScale, f28,
                        f28);
        if (!(environment().mDayNightFade.get() < one))
            environment().mDayNightFadeTarget = zero;
    }
    u32 player = play().mpPlayer.get();
    u32 sd = gabi::ea(&play().mStageData);
    u32 info = gabi::call_ptr<u32>(
        gabi::at<StageVtable>(gabi::at<StageData>(sd)->vtbl)->getStagInfo.get(), sd);
    u32 type = (gabi::at<StageInfo>(info)->mStageTypeAndSchbit.get() >> 16) & 7;
    if (!(type == Dungeon || type == Boss || type == Subdungeon || type == Miniboss))
        return;
    if (player != 0) {
        if (gabi::at<Player>(player)->mRoomLightState.get() == RoomLightPlayerState) {
            if (!(environment().mRoomLightBlendTarget.get() > zero))
                environment().mRoomLightBlendTarget = one;
            if (!(environment().mRoomLightBlend.get() < l_lightingRoomBlendThreshold)) {
                f32 two = l_lightingRoomBlendReturn;
                environment().mRoomLightBlendTarget = two;
            }
        } else {
            if (!(environment().mRoomLightBlendTarget.get() < l_lightingRoomBlendReturn)) {
                if (!(environment().mRoomLightBlend.get() > f28))
                    environment().mRoomLightBlendTarget = zero;
            }
        }
    }
    if (environment().mRoomLightBlendTarget.get() > one) {
        f32 a = l_lightingRoomBlendOutMaxStep, b = l_lightingRoomBlendOutMinStep,
            s = l_lightingFadeScale;
        kankyo3_addCalc(gabi::ea(&environment().mRoomLightBlend), zero, s, a, b);
    } else {
        f32 target = environment().mRoomLightBlendTarget.get();
        f32 a = l_lightingRoomBlendInMaxStep, s = l_lightingSunHeightMinimum;
        kankyo3_addCalc(gabi::ea(&environment().mRoomLightBlend), target, s, a, f28);
    }
}

// Existing TU callers retain their native ABI adapters; verification enters the methods above.
void dScnKy_env_light_c_SetBaseLight(u32 object) { environmentAt(object).SetBaseLight(); }
void dScnKy_env_light_c_setSunpos(u32 object) { environmentAt(object).setSunpos(); }
void dScnKy_env_light_c_setLightPos_hd(u32 object) { environmentAt(object).setLightPosHD(); }
void dScnKy_env_light_c_setLight(u32 object) { environmentAt(object).setLight(); }
} // namespace d_kankyo_cpp
using namespace d_kankyo_cpp;
using namespace d_kankyo_cpp::lighting;

/* WWHD glue
data l_lightingSeaPaletteStep 1004F19C
data l_lightingSeaSunRadians 1004F130
data l_lightingSeaSunDegrees 1004F12C
data l_lightingSeaSunCameraHeight 1004F128
str 1004F1D8 "ShipD" in 02558C40
str 1004F1D0 "kenroom" in 02558C40
str 1004F1AC "sea" in 025580FC
str 1004F1A4 "ma2room" in 025580FC
str 1004F194 "sea" in 02557CF4
str 1004F18C "M2tower" in 02557CF4
str 1004F144 "M2tower" in 02557614
str 1004F13C "sea" in 02557228
ext 028F43F8 f32 kankyo3_sin(f32)
ext 028F4BE0 f32 kankyo3_cos(f32)
ext 0200ECD4 f32 kankyo3_addCalc(u32, f32, f32, f32, f32)
ext 02557614 u32 kankyo3_roomLight(u32)
ext 025580FC void kankyo3_selectPalettes(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32)
ext 025200D4 u32 kankyo3_getPlay()
str 1004F1B4 "d_kankyo.cpp"
str 1004F1B0 "0"

data l_lightingOne 1004F0A4
data l_lightingZero 1004F0B0
data l_lightingFadeStep 1004F0BC
data l_lightingBaseLightRadiusScale 1004F0C4
data l_lightingSeaLightMinStep 1004F0D0
data l_lightingSunTimeOffset 1004F0FC
data l_lightingByteColorScale 1004F104
data l_lightingSunTimeWrap 1004F118
data l_lightingDegreesToRadians 1004F11C
data l_lightingSunRadius 1004F120
data l_lightingSunDepth 1004F124
data l_lightingSunHeightScale 1004F134
data l_lightingSunHeightMinimum 1004F138
data l_lightingRoomLightRadius 1004F140
data l_lightingDayStart 1004F14C
data l_lightingDayEnd 1004F150
data l_lightingDayLowerClamp 1004F154
data l_lightingDayClampRadians 1004F158
data l_lightingNightLowerClamp 1004F15C
data l_lightingNightUpperClamp 1004F160
data l_lightingNightClampRadians 1004F164
data l_lightingFadeScale 1004F168
data l_lightingRoomBlendThreshold 1004F16C
data l_lightingRoomBlendReturn 1004F170
data l_lightingRoomBlendOutMaxStep 1004F174
data l_lightingRoomBlendOutMinStep 1004F178
data l_lightingRoomBlendInMaxStep 1004F17C
data l_lightingTowerLightXZ 1004F180
data l_lightingTowerLightY 1004F184
data l_lightingSeaBlendScale 1004F188
data l_lightingPaletteMinRate 1004F198
data l_lightingRoomPaletteStep 1004F1A0
data l_lightingSeaFogDistance 1004F1C4
data l_lightingDefaultAttenuationScale 1004F1C8
data l_lightingColorPatternAttenuationScale 1004F1CC
data l_lightingSafeStringVtable 1004EF7C
data l_lightingStayRoom 1047E6C8
data l_lightingSinCos 104A44F8
data l_lightingPaletteOverride 101E8E30
*/
