/**
 * d_a_mant.cpp (WWHD)
 * Cape (cloth) of the Darknuts and Phantom Ganon.
 *
 * The GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_mant.cpp) has only "Nonmatching" stubs for this unit; WWHD
 * draws the cape through its own GX2 packet (shader programs, uniform blocks, double-buffered
 * vertex buffers), so everything here is written from the WWHD code and verified against
 * cking.rpx. The HD packet retains its cape simulation while using GX2 resources.
 */
#include "d/actor/d_a_mant.h"

#define MANT_VTBL 0x100145DC        /* mant_class vtable (HD virtual destructor) */
#define MANT_PACKET_VTBL 0x10014714 /* daMant_packet_c vtable */
#define SAFESTRING_VTBL 0x10014574  /* this TU's sead::SafeString vtable */
#define wind_cc_sph_src gabi::at<dCcD_SrcSph>(0x101BA218)
#define mesh_cc_sph_src gabi::at<dCcD_SrcSph>(0x101BA258)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 027F0E04 J3DDrawBuffer::entryImm(J3DPacket*, u16) */
static inline void J3DDrawBuffer_entryImm(u32 buf, void* packet, u32 idx) { gabi::call(0x027F0E04, buf, packet, idx); }
/* j3dSys opa draw buffer (HD) */
static inline u32 j3dSys_getDrawBuffer0() { return gabi::load<u32>(0x104B4634); }
/* 028EFFD0 __construct_array(p, n, size, ctor), 028F0164 __destroy_arr(p, n, size, dtor) */
static inline void __construct_array(u32 p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(u32 p, u32 n, u32 size, u32 dtor) { gabi::call(0x028F0164, p, n, size, dtor, 0, 0); }
/* 025D672C / 025D673C: fopAcM_setCullSizeBox halves (HD: min and max corner set separately) */
static inline void fopAcM_setCullSizeBoxMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_setCullSizeBoxMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
/* sead heap free: findContainHeap(mgr, ptr) (02755FEC on the heap manager at *0x101F8B4C), then
 * heap->free(ptr) (vtable at +0xC, slot +0x3C) */
static inline void sead_free(u32 field) {
    u32 heap = gabi::call<u32>(0x02755FEC, gabi::load<u32>(0x101F8B4C), gabi::load<u32>(field));
    u32 vt = gabi::load<u32>(heap + 0xC);
    u32 fn = gabi::load<u32>(vt + 0x3C);
    gabi::call_ptr(fn, heap, gabi::load<u32>(field)); /* the pointer is re-read after the call */
}

enum { Type_DARKNUT_e = 0, Type_PHANTOM_GANON_e = 1 };


static inline u32 packet(mant_class* i_this) { return gabi::ea(i_this) + 0x3B4; }

/* 021BBAD0: Mtx -> sead::Matrix34f (12 floats through FPRs) */
static void mant_mtx_copy(void* dst, const void* src) {
    WWHD_FUNC(0x021BBAD0, void, dst, src);
    f32 t[12];
    for (int i = 0; i < 12; i++) t[i] = gabi::load<f32>(gabi::ea(src) + 4 * i);
    for (int i = 0; i < 12; i++) gabi::store<f32>(gabi::ea(dst) + 4 * i, t[i]);
}
VERIFY(0x021BBAD0, mant_mtx_copy);

/* 021BBB70: GXColorS10 -> float colour (each component / 255) */
static void mant_color_f(void* dst, const void* src) {
    WWHD_FUNC(0x021BBB70, void, dst, src);
    u32 s = gabi::ea(src);
    f32 r = (f32)gabi::load<s16>(s + 0) / 255.0f;
    f32 g = (f32)gabi::load<s16>(s + 2) / 255.0f;
    f32 b = (f32)gabi::load<s16>(s + 4) / 255.0f;
    f32 a = (f32)gabi::load<s16>(s + 6) / 255.0f;
    u32 d = gabi::ea(dst);
    gabi::store<f32>(d + 0x0, r);
    gabi::store<f32>(d + 0x4, g);
    gabi::store<f32>(d + 0x8, b);
    gabi::store<f32>(d + 0xC, a);
}
VERIFY(0x021BBB70, mant_color_f);

static inline void mant_copy_packet_matrix(u32 src, u32 dst) {
    gabi::Local<Mtx34> matrix;
    f32 values[12];
    for (u32 k = 0; k < 12; ++k) values[k] = gabi::load<f32>(src + k * 4);
    for (u32 k = 0; k < 12; ++k) gabi::store<f32>(gabi::ea(matrix.get()) + k * 4, values[k]);
    gabi::Local<be<u32>[4]> linkage;
    PSMTXCopy(matrix.get(), gabi::at<Mtx34>(dst));
}

/* 021BBC34: fill the current cape vertex buffer and refresh its material uniforms. */
static void daMant_packet_c_draw(void* self) {
    WWHD_FUNC(0x021BBC34, void, self);
    u32 p = gabi::ea(self), db = p + 0x8BC;
    u32 active = gabi::load<u32>(p + 0xD64);
    u32 vertices = gabi::load<u32>(db + active * 0x254);
    u32 end = vertices + 0x1200;
    if (vertices < end) {
        for (u32 line = vertices; line < end; line += 0x20)
            for (u32 k = 0; k < 8; ++k) gabi::store<u32>((line & ~31u) + k * 4, 0);
        active = gabi::load<u32>(p + 0xD64);
    }
    vertices = gabi::load<u32>(db + active * 0x254);
    for (u32 k = 0; k < gabi::load<u32>(0x101B9A78); ++k) {
        u32 indices = 0x101B9D68 + k * 3;
        u32 position = p + 0xFC + gabi::load<u8>(indices) * 12;
        f32 x = gabi::load<f32>(position), y = gabi::load<f32>(position + 4), z = gabi::load<f32>(position + 8);
        u32 out = vertices + k * 0x20;
        gabi::store<f32>(out, x); gabi::store<f32>(out + 4, y); gabi::store<f32>(out + 8, z);
        u32 normal = p + 0x4C8 + gabi::load<u8>(indices + 1) * 12;
        x = gabi::load<f32>(normal); y = gabi::load<f32>(normal + 4); z = gabi::load<f32>(normal + 8);
        gabi::store<f32>(out + 0xC, x); gabi::store<f32>(out + 0x10, y); gabi::store<f32>(out + 0x14, z);
        u32 uv = 0x101B9AE0 + gabi::load<u8>(indices + 2) * 8;
        x = gabi::load<f32>(uv); y = gabi::load<f32>(uv + 4);
        gabi::store<f32>(out + 0x18, x); gabi::store<f32>(out + 0x1C, y);
    }
    active = gabi::load<u32>(db + 0x4A8);
    u32 buffer = db + active * 0x254;
    u32 dirty = gabi::load<u32>(buffer + 0x150);
    gabi::call(0x027B5E94, buffer + 4, 0, dirty);
    active = gabi::load<u32>(db + 0x4A8);
    gabi::store<u32>(db + 0x4A8, active == 0);
    gabi::call(0x0255F8F4, gabi::load<u32>(p + 0xF8));
    gabi::call(0x0255FE90, gabi::load<u32>(p + 0xF8));
    gabi::Local<Mtx34> camera;
    gabi::Local<be<f32>[4]> color, scaledA, scaledB;
    gabi::Local<be<u32>[4]> linkage;
    mant_mtx_copy(camera.get(), gabi::at<void>(0x104B45F8));
    u32 view = gabi::load<u32>(0x104B4708);
    gabi::call(0x027FDA54, p + 0xD7C, 0, camera.get(), 0x104B470C, view + 0x240);
    for (u32 k = 0; k < 2; ++k) {
        u32 tev = gabi::load<u32>(p + 0xF8);
        u32 uniform = gabi::load<u32>(p + 0xD80);
        mant_color_f(color.get(), gabi::at<void>(tev + (k ? 0x160 : 0x90)));
        tev = gabi::load<u32>(p + 0xF8);
        f32 factor = gabi::load<f32>(tev + (k ? 0x16C : 0x28));
        u32 scaled = gabi::ea(k ? scaledB.get() : scaledA.get());
        gabi::call(0x0274D458, scaled, color.get(), factor);
        for (u32 j = 0; j < 4; ++j)
            gabi::store<u32>(uniform + 0x1C4 + k * 0x10 + j * 4, gabi::load<u32>(scaled + j * 4));
    }
    gabi::call(0x027FE010, p + 0xD7C);
    if (gabi::load<u8>(p + 0x895) == 1) {
        for (u32 off = 0x16FC; off <= 0x1718; off += 4) gabi::store<f32>(p + off, 1.0f);
        u32 tev = gabi::load<u32>(p + 0xF8);
        gabi::call(0x0274D2AC, p + 0x170C, gabi::load<f32>(tev + 0x24));
        gabi::call(0x027FB678, p + 0x1648);
        gabi::store<u32>(p + 0xF7C, 0);
        mant_copy_packet_matrix(p + 0x98, p + 0xF4C);
        gabi::call(0x027FB678, p + 0xED8);
    } else {
        u32 tev = gabi::load<u32>(p + 0xF8);
        f32 rgba[4];
        for (u32 j = 0; j < 4; ++j) rgba[j] = (f32)gabi::load<s16>(tev + 0x90 + j * 2) / 255.0f;
        for (u32 j = 0; j < 4; ++j) gabi::store<f32>(p + 0x1034 + j * 4, rgba[j]);
        for (u32 j = 0; j < 4; ++j) rgba[j] = (f32)gabi::load<u8>(tev + 0x98 + j) / 255.0f;
        for (u32 j = 0; j < 4; ++j) gabi::store<f32>(p + 0x1044 + j * 4, rgba[j]);
        gabi::call(0x0274D2AC, p + 0x1044, gabi::load<f32>(tev + 0x24));
        u8 alpha = gabi::load<u8>(tev + 0x9F);
        if (alpha) {
            for (u32 j = 0; j < 3; ++j) rgba[j] = (f32)gabi::load<u8>(tev + 0x9C + j) / 255.0f;
            rgba[3] = (f32)alpha / 255.0f;
        } else for (u32 j = 0; j < 4; ++j) rgba[j] = 0.0f;
        for (u32 j = 0; j < 4; ++j) gabi::store<f32>(p + 0x1054 + j * 4, rgba[j]);
        tev = gabi::load<u32>(p + 0xF8);
        gabi::call(0x0274D2AC, p + 0x1044, gabi::load<f32>(tev + 0x24));
        gabi::call(0x027FB678, p + 0xF80);
        gabi::store<u32>(p + 0xE2C, 0);
        mant_copy_packet_matrix(p + 0x98, p + 0xDFC);
        gabi::call(0x027FB678, p + 0xD88);
        f32 gamma = gabi::call<f32>(0x028F4560, gabi::load<f32>(0x100145FC), 2.2f);
        for (u32 j = 0; j < 3; ++j) gabi::store<f32>(p + 0x1398 + j * 4, gamma);
        gabi::store<f32>(p + 0x13A4, 1.0f);
        for (u32 j = 0; j < 3; ++j) gabi::store<f32>(p + 0x13A8 + j * 4, 0.0f);
        gabi::store<f32>(p + 0x13B4, 1.0f);
        tev = gabi::load<u32>(p + 0xF8);
        gabi::call(0x0274D2AC, p + 0x13A8, gabi::load<f32>(tev + 0x24));
        gabi::call(0x027FB678, p + 0x12E4);
        gabi::store<u32>(p + 0xED4, 0);
        mant_copy_packet_matrix(p + 0xC8, p + 0xEA4);
        gabi::call(0x027FB678, p + 0xE30);
    }
}
VERIFY(0x021BBC34, daMant_packet_c_draw);

/* 021BC2C8 */
static BOOL daMant_Draw(mant_class* i_this) {
    WWHD_FUNC(0x021BC2C8, BOOL, i_this);
    if (i_this->scale.y > 0.01f) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
        MtxTrans(0.0f, 0.0f, 0.0f, 0);
        PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(packet(i_this) + 0x98)); /* mpacket.getMtx() */
        if (i_this->mType != Type_PHANTOM_GANON_e) {
            MtxTrans(0.0f, -3.0f, 0.0f, 0);
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(packet(i_this) + 0xC8)); /* getMtx2() */
        }
        gabi::store<u32>(packet(i_this) + 0xF8, gabi::ea(&i_this->tevStr)); /* setTevStr */
        J3DDrawBuffer_entryImm(j3dSys_getDrawBuffer0(), gabi::at<u8>(packet(i_this)), 0);
        gabi::call(0x021BBC34, packet(i_this)); /* HD: daMant_packet_c::draw() called directly */
    }
    return TRUE;
}
VERIFY(0x021BC2C8, daMant_Draw);

static inline f32 mant_sin(s32 angle) {
    return gabi::load<f32>(0x104A44F8 + ((u16)angle >> 3) * 8);
}
static inline void mant_copy_vec(u32 dst, u32 src) {
    for (u32 k = 0; k < 3; ++k) gabi::store<u32>(dst + k * 4, gabi::load<u32>(src + k * 4));
}
static inline void mant_scale_vec(u32 src, u32 dst, f32 scale) {
    gabi::call(0x0201AE48, src, dst, scale);
}
static inline void mant_ground_finish(u32 ground) {
    gabi::store<u32>(ground + 0x40, 0x100145CC);
    gabi::store<u32>(ground + 0x20, 0x100145AC);
    gabi::store<u32>(ground + 0x4C, 0x1001458C);
    gabi::call(0x02008DAC, ground, 0);
}

/* 021BC3C4: nine cape strips, their collision samples and generated mesh normals. */
static BOOL daMant_Execute(mant_class* self) {
    WWHD_FUNC(0x021BC3C4, BOOL, self);
    u32 p = gabi::ea(self), play = 0;
    if (!(gabi::load<f32>(p + 0x334) > 0.01f)) return TRUE;
    gabi::call(0x025200D4);
    gabi::store<u32>(p + 0x2C94, gabi::load<u32>(p + 0x2C94) + 1);
    s16 hitDelay = gabi::load<s16>(p + 0x4282);
    if (hitDelay) gabi::store<s16>(p + 0x4282, hitDelay - 1);
    gabi::store<u32>(0x10464F1C, 0);
    gabi::store<u32>(0x10464F18, p + 0x4B0);
    gabi::store<u32>(0x10464F20, 0);
    gabi::Local<csXyz> angle, particleAngle;
    gabi::Local<cXyz> delta, bend, offset, wind, turbulence, waveInput, waveOutput;
    gabi::Local<cXyz> scaled, forceA, forceB, forceC, tangent, normal;
    gabi::Local<be<u32>[21]> ground;
    gabi::Local<be<u32>[7]> attack;
    gabi::Local<be<u32>[4]> linkage;
    u32 a = gabi::ea(angle.get()), g = gabi::ea(ground.get());
    gabi::call(0x0201A478, angle.get(), 0, 0, 0);
    gabi::call(0x0201ADE0, p + 0x362C, delta.get(), p + 0x3638);
    s32 heading = gabi::call<s32>(0x020195B0, delta->x.get(), delta->z.get());
    gabi::store<s16>(a + 2, heading + 0x4000);
    for (u32 row = 0; row < 9; ++row) {
        u32 strip = p + 0x2C9C + row * 0x110;
        f32 factor = (f32)row;
        f32 x = gabi::fmadds(delta->x.get() * 0.125f, factor, gabi::load<f32>(p + 0x3638));
        gabi::store<f32>(strip, x);
        f32 y = gabi::fmadds(delta->y.get() * 0.125f, factor, gabi::load<f32>(p + 0x363C));
        gabi::store<f32>(strip + 4, y);
        f32 z = gabi::fmadds(delta->z.get() * 0.125f, factor, gabi::load<f32>(p + 0x3640));
        gabi::store<f32>(strip + 8, z);
        gabi::call(0x025F1884, gabi::load<u32>(0x1018C7B0), (s32)gabi::load<s16>(a + 2));
        s32 phase = gabi::call<s32>(0x02019510, factor * gabi::load<f32>(0x10014624));
        f32 curve = mant_sin(phase);
        bend->x = 0.0f; bend->y = curve * -10.0f; bend->z = curve * -20.0f;
        gabi::call(0x0200FCD8, bend.get(), scaled.get());
        gabi::call(0x028E8D88, strip, scaled.get(), strip);
        gabi::store<s16>(strip + 0xD8, gabi::load<s16>(a));
        gabi::store<s16>(strip + 0xDA, gabi::load<s16>(a + 2) - 12000 + row * 3000);
        gabi::store<s16>(strip + 0xDC, gabi::load<s16>(a + 4));
        gabi::call(0x02008E0C, ground.get());
        gabi::store<u32>(g + 0x50, 1);
        gabi::store<u32>(g + 0x40, 0x100145CC);
        for (u32 j = 4; j <= 0xA; ++j) gabi::store<u8>(g + 0x40 + j, 0);
        gabi::store<u32>(g + 0x20, 0x100145AC);
        gabi::store<u32>(g + 0x10, 0x1001459C);
        gabi::store<u32>(g + 0x4C, 0x100145BC);
        gabi::store<u32>(g + 4, g + 0x4C);
        gabi::store<u32>(g, g + 0x40);
        gabi::store<f32>(g + 0x24, gabi::load<f32>(strip));
        gabi::store<f32>(g + 0x28, gabi::load<f32>(strip + 4) + 50.0f);
        gabi::store<f32>(g + 0x2C, gabi::load<f32>(strip + 8));
        play = gabi::call<u32>(0x025200D4);
        f32 floor = gabi::call<f32>(0x02008974, play + 0x12A0, ground.get()) + 1.5f;
        f32 anchorY = gabi::load<f32>(strip + 4);
        if (floor - anchorY > 50.0f) floor = anchorY;
        waveInput->x = 0.0f; waveInput->y = 0.0f; waveInput->z = 0.0f;
        waveOutput->x = 0.0f; waveOutput->y = 0.0f; waveOutput->z = 0.0f;
        f32 turbulenceX = 0.0f, turbulenceZ = 0.0f, windX = 0.0f, windZ = 0.0f, lift = 0.0f;

        gabi::call(0x025F1884, gabi::load<u32>(0x1018C7B0), (s32)gabi::load<s16>(strip + 0xDA));
        offset->x = 0.0f; offset->y = 0.0f; offset->z = -5.0f;
        gabi::call(0x0200FCD8, offset.get(), wind.get());
        s16 windHeading = gabi::load<s16>(p + 0x3658) - 12000 + row * 3000;
        gabi::call(0x025F1884, gabi::load<u32>(0x1018C7B0), (s32)windHeading);
        offset->x = 0.0f; offset->y = 0.0f; offset->z = gabi::load<f32>(p + 0x364C);
        s16 difference = windHeading - gabi::load<s16>(strip + 0xDA);
        if (difference < 0) difference = (s16)-difference;
        f32 windStrength;
        if ((u16)difference < 0x4000) { offset->z = offset->z.get() * 0.05f; windStrength = 0.0f; }
        else windStrength = 1.0f;
        gabi::call(0x0200FCD8, offset.get(), turbulence.get());
        offset->x = 0.0f; offset->y = 0.0f;
        offset->z = (gabi::load<f32>(p + 0x3644) + mant_sin(row * 23000)) * gabi::load<f32>(p + 0x334);
        bool severed = false;
        for (u32 node = 0; node < 9; ++node) {
            u32 point = strip + node * 12, velocity = point + 0x6C;
            if (node) {
                mant_scale_vec(gabi::ea(wind.get()), gabi::ea(scaled.get()), gabi::load<f32>(0x101B9A9C + node * 4));
                f32 forceX = scaled->x.get(), forceZ = scaled->z.get();
                f32 amplitude = gabi::load<f32>(p + 0x364C);
                f32 gust = gabi::load<f32>(p + 0x3650);
                if (__builtin_fabsf(amplitude) > 0.1f) {
                    u32 tick = gabi::load<u32>(p + 0x2C94);
                    lift = (mant_sin((tick << 12) - node * 10000 + row * 10000) * (amplitude * 0.5f)) * gabi::fmadds((f32)node, 0.1f, 1.0f);
                    windZ = turbulence->z.get(); windX = turbulence->x.get();
                }
                if (gust > 0.01f) {
                    mant_scale_vec(gabi::ea(wind.get()), gabi::ea(forceA.get()), gabi::load<f32>(0x101B9ABC + node * 4));
                    mant_scale_vec(gabi::ea(forceA.get()), gabi::ea(forceB.get()), gabi::load<f32>(p + 0x3650));
                    mant_scale_vec(gabi::ea(forceB.get()), gabi::ea(forceC.get()), windStrength);
                    turbulenceZ = forceC->z.get(); turbulenceX = forceC->x.get();
                }
                if (gabi::load<u8>(p + 0x2C90) == 1) {
                    u32 tick = gabi::load<u32>(p + 0x2C94);
                    waveInput->z = 2.0f * mant_sin((tick << 11) - node * 10000 + row * 10000);
                    gabi::call(0x025F1884, gabi::load<u32>(0x1018C7B0), (s32)gabi::load<s16>(strip + 0xDA));
                    gabi::call(0x0200FCD8, waveInput.get(), waveOutput.get());
                }
                f32 dx = gabi::load<f32>(point) - gabi::load<f32>(point - 12);
                f32 dz = gabi::load<f32>(point + 8) - gabi::load<f32>(point - 4);
                f32 dy = gabi::load<f32>(point + 4) + gabi::load<f32>(velocity + 4);
                dz = dz + gabi::load<f32>(velocity + 8);
                dx = dx + gabi::load<f32>(velocity);
                dz = dz + forceZ; dx = dx + forceX;
                dy = dy + gabi::load<f32>(p + 0x3654);
                dz = dz + windZ; dx = dx + windX; dy = dy + lift;
                dz = dz + turbulenceZ; dx = dx + turbulenceX;
                dz = dz + waveOutput->z.get(); dx = dx + waveOutput->x.get();
                if (dy < floor) dy = floor;
                f32 height = dy - gabi::load<f32>(point - 8);
                s16 pitch = (s16)-gabi::call<s32>(0x020195B0, height, dz);
                f32 distance = gabi::call<f32>(0x028F4384, gabi::fmadds(height, height, dz * dz));
                s32 yaw = gabi::call<s32>(0x020195B0, dx, distance);
                gabi::call(0x025F18EC, gabi::load<u32>(0x1018C7B0), (s32)pitch);
                gabi::call(0x025F1C28, gabi::load<u32>(0x1018C7B0), yaw);
                gabi::call(0x0200FCD8, offset.get(), normal.get());
                mant_copy_vec(velocity, point);
                x = gabi::load<f32>(point - 12) + normal->x.get();
                y = gabi::load<f32>(point - 8) + normal->y.get();
                z = gabi::load<f32>(point - 4) + normal->z.get();
                gabi::store<f32>(point, x); gabi::store<f32>(point + 4, y); gabi::store<f32>(point + 8, z);
                for (u32 j = 0; j < 3; ++j) {
                    f32 previous = gabi::load<f32>(velocity + j * 4);
                    f32 next = gabi::load<f32>(point + j * 4);
                    gabi::store<f32>(velocity + j * 4, (next - previous) * gabi::load<f32>(p + 0x3648));
                }
            }
            u32 output = gabi::load<u32>(0x10464F18);
            mant_copy_vec(output, point);
            output = gabi::load<u32>(0x10464F18);
            u32 count = gabi::load<u32>(0x10464F1C);
            gabi::store<u32>(0x10464F18, output + 12);
            gabi::store<u32>(0x10464F1C, count + 1);
            if (gabi::load<u8>(p + 0x2C90) == 0 && ((node | row) & 1) == 0 && node != 0 && node != 8 && row != 0 && row != 8) {
                u32 collision = gabi::load<u32>(0x10464F20);
                u32 spheres = p + 0x37C4;
                u32 sphere = spheres + collision * 0x12C;
                bool visible = (s8)gabi::load<u8>(p + 0x4280) != 0;
                gabi::call(0x02018C8C, sphere + 0x118, visible ? 30.0f : -200.0f);
                collision = gabi::load<u32>(0x10464F20);
                sphere = spheres + collision * 0x12C;
                gabi::call(0x02018D40, sphere + 0x118, visible ? point : 0x10464F40);
                play = gabi::call<u32>(0x025200D4);
                collision = gabi::load<u32>(0x10464F20);
                gabi::call(0x0200E240, play + 0x26A4, spheres + collision * 0x12C);
                if (!gabi::load<s16>(p + 0x4282) && !gabi::load<s16>(p + 0x4284)) {
                    collision = gabi::load<u32>(0x10464F20);
                    if (gabi::call<s32>(0x025162A4, spheres + collision * 0x12C)) {
                        gabi::store<s16>(p + 0x4282, 10);
                        collision = gabi::load<u32>(0x10464F20);
                        u32 object = gabi::call<u32>(0x02516300, spheres + collision * 0x12C);
                        u32 hit = gabi::ea(attack.get());
                        gabi::store<u32>(hit, object);
                        gabi::call(0x02518DB0, hit);
                        play = gabi::call<u32>(0x025200D4);
                        u8 hitType = gabi::load<u8>(hit + 0xA);
                        u32 player = gabi::load<u32>(play + 0x5B2C);
                        if ((hitType == 1 || hitType == 2) && !gabi::load<u8>(p + 0xC48)) {
                            u32 rotation = gabi::ea(particleAngle.get());
                            gabi::store<s16>(rotation, gabi::load<s16>(p + 0x320));
                            gabi::store<s16>(rotation + 2, gabi::load<s16>(p + 0x322) - 0x8000);
                            s32 room = (s8)gabi::load<u8>(p + 0x326);
                            gabi::store<u8>(p + 0x4280, 0);
                            gabi::store<u8>(p + 0xC48, hitType == 2 ? 2 : 4);
                            gabi::store<s16>(rotation + 4, gabi::load<s16>(p + 0x324));
                            play = gabi::call<u32>(0x025200D4);
                            u32 particles = gabi::load<u32>(play + 0x5AB0);
                            gabi::call(0x025A847C, particles, 0, 0x81B6, point, particleAngle.get(), 0, 0xFF, 0, room, p + 0x1A8, p + 0x1A8, 0);
                            if (p && p + 0x314) {
                                s32 reverb = gabi::call<s32>(0x02520540, (s32)(s8)gabi::load<u8>(p + 0x326));
                                gabi::call(0x025E1A40, 0x58ED, p + 0x314, 0, reverb);
                            }
                            hitType = gabi::load<u8>(hit + 0xA);
                        }
                        gabi::store<s16>(p + 0x365A, 2);
                        s16 direction = gabi::load<s16>(player + 0x32A);
                        gabi::store<f32>(p + 0x364C, 10.0f);
                        gabi::store<s16>(p + 0x3658, direction);
                        if (hitType == 5) {
                            gabi::store<s16>(p + 0x365A, 6);
                            gabi::store<s16>(p + 0x4284, 26);
                            severed = true;
                            break;
                        }
                    }
                }
                u32 collisionNext = gabi::load<u32>(0x10464F20);
                gabi::store<u32>(0x10464F20, collisionNext + 1);
                if (node == 4 && row == 4) {
                    gabi::call(0x02018C8C, p + 0x37B0, 200.0f);
                    gabi::call(0x02018D40, p + 0x37B0, point);
                    play = gabi::call<u32>(0x025200D4);
                    gabi::call(0x0200E240, play + 0x26A4, p + 0x3698);
                }
            }
        }
        mant_ground_finish(g);
        (void)severed;
    }
    offset->x = 0.0f; offset->y = 0.0f; offset->z = 1.0f;
    gabi::call(0x025F1884, gabi::load<u32>(0x1018C7B0), 0x4FA0);
    gabi::call(0x0200FCD8, offset.get(), waveInput.get());
    for (u32 k = 0; k < 81; ++k) {
        u32 out = p + 0x87C + k * 12;
        if (k < 72 && k % 9 == 8) mant_copy_vec(out, out - 12);
        else if (k >= 72) mant_copy_vec(out, out - 0x6C);
        else {
            u32 point = p + 0x4B0 + k * 12;
            gabi::call(0x0201ADE0, point + 0x78, tangent.get(), point);
            f32 dx = tangent->x.get(), dz = tangent->z.get();
            f32 dy = tangent->y.get();
            u32 matrix = gabi::load<u32>(0x1018C7B0);
            s32 yaw = gabi::call<s32>(0x020195B0, dx, dz);
            gabi::call(0x025F1884, matrix, yaw);
            matrix = gabi::load<u32>(0x1018C7B0);
            f32 length = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
            s16 pitch = (s16)-gabi::call<s32>(0x020195B0, dy, length);
            gabi::call(0x025F1BF4, matrix, (s32)pitch);
            gabi::call(0x0200FCD8, waveInput.get(), out);
        }
    }
    gabi::call(0xC00088B8, p + 0x4B0, 0x3CC);
    s16 burn = gabi::load<s16>(p + 0x4284);
    if (burn) {
        gabi::store<s16>(p + 0x4284, burn - 1);
        for (u32 k = 0; k < 81; k += 4) {
            play = gabi::call<u32>(0x025200D4);
            u32 particles = gabi::load<u32>(play + 0x5AB0);
            gabi::call(0x025A8D40, particles, 0x8069, p + 0x4B0 + k * 12, 0xFF, 0x101D5E98, 0x101D5E98, 0);
        }
        if (gabi::load<s16>(p + 0x4284) <= 2) {
            gabi::call(0x025D57E0, self);
            goto finish_matrix;
        }
    }
    gabi::call(0x0200ED84, p + 0x3644, 30.0f, 0.1f, 1.0f);
    gabi::call(0x0200ED84, p + 0x3648, 0.7f, 0.1f, 0.05f);
    if (gabi::call<s32>(0x025162A4, p + 0x3698)) {
        gabi::store<s16>(p + 0x365A, gabi::load<s16>(0x1047BE70) + 5);
        u32 play = gabi::call<u32>(0x025200D4);
        gabi::store<s16>(p + 0x3658, gabi::load<s16>(gabi::load<u32>(play + 0x5B2C) + 0x32A));
    }
    {
        s16 windTimer = gabi::load<s16>(p + 0x365A);
        if (windTimer) {
            gabi::store<s16>(p + 0x365A, windTimer - 1);
            f32 target = gabi::load<f32>(0x1047BDF4) + 50.0f;
            f32 step = gabi::load<f32>(0x1047BDF8) + 10.0f;
            gabi::call(0x0200ED84, p + 0x364C, target, 0.2f, step);
        } else {
            f32 step = gabi::load<f32>(0x1047BDFC) + 2.0f;
            gabi::call(0x0200EDC8, p + 0x364C, 0.1f, step);
        }
        if (gabi::load<s16>(p + 0x365A) <= 4)
            gabi::call(0x0200EDC8, p + 0x3650, 0.1f, 0.1f);
        else gabi::call(0x0200ED84, p + 0x3650, 1.0f, 1.0f, 1.0f);
    }
finish_matrix:
    f32 posX = gabi::load<f32>(p + 0x314), posZ = gabi::load<f32>(p + 0x31C), posY = gabi::load<f32>(p + 0x318);
    MtxTrans(posX, posY, posZ, 0);
    PSMTXCopy(gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0)), gabi::at<Mtx34>(p + 0x4250));
    return TRUE;
}
VERIFY(0x021BC3C4, daMant_Execute);

/* 021BD490 */
static BOOL daMant_IsDelete(mant_class*) {
    WWHD_FUNC(0x021BD490, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021BD490, daMant_IsDelete);

/* 021BD498 */
static BOOL daMant_Delete(mant_class*) {
    WWHD_FUNC(0x021BD498, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021BD498, daMant_Delete);

/* 021BE134 */
static cPhs_State daMant_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x021BE134, cPhs_State, a_this);
    mant_class* i_this = (mant_class*)a_this;

    /* fopAcM_ct(a_this, mant_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = MANT_VTBL;
            gabi::call(0x021BD4A0, packet(i_this)); /* daMant_packet_c::daMant_packet_c */
            dCcD_Stts_ct(&i_this->mStts);
            gabi::call(0x025166F0, &i_this->mWindSph); /* dCcD_Sph::dCcD_Sph */
            __construct_array(gabi::ea(&i_this->mMeshSph[0]), 9, 0x12C, 0x025166F0);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    i_this->mType = (u8)fopAcM_GetParam(a_this);
    fopAcM_setCullSizeBoxMin(a_this, -2000.0f, -2000.0f, -2000.0f);
    fopAcM_setCullSizeBoxMax(a_this, 2000.0f, 2000.0f, 2000.0f);
    u8 type = i_this->mType;
    a_this->cullMtx = gabi::ea(&i_this->mMtx);
    gabi::store<u8>(packet(i_this) + 0x895, type); /* mpacket.setType */

    if (type == Type_PHANTOM_GANON_e) {
        u8 t = i_this->mType;
        gabi::store<u8>(packet(i_this) + 0x894, 6); /* mpacket.setarg0 */
        i_this->m3644 = 40.0f;
        i_this->m3648 = 0.8f;
        i_this->m3654 = -5.0f;
        if (t != Type_PHANTOM_GANON_e)
            goto run;
    } else {
        i_this->m3648 = 0.7f;
        i_this->m3654 = -10.0f;
        i_this->m3644 = 30.0f;
        i_this->mStts.Init(200, 0xFF, a_this);
        i_this->mWindSph.Set(wind_cc_sph_src);
        i_this->mWindSph.SetStts(&i_this->mStts);
        for (int i = 0; i < 9; i++) {
            i_this->mMeshSph[i].Set(mesh_cc_sph_src);
            i_this->mMeshSph[i].SetStts(&i_this->mStts);
        }
        u8 t = i_this->mType;
        i_this->m4280 = 10;
        if (t != Type_PHANTOM_GANON_e)
            goto run;
    }
    for (int i = 0; i < 10; i++) {
        gabi::call(0x021BC3C4, i_this); /* daMant_Execute */
    }
    i_this->scale.z = 1.0f;
    i_this->scale.y = 0.0f;
    i_this->scale.x = 1.0f;
run:
    for (int i = 0; i < 10; i++) {
        gabi::call(0x021BC3C4, i_this); /* daMant_Execute */
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x021BE134, daMant_Create);

/* Bind the three shader-stage locations captured before any GX2 call. */
static inline void mant_bind_uniform_buffer(u32 entry, u32 shader) {
    u32 binding = gabi::load<u32>(shader + 0xC) ? gabi::load<u32>(shader + 0x10) : 0;
    s32 vertex = gabi::load<s16>(binding + 0xC);
    u32 size = gabi::load<u32>(entry + 4);
    u32 data = gabi::load<u32>(entry + 0xC);
    s32 pixel = gabi::load<s16>(binding + 0xE);
    s32 geometry = gabi::load<s16>(binding + 0x10);
    if (pixel != -1) gabi::call(0xC0006900, pixel, data, size);
    if (vertex != -1) gabi::call(0xC0006A38, vertex, data, size);
    if (geometry != -1) gabi::call(0xC00068A8, geometry, data, size);
}

/* 021BE340: shader activation, per-pass uniform blocks and indexed cape drawing. */
static void daMant_packet_c_setupUniforms(void* self, void* pass, void* program, u32 type) {
    WWHD_FUNC(0x021BE340, void, self, pass, program, type);
    u32 p = gabi::ea(self), q = gabi::ea(pass), shader = gabi::ea(program);
    u32 cache = gabi::call<u32>(0x027F29D4, 0x104B45C0);
    u32 current = gabi::load<u32>(cache + 4);
    u32 selected = gabi::load<u32>(shader);
    if (selected != current) {
        u8 flags = gabi::load<u8>(selected);
        u32 mode = gabi::load<u32>(cache);
        if (flags & 2) {
            gabi::store<u8>(selected, flags & ~2);
            gabi::call(0x027BB9E0, selected, 0);
        }
        u32 nextMode = gabi::load<u32>(gabi::load<u32>(selected + 0x7C) + 0x28);
        if (mode != nextMode) gabi::call(0x027B9F68, nextMode);
        u32 displaySize = gabi::load<u32>(selected + 0xC);
        if (displaySize) {
            u32 display = gabi::load<u32>(selected + 4);
            gabi::call(0xC00060E0, display, displaySize);
            gabi::store<u32>(cache, nextMode);
            gabi::store<u32>(cache + 4, selected);
        } else {
            gabi::call(0x027BB7CC, selected);
            gabi::store<u32>(cache + 4, selected);
            gabi::store<u32>(cache, nextMode);
        }
    }
    s32 stage = gabi::load<s32>(q + 0xC);
    if (stage == 0) {
        u32 block = p + 0xD88 + type * 0xA8;
        u32 fn = gabi::load<u32>(gabi::load<u32>(block + 0xC) + 0x2C);
        gabi::call_ptr(fn, block, shader);
        u32 light = gabi::load<u32>(q + 0x14);
        if (light) {
            u32 buffer = gabi::load<u32>(light + 4);
            u32 slot = gabi::load<u32>(buffer + 0x4C);
            mant_bind_uniform_buffer(buffer + 0x10 + slot * 0x1C, shader);
        }
    } else if (stage == 1 || stage == 2) {
        u32 buffer = gabi::load<u32>(p + 0xD80);
        u32 slot = gabi::load<u32>(buffer + 0x4C);
        mant_bind_uniform_buffer(buffer + 0x10 + slot * 0x1C, shader);
        u32 block = p + 0xF80 + type * 0x364;
        u32 fn = gabi::load<u32>(gabi::load<u32>(block + 0xC) + 0x2C);
        gabi::call_ptr(fn, block, shader);
        block = p + 0xD88 + type * 0xA8;
        fn = gabi::load<u32>(gabi::load<u32>(block + 0xC) + 0x2C);
        gabi::call_ptr(fn, block, shader);
        if (stage == 2) {
            block = gabi::load<u32>(q + 0x30);
            if (block) {
                fn = gabi::load<u32>(gabi::load<u32>(block + 0xC) + 0x2C);
                gabi::call_ptr(fn, block, shader);
            }
            gabi::call(0x027FFE54, q, shader);
        }
    }
    u32 sampler = gabi::load<u32>(shader + 0x14);
    u8 texture = gabi::load<u8>(p + 0x894);
    sampler = sampler ? gabi::load<u32>(shader + 0x18) : 0;
    gabi::call(0x027BE53C, p + 0x19C4 + texture * 0x198, sampler + 4, 0, 0);
    gabi::Local<be<u32>[0x120 / 4]> state;
    gabi::Local<be<u32>[4]> linkage;
    u32 st = gabi::ea(state.get());
    gabi::call(0x02750250, st);
    u32 mask = gabi::load<u32>(st + 0xEC);
    gabi::store<u32>(st + 0xC, 2);
    gabi::store<u32>(st + 0xE4, 4);
    gabi::store<u8>(st + 0xE0, 1);
    gabi::store<f32>(st + 0xE8, 0.5f);
    gabi::store<u32>(st + 0xEC, (((mask & 0xFFFFFFF0) + 7) & 0xFFFFFF0F) + 0x10);
    u32 passType = gabi::load<u32>(q + 0xC);
    gabi::store<u32>(st + 8, type == 2 ? 2 : (type != 0));
    gabi::call(0x0280037C, passType, st);
    gabi::call(0x02750370, st);
    passType = gabi::load<u32>(q + 0xC);
    u32 base = p + type * 0xC;
    u32 count = gabi::load<u32>(base + 0x89C);
    u32 index = gabi::load<u32>(p + 0xD64);
    u32 entries = gabi::load<u32>(base + 0x8A0);
    if (passType < count) entries += passType * 0x14;
    gabi::call(0x027BFE5C, gabi::load<u32>(entries + 8 + (index == 0 ? 8 : 0)));
    u32 indexCount = gabi::load<u32>(p + 0x19B8);
    if (indexCount) {
        u32 primitive = gabi::load<u32>(p + 0x19AC);
        u32 indices = gabi::load<u32>(p + 0x19B4);
        u32 format = gabi::load<u32>(p + 0x19B0);
        gabi::call(0xC0006178, format, indexCount, primitive, indices, 0, 1);
    }
}
VERIFY(0x021BE340, daMant_packet_c_setupUniforms);

static inline void mant_freeObjBuffer(u32 b);

static inline u32 mant_heap_alloc(u32 size, u32 align) {
    u32 heap = gabi::call<u32>(0x02756140, gabi::load<u32>(0x101F8B4C));
    u32 fn = gabi::load<u32>(gabi::load<u32>(heap + 0xC) + 0x34);
    return gabi::call_ptr<u32>(fn, heap, size, align);
}

static inline void mant_init_programs(u32 dst, u32 name) {
    gabi::Local<be<u32>[2]> key;
    gabi::store<u32>(gabi::ea(key.get()), name);
    gabi::store<u32>(gabi::ea(key.get()) + 4, SAFESTRING_VTBL);
    gabi::Local<be<u32>[4]> linkage;
    u32 manager = gabi::call<u32>(0x027FFCBC);
    u32 archive = gabi::load<u32>(manager + 4);
    s32 index = gabi::call<s32>(0x027B90AC, archive, key.get());
    u32 entry = 0;
    if (index >= 0) {
        u32 count = gabi::load<u32>(manager + 8);
        u32 base = gabi::load<u32>(manager + 0xC);
        entry = base + ((u32)index < count ? (u32)index * 0x24 : 0);
        if (!gabi::load<u8>(entry + 0x20)) {
            archive = gabi::load<u32>(manager + 4);
            u32 records = gabi::load<u32>(archive + 0x1C);
            u32 resource = (u32)index < records ? gabi::load<u32>(archive + 0x20) + (u32)index * 0x84 : 0;
            gabi::call(0x02800B0C, entry, resource, 0);
            count = gabi::load<u32>(manager + 8);
            base = gabi::load<u32>(manager + 0xC);
        }
        entry = base + ((u32)index < count ? (u32)index * 0x24 : 0);
    }
    gabi::call(0x0280068C, dst, entry, 0);
}

/* 021BD4A0: construct the GX2 packet and its double-buffered resources. */
static void* daMant_packet_c_ct(void* self) {
    WWHD_FUNC(0x021BD4A0, void*, self);
    u32 p = gabi::ea(self);
    if (!p) {
        p = gabi::ea(operator_new(0x28DC));
        if (!p) return nullptr;
    }
    gabi::call(0x027F1278, p);
    gabi::store<u32>(p + 0xC, MANT_PACKET_VTBL);
    __construct_array(p + 0x898, 3, 0xC, 0x021BEA40);
    u32 db = p + 0x8BC, storage = db;
    if (!storage) storage = gabi::ea(operator_new(0x4C0));
    if (storage) {
        __construct_array(storage, 2, 0x254, 0x021BEAB0);
        gabi::store<u32>(storage + 0x4A8, 0);
        gabi::store<u32>(storage + 0x4AC, 0);
        gabi::store<u32>(storage + 0x4B8, 0);
        gabi::store<u32>(storage + 0x4B0, 0x20);
        gabi::store<u8>(storage + 0x4BC, 0);
        gabi::store<u32>(storage, 0);
        gabi::store<u32>(storage + 0x254, 0);
    }
    gabi::call(0x027FD6F4, p + 0xD7C);
    __construct_array(p + 0xD88, 3, 0xA8, 0x021BEB0C);
    __construct_array(p + 0xF80, 3, 0x364, 0x021BEBA8);
    gabi::call(0x027B5430, p + 0x19AC);
    __construct_array(p + 0x19C4, 7, 0x198, 0x027BDF7C);
    __construct_array(p + 0x24EC, 7, 0x90, 0x027BE6B8);
    mant_init_programs(p + 0x898, 0x10014668);
    mant_init_programs(p + 0x8A4, 0x1001465C);
    mant_init_programs(p + 0x8B0, 0x10014650);
    gabi::store<u32>(db + 0x4AC, 0x13);
    gabi::store<u32>(db + 0x4B4, 0x10014708);
    for (u32 k = 0; k < 2; ++k) {
        u32 buffer = db + k * 0x254;
        if (!gabi::load<u32>(buffer)) {
            u32 mem = mant_heap_alloc(0x1200, 0x40);
            if (mem) {
                gabi::store<u32>(buffer + 0x250, mem);
                gabi::store<u32>(buffer + 0x24C, 0x90);
            }
            gabi::store<u32>(buffer, gabi::load<u32>(buffer + 0x250));
        }
        gabi::call(0x027FF478, buffer + 4, gabi::load<u32>(buffer), 0x90, db + 0x4AC);
    }
    gabi::store<u32>(db + 0x4B8, 0);
    gabi::store<u8>(db + 0x4BC, 1);
    for (u32 set = 0; set < 3; ++set) {
        u32 programs = p + 0x898 + set * 0xC;
        for (u32 n = 0; n < gabi::load<u32>(programs); ++n) {
            u32 count = gabi::load<u32>(programs + 4);
            u32 entry = gabi::load<u32>(programs + 8);
            if (n < count) entry += n * 0x14;
            u32 shader = gabi::load<u32>(entry);
            gabi::store<u32>(entry, 0);
            mant_freeObjBuffer(entry + 4);
            mant_freeObjBuffer(entry + 0xC);
            gabi::store<u32>(entry, shader);
            for (u32 k = 0; k < 2; ++k) {
                u32 object = mant_heap_alloc(0xF4, 4);
                if (object) gabi::call(0x027BF734, object);
                if (object) {
                    gabi::store<u32>(entry + 8 + k * 8, object);
                    gabi::store<u32>(entry + 4 + k * 8, 1);
                }
            }
            for (u32 k = 0; k < 2; ++k) {
                u32 object = gabi::load<u32>(entry + 8 + k * 8);
                gabi::call(0x027FF530, shader, object, db + 4 + k * 0x254, db + 0x4AC, 0);
            }
        }
    }
    gabi::call(0x027FD838, p + 0xD7C, 1, 0);
    for (u32 k = 0; k < 3; ++k) gabi::call(0x027FB5D4, p + 0xF80 + k * 0x364, 0);
    for (u32 k = 0; k < 3; ++k) gabi::call(0x027FB5D4, p + 0xD88 + k * 0xA8, 0);
    u32 count = gabi::load<u32>(0x101B9A7C);
    gabi::call(0x027B54E0, p + 0x19AC, 0x101B9F18, 4, count);
    gabi::store<u32>(p + 0x19B0, 4);
    gabi::Local<be<u32>[2]> archiveName, fileName;
    gabi::store<u32>(gabi::ea(fileName.get()) + 4, SAFESTRING_VTBL);
    gabi::store<u32>(gabi::ea(fileName.get()), 0x10014688);
    u32 resources = gabi::load<u32>(0x101F4F7C);
    gabi::store<u32>(gabi::ea(archiveName.get()) + 4, SAFESTRING_VTBL);
    gabi::store<u32>(gabi::ea(archiveName.get()), 0x10014678);
    gabi::Local<be<u32>[4]> linkage;
    u32 textureArchive = gabi::call<u32>(0x026124B0, resources, archiveName.get(), fileName.get(), 0);
    gabi::call(0x0274FBF8, gabi::load<u32>(0x101F8B18));
    for (u32 k = 0; k < 7; ++k) {
        u32 name = 0x10464F4C + k * 8;
        u32 string = gabi::load<u32>(name);
        if (!gabi::load<u8>(string)) continue;
        u32 fn = gabi::load<u32>(gabi::load<u32>(name + 4) + 0x14);
        gabi::call_ptr(fn, name);
        u32 texture = p + 0x24EC + k * 0x90;
        gabi::call(0x02773870, texture, textureArchive, gabi::load<u32>(name));
        u32 sampler = p + 0x19C4 + k * 0x198;
        bool same = true;
        for (u32 off : {4u, 8u, 0xCu, 0x10u, 0x14u, 0x18u, 0x38u, 0x34u, 0x1Cu}) {
            if (gabi::load<u32>(sampler + off) != gabi::load<u32>(texture + off)) { same = false; break; }
        }
        if (!same) gabi::call(0x027BDEB4, sampler, texture);
        else {
            u32 image = gabi::load<u32>(texture + 0x28), mip = gabi::load<u32>(texture + 0x30);
            gabi::store<u32>(sampler + 0x28, image);
            gabi::store<u32>(sampler + 0xDC, mip);
            gabi::store<u32>(sampler + 0xD4, image);
            gabi::store<u32>(sampler + 0x30, mip);
        }
    }
    gabi::call(0x0274FCCC, gabi::load<u32>(0x101F8B18));
    return gabi::at<void>(p);
}
VERIFY(0x021BD4A0, daMant_packet_c_ct);

/* 021BE814: daMant_packet_c virtual (vtable slot): sets the uniforms of each shader program
 * of the current pass (021BE340) for passes < 4, then 02750370 on 0x104B474C */
static void daMant_packet_c_setUniforms(void* self, void* pass) {
    WWHD_FUNC(0x021BE814, void, self, pass);
    u32 p = gabi::ea(self);
    u32 q = gabi::ea(pass);
    s32 k = gabi::load<s32>(q + 0xC);
    if (k < 4) {
        if (gabi::load<u8>(p + 0x895) == 1) {
            u32 n = gabi::load<u32>(p + 0x8B4);
            u32 e = gabi::load<u32>(p + 0x8B8);
            if ((u32)k < n) e += k * 0x14;
            gabi::call(0x021BE340, self, pass, gabi::load<u32>(e), 2);
        } else {
            u32 n = gabi::load<u32>(p + 0x89C);
            u32 e = gabi::load<u32>(p + 0x8A0);
            if ((u32)k < n) e += k * 0x14;
            gabi::call(0x021BE340, self, pass, gabi::load<u32>(e), 0);
            k = gabi::load<s32>(q + 0xC);
            n = gabi::load<u32>(p + 0x8A8);
            e = gabi::load<u32>(p + 0x8AC);
            if ((u32)k < n) e += k * 0x14;
            gabi::call(0x021BE340, self, pass, gabi::load<u32>(e), 1);
        }
    }
    gabi::call(0x02750370, 0x104B474C);
}
VERIFY(0x021BE814, daMant_packet_c_setUniforms);

/* 021BE8F8: static initialisation: header statics, a cXyz (-20000, -200000, -100000) and the
 * seven shader-program names (sead::SafeString) */
static void __sinit_d_a_mant_cpp() {
    WWHD_FUNC(0x021BE8F8, void, (u32)0);
    sinit_header_statics(0x10464F24, 0x101BA298);
    static const u32 names[7] = {0x100146C8, 0x100146D4, 0x100146E0, 0x100146C4, 0x100146EC, 0x100146C4, 0x100146F8};
    for (int i = 0; i < 7; i++) {
        gabi::store<u32>(0x10464F4C + 8 * i + 4, SAFESTRING_VTBL);
        gabi::store<u32>(0x10464F4C + 8 * i, names[i]);
    }
    gabi::store<f32>(0x10464F40, -20000.0f);
    gabi::store<f32>(0x10464F44, -200000.0f);
    gabi::store<f32>(0x10464F48, -100000.0f);
}
VERIFY(0x021BE8F8, __sinit_d_a_mant_cpp);

/* 021BEA2C: sead::SafeString deleting destructor (this TU's vtable slot) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021BEA2C, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x021BEA2C, SafeString_dt);

/* 021BEA40: constructor of a 0xC-byte HD object {u32 n; {u32, u32} buffer} (allocates when NULL) */
static void* mant_buf12_ct(void* self) {
    WWHD_FUNC(0x021BEA40, void*, self);
    u32 p = gabi::ea(self);
    if (p == 0) {
        p = gabi::ea(operator_new(0xC));
        if (p == 0)
            return gabi::at<void>(p);
    }
    gabi::store<u32>(p, 0);
    u32 q = p + 4;
    if (q == 0) {
        q = gabi::ea(operator_new(8));
        if (q == 0)
            return gabi::at<void>(p);
    }
    gabi::store<u32>(q + 4, 0);
    gabi::store<u32>(q + 0, 0);
    return gabi::at<void>(p);
}
VERIFY(0x021BEA40, mant_buf12_ct);

/* 021BEAB0: constructor of a 0x254-byte HD vertex-buffer object (027B5BD8 at +4, 027BF734 at +0x158) */
static void* mant_vtxbuf_ct(void* self) {
    WWHD_FUNC(0x021BEAB0, void*, self);
    u32 p = gabi::ea(self);
    if (p == 0) {
        p = gabi::ea(operator_new(0x254));
        if (p == 0)
            return gabi::at<void>(p);
    }
    gabi::call(0x027B5BD8, p + 4);
    gabi::call(0x027BF734, p + 0x158);
    gabi::store<u32>(p + 0x250, 0);
    gabi::store<u32>(p + 0x24C, 0);
    return gabi::at<void>(p);
}
VERIFY(0x021BEAB0, mant_vtxbuf_ct);

/* 021BEB0C: constructor of a 0xA8-byte HD uniform block (base 027FB40C, vtable 0x1016EF84) */
static void* mant_ublock_a8_ct(void* self) {
    WWHD_FUNC(0x021BEB0C, void*, self);
    u32 p = gabi::ea(self);
    if (p == 0) {
        p = gabi::ea(operator_new(0xA8));
        if (p == 0)
            return gabi::at<void>(p);
    }
    gabi::call(0x027FB40C, p);
    gabi::store<u32>(p + 0xC, 0x1016EF84);
    gabi::call(0x028F521C, p + 0x74, 0x34);
    if (p + 0x74 == 0)
        operator_new(0x30);
    return gabi::at<void>(p);
}
VERIFY(0x021BEB0C, mant_ublock_a8_ct);

/* 021BEB7C: trivial constructor of a 0x10-byte object (allocates when NULL) */
static void* mant_obj16_ct(void* self) {
    WWHD_FUNC(0x021BEB7C, void*, self);
    if (self == nullptr)
        return operator_new(0x10);
    return self;
}
VERIFY(0x021BEB7C, mant_obj16_ct);

/* 021BEBA8: constructor of a 0x364-byte HD uniform block (base 027FB40C, vtable 0x1016EFB4):
 * zeroes the block, identity diagonals, three arrays of two 0x10-byte objects */
static void* mant_ublock_364_ct(void* self) {
    WWHD_FUNC(0x021BEBA8, void*, self);
    u32 p = gabi::ea(self);
    if (p == 0) {
        p = gabi::ea(operator_new(0x364));
        if (p == 0)
            return gabi::at<void>(p);
    }
    gabi::call(0x027FB40C, p);
    gabi::store<u32>(p + 0xC, 0x1016EFB4);
    gabi::call(0x028F521C, p + 0x74, 0x2F0);
    for (u32 off = 0x74; off <= 0x120; off += 4) {
        bool one = off >= 0x80 && ((off - 0x80) & 0xF) == 0;
        gabi::store<f32>(p + off, one ? 1.0f : 0.0f);
    }
    __construct_array(p + 0x124, 2, 0x10, 0x021BEB7C);
    __construct_array(p + 0x144, 2, 0x10, 0x021BEB7C);
    __construct_array(p + 0x164, 2, 0x10, 0x021BEB7C);
    /* GHS's null-checked member constructors */
    for (u32 off = 0x110; off <= 0x260; off += 0x30)
        if (p + 0x74 + off == 0) operator_new(0x30);
    for (u32 off = 0x290; off <= 0x2E0; off += 0x10)
        if (p + 0x74 + off == 0) operator_new(0x10);
    return gabi::at<void>(p);
}
VERIFY(0x021BEBA8, mant_ublock_364_ct);

/* 021BEE30 / 021BEE84: deleting destructors of the two uniform-block classes (base 027FB528) */
static void mant_ublock_364_dt(void* self, s32 flags) {
    WWHD_FUNC(0x021BEE30, void, self, flags);
    if (self != nullptr) {
        gabi::call(0x027FB528, self, 0);
        if (flags & 1)
            operator_delete(self);
    }
}
VERIFY(0x021BEE30, mant_ublock_364_dt);

static void mant_ublock_a8_dt(void* self, s32 flags) {
    WWHD_FUNC(0x021BEE84, void, self, flags);
    if (self != nullptr) {
        gabi::call(0x027FB528, self, 0);
        if (flags & 1)
            operator_delete(self);
    }
}
VERIFY(0x021BEE84, mant_ublock_a8_dt);

/* 021BEED8: deleting destructor of the 0x254-byte vertex-buffer object */
static void mant_vtxbuf_dt(void* self, s32 flags) {
    WWHD_FUNC(0x021BEED8, void, self, flags);
    if (self != nullptr) {
        u32 p = gabi::ea(self);
        gabi::call(0x027BF880, p + 0x158, 2);
        gabi::call(0x027B5CBC, p + 4, 2);
        if (flags & 1)
            operator_delete(self);
    }
}
VERIFY(0x021BEED8, mant_vtxbuf_dt);

/* free the 0xF4-byte objects of one sead::Buffer {s32 n; T* p} at b (virtual destructor at
 * vtable +0xC of each, vtable at +0xF0), then the buffer */
static inline void mant_freeObjBuffer(u32 b) {
    u32 arr = gabi::load<u32>(b + 4);
    if (arr != 0) {
        for (s32 j = 0; j < gabi::load<s32>(b + 0); j++) {
            u32 obj = gabi::load<u32>(b + 4) + j * 0xF4;
            gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(obj + 0xF0) + 0xC), obj, 2);
        }
        sead_free(b + 4);
        gabi::store<u32>(b + 0, 0);
        gabi::store<u32>(b + 4, 0);
    }
}

/* 021BEF38: deleting destructor of the 0xC-byte buffer object of 021BEA40: frees its
 * 0x14-byte entries (each with two object buffers), then the entry buffer */
static void mant_buf12_dt(void* self, s32 flags) {
    WWHD_FUNC(0x021BEF38, void, self, flags);
    u32 p = gabi::ea(self);
    if (p == 0)
        return;
    u32 arr = gabi::load<u32>(p + 8);
    if (arr != 0) {
        s32 n = gabi::load<s32>(p + 4);
        for (s32 i = 0; i < n; i++) {
            u32 e = arr + i * 0x14;
            if (e != 0) {
                u32 a = gabi::load<u32>(e + 8);
                gabi::store<u32>(e + 0, 0);
                if (a != 0) {
                    for (s32 j = 0; j < gabi::load<s32>(e + 4); j++) {
                        u32 obj = gabi::load<u32>(e + 8) + j * 0xF4;
                        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(obj + 0xF0) + 0xC), obj, 2);
                    }
                    sead_free(e + 8);
                    gabi::store<u32>(e + 4, 0);
                    gabi::store<u32>(e + 8, 0);
                }
                mant_freeObjBuffer(e + 0xC);
            }
            n = gabi::load<s32>(p + 4);
            arr = gabi::load<u32>(p + 8);
        }
        sead_free(p + 8);
        gabi::store<u32>(p + 4, 0);
        gabi::store<u32>(p + 8, 0);
    }
    if (flags & 1)
        operator_delete(self);
}
VERIFY(0x021BEF38, mant_buf12_dt);

/* the double-buffered vertex buffer D of the packet (+0x8BC): two 0x254-byte buffers, their
 * GPU memory at +0x250 / +0x4A4 (size +0x24C / +0x4A0) */
static inline void mant_dbuf_release(u32 D) {
    gabi::call(0x027BF7E8, D + 0x158);
    u32 a = gabi::load<u32>(D + 0x250);
    gabi::store<u32>(D + 0, 0);
    if (a != 0) {
        sead_free(D + 0x250);
        gabi::store<u32>(D + 0x24C, 0);
        gabi::store<u32>(D + 0x250, 0);
    }
    gabi::call(0x027BF7E8, D + 0x3AC);
    a = gabi::load<u32>(D + 0x4A4);
    gabi::store<u32>(D + 0x254, 0);
    if (a != 0) {
        sead_free(D + 0x4A4);
        gabi::store<u32>(D + 0x4A0, 0);
        gabi::store<u32>(D + 0x4A4, 0);
    }
    gabi::store<u32>(D + 0x4B8, 0);
}

/* daMant_packet_c::~daMant_packet_c body (inlined into both destructors) */
static inline void mant_packet_dt_body(u32 P) {
    gabi::store<u32>(P + 0xC, MANT_PACKET_VTBL);
    u32 D = P + 0x8BC;
    mant_dbuf_release(D);
    for (s32 i = 0; i < gabi::load<s32>(P + 0xD7C); i++) {
        u32 e = gabi::load<u32>(P + 0xD80);
        if ((u32)i < gabi::load<u32>(P + 0xD7C)) e += i * 0x23C;
        for (int k = 0; k < 2; k++) gabi::call(0x027BEBEC, e + 0x10 + k * 0x1C);
    }
    for (int a = 0; a < 3; a++)
        for (int k = 0; k < 2; k++) gabi::call(0x027BEBEC, P + 0xF90 + a * 0x364 + k * 0x1C);
    for (int a = 0; a < 3; a++)
        for (int k = 0; k < 2; k++) gabi::call(0x027BEBEC, P + 0xD98 + a * 0xA8 + k * 0x1C);
    __destroy_arr(P + 0x19C4, 7, 0x198, 0x027BE2B0);
    gabi::call(0x027B54A0, P + 0x19AC, 2);
    __destroy_arr(P + 0xF80, 3, 0x364, 0x021BEE30);
    __destroy_arr(P + 0xD88, 3, 0xA8, 0x021BEE84);
    gabi::call(0x027FD764, P + 0xD7C, 2);
    if (D != 0) {
        mant_dbuf_release(D);
        __destroy_arr(D, 2, 0x254, 0x021BEED8);
    }
    __destroy_arr(P + 0x898, 3, 0xC, 0x021BEF38);
    gabi::call(0x027F13DC, P, 0); /* J3DPacket::~J3DPacket */
}

/* 021BF0D4: daMant_packet_c deleting destructor */
static void daMant_packet_c_dt(void* self, s32 flags) {
    WWHD_FUNC(0x021BF0D4, void, self, flags);
    if (self != nullptr) {
        mant_packet_dt_body(gabi::ea(self));
        if (flags & 1)
            operator_delete(self);
    }
}
VERIFY(0x021BF0D4, daMant_packet_c_dt);

/* 021BF3C4: empty virtual of daMant_packet_c */
static void daMant_packet_c_empty(void*) {
    WWHD_FUNC(0x021BF3C4, void, (u32)0);
}
VERIFY(0x021BF3C4, daMant_packet_c_empty);

/* 021BF3C8: mant_class deleting destructor (compiler-generated, HD virtual destructor) */
static void mant_class_dt(mant_class* i_this, s32 flags) {
    WWHD_FUNC(0x021BF3C8, void, i_this, flags);
    if (i_this != nullptr) {
        __destroy_arr(gabi::ea(&i_this->mMeshSph[0]), 9, 0x12C, 0x02515AE8);
        gabi::call(0x02515AE8, &i_this->mWindSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mStts, 2);
        if (packet(i_this) != 0)
            mant_packet_dt_body(packet(i_this));
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021BF3C8, mant_class_dt);

/* 021BF708: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x021BF708, void, (u32)0);
}
VERIFY(0x021BF708, SafeString_assureTerminationImpl);
