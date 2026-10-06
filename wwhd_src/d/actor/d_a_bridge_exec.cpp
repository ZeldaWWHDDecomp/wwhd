/**
 * d_a_bridge_exec.cpp (WWHD)
 * Rope bridge: daBridge_Execute (with bridge_move, control1/2/3, cut_control1/2 and search_aite
 * inlined, as GHS compiled them).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bridge.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bridge.h"

/* functions of d_a_bridge.cpp, called by address */
static inline void kikuzu_set(bridge_class* i_this, cXyz* pPos) { gabi::call(0x020DFE7C, i_this, pPos); }
static inline void himo_cut_control1(cXyz* pPos) { gabi::call(0x020E0FAC, pPos); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0257DB04 dKyw_get_wind_power (f32*), 0257DAA8 dKyw_get_wind_vec (cXyz*) (as d_a_npc_ji1_exec) */
static inline u32 dKyw_get_wind_power() { return gabi::call<u32>(0x0257DB04); }
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
/* dComIfGp_event_runCheck(): the play object's event flag (u8 at play+0x5292) */
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
/* 028F43F8 sin (double) */
static inline f64 sin_d(f64 x) { return gabi::call<f64>(0x028F43F8, x); }
/* 02518DB0 at_power_check(CcAtInfo*) (as d_a_bk_exec) */
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
/* JPABaseEmitter (HD): setGlobalRTMatrix = JPASetRMtxTVecfromMtx(m, +0x1F0, +0x22C);
 * becomeInvalidEmitter: +0x5C = -1, flags +0x254 |= 1 (as d_a_sss) */
static inline void JPA_setGlobalRTMatrix(u32 e, Mtx34* m) { gabi::call(0x028249B0, m, e + 0x1F0, e + 0x22C); }
static inline void JPA_becomeInvalidEmitter(u32 e) {
    u32 f = gabi::load<u32>(e + 0x254);
    gabi::store<u32>(e + 0x5C, 0xFFFFFFFF);
    gabi::store<u32>(e + 0x254, f | 1);
}
/* cM_scos: the cosine of the sin/cos table (8 bytes per entry) */
static inline f32 cM_scos_l(s32 a) { return gabi::load<f32>(0x104A44F8 + (((s32)(a & 0xFFFF) >> 3) << 3) + 4); }
static inline void copy_words(u32 dst, u32 src) {
    gabi::store<u32>(dst + 0, gabi::load<u32>(src + 0));
    gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
    gabi::store<u32>(dst + 8, gabi::load<u32>(src + 8));
}
static inline void copy_floats(cXyz* dst, const cXyz* src) {
    f32 x = src->x;
    dst->x = x;
    f32 y = src->y;
    dst->y = y;
    f32 z = src->z;
    dst->z = z;
}
#define ITA_Z_P 0x10192784 /* static f32 ita_z_p[11] */
static inline f32 ita_z_p(int j) { return gabi::load<f32>(ITA_Z_P + (5 + j) * 4); }
static inline br_s* br_at(bridge_class* i_this, int i) { return gabi::at<br_s>(gabi::ea(&i_this->mBr[0]) + i * 0x680); }
static inline f32 wind_power() { return gabi::load<f32>(gabi::load<u32>(BRIDGE_WP)); }
static inline s16 wind_y() { return gabi::load<s16>(BRIDGE_WY); }

/* control1 (inline): the planks from the start, swinging */
static inline void control1(bridge_class* i_this, br_s* pBr) {
    gabi::Local<cXyz> sp3C;
    gabi::Local<cXyz> sp30;
    gabi::Local<cXyz> sp24;
    gabi::Local<cXyz> sp18;
    gabi::Local<cXyz> sp0C;

    pBr++;
    s16 ec = i_this->m02EC, ee = i_this->m02EE, f0 = i_this->m02F0, f2 = i_this->m02F2;
    i_this->m02EC = (s16)(ec + f0);
    i_this->m02EE = (s16)(ee + f2);
    s16 sVar13 = i_this->mBrCount > 10 ? 4000 : 8000;

    sp3C->y = 0.0f;
    sp3C->z = 0.0f;
    sp3C->x = cM_scos_l(i_this->m02EC) * i_this->m02F8;
    cMtx_YrotS(calc_mtx(), i_this->home.angle.y);
    MtxPosition(sp3C, sp24);
    sp3C->x = 1.0f;
    MtxPosition(sp3C, sp18);
    sp3C->x = 0.0f;
    sp3C->z = wind_power() * 5.0f;
    cMtx_YrotS(calc_mtx(), wind_y());
    MtxPosition(sp3C, sp0C);
    sp3C->x = 0.0f;
    sp3C->z = 75.0f;

    for (int i = 1; i < i_this->mBrCount; i++, pBr++) {
        br_s* prev = pBr - 1;
        f32 m3F0 = pBr->m3F0;
        f32 tmp = gabi::fmadds(pBr->m3F8, 0.5f, gabi::fmadds(pBr->m3FC * m3F0, 0.5f, pBr->m3CC.y));
        f32 sinA = cM_ssin(i_this->m02EC + i * sVar13);
        f32 sinB = cM_ssin(i_this->m02EE + i * (sVar13 + 1000));
        f32 fVar8 = sinA * i_this->m02F4 * m3F0;
        f32 x = gabi::fmadds(sp24->x, m3F0, gabi::fmadds(fVar8, sp18->x, pBr->m3CC.x - prev->m3CC.x)) + sp0C->x;
        f32 y = gabi::fmadds(sinB * i_this->m02FC, m3F0, tmp - prev->m3CC.y);
        f32 z = gabi::fmadds(sp24->z, m3F0, gabi::fmadds(fVar8, sp18->z, pBr->m3CC.z - prev->m3CC.z)) + sp0C->z;

        s16 atan = cM_atan2s(x, z);
        s16 atan2 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        cMtx_YrotS(calc_mtx(), atan);
        cMtx_XrotM(calc_mtx(), atan2);
        MtxPosition(sp3C, sp30);
        pBr->m3CC.x = prev->m3CC.x + sp30->x;
        pBr->m3CC.y = prev->m3CC.y + sp30->y;
        pBr->m3CC.z = prev->m3CC.z + sp30->z;
    }
}

/* control2 (inline): the planks from the end */
static inline void control2(bridge_class* i_this, br_s* pBr) {
    gabi::Local<cXyz> sp18;
    gabi::Local<cXyz> sp0C;
    pBr += i_this->mBrCount - 2;
    sp18->x = 0.0f;
    sp18->y = 0.0f;
    sp18->z = 75.0f;
    for (int i = 0; i < i_this->mBrCount - 1; i++, pBr--) {
        br_s* next = pBr + 1;
        f32 z = pBr->m3CC.z - next->m3CC.z;
        f32 tmp = gabi::fmadds(pBr->m3F8, 0.5f, gabi::fmadds(pBr->m3FC * pBr->m3F0, 0.5f, pBr->m3CC.y));
        f32 x = pBr->m3CC.x - next->m3CC.x;
        f32 y = tmp - next->m3CC.y;
        s16 atan = cM_atan2s(x, z);
        s16 atan2 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        next->mRotation.y = atan;
        next->mRotation.x = atan2;
        cMtx_YrotS(calc_mtx(), atan);
        cMtx_XrotM(calc_mtx(), atan2);
        MtxPosition(sp18, sp0C);
        pBr->m3CC.x = next->m3CC.x + sp0C->x;
        pBr->m3CC.y = next->m3CC.y + sp0C->y;
        pBr->m3CC.z = next->m3CC.z + sp0C->z;
    }
}

/* control3 (inline): the first plank's angle */
static inline void control3(bridge_class* i_this, br_s* pBr) {
    f32 x = pBr->m3CC.x - pBr[1].m3CC.x;
    f32 z = pBr->m3CC.z - pBr[1].m3CC.z;
    f32 y = pBr->m3CC.y - pBr[1].m3CC.y;
    pBr->mRotation.y = cM_atan2s(x, z);
    pBr->mRotation.x = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
}

/* cut_control1 (inline): the half hanging from the start after the bridge broke */
static inline void cut_control1(bridge_class* i_this, br_s* pBr) {
    gabi::Local<cXyz> sp24;
    gabi::Local<cXyz> sp18;
    gabi::Local<cXyz> spC;
    pBr++;
    cMtx_YrotS(calc_mtx(), i_this->home.angle.y);
    sp24->z = 1.0f;
    sp24->x = 0.0f;
    sp24->y = 0.0f;
    MtxPosition(sp24, spC);
    sp24->z = 75.0f;

    for (int i = 1; i < i_this->m0304; i++, pBr++) {
        br_s* prev = pBr - 1;
        f32 fVar1 = pBr->m3EC + 30.0f;
        f32 fVar2 = pBr->m3CC.y + pBr->m3FC;
        if (fVar2 < fVar1) {
            pBr->m407 = pBr->m407 + 1;
            fVar2 = fVar1;
        }
        f32 x = (pBr->m3CC.x - prev->m3CC.x) + spC->x;
        f32 z = (pBr->m3CC.z - prev->m3CC.z) + spC->z;
        f32 y = fVar2 - prev->m3CC.y;
        s16 atan = cM_atan2s(x, z);
        s16 atan2 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        s16 ry = (s16)(atan + 0x8000);
        s16 rx = (s16)-atan2;
        prev->mRotation.y = ry;
        prev->mRotation.x = rx;
        if ((u32)i == (u32)(i_this->m0304 - 1)) {
            pBr->mRotation.y = ry;
            pBr->mRotation.x = rx;
        }
        cMtx_YrotS(calc_mtx(), atan);
        cMtx_XrotM(calc_mtx(), atan2);
        MtxPosition(sp24, sp18);
        pBr->m3CC.x = prev->m3CC.x + sp18->x;
        pBr->m3CC.y = prev->m3CC.y + sp18->y;
        pBr->m3CC.z = prev->m3CC.z + sp18->z;
    }
}

/* cut_control2 (inline): the half hanging from the end */
static inline void cut_control2(bridge_class* i_this, br_s* pBr) {
    gabi::Local<cXyz> sp24;
    gabi::Local<cXyz> sp18;
    gabi::Local<cXyz> spC;
    pBr += i_this->mBrCount - 2;
    cMtx_YrotS(calc_mtx(), i_this->home.angle.y);
    sp24->y = 0.0f;
    sp24->z = -1.0f;
    sp24->x = 0.0f;
    MtxPosition(sp24, spC);
    sp24->z = 75.0f;

    for (int i = 0; i < (i_this->mBrCount - 1) - i_this->m0304; i++, pBr--) {
        br_s* next = pBr + 1;
        f32 fVar2 = pBr->m3CC.y + pBr->m3FC;
        f32 fVar1 = pBr->m3EC + 30.0f;
        if (fVar2 < fVar1) {
            pBr->m407 = pBr->m407 + 1;
            fVar2 = fVar1;
        }
        f32 x = (pBr->m3CC.x - next->m3CC.x) + spC->x;
        f32 z = (pBr->m3CC.z - next->m3CC.z) + spC->z;
        f32 y = fVar2 - next->m3CC.y;
        s16 atan = cM_atan2s(x, z);
        s16 atan2 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        next->mRotation.y = atan;
        next->mRotation.x = atan2;
        if ((u32)i == (u32)(((i_this->mBrCount - 1) - i_this->m0304) - 1)) {
            pBr->mRotation.y = atan;
            pBr->mRotation.x = atan2;
        }
        cMtx_YrotS(calc_mtx(), atan);
        cMtx_XrotM(calc_mtx(), atan2);
        MtxPosition(sp24, sp18);
        pBr->m3CC.x = next->m3CC.x + sp18->x;
        pBr->m3CC.y = next->m3CC.y + sp18->y;
        pBr->m3CC.z = next->m3CC.z + sp18->z;
    }
}

/* bridge_move, case 3: the planks follow the ropes, react to riders and hits */
static inline void bridge_move_normal(bridge_class* i_this, fopAc_ac_c* player) {
    br_s* pBr = &i_this->mBr[0];
    gabi::Local<cXyz> sp2C;
    i_this->m0300 = (s16)(i_this->m0300 + 3000);
    copy_words(gabi::ea(&pBr->m3CC), gabi::ea(&i_this->home.pos));

    if ((i_this->mTypeBits & 1) == 1) {
        gabi::Local<cXyz> sp38;
        cMtx_YrotS(calc_mtx(), i_this->home.angle.y);
        sp38->z = 0.0f;
        sp38->x = cM_scos_l(i_this->m02EC) * i_this->m02F8 * -2.0f;
        sp38->y = 0.0f;
        MtxPosition(sp38, sp2C);
        PSVECAdd(&pBr->m3CC, sp2C, &pBr->m3CC);
    }

    control1(i_this, pBr);

    copy_words(gabi::ea(&br_at(i_this, i_this->mBrCount - 1)->m3CC), gabi::ea(&i_this->mEndPos));
    if ((i_this->mTypeBits & 1) == 1) {
        cXyz* last = &br_at(i_this, i_this->mBrCount - 1)->m3CC;
        gabi::call(0x028E8DAC, last, sp2C.get(), last); /* PSVECSubtract */
    }

    control2(i_this, pBr);
    control3(i_this, pBr);

    gabi::Local<cXyz> sp14;
    cXyz_mi(&i_this->home.pos, sp14, &pBr->m3CC);
    f32 dz = sp14->z, dy = sp14->y, dx = sp14->x;
    copy_words(gabi::ea(&i_this->current.pos), gabi::ea(&pBr->m3CC));
    i_this->current.angle.x = pBr->mRotation.x;
    i_this->current.angle.y = pBr->mRotation.y;
    i_this->current.angle.z = pBr->mRotation.z;

    for (int i = 0; i < i_this->mBrCount; i++, pBr++) {
        f32 py = pBr->m3CC.y;
        f32 pz = pBr->m3CC.z;
        pBr->mPosition.y = py;
        f32 px = pBr->m3CC.x;
        pBr->mPosition.z = pz;
        pBr->mPosition.x = px;
        s32 cnt = i_this->mBrCount;
        f32 tmpf = ((f32)(cnt - i) / (f32)cnt) * 0.75f;
        pBr->mPosition.x = gabi::fmadds(dx, tmpf, px);
        pBr->mPosition.y = gabi::fmadds(dy, tmpf, py);
        pBr->mPosition.z = gabi::fmadds(dz, tmpf, pz);

        if (pBr->m406 != 0) {
            for (int j = -5; j <= 5; j++) {
                if ((i + j) < 0 || (i + j) >= i_this->mBrCount) {
                    continue;
                }
                s16 my_tgt = (s16)gabi::ftoi((f32)pBr->m400 * ita_z_p(j) * pBr[j].m3F0);
                cLib_addCalcAngleS2(&pBr[j].m402, my_tgt, 4, 0x800);
                cLib_addCalc2(&pBr[j].m3F8, pBr->m3F4 * ita_z_p(j), 1.0f, 10.0f);
            }
        }

        if ((pBr->m408 & 4) != 0) {
            if ((pBr->m408 & 3) != 3) {
                f32 fVar14 = 0.0f;
                f32 fVar2 = -80.0f;
                if ((pBr->m408 & 3) == 1) {
                    fVar14 = 7000.0f;
                    fVar2 = -30.0f;
                } else if ((pBr->m408 & 3) == 2) {
                    fVar14 = -7000.0f;
                    fVar2 = -30.0f;
                }
                for (int j = -5; j <= 5; j++) {
                    if ((i + j) < 0 || (i + j) >= i_this->mBrCount) {
                        continue;
                    }
                    s16 my_tgt = (s16)gabi::ftoi(fVar14 * ita_z_p(j) * pBr[j].m3F0);
                    cLib_addCalcAngleS2(&pBr[j].m404, my_tgt, 4, 0x800);
                    cLib_addCalc2(&pBr[j].m3F8, fVar2 * ita_z_p(j), 1.0f, 15.0f);

                    if ((pBr->m408 & 3) == 0 && (i_this->mTypeBits & 4) == 0 && (u32)(j + 2) < 5 && pBr[j].m406 != 0) {
                        if (dComIfGp_event_runCheck()) {
                            i_this->m0308 = 0;
                        } else {
                            s32 v = i_this->m0308 + 2;
                            i_this->m0308 = v;
                            /* HD: faster with the player flag 0x02000000 (+0x3B8) */
                            if (gabi::load<u32>(gabi::ea(player) + 0x3B8) & 0x02000000) {
                                v += 8;
                                i_this->m0308 = v;
                            }
                            if (v > 100) {
                                i_this->mMoveProcMode = 4;
                                i_this->m0304 = i + j;
                                if (i_this->m033C != 0) {
                                    gabi::Local<cXyz> dir;
                                    dVibration_c* vib = dComIfGp_getVibration();
                                    dir->x = 0.0f;
                                    dir->y = 1.0f;
                                    dir->z = 0.0f;
                                    gabi::call<BOOL>(0x025CB374, vib, 5, -0x21, dir.get()); /* StartShock(REG0_S(2) + 5, -0x21, cXyz(0, 1, 0)) */
                                }
                                break;
                            }
                        }
                    }
                    if (pBr[j].m3F4 < -200.0f) {
                        i_this->m0304 = i + j;
                        i_this->mMoveProcMode = 4;
                        break;
                    }
                }
            }
            if ((pBr->m408 & 4) != 0) {
                s16 a0 = pBr->m3A0[0];
                s16 a1 = pBr->m3A0[1];
                if (a0 != 0 || a1 != 0) {
                    f32 f = (f32)(a0 | a1) * 150.0f;
                    f32 fVar14 = cM_ssin(i_this->m0300 * 4) * f;
                    for (int j = -5; j <= 5; j++) {
                        if ((i + j) < 0 || (i + j) >= i_this->mBrCount) {
                            continue;
                        }
                        s16 my_tgt = (s16)gabi::ftoi(fVar14 * ita_z_p(j) * pBr[j].m3F0);
                        pBr[j].m404 = (s16)(pBr[j].m404 + my_tgt);
                    }
                }
            }
        }

        u8 m406 = pBr->m406;
        s16 m404 = pBr->m404;
        s16 m402 = pBr->m402;
        if (m406 != 0) {
            pBr->m406 = m406 - 1;
        }
        pBr->m400 = 0;
        pBr->mRotation.z = (s16)(m402 + m404);
        cLib_addCalcAngleS2(&pBr->m402, 0, 4, 0x400);
        cLib_addCalcAngleS2(&pBr->m404, 0, 4, 0x400);
        cLib_addCalc2(&pBr->m3FC, -15.0f, 1.0f, 5.0f);
        cLib_addCalc0(&pBr->m3F8, 1.0f, 5.0f);
    }

    s32 m0308 = i_this->m0308;
    f32 e0 = i_this->m02E0;
    f32 e4 = i_this->m02E4;
    if (m0308 != 0) {
        i_this->m0308 = m0308 - 1;
    }
    i_this->m02FC = e0;
    i_this->m02F4 = e0;
    i_this->m02F8 = e4;
    i_this->m02F2 = 3000;
    i_this->m02F0 = 0x578;

    /* GameCube: *wp > 0.1f ? 2.0f : 0.0f (HD: fsel on 0.1f - *wp) */
    f32 tmpf2 = (0.1f - wind_power()) >= 0.0f ? 0.0f : 2.0f;
    cLib_addCalc2(&i_this->m02E0, tmpf2, 0.1f, 0.1f);
    cLib_addCalc2(&i_this->m02E4, tmpf2 * 0.3f, 0.1f, 0.05f);
}

/* bridge_move (inline) */
static inline void bridge_move(bridge_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    br_s* pBr = &i_this->mBr[0];

    switch (i_this->mMoveProcMode) {
    case 0:
        i_this->m02D9 = 0;
        i_this->mMoveProcMode = 2;
        /* HD: no fopAcM_OffStatus(CULL) */
        /* fallthrough */
    case 2:
        for (int i = 0; i < i_this->mBrCount; i++) {
            br_s* b = br_at(i_this, i);
            if ((i_this->mTypeBits & 1) == 1) {
                b->m3F0 = 1.0f;
                b->m3A5 = 3;
                b->m3A4 = 3;
            } else {
                f32 tmpf = 3.1415927f * ((f32)i / (f32)(i_this->mBrCount - 1));
                f32 s = (f32)(sin_d(tmpf) * 1.0);
                b->m3A5 = 3;
                b->m3A4 = 3;
                b->m3F0 = fabsf(s);
            }
        }
        i_this->mMoveProcMode = 3;
        /* fallthrough */
    case 3:
        bridge_move_normal(i_this, player);
        break;

    case 4: {
        gabi::Local<cXyz> sp38;
        for (int i = 0; i < i_this->mBrCount; i++, pBr++) {
            pBr->m3FC = 0.0f;
            u32 m0304 = i_this->m0304;
            if ((u32)i == m0304 || (u32)i == m0304 - 1 || (u32)i == m0304 + 1) {
                for (int k = 0; k < 2; k++) {
                    f32 x = pBr->mPosition.x;
                    sp38->x = x;
                    sp38->y = pBr->mPosition.y;
                    sp38->z = pBr->mPosition.z;
                    sp38->x = x + cM_rndFX(50.0f);
                    sp38->z = sp38->z + cM_rndFX(50.0f);
                    kikuzu_set(i_this, sp38);
                }
            }
        }
        i_this->mMoveProcMode = 5;
        i_this->m0312 = 50;
        fopAcM_seStart(player, 0x6933 /* JA_SE_OBJ_SBRIDGE_BREAK */, 0);
    }
        /* fallthrough */
    case 5: {
        pBr = &i_this->mBr[0];
        s16 m0312 = i_this->m0312;
        s16 m0300 = i_this->m0300;
        if (m0312 != 0) {
            i_this->m0312 = m0312 - 1;
        }
        i_this->m0300 = (s16)(m0300 + 4000);
        copy_words(gabi::ea(&pBr->m3CC), gabi::ea(&i_this->home.pos));
        cut_control1(i_this, pBr);
        copy_words(gabi::ea(&br_at(i_this, i_this->mBrCount - 1)->m3CC), gabi::ea(&i_this->mEndPos));
        cut_control2(i_this, pBr);
        for (int i = 0; i < i_this->mBrCount; i++, pBr++) {
            copy_words(gabi::ea(&pBr->mPosition), gabi::ea(&pBr->m3CC));
            cLib_addCalc2(&pBr->m3FC, -50.0f, 1.0f, 5.0f);
            pBr->m3EC = -10000.0f;
        }
        break;
    }
    default:
        break;
    }
}

/* the rope hit checks of one plank (type 0 bridges: cuttable ropes) */
static inline void rope_hit(bridge_class* i_this, br_s* pBr, fopAc_ac_c* a_player, int side, u32* soundId, bool* bVar1) {
    dCcD_Cyl* cyl = &pBr->mCyl[side];
    if (pBr->m3A0[side] < 10) {
        u32 w = gabi::ea(cyl) + 0x94; /* OnTgNoConHit */
        gabi::store<u32>(w, gabi::load<u32>(w) | 2);
        pBr->m3A0[side] = 20;
        *soundId = 0x2838; /* JA_SE_LK_CUT_SBRIDGE_ROPE */
        gabi::Local<CcAtInfo_l> atInfo;
        atInfo->mpObj = gabi::ea(cyl->GetTgHitObj());
        at_power_check(atInfo);
        u8 dmg = atInfo->mDamage;
        if (dmg > 1) {
            dmg = 4;
            atInfo->mDamage = dmg;
        }
        be<s8>& hp = side == 0 ? pBr->m3A4 : pBr->m3A5;
        s8 left = (s8)(hp - dmg);
        hp = left;
        if (left <= 0) {
            pBr->m408 = pBr->m408 & (side == 0 ? 0xE : 0xD);
        } else {
            *bVar1 = true;
        }
        dComIfGp_particle_set(0xD /* dPa_name::ID_AK_JN_OK */, &pBr->m3A8[side], &a_player->shape_angle);
        kikuzu_set(i_this, &pBr->m3A8[side]);
    }
}

/* the chain hit checks (type 1/8 bridges) */
static inline void chain_hit(br_s* pBr, fopAc_ac_c* a_player, int side, u32* soundId) {
    dCcD_Cyl* cyl = &pBr->mCyl[side];
    if (pBr->m3A0[side] < 10) {
        u32 w = gabi::ea(cyl) + 0x94; /* OnTgNoConHit */
        gabi::store<u32>(w, gabi::load<u32>(w) | 2);
        pBr->m3A0[side] = 15;
        *soundId = 0x2839; /* JA_SE_LK_HIT_SBRIDGE_CHAIN */
        dComIfGp_particle_set(0xC /* dPa_name::ID_AK_JN_NG */, cyl->GetTgHitPosP(), &a_player->shape_angle,
                              gabi::at<cXyz>(0x10462968) /* spB4 */);
    }
}

/* the rope fire of one side */
static inline void rope_fire(bridge_class* i_this, br_s* pBr, int side, bool bGotFlamePos, cXyz* spC0, cXyz* spA8) {
    cXyz* top = side == 0 ? &pBr->m11C[1] : &pBr->m0F8[1];
    be<s16>& timer = side == 0 ? pBr->m3C0 : pBr->m3C2;
    gptr<JPABaseEmitter>& emitter = side == 0 ? pBr->m3C4 : pBr->m3C8;
    copy_floats(spA8, top);
    if ((pBr->m408 & (side == 0 ? 1 : 2)) == 0) {
        spA8->y = spA8->y - 10000.0f;
    }
    pBr->mCyl[side].SetC(spA8);

    if (timer != 0) {
        timer = timer - 1;
        if (emitter != nullptr) {
            mDoMtx_stack_c::transS(top->x, top->y + 100.0f, top->z);
            JPA_setGlobalRTMatrix(gabi::ea(emitter.get()), mDoMtx_stack_c::get());
            if (timer == 0) {
                JPA_becomeInvalidEmitter(gabi::ea(emitter.get()));
                emitter = nullptr;
            }
        }
        if (timer == 0) {
            pBr->m408 = pBr->m408 & (side == 0 ? 0xE : 0xD);
        }
    } else if (bGotFlamePos && (i_this->mTypeBits & 9) == 0) {
        spA8->y = spA8->y + 100.0f;
        gabi::Local<cXyz> d;
        cXyz_mi(spA8, d, spC0);
        gabi::Local<cXyz> sp48;
        sp48->x = d->x;
        sp48->z = d->z;
        sp48->y = d->y * 0.4f;
        if (std_sqrtf(PSVECSquareMag(sp48)) < 50.0f) {
            timer = 30;
            emitter = dComIfGp_particle_set(0x80EA /* dPa_name::ID_AK_SN_BRIDGEROPEFIRE00 */, spA8);
        }
    }
}

/* the side rope's segment at this plank: hanging from the plank, or cut */
static inline void side_rope_segment(bridge_class* i_this, br_s* pBr, int line, u32 seg, u32 idx, cXyz* sp84) {
    u32 s = seg + idx * 0xC;
    gabi::Local<cXyz> d;
    cXyz_mi(gabi::at<cXyz>(s - 0xC), d, gabi::at<cXyz>(s + 0xC));
    gabi::Local<cXyz> sp30;
    sp30->z = d->z;
    sp30->y = d->y;
    sp30->x = d->x;
    f32 sn = cM_ssin(i_this->m0300 * 6);
    f32 a0 = (f32)pBr->m3A0[line];
    f32 x = gabi::fmadds(sp30->x, 0.5f, gabi::load<f32>(s + 0xC)) + sp84->x;
    gabi::store<f32>(s + 0, x);
    f32 y = gabi::fmadds(sn, a0, gabi::fmadds(sp30->y, 0.5f, gabi::load<f32>(s + 0x10))) - 10.0f;
    gabi::store<f32>(s + 4, y);
    f32 z = gabi::fmadds(sp30->z, 0.5f, gabi::load<f32>(s + 0x14)) + sp84->z;
    gabi::store<f32>(s + 8, z);

    u32 lines = gabi::load<u32>(gabi::ea(&pBr->mLineMat1) + 0x184);
    u32 segment00 = gabi::load<u32>(lines + line * 0x10);
    u32 segment01 = gabi::load<u32>(lines + (line + 2) * 0x10);
    gabi::store<f32>(segment00 + 0, gabi::load<f32>(seg + idx * 0xC));
    gabi::store<f32>(segment00 + 4, gabi::load<f32>(seg + idx * 0xC + 4));
    gabi::store<f32>(segment00 + 8, z);
    himo_cut_control1(gabi::at<cXyz>(segment00));
    copy_words(segment01, gabi::ea(line == 0 ? &pBr->m11C[1] : &pBr->m0F8[1]));
    himo_cut_control1(gabi::at<cXyz>(segment01));
}

/* 020E11E8 */
BOOL daBridge_Execute(bridge_class* i_this) {
    WWHD_FUNC(0x020E11E8, BOOL, i_this);
    /* HD: no distance culling (GameCube: mbStopDraw); nothing happens during an event while the
     * rumble timer runs */
    if (dComIfGp_event_runCheck() && i_this->m033C != 0) {
        return TRUE;
    }
    fopAc_ac_c* a_player = dComIfGp_getPlayer(0);
    if (i_this->m033C != 0) {
        i_this->m033C = i_this->m033C - 1;
    }

    cXyz* wind_vec = dKyw_get_wind_vec();
    gabi::store<u32>(BRIDGE_WIND_VEC, gabi::ea(wind_vec));
    gabi::store<s16>(BRIDGE_WY, cM_atan2s(wind_vec->x, wind_vec->z));
    gabi::store<u32>(BRIDGE_WP, dKyw_get_wind_power());
    i_this->m0302 = i_this->m0302 + 1;

    if ((i_this->mTypeBits & 2) != 0 && i_this->mpAite == nullptr) {
        i_this->mpAite = (bridge_class*)fpcM_Search(0x020E115C /* s_a_b_sub */, i_this); /* search_aite */
    }

    bridge_move(i_this);

    i_this->m030C = 0;
    gabi::Local<cXyz> spC0;
    br_s* pBr = &i_this->mBr[0];
    bool bGotFlamePos = false;
    /* daPy_py_c::getBokoFlamePos (virtual, vtable +0x74) */
    if (gabi::call_ptr<BOOL>(gabi::load<u32>(gabi::load<u32>(gabi::ea(a_player) + 0xB4) + 0x74), a_player, spC0.get()) != FALSE) {
        bGotFlamePos = true;
    }

    /* HD: the rope tops: GameCube +1000 (chains) / +200, HD 1000 - 300 sin (chains) / 230 - 110 sin
     * over the bridge (half a period for joined bridges) */
    f32 topBase, topSwing;
    if (i_this->mTypeBits & 1) {
        topBase = 1000.0f;
        topSwing = 300.0f;
    } else {
        topBase = 230.0f;
        topSwing = 110.0f;
    }

    gabi::Local<cXyz> eyeDir;
    gabi::Local<cXyz> spA8;
    gabi::Local<cXyz> sp84;
    for (int i = 0; i < i_this->mBrCount; i++, pBr++) {
        MtxTrans(pBr->mPosition.x, pBr->mPosition.y, pBr->mPosition.z, 0);
        cMtx_YrotM(calc_mtx(), pBr->mRotation.y);
        cMtx_XrotM(calc_mtx(), pBr->mRotation.x);
        mDoMtx_ZrotM(calc_mtx(), pBr->mRotation.z);

        s32 m0304 = i_this->m0304;
        if (m0304 != 0) {
            MtxTrans(0.0f, 0.0f, i > m0304 ? 30.0f : -30.0f, 1);
        }

        f32 sx = pBr->mScale.x;
        eyeDir->y = 0.0f;
        eyeDir->z = 0.0f;
        eyeDir->x = 99.0f * sx;
        MtxPosition(eyeDir, &pBr->m11C[1]);
        eyeDir->x = -eyeDir->x; /* GameCube: *= -1.0f (fneg) */
        MtxPosition(eyeDir, &pBr->m0F8[1]);
        eyeDir->y = -30.0f; /* REG0_F(4) + -30.0f */
        MtxPosition(eyeDir, &pBr->m0F8[2]);
        eyeDir->x = -eyeDir->x;
        MtxPosition(eyeDir, &pBr->m11C[2]);

        if ((pBr->m408 & 4) != 0) {
            s16 m418 = pBr->m418;
            if (m418 != 0) {
                if (m418 > 0) {
                    pBr->m418 = m418 - 1;
                }
                f32 ratio = (f32)i / (f32)(i_this->mBrCount - 1);
                f64 s;
                if (i_this->mTypeBits & 2) {
                    s = sin_d(1.5707964f * ratio);
                } else {
                    s = sin_d(3.1415927f * ratio);
                }
                f32 add = (f32)(s * (f64)(-topSwing) + (f64)topBase); /* fmadds with the double sine */
                f32 x1 = pBr->m11C[1].x, y1 = pBr->m11C[1].y, z1 = pBr->m11C[1].z;
                f32 x0 = pBr->m0F8[1].x, y0 = pBr->m0F8[1].y, z0 = pBr->m0F8[1].z;
                pBr->m11C[0].z = z1;
                pBr->m11C[0].x = x1;
                pBr->m0F8[0].x = x0;
                pBr->m11C[0].y = y1 + add;
                pBr->m0F8[0].y = y0 + add;
                pBr->m0F8[0].z = z0;
                if ((i_this->mTypeBits & 2) != 0 && (u32)i == (u32)(i_this->m02DD - 1)) {
                    copy_words(gabi::ea(&i_this->m0320), gabi::ea(&pBr->m11C[0]));
                    copy_words(gabi::ea(&i_this->m032C), gabi::ea(&pBr->m0F8[0]));
                }
            }

            u32 soundId = 0;
            bool bVar1 = false;
            if ((i_this->mTypeBits & 9) == 0) {
                if (i_this->mBrCount - i < 3) {
                    pBr->m408 = 0;
                }
                if (i_this->m0304 != 0) {
                    pBr->m408 = pBr->m408 & 0xC;
                }
                if (pBr->mCyl[0].ChkTgHit()) {
                    rope_hit(i_this, pBr, a_player, 0, &soundId, &bVar1);
                }
                if (pBr->mCyl[1].ChkTgHit()) {
                    rope_hit(i_this, pBr, a_player, 1, &soundId, &bVar1);
                }
            } else {
                /* HD: spB4 is a function-local static (2, 2, 2) */
                be<u32>& guard = *gabi::at<be<u32>>(0x10462974);
                if (guard == 0) {
                    gabi::store<f32>(0x1046296C, 2.0f);
                    gabi::store<f32>(0x10462970, 2.0f);
                    guard = 1;
                    gabi::store<f32>(0x10462968, 2.0f);
                }
                if (pBr->mCyl[0].ChkTgHit()) {
                    chain_hit(pBr, a_player, 0, &soundId);
                }
                if (pBr->mCyl[1].ChkTgHit()) {
                    chain_hit(pBr, a_player, 1, &soundId);
                }
            }

            if (soundId != 0) {
                if ((i_this->mTypeBits & 8) != 0 || bVar1) {
                    soundId = 0x2837; /* JA_SE_LK_HIT_SBRIDGE_ROPE */
                }
                mDoAud_seStart(soundId, &pBr->m3CC, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
            }

            rope_fire(i_this, pBr, 0, bGotFlamePos, spC0, spA8);
            rope_fire(i_this, pBr, 1, bGotFlamePos, spC0, spA8);

            dComIfG_Ccsp_Set(&pBr->mCyl[0]);
            dComIfG_Ccsp_Set(&pBr->mCyl[1]);
        }

        for (int k = 0; k < 2; k++) {
            if (pBr->m3A0[k] != 0) {
                pBr->m3A0[k] = pBr->m3A0[k] - 1;
            }
        }

        cMtx_YrotM(calc_mtx(), pBr->mRotationYExtra);
        if (i >= i_this->m02DD) {
            pBr->mScale.z = 0.0f;
            pBr->mScale.y = 0.0f;
            pBr->mScale.x = 0.0f;
            J3DModel* m = pBr->mpModel;
            gabi::store<f32>(gabi::ea(m) + 0xBC, 0.0f);
            gabi::store<f32>(gabi::ea(m) + 0xC0, 0.0f);
            gabi::store<f32>(gabi::ea(m) + 0xC4, 0.0f);
            pBr->m408 = 0;
        }
        J3DModel_setBaseTRMtx(pBr->mpModel, calc_mtx());

        if ((i_this->mTypeBits & 1) == 0 && (pBr->m408 & 4) != 0) {
            u32 lines = gabi::load<u32>(gabi::ea(&i_this->mLineMat) + 0x184);
            u32 segment0 = gabi::load<u32>(lines + 0);
            u32 segment1 = gabi::load<u32>(lines + 0x10);
            u32 idx = i_this->m030C + 1;

            eyeDir->x = 0.0f;
            s16 tmpS = (s16)(i_this->m02D9 * 0x5DC);
            /* HD: the angle is summed in float, then converted */
            s32 ang = gabi::ftoi((f32)(i_this->m0302 * 0x578) + (f32)tmpS);
            f32 sn = cM_ssin(ang);
            eyeDir->z = (wind_power() + 0.3f) * ((sn + sn) + 5.0f);
            cMtx_YrotS(calc_mtx(), wind_y());
            MtxPosition(eyeDir, sp84);

            if (pBr->m408 & 1) {
                copy_words(segment0 + idx * 0xC, gabi::ea(&pBr->m11C[0]));
            } else {
                side_rope_segment(i_this, pBr, 0, segment0, idx, sp84);
            }
            if (pBr->m408 & 2) {
                copy_words(segment1 + idx * 0xC, gabi::ea(&pBr->m0F8[0]));
            } else {
                side_rope_segment(i_this, pBr, 1, segment1, idx, sp84);
            }
            i_this->m030C = i_this->m030C + 1;
        }
    }

    dBgWSv_CopyBackVtx(i_this->mpBgW);
    u32 vtxTbl = gabi::load<u32>(gabi::ea(i_this->mpBgW.get()) + 0x90);
    int other_i = 0;
    gabi::Local<cXyz> spCC;
    for (int i = 0; i < gabi::load<s32>(gabi::load<u32>(gabi::ea(i_this->mpBgW.get()) + 0x94)); i++) {
        int sw = i & 3;
        int idx = i >> 2;
        u32 v = vtxTbl + i * 0xC;
        if (idx < i_this->m02DD) {
            pBr = br_at(i_this, idx);
            switch (sw) {
            case 0: copy_words(v, gabi::ea(&pBr->m11C[2])); break;
            case 1: copy_words(v, gabi::ea(&pBr->m0F8[2])); break;
            case 2: copy_words(v, gabi::ea(&pBr->m11C[1])); break;
            case 3: copy_words(v, gabi::ea(&pBr->m0F8[1])); break;
            }
            if (idx == 0 || (u32)idx == (u32)(i_this->m02DD - 1)) {
                cMtx_YrotS(calc_mtx(), pBr->mRotation.y);
                cMtx_XrotM(calc_mtx(), pBr->mRotation.x);
                eyeDir->y = 0.0f;
                eyeDir->x = 0.0f;
                if (idx == 0) {
                    eyeDir->z = 50.0f;
                } else if ((u32)i_this->m02DD == (u32)i_this->mBrCount) {
                    eyeDir->z = -50.0f;
                } else {
                    eyeDir->z = -40.0f;
                }
                MtxPosition(eyeDir, spCC);
                gabi::store<f32>(v + 0, gabi::load<f32>(v + 0) + spCC->x);
                gabi::store<f32>(v + 4, gabi::load<f32>(v + 4) + spCC->y);
                gabi::store<f32>(v + 8, gabi::load<f32>(v + 8) + spCC->z);
            }
            other_i = i;
        } else {
            copy_words(v, vtxTbl + other_i * 0xC);
        }
        if (i_this->mMoveProcMode >= 4) {
            gabi::store<f32>(v + 4, 10000.0f);
        }
    }

    dBgW_Move(i_this->mpBgW);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    return TRUE;
}
VERIFY(0x020E11E8, daBridge_Execute);
