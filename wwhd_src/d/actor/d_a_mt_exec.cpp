/**
 * d_a_mt_exec.cpp (WWHD)
 * Enemy - Magtail: daMt_Execute
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_mt.cpp) has only a "Nonmatching" placeholder for daMt_Execute, so it is written
 * from the WWHD code (cking.rpx, 021D87A0, 16 KB) and verified against it. WWHD inlines the
 * GameCube functions mt_eye_tex_anm, damage_check, mt_move (with body_control1), mt_fight and
 * the line-of-sight/background checks; they are static inline helpers here, named after the
 * GameCube symbols where the code matches them.
 */
#include "d/actor/d_a_mt_exec.h"
#include <cmath>

using namespace MtExec;
static CheckOffsets moveCheckX;
static CheckOffsets moveCheckY;
static CheckOffsets moveCheckZ;
static CheckBits moveCheckBits;
static TrailOffsets trailOffsets;
static TrailOffsets fastTrailOffsets;
static u8 fightSlot;
static u8 enemyHitEffectTimer;

// HD: bindings for helpers inlined into the execution entry point.
/* d_cc_uty: CcAtInfo (0x1C) as in d_a_bb */

static inline void at_power_check(AttackInfo* i) { mtExec_atPower(i); }
static inline u32 cc_at_check(fopAc_ac_c* a, AttackInfo* i) { return mtExec_atCheck(a, i); }
static inline void def_se_set(fopAc_ac_c* a, u32 obj, u32 mtrl) { mtExec_defenceSound(a, obj, mtrl); }
static inline BOOL enemy_ice(enemyice_l* e) { return mtExec_enemyIce(e); }
static inline void fopKyM_createMpillar(cXyz* pos, f32 scale) { mtExec_createMpillar(pos, scale); }
static inline void mDoMtx_XrotS(Mtx34* m, s16 x) { mtExec_XrotS(m, x); }
static inline void* fpcEx_Search(u32 fn, void* data) { return mtExec_search(fn, data); }
static inline BOOL dAttention_LockonTruth(dAttention_c* a) { return mtExec_lockonTruth(a); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return mtExec_actionTarget(a, i); }
/* daPy_py_c::checkPlayerGuard: virtual, slot +0x3C of the HD vtable (+0xB4) */
static inline BOOL daPy_checkPlayerGuard(fopAc_ac_c* p) { return playerGuard(p); }
static inline void mDoAud_onEnemyDamage() { mtExec_onEnemyDamage(); }
static inline void mDoAud_bgmHitSound(s32 id) { mtExec_bgmHitSound(id); }
static inline void dKy_SordFlush_set(cXyz* pos, s32 p) { mtExec_swordFlush(pos, p); }
/* this TU's out-of-line copies (d_a_mt.cpp) */
static inline f32 sinShort_ool(s16 a) { return mtExec_sinShort(trigTable(), a); }
static inline f32 cosShort_ool(s16 a) { return mtExec_cosShort(trigTable(), a); }
static inline void mt_fopAcM_seStart(fopAc_ac_c* a, u32 id, u32 p) { mtExec_seStart(a, id, p); }
static inline void mt_fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 p) { mtExec_monsSeStart(a, id, p); }

#define REG6_F(i) REG_F(6, i)
#define REG6_S(i) REG_S(6, i)
#define REG10_S(i) REG_S(10, i)

/* dBgS_ObjGndChk_Yogan (lava ground check, stack object; this TU's vtables, group 4) */

static inline void dBgS_ObjGndChk_Yogan_ct(LavaGroundCheck* c) {
    mtExec_groundCtor(c); /* cBgS_GndChk::cBgS_GndChk */
    c->mPass[0] = 1;
    for (int i = 1; i < 7; i++) c->mPass[i] = 0;
    c->mCheckVtable = LAVA_CHECK_VTABLE;
    c->mpPolyPassChk = gabi::ea(&c->mPolyPassVtable);
    c->mpGrpPassChk = gabi::ea(&c->mGroupPassVtable);
    c->mGroupPassVtable = LAVA_GROUP_VTABLE;
    c->mPolyPassVtable = LAVA_PASS_VTABLE;
    c->mGrp = 4;
    c->mPolyInfoVtable = LAVA_POLY_VTABLE;
}
static inline void dBgS_ObjGndChk_Yogan_dt(LavaGroundCheck* c) {
    c->mPolyInfoVtable = GROUND_POLY_VTABLE;
    c->mPolyPassVtable = OBJECT_PASS_VTABLE;
    c->mGroupPassVtable = OBJECT_GROUP_VTABLE;
    mtExec_groundDtor(c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}
/* dBgS_ObjLinChk (mPass[0] = 1): its own vtables; the destructor is dBgS_LinChk's */
static inline void dBgS_ObjLinChk_ct(dBgS_LinChk_l* c) {
    cBgS_LinChk_ct(c);
    c->mPass[0] = 1;
    for (int i = 1; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(&c->__vtbl_64);
    c->mpPolyPassChk = gabi::ea(&c->__vtbl_58);
    c->__vtbl_10 = OBJECT_LINE_VTABLE;
    c->__vtbl_64 = OBJECT_LINE_GROUP_VTABLE;
    c->__vtbl_58 = OBJECT_LINE_PASS_VTABLE;
    c->__vtbl_20 = OBJECT_LINE_POLY_VTABLE;
}
static inline cXyz* LinChk_GetCross(dBgS_LinChk_l* c) { return lineCross(c); }

/* .data / statics of the unit */
static inline f32 mt_move_chk_x(u32 i) { return moveCheckX.values[i]; }
static inline f32 mt_move_chk_y(u32 i) { return moveCheckY.values[i]; }
static inline f32 mt_move_chk_z(u32 i) { return moveCheckZ.values[i]; }
static inline u8 mt_move_chk_bit(u32 i) { return moveCheckBits.values[i]; }
static inline s32 mt_trail_ofs(u32 i) { return trailOffsets.values[i]; }
static inline s32 mt_trail_ofs_fast(u32 i) { return fastTrailOffsets.values[i]; }
/* mt_fight: the attack slot (static u8 in .data) */


/* attention_info (fopAc_ac_c +0x388): position +0x08, flags +0x14 */
static inline cXyz* attention_pos(fopAc_ac_c* a) { return &a->attention_info.position; }
static inline void attention_flags_set(fopAc_ac_c* a, u32 f) { a->attention_info.flags = f; }
static inline u8 daPy_procNo(fopAc_ac_c* p) { return player(p)->mCutType; }
static inline s32 abs16(s16 v) { return v < 0 ? -(s32)v : (s32)v; }

/* fopAcM_monsSeStart (HD inline) */
static inline void mt_monsSe(Actor* i_this, u32 id) {
    if (i_this != nullptr && gabi::ea(&i_this->eyePos) != 0) {
        u32 pid = fopAcM_GetID(i_this);
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
        mtExec_monsSe(id, &i_this->eyePos, pid, 0, reverb); /* mDoAud_monsSeStart */
    }
}

/* the collision push of this frame (dCcD_Stts::GetCCMoveP), applied to the position */
static inline void mt_cc_move(Actor* i_this) {
    if (gabi::ea(&i_this->mStts) != 0) {
        i_this->current.pos.x = i_this->current.pos.x + i_this->mStts.m_cc_move.x;
        i_this->current.pos.z = i_this->current.pos.z + i_this->mStts.m_cc_move.z;
    }
}

/* the knock-back: speed from the hit angle */
/* inlined into daMt_Execute */
static inline void mt_knockback(Actor* i_this, s16 angleY) {
    mDoMtx_YrotS(calc_mtx(), angleY);
    cXyz v;
    f32 k = hostIO().mKnockbackScale;
    v.x = 0.0f;
    v.y = 40.0f * k;
    v.z = -20.0f * k;
    MtxPosition(&v, &i_this->speed);
}

/* the point mTargetPos behind the head: mTargetPos = current.pos + rotY(shape_angle.y) * (0, y, z) */
/* inlined into daMt_Execute */
static inline void mt_set_back_pos(Actor* i_this, s16 angleY, f32 y, f32 z) {
    mDoMtx_YrotS(calc_mtx(), angleY);
    cXyz v;
    v.x = 0.0f;
    v.y = y;
    v.z = z;
    MtxPosition(&v, &i_this->mTargetPos);
    PSVECAdd(&i_this->mTargetPos, &i_this->current.pos, &i_this->mTargetPos);
}

/* mt_eye_tex_anm (inlined) */
static inline void mt_eye_tex_anm(Actor* i_this) {
    if (i_this->mBlinkTimer != 0) {
        i_this->mBlinkTimer = i_this->mBlinkTimer - 1;
    } else {
        i_this->mBlinkTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 50.0f);
        if (i_this->mBtpOn == 0) {
            tex_anm_set(i_this, 0);
        }
    }
    if (i_this->mBtpOn != 0) {
        if (i_this->mBtpFrame < i_this->mBtpMaxFrame) {
            i_this->mBtpFrame = i_this->mBtpFrame + 1;
        } else {
            i_this->mBtpOn = 0;
        }
    }
}

/* inlined into daMt_Execute */
static inline void body_control1(Actor* i_this) {
    cLib_addCalcAngleS2(&i_this->shape_angle.x, i_this->current.angle.x, 2, 0x800);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 2, 0x800);
    cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 4, 0x400);

    i_this->mTrailPos[i_this->mTrailIndex].copy(i_this->current.pos);
    csXyz* tr = &i_this->mTrailAngle[i_this->mTrailIndex];
    tr->x = i_this->shape_angle.x;
    tr->y = i_this->shape_angle.y;
    tr->z = i_this->shape_angle.z;
    i_this->mTrailHeadAngle[i_this->mTrailIndex] = i_this->mHeadWobbleAngle;

    for (int i = 0; i < MT_PART_NUM; i++) {
        s32 ofs = i_this->mFastTrail != 0 ? mt_trail_ofs_fast(i) : mt_trail_ofs(i);
        u32 k = (u32)(i_this->mTrailIndex + ofs) & 0x3F;
        J3DModel* model = i_this->mpMorf[i]->getModel();
        /* J3DModel::setBaseScale(scale) */

        f32 sx = i_this->scale.x, sz = i_this->scale.z, sy = i_this->scale.y;
        model->mBaseScale.y = sy;
        model->mBaseScale.x = sx;
        model->mBaseScale.z = sz;
        cXyz* pos = &i_this->mTrailPos[k];
        PSMTXTrans(mDoMtx_stack_c::get(), pos->x, pos->y, pos->z);
        csXyz* ang = &i_this->mTrailAngle[k];
        mDoMtx_YrotM(mDoMtx_stack_c::get(), ang->y);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), ang->x);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), ang->z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), i_this->mTrailHeadAngle[k]);
        if (i == 0) {
            f32 s = hostIO().mHeadScale;
            mDoMtx_stack_c::scaleM(s, s, s);
        } else {
            f32 s = i_this->mSegmentScale[i];
            mDoMtx_stack_c::scaleM(s, s * i_this->mSegmentScaleY[i], 1.0f);
        }
        mDoMtx_stack_c::transM(0.0f, 0.0f, i_this->mBodyOffset);
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
        if (i == 0) {
            cXyz v;
            v.x = 0.0f;
            v.y = 0.0f;
            v.z = REG0_F(9) + 30.0f;
            PSMTXMultVec(mDoMtx_stack_c::get(), &v, &i_this->eyePos);
            i_this->mAtSph.SetC(&i_this->eyePos);
            i_this->mAtSph.SetR(hostIO().mHeadRadius);
            dComIfG_Ccsp_Set(&i_this->mAtSph);
            cXyz v2;
            v2.x = 0.0f;
            v2.y = 0.0f;
            v2.z = REG6_F(9) + 100.0f;
            cXyz headPos;
            PSMTXMultVec(mDoMtx_stack_c::get(), &v2, &headPos);
            i_this->mSph[0].SetC(&headPos);
            i_this->mSph[0].SetR(50.0f);
            dComIfG_Ccsp_Set(&i_this->mSph[0]);
        } else {
            i_this->mSph[i].SetC(pos);
            if (i_this->mInvulnerableTimer != 0) {
                i_this->mSph[i].SetR(-200.0f);
            } else {
                i_this->mSph[i].SetR(hostIO().mBodyRadius);
            }
            dComIfG_Ccsp_Set(&i_this->mSph[i]);
        }
        if (i_this->mStoreTrail != 0) {
            i_this->mSegmentPos[i].copy(*pos);
            csXyz* dst = &i_this->mSegmentAngle[i];
            dst->x = ang->x;
            dst->y = ang->y;
            dst->z = ang->z;
        }
    }
    if (i_this->mWavePauseTimer == 0) {
        i_this->mTrailIndex = (i_this->mTrailIndex + 1) & 0x3F;
    } else {
        i_this->mTrailIndex = i_this->mTrailIndex & 0x3F;
    }
}

/* mt_move (inlined): crawl along walls/ceilings, steered by eight line checks */
/* inlined into daMt_Execute */
static inline void mt_move(Actor* i_this) {
    dComIfGp_get(); /* HD: result unused */
    u32 hit = 0;
    {
        dBgS_LinChk_l linChk;
        dBgS_LinChk_ct(&linChk, MT_LINCHK_VTBLS);
        mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        mDoMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
        cXyz offs;
        offs.x = 0.0f;
        offs.z = 0.0f;
        offs.y = 50.0f;
        cXyz start;
        MtxPosition(&offs, &start);
        cXyz sum;
        cXyz_pl(&start, &sum, &i_this->current.pos);
        start.copy(*&sum);

        int n = abs16(i_this->current.angle.x) < 0x1000 ? 6 : 4;
        SegmentVectors cross;
        SegmentVectors end;
        for (int i = 0; i < n; i++) {
            offs.z = mt_move_chk_z(i);
            offs.y = mt_move_chk_y(i);
            offs.x = mt_move_chk_x(i);
            MtxPosition(&offs, &end.v[i]);
            PSVECAdd(&end.v[i], &i_this->current.pos, &end.v[i]);
            dBgS_LinChk_Set(&linChk, &start, &end.v[i], i_this);
            if (cBgS_LineCross(dComIfG_Bgsp(), &linChk)) {
                cross.v[i].copy(*LinChk_GetCross(&linChk));
                hit |= mt_move_chk_bit(i);
            }
        }

        if ((hit & 3) == 3) {
            /* on a wall: follow it */
            cXyz d;
            cXyz_mi(&cross.v[0], &d, &cross.v[1]);
            offs.y = hostIO().mHoverHeight;
            cXyz dd;
            dd.copy(*&d);
            offs.z = 0.0f;
            offs.x = 0.0f;
            cXyz up;
            MtxPosition(&offs, &up);
            if (i_this->mWavePauseTimer == 0) {
                cLib_addCalc2(&i_this->current.pos.x, gabi::fmadds(dd.x, 0.5f, cross.v[1].x) + up.x, 1.0f, 1.0f);
                cLib_addCalc2(&i_this->current.pos.y, gabi::fmadds(dd.y, 0.5f, cross.v[1].y) + up.y, 1.0f, 1.0f);
                cLib_addCalc2(&i_this->current.pos.z, gabi::fmadds(dd.z, 0.5f, cross.v[1].z) + up.z, 1.0f, 1.0f);
            }
            f32 dz = dd.z;
            f32 dx = dd.x;
            i_this->current.angle.x = (s16)-cM_atan2s(dd.y, std_sqrtf(gabi::fmadds(dx, dx, dz * dz)));
            dx = dd.x;
            dz = dd.z;
            if (fabsf(dx) > 0.1f || fabsf(dz) > 0.1f) {
                i_this->mWallAngle = cM_atan2s(dx, dz);
            }
            s16 diff = (s16)(i_this->mWallAngle - i_this->current.angle.y);
            if (diff < 0) diff = (s16)-diff;
            if ((u16)diff > 0x4000) {
                i_this->current.angle.x = (s16)(-0x8000 - i_this->current.angle.x);
            }
            i_this->mClimbTimer = 0x17;
        } else if (hit & 1) {
            /* keep the pitch */
        } else if (i_this->mClimbTimer != 0) {
            i_this->mClimbTimer = i_this->mClimbTimer - 1;
        } else {
            i_this->current.angle.x = (s16)(i_this->current.angle.x + (REG0_S(2) + 0x800));
        }

        if (abs16(i_this->current.angle.x) < 0x1000) {
            /* on the floor */
            if ((hit & 0x30) == 0x30) {
                cXyz d;
                cXyz_mi(&cross.v[4], &d, &cross.v[5]);
                f32 dx = d.x, dz = d.z;
                cXyz dd;
                dd.copy(*&d);
                i_this->current.angle.y = (s16)(cM_atan2s(dx, dz) + 0x4000);
            } else if (i_this->mType >= 10) {
                i_this->current.angle.y = (s16)(i_this->current.angle.y + i_this->mTurnSpeed);
            } else {
                s16 target;
                bool turn = true;
                if ((s8)i_this->mHasPath != 0) {
                    /* follow the path */
                    cXyz d;
                    cXyz_mi(&i_this->mTargetPos, &d, &i_this->current.pos);
                    f32 dx = d.x, dz = d.z;
                    cXyz dd;
                    dd.copy(*&d);
                    target = cM_atan2s(dx, dz);
                    dz = dd.z;
                    dx = dd.x;
                    f32 dzz = dz * dz;
                    i_this->mTurnSpeed = 0x800;
                    if (std_sqrtf(gabi::fmadds(dx, dx, dzz)) < 100.0f) {
                        Path* ppd = path(i_this->ppd.get());
                        s8 no = (s8)(i_this->mPathPoint + i_this->mPathDirection);
                        i_this->mPathPoint = no;
                        if (no >= (s8)(u8)ppd->mCountBytes[1]) {
                            if ((u8)ppd->m_closed & 1) {
                                no = 0;
                                i_this->mPathPoint = no;
                            } else {
                                i_this->mPathDirection = -1;
                                no = (s8)((u16)ppd->m_num - 2);
                                i_this->mPathPoint = no;
                            }
                        } else if (no < 0) {
                            i_this->mPathDirection = 1;
                            no = 1;
                            i_this->mPathPoint = no;
                        }
                        cXyz* pnt = &ppd->m_points[no].m_position;
                        i_this->mTargetPos.x = pnt->x;
                        i_this->mTargetPos.y = pnt->y;
                        i_this->mTargetPos.z = pnt->z;
                    }
                } else {
                    target = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
                    cLib_addCalcAngleS2(&i_this->mTurnSpeed, (s16)(REG0_S(4) + 0x400), 1, 0x10);
                }
                if (i_this->mSnapTurn == 1) {
                    i_this->current.angle.y = target;
                } else {
                    cLib_addCalcAngleS2(&i_this->current.angle.y, target, 0x10, i_this->mTurnSpeed);
                }
                (void)turn;
                if (i_this->mType < 10 && i_this->mPlayerOccluded == 0 &&
                    fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0)) < hostIO().mAttackDistance) {
                    /* the player is near: drop to the floor once the body is flat */
                    int k;
                    for (k = 0; k < 8; k++) {
                        if (abs16(i_this->mTrailAngle[k * 8].x) > 0x1000) break;
                    }
                    if (k == 8) {
                        i_this->mStoreTrail = 1;
                    }
                }
            }
            cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 2, 0x400);
        } else {
            /* on a wall or the ceiling */
            if ((hit & 0xC) == 0xC) {
                cXyz d;
                cXyz_mi(&cross.v[2], &d, &cross.v[3]);
                cXyz dd;
                dd.copy(*&d);
                mDoMtx_XrotS(calc_mtx(), (s16)-i_this->current.angle.x);
                mDoMtx_YrotM(calc_mtx(), (s16)-i_this->current.angle.y);
                cXyz t;
                MtxPosition(&dd, &t);
                f32 tz = t.z, tx = t.x;
                i_this->current.angle.z = cM_atan2s(t.y, std_sqrtf(gabi::fmadds(tx, tx, tz * tz)));
            }
            if (i_this->mType < 10) {
                i_this->mTurnSpeed = 0;
            }
        }

        if (i_this->mWavePauseTimer == 0) {
            s16 ph = i_this->mFrameCounter;
            s16 reg = REG0_S(0);
            f32 amp = gabi::fmadds(cM_ssin(ph * 100), 1000.0f, 3500.0f);
            cLib_addCalcAngleS2(&i_this->mHeadWobbleAngle, (s16)gabi::ftoi(cM_ssin(ph * (reg + 2000)) * amp), 4, 0x400);
            if (i_this->mFastTrail != 0) {
                i_this->speedF = 10.0f;
            } else {
                i_this->speedF = 5.0f;
            }
        } else {
            i_this->speedF = 0.0f;
            cLib_addCalc0(&i_this->speedF, 1.0f, 5.0f);
        }
        mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        mDoMtx_YrotM(calc_mtx(), i_this->mHeadWobbleAngle);
        offs.x = 0.0f;
        offs.y = 0.0f;
        offs.z = i_this->speedF;
        MtxPosition(&offs, &i_this->speed);
        fopAcM_posMove(i_this, nullptr);

        if ((hit & 0xF) == 0) {
            s16 c = (s16)(i_this->mWallLostTimer + 1);
            i_this->mWallLostTimer = c;
            if (c >= 10) {
                /* lost the wall: fall and fight */
                i_this->mAction = ACTION_FIGHT;
                i_this->mMode = 0x11;
            }
        } else {
            i_this->mWallLostTimer = 0;
        }
        dBgS_LinChk_dt(&linChk, MT_LINCHK_VTBLS);
    }

    body_control1(i_this);

    if (i_this->mStoreTrail != 0) {
        i_this->mStoreTrail = 0;
        i_this->mAction = ACTION_FIGHT;
        i_this->mMode = 0;
        i_this->mActionTimer = hostIO().mActionDuration;
        i_this->mWavePauseTimer = 0;
        anm_init(i_this, 10, 20.0f, 2, 1.0f, 0);
    }
    cLib_addCalc2(&i_this->mBodyOffset, -10.0f, 1.0f, 1.0f);

    if (gabi::ea(&i_this->mStts) != 0) {
        i_this->current.pos.x = i_this->current.pos.x + i_this->mStts.m_cc_move.x;
        i_this->current.pos.z = i_this->current.pos.z + i_this->mStts.m_cc_move.z;
        for (int i = 0; i < MT_JOINT_NUM; i++) {
            i_this->mTrailPos[i].x = i_this->mTrailPos[i].x + i_this->mStts.m_cc_move.x;
            i_this->mTrailPos[i].z = i_this->mTrailPos[i].z + i_this->mStts.m_cc_move.z;
        }
    }

    if (i_this->mPauseTimer == 0) {
        i_this->mPauseTimer = (s16)gabi::ftoi(cM_rndF(100.0f) + 150.0f);
        if (hostIO().mDisablePauses == 0 && abs16(i_this->current.angle.x) < 0x1000) {
            i_this->mWavePauseTimer = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f + REG0_F(8));
        }
    }
}

/* the fall of mt_fight: integrate the speed */
static inline void mt_fall(Actor* i_this) {
    i_this->current.pos.x = i_this->current.pos.x + i_this->speed.x;
    i_this->current.pos.y = i_this->current.pos.y + i_this->speed.y;
    i_this->current.pos.z = i_this->current.pos.z + i_this->speed.z;
    i_this->mInvulnerableTimer = 5;
    f32 vy = i_this->speed.y + i_this->gravity;
    if (vy < -100.0f) {
        i_this->speed.y = -100.0f;
    } else {
        i_this->speed.y = vy;
    }
}

/* mt_fight (inlined): circle the player, then lunge */
/* inlined into daMt_Execute */
static inline void mt_fight(Actor* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 pitchOfs = 0;

    switch ((u8)i_this->mMode) {
    case 0: {
        mt_all_count() = 0;
        mt_fight_count() = 0;
        fpcEx_Search(MAGTAIL_SEARCH_CALLBACK, i_this);
        FightAngles slotAngle;
        s32 cnt = mt_fight_count();
        if (cnt <= 1) {
            for (int k = 0; k < 5; k++) slotAngle.v[k] = 0;
        } else if (cnt == 2) {
            slotAngle.v[0] = 0x2000;
            slotAngle.v[1] = 0x2000;
            slotAngle.v[2] = 0;
            slotAngle.v[3] = 0;
            slotAngle.v[4] = 0;
        } else if (cnt == 3) {
            slotAngle.v[0] = 0;
            slotAngle.v[1] = -0x3000;
            slotAngle.v[2] = 0x3000;
            slotAngle.v[3] = 0;
            slotAngle.v[4] = 0;
        } else if (cnt == 4) {
            slotAngle.v[0] = 0x2000;
            slotAngle.v[1] = -0x2000;
            slotAngle.v[2] = 0x5000;
            slotAngle.v[3] = -0x5000;
            slotAngle.v[4] = 0;
        } else {
            slotAngle.v[0] = 0;
            slotAngle.v[1] = 0x3000;
            slotAngle.v[2] = -0x3000;
            slotAngle.v[3] = 0x6000;
            slotAngle.v[4] = -0x6000;
        }
        dAttention_c* att = dComIfGp_getAttention();
        if ((dAttention_LockonTruth(att) || ((u32)attention(att)->mFlags & ATTENTION_ACTION_TARGET)) &&
            dAttention_ActionTarget(att, 0) == i_this) {
            fightSlot = 0;
        } else {
            u8 s = (u8)(fightSlot + 1);
            if (s < 5) {
                fightSlot = s;
            } else {
                fightSlot = (u8)(s - 5);
            }
        }

        cXyz d;
        cXyz_mi(&player->current.pos, &d, &i_this->current.pos);
        cXyz v;
        f32 dx = d.x;
        v.x = dx;
        v.z = d.z;
        v.y = d.y + (REG6_F(5) + 90.0f);
        i_this->mPlayerYaw = cM_atan2s(dx, d.z);
        f32 vz = v.z, vx = v.x;
        i_this->mPlayerPitch = (s16)-cM_atan2s(v.y, std_sqrtf(gabi::fmadds(vx, vx, vz * vz)));

        s16 ph = i_this->mFrameCounter;
        cXyz wob;
        wob.x = cM_ssin(ph * 500) * (REG0_F(10) + 30.0f);
        f32 s1400 = cM_ssin(ph * 1400);
        wob.z = gabi::fmadds(cM_ssin(ph * 600), REG0_F(12) + 30.0f, hostIO().mCircleDistance);
        wob.y = gabi::fmadds(s1400, REG0_F(11) + 20.0f, 60.0f);
        pitchOfs = (s16)gabi::ftoi(s1400 * (REG0_F(16) + -2000.0f));
        mDoMtx_YrotS(calc_mtx(), (s16)(player->shape_angle.y + slotAngle.v[fightSlot]));
        MtxPosition(&wob, &v);
        cLib_addCalc2(&i_this->current.pos.x, player->current.pos.x + v.x, 0.1f, REG0_F(10) + 4.0f);
        cLib_addCalc2(&i_this->current.pos.z, player->current.pos.z + v.z, 0.1f, REG0_F(10) + 4.0f);

        f32 dist = fopAcM_searchActorDistance(i_this, dComIfGp_getPlayer(0));
        if ((dist > hostIO().mAttackDistance + 100.0f || i_this->mPlayerOccluded != 0) && i_this->mRollTimer == 0) {
            /* out of reach: back to the floor, then crawl again */
            cLib_addCalc2(&i_this->current.pos.y, i_this->mAcch.m_ground_h + 40.0f, 0.5f, 3.0f);
            f32 y = i_this->current.pos.y - (i_this->mAcch.m_ground_h + 40.0f);
            i_this->mPlayerPitch = 0;
            pitchOfs = 0;
            if (fabsf(y) < 2.0f) {
                i_this->mStoreTrail = 1;
            }
        } else {
            cLib_addCalc2(&i_this->current.pos.y, i_this->mAcch.m_ground_h + hostIO().mHoverHeight + v.y, 0.1f, 5.0f);
            if (i_this->mActionTimer == 0) {
                i_this->mActionTimer = hostIO().mActionDuration;
                if (cM_rndF(1.0f) < hostIO().mAttackChance && i_this->mRollTimer == 0) {
                    i_this->mAttackFrame = 0;
                    i_this->mMode = 1;
                    mt_set_back_pos(i_this, i_this->shape_angle.y, 30.0f, -30.0f);
                    anm_init(i_this, 8, 5.0f, 2, 1.0f, 0);
                    if (gabi::ea(&i_this->eyePos) != 0) {
                        mt_monsSe(i_this, 0x4802);
                    }
                }
            }
        }
        break;
    }
    case 1: {
        /* the lunge */
        if (daPy_procNo(player) == 0x10) {
            dAttention_c* att = dComIfGp_getAttention();
            if ((dAttention_LockonTruth(att) || ((u32)attention(att)->mFlags & ATTENTION_ACTION_TARGET)) &&
                dAttention_ActionTarget(att, 0) == i_this) {
                /* grabbed by the player */
                i_this->mAction = ACTION_CARRIED;
                i_this->mActionTimer = 0;
                anm_init(i_this, 7, 2.0f, 0, 1.0f, 0);
                break;
            }
        }
        i_this->mAttackFrame = i_this->mAttackFrame + 1;
        if (i_this->mAttackFrame >= hostIO().mAttackStartFrame && i_this->mAttackFrame <= hostIO().mAttackEndFrame) {
            i_this->mBtStartFrame = 0.0f;
            i_this->mBtEndFrame = 20.0f;
            i_this->mBtMaxDis = 10000.0f;
            i_this->mBtAttackType = 3;
            i_this->mBtMaxDis = hostIO().mAttackDistanceLimit;
            i_this->mBtNowFrame = 10.0f;
        }
        if (i_this->mAttackFrame == hostIO().mBiteAnimationFrame) {
            anm_init(i_this, 9, 2.0f, 0, 1.0f, 0);
        }
        if (i_this->mAttackFrame == hostIO().mLungeTargetFrame) {
            mDoMtx_YrotS(calc_mtx(), i_this->mPlayerYaw);
            mDoMtx_XrotM(calc_mtx(), i_this->mPlayerPitch);
            cXyz v;
            v.x = 0.0f;
            v.y = REG6_F(6) + -100.0f;
            v.z = REG6_F(7) + 200.0f;
            MtxPosition(&v, &i_this->mTargetPos);
            PSVECAdd(&i_this->mTargetPos, &i_this->current.pos, &i_this->mTargetPos);
        }
        f32 rate, step;
        if (i_this->mAttackFrame >= hostIO().mLungeTargetFrame) {
            step = hostIO().mLungeSpeed;
            rate = 0.5f;
            if (i_this->mAttackFrame == hostIO().mCloseAnimationFrame) {
                anm_init(i_this, 7, 2.0f, 0, 1.0f, 0);
            }
        } else {
            step = REG0_F(11) + 4.0f;
            rate = 0.1f;
        }
        if (i_this->mAttackFrame == (s16)(hostIO().mCloseAnimationFrame + REG0_S(3))) {
            mt_monsSe(i_this, 0x5810);
        }
        if (i_this->mAttackFrame < (s16)(hostIO().mCloseAnimationFrame + REG6_S(7) + 3)) {
            i_this->mHeadColliderMode = 1;
        }
        if (i_this->mAttackFrame == (s16)(hostIO().mCloseAnimationFrame + 2)) {
            i_this->mRubbleFrame = 1;
        }
        if (i_this->mAttackFrame >= (s16)(hostIO().mCloseAnimationFrame + 2) && i_this->mAttackFrame <= (s16)(hostIO().mCloseAnimationFrame + 15)) {
            i_this->mHeadColliderMode = 2;
        }
        cLib_addCalc2(&i_this->current.pos.x, i_this->mTargetPos.x, rate, step);
        cLib_addCalc2(&i_this->current.pos.y, i_this->mTargetPos.y + 20.0f, rate, step);
        cLib_addCalc2(&i_this->current.pos.z, i_this->mTargetPos.z, rate, step);
        i_this->speed.x = i_this->mTargetPos.x - i_this->current.pos.x;
        i_this->speed.y = -1.0f;
        i_this->speed.z = i_this->mTargetPos.z - i_this->current.pos.z;
        if (daPy_checkPlayerGuard(player) && i_this->mSph[0].ChkAtHit()) {
            /* bounced off the shield */
            i_this->mMode = 15;
            mt_set_back_pos(i_this, i_this->shape_angle.y, 80.0f, -120.0f);
            i_this->mShakeTimer = 10;
            i_this->mActionTimer = 10;
            anm_init(i_this, 10, 2.0f, 2, 1.0f, 0);
            i_this->mRubbleFrame = 0;
        }
        if (i_this->mAttackFrame == hostIO().mCircleReturnFrame) {
            i_this->mMode = 0;
            i_this->mActionTimer = hostIO().mActionDuration;
            anm_init(i_this, 10, 5.0f, 2, 1.0f, 0);
        }
        break;
    }
    case 10: {
        f32 vy = i_this->speed.y;
        f32 ny = vy + i_this->gravity;
        i_this->current.pos.y = i_this->current.pos.y + vy;
        if (ny < -100.0f) {
            ny = -100.0f;
        }
        i_this->speed.y = ny;
        if (i_this->mAcch.m_flags & 0x20 /* GROUND_HIT */) {
            i_this->mMode = 0;
        }
        break;
    }
    case 15:
        cLib_addCalc2(&i_this->current.pos.x, i_this->mTargetPos.x, 0.5f, 20.0f);
        cLib_addCalc2(&i_this->current.pos.y, i_this->mTargetPos.y + 20.0f, 0.5f, 20.0f);
        cLib_addCalc2(&i_this->current.pos.z, i_this->mTargetPos.z, 0.5f, 20.0f);
        i_this->speed.x = i_this->mTargetPos.x - i_this->current.pos.x;
        i_this->speed.y = -1.0f;
        i_this->speed.z = i_this->mTargetPos.z - i_this->current.pos.z;
        if (i_this->mActionTimer == 0) {
            i_this->mMode = 0;
            i_this->mActionTimer = hostIO().mActionDuration;
            anm_init(i_this, 10, 5.0f, 2, 1.0f, 0);
        }
        break;
    case 0x11:
        if (i_this->mAcch.m_flags & 0x20 /* GROUND_HIT */) {
            i_this->mMode = 0x17;
            s16 t = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
            i_this->mActionTimer = t;
            i_this->mWavePauseTimer = t;
        }
        mt_fall(i_this);
        break;
    case 0x14:
        /* knocked back */
        i_this->current.angle.x = (s16)(i_this->current.angle.x - 0x500);
        cLib_addCalcAngleS2(&i_this->current.angle.y, (s16)(cM_atan2s(i_this->speed.x, i_this->speed.z) + 0x8000), 2, 0x400);
        cLib_addCalc2(&i_this->mKnockbackRate, 0.9f, 1.0f, 0.1f);
        if (i_this->speed.y > 1.0f) {
            mt_fall(i_this);
        } else if (i_this->mDamageMode != 0) {
            if (i_this->mAcch.m_flags & 0x20 /* GROUND_HIT */) {
                i_this->mDamageMode = 2;
            }
            mt_fall(i_this);
        } else {
            /* roll up */
            i_this->mSph[0].mGObjTg.mSPrm &= ~1u;
            tex_anm_set(i_this, 1);
            f32 sy = i_this->speed.y;
            f32 sz = i_this->speed.z;
            f32 sx = i_this->speed.x;
            i_this->mAction = ACTION_ROLL;
            f32 t = gabi::fmadds(sx, sx, sy * sy);
            i_this->mRollTimer = hostIO().mRollDuration;
            i_this->mRollBodyOffset = 0.0f;
            t = gabi::fmadds(sz, sz, t);
            i_this->mRollPitchStep = 0;
            i_this->mBodyWaveAmplitude = 0.0f;
            i_this->speedF = -std_sqrtf(t);
            i_this->mMode = 0;
            fopAcM_SetMin(i_this, -100.0f, -100.0f, -100.0f);
            fopAcM_SetMax(i_this, 100.0f, 100.0f, 100.0f);
            mt_fall(i_this);
        }
        break;
    case 0x17:
        i_this->speed.y = 0.0f;
        cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 1, 0x400);
        i_this->speed.x = i_this->speed.x * 0.2f;
        i_this->speed.z = i_this->speed.z * 0.2f;
        if (i_this->mActionTimer == 0) {
            i_this->mMode = 10;
        }
        mt_fall(i_this);
        break;
    }

    if (i_this->mAction < 2) {
        if (i_this->mMode < 10) {
            cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mPlayerYaw, 2, 0x400);
            f32 h = i_this->current.pos.y - i_this->mAcch.m_ground_h;
            s16 pitch = i_this->mPlayerPitch;
            if (h > 250.0f) {
                i_this->mMode = 10;
            }
            cLib_addCalcAngleS2(&i_this->current.angle.x, (s16)(pitch + pitchOfs), 4, 0x800);
            cLib_addCalcAngleS2(&i_this->mHeadWobbleAngle, 0, 1, 0x100);
        } else {
            cLib_addCalcAngleS2(&i_this->current.angle.x, 0x3800, 4, 0x800);
            cLib_addCalcAngleS2(&i_this->mHeadWobbleAngle, 0, 1, 0x100);
        }
    }

    /* shake */
    f32 amp = (f32)(s32)i_this->mShakeTimer * (REG0_F(14) + 500.0f);
    i_this->mShakeAngle.y = (s16)gabi::ftoi(sinShort_ool((s16)(i_this->mFrameCounter * 0x2100)) * amp);
    i_this->mShakeAngle.x = (s16)gabi::ftoi(cosShort_ool((s16)(i_this->mFrameCounter * 0x2300)) * amp);
    csXyz_pl(&i_this->shape_angle, &i_this->current.angle, &i_this->mShakeAngle);
    body_control2(i_this);
    body_wall_check(i_this);
    mt_bg_check(i_this);
    mt_cc_move(i_this);
}

/* the line of sight to the player, then the action of the state */
/* damage_check (inlined) */
/* inlined into daMt_Execute */
static inline void damage_check(Actor* i_this) {
    dComIfGp_get(); /* HD: result unused */
    AttackInfo atInfo;
    int hit = 0;
    atInfo.pParticlePos = nullptr;
    i_this->mStts.Move();

    int i = i_this->mHeadColliderMode == 1 ? 2 : 0;
    for (; i < MT_PART_NUM; i++) {
        if (!i_this->mSph[i].ChkTgHit() || i_this->mInvulnerableTimer != 0) {
            continue;
        }
        atInfo.mpObj = gabi::ea(i_this->mSph[i].GetTgHitObj());
        if ((u32)hitObject(atInfo.mpObj)->mAtType & ATTACK_LIGHT_ARROW) {
            i_this->mEnemyIce.mLightShrinkTimer = 1;
            i_this->mDeleteRequested = 1;
            i_this->mEnemyIce.mYOffset = REG0_F(0) + -20.0f;
            i_this->health = 0;
            return;
        }
        at_power_check(&atInfo);
        u8 type = atInfo.mResultingAttackType;
        if (type == 4 || ((u32)hitObject(atInfo.mpObj)->mAtType & ATTACK_ICE_ARROW)) {
            i_this->mHeadHealth = 0;
            i_this->mInvulnerableTimer = 5;
            hit = 2;
            water_damage_se_set(i_this);
            break;
        }
        u8 state = i_this->mAction;
        if (state == 2) {
            if (type == 2) {
                i_this->mDamageMode = 3;
                i_this->mInvulnerableTimer = 5;
                return;
            }
            if (type == 6) {
                i_this->mDamageMode = 2;
                i_this->mInvulnerableTimer = 5;
                return;
            }
        } else {
            if (type == 2) {
                i_this->mInvulnerableTimer = 5;
                mt_knockback(i_this, atInfo.mHitAngle.y);
                i_this->mAction = ACTION_FIGHT;
                i_this->mMaterialMode = 1;
                i_this->mMode = 0x14;
                return;
            }
            if (type == 6) {
                break;
            }
        }
        if (state == 1) {
            i_this->mMode = 15;
            mt_set_back_pos(i_this, i_this->shape_angle.y, 60.0f, -120.0f);
            i_this->mShakeTimer = REG6_S(7) + 6;
            i_this->mActionTimer = REG6_S(8) + 6;
            i_this->mInvulnerableTimer = 5;
            def_se_set(i_this, atInfo.mpObj, 0x40);
            return;
        }
        if (state == 2) {
            i_this->mInvulnerableTimer = REG10_S(0) + 3;
            cc_at_check(i_this, &atInfo);
            if (i_this->health <= 0) {
                i_this->mDamageMode = 2;
            }
            i_this->mHitEffectTimer = 12;
            dComIfGp_particle_set(0x80CF, &i_this->current.pos);
        }
        break;
    }

    if (i_this->mInvulnerableTimer == 0 && i_this->mAtSph.ChkTgHit()) {
        i_this->mInvulnerableTimer = 5;
        atInfo.mpObj = gabi::ea(i_this->mAtSph.GetTgHitObj());
        atInfo.pParticlePos = i_this->mAtSph.GetTgHitPosP();
        if ((u32)hitObject(atInfo.mpObj)->mAtType & ATTACK_LIGHT_ARROW) {
            i_this->mEnemyIce.mLightShrinkTimer = 1;
            return;
        }
        at_power_check(&atInfo);
        i_this->mHeadHealth = (u8)(i_this->mHeadHealth - atInfo.mDamage);
        u8 type = atInfo.mResultingAttackType;
        if (!((type >= 2 && type <= 4) || type == 6) && i_this->mHeadColliderMode != 1) {
            return;
        }
        atInfo.mpActor = cc_at_check(i_this, &atInfo);
        if (atInfo.mResultingAttackType == 6) {
            i_this->mDamageMode = 1;
            hit = 1;
            i_this->mHeadHealth = 0;
            mt_monsSe(i_this, 0x4803 /* JA_SE_CV_MT_DAMAGE? */);
        } else {
            if (atInfo.mResultingAttackType == 4 || ((u32)hitObject(atInfo.mpObj)->mAtType & 0x80000)) {
                i_this->mHeadHealth = 0;
                hit = 2;
                water_damage_se_set(i_this);
            }
            if ((s8)i_this->mHeadHealth > 0) {
                i_this->mMode = 15;
                mt_set_back_pos(i_this, i_this->shape_angle.y, 60.0f, -120.0f);
                i_this->mShakeTimer = 0x19;
                i_this->mActionTimer = 10;
                i_this->current.angle.x = -0x4000;
                if (gabi::ea(&i_this->eyePos) != 0) {
                    mt_monsSe(i_this, 0x4803);
                }
            } else {
                hit = 1;
                mt_monsSe(i_this, 0x4803);
            }
        }
        anm_init(i_this, 10, 2.0f, 2, 1.0f, 0);
        i_this->mRubbleFrame = 0;
    }

    if (hit != 0) {
        mt_knockback(i_this, atInfo.mHitAngle.y);
        if (hit == 2) {
            i_this->speed.y = 0.0f;
        }
        i_this->mMaterialMode = 1;
        i_this->mAction = ACTION_FIGHT;
        i_this->mMode = 0x14;
    }
}

/* body_control1 (inlined into mt_move): the trail, the segment models and colliders */
/* inlined into daMt_Execute */
static inline void mt_action(Actor* i_this, fopAc_ac_c* player) {
    i_this->mHeadColliderMode = 0;
    dBgS_LinChk_l linChk;
    dBgS_LinChk_ct(&linChk, MT_LINCHK_VTBLS);
    cXyz plPos;
    plPos.x = player->current.pos.x;
    plPos.y = player->current.pos.y + 20.0f;
    plPos.z = player->current.pos.z;
    cXyz myPos;
    myPos.x = i_this->current.pos.x;
    myPos.y = i_this->current.pos.y + 30.0f;
    myPos.z = i_this->current.pos.z;
    dBgS_LinChk_Set(&linChk, &myPos, &plPos, i_this);
    i_this->mPlayerOccluded = (u8)cBgS_LineCross(dComIfG_Bgsp(), &linChk);

    switch ((u8)i_this->mAction) {
    case ACTION_MOVE:
        mt_move(i_this);
        break;
    case ACTION_FIGHT:
        mt_fight(i_this);
        break;
    case ACTION_ROLL:
        /* rolled up */
        mt_move_maru(i_this);
        body_control3(i_this);
        if (i_this->mRollTimer > 100) {
            if (i_this->mInLava != 0) {
                i_this->mRollTimer = 0x47;
            }
        } else {
            body_wall_check(i_this);
        }
        mt_cc_move(i_this);
        if (i_this->mRollTimer == 0x32) {
            /* unroll */
            i_this->mMaterialMode = 0;
            i_this->mHeadHealth = 2;
            i_this->mAction = ACTION_FIGHT;
            i_this->mMode = 0;
            i_this->health = 8;
            i_this->mSph[0].mGObjTg.mSPrm |= 1;
            i_this->mActionTimer = hostIO().mActionDuration;
            i_this->mWavePauseTimer = 0;
            anm_init(i_this, 10, 20.0f, 2, 1.0f, 0);
            fopAcM_SetMin(i_this, -200.0f, -200.0f, -200.0f);
            fopAcM_SetMax(i_this, 200.0f, 200.0f, 200.0f);
            i_this->mSph[0].mObjAt.mSPrm = (i_this->mSph[0].mObjAt.mSPrm & ~0xAu) | 4;
        }
        break;
    case ACTION_DESTROYED:
        /* destroyed */
        i_this->mInvulnerableTimer = 5;
        attention_flags_set(i_this, 0);
        body_control4(i_this);
        if (i_this->mDeleteTimer == 1) {
            i_this->mDeleteRequested = 1;
            fopAcM_delete(i_this);
            dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
        }
        break;
    case ACTION_CARRIED:
        /* carried by the player */
        if (daPy_procNo(player) == 0x10) {
            mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
            cXyz v;
            v.x = 0.0f;
            v.y = 0.0f;
            v.z = REG0_F(14) + -50.0f;
            cXyz t;
            MtxPosition(&v, &t);
            cLib_addCalc2(&i_this->current.pos.x, player->current.pos.x + t.x, 0.5f, 50.0f);
            cLib_addCalc2(&i_this->current.pos.z, player->current.pos.z + t.z, 0.5f, 50.0f);
        }
        i_this->mMaterialMode = 1;
        i_this->mBlinkTimer = 3;
        i_this->mInvulnerableTimer = 5;
        body_control5(i_this);
        {
            f32 vy = i_this->speed.y;
            i_this->current.pos.y = i_this->current.pos.y + vy;
            i_this->speed.y = vy + i_this->gravity;
        }
        mt_bg_check(i_this);
        if (i_this->mActionTimer != 0) {
            i_this->mJointWavePhase = 0;
            cLib_addCalc0(&i_this->mWobbleAmplitude, 0.05f, REG0_F(12) + 0.02f);
            if (i_this->mActionTimer == 1) {
                bakuha(i_this);
            }
        } else if (daPy_procNo(player) != 0x10) {
            /* released */
            i_this->mMaterialMode = 0;
            i_this->mAction = ACTION_FIGHT;
            i_this->mMode = 0;
            i_this->mSph[0].mGObjTg.mSPrm |= 1;
            i_this->mActionTimer = hostIO().mActionDuration;
            i_this->mWavePauseTimer = 0;
            anm_init(i_this, 10, 20.0f, 2, 1.0f, 0);
            i_this->mSph[0].mObjAt.mSPrm = (i_this->mSph[0].mObjAt.mSPrm & ~0xAu) | 4;
        } else {
            i_this->mWobbleAmplitude = REG0_F(13) + 0.2f;
            if (i_this->mAtSph.ChkTgHit()) {
                i_this->health = 0;
                i_this->mActionTimer = REG0_S(3) + 0x28;
                i_this->mWobbleAmplitude = REG0_F(13) + 1.5f;
                mt_fopAcM_seStart(i_this, 0x2828, 0);
                mt_fopAcM_monsSeStart(i_this, 0x4803, 0);
                AttackInfo atInfo;
                atInfo.mHitSoundId = 0;
                atInfo.mpObj = gabi::ea(i_this->mAtSph.GetTgHitObj());
                at_power_check(&atInfo);
                mDoAud_onEnemyDamage();
                mDoAud_bgmHitSound(atInfo.mHitSoundId);
                enemyHitEffectTimer = (u8)(REG0_S(7) + 6);
                dComIfGp_particle_set(0x10, &i_this->current.pos);
                csXyz ang;
                cXyz scale;
                ang.x = 0;
                ang.z = 0;
                scale.z = 2.0f;
                scale.x = 2.0f;
                scale.y = 2.0f;
                ang.y = fopAcM_searchActorAngleY(i_this, dComIfGp_getPlayer(0));
                dComIfGp_particle_set(0xD, &i_this->current.pos, &ang, &scale);
                cXyz pos;
                pos.x = i_this->current.pos.x;
                pos.y = i_this->current.pos.y;
                pos.z = i_this->current.pos.z;
                dKy_SordFlush_set(&pos, 1);
                anm_init(i_this, 8, 5.0f, 0, 1.0f, 0);
            }
        }
        break;
    }

    if (i_this->mRollTimer > 60) {
        cLib_addCalc0(&i_this->mHeadScale, 1.0f, 0.05f);
    } else {
        cLib_addCalc2(&i_this->mHeadScale, 1.0f, 1.0f, 0.1f);
    }
    dBgS_LinChk_dt(&linChk, MT_LINCHK_VTBLS);
}

/* keep the head out of the background between the old and the new position */
/* inlined into daMt_Execute */
static inline void mt_bg_line_check(Actor* i_this) {
    f32 h = 40.0f;
    dBgS_LinChk_l chkUp;
    dBgS_ObjLinChk_ct(&chkUp);
    cXyz start;
    start.copy(i_this->old.pos);
    cXyz end;
    end.x = i_this->old.pos.x;
    end.y = i_this->old.pos.y + 40.0f;
    end.z = i_this->old.pos.z;
    dBgS_LinChk_Set(&chkUp, &start, &end, i_this);
    BOOL roof = cBgS_LineCross(dComIfG_Bgsp(), &chkUp);
    f32 oy = i_this->old.pos.y;
    if (roof) {
        f32 t = LinChk_GetCross(&chkUp)->y - 1.0f - oy;
        h = t >= 0.0f ? t : 0.0f;
    }
    cXyz oldPos;
    oldPos.x = i_this->old.pos.x;
    oldPos.y = oy + h;
    oldPos.z = i_this->old.pos.z;
    cXyz newPos;
    newPos.x = i_this->current.pos.x;
    newPos.y = i_this->current.pos.y + h;
    newPos.z = i_this->current.pos.z;

    dBgS_LinChk_l chkFwd;
    dBgS_ObjLinChk_ct(&chkFwd);
    dBgS_LinChk_Set(&chkFwd, &oldPos, &newPos, i_this);
    BOOL fwd = cBgS_LineCross(dComIfG_Bgsp(), &chkFwd);
    dBgS_LinChk_l chkBack;
    dBgS_ObjLinChk_ct(&chkBack);
    dBgS_LinChk_Set(&chkBack, &newPos, &oldPos, i_this);
    BOOL back = cBgS_LineCross(dComIfG_Bgsp(), &chkBack);
    if (fwd && !back) {
        /* went through a wall: back to the wall, pushed out along its normal */
        i_this->current.pos.copy(*LinChk_GetCross(&chkFwd));
        cBgS_PolyInfo* poly = linePoly(&chkFwd);
        cXyz* pla = (cXyz*)cBgS_GetTriPla(dComIfG_Bgsp(), poly->mBgIndex, poly->mPolyIndex);
        if (pla != nullptr) {
            i_this->current.pos.x = i_this->current.pos.x + pla->x;
            i_this->current.pos.y = i_this->current.pos.y + pla->y;
            i_this->current.pos.z = i_this->current.pos.z + pla->z;
        }
    }
    dBgS_LinChk_dt(&chkBack, MT_LINCHK_VTBLS);
    dBgS_LinChk_dt(&chkFwd, MT_LINCHK_VTBLS);
    dBgS_LinChk_dt(&chkUp, MT_LINCHK_VTBLS);
}

/* 021D87A0 */
BOOL daMt_Execute(mt_class* actor) {
    Actor* i_this = reinterpret_cast<Actor*>(actor);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);

    if (i_this->mDeathSwitch != 0 && i_this->current.pos.y < i_this->home.pos.y - 5000.0f) {
        /* fell out of the room: counts as defeated */
        dComIfGs_onSwitch(i_this->mDeathSwitch, i_this->current.roomNo);
        dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
        fopAcM_delete(i_this);
        return TRUE;
    }
    if (enemy_ice(&i_this->mEnemyIce)) {
        return TRUE;
    }
    if (i_this->mAppearSwitch != 0) {
        if (!dComIfGs_isSwitch(i_this->mAppearSwitch - 1, i_this->current.roomNo)) {
            return TRUE;
        }
        i_this->mAppearSwitch = 0;
    }
    i_this->actor_status |= 0x20;

    /* lava */
    LavaGroundCheck gndChk;
    dBgS_ObjGndChk_Yogan_ct(&gndChk);
    f32 y = i_this->current.pos.y;
    f32 z = i_this->current.pos.z;
    f32 x = i_this->current.pos.x;
    gndChk.m_pos.x = x;
    gndChk.m_pos.y = y + 200.0f;
    gndChk.m_pos.z = z;
    f32 lava = cBgS_GroundCross(dComIfG_Bgsp(), &gndChk);
    if (lava != -1000000000.0f && i_this->current.pos.y - 30.0f + REG0_F(13) < lava) {
        if (i_this->mInLava == 0) {
            i_this->speedF = i_this->speedF * 0.1f;
            i_this->speed.y = 0.0f;
            cXyz pos;
            pos.x = x;
            pos.z = z;
            pos.y = lava;
            fopKyM_createMpillar(&pos, 0.5f);
        }
        i_this->mInLava = 1;
        i_this->gravity = -0.5f;
        if (i_this->speed.y < -5.0f) {
            i_this->speed.y = -5.0f;
        }
    } else {
        i_this->mInLava = 0;
        i_this->gravity = -3.0f;
    }
    attention_flags_set(i_this, 4);

    mt_eye_tex_anm(i_this);

    if (hostIO().mNo == 0) {
        if (i_this->mRubbleFrame != 0) {
            u8 n = (u8)(i_this->mRubbleFrame + 1);
            if (n == 11) {
                i_this->mRubbleFrame = 0;
            } else {
                i_this->mRubbleFrame = n;
            }
        }
        i_this->mBtStartFrame = 100.0f;
        i_this->mBtAttackType = 0;
        i_this->mBtEndFrame = 100.0f;
        i_this->mBtMaxDis = 10000.0f;
        i_this->mBtNowFrame = 0.0f;
        for (int i = 0; i < 5; i++) {
            be<s16>* t = &(&i_this->mActionTimer)[i];
            if (*t != 0) {
                *t = *t - 1;
            }
        }
        if (i_this->mHitEffectTimer != 0) i_this->mHitEffectTimer = i_this->mHitEffectTimer - 1;
        if (i_this->mInvulnerableTimer != 0) i_this->mInvulnerableTimer = i_this->mInvulnerableTimer - 1;
        if (i_this->mShakeTimer != 0) i_this->mShakeTimer = i_this->mShakeTimer - 1;
        if (i_this->mSteamTimer != 0) i_this->mSteamTimer = i_this->mSteamTimer - 1;
        if (i_this->mWavePauseTimer != 0) {
            i_this->mWavePauseTimer = i_this->mWavePauseTimer - 1;
            i_this->mWaveExtent = i_this->mWaveExtent + 1;
        } else if (i_this->mWaveExtent != 0) {
            s16 n = i_this->mWaveExtent;
            if (n > 15) n = 15;
            i_this->mWaveExtent = n - 1;
        }
        i_this->mFrameCounter = i_this->mFrameCounter + 1;
        if (i_this->mRollTimer != 0) i_this->mRollTimer = i_this->mRollTimer - 1;

        if (i_this->mAction < 3) {
            damage_check(i_this);
        }
        mt_action(i_this, player);
    }

    mt_bg_line_check(i_this);
    attention_pos(i_this)->copy(i_this->eyePos);

    s32 step = 1, lo, hi, bhi;
    switch ((u8)i_this->mMaterialMode) {
    default:
        JUT_ASSERT_fail("d_a_mt.cpp", 0x108B, "0");
        /* fall through */
    case 0:
        lo = 0;
        hi = 40;
        bhi = 30;
        break;
    case 1:
        step = 2;
        lo = 40;
        hi = 100;
        bhi = 90;
        break;
    case 2:
        lo = 100;
        hi = 130;
        bhi = 120;
        break;
    }
    i_this->mBrkFrame = i_this->mBrkFrame + step;
    if (i_this->mBrkFrame > hi) {
        i_this->mBrkFrame = i_this->mMaterialMode != 0 ? hi : lo;
    }
    i_this->mBtkFrame = i_this->mBtkFrame + step;
    if (i_this->mBtkFrame > bhi) {
        i_this->mBtkFrame = i_this->mMaterialMode != 0 ? bhi : 0;
    }

    /* joint waves */
    csXyz* jnt = &i_this->mJointAnglesLeft[0]; /* mJointAnglesLeft[15] and mJointAnglesRight[15] */
    if (i_this->mAction >= 2 && i_this->mRollTimer > 120) {
        i_this->mJointWaveStep = 0;
        for (int i = 0; i < 30; i++) {
            jnt[i].x = 0;
            cLib_addCalcAngleS2(&jnt[i].z, 10000, 10, 300);
        }
    } else {
        if (i_this->mRollTimer == 120) {
            i_this->mWaveExtent = 15;
        }
        i_this->mpMorf[0]->play(&i_this->current.pos, 0, 0);
        i_this->mJointWavePhase = i_this->mJointWavePhase + hostIO().mJointWaveSpeed;
        for (int i = 0; i < 30; i++) {
            int j = i >= 15 ? i - 15 : i;
            bool still = i_this->mWavePauseTimer != 0 ? j < i_this->mWaveExtent : 14 - j < i_this->mWaveExtent;
            if (still) {
                jnt[i].x = (s16)gabi::ftoi(-(cM_ssin(i * (REG6_S(2) + 0x32C8)) * (REG6_F(11) + 5000.0f)));
                f32 c = cM_scos(i * (REG6_S(3) + 0x32C8));
                s16 t = (s16)gabi::ftoi(gabi::fmadds(c, REG6_F(12) + 5000.0f, hostIO().mJointBendBase));
                cLib_addCalcAngleS2(&jnt[i].z, t, 1, i_this->mJointWaveStep);
            } else {
                s16 ph = i_this->mJointWavePhase;
                jnt[i].x = (s16)gabi::ftoi(-(cM_ssin(ph + i * (REG6_S(2) + 0x32C8)) * (REG6_F(11) + 5000.0f)));
                f32 c = cM_scos(ph + i * (REG6_S(3) + 0x32C8));
                s16 t = (s16)gabi::ftoi(gabi::fmadds(c, REG6_F(12) + 5000.0f, hostIO().mJointBendBase));
                cLib_addCalcAngleS2(&jnt[i].z, t, 1, i_this->mJointWaveStep);
            }
        }
        cLib_addCalcAngleS2(&i_this->mJointWaveStep, 0x2000, 1, 0x100);
        for (int k = 0; k < MT_PART_NUM; k++) {
            f32 s = cM_ssin(i_this->mFrameCounter * (REG0_S(5) + 0x9C4) + k * (REG0_S(6) + 0x1D4C));
            i_this->mSegmentScaleY[k] = gabi::fmadds(s, i_this->m454, 1.0f);
        }
        cLib_addCalc2(&i_this->m454, 0.1f, 1.0f, 0.002f);
    }

    if (!(i_this->mType & 1)) {
        /* lava drips and glow along the body */
        for (int i = 0; i < MT_PART_NUM; i++) {
            J3DModel* model = i_this->mpMorf[i]->getModel();
            cXyz v;
            v.y = REG0_F(6);
            v.z = REG0_F(7);
            v.x = REG0_F(5);
            cXyz& pos = i_this->mParticlePos[i];
            PSMTXMultVec(J3DModel_getBaseTRMtx(model), &v, &pos);
            u8 st = i_this->mAction;
            if ((st >= 2 && (i_this->mRollTimer > 40 || st == 6)) || hostIO().mHideParticles != 0) {
                pos.y = pos.y + 10000.0f;
            }
            if (i < 7) {
                dComIfGp_particle_setSimple(0x8058, &pos);
            }
            if ((u32)(i - 1) < 6) {
                dComIfGp_particle_setSimple(0x8059, &pos);
            }
            if (i_this->mFollowParticlesStarted == 0 && (i == 1 || i == 3 || i == 5)) {
                dPa_control_set(dComIfGp_getParticle(), 4, 0xC06C, &pos, nullptr, nullptr, 0xFF,
                                (dPa_levelEcallBack*)&i_this->mFollowCb[i], -1, nullptr, nullptr, nullptr);
            }
        }
        i_this->mFollowParticlesStarted = 1;
    }

    if (i_this->mAction < 2 && i_this->mInLava == 0 && i_this->mWavePauseTimer == 0) {
        if (i_this->mVoiceTimer == 0) {
            i_this->mVoiceTimer = (s16)gabi::ftoi(cM_rndF(45.0f) + 45.0f);
            mt_monsSe(i_this, 0x4801);
        }
        if (i_this->mMoveSoundTimer == 0) {
            i_this->mMoveSoundTimer = (s16)gabi::ftoi(cM_rndF(3.0f) + 6.0f);
            mt_monsSe(i_this, 0x580F);
        }
    }
    if (i_this->mDamageMode >= 2) {
        bakuha(i_this);
        i_this->mDamageMode = -1;
    }
    dBgS_ObjGndChk_Yogan_dt(&gndChk);
    return TRUE;
}

/* WWHD glue
str 10015740
str 1001573C
data moveCheckX 101BAE74
data moveCheckY 101BAE8C
data moveCheckZ 101BAEA4
data moveCheckBits 101BADC8
data trailOffsets 101BAE14
data fastTrailOffsets 101BAE34
data fightSlot 101BADD0
data enemyHitEffectTimer 101EACB7
ext 02518DB0 void mtExec_atPower(AttackInfo* info)
ext 025192A8 u32 mtExec_atCheck(fopAc_ac_c* actor, AttackInfo* info)
ext 02518CC8 void mtExec_defenceSound(fopAc_ac_c* actor, u32 obj, u32 material)
ext 020402C8 BOOL mtExec_enemyIce(enemyice_l* ice)
ext 025DAF3C void mtExec_createMpillar(cXyz* pos, f32 scale)
ext 025F18EC void mtExec_XrotS(Mtx34* mtx, s16 angle)
ext 025DE508 void* mtExec_search(u32 callback, void* data)
ext 024EDFCC BOOL mtExec_lockonTruth(dAttention_c* attention)
ext 024EC8D0 fopAc_ac_c* mtExec_actionTarget(dAttention_c* attention, s32 index)
ext 025E1FD8 void mtExec_onEnemyDamage()
ext 025E1D30 void mtExec_bgmHitSound(s32 id)
ext 0255F554 void mtExec_swordFlush(cXyz* pos, s32 mode)
ext 0201A4DC void mtExec_addAngles(const csXyz* a, const csXyz* b)
ext 021DD620 f32 mtExec_sinShort(void* table, s16 angle)
ext 021DFDDC f32 mtExec_cosShort(void* table, s16 angle)
ext 021DD518 void mtExec_seStart(fopAc_ac_c* actor, u32 id, u32 param)
ext 021DD584 void mtExec_monsSeStart(fopAc_ac_c* actor, u32 id, u32 param)
ext 02008E0C void mtExec_groundCtor(LavaGroundCheck* check)
ext 02008DAC void mtExec_groundDtor(LavaGroundCheck* check, s32 flag)
ext 025E1AA4 void mtExec_monsSe(u32 id, cXyz* pos, u32 pid, s32 mode, s32 reverb)
*/
