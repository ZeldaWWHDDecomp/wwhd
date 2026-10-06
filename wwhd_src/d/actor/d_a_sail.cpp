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
 * named yet: their methods are bound by address, their members by raw offset (as in
 * d_a_obj_buoyflag_draw.cpp, whose flag packet is built from the same HD pieces).
 */
#include "bindings.h"

/* ---- statics ---- */
#define SAFESTRING_VTBL 0x100393F4u
#define PACKET_VTBL 0x1003970Cu
#define SAIL_VTBL 0x1003941Cu
#define HIO_VTBL 0x1003940Cu
#define l_p_ship_ 0x1046D76Cu     /* daObjPirateship::Act_c* l_p_ship */
#define l_pos_ 0x101CF71Cu        /* cXyz l_pos[0x54] */
#define l_texCoord_ 0x101CFB0Cu   /* f32 l_texCoord[0x54][2] */
#define j3dSys_ 0x104B45C0u
#define J3DSys_viewMtx gabi::at<Mtx34>(0x104B45F8)
#define l_heap 0x101F8B4Cu        /* the GPU heap pointer */
#define l_texLock 0x101F8B18u

/* daSail_HIO_c l_HIO (HD: the vtable after the members) */
struct daSail_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ be<u8> m06;
    /* 0x03 */ be<u8> m07;
    /* 0x04 */ be<u8> m08;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<u32> __vtbl;
};
WWHD_SIZE(daSail_HIO_c, 0x14);
static daSail_HIO_c& l_HIO() { return *gabi::at<daSail_HIO_c>(0x1046D78C); }

struct daSail_packet_c {
    /* 0x0000 */ u8 _000[0xC];                 /* J3DPacket (HD, constructor 027F1278) */
    /* 0x000C */ be<u32> __vtbl;               /* 1003970C */
    /* 0x0010 */ u8 _010[0x1294 - 0x10];       /* HD GPU objects (raw offsets in the source) */
    /* 0x1294 */ Mtx34 mMtx;
    /* 0x12C4 */ Mtx34 mTexMtx;
    /* 0x12F4 */ Mtx34 mStickMtx;
    /* 0x1324 */ gptr<dKy_tevstr_c> mTevStr;
    /* 0x1328 */ cXyz mPos[2][0x54];
    /* 0x1B08 */ cXyz mPosSpd[0x54];
    /* 0x1EF8 */ cXyz mNrm[2][0x54];
    /* 0x26D8 */ cXyz mBackNrm[2][0x54];
    /* 0x2EB8 */ be<s16> m1C34;
    /* 0x2EBA */ be<s16> m1C36;
    /* 0x2EBC */ be<s16> m1C38;
    /* 0x2EBE */ be<u8> m1C3A;
    /* 0x2EBF */ be<u8> m1C3B;

    cXyz* getPos() { return mPos[(u8)m1C3A]; }
    cXyz* getNrm() { return mNrm[(u8)m1C3A]; }
    cXyz* getBackNrm() { return mBackNrm[(u8)m1C3A]; }
    void setCorrectNrmAngle(s16, f32);
    void setNrmMtx();
    void setBackNrm();
    void setNrmVtx(cXyz*, int, int);
    /* HD */
    void update();
    void draw(void* ctx);
};
WWHD_OFFSET(daSail_packet_c, mMtx, 0x1294);
WWHD_OFFSET(daSail_packet_c, mTevStr, 0x1324);
WWHD_OFFSET(daSail_packet_c, mPos, 0x1328);
WWHD_OFFSET(daSail_packet_c, mPosSpd, 0x1B08);
WWHD_OFFSET(daSail_packet_c, mNrm, 0x1EF8);
WWHD_OFFSET(daSail_packet_c, mBackNrm, 0x26D8);
WWHD_OFFSET(daSail_packet_c, m1C34, 0x2EB8);
WWHD_SIZE(daSail_packet_c, 0x2EC0);

struct sail_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mClothPhase;
    /* 0x3B4 */ request_of_phase_process_class mKaizokusenPhase;
    /* 0x3BC */ daSail_packet_c mSailPacket;
    /* 0x327C */ gptr<J3DModel> mStickModel;   /* GameCube: packet m1C3C */
    /* 0x3280 */ be<s32> m1C40;
    /* 0x3284 */ be<f32> m1C44;
    /* 0x3288 */ be<f32> m1C48;
    /* 0x328C */ be<f32> m1C4C;
    /* 0x3290 */ be<s16> m1C50;
    /* 0x3292 */ be<s16> m1C52;
};
WWHD_OFFSET(sail_class, mSailPacket, 0x3BC);
WWHD_OFFSET(sail_class, mStickModel, 0x327C);
WWHD_OFFSET(sail_class, m1C44, 0x3284);
WWHD_OFFSET(sail_class, m1C52, 0x3292);
WWHD_SIZE(sail_class, 0x3294);

/* daObjPirateship::Act_c members read here */
static inline u32 ship() { return gabi::load<u32>(l_p_ship_); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void cXyz_outprod(const cXyz* a, cXyz* res, const cXyz* b) { gabi::call(0x0201B080, a, res, b); }
static inline void cXyz_normZC(const cXyz* a, cXyz* res) { gabi::call(0x0201B1E4, a, res); }
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
static inline f32 dKyw_get_wind_pow() { return gabi::call<f32>(0x02578348); }
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* 0255F5FC HD dKy_FirstlightVec_get: the vector by value (hidden result r3); r4 is the scratch
 * the vector is built in (the caller reads it) */
static inline void dKy_FirstlightVec_get(cXyz* res, cXyz* tmp) { gabi::call(0x0255F5FC, res, tmp); }
static inline void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out) { gabi::call(0x028E8DAC, a, b, out); }
static inline void DCStoreRangeNoSync(void* p, u32 size) { gabi::call(0xC00088B8, p, size); } /* import */
static inline void PSMTXConcat_l(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline void PSMTXIdentity(Mtx34* m) { gabi::call(0x028E9098, m); }
static inline void J3DDrawBuffer_entryImm(u32 buf, void* packet, u32 idx) { gabi::call(0x027F0E04, buf, packet, idx); }
static inline void vcall2C(u32 obj, u32 arg) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(obj + 0xC) + 0x2C), obj, arg); }
static inline void bzero_l(u32 p, u32 n) { gabi::call(0x028F521C, p, n); }
static inline void construct_array(u32 p, u32 n, u32 size, u32 ct) { gabi::call(0x028EFFD0, p, n, size, ct); }
static inline u32 fbits(u32 a) {
    f32 f = gabi::load<f32>(a);
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
static inline void tevstr_copy(u32 d, u32 s) {
    auto f = [&](u32 off, int n) { for (int i = 0; i < n; i++) gabi::store<f32>(d + off + 4 * i, gabi::load<f32>(s + off + 4 * i)); };
    auto b = [&](u32 off, int n) { for (int i = 0; i < n; i++) gabi::store<u8>(d + off + i, gabi::load<u8>(s + off + i)); };
    f(0x00, 6); b(0x18, 4); b(0x1C, 8); f(0x24, 8);
    b(0x84, 0x30);
    f(0xA8, 3); b(0xB4, 9);
    f(0xC0, 6); b(0xD8, 4); b(0xDC, 8); f(0xE4, 8);
    f(0x144, 6); b(0x15C, 4); b(0x160, 8); f(0x168, 8);
}

/* ===================================================================== */
/* HD helpers of the translation unit (per-TU copies of engine inlines)  */
/* ===================================================================== */

/* 0245F420: Mtx -> sead::Matrix34 (all twelve loaded through FPRs, then stored) */
static void mtx_to_sead(u32 dst, u32 src) {
    WWHD_FUNC(0x0245F420, void, dst, src);
    u32 v[12];
    for (int i = 0; i < 12; i++) v[i] = fbits(src + 4 * i);
    for (int i = 0; i < 12; i++) gabi::store<u32>(dst + 4 * i, v[i]);
}
VERIFY(0x0245F420, mtx_to_sead);

/* 0245F4C0: GXColorS10 -> float colour (rgba / 255) */
static void color_s16_to_f(u32 dst, u32 src) {
    WWHD_FUNC(0x0245F4C0, void, dst, src);
    f32 r = (f32)gabi::load<s16>(src) / 255.0f;
    f32 g = (f32)gabi::load<s16>(src + 2) / 255.0f;
    f32 b = (f32)gabi::load<s16>(src + 4) / 255.0f;
    f32 a = (f32)gabi::load<s16>(src + 6) / 255.0f;
    gabi::store<u32>(dst, fbits_of(r));
    gabi::store<u32>(dst + 4, fbits_of(g));
    gabi::store<u32>(dst + 8, fbits_of(b));
    gabi::store<u32>(dst + 0xC, fbits_of(a));
}
VERIFY(0x0245F4C0, color_s16_to_f);

/* 0245F584: GXColor -> float colour (rgba / 255) */
static void color_u8_to_f(u32 dst, u32 src) {
    WWHD_FUNC(0x0245F584, void, dst, src);
    f32 r = (f32)gabi::load<u8>(src) / 255.0f;
    f32 g = (f32)gabi::load<u8>(src + 1) / 255.0f;
    f32 b = (f32)gabi::load<u8>(src + 2) / 255.0f;
    f32 a = (f32)gabi::load<u8>(src + 3) / 255.0f;
    gabi::store<u32>(dst, fbits_of(r));
    gabi::store<u32>(dst + 4, fbits_of(g));
    gabi::store<u32>(dst + 8, fbits_of(b));
    gabi::store<u32>(dst + 0xC, fbits_of(a));
}
VERIFY(0x0245F584, color_u8_to_f);

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

/* 0245F638 daSail_packet_c::update (HD): load the "Cloth" texture once, upload the vertices
 * (positions with front / back normals), the tevStr colours and the matrices */
void daSail_packet_c::update() {
    WWHD_FUNC(0x0245F638, void, this);
    u32 P = gabi::ea(this);
    gabi::call(0x0274FBF8, gabi::load<u32>(l_texLock)); /* lock */
    if (gabi::load<u8>(P + 0x1290) == 0) {
        void* tmp_img = dComIfG_getObjectRes(STR(0x10039588) /* "Cloth" */, 3, SAFESTRING_VTBL);
        if (tmp_img == nullptr)
            JUT_ASSERT_fail(STR(0x10039590), 0x2BC, STR(0x100395A0)); /* tmp_img != (0) */
        gabi::call(0x02773798, P + 0xED0, gabi::load<u32>(gabi::ea(tmp_img) + 0x20));
        tex_bind(P + 0x10F8, P + 0xED0);
        gabi::store<u8>(P + 0x1290, 1);
    }
    gabi::call(0x0274FCCC, gabi::load<u32>(l_texLock)); /* unlock */
    /* the two vertex sets (front, back): position + normal, 0x20 bytes per vertex */
    u32 V = P + 0xA4;
    for (u32 k = 0; k < 2; k++) {
        u32 dst = gabi::load<u32>(V + (k * 2 + gabi::load<u32>(V + 0x950)) * 0x254) - 0x20;
        for (u32 n = 0; n < 0x54; n++) {
            u32 p = P + 0x1328 + (gabi::load<u8>(P + 0x2EBE) * 0x54 + n) * 0xC;
            f32 z = gabi::load<f32>(p + 8), x = gabi::load<f32>(p), y = gabi::load<f32>(p + 4);
            dst += 0x20;
            gabi::store<f32>(dst, x);
            gabi::store<f32>(dst + 4, y);
            gabi::store<f32>(dst + 8, z);
            u32 q = P + (k == 0 ? 0x1EF8 : 0x26D8) + (gabi::load<u8>(P + 0x2EBE) * 0x54 + n) * 0xC;
            f32 nx = gabi::load<f32>(q), ny = gabi::load<f32>(q + 4), nz = gabi::load<f32>(q + 8);
            gabi::store<f32>(dst + 0x14, nz);
            gabi::store<f32>(dst + 0xC, nx);
            gabi::store<f32>(dst + 0x10, ny);
        }
    }
    u32 e = V + gabi::load<u32>(V + 0x950) * 0x254 + 4;
    for (int m = 0; m < 2; m++, e += 0x4A8) gabi::call(0x027B5E94, e, 0, gabi::load<u32>(e + 0x14C)); /* flush */
    gabi::store<u32>(V + 0x950, gabi::load<u32>(V + 0x950) == 0);
    /* the tevStr colours into the uniform block */
    gabi::call(0x0255F8F4, gabi::load<u32>(P + 0x1324));
    gabi::call(0x0255FE90, gabi::load<u32>(P + 0x1324));
    u32 G = P + 0xA0C;
    {
        gabi::Local<Mtx34> view;
        mtx_to_sead(gabi::ea(view.get()), gabi::ea(J3DSys_viewMtx));
        gabi::call(0x027FDA54, G, 0, view.get(), 0x104B470Cu, gabi::load<u32>(0x104B4708) + 0x240);
    }
    gabi::Local<GXColor[4]> c10;
    u32 tev = gabi::load<u32>(P + 0x1324);
    u32 U = gabi::load<u32>(P + 0xA10);
    color_s16_to_f(gabi::ea(c10.get()), tev + 0x90);
    gabi::call(0x0274D458, U + 0x1C4, c10.get(), gabi::load<f32>(gabi::load<u32>(P + 0x1324) + 0x28));
    color_s16_to_f(gabi::ea(c10.get()), gabi::load<u32>(P + 0x1324) + 0x160);
    gabi::call(0x0274D458, U + 0x1D4, c10.get(), gabi::load<f32>(gabi::load<u32>(P + 0x1324) + 0x16C));
    gabi::call(0x027FDFF4, G, 0);
    tev = gabi::load<u32>(P + 0x1324);
    u32 M = P + 0xB34;
    color_s16_to_f(P + 0xB74, tev + 0x90);
    color_u8_to_f(P + 0xB84, tev + 0x98);
    gabi::call(0x0274D2AC, P + 0xB84, gabi::load<f32>(gabi::load<u32>(P + 0x1324) + 0x24));
    if (gabi::load<u8>(tev + 0x9F) != 0) {
        color_u8_to_f(M + 0x60, tev + 0x9C);
    } else {
        gabi::store<f32>(M + 0x64, 0.0f);
        gabi::store<f32>(M + 0x60, 0.0f);
        gabi::store<f32>(M + 0x68, 0.0f);
        gabi::store<f32>(M + 0x6C, 0.0f);
    }
    gabi::call(0x027FB678, P + 0xAC0);
    gabi::Local<Mtx34> mtx;
    mtx_to_sead(gabi::ea(mtx.get()), P + 0x1294);
    PSMTXCopy(mtx.get(), gabi::at<Mtx34>(P + 0xA8C));
    gabi::call(0x027FB678, P + 0xA18);
}
VERIFY(0x0245F638, &daSail_packet_c::update);

/* 0245FAAC */
static BOOL daSail_Draw(sail_class* i_this) {
    WWHD_FUNC(0x0245FAAC, BOOL, i_this);
    u32 sh = ship();
    if (gabi::load<u8>(sh + 0x3E4) == 0) { /* l_p_ship->m2CC */
        return FALSE;
    }
    tevstr_copy(gabi::ea(&i_this->tevStr), sh + 0x110); /* i_this->tevStr = l_p_ship->tevStr */
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
        PSMTXConcat_l(ident, calc_mtx(), &i_this->mSailPacket.mMtx);
    }

    cXyz* vtxPos = i_this->mSailPacket.getPos();
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y + vtxPos[0x53].y, i_this->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
    PSMTXConcat_l(J3DSys_viewMtx, calc_mtx(), &i_this->mSailPacket.mStickMtx);
    {
        /* mStickModel->setBaseTRMtx(*calc_mtx) */
        u32 src = gabi::ea(calc_mtx());
        u32 mdl = gabi::ea(i_this->mStickModel.get());
        u32 v[12]; /* lfs/stfs copies (bits kept) */
        for (int i = 0; i < 12; i++) v[i] = gabi::load<u32>(src + 4 * i);
        for (int i = 0; i < 12; i++) gabi::store<u32>(mdl + 0xC8 + 4 * i, v[i]);
    }

    MtxTrans(0.0f, 0.0f, 0.0f, 0);
    PSMTXCopy(calc_mtx(), &i_this->mSailPacket.mTexMtx);

    i_this->mSailPacket.mTevStr = &i_this->tevStr;

    /* HD: the opaque/translucent buffers are switched around the stick model's and the packet's entries */
    gabi::store<u32>(j3dSys_ + 0x74, gabi::load<u32>(dComIfGp_ea() + 0x5D70));
    gabi::store<u32>(j3dSys_ + 0x78, gabi::load<u32>(dComIfGp_ea() + 0x5D74));
    mDoExt_modelUpdateDL(i_this->mStickModel, 0);
    J3DDrawBuffer_entryImm(gabi::load<u32>(j3dSys_ + 0x74), &i_this->mSailPacket, 0);
    gabi::store<u32>(j3dSys_ + 0x74, gabi::load<u32>(dComIfGp_ea() + 0x5D78));
    gabi::store<u32>(j3dSys_ + 0x78, gabi::load<u32>(dComIfGp_ea() + 0x5D7C));
    i_this->mSailPacket.update();
    return TRUE;
}
VERIFY(0x0245FAAC, daSail_Draw);

/* 0245FFC8 */
void daSail_packet_c::setCorrectNrmAngle(s16 param_0, f32 param_1) {
    WWHD_FUNC(0x0245FFC8, void, this, param_0, param_1);
    s32 rnd = gabi::ftoi(cM_rndF(200.0f));
    m1C38 = (s16)(m1C38 + (rnd + 900));

    m1C34 = (s16)gabi::ftoi(300.0f * cM_ssin(m1C38));

    s16 r28 = (s16)(param_0 + 0x8000);
    s32 r27 = param_0;

    s16 r26 = (s16)gabi::ftoi(l_HIO().m10 * gabi::fnmsubs(0.5f, param_1, 1.0f));
    f32 f9 = (f32)r26;
    s16 lim = (s16)gabi::ftoi(f9 * 1.25f * 182.04445f);

    if (abs(r28) < lim) {
        s16 targetAngle = r28 > 0 ? (s16)gabi::ftoi((f32)(-r26) * 182.04445f) : (s16)gabi::ftoi(f9 * 182.04445f);
        cLib_addCalcAngleS2(&m1C36, targetAngle, 5, 192);
    } else if (abs(r27) < lim) {
        s16 targetAngle = (s16)r27 > 0 ? (s16)gabi::ftoi((f32)(-r26) * 182.04445f) : (s16)gabi::ftoi(f9 * 182.04445f);
        cLib_addCalcAngleS2(&m1C36, targetAngle, 5, 192);
    } else {
        cLib_addCalcAngleS2(&m1C36, 0, 5, 192);
    }
    m1C34 = (s16)(m1C34 + m1C36);
}
VERIFY(0x0245FFC8, &daSail_packet_c::setCorrectNrmAngle);

/* 024601F0 */
void daSail_packet_c::setNrmMtx() {
    WWHD_FUNC(0x024601F0, void, this);
    cMtx_YrotS(calc_mtx(), m1C34);
}
VERIFY(0x024601F0, &daSail_packet_c::setNrmMtx);

/* 024605DC */
void daSail_packet_c::setBackNrm() {
    WWHD_FUNC(0x024605DC, void, this);
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
VERIFY(0x024605DC, &daSail_packet_c::setBackNrm);

/* the normal of the face (a, b) at the vertex: normZC(a x b), added to sum */
static inline void add_face_nrm(gabi::Local<cXyz>& sum, gabi::Local<cXyz>& a, gabi::Local<cXyz>& b, gabi::Local<cXyz>& spE0) {
    gabi::Local<cXyz> tmp;
    cXyz_outprod(a, tmp, b);
    gabi::store<u32>(gabi::ea(spE0.get()), gabi::load<u32>(tmp.a));
    gabi::store<u32>(gabi::ea(spE0.get()) + 4, gabi::load<u32>(tmp.a + 4));
    gabi::store<u32>(gabi::ea(spE0.get()) + 8, gabi::load<u32>(tmp.a + 8));
    cXyz_normZC(spE0, tmp);
    gabi::store<u32>(gabi::ea(spE0.get()), gabi::load<u32>(tmp.a));
    gabi::store<u32>(gabi::ea(spE0.get()) + 4, gabi::load<u32>(tmp.a + 4));
    gabi::store<u32>(gabi::ea(spE0.get()) + 8, gabi::load<u32>(tmp.a + 8));
    PSVECAdd(sum, spE0, sum);
}
/* dst = vtx - spC8 (cXyz::operator- into a temporary, copied) */
static inline void vec_sub(gabi::Local<cXyz>& dst, cXyz* vtx, gabi::Local<cXyz>& spC8) {
    gabi::Local<cXyz> tmp;
    cXyz_mi(vtx, tmp, spC8);
    gabi::store<u32>(dst.a, gabi::load<u32>(tmp.a));
    gabi::store<u32>(dst.a + 4, gabi::load<u32>(tmp.a + 4));
    gabi::store<u32>(dst.a + 8, gabi::load<u32>(tmp.a + 8));
}

/* 02460200 */
void daSail_packet_c::setNrmVtx(cXyz* param_0, int param_1, int param_2) {
    WWHD_FUNC(0x02460200, void, this, param_0, param_1, param_2);
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
        gabi::store<u32>(spD4.a, gabi::load<u32>(tmp.a));
        gabi::store<u32>(spD4.a + 4, gabi::load<u32>(tmp.a + 4));
        gabi::store<u32>(spD4.a + 8, gabi::load<u32>(tmp.a + 8));
    }

    MtxPush();
    cMtx_YrotM(calc_mtx(), (s16)gabi::ftoi(cM_ssin(-800 * (param_1 + param_2)) * 900.0f));
    MtxPosition(spD4, spE0);
    {
        gabi::Local<cXyz> tmp;
        cXyz_normZC(spE0, tmp);
        u32 d = gabi::ea(param_0);
        gabi::store<u32>(d, gabi::load<u32>(tmp.a));
        gabi::store<u32>(d + 4, gabi::load<u32>(tmp.a + 4));
        gabi::store<u32>(d + 8, gabi::load<u32>(tmp.a + 8));
    }
    MtxPull();
}
VERIFY(0x02460200, &daSail_packet_c::setNrmVtx);

/* 02460678 sail_move (sail_pos_move and demo_move inlined).
 * HD: the wind phase speed is 4000 (GameCube 0.8 * 5000), and in stage "Demo46" (play+0x5292 set)
 * the flutter amplitudes follow the wind power; the furl bulge uses 700/150 (GameCube 3.5*200 /
 * 1.5*100, the same) */
static void sail_move(sail_class* i_this) {
    WWHD_FUNC(0x02460678, void, i_this);
    u32 A = gabi::ea(i_this);
    cXyz* windVec = dKyw_get_wind_vec();
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
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) {
            JUT_ASSERT_fail(STR(0x10039444) /* "d_demo.h" */, 0x23A, STR(0x1003942C) /* "m_object != (0)" */);
            obj = gabi::load<u32>(0x101D5FFC);
        }
        u32 demo_actor = gabi::call<u32>(0x02526E70, obj, demoId);
        if (demo_actor != 0) {
            if (gabi::load<u16>(demo_actor + 4) & 0x40) { /* checkEnable(ENABLE_ANM_FRAME_e) */
                f32 frame = gabi::fnmsubs(gabi::load<f32>(demo_actor + 0x30), 0.006f, 0.6f);
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
    cMtx_YrotS(calc_mtx(), (s16)-(i_this->current.angle.y - windAngle));
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
        cXyz* windVec2 = dKyw_get_wind_vec();
        s16 windAngle2 = cM_atan2s(windVec2->x, windVec2->z);
        cMtx_YrotS(calc_mtx(), (s16)-(i_this->current.angle.y - windAngle2));
        gabi::Local<cXyz> sp28;
        gabi::Local<cXyz> sp1C;
        sp28->y = 0.0f;
        sp28->z = 0.064f;
        sp28->x = 0.0f;
        MtxPosition(sp28, sp1C);
        f32 f31 = gabi::fmadds(0.9f, fabsf(sp1C->z), 0.02f) + 0x1.a36e3p-8f; /* 0.9|z| + 0.02 + f14 * 0.1 (0.064f * 0.1f, not the nearest float to 0.0064) */
        sp28->z = 1.0f;
        sp28->x = 0.0f;
        MtxPosition(sp28, sp1C);
        f32 f25 = fabsf(sp1C->z);
        f32 f0 = fabsf(sp1C->x);
        s32 tmp = gabi::ftoi(4000.0f * gabi::fnmsubs(0.95f, i_this->m1C44, 1.0f));
        f25 = gabi::fmadds(0.9f, f25, 0.1f);
        /* HD: in "Demo46" the wind power scales the flutter */
        if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0) {
            gabi::Local<SafeString> s1;
            gabi::Local<SafeString> s0;
            s1->__vtbl = SAFESTRING_VTBL;
            s1->mStringTop = 0x1047E6B8u; /* the stage name */
            s0->mStringTop = 0x100393ECu; /* "Demo46" */
            s0->__vtbl = SAFESTRING_VTBL;
            gabi::call(0x02463348, s0.get());
            gabi::call_ptr(gabi::load<u32>(s0->__vtbl + 0x14), s0.get()); /* assureTermination */
            u32 a = s0->mStringTop;
            gabi::call_ptr(gabi::load<u32>(s1->__vtbl + 0x14), s1.get());
            u32 b = s1->mStringTop;
            BOOL eq = FALSE;
            if (a == b) {
                eq = TRUE;
            } else {
                for (u32 n = 0; n < 0x40001; n++) {
                    u8 ca = gabi::load<u8>(a + n);
                    u8 cb = gabi::load<u8>(b + n);
                    if (ca != cb)
                        break;
                    if (ca == 0) {
                        eq = TRUE;
                        break;
                    }
                }
            }
            if (eq) {
                f25 = f25 * dKyw_get_wind_pow();
                f0 = gabi::fmadds(dKyw_get_wind_pow(), 0.2f, f0);
            }
        }

        i_this->m1C40 = gabi::ftoi(gabi::fmadds((f32)tmp, gabi::fmadds(0.85f, f25, 0.15f), (f32)(s32)i_this->m1C40));
        i_this->m1C50 = (s16)gabi::ftoi(7500.0f * gabi::fmadds(1.5f, f0, 1.0f));
        i_this->m1C52 = (s16)gabi::ftoi(7200.0f * gabi::fmadds(0.95f, f25, 0.05f));

        u8 cur = gabi::load<u8>(A + 0x327A);
        f32 sp6C[7] = {};
        f32 sp34[7] = {};

        f32 a44 = i_this->m1C44;
        f0 = f0 * gabi::fnmsubs(0.75f, a44, 1.0f);
        f25 = f25 * gabi::fnmsubs(0.5f * a44, i_this->m1C4C, 1.0f);
        f32 f0sq = f0 * f0;
        f32 cosAmp = 550.0f * (f0sq + 0.1f);
        f32 sinAmp = 1750.0f * (f0sq + 0.05f);

        u32 vtx = A + 0x16E4 + cur * 0x3F0;
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
                f32 f24 = gabi::fmadds(f25, gabi::fmsubs(10.0f, f17, f17 * f23 * f23), (spz * (18.0f - f23sq)) / 18.0f);
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
                    gabi::store<f32>(vtx, x);
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
                    gabi::store<f32>(vtx, x);
                    y = gabi::fnmsubs(i125, f23sq, sp6C[j] + f21);
                    z = f24 + f20;
                }
                gabi::store<f32>(vtx + 4, y);
                gabi::store<f32>(vtx + 8, z);
                u32 lp = l_pos_ + (i * 7 + j) * 0xC;
                x = x + gabi::load<f32>(lp);
                gabi::store<f32>(vtx, x);
                y = y + gabi::load<f32>(lp + 4);
                gabi::store<f32>(vtx + 4, y);
                z = z + gabi::load<f32>(lp + 8);
                gabi::store<f32>(vtx + 8, z);

                y = y * (1.0f - (i_this->m1C44 * f13sq) / 30.25f);
                gabi::store<f32>(vtx + 4, y);
                y = y * (gabi::fnmsubs(i_this->m1C44, fi, 11.0f) / 11.0f);
                gabi::store<f32>(vtx + 4, y);
                z = z * (1.0f - (i_this->m1C44 * f13sq) / 30.25f);
                gabi::store<f32>(vtx + 8, z);
                z = z * (gabi::fnmsubs(i_this->m1C44, fi, 11.0f) / 11.0f);
                gabi::store<f32>(vtx + 8, z);

                f32 a = i_this->m1C44;
                if (a > 0.0f && i < 6) {
                    f32 f20b = gabi::fnmsubs(f15_2, f15_2, 9.0f) / 9.0f;
                    z = gabi::fmadds(700.0f * f20b, a, z);
                    y = gabi::load<f32>(vtx + 4);
                    gabi::store<f32>(vtx + 8, z);
                    y = gabi::fnmsubs(150.0f * f20b, i_this->m1C44, y);
                    gabi::store<f32>(vtx + 4, y);
                    a = i_this->m1C44;
                } else {
                    y = gabi::load<f32>(vtx + 4);
                }
                f32 odd = (f32)(j & 1);
                y = y * gabi::fmadds(0.3f * odd, a, 1.0f - a);
                gabi::store<f32>(vtx + 4, y);
                a = i_this->m1C44;
                z = gabi::load<f32>(vtx + 8) * gabi::fmadds(0.15f * odd, a, 1.0f - a);
                gabi::store<f32>(vtx + 8, z);
            }
        }
    }

    f32 f31_2 = f31_1 * gabi::fnmsubs(0.5f * i_this->m1C44, i_this->m1C4C, 1.0f);
    u32 nrm = A + 0x22B4 + gabi::load<u8>(A + 0x327A) * 0x3F0;
    s16 angleY = i_this->current.angle.y;
    gabi::Local<cXyz> light;
    gabi::Local<cXyz> light_res;
    dKy_FirstlightVec_get(light_res, light);
    s16 lightAngle = cM_atan2s(light->x, light->z);
    i_this->mSailPacket.setCorrectNrmAngle((s16)(lightAngle - angleY), f31_2);
    i_this->mSailPacket.setNrmMtx();
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 7; j++) {
            i_this->mSailPacket.setNrmVtx(gabi::at<cXyz>(nrm), j, i);
            nrm += 0xC;
        }
    }
    i_this->mSailPacket.setBackNrm();
    /* HD: the three current buffers (GameCube USA/PAL) */
    DCStoreRangeNoSync(gabi::at<u8>(A + 0x16E4 + gabi::load<u8>(A + 0x327A) * 0x3F0), 0x3F0);
    DCStoreRangeNoSync(gabi::at<u8>(A + 0x22B4 + gabi::load<u8>(A + 0x327A) * 0x3F0), 0x3F0);
    DCStoreRangeNoSync(gabi::at<u8>(A + 0x2A94 + gabi::load<u8>(A + 0x327A) * 0x3F0), 0x3F0);
}
VERIFY(0x02460678, sail_move);

/* 024614EC */
static BOOL daSail_Execute(sail_class* i_this) {
    WWHD_FUNC(0x024614EC, BOOL, i_this);
    /* static cXyz sail_offset(0.0f, 2100.0f, 100.0f): guard 1046D7AC, object 1046D7A0 */
    if (gabi::load<u32>(0x1046D7AC) == 0) {
        gabi::store<u32>(0x1046D7AC, 1);
        gabi::store<f32>(0x1046D7A8, 100.0f);
        gabi::store<f32>(0x1046D7A0, 0.0f);
        gabi::store<f32>(0x1046D7A4, 2100.0f);
    }
    J3DModel* mdl = gabi::at<J3DModel>(gabi::load<u32>(ship() + 0x3E8)); /* l_p_ship->mModel */
    PSMTXMultVec(J3DModel_getBaseTRMtx(mdl), gabi::at<cXyz>(0x1046D7A0), &i_this->current.pos);
    u32 sh = ship();
    i_this->current.angle.x = gabi::load<s16>(sh + 0x328); /* l_p_ship->shape_angle */
    i_this->current.angle.y = gabi::load<s16>(sh + 0x32A);
    i_this->current.angle.z = gabi::load<s16>(sh + 0x32C);
    sail_move(i_this);
    return TRUE;
}
VERIFY(0x024614EC, daSail_Execute);

/* 024615A8 */
static BOOL daSail_IsDelete(sail_class* i_this) {
    WWHD_FUNC(0x024615A8, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024615A8, daSail_IsDelete);

/* 024615B0 */
static BOOL daSail_Delete(sail_class* i_this) {
    WWHD_FUNC(0x024615B0, BOOL, i_this);
    dComIfG_resDelete(&i_this->mClothPhase, STR(0x10039678) /* "Cloth" */);
    dComIfG_resDelete(&i_this->mKaizokusenPhase, STR(0x10039680) /* "Kaizokusen" */);
    if (l_HIO().mNo >= 0) {
        mDoHIO_deleteChild(l_HIO().mNo);
        l_HIO().mNo = -1;
    }
    return TRUE;
}
VERIFY(0x024615B0, daSail_Delete);

/* 02461618 */
static BOOL daSail_checkCreateHeap(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02461618, BOOL, i_actor);
    sail_class* i_this = (sail_class*)i_actor;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1003968C) /* "Kaizokusen" */, 0xD, SAFESTRING_VTBL);
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
VERIFY(0x02461618, daSail_checkCreateHeap);

/* ===================================================================== */
/* HD packet construction / destruction / drawing                        */
/* ===================================================================== */
static inline u32 gpu_alloc(u32 size, u32 align) {
    u32 h = gabi::call<u32>(0x02756140, gabi::load<u32>(l_heap));
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(h + 0xC) + 0x34), h, size, align);
}
/* the heap getter is called with r4 still holding the pointer; the pointer to free is re-read at pa */
static inline void gpu_free(u32 pa, u32 r4) {
    u32 h = gabi::call<u32>(0x02755FEC, gabi::load<u32>(l_heap), r4);
    u32 fn = gabi::load<u32>(gabi::load<u32>(h + 0xC) + 0x3C);
    gabi::call_ptr(fn, h, gabi::load<u32>(pa));
}
/* GHS inline constructor of a sub-object at `p`: allocates when the address is NULL */
static inline u32 inl_ct(u32 p, u32 size) {
    if (p == 0) p = gabi::ea(operator_new(size));
    return p;
}
/* release the attribute objects (0xF4 bytes) of a program entry: {count (+cntOff), ptr} */
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

/* 024629DC: constructor of the 0x254-byte vertex buffer element */
static void* vtx_ct(void* p) {
    WWHD_FUNC(0x024629DC, void*, p);
    if (p == nullptr) {
        p = operator_new(0x254);
        if (p == nullptr)
            return p;
    }
    gabi::call(0x027B5BD8, gabi::ea(p) + 4);
    gabi::call(0x027BF734, gabi::ea(p) + 0x158);
    gabi::store<u32>(gabi::ea(p) + 0x250, 0);
    gabi::store<u32>(gabi::ea(p) + 0x24C, 0);
    return p;
}
VERIFY(0x024629DC, vtx_ct);

/* 02462A38: constructor of a trivial 0x10-byte element (allocates when NULL) */
static void* elem10_ct(void* p) {
    WWHD_FUNC(0x02462A38, void*, p);
    if (p == nullptr)
        p = operator_new(0x10);
    return p;
}
VERIFY(0x02462A38, elem10_ct);

/* 02462A64: deleting destructor of the 0x254-byte vertex buffer element */
static void vtx_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02462A64, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x027BF880, gabi::ea(p) + 0x158, 2);
        gabi::call(0x027B5CBC, gabi::ea(p) + 4, 2);
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02462A64, vtx_dt);

/* 024616AC daSail_packet_c::daSail_packet_c (HD: GPU objects; the GameCube members' init last) */
static daSail_packet_c* daSail_packet_ct(daSail_packet_c* pk) {
    WWHD_FUNC(0x024616AC, daSail_packet_c*, pk);
    if (pk == nullptr) {
        pk = (daSail_packet_c*)operator_new(0x2EC0);
        if (pk == nullptr)
            return pk;
    }
    u32 P = gabi::ea(pk);
    gabi::call(0x027F1278, pk); /* J3DPacket */
    /* the program list {s32, sead::Buffer {count, ptr}} and the vertex buffers {elements[4], ...} */
    gabi::store<u32>(P + 0x98, 0);
    gabi::store<u32>(P + 0xC, PACKET_VTBL);
    {
        u32 b = P + 0x9C;
        if (b == 0) b = gabi::ea(operator_new(8));
        if (b != 0) {
            gabi::store<u32>(b + 4, 0);
            gabi::store<u32>(b, 0);
        }
    }
    u32 V = P + 0xA4;
    {
        u32 v = V;
        if (v == 0) v = gabi::ea(operator_new(0x968));
        if (v != 0) {
            construct_array(v, 4, 0x254, 0x024629DC);
            gabi::store<u32>(v + 0x950, 0);
            gabi::store<u32>(v + 0x960, 0);
            gabi::store<u32>(v + 0x958, 0x20);
            gabi::store<u8>(v + 0x964, 0);
            gabi::store<u32>(v + 0x954, 0);
            for (int k = 0; k < 2; k++)
                for (int m = 0; m < 2; m++) gabi::store<u32>(v + k * 0x254 + m * 0x4A8, 0);
        }
    }
    /* the GPU state */
    gabi::call(0x027FD6F4, P + 0xA0C);
    gabi::call(0x027FB40C, P + 0xA18);
    gabi::store<u32>(P + 0xA24, 0x1016EF84);
    bzero_l(P + 0xA8C, 0x34);
    inl_ct(P + 0xA18 + 0x74, 0x30);
    gabi::call(0x027FB40C, P + 0xAC0);
    gabi::store<u32>(P + 0xACC, 0x1016EFB4);
    u32 M = P + 0xB34;
    bzero_l(M, 0x2F0);
    /* a zero cXyz, then 1.0 at every 0x10 bytes */
    for (u32 o = 0; o < 0xB0; o += 4) gabi::store<f32>(M + o, (o >= 0xC && ((o - 0xC) & 0xF) == 0) ? 1.0f : 0.0f);
    construct_array(P + 0xBE4, 2, 0x10, 0x02462A38);
    construct_array(P + 0xC04, 2, 0x10, 0x02462A38);
    construct_array(P + 0xC24, 2, 0x10, 0x02462A38);
    for (u32 o = 0x110; o <= 0x260; o += 0x30) inl_ct(M + o, 0x30);
    for (u32 o = 0x290; o <= 0x2E0; o += 0x10) inl_ct(M + o, 0x10);
    gabi::store<u8>(P + 0xE24, 0);
    gabi::call(0x027B5430, P + 0xE28);
    gabi::call(0x027BE6B8, P + 0xE40);
    gabi::call(0x027BE6B8, P + 0xED0);
    gabi::call(0x027BDF7C, P + 0xF60);
    gabi::call(0x027BDF7C, P + 0x10F8);
    gabi::store<u8>(P + 0x1290, 0);
    /* GameCube: m1C3A = 0; m1C34 = 0; m1C38 = 0; m1C36 = 0; m1C3B = 1 */
    pk->m1C34 = 0;
    pk->m1C3B = 1;
    pk->m1C36 = 0;
    pk->m1C3A = 0;
    pk->m1C38 = 0;

    /* the shader program "flag_default" into the program list */
    {
        gabi::Local<SafeString> key;
        key->mStringTop = 0x10039698u; /* "flag_default" */
        key->__vtbl = SAFESTRING_VTBL;
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
        gabi::call(0x0280068C, P + 0x98, e, 0);
    }
    gabi::store<u32>(V + 0x954, 0x13);
    gabi::store<u32>(V + 0x95C, 0x10039700);
    /* vertex buffers: 2 x 2 sets of 0x54 vertices (0x20 bytes each) */
    for (u32 k = 0; k < 2; k++) {
        for (u32 m = 0; m < 2; m++) {
            u32 el = V + k * 0x254 + m * 0x4A8;
            u32 data = gabi::load<u32>(el);
            if (data == 0) {
                u32 buf = el + 0x24C;
                u32 p = gpu_alloc(0xA80, 0x40);
                if (p != 0) {
                    gabi::store<u32>(buf + 4, p);
                    gabi::store<u32>(buf, 0x54);
                }
                data = gabi::load<u32>(buf + 4);
                gabi::store<u32>(el, data);
            }
            gabi::call(0x027FF478, el + 4, data, 0x54, V + 0x954);
        }
    }
    gabi::store<u32>(V + 0x960, 0);
    gabi::store<u8>(V + 0x964, 1);
    /* per program: two attribute object pairs bound to the vertex buffers */
    for (u32 i = 0; i < gabi::load<u32>(P + 0x98); i++) {
        u32 E = gabi::load<u32>(P + 0xA0);
        if (i < gabi::load<u32>(P + 0x9C))
            E += i * 0x14;
        u32 prog = gabi::load<u32>(E);
        gabi::store<u32>(E, 0);
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
    gabi::call(0x027FE084, P + 0xA0C, 1, 0);
    gabi::call(0x027B54E0, P + 0xE28, 0x10039450u, 4, 0x9A); /* the index buffer (l_sail_idx) */
    gabi::store<u32>(P + 0xE2C, 6);
    /* the initial vertices: l_pos, zero normals, l_texCoord */
    for (u32 k = 0; k < 2; k++) {
        u32 off = (2 * k + gabi::load<u32>(V + 0x950)) * 0x254;
        u32 d = gabi::load<u32>(V + off);
        if (d < d + 0xA80) {
            dcbz_range(d, 0xA80);
            off = (2 * k + gabi::load<u32>(P + 0x9F4)) * 0x254;
        }
        u32 dst = gabi::load<u32>(V + off) - 0x20;
        for (u32 n = 0; n < 0x54; n++) {
            u32 src = l_pos_ + n * 0xC;
            f32 x = gabi::load<f32>(src), y = gabi::load<f32>(src + 4), z = gabi::load<f32>(src + 8);
            dst += 0x20;
            gabi::store<f32>(dst, x);
            gabi::store<f32>(dst + 0xC, 0.0f);
            gabi::store<f32>(dst + 8, z);
            gabi::store<f32>(dst + 0x10, 0.0f);
            gabi::store<f32>(dst + 4, y);
            gabi::store<f32>(dst + 0x14, 0.0f);
            f32 u = gabi::load<f32>(l_texCoord_ + n * 8), v = gabi::load<f32>(l_texCoord_ + n * 8 + 4);
            gabi::store<f32>(dst + 0x18, u);
            gabi::store<f32>(dst + 0x1C, v);
        }
    }
    /* copy into the other buffers */
    {
        u32 sel = gabi::load<u32>(V + 0x950);
        u32 dset = V + (sel == 0) * 0x254;
        for (u32 k = 0; k < 2; k++, dset += 0x4A8) {
            u32 sset = V + (2 * k + sel) * 0x254;
            for (u32 o = 0; o < 0x54 * 0x20; o += 0x20) {
                u32 s = gabi::load<u32>(sset) + o;
                u32 d = gabi::load<u32>(dset) + o;
                /* lfs/stfs pairs: the recompiled original keeps a signalling NaN's bits (seen at an
                 * unaligned buffer, where the float-store NaN tolerance does not apply): copy the bits */
                for (u32 w = 0; w < 0x20; w += 4) gabi::store<u32>(d + w, gabi::load<u32>(s + w));
            }
            sel = gabi::load<u32>(V + 0x950);
        }
        u32 e = V + sel * 0x254 + 4;
        for (int m = 0; m < 2; m++, e += 0x4A8) gabi::call(0x027B5E94, e, 0, gabi::load<u32>(e + 0x14C)); /* flush */
        gabi::store<u32>(V + 0x950, gabi::load<u32>(V + 0x950) == 0);
    }
    /* the sail texture (Kaizokusen, index 0xA) */
    gabi::call(0x0274FBF8, gabi::load<u32>(l_texLock));
    void* tmp_img = dComIfG_getObjectRes(STR(0x100396A8) /* "Kaizokusen" */, 0xA, SAFESTRING_VTBL);
    if (tmp_img == nullptr)
        JUT_ASSERT_fail(STR(0x100396B4), 0x34F, STR(0x100396C4)); /* tmp_img != (0) */
    gabi::call(0x02773798, P + 0xE40, gabi::load<u32>(gabi::ea(tmp_img) + 0x20));
    gabi::call(0x0274FCCC, gabi::load<u32>(l_texLock));
    tex_bind(P + 0xF60, P + 0xE40);
    return pk;
}
VERIFY(0x024616AC, daSail_packet_ct);

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
    gabi::store<u32>(P + 0xC, PACKET_VTBL);
    u32 V = P + 0xA4;
    for (u32 k = 0; k < 4; k++) vbuf_release(V + k * 0x254);
    gabi::store<u32>(V + 0x960, 0);
    uniforms_release(P + 0xA0C);
    for (u32 j = 0; j < 2; j++) gabi::call(0x027BEBEC, P + 0xA28 + j * 0x1C);
    for (u32 j = 0; j < 2; j++) gabi::call(0x027BEBEC, P + 0xAD0 + j * 0x1C);
    gabi::call(0x027BE2B0, P + 0x10F8, 2);
    gabi::call(0x027BE2B0, P + 0xF60, 2);
    gabi::call(0x027B54A0, P + 0xE28, 2);
    gabi::call(0x027FB528, P + 0xAC0, 0);
    gabi::call(0x027FB528, P + 0xA18, 0);
    gabi::call(0x027FD764, P + 0xA0C, 2);
    if (V != 0) {
        for (u32 k = 0; k < 4; k++) vbuf_release(V + k * 0x254);
        gabi::store<u32>(V + 0x960, 0);
        gabi::call(0x028F0164, V, 4, 0x254, 0x02462A64u, 0, 0); /* __destroy_arr */
    }
    proglist_release(P + 0x9C);
    gabi::call(0x027F13DC, P, 0); /* J3DPacket::~J3DPacket */
}

/* 02462AC4 daSail_packet_c::~daSail_packet_c (deleting) */
static void daSail_packet_dt(daSail_packet_c* p, s32 flags) {
    WWHD_FUNC(0x02462AC4, void, p, flags);
    if (p != nullptr) {
        packet_dt_body(gabi::ea(p));
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02462AC4, daSail_packet_dt);

/* 02462EF0 sail_class::~sail_class (deleting) */
static void sail_class_dt(sail_class* a, s32 flags) {
    WWHD_FUNC(0x02462EF0, void, a, flags);
    if (a != nullptr) {
        packet_dt_body(gabi::ea(a) + 0x3BC);
        gabi::call(0x025D50BC, a, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(a);
    }
}
VERIFY(0x02462EF0, sail_class_dt);

/* ---- drawing (HD engine inlines) ---- */
/* bind the uniform block of the shader object `shObj` to the program's pixel/vertex/geometry slots */
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
    if (ps != -1)
        gabi::call(0xC0006900, ps, data, size); /* GX2SetPixelUniformBlock */
    if (vs != -1)
        gabi::call(0xC0006A38, vs, data, size); /* GX2SetVertexUniformBlock */
    if (gs != -1)
        gabi::call(0xC00068A8, gs, data, size); /* GX2SetGeometryUniformBlock */
}
static inline void set_tex(u32 obj, u32 loc) { gabi::call(0x027BE53C, obj, loc + 4, -1, 0); }
/* draw the 11 strips (14 indices each) of the index buffer at P+0xE28 */
static inline void draw_strips(u32 P) {
    for (u32 k = 0; k < 11; k++) {
        u32 type = gabi::load<u32>(P + 0xE28);
        u32 stride = gabi::load<u32>(P + 0xE38);
        u32 base = gabi::load<u32>(P + 0xE30);
        u32 mode = gabi::load<u32>(P + 0xE2C);
        gabi::call(0xC0006178, mode, 0xE, type, base + stride * (k * 0xE), 0, 1); /* GX2DrawIndexedEx */
    }
}
struct DrawState_l {
    u8 _[0x11C]; /* HD sizeof: the ctor allocates 0x11C when this == NULL, vtable at +0x118 */
};

/* 02462364 daSail_packet_c::draw (virtual; ctx: the HD draw context) */
void daSail_packet_c::draw(void* ctxp) {
    WWHD_FUNC(0x02462364, void, this, ctxp);
    u32 P = gabi::ea(this);
    u32 ctx = gabi::ea(ctxp);
    /* the program of the current pass */
    u32 prog = 0;
    {
        s32 pass = gabi::load<s32>(ctx + 0xC);
        if (pass < 4) {
            u32 e = gabi::load<u32>(P + 0xA0);
            if ((u32)pass < gabi::load<u32>(P + 0x9C))
                e += pass * 0x14;
            prog = gabi::load<u32>(e);
        }
    }
    /* bind the shader program (the current one is cached in the state returned by 027F29D4) */
    {
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
    s32 pass = gabi::load<s32>(ctx + 0xC);
    if (pass == 0) {
        vcall2C(P + 0xA18, prog);
        u32 sh = gabi::load<u32>(ctx + 0x14);
        if (sh != 0)
            bind_uniform(gabi::load<u32>(sh + 4), prog);
    } else if (pass == 1) {
        bind_uniform(gabi::load<u32>(P + 0xA10), prog);
        vcall2C(P + 0xAC0, prog);
        vcall2C(P + 0xA18, prog);
    } else if (pass == 2) {
        bind_uniform(gabi::load<u32>(P + 0xA10), prog);
        vcall2C(P + 0xAC0, prog);
        vcall2C(P + 0xA18, prog);
        u32 o = gabi::load<u32>(ctx + 0x30);
        if (o != 0)
            vcall2C(o, prog);
        gabi::call(0x027FFE54, ctx, prog);
    }
    /* the textures: sail (Kaizokusen) and the toon ramp (Cloth) */
    {
        u32 t0 = gabi::load<u32>(prog + 0x14) != 0 ? gabi::load<u32>(prog + 0x18) : 0;
        set_tex(P + 0xF60, t0);
        u32 t1 = gabi::load<u32>(prog + 0x14) > 1 ? gabi::load<u32>(prog + 0x18) + 0x14 : 0;
        set_tex(P + 0x10F8, t1);
    }
    gabi::Local<DrawState_l> st;
    u32 S = gabi::ea(st.get());
    gabi::call(0x02750250, S);
    gabi::store<u32>(S + 0xC, 2);
    gabi::store<u32>(S + 8, 0);
    u32 f = gabi::load<u32>(S + 0xEC);
    gabi::store<u32>(S + 0xE4, 4);
    gabi::store<f32>(S + 0xE8, 0.5f);
    gabi::store<u8>(S + 0xE0, 1);
    gabi::store<u32>(S + 0xEC, (((f & ~0xFu) + 7) & ~0xF0u) + 0x10);
    gabi::call(0x0280037C, gabi::load<s32>(ctx + 0xC), S);
    gabi::call(0x02750370, S);
    /* front faces */
    {
        u32 cnt = gabi::load<u32>(P + 0x9C);
        u32 pass2 = gabi::load<u32>(ctx + 0xC);
        u32 e = gabi::load<u32>(P + 0xA0);
        u32 sel = gabi::load<u32>(P + 0x9F4);
        if (pass2 < cnt)
            e += pass2 * 0x14;
        gabi::call(0x027BFE5C, gabi::load<u32>(e + (sel == 0 ? 8 : 0) + 8));
        draw_strips(P);
    }
    gabi::store<u32>(S + 8, 1);
    gabi::call(0x02750684, S);
    /* back faces */
    {
        u32 cnt = gabi::load<u32>(P + 0x9C);
        u32 pass2 = gabi::load<u32>(ctx + 0xC);
        u32 sel = gabi::load<u32>(P + 0x9F4);
        u32 e = gabi::load<u32>(P + 0xA0);
        if (pass2 < cnt)
            e += pass2 * 0x14;
        e += (sel == 0 ? 8 : 0);
        u32 n = gabi::load<u32>(e + 4);
        u32 attrs = gabi::load<u32>(e + 8);
        if (n > 1)
            attrs += 0xF4;
        gabi::call(0x027BFE5C, attrs);
        draw_strips(P);
    }
    gabi::call(0x02750370, j3dSys_ + 0x18C);
}
VERIFY(0x02462364, &daSail_packet_c::draw);

/* 02462194 */
static cPhs_State daSail_Create(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02462194, cPhs_State, i_actor);
    sail_class* i_this = (sail_class*)i_actor;
    /* fopAcM_ct(i_actor, sail_class) */
    u32 cond = i_this->actor_condition;
    if (!(cond & fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            gabi::store<u32>(gabi::ea(i_this) + 0xB4, SAIL_VTBL);
            daSail_packet_ct(&i_this->mSailPacket);
            cond = i_this->actor_condition;
        }
        i_this->actor_condition = cond | fopAcCnd_INIT_e;
    }

    cPhs_State rt1 = dComIfG_resLoad(&i_this->mClothPhase, STR(0x100396D4) /* "Cloth" */);
    if (rt1 != cPhs_COMPLEATE_e) {
        return rt1;
    }
    cPhs_State rt2 = dComIfG_resLoad(&i_this->mKaizokusenPhase, STR(0x100396DC) /* "Kaizokusen" */);
    if (rt2 != cPhs_COMPLEATE_e) {
        return rt2;
    }

    cPhs_State phase_state = cPhs_COMPLEATE_e;
    if (fopAcM_entrySolidHeap(i_this, (heapCallbackFunc)0x02461618, 0x4C0)) {
        if (l_HIO().mNo < 0) {
            l_HIO().mNo = mDoHIO_createChild(STR(0x100396E8) /* "Pirate Ship's Sail" */, &l_HIO());
        }
        u32 A = gabi::ea(i_this);
        u8 cur = gabi::load<u8>(A + 0x327A);
        i_this->m1C44 = 0.0f;
        i_this->m1C48 = 0.0f;
        u32 spd = A + 0x1EC4;
        u32 pos = A + 0x16E4 + cur * 0x3F0;
        for (int i = 0; i < 0x54; i++, spd += 0xC, pos += 0xC) {
            gabi::store<f32>(spd, 0.0f);
            gabi::store<f32>(spd + 4, 0.0f);
            gabi::store<f32>(spd + 8, 0.0f);
            gabi::store<f32>(pos, gabi::load<f32>(l_pos_ + i * 0xC));
            gabi::store<f32>(pos + 4, gabi::load<f32>(l_pos_ + i * 0xC + 4));
            gabi::store<f32>(pos + 8, gabi::load<f32>(l_pos_ + i * 0xC + 8));
        }

        fopAc_ac_c* p = fopAcM_SearchByID(i_this->parentActorID);
        gabi::store<u32>(l_p_ship_, gabi::ea(p));
        /* HD: null check */
        if (p != nullptr && gabi::load<u8>(gabi::ea(p) + 0x3E6) == 0) { /* l_p_ship->m2CE */
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
VERIFY(0x02462194, daSail_Create);

/* 024628E8 */
static void __sinit_d_a_sail_cpp() {
    WWHD_FUNC(0x024628E8, void, (u32)0);
    sinit_header_statics(0x1046D770, 0x101CFDAC);
    /* daSail_HIO_c l_HIO */
    l_HIO().__vtbl = HIO_VTBL;
    l_HIO().mNo = -1;
    l_HIO().m05 = 1;
    l_HIO().m10 = 0.0f;
    l_HIO().m06 = 1;
    l_HIO().m07 = 0;
    __register_global_object(0x101CFDD0);
}
VERIFY(0x024628E8, __sinit_d_a_sail_cpp);

/* 024629C8: sead::SafeString deleting destructor (trivial) */
static void safestring_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024629C8, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024629C8, safestring_dt);

/* 02462EEC, 02463348: empty virtuals (02463348: sead::SafeString::assureTerminationImpl_) */
static void empty_virtual(void* p) { WWHD_FUNC(0x02462EEC, void, p); }
VERIFY(0x02462EEC, empty_virtual);
static void safestring_assureTermination(void* p) { WWHD_FUNC(0x02463348, void, p); }
VERIFY(0x02463348, safestring_assureTermination);

/* 02463320: daSail_HIO_c deleting destructor */
static void daSail_HIO_dt(daSail_HIO_c* p, s32 flags) {
    WWHD_FUNC(0x02463320, void, p, flags);
    if (p != nullptr) {
        p->mNo = -1;
        p->__vtbl = HIO_VTBL;
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02463320, daSail_HIO_dt);
