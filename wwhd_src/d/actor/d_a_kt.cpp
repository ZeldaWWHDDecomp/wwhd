/**
 * d_a_kt.cpp (WWHD)
 * NPC - Unused - Small bird
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kt.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x100137DC
#define KT_VTBL 0x10013844 /* HD: kt_class vtable */
#define kt_scale_g (*gabi::at<be<f32>>(0x101B8988)) /* static f32 kt_scale */

enum {
    dRes_INDEX_KT_BMD_KT_HANE_e = 0xA,
    dRes_INDEX_KT_BMD_KT_MODEL_e = 0xB,
};

/* HD: the GameCube field_0x29c[2] and mFrameCtrl (0x29C..0x2B8) are gone: GameCube +0x11C up to
 * mpModel, +0x100 from mpModelWing on. Size 0x420. */
struct kt_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<J3DModel> mpModelWing;
    /* 0x3BC */ be<f32> mGroundY;
    /* 0x3C0 */ be<f32> mLiftY;
    /* 0x3C4 */ be<f32> mLiftYTimer;
    /* 0x3C8 */ be<f32> mSpeedLerp;
    /* 0x3CC */ be<s32> mWingTimer;
    /* 0x3D0 */ be<f32> mSpeedFwd;
    /* 0x3D4 */ cXyz mSpeedVel;
    /* 0x3E0 */ be<f32> mWingScale;
    /* 0x3E4 */ cXyz field_0x2e4;
    /* 0x3F0 */ cXyz mTargetPos;
    /* 0x3FC */ cXyz mTargetPosHome;
    /* 0x408 */ cXyz mHomePos;
    /* 0x414 */ be<s16> mTimer[3];
    /* 0x41A */ be<s16> mAngleRoll;
    /* 0x41C */ be<s8> mMode;
    /* 0x41D */ be<u8> field_0x31d;
    /* 0x41E */ be<u8> mHitGround;
    /* 0x41F */ be<u8> field_0x31f;
};
WWHD_OFFSET(kt_class, mpModelWing, 0x3B8);
WWHD_OFFSET(kt_class, mWingScale, 0x3E0);
WWHD_OFFSET(kt_class, mTimer, 0x414);
WWHD_SIZE(kt_class, 0x420);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E2DA8 mDoExt_modelUpdate(J3DModel*) */
static inline void mDoExt_modelUpdate(J3DModel* m) { gabi::call(0x025E2DA8, m); }
/* 020077BC CPad_CHECK_TRIG_LEFT(pad) (HD: out of line) */
static inline BOOL CPad_CHECK_TRIG_LEFT(s32 pad) { return gabi::call<BOOL>(0x020077BC, pad); }
/* fopAcM_prm_class (append) */
static inline u8* fopAcM_CreateAppend() { return gabi::call<u8*>(0x025D5600); }
/* fpcM_Create(name, NULL, append): fpcSCtRq_Request(fpcLy_CurrentLayer(), name, 0, 0, append) */
static inline void fpcM_Create(s16 name, void* append) {
    u32 layer = gabi::call<u32>(0x025DED64);
    gabi::call(0x025E14A8, layer, name, 0, 0, append);
}
/* dBgS_GndChk on the stack (this TU's vtables) */
static const dBgS_GndChk_vt GNDCHK_VT = {0x10013804, 0x10013814, 0x10013834, 0x10013824};
static inline void dBgS_GndChk_dt(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x20, 0x10013814);
    gabi::store<u32>(b + 0x40, 0x10013834);
    gabi::store<u32>(b + 0x4C, 0x100137F4);
    gabi::call(0x02008DAC, c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}
static inline void gndChk_SetPos(void* c, f32 x, f32 y, f32 z) {
    cXyz* p = gabi::at<cXyz>(gabi::ea(c) + 0x24);
    p->x = x;
    p->y = y;
    p->z = z;
}
/* lfs -> stfs with no arithmetic in between is bit-exact in the recompiled original (a signalling
 * NaN is not quieted): such copies are written as word copies */
static inline void fcopy(be<f32>* dst, const be<f32>* src) { gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src))); }
static inline void J3DModel_setBaseScale_bits(J3DModel* m, const cXyz* s) {
    for (u32 i = 0; i < 12; i += 4) gabi::store<u32>(gabi::ea(m) + 0xBC + i, gabi::load<u32>(gabi::ea(s) + i));
}
static inline f32 fabsf_(f32 v) { return v < 0.0f ? -v : (v == 0.0f ? 0.0f * v + 0.0f : v); }

/* 021AAF50 (kotori_draw inlined) */
static BOOL daKt_Draw(kt_class* i_this) {
    WWHD_FUNC(0x021AAF50, BOOL, i_this);
    kt_scale_g = REG0_F(0) + 1.0f;
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y + i_this->mLiftY, i_this->current.pos.z, false);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->mAngleRoll);
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
    f32 scaleMag = 0.2f * kt_scale_g;
    MtxScale(scaleMag, scaleMag, scaleMag, true);
    J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
    mDoExt_modelUpdate(i_this->mpModel);
    if (i_this->mWingScale < 10.0f) {
        MtxTrans(0.0f, 130.0f, 80.0f, true);
        MtxScale(1.0f, i_this->mWingScale, 1.0f, true);
        J3DModel_setBaseTRMtx(i_this->mpModelWing, calc_mtx());
        mDoExt_modelUpdate(i_this->mpModelWing);
    }
    return TRUE;
}
VERIFY(0x021AAF50, daKt_Draw);

/* the reset of the bird (cases 11 and 20 of kotori_move) */
static inline void kt_reset(kt_class* i_this) {
    i_this->mMode = 0;
    i_this->mTimer[0] = 0;
    f32 r = cM_rndFX(10.0f);
    i_this->mLiftYTimer = r + 10.0f;
    i_this->mTargetPosHome.y = i_this->mTargetPosHome.y + 2000.0f;
    i_this->current.angle.x = -0x2000;
    i_this->mTimer[2] = REG0_S(5) + 300;
}

/* 021AB148: daKt_Execute with kotori_move inlined */
static BOOL daKt_Execute(kt_class* i_this) {
    WWHD_FUNC(0x021AB148, BOOL, i_this);
    i_this->mWingTimer = i_this->mWingTimer + REG0_S(0) + 0x5e76;
    for (s32 i = 0; i < 3; i++)
        if (i_this->mTimer[i] != 0)
            i_this->mTimer[i] = i_this->mTimer[i] - 1;

    /* ---- kotori_move ---- */
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    bool dispWing = false;
    u8 ret = 0;
    gabi::Local<u8[0x54]> gndChk;
    dBgS_GndChk_ct(gndChk.get(), GNDCHK_VT, false);

    f32 dx = player->current.pos.x - i_this->current.pos.x;
    f32 dz = player->current.pos.z - i_this->current.pos.z;
    f32 dist_xz = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    cLib_addCalcAngleS2(&i_this->mAngleRoll, 0, 2, REG0_S(4) + 0x1000);

    f32 vx = i_this->mTargetPos.x - i_this->current.pos.x;
    f32 vy = i_this->mTargetPos.y - i_this->current.pos.y;
    f32 vz = i_this->mTargetPos.z - i_this->current.pos.z;
    s16 angleX = cM_atan2s(vx, vz);
    s16 angleY = -cM_atan2s(vy, std_sqrtf(vx * vx + vz * vz));

    gabi::Local<cXyz> offs;
    gabi::Local<cXyz> pt;
    s8 mode = i_this->mMode;
    /* player->getHeadTopPos() (HD: +0x3D8) */
    f32 headX = gabi::load<f32>(gabi::ea(player) + 0x3D8);
    f32 headY = gabi::load<f32>(gabi::ea(player) + 0x3DC);
    f32 headZ = gabi::load<f32>(gabi::ea(player) + 0x3E0);
    f32 dist;

    switch (mode) {
    case 0:
        i_this->mSpeedFwd = gabi::fmadds(kt_scale_g, 20.0f, 30.0f);
        if (CPad_CHECK_TRIG_LEFT(0) && (s32)fopAcM_GetParam(i_this) == 1000) {
            fcopy(&i_this->mTargetPos.x, &player->current.pos.x);
            i_this->mTargetPos.y = player->current.pos.y + 500.0f;
            fcopy(&i_this->mTargetPos.z, &player->current.pos.z);
            i_this->mMode = 2;
            i_this->mSpeedLerp = 0.0f;
            break;
        }

        if (i_this->mTimer[2] == 0) {
            i_this->mMode = 1;
            offs->x = 0.0f;
            offs->y = 0.0f;
            offs->z = gabi::fmadds(REG0_F(13), 100.0f, 3000.0f);
            cMtx_YrotS(calc_mtx(), player->shape_angle.y);
            MtxPosition(offs.get(), pt.get());
            f32 r = REG0_F(14) + 200.0f;
            f32 a = player->current.pos.x + pt->x;
            r = cM_rndFX(r);
            i_this->mTargetPosHome.x = a + r;
            i_this->mTargetPosHome.y = player->current.pos.y + 1000.0f;
            r = cM_rndFX(REG0_F(14) + 200.0f);
            i_this->mTargetPosHome.z = player->current.pos.z + pt->z + r;

            gndChk_SetPos(gndChk.get(), i_this->mTargetPosHome.x, i_this->mTargetPosHome.y, i_this->mTargetPosHome.z);
            i_this->mTargetPosHome.y = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
            if (i_this->mTargetPosHome.y == -1000000000.0f /* -G_CM3D_F_INF */) {
                fcopy(&i_this->mTargetPosHome.x, &player->current.pos.x);
                fcopy(&i_this->mTargetPosHome.y, &player->current.pos.y);
                fcopy(&i_this->mTargetPosHome.z, &player->current.pos.z);
            }
            i_this->mTargetPos.x = i_this->mTargetPosHome.x;
            i_this->mTargetPos.z = i_this->mTargetPosHome.z;
            i_this->mTargetPos.y = i_this->mTargetPosHome.y;
            i_this->mSpeedLerp = 0.0f;
        } else if (i_this->mTimer[0] == 0) {
            i_this->mTimer[0] = (s16)gabi::ftoi(cM_rndF(250.0f) + 60.0f);
            f32 r = cM_rndFX(2000.0f);
            i_this->mTargetPos.x = r + i_this->mTargetPosHome.x;
            r = cM_rndFX(2000.0f);
            i_this->mTargetPos.z = r + i_this->mTargetPosHome.z;
            r = cM_rndF(1000.0f);
            i_this->mTargetPos.y = i_this->mTargetPosHome.y + r;
            i_this->mSpeedLerp = 0.0f;
        }

        cLib_addCalc2(&i_this->mSpeedLerp, 1.0f, 1.0f, 0.1f);
        goto calc_012;
    case 1:
        dist = std_sqrtf(gabi::fmadds(vy, vy, vx * vx) + vz * vz);
        if (dist < gabi::fmadds(REG0_F(1), 10.0f, 800.0f)) {
            i_this->mMode = 8;
        }
    case_1_tail:
        cLib_addCalc2(&i_this->mSpeedLerp, 3.0f, 1.0f, 0.1f);
    calc_012:
        cLib_addCalcAngleS2(&i_this->current.angle.y, angleX, 10,
                            (s16)gabi::ftoi(gabi::fmadds(REG0_F(2), 10.0f, 500.0f) * i_this->mSpeedLerp));
        cLib_addCalcAngleS2(&i_this->current.angle.x, angleY, 10,
                            (s16)gabi::ftoi(gabi::fmadds(REG0_F(2), 10.0f, 500.0f) * i_this->mSpeedLerp));
        offs->x = 0.0f;
        offs->y = 0.0f;
        offs->z = i_this->mSpeedFwd;
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        MtxPosition(offs.get(), &i_this->mSpeedVel);
        i_this->current.pos.x = i_this->current.pos.x + i_this->mSpeedVel.x;
        i_this->current.pos.y = i_this->current.pos.y + i_this->mSpeedVel.y;
        i_this->current.pos.z = i_this->current.pos.z + i_this->mSpeedVel.z;
        if (!(i_this->mLiftYTimer < 0.0f))
            dispWing = true;
        ret = 2;
        break;
    case 8:
        cLib_addCalcAngleS2(&i_this->current.angle.y, angleX, 10,
                            (s16)gabi::ftoi(gabi::fmadds(REG0_F(3), 10.0f, 1500.0f) * i_this->mSpeedLerp));
        cLib_addCalc0(&i_this->mSpeedLerp, 1.0f, REG0_F(4) + 0.05f);
        cLib_addCalc0(&i_this->mSpeedFwd, 1.0f, REG0_F(5) + 1.0f);
        offs->x = 0.0f;
        offs->y = 0.0f;
        offs->z = i_this->mSpeedFwd;
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        MtxPosition(offs.get(), pt.get());
        i_this->current.pos.x = i_this->current.pos.x + pt->x;
        i_this->current.pos.z = i_this->current.pos.z + pt->z;
        cLib_addCalc2(&i_this->current.pos.y, i_this->mGroundY, REG0_F(6) + 0.3f, REG0_F(7) + 20.0f);
        if (fabsf_(i_this->current.pos.y - i_this->mGroundY) < 1.0f) {
            i_this->current.pos.y = i_this->mGroundY;
            i_this->mMode = 10;
        }
        dispWing = true;
        ret = 1;
        break;
    case 2:
        fcopy(&i_this->mTargetPos.x, gabi::at<be<f32>>(gabi::ea(player) + 0x3D8));
        fcopy(&i_this->mTargetPos.z, gabi::at<be<f32>>(gabi::ea(player) + 0x3E0));
        i_this->mTargetPos.y = headY + 200.0f;
        dist = std_sqrtf(gabi::fmadds(vy, vy, vx * vx) + vz * vz);
        if (dist < gabi::fmadds(REG0_F(1), 10.0f, 800.0f)) {
            i_this->mMode = 9;
        }
        goto case_1_tail;
    case 9:
        fcopy(&i_this->mTargetPos.x, gabi::at<be<f32>>(gabi::ea(player) + 0x3D8));
        fcopy(&i_this->mTargetPos.z, gabi::at<be<f32>>(gabi::ea(player) + 0x3E0));
        i_this->mTargetPos.y = headY + 100.0f;
        cLib_addCalcAngleS2(&i_this->current.angle.y, angleX, 10,
                            (s16)gabi::ftoi(gabi::fmadds(REG0_F(3), 10.0f, 1500.0f) * i_this->mSpeedLerp));
        cLib_addCalc0(&i_this->mSpeedLerp, 1.0f, REG0_F(4) + 0.05f);
        cLib_addCalc0(&i_this->mSpeedFwd, 1.0f, REG0_F(5) + 1.0f);
        offs->x = 0.0f;
        offs->y = 0.0f;
        offs->z = i_this->mSpeedFwd;
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        MtxPosition(offs.get(), pt.get());
        i_this->current.pos.x = i_this->current.pos.x + pt->x;
        i_this->current.pos.z = i_this->current.pos.z + pt->z;
        cLib_addCalc2(&i_this->current.pos.y, i_this->mTargetPos.y, REG0_F(6) + 0.5f, REG0_F(7) + 20.0f);
        if (fabsf_(i_this->current.pos.y - i_this->mTargetPos.y) < 1.0f) {
            i_this->mSpeedLerp = 0.0f;
            i_this->mMode = 20;
        }
        dispWing = true;
        ret = 1;
        break;
    case 20:
        fcopy(&i_this->mTargetPos.x, gabi::at<be<f32>>(gabi::ea(player) + 0x3D8));
        fcopy(&i_this->mTargetPos.y, gabi::at<be<f32>>(gabi::ea(player) + 0x3DC));
        fcopy(&i_this->mTargetPos.z, gabi::at<be<f32>>(gabi::ea(player) + 0x3E0));
        cLib_addCalc2(&i_this->current.pos.x, headX, 1.0f, i_this->mSpeedLerp);
        cLib_addCalc2(&i_this->current.pos.y, i_this->mTargetPos.y, 1.0f, 0.5f * i_this->mSpeedLerp);
        cLib_addCalc2(&i_this->current.pos.z, i_this->mTargetPos.z, 1.0f, i_this->mSpeedLerp);
        cLib_addCalc2(&i_this->mSpeedLerp, 1000.0f, 1.0f, REG0_F(16) + 10.0f);
        cLib_addCalcAngleS2(&i_this->current.angle.y, player->shape_angle.y, 2, 0x1000);
        if (fabsf_(i_this->current.pos.y - i_this->mTargetPos.y) > 1.0f)
            dispWing = true;
        if (CPad_CHECK_TRIG_LEFT(0)) {
            kt_reset(i_this);
        }
        break;
    case 10: {
        f32 liftY = i_this->mLiftY + i_this->mLiftYTimer;
        i_this->mLiftY = liftY;
        i_this->mLiftYTimer = i_this->mLiftYTimer - gabi::fmadds(REG0_F(8), 0.1f, 5.0f);
        if (!(liftY > 0.0f)) {
            i_this->mLiftYTimer = REG0_F(9) + 15.0f;
            i_this->mLiftY = 0.0f;
        }
        cLib_addCalcAngleS2(&i_this->current.angle.y, angleX, 10, (s16)gabi::ftoi(gabi::fmadds(REG0_F(10), 10.0f, 500.0f)));
        if (i_this->mTimer[1] == 0 && !(i_this->mLiftY > 0.0f)) {
            i_this->mTimer[1] = (s16)gabi::ftoi(cM_rndF(20.0f) + 20.0f);
            i_this->mMode = 11;
        }
        offs->x = 0.0f;
        offs->y = 0.0f;
        offs->z = REG0_F(11) + 10.0f;
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        MtxPosition(offs.get(), &i_this->mSpeedVel);
        i_this->current.pos.x = i_this->current.pos.x + i_this->mSpeedVel.x;
        i_this->current.pos.z = i_this->current.pos.z + i_this->mSpeedVel.z;
        goto calc_11;
    }
    case 11:
        if (i_this->mTimer[0] == 0) {
            f32 r = cM_rndF(2.0f) + 10.0f;
            i_this->mTimer[0] = (s16)gabi::ftoi(r + REG0_F(12));
        }
        if (i_this->mTimer[0] > (s16)(REG0_S(1) + 8)) {
            cLib_addCalcAngleS2(&i_this->mAngleRoll, REG0_S(2) + 0x3000, 2, REG0_S(3) + 0x2000);
        }
        if (i_this->mTimer[1] == 0) {
            i_this->mTimer[1] = (s16)gabi::ftoi(cM_rndF(50.0f) + 20.0f);
            i_this->mMode = 10;
            f32 r = cM_rndFX(1000.0f);
            i_this->mTargetPos.x = r + i_this->mTargetPosHome.x;
            r = cM_rndFX(1000.0f);
            i_this->mTargetPos.z = r + i_this->mTargetPosHome.z;
        }
    calc_11:
        i_this->current.pos.y = i_this->current.pos.y - 5.0f;
        if (!i_this->mHitGround || dist_xz < gabi::fmadds(REG0_F(15), 100.0f, 1500.0f)) {
            kt_reset(i_this);
        }
        break;
    }

    i_this->mHitGround = false;
    if (i_this->mMode >= 8) {
        cXyz* gp = gabi::at<cXyz>(gabi::ea(gndChk.get()) + 0x24);
        fcopy(&gp->x, &i_this->current.pos.x);
        gp->y = i_this->current.pos.y + 1000.0f;
        fcopy(&gp->z, &i_this->current.pos.z);
        f32 gy = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
        i_this->mGroundY = gy;
        if (!(i_this->current.pos.y > gy)) {
            i_this->current.pos.y = gy;
            i_this->mHitGround = true;
        }
    }

    if (ret == 2) {
        f32 liftY = i_this->mLiftY + i_this->mLiftYTimer;
        i_this->mLiftY = liftY;
        i_this->mLiftYTimer = i_this->mLiftYTimer - 1.5f;
        if (!(liftY > 0.0f)) {
            i_this->mLiftYTimer = cM_rndF(5.0f) + 15.0f;
        }
    } else if (ret == 1) {
        cLib_addCalc0(&i_this->mLiftY, 1.0f, 2.0f);
        i_this->mLiftYTimer = 0.0f;
    }

    if (dispWing) {
        i_this->mWingScale = cM_ssin(i_this->mWingTimer);
    } else {
        i_this->mWingScale = 100.0f;
    }
    dBgS_GndChk_dt(gndChk.get());
    return TRUE;
}
VERIFY(0x021AB148, daKt_Execute);

/* 021AC208 */
static BOOL daKt_IsDelete(kt_class* i_this) {
    WWHD_FUNC(0x021AC208, BOOL, i_this);
    return TRUE;
}
VERIFY(0x021AC208, daKt_IsDelete);

/* 021AC210 */
static BOOL daKt_Delete(kt_class* i_this) {
    WWHD_FUNC(0x021AC210, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x100138C8) /* "Kt" */); /* dComIfG_resDeleteDemo */
    return TRUE;
}
VERIFY(0x021AC210, daKt_Delete);

/* 021AC240 */
static BOOL daKt_solidHeapCB(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x021AC240, BOOL, i_ac);
    kt_class* i_this = (kt_class*)i_ac;
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x100138CB), dRes_INDEX_KT_BMD_KT_MODEL_e, SAFESTRING_VTBL);
    i_this->mpModel = mDoExt_J3DModel__create(modelData, 0x10000, 0x11020203);
    J3DModelData* modelDataWing = (J3DModelData*)dComIfG_getObjectRes(STR(0x100138CB), dRes_INDEX_KT_BMD_KT_HANE_e, SAFESTRING_VTBL);
    i_this->mpModelWing = mDoExt_J3DModel__create(modelDataWing, 0x10000, 0x11020203);
    return modelData != nullptr && modelDataWing != nullptr && i_this->mpModel != nullptr && i_this->mpModelWing != nullptr;
}
VERIFY(0x021AC240, daKt_solidHeapCB);

/* 021AC30C */
static cPhs_State daKt_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x021AC30C, cPhs_State, i_ac);
    /* fopAcM_ct(i_ac, kt_class) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr) {
            fopAc_ac_c_ct(i_ac);
            i_ac->__vtbl = KT_VTBL;
        }
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }
    kt_class* i_this = (kt_class*)i_ac;

    cPhs_State rt = dComIfG_resLoad(&i_this->mPhs, STR(0x100138D4) /* "Kt" */);
    if (rt == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, 0x021AC240 /* daKt_solidHeapCB */, 0)) {
            s32 num = fopAcM_GetParam(i_this);
            if (num < 1000) {
                i_this->mParameters = 1000; /* fopAcM_SetParam */
                i_this->current.pos.y = gabi::fmadds(REG0_F(0), 10.0f, 2500.0f);
                for (s32 i = 0; i < num; i++) {
                    u8* params = fopAcM_CreateAppend();
                    u32 p = gabi::ea(params);
                    fcopy(gabi::at<be<f32>>(p + 4), &i_this->current.pos.x);
                    fcopy(gabi::at<be<f32>>(p + 8), &i_this->current.pos.y);
                    fcopy(gabi::at<be<f32>>(p + 0xC), &i_this->current.pos.z);
                    gabi::store<s16>(p + 0x10, 0);
                    gabi::store<s16>(p + 0x12, 0);
                    gabi::store<s16>(p + 0x14, 0);
                    gabi::store<u32>(p + 0, 1001 + i);
                    fpcM_Create(0xB8 /* fpcNm_KT_e */, params);
                }
            }

            J3DModel_setBaseScale_bits(i_this->mpModel, &i_this->scale);
            J3DModel_setBaseScale_bits(i_this->mpModelWing, &i_this->scale);
            f32 y = i_this->current.pos.y + 2500.0f;
            i_this->current.pos.y = y;
            fcopy(&i_this->mHomePos.x, &i_this->current.pos.x);
            i_this->mHomePos.y = y;
            fcopy(&i_this->mHomePos.z, &i_this->current.pos.z);
            i_this->mTargetPosHome.copy(i_this->mHomePos);
            i_this->mTimer[2] = REG0_S(5) + 500;
        } else {
            rt = cPhs_ERROR_e;
        }
    }
    return rt;
}
VERIFY(0x021AC30C, daKt_Create);

/* 021AC560 */
static void __sinit_d_a_kt_cpp() {
    WWHD_FUNC(0x021AC560, void, (u32)0);
    sinit_header_statics(0x10464C90, 0x101B89AC);
}
VERIFY(0x021AC560, __sinit_d_a_kt_cpp);

/* 021AC5F4: sead::SafeString deleting destructor (this TU's vtable slot) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021AC5F4, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x021AC5F4, SafeString_dt);

/* 021AC608: kt_class deleting destructor (compiler-generated, vtable +0xC; the matcher calls it
 * J3DFrameCtrl::~J3DFrameCtrl) */
static void kt_class_dt(kt_class* i_this, s32 flags) {
    WWHD_FUNC(0x021AC608, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021AC608, kt_class_dt);

/* 021AC65C: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x021AC65C, void, (u32)0);
}
VERIFY(0x021AC65C, SafeString_assureTerminationImpl);
