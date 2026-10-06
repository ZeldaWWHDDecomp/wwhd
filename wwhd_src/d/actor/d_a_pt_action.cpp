/**
 * d_a_pt_action.cpp (WWHD)
 * Enemy - Miniblin: action() with its inlined helpers.
 *
 * Written from the WWHD code (02447300, 10 KB): the
 * GameCube decompilation has only "Nonmatching" stubs for pt_wait, next_pos_set, pt_move,
 * view_check, pt_attack, pt_koke, pt_ples, pt_bat, damage_check, water_check and action. GHS
 * inlined them all into action(); they are written here as inline helpers with the GameCube
 * names (the split of the code between them is a reconstruction).
 */
#include "d/actor/d_a_pt.h"

enum { PROC_PLAYER = 0xA8 };
enum {
    JA_SE_CV_PT_LAND = 0x48F7, JA_SE_CV_PT_JUMP = 0x48F8, JA_SE_CV_PT_DAMAGE = 0x48F9, JA_SE_CV_PT_DIE = 0x48FA,
    JA_SE_CV_PT_KOKE = 0x48FB, JA_SE_CV_PT_KOKE2 = 0x48FC,
    JA_SE_OBJ_PT_JUMP = 0x592D, JA_SE_OBJ_PT_LAND = 0x592E, JA_SE_OBJ_PT_FALL = 0x592F, JA_SE_OBJ_FALL_WATER_S = 0x6918,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02516178 dCcD_GObjInf::GetAtHitObj */
static inline void* GetAtHitObj(dCcD_GObjInf* o) { return gabi::call<void*>(0x02516178, o); }
/* 025192A8 cc_at_check(fopAc_ac_c*, CcAtInfo*) */
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
static inline void cc_at_check(fopAc_ac_c* a, CcAtInfo_l* info) { gabi::call(0x025192A8, a, info); }
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb) (unnamed by the matcher) */
static inline void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb) {
    gabi::call(0x025E1AA4, id, pos, pid, param, reverb);
}
/* fopAcM_seStart / fopAcM_monsSeStart: HD inlines test &eyePos only */
static inline void se_start(pt_class* i_this, u32 id) {
    cXyz* eye = &i_this->eyePos;
    if (gabi::ea(eye) != 0) mDoAud_seStart(id, eye, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
}
static inline void mons_se_start(pt_class* i_this, u32 id) {
    cXyz* eye = &i_this->eyePos;
    if (gabi::ea(eye) != 0) {
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        u32 pid = i_this != nullptr ? gabi::load<u32>(gabi::ea(i_this) + 4) : 0xFFFFFFFFu; /* fopAcM_GetID */
        mDoAud_monsSeStart(id, eye, pid, 0, dComIfGp_getReverb(roomNo));
    }
}
/* camera (play+0x5AF8): eye +0xDC, center +0xE8 */
static inline u32 dComIfGp_getCamera0() { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8); }


static inline void dBgS_GndChk_ct(dBgS_GndChk_l* c) {
    gabi::call(0x02008E0C, c); /* cBgS_GndChk::cBgS_GndChk */
    for (int i = 1; i < 7; i++) c->mPass[i] = 0;
    c->mpGrpPassChk = gabi::ea(c) + 0x4C;
    c->__vtbl_4C = 0x10038194;
    c->mGrp = 0xE;
    c->mpPolyPassChk = gabi::ea(c) + 0x40;
    c->__vtbl_40 = 0x100381A4;
    c->__vtbl_10 = 0x10038174;
    c->__vtbl_20 = 0x10038184;
    c->mPass[0] = 1;
}
static inline void dBgS_GndChk_dt(dBgS_GndChk_l* c) {
    c->__vtbl_20 = 0x10038104;
    c->__vtbl_40 = 0x10038124;
    c->__vtbl_4C = 0x100380E4;
    gabi::call(0x02008DAC, c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}

static inline void anm_init_(pt_class* i_this, int bck, f32 morf, u8 loop, f32 speed, int bas) {
    gabi::call(0x02445DBC, i_this, bck, morf, loop, speed, bas);
}
static inline void smoke_set_(pt_class* i_this, s8 type) { gabi::call(0x02445F98, i_this, type); }
static inline s16 get_z_ang_(pt_class* i_this) { return gabi::call<s16>(0x02446074, i_this); }

static inline bool spawn_switch_ok(pt_class* i_this) {
    if (i_this->mEnableSpawnSwitch != 0xFF && !dComIfGs_isSwitch(i_this->mEnableSpawnSwitch, dComIfGp_roomControl_getStayNo()))
        return false;
    if (i_this->mDisableRespawnSwitch != 0xFF && dComIfGs_isSwitch(i_this->mDisableRespawnSwitch, dComIfGp_roomControl_getStayNo()))
        return false;
    return true;
}

/* action 0: hidden until the player comes near and the spot is in view */
static inline void pt_wait(pt_class* i_this) {
    u32 camera = dComIfGp_getCamera0();
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::store<u32>(gabi::ea(i_this) + 0x39C, 0); /* attention_info.flags */
    i_this->mDamageTimer = 6;
    s16 delay = i_this->mInitialSpawnDelay;
    if (delay != 0) {
        i_this->mInitialSpawnDelay = delay - 1;
    } else if (spawn_switch_ok(i_this)) {
        if (fopAcM_searchPlayerDistance(i_this) < i_this->mNoticeRange * 100.0f) {
            /* view_check */
            gabi::Local<cXyz> pos;
            pos->x = (f32)i_this->current.pos.x;
            pos->y = (f32)i_this->current.pos.y;
            pos->z = (f32)i_this->current.pos.z;
            pos->y = i_this->current.pos.y + 100.0f;
            dBgS_LinChk_Set(linChk, gabi::at<cXyz>(camera + 0xDC), pos, i_this);
            bool appear = true;
            if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                gabi::Local<cXyz> tmp;
                gabi::Local<cXyz> d;
                cXyz_mi(gabi::at<cXyz>(camera + 0xE8), tmp, gabi::at<cXyz>(camera + 0xDC));
                d->z = (f32)tmp->z;
                d->y = (f32)tmp->y;
                d->x = (f32)tmp->x;
                s16 camAngle = cM_atan2s(d->x, d->z);
                cXyz_mi(pos, tmp, gabi::at<cXyz>(camera + 0xDC));
                d->copy(*tmp);
                cMtx_YrotS(calc_mtx(), -camAngle);
                MtxPosition(d, pos);
                /* in front of the camera: appears only when not hidden */
                if (!(pos->z < 0.0f) && i_this->mbHide != 0) appear = false;
            }
            if (appear) {
                i_this->mbHide = 0;
                i_this->actor_status |= 0x36;
                gabi::store<u32>(gabi::ea(i_this) + 0x39C, 4);
                i_this->mAction = 1;
                i_this->mMode = 0;
            }
        }
    }
    dBgS_LinChk_dt(linChk);
}

/* jump target found at the line check's cross point: move it by `ofs` and start jumping */
static inline void jump_target_set(pt_class* i_this, dBgS_LinChk_l* linChk, cXyz* ofs) {
    i_this->mJumpTarget.copy(*LinChk_GetCross(linChk));
    gabi::Local<cXyz> d;
    MtxPosition(ofs, d);
    PSVECAdd(&i_this->mJumpTarget, d, &i_this->mJumpTarget);
    dBgS_LinChk_dt(linChk);
    i_this->mMode = 1;
    gabi::Local<cXyz> tmp;
    gabi::Local<cXyz> diff;
    cXyz_mi(&i_this->mJumpTarget, tmp, &i_this->current.pos);
    diff->copy(*tmp);
    f32 h = std_sqrtf(PSVECSquareMag(diff)) * 0.1f;
    if (h > 50.0f) h = 50.0f;
    i_this->mDrawYSpeed = h;
}

/* pt_move mode 0: pick the next spot to jump to; false when there is none (back to pt_attack) */
static inline bool next_pos_set(pt_class* i_this) {
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::Local<cXyz> ofs;
    gabi::Local<cXyz> p1;
    gabi::Local<cXyz> p2;
    if ((u32)(i_this->current.angle.x + 0x1FFF) < 0x3FFF) {
        /* on the floor: jump towards the target (away from it when the switches say so) */
        s16 ang = i_this->mTargetAngleY;
        if (i_this->mTimers[1] != 0 || !spawn_switch_ok(i_this)) {
            ang -= 0x8000;
        }
        ang += (s16)gabi::ftoi(cM_rndFX(4000.0f));
        cMtx_YrotS(calc_mtx(), ang);
        cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
        ofs->x = 0.0f;
        ofs->y = 200.0f;
        ofs->z = 200.0f;
    } else {
        Mtx34* m = calc_mtx();
        s16 rnd = (s16)gabi::ftoi(cM_rndFX(4000.0f));
        s16 ang = i_this->current.angle.y + rnd;
        cMtx_YrotS(m, ang);
        cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
        ofs->x = 0.0f;
        ofs->z = 200.0f;
        ofs->y = 200.0f;
    }
    MtxPosition(ofs, p1);
    PSVECAdd(p1, &i_this->current.pos, p1);
    dBgS_LinChk_Set(linChk, &i_this->current.pos, p1, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        ofs->z = -2.0f;
        ofs->y = -2.0f;
        jump_target_set(i_this, linChk, ofs);
        return true;
    }
    ofs->y = -400.0f;
    ofs->z = 200.0f;
    MtxPosition(ofs, p2);
    PSVECAdd(p2, &i_this->current.pos, p2);
    dBgS_LinChk_Set(linChk, p1, p2, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        ofs->z = 0.0f;
        ofs->y = 2.0f;
        jump_target_set(i_this, linChk, ofs);
        return true;
    }
    ofs->z = -50.0f;
    ofs->y = -50.0f;
    MtxPosition(ofs, p1);
    PSVECAdd(p1, &i_this->current.pos, p1);
    dBgS_LinChk_Set(linChk, p2, p1, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        ofs->z = 2.0f;
        ofs->y = 0.0f;
        jump_target_set(i_this, linChk, ofs);
        return true;
    }
    dBgS_LinChk_dt(linChk);
    i_this->mAction = 2;
    i_this->mMode = 0;
    return false;
}

/* action 1: jumping from spot to spot */
static inline void pt_move(pt_class* i_this) {
    gabi::Local<cXyz> step;
    step->y = 0.0f;
    step->x = 0.0f;
    step->z = 20.0f;
    switch ((u16)i_this->mMode) {
    case 0:
        if (!next_pos_set(i_this)) return;
        break;
    case 1: {
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> d;
        cXyz_mi(&i_this->mJumpTarget, tmp, &i_this->current.pos);
        d->z = (f32)tmp->z;
        d->y = (f32)tmp->y;
        d->x = (f32)tmp->x;
        i_this->current.angle.y = cM_atan2s(d->x, d->z);
        f32 xz = std_sqrtf(gabi::fmadds(d->x, d->x, d->z * d->z));
        i_this->current.angle.x = -cM_atan2s(d->y, xz);
        s16 z = get_z_ang_(i_this);
        if (z != 0xDCF) i_this->current.angle.z = z;
        if (std_sqrtf(PSVECSquareMag(d)) < step->z * 1.5f) {
            /* arrived */
            if (cM_rndF(1.0f) < 0.02f && (u32)(i_this->current.angle.x + 0xFFF) < 0x1FFF) {
                i_this->mAction = 3;
                i_this->mMode = 0;
                smoke_set_(i_this, 3);
            } else {
                i_this->mMode = 2;
                i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(5.0f) + 2.0f);
                anm_init_(i_this, 0xC, 3.0f, 2, 1.0f, -1);
                se_start(i_this, JA_SE_OBJ_PT_JUMP);
            }
        } else {
            cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
            cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
            MtxPosition(step, &i_this->speed);
            PSVECAdd(&i_this->current.pos, &i_this->speed, &i_this->current.pos);
        }
        break;
    }
    case 2: {
        cLib_addCalc2(&i_this->current.pos.x, i_this->mJumpTarget.x, 1.0f, fabsf(i_this->speed.x));
        cLib_addCalc2(&i_this->current.pos.y, i_this->mJumpTarget.y, 1.0f, fabsf(i_this->speed.y));
        cLib_addCalc2(&i_this->current.pos.z, i_this->mJumpTarget.z, 1.0f, fabsf(i_this->speed.z));
        s16 z = get_z_ang_(i_this);
        s16 t = i_this->mTimers[0];
        if (z != 0xDCF) i_this->current.angle.z = z;
        if (t == 0) {
            i_this->mMode = 0;
            anm_init_(i_this, 9, 3.0f, 0, 1.0f, -1);
            mons_se_start(i_this, JA_SE_CV_PT_JUMP);
        }
        break;
    }
    }
    if (i_this->current.pos.y - i_this->mpTargetPos->y > -50.0f && i_this->mTargetDistXZ < 500.0f) {
        i_this->mAction = 2;
        i_this->mMode = 0;
    }
}

/* action 2: on the ground, running at the target and attacking */
static inline void pt_attack(pt_class* i_this) {
    f32 targetDist = i_this->mTargetDist;
    s16 target = i_this->mTargetAngleY;
    if (i_this->mTimers[1] != 0) target -= 0x8000;
    cLib_addCalcAngleS2(&i_this->current.angle.y, target, 2, 0x1000);
    cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 2, 0x1000);
    cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 2, 0x1000);
    u32 groundHit = i_this->mAcch.m_flags & dBgS_Acch::GROUND_HIT;
    if (groundHit && i_this->mOldSpeedY < -100.0f) {
        /* landed hard */
        i_this->mAction = 3;
        i_this->mMode = cM_rndF(1.0f) < 0.5f;
        smoke_set_(i_this, 5);
        return;
    }
    switch ((u16)i_this->mMode) {
    case 0:
        if (i_this->mTimers[0] == 0) {
            i_this->mMode = 1;
        } else {
            i_this->speed.x = 0.0f;
            i_this->speed.y = 0.0f;
            i_this->speed.z = 0.0f;
        }
        break;
    case 1:
        if (groundHit) {
            if (cM_rndF(1.0f) < 0.02f) {
                i_this->mAction = 3;
                i_this->mMode = 0;
                smoke_set_(i_this, 3);
            } else {
                i_this->mMode = 2;
                anm_init_(i_this, 7, 5.0f, 2, 1.0f, -1);
                i_this->speed.x = 0.0f;
                i_this->speed.y = 0.0f;
                i_this->speed.z = 0.0f;
                i_this->speedF = 0.0f;
                se_start(i_this, JA_SE_OBJ_PT_JUMP);
            }
        }
        break;
    case 2: {
        cLib_addCalc2(&i_this->speedF, 20.0f, 1.0f, 4.0f);
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        gabi::Local<cXyz> fwd;
        gabi::Local<cXyz> spd;
        fwd->x = 0.0f;
        fwd->y = 0.0f;
        fwd->z = i_this->speedF;
        MtxPosition(fwd, spd);
        i_this->speed.x = (f32)spd->x;
        i_this->speed.z = (f32)spd->z;
        if (targetDist < 300.0f) {
            gabi::Local<cXyz> pos;
            cXyz* tp = i_this->mpTargetPos;
            pos->x = (f32)tp->x;
            f32 y = tp->y;
            pos->y = y;
            pos->z = (f32)tp->z;
            pos->y = y + 100.0f;
            gabi::Local<dBgS_LinChk_l> linChk;
            dBgS_LinChk_ct(linChk);
            dBgS_LinChk_Set(linChk, &i_this->eyePos, pos, i_this);
            BOOL hit = cBgS_LineCross(dComIfG_Bgsp(), linChk);
            dBgS_LinChk_dt(linChk);
            if (!hit) {
                i_this->mTimers[2] = i_this->mBrkFrame * 3 + 10;
                i_this->mMode = 3;
                anm_init_(i_this, 0xC, 3.0f, 2, 1.0f, -1);
            }
        }
        break;
    }
    case 3: {
        if (i_this->mTimers[2] == 1) {
            anm_init_(i_this, 6, 5.0f, 0, 1.0f, -1);
            if (i_this != nullptr) {
                mons_se_start(i_this, JA_SE_CV_PT_LAND);
                se_start(i_this, JA_SE_OBJ_PT_LAND);
            }
        }
        i_this->speed.x *= 0.8f;
        i_this->speed.z *= 0.8f;
        if (i_this->mTimers[2] == 0) {
            f32 frame = i_this->mpMorf->getFrame();
            if (!(frame < 11.0f) && !(i_this->mpMorf->getFrame() > 14.0f)) {
                i_this->mbAttack = 1;
            }
        }
        if (i_this->mpMorf->isStop()) {
            i_this->mMode = 1;
        }
        break;
    }
    }
    PSVECAdd(&i_this->current.pos, &i_this->speed, &i_this->current.pos);
    f32 vy = i_this->speed.y - 7.0f;
    if (vy < -120.0f) vy = -120.0f;
    u32 flags = i_this->mAcch.m_flags;
    i_this->speed.y = vy;
    i_this->mbBgCheck = 1;
    if ((flags & dBgS_Acch::GROUND_HIT) && i_this->mTargetDist > 700.0f) {
        i_this->mAction = 1;
        i_this->mMode = 0;
    }
}

static inline void fall_move(pt_class* i_this) {
    f32 vy = i_this->speed.y;
    i_this->mbBgCheck = 1;
    i_this->current.pos.y = i_this->current.pos.y + vy;
    i_this->speed.y = vy - 7.0f;
}

/* action 3: tripped / knocked over */
static inline void pt_koke(pt_class* i_this) {
    cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 2, 0x2000);
    cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 2, 0x2000);
    bool end = false;
    switch ((u16)i_this->mMode) {
    case 0:
        i_this->mMode = 4;
        anm_init_(i_this, 0xA, 2.0f, 0, 1.0f, -1);
        i_this->mTimers[0] = 0;
        if (i_this != nullptr) mons_se_start(i_this, JA_SE_CV_PT_KOKE);
        break;
    case 1:
        i_this->mMode = 5;
        anm_init_(i_this, 0xB, 2.0f, 0, 1.0f, -1);
        i_this->mTimers[0] = 0;
        if (i_this != nullptr) {
            mons_se_start(i_this, JA_SE_CV_PT_KOKE);
            se_start(i_this, JA_SE_OBJ_PT_FALL);
        }
        break;
    case 2:
        i_this->mMode = 5;
        anm_init_(i_this, 8, 2.0f, 2, 1.0f, -1);
        i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(20.0f) + 20.0f);
        if (i_this != nullptr) mons_se_start(i_this, JA_SE_CV_PT_KOKE2);
        break;
    case 4:
        if (gabi::ftoi(i_this->mpMorf->getFrame()) == 6 && i_this != nullptr) se_start(i_this, JA_SE_OBJ_PT_FALL);
        /* fallthrough */
    case 5:
        if (i_this->mTimers[1] == 0) {
            if (i_this->mpMorf->isStop() || i_this->mTimers[0] == 1) end = true;
        }
        break;
    case 10:
        i_this->mMode = 11;
        anm_init_(i_this, 0xA, 2.0f, 0, 1.0f, -1);
        if (i_this != nullptr) mons_se_start(i_this, JA_SE_CV_PT_KOKE);
        break;
    case 11:
        i_this->current.angle.y = i_this->current.angle.y + i_this->mSpinSpeed;
        if (gabi::ftoi(i_this->mpMorf->getFrame()) == 6) se_start(i_this, JA_SE_OBJ_PT_FALL);
        if (i_this->mKnockSpeed < 0.05f) {
            i_this->mMode = 12;
            i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(30.0f));
        }
        break;
    case 12:
        if (i_this->mTimers[0] == 0) end = true;
        break;
    }
    if (end) {
        i_this->mAction = 1;
        i_this->mMode = 0;
        anm_init_(i_this, 9, 3.0f, 0, 1.0f, -1);
    }
    fall_move(i_this);
}

/* action 4: pressed flat (returns: dies) */
static inline int pt_ples(pt_class* i_this) {
    i_this->current.angle.z = 0;
    i_this->current.angle.x = 0;
    i_this->mDamageTimer = 5;
    switch ((u16)i_this->mMode) {
    case 0:
        i_this->mTimers[0] = 0x23;
        i_this->mMode = 1;
        anm_init_(i_this, 0xC, 1.0f, 0, 5.0f, -1);
        /* fallthrough */
    case 1:
        cLib_addCalc2(&i_this->scale.y, 0.1f, 1.0f, 0.5f);
        cLib_addCalc2(&i_this->scale.x, 1.3f, 0.8f, 0.5f);
        if (i_this->mTimers[0] == 0) return 1;
        break;
    }
    fall_move(i_this);
    return 0;
}

/* action 5: batted away (returns: dies) */
static inline int pt_bat(pt_class* i_this) {
    i_this->mDamageTimer = 5;
    switch ((u16)i_this->mMode) {
    case 0: {
        i_this->mTimers[0] = 0x96;
        i_this->mMode = 1;
        anm_init_(i_this, 0xC, 1.0f, 0, 5.0f, -1);
        s16 a = fopAcM_searchPlayerAngleY(i_this);
        s16 ang = a + (s16)gabi::ftoi(cM_rndFX(6000.0f)) + 0x8000;
        i_this->mKnockAngle = ang;
        cMtx_YrotS(calc_mtx(), ang);
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->y = cM_rndF(20.0f) + 50.0f;
        v->z = 100.0f;
        MtxPosition(v, &i_this->speed);
        i_this->current.angle.y = (s16)gabi::ftoi(cM_rndFX(32768.0f));
    }
        /* fallthrough */
    case 1:
        i_this->current.angle.y = i_this->current.angle.y + 0x400;
        i_this->current.angle.x = i_this->current.angle.x + 0x300;
        if (i_this->mTimers[0] == 0) return 1;
        break;
    }
    i_this->mbBgCheck = 1;
    PSVECAdd(&i_this->current.pos, &i_this->speed, &i_this->current.pos);
    f32 vy = i_this->speed.y - 5.0f;
    if (vy < -120.0f) vy = -120.0f;
    u32 flags = i_this->mAcch.m_flags;
    i_this->speed.y = vy;
    if (flags & dBgS_Acch::WALL_HIT) return 1;
    if (vy < 0.0f && (flags & dBgS_Acch::GROUND_HIT)) return 1;
    return 0;
}

/* hits taken and dealt */
static inline void damage_check(pt_class* i_this, fopAc_ac_c* player) {
    i_this->mStts.Move();
    if (i_this->mAtSph.ChkAtHit()) {
        void* hitObj = GetAtHitObj(&i_this->mAtSph);
        if (hitObj != nullptr) {
            u32 stts = gabi::load<u32>(gabi::ea(hitObj) + 0x44);
            fopAc_ac_c* hitAc = stts != 0 ? gabi::at<fopAc_ac_c>(gabi::load<u32>(stts + 0xC)) : nullptr;
            if (hitAc != nullptr && fpcM_GetName(hitAc) == PROC_PLAYER) {
                /* hit the player: bounce off a guard */
                u32 fn = gabi::load<u32>(player->__vtbl + 0x3C);
                if (gabi::call_ptr<BOOL>(fn, player)) {
                    i_this->mKnockAngle = fopAcM_searchPlayerAngleY(i_this);
                    i_this->mKnockSpeed = 40.0f;
                } else {
                    i_this->mAction = 3;
                    i_this->mMode = 2;
                    i_this->mDamageTimer = 6;
                    return;
                }
            }
        }
    }
    if (gabi::load<u32>(gabi::ea(player) + 0x3C0) & 0x20000) {
        /* the player's spin / shockwave nearby */
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> d;
        cXyz_mi(&player->current.pos, tmp, &i_this->current.pos);
        d->copy(*tmp);
        if (std_sqrtf(PSVECSquareMag(d)) < 350.0f) {
            i_this->mAction = 3;
            i_this->mMode = 0;
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(30.0f) + 20.0f);
            smoke_set_(i_this, 3);
            i_this->mDamageTimer = 10;
            i_this->speed.y = 350.0f;
            return;
        }
    }
    if (!i_this->mSph.ChkTgHit()) return;

    i_this->mDamageTimer = 6;
    gabi::Local<CcAtInfo_l> atInfo;
    void* hitObj = i_this->mSph.GetTgHitObj();
    atInfo->mpObj = gabi::ea(hitObj);
    atInfo->pParticlePos = gabi::ea(&i_this->mSph.mGObjTg.mHitPos);
    if (gabi::load<u32>(gabi::ea(hitObj) + 0x10) & 0x100000) {
        /* light arrow */
        i_this->mEnemyIce.mLightShrinkTimer = 1;
        enemy_fire_remove(&i_this->mEnemyFire);
        i_this->mSmokeType = 0;
        dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->mSmokeCb);
        return;
    }
    if (gabi::load<u32>(gabi::ea(hitObj) + 0x10) & 0x40200) {
        /* fire */
        i_this->mEnemyFire.mFireDuration = 100;
        i_this->mDamageTimer = 50;
    }
    if (gabi::load<u32>(gabi::ea(hitObj) + 0x10) & 0x200000) {
        i_this->mKnockAngle = fopAcM_searchPlayerAngleY(i_this);
        smoke_set_(i_this, 5);
        i_this->mKnockSpeed = cM_rndF(30.0f) + 90.0f;
        i_this->mSpinSpeed = (s16)gabi::ftoi(cM_rndFX(2000.0f));
        i_this->mAction = 3;
        i_this->mMode = 10;
        smoke_set_(i_this, 10);
        if (i_this != nullptr) mons_se_start(i_this, JA_SE_CV_PT_DAMAGE);
        return;
    }
    cc_at_check(i_this, atInfo);
    if (atInfo->mResultingAttackType == 9) {
        if (gabi::load<u8>(gabi::ea(player) + 0x3AC) == 0x11) {
            i_this->mAction = 5;
            smoke_set_(i_this, 0x1E);
            i_this->mMode = 0;
        } else {
            i_this->mAction = 4;
            i_this->mMode = 0;
        }
        if (i_this != nullptr) mons_se_start(i_this, JA_SE_CV_PT_DIE);
        i_this->mDamageTimer = 50;
        return;
    }
    i_this->mKnockAngle = fopAcM_searchPlayerAngleY(i_this);
    smoke_set_(i_this, 10);
    if (i_this->health > 0) {
        mons_se_start(i_this, JA_SE_CV_PT_DAMAGE);
        i_this->mAction = 2;
        i_this->mMode = 1;
        i_this->mKnockSpeed = 80.0f;
    } else {
        i_this->mDamageTimer = 100;
        mons_se_start(i_this, JA_SE_CV_PT_DIE);
        i_this->mAction = 2;
        i_this->mKnockSpeed = 110.0f;
        i_this->mMode = 1;
    }
}

/* below the ground or in the sea (returns: dies) */
static inline int water_check(pt_class* i_this) {
    int dead = 0;
    gabi::Local<dBgS_GndChk_l> gndChk;
    dBgS_GndChk_ct(gndChk);
    gndChk->m_pos.x = (f32)i_this->current.pos.x;
    gndChk->m_pos.z = (f32)i_this->current.pos.z;
    gndChk->m_pos.y = i_this->current.pos.y + 500.0f;
    f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
    if (groundY != -1e9f && !(i_this->current.pos.y > groundY)) {
        dead = 1;
    } else if (daSea_ChkArea(i_this->current.pos.x, i_this->current.pos.z)) {
        f32 h = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z) - 30.0f;
        if (!(i_this->current.pos.y > h)) {
            i_this->current.pos.y = h;
            gabi::Local<cXyz> pos;
            pos->x = (f32)i_this->current.pos.x;
            pos->y = (f32)i_this->current.pos.y;
            pos->y = h;
            pos->z = (f32)i_this->current.pos.z;
            fopKyM_createWpillar(pos, 1.0f, 1.0f, 0);
            se_start(i_this, JA_SE_OBJ_FALL_WATER_S);
            dead = 1;
        }
    }
    dBgS_GndChk_dt(gndChk);
    return dead;
}

/* 02447300 */
void action(pt_class* i_this) {
    WWHD_FUNC(0x02447300, void, i_this);
    fopAc_ac_c* esa = (fopAc_ac_c*)fpcM_Search(0x02446324 /* esa_s_sub */, i_this);
    if (esa != nullptr) {
        i_this->mpTargetPos = &esa->current.pos;
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> d;
        cXyz_mi(&esa->current.pos, tmp, &i_this->current.pos);
        d->copy(*tmp);
        i_this->mTargetDist = std_sqrtf(PSVECSquareMag(d));
        d->y = 0.0f;
        i_this->mTargetDistXZ = std_sqrtf(PSVECSquareMag(d));
        i_this->mTargetAngleY = cM_atan2s(d->x, d->z);
    } else {
        i_this->mpTargetPos = &dComIfGp_getPlayer(0)->current.pos;
        i_this->mTargetDist = fopAcM_searchPlayerDistance(i_this);
        i_this->mTargetDistXZ = fopAcM_searchPlayerDistanceXZ(i_this);
        i_this->mTargetAngleY = fopAcM_searchPlayerAngleY(i_this);
    }

    int dead = 0;
    switch ((u16)i_this->mAction) {
    case 0: pt_wait(i_this); break;
    case 1: pt_move(i_this); break;
    case 2: pt_attack(i_this); break;
    case 3: pt_koke(i_this); break;
    case 4: dead = pt_ples(i_this); break;
    case 5: dead = pt_bat(i_this); break;
    }

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (i_this->mDamageTimer == 0) {
        damage_check(i_this, player);
    }
    dead += water_check(i_this);

    if (i_this->mKnockSpeed > 0.01f) {
        /* knock-back */
        i_this->current.pos.x = (f32)i_this->old.pos.x;
        i_this->current.pos.z = (f32)i_this->old.pos.z;
        gabi::Local<cXyz> v;
        gabi::Local<cXyz> mv;
        f32 s = i_this->mKnockSpeed;
        s16 a = i_this->mKnockAngle;
        v->y = 0.0f;
        v->x = 0.0f;
        v->z = -s;
        cMtx_YrotS(calc_mtx(), a);
        MtxPosition(v, mv);
        PSVECAdd(&i_this->current.pos, mv, &i_this->current.pos);
        cLib_addCalc0(&i_this->mKnockSpeed, 1.0f, 7.0f);
        if (i_this->health <= 0) {
            if ((i_this->mAcch.m_flags & dBgS_Acch::WALL_HIT) || i_this->mKnockSpeed < 0.1f) dead++;
        }
        i_this->mbBgCheck = 1;
    }

    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 4, 0x2000);
    cLib_addCalcAngleS2(&i_this->shape_angle.x, i_this->current.angle.x, 4, 0x1000);
    cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 4, 0x1000);

    /* the hop drawn on top of the movement */
    f32 dy = i_this->mDrawYSpeed;
    f32 y = i_this->mDrawYOffset + dy;
    i_this->mDrawYOffset = y;
    i_this->mDrawYSpeed = dy - 5.0f;
    if (y < 0.0f) i_this->mDrawYOffset = 0.0f;

    if (i_this->mSmokeType != 0) {
        s8 t = i_this->mSmokeType - 1;
        i_this->mSmokeType = t;
        if (t == 0) dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->mSmokeCb);
    }

    if (dead != 0) {
        gabi::Local<cXyz> pos;
        pos->z = (f32)i_this->current.pos.z;
        pos->x = (f32)i_this->current.pos.x;
        pos->y = i_this->current.pos.y + 30.0f;
        fopAcM_createDisappear(i_this, pos, 5, 4, 0xFF);
        fopAcM_delete(i_this);
        dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
        if (i_this->mBehaviorType == 0) {
            i_this->mbRespawn = 1;
            return;
        }
        if (i_this->mDisableRespawnSwitch != 0xFF) {
            dComIfGs_onSwitch(i_this->mDisableRespawnSwitch, dComIfGp_roomControl_getStayNo());
        }
    }
}
VERIFY(0x02447300, action);
