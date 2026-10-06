/**
 * d_a_vrbox2.cpp (WWHD)
 * Sky box: back clouds, horizon haze ("kasumi mae") and the false sea ("uso umi").
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_vrbox2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * HD rewrote most of this actor: the wind/scroll computation of daVrbox2_color_set moved to
 * Execute (result kept in an HD member), the material updates (inlined into Draw) look up
 * materials and texture-SRT animations by name and write HD constant-colour registers, and Draw
 * renders through an HD layer with its own projection (sky near/far), with stage-specific
 * height offsets.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x10041D9C    /* this TU's sead::SafeString vtable */
#define VRBOX2_VTBL 0x10041DF4        /* HD: vrbox2_class vtable */
#define ASSURE_TERMINATION 0x024D3BD0 /* this TU's SafeString::assureTerminationImpl_ */

struct vrbox2_class : fopAc_ac_c {
    /* 0x3AC */ u8 m290[4];
    /* 0x3B0 */ gptr<J3DModel> mpBackCloud;
    /* 0x3B4 */ u8 m298[4];
    /* 0x3B8 */ gptr<J3DModel> mpKasumiMae;
    /* 0x3BC */ u8 m2A0[4];
    /* 0x3C0 */ gptr<J3DModel> mpUsoUmi;
    /* 0x3C4 */ u8 m2A8[4];
    /* 0x3C8 */ be<f32> mScrollSpeed; /* HD: computed in Execute, applied (and cleared) in Draw */
};
WWHD_OFFSET(vrbox2_class, mpBackCloud, 0x3B0);
WWHD_OFFSET(vrbox2_class, mpUsoUmi, 0x3C0);
WWHD_OFFSET(vrbox2_class, mScrollSpeed, 0x3C8);

/* sead::Color4f */
struct Color4f_l {
    be<f32> r, g, b, a;
};
/* GXColorS10 */
struct GXColorS10_l {
    be<s16> r, g, b, a;
};
/* the HD layer projection object built on the stack in Draw (0xC4 bytes) */
struct SkyProjection_l {
    u8 _[0xC4];
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 envl() { return gabi::ea(dKy_getEnvlight()); }
static inline u32 play() { return dComIfGp_ea(); }
/* 027DF9B0 JUTNameTab::getIndex(name) (HD) */
static inline s32 JUTNameTab_getIndex(u32 tab, u32 name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
/* 0252447C dComIfG_getStageRes(arc, name) */
static inline u32 dComIfG_getStageRes(const char* arc, const char* name) { return gabi::call<u32>(0x0252447C, arc, name); }
/* 025C11DC dStage_roomControl_c::getStatusRoomDt(roomNo) */
static inline u32 dStage_roomControl_getStatusRoomDt(u32 rc, s32 roomNo) { return gabi::call<u32>(0x025C11DC, rc, roomNo); }
static inline u32 dKyw_get_wind_vec() { return gabi::call<u32>(0x0257DAA8); }
static inline f32 dKyw_get_wind_pow() { return gabi::call<f32>(0x02578348); }
static inline void dKyr_get_vectle_calc(cXyz* a, cXyz* b, cXyz* out) { gabi::call(0x02563F64, a, b, out); }
static inline f32 cM3d_VectorProduct2d(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2) {
    return gabi::call<f32>(0x02010CFC, x0, y0, x1, y1, x2, y2);
}
/* HD J3D material helpers (names unknown) */
static inline u32 matAnm_getTexSrt(u32 anm, s32 i) { return gabi::call<u32>(0x027FA7F8, anm, i); }    /* 027FA7F8 */
static inline void Color4f_scaleRGB(Color4f_l* out, Color4f_l* in, f32 s) { gabi::call(0x0274D458, out, in, s); } /* 0274D458 */
static inline u32 matColorReg(u32 flagsAddr, s32 i) { return gabi::call<u32>(0x027F9F0C, flagsAddr, i); }     /* 027F9F0C */
/* HD sky layer projection: constructor 027389F8, set 0274E04C(fovy, aspect, near?, far?),
 * copy 02738AC0, destructor 02738A6C */
static inline void SkyProjection_ct(SkyProjection_l* p) { gabi::call(0x027389F8, p); }
static inline void SkyProjection_set(SkyProjection_l* p, f32 a, f32 b, f32 c, f32 d) { gabi::call(0x0274E04C, p, a, b, c, d); }
static inline void SkyProjection_copy(SkyProjection_l* p, u32 src) { gabi::call(0x02738AC0, p, src); }
static inline void SkyProjection_dt(SkyProjection_l* p, s32 flags) { gabi::call(0x02738A6C, p, flags); }

static inline u32 vtbl_fn(u32 obj, u32 vtOff, u32 slot) { return gabi::load<u32>(gabi::load<u32>(obj + vtOff) + slot); }

/* sead::SafeStringBase<char>::isEqual (HD inline): lhs = a literal, rhs = the start stage name
 * (play+0x5134), both as SafeString temporaries of this TU */
static bool startStageIs(const char* name) {
    gabi::Local<SafeString> lhs;
    lhs->mStringTop = gabi::ea(name);
    lhs->__vtbl = SAFESTRING_VTBL;
    u32 p = play();
    gabi::Local<SafeString> rhs;
    rhs->mStringTop = p + 0x5134;
    rhs->__vtbl = SAFESTRING_VTBL;
    gabi::call_ptr(vtbl_fn(gabi::ea(lhs.get()), 4, 0x14), lhs.get()); /* assureTerminationImpl_ */
    gabi::call_ptr(vtbl_fn(gabi::ea(lhs.get()), 4, 0x14), lhs.get()); /* cstr() */
    u32 a = lhs->mStringTop;
    gabi::call_ptr(vtbl_fn(gabi::ea(rhs.get()), 4, 0x14), rhs.get()); /* str.cstr() */
    if (a == rhs->mStringTop) {
        return true;
    }
    u32 b = rhs->mStringTop;
    a = lhs->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(a + i);
        if (c != gabi::load<u8>(b + i)) {
            return false;
        }
        if (c == 0) {
            return true;
        }
    }
    return false;
}

/* 024D1C44 */
void texScrollCheck(be<f32>* v) {
    WWHD_FUNC(0x024D1C44, void, v);
    f32 f = *v;
    if (f < 0.0f) {
        do {
            f += 1.0f;
        } while (f < 0.0f);
        *v = f;
    }
    if (f > 1.0f) {
        do {
            f -= 1.0f;
        } while (f > 1.0f);
        *v = f;
    }
}
VERIFY(0x024D1C44, texScrollCheck);

/* 024D1C90 (not named by the matcher): sead::Color4f from a GXColor (each channel / 255) */
static void Color4f_setGXColor(Color4f_l* out, GXColor* c) {
    WWHD_FUNC(0x024D1C90, void, out, c);
    f32 r = (f32)(u8)c->r / 255.0f;
    f32 g = (f32)(u8)c->g / 255.0f;
    f32 b = (f32)(u8)c->b / 255.0f;
    f32 a = (f32)(u8)c->a / 255.0f;
    out->r = r;
    out->g = g;
    out->b = b;
    out->a = a;
}
VERIFY(0x024D1C90, Color4f_setGXColor);

/* ---- Draw helpers (daVrbox2_color_set, HD) ---- */
static bool envColorsZero() {
    u32 e1 = envl();
    u32 e2 = envl();
    s32 sum = gabi::load<u8>(e1 + 0xBA0) + gabi::load<u8>(e2 + 0xBA1); /* mVrKasumiMaeColor */
    sum += gabi::load<u8>(envl() + 0xBA2);
    sum += gabi::load<u8>(envl() + 0xB90); /* mVrSkyColor */
    sum += gabi::load<u8>(envl() + 0xB91);
    sum += gabi::load<u8>(envl() + 0xB92);
    sum += gabi::load<u8>(envl() + 0xB98); /* mVrkumoColor */
    sum += gabi::load<u8>(envl() + 0xB99);
    sum += gabi::load<u8>(envl() + 0xB9A);
    return sum == 0;
}

/* J3DModelData::getMaterialNodePointer(getMaterialName()->getIndex(name)) (HD inline) */
static u32 getMaterialByName(u32 md, const char* name) {
    gabi::Local<SafeString> s;
    s->mStringTop = gabi::ea(name);
    s->__vtbl = SAFESTRING_VTBL;
    u32 hdr = gabi::load<u32>(md);
    gabi::call(ASSURE_TERMINATION, s.get());
    s32 off = gabi::load<s32>(hdr + 0x18);
    u32 tab = off != 0 ? hdr + 0x18 + off : 0;
    s32 idx = JUTNameTab_getIndex(tab, s->mStringTop);
    if (idx < 0) {
        return 0;
    }
    u32 base = gabi::load<u32>(md + 0x10);
    if ((u32)idx < gabi::load<u32>(md + 0xC)) {
        return base + idx * 0x39C;
    }
    return base;
}

/* the model's material animation (0x3C each, +0x34) for a material name (name table at +0x14) */
static u32 getMatAnmByName(u32 model, const char* name) {
    u32 t = gabi::load<u32>(model + 0x14);
    s32 off = gabi::load<s32>(t + 0x18);
    u32 tab = off != 0 ? t + 0x18 + off : 0;
    s32 idx = JUTNameTab_getIndex(tab, gabi::ea(name));
    if (idx < 0) {
        return 0;
    }
    return gabi::load<u32>(model + 0x34) + idx * 0x3C;
}

static void scrollTexSrt(vrbox2_class* i_this, u32 anm, s32 i, s32 line) {
    u32 tex_srt = matAnm_getTexSrt(anm, i);
    if (tex_srt == 0) /* JUT_ASSERT(line, tex_srt != 0) */
        JUT_ASSERT_fail(STR(0x10041E20), line, STR(0x10041E30));
    f32 v = gabi::load<f32>(tex_srt + 0x10) + i_this->mScrollSpeed; /* mSRT.mTranslationX */
    gabi::store<f32>(tex_srt + 0x10, v);
    texScrollCheck(gabi::at<be<f32>>(tex_srt + 0x10));
}

/* mat->setTevKColor(0, k) (virtual on the material's TEV block) and the HD constant colour
 * register 7 = rgb(k) * scale, alpha k.a */
static void setKColor(u32 mat, GXColor* k, f32 scale) {
    u32 tev = gabi::load<u32>(mat + 0x18);
    gabi::call_ptr(vtbl_fn(tev, 4, 0x3C), tev, 0, k);
    gabi::Local<Color4f_l> c;
    Color4f_setGXColor(c.get(), k);
    gabi::Local<Color4f_l> s;
    Color4f_scaleRGB(s.get(), c.get(), scale);
    u32 flags = gabi::load<u32>(mat + 0xA0) | 0x80;
    gabi::store<u32>(mat + 0xA0, flags);
    u32 reg = matColorReg(mat + 0xA0, 7);
    f32 a = (f32)(u8)k->a / 255.0f;
    gabi::store<u32>(reg + 0x0, gabi::load<u32>(gabi::ea(s.get()) + 0x0)); /* lfs/stfs: a bit copy in the recompiled code */
    gabi::store<u32>(reg + 0x4, gabi::load<u32>(gabi::ea(s.get()) + 0x4)); /* lfs/stfs: a bit copy in the recompiled code */
    gabi::store<u32>(reg + 0x8, gabi::load<u32>(gabi::ea(s.get()) + 0x8)); /* lfs/stfs: a bit copy in the recompiled code */
    gabi::store<f32>(reg + 0xC, a);
}

static void cloudMaterial(vrbox2_class* i_this, u32 md, J3DModel* pBackCloud, const char* matName, const char* anmName,
                          s32 line0, s32 line1, GXColor* k0) {
    u32 mat = getMaterialByName(md, matName);
    u32 anm = getMatAnmByName(gabi::ea(pBackCloud), anmName);
    scrollTexSrt(i_this, anm, 0, line0);
    scrollTexSrt(i_this, anm, 1, line1);
    f32 scale = gabi::load<f32>(envl() + 0x10DC);
    setKColor(mat, k0, scale);
}

/* daVrbox2_color_set (HD, inlined into Draw) */
static void color_set(vrbox2_class* i_this, J3DModel* pBackCloud) {
    if (envColorsZero()) {
        return;
    }
    u32 md = gabi::ea(J3DModel_getModelData(i_this->mpBackCloud));
    gabi::Local<GXColor> k0;

    /* lambert121: material and its texture SRT animation */
    u32 mat = getMaterialByName(md, STR(0x10041E14) /* "lambert121" */);
    u32 anm = getMatAnmByName(gabi::ea(pBackCloud), STR(0x10041E14));
    scrollTexSrt(i_this, anm, 0, 0x1B1);
    scrollTexSrt(i_this, anm, 1, 0x1B9);
    k0->r = gabi::load<u8>(envl() + 0xB98);
    k0->g = gabi::load<u8>(envl() + 0xB99);
    u32 e = envl();
    k0->a = 0xFF;
    k0->b = gabi::load<u8>(e + 0xB9A);
    f32 scale = gabi::load<f32>(envl() + 0x10DC);
    setKColor(mat, k0.get(), scale);

    /* lambert122 (HD: scrolled by the animation of lambert121, at the full speed) */
    cloudMaterial(i_this, md, pBackCloud, STR(0x10041E40) /* "lambert122" */, STR(0x10041E14), 0x1F5, 0x1FD, k0.get());
    /* lambert121_2_ */
    cloudMaterial(i_this, md, pBackCloud, STR(0x10041E04) /* "lambert121_2_" */, STR(0x10041E04), 0x233, 0x23B, k0.get());

    J3DModel* kasumi = i_this->mpKasumiMae;
    if (kasumi != nullptr) {
        u32 kmat = gabi::load<u32>(gabi::ea(J3DModel_getModelData(kasumi)) + 0x10); /* getMaterialNodePointer(0) */
        gabi::Local<GXColorS10_l> c0;
        c0->r = gabi::load<u8>(envl() + 0xBA0);
        c0->g = gabi::load<u8>(envl() + 0xBA1);
        c0->b = gabi::load<u8>(envl() + 0xBA2);
        u32 e2 = envl();
        k0->a = 0;
        k0->r = gabi::load<u8>(e2 + 0xB9B); /* mVrkumoColor.a */
        k0->g = 0;
        k0->b = 0;
        f32 cscale = gabi::load<f32>(envl() + 0x10E4);
        u32 tev = gabi::load<u32>(kmat + 0x18);
        gabi::call_ptr(vtbl_fn(tev, 4, 0x24), tev, 0, c0.get()); /* setTevColor(0, c0) */
        gabi::Local<Color4f_l> cf;
        {
            f32 r = (f32)(s16)c0->r / 255.0f;
            f32 g = (f32)(s16)c0->g / 255.0f;
            f32 b = (f32)(s16)c0->b / 255.0f;
            f32 a = (f32)(s16)c0->a / 255.0f;
            cf->r = r;
            cf->g = g;
            cf->b = b;
            cf->a = a;
        }
        gabi::Local<Color4f_l> cs;
        Color4f_scaleRGB(cs.get(), cf.get(), cscale);
        u32 flags = gabi::load<u32>(kmat + 0xA0) | 0x10;
        gabi::store<u32>(kmat + 0xA0, flags);
        u32 reg = matColorReg(kmat + 0xA0, 4);
        f32 a = (f32)(s16)c0->a / 255.0f;
        gabi::store<u32>(reg + 0x0, gabi::load<u32>(gabi::ea(cs.get()) + 0x0)); /* lfs/stfs: a bit copy in the recompiled code */
        gabi::store<u32>(reg + 0x4, gabi::load<u32>(gabi::ea(cs.get()) + 0x4)); /* lfs/stfs: a bit copy in the recompiled code */
        gabi::store<u32>(reg + 0x8, gabi::load<u32>(gabi::ea(cs.get()) + 0x8)); /* lfs/stfs: a bit copy in the recompiled code */
        gabi::store<f32>(reg + 0xC, a);
        setKColor(kmat, k0.get(), 1.0f);
    }

    J3DModel* uso = i_this->mpUsoUmi;
    if (uso != nullptr) {
        u32 umat = gabi::load<u32>(gabi::ea(J3DModel_getModelData(uso)) + 0x10);
        k0->r = gabi::load<u8>(envl() + 0xB94); /* mVrUsoUmiColor */
        k0->g = gabi::load<u8>(envl() + 0xB95);
        u32 e3 = envl();
        k0->a = 0xFF;
        k0->b = gabi::load<u8>(e3 + 0xB96);
        f32 uscale = gabi::load<f32>(envl() + 0x10D8);
        setKColor(umat, k0.get(), uscale);
    }
    i_this->mScrollSpeed = 0.0f;
}

static inline void setModelTRMtx(J3DModel* m) {
    J3DModel_setBaseTRMtx(m, mDoMtx_stack_c::get());
    mDoExt_modelUpdateDL(m);
}

/* 024D1D44 */
static BOOL daVrbox2_Draw(vrbox2_class* i_this) {
    WWHD_FUNC(0x024D1D44, BOOL, i_this);
    J3DModel* pUsoUmi = i_this->mpUsoUmi;
    J3DModel* pBackCloud = i_this->mpBackCloud;
    J3DModel* pKasumiMae = i_this->mpKasumiMae;
    f32 y_origin = 0.0f;

    color_set(i_this, pBackCloud);

    if (envColorsZero()) {
        return TRUE;
    }

    s32 roomNo = gabi::load<s8>(0x1047E6C8); /* dComIfGp_roomControl_getStayNo() */
    if (roomNo >= 0) {
        u32 rc = dStage_roomControl_getStatusRoomDt(play() + 0x51CC, roomNo);
        if (rc != 0) {
            u32 fili = gabi::call_ptr<u32>(vtbl_fn(rc, 0, 0x1DC), rc); /* getFileListInfo() */
            if (fili != 0) {
                y_origin = gabi::load<f32>(fili + 4); /* mSeaLevel */
            }
        }
    }

    f32 y_offset;
    if (gabi::load<u32>(play() + 0x5FA4) != 0) { /* dComIfGd_getView() */
        u32 view = gabi::load<u32>(play() + 0x5FA4);
        y_offset = (gabi::load<f32>(view + 0x190) - y_origin) * 0.09f;
    } else {
        y_offset = 0.0f;
    }

    /* HD: stage-specific offsets */
    if (startStageIs(STR(0x10041E90) /* "A_mori" */)) {
        y_offset += 30000.0f;
    }
    if (startStageIs(STR(0x10041E80) /* "sea_E" */)) {
        u32 view = gabi::load<u32>(play() + 0x5FA4);
        f32 r = gabi::load<f32>(view + 0xD4) / 60.0f; /* fovy / 60 */
        if (r > 1.0f) {
            y_offset = gabi::fmadds(16600.0f, (r - 1.0f) / 0.854871988f, y_offset);
        }
    }
    bool special = true;
    if (!startStageIs(STR(0x10041E78) /* "M_NewD2" */)) {
        if (!startStageIs(STR(0x10041E88) /* "Siren" */)) {
            special = false;
        }
    }

    if (special) {
        /* HD: the clear colour of a sead layer (RTTI-checked) is the false-sea colour */
        u32 tbl = gabi::load<u32>(0x104B45C0 + 0x148);
        s16 idx = gabi::load<s16>(gabi::load<u32>(0x104A1460));
        u32 cnt = gabi::load<u32>(tbl + 8);
        u32 arr = gabi::load<u32>(tbl + 0xC);
        u32 ent = (u32)idx < cnt ? arr + (u32)idx * 4 : arr;
        u32 obj = 0;
        if (gabi::load<u16>(ent + 2) != 0) {
            u32 i2 = gabi::load<u16>(ent);
            u32 p = 0;
            if (i2 < gabi::load<u32>(tbl + 0x10)) {
                p = gabi::load<u32>(tbl + 0x14) + i2 * 4;
            }
            obj = gabi::load<u32>(p);
        }
        if (gabi::load<u32>(0x101FD7DC) == 0) { /* function-local static RTTI */
            gabi::store<u32>(0x101FD7DC, 1);
            gabi::store<u32>(0x101FDCE0, 0x10041DD4);
        }
        if (obj != 0) {
            if (gabi::call_ptr<u32>(vtbl_fn(obj, 0x58, 0x44), obj, 0x101FDCE0) == 0) { /* checkDerivedRuntimeTypeInfo */
                obj = 0;
            }
        }
        gabi::Local<GXColor> k;
        k->r = gabi::load<u8>(envl() + 0xB94);
        k->g = gabi::load<u8>(envl() + 0xB95);
        u32 e = envl();
        k->a = 0xFF;
        k->b = gabi::load<u8>(e + 0xB96);
        gabi::Local<Color4f_l> c;
        Color4f_setGXColor(c.get(), k.get());
        gabi::store<u32>(obj + 0x114, gabi::load<u32>(gabi::ea(c.get()) + 0x0));
        gabi::store<u32>(obj + 0x118, gabi::load<u32>(gabi::ea(c.get()) + 0x4));
        gabi::store<u32>(obj + 0x11C, gabi::load<u32>(gabi::ea(c.get()) + 0x8));
        gabi::store<u32>(obj + 0x120, gabi::load<u32>(gabi::ea(c.get()) + 0xC));
    }

    {
        /* mDoMtx_stack_c::transS(view->mInvViewMtx[0][3], [1][3] - y_offset, [2][3]) */
        f32 x = gabi::load<f32>(gabi::load<u32>(play() + 0x5FA4) + 0x180);
        f32 y = gabi::load<f32>(gabi::load<u32>(play() + 0x5FA4) + 0x190) - y_offset;
        f32 z = gabi::load<f32>(gabi::load<u32>(play() + 0x5FA4) + 0x1A0);
        mDoMtx_stack_c::transS(x, y, z);
    }

    /* HD: draw through the sky layer with its own projection */
    u32 mgr = gabi::load<u32>(0x101F95D0);
    u32 n = gabi::load<u32>(mgr + 0x1020);
    u32 lp = gabi::load<u32>(mgr + 0x1024);
    if (n > 1) {
        lp += 4;
    }
    u32 layer = gabi::load<u32>(lp);
    if (layer != 0) {
        u32 lflags = gabi::load<u32>(layer + 0x50);
        u32 saved = gabi::load<u32>(layer + 0x4C);
        u32 cam;
        if (((lflags >> 12) & 1) == 0) {
            cam = saved != 0 ? saved : 0x104A20FC;
        } else {
            cam = gabi::load<u32>(layer + 0x164);
        }
        gabi::Local<SkyProjection_l> proj;
        u32 pj = gabi::ea(proj.get());
        SkyProjection_ct(proj.get());
        SkyProjection_set(proj.get(), gabi::load<f32>(cam + 0x94), gabi::load<f32>(cam + 0x98), gabi::load<f32>(cam + 0x9C),
                          gabi::load<f32>(cam + 0xAC));
        gabi::store<u32>(pj + 0xB0, gabi::load<u32>(cam + 0xB0)); /* lfs/stfs */
        gabi::store<u32>(pj + 0xB4, gabi::load<u32>(cam + 0xB4));
        gabi::store<u8>(pj + 0, 1);
        if (gabi::load<u32>(0x101FD9F0) == 0) { /* function-local static RTTI */
            gabi::store<u32>(0x101FD9F0, 1);
            gabi::store<u32>(0x101FDCD4, 0x10041DE4);
        }
        if (gabi::call_ptr<u32>(vtbl_fn(cam, 0x90, 0xC), cam, 0x101FDCD4) != 0 && cam != 0) {
            SkyProjection_copy(proj.get(), cam + 0xB8);
        }
        gabi::store<f32>(pj + 0x94, 0.0001f); /* near */
        gabi::store<f32>(pj + 0x98, 16000.0f); /* far */
        gabi::store<u8>(pj + 0, 1);
        gabi::store<u32>(layer + 0x4C, pj);

        /* dComIfGd_setListSky() */
        gabi::store<u32>(0x104B4634, gabi::load<u32>(play() + 0x5D4C));
        gabi::store<u32>(0x104B4638, gabi::load<u32>(play() + 0x5D50));

        if (pUsoUmi != nullptr) {
            setModelTRMtx(pUsoUmi);
        }
        if (pKasumiMae != nullptr) {
            setModelTRMtx(pKasumiMae);
        }
        mDoMtx_stack_c::transM(0.0f, 100.0f, 0.0f);
        setModelTRMtx(pBackCloud);

        gabi::store<u32>(layer + 0x4C, saved);
        SkyProjection_dt(proj.get(), 2);
    }

    dComIfGd_setList();

    bool special2 = true;
    if (!startStageIs(STR(0x10041E78) /* "M_NewD2" */)) {
        if (!startStageIs(STR(0x10041E88) /* "Siren" */)) {
            special2 = false;
        }
    }
    if (special2) {
        gabi::call(0x0255F84C); /* HD */
    }
    return TRUE;
}
VERIFY(0x024D1D44, daVrbox2_Draw);

/* 024D3248: HD: the wind part of daVrbox2_color_set */
static BOOL daVrbox2_Execute(vrbox2_class* i_this) {
    WWHD_FUNC(0x024D3248, BOOL, i_this);
    u32 pCamera = gabi::load<u32>(play() + 0x5AF8); /* dComIfGp_getCamera(0) */
    u32 windVec = dKyw_get_wind_vec();
    f32 windPow = dKyw_get_wind_pow();
    f32 windX_ = gabi::load<f32>(windVec + 0);
    f32 windZ_ = gabi::load<f32>(windVec + 8);

    if (!startStageIs(STR(0x10041EBC) /* "Name" */)) {
        u32 sd = play() + 0x5150;
        u32 stag = gabi::call_ptr<u32>(vtbl_fn(sd, 0, 0x15C), sd); /* getStagInfo() */
        if (((gabi::load<u32>(stag + 0xC) >> 16) & 7) == 2 /* dStageType_MISC_e */) {
            s16 stageWindY = 0;
            if (startStageIs(STR(0x10041EC4) /* "LinkRM" */))
                stageWindY = 0x4000;
            else if (startStageIs(STR(0x10041ECC) /* "Orichh" */))
                stageWindY = -0x4000;
            else if (startStageIs(STR(0x10041EA4) /* "Ojhous2" */))
                stageWindY = 0x7fff;
            else if (startStageIs(STR(0x10041ED4) /* "Omasao" */))
                stageWindY = -0x4000;
            else if (startStageIs(STR(0x10041EAC) /* "Onobuta" */))
                stageWindY = 0x4000;

            s32 windX;
            s16 windY;
            u32 sv = gabi::load<u32>(0x101F84DC);
            if (gabi::load<s16>(sv + 0x4A) == -1 && gabi::load<s16>(sv + 0x4C) == -1) { /* dComIfGs_getWindX/Y */
                windX = 0;
                windY = 0;
            } else {
                windX = gabi::load<u16>(envl() + 0xA24); /* mWind.mTactWindAngleX */
                windY = gabi::load<s16>(envl() + 0xA26); /* mWind.mTactWindAngleY */
            }
            windY += stageWindY;
            windX_ = cM_scos(windX) * cM_scos(windY);
            windZ_ = cM_scos(windX) * cM_ssin(windY);
            windPow = 0.6f;
        }
    }

    gabi::Local<cXyz> eyePosXZ;
    gabi::Local<cXyz> centerPosXZ;
    gabi::Local<cXyz> lookDirXZ;
    for (int i = 0; i < 3; i++) {
        gabi::store<u32>(gabi::ea(eyePosXZ.get()) + 4 * i, gabi::load<u32>(pCamera + 0xDC + 4 * i));     /* view.mLookat.mEye */
        gabi::store<u32>(gabi::ea(centerPosXZ.get()) + 4 * i, gabi::load<u32>(pCamera + 0xE8 + 4 * i));  /* view.mLookat.mCenter */
    }
    eyePosXZ->y = 0.0f;
    centerPosXZ->y = 0.0f;

    dKyr_get_vectle_calc(eyePosXZ.get(), centerPosXZ.get(), lookDirXZ.get());
    f32 windDirView = cM3d_VectorProduct2d(0.0f, 0.0f, -windX_, -windZ_, lookDirXZ->x, lookDirXZ->z) * 0.0005f;
    f32 scrollSpeed = windDirView * windPow;
    if (startStageIs(STR(0x10041EB4) /* "M_DragB" */))
        scrollSpeed = -0.0004f;
    i_this->mScrollSpeed = scrollSpeed;
    return TRUE;
}
VERIFY(0x024D3248, daVrbox2_Execute);

/* 024D3920 */
static BOOL daVrbox2_IsDelete(vrbox2_class*) {
    WWHD_FUNC(0x024D3920, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024D3920, daVrbox2_IsDelete);

/* 024D3928 */
static BOOL daVrbox2_Delete(vrbox2_class*) {
    WWHD_FUNC(0x024D3928, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024D3928, daVrbox2_Delete);

/* 024D3930 */
static BOOL daVrbox2_solidHeapCB(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x024D3930, BOOL, i_actor);
    vrbox2_class* i_this = static_cast<vrbox2_class*>(i_actor);
    const char* arc = STR(0x10041EDC); /* "Stage" */

    u32 modelData = dComIfG_getStageRes(arc, STR(0x10041EE4) /* "vr_back_cloud.bmd" */);
    if (modelData == 0) /* JUT_ASSERT(0x324, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x10041F0C), 0x324, STR(0x10041F1C));
    i_this->mpBackCloud = mDoExt_J3DModel__create(gabi::at<J3DModelData>(modelData), 0x80000, 0x11020202);

    modelData = dComIfG_getStageRes(arc, STR(0x10041EF8) /* "vr_kasumi_mae.bmd" */);
    if (modelData != 0)
        i_this->mpKasumiMae = mDoExt_J3DModel__create(gabi::at<J3DModelData>(modelData), 0x80000, 0x11020202);
    modelData = dComIfG_getStageRes(arc, STR(0x10041F30) /* "vr_uso_umi.bmd" */);
    if (modelData != 0)
        i_this->mpUsoUmi = mDoExt_J3DModel__create(gabi::at<J3DModelData>(modelData), 0x80000, 0x11020202);

    i_this->mScrollSpeed = 0.0f; /* HD */
    BOOL success = FALSE;
    if (i_this->mpBackCloud != nullptr && i_this->mpKasumiMae != nullptr && i_this->mpUsoUmi != nullptr) {
        success = TRUE;
    }
    return success;
}
VERIFY(0x024D3930, daVrbox2_solidHeapCB);

/* 024D3A48 */
static cPhs_State daVrbox2_Create(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x024D3A48, cPhs_State, i_actor);
    /* fopAcM_ct(i_actor, vrbox2_class) */
    if (!fopAcM_CheckCondition(i_actor, fopAcCnd_INIT_e)) {
        if (i_actor != nullptr) {
            fopAc_ac_c_ct(i_actor);
            i_actor->__vtbl = VRBOX2_VTBL;
        }
        fopAcM_OnCondition(i_actor, fopAcCnd_INIT_e);
    }
    cPhs_State phase_state = cPhs_COMPLEATE_e;
    if (!fopAcM_entrySolidHeap(i_actor, 0x024D3930 /* daVrbox2_solidHeapCB */, 0x21a0))
        phase_state = cPhs_ERROR_e;
    return phase_state;
}
VERIFY(0x024D3A48, daVrbox2_Create);

/* 024D3AD4 */
static void __sinit_d_a_vrbox2_cpp() {
    WWHD_FUNC(0x024D3AD4, void, (u32)0);
    sinit_header_statics(0x1046EABC, 0x101D2C00);
}
VERIFY(0x024D3AD4, __sinit_d_a_vrbox2_cpp);

/* 024D3B68: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024D3B68, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x024D3B68, SafeString_dt);

/* 024D3B7C: vrbox2_class deleting destructor (compiler-generated) */
static void vrbox2_class_dt(vrbox2_class* i_this, s32 flags) {
    WWHD_FUNC(0x024D3B7C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024D3B7C, vrbox2_class_dt);

/* 024D3BD0: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x024D3BD0, void, (u32)0);
}
VERIFY(0x024D3BD0, SafeString_assureTerminationImpl);
