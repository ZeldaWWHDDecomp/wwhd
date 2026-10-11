/**
 * d_a_sail.cpp (WWHD)
 * Object - Pirate ship's sail (Tetra's ship): a 7x12 cloth driven by the wind, drawn by a custom
 * packet. HD keeps the GameCube cloth simulation (positions/normals double-buffered) but draws it
 * through HD GPU objects (vertex buffers, shader/uniform blocks, the "Cloth" texture), so
 * daSail_packet_c grew from 0x1C3C to 0x2EC0 bytes; GameCube's packet members from m1C3C on
 * (stick model, wind phase, furl state) are members of sail_class in HD.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_sail.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source. The HD engine classes are not
 * named yet: their methods use named bindings and explicit private layouts.
 */
#include "d/actor/d_a_sail.h"

namespace sail_tu {
inline constexpr u32 safeStringVtable = 0x100393F4u;
inline constexpr u32 packetVtable = 0x1003970Cu;
inline constexpr u32 sailVtable = 0x1003941Cu;
inline constexpr u32 hioVtable = 0x1003940Cu;
inline constexpr u32 graphicsSystem = 0x104B45C0u;
inline constexpr u32 drawUniformVtable = 0x1016EF84;
inline constexpr u32 materialUniformVtable = 0x1016EFB4;
inline constexpr u32 stageName = 0x1047E6B8u;
inline constexpr u32 projectionData = 0x104B470Cu;
} // namespace sail_tu

/* ---- statics ---- */
// Model data (vertex positions, texture coordinates, index lists) generated from your copy of the
// game at build time (tools/verify/assets.py, build/verify/include/assets/); never committed.
#include "assets/l_pos__d_a_sail.h"
#include "assets/l_texCoord__d_a_sail.h"
#include "assets/l_sail_idx__d_a_sail.h"
static u32 l_vertexAttributes[3] = {0, 12, 24};

/* daObjPirateship::Act_c members read here */
static inline u32 ship() { return (u32)sail_shipAddress(); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void cXyz_outprod(const cXyz* a, cXyz* res, const cXyz* b) { sail_vectorCross(a, res, b); }
static inline void cXyz_normZC(const cXyz* a, cXyz* res) { sail_vectorNormalize(a, res); }
static inline cXyz* sail_dKyw_get_wind_vec() { return sail_getWindVector(); }
static inline f32 sail_dKyw_get_wind_pow() { return sail_getWindPower(); }
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return sail_addCalc(v, target, scale, maxStep, minStep);
}
/* HD: HD dKy_FirstlightVec_get: the vector by value (hidden result r3); r4 is the scratch
 * the vector is built in (the caller reads it) */
static inline void dKy_FirstlightVec_get(cXyz* res, cXyz* tmp) { sail_getLightVector(res, tmp); }
static inline void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out) { sail_vectorSubtract(a, b, out); }
static inline void DCStoreRangeNoSync(void* p, u32 size) { sail_DCStoreRangeNoSync(p, size); } /* import */
static inline void sail_concatMatrices(const Mtx34* a, const Mtx34* b, Mtx34* ab) {
    sail_matrixConcat(a, b, ab);
}
static inline void PSMTXIdentity(Mtx34* m) { sail_matrixIdentity(m); }
static inline void J3DDrawBuffer_entryImm(u32 buf, void* packet, u32 idx) { sail_entryImm(buf, packet, idx); }
static inline void vcall2C(u32 obj, u32 arg) {
    gabi::call_ptr(
        (u32)sail_view<SailMaterialVtable>((u32)sail_view<SailMaterialInterface>(obj)->vtable)->apply, obj,
        arg);
}
static inline void sail_clearBytes(u32 p, u32 n) { sail_clearStorage(p, n); }
static inline void construct_array(u32 p, u32 n, u32 size, u32 ct) { sail_construct_array(p, n, size, ct); }
static inline u32 fbits(u32 a) {
    f32 f = (f32)*sail_view<be<f32>>(a);
    u32 v;
    memcpy(&v, &f, 4);
    return v;
}
static inline u32 fbits_of(f32 f) {
    u32 v;
    memcpy(&v, &f, 4);
    return v;
}
/* fsel(a, b, c): a >= 0 ? b : c (NaN: c) */
static inline f32 fsel(f32 a, f32 b, f32 c) { return a >= 0.0f ? b : c; }

/* dKy_tevstr_c::operator= (HD, inline; as in d_a_kb.cpp): members one by one, floats through FPRs */
// HD: explicit scalar assignment matches the HD lighting object, including the overlapping state/position
// bytes.
static inline void tevstr_copy(u32 d, u32 s) {
    auto& dst = *sail_view<SailLightingCopyView>(d);
    auto& src = *sail_view<SailLightingCopyView>(s);
    auto copyLight = [](SailLightObject& out, const SailLightObject& in) {
        for (int i = 0; i < 6; i++)
            out.vectors[i] = (f32)in.vectors[i];
        for (int i = 0; i < 4; i++)
            out.color[i] = (u8)in.color[i];
        for (int i = 0; i < 8; i++)
            out.parameters[i] = (u8)in.parameters[i];
        for (int i = 0; i < 8; i++)
            out.attenuation[i] = (f32)in.attenuation[i];
    };
    copyLight(dst.primary, src.primary);
    for (int i = 0; i < 0x30; i++)
        dst.state[i] = (u8)src.state[i];
    for (int i = 0; i < 3; i++)
        sail_view<SailLightingPositionView>(d)->position[i] =
            (f32)sail_view<SailLightingPositionView>(s)->position[i];
    for (int i = 0; i < 9; i++)
        dst.flags[i] = (u8)src.flags[i];
    copyLight(dst.secondary, src.secondary);
    copyLight(dst.extra, src.extra);
}

/* ===================================================================== */
/* HD helpers of the translation unit (per-TU copies of engine inlines)  */
/* ===================================================================== */

/* HD: Mtx -> sead::Matrix34 (all twelve loaded through FPRs, then stored) */

/* 0245F420 */
static void mtx_to_sead(u32 dst, u32 src) {
    u32 v[12];
    for (int i = 0; i < 12; i++)
        v[i] = fbits(gabi::ea(&sail_view<SailMatrixWords>(src)->words[i]));
    for (int i = 0; i < 12; i++)
        sail_view<SailMatrixWords>(dst)->words[i] = v[i];
}

/* HD: GXColorS10 -> float colour (rgba / 255) */

/* 0245F4C0 */
static void color_s16_to_f(u32 dst, u32 src) {
    f32 r = (f32)(s16)sail_view<SailColorS16>(src)->r / 255.0f;
    f32 g = (f32)(s16)sail_view<SailColorS16>(src)->g / 255.0f;
    f32 b = (f32)(s16)sail_view<SailColorS16>(src)->b / 255.0f;
    f32 a = (f32)(s16)sail_view<SailColorS16>(src)->a / 255.0f;
    sail_view<SailFloatColorWords>(dst)->r = fbits_of(r);
    sail_view<SailFloatColorWords>(dst)->g = fbits_of(g);
    sail_view<SailFloatColorWords>(dst)->b = fbits_of(b);
    sail_view<SailFloatColorWords>(dst)->a = fbits_of(a);
}

/* HD: GXColor -> float colour (rgba / 255) */

/* 0245F584 */
static void color_u8_to_f(u32 dst, u32 src) {
    f32 r = (f32)(u8)sail_view<SailColorU8>(src)->r / 255.0f;
    f32 g = (f32)(u8)sail_view<SailColorU8>(src)->g / 255.0f;
    f32 b = (f32)(u8)sail_view<SailColorU8>(src)->b / 255.0f;
    f32 a = (f32)(u8)sail_view<SailColorU8>(src)->a / 255.0f;
    sail_view<SailFloatColorWords>(dst)->r = fbits_of(r);
    sail_view<SailFloatColorWords>(dst)->g = fbits_of(g);
    sail_view<SailFloatColorWords>(dst)->b = fbits_of(b);
    sail_view<SailFloatColorWords>(dst)->a = fbits_of(a);
}

/* texture T bound into sampler/texture object S: copy the image fields when they match, else rebuild */
static inline void tex_bind(u32 S, u32 T) {
    if ((u32)sail_view<SailTextureView>(S)->width == (u32)sail_view<SailTextureView>(T)->width &&
        (u32)sail_view<SailTextureView>(S)->height == (u32)sail_view<SailTextureView>(T)->height &&
        (u32)sail_view<SailTextureView>(S)->depth == (u32)sail_view<SailTextureView>(T)->depth &&
        (u32)sail_view<SailTextureView>(S)->dimension == (u32)sail_view<SailTextureView>(T)->dimension &&
        (u32)sail_view<SailTextureView>(S)->format == (u32)sail_view<SailTextureView>(T)->format &&
        (u32)sail_view<SailTextureView>(S)->mipCount == (u32)sail_view<SailTextureView>(T)->mipCount &&
        (u32)sail_view<SailTextureView>(S)->swizzle == (u32)sail_view<SailTextureView>(T)->swizzle &&
        (u32)sail_view<SailTextureView>(S)->tileMode == (u32)sail_view<SailTextureView>(T)->tileMode &&
        (u32)sail_view<SailTextureView>(S)->arrayCount == (u32)sail_view<SailTextureView>(T)->arrayCount) {
        u32 a = (u32)sail_view<SailTextureView>(T)->image;
        u32 b = (u32)sail_view<SailTextureView>(T)->mipOffset;
        sail_view<SailTextureView>(S)->duplicateImage = a;
        sail_view<SailTextureView>(S)->duplicateMipOffset = b;
        sail_view<SailTextureView>(S)->mipOffset = b;
        sail_view<SailTextureView>(S)->image = a;
    } else {
        sail_rebuildTexture(S, T);
    }
    sail_view<SailTextureView>(S)->wrapX = 2;
    sail_view<SailTextureView>(S)->wrapY = 2;
    sail_view<SailTextureView>(S)->wrapZ = 2;
    sail_view<SailTextureView>(S)->flags = (u8)((u8)sail_view<SailTextureView>(S)->flags | 2);
}

/* HD: daSail_packet_c::update (HD): load the "Cloth" texture once, upload the vertices
 * (positions with front / back normals), the tevStr colours and the matrices */

/* 0245F638 */
void daSail_packet_c::update() {
    u32 P = gabi::ea(this);
    sail_lockTextureManager((u32)sail_textureLock()); /* lock */
    if ((u8)sail_view<daSail_packet_c>(P)->textureLoaded == 0) {
        void* tmp_img = dComIfG_getObjectRes("Cloth" /* "Cloth" */, dRes_INDEX_CLOTH_BTI_TOON_e,
                                             sail_tu::safeStringVtable);
        if (tmp_img == nullptr)
            JUT_ASSERT_fail("d_a_sail.cpp", 0x2BC, "tmp_img != (0)"); /* tmp_img != (0) */
        sail_loadTexture(gabi::ea(&sail_pointer<daSail_packet_c>(P)->toonResource),
                         (u32)sail_view<SailImageResource>(gabi::ea(tmp_img))->texture);
        tex_bind(gabi::ea(&sail_pointer<daSail_packet_c>(P)->toonTexture),
                 gabi::ea(&sail_pointer<daSail_packet_c>(P)->toonResource));
        sail_view<daSail_packet_c>(P)->textureLoaded = 1;
    }
    sail_unlockTextureManager((u32)sail_textureLock()); /* unlock */
    /* the two vertex sets (front, back): position + normal, 0x20 bytes per vertex */
    u32 V = gabi::ea(&sail_pointer<daSail_packet_c>(P)->materialPool);
    for (u32 k = 0; k < 2; k++) {
        u32 dst = (u32)sail_materialOffset(V, (k * 2 + (u32)sail_view<SailMaterialPool>(V)->current) *
                                                  (u32)sizeof(SailGpuMaterial))
                      ->vertices.v -
                  0x20;
        for (u32 n = 0; n < 0x54; n++) {
            u32 p = gabi::ea(&sail_pointer<daSail_packet_c>(P)->mPos[0][0]) +
                    ((u8)sail_view<daSail_packet_c>(P)->m1C3A * 0x54 + n) * 0xC;
            f32 z = (f32)sail_view<cXyz>(p)->z, x = (f32)sail_view<cXyz>(p)->x,
                y = (f32)sail_view<cXyz>(p)->y;
            dst += 0x20;
            sail_view<SailVertex>(dst)->position.x = x;
            sail_view<SailVertex>(dst)->position.y = y;
            sail_view<SailVertex>(dst)->position.z = z;
            u32 q = gabi::ea(k == 0 ? &sail_pointer<daSail_packet_c>(P)->mNrm[0][0]
                                    : &sail_pointer<daSail_packet_c>(P)->mBackNrm[0][0]) +
                    ((u8)sail_view<daSail_packet_c>(P)->m1C3A * 0x54 + n) * 0xC;
            f32 nx = (f32)sail_view<cXyz>(q)->x, ny = (f32)sail_view<cXyz>(q)->y,
                nz = (f32)sail_view<cXyz>(q)->z;
            sail_view<SailVertex>(dst)->normal.z = nz;
            sail_view<SailVertex>(dst)->normal.x = nx;
            sail_view<SailVertex>(dst)->normal.y = ny;
        }
    }
    u32 e = V + (u32)sail_view<SailMaterialPool>(V)->current * sizeof(SailGpuMaterial) + 4;
    for (int m = 0; m < 2; m++, e += 2 * sizeof(SailGpuMaterial))
        sail_flushVertexBuffer(e, 0, (u32)sail_view<SailVertexBufferView>(e)->byteLength); /* flush */
    sail_view<SailMaterialPool>(V)->current = (u32)sail_view<SailMaterialPool>(V)->current == 0;
    /* the tevStr colours into the uniform block */
    sail_updateLighting((u32)sail_view<daSail_packet_c>(P)->mTevStr.v);
    sail_calculateLightingColors((u32)sail_view<daSail_packet_c>(P)->mTevStr.v);
    u32 G = gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniforms);
    {
        gabi::Local<Mtx34> view;
        mtx_to_sead(gabi::ea(view.get()), gabi::ea(sail_viewMatrix()));
        sail_setMaterialView(G, 0, view.get(), sail_tu::projectionData,
                             (u32)sail_projectionAddress() + 0x240);
    }
    gabi::Local<GXColor[4]> c10;
    u32 tev = (u32)sail_view<daSail_packet_c>(P)->mTevStr.v;
    u32 U = (u32)sail_view<daSail_packet_c>(P)->uniforms.objects.v;
    color_s16_to_f(gabi::ea(c10.get()), gabi::ea(&sail_pointer<SailLightingColors>(tev)->ambient));
    sail_scaleMaterialColor(
        gabi::ea(&sail_pointer<SailDrawMaterial>(U)->frontColor), c10.get(),
        (f32)sail_view<SailLightingParameters>((u32)sail_view<daSail_packet_c>(P)->mTevStr.v)->materialScale);
    color_s16_to_f(
        gabi::ea(c10.get()),
        gabi::ea(
            &sail_pointer<SailLightingExtraColor>((u32)sail_view<daSail_packet_c>(P)->mTevStr.v)->ambient));
    sail_scaleMaterialColor(
        gabi::ea(&sail_pointer<SailDrawMaterial>(U)->backColor), c10.get(),
        (f32)sail_view<SailLightingParameters>((u32)sail_view<daSail_packet_c>(P)->mTevStr.v)->extraScale);
    sail_flushDrawMaterial(G, 0);
    tev = (u32)sail_view<daSail_packet_c>(P)->mTevStr.v;
    u32 M = gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniformState);
    color_s16_to_f(gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniformState.initialColors[16]),
                   gabi::ea(&sail_pointer<SailLightingColors>(tev)->ambient));
    color_u8_to_f(gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniformState.initialColors[20]),
                  gabi::ea(&sail_pointer<SailLightingColors>(tev)->material));
    sail_scaleUniformColor(
        gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniformState.initialColors[20]),
        (f32)sail_view<SailLightingParameters>((u32)sail_view<daSail_packet_c>(P)->mTevStr.v)->colorScale);
    if ((u8)sail_view<SailLightingParameters>(tev)->specialColorAlpha != 0) {
        color_u8_to_f(M + 0x60, gabi::ea(&sail_pointer<SailLightingColors>(tev)->special));
    } else {
        sail_view<SailUniformState>(M)->initialColors[25] = 0.0f;
        sail_view<SailUniformState>(M)->initialColors[24] = 0.0f;
        sail_view<SailUniformState>(M)->initialColors[26] = 0.0f;
        sail_view<SailUniformState>(M)->initialColors[27] = 0.0f;
    }
    sail_flushUniformObject(gabi::ea(&sail_pointer<daSail_packet_c>(P)->materialUniform));
    gabi::Local<Mtx34> mtx;
    mtx_to_sead(gabi::ea(mtx.get()), gabi::ea(&sail_pointer<daSail_packet_c>(P)->mMtx));
    PSMTXCopy(mtx.get(), &sail_pointer<daSail_packet_c>(P)->modelMatrix);
    sail_flushUniformObject(gabi::ea(&sail_pointer<daSail_packet_c>(P)->drawUniform));
}

/* 0245FAAC */
static BOOL daSail_Draw(sail_class* i_this) {
    u32 sh = ship();
    if ((u8)sail_view<SailShipView>(sh)->m2CC == 0) { /* l_p_ship->m2CC */
        return FALSE;
    }
    tevstr_copy(gabi::ea(&i_this->tevStr),
                gabi::ea(&sail_pointer<SailShipView>(sh)->tevStr)); /* i_this->tevStr = l_p_ship->tevStr */
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mStickModel, &i_this->tevStr);

    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
    {
        /* HD: the sail's matrix is not multiplied by the view matrix (identity instead) */
        gabi::Local<Mtx34> ident;
        PSMTXIdentity(ident);
        sail_concatMatrices(ident, calc_mtx(), &i_this->mSailPacket.mMtx);
    }

    cXyz* vtxPos = i_this->mSailPacket.getPos();
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y + vtxPos[0x53].y, i_this->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
    sail_concatMatrices(sail_viewMatrix(), calc_mtx(), &i_this->mSailPacket.mStickMtx);
    {
        /* mStickModel->setBaseTRMtx(*calc_mtx) */
        u32 src = gabi::ea(calc_mtx());
        u32 mdl = gabi::ea(i_this->mStickModel.get());
        u32 v[12]; /* lfs/stfs copies (bits kept) */
        for (int i = 0; i < 12; i++)
            v[i] = (u32)sail_view<SailMatrixWords>(src)->words[i];
        for (int i = 0; i < 12; i++)
            sail_view<SailModelView>(mdl)->baseMatrix.words[i] = v[i];
    }

    MtxTrans(0.0f, 0.0f, 0.0f, 0);
    PSMTXCopy(calc_mtx(), &i_this->mSailPacket.mTexMtx);

    i_this->mSailPacket.mTevStr = &i_this->tevStr;

    /* HD: the opaque/translucent buffers are switched around the stick model's and the packet's entries */
    sail_view<SailGraphicsSystem>(sail_tu::graphicsSystem)->opaqueBuffer =
        (u32)sail_view<SailPlayView>(dComIfGp_ea())->stickOpaqueBuffer;
    sail_view<SailGraphicsSystem>(sail_tu::graphicsSystem)->translucentBuffer =
        (u32)sail_view<SailPlayView>(dComIfGp_ea())->stickTranslucentBuffer;
    mDoExt_modelUpdateDL(i_this->mStickModel, 0);
    J3DDrawBuffer_entryImm((u32)sail_view<SailGraphicsSystem>(sail_tu::graphicsSystem)->opaqueBuffer,
                           &i_this->mSailPacket, 0);
    sail_view<SailGraphicsSystem>(sail_tu::graphicsSystem)->opaqueBuffer =
        (u32)sail_view<SailPlayView>(dComIfGp_ea())->sailOpaqueBuffer;
    sail_view<SailGraphicsSystem>(sail_tu::graphicsSystem)->translucentBuffer =
        (u32)sail_view<SailPlayView>(dComIfGp_ea())->sailTranslucentBuffer;
    i_this->mSailPacket.update();
    return TRUE;
}

/* 0245FFC8 */
void daSail_packet_c::setCorrectNrmAngle(s16 param_0, f32 param_1) {
    s32 rnd = gabi::ftoi(cM_rndF(200.0f));
    m1C38 = (s16)(m1C38 + (rnd + 900));

    m1C34 = (s16)gabi::ftoi(300.0f * cM_ssin(m1C38));

    s16 r28 = (s16)(param_0 + 0x8000);
    s32 r27 = param_0;

    s16 r26 = (s16)gabi::ftoi(l_HIO().m10 * gabi::fnmsubs(0.5f, param_1, 1.0f));
    f32 f9 = (f32)r26;
    s16 lim = (s16)gabi::ftoi(f9 * 1.25f * 182.04445f);

    if (abs(r28) < lim) {
        s16 targetAngle =
            r28 > 0 ? (s16)gabi::ftoi((f32)(-r26) * 182.04445f) : (s16)gabi::ftoi(f9 * 182.04445f);
        cLib_addCalcAngleS2(&m1C36, targetAngle, 5, 192);
    } else if (abs(r27) < lim) {
        s16 targetAngle =
            (s16)r27 > 0 ? (s16)gabi::ftoi((f32)(-r26) * 182.04445f) : (s16)gabi::ftoi(f9 * 182.04445f);
        cLib_addCalcAngleS2(&m1C36, targetAngle, 5, 192);
    } else {
        cLib_addCalcAngleS2(&m1C36, 0, 5, 192);
    }
    m1C34 = (s16)(m1C34 + m1C36);
}

/* 024601F0 */
void daSail_packet_c::setNrmMtx() { cMtx_YrotS(calc_mtx(), m1C34); }

/* 024605DC */
void daSail_packet_c::setBackNrm() {
    cXyz* nrm = mNrm[(u8)m1C3A];
    cXyz* backNrm = mBackNrm[(u8)m1C3A];
    for (int i = 0; i < 0x54; i++) {
        backNrm->x = 0.0f;
        backNrm->y = 0.0f;
        backNrm->z = 0.0f;
        PSVECSubtract(backNrm, nrm, backNrm); /* *backNrm -= *nrm */
        nrm++;
        backNrm++;
    }
}

/* the normal of the face (a, b) at the vertex: normZC(a x b), added to sum */
static inline void add_face_nrm(gabi::Local<cXyz>& sum, gabi::Local<cXyz>& a, gabi::Local<cXyz>& b,
                                gabi::Local<cXyz>& spE0) {
    gabi::Local<cXyz> tmp;
    cXyz_outprod(a, tmp, b);
    sail_words(spE0.get()).x = (u32)sail_words(tmp.get()).x;
    sail_words(spE0.get()).y = (u32)sail_words(tmp.get()).y;
    sail_words(spE0.get()).z = (u32)sail_words(tmp.get()).z;
    cXyz_normZC(spE0, tmp);
    sail_words(spE0.get()).x = (u32)sail_words(tmp.get()).x;
    sail_words(spE0.get()).y = (u32)sail_words(tmp.get()).y;
    sail_words(spE0.get()).z = (u32)sail_words(tmp.get()).z;
    PSVECAdd(sum, spE0, sum);
}
/* dst = vtx - spC8 (cXyz::operator- into a temporary, copied) */
static inline void vec_sub(gabi::Local<cXyz>& dst, cXyz* vtx, gabi::Local<cXyz>& spC8) {
    gabi::Local<cXyz> tmp;
    cXyz_mi(vtx, tmp, spC8);
    sail_words(dst.get()).x = (u32)sail_words(tmp.get()).x;
    sail_words(dst.get()).y = (u32)sail_words(tmp.get()).y;
    sail_words(dst.get()).z = (u32)sail_words(tmp.get()).z;
}

/* 02460200 */
void daSail_packet_c::setNrmVtx(cXyz* param_0, int param_1, int param_2) {
    gabi::Local<cXyz> spF8;
    gabi::Local<cXyz> spEC;
    gabi::Local<cXyz> spE0;
    gabi::Local<cXyz> spD4;
    gabi::Local<cXyz> spC8;
    cXyz* vtxPos = getPos();
    s32 idx = param_2 * 7;
    spC8->x = vtxPos[param_1 + idx].x;
    spC8->y = vtxPos[param_1 + idx].y;
    spC8->z = vtxPos[param_1 + idx].z;
    spD4->z = 0.0f;
    spD4->x = 0.0f;
    spD4->y = 0.0f;

    if (param_1 != 0) {
        vec_sub(spF8, &vtxPos[idx - 1 + param_1], spC8);
        if (param_2 != 0) {
            vec_sub(spEC, &vtxPos[(param_2 - 1) * 7 + param_1], spC8);
            add_face_nrm(spD4, spEC, spF8, spE0);
        }
        if (param_2 != 11) {
            vec_sub(spEC, &vtxPos[(param_2 + 1) * 7 + param_1], spC8);
            add_face_nrm(spD4, spF8, spEC, spE0);
        }
    }
    if (param_1 != 6) {
        vec_sub(spF8, &vtxPos[param_1 + 1 + idx], spC8);
        if (param_2 != 0) {
            vec_sub(spEC, &vtxPos[(param_2 - 1) * 7 + param_1], spC8);
            add_face_nrm(spD4, spF8, spEC, spE0);
        }
        if (param_2 != 11) {
            vec_sub(spEC, &vtxPos[(param_2 + 1) * 7 + param_1], spC8);
            add_face_nrm(spD4, spEC, spF8, spE0);
        }
    }

    {
        gabi::Local<cXyz> tmp;
        cXyz_normZC(spD4, tmp);
        sail_words(spD4.get()).x = (u32)sail_words(tmp.get()).x;
        sail_words(spD4.get()).y = (u32)sail_words(tmp.get()).y;
        sail_words(spD4.get()).z = (u32)sail_words(tmp.get()).z;
    }

    MtxPush();
    cMtx_YrotM(calc_mtx(), (s16)gabi::ftoi(cM_ssin(-800 * (param_1 + param_2)) * 900.0f));
    MtxPosition(spD4, spE0);
    {
        gabi::Local<cXyz> tmp;
        cXyz_normZC(spE0, tmp);
        sail_words(param_0).x = (u32)sail_words(tmp.get()).x;
        sail_words(param_0).y = (u32)sail_words(tmp.get()).y;
        sail_words(param_0).z = (u32)sail_words(tmp.get()).z;
    }
    MtxPull();
}

/* HD: sail_move (sail_pos_move and demo_move inlined).
 * HD: the wind phase speed is 4000 (GameCube 0.8 * 5000), and in stage "Demo46" (play+0x5292 set)
 * the flutter amplitudes follow the wind power; the furl bulge uses 700/150 (GameCube 3.5*200 /
 * 1.5*100, the same) */

/* 02460678 */
static void sail_move(sail_class* i_this) {
    cXyz* windVec = sail_dKyw_get_wind_vec();
    if (l_HIO().m06 == 0) {
        if (i_this->m1C44 < 0.6f) {
            if (!(i_this->m1C48 > 0.0f)) {
                i_this->m1C48 = 0.015f;
            } else {
                i_this->m1C48 = i_this->m1C48 - 0.001f;
            }
            cLib_addCalc(&i_this->m1C44, 0.6f, 0.1f, i_this->m1C48, 0.01f);
        } else {
            i_this->m1C48 = 0.0f;
        }
    } else if (l_HIO().m06 == 1 && i_this->m1C44 > 0.0f) {
        i_this->m1C48 = i_this->m1C48 + 0.0075f;
        cLib_addCalc(&i_this->m1C44, 0.0f, 0.3f, i_this->m1C48, 0.01f);
    } else {
        i_this->m1C48 = 0.0f;
    }

    if (i_this->m1C44 > 1.0f) {
        i_this->m1C44 = 1.0f;
    } else if (i_this->m1C44 < 0.0f) {
        i_this->m1C44 = 0.0f;
    }

    /* demo_move (inline) */
    BOOL demo = FALSE;
    u8 demoId = i_this->demoActorID;
    if (demoId != 0 && demoId <= 0x20) {
        /* dComIfGp_demo_getActor(id) */
        u32 obj = (u32)sail_demoManager();
        if (obj == 0) {
            JUT_ASSERT_fail("d_demo.h" /* "d_demo.h" */, 0x23A, "m_object != (0)" /* "m_object != (0)" */);
            obj = (u32)sail_demoManager();
        }
        u32 demo_actor = sail_getDemoActor(obj, demoId);
        if (demo_actor != 0) {
            if ((u16)sail_view<SailDemoActorView>(demo_actor)->enabled &
                0x40) { /* checkEnable(ENABLE_ANM_FRAME_e) */
                f32 frame = gabi::fnmsubs((f32)sail_view<SailDemoActorView>(demo_actor)->animationFrame,
                                          0.006f, 0.6f);
                /* cLib_minMaxLimit<f32>(frame, 0.0f, 0.6f) */
                if (frame < 0.0f) {
                    i_this->m1C44 = 0.0f;
                } else {
                    i_this->m1C44 = fsel(frame - 0.6f, 0.6f, frame);
                }
            }
            demo = TRUE;
        }
    }
    if (!demo && l_HIO().m08 != 0) {
        i_this->m1C44 = l_HIO().m0C;
    }
    i_this->m1C4C = 15.0f * (i_this->m1C44 - 0.6f);

    s16 windAngle = cM_atan2s(windVec->x, windVec->z);
    cMtx_YrotS(calc_mtx(), (s16) - (i_this->current.angle.y - windAngle));
    gabi::Local<cXyz> sp2C;
    gabi::Local<cXyz> sp20;
    sp2C->x = 0.0f;
    sp2C->y = 0.0f;
    sp2C->z = 0.064f; /* 0.08f * 0.8f */
    MtxPosition(sp2C, sp20);
    sp2C->x = 0.0f;
    sp2C->z = 1.0f;
    MtxPosition(sp2C, sp20);
    f32 f31_1 = fabsf(sp20->z);

    /* sail_pos_move (inline) */
    {
        cXyz* windVec2 = sail_dKyw_get_wind_vec();
        s16 windAngle2 = cM_atan2s(windVec2->x, windVec2->z);
        cMtx_YrotS(calc_mtx(), (s16) - (i_this->current.angle.y - windAngle2));
        gabi::Local<cXyz> sp28;
        gabi::Local<cXyz> sp1C;
        sp28->y = 0.0f;
        sp28->z = 0.064f;
        sp28->x = 0.0f;
        MtxPosition(sp28, sp1C);
        f32 f31 =
            gabi::fmadds(0.9f, fabsf(sp1C->z), 0.02f) +
            0x1.a36e3p-8f; /* 0.9|z| + 0.02 + f14 * 0.1 (0.064f * 0.1f, not the nearest float to 0.0064) */
        sp28->z = 1.0f;
        sp28->x = 0.0f;
        MtxPosition(sp28, sp1C);
        f32 f25 = fabsf(sp1C->z);
        f32 f0 = fabsf(sp1C->x);
        s32 tmp = gabi::ftoi(4000.0f * gabi::fnmsubs(0.95f, i_this->m1C44, 1.0f));
        f25 = gabi::fmadds(0.9f, f25, 0.1f);
        /* HD: in "Demo46" the wind power scales the flutter */
        if ((u8)sail_view<SailPlayView>(dComIfGp_ea())->demoRunning != 0) {
            gabi::Local<SafeString> s1;
            gabi::Local<SafeString> s0;
            s1->__vtbl = sail_tu::safeStringVtable;
            s1->mStringTop = sail_tu::stageName;        /* the stage name */
            s0->mStringTop = gabi::guest_str("Demo46"); /* "Demo46" */
            s0->__vtbl = sail_tu::safeStringVtable;
            sail_assureStringTermination(s0.get());
            gabi::call_ptr((u32)sail_view<SailStringVtable>(s0->__vtbl)->assureTermination,
                           s0.get()); /* assureTermination */
            u32 a = s0->mStringTop;
            gabi::call_ptr((u32)sail_view<SailStringVtable>(s1->__vtbl)->assureTermination, s1.get());
            u32 b = s1->mStringTop;
            BOOL eq = FALSE;
            if (a == b) {
                eq = TRUE;
            } else {
                for (u32 n = 0; n < 0x40001; n++) {
                    u8 ca = (u8)sail_pointer<be<u8>>(a)[n];
                    u8 cb = (u8)sail_pointer<be<u8>>(b)[n];
                    if (ca != cb)
                        break;
                    if (ca == 0) {
                        eq = TRUE;
                        break;
                    }
                }
            }
            if (eq) {
                f25 = f25 * sail_dKyw_get_wind_pow();
                f0 = gabi::fmadds(sail_dKyw_get_wind_pow(), 0.2f, f0);
            }
        }

        i_this->m1C40 =
            gabi::ftoi(gabi::fmadds((f32)tmp, gabi::fmadds(0.85f, f25, 0.15f), (f32)(s32)i_this->m1C40));
        i_this->m1C50 = (s16)gabi::ftoi(7500.0f * gabi::fmadds(1.5f, f0, 1.0f));
        i_this->m1C52 = (s16)gabi::ftoi(7200.0f * gabi::fmadds(0.95f, f25, 0.05f));

        u8 cur = (u8)i_this->mSailPacket.m1C3A;
        f32 sp6C[7] = {};
        f32 sp34[7] = {};

        f32 a44 = i_this->m1C44;
        f0 = f0 * gabi::fnmsubs(0.75f, a44, 1.0f);
        f25 = f25 * gabi::fnmsubs(0.5f * a44, i_this->m1C4C, 1.0f);
        f32 f0sq = f0 * f0;
        f32 cosAmp = 550.0f * (f0sq + 0.1f);
        f32 sinAmp = 1750.0f * (f0sq + 0.05f);

        u32 vtx = gabi::ea(&i_this->mSailPacket.mPos[0][0]) + cur * sizeof(cXyz[84]);
        for (int i = 0; i < 12; i++) {
            f32 f12 = (f32)(i - 6);
            f32 fi = (f32)i;
            s32 i12 = gabi::ftoi(f12);
            f32 spz = f25 * gabi::fmadds(-11.0f * f12, f12, 396.0f); /* sp10.z */
            f32 f13 = fi - 5.5f;
            f32 i033 = 0.33f * fi;
            f32 f17 = 5.0f * fi;
            f32 f15_2 = (f32)(i - 3);
            f32 f13sq = f13 * f13;
            f32 i125 = fi * 1.25f;
            for (int j = 0; j < 7; j++, vtx += 0xC) {
                f32 f23 = (f32)(j - 3);
                s32 m50 = i_this->m1C50;
                s32 m52 = i_this->m1C52;
                u32 r22 = (u32)(m50 * gabi::ftoi(f23) + m52 * i12);
                s32 m40 = i_this->m1C40;
                f32 f20 = sinAmp * cM_ssin((s32)(r22 - m40)) * f31;
                f32 f22 = cosAmp * cM_scos((s32)(m40 + r22)) * f31;
                f32 f21 = std_sqrtf(gabi::fmadds(f22, f22, f20 * f20));
                f32 f23sq = f23 * f23;
                f32 f24 = gabi::fmadds(f25, gabi::fmsubs(10.0f, f17, f17 * f23 * f23),
                                       (spz * (18.0f - f23sq)) / 18.0f);
                f32 prev = sp34[j];
                f32 f15 = f24 - prev;
                f21 = f21 * 0.1f;
                f32 f14;
                if (f15 > 100.0f) {
                    sp34[j] = prev + 100.0f;
                    f14 = std_sqrtf(gabi::fnmsubs(100.0f, 100.0f, 15625.0f));
                } else if (f15 < -100.0f) {
                    sp34[j] = prev - 100.0f;
                    f14 = std_sqrtf(gabi::fnmsubs(-100.0f, -100.0f, 15625.0f));
                } else {
                    sp34[j] = f24;
                    f14 = std_sqrtf(gabi::fnmsubs(f15, f15, 15625.0f));
                }
                f14 = 100.0f - f14;
                sp6C[j] = sp6C[j] + fsel(-10.0f - f14, -10.0f, f14); /* f14 > -10 ? f14 : -10 */

                f32 x, y, z;
                if (i < 3) {
                    x = f22 * i033;
                    sail_view<cXyz>(vtx)->x = x;
                    y = gabi::fnmsubs(i125, f23sq, gabi::fmadds(f21, i033, sp6C[j]));
                    z = gabi::fmadds(f20, i033, f24);
                } else {
                    if (i == 11) { /* fabsf(f23) == 3.0f */
                        f32 c = -fabsf(3.0f - fabsf(f23));
                        f20 = fsel(c, 0.0f, f20);
                        f22 = fsel(c, 0.0f, f22);
                        f21 = fsel(c, 0.0f, f21);
                    }
                    x = f22;
                    sail_view<cXyz>(vtx)->x = x;
                    y = gabi::fnmsubs(i125, f23sq, sp6C[j] + f21);
                    z = f24 + f20;
                }
                sail_view<cXyz>(vtx)->y = y;
                sail_view<cXyz>(vtx)->z = z;
                u32 lp = gabi::ea(l_pos) + (i * 7 + j) * 0xC;
                x = x + (f32)sail_view<cXyz>(lp)->x;
                sail_view<cXyz>(vtx)->x = x;
                y = y + (f32)sail_view<cXyz>(lp)->y;
                sail_view<cXyz>(vtx)->y = y;
                z = z + (f32)sail_view<cXyz>(lp)->z;
                sail_view<cXyz>(vtx)->z = z;

                y = y * (1.0f - (i_this->m1C44 * f13sq) / 30.25f);
                sail_view<cXyz>(vtx)->y = y;
                y = y * (gabi::fnmsubs(i_this->m1C44, fi, 11.0f) / 11.0f);
                sail_view<cXyz>(vtx)->y = y;
                z = z * (1.0f - (i_this->m1C44 * f13sq) / 30.25f);
                sail_view<cXyz>(vtx)->z = z;
                z = z * (gabi::fnmsubs(i_this->m1C44, fi, 11.0f) / 11.0f);
                sail_view<cXyz>(vtx)->z = z;

                f32 a = i_this->m1C44;
                if (a > 0.0f && i < 6) {
                    f32 f20b = gabi::fnmsubs(f15_2, f15_2, 9.0f) / 9.0f;
                    z = gabi::fmadds(700.0f * f20b, a, z);
                    y = (f32)sail_view<cXyz>(vtx)->y;
                    sail_view<cXyz>(vtx)->z = z;
                    y = gabi::fnmsubs(150.0f * f20b, i_this->m1C44, y);
                    sail_view<cXyz>(vtx)->y = y;
                    a = i_this->m1C44;
                } else {
                    y = (f32)sail_view<cXyz>(vtx)->y;
                }
                f32 odd = (f32)(j & 1);
                y = y * gabi::fmadds(0.3f * odd, a, 1.0f - a);
                sail_view<cXyz>(vtx)->y = y;
                a = i_this->m1C44;
                z = (f32)sail_view<cXyz>(vtx)->z * gabi::fmadds(0.15f * odd, a, 1.0f - a);
                sail_view<cXyz>(vtx)->z = z;
            }
        }
    }

    f32 f31_2 = f31_1 * gabi::fnmsubs(0.5f * i_this->m1C44, i_this->m1C4C, 1.0f);
    u32 nrm = gabi::ea(&i_this->mSailPacket.mNrm[0][0]) + (u8)i_this->mSailPacket.m1C3A * sizeof(cXyz[84]);
    s16 angleY = i_this->current.angle.y;
    gabi::Local<cXyz> light;
    gabi::Local<cXyz> light_res;
    dKy_FirstlightVec_get(light_res, light);
    s16 lightAngle = cM_atan2s(light->x, light->z);
    i_this->mSailPacket.setCorrectNrmAngle((s16)(lightAngle - angleY), f31_2);
    i_this->mSailPacket.setNrmMtx();
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 7; j++) {
            i_this->mSailPacket.setNrmVtx(sail_pointer<cXyz>(nrm), j, i);
            nrm += 0xC;
        }
    }
    i_this->mSailPacket.setBackNrm();
    /* HD: the three current buffers (GameCube USA/PAL) */
    DCStoreRangeNoSync(sail_pointer<u8>(gabi::ea(&i_this->mSailPacket.mPos[0][0]) +
                                        (u8)i_this->mSailPacket.m1C3A * sizeof(cXyz[84])),
                       sizeof(cXyz[84]));
    DCStoreRangeNoSync(sail_pointer<u8>(gabi::ea(&i_this->mSailPacket.mNrm[0][0]) +
                                        (u8)i_this->mSailPacket.m1C3A * sizeof(cXyz[84])),
                       sizeof(cXyz[84]));
    DCStoreRangeNoSync(sail_pointer<u8>(gabi::ea(&i_this->mSailPacket.mBackNrm[0][0]) +
                                        (u8)i_this->mSailPacket.m1C3A * sizeof(cXyz[84])),
                       sizeof(cXyz[84]));
}

/* 024614EC */
static BOOL daSail_Execute(sail_class* i_this) {
    /* static cXyz sail_offset(0.0f, 2100.0f, 100.0f): guard 1046D7AC, object 1046D7A0 */
    if ((u32)sail_offsetInitialized() == 0) {
        sail_offsetInitialized() = 1;
        sail_offset().z = 100.0f;
        sail_offset().x = 0.0f;
        sail_offset().y = 2100.0f;
    }
    J3DModel* mdl =
        sail_pointer<J3DModel>((u32)sail_view<SailShipView>(ship())->mModel.v); /* l_p_ship->mModel */
    PSMTXMultVec(J3DModel_getBaseTRMtx(mdl), &sail_offset(), &i_this->current.pos);
    u32 sh = ship();
    i_this->current.angle.x = (s16)sail_view<SailShipView>(sh)->shape_angle.x; /* l_p_ship->shape_angle */
    i_this->current.angle.y = (s16)sail_view<SailShipView>(sh)->shape_angle.y;
    i_this->current.angle.z = (s16)sail_view<SailShipView>(sh)->shape_angle.z;
    sail_move(i_this);
    return TRUE;
}

/* 024615A8 */
static BOOL daSail_IsDelete(sail_class* i_this) { return TRUE; }

/* 024615B0 */
static BOOL daSail_Delete(sail_class* i_this) {
    dComIfG_resDelete(&i_this->mClothPhase, "Cloth" /* "Cloth" */);
    dComIfG_resDelete(&i_this->mKaizokusenPhase, "Kaizokusen" /* "Kaizokusen" */);
    if (l_HIO().mNo >= 0) {
        mDoHIO_deleteChild(l_HIO().mNo);
        l_HIO().mNo = -1;
    }
    return TRUE;
}

/* 02461618 */
static BOOL daSail_checkCreateHeap(fopAc_ac_c* i_actor) {
    sail_class* i_this = (sail_class*)i_actor;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(
        "Kaizokusen" /* "Kaizokusen" */, dRes_INDEX_KAIZOKUSEN_BDL_AMAST_e, sail_tu::safeStringVtable);
    if (modelData == NULL) {
        return FALSE;
    }
    i_this->mStickModel = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000002);
    if (i_this->mStickModel != nullptr) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/* ===================================================================== */
/* HD packet construction / destruction / drawing                        */
/* ===================================================================== */
static inline u32 gpu_alloc(u32 size, u32 align) {
    u32 h = sail_getCurrentHeap((u32)sail_heapAddress());
    return gabi::call_ptr<u32>((u32)sail_view<SailHeapVtable>((u32)sail_view<SailHeap>(h)->vtable)->allocate,
                               h, size, align);
}
/* the heap getter is called with r4 still holding the pointer; the pointer to free is re-read at pa */
static inline void gpu_free(u32 pa, u32 r4) {
    u32 h = sail_findOwningHeap((u32)sail_heapAddress(), r4);
    u32 fn = (u32)sail_view<SailHeapVtable>((u32)sail_view<SailHeap>(h)->vtable)->free;
    gabi::call_ptr(fn, h, (u32)*sail_view<be<u32>>(pa));
}
/* GHS inline constructor of a sub-object at `p`: allocates when the address is NULL */
static inline u32 inl_ct(u32 p, u32 size) {
    if (p == 0)
        p = gabi::ea(operator_new(size));
    return p;
}
/* release the attribute objects (0xF4 bytes) of a program entry: {count (+cntOff), ptr} */
static inline void attrs_free(u32 E, u32 cntOff) {
    u32 ptr = (u32)sail_view<SailShaderArray>(E + cntOff)->array;
    if (ptr == 0)
        return;
    s32 cnt = (s32)sail_view<SailShaderArray>(E + cntOff)->count;
    for (s32 i = 0; i < cnt;) {
        u32 obj = ptr + i * sizeof(SailShader);
        gabi::call_ptr((u32)sail_view<SailShaderVtable>((u32)sail_view<SailShader>(obj)->vtable)->destroy,
                       obj, 2);
        cnt = (s32)sail_view<SailShaderArray>(E + cntOff)->count;
        i++;
        ptr = (u32)sail_view<SailShaderArray>(E + cntOff)->array;
    }
    gpu_free(E + cntOff + 4, ptr);
    sail_view<SailShaderArray>(E + cntOff)->count = 0;
    sail_view<SailShaderArray>(E + cntOff)->array = 0;
}
static inline void dcbz_range(u32 d, u32 size) {
    u32 end = d + size;
    if (d < end) {
        do {
            u32 a = d & ~31u;
            for (u32 i = 0; i < 32; i += 4)
                sail_view<SailVertexWords>(a)->words[i / 4] = 0;
            d += 0x20;
        } while (d < end);
    }
}

/* HD: constructor of the 0x254-byte vertex buffer element */

/* 024629DC */
static void* vtx_ct(void* p) {
    if (p == nullptr) {
        p = operator_new(0x254);
        if (p == nullptr)
            return p;
    }
    sail_constructVertexBuffer(gabi::ea(&sail_pointer<SailGpuMaterial>(gabi::ea(p))->vertexBuffer));
    sail_constructShader(gabi::ea(&sail_pointer<SailGpuMaterial>(gabi::ea(p))->shaderObject));
    sail_view<SailGpuMaterial>(gabi::ea(p))->allocation.v = 0;
    sail_view<SailGpuMaterial>(gabi::ea(p))->allocationCount = 0;
    return p;
}

/* HD: constructor of a trivial 0x10-byte element (allocates when NULL) */

/* 02462A38 */
static void* elem10_ct(void* p) {
    if (p == nullptr)
        p = operator_new(0x10);
    return p;
}

/* HD: deleting destructor of the 0x254-byte vertex buffer element */

/* 02462A64 */
static void vtx_dt(void* p, s32 flags) {
    if (p != nullptr) {
        sail_destroyShader(gabi::ea(&sail_pointer<SailGpuMaterial>(gabi::ea(p))->shaderObject), 2);
        sail_destroyVertexBuffer(gabi::ea(&sail_pointer<SailGpuMaterial>(gabi::ea(p))->vertexBuffer), 2);
        if (flags & 1)
            operator_delete(p);
    }
}

/* HD: daSail_packet_c::daSail_packet_c (HD: GPU objects; the GameCube members' init last) */

/* 024616AC */
static daSail_packet_c* daSail_packet_ct(daSail_packet_c* pk) {
    if (pk == nullptr) {
        pk = (daSail_packet_c*)operator_new(0x2EC0);
        if (pk == nullptr)
            return pk;
    }
    u32 P = gabi::ea(pk);
    sail_constructPacket(pk); /* J3DPacket */
    /* the program list {s32, sead::Buffer {count, ptr}} and the vertex buffers {elements[4], ...} */
    sail_view<daSail_packet_c>(P)->programCount = 0;
    sail_view<daSail_packet_c>(P)->__vtbl = sail_tu::packetVtable;
    {
        u32 b = gabi::ea(&sail_pointer<daSail_packet_c>(P)->programs);
        if (b == 0)
            b = gabi::ea(operator_new(8));
        if (b != 0) {
            sail_view<SailList>(b)->array = 0;
            sail_view<SailList>(b)->count = 0;
        }
    }
    u32 V = gabi::ea(&sail_pointer<daSail_packet_c>(P)->materialPool);
    {
        u32 v = V;
        if (v == 0)
            v = gabi::ea(operator_new(0x968));
        if (v != 0) {
            construct_array(v, 4, 0x254, gabi::guest_fn((const void*)&vtx_ct));
            sail_view<SailMaterialPool>(v)->current = 0;
            sail_view<SailMaterialPool>(v)->unused = 0;
            sail_view<SailMaterialPool>(v)->stride = 0x20;
            sail_view<SailMaterialPool>(v)->initialized = 0;
            sail_view<SailMaterialPool>(v)->attributeCount = 0;
            for (int k = 0; k < 2; k++)
                for (int m = 0; m < 2; m++)
                    sail_materialOffset(v, (k + 2 * m) * (u32)sizeof(SailGpuMaterial))->vertices.v = 0;
        }
    }
    /* the GPU state */
    sail_constructMaterialList(gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniforms));
    sail_constructUniformObject(gabi::ea(&sail_pointer<daSail_packet_c>(P)->drawUniform));
    sail_view<daSail_packet_c>(P)->drawUniform.vtable = sail_tu::drawUniformVtable;
    sail_clearBytes(gabi::ea(&sail_pointer<daSail_packet_c>(P)->modelMatrix), 0x34);
    inl_ct(gabi::ea(&sail_pointer<daSail_packet_c>(P)->modelMatrix), 0x30);
    sail_constructUniformObject(gabi::ea(&sail_pointer<daSail_packet_c>(P)->materialUniform));
    sail_view<daSail_packet_c>(P)->materialUniform.vtable = sail_tu::materialUniformVtable;
    u32 M = gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniformState);
    sail_clearBytes(M, 0x2F0);
    /* a zero cXyz, then 1.0 at every 0x10 bytes */
    for (u32 o = 0; o < 0xB0; o += 4)
        sail_view<SailUniformState>(M)->initialColors[o / 4] =
            (o >= 0xC && ((o - 0xC) & 0xF) == 0) ? 1.0f : 0.0f;
    construct_array(gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniformState.descriptors[0][0]), 2, 0x10,
                    gabi::guest_fn((const void*)&elem10_ct));
    construct_array(gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniformState.descriptors[1][0]), 2, 0x10,
                    gabi::guest_fn((const void*)&elem10_ct));
    construct_array(gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniformState.descriptors[2][0]), 2, 0x10,
                    gabi::guest_fn((const void*)&elem10_ct));
    for (u32 o = 0x110; o <= 0x260; o += 0x30)
        inl_ct(M + o, 0x30);
    for (u32 o = 0x290; o <= 0x2E0; o += 0x10)
        inl_ct(M + o, 0x10);
    sail_view<daSail_packet_c>(P)->uniformFlag = 0;
    sail_constructIndexBuffer(gabi::ea(&sail_pointer<daSail_packet_c>(P)->indexBuffer));
    sail_constructTextureResource(gabi::ea(&sail_pointer<daSail_packet_c>(P)->sailResource));
    sail_constructTextureResource(gabi::ea(&sail_pointer<daSail_packet_c>(P)->toonResource));
    sail_constructTexture(gabi::ea(&sail_pointer<daSail_packet_c>(P)->sailTexture));
    sail_constructTexture(gabi::ea(&sail_pointer<daSail_packet_c>(P)->toonTexture));
    sail_view<daSail_packet_c>(P)->textureLoaded = 0;
    /* GameCube: m1C3A = 0; m1C34 = 0; m1C38 = 0; m1C36 = 0; m1C3B = 1 */
    pk->m1C34 = 0;
    pk->m1C3B = 1;
    pk->m1C36 = 0;
    pk->m1C3A = 0;
    pk->m1C38 = 0;

    /* the shader program "flag_default" into the program list */
    {
        gabi::Local<SafeString> key;
        key->mStringTop = gabi::guest_str("flag_default"); /* "flag_default" */
        key->__vtbl = sail_tu::safeStringVtable;
        u32 mgr = sail_getShaderArchives(P);
        s32 i = sail_findShaderArchive((u32)sail_view<SailArchiveCatalog>(mgr)->root, key.get());
        u32 e = 0;
        if (i >= 0) {
            u32 n = (u32)sail_view<SailArchiveCatalog>(mgr)->count;
            u32 tab = (u32)sail_view<SailArchiveCatalog>(mgr)->entries;
            u32 ent = (u32)i < n ? tab + i * sizeof(SailArchiveEntry) : tab;
            if ((u8)sail_view<SailArchiveEntry>(ent)->initialized == 0) {
                u32 src = (u32)sail_view<SailArchiveCatalog>(mgr)->root;
                u32 m = (u32)sail_view<SailArchiveRoot>(src)->programCount;
                u32 ent2 = (u32)i < n ? tab + i * sizeof(SailArchiveEntry) : tab;
                u32 info = (u32)i < m ? (u32)sail_view<SailArchiveRoot>(src)->programs +
                                            i * sizeof(SailShaderProgram)
                                      : 0;
                sail_initializeShaderArchive(ent2, info, 0);
                n = (u32)sail_view<SailArchiveCatalog>(mgr)->count;
                tab = (u32)sail_view<SailArchiveCatalog>(mgr)->entries;
            }
            e = (u32)i < n ? tab + i * sizeof(SailArchiveEntry) : tab;
        }
        sail_createShaderCollections(gabi::ea(&sail_pointer<daSail_packet_c>(P)->programCount), e, 0);
    }
    sail_view<SailMaterialPool>(V)->attributeCount = 0x13;
    sail_view<SailMaterialPool>(V)->attributes.v = gabi::ea(l_vertexAttributes);
    /* vertex buffers: 2 x 2 sets of 0x54 vertices (0x20 bytes each) */
    for (u32 k = 0; k < 2; k++) {
        for (u32 m = 0; m < 2; m++) {
            u32 el = V + k * sizeof(SailGpuMaterial) + m * (2 * sizeof(SailGpuMaterial));
            u32 data = (u32)sail_view<SailGpuMaterial>(el)->vertices.v;
            if (data == 0) {
                u32 buf = gabi::ea(&sail_pointer<SailGpuMaterial>(el)->allocationCount);
                u32 p = gpu_alloc(0xA80, 0x40);
                if (p != 0) {
                    sail_view<SailList>(buf)->array = p;
                    sail_view<SailList>(buf)->count = 0x54;
                }
                data = (u32)sail_view<SailList>(buf)->array;
                sail_view<SailGpuMaterial>(el)->vertices.v = data;
            }
            sail_initializeVertexBuffer(gabi::ea(&sail_pointer<SailGpuMaterial>(el)->vertexBuffer), data,
                                        0x54, gabi::ea(&sail_pointer<SailMaterialPool>(V)->attributeCount));
        }
    }
    sail_view<SailMaterialPool>(V)->unused = 0;
    sail_view<SailMaterialPool>(V)->initialized = 1;
    /* per program: two attribute object pairs bound to the vertex buffers */
    for (u32 i = 0; i < (u32)sail_view<daSail_packet_c>(P)->programCount; i++) {
        u32 E = (u32)sail_view<daSail_packet_c>(P)->programs.entries.v;
        if (i < (u32)sail_view<daSail_packet_c>(P)->programs.count)
            E += i * sizeof(SailShaderCollection);
        u32 prog = (u32)sail_view<SailShaderCollection>(E)->program.v;
        sail_view<SailShaderCollection>(E)->program.v = 0;
        attrs_free(E, 4);
        attrs_free(E, 0xC);
        sail_view<SailShaderCollection>(E)->program.v = prog;
        for (u32 c = 4; c <= 0xC; c += 8) {
            u32 p = gpu_alloc(0x1E8, 4);
            for (u32 o = 0; o < 0x1E8; o += sizeof(SailShader))
                if (p + o != 0)
                    sail_constructShader(p + o);
            if (p != 0) {
                sail_view<SailShaderArray>(E + c)->array = p;
                sail_view<SailShaderArray>(E + c)->count = 2;
            }
        }
        for (u32 k = 0; k < 2; k++) {
            for (u32 m = 0; m < 2; m++) {
                u32 L = E + 4 + k * 8;
                u32 a = (u32)sail_view<SailList>(L)->array;
                if (m < (u32)sail_view<SailList>(L)->count)
                    a += m * sizeof(SailShader);
                sail_initializeShader(prog, a,
                                      V + 4 + k * sizeof(SailGpuMaterial) + m * (2 * sizeof(SailGpuMaterial)),
                                      gabi::ea(&sail_pointer<SailMaterialPool>(V)->attributeCount), 0);
            }
        }
    }
    sail_createDrawMaterials(gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniforms), 1, 0);
    sail_initializeIndexBuffer(gabi::ea(&sail_pointer<daSail_packet_c>(P)->indexBuffer), gabi::ea(l_sail_idx),
                               4, 0x9A); /* the index buffer (l_sail_idx) */
    sail_view<daSail_packet_c>(P)->indexBuffer.mode = 6;
    /* the initial vertices: l_pos, zero normals, l_texCoord */
    for (u32 k = 0; k < 2; k++) {
        u32 off = (2 * k + (u32)sail_view<SailMaterialPool>(V)->current) * sizeof(SailGpuMaterial);
        u32 d = (u32)sail_materialOffset(V, off)->vertices.v;
        if (d < d + 0xA80) {
            dcbz_range(d, 0xA80);
            off =
                (2 * k + (u32)sail_view<daSail_packet_c>(P)->materialPool.current) * sizeof(SailGpuMaterial);
        }
        u32 dst = (u32)sail_materialOffset(V, off)->vertices.v - 0x20;
        for (u32 n = 0; n < 0x54; n++) {
            u32 src = gabi::ea(l_pos) + n * 0xC;
            f32 x = (f32)sail_view<cXyz>(src)->x, y = (f32)sail_view<cXyz>(src)->y,
                z = (f32)sail_view<cXyz>(src)->z;
            dst += 0x20;
            sail_view<SailVertex>(dst)->position.x = x;
            sail_view<SailVertex>(dst)->normal.x = 0.0f;
            sail_view<SailVertex>(dst)->position.z = z;
            sail_view<SailVertex>(dst)->normal.y = 0.0f;
            sail_view<SailVertex>(dst)->position.y = y;
            sail_view<SailVertex>(dst)->normal.z = 0.0f;
            f32 u = (f32)sail_pointer<SailTexCoord>(gabi::ea(l_texCoord))[n].u,
                v = (f32)sail_pointer<SailTexCoord>(gabi::ea(l_texCoord))[n].v;
            sail_view<SailVertex>(dst)->u = u;
            sail_view<SailVertex>(dst)->v = v;
        }
    }
    /* copy into the other buffers */
    {
        u32 sel = (u32)sail_view<SailMaterialPool>(V)->current;
        u32 dset = V + (sel == 0) * sizeof(SailGpuMaterial);
        for (u32 k = 0; k < 2; k++, dset += 2 * sizeof(SailGpuMaterial)) {
            u32 sset = V + (2 * k + sel) * sizeof(SailGpuMaterial);
            for (u32 o = 0; o < 0x54 * 0x20; o += 0x20) {
                u32 s = (u32)sail_view<SailGpuMaterial>(sset)->vertices.v + o;
                u32 d = (u32)sail_view<SailGpuMaterial>(dset)->vertices.v + o;
                /* lfs/stfs pairs: the recompiled original keeps a signalling NaN's bits (seen at an
                 * unaligned buffer, where the float-store NaN tolerance does not apply): copy the bits */
                for (u32 w = 0; w < 0x20; w += 4)
                    sail_view<SailVertexWords>(d)->words[w / 4] =
                        (u32)sail_view<SailVertexWords>(s)->words[w / 4];
            }
            sel = (u32)sail_view<SailMaterialPool>(V)->current;
        }
        u32 e = V + sel * sizeof(SailGpuMaterial) + 4;
        for (int m = 0; m < 2; m++, e += 2 * sizeof(SailGpuMaterial))
            sail_flushVertexBuffer(e, 0, (u32)sail_view<SailVertexBufferView>(e)->byteLength); /* flush */
        sail_view<SailMaterialPool>(V)->current = (u32)sail_view<SailMaterialPool>(V)->current == 0;
    }
    /* the sail texture (Kaizokusen, index 0xA) */
    sail_lockTextureManager((u32)sail_textureLock());
    void* tmp_img = dComIfG_getObjectRes("Kaizokusen" /* "Kaizokusen" */, dRes_INDEX_KAIZOKUSEN_BTI_SAIL_e,
                                         sail_tu::safeStringVtable);
    if (tmp_img == nullptr)
        JUT_ASSERT_fail("d_a_sail.cpp", 0x34F, "tmp_img != (0)"); /* tmp_img != (0) */
    sail_loadTexture(gabi::ea(&sail_pointer<daSail_packet_c>(P)->sailResource),
                     (u32)sail_view<SailImageResource>(gabi::ea(tmp_img))->texture);
    sail_unlockTextureManager((u32)sail_textureLock());
    tex_bind(gabi::ea(&sail_pointer<daSail_packet_c>(P)->sailTexture),
             gabi::ea(&sail_pointer<daSail_packet_c>(P)->sailResource));
    return pk;
}

/* ---- destructors (HD) ---- */
/* release the GPU memory of one vertex buffer element (0x254 bytes) */
static inline void vbuf_release(u32 el) {
    sail_releaseShader(gabi::ea(&sail_pointer<SailGpuMaterial>(el)->shaderObject));
    u32 p = (u32)sail_view<SailGpuMaterial>(el)->allocation.v;
    sail_view<SailGpuMaterial>(el)->vertices.v = 0;
    if (p != 0) {
        gpu_free(gabi::ea(&sail_pointer<SailGpuMaterial>(el)->allocation), p);
        sail_view<SailGpuMaterial>(el)->allocationCount = 0;
        sail_view<SailGpuMaterial>(el)->allocation.v = 0;
    }
}
/* release a program list {count (+0), entries (+4)} with its attribute objects */
static inline void proglist_release(u32 L) {
    u32 ptr = (u32)sail_view<SailList>(L)->array;
    if (ptr == 0)
        return;
    s32 cnt = (s32)sail_view<SailList>(L)->count;
    for (s32 i = 0; i < cnt; i++) {
        u32 E = ptr + i * sizeof(SailShaderCollection);
        if (E == 0)
            continue;
        sail_view<SailShaderCollection>(E)->program.v = 0;
        attrs_free(E, 4);
        attrs_free(E, 0xC);
        cnt = (s32)sail_view<SailList>(L)->count;
        ptr = (u32)sail_view<SailList>(L)->array;
    }
    gpu_free(L + 4, ptr);
    sail_view<SailList>(L)->count = 0;
    sail_view<SailList>(L)->array = 0;
}
/* the uniform blocks {count (+0), blocks (+4), 0x23C each}: two buffers each (027BEBEC) */
static inline void uniforms_release(u32 L) {
    s32 cnt = (s32)sail_view<SailList>(L)->count;
    for (s32 i = 0; i < cnt; i++) {
        u32 b = (u32)sail_view<SailList>(L)->array;
        if ((u32)i < (u32)cnt)
            b += i * sizeof(SailDrawMaterial);
        for (u32 j = 0; j < 2; j++)
            sail_destroyUniformBinding(b + 0x10 + j * 0x1C);
        cnt = (s32)sail_view<SailList>(L)->count;
    }
}
static inline void packet_dt_body(u32 P) {
    sail_view<daSail_packet_c>(P)->__vtbl = sail_tu::packetVtable;
    u32 V = gabi::ea(&sail_pointer<daSail_packet_c>(P)->materialPool);
    for (u32 k = 0; k < 4; k++)
        vbuf_release(V + k * sizeof(SailGpuMaterial));
    sail_view<SailMaterialPool>(V)->unused = 0;
    uniforms_release(gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniforms));
    for (u32 j = 0; j < 2; j++)
        sail_destroyUniformBinding(gabi::ea(&sail_pointer<daSail_packet_c>(P)->drawUniform.bindings[0]) +
                                   j * 0x1C);
    for (u32 j = 0; j < 2; j++)
        sail_destroyUniformBinding(gabi::ea(&sail_pointer<daSail_packet_c>(P)->materialUniform.bindings[0]) +
                                   j * 0x1C);
    sail_destroyTexture(gabi::ea(&sail_pointer<daSail_packet_c>(P)->toonTexture), 2);
    sail_destroyTexture(gabi::ea(&sail_pointer<daSail_packet_c>(P)->sailTexture), 2);
    sail_destroyIndexBuffer(gabi::ea(&sail_pointer<daSail_packet_c>(P)->indexBuffer), 2);
    sail_releaseUniformObject(gabi::ea(&sail_pointer<daSail_packet_c>(P)->materialUniform), 0);
    sail_releaseUniformObject(gabi::ea(&sail_pointer<daSail_packet_c>(P)->drawUniform), 0);
    sail_destroyMaterialList(gabi::ea(&sail_pointer<daSail_packet_c>(P)->uniforms), 2);
    if (V != 0) {
        for (u32 k = 0; k < 4; k++)
            vbuf_release(V + k * sizeof(SailGpuMaterial));
        sail_view<SailMaterialPool>(V)->unused = 0;
        sail_destroyArray(V, 4, 0x254, gabi::guest_fn((const void*)&vtx_dt), 0, 0); /* __destroy_arr */
    }
    proglist_release(gabi::ea(&sail_pointer<daSail_packet_c>(P)->programs));
    sail_destroyMaterialPacket(P, 0); /* J3DPacket::~J3DPacket */
}

/* HD: daSail_packet_c::~daSail_packet_c (deleting) */

/* 02462AC4 */
static void daSail_packet_dt(daSail_packet_c* p, s32 flags) {
    if (p != nullptr) {
        packet_dt_body(gabi::ea(p));
        if (flags & 1)
            operator_delete(p);
    }
}

/* HD: sail_class::~sail_class (deleting) */

/* 02462EF0 */
static void sail_class_dt(sail_class* a, s32 flags) {
    if (a != nullptr) {
        packet_dt_body(gabi::ea(&a->mSailPacket));
        sail_destroyActor(a, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(a);
    }
}

/* ---- drawing (HD engine inlines) ---- */
/* bind the uniform block of the shader object `shObj` to the program's pixel/vertex/geometry slots */
static inline void bind_uniform(u32 shObj, u32 prog) {
    u32 blk = gabi::ea(&sail_pointer<SailUniformObject>(shObj)->bindings[0]) +
              (u32)sail_view<SailUniformObject>(shObj)->currentLocation * sizeof(SailUniformBindingStorage);
    u32 loc = 0;
    if ((u32)sail_view<SailShaderObject>(prog)->uniformCount != 0)
        loc = (u32)sail_view<SailShaderObject>(prog)->uniformInfo;
    s16 vs = (s16)sail_view<SailShaderLocations>(loc)->vertex;
    u32 size = (u32)sail_view<SailUniformBinding>(blk)->size;
    u32 data = (u32)sail_view<SailUniformBinding>(blk)->data;
    s16 ps = (s16)sail_view<SailShaderLocations>(loc)->pixel;
    s16 gs = (s16)sail_view<SailShaderLocations>(loc)->geometry;
    if (vs == -1 && ps == -1 && gs == -1)
        return;
    if (ps != -1)
        sail_GX2SetPixelUniformBlock(ps, data, size); /* GX2SetVertexUniformBlock */
    if (vs != -1)
        sail_GX2SetVertexUniformBlock(vs, data, size); /* GX2SetPixelUniformBlock */
    if (gs != -1)
        sail_GX2SetGeometryUniformBlock(gs, data, size); /* GX2SetGeometryUniformBlock */
}
static inline void set_tex(u32 obj, u32 loc) { sail_bindTexture(obj, loc + 4, -1, 0); }
/* draw the 11 strips (14 indices each) of the index buffer at P+0xE28 */
static inline void draw_strips(u32 P) {
    for (u32 k = 0; k < 11; k++) {
        u32 type = (u32)sail_view<daSail_packet_c>(P)->indexBuffer.indexType;
        u32 stride = (u32)sail_view<daSail_packet_c>(P)->indexBuffer.stride;
        u32 base = (u32)sail_view<daSail_packet_c>(P)->indexBuffer.base;
        u32 mode = (u32)sail_view<daSail_packet_c>(P)->indexBuffer.mode;
        sail_GX2DrawIndexedEx(mode, 0xE, type, base + stride * (k * 0xE), 0, 1); /* GX2DrawIndexedEx */
    }
}

/* HD: daSail_packet_c::draw (virtual; ctx: the HD draw context) */

/* 02462364 */
void daSail_packet_c::draw(void* ctxp) {
    u32 P = gabi::ea(this);
    u32 ctx = gabi::ea(ctxp);
    /* the program of the current pass */
    u32 prog = 0;
    {
        s32 pass = (s32)sail_view<SailDrawContext>(ctx)->pass;
        if (pass < 4) {
            u32 e = (u32)sail_view<daSail_packet_c>(P)->programs.entries.v;
            if ((u32)pass < (u32)sail_view<daSail_packet_c>(P)->programs.count)
                e += pass * sizeof(SailShaderCollection);
            prog = (u32)sail_view<SailShaderCollection>(e)->program.v;
        }
    }
    /* bind the shader program (the current one is cached in the state returned by 027F29D4) */
    {
        u32 S = sail_getActiveShader(sail_tu::graphicsSystem);
        u32 sh = (u32)sail_view<SailShaderObject>(prog)->program;
        if (sh != (u32)sail_view<SailActiveShader>(S)->program) {
            u8 f = (u8)sail_view<SailShaderProgram>(sh)->flags;
            u32 cur = (u32)sail_view<SailActiveShader>(S)->group;
            if (f & 2) {
                sail_view<SailShaderProgram>(sh)->flags = (u8)(f & ~2);
                sail_updateShaderProgram(sh, 0);
            }
            u32 fs =
                (u32)sail_view<SailShaderResource>((u32)sail_view<SailShaderProgram>(sh)->resource)->group;
            if (cur != fs)
                sail_bindShaderGroup(fs);
            u32 dlSize = (u32)sail_view<SailShaderProgram>(sh)->displayListSize;
            if (dlSize != 0) {
                sail_GX2CallDisplayList((u32)sail_view<SailShaderProgram>(sh)->displayList,
                                        dlSize); /* GX2CallDisplayList */
                sail_view<SailActiveShader>(S)->group = fs;
                sail_view<SailActiveShader>(S)->program = sh;
            } else {
                sail_bindShaderProgram(sh);
                sail_view<SailActiveShader>(S)->program = sh;
                sail_view<SailActiveShader>(S)->group = fs;
            }
        }
    }
    s32 pass = (s32)sail_view<SailDrawContext>(ctx)->pass;
    if (pass == 0) {
        vcall2C(gabi::ea(&sail_pointer<daSail_packet_c>(P)->drawUniform), prog);
        u32 sh = (u32)sail_view<SailDrawContext>(ctx)->extra;
        if (sh != 0)
            bind_uniform((u32)sail_view<SailMaterialExtra>(sh)->material, prog);
    } else if (pass == 1) {
        bind_uniform((u32)sail_view<daSail_packet_c>(P)->uniforms.objects.v, prog);
        vcall2C(gabi::ea(&sail_pointer<daSail_packet_c>(P)->materialUniform), prog);
        vcall2C(gabi::ea(&sail_pointer<daSail_packet_c>(P)->drawUniform), prog);
    } else if (pass == 2) {
        bind_uniform((u32)sail_view<daSail_packet_c>(P)->uniforms.objects.v, prog);
        vcall2C(gabi::ea(&sail_pointer<daSail_packet_c>(P)->materialUniform), prog);
        vcall2C(gabi::ea(&sail_pointer<daSail_packet_c>(P)->drawUniform), prog);
        u32 o = (u32)sail_view<SailDrawContext>(ctx)->alternateExtra;
        if (o != 0)
            vcall2C(o, prog);
        sail_applyExtraMaterial(ctx, prog);
    }
    /* the textures: sail (Kaizokusen) and the toon ramp (Cloth) */
    {
        u32 t0 = (u32)sail_view<SailShaderObject>(prog)->textureCount != 0
                     ? (u32)sail_view<SailShaderObject>(prog)->textures
                     : 0;
        set_tex(gabi::ea(&sail_pointer<daSail_packet_c>(P)->sailTexture), t0);
        u32 t1 = (u32)sail_view<SailShaderObject>(prog)->textureCount > 1
                     ? (u32)sail_view<SailShaderObject>(prog)->textures + 0x14
                     : 0;
        set_tex(gabi::ea(&sail_pointer<daSail_packet_c>(P)->toonTexture), t1);
    }
    gabi::Local<SailDrawState> st;
    u32 S = gabi::ea(st.get());
    sail_initializeRenderState(S);
    sail_view<SailDrawState>(S)->mode = 2;
    sail_view<SailDrawState>(S)->cullMode = 0;
    u32 f = (u32)sail_view<SailDrawState>(S)->flags;
    sail_view<SailDrawState>(S)->compare = 4;
    sail_view<SailDrawState>(S)->reference = 0.5f;
    sail_view<SailDrawState>(S)->alphaTest = 1;
    sail_view<SailDrawState>(S)->flags = (((f & ~0xFu) + 7) & ~0xF0u) + 0x10;
    sail_configureRenderState((s32)sail_view<SailDrawContext>(ctx)->pass, S);
    sail_applyRenderState(S);
    /* front faces */
    {
        u32 cnt = (u32)sail_view<daSail_packet_c>(P)->programs.count;
        u32 pass2 = (u32)sail_view<SailDrawContext>(ctx)->pass;
        u32 e = (u32)sail_view<daSail_packet_c>(P)->programs.entries.v;
        u32 sel = (u32)sail_view<daSail_packet_c>(P)->materialPool.current;
        if (pass2 < cnt)
            e += pass2 * sizeof(SailShaderCollection);
        sail_bindShader((u32)sail_view<SailShaderArray>(e + 4 + (sel == 0 ? 8 : 0))->array);
        draw_strips(P);
    }
    sail_view<SailDrawState>(S)->cullMode = 1;
    sail_engine2750684(S);
    /* back faces */
    {
        u32 cnt = (u32)sail_view<daSail_packet_c>(P)->programs.count;
        u32 pass2 = (u32)sail_view<SailDrawContext>(ctx)->pass;
        u32 sel = (u32)sail_view<daSail_packet_c>(P)->materialPool.current;
        u32 e = (u32)sail_view<daSail_packet_c>(P)->programs.entries.v;
        if (pass2 < cnt)
            e += pass2 * sizeof(SailShaderCollection);
        e += (sel == 0 ? 8 : 0);
        u32 n = (u32)sail_view<SailShaderCollection>(e)->frontShaderCount;
        u32 attrs = (u32)sail_view<SailShaderCollection>(e)->frontShaders.v;
        if (n > 1)
            attrs += sizeof(SailShader);
        sail_bindShader(attrs);
        draw_strips(P);
    }
    sail_applyRenderState(gabi::ea(&sail_pointer<SailGraphicsSystem>(sail_tu::graphicsSystem)->state));
}

/* 02462194 */
static cPhs_State daSail_Create(fopAc_ac_c* i_actor) {
    sail_class* i_this = (sail_class*)i_actor;
    /* fopAcM_ct(i_actor, sail_class) */
    u32 cond = i_this->actor_condition;
    if (!(cond & fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = sail_tu::sailVtable;
            daSail_packet_ct(&i_this->mSailPacket);
            cond = i_this->actor_condition;
        }
        i_this->actor_condition = cond | fopAcCnd_INIT_e;
    }

    cPhs_State rt1 = dComIfG_resLoad(&i_this->mClothPhase, "Cloth" /* "Cloth" */);
    if (rt1 != cPhs_COMPLEATE_e) {
        return rt1;
    }
    cPhs_State rt2 = dComIfG_resLoad(&i_this->mKaizokusenPhase, "Kaizokusen" /* "Kaizokusen" */);
    if (rt2 != cPhs_COMPLEATE_e) {
        return rt2;
    }

    cPhs_State phase_state = cPhs_COMPLEATE_e;
    if (fopAcM_entrySolidHeap(i_this, daSail_checkCreateHeap, 0x4C0)) {
        if (l_HIO().mNo < 0) {
            l_HIO().mNo =
                mDoHIO_createChild("Pirate Ship's Sail" // HD: translated debugger label; glue retains the
                                                        // original Japanese guest string.
                                   ,
                                   &l_HIO());
        }
        u8 cur = (u8)i_this->mSailPacket.m1C3A;
        i_this->m1C44 = 0.0f;
        i_this->m1C48 = 0.0f;
        u32 spd = gabi::ea(&i_this->mSailPacket.mPosSpd[0]);
        u32 pos = gabi::ea(&i_this->mSailPacket.mPos[0][0]) + cur * sizeof(cXyz[84]);
        for (int i = 0; i < 0x54; i++, spd += 0xC, pos += 0xC) {
            sail_view<cXyz>(spd)->x = 0.0f;
            sail_view<cXyz>(spd)->y = 0.0f;
            sail_view<cXyz>(spd)->z = 0.0f;
            sail_view<cXyz>(pos)->x = (f32)sail_pointer<cXyz>(gabi::ea(l_pos))[i].x;
            sail_view<cXyz>(pos)->y = (f32)sail_pointer<cXyz>(gabi::ea(l_pos))[i].y;
            sail_view<cXyz>(pos)->z = (f32)sail_pointer<cXyz>(gabi::ea(l_pos))[i].z;
        }

        fopAc_ac_c* p = fopAcM_SearchByID(i_this->parentActorID);
        sail_shipAddress() = gabi::ea(p);
        /* HD: null check */
        if (p != nullptr && (u8)sail_view<SailShipView>(gabi::ea(p))->m2CE == 0) { /* l_p_ship->m2CE */
            l_HIO().m06 = 0;
            i_this->m1C44 = 0.6f;
        } else {
            l_HIO().m06 = 1;
        }
        sail_move(i_this);
    } else {
        phase_state = cPhs_ERROR_e;
    }
    return phase_state;
}

/* 024628E8 */
static void __sinit_d_a_sail_cpp() {
    sail_initializeHeaderStatics();
    /* daSail_HIO_c l_HIO */
    l_HIO().__vtbl = sail_tu::hioVtable;
    l_HIO().mNo = -1;
    l_HIO().m05 = 1;
    l_HIO().m10 = 0.0f;
    l_HIO().m06 = 1;
    l_HIO().m07 = 0;
    sail_registerHio();
}

/* HD: sead::SafeString deleting destructor (trivial) */

/* 024629C8 */
static void safestring_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* HD: empty virtuals (02463348: sead::SafeString::assureTerminationImpl_) */
/* 02462EEC */
static void empty_virtual(void* p) {}
/* 02463348 */
static void safestring_assureTermination(void* p) {}

/* HD: daSail_HIO_c deleting destructor */

/* 02463320 */
static void daSail_HIO_dt(daSail_HIO_c* p, s32 flags) {
    if (p != nullptr) {
        p->mNo = -1;
        p->__vtbl = sail_tu::hioVtable;
        if (flags & 1)
            operator_delete(p);
    }
}

// clang-format off
/* WWHD glue
data l_pos 101CF71C
data l_texCoord 101CFB0C
data l_sail_idx 10039450
data l_vertexAttributes 10039700
str 100396E8 "Pirate Ship's Sail" renamed in 02462194
str 100396DC in 02462194
str 100396D4 in 02462194
str 100396C4 in 024616AC
str 100396B4 in 024616AC
str 100396A8 in 024616AC
str 1003968C in 02461618
str 10039680 in 024615B0
str 10039678 in 024615B0
str 1003942C in 02460678
str 10039444 in 02460678
str 100395A0 in 0245F638
str 10039590 in 0245F638
str 10039588 in 0245F638
str 100393EC in 02460678
str 10039698 in 024616AC
ext 0201B080 void sail_vectorCross(const cXyz *, cXyz *, const cXyz *)
ext 0201B1E4 void sail_vectorNormalize(const cXyz *, cXyz *)
ext 0257DAA8 cXyz * sail_getWindVector()
ext 02578348 float sail_getWindPower()
ext 0200ECD4 float sail_addCalc(gabi::be<float> *, float, float, float, float)
ext 0255F5FC void sail_getLightVector(cXyz *, cXyz *)
ext 028E8DAC void sail_vectorSubtract(const cXyz *, const cXyz *, cXyz *)
ext C00088B8 void sail_DCStoreRangeNoSync(void *, unsigned int)
ext 028E9108 void sail_matrixConcat(const Mtx34 *, const Mtx34 *, Mtx34 *)
ext 028E9098 void sail_matrixIdentity(Mtx34 *)
ext 027F0E04 void sail_entryImm(unsigned int, void *, unsigned int)
ext 028F521C void sail_clearStorage(unsigned int, unsigned int)
ext 028EFFD0 void sail_construct_array(unsigned int, unsigned int, unsigned int, unsigned int)
ext 027BDEB4 void sail_rebuildTexture(unsigned int, unsigned int)
ext 0274FBF8 void sail_lockTextureManager(unsigned int)
ext 02773798 void sail_loadTexture(unsigned int, unsigned int)
ext 0274FCCC void sail_unlockTextureManager(unsigned int)
ext 027B5E94 void sail_flushVertexBuffer(unsigned int, int, unsigned int)
ext 0255F8F4 void sail_updateLighting(unsigned int)
ext 0255FE90 void sail_calculateLightingColors(unsigned int)
ext 027FDA54 void sail_setMaterialView(unsigned int, int, Mtx34 *, unsigned int, unsigned int)
ext 0274D458 void sail_scaleMaterialColor(unsigned int, SailColorScratch*, float)
ext 027FDFF4 void sail_flushDrawMaterial(unsigned int, int)
ext 0274D2AC void sail_scaleUniformColor(unsigned int, float)
ext 027FB678 void sail_flushUniformObject(unsigned int)
ext 02526E70 unsigned int sail_getDemoActor(unsigned int, unsigned char)
ext 02463348 void sail_assureStringTermination(SafeString *)
ext 02756140 unsigned int sail_getCurrentHeap(unsigned int)
ext 02755FEC unsigned int sail_findOwningHeap(unsigned int, unsigned int)
ext 027B5BD8 void sail_constructVertexBuffer(unsigned int)
ext 027BF734 void sail_constructShader(unsigned int)
ext 027BF880 void sail_destroyShader(unsigned int, int)
ext 027B5CBC void sail_destroyVertexBuffer(unsigned int, int)
ext 027F1278 void sail_constructPacket(daSail_packet_c *)
ext 027FD6F4 void sail_constructMaterialList(unsigned int)
ext 027FB40C void sail_constructUniformObject(unsigned int)
ext 027B5430 void sail_constructIndexBuffer(unsigned int)
ext 027BE6B8 void sail_constructTextureResource(unsigned int)
ext 027BDF7C void sail_constructTexture(unsigned int)
ext 027FFCBC unsigned int sail_getShaderArchives(unsigned int)
ext 027B90AC int sail_findShaderArchive(unsigned int, SafeString *)
ext 02800B0C void sail_initializeShaderArchive(unsigned int, unsigned int, int)
ext 0280068C void sail_createShaderCollections(unsigned int, unsigned int, int)
ext 027FF478 void sail_initializeVertexBuffer(unsigned int, unsigned int, int, unsigned int)
ext 027FF530 void sail_initializeShader(unsigned int, unsigned int, unsigned int, unsigned int, int)
ext 027FE084 void sail_createDrawMaterials(unsigned int, int, int)
ext 027B54E0 void sail_initializeIndexBuffer(unsigned int, unsigned int, int, int)
ext 027BF7E8 void sail_releaseShader(unsigned int)
ext 027BEBEC void sail_destroyUniformBinding(unsigned int)
ext 027BE2B0 void sail_destroyTexture(unsigned int, int)
ext 027B54A0 void sail_destroyIndexBuffer(unsigned int, int)
ext 027FB528 void sail_releaseUniformObject(unsigned int, int)
ext 027FD764 void sail_destroyMaterialList(unsigned int, int)
ext 028F0164 void sail_destroyArray(unsigned int, int, int, unsigned int, int, int)
ext 027F13DC void sail_destroyMaterialPacket(unsigned int, int)
ext 025D50BC void sail_destroyActor(sail_class *, int)
ext C0006900 void sail_GX2SetPixelUniformBlock(short, unsigned int, unsigned int)
ext C0006A38 void sail_GX2SetVertexUniformBlock(short, unsigned int, unsigned int)
ext C00068A8 void sail_GX2SetGeometryUniformBlock(short, unsigned int, unsigned int)
ext 027BE53C void sail_bindTexture(unsigned int, unsigned int, int, int)
ext C0006178 void sail_GX2DrawIndexedEx(unsigned int, int, unsigned int, unsigned int, int, int)
ext 027F29D4 unsigned int sail_getActiveShader(unsigned int)
ext 027BB9E0 void sail_updateShaderProgram(unsigned int, int)
ext 027B9F68 void sail_bindShaderGroup(unsigned int)
ext C00060E0 void sail_GX2CallDisplayList(unsigned int, unsigned int)
ext 027BB7CC void sail_bindShaderProgram(unsigned int)
ext 027FFE54 void sail_applyExtraMaterial(unsigned int, unsigned int)
ext 02750250 void sail_initializeRenderState(unsigned int)
ext 0280037C void sail_configureRenderState(int, unsigned int)
ext 02750370 void sail_applyRenderState(unsigned int)
ext 027BFE5C void sail_bindShader(unsigned int)
ext 02750684 void sail_engine2750684(unsigned int)
*/
// clang-format on
