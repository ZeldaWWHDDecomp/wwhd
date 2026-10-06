/** Point-wind utility, WWHD. */
#include "d/d_point_wind.h"

static void set_pwind_init(dPointWind_c* wind, void* capsule) {
    WWHD_FUNC(0x025AB3F8, void, wind, capsule);
    wind->mpCps = capsule;
    u32 cps = gabi::ea(capsule);
    wind->mWind.mPos.x = gabi::load<f32>(cps);
    wind->mWind.mPos.y = gabi::load<f32>(cps + 4);
    wind->mWind.mPos.z = gabi::load<f32>(cps + 8);
    wind->mWind.mDir.x = gabi::load<f32>(cps + 0xC);
    wind->mWind.mDir.y = gabi::load<f32>(cps + 0x10);
    wind->mWind.mDir.z = gabi::load<f32>(cps + 0x14);
    f32 radius = gabi::load<f32>(cps + 0x18);
    wind->mWind.field_0x20 = 0.0f;
    wind->mWind.mRadius = radius;
    wind->mWind.mStrength = 1.0f;
    gabi::call(0x0257DE5C, &wind->mWind);
}
VERIFY(0x025AB3F8, set_pwind_init);

static void set_pwind_move(dPointWind_c* wind) {
    WWHD_FUNC(0x025AB454, void, wind);
    gabi::Local<cXyz> p0, p1, direction, sum, scaled;
    struct WindSafeString_l { be<u32> top; be<u32> vtable; };
    gabi::Local<WindSafeString_l> sea, stage;
    u32 cps = gabi::ea(wind->mpCps.get());
    p0->x = gabi::load<f32>(cps);
    p0->y = gabi::load<f32>(cps + 4);
    p0->z = gabi::load<f32>(cps + 8);
    p1->x = gabi::load<f32>(cps + 0xC);
    p1->y = gabi::load<f32>(cps + 0x10);
    p1->z = gabi::load<f32>(cps + 0x14);
    f32 radScale = 1.8f;
    bool animate = true;
    gabi::call(0x02563F64, p0.get(), p1.get(), direction.get());
    sea->top = 0x10052558;
    sea->vtable = 0x10052530;
    bool special = false;
    u32 play = dComIfGp_ea();
    stage->vtable = 0x10052530;
    stage->top = play + 0x5134;
    u32 vt = sea->vtable;
    u32 terminate = gabi::load<u32>(vt + 0x14);
    gabi::call_ptr(terminate, sea.get());
    vt = sea->vtable;
    terminate = gabi::load<u32>(vt + 0x14);
    gabi::call_ptr(terminate, sea.get());
    vt = stage->vtable;
    terminate = gabi::load<u32>(vt + 0x14);
    u32 seaTop = sea->top;
    gabi::call_ptr(terminate, stage.get());
    u32 stageTop = stage->top;
    bool equal = seaTop == stageTop;
    if (!equal) {
        u32 a = sea->top;
        u32 b = stage->top;
        for (u32 count = 0x40001; count != 0; --count) {
            u32 x = gabi::load<u8>(a);
            u32 y = gabi::load<u8>(b);
            if (x != y) break;
            if (x == 0) { equal = true; break; }
            ++a;
            ++b;
        }
    }
    if (equal && gabi::load<s8>(0x1047E6C8) == 4) special = true;
    if (special) {
        radScale = 11.0f;
        animate = false;
        gabi::call(0x0201AE48, direction.get(), scaled.get(), -100.0f);
        gabi::call(0x0201AD78, p0.get(), sum.get(), scaled.get());
        u32 x = gabi::load<u32>(gabi::ea(sum.get()));
        u32 z = gabi::load<u32>(gabi::ea(sum.get()) + 8);
        gabi::store<u32>(gabi::ea(wind) + 4, x);
        u32 y = gabi::load<u32>(gabi::ea(sum.get()) + 4);
        gabi::store<u32>(gabi::ea(wind) + 0xC, z);
        gabi::store<u32>(gabi::ea(wind) + 8, y);
    }
    u32 dy = gabi::load<u32>(gabi::ea(direction.get()) + 4);
    cps = gabi::ea(wind->mpCps.get());
    gabi::store<u32>(gabi::ea(wind) + 0x14, dy);
    u32 dx = gabi::load<u32>(gabi::ea(direction.get()));
    u32 dz = gabi::load<u32>(gabi::ea(direction.get()) + 8);
    gabi::store<u32>(gabi::ea(wind) + 0x10, dx);
    gabi::store<u32>(gabi::ea(wind) + 0x18, dz);
    f32 radius = gabi::load<f32>(cps + 0x18) * radScale;
    wind->mWind.field_0x20 = 0.0f;
    wind->mWind.mRadius = radius;
    if (animate) {
        f32 currentRadius = wind->mWind.mRadius;
        f32 half = currentRadius * 0.5f;
        cps = gabi::ea(wind->mpCps.get());
        f32 end = gabi::load<f32>(cps + 0xC);
        gabi::call(0x0200ECD4, &wind->mWind.mPos.x, end, 0.1f, currentRadius, half);
        currentRadius = wind->mWind.mRadius;
        cps = gabi::ea(wind->mpCps.get());
        half = currentRadius * 0.5f;
        end = gabi::load<f32>(cps + 0x10);
        gabi::call(0x0200ECD4, &wind->mWind.mPos.y, end, 0.1f, currentRadius, half);
        currentRadius = wind->mWind.mRadius;
        cps = gabi::ea(wind->mpCps.get());
        half = currentRadius * 0.5f;
        end = gabi::load<f32>(cps + 0x14);
        gabi::call(0x0200ECD4, &wind->mWind.mPos.z, end, 0.1f, currentRadius, half);
        f32 distance = gabi::call<f32>(0x028E8DE8, &wind->mWind.mPos, p1.get());
        distance = gabi::call<f32>(0x028F4384, distance);
        if (distance < (f32)wind->mWind.mRadius) {
            cps = gabi::ea(wind->mpCps.get());
            wind->mWind.mPos.x = gabi::load<f32>(cps);
            wind->mWind.mPos.y = gabi::load<f32>(cps + 4);
            wind->mWind.mPos.z = gabi::load<f32>(cps + 8);
        }
    }
}
VERIFY(0x025AB454, set_pwind_move);

static void set_pwind_delete(dPointWind_c* wind) {
    WWHD_FUNC(0x025AB710, void, wind);
    gabi::call(0x0257D1B8, &wind->mWind);
}
VERIFY(0x025AB710, set_pwind_delete);

static void point_wind_header_static() {
    WWHD_FUNC(0x025AB718, void);
    gabi::store<u32>(0x1047B358, 0);
    gabi::store<u32>(0x1047B350, 0);
    gabi::store<u32>(0x1047B35C, 0);
    gabi::store<u32>(0x1047B354, 0);
    gabi::call(0x028F026C, (u32)0x101EA5A8);
    gabi::store<f32>(0x1047B344, -3.141592741012573242f);
    gabi::store<f32>(0x1047B348, 3.141592741012573242f);
    gabi::call(0x028ED6F8, (u32)0x1047B34C);
    gabi::call(0x028F026C, (u32)0x101EA5B4);
    gabi::call(0x028EAB2C, (u32)0x1047B34D);
    gabi::call(0x028F026C, (u32)0x101EA5C0);
}
VERIFY(0x025AB718, point_wind_header_static);

static void point_wind_string_destructor(void* string, u32 flags) {
    WWHD_FUNC(0x025AB7AC, void, string, flags);
    if (string != nullptr && (flags & 1)) gabi::call(0x0273AF40, string);
}
VERIFY(0x025AB7AC, point_wind_string_destructor);

static void point_wind_string_terminate(void* string) {
    WWHD_FUNC(0x025AB7C0, void, string);
}
VERIFY(0x025AB7C0, point_wind_string_terminate);
