/**
 * d_a_bgn_draw.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 1): daBgn_Draw with the inlined water0_disp, room_disp, obj_disp,
 * daBgn_DrawS, daBgn2_Draw and daBgn3_Draw.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bgn.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bgn.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* mDoGph_gInf_c blur (HD): rate byte 0x101F4826, flag byte 0x101F4825; 025F064C onBlure */
static inline void bgn_setBlureRate(u8 rate) { gabi::store<u8>(0x101F4826, rate); }
static inline void bgn_onBlure() { gabi::call(0x025F064C); }
static inline void bgn_offBlure() { gabi::store<u8>(0x101F4825, 0); }
/* 0274D458 (HD): colour vector (four floats) scaled by an intensity into a GX2 uniform vector */
static inline void bgn_color_scale(u32 dst, u32 src, f32 k) { gabi::call(0x0274D458, dst, src, k); }
/* 027F9F0C (HD J3D): the uniform slot `idx` of a material's register block (dirty flags at +0) */
static inline u32 bgn_uniform_slot(u32 flags, s32 idx) { return gabi::call<u32>(0x027F9F0C, flags, idx); }
/* 025ED1BC mDoExt_3DlineMat1_c::update(u16 n, const GXColor& color, dKy_tevstr_c*) */
static inline void bgn_lineMat_update(u32 mat, u32 color, u32 tev) { gabi::call(0x025ED1BC, mat, 60, color, tev); }
/* dComIfGd_set3DlineMat (HD inline): play+0x5FB4 sort packets (0x9C each) indexed by the material id
 * (virtual slot 0x14 of the line material's second vtable at +0x130); 025EDD04 setMat */
static inline void bgn_set3DlineMat(u32 mat) {
    u32 sp = dComIfGp_ea() + 0x5FB4;
    s32 id = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(mat + 0x130) + 0x14), mat);
    gabi::call(0x025EDD04, sp + id * 0x9C, mat);
}
/* 025E5590 mDoExt_McaMorf::entryDL */
static inline void bgn_morf_entryDL(u32 morf) { gabi::call(0x025E5590, morf); }
static inline void bgn_update(u32 pkt) { mDoExt_J3DModelPacketS_update(gabi::at<mDoExt_J3DModelPacketS_l>(pkt)); }
static inline dKy_tevstr_c* bgn_tev(u32 a) { return gabi::at<dKy_tevstr_c>(a); }

/* the arrow-hit fog flash shared by the three bodies and the parts (GameCube: written out each time) */
static inline void bgn_fog_dim(u32 tev, f32 dim, be<f32>* dimp) {
    if (dim > 0.0f) {
        gabi::store<s16>(tev + 0xA4, 0);
        gabi::store<s16>(tev + 0xA2, 0);
        gabi::store<s16>(tev + 0xA0, 0);
        gabi::store<f32>(tev + 0xA8, gabi::fmadds(-50000.0f, *dimp, gabi::load<f32>(tev + 0xA8)));
    }
}
static inline void bgn_fog_flash(u32 tev, be<s16>* c, be<f32>* z, s16 timer, s16 thr, s16 step0, f32 z0step) {
    s16 cc = *c;
    s16 r = (s16)(gabi::load<s16>(tev + 0xA0) + cc);
    s16 g = gabi::load<s16>(tev + 0xA2);
    if (r > 0xFF)
        r = 0xFF;
    s16 gg = (s16)(g + cc);
    if (gg > 0xFF)
        gg = 0xFF;
    s16 b = (s16)(g + cc / 2);
    if (b > 0xFF)
        b = 0xFF;
    if (timer > thr) {
        cLib_addCalcAngleS2(c, 0x118, 1, 0x1E);
        cLib_addCalc2(z, -50000.0f, 1.0f, 5000.0f);
    } else {
        cLib_addCalcAngleS2(c, 0, 1, step0);
        cLib_addCalc0(z, 1.0f, z0step);
    }
    f32 fz = gabi::load<f32>(tev + 0xA8);
    gabi::store<s16>(tev + 0xA0, (s16)(r & 0xFF));
    gabi::store<s16>(tev + 0xA2, (s16)(gg & 0xFF));
    gabi::store<s16>(tev + 0xA4, (s16)(b & 0xFF));
    gabi::store<f32>(tev + 0xA8, gabi::fadds_ppc(fz, *z));
}

/* SafeString::isEqual as GHS expands it (see the packet draw): `a` is made terminated (directly the first
 * time, then through the vtable), again through the vtable, then `b`; byte compare up to 0x40001 */
static bool bgn_name_is(SafeString* a, SafeString* b, u32 lit, bool first) {
    b->mStringTop = lit;
    b->__vtbl = BGN_SAFESTRING_VTBL;
    if (first)
        SafeString_assureTermination(a);
    else
        gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), gabi::ea(a));
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), gabi::ea(a));
    u32 p = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), gabi::ea(b));
    u32 q = b->mStringTop;
    if (p == q)
        return true;
    for (u32 n = 0x40001; n != 0; n--, p++, q++) {
        u8 c1 = gabi::load<u8>(p);
        u8 c2 = gabi::load<u8>(q);
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
    }
    return false;
}

/* water0_disp (inline). HD: only the opaque sky list is switched; the material's blend/z/colour state is
 * set through the HD J3D API and the colours are pushed into the material's uniform block */
static inline void bgn_water0_disp(bgn_class* i_this) {
    settingTevStruct(dKy_getEnvlight(), 2 /* TEV_TYPE_BG1 */, gabi::at<cXyz>(0x10461CB8) /* w_pos */, (dKy_tevstr_c*)&i_this->mWaterTevStr);
    MtxTrans(0.0f, REG0_F(11), 0.0f, false);
    J3DModel_setBaseTRMtx(i_this->mpWater0Model, calc_mtx());
    setLightTevColorType(dKy_getEnvlight(), i_this->mpWater0Model, (dKy_tevstr_c*)&i_this->mWaterTevStr);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5DA0));
    u32 node = gabi::load<u32>(gabi::load<u32>(gabi::ea(i_this->mpWater0Model.get()) + 0xAC) + 0x10);
    u32 mat = gabi::load<u32>(node);
    u32 tevobj = gabi::load<u32>(node + 0x18);
    s32 off = gabi::load<s32>(mat + 0x20);
    u32 pe = off != 0 ? mat + 0x20 + off : 0;
    u32 tev_col = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(tevobj + 4) + 0x34), tevobj, 0);
    tevobj = gabi::load<u32>(node + 0x18);
    u32 tev_kcol = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(tevobj + 4) + 0x4C), tevobj, 0);
    tevobj = gabi::load<u32>(node + 0x18);
    u32 tev_kcol2 = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(tevobj + 4) + 0x4C), tevobj, 3);
    gabi::call(0x027E212C, pe, 3);        /* blend->setType(GX_BM_BLEND) */
    gabi::call(0x027E19B4, pe + 0x18, 4); /* source factors: GX_BL_SRC_ALPHA */
    gabi::call(0x027E19E4, pe + 0x18, 4);
    gabi::call(0x027E19C4, pe + 0x18, 5); /* destination factors: GX_BL_INV_SRC_ALPHA */
    gabi::call(0x027E19F4, pe + 0x18, 5);
    gabi::call(0x027E18F8, pe + 8, 1);
    gabi::call(0x027E1908, pe + 8, 3);
    gabi::call(0x027E195C, pe + 0x14, 1); /* zMode->setUpdateEnable(1) */
    gabi::store<s16>(tev_col + 4, 0);
    gabi::store<s16>(tev_col, 0);
    gabi::store<s16>(tev_col + 2, 0);
    gabi::store<u8>(tev_kcol + 2, 0xFF);
    gabi::store<u8>(tev_kcol + 1, 0xFF);
    gabi::store<u8>(tev_kcol, 0xFF);
    f32 fVar1 = l_HIO().m010;
    if (i_this->m02B5 == 1 && l_HIO().m00D == 0) {
        f32 y = gabi::at<fopAc_ac_c>(bgn2_g())->current.pos.y;
        fVar1 = gabi::fmuls_ppc(gabi::fadds_ppc(gabi::fadds_ppc(REG0_F(4), 4000.0f), y), gabi::fadds_ppc(REG0_F(5), 0.01f));
        if (fVar1 > 90.0f)
            fVar1 = 90.0f;
        else if (fVar1 < 50.0f)
            fVar1 = 50.0f;
    }
    gabi::store<u8>(tev_kcol2 + 3, (u8)gabi::ftoi(gabi::fmuls_ppc(fVar1, 2.559f)));
    gabi::Local<f32[4]> c98, c68, ca8, c78, cb8, c88;
    /* tev colour 0 -> uniform 4 */
    tevobj = gabi::load<u32>(node + 0x18);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(tevobj + 4) + 0x24), tevobj, 0, tev_col);
    bgn_colorS10_to_f(gabi::at<be<f32>>(c98.a), gabi::at<be<s16>>(tev_col));
    bgn_color_scale(c68.a, c98.a, 1.0f);
    gabi::store<u32>(node + 0xA0, gabi::load<u32>(node + 0xA0) | 0x10);
    u32 u = bgn_uniform_slot(node + 0xA0, 4);
    f32 a = (f32)(s32)gabi::load<s16>(tev_col + 6) / 255.0f;
    gabi::store<f32>(u, gabi::load<f32>(c68.a));
    gabi::store<f32>(u + 4, gabi::load<f32>(c68.a + 4));
    gabi::store<f32>(u + 8, gabi::load<f32>(c68.a + 8));
    gabi::store<f32>(u + 0xC, a);
    /* konst colour 0 -> uniform 7 */
    tevobj = gabi::load<u32>(node + 0x18);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(tevobj + 4) + 0x3C), tevobj, 0, tev_kcol);
    bgn_color_to_f(gabi::at<be<f32>>(ca8.a), gabi::at<be<u8>>(tev_kcol));
    bgn_color_scale(c78.a, ca8.a, 1.0f);
    gabi::store<u32>(node + 0xA0, gabi::load<u32>(node + 0xA0) | 0x80);
    u = bgn_uniform_slot(node + 0xA0, 7);
    a = (f32)(u32)gabi::load<u8>(tev_kcol + 3) / 255.0f;
    gabi::store<f32>(u, gabi::load<f32>(c78.a));
    gabi::store<f32>(u + 4, gabi::load<f32>(c78.a + 4));
    gabi::store<f32>(u + 8, gabi::load<f32>(c78.a + 8));
    gabi::store<f32>(u + 0xC, a);
    /* konst colour 3 -> uniform 10 */
    tevobj = gabi::load<u32>(node + 0x18);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(tevobj + 4) + 0x3C), tevobj, 3, tev_kcol2);
    bgn_color_to_f(gabi::at<be<f32>>(cb8.a), gabi::at<be<u8>>(tev_kcol2));
    bgn_color_scale(c88.a, cb8.a, 1.0f);
    gabi::store<u32>(node + 0xA0, gabi::load<u32>(node + 0xA0) | 0x400);
    u = bgn_uniform_slot(node + 0xA0, 10);
    a = (f32)(u32)gabi::load<u8>(tev_kcol2 + 3) / 255.0f;
    gabi::store<f32>(u + 4, gabi::load<f32>(c88.a + 4));
    gabi::store<f32>(u + 8, gabi::load<f32>(c88.a + 8));
    gabi::store<f32>(u, gabi::load<f32>(c88.a));
    gabi::store<f32>(u + 0xC, a);
    mDoExt_modelUpdateDL(i_this->mpWater0Model, 0);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D78)); /* dComIfGd_setList */
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D7C));
}

/* 0207B2CC daBgn_Draw. HD: room_disp only feeds the reflection packet (the room model itself is not drawn);
 * obj_disp also reflects the actors found by 0207B13C / 0207B1C8; Link's reflection packets (a list on the
 * player, +0x4B58) are entered here, leaving out his hands/rings/sword meshes when he is close (+0x3DC <= 10)
 * and his bow/hookshot while he is low and turned away; the ropes and the defeat rope are reflected too. */
BOOL daBgn_Draw(bgn_class* i_this) {
    WWHD_FUNC(0x0207B2CC, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    u32 link = gabi::load<u32>(dComIfGp_ea() + 0x5B34);
    bool far = gabi::load<f32>(link + 0x3DC) > 10.0f;
    s16 ca60 = i_this->mCA60;
    if (ca60 >= 1) {
        if (ca60 > 1) {
            bgn_setBlureRate((u8)ca60);
            bgn_onBlure();
        } else {
            i_this->mCA60 = 0;
            bgn_offBlure();
        }
    }
    bgn_water0_disp(i_this);
    if (l_HIO().m00D != 0) {
        /* room_disp (inline) */
        settingTevStruct(dKy_getEnvlight(), 2, gabi::at<cXyz>(0x10461CB8), (dKy_tevstr_c*)&i_this->mRoomTevStr);
        MtxTrans(0.0f, REG0_F(12), 0.0f, false);
        f32 x = gabi::fmadds(REG0_F(13), 0.01f, 1.0f);
        MtxScale(x, x, x, true);
        J3DModel_setBaseTRMtx(i_this->mpRoomReflectionModel, calc_mtx());
        setLightTevColorType(dKy_getEnvlight(), i_this->mpRoomReflectionModel, (dKy_tevstr_c*)&i_this->mRoomTevStr);
        i_this->mCB60.mpModel = i_this->mpRoomReflectionModel;
        mDoExt_J3DModelPacketS_update(&i_this->mCB60);
    }
    if (l_HIO().m00C != 0) {
        /* obj_disp (inline) */
        if (REG0_S(8) == 0)
            fpcM_Search(0x0207B040 /* ten_a_d_sub */, i_this);
        fpcM_Search(0x0207B0D8 /* ki_a_d_sub */, i_this);
        fpcM_Search(0x0207B13C, i_this);
        fpcM_Search(0x0207B1C8, i_this);
    }
    /* HD: Link's reflection packets */
    {
        u32 list = link + 0x4B58;
        u32 p = gabi::load<u32>(link + 0x4B60);
        u32 end = p + gabi::load<u32>(list) * 4;
        if (p != end) {
            gabi::Local<SafeString> a;
            gabi::Local<SafeString> b;
            for (; p != end; p += 4) {
                u32 pkt = gabi::load<u32>(p);
                if (gabi::load<u32>(pkt + 0x98) != 0) {
                    u32 res = gabi::load<u32>(gabi::load<u32>(pkt + 0x98) + 0x14);
                    s32 off = gabi::load<s32>(res + 4);
                    a->mStringTop = off != 0 ? res + 4 + off : 0;
                    a->__vtbl = BGN_SAFESTRING_VTBL;
                    bool first = true;
                    if (!far) {
                        if (bgn_name_is(a, b, 0x10008B60 /* "hands" */, true) || bgn_name_is(a, b, 0x10008B68 /* "pring" */, false) ||
                            bgn_name_is(a, b, 0x10008B78 /* "shms" */, false) || bgn_name_is(a, b, 0x10008B80 /* "swms" */, false) ||
                            bgn_name_is(a, b, 0x10008B88 /* "swgripms" */, false) || bgn_name_is(a, b, 0x10008B70 /* "podms" */, false))
                            continue;
                        first = false;
                    }
                    bool held = bgn_name_is(a, b, 0x10008B60 /* "hands" */, first) || bgn_name_is(a, b, 0x10008B68 /* "pring" */, false) ||
                                bgn_name_is(a, b, 0x10008B5C /* "bow" */, false) || bgn_name_is(a, b, 0x10008B94 /* "hookshot" */, false);
                    if (held) {
                        f32 y = gabi::load<f32>(link + 0x318);
                        s16 ang = gabi::load<s16>(link + 0x3D0);
                        if (y < 1.0f && ang > 0x1800)
                            continue;
                    }
                }
                bgn_update(gabi::load<u32>(p));
            }
        }
        gabi::store<u32>(list, 0);
    }
    if (i_this->m02B4 != 0xFF) {
        settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &actor->current.pos, bgn_tev(gabi::ea(actor) + 0x110));
        bgn2_g() = gabi::ea(fpcM_Search(0x0207B22C /* bgn2_s_sub */, i_this));
        u32 b3 = gabi::ea(fpcM_Search(0x0207B27C /* bgn3_s_sub */, i_this));
        bgn3_g() = b3;
        s8 phase = i_this->m02B5;
        if (phase == 0) {
            /* daBgn_DrawS (inline) */
            u32 tev = gabi::ea(actor) + 0x110;
            bgn_fog_dim(tev, i_this->mCC88, &i_this->mCC88);
            s16 timer = i_this->mArrowHitFlashTimer;
            if (timer != 0)
                bgn_fog_flash(tev, &i_this->m02F8, &i_this->m02FC, timer, 40, 7, 1250.0f);
            setLightTevColorType(dKy_getEnvlight(), i_this->mpChestModel, bgn_tev(tev));
            mDoExt_modelUpdateDL(i_this->mpChestModel, 0);
            dSnap_RegistFig(0xCD /* DSNAP_TYPE_BGN */, actor, 1.0f, 1.0f, 1.0f);
            if (l_HIO().m00C != 0) {
                i_this->m02C0.mpModel = i_this->mpChestModel;
                mDoExt_J3DModelPacketS_update(&i_this->m02C0);
            }
            part_draw(i_this, &i_this->mHeadParts[0]);
            part_draw(i_this, &i_this->mPelvisParts[0]);
            for (s32 i = 0; i < BGN_HAND_MAX(); i++) {
                part_draw(i_this, &i_this->mLeftArmParts[i]);
                part_draw(i_this, &i_this->mRightArmParts[i]);
            }
            for (s32 i = 0; i < 3; i++) {
                part_draw(i_this, &i_this->mLeftLegParts[i]);
                part_draw(i_this, &i_this->mRightLegParts[i]);
            }
            for (s32 i = 0; i < BGN_TAIL_MAX(); i++)
                part_draw(i_this, &i_this->mTailParts[i]);
            u32 blue = gabi::ea(&i_this->mBlueRopeMat);
            bgn_lineMat_update(blue, 0x10190F34, tev);
            bgn_set3DlineMat(blue);
            u32 red = gabi::ea(&i_this->mRedRopeMat);
            bgn_lineMat_update(red, 0x10190F34, tev);
            bgn_set3DlineMat(red);
            /* HD: the ropes are reflected */
            i_this->mHdPacket[0].mA4 = i_this->m3C8 + 0x110;
            i_this->mHdPacket[0].mA0 = blue;
            mDoExt_J3DModelPacketS_update(&i_this->mHdPacket[0]);
            i_this->mHdPacket[1].mA4 = i_this->m3C8 + 0x110;
            i_this->mHdPacket[1].mA0 = red;
            mDoExt_J3DModelPacketS_update(&i_this->mHdPacket[1]);
        } else if (phase == 1) {
            u32 b2 = bgn2_g();
            if (b2 != 0) {
                /* daBgn2_Draw (inline; bgn2_class HD offsets) */
                u32 tev = b2 + 0x110;
                settingTevStruct(dKy_getEnvlight(), 0, gabi::at<cXyz>(b2 + 0x314), bgn_tev(tev));
                f32 dim = gabi::load<f32>(b2 + 0x316C); /* m2E7C */
                s16 timer = gabi::load<s16>(b2 + 0x3058); /* mArrowHitFlashTimer */
                bgn_fog_dim(tev, dim, gabi::at<be<f32>>(b2 + 0x316C));
                if (timer != 0)
                    bgn_fog_flash(tev, gabi::at<be<s16>>(b2 + 0x3050), gabi::at<be<f32>>(b2 + 0x3054), timer, 20, 0xE, 2500.0f);
                u32 head = gabi::load<u32>(b2 + 0x3D0); /* mpHeadMorf */
                setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(gabi::load<u32>(head + 0x90)), bgn_tev(tev));
                u32 body = gabi::load<u32>(b2 + 0x484); /* mpBodyMorf */
                setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(gabi::load<u32>(body + 0x90)), bgn_tev(tev));
                bgn_morf_entryDL(gabi::load<u32>(b2 + 0x3D0));
                if (l_HIO().m00C != 0) {
                    gabi::store<u32>(b2 + 0x46C, gabi::load<u32>(gabi::load<u32>(b2 + 0x3D0) + 0x90));
                    bgn_update(b2 + 0x3D4); /* m02B8 */
                }
                bgn_morf_entryDL(gabi::load<u32>(b2 + 0x484));
                if (l_HIO().m00C != 0) {
                    gabi::store<u32>(b2 + 0x520, gabi::load<u32>(gabi::load<u32>(b2 + 0x484) + 0x90));
                    bgn_update(b2 + 0x488); /* m02D0 */
                }
                s8 health = gabi::load<s8>(b2 + 0x3A1);
                if (health != 0) {
                    u32 model;
                    if (health == 3) {
                        model = gabi::load<u32>(b2 + 0x5F0);
                    } else if (health == 1) {
                        model = gabi::load<u32>(b2 + 0x5E8);
                        u32 brk = gabi::load<u32>(b2 + 0x5F4);
                        mDoExt_brkAnm_entry(gabi::at<mDoExt_brkAnm>(brk), gabi::at<J3DModelData>(gabi::load<u32>(model + 0xAC)), gabi::load<f32>(brk + 4));
                    } else {
                        model = gabi::load<u32>(b2 + 0x5EC);
                        u32 brk = gabi::load<u32>(b2 + 0x5F8);
                        mDoExt_brkAnm_entry(gabi::at<mDoExt_brkAnm>(brk), gabi::at<J3DModelData>(gabi::load<u32>(model + 0xAC)), gabi::load<f32>(brk + 4));
                    }
                    setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(model), bgn_tev(tev));
                    mDoExt_modelUpdateDL(gabi::at<J3DModel>(model), 0);
                    if (l_HIO().m00C != 0) {
                        gabi::store<u32>(b2 + 0x5D0, model);
                        bgn_update(b2 + 0x538); /* m02E4 */
                    }
                }
                bgn_lineMat_update(b2 + 0x3178, 0x10190F38, tev);
                bgn_set3DlineMat(b2 + 0x3178);
                gabi::store<u32>(b2 + 0x33A8, tev); /* HD: the rope is reflected */
                gabi::store<u32>(b2 + 0x33A4, b2 + 0x3178);
                bgn_update(b2 + 0x3304);
            }
        } else if (phase == 2) {
            if (b3 != 0) {
                /* daBgn3_Draw (inline; bgn3_class HD offsets) */
                u32 tev = b3 + 0x110;
                settingTevStruct(dKy_getEnvlight(), 0, gabi::at<cXyz>(b3 + 0x314), bgn_tev(tev));
                be<f32>* dimp = gabi::at<be<f32>>(b3 + 0x1213C); /* m10060 */
                bgn_fog_dim(tev, *dimp, dimp);
                s16 timer = gabi::load<s16>(b3 + 0x11E78);
                if (timer != 0)
                    bgn_fog_flash(tev, gabi::at<be<s16>>(b3 + 0x11E72), gabi::at<be<f32>>(b3 + 0x11E74), timer, 40, 7, 1250.0f);
                u32 model = gabi::load<u32>(b3 + 0x484); /* m002CC */
                setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(model), bgn_tev(tev));
                mDoExt_modelUpdateDL(gabi::at<J3DModel>(model), 0);
                if (l_HIO().m00C != 0) {
                    gabi::store<u32>(b3 + 0x520, model);
                    bgn_update(b3 + 0x488);
                }
                u32 morf = gabi::load<u32>(b3 + 0x3D0);
                setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(gabi::load<u32>(morf + 0x90)), bgn_tev(tev));
                bgn_morf_entryDL(gabi::load<u32>(b3 + 0x3D0));
                if (l_HIO().m00C != 0) {
                    gabi::store<u32>(b3 + 0x46C, gabi::load<u32>(gabi::load<u32>(b3 + 0x3D0) + 0x90));
                    bgn_update(b3 + 0x3D4);
                }
                u32 part = b3 + 0x1BE8; /* mParts[9], 0x19D8 each */
                for (s32 i = 0; i < 9; i++, part += 0x19D8) {
                    settingTevStruct(dKy_getEnvlight(), 0, gabi::at<cXyz>(part + 0x288), bgn_tev(part + 0xB4));
                    s16 ptimer = gabi::load<s16>(part + 0x284);
                    bgn_fog_dim(part + 0xB4, *dimp, dimp);
                    if (ptimer != 0)
                        bgn_fog_flash(part + 0xB4, gabi::at<be<s16>>(part + 0x27C), gabi::at<be<f32>>(part + 0x280), ptimer, 40, 7, 1250.0f);
                    if (gabi::load<u32>(part) != 0) {
                        setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(gabi::load<u32>(part)), bgn_tev(part + 0xB4));
                        if (i == 8) {
                            s8 health = gabi::load<s8>(b3 + 0x3A1);
                            if (health == 1) {
                                u32 brk = gabi::load<u32>(b3 + 0x1BDC);
                                mDoExt_brkAnm_entry(gabi::at<mDoExt_brkAnm>(brk), gabi::at<J3DModelData>(gabi::load<u32>(gabi::load<u32>(b3 + 0x1BE8 + 8 * 0x19D8) + 0xAC)),
                                                    gabi::load<f32>(brk + 4));
                            } else if (health == 2) {
                                u32 brk = gabi::load<u32>(b3 + 0x1BE0);
                                mDoExt_brkAnm_entry(gabi::at<mDoExt_brkAnm>(brk), gabi::at<J3DModelData>(gabi::load<u32>(gabi::load<u32>(b3 + 0x1BE8 + 8 * 0x19D8) + 0xAC)),
                                                    gabi::load<f32>(brk + 4));
                            }
                        }
                        mDoExt_modelUpdateDL(gabi::at<J3DModel>(gabi::load<u32>(part)), 0);
                        if (l_HIO().m00C != 0) {
                            gabi::store<u32>(part + 0x9C, gabi::load<u32>(part));
                            bgn_update(part + 4);
                        }
                    }
                }
                bgn_lineMat_update(b3 + 0x12140, 0x10190F3C, tev);
                bgn_set3DlineMat(b3 + 0x12140);
                gabi::store<u32>(b3 + 0x12370, tev); /* HD: the rope is reflected */
                gabi::store<u32>(b3 + 0x1236C, b3 + 0x12140);
                bgn_update(b3 + 0x122CC);
            }
        }
        if (i_this->mC720 != 0) {
            u32 rope = gabi::ea(&i_this->mDefeatCSRopeMat);
            bgn_lineMat_update(rope, 0x10190F64, gabi::ea(actor) + 0x110);
            bgn_set3DlineMat(rope);
            i_this->mHdPacket[2].mA4 = i_this->m3C8 + 0x110; /* HD: reflected */
            i_this->mHdPacket[2].mA0 = rope;
            mDoExt_J3DModelPacketS_update(&i_this->mHdPacket[2]);
        }
    }
    /* HD: two more of Link's reflection packets (+0x4BE8, +0x52C8) while he is far enough away */
    if (far && link + 0x4BE8 != 0 && gabi::load<u8>(link + 0x4BE8 + 0xAC) != 0)
        bgn_update(link + 0x4BE8);
    water1_disp(i_this);
    if (far && link + 0x52C8 != 0 && gabi::load<u8>(link + 0x52C8 + 0xAC) != 0)
        bgn_update(link + 0x52C8);
    return TRUE;
}
VERIFY(0x0207B2CC, daBgn_Draw);
