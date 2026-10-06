/**
 * d_a_fgmahou.cpp (WWHD)
 * Phantom Ganon's energy ball (one of 8 orbs): flies at the player, can be deflected back.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fgmahou.cpp, USA version) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define FGMAHOU_VTBL 0x1000F058    /* fgmahou_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x1000F010 /* this TU's sead::SafeString vtable */
#define ARC_HEAP STR(0x1000F0A4)   /* "Fganon" (useHeapInit) */
#define ARC_DELETE STR(0x1000F098) /* "Fganon" (Delete) */
#define ARC_CREATE STR(0x1000F0B0) /* "Fganon" (Create) */
#define tg_sph_src gabi::at<dCcD_SrcSph>(0x101B4E70)
#define at_sph_src gabi::at<dCcD_SrcSph>(0x101B4EB0)
/* move(): static f32 spdd[8], static s16 angXd[8] (.data) */
static inline f32 spdd(u32 i) { return gabi::load<f32>(0x101B4E40 + 4 * i); }
static inline s16 angXd(u32 i) { return gabi::load<s16>(0x101B4E60 + 2 * i); }

enum {
    dRes_INDEX_FGANON_BDL_YDKSP00_e = 0x1C,
    dRes_INDEX_FGANON_BRK_YDKSP00_e = 0x27,
    dRes_INDEX_FGANON_BTK_YDKSP00_e = 0x2B,
};
enum { YDKSP00_JNT_HEAD_e = 1 };
enum {
    ID_AK_SN_BPGDARKSHOT00 = 0x821E,
    ID_AK_SN_BPGSMASHDARKSHOT00 = 0x8244,
    ID_AK_SN_BPGHITDARKSHOT00 = 0x8245,
    ID_AK_SN_BPGHITDARKSHOT01 = 0x8246,
};
enum { JA_SE_LK_PG_BOMB_STRIKE = 0x28A5, JA_SE_OBJ_PG_EBALL_EXP_L = 0x6A36 };
enum { fpcNm_FGANON_e = 0xF1, fpcNm_HD_A8 = 0xA8 };

#define REG6_F(i) REG_F(6, i)
#define REG6_S(i) REG_S(6, i)
#define REG8_S(i) REG_S(8, i)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02516178 dCcD_GObjInf::GetAtHitObj */
static inline void* dCcD_GObjInf_GetAtHitObj(dCcD_GObjInf* o) { return gabi::call<void*>(0x02516178, o); }
/* 028249B0 JPASetRMtxTVecfromMtx(const Mtx, Mtx (rotation), TVec3 (translation)) */
static inline void JPASetRMtxTVecfromMtx(u32 mtx, u32 r, u32 t) { gabi::call(0x028249B0, mtx, r, t); }
/* JPABaseEmitter::setGlobalRTMatrix (HD inline): rotation at +0x1F0, translation at +0x22C */
static inline void JPABaseEmitter_setGlobalRTMatrix(JPABaseEmitter* e, u32 mtx) {
    JPASetRMtxTVecfromMtx(mtx, gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C);
}
/* JPABaseEmitter::becomeInvalidEmitter (HD inline): +0x5C = -1, flags +0x254 |= 1 */
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 p = gabi::ea(e);
    u32 f = gabi::load<u32>(p + 0x254);
    gabi::store<s32>(p + 0x5C, -1);
    gabi::store<u32>(p + 0x254, f | 1);
}
/* J3DModel::getAnmMtx (HD): joint matrix block at +0x2C (flags +4 |= 0x10, matrices at +0x10) */
static inline u32 J3DModel_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    u16 flags = gabi::load<u16>(blk + 4);
    u32 mtx = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, flags | 0x10);
    return mtx + jnt * 0x30;
}
/* fopAcM_seStartCurrent (HD inline): null checks on the actor and &current */
static inline void fopAcM_seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(a->current.roomNo));
}
/* the same inline where the actor is known non-null: only &current is checked */
static inline void fopAcM_seStartCurrent_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(a->current.roomNo));
}
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0 (matcher: "init"), init 025E8154 */
static inline mDoExt_brkAnm* mDoExt_brkAnm_ct(void* p) { return gabi::call<mDoExt_brkAnm*>(0x025E80D0, p); }
static inline BOOL mDoExt_brkAnm_init(mDoExt_brkAnm* a, J3DModelData* d, J3DAnmTevRegKey* k, bool play, s32 mode, f32 rate,
                                      s16 start, s16 end, bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, k, play, mode, rate, start, end, modify, entry);
}
static inline J3DFrameCtrl* anm_frameCtrl(void* anm) { return gabi::at<J3DFrameCtrl>(gabi::ea(anm)); }
/* dCcD_GObjInf::OnTgNoHitMark: mGObjTg.mSPrm |= 4 */
static inline void OnTgNoHitMark(dCcD_GObjInf* o) { o->mGObjTg.mSPrm |= 4; }

struct fgmahou_class : fopAc_ac_c {
    /* 0x3AC */ u8 field_0x290[0x3C8 - 0x3AC];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ be<u8> mOrbNumber;
    /* 0x3D1 */ u8 _3D1[3];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ gptr<mDoExt_btkAnm> mpBtk;
    /* 0x3DC */ gptr<mDoExt_brkAnm> mpBrk;
    /* 0x3E0 */ be<s16> mAge;
    /* 0x3E2 */ be<s16> mState;
    /* 0x3E4 */ cXyz mTargetPos;
    /* 0x3F0 */ be<s16> field_0x2D4;
    /* 0x3F2 */ be<s16> field_0x2D6;
    /* 0x3F4 */ be<s16> field_0x2D8;
    /* 0x3F6 */ be<s16> mTimers[2];
    /* 0x3FA */ be<s16> field_0x2DE;
    /* 0x3FC */ be<s16> field_0x2E0;
    /* 0x3FE */ be<s16> field_0x2E2;
    /* 0x400 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x404 */ dBgS_AcchCir mCir;
    /* 0x444 */ dBgS_ObjAcch mAcch;
    /* 0x608 */ dCcD_Stts mStts;
    /* 0x644 */ dCcD_Sph mAtSph;
    /* 0x770 */ dCcD_Sph mTgSph;
    /* 0x89C */ be<s8> field_0x780;
    /* 0x89D */ u8 _89D[3];
};
WWHD_OFFSET(fgmahou_class, mPhs, 0x3C8);
WWHD_OFFSET(fgmahou_class, mpEmitter, 0x400);
WWHD_OFFSET(fgmahou_class, mAcch, 0x444);
WWHD_OFFSET(fgmahou_class, mTgSph, 0x770);
WWHD_OFFSET(fgmahou_class, field_0x780, 0x89C);
WWHD_SIZE(fgmahou_class, 0x8A0);

/* 0213D4E0 */
static BOOL daFgmahou_Draw(fgmahou_class* i_this) {
    WWHD_FUNC(0x0213D4E0, BOOL, i_this);
    J3DModel* pModel = i_this->mpMorf->getModel();
    mDoExt_brkAnm* brk = i_this->mpBrk;
    mDoExt_brkAnm_entry(brk, J3DModel_getModelData(pModel), anm_frameCtrl(brk)->getFrame());
    mDoExt_btkAnm* btk = i_this->mpBtk;
    btk->entry(J3DModel_getModelData(pModel), btk->getFrame());
    i_this->mpMorf->entryDL();
    return TRUE;
}
VERIFY(0x0213D4E0, daFgmahou_Draw);

/* 0213D544 */
static void* boss_s_sub(void* param_1, void* param_2) {
    WWHD_FUNC(0x0213D544, void*, param_1, param_2);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_FGANON_e) {
        return param_1;
    }
    return nullptr;
}
VERIFY(0x0213D544, boss_s_sub);

/* |mTargetPos - current.pos| (cXyz::abs: the difference is copied, then sqrtf(PSVECSquareMag)) */
static inline f32 target_dist(fgmahou_class* i_this) {
    gabi::Local<cXyz> tmp;
    cXyz_mi(&i_this->mTargetPos, tmp, &i_this->current.pos);
    gabi::Local<cXyz> diff3;
    diff3->copy(*tmp);
    return std_sqrtf(PSVECSquareMag(diff3));
}

/* turn home.angle towards mTargetPos */
static inline void aim_at_target(fgmahou_class* i_this) {
    gabi::Local<cXyz> tmp;
    cXyz_mi(&i_this->mTargetPos, tmp, &i_this->current.pos);
    gabi::Local<cXyz> diff2;
    diff2->x = tmp->x;
    diff2->y = tmp->y;
    diff2->z = tmp->z;
    s16 ay = cM_atan2s(diff2->x, diff2->z);
    cLib_addCalcAngleS2(&i_this->home.angle.y, ay, 2, (s16)(REG8_S(4) + 0x500));
    f32 z = diff2->z;
    f32 x = diff2->x;
    f32 xz = std_sqrtf(gabi::fmadds(x, x, z * z));
    s16 ax = cM_atan2s(diff2->y, xz);
    cLib_addCalcAngleS2(&i_this->home.angle.x, (s16)-ax, 2, (s16)(REG8_S(4) + 0x500));
}

/* move(): inlined into daFgmahou_Execute */
static inline void move(fgmahou_class* i_this) {
    fopAc_ac_c* pPlayer = dComIfGp_getPlayer(0);
    int temp2 = 1;
    f32 temp3 = 0.0f;

    i_this->mStts.Move();

    switch (i_this->mState) {
    case 0: {
        i_this->mState = 1;
        i_this->field_0x2D4 = (s16)(i_this->mOrbNumber * REG6_S(2) - REG6_S(3));
        i_this->field_0x2D6 = (s16)gabi::ftoi(cM_rndF(65536.0f));

        f32 spd = REG0_F(0xD) + 50.0f;
        i_this->speedF = spd;
        f32 k = REG0_F(4) + 0.2f;
        i_this->speedF = spd * gabi::fmadds(spdd(i_this->mOrbNumber), k, 1.0f);
        i_this->mTargetPos.copy(pPlayer->eyePos);

        {
            gabi::Local<cXyz> tmp;
            cXyz_mi(&i_this->mTargetPos, tmp, &i_this->current.pos);
            gabi::Local<cXyz> diff2;
            diff2->x = tmp->x;
            diff2->z = tmp->z;
            diff2->y = tmp->y;
            s16 a = cM_atan2s(diff2->x, diff2->z);
            u8 orb = i_this->mOrbNumber;
            s16 y = (s16)(orb * (REG8_S(2) + 0x1400) - (REG8_S(3) + 0x4000) + a);
            s16 z = i_this->home.angle.z;
            i_this->home.angle.y = y;
            s16 x = angXd(orb);
            i_this->shape_angle.y = y;
            i_this->shape_angle.x = x;
            i_this->home.angle.x = x;
            i_this->shape_angle.z = z;
        }
        i_this->mTimers[1] = 0x14;
        i_this->mTimers[0] = (s16)(REG8_S(9) + 5);

        i_this->mpEmitter = dComIfGp_particle_set(ID_AK_SN_BPGDARKSHOT00, &i_this->current.pos);
        i_this->mTgSph.SetR(REG6_F(5) + 110.0f);
        temp2 = REG0_S(2) + 2;
    }
        /* fallthrough */
    case 1:
    case 2:
        temp3 = target_dist(i_this);
        if (i_this->mTimers[0] != 0) {
            break;
        }
        if (temp3 > REG0_F(0x11) + 500.0f && i_this->mState < 2) {
            aim_at_target(i_this);
        } else {
            i_this->mState = 2;
        }

        /* HD: the ball only reports a hit to Phantom Ganon when the At hit object's actor is
         * process 0xA8 (GameCube: any At hit) */
        if (i_this->mAtSph.ChkAtHit()) {
            void* obj = dCcD_GObjInf_GetAtHitObj(&i_this->mAtSph);
            if (obj != nullptr) {
                u32 stts = gabi::load<u32>(gabi::ea(obj) + 0x44);
                if (stts != 0) {
                    u32 ac = gabi::load<u32>(stts + 0xC);
                    if (ac != 0 && gabi::load<s16>(ac + 8) == fpcNm_HD_A8) {
                        void* fganon = fpcM_Search(0x0213D544 /* boss_s_sub */, i_this);
                        if (fganon != nullptr) {
                            gabi::store<u8>(gabi::ea(fganon) + 0x8BF, 1); /* fganon->m68B */
                            break;
                        }
                    }
                }
            }
        }

        if (!i_this->mTgSph.ChkTgHit()) {
            break;
        }

        dComIfGp_particle_set(ID_AK_SN_BPGSMASHDARKSHOT00, &i_this->current.pos, &i_this->home.angle);
        fopAcM_seStartCurrent(i_this, JA_SE_LK_PG_BOMB_STRIKE, 0);
        i_this->mState = 5;
        /* fallthrough */
    case 5: {
        fopAc_ac_c* fganon2 = (fopAc_ac_c*)fpcM_Search(0x0213D544 /* boss_s_sub */, i_this);
        if (fganon2 == nullptr) {
            i_this->field_0x780 = 0x32;
            break;
        }
        /* HD: the target's y is kept in a register across cM_rndFX */
        f32 ex = fganon2->eyePos.x;
        i_this->mTargetPos.x = ex;
        f32 ey = fganon2->eyePos.y;
        i_this->mTargetPos.y = ey;
        f32 ez = fganon2->eyePos.z;
        i_this->mTargetPos.z = ez;
        f32 r = cM_rndFX(50.0f) + 50.0f;
        s16 hy = i_this->home.angle.y;
        s16 hx = i_this->home.angle.x;
        i_this->home.angle.y = (s16)(hy - 0x8000);
        i_this->home.angle.x = (s16)-hx;
        i_this->mTargetPos.y = ey - r;
        i_this->mTimers[0] = (s16)(REG8_S(9) + 5);
        i_this->field_0x2E0 = 1;
        i_this->shape_angle.x = (s16)i_this->home.angle.x;
        i_this->shape_angle.y = (s16)i_this->home.angle.y;
        i_this->shape_angle.z = (s16)i_this->home.angle.z;
        i_this->field_0x2E2 = (s16)-0x8000;
        i_this->mState = 6;
        i_this->mAtSph.SetR(60.0f);
    }
        /* fallthrough */
    case 6:
    case 7:
        temp3 = target_dist(i_this);
        if (i_this->mTimers[0] != 0) {
            break;
        }
        if (temp3 > REG0_F(0x11) + 500.0f && i_this->mState < 7) {
            aim_at_target(i_this);
        } else {
            i_this->mState = 7;
        }
        break;
    case 10:
        if (i_this->mTimers[0] == 0) {
            fopAcM_delete(i_this);
        }
        return;
    default: /* 3, 4, 8, 9 */
        break;
    }

    f32 temp4 = temp3 * (REG0_F(3) + 3.0f);
    f32 lim = REG0_F(8) + 8000.0f;
    if (temp4 > lim) {
        temp4 = lim;
    }

    s16 a1 = (s16)(i_this->field_0x2D4 + (s16)(REG0_S(6) + 0xDAC));
    i_this->field_0x2D4 = a1;
    s16 a2 = (s16)(i_this->field_0x2D6 + (s16)(REG0_S(7) + 0xCE4));
    i_this->field_0x2D6 = a2;

    s16 temp5 = (s16)gabi::ftoi(cM_ssin(a1) * temp4);
    s16 temp7 = (s16)gabi::ftoi(cM_ssin(a2) * temp4 * 0.75f);
    s16 sy = (s16)(i_this->home.angle.y + temp5);
    s16 sx = (s16)(i_this->home.angle.x + temp7);
    i_this->shape_angle.y = sy;
    i_this->shape_angle.x = sx;

    {
        gabi::Local<cXyz> diff2;
        diff2->x = 0.0f;
        diff2->y = 0.0f;
        diff2->z = i_this->speedF;
        cMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
        cMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
        MtxPosition(diff2, &i_this->speed);
    }

    if (temp2 <= 1) {
        PSVECAdd(&i_this->current.pos, &i_this->speed, &i_this->current.pos);
    } else {
        for (int i = 0; i < temp2; i++) {
            PSVECAdd(&i_this->current.pos, &i_this->speed, &i_this->current.pos);
        }
    }

    gabi::Local<cXyz> temp6;
    temp6->x = i_this->current.pos.x;
    temp6->y = i_this->current.pos.y;
    temp6->z = i_this->current.pos.z;

    if (i_this->mTimers[1] == 0) {
        i_this->mTgSph.SetC(temp6);
        dComIfG_Ccsp_Set(&i_this->mTgSph);
        i_this->mAtSph.SetC(temp6);
        dComIfG_Ccsp_Set(&i_this->mAtSph);
        i_this->mAcch.CrrPos(dComIfG_Bgsp());

        if (fopAcM_searchPlayerDistance(i_this) > 5000.0f || i_this->mAtSph.ChkAtHit() ||
            (i_this->mAcch.m_flags & (dBgS_Acch::GROUND_HIT | dBgS_Acch::WALL_HIT | dBgS_Acch::ROOF_HIT)) != 0) {
            i_this->field_0x780 = 0x32;
            i_this->health = 1;

            JPABaseEmitter* pEmtr = dComIfGp_particle_set(ID_AK_SN_BPGHITDARKSHOT00, &i_this->current.pos);
            JPABaseEmitter_setGlobalRTMatrix(pEmtr, J3DModel_getAnmMtx(i_this->mpMorf->getModel(), YDKSP00_JNT_HEAD_e));
            JPABaseEmitter* pEmtr2 = dComIfGp_particle_set(ID_AK_SN_BPGHITDARKSHOT01, &i_this->current.pos);
            JPABaseEmitter_setGlobalRTMatrix(pEmtr2, J3DModel_getAnmMtx(i_this->mpMorf->getModel(), YDKSP00_JNT_HEAD_e));

            fopAcM_seStartCurrent_nn(i_this, JA_SE_OBJ_PG_EBALL_EXP_L, 0);
        }
    }

    if (i_this->field_0x2E0 != 0) {
        s16 v = (s16)(i_this->field_0x2E0 + 1);
        if (v > 8) {
            i_this->field_0x2E0 = 0;
        } else {
            i_this->field_0x2E0 = v;
        }
    }
}

/* 0213D594 (move() inlined) */
static BOOL daFgmahou_Execute(fgmahou_class* i_this) {
    WWHD_FUNC(0x0213D594, BOOL, i_this);
    dComIfGp_get(); /* HD: leftover accessor call (result unused) */

    i_this->mAge += 1;
    for (int i = 0; i < 2; i++) {
        if (i_this->mTimers[i] != 0) {
            i_this->mTimers[i] -= 1;
        }
    }
    if (i_this->field_0x2DE != 0) {
        i_this->field_0x2DE -= 1;
    }

    s8 cnt = i_this->field_0x780;
    if (cnt == 0) {
        move(i_this);
    } else {
        cnt = (s8)(cnt - 1);
        i_this->field_0x780 = cnt;
        if (cnt == 0) {
            fopAcM_delete(i_this);
            return TRUE;
        }
    }

    mDoExt_baseAnm_play(i_this->mpBrk);
    mDoExt_baseAnm_play(i_this->mpBtk);
    J3DModel* pModel = i_this->mpMorf->getModel();
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
    i_this->mpMorf->calc();

    JPABaseEmitter* em = i_this->mpEmitter;
    if (em != nullptr) {
        JPABaseEmitter_setGlobalRTMatrix(em, J3DModel_getAnmMtx(i_this->mpMorf->getModel(), YDKSP00_JNT_HEAD_e));
        s8 v = i_this->field_0x780;
        if (v != 0 && v == 0x32) {
            JPABaseEmitter_becomeInvalidEmitter(i_this->mpEmitter);
            mDoExt_brkAnm* brk = i_this->mpBrk;
            i_this->mpEmitter = nullptr;
            anm_frameCtrl(brk)->setRate(-1.0f);
        }
    }

    i_this->eyePos.copy(i_this->current.pos);
    return TRUE;
}
VERIFY(0x0213D594, daFgmahou_Execute);

/* 0213E328 */
static BOOL daFgmahou_IsDelete(fgmahou_class* i_this) {
    WWHD_FUNC(0x0213E328, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0213E328, daFgmahou_IsDelete);

/* 0213E330 */
static BOOL daFgmahou_Delete(fgmahou_class* i_this) {
    WWHD_FUNC(0x0213E330, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, ARC_DELETE);
    if (i_this->mpEmitter) {
        JPABaseEmitter_becomeInvalidEmitter(i_this->mpEmitter);
    }
    return TRUE;
}
VERIFY(0x0213E330, daFgmahou_Delete);

/* 0213E38C */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0213E38C, BOOL, a_this);
    fgmahou_class* i_this = static_cast<fgmahou_class*>(a_this);

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(ARC_HEAP, dRes_INDEX_FGANON_BDL_YDKSP00_e, SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, J3DFrameCtrl::EMode_LOOP, 1.0f,
                                            0, -1, 1, nullptr, 0, 0x11020203);
    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    J3DModelData* pModelData = J3DModel_getModelData(i_this->mpMorf->getModel());

    void* p = operator_new(0x74);
    if (p != nullptr) {
        p = gabi::call<void*>(0x025E7C6C, p); /* mDoExt_btkAnm::mDoExt_btkAnm (matcher: "init") */
    }
    i_this->mpBtk = (mDoExt_btkAnm*)p;
    if (i_this->mpBtk == nullptr) {
        return FALSE;
    }
    J3DAnmTextureSRTKey* btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(ARC_HEAP, dRes_INDEX_FGANON_BTK_YDKSP00_e, SAFESTRING_VTBL);
    if (!i_this->mpBtk->init(pModelData, btk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    p = operator_new(0x78);
    if (p != nullptr) {
        p = mDoExt_brkAnm_ct(p);
    }
    i_this->mpBrk = (mDoExt_brkAnm*)p;
    if (i_this->mpBrk == nullptr) {
        return FALSE;
    }
    J3DAnmTevRegKey* brk = (J3DAnmTevRegKey*)dComIfG_getObjectRes(ARC_HEAP, dRes_INDEX_FGANON_BRK_YDKSP00_e, SAFESTRING_VTBL);
    if (!mDoExt_brkAnm_init(i_this->mpBrk, pModelData, brk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }
    anm_frameCtrl(i_this->mpBrk)->setFrame(6.999f);

    return TRUE;
}
VERIFY(0x0213E38C, useHeapInit);

/* 0213E574 */
static cPhs_State daFgmahou_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0213E574, cPhs_State, a_this);
    fgmahou_class* i_this = static_cast<fgmahou_class*>(a_this);

    /* fopAcM_ct(i_this, fgmahou_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = FGMAHOU_VTBL;
            dBgS_AcchCir_ct(&i_this->mCir);
            dBgS_ObjAcch_ct(&i_this->mAcch, {0x1000F028, 0x1000F048, 0x1000F038});
            dCcD_Stts_ct(&i_this->mStts);
            gabi::call(0x025166F0, &i_this->mAtSph); /* dCcD_Sph::dCcD_Sph */
            gabi::call(0x025166F0, &i_this->mTgSph);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhs, ARC_CREATE);
    if (phase_state == cPhs_COMPLEATE_e) {
        i_this->mOrbNumber = fopAcM_GetParam(i_this) & 0xF;

        if (!fopAcM_entrySolidHeap(i_this, 0x0213E38C /* useHeapInit */, 0x19000)) {
            return cPhs_ERROR_e;
        }
        i_this->mAcch.Set(&i_this->current.pos, &i_this->old.pos, i_this, 1, &i_this->mCir, &i_this->speed);
        i_this->mCir.SetWall(50.0f, 50.0f);
        i_this->mAcch.m_flags &= ~(u32)dBgS_Acch::ROOF_NONE; /* ClrRoofNone */
        i_this->mAcch.SetRoofCrrHeight(REG0_F(7) + 70.0f);
        i_this->mStts.Init(0xFA, 0xFF, i_this);
        i_this->mTgSph.Set(tg_sph_src);
        i_this->mTgSph.SetStts(&i_this->mStts);
        OnTgNoHitMark(&i_this->mTgSph);
        i_this->mAtSph.Set(at_sph_src);
        i_this->mAtSph.SetStts(&i_this->mStts);
    }

    return phase_state;
}
VERIFY(0x0213E574, daFgmahou_Create);

/* 0213E76C: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_fgmahou_cpp() {
    WWHD_FUNC(0x0213E76C, void, (u32)0);
    sinit_header_statics(0x10463D9C, 0x101B4EF0);
}
VERIFY(0x0213E76C, __sinit_d_a_fgmahou_cpp);

/* 0213E800: sead::SafeString deleting destructor (this TU's vtable slot) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0213E800, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x0213E800, SafeString_dt);

/* 0213E814: fgmahou_class deleting destructor (compiler-generated, HD virtual destructor) */
static void fgmahou_class_dt(fgmahou_class* i_this, s32 flags) {
    WWHD_FUNC(0x0213E814, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mTgSph, 2); /* dCcD_Sph::~dCcD_Sph */
        gabi::call(0x02515AE8, &i_this->mAtSph, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        /* dBgS_ObjAcch::~dBgS_ObjAcch (inline): this TU's vtables, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(gabi::ea(&i_this->mAcch) + 0x20, 0x1000F038);
        gabi::store<u32>(gabi::ea(&i_this->mAcch) + 0x14, 0x1000F048);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0);
        gabi::call(0x02018034, gabi::ea(&i_this->mCir) + 0x14, 2); /* dBgS_AcchCir: cM3dGCir::~cM3dGCir */
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0213E814, fgmahou_class_dt);

/* 0213E8BC: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x0213E8BC, void, (u32)0);
}
VERIFY(0x0213E8BC, SafeString_assureTerminationImpl);
