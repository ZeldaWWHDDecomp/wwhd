/**
 * d_a_bdk_exe.cpp (WWHD)
 * Boss - Helmaroc King (battle): daBdk_Execute (damage_check, end_set, col_set, tail_control and
 * kamen_break_move are inlined into it).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bdk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bdk.h"

using gabi::fadds_ppc;
using gabi::fmuls_ppc;
using gabi::fsubs_ppc;

/* ---- local bindings (SHARED-CANDIDATE) ---- */
struct CcAtInfo_bdk {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
static inline u32 at_power_check_bdk(CcAtInfo_bdk* info) { return gabi::call<u32>(0x02518DB0, info); }
static inline u32 cc_at_check_bdk(fopAc_ac_c* a, CcAtInfo_bdk* info) { return gabi::call<u32>(0x025192A8, a, info); }
static inline void def_se_set_bdk(fopAc_ac_c* a, u32 obj, u32 se) { gabi::call(0x02518CC8, a, obj, se); }
static inline void dKy_SordFlush_set_bdk(cXyz* pos, s32 v) { gabi::call(0x0255F554, pos, v); }
static inline void mDoAud_bgmStop_bdk(u32 t) { gabi::call(0x025E1904, t); }
static inline void cM3dGSph_SetR_bdk(void* sph, f32 r) { gabi::call(0x02018C8C, sph, r); }
static inline void cM3dGSph_SetC_bdk(void* sph, cXyz* c) { gabi::call(0x02018D40, sph, c); }
static inline void ccs_Set_bdk(void* obj) { cCcS_Set(dComIfG_Ccsp(), obj); }
static inline f32 bdk_REGF(u32 a) { return gabi::load<f32>(a); }
static inline s16 bdk_REGS(u32 a) { return gabi::load<s16>(a); }
static inline void bdk_vbits(void* d, const void* s) {
    for (int k = 0; k < 12; k += 4) gabi::store<u32>(gabi::ea(d) + k, gabi::load<u32>(gabi::ea(s) + k));
}
static inline f32 bdk_GroundCross(void* chk) { return gabi::call<f32>(0x02008974, dComIfG_Bgsp(), chk); }
static inline BOOL bdk_LineCross(void* chk) { return gabi::call<BOOL>(0x02008860, dComIfG_Bgsp(), chk); }
static const dBgS_GndChk_vt bdk_gnd_vt = {0x10007E98, 0x10007EA8, 0x10007EC8, 0x10007EB8};
static const dBgS_LinChk_vt bdk_lin_vt = {0x10007F08, 0x10007F18, 0x10007F38, 0x10007F28};
static inline void bdk_chk_dt(void* lin, void* gnd) {
    u32 l = gabi::ea(lin), g = gabi::ea(gnd);
    gabi::store<u32>(l + 0x58, 0x10007F38);
    gabi::store<u32>(l + 0x64, 0x10007E88);
    gabi::store<u32>(l + 0x20, 0x10007E78);
    gabi::call(0x02008B4C, l, 0); /* cBgS_LinChk::~cBgS_LinChk */
    gabi::store<u32>(g + 0x40, 0x10007EC8);
    gabi::store<u32>(g + 0x4C, 0x10007E88);
    gabi::store<u32>(g + 0x20, 0x10007EA8);
    gabi::call(0x02008DAC, g, 0); /* cBgS_Chk::~cBgS_Chk */
}
/* dCcD_GObjInf flag words: +0 (at set), +0x18 (tg set), +0x94 (tg shield); dCcD_Sph's cM3dGSph at +0x118,
 * the tg hit position at +0xCC, at spl at +0x6F */
#define SPH(p) gabi::ea(p)
#define bdk_foot_eff_pos(i) (*gabi::at<cXyz>(0x104618D0 + 0xC * (i)))
/* kamen-break (HD): four s16[4] arrays at 0xAC8 (yaw offset), 0xAD0 (its target), 0xAD8 (roll
 * offset), 0xAE0 (its target) in place of the GameCube m9B8/m9D0 */
static inline be<s16>& kb_yaw(bdk_class* t, s32 i) { return *gabi::at<be<s16>>(gabi::ea(t) + 0xAC8 + 2 * i); }
static inline be<s16>& kb_yaw_t(bdk_class* t, s32 i) { return *gabi::at<be<s16>>(gabi::ea(t) + 0xAD0 + 2 * i); }
static inline be<s16>& kb_roll(bdk_class* t, s32 i) { return *gabi::at<be<s16>>(gabi::ea(t) + 0xAD8 + 2 * i); }
static inline be<s16>& kb_roll_t(bdk_class* t, s32 i) { return *gabi::at<be<s16>>(gabi::ea(t) + 0xAE0 + 2 * i); }
static inline s16 z_d(s32 i) { return gabi::load<s16>(0x10190654 + 2 * i); }
static inline s16 z_d2(s32 i) { return gabi::load<s16>(0x1019065C + 2 * i); }
static inline f32 kb_off_x(s32 i) { return gabi::load<f32>(0x10190668 + 4 * i); }
static inline f32 kb_off_y(s32 i) { return gabi::load<f32>(0x10190678 + 4 * i); }
static inline f32 kb_off_z(s32 i) { return gabi::load<f32>(0x10190688 + 4 * i); }
static inline GXColor* bdk_colorK0_l(fopAc_ac_c* a) { return gabi::at<GXColor>(gabi::ea(a) + 0x1A8); }

/* damage_check (inlined). HD: no pause timers; a REG switch skips the mask-break demo count; the
 * spin-attack (type 1) hit cools down for 3 frames, a hammer hit for 12; a damage with the
 * ground flag sets the wait action and state 10; a REG switch keeps 4 health */
static inline void damage_check(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::call(0x02515E50, gabi::ea(&i_this->mStts) + 0x1C); /* dCcD_GStts::Move */
    gabi::Local<CcAtInfo_bdk> atInfo;
    u8 hit_type = 0;
    atInfo->pParticlePos = 0;
    if (i_this->m8F8 < 4) {
        if (!i_this->mHeadTgSph.ChkTgHit()) return;
        hit_type = 1;
        atInfo->mpObj = gabi::ea(i_this->mHeadTgSph.GetTgHitObj());
        atInfo->pParticlePos = gabi::ea(&i_this->mHeadTgSph) + 0xCC;
    } else {
        if (!i_this->mTosakaTgSph.ChkTgHit()) return;
        hit_type = 2;
        atInfo->mpObj = gabi::ea(i_this->mTosakaTgSph.GetTgHitObj());
        atInfo->pParticlePos = gabi::ea(&i_this->mTosakaTgSph) + 0xCC;
    }
    if (i_this->m2F8 != 0) return;
    at_power_check_bdk(atInfo);
    if (hit_type == 1) {
        i_this->m2F8 = 0xC;
        if (atInfo->mResultingAttackType == 9 && i_this->m1150.y < 10000.0f) {
            gabi::store<s8>(gabi::ea(actor) + 0x3A1, 0x14); /* health */
            mDoAud_seStart(0x2855 /* JA_SE_LK_HAMMER_HIT */, &actor->eyePos, 0x32, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
            if (gabi::load<u8>(gabi::ea(player) + 0x3AC) == 0x12) { /* HD */
                gabi::store<u16>(gabi::ea(player) + 0x69EC, 0xB);
                gabi::store<s16>(gabi::ea(player) + 0x69EA, (s16)(REG_S(18, 7) + 0xFA0));
            }
            if (i_this->m2B4 == 1) {
                i_this->mAction = 0x67; /* ACTION_T_DOWN */
                i_this->mState = 0;
                i_this->m2FA = (s16)(REG0_S(3) + 10);
                i_this->m2F8 = 0x1E;
                bdk_monsSeStart_e(actor, 0x486A /* JA_SE_CV_DK_LAST_DAMAGE */, 0);
                mDoAud_bgmStop_bdk(30);
            } else {
                dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), 0x3C01);
                u8 n = (u8)(i_this->m8F8 + 1);
                i_this->m8F8 = n;
                if (REG_S(10, 1) != 0 || n == 4) {
                    i_this->m8F8 = 3;
                    i_this->mAction = 9; /* ACTION_KAMEN_DEMO */
                    i_this->mState = 0;
                    bdk_seStart(actor, 0x5875 /* JA_SE_CM_DK_BREAK_MASK */, 0);
                } else {
                    i_this->m2FA = (s16)(REG0_S(3) + 10);
                    i_this->m2EC[2] = 0xF;
                    i_this->m2F8 = 0x1E;
                }
            }
            i_this->m1138 = 0;
            gabi::Local<cXyz> pos;
            gabi::Local<csXyz> angle;
            pos->x = 2.0f;
            pos->y = 2.0f;
            pos->z = 2.0f;
            angle->x = 0;
            angle->z = 0;
            angle->y = (s16)(player->shape_angle.y + 0x8000);
            cXyz* hp = gabi::at<cXyz>(gabi::ea(&i_this->mHeadTgSph) + 0xCC);
            dComIfGp_particle_set(0xD /* ID_AK_JN_OK */, hp, angle, pos, 0xFF);
            dComIfGp_particle_set(0x8129 /* ID_IT_SN_DK_KAMEN_HAHEN00 */, hp, &actor->shape_angle, nullptr, 0xFF);
            gabi::Local<cXyz> fp;
            bdk_vbits(fp.get(), &actor->current.pos);
            dKy_SordFlush_set_bdk(fp, 1);
            bdk_StartShock(5);
        } else {
            def_se_set_bdk(actor, atInfo->mpObj, 0x40);
        }
        return;
    }
    /* hit_type 2 */
    u8 type = atInfo->mResultingAttackType;
    if (type == 9) {
        i_this->m2F8 = 0xC;
    } else {
        i_this->m2F8 = type == 1 ? 3 : 6;
    }
    atInfo->mpActor = cc_at_check_bdk(actor, atInfo);
    bdk_monsSeStart(actor, 0x4868 /* JA_SE_CV_DK_DAMAGE */, 0);
    i_this->m1138 = (s16)(REG0_S(5) + 0x1E);
    {
        f32 px = player->current.pos.x;
        f32 ex = actor->eyePos.x;
        f32 ez = actor->eyePos.z;
        f32 pz = player->current.pos.z;
        i_this->m1136 = cM_atan2s(fsubs_ppc(ex, px), fsubs_ppc(ez, pz));
    }
    if (i_this->mAction == 5 /* ACTION_KUTI_ATTACK */ && i_this->m1150.y < 10000.0f) {
        anm_init(i_this, 0x21 /* BCK_DAMAGE1 */, 2.0f, 0, 1.0f, -1, 0);
        s8 n = (s8)(i_this->m2594 + 1);
        i_this->mState = 10;
        i_this->m2594 = n;
        if (n >= 6) {
            i_this->m2EC[2] = 10;
            i_this->m2F8 = 0x14;
        }
    } else if (i_this->m2586 != 0) {
        if (i_this->mAction != 3) {
            i_this->mAction = 3; /* ACTION_WAIT */
        }
        i_this->mState = 10; /* HD */
        anm_init(i_this, 0x23 /* BCK_DAMAGE3 */, 2.0f, 0, 1.0f, -1, 1);
    } else {
        anm_init(i_this, 0x22 /* BCK_DAMAGE2 */, 2.0f, 0, 1.0f, 9 /* BAS_DAMAGE2 */, 1);
    }
    if (REG_S(10, 2) != 0) {
        gabi::store<s8>(gabi::ea(actor) + 0x3A1, 4); /* HD: health kept */
    } else if (!(gabi::load<s8>(gabi::ea(actor) + 0x3A1) > 0)) {
        /* end_set (inlined); HD: the pause timer is a static, play+0x5B44 cleared */
        gabi::store<f32>(dComIfGp_ea() + 0x5B44, 0.0f);
        gabi::store<u8>(0x101EACB7, 8);
        i_this->m261C[0x27].m000 = 0;
        i_this->mAction = 10; /* ACTION_END */
        if (actor->current.pos.y < 9810.0f) {
            i_this->m25A0 = 100;
            i_this->mState = (i_this->m1150.y < 10000.0f) ? 0 : 1;
        } else {
            i_this->m25A2 = 1;
            i_this->mState = 2;
            i_this->m25A0 = 100;
        }
        actor->actor_status |= 0x4000; /* fopAcStts_UNK4000_e */
        mDoAud_bgmStop_bdk(30);
        if (gabi::ea(&actor->eyePos) != 0) {
            bdk_monsSeStart_e(actor, 0x486A /* JA_SE_CV_DK_LAST_DAMAGE */, 0);
        }
        return;
    }
    JPABaseEmitter* emitter = dComIfGp_particle_set(0x8137 /* ID_IT_SN_DK_DMG_HANE_M00 */, &actor->current.pos, &actor->shape_angle, nullptr, 0xFF,
                                                    nullptr, (s8)fopAcM_GetRoomNo(actor), bdk_colorK0_l(actor), bdk_colorK0_l(actor));
    if (emitter != nullptr) {
        bdk_setGlobalRTMatrix(emitter, bdk_morfAnmMtx(i_this, 0x16 /* DK_JNT_J_DK_MUNE2_e */));
    }
}

/* col_set (inlined). HD: the body sphere is 400 in t_lastattack (250 otherwise, no REG); the foot
 * sphere radii use HD registers */
static inline void col_set(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    gabi::Local<cXyz> offset;
    {
        f32 y = actor->current.pos.y;
        bdk_fcopy(i_this->m1168.z, actor->current.pos.z);
        i_this->m1168.y = fadds_ppc(y, 380.0f);
        bdk_fcopy(i_this->m1168.x, actor->current.pos.x);
    }
    PSMTXCopy(bdk_morfAnmMtx(i_this, 0x18 /* DK_JNT_J_DK_ATAMA1_e */), calc_mtx());
    offset->x = fadds_ppc(REG8_F(0), 40.0f);
    bdk_fcopy(offset->y, *gabi::at<be<f32>>(0x1047BA94)); /* REG8_F(1) */
    if (i_this->m8F8 >= 4) {
        offset->z = 0.0f;
        cM3dGSph_SetR_bdk(gabi::at<void>(SPH(&i_this->mHeadTgSph) + 0x118), 80.0f);
    } else {
        offset->z = fadds_ppc(REG8_F(2), -70.0f);
        cM3dGSph_SetR_bdk(gabi::at<void>(SPH(&i_this->mHeadTgSph) + 0x118), 110.0f);
    }
    bdk_vbits(&i_this->m1144, &i_this->m1150);
    MtxPosition(offset, &i_this->m1150);
    if (i_this->m2584 == 1) {
        PSMTXCopy(bdk_morfAnmMtx(i_this, 0x19 /* DK_JNT_J_DK_AGO_e */), calc_mtx());
        bdk_fcopy(offset->z, *gabi::at<be<f32>>(0x1047BAA4)); /* REG8_F(5) */
        bdk_fcopy(offset->y, *gabi::at<be<f32>>(0x1047BAA0)); /* REG8_F(4) */
        offset->x = fadds_ppc(REG8_F(3), 120.0f);
        MtxPosition(offset, &i_this->m115C);
    } else {
        i_this->m115C.y = -10000.0f;
    }
    cM3dGSph_SetR_bdk(gabi::at<void>(SPH(&i_this->mHeadAtSph) + 0x118), fadds_ppc(REG8_F(6), 70.0f));
    cM3dGSph_SetR_bdk(gabi::at<void>(SPH(&i_this->mBodyCCSph) + 0x118), i_this->mAction == 0x66 ? 400.0f : 250.0f);
    cM3dGSph_SetC_bdk(gabi::at<void>(SPH(&i_this->mHeadAtSph) + 0x118), &i_this->m115C);
    cM3dGSph_SetC_bdk(gabi::at<void>(SPH(&i_this->mHeadTgSph) + 0x118), &i_this->m1150);
    cM3dGSph_SetC_bdk(gabi::at<void>(SPH(&i_this->mBodyCCSph) + 0x118), &i_this->m1168);
    ccs_Set_bdk(&i_this->mHeadAtSph);
    ccs_Set_bdk(&i_this->mHeadTgSph);
    ccs_Set_bdk(&i_this->mBodyCCSph);
    if (i_this->m8F8 >= 4) {
        PSMTXCopy(bdk_morfAnmMtx(i_this, REG_S(8, 2) + 0x21 /* DK_JNT_J_DK_TOSAKA_A3_e */), calc_mtx());
        gabi::Local<cXyz> dist;
        bdk_fcopy(offset->z, *gabi::at<be<f32>>(0x1047BAA4));
        bdk_fcopy(offset->y, *gabi::at<be<f32>>(0x1047BAA0));
        bdk_fcopy(offset->x, *gabi::at<be<f32>>(0x1047BA9C));
        MtxPosition(offset, dist);
        cM3dGSph_SetR_bdk(gabi::at<void>(SPH(&i_this->mTosakaTgSph) + 0x118), fadds_ppc(REG8_F(11), 80.0f));
        cM3dGSph_SetC_bdk(gabi::at<void>(SPH(&i_this->mTosakaTgSph) + 0x118), dist);
        ccs_Set_bdk(&i_this->mTosakaTgSph);
    }
    for (s32 i = 0; i < 2; i++) {
        PSMTXCopy(bdk_morfAnmMtx(i_this, i == 0 ? 5 /* DK_JNT_J_DK_ASHI_L3_e */ : 0xE /* DK_JNT_J_DK_ASHI_R3_e */), calc_mtx());
        u32 s = SPH(&i_this->mFootCCSph[i]);
        if (i_this->mAction == 0x66) {
            cM3dGSph_SetR_bdk(gabi::at<void>(s + 0x118), fadds_ppc(bdk_REGF(0x1047C038), 200.0f));
            gabi::store<u32>(s + 0x18, gabi::load<u32>(s + 0x18) & ~1u); /* OffTgSetBit */
            offset->x = fadds_ppc(REG0_F(3), 100.0f);
            offset->y = fadds_ppc(REG0_F(4), 200.0f);
            bdk_fcopy(offset->z, *gabi::at<be<f32>>(0x1047B624)); /* REG0_F(5) */
        } else {
            cM3dGSph_SetR_bdk(gabi::at<void>(s + 0x118), fadds_ppc(bdk_REGF(0x1047C03C), 80.0f));
            gabi::store<u32>(s + 0x18, gabi::load<u32>(s + 0x18) | 1u); /* OnTgSetBit */
            bdk_fcopy(offset->z, *gabi::at<be<f32>>(0x1047B624));
            bdk_fcopy(offset->y, *gabi::at<be<f32>>(0x1047B620)); /* REG0_F(4) */
            offset->x = fadds_ppc(REG0_F(3), 50.0f);
        }
        MtxPosition(offset, &i_this->m1174[i]);
        cM3dGSph_SetC_bdk(gabi::at<void>(s + 0x118), &i_this->m1174[i]);
        if (i_this->m2584 == 2) {
            gabi::store<u32>(s, gabi::load<u32>(s) | 1u); /* OnAtSetBit */
            s16 act = i_this->mAction;
            if (act == 7 || act == 0x64 || act == 6) {
                gabi::store<u8>(s + 0x6F, 9); /* SetAtSpl(dCcG_At_Spl_UNK9) */
            } else {
                gabi::store<u8>(s + 0x6F, 0);
            }
            if (i_this->m2585 == 0) {
                dCcD_Sph_StartCAt(&i_this->mFootCCSph[i], &i_this->m1174[i]);
                i_this->m2585 = i_this->m2585 + 1;
            } else {
                dCcD_Sph_MoveCAt(&i_this->mFootCCSph[i], &i_this->m1174[i]);
            }
        } else {
            gabi::store<u32>(s, gabi::load<u32>(s) & ~1u); /* OffAtSetBit */
            i_this->m2585 = 0;
        }
        ccs_Set_bdk(&i_this->mFootCCSph[i]);
    }
}

/* the head swing after the mask broke (inlined into Execute) */
static inline void head_swing(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->m113A == 0) {
        i_this->m1138 = 0;
        i_this->m1134 = 0;
        i_this->m1132 = 0;
        return;
    }
    {
        u32 f = SPH(&i_this->mHeadTgSph) + 0x94;
        gabi::store<u32>(f, gabi::load<u32>(f) & ~1u); /* OffTgShield */
    }
    cMtx_YrotS(calc_mtx(), (s16)-actor->shape_angle.y);
    gabi::Local<cXyz> offset;
    gabi::Local<cXyz> out;
    {
        gabi::Local<cXyz> tmp;
        cXyz_mi(&i_this->m1150, tmp, &i_this->m1144);
        bdk_vbits(offset.get(), tmp.get());
    }
    MtxPosition(offset, out);
    s16 angle = (s16)gabi::ftoi(fmuls_ppc(out->z, fadds_ppc(REG8_F(15), -200.0f)));
    if (angle > 8000) {
        angle = 8000;
    } else if (angle < -8000) {
        angle = -8000;
    }
    s32 k = REG_S(8, 4) + 2500;
    s16 m1130 = i_this->m1130;
    f32 f9 = fadds_ppc(REG0_F(5), -2000.0f);
    f32 f13 = fmuls_ppc(f9, i_this->m1140);
    f32 f0 = fmuls_ppc(4000.0f, i_this->m113C);
    s16 iVar4 = (s16)gabi::ftoi(fmuls_ppc(cM_ssin(m1130 * k), f13));
    s16 iVar11 = (s16)gabi::ftoi(f0);
    cLib_addCalc2(&i_this->m113C, 1.0f, 1.0f, fadds_ppc(REG0_F(4), 0.01f));
    if (i_this->m25A6 > REG0_S(9) + 0x41) {
        cLib_addCalc2(&i_this->m1140, 1.0f, 1.0f, fadds_ppc(REG0_F(2), 0.4f));
    }
    cLib_addCalcAngleS2(&i_this->m1120, (s16)(angle + iVar4 + 4500), 4, iVar11);
    cLib_addCalcAngleS2(&i_this->m1122, 4, 10, iVar11);
    s16 c = (s16)(i_this->m1130 + 1);
    i_this->m1130 = c;
    if (i_this->m1138 != 0) {
        f32 t = (f32)(s32)i_this->m1138;
        i_this->m1134 = 0;
        i_this->m1132 = 0;
        offset->x = 0.0f;
        offset->y = 0.0f;
        offset->z = fmuls_ppc(t, fadds_ppc(REG0_F(7), 500.0f));
        cMtx_YrotS(calc_mtx(), i_this->m1136);
        MtxPosition(offset, out);
        s16 m2C4 = i_this->m2C4;
        i_this->m112E = (s16)gabi::ftoi(fmuls_ppc(cM_ssin(m2C4 * (REG0_S(4) + 7000)), out->z));
        i_this->m112C = (s16)gabi::ftoi(fmuls_ppc(cM_ssin(m2C4 * (REG0_S(4) + 7000)), out->x));
        c = i_this->m1130;
    }
    if (i_this->m1134 > 3000) {
        c = (s16)(c + 1);
        i_this->m1130 = c;
    }
    {
        f32 m1132 = (f32)(s32)i_this->m1132;
        i_this->m1124 = (s16)gabi::ftoi(fmuls_ppc(cM_ssin(c * 1500), m1132));
        c = i_this->m1130;
        i_this->m1126 = (s16)gabi::ftoi(fmuls_ppc(cM_scos(c * 1200), m1132));
        i_this->m1128 = (s16)gabi::ftoi(fmuls_ppc(cM_ssin(c * 1500 + 0x4000), m1132));
        f32 m1132b = (f32)(s32)i_this->m1132;
        i_this->m112A = (s16)gabi::ftoi(fmuls_ppc(cM_scos(c * 1200 + 0x4000), m1132b));
    }
    cLib_addCalcAngleS2(&i_this->m1132, i_this->m1134, 2, 500);
    i_this->m1134 = 1500;
}

/* tail_control (inlined) */
static inline void tail_control(bdk_class* i_this, bdk_tail_s* tail) {
    gabi::Local<cXyz> sp10;
    gabi::Local<cXyz> sp28;
    {
        gabi::Local<cXyz> tmp;
        cXyz_mi(&tail->m0150[1], tmp, &tail->m0150[0]);
        bdk_vbits(sp10.get(), tmp.get());
    }
    tail->m0168.y = cM_atan2s(sp10->x, sp10->z);
    {
        f32 z = sp10->z;
        f32 x = sp10->x;
        f32 d = std_sqrtf(gabi::fmadds(x, x, fmuls_ppc(z, z)));
        tail->m0168.x = (s16)-cM_atan2s(sp10->y, d);
    }
    sp10->x = 0.0f;
    sp10->z = 30.0f;
    sp10->y = 0.0f;
    cMtx_YrotS(calc_mtx(), tail->m0168.y);
    cMtx_XrotM(calc_mtx(), tail->m0168.x);
    MtxPosition(sp10, &tail->m0170);
    bdk_vbits(&tail->m024[0], &tail->m0150[1]);
    f32 fVar2 = fadds_ppc(i_this->mAcch.GetGroundH(), 5.0f);
    if (i_this->mAction == 0xF /* ACTION_START */) {
        fVar2 = -30000.0f;
    }
    for (s32 i = 1; i < 10; i++) {
        cXyz* a24 = &tail->m024[i];
        cXyz* a24p = &tail->m024[i - 1];
        cXyz* d8 = &tail->m0D8[i];
        f32 dx0 = d8->x;
        f32 f8 = (f32)(i - 1);
        f32 dy0 = d8->y;
        f32 dz0 = d8->z;
        f32 f13 = gabi::fnmsubs(f8, 0.1f, 1.0f);
        f32 sy = gabi::fmadds(tail->m0170.y, f13, dy0);
        f32 sx = gabi::fmadds(tail->m0170.x, f13, dx0);
        f32 sz = gabi::fmadds(tail->m0170.z, f13, dz0);
        f32 fVar3 = fadds_ppc(a24->y, sy);
        f32 apz = a24p->z;
        f32 ax = a24->x;
        f32 az = a24->z;
        f32 apx = a24p->x;
        if (fVar3 < fVar2) {
            fVar3 = fVar2;
        }
        f32 dx = fadds_ppc(fsubs_ppc(ax, apx), sx);
        f32 dz = fadds_ppc(fsubs_ppc(az, apz), sz);
        f32 dy = fsubs_ppc(fVar3, a24p->y);
        s16 sVar2 = cM_atan2s(dx, dz);
        s16 sVar1 = (s16)-cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, fmuls_ppc(dz, dz))));
        tail->m09C[i - 1].y = sVar2;
        tail->m09C[i - 1].x = sVar1;
        f32 l = fmuls_ppc(fmuls_ppc(gabi::fadds_ppc(fmuls_ppc(20.0f, gabi::fmadds((f32)i, 0.03f, 0.25f)), fmuls_ppc(20.0f, gabi::fmadds((f32)i, 0.03f, 0.25f))),
                                    l_HIO.m00C),
                          l_HIO.m010);
        sp10->x = 0.0f;
        sp10->y = 0.0f;
        sp10->z = l;
        cMtx_YrotS(calc_mtx(), sVar2);
        cMtx_XrotM(calc_mtx(), sVar1);
        MtxPosition(sp10, sp28);
        bdk_vbits(d8, a24);
        f32 nx = fadds_ppc(a24p->x, sp28->x);
        a24->x = nx;
        a24->y = fadds_ppc(a24p->y, sp28->y);
        a24->z = fadds_ppc(a24p->z, sp28->z);
        d8->x = fmuls_ppc(fsubs_ppc(nx, d8->x), 0.77000004f);
        d8->y = fmuls_ppc(fsubs_ppc(a24->y, d8->y), 0.77000004f);
        d8->z = fmuls_ppc(fsubs_ppc(a24->z, d8->z), 0.77000004f);
    }
}

/* kamen_break_move (inlined). HD: the fragments turn towards random yaw/roll targets when hit,
 * bounce to 20 + REG (GameCube 14 + REG), keep their tilt; a line hit only stops them */
static inline void kamen_break_move(bdk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* HD: an unused play-object read */
    be<u32>& guard = *gabi::at<be<u32>>(0x104618C8);
    cXyz* non_pos = gabi::at<cXyz>(0x10461954);
    if (guard == 0) {
        guard = 1;
        non_pos->y = -50000.0f;
        non_pos->z = -50000.0f;
        non_pos->x = -50000.0f;
    }
    gabi::Local<u8[0x54]> gndChk;
    gabi::Local<u8[0x6C]> linChk;
    dBgS_GndChk_ct(gndChk.get(), bdk_gnd_vt, false);
    dBgS_LinChk_ct(linChk.get(), bdk_lin_vt, false);
    gabi::Local<cXyz> vec1;
    gabi::Local<cXyz> vec2;
    gabi::Local<cXyz> vec4;
    gabi::Local<cXyz> vec5;
    gabi::Local<CcAtInfo_bdk> atInfo;
    gabi::Local<u8[4]> rot; /* s16 rot2 at +0 (0x24), rot1 at +2 (0x26) */
    for (s32 i = 0; i < 4; i++) {
        dCcD_Sph* sph = &i_this->mA50[i];
        if (i_this->m90C[i] == 0) {
            cM3dGSph_SetC_bdk(gabi::at<void>(SPH(sph) + 0x118), non_pos);
            ccs_Set_bdk(sph);
            continue;
        }
        be<s16>* rots = gabi::at<be<s16>>(gabi::ea(rot.get()));
        rots[0] = 0;
        rots[1] = 0;
        cXyz* p = &i_this->m910[i];
        cXyz* v = &i_this->m970[i];
        if (i_this->m9E8[i] != 0) {
            s8 t = (s8)(i_this->m9E8[i] - 1);
            i_this->m9E8[i] = t;
            if (t < 0x14) {
                rots[1] = (s16)gabi::ftoi(fmuls_ppc(cM_ssin(t * 3300), 200.0f));
                rots[0] = (s16)gabi::ftoi(fmuls_ppc(cM_scos(t * 4800), 200.0f));
            }
        } else {
            bdk_vbits(&i_this->m940[i], p);
            PSVECAdd(p, v, p);
            v->y = fsubs_ppc(v->y, 5.0f);
            if (p->y < 8000.0f) {
                i_this->m90C[i] = 0;
            }
            {
                u32 g = gabi::ea(gndChk.get());
                f32 y = p->y;
                bdk_fbits(g + 0x24, gabi::ea(&p->x));
                gabi::store<f32>(g + 0x28, fadds_ppc(y, 200.0f));
                bdk_fbits(g + 0x2C, gabi::ea(&p->z));
            }
            f32 ground_y = fadds_ppc(fadds_ppc(bdk_GroundCross(gndChk.get()), 15.0f), REG0_F(11));
            if (!(p->y > ground_y)) {
                p->y = ground_y;
                if (v->y < fadds_ppc(REG0_F(2), -30.0f)) {
                    v->y = fadds_ppc(REG0_F(3), 20.0f);
                    i_this->m9EC[i] = fadds_ppc(REG0_F(4), 3000.0f);
                    i_this->m9A0[i].z = z_d2(i);
                    mDoAud_seStart(0x5815 /* JA_SE_CM_MAGBALL_BOUND */, p, 0x63, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                } else {
                    v->y = 0.0f;
                    s16 target = (s16)gabi::ftoi(fmuls_ppc(cM_ssin(i_this->m2C4 * 0x2000 + i * 0x3000), i_this->m9EC[i]));
                    cLib_addCalc0(&i_this->m9EC[i], 1.0f, fadds_ppc(REG0_F(5), 200.0f));
                    cLib_addCalcAngleS2(&i_this->m9A0[i].z, (s16)(z_d(i) + target), 1, 0x1000);
                }
                vec1->x = 0.0f;
                vec1->y = 0.0f;
                bdk_fcopy(vec1->z, i_this->m9FC[i]);
                cMtx_YrotS(calc_mtx(), i_this->mA0C[i]);
                MtxPosition(vec1, vec2);
                bdk_fcopy(v->x, vec2->x);
                bdk_fcopy(v->z, vec2->z);
                i_this->m9A0[i].y = (s16)(i_this->m9A0[i].y + (s16)gabi::ftoi(fmuls_ppc(i_this->mF00[i], i_this->m9FC[i])));
                cLib_addCalc0(&i_this->m9FC[i], 1.0f, fadds_ppc(REG0_F(5), 0.2f));
                if (sph->ChkTgHit() && i_this->m9FC[i] < 1.0f) {
                    atInfo->pParticlePos = 0;
                    atInfo->mpObj = gabi::ea(sph->GetTgHitObj());
                    atInfo->mpActor = at_power_check_bdk(atInfo);
                    if (atInfo->mResultingAttackType == 8) {
                        f32 b = fadds_ppc(REG0_F(4), 7.0f);
                        i_this->m9FC[i] = fadds_ppc(b, cM_rndF(2.0f));
                        u32 a = atInfo->mpActor;
                        if (a != 0) {
                            s16 r = (s16)gabi::ftoi(cM_rndFX(2000.0f));
                            i_this->mA0C[i] = (s16)(gabi::load<s16>(a + 0x32A) + r);
                        }
                        i_this->mF00[i] = (f32)(s32)(s16)gabi::ftoi(cM_rndFX(200.0f));
                    } else {
                        gabi::Local<cXyz> tmp;
                        cXyz_mi(p, tmp, gabi::at<cXyz>(SPH(sph) + 0xCC));
                        bdk_vbits(vec1.get(), tmp.get());
                        Mtx34* m = calc_mtx();
                        cMtx_YrotS(m, cM_atan2s(vec1->x, vec1->z));
                        if (atInfo->mResultingAttackType == 1) {
                            f32 y = fadds_ppc(cM_rndF(5.0f), 5.0f);
                            f32 z = fadds_ppc(cM_rndF(5.0f), 5.0f);
                            vec1->x = 0.0f;
                            vec1->y = y;
                            vec1->z = z;
                        } else {
                            f32 y = fadds_ppc(cM_rndF(10.0f), 30.0f);
                            f32 z = fadds_ppc(cM_rndF(5.0f), 10.0f);
                            vec1->x = 0.0f;
                            vec1->y = y;
                            vec1->z = z;
                            s16 nt;
                            if (kb_roll_t(i_this, i) != 0) {
                                nt = 0;
                            } else {
                                nt = cM_rndF(1.0f) < 0.5f ? 0x7FFF : -0x7FFF;
                            }
                            kb_roll_t(i_this, i) = nt;
                            s16 r = (s16)gabi::ftoi(cM_rndFX(32768.0f));
                            kb_yaw_t(i_this, i) = (s16)(kb_yaw_t(i_this, i) + r);
                        }
                        MtxPosition(vec1, v);
                        i_this->m9FC[i] = 0.0f;
                        mDoAud_seStart(0x5815 /* JA_SE_CM_MAGBALL_BOUND */, p, 0x63, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                    }
                }
            }
            cM3dGSph_SetC_bdk(gabi::at<void>(SPH(sph) + 0x118), p);
            ccs_Set_bdk(sph);
            {
                gabi::Local<cXyz> tmp;
                cXyz_mi(p, tmp, &i_this->m940[i]);
                bdk_vbits(vec1.get(), tmp.get());
            }
            if (std_sqrtf(PSVECSquareMag(vec1)) > 0.0f) {
                Mtx34* m = calc_mtx();
                cMtx_YrotS(m, cM_atan2s(vec1->x, vec1->z));
                vec1->x = 0.0f;
                vec1->y = 30.0f;
                vec1->z = 20.0f;
                MtxPosition(vec1, vec2);
                bdk_fcopy(vec4->x, p->x);
                f32 y = p->y;
                bdk_fcopy(vec4->z, p->z);
                vec4->y = fadds_ppc(y, 30.0f);
                {
                    gabi::Local<cXyz> tmp;
                    cXyz_pl(p, tmp, vec2);
                    bdk_vbits(vec5.get(), tmp.get());
                }
                dBgS_LinChk_Set(linChk.get(), vec4, vec5, actor);
                if (bdk_LineCross(linChk.get())) {
                    bdk_fcopy(p->x, i_this->m940[i].x);
                    bdk_fcopy(p->z, i_this->m940[i].z);
                    i_this->m9FC[i] = 0.0f;
                }
            }
        }
        MtxTrans(p->x, p->y, p->z, 0);
        cMtx_YrotM(calc_mtx(), (s16)(i_this->m9A0[i].y + kb_yaw(i_this, i)));
        cMtx_ZrotM(calc_mtx(), (s16)(i_this->m9A0[i].z + rots[0] + kb_roll(i_this, i)));
        cMtx_XrotM(calc_mtx(), (s16)(i_this->m9A0[i].x + rots[1]));
        cLib_addCalcAngleS2(&kb_yaw(i_this, i), kb_yaw_t(i_this, i), 4, 0x400);
        cLib_addCalcAngleS2(&kb_roll(i_this, i), kb_roll_t(i_this, i), 4, 0x800);
        MtxTrans(kb_off_x(i), kb_off_y(i), kb_off_z(i), 1);
        bdk_setBaseTRMtx(i_this->m8FC[i], calc_mtx());
    }
    bdk_chk_dt(linChk.get(), gndChk.get());
}

/* 02066A38 */
static BOOL daBdk_Execute(bdk_class* i_this) {
    WWHD_FUNC(0x02066A38, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    bdk_vbits(&i_this->m260C, &actor->current.pos);
    i_this->m260C.y = 9800.0f;
    i_this->m2C4 = i_this->m2C4 + 1;
    for (s32 i = 0; i < 5; i++) {
        if (i_this->m2EC[i] != 0) {
            i_this->m2EC[i] = i_this->m2EC[i] - 1;
        }
    }
    if (i_this->m2F8 != 0) {
        i_this->m2F8 = i_this->m2F8 - 1;
    }
    if (i_this->m2FA != 0) {
        i_this->m2FA = i_this->m2FA - 1;
    }
    if (i_this->m1138 != 0) {
        i_this->m1138 = i_this->m1138 - 1;
    }
    if (!l_HIO.m005) {
        gabi::call(0x025E535C, i_this->mpMorf.get(), &actor->eyePos, 0, 0); /* mDoExt_McaMorf::play */
        gabi::call(0x0206D644, i_this); /* move */
        if (i_this->mAction != 0xF && i_this->mAction < 0x64) {
            i_this->mAcch.CrrPos(dComIfG_Bgsp());
            if (i_this->mAcch.ChkGroundHit()) {
                actor->speed.y = -10.0f;
                i_this->m2586 = 2; /* HD: 2 (GameCube 3) */
            } else if (i_this->m2586 != 0) {
                i_this->m2586 = i_this->m2586 - 1;
            }
            i_this->mAcchCir.SetWall(50.0f, 300.0f);
            gabi::call(0x024F12A8, &i_this->mAcch, 0.0f); /* SetGroundUpY */
        }
        damage_check(i_this);
    }
    J3DModel* model;
    if (i_this->m2B4 == 1) {
        f32 s = fmuls_ppc(l_HIO.m008, 0.85f);
        actor->scale.z = s;
        actor->scale.y = s;
        actor->scale.x = s;
        model = morf_model(i_this->mpMorf);
        gabi::store<f32>(gabi::ea(model) + 0xBC, s);
        gabi::store<f32>(gabi::ea(model) + 0xC0, s);
        gabi::store<f32>(gabi::ea(model) + 0xC4, s);
    } else {
        u32 s = gabi::load<u32>(gabi::ea(&l_HIO.m008));
        gabi::store<u32>(gabi::ea(&actor->scale.z), s);
        gabi::store<u32>(gabi::ea(&actor->scale.y), s);
        gabi::store<u32>(gabi::ea(&actor->scale.x), s);
        model = morf_model(i_this->mpMorf);
        gabi::store<u32>(gabi::ea(model) + 0xBC, s);
        gabi::store<u32>(gabi::ea(model) + 0xC0, s);
        gabi::store<u32>(gabi::ea(model) + 0xC4, s);
    }
    mDoMtx_stack_c::transS(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z);
    mDoMtx_stack_c::YrotM(actor->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), actor->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), actor->shape_angle.z);
    bdk_setBaseTRMtx(model, mDoMtx_stack_c::get());
    gabi::call(0x025E55A0, i_this->mpMorf.get()); /* mDoExt_McaMorf::calc */

    col_set(i_this);
    head_swing(i_this);
    for (s32 i = 0; i < 4; i++) {
        tail_control(i_this, &i_this->m300[i]);
    }
    actor->shape_angle.y = actor->current.angle.y;
    actor->shape_angle.z = actor->current.angle.z;
    PSMTXCopy(bdk_morfAnmMtx(i_this, 0x18 /* DK_JNT_J_DK_ATAMA1_e */), calc_mtx());
    {
        gabi::Local<cXyz> offset;
        offset->x = fadds_ppc(REG0_F(10), 120.0f);
        offset->y = 0.0f;
        offset->z = fadds_ppc(REG0_F(12), -120.0f);
        MtxPosition(offset, &actor->eyePos);
    }
    bdk_vbits(gabi::at<cXyz>(gabi::ea(actor) + 0x390), &actor->eyePos); /* attention_info.position */
    s16 angle = 0;
    if (i_this->mF10 != 0) {
        gabi::Local<cXyz> d;
        {
            gabi::Local<cXyz> tmp;
            cXyz_mi(&player->current.pos, tmp, &actor->current.pos);
            bdk_vbits(d.get(), tmp.get());
        }
        s16 a = cM_atan2s(d->x, d->z);
        angle = (s16)(a - actor->shape_angle.y);
        if (angle > 7000) {
            angle = 7000;
        } else if (angle < -7000) {
            angle = -7000;
        }
    }
    i_this->mF10 = 0;
    cLib_addCalcAngleS2(&i_this->mF12, angle, 4, 0x800); /* HD: no mF14 */

    kamen_break_move(i_this);
    gabi::call(0x02071A68, i_this); /* my_effect_move */
    gabi::call(0x0206FC6C, i_this); /* obj_move */
    gabi::call(0x0206FEF0, i_this); /* demo_camera */
    gabi::call(0x0206F9E0, i_this); /* kankyo_cont */
    for (s32 i = 0; i < 4; i++) {
        if (i_this->m6078[i] != 0) {
            i_this->m6078[i] = i_this->m6078[i] - 1;
        }
    }
    if (i_this->m2618 != 0) {
        s8 n = (s8)(i_this->m2618 - 1);
        i_this->m2618 = n;
        if (n == 0) {
            bdk_particle_setToon(0xA136 /* ID_IT_ST_DK_TOBITATI_SMOKE00 */, &i_this->m260C, &actor->shape_angle, nullptr, 0xB9, &i_this->m6110,
                                 (s8)fopAcM_GetRoomNo(actor));
        }
    }
    if (i_this->m2619 != 0) {
        gabi::call(0x0206A590, actor, 0x5079 /* JA_SE_CM_DK_NAIL */, 0); /* fopAcM_seStartCurrent */
        f32 dy = fadds_ppc(REG0_F(9), 200.0f);
        for (s32 i = 0; i < 2; i++) {
            cXyz* fp = &bdk_foot_eff_pos(i);
            bdk_fcopy(fp->x, i_this->m1174[i].x);
            f32 y = fsubs_ppc(i_this->m1174[i].y, dy);
            bdk_fcopy(fp->z, i_this->m1174[i].z);
            if (y < 9800.0f) {
                y = 9800.0f;
            }
            fp->y = y;
        }
        if (bdk_foot_eff_pos(0).y > 9810.0f || i_this->m2584 == 0) {
            i_this->m2619 = 0;
            for (s32 i = 0; i < 4; i++) {
                JPABaseEmitter* e = i_this->mp6214[i];
                if (e != nullptr) {
                    gabi::store<s32>(gabi::ea(e) + 0x5C, -1);
                    gabi::call(0x0206BA98, e, 1); /* JPABaseEmitter_setStatus */
                    i_this->mp6214[i] = nullptr;
                }
            }
        } else {
            for (s32 i = 0; i < 4; i++) {
                JPABaseEmitter* e = i_this->mp6214[i];
                if (e != nullptr) {
                    cXyz* fp = &bdk_foot_eff_pos(i & 1);
                    f32 y = fp->y;
                    f32 x = fp->x;
                    f32 z = fp->z;
                    if (gabi::load<u8>(gabi::ea(e) + 0x262) >= 7) { /* HD */
                        y = -y;
                    }
                    gabi::call(0x020725FC, gabi::ea(e) + 0x22C, x, y, z); /* TVec3_set (setGlobalTranslation) */
                }
            }
        }
    }
    i_this->m2584 = 0;
    return TRUE;
}
VERIFY(0x02066A38, daBdk_Execute);
