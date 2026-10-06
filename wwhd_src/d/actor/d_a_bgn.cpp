/**
 * d_a_bgn.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 1) / G (Puppet)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bgn.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source. The translation unit also
 * defines mDoExt_J3DModelPacketS (the reflection packets other actors use as well).
 */
#include "d/actor/d_a_bgn.h"

/* ---- mDoExt_J3DModelPacketS (HD) and small helpers ---- */

/* 0207A9C0 dPa_smokeEcallBack array element constructor: dPa_smokeEcallBack(this, 1) (HD) */
static void* smokeEcallBack_ct(void* p) {
    WWHD_FUNC(0x0207A9C0, void*, p);
    return gabi::call<void*>(0x025A5B18, p, 1);
}
VERIFY(0x0207A9C0, smokeEcallBack_ct);

/* 0207A9C8 matrix copy (HD inline helper): the twelve values through FPRs, then word copies */
void bgn_mtx_copy(Mtx34* dst, Mtx34* src) {
    WWHD_FUNC(0x0207A9C8, void, dst, src);
    f32 t[12];
    t[5] = src->m[1][1];
    t[4] = src->m[1][0];
    t[6] = src->m[1][2];
    t[7] = src->m[1][3];
    t[8] = src->m[2][0];
    t[9] = src->m[2][1];
    t[11] = src->m[2][3];
    t[0] = src->m[0][0];
    t[3] = src->m[0][3];
    t[1] = src->m[0][1];
    t[10] = src->m[2][2];
    t[2] = src->m[0][2];
    for (int i = 0; i < 12; i++) gabi::store<f32>(gabi::ea(dst) + 4 * i, t[i]);
}
VERIFY(0x0207A9C8, bgn_mtx_copy);

/* 0207AA68 HD: GXColorS10 -> four floats (component / 255) */
void bgn_colorS10_to_f(be<f32>* dst, be<s16>* src) {
    WWHD_FUNC(0x0207AA68, void, dst, src);
    f32 r = (f32)(s32)src[0] / 255.0f;
    f32 g = (f32)(s32)src[1] / 255.0f;
    f32 b = (f32)(s32)src[2] / 255.0f;
    f32 a = (f32)(s32)src[3] / 255.0f;
    dst[0] = r;
    dst[1] = g;
    dst[2] = b;
    dst[3] = a;
}
VERIFY(0x0207AA68, bgn_colorS10_to_f);

/* 0207AB2C mDoExt_J3DModelPacketS::update (HD): the mirrored view matrix is set up here and the
 * packet is entered into the sky opa list (GameCube: only entryImm) */
void mDoExt_J3DModelPacketS_update(mDoExt_J3DModelPacketS_l* p) {
    WWHD_FUNC(0x0207AB2C, void, p);
    if (p->mpHelper == 0)
        return;
    if (gabi::ea(p->mpModel.get()) == 0 && p->m9C == 0 && p->mA0 == 0)
        return;
    gabi::Local<Mtx34> view;
    gabi::Local<Mtx34> m;
    gabi::Local<f32[4]> col;
    PSMTXCopy(j3dSys_viewMtx(), view);
    f32 y = gabi::fadds_ppc(gabi::load<f32>(0x1019100C), REG0_F(15));
    PSMTXTrans(mDoMtx_stack_c::get(), REG0_F(14), y, REG0_F(16));
    mDoMtx_stack_c::scaleM(1.0f, -1.0f, 1.0f);
    PSMTXConcat(j3dSys_viewMtx(), mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
    bgn_mtx_copy(m, mDoMtx_stack_c::get());
    if (gabi::ea(p->mpModel.get()) != 0) {
        gabi::call(0x027F53CC, p->mpModel.get(), p->mpHelper.get(), 1, mDoMtx_stack_c::get());
        J3DDrawBuffer_entryImm(dComIfGd_getOpaListSky(), p, 0);
        return;
    }
    u32 tex = gabi::load<u32>(0x104B4708) + 0x240;
    gabi::call(0x027FDA54, p->mpHelper.get(), 0, m.get(), 0x104B470Cu, tex);
    if (p->mA4 != 0) {
        u32 h = gabi::load<u32>(p->mpHelper + 4);
        bgn_colorS10_to_f(gabi::at<be<f32>>(col.a), gabi::at<be<s16>>(p->mA4 + 0x90));
        gabi::call(0x0274D458, h + 0x1C4, col.a, gabi::load<f32>(p->mA4 + 0x28));
        bgn_colorS10_to_f(gabi::at<be<f32>>(col.a), gabi::at<be<s16>>(p->mA4 + 0x160));
        gabi::call(0x0274D458, h + 0x1D4, col.a, gabi::load<f32>(p->mA4 + 0x16C));
    }
    gabi::call(0x027FDFF4, p->mpHelper.get(), 0);
    J3DDrawBuffer_entryImm(dComIfGd_getOpaListSky(), p, 0);
}
VERIFY(0x0207AB2C, mDoExt_J3DModelPacketS_update);

/* 0207ACD0 */
void part_draw(bgn_class* i_this, part_s* param_2) {
    WWHD_FUNC(0x0207ACD0, void, i_this, param_2);
    fopAc_ac_c* actor = i_this;
    J3DModel* model = param_2->mpPartModel;
    if (model == nullptr)
        return;
    dKy_tevstr_l* tev = &param_2->mPartTevStr;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &param_2->m0D4, (dKy_tevstr_c*)tev);
    if (i_this->mCC88 > 0.0f) {
        f32 z = tev->mFogStartZ;
        tev->mFogColorR = 0;
        tev->mFogColorG = 0;
        tev->mFogColorB = 0;
        tev->mFogStartZ = gabi::fmadds(-50000.0f, i_this->mCC88, z);
    }
    s16 timer = param_2->mPartArrowHitFlashTimer;
    if (timer != 0) {
        s16 c8 = param_2->m0C8;
        s16 uVar4 = (s16)(tev->mFogColorR + c8);
        if (uVar4 > 0xFF)
            uVar4 = 0xFF;
        s16 g = tev->mFogColorG;
        s16 uVar5 = (s16)(g + c8);
        if (uVar5 > 0xFF)
            uVar5 = 0xFF;
        s16 uVar6 = (s16)(g + c8 / 2);
        if (uVar6 > 0xFF)
            uVar6 = 0xFF;
        if (timer > 40) {
            cLib_addCalcAngleS2(&param_2->m0C8, 0x118, 1, 30);
            cLib_addCalc2(&param_2->m0CC, -50000.0f, 1.0f, 5000.0f);
        } else {
            cLib_addCalcAngleS2(&param_2->m0C8, 0, 1, 7);
            cLib_addCalc0(&param_2->m0CC, 1.0f, 1250.0f);
        }
        f32 z = tev->mFogStartZ;
        f32 cc = param_2->m0CC;
        tev->mFogColorR = (s16)(uVar4 & 0xFF);
        tev->mFogColorG = (s16)(uVar5 & 0xFF);
        tev->mFogColorB = (s16)(uVar6 & 0xFF);
        tev->mFogStartZ = z + cc;
    }
    gabi::Local<cXyz> local_38;
    u32 camera = gabi::load<u32>(dComIfGp_ea() + 0x5AF8);
    cXyz_mi(&param_2->m224, local_38, gabi::at<cXyz>(camera + 0xDC));
    f32 dist = std_sqrtf(PSVECSquareMag(local_38));
    if (dist > l_HIO().m008 * param_2->m0F4) {
        setLightTevColorType(dKy_getEnvlight(), model, (dKy_tevstr_c*)tev);
        s8 health = actor->health;
        if (health <= 2 && gabi::ea(param_2) == gabi::ea(i_this) + 0xD970 - 0x3F0 + (u32)BGN_TAIL_MAX() * 0x3F0) {
            J3DModelData* md = J3DModel_getModelData(model);
            if (health == 1) {
                mDoExt_brkAnm_entry(i_this->mJyakutenCBrkAnm, md, gabi::load<f32>(gabi::ea(i_this->mJyakutenCBrkAnm.get()) + 4));
            } else {
                mDoExt_brkAnm_entry(i_this->mJyakutenBBrkAnm, md, gabi::load<f32>(gabi::ea(i_this->mJyakutenBBrkAnm.get()) + 4));
            }
        }
        if (param_2 == &i_this->mHeadParts[0]) {
            i_this->mpMorf->entryDL();
        } else {
            mDoExt_modelUpdateDL(model);
        }
    }
    if (l_HIO().m00C != 0) {
        param_2->m004.setModel(model);
        mDoExt_J3DModelPacketS_update(&param_2->m004);
    }
}
VERIFY(0x0207ACD0, part_draw);

/* 0207AF8C HD: GXColor -> four floats (component / 255) */
void bgn_color_to_f(be<f32>* dst, be<u8>* src) {
    WWHD_FUNC(0x0207AF8C, void, dst, src);
    f32 r = (f32)(u32)src[0] / 255.0f;
    f32 g = (f32)(u32)src[1] / 255.0f;
    f32 b = (f32)(u32)src[2] / 255.0f;
    f32 a = (f32)(u32)src[3] / 255.0f;
    dst[0] = r;
    dst[1] = g;
    dst[2] = b;
    dst[3] = a;
}
VERIFY(0x0207AF8C, bgn_color_to_f);

/* 0207B040 */
static void* ten_a_d_sub(void* param_1, void*) {
    WWHD_FUNC(0x0207B040, void*, param_1, (u32)0);
    fopAc_ac_c* actor = (fopAc_ac_c*)param_1;
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0x93 /* fpcNm_Obj_Vteng_e */) {
        u32 model = actor->model;
        if (model != 0 && bgn_g() != 0) {
            gabi::at<bgn_class>(bgn_g())->mCC24.mpModel = gabi::at<J3DModel>(model);
            mDoExt_J3DModelPacketS_update(&gabi::at<bgn_class>(bgn_g())->mCC24);
        }
        return param_1;
    } else {
        return nullptr;
    }
}
VERIFY(0x0207B040, ten_a_d_sub);

/* 0207B0D8 */
static void* ki_a_d_sub(void* param_1, void*) {
    WWHD_FUNC(0x0207B0D8, void*, param_1, (u32)0);
    fopAc_ac_c* keese = (fopAc_ac_c*)param_1;
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xD7 /* fpcNm_KI_e */) {
        u32 model = keese->model;
        if (model != 0) {
            mDoExt_J3DModelPacketS_l* pkt = gabi::at<mDoExt_J3DModelPacketS_l>(gabi::ea(keese) + 0x3D4); /* ki_class::m2B8 */
            pkt->setModel(gabi::at<J3DModel>(model));
            mDoExt_J3DModelPacketS_update(pkt);
        }
    }
    return nullptr;
}
VERIFY(0x0207B0D8, ki_a_d_sub);

/* 0207B13C HD: Morth (fpcNm_KS_e) reflections: its two morf models */
static void* ks_a_d_sub(void* param_1, void*) {
    WWHD_FUNC(0x0207B13C, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xCD /* fpcNm_KS_e */) {
        u32 a = gabi::ea(param_1);
        u32 m0 = gabi::load<u32>(gabi::load<u32>(a + 0x3D0) + 0x90);
        u32 m1 = gabi::load<u32>(gabi::load<u32>(a + 0x3D4) + 0x90);
        if (m0 != 0 && m1 != 0) {
            mDoExt_J3DModelPacketS_l* p0 = gabi::at<mDoExt_J3DModelPacketS_l>(a + 0x3E4);
            mDoExt_J3DModelPacketS_l* p1 = gabi::at<mDoExt_J3DModelPacketS_l>(a + 0x494);
            p0->setModel(gabi::at<J3DModel>(m0));
            mDoExt_J3DModelPacketS_update(p0);
            p1->setModel(gabi::at<J3DModel>(m1));
            mDoExt_J3DModelPacketS_update(p1);
        }
    }
    return nullptr;
}
VERIFY(0x0207B13C, ks_a_d_sub);

/* 0207B1C8 HD: the hanging rope (fpcNm_HIMO3_e) reflection: its line material and tevStr */
static void* himo3_a_d_sub(void* param_1, void*) {
    WWHD_FUNC(0x0207B1C8, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0x1BF /* fpcNm_HIMO3_e */) {
        u32 a = gabi::ea(param_1);
        mDoExt_J3DModelPacketS_l* p = gabi::at<mDoExt_J3DModelPacketS_l>(a + 0x182C);
        p->mA0 = a + 0x169C;
        p->mA4 = a + 0x110;
        mDoExt_J3DModelPacketS_update(p);
    }
    return nullptr;
}
VERIFY(0x0207B1C8, himo3_a_d_sub);

/* 0207B22C */
static void* bgn2_s_sub(void* param_1, void*) {
    WWHD_FUNC(0x0207B22C, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xF4 /* fpcNm_BGN2_e */) {
        return param_1;
    } else {
        return nullptr;
    }
}
VERIFY(0x0207B22C, bgn2_s_sub);

/* 0207B27C */
static void* bgn3_s_sub(void* param_1, void*) {
    WWHD_FUNC(0x0207B27C, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xF5 /* fpcNm_BGN3_e */) {
        return param_1;
    } else {
        return nullptr;
    }
}
VERIFY(0x0207B27C, bgn3_s_sub);

/* 0207CF04 */
static void* ki_del_sub(void* param_1, void*) {
    WWHD_FUNC(0x0207CF04, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xD7 /* fpcNm_KI_e */) {
        fopAcM_delete((fopAc_ac_c*)param_1);
    }
    return nullptr;
}
VERIFY(0x0207CF04, ki_del_sub);

/* 0207CF58 */
static void* ks_del_sub(void* param_1, void*) {
    WWHD_FUNC(0x0207CF58, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xCD /* fpcNm_KS_e */) {
        fopAcM_delete((fopAc_ac_c*)param_1);
    }
    return nullptr;
}
VERIFY(0x0207CF58, ks_del_sub);

/* 0207CFAC */
static void* ki_c_sub(void* param_1, void*) {
    WWHD_FUNC(0x0207CFAC, void*, param_1, (u32)0);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xD7 /* fpcNm_KI_e */) {
        ki_all_count() = ki_all_count() + 1;
    }
    return nullptr;
}
VERIFY(0x0207CFAC, ki_c_sub);

/* 0207D008 */
s32 ki_check(bgn_class* i_this) {
    WWHD_FUNC(0x0207D008, s32, i_this);
    ki_all_count() = 0;
    fpcM_Search(0x0207CFAC /* ki_c_sub */, i_this);
    return ki_all_count();
}
VERIFY(0x0207D008, ki_check);

/* 0207FC5C */
static BOOL daBgn_IsDelete(bgn_class*) {
    WWHD_FUNC(0x0207FC5C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0207FC5C, daBgn_IsDelete);

/* 0207FC64 */
static BOOL daBgn_Delete(bgn_class* i_this) {
    WWHD_FUNC(0x0207FC64, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x10008C48) /* "Bgn" */);
    smoke_remove(i_this->mPunchSmokeCb[0]);
    smoke_remove(i_this->mPunchSmokeCb[1]);
    if (i_this->mCC91 != 0) {
        s8 no = l_HIO().mNo;
        hio_set() = 0;
        mDoHIO_deleteChild(no);
    }
    mDoAud_seDeleteObject(&i_this->mC77C);
    mDoAud_seDeleteObject(&i_this->mC788);
    mDoAud_seDeleteObject(&i_this->mCA54);
    mDoAud_seDeleteObject(&i_this->m0308);
    return TRUE;
}
VERIFY(0x0207FC64, daBgn_Delete);

/* 0207FD38 mDoExt_J3DModelPacketS::setup (HD): creates the 0xC-byte draw helper on first use */
void mDoExt_J3DModelPacketS_setup(mDoExt_J3DModelPacketS_l* p, u32 heap) {
    WWHD_FUNC(0x0207FD38, void, p, heap);
    if (p->mpHelper == 0) {
        u32 h = gabi::ea(gabi::call<void*>(0x0273B050, 0xC, heap, 4)); /* operator new(size, heap, align) */
        if (h != 0)
            h = gabi::ea(gabi::call<void*>(0x027FD6F4, h));
        p->mpHelper = h;
        gabi::call(0x027FD838, h, 1, heap);
    }
}
VERIFY(0x0207FD38, mDoExt_J3DModelPacketS_setup);

/* 0207FDA4 HD: the part's reflection packet is set up as well */
s32 part_init(part_s* param_1, J3DModelData* param_2) {
    WWHD_FUNC(0x0207FDA4, s32, param_1, param_2);
    J3DModel* model = mDoExt_J3DModel__create(param_2, 0, 0x11020203);
    param_1->mpPartModel = model;
    if (model == nullptr)
        return FALSE;
    mDoExt_J3DModelPacketS_setup(&param_1->m004, 0);
    return TRUE;
}
VERIFY(0x0207FDA4, part_init);

/* 02080404 mDoExt_J3DModelPacketS::mDoExt_J3DModelPacketS (HD; allocates when this == NULL) */
mDoExt_J3DModelPacketS_l* mDoExt_J3DModelPacketS_ct(mDoExt_J3DModelPacketS_l* p) {
    WWHD_FUNC(0x02080404, mDoExt_J3DModelPacketS_l*, p);
    if (p == nullptr) {
        p = (mDoExt_J3DModelPacketS_l*)operator_new(0xB0);
        if (p == nullptr)
            return p;
    }
    gabi::call(0x027F1278, p); /* J3DPacket::J3DPacket */
    u32 name = p->m90;
    p->mA4 = 0;
    p->mAC = 1;
    p->mpModel = nullptr;
    p->m9C = 0;
    p->__vtbl = 0x10008DA8;
    p->mAF = 0;
    p->mAD = 0;
    p->mAE = 0;
    p->mpHelper = 0;
    p->mA0 = 0;
    gabi::call(0x02759C28, name, STR(0x10008C70) /* "mDoExt_J3DModelPacketS" */);
    return p;
}
VERIFY(0x02080404, mDoExt_J3DModelPacketS_ct);

/* 02082DDC mDoExt_J3DModelPacketS::~mDoExt_J3DModelPacketS (HD) */
void mDoExt_J3DModelPacketS_dt(mDoExt_J3DModelPacketS_l* p, s32 flags) {
    WWHD_FUNC(0x02082DDC, void, p, flags);
    if (p == nullptr)
        return;
    u32 h = p->mpHelper;
    p->__vtbl = 0x10008DA8;
    if (h != 0) {
        gabi::call(0x027FD764, h, 3);
        p->mpHelper = 0;
    }
    gabi::call(0x027F13DC, p, 0); /* J3DPacket::~J3DPacket */
    if (flags & 1)
        operator_delete(p);
}
VERIFY(0x02082DDC, mDoExt_J3DModelPacketS_dt);

/* 02082E58 mDoExt_J3DModelPacketS::setMaterial (HD): front-face culling and the packet's flags
 * written into the GX2 state block (GameCube: GFSetCullMode + a display list) */
void mDoExt_J3DModelPacketS_setMaterial(mDoExt_J3DModelPacketS_l* p, u32 st) {
    WWHD_FUNC(0x02082E58, void, p, st);
    u32 w = gabi::load<u32>(st + 0xEC);
    gabi::store<u32>(st + 0xC, 3);
    gabi::store<u32>(st + 0xE4, 4);
    gabi::store<f32>(st + 0xE8, 0.0f);
    gabi::store<u8>(st + 0xE0, 1);
    gabi::store<u32>(st + 0xEC, (((w & 0xFFFFFFF0u) + 7) & 0xFFFFFF0Fu) + 0x10);
    gabi::store<u32>(st + 8, 0);
    if (p->mAE != 0)
        gabi::store<u32>(st + 0xEC, gabi::load<u32>(st + 0xEC) & 0xFFFFFF00u);
    if (p->mAF != 0) {
        gabi::store<u8>(st + 1, 0);
        gabi::store<u32>(st + 4, 2);
    }
}
VERIFY(0x02082E58, mDoExt_J3DModelPacketS_setMaterial);

/* 020844E0 sead::SafeString::~SafeString (this TU's copy, deleting; vtable 0x10008A1C slot 0xC) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x020844E0, void, p, flags);
    if (p == nullptr)
        return;
    if (flags & 1)
        operator_delete(p);
}
VERIFY(0x020844E0, SafeString_dt);

/* 0208AC64 dPa_smokeEcallBack::~dPa_smokeEcallBack (this TU's copy; nothing to destroy) */
static void smokeEcallBack_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0208AC64, void, p, flags);
    if (p == nullptr)
        return;
    if (flags & 1)
        operator_delete(p);
}
VERIFY(0x0208AC64, smokeEcallBack_dt);

/* 0208AC78 part_s::~part_s */
static void part_s_dt(part_s* p, s32 flags) {
    WWHD_FUNC(0x0208AC78, void, p, flags);
    if (p == nullptr)
        return;
    gabi::call(0x02515AE8, &p->mPartSph, 2); /* dCcD_Sph::~dCcD_Sph */
    mDoExt_J3DModelPacketS_dt(&p->m004, 2);
    if (flags & 1)
        operator_delete(p);
}
VERIFY(0x0208AC78, part_s_dt);

/* 0208AEF4 sead::SafeString::assureTerminationImpl_ (this TU's copy: empty; vtable 0x10008A1C slot 0x14) */
void SafeString_assureTermination(SafeString* s) {
    WWHD_FUNC(0x0208AEF4, void, s);
}
VERIFY(0x0208AEF4, SafeString_assureTermination);

/* 0207D04C */
void move_se_set(bgn_class* i_this) {
    WWHD_FUNC(0x0207D04C, void, i_this);
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_28;
    gabi::Local<cXyz> tmp;
    cXyz_mi(&actor->current.pos, local_28, &actor->old.pos);
    local_28->y = 0.0f;
    u32 uVar2 = f2u(std_sqrtf(PSVECSquareMag(local_28)) * 3.5f);
    if (uVar2 > 100)
        uVar2 = 100;
    fopAcM_seStart(actor, 0x705A /* JA_SE_CM_BGN_MECHA_ROTATE */, uVar2);
    cXyz_mi(&i_this->mC728, tmp, &i_this->mC734);
    u32 y = gabi::load<u32>(gabi::ea(tmp.get()) + 4);
    local_28->z = 0.0f;
    gabi::store<u32>(gabi::ea(local_28.get()) + 4, y);
    local_28->x = 0.0f;
    uVar2 = f2u(std_sqrtf(PSVECSquareMag(local_28)));
    if (uVar2 > 100)
        uVar2 = 100;
    fopAcM_seStart(actor, 0x705B /* JA_SE_CM_BGN_MECHA_ROPE */, uVar2);
}
VERIFY(0x0207D04C, move_se_set);

/* 0207D204 */
s32 gr_check(bgn_class* i_this, cXyz* param_2) {
    WWHD_FUNC(0x0207D204, s32, i_this, param_2);
    fopAc_ac_c* actor = i_this;
    gabi::Local<dBgS_LinChk_bgn> linChk;
    cBgS_LinChk_ct(linChk);
    for (int i = 0; i < 7; i++) linChk->mPass[i] = 0;
    linChk->mGrp = 1;
    linChk->mpGrpPassChk = linChk.a + 0x64;
    linChk->mpPolyPassChk = linChk.a + 0x58;
    linChk->__vtbl_10 = 0x10008A94;
    linChk->__vtbl_64 = 0x10008AB4;
    linChk->__vtbl_58 = 0x10008AC4;
    linChk->__vtbl_20 = 0x10008AA4;

    gabi::Local<cXyz> local_ac;
    gabi::Local<cXyz> local_b8;
    f32 x = param_2->x;
    f32 y = param_2->y;
    f32 z = param_2->z;
    local_ac->x = x;
    local_ac->y = y + 200.0f;
    local_ac->z = z;
    local_b8->x = x;
    local_b8->y = y - 1000.0f;
    local_b8->z = z;
    dBgS_LinChk_Set(linChk, local_ac, local_b8, actor);
    s32 ret = TRUE;
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        param_2->copy(linChk->mCross);
        param_2->y = REG0_F(8) + -2.0f;
        if (dBgS_GetAttributeCode(dComIfG_Bgsp(), gabi::at<u8>(linChk.a + 0x14)) == 0x13 /* dBgS_Attr_WATER_e */)
            ret = FALSE;
    }
    linChk->__vtbl_58 = 0x10008AC4;
    linChk->__vtbl_64 = 0x10008A44;
    linChk->__vtbl_20 = 0x10008A34;
    cBgS_LinChk_dt(linChk, 0);
    return ret;
}
VERIFY(0x0207D204, gr_check);

/* 0207D3AC */
static void* s_b_sub(void* param_1, void* param_2) {
    WWHD_FUNC(0x0207D3AC, void*, param_1, param_2);
    bgn_class* bgn = (bgn_class*)param_2;
    fopAc_ac_c* bomb = (fopAc_ac_c*)param_1;
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0x126 /* fpcNm_BOMB_e */ &&
        gabi::call<BOOL>(0x020CB98C, bomb) /* daBomb_c::get_explode_instant */) {
        gabi::Local<cXyz> local_18;
        f32 x = bomb->current.pos.x;
        f32 y = bomb->current.pos.y;
        local_18->x = x;
        f32 z = bomb->current.pos.z;
        local_18->y = y;
        local_18->z = z;
        if (!gr_check(bgn, local_18)) {
            dComIfGp_particle_set(0x8414 /* ID_AK_SN_KGTT1PUNCHSPLASH00 */, local_18);
            dComIfGp_particle_set(0x8415 /* ID_AK_SN_KGTT1PUNCHSPLASH01 */, local_18);
            dComIfGp_particle_set(0x8416 /* ID_AK_SN_KGTT1PUNCHHAMON00 */, local_18);
            dComIfGp_particle_setSimple(0x840E /* ID_AK_SN_O_KGTCOMMONSPLASH01 */, local_18, 0xFF);
            dComIfGp_particle_setSimple(0x8408 /* ID_AK_SN_O_KGTCOMMONHAMON01 */, local_18, 0xFF);
        }
    }
    return nullptr;
}
VERIFY(0x0207D3AC, s_b_sub);

/* 0207D54C (cXyz param_2 by value: a pointer to the caller's copy) */
void attack_eff_set(bgn_class* i_this, cXyz* param_2, int param_3) {
    WWHD_FUNC(0x0207D54C, void, i_this, param_2, param_3);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (!gr_check(i_this, param_2)) {
        dComIfGp_particle_set(0x8414 /* ID_AK_SN_KGTT1PUNCHSPLASH00 */, param_2);
        dComIfGp_particle_set(0x8415 /* ID_AK_SN_KGTT1PUNCHSPLASH01 */, param_2);
        dComIfGp_particle_set(0x8416 /* ID_AK_SN_KGTT1PUNCHHAMON00 */, param_2);
        dComIfGp_particle_setSimple(0x840E /* ID_AK_SN_O_KGTCOMMONSPLASH01 */, param_2, 0xFF);
        dComIfGp_particle_setSimple(0x8408 /* ID_AK_SN_O_KGTCOMMONHAMON01 */, param_2, 0xFF);
        if (param_3 <= 1) {
            fopAcM_seStart(player, 0x6A41 /* JA_SE_CM_BGN_D_HIT_PUNCH_W */, 0);
        } else {
            fopAcM_seStart(player, 0x5974 /* JA_SE_CM_BGN_D_FALL_WATER */, 0);
        }
    } else {
        if (param_3 <= 1) {
            fopAcM_seStart(player, 0x6A40 /* JA_SE_CM_BGN_D_HIT_PUNCH */, 0);
        } else {
            fopAcM_seStart(player, 0x5975 /* JA_SE_CM_BGN_D_FALL */, 0);
        }
        if (param_3 > 1)
            param_3 = 1;
        void* cb = i_this->mPunchSmokeCb[param_3];
        dPa_smokeEcallBack_end((dPa_smokeEcallBack*)cb);
        s8 room = fopAcM_GetRoomNo(actor);
        JPABaseEmitter* emitter = dComIfGp_particle_setToon(0xA418 /* ID_AK_ST_KGTT1PUNCHSMOKE00 */, param_2, nullptr, nullptr, 0xA0, cb, room);
        if (emitter != nullptr) {
            u32 e = gabi::ea(emitter);
            u32 bg = bg_tevstr();
            gabi::store<u8>(e + 0x244, gabi::load<u8>(bg + 0x91));
            gabi::store<u8>(e + 0x246, gabi::load<u8>(bg + 0x95));
            gabi::store<u8>(e + 0x245, gabi::load<u8>(bg + 0x93));
            gabi::store<u8>(e + 0x248, gabi::load<u8>(bg + 0x98));
            gabi::store<u8>(e + 0x249, gabi::load<u8>(bg + 0x99));
            gabi::store<u8>(e + 0x24A, gabi::load<u8>(bg + 0x9A));
        }
        dComIfGp_particle_set(0x8417 /* ID_AK_SN_KGTT1PUNCHHAHEN00 */, param_2);
    }
}
VERIFY(0x0207D54C, attack_eff_set);

/* 0207D858 */
void part_control_0(bgn_class* i_this, int param_2, part_s* param_3, move_s* param_4, f32 param_5) {
    WWHD_FUNC(0x0207D858, void, i_this, param_2, param_3, param_4, param_5);
    gabi::Local<cXyz> local_d0;
    gabi::Local<s32> ftmp;
    gabi::Local<cXyz> local_e8;
    gabi::Local<cXyz> cStack_dc;
    gabi::Local<cXyz> sum;
    local_e8->x = 0.0f;
    local_e8->y = 0.0f;
    local_e8->z = 0.0f;
    if (param_3 == &i_this->mHeadParts[0] && (i_this->mAAA8[0].m2D0 != 0 || i_this->mAAA8[1].m2D0 != 0)) {
        cMtx_YrotS(calc_mtx(), i_this->mC314.y);
        local_d0->x = 0.0f;
        local_d0->y = 0.0f;
        local_d0->z = REG0_F(6) + 200.0f;
        MtxPosition(local_d0, local_e8);
    } else if (param_3 == &i_this->mLeftArmParts[0] && i_this->mAAA8[3].m2D0 != 0) {
        cMtx_YrotS(calc_mtx(), i_this->mC314.y);
        local_d0->y = 0.0f;
        local_d0->z = 0.0f;
        local_d0->x = REG0_F(8) + 20.0f;
        MtxPosition(local_d0, local_e8);
    } else if (param_3 == &i_this->mRightArmParts[0] && i_this->mAAA8[4].m2D0 != 0) {
        cMtx_YrotS(calc_mtx(), i_this->mC314.y);
        local_d0->y = 0.0f;
        local_d0->z = 0.0f;
        local_d0->x = -(REG0_F(8) + 20.0f);
        MtxPosition(local_d0, local_e8);
    }
    f32 dVar11 = (i_this->mC7BC - 50.0f) + REG0_F(6);
    f32 dVar10;
    param_3++;
    if (param_4->m2D0 == 0) {
        dVar10 = l_HIO().m16C;
    } else if (i_this->mCSMode != 0) {
        dVar10 = -40.0f;
    } else {
        dVar10 = l_HIO().m170;
    }
    for (s32 i = 1; i < (s32)((u32)param_2 + 1); i++, param_3++) {
        part_s* prev = param_3 - 1;
        f32 fVar3 = dVar11 * prev->m0F4;
        f32 y2 = param_3->m0D4.y + dVar10;
        if (!(y2 > fVar3))
            y2 = fVar3;
        f32 m2f4 = param_4->m2F4;
        f32 dx = param_3->m0D4.x - prev->m0D4.x;
        f32 sn = cM_ssin(param_4->m2FA + i * (REG0_S(3) + 8000));
        f32 dz = param_3->m0D4.z - prev->m0D4.z;
        f32 temp_f25 = dz + local_e8->z;
        s16 iVar5 = (s16)gabi::ftoi(sn * m2f4);
        f32 temp_f26 = y2 - prev->m0D4.y;
        f32 cs = cM_scos(param_4->m2FC + i * (REG0_S(4) + 9000));
        s16 iVar2 = (s16)gabi::ftoi(cs * m2f4);
        f32 temp_f30 = dx + local_e8->x;
        s16 a = cM_atan2s(temp_f26, temp_f25);
        f32 zz = temp_f25 * temp_f25;
        prev->m0E0.x = (s16)(iVar5 - a);
        f32 sq = std_sqrtf(gabi::fmadds(temp_f26, temp_f26, zz));
        s16 b = cM_atan2s(temp_f30, sq);
        s16 rx = prev->m0E0.x;
        prev->m0E0.y = (s16)(b + iVar2);
        cMtx_XrotS(calc_mtx(), rx);
        cMtx_YrotM(calc_mtx(), prev->m0E0.y);
        f32 len = gabi::fmuls_ppc(gabi::fmuls_ppc(param_5, prev->m0F4), i_this->mCC80);
        local_d0->x = 0.0f;
        local_d0->y = 0.0f;
        local_d0->z = len;
        MtxPosition(local_d0, cStack_dc);
        cXyz_pl(&prev->m0D4, sum, cStack_dc);
        param_3->m0D4.copy(*sum);
    }
}
VERIFY(0x0207D858, part_control_0);

/* 0207DC84 HD: the final shift uses PSVECSubtract */
void part_control_2(bgn_class* i_this, int param_2, part_s* param_3, f32 param_4) {
    WWHD_FUNC(0x0207DC84, void, i_this, param_2, param_3, param_4); /* (i_this is not read) */
    gabi::Local<cXyz> local_84;
    gabi::Local<cXyz> cStack_90;
    gabi::Local<cXyz> sum;
    gabi::Local<cXyz> local_9c;
    f32 x0 = param_3->m0D4.x;
    local_9c->x = x0;
    f32 z0 = param_3->m0D4.z;
    f32 y0 = param_3->m0D4.y;
    local_9c->z = z0;
    local_9c->y = y0;
    part_s* ppVar6 = gabi::at<part_s>(gabi::ea(param_3) + (u32)param_2 * 0x3F0 - 0x3F0);
    if (param_2 - 1 >= 0) {
        for (s32 n = param_2; n != 0; n--, ppVar6--) {
            f32 y = ppVar6[0].m0D4.y - ppVar6[1].m0D4.y;
            f32 z = ppVar6[0].m0D4.z - ppVar6[1].m0D4.z;
            f32 x = ppVar6[0].m0D4.x - ppVar6[1].m0D4.x;
            s16 iVar4 = (s16)-cM_atan2s(y, z);
            f32 zz = z * z;
            f32 sq = std_sqrtf(gabi::fmadds(y, y, zz));
            s16 iVar5 = cM_atan2s(x, sq);
            cMtx_XrotS(calc_mtx(), iVar4);
            cMtx_YrotM(calc_mtx(), iVar5);
            local_84->x = 0.0f;
            local_84->y = 0.0f;
            local_84->z = param_4 * ppVar6[0].m0F4;
            MtxPosition(local_84, cStack_90);
            cXyz_pl(&ppVar6[1].m0D4, sum, cStack_90);
            ppVar6[0].m0D4.copy(*sum);
        }
    }
    ppVar6++;
    cXyz_mi(&ppVar6->m0D4, sum, local_9c);
    local_84->copy(*sum);
    for (s32 n = param_2; n > 0; n--, ppVar6++) {
        PSVECSubtract(&ppVar6->m0D4, local_84, &ppVar6->m0D4);
    }
}
VERIFY(0x0207DC84, part_control_2);

/* 0207DE64 HD: while the action step mC748 is 6 the parts of the normal branch get no Z rotation */
void part_mtx_set(bgn_class* i_this, int param_2, part_s* param_3, int param_4, int param_5) {
    WWHD_FUNC(0x0207DE64, void, i_this, param_2, param_3, param_4, param_5);
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> local_8c;
    gabi::Local<cXyz> local_98;
    local_8c->z = 0.0f;
    local_8c->x = 0.0f;
    local_8c->y = 0.0f;
    for (s32 i = 0; i < param_2; i++, param_3++) {
        if (i_this->m0302 == param_3->m0D2) {
            param_3->mPartArrowHitEffectTimer = 100;
            param_3->mPartArrowHitFlashTimer = 50;
        }
        if (param_3->mPartArrowHitFlashTimer != 0)
            param_3->mPartArrowHitFlashTimer = (s16)(param_3->mPartArrowHitFlashTimer - 1);
        cLib_addCalcAngleS2(&param_3->m0E0.z, i_this->mC314.y, 4, 0x2000);
        f32 y = param_3->m0D4.y;
        f32 c = i_this->mC7BC;
        f32 z = param_3->m0D4.z;
        f32 x = param_3->m0D4.x;
        if (!(y > c)) {
            y = c;
            param_3->m0D4.y = y;
        }
        MtxTrans(x, y, z, false);
        if (param_4 == 7 && i_this->mAAA8[7].m2D0 != 0) {
            cMtx_YrotM(calc_mtx(), param_3->m0E0.y);
            cMtx_XrotM(calc_mtx(), param_3->m0E0.x);
            cMtx_ZrotM(calc_mtx(), (s16)-param_3->m0E0.z);
        } else {
            cMtx_XrotM(calc_mtx(), param_3->m0E0.x);
            cMtx_YrotM(calc_mtx(), param_3->m0E0.y);
            if (i_this->mC748 != 6) {
                if (param_3->m0E0.x < 0) {
                    cMtx_ZrotM(calc_mtx(), param_3->m0E0.z);
                } else {
                    cMtx_ZrotM(calc_mtx(), (s16)-param_3->m0E0.z);
                }
            }
        }
        f32 hio = l_HIO().m0F4;
        f32 fVar4 = gabi::fmuls_ppc(param_3->m0F4, hio);
        if ((u32)i != (u32)(param_2 - 1) && (param_4 == 3 || param_4 == 4)) {
            f32 diff = gabi::fsubs_ppc(i_this->mC324[param_4 - 3], l_HIO().m124);
            f32 fVar5 = gabi::fmadds(diff, gabi::fadds_ppc(REG0_F(0), 0.005f), 1.0f);
            MtxScale(fVar4, fVar4, gabi::fmuls_ppc(fVar4, fVar5), true);
        } else {
            MtxScale(fVar4, fVar4, gabi::fmuls_ppc(fVar4, 1.0f), true);
        }
        if (param_4 == 0) {
            s16 uVar7 = (s16)(i_this->mAAA8[0].m300 + i_this->mAAA8[1].m300);
            f32 f = gabi::fmuls_ppc((f32)uVar7, gabi::fadds_ppc(REG0_F(14), 170.0f));
            s16 sVar8 = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_ssin(uVar7 * 0x2100), f));
            s16 iVar1 = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_ssin(sVar8 * 0x2300), f));
            cMtx_YrotM(calc_mtx(), (s16)(i_this->mC744 + l_HIO().m03C + iVar1));
            cMtx_ZrotM(calc_mtx(), (s16)(l_HIO().m040 + sVar8));
        }
        MtxTrans(0.0f, 0.0f, l_HIO().m168, true);
        if (param_4 == 0) {
            MtxTrans(0.0f, l_HIO().m034, l_HIO().m038, true);
            cMtx_XrotM(calc_mtx(), l_HIO().m03E);
        } else if (param_4 == 7 && (u32)i == (u32)(param_2 - 1)) {
            s8 health = actor->health;
            if (health == 3) {
                param_3->mpPartModel = i_this->mpJyakutenAModel;
            } else if (health == 2) {
                param_3->mpPartModel = i_this->mpJyakutenBModel;
            } else if (health == 1) {
                param_3->mpPartModel = i_this->mpJyakutenCModel;
            } else {
                param_3->mpPartModel = nullptr;
            }
        }
        if (param_3->mpPartModel != nullptr)
            J3DModel_setBaseTRMtx(param_3->mpPartModel, calc_mtx());
        if (param_4 == 0) {
            if (i_this->mAAA8[0].m2D0 == 0 || i_this->mAAA8[1].m2D0 == 0 ||
                gabi::ftoi(i_this->mpMorf->getFrame()) != 0x18) {
                i_this->mpMorf->play(nullptr, 0, 0);
            }
            i_this->mpMorf->calc();
        }
        MtxPosition(gabi::at<cXyz>(0x10461CAC) /* zero */, local_98);
        u32 px = gabi::load<u32>(local_98.a);
        u32 py = gabi::load<u32>(local_98.a + 4);
        u32 pz = gabi::load<u32>(local_98.a + 8);
        gabi::store<u32>(gabi::ea(&param_3->m224), px);
        gabi::store<u32>(gabi::ea(&param_3->m224) + 4, py);
        gabi::store<u32>(gabi::ea(&param_3->m224) + 8, pz);
        if (param_4 == 7 && (u32)i == (u32)(param_2 - 1)) {
            i_this->mCoreSph.SetR(gabi::fadds_ppc(REG0_F(4), 150.0f));
            i_this->mCoreSph.SetC(local_98);
            u32 cx = gabi::load<u32>(local_98.a);
            u32 cy = gabi::load<u32>(local_98.a + 4);
            u32 cz = gabi::load<u32>(local_98.a + 8);
            gabi::store<u32>(gabi::ea(&i_this->mCA54), cx);
            gabi::store<u32>(gabi::ea(&i_this->mCA54) + 4, cy);
            gabi::store<u32>(gabi::ea(&i_this->mCA54) + 8, cz);
            dComIfG_Ccsp_Set(&i_this->mCoreSph);
        }
        param_3->mPartSph.SetR(gabi::fmuls_ppc(fVar4, gabi::fadds_ppc(REG0_F(0), 120.0f)));
        param_3->mPartSph.SetC(local_98);
        dComIfG_Ccsp_Set(&param_3->mPartSph);
        if ((u32)i == (u32)param_5) {
            if (param_4 == 0) {
                local_8c->x = 90.0f;
                local_8c->y = 180.0f;
                local_8c->z = -40.0f;
                MtxPosition(local_8c, &i_this->mC33C[0]);
                local_8c->x = -(f32)local_8c->x;
                MtxPosition(local_8c, &i_this->mC33C[1]);
                local_8c->x = 0.0f;
                local_8c->z = 0.0f;
            } else {
                local_8c->y = 0.0f;
                local_8c->x = 0.0f;
                local_8c->z = l_HIO().m168;
                MtxPosition(local_8c, &i_this->mC33C[param_4]);
                local_8c->z = 0.0f;
                local_8c->x = 0.0f;
            }
        }
        if (param_3->mPartArrowHitEffectTimer != 0) {
            s16 t = (s16)(param_3->mPartArrowHitEffectTimer - 1);
            param_3->mPartArrowHitEffectTimer = t;
            f32 fVar6 = gabi::fmuls_ppc(gabi::fmuls_ppc(param_3->m0F4, (f32)t), gabi::fadds_ppc(REG_F(8, 0), 0.04f));
            if (param_3->mpPartArrowHitEmitter1 == nullptr) {
                param_3->mpPartArrowHitEmitter1 = dComIfGp_particle_set(0x3ED /* ID_AK_JN_CCTHUNDER00 */, &param_3->m0D4);
            } else {
                JPABaseEmitter_setGlobalTranslation(param_3->mpPartArrowHitEmitter1, param_3->m0D4.x, param_3->m0D4.y, param_3->m0D4.z);
                JPABaseEmitter_setGlobalScale(param_3->mpPartArrowHitEmitter1, fVar6, fVar6, fVar6);
            }
            if (param_3->mpPartArrowHitEmitter2 == nullptr) {
                local_8c->y = fVar6;
                local_8c->z = fVar6;
                local_8c->x = fVar6;
                param_3->mpPartArrowHitEmitter2 = dComIfGp_particle_set(0x3EE /* ID_AK_JN_CCTHUNDER01 */, &param_3->m0D4);
            } else {
                JPABaseEmitter_setGlobalTranslation(param_3->mpPartArrowHitEmitter2, param_3->m0D4.x, param_3->m0D4.y, param_3->m0D4.z);
                JPABaseEmitter_setGlobalScale(param_3->mpPartArrowHitEmitter2, fVar6, fVar6, fVar6);
            }
        } else {
            if (param_3->mpPartArrowHitEmitter1 != nullptr) {
                JPABaseEmitter_becomeInvalidEmitter(param_3->mpPartArrowHitEmitter1);
                param_3->mpPartArrowHitEmitter1 = nullptr;
            }
            if (param_3->mpPartArrowHitEmitter2 != nullptr) {
                JPABaseEmitter_becomeInvalidEmitter(param_3->mpPartArrowHitEmitter2);
                param_3->mpPartArrowHitEmitter2 = nullptr;
            }
        }
    }
}
VERIFY(0x0207DE64, part_mtx_set);

/* 02083558 */
static daBgn_HIO_c* daBgn_HIO_c_ct(daBgn_HIO_c* p) {
    WWHD_FUNC(0x02083558, daBgn_HIO_c*, p);
    if (p == nullptr) {
        p = (daBgn_HIO_c*)operator_new(0x17C);
        if (p == nullptr)
            return p;
    }
    p->__vtbl = 0x10008AE4;
    p->mNo = -1;
    p->m00C = 1;
    p->m00D = 1;
    p->m010 = 50.0f;
    p->m014 = 10;
    p->m016 = 10;
    p->m018 = 80;
    p->m01C = 1000.0f;
    p->m020 = 10000.0f;
    p->m025 = 0;
    p->m026 = 0;
    p->m027 = 0;
    p->m024 = 0;
    p->m028 = 1;
    p->m008 = 200.0f;
    p->m004 = 0;
    p->m029 = 0;
    p->m02A = 0;
    p->m02B = 0;
    p->m02C = 0;
    p->m02D = 0;
    p->m02E = 0;
    p->m02F = 0;
    p->m030 = 0;
    p->m034 = -20.0f;
    p->m038 = -30.0f;
    p->m03C = 0;
    p->m03E = 22000;
    p->m040 = 0;
    p->m042 = 0;
    p->m044 = 0;
    p->m048 = 0.0f;
    p->m04C = 950.0f;
    p->m050 = 0.0f;
    p->m054 = 0.0f;
    p->m058 = 2000.0f;
    p->m05C = 800.0f;
    p->m060 = 0.0f;
    p->m064 = -800.0f;
    p->m068 = -800.0f;
    p->m06C = 700.0f;
    p->m070 = -900.0f;
    p->m074 = 300.0f;
    p->m078 = -700.0f;
    p->m07C = -900.0f;
    p->m080 = 300.0f;
    p->m084 = 500.0f;
    p->m088 = -1300.0f;
    p->m08C = -100.0f;
    p->m090 = -500.0f;
    p->m094 = -1300.0f;
    p->m098 = -100.0f;
    p->m09C = 0.0f;
    p->m0A0 = -700.0f;
    p->m0A4 = -1200.0f;
    p->m0A8 = 0;
    p->m0AA = 0;
    p->m0AC = 0;
    p->m0AE = 0;
    p->m0B0 = 0;
    p->m0B2 = 0;
    p->m0B4 = 0;
    p->m0B6 = 0;
    p->m0B8 = 0;
    p->m0BA = 0;
    p->m0BC = 0;
    p->m0BE = 0;
    p->m0C0 = 0;
    p->m0C2 = 0;
    p->m0C4 = 0;
    p->m0C6 = 0;
    p->m0C8 = 0;
    p->m0CA = 0;
    p->m0CC = 0;
    p->m0CE = 0;
    p->m0D0 = 0;
    p->m0D2 = 0;
    p->m0D8 = 120;
    p->m0DA = 20;
    p->m0DC = 1200;
    p->m0DE = 1500;
    p->m0E0 = 2000;
    p->m0E2 = 300;
    p->m0E4 = 200;
    p->m0E6 = 150;
    p->m0E8 = 15;
    p->m0EA = 15;
    p->m0EC = 15;
    p->m0EE = 500;
    p->m0D4 = 40.0f;
    p->m0F0 = 5;
    p->m0F2 = 10;
    p->m0F4 = 1.3f;
    p->m0FC = 3.25f;
    p->m0F8 = 1.6f;
    p->m100 = 1.8f;
    p->m104 = 1.2f;
    p->m108 = 0.3f;
    p->m10C = 1.2f;
    p->m110 = 0.4f;
    p->m114 = 1.2f;
    p->m118 = 0.1f;
    p->m11C = 185.0f;
    p->m124 = 185.0f;
    p->m120 = 185.0f;
    p->m128 = 185.0f;
    p->m168 = 100.0f;
    p->m12C = 0.0f;
    p->m130 = -10.0f;
    p->m134 = 120.0f;
    p->m138 = 100.0f;
    p->m13C = 25.0f;
    p->m140 = 20.0f;
    p->m144 = 0.0f;
    p->m148 = -115.0f;
    p->m14C = 0.0f;
    p->m150 = 80.0f;
    p->m154 = -20.0f;
    p->m158 = 100.0f;
    p->m15C = 0.0f;
    p->m160 = -60.0f;
    p->m164 = 150.0f;
    p->m16C = -50.0f;
    p->m170 = -100.0f;
    p->mKeeseNum3HP = 3;
    p->mKeeseNum2HP = 4;
    p->mKeeseNum1HP = 5;
    p->mKeeseNumMax = 5;
    return p;
}
VERIFY(0x02083558, daBgn_HIO_c_ct);

/* 02083924 the TU's statics: the header statics, zero, l_HIO, bg_tevstr, w_pos, the twelve pose tables
 * (dance_pause_1..4, punch_*_d, start_pause; eight csXyz each, constructed out of line) and center_pos */
static const s16 bgn_pose_tables[12][8][3] = {
    /* dance_pause_1 (0x10461A5C) */ {{0, 0, -1200}, {0, 0, 0}, {0, 0, 0}, {500, 1000, 1500}, {-500, 1000, 1500}, {75, 900, -700}, {500, -1300, -100}, {-200, 600, -400}},
    /* dance_pause_2 (0x10461A8C) */ {{0, 0, -1200}, {0, 0, 0}, {0, 300, 0}, {500, -500, 1500}, {-500, -500, 1500}, {75, 400, -700}, {500, -1100, -800}, {-200, 200, -500}},
    /* dance_pause_3 (0x10461ABC) */ {{0, 0, -1000}, {0, 0, 0}, {500, 0, -200}, {-200, 1000, 350}, {200, 1400, 350}, {-400, 500, 500}, {400, -200, -300}, {800, 400, -200}},
    /* dance_pause_4 (0x10461AEC) */ {{0, 0, -1000}, {0, 0, 0}, {-500, 0, -200}, {-200, 1400, 350}, {200, 1000, 350}, {-400, -200, -300}, {400, 500, 500}, {-800, 400, -200}},
    /* punch_lr1_d (0x10461B1C) */ {{0, 0, -1700}, {0, 0, 0}, {-150, 0, 650}, {100, 1400, -500}, {-100, 1200, 500}, {200, 300, 900}, {-100, 500, 900}, {-600, 500, 300}},
    /* punch_lr12_d (0x10461B4C) */ {{0, 0, -1700}, {0, 0, 0}, {150, 0, 650}, {100, 1200, -500}, {-100, 1400, 500}, {200, 500, 900}, {-100, 300, 900}, {600, 500, 300}},
    /* punch_lr2_d (0x10461B7C) */ {{0, 0, 0}, {0, 0, 0}, {0, 0, 500}, {-500, 150, 1000}, {500, 150, 1000}, {150, 600, 1000}, {-150, 600, 900}, {0, 950, 100}},
    /* punch_r1_d (0x10461BAC) */ {{-150, 0, 0}, {0, 0, 0}, {-200, 0, 400}, {0, 400, 600}, {-150, 1600, -400}, {-600, 300, 650}, {-1200, 0, 200}, {700, 700, 200}},
    /* punch_r2_d (0x10461BDC) */ {{150, 0, -500}, {0, 0, 0}, {600, 0, 0}, {650, 1000, -500}, {-600, 0, 1200}, {1000, 250, 2000}, {900, 200, -300}, {0, 800, -350}},
    /* punch_l1_d (0x10461C0C) */ {{150, 0, 0}, {0, 0, 0}, {200, 0, 400}, {150, 1600, -400}, {0, 400, 600}, {1200, 0, 2000}, {600, 300, 650}, {-700, 700, 200}},
    /* punch_l2_d (0x10461C3C) */ {{-150, 0, -500}, {0, 0, 0}, {-600, 0, 0}, {600, 0, 1200}, {-650, 1000, -500}, {-900, 200, -300}, {-1000, 250, 2000}, {0, 800, -350}},
    /* start_pause (0x10461C6C) */ {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {-100, 250, 0}, {400, 250, 0}, {-200, 0, 0}, {200, 0, 0}, {0, 0, 0}},
};
static void __sinit_d_a_bgn_cpp() {
    WWHD_FUNC(0x02083924, void, (u32)0);
    /* header statics (as sinit_header_statics_z, with a word between the angle limits and the flags) */
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10461C9C + 4 * i, 0);
    __register_global_object(0x10190FE8);
    gabi::store<f32>(0x10461A4C, -3.1415927f);
    gabi::store<f32>(0x10461A50, 3.1415927f);
    gabi::call(0x028ED6F8, 0x10461A58u);
    __register_global_object(0x10190FF4);
    gabi::call(0x028EAB2C, 0x10461A59u);
    __register_global_object(0x10191000);
    gabi::store<f32>(0x10461CAC, 0.0f); /* zero */
    gabi::store<f32>(0x10461CB0, 0.0f);
    gabi::store<f32>(0x10461CB4, 0.0f);
    daBgn_HIO_c_ct(&l_HIO());
    tevstr_tmpl t;
    t.load();
    t.store(bg_tevstr());
    gabi::store<f32>(0x10461CB8, 0.0f); /* w_pos */
    gabi::store<f32>(0x10461CBC, 0.0f);
    gabi::store<f32>(0x10461CC0, 0.0f);
    for (int k = 0; k < 12; k++)
        for (int j = 0; j < 8; j++)
            csXyz_ct(gabi::at<csXyz>(0x10461A5C + k * 0x30 + j * 6), bgn_pose_tables[k][j][0], bgn_pose_tables[k][j][1],
                     bgn_pose_tables[k][j][2]);
    gabi::store<f32>(0x10461CD0, 0.0f); /* center_pos */
    gabi::store<f32>(0x10461CD4, 0.0f);
    gabi::store<f32>(0x10461CD8, 0.0f);
}
VERIFY(0x02083924, __sinit_d_a_bgn_cpp);

/* 02084374 HD: the material's blend/z state is set through the HD pixel-engine block */
void water1_disp(bgn_class* i_this) {
    WWHD_FUNC(0x02084374, void, i_this);
    MtxTrans(0.0f, REG0_F(11), 0.0f, false);
    J3DModel_setBaseTRMtx(i_this->mpWater1Model, calc_mtx());
    setLightTevColorType(dKy_getEnvlight(), i_this->mpWater1Model, (dKy_tevstr_c*)&i_this->mWaterTevStr);
    dComIfGd_setListSky();
    u32 pe = bgn_material_pe(bgn_material0(i_this->mpWater1Model));
    gabi::call(0x027E212C, pe, 1);     /* blend->setType (HD) */
    gabi::call(0x027E18F8, pe + 8, 1);
    gabi::call(0x027E1908, pe + 8, 6);
    gabi::call(0x027E195C, pe + 0x14, 0); /* zMode->setUpdateEnable(0) (HD) */
    mDoExt_modelUpdateDL(i_this->mpWater1Model);
    dComIfGd_setList();
}
VERIFY(0x02084374, water1_disp);

/* 020844F4 */
void start(bgn_class* i_this) {
    WWHD_FUNC(0x020844F4, void, i_this);
    fopAc_ac_c* actor = i_this;
    s16 target = fopAcM_searchPlayerAngleY(actor);
    cLib_addCalcAngleS2(&actor->shape_angle.y, target, 10, 0x400);
    for (s32 i = 0; i < 8; i++) {
        u32 src = 0x10461C6C + 6 * i; /* start_pause[i] */
        u32 dst = gabi::ea(&i_this->mAAA8[i].m2E0);
        gabi::store<u16>(dst, gabi::load<u16>(src));
        gabi::store<u16>(dst + 2, gabi::load<u16>(src + 2));
        gabi::store<u16>(dst + 4, gabi::load<u16>(src + 4));
    }
    if (i_this->mC7AC[0] == 0) {
        i_this->mC748 = 0;
        i_this->mC754 = 1;
        i_this->mC74C = 0;
    }
}
VERIFY(0x020844F4, start);

/* 0208459C */
void tail_attack(bgn_class* i_this) {
    WWHD_FUNC(0x0208459C, void, i_this);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    switch ((u16)i_this->mC74A) {
    case 0:
        i_this->mC76C = 0.0f;
        i_this->mC74A = 1;
        i_this->mC764 = 0;
        i_this->mC7AC[0] = 100;
        break;
    case 1:
        actor->shape_angle.y = (s16)(actor->shape_angle.y + i_this->mC764);
        if (i_this->mC7AC[0] > 0x28) {
            cLib_addCalcAngleS2(&i_this->mC764, 0x800, 1, 0x80);
            cLib_addCalc2(&actor->current.pos.x, player->current.pos.x, 0.1f, i_this->mC76C);
            cLib_addCalc2(&actor->current.pos.z, player->current.pos.z, 0.1f, i_this->mC76C);
            cLib_addCalc2(&i_this->mC76C, REG0_F(19) + 15.0f, 1.0f, 0.2f);
            for (s32 i = 0; i < BGN_TAIL_MAX(); i++) {
                OnAtSetBit(&i_this->mTailParts[i].mPartSph);
            }
            i_this->mAAA8[7].m2E0.z = (s16)gabi::ftoi(REG0_F(3) + -600.0f);
            cLib_addCalc2(&i_this->mC728.y, -550.0f, 0.1f, 20.0f);
        } else {
            cLib_addCalcAngleS2(&i_this->mC764, 0, 1, 0x20);
            cLib_addCalc2(&i_this->mC728.y, 0.0f, 0.1f, 5.0f);
            i_this->mAAA8[7].m2E0.z = 0;
        }
        if (i_this->mC7AC[0] == 0) {
            i_this->mC748 = 0;
            i_this->mC74A = 0;
        }
        break;
    }
}
VERIFY(0x0208459C, tail_attack);

/* 02084804 */
void damage(bgn_class* i_this) {
    WWHD_FUNC(0x02084804, void, i_this);
    fopAc_ac_c* actor = i_this;
    i_this->mpMorf->play(nullptr, 0, 0);
    i_this->mpMorf->play(nullptr, 0, 0);
    switch ((u16)i_this->mC74A) {
    case 0:
        i_this->mC7AC[0] = 0;
        i_this->mC74A = 1;
        actor->speed.y = REG0_F(8) + 100.0f;
        i_this->mC770 = (s16)(REG0_S(0) + 0x32);
        for (s32 i = 0; i < 8; i++) {
            i_this->mAAA8[i].m2F8 = (s16)(REG0_S(7) + 0x46);
            i_this->mAAA8[i].m2F4 = REG0_F(15) + 8000.0f;
        }
        break;
    case 1:
        if (!(i_this->mC728.y > 0.0f)) {
            actor->speed.y = REG0_F(9) + 30.0f;
            i_this->mC7AC[0] = (s16)(REG0_S(5) + 0x28);
            i_this->mC74A = 2;
        }
        break;
    case 2:
        if (i_this->mC7AC[0] == 0) {
            if (i_this->mAAA8[7].m2D0 != 0) {
                i_this->mC748 = 4;
            } else {
                i_this->mC748 = 0;
            }
            i_this->mC74A = 0;
        }
        break;
    }
    i_this->mC728.y = i_this->mC728.y + actor->speed.y;
    actor->speed.y = actor->speed.y - (REG0_F(4) + 10.0f);
    if (!(i_this->mC728.y > 0.0f)) {
        i_this->mC728.y = 0.0f;
        actor->speed.y = 0.0f;
    }
    if (i_this->mC728.y > 100.0f) {
        cLib_addCalc2(&i_this->mC774, REG0_F(9) + 250.0f, 1.0f, REG0_F(10) + 50.0f);
        for (s32 i = 0; i < 8; i++) {
            cLib_addCalc2(&i_this->mAAA8[i].m2EC, REG0_F(9) + 250.0f, 1.0f, REG0_F(10) + 50.0f);
        }
    }
}
VERIFY(0x02084804, damage);

/* 02084B90 */
void head_recover(bgn_class* i_this) {
    WWHD_FUNC(0x02084B90, void, i_this);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    switch ((u16)i_this->mC74A) {
    case 0:
        cLib_addCalc2(&i_this->mC728.y, 4000.0f, 0.2f, (REG0_F(10) + 100.0f) * i_this->mCA98);
        cLib_addCalc2(&i_this->mCA98, 1.0f, 1.0f, 0.02f);
        actor->shape_angle.y = (s16)(actor->shape_angle.y + 0x200);
        if (!(i_this->mC728.y < 3990.0f)) {
            i_this->mC74A = 1;
            i_this->mAAA8[4].m2D0 = 0;
            i_this->mAAA8[3].m2D0 = 0;
            i_this->mC7AC[0] = (s16)(REG0_S(5) + 0x28);
            i_this->mAAA8[1].m2D0 = 0;
            i_this->mAAA8[3].m308 = 2;
            i_this->mAAA8[7].m2D0 = 0;
            i_this->mAAA8[0].m308 = 2;
            i_this->mAAA8[4].m308 = 2;
            i_this->mAAA8[7].m308 = 2;
            i_this->mAAA8[0].m2D0 = 0;
            i_this->mAAA8[1].m308 = 2;
            fopAcM_seStart(player, 0x5972 /* JA_SE_CM_BGN_D_ROPE_RESET */, 0);
        }
        break;
    case 1:
        if (i_this->mC7AC[0] == 0) {
            actor->shape_angle.y = (s16)(actor->shape_angle.y + -0x400);
            cLib_addCalc2(&i_this->mC728.y, 0.0f, 0.05f, REG0_F(10) + 200.0f);
            if (i_this->mC728.y < 5.0f) {
                i_this->mC748 = 0;
                i_this->mC74A = 0;
            }
        }
        break;
    }
}
VERIFY(0x02084B90, head_recover);

/* 02085630 action_s, first part (HD: the rope update of the GameCube action_s is a separate function,
 * 020859AC, called from shape_calc; this part updates the timers, the sway target and the pose chase) */
void action_s(bgn_class* i_this, move_s* param_2, int param_3) {
    WWHD_FUNC(0x02085630, void, i_this, param_2, param_3);
    s16 t = param_2->m2F8;
    if (t != 0) {
        t = (s16)(t - 1);
        param_2->m2F8 = t;
    }
    f32 target = 0.0f;
    if (l_HIO().m025 == 0 && i_this->mC728.y > REG0_F(12) + -900.0f) {
        target = gabi::fadds_ppc(gabi::fmadds((f32)t, REG0_F(2) + 20.0f, 300.0f), REG0_F(13));
    }
    cLib_addCalc2(&param_2->m2F4, target, 0.5f, REG0_F(8) + 200.0f);
    param_2->m2FA = (s16)(param_2->m2FA + (param_3 * 3 + REG0_S(5) + 2600));
    param_2->m2FC = (s16)(param_2->m2FC + (param_3 * 3 + REG0_S(6) + 2300));
    if (param_2->m2FE != 0)
        param_2->m2FE = (s16)(param_2->m2FE - 1);
    if (l_HIO().m025 != 0) {
        param_2->m2D0 = 0;
        daBgn_HIO_c& h = l_HIO();
        switch ((u32)param_3) {
        case 0:
            param_2->m2E0.x = h.m0AA;
            param_2->m2E0.y = h.m0AC;
            param_2->m2E0.z = h.m0AE;
            break;
        case 2:
            param_2->m2E0.x = h.m0B0;
            param_2->m2E0.y = h.m0B2;
            param_2->m2E0.z = h.m0B4;
            break;
        case 3:
            param_2->m2E0.x = h.m0B6;
            param_2->m2E0.y = h.m0B8;
            param_2->m2E0.z = h.m0BA;
            break;
        case 4:
            param_2->m2E0.x = h.m0BC;
            param_2->m2E0.y = h.m0BE;
            param_2->m2E0.z = h.m0C0;
            break;
        case 5:
            param_2->m2E0.x = h.m0C2;
            param_2->m2E0.y = h.m0C4;
            param_2->m2E0.z = h.m0C6;
            break;
        case 6:
            param_2->m2E0.x = h.m0C8;
            param_2->m2E0.y = h.m0CA;
            param_2->m2E0.z = h.m0CC;
            break;
        case 7:
            param_2->m2E0.x = h.m0CE;
            param_2->m2E0.y = h.m0D0;
            param_2->m2E0.z = h.m0D2;
            break;
        }
    }
    if (param_2->m2D0 == 0) {
        cLib_addCalc2(&param_2->m2D4.x, (f32)param_2->m2E0.x, 0.5f, i_this->mC334);
        cLib_addCalc2(&param_2->m2D4.y, (f32)param_2->m2E0.y, 0.5f, i_this->mC334);
        cLib_addCalc2(&param_2->m2D4.z, (f32)param_2->m2E0.z, 0.5f, i_this->mC334);
    }
}
VERIFY(0x02085630, action_s);

/* 020859AC action_s, second part (HD split, called from shape_calc): the blue rope of puppet string
 * param_3 (GameCube: the end of action_s with himo_control inlined). HD: the hanging rope's segments
 * are REG10_F(8) + 80 apart (GameCube 50). */
void action_s_himo(bgn_class* i_this, move_s* param_2, int param_3) {
    WWHD_FUNC(0x020859AC, void, i_this, param_2, param_3);
    fopAc_ac_c* actor = i_this;
    u32 lines = i_this->mBlueRopeMat.mpLines;
    u32 pcVar4 = gabi::load<u32>(lines + param_3 * 16);
    u32 pcVar7 = gabi::load<u32>(lines + param_3 * 16 + 4);
    gabi::Local<cXyz> local_64;
    u32 src = gabi::ea(&i_this->mC33C[param_3]);
    u32 lx = gabi::load<u32>(src);
    u32 ly = gabi::load<u32>(src + 4);
    u32 lz = gabi::load<u32>(src + 8);
    gabi::store<u32>(local_64.a, lx);
    gabi::store<u32>(local_64.a + 4, ly);
    gabi::store<u32>(local_64.a + 8, lz);
    if (param_2->m2D0 != 0) {
        /* himo_control (inline) */
        u32 h = gabi::ea(param_2);
        gabi::store<u32>(h + 4, ly);
        gabi::store<u32>(h + 8, lz);
        gabi::store<u32>(h, lx);
        gabi::store<u32>(pcVar4 + 4, ly);
        gabi::store<u32>(pcVar4 + 8, lz);
        gabi::store<u32>(pcVar4, lx);
        gabi::Local<cXyz> local_ac;
        gabi::Local<cXyz> local_b8;
        gabi::Local<cXyz> local_c4;
        local_ac->x = 0.0f;
        local_ac->y = 0.0f;
        local_ac->z = gabi::fmuls_ppc(gabi::fadds_ppc(REG0_F(12), 15.0f), i_this->mCC80);
        s16 j = 0;
        s16 k = 0;
        cXyz* pcVar5 = &param_2->mHimo[1];
        u32 out = pcVar4 + 0xC;
        for (s32 i = 1; i < 60; i++, pcVar5++, out += 0xC) {
            f32 x = gabi::fsubs_ppc(pcVar5[0].x, pcVar5[-1].x);
            f32 y2 = gabi::fadds_ppc(gabi::fsubs_ppc(pcVar5[0].y, 40.0f), REG0_F(13));
            f32 z = gabi::fsubs_ppc(pcVar5[0].z, pcVar5[-1].z);
            bool bVar9;
            f32 y;
            if (y2 < 0.0f) {
                y = gabi::fsubs_ppc(0.0f, pcVar5[-1].y);
                bVar9 = true;
            } else {
                y = gabi::fsubs_ppc(y2, pcVar5[-1].y);
                bVar9 = false;
            }
            f32 m304 = param_2->m304;
            if (m304 > 0.01f) {
                j = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_ssin(param_2->m2FA + i * (REG0_S(3) + 2000)), m304));
                k = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_scos(param_2->m2FC + i * (REG0_S(4) + 0x9C4)), m304));
            }
            u32 m = gabi::load<u32>(0x1018C7B0);
            s16 a = cM_atan2s(y, z);
            cMtx_XrotS(gabi::at<Mtx34>(m), (s16)(j - a));
            f32 d = gabi::fmadds(y, y, gabi::fmuls_ppc(z, z));
            m = gabi::load<u32>(0x1018C7B0);
            f32 sq = std_sqrtf(d);
            s16 b = cM_atan2s(x, sq);
            cMtx_YrotM(gabi::at<Mtx34>(m), (s16)(b + k));
            MtxPosition(local_ac, local_b8);
            f32 nx = gabi::fadds_ppc(pcVar5[-1].x, local_b8->x);
            pcVar5[0].x = nx;
            pcVar5[0].y = gabi::fadds_ppc(pcVar5[-1].y, local_b8->y);
            f32 nz = gabi::fadds_ppc(pcVar5[-1].z, local_b8->z);
            pcVar5[0].z = nz;
            gabi::store<f32>(out, nx);
            gabi::store<f32>(out + 4, pcVar5[0].y);
            gabi::store<f32>(out + 8, nz);
            s16 c746 = i_this->mC746;
            if (bVar9 && (c746 & 7) == 0 && (((c746 >> 3) ^ i) & 0xF) == 0) {
                u32 pc = gabi::ea(pcVar5);
                gabi::store<u32>(local_c4.a, gabi::load<u32>(pc));
                gabi::store<u32>(local_c4.a + 4, gabi::load<u32>(pc + 4));
                gabi::store<u32>(local_c4.a + 8, gabi::load<u32>(pc + 8));
                if (!gr_check(i_this, local_c4)) {
                    dComIfGp_particle_setSimple(0x8407 /* ID_AK_SN_O_KGTCOMMONHAMON00 */, local_c4, 0xFF);
                    if (i_this->m0304 == 0) {
                        f32 r = gabi::fadds_ppc(cM_rndF(20.0f), 20.0f);
                        s8 room = fopAcM_GetRoomNo(actor);
                        u32 cx = gabi::load<u32>(local_c4.a);
                        u32 cy = gabi::load<u32>(local_c4.a + 4);
                        u32 cz = gabi::load<u32>(local_c4.a + 8);
                        u32 dst = gabi::ea(&i_this->m0308);
                        gabi::store<u32>(dst, cx);
                        gabi::store<u32>(dst + 4, cy);
                        i_this->m0304 = (s16)gabi::ftoi(r);
                        gabi::store<u32>(dst + 8, cz);
                        mDoAud_seStart(0x6A42 /* JA_SE_CM_BGN_STRING_RIPPLE */, &i_this->m0308, 0, dComIfGp_getReverb(room));
                    }
                }
            }
        }
        cLib_addCalc0(&param_2->m304, 1.0f, REG0_F(5) + 100.0f);
    } else {
        gabi::Local<cXyz> local_70;
        gabi::Local<cXyz> cStack_7c;
        gabi::Local<cXyz> sum;
        local_70->y = 0.0f;
        cLib_addCalc0(&param_2->m2EC, 1.0f, 25.0f);
        if (param_2->m300 != 0)
            param_2->m300 = (s16)(param_2->m300 - 1);
        cMtx_YrotS(calc_mtx(), (s16)(param_3 * (REG0_S(2) + 13000)));
        u32 himo = gabi::ea(param_2);
        for (s32 i = 0; i < 60; i++, pcVar4 += 0xC, pcVar7++) {
            s16 t = param_2->m300;
            f32 s = cM_ssin(bgn_cM_rad2s(gabi::fmuls_ppc((f32)i, 0.053247336f)));
            f32 fVar3;
            if (t != 0) {
                fVar3 = gabi::fmuls_ppc(gabi::fmuls_ppc(s, (f32)(s16)param_2->m300), gabi::fadds_ppc(REG0_F(12), 10.0f));
                s16 c746 = i_this->mC746;
                local_70->x = gabi::fmuls_ppc(cM_scos(c746 * (REG0_S(3) + 0x5800)), fVar3);
                local_70->z = gabi::fmuls_ppc(cM_scos(c746 * (REG0_S(5) + 0x5200)), fVar3);
            } else {
                fVar3 = gabi::fmuls_ppc(gabi::fmuls_ppc(s, param_2->m2EC), gabi::fmuls_ppc((f32)(59 - i), 0.01666667f));
                s16 c746 = i_this->mC746;
                local_70->x = gabi::fmuls_ppc(cM_scos(c746 * (REG0_S(3) + 200) + i * (REG0_S(4) + 2000)), fVar3);
                local_70->z = gabi::fmuls_ppc(cM_scos(c746 * (REG0_S(5) + 0xFA) + i * (REG0_S(6) + 2000)), fVar3);
            }
            MtxPosition(local_70, cStack_7c);
            gabi::store<u32>(himo + i * 0xC, gabi::load<u32>(local_64.a));
            gabi::store<u32>(himo + i * 0xC + 4, gabi::load<u32>(local_64.a + 4));
            gabi::store<u32>(himo + i * 0xC + 8, gabi::load<u32>(local_64.a + 8));
            cXyz_pl(local_64, sum, cStack_7c);
            gabi::store<u32>(pcVar4, gabi::load<u32>(sum.a));
            gabi::store<u32>(pcVar4 + 4, gabi::load<u32>(sum.a + 4));
            gabi::store<u32>(pcVar4 + 8, gabi::load<u32>(sum.a + 8));
            gabi::store<u8>(pcVar7, (u8)(REG0_S(2) + 6));
            local_64->y = gabi::fadds_ppc(local_64->y, gabi::fadds_ppc(REG_F(10, 8), 80.0f));
        }
    }
}
VERIFY(0x020859AC, action_s_himo);

/* 02084D6C HD: the first step also resets the facing (shape_angle.y, mC750); at the end of the
 * transformation the phase switch is set directly (m02B5 = 1; GameCube: l_HIO.m024++) */
void hensin(bgn_class* i_this) {
    WWHD_FUNC(0x02084D6C, void, i_this);
    fopAc_ac_c* actor = i_this;
    cLib_addCalc2(&actor->current.pos.x, 0.0f, 0.05f, 50.0f);
    cLib_addCalc2(&actor->current.pos.z, 0.0f, 0.05f, 50.0f);
    switch ((u16)i_this->mC74A) {
    case 0:
        i_this->mC74A = 1;
        for (s32 i = 0; i < 8; i++) {
            i_this->mAAA8[i].m2D0 = 1;
        }
        i_this->mCSMode = 1;
        actor->shape_angle.y = 0;
        i_this->mC750 = 0;
        // fallthrough
    case 1:
    case 2:
    case 3:
    case 4: {
        i_this->mC728.y = i_this->mC728.y + actor->speed.y;
        actor->speed.y = actor->speed.y - (REG0_F(4) + 10.0f);
        f32 floor = REG0_F(2) + -1000.0f;
        if (!(i_this->mC728.y > floor)) {
            i_this->mC728.y = floor;
            i_this->mC770 = (s16)(REG0_S(0) + 0x14);
            for (s32 i = 0; i < 8; i++) {
                i_this->mAAA8[i].m2F8 = (s16)(REG0_S(7) + 60);
                i_this->mAAA8[i].m2F4 = REG0_F(15) + 6000.0f;
            }
            if (i_this->mC74A == 1) {
                actor->speed.y = REG0_F(3) + 220.0f;
                gabi::Local<cXyz> v;
                v->x = 0.0f;
                v->y = 1.0f;
                v->z = 0.0f;
                { dVibration_c* vib = dComIfGp_getVibration(); StartShock(vib, REG0_S(2) + 6, -0x21, v); }
                i_this->mCA9C = REG0_F(18) + 20.0f;
                gabi::Local<cXyz> pos;
                f32 z = i_this->mHeadParts[0].m0D4.z;
                f32 x = i_this->mHeadParts[0].m0D4.x;
                f32 y = i_this->mHeadParts[0].m0D4.y;
                pos->x = x;
                pos->y = y;
                pos->z = z;
                attack_eff_set(i_this, pos, 0);
            } else if (i_this->mC74A == 2) {
                actor->speed.y = REG0_F(3) + 190.0f;
                gabi::Local<cXyz> v;
                v->x = 0.0f;
                v->y = 1.0f;
                v->z = 0.0f;
                { dVibration_c* vib = dComIfGp_getVibration(); StartShock(vib, REG0_S(2) + 4, -0x21, v); }
                i_this->mCA9C = REG0_F(18) + 15.0f;
                gabi::Local<cXyz> pos;
                f32 z = i_this->mHeadParts[0].m0D4.z;
                f32 x = i_this->mHeadParts[0].m0D4.x;
                f32 y = i_this->mHeadParts[0].m0D4.y;
                pos->x = x;
                pos->y = y;
                pos->z = z;
                attack_eff_set(i_this, pos, 1);
            } else if (i_this->mC74A == 3) {
                actor->speed.y = REG0_F(3) + 130.0f;
                gabi::Local<cXyz> v;
                v->x = 0.0f;
                v->y = 1.0f;
                v->z = 0.0f;
                { dVibration_c* vib = dComIfGp_getVibration(); StartShock(vib, REG0_S(2) + 3, -0x21, v); }
                f32 r = REG0_F(18);
                i_this->mC74A = (s16)(i_this->mC74A + 1);
                i_this->mCA9C = r + 10.0f;
                return;
            } else {
                actor->speed.y = 0.0f;
                mDoAud_bgmStart(0x8000005C /* JA_BGM_BGN_TARABA_IN */);
            }
            i_this->mC74A = (s16)(i_this->mC74A + 1);
        }
        break;
    }
    case 5:
        if (i_this->mKSubCount >= (s16)(REG0_S(3) + 150)) {
            cLib_addCalc2(&i_this->mCC88, 1.0f, 1.0f, REG0_F(15) + 0.005f);
        }
        cLib_addCalc2(&i_this->mC774, REG0_F(9) + 250.0f, 1.0f, REG0_F(10) + 50.0f);
        if (i_this->mKSubCount == 280) {
            i_this->mC74A = 10;
            i_this->mC7AC[0] = (s16)(REG0_S(6) + 300);
            i_this->mC7AC[1] = (s16)(REG0_S(7) + 0x96);
            i_this->mCC8C = dComIfGp_particle_set(0x8419 /* ID_AK_SN_KGTT2CHESTPOTA00 */, &actor->current.pos);
        }
        break;
    case 10: {
        cLib_addCalc2(&i_this->mCC88, 1.0f, 1.0f, REG0_F(15) + 0.005f);
        JPABaseEmitter* e = i_this->mCC8C;
        if (e != nullptr) {
            if (i_this->mC7AC[1] != 0) {
                JPABaseEmitter_setGlobalRTMatrix(e, model_getAnmMtx(i_this->mpMorf->getModel(), 2));
            } else {
                JPABaseEmitter_becomeInvalidEmitter(e);
                i_this->mCC8C = nullptr;
            }
        }
        cLib_addCalc2(&i_this->mC728.y, REG0_F(9) + 1000.0f, 0.05f, actor->speed.y);
        cLib_addCalc2(&actor->speed.y, 50.0f, 1.0f, 0.2f);
        if (i_this->mC7AC[0] <= 100) {
            cLib_addCalc0(&i_this->mCC80, 1.0f, 0.02f);
            s16 t = i_this->mC7AC[0];
            if (t == 100) {
                mDoAud_seStart(0x5976 /* JA_SE_CM_BGN_D_TO_T_1 */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                t = i_this->mC7AC[0];
            }
            if (t <= 1) {
                cLib_addCalc2(&i_this->mCC84, 0.22f, 0.1f, 0.0055f);
                if (i_this->mC7AC[0] == 1) {
                    mDoAud_seStart(0x5977 /* JA_SE_CM_BGN_D_TO_T_2 */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                }
                if (i_this->mCC84 > 0.21f) {
                    i_this->m02B5 = 1;
                    fopAc_ac_c* b2 = gabi::at<fopAc_ac_c>(bgn2_g());
                    b2->current.angle.x = i_this->mC314.x;
                    b2->current.angle.y = i_this->mC314.y;
                    b2->current.angle.z = i_this->mC314.z;
                    b2 = gabi::at<fopAc_ac_c>(bgn2_g());
                    b2->shape_angle.x = i_this->mC314.x;
                    b2->shape_angle.y = i_this->mC314.y;
                    b2->shape_angle.z = i_this->mC314.z;
                    b2 = gabi::at<fopAc_ac_c>(bgn2_g());
                    b2->current.pos.copy(i_this->mC308);
                    gabi::store<f32>(bgn2_g() + 0x316C, 1.0f); /* bgn2->m2E7C */
                    s8 mode = i_this->mCSMode;
                    i_this->mKSubCount = 0;
                    i_this->mCSMode = (s8)(mode + 1);
                    gabi::Local<cXyz> local_20;
                    f32 s = REG0_F(4) + 10.0f;
                    local_20->y = s;
                    local_20->x = s;
                    local_20->z = s;
                    dComIfGp_particle_set(0x13 /* ID_AK_JN_SIBOUBAKUEN */, &gabi::at<fopAc_ac_c>(bgn2_g())->current.pos, nullptr, local_20);
                    dComIfGp_particle_set(0x16 /* ID_AK_JN_SIBOUFLASH */, &gabi::at<fopAc_ac_c>(bgn2_g())->current.pos, nullptr, local_20);
                    mDoAud_seStart(0x5980 /* JA_SE_CM_BGN_METAM_EXPLODE */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                    gabi::Local<cXyz> v;
                    v->y = 1.0f;
                    v->z = 0.0f;
                    v->x = 0.0f;
                    { dVibration_c* vib = dComIfGp_getVibration(); StartShock(vib, REG0_S(2) + 8, -0x21, v); }
                } else {
                    cLib_addCalcAngleS2(&i_this->mCA60, 0xB4, 1, 4);
                }
            }
        }
        break;
    }
    }
}
VERIFY(0x02084D6C, hensin);

/* 0207FE14 HD: the reflection packets of the head, chest, room and the HD-only packets are set up */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0207FE14, BOOL, a_this);
    bgn_class* i_this = (bgn_class*)a_this;
    J3DModelData* md = (J3DModelData*)dComIfG_getObjectRes(BGN_ARC, 0x15 /* BDL_BGN_HEAD1 */, BGN_SAFESTRING_VTBL);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(BGN_ARC, 0x6 /* BCK_BGN_HEAD1 */, BGN_SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, md, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                            nullptr, 0, 0x11020203);
    if (i_this->mpMorf == nullptr || i_this->mpMorf->mpModel.v == 0)
        return FALSE;
    i_this->mHeadParts[0].mpPartModel = i_this->mpMorf->getModel();
    mDoExt_J3DModelPacketS_setup(&i_this->mHeadParts[0].m004, 0);
    i_this->mHeadParts[0].m0D2 = 16;
    md = (J3DModelData*)dComIfG_getObjectRes(BGN_ARC, 0x20 /* BDL_BGN_MAIN1 */, BGN_SAFESTRING_VTBL);
    i_this->mpChestModel = mDoExt_J3DModel__create(md, 0, 0x11020203);
    mDoExt_J3DModelPacketS_setup(&i_this->m02C0, 0);
    if (i_this->mpChestModel == nullptr)
        return FALSE;
    J3DModelData* bodyModelData = (J3DModelData*)dComIfG_getObjectRes(BGN_ARC, 0x13 /* BDL_BGN_BODY1 */, BGN_SAFESTRING_VTBL);
    if (!part_init(&i_this->mPelvisParts[0], bodyModelData))
        return FALSE;
    i_this->mPelvisParts[0].m0D2 = 10;
    for (s32 i = 0; i < 20; i++) {
        if (!part_init(&i_this->mLeftArmParts[i], bodyModelData))
            return FALSE;
        if (!part_init(&i_this->mRightArmParts[i], bodyModelData))
            return FALSE;
        i_this->mRightArmParts[i].m0D2 = (s8)(i * 3 + 13);
        i_this->mLeftArmParts[i].m0D2 = (s8)(i * 3 + 13);
    }
    for (s32 i = 0; i < 3; i++) {
        if (!part_init(&i_this->mLeftLegParts[i], bodyModelData))
            return FALSE;
        if (!part_init(&i_this->mRightLegParts[i], bodyModelData))
            return FALSE;
        i_this->mRightLegParts[i].m0D2 = (s8)(i * 3 + 10);
        i_this->mLeftLegParts[i].m0D2 = (s8)(i * 3 + 10);
    }
    for (s32 i = 0; i < 20; i++) {
        if (!part_init(&i_this->mTailParts[i], bodyModelData))
            return FALSE;
        i_this->mTailParts[i].m0D2 = (s8)(11 - i);
    }
    md = (J3DModelData*)dComIfG_getObjectRes(BGN_ARC, 0x16 /* BDL_BGN_JYAKUTENA */, BGN_SAFESTRING_VTBL);
    i_this->mpJyakutenAModel = mDoExt_J3DModel__create(md, 0, 0x11020203);
    if (i_this->mpJyakutenAModel == nullptr)
        return FALSE;
    md = (J3DModelData*)dComIfG_getObjectRes(BGN_ARC, 0x19 /* BDL_BGN_JYAKUTENB */, BGN_SAFESTRING_VTBL);
    i_this->mpJyakutenBModel = mDoExt_J3DModel__create(md, 0, 0x11020203);
    if (i_this->mpJyakutenBModel == nullptr)
        return FALSE;
    {
        mDoExt_brkAnm* p = (mDoExt_brkAnm*)operator_new(0x78);
        if (p != nullptr)
            p = mDoExt_brkAnm_ct(p);
        i_this->mJyakutenBBrkAnm = p;
    }
    if (i_this->mJyakutenBBrkAnm == nullptr)
        return FALSE;
    void* pBrk = dComIfG_getObjectRes(BGN_ARC, 0x25 /* BRK_BGN_JYAKUTENB */, BGN_SAFESTRING_VTBL);
    if (!mDoExt_brkAnm_init(i_this->mJyakutenBBrkAnm, md, pBrk, 1, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0, 0))
        return FALSE;
    md = (J3DModelData*)dComIfG_getObjectRes(BGN_ARC, 0x1C /* BDL_BGN_JYAKUTENC */, BGN_SAFESTRING_VTBL);
    i_this->mpJyakutenCModel = mDoExt_J3DModel__create(md, 0, 0x11020203);
    if (i_this->mpJyakutenCModel == nullptr)
        return FALSE;
    {
        mDoExt_brkAnm* p = (mDoExt_brkAnm*)operator_new(0x78);
        if (p != nullptr)
            p = mDoExt_brkAnm_ct(p);
        i_this->mJyakutenCBrkAnm = p;
    }
    if (i_this->mJyakutenCBrkAnm == nullptr)
        return FALSE;
    pBrk = dComIfG_getObjectRes(BGN_ARC, 0x28 /* BRK_BGN_JYAKUTENC */, BGN_SAFESTRING_VTBL);
    if (!mDoExt_brkAnm_init(i_this->mJyakutenCBrkAnm, md, pBrk, 1, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0, 0))
        return FALSE;
    void* pBti = dComIfG_getObjectRes(BGN_ARC, 0x2E /* BTI_NOT_CUT1 */, BGN_SAFESTRING_VTBL);
    if (!mDoExt_3DlineMat1_init(&i_this->mRedRopeMat, 1, 60, pBti, 1))
        return FALSE;
    pBti = dComIfG_getObjectRes(BGN_ARC, 0x2D /* BTI_HIMO */, BGN_SAFESTRING_VTBL);
    if (!mDoExt_3DlineMat1_init(&i_this->mBlueRopeMat, 8, 60, pBti, 1))
        return FALSE;
    pBti = dComIfG_getObjectRes(BGN_ARC, 0x2E /* BTI_NOT_CUT1 */, BGN_SAFESTRING_VTBL);
    if (!mDoExt_3DlineMat1_init(&i_this->mDefeatCSRopeMat, 1, 60, pBti, 1))
        return FALSE;
    md = (J3DModelData*)dComIfG_getObjectRes(BGN_ARC, 0x22 /* BDL_R0E_A */, BGN_SAFESTRING_VTBL);
    if (md == nullptr)
        JUT_ASSERT_fail(STR(0x10008C50) /* "d_a_bgn.cpp" */, 0x1E43, STR(0x10008C5C) /* "modelData != (0)" */);
    i_this->mpWater0Model = mDoExt_J3DModel__create(md, 0x80000, 0x31000000);
    if (i_this->mpWater0Model == nullptr)
        return FALSE;
    i_this->mpWater1Model = mDoExt_J3DModel__create(md, 0x80000, 0x30000000);
    if (i_this->mpWater1Model == nullptr)
        return FALSE;
    md = (J3DModelData*)dComIfG_getObjectRes(BGN_ARC, 0x21 /* BDL_R00 */, BGN_SAFESTRING_VTBL);
    i_this->mpRoomReflectionModel = mDoExt_J3DModel__create(md, 0x80000, 0x11000022);
    mDoExt_J3DModelPacketS_setup(&i_this->mCB60, 0);
    if (md == nullptr)
        JUT_ASSERT_fail(STR(0x10008C50) /* "d_a_bgn.cpp" */, 0x1E5D, STR(0x10008C5C) /* "modelData != (0)" */);
    if (i_this->mpRoomReflectionModel == nullptr)
        return FALSE;
    mDoExt_J3DModelPacketS_setup(&i_this->mCC24, 0);
    mDoExt_J3DModelPacketS_setup(&i_this->mHdPacket[0], 0);
    mDoExt_J3DModelPacketS_setup(&i_this->mHdPacket[1], 0);
    mDoExt_J3DModelPacketS_setup(&i_this->mHdPacket[2], 0);
    return TRUE;
}
VERIFY(0x0207FE14, useHeapInit);

/* 02080498 part_s::part_s (allocates when this == NULL) */
static part_s* part_s_ct(part_s* p) {
    WWHD_FUNC(0x02080498, part_s*, p);
    if (p == nullptr) {
        p = (part_s*)operator_new(0x3F0);
        if (p == nullptr)
            return p;
    }
    mDoExt_J3DModelPacketS_ct(&p->m004);
    tevstr_tmpl t;
    t.load();
    t.store(gabi::ea(&p->mPartTevStr));
    dCcD_Sph_ct(&p->mPartSph);
    return p;
}
VERIFY(0x02080498, part_s_ct);

/* 0208064C bgn_class::bgn_class (inline constructor of fopAcM_ct; allocates when this == NULL) */
static bgn_class* bgn_class_ct(bgn_class* p) {
    WWHD_FUNC(0x0208064C, bgn_class*, p);
    if (p == nullptr) {
        p = (bgn_class*)operator_new(0x15768);
        if (p == nullptr)
            return p;
    }
    fopAc_ac_c_ct(p);
    p->m3C8 = gabi::ea(p);
    p->__vtbl = 0x10008AD4;
    mDoExt_J3DModelPacketS_ct(&p->m02C0);
    __construct_array(p->mHeadParts, 2, 0x3F0, 0x02080498);
    __construct_array(p->mPelvisParts, 2, 0x3F0, 0x02080498);
    __construct_array(p->mLeftArmParts, 21, 0x3F0, 0x02080498);
    __construct_array(p->mRightArmParts, 21, 0x3F0, 0x02080498);
    __construct_array(p->mLeftLegParts, 4, 0x3F0, 0x02080498);
    __construct_array(p->mRightLegParts, 4, 0x3F0, 0x02080498);
    __construct_array(p->mTailParts, 21, 0x3F0, 0x02080498);
    mDoExt_3DlineMat1_ct(&p->mBlueRopeMat);
    mDoExt_3DlineMat1_ct(&p->mRedRopeMat);
    mDoExt_3DlineMat1_ct(&p->mDefeatCSRopeMat);
    mDoExt_J3DModelPacketS_ct(&p->mHdPacket[0]);
    mDoExt_J3DModelPacketS_ct(&p->mHdPacket[1]);
    mDoExt_J3DModelPacketS_ct(&p->mHdPacket[2]);
    dCcD_Stts_ct(&p->mStts);
    dCcD_Sph_ct(&p->mC7FC);
    dCcD_Sph_ct(&p->mCoreSph);
    tevstr_tmpl t;
    t.load();
    t.store(gabi::ea(&p->mWaterTevStr));
    mDoExt_J3DModelPacketS_ct(&p->mCB60);
    t.store(gabi::ea(&p->mRoomTevStr));
    mDoExt_J3DModelPacketS_ct(&p->mCC24);
    __construct_array(p->mPunchSmokeCb, 2, 0x20, 0x0207A9C0);
    return p;
}
VERIFY(0x0208064C, bgn_class_ct);

/* 0208ACD8 bgn_class::~bgn_class (deleting destructor) */
static void bgn_class_dt(bgn_class* p, s32 flags) {
    WWHD_FUNC(0x0208ACD8, void, p, flags);
    if (p == nullptr)
        return;
    __destroy_arr(p->mPunchSmokeCb, 2, 0x20, 0x0208AC64, 0, 0);
    mDoExt_J3DModelPacketS_dt(&p->mCC24, 2);
    mDoExt_J3DModelPacketS_dt(&p->mCB60, 2);
    dCcD_Sph_dt(&p->mCoreSph, 2);
    dCcD_Sph_dt(&p->mC7FC, 2);
    dCcD_Stts_dt(&p->mStts, 2);
    mDoExt_J3DModelPacketS_dt(&p->mHdPacket[2], 2);
    mDoExt_J3DModelPacketS_dt(&p->mHdPacket[1], 2);
    mDoExt_J3DModelPacketS_dt(&p->mHdPacket[0], 2);
    mDoExt_3DlineMat1_dt(&p->mDefeatCSRopeMat, 2);
    mDoExt_3DlineMat1_dt(&p->mRedRopeMat, 2);
    mDoExt_3DlineMat1_dt(&p->mBlueRopeMat, 2);
    __destroy_arr(p->mTailParts, 21, 0x3F0, 0x0208AC78, 0, 0);
    __destroy_arr(p->mRightLegParts, 4, 0x3F0, 0x0208AC78, 0, 0);
    __destroy_arr(p->mLeftLegParts, 4, 0x3F0, 0x0208AC78, 0, 0);
    __destroy_arr(p->mRightArmParts, 21, 0x3F0, 0x0208AC78, 0, 0);
    __destroy_arr(p->mLeftArmParts, 21, 0x3F0, 0x0208AC78, 0, 0);
    __destroy_arr(p->mPelvisParts, 2, 0x3F0, 0x0208AC78, 0, 0);
    __destroy_arr(p->mHeadParts, 2, 0x3F0, 0x0208AC78, 0, 0);
    mDoExt_J3DModelPacketS_dt(&p->m02C0, 2);
    gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1)
        operator_delete(p);
}
VERIFY(0x0208ACD8, bgn_class_dt);

/* inline sead::SafeString::isEqual as GHS expands it in the HD packet draw: both strings are made
 * terminated (the first directly, then both through the vtable), then compared byte by byte (at
 * most 0x40001 characters; running out counts as different) */
static bool bgn_safestring_equal(u32 lit, u32 name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    a->__vtbl = BGN_SAFESTRING_VTBL;
    a->mStringTop = lit;
    b->__vtbl = BGN_SAFESTRING_VTBL;
    b->mStringTop = name;
    SafeString_assureTermination(a);
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    u32 p = a->mStringTop;
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
/* the material packet of a mesh (HD J3DModel tables at +0x128..+0x134, sead::Buffer semantics) */
static u32 bgn_matPacket(u32 model, u32 mesh) {
    u32 shape0 = gabi::load<u32>(gabi::load<u32>(mesh + 8));
    u16 idx = gabi::load<u16>(shape0 + 0xC);
    u32 n130 = gabi::load<u32>(model + 0x130);
    u32 p = gabi::load<u32>(model + 0x134);
    if (idx < n130)
        p += idx * 2;
    u16 idx2 = gabi::load<u16>(p);
    u32 n128 = gabi::load<u32>(model + 0x128);
    u32 mp = gabi::load<u32>(model + 0x12C);
    if (idx2 < n128)
        mp += idx2 * 0xAC;
    return mp;
}
static u32 bgn_mesh_name(u32 mesh) {
    u32 h = gabi::load<u32>(mesh);
    s32 off = gabi::load<s32>(h + 4);
    return off != 0 ? h + 4 + off : 0;
}

/* 02082ED0 mDoExt_J3DModelPacketS::draw (HD, virtual slot 0x2C): the model's visible meshes are drawn
 * with the mirror state; HD skips the ear meshes "ear_3_"/"ear_8_" while a save flag (+0x1C0) is set
 * and draws the hidden eye/eyebrow meshes ("eyeL", "eyeR", "mayuL", "mayuR") when mAD is set */
static void mDoExt_J3DModelPacketS_draw(mDoExt_J3DModelPacketS_l* p, u32 ctx) {
    WWHD_FUNC(0x02082ED0, void, p, ctx);
    if (gabi::load<u32>(ctx + 0xC) != 2)
        return;
    gabi::Local<u8[0x11C]> st;
    gabi::call(0x02750250, st.get());
    mDoExt_J3DModelPacketS_setMaterial(p, st.a);
    gabi::call(0x02750370, st.get());
    if (p->mpHelper != 0) {
        gabi::Local<be<u32>[13]> info;
        u32 i0 = gabi::load<u32>(ctx);
        u32 helper = p->mpHelper;
        u32 model = gabi::ea(p->mpModel.get());
        for (int i = 0; i < 13; i++) (*info)[i] = 0;
        (*info)[5] = helper;
        (*info)[3] = 2;
        (*info)[0] = i0;
        (*info)[1] = 2;
        if (model != 0) {
            u32 md = gabi::load<u32>(model + 0xAC);
            for (u16 j = 0; j < gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, md)) + 8); j = (u16)(j + 1)) {
                u32 node = gabi::load<u32>(md + 8);
                if (j < gabi::load<u32>(md + 4))
                    node += j * 0x1C;
                for (u32 mesh = gabi::load<u32>(node + 0x10); mesh != 0; mesh = gabi::load<u32>(mesh + 4)) {
                    u32 shape = gabi::load<u32>(mesh + 8);
                    if (gabi::load<u8>(shape + 4) != 0) {
                        u32 mp = bgn_matPacket(gabi::ea(p->mpModel.get()), mesh);
                        bool skip = false;
                        if (p->mAD != 0 && gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1C0) != 0) {
                            if (bgn_safestring_equal(0x10008CB4 /* "ear_3_" */, bgn_mesh_name(mesh)) ||
                                bgn_safestring_equal(0x10008CBC /* "ear_8_" */, bgn_mesh_name(mesh)))
                                skip = true;
                        }
                        if (!skip)
                            gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(mp + 0xC) + 0x2C), mp, info.a);
                    } else if (p->mAD != 0) {
                        if (bgn_safestring_equal(0x10008CC4 /* "eyeL" */, bgn_mesh_name(mesh)) ||
                            bgn_safestring_equal(0x10008CCC /* "eyeR" */, bgn_mesh_name(mesh)) ||
                            bgn_safestring_equal(0x10008CA4 /* "mayuL" */, bgn_mesh_name(mesh)) ||
                            bgn_safestring_equal(0x10008CAC /* "mayuR" */, bgn_mesh_name(mesh))) {
                            u32 mp = bgn_matPacket(gabi::ea(p->mpModel.get()), mesh);
                            gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(mp + 0xC) + 0x2C), mp, info.a);
                        }
                    }
                }
            }
        }
        if (p->m9C != 0)
            gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(p->m9C + 0xC) + 0x2C), (u32)p->m9C, info.a);
        if (p->mA0 != 0)
            gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(p->mA0 + 0x130) + 0x2C), (u32)p->mA0, info.a);
    }
    gabi::call(0x02750370, 0x104B474Cu);
}
VERIFY(0x02082ED0, mDoExt_J3DModelPacketS_draw);
