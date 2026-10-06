/**
 * d_a_ff.cpp (WWHD)
 * Object - Firefly (not collectable)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ff.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * HD: each firefly is a point light (LIGHT_INFLUENCE at the end of the class); the glow model
 * and the z-buffer visibility test (z_check, GXPeekZ) are gone from Draw.
 */
#include "bindings.h"

#define FF_VTBL 0x1000EB2C         /* ff_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x1000EAC4 /* this TU's sead::SafeString vtable */
#define cc_sph_src gabi::at<dCcD_SrcSph>(0x101B4C04)
enum { fpcNm_FF_e = 0xBB };
#define REG13_F(i) REG_F(13, i)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u8* fopAcM_CreateAppend() { return gabi::call<u8*>(0x025D5600); }
static inline void* fpcLy_CurrentLayer() { return gabi::call<void*>(0x025DED64); }
static inline u32 fopAcM_Create(s16 name, void* append) {
    void* layer = fpcLy_CurrentLayer();
    return gabi::call<u32>(0x025E14A8, layer, name, 0, 0, append);
}
/* 025E2DA8 mDoExt_modelUpdate(J3DModel*) */
static inline void mDoExt_modelUpdate(J3DModel* m) { gabi::call(0x025E2DA8, m); }
/* dComIfGd_setListMaskOff: play+0x5D84/0x5D88 */
static inline void dComIfGd_setListMaskOff() {
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D84));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D88));
}
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0 (the matcher calls it init), init 025E8154,
 * entry 025E83FC; frame at +4 */
static inline void* mDoExt_brkAnm_ct(void* p) { return gabi::call<void*>(0x025E80D0, p); }
static inline BOOL mDoExt_brkAnm_init_l(void* a, J3DModelData* d, void* brk, s32 anmPlay, s32 mode, f32 rate, s16 start, s16 end,
                                        bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, brk, anmPlay, mode, rate, start, end, modify, entry);
}
static inline void cBgS_Chk_dt(void* c, s32 flags) { gabi::call(0x02008DAC, c, flags); }
static inline be<s32>& ff_count() { return *gabi::at<be<s32>>(0x101B4BD0); }

struct ff_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel[2];
    /* 0x3BC */ gptr<u8> mBrkAnm[2];          /* mDoExt_brkAnm* */
    /* 0x3C4 */ u8 m2A8;
    /* 0x3C5 */ be<u8> mbNoUseGroundY;
    /* 0x3C6 */ u8 _3C6[2];
    /* 0x3C8 */ be<f32> mRotVel;
    /* 0x3CC */ be<f32> mVelocityFwdTarget;
    /* 0x3D0 */ cXyz mVelImpulse;
    /* 0x3DC */ cXyz m2C0;
    /* 0x3E8 */ be<f32> mScale;
    /* 0x3EC */ be<f32> mScaleTarget;
    /* 0x3F0 */ be<f32> mGlowScale;
    /* 0x3F4 */ be<f32> mGlowScaleY;
    /* 0x3F8 */ be<f32> mGroundY;
    /* 0x3FC */ cXyz mScatterPos;
    /* 0x408 */ cXyz m2EC;
    /* 0x414 */ cXyz mHomePos;
    /* 0x420 */ be<s16> mTargetRotX;
    /* 0x422 */ be<s16> mTargetRotY;
    /* 0x424 */ be<s16> mTimers[5];
    /* 0x42E */ be<s16> mLiveTimer;
    /* 0x430 */ be<s8> mMode;
    /* 0x431 */ u8 _431[2];
    /* 0x433 */ be<u8> mbNotVisibleZ;
    /* 0x434 */ dCcD_Stts mStts;
    /* 0x470 */ dCcD_Sph mSph;
    /* 0x59C */ LIGHT_INFLUENCE mInf;          /* HD */
};
WWHD_OFFSET(ff_class, mStts, 0x434);
WWHD_OFFSET(ff_class, mSph, 0x470);
WWHD_OFFSET(ff_class, mInf, 0x59C);
WWHD_SIZE(ff_class, 0x5C0);

/* 02132F3C: fire_fly_draw inlined. HD: no mDoLib_project/z_check, no glow model */
static BOOL daFf_Draw(ff_class* i_this) {
    WWHD_FUNC(0x02132F3C, BOOL, i_this);
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, false);
    f32 s = i_this->mScale;
    MtxScale(s, s, s, true);
    J3DModel_setBaseTRMtx(i_this->mpModel[0], calc_mtx());
    if (i_this->mScale > 0.01f) {
        mDoExt_baseAnm_play(i_this->mBrkAnm[0].get());
        J3DModel* m = i_this->mpModel[0];
        u8* brk = i_this->mBrkAnm[0];
        mDoExt_brkAnm_entry((mDoExt_brkAnm*)(void*)brk, J3DModel_getModelData(m), gabi::load<f32>(gabi::ea(brk) + 4));
        dComIfGd_setListMaskOff();
        mDoExt_modelUpdate(i_this->mpModel[0]);
        dComIfGd_setList();
    }
    return TRUE;
}
VERIFY(0x02132F3C, daFf_Draw);

/* the dBgS_GndChk on fire_fly_move's stack (this TU's vtables) */
static const dBgS_GndChk_vt l_gnd_vt = {0x1000EAEC, 0x1000EAFC, 0x1000EB1C, 0x1000EB0C};

/* label_870 of fire_fly_move */
static inline void ff_groundGlow(ff_class* i_this) {
    f32 g = i_this->mGroundY;
    f32 y = i_this->current.pos.y;
    if (y < g + 12.5f) {
        if (y < g)
            i_this->current.pos.y = g;
        cLib_addCalc2(&i_this->mGlowScaleY, 0.5f, 0.2f, 0.1f);
    } else {
        cLib_addCalc2(&i_this->mGlowScaleY, 1.0f, 0.2f, 0.1f);
    }
}

/* 02133070: fire_fly_move inlined */
static BOOL daFf_Execute(ff_class* i_this) {
    WWHD_FUNC(0x02133070, BOOL, i_this);
    for (int i = 0; i < 5; i++) {
        if (i_this->mTimers[i] != 0) {
            i_this->mTimers[i] -= 1;
        }
    }

    /* ---- fire_fly_move ---- */
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<dBgS_GndChk> chk;
    dBgS_GndChk_ct(chk, l_gnd_vt, false);
    i_this->mLiveTimer += 1;
    if (i_this->mTimers[2] == 0) {
        i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(50.0f) + 40.0f);
        i_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(200.0f) + 100.0f);
    }
    if (i_this->mTimers[1] == 0) {
        i_this->mScaleTarget = gabi::fmadds(cM_ssin(i_this->mLiveTimer * 1000) * 0.15f, 0.25f, 0.225f);
    } else {
        i_this->mScaleTarget = 0.0f;
    }
    cLib_addCalc2(&i_this->mScale, i_this->mScaleTarget, 0.1f, 0.05f);

    u32 mode = (u32)(s32)i_this->mMode;
    bool move = false;
    if (mode == 0 || mode == 1) {
        if (mode == 0) {
            u32 c = gabi::ea(chk.get());
            f32 x = i_this->current.pos.x, y = i_this->current.pos.y + 250.0f, z = i_this->current.pos.z;
            gabi::store<f32>(c + 0x2C, z);
            gabi::store<f32>(c + 0x24, x);
            gabi::store<f32>(c + 0x28, y);
            i_this->mGroundY = cBgS_GroundCross(dComIfG_Bgsp(), chk) + 12.5f;
            if (i_this->mbNoUseGroundY == 0) {
                i_this->current.pos.y = i_this->mGroundY;
            }
            i_this->mHomePos.y = i_this->current.pos.y;
            i_this->mHomePos.x = i_this->current.pos.x;
            i_this->mHomePos.z = i_this->current.pos.z;
            i_this->mMode += 1;
        }
        /* case 1 */
        cLib_addCalc2(&i_this->current.pos.x, i_this->mHomePos.x, 0.1f, std::fabs((f32)i_this->speed.x));
        cLib_addCalc2(&i_this->current.pos.y, i_this->mHomePos.y, 0.1f, std::fabs((f32)i_this->speed.y));
        cLib_addCalc2(&i_this->current.pos.z, i_this->mHomePos.z, 0.1f, std::fabs((f32)i_this->speed.z));
        f32 yd = player->current.pos.y - i_this->current.pos.y;
        f32 xd = player->current.pos.x - i_this->current.pos.x;
        f32 zd = player->current.pos.z - i_this->current.pos.z;
        f32 dist = std_sqrtf(gabi::fmadds(zd, zd, gabi::fmadds(xd, xd, yd * yd)));
        if (dist < 250.0f) {
            i_this->mMode += 1;
            i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(100.0f) + 1000.0f);
            i_this->current.angle.x = -0x3000;
            i_this->speedF = 10.0f;
        }
        ff_groundGlow(i_this);
    } else if (mode == 2) {
        if (i_this->mTimers[0] == 0) {
            i_this->mScatterPos.x = cM_rndFX(750.0f) + i_this->mHomePos.x;
            i_this->mScatterPos.z = cM_rndFX(750.0f) + i_this->mHomePos.z;
            f32 ry = cM_rndFX(225.0f);
            i_this->mScatterPos.y = (i_this->mHomePos.y + ry) + 137.5f;
            i_this->mRotVel = 0.0f;
            i_this->mVelocityFwdTarget = cM_rndF(20.0f) + 10.0f;
            f32 xd = i_this->mScatterPos.x - i_this->current.pos.x;
            f32 yd = i_this->mScatterPos.y - i_this->current.pos.y;
            f32 zd = i_this->mScatterPos.z - i_this->current.pos.z;
            f32 xx = xd * xd, zz = zd * zd;
            f32 dist = std_sqrtf(gabi::fmadds(yd, yd, xx) + zz);
            i_this->mTimers[0] = (s16)gabi::ftoi(dist / i_this->mVelocityFwdTarget);
            i_this->mTargetRotY = cM_atan2s(xd, zd);
            f32 xz_dist = std_sqrtf(xx + zz);
            i_this->mTargetRotX = (s16)-cM_atan2s(yd, xz_dist);
        }
        if (i_this->mTimers[3] == 0) {
            i_this->mScatterPos.x = i_this->mHomePos.x;
            i_this->mScatterPos.y = i_this->mHomePos.y;
            i_this->mScatterPos.z = i_this->mHomePos.z;
            i_this->mRotVel = 0.0f;
            i_this->mMode += 1;
        }
        move = true;
    } else if (mode == 3) {
        f32 xd = i_this->mScatterPos.x - i_this->current.pos.x;
        f32 zd = i_this->mScatterPos.z - i_this->current.pos.z;
        f32 yd = i_this->mScatterPos.y - i_this->current.pos.y;
        i_this->mTargetRotY = cM_atan2s(xd, zd);
        f32 xx = xd * xd, zz = zd * zd;
        f32 xz_dist = std_sqrtf(xx + zz);
        i_this->mTargetRotX = (s16)-cM_atan2s(yd, xz_dist);
        if (gabi::fmadds(yd, yd, xx) + zz < 2500.0f) {
            i_this->mMode = 1;
        }
        cLib_addCalc2(&i_this->mRotVel, 10.0f, 1.0f, 0.15f);
        move = true;
    } else if (mode == 4) {
        if (i_this->mTimers[4] == 0) {
            i_this->mMode = 2;
            i_this->mTimers[3] = (s16)gabi::ftoi(cM_rndF(100.0f) + 1000.0f);
        } else {
            PSVECAdd(&i_this->mVelImpulse, &i_this->m2C0, &i_this->mVelImpulse); /* mVelImpulse += m2C0 */
            if (cM_rndF(1.0f) < REG13_F(0) + 0.1f) {
                i_this->m2C0.x = cM_rndFX(REG13_F(1) + 1.0f);
            }
            if (cM_rndF(1.0f) < REG13_F(0) + 0.1f) {
                i_this->m2C0.z = cM_rndFX(REG13_F(1) + 1.0f);
            }
            i_this->m2C0.y = REG13_F(2) + 2.0f;
            f32 lim = REG13_F(3) + 10.0f;
            if (i_this->mVelImpulse.y > lim) {
                i_this->mVelImpulse.y = lim;
            }
        }
        move = true;
    } else {
        ff_groundGlow(i_this);
    }

    if (move) {
        f32 step = 500.0f;
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mTargetRotY, 10, (s16)gabi::ftoi(step * i_this->mRotVel));
        cLib_addCalcAngleS2(&i_this->current.angle.x, i_this->mTargetRotX, 10, (s16)gabi::ftoi(step * i_this->mRotVel));
        cLib_addCalc2(&i_this->mRotVel, 1.0f, 1.0f, 0.1f);
        cLib_addCalc2(&i_this->speedF, i_this->mVelocityFwdTarget, 1.0f, 3.0f);
        gabi::Local<cXyz> local_cc;
        local_cc->x = 0.0f;
        local_cc->y = 0.0f;
        local_cc->z = i_this->speedF * 0.25f;
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        MtxPosition(local_cc, &i_this->speed);
        gabi::Local<cXyz> sum;
        cXyz_pl(&i_this->speed, sum, &i_this->mVelImpulse);
        PSVECAdd(&i_this->current.pos, sum, &i_this->current.pos);
        cLib_addCalc0(&i_this->mVelImpulse.x, 1.0f, 0.1f);
        cLib_addCalc0(&i_this->mVelImpulse.y, 1.0f, 0.1f);
        cLib_addCalc0(&i_this->mVelImpulse.z, 1.0f, 0.1f);
        ff_groundGlow(i_this);
    }

    /* HD: the firefly's point light follows it; its power follows the scale */
    {
        u32 p = gabi::ea(&i_this->current.pos);
        u32 x = gabi::load<u32>(p), y = gabi::load<u32>(p + 4), z = gabi::load<u32>(p + 8);
        f32 power = 255.0f * i_this->mScale;
        gabi::store<u32>(gabi::ea(&i_this->mInf.mPos), x);
        gabi::store<u32>(gabi::ea(&i_this->mInf.mPos) + 4, y);
        gabi::store<u32>(gabi::ea(&i_this->mInf.mPos) + 8, z);
        i_this->mInf.mColorR = 100;
        i_this->mInf.mColorG = 300;
        i_this->mInf.mColorB = 300;
        i_this->mInf.mPower = power;
    }
    /* ~dBgS_GndChk */
    {
        u32 b = gabi::ea(chk.get());
        gabi::store<u32>(b + 0x20, 0x1000EAFC);
        gabi::store<u32>(b + 0x40, 0x1000EB1C);
        gabi::store<u32>(b + 0x4C, 0x1000EADC);
        cBgS_Chk_dt(chk, 0);
    }

    if (i_this->mSph.ChkTgHit() != 0) {
        i_this->mMode = 4;
        i_this->mTimers[4] = (s16)gabi::ftoi((cM_rndF(20.0f) + 30.0f) + REG13_F(9));
    }
    i_this->mSph.SetC(&i_this->current.pos);
    dComIfG_Ccsp_Set(&i_this->mSph);
    return TRUE;
}
VERIFY(0x02133070, daFf_Execute);

/* 02133AB8 */
static BOOL daFf_IsDelete(ff_class*) {
    WWHD_FUNC(0x02133AB8, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02133AB8, daFf_IsDelete);

/* 02133AC0 */
static BOOL daFf_Delete(ff_class* i_this) {
    WWHD_FUNC(0x02133AC0, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x1000EBAC) /* "Ff" */); /* resDeleteDemo */
    ff_count() = 0;
    dKy_plight_cut(&i_this->mInf); /* HD */
    return TRUE;
}
VERIFY(0x02133AC0, daFf_Delete);

/* 02133B10 */
static BOOL useHeapInit(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x02133B10, BOOL, i_ac);
    ff_class* a_this = (ff_class*)i_ac;
    for (int i = 0; i < 2; i++) {
        u32 bmd = gabi::load<u32>(0x101B4BF4 + 4 * i); /* ho_bmd[i] */
        J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1000EBB4) /* "Ff" */, bmd, SAFESTRING_VTBL);
        if (modelData == nullptr) { /* JUT_ASSERT(741, modelData != NULL) */
            JUT_ASSERT_fail(STR(0x1000EBB8), 0x2E5, STR(0x1000EBC4));
            return FALSE;
        }
        a_this->mpModel[i] = mDoExt_J3DModel__create(modelData, 0x10000, 0x11020203);
        if (a_this->mpModel[i] == nullptr) {
            return FALSE;
        }
        /* new mDoExt_brkAnm() */
        void* brk = operator_new(0x78);
        if (brk != nullptr)
            brk = mDoExt_brkAnm_ct(brk);
        a_this->mBrkAnm[i] = (u8*)brk;
        if (a_this->mBrkAnm[i] == nullptr) {
            return FALSE;
        }
        J3DModel* model = a_this->mpModel[i];
        u32 brkIdx = gabi::load<u32>(0x101B4BFC + 4 * i); /* ho_brk[i] */
        void* key = dComIfG_getObjectRes(STR(0x1000EBB4), brkIdx, SAFESTRING_VTBL);
        f32 rate = cM_rndF(0.15f) + 0.9f;
        int iVar6 = mDoExt_brkAnm_init_l(a_this->mBrkAnm[i].get(), J3DModel_getModelData(model), key, true, J3DFrameCtrl::EMode_LOOP,
                                         rate, 0, -1, false, 0);
        if (iVar6 == 0) {
            return FALSE;
        }
    }
    return TRUE;
}
VERIFY(0x02133B10, useHeapInit);

/* 02133CF0: ff_class::ff_class (HD: out of line; allocates when this == NULL) */
static ff_class* ff_class_ct(ff_class* i_this) {
    WWHD_FUNC(0x02133CF0, ff_class*, i_this);
    if (i_this == nullptr) {
        i_this = (ff_class*)operator_new(0x5C0);
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = FF_VTBL;
    dCcD_Stts_ct(&i_this->mStts);
    gabi::call(0x025166F0, &i_this->mSph); /* dCcD_Sph::dCcD_Sph */
    i_this->mInf.mHD20 = 1.0f;            /* HD: LIGHT_INFLUENCE constructor */
    return i_this;
}
VERIFY(0x02133CF0, ff_class_ct);

/* 02133D80 */
static cPhs_State daFf_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02133D80, cPhs_State, i_this);
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr)
            ff_class_ct((ff_class*)i_this);
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    ff_class* a_this = (ff_class*)i_this;
    cPhs_State phase_state = dComIfG_resLoad(&a_this->mPhs, STR(0x1000EBDC) /* "Ff" */);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(a_this, 0x02133B10 /* useHeapInit */, 0x2320)) {
            int uVar1 = fopAcM_GetParam(a_this) & 0xFF;
            if (uVar1 != 0) {
                a_this->mParameters = fopAcM_GetParam(a_this) & 0xFF00;
                for (int i = 0; i < uVar1; i++) {
                    u8* p = fopAcM_CreateAppend();
                    u32 pa = gabi::ea(p);
                    f32 rx = cM_rndFX(500.0f);
                    gabi::store<f32>(pa + 4, a_this->current.pos.x + rx);
                    gabi::store<f32>(pa + 8, a_this->current.pos.y);
                    f32 rz = cM_rndFX(500.0f);
                    gabi::store<s16>(pa + 0x14, 0);
                    gabi::store<s16>(pa + 0x12, 0);
                    gabi::store<s16>(pa + 0x10, 0);
                    gabi::store<f32>(pa + 0xC, a_this->current.pos.z + rz);
                    gabi::store<u32>(pa + 0, fopAcM_GetParam(a_this));
                    fopAcM_Create(fpcNm_FF_e, p);
                }
            }
            a_this->mbNoUseGroundY = fopAcM_GetParam(a_this) >> 8;
            ff_count() += 1;
            a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(a_this->mpModel[0]));
            a_this->mHomePos.copy(a_this->current.pos);
            a_this->m2EC.copy(a_this->mHomePos);
            a_this->mLiveTimer = (s16)gabi::ftoi(cM_rndF(32768.0f));
            a_this->mTimers[2] = (s16)gabi::ftoi(cM_rndF(100.0f));
            a_this->mStts.Init(200, 0, a_this);
            a_this->mSph.Set(cc_sph_src);
            /* HD: the point light starts at the home position */
            a_this->mInf.mPos.copy(a_this->m2EC);
            a_this->mInf.mPower = 0.0f;
            a_this->mSph.SetStts(&a_this->mStts);
            a_this->mInf.mFluctuation = 250.0f;
            a_this->mInf.mColorR = 100;
            a_this->mInf.mColorG = 300;
            a_this->mInf.mColorB = 300;
            dKy_plight_set(&a_this->mInf);
        } else {
            return cPhs_ERROR_e;
        }
    }
    return phase_state;
}
VERIFY(0x02133D80, daFf_Create);

/* 02133FF0: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_ff_cpp() {
    WWHD_FUNC(0x02133FF0, void, (u32)0);
    sinit_header_statics(0x10463D14, 0x101B4C44);
}
VERIFY(0x02133FF0, __sinit_d_a_ff_cpp);

/* 02134084: sead::SafeString deleting destructor (this TU's vtable 0x1000EAC4) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02134084, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x02134084, SafeString_dt);

/* 02134098: ff_class deleting destructor (compiler-generated, HD virtual destructor) */
static void ff_class_dt(ff_class* i_this, s32 flags) {
    WWHD_FUNC(0x02134098, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02134098, ff_class_dt);

/* 02134104: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x02134104, void, (u32)0);
}
VERIFY(0x02134104, SafeString_assureTerminationImpl);
