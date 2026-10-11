/** Unused small bird. Tww names and structured motion, preserving verified HD behavior. */
#include "d/actor/d_a_kt.h"
const gc_tu_data gc_tu = {
    .safeString = 0x100137DC,
    .gndChk = {0x10013804, 0x10013814, 0x10013834, 0x10013824},
    .gndChkDt = {0x10013814, 0x10013834, 0x100137F4},
    .headerStatics = {0x10464C90, 0x101B89AC, 0x10464C9C},
};
static f32 kt_scale = 1.5f;
// HD: keep fused arithmetic, operand order, unordered comparisons and word copies below.
static inline f32 fabsf_(f32 v) { return v < 0.0f ? -v : (v == 0.0f ? 0.0f * v + 0.0f : v); }

/* 021AAF50 */
static BOOL daKt_Draw(kt_class* i_this) {
    kt_scale = REG0_F(0) + 1.0f;
    MtxTrans(i_this->current.pos.x, i_this->current.pos.y + i_this->mLiftY, i_this->current.pos.z,
             false);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->mAngleRoll);
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
    f32 scaleMag = 0.2f * kt_scale;
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

/* the reset of the bird (cases 11 and 20 of kotori_move) */
static inline void kt_reset(kt_class* i_this) {
    i_this->mMode = KT_FLY;
    i_this->mTimer[0] = 0;
    f32 r = cM_rndFX(10.0f);
    i_this->mLiftYTimer = r + 10.0f;
    i_this->mTargetPosHome.y = i_this->mTargetPosHome.y + 2000.0f;
    i_this->current.angle.x = -0x2000;
    i_this->mTimer[2] = REG0_S(5) + 300;
}

/* 021AB148 */
static BOOL daKt_Execute(kt_class* i_this) {
    i_this->mWingTimer = i_this->mWingTimer + REG0_S(0) + 0x5e76;
    for (s32 i = 0; i < 3; i++)
        if (i_this->mTimer[i] != 0)
            i_this->mTimer[i] = i_this->mTimer[i] - 1;

    /* ---- kotori_move ---- */
    daPy_py_c* player = static_cast<daPy_py_c*>(dComIfGp_getPlayer(0));
    bool dispWing = false;
    u8 ret = 0;
    dBgS_GndChk gndChk;

    f32 dx = player->current.pos.x - i_this->current.pos.x;
    f32 dz = player->current.pos.z - i_this->current.pos.z;
    f32 dist_xz = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    cLib_addCalcAngleS2(&i_this->mAngleRoll, 0, 2, REG0_S(4) + 0x1000);

    f32 vx = i_this->mTargetPos.x - i_this->current.pos.x;
    f32 vy = i_this->mTargetPos.y - i_this->current.pos.y;
    f32 vz = i_this->mTargetPos.z - i_this->current.pos.z;
    s16 angleX = cM_atan2s(vx, vz);
    s16 angleY = -cM_atan2s(vy, std_sqrtf(vx * vx + vz * vz));

    cXyz offs;
    cXyz pt;
    s8 mode = i_this->mMode;
    /* player->getHeadTopPos() (HD: +0x3D8) */
    f32 headX = player->mHeadTopPos.x;
    f32 headY = player->mHeadTopPos.y;
    [[maybe_unused]] f32 headZ = player->mHeadTopPos.z;
    f32 dist;

    bool moveToTarget = false;
    bool finishGroundMove = false;
    switch (mode) {
    case KT_FLY:
        i_this->mSpeedFwd = gabi::fmadds(kt_scale, 20.0f, 30.0f);
        if (CPad_CHECK_TRIG_LEFT(0) && (s32)fopAcM_GetParam(i_this) == 1000) {
            kt_copyFloatBits(&i_this->mTargetPos.x, &player->current.pos.x);
            i_this->mTargetPos.y = player->current.pos.y + 500.0f;
            kt_copyFloatBits(&i_this->mTargetPos.z, &player->current.pos.z);
            i_this->mMode = KT_APPROACH_PLAYER;
            i_this->mSpeedLerp = 0.0f;
            break;
        }

        if (i_this->mTimer[2] == 0) {
            i_this->mMode = KT_APPROACH_GROUND;
            offs.x = 0.0f;
            offs.y = 0.0f;
            offs.z = gabi::fmadds(REG0_F(13), 100.0f, 3000.0f);
            cMtx_YrotS(calc_mtx(), player->shape_angle.y);
            MtxPosition(&offs, &pt);
            f32 r = REG0_F(14) + 200.0f;
            f32 a = player->current.pos.x + pt.x;
            r = cM_rndFX(r);
            i_this->mTargetPosHome.x = a + r;
            i_this->mTargetPosHome.y = player->current.pos.y + 1000.0f;
            r = cM_rndFX(REG0_F(14) + 200.0f);
            i_this->mTargetPosHome.z = player->current.pos.z + pt.z + r;

            gndChk.m_pos.x = i_this->mTargetPosHome.x;
            gndChk.m_pos.y = i_this->mTargetPosHome.y;
            gndChk.m_pos.z = i_this->mTargetPosHome.z;
            i_this->mTargetPosHome.y = cBgS_GroundCross(dComIfG_Bgsp(), &gndChk);
            if (i_this->mTargetPosHome.y == -1000000000.0f /* -G_CM3D_F_INF */) {
                kt_copyFloatBits(&i_this->mTargetPosHome.x, &player->current.pos.x);
                kt_copyFloatBits(&i_this->mTargetPosHome.y, &player->current.pos.y);
                kt_copyFloatBits(&i_this->mTargetPosHome.z, &player->current.pos.z);
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
        moveToTarget = true;
        break;
    case KT_APPROACH_GROUND:
        dist = std_sqrtf(gabi::fmadds(vy, vy, vx * vx) + vz * vz);
        if (dist < gabi::fmadds(REG0_F(1), 10.0f, 800.0f)) {
            i_this->mMode = KT_LAND;
        }
        cLib_addCalc2(&i_this->mSpeedLerp, 3.0f, 1.0f, 0.1f);
        moveToTarget = true;
        break;
    case KT_LAND:
        cLib_addCalcAngleS2(
            &i_this->current.angle.y, angleX, 10,
            (s16)gabi::ftoi(gabi::fmadds(REG0_F(3), 10.0f, 1500.0f) * i_this->mSpeedLerp));
        cLib_addCalc0(&i_this->mSpeedLerp, 1.0f, REG0_F(4) + 0.05f);
        cLib_addCalc0(&i_this->mSpeedFwd, 1.0f, REG0_F(5) + 1.0f);
        offs.x = 0.0f;
        offs.y = 0.0f;
        offs.z = i_this->mSpeedFwd;
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        MtxPosition(&offs, &pt);
        i_this->current.pos.x = i_this->current.pos.x + pt.x;
        i_this->current.pos.z = i_this->current.pos.z + pt.z;
        cLib_addCalc2(&i_this->current.pos.y, i_this->mGroundY, REG0_F(6) + 0.3f,
                      REG0_F(7) + 20.0f);
        if (fabsf_(i_this->current.pos.y - i_this->mGroundY) < 1.0f) {
            i_this->current.pos.y = i_this->mGroundY;
            i_this->mMode = KT_HOP;
        }
        dispWing = true;
        ret = 1;
        break;
    case KT_APPROACH_PLAYER:
        kt_copyFloatBits(&i_this->mTargetPos.x, &player->mHeadTopPos.x);
        kt_copyFloatBits(&i_this->mTargetPos.z, &player->mHeadTopPos.z);
        i_this->mTargetPos.y = headY + 200.0f;
        dist = std_sqrtf(gabi::fmadds(vy, vy, vx * vx) + vz * vz);
        if (dist < gabi::fmadds(REG0_F(1), 10.0f, 800.0f)) {
            i_this->mMode = KT_APPROACH_HEAD;
        }
        cLib_addCalc2(&i_this->mSpeedLerp, 3.0f, 1.0f, 0.1f);
        moveToTarget = true;
        break;
    case KT_APPROACH_HEAD:
        kt_copyFloatBits(&i_this->mTargetPos.x, &player->mHeadTopPos.x);
        kt_copyFloatBits(&i_this->mTargetPos.z, &player->mHeadTopPos.z);
        i_this->mTargetPos.y = headY + 100.0f;
        cLib_addCalcAngleS2(
            &i_this->current.angle.y, angleX, 10,
            (s16)gabi::ftoi(gabi::fmadds(REG0_F(3), 10.0f, 1500.0f) * i_this->mSpeedLerp));
        cLib_addCalc0(&i_this->mSpeedLerp, 1.0f, REG0_F(4) + 0.05f);
        cLib_addCalc0(&i_this->mSpeedFwd, 1.0f, REG0_F(5) + 1.0f);
        offs.x = 0.0f;
        offs.y = 0.0f;
        offs.z = i_this->mSpeedFwd;
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        MtxPosition(&offs, &pt);
        i_this->current.pos.x = i_this->current.pos.x + pt.x;
        i_this->current.pos.z = i_this->current.pos.z + pt.z;
        cLib_addCalc2(&i_this->current.pos.y, i_this->mTargetPos.y, REG0_F(6) + 0.5f,
                      REG0_F(7) + 20.0f);
        if (fabsf_(i_this->current.pos.y - i_this->mTargetPos.y) < 1.0f) {
            i_this->mSpeedLerp = 0.0f;
            i_this->mMode = KT_PERCH;
        }
        dispWing = true;
        ret = 1;
        break;
    case KT_PERCH:
        kt_copyFloatBits(&i_this->mTargetPos.x, &player->mHeadTopPos.x);
        kt_copyFloatBits(&i_this->mTargetPos.y, &player->mHeadTopPos.y);
        kt_copyFloatBits(&i_this->mTargetPos.z, &player->mHeadTopPos.z);
        cLib_addCalc2(&i_this->current.pos.x, headX, 1.0f, i_this->mSpeedLerp);
        cLib_addCalc2(&i_this->current.pos.y, i_this->mTargetPos.y, 1.0f,
                      0.5f * i_this->mSpeedLerp);
        cLib_addCalc2(&i_this->current.pos.z, i_this->mTargetPos.z, 1.0f, i_this->mSpeedLerp);
        cLib_addCalc2(&i_this->mSpeedLerp, 1000.0f, 1.0f, REG0_F(16) + 10.0f);
        cLib_addCalcAngleS2(&i_this->current.angle.y, player->shape_angle.y, 2, 0x1000);
        if (fabsf_(i_this->current.pos.y - i_this->mTargetPos.y) > 1.0f)
            dispWing = true;
        if (CPad_CHECK_TRIG_LEFT(0)) {
            kt_reset(i_this);
        }
        break;
    case KT_HOP: {
        f32 liftY = i_this->mLiftY + i_this->mLiftYTimer;
        i_this->mLiftY = liftY;
        i_this->mLiftYTimer = i_this->mLiftYTimer - gabi::fmadds(REG0_F(8), 0.1f, 5.0f);
        if (!(liftY > 0.0f)) {
            i_this->mLiftYTimer = REG0_F(9) + 15.0f;
            i_this->mLiftY = 0.0f;
        }
        cLib_addCalcAngleS2(&i_this->current.angle.y, angleX, 10,
                            (s16)gabi::ftoi(gabi::fmadds(REG0_F(10), 10.0f, 500.0f)));
        if (i_this->mTimer[1] == 0 && !(i_this->mLiftY > 0.0f)) {
            i_this->mTimer[1] = (s16)gabi::ftoi(cM_rndF(20.0f) + 20.0f);
            i_this->mMode = KT_WAIT;
        }
        offs.x = 0.0f;
        offs.y = 0.0f;
        offs.z = REG0_F(11) + 10.0f;
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        MtxPosition(&offs, &i_this->mSpeedVel);
        i_this->current.pos.x = i_this->current.pos.x + i_this->mSpeedVel.x;
        i_this->current.pos.z = i_this->current.pos.z + i_this->mSpeedVel.z;
        finishGroundMove = true;
        break;
    }
    case KT_WAIT:
        if (i_this->mTimer[0] == 0) {
            f32 r = cM_rndF(2.0f) + 10.0f;
            i_this->mTimer[0] = (s16)gabi::ftoi(r + REG0_F(12));
        }
        if (i_this->mTimer[0] > (s16)(REG0_S(1) + 8)) {
            cLib_addCalcAngleS2(&i_this->mAngleRoll, REG0_S(2) + 0x3000, 2, REG0_S(3) + 0x2000);
        }
        if (i_this->mTimer[1] == 0) {
            i_this->mTimer[1] = (s16)gabi::ftoi(cM_rndF(50.0f) + 20.0f);
            i_this->mMode = KT_HOP;
            f32 r = cM_rndFX(1000.0f);
            i_this->mTargetPos.x = r + i_this->mTargetPosHome.x;
            r = cM_rndFX(1000.0f);
            i_this->mTargetPos.z = r + i_this->mTargetPosHome.z;
        }
        finishGroundMove = true;
        break;
    }

    if (moveToTarget) {
        cLib_addCalcAngleS2(
            &i_this->current.angle.y, angleX, 10,
            (s16)gabi::ftoi(gabi::fmadds(REG0_F(2), 10.0f, 500.0f) * i_this->mSpeedLerp));
        cLib_addCalcAngleS2(
            &i_this->current.angle.x, angleY, 10,
            (s16)gabi::ftoi(gabi::fmadds(REG0_F(2), 10.0f, 500.0f) * i_this->mSpeedLerp));
        offs.x = 0.0f;
        offs.y = 0.0f;
        offs.z = i_this->mSpeedFwd;
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        MtxPosition(&offs, &i_this->mSpeedVel);
        i_this->current.pos.x = i_this->current.pos.x + i_this->mSpeedVel.x;
        i_this->current.pos.y = i_this->current.pos.y + i_this->mSpeedVel.y;
        i_this->current.pos.z = i_this->current.pos.z + i_this->mSpeedVel.z;
        if (!(i_this->mLiftYTimer < 0.0f))
            dispWing = true;
        ret = 2;
    }
    if (finishGroundMove) {
        i_this->current.pos.y = i_this->current.pos.y - 5.0f;
        if (!i_this->mHitGround || dist_xz < gabi::fmadds(REG0_F(15), 100.0f, 1500.0f)) {
            kt_reset(i_this);
        }
    }

    i_this->mHitGround = false;
    if (i_this->mMode >= KT_LAND) {
        cXyz* gp = &gndChk.m_pos;
        kt_copyFloatBits(&gp->x, &i_this->current.pos.x);
        gp->y = i_this->current.pos.y + 1000.0f;
        kt_copyFloatBits(&gp->z, &i_this->current.pos.z);
        f32 gy = cBgS_GroundCross(dComIfG_Bgsp(), &gndChk);
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
    return TRUE;
}

/* 021AC208 */
static BOOL daKt_IsDelete(kt_class* i_this) { return TRUE; }

/* 021AC210 */
static BOOL daKt_Delete(kt_class* i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Kt"); /* dComIfG_resDeleteDemo */
    return TRUE;
}

/* 021AC240 */
static BOOL daKt_solidHeapCB(fopAc_ac_c* i_ac) {
    kt_class* i_this = (kt_class*)i_ac;
    J3DModelData* modelData =
        (J3DModelData*)dComIfG_getObjectRes("Kt", dRes_INDEX_KT_BMD_KT_MODEL_e, gc_tu.safeString);
    i_this->mpModel = mDoExt_J3DModel__create(modelData, 0x10000, 0x11020203);
    J3DModelData* modelDataWing =
        (J3DModelData*)dComIfG_getObjectRes("Kt", dRes_INDEX_KT_BMD_KT_HANE_e, gc_tu.safeString);
    i_this->mpModelWing = mDoExt_J3DModel__create(modelDataWing, 0x10000, 0x11020203);
    return modelData != nullptr && modelDataWing != nullptr && i_this->mpModel != nullptr &&
           i_this->mpModelWing != nullptr;
}

/* 021AC30C */
static cPhs_State daKt_Create(fopAc_ac_c* i_ac) {
    /* fopAcM_ct(i_ac, kt_class) */
    if (!fopAcM_CheckCondition(i_ac, fopAcCnd_INIT_e)) {
        if (i_ac != nullptr) {
            fopAc_ac_c_ct(i_ac);
            i_ac->__vtbl = 0x10013844;
        }
        fopAcM_OnCondition(i_ac, fopAcCnd_INIT_e);
    }
    kt_class* i_this = (kt_class*)i_ac;

    cPhs_State rt = dComIfG_resLoad(&i_this->mPhs, "Kt");
    if (rt == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, gabi::guest_fn((void*)daKt_solidHeapCB), 0)) {
            s32 num = fopAcM_GetParam(i_this);
            if (num < 1000) {
                i_this->mParameters = 1000; /* fopAcM_SetParam */
                i_this->current.pos.y = gabi::fmadds(REG0_F(0), 10.0f, 2500.0f);
                for (s32 i = 0; i < num; i++) {
                    KtAppend* params = fopAcM_CreateAppend();
                    kt_copyFloatBits(&params->base.position.x, &i_this->current.pos.x);
                    kt_copyFloatBits(&params->base.position.y, &i_this->current.pos.y);
                    kt_copyFloatBits(&params->base.position.z, &i_this->current.pos.z);
                    params->base.angle.x = 0;
                    params->base.angle.y = 0;
                    params->base.angle.z = 0;
                    params->base.parameters = 1001 + i;
                    layer_class* layer = fpcLy_CurrentLayer();
                    fpcSCtRq_Request(layer, fpcNm_KT_e, 0, 0, params);
                }
            }

            i_this->mpModel->mBaseScale.copy(i_this->scale);
            i_this->mpModelWing->mBaseScale.copy(i_this->scale);
            f32 y = i_this->current.pos.y + 2500.0f;
            i_this->current.pos.y = y;
            kt_copyFloatBits(&i_this->mHomePos.x, &i_this->current.pos.x);
            i_this->mHomePos.y = y;
            kt_copyFloatBits(&i_this->mHomePos.z, &i_this->current.pos.z);
            i_this->mTargetPosHome.copy(i_this->mHomePos);
            i_this->mTimer[2] = REG0_S(5) + 500;
        } else {
            rt = cPhs_ERROR_e;
        }
    }
    return rt;
}

// HD: compiler-generated initialization and deleting destructors.
/* 021AC560 */
static void __sinit_d_a_kt_cpp() { __sinit_header_statics(); }

/* 021AC5F4 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 021AC608 */
static void kt_class_dt(kt_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        kt_actor_dt(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021AC65C */
static void SafeString_assureTerminationImpl(void*) {}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): per-TU statics and external APIs
str 100138C8 "Kt" in 021AC210
str 100138CB "Kt" in 021AC240
str 100138D4 "Kt" in 021AC30C
data kt_scale 101B8988
ext 025E2DA8 void mDoExt_modelUpdate(J3DModel* model)
ext 020077BC BOOL CPad_CHECK_TRIG_LEFT(s32 pad)
ext 025D5600 KtAppend* fopAcM_CreateAppend()
ext 025E14A8 void fpcSCtRq_Request(layer_class* layer,s16 name,u32 first,u32 second,KtAppend* append)
ext 025D50BC void kt_actor_dt(fopAc_ac_c* actor,s32 flags)
*/
// clang-format on
