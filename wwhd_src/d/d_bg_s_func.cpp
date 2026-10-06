#include "gabi.h"

// HD object: 64-byte cBgS_GndChk plus two embedded filters, total84 bytes.
struct GroundCheck { u8 storage[84]; };
struct Bounds { gabi::be<f32> top; gabi::be<f32> bottom; };
static void configure(u32 check, bool water) {
    gmem_st32(check, check + 0x40);
    gmem_st32(check + 4, check + 0x4C);
    gmem_st32(check + 0x10, water ? 0x10043B14u : 0x10043AD4u);
    gmem_st32(check + 0x20, water ? 0x10043B24u : 0x10043AE4u);
    gmem_st32(check + 0x40, water ? 0x10043B44u : 0x10043B04u);
    gmem_st32(check + 0x4C, water ? 0x10043B34u : 0x10043AF4u);
    gmem_st32(check + 0x50, water ? 2 : 1);
    for (u32 i = 0x44; i <= 0x4A; ++i) gmem_st8(check + i, 0);
    gmem_st8(check + 0x44, 1);
}
static void teardown(u32 check) {
    gmem_st32(check + 0x20, 0x10043AA4);
    gmem_st32(check + 0x40, 0x10043AC4);
    gmem_st32(check + 0x4C, 0x10043A84);
    gabi::call<void>(0x02008DAC, check, 0);
}
static u32 bgspace() { return gabi::call<u32>(0x025200D4) + 0x12A0; }

f32 dBgS_ObjGndChk_Func(u32 pos) {
    WWHD_FUNC(0x024F1350, f32, pos);
    gabi::Local<GroundCheck> check;
    gabi::call<void>(0x02008E0C, check.get());
    configure(check.a, false);
    u32 x = gmem_ld32(pos);
    u32 z = gmem_ld32(pos + 8);
    u32 y = gmem_ld32(pos + 4);
    gmem_st32(check.a + 0x24, x);
    gmem_st32(check.a + 0x28, y);
    gmem_st32(check.a + 0x2C, z);
    u32 bg = bgspace();
    f32 height = gabi::call<f32>(0x02008974, bg, check.get());
    teardown(check.a);
    return height;
}
VERIFY(0x024F1350, dBgS_ObjGndChk_Func);

f32 dBgS_ObjGndChk_Wtr_Func(u32 pos) {
    WWHD_FUNC(0x024F1478, f32, pos);
    gabi::Local<GroundCheck> check;
    gabi::call<void>(0x02008E0C, check.get());
    configure(check.a, true);
    u32 x = gmem_ld32(pos);
    u32 z = gmem_ld32(pos + 8);
    u32 y = gmem_ld32(pos + 4);
    gmem_st32(check.a + 0x24, x);
    gmem_st32(check.a + 0x28, y);
    gmem_st32(check.a + 0x2C, z);
    u32 bg = bgspace();
    f32 height = gabi::call<f32>(0x02008974, bg, check.get());
    teardown(check.a);
    return height;
}
VERIFY(0x024F1478, dBgS_ObjGndChk_Wtr_Func);

BOOL dBgS_SplGrpChk_In_ObjGnd(u32 pos, u32 group, f32 offset) {
    WWHD_FUNC(0x024F15A4, BOOL, pos, group, offset);
    gabi::Local<GroundCheck> check;
    gabi::Local<Bounds> bounds;
    gabi::call<void>(0x02008E0C, check.get());
    configure(check.a, false);
    f32 z = gabi::load<f32>(pos + 8);
    f32 y = gabi::load<f32>(pos + 4);
    f32 x = gabi::load<f32>(pos);
    gabi::store<f32>(check.a + 0x28, y + offset);
    gabi::store<f32>(check.a + 0x24, x);
    gabi::store<f32>(check.a + 0x2C, z);
    u32 bg = bgspace();
    f32 height = gabi::call<f32>(0x02008974, bg, check.get());
    gabi::store<f32>(group + 0x48, height);
    BOOL success = 0;
    if (height != gabi::load<f32>(0x10043B54)) {
        bg = bgspace();
        u32 room = gabi::call<u32>(0x024EF130, bg, check.a + 0x14);
        if (room >= 64) gabi::call<void>(0x0273AA24, 0x10043B58u, 93, 0x10043B68u);
        gabi::call<void>(0x025200D4);
        u32 world = gmem_ld32(0x1047E8F4 + room * 0x22C);
        if (world) {
            gabi::call<void>(0x0200A3FC, world, bounds.get(), bounds.a + 4);
            f32 px = gabi::load<f32>(pos);
            f32 pz = gabi::load<f32>(pos + 8);
            f32 top = bounds->top;
            gmem_ld32(pos + 4);
            gabi::store<f32>(group + 0x44, top);
            gabi::store<f32>(group + 0x38, px);
            gabi::store<f32>(group + 0x3C, height);
            gabi::store<f32>(group + 0x40, pz);
            bg = bgspace();
            if (gabi::call<BOOL>(0x024EF7C0, bg, group)) {
                f32 actual = gabi::load<f32>(group + 0x48);
                f32 originalY = gabi::load<f32>(pos + 4);
                if (actual > originalY) gmem_st32(group + 0x4C, gmem_ld32(group + 0x4C) | 2);
                success = 1;
            }
        }
    }
    teardown(check.a);
    return success;
}
VERIFY(0x024F15A4, dBgS_SplGrpChk_In_ObjGnd);

f32 dBgS_GetWaterHeight(u32 pos) {
    WWHD_FUNC(0x024F17D4, f32, pos);
    gabi::Local<GroundCheck> check;
    gabi::call<void>(0x02008E0C, check.get());
    configure(check.a, true);
    gmem_st32(check.a + 0x24, gmem_ld32(pos));
    gmem_st32(check.a + 0x2C, gmem_ld32(pos + 8));
    gmem_st32(check.a + 0x28, gmem_ld32(pos + 4));
    u32 bg = bgspace();
    f32 height = gabi::call<f32>(0x02008974, bg, check.get());
    f32 z = gabi::load<f32>(pos + 8);
    f32 x = gabi::load<f32>(pos);
    if (gabi::call<BOOL>(0x0246B6A4, x, z)) {
        x = gabi::load<f32>(pos);
        z = gabi::load<f32>(pos + 8);
        f32 wave = gabi::call<f32>(0x0246BA0C, x, z);
        if (height < wave) height = wave;
    }
    teardown(check.a);
    return height;
}
VERIFY(0x024F17D4, dBgS_GetWaterHeight);

u32 dBgS_GetGndMtrlSndId_Func(u32 pos, f32 offset) {
    WWHD_FUNC(0x024F1914, u32, pos, offset);
    gabi::store<f32>(pos + 4, gabi::load<f32>(pos + 4) + offset);
    gabi::Local<GroundCheck> check;
    gabi::call<void>(0x02008E0C, check.get());
    configure(check.a, false);
    u32 z = gmem_ld32(pos + 8);
    u32 x = gmem_ld32(pos);
    u32 y = gmem_ld32(pos + 4);
    gmem_st32(check.a + 0x24, x);
    gmem_st32(check.a + 0x2C, z);
    gmem_st32(check.a + 0x28, y);
    u32 bg = bgspace();
    f32 height = gabi::call<f32>(0x02008974, bg, check.get());
    u32 result = 0;
    if (height != gabi::load<f32>(0x10043B54)) {
        bg = bgspace();
        result = gabi::call<u32>(0x024EECAC, bg, check.a + 0x14);
    }
    teardown(check.a);
    return result;
}
VERIFY(0x024F1914, dBgS_GetGndMtrlSndId_Func);

void dBgS_Func_StaticInit() {
    WWHD_FUNC(0x024F1A68, void);
    gmem_st32(0x1046ED3C, 0);
    gmem_st32(0x1046ED34, 0);
    gmem_st32(0x1046ED40, 0);
    gmem_st32(0x1046ED38, 0);
    gabi::call<void>(0x028F026C, 0x101D5270u);
    u32 lower = gmem_ld32(0x10043B8C);
    u32 upper = gmem_ld32(0x10043B90);
    gmem_stf32(0x1046ED28, lower);
    gmem_stf32(0x1046ED2C, upper);
    gabi::call<void>(0x028ED6F8, 0x1046ED30u);
    gabi::call<void>(0x028F026C, 0x101D527Cu);
    gabi::call<void>(0x028EAB2C, 0x1046ED31u);
    gabi::call<void>(0x028F026C, 0x101D5288u);
}
VERIFY(0x024F1A68, dBgS_Func_StaticInit);
