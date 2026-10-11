/**
 * d_a_obj_buoyflag_draw.cpp (WWHD)
 * Object - Buoy flag: drawing. HD replaces the GameCube display-list drawing (draw_hata /
 * draw_hasi with GX calls) by GPU objects: per part (flag "hata", pole "hasi") a double-buffered
 * vertex buffer set, a shader/uniform state, and the "Cloth" texture with its sampler.
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx) and verified against it. The HD engine classes are not named yet: their
 * methods use named guest bindings and explicitly checked HD layouts.
 */
#include "d/actor/d_a_obj_buoyflag.h"

namespace daObjBuoyflag {

/* ---- local bindings (HD engine, unnamed) ---- */
static inline void vcall2C(u32 obj, u32 arg) {
    gabi::call_ptr(static_cast<u32>(buoyflag_view<BuoyMaterialVtable>(
                                        static_cast<u32>(buoyflag_view<BuoyMaterialInterface>(obj)->vtable))
                                        ->apply),
                   obj, arg);
}
#define j3dSys_ 0x104B45C0u
#define J3DSys_viewMtx buoyflag_viewMatrix()

// HD: GXColor -> float colour (rgba / 255)
/* 02328F20 */
static void color_u8_to_f(u32 dst, u32 src) {
    f32 r = (f32)(u8)buoyflag_view<BuoyColorU8>(src)->r / 255.0f;
    f32 g = (f32)(u8)buoyflag_view<BuoyColorU8>(src)->g / 255.0f;
    f32 b = (f32)(u8)buoyflag_view<BuoyColorU8>(src)->b / 255.0f;
    f32 a = (f32)(u8)buoyflag_view<BuoyColorU8>(src)->a / 255.0f;
    buoyflag_view<BuoyFloatColorWords>(dst)->r = fbits_of(r);
    buoyflag_view<BuoyFloatColorWords>(dst)->g = fbits_of(g);
    buoyflag_view<BuoyFloatColorWords>(dst)->b = fbits_of(b);
    buoyflag_view<BuoyFloatColorWords>(dst)->a = fbits_of(a);
}

// HD: GXColorS10 -> float colour (rgba / 255)
/* 0232B560 */
static void color_s16_to_f(u32 dst, u32 src) {
    f32 r = (f32)(s16)buoyflag_view<BuoyColorS16>(src)->r / 255.0f;
    f32 g = (f32)(s16)buoyflag_view<BuoyColorS16>(src)->g / 255.0f;
    f32 b = (f32)(s16)buoyflag_view<BuoyColorS16>(src)->b / 255.0f;
    f32 a = (f32)(s16)buoyflag_view<BuoyColorS16>(src)->a / 255.0f;
    buoyflag_view<BuoyFloatColorWords>(dst)->r = fbits_of(r);
    buoyflag_view<BuoyFloatColorWords>(dst)->g = fbits_of(g);
    buoyflag_view<BuoyFloatColorWords>(dst)->b = fbits_of(b);
    buoyflag_view<BuoyFloatColorWords>(dst)->a = fbits_of(a);
}

// HD: Mtx -> sead::Matrix34 (through FPRs: all twelve loaded, then stored)
/* 0232B318 */
static void mtx_to_sead(u32 dst, u32 src) {
    u32 v[12];
    for (int i = 0; i < 12; i++)
        v[i] = fbits(gabi::ea(&buoyflag_view<BuoyMatrixWords>(src)->words[i]));
    for (int i = 0; i < 12; i++)
        buoyflag_view<BuoyMatrixWords>(dst)->words[i] = v[i];
}

/* texture T bound into sampler/texture object S: copy the image fields when they match, else rebuild */
static inline void tex_bind(u32 S, u32 T) {
    if ((u32)buoyflag_view<BuoyTextureView>(S)->width == (u32)buoyflag_view<BuoyTextureView>(T)->width &&
        (u32)buoyflag_view<BuoyTextureView>(S)->height == (u32)buoyflag_view<BuoyTextureView>(T)->height &&
        (u32)buoyflag_view<BuoyTextureView>(S)->depth == (u32)buoyflag_view<BuoyTextureView>(T)->depth &&
        (u32)buoyflag_view<BuoyTextureView>(S)->dimension ==
            (u32)buoyflag_view<BuoyTextureView>(T)->dimension &&
        (u32)buoyflag_view<BuoyTextureView>(S)->format == (u32)buoyflag_view<BuoyTextureView>(T)->format &&
        (u32)buoyflag_view<BuoyTextureView>(S)->mipCount ==
            (u32)buoyflag_view<BuoyTextureView>(T)->mipCount &&
        (u32)buoyflag_view<BuoyTextureView>(S)->swizzle == (u32)buoyflag_view<BuoyTextureView>(T)->swizzle &&
        (u32)buoyflag_view<BuoyTextureView>(S)->tileMode ==
            (u32)buoyflag_view<BuoyTextureView>(T)->tileMode &&
        (u32)buoyflag_view<BuoyTextureView>(S)->arrayCount ==
            (u32)buoyflag_view<BuoyTextureView>(T)->arrayCount) {
        u32 a = (u32)buoyflag_view<BuoyTextureView>(T)->image;
        u32 b = (u32)buoyflag_view<BuoyTextureView>(T)->mipOffset;
        buoyflag_view<BuoyTextureView>(S)->duplicateImage = a;
        buoyflag_view<BuoyTextureView>(S)->duplicateMipOffset = b;
        buoyflag_view<BuoyTextureView>(S)->mipOffset = b;
        buoyflag_view<BuoyTextureView>(S)->image = a;
    } else {
        buoyflag_rebuildTexture(S, T);
    }
    buoyflag_view<BuoyTextureView>(S)->wrapX = 2;
    buoyflag_view<BuoyTextureView>(S)->wrapY = 2;
    buoyflag_view<BuoyTextureView>(S)->wrapZ = 2;
    buoyflag_view<BuoyTextureView>(S)->flags = (u8)((u8)buoyflag_view<BuoyTextureView>(S)->flags | 2);
}

// HD: load the "Cloth" texture (index 3) once
/* 0232B3B8 */
void Packet_c::load_texture() {

    u32 P = gabi::ea(this);
    buoyflag_lockTextureManager(static_cast<u32>(*buoyflag_textureManager())); /* lock */
    if (static_cast<u8>(buoyflag_view<Packet_c>(P)->textureLoaded) == 0) {
        void* tmp_img = dComIfG_getObjectRes("Cloth" /* "Cloth" */, dRes_INDEX_CLOTH_BTI_TOON_e, 0x10026548);
        if (tmp_img == nullptr)
            JUT_ASSERT_fail("d_a_obj_buoyflag.cpp", 0x7D6, "tmp_img != (0)"); /* tmp_img != (0) */
        buoyflag_loadTexture(gabi::ea(&buoyflag_view<Packet_c>(P)->toonResource),
                             static_cast<u32>(buoyflag_view<BuoyImageResource>(gabi::ea(tmp_img))->texture));
        tex_bind(gabi::ea(&buoyflag_view<Packet_c>(P)->toonTexture),
                 gabi::ea(&buoyflag_view<Packet_c>(P)->toonResource));
        buoyflag_view<Packet_c>(P)->textureLoaded = 1;
    }
    buoyflag_unlockTextureManager(static_cast<u32>(*buoyflag_textureManager())); /* unlock */
}

/* tevStr colours into the uniform block of a part: G =
 * gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.uniforms) (flag) /
 * gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.uniforms) (pole) */
static inline void set_tev_colors(Packet_c* pk, u32 G, u32 M, u32 C, u32 mtxSrc) {
    u32 P = gabi::ea(pk);
    buoyflag_updateLighting(gabi::ea(pk->mpTevStr.get()));
    buoyflag_calculateLightingColors(gabi::ea(pk->mpTevStr.get()));
    {
        gabi::Local<Mtx34> view;
        mtx_to_sead(gabi::ea(view.get()), gabi::ea(J3DSys_viewMtx));
        buoyflag_setMaterialView(G, 0, view.get(), 0x104B470Cu,
                                 static_cast<u32>(*buoyflag_modelSystem()) + 0x240);
    }
    gabi::Local<GXColor[4]> c10;
    gabi::Local<GXColor[4]> amb;
    gabi::Local<GXColor[4]> fog;
    u32 tev = gabi::ea(pk->mpTevStr.get());
    u32 U = static_cast<u32>(buoyflag_view<BuoyList>(G)->array);
    color_s16_to_f(gabi::ea(c10.get()), tev + 0x90);
    buoyflag_scaleMaterialColor(
        amb.get(), c10.get(),
        static_cast<f32>(buoyflag_view<BuoyLightingParameters>(gabi::ea(pk->mpTevStr.get()))->materialScale));
    for (u32 i = 0; i < 4; i++)
        buoyflag_view<BuoyColorWords>(gabi::ea(&buoyflag_view<BuoyDrawMaterial>(U)->frontColor))->words[i] =
            static_cast<u32>(buoyflag_view<BuoyColorWords>(gabi::ea(amb.get()))->words[i]);
    tev = gabi::ea(pk->mpTevStr.get());
    U = static_cast<u32>(buoyflag_view<BuoyList>(G)->array);
    color_s16_to_f(gabi::ea(c10.get()), tev + 0x160);
    buoyflag_scaleMaterialColor(
        fog.get(), c10.get(),
        static_cast<f32>(buoyflag_view<BuoyLightingParameters>(gabi::ea(pk->mpTevStr.get()))->extraScale));
    for (u32 i = 0; i < 4; i++)
        buoyflag_view<BuoyColorWords>(gabi::ea(&buoyflag_view<BuoyDrawMaterial>(U)->backColor))->words[i] =
            static_cast<u32>(buoyflag_view<BuoyColorWords>(gabi::ea(fog.get()))->words[i]);
    buoyflag_flushDrawMaterial(G, 0);
    tev = gabi::ea(pk->mpTevStr.get());
    color_s16_to_f(C, tev + 0x90);
    color_u8_to_f(C + 0x10, tev + 0x98);
    buoyflag_scaleUniformColor(
        C + 0x10,
        static_cast<f32>(buoyflag_view<BuoyLightingParameters>(gabi::ea(pk->mpTevStr.get()))->colorScale));
    if (static_cast<u8>(buoyflag_view<BuoyLightingParameters>(tev)->specialColorAlpha) != 0) {
        color_u8_to_f(M + 0x60, tev + 0x9C);
    } else {
        buoyflag_view<BuoyUniformState>(M)->initialColors[25] = 0.0f;
        buoyflag_view<BuoyUniformState>(M)->initialColors[24] = 0.0f;
        buoyflag_view<BuoyUniformState>(M)->initialColors[26] = 0.0f;
        buoyflag_view<BuoyUniformState>(M)->initialColors[27] = 0.0f;
    }
    buoyflag_flushUniformObject(M - 0x74);
    gabi::Local<Mtx34> mtx;
    mtx_to_sead(gabi::ea(mtx.get()), mtxSrc);
    PSMTXCopy(mtx.get(), buoyflag_view<Mtx34>(M - 0xA8));
    buoyflag_flushUniformObject(G + 0xC);
    (void)P;
}

// HD: upload the flag's vertices (positions and front/back normals, 62 strip vertices)
/* 0232B624 */
void Packet_c::update_hata() {

    u32 P = gabi::ea(this);
    load_texture();
    u32 V = gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.materialPool);
    for (u32 k = 0; k < 2; k++) {
        u32 sel = static_cast<u32>(buoyflag_view<BuoyMaterialPool>(V)->current);
        u32 dst = gabi::ea(buoyflag_view<BuoyGpuMaterial>(V + (k * 2 + sel) * 0x254)->vertices.get()) - 0x20;
        for (u32 n = 0; n < 62; n++) {
            dst += 0x20;
            u32 b = P + 0x9C + (u32)mCurBuf * 0x4EC;
            const u32 t = gabi::ea(&buoyflag_strip) + n * 3; /* strip table {pos, nrm, uv} */
            u32 p = b + static_cast<u8>(buoyflag_view<BuoyStripVertex>(t)->position) * 0xC;
            f32 x = static_cast<f32>(buoyflag_view<cXyz>(p)->x),
                y = static_cast<f32>(buoyflag_view<cXyz>(p)->y),
                z = static_cast<f32>(buoyflag_view<cXyz>(p)->z);
            buoyflag_view<BuoyVertex>(dst)->position.x = x;
            buoyflag_view<BuoyVertex>(dst)->position.y = y;
            buoyflag_view<BuoyVertex>(dst)->position.z = z;
            b = P + 0x9C + (u32)mCurBuf * 0x4EC + (k == 0 ? 0x1A4 : 0x348);
            u32 q = b + static_cast<u8>(buoyflag_view<BuoyStripVertex>(t)->normal) * 0xC;
            f32 nx = static_cast<f32>(buoyflag_view<cXyz>(q)->x),
                ny = static_cast<f32>(buoyflag_view<cXyz>(q)->y),
                nz = static_cast<f32>(buoyflag_view<cXyz>(q)->z);
            buoyflag_view<BuoyVertex>(dst)->normal.y = ny;
            buoyflag_view<BuoyVertex>(dst)->normal.z = nz;
            buoyflag_view<BuoyVertex>(dst)->normal.x = nx;
        }
    }
    u32 e = V + static_cast<u32>(buoyflag_view<BuoyMaterialPool>(V)->current) * 0x254 + 4;
    for (int m = 0; m < 2; m++, e += 0x4A8)
        buoyflag_flushVertexBuffer(
            e, 0, static_cast<u32>(buoyflag_view<BuoyVertexBufferView>(e)->byteLength)); /* flush */
    buoyflag_view<BuoyMaterialPool>(V)->current =
        static_cast<u32>(buoyflag_view<BuoyMaterialPool>(V)->current) == 0;
    set_tev_colors(this, gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.uniforms),
                   gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.uniformState), P + 0x180C,
                   gabi::ea(&mMtxHata));
}

// HD: upload the pole's normals (35 vertices, l_hasi_nrm by table)
/* 0232B984 */
void Packet_c::update_hasi() {

    u32 P = gabi::ea(this);
    load_texture();
    u32 V = gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.materialPool);
    u32 dst = gabi::ea(buoyflag_view<BuoyGpuMaterial>(
                           V + static_cast<u32>(buoyflag_view<BuoyPoleMaterialPool>(V)->current) * 0x254)
                           ->vertices.get()) -
              0x14;
    for (u32 n = 0; n < 35; n++) {
        u32 idx = static_cast<u8>(buoyflag_strip[n][1]);
        u32 s = gabi::ea(&buoyflag_clothStatics()->poleNormals) + idx * sizeof(cXyz);
        f32 z = static_cast<f32>(buoyflag_view<cXyz>(s)->z), x = static_cast<f32>(buoyflag_view<cXyz>(s)->x),
            y = static_cast<f32>(buoyflag_view<cXyz>(s)->y);
        dst += 0x20;
        buoyflag_view<BuoyVertex>(dst)->position.x = x;
        buoyflag_view<BuoyVertex>(dst)->position.y = y;
        buoyflag_view<BuoyVertex>(dst)->position.z = z;
    }
    u32 e = V + static_cast<u32>(buoyflag_view<BuoyPoleMaterialPool>(V)->current) * 0x254;
    buoyflag_flushVertexBuffer(
        e + 4, 0, static_cast<u32>(buoyflag_view<BuoyVertexBufferView>(e + 4)->byteLength)); /* flush */
    buoyflag_view<BuoyPoleMaterialPool>(V)->current =
        static_cast<u32>(buoyflag_view<BuoyPoleMaterialPool>(V)->current) == 0;
    set_tev_colors(this, gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.uniforms),
                   gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.uniformState), P + 0x255C,
                   gabi::ea(&mMtxHasi));
}

// HD: Packet_c::update
/* 0232BBEC */
void Packet_c::update(Act_c* a) {

    buoyflag_concatMatrix(J3DSys_viewMtx, &mMtxHasi, &mViewMtxHasi);
    if (attr_type(a, "d_a_obj_buoyflag.cpp", "(M_type >= 0) && (M_type < Type_Max)")->mHata)
        buoyflag_concatMatrix(J3DSys_viewMtx, &mMtxHata, &mViewMtxHata);
    mpTevStr = &a->tevStr;
    buoyflag_entryImm(static_cast<u32>(buoyflag_view<BuoyGraphicsSystem>(j3dSys_)->opaqueBuffer), this,
                      0); /* J3DDrawBuffer::entryImm */
    if (attr_type(a, "d_a_obj_buoyflag.cpp", "(M_type >= 0) && (M_type < Type_Max)")->mHata)
        update_hata();
    update_hasi();
}

/* ---- drawing helpers (HD engine inlines) ---- */
/* the program entry of the current pass: prog = programs[pass].shader (pass < 4) */
static inline u32 pass_program(u32 cntAddr, u32 ctx) {
    u32 prog = 0;
    s32 pass = static_cast<s32>(buoyflag_view<BuoyDrawContext>(ctx)->pass);
    if (pass < 4) {
        u32 cnt = static_cast<u32>(buoyflag_view<BuoyList>(cntAddr)->count);
        u32 e = static_cast<u32>(buoyflag_view<BuoyList>(cntAddr)->array);
        if ((u32)pass < cnt)
            e += pass * 0x14;
        prog = static_cast<u32>(buoyflag_view<BuoyShaderObject>(e)->program);
    }
    return prog;
}
/* bind the shader program (the current one is cached in the state returned by 027F29D4) */
static inline void bind_program(u32 prog) {
    u32 S = buoyflag_getActiveShader(j3dSys_);
    u32 sh = static_cast<u32>(buoyflag_view<BuoyShaderObject>(prog)->program);
    if (sh != static_cast<u32>(buoyflag_view<BuoyActiveShader>(S)->program)) {
        u8 f = static_cast<u8>(buoyflag_view<BuoyShaderProgram>(sh)->flags);
        u32 cur = static_cast<u32>(buoyflag_view<BuoyActiveShader>(S)->group);
        if (f & 2) {
            buoyflag_view<BuoyShaderProgram>(sh)->flags = (u8)(f & ~2);
            buoyflag_updateShaderProgram(sh, 0);
        }
        u32 fs = static_cast<u32>(buoyflag_view<BuoyShaderResource>(
                                      static_cast<u32>(buoyflag_view<BuoyShaderProgram>(sh)->resource))
                                      ->group);
        if (cur != fs)
            buoyflag_bindShaderGroup(fs);
        u32 dlSize = static_cast<u32>(buoyflag_view<BuoyShaderProgram>(sh)->displayListSize);
        if (dlSize != 0) {
            buoyflag_GX2CallDisplayList(static_cast<u32>(buoyflag_view<BuoyShaderProgram>(sh)->displayList),
                                        dlSize); /* GX2CallDisplayList */
            buoyflag_view<BuoyActiveShader>(S)->group = fs;
            buoyflag_view<BuoyActiveShader>(S)->program = sh;
        } else {
            buoyflag_bindShaderProgram(sh);
            buoyflag_view<BuoyActiveShader>(S)->program = sh;
            buoyflag_view<BuoyActiveShader>(S)->group = fs;
        }
    }
}
/* bind the uniform block `blk` of the shader object `shObj` to the program's vertex/geometry/pixel slots */
static inline void bind_uniform(u32 shObj, u32 prog) {
    u32 blk =
        shObj + 0x10 + static_cast<u32>(buoyflag_view<BuoyUniformObject>(shObj)->currentLocation) * 0x1C;
    u32 loc = 0;
    if (static_cast<u32>(buoyflag_view<BuoyShaderObject>(prog)->uniformCount) != 0)
        loc = static_cast<u32>(buoyflag_view<BuoyShaderObject>(prog)->uniformInfo);
    s16 vs = static_cast<s16>(buoyflag_view<BuoyShaderLocations>(loc)->vertex);
    u32 size = static_cast<u32>(buoyflag_view<BuoyUniformBinding>(blk)->size);
    u32 data = static_cast<u32>(buoyflag_view<BuoyUniformBinding>(blk)->data);
    s16 ps = static_cast<s16>(buoyflag_view<BuoyShaderLocations>(loc)->pixel);
    s16 gs = static_cast<s16>(buoyflag_view<BuoyShaderLocations>(loc)->geometry);
    if (vs == -1 && ps == -1 && gs == -1)
        return;
    /* GX2 imports (the 028FAxxx stubs) */
    if (ps != -1)
        buoyflag_GX2SetPixelUniformBlock(ps, data, size); /* GX2SetPixelUniformBlock */
    if (vs != -1)
        buoyflag_GX2SetVertexUniformBlock(vs, data, size); /* GX2SetVertexUniformBlock */
    if (gs != -1)
        buoyflag_GX2SetGeometryUniformBlock(gs, data, size); /* GX2SetGeometryUniformBlock */
}
static inline u32 prog_tex(u32 prog, u32 i) {
    if (i == 0)
        return static_cast<u32>(buoyflag_view<BuoyShaderObject>(prog)->textureCount) != 0
                   ? static_cast<u32>(buoyflag_view<BuoyShaderObject>(prog)->textures)
                   : 0;
    return static_cast<u32>(buoyflag_view<BuoyShaderObject>(prog)->textureCount) > 1
               ? static_cast<u32>(buoyflag_view<BuoyShaderObject>(prog)->textures) + 0x14
               : 0;
}
static inline void set_tex(u32 obj, u32 loc) { buoyflag_bindTexture(obj, loc + 4, -1, 0); }

/* the draw state on the stack (0x11C bytes, constructor 02750250) */

static inline void draw_state_set(u32 st, s32 pass) {
    buoyflag_view<BuoyDrawState>(st)->mode = 2;
    buoyflag_view<BuoyDrawState>(st)->alphaTest = 1;
    u32 f = static_cast<u32>(buoyflag_view<BuoyDrawState>(st)->flags);
    buoyflag_view<BuoyDrawState>(st)->compare = 4;
    buoyflag_view<BuoyDrawState>(st)->reference = 0.5f;
    buoyflag_view<BuoyDrawState>(st)->flags = (((f & ~0xFu) + 7) & ~0xF0u) + 0x10;
    buoyflag_configureRenderState(pass, st);
    buoyflag_applyRenderState(st);
}
/* draw the vertex buffer of the current set: V = the set, I = the index buffer object */
static inline void draw_vtx(u32 attrs, u32 I) {
    buoyflag_bindShader(attrs);
    u32 n = static_cast<u32>(buoyflag_view<BuoyIndexBuffer>(I)->count);
    if (n != 0)
        buoyflag_GX2DrawIndexedEx(static_cast<u32>(buoyflag_view<BuoyIndexBuffer>(I)->mode), n,
                                  static_cast<u32>(buoyflag_view<BuoyIndexBuffer>(I)->indexType),
                                  static_cast<u32>(buoyflag_view<BuoyIndexBuffer>(I)->base), 0,
                                  1); /* GX2DrawIndexedEx (import) */
}

// HD: Packet_c::draw_hata (HD): the flag, front and back faces
/* 02328FD4 */
void Packet_c::draw_hata(Act_c* ctxp) {

    u32 P = gabi::ea(this);
    u32 ctx = gabi::ea(ctxp);
    u32 prog = pass_program(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.programs), ctx);
    bind_program(prog);
    s32 pass = static_cast<s32>(buoyflag_view<BuoyDrawContext>(ctx)->pass);
    Act_c* a = mpActor;
    gabi::Local<BuoyDrawState> st;
    u32 S = gabi::ea(st.get());
    if (pass == 0) {
        vcall2C(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.materialUniform), prog);
        vcall2C(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.drawUniform), prog);
        u32 sh = static_cast<u32>(buoyflag_view<BuoyDrawContext>(ctx)->extra);
        if (sh != 0)
            bind_uniform(static_cast<u32>(buoyflag_view<BuoyMaterialExtra>(sh)->material), prog);
        set_tex(PrmAbstract(a, 1, 8) == Texture_00_e
                    ? gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.primaryTexture)
                    : gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.alternateTexture),
                prog_tex(prog, 0));
        buoyflag_initializeRenderState(S);
        buoyflag_view<BuoyDrawState>(S)->cullMode = 0;
    } else if (pass == 1) {
        bind_uniform(gabi::ea(buoyflag_view<Packet_c>(P)->flagGpu.uniforms.objects.get()), prog);
        vcall2C(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.materialUniform), prog);
        vcall2C(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.drawUniform), prog);
        set_tex(PrmAbstract(a, 1, 8) == Texture_00_e
                    ? gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.primaryTexture)
                    : gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.alternateTexture),
                prog_tex(prog, 0));
        set_tex(gabi::ea(&buoyflag_view<Packet_c>(P)->toonTexture), prog_tex(prog, 1));
        buoyflag_initializeRenderState(S);
        buoyflag_view<BuoyDrawState>(S)->cullMode = 0;
    } else {
        if (pass == 2) {
            bind_uniform(gabi::ea(buoyflag_view<Packet_c>(P)->flagGpu.uniforms.objects.get()), prog);
            vcall2C(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.materialUniform), prog);
            vcall2C(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.drawUniform), prog);
            u32 o = static_cast<u32>(buoyflag_view<BuoyDrawContext>(ctx)->alternateExtra);
            if (o != 0)
                vcall2C(o, prog);
            set_tex(PrmAbstract(a, 1, 8) == Texture_00_e
                        ? gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.primaryTexture)
                        : gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.alternateTexture),
                    prog_tex(prog, 0));
            set_tex(gabi::ea(&buoyflag_view<Packet_c>(P)->toonTexture), prog_tex(prog, 1));
            buoyflag_applyExtraMaterial(ctx, prog);
        }
        buoyflag_initializeRenderState(S);
        buoyflag_view<BuoyDrawState>(S)->cullMode = 0;
    }
    draw_state_set(S, static_cast<s32>(buoyflag_view<BuoyDrawContext>(ctx)->pass));
    /* front faces: the current vertex set's first buffer */
    {
        u32 cnt = static_cast<u32>(buoyflag_view<Packet_c>(P)->flagGpu.programs.count);
        u32 pass2 = static_cast<u32>(buoyflag_view<BuoyDrawContext>(ctx)->pass);
        u32 e = gabi::ea(buoyflag_view<Packet_c>(P)->flagGpu.programs.entries.get());
        if (pass2 < cnt)
            e += pass2 * 0x14;
        u32 sel = static_cast<u32>(buoyflag_view<Packet_c>(P)->flagGpu.materialPool.current);
        draw_vtx(gabi::ea(buoyflag_view<BuoyShaderCollection>(e + (sel == 0 ? 8 : 0))->frontShaders.get()),
                 gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.indexBuffer));
    }
    buoyflag_view<BuoyDrawState>(S)->cullMode = 1;
    buoyflag_applyRenderState(S);
    /* back faces */
    {
        u32 cnt = static_cast<u32>(buoyflag_view<Packet_c>(P)->flagGpu.programs.count);
        u32 pass2 = static_cast<u32>(buoyflag_view<BuoyDrawContext>(ctx)->pass);
        u32 sel = static_cast<u32>(buoyflag_view<Packet_c>(P)->flagGpu.materialPool.current);
        u32 e = gabi::ea(buoyflag_view<Packet_c>(P)->flagGpu.programs.entries.get());
        if (pass2 < cnt)
            e += pass2 * 0x14;
        e += (sel == 0 ? 8 : 0);
        u32 n = static_cast<u32>(buoyflag_view<BuoyShaderCollection>(e)->frontShaderCount);
        u32 attrs = gabi::ea(buoyflag_view<BuoyShaderCollection>(e)->frontShaders.get());
        if (n > 1)
            attrs += 0xF4;
        draw_vtx(attrs, gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.indexBuffer));
    }
    buoyflag_applyRenderState(j3dSys_ + 0x18C);
}

// HD: Packet_c::draw_hasi (HD): the pole
/* 023296B0 */
void Packet_c::draw_hasi(Act_c* ctxp) {

    u32 P = gabi::ea(this);
    u32 ctx = gabi::ea(ctxp);
    u32 prog = pass_program(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.programs), ctx);
    bind_program(prog);
    s32 pass = static_cast<s32>(buoyflag_view<BuoyDrawContext>(ctx)->pass);
    gabi::Local<BuoyDrawState> st;
    u32 S = gabi::ea(st.get());
    if (pass == 0) {
        vcall2C(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.materialUniform), prog);
        vcall2C(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.drawUniform), prog);
        u32 sh = static_cast<u32>(buoyflag_view<BuoyDrawContext>(ctx)->extra);
        if (sh != 0)
            bind_uniform(static_cast<u32>(buoyflag_view<BuoyMaterialExtra>(sh)->material), prog);
        set_tex(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.texture), prog_tex(prog, 0));
        set_tex(gabi::ea(&buoyflag_view<Packet_c>(P)->toonTexture), prog_tex(prog, 1));
        buoyflag_initializeRenderState(S);
        buoyflag_view<BuoyDrawState>(S)->cullMode = 0;
    } else if (pass == 1) {
        bind_uniform(gabi::ea(buoyflag_view<Packet_c>(P)->poleGpu.uniforms.objects.get()), prog);
        vcall2C(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.materialUniform), prog);
        vcall2C(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.drawUniform), prog);
        set_tex(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.texture), prog_tex(prog, 0));
        set_tex(gabi::ea(&buoyflag_view<Packet_c>(P)->toonTexture), prog_tex(prog, 1));
        buoyflag_initializeRenderState(S);
        buoyflag_view<BuoyDrawState>(S)->cullMode = 0;
    } else {
        if (pass == 2) {
            bind_uniform(gabi::ea(buoyflag_view<Packet_c>(P)->poleGpu.uniforms.objects.get()), prog);
            vcall2C(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.materialUniform), prog);
            vcall2C(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.drawUniform), prog);
            u32 o = static_cast<u32>(buoyflag_view<BuoyDrawContext>(ctx)->alternateExtra);
            if (o != 0)
                vcall2C(o, prog);
            set_tex(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.texture), prog_tex(prog, 0));
            set_tex(gabi::ea(&buoyflag_view<Packet_c>(P)->toonTexture), prog_tex(prog, 1));
            buoyflag_applyExtraMaterial(ctx, prog);
        }
        buoyflag_initializeRenderState(S);
        buoyflag_view<BuoyDrawState>(S)->cullMode = 0;
    }
    draw_state_set(S, static_cast<s32>(buoyflag_view<BuoyDrawContext>(ctx)->pass));
    {
        u32 cnt = static_cast<u32>(buoyflag_view<Packet_c>(P)->poleGpu.programs.count);
        u32 pass2 = static_cast<u32>(buoyflag_view<BuoyDrawContext>(ctx)->pass);
        u32 e = gabi::ea(buoyflag_view<Packet_c>(P)->poleGpu.programs.entries.get());
        u32 sel = static_cast<u32>(buoyflag_view<Packet_c>(P)->poleGpu.materialPool.current);
        if (pass2 < cnt)
            e += pass2 * 0x14;
        draw_vtx(gabi::ea(buoyflag_view<BuoyShaderCollection>(e + (sel == 0 ? 8 : 0))->frontShaders.get()),
                 gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.indexBuffer));
    }
    buoyflag_applyRenderState(j3dSys_ + 0x18C);
}

/* ---- GPU initialisation (HD) ---- */
#define l_heap 0x101F8B4Cu /* the GPU heap pointer */
static inline u32 gpu_alloc(u32 size, u32 align) {
    u32 h = buoyflag_getCurrentHeap(static_cast<u32>(*buoyflag_gpuHeap()));
    return gabi::call_ptr<u32>(
        static_cast<u32>(
            buoyflag_view<BuoyHeapVtable>(static_cast<u32>(buoyflag_view<BuoyHeap>(h)->vtable))->allocate),
        h, size, align);
}
/* r4 still holds the array pointer at the heap getter (it reads it) */
/* the pointer to free is read (at pa) after the heap getter */
static inline void gpu_free(u32 pa, u32 r4) {
    u32 h = buoyflag_findOwningHeap(static_cast<u32>(*buoyflag_gpuHeap()), r4);
    u32 fn = static_cast<u32>(
        buoyflag_view<BuoyHeapVtable>(static_cast<u32>(buoyflag_view<BuoyHeap>(h)->vtable))->free);
    gabi::call_ptr(fn, h, gabi::ea(buoyflag_view<gptr<void>>(pa)->get()));
}
static inline void safestring(u32 s, u32 str) {
    buoyflag_view<BuoySafeString>(s)->string = str;
    buoyflag_view<BuoySafeString>(s)->vtable = gc_tu.safeString;
}
/* the shader program list of the part, found by name ("flag_default"), into the part's list at L */
static inline void find_program(u32 P, u32 name, u32 L) {
    gabi::Local<SafeString> key;
    safestring(gabi::ea(key.get()), name);
    u32 mgr = buoyflag_getShaderArchives(P);
    s32 i =
        buoyflag_findShaderArchive(static_cast<u32>(buoyflag_view<BuoyArchiveCatalog>(mgr)->root), key.get());
    u32 e = 0;
    if (i >= 0) {
        u32 n = static_cast<u32>(buoyflag_view<BuoyArchiveCatalog>(mgr)->count);
        u32 tab = static_cast<u32>(buoyflag_view<BuoyArchiveCatalog>(mgr)->entries);
        u32 ent = (u32)i < n ? tab + i * 0x24 : tab;
        if (static_cast<u8>(buoyflag_view<BuoyArchiveEntry>(ent)->initialized) == 0) {
            u32 src = static_cast<u32>(buoyflag_view<BuoyArchiveCatalog>(mgr)->root);
            u32 m = static_cast<u32>(buoyflag_view<BuoyArchiveRoot>(src)->programCount);
            u32 ent2 = (u32)i < n ? tab + i * 0x24 : tab;
            u32 info =
                (u32)i < m ? static_cast<u32>(buoyflag_view<BuoyArchiveRoot>(src)->programs) + i * 0x84 : 0;
            buoyflag_initializeShaderArchive(ent2, info, 0);
            n = static_cast<u32>(buoyflag_view<BuoyArchiveCatalog>(mgr)->count);
            tab = static_cast<u32>(buoyflag_view<BuoyArchiveCatalog>(mgr)->entries);
        }
        e = (u32)i < n ? tab + i * 0x24 : tab;
    }
    buoyflag_createShaderCollections(L, e, 0);
}
/* release and recreate the attribute objects (0xF4 bytes, constructor 027BF734) of a program entry */
static inline void attrs_free(u32 E, u32 cntOff) {
    u32 ptr = static_cast<u32>(buoyflag_view<BuoyShaderArray>(E + cntOff)->array);
    if (ptr == 0)
        return;
    s32 cnt = static_cast<s32>(buoyflag_view<BuoyShaderArray>(E + cntOff)->count);
    for (s32 i = 0; i < cnt;) {
        u32 obj = ptr + i * 0xF4;
        gabi::call_ptr(static_cast<u32>(buoyflag_view<BuoyShaderVtable>(
                                            static_cast<u32>(buoyflag_view<BuoyShader>(obj)->vtable))
                                            ->destroy),
                       obj, 2);
        cnt = static_cast<s32>(buoyflag_view<BuoyShaderArray>(E + cntOff)->count);
        i++;
        ptr = static_cast<u32>(buoyflag_view<BuoyShaderArray>(E + cntOff)->array);
    }
    gpu_free(E + cntOff + 4, ptr);
    buoyflag_view<BuoyShaderArray>(E + cntOff)->count = 0;
    buoyflag_view<BuoyShaderArray>(E + cntOff)->array = 0;
}
static inline void dcbz_range(u32 d, u32 size) {
    u32 end = d + size;
    if (d < end) {
        do {
            u32 a = d & ~31u;
            for (u32 i = 0; i < 8; i++)
                buoyflag_view<BuoyCacheLine>(a)->words[i] = 0;
            d += 0x20;
        } while (d < end);
    }
}
/* lfs/stfs pairs; the recompiled original keeps a signalling NaN's bits here (seen at an unaligned
 * address, where the harness's float-store NaN tolerance does not apply): copy the bits */
static inline void copy_floats(u32 d, u32 s, u32 n) {
    for (u32 i = 0; i < n / sizeof(u32); i++)
        *buoyflag_view<be<u32>>(d + sizeof(u32) * i) =
            static_cast<u32>(*buoyflag_view<be<u32>>(s + sizeof(u32) * i));
}

// HD: the flag's GPU objects: programs, vertex buffers (4 x 62 vertices), textures
/* 0232BDC0 */
void Packet_c::gpu_init_hata() {

    u32 P = gabi::ea(this);
    find_program(P, gabi::guest_str("flag_default") /* "flag_default" */,
                 gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.programCount));
    buoyflag_view<Packet_c>(P)->flagGpu.materialPool.attributeCount = 0x13;
    buoyflag_view<Packet_c>(P)->flagGpu.materialPool.attributes = static_cast<void*>(&buoyflag_attributes);
    u32 V = gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.materialPool);
    for (u32 k = 0; k < 2; k++) {
        for (u32 m = 0; m < 2; m++) {
            u32 el = V + k * 0x254 + m * 0x4A8;
            u32 data = gabi::ea(buoyflag_view<BuoyGpuMaterial>(el)->vertices.get());
            if (data == 0) {
                u32 buf = el + 0x24C;
                u32 p = gpu_alloc(0x7C0, 0x40);
                if (p != 0) {
                    buoyflag_view<BuoyList>(buf)->array = p;
                    buoyflag_view<BuoyList>(buf)->count = 0x3E;
                }
                data = static_cast<u32>(buoyflag_view<BuoyList>(buf)->array);
                buoyflag_view<BuoyGpuMaterial>(el)->vertices = buoyflag_view<void>(data);
            }
            buoyflag_initializeVertexBuffer(el + 4, data, 0x3E, V + 0x954);
        }
    }
    buoyflag_view<BuoyMaterialPool>(V)->unused = 0;
    buoyflag_view<BuoyMaterialPool>(V)->initialized = 1;
    for (u32 i = 0; i < static_cast<u32>(buoyflag_view<Packet_c>(P)->flagGpu.programCount); i++) {
        u32 E = gabi::ea(buoyflag_view<Packet_c>(P)->flagGpu.programs.entries.get());
        if (i < static_cast<u32>(buoyflag_view<Packet_c>(P)->flagGpu.programs.count))
            E += i * 0x14;
        u32 hasPtr = gabi::ea(buoyflag_view<BuoyShaderCollection>(E)->frontShaders.get());
        u32 prog = static_cast<u32>(buoyflag_view<BuoyShaderObject>(E)->program);
        buoyflag_view<BuoyShaderObject>(E)->program = 0;
        (void)hasPtr;
        attrs_free(E, 4);
        attrs_free(E, 0xC);
        buoyflag_view<BuoyShaderObject>(E)->program = prog;
        for (u32 c = 4; c <= 0xC; c += 8) {
            u32 p = gpu_alloc(0x1E8, 4);
            for (u32 o = 0; o < 0x1E8; o += 0xF4)
                if (p + o != 0)
                    buoyflag_constructShader(p + o);
            if (p != 0) {
                buoyflag_view<BuoyShaderArray>(E + c)->array = p;
                buoyflag_view<BuoyShaderArray>(E + c)->count = 2;
            }
        }
        for (u32 k = 0; k < 2; k++) {
            for (u32 m = 0; m < 2; m++) {
                u32 L = E + 4 + k * 8;
                u32 a = static_cast<u32>(buoyflag_view<BuoyList>(L)->array);
                if (m < static_cast<u32>(buoyflag_view<BuoyList>(L)->count))
                    a += m * 0xF4;
                buoyflag_initializeShader(prog, a, V + 4 + k * 0x254 + m * 0x4A8, V + 0x954, 0);
            }
        }
    }
    buoyflag_createDrawMaterials(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.uniforms), 1, 0);
    buoyflag_initializeIndexBuffer(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.indexBuffer),
                                   gabi::ea(&buoyflag_indices), 4, static_cast<u32>(buoyflag_indexCount));
    buoyflag_view<Packet_c>(P)->flagGpu.indexBuffer.mode = 4;
    /* the initial vertices: l_pos by the strip table, zero normals, uv */
    u32 sel = static_cast<u32>(buoyflag_view<BuoyMaterialPool>(V)->current);
    for (u32 k = 0; k < 2; k++) {
        u32 off = (2 * k + sel) * 0x254;
        u32 d = gabi::ea(buoyflag_view<BuoyGpuMaterial>(V + off)->vertices.get());
        if (d < d + 0x7C0) {
            dcbz_range(d, 0x7C0);
            sel = static_cast<u32>(buoyflag_view<Packet_c>(P)->flagGpu.materialPool.current);
            off = (2 * k + sel) * 0x254;
        }
        u32 data = gabi::ea(buoyflag_view<BuoyGpuMaterial>(V + off)->vertices.get());
        if (0 < static_cast<u32>(buoyflag_vertexCount)) {
            u32 dst = data - 0x20;
            u32 t = gabi::ea(&buoyflag_strip) - 3;
            u32 i = 0;
            do {
                t += 3;
                u32 src = gabi::ea(&buoyflag_positions) +
                          static_cast<u8>(buoyflag_view<BuoyStripVertex>(t)->position) * 0xC;
                f32 z = static_cast<f32>(buoyflag_view<cXyz>(src)->z),
                    x = static_cast<f32>(buoyflag_view<cXyz>(src)->x),
                    y = static_cast<f32>(buoyflag_view<cXyz>(src)->y);
                dst += 0x20;
                buoyflag_view<BuoyVertex>(dst)->position.x = x;
                buoyflag_view<BuoyVertex>(dst)->position.y = y;
                buoyflag_view<BuoyVertex>(dst)->position.z = z;
                buoyflag_view<BuoyVertex>(dst)->normal.x = 0.0f;
                buoyflag_view<BuoyVertex>(dst)->normal.y = 0.0f;
                buoyflag_view<BuoyVertex>(dst)->normal.z = 0.0f;
                u32 uv = static_cast<u8>(buoyflag_view<BuoyStripVertex>(t)->texCoord) * 8;
                f32 u = static_cast<f32>(buoyflag_view<BuoyTexCoord>(gabi::ea(&buoyflag_texCoords) + uv)->u),
                    v = static_cast<f32>(buoyflag_view<BuoyTexCoord>(gabi::ea(&buoyflag_texCoords) + uv)->v);
                buoyflag_view<BuoyVertex>(dst)->u = u;
                buoyflag_view<BuoyVertex>(dst)->v = v;
                i++;
            } while (i < static_cast<u32>(buoyflag_vertexCount));
            sel = static_cast<u32>(buoyflag_view<BuoyMaterialPool>(V)->current);
        }
    }
    /* copy into the other buffers */
    u32 dset = V + (sel == 0) * 0x254;
    for (u32 k = 0; k < 2; k++, dset += 0x4A8) {
        u32 sset = V + (2 * k + sel) * 0x254;
        for (u32 n = 0; n < 62; n++)
            copy_floats(gabi::ea(buoyflag_view<BuoyGpuMaterial>(dset)->vertices.get()) + n * 0x20,
                        gabi::ea(buoyflag_view<BuoyGpuMaterial>(sset)->vertices.get()) + n * 0x20, 0x20);
        sel = static_cast<u32>(buoyflag_view<BuoyMaterialPool>(V)->current);
    }
    u32 e = V + sel * 0x254 + 4;
    for (int m = 0; m < 2; m++, e += 0x4A8)
        buoyflag_flushVertexBuffer(e, 0,
                                   static_cast<u32>(buoyflag_view<BuoyVertexBufferView>(e)->byteLength));
    buoyflag_view<BuoyMaterialPool>(V)->current =
        static_cast<u32>(buoyflag_view<BuoyMaterialPool>(V)->current) == 0;
    /* textures "k_hata01" / "k_hata02" from ProgramTexture.bfres */
    buoyflag_lockTextureManager(static_cast<u32>(*buoyflag_textureManager()));
    {
        gabi::Local<SafeString> s0, s1;
        safestring(gabi::ea(s0.get()), gabi::guest_str("ProgramTexture"));
        safestring(gabi::ea(s1.get()), gabi::guest_str("ProgramTexture.bfres"));
        u32 r =
            buoyflag_getArchiveResource(static_cast<u32>(*buoyflag_resourceManager()), s0.get(), s1.get(), 0);
        buoyflag_loadNamedTexture(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.primaryResource), r,
                                  gabi::guest_str("k_hata01"));
        gabi::Local<SafeString> s2, s3;
        safestring(gabi::ea(s2.get()), gabi::guest_str("ProgramTexture"));
        safestring(gabi::ea(s3.get()), gabi::guest_str("ProgramTexture.bfres"));
        r = buoyflag_getArchiveResource(static_cast<u32>(*buoyflag_resourceManager()), s2.get(), s3.get(), 0);
        buoyflag_loadNamedTexture(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.alternateResource), r,
                                  gabi::guest_str("k_hata02"));
    }
    buoyflag_unlockTextureManager(static_cast<u32>(*buoyflag_textureManager()));
    tex_bind(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.primaryTexture),
             gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.primaryResource));
    tex_bind(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.alternateTexture),
             gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.alternateResource));
}

// HD: the pole's GPU objects (2 x 35 vertices, texture "k_taru02")
/* 0232C670 */
void Packet_c::gpu_init_hasi() {

    u32 P = gabi::ea(this);
    find_program(P, gabi::guest_str("flag_default") /* "flag_default" */,
                 gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.programCount));
    buoyflag_view<Packet_c>(P)->poleGpu.materialPool.attributeCount = 0x13;
    buoyflag_view<Packet_c>(P)->poleGpu.materialPool.attributes = static_cast<void*>(&buoyflag_attributes);
    u32 V = gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.materialPool);
    for (u32 k = 0; k < 2; k++) {
        u32 el = V + k * 0x254;
        u32 data = gabi::ea(buoyflag_view<BuoyGpuMaterial>(el)->vertices.get());
        if (data == 0) {
            u32 buf = el + 0x24C;
            u32 p = gpu_alloc(0x460, 0x40);
            if (p != 0) {
                buoyflag_view<BuoyList>(buf)->array = p;
                buoyflag_view<BuoyList>(buf)->count = 0x23;
            }
            data = static_cast<u32>(buoyflag_view<BuoyList>(buf)->array);
            buoyflag_view<BuoyGpuMaterial>(el)->vertices = buoyflag_view<void>(data);
        }
        buoyflag_initializeVertexBuffer(el + 4, data, 0x23, V + 0x4AC);
    }
    buoyflag_view<BuoyPoleMaterialPool>(V)->unused = 0;
    buoyflag_view<BuoyPoleMaterialPool>(V)->initialized = 1;
    for (u32 i = 0; i < static_cast<u32>(buoyflag_view<Packet_c>(P)->poleGpu.programCount); i++) {
        u32 E = gabi::ea(buoyflag_view<Packet_c>(P)->poleGpu.programs.entries.get());
        if (i < static_cast<u32>(buoyflag_view<Packet_c>(P)->poleGpu.programs.count))
            E += i * 0x14;
        u32 prog = static_cast<u32>(buoyflag_view<BuoyShaderObject>(E)->program);
        buoyflag_view<BuoyShaderObject>(E)->program = 0;
        attrs_free(E, 4);
        attrs_free(E, 0xC);
        buoyflag_view<BuoyShaderObject>(E)->program = prog;
        for (u32 c = 4; c <= 0xC; c += 8) {
            u32 p = gpu_alloc(0xF4, 4);
            if (p != 0)
                buoyflag_constructShader(p);
            if (p != 0) {
                buoyflag_view<BuoyShaderArray>(E + c)->array = p;
                buoyflag_view<BuoyShaderArray>(E + c)->count = 1;
            }
        }
        for (u32 k = 0; k < 2; k++) {
            u32 L = E + 4 + k * 8;
            u32 a = static_cast<u32>(buoyflag_view<BuoyList>(L)->array);
            buoyflag_initializeShader(prog, a, V + 4 + k * 0x254, V + 0x4AC, 0);
        }
    }
    buoyflag_createDrawMaterials(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.uniforms), 1, 0);
    buoyflag_initializeIndexBuffer(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.indexBuffer),
                                   gabi::ea(&buoyflag_poleIndices), 4,
                                   static_cast<u32>(buoyflag_poleIndexCount));
    buoyflag_view<Packet_c>(P)->poleGpu.indexBuffer.mode = 4;
    u32 sel = static_cast<u32>(buoyflag_view<BuoyPoleMaterialPool>(V)->current);
    u32 cur = V + sel * 0x254;
    u32 d = gabi::ea(buoyflag_view<BuoyGpuMaterial>(cur)->vertices.get());
    if (d < d + 0x460) {
        dcbz_range(d, 0x460);
        sel = static_cast<u32>(buoyflag_view<Packet_c>(P)->poleGpu.materialPool.current);
        cur = V + sel * 0x254;
    }
    u32 data = gabi::ea(buoyflag_view<BuoyGpuMaterial>(cur)->vertices.get());
    if (0 < static_cast<u32>(buoyflag_poleVertexCount)) {
        u32 dst = data - 0x20;
        u32 t = (gabi::ea(&buoyflag_poleStrip) - 3);
        u32 i = 0;
        do {
            t += 3;
            u32 src = gabi::ea(&buoyflag_polePositions) +
                      static_cast<u8>(buoyflag_view<BuoyStripVertex>(t)->position) * 0xC; /* l_hasi_pos */
            f32 z = static_cast<f32>(buoyflag_view<cXyz>(src)->z),
                x = static_cast<f32>(buoyflag_view<cXyz>(src)->x),
                y = static_cast<f32>(buoyflag_view<cXyz>(src)->y);
            dst += 0x20;
            buoyflag_view<BuoyVertex>(dst)->position.x = x;
            buoyflag_view<BuoyVertex>(dst)->position.y = y;
            buoyflag_view<BuoyVertex>(dst)->position.z = z;
            buoyflag_view<BuoyVertex>(dst)->normal.x = 0.0f;
            buoyflag_view<BuoyVertex>(dst)->normal.y = 0.0f;
            buoyflag_view<BuoyVertex>(dst)->normal.z = 0.0f;
            u32 uv = static_cast<u8>(buoyflag_view<BuoyStripVertex>(t)->texCoord) * 8;
            f32 u = static_cast<f32>(buoyflag_view<BuoyTexCoord>(gabi::ea(&buoyflag_poleTexCoords) + uv)->u),
                v = static_cast<f32>(buoyflag_view<BuoyTexCoord>(gabi::ea(&buoyflag_poleTexCoords) + uv)->v);
            buoyflag_view<BuoyVertex>(dst)->u = u;
            buoyflag_view<BuoyVertex>(dst)->v = v;
            i++;
        } while (i < static_cast<u32>(buoyflag_poleVertexCount));
        sel = static_cast<u32>(buoyflag_view<BuoyPoleMaterialPool>(V)->current);
        cur = V + sel * 0x254;
    }
    u32 other = V + (sel == 0) * 0x254;
    for (u32 n = 0; n < 35; n++)
        copy_floats(gabi::ea(buoyflag_view<BuoyGpuMaterial>(other)->vertices.get()) + n * 0x20,
                    gabi::ea(buoyflag_view<BuoyGpuMaterial>(cur)->vertices.get()) + n * 0x20, 0x20);
    u32 e = V + static_cast<u32>(buoyflag_view<BuoyPoleMaterialPool>(V)->current) * 0x254;
    buoyflag_flushVertexBuffer(e + 4, 0,
                               static_cast<u32>(buoyflag_view<BuoyVertexBufferView>(e + 4)->byteLength));
    buoyflag_view<BuoyPoleMaterialPool>(V)->current =
        static_cast<u32>(buoyflag_view<BuoyPoleMaterialPool>(V)->current) == 0;
    buoyflag_lockTextureManager(static_cast<u32>(*buoyflag_textureManager()));
    {
        gabi::Local<SafeString> s0, s1;
        safestring(gabi::ea(s0.get()), gabi::guest_str("ProgramTexture"));
        safestring(gabi::ea(s1.get()), gabi::guest_str("ProgramTexture.bfres"));
        u32 r =
            buoyflag_getArchiveResource(static_cast<u32>(*buoyflag_resourceManager()), s0.get(), s1.get(), 0);
        buoyflag_loadNamedTexture(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.textureResource), r,
                                  gabi::guest_str("k_taru02"));
    }
    buoyflag_unlockTextureManager(static_cast<u32>(*buoyflag_textureManager()));
    tex_bind(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.texture),
             gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.textureResource));
}

/* ---- destructors (HD) ---- */
/* release the GPU memory of one vertex buffer element (0x254 bytes) */
static inline void vbuf_release(u32 el) {
    buoyflag_releaseShader(el + 0x158);
    u32 p = gabi::ea(buoyflag_view<BuoyGpuMaterial>(el)->allocation.get());
    buoyflag_view<BuoyGpuMaterial>(el)->vertices = nullptr;
    if (p != 0) {
        gpu_free(el + 0x250, p);
        buoyflag_view<BuoyGpuMaterial>(el)->allocationCount = 0;
        buoyflag_view<BuoyGpuMaterial>(el)->allocation = nullptr;
    }
}
/* release a program list {count (+0), entries (+4)} with its attribute objects */
static inline void proglist_release(u32 L) {
    u32 ptr = static_cast<u32>(buoyflag_view<BuoyList>(L)->array);
    if (ptr == 0)
        return;
    s32 cnt = static_cast<s32>(buoyflag_view<BuoyList>(L)->count);
    for (s32 i = 0; i < cnt; i++) {
        u32 E = ptr + i * 0x14;
        if (E == 0)
            continue;
        buoyflag_view<BuoyShaderCollection>(E)->program = nullptr;
        attrs_free(E, 4);
        attrs_free(E, 0xC);
        cnt = static_cast<s32>(buoyflag_view<BuoyList>(L)->count);
        ptr = static_cast<u32>(buoyflag_view<BuoyList>(L)->array);
    }
    gpu_free(L + 4, ptr);
    buoyflag_view<BuoyList>(L)->count = 0;
    buoyflag_view<BuoyList>(L)->array = 0;
}
/* the uniform blocks {count (+0), blocks (+4), 0x23C each}: two buffers each (027BEBEC) */
static inline void uniforms_release(u32 L) {
    s32 cnt = static_cast<s32>(buoyflag_view<BuoyList>(L)->count);
    for (s32 i = 0; i < cnt; i++) {
        u32 b = static_cast<u32>(buoyflag_view<BuoyList>(L)->array);
        if ((u32)i < (u32)cnt)
            b += i * 0x23C;
        for (u32 j = 0; j < 2; j++)
            buoyflag_destroyUniformBinding(b + 0x10 + j * 0x1C);
        cnt = static_cast<s32>(buoyflag_view<BuoyList>(L)->count);
    }
}
static inline void packet_dt_body(u32 P) {
    buoyflag_view<Packet_c>(P)->__vtbl = 0x10026860;
    u32 V = gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.materialPool);
    for (u32 k = 0; k < 4; k++)
        vbuf_release(V + k * 0x254);
    buoyflag_view<BuoyMaterialPool>(V)->unused = 0;
    u32 W = gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.materialPool);
    vbuf_release(W);
    vbuf_release(W + 0x254);
    buoyflag_view<BuoyPoleMaterialPool>(W)->unused = 0;
    uniforms_release(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.uniforms));
    for (u32 j = 0; j < 2; j++)
        buoyflag_destroyUniformBinding(P + 0x16C0 + j * 0x1C);
    for (u32 j = 0; j < 2; j++)
        buoyflag_destroyUniformBinding(P + 0x1768 + j * 0x1C);
    uniforms_release(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.uniforms));
    for (u32 j = 0; j < 2; j++)
        buoyflag_destroyUniformBinding(P + 0x2410 + j * 0x1C);
    for (u32 j = 0; j < 2; j++)
        buoyflag_destroyUniformBinding(P + 0x24B8 + j * 0x1C);
    /* the pole's objects */
    buoyflag_destroyTexture(gabi::ea(&buoyflag_view<Packet_c>(P)->toonTexture), 2);
    buoyflag_destroyTexture(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.texture), 2);
    buoyflag_releaseUniformObject(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.materialUniform), 0);
    buoyflag_releaseUniformObject(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.drawUniform), 0);
    buoyflag_destroyMaterialList(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.uniforms), 2);
    buoyflag_destroyIndexBuffer(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.indexBuffer), 2);
    if (W != 0) {
        vbuf_release(W);
        vbuf_release(W + 0x254);
        buoyflag_view<BuoyPoleMaterialPool>(W)->unused = 0;
        buoyflag_destroyArray(W, 2, 0x254, buoyflag_destroyPoleVertexCallback, 0, 0); /* __destroy_arr */
    }
    proglist_release(gabi::ea(&buoyflag_view<Packet_c>(P)->poleGpu.programs));
    /* the flag's objects */
    buoyflag_destroyTexture(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.alternateTexture), 2);
    buoyflag_destroyTexture(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.primaryTexture), 2);
    buoyflag_releaseUniformObject(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.materialUniform), 0);
    buoyflag_releaseUniformObject(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.drawUniform), 0);
    buoyflag_destroyMaterialList(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.uniforms), 2);
    buoyflag_destroyIndexBuffer(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.indexBuffer), 2);
    if (V != 0) {
        for (u32 k = 0; k < 4; k++)
            vbuf_release(V + k * 0x254);
        buoyflag_view<BuoyMaterialPool>(V)->unused = 0;
        buoyflag_destroyArray(V, 4, 0x254, buoyflag_destroyFlagVertexCallback, 0, 0); /* __destroy_arr */
    }
    proglist_release(gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.programs));
    buoyflag_destroyMaterialPacket(P, 0); /* J3DPacket::~J3DPacket */
}

// HD: Packet_c::~Packet_c (deleting)
/* 0232DDB4 */
static void Packet_dt(Packet_c* p, s32 flags) {

    if (p != nullptr) {
        packet_dt_body(gabi::ea(p));
        if (flags & 1)
            operator_delete(p);
    }
}

// HD: Act_c::~Act_c (deleting)
/* 0232E608 */
static void Act_dt(Act_c* a, s32 flags) {

    if (a != nullptr) {
        packet_dt_body(gabi::ea(&a->mPacket));
        dCcD_Cyl_dt(&a->mCyl, 2);
        dCcD_Stts_dt(&a->mStts, 2);
        buoyflag_destroyActor(a, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(a);
    }
}

// HD: Packet_c::draw (virtual; ctx: the HD draw context)
/* 02329C4C */
void Packet_c::draw(Act_c* ctx) {

    if (attr_type(mpActor, "d_a_obj_buoyflag.cpp", "(M_type >= 0) && (M_type < Type_Max)")->mHata)
        draw_hata(ctx);
    draw_hasi(ctx);
}

} // namespace daObjBuoyflag

using namespace daObjBuoyflag;
// clang-format off
/* WWHD glue
str 10026658 "Cloth" in 0232B3B8
str 10026660 "d_a_obj_buoyflag.cpp" in 0232B3B8
str 10026678 "tmp_img != (0)" in 0232B3B8
str 10026584 "d_a_obj_buoyflag.cpp" in 0232BBEC
str 1002659C "(M_type >= 0) && (M_type < Type_Max)" in 0232BBEC
str 10026584 "d_a_obj_buoyflag.cpp" in 02329C4C
str 1002659C "(M_type >= 0) && (M_type < Type_Max)" in 02329C4C
str 10026688 "flag_default" in 0232BDC0
str 10026698 "ProgramTexture" in 0232BDC0
str 100266A8 "ProgramTexture.bfres" in 0232BDC0
str 100266C0 "k_hata01" in 0232BDC0
str 100266CC "k_hata02" in 0232BDC0
str 100266D8 "flag_default" in 0232C670
str 100266E8 "ProgramTexture" in 0232C670
str 100266F8 "ProgramTexture.bfres" in 0232C670
str 10026710 "k_taru02" in 0232C670
*/
// clang-format on
