/**
 * d_a_himo2_move.cpp (WWHD)
 * Item - Grappling Hook / Rope: new_himo2_move (the rope state machine and the Helmaroc /
 * Gohma-style camera sequences)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_himo2.cpp, "Nonmatching - retail-only regalloc") to the WWHD
 * layout and code, verified against cking.rpx. "HD:" marks where WWHD's code differs.
 *
 * HD keeps the debug registers (REG0_F/REG0_S/REG8_F) of this TU; they are read through
 * HIMO2_REG(offset) with the offsets of the HD code.
 */
#include "d/actor/d_a_himo2.h"

enum {
    JA_SE_LK_ROPE_LAUNCH = 0x2817,
    JA_SE_LK_ROPE_COIL_1 = 0x2818,
    JA_SE_LK_ROPE_COIL_2 = 0x2819,
    JA_SE_LK_ROPE_COIL_3 = 0x281A,
    JA_SE_LK_ROPE_HOOK_WOOD = 0x281B,
    JA_SE_LK_ROPE_MAXLENGTH = 0x2820,
    JA_SE_LK_ROPE_UNCOIL = 0x2821,
    JA_SE_LK_ROPE_HOOK_BODY = 0x2830,
    JA_SE_LK_ROPE_SWING_ROUND = 0x2842,
    JA_SE_LK_ROPE_UNWIND = 0x201F,
    JA_SE_CV_DRG_ROCKFALL_1 = 0x483F,
    JA_SE_CV_DRG_ROCKFALL_1_2 = 0x4844,
    JA_SE_CV_DRG_ROCKFALL_1_3 = 0x4845,
    JA_SE_CV_DRG_ROCKFALL_2 = 0x4840,
    JA_SE_CV_DRG_SET_ROPE = 0x4841,
    JA_SE_CM_BTD_ROPE_SET = 0x5827,
    JA_SE_CM_BTD_BEF_ROCK_FALL = 0x5829,
    fpcNm_KUI_e = 0xFA,
    fpcNm_BK_e = 0xBD,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void cLib_addCalcPos2(cXyz* v, const cXyz* t, f32 scale, f32 maxStep) { gabi::call(0x0200F164, v, t, scale, maxStep); }
static inline u32 mv_LockonTruth(u32 att) { return gabi::call<u32>(0x024EDFCC, att); }
/* 024EC8D0 dAttention_c::LockonTarget(s32) (matcher: ActionTarget) */
static inline u32 mv_LockonTarget(u32 att, s32 i) { return gabi::call<u32>(0x024EC8D0, att, i); }
static inline u32 mv_padTrigX() { return gabi::call<u32>(0x020078E8, 0); }
static inline u32 mv_padTrigY() { return gabi::call<u32>(0x02007914, 0); }
static inline u32 mv_padTrigZ() { return gabi::call<u32>(0x02007814, 0); }
static inline f32 mv_subStickY() { return gabi::call<f32>(0x02007B20, 0); }
static inline f32 mv_subStickX() { return gabi::call<f32>(0x02007AFC, 0); }
static inline void mv_camStop(u32 cam) { gabi::call(0x02514F2C, cam + 0x248); }
static inline void mv_camStart(u32 cam) { gabi::call(0x02514F38, cam + 0x248); }
static inline void mv_camSetTrimSize(u32 cam, s32 s) { gabi::call(0x02515280, cam + 0x248, s); }
static inline void mv_camReset(u32 cam, cXyz* c, cXyz* e) { gabi::call(0x0251510C, cam + 0x248, c, e); }
static inline void mv_camSet(u32 cam, cXyz* c, cXyz* e, s16 fovy_bank, f32 f) { gabi::call(0x02514FE8, cam + 0x248, c, e, fovy_bank, f); }
static inline void mv_colset(f32 f) { gabi::call(0x0255FDA4, 0, 4, f); } /* dKy_custom_colset(0, 4, f) */
static inline u32 mv_saveEvent() { return gabi::load<u32>(0x101F84DC) + 0x644; }
static inline void mv_onEventBit(u16 f) { gabi::call(0x025B8B68, mv_saveEvent(), f); }
static inline u32 mv_isEventBit(u16 f) { return gabi::call<u32>(0x025B8B94, mv_saveEvent(), f); }
static inline void mv_event_reset() {
    u32 p = dComIfGp_ea();
    gabi::store<u16>(p + 0x52B8, (u16)(gabi::load<u16>(p + 0x52B8) | 8));
}
static inline void mv_orderPotentialEvent(fopAc_ac_c* a, u16 flag) { gabi::call(0x025D7B24, a, flag, (u16)0xFFFF, 0); }
static inline fopAc_ac_c* mv_player() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER)); }
/* fopAcM_seStart (HD: checks &eyePos != NULL; the actor pointer itself is not checked) */
static inline void mv_seStart(fopAc_ac_c* a, u32 id) {
    if (gabi::ea(a) + 0x37C != 0)
        mDoAud_seStart(id, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* fopAcM_seStart(player, ...) (HD: checks the player and its eyePos) */
static inline void mv_seStartP(fopAc_ac_c* a, u32 id) {
    if (a != nullptr && gabi::ea(a) + 0x37C != 0)
        mDoAud_seStart(id, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* fopAcM_seStartCurrent (HD: checks the actor and its position) */
static inline void mv_seStartCurrent(u32 a, u32 id) {
    if (a != 0 && a + 0x314 != 0)
        mDoAud_seStart(id, gabi::at<cXyz>(a + 0x314), 0, dComIfGp_getReverb(gabi::load<s8>(a + 0x326)));
}
static inline void mv_seStart0(u32 id) { mDoAud_seStart(id, nullptr, 0, 0); }
static inline void mv_wcopy3(u32 d, u32 s) {
    gabi::store<u32>(d + 0, gabi::load<u32>(s + 0));
    gabi::store<u32>(d + 4, gabi::load<u32>(s + 4));
    gabi::store<u32>(d + 8, gabi::load<u32>(s + 8));
}
static inline f32 regF(u32 o) { return gabi::load<f32>(HIMO2_REG(o)); }
static inline s16 regS(u32 o) { return gabi::load<s16>(HIMO2_REG(o)); }
/* btd (0x10464678) and dr (0x1046467C) fields used here */
static inline void btd_setBlur(u8 v) { gabi::store<u8>(himo2_btd + 0x7059, v); }       /* btd->m6E15 */
static inline void btd_onStatus4000() { u32 b = himo2_btd; gabi::store<u32>(b + 0x2E0, gabi::load<u32>(b + 0x2E0) | 0x4000); }
static inline void btd_offStatus4000() { u32 b = himo2_btd; gabi::store<u32>(b + 0x2E0, gabi::load<u32>(b + 0x2E0) & ~0x4000u); }
/* daYkgr_c::hide / show: the ykgr instance at 0x10475628, flags +0x254 */
static inline void ykgr_hide() { u32 y = gabi::load<u32>(0x10475628); if (y) gabi::store<u32>(y + 0x254, gabi::load<u32>(y + 0x254) | 4); }
static inline void ykgr_show() { u32 y = gabi::load<u32>(0x10475628); if (y) gabi::store<u32>(y + 0x254, gabi::load<u32>(y + 0x254) & ~4u); }
static inline f32 mv_abs(cXyz* v) { return std_sqrtf(PSVECSquareMag(v)); }
static inline void mv_pl_pos_add(himo2_class* i_this) { gabi::call(0x0216E144, i_this); }
static inline BOOL mv_bg_check(himo2_class* i_this) { return gabi::call<BOOL>(0x0216DE7C, i_this); }
static inline void mv_posMove(himo2_class* i_this) {
    /* speed = Rotation(angle) * (0, 0, speedF); pos += speed */
    gabi::Local<cXyz> sp130;
    sp130->x = 0.0f;
    sp130->y = 0.0f;
    sp130->z = i_this->speedF;
    MtxPosition(sp130, &i_this->speed);
    gabi::call(0x028E8D88, &i_this->current.pos, &i_this->speed, &i_this->current.pos); /* PSVECAdd */
}
/* |pos - sp100| (cXyz::operator- then abs of the copy) */
static inline f32 mv_dist(himo2_class* i_this, cXyz* sp100) {
    gabi::Local<cXyz> tmp;
    gabi::call(0x0201ADE0, &i_this->current.pos, tmp.get(), sp100);
    gabi::Local<cXyz> sp130;
    mv_wcopy3(gabi::ea(sp130.get()), gabi::ea(tmp.get()));
    return mv_abs(sp130);
}
/* fopAcM_SearchByID (HD inline): fopAcIt_Judge(fpcSch_JudgeByID, &id) unless id == -1 */
static inline fopAc_ac_c* mv_searchByID(u32 id) {
    gabi::Local<be<u32>> pid;
    *pid.get() = id;
    if (id == 0xFFFFFFFF)
        return nullptr;
    return gabi::call<fopAc_ac_c*>(0x025D5218, (u32)0x025E1234, pid.get());
}

/* cases 5/8/9: pull the hook back to the hand */
static inline bool mv_pull_back(himo2_class* i_this, cXyz* sp100) {
    f32 step = (regF(0xA5C) + 400.0f) * i_this->m2500;
    cLib_addCalcPos2(&i_this->current.pos, sp100, 1.0f, step);
    cLib_addCalc2(&i_this->m2500, 1.0f, 1.0f, regF(0xA60) + 0.1f);
    mv_pl_pos_add(i_this);
    return mv_dist(i_this, sp100) > 5.0f;
}

/* the rope wound in the spin (cases 0 and 1) */
static inline void mv_spin(himo2_class* i_this, fopAc_ac_c* player, cXyz* sp100) {
    mDoMtx_YrotS(calc_mtx(), player->shape_angle.y);
    mDoMtx_ZrotM(calc_mtx(), (s16)(regS(0x84) + -12000));
    mDoMtx_YrotM(calc_mtx(), (s16)((s32)i_this->m02D8 * (regS(0x86) + 0x2000)));
    gabi::Local<cXyz> sp130;
    sp130->x = 0.0f;
    sp130->y = 0.0f;
    sp130->z = regF(0x34) + 70.0f;
    MtxPosition(sp130, &i_this->current.pos);
    gabi::call(0x028E8D88, &i_this->current.pos, sp100, &i_this->current.pos); /* PSVECAdd */
    i_this->m02CC = regS(0x8A) + 0x60;
}

/* 0216F7FC */
static void new_himo2_move(himo2_class* i_this) {
    WWHD_FUNC(0x0216F7FC, void, i_this);
    fopAc_ac_c* player = mv_player();
    s8 camId = gabi::load<s8>(dComIfGp_ea() + 0x5B30); /* dComIfGp_getPlayerCameraID(0) */
    u32 play = dComIfGp_ea();
    f32 sz = i_this->m02EC[0].z;
    f32 sx = i_this->m02EC[0].x;
    f32 sy = i_this->m02EC[0].y;
    u32 camera = gabi::load<u32>(play + 0x5AF8 + camId * 0x34);
    gabi::Local<cXyz> sp100;
    sp100->x = sx;
    sp100->y = sy;
    sp100->z = sz;
    f32 dy = i_this->current.pos.y - sy;
    f32 dx = i_this->current.pos.x - sx;
    f32 dz = i_this->current.pos.z - sz;
    i_this->m2188 = std_sqrtf(gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy)));
    if (i_this->m0308 != 0) i_this->m0308 = i_this->m0308 - 1;
    if (i_this->m02A2 != 0) i_this->m02A2 = i_this->m02A2 - 1;
    if (i_this->m029C != 0) i_this->m029C = i_this->m029C - 1;
    if (i_this->m029E != 0) i_this->m029E = i_this->m029E - 1;
    if (i_this->m02A0 != 0) i_this->m02A0 = i_this->m02A0 - 1;

    u32 att = dComIfGp_ea() + 0x5804; /* dComIfGp_getAttention() */
    bool lockon = true;
    if (mv_LockonTruth(att) == 0 && (gabi::load<u32>(att + 0x20) & 0x20000000) == 0)
        lockon = false;
    bool r30 = !lockon; /* attention->Lockon() == false */
    bool r27 = false;   /* a lock-on target */
    bool r25 = false;
    fopAc_ac_c* r24 = nullptr;
    if (r30) {
        rope_scale = l_himo2HIO.m20;
    } else {
        rope_scale = l_himo2HIO.m1C;
        i_this->m029E = 0;
    }
    if (mv_LockonTruth(att) != 0) {
        r24 = gabi::at<fopAc_ac_c>(mv_LockonTarget(att, 0));
        if (r24 != nullptr) {
            mv_wcopy3(gabi::ea(&i_this->m2524), gabi::ea(&r24->eyePos));
            r27 = true;
        }
    }
    if (i_this->m02DC != 0 && i_this->m24D9 < 3) {
        u32 link = gabi::load<u32>(dComIfGp_ea() + 0x5B34); /* daPy_getPlayerLinkActorClass() */
        /* checkRopeForceEnd() && !eventInfo.checkCommandDemoAccrpt() */
        if ((gabi::load<u32>(link + 0x3C0) & 0x40000000) && gabi::load<u16>(gabi::ea(i_this) + 0xF8) != 2) {
            i_this->m24D9 = 0;
            i_this->m0308 = 30;
            s32 dc = i_this->m02DC;
            i_this->mParameters = 0;
            i_this->m24D8 = 0;
            if (dc == 11) {
                mv_seStart(i_this, JA_SE_LK_ROPE_UNCOIL);
                i_this->m02DC = 0;
            } else {
                i_this->m02DC = 9;
            }
            fopAc_ac_c* t = i_this->m217C;
            if (t != nullptr && fpcM_GetName(t) == fpcNm_KUI_e)
                t->health = 0;
            i_this->m217C = nullptr;
        }
    }

    switch ((u32)i_this->m02DC) {
    case 0: {
        i_this->speedF = 0.0f;
        if (!r30 && gabi::call_ptr<u32>(gabi::load<u32>(player->__vtbl + 0xDC), player) != 0) { /* checkRopeReadyAnime */
            mv_spin(i_this, player, sp100);
            if ((i_this->m02D8 & 7) == 0)
                mv_seStart(i_this, JA_SE_LK_ROPE_COIL_1);
        } else {
            i_this->m02CC = 0x61 - gabi::ftoi(i_this->m2188 * (regF(0x14) + 0.060000002f));
            cLib_addCalc2(&i_this->current.pos.x, sp100->x, 1.0f, 200.0f * i_this->m2500);
            cLib_addCalc2(&i_this->current.pos.y, sp100->y, 1.0f, 200.0f * i_this->m2500);
            cLib_addCalc2(&i_this->current.pos.z, sp100->z, 1.0f, 200.0f * i_this->m2500);
            cLib_addCalc2(&i_this->m2500, 1.0f, 1.0f, 0.005f);
            if (mv_dist(i_this, sp100) > 5.0f)
                mv_seStartP(player, JA_SE_LK_ROPE_UNWIND);
        }
        if (i_this->mParameters == 1) {
            i_this->m02E4 = 0.0f;
            i_this->m2500 = 0.0f;
            i_this->m029E = 0;
            i_this->m0308 = regS(0xAA0) + 0x14;
            i_this->m02A0 = regS(0x92) + 10;
            i_this->m02A2 = 3;
            u32 cam2 = gabi::load<u32>(dComIfGp_ea() + 0x5AF8); /* dComIfGp_getCamera(0) */
            f32 f26 = i_this->m2524.y - gabi::load<f32>(cam2 + 0xE0);
            f32 f25 = i_this->m2524.z - gabi::load<f32>(cam2 + 0xE4);
            f32 f27 = i_this->m2524.x - gabi::load<f32>(cam2 + 0xDC);
            f32 f24 = f27 * f27;
            f32 f23 = f25 * f25;
            gabi::Local<cXyz> sp130;
            sp130->z = gabi::fmadds(regF(0x44), 100.0f, 1000.0f);
            if (std_sqrtf(gabi::fmadds(f26, f26, f24) + f23) > sp130->z) {
                s16 r24_2 = cM_atan2s(f27, f25);
                s16 r22 = (s16)-cM_atan2s(f26, std_sqrtf(f24 + f23));
                mDoMtx_YrotS(calc_mtx(), r24_2);
                mDoMtx_XrotM(calc_mtx(), r22);
                sp130->x = 0.0f;
                sp130->y = 0.0f;
                gabi::Local<cXyz> sp124;
                MtxPosition(sp130, sp124);
                i_this->m2524.x = gabi::load<f32>(cam2 + 0xDC) + sp124->x;
                i_this->m2524.y = gabi::load<f32>(cam2 + 0xE0) + sp124->y;
                i_this->m2524.z = gabi::load<f32>(cam2 + 0xE4) + sp124->z;
            }
            if (r30) {
                i_this->m02DC = 2;
            } else {
                i_this->m02DC = 1;
                i_this->m0308 = regS(0x88) + 2;
            }
            i_this->m2530 = regF(0xA60) + 4.0f;
            mv_wcopy3(gabi::ea(&i_this->current.pos), gabi::ea(sp100.get()));
            i_this->m02CC = regS(0x8C) + 0x5A;
            himo2_s* phVar18 = i_this->m0310;
            for (int iVar8 = 0; iVar8 < 100; iVar8++, phVar18++)
                mv_wcopy3(gabi::ea(&phVar18->m10), gabi::ea(sp100.get()));
        }
        break;
    }
    case 1: {
        mv_spin(i_this, player, sp100);
        if ((i_this->m02D8 & 7) == 0)
            mv_seStart(i_this, JA_SE_LK_ROPE_SWING_ROUND);
        s16 r4 = (s16)((s32)i_this->m02D8 * (regS(0x86) + 0x2000));
        s16 r8 = regS(0x8E);
        s16 lo = (s16)(r8 + -20000);
        if (r4 >= lo && r4 < (s16)(r8 + -0x2E20))
            r25 = true;
        if (i_this->m0308 == 0 && r25) {
            i_this->m02DC = 3;
            if (r27) {
                i_this->m217C = r24;
                i_this->m0308 = regS(0x88) + 70;
                i_this->m2180 = r24 != nullptr ? gabi::load<u32>(gabi::ea(r24) + 4) : 0xFFFFFFFF; /* fopAcM_GetID */
            } else {
                i_this->m2180 = 0xFFFFFFFF;
                i_this->m0308 = regS(0x84) + 20;
            }
            gabi::Local<cXyz> tmp;
            gabi::call(0x0201ADE0, &i_this->m2524, tmp.get(), &i_this->current.pos); /* cXyz::operator- */
            gabi::Local<cXyz> sp130;
            f32 y = tmp->y, x = tmp->x, z = tmp->z;
            sp130->x = x;
            sp130->z = z;
            sp130->y = y;
            i_this->current.angle.y = cM_atan2s(x, z);
            f32 zz = sp130->z;
            f32 xx = sp130->x;
            f32 d = std_sqrtf(gabi::fmadds(xx, xx, zz * zz));
            i_this->current.angle.x = (s16)-cM_atan2s(sp130->y, d);
            mv_seStart(i_this, JA_SE_LK_ROPE_LAUNCH);
        }
        break;
    }
    case 2: {
        i_this->m02CC = regS(0x80);
        f32 f27, f26, f25;
        fopAc_ac_c* t = i_this->m217C;
        if (t != nullptr) {
            /* HD: always the target's position (GameCube: m2524 for plain kui) */
            f32 tz = t->current.pos.z, tx = t->current.pos.x;
            s16 ay = i_this->current.angle.y;
            f32 ty = t->current.pos.y;
            mDoMtx_YrotS(calc_mtx(), ay);
            mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
            gabi::Local<cXyz> sp130;
            sp130->y = 0.0f;
            sp130->x = 0.0f;
            sp130->z = gabi::fmadds(regF(0x40), 100.0f, 100.0f);
            gabi::Local<cXyz> sp124;
            MtxPosition(sp130, sp124);
            f27 = (tx + sp124->x) - i_this->current.pos.x;
            f25 = (tz + sp124->z) - i_this->current.pos.z;
            f26 = (ty + sp124->y) - i_this->current.pos.y;
        } else {
            f27 = i_this->m2524.x - i_this->current.pos.x;
            f26 = i_this->m2524.y - i_this->current.pos.y;
            f25 = i_this->m2524.z - i_this->current.pos.z;
        }
        f32 f24 = f27 * f27;
        f32 f23 = f25 * f25;
        f32 f22 = std_sqrtf(gabi::fmadds(f26, f26, f24) + f23);
        f32 f27_2 = std_sqrtf(f24 + f23);
        s16 r31 = (s16)gabi::ftoi(f22 * (regF(0x14) + -5.0f));
        s16 lim = (s16)(regS(0x84) + -3000);
        if (r31 < lim)
            r31 = lim;
        if (i_this->m217C != nullptr || f22 > i_this->speedF * 10.0f) {
            i_this->current.angle.y = cM_atan2s(f27, f25);
            i_this->current.angle.x = (s16)-cM_atan2s(f26, f27_2);
        }
        i_this->speedF = regF(0xA28) + 50.0f; /* HD: 50 (GameCube 20) */
        mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        mDoMtx_XrotM(calc_mtx(), (s16)(i_this->current.angle.x + r31));
        mv_posMove(i_this);
        mv_pl_pos_add(i_this);
        if (i_this->m217C != nullptr) {
            /* HD: also when m0308 <= 10 (GameCube: m0308 == 0) */
            if (f22 < i_this->speedF * 10.0f || i_this->m0308 <= 10) {
                i_this->m02DC = 10;
                i_this->m24D9 = -1;
                i_this->m24D8 = 1;
                if (himo2_btd != 0)
                    btd_offStatus4000();
                break;
            }
            if (i_this->m02A2 == 0 && i_this->m2050.ChkAtHit()) {
                i_this->m0308 = 0x28;
                i_this->m217C = nullptr;
                i_this->speedF = -i_this->speedF;
                i_this->m02DC = 9;
                break;
            }
        } else if (!mv_bg_check(i_this)) {
            if (i_this->m02A2 == 0 && i_this->m2050.ChkAtHit()) {
                i_this->m0308 = 0x28;
                i_this->m02DC = 9;
                i_this->speedF = -i_this->speedF;
                break;
            }
            if (i_this->m0308 == 0) {
                i_this->m02DC = 8;
                i_this->m0308 = 50;
                mv_seStart(i_this, JA_SE_LK_ROPE_MAXLENGTH);
                break;
            }
        } else {
            break;
        }
        if ((mv_padTrigX() || mv_padTrigY() || mv_padTrigZ()) && i_this->m02A0 == 0) {
            i_this->m02DC = 8;
            i_this->m0308 = 0x28;
            mv_seStart(i_this, JA_SE_LK_ROPE_MAXLENGTH);
            i_this->m217C = nullptr;
        }
        break;
    }
    case 3: {
        u32 id = i_this->m2180;
        i_this->m02CC = regS(0x80);
        fopAc_ac_c* r4_r27 = mv_searchByID(id);
        bool r26 = false;
        if (r4_r27 != nullptr) {
            gabi::Local<cXyz> tmp;
            gabi::call(0x0201ADE0, &r4_r27->eyePos, tmp.get(), &i_this->current.pos); /* cXyz::operator- */
            gabi::Local<cXyz> sp130;
            f32 x = tmp->x, y = tmp->y, z = tmp->z;
            sp130->x = x;
            sp130->y = y;
            sp130->z = z;
            s16 a = cM_atan2s(x, z);
            cLib_addCalcAngleS2(&i_this->current.angle.y, a, 2, (s16)(regS(0xAB2) + 0x1000));
            f32 zz = sp130->z;
            f32 xx = sp130->x;
            f32 d = std_sqrtf(gabi::fmadds(xx, xx, zz * zz));
            s16 b = cM_atan2s(sp130->y, d);
            cLib_addCalcAngleS2(&i_this->current.angle.x, (s16)-b, 2, (s16)(regS(0xAB2) + 0x1000));
            if (mv_abs(sp130) < regF(0x14) + 50.0f)
                r26 = true;
        }
        i_this->speedF = regF(0xA2C) + 50.0f; /* HD: 50 (GameCube 30) */
        mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        mv_posMove(i_this);
        mv_pl_pos_add(i_this);
        BOOL bg = mv_bg_check(i_this);
        s16 a2 = i_this->m02A2;
        if (bg) {
            r26 = true;
            i_this->m2500 = 0.1f;
        }
        if (a2 == 0 && i_this->m2050.ChkAtHit()) {
            r26 = true;
            i_this->m2500 = 0.1f;
        }
        if (i_this->m0308 == 0) {
            r26 = true;
            i_this->m2500 = 0.02f;
        }
        if (((mv_padTrigX() || mv_padTrigY() || mv_padTrigZ()) && i_this->m02A0 == 0) || r26)
            i_this->m02DC = 5;
        break;
    }
    case 4: {
        fopAc_ac_c* r4_r27 = mv_searchByID(i_this->m2180);
        if (r4_r27 == nullptr) {
            i_this->m02DC = 5;
        } else {
            mDoMtx_YrotS(calc_mtx(), (s16)(i_this->m02D8 << 13));
            gabi::Local<cXyz> sp130;
            sp130->y = 0.0f;
            sp130->z = i_this->m2184;
            sp130->x = 0.0f;
            gabi::Local<cXyz> sp124;
            MtxPosition(sp130, sp124);
            gabi::Local<cXyz> tmp;
            gabi::call(0x0201AD78, &r4_r27->eyePos, tmp.get(), sp124.get()); /* cXyz::operator+ */
            mv_wcopy3(gabi::ea(&i_this->current.pos), gabi::ea(tmp.get()));
            cLib_addCalc0(&i_this->m2184, 1.0f, 5.0f);
            if (i_this->m2184 < 0.1f)
                i_this->mParameters = 0;
            s32 r3 = 0x61 - gabi::ftoi(i_this->m2188 * (regF(0x1C) + 0.060000002f));
            if (r3 < i_this->m02CC || i_this->m2184 < 0.1f)
                i_this->m02CC = r3;
            if (mv_padTrigX() || mv_padTrigY() || mv_padTrigZ()) {
                i_this->m02DC = 5;
                if (r4_r27 != nullptr && fpcM_GetName(r4_r27) == fpcNm_BK_e) {
                    /* bk->dr.mAction = 0; bk->dr.mMode = 0 */
                    gabi::store<s16>(gabi::ea(r4_r27) + 0x4A2, 0);
                    gabi::store<s16>(gabi::ea(r4_r27) + 0x4A0, 0);
                }
            }
        }
        i_this->m02E4 = -5.0f;
        break;
    }
    case 5: {
        /* HD: no m02CC = REG0_S(0); the pull uses cLib_addCalcPos2 */
        if (mv_pull_back(i_this, sp100)) {
            mv_seStartP(player, JA_SE_LK_ROPE_UNWIND);
        } else {
            i_this->mParameters = 0;
            mv_seStart(i_this, JA_SE_LK_ROPE_UNCOIL);
            i_this->m217C = nullptr;
            i_this->m24D8 = 0;
            i_this->m02DC = 0;
        }
        break;
    }
    case 8:
        cLib_addCalc2(&i_this->speedF, -50.0f, 1.0f, regF(0xA58) + 6.0f);
        /* fallthrough */
    case 9: {
        if (mv_pull_back(i_this, sp100)) {
            mv_seStartP(player, JA_SE_LK_ROPE_UNWIND);
        } else {
            i_this->mParameters = 0;
            mv_wcopy3(gabi::ea(&i_this->current.pos), gabi::ea(sp100.get()));
            i_this->m02DC = 0;
            i_this->m029E = regS(0x8C) + 0x14;
        }
        break;
    }
    case 10: {
        i_this->m02CC = 100 - gabi::ftoi(i_this->m2188 * (regF(0xC) + 0.060000002f));
        mv_wcopy3(gabi::ea(&i_this->current.pos), gabi::ea(&i_this->m2504));
        cLib_addCalc2(&i_this->m02E4, -6.25f, 1.0f, 0.375f);
        if (i_this->mParameters == 3) {
            i_this->m02DC = 11;
            if ((fopAcM_GetParam(i_this->m217C) & 0xF0) != 0)
                mv_onEventBit(0x540);
            else
                mv_onEventBit(0x580);
        }
        i_this->m217C->health = 1;
        goto label_1260;
    }
    case 11: {
        mv_wcopy3(gabi::ea(&i_this->current.pos), gabi::ea(&i_this->m2504));
        i_this->m02CC = 100 - gabi::ftoi(i_this->m2188 * (regF(0x10) + 0.060000002f));
        i_this->m217C->health = 3;
    label_1260:
        if (i_this->mParameters == 4) {
            i_this->mParameters = 0;
            mv_seStart(i_this, JA_SE_LK_ROPE_UNCOIL);
            fopAc_ac_c* t = i_this->m217C;
            i_this->m02DC = 0;
            t->health = 0;
            s8 d9 = i_this->m24D9;
            i_this->m217C = nullptr;
            if (d9 != 0) {
                i_this->m24D9 = 6;
                i_this->m029C = 120;
            }
            i_this->m24D8 = 0;
        }
        break;
    }
    default:
        break;
    }

    f32 f27 = 0.0f; /* the m24B4 step (case 2) */
    f32 f26 = 0.0f; /* the sub stick Y (case 5) */
    s8 d9 = i_this->m24D9;
    bool stickBlock = false;
    switch (d9) {
    case -1: {
        if (gabi::load<u16>(gabi::ea(i_this) + 0xF8) != 2) { /* !eventInfo.checkCommandDemoAccrpt() */
            mv_orderPotentialEvent(i_this, 1 /* dEvtFlag_NOPARTNER_e */);
            u32 c = gabi::ea(i_this) + 0xFA; /* eventInfo.onCondition(dEvtCnd_UNK2_e) */
            gabi::store<u16>(c, (u16)(gabi::load<u16>(c) | 2));
            break;
        }
        i_this->m24F4 = regF(0xA54) + 50.0f;
        mv_camStop(camera);
        mv_camSetTrimSize(camera, 2);
        s8 m251C = i_this->m251C;
        i_this->m24D9 = 2;
        f32 b4 = regF(0xA3C) + -400.0f;
        fopAc_ac_c* t = i_this->m217C;
        i_this->m24C8 = 0;
        i_this->m24CA = 0;
        i_this->m24B4 = b4;
        i_this->m24C4 = 0.0f;
        i_this->m24BC = 0;
        s16 ay = t->current.angle.y;
        if (m251C != 0)
            i_this->m2510 = (s16)(ay + -0x4000);
        else
            i_this->m2510 = (s16)(ay + 0x4000);
        if ((fopAcM_GetParam(t) & 0xF0) != 0) {
            i_this->m2514 = -1.0f;
            i_this->m24B8 = regF(0x20) + -90.0f;
            i_this->m2518 = regF(0x24) + 1.0f;
            i_this->m24FC = regF(0x48) + 3.5f;
        } else {
            /* HD: no l_himo2HIO.m06 choice: always (-1, -1) */
            i_this->m24B8 = 0.0f;
            i_this->m2514 = -1.0f;
            i_this->m2518 = -1.0f;
            i_this->m24FC = regF(0xA40) + 3.0f;
        }
        mv_wcopy3(gabi::ea(&i_this->m24E8), gabi::ea(&i_this->m217C->current.pos));
    }
        /* fallthrough */
    case 2: {
        cLib_addCalc0(&i_this->m24B8, 1.0f, regF(0xA38) + 3.0f);
        if ((fopAcM_GetParam(i_this->m217C) & 0xF0) == 0)
            f27 = regF(0xA34) + 10.0f;
        /* HD: step 10 (+ REG + 10 for plain kui) */
        cLib_addCalc2(&i_this->m24B4, -144.0f, 1.0f, f27 + 10.0f);
        if (i_this->m24B4 > -145.0f) {
            s32 bc = i_this->m24BC;
            if (bc < 400) {
                f32 f1 = gabi::fnmsubs(i_this->m217C->scale.y - 1.0f, 17.0f, 20.0f);
                f32 c4 = i_this->m24C4;
                if (f1 < 0.5f)
                    f1 = 0.5f;
                s32 r3 = gabi::ftoi(c4 / f1);
                s32 r5 = r3 + 1;
                i_this->m24C0 = bc;
                i_this->m24BC = i_this->m24BC + r5;
                i_this->m24C4 = c4 + (regF(0xA30) + 2.0f); /* HD: + REG + 2 (GameCube + 1) */
                if (bc == 0) {
                    mv_seStart(i_this, JA_SE_LK_ROPE_COIL_1);
                } else if (bc >= 13) {
                    s32 nbc = i_this->m24BC;
                    if (nbc <= r5 + 13) {
                        mv_seStart(i_this, JA_SE_LK_ROPE_COIL_2);
                    } else if (bc >= 26) {
                        if (nbc <= r5 + 26 || (bc >= 39 && nbc <= r5 + 39))
                            mv_seStart(i_this, JA_SE_LK_ROPE_COIL_3);
                    }
                }
            }
            fopAc_ac_c* t = i_this->m217C;
            u32 prm = fopAcM_GetParam(t);
            f32 f3 = t->scale.y;
            if ((prm & 0x0F) == 3)
                f3 = (f3 - 0.14f) + gabi::load<f32>(HIMO2_REG(0x4A4)); /* REG8_F(7) */
            s16 r4 = (s16)gabi::ftoi(gabi::fnmsubs(2000.0f, f3 - 1.0f, 4000.0f));
            cLib_addCalcAngleS2(&i_this->m24C8, r4, 1, (s16)(r4 / 8));
            if (i_this->m24BC > 49) {
                s16 ca = i_this->m24CA;
                if (ca < 9000) {
                    i_this->m24CA = 9000;
                    if (gabi::ea(i_this) + 0x37C != 0) {
                        u32 p = fopAcM_GetParam(i_this->m217C);
                        mDoAud_seStart((p & 0xF0) != 0 ? JA_SE_LK_ROPE_HOOK_BODY : JA_SE_LK_ROPE_HOOK_WOOD, &i_this->eyePos, 0,
                                       dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
                    }
                } else if (ca == 9000) {
                    i_this->m24CA = 0x238C;
                }
            }
        }
        cLib_addCalc2(&i_this->m24E8.x, i_this->m217C->current.pos.x, 0.3f, 100.0f);
        cLib_addCalc2(&i_this->m24E8.y, i_this->m217C->current.pos.y, 0.3f, 100.0f);
        cLib_addCalc2(&i_this->m24E8.z, i_this->m217C->current.pos.z, 0.3f, 100.0f);
        mv_wcopy3(gabi::ea(&i_this->m24DC), gabi::ea(&i_this->m217C->current.pos));
        mDoMtx_YrotS(calc_mtx(), i_this->m2510);
        gabi::Local<cXyz> sp130;
        f32 z4c = regF(0xA4C) + 120.0f;
        f32 x44 = regF(0xA44) + 10.0f;
        sp130->y = regF(0xA48) + -85.0f;
        sp130->x = x44 * i_this->m2514;
        sp130->z = z4c * i_this->m2518;
        gabi::Local<cXyz> sp124;
        MtxPosition(sp130, sp124);
        f32 fc = i_this->m24FC;
        f32 nx = gabi::fmadds(sp124->x, fc, i_this->m24DC.x);
        f32 ny = gabi::fmadds(sp124->y, fc, i_this->m24DC.y);
        f32 nz = gabi::fmadds(sp124->z, fc, i_this->m24DC.z);
        i_this->m24DC.x = nx;
        i_this->m24DC.y = ny;
        i_this->m24DC.z = nz;
        if ((fopAcM_GetParam(i_this->m217C) & 0xF0) != 0)
            cLib_addCalc2(&i_this->m24FC, regF(0x20) + 1.8f, regF(0x18) + 0.2f, regF(0x1C) + 0.1f);
        else
            cLib_addCalc2(&i_this->m24FC, regF(0xA50) + 1.2f, regF(0xA38) + 0.2f, regF(0xA3C) + 0.2f);
        break;
    }
    case 3: {
        i_this->m217C->health = 2;
        mDoMtx_YrotS(calc_mtx(), i_this->m217C->current.angle.y);
        gabi::Local<cXyz> sp130;
        sp130->x = regF(0x34) + 900.0f;
        sp130->y = (i_this->m217C->home.pos.y + regF(0x38)) + 2000.0f;
        sp130->z = regF(0x3C) + 1000.0f;
        gabi::Local<cXyz> sp124;
        MtxPosition(sp130, sp124);
        cLib_addCalc2(&i_this->m24DC.x, sp124->x, 0.3f, 200.0f);
        cLib_addCalc2(&i_this->m24DC.y, sp124->y, 0.3f, 200.0f);
        cLib_addCalc2(&i_this->m24DC.z, sp124->z, 0.3f, 200.0f);
        cLib_addCalc2(&i_this->m24E8.y, (i_this->m217C->home.pos.y + 2500.0f) + regF(0x40), 0.3f, 100.0f);
        cLib_addCalc2(&i_this->m24F4, regF(0x3C) + 80.0f, 1.0f, 2.0f);
        if (i_this->m029C <= 30) {
            if (i_this->m029C < 30)
                btd_setBlur(0xB4);
            if (i_this->m029C == 30)
                mv_seStartCurrent(himo2_dr, JA_SE_CM_BTD_ROPE_SET);
        }
        if (gabi::load<u8>(dComIfGp_ea() + 0x5134) != 'X') { /* dComIfGp_getStartStageName()[0] */
            if (i_this->m029C > 1)
                break;
            if (!mv_isEventBit(0x420))
                mv_colset(1.0f);
        }
        if (i_this->m029C != 0 || regS(0x90) != 0)
            break;
        btd_setBlur(1);
        i_this->m24D9 = 4;
        if (gabi::load<u8>(dComIfGp_ea() + 0x5134) == 'X' || mv_isEventBit(0x420)) {
            i_this->m02E0 = 0;
            fopAc_ac_c* t = i_this->m217C;
            i_this->m029C = 0;
            t->health = 0;
        } else {
            mv_onEventBit(0x420);
            i_this->m029C = regS(0x84) + 0x3E;
            ykgr_hide();
            fopAc_ac_c* t = i_this->m217C;
            i_this->m02E0 = 0;
            t->health = 0;
        }
        if (himo2_btd != 0)
            btd_onStatus4000();
    }
        /* fallthrough */
    case 4: {
        mv_colset(1.0f);
        if ((u32)(s32)i_this->m029C == (u32)(s32)(s16)(regS(0x84) + 0x39))
            mv_seStart0(JA_SE_CV_DRG_SET_ROPE);
        bool big = false;
        if (i_this->m029C <= 1) {
            mv_colset(0.0f);
            if (i_this->m029C == 0 && regS(0x90) == 0)
                big = true;
        }
        if (big) {
            gabi::store<u8>(himo2_dr + 0x628, 0); /* dr->unk_50C */
            mv_colset(0.0f);
            i_this->m02E0 = 0;
            i_this->m029C = 50;
            i_this->m24D9 = 5;
            i_this->mParameters = 2;
            mv_event_reset();
            i_this->m02A4 = 20;
            u32 p = dComIfGp_ea();
            s16 ang = gabi::call<s16>(0x025D6894, i_this, gabi::load<u32>(p + PLAY_PLAYER)); /* fopAcM_searchPlayerAngleY */
            s16 a = (s16)(ang + 0x8000);
            i_this->m2510 = a;
            i_this->m24F4 = 65.0f;
            i_this->m2512 = a;
            i_this->m24F8 = 65.0f;
            mDoMtx_YrotS(calc_mtx(), a);
            gabi::Local<cXyz> sp130;
            sp130->x = regF(0x24) + 300.0f;
            sp130->y = (player->current.pos.y + 700.0f) + regF(0x28);
            sp130->z = regF(0x2C) + -500.0f;
            MtxPosition(sp130, &i_this->m24DC);
            i_this->m24DC.x = gabi::fmadds(player->current.pos.x, regF(0x44) + 0.55f, i_this->m24DC.x);
            i_this->m24DC.z = gabi::fmadds(player->current.pos.z, regF(0x44) + 0.55f, i_this->m24DC.z);
            i_this->m24E8.x = player->current.pos.x;
            f32 py = player->current.pos.y;
            i_this->m24E8.y = py;
            i_this->m24E8.z = player->current.pos.z;
            i_this->m24E8.y = py - 50.0f;
            ykgr_show();
        } else {
            i_this->m24F4 = regF(0x38) + 50.0f;
            gabi::store<u8>(himo2_dr + 0x628, 1);
            i_this->m24DC.x = regF(0x40) + -5000.0f;
            i_this->m24DC.y = regF(0x44) + 21720.0f;
            i_this->m24DC.z = regF(0x48) + 4000.0f;
            i_this->m24E8.x = regF(0x4C);
            i_this->m24E8.y = regF(0x50) + 21420.0f;
            i_this->m24E8.z = regF(0x54);
        }
        break;
    }
    case 5: {
        if (i_this->m029C == 0) {
            s8 e0 = i_this->m02E0;
            if (e0 == 0) {
                mv_seStart0(JA_SE_CV_DRG_ROCKFALL_1);
                i_this->m029C = 50;
                i_this->m02E0 = (s8)(i_this->m02E0 + 1);
            } else if (e0 == 1) {
                mv_seStart0(JA_SE_CV_DRG_ROCKFALL_1_2);
                i_this->m029C = 50;
                i_this->m02E0 = (s8)(i_this->m02E0 + 1);
            } else {
                if (e0 >= 2) {
                    mv_seStart0(JA_SE_CV_DRG_ROCKFALL_1_3);
                    e0 = 2;
                }
                i_this->m02E0 = (s8)(e0 + 1);
                i_this->m029C = 50;
            }
        }
        f26 = mv_subStickY();
        stickBlock = true;
        break;
    }
    case 6: {
        if (!(gabi::load<u32>(gabi::ea(player) + 0x3C0) & 0x200) && i_this->m029C != 0) { /* !getRopeJumpLand() */
            if (gabi::load<u16>(gabi::ea(i_this) + 0xF8) != 2) {
                mv_orderPotentialEvent(i_this, 2 /* dEvtFlag_STAFF_ALL_e */);
                u32 c = gabi::ea(i_this) + 0xFA;
                gabi::store<u16>(c, (u16)(gabi::load<u16>(c) | 2));
            }
            gabi::store<s16>(himo2_dr + 0x5E6, 0x5A); /* dr->unk_4CA */
            cLib_addCalc2(&i_this->m24F8, regF(0x28) + 20.0f, 1.0f, regF(0x2C) + 2.0f);
            stickBlock = true; /* goto label_1d50 (f26 stays 0) */
            break;
        }
        i_this->m24D9 = 7;
        i_this->m029C = 10;
        break;
    }
    case 7: {
        if (i_this->m029C != 0)
            break;
        if (gabi::load<s16>(himo2_dr + 0x5D6) >= 10) { /* dr->unk_4BA */
            gabi::Local<cXyz> c;
            gabi::Local<cXyz> e;
            c->x = i_this->m24E8.x;
            c->y = i_this->m24E8.y;
            e->z = i_this->m24DC.z;
            e->x = i_this->m24DC.x;
            c->z = i_this->m24E8.z;
            i_this->m24D9 = 0;
            e->y = i_this->m24DC.y;
            mv_camReset(camera, c, e);
            mv_camStart(camera);
            mv_camSetTrimSize(camera, 0);
            mv_event_reset();
            break;
        }
        i_this->m24F4 = regF(0x38) + 50.0f;
        btd_onStatus4000();
        i_this->m24D9 = 8;
        i_this->m24E8.x = 0.0f;
        i_this->m24E8.y = gabi::fmadds(regF(0x1C), 0.1f, 3000.0f);
        i_this->m24E8.z = regF(0x20) * 0.1f;
        mDoMtx_YrotS(calc_mtx(), gabi::load<s16>(himo2_btd + 0x322));
        {
            gabi::Local<cXyz> sp130;
            sp130->x = regF(0x24) * 0.1f;
            sp130->y = gabi::fmadds(regF(0x28), 0.1f, 1000.0f);
            sp130->z = gabi::fmadds(regF(0x2C), 0.1f, 1400.0f);
            MtxPosition(sp130, &i_this->m24DC);
        }
        i_this->m029C = 210;
        i_this->m24F4 = regF(0x3C) + 80.0f;
        {
            gabi::Local<dBgS_LinChk> lin_chk;
            dBgS_LinChk_ct(lin_chk, dBgS_LinChk_vt{0x10010EA4, 0x10010EB4, 0x10010ED4, 0x10010EC4}, false);
            gabi::call(0x024F1AFC, lin_chk.get(), &i_this->m24E8, &i_this->m24DC, i_this); /* dBgS_LinChk::Set */
            if (gabi::call<u32>(0x02008860, dComIfG_Bgsp(), lin_chk.get()) != 0) /* cBgS::LineCross */
                mv_wcopy3(gabi::ea(&i_this->m24DC), gabi::ea(lin_chk.get()) + 0x30); /* GetCross() */
            /* ~dBgS_LinChk */
            u32 b = gabi::ea(lin_chk.get());
            gabi::store<u32>(b + 0x58, 0x10010ED4);
            gabi::store<u32>(b + 0x64, 0x10010E24);
            gabi::store<u32>(b + 0x20, 0x10010E14);
            gabi::call(0x02008B4C, lin_chk.get(), 0);
        }
    }
        /* fallthrough */
    case 8: {
        gabi::store<u8>(himo2_dr + 0x526, 3); /* dr->unk_40A */
        if (i_this->m029C == 200)
            mv_seStart0(JA_SE_CV_DRG_ROCKFALL_2);
        if (i_this->m029C == 170)
            mv_seStartCurrent(himo2_dr, JA_SE_CM_BTD_BEF_ROCK_FALL);
        u32 dr = himo2_dr;
        f32 f1 = i_this->m029C > 150 ? 2000.0f : 100.0f;
        cLib_addCalc2(gabi::at<be<f32>>(dr + 0x530), f1, 0.5f, 100.0f); /* dr->unk_414 */
        if (i_this->m029C > 120)
            break;
        cLib_addCalc2(&i_this->m24E8.y, gabi::fmadds(regF(0x1C), 0.1f, 650.0f), 1.0f, 100.0f);
        cLib_addCalc2(&i_this->m24DC.y, 0.0f, 0.5f, 100.0f);
        if ((u32)(s32)i_this->m029C == (u32)(s32)(s16)(regS(0x88) + 0x6E)) {
            i_this->m2520 = regF(0x34) + 50.0f;
            btd_setBlur(0xB4);
        }
        if (i_this->m029C > 80)
            break;
        cLib_addCalc2(&i_this->m24F4, regF(0x40) + 40.0f, 0.5f, 2.0f);
        if (i_this->m029C == 2)
            btd_setBlur(1);
        if (i_this->m029C != 0)
            break;
        if (gabi::load<u8>(himo2_btd + 0x63D4) >= 3) { /* btd->m6190 */
            i_this->m24D9 = 9;
            i_this->m029C = 220;
            gabi::store<u8>(himo2_dr + 0x526, 0);
        } else {
            i_this->m24D9 = 0;
            f32 ey = player->eyePos.y;
            f32 ex = player->eyePos.x;
            f32 ez = player->eyePos.z;
            gabi::Local<cXyz> c;
            gabi::Local<cXyz> spDC;
            c->x = ex;
            c->y = player->eyePos.y;
            c->z = player->eyePos.z;
            spDC->x = ex * 0.9f;
            spDC->y = ey;
            spDC->z = ez * 0.9f;
            mv_camReset(camera, c, spDC);
            mv_camStart(camera);
            mv_camSetTrimSize(camera, 0);
            btd_offStatus4000();
            mv_event_reset();
            gabi::store<u8>(himo2_dr + 0x526, 0);
        }
        break;
    }
    case 9: {
        cLib_addCalc2(&i_this->m24F4, regF(0x18) + 50.0f, 0.5f, 3.0f);
        f32 by = gabi::load<f32>(himo2_btd + 0x380); /* btd->actor.eyePos.y */
        cLib_addCalc2(&i_this->m24E8.y, gabi::fmadds(regF(0x1C), 0.1f, by) + 200.0f, 0.2f, 1000.0f);
        mDoMtx_YrotS(calc_mtx(), gabi::load<s16>(himo2_btd + 0x322));
        gabi::Local<cXyz> sp130;
        sp130->x = gabi::fmadds(regF(0x24), 0.1f, -300.0f);
        sp130->y = gabi::fmadds(regF(0x28), 0.1f, 100.0f);
        sp130->z = gabi::fmadds(regF(0x2C), 0.1f, 2000.0f);
        gabi::Local<cXyz> sp124;
        MtxPosition(sp130, sp124);
        cLib_addCalc2(&i_this->m24DC.x, sp124->x, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m24DC.y, sp124->y, 0.1f, 50.0f);
        cLib_addCalc2(&i_this->m24DC.z, sp124->z, 0.1f, 50.0f);
        if (i_this->m029C == 99)
            btd_setBlur(0xB4);
        if (i_this->m029C == 40)
            btd_setBlur(1);
        if (i_this->m029C == 0) {
            i_this->m24D9 = 0;
            f32 ex = player->eyePos.x;
            f32 ez = player->eyePos.z;
            f32 ey = player->eyePos.y;
            gabi::Local<cXyz> c;
            gabi::Local<cXyz> spD0;
            c->x = ex;
            c->y = player->eyePos.y;
            c->z = player->eyePos.z;
            spD0->x = ex * 0.9f;
            spD0->y = ey;
            spD0->z = ez * 0.9f;
            mv_camReset(camera, c, spD0);
            mv_camStart(camera);
            mv_camSetTrimSize(camera, 0);
            btd_offStatus4000();
            mv_event_reset();
        }
        break;
    }
    default:
        break;
    }

    if (stickBlock) {
        /* label_1d50 */
        f32 f2 = mv_subStickX();
        s32 add = gabi::ftoi(f2 * (regF(0x20) + 1000.0f));
        i_this->m2512 = (s16)(i_this->m2512 + add);
        cLib_addCalcAngleS2(&i_this->m2510, i_this->m2512, 4, 0x1000);
        if (!(f26 > -0.1f)) {
            cLib_addCalc2(&i_this->m24F8, regF(0x24) + 70.0f, 1.0f, std::fabs(f26) * 3.0f);
        } else if (!(f26 < 0.1f)) {
            cLib_addCalc2(&i_this->m24F8, regF(0x28) + 20.0f, 1.0f, std::fabs(f26) * 3.0f);
        }
        cLib_addCalc2(&i_this->m24F4, i_this->m24F8, 0.1f, 10.0f);
        mDoMtx_YrotS(calc_mtx(), i_this->m2510);
        gabi::Local<cXyz> sp130;
        sp130->x = regF(0x24) + 300.0f;
        sp130->y = (player->current.pos.y + 700.0f) + regF(0x28);
        sp130->z = regF(0x2C) + -500.0f;
        MtxPosition(sp130, &i_this->m24DC);
        f32 nx = gabi::fmadds(player->current.pos.x, regF(0x44) + 0.55f, i_this->m24DC.x);
        f32 oz = i_this->m24DC.z;
        i_this->m24DC.x = nx;
        i_this->m24DC.z = gabi::fmadds(player->current.pos.z, regF(0x44) + 0.55f, oz);
        cLib_addCalc2(&i_this->m24E8.x, player->current.pos.x, 0.3f, 100.0f);
        cLib_addCalc2(&i_this->m24E8.y, (player->current.pos.y - 50.0f) + regF(0x30), 0.3f, 100.0f);
        cLib_addCalc2(&i_this->m24E8.z, player->current.pos.z, 0.3f, 100.0f);
        if (i_this->m02A4 == 0 && gabi::call_ptr<u32>(gabi::load<u32>(player->__vtbl + 0x4C), player) == 0) { /* !checkPlayerFly() */
            mv_camStart(camera);
            i_this->m24D9 = 0;
            i_this->m24D8 = 0;
            i_this->m217C = nullptr;
            i_this->m02DC = 0;
            i_this->mParameters = 0;
            mv_event_reset();
        }
    }

    if (i_this->m24D9 > 0) {
        u32 d8 = i_this->m02D8;
        f32 m2520 = i_this->m2520;
        f32 f27_3 = cM_ssin((s32)(d8 * 0x3300)) * m2520;
        f32 f1 = cM_scos((s32)(d8 * 0x3000)) * m2520;
        f32 c3 = cM_scos((s32)(d8 * 0x1C00));
        gabi::Local<cXyz> spB8;
        gabi::Local<cXyz> spC4;
        f32 dcx = i_this->m24DC.x;
        f32 e8x = i_this->m24E8.x;
        f32 e8y = i_this->m24E8.y;
        f32 e8z = i_this->m24E8.z;
        spB8->y = e8y + f1;
        spB8->z = e8z;
        spB8->x = e8x + f27_3;
        spC4->x = dcx + f27_3;
        spC4->y = i_this->m24DC.y + f1;
        s16 r23 = (s16)gabi::ftoi((c3 * m2520) * 7.5f);
        spC4->z = i_this->m24DC.z;
        mv_camSet(camera, spB8, spC4, r23, i_this->m24F4);
        cLib_addCalc0(&i_this->m2520, 1.0f, regF(0x48) + 2.0f);
        fopAc_ac_c* t = i_this->m217C;
        s16 r23_2 = 0;
        s32 bc = i_this->m24BC;
        if (t != nullptr && (fopAcM_GetParam(t) & 0xF0) != 0)
            r23_2 = -50;
        if (bc > (s16)(r23_2 + 0x82) && regS(0x90) == 0) {
            if ((fopAcM_GetParam(t) & 0xF0) != 0) {
                if (i_this->m24D9 == 2) {
                    if (gabi::load<s16>(himo2_dr + 0x5D6) >= 10) {
                        i_this->m029C = 0;
                        i_this->m24D9 = 4;
                        i_this->m24D8 = 2;
                        if (himo2_btd != 0)
                            btd_onStatus4000();
                    } else {
                        i_this->m24D9 = 3;
                        i_this->m029C = regS(0x84) + 0x41;
                    }
                    i_this->m24D8 = 2;
                }
            } else {
                mv_camStart(camera);
                i_this->m24D9 = 0;
                i_this->mParameters = 2;
                i_this->m24D8 = 2;
                mv_event_reset();
            }
        }
    }

    if (i_this->m02CC < 2)
        i_this->m02CC = 2;
}
VERIFY(0x0216F7FC, new_himo2_move);

/* ---- leftover functions of the translation unit ---- */

/* 021726E0 daHimo2_c::~daHimo2_c (deleting; vtable slot 10010F00): members in reverse order, then fopAc_ac_c */
static void daHimo2_c_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021726E0, void, p, flags);
    if (p != nullptr) {
        u32 t = gabi::ea(p);
        gabi::store<u32>(t + 0x2BC4, 0x10010E84);
        gabi::store<u32>(t + 0x2BB8, 0x10010E94);
        gabi::call(0x024EFD9C, t + 0x2BA4, 0); /* line check */
        gabi::call(0x02018034, t + 0x2B78, 2);
        gabi::call(0x02082DDC, t + 0x2A34, 2);
        gabi::call(0x02082DDC, t + 0x2984, 2);
        gabi::call(0x02082DDC, t + 0x28D4, 2);
        gabi::call(0x02082DDC, t + 0x2824, 2);
        gabi::call(0x02515AE8, t + 0x2550, 2); /* dCcD_Sph */
        gabi::call(0x02515860, t + 0x2514, 2); /* dCcD_Stts */
        gabi::call(0x025EB8B8, t + 0x238C, 2); /* mDoExt_3DlineMat1_c */
        gabi::call(0x025EB8B8, t + 0x2200, 2); /* mDoExt_3DlineMat1_c */
        gabi::call(0x025EB8B8, t + 0x204C, 2); /* mDoExt_3DlineMat1_c */
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x021726E0, daHimo2_c_dt);

/* 021727D0 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10010E00, after the destructor 0216F7E8 */
static void himo2_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x021727D0, void, p);
}
VERIFY(0x021727D0, himo2_SafeString_assureTermination);
