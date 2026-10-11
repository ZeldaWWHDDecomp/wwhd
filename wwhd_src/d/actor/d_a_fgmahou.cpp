/**
 * d_a_fgmahou.cpp (WWHD)
 * Phantom Ganon's energy ball (one of 8 orbs): flies at the player, can be deflected back.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_fgmahou.cpp, USA version) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_fgmahou.h"

#define FGMAHOU_VTBL 0x1000F058    /* fgmahou_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x1000F010 /* this TU's sead::SafeString vtable */
#define ARC_HEAP "Fganon"          /* "Fganon" (useHeapInit) */
#define ARC_DELETE "Fganon"        /* "Fganon" (Delete) */
#define ARC_CREATE "Fganon"        /* "Fganon" (Create) */
#define tg_sph_src gabi::at<dCcD_SrcSph>(0x101B4E70)
#define at_sph_src gabi::at<dCcD_SrcSph>(0x101B4EB0)
/* move(): static f32 spdd[8], static s16 angXd[8] (.data) */
static inline f32 spdd(u32 i) { return (f32)(*gabi::at<be<f32>[8]>(0x101B4E40))[i]; }
static inline s16 angXd(u32 i) { return (s16)(*gabi::at<be<s16>[8]>(0x101B4E60))[i]; }

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

// HD: private inline views preserve the HD field positions and load/store order.
// HD: emitter matrices/flags moved in the HD JPA layout.
static inline void JPABaseEmitter_setGlobalRTMatrix(JPABaseEmitter *emitter, const Mtx34 *matrix) {
    auto *view = reinterpret_cast<FgmahouEmitterHD *>(emitter);
    fgmahouSetEmitterMatrix(matrix, &view->mGlobalRotation, &view->mGlobalTranslation);
}
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter *emitter) {
    auto *view = reinterpret_cast<FgmahouEmitterHD *>(emitter);
    u32 flags = view->mFlags;
    view->mMaxFrame = -1;
    view->mFlags = flags | 1;
}
/* J3DModel::getAnmMtx (HD): joint matrix block at +0x2C (flags +4 |= 0x10, matrices at +0x10) */
static inline const Mtx34 *fgmahouAnmMtx(J3DModel *model, s32 joint) {
    auto *buffer = model->mpMtxBuffer.get();
    u16 flags = buffer->mFlags;
    u32 matrices = buffer->mpAnmMtx;
    buffer->mFlags = flags | 0x10;
    return gabi::at<Mtx34>(matrices + (u32)joint * 0x30);
}

/* fopAcM_seStartCurrent (HD inline): null checks on the actor and &current */
static inline void fopAcM_seStartCurrent(fopAc_ac_c *a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(a->current.roomNo));
}
/* the same inline where the actor is known non-null: only &current is checked */
static inline void fopAcM_seStartCurrent_nn(fopAc_ac_c *a, u32 id, u32 param) {
    if (gabi::ea(&a->current.pos) != 0)
        mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(a->current.roomNo));
}
static inline J3DFrameCtrl *anm_frameCtrl(void *anm) {
    return gabi::at<J3DFrameCtrl>(gabi::ea(anm));
}
/* dCcD_GObjInf::OnTgNoHitMark: mGObjTg.mSPrm |= 4 */
static inline void OnTgNoHitMark(dCcD_GObjInf *o) { o->mGObjTg.mSPrm |= 4; }

/* 0213D4E0 */
BOOL daFgmahou_Draw(fgmahou_class *i_this) {
    J3DModel *pModel = i_this->mpMorf->getModel();
    mDoExt_brkAnm *brk = i_this->mpBrk;
    mDoExt_brkAnm_entry(brk, J3DModel_getModelData(pModel), anm_frameCtrl(brk)->getFrame());
    mDoExt_btkAnm *btk = i_this->mpBtk;
    btk->entry(J3DModel_getModelData(pModel), btk->getFrame());
    i_this->mpMorf->entryDL();
    return TRUE;
}

/* 0213D544 */
void *boss_s_sub(void *param_1, void *param_2) {
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_FGANON_e) {
        return param_1;
    }
    return nullptr;
}

/* |mTargetPos - current.pos| (cXyz::abs: the difference is copied, then sqrtf(PSVECSquareMag)) */
static inline f32 target_dist(fgmahou_class *i_this) {
    gabi::Local<cXyz> tmp;
    cXyz_mi(&i_this->mTargetPos, tmp, &i_this->current.pos);
    gabi::Local<cXyz> diff3;
    diff3->copy(*tmp);
    return std_sqrtf(PSVECSquareMag(diff3));
}

/* turn home.angle towards mTargetPos */
static inline void aim_at_target(fgmahou_class *i_this) {
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
static inline void move(fgmahou_class *i_this) {
    fopAc_ac_c *pPlayer = dComIfGp_getPlayer(0);
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

        // HD: the ball only reports a hit to Phantom Ganon when the At hit object's actor is
        // process 0xA8 (GameCube: any At hit)
        if (i_this->mAtSph.ChkAtHit()) {
            dCcD_GObjInf *obj = fgmahouGetAtHitObj(&i_this->mAtSph);
            if (obj != nullptr) {
                dCcD_Stts *stts = obj->mStts;
                if (stts != 0) {
                    fopAc_ac_c *ac = stts->mp_actor;
                    if (ac != 0 && (s16)ac->mProcName == fpcNm_HD_A8) {
                        void *fganon = fpcM_Search(FGMAHOU_BOSS_SEARCH, i_this);
                        if (fganon != nullptr) {
                            reinterpret_cast<FgmahouBossHD *>(fganon)->m68B = 1; /* fganon->m68B */
                            break;
                        }
                    }
                }
            }
        }

        if (!i_this->mTgSph.ChkTgHit()) {
            break;
        }

        dComIfGp_particle_set(ID_AK_SN_BPGSMASHDARKSHOT00, &i_this->current.pos,
                              &i_this->home.angle);
        fopAcM_seStartCurrent(i_this, JA_SE_LK_PG_BOMB_STRIKE, 0);
        i_this->mState = 5;
        /* fallthrough */
    case 5: {
        fopAc_ac_c *fganon2 = (fopAc_ac_c *)fpcM_Search(FGMAHOU_BOSS_SEARCH, i_this);
        if (fganon2 == nullptr) {
            i_this->field_0x780 = 0x32;
            break;
        }
        // HD: the target's y is kept in a register across cM_rndFX
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
            (i_this->mAcch.m_flags &
             (dBgS_Acch::GROUND_HIT | dBgS_Acch::WALL_HIT | dBgS_Acch::ROOF_HIT)) != 0) {
            i_this->field_0x780 = 0x32;
            i_this->health = 1;

            JPABaseEmitter *pEmtr =
                dComIfGp_particle_set(ID_AK_SN_BPGHITDARKSHOT00, &i_this->current.pos);
            JPABaseEmitter_setGlobalRTMatrix(
                pEmtr, fgmahouAnmMtx(i_this->mpMorf->getModel(), YDKSP00_JNT_HEAD_e));
            JPABaseEmitter *pEmtr2 =
                dComIfGp_particle_set(ID_AK_SN_BPGHITDARKSHOT01, &i_this->current.pos);
            JPABaseEmitter_setGlobalRTMatrix(
                pEmtr2, fgmahouAnmMtx(i_this->mpMorf->getModel(), YDKSP00_JNT_HEAD_e));

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

/* 0213D594 */
BOOL daFgmahou_Execute(fgmahou_class *i_this) {
    dComIfGp_get(); // HD: leftover accessor call (result unused)

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
    J3DModel *pModel = i_this->mpMorf->getModel();
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
    i_this->mpMorf->calc();

    JPABaseEmitter *em = i_this->mpEmitter;
    if (em != nullptr) {
        JPABaseEmitter_setGlobalRTMatrix(
            em, fgmahouAnmMtx(i_this->mpMorf->getModel(), YDKSP00_JNT_HEAD_e));
        s8 v = i_this->field_0x780;
        if (v != 0 && v == 0x32) {
            JPABaseEmitter_becomeInvalidEmitter(i_this->mpEmitter);
            mDoExt_brkAnm *brk = i_this->mpBrk;
            i_this->mpEmitter = nullptr;
            anm_frameCtrl(brk)->setRate(-1.0f);
        }
    }

    i_this->eyePos.copy(i_this->current.pos);
    return TRUE;
}

/* 0213E328 */
BOOL daFgmahou_IsDelete(fgmahou_class *i_this) { return TRUE; }

/* 0213E330 */
BOOL daFgmahou_Delete(fgmahou_class *i_this) {
    dComIfG_resDelete(&i_this->mPhs, ARC_DELETE);
    if (i_this->mpEmitter) {
        JPABaseEmitter_becomeInvalidEmitter(i_this->mpEmitter);
    }
    return TRUE;
}

/* 0213E38C */
BOOL useHeapInit(fopAc_ac_c *a_this) {
    fgmahou_class *i_this = static_cast<fgmahou_class *>(a_this);

    J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes(
        ARC_HEAP, dRes_INDEX_FGANON_BDL_YDKSP00_e, SAFESTRING_VTBL);
    i_this->mpMorf =
        mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr,
                               J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1, nullptr, 0, 0x11020203);
    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    J3DModelData *pModelData = J3DModel_getModelData(i_this->mpMorf->getModel());

    void *p = operator_new(0x74);
    if (p != nullptr) {
        p = fgmahouBtkConstruct(p); /* mDoExt_btkAnm::mDoExt_btkAnm (matcher: "init") */
    }
    i_this->mpBtk = (mDoExt_btkAnm *)p;
    if (i_this->mpBtk == nullptr) {
        return FALSE;
    }
    J3DAnmTextureSRTKey *btk = (J3DAnmTextureSRTKey *)dComIfG_getObjectRes(
        ARC_HEAP, dRes_INDEX_FGANON_BTK_YDKSP00_e, SAFESTRING_VTBL);
    if (!i_this->mpBtk->init(pModelData, btk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false,
                             0)) {
        return FALSE;
    }

    p = operator_new(0x78);
    if (p != nullptr) {
        p = fgmahouBrkConstruct(p);
    }
    i_this->mpBrk = (mDoExt_brkAnm *)p;
    if (i_this->mpBrk == nullptr) {
        return FALSE;
    }
    J3DAnmTevRegKey *brk = (J3DAnmTevRegKey *)dComIfG_getObjectRes(
        ARC_HEAP, dRes_INDEX_FGANON_BRK_YDKSP00_e, SAFESTRING_VTBL);
    if (!i_this->mpBrk->init(pModelData, brk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false,
                             0)) {
        return FALSE;
    }
    anm_frameCtrl(i_this->mpBrk)->setFrame(6.999f);

    return TRUE;
}

/* 0213E574 */
cPhs_State daFgmahou_Create(fopAc_ac_c *a_this) {
    fgmahou_class *i_this = static_cast<fgmahou_class *>(a_this);

    /* fopAcM_ct(i_this, fgmahou_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = FGMAHOU_VTBL;
            dBgS_AcchCir_ct(&i_this->mCir);
            dBgS_ObjAcch_ct(&i_this->mAcch,
                            {FGMAHOU_ACCH_VTABLE, FGMAHOU_POLY_VTABLE, FGMAHOU_GROUP_VTABLE});
            dCcD_Stts_ct(&i_this->mStts);
            fgmahouSphereConstruct(&i_this->mAtSph); /* dCcD_Sph::dCcD_Sph */
            fgmahouSphereConstruct(&i_this->mTgSph);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhs, ARC_CREATE);
    if (phase_state == cPhs_COMPLEATE_e) {
        i_this->mOrbNumber = fopAcM_GetParam(i_this) & 0xF;

        if (!fopAcM_entrySolidHeap(i_this, FGMAHOU_HEAP_INIT, 0x19000)) {
            return cPhs_ERROR_e;
        }
        i_this->mAcch.Set(&i_this->current.pos, &i_this->old.pos, i_this, 1, &i_this->mCir,
                          &i_this->speed);
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

/* 0213E76C */
void __sinit_d_a_fgmahou_cpp() { sinit_header_statics(0x10463D9C, 0x101B4EF0); }

/* 0213E800 */
void SafeString_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 0213E814 */
void fgmahou_class_dt(fgmahou_class *i_this, s32 flags) {
    if (i_this != nullptr) {
        fgmahouSphereDestroy(&i_this->mTgSph, 2); /* dCcD_Sph::~dCcD_Sph */
        fgmahouSphereDestroy(&i_this->mAtSph, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        /* dBgS_ObjAcch::~dBgS_ObjAcch (inline): this TU's vtables, then dBgS_Acch::~dBgS_Acch */
        i_this->mAcch.dBgS_GrpPassChk::__vtbl = FGMAHOU_GROUP_VTABLE;
        i_this->mAcch.dBgS_PolyPassChk::__vtbl = FGMAHOU_POLY_VTABLE;
        fgmahouAcchDestroy(&i_this->mAcch, 0);
        fgmahouCircleDestroy(&i_this->mCir.m_cir, 2); /* dBgS_AcchCir: cM3dGCir::~cM3dGCir */
        fgmahouActorDestroy(i_this, 0);               /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0213E8BC */
void SafeString_assureTerminationImpl(void *object) {}

// clang-format off
/* WWHD glue
str 1000F0A4 in 0213E38C
str 1000F098 in 0213E330
str 1000F0B0 in 0213E574
ext 028249B0 void fgmahouSetEmitterMatrix(const Mtx34*,Mtx34*,cXyz*)
ext 02516178 dCcD_GObjInf* fgmahouGetAtHitObj(dCcD_GObjInf*)
ext 025E7C6C void* fgmahouBtkConstruct(void*)
ext 025E80D0 mDoExt_brkAnm* fgmahouBrkConstruct(void*)
ext 025166F0 void fgmahouSphereConstruct(dCcD_Sph*)
ext 02515AE8 void fgmahouSphereDestroy(dCcD_Sph*,s32)
ext 024EFD9C void fgmahouAcchDestroy(dBgS_Acch*,s32)
ext 02018034 void fgmahouCircleDestroy(void*,s32)
ext 025D50BC void fgmahouActorDestroy(fopAc_ac_c*,s32)
*/
