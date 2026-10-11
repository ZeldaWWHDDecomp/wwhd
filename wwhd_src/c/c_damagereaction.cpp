/* WWHD damage reactions.
 * Complete fourteen-entry HD inventory 020402C8..0204698C. */
#include "c/c_damagereaction.h"
using gabi::load;
using gabi::store;

static DamageHeaderStatics l_reactionHeaderStatics;
static DamageHeaderStatics l_drawHeaderStatics;
static cXyz non_pos;
static u8 test_sw;
static s32 kado_bit[] = {1, 2};
static f32 kado_check_x[] = {80.0f, -80.0f};

static void enemy_fire_remove(u32 fire);
static void enemy_piyo_set(u32 actor);
static void init_nonpos();
static void enemy_seStart(u32 actor, u32 id, u32 flags);
static u32 vec_ctor(u32 p);
static void vec_copy(u32 out, u32 in);
static void sdk_init();
static u32 sdk_ctor(u32 p);
static void line_init(u32 p);
static void line_destroy(u32 p);
static s32 kado_check(u32 dr);
static s32 wall_angle_get(u32 actor, s32 angle);
static void enemy_fire(u32 fire);
static bool ice_bg_check(u32 ice, f32 savedY, f32 zero);
static void ice_particle(u32 id, u32 pos, u32 angle, u32 scale);
static void ice_pos_scale(u32 ice, u32 actor, u32 pos, u32 scale);
static void ice_render(u32 ice, u32 actor, f32 shiverX, f32 shiverZ, f32 one);
static u32 enemy_ice(u32 ice);
static void dr_damage_anime(damagereaction* dr);
static void dr_matrix_set(damagereaction* dr);
static s32 damage_reaction(u32 address);
static s16 hang_ang_get(damagereaction* dr, u32 scratch, u32 auxiliary);
static void dr_body_bg_check(damagereaction* dr, u32 scratch);
static s32 dr_joint_bg_check(damagereaction* dr, u32 scratch, u32 auxiliary);
static void damage_sound_inline(fopAc_ac_c* actor, u32 id);
static s32 dr_damage_set(u32 address);

/* The line object is the full dBgS_LinChk, including both embedded pass checks. */
static void line_init(u32 p) {
    auto* chk = damageView<DamageLine>(p);
    cBgS_LinChk_ct(damageView<DamageLine>(p));
    chk->cBgS_Chk::__vtbl = 0x10006A14;
    chk->cBgS_PolyInfo::__vtbl = 0x10006A24;
    chk->dBgS_GrpPassChk::mGrp = 1;
    chk->dBgS_GrpPassChk::__vtbl = 0x10006A34;
    chk->mbObjThrough = 0;
    chk->mbCamThrough = 0;
    chk->mbLinkThrough = 0;
    chk->mbArrowThrough = 0;
    chk->mbBombThrough = 0;
    chk->mbBoomerangThrough = 0;
    chk->mbRopeThrough = 0;
    chk->cBgS_Chk::mGrpPassChk = p + 0x64;
    chk->dBgS_PolyPassChk::__vtbl = 0x10006A44;
    chk->cBgS_Chk::mPolyPassChk = p + 0x58;
}

static void line_destroy(u32 p) {
    auto* chk = damageView<DamageLine>(p);
    chk->dBgS_PolyPassChk::__vtbl = 0x10006A44;
    chk->dBgS_GrpPassChk::__vtbl = 0x10006904;
    chk->cBgS_PolyInfo::__vtbl = 0x100068F4;
    cBgS_LinChk_dt(damageView<DamageLine>(p), 0);
}

/* Inlined HD ice_bg_check portion02040CDC..02040E94; composed into the ice entry next. */
static bool ice_bg_check(u32 ice, f32 savedY, f32 zero) {
    auto* ei = damageView<enemyice>(ice);

    u32 actor = static_cast<u32>(damageWord(ei->mpActor));
    auto* ac = damageView<fopAc_ac_c>(actor);
    bool shatter = false;
    u32 play = dComIfGp_ea();
    damageView<dBgS_ObjAcch>(ice + 0x1F4)->CrrPos(damageView<dBgS>(play + 0x12A0));
    u32 flags = static_cast<u32>(damageWord(ei->mBgAcch.m_flags));
    if (flags & 0x800) {
        if (static_cast<f32>(ac->current.pos.y) <
            static_cast<f32>(ei->mBgAcch.m_wtr.m_height) + 100.0f) {
            ei->mSpeed.y = static_cast<f32>(zero);
            if (static_cast<s8>(ac->health) <= 0)
                shatter = true;
        }
    } else if (flags & 0x20) {
        ei->mSpeedF = static_cast<f32>(static_cast<f32>(ei->mSpeedF) * 0.5f);
        if (static_cast<s8>(ac->health) <= 0 || savedY < REG0_F(14) + -40.0f)
            shatter = true;
        f32 choose = savedY - -20.0f;
        ei->mSpeed.y = static_cast<f32>((f32)ppc_fsel(choose, 10.0f, zero));
        ei->mAngularVelY = static_cast<s16>(0);
    } else {
        if (daSea_ChkArea(static_cast<f32>(ac->current.pos.x),
                          static_cast<f32>(ac->current.pos.z)) != 0) {
            f32 wave = daSea_calcWave(static_cast<f32>(ac->current.pos.x),
                                      static_cast<f32>(ac->current.pos.z));
            if (static_cast<f32>(ac->current.pos.y) < wave + 100.0f) {
                ei->mSpeed.y = static_cast<f32>(zero);
                if (static_cast<s8>(ac->health) <= 0)
                    shatter = true;
            }
        }
    }
    if (static_cast<u32>(damageWord(ei->mBgAcch.m_flags)) & 0x10) {
        f32 speed = static_cast<f32>(ei->mSpeedF);
        if (std::fabs(speed) > 10.0f)
            shatter = true;
        ei->mAngleY = static_cast<s16>((s16)(static_cast<s16>(ei->mAngleY) - 0x8000));
        ei->mSpeedF = static_cast<f32>(speed * 0.5f);
    }
    return shatter;
}

static void ice_particle(u32 id, u32 pos, u32 angle, u32 scale) {
    u32 play = dComIfGp_ea();
    damageView<dPa_control_c>(
        static_cast<u32>(damageWord(damageView<dComIfG_inf_c>(play)->play.mParticle)))
        ->set(0, static_cast<u16>(id), damageView<cXyz>(pos), damageView<csXyz>(angle),
              damageView<cXyz>(scale), 255, nullptr, -1, nullptr, nullptr, nullptr);
}

static void ice_pos_scale(u32 ice, u32 actor, u32 pos, u32 scale) {
    auto* ei = damageView<enemyice>(ice);
    auto* ac = damageView<fopAc_ac_c>(actor);

    auto* particlePos = damageView<cXyz>(pos);
    auto* particleScale = damageView<cXyz>(scale);
    f32 size = ei->mParticleScale;
    particleScale->x = size;
    particleScale->y = size;
    particleScale->z = size;
    // Preserve the scalar float copies and the subsequent Y reload.
    particlePos->x = static_cast<f32>(ac->current.pos.x);
    particlePos->y = static_cast<f32>(ac->current.pos.y);
    particlePos->z = static_cast<f32>(ac->current.pos.z);
    particlePos->y = static_cast<f32>(particlePos->y) + static_cast<f32>(ei->mYOffset);
}

static void ice_render(u32 ice, u32 actor, f32 shiverX, f32 shiverZ, f32 one) {
    auto* ei = damageView<enemyice>(ice);
    auto* ac = damageView<fopAc_ac_c>(actor);

    ac->current.angle.x = static_cast<u16>(static_cast<u16>(ac->shape_angle.x));
    ac->current.angle.y = static_cast<u16>(static_cast<u16>(ac->shape_angle.y));
    ac->current.angle.z = static_cast<u16>(static_cast<u16>(ac->shape_angle.z));
    cLib_addCalc2(&ei->m028, static_cast<f32>(ei->m02C), one, 3.0f);
    PSMTXTrans(damageView<Mtx34>(0x1048D0CC), static_cast<f32>(ac->current.pos.x) + shiverX,
               static_cast<f32>(ac->current.pos.y) + static_cast<f32>(ei->m02C),
               static_cast<f32>(ac->current.pos.z) + shiverZ);
    cMtx_YrotM(damageView<Mtx34>(0x1048D0CC), static_cast<s16>(ac->shape_angle.y));
    cMtx_XrotM(damageView<Mtx34>(0x1048D0CC), static_cast<s16>(ac->shape_angle.x));
    cMtx_ZrotM(damageView<Mtx34>(0x1048D0CC), static_cast<s16>(ac->shape_angle.z));
    mDoMtx_stack_c::scaleM(static_cast<f32>(ac->scale.x), static_cast<f32>(ac->scale.y),
                           static_cast<f32>(ac->scale.z));
    damageWord(ac->actor_status) =
        static_cast<u32>(static_cast<u32>(damageWord(ac->actor_status)) | 0x400);
}

/* 020402C8 */
static u32 enemy_ice(u32 ice) {
    auto* ei = damageView<enemyice>(ice);

    u32 play = dComIfGp_ea(), actor = static_cast<u32>(damageWord(ei->mpActor)),
        player = static_cast<u32>(
            damageWord(damageView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer));
    auto* playerActor = damageView<fopAc_ac_c>(player);
    auto* ac = damageView<fopAc_ac_c>(actor);
    f32 small = 0.1f, zero = 0.0f, one = 1.0f, gravity = 5.0f;
    u32 matrix = gabi::ea(calc_mtx());
    cXyz position;
    cXyz scale;
    cXyz direction;
    cXyz offset;
    u32 pos = gabi::ea((&position)), sc = gabi::ea((&scale)), dir = gabi::ea((&direction)),
        off = gabi::ea((&offset));
    auto soundIce = [&](u32 id) {
        if (actor + 0x37C) {
            s32 rv = dComIfGp_getReverb(static_cast<s8>(ac->current.roomNo));
            mDoAud_seStart(id, damageView<cXyz>(actor + 0x37C), 0, rv);
        }
    };
    auto item = [&]() {
        gabi::call(0x025D9154, pos, static_cast<u32>(damageWord(ac->itemTableIdx)),
                   static_cast<s8>(ac->current.roomNo), actor + 0x320,
                   static_cast<u8>(ac->stealItemBitNo));
    };
    auto freeActor = [&]() {
        fopAcM_delete(ac);
        dComIfGs_onActor(static_cast<u16>(ac->setID), static_cast<s8>(ac->home.roomNo));
    };
    s8 light = static_cast<s8>(ei->mLightShrinkTimer);
    if (light) {
        ice_pos_scale(ice, actor, pos, sc);
        ei->mLightShrinkTimer = static_cast<s8>((s8)(light + 1));
        if (light == 1) {
            ice_particle(0x272, pos, 0, sc);
            ac->tevStr.mFogColor.b = static_cast<u16>(255);
            ac->tevStr.mFogColor.g = static_cast<u16>(255);
            ac->tevStr.mFogColor.r = static_cast<u16>(255);
            ac->tevStr.mFogStartZ = static_cast<f32>(zero);
            ac->tevStr.mFogEndZ = static_cast<f32>(2000.0f);
            soundIce(0x5904);
            damageWord(ac->attention_info.flags) =
                static_cast<u32>(static_cast<u32>(damageWord(ac->attention_info.flags)) & ~4u);
            return 1;
        }
        cLib_addCalc2(damageView<be<f32>>(actor + 0x1BC), 10.0f, one, 80.0f);
        PSMTXTrans(damageView<Mtx34>(0x1048D0CC), static_cast<f32>(ac->current.pos.x),
                   static_cast<f32>(ac->current.pos.y), static_cast<f32>(ac->current.pos.z));
        mDoMtx_stack_c::transM(zero, static_cast<f32>(ei->mYOffset), zero);
        mDoMtx_stack_c::scaleM(static_cast<f32>(ei->mScaleXZ), static_cast<f32>(ei->mScaleY),
                               static_cast<f32>(ei->mScaleXZ));
        mDoMtx_stack_c::transM(zero, -static_cast<f32>(ei->mYOffset), zero);
        cMtx_YrotM(damageView<Mtx34>(0x1048D0CC), static_cast<s16>(ac->shape_angle.y));
        cMtx_XrotM(damageView<Mtx34>(0x1048D0CC), static_cast<s16>(ac->shape_angle.x));
        cMtx_ZrotM(damageView<Mtx34>(0x1048D0CC), static_cast<s16>(ac->shape_angle.z));
        mDoMtx_stack_c::scaleM(static_cast<f32>(ac->scale.x), static_cast<f32>(ac->scale.y),
                               static_cast<f32>(ac->scale.z));
        s8 current = static_cast<s8>(ei->mLightShrinkTimer), threshold = (s8)(REG_S(14, 1) + 70);
        if (current < threshold) {
            f32 step = REG_F(14, 0) + 0.01f;
            cLib_addCalc0(&ei->mScaleXZ, small, step);
            cLib_addCalc0(&ei->mScaleY, small, REG_F(14, 0) + 0.01f);
            return 1;
        }
        if (current == threshold)
            soundIce(0x5905);
        cLib_addCalc2(&ei->mScaleY, REG_F(14, 1) + gravity, small, REG_F(14, 2) + one);
        cLib_addCalc0(&ei->mScaleXZ, REG_F(14, 3) + small, REG_F(14, 4) + 0.05f);
        if (static_cast<s8>(ei->mLightShrinkTimer) > (s8)(REG_S(14, 2) + 90)) {
            freeActor();
            if (!(actor && static_cast<u16>(ac->mProcName) == 210))
                item();
            if (static_cast<u8>(ei->mDeathSwitch))
                dComIfGs_onSwitch(static_cast<u8>(ei->mDeathSwitch),
                                  static_cast<s8>(ac->current.roomNo));
            ac->health = static_cast<s8>(-128);
        }
        return 1;
    }
    u32 frozen = 0;
    bool movement = false;
    switch (static_cast<s8>(ei->mMode)) {
    case 0:
        ei->mStts.Init(250, 255, ac);
        ei->mCyl.Set(damageView<dCcD_SrcCyl>(0x1018F5F0));
        damageWord(ei->mCyl.mStts) = static_cast<u32>(ice + 0x30);
        ei->mCyl.SetR(ei->mWallRadius);
        ei->mCyl.SetH(ei->mCylHeight);
        ei->mBgAcch.Set(&ac->current.pos, &ac->old.pos, ac, 1, &ei->mBgAcchCir, &ei->mSpeed,
                        nullptr, nullptr);
        ei->mBgAcchCir.SetWall(40.0f, ei->mWallRadius);
        if (static_cast<f32>(ei->mParticleScale) < small)
            ei->mParticleScale = static_cast<f32>(one);
        if (std::fabs(static_cast<f32>(ei->mYOffset)) < small)
            ei->mYOffset = static_cast<f32>(80.0f);
        ei->mScaleY = static_cast<f32>(one);
        ei->mScaleXZ = static_cast<f32>(one);
        ei->mMode = static_cast<u8>(1);
        damageWord(ac->actor_status) = static_cast<u32>(
            (static_cast<u32>(damageWord(ac->actor_status)) | 0x08000000) & ~0x400u);
        cLib_addCalc0(&ei->m028, one, 3.0f);
        return 0;
    case 1:
        if (static_cast<s16>(ei->mFreezeDuration) == 0)
            return 0;
        ei->mFreezeTimer = static_cast<s16>(static_cast<s16>(ei->mFreezeDuration));
        ei->mFreezeDuration = static_cast<s16>(0);
        ei->mMode = static_cast<u8>(2);
        if (static_cast<s8>(ei->m00C) == 0) {
            ei->mSpeed.y = static_cast<f32>(30.0f);
            ei->mAngularVelY =
                static_cast<s16>((s16)gabi::ftoi(damageRndFX(3000.0f)));
        }
        ac->health = static_cast<s8>((s8)(static_cast<s8>(ac->health) - 4));
        if (static_cast<s8>(ac->health) <= 0) {
            ei->mSpeed.y = static_cast<f32>(zero);
            ei->mMoveDelayTimer = static_cast<s16>(40);
        }
        soundIce(0x58FA);
        ice_pos_scale(ice, actor, pos, sc);
        ice_particle(0x274, pos, 0, sc);
        frozen = 1;
        if (static_cast<s8>(ei->m00C) == 1) {
            movement = true;
            break;
        }
        [[fallthrough]];
    case 2:
        frozen = 1;
        if (static_cast<s8>(ei->m00C) == 1) {
            movement = true;
            break;
        }
        damageWord(ac->attention_info.flags) =
            static_cast<u32>(static_cast<u32>(damageWord(ac->attention_info.flags)) | 0x10);
        ac->attention_info.distances[4] = static_cast<u8>(0x12);
        if (static_cast<u32>(damageWord(ac->actor_status)) & 0x2000) {
            damageWord(ac->attention_info.flags) =
                static_cast<u32>(static_cast<u32>(damageWord(ac->attention_info.flags)) & ~0x10u);
            ei->mMode = static_cast<u8>(3);
            if (static_cast<s8>(ei->m00C) == 2)
                ei->m00C = static_cast<u8>(0);
        }
        movement = true;
        break;
    case 3: {
        frozen = 1;
        if (static_cast<u32>(damageWord(ac->actor_status)) & 0x2000)
            break;
        gabi::Local<dBgS_LinChk> line;
        u32 ln = gabi::ea(line.get());
        line_init(ln);
        position.x = static_cast<f32>(static_cast<f32>(playerActor->current.pos.x));
        position.y = static_cast<f32>(static_cast<f32>(playerActor->current.pos.y) + 50.0f);
        position.z = static_cast<f32>(static_cast<f32>(playerActor->current.pos.z));
        cMtx_YrotS(damageView<Mtx34>(matrix), static_cast<s16>(playerActor->shape_angle.y));
        direction.x = static_cast<f32>(zero);
        direction.y = static_cast<f32>(zero);
        direction.z = static_cast<f32>(REG_F(18, 8) + 25.0f);
        MtxPosition(damageView<cXyz>(dir), damageView<cXyz>(off));
        direction.x =
            static_cast<f32>(static_cast<f32>(ac->current.pos.x) + static_cast<f32>(offset.x));
        direction.y = static_cast<f32>(static_cast<f32>(ac->current.pos.y) + 50.0f);
        direction.z =
            static_cast<f32>(static_cast<f32>(ac->current.pos.z) + static_cast<f32>(offset.z));
        damageView<dBgS_LinChk>(ln)->Set(damageView<cXyz>(pos), damageView<cXyz>(dir),
                                         damageView<fopAc_ac_c>(actor));
        u32 p = dComIfGp_ea();
        s32 crossed = damageView<dBgS>(p + 0x12A0)->LineCross(damageView<dBgS_LinChk>(ln));
        f32 speed = static_cast<f32>(ac->speedF);
        // HD: a blocked carry-release ray resets both positions to the player.
        if (crossed) {
            f32 x = static_cast<f32>(playerActor->current.pos.x);
            ac->current.pos.x = static_cast<f32>(x);
            f32 y = static_cast<f32>(playerActor->current.pos.y);
            ac->current.pos.y = static_cast<f32>(y);
            f32 z = static_cast<f32>(playerActor->current.pos.z);
            ac->old.pos.x = static_cast<f32>(x);
            ac->current.pos.z = static_cast<f32>(z);
            ac->old.pos.y = static_cast<f32>(y);
            ac->old.pos.z = static_cast<f32>(z);
        }
        if (speed > zero) {
            ei->mSpeedF = static_cast<f32>(REG0_F(5) + 25.0f);
            ei->mSpeed.y = static_cast<f32>(REG0_F(6) + 20.0f);
        } else {
            ei->mSpeedF = static_cast<f32>(gravity);
            ei->mSpeed.y = static_cast<f32>(-15.0f);
        }
        ei->mAngleY = static_cast<u16>(static_cast<u16>(playerActor->shape_angle.y));
        ei->mMode = static_cast<u8>(2);
        line_destroy(ln);
        break;
    }
    default:
        damageWord(ac->actor_status) =
            static_cast<u32>(static_cast<u32>(damageWord(ac->actor_status)) & ~0x400u);
        cLib_addCalc0(&ei->m028, one, 3.0f);
        return 0;
    }
    if (movement) {
        s16 delay = static_cast<s16>(ei->mMoveDelayTimer);
        if (delay == 0) {
            cMtx_YrotS(damageView<Mtx34>(matrix), static_cast<s16>(ei->mAngleY));
            direction.x = static_cast<f32>(zero);
            direction.y = static_cast<f32>(zero);
            direction.z = static_cast<f32>(static_cast<f32>(ei->mSpeedF));
            MtxPosition(damageView<cXyz>(dir), damageView<cXyz>(off));
            ei->mSpeed.x = static_cast<f32>(static_cast<f32>(offset.x));
            ei->mSpeed.z = static_cast<f32>(static_cast<f32>(offset.z));
            if (static_cast<s8>(ei->m00C) == 2)
                ei->mSpeed.y = static_cast<f32>(zero);
            else {
                PSVECAdd(damageView<cXyz>(actor + 0x314), damageView<cXyz>(ice + 0x18),
                         damageView<cXyz>(actor + 0x314));
                ei->mSpeed.y = static_cast<f32>(static_cast<f32>(ei->mSpeed.y) - gravity);
            }
            ac->shape_angle.y = static_cast<s16>(
                (s16)(static_cast<s16>(ac->shape_angle.y) + static_cast<s16>(ei->mAngularVelY)));
            if (ice_bg_check(ice, static_cast<f32>(ei->mSpeed.y), zero))
                ei->mFreezeTimer = static_cast<s16>(-1);
        } else
            ei->mMoveDelayTimer = static_cast<s16>((s16)(delay - 1));
        if (ice + 0x30)
            PSVECAdd(damageView<cXyz>(actor + 0x314), damageView<cXyz>(ice + 0x30),
                     damageView<cXyz>(actor + 0x314));
        u32 flags = static_cast<u32>(damageWord(ei->mCyl.mObjAt.mSPrm));
        damageWord(ei->mCyl.mObjAt.mSPrm) = static_cast<u32>(
            std::fabs(static_cast<f32>(ei->mSpeedF)) > gravity ? flags | 1 : flags & ~1u);
        ei->mCyl.SetC(&ac->current.pos);
        u32 p = dComIfGp_ea();
        damageSetCollision(damageView<cCcS>(p + 0x26A4), damageView<dCcD_GObjInf>(ice + 0x6C));
        if (static_cast<s16>(ei->mFreezeTimer) >= 23 && ei->mCyl.ChkTgHit()) {
            u32 hit = gabi::ea(ei->mCyl.GetTgHitObj()), type = load<u32>(hit + 0x10);
            if (type & 0x100000) {
                ei->mLightShrinkTimer = static_cast<u8>(1);
                ei->mFreezeTimer = static_cast<s16>(1);
            } else if (type & 0x200)
                ei->mFreezeTimer = static_cast<s16>(1);
            else if (type & 0x50020)
                ei->mFreezeTimer = static_cast<s16>(type & 0x10000 ? -2 : -1);
            else
                gabi::call(0x02518CC8, actor, hit, 0x42);
        }
    }
    f32 shiverX = zero, shiverZ = zero;
    s16 timer = static_cast<s16>(ei->mFreezeTimer);
    bool shattered = false, expire = false;
    if (timer > 0) {
        timer--;
        ei->mFreezeTimer = static_cast<s16>(timer);
        expire = timer == 0;
    } else if (timer < 0) {
        ice_pos_scale(ice, actor, pos, sc);
        ice_particle(0x273, pos, 0, sc);
        ice_particle(0x274, pos, 0, sc);
        if (static_cast<s16>(ei->mFreezeTimer) == -2) {
            ice_particle(0x10, pos, 0, 0);
            u32 p = dComIfGp_ea(), pl = static_cast<u32>(damageWord(
                                       damageView<dComIfG_inf_c>(p)->play.mPlayerInfo[0].mpPlayer));
            s16 yaw = fopAcM_searchActorAngleY(ac, damageView<fopAc_ac_c>(pl));
            csXyz angle;
            csXyz_ct(&angle, 0, yaw, 0);
            f32 size = 2.0f;
            scale.x = static_cast<f32>(size);
            scale.y = static_cast<f32>(size);
            scale.z = static_cast<f32>(size);
            ice_particle(0xD, pos, gabi::ea((&angle)), sc);
            store<u8>(0x101EACB7, 8);
        }
        soundIce(0x5902);
        item();
        ei->mFreezeTimer = static_cast<s16>(0);
        if (static_cast<u8>(ei->mDeathSwitch))
            dComIfGs_onSwitch(static_cast<u8>(ei->mDeathSwitch),
                              static_cast<s8>(ac->current.roomNo));
        timer = static_cast<s16>(ei->mFreezeTimer);
        shattered = true;
        expire = timer == 0;
    } else {
        ice_render(ice, actor, zero, zero, one);
        return frozen;
    }
    if (expire) {
        ei->mMode = static_cast<u8>(1);
        ei->mCyl.SetC(&non_pos);
        u32 p = dComIfGp_ea();
        damageSetCollision(damageView<cCcS>(p + 0x26A4), damageView<dCcD_GObjInf>(ice + 0x6C));
        if (static_cast<u32>(damageWord(ac->actor_status)) & 0x2000)
            fopAcM_cancelCarryNow(ac);
        if (shattered) {
            freeActor();
            ac->health = static_cast<s8>(-128);
        } else
            frozen = 0;
        timer = static_cast<s16>(ei->mFreezeTimer);
    }
    if (timer < 50) {
        shiverX = (f32)((timer & 1) * 2 - 1);
        shiverZ = (f32)(((timer + 1) & 1) * 2 - 1);
    }
    if (timer == 20) {
        ice_pos_scale(ice, actor, pos, sc);
        ice_particle(0x277, pos, 0, sc);
        soundIce(0x5903);
    }
    ice_render(ice, actor, shiverX, shiverZ, one);
    return frozen;
}

/* 02041570 */
static void enemy_fire(u32 fire) {
    auto* ef = damageView<enemyfire>(fire);

    cXyz scale;
    cXyz offset;
    cXyz pos;
    cXyz velocity;
    u32 sc = gabi::ea((&scale)), off = gabi::ea((&offset)), po = gabi::ea((&pos)),
        ve = gabi::ea((&velocity));
    u32 actor = static_cast<u32>(damageWord(ef->mpActor));
    auto* ac = damageView<fopAc_ac_c>(actor);
    f32 zero = 0.0f;
    offset.x = static_cast<f32>(zero);
    offset.y = static_cast<f32>(zero);
    offset.z = static_cast<f32>(zero);
    s8 mode = static_cast<s8>(ef->mMode);
    if (mode == 0) {
        s16 duration = static_cast<s16>(ef->mFireDuration);
        if (duration == 0)
            return;
        ef->mFireTimer = static_cast<s16>(duration);
        ef->mFireDuration = static_cast<s16>(0);
        ef->mMode = static_cast<u8>(1);
        dKy_plight_set(damageView<LIGHT_INFLUENCE>(fire + 0x208));
        f32 startY = 2.0f, randomMax = 60.0f;
        for (u32 i = 0; i < 10; i++) {
            if (static_cast<s8>(ef->mFlameJntIdxs[i]) < 0 ||
                static_cast<u32>(damageWord(ef->mpFlameEmitters[i])) != 0)
                continue;
            u32 size = static_cast<u32>(damageWord(ef->mParticleScale[i]));
            gmem_stf32(sc, size);
            gmem_stf32(sc + 8, size);
            gmem_stf32(sc + 4, size);
            u32 play = dComIfGp_ea();
            u32 emitter = gabi::ea(
                damageView<dPa_control_c>(
                    static_cast<u32>(damageWord(damageView<dComIfG_inf_c>(play)->play.mParticle)))
                    ->set(0, 0x3F1, &ac->current.pos, nullptr, damageView<cXyz>(sc), 255, nullptr,
                          -1, nullptr, nullptr, nullptr));
            damageWord(ef->mpFlameEmitters[i]) = static_cast<u32>(emitter);
            s16 noise = (s16)gabi::ftoi(damageRndF(randomMax));
            s16 timer = (s16)(static_cast<s16>(ef->mFireTimer) - noise);
            if (timer < 10)
                timer = 10;
            ef->mFlameTimers[i] = static_cast<s16>(timer);
            ef->mFlameScaleY = static_cast<f32>(startY);
        }
        ef->mStts.Init(250, 255, ac);
        ef->mSph.Set(damageView<dCcD_SrcSph>(0x1018F5B0));
        damageWord(ef->mSph.mStts) = static_cast<u32>(fire + 0xA0);
        return;
    }
    if (mode != 1)
        return;
    f32 power = 150.0f * static_cast<f32>(ef->mFlameScaleY);
    damageWord(ef->mLight.mPos.x) =
        static_cast<u32>(static_cast<u32>(damageWord(ac->current.pos.x)));
    damageWord(ef->mLight.mPos.y) =
        static_cast<u32>(static_cast<u32>(damageWord(ac->current.pos.y)));
    s16 lightPower = (s16)gabi::ftoi(power);
    ef->mLight.mColorR = static_cast<u16>(600);
    damageWord(ef->mLight.mPos.z) =
        static_cast<u32>(static_cast<u32>(damageWord(ac->current.pos.z)));
    ef->mLight.mColorG = static_cast<u16>(400);
    ef->mLight.mColorB = static_cast<u16>(120);
    ef->mLight.mPower = static_cast<f32>((f32)lightPower);
    ef->mLight.mFluctuation = static_cast<f32>(250.0f);
    cXyz_mi(damageView<cXyz>(actor + 0x314), damageView<cXyz>(ve), damageView<cXyz>(fire + 0x80));
    damageWord(ef->mPrevPos.x) = static_cast<u32>(static_cast<u32>(damageWord(ac->current.pos.x)));
    damageWord(ef->mPrevPos.y) = static_cast<u32>(static_cast<u32>(damageWord(ac->current.pos.y)));
    damageWord(ef->mPrevPos.z) = static_cast<u32>(static_cast<u32>(damageWord(ac->current.pos.z)));
    f32 factor = REG0_F(4) + -0.02f;
    f32 vx = static_cast<f32>(velocity.x), vy = static_cast<f32>(velocity.y),
        vz = static_cast<f32>(velocity.z);
    f32 one = 1.0f, minus = -1.0f;
    f32 dx = vx * factor, dz = vz * factor;
    if (dx > one)
        dx = one;
    else if (dx < minus)
        dx = minus;
    if (dz > one)
        dz = one;
    else if (dz < minus)
        dz = minus;
    f32 ratio = 0.5f, step = 0.05f;
    cLib_addCalc2(&ef->mDirection.x, dx, ratio, step);
    cLib_addCalc2(&ef->mDirection.z, dz, ratio, step);
    f32 sum = gabi::fmadds(vz, vz, gabi::fmadds(vx, vx, vy * vy));
    ef->mDirection.y = static_cast<f32>(REG0_F(11) + 0.2f);
    f64 length = damageSqrt(sum);
    f32 mul = REG0_F(12) + 0.03f, limit = REG0_F(13) + 1.5f;
    f32 target = (f32)(length * (f64)mul + (f64)one);
    if (target > limit)
        target = limit;
    cLib_addCalc2(&ef->mFlameScaleY, target, ratio, step);
    u8 active = 0;
    for (u32 i = 0; i < 10; i++) {
        if (static_cast<s8>(ef->mFlameJntIdxs[i]) < 0)
            continue;
        auto& slot = ef->mpFlameEmitters[i];
        u32 emitter = static_cast<u32>(slot);
        if (emitter == 0)
            continue;
        s16 timer = static_cast<s16>(ef->mFlameTimers[i]);
        if (timer == 0) {
            u32 flags = static_cast<u32>(damageView<DamageEmitter>(emitter)->mFlags);
            damageWord(damageView<DamageEmitter>(emitter)->mMaxFrame) = 0xFFFFFFFF;
            damageView<DamageEmitter>(emitter)->mFlags = flags | 1;
            slot = 0;
            continue;
        }
        ef->mFlameTimers[i] = static_cast<s16>((s16)(timer - 1));
        u32 model = damageWord(ef->mpMcaMorf);
        auto* morf = damageView<mDoExt_McaMorf>(model);
        J3DMtxBuffer_hd* joints = morf->mpModel->mpMtxBuffer;
        s8 joint = ef->mFlameJntIdxs[i];
        u32 matrices = joints->mpAnmMtx;
        joints->mFlags = static_cast<u16>(joints->mFlags) | 0x10;
        PSMTXCopy(damageView<Mtx34>(matrices + (u32)((s32)joint * 48)),
                  damageView<Mtx34>(gabi::ea(calc_mtx())));
        MtxPosition(damageView<cXyz>(off), damageView<cXyz>(po));
        emitter = static_cast<u32>(slot);
        f32 px = static_cast<f32>(pos.x), py = static_cast<f32>(pos.y),
            pz = static_cast<f32>(pos.z);
        if (static_cast<u8>(damageView<DamageEmitter>(emitter)->mGroupID) >= 7)
            py = -py;
        damageView<DamageEmitter>(emitter)->mGlobalTranslation.x = px;
        damageView<DamageEmitter>(emitter)->mGlobalTranslation.y = py;
        damageView<DamageEmitter>(emitter)->mGlobalTranslation.z = pz;
        emitter = static_cast<u32>(slot);
        damageWord(damageView<DamageEmitter>(emitter)->mEmitterDir[0]) =
            static_cast<u32>(damageWord(ef->mDirection.x));
        damageWord(damageView<DamageEmitter>(emitter)->mEmitterDir[1]) =
            static_cast<u32>(damageWord(ef->mDirection.y));
        damageWord(damageView<DamageEmitter>(emitter)->mEmitterDir[2]) =
            static_cast<u32>(damageWord(ef->mDirection.z));
        f32 particleScale = static_cast<f32>(ef->mParticleScale[i]),
            sy = static_cast<f32>(ef->mFlameScaleY) * particleScale;
        emitter = static_cast<u32>(slot);
        damageView<DamageEmitter>(emitter)->mGlobalParticleScale.x = particleScale;
        damageView<DamageEmitter>(emitter)->mGlobalParticleScale.y = sy;
        damageView<DamageEmitter>(emitter)->mGlobalParticleScale.z = particleScale;
        if (static_cast<u8>(ef->mHitboxFlameIdx) == active) {
            ef->mSph.SetC(damageView<cXyz>(po));
            u32 play = dComIfGp_ea();
            damageSetCollision(damageView<cCcS>(play + 0x26A4),
                               damageView<dCcD_GObjInf>(fire + 0xDC));
        }
        active++;
    }
    u8 next = (u8)(static_cast<u8>(ef->mHitboxFlameIdx) + 1);
    ef->mHitboxFlameIdx = static_cast<u8>(next < active ? next : 0);
    u32 soundPos = actor + 0x37C;
    if (soundPos) {
        s32 reverb = dComIfGp_getReverb(static_cast<s8>(ac->current.roomNo));
        mDoAud_seStart(0x6103, damageView<cXyz>(soundPos), 0, reverb);
    }
    s16 timer = static_cast<s16>(ef->mFireTimer);
    if (timer == 0) {
        ef->mMode = static_cast<u8>(0);
        dKy_plight_cut(damageView<LIGHT_INFLUENCE>(fire + 0x208));
        ef->mSph.SetC(&non_pos);
        u32 play = dComIfGp_ea();
        damageSetCollision(damageView<cCcS>(play + 0x26A4), damageView<dCcD_GObjInf>(fire + 0xDC));
        timer = static_cast<s16>(ef->mFireTimer);
    } else if (soundPos) {
        s32 reverb = dComIfGp_getReverb(static_cast<s8>(ac->current.roomNo));
        mDoAud_seStart(0x5101, damageView<cXyz>(soundPos), 0, reverb);
        timer = static_cast<s16>(ef->mFireTimer);
    }
    if (timer != 0)
        ef->mFireTimer = static_cast<s16>((s16)(timer - 1));
}

/* 02041C30 */
static void enemy_fire_remove(u32 fire) {
    auto* ef = damageView<enemyfire>(fire);

    ef->mMode = static_cast<u8>(0);
    dKy_plight_cut(damageView<LIGHT_INFLUENCE>(fire + 0x208));
    for (u32 i = 0; i < 10; i++) {
        u32 emitter = static_cast<u32>(damageWord(ef->mpFlameEmitters[i]));
        if (emitter) {
            u32 flags = static_cast<u32>(damageView<DamageEmitter>(emitter)->mFlags);
            damageWord(damageView<DamageEmitter>(emitter)->mMaxFrame) = 0xFFFFFFFF;
            damageView<DamageEmitter>(emitter)->mFlags = flags | 1;
            damageWord(ef->mpFlameEmitters[i]) = static_cast<u32>(0);
        }
    }
}

/* 02041CAC */
static void enemy_piyo_set(u32 actor) {

    u32 play = dComIfGp_ea();
    u32 manager = static_cast<u32>(damageWord(damageView<dComIfG_inf_c>(play)->play.mParticle));
    damageView<dPa_control_c>(manager)->set(0, 0x27A, damageView<cXyz>(actor + 0x390), nullptr,
                                            nullptr, 255, nullptr, -1, nullptr, nullptr, nullptr);
}

/* 02043B98 */
static void enemy_seStart(u32 actor, u32 id, u32 flags) {
    auto* ac = damageView<fopAc_ac_c>(actor);

    if (actor == 0 || actor + 0x37C == 0)
        return;
    s32 reverb = dComIfGp_getReverb(static_cast<s8>(ac->current.roomNo));
    mDoAud_seStart(id, damageView<cXyz>(actor + 0x37C), flags, reverb);
}

/* 02043C4C */
static s32 wall_angle_get(u32 actor, s32 angle) {

    gabi::Local<dBgS_LinChk> line;
    cXyz scratch;
    cXyz start;
    cXyz diff;
    // HD: keep the two-vector guest frame contiguous for indexed line endpoints.
    gabi::Local<cXyz[2]> points;
    u32 ln = gabi::ea(line.get()), sc = gabi::ea((&scratch)), st = gabi::ea((&start)),
        di = gabi::ea((&diff)), pt = gabi::ea(points.get());
    line_init(ln);
    damageYrotS(calc_mtx(), angle);
    f32 zero = 0.0f;
    scratch.x = static_cast<f32>(zero);
    scratch.y = static_cast<f32>(zero);
    scratch.z = static_cast<f32>(100.0f);
    MtxPosition(damageView<cXyz>(sc), damageView<cXyz>(st));
    PSVECAdd(damageView<cXyz>(st), damageView<cXyz>(actor + 0x314), damageView<cXyz>(st));
    scratch.y = static_cast<f32>(zero);
    scratch.z = static_cast<f32>(-300.0f);
    scratch.x = static_cast<f32>(10.0f);
    for (u32 i = 0; i < 2; i++) {
        u32 p = pt + 12 * i;
        MtxPosition(damageView<cXyz>(sc), damageView<cXyz>(p));
        scratch.x = static_cast<f32>(-static_cast<f32>(scratch.x));
        PSVECAdd(damageView<cXyz>(p), damageView<cXyz>(st), damageView<cXyz>(p));
        damageView<dBgS_LinChk>(ln)->Set(damageView<cXyz>(st), damageView<cXyz>(p),
                                         damageView<fopAc_ac_c>(actor));
        u32 play = dComIfGp_ea();
        if (damageView<dBgS>(play + 0x12A0)->LineCross(damageView<dBgS_LinChk>(ln)) == 0) {
            line_destroy(ln);
            return 1;
        }
        cXyz& cross = line.get()->mLin.mEnd;
        u32 x = damageWord(cross.x), y = damageWord(cross.y), z = damageWord(cross.z);
        auto* endpoint = damageView<cXyz>(p);
        damageWord(endpoint->x) = x;
        damageWord(endpoint->y) = y;
        damageWord(endpoint->z) = z;
    }
    cXyz_mi(damageView<cXyz>(pt + 12), damageView<cXyz>(di), damageView<cXyz>(pt));
    f32 x = static_cast<f32>(diff.x), y = static_cast<f32>(diff.y), z = static_cast<f32>(diff.z);
    scratch.x = static_cast<f32>(x);
    scratch.y = static_cast<f32>(y);
    scratch.z = static_cast<f32>(z);
    s32 ret = (s16)(damageAtan2s(x, z) + 0x4000);
    line_destroy(ln);
    return ret;
}

/* inlined into caller */
static void dr_body_bg_check(damagereaction* dr, u32 scratch) {

    f32 lift = dr->m488 == 1 ? 12.5f : 0.0f;
    if (dr->m71E == 0) {
        dr->mpEnemy->current.pos.y -= dr->m44C.y + lift;
        dr->mpEnemy->old.pos.y -= dr->m44C.y + lift;
        dr->mpEnemy->speed.y *= 0.25f;
        dr->mAcch.CrrPos(dComIfG_Bgsp());
        dr->mpEnemy->speed.y *= 4.0f;
        dr->mpEnemy->current.pos.y += dr->m44C.y + lift;
        dr->mpEnemy->old.pos.y += dr->m44C.y + lift;
        dr->m6E8.copy(dr->mpEnemy->old.pos);
        dr->m6E8.y = dr->mSpawnY;
        dr->mSpawnY = dr->mAcch.GetGroundH();
    } else
        dr->m71E--;
    if (dr->mAction != 21 && dr->mAction != 22) {
        DamageGround* chk = gabi::at<DamageGround>(scratch);
        chk->Init(true);
        f32 x = dr->mpEnemy->current.pos.x, y = dr->mpEnemy->current.pos.y,
            z = dr->mpEnemy->current.pos.z;
        cXyz point;
        (&point)->set(x, y + 1000.0f, z);
        chk->SetPos((&point));
        f32 height = cBgS_GroundCross(dComIfG_Bgsp(), chk);
        if (height != -1000000000.0f && !(dr->mpEnemy->current.pos.y > height)) {
            dr->mpEnemy->current.pos.y = height + REG0_F(13);
            dr->mMode = 0;
            dr->m47C = 0;
            u32 grp = gabi::call<u32>(0x024EEE30, dComIfG_Bgsp(), gabi::ea(chk) + 0x14, 0x100);
            (&point)->set(x, height, z);
            if (grp) {
                dr->mAction = 22;
                fopKyM_createWpillar((&point), REG0_F(9) + 1.0f, REG0_F(10) + 1.0f, 0);
            } else {
                dr->mAction = 21;
                gabi::call(0x025DAF3C, (&point), REG0_F(14) + 0.5f);
            }
        }
        chk->End();
    }
    if (dr->mEnemyType == 2 &&
        daSea_ChkArea((f32)dr->mpEnemy->current.pos.x, (f32)dr->mpEnemy->current.pos.z)) {
        f32 sea = daSea_calcWave((f32)dr->mpEnemy->current.pos.x, (f32)dr->mpEnemy->current.pos.z);
        sea -= 40.0f;
        sea = REG0_F(13) + sea;
        sea = (f32)dr->m44C.y + sea;
        if (!(dr->mpEnemy->current.pos.y > sea)) {
            dr->mpEnemy->current.pos.y = sea;
            cXyz pos;
            (&pos)->copy(dr->mpEnemy->current.pos);
            (&pos)->y = sea;
            fopKyM_createWpillar((&pos), REG0_F(9) + 1.0f, REG0_F(10) + 1.0f, 0);
            fopAcM_seStart(dr->mpEnemy.get(), 0x6918, 0);
            fopAcM_delete(dr->mpEnemy.get());
            u32 actor = gabi::ea(dr->mpEnemy.get());
            auto* ac = damageView<fopAc_ac_c>(actor);
            u8 sw = static_cast<u8>(damageView<DamageBokoblin>(actor)->m02B8);
            if (sw)
                dComIfGs_onSwitch(sw, static_cast<s8>(ac->current.roomNo));
        }
    }
}

/* inlined into caller */
static s32 dr_joint_bg_check(damagereaction* dr, u32 scratch, u32 auxiliary) {
    int result = 0;

    DamageGround& gndChk = *gabi::at<DamageGround>(auxiliary);
    gndChk.Init(false);

    if (dr->m420 != 0) {
        dr->m420--;
        if (dr->m420 == 0 && dr->m70D == 1) {
            result = 2;
        }

        for (int i = 0; i < 0xE; i++) {
            dr->m1F0[i].set(dr->m100[i].x, dr->m100[i].y, dr->m100[i].z);
            dr->m1F0[i].y += gabi::fmadds(REG0_F(7), 10.0f, 200.0f);
            gndChk.SetPos(&dr->m1F0[i]);
            dr->m1F0[i].y = cBgS_GroundCross(dComIfG_Bgsp(), &gndChk);
            if (dr->m1F0[i].y == -1000000000.0f) {
                dr->m1F0[i].y = 1000000000.0f;
            }

            if (i == 0 || i == 1 || i == 2 || i == 3) {
                if (dr->m1F0[i].y == 1000000000.0f) {
                    dr->m3D0[i] = 2;
                } else if (dr->m100[i].y - dr->m1F0[i].y > gabi::fmadds(REG0_F(8), 10.0f, 200.0f)) {
                    dr->m3D0[i] = 0;
                } else {
                    dr->m3D0[i] = 1;
                }
            }
        }

        DamageLine& linChk = *gabi::at<DamageLine>(scratch);
        linChk.Init();

        cXyz t1;
        cXyz t2;
        cXyz& temp = *(&t1);
        cXyz& temp2 = *(&t2);
        temp.x = 0.0f;
        temp.y = 0.0f;
        f32 diffX = dr->m100[0xE].x - dr->m100[0xA].x;
        f32 diffY = dr->m100[0xE].y - dr->m100[0xA].y;
        f32 diffZ = dr->m100[0xE].z - dr->m100[0xA].z;
        temp.z = std_sqrtf(gabi::fmadds(diffZ, diffZ, gabi::fmadds(diffX, diffX, diffY * diffY)));
        s16 angleY = cM_atan2s(diffX, diffZ);
        s16 angleX = 0;
        for (; angleX >= -0x4000; angleX -= 0x400) {
            mDoMtx_YrotS(calc_mtx(), angleY);
            mDoMtx_XrotM(calc_mtx(), angleX);
            MtxPosition(&temp, &temp2);
            temp2.x = temp2.x + dr->m100[0xA].x;
            temp2.y = temp2.y + dr->m100[0xA].y;
            temp2.z = temp2.z + dr->m100[0xA].z;
            linChk.Set(&dr->m100[0xA], &temp2, dr->mpEnemy);
            if (!cBgS_LineCross(dComIfG_Bgsp(), &linChk)) {
                break;
            }
        }

        if (angleX != 0) {
            dr->m440 = angleX + 0x400;
            dr->m408 = 2;
        } else {
            f32 diff2X = dr->m1F0[6].x - dr->m1F0[0xA].x;
            f32 diff2Y = dr->m1F0[6].y - dr->m1F0[0xA].y;
            f32 diff2Z = dr->m1F0[6].z - dr->m1F0[0xA].z;
            dr->m440 = -cM_atan2s(diff2Y, std_sqrtf(gabi::fmadds(diff2X, diff2X, diff2Z * diff2Z)));
            dr->m408 = 0;

            if (dr->m70D == 1) {
                dr->m4B4 = 0;
                dr->m4B0 = 0;
            }
        }

        if (dr->m440 > 7000) {
            dr->m440 = 7000;
        } else if (dr->m440 < -10000) {
            dr->m440 = -10000;
        }

        diffX = dr->m100[0xF].x - dr->m100[0xB].x;
        diffY = dr->m100[0xF].y - dr->m100[0xB].y;
        diffZ = dr->m100[0xF].z - dr->m100[0xB].z;
        temp.z = std_sqrtf(gabi::fmadds(diffZ, diffZ, gabi::fmadds(diffX, diffX, diffY * diffY)));
        angleY = cM_atan2s(diffX, diffZ);
        angleX = 0;
        for (; angleX >= -0x4000; angleX -= 0x400) {
            mDoMtx_YrotS(calc_mtx(), angleY);
            mDoMtx_XrotM(calc_mtx(), angleX);
            MtxPosition(&temp, &temp2);
            temp2.x = temp2.x + dr->m100[0xB].x;
            temp2.y = temp2.y + dr->m100[0xB].y;
            temp2.z = temp2.z + dr->m100[0xB].z;
            linChk.Set(&dr->m100[0xB], &temp2, dr->mpEnemy);
            if (!cBgS_LineCross(dComIfG_Bgsp(), &linChk)) {
                break;
            }
        }

        if (angleX != 0) {
            dr->m442 = angleX + 0x400;
            dr->m40C = 2;
        } else {
            f32 diff2X = dr->m1F0[7].x - dr->m1F0[0xB].x;
            f32 diff2Y = dr->m1F0[7].y - dr->m1F0[0xB].y;
            f32 diff2Z = dr->m1F0[7].z - dr->m1F0[0xB].z;
            dr->m442 = -cM_atan2s(diff2Y, std_sqrtf(gabi::fmadds(diff2X, diff2X, diff2Z * diff2Z)));
            dr->m40C = 0;

            if (dr->m70D == 1) {
                dr->m4B6 = 0;
                dr->m4B2 = 0;
            }
        }

        diffX = dr->m100[0x10].x - dr->m100[0x8].x;
        diffY = dr->m100[0x10].y - dr->m100[0x8].y;
        diffZ = dr->m100[0x10].z - dr->m100[0x8].z;
        temp.z = std_sqrtf(gabi::fmadds(diffZ, diffZ, gabi::fmadds(diffX, diffX, diffY * diffY)));
        angleY = cM_atan2s(diffX, diffZ);
        angleX = 0;
        for (; angleX >= -0x4000; angleX -= 0x400) {
            mDoMtx_YrotS(calc_mtx(), angleY);
            mDoMtx_XrotM(calc_mtx(), angleX);
            MtxPosition(&temp, &temp2);
            temp2.x = temp2.x + dr->m100[0x8].x;
            temp2.y = temp2.y + dr->m100[0x8].y;
            temp2.z = temp2.z + dr->m100[0x8].z;
            linChk.Set(&dr->m100[0x8], &temp2, dr->mpEnemy);
            if (!cBgS_LineCross(dComIfG_Bgsp(), &linChk)) {
                break;
            }
        }

        if (angleX != 0) {
            dr->m444 = angleX + 0x400;
            dr->m410 = 2;
        } else {
            f32 diff2X = dr->m1F0[4].x - dr->m1F0[0x8].x;
            f32 diff2Y = dr->m1F0[4].y - dr->m1F0[0x8].y;
            f32 diff2Z = dr->m1F0[4].z - dr->m1F0[0x8].z;
            dr->m444 = -cM_atan2s(diff2Y, std_sqrtf(gabi::fmadds(diff2X, diff2X, diff2Z * diff2Z)));
            dr->m410 = 0;
        }

        diffX = dr->m100[0x11].x - dr->m100[0x9].x;
        diffY = dr->m100[0x11].y - dr->m100[0x9].y;
        diffZ = dr->m100[0x11].z - dr->m100[0x9].z;
        temp.z = std_sqrtf(gabi::fmadds(diffZ, diffZ, gabi::fmadds(diffX, diffX, diffY * diffY)));
        angleY = cM_atan2s(diffX, diffZ);
        angleX = 0;
        for (; angleX >= -0x4000; angleX -= 0x400) {
            mDoMtx_YrotS(calc_mtx(), angleY);
            mDoMtx_XrotM(calc_mtx(), angleX);
            MtxPosition(&temp, &temp2);
            temp2.x = temp2.x + dr->m100[0x9].x;
            temp2.y = temp2.y + dr->m100[0x9].y;
            temp2.z = temp2.z + dr->m100[0x9].z;
            linChk.Set(&dr->m100[0x9], &temp2, dr->mpEnemy);
            if (!cBgS_LineCross(dComIfG_Bgsp(), &linChk)) {
                break;
            }
        }

        if (angleX != 0) {
            dr->m446 = angleX + 0x400;
            dr->m414 = 2;
        } else {
            f32 diff2X = dr->m1F0[5].x - dr->m1F0[0x9].x;
            f32 diff2Y = dr->m1F0[5].y - dr->m1F0[0x9].y;
            f32 diff2Z = dr->m1F0[5].z - dr->m1F0[0x9].z;
            dr->m446 = -cM_atan2s(diff2Y, std_sqrtf(gabi::fmadds(diff2X, diff2X, diff2Z * diff2Z)));
            dr->m414 = 0;
        }

        diffX = dr->m100[0x12].x - dr->m100[0xC].x;
        diffY = dr->m100[0x12].y - dr->m100[0xC].y;
        diffZ = dr->m100[0x12].z - dr->m100[0xC].z;
        temp.z = std_sqrtf(gabi::fmadds(diffZ, diffZ, gabi::fmadds(diffX, diffX, diffY * diffY)));
        angleY = cM_atan2s(diffX, diffZ);

        if (REG0_S(4) == 0) {
            angleX = 0;
            for (; angleX >= -0x4000; angleX -= 0x400) {
                mDoMtx_YrotS(calc_mtx(), angleY);
                mDoMtx_XrotM(calc_mtx(), angleX);
                MtxPosition(&temp, &temp2);
                temp2.x = temp2.x + dr->m100[0xC].x;
                temp2.y = temp2.y + dr->m100[0xC].y;
                temp2.z = temp2.z + dr->m100[0xC].z;
                linChk.Set(&dr->m100[0xC], &temp2, dr->mpEnemy);
                if (!cBgS_LineCross(dComIfG_Bgsp(), &linChk)) {
                    break;
                }
            }

            if (angleX != 0) {
                dr->m448 = angleX + 0x400;
                dr->m418 = 2;
            } else {
                f32 diff2X = dr->m1F0[0xD].x - dr->m1F0[0xC].x;
                f32 diff2Y = dr->m1F0[0xD].y - dr->m1F0[0xC].y;
                f32 diff2Z = dr->m1F0[0xD].z - dr->m1F0[0xC].z;
                dr->m448 =
                    -cM_atan2s(diff2Y, std_sqrtf(gabi::fmadds(diff2X, diff2X, diff2Z * diff2Z)));
                dr->m418 = 0;
            }

            if (dr->m448 > 13000) {
                dr->m448 = 13000;
            } else if (dr->m448 < -13000) {
                dr->m448 = -13000;
            }
        }
        linChk.End();
    }
    gndChk.End();
    return result;
}

/* 02041D10 */
static s32 kado_check(u32 dr) {
    auto* reaction = damageView<damagereaction>(dr);

    gabi::Local<dBgS_LinChk> line;
    cXyz start;
    cXyz end;
    cXyz delta;
    u32 ln = gabi::ea(line.get()), st = gabi::ea((&start)), en = gabi::ea((&end)),
        de = gabi::ea((&delta));
    line_init(ln);
    cMtx_YrotS(damageView<Mtx34>(gabi::ea(calc_mtx())), static_cast<s16>(reaction->m482));
    cMtx_XrotM(damageView<Mtx34>(gabi::ea(calc_mtx())), static_cast<s16>(reaction->m480));
    f32 c31 = 150.0f, c30 = -70.0f, c29 = 1.0f, c28 = 50.0f, c27 = 100.0f;
    u32 hit = 0;
    for (u32 i = 0; i < 2; i++) {
        u32 actor = static_cast<u32>(damageWord(reaction->mpEnemy));
        auto* ac = damageView<fopAc_ac_c>(actor);
        f32 off = REG_F(14, 0) + c27;
        start.x = static_cast<f32>(static_cast<f32>(ac->current.pos.x));
        start.y = static_cast<f32>(static_cast<f32>(ac->current.pos.y) + off);
        start.z = static_cast<f32>(static_cast<f32>(ac->current.pos.z));
        delta.x = static_cast<f32>(REG_F(14, 1));
        delta.y = static_cast<f32>(REG_F(14, 2));
        delta.z = static_cast<f32>(REG_F(14, 3) + c30);
        MtxPosition(damageView<cXyz>(de), damageView<cXyz>(en));
        PSVECAdd(damageView<cXyz>(st), damageView<cXyz>(en), damageView<cXyz>(st));
        f32 x = kado_check_x[i] * (REG_F(14, 6) + c29);
        delta.x = static_cast<f32>(x);
        delta.y = static_cast<f32>(REG_F(14, 4) + c28);
        delta.z = static_cast<f32>(REG_F(14, 5) + c31);
        MtxPosition(damageView<cXyz>(de), damageView<cXyz>(en));
        PSVECAdd(damageView<cXyz>(en), damageView<cXyz>(st), damageView<cXyz>(en));
        damageView<dBgS_LinChk>(ln)->Set(
            damageView<cXyz>(st), damageView<cXyz>(en),
            damageView<fopAc_ac_c>(static_cast<u32>(damageWord(reaction->mpEnemy))));
        u32 play = dComIfGp_ea();
        if (damageView<dBgS>(play + 0x12A0)->LineCross(damageView<dBgS_LinChk>(ln)) != 0)
            hit |= static_cast<u32>(kado_bit[i]);
    }
    if (hit == 3)
        hit = 0;
    line_destroy(ln);
    return (s32)hit;
}

/* inlined into caller */
static s16 hang_ang_get(damagereaction* dr, u32 scratch, u32 auxiliary) {
    DamageLine* chk = gabi::at<DamageLine>(scratch);
    chk->Init();
    cXyz t;
    cXyz tmp;
    cXyz* v = (&t);
    cXyz* center = (&tmp);
    cXyz_mi(damageView<cXyz>(&dr->m7A0), damageView<cXyz>(v),
            damageView<cXyz>(&dr->mpEnemy->current.pos));
    mDoMtx_YrotS(calc_mtx(), cM_atan2s(v->x, v->z));
    v->set(0, 0, -100);
    MtxPosition(v, center);
    PSVECAdd(damageView<cXyz>(center), damageView<cXyz>(&dr->mpEnemy->current.pos),
             damageView<cXyz>(center));
    center->y = dr->m7A0.y - 5.0f;
    v->set(10, 0, 250);
    cXyz* dst[2] = {gabi::at<cXyz>(auxiliary), gabi::at<cXyz>(auxiliary + 12)};
    for (u32 i = 0; i < 2; i++) {
        MtxPosition(v, dst[i]);
        v->x = -(f32)v->x;
        PSVECAdd(damageView<cXyz>(dst[i]), damageView<cXyz>(center), damageView<cXyz>(dst[i]));
        chk->Set(center, dst[i], dr->mpEnemy);
        if (cBgS_LineCross(dComIfG_Bgsp(), chk)) {
            dst[i]->set(chk->GetCross().x, chk->GetCross().y, chk->GetCross().z);
        } else {
            chk->End();
            return 0xDCF;
        }
    }
    cXyz_mi(damageView<cXyz>(dst[1]), damageView<cXyz>(v), damageView<cXyz>(dst[0]));
    s16 ret = cM_atan2s(v->x, v->z) + 0x4000;
    chk->End();
    return ret;
}

static void damage_sound_inline(fopAc_ac_c* actor, u32 id) {
    u32 a = gabi::ea(actor);
    if (a + 0x37C != 0) {
        s8 room = actor->current.roomNo;
        s8 reverb = static_cast<s8>(dComIfGp_getReverb(room));
        mDoAud_seStart(id, damageView<cXyz>(a + 0x37C), 0, reverb);
    }
}

/* 02043F34 */
static s32 dr_damage_set(u32 address) {
    damagereaction* dr = gabi::at<damagereaction>(address);
    dComIfGp_ea();
    gabi::Local<DamageLine> checkScratch;
    gabi::Local<DamageGround> groundScratch;
    u32 auxiliary = gabi::ea(groundScratch.get());
    u32 scratch = gabi::ea(checkScratch.get());
    int react = 0;
    bool temp = false;
    cXyz t2;
    cXyz t4;
    cXyz t3;
    cXyz& temp2 = *(&t2);
    cXyz& temp4 = *(&t4);
    cXyz& temp3 = *(&t3);

    if (dr->mEnemyType == damagereaction::TYPE_BOKOBLIN && dr->m488 == 0) {
        // HD: this status bit suppresses the Bokoblin fall-distance reaction.
        if (dr->mAction < 0x13 && !(static_cast<u32>(dr->mpEnemy->actor_status) & 0x100000) &&
            dr->mpEnemy->current.pos.y - dr->mSpawnY > dr->mMaxFallDistance) {
            if (dr->m7B2 != 0 && dr->mMode > -100) {
                dr->mAction = 0x13;
                dr->mMode = 0;
                dr->m71E = 2;
            } else {
                if (dr->m6E8.y - dr->mSpawnY > 300.0f) {
                    dr->m7A0.copy(dr->m6E8);
                    dr->m7AC.y = hang_ang_get(dr, scratch, auxiliary);
                    if (dr->m7AC.y == 0xDCF) {
                        temp = true;
                        dr->mpEnemy->health = 0;
                    } else {
                        dr->mAction = 0x13;
                        dr->mMode = 0;
                        dr->m71E = 2;
                        dr->m7B8 = 0xFFFFFFFF;
                    }
                } else {
                    temp = true;
                    dr->mpEnemy->health = 0;
                }
            }
        }
    } else if (dr->mpEnemy->current.pos.y - dr->mSpawnY > dr->mMaxFallDistance) {
        temp = true;
        if (!(dr->mpEnemy->speed.y > -100.0f))
            dr->mpEnemy->health = 0;
    }

    switch (dr->m488) {
    case 0:
        if (temp || (dr->m424 != 0 && dr->m428 > 25.0f)) {
            dr->mMode = -100;
            dr->m488 = 1;
            dr->m713 = 0;
            dr->m44C.y = -125.0f;
            dr->m486 = 0x4000;
            temp2.copy(dr->m42C);

            if (temp) {
                dr->m482 = dr->mpEnemy->shape_angle.y;
                dr->m478 = 0.0f;
                dr->mpEnemy->speed.y = 0.0f;
                dr->m71E = 10;
            } else if (dr->m424 & 0x40) {
                if (dr->m712) {
                    dr->m478 = -7.0f;
                    dr->mpEnemy->speed.y = 72.0f;
                } else {
                    dr->m478 = -20.0f;
                    dr->mpEnemy->speed.y = 96.0f;
                }

                dr->m482 = cM_atan2s(temp2.x, temp2.z) + 0x8000;
            } else {
                dr->m482 = cM_atan2s(temp2.x, temp2.z);
                dr->m478 = dr->m428;

                if (((dr->mEnemyType == damagereaction::TYPE_BOKOBLIN) &&
                     (fopAcM_GetParam(dr->mpEnemy) & 0xF) == 0xA) ||
                    REG0_S(5) != 0) {
                    dr->mpEnemy->speed.y = dr->m478 * 0.8f * (REG0_F(7) + 0.8f);
                } else {
                    dr->mpEnemy->speed.y = dr->m478 * 0.9f * 1.2f;
                }
            }

            temp2.x = 0.0f;
            temp2.y = 0.0f;
            temp2.z = dr->m478;
            mDoMtx_YrotS(calc_mtx(), dr->m482);
            MtxPosition(&temp2, &temp4);
            damageWord(dr->mpEnemy->speed.x) = static_cast<u32>(damageWord(temp4.x));
            damageWord(dr->mpEnemy->speed.z) = static_cast<u32>(damageWord(temp4.z));
            dr->m47C = 1;
            dr->m4AE = cM_rndF(20000.0f);
            dr->m4AC = cM_rndF(20000.0f);
            dr->m4AA = cM_rndF(-20000.0f);
            dr->m4A8 = cM_rndF(-20000.0f);
            dr->m70C = 0;
            dr->m474 = 2000.0f;
            dr->m70E = 0xFF;

            if (dr->mEnemyType == damagereaction::TYPE_DARKNUT) {
                dr->m4BC = -cM_rndF(9000.0f);
                dr->m4BE = cM_rndF(9000.0f);
            } else {
                dr->m4B8 = cM_rndF(6000.0f) + 1000.0f;
                dr->m4BA = cM_rndF(6000.0f) + 1000.0f;
                dr->m4BC = cM_rndF(10000.0f) + 5000.0f;
                dr->m4BE = cM_rndF(10000.0f) + 5000.0f;
            }

            react = temp ? 0x1E : 1;

            dr->m48A = 0;
        } else if (dr->m424 & 0x10) {
            dr->m470 = REG0_F(0xD) + 4000.0f;
            dr->m474 = REG0_F(0xE) + 4000.0f;
            dr->m70E = 7;
            react = 5;
        }

        break;
    case 1: {
        s16 temp5;
        if (dr->m486 > 0) {
            temp5 = 0x350;
        } else {
            temp5 = 0x800;
        }
        if (test_sw != 0) {
            dr->m486 = 0;
        }
        cLib_addCalcAngleS2(&dr->m480, dr->m486, 3, temp5);

        if (dr->m70C != 0 && !(dr->m474 > 0.01f) && dr->m4C8[2] == 0 && test_sw == 0) {
            if (dr->mpEnemy->health <= 0) {
                dr->mAction = 0x14;
                dr->mMode = 0;
                dr->m4C8[2] = 10000;
            } else {
                dr->m4BE = 0;
                dr->m4BC = 0;
                dr->m4BA = 0;
                dr->m4B8 = 0;
                dr->m49C = 0;
                dr->m49A = 0;
                dr->m4B2 = 0;
                dr->m4B6 = 0;
                dr->m4B0 = 0;
                dr->m4B4 = 0;

                if (dr->m70D == 0) {
                    dr->mMode = 10;
                } else {
                    dr->mMode = 0xC;
                }

                dr->mAction = 0xB;
                dr->m48A = 0;
                // HD: recovery clears this mode instead of setting the GameCube value 2.
                dr->m488 = 0;
                dr->mpEnemy->current.angle.y += dr->m484;
                dr->mpEnemy->shape_angle.y += dr->m484;
                dr->m4C0 += dr->m484;
                dr->m7B6 = 1;
                dr->m484 = 0;
                dr->m6E0 = 0;
            }
        }
    }

    break;
    case 2:
        break;
    default:
        break;
    }

    if (dr->m7B6 == 0 && test_sw == 0) {
        if (dr->m4C8[0] == 0) {
            // HD: action 18 cancels the additional forward displacement.
            if (dr->mAction == 18)
                dr->m4D4 = 0;
            mDoMtx_YrotS(calc_mtx(), cM_atan2s(-dr->m42C.x, -dr->m42C.z));
            temp2.x = 0.0f;
            temp2.y = 0.0f;
            temp2.z = dr->m4D4;
            MtxPosition(&temp2, &temp3);
            dr->mpEnemy->current.pos.x +=
                gabi::fmadds((f32)dr->mpEnemy->speed.x, 0.25f, (f32)temp3.x);
            dr->mpEnemy->current.pos.y =
                gabi::fmadds((f32)dr->mpEnemy->speed.y, 0.25f, (f32)dr->mpEnemy->current.pos.y);
            dr->mpEnemy->current.pos.z +=
                gabi::fmadds((f32)dr->mpEnemy->speed.z, 0.25f, (f32)temp3.z);

            // HD: disabled modes skip collision displacement; only X and Z are applied.
            if (dr->mAction != 0xF && dr->mMode > -100) {
                DamageStts* pStts = &dr->mStts;
                if (pStts != NULL) {
                    // Match the reference's lfs promotion, offset-before-position loads and
                    // double add followed by single rounding; preserve its NaN transport.
                    f64 moveX = (f32)pStts->GetCCMoveP()->x;
                    f64 positionX = (f32)dr->mpEnemy->current.pos.x;
                    dr->mpEnemy->current.pos.x = (f32)(positionX + moveX);
                    f64 positionZ = (f32)dr->mpEnemy->current.pos.z;
                    f64 moveZ = (f32)pStts->GetCCMoveP()->z;
                    dr->mpEnemy->current.pos.z = (f32)(positionZ + moveZ);
                }
            }

            if (dr->mInvincibleTimer == 0) {
                dr->mpEnemy->speed.y -= 12.0f;
                if (dr->mpEnemy->speed.y < -200.0f) {
                    dr->mpEnemy->speed.y = -200.0f;
                }
            }

            cLib_addCalc0(&dr->m4D4, 1.0f, 10.0f);
        } else if (dr->m4C8[0] == 1 && dr->m486 == 0) {
            dr->m486 = -0x4000;
            dr->m4AE = cM_rndF(20000.0f);
            dr->m4AC = cM_rndF(20000.0f);
            dr->m4AA = cM_rndF(-20000.0f);
            dr->m4A8 = cM_rndF(-20000.0f);
        }

        dr_body_bg_check(dr, scratch);
        if (dr_joint_bg_check(dr, scratch, auxiliary) == 2) {
            react = 2;
        }
    } else {
        dr->m7B6 = 0;
    }

    if (dr->m478 != 0.0f && react == 0 && dr->mAcch.ChkWallHit()) {
        react = 0x15;
        dr->m47C = 0;
        dr->mpEnemy->speed.y = 0.0f;
        dr->mpEnemy->speed.x *= -0.4f;
        dr->mpEnemy->speed.z *= -0.4f;

        if (dr->m70C == 0 && std::fabs(dr->m478) > 30.0f) {
            dr->m474 = 6000.0f;
            dr->m70E = 0xFF;

            if (dr->mpEnemy->current.pos.y - dr->mSpawnY > REG_F(14, 6) + 50.0f) {
                dr->m4C8[0] = 10;
                dr->m486 = 0;
            } else {
                dr->mpEnemy->speed.z = 0.0f;
                dr->mpEnemy->speed.x = 0.0f;
                dr->m486 = 0x4000;
            }

            int kado_check_temp = kado_check(address);
            if (kado_check_temp != 0) {
                dr->m4C8[0] = 0;
                dr->m486 = 0x4000;

                int temp;
                if (dr->mpEnemy->current.pos.y - dr->mSpawnY > REG0_F(6) + 90.0f) {
                    temp = 0x10000;
                } else {
                    temp = 0x8000;
                }

                if (kado_check_temp == 2) {
                    dr->m6E0 += temp;
                    temp2.x = dr->m478 * 0.5f;
                } else {
                    dr->m6E0 -= temp;
                    temp2.x = dr->m478 * -0.5f;
                }
                temp2.y = 0.0f;
                temp2.z = dr->m478 * 0.5f;
                mDoMtx_YrotS(calc_mtx(), dr->m482);
                MtxPosition(&temp2, &temp4);
                dr->mpEnemy->speed.x = temp4.x;
                dr->mpEnemy->speed.z = temp4.z;
            }
        }

        dr->m478 = 0.0f;
        dr->m4A8 = 0;
        dr->m4AA = 0;
        dr->m4AC = 0;
        dr->m4AE = 0;
        dr->mParticlePos.copy(dr->m100[0xC]);
        dr->mParticleAngle.z = 0;
        s16 wallYaw = cM_atan2s(-dr->m42C.x, -dr->m42C.z);
        fopAc_ac_c* wallActor = dr->mpEnemy.get();
        dr->mParticleAngle.y = static_cast<s16>(wall_angle_get(gabi::ea(wallActor), wallYaw));
        dr->mParticleAngle.x = 0x4000;
        dr->mParticleCallBack.end();
        s8 room = fopAcM_GetRoomNo(dr->mpEnemy);
        u32 play = dComIfGp_ea();
        u32 emitter = gabi::ea(
            damageView<dPa_control_c>(
                static_cast<u32>(damageWord(damageView<dComIfG_inf_c>(play)->play.mParticle)))
                ->set(2, 0x2022, &dr->mParticlePos, &dr->mParticleAngle, nullptr, 0xB9,
                      damageView<dPa_levelEcallBack>(&dr->mParticleCallBack), room, nullptr,
                      nullptr, nullptr));
        if (emitter) {
            f32 scale = dr->mEnemyType == 2 ? 1.0f : 1.2f;
            damageView<DamageEmitter>(emitter)->mRate = 3.0f;
            damageView<DamageEmitter>(emitter)->mSpread = 1.0f;
            cXyz vec;
            vec_ctor(gabi::ea(&vec));
            (&vec)->set(scale, scale, scale);
            vec_copy(emitter + 0x220, gabi::ea(&vec));
            vec_copy(emitter + 0x238, gabi::ea(&vec));
            (&vec)->set(scale + scale, scale + scale, scale + scale);
            vec_copy(emitter + 0x238, gabi::ea(&vec));
            dr->m794 = 6;
        }
    }

    if (dr->mAcch.ChkGroundHit()) {
        if (dr->m488 != 0 && react == 0 && (dr->m70C == 0 || !(dr->mpEnemy->speed.y > -100.0f))) {
            react = 0x14;
            dr->mpEnemy->speed.x *= 0.75f;
            dr->mpEnemy->speed.z *= 0.75f;
            dr->m70E = 0xFF;

            if (dr->m712 != 0) {
                dr->mpEnemy->speed.y = 60.0f;
                dr->m474 = gabi::fmadds(REG0_F(0xC), 100.0f, 9000.0f);
            } else {
                dr->mpEnemy->speed.y = 60.0f;
                dr->m474 = 8000.0f;
            }

            dr->m70C += 1;
            dr->m47C = 0;
            dr->m420 = REG0_S(3) + 1;

            s16 temp5 = dr->m482;
            s16 temp6 = dr->mpEnemy->current.angle.y - temp5;
            if (temp6 < 0) {
                temp6 = -temp6;
            }

            if (temp6 < 0x4000) {
                dr->m4C0 = temp5;
            } else {
                dr->m4C0 = temp5 + 0x8000;
            }

            dr->m49A = cM_rndFX(18000.0f);
            dr->m49C = cM_rndFX(10000.0f);
            dr->m4B0 = cM_rndF(20000.0f);
            dr->m4B4 = dr->m4B0 / 2;
            dr->m4B2 = cM_rndF(20000.0f);
            dr->m4B6 = dr->m4B2 / 2;

            if (dr->mpEnemy->health <= 0) {
                damage_sound_inline(dr->mpEnemy.get(), 0x580C);
            } else if (dr->mEnemyType == damagereaction::TYPE_DARKNUT) {
                if (static_cast<u8>(damageView<DamageDarknut>(dr->mpEnemy.get())->mRemainingEquipmentPieces) & 1) {
                    damage_sound_inline(dr->mpEnemy.get(), 0x58EE);
                } else {
                    damage_sound_inline(dr->mpEnemy.get(), 0x58EC);
                }
            } else {
                enemy_seStart(gabi::ea(dr->mpEnemy.get()), 0x580B, 0);
            }

            int kado_check_temp = kado_check(address);
            if (kado_check_temp != 0) {
                if (kado_check_temp == 2) {
                    dr->m6E0 += 0x8000;
                    temp2.x = dr->m478 * 0.3f;
                } else {
                    dr->m6E0 += -0x8000;
                    temp2.x = dr->m478 * -0.3f;
                }
                temp2.y = 0.0f;
                temp2.z = dr->m478 * 0.75f;
                mDoMtx_YrotS(calc_mtx(), dr->m482);
                MtxPosition(&temp2, &temp4);
                dr->mpEnemy->speed.x = temp4.x;
                dr->mpEnemy->speed.z = temp4.z;
            }
        } else {
            if (dr->m478 != 0) {
                dr->m420 = 1;
            }

            dr->m478 = 0.0f;

            if (dr->m6E0 == 0) {
                dr->mpEnemy->speed.z = 0.0f;
                dr->mpEnemy->speed.x = 0.0f;
            } else {
                cLib_addCalc0(&dr->mpEnemy->speed.x, 1.0f, 4.0f);
                cLib_addCalc0(&dr->mpEnemy->speed.z, 1.0f, 4.0f);
            }
            dr->mpEnemy->speed.y = -1.0f;

            dr->m4A8 = 0;
            dr->m4AA = 0;
            dr->m4AC = 0;
            dr->m4AE = 0;

            if (dr->m70D != 0) {
                dr->m4A8 = 0;
                dr->m4AA = 0;
                dr->m4AC = 0;
                dr->m4AE = 0;
                dr->m4A0 = 0;
                dr->m4A2 = 0;
                dr->m4A4 = 0;
                dr->m4A6 = 0;
            }
        }
    }

    if (dr->m6E0 != 0) {
        if (dr->m6E0 > 0) {
            dr->m6E0 += -0x1000;
            dr->m484 += -0x1000;
        } else {
            dr->m6E0 += 0x1000;
            dr->m484 += 0x1000;
        }
    }

    if (dr->m70C != 0) {
        if (dr->m488 != 0) {
            cLib_addCalcAngleS2(&dr->mpEnemy->current.angle.y, dr->m4C0, 1, 0x500);
        }

        cLib_addCalc0(&dr->m474, 1.0f, gabi::fmadds(REG0_F(5), 10.0f, 300.0f));
    }

    return react;
}

/* inlined into caller */
static void dr_damage_anime(damagereaction* dr) {
    csXyz angleTemp;
    csXyz& csxyz_temp = *(&angleTemp);

    for (int i = 0; i < 0x14; i++) {
        dr->m010[i].x = dr->m010[i].y = dr->m010[i].z = 0;
    }

    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    s16 maxSpeed;
    if (dr->m47C != 0 || test_sw != 0) {
        maxSpeed = 0x3000;

        f32 temp = (s16)(dr->m482 + 0x8000 - dr->mpEnemy->current.angle.y);
        f32 zero = 0.0f;
        f32 temp2 = temp;
        if (temp2 > 5000.0f) {
            temp2 = 5000.0f;
        }
        if (temp2 < -22000.0f) {
            temp2 = -22000.0f;
        }
        dr->m010[8].y = (0.4f * temp2) + 8192.0f;
        dr->m010[4].z = -(0.4f * temp2) - 8192.0f;
        dr->m010[8].z = 0.4f * zero + 0x2000;
        dr->m010[4].y = 0.4f * zero + 0x2000;

        f32 temp3 = temp;
        if (temp3 > 22000.0f) {
            temp3 = 22000.0f;
        }
        if (temp3 < -5000.0f) {
            temp3 = -5000.0f;
        }
        dr->m010[9].y = gabi::fmadds(temp3, 0.4f, -8192.0f);
        dr->m010[5].z = gabi::fmadds(temp3, 0.4f, -8192.0f);
        dr->m010[9].z = 0.4f * zero + 0x2000;
        dr->m010[5].y = 0.4f * -zero - 0x2000;

        f32 temp4 = temp;
        if (temp4 > 20000.0f) {
            temp4 = 20000.0f;
        }
        if (temp4 < -20000.0f) {
            temp4 = -20000.0f;
        }
        dr->m010[0x0C].x = 0.2f * temp4;
        dr->m010[0x13].x = 0.2f * temp4;
        dr->m010[0x0C].z = 0.2f * -zero;
        dr->m010[0x13].z = 0.2f * -zero;

        f32 temp5 = temp;
        if (temp5 > 7000.0f) {
            temp5 = 7000.0f;
        }
        if (temp5 < -10000.0f) {
            temp5 = -10000.0f;
        }
        dr->m010[6].x = (3000.0f - temp5) + REG0_S(0);
        dr->m010[6].z = (-zero - 0x4000) + REG0_S(1);

        f32 temp6 = temp;
        if (temp6 > 10000.0f) {
            temp6 = 10000.0f;
        }
        if (temp6 < -7000.0f) {
            temp6 = -7000.0f;
        }
        dr->m010[7].x = (-temp6 - 3000.0f);
        dr->m010[7].z = (-zero - 0x4000);
    } else {
        dr->m010[6].y += dr->m4B8;
        dr->m010[7].y -= dr->m4BA;
        dr->m010[4].y += dr->m4BC;
        dr->m010[5].y -= dr->m4BE;
        maxSpeed = 0x400;
    }

    cLib_addCalcAngleS2(&dr->m4A6, dr->m4AE, 1, 0x800);
    cLib_addCalcAngleS2(&dr->m4A4, dr->m4AC, 1, 0x800);
    cLib_addCalcAngleS2(&dr->m4A2, dr->m4AA, 1, 0x800);
    cLib_addCalcAngleS2(&dr->m4A0, dr->m4A8, 1, 0x800);
    cLib_addCalcAngleS2(&dr->m496, dr->m49A, 2, 0x200);
    cLib_addCalcAngleS2(&dr->m498, dr->m49C, 2, 0x400);

    dr->m010[6].z = dr->m010[6].z - dr->m4A4 / 2 - dr->m4B4;
    dr->m010[2].z = dr->m010[2].z + dr->m4A4 + dr->m4B0;
    dr->m010[7].z = dr->m010[7].z - dr->m4A6 / 2 - dr->m4B6;
    dr->m010[3].z = dr->m010[3].z + dr->m4A6 + dr->m4B2;
    dr->m010[0].z += dr->m4A0;
    dr->m010[1].z += dr->m4A2;
    dr->m010[0x12].y += dr->m496;
    dr->m010[0xD].y += dr->m498;

    if (dr->m70D != 0) {
        dr->m010[0x13].z += -0x960;
    }

    if (dr->m70C != 0 && dr->mpEnemy->speed.y > 0.0f) {
        dr->m010[0xC].z += -3000;
        dr->m010[0x13].z += -3000;
    }

    cXyz vectorTemp;
    cXyz vectorTemp2;
    cXyz& cxyz_temp = *(&vectorTemp);
    cXyz& cxyz_temp2 = *(&vectorTemp2);
    if (dr->m470 > 200.0f) {
        s16 angle = cM_atan2s(-dr->m42C.x, -dr->m42C.z);
        cxyz_temp.x = 0.0f;
        cxyz_temp.y = 0.0f;
        cxyz_temp.z = dr->m470;
        mDoMtx_YrotS(calc_mtx(), angle - dr->mpEnemy->current.angle.y);
        MtxPosition(&cxyz_temp, &cxyz_temp2);

        if (dr->mEnemyType == damagereaction::TYPE_BOKOBLIN) {
            dr->m010[0xC].z -= cxyz_temp2.z * 4.0f;
            dr->m010[0xC].y += cxyz_temp2.x * 4.0f;
            dr->m010[0xD].y += cxyz_temp2.x * 6.0f;
            dr->m010[0xD].z -= cxyz_temp2.z * 6.0f;
        } else if (dr->mEnemyType == damagereaction::TYPE_DARKNUT) {
            dr->m010[0xD].y += (s16)gabi::ftoi((f32)cxyz_temp2.z * (REG_F(8, 5) + 2.0f));
            dr->m010[0x12].y += (s16)gabi::ftoi((f32)cxyz_temp2.z * (REG_F(8, 5) + 2.0f));
            dr->m010[0xC].y += (s16)gabi::ftoi((f32)cxyz_temp2.z * (REG_F(8, 5) + 2.0f));
            dr->m010[0xD].x += (s16)gabi::ftoi((f32)cxyz_temp2.x * (REG_F(8, 6) + -2.0f));
            dr->m010[0x12].x += (s16)gabi::ftoi((f32)cxyz_temp2.x * (REG_F(8, 6) + -2.0f));
            dr->m010[0xC].x += (s16)gabi::ftoi((f32)cxyz_temp2.x * (REG_F(8, 6) + -2.0f));
        } else {
            dr->m010[0x13].z -= cxyz_temp2.z;
            dr->m010[0x13].y += cxyz_temp2.x;
            dr->m010[0xC].z -= cxyz_temp2.z;
            dr->m010[0xC].y += cxyz_temp2.x;
            dr->m010[0xD].y += cxyz_temp2.x * 4.0f;
            dr->m010[0xD].z -= cxyz_temp2.z * 4.0f;
        }

        maxSpeed = 0x3000;
    } else if (dr->m710 != 0 || dr->m49E != 0) {
        f32 f31 = 0.5f;
        maxSpeed = 0x800;

        s16 temp;
        s16 temp2;
        if (dr->m710 == 1 || dr->m710 == 5 || dr->m49E != 0) {
            if (dr->m710 == 1 || dr->m49E != 0) {
                if (dr->m49E != 0) {
                    dr->m49E--;
                    maxSpeed = 0x2000;
                    temp2 = (s16)gabi::ftoi((REG0_F(6) + 1000.0f) *
                                            cM_ssin(dr->m49E * (REG0_S(5) + 16000)));
                    temp = 0;
                } else {
                    cxyz_temp.x = dr->m714->eyePos.x - dr->mpEnemy->current.pos.x;
                    cxyz_temp.y = dr->m714->eyePos.y - (dr->mpEnemy->current.pos.y + 100.0f);
                    cxyz_temp.z = dr->m714->eyePos.z - dr->mpEnemy->current.pos.z;
                    temp2 = cM_atan2s(cxyz_temp.x, cxyz_temp.z) - dr->mpEnemy->current.angle.y;
                    temp = -cM_atan2s(cxyz_temp.y,
                                      std_sqrtf(gabi::fmadds((f32)cxyz_temp.x, (f32)cxyz_temp.x,
                                                             (f32)cxyz_temp.z * (f32)cxyz_temp.z)));
                }
            } else {
                temp = dr->m718;
                temp2 = dr->m71A;
            }

            if (temp2 > 0x2000) {
                temp2 = 0x2000;
            } else if (temp2 < -0x2000) {
                temp2 = -0x2000;
            }
            if (temp > 0x2000) {
                temp = 0x2000;
            } else if (temp < -0x2000) {
                temp = -0x2000;
            }

            if (dr->mEnemyType == damagereaction::TYPE_DARKNUT) {
                dr->m010[0xD].x += temp2 >> 2;
                dr->m010[0xD].y -= temp >> 2;
                dr->m010[0x12].x += temp2 >> 2;
                dr->m010[0x12].y -= temp >> 2;
            } else if (dr->m710 == 1) {
                if (dr->mEnemyType == damagereaction::TYPE_BOKOBLIN) {
                    dr->m010[0xD].x += temp2;
                    dr->m010[0xD].y -= temp2 >> 2;
                    dr->m010[0xD].z += temp;
                } else {
                    dr->m010[0xD].y -= temp2;
                    dr->m010[0xC].y -= temp2 * f31;
                    dr->m010[0xC].x = dr->m010[0xC].y + temp2 * f31;
                    dr->m010[0xD].z += temp;
                }
            } else {
                dr->m010[0xD].y -= (int)temp2;
                dr->m010[0xC].x = dr->m010[0xC].y + temp2;
                dr->m010[0xD].z += temp;
            }
        } else if (dr->m710 == 3) {
            temp = -6000;
            temp2 = -10000;
        } else if (dr->m710 == 4) {
            temp = 6000;
            temp2 = 10000;
        } else if (dr->m710 == 6) {
            dr->m010[0x12].y += dr->m71A;
            dr->m010[0x12].x += REG0_S(6);
            dr->m010[0x12].z += dr->m718;

            maxSpeed = 0x300;
        }

        if (dr->m710 == 3 || dr->m710 == 4) {
            maxSpeed = 0x800;

            dr->m010[0xC].x -= (int)temp;
            dr->m010[0xC].y += (int)temp;
            dr->m010[0xD].y += temp2;
        }
    }

    if (dr->m711 != 0) {
        dr->m010[8].x = 8000;
        dr->m010[9].x = -8000;
        maxSpeed = 0x3000;
    }

    if (dr->mAction != 5 && dr->m488 != 1) {
        cxyz_temp.x = dr->m48C.z;
        cxyz_temp.y = 0.0f;
        cxyz_temp.z = dr->m48C.x;
        mDoMtx_YrotS(calc_mtx(), dr->mpEnemy->current.angle.y);
        MtxPosition(&cxyz_temp, &cxyz_temp2);
        cLib_addCalcAngleS2(&dr->m492, (s16)gabi::ftoi((f32)cxyz_temp2.x), 4, 0x100);
        cLib_addCalcAngleS2(&dr->m494, (s16)gabi::ftoi((f32)cxyz_temp2.z), 4, 0x100);
    } else {
        cLib_addCalcAngleS2(&dr->m492, 0, 4, 0x100);
        cLib_addCalcAngleS2(&dr->m494, 0, 4, 0x100);
    }

    dr->m010[0xC].y -= (f32)dr->m492;
    dr->m010[0xC].z -= (f32)dr->m494;

    cLib_addCalc0(&dr->m470, 0.5f, 200.0f);

    if (dr->m478 == 0.0f && dr->m488 == 1) {
        if (dr->m70D == 0) {
            if (dr->m3D0[2] == 0 && dr->m408 == 0) {
                dr->m010[6].z += dr->m4B4;
                dr->m010[2].z = dr->m010[2].z - dr->m4B0 + 13000;

                if (dr->m440 > 2000) {
                    dr->m010[2].z += -7000;
                }
            }

            if (dr->m3D0[3] == 0 && dr->m40C == 0) {
                dr->m010[7].z += dr->m4B6;
                dr->m010[3].z = dr->m010[3].z - dr->m4B2 + 13000;

                if (dr->m442 > 2000) {
                    dr->m010[3].z += -7000;
                }
            }
        } else {
            if (dr->m3D0[0] == 0) {
                dr->m010[0].z += -10000;
            }

            if (dr->m3D0[1] == 0) {
                dr->m010[1].z += -10000;
            }
        }
    }

    if (dr->m478 == 0.0f && dr->m488 == 1) {
        s16 temp = 1;
        if (dr->m70D == 0) {
            temp = -1;
        }

        dr->m010[6].z -= dr->m440 * temp;
        dr->m010[7].z -= dr->m442 * temp;
        dr->m010[4].z -= dr->m444 * temp;
        dr->m010[5].z -= dr->m446 * temp;
        dr->m010[0xC].z += temp * (s16)(dr->m448 / 2);
        dr->m010[0x13].z += temp * (s16)(dr->m448 / 2);

        if (dr->m418 == 2) {
            dr->m010[0xD].z += temp * (s16)(dr->m448 / 2);
            dr->m010[0x12].z += temp * (s16)(dr->m448 / 2);
        }
    }

    switch (dr->mEnemyType) {
    case damagereaction::TYPE_MOBLIN:
    case damagereaction::TYPE_BOKOBLIN:
        dr->m010[0].y = -dr->m010[0].z;
        dr->m010[0].z = 0;
        dr->m010[1].y = dr->m010[1].z;
        dr->m010[1].z = 0;

        if (dr->mEnemyType == damagereaction::TYPE_BOKOBLIN) {
            dr->m010[0].y = -dr->m010[0].y;
            dr->m010[0].x = -dr->m010[0].x;
        }

        {
            s16 temp = dr->m010[4].z;
            dr->m010[4].z = dr->m010[4].y;
            dr->m010[4].y = -temp;
            s16 temp2 = dr->m010[5].z;
            dr->m010[5].z = -dr->m010[5].y;
            dr->m010[5].y = temp2;
        }

        break;
    case damagereaction::TYPE_DARKNUT:
        for (int i = 0; i < 0x14; i++) {
            if (i != 0xD && i != 0x12) {
                csXyz cleared;
                csXyz* value = csXyz_ct(&cleared, 0, 0, 0);
                dr->m010[i].x = static_cast<s16>(value->x);
                dr->m010[i].y = static_cast<s16>(value->y);
                dr->m010[i].z = static_cast<s16>(value->z);
            }
        }

        dr->m010[8].z = dr->m4BC;
        dr->m010[9].z = dr->m4BE;

        if (dr->m47C != 0) {
            if (test_sw != 0) {
                dr->m482 = fopAcM_searchPlayerAngleY(dr->mpEnemy) + 0x8000 + REG0_S(3);
            }

            f32 temp = -(s16)(dr->m482 + 0x8000 - dr->mpEnemy->current.angle.y);
            f32 temp2 = temp;
            if (temp2 > 20000.0f) {
                temp2 = 20000.0f;
            }
            if (temp2 < -9000.0f) {
                temp2 = -9000.0f;
            }
            dr->m010[4].x = temp2;
            dr->m010[8].y = -5000;
            dr->m010[8].z = -5000;
            dr->m010[0].y = -10000;
            dr->m010[4].z = -15000;

            f32 temp3 = temp;
            if (temp3 > 9000.0f) {
                temp3 = 9000.0f;
            }
            if (temp3 < -20000.0f) {
                temp3 = -20000.0f;
            }
            dr->m010[5].x = temp3;
            dr->m010[9].y = -5000;
            dr->m010[9].z = 5000;
            dr->m010[1].y = -10000;
            dr->m010[5].z = 15000;
            dr->m010[0xA].y = -15000;
            dr->m010[0xB].y = -15000;

            f32 temp4 = temp;
            if (temp4 > 10000.0f) {
                temp4 = 10000.0f;
            }
            if (temp4 < -10000.0f) {
                temp4 = -10000.0f;
            }
            dr->m010[0xA].z = temp4;
            dr->m010[0XB].z = temp4;
            dr->m010[0XC].y = 5000;

            if (temp > 8000.0f) {
                temp = 8000.0f;
            }
            if (temp < -8000.0f) {
                temp = -8000.0f;
            }
            dr->m010[0xC].z = temp;
        }

        break;
    }

    csxyz_temp.x = 0;
    csxyz_temp.y = 0;
    csxyz_temp.z = 0;
    for (int i = 0; i < 20; i++) {
        if ((dr->m474 > 0.1f && (i == 2 || i == 6 || i == 7 || i == 3) && (dr->m70E & 8)) ||
            ((i == 0 || i == 4 || i == 5 || i == 1) && (dr->m70E & 4)) ||
            (i == 0x13 && (dr->m70E & 2)) || (i == 0xD && (dr->m70E & 1))) {
            dr->m010[i].y = gabi::fmadds(cM_ssin((u32)dr->mTimer * 5000 + i * 0xED8), (f32)dr->m474,
                                         (f32)(s16)dr->m010[i].y);
            dr->m010[i].z = gabi::fmadds(cM_scos((u32)dr->mTimer * 4000 + i * 0xC80), (f32)dr->m474,
                                         (f32)(s16)dr->m010[i].z);
        }

        cLib_addCalcAngleS2(&dr->m088[i].x, dr->m010[i].x + csxyz_temp.x, 2, (int)maxSpeed);
        cLib_addCalcAngleS2(&dr->m088[i].y, dr->m010[i].y + csxyz_temp.y, 2, (int)maxSpeed);
        cLib_addCalcAngleS2(&dr->m088[i].z, dr->m010[i].z + csxyz_temp.z, 2, (int)maxSpeed);
    }
}

/* inlined into caller */
static void dr_matrix_set(damagereaction* dr) {
    MtxTrans(dr->mpEnemy->current.pos.x + dr->m458.x, dr->mpEnemy->current.pos.y + dr->m458.y,
             dr->mpEnemy->current.pos.z + dr->m458.z, 0);
    mDoMtx_XrotM(calc_mtx(), dr->m48C.x);
    mDoMtx_ZrotM(calc_mtx(), dr->m48C.z);
    MtxTrans(-dr->m44C.x, -dr->m44C.y, -dr->m44C.z, 1);
    mDoMtx_YrotM(calc_mtx(), dr->m482);
    mDoMtx_XrotM(calc_mtx(), dr->m480);
    mDoMtx_YrotM(calc_mtx(), dr->m484);
    mDoMtx_YrotM(calc_mtx(), -(s16)dr->m482);
    MtxTrans(dr->m44C.x, dr->m44C.y, dr->m44C.z, 1);
    mDoMtx_YrotM(calc_mtx(), dr->mpEnemy->shape_angle.y);
    mDoMtx_XrotM(calc_mtx(), dr->mpEnemy->shape_angle.x);
    mDoMtx_ZrotM(calc_mtx(), dr->mpEnemy->shape_angle.z);
    MtxTrans(0.0f, dr->m468, dr->m46C, 1);
    cXyz v;
    cXyz out;
    (&v)->set(0.0f, 0.0f, 10000.0f);
    MtxPosition((&v), (&out));
    dr->m70D = (&out)->y < dr->mpEnemy->current.pos.y ? 1 : 0;
    dr->m424 = 0;
}

/* 02041F94 */
static s32 damage_reaction(u32 address) {

    damagereaction* dr = gabi::at<damagereaction>(address);
    dr->mTimer++;
    for (u32 i = 0; i < 3; i++)
        if (dr->m4C8[i] != 0)
            dr->m4C8[i]--;
    s32 result = 0;
    if (dr->mInvincibleTimer != 0) {
        dr->mInvincibleTimer--;
        dr->mSpawnY = dr->mpEnemy->current.pos.y;
    } else
        result = dr_damage_set(address);
    dr_damage_anime(dr);
    dr_matrix_set(dr);
    if (dr->m794 != 0) {
        dr->m794--;
        if (dr->m794 == 0)
            dr->mParticleCallBack.end();
    }
    if (dr->m7B2 != 0)
        dr->m7B2--;
    return result;
}

// HD: compiler-generated and template code of this translation unit

/* 02043AE4 */
static void init_nonpos() {

    l_reactionHeaderStatics.mZero[3] = 0;
    l_reactionHeaderStatics.mZero[2] = 0;
    l_reactionHeaderStatics.mZero[1] = 0;
    l_reactionHeaderStatics.mZero[0] = 0;
    __register_global_object(0x1018F644);
    f32 a = -3.1415927f, b = 3.1415927f;
    l_reactionHeaderStatics.mMinAngle = a;
    l_reactionHeaderStatics.mMaxAngle = b;
    gabi::call(0x028ED6F8, 0x10461308);
    __register_global_object(0x1018F650);
    gabi::call(0x028EAB2C, 0x10461309);
    __register_global_object(0x1018F65C);
    f32 xy = -20000.0f, z = 20000.0f;
    non_pos.x = xy;
    non_pos.y = xy;
    non_pos.z = z;
}

/* 02043C04 */
static u32 vec_ctor(u32 p) {
    if (p == 0)
        return gabi::ea(operator_new(12));
    return p;
}

/* 02043C30 */
static void vec_copy(u32 out, u32 in) {

    auto* dst = damageView<cXyz>(out);
    auto* src = damageView<cXyz>(in);
    damageWord(dst->x) = static_cast<u32>(damageWord(src->x));
    damageWord(dst->y) = static_cast<u32>(damageWord(src->y));
    damageWord(dst->z) = static_cast<u32>(damageWord(src->z));
}

/* 020468F8 */
static void sdk_init() {

    l_drawHeaderStatics.mZero[2] = 0;
    l_drawHeaderStatics.mZero[0] = 0;
    l_drawHeaderStatics.mZero[3] = 0;
    l_drawHeaderStatics.mZero[1] = 0;
    __register_global_object(0x1018F668);
    f32 a = -3.1415927f, b = 3.1415927f;
    l_drawHeaderStatics.mMinAngle = a;
    l_drawHeaderStatics.mMaxAngle = b;
    gabi::call(0x028ED6F8, 0x10461330);
    __register_global_object(0x1018F674);
    gabi::call(0x028EAB2C, 0x10461331);
    __register_global_object(0x1018F680);
}

/* 0204698C */
static u32 sdk_ctor(u32 p) {

    if (p == 0) {
        p = gabi::ea(operator_new(0x5C));
        if (p == 0)
            return 0;
    }
    auto* list = damageView<DamageDrawList>(p);
    gabi::call(0x0252CCBC, list);
    list->mRandomState[0] = 0;
    list->mRandomState[2] = 0;
    list->mRandomState[1] = 0;
    list->__vtbl = 0x10006C80;
    for (u32 i = 0; i < 9; i++)
        gabi::call(0x028F521C, list->mEntries[i], 8);
    list->mValue = 0.0f;
    return p;
}

/* WWHD glue
data l_reactionHeaderStatics 10461300
data l_drawHeaderStatics 10461328
data non_pos 1046131C
data test_sw 1018F5AC
data kado_bit 1018F634
data kado_check_x 1018F63C
 */
