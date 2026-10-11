/**
 * d_a_mt_body.cpp (WWHD)
 * Enemy - Magtail: body control (segment chain) and the rolled-up movement.
 *
 * The GameCube decompilation has only "Nonmatching"
 * placeholders for these functions: written from the WWHD code (cking.rpx) and verified against it.
 */
#include "d/actor/d_a_mt_body.h"
#include "gc_api/JSystem/JPAMath.h"
#include "gc_api/f_op/f_op_actor.h"
#include <cmath>

static inline Mtx34* mtx_now() { return mDoMtx_stack_c::get(); }

static u32 l_trailStart[8] = {0,          4294967290, 4294967284, 4294967278,
                              4294967272, 4294967266, 4294967260, 4294967254};
static u32 l_reverseTrailStart[8] = {0,          4294967293, 4294967290, 4294967287,
                                     4294967284, 4294967281, 4294967278, 4294967275};




/* body_control2: trail start indices (.data, per segment), two tables */
static inline u32 mt_trail_idx(u8 sel, u32 i) {
    return sel != 0 ? l_reverseTrailStart[i] : l_trailStart[i];
}



// HD: preserve GHS unsigned conversion above the signed range.
static inline u32 f2u(f32 f) {
    if (f < 2147483648.0f)
        return (u32)gabi::ftoi(f);
    return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}


/* 021DD634 */
void body_control2(mt_class* actor) {
    mtBodyActorView* i_this = reinterpret_cast<mtBodyActorView*>(actor);
    i_this->mSegmentPositions[0].copy(i_this->current.pos);
    i_this->mSegmentAngles[0].x = i_this->shape_angle.x;
    i_this->mSegmentAngles[0].y = i_this->shape_angle.y;
    i_this->mSegmentAngles[0].z = i_this->shape_angle.z;
    f32 spdScale = i_this->mSegmentSpeedScale;
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk, MT_LINCHK_VTBLS);
    gabi::Local<cXyz> wave;
    wave->x = 0.0f;
    wave->y = 0.0f;
    wave->z = 0.0f;
    gabi::Local<cXyz> vec;
    gabi::Local<cXyz> start;
    gabi::Local<cXyz> side1;
    gabi::Local<cXyz> side2;
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> out;
    for (int i = 0; i < MT_PART_NUM; i++) {
        cXyz* pos = &i_this->mSegmentPositions[i];
        csXyz* ang = &i_this->mSegmentAngles[i];
        cXyz* spd = &i_this->mSegmentSpeeds[i];
        if (i > 0) {
            start->x = pos->x;
            start->y = pos->y + 50.0f;
            start->z = pos->z;
            u8 hits = 0;
            mDoMtx_YrotS(calc_mtx(), ang->y);
            vec->x = 3.0f;
            vec->y = -200.0f;
            vec->z = 0.0f;
            MtxPosition(vec, side1);
            PSVECAdd(side1, pos, side1);
            dBgS_LinChk_Set(linChk, start, side1, i_this);
            if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                side1->copy(bodyLineCross(linChk)); /* GetCross() */
                hits = 1;
            }
            vec->x = -vec->x;
            MtxPosition(vec, side2);
            PSVECAdd(side2, pos, side2);
            dBgS_LinChk_Set(linChk, start, side2, i_this);
            if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                side2->copy(bodyLineCross(linChk));
                hits = hits + 1;
            }
            f32 y = pos->y - 10.0f;
            s16 roll = 0;
            if (hits == 2) {
                f32 floorY = side1->y + bodyHIO().mGroundClearance;
                if (y < floorY) {
                    y = floorY;
                    cXyz_mi(side1, diff, side2);
                    f32 dx = diff->x, dy = diff->y, dz = diff->z;
                    vec->x = dx;
                    vec->y = dy;
                    vec->z = dz;
                    roll = cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz)));
                }
            }
            cLib_addCalcAngleS2(&ang->z, roll, 2, 0x400);
            cXyz* prev = &i_this->mSegmentPositions[i - 1];
            f32 dy = y - prev->y + spd->y;
            if (i_this->mSuppressBodyWave == 0) {
                s32 a = i_this->mBodyWavePhase * (REG0_S(5) + 0x5DC) + i * (REG0_S(6) + 0x1D4C);
                vec->y = 0.0f;
                vec->z = REG0_F(3) + -5.0f;
                vec->x = cM_ssin(a) * 3.0f;
                mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
                MtxPosition(vec, wave);
            }
            f32 dx = pos->x - prev->x + spd->x + wave->x;
            f32 dz = pos->z - prev->z + spd->z + wave->z;
            s16 yaw = cM_atan2s(dx, dz);
            f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
            s16 pitch = -cM_atan2s(dy, dist);
            vec->y = 0.0f;
            vec->x = 0.0f;
            vec->z = REG0_F(7) + 35.0f;
            mDoMtx_YrotS(calc_mtx(), yaw);
            mDoMtx_XrotM(calc_mtx(), pitch);
            MtxPosition(vec, out);
            ang->x = -pitch;
            ang->y = yaw + 0x8000;
            cXyz* old = &i_this->mPreviousSegmentPositions[i];
            old->copy(*pos);
            f32 nx = prev->x + out->x;
            pos->x = nx;
            pos->y = prev->y + out->y;
            pos->z = prev->z + out->z;
            spd->x = (nx - old->x) * spdScale;
            spd->y = (pos->y - old->y) * spdScale;
            spd->z = (pos->z - old->z) * spdScale;
        }
        J3DModel* model = i_this->mpMorf[i]->getModel();
        f32 sz = i_this->scale.z, sx = i_this->scale.x, sy = i_this->scale.y;
        model->mBaseScale.x = sx;
        model->mBaseScale.y = sy;
        model->mBaseScale.z = sz;
        mDoMtx_stack_c::transS(pos->x, pos->y, pos->z);
        mDoMtx_stack_c::YrotM(ang->y);
        mDoMtx_XrotM(mtx_now(), ang->x);
        mDoMtx_ZrotM(mtx_now(), ang->z);
        if (i == 0) {
            mDoMtx_stack_c::YrotM(i_this->mHeadWobbleAngle);
            mDoMtx_stack_c::scaleM(bodyHIO().mHeadScale, bodyHIO().mHeadScale,
                                   bodyHIO().mHeadScale);
        } else {
            f32 s = i_this->mSegmentScales[i];
            mDoMtx_stack_c::scaleM(s, s * i_this->mSegmentScaleY[i], 1.0f);
            if (i == 7) {
                mDoMtx_stack_c::scaleM(i_this->mHeadScale, i_this->mHeadScale, i_this->mHeadScale);
            }
        }
        mDoMtx_stack_c::transM(0.0f, 0.0f, i_this->mModelOffsetZ);
        J3DModel_setBaseTRMtx(model, mtx_now());
        dCcD_Sph* sph = &i_this->mSph[i];
        if (i == 0) {
            vec->x = 0.0f;
            vec->y = 0.0f;
            vec->z = REG0_F(9) + 30.0f;
            PSMTXMultVec(mtx_now(), vec, &i_this->eyePos);
            i_this->mAtSph.SetC(&i_this->eyePos);
            vec->x = 0.0f;
            vec->y = 0.0f;
            vec->z = REG_F(6, 9) + 100.0f;
            PSMTXMultVec(mtx_now(), vec, out);
            sph->SetC(out);
            sph->mObjAt.mSPrm = (sph->mObjAt.mSPrm & ~2u) | 8;
            u8 mode = i_this->mHeadColliderMode;
            if (mode == 1) {
                sph->OffAtSPrmBit(1);
                sph->OffCoSPrmBit(1);
                sph->OffTgSPrmBit(1);
                i_this->mAtSph.SetR(40.0f);
            } else {
                if (mode == 2) {
                    sph->OnAtSPrmBit(1);
                } else {
                    sph->OffAtSPrmBit(1);
                }
                sph->OnCoSPrmBit(1);
                sph->OnTgSPrmBit(1);
                sph->SetR(bodyHIO().mHeadColliderRadius);
                i_this->mAtSph.SetR(bodyHIO().mAttackColliderRadius);
            }
            dComIfG_Ccsp_Set(&i_this->mAtSph);
            dComIfG_Ccsp_Set(sph);
        } else {
            sph->SetC(pos);
            if (i_this->mShrinkColliderTimer != 0) {
                sph->SetR(-200.0f);
            } else {
                sph->SetR(bodyHIO().mSegmentColliderRadius);
            }
            dComIfG_Ccsp_Set(sph);
        }
        if (i_this->mWriteTrail != 0 && i > 0) {
            u32 k = mt_trail_idx(i_this->mTrailTableSelect, i);
            cXyz* prev = &i_this->mSegmentPositions[i - 1];
            for (int j = 0; j < 6; j++, k++) {
                f32 fj = (f32)j;
                u32 idx = k & 0x3F;
                f32 px = pos->x;
                f32 stepX = (prev->x - px) / 5.0f;
                f32 stepY = (prev->y - pos->y) / 5.0f;
                i_this->mTrailPositions[idx].x = gabi::fmadds(stepX, fj, px);
                i_this->mTrailPositions[idx].y = gabi::fmadds(stepY, fj, pos->y);
                f32 stepZ = (prev->z - pos->z) / 5.0f;
                i_this->mTrailPositions[idx].z = gabi::fmadds(stepZ, fj, pos->z);
                i_this->mTrailAngles[idx].x = ang->x;
                i_this->mTrailAngles[idx].y = ang->y;
                i_this->mTrailAngles[idx].z = ang->z;
            }
        }
    }
    if (i_this->mWriteTrail != 0) {
        i_this->mAction = 0;
        i_this->mWriteTrail = 0;
        i_this->mActionTimer = 100;
        i_this->mSuppressBodyWave = 0;
        i_this->mRollMode = 0;
        i_this->mTrailState = 0;
        anm_init(actor, 10, 20.0f, 2, 1.0f, 0);
    }
    cLib_addCalc0(&i_this->mSegmentSpeedScale, 1.0f, 0.01f);
    cLib_addCalc2(&i_this->mModelOffsetZ, 20.0f, 1.0f, 1.0f);
    s32 a = i_this->mBodyWavePhase * (REG0_S(8) + 0x258);
    cLib_addCalcAngleS2(&i_this->current.angle.z,
                        (s16)gabi::ftoi(cM_ssin(a) * (REG0_F(5) + 2000.0f)), 2, 0x400);
    dBgS_LinChk_dt(linChk, MT_LINCHK_VTBLS);
}

/* 021DE1E0 */
void body_control3(mt_class* actor) {
    mtBodyActorView* i_this = reinterpret_cast<mtBodyActorView*>(actor);
    i_this->mSegmentPositions[0].copy(i_this->current.pos);
    i_this->mSegmentAngles[0].z = i_this->shape_angle.z;
    i_this->mSegmentAngles[0].x = i_this->shape_angle.x;
    i_this->mSegmentAngles[0].y = i_this->shape_angle.y;
    mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
    mDoMtx_XrotM(calc_mtx(), i_this->shape_angle.x);
    mDoMtx_XrotM(calc_mtx(), i_this->shape_angle.z); // HD: the original applies X rotation twice.
    gabi::Local<cXyz> vec;
    gabi::Local<cXyz> out2;
    gabi::Local<cXyz> out;
    gabi::Local<cXyz> offset;
    f32 amp = i_this->mBodyWaveScale;
    vec->x = 0.0f;
    vec->y = 32.0f * amp;
    vec->z = -17.0f * amp;
    MtxPosition(vec, offset);
    s16 pitchAdd = 0;
    if (i_this->mCrawlPitchFlags != 0) {
        pitchAdd = (s16)((i_this->mCrawlPitchFlags & 2) * 500);
    }
    s16 pitchAcc = 0;
    for (int i = 0; i < MT_PART_NUM; i++) {
        cXyz* pos = &i_this->mSegmentPositions[i];
        csXyz* ang = &i_this->mSegmentAngles[i];
        if (i > 0) {
            s32 a = i_this->mCrawlPhase * (REG0_S(5) + 0x1388) + i * (REG0_S(6) + 0x1B58);
            pitchAcc = (s16)(pitchAcc - (i_this->mSegmentPitchStep + pitchAdd));
            vec->y = 0.0f;
            vec->z = -i_this->mCrawlSegmentOffset;
            vec->x = cM_ssin(a) * i_this->mBodyWaveAmplitude;
            mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
            mDoMtx_XrotM(calc_mtx(), (s16)(i_this->shape_angle.x + pitchAcc));
            mDoMtx_ZrotM(calc_mtx(), i_this->shape_angle.z);
            MtxPosition(vec, out);
            cXyz* prev = &i_this->mSegmentPositions[i - 1];
            f32 ddy = pos->y - prev->y;
            f32 ddx = pos->x - prev->x;
            f32 ddz = pos->z - prev->z;
            f32 dx = ddx + out->x;
            f32 dz = ddz + out->z;
            f32 dy = ddy + out->y;
            s16 yaw = cM_atan2s(dx, dz);
            f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
            s16 pitch = -cM_atan2s(dy, dist);
            vec->x = 0.0f;
            vec->y = 0.0f;
            vec->z = REG0_F(7) + 35.0f;
            mDoMtx_YrotS(calc_mtx(), yaw);
            mDoMtx_XrotM(calc_mtx(), pitch);
            MtxPosition(vec, out2);
            s16 d = (s16)(yaw - i_this->shape_angle.y);
            if (d < 0)
                d = -d;
            if ((u16)d < 0x4000) {
                ang->y = yaw;
                ang->x = (s16)(pitch - 0x8000);
            } else {
                ang->y = (s16)(yaw + 0x8000);
                ang->x = -pitch;
            }
            i_this->mPreviousSegmentPositions[i].copy(*pos);
            pos->x = prev->x + out2->x;
            pos->y = prev->y + out2->y;
            pos->z = prev->z + out2->z;
        }
        J3DModel* model = i_this->mpMorf[i]->getModel();
        f32 sx = i_this->scale.x, sz = i_this->scale.z, sy = i_this->scale.y;
        model->mBaseScale.x = sx;
        model->mBaseScale.y = sy;
        model->mBaseScale.z = sz;
        mDoMtx_stack_c::transS(pos->x + offset->x, pos->y + offset->y, pos->z + offset->z);
        mDoMtx_stack_c::YrotM(ang->y);
        mDoMtx_XrotM(mtx_now(), ang->x);
        mDoMtx_ZrotM(mtx_now(), ang->z);
        if (i == 0) {
            mDoMtx_stack_c::scaleM(bodyHIO().mCrawlHeadScale, bodyHIO().mCrawlHeadScale,
                                   bodyHIO().mCrawlHeadScale);
        } else {
            f32 s = i_this->mSegmentScales[i];
            mDoMtx_stack_c::scaleM(s, s * i_this->mSegmentScaleY[i], 1.0f);
            if (i == 7) {
                mDoMtx_stack_c::scaleM(i_this->mHeadScale, i_this->mHeadScale, i_this->mHeadScale);
            }
        }
        mDoMtx_stack_c::transM(0.0f, 0.0f, i_this->mModelOffsetZ);
        J3DModel_setBaseTRMtx(model, mtx_now());
        if (i == 0) {
            i_this->eyePos.copy(i_this->current.pos);
            dCcD_Sph* sph = &i_this->mSph[0];
            sph->SetC(&i_this->current.pos);
            sph->OnAtSPrmBit(1);
            i_this->mAtSph.SetR(-30.0f);
            if (std::fabs((f32)i_this->speedF) > 2.0f) {
                sph->OnAtSPrmBit(0xA);
            } else {
                sph->OffAtSPrmBit(0xA);
            }
            if (i_this->mCrawlPhase >= 0x5A) {
                sph->OffAtSPrmBit(4);
                sph->SetR(40.0f);
            } else {
                sph->OnAtSPrmBit(4);
                sph->SetR(60.0f);
            }
            dComIfG_Ccsp_Set(sph);
        }
    }
    cLib_addCalc0(&i_this->mSegmentSpeedScale, 1.0f, 0.01f);
    cLib_addCalc2(&i_this->mModelOffsetZ, 20.0f, 1.0f, 1.0f);
    cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 2, 0x400);
}

/* 021DE888 */
void body_control4(mt_class* actor) {
    mtBodyActorView* i_this = reinterpret_cast<mtBodyActorView*>(actor);
    J3DModel* model = i_this->mpMorf[0]->getModel();
    mtBodyScaleMatrix(mtx_now(), 0.0f, 0.0f, 0.0f);
    J3DModel_setBaseTRMtx(model, mtx_now());
    for (int i = 1; i < MT_PART_NUM; i++) {
        cXyz* pos = &i_this->mSegmentPositions[i];
        csXyz* ang = &i_this->mSegmentAngles[i];
        cXyz* spd = &i_this->mSegmentSpeeds[i];
        if (i_this->mScatterDelay[i] != 0) {
            i_this->mScatterDelay[i] = i_this->mScatterDelay[i] - 1;
        } else {
            dComIfGp_particle_setSimple(0x8060, pos);
            pos->x = pos->x + spd->x;
            pos->y = pos->y + spd->y;
            pos->z = pos->z + spd->z;
            spd->y = spd->y - 2.5f;
            ang->x = ang->x + 0x1800;
            ang->y = ang->y + 0x1000;
            if (spd->y < 0.0f) {
                cLib_addCalc0(&i_this->mSegmentScales[i], 1.0f, 0.025f);
            }
        }
        model = i_this->mpMorf[i]->getModel();
        mDoMtx_stack_c::transS(pos->x, pos->y, pos->z);
        mDoMtx_stack_c::YrotM(ang->y);
        mDoMtx_XrotM(mtx_now(), ang->x);
        mDoMtx_ZrotM(mtx_now(), ang->z);
        mDoMtx_stack_c::scaleM(i_this->mSegmentScales[i], i_this->mSegmentScales[i],
                               i_this->mSegmentScales[i]);
        if (i == 7) {
            mDoMtx_stack_c::scaleM(0.0f, 0.0f, 0.0f);
        }
        J3DModel_setBaseTRMtx(model, mtx_now());
    }
}

/* 021DEBC8 */
void body_control5(mt_class* actor) {
    mtBodyActorView* i_this = reinterpret_cast<mtBodyActorView*>(actor);
    f32 groundY = i_this->mAcch.m_ground_h + bodyHIO().mGroundClearance;
    i_this->mSegmentPositions[0].copy(i_this->current.pos);
    i_this->mSegmentAngles[0].x = i_this->shape_angle.x;
    i_this->mSegmentAngles[0].y = i_this->shape_angle.y;
    i_this->mSegmentAngles[0].z = i_this->shape_angle.z;
    f32 rate = 0.2f;
    gabi::Local<cXyz> vec;
    gabi::Local<cXyz> out2;
    gabi::Local<cXyz> out;
    for (int i = 0; i < MT_PART_NUM; i++) {
        cXyz* pos = &i_this->mSegmentPositions[i];
        csXyz* ang = &i_this->mSegmentAngles[i];
        if (i > 0) {
            s32 a1 = i_this->mBodyWavePhase * (REG0_S(5) + 0x1194) - i * (REG0_S(6) + 0x1B58);
            f32 r1 = REG0_F(4) + 50.0f;
            s32 a2 = i_this->mBodyWavePhase * (REG0_S(7) + 0xDAC) - i * (REG0_S(8) + 0x1770);
            f32 r2 = REG0_F(5) + 80.0f;
            f32 amp = i_this->mBodyWaveScale;
            vec->x = cM_ssin(a1) * r1 * amp * rate;
            vec->y = cM_ssin(a2) * r2 * amp * rate;
            vec->z = REG0_F(3) + -30.0f;
            rate += 0.2f;
            mDoMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
            MtxPosition(vec, out);
            cXyz* prev = &i_this->mSegmentPositions[i - 1];
            f32 y = pos->y - 10.0f + out->y;
            if (y < groundY) {
                y = groundY;
            }
            f32 dx = pos->x - prev->x + out->x;
            f32 dz = pos->z - prev->z + out->z;
            f32 dy = y - prev->y;
            s16 yaw = cM_atan2s(dx, dz);
            f32 dist = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
            s16 pitch = -cM_atan2s(dy, dist);
            vec->y = 0.0f;
            vec->x = 0.0f;
            vec->z = REG0_F(7) + 35.0f;
            mDoMtx_YrotS(calc_mtx(), yaw);
            mDoMtx_XrotM(calc_mtx(), pitch);
            MtxPosition(vec, out2);
            ang->y = yaw + 0x8000;
            ang->x = -pitch;
            pos->x = prev->x + out2->x;
            pos->y = prev->y + out2->y;
            pos->z = prev->z + out2->z;
        }
        J3DModel* model = i_this->mpMorf[i]->getModel();
        f32 sz = i_this->scale.z, sx = i_this->scale.x, sy = i_this->scale.y;
        model->mBaseScale.x = sx;
        model->mBaseScale.y = sy;
        model->mBaseScale.z = sz;
        mDoMtx_stack_c::transS(pos->x, pos->y, pos->z);
        mDoMtx_stack_c::YrotM(ang->y);
        mDoMtx_XrotM(mtx_now(), ang->x);
        mDoMtx_ZrotM(mtx_now(), ang->z);
        if (i == 0) {
            mDoMtx_stack_c::YrotM(i_this->mHeadWobbleAngle);
            mDoMtx_stack_c::scaleM(bodyHIO().mHeadScale, bodyHIO().mHeadScale,
                                   bodyHIO().mHeadScale);
        } else {
            f32 s = i_this->mSegmentScales[i];
            mDoMtx_stack_c::scaleM(s, s * i_this->mSegmentScaleY[i], 1.0f);
            if (i == 7) {
                mDoMtx_stack_c::scaleM(i_this->mHeadScale, i_this->mHeadScale, i_this->mHeadScale);
            }
        }
        mDoMtx_stack_c::transM(0.0f, 0.0f, i_this->mModelOffsetZ);
        J3DModel_setBaseTRMtx(model, mtx_now());
        dCcD_Sph* sph = &i_this->mSph[i];
        if (i == 0) {
            vec->x = 0.0f;
            vec->y = 0.0f;
            vec->z = REG0_F(9) + 30.0f;
            PSMTXMultVec(mtx_now(), vec, &i_this->eyePos);
            i_this->mAtSph.SetC(&i_this->eyePos);
            i_this->mAtSph.SetR(30.0f);
            dComIfG_Ccsp_Set(&i_this->mAtSph);
            sph->OffAtSPrmBit(1);
            sph->OffCoSPrmBit(1);
            dComIfG_Ccsp_Set(sph);
        } else {
            sph->OffCoSPrmBit(1);
            sph->SetC(pos);
            sph->OffAtSPrmBit(1);
            sph->OffCoSPrmBit(1);
            dComIfG_Ccsp_Set(sph);
        }
    }
    cLib_addCalc2(&i_this->mModelOffsetZ, 20.0f, 1.0f, 1.0f);
    s32 a = i_this->mBodyWavePhase * (REG0_S(0) + 0xBB8);
    i_this->mHeadWobbleAngle =
        (s16)gabi::ftoi(cM_ssin(a) * (REG0_F(7) + 3000.0f) * i_this->mBodyWaveScale);
}

/* 021DF238 */
void mt_move_maru(mt_class* actor) {
    mtBodyActorView* i_this = reinterpret_cast<mtBodyActorView*>(actor);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    u8 mode = i_this->mRollMode;
    u32 se = 0;
    i_this->mRollingStateTimer = 3;
    if (mode == 0) {
        if (i_this->mRollCollisionTimer == 0) {
            i_this->mSph[0].OnCoSPrmBit(1);
        }
        i_this->shape_angle.x = i_this->shape_angle.x + (s16)gabi::ftoi(i_this->speedF * 200.0f);
        mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
        gabi::Local<cXyz> vec;
        gabi::Local<cXyz> move;
        vec->x = 0.0f;
        vec->y = 0.0f;
        vec->z = i_this->speedF;
        MtxPosition(vec, move);
        f32 vy = i_this->speed.y;
        f32 fall = vy + i_this->gravity;
        f32 mx = move->x;
        i_this->speed.x = mx;
        f32 nx = i_this->current.pos.x + mx;
        f32 ny = i_this->current.pos.y + vy;
        f32 mz = move->z;
        i_this->speed.z = mz;
        f32 nz = i_this->current.pos.z + mz;
        i_this->current.pos.x = nx;
        i_this->current.pos.y = ny;
        i_this->current.pos.z = nz;
        if (fall < -100.0f) {
            fall = -100.0f;
        }
        i_this->speed.y = fall;
        mt_bg_check(actor);
        if (i_this->mAcch.m_flags & dBgS_Acch::GROUND_HIT) {
            if (fall < REG0_F(12) + -50.0f) {
                i_this->mImpactState = 2;
            }
            f32 target = 0.0f;
            f32 step = 1.0f;
            gabi::Local<mtBodyGroundCheck> gnd;
            mt_GndChk_ct(gnd);
            gnd->m_pos.x = i_this->current.pos.x;
            gnd->m_pos.y = i_this->current.pos.y + 50.0f;
            gnd->m_pos.z = i_this->current.pos.z;
            f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), gnd);
            mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
            vec->x = 0.0f;
            vec->y = 50.0f;
            vec->z = 5.0f;
            gabi::Local<cXyz> ofs;
            MtxPosition(vec, ofs);
            gabi::Local<cXyz> ahead;
            cXyz_pl(&i_this->current.pos, ahead, ofs);
            gnd->m_pos.copy(*ahead);
            f32 aheadY = cBgS_GroundCross(dComIfG_Bgsp(), gnd);
            if (aheadY != -1000000000.0f) {
                if (aheadY < groundY - 1.0f) {
                    target = 5.0f;
                    step = 0.3f;
                } else if (aheadY > groundY + 1.0f) {
                    target = -5.0f;
                    step = 0.3f;
                }
            }
            cLib_addCalc2(&i_this->speedF, target, 1.0f, bodyHIO().mRollingFriction * step);
            if (fall < REG0_F(14) + -15.0f) {
                f32 bounce = fall * (REG0_F(15) + -0.4f);
                i_this->speed.y = bounce;
                se = f2u(bounce * (REG0_F(5) + 6.0f));
                if (se > 100)
                    se = 100;
            } else {
                i_this->speed.y = -5.0f;
            }
            mt_GndChk_dt(gnd);
        }
        if ((i_this->mAcch.m_flags & dBgS_Acch::WALL_HIT) &&
            std::fabs((f32)i_this->speedF) > 3.0f) {
            i_this->speedF = i_this->speedF * -0.5f;
            se = 0x32;
            fopAcM_seStart(i_this, 0x5815 /* JA_SE_CM_MT_BOUND */, se);
        } else if (se != 0) {
            fopAcM_seStart(i_this, 0x5815, se);
        }
        i_this->attention_info.flags |= 0x10;
        i_this->attention_info.distances[4] = 9;
        if (i_this->actor_status & fopAcStts_CARRY_e) {
            i_this->mRollMode = 1;
            i_this->attention_info.flags &= ~0x10u;
        }
        cLib_addCalcAngleS2(&i_this->mHeadWobbleAngle, 0, 1, 0x100);
    } else if (mode == 1) {
        i_this->current.angle.x = 0;
        i_this->current.angle.y = i_this->shape_angle.y;
        i_this->current.angle.z = i_this->shape_angle.z;
        i_this->mSph[0].OffCoSPrmBit(1);
        if (!(i_this->actor_status & fopAcStts_CARRY_e)) {
            /* thrown: if the line from the player to the ball hits a wall, it drops at the player
             */
            gabi::Local<dBgS_LinChk_l> linChk;
            dBgS_LinChk_ct(linChk, MT_LINCHK_VTBLS);
            gabi::Local<cXyz> start;
            start->x = player->current.pos.x;
            start->y = player->current.pos.y + 50.0f;
            start->z = player->current.pos.z;
            mDoMtx_YrotS(calc_mtx(), player->shape_angle.y);
            gabi::Local<cXyz> vec;
            vec->x = 0.0f;
            vec->y = 0.0f;
            vec->z = REG_F(18, 8) + 25.0f;
            gabi::Local<cXyz> ofs;
            MtxPosition(vec, ofs);
            gabi::Local<cXyz> end;
            end->x = i_this->current.pos.x + ofs->x;
            end->y = i_this->current.pos.y + 50.0f;
            end->z = i_this->current.pos.z + ofs->z;
            dBgS_LinChk_Set(linChk, start, end, i_this);
            if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                f32 px = player->current.pos.x;
                i_this->current.pos.x = px;
                f32 py = player->current.pos.y + 20.0f + REG_F(10, 11);
                i_this->current.pos.y = py;
                f32 pz = player->current.pos.z;
                i_this->old.pos.x = px;
                i_this->current.pos.z = pz;
                i_this->old.pos.y = py;
                i_this->old.pos.z = pz;
            }
            f32 spd = i_this->speedF;
            i_this->mRollMode = 0;
            if (spd > 0.0f) {
                i_this->speedF = 20.0f * bodyHIO().mThrowSpeedScale;
                i_this->mRollCollisionTimer = 20;
                i_this->speed.y = 20.0f * bodyHIO().mThrowSpeedScale;
            } else {
                i_this->speedF = 0.0f;
                i_this->mRollCollisionTimer = 20;
                i_this->speed.y = REG0_F(11) + -15.0f;
            }
            dBgS_LinChk_dt(linChk, MT_LINCHK_VTBLS);
        } else if (bodyPauseState() != 0) {
            i_this->mCrawlPhase = i_this->mCrawlPhase + 1;
        }
        cLib_addCalcAngleS2(&i_this->mHeadWobbleAngle, 0, 1, 0x100);
    } else {
        cLib_addCalcAngleS2(&i_this->mHeadWobbleAngle, 0, 1, 0x100);
    }

    s16 t = i_this->mCrawlPhase;
    if (t > 0x46) {
        cLib_addCalc2(&i_this->mBodyWaveScale, 1.0f, 1.0f, 0.05f);
        s16 step = bodyHIO().mUnrollPitchStep;
        if (i_this->mCrawlPhase < 0x6E) {
            if ((u32)(i_this->mCrawlPhase - 0x50) >= 0x15) {
                step = (s16)(step - 0x5DC);
            }
            cLib_addCalcAngleS2(&i_this->shape_angle.x, 0x7800, 4, 0x300);
        }
        cLib_addCalcAngleS2(&i_this->mSegmentPitchStep, step, 4, 0x1000);
        cLib_addCalc2(&i_this->mCrawlSegmentOffset, 1000.0f, 1.0f, 5.0f);
        cLib_addCalc2(&i_this->m454, 0.3f, 1.0f, 0.01f);
    } else {
        if (t == 0x46) {
            i_this->mBodyWaveAmplitude = REG0_F(3) + 2500.0f;
            i_this->m454 = 0.5f;
            if (i_this->mSuppressUnrollHop == 0) {
                i_this->speed.y = REG0_F(11) + 30.0f;
            }
        }
        cLib_addCalc0(&i_this->mBodyWaveAmplitude, 1.0f, 125.0f);
        s32 a = i_this->mCrawlPhase * (REG0_S(5) + 0x1388);
        cLib_addCalcAngleS2(&i_this->mHeadWobbleAngle,
                            (s16)gabi::ftoi(cM_ssin(a) * i_this->mBodyWaveAmplitude * 5.0f), 2,
                            0x1000);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 4, 0x1000);
        cLib_addCalc2(&i_this->mBodyWaveScale, -0.4f, 1.0f, 0.2f);
        a = i_this->mCrawlPhase * (REG0_S(5) + 0x1388);
        cLib_addCalcAngleS2(
            &i_this->mSegmentPitchStep,
            (s16)gabi::ftoi(cM_ssin(a) * i_this->mBodyWaveAmplitude * (REG0_F(14) + 4.0f)), 1,
            0x1000);
        s16 headWobbleAngle = i_this->mHeadWobbleAngle;
        s16 ay = i_this->current.angle.y;
        i_this->current.angle.x = i_this->shape_angle.x;
        i_this->shape_angle.y = ay + headWobbleAngle;
        cLib_addCalc0(&i_this->speedF, 1.0f, 0.5f);
    }
    if (i_this->mCrawlPhase == 100) {
        i_this->mTextureFrameMode = 2;
    }

    /* steam from the water (water_damage_se_set) */
    u8 st = i_this->mWaterSteamState;
    if (st == 0)
        return;
    if (st == 1) {
        s8 room = i_this->current.roomNo;
        JPABaseEmitter* em = dPa_control_set(dComIfGp_getParticle(), 2, 0x8095,
                                             &i_this->current.pos, &i_this->current.angle, nullptr,
                                             0xB4, nullptr, room, nullptr, nullptr, nullptr);
        i_this->mpEmitter = em;
        if (em != nullptr) {
            gabi::at<mtBodyEmitterView>(gabi::ea(em))->mStatus |= 0x40;
        }
        i_this->mWaterSteamState = 2;
        i_this->mWaterSteamTimer = 80;
    }
    if (i_this->mpEmitter != nullptr) {
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, 0);
        mDoMtx_YrotM(calc_mtx(), i_this->shape_angle.y);
        u32 e = gabi::ea(i_this->mpEmitter.get());
        mtBodyEmitterView* emitter = gabi::at<mtBodyEmitterView>(e);
        JPASetRMtxTVecfromMtx(calc_mtx(), &emitter->mGlobalRotation, &emitter->mGlobalTranslation);
        s16 timer = i_this->mWaterSteamTimer;
        if (timer == 0) {
            JPABaseEmitter_becomeInvalidEmitter(i_this->mpEmitter);
            e = gabi::ea(i_this->mpEmitter.get());
            emitter = gabi::at<mtBodyEmitterView>(e);
            emitter->mMaxFrame = -1;
            emitter->mStatus |= 1;
            i_this->mpEmitter = nullptr;
            i_this->mWaterSteamState = 0;
        } else {
            u8 alpha = timer < 30 ? (u8)(timer * 6) : 0xB4;
            gabi::at<mtBodyEmitterView>(gabi::ea(i_this->mpEmitter.get()))->mGlobalPrmColor.a =
                alpha;
        }
    }
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): checked trail tables
data l_trailStart 101BAE14
data l_reverseTrailStart 101BAE34
*/
// clang-format on
