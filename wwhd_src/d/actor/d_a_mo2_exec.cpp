/**
 * d_a_mo2_exec.cpp (WWHD)
 * Enemy - Moblin: daMo2_Execute with its inlined helpers (damage_check, mo2_eye_tex_anm,
 * mo2_demo_camera, ground_smoke_set, yari_off_check).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_mo2.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_mo2.h"

/* ---- other mo2 functions: called by address ---- */
static void tex_anm_set(mo2_class* i, u16 idx) { gabi::call(0x021C6368, i, (u32)idx); }
static void anm_init(mo2_class* i, s32 bck, f32 morf, u8 loopMode, f32 speed, s32 snd) {
    gabi::call(0x021C6440, i, bck, morf, (u32)loopMode, speed, snd);
}
static void smoke_set_s(mo2_class* i, f32 rate) { gabi::call(0x021C6578, i, rate); }
static void wait_set(mo2_class* i) { gabi::call(0x021C8E30, i); }
static void path_check(mo2_class* i) { gabi::call(0x021C9004, i); }
static void Mo2_move(mo2_class* i) { gabi::call(0x021D0A14, i); } /* the matcher names it fight */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline BOOL enemy_ice(enemyice_l* e) { return gabi::call<BOOL>(0x020402C8, e); }
static inline void enemy_fire(enemyfire_l* e) { gabi::call(0x02041570, e); }
static inline s32 dr_damage_anime(damagereaction_l* d) { return gabi::call<s32>(0x02041F94, d); } /* damage_reaction */
static inline void dComIfGp_setNextStage(const char* stage, s16 point, s8 room, s8 layer, f32 speed, u32 mode, s32 setPoint, s8 wipe) {
    gabi::call(0x0252012C, stage, point, room, layer, speed, mode, setPoint, wipe);
}
static inline void fopAcM_orderPotentialEvent(fopAc_ac_c* a, u16 flag, u16 prio, u16 p3) { gabi::call(0x025D7B24, a, flag, prio, p3); }
/* HD controller checks (CPad_CHECK_TRIG_B(0) / CPad_CHECK_HOLD_Y(0)) */
static inline s32 CPad_CHECK_TRIG_B(s32 pad) { return gabi::call<s32>(0x020078BC, pad); }
static inline s32 CPad_CHECK_HOLD_Y(s32 pad) { return gabi::call<s32>(0x02007738, pad); }
/* CcAtInfo (unchanged from GameCube) */
struct CcAtInfo_l {
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
WWHD_SIZE(CcAtInfo_l, 0x1C);
static inline void def_se_set(fopAc_ac_c* a, void* obj, u32 mtrl) { gabi::call(0x02518CC8, a, obj, mtrl); }
static inline void at_power_check(CcAtInfo_l* info) { gabi::call(0x02518DB0, info); }
static inline u32 cc_at_check(fopAc_ac_c* a, CcAtInfo_l* info) { return gabi::call<u32>(0x025192A8, a, info); }
static inline void dKy_Sound_set(cXyz* pos, s32 a, u32 id, s32 b) { gabi::call(0x0255F458, pos, a, id, b); }
static inline void dCcMassS_Set(void* obj, u8 prio) { gabi::call(0x02516C14, gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), obj, (u32)prio); }
/* camera (HD) */
static inline void dCam_getCamera() { gabi::call(0x024F8020); }
static inline u8* dCam_getBody() { return gabi::call<u8*>(0x024F8044); }
static inline void dCamera_Set(u8* cam, cXyz* center, cXyz* eye, f32 fovy, s16 bank) { gabi::call(0x02514F88, cam, center, eye, fovy, bank); }
static inline void dCamera_SetTypeForce(u8* cam, const char* type, fopAc_ac_c* a) { gabi::call(0x02514EE4, cam, type, a); }
static inline void dCamera_SetTrimTypeForce(u8* cam, s32 t) { gabi::call(0x0251528C, cam, t); }
static inline void dCamera_SetTrimSize_l(u8* cam, s32 t) { gabi::call(0x02515280, cam, t); }
static inline void dCamera_Stop(u8* cam) { gabi::call(0x02514F2C, cam); }
static inline void dCamera_Start(u8* cam) { gabi::call(0x02514F38, cam); }
static inline void dMeter_mtrShow() { gabi::call(0x0259169C); }
static inline void mDoAud_seStart_4(u32 id, cXyz* pos, u32 param, s32 reverb) { gabi::call(0x025E1A40, id, pos, param, reverb); }
static inline void dVibration_StartShock(u8* vib, s32 strength, s32 flags, cXyz* dir) { gabi::call(0x025CB374, vib, strength, flags, dir); }
/* daPy_py_c::getCutType(): +0x3AC (HD) */
static inline u8 daPy_getCutType(fopAc_ac_c* p) { return gabi::load<u8>(gabi::ea(p) + 0x3AC); }
/* daBoko_c (HD offsets): setRotAngleSpeed +0x43A; moveStateInit stores angle.y, speed.y, speedF */
static inline void daBoko_setRotAngleSpeed(fopAc_ac_c* b, s16 s) { gabi::store<s16>(gabi::ea(b) + 0x43A, s); }

enum {
    AT_TYPE_HD_ICE_ARROW = 0x80000, AT_TYPE_HD_ICE_OR_LIGHT = 0x180000, AT_TYPE_HD_FIRE = 0x40200,
    JA_SE_CV_MO_DAMAGE = 0x4809, JA_SE_CV_MO_DAMAGE_HIP = 0x480A, JA_SE_CV_MO_FAINTED = 0x480E, JA_SE_MAJU_MO_CHECK = 0x905,
};

/* mo2_eye_tex_anm (inlined) */
static inline void mo2_eye_tex_anm(mo2_class* i_this) {
    if (i_this->m02E0 != 0) {
        i_this->m02E0--;
    } else {
        i_this->m02E0 = (s16)gabi::ftoi(cM_rndF(50.0f) + 10.0f);
        if (i_this->m02DD == 0) {
            tex_anm_set(i_this, 3);
        }
    }
    if (i_this->m02DD != 0) {
        if (i_this->m02DC < i_this->m02DE) {
            i_this->m02DC++;
        } else {
            i_this->m02DD = 0;
        }
    }
}

/* damage_check (inlined) */
static inline u8 damage_check(mo2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<CcAtInfo_l> atInfo;
    atInfo->mpObj = 0;
    atInfo->mDamage = 0;
    atInfo->mPlCutBit = 0;
    u8 iVar12 = 0;
    u8 iVar11 = 0;
    i_this->mDamageReaction.mStts.Move();
    dComIfG_Ccsp_Set(&i_this->mDefenseSph);
    i_this->mDefenseSph.SetC(&i_this->m2928);
    if (i_this->mDefenseSph.ChkTgHit()) {
        void* obj = i_this->mDefenseSph.GetTgHitObj();
        def_se_set(actor, obj, 0x41);
        cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
        gabi::Local<cXyz> local_48;
        local_48->x = 0.0f;
        local_48->y = 0.0f;
        local_48->z = -10.0f;
        MtxPosition(local_48, &i_this->mDamageReaction.m42C);
        i_this->mDamageReaction.m4D4 = -15.0f;
        gabi::Local<csXyz> local_68;
        local_68->x = player->shape_angle.x;
        s16 py = player->shape_angle.y;
        local_68->y = py;
        s16 pz = player->shape_angle.z;
        local_68->y = (s16)(py - 0x8000);
        local_68->z = pz;
        GXColor* k0 = gabi::at<GXColor>(gabi::ea(&actor->tevStr) + 0x98); /* tevStr.mColorK0 */
        JPABaseEmitter* emitter = dComIfGp_particle_set(0x2B /* ID_AK_JN_ELEMENTKIKUZU00 */, &i_this->m2928, local_68, nullptr,
                                                        0xFF, nullptr, -1, k0, k0, nullptr);
        if (emitter != nullptr) {
            JPA_setMaxFrame(emitter, 1);
            JPA_setRate(emitter, 10.0f);
            JPA_setSpread(emitter, 0.2f);
            gabi::store<f32>(gabi::ea(emitter) + 0x7C, 0.15f); /* setVolumeSweep */
            JPA_setGlobalParticleScale(emitter, REG_F(14, 16) + 0.85f);
        }
        return 0;
    }
    i_this->m2928.y = -10000.0f;
    i_this->mDefenseSph.SetR(-200.0f);
    if (i_this->m05B4 == 0 && (i_this->mHeadSph.ChkTgHit() || i_this->mTgCyl.ChkTgHit())) {
        i_this->m05A0 = 0;
        i_this->m2944 = -1;
        i_this->m294E = 0;
        i_this->m05B4 = 4; /* HD: 4 (GameCube REG0_S(7) + 5) */
        u32 obj;
        if (i_this->mHeadSph.ChkTgHit()) {
            iVar12 = 1;
            obj = gabi::ea(i_this->mHeadSph.GetTgHitObj());
            atInfo->mpObj = obj;
            atInfo->pParticlePos = gabi::ea(i_this->mHeadSph.GetTgHitPosP());
        } else if (i_this->mTgCyl.ChkTgHit()) {
            iVar12 = 2;
            obj = gabi::ea(i_this->mTgCyl.GetTgHitObj());
            atInfo->mpObj = obj;
            atInfo->pParticlePos = gabi::ea(i_this->mTgCyl.GetTgHitPosP());
        } else {
            obj = atInfo->mpObj;
        }
        /* HD: ChkAtType tests the object for NULL */
        if (obj != 0) {
            if (gabi::load<u32>(obj + 0x10) & AT_TYPE_HD_ICE_OR_LIGHT) {
                if (gabi::load<u32>(obj + 0x10) & AT_TYPE_HD_ICE_ARROW) {
                    i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
                    i_this->mEnemyIce.mFreezeDuration = REG0_S(3) + 300;
                    path_check(i_this);
                    wait_set(i_this);
                    i_this->mDamageReaction.mMode = -10;
                } else {
                    i_this->mEnemyIce.mLightShrinkTimer = 1;
                }
                enemy_fire_remove(&i_this->mEnemyFire);
                i_this->m05F0 = 0;
                dPa_smokeEcallBack_end_l(&i_this->m05F4);
                if (i_this->mbHasInnateWeapon != 0) {
                    i_this->mSpawnWeaponActor = 2;
                }
                i_this->m2A09 = 1;
                return 0;
            }
            if (gabi::load<u32>(obj + 0x10) & AT_TYPE_HD_FIRE) {
                i_this->m05B4 = 0x32;
                i_this->mEnemyFire.mFireDuration = REG0_S(2) + 100;
            }
        }
        s8 hp = actor->health;
        at_power_check(atInfo);
        if (atInfo->mResultingAttackType == 10 || atInfo->mResultingAttackType == 0xe) {
            actor->health = 0x14;
        }
        atInfo->mpActor = cc_at_check(actor, atInfo);
        if (atInfo->mResultingAttackType == 10 || atInfo->mResultingAttackType == 0xe) {
            actor->health = hp;
            if (atInfo->mResultingAttackType == 0xe && i_this->m2951 == 0) {
                i_this->m2951 = 1;
                i_this->mDamageReaction.mAction = ACTION_P_LOST;
                i_this->mDamageReaction.mMode = -10;
            }
        }
        gabi::Local<cXyz> pos;
        f32 px = actor->current.pos.x;
        pos->x = px;
        f32 py = actor->current.pos.y;
        pos->y = py;
        f32 pz = actor->current.pos.z;
        i_this->m05B6 = 0x19;
        pos->z = pz;
        dKy_Sound_set(pos, 100, fopAcM_GetID(actor), 5);
        /* HD: no l_mo2HIO.m007 health reset; type 0xC sets m05B4 */
        if (atInfo->mResultingAttackType == 0xC) {
            i_this->m05B4 = 0xA;
        }
        i_this->m2960 |= atInfo->mPlCutBit;
        if (atInfo->mResultingAttackType == 10) {
            i_this->m2A0B = REG_S(13, 3) + 8;
            atInfo->mDamage = 1;
            iVar11 = atInfo->mbDead != 0 ? 7 : 4;
            cMtx_YrotS(calc_mtx(), atInfo->m0C.y);
        } else {
            i_this->m2A0B = 0;
            if (atInfo->mResultingAttackType == 1) {
                s16 sVar3 = i_this->m05D6 - actor->current.angle.y;
                if (sVar3 < 0) {
                    sVar3 = -sVar3;
                }
                if (daPy_getCutType(player) == 5) {
                    iVar11 = 2;
                } else if ((u16)sVar3 > 0x4000) {
                    if (atInfo->mbDead != 0) {
                        iVar11 = 3;
                    } else if (iVar12 == 1) {
                        iVar11 = 4;
                    } else {
                        iVar11 = 6;
                    }
                } else if (atInfo->mbDead != 0) {
                    iVar11 = 1;
                } else if (iVar12 == 1) {
                    iVar11 = 4;
                } else {
                    iVar11 = 5;
                }
            } else if (atInfo->mResultingAttackType == 2) {
                iVar11 = 7;
                cMtx_YrotS(calc_mtx(), atInfo->m0C.y);
            } else {
                iVar11 = atInfo->mbDead != 0 ? 7 : 4;
                cMtx_YrotS(calc_mtx(), atInfo->m0C.y);
            }
        }
    }
    if (iVar11 != 0 && i_this->mMode == 1) {
        if (rouya_mode() != 0) {
            i_this->mDamageReaction.mAction = ACTION_NAGE;
            i_this->mDamageReaction.mMode = -10;
            iVar11 = 5;
            i_this->m05B4 = 0x32;
        } else {
            i_this->mMode = 0;
            i_this->m2A09 = 1;
        }
    }
    switch (iVar11) {
    case 1:
        tex_anm_set(i_this, 4);
        i_this->mDamageReaction.m424 |= 0x10;
        i_this->mDamageReaction.m428 = 26.0f;
        cMtx_YrotS(calc_mtx(), i_this->m05D6);
        break;
    case 2:
        tex_anm_set(i_this, 4);
        i_this->mDamageReaction.m424 |= 0x40;
        cMtx_YrotS(calc_mtx(), (s16)(actor->current.angle.y + 0x8000));
        i_this->mDamageReaction.m428 = 26.0f;
        break;
    case 3:
        tex_anm_set(i_this, 4);
        i_this->mDamageReaction.m424 |= 0x40;
        i_this->mDamageReaction.m428 = 26.0f;
        cMtx_YrotS(calc_mtx(), i_this->m05D6);
        break;
    case 4:
        tex_anm_set(i_this, 1);
        i_this->mDamageReaction.m428 = 23.0f;
        i_this->mDamageReaction.m424 |= 0x10;
        cMtx_YrotS(calc_mtx(), i_this->m05D6);
        break;
    case 5:
        tex_anm_set(i_this, 1);
        i_this->mDamageReaction.m424 |= 0x20;
        i_this->mDamageReaction.m428 = 23.0f;
        cMtx_YrotS(calc_mtx(), i_this->m05D6);
        anm_init(i_this, dRes_INDEX_MO2_BCK_PAOMUKE_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_PAOMUKE_e);
        i_this->mDamageReaction.m70E = 7;
        i_this->mDamageReaction.m48A = 10;
        i_this->mDamageReaction.m474 = 5000.0f;
        if (i_this->m05DA == 0) {
            i_this->m05DA = 3;
            i_this->m05E8.x = 0;
        }
        break;
    case 6:
        tex_anm_set(i_this, 1);
        i_this->m05B4 = 200;
        i_this->mDamageReaction.mAction = ACTION_HIP_DAMAGE;
        i_this->mDamageReaction.mMode = 0;
        if (i_this->m05DA == 0) {
            i_this->m05DA = 10;
            i_this->m05E8.x = -0x4000;
        }
        break;
    case 7:
        tex_anm_set(i_this, 4);
        i_this->mDamageReaction.m424 |= 0x10;
        i_this->mDamageReaction.m428 = 26.0f;
        break;
    }
    if (i_this->mDamageReaction.m424 != 0) {
        gabi::Local<cXyz> local_54;
        local_54->x = 0.0f;
        local_54->y = 0.0f;
        local_54->z = -10.0f;
        MtxPosition(local_54, &i_this->mDamageReaction.m42C);
        if (i_this->mDamageReaction.m428 < 25.0f) {
            i_this->mDamageReaction.m4D4 = -l_mo2HIO().m13C;
        } else {
            i_this->mDamageReaction.m428 = cM_rndF(10.0f) + 70.0f;
        }
    }
    if (iVar11 != 0) {
        if (actor->health <= 0 && atInfo->mbDead != 0) {
            fopAcM_monsSeStart(actor, JA_SE_CV_MO_FAINTED, 0);
            if (fopAcM_CheckStatus(actor, fopAcStts_BOSS_e)) {
                i_this->m2A1D = 0x32;
            }
        } else if (iVar11 == 6) {
            fopAcM_monsSeStart(actor, JA_SE_CV_MO_DAMAGE_HIP, 0);
        } else {
            fopAcM_monsSeStart(actor, JA_SE_CV_MO_DAMAGE, 0);
        }
    }
    return atInfo->mDamage;
}

/* mo2_demo_camera (inlined) */
static inline void mo2_demo_camera(mo2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    dComIfGp_get();
    dCam_getCamera();
    u8* camera = dCam_getBody();
    bool r28 = true;
    switch ((u8)i_this->m2A1D) {
    case 0:
        r28 = false;
        break;
    case 1:
    case 2: {
        mDoAud_seStart_4(JA_SE_MAJU_MO_CHECK, nullptr, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
        gabi::Local<cXyz> dir;
        dir->x = 0.0f;
        dir->y = 1.0f;
        dir->z = 0.0f;
        u8* vib = gabi::at<u8>(dComIfGp_ea() + PLAY_VIBRATION);
        dVibration_StartShock(vib, REG0_S(2) + 4, -0x21, dir);
        camera_mode() = 1;
        dCamera_SetTrimSize_l(camera, 1);
        i_this->m2A44 = 55.0f;
        i_this->m2A2C.copy(actor->current.pos);
        i_this->m2A1D = 3;
        i_this->m2A38 = REG0_F(8) + 200.0f;
        i_this->m2A3C = 0.0f;
    }
        /* fallthrough */
    case 3:
        r28 = false;
        dCamera_SetTypeForce(camera, STR(0x10014EF4) /* "Restrict" */, actor);
        dCamera_SetTrimTypeForce(camera, 1);
        break;
    case 10:
        dCamera_SetTrimSize_l(camera, 0);
        r28 = false;
        dMeter_mtrShow();
        i_this->m2A1D = 0;
        camera_mode() = 0;
        break;
    case 0x32:
        if (!eventInfo_checkCommandDemoAccrpt(actor)) {
            fopAcM_orderPotentialEvent(actor, 2 /* dEvtFlag_STAFF_ALL_e */, 0xFFFF, 0);
            eventInfo_onCondition(actor, 2);
            r28 = false;
            break;
        }
        dCamera_Stop(camera);
        dCamera_SetTrimSize_l(camera, 1);
        i_this->m2A1D = 0x33;
        {
            u32 cam2 = gabi::load<u32>(dComIfGp_ea() + 0x5AF8); /* dComIfGp_getCamera(0) */
            i_this->m2A20.copy(*gabi::at<cXyz>(cam2 + 0xDC));  /* view.mLookat.mEye */
            i_this->m2A2C.copy(*gabi::at<cXyz>(cam2 + 0xE8));  /* view.mLookat.mCenter */
        }
        i_this->m2A44 = 55.0f;
        i_this->m2A1E = 0;
        /* fallthrough */
    case 0x33:
        cLib_addCalc2(&i_this->m2A44, REG0_F(0xd) + 30.0f, 0.2f, REG0_F(0xe) + 0.4f);
        if (i_this->m2A1C == 0) {
            cLib_addCalc2(&i_this->m2A2C.x, actor->current.pos.x, 0.1f, 100.0f);
            cLib_addCalc2(&i_this->m2A2C.y, actor->current.pos.y + 230.0f + REG0_F(0xc), 0.1f, 100.0f);
            cLib_addCalc2(&i_this->m2A2C.z, actor->current.pos.z, 0.1f, 100.0f);
        }
        i_this->m2A1E++;
        if (i_this->m2A1E > 0x96) {
            dCamera_SetTrimSize_l(camera, 0);
            dCamera_Start(camera);
            dMeter_mtrShow();
            dComIfGp_event_reset();
            fopAcM_delete(actor);
            return;
        }
        break;
    }
    if (r28) {
        gabi::Local<cXyz> center;
        gabi::Local<cXyz> eye;
        *center = i_this->m2A2C.get();
        *eye = i_this->m2A20.get();
        dCamera_Set(camera, center, eye, i_this->m2A44, 0);
    }
}

/* ground_smoke_set (inlined) */
static inline void ground_smoke_set(mo2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->m05F0 == 0) {
        return;
    }
    i_this->m05F0--;
    if (i_this->m05F0 >= l_mo2HIO().m024) {
        gabi::Local<cXyz> sp8;
        sp8->x = 0.0f;
        sp8->y = 0.0f;
        i_this->m05E8.x = 0;
        i_this->m05E8.z = 0;
        MtxTrans(actor->current.pos.x, actor->current.pos.y + 7.5f, actor->current.pos.z, 0);
        if (i_this->m05F2 == 0) {
            sp8->z = -350.0f;
            cMtx_YrotM(calc_mtx(), i_this->m05EE);
            MtxPosition(sp8, &i_this->m05DC);
            i_this->m05E8.y = i_this->m05EE;
            smoke_set_s(i_this, 6.0f);
            i_this->m05EE = i_this->m05EE + REG0_S(7) + 2000;
        } else if (i_this->m05F2 == 1) {
            cMtx_YrotM(calc_mtx(), actor->current.angle.y);
            cMtx_YrotM(calc_mtx(), i_this->m05EE);
            sp8->z = -55.0f;
            MtxPosition(sp8, &i_this->m05DC);
            i_this->m05E8.y = i_this->m05EE;
            smoke_set_s(i_this, 6.0f);
            i_this->m05EE += 0x1FA0;
        } else if (i_this->m05F2 == 2) {
            MtxTrans(i_this->m2904.x, i_this->m2904.y + 7.5f, i_this->m2904.z, 0);
            cMtx_YrotM(calc_mtx(), i_this->m05EE);
            sp8->z = -12.5f;
            MtxPosition(sp8, &i_this->m05DC);
            i_this->m05E8.y = i_this->m05EE;
            smoke_set_s(i_this, 6.0f);
            i_this->m05EE += 0x2000;
        } else if (i_this->m05F2 == 3) {
            cMtx_YrotM(calc_mtx(), actor->current.angle.y);
            cMtx_YrotM(calc_mtx(), i_this->m05EE);
            sp8->z = -37.5f;
            MtxPosition(sp8, &i_this->m05DC);
            i_this->m05E8.y = i_this->m05EE;
            smoke_set_s(i_this, 2.0f);
            i_this->m05EE += 0x1FA0;
        } else if (i_this->m05F2 == 4) {
            cXyz* src = (i_this->m059C & 1) ? &i_this->mDamageReaction.m100[14] : &i_this->mDamageReaction.m100[15];
            f32 sy = src->y;
            i_this->m05DC.y = sy;
            f32 sz = src->z;
            f32 sx = src->x;
            i_this->m05DC.x = sx;
            i_this->m05DC.z = sz;
            if (i_this->mDamageReaction.m712 != 0) {
                i_this->m05DC.y = 512.5f;
            } else {
                i_this->m05DC.y = sy - 12.5f;
            }
            i_this->m05E8.y = cM_atan2s(actor->speed.x, actor->speed.z);
            smoke_set_s(i_this, 1.0f);
        }
    } else {
        i_this->m05DC.y = i_this->mDamageReaction.mSpawnY + 25000.0f;
    }
    if (i_this->m05F0 == 0) {
        dPa_smokeEcallBack_end_l(&i_this->m05F4);
        i_this->m05F3 = 0;
    }
}

/* yari_off_check (inlined) */
static inline void yari_off_check(mo2_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->mSpawnWeaponActor != 0) {
        i_this->m05AE = l_mo2HIO().m08A;
        PSMTXCopy(model_getAnmMtx((J3DModel_l*)i_this->mpMorf->getModel(), MO_JNT_MO_YARI_e), calc_mtx());
        gabi::Local<cXyz> local_48;
        local_48->x = 0.0f;
        local_48->y = 0.0f;
        local_48->z = 0.0f;
        gabi::Local<cXyz> cStack_54;
        MtxPosition(local_48, cStack_54);
        i_this->mWeaponPcId = fopAcM_create(fpcNm_BOKO_e, 4 /* Type_MOBLIN_SPEAR_e */, cStack_54, fopAcM_GetRoomNo(actor),
                                            nullptr, nullptr, -1, 0);
        i_this->mbThrowWeapon = 1;
        i_this->mSpawnWeaponActorMode = i_this->mSpawnWeaponActor;
        i_this->mSpawnWeaponActor = 0;
        i_this->mbHasInnateWeapon = 0;
    }
    if (i_this->mbThrowWeapon != 0) {
        fopAc_ac_c* boko = fopAcM_SearchByID(i_this->mWeaponPcId);
        if (boko != nullptr && i_this->mSpawnWeaponActorMode != 2) {
            daBoko_setRotAngleSpeed(boko, (s16)gabi::ftoi(cM_rndFX(2000.0f)));
            s16 angleY = actor->shape_angle.y + 0x8000;
            angleY = angleY + (s16)gabi::ftoi(cM_rndFX(8000.0f));
            f32 s1 = cM_rndF(10.0f) + 20.0f;
            f32 s2 = cM_rndF(10.0f) + 20.0f;
            /* daBoko_c::moveStateInit */
            boko->current.angle.y = angleY;
            boko->speed.y = s2;
            boko->speedF = s1;
            i_this->mbThrowWeapon = 0;
        }
    }
}

/* 021C9B1C */
static BOOL daMo2_Execute(mo2_class* i_this) {
    WWHD_FUNC(0x021C9B1C, BOOL, i_this);
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    /* HD: a Moblin with a death switch that fell far below its home sets the switch and goes away */
    if (i_this->mDeathSwitch != 0 && actor->current.pos.y < actor->home.pos.y - 5000.0f) {
        dComIfGs_onSwitch(i_this->mDeathSwitch, fopAcM_GetRoomNo(actor));
        fopAcM_onActor(actor);
        fopAcM_delete(actor);
        if (i_this->mbHasInnateWeapon != 0) {
            fopAc_ac_c* boko = fopAcM_SearchByID(i_this->mWeaponPcId);
            if (boko != nullptr) {
                fopAcM_delete(boko);
            }
        }
        return TRUE;
    }
    if (enemy_ice(&i_this->mEnemyIce)) {
        i_this->mpMorf->setPlayMode(J3DFrameCtrl::EMode_NONE);
        i_this->mpMorf->setPlaySpeed(3.0f);
        i_this->mpMorf->play(&actor->eyePos, 0, 0);
        J3DModel_setBaseTRMtx(i_this->mpMorf->getModel(), mDoMtx_stack_c::get());
        i_this->mpMorf->calc();
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &actor->current.pos, &actor->tevStr);
    enemy_fire(&i_this->mEnemyFire);
    if (i_this->m2A4B != 0 || i_this->m2A4C == 1) {
        dComIfGp_setNextStage(STR(0x10015118) /* "majroom" */, 0, 0, -1, 0.0f, 0, 1, 0);
        return TRUE;
    }
    if (i_this->m2A4C != 0) {
        i_this->m2A4C--;
        if (i_this->m2A4C == 0x1E) {
            mDoAud_seStop(JA_SE_MAJUTOU_ALERM, 30);
        }
    }
    /* HD: no m2970 / fell-4000-below-home deletion here */
    if (i_this->m2A4A != 0) {
        if (eventInfo_checkCommandDemoAccrpt(actor)) {
            i_this->m2A4A = 0;
        } else {
            fopAcM_orderPotentialEvent(actor, 2 /* dEvtType_OTHER_e */, 0xFFFF, 0);
            eventInfo_onCondition(actor, 2);
        }
    }
    if (i_this->m2A48 != 0) {
        i_this->m2A48--;
        if (i_this->m2A48 == 0) {
            if (camera_mode() == 0) {
                i_this->m2A1D = 1;
            }
            i_this->mDamageReaction.mAction = ACTION_E3_DEMO;
            i_this->mDamageReaction.mMode = 0;
        }
    }
    if (i_this->mDamageReaction.mAction == ACTION_JYUNKAI && daPy_checkGrabWear(player)) {
        if (i_this->mDamageReaction.mMode < 2) {
            l_mo2HIO().m02C = REG0_F(10) + 800.0f;
        } else {
            l_mo2HIO().m02C = REG0_F(0xb) + 1200.0f;
        }
        l_mo2HIO().m038 = 0x1932;
        l_mo2HIO().m040.x = 340.0f;
        l_mo2HIO().m040.z = 340.0f;
        l_mo2HIO().m04C = -200.0f;
    }
    u8 r23 = 0;
    if (l_mo2HIO().m006 == 0 || CPad_CHECK_TRIG_B(0) || CPad_CHECK_HOLD_Y(0)) {
        i_this->m059C++;
        for (s32 i = 0; i < 5; i++) {
            if (i_this->m05A4[i] != 0) {
                i_this->m05A4[i]--;
            }
        }
        if (i_this->m05BA != 0) {
            i_this->m05BA--;
        }
        if (i_this->m05BC != 0) {
            i_this->m05BC--;
        }
        if (i_this->m05AE != 0) {
            i_this->m05AE--;
        }
        if (i_this->m02E2 != 0) {
            i_this->m02E2--;
        }
        if (i_this->m05B0 != 0) {
            i_this->m05B0--;
            i_this->m05F0 = l_mo2HIO().m024 + 4;
            i_this->m05F2 = 4;
        }
        if (i_this->mDamageReaction.m48A != 0) {
            i_this->mDamageReaction.m48A--;
        }
        if (i_this->m05B4 != 0) {
            i_this->m05B4--;
        }
        if (i_this->m05B6 != 0) {
            i_this->m05B6--;
        }
        if (dComIfGs_isCollect(0, 0) || dComIfGs_isCollect(0, 1) || dComIfGs_isCollect(0, 2) || dComIfGs_isCollect(0, 3)) {
            rouya_mode() = 0;
        } else {
            rouya_mode() = 1;
        }
        if (i_this->m02C1 != 0) {
            if (dComIfGs_isSwitch(i_this->m02C1 - 1, fopAcM_GetRoomNo(actor))) {
                i_this->m02C1 = 0;
                attention_flags(actor) = fopAc_Attn_LOCKON_BATTLE_e;
            } else {
                attention_flags(actor) = 0;
                return TRUE;
            }
        } else {
            attention_flags(actor) &= ~0x10u; /* HD */
        }
        actor->actor_status |= 0x20; /* fopAcM_OnStatus(fopAcStts_SHOWMAP_e) */
        i_this->m0594 = 0;
        i_this->m0598 = 0;
        mo2_eye_tex_anm(i_this);
        i_this->mDamageReaction.m438 = 0;
        if (i_this->m05B2 == 0) {
            i_this->mpMorf->play(&actor->eyePos, 0, 0);
        }
        if (i_this->mDamageReaction.mAction < ACTION_CARRY) {
            i_this->mpMorf->calc();
        }
        r23 = damage_check(i_this);
        Mo2_move(i_this);
        mo2_demo_camera(i_this);
        ground_smoke_set(i_this);
        if (i_this->m05B2 != 0) {
            i_this->m05B2--;
        }
        i_this->mDamageReaction.mpEnemy = (fopEn_enemy_c*)actor;
        i_this->mDamageReaction.mEnemyType = 1; /* damagereaction::TYPE_MOBLIN */
    }
    if (i_this->mDamageReaction.mAction != ACTION_CARRY_DROP) {
        actor->shape_angle.x = actor->current.angle.x;
        actor->shape_angle.y = actor->current.angle.y;
        actor->shape_angle.z = actor->current.angle.z;
    }
    s32 r3 = dr_damage_anime(&i_this->mDamageReaction);
    if (r3 != 0) {
        i_this->m05F0 = 1;
        i_this->m2954 = fpcM_ERROR_PROCESS_ID_e;
        switch ((u32)r3) {
        case 1:
            anm_init(i_this, dRes_INDEX_MO2_BCK_PAOMUKE_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_PAOMUKE_e);
            if (i_this->mbHasInnateWeapon != 0 && (actor->health <= 0 || r23 >= 4 || cM_rndF(1.0f) < 0.5f)) {
                i_this->mSpawnWeaponActor = 1;
            }
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            break;
        case 2:
            anm_init(i_this, dRes_INDEX_MO2_BCK_PUTSUBUSE_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_PUTSUBUSE_e);
            break;
        case 5:
            if ((i_this->mMode != 1 || rouya_mode() == 0) &&
                (i_this->mDamageReaction.mAction != ACTION_FIGHT_RUN && i_this->mDamageReaction.mAction != ACTION_HUKKI &&
                 i_this->mDamageReaction.mAction != ACTION_P_LOST && i_this->m05A4[1] == 0)) {
                wait_set(i_this);
                i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
                i_this->mDamageReaction.mMode = 0;
                i_this->m05A4[1] = 0x1e;
            }
            i_this->m05DA = 5;
            i_this->m05E8.x = -0x4000;
            break;
        case 10:
            wait_set(i_this);
            i_this->mDamageReaction.mMode = 2;
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            path_check(i_this);
            break;
        case 0x14:
            i_this->m05DA = 1;
            i_this->m05E8.x = -0x4000;
            i_this->m05F2 = 1;
            i_this->m05F0 = l_mo2HIO().m024 + 0x10;
            i_this->mDamageReaction.m4C8[2] = l_mo2HIO().m088;
            break;
        case 0x15:
            i_this->m05DA = 1;
            i_this->m05E8.x = 0;
            i_this->mDamageReaction.m4C8[2] = l_mo2HIO().m088;
            break;
        case 0x1E:
            anm_init(i_this, dRes_INDEX_MO2_BCK_PAOMUKE_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_MO2_BAS_PAOMUKE_e);
            if (i_this->mbHasInnateWeapon != 0) {
                i_this->mSpawnWeaponActor = 1;
            }
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            if (actor != nullptr) {
                fopAcM_monsSeStart(actor, JA_SE_CV_MO_FAINTED, 0);
            }
            break;
        }
    }
    f32 sc = l_mo2HIO().m010;
    J3DModel* model = i_this->mpMorf->getModel();
    MtxScale(sc, sc, sc, 1);
    J3DModel_setBaseTRMtx(model, calc_mtx());
    if (i_this->mDamageReaction.mAction >= ACTION_CARRY) {
        i_this->mpMorf->calc();
    }
    enemy_fire(&i_this->mEnemyFire);
    yari_off_check(i_this);

    gabi::Local<cXyz> local_b8;
    local_b8->y = 0.0f;
    local_b8->x = 0.0f;
    /* HD: the rotation comes before the (resetting) translation */
    cMtx_YrotM(calc_mtx(), actor->current.angle.y);
    if (i_this->mDamageReaction.mAction == ACTION_D_DOZOU) {
        MtxTrans(actor->eyePos.x, actor->current.pos.y, actor->eyePos.z, 0);
        local_b8->z = REG_F(18, 11);
    } else {
        MtxTrans(actor->current.pos.x, actor->current.pos.y, actor->current.pos.z, 0);
        if (i_this->mDamageReaction.mAction == ACTION_D_MAHI) {
            local_b8->z = REG_F(10, 4) + -50.0f;
        } else {
            local_b8->z = 35.0f;
        }
    }
    gabi::Local<cXyz> cStack_c4;
    MtxPosition(local_b8, cStack_c4);
    i_this->mCoCyl.SetC(cStack_c4);
    dComIfG_Ccsp_Set(&i_this->mCoCyl);
    dCcMassS_Set(&i_this->mCoCyl, 3);
    gabi::Local<cXyz> local_d0;
    gabi::Local<cXyz> local_dc;
    f32 d0x = i_this->m28C8.x, d0y = i_this->m28C8.y, d0z = i_this->m28C8.z;
    f32 dcx = actor->current.pos.x, dcy = actor->current.pos.y, dcz = actor->current.pos.z;
    if (i_this->m05B4 != 0) {
        d0y -= 20000.0f;
        dcy -= 20000.0f;
    }
    if (i_this->mDamageReaction.mAction == ACTION_DEFENCE) {
        dcy -= 100.0f;
        d0y -= 20000.0f;
    }
    local_d0->x = d0x;
    local_d0->y = d0y;
    local_d0->z = d0z;
    local_dc->x = dcx;
    local_dc->y = dcy;
    local_dc->z = dcz;
    i_this->mTgCyl.SetC(local_dc);
    dComIfG_Ccsp_Set(&i_this->mTgCyl);
    i_this->mHeadSph.SetC(local_d0);
    dComIfG_Ccsp_Set(&i_this->mHeadSph);
    if (i_this->m05DA != 0) {
        i_this->m05DA--;
        if (i_this->m05DA == 0) {
            i_this->m05E8.y = actor->current.angle.y;
            dComIfGp_particle_set(0xE /* ID_AK_JN_TUBA00 */, &i_this->m28C8, &i_this->m05E8);
        }
    }
    if (i_this->mDamageReaction.mAcch.ChkGroundHit()) {
        gabi::Local<dBgS_GndChk_l> gndChk;
        dBgS_GndChk_ct(gndChk);
        s16 r22 = 0x7FFF;
        s16 r21 = 0x7FFF;
        f32 f31 = i_this->mDamageReaction.m480 != 0 ? 100.0f : 10.0f;
        f32 px = actor->current.pos.x;
        f32 py = actor->current.pos.y + (50.0f - i_this->mDamageReaction.m44C.y);
        f32 pz = actor->current.pos.z;
        gndChk->m_pos.x = px;
        gndChk->m_pos.y = py;
        gndChk->m_pos.z = pz;
        py = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
        if (py != -1000000000.0f) {
            f32 tz = pz + f31;
            f32 ty = py + 50.0f;
            gndChk->m_pos.x = px;
            gndChk->m_pos.z = tz;
            gndChk->m_pos.y = ty;
            f32 f1 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
            if (f1 != -1000000000.0f) {
                r22 = -cM_atan2s(f1 - py, tz - pz);
                if (r22 > 0x2000 || r22 < -0x2000) {
                    r22 = 0;
                }
            }
            f32 tx = px + f31;
            gndChk->m_pos.y = ty;
            gndChk->m_pos.z = pz;
            gndChk->m_pos.x = tx;
            f1 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
            if (f1 != -1000000000.0f) {
                r21 = cM_atan2s(f1 - py, tx - px);
                if (r21 > 0x2000 || r21 < -0x2000) {
                    r21 = 0;
                }
            }
        }
        s16 maxSpeed, speedRatio;
        if (i_this->mDamageReaction.m480 != 0) {
            maxSpeed = 0x400;
            speedRatio = 1;
        } else {
            maxSpeed = 0x100;
            speedRatio = 8;
        }
        if (r22 != 0x7FFF) {
            cLib_addCalcAngleS2(&i_this->mDamageReaction.m48C.x, r22, speedRatio, maxSpeed);
        }
        if (r21 != 0x7FFF) {
            cLib_addCalcAngleS2(&i_this->mDamageReaction.m48C.z, r21, speedRatio, maxSpeed);
        }
        dBgS_GndChk_dt(gndChk);
    }
    if (i_this->m2944 >= 0) {
        i_this->m2944++;
        if (i_this->m2944 == i_this->m2946) {
            i_this->m294E = i_this->m2948;
        }
        if (i_this->m2944 == i_this->m294A) {
            i_this->m294E = i_this->m294C;
        }
        if (i_this->m2944 > 200) {
            i_this->m2944 = -1;
        }
    }
    /* setBtAttackData(0, 10, 10000, type); setBtMaxDis(m140) */
    i_this->mBtStartFrame = 0.0f;
    i_this->mBtEndFrame = 10.0f;
    i_this->mBtMaxDis = 10000.0f;
    i_this->mBtAttackType = i_this->mParryOpeningType;
    i_this->mBtMaxDis = l_mo2HIO().m140;
    if (i_this->m294E != 0) {
        i_this->m294E--;
        i_this->mBtNowFrame = 5.0f;
    } else {
        i_this->mBtNowFrame = 100.0f;
    }
    return TRUE;
}
VERIFY(0x021C9B1C, daMo2_Execute);
