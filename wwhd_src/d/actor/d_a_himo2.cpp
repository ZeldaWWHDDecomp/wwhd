/**
 * d_a_himo2.cpp (WWHD)
 * Item - Grappling Hook / Rope
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_himo2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_himo2.h"

const gc_tu_data gc_tu = {
    .safeString = HIMO2_SAFESTRING_VTBL,
    .objAcch = {0x10010E74, 0x10010E94, 0x10010E84},
    .objAcchDt = {0x10010E84, 0x10010E94},
    .gndChk = {0x10010E34, 0x10010E44, 0x10010E64, 0x10010E54},
    .gndChkDt = {0x10010E44, 0x10010E64, 0x10010E24},
    .headerStatics = {0x10464680, 0x101B6E58},
};

static himo2HIO_c l_himo2HIO;
static f32 rope_scale;
static himo2_btd_c* himo2_btd;
static himo2_dr_c* himo2_dr;
static himo2_reg_HIO_c l_himo2Regs;
static himo2_guideEmitter_c* l_guideEmitter;
static u8 rope_color[4] = {200, 150, 50, 255};
static u32 cc_sph_src[16] = {
    0x00000000, 0x08000000, 0x01000000, 0x0000000B, 0x00000000, 0x00000000, 0x00000000, 0x05010000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x41A00000,
};

enum {
    fpcNm_KUI_e = 0xFA,
    fpcNm_DR2_e = 0xDF,
    fpcNm_BTD_e = 0xEA,
    dBgS_Attr_WOOD_e = 2,
    dBgS_Attr_STONE_e = 3,
    JA_SE_LK_SW_HIT_S = 0x2803,
    JA_SE_LK_MS_WEP_HIT = 0x2834,
    dPa_name_ID_AK_JN_ELEMENTKIKUZU00 = 0x2B,
    dPa_name_ID_AK_JN_ELEMENTHIBANA00 = 0x2C,
    dRes_INDEX_ALWAYS_BTI_ROPE_e = 0x7E,
    dRes_INDEX_LINK_BDL_ROPEEND_e = 0x2E,
};

// HD: the impact check uses this translation unit's polygon-info vtable.
static inline void himo2_initPolyInfo(cBgS_PolyInfo* info) {
    info->mPolyIndex = 0xFFFF;
    info->mpBgW = 0;
    info->mActorId = (u32)-1;
    info->__vtbl = 0x10010E04;
    info->mBgIndex = 0x100;
}
static inline bool SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::at<himo2_safeStringVtable_c>(a->__vtbl)->assureTermination, a);
    gabi::call_ptr(gabi::at<himo2_safeStringVtable_c>(a->__vtbl)->assureTermination, a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::at<himo2_safeStringVtable_c>(b->__vtbl)->assureTermination, b);
    if (s1 == b->mStringTop)
        return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(p + i);
        if (c != gabi::load<u8>(q + i))
            return false;
        if (c == 0)
            return true;
    }
    return false;
}

/* 0216DD3C */
static void* s_a_d_sub(void* param_1, void* param_2) {
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_KUI_e) {
        himo2_class* rope = (himo2_class*)param_2;
        /* HD: bounded (JUT_ASSERT(0x34D, m24AC < 100)) */
        if (rope->m24AC >= 100) {
            JUT_ASSERT_fail("d_a_himo2.cpp", 0x34D, "i_this->m24AC < 100");
            if (rope->m24AC >= 100)
                return nullptr;
        }
        rope->m218C[rope->m24AC] = (fopAc_ac_c*)param_1;
        rope->m24AC = rope->m24AC + 1;
    }
    return nullptr;
}

/* 0216DDDC */
static void* dr_a_sub(void* param_1, void* param_2) {
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_DR2_e)
        return param_1;
    return nullptr;
}

/* 0216DE2C */
static void* b_a_sub(void* param_1, void* param_2) {
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_BTD_e)
        return param_1;
    return nullptr;
}

/* 0216DE7C */
static BOOL himo2_bg_check(himo2_class* i_this) {
    if (i_this->m02A2 != 0)
        return FALSE;

    i_this->m2574.CrrPos(dComIfG_Bgsp());
    if (!(i_this->m2574.m_flags & 0x230)) /* ChkWallHit() || ChkRoofHit() || ChkGroundHit() */
        return FALSE;

    i_this->m02DC = HIMO2_REBOUND;
    i_this->m0308 = 30;
    i_this->speedF = -i_this->speedF;
    gabi::Local<cBgS_PolyInfo> local_24;
    himo2_initPolyInfo(local_24.get());
    if (himo2_GetOnePolyInfo(&i_this->m2574, local_24) != 0) /* JUT_ASSERT(0x446, flag == NULL) */
        JUT_ASSERT_fail("d_a_himo2.cpp", 0x446, "flag == 0");
    u32 uVar3 = dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo*)local_24.get());
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
    mDoAud_seStart(JA_SE_LK_SW_HIT_S, &i_this->current.pos, uVar3, reverb);
    s32 attrib_code = himo2_GetAttributeCode(dComIfG_Bgsp(), local_24);
    if (attrib_code == dBgS_Attr_WOOD_e || attrib_code == dBgS_Attr_STONE_e) {
        gabi::Local<csXyz> local_38;
        local_38->x = i_this->current.angle.x;
        s16 y = i_this->current.angle.y;
        local_38->y = y;
        s16 z = i_this->current.angle.z;
        local_38->y = (s16)(y + 0x8000);
        local_38->z = z;
        /* fopAcM_seStart (HD: checks &eyePos != NULL) */
        if (gabi::ea(i_this) + 0x37C != 0)
            mDoAud_seStart(JA_SE_LK_MS_WEP_HIT, &i_this->eyePos, 0,
                           dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
        himo2_particle_c* emitter;
        if (attrib_code == dBgS_Attr_WOOD_e) {
            dPa_control_c* pa = dComIfGp_getParticle();
            u32 k0 = gabi::ea(i_this) + 0x1A8; /* &tevStr.mColorK0 */
            emitter = (himo2_particle_c*)dPa_control_set(
                pa, 0, dPa_name_ID_AK_JN_ELEMENTKIKUZU00, &i_this->current.pos, local_38, nullptr,
                0xFF, nullptr, -1, gabi::at<GXColor>(k0), gabi::at<GXColor>(k0), nullptr);
            if (emitter != 0) {
                emitter->mSpread = 0.2f;
                emitter->mVolumeSweep = 0.15f;
            }
        } else {
            local_38->x = (s16)(local_38->x + 0x4000);
            dPa_control_c* pa = dComIfGp_getParticle();
            emitter = (himo2_particle_c*)dPa_control_set(
                pa, 0, dPa_name_ID_AK_JN_ELEMENTHIBANA00, &i_this->current.pos, local_38, nullptr,
                0xFF, nullptr, -1, nullptr, nullptr, nullptr);
            if (emitter != 0)
                emitter->mInitialVelAxis = 15.0f;
            gabi::Local<cXyz> pos;
            pos->x = i_this->current.pos.x; /* cXyz passed by value: an FPR copy */
            pos->y = i_this->current.pos.y;
            pos->z = i_this->current.pos.z;
            u32 id = fopAcM_GetID(i_this); /* fopAcM_GetID */
            himo2_SoundSet(pos, 100, id, 5);
        }
        if (emitter != 0) {
            emitter->mRate = 8.0f;
            emitter->mMaxFrame = 1;
        }
    }
    return TRUE;
}

/* 0216E144 */
static void pl_pos_add(himo2_class* i_this) {
    daPy_py_c* player = (daPy_py_c*)dComIfGp_getPlayer(0);
    gabi::Local<cXyz> local_28;
    *local_28.get() = player->old.pos - player->current.pos; /* cXyz::operator- */
    PSVECAdd(&i_this->current.pos, local_28.get(),
             &i_this->current.pos); /* PSVECAdd (operator+=) */
}

/* 0216E9A8 */
static BOOL daHimo2_IsDelete(himo2_class* i_this) { return TRUE; }

/* 0216E9B0 */
static BOOL daHimo2_Delete(himo2_class* i_this) {
    mDoHIO_deleteChild(l_himo2HIO.mNo);
    return TRUE;
}

/* 0216E9E0 */
static BOOL CallbackCreateHeap(fopAc_ac_c* i_this) {
    himo2_class* a_this = (himo2_class*)i_this;
    const char* always = "Always"; /* "Always" */
    void* img = dComIfG_getObjectRes(always, dRes_INDEX_ALWAYS_BTI_ROPE_e, HIMO2_SAFESTRING_VTBL);
    if (!((mDoExt_3DlineMat1_c*)&a_this->m1F30)->init(1, 200, (ResTIMG*)img, 0))
        return FALSE;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(
        "Link" /* "Link" */, dRes_INDEX_LINK_BDL_ROPEEND_e, HIMO2_SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0xAC1, modelData != NULL) */
        JUT_ASSERT_fail("d_a_himo2.cpp", 0xAC1, "modelData != (0)");
    a_this->mpHookModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203U);
    if (!a_this->mpHookModel)
        return FALSE;
    img = dComIfG_getObjectRes(always, dRes_INDEX_ALWAYS_BTI_ROPE_e, HIMO2_SAFESTRING_VTBL);
    if (!((mDoExt_3DlineMat1_c*)&a_this->m1F98)->init(5, 0x20, (ResTIMG*)img, 0))
        return FALSE;
    img = dComIfG_getObjectRes(always, dRes_INDEX_ALWAYS_BTI_ROPE_e, HIMO2_SAFESTRING_VTBL);
    if (!((mDoExt_3DlineMat1_c*)&a_this->m1FD8)->init(1, 0x10, (ResTIMG*)img, 0))
        return FALSE;
    /* HD: in one stage (0x10011014) the hook's packets get an extra setup */
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::guest_addr("GanonK");
    a->__vtbl = HIMO2_SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mStringTop =
        gabi::guest_addr(dComIfGp_getStartStageName()); /* dComIfGp_getStartStageName() */
    b->__vtbl = HIMO2_SAFESTRING_VTBL;
    if (SafeString_eq(a.get(), b.get())) {
        for (int i = 0; i < 4; i++)
            himo2_setupPacket(&a_this->mPackets[i], 0);
    }
    return TRUE;
}

/* 0216EC18 */
static cPhs_State daHimo2_Create(fopAc_ac_c* i_this) {
    himo2_class* a_this = (himo2_class*)i_this;
    daPy_py_c* player = (daPy_py_c*)dComIfGp_getPlayer(0);

    /* fopAcM_ct(i_this, himo2_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = HIMO2_VTBL;
            __ct((mDoExt_3DlineMat1_c*)&a_this->m1F30);
            __ct((mDoExt_3DlineMat1_c*)&a_this->m1F98);
            __ct((mDoExt_3DlineMat1_c*)&a_this->m1FD8);
            dCcD_Stts_ct(&a_this->m2014);
            himo2_constructSphere(&a_this->m2050); /* dCcD_Sph::dCcD_Sph */
            for (int i = 0; i < 4; i++)
                himo2_constructPacket(&a_this->mPackets[i]);
            dBgS_AcchCir_ct(&a_this->m2534);
            __ct(&a_this->m2574);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    if (!fopAcM_entrySolidHeap(a_this, 0x0216E9E0 /* CallbackCreateHeap */,
                               (s16)l_himo2Regs.mChild[0].mShortRegs[9] + 0x9050))
        return cPhs_ERROR_e;

    /* mpHookModel->setBaseScale(cXyz(1.0f, 1.0f, 1.0f)) */
    J3DModel* model = a_this->mpHookModel;
    model->mBaseScale.x = 1.0f;
    model->mBaseScale.y = 1.0f;
    model->mBaseScale.z = 1.0f;
    a_this->m02CC = (s16)l_himo2Regs.mChild[0].mShortRegs[1] + 96;
    /* player->getLeftHandPos() (HD +0x3F0) */
    f32 y = (f32)player->mLeftHandPos.y;
    f32 x = (f32)player->mLeftHandPos.x;
    f32 z = (f32)player->mLeftHandPos.z;
    a_this->current.pos.x = x;
    a_this->current.pos.y = y;
    a_this->current.pos.z = z;
    a_this->m2574.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &a_this->m2534,
                      &a_this->speed, nullptr, nullptr);
    a_this->m2534.SetWall(30.0f, 30.0f);
    a_this->m2574.m_flags &= ~(u32)dBgS_Acch::ROOF_NONE;
    f32 h = (f32)l_himo2Regs.mChild[0].mFloatRegs[7] + 60.0f;
    a_this->m2500 = 1.0f;
    a_this->m2574.m_roof_crr_height = h;
    l_himo2HIO.mNo = mDoHIO_createChild("Rope" /* "Rope" */, &l_himo2HIO);
    rope_scale = 2.0f;
    a_this->m2014.Init(0xFF, 0xFF, a_this);
    a_this->m2050.Set((dCcD_SrcSph*)cc_sph_src);
    a_this->m2050.SetStts(&a_this->m2014);
    himo2_dr = 0;
    himo2_btd = 0;
    return cPhs_COMPLEATE_e;
}

/* search_target (GameCube 800ECC54) is inlined into setTargetPos in HD. HD: the candidate list
 * holds 100 actors; in "ADMumi" the angular window is 10000 wider and the height tolerance is
 * extended by REG(0x5A8) - 600; a REG0_S (0x62A) widens the window. */
static inline bool himo2_isStartStage(const char* stage) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::guest_addr(stage);
    a->__vtbl = HIMO2_SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mStringTop = gabi::guest_addr(dComIfGp_getStartStageName());
    b->__vtbl = HIMO2_SAFESTRING_VTBL;
    return SafeString_eq(a.get(), b.get());
}

static inline fopAc_ac_c* search_target(himo2_class* i_this, f32 px, f32 py, f32 pz) {
    daPy_py_c* player = (daPy_py_c*)dComIfGp_getPlayer(0);
    i_this->m24AC = 0;
    for (int i = 0; i < 100; i++)
        i_this->m218C[i] = nullptr;
    fpcM_Search(0x0216DD3C /* s_a_d_sub */, i_this);
    mDoMtx_YrotS(calc_mtx(), (s16)-player->shape_angle.y);
    f32 f24 = 100.0f;
    f32 f31 = 100.0f; /* the distance limit, GameCube f31 */
    s16 m08 = l_himo2HIO.m08;
    s16 reg = (s16)l_himo2Regs.mChild[10].mShortRegs[5];
    s16 r27 = (s16)(0x4000 - m08 - reg);
    s16 r26 = (s16)(m08 + reg + 0x4000);
    f32 yTol = 0.0f;
    u32 n;
    if (himo2_isStartStage("ADMumi")) {
        r26 = (s16)(r26 - 10000);
        r27 = (s16)(r27 + 10000);
        yTol = (f32)l_himo2Regs.mChild[10].mFloatRegs[0] + -600.0f;
    }
    n = i_this->m24AC;
    if (n == 0)
        return nullptr;
    s32 r25 = 0;
    if (r25 >= (s32)n)
        return nullptr;
    for (;;) {
        bool r24 = false;
        if (r25 < 100) {
            fopAc_ac_c* r28 = i_this->m218C[r25];
            if ((fopAcM_GetParam(r28) & 0xF0) != 0)
                r24 = true;
            gabi::Local<cXyz> sp3C;
            sp3C->x = player->current.pos.x - r28->current.pos.x;
            sp3C->z = player->current.pos.z - r28->current.pos.z;
            s16 at = cM_atan2s(sp3C->x, sp3C->z);
            s16 r3 = (s16)(r28->current.angle.y - at);
            bool cand;
            if (r3 < 0 && !r24) {
                r3 = (s16)-r3;
                i_this->m251C = 1;
                cand = r3 > r27 && r3 < r26;
            } else {
                i_this->m251C = 0;
                cand = r24 || (r3 > r27 && r3 < r26);
            }
            if (cand) {
                sp3C->x = r28->current.pos.x - player->current.pos.x;
                sp3C->y = r28->current.pos.y - player->current.pos.y;
                sp3C->z = r28->current.pos.z - player->current.pos.z;
                gabi::Local<cXyz> sp30;
                MtxPosition(sp3C, sp30);
                f32 z = sp30->z;
                if (z > f24 && (r24 || std::fabs((f32)sp30->y) < l_himo2HIO.m14 + yTol)) {
                    f32 zz = z * z;
                    f32 x = sp30->x;
                    f32 f4 = std_sqrtf(gabi::fmadds(x, x, zz));
                    if (f4 < f31) {
                        himo2_camera_c* camera =
                            (himo2_camera_c*)dComIfGp_getCamera(0); /* dComIfGp_getCamera(0) */
                        f32 ez = (f32)camera->eye.z;
                        f32 ex = (f32)camera->eye.x;
                        f32 s18z = r28->current.pos.z - ez;
                        f32 s24x = px - ex;
                        f32 s24z = pz - ez;
                        f32 s18x = r28->current.pos.x - ex;
                        f32 ey = (f32)camera->eye.y;
                        f32 s18y = r28->current.pos.y - ey;
                        f32 s24y = py - ey;
                        s16 r21 = cM_atan2s(s24x, s24z);
                        r21 = (s16)(cM_atan2s(s18x, s18z) - r21);
                        if (r21 < 0)
                            r21 = (s16)-r21;
                        f32 s18x2 = s18x * s18x;
                        f32 s18z2 = s18z * s18z;
                        f32 f2 = std_sqrtf(gabi::fmadds(s18y, s18y, s18x2) + s18z2);
                        s16 r4 = cM_atan2s(r28->scale.z * l_himo2HIO.m10, f2);
                        if (r4 < 0)
                            r4 = (s16)-r4;
                        if (r21 > (s16)-r4 && r21 < r4) {
                            f2 = std_sqrtf(gabi::fmadds(s24x, s24x, s24z * s24z));
                            s16 a21 = (s16)-cM_atan2s(s24y, f2);
                            f2 = std_sqrtf(s18x2 + s18z2);
                            s16 a3 = (s16)-cM_atan2s(s18y, f2);
                            s32 lim = r24 ? 2000 : (s32)l_himo2HIO.m0C;
                            if (a21 < a3 + l_himo2HIO.m0E && (s16)(a3 - a21) < lim)
                                return r28;
                        }
                    }
                }
            }
        }
        r25++;
        n = i_this->m24AC;
        if (r25 > (s32)n)
            return nullptr;
        if (r25 == (s32)n) {
            f31 = f31 + f24;
            r25 = 0;
            if (r24) {
                if (f31 > 2000.0f)
                    return nullptr;
            } else {
                if (f31 > l_himo2HIO.m18)
                    return nullptr;
            }
            if (!(r25 < (s32)n))
                return nullptr;
        }
    }
}

// himo2_class::setTargetPos (matcher: search_target, which it inlines)
/* 0216EEB8 */
BOOL himo2_class::setTargetPos(cXyz* param_1, be<f32>* param_2, be<f32>* param_3) {
    *param_2 = -1.0f;
    *param_3 = -1.0f;
    /* m2524 = *param_1 (word copy), then read back as the by-value argument of search_target */
    himo2_xyzWords_c* destination = (himo2_xyzWords_c*)&this->m2524;
    himo2_xyzWords_c* source = (himo2_xyzWords_c*)param_1;
    destination->x = source->x;
    destination->y = source->y;
    f32 px = this->m2524.x;
    destination->z = source->z;
    f32 py = this->m2524.y;
    f32 pz = this->m2524.z;
    fopAc_ac_c* t = search_target(this, px, py, pz);
    this->m217C = t;
    if (t == nullptr)
        return FALSE;
    if ((fopAcM_GetParam(t) & 0xF0) != 0) {
        *param_2 = 700.0f;
        *param_3 = 100.0f;
    } else {
        *param_2 = (f32)((fopAcM_GetParam(t) >> 0x10) & 0x0F) * 100.0f;
        *param_3 = (f32)((fopAcM_GetParam(this->m217C) >> 0x14) & 0x0F) * 100.0f;
        if (!(*param_2 < 1500.0f))
            *param_2 = -1.0f;
        if (!(*param_3 < 1500.0f))
            *param_3 = -1.0f;
    }
    return TRUE;
}

/* ---- Execute ---- */

/* daPy_py_c virtuals (HD vtable at +0xB4): +0x14 getLeftHandMatrix, +0x1C getRightHandMatrix */
static inline Mtx34* player_getHandMatrix(fopAc_ac_c* player, bool rightHand) {
    himo2_playerVtable_c* methods = himo2_playerVtable(player);
    u32 method = rightHand ? methods->getRightHandMatrix : methods->getLeftHandMatrix;
    return gabi::call_ptr<Mtx34*>(method, player);
}

/* himo2_control (inline): from the hook end towards the player */
static inline void himo2_control(himo2_class* i_this, himo2_s* param_2) {
    gabi::Local<cXyz> local_68;
    gabi::Local<cXyz> local_74;
    local_68->x = 0.0f;
    local_68->y = 0.0f;
    if (i_this->m02DC == HIMO2_WAIT) {
        local_68->z = (15.625f * i_this->m2188) *
                      ((f32)l_himo2Regs.mChild[8].mFloatRegs[18] + 0.002f); /* REG8_F(18) */
    } else {
        local_68->z = (15.625f * i_this->m2188) *
                      ((f32)l_himo2Regs.mChild[0].mFloatRegs[1] + 0.0007f); /* REG0_F(1) */
    }
    s32 i2 = i_this->m02CC + 1;
    param_2 += i2;
    for (s32 i = i2; i < 100; i++, param_2++) {
        f32 fVar2 = param_2->m10.z - param_2[-1].m10.z;
        f32 fVar1 = param_2->m10.x - param_2[-1].m10.x;
        f32 dVar8 = (param_2->m10.y + i_this->m02E4) - param_2[-1].m10.y;
        s16 r29 = cM_atan2s(fVar1, fVar2);
        f32 d = std_sqrtf(gabi::fmadds(fVar1, fVar1, fVar2 * fVar2));
        s16 r30 = (s16)-cM_atan2s(dVar8, d);
        mDoMtx_YrotS(calc_mtx(), r29);
        mDoMtx_XrotM(calc_mtx(), r30);
        MtxPosition(local_68, local_74);
        param_2->m10.x = param_2[-1].m10.x + local_74->x;
        param_2->m10.y = param_2[-1].m10.y + local_74->y;
        param_2->m10.z = param_2[-1].m10.z + local_74->z;
    }
}

/* himo2_control2 (inline): from the player towards the hook, with the sway */
static inline void himo2_control2(himo2_class* i_this, himo2_s* param_2) {
    gabi::Local<cXyz> local_a8;
    gabi::Local<cXyz> local_b4;
    param_2 += 98;
    local_a8->y = 0.0f;
    s32 dc = i_this->m02DC;
    s32 cc = i_this->m02CC;
    local_a8->x = 0.0f;
    f32 m2188 = i_this->m2188;
    if (dc == 0) {
        local_a8->z = (15.625f * m2188) * ((f32)l_himo2Regs.mChild[8].mFloatRegs[18] + 0.002f);
    } else {
        local_a8->z = (15.625f * m2188) * ((f32)l_himo2Regs.mChild[0].mFloatRegs[1] + 0.0007f);
    }
    f32 sinT = 0.0f;                                            /* GameCube f29 */
    f32 cosT = 0.0f;                                            /* GameCube f28 */
    f32 f1 = (f32)l_himo2Regs.mChild[8].mFloatRegs[17] + 10.0f; /* REG8_F(17) */
    s32 iVar4 = cc + 1;
    for (; iVar4 < 100; iVar4++, param_2--) {
        f32 f30 = param_2->m10.y - param_2[1].m10.y;
        if ((u32)dc < 4) { /* m02DC >= 0 && m02DC <= 3 */
            if (dc != 0)
                f1 = (i_this->m2530 * (f32)(s32)(99 - iVar4)) * 0.01f;
            s32 a1 = (s32)i_this->m02D8 * (l_himo2Regs.mChild[18].mShortRegs[5] + 700) +
                     iVar4 * (l_himo2Regs.mChild[18].mShortRegs[6] + 2000);
            s32 a2 = (s32)i_this->m02D8 * (l_himo2Regs.mChild[18].mShortRegs[7] + 500) +
                     iVar4 * (l_himo2Regs.mChild[18].mShortRegs[8] + 2000);
            sinT = cM_ssin(a1) * f1;
            cosT = cM_scos(a2) * f1;
        }
        f32 dVar7 = (param_2->m10.x - param_2[1].m10.x) + sinT;
        f32 dVar6 = (param_2->m10.z - param_2[1].m10.z) + cosT;
        s16 r26 = cM_atan2s(dVar7, dVar6);
        f32 d = std_sqrtf(gabi::fmadds(dVar7, dVar7, dVar6 * dVar6));
        s16 r27 = (s16)-cM_atan2s(f30, d);
        param_2[1].m1E = r26;
        param_2[1].m1C = r27;
        mDoMtx_YrotS(calc_mtx(), r26);
        mDoMtx_XrotM(calc_mtx(), r27);
        MtxPosition(local_a8, local_b4);
        param_2->m10.x = param_2[1].m10.x + local_b4->x;
        param_2->m10.y = param_2[1].m10.y + local_b4->y;
        param_2->m10.z = param_2[1].m10.z + local_b4->z;
        dc = i_this->m02DC;
    }
}

/* 0216E194 */
static BOOL daHimo2_Execute(himo2_class* i_this) {
    daPy_py_c* player = (daPy_py_c*)dComIfGp_getPlayer(0);
    if (himo2_btd == 0)
        himo2_btd = (himo2_btd_c*)fpcM_Search(0x0216DE2C /* b_a_sub */, i_this);
    if (himo2_dr == 0)
        himo2_dr = (himo2_dr_c*)fpcM_Search(0x0216DDDC /* dr_a_sub */, i_this);
    i_this->m02D8 = i_this->m02D8 + 1;
    rope_scale = l_himo2HIO.m1C;
    gabi::Local<cXyz> local_c8;
    local_c8->y = 0.0f;
    local_c8->x = 4.0f;
    local_c8->z = 0.0f;
    if ((u32)player->mResetFlg0 &
        daPy_py_c::daPyRFlg0_ROPE_GRAB_RIGHT_HAND) { /* player->getRopeGrabRightHand() */
        PSMTXCopy(player_getHandMatrix(player, false), calc_mtx());
        MtxPosition(local_c8, &i_this->m02EC[1]);
        PSMTXCopy(player_getHandMatrix(player, true), calc_mtx());
        MtxPosition(local_c8, &i_this->m02EC[0]);
    } else {
        PSMTXCopy(player_getHandMatrix(player, false), calc_mtx());
        MtxPosition(local_c8, &i_this->m02EC[0]);
        PSMTXCopy(player_getHandMatrix(player, true), calc_mtx());
        MtxPosition(local_c8, &i_this->m02EC[1]);
    }
    i_this->m02B4 = i_this->m02EC[0];
    s32 iVar5 = i_this->m02CC + -50;
    if (iVar5 < 3)
        iVar5 = 3;

    gabi::Local<dBgS_GndChk> gndChk;
    dBgS_GndChk_ct(gndChk, gc_tu.gndChk, false);
    if (iVar5 <= 0)
        iVar5 = 1;
    himo2_s* phVar3 = &i_this->m1120[0];
    do {
        if (i_this->m02DC != HIMO2_LAUNCH_FREE) {
            f32 z = phVar3->m10.z;
            f32 y = phVar3->m10.y;
            f32 x = phVar3->m10.x;
            gndChk->m_pos.z = z; /* gndChk.SetPos(&pos), pos.y += 60.0f */
            gndChk->m_pos.x = x;
            gndChk->m_pos.y = y + 60.0f;
            f32 h = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
            phVar3->m0C = h + 5.0f;
        } else {
            phVar3->m0C = -100000.0f;
        }
        phVar3++;
    } while (--iVar5);

    himo2_moveExternal(i_this); /* new_himo2_move */
    phVar3 = &i_this->m0310[0];
    for (s32 i = 0; i < i_this->m02CC + 1; i++, phVar3++)
        phVar3->m10 = i_this->m02B4;
    phVar3 = &i_this->m0310[0];
    himo2_control(i_this, phVar3);
    i_this->m0310[99].m10 = i_this->current.pos;
    himo2_control2(i_this, phVar3);
    if (i_this->m02DC == HIMO2_LAUNCH_FREE || i_this->m02DC == HIMO2_LAUNCH_TARGET) {
        i_this->m2050.SetR(20.0f);
    } else {
        i_this->m2050.SetR(-100.0f);
    }
    i_this->m2050.SetC(&i_this->current.pos);
    dComIfG_Ccsp_Set(&i_this->m2050);
    i_this->eyePos = i_this->current.pos;
    /* ~dBgS_GndChk */
    gndChk->dBgS_GrpPassChk::__vtbl = gc_tu.gndChkDt[2];
    gndChk->dBgS_PolyPassChk::__vtbl = gc_tu.gndChkDt[1];
    gndChk->cBgS_PolyInfo::__vtbl = gc_tu.gndChkDt[0];
    himo2_destroyGroundCheck(gndChk.get(), 0);
    return TRUE;
}

/* ---- Draw ---- */

/* mDoExt_3DlineMat1_c (HD): vtable at +0x130, line array at +0x184 (0x10 per line, segment
 * positions first) */
static inline cXyz* lineMat_getPos(himo2_lineMat_c* m, s32 i) {
    return m->mpLines.get()[i].mpPositions;
}
// mDoExt_3DlineMat1_c::update(u16 n, f32 width, GXColor&, u16, dKy_tevstr_c*)
static inline void lineMat_update(himo2_lineMat_c* m, u16 n, f32 w, dKy_tevstr_c* tev) {
    ((mDoExt_3DlineMat1_c*)m)->update(n, w, *(GXColor*)rope_color, 0, tev);
}
/* dComIfGd_set3DlineMat (HD): play+0x5FB4 holds the sort packets (0x9C each), indexed by the
 * line's material id (virtual +0x14); 025EDD04 mDoExt_3DlineMatSortPacket::setMat */
static inline void himo2_sortLineMaterial(himo2_lineMat_c* m) {
    dComIfGd_set3DlineMat((mDoExt_3DlineMat1_c*)m);
}
/* HD: after each line, its packet gets the tev struct and 02415ABC(play+0x5B34 object, packet,
 * line) */
static inline void himo2_hd_line_packet(himo2_class* i_this, s32 lightingPacket, s32 packet,
                                        himo2_lineMat_c* m) {
    i_this->mPackets[lightingPacket].mpTevStr = &i_this->tevStr;
    daPy_py_c* player = daPy_getPlayerLinkActorClass();
    himo2_setLinePacket(player, &i_this->mPackets[packet], m);
}
static inline void hook_model_draw(himo2_class* i_this, J3DModel* hookModel) {
    J3DModel_setBaseTRMtx(hookModel, calc_mtx());
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, hookModel, &i_this->tevStr);
    mDoExt_modelUpdateDL(hookModel);
}

/* spin_draw (inline): the rope wound around the target (kui) */
static inline void spin_draw(himo2_class* i_this) {
    dComIfGp_ea(); /* HD: an unused dComIfGp_get() */
    s16 r21 = 0;   /* GameCube r26 */
    fopAc_ac_c* t = i_this->m217C;
    s16 r20 = i_this->m2510; /* GameCube r23 */
    if (t == nullptr)        /* HD */
        return;
    f32 sy = t->scale.y;
    f32 d = sy - 1.0f;
    f32 f5 = -7.0f * d;
    f32 f4 = 8.0f * d;
    if ((fopAcM_GetParam(t) & 0x0F) == 3) {
        f32 a = (f32)l_himo2Regs.mChild[8].mFloatRegs[8] + -3.0f;
        f32 b = (f32)l_himo2Regs.mChild[8].mFloatRegs[9] + 1.5f;
        f4 = f4 + a;
        f5 = f5 + b;
    }
    if (sy < 0.7f)
        f4 = f4 + 3.0f;
    /* HD: always the m251C == 0 side (GameCube: 700 and -15 when m251C != 0) */
    gabi::Local<cXyz> local_504;
    gabi::Local<cXyz> local_510;
    local_504->x =
        gabi::fmadds((f32)l_himo2Regs.mChild[0].mFloatRegs[0], 10.0f, 15.0f) + i_this->m24B8;
    local_504->y = f4 + -152.5f;
    local_504->z = (i_this->m24B4 + 26.0f) + f5;
    mDoMtx_YrotS(calc_mtx(), r20);
    MtxPosition(local_504, local_510);
    t = i_this->m217C;
    f32 px = t->current.pos.x + local_510->x;
    f32 py = t->current.pos.y + local_510->y;
    f32 pz = t->current.pos.z + local_510->z;
    local_504->x = 0.0f;
    local_504->y = 0.0f;
    local_504->z = 3.90625f;
    s16 r22 = -0x30D4; /* GameCube r30 */
    cXyz* r29 = lineMat_getPos(&i_this->m1F30, 0) + i_this->m1F6C;
    gabi::Local<cXyz[100]> local_4f8;
    cXyz* positions = (cXyz*)local_4f8.get();
    for (s32 i = 0; i < 100; i++) {
        positions[i].y = -200000.0f;
        s16 r18 = r22; /* GameCube r22 */
        if (i < 50) {
            r22 = (s16)(r22 + ((s16)l_himo2Regs.mChild[18].mShortRegs[1] + 100));
        } else if (i <= i_this->m24BC + 49) {
            s16 rs4 = (s16)l_himo2Regs.mChild[0].mShortRegs[4];
            r22 = (s16)(r22 + i_this->m24C8);
            fopAc_ac_c* tt = i_this->m217C;
            if (i >= 95 - rs4)
                r22 = (s16)(r22 + ((s16)l_himo2Regs.mChild[0].mShortRegs[5] + -1000));
            if (tt != nullptr && (fopAcM_GetParam(tt) & 0xF0) != 0)
                r21 = (s16)(r21 + (rs4 + -400));
        } else {
            if (i_this->m24BC == 0)
                r22 = (s16)(r22 + ((s16)l_himo2Regs.mChild[18].mShortRegs[2] + 70));
            else
                r22 = (s16)(r22 + 50);
        }
        mDoMtx_YrotS(calc_mtx(), r20);
        mDoMtx_ZrotM(calc_mtx(), r21);
        mDoMtx_XrotM(calc_mtx(), r18);
        mDoMtx_YrotM(calc_mtx(), -700);
        MtxPosition(local_504, local_510);
        px = px + local_510->x;
        py = py + local_510->y;
        pz = pz + local_510->z;
        if ((u32)i == (u32)((s16)l_himo2Regs.mChild[0].mShortRegs[6] + 48)) {
            i_this->m2504.y = py;
            i_this->m2504.z = pz;
            i_this->m2504.x = px;
        }
        if (i_this->m24D8 < 2 || i >= 50) {
            if (i == 99) {
                MtxTrans(px, py, pz, 0);
                mDoMtx_YrotM(calc_mtx(), r20);
                mDoMtx_ZrotM(calc_mtx(), r21);
                mDoMtx_XrotM(calc_mtx(), r18);
                mDoMtx_YrotM(calc_mtx(), -700);
                MtxTrans(0.0f, 0.0f, -3.0f, 1);
                mDoMtx_XrotM(calc_mtx(), i_this->m24CA);
                gabi::Local<cXyz> local_528;
                local_528->x = 0.0f;
                local_528->y = 0.0f;
                local_528->z = 15.0f;
                MtxPosition(local_528, &i_this->m24CC);
                MtxScale(0.2f, 0.2f, 0.2f, 1);
                hook_model_draw(i_this, i_this->mpHookModel);
            } else {
                positions[i].y = py;
                positions[i].z = pz;
                positions[i].x = px;
            }
        }
    }
    for (int i = 99; i >= 0; i--) {
        f32 y = (f32)positions[i].y;
        if (y > -100000.0f) {
            r29->y = y;
            r29->x = (f32)positions[i].x;
            r29->z = (f32)positions[i].z;
            i_this->m1F6C = i_this->m1F6C + 1;
            ++r29;
        }
    }
}

/* himo2_draw (inline) */
static inline void himo2_draw(himo2_class* i_this, himo2_s* param_2) {
    daPy_py_c* player = (daPy_py_c*)dComIfGp_getPlayer(0);
    cXyz* pcVar6 = lineMat_getPos(&i_this->m1F30, 0) + i_this->m1F6C;
    *pcVar6 = i_this->m02B4;
    s32 r30 = i_this->m02CC;
    i_this->m1F6C = i_this->m1F6C + 1;
    ++pcVar6;
    himo2_s* phVar4 = &param_2[r30 + 1];
    for (; r30 < 98; r30++, phVar4++) {
        if (r30 == 97) {
            if (i_this->m02DC == HIMO2_WAIT &&
                i_this->m2188 < (f32)l_himo2Regs.mChild[0].mFloatRegs[3] + 50.0f) {
                PSMTXCopy(player_getHandMatrix(player, false) /* getLeftHandMatrix */, calc_mtx());
                MtxTrans(10.0f, 0.0f, 0.0f, 1);
                mDoMtx_YrotM(calc_mtx(), 0x4000);
            } else {
                MtxTrans(phVar4->m10.x, phVar4->m10.y, phVar4->m10.z, 0);
                mDoMtx_YrotM(calc_mtx(), phVar4->m1E);
                mDoMtx_XrotM(calc_mtx(), phVar4->m1C);
                MtxTrans(0.0f, 0.0f, 12.0f, 1);
                MtxRotX(3.1415927f, 1);
                MtxRotZ(3.1415927f, 1);
            }
            MtxScale(0.2f, 0.2f, 0.2f, 1);
            gabi::Local<cXyz> local_38;
            local_38->x = 0.0f;
            local_38->y = 0.0f;
            local_38->z = 15.0f;
            MtxPosition(local_38, &i_this->m24CC);
            J3DModel* hookModel = i_this->mpHookModel;
            hook_model_draw(i_this, hookModel);
            /* HD: the hook's packet */
            daPy_py_c* player = daPy_getPlayerLinkActorClass();
            himo2_setHookPacket(player, &i_this->mPackets[0], hookModel);
        } else {
            *pcVar6 = phVar4->m10;
            i_this->m1F6C = i_this->m1F6C + 1;
            ++pcVar6;
        }
    }
}

/* himo_hang_draw (inline) */
static inline void himo_hang_draw(himo2_class* i_this) {
    cXyz* pcVar3 = lineMat_getPos(&i_this->m1F30, 0) + i_this->m1F6C;
    gabi::Local<cXyz> local_38;
    *local_38.get() = i_this->m02EC[0] - i_this->m2504; /* cXyz::operator- */
    local_38->x = local_38->x * 0.25f;
    local_38->y = local_38->y * 0.25f;
    local_38->z = local_38->z * 0.25f;
    for (int uVar2 = 0; uVar2 < 5; uVar2++, ++pcVar3) {
        f32 u = (f32)uVar2;
        pcVar3->x = gabi::fmadds(local_38->x, u, i_this->m2504.x);
        pcVar3->y = gabi::fmadds(local_38->y, u, i_this->m2504.y);
        pcVar3->z = gabi::fmadds(local_38->z, u, i_this->m2504.z);
        i_this->m1F6C = i_this->m1F6C + 1;
    }
}

// daHimo2_Draw (inlines spin_draw, himo2_disp, himo2_draw, himo_hang_draw)
/* 0216CA5C */
static BOOL daHimo2_Draw(himo2_class* i_this) {
    if (i_this->m24D9 < HIMO2_DEMO_INACTIVE || i_this->m029E != 0)
        return TRUE;
    if (i_this->m02DC == HIMO2_WAIT) {
        daPy_py_c* pl = (daPy_py_c*)dComIfGp_getPlayer(0);
        if (gabi::call_ptr<u32>(himo2_playerVtable(pl)->checkPlayerNoDraw, pl) !=
            0) /* checkPlayerNoDraw */
            return TRUE;
        pl = (daPy_py_c*)dComIfGp_getPlayer(0);
        if (gabi::call_ptr<u32>(himo2_playerVtable(pl)->checkPlayerGuard, pl) !=
            0) /* checkPlayerGuard */
            return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    i_this->m1F6C = 0;
    /* dComIfGd_setListP1() */
    dComIfGd_setListP1();
    if (i_this->m24D8 != 0)
        spin_draw(i_this);
    if (i_this->m24D9 == HIMO2_DEMO_INACTIVE || i_this->m24D9 >= HIMO2_DEMO_SET_ROCK) {
        /* himo2_disp */
        if (i_this->m02DC < 10)
            himo2_draw(i_this, &i_this->m0310[0]);
        else
            himo_hang_draw(i_this);
    }
    if (i_this->m1F6C > 200) {
        dComIfGd_setList();
        return TRUE;
    }

    cXyz* r19 = lineMat_getPos(&i_this->m1F30, 0);
    gabi::Local<cXyz[200]> local_a10;
    cXyz* positions = (cXyz*)local_a10.get();
    s32 n = i_this->m1F6C;
    for (int i = 0; i < n; i++) {
        positions[i] = *r19;
        ++r19;
    }
    for (int i = 0; i < i_this->m1F6C; i++) {
        --r19;
        *r19 = positions[i];
    }
    lineMat_update(&i_this->m1F30, (u16)i_this->m1F6C, rope_scale, &i_this->tevStr);
    himo2_sortLineMaterial(&i_this->m1F30);
    himo2_hd_line_packet(i_this, 1, 1, &i_this->m1F30);

    daPy_py_c* player = (daPy_py_c*)dComIfGp_getPlayer(0);
    mDoMtx_YrotS(calc_mtx(), (s16)-player->shape_angle.y);
    gabi::Local<cXyz> sp44;
    gabi::Local<cXyz> sp38;
    {
        gabi::Local<cXyz> tmp;
        *tmp.get() = i_this->m02EC[1] - i_this->m1F84; /* cXyz::operator- */
        *sp44.get() = *tmp.get();
    }
    MtxPosition(sp44, sp38);
    sp38->z = sp38->z * ((f32)l_himo2Regs.mChild[0].mFloatRegs[5] + 500.0f);
    if (sp38->z > 16384.0f) {
        sp38->z = 16384.0f;
    } else if (sp38->z < -16384.0f) {
        sp38->z = -16384.0f;
    }
    cLib_addCalcAngleS2(&i_this->m1F90, (s16)gabi::ftoi(sp38->z), 2,
                        (s16)((s16)l_himo2Regs.mChild[0].mShortRegs[2] + 0x800));
    sp38->x = sp38->x * ((f32)l_himo2Regs.mChild[0].mFloatRegs[6] + -200.0f);
    f32 f1 = (f32)l_himo2Regs.mChild[0].mFloatRegs[7] + 4000.0f;
    if (sp38->x > f1)
        sp38->x = f1;
    if (sp38->x < -f1)
        sp38->x = -f1;
    cLib_addCalcAngleS2(&i_this->m1F94, (s16)gabi::ftoi(sp38->x), 2,
                        (s16)((s16)l_himo2Regs.mChild[0].mShortRegs[2] + 0x800));
    if (sp38->y < (f32)l_himo2Regs.mChild[0].mFloatRegs[8] + -20.0f) {
        cLib_addCalcAngleS2(&i_this->m1F92, -0x8000, 2, 0x1000);
    } else {
        cLib_addCalcAngleS2(&i_this->m1F92, 0, 2, 0x1000);
    }
    i_this->m1F84 = i_this->m02EC[1];
    f32 f29 = gabi::fmadds(i_this->m2188, (f32)l_himo2Regs.mChild[0].mFloatRegs[9] + 3.0f,
                           (f32)l_himo2Regs.mChild[0].mFloatRegs[10]) -
              120.0f;
    sp44->z = 0.0f;
    sp44->x = 0.0f;
    for (int i = 0; i < 5; i++) {
        f32 lim = (f32)(4 - i) * 300.0f;
        cXyz* r16 = lineMat_getPos(&i_this->m1F98, i);
        if (f29 > lim) {
            cLib_addCalc0(&i_this->m1F70[i], 1.0f,
                          (f32)l_himo2Regs.mChild[0].mFloatRegs[11] + 0.2f);
        } else {
            cLib_addCalc2(&i_this->m1F70[i], 1.0f, 1.0f,
                          (f32)l_himo2Regs.mChild[0].mFloatRegs[12] + 0.1f);
        }
        sp44->y = i_this->m1F70[i] * 15.0f;
        mDoMtx_YrotS(calc_mtx(), player->shape_angle.y);
        f32 k = 1.0f - i_this->m1F70[i];
        s16 m1F94 = i_this->m1F94;
        s16 r6 = (s16)gabi::ftoi(k * ((f32)l_himo2Regs.mChild[0].mFloatRegs[13] + -20000.0f));
        s16 m1F92 = i_this->m1F92;
        mDoMtx_ZrotM(calc_mtx(), (s16)((m1F94 + -2000) * i + m1F92 + r6)); /* HD: no REG0_S(0) */
        mDoMtx_XrotM(calc_mtx(), i_this->m1F90);
        MtxTrans(0.0f, -sp44->y, 0.0f, 1);
        for (int j = 0; j < 32; j++, ++r16) {
            mDoMtx_XrotM(calc_mtx(), 0x800);
            MtxPosition(sp44, r16);
            PSVECAdd(r16, &i_this->m02EC[1], r16); /* PSVECAdd (operator+=) */
        }
    }
    lineMat_update(&i_this->m1F98, 0x20, rope_scale, &i_this->tevStr);
    himo2_sortLineMaterial(&i_this->m1F98);
    himo2_hd_line_packet(i_this, 0, 2,
                         &i_this->m1F98); /* HD stores the tev struct into packet 0's field */

    cXyz* r19b = lineMat_getPos(&i_this->m1FD8, 0);
    f32 step = (f32)l_himo2Regs.mChild[0].mFloatRegs[15];
    f32 f1_2;
    if ((gabi::ftoi(i_this->m2188) & ((s16)l_himo2Regs.mChild[0].mShortRegs[4] + 64)) != 0)
        f1_2 = (f32)l_himo2Regs.mChild[0].mFloatRegs[14] + 15.0f;
    else
        f1_2 = 0.0f;
    cLib_addCalc2(&i_this->m1FD4, f1_2, 1.0f, step + 4.0f);
    gabi::Local<cXyz> sp2C;
    *sp2C.get() = i_this->m02EC[0] - i_this->m02EC[1]; /* cXyz::operator- */
    Mtx34* cm = calc_mtx();
    s16 a = cM_atan2s(sp2C->x, sp2C->z);
    mDoMtx_YrotS(cm, (s16)(a - 0x4000)); /* HD: no REG0_S(0) */
    f32 m1FD4 = i_this->m1FD4;
    f32 y = ((f32)l_himo2Regs.mChild[0].mFloatRegs[7] + -10.0f) - m1FD4;
    f32 z = ((f32)l_himo2Regs.mChild[0].mFloatRegs[8] + 10.0f) + m1FD4;
    f32 s0 = i_this->m1F70[0];
    sp44->x = 0.0f;
    sp44->y = y * s0;
    sp44->z = z * s0;
    MtxPosition(sp44, sp38);
    sp2C->x = sp2C->x * (1.0f / 15.0f);
    sp2C->y = sp2C->y * (1.0f / 15.0f);
    sp2C->z = sp2C->z * (1.0f / 15.0f);
    s32 r3_2 = 0;
    for (int i = 0; i < 16; i++, r3_2 += 0x888) {
        f32 fi = (f32)i;
        f32 fVar1 = cM_ssin(r3_2);
        r19b->x = gabi::fmadds(sp38->x, fVar1, gabi::fmadds(sp2C->x, fi, i_this->m02EC[1].x));
        r19b->y = gabi::fmadds(sp38->y, fVar1, gabi::fmadds(sp2C->y, fi, i_this->m02EC[1].y));
        r19b->z = gabi::fmadds(sp38->z, fVar1, gabi::fmadds(sp2C->z, fi, i_this->m02EC[1].z));
        ++r19b;
    }
    lineMat_update(&i_this->m1FD8, 16, rope_scale, &i_this->tevStr);
    himo2_sortLineMaterial(&i_this->m1FD8);
    himo2_hd_line_packet(i_this, 0, 3, &i_this->m1FD8);
    dComIfGd_setList();
    return TRUE;
}

// HD: compiler-generated functions of this translation unit.
// himo2HIO_c::~himo2HIO_c (deleting)
/* 0216F7E8 */
static void himo2HIO_c_dt(himo2HIO_c* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

// __sinit_d_a_himo2_cpp
/* 0216F6D8 */
static void __sinit_d_a_himo2_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
    /* l_himo2HIO: himo2HIO_c::himo2HIO_c() */
    himo2HIO_c& h = l_himo2HIO;
    h.m10 = 60.0f;
    h.m06 = 0;
    h.m08 = 15000;
    h.m14 = 1500.0f;
    h.m0A = 1000;
    h.m18 = 1000.0f;
    h.m0C = 600;
    h.m0E = 400;
    h.m1C = 2.3f;
    h.__vtbl = 0x10010EE4;
    h.m20 = 1.9f;
}

/* WWHD glue
str 10010F94 "i_this->m24AC < 100" renamed
str 10010FB4 in 0216DD3C
str 10010FC8 in 0216DE7C
str 10010FD8
str 10011004
str 1001100C
str 1001101C in 0216E9E0
str 1001102C
str 10011048 "Rope" renamed
str 10010DE4
str 10011014
data cc_sph_src 101B6E18
ext 024F0F98 u32 himo2_GetOnePolyInfo(dBgS_Acch*, cBgS_PolyInfo*)
ext 024EF0F4 s32 himo2_GetAttributeCode(dBgS*, cBgS_PolyInfo*)
ext 0255F458 void himo2_SoundSet(cXyz*, s32, u32, s32)
ext 025166F0 void himo2_constructSphere(dCcD_Sph*)
ext 02080404 void himo2_constructPacket(himo2_modelPacket_c*)
ext 0207FD38 void himo2_setupPacket(void*, s32)
ext 0216F7FC void himo2_moveExternal(himo2_class*)
ext 02008DAC void himo2_destroyGroundCheck(dBgS_GndChk*, s32)
data l_himo2HIO 1046469C
data rope_scale 10464674
data himo2_btd 10464678
data himo2_dr 1046467C
data l_himo2Regs 1047B608
data l_guideEmitter 10475628
data rope_color 101B6E14
ext 02415ABC void himo2_setLinePacket(daPy_py_c*, himo2_modelPacket_c*, himo2_lineMat_c*)
ext 023D9340 void himo2_setHookPacket(daPy_py_c*, himo2_modelPacket_c*, J3DModel*)
*/
