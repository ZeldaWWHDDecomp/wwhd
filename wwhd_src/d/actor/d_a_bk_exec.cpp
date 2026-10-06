/**
 * d_a_bk_exec.cpp (WWHD)
 * Enemy - Bokoblin: daBk_Execute (inlines waki_set, bk_eye_tex_anm, damage_check, demo_camera,
 * yari_off_check).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bk_local.h"

/* functions of d_a_bk.cpp (weak guest-call stubs so this part links alone) */
__attribute__((weak)) void anm_init(bk_class* a, int b, f32 m, u8 l, f32 s, int f) { gabi::call(0x02098DA4, a, b, m, l, s, f); }
__attribute__((weak)) void wait_set(bk_class* a) { gabi::call(0x0209B264, a); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline be<u8>& another_hit() { return *gabi::at<be<u8>>(0x1046232C); }
static inline fopAc_ac_c* ken() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(0x1046231C)); }
/* dComIfGp_roomControl_getStayNo: s8 at 0x1047E6C8 */
static inline s8 dComIfGp_roomControl_getStayNo() { return gabi::load<s8>(0x1047E6C8); }
/* CPad_CHECK_TRIG_B(0) / CPad_CHECK_HOLD_Y(0) (out of line in HD) */
static inline BOOL CPad_CHECK_TRIG_B(s32 port) { return gabi::call<BOOL>(0x020078BC, port); }
static inline BOOL CPad_CHECK_HOLD_Y(s32 port) { return gabi::call<BOOL>(0x02007738, port); }
static inline BOOL enemy_ice(enemyice_l* e) { return gabi::call<BOOL>(0x020402C8, e); }
static inline void enemy_fire(enemyfire_l* e) { gabi::call(0x02041570, e); }
/* c_damagereaction: damage_reaction (the matcher calls 02041F94 dr_damage_anime) */
static inline s32 damage_reaction(damagereaction_l* dr) { return gabi::call<s32>(0x02041F94, dr); }
static inline void fopAcM_setCarryNow(fopAc_ac_c* a, BOOL b) { gabi::call(0x025D9D0C, a, b); }
static inline void fopAcM_orderPotentialEvent(fopAc_ac_c* a, u16 flag, u16 p2, u16 p3) { gabi::call(0x025D7B24, a, flag, p2, p3); }
/* out-of-line TU copies */
static inline fopAc_ac_c* fopAcM_SearchByID_ool(u32 id) { return gabi::call<fopAc_ac_c*>(0x020A0634, id); }
static inline void fopAcM_monsSeStart_ool(fopAc_ac_c* a, u32 id, u32 prm) { gabi::call(0x020A0670, a, id, prm); }
/* mDoAud_monsSeStart(id, pos, procId, param, reverb) */
static inline void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 pid, u32 prm, s32 reverb) { gabi::call(0x025E1AA4, id, pos, pid, prm, reverb); }
/* fopAcM_monsSeStart, inline (HD: eyePos null check) */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 prm) {
    if (gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        mDoAud_monsSeStart(id, &a->eyePos, pid, prm, dComIfGp_getReverb(room));
    }
}
static inline void mDoAud_seStop(u32 id, s32 t) { gabi::call(0x025E1AE0, id, t); }
static inline void mDoAud_bgmAllMute(s32 t) { gabi::call(0x025E1960, t); }
static inline void mDoAud_subBgmStart(u32 id) { gabi::call(0x025E1918, id); }
static inline void JUTReport(s32 x, s32 y, const char* fmt, s32 v) { gabi::call(0x027EC9E8, x, y, fmt, v); }
static inline void dKy_Sound_set(cXyz* pos, s32 p, u32 pid, s32 t) { gabi::call(0x0255F458, pos, p, pid, t); }
static inline void def_se_set(fopAc_ac_c* a, void* obj, u32 p) { gabi::call(0x02518CC8, a, obj, p); }
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
static inline void at_power_check(CcAtInfo_l* i) { gabi::call(0x02518DB0, i); }
static inline u32 cc_at_check(fopAc_ac_c* a, CcAtInfo_l* i) { return gabi::call<u32>(0x025192A8, a, i); }
static inline u32 CcObj_AtType(u32 obj) { return gabi::load<u32>(obj + 0x10); }
static inline void dSmokeEcallBack_end(dPa_smokeEcallBack_l* cb) { gabi::call(0x025A5F88, cb); }
static inline void* GetCoHitObj(dCcD_GObjInf* o) { return gabi::call<void*>(0x025163BC, o); }
static inline void dCcMassS_Set(void* obj, u8 prio) { gabi::call(0x02516C14, gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), obj, prio); }
/* camera_process_class: dCamera_c at +0x248; view eye +0xDC, center +0xE8 */
static inline u32 dComIfGp_getCamera(s32 id) { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8 + 0x34 * id); }
static inline s8 dComIfGp_getPlayerCameraID(int i) { return gabi::load<s8>(dComIfGp_ea() + PLAY_PLAYER + 4 + 8 * i); }
static inline void dCam_Stop(u32 cam) { gabi::call(0x02514F2C, cam + 0x248); }
static inline void dCam_Start(u32 cam) { gabi::call(0x02514F38, cam + 0x248); }
static inline void dCam_SetTrimSize(u32 cam, s32 s) { gabi::call(0x02515280, cam + 0x248, s); }
static inline void dCam_Reset(u32 cam, cXyz* center, cXyz* eye) { gabi::call(0x0251510C, cam + 0x248, center, eye); }
static inline void dCam_Set(u32 cam, cXyz* center, cXyz* eye, f32 fovy, s16 bank) { gabi::call(0x02514F88, cam + 0x248, center, eye, fovy, bank); }
/* daPy_py_c virtuals (vtable at +0xB4) and demo fields */
static inline void daPy_vcall_setPlayerPosAndAngle(fopAc_ac_c* p, cXyz* pos, s16 ang) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 0xB4);
    gabi::call_ptr(gabi::load<u32>(vt + 0x114), p, pos, ang);
}
static inline void daPy_vcall_voiceStart(fopAc_ac_c* p, u32 id) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 0xB4);
    gabi::call_ptr(gabi::load<u32>(vt + 0xE4), p, id);
}
static inline u8 daPy_getCutType(fopAc_ac_c* p) { return gabi::load<u8>(gabi::ea(p) + 0x3AC); }
/* daBoko_c: model at +0x3B4; setMatrix copies to the model's base matrix (HD: null check) */
static inline void daBoko_setMatrix(fopAc_ac_c* boko, Mtx34* m) {
    J3DModel* model = gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(boko) + 0x3B4));
    if (model != nullptr) J3DModel_setBaseTRMtx(model, m);
}
/* J3DModel::getAnmMtx (HD matrix block, marks it dirty) */
static inline Mtx34* J3DModel_getAnmMtx(J3DModel* model, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + 0x30 * jnt);
}
/* dPath / dPnt */
struct dPnt_ex {
    /* 0x0 */ u8 _0[4];
    /* 0x4 */ cXyz m_position;
};
static inline u16 path_num(bk_class* i_this) { return gabi::load<u16>(gabi::ea(i_this->ppd.get())); }
static inline dPnt_ex* path_points(bk_class* i_this) { return gabi::at<dPnt_ex>(gabi::load<u32>(gabi::ea(i_this->ppd.get()) + 8)); }
/* fopAcM_prm_class */
struct fopAcM_prm_l {
    /* 0x00 */ be<u32> parameters;
    /* 0x04 */ cXyz position;
    /* 0x10 */ csXyz angle;
    /* 0x16 */ u8 _16[0x21 - 0x16];
    /* 0x21 */ be<s8> room_no;
};
static inline fopAcM_prm_l* fopAcM_CreateAppend() { return gabi::call<fopAcM_prm_l*>(0x025D5600); }
static inline void fopAcM_Create(s16 name, fopAcM_prm_l* p) {
    u32 layer = gabi::call<u32>(0x025DED64);
    gabi::call(0x025E14A8, layer, name, 0, 0, p);
}
/* dBgS_GndChk (stack object), this TU's vtables */
struct dBgS_GndChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40;
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C;
    /* 0x50 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_GndChk_l, 0x54);
static inline void dBgS_GndChk_ct(dBgS_GndChk_l* c) {
    gabi::call(0x02008E0C, c);
    c->__vtbl_10 = 0x100094BC;
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mpPolyPassChk = gabi::ea(c) + 0x40;
    c->mpGrpPassChk = gabi::ea(c) + 0x4C;
    c->__vtbl_4C = 0x100094DC;
    c->mGrp = 1;
    c->__vtbl_40 = 0x100094EC;
    c->__vtbl_20 = 0x100094CC;
}
static inline void dBgS_GndChk_dt(dBgS_GndChk_l* c) {
    c->__vtbl_20 = 0x100094CC;
    c->__vtbl_40 = 0x100094EC;
    c->__vtbl_4C = 0x100094AC;
    gabi::call(0x02008DAC, c, 0);
}
static inline f32 GroundCross(dBgS_GndChk_l* c) { return cBgS_GroundCross(dComIfG_Bgsp(), c); }

/* Float moves in the recompiled code: the host compiler folds some lfs/stfs pairs into bit
 * copies (mostly when both are in one basic block and close together) and leaves others going
 * through a double (a signalling NaN is quieted). Which one applies was measured per move:
 * bitcopy_f32 models the former, be<f32> the latter. */
static inline void bitcopy_f32(be<f32>* dst, const be<f32>* src) { gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src))); }

static inline void setBaseTRMtx_calc(J3DModel* m) { J3DModel_setBaseTRMtx(m, calc_mtx()); }

#define REG8_S(i) REG_S(8, i)
#define REG8_F(i) REG_F(8, i)
#define REG13_F(i) REG_F(13, i)

/* ---- inlined GameCube functions ---- */

/* waki_set (HD: inlined) */
static inline void waki_set(bk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    bool r30 = false;
    i_this->mpSearchLight = (daObj_Search_Act_c*)fpcM_Search(0x0209BA78 /* s_s2_sub */, i_this);
    switch ((u16)(s16)i_this->dr.mMode) {
    case 0:
        if (i_this->mpSearchLight != nullptr) {
            i_this->dr.mMode = 1;
            i_this->m0300[0] = 1000;
        }
        break;
    case 1:
        if (i_this->m0300[0] != 0) {
            if (i_this->m1212 < 5 && (i_this->m0300[0] & 7) == 0) {
                r30 = true;
            }
        } else {
            gabi::store<u8>(0x1046BE20, 0); /* daObj_Search::Act_c::setFindFlag(false) */
            mDoAud_seStop(0x834 /* JA_SE_MAJUTOU_ALERM */, 30);
            i_this->m1212 = 0;
            i_this->dr.mMode = 0;
        }
        break;
    }
    if (!r30) {
        return;
    }

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    u32 camera = dComIfGp_getCamera(0);
    gabi::Local<cXyz> d;
    gabi::Local<cXyz> sp2C;
    gabi::Local<cXyz> sp20;
    static u8 sp38[0x10000];
    cXyz_mi(gabi::at<cXyz>(camera + 0xE8), d, gabi::at<cXyz>(camera + 0xDC));
    sp2C->copy(*d);
    s16 r27_1 = cM_atan2s(sp2C->x, sp2C->z);
    dPnt_ex* pnt = path_points(i_this);
    for (int i = 0; i < path_num(i_this); i++, pnt++) {
        cXyz* eye = gabi::at<cXyz>(camera + 0xDC);
        sp2C->x = pnt->m_position.x - eye->x;
        sp2C->y = pnt->m_position.y - eye->y;
        sp2C->z = pnt->m_position.z - eye->z;
        cMtx_YrotS(calc_mtx(), -r27_1);
        MtxPosition(sp2C, sp20);
        sp38[i] = sp20->z < 0.0f;
    }

    f32 f29 = REG0_F(3) + 100.0f;
    bool r23 = false;
    int r27 = -1;
    int pnt_idx = 0;
    for (int r24 = 0; r24 < 100; r24++, f29 += 100.0f) {
        pnt = path_points(i_this);
        for (pnt_idx = 0; pnt_idx < path_num(i_this); pnt_idx++, pnt++) {
            if (sp38[pnt_idx] == 0) continue;
            sp2C->x = player->current.pos.x - pnt->m_position.x;
            sp2C->y = player->current.pos.y - pnt->m_position.y;
            sp2C->z = player->current.pos.z - pnt->m_position.z;
            if (std_sqrtf(PSVECSquareMag(sp2C)) < f29) {
                r27 = pnt_idx;
                r23 = true;
                break;
            }
        }
        if (r23) break;
    }

    f29 = REG0_F(4) + 1000.0f;
    r23 = false;
    for (int r24 = 0; r24 < 100; r24++, f29 += 100.0f) {
        pnt = path_points(i_this);
        for (pnt_idx = 0; pnt_idx < path_num(i_this); pnt_idx++, pnt++) {
            if (sp38[pnt_idx] == 0) continue;
            sp2C->x = player->current.pos.x - pnt->m_position.x;
            sp2C->y = player->current.pos.y - pnt->m_position.y;
            sp2C->z = player->current.pos.z - pnt->m_position.z;
            if (std_sqrtf(PSVECSquareMag(sp2C)) > f29 && std_sqrtf(PSVECSquareMag(sp2C)) < f29 + 200.0f && (u32)r27 != (u32)pnt_idx) {
                r23 = true;
                break;
            }
        }
        if (r23) break;
    }

    if (r23 && r27 >= 0) {
        fopAcM_prm_l* params = fopAcM_CreateAppend();
        params->position.x = pnt->m_position.x;
        bitcopy_f32(&params->position.y, &pnt->m_position.y);
        params->position.z = pnt->m_position.z;
        params->angle.x = 0;
        params->angle.z = pnt_idx;
        if (r27 > pnt_idx) {
            params->angle.y = 1;
        } else {
            params->angle.y = -1;
        }
        u32 prm = cM_rndF(1.0f) < 0.5f ? 0xFF00FF19 : 0xFF00FF39;
        params->parameters = prm | (i_this->m02B6 << 0x10);
        params->room_no = fopAcM_GetRoomNo(actor);
        fopAcM_Create(0xBD /* fpcNm_BK_e */, params);
        i_this->m1212 = i_this->m1212 + 1;
    }
}

/* bk_eye_tex_anm (HD: inlined) */
static inline void bk_eye_tex_anm(bk_class* i_this) {
    if (i_this->m02C8 != 0) {
        i_this->m02C8 = i_this->m02C8 - 1;
    } else {
        i_this->m02C8 = (s16)gabi::ftoi(cM_rndF(100.0f) + 20.0f);
        i_this->m02CA = (s16)gabi::ftoi(cM_rndF(3.0f) + 3.0f);
    }
    u32 btp = gabi::ea(i_this->m02C4.get());
    if (i_this->m02CA != 0) {
        i_this->m02CA = i_this->m02CA - 1;
        gabi::store<f32>(btp + 4, 6.0f); /* mDoExt_btpAnm::setFrame */
    } else {
        gabi::store<f32>(btp + 4, 0.0f);
    }
}

/* damage_check (HD: inlined) */
static inline void damage_check(bk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    gabi::Local<CcAtInfo_l> atInfo;
    atInfo->pParticlePos = 0;
    atInfo->mbDead = 0;
    atInfo->mPlCutBit = 0;
    gabi::Local<cXyz> sp38;
    u8 r28 = 0;
    u8 r27 = 0;
    bool r26 = false;

    i_this->dr.mStts.Move();
    dComIfG_Ccsp_Set(&i_this->m0F14);
    i_this->m0F14.SetC(&i_this->m11D8);
    i_this->m11D8.y = -10000.0f;
    i_this->m0F14.SetR(-200.0f);

    if (i_this->m0F14.ChkTgHit()) {
        void* hitObj = i_this->m0F14.GetTgHitObj();
        def_se_set(actor, hitObj, 0x41);
        r26 = true;
        cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
        sp38->x = 0.0f;
        sp38->y = 0.0f;
        sp38->z = -10.0f;
        MtxPosition(sp38, &i_this->dr.m42C);
        i_this->dr.m4D4 = REG0_F(9) + -20.0f;

        gabi::Local<csXyz> sp18;
        sp18->x = player->shape_angle.x;
        sp18->y = (s16)(player->shape_angle.y + 0x8000);
        sp18->z = player->shape_angle.z;
        GXColor* k0 = gabi::at<GXColor>(gabi::ea(&actor->tevStr) + 0x98); /* tevStr.mColorK0 */
        JPABaseEmitter* emitter = dComIfGp_particle_set(0x2B /* ID_AK_JN_ELEMENTKIKUZU00 */, &i_this->m11CC, sp18,
                                                        nullptr, 0xFF, nullptr, -1, k0, k0);
        if (emitter != nullptr) {
            u32 e = gabi::ea(emitter);
            gabi::store<s32>(e + 0x5C, 1);       /* setMaxFrame */
            gabi::store<f32>(e + 0x34, 10.0f);   /* setRate */
            gabi::store<f32>(e + 0x58, 0.2f);    /* setSpread */
            gabi::store<f32>(e + 0x7C, 0.15f);   /* setVolumeSweep */
            f32 s = REG_F(14, 16) + 0.85f;
            gabi::store<f32>(e + 0x238, s);      /* setGlobalParticleScale */
            gabi::store<f32>(e + 0x23C, s);
            gabi::store<f32>(e + 0x240, s);
        }
    }

    /* HD: ChkTgHit is called once per collider */
    if (i_this->m030E == 0) {
        bool hit = false;
        if (i_this->m0DE8.ChkTgHit()) {
            i_this->m02FC = 0;
            i_this->m030E = 4; /* GameCube REG0_S(7) + 5 */
            r28 = 1;
            atInfo->mpObj = gabi::ea(i_this->m0DE8.GetTgHitObj());
            atInfo->pParticlePos = gabi::ea(i_this->m0DE8.GetTgHitPosP());
            hit = true;
        } else if (i_this->m0CB8.ChkTgHit()) {
            i_this->m02FC = 0;
            i_this->m030E = 4;
            r28 = 2;
            atInfo->mpObj = gabi::ea(i_this->m0CB8.GetTgHitObj());
            atInfo->pParticlePos = gabi::ea(i_this->m0CB8.GetTgHitPosP());
            hit = true;
        }
        if (hit) {
            at_power_check(atInfo);
            if (atInfo->mDamage < 4) {
                if (r26 || (i_this->dr.mAction == 15 && i_this->m0310 != 0)) {
                    return;
                }
            }

            /* HD AT types: 0x180000 (ice | light arrow), 0x80000 freezes */
            if (CcObj_AtType(atInfo->mpObj) & 0x180000) {
                if (CcObj_AtType(atInfo->mpObj) & 0x80000) {
                    i_this->mEnemyIce.mFreezeDuration = REG0_S(3) + 300;
                    i_this->dr.mAction = 0;
                    i_this->dr.mMode = 0;
                    path_check(i_this, 0);
                } else {
                    i_this->mEnemyIce.mLightShrinkTimer = 1;
                }
                enemy_fire_remove(&i_this->mEnemyFire);
                i_this->m034C = 0;
                dSmokeEcallBack_end(&i_this->m0350);
                if (i_this->m0B30 == 0) {
                    return;
                }
                i_this->m0B34 = 2;
                return;
            }

            if (CcObj_AtType(atInfo->mpObj) & 0x40200) {
                i_this->mEnemyFire.mFireDuration = REG0_S(2) + 100;
                i_this->m030E = 50;
            }

            i_this->m0310 = 25;

            s8 health = actor->health;
            at_power_check(atInfo);
            if (atInfo->mResultingAttackType == 10 || atInfo->mResultingAttackType == 14) {
                actor->health = 20;
            }
            atInfo->mpActor = cc_at_check(actor, atInfo);
            if (atInfo->mResultingAttackType == 10 || atInfo->mResultingAttackType == 14) {
                actor->health = health;
            }

            {
                gabi::Local<cXyz> pos;
                pos->x = actor->current.pos.x;
                bitcopy_f32(&pos->y, &actor->current.pos.y);
                bitcopy_f32(&pos->z, &actor->current.pos.z);
                dKy_Sound_set(pos, 100, fopAcM_GetID(actor), 5);
            }

            if (atInfo->mResultingAttackType == 12) {
                i_this->m030E = 10; /* HD */
            }

            if (l_bkHIO().m007 != 0) {
                actor->health = 10;
            }

            i_this->m1208 = i_this->m1208 | atInfo->mPlCutBit;

            u8 type = atInfo->mResultingAttackType;
            bool generic = false;
            if (type == 10) {
                if (i_this->dr.mAction == 19) { /* HD */
                    generic = true;
                } else {
                    atInfo->mDamage = 1;
                    i_this->m122C = REG_S(13, 3) + 8;
                    generic = true;
                }
            } else {
                i_this->m122C = 0;
                if (type == 1) {
                    s16 r3 = i_this->m0332 - actor->current.angle.y;
                    if (r3 < 0) {
                        r3 = -r3;
                    }
                    if (daPy_getCutType(player) == 5 /* CUT_TYPE_BT_JUMPCUT */) {
                        r27 = 2;
                    } else if (atInfo->mbDead) {
                        r27 = 1;
                    } else if ((u16)r3 > 0x4000) {
                        r27 = 3;
                    } else if (r28 == 1) {
                        r27 = 4;
                    } else {
                        r27 = 5;
                    }
                } else if (type == 9 && daPy_getCutType(player) == 0x11 /* CUT_TYPE_HAMMER_SIDESWING */) {
                    r27 = 7;
                    cMtx_YrotS(calc_mtx(), player->shape_angle.y + 0x4000);
                } else if (type == 2) {
                    r27 = 7;
                    cMtx_YrotS(calc_mtx(), atInfo->m0C.y);
                } else {
                    generic = true;
                }
            }
            if (generic) {
                if (atInfo->mbDead) {
                    r27 = 7;
                } else {
                    r27 = 4;
                }
                cMtx_YrotS(calc_mtx(), atInfo->m0C.y);
            }
        }
    }

    if (another_hit() != 0) {
        r27 = 1;
        i_this->m0332 = actor->shape_angle.y + 0x8000;
        i_this->dr.mInvincibleTimer = 0;
        i_this->m030E = REG0_S(7) + 5;
    }

    switch (r27) {
    case 1:
        i_this->dr.m424 = i_this->dr.m424 | 0x10;
        i_this->dr.m428 = 26.0f;
        cMtx_YrotS(calc_mtx(), i_this->m0332);
        break;
    case 2:
        i_this->dr.m424 = i_this->dr.m424 | 0x40;
        cMtx_YrotS(calc_mtx(), actor->current.angle.y + 0x8000);
        i_this->dr.m428 = 26.0f;
        break;
    case 3:
        i_this->dr.m424 = i_this->dr.m424 | 0x40;
        i_this->dr.m428 = 26.0f;
        cMtx_YrotS(calc_mtx(), i_this->m0332);
        break;
    case 4:
        i_this->dr.m424 = i_this->dr.m424 | 0x10;
        i_this->dr.m428 = 23.0f;
        cMtx_YrotS(calc_mtx(), i_this->m0332);
        break;
    case 5:
        i_this->dr.m424 = i_this->dr.m424 | 0x20;
        i_this->dr.m428 = 23.0f;
        cMtx_YrotS(calc_mtx(), i_this->m0332);
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_AOMUKE_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_AOMUKE_e);
        i_this->dr.m48A = 10;
        i_this->dr.m474 = REG0_F(14) + 5000.0f;
        i_this->dr.m70E = 7;
        if (i_this->m0336 == 0) {
            i_this->m0336 = 3;
            i_this->m0344.x = 0;
        }
        break;
    case 7:
        i_this->dr.m424 = i_this->dr.m424 | 0x10;
        i_this->dr.m428 = 26.0f;
        break;
    case 8:
        i_this->dr.m424 = i_this->dr.m424 | 0x10;
        i_this->dr.m428 = 23.0f;
        break;
    }

    if (i_this->dr.m424 != 0) {
        if (i_this->dr.mAction == 19) {
            /* HD: different knock-down values, the face-down animation is restarted */
            i_this->dr.m488 = 1;
            i_this->dr.m486 = 0x4000;
            i_this->dr.mMode = -100;
            i_this->dr.m44C.y = -125.0f;
            i_this->dr.mAction = 0;
            i_this->dr.m71E = REG_S(10, 6) + 10;
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_UTUBUSE_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_UTUBUSE_e);
            i_this->dr.m70C = 0;
            i_this->dr.m482 = actor->shape_angle.y;
            i_this->dr.m47C = 1;
            i_this->dr.m48A = 0;
            i_this->dr.m478 = 0.0f;
            i_this->dr.mAction = 19;
            cMtx_YrotS(calc_mtx(), actor->current.angle.y);
            sp38->x = 0.0f;
            sp38->y = 0.0f;
            sp38->z = REG_F(14, 2) + -30.0f;
            MtxPosition(sp38, &actor->speed);
            fopAcM_monsSeStart(actor, 0x4829 /* JA_SE_CV_BK_FAINTED */, 0);
            return;
        }

        gabi::Local<cXyz> sp2C;
        sp2C->x = 0.0f;
        sp2C->y = 0.0f;
        sp2C->z = -10.0f;
        MtxPosition(sp2C, &i_this->dr.m42C);

        if (i_this->dr.m428 < 25.0f) {
            i_this->dr.m4D4 = -l_bkHIO().m010;
        } else {
            i_this->dr.m428 = cM_rndF(10.0f) + 90.0f;
        }
    }

    if (r27 != 0) {
        if (atInfo->mbDead) {
            if (actor->health <= 0) {
                fopAcM_monsSeStart(actor, 0x4829 /* JA_SE_CV_BK_FAINTED */, 0);
                if (i_this->mType == 10) {
                    i_this->m1234 = 50;
                    actor->actor_status = actor->actor_status | 0x4000;
                }
            } else {
                fopAcM_monsSeStart(actor, 0x4828 /* JA_SE_CV_BK_DAMAGE_L */, 0);
            }
        } else {
            fopAcM_monsSeStart(actor, 0x4827 /* JA_SE_CV_BK_DAMAGE_S */, 0);
        }
    }
}

/* demo_camera (HD: inlined) */
static inline void demo_camera(bk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s8 camId = dComIfGp_getPlayerCameraID(0);
    u32 camera = dComIfGp_getCamera(camId);

    bool r28 = true;
    gabi::Local<cXyz> sp8C;
    gabi::Local<cXyz> sp80;
    gabi::Local<cXyz> tmp;
    switch (i_this->m1234) {
    case 0:
        break;
    case 1:
        if (gabi::load<u16>(gabi::ea(i_this) + 0xF8) != 2) { /* eventInfo.checkCommandDemoAccrpt() */
            r28 = false;
            break;
        }
        i_this->m1234 = i_this->m1234 + 1;
        dCam_Stop(camera);
        dCam_SetTrimSize(camera, 2);
        i_this->m1236 = 0;
        /* fall through */
    case 2:
        sp80->x = 1884.0f;
        sp80->y = player->current.pos.y;
        sp80->z = -4100.0f;
        daPy_vcall_setPlayerPosAndAngle(player, sp80, REG8_S(4) + 0x61A8);
        bitcopy_f32(&i_this->m1244.x, &player->current.pos.x);
        i_this->m1244.y = player->current.pos.y;
        i_this->m1244.z = player->current.pos.z;
        i_this->m1244.y = i_this->m1244.y + (REG13_F(6) + 100.0f);
        cMtx_YrotS(calc_mtx(), player->shape_angle.y);
        sp8C->x = 0.0f;
        sp8C->y = REG13_F(7) + 50.0f;
        sp8C->z = REG13_F(8) + 150.0f;
        MtxPosition(sp8C, sp80);
        cXyz_pl(&player->current.pos, tmp, sp80);
        i_this->m1238.copy(*tmp);
        i_this->m1260 = REG13_F(9) + 45.0f;
        if (i_this->m1236 == 30) {
            /* changeOriginalDemo(); changeDemoMode(50) */
            gabi::store<s32>(gabi::ea(player) + 0x428, 0);
            gabi::store<s16>(gabi::ea(player) + 0x420, 3);
            gabi::store<s32>(gabi::ea(player) + 0x430, 50);
        }
        if (i_this->m1236 != 50) {
            break;
        }
        i_this->m1236 = 0;
        i_this->m1234 = 3;
        i_this->m1260 = REG8_F(5) + 35.0f;
        /* fall through */
    case 3:
        cMtx_YrotS(calc_mtx(), REG8_S(4) + 0x61A8);
        sp8C->x = 0.0f;
        sp8C->y = 100.0f;
        sp8C->z = REG8_F(18) + 30.0f;
        MtxPosition(sp8C, sp80);
        cXyz_pl(&player->current.pos, tmp, sp80);
        i_this->m1238.copy(*tmp);
        i_this->m1244.copy(ken()->current.pos);
        i_this->m1244.y = i_this->m1244.y + REG8_F(4);
        if (i_this->m1236 > 10) {
            cLib_addCalc2(&i_this->m1260, REG8_F(5) + 15.0f, 0.8f, REG0_F(14) + 3.0f);
        }
        if (i_this->m1236 > 60) {
            i_this->m1236 = 0;
            i_this->m1234 = 4;
        }
        break;
    case 4:
        if (i_this->m1236 == 5) {
            gabi::store<s32>(gabi::ea(player) + 0x430, 29); /* changeDemoMode(29) */
            daPy_vcall_voiceStart(player, 0x1F);
        }
        i_this->m1244.copy(player->current.pos);
        i_this->m1244.y = i_this->m1244.y + (REG8_F(6) + 90.0f);
        cMtx_YrotS(calc_mtx(), player->shape_angle.y);
        sp8C->x = 0.0f;
        sp8C->y = REG8_F(7) + 50.0f;
        sp8C->z = REG8_F(8) + 200.0f;
        MtxPosition(sp8C, sp80);
        cXyz_pl(&player->current.pos, tmp, sp80);
        i_this->m1238.copy(*tmp);
        i_this->m1260 = REG8_F(9) + 55.0f;
        if (i_this->m1236 == 30) {
            dComIfGs_onSwitch(0xE0, fopAcM_GetRoomNo(actor));
            mDoAud_bgmAllMute(30);
        }
        if (i_this->m1236 == 50) {
            gabi::store<s32>(gabi::ea(player) + 0x430, 25); /* changeDemoMode(25) */
            daPy_vcall_voiceStart(player, 27);
        }
        if (i_this->m1236 == 70) {
            i_this->m1236 = 0;
            i_this->m1234 = 5;
        }
        break;
    case 5:
        if (i_this->m1236 < 35) {
            bitcopy_f32(&i_this->m1244.x, &actor->current.pos.x);
            i_this->m1244.y = actor->current.pos.y;
            i_this->m1244.z = actor->current.pos.z;
            i_this->m1244.y = i_this->m1244.y + (REG8_F(10) + 100.0f);
            cMtx_YrotS(calc_mtx(), actor->shape_angle.y);
            sp8C->x = REG8_F(11) + 200.0f;
            sp8C->y = REG8_F(12) + 50.0f;
            sp8C->z = REG8_F(13) + 250.0f;
            MtxPosition(sp8C, sp80);
            cXyz_pl(&actor->current.pos, tmp, sp80);
            i_this->m1238.copy(*tmp);
            i_this->m1260 = REG8_F(14) + 55.0f;
        }
        if (i_this->m1236 == 5) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_BIKKURI_e, 3.0f, 0, 1.0f, -1);
        }
        if (i_this->m1236 >= 35) {
            i_this->dr.m710 = 1;
        }
        if (i_this->m1236 == 45) {
            i_this->m02B5 = 200;
            i_this->dr.mAction = 4;
            i_this->m0300[1] = 0;
            i_this->dr.mMode = 0;
        }
        if (i_this->m1236 > 55) {
            i_this->m1234 = 0;
            gabi::Local<cXyz> c;
            gabi::Local<cXyz> e;
            *c = i_this->m1244.get();
            *e = i_this->m1238.get();
            dCam_Reset(camera, c, e);
            dCam_Start(camera);
            dCam_SetTrimSize(camera, 0);
            dComIfGp_event_reset();
            actor->actor_status = actor->actor_status & ~0x4000u;
            mDoAud_subBgmStart(0x80000019 /* JA_BGM_MBOSS */);
        }
        break;
    case 10:
        if (gabi::load<u16>(gabi::ea(i_this) + 0xF8) != 2) {
            r28 = false;
            break;
        }
        i_this->m1234 = i_this->m1234 + 1;
        dCam_Stop(camera);
        dCam_SetTrimSize(camera, 2);
        i_this->m1236 = 30;
        /* fall through */
    case 11:
        if (i_this->m1236 == 30) {
            i_this->m1234 = 0;
            gabi::Local<cXyz> c;
            gabi::Local<cXyz> e;
            *c = i_this->m1244.get();
            *e = i_this->m1238.get();
            dCam_Reset(camera, c, e);
            dCam_Start(camera);
            dCam_SetTrimSize(camera, 0);
            dComIfGp_event_reset();
            dComIfGs_onSwitch(0xE1, fopAcM_GetRoomNo(actor));
        }
        break;
    case 50:
        if (gabi::load<u16>(gabi::ea(i_this) + 0xF8) != 2) {
            r28 = false;
            break;
        }
        dCam_Stop(camera);
        dCam_SetTrimSize(camera, 1);
        i_this->m1234 = 51;
        {
            u32 r3 = dComIfGp_getCamera(0);
            i_this->m1238.copy(*gabi::at<cXyz>(r3 + 0xDC));
            i_this->m1244.copy(*gabi::at<cXyz>(r3 + 0xE8));
        }
        i_this->m1260 = 55.0f;
        i_this->m1236 = 0;
        /* fall through */
    case 51:
        cLib_addCalc2(&i_this->m1260, REG0_F(13) + 30.0f, 0.2f, REG0_F(14) + 0.4f);
        if (i_this->m02DE == 0) {
            cLib_addCalc2(&i_this->m1244.x, actor->current.pos.x, 0.1f, 100.0f);
            cLib_addCalc2(&i_this->m1244.y, actor->current.pos.y + 190.0f + REG0_F(12), 0.1f, 100.0f);
            cLib_addCalc2(&i_this->m1244.z, actor->current.pos.z, 0.1f, 100.0f);
        }
        if (i_this->m1236 > 150) {
            i_this->m1236 = 0;
            i_this->m1234 = 11;
        }
        break;
    }

    if (!r28) {
        /* the event is not running yet: order it */
        fopAcM_orderPotentialEvent(actor, 2 /* dEvtFlag_STAFF_ALL_e */, 0xFFFF, 0);
        u32 cond = gabi::ea(i_this) + 0xFA;
        gabi::store<u16>(cond, gabi::load<u16>(cond) | 2);
        return;
    }
    if (i_this->m1234 != 0) {
        gabi::Local<cXyz> c;
        gabi::Local<cXyz> e;
        *c = i_this->m1244.get();
        *e = i_this->m1238.get();
        dCam_Set(camera, c, e, i_this->m1260, 0);
        JUTReport(410, 430, STR(0x100095AC) /* "K SUB  COUNT  %d" */, i_this->m1236);
        i_this->m1236 = i_this->m1236 + 1;
    }
}

/* yari_off_check (HD: inlined) */
static inline void yari_off_check(bk_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->m0B34 == 0) {
        return;
    }
    fopAc_ac_c* boko = fopAcM_SearchByID(i_this->m1200);
    if (boko) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        fopAcM_cancelCarryNow(boko);
        if (i_this->m0B34 != 2) {
            gabi::store<s16>(gabi::ea(boko) + 0x43A, (s16)gabi::ftoi(cM_rndFX(2000.0f))); /* setRotAngleSpeed */
            s16 angleY = actor->shape_angle.y + 0x8000;
            angleY += (s16)gabi::ftoi(cM_rndFX(8000.0f));
            f32 speedForward = cM_rndF(10.0f) + 20.0f;
            f32 speedY = cM_rndF(20.0f) + 20.0f;
            /* moveStateInit(speedForward, speedY, angleY) */
            boko->current.angle.y = angleY;
            boko->speed.y = speedY;
            boko->speedF = speedForward;
        }
        boko->current.angle.y = player->shape_angle.y;

        gabi::Local<dBgS_LinChk_l> linChk;
        dBgS_LinChk_ct(linChk);
        dBgS_LinChk_Set_l(linChk, &actor->eyePos, &boko->current.pos, i_this);
        if (LineCross(linChk)) {
            Mtx34* mtx = J3DModel_getAnmMtx(i_this->mpMorf->getModel(), BK_JNT_MUNE_e);
            PSMTXCopy(mtx, calc_mtx());
            daBoko_setMatrix(boko, calc_mtx());
            gabi::Local<cXyz> offset;
            offset->x = 0.0f;
            offset->y = 0.0f;
            offset->z = 0.0f;
            MtxPosition(offset, &boko->current.pos);
        }
        dBgS_LinChk_dt(linChk);
    }
    i_this->m0B34 = 0;
    i_this->m0B30 = 0;
    i_this->m121F = 1;
}

/* 0209BD1C */
BOOL daBk_Execute(bk_class* i_this) {
    WWHD_FUNC(0x0209BD1C, BOOL, i_this);
    dComIfGp_get(); /* daPy_py_c* player = dComIfGp_getPlayer(0): unused */

    /* HD: fell far below its home: set the switch and the actor flag, delete */
    if (i_this->m02B8 != 0 && i_this->current.pos.y < i_this->home.pos.y - 5000.0f) {
        dComIfGs_onSwitch(i_this->m02B8, fopAcM_GetRoomNo(i_this));
        dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
        fopAcM_delete(i_this);
        if (i_this->m0B30 != 0) {
            fopAc_ac_c* temp = fopAcM_SearchByID(i_this->m1200);
            if (temp != nullptr) {
                fopAcM_delete(temp);
            }
        }
        return TRUE;
    }

    another_hit() = 0;
    if (i_this->mpSearchLight != nullptr) {
        gabi::store<u8>(gabi::ea(i_this->mpSearchLight.get()) + 0x780, 0); /* setBkControl(false) */
    }

    /* HD: no home-height test here */
    if (i_this->m121C != 0) {
        if (i_this->m0B30 != 0) {
            fopAc_ac_c* temp = fopAcM_SearchByID(i_this->m1200);
            if (temp != nullptr) {
                fopAcM_delete(temp);
            }
        }
        fopAcM_delete(i_this);
        return TRUE;
    }

    /* HD: a timer that clears status bit 0x4000 when it runs out */
    if (i_this->m02DF != 0) {
        s8 t = (s8)(i_this->m02DF - 1);
        i_this->m02DF = t;
        if (t == 0) {
            i_this->actor_status = i_this->actor_status & ~0x4000u;
        }
    }

    if (enemy_ice(&i_this->mEnemyIce)) {
        i_this->mpMorf->setPlayMode(0);
        i_this->mpMorf->setPlaySpeed(3.0f);
        i_this->mpMorf->play(&i_this->eyePos, 0, 0);
        J3DModel_setBaseTRMtx(i_this->mpMorf->getModel(), mDoMtx_stack_c::get());
        i_this->mpMorf->calc();
        tate_mtx_set(i_this);
        bou_mtx_set(i_this);
        return TRUE;
    }

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);

    if (i_this->mType == 8) {
        for (int i = 0; i < 5; i++) {
            if (i_this->m0300[i] != 0) {
                i_this->m0300[i] = i_this->m0300[i] - 1;
            }
        }
        waki_set(i_this);
        return TRUE;
    }

    if (l_bkHIO().m006 == 0 || CPad_CHECK_TRIG_B(0) || CPad_CHECK_HOLD_Y(0)) {
        i_this->m02F8 = i_this->m02F8 + 1;
        for (int i = 0; i < 5; i++) {
            if (i_this->m0300[i] != 0) {
                i_this->m0300[i] = i_this->m0300[i] - 1;
            }
        }
        if (i_this->m0314 != 0) i_this->m0314 = i_this->m0314 - 1;
        if (i_this->m0316 != 0) i_this->m0316 = i_this->m0316 - 1;
        if (i_this->m030A != 0) i_this->m030A = i_this->m030A - 1;
        if (i_this->m02CC != 0) i_this->m02CC = i_this->m02CC - 1;
        if (i_this->dr.m48A != 0) i_this->dr.m48A = i_this->dr.m48A - 1;
        if (i_this->m030E != 0) i_this->m030E = i_this->m030E - 1;
        if (i_this->m0310 != 0) i_this->m0310 = i_this->m0310 - 1;
        if (i_this->m0B78 != 0) i_this->m0B78 = i_this->m0B78 - 1;

        if (i_this->m02B7 != 0xFF && i_this->mType == 6 && dComIfGs_isSwitch(i_this->m02B7, dComIfGp_roomControl_getStayNo())) {
            return TRUE;
        }

        if (i_this->m1214 != 0) {
            fopAc_ac_c* boko = fopAcM_SearchByID(i_this->m1200);
            if (boko != nullptr) {
                i_this->m1214 = 0;
                i_this->m0B30 = 1;
                fopAcM_setCarryNow(boko, FALSE);
                MtxTrans(-10000.0f, -10000.0f, 0.0f, 0);
                daBoko_setMatrix(boko, calc_mtx());
            }
        }

        if (i_this->m02BA != 0) {
            if (i_this->m02BA == 0xFF) {
                f32 f30;
                if (i_this->m02B5 != 0xFF) {
                    f30 = (f32)i_this->m02B5 * 10.0f;
                } else {
                    f30 = 300.0f;
                }
                fopAc_ac_c* r23 = fopAcM_SearchByID(i_this->m1204);
                if (i_this->m030E == 0 && (fopAcM_searchPlayerDistance(i_this) < f30 || r23 == nullptr)) {
                    i_this->m02BA = 0;
                    if (r23 == nullptr) {
                        i_this->m0300[0] = 50;
                        i_this->m0310 = 20;
                        if (std::fabs((f32)i_this->speedF) > 10.0f) {
                            another_hit() = 1;
                        } else {
                            i_this->scale.x = 0.5f;
                            i_this->scale.y = 0.5f;
                            i_this->scale.z = 0.5f;
                        }
                    }
                    i_this->m0B88.SetR(62.5f);
                } else {
                    if (r23 != nullptr) {
                        i_this->current.pos.copy(r23->current.pos);
                        f32 spd = r23->speedF;
                        if (spd > 1.0f) {
                            i_this->speedF = spd;
                            i_this->shape_angle.x = r23->shape_angle.x;
                            i_this->shape_angle.y = r23->shape_angle.y;
                            i_this->shape_angle.z = r23->shape_angle.z;
                            i_this->current.angle.x = r23->shape_angle.x;
                            i_this->current.angle.y = r23->shape_angle.y;
                            i_this->current.angle.z = r23->shape_angle.z;
                        }
                    } else {
                        i_this->m0B88.SetC(&i_this->current.pos);
                        i_this->m0B88.SetR(10.0f);
                        dComIfG_Ccsp_Set(&i_this->m0B88);
                        void* r3 = GetCoHitObj(&i_this->m0B88);
                        if (r3 != nullptr) {
                            u32 stts = gabi::load<u32>(gabi::ea(r3) + 0x44);
                            fopAc_ac_c* temp = stts ? gabi::at<fopAc_ac_c>(gabi::load<u32>(stts + 0xC)) : nullptr;
                            if (temp != nullptr) {
                                i_this->m1204 = fopAcM_GetID(temp);
                            }
                        }
                    }
                    return TRUE;
                }
            } else {
                if (dComIfGs_isSwitch(i_this->m02BA - 1, fopAcM_GetRoomNo(i_this))) {
                    i_this->m02BA = 0;
                } else {
                    return TRUE;
                }
            }
        }

        gabi::store<u32>(gabi::ea(i_this) + 0x39C, 4); /* attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e */
        i_this->actor_status = i_this->actor_status | 0x20; /* fopAcStts_SHOWMAP_e */
        i_this->m02F0 = 0;
        i_this->m02F4 = 0;
        i_this->m0B7B = 0;

        bk_eye_tex_anm(i_this);
        i_this->dr.m438 = 0;
        i_this->mBtStartFrame = 100.0f; /* setBtAttackData(100, 100, 10000, 0) */
        i_this->mBtEndFrame = 100.0f;
        i_this->mBtMaxDis = 10000.0f;
        i_this->mBtAttackType = 0;
        i_this->mBtNowFrame = 0.0f;
        damage_check(i_this);
        Bk_move(i_this);
        demo_camera(i_this);
        ground_smoke_set(i_this);
        if (i_this->m030C != 0) {
            i_this->m030C = i_this->m030C - 1;
        }
        i_this->dr.mpEnemy = i_this;
        i_this->dr.mEnemyType = 2;
    }
    if (i_this->dr.mAction != 31) {
        i_this->shape_angle.x = i_this->current.angle.x;
        i_this->shape_angle.y = i_this->current.angle.y;
        i_this->shape_angle.z = i_this->current.angle.z;
    }

    s32 r3 = damage_reaction(&i_this->dr);
    if (r3 != 0) {
        i_this->m034C = 1;
        i_this->m11FC = 0xFFFFFFFF;
        switch (r3) {
        case 1:
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_AOMUKE_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_AOMUKE_e);
            if (i_this->m0B30 != 0) {
                i_this->m0B34 = 1;
            }
            i_this->dr.mAction = 0;
            break;
        case 2:
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_UTUBUSE_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_UTUBUSE_e);
            break;
        case 5:
            if (i_this->dr.mAction != 4 && i_this->dr.mAction != 11) {
                wait_set(i_this);
                i_this->dr.mAction = 4;
                i_this->dr.mMode = 0;
                i_this->m0300[1] = 30;
            }
            i_this->m0336 = 5;
            i_this->m0344.x = -0x4000;
            break;
        case 10:
            wait_set(i_this);
            i_this->dr.mMode = 2;
            i_this->dr.mAction = 0;
            path_check(i_this, 0);
            break;
        case 20:
            i_this->m0336 = 1;
            i_this->m0344.x = -0x4000;
            i_this->m034C = l_bkHIO().m00C + 16;
            i_this->m034E = 1;
            i_this->dr.m4C8[2] = l_bkHIO().m08C;
            break;
        case 21:
            i_this->m0336 = 1;
            i_this->m0344.x = 0;
            i_this->dr.m4C8[2] = l_bkHIO().m08C;
            break;
        case 30:
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_AOMUKE_e, 2.0f, 0, 1.0f, dRes_INDEX_BK_BAS_BK_AOMUKE_e);
            if (i_this->m0B30 != 0) {
                i_this->m0B34 = 1;
            }
            i_this->dr.mAction = 0;
            fopAcM_monsSeStart_ool(i_this, 0x4829 /* JA_SE_CV_BK_FAINTED */, 0);
            break;
        }
    }

    J3DModel* model = i_this->mpMorf->getModel();
    {
        cXyz* bs = gabi::at<cXyz>(gabi::ea(model) + 0xBC); /* setBaseScale */
        *bs = i_this->scale.get();
    }
    setBaseTRMtx_calc(model);
    if (i_this->m030C == 0) {
        i_this->mpMorf->play(&i_this->eyePos, 0, 0);
    }
    i_this->mpMorf->calc();

    enemy_fire(&i_this->mEnemyFire);

    if (i_this->m0B30 != 0) {
        fopAc_ac_c* boko = fopAcM_SearchByID_ool(i_this->m1200);
        if (boko != nullptr) {
            if (fopAcM_checkCarryNow(boko)) {
                if (i_this->m0B7B == 0) {
                    PSMTXCopy(J3DModel_getAnmMtx(i_this->mpMorf->getModel(), BK_JNT_BUKI_e), calc_mtx());
                    cMtx_YrotM(calc_mtx(), (s16)(0x3E80 + REG8_S(1)));
                    cMtx_XrotM(calc_mtx(), REG8_S(2));
                    cMtx_ZrotM(calc_mtx(), REG8_S(3));
                    MtxTrans(REG8_F(9), REG8_F(10), REG8_F(11) + 65.0f, 1);
                } else {
                    MtxTrans(i_this->home.pos.x, i_this->home.pos.y, i_this->home.pos.z, 0);
                    cMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
                    MtxTrans(REG_F(6, 7) - 40.0f, REG_F(6, 8) + 68.0f, REG_F(6, 9) + 82.0f, 1);
                    cMtx_XrotM(calc_mtx(), (s16)(0x5B1B + REG8_S(5)));
                }
                daBoko_setMatrix(boko, calc_mtx());
                gabi::Local<cXyz> sp64;
                sp64->x = REG8_F(12);
                sp64->y = REG8_F(13);
                sp64->z = REG8_F(14);
                MtxPosition(sp64, &i_this->m1178);
            }
        } else {
            i_this->m0B30 = 0;
        }
    }

    tate_mtx_set(i_this);
    bou_mtx_set(i_this);
    yari_off_check(i_this);
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    gabi::Local<cXyz> sp58;
    sp58->x = 0.0f;
    sp58->y = 0.0f;
    /* HD: no forward offset while dr.mAction == 18 */
    sp58->z = i_this->dr.mAction == 18 ? REG_F(10, 4) : 35.0f;
    gabi::Local<cXyz> sp4C;
    MtxPosition(sp58, sp4C);
    i_this->m0B88.SetC(sp4C);
    dComIfG_Ccsp_Set(&i_this->m0B88);
    dCcMassS_Set(&i_this->m0B88, 3);

    gabi::Local<cXyz> sp40;
    gabi::Local<cXyz> sp34;
    *sp40 = i_this->m116C.get();
    bitcopy_f32(&sp34->x, &i_this->current.pos.x);
    sp34->y = i_this->current.pos.y;
    sp34->z = i_this->current.pos.z;
    if (i_this->m030E != 0) {
        sp40->y = sp40->y - 20000.0f;
        sp34->y = sp34->y - 20000.0f;
    }
    if (i_this->dr.mAction == 10) {
        sp40->y = sp40->y - 20000.0f;
        sp34->y = sp34->y - 100.0f;
    }
    i_this->m0CB8.SetC(sp34);
    dComIfG_Ccsp_Set(&i_this->m0CB8);
    i_this->m0DE8.SetC(sp40);
    dComIfG_Ccsp_Set(&i_this->m0DE8);

    if (i_this->m0336 != 0) {
        s16 t = i_this->m0336 - 1;
        i_this->m0336 = t;
        if (t == 0) {
            i_this->m0344.y = i_this->current.angle.y;
            dComIfGp_particle_set(0xE /* ID_AK_JN_TUBA00 */, &i_this->m116C, &i_this->m0344);
        }
    }

    if (i_this->dr.mAcch.ChkGroundHit() || i_this->dr.mAction == 19) {
        gabi::Local<dBgS_GndChk_l> gndChk;
        dBgS_GndChk_ct(gndChk);
        s16 r21 = 0x7FFF;
        s16 r23 = 0x7FFF;
        f32 f31 = i_this->dr.m480 != 0 ? 100.0f : 10.0f;
        f32 x = i_this->current.pos.x;
        f32 y = i_this->current.pos.y + (50.0f - i_this->dr.m44C.y);
        f32 z = i_this->current.pos.z;
        gndChk->m_pos.x = x;
        gndChk->m_pos.y = y;
        gndChk->m_pos.z = z;
        f32 gy = GroundCross(gndChk);
        if (gy != -1000000000.0f) {
            f32 tz = z + f31;
            f32 ty = 50.0f + gy;
            gndChk->m_pos.x = x;
            gndChk->m_pos.y = ty;
            gndChk->m_pos.z = tz;
            f32 f1 = GroundCross(gndChk);
            if (f1 != -1000000000.0f) {
                r21 = (s16)-cM_atan2s(f1 - gy, tz - z);
                if (r21 > 0x2000 || r21 < -0x2000) {
                    r21 = 0;
                }
            }
            f32 tx = x + f31;
            gndChk->m_pos.x = tx;
            gndChk->m_pos.y = ty;
            gndChk->m_pos.z = z;
            f1 = GroundCross(gndChk);
            if (f1 != -1000000000.0f) {
                r23 = cM_atan2s(f1 - gy, tx - x);
                if (r23 > 0x2000 || r23 < -0x2000) {
                    r23 = 0;
                }
            }
        }
        if (i_this->dr.mAction == 19) {
            r23 = 0;
            r21 = 0;
        }
        if (r21 != 0x7FFF) {
            cLib_addCalcAngleS2(&i_this->dr.m48C.x, r21, 1, 0x400);
        }
        if (r23 != 0x7FFF) {
            cLib_addCalcAngleS2(&i_this->dr.m48C.z, r23, 1, 0x400);
        }
        dBgS_GndChk_dt(gndChk);
    }

    return TRUE;
}
VERIFY(0x0209BD1C, daBk_Execute);
