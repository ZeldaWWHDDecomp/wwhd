/**
 * d_a_himo2.cpp (WWHD)
 * Item - Grappling Hook / Rope
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_himo2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_himo2.h"

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

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 024F0F98 dBgS_Acch::GetOnePolyInfo(cBgS_PolyInfo*) (the result is tested as a full word) */
static inline u32 dBgS_Acch_GetOnePolyInfo(dBgS_Acch* a, void* pi) { return gabi::call<u32>(0x024F0F98, a, pi); }
static inline s32 dBgS_GetAttributeCode(dBgS* bgs, void* pi) { return gabi::call<s32>(0x024EF0F4, bgs, pi); }
/* 0255F458 dKy_Sound_set(cXyz pos (by value), s32, u32 actorId, s32) */
static inline void dKy_Sound_set(cXyz* pos, s32 p, u32 id, s32 t) { gabi::call(0x0255F458, pos, p, id, t); }
/* cBgS_PolyInfo inline constructor (HD: this TU's vtable 0x10010E04) */
static inline void cBgS_PolyInfo_ct(u32 p) {
    gabi::store<u16>(p + 0x0, 0xFFFF);
    gabi::store<u32>(p + 0x4, 0);
    gabi::store<s32>(p + 0x8, -1);
    gabi::store<u32>(p + 0xC, 0x10010E04);
    gabi::store<u16>(p + 0x2, 0x100);
}
/* JPABaseEmitter (HD) fields written by the inline setters */
static inline void emitter_setRate(u32 e, f32 v) { gabi::store<f32>(e + 0x34, v); }
static inline void emitter_setMaxFrame(u32 e, s32 v) { gabi::store<s32>(e + 0x5C, v); }
static inline void emitter_setSpread(u32 e, f32 v) { gabi::store<f32>(e + 0x58, v); }
static inline void emitter_setVolumeSweep(u32 e, f32 v) { gabi::store<f32>(e + 0x7C, v); }
static inline void emitter_setAwayFromAxisSpeed(u32 e, f32 v) { gabi::store<f32>(e + 0x6C, v); }
static inline bool SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b);
    if (s1 == b->mStringTop) return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(p + i);
        if (c != gabi::load<u8>(q + i)) return false;
        if (c == 0) return true;
    }
    return false;
}

/* 0216DD3C */
static void* s_a_d_sub(void* param_1, void* param_2) {
    WWHD_FUNC(0x0216DD3C, void*, param_1, param_2);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_KUI_e) {
        himo2_class* rope = (himo2_class*)param_2;
        /* HD: bounded (JUT_ASSERT(0x34D, m24AC < 100)) */
        if (rope->m24AC >= 100) {
            JUT_ASSERT_fail(gabi::at<const char>(0x10010FB4), 0x34D, gabi::at<const char>(0x10010F94));
            if (rope->m24AC >= 100)
                return nullptr;
        }
        rope->m218C[rope->m24AC] = (fopAc_ac_c*)param_1;
        rope->m24AC = rope->m24AC + 1;
    }
    return nullptr;
}
VERIFY(0x0216DD3C, s_a_d_sub);

/* 0216DDDC */
static void* dr_a_sub(void* param_1, void* param_2) {
    WWHD_FUNC(0x0216DDDC, void*, param_1, param_2);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_DR2_e)
        return param_1;
    return nullptr;
}
VERIFY(0x0216DDDC, dr_a_sub);

/* 0216DE2C */
static void* b_a_sub(void* param_1, void* param_2) {
    WWHD_FUNC(0x0216DE2C, void*, param_1, param_2);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_BTD_e)
        return param_1;
    return nullptr;
}
VERIFY(0x0216DE2C, b_a_sub);

/* 0216DE7C */
static BOOL himo2_bg_check(himo2_class* i_this) {
    WWHD_FUNC(0x0216DE7C, BOOL, i_this);
    if (i_this->m02A2 != 0)
        return FALSE;

    i_this->m2574.CrrPos(dComIfG_Bgsp());
    if (!(i_this->m2574.m_flags & 0x230)) /* ChkWallHit() || ChkRoofHit() || ChkGroundHit() */
        return FALSE;

    i_this->m02DC = 9;
    i_this->m0308 = 30;
    i_this->speedF = -i_this->speedF;
    gabi::Local<u8[0x10]> local_24; /* cBgS_PolyInfo */
    cBgS_PolyInfo_ct(gabi::ea(local_24.get()));
    if (dBgS_Acch_GetOnePolyInfo(&i_this->m2574, local_24) != 0) /* JUT_ASSERT(0x446, flag == NULL) */
        JUT_ASSERT_fail(gabi::at<const char>(0x10010FC8), 0x446, gabi::at<const char>(0x10010FD8));
    u32 uVar3 = dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo*)local_24.get());
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
    mDoAud_seStart(JA_SE_LK_SW_HIT_S, &i_this->current.pos, uVar3, reverb);
    s32 attrib_code = dBgS_GetAttributeCode(dComIfG_Bgsp(), local_24);
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
            mDoAud_seStart(JA_SE_LK_MS_WEP_HIT, &i_this->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
        u32 emitter;
        if (attrib_code == dBgS_Attr_WOOD_e) {
            u32 pa = dComIfGp_getParticle_ea();
            u32 k0 = gabi::ea(i_this) + 0x1A8; /* &tevStr.mColorK0 */
            emitter = gabi::ea(dPa_control_set(gabi::at<dPa_control_c>(pa), 0, dPa_name_ID_AK_JN_ELEMENTKIKUZU00, &i_this->current.pos,
                                               local_38, nullptr, 0xFF, nullptr, -1, gabi::at<GXColor>(k0), gabi::at<GXColor>(k0), nullptr));
            if (emitter != 0) {
                emitter_setSpread(emitter, 0.2f);
                emitter_setVolumeSweep(emitter, 0.15f);
            }
        } else {
            local_38->x = (s16)(local_38->x + 0x4000);
            u32 pa = dComIfGp_getParticle_ea();
            emitter = gabi::ea(dPa_control_set(gabi::at<dPa_control_c>(pa), 0, dPa_name_ID_AK_JN_ELEMENTHIBANA00, &i_this->current.pos,
                                               local_38, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
            if (emitter != 0)
                emitter_setAwayFromAxisSpeed(emitter, 15.0f);
            gabi::Local<cXyz> pos;
            pos->x = i_this->current.pos.x; /* cXyz passed by value: an FPR copy */
            pos->y = i_this->current.pos.y;
            pos->z = i_this->current.pos.z;
            u32 id = i_this != nullptr ? gabi::load<u32>(gabi::ea(i_this) + 4) : (u32)-1; /* fopAcM_GetID */
            dKy_Sound_set(pos, 100, id, 5);
        }
        if (emitter != 0) {
            emitter_setRate(emitter, 8.0f);
            emitter_setMaxFrame(emitter, 1);
        }
    }
    return TRUE;
}
VERIFY(0x0216DE7C, himo2_bg_check);

/* 0216E144 */
static void pl_pos_add(himo2_class* i_this) {
    WWHD_FUNC(0x0216E144, void, i_this);
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER));
    gabi::Local<cXyz> local_28;
    gabi::call(0x0201ADE0, &player->old.pos, local_28.get(), &player->current.pos); /* cXyz::operator- */
    gabi::call(0x028E8D88, &i_this->current.pos, local_28.get(), &i_this->current.pos); /* PSVECAdd (operator+=) */
}
VERIFY(0x0216E144, pl_pos_add);

/* 0216E9A8 */
static BOOL daHimo2_IsDelete(himo2_class*) {
    WWHD_FUNC(0x0216E9A8, BOOL, (himo2_class*)nullptr);
    return TRUE;
}
VERIFY(0x0216E9A8, daHimo2_IsDelete);

/* 0216E9B0 */
static BOOL daHimo2_Delete(himo2_class*) {
    WWHD_FUNC(0x0216E9B0, BOOL, (himo2_class*)nullptr);
    mDoHIO_deleteChild(l_himo2HIO.mNo);
    return TRUE;
}
VERIFY(0x0216E9B0, daHimo2_Delete);

/* 0216E9E0 */
static BOOL CallbackCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0216E9E0, BOOL, i_this);
    himo2_class* a_this = (himo2_class*)i_this;
    const char* always = gabi::at<const char>(0x10011004); /* "Always" */
    void* img = dComIfG_getObjectRes(always, dRes_INDEX_ALWAYS_BTI_ROPE_e, HIMO2_SAFESTRING_VTBL);
    if (!mDoExt_3DlineMat1_c_init(&a_this->m1F30, 1, 200, img, 0))
        return FALSE;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(gabi::at<const char>(0x1001100C) /* "Link" */,
                                                                  dRes_INDEX_LINK_BDL_ROPEEND_e, HIMO2_SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0xAC1, modelData != NULL) */
        JUT_ASSERT_fail(gabi::at<const char>(0x1001101C), 0xAC1, gabi::at<const char>(0x1001102C));
    a_this->mpHookModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203U);
    if (!a_this->mpHookModel)
        return FALSE;
    img = dComIfG_getObjectRes(always, dRes_INDEX_ALWAYS_BTI_ROPE_e, HIMO2_SAFESTRING_VTBL);
    if (!mDoExt_3DlineMat1_c_init(&a_this->m1F98, 5, 0x20, img, 0))
        return FALSE;
    img = dComIfG_getObjectRes(always, dRes_INDEX_ALWAYS_BTI_ROPE_e, HIMO2_SAFESTRING_VTBL);
    if (!mDoExt_3DlineMat1_c_init(&a_this->m1FD8, 1, 0x10, img, 0))
        return FALSE;
    /* HD: in one stage (0x10011014) the hook's packets get an extra setup */
    gabi::Local<SafeString> a;
    a->mStringTop = 0x10011014;
    a->__vtbl = HIMO2_SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mStringTop = dComIfGp_ea() + 0x5134; /* dComIfGp_getStartStageName() */
    b->__vtbl = HIMO2_SAFESTRING_VTBL;
    if (SafeString_eq(a.get(), b.get())) {
        for (int i = 0; i < 4; i++)
            gabi::call(0x0207FD38, a_this->mPackets[i], 0);
    }
    return TRUE;
}
VERIFY(0x0216E9E0, CallbackCreateHeap);

/* 0216EC18 */
static cPhs_State daHimo2_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0216EC18, cPhs_State, i_this);
    himo2_class* a_this = (himo2_class*)i_this;
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER));

    /* fopAcM_ct(i_this, himo2_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = HIMO2_VTBL;
            mDoExt_3DlineMat1_c_ct(&a_this->m1F30);
            mDoExt_3DlineMat1_c_ct(&a_this->m1F98);
            mDoExt_3DlineMat1_c_ct(&a_this->m1FD8);
            dCcD_Stts_ct(&a_this->m2014);
            gabi::call(0x025166F0, &a_this->m2050); /* dCcD_Sph::dCcD_Sph */
            for (int i = 0; i < 4; i++)
                mDoExt_J3DModelPacketS_ct(a_this->mPackets[i]);
            dBgS_AcchCir_ct(&a_this->m2534);
            dBgS_ObjAcch_ct(&a_this->m2574, dBgS_ObjAcch_vt{0x10010E74, 0x10010E94, 0x10010E84});
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    if (!fopAcM_entrySolidHeap(a_this, 0x0216E9E0 /* CallbackCreateHeap */, gabi::load<s16>(HIMO2_REG(0x92)) + 0x9050))
        return cPhs_ERROR_e;

    /* mpHookModel->setBaseScale(cXyz(1.0f, 1.0f, 1.0f)) */
    u32 m = gabi::ea(a_this->mpHookModel.get());
    gabi::store<f32>(m + 0xBC, 1.0f);
    gabi::store<f32>(m + 0xC0, 1.0f);
    gabi::store<f32>(m + 0xC4, 1.0f);
    a_this->m02CC = gabi::load<s16>(HIMO2_REG(0x82)) + 96;
    /* player->getLeftHandPos() (HD +0x3F0) */
    f32 y = gabi::load<f32>(gabi::ea(player) + 0x3F4);
    f32 x = gabi::load<f32>(gabi::ea(player) + 0x3F0);
    f32 z = gabi::load<f32>(gabi::ea(player) + 0x3F8);
    a_this->current.pos.x = x;
    a_this->current.pos.y = y;
    a_this->current.pos.z = z;
    a_this->m2574.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &a_this->m2534, &a_this->speed, nullptr, nullptr);
    a_this->m2534.SetWall(30.0f, 30.0f);
    a_this->m2574.m_flags &= ~(u32)dBgS_Acch::ROOF_NONE;
    f32 h = gabi::load<f32>(HIMO2_REG(0x24)) + 60.0f;
    a_this->m2500 = 1.0f;
    a_this->m2574.m_roof_crr_height = h;
    l_himo2HIO.mNo = mDoHIO_createChild(gabi::at<const char>(0x10011048) /* "Rope" */, &l_himo2HIO);
    rope_scale = 2.0f;
    a_this->m2014.Init(0xFF, 0xFF, a_this);
    a_this->m2050.Set(gabi::at<dCcD_SrcSph>(0x101B6E18));
    a_this->m2050.SetStts(&a_this->m2014);
    himo2_dr = 0;
    himo2_btd = 0;
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0216EC18, daHimo2_Create);

/* 0216F7E8 himo2HIO_c::~himo2HIO_c (deleting) */
static void himo2HIO_c_dt(himo2HIO_c* i_this, s32 flags) {
    WWHD_FUNC(0x0216F7E8, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x0216F7E8, himo2HIO_c_dt);

/* 0216F6D8 __sinit_d_a_himo2_cpp */
static void __sinit_d_a_himo2_cpp() {
    WWHD_FUNC(0x0216F6D8, void);
    sinit_header_statics(0x10464680, 0x101B6E58);
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
VERIFY(0x0216F6D8, __sinit_d_a_himo2_cpp);

/* search_target (GameCube 800ECC54) is inlined into setTargetPos in HD. HD: the candidate list
 * holds 100 actors; in "ADMumi" the angular window is 10000 wider and the height tolerance is
 * extended by REG(0x5A8) - 600; a REG0_S (0x62A) widens the window. */
static inline bool himo2_isStartStage(u32 lit) {
    gabi::Local<SafeString> a;
    a->mStringTop = lit;
    a->__vtbl = HIMO2_SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mStringTop = dComIfGp_ea() + 0x5134;
    b->__vtbl = HIMO2_SAFESTRING_VTBL;
    return SafeString_eq(a.get(), b.get());
}

static inline fopAc_ac_c* search_target(himo2_class* i_this, f32 px, f32 py, f32 pz) {
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER));
    i_this->m24AC = 0;
    for (int i = 0; i < 100; i++)
        i_this->m218C[i] = nullptr;
    fpcM_Search(0x0216DD3C /* s_a_d_sub */, i_this);
    mDoMtx_YrotS(calc_mtx(), (s16)-player->shape_angle.y);
    f32 f24 = 100.0f;
    f32 f31 = 100.0f; /* the distance limit, GameCube f31 */
    s16 m08 = l_himo2HIO.m08;
    s16 reg = gabi::load<s16>(HIMO2_REG(0x62A));
    s16 r27 = (s16)(0x4000 - m08 - reg);
    s16 r26 = (s16)(m08 + reg + 0x4000);
    f32 yTol = 0.0f;
    u32 n;
    if (himo2_isStartStage(0x10010DE4 /* "ADMumi" */)) {
        r26 = (s16)(r26 - 10000);
        r27 = (s16)(r27 + 10000);
        yTol = gabi::load<f32>(HIMO2_REG(0x5A8)) + -600.0f;
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
                        u32 camera = gabi::load<u32>(dComIfGp_ea() + 0x5AF8); /* dComIfGp_getCamera(0) */
                        f32 ez = gabi::load<f32>(camera + 0xE4);
                        f32 ex = gabi::load<f32>(camera + 0xDC);
                        f32 s18z = r28->current.pos.z - ez;
                        f32 s24x = px - ex;
                        f32 s24z = pz - ez;
                        f32 s18x = r28->current.pos.x - ex;
                        f32 ey = gabi::load<f32>(camera + 0xE0);
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

/* 0216EEB8 himo2_class::setTargetPos (matcher: search_target, which it inlines) */
static BOOL himo2_setTargetPos(himo2_class* i_this, cXyz* param_1, be<f32>* param_2, be<f32>* param_3) {
    WWHD_FUNC(0x0216EEB8, BOOL, i_this, param_1, param_2, param_3);
    *param_2 = -1.0f;
    *param_3 = -1.0f;
    /* m2524 = *param_1 (word copy), then read back as the by-value argument of search_target */
    u32 d = gabi::ea(&i_this->m2524), s = gabi::ea(param_1);
    gabi::store<u32>(d + 0, gabi::load<u32>(s + 0));
    gabi::store<u32>(d + 4, gabi::load<u32>(s + 4));
    f32 px = i_this->m2524.x;
    gabi::store<u32>(d + 8, gabi::load<u32>(s + 8));
    f32 py = i_this->m2524.y;
    f32 pz = i_this->m2524.z;
    fopAc_ac_c* t = search_target(i_this, px, py, pz);
    i_this->m217C = t;
    if (t == nullptr)
        return FALSE;
    if ((fopAcM_GetParam(t) & 0xF0) != 0) {
        *param_2 = 700.0f;
        *param_3 = 100.0f;
    } else {
        *param_2 = (f32)((fopAcM_GetParam(t) >> 0x10) & 0x0F) * 100.0f;
        *param_3 = (f32)((fopAcM_GetParam(i_this->m217C) >> 0x14) & 0x0F) * 100.0f;
        if (!(*param_2 < 1500.0f))
            *param_2 = -1.0f;
        if (!(*param_3 < 1500.0f))
            *param_3 = -1.0f;
    }
    return TRUE;
}
VERIFY(0x0216EEB8, himo2_setTargetPos);

/* ---- Execute ---- */

static inline void word_copy3(u32 d, u32 s) {
    gabi::store<u32>(d + 0, gabi::load<u32>(s + 0));
    gabi::store<u32>(d + 4, gabi::load<u32>(s + 4));
    gabi::store<u32>(d + 8, gabi::load<u32>(s + 8));
}
/* daPy_py_c virtuals (HD vtable at +0xB4): +0x14 getLeftHandMatrix, +0x1C getRightHandMatrix */
static inline Mtx34* player_vmtx(fopAc_ac_c* p, u32 slot) { return gabi::call_ptr<Mtx34*>(gabi::load<u32>(p->__vtbl + slot), p); }

/* himo2_control (inline): from the hook end towards the player */
static inline void himo2_control(himo2_class* i_this, himo2_s* param_2) {
    gabi::Local<cXyz> local_68;
    gabi::Local<cXyz> local_74;
    local_68->x = 0.0f;
    local_68->y = 0.0f;
    if (i_this->m02DC == 0) {
        local_68->z = (15.625f * i_this->m2188) * (gabi::load<f32>(HIMO2_REG(0x4D0)) + 0.002f); /* REG8_F(18) */
    } else {
        local_68->z = (15.625f * i_this->m2188) * (gabi::load<f32>(HIMO2_REG(0xC)) + 0.0007f); /* REG0_F(1) */
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
        local_a8->z = (15.625f * m2188) * (gabi::load<f32>(HIMO2_REG(0x4D0)) + 0.002f);
    } else {
        local_a8->z = (15.625f * m2188) * (gabi::load<f32>(HIMO2_REG(0xC)) + 0.0007f);
    }
    f32 sinT = 0.0f; /* GameCube f29 */
    f32 cosT = 0.0f; /* GameCube f28 */
    f32 f1 = gabi::load<f32>(HIMO2_REG(0x4CC)) + 10.0f; /* REG8_F(17) */
    s32 iVar4 = cc + 1;
    for (; iVar4 < 100; iVar4++, param_2--) {
        f32 f30 = param_2->m10.y - param_2[1].m10.y;
        if ((u32)dc < 4) { /* m02DC >= 0 && m02DC <= 3 */
            if (dc != 0)
                f1 = (i_this->m2530 * (f32)(s32)(99 - iVar4)) * 0.01f;
            u32 rs = HIMO2_REG(0x4 + 0xAA6);
            s32 a1 = (s32)i_this->m02D8 * (gabi::load<s16>(rs) + 700) + iVar4 * (gabi::load<s16>(rs + 2) + 2000);
            s32 a2 = (s32)i_this->m02D8 * (gabi::load<s16>(rs + 4) + 500) + iVar4 * (gabi::load<s16>(rs + 6) + 2000);
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
    WWHD_FUNC(0x0216E194, BOOL, i_this);
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER));
    if (himo2_btd == 0)
        himo2_btd = gabi::ea(fpcM_Search(0x0216DE2C /* b_a_sub */, i_this));
    if (himo2_dr == 0)
        himo2_dr = gabi::ea(fpcM_Search(0x0216DDDC /* dr_a_sub */, i_this));
    i_this->m02D8 = i_this->m02D8 + 1;
    rope_scale = l_himo2HIO.m1C;
    gabi::Local<cXyz> local_c8;
    local_c8->y = 0.0f;
    local_c8->x = 4.0f;
    local_c8->z = 0.0f;
    if (gabi::load<u32>(gabi::ea(player) + 0x3C0) & 4) { /* player->getRopeGrabRightHand() */
        PSMTXCopy(player_vmtx(player, 0x14), calc_mtx());
        MtxPosition(local_c8, &i_this->m02EC[1]);
        PSMTXCopy(player_vmtx(player, 0x1C), calc_mtx());
        MtxPosition(local_c8, &i_this->m02EC[0]);
    } else {
        PSMTXCopy(player_vmtx(player, 0x14), calc_mtx());
        MtxPosition(local_c8, &i_this->m02EC[0]);
        PSMTXCopy(player_vmtx(player, 0x1C), calc_mtx());
        MtxPosition(local_c8, &i_this->m02EC[1]);
    }
    word_copy3(gabi::ea(&i_this->m02B4), gabi::ea(&i_this->m02EC[0]));
    s32 iVar5 = i_this->m02CC + -50;
    if (iVar5 < 3)
        iVar5 = 3;

    gabi::Local<dBgS_GndChk> gndChk;
    dBgS_GndChk_ct(gndChk, dBgS_GndChk_vt{0x10010E34, 0x10010E44, 0x10010E64, 0x10010E54}, false);
    if (iVar5 <= 0)
        iVar5 = 1;
    himo2_s* phVar3 = &i_this->m1120[0];
    do {
        if (i_this->m02DC != 2) {
            u32 g = gabi::ea(gndChk.get());
            f32 z = phVar3->m10.z;
            f32 y = phVar3->m10.y;
            f32 x = phVar3->m10.x;
            gabi::store<f32>(g + 0x2C, z); /* gndChk.SetPos(&pos), pos.y += 60.0f */
            gabi::store<f32>(g + 0x24, x);
            gabi::store<f32>(g + 0x28, y + 60.0f);
            f32 h = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
            phVar3->m0C = h + 5.0f;
        } else {
            phVar3->m0C = -100000.0f;
        }
        phVar3++;
    } while (--iVar5);

    gabi::call(0x0216F7FC, i_this); /* new_himo2_move */
    phVar3 = &i_this->m0310[0];
    for (s32 i = 0; i < i_this->m02CC + 1; i++, phVar3++)
        word_copy3(gabi::ea(&phVar3->m10), gabi::ea(&i_this->m02B4));
    phVar3 = &i_this->m0310[0];
    himo2_control(i_this, phVar3);
    word_copy3(gabi::ea(&i_this->m0310[99].m10), gabi::ea(&i_this->current.pos));
    himo2_control2(i_this, phVar3);
    if (i_this->m02DC == 2 || i_this->m02DC == 3) {
        i_this->m2050.SetR(20.0f);
    } else {
        i_this->m2050.SetR(-100.0f);
    }
    i_this->m2050.SetC(&i_this->current.pos);
    dComIfG_Ccsp_Set(&i_this->m2050);
    word_copy3(gabi::ea(&i_this->eyePos), gabi::ea(&i_this->current.pos));
    /* ~dBgS_GndChk */
    u32 g = gabi::ea(gndChk.get());
    gabi::store<u32>(g + 0x4C, 0x10010E24);
    gabi::store<u32>(g + 0x40, 0x10010E64);
    gabi::store<u32>(g + 0x20, 0x10010E44);
    gabi::call(0x02008DAC, gndChk.get(), 0);
    return TRUE;
}
VERIFY(0x0216E194, daHimo2_Execute);

/* ---- Draw ---- */

/* mDoExt_3DlineMat1_c (HD): vtable at +0x130, line array at +0x184 (0x10 per line, segment
 * positions first) */
static inline u32 lineMat_getPos(mDoExt_3DlineMat1_c_l* m, s32 i) { return gabi::load<u32>(gabi::load<u32>(gabi::ea(m) + 0x184) + i * 0x10); }
/* 025EC62C mDoExt_3DlineMat1_c::update(u16 n, f32 width, GXColor&, u16, dKy_tevstr_c*) */
static inline void lineMat_update(mDoExt_3DlineMat1_c_l* m, u16 n, f32 w, dKy_tevstr_c* tev) {
    gabi::call(0x025EC62C, m, n, w, (u32)0x101B6E14 /* {200, 150, 50, 255} */, (u16)0, tev);
}
/* dComIfGd_set3DlineMat (HD): play+0x5FB4 holds the sort packets (0x9C each), indexed by the
 * line's material id (virtual +0x14); 025EDD04 mDoExt_3DlineMatSortPacket::setMat */
static inline void dComIfGd_set3DlineMat_l(mDoExt_3DlineMat1_c_l* m) {
    u32 play = dComIfGp_ea();
    u32 vt = gabi::load<u32>(gabi::ea(m) + 0x130);
    s32 id = gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), m);
    gabi::call(0x025EDD04, play + 0x5FB4 + id * 0x9C, m);
}
/* HD: after each line, its packet gets the tev struct and 02415ABC(play+0x5B34 object, packet, line) */
static inline void himo2_hd_line_packet(himo2_class* i_this, u32 tevField, s32 packet, mDoExt_3DlineMat1_c_l* m) {
    gabi::store<u32>(gabi::ea(i_this) + tevField, gabi::ea(&i_this->tevStr));
    u32 obj = gabi::load<u32>(dComIfGp_ea() + 0x5B34);
    gabi::call(0x02415ABC, obj, i_this->mPackets[packet], m);
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
    s16 r21 = 0; /* GameCube r26 */
    fopAc_ac_c* t = i_this->m217C;
    s16 r20 = i_this->m2510; /* GameCube r23 */
    if (t == nullptr) /* HD */
        return;
    f32 sy = t->scale.y;
    f32 d = sy - 1.0f;
    f32 f5 = -7.0f * d;
    f32 f4 = 8.0f * d;
    if ((fopAcM_GetParam(t) & 0x0F) == 3) {
        f32 a = gabi::load<f32>(HIMO2_REG(0x4A8)) + -3.0f;
        f32 b = gabi::load<f32>(HIMO2_REG(0x4AC)) + 1.5f;
        f4 = f4 + a;
        f5 = f5 + b;
    }
    if (sy < 0.7f)
        f4 = f4 + 3.0f;
    /* HD: always the m251C == 0 side (GameCube: 700 and -15 when m251C != 0) */
    gabi::Local<cXyz> local_504;
    gabi::Local<cXyz> local_510;
    local_504->x = gabi::fmadds(gabi::load<f32>(HIMO2_REG(0x8)), 10.0f, 15.0f) + i_this->m24B8;
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
    u32 r29 = lineMat_getPos(&i_this->m1F30, 0) + i_this->m1F6C * 12;
    gabi::Local<cXyz[100]> local_4f8;
    u32 l4f8 = gabi::ea(local_4f8.get());
    for (s32 i = 0; i < 100; i++) {
        gabi::store<f32>(l4f8 + i * 12 + 4, -200000.0f);
        s16 r18 = r22; /* GameCube r22 */
        if (i < 50) {
            r22 = (s16)(r22 + (gabi::load<s16>(HIMO2_REG(0xAA2)) + 100));
        } else if (i <= i_this->m24BC + 49) {
            s16 rs4 = gabi::load<s16>(HIMO2_REG(0x88));
            r22 = (s16)(r22 + i_this->m24C8);
            fopAc_ac_c* tt = i_this->m217C;
            if (i >= 95 - rs4)
                r22 = (s16)(r22 + (gabi::load<s16>(HIMO2_REG(0x8A)) + -1000));
            if (tt != nullptr && (fopAcM_GetParam(tt) & 0xF0) != 0)
                r21 = (s16)(r21 + (rs4 + -400));
        } else {
            if (i_this->m24BC == 0)
                r22 = (s16)(r22 + (gabi::load<s16>(HIMO2_REG(0xAA4)) + 70));
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
        if ((u32)i == (u32)(gabi::load<s16>(HIMO2_REG(0x8C)) + 48)) {
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
                gabi::store<f32>(l4f8 + i * 12 + 4, py);
                gabi::store<f32>(l4f8 + i * 12 + 8, pz);
                gabi::store<f32>(l4f8 + i * 12 + 0, px);
            }
        }
    }
    for (int i = 99; i >= 0; i--) {
        f32 y = gabi::load<f32>(l4f8 + i * 12 + 4);
        if (y > -100000.0f) {
            gabi::store<f32>(r29 + 4, y);
            gabi::store<f32>(r29 + 0, gabi::load<f32>(l4f8 + i * 12 + 0));
            gabi::store<f32>(r29 + 8, gabi::load<f32>(l4f8 + i * 12 + 8));
            i_this->m1F6C = i_this->m1F6C + 1;
            r29 += 12;
        }
    }
}

/* himo2_draw (inline) */
static inline void himo2_draw(himo2_class* i_this, himo2_s* param_2) {
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER));
    u32 pcVar6 = lineMat_getPos(&i_this->m1F30, 0) + i_this->m1F6C * 12;
    word_copy3(pcVar6, gabi::ea(&i_this->m02B4));
    s32 r30 = i_this->m02CC;
    i_this->m1F6C = i_this->m1F6C + 1;
    pcVar6 += 12;
    himo2_s* phVar4 = &param_2[r30 + 1];
    for (; r30 < 98; r30++, phVar4++) {
        if (r30 == 97) {
            if (i_this->m02DC == 0 && i_this->m2188 < gabi::load<f32>(HIMO2_REG(0x14)) + 50.0f) {
                PSMTXCopy(player_vmtx(player, 0x14) /* getLeftHandMatrix */, calc_mtx());
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
            u32 obj = gabi::load<u32>(dComIfGp_ea() + 0x5B34);
            gabi::call(0x023D9340, obj, i_this->mPackets[0], hookModel);
        } else {
            word_copy3(pcVar6, gabi::ea(&phVar4->m10));
            i_this->m1F6C = i_this->m1F6C + 1;
            pcVar6 += 12;
        }
    }
}

/* himo_hang_draw (inline) */
static inline void himo_hang_draw(himo2_class* i_this) {
    u32 pcVar3 = lineMat_getPos(&i_this->m1F30, 0) + i_this->m1F6C * 12;
    gabi::Local<cXyz> local_38;
    gabi::call(0x0201ADE0, &i_this->m02EC[0], local_38.get(), &i_this->m2504); /* cXyz::operator- */
    local_38->x = local_38->x * 0.25f;
    local_38->y = local_38->y * 0.25f;
    local_38->z = local_38->z * 0.25f;
    for (int uVar2 = 0; uVar2 < 5; uVar2++, pcVar3 += 12) {
        f32 u = (f32)uVar2;
        gabi::store<f32>(pcVar3 + 0, gabi::fmadds(local_38->x, u, i_this->m2504.x));
        gabi::store<f32>(pcVar3 + 4, gabi::fmadds(local_38->y, u, i_this->m2504.y));
        gabi::store<f32>(pcVar3 + 8, gabi::fmadds(local_38->z, u, i_this->m2504.z));
        i_this->m1F6C = i_this->m1F6C + 1;
    }
}

/* 0216CA5C daHimo2_Draw (inlines spin_draw, himo2_disp, himo2_draw, himo_hang_draw) */
static BOOL daHimo2_Draw(himo2_class* i_this) {
    WWHD_FUNC(0x0216CA5C, BOOL, i_this);
    if (i_this->m24D9 < 0 || i_this->m029E != 0)
        return TRUE;
    if (i_this->m02DC == 0) {
        fopAc_ac_c* pl = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER));
        if (gabi::call_ptr<u32>(gabi::load<u32>(pl->__vtbl + 0xCC), pl) != 0) /* checkPlayerNoDraw */
            return TRUE;
        pl = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER));
        if (gabi::call_ptr<u32>(gabi::load<u32>(pl->__vtbl + 0x3C), pl) != 0) /* checkPlayerGuard */
            return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    i_this->m1F6C = 0;
    /* dComIfGd_setListP1() */
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D58));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D60));
    if (i_this->m24D8 != 0)
        spin_draw(i_this);
    if (i_this->m24D9 == 0 || i_this->m24D9 >= 3) {
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

    u32 r19 = lineMat_getPos(&i_this->m1F30, 0);
    gabi::Local<cXyz[200]> local_a10;
    u32 la = gabi::ea(local_a10.get());
    s32 n = i_this->m1F6C;
    for (int i = 0; i < n; i++) {
        word_copy3(la + i * 12, r19);
        r19 += 12;
    }
    for (int i = 0; i < i_this->m1F6C; i++) {
        r19 -= 12;
        word_copy3(r19, la + i * 12);
    }
    lineMat_update(&i_this->m1F30, (u16)i_this->m1F6C, rope_scale, &i_this->tevStr);
    dComIfGd_set3DlineMat_l(&i_this->m1F30);
    himo2_hd_line_packet(i_this, 0x2978, 1, &i_this->m1F30);

    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER));
    mDoMtx_YrotS(calc_mtx(), (s16)-player->shape_angle.y);
    gabi::Local<cXyz> sp44;
    gabi::Local<cXyz> sp38;
    {
        gabi::Local<cXyz> tmp;
        gabi::call(0x0201ADE0, &i_this->m02EC[1], tmp.get(), &i_this->m1F84); /* cXyz::operator- */
        word_copy3(gabi::ea(sp44.get()), gabi::ea(tmp.get()));
    }
    MtxPosition(sp44, sp38);
    sp38->z = sp38->z * (gabi::load<f32>(HIMO2_REG(0x1C)) + 500.0f);
    if (sp38->z > 16384.0f) {
        sp38->z = 16384.0f;
    } else if (sp38->z < -16384.0f) {
        sp38->z = -16384.0f;
    }
    cLib_addCalcAngleS2(&i_this->m1F90, (s16)gabi::ftoi(sp38->z), 2, (s16)(gabi::load<s16>(HIMO2_REG(0x84)) + 0x800));
    sp38->x = sp38->x * (gabi::load<f32>(HIMO2_REG(0x20)) + -200.0f);
    f32 f1 = gabi::load<f32>(HIMO2_REG(0x24)) + 4000.0f;
    if (sp38->x > f1)
        sp38->x = f1;
    if (sp38->x < -f1)
        sp38->x = -f1;
    cLib_addCalcAngleS2(&i_this->m1F94, (s16)gabi::ftoi(sp38->x), 2, (s16)(gabi::load<s16>(HIMO2_REG(0x84)) + 0x800));
    if (sp38->y < gabi::load<f32>(HIMO2_REG(0x28)) + -20.0f) {
        cLib_addCalcAngleS2(&i_this->m1F92, -0x8000, 2, 0x1000);
    } else {
        cLib_addCalcAngleS2(&i_this->m1F92, 0, 2, 0x1000);
    }
    word_copy3(gabi::ea(&i_this->m1F84), gabi::ea(&i_this->m02EC[1]));
    f32 f29 = gabi::fmadds(i_this->m2188, gabi::load<f32>(HIMO2_REG(0x2C)) + 3.0f, gabi::load<f32>(HIMO2_REG(0x30))) - 120.0f;
    sp44->z = 0.0f;
    sp44->x = 0.0f;
    for (int i = 0; i < 5; i++) {
        f32 lim = (f32)(4 - i) * 300.0f;
        u32 r16 = lineMat_getPos(&i_this->m1F98, i);
        if (f29 > lim) {
            cLib_addCalc0(&i_this->m1F70[i], 1.0f, gabi::load<f32>(HIMO2_REG(0x34)) + 0.2f);
        } else {
            cLib_addCalc2(&i_this->m1F70[i], 1.0f, 1.0f, gabi::load<f32>(HIMO2_REG(0x38)) + 0.1f);
        }
        sp44->y = i_this->m1F70[i] * 15.0f;
        mDoMtx_YrotS(calc_mtx(), player->shape_angle.y);
        f32 k = 1.0f - i_this->m1F70[i];
        s16 m1F94 = i_this->m1F94;
        s16 r6 = (s16)gabi::ftoi(k * (gabi::load<f32>(HIMO2_REG(0x3C)) + -20000.0f));
        s16 m1F92 = i_this->m1F92;
        mDoMtx_ZrotM(calc_mtx(), (s16)((m1F94 + -2000) * i + m1F92 + r6)); /* HD: no REG0_S(0) */
        mDoMtx_XrotM(calc_mtx(), i_this->m1F90);
        MtxTrans(0.0f, -sp44->y, 0.0f, 1);
        for (int j = 0; j < 32; j++, r16 += 12) {
            mDoMtx_XrotM(calc_mtx(), 0x800);
            MtxPosition(sp44, gabi::at<cXyz>(r16));
            gabi::call(0x028E8D88, gabi::at<cXyz>(r16), &i_this->m02EC[1], gabi::at<cXyz>(r16)); /* PSVECAdd (operator+=) */
        }
    }
    lineMat_update(&i_this->m1F98, 0x20, rope_scale, &i_this->tevStr);
    dComIfGd_set3DlineMat_l(&i_this->m1F98);
    himo2_hd_line_packet(i_this, 0x28C8, 2, &i_this->m1F98); /* HD stores the tev struct into packet 0's field */

    u32 r19b = lineMat_getPos(&i_this->m1FD8, 0);
    f32 step = gabi::load<f32>(HIMO2_REG(0x44));
    f32 f1_2;
    if ((gabi::ftoi(i_this->m2188) & (gabi::load<s16>(HIMO2_REG(0x88)) + 64)) != 0)
        f1_2 = gabi::load<f32>(HIMO2_REG(0x40)) + 15.0f;
    else
        f1_2 = 0.0f;
    cLib_addCalc2(&i_this->m1FD4, f1_2, 1.0f, step + 4.0f);
    gabi::Local<cXyz> sp2C;
    gabi::call(0x0201ADE0, &i_this->m02EC[0], sp2C.get(), &i_this->m02EC[1]); /* cXyz::operator- */
    Mtx34* cm = calc_mtx();
    s16 a = cM_atan2s(sp2C->x, sp2C->z);
    mDoMtx_YrotS(cm, (s16)(a - 0x4000)); /* HD: no REG0_S(0) */
    f32 m1FD4 = i_this->m1FD4;
    f32 y = (gabi::load<f32>(HIMO2_REG(0x24)) + -10.0f) - m1FD4;
    f32 z = (gabi::load<f32>(HIMO2_REG(0x28)) + 10.0f) + m1FD4;
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
        gabi::store<f32>(r19b + 0, gabi::fmadds(sp38->x, fVar1, gabi::fmadds(sp2C->x, fi, i_this->m02EC[1].x)));
        gabi::store<f32>(r19b + 4, gabi::fmadds(sp38->y, fVar1, gabi::fmadds(sp2C->y, fi, i_this->m02EC[1].y)));
        gabi::store<f32>(r19b + 8, gabi::fmadds(sp38->z, fVar1, gabi::fmadds(sp2C->z, fi, i_this->m02EC[1].z)));
        r19b += 12;
    }
    lineMat_update(&i_this->m1FD8, 16, rope_scale, &i_this->tevStr);
    dComIfGd_set3DlineMat_l(&i_this->m1FD8);
    himo2_hd_line_packet(i_this, 0x28C8, 3, &i_this->m1FD8);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x0216CA5C, daHimo2_Draw);
