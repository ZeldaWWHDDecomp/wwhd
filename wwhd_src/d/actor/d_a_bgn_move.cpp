/**
 * d_a_bgn_move.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 1): move (0208857C; the function map calls it shape_calc) with the
 * inlined size_set, damage_check, shape_calc and part_control_0Z.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bgn.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bgn_move.h"

void action_main(bgn_class *i_this);
void part_mtx_set(bgn_class *i_this, int param_2, part_s *param_3, int param_4, int param_5);
void action_s_himo(bgn_class *i_this, move_s *param_2, int param_3);

static inline void bgn_mtx_set(J3DModel *m) { J3DModel_setBaseTRMtx(m, calc_mtx()); }

/* the string's rest pose for a limb end: HIO offset + the string's sway, turned with the body, at
 * mBodyPos */
static inline void bgn_limb_end(bgn_class *i_this, cXyz *local_8c, be<f32> *hio, move_s *m,
                                cXyz *dst) {
    cMtx_YrotS(calc_mtx(), i_this->mBodyAngle.y);
    local_8c->z = hio[2];
    local_8c->y = hio[1];
    local_8c->x = hio[0];
    PSVECAdd(local_8c, &m->mPose, local_8c);
    MtxPosition(local_8c, dst);
    PSVECAdd(dst, &i_this->mBodyPos, dst);
}
static inline void bgn_copy3(cXyz *dst, const cXyz *src) {
    auto *d = bgn_move_detail::words(dst);
    auto *source = bgn_move_detail::words(src);
    d->x = (u32)source->x;
    d->y = (u32)source->y;
    d->z = (u32)source->z;
}
static inline void bgn_emitter_scale(JPABaseEmitter *e, f32 s) {
    JPABaseEmitter_setGlobalScale(e, s, s, s);
}

/* 0208857C */
// HD: no fopAcM_OffStatus(actor,0); arm/tail lengths use division instead of reciprocal
// multiplication. Tail sway uses fused multiply-adds; action_s rope updates run at the end.
// The last orb hit also clears mItemLifeCount in the play object.
void move(bgn_class *i_this) {
    fopAc_ac_c *actor = i_this;
    dComIfGp_get();
    bgn_copy3(&i_this->mBodyOffsetOld, &i_this->mBodyOffset);
    mDoExt_baseAnm_play(i_this->mJyakutenCBrkAnm.get());
    mDoExt_baseAnm_play(i_this->mJyakutenBBrkAnm.get());
    if (i_this->mForm != 0) {
        i_this->mAction = 10;
        actor->current.pos.x = 0.0f;
        actor->current.pos.y = 30000.0f;
        actor->current.pos.z = 0.0f;
        actor->attention_info.flags = 0;
        i_this->mPartLengthScale = gabi::fadds_ppc(REG0_F(25), 1.0f);
        i_this->mChestSquash = REG0_F(20);
        i_this->mFogFade = REG0_F(21);
        return;
    }
    if (i_this->mAction == 10) {
        auto *h = bgn_move_detail::words(&actor->home.pos);
        auto *p = bgn_move_detail::words(&actor->current.pos);
        u32 hx = (u32)h->x, hy = (u32)h->y;
        p->x = hx;
        u32 hz = (u32)h->z;
        p->y = hy;
        p->z = hz;
        i_this->mAction = 0;
        i_this->mMode = 0;
        actor->health = 3;
    }
    actor->attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e;
    actor->attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 0x22;
    {
        gabi::Local<cXyz> sum;
        cXyz_pl(&actor->current.pos, sum, &i_this->mBodyOffset);
        i_this->mBodyPos.x = sum->x;
        i_this->mBodyPos.y = sum->y;
        i_this->mBodyPos.z = sum->z;
    }
    f32 y = gabi::fadds_ppc(i_this->mBodyPos.y, l_HIO().mBodyHeight);
    i_this->mBodyPos.y = y;
    f32 c = gabi::fadds_ppc(gabi::fadds_ppc(i_this->mGroundY, 100.0f), REG0_F(0));
    if (!(y > c))
        i_this->mBodyPos.y = c;
    s16 lz = actor->shape_angle.z;
    s16 ly = actor->shape_angle.y;
    s16 lx = actor->shape_angle.x;
    if (l_HIO().mPoseEditMode != 0)
        ly = l_HIO().mPoseEditAngleY;
    BGN_HAND_MAX() = l_HIO().mHandMax;
    BGN_TAIL_MAX() = l_HIO().mTailMax;
    gabi::Local<dBgS_GndChk> gndChk;
    dBgS_GndChk_ct(gndChk, dBgS_GndChk_vt{0x10008A54, 0x10008A64, 0x10008A84, 0x10008A74}, false);
    {
        gabi::Local<cXyz> pos;
        bgn_copy3(pos, &actor->current.pos);
        f32 py = gabi::fadds_ppc(actor->current.pos.y, 500.0f);
        pos->y = py;
        auto *g = gndChk.get();
        g->m_pos.y = py;
        g->m_pos.x = actor->current.pos.x;
        i_this->mGroundY = gabi::fadds_ppc(REG0_F(16), 90.0f);
        g->m_pos.z = pos->z;
    }
    s16 sVar1 = 0;
    if (i_this->mMove[0].mCut != 0)
        sVar1 = 8000;
    if (i_this->mMove[1].mCut != 0)
        sVar1 = (s16)(sVar1 - 8000);
    cLib_addCalcAngleS2(&i_this->mHeadCutYaw, sVar1, 8, 0x400);
    if (i_this->mMove[2].mCut == 0)
        lx = (s16)(lx + (s16)gabi::ftoi(gabi::fmuls_ppc(i_this->mMove[2].mPose.y,
                                                        gabi::fadds_ppc(REG0_F(11), 8.0f))));
    if (i_this->mMove[3].mCut != 0) {
        lz = (s16)(lz - (REG0_S(0) + 3000));
    } else {
        s16 dz = (s16)gabi::ftoi(
            gabi::fmuls_ppc(i_this->mMove[3].mPose.y, gabi::fadds_ppc(REG0_F(12), 5.0f)));
        s16 dy = (s16)gabi::ftoi(
            -(f32)gabi::fmuls_ppc(i_this->mMove[3].mPose.z, gabi::fadds_ppc(REG0_F(13), 5.0f)));
        lz = (s16)(lz + dz);
        ly = (s16)(ly + dy);
    }
    if (i_this->mMove[4].mCut != 0) {
        lz = (s16)(lz + (REG0_S(0) + 3000));
    } else {
        s16 dz = (s16)gabi::ftoi(
            -(f32)gabi::fmuls_ppc(i_this->mMove[4].mPose.y, gabi::fadds_ppc(REG0_F(12), 5.0f)));
        s16 dy = (s16)gabi::ftoi(
            gabi::fmuls_ppc(i_this->mMove[4].mPose.z, gabi::fadds_ppc(REG0_F(13), 5.0f)));
        lz = (s16)(lz + dz);
        ly = (s16)(ly + dy);
    }
    cLib_addCalcAngleS2(&i_this->mBodyAngle.x, lx, 4, 0x200);
    cLib_addCalcAngleS2(&i_this->mBaseAngleY, ly, 4, 0x1000);
    cLib_addCalcAngleS2(&i_this->mBodyAngle.z, lz, 4, 0x200);
    i_this->mBodyAngle.y = (s16)(i_this->mBaseAngleY + i_this->mDanceAngleY);
    if (l_HIO().mForceCutString0 != 0)
        i_this->mMove[0].mCut = 1;
    if (l_HIO().mForceCutString1 != 0)
        i_this->mMove[1].mCut = 1;
    if (l_HIO().mForceCutString2 != 0)
        i_this->mMove[2].mCut = 1;
    if (l_HIO().mForceCutString3 != 0)
        i_this->mMove[3].mCut = 1;
    if (l_HIO().mForceCutString4 != 0)
        i_this->mMove[4].mCut = 1;
    if (l_HIO().mForceCutString5 != 0)
        i_this->mMove[5].mCut = 1;
    if (l_HIO().mForceCutString6 != 0)
        i_this->mMove[6].mCut = 1;
    if (l_HIO().mForceCutString7 != 0)
        i_this->mMove[7].mCut = 1;
    {
        f32 fVar9 = 0.0f;
        gabi::Local<cXyz> d1;
        gabi::Local<cXyz> d2;
        gabi::Local<cXyz> local_e4;
        for (s32 i = 0; i < 2; i++) {
            if (i == 0 && i_this->mLeftPunchOn != 0) {
                s32 hm = BGN_HAND_MAX();
                cXyz_mi(&i_this->mLeftArmParts[hm].mPartPos, d1, &i_this->mLeftArmParts[hm - 1].mPartPos);
                bgn_copy3(local_e4, d1.get());
                fVar9 = std_sqrtf(PSVECSquareMag(local_e4));
            } else if (i == 1 && i_this->mRightPunchOn != 0) {
                s32 hm = BGN_HAND_MAX();
                cXyz_mi(&i_this->mRightArmParts[hm].mPartPos, d2, &i_this->mRightArmParts[hm - 1].mPartPos);
                auto *s = bgn_move_detail::words(d2.get());
                auto *d = bgn_move_detail::words(local_e4.get());
                u32 z = (u32)s->z, x = (u32)s->x, yy = (u32)s->y;
                d->x = x;
                d->y = yy;
                d->z = z;
                fVar9 = std_sqrtf(PSVECSquareMag(local_e4));
            }
            f32 t = gabi::fadds_ppc(REG0_F(5), 200.0f);
            f32 mArmPartSpacing = l_HIO().mArmPartSpacing;
            if (fVar9 > t) {
                fVar9 = gabi::fsubs_ppc(fVar9, t);
                fVar9 = gabi::fmuls_ppc(fVar9, gabi::fadds_ppc(REG0_F(6), 0.2f));
            } else {
                fVar9 = 0.0f;
            }
            cLib_addCalc2(&i_this->mArmLength[i], gabi::fadds_ppc(mArmPartSpacing, fVar9), 1.0f,
                          gabi::fadds_ppc(REG0_F(4), 20.0f));
            cLib_addCalc0(&i_this->mArmStretch[i], 0.05f, 0.02f);
        }
    }
    /* size_set (inline). HD: the steps are divided (GameCube: reciprocal multiplied) */
    {
        daBgn_HIO_c &h = l_HIO();
        i_this->mHeadParts[0].mPartScale = h.mHeadScale;
        i_this->mPelvisParts[0].mPartScale = h.mPelvisScale;
        i_this->mPelvisParts[1].mPartScale = h.mPelvisScale;
        f32 mArmRootScale = h.mArmRootScale;
        f32 fVar2 = gabi::fsubs_ppc(mArmRootScale, h.mArmTipScale) / (f32)(BGN_HAND_MAX() - 2);
        i_this->mLeftArmParts[0].mPartScale = gabi::fsubs_ppc(mArmRootScale, fVar2);
        i_this->mRightArmParts[0].mPartScale = gabi::fsubs_ppc(h.mArmRootScale, fVar2);
        for (s32 i = 1; i < BGN_HAND_MAX(); i++) {
            i_this->mLeftArmParts[i].mPartScale = gabi::fnmsubs(fVar2, (f32)(i - 1), h.mArmRootScale);
            i_this->mRightArmParts[i].mPartScale = gabi::fnmsubs(fVar2, (f32)(i - 1), h.mArmRootScale);
        }
        i_this->mLeftArmParts[BGN_HAND_MAX()].mPartScale = h.mArmTipScale;
        i_this->mRightArmParts[BGN_HAND_MAX()].mPartScale = h.mArmTipScale;
        f32 a = h.mArmRootScale, b = h.mArmTipScale;
        fVar2 = gabi::fmadds(gabi::fsubs_ppc(a, b), 0.5f, b);
        for (s32 i = 0; i < BGN_HAND_MAX(); i++) {
            f32 l = i_this->mLeftArmParts[i].mPartScale;
            f32 dl = gabi::fmuls_ppc(gabi::fsubs_ppc(fVar2, l), i_this->mArmStretch[0]);
            i_this->mLeftArmParts[i].mPartScale = gabi::fadds_ppc(l, gabi::fadds_ppc(dl, dl));
            f32 r = i_this->mRightArmParts[i].mPartScale;
            f32 dr = gabi::fmuls_ppc(gabi::fsubs_ppc(fVar2, r), i_this->mArmStretch[1]);
            i_this->mRightArmParts[i].mPartScale = gabi::fadds_ppc(r, gabi::fadds_ppc(dr, dr));
        }
        fVar2 = gabi::fmuls_ppc(gabi::fsubs_ppc(h.mLegRootScale, h.mLegTipScale), 0.5f);
        for (s32 i = 0; i < 3; i++) {
            i_this->mLeftLegParts[i].mPartScale = gabi::fnmsubs(fVar2, (f32)i, h.mLegRootScale);
            i_this->mRightLegParts[i].mPartScale = gabi::fnmsubs(fVar2, (f32)i, h.mLegRootScale);
        }
        i_this->mLeftLegParts[3].mPartScale = h.mLegTipScale;
        i_this->mRightLegParts[3].mPartScale = h.mLegTipScale;
        f32 mTailRootScale = h.mTailRootScale;
        fVar2 = gabi::fsubs_ppc(mTailRootScale, h.mTailTipScale) / (f32)(BGN_TAIL_MAX() - 1);
        for (s32 i = 0; i < BGN_TAIL_MAX(); i++) {
            mTailRootScale = h.mTailRootScale;
            i_this->mTailParts[i].mPartScale = gabi::fnmsubs(fVar2, (f32)i, mTailRootScale);
        }
        if (BGN_TAIL_MAX() > 0)
            mTailRootScale = h.mTailRootScale;
        i_this->mRightLegParts[BGN_TAIL_MAX() + 3].mPartScale = gabi::fmuls_ppc(mTailRootScale, 0.7f);
        i_this->mTailParts[BGN_TAIL_MAX()].mPartScale = gabi::fmuls_ppc(h.mTailRootScale, 0.7f);
    }
    OffAtSetBit(&i_this->mBodyAtSph);
    OffAtSetBit(&i_this->mPelvisParts[0].mPartSph);
    OffAtSetBit(&i_this->mLeftArmParts[BGN_HAND_MAX() - 1].mPartSph);
    OffAtSetBit(&i_this->mRightArmParts[BGN_HAND_MAX() - 1].mPartSph);
    for (s32 i = 0; i < BGN_TAIL_MAX(); i++)
        OffAtSetBit(&i_this->mTailParts[i].mPartSph);
    action_main(i_this);
    dComIfGp_get();

    /* damage_check (inline) */
    if (i_this->mDamageTimer == 0) {
        u32 hitObj = 0;
        gabi::Local<be<u32>> atObj;
        if (i_this->mCoreSph.ChkTgHit()) {
            *atObj = gabi::ea(i_this->mCoreSph.GetTgHitObj());
            i_this->mCoreHitFrame = 1;
            i_this->mDamageTimer = 100;
            i_this->mAction = 5;
            i_this->mMode = 0;
            i_this->mRightPunchOn = 0;
            i_this->mLeftPunchOn = 0;
            s8 health = actor->health;
            if (health != 0) {
                s8 room = fopAcM_GetRoomNo(actor);
                actor->health = (s8)(health - 1);
                mDoAud_seStart(bgn_move_detail::JA_SE_LK_ARROW_HIT, nullptr, 0x35,
                               dComIfGp_getReverb(room));
                f32 s = gabi::fadds_ppc(REG0_F(5), 2.0f);
                if (actor->health == 0) {
                    mDoAud_bgmStop(30);
                    gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mItemLifeCount = 0.0f;
                    i_this->mAction = 6;
                    i_this->mMode = 0;
                    JPABaseEmitter *e = dComIfGp_particle_set(
                        bgn_move_detail::ID_AK_SN_KGTBREAKWEAKPOINT00, &i_this->mCorePos);
                    if (e != nullptr)
                        bgn_emitter_scale(e, s);
                    e = dComIfGp_particle_set(bgn_move_detail::ID_AK_SN_KGTBREAKWEAKPOINT01,
                                              &i_this->mCorePos);
                    if (e != nullptr)
                        bgn_emitter_scale(e, s);
                    mDoAud_monsSeStart(bgn_move_detail::JA_SE_CV_BGN_HIT_2, nullptr, 0,
                                       dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                    mDoAud_seStart(bgn_move_detail::JA_SE_LK_LAST_HIT, nullptr, 0,
                                   dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                    mDoAud_seStart(bgn_move_detail::JA_SE_CM_BGN_M_BRK_ORB, nullptr, 0,
                                   dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                } else {
                    JPABaseEmitter *e = dComIfGp_particle_set(
                        bgn_move_detail::ID_AK_SN_KGTHITWEAKPOINT00, &i_this->mCorePos);
                    if (e != nullptr)
                        bgn_emitter_scale(e, s);
                    mDoAud_monsSeStart(bgn_move_detail::JA_SE_CV_BGN_HIT_1, nullptr, 0,
                                       dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                }
            }
        }
        s8 cVar5 = 0;
        if (i_this->mBodyAtSph.ChkTgHit()) {
            *atObj = gabi::ea(i_this->mBodyAtSph.GetTgHitObj());
            cVar5 = 1;
        }
        if (i_this->mHeadParts[0].mPartSph.ChkTgHit()) {
            *atObj = gabi::ea(i_this->mHeadParts[0].mPartSph.GetTgHitObj());
            cVar5 = 2;
        }
        if (i_this->mPelvisParts[0].mPartSph.ChkTgHit()) {
            hitObj = gabi::ea(i_this->mPelvisParts[0].mPartSph.GetTgHitObj());
            cVar5 = 3;
        } else {
            hitObj = *atObj;
        }
        for (s32 i = 0; i < 20; i++) {
            if (i_this->mLeftArmParts[i].mPartSph.ChkTgHit()) {
                hitObj = gabi::ea(i_this->mLeftArmParts[i].mPartSph.GetTgHitObj());
                cVar5 = 4;
            }
            if (i_this->mRightArmParts[i].mPartSph.ChkTgHit()) {
                hitObj = gabi::ea(i_this->mRightArmParts[i].mPartSph.GetTgHitObj());
                cVar5 = 5;
            }
        }
        for (s32 i = 0; i < 3; i++) {
            if (i_this->mLeftLegParts[i].mPartSph.ChkTgHit()) {
                hitObj = gabi::ea(i_this->mLeftLegParts[i].mPartSph.GetTgHitObj());
                cVar5 = 6;
            }
            if (i_this->mRightLegParts[i].mPartSph.ChkTgHit()) {
                hitObj = gabi::ea(i_this->mRightLegParts[i].mPartSph.GetTgHitObj());
                cVar5 = 7;
            }
        }
        for (s32 i = 0; i < 20; i++) {
            if (i_this->mTailParts[i].mPartSph.ChkTgHit()) {
                hitObj = gabi::ea(i_this->mTailParts[i].mPartSph.GetTgHitObj());
                cVar5 = 8;
            }
        }
        if (cVar5 != 0 && i_this->mPartHitSeTimer == 0) {
            i_this->mPartHitSeTimer = 10;
            def_se_set(actor, hitObj, 0x44);
        }
    }

    /* shape_calc (inline) */
    gabi::Local<cXyz> local_8c;
    gabi::Local<cXyz> cStack_98;
    gabi::Local<cXyz> sum;
    {
        f32 hy = (f32)(s32)(s16)l_HIO().mBodyDrawOffsetY;
        f32 c2 = gabi::fadds_ppc(gabi::fadds_ppc(i_this->mGroundY, 100.0f), REG0_F(0));
        f32 by = gabi::fadds_ppc(i_this->mBodyPos.y, hy);
        f32 bz = i_this->mBodyPos.z;
        s16 mCoreHitFrame = i_this->mCoreHitFrame;
        f32 bx = i_this->mBodyPos.x;
        if (!(by > c2))
            by = c2;
        if (mCoreHitFrame == 0xD) {
            i_this->mArrowHitFlashTimer = 50;
            i_this->mArrowHitEffectTimer = 100;
        }
        MtxTrans(bx, by, bz, false);
    }
    s16 sVar8 = 0;
    s16 sVar5 = 0;
    if (i_this->mShakeTimer != 0) {
        s16 t = (s16)(i_this->mShakeTimer - 1);
        i_this->mShakeTimer = t;
        s16 c746 = i_this->mFrameCounter;
        f32 f = gabi::fmuls_ppc((f32)t, gabi::fadds_ppc(REG0_F(14), 300.0f));
        sVar8 = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_ssin(c746 * 0x1C00), f));
        sVar5 = (s16)gabi::ftoi(gabi::fmuls_ppc(cM_scos(c746 * 0x1900), f));
    }
    cMtx_YrotM(calc_mtx(), i_this->mBodyAngle.y);
    cMtx_XrotM(calc_mtx(), (s16)(i_this->mBodyAngle.x + sVar8));
    cMtx_ZrotM(calc_mtx(), (s16)(i_this->mBodyAngle.z + sVar5));
    f32 dVar12 = gabi::fmuls_ppc(l_HIO().mChestScale, l_HIO().mScale);
    MtxScale(dVar12, dVar12, dVar12, true);
    MtxPush();
    cMtx_XrotM(calc_mtx(), l_HIO().mChestRotX);
    if (i_this->mChestSquash > 0.0f) {
        s16 s = (s16)(i_this->mFrameCounter * (REG0_S(4) + 0x900));
        cMtx_ZrotM(calc_mtx(), s);
        f32 k = i_this->mChestSquash;
        f32 a = gabi::fadds_ppc(k, 1.0f);
        MtxScale(a, gabi::fsubs_ppc(1.0f, k), a, true);
        cMtx_ZrotM(calc_mtx(), (s16)-s);
    }
    bgn_mtx_set(i_this->mpChestModel);
    i_this->mBodyAtSph.SetR(gabi::fmuls_ppc(dVar12, gabi::fadds_ppc(REG0_F(1), 120.0f)));
    i_this->mBodyAtSph.SetC(&i_this->mBodyPos);
    dComIfG_Ccsp_Set(&i_this->mBodyAtSph);
    if (i_this->mArrowHitEffectTimer != 0) {
        s16 t = (s16)(i_this->mArrowHitEffectTimer - 1);
        i_this->mArrowHitEffectTimer = t;
        f32 fVar3 =
            gabi::fmuls_ppc(gabi::fmuls_ppc(dVar12, (f32)t), gabi::fadds_ppc(REG_F(8, 1), 0.03f));
        if (i_this->mpArrowHitEmitter1 == nullptr) {
            i_this->mpArrowHitEmitter1 =
                dComIfGp_particle_set(bgn_move_detail::ID_AK_JN_CCTHUNDER00, &i_this->mBodyPos);
        } else {
            JPABaseEmitter_setGlobalTranslation(i_this->mpArrowHitEmitter1, i_this->mBodyPos.x,
                                                i_this->mBodyPos.y, i_this->mBodyPos.z);
            bgn_emitter_scale(i_this->mpArrowHitEmitter1, fVar3);
        }
        if (i_this->mpArrowHitEmitter2 == nullptr) {
            local_8c->y = dVar12;
            local_8c->z = dVar12;
            local_8c->x = dVar12;
            i_this->mpArrowHitEmitter2 =
                dComIfGp_particle_set(bgn_move_detail::ID_AK_JN_CCTHUNDER01, &i_this->mBodyPos);
        } else {
            JPABaseEmitter_setGlobalTranslation(i_this->mpArrowHitEmitter2, i_this->mBodyPos.x,
                                                i_this->mBodyPos.y, i_this->mBodyPos.z);
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
    f32 dVar12b = gabi::fmuls_ppc(l_HIO().mBodyPartSpacing, l_HIO().mScale);
    f32 dVar11 = gabi::fmuls_ppc(l_HIO().mLegPartSpacing, l_HIO().mScale);
    f32 dVar10 = gabi::fmuls_ppc(i_this->mArmLength[0], l_HIO().mScale);
    f32 dVar9 = gabi::fmuls_ppc(i_this->mArmLength[1], l_HIO().mScale);
    f32 dVar13 = gabi::fmuls_ppc(l_HIO().mTailPartSpacing, l_HIO().mScale);
    MtxPush();
    local_8c->y = l_HIO().mHeadRootPosY;
    local_8c->x = l_HIO().mHeadRootPosX;
    local_8c->z = l_HIO().mHeadRootPosZ;
    MtxPosition(local_8c, &i_this->mHeadParts[0].mPartPos);
    part_control_0(i_this, 1, &i_this->mHeadParts[0], &i_this->mMove[0], dVar12b);
    if (i_this->mMove[0].mCut == 0 || i_this->mMove[1].mCut == 0) {
        bgn_limb_end(i_this, local_8c, &l_HIO().mHeadEndPosX, &i_this->mMove[0],
                     &i_this->mHeadParts[1].mPartPos);
        part_control_2(i_this, 1, &i_this->mHeadParts[0], dVar12b);
    }
    part_mtx_set(i_this, 1, &i_this->mHeadParts[0], 0, 0);
    {
        auto *src = bgn_move_detail::words(&i_this->mHeadParts[0].mPartPos);
        auto *eyePos = bgn_move_detail::words(&actor->eyePos);
        auto *attentionPos = bgn_move_detail::words(&actor->attention_info.position);
        u32 x = (u32)src->x, yy = (u32)src->y;
        eyePos->x = x;
        u32 z = (u32)src->z;
        eyePos->y = yy;
        eyePos->z = z;
        attentionPos->x = x;
        attentionPos->y = yy;
        f32 ay = gabi::fadds_ppc(actor->attention_info.position.y, 50.0f);
        attentionPos->z = z;
        actor->attention_info.position.y = ay;
    }
    MtxPull();
    MtxPush();
    {
        f32 k = i_this->mPartLengthScale;
        local_8c->x = gabi::fmuls_ppc(l_HIO().mPelvisRootPosX, k);
        local_8c->y = gabi::fmuls_ppc(gabi::fadds_ppc(l_HIO().mPelvisRootPosY, 20.0f), k);
        local_8c->z = gabi::fmuls_ppc(l_HIO().mPelvisRootPosZ, k);
    }
    MtxPosition(local_8c, &i_this->mPelvisParts[0].mPartPos);
    part_control_0(i_this, 1, &i_this->mPelvisParts[0], &i_this->mMove[2], dVar12b);
    if (i_this->mMove[2].mCut == 0) {
        bgn_limb_end(i_this, local_8c, &l_HIO().mPelvisEndPosX, &i_this->mMove[2],
                     &i_this->mPelvisParts[1].mPartPos);
        part_control_2(i_this, 1, &i_this->mPelvisParts[0], dVar12b);
    }
    part_mtx_set(i_this, 1, &i_this->mPelvisParts[0], 2, 0);
    MtxPull();
    MtxPush();
    {
        f32 k = i_this->mPartLengthScale;
        local_8c->x = gabi::fmuls_ppc(l_HIO().mShoulderPosX, k);
        local_8c->y = gabi::fmuls_ppc(l_HIO().mShoulderPosY, k);
        local_8c->z = gabi::fmuls_ppc(l_HIO().mShoulderPosZ, k);
    }
    MtxPosition(local_8c, &i_this->mLeftArmParts[0].mPartPos);
    if (i_this->mMove[3].mCut == 0) {
        part_control_0(i_this, BGN_HAND_MAX(), &i_this->mLeftArmParts[0], &i_this->mMove[3],
                       dVar10);
        if (i_this->mLeftPunchOn != 0) {
            bgn_copy3(&i_this->mLeftArmParts[BGN_HAND_MAX()].mPartPos, &i_this->mLeftPunchPos);
        } else {
            cMtx_YrotS(calc_mtx(), i_this->mBodyAngle.y);
            local_8c->x = l_HIO().mLeftHandPosX;
            local_8c->z = l_HIO().mLeftHandPosZ;
            local_8c->y = l_HIO().mLeftHandPosY;
            PSVECAdd(local_8c, &i_this->mMove[3].mPose, local_8c);
            MtxPosition(local_8c, &i_this->mLeftArmParts[BGN_HAND_MAX()].mPartPos);
            cXyz *e = &i_this->mLeftArmParts[BGN_HAND_MAX()].mPartPos;
            PSVECAdd(e, &i_this->mBodyPos, e);
        }
        part_control_2(i_this, BGN_HAND_MAX(), &i_this->mLeftArmParts[0], dVar10);
    } else {
        part_control_0(i_this, BGN_HAND_MAX(), &i_this->mLeftArmParts[0], &i_this->mMove[3],
                       dVar10);
    }
    part_mtx_set(i_this, BGN_HAND_MAX(), &i_this->mLeftArmParts[0], 3, BGN_HAND_MAX() - 1);
    MtxPull();
    {
        f32 k = i_this->mPartLengthScale;
        local_8c->x = gabi::fmuls_ppc(-(f32)l_HIO().mShoulderPosX, k);
        local_8c->y = gabi::fmuls_ppc(l_HIO().mShoulderPosY, k);
        local_8c->z = gabi::fmuls_ppc(l_HIO().mShoulderPosZ, k);
    }
    MtxPosition(local_8c, &i_this->mRightArmParts[0].mPartPos);
    if (i_this->mMove[4].mCut == 0) {
        part_control_0(i_this, BGN_HAND_MAX(), &i_this->mRightArmParts[0], &i_this->mMove[4],
                       dVar9);
        if (i_this->mRightPunchOn != 0) {
            bgn_copy3(&i_this->mRightArmParts[BGN_HAND_MAX()].mPartPos, &i_this->mRightPunchPos);
        } else {
            cMtx_YrotS(calc_mtx(), i_this->mBodyAngle.y);
            local_8c->x = l_HIO().mRightHandPosX;
            local_8c->z = l_HIO().mRightHandPosZ;
            local_8c->y = l_HIO().mRightHandPosY;
            PSVECAdd(local_8c, &i_this->mMove[4].mPose, local_8c);
            MtxPosition(local_8c, &i_this->mRightArmParts[BGN_HAND_MAX()].mPartPos);
            cXyz *e = &i_this->mRightArmParts[BGN_HAND_MAX()].mPartPos;
            PSVECAdd(e, &i_this->mBodyPos, e);
        }
        part_control_2(i_this, BGN_HAND_MAX(), &i_this->mRightArmParts[0], dVar9);
    } else {
        part_control_0(i_this, BGN_HAND_MAX(), &i_this->mRightArmParts[0], &i_this->mMove[4],
                       dVar9);
    }
    part_mtx_set(i_this, BGN_HAND_MAX(), &i_this->mRightArmParts[0], 4, BGN_HAND_MAX() - 1);
    cMtx_XrotS(calc_mtx(), i_this->mPelvisParts[0].mPartAngle.x);
    cMtx_YrotM(calc_mtx(), i_this->mPelvisParts[0].mPartAngle.y);
    cMtx_ZrotM(calc_mtx(), (s16)-i_this->mPelvisParts[0].mPartAngle.z);
    {
        f32 s = gabi::fmuls_ppc(l_HIO().mPelvisScale, l_HIO().mScale);
        MtxScale(s, s, s, true);
    }
    MtxPush();
    {
        f32 k = i_this->mPartLengthScale;
        local_8c->x = gabi::fmuls_ppc(l_HIO().mHipPosX, k);
        local_8c->y = gabi::fmuls_ppc(l_HIO().mHipPosY, k);
        local_8c->z = gabi::fmuls_ppc(l_HIO().mHipPosZ, k);
    }
    MtxPosition(local_8c, cStack_98);
    cXyz_pl(&i_this->mPelvisParts[0].mPartPos, sum, cStack_98);
    bgn_copy3(&i_this->mLeftLegParts[0].mPartPos, sum.get());
    part_control_0(i_this, 3, &i_this->mLeftLegParts[0], &i_this->mMove[5], dVar11);
    if (i_this->mMove[5].mCut == 0) {
        bgn_limb_end(i_this, local_8c, &l_HIO().mLeftFootPosX, &i_this->mMove[5],
                     &i_this->mLeftLegParts[3].mPartPos);
        part_control_2(i_this, 3, &i_this->mLeftLegParts[0], dVar11);
    }
    part_mtx_set(i_this, 3, &i_this->mLeftLegParts[0], 5, 2);
    MtxPull();
    MtxPush();
    {
        f32 k = i_this->mPartLengthScale;
        local_8c->x = gabi::fmuls_ppc(-(f32)l_HIO().mHipPosX, k);
        local_8c->y = gabi::fmuls_ppc(l_HIO().mHipPosY, k);
        local_8c->z = gabi::fmuls_ppc(l_HIO().mHipPosZ, k);
    }
    MtxPosition(local_8c, cStack_98);
    cXyz_pl(&i_this->mPelvisParts[0].mPartPos, sum, cStack_98);
    bgn_copy3(&i_this->mRightLegParts[0].mPartPos, sum.get());
    part_control_0(i_this, 3, &i_this->mRightLegParts[0], &i_this->mMove[6], dVar11);
    if (i_this->mMove[6].mCut == 0) {
        bgn_limb_end(i_this, local_8c, &l_HIO().mRightFootPosX, &i_this->mMove[6],
                     &i_this->mRightLegParts[3].mPartPos);
        part_control_2(i_this, 3, &i_this->mRightLegParts[0], dVar11);
    }
    part_mtx_set(i_this, 3, &i_this->mRightLegParts[0], 6, 2);
    MtxPull();
    {
        f32 k = i_this->mPartLengthScale;
        local_8c->x = gabi::fmuls_ppc(l_HIO().mTailRootPosX, k);
        local_8c->y = gabi::fmuls_ppc(l_HIO().mTailRootPosY, k);
        local_8c->z = gabi::fmuls_ppc(l_HIO().mTailRootPosZ, k);
    }
    MtxPosition(local_8c, cStack_98);
    cXyz_pl(&i_this->mPelvisParts[0].mPartPos, sum, cStack_98);
    bgn_copy3(&i_this->mTailParts[0].mPartPos, sum.get());
    s32 tailMax = BGN_TAIL_MAX();
    if (i_this->mMove[7].mCut == 0) {
        part_control_0(i_this, tailMax, &i_this->mTailParts[0], &i_this->mMove[7], dVar13);
        cMtx_YrotS(calc_mtx(), i_this->mBodyAngle.y);
        local_8c->x = l_HIO().mTailEndPosX;
        local_8c->y = l_HIO().mTailEndPosY;
        local_8c->z = l_HIO().mTailEndPosZ;
        PSVECAdd(local_8c, &i_this->mMove[7].mPose, local_8c);
        MtxPosition(local_8c, &i_this->mTailParts[BGN_TAIL_MAX()].mPartPos);
        cXyz *e = &i_this->mTailParts[BGN_TAIL_MAX()].mPartPos;
        PSVECAdd(e, &i_this->mBodyPos, e);
        f32 c3 = i_this->mGroundY;
        part_s *last = &i_this->mTailParts[BGN_TAIL_MAX()];
        if (!(last->mPartPos.y > c3))
            last->mPartPos.y = c3;
        part_control_2(i_this, BGN_TAIL_MAX(), &i_this->mTailParts[0], dVar13);
        part_mtx_set(i_this, BGN_TAIL_MAX(), &i_this->mTailParts[0], 7, BGN_TAIL_MAX() - 1);
    } else {
        /* part_control_0Z (inline) */
        s32 param_2 = tailMax;
        part_s *param_3 = &i_this->mTailParts[0];
        move_s *param_4 = &i_this->mMove[7];
        f32 param_5 = dVar13;
        gabi::Local<cXyz> local_f8;
        gabi::Local<cXyz> local_128;
        gabi::Local<cXyz> local_134;
        gabi::Local<cXyz> cStack_104;
        gabi::Local<cXyz> psum;
        cMtx_YrotS(calc_mtx(), i_this->mBodyAngle.y);
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
        f32 dVar10z = i_this->mGroundY;
        f32 dVar9z = l_HIO().mPartDropCut;
        param_3++;
        for (s32 i = 1; i < param_2 + 1; i++, param_3++) {
            part_s *prev = param_3 - 1;
            f32 py = param_3->mPartPos.y;
            f32 dVar11z = gabi::fadds_ppc(py, dVar9z);
            if (!(dVar11z > dVar10z)) {
                dVar11z = dVar10z;
                if ((u32)i == (u32)param_2 && (i_this->mFrameCounter & 7) == 0) {
                    local_134->y = py;
                    local_134->x = param_3->mPartPos.x;
                    local_134->z = param_3->mPartPos.z;
                    if (!gr_check(i_this, local_134)) {
                        dComIfGp_particle_setSimple(bgn_move_detail::ID_AK_SN_O_KGTCOMMONHAMON03,
                                                    local_134, 0xFF);
                        if (i_this->mRippleSeTimer == 0) {
                            f32 r = gabi::fadds_ppc(cM_rndF(20.0f), 20.0f);
                            u32 sx = (u32)bgn_move_detail::words(local_134.get())->y,
                                sz = (u32)bgn_move_detail::words(local_134.get())->z,
                                sxx = (u32)bgn_move_detail::words(local_134.get())->x;
                            auto *dst = bgn_move_detail::words(&i_this->mRipplePos);
                            dst->x = sxx;
                            dst->y = sx;
                            dst->z = sz;
                            i_this->mRippleSeTimer = (s16)gabi::ftoi(r);
                            mDoAud_seStart(bgn_move_detail::JA_SE_CM_BGN_BODY_RIPPLE,
                                           &i_this->mRipplePos, 0,
                                           dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
                        }
                    }
                }
            }
            f32 f1 = (f32)(param_2 - i);
            s16 c3 = REG0_S(3);
            f32 m2f4 = param_4->mLimbShakeAmplitude;
            f32 x = gabi::fmadds(local_128->x, f1, gabi::fsubs_ppc(param_3->mPartPos.x, prev->mPartPos.x));
            s16 iVar2 =
                (s16)gabi::ftoi(gabi::fmuls_ppc(cM_ssin(param_4->mPhaseSin + i * (c3 + 8000)), m2f4));
            f32 dz = gabi::fsubs_ppc(param_3->mPartPos.z, prev->mPartPos.z);
            f32 yv = gabi::fsubs_ppc(dVar11z, prev->mPartPos.y);
            f32 z = gabi::fmadds(local_128->z, f1, dz);
            s16 iVar4 = (s16)gabi::ftoi(
                gabi::fmuls_ppc(cM_scos(param_4->mPhaseCos + i * (REG0_S(4) + 9000)), m2f4));
            s16 a = cM_atan2s(x, z);
            prev->mPartAngle.y = (s16)(a + iVar4);
            f32 sq = std_sqrtf(gabi::fmadds(x, x, gabi::fmuls_ppc(z, z)));
            s16 b = cM_atan2s(yv, sq);
            s16 ry = prev->mPartAngle.y;
            prev->mPartAngle.x = (s16)(iVar2 - b);
            cMtx_YrotS(calc_mtx(), ry);
            cMtx_XrotM(calc_mtx(), prev->mPartAngle.x);
            local_f8->x = 0.0f;
            local_f8->y = 0.0f;
            local_f8->z = gabi::fmuls_ppc(gabi::fmuls_ppc(param_5, prev->mPartScale), i_this->mPartLengthScale);
            MtxPosition(local_f8, cStack_104);
            cXyz_pl(&prev->mPartPos, psum, cStack_104);
            bgn_copy3(&param_3->mPartPos, psum.get());
        }
        part_mtx_set(i_this, BGN_TAIL_MAX(), &i_this->mTailParts[0], 7, BGN_TAIL_MAX() - 1);
    }
    for (s32 i = 0; i < 8; i++)
        action_s_himo(i_this, &i_this->mMove[i], i);
    /* ~dBgS_GndChk (inline): the base vtables back, then ~cBgS_Chk */
    {
        auto *g = gndChk.get();
        g->cBgS_PolyInfo::__vtbl = bgn_move_detail::gnd_cBgS_PolyInfo_vt;
        g->dBgS_PolyPassChk::__vtbl = bgn_move_detail::gnd_dBgS_PolyPassChk_vt;
        g->dBgS_GrpPassChk::__vtbl = bgn_move_detail::gnd_dBgS_GrpPassChk_vt;
        bgn_move_chk_dt(gndChk.get(), 0);
    }
}

// clang-format off
/* WWHD glue
ext 02518CC8 void def_se_set(fopAc_ac_c*,u32,u32)
ext 025E1904 void mDoAud_bgmStop(u32)
ext 025E1A7C void mDoAud_monsSeStart(u32,cXyz*,u32,s32)
ext 02008DAC void bgn_move_chk_dt(dBgS_GndChk*,s32)
*/
