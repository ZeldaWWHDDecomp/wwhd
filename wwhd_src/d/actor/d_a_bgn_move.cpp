/**
 * d_a_bgn_move.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 1): move (0208857C; the function map calls it shape_calc) with the inlined
 * size_set, damage_check, shape_calc and part_control_0Z.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bgn.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bgn.h"

void action_main(bgn_class* i_this);
void part_mtx_set(bgn_class* i_this, int param_2, part_s* param_3, int param_4, int param_5);
void action_s_himo(bgn_class* i_this, move_s* param_2, int param_3);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02518CC8 def_se_set(actor, cCcD_Obj* hitObj, u32 material) */
static inline void bgn_def_se_set(fopAc_ac_c* a, u32 obj, u32 mtrl) { gabi::call(0x02518CC8, a, obj, mtrl); }
/* 025E1904 mDoAud_bgmStop(frames) */
static inline void bgn_bgmStop(u32 frames) { gabi::call(0x025E1904, frames); }
/* 025E1A7C mDoAud_monsSeStart(id, pos, param, reverb) [as in d_a_bgn2] */
static inline void bgn_monsSeStart4(u32 id, cXyz* pos, u32 param, s32 reverb) { gabi::call(0x025E1A7C, id, pos, param, reverb); }
/* dCcD_GObjInf::ChkTgHit 025162A4, GetTgHitObj 02516300 */
static inline BOOL bgn_ChkTgHit(void* o) { return gabi::call<BOOL>(0x025162A4, o); }
static inline u32 bgn_GetTgHitObj(void* o) { return gabi::call<u32>(0x02516300, o); }
static inline void bgn_mtx_set(J3DModel* m) { J3DModel_setBaseTRMtx(m, calc_mtx()); }

/* the string's rest pose for a limb end: HIO offset + the string's sway, turned with the body, at mC308 */
static inline void bgn_limb_end(bgn_class* i_this, cXyz* local_8c, be<f32>* hio, move_s* m, cXyz* dst) {
    cMtx_YrotS(calc_mtx(), i_this->mC314.y);
    local_8c->z = hio[2];
    local_8c->y = hio[1];
    local_8c->x = hio[0];
    PSVECAdd(local_8c, &m->m2D4, local_8c);
    MtxPosition(local_8c, dst);
    PSVECAdd(dst, &i_this->mC308, dst);
}
static inline void bgn_copy3(cXyz* dst, u32 src) {
    u32 d = gabi::ea(dst);
    gabi::store<u32>(d, gabi::load<u32>(src));
    gabi::store<u32>(d + 4, gabi::load<u32>(src + 4));
    gabi::store<u32>(d + 8, gabi::load<u32>(src + 8));
}
static inline void bgn_emitter_scale(JPABaseEmitter* e, f32 s) { JPABaseEmitter_setGlobalScale(e, s, s, s); }

/* 0208857C move (matcher: shape_calc). HD: the GameCube's fopAcM_OffStatus(actor, 0) is gone; the lengths of
 * the arm/tail parts are divided directly (GameCube: multiplied by the reciprocal); the tail's free swing
 * (part_control_0Z) adds its sideways shift with fused multiply-adds; the rope updates of action_s run at
 * the end (020859AC); the last orb hit clears a play word (+0x5B44). */
void bgn_move(bgn_class* i_this) {
    WWHD_FUNC(0x0208857C, void, i_this);
    fopAc_ac_c* actor = i_this;
    dComIfGp_get();
    bgn_copy3(&i_this->mC734, gabi::ea(&i_this->mC728));
    mDoExt_baseAnm_play(i_this->mJyakutenCBrkAnm.get());
    mDoExt_baseAnm_play(i_this->mJyakutenBBrkAnm.get());
    if (i_this->m02B5 != 0) {
        i_this->mC748 = 10;
        actor->current.pos.x = 0.0f;
        actor->current.pos.y = 30000.0f;
        actor->current.pos.z = 0.0f;
        gabi::store<u32>(gabi::ea(actor) + 0x39C, 0); /* attention_info.flags */
        i_this->mCC80 = gabi::fadds_ppc(REG0_F(25), 1.0f);
        i_this->mCC84 = REG0_F(20);
        i_this->mCC88 = REG0_F(21);
        return;
    }
    if (i_this->mC748 == 10) {
        u32 h = gabi::ea(&actor->home.pos);
        u32 p = gabi::ea(&actor->current.pos);
        u32 hx = gabi::load<u32>(h), hy = gabi::load<u32>(h + 4);
        gabi::store<u32>(p, hx);
        u32 hz = gabi::load<u32>(h + 8);
        gabi::store<u32>(p + 4, hy);
        gabi::store<u32>(p + 8, hz);
        i_this->mC748 = 0;
        i_this->mC74A = 0;
        actor->health = 3;
    }
    gabi::store<u32>(gabi::ea(actor) + 0x39C, 4);   /* attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e */
    gabi::store<u8>(gabi::ea(actor) + 0x38A, 0x22); /* attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] */
    {
        gabi::Local<cXyz> sum;
        cXyz_pl(&actor->current.pos, sum, &i_this->mC728);
        i_this->mC308.x = sum->x;
        i_this->mC308.y = sum->y;
        i_this->mC308.z = sum->z;
    }
    f32 y = gabi::fadds_ppc(i_this->mC308.y, l_HIO().m04C);
    i_this->mC308.y = y;
    f32 c = gabi::fadds_ppc(gabi::fadds_ppc(i_this->mC7BC, 100.0f), REG0_F(0));
    if (!(y > c))
        i_this->mC308.y = c;
    s16 lz = actor->shape_angle.z;
    s16 ly = actor->shape_angle.y;
    s16 lx = actor->shape_angle.x;
    if (l_HIO().m025 != 0)
        ly = l_HIO().m042;
    BGN_HAND_MAX() = l_HIO().m0F0;
    BGN_TAIL_MAX() = l_HIO().m0F2;
    gabi::Local<u8[0x54]> gndChk;
    dBgS_GndChk_ct(gndChk, dBgS_GndChk_vt{0x10008A54, 0x10008A64, 0x10008A84, 0x10008A74}, false);
    {
        gabi::Local<cXyz> pos;
        bgn_copy3(pos, gabi::ea(&actor->current.pos));
        f32 py = gabi::fadds_ppc(actor->current.pos.y, 500.0f);
        pos->y = py;
        u32 g = gabi::ea(gndChk.get());
        gabi::store<f32>(g + 0x28, py);
        gabi::store<f32>(g + 0x24, actor->current.pos.x);
        i_this->mC7BC = gabi::fadds_ppc(REG0_F(16), 90.0f);
        gabi::store<f32>(g + 0x2C, pos->z);
    }
    s16 sVar1 = 0;
    if (i_this->mAAA8[0].m2D0 != 0)
        sVar1 = 8000;
    if (i_this->mAAA8[1].m2D0 != 0)
        sVar1 = (s16)(sVar1 - 8000);
    cLib_addCalcAngleS2(&i_this->mC744, sVar1, 8, 0x400);
    if (i_this->mAAA8[2].m2D0 == 0)
        lx = (s16)(lx + (s16)gabi::ftoi(gabi::fmuls_ppc(i_this->mAAA8[2].m2D4.y, gabi::fadds_ppc(REG0_F(11), 8.0f))));
    if (i_this->mAAA8[3].m2D0 != 0) {
        lz = (s16)(lz - (REG0_S(0) + 3000));
    } else {
        s16 dz = (s16)gabi::ftoi(gabi::fmuls_ppc(i_this->mAAA8[3].m2D4.y, gabi::fadds_ppc(REG0_F(12), 5.0f)));
        s16 dy = (s16)gabi::ftoi(-(f32)gabi::fmuls_ppc(i_this->mAAA8[3].m2D4.z, gabi::fadds_ppc(REG0_F(13), 5.0f)));
        lz = (s16)(lz + dz);
        ly = (s16)(ly + dy);
    }
    if (i_this->mAAA8[4].m2D0 != 0) {
        lz = (s16)(lz + (REG0_S(0) + 3000));
    } else {
        s16 dz = (s16)gabi::ftoi(-(f32)gabi::fmuls_ppc(i_this->mAAA8[4].m2D4.y, gabi::fadds_ppc(REG0_F(12), 5.0f)));
        s16 dy = (s16)gabi::ftoi(gabi::fmuls_ppc(i_this->mAAA8[4].m2D4.z, gabi::fadds_ppc(REG0_F(13), 5.0f)));
        lz = (s16)(lz + dz);
        ly = (s16)(ly + dy);
    }
    cLib_addCalcAngleS2(&i_this->mC314.x, lx, 4, 0x200);
    cLib_addCalcAngleS2(&i_this->mC31A, ly, 4, 0x1000);
    cLib_addCalcAngleS2(&i_this->mC314.z, lz, 4, 0x200);
    i_this->mC314.y = (s16)(i_this->mC31A + i_this->mC750);
    if (l_HIO().m029 != 0) i_this->mAAA8[0].m2D0 = 1;
    if (l_HIO().m02A != 0) i_this->mAAA8[1].m2D0 = 1;
    if (l_HIO().m02B != 0) i_this->mAAA8[2].m2D0 = 1;
    if (l_HIO().m02C != 0) i_this->mAAA8[3].m2D0 = 1;
    if (l_HIO().m02D != 0) i_this->mAAA8[4].m2D0 = 1;
    if (l_HIO().m02E != 0) i_this->mAAA8[5].m2D0 = 1;
    if (l_HIO().m02F != 0) i_this->mAAA8[6].m2D0 = 1;
    if (l_HIO().m030 != 0) i_this->mAAA8[7].m2D0 = 1;
    {
        f32 fVar9 = 0.0f;
        gabi::Local<cXyz> d1;
        gabi::Local<cXyz> d2;
        gabi::Local<cXyz> local_e4;
        for (s32 i = 0; i < 2; i++) {
            if (i == 0 && i_this->mC778 != 0) {
                s32 hm = BGN_HAND_MAX();
                cXyz_mi(&i_this->mLeftArmParts[hm].m0D4, d1, &i_this->mLeftArmParts[hm - 1].m0D4);
                bgn_copy3(local_e4, d1.a);
                fVar9 = std_sqrtf(PSVECSquareMag(local_e4));
            } else if (i == 1 && i_this->mC779 != 0) {
                s32 hm = BGN_HAND_MAX();
                cXyz_mi(&i_this->mRightArmParts[hm].m0D4, d2, &i_this->mRightArmParts[hm - 1].m0D4);
                u32 s = d2.a;
                u32 d = local_e4.a;
                u32 z = gabi::load<u32>(s + 8), x = gabi::load<u32>(s), yy = gabi::load<u32>(s + 4);
                gabi::store<u32>(d, x);
                gabi::store<u32>(d + 4, yy);
                gabi::store<u32>(d + 8, z);
                fVar9 = std_sqrtf(PSVECSquareMag(local_e4));
            }
            f32 t = gabi::fadds_ppc(REG0_F(5), 200.0f);
            f32 m124 = l_HIO().m124;
            if (fVar9 > t) {
                fVar9 = gabi::fsubs_ppc(fVar9, t);
                fVar9 = gabi::fmuls_ppc(fVar9, gabi::fadds_ppc(REG0_F(6), 0.2f));
            } else {
                fVar9 = 0.0f;
            }
            cLib_addCalc2(&i_this->mC324[i], gabi::fadds_ppc(m124, fVar9), 1.0f, gabi::fadds_ppc(REG0_F(4), 20.0f));
            cLib_addCalc0(&i_this->mC32C[i], 0.05f, 0.02f);
        }
    }
    /* size_set (inline). HD: the steps are divided (GameCube: reciprocal multiplied) */
    {
        daBgn_HIO_c& h = l_HIO();
        i_this->mHeadParts[0].m0F4 = h.m0F8;
        i_this->mPelvisParts[0].m0F4 = h.m100;
        i_this->mPelvisParts[1].m0F4 = h.m100;
        f32 m104 = h.m104;
        f32 fVar2 = gabi::fsubs_ppc(m104, h.m108) / (f32)(BGN_HAND_MAX() - 2);
        i_this->mLeftArmParts[0].m0F4 = gabi::fsubs_ppc(m104, fVar2);
        i_this->mRightArmParts[0].m0F4 = gabi::fsubs_ppc(h.m104, fVar2);
        for (s32 i = 1; i < BGN_HAND_MAX(); i++) {
            i_this->mLeftArmParts[i].m0F4 = gabi::fnmsubs(fVar2, (f32)(i - 1), h.m104);
            i_this->mRightArmParts[i].m0F4 = gabi::fnmsubs(fVar2, (f32)(i - 1), h.m104);
        }
        i_this->mLeftArmParts[BGN_HAND_MAX()].m0F4 = h.m108;
        i_this->mRightArmParts[BGN_HAND_MAX()].m0F4 = h.m108;
        f32 a = h.m104, b = h.m108;
        fVar2 = gabi::fmadds(gabi::fsubs_ppc(a, b), 0.5f, b);
        for (s32 i = 0; i < BGN_HAND_MAX(); i++) {
            f32 l = i_this->mLeftArmParts[i].m0F4;
            f32 dl = gabi::fmuls_ppc(gabi::fsubs_ppc(fVar2, l), i_this->mC32C[0]);
            i_this->mLeftArmParts[i].m0F4 = gabi::fadds_ppc(l, gabi::fadds_ppc(dl, dl));
            f32 r = i_this->mRightArmParts[i].m0F4;
            f32 dr = gabi::fmuls_ppc(gabi::fsubs_ppc(fVar2, r), i_this->mC32C[1]);
            i_this->mRightArmParts[i].m0F4 = gabi::fadds_ppc(r, gabi::fadds_ppc(dr, dr));
        }
        fVar2 = gabi::fmuls_ppc(gabi::fsubs_ppc(h.m10C, h.m110), 0.5f);
        for (s32 i = 0; i < 3; i++) {
            i_this->mLeftLegParts[i].m0F4 = gabi::fnmsubs(fVar2, (f32)i, h.m10C);
            i_this->mRightLegParts[i].m0F4 = gabi::fnmsubs(fVar2, (f32)i, h.m10C);
        }
        i_this->mLeftLegParts[3].m0F4 = h.m110;
        i_this->mRightLegParts[3].m0F4 = h.m110;
        f32 m114 = h.m114;
        fVar2 = gabi::fsubs_ppc(m114, h.m118) / (f32)(BGN_TAIL_MAX() - 1);
        for (s32 i = 0; i < BGN_TAIL_MAX(); i++) {
            m114 = h.m114;
            i_this->mTailParts[i].m0F4 = gabi::fnmsubs(fVar2, (f32)i, m114);
        }
        if (BGN_TAIL_MAX() > 0)
            m114 = h.m114;
        i_this->mRightLegParts[BGN_TAIL_MAX() + 3].m0F4 = gabi::fmuls_ppc(m114, 0.7f);
        i_this->mTailParts[BGN_TAIL_MAX()].m0F4 = gabi::fmuls_ppc(h.m114, 0.7f);
    }
    OffAtSetBit(&i_this->mC7FC);
    OffAtSetBit(&i_this->mPelvisParts[0].mPartSph);
    OffAtSetBit(&i_this->mLeftArmParts[BGN_HAND_MAX() - 1].mPartSph);
    OffAtSetBit(&i_this->mRightArmParts[BGN_HAND_MAX() - 1].mPartSph);
    for (s32 i = 0; i < BGN_TAIL_MAX(); i++)
        OffAtSetBit(&i_this->mTailParts[i].mPartSph);
    action_main(i_this);
    dComIfGp_get();

    /* damage_check (inline) */
    if (i_this->mC7B6 == 0) {
        u32 hitObj = 0;
        gabi::Local<be<u32>> atObj;
        if (bgn_ChkTgHit(&i_this->mCoreSph)) {
            *atObj = bgn_GetTgHitObj(&i_this->mCoreSph);
            i_this->m0302 = 1;
            i_this->mC7B6 = 100;
            i_this->mC748 = 5;
            i_this->mC74A = 0;
            i_this->mC779 = 0;
            i_this->mC778 = 0;
            s8 health = actor->health;
            if (health != 0) {
                s8 room = fopAcM_GetRoomNo(actor);
                actor->health = (s8)(health - 1);
                mDoAud_seStart(0x2879 /* JA_SE_LK_ARROW_HIT */, nullptr, 0x35, dComIfGp_getReverb(room));
                f32 s = gabi::fadds_ppc(REG0_F(5), 2.0f);
                if (actor->health == 0) {
                    bgn_bgmStop(30);
                    gabi::store<f32>(dComIfGp_ea() + 0x5B44, 0.0f);
                    i_this->mC748 = 6;
                    i_this->mC74A = 0;
                    JPABaseEmitter* e = dComIfGp_particle_set(0x8457 /* ID_AK_SN_KGTBREAKWEAKPOINT00 */, &i_this->mCA54);
                    if (e != nullptr)
                        bgn_emitter_scale(e, s);
                    e = dComIfGp_particle_set(0x8458 /* ID_AK_SN_KGTBREAKWEAKPOINT01 */, &i_this->mCA54);
                    if (e != nullptr)
                        bgn_emitter_scale(e, s);
                    bgn_monsSeStart4(0x496F /* JA_SE_CV_BGN_HIT_2 */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                    mDoAud_seStart(0x2828 /* JA_SE_LK_LAST_HIT */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                    mDoAud_seStart(0x5983 /* JA_SE_CM_BGN_M_BRK_ORB */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                } else {
                    JPABaseEmitter* e = dComIfGp_particle_set(0x8459 /* ID_AK_SN_KGTHITWEAKPOINT00 */, &i_this->mCA54);
                    if (e != nullptr)
                        bgn_emitter_scale(e, s);
                    bgn_monsSeStart4(0x496E /* JA_SE_CV_BGN_HIT_1 */, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                }
            }
        }
        s8 cVar5 = 0;
        if (bgn_ChkTgHit(&i_this->mC7FC)) {
            *atObj = bgn_GetTgHitObj(&i_this->mC7FC);
            cVar5 = 1;
        }
        if (bgn_ChkTgHit(&i_this->mHeadParts[0].mPartSph)) {
            *atObj = bgn_GetTgHitObj(&i_this->mHeadParts[0].mPartSph);
            cVar5 = 2;
        }
        if (bgn_ChkTgHit(&i_this->mPelvisParts[0].mPartSph)) {
            hitObj = bgn_GetTgHitObj(&i_this->mPelvisParts[0].mPartSph);
            cVar5 = 3;
        } else {
            hitObj = *atObj;
        }
        for (s32 i = 0; i < 20; i++) {
            if (bgn_ChkTgHit(&i_this->mLeftArmParts[i].mPartSph)) {
                hitObj = bgn_GetTgHitObj(&i_this->mLeftArmParts[i].mPartSph);
                cVar5 = 4;
            }
            if (bgn_ChkTgHit(&i_this->mRightArmParts[i].mPartSph)) {
                hitObj = bgn_GetTgHitObj(&i_this->mRightArmParts[i].mPartSph);
                cVar5 = 5;
            }
        }
        for (s32 i = 0; i < 3; i++) {
            if (bgn_ChkTgHit(&i_this->mLeftLegParts[i].mPartSph)) {
                hitObj = bgn_GetTgHitObj(&i_this->mLeftLegParts[i].mPartSph);
                cVar5 = 6;
            }
            if (bgn_ChkTgHit(&i_this->mRightLegParts[i].mPartSph)) {
                hitObj = bgn_GetTgHitObj(&i_this->mRightLegParts[i].mPartSph);
                cVar5 = 7;
            }
        }
        for (s32 i = 0; i < 20; i++) {
            if (bgn_ChkTgHit(&i_this->mTailParts[i].mPartSph)) {
                hitObj = bgn_GetTgHitObj(&i_this->mTailParts[i].mPartSph);
                cVar5 = 8;
            }
        }
        if (cVar5 != 0 && i_this->mC7B8 == 0) {
            i_this->mC7B8 = 10;
            bgn_def_se_set(actor, hitObj, 0x44);
        }
    }

    /* shape_calc (inline) */
    gabi::Local<cXyz> local_8c;
    gabi::Local<cXyz> cStack_98;
    gabi::Local<cXyz> sum;
    {
        f32 hy = (f32)(s32)(s16)l_HIO().m0A8;
        f32 c2 = gabi::fadds_ppc(gabi::fadds_ppc(i_this->mC7BC, 100.0f), REG0_F(0));
        f32 by = gabi::fadds_ppc(i_this->mC308.y, hy);
        f32 bz = i_this->mC308.z;
        s16 m0302 = i_this->m0302;
        f32 bx = i_this->mC308.x;
        if (!(by > c2))
            by = c2;
        if (m0302 == 0xD) {
            i_this->mArrowHitFlashTimer = 50;
            i_this->mArrowHitEffectTimer = 100;
        }
        MtxTrans(bx, by, bz, false);
    }
    s16 sVar8 = 0;
    s16 sVar5 = 0;
    if (i_this->mC770 != 0) {
        s16 t = (s16)(i_this->mC770 - 1);
        i_this->mC770 = t;
        s16 c746 = i_this->mC746;
        f32 f = gabi::fmuls_ppc((f32)t, gabi::fadds_ppc(REG0_F(14), 300.0f));
        sVar8 = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_ssin(c746 * 0x1C00), f));
        sVar5 = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_scos(c746 * 0x1900), f));
    }
    cMtx_YrotM(calc_mtx(), i_this->mC314.y);
    cMtx_XrotM(calc_mtx(), (s16)(i_this->mC314.x + sVar8));
    cMtx_ZrotM(calc_mtx(), (s16)(i_this->mC314.z + sVar5));
    f32 dVar12 = gabi::fmuls_ppc(l_HIO().m0FC, l_HIO().m0F4);
    MtxScale(dVar12, dVar12, dVar12, true);
    MtxPush();
    cMtx_XrotM(calc_mtx(), l_HIO().m004);
    if (i_this->mCC84 > 0.0f) {
        s16 s = (s16)(i_this->mC746 * (REG0_S(4) + 0x900));
        cMtx_ZrotM(calc_mtx(), s);
        f32 k = i_this->mCC84;
        f32 a = gabi::fadds_ppc(k, 1.0f);
        MtxScale(a, gabi::fsubs_ppc(1.0f, k), a, true);
        cMtx_ZrotM(calc_mtx(), (s16)-s);
    }
    bgn_mtx_set(i_this->mpChestModel);
    i_this->mC7FC.SetR(gabi::fmuls_ppc(dVar12, gabi::fadds_ppc(REG0_F(1), 120.0f)));
    i_this->mC7FC.SetC(&i_this->mC308);
    dComIfG_Ccsp_Set(&i_this->mC7FC);
    if (i_this->mArrowHitEffectTimer != 0) {
        s16 t = (s16)(i_this->mArrowHitEffectTimer - 1);
        i_this->mArrowHitEffectTimer = t;
        f32 fVar3 = gabi::fmuls_ppc(gabi::fmuls_ppc(dVar12, (f32)t), gabi::fadds_ppc(REG_F(8, 1), 0.03f));
        if (i_this->mpArrowHitEmitter1 == nullptr) {
            i_this->mpArrowHitEmitter1 = dComIfGp_particle_set(0x3ED /* ID_AK_JN_CCTHUNDER00 */, &i_this->mC308);
        } else {
            JPABaseEmitter_setGlobalTranslation(i_this->mpArrowHitEmitter1, i_this->mC308.x, i_this->mC308.y, i_this->mC308.z);
            bgn_emitter_scale(i_this->mpArrowHitEmitter1, fVar3);
        }
        if (i_this->mpArrowHitEmitter2 == nullptr) {
            local_8c->y = dVar12;
            local_8c->z = dVar12;
            local_8c->x = dVar12;
            i_this->mpArrowHitEmitter2 = dComIfGp_particle_set(0x3EE /* ID_AK_JN_CCTHUNDER01 */, &i_this->mC308);
        } else {
            JPABaseEmitter_setGlobalTranslation(i_this->mpArrowHitEmitter2, i_this->mC308.x, i_this->mC308.y, i_this->mC308.z);
            bgn_emitter_scale(i_this->mpArrowHitEmitter2, fVar3);
        }
    } else {
        if (i_this->mpArrowHitEmitter1 != nullptr) {
            JPABaseEmitter_becomeInvalidEmitter(i_this->mpArrowHitEmitter1);
            i_this->mpArrowHitEmitter1 = nullptr;
        }
        if (i_this->mpArrowHitEmitter2 != nullptr) {
            JPABaseEmitter_becomeInvalidEmitter(i_this->mpArrowHitEmitter2);
            i_this->mpArrowHitEmitter2 = nullptr;
        }
    }
    MtxPull();
    f32 dVar12b = gabi::fmuls_ppc(l_HIO().m11C, l_HIO().m0F4);
    f32 dVar11 = gabi::fmuls_ppc(l_HIO().m120, l_HIO().m0F4);
    f32 dVar10 = gabi::fmuls_ppc(i_this->mC324[0], l_HIO().m0F4);
    f32 dVar9 = gabi::fmuls_ppc(i_this->mC324[1], l_HIO().m0F4);
    f32 dVar13 = gabi::fmuls_ppc(l_HIO().m128, l_HIO().m0F4);
    MtxPush();
    local_8c->y = l_HIO().m130;
    local_8c->x = l_HIO().m12C;
    local_8c->z = l_HIO().m134;
    MtxPosition(local_8c, &i_this->mHeadParts[0].m0D4);
    part_control_0(i_this, 1, &i_this->mHeadParts[0], &i_this->mAAA8[0], dVar12b);
    if (i_this->mAAA8[0].m2D0 == 0 || i_this->mAAA8[1].m2D0 == 0) {
        bgn_limb_end(i_this, local_8c, &l_HIO().m054, &i_this->mAAA8[0], &i_this->mHeadParts[1].m0D4);
        part_control_2(i_this, 1, &i_this->mHeadParts[0], dVar12b);
    }
    part_mtx_set(i_this, 1, &i_this->mHeadParts[0], 0, 0);
    {
        u32 src = gabi::ea(&i_this->mHeadParts[0].m0D4);
        u32 a = gabi::ea(actor);
        u32 x = gabi::load<u32>(src), yy = gabi::load<u32>(src + 4);
        gabi::store<u32>(a + 0x37C, x); /* eyePos */
        u32 z = gabi::load<u32>(src + 8);
        gabi::store<u32>(a + 0x380, yy);
        gabi::store<u32>(a + 0x384, z);
        gabi::store<u32>(a + 0x390, x); /* attention_info.position */
        gabi::store<u32>(a + 0x394, yy);
        f32 ay = gabi::fadds_ppc(gabi::load<f32>(a + 0x394), 50.0f);
        gabi::store<u32>(a + 0x398, z);
        gabi::store<f32>(a + 0x394, ay);
    }
    MtxPull();
    MtxPush();
    {
        f32 k = i_this->mCC80;
        local_8c->x = gabi::fmuls_ppc(l_HIO().m144, k);
        local_8c->y = gabi::fmuls_ppc(gabi::fadds_ppc(l_HIO().m148, 20.0f), k);
        local_8c->z = gabi::fmuls_ppc(l_HIO().m14C, k);
    }
    MtxPosition(local_8c, &i_this->mPelvisParts[0].m0D4);
    part_control_0(i_this, 1, &i_this->mPelvisParts[0], &i_this->mAAA8[2], dVar12b);
    if (i_this->mAAA8[2].m2D0 == 0) {
        bgn_limb_end(i_this, local_8c, &l_HIO().m060, &i_this->mAAA8[2], &i_this->mPelvisParts[1].m0D4);
        part_control_2(i_this, 1, &i_this->mPelvisParts[0], dVar12b);
    }
    part_mtx_set(i_this, 1, &i_this->mPelvisParts[0], 2, 0);
    MtxPull();
    MtxPush();
    {
        f32 k = i_this->mCC80;
        local_8c->x = gabi::fmuls_ppc(l_HIO().m138, k);
        local_8c->y = gabi::fmuls_ppc(l_HIO().m13C, k);
        local_8c->z = gabi::fmuls_ppc(l_HIO().m140, k);
    }
    MtxPosition(local_8c, &i_this->mLeftArmParts[0].m0D4);
    if (i_this->mAAA8[3].m2D0 == 0) {
        part_control_0(i_this, BGN_HAND_MAX(), &i_this->mLeftArmParts[0], &i_this->mAAA8[3], dVar10);
        if (i_this->mC778 != 0) {
            bgn_copy3(&i_this->mLeftArmParts[BGN_HAND_MAX()].m0D4, gabi::ea(&i_this->mC77C));
        } else {
            cMtx_YrotS(calc_mtx(), i_this->mC314.y);
            local_8c->x = l_HIO().m06C;
            local_8c->z = l_HIO().m074;
            local_8c->y = l_HIO().m070;
            PSVECAdd(local_8c, &i_this->mAAA8[3].m2D4, local_8c);
            MtxPosition(local_8c, &i_this->mLeftArmParts[BGN_HAND_MAX()].m0D4);
            cXyz* e = &i_this->mLeftArmParts[BGN_HAND_MAX()].m0D4;
            PSVECAdd(e, &i_this->mC308, e);
        }
        part_control_2(i_this, BGN_HAND_MAX(), &i_this->mLeftArmParts[0], dVar10);
    } else {
        part_control_0(i_this, BGN_HAND_MAX(), &i_this->mLeftArmParts[0], &i_this->mAAA8[3], dVar10);
    }
    part_mtx_set(i_this, BGN_HAND_MAX(), &i_this->mLeftArmParts[0], 3, BGN_HAND_MAX() - 1);
    MtxPull();
    {
        f32 k = i_this->mCC80;
        local_8c->x = gabi::fmuls_ppc(-(f32)l_HIO().m138, k);
        local_8c->y = gabi::fmuls_ppc(l_HIO().m13C, k);
        local_8c->z = gabi::fmuls_ppc(l_HIO().m140, k);
    }
    MtxPosition(local_8c, &i_this->mRightArmParts[0].m0D4);
    if (i_this->mAAA8[4].m2D0 == 0) {
        part_control_0(i_this, BGN_HAND_MAX(), &i_this->mRightArmParts[0], &i_this->mAAA8[4], dVar9);
        if (i_this->mC779 != 0) {
            bgn_copy3(&i_this->mRightArmParts[BGN_HAND_MAX()].m0D4, gabi::ea(&i_this->mC788));
        } else {
            cMtx_YrotS(calc_mtx(), i_this->mC314.y);
            local_8c->x = l_HIO().m078;
            local_8c->z = l_HIO().m080;
            local_8c->y = l_HIO().m07C;
            PSVECAdd(local_8c, &i_this->mAAA8[4].m2D4, local_8c);
            MtxPosition(local_8c, &i_this->mRightArmParts[BGN_HAND_MAX()].m0D4);
            cXyz* e = &i_this->mRightArmParts[BGN_HAND_MAX()].m0D4;
            PSVECAdd(e, &i_this->mC308, e);
        }
        part_control_2(i_this, BGN_HAND_MAX(), &i_this->mRightArmParts[0], dVar9);
    } else {
        part_control_0(i_this, BGN_HAND_MAX(), &i_this->mRightArmParts[0], &i_this->mAAA8[4], dVar9);
    }
    part_mtx_set(i_this, BGN_HAND_MAX(), &i_this->mRightArmParts[0], 4, BGN_HAND_MAX() - 1);
    cMtx_XrotS(calc_mtx(), i_this->mPelvisParts[0].m0E0.x);
    cMtx_YrotM(calc_mtx(), i_this->mPelvisParts[0].m0E0.y);
    cMtx_ZrotM(calc_mtx(), (s16)-i_this->mPelvisParts[0].m0E0.z);
    {
        f32 s = gabi::fmuls_ppc(l_HIO().m100, l_HIO().m0F4);
        MtxScale(s, s, s, true);
    }
    MtxPush();
    {
        f32 k = i_this->mCC80;
        local_8c->x = gabi::fmuls_ppc(l_HIO().m150, k);
        local_8c->y = gabi::fmuls_ppc(l_HIO().m154, k);
        local_8c->z = gabi::fmuls_ppc(l_HIO().m158, k);
    }
    MtxPosition(local_8c, cStack_98);
    cXyz_pl(&i_this->mPelvisParts[0].m0D4, sum, cStack_98);
    bgn_copy3(&i_this->mLeftLegParts[0].m0D4, sum.a);
    part_control_0(i_this, 3, &i_this->mLeftLegParts[0], &i_this->mAAA8[5], dVar11);
    if (i_this->mAAA8[5].m2D0 == 0) {
        bgn_limb_end(i_this, local_8c, &l_HIO().m084, &i_this->mAAA8[5], &i_this->mLeftLegParts[3].m0D4);
        part_control_2(i_this, 3, &i_this->mLeftLegParts[0], dVar11);
    }
    part_mtx_set(i_this, 3, &i_this->mLeftLegParts[0], 5, 2);
    MtxPull();
    MtxPush();
    {
        f32 k = i_this->mCC80;
        local_8c->x = gabi::fmuls_ppc(-(f32)l_HIO().m150, k);
        local_8c->y = gabi::fmuls_ppc(l_HIO().m154, k);
        local_8c->z = gabi::fmuls_ppc(l_HIO().m158, k);
    }
    MtxPosition(local_8c, cStack_98);
    cXyz_pl(&i_this->mPelvisParts[0].m0D4, sum, cStack_98);
    bgn_copy3(&i_this->mRightLegParts[0].m0D4, sum.a);
    part_control_0(i_this, 3, &i_this->mRightLegParts[0], &i_this->mAAA8[6], dVar11);
    if (i_this->mAAA8[6].m2D0 == 0) {
        bgn_limb_end(i_this, local_8c, &l_HIO().m090, &i_this->mAAA8[6], &i_this->mRightLegParts[3].m0D4);
        part_control_2(i_this, 3, &i_this->mRightLegParts[0], dVar11);
    }
    part_mtx_set(i_this, 3, &i_this->mRightLegParts[0], 6, 2);
    MtxPull();
    {
        f32 k = i_this->mCC80;
        local_8c->x = gabi::fmuls_ppc(l_HIO().m15C, k);
        local_8c->y = gabi::fmuls_ppc(l_HIO().m160, k);
        local_8c->z = gabi::fmuls_ppc(l_HIO().m164, k);
    }
    MtxPosition(local_8c, cStack_98);
    cXyz_pl(&i_this->mPelvisParts[0].m0D4, sum, cStack_98);
    bgn_copy3(&i_this->mTailParts[0].m0D4, sum.a);
    s32 tailMax = BGN_TAIL_MAX();
    if (i_this->mAAA8[7].m2D0 == 0) {
        part_control_0(i_this, tailMax, &i_this->mTailParts[0], &i_this->mAAA8[7], dVar13);
        cMtx_YrotS(calc_mtx(), i_this->mC314.y);
        local_8c->x = l_HIO().m09C;
        local_8c->y = l_HIO().m0A0;
        local_8c->z = l_HIO().m0A4;
        PSVECAdd(local_8c, &i_this->mAAA8[7].m2D4, local_8c);
        MtxPosition(local_8c, &i_this->mTailParts[BGN_TAIL_MAX()].m0D4);
        cXyz* e = &i_this->mTailParts[BGN_TAIL_MAX()].m0D4;
        PSVECAdd(e, &i_this->mC308, e);
        f32 c3 = i_this->mC7BC;
        part_s* last = &i_this->mTailParts[BGN_TAIL_MAX()];
        if (!(last->m0D4.y > c3))
            last->m0D4.y = c3;
        part_control_2(i_this, BGN_TAIL_MAX(), &i_this->mTailParts[0], dVar13);
        part_mtx_set(i_this, BGN_TAIL_MAX(), &i_this->mTailParts[0], 7, BGN_TAIL_MAX() - 1);
    } else {
        /* part_control_0Z (inline) */
        s32 param_2 = tailMax;
        part_s* param_3 = &i_this->mTailParts[0];
        move_s* param_4 = &i_this->mAAA8[7];
        f32 param_5 = dVar13;
        gabi::Local<cXyz> local_f8;
        gabi::Local<cXyz> local_128;
        gabi::Local<cXyz> local_134;
        gabi::Local<cXyz> cStack_104;
        gabi::Local<cXyz> psum;
        cMtx_YrotS(calc_mtx(), i_this->mC314.y);
        if (param_3 == &i_this->mLeftArmParts[0]) {
            local_f8->y = 0.0f;
            local_f8->z = 0.0f;
            local_f8->x = gabi::fadds_ppc(REG0_F(8), 20.0f);
            MtxPosition(local_f8, local_128);
        } else if (param_3 == &i_this->mRightArmParts[0]) {
            f32 v = gabi::fadds_ppc(REG0_F(8), 20.0f);
            local_f8->z = 0.0f;
            local_f8->y = 0.0f;
            local_f8->x = -v;
            MtxPosition(local_f8, local_128);
        } else if (param_3 == &i_this->mTailParts[0]) {
            local_f8->x = 0.0f;
            local_f8->y = 0.0f;
            local_f8->z = gabi::fadds_ppc(REG0_F(6), -5.0f);
            MtxPosition(local_f8, local_128);
        }
        f32 dVar10z = i_this->mC7BC;
        f32 dVar9z = l_HIO().m170;
        param_3++;
        for (s32 i = 1; i < param_2 + 1; i++, param_3++) {
            part_s* prev = param_3 - 1;
            f32 py = param_3->m0D4.y;
            f32 dVar11z = gabi::fadds_ppc(py, dVar9z);
            if (!(dVar11z > dVar10z)) {
                dVar11z = dVar10z;
                if ((u32)i == (u32)param_2 && (i_this->mC746 & 7) == 0) {
                    local_134->y = py;
                    local_134->x = param_3->m0D4.x;
                    local_134->z = param_3->m0D4.z;
                    if (!gr_check(i_this, local_134)) {
                        dComIfGp_particle_setSimple(0x8443 /* ID_AK_SN_O_KGTCOMMONHAMON03 */, local_134, 0xFF);
                        if (i_this->m0304 == 0) {
                            f32 r = gabi::fadds_ppc(cM_rndF(20.0f), 20.0f);
                            u32 sx = gabi::load<u32>(local_134.a + 4), sz = gabi::load<u32>(local_134.a + 8), sxx = gabi::load<u32>(local_134.a);
                            u32 dst = gabi::ea(&i_this->m0308);
                            gabi::store<u32>(dst, sxx);
                            gabi::store<u32>(dst + 4, sx);
                            gabi::store<u32>(dst + 8, sz);
                            i_this->m0304 = (s16)gabi::ftoi(r);
                            mDoAud_seStart(0x6A43 /* JA_SE_CM_BGN_BODY_RIPPLE */, &i_this->m0308, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                        }
                    }
                }
            }
            f32 f1 = (f32)(param_2 - i);
            s16 c3 = REG0_S(3);
            f32 m2f4 = param_4->m2F4;
            f32 x = gabi::fmadds(local_128->x, f1, gabi::fsubs_ppc(param_3->m0D4.x, prev->m0D4.x));
            s16 iVar2 = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_ssin(param_4->m2FA + i * (c3 + 8000)), m2f4));
            f32 dz = gabi::fsubs_ppc(param_3->m0D4.z, prev->m0D4.z);
            f32 yv = gabi::fsubs_ppc(dVar11z, prev->m0D4.y);
            f32 z = gabi::fmadds(local_128->z, f1, dz);
            s16 iVar4 = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_scos(param_4->m2FC + i * (REG0_S(4) + 9000)), m2f4));
            s16 a = cM_atan2s(x, z);
            prev->m0E0.y = (s16)(a + iVar4);
            f32 sq = std_sqrtf(gabi::fmadds(x, x, gabi::fmuls_ppc(z, z)));
            s16 b = cM_atan2s(yv, sq);
            s16 ry = prev->m0E0.y;
            prev->m0E0.x = (s16)(iVar2 - b);
            cMtx_YrotS(calc_mtx(), ry);
            cMtx_XrotM(calc_mtx(), prev->m0E0.x);
            local_f8->x = 0.0f;
            local_f8->y = 0.0f;
            local_f8->z = gabi::fmuls_ppc(gabi::fmuls_ppc(param_5, prev->m0F4), i_this->mCC80);
            MtxPosition(local_f8, cStack_104);
            cXyz_pl(&prev->m0D4, psum, cStack_104);
            bgn_copy3(&param_3->m0D4, psum.a);
        }
        part_mtx_set(i_this, BGN_TAIL_MAX(), &i_this->mTailParts[0], 7, BGN_TAIL_MAX() - 1);
    }
    for (s32 i = 0; i < 8; i++)
        action_s_himo(i_this, &i_this->mAAA8[i], i);
    /* ~dBgS_GndChk (inline): the base vtables back, then ~cBgS_Chk */
    {
        u32 g = gabi::ea(gndChk.get());
        gabi::store<u32>(g + 0x20, 0x10008A64);
        gabi::store<u32>(g + 0x40, 0x10008A84);
        gabi::store<u32>(g + 0x4C, 0x10008A44);
        gabi::call(0x02008DAC, gndChk.get(), 0);
    }
}
VERIFY(0x0208857C, bgn_move);
