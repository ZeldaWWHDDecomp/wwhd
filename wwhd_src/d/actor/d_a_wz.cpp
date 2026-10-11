/**
 * d_a_wz.cpp (WWHD)
 * Enemy - Wizzrobe / Mini-Boss - Wizzrobe (Wind Temple)
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_wz.cpp) has only "Nonmatching" placeholders for this unit, so every function
 * here is written from the WWHD code (cking.rpx) and verified against it. GameCube names are
 * kept where the matcher found them; the GameCube helpers hontai_draw / summon_door_draw /
 * damage_ball_draw / action_dousa / action_itai / summon_call_sub / sea_water_check /
 * action_tama_dousa are inlined into their WWHD callers.
 */
#include "d/actor/d_a_wz.h"
#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"


// HD: per-TU tables and mutable state, owned once by this source.
static const s32 l_summonNum[120] = {
    8, 0, 2, 0, 1, 3, 6, 0,
    8, 0, 2, 0, 6, 0, 1, 3,
    2, 0, 1, 0, 1, 0, 1, 0,
    20, 0, 15, 0, 15, 0, 20, 0,
    8, 0, 10, 0, 8, 0, 8, 0,
    2, 0, 2, 0, 1, 1, 1, 2,
    1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 0, 1, 0, 1, 0,
    2, 0, 6, 0, 3, 0, 2, 0,
    4, 0, 1, 0, 1, 2, 3, 0,
    4, 0, 1, 0, 3, 0, 1, 2,
    10, 0, 8, 0, 8, 0, 10, 0,
    4, 0, 5, 0, 4, 0, 4, 0,
    1, 0, 1, 1, 1, 1, 1, 1,
    2, 0, 4, 0, 2, 2, 4, 8
};
static const s16 l_summonName[120] = {
    215, 32767, 216, 32767, 216, 215, 215, 32767,
    215, 32767, 207, 32767, 215, 32767, 207, 215,
    207, 32767, 207, 32767, 207, 32767, 207, 32767,
    205, 32767, 205, 32767, 205, 32767, 205, 32767,
    206, 32767, 206, 32767, 206, 32767, 206, 32767,
    207, 32767, 207, 32767, 207, 207, 212, 207,
    190, 32767, 224, 32767, 190, 32767, 224, 32767,
    188, 32767, 191, 32767, 188, 32767, 191, 32767,
    209, 32767, 215, 32767, 209, 32767, 181, 32767,
    215, 32767, 216, 32767, 216, 215, 215, 32767,
    215, 32767, 207, 32767, 215, 32767, 207, 215,
    205, 32767, 205, 32767, 205, 32767, 205, 32767,
    206, 32767, 206, 32767, 206, 32767, 206, 32767,
    189, 32767, 189, 188, 189, 189, 189, 188,
    181, 32767, 215, 32767, 206, 206, 215, 205
};
static const u32 l_summonParam[120] = {
    0xffffff03, 0x0, 0xffffff00, 0x0, 0xffffff00, 0xffffff03, 0xffffff03, 0x0,
    0xffffff03, 0x0, 0x100ff00, 0x0, 0xffffff03, 0x0, 0x100ff00, 0xffffff03,
    0x100ff01, 0x0, 0x100ff01, 0x0, 0x100ff01, 0x0, 0x100ff01, 0x0,
    0x3, 0x0, 0x3, 0x0, 0x3, 0x0, 0x3, 0x0,
    0xffff0000, 0x0, 0xffff0000, 0x0, 0xffff0000, 0x0, 0xffff0000, 0x0,
    0x100ff00, 0x0, 0x100ff00, 0x0, 0x100ff01, 0x100ff00, 0xff000000, 0x100ff01,
    0xffffff00, 0x0, 0xfffffffe, 0x0, 0xffffff00, 0x0, 0xfffffffe, 0x0,
    0xffffff00, 0x0, 0xffffff20, 0x0, 0xffffff00, 0x0, 0xffffff20, 0x0,
    0xfff0f000, 0x0, 0xffff0003, 0x0, 0xfff0f000, 0x0, 0xffffff03, 0x0,
    0xffffff03, 0x0, 0xffffff00, 0x0, 0xffffff00, 0xffffff03, 0xffffff03, 0x0,
    0xffffff03, 0x0, 0x100ff00, 0x0, 0xffffff03, 0x0, 0x100ff00, 0xffffff03,
    0x3, 0x0, 0x3, 0x0, 0x3, 0x0, 0x3, 0x0,
    0xffff0000, 0x0, 0xffff0000, 0x0, 0xffff0000, 0x0, 0xffff0000, 0x0,
    0xffffff00, 0x0, 0xffffff00, 0xffffff00, 0xffffff00, 0xffffff20, 0xffffff00, 0xffffff00,
    0xffffff03, 0x0, 0xffffff03, 0x0, 0xffff0a00, 0xffff0b00, 0xffff0003, 0x3
};
static const u32 l_summonColor[3] = {
    0x0, 0x1, 0x4
};
static const u8 l_fireJoints[10] = {
    1, 2, 6, 7, 8, 12, 13, 17,
    19, 21
};
static const f32 l_fireScales[10] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f
};
static u8 l_demoSignal;
static wz::DebugFields l_debug;
static u32 l_systemTextures;


static const GXColor l_rodPrimColor[3] = {
    {255, 71, 0, 255},
    {255, 255, 8, 255},
    {255, 255, 160, 255}
};
static const GXColor l_rodEnvColor[3] = {
    {255, 0, 0, 255},
    {125, 72, 56, 255},
    {75, 186, 255, 255}
};

// HD: this source owns the translation unit's constructor/header metadata.
const gc_tu_data gc_tu = {
    .safeString = 0x10042D38,
    .headerStatics = {0x1046EC1C, 0x101D3B48, 0x1046EC28},
};

static s8 l_stayRoom; // HD: shared stay-room byte, mapped in glue.

#define M_arcname STR(0x10042DC8)  /* "Wz" */
#define SAFESTRING_VTBL 0x10042D38 /* this TU's sead::SafeString vtable */

// Draw, animation and damage functions, in GameCube source order.

/* 024E1EE8 */
static BOOL nodeCallBack(J3DNode* node, s32 timing) {
    return wz_jointCallBack(node, timing, wz::JOINT_HAND, &wz_class::mHandPos);
}

/* 024E2018 */
static BOOL rod_nodeCallBack(J3DNode* node, s32 timing) {
    return wz_jointCallBack(node, timing, wz::JOINT_ROD_TIP, &wz_class::mRodTipPos);
}

/* 024E2148 */
void draw_SUB(wz_class* i_this) {
    mDoExt_McaMorf* morf = i_this->m2B4;
    f32 sz = i_this->scale.z;
    J3DModel* model = morf->getModel();
    f32 sx = i_this->scale.x, sy = i_this->scale.y;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    model->mBaseScale.z = sz;
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    mtx_copy(&model->mBaseTransformMtx, mDoMtx_stack_c::get());
    i_this->m2B4->calc();
    if (i_this->mBehaviorType < wz::FIRE_BALL) {
        if (i_this->mIsMiniBoss) {
            // The second body follows the back joint.
            wz_jointMtxCopy(i_this->m2B4, wz::JOINT_SECOND_BODY, i_this->mpMorf2);
            i_this->mpMorf2->calc();
        }
        mDoExt_McaMorf* rodMorf = i_this->mpRodMorf;
        f32 rx = i_this->mRodScaleX;
        J3DModel* rod = rodMorf->getModel();
        f32 rz = i_this->mRodScaleZ, ry = i_this->mRodScaleY;
        rod->mBaseScale.x = rx;
        rod->mBaseScale.y = ry;
        rod->mBaseScale.z = rz;
        // The rod follows the hand joint.
        wz_jointMtxCopy(i_this->m2B4, wz::JOINT_HAND, i_this->mpRodMorf);
        i_this->mpRodMorf->calc();
        enemy_fire(i_this->mEnemyFire);
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
}

/* 024E23B4 */
static void wz_colorToF(wz::ColorF* out, const GXColor* in) {
    f32 r = (f32)(u8)in->r / 255.0f;
    f32 g = (f32)(u8)in->g / 255.0f;
    f32 b = (f32)(u8)in->b / 255.0f;
    f32 a = (f32)(u8)in->a / 255.0f;
    out->r = r;
    out->g = g;
    out->b = b;
    out->a = a;
}

/* 024E2468 */
static BOOL daWZ_Draw(wz_class* i_this) {
    u8 type = i_this->mBehaviorType;
    if (type == wz::FIRE_BALL || type == wz::SUMMON_BALL) {
        /* damage ball: only the matrix */
        if (i_this->mBallFlareTimer == 0) {
            return TRUE;
        }
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
        MtxScale(0.6f, 0.6f, 0.6f, 1);
        mDoMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
        PSMTXCopy(calc_mtx(), &i_this->mBallMtx);
        return TRUE;
    }
    J3DModel* model = i_this->m2B4->getModel();
    if (type >= wz::FIRE_BALL) {
        /* summon door */
        setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
        mDoExt_btkAnm* btk = i_this->mpBtkAnm;
        btk->entry(model->getModelData(), btk->getFrame());
        i_this->m2B4->updateDL();
        wz::view<wz::ModelDataAnimations>(model->getModelData())->mpTextureSRTAnm = 0;
        return TRUE;
    }
    J3DModelData* data = model->getModelData();
    J3DModel* rodModel = i_this->mpRodMorf->getModel();
    if (i_this->mEnableSpawnSwitch != 0xFF &&
        !dComIfGs_isSwitch(i_this->mEnableSpawnSwitch, l_stayRoom)) {
        return TRUE;
    }
    if (!i_this->mIsMiniBoss && i_this->mAlpha >= 0x80) {
        dSnap_RegistFig(0xC4, i_this, 1.0f, 1.0f, 1.0f);
    }
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), rodModel, &i_this->tevStr);
    wz_setMaterialAlpha(i_this, data);
    if (i_this->mIsMiniBoss) {
        J3DModel* model2 = i_this->mpMorf2->getModel();
        J3DModelData* data2 = model2->getModelData();
        setLightTevColorType(dKy_getEnvlight(), model2, &i_this->tevStr);
        wz_setMaterialAlpha(i_this, data2);
        if (i_this->mAlpha >= 0x80) {
            dSnap_RegistFig(0xC5, i_this, 1.0f, 1.0f, 1.0f);
        }
    }
    u32 translucent = i_this->mAlpha != 0xFF ? 1 : 0;
    wz_setTranslucent(model, translucent);
    if (i_this->mIsMiniBoss) {
        wz_setTranslucent(i_this->mpMorf2->getModel(), translucent);
    }
    if (i_this->mIceTimer > 20) {
        /* frozen */
        if (i_this->mAlpha != 0xFF) {
            wz_iceEntryDL(i_this->m2B4, -1, 0);
            if (i_this->mIsMiniBoss) {
                wz_iceEntryDL(i_this->mpMorf2, -1, 0);
            }
        } else {
            wz_iceInvisibleDL(i_this->m2B4, -1, wz::view<wz::InvisibleModel>(i_this->mInvisModel));
            if (i_this->mIsMiniBoss) {
                wz_iceInvisibleDL(i_this->mpMorf2, -1,
                                  wz::view<wz::InvisibleModel>(i_this->mInvisModel2));
            }
        }
        return TRUE;
    }
    dComIfGp_get();
    mDoExt_brkAnm* brk = i_this->mpBrkAnm;
    brk->entry(model->getModelData(), brk->getFrame());
    f32 frame = (f32)(s32)(i_this->mBehaviorType + i_this->mIsMiniBoss);
    brk = i_this->mpBrkAnm;
    if (i_this->m351) {
        frame = 3.0f;
    }
    brk->setFrame(frame);
    i_this->m2B4->entryDL();
    if (i_this->mIsMiniBoss) {
        i_this->mpMorf2->entryDL();
        if (i_this->mAlpha != 0xFF) {
            wz_invisibleEntry(wz::view<wz::InvisibleModel>(i_this->mInvisModel2));
        } else {
            wz_invisibleEntryOpa(wz::view<wz::InvisibleModel>(i_this->mInvisModel2));
        }
    }
    wz::view<wz::ModelDataAnimations>(model->getModelData())->mpTevRegAnm = 0;
    if (i_this->mAlpha != 0xFF) {
        wz_invisibleEntry(wz::view<wz::InvisibleModel>(i_this->mInvisModel));
    } else {
        wz_invisibleEntryOpa(wz::view<wz::InvisibleModel>(i_this->mInvisModel));
    }
    if (i_this->mAlpha == 0) {
        return TRUE;
    }
    i_this->mpRodMorf->entryDL();
    return TRUE;
}

/* 024E2A90 */
void anm_init(wz_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 playSpeed,
              int soundFileIdx) {
    i_this->mCurrBckIdx = bckFileIdx;
    if (soundFileIdx >= 0) {
        void* anm = dComIfG_getObjectRes("Wz", bckFileIdx, SAFESTRING_VTBL);
        void* snd = dComIfG_getObjectRes("Wz", soundFileIdx, SAFESTRING_VTBL);
        i_this->m2B4->setAnm((J3DAnmTransform*)anm, loopMode, morf, playSpeed, 0.0f, -1.0f, snd);
    } else {
        void* anm = dComIfG_getObjectRes("Wz", bckFileIdx, SAFESTRING_VTBL);
        i_this->m2B4->setAnm((J3DAnmTransform*)anm, loopMode, morf, playSpeed, 0.0f, -1.0f,
                             nullptr);
    }
}

/* 024E2BBC */
void rod_size_set(wz_class* i_this, u8 shrink) {
    if (shrink == 0) {
        cLib_addCalc2(&i_this->mRodScaleX, 1.0f, 0.2f, 0.5f);
        *wz::view<be<u32>>(&i_this->mRodScaleZ) = *wz::view<be<u32>>(&i_this->mRodScaleX);
        *wz::view<be<u32>>(&i_this->mRodScaleY) = *wz::view<be<u32>>(&i_this->mRodScaleX);
    } else {
        cLib_addCalc0(&i_this->mRodScaleX, 0.2f, 0.5f);
        *wz::view<be<u32>>(&i_this->mRodScaleY) = *wz::view<be<u32>>(&i_this->mRodScaleX);
        *wz::view<be<u32>>(&i_this->mRodScaleZ) = *wz::view<be<u32>>(&i_this->mRodScaleX);
    }
}

/* 024E2C34 */
BOOL body_atari_check(wz_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    i_this->mStts.Move();
    i_this->mHitKind = wz::HIT_NORMAL;
    if (!i_this->mBodyCyl.ChkTgHit()) {
        i_this->mHitLock = 0;
        return FALSE;
    }
    dCcD_GObjInf* hit = static_cast<dCcD_GObjInf*>(i_this->mBodyCyl.GetTgHitObj());
    if (hit == nullptr || i_this->mHitLock != 0) {
        return FALSE;
    }
    gabi::Local<wz::CcAtInfo> info;
    info->pParticlePos = 0;
    i_this->mHitLock = 1;
    u32 type = hit->mAtType;
    switch (type) {
    case wz::AT_TYPE_SWORD: { /* sword */
        wz_se(i_this, wz::JA_SE_LK_SW_HIT_S, 0x20);
        i_this->mHitKind = wz::HIT_NORMAL;
        u8 mode = wz::view<wz::Player>(player)->mCutType;
        if ((mode >= 5 && mode <= 10) || mode == 12 || (mode >= 14 && mode <= 16) || mode == 21 ||
            mode == 23 || (mode >= 25 && mode <= 27) || (mode >= 30 && mode <= 31)) {
            i_this->mHitKind = wz::HIT_CRITICAL;
        }
        break;
    }
    case wz::AT_TYPE_BOMB:
        i_this->mHitKind = wz::HIT_BOMB;
        break;
    case wz::AT_TYPE_BOOMERANG: {
        i_this->mHitKind = wz::HIT_BOOMERANG;
        dPa_control_set(dComIfGp_getParticle(), 0, wz::ID_IT_JN_PIYOHIT00,
                        &i_this->attention_info.position, nullptr, nullptr, 0xFF, nullptr, -1,
                        nullptr, nullptr, nullptr);
        wz_stunSe(i_this, true);
        i_this->mAction = 1;
        i_this->mMode = 10;
        return TRUE;
    }
    case wz::AT_TYPE_BOKO_STICK:
        wz_se(i_this, wz::JA_SE_LK_W_WEP_HIT, 0x20);
        break;
    case wz::AT_TYPE_FIRE:
    case wz::AT_TYPE_FIRE_ARROW:
        wz::view<wz::FirePrefix>(i_this->mEnemyFire)->mFireDuration =
            100; /* enemyfire: burn timer */
        break;
    case wz::AT_TYPE_SKULL_HAMMER:
        wz_se(i_this, wz::JA_SE_LK_HAMMER_HIT, 0x20);
        i_this->mHitKind = wz::HIT_HAMMER;
        if (wz::view<wz::Player>(player)->mCutType == 0x11) {
            i_this->mHitKind = wz::HIT_HAMMER_CRITICAL;
        }
        break;
    case wz::AT_TYPE_ICE_ARROW: /* ice arrow */
        wz_followEnd(i_this, 2);
        wz_followEnd(i_this, 3);
        wz_followEnd(i_this, 4);
        i_this->attention_info.flags = 0;
        i_this->mFreezeDuration = 200;
        i_this->mRodScaleY = 0.0f;
        i_this->mRodScaleZ = 0.0f;
        i_this->mAction = 1;
        i_this->mIceMode = 2;
        i_this->mMode = 10;
        i_this->mRodScaleX = 0.0f;
        return TRUE;
    case wz::AT_TYPE_LIGHT: /* light arrow */
        i_this->mIceScaleXZ = 1.0f;
        i_this->mFreezeYOffset = 80.0f;
        i_this->mLightShrinkTimer = 1;
        {
            dCcD_Cyl* body = (&i_this->mBodyCyl);
            i_this->attention_info.flags = 0;
            body->mObjTg.mSPrm &= ~1u;
        }
        i_this->mBodyCyl.ClrTgHit();
        rod_size_set(i_this, 1);
        wz_followEnd(i_this, 2);
        wz_followEnd(i_this, 3);
        wz_followEnd(i_this, 4);
        break;
    case wz::AT_TYPE_WIND:
        i_this->mHitKind = wz::HIT_WIND;
        wz_stunSe(i_this, true);
        i_this->mAction = 1;
        i_this->mMode = 10;
        return TRUE;
    case wz::AT_TYPE_GRAPPLING_HOOK:
        if (i_this->stealItemLeft > 0) {
            s8 health = i_this->health;
            i_this->health = 10;
            info->mpObj = static_cast<dCcD_GObjInf*>(i_this->mBodyCyl.GetTgHitObj());
            wz_ccAtCheck(i_this, info.get());
            i_this->health = health;
            i_this->mStealCount = (u8)(i_this->mStealCount + 1);
        }
        dPa_control_set(dComIfGp_getParticle(), 0, wz::ID_IT_JN_PIYOHIT00,
                        &i_this->attention_info.position, nullptr, nullptr, 0xFF, nullptr, -1,
                        nullptr, nullptr, nullptr);
        i_this->mHitKind = wz::HIT_GRAPPLE;
        wz_stunSe(i_this, false);
        i_this->mAction = 1;
        i_this->mMode = 10;
        return TRUE;
    default:
        i_this->mHitKind = wz::HIT_NORMAL;
        wz_se(i_this, wz::JA_SE_LK_MS_WEP_HIT, 0x20);
        break;
    }
    gabi::Local<cXyz> pos;
    {
        dCcD_Cyl* body = (&i_this->mBodyCyl);
        f32 x = body->mGObjTg.mHitPos.x, y = body->mGObjTg.mHitPos.y, z = body->mGObjTg.mHitPos.z;
        pos->x = x;
        pos->y = y;
        pos->z = z;
    }
    info->mpObj = static_cast<dCcD_GObjInf*>(i_this->mBodyCyl.GetTgHitObj());
    wz_ccAtCheck(i_this, info.get());
    u8 kind = i_this->mHitKind;
    if (kind == wz::HIT_CRITICAL || kind == wz::HIT_HAMMER || kind == wz::HIT_HAMMER_CRITICAL ||
        i_this->health <= 0) {
        dPa_control_set(dComIfGp_getParticle(), 0, wz::ID_AK_JN_CRITICALHITFLASH, pos.get(),
                        nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
        gabi::Local<cXyz> scale;
        scale->x = 2.0f;
        scale->y = 2.0f;
        scale->z = 2.0f;
        dPa_control_set(dComIfGp_getParticle(), 0, wz::ID_AK_JN_CRITICALHIT, pos.get(),
                        &player->shape_angle, scale.get(), 0xFF, nullptr, -1, nullptr, nullptr,
                        nullptr);
    } else {
        dPa_control_set(dComIfGp_getParticle(), 0, wz::ID_AK_JN_OK, pos.get(), &player->shape_angle,
                        nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    }
    i_this->mAction = 1;
    i_this->mMode = 10;
    return TRUE;
}

/* 024E359C */
void BG_check(wz_class* i_this) {
    i_this->mAcchCir.SetWall(i_this->mAcchWallH, i_this->mAcchWallR);
    i_this->mCorrectionOffsetY = 20.0f;
    i_this->old.pos.y = i_this->old.pos.y - 20.0f;
    i_this->current.pos.y = i_this->current.pos.y - 20.0f;
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    f32 off = i_this->mCorrectionOffsetY;
    i_this->current.pos.y = i_this->current.pos.y + off;
    i_this->old.pos.y = i_this->old.pos.y + off;
}

/* 024E3624 */
void weapon_shoot(wz_class* i_this, u8 kind) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<csXyz> angle;
    for (int k = 0; k < 2; k++) { /* the shape angle is copied twice */
        angle->x = i_this->shape_angle.x;
        angle->y = i_this->shape_angle.y;
        angle->z = i_this->shape_angle.z;
    }
    cXyz* eye = &i_this->eyePos;
    if (eye != nullptr) {
        s8 room = i_this->current.roomNo;
        u32 id = fopAcM_GetID(i_this);
        mDoAud_monsSeStart(wz::JA_SE_CV_WZ_ATTACK, eye, id, 0, dComIfGp_getReverb(room));
    }
    if (kind == 0) {
        /* three fire balls in a fan */
        f32 dz = player->current.pos.z - i_this->mRodTipPos.z;
        f32 dx = player->current.pos.x - i_this->mRodTipPos.x;
        f32 dy = (player->current.pos.y + 50.0f) - i_this->mRodTipPos.y;
        angle->x = (s16)-cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz)));
        s16 spread = -3000;
        for (int i = 3; i != 0; i--) {
            angle->y = (s16)(cM_atan2s(dx, dz) + spread);
            fopAcM_create(wz::fpcNm_WZ_e, 0xFFFFFF0A, &i_this->mRodTipPos, i_this->current.roomNo,
                          angle.get(), nullptr, -1, 0);
            spread = (s16)(spread + 3000);
        }
        if (eye != nullptr) {
            mDoAud_seStart(wz::JA_SE_CM_WZ_ST_FIREBALL, eye, 0,
                           dComIfGp_getReverb(i_this->current.roomNo));
        }
    } else if (kind == 1) {
        /* summon: a child at the summon point */
        if (i_this->mSummonSw == 0xFF || i_this->m540 == 0) {
            return;
        }
        f32 dz = i_this->mSummonPos.z - i_this->mRodTipPos.z;
        f32 dx = i_this->mSummonPos.x - i_this->mRodTipPos.x;
        f32 dy = (i_this->mSummonPos.y + 300.0f) - i_this->mRodTipPos.y;
        gabi::Local<cXyz> scale;
        angle->x = (s16)-cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz)));
        angle->y = cM_atan2s(dx, dz);
        scale->x = 1.0f;
        scale->z = 1.0f;
        scale->y = 1.0f;
        if (eye != nullptr) {
            mDoAud_seStart(wz::JA_SE_CM_WZ_ST_SUMMONBALL, eye, 0,
                           dComIfGp_getReverb(i_this->current.roomNo));
        }
        s8 room = i_this->current.roomNo;
        if (fopAcM_createChild(wz::fpcNm_WZ_e, fopAcM_GetID(i_this), 0xFFFFFF0B,
                               &i_this->mRodTipPos, room, angle.get(), scale.get(), 0,
                               0) != fpcM_ERROR_PROCESS_ID_e) {
            i_this->m470 = 1;
        }
    }
}

// Gameplay, creation and compiler helpers.

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void *wzRes(const char *arc, s32 index) {
  return dComIfG_getObjectRes(arc, index, SAFESTRING_VTBL);
}
/* 02041570 enemy_fire(enemyfire*) */
static inline void enemy_fire_remove(void *f) { wz_removeFire(f); }
static inline void mDoAud_subBgmStop() { wz_stopSubBgm(); }
/* mDoExt_McaMorf::mDoExt_McaMorf (HD: this first; NULL allocates) */
static inline u32 new_McaMorf(void *data, void *anm, s32 loop, f32 morf, s32 i1,
                              s32 i2, s32 i3, u32 modelFlag, u32 dlFlag) {
  return wz_newMcaMorf((void *)nullptr, data, (void *)nullptr, (void *)nullptr,
                       anm, loop, morf, i1, i2, i3, (void *)nullptr, modelFlag,
                       dlFlag);
}

static inline u16 modelData_jointNum(u32 data) {
  return (u16)gabi::at<wz::JointTree>(wz_getJointTree(gabi::at<void>(data)))
      ->mJointNum;
}
static inline u32 morf_model(u32 morf) {
  return gabi::ea(wz::morf(morf)->getModel());
}
/* J3DModel: user area +0xB8, model data +0xAC, matrix buffer +0x2C (flags u16
 * +4, anm matrices +0x10) */

// HD: copies stay one guest word at a time, including unaligned/wrapping
// scratch addresses.
static inline void cpy_u32(u32 dst, u32 src) {
  *gabi::at<be<u32>>(dst) = *gabi::at<be<u32>>(src);
}
static inline s32 dComIfGp_CharTbl_GetNameIndex_wz(const char *name) {
  return wz_getActorNameIndex(gabi::at<u8>(dComIfGp_ea() + 0x50AC), name, 0);
}

/* ---- joint callbacks ---- */
// HD: the two callbacks share the same dirty-flag and matrix-copy sequence.
/* inlined into nodeCallBack */
/* inlined into rod_nodeCallBack */
static BOOL wz_jointCallBack(J3DNode *node, s32 timing, u16 jointNo,
                             cXyz wz_class::*output) {
  if (timing == J3DNodeCBCalcTiming_In) {
    J3DJoint *joint = J3DNode_toJoint(node);
    J3DModel *model = j3dSys.getModel();
    wz_class *i_this = static_cast<wz_class *>(model->getUserArea());
    // HD: J3DNode_toJoint has already called the out-of-line joint accessor.
    u16 no = wz::view<wz::JointData>(joint)->mJntNo;
    if (i_this != nullptr && no == jointNo) {
      J3DMtxBuffer_hd *buffer = model->mpMtxBuffer;
      buffer->mFlags |= 0x10;
      PSMTXCopy(gabi::at<Mtx34>(buffer->mpAnmMtx + jointNo * sizeof(Mtx34)),
                calc_mtx());
      gabi::Local<cXyz> zero;
      zero->x = 0.0f;
      zero->y = 0.0f;
      zero->z = 0.0f;
      MtxPosition(zero.get(), &(i_this->*output));
      buffer = model->mpMtxBuffer;
      Mtx34 *calc = calc_mtx();
      buffer->mFlags |= 0x10;
      mtx_copy(gabi::at<Mtx34>(buffer->mpAnmMtx + jointNo * sizeof(Mtx34)),
               calc);
      PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
    }
  }
  return TRUE;
}

/* a model's base matrix := joint `jnt` of another model; the destination model
 * and the source matrix base are read before the dirty flag (u16) is stored */
/* inlined into draw_SUB */
static void wz_jointMtxCopy(gptr<mDoExt_McaMorf> &srcMorf, u32 jointNo,
                            gptr<mDoExt_McaMorf> &dstMorf) {
  J3DMtxBuffer_hd *buffer = srcMorf->getModel()->mpMtxBuffer;
  J3DModel *dst = dstMorf->getModel();
  u16 flags = buffer->mFlags;
  u32 matrices = buffer->mpAnmMtx;
  buffer->mFlags = flags | 0x10;
  mtx_copy(&dst->mBaseTransformMtx,
           gabi::at<Mtx34>(matrices + jointNo * sizeof(Mtx34)));
}

/* 024E3950 */
void next_tama_move(wz_class *i_this, const cXyz *pos) {
  *wz::view<be<u32>>(&i_this->m314.x) = *wz::view<const be<u32>>(&pos->x);
  *wz::view<be<u32>>(&i_this->m314.y) = *wz::view<const be<u32>>(&pos->y);
  i_this->speedF = 0.0f;
  i_this->mMode = 0x68;
  // HD: this path clears the sphere header word.
  *wz::view<be<u32>>(&i_this->m7D8) &= ~1u;
  *wz::view<be<u32>>(&i_this->m314.z) = *wz::view<const be<u32>>(&pos->z);
}

/* 024E61D8 */
static BOOL daWZ_IsDelete(wz_class *) { return TRUE; }

/* 024E78C0 */
void fuwafuwa_calc(wz_class *i_this) {
  s16 a = (s16)(i_this->mFuwaAngle + 0x7D0);
  f32 base = i_this->mBaseY + 15.0f;
  i_this->mFuwaAngle = a;
  i_this->current.pos.y = gabi::fmadds(cM_ssin(a), 5.0f, base);
}

/* ---- delete / heap ---- */

/* HD strcmp(dComIfGp_getStartStageName(), name) == 0: SafeString compare (as
 * d_a_bk) */
static inline void wz_SafeString_vcall(SafeString *s) {
  gabi::call_ptr(
      (u32)gabi::at<wz::SafeStringVirtuals>(s->__vtbl)->mAssureTermination, s);
}
static bool wz_isStartStage(const char *name) {
  gabi::Local<SafeString> a;
  a->mStringTop = gabi::guest_addr(name);
  a->__vtbl = SAFESTRING_VTBL;
  u32 stage = gabi::ea(dComIfGp_getStartStageName());
  gabi::Local<SafeString> b;
  b->__vtbl = SAFESTRING_VTBL;
  b->mStringTop = stage;
  wz_SafeString_vcall(a);
  wz_SafeString_vcall(a);
  u32 pa = a->mStringTop;
  wz_SafeString_vcall(b);
  u32 pb = b->mStringTop;
  if (pa == pb) {
    return true;
  }
  for (u32 i = 0; i < 0x40001; i++) {
    u8 ca = (u8)gabi::at<be<u8>>(pa)[i];
    u8 cb = (u8)gabi::at<be<u8>>(pb)[i];
    if (ca != cb) {
      return false;
    }
    if (ca == 0) {
      return true;
    }
  }
  return false;
}

/* 024E61E0 */
static BOOL daWZ_Delete(wz_class *i_this) {
  if (i_this->health == -0x80) { /* killed */
    if (i_this->mIsMiniBoss) {
      mDoAud_subBgmStop();
    }
    if (!wz_isStartStage("kazeMB" /* "kazeMB" */) &&
        i_this->mDisableSpawnOnDeathSwitch != 0xFF) {
      dComIfGs_onSwitch(i_this->mDisableSpawnOnDeathSwitch,
                        i_this->current.roomNo);
    }
  }
  u8 type = i_this->mBehaviorType;
  if (type == 12) {
    /* a summoned Wizzrobe: tell the summoner */
    fopAc_ac_c *parent = fopAcM_SearchByID(i_this->mParentId);
    if (parent != nullptr) {
      static_cast<wz_class *>(parent)->m470 = 0;
    }
    dComIfG_resDelete((request_of_phase_process_class *)i_this->mPhase,
                      "Wzb" /* "Wzb" */);
  } else if (type == 13) {
    dComIfG_resDelete((request_of_phase_process_class *)i_this->mPhase,
                      "Wzb" /* "Wzb" */);
  } else {
    dComIfG_resDelete((request_of_phase_process_class *)i_this->mPhase,
                      "Wz" /* "Wz" */);
  }
  type = i_this->mBehaviorType;
  if (type == 10 || type == 11) {
    dKy_plight_cut((LIGHT_INFLUENCE *)(&i_this->mLight));
  }
  for (int i = 0; i < 5; i++) {
    i_this->mFollowCb[i].remove();
  }
  if (i_this->mBehaviorType < wz::FIRE_BALL) {
    enemy_fire_remove(i_this->mEnemyFire);
  }
  return TRUE;
}

/* 024E6420 */
static BOOL useHeapInit(fopAc_ac_c *a_this) {
  wz_class *i_this = (wz_class *)a_this;
  void *bmd = wzRes("Wz", 0x18);
  void *bck = wzRes("Wz", 0x13);
  i_this->m2B4.v =
      new_McaMorf(bmd, bck, 2, 1.0f, 0, -1, 1, 0x80000, 0x37441422);
  if (i_this->m2B4.v == 0 || morf_model(i_this->m2B4.v) == 0) {
    return FALSE;
  }
  gabi::at<J3DModel>(morf_model(i_this->m2B4.v))->mUserArea = gabi::ea(i_this);
  for (u16 i = 0;
       i < modelData_jointNum(gabi::ea(
               gabi::at<J3DModel>(morf_model(i_this->m2B4.v))->getModelData()));
       i = (u16)(i + 1)) {
    u32 data = gabi::ea(
        gabi::at<J3DModel>(morf_model(i_this->m2B4.v))->getModelData());
    u32 num = (u32)gabi::at<wz::JointArray>(data)->mCount;
    u32 jnt = gabi::at<wz::JointArray>(data)->mpEntries.v;
    if (i < num) {
      jnt += i * 0x1C;
    }
    gabi::at<wz::JointSlot>(jnt)->mCallback =
        gabi::guest_fn((const void *)nodeCallBack);
  }
  if (i_this->mBehaviorType > wz::FIRE_BALL) {
    return TRUE;
  }
  if (i_this->mIsMiniBoss) {
    void *bmd2 = wzRes("Wz", 0x19);
    i_this->mpMorf2.v =
        new_McaMorf(bmd2, nullptr, 0, 0.0f, 0, -1, 1, 0x80000, 0x37441422);
    if (i_this->mpMorf2.v == 0 || morf_model(i_this->mpMorf2.v) == 0) {
      return FALSE;
    }
    if (!wz_createInvisibleModel(
            i_this->mInvisModel2,
            gabi::at<void>(morf_model(i_this->mpMorf2.v)))) {
      return FALSE;
    }
  }
  void *rod = wzRes("Wz", 0x1A);
  i_this->mpRodMorf.v =
      new_McaMorf(rod, nullptr, 2, 0.0f, 0, -1, 1, 0, 0x11020203);
  if (i_this->mpRodMorf.v == 0 || morf_model(i_this->mpRodMorf.v) == 0) {
    return FALSE;
  }
  gabi::at<J3DModel>(morf_model(i_this->mpRodMorf.v))->mUserArea =
      gabi::ea(i_this);
  for (u16 i = 0; i < modelData_jointNum(gabi::ea(
                          gabi::at<J3DModel>(morf_model(i_this->mpRodMorf.v))
                              ->getModelData()));
       i = (u16)(i + 1)) {
    u32 data = gabi::ea(
        gabi::at<J3DModel>(morf_model(i_this->mpRodMorf.v))->getModelData());
    u32 num = (u32)gabi::at<wz::JointArray>(data)->mCount;
    u32 jnt = gabi::at<wz::JointArray>(data)->mpEntries.v;
    if (i < num) {
      jnt += i * 0x1C;
    }
    gabi::at<wz::JointSlot>(jnt)->mCallback =
        gabi::guest_fn((const void *)rod_nodeCallBack);
  }
  u32 model = morf_model(i_this->m2B4.v);
  void *brk = operator_new(0x78);
  if (brk != nullptr) {
    brk = wz_constructBrk(brk); /* mDoExt_brkAnm */
  }
  i_this->mpBrkAnm.v = gabi::ea(brk);
  if (brk == nullptr) {
    return FALSE;
  }
  void *res = wzRes("Wz", 0x1D);
  if (!wz_initBrk(
          gabi::at<void>(i_this->mpBrkAnm.v),
          gabi::at<void>(gabi::ea(gabi::at<J3DModel>(model)->getModelData())),
          res, 1, 0, 1.0f, 0, -1, 0, 0)) {
    return FALSE;
  }
  if (!wz_createInvisibleModel(i_this->mInvisModel,
                               gabi::at<void>(morf_model(i_this->m2B4.v)))) {
    return FALSE;
  }
  return TRUE;
}

/* 024E67FC */
static BOOL useHeapInit2(fopAc_ac_c *a_this) {
  wz_class *i_this = (wz_class *)a_this;
  void *bmd = wzRes("Wzb", 8);
  void *bck = wzRes("Wzb", 5);
  i_this->m2B4.v =
      new_McaMorf(bmd, bck, 0, 1.0f, 0, -1, 1, 0x80000, 0x19000222);
  if (i_this->m2B4.v == 0 || morf_model(i_this->m2B4.v) == 0) {
    return FALSE;
  }
  u32 model = morf_model(i_this->m2B4.v);
  void *btk = operator_new(0x74);
  if (btk != nullptr) {
    btk = wz_constructBtk(btk); /* mDoExt_btkAnm */
  }
  i_this->mpBtkAnm.v = gabi::ea(btk);
  if (btk == nullptr) {
    return FALSE;
  }
  void *res = wzRes("Wzb", 0xB);
  if (!wz_initBtk(
          gabi::at<void>(i_this->mpBtkAnm.v),
          gabi::at<void>(gabi::ea(gabi::at<J3DModel>(model)->getModelData())),
          res, 1, 0, 1.0f, 0, -1, 0, 0)) {
    return FALSE;
  }
  return TRUE;
}

/* ---- draw ---- */

// HD: propagate alpha through the TEV virtuals and the float material colour
// block.
/* inlined into daWZ_Draw */
static void wz_setMaterialAlpha(wz_class *i_this, J3DModelData *data) {
  for (u16 i = 0; i < wz_getMaterialTable(data)->mMaterialNum;
       i = (u16)(i + 1)) {
    wz::Material *material =
        wz::view<wz::Material>(data->getMaterialNodePointer(i));
    GXColor *color = material->mTevBlock->getTevKColor(3);
    color->a = (u8)i_this->mAlpha;
    color = material->mTevBlock->getTevKColor(3);
    wz::TevBlock *block = material->mTevBlock;
    block->setTevKColor(3, color);
    gabi::Local<wz::ColorF> in;
    gabi::Local<wz::ColorF> conv;
    wz_colorToF(in.get(), color);
    wz_convertColor(conv.get(), in.get(), 1.0f);
    material->mFlags |= 0x400;
    wz::ColorF *out = wz_getMaterialColor(&material->mFlags, 10);
    f32 alpha = (f32)(u8)color->a / 255.0f;
    // HD: the lfs/stfs copies quiet signalling NaNs; store order is observable.
    f32 r = conv->r, g = conv->g, b = conv->b;
    out->g = g;
    out->b = b;
    out->r = r;
    out->a = alpha;
  }
}

/* ---- attacks ---- */

/* ---- damage ---- */

/* CcAtInfo (GameCube 0x1C): the hit object at +0, pParticlePos at +0x14 */

static inline BOOL dCcD_GObjInf_ChkTgHit(void *o) {
  return wz_checkTargetHit(o);
}
static inline void dCcD_GObjInf_ClrTgHit(void *o) { wz_clearTargetHit(o); }
/* fopAcM_seStart with the HD actor/eyePos null checks; `checkActor` false where
 * the WWHD code only tests the eyePos address */
static inline void wz_se(wz_class *a, u32 id, u32 param, bool checkActor) {
  if ((!checkActor || gabi::ea(a) != 0) && gabi::ea(&a->eyePos) != 0) {
    mDoAud_seStart(id, &a->eyePos, param,
                   dComIfGp_getReverb(a->current.roomNo));
  }
}
/* the hit sound pair of the stun hits */
static inline void wz_stunSe(wz_class *a, bool checkActor) {
  if ((!checkActor || gabi::ea(a) != 0) && gabi::ea(&a->eyePos) != 0) {
    mDoAud_seStart(0x2833, &a->eyePos, 0x44,
                   dComIfGp_getReverb(a->current.roomNo));
    s8 room = a->current.roomNo;
    u32 id = fopAcM_GetID(a);
    mDoAud_monsSeStart(0x48D8, &a->eyePos, id, 0, dComIfGp_getReverb(room));
  }
}
static inline void wz_followEnd(wz_class *a, int i) {
  dPa_followEcallBack_end(&a->mFollowCb[i]);
}

/* ---- the summoner (types 12/13) ---- */

static inline void wz_morfPlay(wz_class *i_this) {
  wz_playMorf(gabi::at<void>(i_this->m2B4.v), (void *)nullptr, 0, 0);
}
/* the summon smoke: the emitter's two scale vectors follow the actor scale */
static void wz_summonSmoke(wz_class *i_this, u16 id) {
  JPABaseEmitter *e = dPa_control_set(
      dComIfGp_getParticle(), 0, id, &i_this->current.pos, nullptr, nullptr,
      0xFF, nullptr, -1, nullptr, nullptr, nullptr);
  f32 x = i_this->scale.x, y = i_this->scale.y, z = i_this->scale.z;
  if (e != nullptr) {
    u32 p = gabi::ea(e);
    gabi::at<wz::Emitter>(p)->mGlobalDynamicsScale.x = x;
    gabi::at<wz::Emitter>(p)->mGlobalDynamicsScale.y = y;
    gabi::at<wz::Emitter>(p)->mGlobalDynamicsScale.z = z;
    gabi::at<wz::Emitter>(p)->mGlobalParticleScale.x = x;
    gabi::at<wz::Emitter>(p)->mGlobalParticleScale.y = y;
    gabi::at<wz::Emitter>(p)->mGlobalParticleScale.z = z;
  }
}
/* one summon group: create up to `num` enemies in the free child slots */
template <class Prm>
static void wz_summonGroup(wz_class *i_this, s32 num, s32 row, cXyz *pos,
                           csXyz *angle, Prm prm) {
  s32 n = 0;
  if (n >= num) {
    return;
  }
  for (int i = 0;;) {
    if ((s32)i_this->mChildId[i] == -1) {
      s16 name;
      u32 param = prm(&name);
      i_this->mChildId[i] = fopAcM_create(
          name, param, pos, i_this->current.roomNo, angle, nullptr, -1, 0);
      if ((s32)i_this->mChildId[i] != -1) {
        i_this->mChildFlag[i] = 1;
        f32 x = i_this->current.pos.x;
        pos->x = x;
        pos->y = i_this->current.pos.y;
        pos->z = i_this->current.pos.z;
        n++;
        pos->x = x + cM_rndFX(100.0f);
        pos->y = pos->y + cM_rndFX(100.0f);
        pos->z = pos->z + cM_rndFX(100.0f);
      }
    }
    i++;
    if (i >= 20 || n >= num) {
      break;
    }
  }
}

/* 024E9384 */
void action_summon_dousa(wz_class *i_this) {
  dComIfGp_get();
  u8 cnt = 0;
  u8 st = i_this->mMode;
  if (st < 200) {
  } else if (st == 200) {
    /* open the door */
    for (int i = 0; i < 4; i++) {
      wz::state(i_this)->mDemoSteps[i] = 0;
    }
    for (int i = 5; i != 0; i--) {
      wz_summonSmoke(i_this, 0x8283);
    }
    if (gabi::ea(&i_this->eyePos) != 0) {
      mDoAud_seStart(0x69E0, &i_this->eyePos, 0,
                     dComIfGp_getReverb(i_this->current.roomNo));
    }
    wz_summonSmoke(i_this, 0x8284);
    wz::state(i_this)->mTimers[0] = 0x28;
    for (int i = 0; i < 20; i++) {
      i_this->mChildId[i] = 0xFFFFFFFF;
      i_this->mChildFlag[i] = 0;
    }
    i_this->mMode = (u8)(i_this->mMode + 1);
    wz_morfPlay(i_this);
    mDoExt_baseAnm_play(gabi::at<void>(i_this->mpBtkAnm.v));
    return;
  } else if (st < 202) {
    /* summon the enemies of the mini-boss's current row */
    if (i_this->mBehaviorType == wz::SUMMON_DOOR) {
      wz_class *parent = (wz_class *)fopAcM_SearchByID(i_this->mParentId);
      if (parent != nullptr && wz::state(i_this)->mTimers[0] == 1) {
        s32 row = parent->mSummonWave * 8 + parent->mSummonSlot * 2;
        s32 num = (s32)l_summonNum[row];
        gabi::Local<cXyz> pos;
        gabi::Local<csXyz> angle;
        angle->z = 0;
        angle->x = 0;
        angle->y = 0;
        angle->y = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        pos->copy(i_this->current.pos);
        if (row == 0xBF) {
          angle->x = 0x80;
        }
        wz_summonGroup(i_this, num, row, pos, angle, [&](s16 *name) {
          *name = (s16)l_summonName[row];
          u32 param = (u32)l_summonParam[row];
          if (*name == 0xCE && (param & 0xA00) == 0) {
            s32 r = gabi::ftoi(cM_rndF(2.99f));
            param |= ((u32)l_summonColor[r] << 8) + 0xA00;
            *name = (s16)l_summonName[row];
          }
          return param;
        });
        s32 slot = parent->mSummonSlot;
        s32 wave = parent->mSummonWave;
        parent->mSummonSlot = (slot + 1) & 3;
        if (wave == 7 || wave == 13) {
          s16 c = (s16)(parent->mSummonCount + 1);
          parent->mSummonCount = c;
          if (c > 7) {
            parent->mSummonWave = 10;
          }
        }
        row++;
        if ((s16)l_summonName[row] != 0x7FFF) {
          /* the row's second group */
          angle->x = 0;
          angle->y = 0;
          num = (s32)l_summonNum[row];
          angle->z = 0;
          angle->y = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
          pos->copy(i_this->current.pos);
          wz_summonGroup(i_this, num, row, pos, angle, [&](s16 *name) {
            u32 param = (u32)l_summonParam[row];
            if (row == 0xCE) {
              param |= 0xA00;
              s32 r = gabi::ftoi(cM_rndF(3.19f));
              param |= (u32)l_summonColor[r] << 8;
            }
            *name = (s16)l_summonName[row];
            return param;
          });
        }
      }
    }
    mDoExt_baseAnm *btk = gabi::at<mDoExt_baseAnm>(i_this->mpBtkAnm.v);
    if (btk->mFrameCtrl.checkState(J3DFrameCtrl::STATE_STOP_E) ||
        btk->mFrameCtrl.getRate() == 0.0f) {
      i_this->mMode = (u8)(i_this->mMode + 1);
      if (i_this->mBehaviorType == wz::INTRO_DOOR) {
        fopAcM_delete(i_this);
      }
    }
  } else if (st == 202) {
    /* wait until every summoned enemy is gone or out of the arena */
    for (int i = 0; i < 20; i++) {
      if (i_this->mChildFlag[i] == 0) {
        cnt = (u8)(cnt + 1);
        continue;
      }
      fopAc_ac_c *child = fopAcM_SearchByID(i_this->mChildId[i]);
      bool inStage = false;
      if (child != nullptr) {
        inStage = wz_isStartStage("kazeMB");
      }
      bool keep;
      if (inStage) {
        f32 x = child->current.pos.x, z = child->current.pos.z;
        f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
        keep = false;
        if (!(d > 1800.0f)) {
          f32 y = child->current.pos.y;
          if (!(y < -200.0f) && !(y > 2100.0f)) {
            keep = true;
          }
        }
      } else {
        keep = child != nullptr;
      }
      if (!keep) {
        i_this->mChildFlag[i] = 0;
        i_this->mChildId[i] = 0xFFFFFFFF;
      }
    }
    if (cnt == 20) {
      fopAc_ac_c *parent = fopAcM_SearchByID(i_this->mParentId);
      if (parent != nullptr) {
        static_cast<wz_class *>(parent)->m470 = 0;
      }
      fopAcM_delete(i_this);
    }
  }
  wz_morfPlay(i_this);
  mDoExt_baseAnm_play(gabi::at<void>(i_this->mpBtkAnm.v));
}

/* ---- create ---- */

static u32 ppcSlw(u32 x, u32 n) {
  n &= 63;
  return n < 32 ? x << n : 0;
}
static u32 ppcSrw(u32 x, u32 n) {
  n &= 63;
  return n < 32 ? x >> n : 0;
}

/* a summoned Wizzrobe takes the summon data of the parent */
static void wz_copySummonData(wz_class *i_this, wz_class *p) {
  cpy_u32(gabi::ea(&i_this->mSummonPos.x), gabi::ea(&p->mSummonPos.x));
  cpy_u32(gabi::ea(&i_this->mSummonPos.y), gabi::ea(&p->mSummonPos.y));
  cpy_u32(gabi::ea(&i_this->mSummonPos.z), gabi::ea(&p->mSummonPos.z));
  cpy_u32(gabi::ea(&wz::state(i_this)->mPathPoint),
          gabi::ea(&wz::state(p)->mPathPoint));
  wz::state(i_this)->mHasActiveSummon = (u8)p->m470;
  i_this->mSummonSlot = (s32)p->mSummonSlot;
  i_this->mSummonWave = (s32)p->mSummonWave;
}
/* the summoner's frame: the HD texture swap keeps a ResTIMG at entry SP - 0xA4
 * whose relative offsets record its own address, so the frame is laid out as
 * the original's (0xB0 bytes) */

/* HD: the summon door's "__dummy" texture is replaced by a system texture
 * (027F81A4(.., 6)) */
static void wz_swapDummyTexture(wzCreateFrame *f, u32 tex, u32 names) {
  for (u16 i = 0; i < (u16)gabi::at<wz::TextureTable>(tex)->mCount;) {
    u32 name = wz_getTextureName(gabi::at<void>(names), i);
    if (name == 0) {
      JUT_ASSERT_fail("d_a_wz.cpp" /* "d_a_wz.cpp" */, 0x1086,
                      "name != (0)" /* "name != (0)" */);
      i = (u16)(i + 1);
      continue;
    }
    u32 p = name, q = gabi::guest_addr("__dummy"); /* "__dummy" */
    u8 a, b;
    do {
      a = (u8)*gabi::at<be<u8>>(p++);
      b = (u8)*gabi::at<be<u8>>(q++);
    } while (a == b && a != 0);
    if (a == b) {
      u32 image = wz_getSystemTexture(gabi::at<void>((u32)l_systemTextures), 6);
      u32 obj = wz_constructTexture(gabi::at<void>(wz_allocateTexture(0xC0)),
                                    gabi::at<void>(image));
      u32 local = gabi::ea(f) + 0xC;
      f->texObj = obj;
      gabi::at<wz::TextureImage>(local)->mWidth =
          (u16)gabi::at<wz::NativeTexture>(obj)->mWidth;
      gabi::at<wz::TextureImage>(local)->mPaletteFormat = 0;
      gabi::at<wz::TextureImage>(local)->mHeight =
          (u16)gabi::at<wz::NativeTexture>(obj)->mHeight;
      u32 off = i * 0x24;
      u32 dest = gabi::at<wz::TextureTable>(tex)->mpImages.v + off;
      for (int j = 0; j < 3; j++) {
        cpy_u32(dest + j * 12, local + j * 12);
        cpy_u32(dest + j * 12 + 4, local + j * 12 + 4);
        cpy_u32(dest + j * 12 + 8, local + j * 12 + 8);
      }
      dest = gabi::at<wz::TextureTable>(tex)->mpImages.v + off;
      gabi::at<wz::TextureImage>(dest)->mImageOffset =
          (u32)gabi::at<wz::TextureImage>(dest)->mImageOffset + local - dest;
      dest = gabi::at<wz::TextureTable>(tex)->mpImages.v + off;
      gabi::at<wz::TextureImage>(dest)->mPaletteOffset =
          (u32)gabi::at<wz::TextureImage>(dest)->mPaletteOffset + local - dest;
      dest = gabi::at<wz::TextureTable>(tex)->mpImages.v + off;
      gabi::at<wz::TextureImage>(dest)->mpTexture = (u32)f->texObj;
      u32 hi = (u32)gabi::at<wz::TextureTable>(tex)->mShiftHi,
          lo = (u32)gabi::at<wz::TextureTable>(tex)->mShiftLo;
      if (i < 0x40) {
        u32 upper = ppcSlw(lo, i + 32) | (ppcSlw(hi, i) | ppcSrw(lo, 32 - i));
        u32 lower = ppcSlw(lo, i);
        u32 a8 = (u32)gabi::at<wz::TextureTable>(tex)->mValidHi,
            aC = (u32)gabi::at<wz::TextureTable>(tex)->mValidLo;
        i = (u16)(i + 1);
        gabi::at<wz::TextureTable>(tex)->mValidHi = a8 | upper;
        gabi::at<wz::TextureTable>(tex)->mValidLo = aC | lower;
        continue;
      }
      u32 n = i - 0x40;
      u32 a10 = (u32)gabi::at<wz::TextureTable>(tex)->mOverflowHi;
      u32 upper = ppcSlw(lo, n + 32) | (ppcSlw(hi, n) | ppcSrw(lo, 32 - n));
      u32 a14 = (u32)gabi::at<wz::TextureTable>(tex)->mOverflowLo;
      gabi::at<wz::TextureTable>(tex)->mOverflowHi = a10 | upper;
      gabi::at<wz::TextureTable>(tex)->mOverflowLo = a14 | ppcSlw(lo, n);
    }
    i = (u16)(i + 1);
  }
}

/* 024E6BEC */
static cPhs_State daWZ_Create(fopAc_ac_c *a_this) {
  gabi::Local<wzCreateFrame> frame;
  wz_class *i_this = (wz_class *)a_this;
  u32 cond = a_this->actor_condition;
  if (!(cond & 8)) {
    if (a_this != nullptr) {
      wz_class_ct(a_this);
      cond = a_this->actor_condition;
    }
    a_this->actor_condition = cond | 8;
  }
  u32 prm = a_this->mParameters;
  s16 az = a_this->current.angle.z;
  i_this->mBehaviorType = (u8)prm;
  i_this->mDisableSpawnOnDeathSwitch = (u8)(prm >> 8);
  i_this->mEnableSpawnSwitch = (u8)(prm >> 16);
  wz::state(i_this)->mPathIndex = (u8)(prm >> 24); /* path */
  i_this->mEnemySummonTableIndex = (u8)az;
  cPhs_State phase;
  if ((u8)prm == 12 || (u8)prm == 13) {
    phase = dComIfG_resLoad((request_of_phase_process_class *)i_this->mPhase,
                            "Wzb" /* "Wzb" */);
  } else {
    phase = dComIfG_resLoad((request_of_phase_process_class *)i_this->mPhase,
                            "Wz" /* "Wz" */);
  }
  u8 type = i_this->mBehaviorType;
  if (type == 0xFF) {
    i_this->mBehaviorType = 0;
  } else {
    if (type == 2) {
      i_this->mBehaviorType = 1;
      type = 1;
      i_this->mIsMiniBoss = 1;
    }
    if (type == 3) {
      i_this->mBehaviorType = 0;
      i_this->m351 = 1;
    }
  }
  if (i_this->mEnemySummonTableIndex == 0xFF) {
    i_this->mEnemySummonTableIndex = 0;
  }

  if (phase != cPhs_COMPLEATE_e) {
    if (i_this->mBehaviorType == wz::SUMMON_DOOR &&
        (u8)wz::state(i_this)->mSummonParentResolved == 0) {
      u32 id = a_this->parentActorID;
      i_this->mParentId = id;
      if (id == 0xFFFFFFFF) {
        return cPhs_ERROR_e;
      }
      fopAc_ac_c *p = fopAcM_SearchByID(id);
      if (p == nullptr) {
        return phase;
      }
      u32 pid = (u32) static_cast<wz_class *>(p)->mParentId;
      wz::state(i_this)->mSummonParentResolved = 1;
      i_this->mParentId = pid;
    }
    if (phase == cPhs_ERROR_e && i_this->mBehaviorType == wz::SUMMON_DOOR) {
      u32 id = a_this->parentActorID;
      i_this->mParentId = id;
      if (id == 0xFFFFFFFF) {
        return cPhs_ERROR_e;
      }
      fopAc_ac_c *p = fopAcM_SearchByID(id);
      if (p != nullptr) {
        static_cast<wz_class *>(p)->m470 = 0;
      }
    }
    return phase;
  }

  type = i_this->mBehaviorType;
  // HD: an already-resolved summon parent bypasses the ordinary spawn-switch
  // check.
  auto selectHeap = [&]() -> s32 {
    bool checkSw = type < 10;
    if (type > 10) {
      type = i_this->mBehaviorType;
      i_this->m7D8.mAtType = 0x400; /* ball sphere */
      wz_class *p;
      if (type == 12) {
        if ((u8)wz::state(i_this)->mSummonParentResolved != 0) {
          return 1;
        }
        u32 id = a_this->parentActorID;
        i_this->mParentId = id;
        if (id == 0xFFFFFFFF) {
          return -1;
        }
        p = (wz_class *)fopAcM_SearchByID(id);
        if (p == nullptr) {
          return -1;
        }
        i_this->mParentId = p->mParentId;
        if (fopAcM_SearchByID(i_this->mParentId) == nullptr) {
          return -1;
        }
        wz::state(i_this)->mSummonParentResolved = 1;
      } else {
        u32 id = a_this->parentActorID;
        i_this->mParentId = id;
        if (id == 0xFFFFFFFF) {
          return -1;
        }
        p = (wz_class *)fopAcM_SearchByID(id);
        if (p == nullptr) {
          return -1;
        }
      }
      wz_copySummonData(i_this, p);
      type = i_this->mBehaviorType;
      checkSw = type < 10;
    }
    if (checkSw) {
      if (i_this->mDisableSpawnOnDeathSwitch == 0xFF) {
        return 0;
      }
      if (dComIfGs_isSwitch(i_this->mDisableSpawnOnDeathSwitch, l_stayRoom)) {
        return -1;
      }
      type = i_this->mBehaviorType;
    }
    if (type == 12 || type == 13) {
      return 1;
    }
    return 0;
  };
  s32 heapKind = selectHeap();
  if (heapKind < 0) {
    return cPhs_ERROR_e;
  }
  if (heapKind == 0) {

    if (!fopAcM_entrySolidHeap(a_this, 0x024E6420 /* useHeapInit */,
                               type < 10 ? 0x3440 : 0x3640)) {
      return cPhs_ERROR_e;
    }
  } else {
    if (!fopAcM_entrySolidHeap(a_this, 0x024E67FC /* useHeapInit2 */, 0xCC0)) {
      fopAc_ac_c *p = fopAcM_SearchByID(i_this->mParentId);
      if (p != nullptr) {
        static_cast<wz_class *>(p)->m470 = 0;
      }
      return cPhs_ERROR_e;
    }
    u32 morf = i_this->m2B4.v;
    if (i_this->mBehaviorType == wz::INTRO_DOOR) {
      a_this->actor_status |= 0x4000;
    }
    u32 data = gabi::ea(gabi::at<J3DModel>(morf_model(morf))->getModelData());
    u32 tex = (u32)gabi::at<wz::ModelTextureData>(data)->mpTextures;
    u32 names = 0;
    if (tex != 0) {
      names = (u32)gabi::at<wz::ModelTextureData>(data)->mpTextureNames;
    }
    if (tex == 0 || names == 0) {
      fopAc_ac_c *p = fopAcM_SearchByID(i_this->mParentId);
      if (p != nullptr) {
        static_cast<wz_class *>(p)->m470 = 0;
      }
      return cPhs_ERROR_e;
    }
    wz_swapDummyTexture(frame, tex, names);
  }

  a_this->current.angle.z = 0;
  u32 morf = i_this->m2B4.v;
  a_this->shape_angle.z = 0;
  i_this->attention_info.flags = 0;
  u32 model = morf_model(morf);
  a_this->cullMtx = gabi::ea(gabi::at<J3DModel>(model)->getBaseTRMtx());
  fopAcM_setCullSizeBox(a_this, -100.0f, -50.0f, -50.0f, 100.0f, 200.0f,
                        100.0f);
  if (i_this->mBehaviorType < wz::FIRE_BALL) {
    u8 t = i_this->mBehaviorType;
    a_this->stealItemLeft = 3;
    a_this->max_health = 4;
    a_this->health = 4;
    i_this->attention_info.distances[2] = 4; /* attention_info distance */
    if (t < 1) {
      a_this->itemTableIdx =
          dComIfGp_CharTbl_GetNameIndex_wz("wiz_r" /* "wiz_r" */);
    } else if (t == 1) {
      a_this->max_health = 8;
      a_this->health = 8;
      s32 idx = dComIfGp_CharTbl_GetNameIndex_wz("wiz_s" /* "wiz_s" */);
      u8 mini = i_this->mIsMiniBoss;
      u8 tbl = i_this->mEnemySummonTableIndex;
      a_this->itemTableIdx = idx;
      wz::state(i_this)->mHasActiveSummon = 0;
      i_this->mSummonWave = tbl;
      if (mini == 1) {
        a_this->max_health = 12;
        i_this->mSummonWave = 7;
        a_this->actor_status |= 0x4000000;
        a_this->health = 12;
      } else {
        i_this->mSummonSlot = (s16)gabi::ftoi(cM_rndF(3.99f)) & 3;
      }
    }
    if ((s16)l_debug.mInvincible != 0) { /* HD debug: invincible */
      a_this->max_health = 0x7F;
      a_this->health = 0x7F;
    }
    u8 pathNo = (u8)wz::state(i_this)->mPathIndex;
    if (pathNo != 0xFF) {
      u32 path = gabi::ea(dPath_GetRoomPath(pathNo, a_this->current.roomNo));
      wz::state(i_this)->mpPath = path;
      if (path != 0) {
        f32 r = cM_rndF((f32)(u16)gabi::at<wz::Path>(path)->m_num);
        path = (u32)wz::state(i_this)->mpPath;
        u32 idx = (u32)gabi::ftoi(r);
        if (idx == (u16)gabi::at<wz::Path>(path)->m_num) {
          idx--;
        }
        u32 pnt = gabi::at<wz::Path>(path)->m_points.v + idx * 16;
        s8 room = a_this->current.roomNo;
        a_this->current.pos.x = (f32)gabi::at<wz::PathPoint>(pnt)->m_position.x;
        a_this->current.pos.y = (f32)gabi::at<wz::PathPoint>(pnt)->m_position.y;
        a_this->current.pos.z = (f32)gabi::at<wz::PathPoint>(pnt)->m_position.z;
        wz::state(i_this)->mpNextRoomPath =
            wz_getNextRoomPath(gabi::at<void>((u32)wz::state(i_this)->mpPath),
                               room); /* dPath_GetNextRoomPath */
      }
    }
    cpy_u32(gabi::ea(&wz::state(i_this)->mHomeX),
            gabi::ea(&a_this->current.pos.x));
    cpy_u32(gabi::ea(&i_this->mBaseY), gabi::ea(&a_this->current.pos.y));
    cpy_u32(gabi::ea(&wz::state(i_this)->mHomeZ),
            gabi::ea(&a_this->current.pos.z));
    draw_SUB(i_this);
    wz_setCylinder((&i_this->mBodyCyl),
                   gabi::at<void>(0x101D3B04)); /* dCcD_Cyl::Set */
    i_this->mBodyCyl.mStts.v = gabi::ea((&i_this->mStts));
    i_this->mBodyCyl.mObjTg.mSPrm = (u32)i_this->mBodyCyl.mObjTg.mSPrm & ~1u;
    dCcD_GObjInf_ClrTgHit((&i_this->mBodyCyl));
    s16 ang = fopAcM_searchActorAngleY(a_this, dComIfGp_getPlayer(0));
    a_this->current.angle.y = ang;
    wz::state(i_this)->mTargetAngle = ang;
    a_this->shape_angle.y = ang;
    wz::state(i_this)->mIceWallRadius = 80.0f;
    wz::state(i_this)->mIceWallHeight = 50.0f;
    i_this->mAcchWallH = 100.0f;
    wz::view<wz::EnemyFire>(i_this->mEnemyFire)->mpMorf =
        i_this->m2B4; /* enemyfire: morf */
    wz::view<wz::IcePrefix>(i_this->_A40)->mpActor = i_this;
    i_this->mAcchWallR = 110.0f;
    wz::view<wz::EnemyFire>(i_this->mEnemyFire)->mpActor =
        i_this; /* enemyfire: actor */
    for (int i = 0; i < 10; i++) {
      wz::view<wz::EnemyFire>(i_this->mEnemyFire)->mJointIndices[i] =
          (u8)l_fireJoints[i];
      wz::view<wz::EnemyFire>(i_this->mEnemyFire)->mJointScales[i] =
          (f32)l_fireScales[i];
    }
    i_this->mAlpha = 0;
    i_this->mRodScaleX = 0.0f;
    i_this->mRodScaleY = 0.0f;
    i_this->mRodScaleZ = 0.0f;
    if (wz_isStartStage("kazeMB" /* "kazeMB" */)) {
      i_this->mAction = wz::ACTION_DEMO;
      if (!i_this->mIsMiniBoss) {
        a_this->actor_status |= 0x4000;
        i_this->mMode = 0x3C;
      } else {
        a_this->current.pos.x = 0.0f;
        a_this->current.pos.y = 0.0f;
        a_this->current.pos.z = 0.0f;
        l_demoSignal = 0;
        i_this->mMode = 0x32;
      }
    } else {
      i_this->mAction = wz::ACTION_NORMAL;
      i_this->mMode = 0;
    }
  } else if (i_this->mBehaviorType == wz::SUMMON_DOOR ||
             i_this->mBehaviorType == wz::INTRO_DOOR) {
    i_this->mAction = wz::ACTION_SUMMON;
    i_this->mMode = 200;
  } else {
    i_this->mAcchWallH = 0.0f;
    i_this->mAcchWallR = 40.0f;
    wz_setSphere((&i_this->m7D8),
                 gabi::at<void>(0x101D3AB8)); /* dCcD_Sph::Set */
    i_this->m7D8.mStts.v = gabi::ea((&i_this->mStts));
    i_this->mAction = wz::ACTION_BALL;
    wz::state(i_this)->mTimers[2] = 300;
    i_this->mMode = 100;
  }
  type = i_this->mBehaviorType;
  if (type == 12 || type == 13) {
    return phase;
  }
  wz_setAcch((&i_this->mAcch), &a_this->current.pos, &a_this->old.pos, a_this,
             1, (&i_this->mAcchCir), &a_this->speed, (void *)nullptr,
             (void *)nullptr);                        /* dBgS_Acch::Set */
  wz_initCcStatus((&i_this->mStts), 0xFE, 1, a_this); /* dCcD_Stts::Init */
  BG_check(i_this);
  type = i_this->mBehaviorType;
  if (type == 10 || type == 11) {
    dKy_plight_set((LIGHT_INFLUENCE *)(&i_this->mLight));
  }
  return phase;
}

/* ---- mini-boss intro / defeat demo ---- */

/* HD debug parameter block (all zero in the shipped game: offsets added to the
 * demo constants) */
static inline void dCamera_Stop_wz(u32 cam) {
  wz_stopCamera(gabi::at<void>(cam));
}
static inline void dCamera_Start_wz(u32 cam) {
  wz_startCamera(gabi::at<void>(cam));
}
static inline void dCamera_SetTrimSize_wz(u32 cam, s32 s) {
  wz_setCameraTrim(gabi::at<void>(cam), s);
}
static inline void dCamera_Reset_wz(u32 cam, cXyz *center, cXyz *eye) {
  wz_resetCamera(gabi::at<void>(cam), center, eye);
}
static inline void dCamera_Set_wz(u32 cam, cXyz *center, cXyz *eye, f32 fovy,
                                  s16 bank) {
  wz_setCamera(gabi::at<void>(cam), center, eye, fovy, bank);
}
static inline void dComIfGp_StopQuake_wz(s32 a) {
  wz_stopQuake(dComIfGp_getVibration(), a);
}
static inline void mDoAud_bgmAllMute_wz(s32 t) { wz_muteBgm(t); }
static inline void mDoAud_subBgmStart_wz(u32 id) { wz_startSubBgm(id); }
static inline BOOL fopAcM_orderPotentialEvent_wz(fopAc_ac_c *a, u16 type,
                                                 u16 flag, u16 p) {
  return wz_orderPotentialEvent(a, type, flag, p);
}
/* the player's virtual setPlayerPosAndAngle (vtable +0x114); the vtable is read
 * before the angle is computed */
static inline void wz_playerSetPos(fopAc_ac_c *player, u32 vt, cXyz *pos,
                                   s16 ang) {
  gabi::call_ptr((u32)gabi::at<wz::PlayerVirtuals>(vt)->mSetPlayerPosAndAngle,
                 player, pos, ang);
}
/* cLib_addCalc2 toward a target with a step proportional to the distance */
static inline void wz_camCalc(be<f32> *v, f32 target, f32 rateAdd, f32 rate) {
  f32 d = *v - target;
  f32 r = rateAdd + rate;
  cLib_addCalc2(v, target, 1.0f, std::fabs(d) * r);
}
static inline void wz_demoSe(wz_class *i_this, u32 id) {
  if (gabi::ea(&i_this->eyePos) != 0) {
    mDoAud_seStart(id, &i_this->eyePos, 0,
                   dComIfGp_getReverb(i_this->current.roomNo));
  }
}
static inline void wz_demoMonsSe(wz_class *i_this) {
  if (gabi::ea(&i_this->eyePos) != 0) {
    s8 room = i_this->current.roomNo;
    u32 id = fopAcM_GetID(i_this);
    mDoAud_monsSeStart(0x48D6, &i_this->eyePos, id, 0,
                       dComIfGp_getReverb(room));
  }
}
/* 024E7904 */
void action_demo(wz_class *i_this) {
  fopAc_ac_c *player = dComIfGp_getPlayer(0);
  s32 camId = (s8)wz::playCamera()->mPlayerCameraId;
  u32 cam =
      (u32)wz::playCamera()->mCamera[camId].mpCamera + 0x248; /* dCamera_c */
  gabi::Local<cXyz> dpos; /* where the defeat effect appears */
  {
    f32 x = i_this->current.pos.x;
    f32 add = (f32)l_debug.mDefeatEffectHeight + 160.0f;
    dpos->x = x;
    f32 y = i_this->current.pos.y;
    dpos->y = y;
    dpos->y = y + add;
    dpos->z = i_this->current.pos.z;
  }
  u8 st = i_this->mMode;
  switch (st) {
  case 0x32: {
    /* wait for the player */
    f32 d = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
    if (d > (f32)l_debug.mTriggerDistance + 1250.0f) {
      break;
    }
    cpy_u32(gabi::ea(&i_this->eyePos.x), gabi::ea(&player->current.pos.x));
    cpy_u32(gabi::ea(&i_this->eyePos.y), gabi::ea(&player->current.pos.y));
    cpy_u32(gabi::ea(&i_this->eyePos.z), gabi::ea(&player->current.pos.z));
    wz_demoSe(i_this, 0x5909);
    s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
    i_this->current.angle.y = ang;
    i_this->shape_angle.y = ang;
    wz::state(i_this)->mTargetAngle = ang;
    mDoAud_bgmAllMute_wz(0x1E);
    i_this->current.pos.x = 0.0f;
    i_this->mMode = 0x46;
    i_this->mBaseY = 0.0f;
    wz::state(i_this)->mHomeZ = 0.0f;
    i_this->current.pos.y = 0.0f;
    wz::state(i_this)->mHomeX = 0.0f;
    i_this->current.pos.z = 0.0f;
    break;
  }
  case 0x3C: {
    u32 sub = (u32)(s32)(s16)wz::state(i_this)->mDemoSteps[0];
    if (sub < 1) {
      if ((u8)l_demoSignal == 0) {
        break;
      }
      anm_init(i_this, 0x10, 5.0f, 2, 1.0f, -1);
      i_this->current.pos.x = -250.0f;
      i_this->current.pos.y = 20.0f;
      i_this->current.pos.z = 100.0f;
      s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
      i_this->current.angle.y = ang;
      i_this->shape_angle.y = ang;
      wz::state(i_this)->mTargetAngle = ang;
      l_demoSignal = 0;
      wz::state(i_this)->mDemoSteps[0] =
          (s16)(wz::state(i_this)->mDemoSteps[0] + 1);
    } else if (sub == 1) {
      s16 a = (s16)(i_this->mAlpha + 8);
      if (a <= 0xFF) {
        i_this->mAlpha = a;
        break;
      }
      s16 n = wz::state(i_this)->mDemoSteps[0];
      i_this->mAlpha = 0xFF;
      wz::state(i_this)->mDemoSteps[0] = (s16)(n + 1);
    } else if (sub == 2) {
      if ((u8)l_demoSignal == 0) {
        break;
      }
      l_demoSignal = 0;
      i_this->actor_status &= ~0x4000u;
      i_this->mAction = wz::ACTION_NORMAL;
      i_this->mMode = 6;
    }
    break;
  }
  case 0x46: {
    i_this->actor_status |= 0x4000;
    if ((u16)i_this->eventInfo.mCommand != 2) {
      /* request the demo event */
      u32 play = dComIfGp_ea();
      gabi::at<wz::PlayCameraFields>(play)->mEventFlags |= 1;
      fopAcM_orderPotentialEvent_wz(i_this, 0, 0xFFFF, 0);
      i_this->eventInfo.mCondition |= 2;
      break;
    }
    wz::view<wz::PlayerDemo>(player)->mDemoData = 0;
    wz::view<wz::PlayerDemo>(player)->mDemoMode = 3;
    dCamera_Stop_wz(cam);
    dCamera_SetTrimSize_wz(cam, 2);
    s8 h = i_this->health;
    wz::state(i_this)->mDemoFovy = 50.0f;
    if (h > 0) {
      /* the intro */
      wz::view<wz::PlayerDemo>(player)->mDemoParam = 0x18;
      s16 t = (s16)gabi::ftoi((f32)l_debug.mIntroWait + 45.0f);
      i_this->current.pos.x = 0.0f;
      wz::state(i_this)->mTimers[0] = t;
      i_this->current.pos.y = 0.0f;
      i_this->mMode = 0x50;
      i_this->current.pos.z = 0.0f;
      break;
    }
    /* defeated: freeze the summoned enemies */
    i_this->current.angle.y =
        (s16)(fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0)) + 0x8000);
    i_this->shape_angle.y =
        fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
    for (int i = 0; i < 20; i++) {
      if (i_this->mChildFlag[i] == 0) {
        continue;
      }
      fopAc_ac_c *c = fopAcM_SearchByID(i_this->mChildId[i]);
      if (c == nullptr) {
        continue;
      }
      u32 s = c->actor_status;
      if (s & 0x4000) {
        continue;
      }
      c->actor_status = s & ~0x4000u;
      wz::state(i_this)->mDemoChildFlag[i] = 1;
    }
    u32 vt = (u32)player->__vtbl;
    s16 ang =
        (s16)(fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0)) + 0x8000);
    wz_playerSetPos(player, vt, &player->current.pos, ang);
    if (i_this->mCurrBckIdx != 0xD) {
      anm_init(i_this, 5, 5.0f, 0, 1.0f, -1);
      i_this->mMode = 0x5A;
      i_this->speedF = 40.0f;
    } else {
      i_this->mMode = 0x5B;
    }
    break;
  }
  case 0x50: {
    if (wz::state(i_this)->mTimers[0] != 0) {
      break;
    }
    cLib_addCalc2(&wz::state(i_this)->mDemoFovy,
                  (f32)l_debug.mIntroFovy + 50.0f, 1.0f,
                  (f32)l_debug.mIntroFovyStep + 0.5f);
    u32 vt = (u32)player->__vtbl;
    s16 ang =
        (s16)(fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0)) + 0x8000);
    wz_playerSetPos(player, vt, &player->current.pos, ang);
    wz::state(i_this)->mDemoCenterX = (f32)l_debug.mIntroCenterX;
    wz::state(i_this)->mDemoCenterY = (f32)l_debug.mIntroCenterY + 125.0f;
    wz::state(i_this)->mDemoCenterZ = (f32)l_debug.mIntroCenterZ + -29.0f;
    wz::state(i_this)->mDemoEyeX = (f32)l_debug.mIntroEyeX + 1470.0f;
    wz::state(i_this)->mDemoEyeY = (f32)l_debug.mIntroEyeY + 917.0f;
    wz::state(i_this)->mDemoEyeZ = (f32)l_debug.mIntroEyeZ + -222.0f;
    wz::state(i_this)->mTimers[0] =
        (s16)gabi::ftoi((f32)l_debug.mIntroLookWait + 20.0f);
    anm_init(i_this, 0x12, 0.0f, 2, 1.0f, -1);
    i_this->mMode = (u8)(i_this->mMode + 1);
  }
    [[fallthrough]];
  case 0x51: {
    if ((s16)l_debug.mHoldLookCamera != 0) {
      /* debug: hold the camera */
      wz::state(i_this)->mDemoCenterX = (f32)l_debug.mIntroCenterX;
      wz::state(i_this)->mDemoCenterY = (f32)l_debug.mIntroCenterY + 125.0f;
      wz::state(i_this)->mDemoCenterZ = (f32)l_debug.mIntroCenterZ + -29.0f;
      wz::state(i_this)->mDemoEyeX = (f32)l_debug.mIntroEyeX + 1470.0f;
      wz::state(i_this)->mDemoEyeY = (f32)l_debug.mIntroEyeY + 917.0f;
      wz::state(i_this)->mDemoEyeZ = (f32)l_debug.mIntroEyeZ + -222.0f;
      wz::state(i_this)->mTimers[0] =
          (s16)gabi::ftoi((f32)l_debug.mIntroLookWait + 20.0f);
      break;
    }
    if (wz::state(i_this)->mTimers[0] == 0) {
      u32 sub = (u32)(s32)(s16)wz::state(i_this)->mDemoSteps[0];
      if (sub < 2) {
        s16 yrot;
        be<f32> *lookDistance;
        if (sub < 1) {
          wz::view<wz::PlayerDemo>(player)->mDemoParam = 0x14;
          wz::state(i_this)->mTimers[0] =
              (s16)gabi::ftoi((f32)l_debug.mFirstLookWait + 60.0f);
          Mtx34 *m = calc_mtx();
          yrot = player->shape_angle.y;
          mDoMtx_YrotS(m, yrot);
          lookDistance = &l_debug.mFirstLookDistance;
        } else {
          wz::view<wz::PlayerDemo>(player)->mDemoParam = 0x19;
          wz::state(i_this)->mTimers[0] =
              (s16)gabi::ftoi((f32)l_debug.mSecondLookWait + 45.0f);
          yrot = (s16)(player->shape_angle.y + 0x8000);
          mDoMtx_YrotS(calc_mtx(), yrot);
          lookDistance = &l_debug.mSecondLookDistance;
        }
        gabi::Local<cXyz> v;
        gabi::Local<cXyz> out;
        gabi::Local<cXyz> sum;
        v->x = 0.0f;
        f32 z = (f32)*lookDistance + 40000.0f;
        v->y = 0.0f;
        v->z = z;
        MtxPosition(v, out);
        cXyz_pl(out, sum, &player->current.pos);
        i_this->eyePos.copy(*sum);
        wz_demoSe(i_this, 0x5909);
        wz::state(i_this)->mDemoSteps[0] =
            (s16)(wz::state(i_this)->mDemoSteps[0] + 1);
      }
    }
    cLib_addCalc2(&wz::state(i_this)->mDemoFovy, (f32)l_debug.mLookFovy + 50.0f,
                  1.0f, (f32)l_debug.mIntroFovyStep + 0.5f);
    wz_camCalc(&wz::state(i_this)->mDemoCenterX,
               (f32)l_debug.mLookCenterX + 150.0f, (f32)l_debug.mLookCameraRate,
               0.01f);
    wz_camCalc(&wz::state(i_this)->mDemoCenterY,
               (f32)l_debug.mLookCenterY + 158.0f, (f32)l_debug.mLookCameraRate,
               0.01f);
    wz_camCalc(&wz::state(i_this)->mDemoCenterZ,
               (f32)l_debug.mLookCenterZ + 114.0f, (f32)l_debug.mLookCameraRate,
               0.01f);
    wz_camCalc(&wz::state(i_this)->mDemoEyeX,
               (f32)(s32)((s16)l_debug.mLookEyeX + 0x492),
               (f32)l_debug.mLookCameraRate, 0.01f);
    wz_camCalc(&wz::state(i_this)->mDemoEyeY,
               (f32)(s32)((s16)l_debug.mLookEyeY + 0x2CD),
               (f32)l_debug.mLookCameraRate, 0.01f);
    wz_camCalc(&wz::state(i_this)->mDemoEyeZ,
               (f32)(s32)((s16)l_debug.mLookEyeZ - 0x336),
               (f32)l_debug.mLookCameraRate, 0.01f);
    s16 sub = wz::state(i_this)->mDemoSteps[0];
    if (sub < 2) {
      break;
    }
    if (wz::state(i_this)->mTimers[0] <=
        (s16)((s16)l_debug.mAppearWait + 0x14)) {
      if (sub == 2) {
        wz_demoSe(i_this, 0x5909);
        wz::state(i_this)->mDemoSteps[0] = 3;
      }
      s16 a = (s16)(i_this->mAlpha + ((s16)l_debug.mAppearAlphaStep + 10));
      if (a > 0xFF) {
        i_this->mAlpha = 0xFF;
      } else {
        i_this->mAlpha = a;
      }
    }
    if ((s16)l_debug.mHoldIntro != 0) {
      break;
    }
    if (wz::state(i_this)->mTimers[0] != 0) {
      break;
    }
    wz::view<wz::PlayerDemo>(player)->mDemoParam = 0x18;
    wz::state(i_this)->mTimers[0] = (s16)((s16)l_debug.mLookEndWait + 0x1E);
    i_this->mMode = (u8)(i_this->mMode + 1);
    break;
  }
  case 0x52: {
    s16 a = (s16)(i_this->mAlpha + ((s16)l_debug.mAppearEndAlphaStep + 5));
    s16 t = wz::state(i_this)->mTimers[0];
    if (a > 0xFF) {
      i_this->mAlpha = 0xFF;
    } else {
      i_this->mAlpha = a;
    }
    if (t != 0) {
      break;
    }
    i_this->mMode = (u8)(i_this->mMode + 1);
    break;
  }
  case 0x53: {
    f32 x = i_this->current.pos.x;
    f32 y = i_this->current.pos.y + 15.0f;
    f32 z = i_this->current.pos.z;
    wz::state(i_this)->mHomeX = x;
    wz::state(i_this)->mHomeZ = z;
    i_this->mBaseY = y;
    anm_init(i_this, 0x11, 0.0f, 0, 1.0f, -1);
    i_this->mMode = (u8)(i_this->mMode + 1);
  }
    [[fallthrough]];
  case 0x54: {
    s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
    i_this->current.angle.y = ang;
    i_this->shape_angle.y = ang;
    gabi::Local<cXyz> ppos;
    ppos->y = 0.0f;
    ppos->z = 850.0f;
    wz::state(i_this)->mTargetAngle = ang;
    ppos->x = 50.0f;
    u32 vt = (u32)player->__vtbl;
    s16 pang = fopAcM_searchActorAngleY(player, i_this);
    wz_playerSetPos(player, vt, ppos, pang);
    cLib_addCalc2(&wz::state(i_this)->mDemoFovy,
                  (f32)l_debug.mEntranceFovy + 50.0f, 1.0f,
                  (f32)l_debug.mIntroFovyStep + 0.5f);
    wz::state(i_this)->mDemoCenterX = (f32)l_debug.mEntranceCenterX + -108.0f;
    wz::state(i_this)->mDemoCenterY = (f32)l_debug.mEntranceCenterY + 174.0f;
    wz::state(i_this)->mDemoCenterZ = (f32)l_debug.mEntranceCenterZ + 231.0f;
    wz::state(i_this)->mDemoEyeX = (f32)l_debug.mEntranceEyeX + 199.0f;
    wz::state(i_this)->mDemoEyeY = (f32)l_debug.mEntranceEyeY + 50.0f;
    wz::state(i_this)->mDemoEyeZ = (f32)l_debug.mEntranceEyeZ + 1092.0f;
    s16 a = (s16)(i_this->mAlpha +
                  (s16)gabi::ftoi((f32)l_debug.mEntranceAlphaStep + 5.0f));
    if (a < 0xFF) {
      i_this->mAlpha = a;
      break;
    }
    i_this->mAlpha = 0xFF;
    if ((s16)l_debug.mHoldIntro != 0) {
      i_this->m2B4->mFrameCtrl.mRate = 0.0f; /* stop the animation */
      break;
    }
    wz::state(i_this)->mTimers[0] = (s16)gabi::ftoi((f32)l_debug.mEntranceWait);
    i_this->m2B4->mFrameCtrl.mRate = 1.0f;
    i_this->mMode = (u8)(i_this->mMode + 1);
  }
    [[fallthrough]];
  case 0x55:
    if (wz::state(i_this)->mTimers[0] != 0) {
      break;
    }
    i_this->m2B4->mFrameCtrl.mRate = 1.0f;
    i_this->mMode = (u8)(i_this->mMode + 1);
    [[fallthrough]];
  case 0x56: {
    cLib_addCalc2(&wz::state(i_this)->mDemoFovy,
                  (f32)l_debug.mDoorsFovy + 45.0f, 1.0f,
                  (f32)l_debug.mDoorsFovyStep + 0.2f);
    wz_camCalc(&wz::state(i_this)->mDemoCenterX, (f32)l_debug.mDoorsCenterX,
               (f32)l_debug.mDoorsCameraRate, 0.025f);
    wz_camCalc(&wz::state(i_this)->mDemoCenterY,
               (f32)l_debug.mDoorsCenterY + 135.0f,
               (f32)l_debug.mDoorsCameraRate, 0.025f);
    wz_camCalc(&wz::state(i_this)->mDemoCenterZ,
               (f32)l_debug.mDoorsCenterZ + -60.0f,
               (f32)l_debug.mDoorsCameraRate, 0.025f);
    wz_camCalc(&wz::state(i_this)->mDemoEyeX, (f32)l_debug.mDoorsEyeX + 50.0f,
               (f32)l_debug.mDoorsCameraRate, 0.025f);
    wz_camCalc(&wz::state(i_this)->mDemoEyeY, (f32)l_debug.mDoorsEyeY + 110.0f,
               (f32)l_debug.mDoorsCameraRate, 0.025f);
    wz_camCalc(&wz::state(i_this)->mDemoEyeZ, (f32)l_debug.mDoorsEyeZ + 330.0f,
               (f32)l_debug.mDoorsCameraRate, 0.025f);
    if (!i_this->m2B4->mFrameCtrl.checkPass(159.0f)) {
      break;
    }
    if ((s16)l_debug.mHoldIntro != 0) {
      i_this->m2B4->mFrameCtrl.mRate = 0.0f;
      break;
    }
    f32 sp = (f32)l_debug.mIntroBackwardSpeed + 10.0f;
    wz::state(i_this)->mDemoSteps[0] = 0;
    wz::state(i_this)->mDemoSteps[1] = 0;
    wz::state(i_this)->mTimers[0] = 2;
    i_this->mMode = (u8)(i_this->mMode + 1);
    i_this->speedF = -sp;
    break;
  }
  case 0x57: {
    cLib_addCalc0(&i_this->speedF, 1.0f,
                  (f32)l_debug.mIntroDeceleration + 0.3f);
    if ((s16)l_debug.mOpenDoors != 0) {
      wz::state(i_this)->mTimers[0] = 1;
      l_debug.mOpenDoors = 0;
    }
    bool bgm;
    if (wz::state(i_this)->mDemoSteps[0] == 0 &&
        wz::state(i_this)->mTimers[0] == 0) {
      /* open the two summon doors */
      gabi::Local<cXyz> pos;
      gabi::Local<cXyz> scale;
      scale->y = 4.0f;
      pos->z = 100.0f;
      scale->z = 4.0f;
      pos->x = 215.0f;
      pos->y = 180.0f;
      scale->x = 4.0f;
      {
        s8 room = i_this->current.roomNo;
        if (fopAcM_createChild(0xD0, fopAcM_GetID(i_this), 0xFFFFFF0D, pos,
                               room, &i_this->current.angle, scale, 0,
                               0) == fpcM_ERROR_PROCESS_ID_e) {
          break;
        }
      }
      pos->y = 180.0f;
      pos->z = 100.0f;
      pos->x = -250.0f;
      {
        s8 room = i_this->current.roomNo;
        if (fopAcM_createChild(0xD0, fopAcM_GetID(i_this), 0xFFFFFF0D, pos,
                               room, &i_this->current.angle, scale, 0,
                               0) == fpcM_ERROR_PROCESS_ID_e) {
          break;
        }
      }
      wz::state(i_this)->mDemoSteps[0] = 1;
      wz::state(i_this)->mTimers[1] = 0x28;
      s16 t = (s16)gabi::ftoi((f32)l_debug.mBgmWait + 70.0f);
      wz::state(i_this)->mTimers[2] = t;
      bgm = t == 1;
    } else {
      bgm = wz::state(i_this)->mTimers[2] == 1;
    }
    if (bgm) {
      mDoAud_subBgmStart_wz(0x80000019);
    }
    bool freezeParent = true;
    if (wz::state(i_this)->mDemoSteps[0] != 0 &&
        wz::state(i_this)->mDemoSteps[1] == 0 &&
        wz::state(i_this)->mTimers[1] == 0) {
      /* the mini-boss itself */
      gabi::Local<cXyz> pos;
      pos->y = 0.0f;
      pos->z = 100.0f;
      pos->x = 215.0f;
      u32 id = fopAcM_create(0xBF, 0xFFFFFF2C, pos, i_this->current.roomNo,
                             &i_this->current.angle, nullptr, -1, 0);
      i_this->mParentId = id;
      if (id == 0xFFFFFFFF) {
        freezeParent = false;
      } else {
        wz::state(i_this)->mDemoSteps[1] = 1;
        l_demoSignal = 1;
      }
    }
    if (freezeParent && (s32)i_this->mParentId != -1) {
      fopAc_ac_c *p = fopAcM_SearchByID(i_this->mParentId);
      if (p != nullptr) {
        u32 s = p->actor_status;
        p->speedF = 0.0f;
        p->actor_status = s | 0x4000;
      }
    }
    cLib_addCalc2(&wz::state(i_this)->mDemoFovy,
                  (f32)l_debug.mFightFovy + 65.0f, 1.0f,
                  (f32)l_debug.mFightFovyStep + 2.5f);
    wz_camCalc(&wz::state(i_this)->mDemoCenterX, (f32)l_debug.mFightCenterX,
               (f32)l_debug.mFightCameraRate, 0.3f);
    wz_camCalc(&wz::state(i_this)->mDemoCenterY,
               (f32)l_debug.mFightCenterY + 90.0f,
               (f32)l_debug.mFightCameraRate, 0.3f);
    wz_camCalc(&wz::state(i_this)->mDemoCenterZ,
               (f32)l_debug.mFightCenterZ + 440.0f,
               (f32)l_debug.mFightCameraRate, 0.3f);
    wz_camCalc(&wz::state(i_this)->mDemoEyeX, (f32)l_debug.mFightEyeX,
               (f32)l_debug.mFightCameraRate, 0.3f);
    wz_camCalc(&wz::state(i_this)->mDemoEyeY, (f32)l_debug.mFightEyeY + 50.0f,
               (f32)l_debug.mFightCameraRate, 0.3f);
    wz_camCalc(&wz::state(i_this)->mDemoEyeZ, (f32)l_debug.mFightEyeZ + 570.0f,
               (f32)l_debug.mFightCameraRate, 0.3f);
    {
      mDoExt_McaMorf *morf = gabi::at<mDoExt_McaMorf>(i_this->m2B4.v);
      if (!(morf->mFrameCtrl.mState & 1) &&
          !(morf->mFrameCtrl.getRate() == 0.0f)) {
        if (!morf->mFrameCtrl.checkPass((f32)l_debug.mFightStartFrame +
                                        274.0f)) {
          break;
        }
      }
    }
    if ((s16)l_debug.mHoldIntro != 0) {
      break;
    }
    /* the fight starts */
    dComIfGp_StopQuake_wz(0x20);
    {
      gabi::Local<cXyz> center;
      gabi::Local<cXyz> eye;
      center->z = wz::state(i_this)->mDemoCenterZ;
      center->x = wz::state(i_this)->mDemoCenterX;
      eye->x = wz::state(i_this)->mDemoEyeX;
      eye->y = wz::state(i_this)->mDemoEyeY;
      center->y = wz::state(i_this)->mDemoCenterY;
      eye->z = wz::state(i_this)->mDemoEyeZ;
      dCamera_Reset_wz(cam, center, eye);
    }
    dCamera_Start_wz(cam);
    dCamera_SetTrimSize_wz(cam, 0);
    wz::view<wz::PlayerDemo>(player)->mDemoMode = 2;
    wz::view<wz::PlayerDemo>(player)->mDemoParam = 1;
    {
      u32 play = dComIfGp_ea();
      gabi::at<wz::PlayCameraFields>(play)->mEventFlags |= 8;
    }
    wz_demoMonsSe(i_this);
    {
      fopAc_ac_c *p = fopAcM_SearchByID(i_this->mParentId);
      if (p != nullptr) {
        p->actor_status &= ~0x4000u;
      }
    }
    if ((s16)l_debug.mHoldPartner == 0) {
      wz::state(i_this)->mHasActiveSummon = 1;
      wz::state(i_this)->mHasSummonedPartner = 1;
      l_demoSignal = 1;
    } else {
      i_this->mParentId = 0xFFFFFFFF;
      l_demoSignal = 1;
    }
    i_this->mAction = wz::ACTION_NORMAL;
    i_this->mMode = 6;
    i_this->actor_status &= ~0x4000u;
    break;
  }
  case 0x5A: {
    rod_size_set(i_this, 1);
    u32 vt = (u32)player->__vtbl;
    s16 ang =
        (s16)(fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0)) + 0x8000);
    wz_playerSetPos(player, vt, &player->current.pos, ang);
    cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
    mDoExt_McaMorf *morf = gabi::at<mDoExt_McaMorf>(i_this->m2B4.v);
    if (!(morf->mFrameCtrl.mState & 1) &&
        !(morf->mFrameCtrl.getRate() == 0.0f)) {
      break;
    }
    i_this->mMode = (u8)(i_this->mMode + 1);
    i_this->speedF = 0.0f;
  }
    [[fallthrough]];
  case 0x5B: {
    if ((s16)l_debug.mHoldDefeat != 0) {
      break;
    }
    /* defeated: the summoned enemies vanish */
    for (int i = 0; i < 20; i++) {
      if ((u8)wz::state(i_this)->mDemoChildFlag[i] == 0) {
        continue;
      }
      fopAc_ac_c *c = fopAcM_SearchByID(i_this->mChildId[i]);
      if (c != nullptr) {
        c->actor_status |= 0x4000;
      }
      wz::state(i_this)->mDemoChildFlag[i] = 0;
    }
    fopAcM_createDisappear(i_this, dpos, 10, 0, i_this->stealItemBitNo);
    u32 tg = (u32)i_this->mBodyCyl.mObjTg.mSPrm;
    i_this->mRodScaleX = 0.0f;
    i_this->mRodScaleY = 0.0f;
    i_this->mRodScaleZ = 0.0f;
    u32 s = i_this->actor_status;
    i_this->attention_info.flags = 0;
    i_this->mBodyCyl.mObjTg.mSPrm = tg & ~1u;
    i_this->speedF = 0.0f;
    i_this->scale.x = 0.0f;
    i_this->scale.y = 0.0f;
    i_this->actor_status = s & ~0x20u;
    i_this->mAlpha = 0;
    i_this->scale.z = 0.0f;
    dCcD_GObjInf_ClrTgHit((&i_this->mBodyCyl));
    wz::state(i_this)->mTimers[0] = (s16)((s16)l_debug.mDefeatWait + 0x1E);
    i_this->mMode = (u8)(i_this->mMode + 1);
  }
    [[fallthrough]];
  case 0x5C: {
    if (wz::state(i_this)->mTimers[0] != 0) {
      break;
    }
    dComIfGp_StopQuake_wz(0x20);
    {
      gabi::Local<cXyz> center;
      gabi::Local<cXyz> eye;
      eye->z = wz::state(i_this)->mDemoEyeZ;
      center->y = wz::state(i_this)->mDemoCenterY;
      eye->x = wz::state(i_this)->mDemoEyeX;
      center->x = wz::state(i_this)->mDemoCenterX;
      eye->y = wz::state(i_this)->mDemoEyeY;
      center->z = wz::state(i_this)->mDemoCenterZ;
      dCamera_Reset_wz(cam, center, eye);
    }
    dCamera_Start_wz(cam);
    dCamera_SetTrimSize_wz(cam, 0);
    wz::view<wz::PlayerDemo>(player)->mDemoMode = 2;
    wz::view<wz::PlayerDemo>(player)->mDemoParam = 1;
    {
      u32 play = dComIfGp_ea();
      gabi::at<wz::PlayCameraFields>(play)->mEventFlags |= 8;
    }
    if (wz_isStartStage("kazeMB" /* "kazeMB" */) && i_this->mIsMiniBoss) {
      mDoAud_subBgmStop();
    }
    if (wz::state(i_this)->mHasActiveSummon != 0) {
      i_this->mAction = wz::ACTION_DAMAGE;
      i_this->mMode = 0x2B;
      break;
    }
    fopAcM_delete(i_this);
    if (wz_isStartStage("kazeMB")) {
      break;
    }
    if (i_this->mDisableSpawnOnDeathSwitch != 0xFF) {
      dComIfGs_onSwitch(i_this->mDisableSpawnOnDeathSwitch,
                        i_this->current.roomNo);
    }
    dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
    break;
  }
  default:
    break;
  }

  /* the demo camera */
  if (i_this->health <= 0) {
    f32 x = i_this->current.pos.x, y = i_this->current.pos.y,
        z = i_this->current.pos.z;
    wz::state(i_this)->mDemoCenterX = x;
    wz::state(i_this)->mDemoCenterZ = z;
    wz::state(i_this)->mDemoCenterY = y + 50.0f;
    Mtx34 *m = calc_mtx();
    mDoMtx_YrotS(m, fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0)));
    gabi::Local<cXyz> v;
    gabi::Local<cXyz> out;
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = 400.0f;
    MtxPosition(v, out);
    PSVECAdd(out, &i_this->current.pos, out);
    f32 ex = out->x - 100.0f;
    f32 ey = out->y + 400.0f;
    wz::state(i_this)->mDemoEyeX = ex;
    f32 ez = out->z - 300.0f;
    wz::state(i_this)->mDemoEyeY = ey;
    wz::state(i_this)->mDemoEyeZ = ez;
  }
  if (i_this->mCurrBckIdx == 0x11 &&
      i_this->m2B4->mFrameCtrl.checkPass(70.0f)) {
    wz_demoMonsSe(i_this);
  }
  s8 h = i_this->health;
  u8 st2 = i_this->mMode;
  if (h > 0 ? st2 >= 0x51 : st2 >= 0x5A) {
    gabi::Local<cXyz> center;
    gabi::Local<cXyz> eye;
    eye->x = wz::state(i_this)->mDemoEyeX;
    center->y = wz::state(i_this)->mDemoCenterY;
    eye->z = wz::state(i_this)->mDemoEyeZ;
    center->x = wz::state(i_this)->mDemoCenterX;
    eye->y = wz::state(i_this)->mDemoEyeY;
    center->z = wz::state(i_this)->mDemoCenterZ;
    dCamera_Set_wz(cam, center, eye, wz::state(i_this)->mDemoFovy, 0);
  }
}

/* ---- execute (GameCube action_dousa / action_itai / action_tama_dousa /
 * summon_call_sub / sea_water_check inlined) ---- */

static inline BOOL enemy_ice_wz(void *ice) { return wz_updateIce(ice); }
static inline BOOL dBgS_ChkMoveBG_wz(void *polyInfo) {
  return wz_checkMoveBg(dComIfG_Bgsp(), polyInfo);
}
static inline BOOL dCcD_GObjInf_ChkAtHit_wz(void *o) {
  return wz_checkAttackHit(o);
}
static inline void dKy_arrowcol_chg_on_wz(void *a, s32 b) {
  wz_enableArrowColor(a, b);
}
static inline void fopAcM_setGbaName_wz(fopAc_ac_c *a, u8 itemNo, u8 n0,
                                        u8 n1) {
  wz_setGbaName(a, itemNo, n0, n1);
}
static inline void fopAcM_cancelCarryNow_wz(fopAc_ac_c *a) {
  wz_cancelCarry(a);
}
static inline void enemy_piyo_set_wz(fopAc_ac_c *a) { wz_setStun(a); }
static inline void cCcS_Set_wz(void *obj) {
  wz_registerCollision(gabi::at<void>(dComIfGp_ea() + 0x26A4), obj);
}
static inline void dCcMassS_Mng_Set_wz(void *obj, u8 p) {
  wz_registerMass(gabi::at<void>(dComIfGp_ea() + 0x4EF8), obj, p);
}

/* the rod-tip emitters follow the rod tip (rod joint 2) */
static void wz_rodEmitterMtx(wz_class *i_this, u32 em) {
  u32 model = morf_model(i_this->mpRodMorf.v);
  u32 blk = gabi::at<J3DModel>(model)->mpMtxBuffer.v;
  u16 fl = (u16)gabi::at<J3DMtxBuffer_hd>(blk)->mFlags;
  u32 mtx = (u32)gabi::at<J3DMtxBuffer_hd>(blk)->mpAnmMtx;
  gabi::at<J3DMtxBuffer_hd>(blk)->mFlags = (u16)(fl | 0x10);
  wz::Emitter *emitter = gabi::at<wz::Emitter>(em);
  wz_setEmitterMatrix(&gabi::at<Mtx34>(mtx)[wz::JOINT_ROD_TIP],
                      &emitter->mGlobalRotation, &emitter->mGlobalDynamicsScale,
                      &emitter->mGlobalTranslation);
}
static inline u32 wz_cbEmitter(wz_class *i_this, int i) {
  return (u32)i_this->mFollowCb[i].mpEmitter.v;
}
static inline void wz_rodParticle(wz_class *i_this, u16 id, int cb) {
  dPa_control_set(
      dComIfGp_getParticle(), 0, id, &i_this->mRodTipPos, nullptr, nullptr,
      0xFF, reinterpret_cast<dPa_levelEcallBack *>(&i_this->mFollowCb[cb]), -1,
      nullptr, nullptr, nullptr);
}
/* the summoner colours of the rod light: prim (0x101D35C8) and env (0x101D35D4)
 * per summon kind */
static void wz_rodColor(wz_class *i_this, int cb) {
  wz::Emitter *emitter = gabi::at<wz::Emitter>(wz_cbEmitter(i_this, cb));
  const GXColor *color = &l_rodPrimColor[(u8)wz::state(i_this)->mWeaponKind];
  u8 blue = color->b, red = color->r, green = color->g;
  emitter->mGlobalPrmColor.r = red;
  emitter->mGlobalPrmColor.g = green;
  emitter->mGlobalPrmColor.b = blue;
  color = &l_rodEnvColor[(u8)wz::state(i_this)->mWeaponKind];
  emitter = gabi::at<wz::Emitter>(wz_cbEmitter(i_this, cb));
  blue = color->b;
  red = color->r;
  green = color->g;
  emitter->mGlobalEnvColor.r = red;
  emitter->mGlobalEnvColor.g = green;
  emitter->mGlobalEnvColor.b = blue;
  wz_rodEmitterMtx(i_this, wz_cbEmitter(i_this, cb));
}
static inline bool wz_morfStopped(wz_class *i_this) {
  mDoExt_McaMorf *m = gabi::at<mDoExt_McaMorf>(i_this->m2B4.v);
  return (m->mFrameCtrl.mState & 1) || m->mFrameCtrl.getRate() == 0.0f;
}
static inline void wz_monsSe(wz_class *i_this, u32 id) {
  if (gabi::ea(i_this) != 0 && gabi::ea(&i_this->eyePos) != 0) {
    s8 room = i_this->current.roomNo;
    u32 pid = fopAcM_GetID(i_this);
    mDoAud_monsSeStart(id, &i_this->eyePos, pid, 0, dComIfGp_getReverb(room));
  }
}
static inline void wz_followEnd2(wz_class *i_this, int a, int b) {
  wz_followEnd(i_this, a);
  wz_followEnd(i_this, b);
}
static inline void wz_tamaNext(wz_class *i_this, f32 x, f32 y, f32 z) {
  gabi::Local<cXyz> p;
  p->x = x;
  p->y = y;
  p->z = z;
  next_tama_move(i_this, p);
}

// HD: GHS inlined these GameCube action helpers into Execute.
/* inlined into daWZ_Execute */
static void action_dousa(wz_class *i_this) {
  dComIfGp_get();
  switch (i_this->mMode) {
  case 0: {
    f32 d = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
    if (d > 7500.0f) {
      break;
    }
    for (int i = 0; i < 4; i++) {
      wz::state(i_this)->mDemoSteps[i] = 0;
    }
    if (i_this->mBehaviorType == 1) {
      u32 pnt =
          gabi::at<wz::Path>(wz::state(i_this)->mpNextRoomPath)->m_points.v +
          (u32)wz::state(i_this)->mPathPoint * 16;
      i_this->mSummonPos.x = (f32)gabi::at<wz::PathPoint>(pnt)->m_position.x;
      i_this->mSummonPos.y = (f32)gabi::at<wz::PathPoint>(pnt)->m_position.y;
      wz::state(i_this)->mWeaponKind = 1;
      i_this->mSummonPos.z = (f32)gabi::at<wz::PathPoint>(pnt)->m_position.z;
    }
    i_this->scale.x = 1.0f;
    i_this->scale.y = 1.0f;
    i_this->scale.z = 1.0f;
    wz_demoSe(i_this, 0x5909);
    anm_init(i_this, 0x10, 5.0f, 2, 1.0f, -1);
    u32 s = i_this->actor_status;
    i_this->attention_info.flags = 4;
    i_this->actor_status = s | 0x20;
    i_this->mMode = (u8)(i_this->mMode + 1);
  }
    [[fallthrough]];
  case 1: {
    s16 a = (s16)(i_this->mAlpha + 8);
    if (a < 0xFF) {
      i_this->mAlpha = a;
      break;
    }
    i_this->mAlpha = 0xFF;
    i_this->mMode = (u8)(i_this->mMode + 1);
  }
    [[fallthrough]];
  case 2: {
    wz::state(i_this)->mTimers[0] = 0x5A;
    u8 f = 0;
    wz::state(i_this)->mWeaponKind = 0;
    if (i_this->mBehaviorType == 1 &&
        wz::state(i_this)->mHasActiveSummon == 0) {
      f32 dx = i_this->mSummonPos.x - i_this->current.pos.x;
      f32 dz = i_this->mSummonPos.z - i_this->current.pos.z;
      wz::state(i_this)->mWeaponKind = 1;
      s16 a = cM_atan2s(dx, dz);
      f = (u8)wz::state(i_this)->mWeaponKind;
      wz::state(i_this)->mTargetAngle = a;
    }
    if (f >= 1) {
      if (f == 1) {
        anm_init(i_this, 0x13, 5.0f, 2, 1.0f, -1);
      }
    } else {
      anm_init(i_this, 0x14, 5.0f, 2, 1.0f, -1);
    }
    wz_rodParticle(i_this, 0x8287, 2);
    u32 em = wz_cbEmitter(i_this, 2);
    if (em != 0) {
      wz_rodEmitterMtx(i_this, em);
    }
    i_this->mMode = (u8)(i_this->mMode + 1);
    i_this->mBodyCyl.mObjTg.mSPrm = (u32)i_this->mBodyCyl.mObjTg.mSPrm | 1;
    break;
  }
  case 3: {
    u32 em = wz_cbEmitter(i_this, 2);
    if (em != 0) {
      wz_rodEmitterMtx(i_this, em);
    }
    rod_size_set(i_this, 0);
    if (wz::state(i_this)->mTimers[0] != 0) {
      break;
    }
    i_this->mMode = (u8)(i_this->mMode + 1);
    wz_followEnd(i_this, 2);
    break;
  }
  case 4: {
    anm_init(i_this, 6, 5.0f, 0, 1.0f, -1);
    if (wz_cbEmitter(i_this, 3) != 0 ||
        (wz_rodParticle(i_this, 0x8288, 3), wz_cbEmitter(i_this, 3) != 0)) {
      wz_rodColor(i_this, 3);
    }
    if (wz_cbEmitter(i_this, 4) != 0 ||
        (wz_rodParticle(i_this, 0x8289, 3), wz_cbEmitter(i_this, 4) != 0)) {
      wz_rodColor(i_this, 4);
    }
    u8 f = (u8)wz::state(i_this)->mWeaponKind;
    if (f < 1) {
      wz_se(i_this, 0x590C, 0);
    } else if (f == 1) {
      wz_se(i_this, 0x590D, 0);
    }
    i_this->mMode = (u8)(i_this->mMode + 1);
    break;
  }
  case 5: {
    u32 em = wz_cbEmitter(i_this, 3);
    if (em != 0) {
      wz_rodEmitterMtx(i_this, em);
    }
    em = wz_cbEmitter(i_this, 4);
    if (em != 0) {
      wz_rodEmitterMtx(i_this, em);
    }
    u32 morf;
    if ((u8)wz::state(i_this)->mWeaponKind != 1) {
      morf = i_this->m2B4.v;
      if ((f32)wz::morf(morf)->mFrameCtrl.mFrame < 38.0f) {
        wz::state(i_this)->mTargetAngle =
            fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        morf = i_this->m2B4.v;
      }
    } else {
      morf = i_this->m2B4.v;
    }
    if (wz::morf(morf)->mFrameCtrl.checkPass(35.0f)) {
      u8 f = (u8)wz::state(i_this)->mWeaponKind;
      if (f < 1 || (f == 1 && wz::state(i_this)->mHasActiveSummon != 0)) {
        weapon_shoot(i_this, 0);
      } else if (f == 1) {
        weapon_shoot(i_this, 1);
      }
    }
    if (!wz_morfStopped(i_this)) {
      break;
    }
    wz_monsSe(i_this, 0x48D6);
    i_this->mMode = 6;
    wz_followEnd2(i_this, 3, 4);
    break;
  }
  case 6: {
    i_this->speed.x = 0.0f;
    i_this->speed.y = 0.0f;
    i_this->speed.z = 0.0f;
    i_this->speedF = 0.0f;
    fopAcM_cancelCarryNow_wz(i_this);
    u32 v = (u32)i_this->attention_info.flags;
    i_this->shape_angle.x = 0;
    i_this->current.angle.x = 0;
    i_this->shape_angle.z = 0;
    i_this->attention_info.flags = v & ~0x10u;
    i_this->current.angle.z = 0;
    anm_init(i_this, 0x10, 5.0f, 2, 1.0f, -1);
    u32 s = i_this->actor_status;
    u32 tg = (u32)i_this->mBodyCyl.mObjTg.mSPrm;
    i_this->attention_info.flags = 0;
    i_this->actor_status = s & ~0x20u;
    i_this->mBodyCyl.mObjTg.mSPrm = tg & ~1u;
    dCcD_GObjInf_ClrTgHit((&i_this->mBodyCyl));
    i_this->current.angle.y = i_this->shape_angle.y;
    i_this->mMode = (u8)(i_this->mMode + 1);
  }
    [[fallthrough]];
  case 7: {
    s16 a = (s16)(i_this->current.angle.y -
                  ((s16)l_debug.mVanishAngleStep + 0x700));
    i_this->current.angle.y = a;
    i_this->shape_angle.y = a;
    wz::state(i_this)->mTargetAngle = a;
    rod_size_set(i_this, 1);
    s16 al = (s16)(i_this->mAlpha - 8);
    i_this->mAlpha = al;
    if (al > 0) {
      break;
    }
    if (wz::state(i_this)->mHasActiveSummon != 0) {
      wz::state(i_this)->mTimers[1] = (s16)gabi::ftoi(cM_rndF(100.0f) + 100.0f);
    }
    i_this->mMode = (u8)(i_this->mMode + 1);
    i_this->scale.x = 0.0f;
    i_this->scale.y = 0.0f;
    i_this->scale.z = 0.0f;
    i_this->mAlpha = 0;
  }
    [[fallthrough]];
  case 8: {
    if (wz::state(i_this)->mTimers[1] != 0) {
      break;
    }
    u32 path;
    if ((u8)wz::state(i_this)->mPathIndex != 0xFF &&
        (path = (u32)wz::state(i_this)->mpPath) != 0) {
      /* warp to a random point of the path */
      f32 r = cM_rndF((f32)(u16)gabi::at<wz::Path>(path)->m_num);
      path = (u32)wz::state(i_this)->mpPath;
      u32 idx = (u32)gabi::ftoi(r);
      wz::state(i_this)->mPathPoint = idx;
      if (idx == (u16)gabi::at<wz::Path>(path)->m_num) {
        path = (u32)wz::state(i_this)->mpPath;
        idx--;
        wz::state(i_this)->mPathPoint = idx;
      }
      u32 pnt = gabi::at<wz::Path>(path)->m_points.v + idx * 16;
      f32 x = (f32)gabi::at<wz::PathPoint>(pnt)->m_position.x;
      i_this->current.pos.x = x;
      f32 y = (f32)gabi::at<wz::PathPoint>(pnt)->m_position.y;
      u32 wx = (u32)*wz::view<be<u32>>(&i_this->current.pos.x);
      i_this->current.pos.y = y;
      f32 z = (f32)gabi::at<wz::PathPoint>(pnt)->m_position.z;
      i_this->mRodScaleX = 0.0f;
      i_this->current.pos.z = z;
      u32 wz = (u32)*wz::view<be<u32>>(&i_this->current.pos.z);
      i_this->old.pos.x = x;
      i_this->mRodScaleY = 0.0f;
      i_this->mRodScaleZ = 0.0f;
      i_this->mMode = 0;
      *wz::view<be<u32>>(&wz::state(i_this)->mHomeX) = wx;
      *wz::view<be<u32>>(&i_this->mBaseY) =
          (u32)*wz::view<be<u32>>(&i_this->current.pos.y);
      i_this->old.pos.y = y;
      i_this->old.pos.z = z;
      *wz::view<be<u32>>(&wz::state(i_this)->mHomeZ) = wz;
      break;
    }
    f32 r = cM_rndFX(200.0f);
    f32 hx = wz::state(i_this)->mHomeX;
    f32 hy = i_this->mBaseY;
    i_this->current.pos.x = hx + r;
    i_this->current.pos.y = hy;
    r = cM_rndFX(200.0f);
    f32 hz = wz::state(i_this)->mHomeZ;
    i_this->mMode = 0;
    i_this->current.pos.z = hz + r;
    i_this->mRodScaleX = 0.0f;
    i_this->mRodScaleY = 0.0f;
    i_this->mRodScaleZ = 0.0f;
    break;
  }
  default:
    break;
  }
  if ((u8)wz::state(i_this)->mWeaponKind != 1 && i_this->mMode < 4) {
    wz::state(i_this)->mTargetAngle =
        fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
  }
  body_atari_check(i_this);
}
/* inlined into daWZ_Execute */
static void action_itai(wz_class *i_this) {
  fopAc_ac_c *player = dComIfGp_getPlayer(0);
  gabi::Local<cXyz> dpos;
  dpos->x = i_this->current.pos.x;
  dpos->y = i_this->current.pos.y;
  dpos->z = i_this->current.pos.z;
  bool eye = false;
  auto stopDamage = [&]() {
    u32 tg = (u32)i_this->mBodyCyl.mObjTg.mSPrm;
    i_this->speedF = 0.0f;
    i_this->mBodyCyl.mObjTg.mSPrm = tg & ~1u;
    dCcD_GObjInf_ClrTgHit((&i_this->mBodyCyl));
    i_this->mAction = wz::ACTION_NORMAL;
    i_this->mMode = 6;
  };
  auto setDeathSwitches = [&]() {
    if (i_this->mDisableSpawnOnDeathSwitch != 0xFF) {
      dComIfGs_onSwitch(i_this->mDisableSpawnOnDeathSwitch,
                        i_this->current.roomNo);
    }
    dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
  };
  auto damageState = [&]() {
    switch (i_this->mMode) {
    case 10: {
      s16 py = player->shape_angle.y;
      i_this->current.angle.y = py;
      wz::state(i_this)->mTargetAngle = py;
      i_this->shape_angle.y =
          fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
      wz_followEnd(i_this, 2);
      wz_followEnd2(i_this, 3, 4);
      for (int i = 0; i < 4; i++) {
        wz::state(i_this)->mDemoSteps[i] = 0;
      }
      u8 k = i_this->mHitKind;
      if (k == 7) {
        u32 tg = (u32)i_this->mBodyCyl.mObjTg.mSPrm;
        i_this->speedF = 0.0f;
        i_this->mBodyCyl.mObjTg.mSPrm = tg & ~1u;
        dCcD_GObjInf_ClrTgHit((&i_this->mBodyCyl));
        i_this->mRodScaleX = 0.0f;
        i_this->mRodScaleY = 0.0f;
        i_this->mRodScaleZ = 0.0f;
        i_this->attention_info.flags = 0;
        anm_init(i_this, 0xD, 5.0f, 0, 1.0f, -1);
        i_this->mMode = 0x14;
      } else if (k == 1 || k == 8) {
        anm_init(i_this, 9, 5.0f, 0, 1.0f, -1);
        i_this->mMode = 0xB;
        i_this->speedF = 48.0f;
      } else if (k == 3) {
        anm_init(i_this, 8, 5.0f, 0, 1.0f, -1);
        i_this->mMode = 0xC;
        i_this->speedF = 32.0f;
      } else if (k == 4) {
        enemy_piyo_set_wz(i_this);
        wz_se(i_this, 0x50BC, 0, false);
        anm_init(i_this, 0xC, 5.0f, 0, 1.0f, -1);
        i_this->speedF = 28.0f;
        wz::state(i_this)->mTimers[0] = 0x4B;
        i_this->mMode = 0xD;
      } else if (k == 9) {
        anm_init(i_this, 0xC, 5.0f, 0, 1.0f, -1);
        i_this->speedF = 28.0f;
        i_this->mMode = 0xC;
      } else {
        anm_init(i_this, 9, 5.0f, 0, 1.0f, -1);
        i_this->mMode = 0xB;
        i_this->speedF = 32.0f;
      }
      eye = gabi::ea(&i_this->eyePos) != 0;
      if (i_this->mHitKind != 7 && i_this->health <= 0) {
        if (eye) {
          wz_monsSe(i_this, 0x48D9);
        }
        i_this->mMode = 0x28;
        if (i_this->mIsMiniBoss) {
          i_this->mAction = wz::ACTION_DEMO;
          i_this->mMode = 0x46;
          return;
        }
        return;
      }
      if (eye) {
        wz_monsSe(i_this, 0x48D7);
      }
      return;
    }
    case 11: {
      cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
      f32 v = i_this->speedF;
      if (std::fabs(v) < 0.2f) {
        stopDamage();
        return;
      }
      return;
    }
    case 12:
      cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
      if (wz_morfStopped(i_this)) {
        stopDamage();
        return;
      }
      return;
    case 13: {
      cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
      if (wz::state(i_this)->mTimers[0] != 0) {
        return;
      }
      i_this->mBodyCyl.mObjTg.mSPrm = (u32)i_this->mBodyCyl.mObjTg.mSPrm & ~1u;
      dCcD_GObjInf_ClrTgHit((&i_this->mBodyCyl));
      i_this->mAction = wz::ACTION_NORMAL;
      i_this->mMode = 6;
      return;
    }
    case 20: {
      if (!wz_morfStopped(i_this)) {
        return;
      }
      if (i_this->health > 0) {
        anm_init(i_this, 0xE, 0.0f, 0, 1.0f, -1);
        i_this->mMode = 0x15;
        return;
      }
      wz_monsSe(i_this, 0x48D9);
      i_this->mMode = 0x2A;
      if (!i_this->mIsMiniBoss) {
        return;
      }
      i_this->mAction = wz::ACTION_DEMO;
      i_this->mMode = 0x46;
      return;
    }
    case 21:
      if (!wz_morfStopped(i_this)) {
        return;
      }
      anm_init(i_this, 0xF, 0.0f, 0, 1.0f, -1);
      i_this->mMode = 0x16;
      return;
    case 22: {
      s16 a = (s16)(i_this->mAlpha - 8);
      if (a < 0) {
        i_this->mAlpha = 0;
      } else {
        i_this->mAlpha = a;
      }
      mDoExt_McaMorf *m = gabi::at<mDoExt_McaMorf>(i_this->m2B4.v);
      (void)m;
      if (!wz_morfStopped(i_this)) {
        return;
      }
      i_this->mAction = wz::ACTION_NORMAL;
      i_this->mMode = 6;
      i_this->mAlpha = 0;
      return;
    }
    case 0x28: {
      s16 py = player->shape_angle.y;
      wz::state(i_this)->mTargetAngle = py;
      i_this->current.angle.y = py;
      s16 a = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
      i_this->shape_angle.y = a;
      i_this->speedF = 40.0f;
      anm_init(i_this, 5, 5.0f, 0, 1.0f, -1);
      i_this->mMode = (u8)(i_this->mMode + 1);
    }
      [[fallthrough]];
    case 0x29:
      rod_size_set(i_this, 1);
      cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
      if (!wz_morfStopped(i_this)) {
        return;
      }
      i_this->mMode = (u8)(i_this->mMode + 1);
      i_this->speedF = 0.0f;
      [[fallthrough]];
    case 0x2A: {
      if (wz::state(i_this)->mHasActiveSummon == 0) {
        f32 y = dpos->y;
        y = i_this->mCurrBckIdx == 5 ? y + 160.0f : y + 20.0f;
        dpos->y = y;
        fopAcM_createDisappear(i_this, dpos, 10, 0, i_this->stealItemBitNo);
        fopAcM_delete(i_this);
        if (wz_isStartStage("kazeMB" /* "kazeMB" */)) {
          return;
        }
        setDeathSwitches();
        return;
      }
      s32 bck = i_this->mCurrBckIdx;
      i_this->mAlpha = 0;
      i_this->speedF = 0.0f;
      f32 y = dpos->y;
      y = bck == 5 ? y + 160.0f : y + 20.0f;
      dpos->y = y;
      fopAcM_createDisappear(i_this, dpos, 10, 0, i_this->stealItemBitNo);
      u32 s = i_this->actor_status;
      u32 tg = (u32)i_this->mBodyCyl.mObjTg.mSPrm;
      i_this->scale.x = 0.0f;
      i_this->scale.y = 0.0f;
      i_this->scale.z = 0.0f;
      i_this->actor_status = s & ~0x20u;
      i_this->mRodScaleX = 0.0f;
      i_this->mRodScaleY = 0.0f;
      i_this->mRodScaleZ = 0.0f;
      i_this->attention_info.flags = 0;
      i_this->mBodyCyl.mObjTg.mSPrm = tg & ~1u;
      dCcD_GObjInf_ClrTgHit((&i_this->mBodyCyl));
      i_this->mMode = (u8)(i_this->mMode + 1);
      return;
    }
    case 0x2B:
      if (wz::state(i_this)->mHasActiveSummon != 0) {
        return;
      }
      fopAcM_delete(i_this);
      if (wz_isStartStage("kazeMB")) {
        return;
      }
      setDeathSwitches();
      return;
    default:
      return;
    }
  };
  damageState();
  if (i_this->health > 0) {
    body_atari_check(i_this);
  }
}
/* inlined into daWZ_Execute */
static void action_tama_dousa(wz_class *i_this) {
  dComIfGp_get();
  gabi::Local<csXyz> ang;
  gabi::Local<cXyz> scl;
  ang->x = i_this->current.angle.x;
  ang->y = i_this->current.angle.y;
  ang->z = i_this->current.angle.z;
  scl->x = i_this->scale.x;
  scl->y = i_this->scale.y;
  scl->z = i_this->scale.z;
  u8 st = i_this->mMode;
  if (st == 0x64) {
    for (int i = 0; i < 4; i++) {
      wz::state(i_this)->mDemoSteps[i] = 0;
    }
    wz::state(i_this)->mBallRadius = 25.0f;
    cpy_u32(gabi::ea(&wz::state(i_this)->mParticlePos.x),
            gabi::ea(&i_this->current.pos.x));
    cpy_u32(gabi::ea(&wz::state(i_this)->mParticlePos.y),
            gabi::ea(&i_this->current.pos.y));
    cpy_u32(gabi::ea(&wz::state(i_this)->mParticlePos.z),
            gabi::ea(&i_this->current.pos.z));
    wz::state(i_this)->mParticleAngle.x = (s16)i_this->shape_angle.x;
    u8 type = i_this->mBehaviorType;
    wz::state(i_this)->mParticleAngle.y = (s16)i_this->shape_angle.y;
    wz::state(i_this)->mParticleAngle.z = (s16)i_this->shape_angle.z;
    if (type == 10 || type == 11) {
      u16 id0 = type == 10 ? 0x8238 : 0x8280;
      if (wz_cbEmitter(i_this, 0) == 0) {
        dPa_control_set(
            dComIfGp_getParticle(), 0, id0, &wz::state(i_this)->mParticlePos,
            &wz::state(i_this)->mParticleAngle, nullptr, 0xFF,
            reinterpret_cast<dPa_levelEcallBack *>(&i_this->mFollowCb[0]), -1,
            nullptr, nullptr, nullptr);
      }
      if (wz_cbEmitter(i_this, 1) == 0) {
        dPa_control_set(
            dComIfGp_getParticle(), 0, (u16)(id0 + 1),
            &wz::state(i_this)->mParticlePos,
            &wz::state(i_this)->mParticleAngle, nullptr, 0xFF,
            reinterpret_cast<dPa_levelEcallBack *>(&i_this->mFollowCb[1]), -1,
            nullptr, nullptr, nullptr);
      }
    }
    i_this->speedF = 45.0f;
    i_this->mMode = (u8)(i_this->mMode + 1);
    st = 0x65;
  }
  if (st == 0x65) {
    u32 flags;
    if (i_this->mBehaviorType == wz::SUMMON_BALL) {
      /* the summon ball flies to the summon point */
      wz_se(i_this, 0x7048, 0, false);
      f32 ty = i_this->mSummonPos.y + 300.0f;
      f32 dy = ty - i_this->current.pos.y;
      f32 dx = i_this->mSummonPos.x - i_this->current.pos.x;
      f32 dz = i_this->mSummonPos.z - i_this->current.pos.z;
      f32 d = std_sqrtf(gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy)));
      if (d < 50.0f) {
        i_this->speedF = 0.0f;
        i_this->mMode = 0x66;
        return;
      }
      flags = (u32)wz::view<wz::GroundFields>(i_this)->mAcchFlags;
    } else {
      wz_se(i_this, 0x7047, 0, false);
      cpy_u32(gabi::ea(&wz::state(i_this)->mParticlePos.x),
              gabi::ea(&i_this->current.pos.x));
      cpy_u32(gabi::ea(&wz::state(i_this)->mParticlePos.y),
              gabi::ea(&i_this->current.pos.y));
      cpy_u32(gabi::ea(&wz::state(i_this)->mParticlePos.z),
              gabi::ea(&i_this->current.pos.z));
      bool splash;
      bool inSea = daSea_ChkArea(i_this->current.pos.x, i_this->current.pos.z);
      if (inSea) {
        f32 h = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z) +
                50.0f;
        splash = i_this->current.pos.y < h;
        flags = 0;
      } else {
        flags = (u32)wz::view<wz::GroundFields>(i_this)->mAcchFlags;
        splash = false;
        if (flags & 0x1000) {
          f32 h = wz::view<wz::GroundFields>(i_this)->mWaterHeight + 50.0f;
          splash = i_this->current.pos.y < h;
        }
      }
      if (splash) {
        gabi::Local<cXyz> s2;
        s2->x = 2.0f;
        s2->y = 2.0f;
        s2->z = 2.0f;
        dPa_control_set(dComIfGp_getParticle(), 0, 0x35A, &i_this->current.pos,
                        &i_this->current.angle, s2, 0xFF, nullptr, -1, nullptr,
                        nullptr, nullptr);
        wz_followEnd2(i_this, 0, 1);
        fopAcM_delete(i_this);
      }
      if (splash || (flags & 0x1000) || inSea) {
        flags = (u32)wz::view<wz::GroundFields>(i_this)->mAcchFlags;
      }
    }
    if (flags & 0x20) {
      if (i_this->mBehaviorType != wz::SUMMON_BALL &&
          !dBgS_ChkMoveBG_wz(
              static_cast<cBgS_PolyInfo *>(&i_this->mAcch.m_gnd))) {
        wz::state(i_this)->mBallRadius = 40.0f;
        i_this->speedF = 0.0f;
        i_this->mMode = 0x66;
        return;
      }
      wz_tamaNext(i_this, i_this->current.pos.x, i_this->current.pos.y,
                  i_this->current.pos.z);
      return;
    }
    if (wz::state(i_this)->mTimers[2] == 0 || (flags & 0x10)) {
      wz_tamaNext(i_this, i_this->current.pos.x, i_this->current.pos.y,
                  i_this->current.pos.z);
      return;
    }
    if (((u32)i_this->m7D8.mGObjAt.mRPrm & 1) ||
        dCcD_GObjInf_ChkAtHit_wz((&i_this->m7D8))) {
      f32 z = i_this->m7D8.mGObjAt.mHitPos.z,
          x = i_this->m7D8.mGObjAt.mHitPos.x,
          y = i_this->m7D8.mGObjAt.mHitPos.y;
      wz_tamaNext(i_this, x, y, z);
      return;
    }
    if (dCcD_GObjInf_ChkTgHit((&i_this->m7D8))) {
      f32 z = i_this->m7D8.mGObjTg.mHitPos.z,
          x = i_this->m7D8.mGObjTg.mHitPos.x,
          y = i_this->m7D8.mGObjTg.mHitPos.y;
      wz_tamaNext(i_this, x, y, z);
    }
    return;
  }
  if (st == 0x66) {
    gabi::Local<cXyz> pos;
    cpy_u32(gabi::ea(pos.get()), gabi::ea(&i_this->current.pos.x));
    cpy_u32(gabi::ea(pos.get()) + 4, gabi::ea(&i_this->current.pos.y));
    cpy_u32(gabi::ea(pos.get()) + 8, gabi::ea(&i_this->current.pos.z));
    pos->y = wz::view<wz::GroundFields>(i_this)->mGroundHeight; /* the ground */
    ang->z = 0;
    u8 type = i_this->mBehaviorType;
    ang->x = 0;
    if (type == 10) {
      dPa_control_set(dComIfGp_getParticle(), 0, 0x823C, pos, ang, nullptr,
                      0xFF, nullptr, -1, nullptr, nullptr, nullptr);
      dPa_control_set(dComIfGp_getParticle(), 0, 0x823D, pos, ang, nullptr,
                      0xFF, nullptr, -1, nullptr, nullptr, nullptr);
      dPa_control_set(dComIfGp_getParticle(), 0, 0x823A, pos,
                      &i_this->current.angle, nullptr, 0xFF, nullptr, -1,
                      nullptr, nullptr, nullptr);
      if (wz::state(i_this)->mTimers[1] == 0) {
        wz::state(i_this)->mTimers[1] = (s16)gabi::ftoi(cM_rndF(5.0f));
        i_this->mBallFlareTimer = (s16)gabi::ftoi(cM_rndF(5.0f) + 8.0f);
      }
      if (!wz_isStartStage("sea" /* "sea" */)) {
        dKy_arrowcol_chg_on_wz(nullptr, 0);
      }
      i_this->actor_status |= 0x4000;
      wz::state(i_this)->mTimers[0] = 0x50;
      wz_se(i_this, 0x69DF, 0, false);
    } else if (type == 11) {
      /* the summon ball opens a summon door */
      gabi::Local<cXyz> s5;
      s5->y = 5.0f;
      s5->z = 5.0f;
      s5->x = 5.0f;
      cpy_u32(gabi::ea(pos.get()), gabi::ea(&i_this->current.pos.x));
      cpy_u32(gabi::ea(pos.get()) + 4, gabi::ea(&i_this->current.pos.y));
      cpy_u32(gabi::ea(pos.get()) + 8, gabi::ea(&i_this->current.pos.z));
      int n;
      for (n = 10; n != 0; n--) {
        s8 room = i_this->current.roomNo;
        if (fopAcM_createChild(0xD0, fopAcM_GetID(i_this), 0xFFFFFF0C, pos,
                               room, &i_this->current.angle, s5, 0,
                               0) != fpcM_ERROR_PROCESS_ID_e) {
          break;
        }
      }
      if (n == 0) {
        fopAc_ac_c *p = fopAcM_SearchByID(i_this->mParentId);
        if (p != nullptr) {
          static_cast<wz_class *>(p)->m470 = 0;
        }
      }
      u32 at = (u32)*wz::view<be<u32>>(&i_this->m7D8),
          tg = (u32)i_this->m7D8.mObjTg.mSPrm;
      *wz::view<be<u32>>(&i_this->m7D8) = at & ~1u;
      i_this->m7D8.mObjTg.mSPrm = tg & ~1u;
      dCcD_GObjInf_ClrTgHit((&i_this->m7D8));
      i_this->scale.x = 0.0f;
      i_this->scale.y = 0.0f;
      i_this->scale.z = 0.0f;
      wz::state(i_this)->mTimers[0] = (s16)((s16)l_debug.mBallWait + 0x1E);
    }
    wz_followEnd2(i_this, 0, 1);
    i_this->mMode = (u8)(i_this->mMode + 1);
    return;
  }
  auto playBallSound = [&]() {
    if (i_this->mBehaviorType == wz::FIRE_BALL &&
        gabi::ea(&i_this->eyePos) != 0) {
      mDoAud_seStart(0x614F, &i_this->eyePos, 0,
                     dComIfGp_getReverb(i_this->current.roomNo));
    }
  };
  if (st == 0x67) {
    s16 t;
    if (i_this->mBehaviorType == wz::FIRE_BALL) {
      i_this->shape_angle.y = (s16)(i_this->shape_angle.y + 0x100);
      t = wz::state(i_this)->mTimers[0];
      if (t > 10) {
        if (wz::state(i_this)->mTimers[1] != 0) {
          playBallSound();
          return;
        }
        wz::state(i_this)->mTimers[1] = (s16)gabi::ftoi(cM_rndF(5.0f));
        i_this->mBallFlareTimer = (s16)gabi::ftoi(cM_rndF(5.0f) + 8.0f);
        t = wz::state(i_this)->mTimers[0];
        if (t != 0) {
          playBallSound();
          return;
        }
        fopAcM_delete(i_this);
        return;
      }
      s16 c = i_this->mBallFlareTimer;
      if (c > 0) {
        i_this->mBallFlareTimer = (s16)(c - 1);
        t = wz::state(i_this)->mTimers[0];
      }
    } else {
      t = wz::state(i_this)->mTimers[0];
    }
    if (t == 0) {
      fopAcM_delete(i_this);
      return;
    }
    if (i_this->mBehaviorType == wz::FIRE_BALL &&
        gabi::ea(&i_this->eyePos) != 0) {
      mDoAud_seStart(0x614F, &i_this->eyePos, 0,
                     dComIfGp_getReverb(i_this->current.roomNo));
    }
    return;
  }
  if (st == 0x68) {
    u8 type = i_this->mBehaviorType;
    if (type == 10) {
      dPa_control_set(dComIfGp_getParticle(), 0, 0x823A, &i_this->m314,
                      &i_this->current.angle, nullptr, 0xFF, nullptr, -1,
                      nullptr, nullptr, nullptr);
    } else if (type == 11) {
      dPa_control_set(dComIfGp_getParticle(), 0, 0x8282, &i_this->m314,
                      &i_this->current.angle, nullptr, 0xFF, nullptr, -1,
                      nullptr, nullptr, nullptr);
      fopAc_ac_c *p = fopAcM_SearchByID(i_this->mParentId);
      if (p != nullptr) {
        static_cast<wz_class *>(p)->m470 = 0;
      }
    }
    wz_followEnd2(i_this, 0, 1);
    fopAcM_delete(i_this);
  }
}

/* 024E398C */
static BOOL daWZ_Execute(wz_class *i_this) {
  if (i_this->mBehaviorType < wz::FIRE_BALL &&
      i_this->mEnableSpawnSwitch != 0xFF &&
      !dComIfGs_isSwitch(i_this->mEnableSpawnSwitch, l_stayRoom)) {
    return TRUE;
  }
  if ((s16)l_debug.mHoldPartner == 0 &&
      (u8)wz::state(i_this)->mHasSummonedPartner != 0) {
    if (fopAcM_SearchByID(i_this->mParentId) == nullptr) {
      wz::state(i_this)->mHasActiveSummon = 0;
      wz::state(i_this)->mHasSummonedPartner = 0;
      i_this->mParentId = 0xFFFFFFFF;
    }
  }
  {
    u8 type = i_this->mBehaviorType;
    cpy_u32(gabi::ea(&i_this->mLight.mPos.x),
            gabi::ea(&i_this->current.pos.x)); /* the light follows */
    cpy_u32(gabi::ea(&i_this->mLight.mPos.y), gabi::ea(&i_this->current.pos.y));
    cpy_u32(gabi::ea(&i_this->mLight.mPos.z), gabi::ea(&i_this->current.pos.z));
    if (type == 10 || type == 11) {
      if (type == 10) {
        i_this->mLight.mColorR =
            (s16)gabi::ftoi((f32)l_debug.mFireRed + 300.0f);
        i_this->mLight.mColorG =
            (s16)gabi::ftoi((f32)l_debug.mFireGreen + 50.0f);
        i_this->mLight.mColorB = (s16)gabi::ftoi((f32)l_debug.mFireBlue);
      } else {
        i_this->mLight.mColorR =
            (s16)gabi::ftoi((f32)l_debug.mSummonRed + 300.0f);
        i_this->mLight.mColorG =
            (s16)gabi::ftoi((f32)l_debug.mSummonGreen + 300.0f);
        i_this->mLight.mColorB =
            (s16)gabi::ftoi((f32)l_debug.mSummonBlue + 20.0f);
      }
      i_this->mLight.mPower = (f32)l_debug.mLightPower + 550.0f;
      i_this->mLight.mFluctuation = (f32)l_debug.mLightFluctuation + 200.0f;
      type = i_this->mBehaviorType;
    }
    if (type < 10) {
      fopAcM_setGbaName_wz(i_this, 0x3C, 0x11, 0x2F);
      if (enemy_ice_wz(i_this->_A40)) {
        /* frozen: the models only follow the ice matrix */
        u32 model = morf_model(i_this->m2B4.v);
        f32 m[12];
        for (int k = 0; k < 12; k++) {
          m[k] = (f32)mDoMtx_stack_c::get()->m[k / 4][k % 4];
        }
        for (int k = 0; k < 12; k++) {
          gabi::at<J3DModel>(model)->mBaseTransformMtx.m[k / 4][k % 4] = m[k];
        }
        McaMorf_calc(gabi::at<mDoExt_McaMorf_c>(i_this->m2B4.v));
        if (i_this->mIsMiniBoss) {
          u32 blk = i_this->m2B4->getModel()->mpMtxBuffer.v;
          u32 model2 = morf_model(i_this->mpMorf2.v);
          u16 fl = (u16)gabi::at<J3DMtxBuffer_hd>(blk)->mFlags;
          u32 src = (u32)gabi::at<J3DMtxBuffer_hd>(blk)->mpAnmMtx + 0x390;
          gabi::at<J3DMtxBuffer_hd>(blk)->mFlags = (u16)(fl | 0x10);
          for (int k = 0; k < 12; k++) {
            m[k] = (f32)gabi::at<Mtx34>(src)->m[k / 4][k % 4];
          }
          for (int k = 0; k < 12; k++) {
            gabi::at<J3DModel>(model2)->mBaseTransformMtx.m[k / 4][k % 4] =
                m[k];
          }
          McaMorf_calc(gabi::at<mDoExt_McaMorf_c>(i_this->mpMorf2.v));
        }
        enemy_fire_remove(i_this->mEnemyFire);
        rod_size_set(i_this, 1);
        BG_check(i_this);
        return TRUE;
      }
    }
  }
  for (int i = 0; i < 5; i++) {
    s16 t = wz::state(i_this)->mTimers[i];
    if (t != 0) {
      wz::state(i_this)->mTimers[i] = (s16)(t - 1);
    }
  }

  switch (i_this->mAction) {
  case 0:
    action_dousa(i_this);
    break;
  case 1:
    action_itai(i_this);
    break;
  case 3:
    action_demo(i_this);
    break;
  case 100:
    action_tama_dousa(i_this);
    break;
  case 200:
    action_summon_dousa(i_this);
    break;
  default:
    break;
  }

  if (i_this->mBehaviorType < wz::FIRE_BALL) {
    if (i_this->mAction != wz::ACTION_DAMAGE &&
        i_this->mAction != wz::ACTION_DEMO) {
      cLib_addCalcAngleS2(&i_this->current.angle.y,
                          (s16)wz::state(i_this)->mTargetAngle, 1, 0x500);
      cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 1,
                          0x500);
    }
    wz_morfPlay(i_this);
  }
  {
    Mtx34 *m = calc_mtx();
    mDoMtx_YrotS(m, i_this->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    gabi::Local<cXyz> v;
    gabi::Local<cXyz> out;
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = i_this->speedF;
    MtxPosition(v, out);
    i_this->speed.x = out->x;
    if (i_this->mBehaviorType >= wz::FIRE_BALL) {
      i_this->speed.y = out->y;
    }
    f32 g = i_this->gravity;
    f32 sy = i_this->speed.y + g;
    i_this->speed.z = out->z;
    if (sy < -100.0f) {
      sy = -100.0f;
    }
    i_this->speed.y = sy;
  }
  fopAcM_posMove(i_this, nullptr);
  i_this->eyePos.copy(i_this->current.pos);
  if (i_this->mBehaviorType < wz::FIRE_BALL) {
    BG_check(i_this);
    if (i_this->mAction != wz::ACTION_DAMAGE &&
        i_this->mAction != wz::ACTION_DEMO && i_this->mMode != 7) {
      fuwafuwa_calc(i_this);
    }
    {
      f32 y = i_this->current.pos.y + 190.0f;
      f32 z = i_this->current.pos.z, x = i_this->current.pos.x;
      i_this->attention_info.position.z = z;
      i_this->attention_info.position.x = x;
      i_this->attention_info.position.y = y;
      f32 add = (f32)l_debug.mEyeHeight + 120.0f;
      i_this->eyePos.y = i_this->eyePos.y + add;
    }
    wz_setCylinderCenter(&i_this->mBodyCyl.mCyl,
                         &i_this->current.pos);           /* cM3dGCyl::SetC */
    wz_setCylinderHeight(&i_this->mBodyCyl.mCyl, 170.0f); /* SetH */
    wz_setCylinderRadius(&i_this->mBodyCyl.mCyl, 50.0f);  /* SetR */
    cCcS_Set_wz((&i_this->mBodyCyl));
  } else {
    u8 type = i_this->mBehaviorType;
    if (type != 12 && type != 13) {
      BG_check(i_this);
      wz_setSphereCenter(&i_this->m7D8.mSph,
                         &i_this->current.pos); /* cM3dGSph::SetC */
      wz_setSphereRadius(&i_this->m7D8.mSph,
                         (f32)wz::state(i_this)->mBallRadius); /* SetR */
      cCcS_Set_wz((&i_this->m7D8));
      dCcMassS_Mng_Set_wz((&i_this->m7D8), 1);
    }
  }
  draw_SUB(i_this);
  cpy_u32(gabi::ea(&wz::state(i_this)->mParticlePos.x),
          gabi::ea(&i_this->current.pos.x));
  cpy_u32(gabi::ea(&wz::state(i_this)->mParticlePos.y),
          gabi::ea(&i_this->current.pos.y));
  cpy_u32(gabi::ea(&wz::state(i_this)->mParticlePos.z),
          gabi::ea(&i_this->current.pos.z));
  wz::state(i_this)->mParticleAngle.x = (s16)i_this->shape_angle.x;
  wz::state(i_this)->mParticleAngle.y = (s16)i_this->shape_angle.y;
  wz::state(i_this)->mParticleAngle.z = (s16)i_this->shape_angle.z;
  return TRUE;
}

/* HD: compiler-generated and template code of this translation unit */

/* 024E78AC */
static void wz_SafeString_dt(void *p, s32 flags) {
  if (p != nullptr && (flags & 1)) {
    operator_delete(p);
  }
}

/* 024E9D00 */
static void wz_SafeString_empty(void *p) {}

/* 024E7818 */
static void __sinit_d_a_wz_cpp() {
  sinit_header_statics_z(gc_tu.headerStatics[0], gc_tu.headerStatics[1],
                         gc_tu.headerStatics[2]);
}

// HD: GHS enemyfire constructor.
/* 024E6984 */
static void *enemyfire_ct(void *p_) {
  wz::EnemyFire *fire = wz::view<wz::EnemyFire>(p_);
  if (fire == nullptr) {
    fire = static_cast<wz::EnemyFire *>(operator_new(sizeof(wz::EnemyFire)));
    if (fire == nullptr) {
      return nullptr;
    }
  }
  // HD: preserve the GHS null check on the inline subobject, including guest
  // wrapping.
  if (gabi::ea(fire->mInlineObject) == 0) {
    operator_new(0xC);
  }
  dCcD_Stts_ct(&fire->mStts);
  wz_constructSphere(&fire->mSphere);
  fire->mHDScale = 1.0f;
  return fire;
}

/* 024E6A10 */
static void *wz_class_ct(void *p_) {
  wz_class *i_this = static_cast<wz_class *>(p_);
  if (i_this == nullptr) {
    i_this = static_cast<wz_class *>(operator_new(sizeof(wz_class)));
    if (i_this == nullptr) {
      return nullptr;
    }
  }
  fopAc_ac_c_ct(i_this);
  i_this->__vtbl = wz::VT_ACTOR;
  dBgS_AcchCir_ct(&i_this->mAcchCir);
  wz_constructAcch(&i_this->mAcch);
  static_cast<cBgS_Chk *>(&i_this->mAcch)->__vtbl = wz::VT_ACCH_CHECK;
  static_cast<dBgS_GrpPassChk *>(&i_this->mAcch)->__vtbl = wz::VT_ACCH_GROUP;
  static_cast<dBgS_PolyPassChk *>(&i_this->mAcch)->__vtbl = wz::VT_ACCH_POLY;
  static_cast<dBgS_PolyPassChk *>(&i_this->mAcch)->mbObjThrough = 1;
  dCcD_Stts_ct(&i_this->mStts);
  dCcD_Cyl *body = &i_this->mBodyCyl;
  wz_constructGObjInf(body);
  wz::ShapeVtables *bodyShape = wz::view<wz::ShapeVtables>(body);
  bodyShape->mShapeVtable = wz::VT_SHAPE_BASE;
  bodyShape->mAabbVtable = wz::VT_AABB;
  wz_constructCylinderShape(&body->mCyl);
  body->__vtbl_hitinf = wz::VT_CYL;
  body->mCyl.__vtbl = wz::VT_CYL_GEOMETRY;
  bodyShape->mShapeVtable = wz::VT_SHAPE_CYL;
  wz_constructSphere(&i_this->m7D8);
  i_this->mLight.mHD20 = 1.0f;
  dCcD_Stts *stts2 = wz::view<dCcD_Stts>(i_this->mStts2);
  dCcD_Stts_ct(stts2);
  dCcD_Cyl *cyl2 = wz::view<dCcD_Cyl>(i_this->mCyl2);
  wz_constructGObjInf(cyl2);
  wz::ShapeVtables *shape = wz::view<wz::ShapeVtables>(cyl2);
  shape->mShapeVtable = wz::VT_SHAPE_BASE;
  shape->mAabbVtable = wz::VT_AABB;
  wz_constructCylinderShape(&cyl2->mCyl);
  shape->mShapeVtable = wz::VT_SHAPE_CYL;
  cyl2->mCyl.__vtbl = wz::VT_CYL_GEOMETRY;
  cyl2->__vtbl_hitinf = wz::VT_CYL;
  dBgS_AcchCir_ct(wz::view<dBgS_AcchCir>(i_this->mAcchCir2));
  dBgS_Acch *acch2 = wz::view<dBgS_Acch>(i_this->mAcch2);
  wz_constructAcch(acch2);
  static_cast<cBgS_Chk *>(acch2)->__vtbl = wz::VT_ACCH_CHECK;
  static_cast<dBgS_PolyPassChk *>(acch2)->__vtbl = wz::VT_ACCH_POLY;
  static_cast<dBgS_GrpPassChk *>(acch2)->__vtbl = wz::VT_ACCH_GROUP;
  static_cast<dBgS_PolyPassChk *>(acch2)->mbObjThrough = 1;
  enemyfire_ct(i_this->mEnemyFire);
  for (int i = 0; i < 5; i++) {
    dPa_followEcallBack_ct(&i_this->mFollowCb[i], 0, 0);
  }
  wz_constructInvisibleModel(i_this->mInvisModel);
  wz_constructInvisibleModel(i_this->mInvisModel2);
  return i_this;
}

/* 024E9BE0 */
static void wz_class_dt(void *p_, s32 flags) {
  wz_class *i_this = static_cast<wz_class *>(p_);
  if (i_this == nullptr) {
    return;
  }
  wz_destroyInvisibleModel(i_this->mInvisModel2, 2);
  wz_destroyInvisibleModel(i_this->mInvisModel, 2);
  wz::EnemyFire *fire = wz::view<wz::EnemyFire>(i_this->mEnemyFire);
  wz_destroySphere(&fire->mSphere, 2);
  dCcD_Stts_dt(&fire->mStts, 2);
  dBgS_Acch *acch2 = wz::view<dBgS_Acch>(i_this->mAcch2);
  static_cast<dBgS_GrpPassChk *>(acch2)->__vtbl = wz::VT_ACCH_GROUP;
  static_cast<dBgS_PolyPassChk *>(acch2)->__vtbl = wz::VT_ACCH_POLY;
  wz_destroyAcch(acch2, 0);
  wz_destroyAcchCircle(&wz::view<dBgS_AcchCir>(i_this->mAcchCir2)->m_cir, 2);
  dCcD_Cyl_dt(wz::view<dCcD_Cyl>(i_this->mCyl2), 2);
  dCcD_Stts_dt(wz::view<dCcD_Stts>(i_this->mStts2), 2);
  wz_destroySphere(&i_this->m7D8, 2);
  dCcD_Cyl_dt(&i_this->mBodyCyl, 2);
  dCcD_Stts_dt(&i_this->mStts, 2);
  static_cast<dBgS_GrpPassChk *>(&i_this->mAcch)->__vtbl = wz::VT_ACCH_GROUP;
  static_cast<dBgS_PolyPassChk *>(&i_this->mAcch)->__vtbl = wz::VT_ACCH_POLY;
  wz_destroyAcch(&i_this->mAcch, 0);
  wz_destroyAcchCircle(&i_this->mAcchCir.m_cir, 2);
  wz_destroyActor(i_this, 0);
  if (flags & 1) {
    operator_delete(p_);
  }
}
// clang-format off
/* WWHD glue
ext 02041570 void enemy_fire(void* fire)
ext 025E1AA4 void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 procId, u32 param, s32 reverb)
ext 025D5A20 fpc_ProcID fopAcM_createChild(s16 name, fpc_ProcID parent, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale, s8 subtype, u32 createFunc)
ext 027F3F8C wz::MaterialTable* wz_getMaterialTable(J3DModelData* data)
ext 0274D458 void wz_convertColor(wz::ColorF* out, wz::ColorF* in, f32 scale)
ext 027F9F0C wz::ColorF* wz_getMaterialColor(be<u32>* flags, u32 index)
ext 027F58E0 void wz_setTranslucent(J3DModel* model, u32 translucent)
ext 025E8BC4 void wz_invisibleEntry(wz::InvisibleModel* model)
ext 025E8EC0 void wz_invisibleEntryOpa(wz::InvisibleModel* model)
ext 0259138C void wz_iceEntryDL(mDoExt_McaMorf* morf, s32 index, u32 param)
ext 02591414 void wz_iceInvisibleDL(mDoExt_McaMorf* morf, s32 index, wz::InvisibleModel* model)
ext 025192A8 void wz_ccAtCheck(fopAc_ac_c* actor, wz::CcAtInfo* info)
ext 02041C30 void wz_removeFire(void* fire)
ext 025E1928 void wz_stopSubBgm()
ext 024EFF44 void wz_setAcchWall(void* circle, f32 height, f32 radius)
ext 025E4F64 u32 wz_newMcaMorf(void* self, void* model, void* node, void* material, void* animation, s32 loop, f32 speed, s32 start, s32 end, s32 modify, void* sound, u32 flags, u32 displayFlags)
ext 027F3F94 u32 wz_getJointTree(void* modelData)
ext 0200E814 s32 wz_getActorNameIndex(void* table, const char* name, s32 variant)
ext 0200BD2C void wz_constructCcStatus(void* status)
ext 02515DA0 void wz_constructGStatus(void* status)
ext 025166F0 void wz_constructSphere(void* sphere)
ext 024EFE94 void wz_constructAcchCircle(void* circle)
ext 024F0474 void wz_constructAcch(void* acch)
ext 02515FB8 void wz_constructGObjInf(void* object)
ext 02018590 void wz_constructCylinderShape(void* cylinder)
ext 025E895C void wz_constructInvisibleModel(void* model)
ext 025E89F8 void wz_destroyInvisibleModel(void* model, s32 flags)
ext 02515AE8 void wz_destroySphere(void* sphere, s32 flags)
ext 02515860 void wz_destroyCcStatus(void* status, s32 flags)
ext 024EFD9C void wz_destroyAcch(void* acch, s32 flags)
ext 02018034 void wz_destroyAcchCircle(void* circle, s32 flags)
ext 02515A70 void wz_destroyCylinder(void* cylinder, s32 flags)
ext 025D50BC void wz_destroyActor(void* actor, s32 flags)
ext 025E8A48 BOOL wz_createInvisibleModel(void* invisible, void* model)
ext 025E80D0 void* wz_constructBrk(void* brk)
ext 025E8154 BOOL wz_initBrk(void* brk, void* modelData, void* animation, s32 modify, s32 loop, f32 speed, s32 start, s32 end, s32 color, s32 alpha)
ext 025E7C6C void* wz_constructBtk(void* btk)
ext 025E7CE0 BOOL wz_initBtk(void* btk, void* modelData, void* animation, s32 modify, s32 loop, f32 speed, s32 start, s32 end, s32 texMtx, s32 flags)
ext 025162A4 BOOL wz_checkTargetHit(void* object)
ext 02516300 u32 wz_getTargetHitObject(void* object)
ext 0251621C void wz_clearTargetHit(void* object)
ext 02515E50 void wz_moveGStatus(void* status)
ext 025E535C void wz_playMorf(void* morf, void* position, s32 room, s32 reverb)
ext 027ED1F0 u32 wz_getTextureName(void* names, u32 index)
ext 027F81A4 u32 wz_getSystemTexture(void* textures, s32 index)
ext 02773680 u32 wz_constructTexture(void* texture, void* image)
ext 0273AD10 u32 wz_allocateTexture(u32 size)
ext 025AB070 u32 wz_getNextRoomPath(void* path, s32 room)
ext 02516518 void wz_setCylinder(void* cylinder, void* source)
ext 0251677C void wz_setSphere(void* sphere, void* source)
ext 024F06B4 void wz_setAcch(void* acch, cXyz* position, cXyz* oldPosition, fopAc_ac_c* actor, s32 flags, void* circle, cXyz* speed, void* angle, void* shapeAngle)
ext 02515F14 void wz_initCcStatus(void* status, s32 weight, s32 flags, fopAc_ac_c* actor)
ext 02514F2C void wz_stopCamera(void* camera)
ext 02514F38 void wz_startCamera(void* camera)
ext 02515280 void wz_setCameraTrim(void* camera, s32 trim)
ext 0251510C void wz_resetCamera(void* camera, cXyz* center, cXyz* eye)
ext 02514F88 void wz_setCamera(void* camera, cXyz* center, cXyz* eye, f32 fovy, s16 bank)
ext 025CB610 void wz_stopQuake(void* vibration, s32 id)
ext 025E1960 void wz_muteBgm(s32 frames)
ext 025E1918 void wz_startSubBgm(u32 id)
ext 025D7B24 BOOL wz_orderPotentialEvent(fopAc_ac_c* actor, u16 type, u16 flags, u16 param)
ext 020402C8 BOOL wz_updateIce(void* ice)
ext 02824890 void wz_setEmitterMatrix(void* matrix, void* rotation, void* scale, void* translation)
ext 024EEABC BOOL wz_checkMoveBg(void* background, void* polygon)
ext 025160DC BOOL wz_checkAttackHit(void* object)
ext 0255F3E8 void wz_enableArrowColor(void* actor, s32 param)
ext 025DA088 void wz_setGbaName(fopAc_ac_c* actor, u8 item, u8 name0, u8 name1)
ext 025D9D24 void wz_cancelCarry(fopAc_ac_c* actor)
ext 02041CAC void wz_setStun(fopAc_ac_c* actor)
ext 0200E240 void wz_registerCollision(void* collision, void* object)
ext 02516C14 void wz_registerMass(void* mass, void* object, u8 priority)
ext 020182E0 void wz_setCylinderCenter(void* cylinder, cXyz* center)
ext 02018428 void wz_setCylinderHeight(void* cylinder, f32 height)
ext 020184DC void wz_setCylinderRadius(void* cylinder, f32 radius)
ext 02018D40 void wz_setSphereCenter(void* sphere, cXyz* center)
ext 02018C8C void wz_setSphereRadius(void* sphere, f32 radius)
data l_summonNum 101D38B0
data l_summonName 101D35E0
data l_summonParam 101D36D0
data l_summonColor 101D35B8
data l_fireJoints 101D3AF8
data l_fireScales 101D3A90
data l_demoSignal 101D3594
data l_debug 1047B608
data l_systemTextures 101F9968
str 10042E43 "Wz" in 024E6420
str 10042E48 "Wzb" in 024E67FC
str 10042E38 "kazeMB" in 024E61E0
str 10042E34 "Wzb" in 024E61E0
str 10042E40 "Wz" in 024E61E0
str 10042D30 "kazeMB" in 024E9384
str 10042E8C "d_a_wz.cpp" in 024E6BEC
str 10042E80 "name != (0)" in 024E6BEC
str 10042E58 "__dummy" in 024E6BEC
str 10042E60 "Wzb" in 024E6BEC
str 10042E74 "Wz" in 024E6BEC
str 10042E64 "wiz_r" in 024E6BEC
str 10042E6C "wiz_s" in 024E6BEC
str 10042E78 "kazeMB" in 024E6BEC
str 10042D28 "kazeMB" in 024E7904
str 10042D20 "kazeMB" in 024E398C
str 10042D1C "sea" in 024E398C
data l_rodPrimColor 101D35C8
data l_rodEnvColor 101D35D4
data l_stayRoom 1047E6C8
str 10042DC8 "Wz" in 024E2A90
*/
// clang-format on
