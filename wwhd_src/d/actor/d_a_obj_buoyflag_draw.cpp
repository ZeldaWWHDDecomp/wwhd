/**
 * d_a_obj_buoyflag_draw.cpp (WWHD)
 * Object - Buoy flag: drawing. HD replaces the GameCube display-list drawing (draw_hata /
 * draw_hasi with GX calls) by GPU objects: per part (flag "hata", pole "hasi") a double-buffered
 * vertex buffer set, a shader/uniform state, and the "Cloth" texture with its sampler.
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx) and verified against it. The HD engine classes are not named yet: their
 * methods are bound by address with descriptive names, their members by raw offset.
 */
#include "d/actor/d_a_obj_buoyflag.h"

namespace daObjBuoyflag {

/* ---- local bindings (HD engine, unnamed) ---- */
static inline void vcall2C(u32 obj, u32 arg) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(obj + 0xC) + 0x2C), obj, arg); }
#define j3dSys_ 0x104B45C0u
#define J3DSys_viewMtx gabi::at<Mtx34>(0x104B45F8)

/* 02328F20: GXColor -> float colour (rgba / 255) */
static void color_u8_to_f(u32 dst, u32 src) {
    WWHD_FUNC(0x02328F20, void, dst, src);
    f32 r = (f32)gabi::load<u8>(src) / 255.0f;
    f32 g = (f32)gabi::load<u8>(src + 1) / 255.0f;
    f32 b = (f32)gabi::load<u8>(src + 2) / 255.0f;
    f32 a = (f32)gabi::load<u8>(src + 3) / 255.0f;
    gabi::store<u32>(dst, fbits_of(r));
    gabi::store<u32>(dst + 4, fbits_of(g));
    gabi::store<u32>(dst + 8, fbits_of(b));
    gabi::store<u32>(dst + 0xC, fbits_of(a));
}
VERIFY(0x02328F20, color_u8_to_f);

/* 0232B560: GXColorS10 -> float colour (rgba / 255) */
static void color_s16_to_f(u32 dst, u32 src) {
    WWHD_FUNC(0x0232B560, void, dst, src);
    f32 r = (f32)gabi::load<s16>(src) / 255.0f;
    f32 g = (f32)gabi::load<s16>(src + 2) / 255.0f;
    f32 b = (f32)gabi::load<s16>(src + 4) / 255.0f;
    f32 a = (f32)gabi::load<s16>(src + 6) / 255.0f;
    gabi::store<u32>(dst, fbits_of(r));
    gabi::store<u32>(dst + 4, fbits_of(g));
    gabi::store<u32>(dst + 8, fbits_of(b));
    gabi::store<u32>(dst + 0xC, fbits_of(a));
}
VERIFY(0x0232B560, color_s16_to_f);

/* 0232B318: Mtx -> sead::Matrix34 (through FPRs: all twelve loaded, then stored) */
static void mtx_to_sead(u32 dst, u32 src) {
    WWHD_FUNC(0x0232B318, void, dst, src);
    u32 v[12];
    for (int i = 0; i < 12; i++) v[i] = fbits(src + 4 * i);
    for (int i = 0; i < 12; i++) gabi::store<u32>(dst + 4 * i, v[i]);
}
VERIFY(0x0232B318, mtx_to_sead);

/* texture T bound into sampler/texture object S: copy the image fields when they match, else rebuild */
static inline void tex_bind(u32 S, u32 T) {
    if (gabi::load<u32>(S + 4) == gabi::load<u32>(T + 4) && gabi::load<u32>(S + 8) == gabi::load<u32>(T + 8) &&
        gabi::load<u32>(S + 0xC) == gabi::load<u32>(T + 0xC) && gabi::load<u32>(S + 0x10) == gabi::load<u32>(T + 0x10) &&
        gabi::load<u32>(S + 0x14) == gabi::load<u32>(T + 0x14) && gabi::load<u32>(S + 0x18) == gabi::load<u32>(T + 0x18) &&
        gabi::load<u32>(S + 0x38) == gabi::load<u32>(T + 0x38) && gabi::load<u32>(S + 0x34) == gabi::load<u32>(T + 0x34) &&
        gabi::load<u32>(S + 0x1C) == gabi::load<u32>(T + 0x1C)) {
        u32 a = gabi::load<u32>(T + 0x28);
        u32 b = gabi::load<u32>(T + 0x30);
        gabi::store<u32>(S + 0xD4, a);
        gabi::store<u32>(S + 0xDC, b);
        gabi::store<u32>(S + 0x30, b);
        gabi::store<u32>(S + 0x28, a);
    } else {
        gabi::call(0x027BDEB4, S, T);
    }
    gabi::store<u32>(S + 0x160, 2);
    gabi::store<u32>(S + 0x15C, 2);
    gabi::store<u32>(S + 0x164, 2);
    gabi::store<u8>(S + 0x190, (u8)(gabi::load<u8>(S + 0x190) | 2));
}

/* 0232B3B8 (HD): load the "Cloth" texture (index 3) once */
void Packet_c::load_texture() {
    WWHD_FUNC(0x0232B3B8, void, this);
    u32 P = gabi::ea(this);
    gabi::call(0x0274FBF8, gabi::load<u32>(0x101F8B18)); /* lock */
    if (gabi::load<u8>(P + 0x2C60) == 0) {
        void* tmp_img = dComIfG_getObjectRes(STR(0x10026658) /* "Cloth" */, 3, 0x10026548);
        if (tmp_img == nullptr)
            JUT_ASSERT_fail(STR(0x10026660), 0x7D6, STR(0x10026678)); /* tmp_img != (0) */
        gabi::call(0x02773798, P + 0x2A38, gabi::load<u32>(gabi::ea(tmp_img) + 0x20));
        tex_bind(P + 0x2AC8, P + 0x2A38);
        gabi::store<u8>(P + 0x2C60, 1);
    }
    gabi::call(0x0274FCCC, gabi::load<u32>(0x101F8B18)); /* unlock */
}
VERIFY(0x0232B3B8, &Packet_c::load_texture);

/* tevStr colours into the uniform block of a part: G = P + 0x16A4 (flag) / P + 0x23F4 (pole) */
static inline void set_tev_colors(Packet_c* pk, u32 G, u32 M, u32 C, u32 mtxSrc) {
    u32 P = gabi::ea(pk);
    gabi::call(0x0255F8F4, gabi::ea(pk->mpTevStr.get()));
    gabi::call(0x0255FE90, gabi::ea(pk->mpTevStr.get()));
    {
        gabi::Local<Mtx34> view;
        mtx_to_sead(gabi::ea(view.get()), gabi::ea(J3DSys_viewMtx));
        gabi::call(0x027FDA54, G, 0, view.get(), 0x104B470Cu, gabi::load<u32>(0x104B4708) + 0x240);
    }
    gabi::Local<GXColor[4]> c10;
    gabi::Local<GXColor[4]> amb;
    gabi::Local<GXColor[4]> fog;
    u32 tev = gabi::ea(pk->mpTevStr.get());
    u32 U = gabi::load<u32>(G + 4);
    color_s16_to_f(gabi::ea(c10.get()), tev + 0x90);
    gabi::call(0x0274D458, amb.get(), c10.get(), gabi::load<f32>(gabi::ea(pk->mpTevStr.get()) + 0x28));
    for (u32 i = 0; i < 16; i += 4) gabi::store<u32>(U + 0x1C4 + i, gabi::load<u32>(gabi::ea(amb.get()) + i));
    tev = gabi::ea(pk->mpTevStr.get());
    U = gabi::load<u32>(G + 4);
    color_s16_to_f(gabi::ea(c10.get()), tev + 0x160);
    gabi::call(0x0274D458, fog.get(), c10.get(), gabi::load<f32>(gabi::ea(pk->mpTevStr.get()) + 0x16C));
    for (u32 i = 0; i < 16; i += 4) gabi::store<u32>(U + 0x1D4 + i, gabi::load<u32>(gabi::ea(fog.get()) + i));
    gabi::call(0x027FDFF4, G, 0);
    tev = gabi::ea(pk->mpTevStr.get());
    color_s16_to_f(C, tev + 0x90);
    color_u8_to_f(C + 0x10, tev + 0x98);
    gabi::call(0x0274D2AC, C + 0x10, gabi::load<f32>(gabi::ea(pk->mpTevStr.get()) + 0x24));
    if (gabi::load<u8>(tev + 0x9F) != 0) {
        color_u8_to_f(M + 0x60, tev + 0x9C);
    } else {
        gabi::store<f32>(M + 0x64, 0.0f);
        gabi::store<f32>(M + 0x60, 0.0f);
        gabi::store<f32>(M + 0x68, 0.0f);
        gabi::store<f32>(M + 0x6C, 0.0f);
    }
    gabi::call(0x027FB678, M - 0x74);
    gabi::Local<Mtx34> mtx;
    mtx_to_sead(gabi::ea(mtx.get()), mtxSrc);
    PSMTXCopy(mtx.get(), gabi::at<Mtx34>(M - 0xA8));
    gabi::call(0x027FB678, G + 0xC);
    (void)P;
}

/* 0232B624 (HD): upload the flag's vertices (positions and front/back normals, 62 strip vertices) */
void Packet_c::update_hata() {
    WWHD_FUNC(0x0232B624, void, this);
    u32 P = gabi::ea(this);
    load_texture();
    u32 V = P + 0xD24;
    for (u32 k = 0; k < 2; k++) {
        u32 sel = gabi::load<u32>(V + 0x950);
        u32 dst = gabi::load<u32>(V + (k * 2 + sel) * 0x254) - 0x20;
        for (u32 n = 0; n < 62; n++) {
            dst += 0x20;
            u32 b = P + 0x9C + (u32)mCurBuf * 0x4EC;
            const u32 t = 0x101C867C + n * 3; /* strip table {pos, nrm, uv} */
            u32 p = b + gabi::load<u8>(t) * 0xC;
            f32 x = gabi::load<f32>(p), y = gabi::load<f32>(p + 4), z = gabi::load<f32>(p + 8);
            gabi::store<f32>(dst, x);
            gabi::store<f32>(dst + 4, y);
            gabi::store<f32>(dst + 8, z);
            b = P + 0x9C + (u32)mCurBuf * 0x4EC + (k == 0 ? 0x1A4 : 0x348);
            u32 q = b + gabi::load<u8>(t + 1) * 0xC;
            f32 nx = gabi::load<f32>(q), ny = gabi::load<f32>(q + 4), nz = gabi::load<f32>(q + 8);
            gabi::store<f32>(dst + 0x10, ny);
            gabi::store<f32>(dst + 0x14, nz);
            gabi::store<f32>(dst + 0xC, nx);
        }
    }
    u32 e = V + gabi::load<u32>(V + 0x950) * 0x254 + 4;
    for (int m = 0; m < 2; m++, e += 0x4A8) gabi::call(0x027B5E94, e, 0, gabi::load<u32>(e + 0x14C)); /* flush */
    gabi::store<u32>(V + 0x950, gabi::load<u32>(V + 0x950) == 0);
    set_tev_colors(this, P + 0x16A4, P + 0x17CC, P + 0x180C, gabi::ea(&mMtxHata));
}
VERIFY(0x0232B624, &Packet_c::update_hata);

/* 0232B984 (HD): upload the pole's normals (35 vertices, l_hasi_nrm by table) */
void Packet_c::update_hasi() {
    WWHD_FUNC(0x0232B984, void, this);
    u32 P = gabi::ea(this);
    load_texture();
    u32 V = P + 0x1F1C;
    u32 dst = gabi::load<u32>(V + gabi::load<u32>(V + 0x4A8) * 0x254) - 0x14;
    for (u32 n = 0; n < 35; n++) {
        u32 idx = gabi::load<u8>(0x101C867D + n * 3);
        u32 s = 0x10469388 + idx * 0xC;
        f32 z = gabi::load<f32>(s + 8), x = gabi::load<f32>(s), y = gabi::load<f32>(s + 4);
        dst += 0x20;
        gabi::store<f32>(dst, x);
        gabi::store<f32>(dst + 4, y);
        gabi::store<f32>(dst + 8, z);
    }
    u32 e = V + gabi::load<u32>(V + 0x4A8) * 0x254;
    gabi::call(0x027B5E94, e + 4, 0, gabi::load<u32>(e + 0x150)); /* flush */
    gabi::store<u32>(V + 0x4A8, gabi::load<u32>(V + 0x4A8) == 0);
    set_tev_colors(this, P + 0x23F4, P + 0x251C, P + 0x255C, gabi::ea(&mMtxHasi));
}
VERIFY(0x0232B984, &Packet_c::update_hasi);

/* 0232BBEC Packet_c::update */
void Packet_c::update(Act_c* a) {
    WWHD_FUNC(0x0232BBEC, void, this, a);
    PSMTXConcat_l(J3DSys_viewMtx, &mMtxHasi, &mViewMtxHasi);
    if (attr_type(a, 0x10026584, 0x1002659C)->mHata)
        PSMTXConcat_l(J3DSys_viewMtx, &mMtxHata, &mViewMtxHata);
    mpTevStr = &a->tevStr;
    gabi::call(0x027F0E04, gabi::load<u32>(j3dSys_ + 0x74), this, 0); /* J3DDrawBuffer::entryImm */
    if (attr_type(a, 0x10026584, 0x1002659C)->mHata)
        update_hata();
    update_hasi();
}
VERIFY(0x0232BBEC, &Packet_c::update);

/* ---- drawing helpers (HD engine inlines) ---- */
/* the program entry of the current pass: prog = programs[pass].shader (pass < 4) */
static inline u32 pass_program(u32 cntAddr, u32 ctx) {
    u32 prog = 0;
    s32 pass = gabi::load<s32>(ctx + 0xC);
    if (pass < 4) {
        u32 cnt = gabi::load<u32>(cntAddr);
        u32 e = gabi::load<u32>(cntAddr + 4);
        if ((u32)pass < cnt)
            e += pass * 0x14;
        prog = gabi::load<u32>(e);
    }
    return prog;
}
/* bind the shader program (the current one is cached in the state returned by 027F29D4) */
static inline void bind_program(u32 prog) {
    u32 S = gabi::call<u32>(0x027F29D4, j3dSys_);
    u32 sh = gabi::load<u32>(prog);
    if (sh != gabi::load<u32>(S + 4)) {
        u8 f = gabi::load<u8>(sh);
        u32 cur = gabi::load<u32>(S);
        if (f & 2) {
            gabi::store<u8>(sh, (u8)(f & ~2));
            gabi::call(0x027BB9E0, sh, 0);
        }
        u32 fs = gabi::load<u32>(gabi::load<u32>(sh + 0x7C) + 0x28);
        if (cur != fs)
            gabi::call(0x027B9F68, fs);
        u32 dlSize = gabi::load<u32>(sh + 0xC);
        if (dlSize != 0) {
            gabi::call(0xC00060E0, gabi::load<u32>(sh + 4), dlSize); /* GX2CallDisplayList */
            gabi::store<u32>(S, fs);
            gabi::store<u32>(S + 4, sh);
        } else {
            gabi::call(0x027BB7CC, sh);
            gabi::store<u32>(S + 4, sh);
            gabi::store<u32>(S, fs);
        }
    }
}
/* bind the uniform block `blk` of the shader object `shObj` to the program's vertex/geometry/pixel slots */
static inline void bind_uniform(u32 shObj, u32 prog) {
    u32 blk = shObj + 0x10 + gabi::load<u32>(shObj + 0x4C) * 0x1C;
    u32 loc = 0;
    if (gabi::load<u32>(prog + 0xC) != 0)
        loc = gabi::load<u32>(prog + 0x10);
    s16 vs = gabi::load<s16>(loc + 0xC);
    u32 size = gabi::load<u32>(blk + 4);
    u32 data = gabi::load<u32>(blk + 0xC);
    s16 ps = gabi::load<s16>(loc + 0xE);
    s16 gs = gabi::load<s16>(loc + 0x10);
    if (vs == -1 && ps == -1 && gs == -1)
        return;
    /* GX2 imports (the 028FAxxx stubs) */
    if (ps != -1)
        gabi::call(0xC0006900, ps, data, size); /* GX2SetPixelUniformBlock */
    if (vs != -1)
        gabi::call(0xC0006A38, vs, data, size); /* GX2SetVertexUniformBlock */
    if (gs != -1)
        gabi::call(0xC00068A8, gs, data, size); /* GX2SetGeometryUniformBlock */
}
static inline u32 prog_tex(u32 prog, u32 i) {
    if (i == 0)
        return gabi::load<u32>(prog + 0x14) != 0 ? gabi::load<u32>(prog + 0x18) : 0;
    return gabi::load<u32>(prog + 0x14) > 1 ? gabi::load<u32>(prog + 0x18) + 0x14 : 0;
}
static inline void set_tex(u32 obj, u32 loc) { gabi::call(0x027BE53C, obj, loc + 4, -1, 0); }

/* the draw state on the stack (0x11C bytes, constructor 02750250) */
struct DrawState_l {
    u8 _[0x11C];
};
static inline void draw_state_set(u32 st, s32 pass) {
    gabi::store<u32>(st + 0xC, 2);
    gabi::store<u8>(st + 0xE0, 1);
    u32 f = gabi::load<u32>(st + 0xEC);
    gabi::store<u32>(st + 0xE4, 4);
    gabi::store<f32>(st + 0xE8, 0.5f);
    gabi::store<u32>(st + 0xEC, (((f & ~0xFu) + 7) & ~0xF0u) + 0x10);
    gabi::call(0x0280037C, pass, st);
    gabi::call(0x02750370, st);
}
/* draw the vertex buffer of the current set: V = the set, I = the index buffer object */
static inline void draw_vtx(u32 attrs, u32 I) {
    gabi::call(0x027BFE5C, attrs);
    u32 n = gabi::load<u32>(I + 0xC);
    if (n != 0)
        gabi::call(0xC0006178, gabi::load<u32>(I + 4), n, gabi::load<u32>(I), gabi::load<u32>(I + 8), 0, 1); /* GX2DrawIndexedEx (import) */
}

/* 02328FD4 Packet_c::draw_hata (HD): the flag, front and back faces */
void Packet_c::draw_hata(Act_c* ctxp) {
    WWHD_FUNC(0x02328FD4, void, this, ctxp);
    u32 P = gabi::ea(this);
    u32 ctx = gabi::ea(ctxp);
    u32 prog = pass_program(P + 0xD1C, ctx);
    bind_program(prog);
    s32 pass = gabi::load<s32>(ctx + 0xC);
    Act_c* a = mpActor;
    gabi::Local<DrawState_l> st;
    u32 S = gabi::ea(st.get());
    if (pass == 0) {
        vcall2C(P + 0x1758, prog);
        vcall2C(P + 0x16B0, prog);
        u32 sh = gabi::load<u32>(ctx + 0x14);
        if (sh != 0)
            bind_uniform(gabi::load<u32>(sh + 4), prog);
        set_tex(PrmAbstract(a, 1, 8) == 0 ? P + 0x1BE0 : P + 0x1D78, prog_tex(prog, 0));
        gabi::call(0x02750250, S);
        gabi::store<u32>(S + 8, 0);
    } else if (pass == 1) {
        bind_uniform(gabi::load<u32>(P + 0x16A8), prog);
        vcall2C(P + 0x1758, prog);
        vcall2C(P + 0x16B0, prog);
        set_tex(PrmAbstract(a, 1, 8) == 0 ? P + 0x1BE0 : P + 0x1D78, prog_tex(prog, 0));
        set_tex(P + 0x2AC8, prog_tex(prog, 1));
        gabi::call(0x02750250, S);
        gabi::store<u32>(S + 8, 0);
    } else {
        if (pass == 2) {
            bind_uniform(gabi::load<u32>(P + 0x16A8), prog);
            vcall2C(P + 0x1758, prog);
            vcall2C(P + 0x16B0, prog);
            u32 o = gabi::load<u32>(ctx + 0x30);
            if (o != 0)
                vcall2C(o, prog);
            set_tex(PrmAbstract(a, 1, 8) == 0 ? P + 0x1BE0 : P + 0x1D78, prog_tex(prog, 0));
            set_tex(P + 0x2AC8, prog_tex(prog, 1));
            gabi::call(0x027FFE54, ctx, prog);
        }
        gabi::call(0x02750250, S);
        gabi::store<u32>(S + 8, 0);
    }
    draw_state_set(S, gabi::load<s32>(ctx + 0xC));
    /* front faces: the current vertex set's first buffer */
    {
        u32 cnt = gabi::load<u32>(P + 0xD1C);
        u32 pass2 = gabi::load<u32>(ctx + 0xC);
        u32 e = gabi::load<u32>(P + 0xD20);
        if (pass2 < cnt)
            e += pass2 * 0x14;
        u32 sel = gabi::load<u32>(P + 0x1674);
        draw_vtx(gabi::load<u32>(e + (sel == 0 ? 8 : 0) + 8), P + 0x168C);
    }
    gabi::store<u32>(S + 8, 1);
    gabi::call(0x02750370, S);
    /* back faces */
    {
        u32 cnt = gabi::load<u32>(P + 0xD1C);
        u32 pass2 = gabi::load<u32>(ctx + 0xC);
        u32 sel = gabi::load<u32>(P + 0x1674);
        u32 e = gabi::load<u32>(P + 0xD20);
        if (pass2 < cnt)
            e += pass2 * 0x14;
        e += (sel == 0 ? 8 : 0);
        u32 n = gabi::load<u32>(e + 4);
        u32 attrs = gabi::load<u32>(e + 8);
        if (n > 1)
            attrs += 0xF4;
        draw_vtx(attrs, P + 0x168C);
    }
    gabi::call(0x02750370, j3dSys_ + 0x18C);
}
VERIFY(0x02328FD4, &Packet_c::draw_hata);

/* 023296B0 Packet_c::draw_hasi (HD): the pole */
void Packet_c::draw_hasi(Act_c* ctxp) {
    WWHD_FUNC(0x023296B0, void, this, ctxp);
    u32 P = gabi::ea(this);
    u32 ctx = gabi::ea(ctxp);
    u32 prog = pass_program(P + 0x1F14, ctx);
    bind_program(prog);
    s32 pass = gabi::load<s32>(ctx + 0xC);
    gabi::Local<DrawState_l> st;
    u32 S = gabi::ea(st.get());
    if (pass == 0) {
        vcall2C(P + 0x24A8, prog);
        vcall2C(P + 0x2400, prog);
        u32 sh = gabi::load<u32>(ctx + 0x14);
        if (sh != 0)
            bind_uniform(gabi::load<u32>(sh + 4), prog);
        set_tex(P + 0x28A0, prog_tex(prog, 0));
        set_tex(P + 0x2AC8, prog_tex(prog, 1));
        gabi::call(0x02750250, S);
        gabi::store<u32>(S + 8, 0);
    } else if (pass == 1) {
        bind_uniform(gabi::load<u32>(P + 0x23F8), prog);
        vcall2C(P + 0x24A8, prog);
        vcall2C(P + 0x2400, prog);
        set_tex(P + 0x28A0, prog_tex(prog, 0));
        set_tex(P + 0x2AC8, prog_tex(prog, 1));
        gabi::call(0x02750250, S);
        gabi::store<u32>(S + 8, 0);
    } else {
        if (pass == 2) {
            bind_uniform(gabi::load<u32>(P + 0x23F8), prog);
            vcall2C(P + 0x24A8, prog);
            vcall2C(P + 0x2400, prog);
            u32 o = gabi::load<u32>(ctx + 0x30);
            if (o != 0)
                vcall2C(o, prog);
            set_tex(P + 0x28A0, prog_tex(prog, 0));
            set_tex(P + 0x2AC8, prog_tex(prog, 1));
            gabi::call(0x027FFE54, ctx, prog);
        }
        gabi::call(0x02750250, S);
        gabi::store<u32>(S + 8, 0);
    }
    draw_state_set(S, gabi::load<s32>(ctx + 0xC));
    {
        u32 cnt = gabi::load<u32>(P + 0x1F14);
        u32 pass2 = gabi::load<u32>(ctx + 0xC);
        u32 e = gabi::load<u32>(P + 0x1F18);
        u32 sel = gabi::load<u32>(P + 0x23C4);
        if (pass2 < cnt)
            e += pass2 * 0x14;
        draw_vtx(gabi::load<u32>(e + (sel == 0 ? 8 : 0) + 8), P + 0x23DC);
    }
    gabi::call(0x02750370, j3dSys_ + 0x18C);
}
VERIFY(0x023296B0, &Packet_c::draw_hasi);

/* ---- GPU initialisation (HD) ---- */
#define l_heap 0x101F8B4Cu /* the GPU heap pointer */
static inline u32 gpu_alloc(u32 size, u32 align) {
    u32 h = gabi::call<u32>(0x02756140, gabi::load<u32>(l_heap));
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(h + 0xC) + 0x34), h, size, align);
}
/* r4 still holds the array pointer at the heap getter (it reads it) */
/* the pointer to free is read (at pa) after the heap getter */
static inline void gpu_free(u32 pa, u32 r4) {
    u32 h = gabi::call<u32>(0x02755FEC, gabi::load<u32>(l_heap), r4);
    u32 fn = gabi::load<u32>(gabi::load<u32>(h + 0xC) + 0x3C);
    gabi::call_ptr(fn, h, gabi::load<u32>(pa));
}
static inline void safestring(u32 s, u32 str) {
    gabi::store<u32>(s, str);
    gabi::store<u32>(s + 4, 0x10026548);
}
/* the shader program list of the part, found by name ("flag_default"), into the part's list at L */
static inline void find_program(u32 P, u32 name, u32 L) {
    gabi::Local<SafeString> key;
    safestring(gabi::ea(key.get()), name);
    u32 mgr = gabi::call<u32>(0x027FFCBC, P);
    s32 i = gabi::call<s32>(0x027B90AC, gabi::load<u32>(mgr + 4), key.get());
    u32 e = 0;
    if (i >= 0) {
        u32 n = gabi::load<u32>(mgr + 8);
        u32 tab = gabi::load<u32>(mgr + 0xC);
        u32 ent = (u32)i < n ? tab + i * 0x24 : tab;
        if (gabi::load<u8>(ent + 0x20) == 0) {
            u32 src = gabi::load<u32>(mgr + 4);
            u32 m = gabi::load<u32>(src + 0x1C);
            u32 ent2 = (u32)i < n ? tab + i * 0x24 : tab;
            u32 info = (u32)i < m ? gabi::load<u32>(src + 0x20) + i * 0x84 : 0;
            gabi::call(0x02800B0C, ent2, info, 0);
            n = gabi::load<u32>(mgr + 8);
            tab = gabi::load<u32>(mgr + 0xC);
        }
        e = (u32)i < n ? tab + i * 0x24 : tab;
    }
    gabi::call(0x0280068C, L, e, 0);
}
/* release and recreate the attribute objects (0xF4 bytes, constructor 027BF734) of a program entry */
static inline void attrs_free(u32 E, u32 cntOff) {
    u32 ptr = gabi::load<u32>(E + cntOff + 4);
    if (ptr == 0)
        return;
    s32 cnt = gabi::load<s32>(E + cntOff);
    for (s32 i = 0; i < cnt;) {
        u32 obj = ptr + i * 0xF4;
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(obj + 0xF0) + 0xC), obj, 2);
        cnt = gabi::load<s32>(E + cntOff);
        i++;
        ptr = gabi::load<u32>(E + cntOff + 4);
    }
    gpu_free(E + cntOff + 4, ptr);
    gabi::store<u32>(E + cntOff, 0);
    gabi::store<u32>(E + cntOff + 4, 0);
}
static inline void dcbz_range(u32 d, u32 size) {
    u32 end = d + size;
    if (d < end) {
        do {
            u32 a = d & ~31u;
            for (u32 i = 0; i < 32; i += 4) gabi::store<u32>(a + i, 0);
            d += 0x20;
        } while (d < end);
    }
}
/* lfs/stfs pairs; the recompiled original keeps a signalling NaN's bits here (seen at an unaligned
 * address, where the harness's float-store NaN tolerance does not apply): copy the bits */
static inline void copy_floats(u32 d, u32 s, u32 n) {
    for (u32 i = 0; i < n; i += 4) gabi::store<u32>(d + i, gabi::load<u32>(s + i));
}

/* 0232BDC0 (HD): the flag's GPU objects: programs, vertex buffers (4 x 62 vertices), textures */
void Packet_c::gpu_init_hata() {
    WWHD_FUNC(0x0232BDC0, void, this);
    u32 P = gabi::ea(this);
    find_program(P, 0x10026688 /* "flag_default" */, P + 0xD18);
    gabi::store<u32>(P + 0x1678, 0x13);
    gabi::store<u32>(P + 0x1680, 0x10026810);
    u32 V = P + 0xD24;
    for (u32 k = 0; k < 2; k++) {
        for (u32 m = 0; m < 2; m++) {
            u32 el = V + k * 0x254 + m * 0x4A8;
            u32 data = gabi::load<u32>(el);
            if (data == 0) {
                u32 buf = el + 0x24C;
                u32 p = gpu_alloc(0x7C0, 0x40);
                if (p != 0) {
                    gabi::store<u32>(buf + 4, p);
                    gabi::store<u32>(buf, 0x3E);
                }
                data = gabi::load<u32>(buf + 4);
                gabi::store<u32>(el, data);
            }
            gabi::call(0x027FF478, el + 4, data, 0x3E, V + 0x954);
        }
    }
    gabi::store<u32>(V + 0x960, 0);
    gabi::store<u8>(V + 0x964, 1);
    for (u32 i = 0; i < gabi::load<u32>(P + 0xD18); i++) {
        u32 E = gabi::load<u32>(P + 0xD20);
        if (i < gabi::load<u32>(P + 0xD1C))
            E += i * 0x14;
        u32 hasPtr = gabi::load<u32>(E + 8);
        u32 prog = gabi::load<u32>(E);
        gabi::store<u32>(E, 0);
        (void)hasPtr;
        attrs_free(E, 4);
        attrs_free(E, 0xC);
        gabi::store<u32>(E, prog);
        for (u32 c = 4; c <= 0xC; c += 8) {
            u32 p = gpu_alloc(0x1E8, 4);
            for (u32 o = 0; o < 0x1E8; o += 0xF4)
                if (p + o != 0)
                    gabi::call(0x027BF734, p + o);
            if (p != 0) {
                gabi::store<u32>(E + c + 4, p);
                gabi::store<u32>(E + c, 2);
            }
        }
        for (u32 k = 0; k < 2; k++) {
            for (u32 m = 0; m < 2; m++) {
                u32 L = E + 4 + k * 8;
                u32 a = gabi::load<u32>(L + 4);
                if (m < gabi::load<u32>(L))
                    a += m * 0xF4;
                gabi::call(0x027FF530, prog, a, V + 4 + k * 0x254 + m * 0x4A8, V + 0x954, 0);
            }
        }
    }
    gabi::call(0x027FE084, P + 0x16A4, 1, 0);
    gabi::call(0x027B54E0, P + 0x168C, 0x101C8738u, 4, gabi::load<u32>(0x101C81E4));
    gabi::store<u32>(P + 0x1690, 4);
    /* the initial vertices: l_pos by the strip table, zero normals, uv */
    u32 sel = gabi::load<u32>(V + 0x950);
    for (u32 k = 0; k < 2; k++) {
        u32 off = (2 * k + sel) * 0x254;
        u32 d = gabi::load<u32>(V + off);
        if (d < d + 0x7C0) {
            dcbz_range(d, 0x7C0);
            sel = gabi::load<u32>(P + 0x1674);
            off = (2 * k + sel) * 0x254;
        }
        u32 data = gabi::load<u32>(V + off);
        if (0 < gabi::load<u32>(0x101C81E0)) {
            u32 dst = data - 0x20;
            u32 t = 0x101C867C - 3;
            u32 i = 0;
            do {
                t += 3;
                u32 src = 0x101C83C0 + gabi::load<u8>(t) * 0xC;
                f32 z = gabi::load<f32>(src + 8), x = gabi::load<f32>(src), y = gabi::load<f32>(src + 4);
                dst += 0x20;
                gabi::store<f32>(dst, x);
                gabi::store<f32>(dst + 4, y);
                gabi::store<f32>(dst + 8, z);
                gabi::store<f32>(dst + 0xC, 0.0f);
                gabi::store<f32>(dst + 0x10, 0.0f);
                gabi::store<f32>(dst + 0x14, 0.0f);
                u32 uv = gabi::load<u8>(t + 2) * 8;
                f32 u = gabi::load<f32>(0x101C8564 + uv), v = gabi::load<f32>(0x101C8568 + uv);
                gabi::store<f32>(dst + 0x18, u);
                gabi::store<f32>(dst + 0x1C, v);
                i++;
            } while (i < gabi::load<u32>(0x101C81E0));
            sel = gabi::load<u32>(V + 0x950);
        }
    }
    /* copy into the other buffers */
    u32 dset = V + (sel == 0) * 0x254;
    for (u32 k = 0; k < 2; k++, dset += 0x4A8) {
        u32 sset = V + (2 * k + sel) * 0x254;
        for (u32 n = 0; n < 62; n++) copy_floats(gabi::load<u32>(dset) + n * 0x20, gabi::load<u32>(sset) + n * 0x20, 0x20);
        sel = gabi::load<u32>(V + 0x950);
    }
    u32 e = V + sel * 0x254 + 4;
    for (int m = 0; m < 2; m++, e += 0x4A8) gabi::call(0x027B5E94, e, 0, gabi::load<u32>(e + 0x14C));
    gabi::store<u32>(V + 0x950, gabi::load<u32>(V + 0x950) == 0);
    /* textures "k_hata01" / "k_hata02" from ProgramTexture.bfres */
    gabi::call(0x0274FBF8, gabi::load<u32>(0x101F8B18));
    {
        gabi::Local<SafeString> s0, s1;
        safestring(gabi::ea(s0.get()), 0x10026698);
        safestring(gabi::ea(s1.get()), 0x100266A8);
        u32 r = gabi::call<u32>(0x026124B0, gabi::load<u32>(0x101F4F7C), s0.get(), s1.get(), 0);
        gabi::call(0x02773870, P + 0x1AC0, r, 0x100266C0u);
        gabi::Local<SafeString> s2, s3;
        safestring(gabi::ea(s2.get()), 0x10026698);
        safestring(gabi::ea(s3.get()), 0x100266A8);
        r = gabi::call<u32>(0x026124B0, gabi::load<u32>(0x101F4F7C), s2.get(), s3.get(), 0);
        gabi::call(0x02773870, P + 0x1B50, r, 0x100266CCu);
    }
    gabi::call(0x0274FCCC, gabi::load<u32>(0x101F8B18));
    tex_bind(P + 0x1BE0, P + 0x1AC0);
    tex_bind(P + 0x1D78, P + 0x1B50);
}
VERIFY(0x0232BDC0, &Packet_c::gpu_init_hata);

/* 0232C670 (HD): the pole's GPU objects (2 x 35 vertices, texture "k_taru02") */
void Packet_c::gpu_init_hasi() {
    WWHD_FUNC(0x0232C670, void, this);
    u32 P = gabi::ea(this);
    find_program(P, 0x100266D8 /* "flag_default" */, P + 0x1F10);
    gabi::store<u32>(P + 0x23C8, 0x13);
    gabi::store<u32>(P + 0x23D0, 0x10026810);
    u32 V = P + 0x1F1C;
    for (u32 k = 0; k < 2; k++) {
        u32 el = V + k * 0x254;
        u32 data = gabi::load<u32>(el);
        if (data == 0) {
            u32 buf = el + 0x24C;
            u32 p = gpu_alloc(0x460, 0x40);
            if (p != 0) {
                gabi::store<u32>(buf + 4, p);
                gabi::store<u32>(buf, 0x23);
            }
            data = gabi::load<u32>(buf + 4);
            gabi::store<u32>(el, data);
        }
        gabi::call(0x027FF478, el + 4, data, 0x23, V + 0x4AC);
    }
    gabi::store<u32>(V + 0x4B8, 0);
    gabi::store<u8>(V + 0x4BC, 1);
    for (u32 i = 0; i < gabi::load<u32>(P + 0x1F10); i++) {
        u32 E = gabi::load<u32>(P + 0x1F18);
        if (i < gabi::load<u32>(P + 0x1F14))
            E += i * 0x14;
        u32 prog = gabi::load<u32>(E);
        gabi::store<u32>(E, 0);
        attrs_free(E, 4);
        attrs_free(E, 0xC);
        gabi::store<u32>(E, prog);
        for (u32 c = 4; c <= 0xC; c += 8) {
            u32 p = gpu_alloc(0xF4, 4);
            if (p != 0)
                gabi::call(0x027BF734, p);
            if (p != 0) {
                gabi::store<u32>(E + c + 4, p);
                gabi::store<u32>(E + c, 1);
            }
        }
        for (u32 k = 0; k < 2; k++) {
            u32 L = E + 4 + k * 8;
            u32 a = gabi::load<u32>(L + 4);
            gabi::call(0x027FF530, prog, a, V + 4 + k * 0x254, V + 0x4AC, 0);
        }
    }
    gabi::call(0x027FE084, P + 0x23F4, 1, 0);
    gabi::call(0x027B54E0, P + 0x23DC, 0x101C8364u, 4, gabi::load<u32>(0x101C81DC));
    gabi::store<u32>(P + 0x23E0, 4);
    u32 sel = gabi::load<u32>(V + 0x4A8);
    u32 cur = V + sel * 0x254;
    u32 d = gabi::load<u32>(cur);
    if (d < d + 0x460) {
        dcbz_range(d, 0x460);
        sel = gabi::load<u32>(P + 0x23C4);
        cur = V + sel * 0x254;
    }
    u32 data = gabi::load<u32>(cur);
    if (0 < gabi::load<u32>(0x101C81D8)) {
        u32 dst = data - 0x20;
        u32 t = 0x101C82F5;
        u32 i = 0;
        do {
            t += 3;
            u32 src = 0x101C8274 + gabi::load<u8>(t) * 0xC; /* l_hasi_pos */
            f32 z = gabi::load<f32>(src + 8), x = gabi::load<f32>(src), y = gabi::load<f32>(src + 4);
            dst += 0x20;
            gabi::store<f32>(dst, x);
            gabi::store<f32>(dst + 4, y);
            gabi::store<f32>(dst + 8, z);
            gabi::store<f32>(dst + 0xC, 0.0f);
            gabi::store<f32>(dst + 0x10, 0.0f);
            gabi::store<f32>(dst + 0x14, 0.0f);
            u32 uv = gabi::load<u8>(t + 2) * 8;
            f32 u = gabi::load<f32>(0x101C821C + uv), v = gabi::load<f32>(0x101C8220 + uv);
            gabi::store<f32>(dst + 0x18, u);
            gabi::store<f32>(dst + 0x1C, v);
            i++;
        } while (i < gabi::load<u32>(0x101C81D8));
        sel = gabi::load<u32>(V + 0x4A8);
        cur = V + sel * 0x254;
    }
    u32 other = V + (sel == 0) * 0x254;
    for (u32 n = 0; n < 35; n++) copy_floats(gabi::load<u32>(other) + n * 0x20, gabi::load<u32>(cur) + n * 0x20, 0x20);
    u32 e = V + gabi::load<u32>(V + 0x4A8) * 0x254;
    gabi::call(0x027B5E94, e + 4, 0, gabi::load<u32>(e + 0x150));
    gabi::store<u32>(V + 0x4A8, gabi::load<u32>(V + 0x4A8) == 0);
    gabi::call(0x0274FBF8, gabi::load<u32>(0x101F8B18));
    {
        gabi::Local<SafeString> s0, s1;
        safestring(gabi::ea(s0.get()), 0x100266E8);
        safestring(gabi::ea(s1.get()), 0x100266F8);
        u32 r = gabi::call<u32>(0x026124B0, gabi::load<u32>(0x101F4F7C), s0.get(), s1.get(), 0);
        gabi::call(0x02773870, P + 0x2810, r, 0x10026710u);
    }
    gabi::call(0x0274FCCC, gabi::load<u32>(0x101F8B18));
    tex_bind(P + 0x28A0, P + 0x2810);
}
VERIFY(0x0232C670, &Packet_c::gpu_init_hasi);

/* ---- destructors (HD) ---- */
/* release the GPU memory of one vertex buffer element (0x254 bytes) */
static inline void vbuf_release(u32 el) {
    gabi::call(0x027BF7E8, el + 0x158);
    u32 p = gabi::load<u32>(el + 0x250);
    gabi::store<u32>(el, 0);
    if (p != 0) {
        gpu_free(el + 0x250, p);
        gabi::store<u32>(el + 0x24C, 0);
        gabi::store<u32>(el + 0x250, 0);
    }
}
/* release a program list {count (+0), entries (+4)} with its attribute objects */
static inline void proglist_release(u32 L) {
    u32 ptr = gabi::load<u32>(L + 4);
    if (ptr == 0)
        return;
    s32 cnt = gabi::load<s32>(L);
    for (s32 i = 0; i < cnt; i++) {
        u32 E = ptr + i * 0x14;
        if (E == 0)
            continue;
        gabi::store<u32>(E, 0);
        attrs_free(E, 4);
        attrs_free(E, 0xC);
        cnt = gabi::load<s32>(L);
        ptr = gabi::load<u32>(L + 4);
    }
    gpu_free(L + 4, ptr);
    gabi::store<u32>(L, 0);
    gabi::store<u32>(L + 4, 0);
}
/* the uniform blocks {count (+0), blocks (+4), 0x23C each}: two buffers each (027BEBEC) */
static inline void uniforms_release(u32 L) {
    s32 cnt = gabi::load<s32>(L);
    for (s32 i = 0; i < cnt; i++) {
        u32 b = gabi::load<u32>(L + 4);
        if ((u32)i < (u32)cnt)
            b += i * 0x23C;
        for (u32 j = 0; j < 2; j++) gabi::call(0x027BEBEC, b + 0x10 + j * 0x1C);
        cnt = gabi::load<s32>(L);
    }
}
static inline void packet_dt_body(u32 P) {
    gabi::store<u32>(P + 0xC, 0x10026860);
    u32 V = P + 0xD24;
    for (u32 k = 0; k < 4; k++) vbuf_release(V + k * 0x254);
    gabi::store<u32>(V + 0x960, 0);
    u32 W = P + 0x1F1C;
    vbuf_release(W);
    vbuf_release(W + 0x254);
    gabi::store<u32>(W + 0x4B8, 0);
    uniforms_release(P + 0x16A4);
    for (u32 j = 0; j < 2; j++) gabi::call(0x027BEBEC, P + 0x16C0 + j * 0x1C);
    for (u32 j = 0; j < 2; j++) gabi::call(0x027BEBEC, P + 0x1768 + j * 0x1C);
    uniforms_release(P + 0x23F4);
    for (u32 j = 0; j < 2; j++) gabi::call(0x027BEBEC, P + 0x2410 + j * 0x1C);
    for (u32 j = 0; j < 2; j++) gabi::call(0x027BEBEC, P + 0x24B8 + j * 0x1C);
    /* the pole's objects */
    gabi::call(0x027BE2B0, P + 0x2AC8, 2);
    gabi::call(0x027BE2B0, P + 0x28A0, 2);
    gabi::call(0x027FB528, P + 0x24A8, 0);
    gabi::call(0x027FB528, P + 0x2400, 0);
    gabi::call(0x027FD764, P + 0x23F4, 2);
    gabi::call(0x027B54A0, P + 0x23DC, 2);
    if (W != 0) {
        vbuf_release(W);
        vbuf_release(W + 0x254);
        gabi::store<u32>(W + 0x4B8, 0);
        gabi::call(0x028F0164, W, 2, 0x254, 0x0232DCF4u, 0, 0); /* __destroy_arr */
    }
    proglist_release(P + 0x1F14);
    /* the flag's objects */
    gabi::call(0x027BE2B0, P + 0x1D78, 2);
    gabi::call(0x027BE2B0, P + 0x1BE0, 2);
    gabi::call(0x027FB528, P + 0x1758, 0);
    gabi::call(0x027FB528, P + 0x16B0, 0);
    gabi::call(0x027FD764, P + 0x16A4, 2);
    gabi::call(0x027B54A0, P + 0x168C, 2);
    if (V != 0) {
        for (u32 k = 0; k < 4; k++) vbuf_release(V + k * 0x254);
        gabi::store<u32>(V + 0x960, 0);
        gabi::call(0x028F0164, V, 4, 0x254, 0x0232DD54u, 0, 0); /* __destroy_arr */
    }
    proglist_release(P + 0xD1C);
    gabi::call(0x027F13DC, P, 0); /* J3DPacket::~J3DPacket */
}

/* 0232DDB4 Packet_c::~Packet_c (deleting) */
static void Packet_dt(Packet_c* p, s32 flags) {
    WWHD_FUNC(0x0232DDB4, void, p, flags);
    if (p != nullptr) {
        packet_dt_body(gabi::ea(p));
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x0232DDB4, Packet_dt);

/* 0232E608 Act_c::~Act_c (deleting) */
static void Act_dt(Act_c* a, s32 flags) {
    WWHD_FUNC(0x0232E608, void, a, flags);
    if (a != nullptr) {
        packet_dt_body(gabi::ea(a) + 0x520);
        dCcD_Cyl_dt(&a->mCyl, 2);
        dCcD_Stts_dt(&a->mStts, 2);
        gabi::call(0x025D50BC, a, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(a);
    }
}
VERIFY(0x0232E608, Act_dt);

/* 02329C4C Packet_c::draw (virtual; ctx: the HD draw context) */
void Packet_c::draw(Act_c* ctx) {
    WWHD_FUNC(0x02329C4C, void, this, ctx);
    if (attr_type(mpActor, 0x10026584, 0x1002659C)->mHata)
        draw_hata(ctx);
    draw_hasi(ctx);
}
VERIFY(0x02329C4C, &Packet_c::draw);

}  // namespace daObjBuoyflag
