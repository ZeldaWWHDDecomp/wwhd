/**
 * d_a_gnd_move.cpp (WWHD)
 * Boss - Ganondorf: gnd_move (02159FC8; the function map calls it attack0), the action dispatch
 * by m3EA with move0, attack0, attack1, attack2 and attackPZ inlined, followed by the shot (light
 * arrow) check and damage_check.
 *
 * The GameCube source of this unit is all "Nonmatching"
 * stubs: written from the WWHD code (cking.rpx) and verified against it.
 */
#include "d/actor/d_a_gnd.h"

#define ZELDA_PTR 0x10464008u
#define HIO_F(off) gabi::load<f32>(L_HIO + (off))
#define HIO_S(off) gabi::load<s16>(L_HIO + (off))

static inline bool mv_isStop(mDoExt_McaMorf* morf) { return (morf->mFrameCtrl.mState & 1) || morf->mFrameCtrl.mRate == 0.0f; }
static inline void mv_mons_se(gnd_class* a, u32 id) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, 0, reverb);
    }
}
static inline void mv_mons_se_eye(gnd_class* a, u32 id) {
    if (gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, 0, reverb);
    }
}
static inline void mv_se(fopAc_ac_c* a, u32 id) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        mDoAud_seStart(id, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
    }
}
static inline void mv_se_eye(fopAc_ac_c* a, u32 id) {
    if (gabi::ea(&a->eyePos) != 0) {
        mDoAud_seStart(id, &a->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
    }
}
static inline void mv_setTarget(gnd_class* i_this, fopAc_ac_c* pl) { GXYZ(0x3F0)->copy(pl->current.pos); }
static inline fopAc_ac_c* mv_zelda() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(ZELDA_PTR)); }

/* move0's common end */
static inline void mv_moveEnd(gnd_class* i_this) {
    pos_move(i_this, 0);
    GF(f32, 0x40C) = 2000.0f;
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 2, 0x1000);
}

/* move0 (m3EA 0) */
static inline void mv_move0(gnd_class* i_this) {
    fopAc_ac_c* pl = dComIfGp_getPlayer(0);
    f32 dist = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
    s32 frame = gabi::ftoi(i_this->mpMorf->mFrameCtrl.mFrame);
    switch ((u32)(s32)GF(s16, 0x3EC)) {
    case 0:
        anm_init(i_this, 0x59, 10.0f, 2, 1.0f, 0x21);
        GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        break;
    case 1:
        GF(f32, 0x414) = HIO_F(0x10);
        if (dist < HIO_F(0x14)) {
            anm_init(i_this, 0x58, 10.0f, 2, 1.0f, -1);
            GF(s16, 0x3EC) = 2;
        } else if (dist > HIO_F(0x18)) {
            anm_init(i_this, 0x44, 10.0f, 0, 1.0f, 0x17);
            i_this->speedF = 0.0f;
            GF(f32, 0x414) = 0.0f;
            GF(s16, 0x3EC) = 3;
        }
        mv_setTarget(i_this, pl);
        break;
    case 2: {
        GF(f32, 0x414) = 0.0f;
        s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        if (player_view_check(i_this, ang)) {
            s16 next;
            if (dist > HIO_F(0x38) && GF(s16, 0x420) == 0) {
                next = 0;
            } else if (dist < HIO_F(0x28)) {
                next = 0x14;
            } else if (GF(s16, 0x420) != 0) {
                next = -1;
            } else if (cM_rndF(100.0f) < HIO_F(0x44)) {
                GF(s16, 0x3EA) = 2;
                next = (cM_rndF(1.0f) < 0.5f) ? 0 : 1;
            } else {
                s8 g = GF(s8, 0x60C);
                GF(s16, 0x3EA) = 1;
                if (g == 0) {
                    next = 0;
                } else {
                    u8 h = gabi::load<u8>(L_HIO + 0xD);
                    if (h == 1) {
                        next = 0;
                    } else if (g == 1 || h == 2) {
                        next = 5;
                    } else {
                        next = (cM_rndF(1.0f) < 0.5f) ? 0 : 5;
                    }
                }
            }
            if (next >= 0) GF(s16, 0x3EC) = next;
        }
        mv_setTarget(i_this, pl);
        break;
    }
    case 3:
        i_this->gravity = HIO_F(0x24);
        if (frame == 0xC) {
            i_this->speed.y = HIO_F(0x20);
            f32 s = HIO_F(0x1C);
            GF(f32, 0x414) = s;
            i_this->speedF = s;
            splash_set(i_this);
        }
        if (mv_isStop(i_this->mpMorf)) {
            anm_init(i_this, 0x43, 5.0f, 2, 1.0f, -1);
            GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        }
        break;
    case 4: {
        mv_se(i_this, 0x5158);
        f32 sy = i_this->speed.y;
        i_this->gravity = HIO_F(0x24);
        if (sy < 0.0f) { /* bge: taken on NaN */
            anm_init(i_this, 0x41, 5.0f, 0, 1.0f, -1);
            GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        }
        break;
    }
    case 5: {
        mv_se(i_this, 0x5158);
        f32 g = HIO_F(0x24);
        GF(f32, 0x40C) = 0.0f;
        i_this->gravity = g;
        if (checkGround(i_this, 0.0f)) {
            anm_init(i_this, 0x42, 2.0f, 0, 1.0f, -1);
            GF(s16, 0x3EC) = 6;
            splash_set(i_this);
        }
        break;
    }
    case 6: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        GF(f32, 0x414) = 0.0f;
        GF(f32, 0x40C) = 0.0f;
        if (mv_isStop(morf)) {
            GF(s16, 0x3EC) = 2;
            anm_init(i_this, 0x58, 10.0f, 2, 1.0f, -1);
        }
        break;
    }
    case 0x14:
        anm_init(i_this, 0x4A, 5.0f, 0, 1.0f, 0x1B);
        GF(s16, 0x3EC) = 0x15;
        break;
    case 0x15: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        i_this->speedF = 0.0f;
        GF(f32, 0x414) = 0.0f;
        if (mv_isStop(morf)) {
            i_this->speed.y = HIO_F(0x30);
            f32 s = HIO_F(0x2C);
            i_this->speedF = s;
            GF(f32, 0x414) = s;
            anm_init(i_this, 0x4B, 5.0f, 0, 1.0f, -1);
            GF(s16, 0x3EC) = 0x16;
        }
        mv_setTarget(i_this, pl);
        break;
    }
    case 0x16:
        i_this->gravity = HIO_F(0x34);
        if (checkGround(i_this, 0.0f)) {
            anm_init(i_this, 0x4C, 5.0f, 0, 1.0f, 0x1C);
            GF(s16, 0x3EC) = 0x17;
        }
        mv_setTarget(i_this, pl);
        break;
    case 0x17: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        GF(f32, 0x414) = 0.0f;
        if (mv_isStop(morf)) {
            GF(s16, 0x3EC) = 2;
            wait_set(i_this);
            GF(s16, 0x420) = 0x14;
        }
        break;
    }
    }
    mv_moveEnd(i_this);
}

/* attack0 (m3EA 1): the sword combos. Returns the guard flag (r30). */
static inline void mv_attack0(gnd_class* i_this) {
    fopAc_ac_c* pl = dComIfGp_getPlayer(0);
    f32 dist = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
    mDoExt_McaMorf* morf0 = i_this->mpMorf;
    bool follow = false, guard = false, endAtk = false, home = false;
    int turn = 0;
    s16 mode = GF(s16, 0x3EC);
    s32 frame = gabi::ftoi(morf0->mFrameCtrl.mFrame);
    switch ((u32)(s32)mode) {
    case 0:
        anm_init(i_this, 0x24, 3.0f, 0, 1.0f, 8);
        mv_mons_se(i_this, 0x4947);
        GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        guard = true;
        break;
    case 1: {
        follow = true;
        guard = true;
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if (frame >= 0x17) {
            GF(f32, 0x414) = HIO_F(0x48);
        } else {
            GF(f32, 0x414) = HIO_F(0x50);
            if ((u32)(frame - 0xD) < 4) GF(u8, 0x1718) = 1;
        }
        if (mv_isStop(morf)) {
            anm_init(i_this, 0x25, 1.0f, 0, 1.0f, 9);
            mv_mons_se(i_this, 0x4948);
            GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        }
        break;
    }
    case 2: {
        follow = true;
        guard = true;
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if (frame >= 0xC) {
            GF(f32, 0x414) = HIO_F(0x48);
        } else {
            GF(f32, 0x414) = HIO_F(0x4C);
            if ((u32)(frame - 5) < 6) GF(u8, 0x1718) = 1;
        }
        if (mv_isStop(morf)) {
            if (GF(s8, 0x608) != 0) goto counter;
            anm_init(i_this, 0x26, 2.0f, 0, 1.0f, 0xA);
            mv_mons_se(i_this, 0x4949);
            GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        }
        if (frame > HIO_S(0x66)) GF(u8, 0x172F) = 1;
        break;
    }
    case 3:
        guard = true;
        if (frame >= 0x19) {
            GF(f32, 0x414) = HIO_F(0x48);
        } else {
            GF(f32, 0x414) = HIO_F(0x4C);
            if ((u32)(frame - 6) < 0x11) {
                GF(u8, 0x1718) = 1;
                GF(u8, 0x1719) = 1;
            }
        }
        if (frame >= HIO_S(0x68) && frame <= HIO_S(0x6A)) GF(u8, 0x172F) = 1;
        if (mv_isStop(i_this->mpMorf)) {
            GF(s16, 0x3EA) = 0;
            GF(s16, 0x3EC) = 0;
        }
        break;
    case 5:
        anm_init(i_this, 0x27, 3.0f, 0, 1.0f, 0xB);
        mv_mons_se(i_this, 0x4947);
        GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        guard = true;
        break;
    case 6: {
        guard = true;
        if ((u32)(frame - 9) < 0xB) {
            GF(f32, 0x414) = HIO_F(0x50);
        } else {
            GF(f32, 0x414) = HIO_F(0x48);
        }
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if ((u32)(frame - 0xD) < 5) GF(u8, 0x1719) = 1;
        follow = true;
        if (mv_isStop(morf)) {
            anm_init(i_this, 0x28, 2.0f, 0, 1.0f, 0xC);
            mv_mons_se(i_this, 0x4949);
            GF(s16, 0x3EC) = 7;
        }
        break;
    }
    case 7: {
        follow = true;
        guard = true;
        GF(f32, 0x414) = HIO_F(0x4C);
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if ((u32)(frame - 8) < 0x10) {
            GF(u8, 0x1718) = 1;
            GF(u8, 0x1719) = 1;
        }
        if (!mv_isStop(morf)) break;
        if (GF(s8, 0x608) != 0) goto counter;
        anm_init(i_this, 0x2B, 5.0f, 0, 1.0f, 0xF);
        GF(s16, 0x3EC) = 8;
        break;
    }
    counter:
        guard = true;
        GF(s8, 0x608) = 0;
        anm_init(i_this, 0x4D, 3.0f, 0, 1.0f, 0x1D);
        GF(s16, 0x3EC) = 0x15;
        break;
    case 8: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        GF(f32, 0x414) = 0.0f;
        guard = true;
        if (!mv_isStop(morf)) break;
        anm_init(i_this, 0x29, 2.0f, 0, 1.0f, 0xD);
        mv_mons_se(i_this, 0x4948);
        GF(s16, 0x3EC) = 9;
        i_this->speed.y = HIO_F(0x58);
        GF(f32, 0x414) = gabi::fmuls_ppc(dist, REG0_F(9) + 0.015f);
        splash_set(i_this);
        break;
    }
    case 9: {
        guard = true;
        f32 sy = i_this->speed.y;
        if (sy > 0.0f) { /* ble: taken on NaN */
            f32 sx = i_this->speed.x;
            f32 sz = i_this->speed.z;
            GF(f32, 0x3FC) = sx;
            GF(f32, 0x400) = sy;
            GF(f32, 0x404) = sz;
        }
        home = true;
        turn = 1;
        if (frame > HIO_S(0x6C)) GF(u8, 0x172F) = 2;
        f32 g = HIO_F(0x5C);
        GF(f32, 0x40C) = 0.0f;
        i_this->gravity = g;
        if (!checkGround(i_this, 0.0f)) break;
        anm_init(i_this, 0x2A, 2.0f, 0, 1.0f, 0xE);
        GF(s16, 0x3EC) = 0xA;
        splash_set(i_this);
        break;
    }
    case 0xA: {
        guard = true;
        s8 g = GF(s8, 0x60C);
        s16 a = i_this->shape_angle.y;
        if (g >= 2) GF(u8, 0x60B) = 2;
        i_this->current.angle.y = a;
        if (frame >= HIO_S(0x6E) && frame <= HIO_S(0x70)) GF(u8, 0x172F) = 2;
        GF(f32, 0x414) = 0.0f;
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if ((u32)(frame - 3) < 3) {
            GF(u8, 0x1718) = 1;
            GF(u8, 0x1719) = 1;
            GF(f32, 0x1728) = 60.0f;
            GF(f32, 0x1724) = 60.0f;
        }
        if (mv_isStop(morf)) {
            GF(s16, 0x3EC) = 0xB;
            GF(s16, 0x41E) = HIO_S(0x72);
        }
        break;
    }
    case 0xB: {
        s8 g = GF(s8, 0x60C);
        s16 a = i_this->shape_angle.y;
        if (g >= 2) GF(u8, 0x60B) = 2;
        s16 t = GF(s16, 0x41E);
        i_this->current.angle.y = a;
        if (t == 0) endAtk = true;
        break;
    }
    case 0xF: {
        s16 t = GF(s16, 0x41E);
        GF(f32, 0x40C) = 8000.0f;
        GF(f32, 0x414) = 0.0f;
        follow = true;
        if (t != 0) break;
        anm_init(i_this, 0x48, 5.0f, 0, 1.0f, 0x1A);
        GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        break;
    }
    case 0x10: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        GF(f32, 0x40C) = 8000.0f;
        follow = true;
        if (!mv_isStop(morf)) break;
        anm_init(i_this, 0x47, 2.0f, 0, 1.0f, -1);
        mv_mons_se(i_this, 0x494A);
        GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        i_this->speed.y = REG0_F(9) + 30.0f;
        GF(f32, 0x414) = gabi::fmuls_ppc(dist, REG0_F(10) + 0.025f);
        splash_set(i_this);
        break;
    }
    case 0x11: {
        f32 g = REG0_F(11) + -2.0f;
        f32 sy = i_this->speed.y;
        GF(f32, 0x40C) = 5000.0f;
        i_this->gravity = g;
        if (sy > 0.0f) break;
        anm_init(i_this, 0x45, 2.0f, 0, 1.0f, 0x18);
        mv_mons_se_eye(i_this, 0x494B);
        s16 m = GF(s16, 0x3EC);
        u32 sx = gabi::load<u32>(gabi::ea(i_this) + 0x33C);
        u32 syw = gabi::load<u32>(gabi::ea(i_this) + 0x340);
        GF(u32, 0x3FC) = sx;
        u32 sz = gabi::load<u32>(gabi::ea(i_this) + 0x344);
        GF(u32, 0x400) = syw;
        GF(s16, 0x3EC) = m + 1;
        GF(u32, 0x404) = sz;
        {
            u32 lp = gabi::load<u32>(dComIfGp_ea() + 0x5B34);
            u32 f = gabi::load<u32>(lp + 0x6A70);
            gabi::store<s16>(lp + 0x3B0, 0);
            gabi::store<u32>(lp + 0x6A70, f & ~8u);
        }
        break;
    }
    case 0x12:
        GF(u8, 0x1718) = 4;
        home = true;
        GF(u8, 0x1719) = 4;
        if (!checkGround(i_this, 0.0f)) break;
        anm_init(i_this, 0x46, 2.0f, 0, 1.0f, 0x19);
        {
            s16 m = GF(s16, 0x3EC);
            i_this->speedF = 0.0f;
            GF(f32, 0x414) = 0.0f;
            GF(s16, 0x3EC) = m + 1;
        }
        splash_set(i_this);
        break;
    case 0x13:
        if (frame <= 4) {
            GF(u8, 0x1718) = 4;
            GF(u8, 0x1719) = 4;
        }
        if (mv_isStop(morf0)) endAtk = true;
        break;
    case 0x14:
        guard = true;
        anm_init(i_this, 0x4D, 3.0f, 0, 1.0f, 0x1D);
        GF(s16, 0x3EC) = 0x15;
        break;
    case 0x15: {
        GF(f32, 0x40C) = 5000.0f;
        mDoExt_McaMorf* morf = i_this->mpMorf;
        GF(f32, 0x414) = HIO_F(0x60);
        guard = true;
        if (!mv_isStop(morf)) break;
        anm_init(i_this, 0x4E, 2.0f, 0, 1.0f, -1);
        mv_mons_se(i_this, 0x494A);
        GF(s16, 0x3EC) = 0x16;
        break;
    }
    case 0x16: {
        guard = true;
        if ((u32)frame < 5) {
            GF(u8, 0x1719) = 3;
            GF(f32, 0x1728) = REG0_F(12) + 170.0f;
        }
        if (frame == 0x14 && GF(s8, 0x608) == 0) {
            GF(s8, 0x1731) = (s8)(REG0_S(4) + 0xD);
        }
        if (!mv_isStop(i_this->mpMorf)) break;
        if (GF(s8, 0x608) != 0) {
            s8 g = GF(s8, 0x60C);
            GF(s8, 0x608) = 0;
            if (g == 0) {
                GF(f32, 0x414) = 0.0f;
                wait_set(i_this);
                GF(s16, 0x3EC) = 0x17;
                GF(s16, 0x41E) = 0x96;
            } else {
                GF(s16, 0x3EC) = 0xF;
                GF(s16, 0x41E) = REG0_S(7);
            }
        } else {
            GF(s16, 0x3EC) = 0x1A;
            wait_set(i_this);
            GF(s16, 0x41E) = REG0_S(5) + 0x2D;
        }
        break;
    }
    case 0x17:
        if (GF(s16, 0x41E) != 0) break;
        anm_init(i_this, 0x50, 3.0f, 0, 1.0f, -1);
        GF(s16, 0x3EC) = 0x18;
        break;
    case 0x18: {
        if (!mv_isStop(morf0)) break;
        s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        if (player_view_check(i_this, ang)) {
            endAtk = true;
            break;
        }
        anm_init(i_this, 0x3F, 3.0f, 0, 1.0f, -1);
        mv_mons_se(i_this, 0x4954);
        GF(s16, 0x3EC) = 0x19;
        GF(s16, 0x41E) = 0x14;
        break;
    }
    case 0x19:
        turn = 1;
        if (GF(s16, 0x41E) == 0) endAtk = true;
        break;
    case 0x1A: {
        s16 t = GF(s16, 0x41E);
        guard = true;
        GF(f32, 0x414) = 0.0f;
        if (t == 0) endAtk = true;
        break;
    }
    }
    i_this->speedF = GF(f32, 0x414);
    if (follow) {
        mv_setTarget(i_this, pl);
        pos_move(i_this, 0);
    } else {
        pos_move(i_this, 1);
    }
    GF(f32, 0x40C) = 2000.0f;
    if (turn == 0) {
        cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 4, 0x2000);
    } else {
        s16 ang = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
        cLib_addCalcAngleS2(&i_this->shape_angle.y, ang, 4, (s16)(turn << 12));
    }
    if (home) {
        cLib_addCalc2(&i_this->current.pos.x, pl->current.pos.x, 0.8f, fabsf(GF(f32, 0x3FC)));
        cLib_addCalc2(&i_this->current.pos.z, pl->current.pos.z, 0.8f, fabsf(GF(f32, 0x404)));
    }
    if (endAtk) {
        s16 a = i_this->shape_angle.y;
        GF(s16, 0x3EA) = 0;
        GF(s16, 0x3EC) = 0;
        i_this->current.angle.y = a;
        GF(s16, 0x420) = HIO_S(0x64);
    }
    if (gabi::load<u8>(L_HIO + 0x40) == 0 && GF(s8, 0x60C) < 2 && guard) {
        GF(u8, 0x172E) = 0;
        GF(u8, 0x60B) = 5;
    }
}

/* attack1 (m3EA 2) */
static inline void mv_attack1(gnd_class* i_this) {
    u32 play = dComIfGp_ea();
    mDoExt_McaMorf* morf = i_this->mpMorf;
    f32 reg = REG0_F(13);
    s32 frame = gabi::ftoi(morf->mFrameCtrl.mFrame);
    fopAc_ac_c* pl = gabi::at<fopAc_ac_c>(gabi::load<u32>(play + 0x5B2C));
    bool done = false;
    f32 r = reg + 110.0f;
    s16 mode = GF(s16, 0x3EC);
    GF(f32, 0x1728) = r;
    GF(f32, 0x1724) = r;
    switch ((u32)(s32)mode) {
    case 0:
        anm_init(i_this, 0x53, 5.0f, 0, 1.0f, 0x1F);
        mv_mons_se(i_this, 0x494A);
        GF(s16, 0x3EC) = 5;
        break;
    case 1:
        anm_init(i_this, 0x54, 5.0f, 0, 1.0f, 0x20);
        mv_mons_se(i_this, 0x494C);
        GF(s16, 0x3EC) = 6;
        break;
    case 5:
    case 6:
        if (mode == 5 ? (u32)(frame - 0xF) < 4 : (u32)(frame - 0xC) < 0x17) {
            mDoExt_McaMorf* m = i_this->mpMorf;
            GF(u8, mode == 5 ? 0x1718 : 0x1719) = 1;
            done = mv_isStop(m);
        } else {
            mv_setTarget(i_this, pl);
            done = mv_isStop(i_this->mpMorf);
        }
        break;
    }
    GF(f32, 0x414) = HIO_F(0x10);
    pos_move(i_this, 0);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 2, 0x1000);
    if (done) {
        GF(s16, 0x3EA) = 0;
        GF(s16, 0x3EC) = 0;
        s16 a = i_this->shape_angle.y;
        GF(s16, 0x420) = 0;
        i_this->current.angle.y = a;
    }
}

/* attack2 (m3EA 3) */
static inline void mv_attack2(gnd_class* i_this) {
    dComIfGp_get();
    mDoExt_McaMorf* morf = i_this->mpMorf;
    f32 reg = REG0_F(13);
    s16 mode = GF(s16, 0x3EC);
    s32 frame = gabi::ftoi(morf->mFrameCtrl.mFrame);
    GF(u8, 0x172E) = 0;
    f32 r = reg + 110.0f;
    GF(f32, 0x1728) = r;
    GF(f32, 0x1724) = r;
    switch ((u32)(s32)mode) {
    case 0:
        anm_init(i_this, 0x36, 2.0f, 0, 1.0f, 0x13);
        mv_mons_se(i_this, 0x494A);
        GF(s16, 0x3EC) = 2;
        break;
    case 1:
        anm_init(i_this, 0x37, 2.0f, 0, 1.0f, 0x14);
        mv_mons_se(i_this, 0x4947);
        GF(s16, 0x3EC) = 3;
        break;
    case 2:
    case 3:
        if (mode == 2) {
            if (frame >= 4) GF(u8, 0x1718) = 1;
        } else {
            if (frame >= 6) GF(u8, 0x1719) = 1;
        }
        if (mv_isStop(i_this->mpMorf)) {
            GF(s16, 0x3EA) = 0;
            GF(s16, 0x3EC) = 0;
            s16 a = i_this->shape_angle.y;
            GF(s16, 0x420) = 0;
            i_this->current.angle.y = a;
        }
        break;
    }
}

/* attackPZ's common end: follow a point beside Zelda */
static inline void mv_pzEnd(gnd_class* i_this, bool done) {
    i_this->speedF = GF(f32, 0x414);
    mDoMtx_YrotS(calc_mtx(), GF(s16, 0x436));
    gabi::Local<cXyz> l20;
    gabi::Local<cXyz> l4c;
    gabi::Local<cXyz> l64;
    l20->y = 0.0f;
    l20->x = REG0_F(16) + -20.0f;
    l20->z = REG0_F(17) + 100.0f;
    MtxPosition(l20, l4c);
    cXyz_pl(&mv_zelda()->current.pos, l64, l4c);
    GXYZ(0x3F0)->copy(*l64);
    pos_move(i_this, 0);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 4, 0x2000);
    if (done) {
        cLib_addCalc2(&i_this->current.pos.x, GF(f32, 0x3F0), 0.2f, fabsf(GF(f32, 0x3FC)));
        cLib_addCalc2(&i_this->current.pos.z, GF(f32, 0x3F8), 0.2f, fabsf(GF(f32, 0x404)));
    }
}

/* attackPZ (m3EA 4): the fight beside Zelda */
static inline void mv_attackPZ(gnd_class* i_this) {
    fopAc_ac_c* zelda0 = mv_zelda();
    gabi::Local<cXyz> d;
    cXyz_mi(&zelda0->current.pos, d, &i_this->current.pos);
    f32 dist = std_sqrtf(PSVECSquareMag(d));
    fopAc_ac_c* z = mv_zelda();
    f32 dx = gabi::fsubs_ppc(i_this->current.pos.x, z->current.pos.x);
    f32 dz = gabi::fsubs_ppc(i_this->current.pos.z, z->current.pos.z);
    s16 ang = cM_atan2s(dx, dz);
    mDoExt_McaMorf* morf0 = i_this->mpMorf;
    GF(u8, 0x172E) = 0;
    GF(s16, 0x428) = 5;
    s16 mode = GF(s16, 0x3EC);
    s32 frame = gabi::ftoi(morf0->mFrameCtrl.mFrame);
    bool done = false;
    switch ((u32)(s32)mode) {
    case 0:
        if (dist > REG_F(6, 1) + 1000.0f) {
            mDoMtx_YrotS(calc_mtx(), ang);
            gabi::Local<cXyz> l20;
            gabi::Local<cXyz> l4c;
            gabi::Local<cXyz> l7c;
            l20->x = 0.0f;
            l20->y = 0.0f;
            l20->z = -(REG0_F(1) + 1000.0f);
            MtxPosition(l20, l4c);
            cXyz_pl(&i_this->current.pos, l7c, l4c);
            mv_zelda()->current.pos.copy(*l7c);
        }
        mv_zelda()->shape_angle.y = ang;
        mv_zelda()->current.angle.y = ang;
        GF(s16, 0x436) = ang;
        GF(s16, 0x3EC) = 1;
        GF(s16, 0x41E) = 0x14;
        GF(s16, 0x18BE) = 1;
        gabi::store<u8>(gabi::ea(zelda0) + 0x857, 1);
        i_this->speedF = 0.0f;
        GF(f32, 0x414) = 0.0f;
        gabi::call(0x025E1950, 1); /* mDoAud_changeBgmStatus */
        /* fall through */
    case 1: {
        s16 t = GF(s16, 0x41E);
        GF(f32, 0x40C) = 0.0f;
        if (t != 0) break;
        anm_init(i_this, 0x4F, 10.0f, 0, 1.0f, -1);
        mv_mons_se_eye(i_this, 0x4954);
        J3DModel* model = i_this->mpMorf->getModel();
        void* res = dComIfG_getObjectRes(STR(0x1000FFE4) /* "Gnd" */, 0x62, GND_SAFESTRING_VTBL);
        gnd_btkAnm_init(i_this->mpBtk, J3DModel_getModelData(model), res, 1, 0, 1.0f, 0, -1, true, 0);
        GF(s16, 0x3EC) = 2;
        break;
    }
    case 2:
        GF(f32, 0x40C) = 0.0f;
        break;
    case 3:
        GF(s16, 0x3EC) = 5;
        /* fall through */
    case 5:
        anm_init(i_this, 0x44, 5.0f, 0, 1.0f, 0x17);
        GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        break;
    case 6:
        GF(f32, 0x40C) = 8000.0f;
        i_this->gravity = REG0_F(17) + -0.5f;
        if (frame == 0xC) {
            i_this->speed.y = REG0_F(19) + 10.0f;
            GF(f32, 0x414) = gabi::fmuls_ppc(dist, REG0_F(18) + 0.018f);
            splash_set(i_this);
        }
        if (!mv_isStop(i_this->mpMorf)) break;
        anm_init(i_this, 0x43, 2.0f, 2, 1.0f, -1);
        GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        break;
    case 7: {
        mv_se_eye(i_this, 0x5158);
        f32 g = REG0_F(17) + -0.5f;
        f32 sy = i_this->speed.y;
        i_this->gravity = g;
        if (sy < 0.0f) { /* bge: taken on NaN */
            anm_init(i_this, 0x41, 5.0f, 0, 1.0f, -1);
            GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        }
        break;
    }
    case 8: {
        mv_se_eye(i_this, 0x5158);
        f32 g = REG0_F(17) + -0.5f;
        GF(f32, 0x40C) = 0.0f;
        i_this->gravity = g;
        if (!checkGround(i_this, 0.0f)) break;
        anm_init(i_this, 0x42, 2.0f, 0, 1.0f, -1);
        u32 sx = gabi::load<u32>(gabi::ea(i_this) + 0x33C);
        u32 sy = gabi::load<u32>(gabi::ea(i_this) + 0x340);
        GF(u32, 0x3FC) = sx;
        s16 m = GF(s16, 0x3EC);
        GF(u32, 0x400) = sy;
        u32 sz = gabi::load<u32>(gabi::ea(i_this) + 0x344);
        GF(s16, 0x3EC) = m + 1;
        GF(u32, 0x404) = sz;
        splash_set(i_this);
        break;
    }
    case 9: {
        mDoExt_McaMorf* morf = i_this->mpMorf;
        GF(f32, 0x414) = 0.0f;
        GF(f32, 0x40C) = 0.0f;
        done = true;
        if (!mv_isStop(morf)) break;
        GF(s16, 0x3EC) = GF(s16, 0x3EC) + 1;
        anm_init(i_this, 0x2C, 3.0f, 0, 1.0f, 0x10);
        break;
    }
    case 0xA: {
        done = true;
        if (!mv_isStop(morf0)) break;
        GF(s16, 0x3EC) = mode + 1;
        anm_init(i_this, 0x2D, 2.0f, 0, 1.0f, 0x11);
        mv_mons_se_eye(i_this, 0x4955);
        JPABaseEmitter* emitter = dComIfGp_particle_set(0x837E, &i_this->current.pos);
        GF(u32, 0x174C) = gabi::ea(emitter);
        if (emitter == nullptr) break;
        u32 e = gabi::ea(emitter);
        u8 r = GF(u8, 0x4CD);
        u8 b = GF(u8, 0x4D1);
        gabi::store<u8>(e + 0x244, r);
        u8 g = GF(u8, 0x4CF);
        gabi::store<u8>(e + 0x245, g);
        gabi::store<u8>(e + 0x246, b);
        break;
    }
    case 0xB: {
        if ((u32)frame == (u32)(REG0_S(5) + 2)) {
            GF(s16, 0x18BC) = 0x96;
            gabi::store<u32>(gabi::ea(zelda0) + 0xADC, 1);
            mv_se(mv_zelda(), 0x2828);
            gabi::Local<cXyz> p;
            u32 vib = gabi::ea(dComIfGp_getVibration());
            s16 reg = REG0_S(2);
            p->x = 0.0f;
            p->y = 1.0f;
            p->z = 0.0f;
            gabi::call<BOOL>(0x025CB374, vib, reg + 4, -0x21, p.get());
        }
        u32 e;
        {
            s16 reg6 = REG0_S(6);
            e = GF(u32, 0x174C);
            if ((u32)frame == (u32)(reg6 + 2)) {
                GF(s16, 0x18BE) = 4;
                GF(s16, 0x18C0) = 0;
            }
        }
        if (e == 0) break;
        mDoExt_McaMorf* morf = i_this->mpMorf;
        if (mv_isStop(morf)) {
            gnd_becomeInvalidEmitter(e);
            GF(u32, 0x174C) = 0;
        } else {
            gabi::call(0x028249B0, gnd_getAnmMtx(morf->getModel(), 0x23), e + 0x1F0, e + 0x22C);
        }
        break;
    }
    }
    mv_pzEnd(i_this, done);
}

/* 02159FC8 */
void gnd_move(gnd_class* i_this) {
    WWHD_FUNC(0x02159FC8, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    switch ((u32)(s32)GF(s16, 0x3EA)) {
    case 0:
        mv_move0(i_this);
        break;
    case 1:
        mv_attack0(i_this);
        break;
    case 2:
        mv_attack1(i_this);
        break;
    case 3:
        mv_attack2(i_this);
        break;
    case 4:
        mv_attackPZ(i_this);
        break;
    case 5:
        attack_last(i_this);
        break;
    case 10:
        defence0(i_this);
        break;
    case 11:
        damage(i_this);
        break;
    case 20:
        demowait(i_this);
        break;
    case 21:
        yawait(i_this);
        break;
    case 30:
        finish(i_this);
        return;
    }
    if (GF(s8, 0x60C) >= 2 && GF(s16, 0x3EA) != 0xB) GF(s16, 0x42A) = 3;
    {
        u8 t = gabi::load<u8>(gabi::ea(player) + 0x3AC);
        if ((t == 5 || t == 0xF) && GF(s8, 0x60C) < 2) {
            GF(u8, 0x60B) = 0;
            GF(u8, 0x172E) = 0;
        }
    }
    if (GF(s8, 0x172E) != 0) {
        /* the light arrows */
        bool hit = false;
        bool check = true;
        fopAc_ac_c* shot = (fopAc_ac_c*)fpcM_Search(0x02154E4C /* shot_s_sub */, i_this);
        if (shot != nullptr) {
            gabi::Local<cXyz> d;
            gabi::Local<cXyz> v;
            cXyz_mi(&shot->current.pos, d, GXYZ(0x1940));
            v->copy(*d);
            if (fpcM_GetName(shot) == 0x1D8) {
                f32 len = std_sqrtf(PSVECSquareMag(v));
                if (len < shot->speedF * 10.0f && gabi::load<u8>(gabi::ea(shot) + 0x3AC) != 0 && GF(s8, 0x60C) < 2) {
                    s16 dd = i_this->shape_angle.y - shot->current.angle.y;
                    if (dd < 0) dd = -dd;
                    if ((u16)dd < 0x4000) {
                        GF(u8, 0x172E) = 0;
                    } else {
                        GF(s16, 0x3EA) = 0xA;
                        s16 m = 0;
                        if (checkGround(i_this, 50.0f)) m = 0xF;
                        GF(s16, 0x3EC) = m;
                        GF(u8, 0x60F) = 0;
                        GF(s16, 0x428) = 0x1E;
                    }
                }
            } else {
                f32 len = std_sqrtf(PSVECSquareMag(v));
                if (len < shot->speedF * 10.0f) hit = true;
            }
        }
        f32 pd = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
        if (!hit) {
            check = gabi::load<u8>(gabi::ea(player) + 0x3AC) != 0 && pd < HIO_F(0x3C);
        }
        if (check) {
            if (GF(s16, 0x3EA) != 0xA) {
                GF(u8, 0x609) = 0;
                GF(u8, 0x60F) = 0;
                GF(u8, 0x610) = 0;
                GF(s16, 0x3EA) = 0xA;
                GF(s16, 0x3EC) = 0;
            }
            GF(s16, 0x41E) = REG_S(10, 3) + 2;
        }
    }
    damage_check(i_this);
    GF(u8, 0x172E) = 1;
    if (gabi::ea(&i_this->mStts) != 0) {
        cXyz* mv = &i_this->mStts.m_cc_move;
        i_this->current.pos.x = gabi::fadds_ppc(i_this->current.pos.x, mv->x);
        i_this->current.pos.y = gabi::fadds_ppc(i_this->current.pos.y, mv->y);
        i_this->current.pos.z = gabi::fadds_ppc(i_this->current.pos.z, mv->z);
    }
    if (GF(f32, 0x42C) > 0.01f) { /* ble: taken on NaN */
        gabi::Local<cXyz> p;
        gabi::Local<cXyz> o;
        p->z = GF(f32, 0x42C);
        p->x = 0.0f;
        p->y = 0.0f;
        mDoMtx_YrotS(calc_mtx(), GF(s16, 0x430));
        MtxPosition(p, o);
        PSVECAdd(&i_this->current.pos, o, &i_this->current.pos);
        cLib_addCalc0(&GF(f32, 0x42C), 1.0f, REG0_F(12) + 7.0f);
    }
}
VERIFY(0x02159FC8, gnd_move);
