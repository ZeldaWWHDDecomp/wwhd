/**
 * d_a_oq_exec.cpp (WWHD)
 * Enemy - Octorok: daOQ_Execute with its actions inlined.
 *
 * Written from the WWHD code (023C1380, 9.5 KB): the
 * GameCube decompilation has only "Nonmatching" stubs for action_dousa, action_kougeki,
 * action_tama_shoot, action_itai, moguru_check, Line_check and daOQ_Execute. GHS inlined them
 * into daOQ_Execute; they are written here as inline helpers keyed by the action (m3DF), with
 * the GameCube names where the behaviour fits (the split is a reconstruction).
 *
 * m3DF: 0xA wait/surface (dousa), 0xB attack (kougeki: spits a rock or, for m3DE != 0, a bomb),
 * 0x14 damage/death (itai), 0x1E the rock in flight (tama_shoot), 0x64 the spawner.
 */
#include "d/actor/d_a_oq.h"

/* calls to this unit's out-of-line functions (by address) */
static inline void anm_init_(oq_class* i_this, int bck, f32 morf, u8 loop, f32 speed, int bas) {
    gabi::call(0x023C0520, i_this, bck, morf, loop, speed, bas);
}
static inline BOOL body_atari_check_(oq_class* i_this) { return gabi::call<BOOL>(0x023C064C, i_this); }
static inline void shibuki_set_(oq_class* i_this, cXyz* pos, f32 scale) { gabi::call(0x023C0FDC, i_this, pos, scale); }
static inline BOOL sea_water_check_(oq_class* i_this) { return gabi::call<BOOL>(0x023C1080, i_this); }
static inline void search_y_check_(oq_class* i_this, s16 step) { gabi::call(0x023C1324, i_this, step); }
static inline void draw_SUB_(oq_class* i_this) { gabi::call(0x023C024C, i_this); }
static inline void BG_check_(oq_class* i_this) { gabi::call(0x023C4830, i_this); }
static inline void action_wakidasi_(oq_class* i_this) { gabi::call(0x023C49B4, i_this); }

static inline bool oq_octorok(u8 t) { return t <= 1 || (t >= 4 && t <= 5); }
static inline f32 oq_morf() { return REG_F(8, 3) + 15.0f; }
/* the "rise" animation: big kinds (1, 4, 5) 0x14, the small one 0x11 */
static inline void oq_anm_wait(oq_class* i_this) {
    u8 t = i_this->mType;
    if (t == 1 || t == 4 || t == 5) {
        anm_init_(i_this, 0x14, oq_morf(), 2, 1.0f, -1);
    } else {
        anm_init_(i_this, 0x11, oq_morf(), 2, 1.0f, -1);
    }
}
static inline void oq_particle(u16 id, cXyz* pos, csXyz* angle = nullptr, cXyz* scale = nullptr, dPa_levelEcallBack* cb = nullptr) {
    dPa_control_set(dComIfGp_getParticle(), 0, id, pos, angle, scale, 0xFF, cb, -1, nullptr, nullptr, nullptr);
}
static inline void oq_clear_m3F2(oq_class* i_this) {
    for (int i = 0; i < 6; i++) i_this->m3F2[i] = 0;
}
/* seStart with the actor test of the HD inline */
static inline void oq_se_a(oq_class* i_this, u32 id, u32 param) {
    if (i_this != nullptr) oq_se(i_this, id, param);
}
static inline void oq_mons_se_a(oq_class* i_this, u32 id) {
    if (i_this != nullptr) oq_mons_se(i_this, id);
}
static inline bool oq_isStop(oq_class* i_this) { return i_this->mpMorf->isStop() != 0; }
static inline void oq_follow_mtx(oq_class* i_this) {
    JPABaseEmitter* e = i_this->mFollow.mpEmitter;
    if (e != nullptr) {
        Mtx34* m = getAnmMtx(i_this->mpMorf->getModel(), 0);
        JPASetRMtxTVecfromMtx(m, gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C);
    }
}

/* m3DF 0xA: under water / surfacing (action_dousa) */
static inline void action_dousa(oq_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    bool near = false;
    f32 range;
    switch (i_this->m3E0) {
    case 0:
        oq_clear_m3F2(i_this);
        i_this->mTimer[4] = (s16)gabi::ftoi(REG_F(8, 11) + 300.0f);
        i_this->m3E3 = 1;
        i_this->m3E0 += 1;
        i_this->m438 = REG_F(8, 19) + 2.75f;
        /* fallthrough */
    case 1: {
        u8 t = i_this->mType;
        if (t == 4) {
            near = true;
            goto check_timer;
        }
        if (t == 0) {
            i_this->m438 = 1.0f;
            i_this->mTimer[4] = (s16)gabi::ftoi(REG_F(8, 11) + 300.0f);
            range = i_this->m3DE == 0 ? 2000.0f : 5000.0f;
        } else if (t == 1) {
            i_this->mTimer[4] = (s16)gabi::ftoi(REG_F(8, 11) + 300.0f);
            range = 5000.0f;
        } else if (t == 5) {
            range = (REG_F(12, 11) + 60.0f) * 100.0f;
        } else {
            JUT_ASSERT_fail(STR(0x10033E3C), 0x46C, STR(0x10033D56));
            range = 5000.0f;
        }
        if (!(fopAcM_searchPlayerDistance(i_this) < range)) {
            goto check_timer;
        }
        near = true;
        if (i_this->mType != 0) {
            goto check_timer;
        }
        if (fopAcM_searchPlayerDistance(i_this) < 400.0f) {
            goto end;
        }
        {
            f32 roof = i_this->mAcch.GetRoofHeight();
            if (roof != 1000000000.0f && roof < REG_F(8, 8) + 150.0f) {
                goto end;
            }
        }
    check_timer:
        if (i_this->mTimer[4] == 0 || near) {
            /* surface */
            i_this->m3E3 = 0;
            attn_flags_oq(i_this) = 4;
            i_this->actor_status |= 0x20;
            i_this->mBodyCoCyl.OnTgSPrmBit(1);
            i_this->mBodyCoCyl.OnCoSPrmBit(1);
            anm_init_(i_this, 0x13, oq_morf(), 0, 1.0f, -1);
            oq_se(i_this, 0x58C9, 0);
            i_this->m3E0 += 1;
        }
        if (i_this->m3E4 != 0) {
            goto del;
        }
        goto end;
    }
    case 2: {
        cLib_addCalc2(&i_this->scale.x, i_this->m438, 1.0f, 0.2f);
        f32 s = i_this->scale.x;
        i_this->scale.z = s;
        i_this->scale.y = s;
        if (!oq_isStop(i_this)) {
            goto end;
        }
        i_this->mBodyAtCyl.OnAtSPrmBit(1);
        s = i_this->m438;
        i_this->scale.x = s;
        i_this->scale.y = s;
        i_this->mBodyAtCyl.mObjAt.mRPrm = 1;
        i_this->scale.z = s;
        oq_anm_wait(i_this);
        i_this->m3E0 += 1;
        i_this->mTimer[0] = 0x3C;
        goto end;
    }
    case 3: {
        dComIfGp_get();
        u8 t = i_this->mType;
        if (t == 0) {
            range = i_this->m3DE == 0 ? 2000.0f : 5000.0f;
            if (i_this->mAcch.ChkWallHit()) {
                /* hit a wall while swimming: dies */
                oq_se_a(i_this, 0x2828, 0);
                oq_mons_se_a(i_this, 0x48CC);
                i_this->m3DF = 0x14;
                i_this->m3E0 = 0x1E;
                return;
            }
        } else if (t == 1 || t == 4) {
            range = 5000.0f;
        } else if (t == 5) {
            range = (REG_F(12, 13) + 65.0f) * 100.0f;
        } else {
            JUT_ASSERT_fail(STR(0x10033E30), 0x40E, STR(0x10033D54));
            range = 5000.0f;
        }
        if (i_this->m3E4 != 0 || fopAcM_searchPlayerDistance(i_this) > range) {
            i_this->m3E0 = 0xA;
            return;
        }
        t = i_this->mType;
        if (t == 0) {
            if (fopAcM_searchPlayerDistance(i_this) < 400.0f) {
                range = 400.0f;
                if (i_this->mAcch.ChkWaterIn()) {
                    i_this->m3E0 = 0xA;
                }
            } else {
                range = i_this->m3DE != 0 ? 4500.0f : 1800.0f;
                if (i_this->mTimer[0] == 0 && fopAcM_searchPlayerDistance(i_this) < range &&
                    std::fabs(i_this->current.pos.y - player->current.pos.y) < 200.0f) {
                    i_this->m3DF = 0xB;
                    i_this->m3E0 = 0x14;
                }
            }
        } else if (t == 1) {
            range = 4500.0f;
        } else if (t == 4) {
            if (i_this->mTimer[0] == 0) {
                i_this->m3DF = 0xB;
                i_this->m3E0 = 0x14;
            }
        } else if (t == 5) {
            range = (REG_F(12, 12) + 55.0f) * 100.0f;
        }
        t = i_this->mType;
        if (t != 1 && t != 5) {
            goto end;
        }
        if (fopAcM_searchPlayerDistance(i_this) < 1700.0f) {
            if (i_this->mAnmIdx == 0xC || i_this->mAnmIdx == 0x12) {
                goto end;
            }
            anm_init_(i_this, 0xC, oq_morf(), 2, 1.0f, -1);
            goto end;
        }
        if (i_this->mTimer[0] != 0 || !(fopAcM_searchPlayerDistance(i_this) < range)) {
            goto end;
        }
        i_this->m3DF = 0xB;
        i_this->m3E0 = 0x14;
        goto end;
    }
    case 0xA:
        /* dive */
        i_this->mBodyAtCyl.OffAtSPrmBit(1);
        i_this->mBodyCoCyl.OffAtSPrmBit(1);
        i_this->mBodyCoCyl.OffTgSPrmBit(1);
        i_this->mBodyCoCyl.OffCoSPrmBit(1);
        i_this->mBodyCoCyl.ClrTgHit();
        i_this->m3E3 = 0;
        attn_flags_oq(i_this) = 0;
        oq_se(i_this, 0x58CA, 0);
        anm_init_(i_this, 0x10, oq_morf(), 0, 1.0f, -1);
        i_this->m3E0 += 1;
        /* fallthrough */
    case 0xB:
        cLib_addCalc2(&i_this->m43C, -(REG_F(8, 6) + 600.0f), 1.0f, REG_F(8, 7) + 50.0f);
        if (i_this->m43C < -(REG_F(8, 6) + 599.0f)) {
            i_this->scale.x = 0.0f;
            i_this->scale.y = 0.0f;
            i_this->scale.z = 0.0f;
            i_this->actor_status &= ~0x20u;
            if (i_this->mType == 4 || i_this->mType == 5) {
                goto del;
            }
            i_this->m3E0 = 0;
            i_this->m43C = 0.0f;
        }
        goto end;
    default:
        goto end;
    }
del:
    fopAcM_delete(i_this);
end:
    i_this->m3FE = fopAcM_searchPlayerAngleY(i_this);
    if (i_this->m3E0 >= 3) {
        body_atari_check_(i_this);
    }
    sea_water_check_(i_this);
    search_y_check_(i_this, 0x500);
}

/* m3DF 0xB: attack (action_kougeki / action_tama_shoot) */
static inline void action_kougeki(oq_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    switch (i_this->m3E0) {
    case 0x14:
        oq_clear_m3F2(i_this);
        search_y_check_(i_this, 0x1000);
        i_this->m3E0 = 0x16;
        oq_se_a(i_this, 0x58CB, 0);
        if (i_this->m3E3 != 0) {
            if (i_this->current.pos.y > player->current.pos.y + 100.0f) {
                gabi::Local<cXyz> pos;
                pos->x = (f32)i_this->current.pos.x;
                pos->y = (f32)i_this->current.pos.y;
                pos->z = (f32)i_this->current.pos.z;
                shibuki_set_(i_this, pos, 1.5f);
            }
            i_this->m3E3 = 0;
            dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->mRipple);
        }
        if (i_this->m3DE == 0) {
            if (i_this->current.pos.y > player->current.pos.y + 100.0f) {
                break;
            }
            anm_init_(i_this, 6, oq_morf(), 0, 1.0f, -1);
            i_this->gravity = 0.0f;
            i_this->m3F2[0] = 1;
            i_this->m3E0 = 0x15;
        } else {
            /* jumps out of the water */
            i_this->gravity = -3.0f;
            i_this->speed.y = 40.0f;
            i_this->m3F2[0] = 1;
        }
        goto body;
    case 0x15:
        search_y_check_(i_this, 0x1000);
        cLib_addCalc2(&i_this->current.pos.y, player->current.pos.y + 30.0f, 1.0f, 30.0f);
        if (std::fabs(i_this->current.pos.y - (player->current.pos.y + 30.0f)) > 2.0f) {
            break;
        }
        i_this->m3E0 += 1;
        /* fallthrough */
    case 0x16:
        anm_init_(i_this, 7, 0.0f, 0, 1.0f, -1);
        i_this->m3E0 += 1;
        goto body;
    case 0x17:
        if (!i_this->mpMorf->checkFrame(1.0f)) {
            break;
        }
        if (REG_S(8, 5) == 0) {
            if (i_this->m3DE == 0) {
                /* spit a rock (type 6) */
                gabi::Local<cXyz> scale;
                scale->set(0.5f, 0.5f, 0.5f);
                fopAcM_create(PROC_OQ, 6, &i_this->m444, fopAcM_GetRoomNo(i_this), &i_this->current.angle, scale, 0, 0);
                gabi::Local<csXyz> angle;
                angle->x = (s16)i_this->current.angle.x;
                s16 y = i_this->current.angle.y;
                angle->y = y;
                angle->y = y + 0x4000;
                angle->z = (s16)i_this->current.angle.z;
                oq_particle(0x468, &i_this->m450, angle);
                oq_se(i_this, 0x58C7, 0);
            } else {
                /* spit a bomb */
                gabi::Local<cXyz> scale;
                f32 s = REG_F(8, 18) + 2.25f;
                scale->set(s, s, s);
                u32 prm = daBomb_prm_make(4, true, true);
                fopAc_ac_c* bomb = fopAcM_fastCreate(0x126 /* PROC_BOMB */, prm, &i_this->m444, fopAcM_GetRoomNo(i_this),
                                                     &i_this->current.angle, scale, -1, 0, nullptr);
                f32 dz = player->current.pos.z - i_this->current.pos.z;
                f32 dx = player->current.pos.x - i_this->current.pos.x;
                f32 dy = player->current.pos.y - i_this->current.pos.y;
                f32 d = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
                s16 angleX = -cM_atan2s(dy, d);
                if (!(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & 0x10000)) {
                    cM_rndFX(8000.0f); /* unused */
                }
                if (bomb == nullptr) {
                    JUT_ASSERT_fail(STR(0x10033E48), 0x5CA, STR(0x10033E20));
                } else {
                    bomb->current.angle.x = (s16)gabi::ftoi((f32)angleX + cM_rndFX(REG_F(12, 15) + 3000.0f));
                    bomb->speedF = REG_F(12, 16) + 40.0f;
                    bomb->speed.y = cM_rndF(REG_F(12, 17) + 30.0f) + 70.0f;
                }
                oq_se(i_this, 0x58C8, 0);
            }
        }
        i_this->m3E0 += 1;
        goto body;
    case 0x18:
        if (i_this->m3DE == 0 && i_this->m3F2[0] != 0) {
            i_this->gravity = -3.0f;
            i_this->speed.y = 30.0f;
        }
        i_this->m3E3 = 0;
        i_this->m3E0 += 1;
        /* fallthrough */
    case 0x19:
        if (i_this->mAnmIdx == 7) {
            if (!oq_isStop(i_this)) {
                break;
            }
            oq_anm_wait(i_this);
        }
        if (i_this->m3F2[0] != 0) {
            if (sea_water_check_(i_this)) {
                if (i_this->m3E3 == 0) {
                    break;
                }
                anm_init_(i_this, 8, oq_morf(), 0, 1.0f, -1);
                i_this->m3E0 = 0x1A;
                oq_se_a(i_this, 0x58CC, 0);
            } else {
                if (!i_this->mAcch.ChkGroundHit()) {
                    break;
                }
                anm_init_(i_this, 8, oq_morf(), 0, 1.0f, -1);
                i_this->m3E0 = 0x1A;
            }
        } else {
            oq_anm_wait(i_this);
            i_this->m3E0 = 0x1A;
        }
        goto body;
    case 0x1A: {
        sea_water_check_(i_this);
        if (i_this->m3F2[0] != 0 && !oq_isStop(i_this)) {
            break;
        }
        s16 wait = i_this->m3DE != 0 ? 0x64 : 0x3C;
        i_this->mTimer[0] = wait;
        if (i_this->mType == 1 || i_this->mType == 5) {
            wait = 0x64;
            i_this->mTimer[0] = 0x64;
        }
        f32 w = (f32)wait;
        i_this->mTimer[0] = (s16)gabi::ftoi(w + cM_rndF(w));
        i_this->speed.set(0.0f, 0.0f, 0.0f);
        u8 t = i_this->mType;
        if (t == 1 || t == 4 || t == 5) {
            if (i_this->mAnmIdx != 0x14) {
                anm_init_(i_this, 0x14, oq_morf(), 2, 1.0f, -1);
            }
        } else {
            if (i_this->mAnmIdx != 0x11) {
                anm_init_(i_this, 0x11, oq_morf(), 2, 1.0f, -1);
            }
        }
        i_this->m3DF = 0xA;
        i_this->m3E0 = 3;
        break;
    }
    }
    if (body_atari_check_(i_this)) {
        return;
    }
    goto check;
body:
    if (body_atari_check_(i_this)) {
        return;
    }
check:
    if (i_this->m3E0 >= 0x1A && i_this->mType == 0 && fopAcM_searchPlayerDistance(i_this) < 400.0f) {
        i_this->m3DF = 0xA;
        i_this->m3E0 = 0xA;
    }
}

/* m3DF 0x14: hit / dying (action_itai) */
static inline void action_itai(oq_class* i_this) {
    dComIfGp_get();
    switch (i_this->m3E0) {
    case 0x1E:
        oq_clear_m3F2(i_this);
        i_this->mBodyAtCyl.OffAtSPrmBit(1);
        i_this->mBodyCoCyl.OffTgSPrmBit(1);
        i_this->mBodyCoCyl.OffCoSPrmBit(1);
        i_this->mBodyCoCyl.ClrTgHit();
        i_this->gravity = 0.0f;
        i_this->speed.set(0.0f, 0.0f, 0.0f);
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->mRipple);
        anm_init_(i_this, 9, 0.0f, 0, 1.0f, -1);
        i_this->m3E0 += 1;
        break;
    case 0x1F:
        sea_water_check_(i_this);
        if (!oq_isStop(i_this)) {
            break;
        }
        anm_init_(i_this, 0xA, 0.0f, 2, 1.0f, -1);
        oq_se_a(i_this, 0x58CD, 0);
        if (i_this->mFollow.mpEmitter.get() == nullptr) {
            oq_particle(0x467, &i_this->current.pos, nullptr, nullptr, (dPa_levelEcallBack*)&i_this->mFollow);
        }
        /* spins left or right by the process id */
        if (i_this == nullptr) {
            i_this->m468.y = -0x1388;
        } else {
            u32 id = gabi::load<u32>(gabi::ea(i_this) + 4);
            i_this->m468.y = 0x1388;
            if (id & 1) {
                i_this->m468.y = -0x1388;
            }
        }
        i_this->mTimer[0] = 0xA;
        i_this->m3E3 = 0;
        i_this->m3E0 += 1;
        break;
    case 0x20:
        oq_follow_mtx(i_this);
        i_this->shape_angle.y += i_this->m468.y;
        i_this->gravity = 0.5f;
        if (i_this->mTimer[0] == 0) {
            anm_init_(i_this, 0xB, 0.0f, 0, 1.0f, -1);
            i_this->m3E0 += 1;
        }
        break;
    case 0x21:
        oq_follow_mtx(i_this);
        i_this->shape_angle.y += i_this->m468.y;
        if (!oq_isStop(i_this)) {
            break;
        }
        dPa_followEcallBack_end(&i_this->mFollow);
        if (i_this->mType == 5) {
            /* tell the spawner */
            fopAc_ac_c* parent = fopAcM_SearchByID(i_this->m440);
            if (parent != nullptr) {
                ((oq_class*)parent)->m404 += 1;
            }
        }
        if (i_this->mType == 1 || i_this->mType == 5) {
            s32 n = dSv_event_getEventReg(dComIfGs_getEvent_oq(), 0x7EFF) + 1;
            u8 v = (u8)n;
            if (n > 0xFF) v = 0xFF;
            dSv_event_setEventReg(dComIfGs_getEvent_oq(), 0x7EFF, v);
        }
        fopAcM_createDisappear(i_this, &i_this->eyePos, 0xA, 0, 0xFF);
        fopAcM_delete(i_this);
        i_this->m3E5 = 1;
        break;
    }
}

/* the rock hits something: splash of debris, gone */
static inline void tama_break(oq_class* i_this) {
    oq_se_a(i_this, 0x692C, 0);
    oq_particle(0xC, &i_this->current.pos);
    fopAcM_delete(i_this);
}

/* m3DF 0x1E: the rock in flight (type 6) */
static inline void action_tama(oq_class* i_this) {
    dComIfGp_get();
    u8 m = i_this->m3E0;
    if (m == 0x28) {
        oq_clear_m3F2(i_this);
        s16 i = (s16)gabi::ftoi(cM_rndF(3.99f));
        csXyz* spin = gabi::at<csXyz>(0x1046CAF8 + i * 6); /* the four spins from __sinit */
        i_this->m468.x = (s16)spin->x;
        i_this->m468.y = (s16)spin->y;
        i_this->m468.z = (s16)spin->z;
        i_this->mTimer[0] = 0xC8;
        i_this->m3E0 += 1;
    } else if (m == 0x29) {
        i_this->mTamaAtSph.OnAtSPrmBit(2);
        if (i_this->m3F2[0] == 0) {
            if (i_this->mTamaTgSph.mGObjAt.mRPrm & 1) {
                /* bounced off the shield */
                gabi::Local<cXyz> pos;
                cXyz* hit = &i_this->mTamaTgSph.mGObjAt.mHitPos;
                f32 y = hit->y;
                i_this->mTamaTgSph.OffAtSPrmBit(1);
                f32 z = hit->z;
                pos->y = y;
                i_this->mTimer[0] = 0xC8;
                pos->x = (f32)hit->x;
                pos->z = z;
                f32 sp = i_this->speedF;
                i_this->speed.set(0.0f, 0.0f, 0.0f);
                i_this->mTamaAtSph.OffAtSPrmBit(4);
                i_this->m3F2[0] = 1;
                i_this->speedF = -sp;
                oq_particle(0xC, pos);
                oq_se(i_this, 0x2855, 0x40);
            } else if (i_this->mTamaTgSph.ChkTgHit()) {
                void* obj = i_this->mTamaTgSph.GetTgHitObj();
                fopAc_ac_c* ac = dCcD_GAtTgCoCommonBase_GetAc(&i_this->mTamaTgSph.mGObjTg);
                if (obj != nullptr && gabi::load<u32>(gabi::ea(obj) + 0x10) == 2 /* sword */) {
                    /* hit back */
                    i_this->mTamaTgSph.OffAtSPrmBit(1);
                    gabi::Local<cXyz> pos;
                    cXyz* hit = &i_this->mTamaTgSph.mGObjTg.mHitPos;
                    pos->y = (f32)hit->y;
                    pos->z = (f32)hit->z;
                    i_this->mTimer[0] = 0xC8;
                    pos->x = (f32)hit->x;
                    oq_se_a(i_this, 0x2855, 0x40);
                    oq_particle(0xC, pos);
                    f32 sp = i_this->speedF;
                    i_this->speed.set(0.0f, 0.0f, 0.0f);
                    i_this->speedF = -sp;
                    i_this->mTamaAtSph.OffAtSPrmBit(4);
                    i_this->m3F2[0] = 1;
                } else {
                    if (ac != nullptr) {
                        s16 n1 = ac != nullptr ? fpcM_GetName(ac) : 0x7FFF;
                        s16 n2 = i_this != nullptr ? fpcM_GetName(i_this) : 0x7FFF;
                        if (n1 == n2 && ((oq_class*)ac)->mType == 6) {
                            fopAcM_delete(ac); /* two rocks */
                        }
                    }
                    tama_break(i_this);
                }
            } else if (i_this->mTamaAtSph.ChkAtHit()) {
                tama_break(i_this);
            }
        }
        if (i_this->mTimer[0] == 0) {
            i_this->mTamaAtSph.OffAtSPrmBit(1);
            i_this->mTamaTgSph.OffTgSPrmBit(1);
            i_this->mTamaTgSph.OffAtSPrmBit(1);
            i_this->mTamaTgSph.ClrTgHit();
            i_this->gravity = -3.0f;
            i_this->m3E0 += 1;
        }
    } else if (m == 0x2A) {
        cLib_addCalc0(&i_this->scale.x, 1.0f, 0.1f);
        f32 s = i_this->scale.x;
        i_this->scale.z = s;
        i_this->scale.y = s;
        if (s < 0.1f) {
            fopAcM_delete(i_this);
        }
    }
    csXyz_add(&i_this->shape_angle, &i_this->m468);
    if (daSea_ChkArea(i_this->current.pos.x, i_this->current.pos.z)) {
        f32 h = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z);
        if (i_this->current.pos.y < h + 40.0f) {
            gabi::Local<cXyz> pos;
            pos->z = (f32)i_this->current.pos.z;
            pos->x = (f32)i_this->current.pos.x;
            pos->y = h;
            shibuki_set_(i_this, pos, 0.4f);
            fopAcM_delete(i_this);
            return;
        }
    }
    /* Line_check: a wall ahead? */
    cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    gabi::Local<cXyz> offset;
    offset->set(0.0f, -40.0f, 80.0f);
    gabi::Local<cXyz> ahead;
    MtxPosition(offset, ahead);
    PSVECAdd(ahead, &i_this->current.pos, ahead);
    if (!(i_this->mAcch.m_flags & (dBgS_Acch::WALL_HIT | dBgS_Acch::GROUND_HIT))) {
        gabi::Local<cXyz> end;
        end->x = (f32)ahead->x;
        end->y = (f32)ahead->y;
        end->z = (f32)ahead->z;
        gabi::Local<dBgS_LinChk> linChk;
        dBgS_LinChk_ct(linChk, OQ_LINCHK_VT, false);
        gabi::Local<cXyz> start;
        start->x = (f32)i_this->current.pos.x;
        f32 y = i_this->current.pos.y;
        start->y = y;
        start->z = (f32)i_this->current.pos.z;
        start->y = y + 40.0f;
        dBgS_LinChk_Set(linChk, start, end, i_this);
        BOOL hit = cBgS_LineCross(dComIfG_Bgsp(), linChk);
        dBgS_LinChk_dt_oq(linChk);
        if (!hit) {
            return;
        }
    }
    oq_se(i_this, 0x692C, 0);
    oq_particle(0xC, &i_this->current.pos);
    fopAcM_delete(i_this);
}

/* 023C1380 */
static BOOL daOQ_Execute(oq_class* i_this) {
    WWHD_FUNC(0x023C1380, BOOL, i_this);
    fopAc_ac_c* a_this = i_this;
    if (i_this->mType == 1 || i_this->mType == 4 || i_this->mType == 5) {
        fopAcM_setGbaName(a_this, 0x2D, 0x41, 0x42);
    }
    if (oq_octorok(i_this->mType) && enemy_ice(&i_this->mEnemyIce)) {
        J3DModel_setBaseTRMtx(i_this->mpMorf->getModel(), mDoMtx_stack_c::get());
        i_this->mpMorf->calc();
        enemy_fire_remove(&i_this->mEnemyFire);
        return TRUE;
    }
    for (int i = 0; i < 6; i++) {
        if (i_this->mTimer[i] != 0) {
            i_this->mTimer[i]--;
        }
    }

    switch (i_this->m3DF) {
    case 0xA:
        action_dousa(i_this);
        break;
    case 0xB:
        action_kougeki(i_this);
        break;
    case 0x14:
        action_itai(i_this);
        break;
    case 0x1E:
        action_tama(i_this);
        break;
    case 0x64:
        action_wakidasi_(i_this);
        break;
    }

    u8 t = i_this->mType;
    if (t == 3 || t == 2 || i_this->m3E5 != 0) {
        return TRUE;
    }
    i_this->mpMorf->play(nullptr, 0, 0);
    t = i_this->mType;
    if (t == 0) {
        if (i_this->mAnmIdx == 0x11 && i_this->mAcch.ChkWaterIn() && i_this->mpMorf->checkFrame(18.0f)) {
            /* the water rings while waiting */
            gabi::Local<csXyz> angle;
            angle->x = (s16)a_this->current.angle.x;
            angle->y = (s16)a_this->current.angle.y;
            angle->z = (s16)a_this->current.angle.z;
            oq_particle(0x469, &a_this->current.pos, angle);
            angle->y = angle->y - 0x8000;
            oq_particle(0x469, &a_this->current.pos, angle);
        }
    } else if (t == 1 || t == 4 || t == 5) {
        /* the glow (brk frame) at night */
        if (dKy_daynight_check()) {
            f32 f = i_this->m434 + 1.0f;
            if (f > 89.0f) {
                i_this->m434 = 0.0f;
            } else {
                i_this->m434 = f;
            }
        } else {
            cLib_addCalc0(&i_this->m434, 1.0f, 10.0f);
        }
    }

    cMtx_YrotS(calc_mtx(), a_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), a_this->current.angle.x);
    gabi::Local<cXyz> vel;
    vel->x = 0.0f;
    vel->y = 0.0f;
    vel->z = (f32)a_this->speedF;
    gabi::Local<cXyz> out;
    MtxPosition(vel, out);
    a_this->speed.x = (f32)out->x;
    f32 sy = a_this->speed.y + a_this->gravity;
    a_this->speed.z = (f32)out->z;
    if (sy < -100.0f) {
        sy = -100.0f;
    }
    a_this->speed.y = sy;
    fopAcM_posMove(a_this, (i_this->mBodyCoCyl.mObjCo.mSPrm & 1) ? &i_this->mStts.m_cc_move : nullptr);
    BG_check_(i_this);

    t = i_this->mType;
    if (oq_octorok(t)) {
        cXyz* attn = gabi::at<cXyz>(gabi::ea(a_this) + 0x390); /* attention_info.position */
        f32 x = a_this->current.pos.x, z = a_this->current.pos.z, y = a_this->current.pos.y;
        attn->z = z;
        attn->x = x;
        a_this->eyePos.z = z;
        attn->y = y + 250.0f;
        a_this->eyePos.x = x;
        a_this->eyePos.y = y + 130.0f;
        i_this->mBodyCoCyl.SetC(&a_this->current.pos);
        if (i_this->mType == 1 || i_this->mType == 5) {
            z = a_this->current.pos.z;
            y = a_this->current.pos.y;
            attn->z = z;
            x = a_this->current.pos.x;
            a_this->eyePos.z = z;
            attn->x = x;
            attn->y = y + 550.0f;
            a_this->eyePos.y = y + 250.0f;
            a_this->eyePos.x = x;
            f32 atR = REG_F(8, 13) + 175.0f;
            if (gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & 0x10000) {
                if (fopAcM_searchPlayerDistance(a_this) < REG_F(8, 14) + 250.0f) {
                    if (i_this->m3F2[5] == 0) {
                        i_this->m3F2[5] = 1;
                        i_this->mTimer[5] = (s16)gabi::ftoi(REG_F(8, 15) + 30.0f);
                    }
                } else {
                    i_this->m3F2[5] = 0;
                }
            }
            f32 coR = i_this->m428;
            if (i_this->mTimer[5] != 0) {
                atR = REG_F(8, 16) + 250.0f;
            }
            i_this->mBodyAtCyl.SetC(&a_this->current.pos);
            i_this->mBodyAtCyl.SetH(600.0f);
            i_this->mBodyAtCyl.SetR(atR);
            dComIfG_Ccsp_Set(&i_this->mBodyAtCyl);
            i_this->mBodyCoCyl.SetH(600.0f);
            i_this->mBodyCoCyl.SetR(coR);
            dComIfG_Ccsp_Set(&i_this->mBodyCoCyl);
        } else {
            i_this->mBodyCoCyl.SetH(170.0f);
            i_this->mBodyCoCyl.SetR(80.0f);
            dComIfG_Ccsp_Set(&i_this->mBodyCoCyl);
        }
    } else if (t == 6) {
        f32 x = a_this->current.pos.x, y = a_this->current.pos.y, z = a_this->current.pos.z;
        a_this->eyePos.x = x;
        a_this->eyePos.y = y;
        a_this->eyePos.z = z;
        cXyz* attn = gabi::at<cXyz>(gabi::ea(a_this) + 0x390);
        attn->x = x;
        attn->y = y;
        attn->z = z;
        if (i_this->m3DE == 0) {
            i_this->mTamaAtSph.SetC(&a_this->current.pos);
            i_this->mTamaAtSph.SetR(i_this->m3F2[0] == 0 ? 15.0f : 40.0f);
            dComIfG_Ccsp_Set(&i_this->mTamaAtSph);
            i_this->mTamaTgSph.SetC(&a_this->current.pos);
            i_this->mTamaTgSph.SetR(60.0f);
            dComIfG_Ccsp_Set(&i_this->mTamaTgSph);
        }
    }
    draw_SUB_(i_this);
    return TRUE;
}
VERIFY(0x023C1380, daOQ_Execute);
