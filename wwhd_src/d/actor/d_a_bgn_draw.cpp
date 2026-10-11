/**
 * d_a_bgn_draw.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 1): daBgn_Draw with the inlined water0_disp, room_disp, obj_disp,
 * daBgn_DrawS, daBgn2_Draw and daBgn3_Draw.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bgn.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bgn_draw.h"

// Private bindings for HD renderer operations.
/* mDoGph_gInf_c blur (HD): rate byte 0x101F4826, flag byte 0x101F4825; 025F064C onBlure */
static inline void bgn_setBlureRate(u8 rate) {
    bgn_draw::view<bgn_draw::Blur>(0x101F4825)->mRate = rate;
}
static inline void bgn_onBlure() { bgnDraw_onBlure(); }
static inline void bgn_offBlure() { bgn_draw::view<bgn_draw::Blur>(0x101F4825)->mEnabled = 0; }
/* 0274D458 (HD): colour vector (four floats) scaled by an intensity into a GX2 uniform vector */
static inline void bgn_color_scale(u32 dst, u32 src, f32 k) { bgnDraw_scaleColor(dst, src, k); }
/* 027F9F0C (HD J3D): the uniform slot `idx` of a material's register block (dirty flags at +0) */
static inline u32 bgn_uniform_slot(u32 flags, s32 idx) { return bgnDraw_uniformSlot(flags, idx); }
/* 025ED1BC mDoExt_3DlineMat1_c::update(u16 n, const GXColor& color, dKy_tevstr_c*) */
static inline void bgn_lineMat_update(u32 mat, u32 color, u32 tev) {
    bgnDraw_updateLine(mat, 60, color, tev);
}
/* dComIfGd_set3DlineMat (HD inline): play+0x5FB4 sort packets (0x9C each) indexed by the material
 * id (virtual slot 0x14 of the line material's second vtable at +0x130); 025EDD04 setMat */
static inline void bgn_set3DlineMat(u32 mat) {
    u32 sp = dComIfGp_ea() + 0x5FB4;
    s32 id = gabi::call_ptr<s32>(
        bgn_draw::view<bgn_draw::Vtable>(gabi::at<mDoExt_3DlineMat1_bgn>(mat)->__vtbl)
            ->mGetMaterialID,
        mat);
    bgnDraw_setLineMaterial(sp + id * 0x9C, mat);
}
/* 025E5590 mDoExt_McaMorf::entryDL */
static inline void bgn_morf_entryDL(u32 morf) { bgnDraw_entryDL(morf); }
static inline void bgn_update(u32 pkt) {
    mDoExt_J3DModelPacketS_update(gabi::at<mDoExt_J3DModelPacketS_l>(pkt));
}
static inline dKy_tevstr_c *bgn_tev(u32 a) { return gabi::at<dKy_tevstr_c>(a); }

/* the arrow-hit fog flash shared by the three bodies and the parts (GameCube: written out each
 * time) */
static inline void bgn_fog_dim(u32 tev, f32 dim, be<f32> *dimp) {
    auto *lighting = gabi::at<dKy_tevstr_c>(tev);
    if (dim > 0.0f) {
        lighting->mFogColor.b = 0;
        lighting->mFogColor.g = 0;
        lighting->mFogColor.r = 0;
        lighting->mFogStartZ = gabi::fmadds(-50000.0f, *dimp, (lighting->mFogStartZ));
    }
}
static inline void bgn_fog_flash(u32 tev, be<s16> *c, be<f32> *z, s16 timer, s16 thr, s16 step0,
                                 f32 z0step) {
    auto *lighting = gabi::at<dKy_tevstr_c>(tev);
    s16 cc = *c;
    s16 r = (s16)((lighting->mFogColor.r) + cc);
    s16 g = (lighting->mFogColor.g);
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
    f32 fz = (lighting->mFogStartZ);
    lighting->mFogColor.r = (s16)(r & 0xFF);
    lighting->mFogColor.g = (s16)(gg & 0xFF);
    lighting->mFogColor.b = (s16)(b & 0xFF);
    lighting->mFogStartZ = gabi::fadds_ppc(fz, *z);
}

/* SafeString::isEqual as GHS expands it (see the packet draw): `a` is made terminated (directly the
 * first time, then through the vtable), again through the vtable, then `b`; byte compare up to
 * 0x40001 */
static bool bgn_name_is(SafeString *a, SafeString *b, const char *lit, bool first) {
    b->mStringTop = gabi::guest_addr(lit);
    b->__vtbl = BGN_SAFESTRING_VTBL;
    if (first)
        SafeString_assureTermination(a);
    else
        gabi::call_ptr(bgn_draw::view<bgn_draw::StringVtable>(a->__vtbl)->mAssureTermination,
                       gabi::ea(a));
    gabi::call_ptr(bgn_draw::view<bgn_draw::StringVtable>(a->__vtbl)->mAssureTermination,
                   gabi::ea(a));
    u32 p = a->mStringTop;
    gabi::call_ptr(bgn_draw::view<bgn_draw::StringVtable>(b->__vtbl)->mAssureTermination,
                   gabi::ea(b));
    u32 q = b->mStringTop;
    if (p == q)
        return true;
    for (u32 n = 0x40001; n != 0; n--, p++, q++) {
        u8 c1 = *gabi::at<be<u8>>(p);
        u8 c2 = *gabi::at<be<u8>>(q);
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
    }
    return false;
}

// HD: only the opaque sky list is switched; the material's blend/z/colour state is
// set through the HD J3D API and the colours are pushed into the material's uniform block.
static inline void water0_disp(bgn_class *i_this) {
    settingTevStruct(dKy_getEnvlight(), 2 /* TEV_TYPE_BG1 */,
                     gabi::at<cXyz>(0x10461CB8) /* w_pos */, (dKy_tevstr_c *)&i_this->mWaterTevStr);
    MtxTrans(0.0f, REG0_F(11), 0.0f, false);
    J3DModel_setBaseTRMtx(i_this->mpWater0Model, calc_mtx());
    setLightTevColorType(dKy_getEnvlight(), i_this->mpWater0Model,
                         (dKy_tevstr_c *)&i_this->mWaterTevStr);
    bgn_draw::view<bgn_draw::DrawLists>(0x104B4634)->mpOpa =
        (bgn_draw::view<bgn_draw::PlayDraw>(dComIfGp_ea())->mpOpaListSky);
    u32 node = i_this->mpWater0Model->mModelData->mMaterials;
    u32 mat = (bgn_draw::view<bgn_draw::MaterialNode>(node)->mpMaterial);
    u32 tevobj = (bgn_draw::view<bgn_draw::MaterialNode>(node)->mpTevBlock);
    s32 off = (bgn_draw::view<bgn_draw::Material>(mat)->mPEBlockOffset);
    u32 pe = off != 0 ? mat + 0x20 + off : 0;
    u32 tev_col = gabi::call_ptr<u32>(
        bgn_draw::view<bgn_draw::Vtable>(bgn_draw::view<bgn_draw::TevBlock>(tevobj)->mpVtable)
            ->mGetTevColor,
        tevobj, 0);
    tevobj = (bgn_draw::view<bgn_draw::MaterialNode>(node)->mpTevBlock);
    u32 tev_kcol = gabi::call_ptr<u32>(
        bgn_draw::view<bgn_draw::Vtable>(bgn_draw::view<bgn_draw::TevBlock>(tevobj)->mpVtable)
            ->mGetTevKColor,
        tevobj, 0);
    tevobj = (bgn_draw::view<bgn_draw::MaterialNode>(node)->mpTevBlock);
    u32 tev_kcol2 = gabi::call_ptr<u32>(
        bgn_draw::view<bgn_draw::Vtable>(bgn_draw::view<bgn_draw::TevBlock>(tevobj)->mpVtable)
            ->mGetTevKColor,
        tevobj, 3);
    bgnDraw_setBlendType(pe, 3);       /* blend->setType(GX_BM_BLEND) */
    bgnDraw_setColorSrc(pe + 0x18, 4); /* source factors: GX_BL_SRC_ALPHA */
    bgnDraw_setAlphaSrc(pe + 0x18, 4);
    bgnDraw_setColorDst(pe + 0x18, 5); /* destination factors: GX_BL_INV_SRC_ALPHA */
    bgnDraw_setAlphaDst(pe + 0x18, 5);
    bgnDraw_setDepthEnable(pe + 8, 1);
    bgnDraw_setDepthFunction(pe + 8, 3);
    bgnDraw_setDepthUpdate(pe + 0x14, 1); /* zMode->setUpdateEnable(1) */
    gabi::at<GXColorS10>(tev_col)->b = 0;
    gabi::at<GXColorS10>(tev_col)->r = 0;
    gabi::at<GXColorS10>(tev_col)->g = 0;
    gabi::at<GXColor>(tev_kcol)->b = 0xFF;
    gabi::at<GXColor>(tev_kcol)->g = 0xFF;
    gabi::at<GXColor>(tev_kcol)->r = 0xFF;
    f32 fVar1 = l_HIO().m010;
    if (i_this->m02B5 == 1 && l_HIO().m00D == 0) {
        f32 y = gabi::at<fopAc_ac_c>(bgn2_g())->current.pos.y;
        fVar1 = gabi::fmuls_ppc(gabi::fadds_ppc(gabi::fadds_ppc(REG0_F(4), 4000.0f), y),
                                gabi::fadds_ppc(REG0_F(5), 0.01f));
        if (fVar1 > 90.0f)
            fVar1 = 90.0f;
        else if (fVar1 < 50.0f)
            fVar1 = 50.0f;
    }
    gabi::at<GXColor>(tev_kcol2)->a = (u8)gabi::ftoi(gabi::fmuls_ppc(fVar1, 2.559f));
    gabi::Local<be<f32>[4]> tevColor, scaledTevColor, konstColor, scaledKonstColor, fadeColor,
        scaledFadeColor;
    /* tev colour 0 -> uniform 4 */
    tevobj = (bgn_draw::view<bgn_draw::MaterialNode>(node)->mpTevBlock);
    gabi::call_ptr(
        bgn_draw::view<bgn_draw::Vtable>(bgn_draw::view<bgn_draw::TevBlock>(tevobj)->mpVtable)
            ->mSetTevColor,
        tevobj, 0, tev_col);
    bgn_colorS10_to_f(gabi::at<be<f32>>(tevColor.a), gabi::at<be<s16>>(tev_col));
    bgn_color_scale(scaledTevColor.a, tevColor.a, 1.0f);
    bgn_draw::view<bgn_draw::MaterialNode>(node)->mUniformFlags =
        (bgn_draw::view<bgn_draw::MaterialNode>(node)->mUniformFlags) | 0x10;
    u32 u = bgn_uniform_slot(node + 0xA0, 4);
    f32 a = (f32)(s32)(gabi::at<GXColorS10>(tev_col)->a) / 255.0f;
    gabi::at<bgn_draw::Uniform>(u)->r = (*scaledTevColor.get())[0];
    gabi::at<bgn_draw::Uniform>(u)->g = (*scaledTevColor.get())[1];
    gabi::at<bgn_draw::Uniform>(u)->b = (*scaledTevColor.get())[2];
    gabi::at<bgn_draw::Uniform>(u)->a = a;
    /* konst colour 0 -> uniform 7 */
    tevobj = (bgn_draw::view<bgn_draw::MaterialNode>(node)->mpTevBlock);
    gabi::call_ptr(
        bgn_draw::view<bgn_draw::Vtable>(bgn_draw::view<bgn_draw::TevBlock>(tevobj)->mpVtable)
            ->mSetTevKColor,
        tevobj, 0, tev_kcol);
    bgn_color_to_f(gabi::at<be<f32>>(konstColor.a), gabi::at<be<u8>>(tev_kcol));
    bgn_color_scale(scaledKonstColor.a, konstColor.a, 1.0f);
    bgn_draw::view<bgn_draw::MaterialNode>(node)->mUniformFlags =
        (bgn_draw::view<bgn_draw::MaterialNode>(node)->mUniformFlags) | 0x80;
    u = bgn_uniform_slot(node + 0xA0, 7);
    a = (f32)(u32)(gabi::at<GXColor>(tev_kcol)->a) / 255.0f;
    gabi::at<bgn_draw::Uniform>(u)->r = (*scaledKonstColor.get())[0];
    gabi::at<bgn_draw::Uniform>(u)->g = (*scaledKonstColor.get())[1];
    gabi::at<bgn_draw::Uniform>(u)->b = (*scaledKonstColor.get())[2];
    gabi::at<bgn_draw::Uniform>(u)->a = a;
    /* konst colour 3 -> uniform 10 */
    tevobj = (bgn_draw::view<bgn_draw::MaterialNode>(node)->mpTevBlock);
    gabi::call_ptr(
        bgn_draw::view<bgn_draw::Vtable>(bgn_draw::view<bgn_draw::TevBlock>(tevobj)->mpVtable)
            ->mSetTevKColor,
        tevobj, 3, tev_kcol2);
    bgn_color_to_f(gabi::at<be<f32>>(fadeColor.a), gabi::at<be<u8>>(tev_kcol2));
    bgn_color_scale(scaledFadeColor.a, fadeColor.a, 1.0f);
    bgn_draw::view<bgn_draw::MaterialNode>(node)->mUniformFlags =
        (bgn_draw::view<bgn_draw::MaterialNode>(node)->mUniformFlags) | 0x400;
    u = bgn_uniform_slot(node + 0xA0, 10);
    a = (f32)(u32)(gabi::at<GXColor>(tev_kcol2)->a) / 255.0f;
    gabi::at<bgn_draw::Uniform>(u)->g = (*scaledFadeColor.get())[1];
    gabi::at<bgn_draw::Uniform>(u)->b = (*scaledFadeColor.get())[2];
    gabi::at<bgn_draw::Uniform>(u)->r = (*scaledFadeColor.get())[0];
    gabi::at<bgn_draw::Uniform>(u)->a = a;
    mDoExt_modelUpdateDL(i_this->mpWater0Model, 0);
    bgn_draw::view<bgn_draw::DrawLists>(0x104B4634)->mpOpa =
        (bgn_draw::view<bgn_draw::PlayDraw>(dComIfGp_ea())->mpOpaList); /* dComIfGd_setList */
    bgn_draw::view<bgn_draw::DrawLists>(0x104B4634)->mpXlu =
        (bgn_draw::view<bgn_draw::PlayDraw>(dComIfGp_ea())->mpXluList);
}

/* daBgn_Draw. HD: room_disp only feeds the reflection packet (the room model itself is not
 * drawn); obj_disp also reflects the actors found by 0207B13C / 0207B1C8; Link's reflection packets
 * (a list on the player, +0x4B58) are entered here, leaving out his hands/rings/sword meshes when
 * he is close (+0x3DC <= 10) and his bow/hookshot while he is low and turned away; the ropes and
 * the defeat rope are reflected too. */
/* 0207B2CC */
BOOL daBgn_Draw(bgn_class *i_this) {

    fopAc_ac_c *actor = i_this;
    u32 link = (bgn_draw::view<bgn_draw::PlayDraw>(dComIfGp_ea())->mpLink.v);
    // HD: guest address zero must remain readable in the verifier; do not null-normalize these
    // views.
    auto *player = bgn_draw::view<bgn_draw::LinkReflection>(link);
    bool far = (player->mCameraDistance) > 10.0f;
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
    water0_disp(i_this);
    if (l_HIO().m00D != 0) {
        /* room_disp (inline) */
        settingTevStruct(dKy_getEnvlight(), 2, gabi::at<cXyz>(0x10461CB8),
                         (dKy_tevstr_c *)&i_this->mRoomTevStr);
        MtxTrans(0.0f, REG0_F(12), 0.0f, false);
        f32 x = gabi::fmadds(REG0_F(13), 0.01f, 1.0f);
        MtxScale(x, x, x, true);
        J3DModel_setBaseTRMtx(i_this->mpRoomReflectionModel, calc_mtx());
        setLightTevColorType(dKy_getEnvlight(), i_this->mpRoomReflectionModel,
                             (dKy_tevstr_c *)&i_this->mRoomTevStr);
        i_this->mCB60.mpModel = i_this->mpRoomReflectionModel;
        mDoExt_J3DModelPacketS_update(&i_this->mCB60);
    }
    if (l_HIO().m00C != 0) {
        /* obj_disp (inline) */
        if (REG0_S(8) == 0)
            fpcM_Search(BGN_DRAW_ten_a_d_sub, i_this);
        fpcM_Search(BGN_DRAW_ki_a_d_sub, i_this);
        fpcM_Search(BGN_DRAW_ks_a_d_sub, i_this);
        fpcM_Search(BGN_DRAW_himo3_a_d_sub, i_this);
    }
    // HD: Link's reflection packets.
    {
        u32 p = (player->mpPackets.v);
        u32 end = p + (player->mPacketCount) * 4;
        if (p != end) {
            gabi::Local<SafeString> a;
            gabi::Local<SafeString> b;
            for (; p != end; p += 4) {
                u32 pkt = gabi::at<gptr<mDoExt_J3DModelPacketS_l>>(p)->v;
                if ((gabi::at<mDoExt_J3DModelPacketS_l>(pkt)->mpModel.v) != 0) {
                    u32 res = bgn_draw::view<bgn_draw::ModelResource>(
                                  gabi::at<mDoExt_J3DModelPacketS_l>(pkt)->mpModel.v)
                                  ->mpResource;
                    s32 off = (bgn_draw::view<bgn_draw::Resource>(res)->mNameOffset);
                    a->mStringTop = off != 0 ? res + 4 + off : 0;
                    a->__vtbl = BGN_SAFESTRING_VTBL;
                    bool first = true;
                    if (!far) {
                        if (bgn_name_is(a, b, "hands", true) || bgn_name_is(a, b, "pring", false) ||
                            bgn_name_is(a, b, "shms", false) || bgn_name_is(a, b, "swms", false) ||
                            bgn_name_is(a, b, "swgripms", false) ||
                            bgn_name_is(a, b, "podms", false))
                            continue;
                        first = false;
                    }
                    bool held =
                        bgn_name_is(a, b, "hands", first) || bgn_name_is(a, b, "pring", false) ||
                        bgn_name_is(a, b, "bow", false) || bgn_name_is(a, b, "hookshot", false);
                    if (held) {
                        f32 y = (player->mVerticalSpeed);
                        s16 ang = (player->mViewAngle);
                        if (y < 1.0f && ang > 0x1800)
                            continue;
                    }
                }
                bgn_update(gabi::at<gptr<mDoExt_J3DModelPacketS_l>>(p)->v);
            }
        }
        player->mPacketCount = 0;
    }
    if (i_this->m02B4 != 0xFF) {
        settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &actor->current.pos,
                         bgn_tev(gabi::ea(actor) + 0x110));
        bgn2_g() = gabi::ea(fpcM_Search(BGN_DRAW_bgn2_s_sub, i_this));
        u32 b3 = gabi::ea(fpcM_Search(BGN_DRAW_bgn3_s_sub, i_this));
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
            // HD: the ropes are reflected.
            i_this->mHdPacket[0].mA4 = i_this->m3C8 + 0x110;
            i_this->mHdPacket[0].mA0 = blue;
            mDoExt_J3DModelPacketS_update(&i_this->mHdPacket[0]);
            i_this->mHdPacket[1].mA4 = i_this->m3C8 + 0x110;
            i_this->mHdPacket[1].mA0 = red;
            mDoExt_J3DModelPacketS_update(&i_this->mHdPacket[1]);
        } else if (phase == 1) {
            u32 b2 = bgn2_g();
            auto *body = bgn_draw::view<bgn_draw::bgn2_class>(b2);
            if (b2 != 0) {
                /* daBgn2_Draw (inline; bgn2_class HD offsets) */
                u32 tev = b2 + 0x110;
                settingTevStruct(dKy_getEnvlight(), 0, gabi::at<cXyz>(b2 + 0x314), bgn_tev(tev));
                f32 dim = (body->m2E7C);                 /* m2E7C */
                s16 timer = (body->mArrowHitFlashTimer); /* mArrowHitFlashTimer */
                bgn_fog_dim(tev, dim, &body->m2E7C);
                if (timer != 0)
                    bgn_fog_flash(tev, &body->m2D60, &body->m2D64, timer, 20, 0xE, 2500.0f);
                u32 head = (body->mpHeadMorf.v); /* mpHeadMorf */
                setLightTevColorType(
                    dKy_getEnvlight(),
                    gabi::at<J3DModel>((gabi::at<mDoExt_McaMorf>(head)->mpModel.v)), bgn_tev(tev));
                u32 bodyMorf = (body->mpBodyMorf.v); /* mpBodyMorf */
                setLightTevColorType(
                    dKy_getEnvlight(),
                    gabi::at<J3DModel>((gabi::at<mDoExt_McaMorf>(bodyMorf)->mpModel.v)),
                    bgn_tev(tev));
                bgn_morf_entryDL((body->mpHeadMorf.v));
                if (l_HIO().m00C != 0) {
                    body->m02B8.mpModel.v = gabi::at<mDoExt_McaMorf>(body->mpHeadMorf.v)->mpModel.v;
                    bgn_update(b2 + 0x3D4); /* m02B8 */
                }
                bgn_morf_entryDL((body->mpBodyMorf.v));
                if (l_HIO().m00C != 0) {
                    body->m02D0.mpModel.v = gabi::at<mDoExt_McaMorf>(body->mpBodyMorf.v)->mpModel.v;
                    bgn_update(b2 + 0x488); /* m02D0 */
                }
                s8 health = (body->health);
                if (health != 0) {
                    u32 model;
                    if (health == 3) {
                        model = (body->mpJyakutenModel[2].v);
                    } else if (health == 1) {
                        model = (body->mpJyakutenModel[0].v);
                        u32 brk = (body->mJyakutenCBrkAnm.v);
                        mDoExt_brkAnm_entry(
                            gabi::at<mDoExt_brkAnm>(brk),
                            gabi::at<J3DModelData>((gabi::at<J3DModel>(model)->mModelData.v)),
                            gabi::at<mDoExt_brkAnm>(brk)->mFrameCtrl.mFrame);
                    } else {
                        model = (body->mpJyakutenModel[1].v);
                        u32 brk = (body->mJyakutenBBrkAnm.v);
                        mDoExt_brkAnm_entry(
                            gabi::at<mDoExt_brkAnm>(brk),
                            gabi::at<J3DModelData>((gabi::at<J3DModel>(model)->mModelData.v)),
                            gabi::at<mDoExt_brkAnm>(brk)->mFrameCtrl.mFrame);
                    }
                    setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(model),
                                         bgn_tev(tev));
                    mDoExt_modelUpdateDL(gabi::at<J3DModel>(model), 0);
                    if (l_HIO().m00C != 0) {
                        body->m02E4.mpModel.v = model;
                        bgn_update(b2 + 0x538); /* m02E4 */
                    }
                }
                bgn_lineMat_update(b2 + 0x3178, 0x10190F38, tev);
                bgn_set3DlineMat(b2 + 0x3178);
                body->mHdPacket.mA4 = tev; // HD: the rope is reflected.
                body->mHdPacket.mA0 = b2 + 0x3178;
                bgn_update(b2 + 0x3304);
            }
        } else if (phase == 2) {
            auto *body = bgn_draw::view<bgn_draw::bgn3_class>(b3);
            if (b3 != 0) {
                /* daBgn3_Draw (inline; bgn3_class HD offsets) */
                u32 tev = b3 + 0x110;
                settingTevStruct(dKy_getEnvlight(), 0, gabi::at<cXyz>(b3 + 0x314), bgn_tev(tev));
                be<f32> *dimp = &body->m10060; /* m10060 */
                bgn_fog_dim(tev, *dimp, dimp);
                s16 timer = (body->mArrowHitFlashTimer);
                if (timer != 0)
                    bgn_fog_flash(tev, &body->m0FD96, &body->m0FD98, timer, 40, 7, 1250.0f);
                u32 model = (body->m002CC.v); /* m002CC */
                setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(model), bgn_tev(tev));
                mDoExt_modelUpdateDL(gabi::at<J3DModel>(model), 0);
                if (l_HIO().m00C != 0) {
                    body->m002D0.mpModel.v = model;
                    bgn_update(b3 + 0x488);
                }
                u32 morf = (body->mpMorf.v);
                setLightTevColorType(
                    dKy_getEnvlight(),
                    gabi::at<J3DModel>((gabi::at<mDoExt_McaMorf>(morf)->mpModel.v)), bgn_tev(tev));
                bgn_morf_entryDL((body->mpMorf.v));
                if (l_HIO().m00C != 0) {
                    body->m002B8.mpModel.v = gabi::at<mDoExt_McaMorf>(body->mpMorf.v)->mpModel.v;
                    bgn_update(b3 + 0x3D4);
                }
                u32 part = b3 + 0x1BE8; /* mParts[9], 0x19D8 each */
                for (s32 i = 0; i < 9; i++, part += 0x19D8) {
                    auto *hairPart = bgn_draw::view<bgn_draw::part_s3>(part);
                    settingTevStruct(dKy_getEnvlight(), 0, &hairPart->m00D4, bgn_tev(part + 0xB4));
                    s16 ptimer = (hairPart->mPart3ArrowHitFlashTimer);
                    bgn_fog_dim(part + 0xB4, *dimp, dimp);
                    if (ptimer != 0)
                        bgn_fog_flash(part + 0xB4, &hairPart->m00C8, &hairPart->m00CC, ptimer, 40,
                                      7, 1250.0f);
                    if ((hairPart->mpPart3Model.v) != 0) {
                        setLightTevColorType(dKy_getEnvlight(),
                                             gabi::at<J3DModel>((hairPart->mpPart3Model.v)),
                                             bgn_tev(part + 0xB4));
                        if (i == 8) {
                            s8 health = (body->health);
                            if (health == 1) {
                                u32 brk = (body->mJyakutenCBrkAnm.v);
                                mDoExt_brkAnm_entry(
                                    gabi::at<mDoExt_brkAnm>(brk),
                                    gabi::at<J3DModelData>(
                                        gabi::at<J3DModel>(body->mParts[8].mpPart3Model.v)
                                            ->mModelData.v),
                                    gabi::at<mDoExt_brkAnm>(brk)->mFrameCtrl.mFrame);
                            } else if (health == 2) {
                                u32 brk = (body->mJyakutenBBrkAnm.v);
                                mDoExt_brkAnm_entry(
                                    gabi::at<mDoExt_brkAnm>(brk),
                                    gabi::at<J3DModelData>(
                                        gabi::at<J3DModel>(body->mParts[8].mpPart3Model.v)
                                            ->mModelData.v),
                                    gabi::at<mDoExt_brkAnm>(brk)->mFrameCtrl.mFrame);
                            }
                        }
                        mDoExt_modelUpdateDL(gabi::at<J3DModel>((hairPart->mpPart3Model.v)), 0);
                        if (l_HIO().m00C != 0) {
                            hairPart->m0004.mpModel.v = (hairPart->mpPart3Model.v);
                            bgn_update(part + 4);
                        }
                    }
                }
                bgn_lineMat_update(b3 + 0x12140, 0x10190F3C, tev);
                bgn_set3DlineMat(b3 + 0x12140);
                body->mHdPacket.mA4 = tev; // HD: the rope is reflected.
                body->mHdPacket.mA0 = b3 + 0x12140;
                bgn_update(b3 + 0x122CC);
            }
        }
        if (i_this->mC720 != 0) {
            u32 rope = gabi::ea(&i_this->mDefeatCSRopeMat);
            bgn_lineMat_update(rope, 0x10190F64, gabi::ea(actor) + 0x110);
            bgn_set3DlineMat(rope);
            i_this->mHdPacket[2].mA4 = i_this->m3C8 + 0x110; // HD: reflected.
            i_this->mHdPacket[2].mA0 = rope;
            mDoExt_J3DModelPacketS_update(&i_this->mHdPacket[2]);
        }
    }
    // HD: two more of Link's reflection packets while he is far enough away.
    if (far && link + 0x4BE8 != 0 && player->mNearPacket.mAC != 0)
        bgn_update(link + 0x4BE8);
    water1_disp(i_this);
    if (far && link + 0x52C8 != 0 && player->mFarPacket.mAC != 0)
        bgn_update(link + 0x52C8);
    return TRUE;
}

// clang-format off
/* WWHD glue
str 10008B60
str 10008B68
str 10008B78
str 10008B80
str 10008B88
str 10008B70
str 10008B5C
str 10008B94
ext 025F064C void bgnDraw_onBlure()
ext 0274D458 void bgnDraw_scaleColor(u32 dst, u32 src, f32 intensity)
ext 027F9F0C u32 bgnDraw_uniformSlot(u32 flags, s32 index)
ext 025ED1BC void bgnDraw_updateLine(u32 material, s32 count, u32 color, u32 tev)
ext 025EDD04 void bgnDraw_setLineMaterial(u32 packet, u32 material)
ext 025E5590 void bgnDraw_entryDL(u32 morf)
ext 027E212C void bgnDraw_setBlendType(u32 blend, s32 value)
ext 027E19B4 void bgnDraw_setColorSrc(u32 blend, s32 value)
ext 027E19E4 void bgnDraw_setAlphaSrc(u32 blend, s32 value)
ext 027E19C4 void bgnDraw_setColorDst(u32 blend, s32 value)
ext 027E19F4 void bgnDraw_setAlphaDst(u32 blend, s32 value)
ext 027E18F8 void bgnDraw_setDepthEnable(u32 mode, s32 value)
ext 027E1908 void bgnDraw_setDepthFunction(u32 mode, s32 value)
ext 027E195C void bgnDraw_setDepthUpdate(u32 mode, s32 value)
*/
