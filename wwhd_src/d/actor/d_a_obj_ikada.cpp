/* Various Great Sea ships and rafts.
 * HD behavior is checked against the original verified translation unit.
 */
#include "d/actor/d_a_obj_ikada.h"
#include "gc_api/gc_api.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1002B2E4,
    .objAcch = {0x1002B324, 0x1002B344, 0x1002B334},
    .objAcchDt = {0x1002B334, 0x1002B344},
};

static daObj_Ikada_HIO_c l_HIO;
// HD: the sea surface uses a separate model and texture animation.
static IkadaNameTable sea_bdl = {{"vikae_minamo00.bdl", "vtsp_minamo00.bdl", "vikah_minamo00.bdl", "vtsp2_minamo00.bdl", "vsvsp_minamo00.bdl"}};
static IkadaNameTable sea_btk = {{"vikae_minamo00.btk", "vtsp_minamo00.btk", "vikah_minamo00.btk", "vtsp2_minamo00.btk", "vsvsp_minamo00.btk"}};

static IkadaGuard wave_l_guard;
static IkadaGuard sea_quad_a_guard;
static IkadaGuard sea_quad_b_guard;
static IkadaGuard sea_quad_c_guard;
static IkadaGuard sea_quad_d_guard;
static IkadaGuard sea_cross_guard;
static IkadaQuad sea_quad_a;
static IkadaQuad sea_quad_b;
static IkadaQuad sea_quad_c;
static IkadaQuad sea_quad_d;
static IkadaQuad sea_cross_points;
static IkadaQuadTable control = {{0x1046A0A8, 0x1046A0D8, 0x1046A108, 0x1046A0D8, 0x1046A138}};

static IkadaGuard flag_offset_guard;
static IkadaGuard fire_scale_guard;
static IkadaGuard ripple_scale_guard;
static IkadaVectorTable flag_offset;
static cXyz fire_scale;
static cXyz ripple_scale;
static IkadaGuard wave_r_guard;
static cXyz wave_l_direction;
static cXyz wave_r_direction;
static cXyz m_rope_base_vec;
static cXyz m_crane_offset;
static cXyz ikada_vector_zero;
static cXyz ikada_vector_up;
static IkadaGuard debug_yellow_guard;
static IkadaGuard debug_blue_guard;
static IkadaGuard debug_red_guard;
static GXColor debug_yellow;
static GXColor debug_blue;
static GXColor debug_red;
static GXColor debug_yellow_source = {255, 255, 0, 128};
static GXColor debug_blue_source = {0, 0, 255, 128};
static GXColor debug_red_source = {255, 0, 0, 128};


static IkadaIndexTable m_heapsize = {{7424, 12128, 11808, 13024, 28000}};
static IkadaIndexTable bdl = {{dRes_INDEX_IKADAH_BDL_VIKAE_e, dRes_INDEX_IKADAH_BDL_VTSP_e, dRes_INDEX_IKADAH_BDL_VIKAH_e, dRes_INDEX_IKADAH_BDL_VTSP2_e, dRes_INDEX_IKADAH_BDL_VSVSP_e}};
static IkadaIndexTable dzb = {{dRes_INDEX_IKADAH_DZB_VIKAE_e, dRes_INDEX_IKADAH_DZB_VTSP_e, dRes_INDEX_IKADAH_DZB_VIKAH_e, dRes_INDEX_IKADAH_DZB_VTSP_e, dRes_INDEX_IKADAH_DZB_VSVSP_e}};
static IkadaIndexTable flagparam = {{4, 4, 4, 4, 0x02000000}};
static IkadaScaleTable flag_scale = {{0.2f, 0.0f, 0.0f, 0.0f, 0.12f}};

/* 02360038 */
s32 nodeControl_CB(void *node, s32 timing) {
    if (!timing) {
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daObj_Ikada_c*>(model->getUserArea());
        if (actor)
            actor->_nodeControl(node, model);
    }
    return 1;
}
/* 0235FE84 */
void daObj_Ikada_c::_nodeControl(void *node, J3DModel *model) {
    u32 index = static_cast<J3DJoint*>(node)->getJntNo();
    if (index >= 4)
        ikadaApi_assertionShow("d_a_obj_ikada.cpp", 0x14C, "(jntNo >= 0) && (jntNo < JNT_END)");
    J3DMtxBuffer_hd* block = model->mpMtxBuffer;
    u32 array = block->mpAnmMtx;
    Mtx34* matrix = mDoMtx_stack_c::get();
    block->mFlags = (u16)block->mFlags | 0x10;
    ikadaApi_copyMatrix(gabi::at<Mtx34>(array + index * 0x30), matrix);
    csXyz* rotation = &mJointRot[index];
    ikadaApi_rotateZXY(matrix, (s16)rotation->x, (s16)rotation->y, (s16)rotation->z);
    if (index == 1) {
        s32 product =
            (s32)((u32)(s32)(s16)m115E * (u32)(REG_S(12, 5) + 5));
        u16 phase = m115C;
        s16 base = m115A;
        f32 sine = cM_ssin(phase);
        s16 angle = (s16)gabi::ftoi(gabi::fmadds((f32)product, sine, (f32)base));
        ikadaApi_rotateX(matrix, angle);
    }
    ikadaApi_copyMatrix(matrix, J3DSys::mCurrentMtx);
    block = model->mpMtxBuffer;
    array = (u32)block->mpAnmMtx;
    block->mFlags = (u16)block->mFlags | 0x10;
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (f32)matrix->m[i / 4][i % 4];
    for (u32 i = 0; i < 12; ++i)
        gabi::store<f32>(array + index * 0x30 + i * 4, values[i]);
}
/* 02360368 */
s32 pathMove_CB(cXyz *pos, cXyz *point, cXyz *path, daObj_Ikada_c *actor) {
    return actor->_pathMove(pos, point, path);
}
/* 0236012C */
s32 daObj_Ikada_c::_pathMove(cXyz *position, cXyz *previous, cXyz *next) {
    auto* path = mpPath.get();
    s32 pointIndex = mCurPathPoint;
    s32 index = pointIndex < (s32)(u16)path->m_num - 1 ? (s8)(pointIndex + 1) : 0;
    auto* points = path->m_points.get();
    mbCraneMode = (u8)points[index].mArg3 != 255;
    f32 y = current.pos.y;
    mCurPathP0.copy(*previous);
    mCurPathP0.y = y;
    mCurPathP1.copy(*next);
    mCurPathP1.y = y;
    gabi::Local<cXyz> direction, delta, horizontal, delta2, horizontal2;
    ikadaApi_subtractVector(&mCurPathP1, direction.get(), &mCurPathP0);
    if (ikadaApi_cXyz_normalizeRS(direction.get())) {
        s32 angle = ikadaApi_cM_atan2s((f32)direction->x, (f32)direction->z);
        u16 difference = ikadaApi_cLib_addCalcAngleS(&current.angle.y, angle, 8, 0x200, 8);
        f32 rate =
            (f32)speedF * __builtin_fabsf(cM_scos(difference));
        ikadaApi_cLib_chasePosXZ(position, &mCurPathP1, rate);
        ikadaApi_subtractVector(position, delta.get(), &mCurPathP1);
        horizontal->set(delta->x, 0.0f, delta->z);
        f32 length = std_sqrtf(PSVECSquareMag(horizontal.get()));
        f32 threshold = rate * (REG_F(12, 2) + 1.0f);
        if (!(length < threshold)) {
            ikadaApi_subtractVector(position, delta2.get(), &mCurPathP1);
            horizontal2->set(delta2->x, 0.0f, delta2->z);
            if (std_sqrtf(PSVECSquareMag(horizontal2.get())) != 0.0f)
                return 0;
        }
        if (mbCraneMode)
            modeProc(PROC_INIT_e, MODE_CRANE_TURN_e);
    }
    return 1;
}
/* 02360414 */
void ride_CB(void *bg, daObj_Ikada_c *actor, fopAc_ac_c *rider) {
    actor->_ride(rider);
}
/* 02360380 */
void daObj_Ikada_c::_ride(fopAc_ac_c *actor) {
    // HD: the bomb trigger rocks the ship without creating GC bomb-smoke particles.
    if (mType != 1)
        return;
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    if (gabi::ea(actor) == (u32)play->play.mpPlayerPtr[0].v)
        mbIsLinkRiding = 1;
    if (!actor || (s16)actor->mProcName != fpcNm_BOMB_e)
        return;
    if (ikadaApi_daBomb_c_getBombRestTime(actor) > 1)
        return;
    mLinkRideRockAmpl = 300;
    modeProc(PROC_INIT_e, MODE_STOP_BOMB_TERRY_e);
}
/* 02361000 */
s32 createHeap_CB(daObj_Ikada_c *actor) {
    return actor->_createHeap();
}
/* 023628CC */
f32 searchPlayerDistance(daObj_Ikada_c *actor) {
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    f32 distance = ikadaApi_fopAcM_searchActorDistanceXZ(actor, (u32)play->play.mPlayerInfo[0].mpPlayer.v);
    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    u32 ship = (u32)play->play.mpPlayerPtr[2].v;
    if (ship) {
        f32 other = ikadaApi_fopAcM_searchActorDistanceXZ(actor, ship);
        if (other < distance)
            return other;
    }
    return distance;
}
/* 023647D0 */
void daObj_Ikada_c::setCollision() {
    if (mbIsLinkRiding)
        return;
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    if ((u32)play->play.mPlayerStatus[0][0] & 0x4000)
        return;
    ikadaApi_cM3dGSph_SetR(gabi::ea(this) + 0x15F0, (f32)l_HIO.m9C * (f32)scale.x);
    ikadaApi_cM3dGSph_SetC(gabi::ea(this) + 0x15F0, &current.pos);
    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    ikadaApi_cCcS_Set(gabi::ea(&play->play.mCcS), &mSph);
}
/* 02364854 */
s32 daObj_Ikada_c::checkTgHit() {
    if (mbIsLinkRiding) {
        auto* player = reinterpret_cast<daPy_py_c*>(reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mPlayerInfo[0].mpPlayer.get());
        if ((u32)player->mResetFlg0 & 0x20000) {
            gabi::Local<cXyz> pos, delta, horizontal;
            pos->set((f32)player->mSwordTopPos.x, (f32)player->mSwordTopPos.y,
                     (f32)player->mSwordTopPos.z);
            ikadaApi_subtractVector(pos.get(), delta.get(), &current.pos);
            horizontal->set(delta->x, 0.0f, delta->z);
            f32 length = std_sqrtf(PSVECSquareMag(horizontal.get()));
            if (length < 1000.0f) {
                mLinkRideRockAmpl = 200;
                modeProc(PROC_INIT_e, MODE_STOP_TERRY_e);
                return 1;
            }
        }
    }
    ikadaApi_dCcD_GStts_Move(gabi::ea(this) + 0x1620);
    if (!ikadaApi_calcTimer(&m12E4)) {
        u32 hit = ikadaApi_getTargetHitObject(&mSph);
        ikadaApi_getTargetHitObject(&mSph);
        if (hit && gabi::load<u32>(hit + 0x10) == 0x20) {
            m12E4 = 60;
            mLinkRideRockAmpl = 300;
            modeProc(PROC_INIT_e, MODE_STOP_BOMB_TERRY_e);
            return 1;
        }
    }
    return 0;
}
/* 023644B0 */
void daObj_Ikada_c::pathMove() {
    cLib_addCalc2(&speedF, (f32)mVelocityFwdTarget, 0.1f, 2.0f);
    ikadaApi_dLib_pathMove(&mPathPosTarget, &mCurPathPoint, (u32)mpPath.v, gabi::guest_fn(reinterpret_cast<const void*>(pathMove_CB)), this,
               (f32)speedF);
    ikadaApi_cLib_addCalcPosXZ2(&current.pos, &mPathPosTarget, REG_F(12, 0) + 0.01f,
               (f32)speedF);
    if (speedF != 0.0f && mVelocityFwdTarget != 0.0f) {
        s32 target = ikadaApi_cLib_targetAngleY(&current.pos, &mPathPosTarget);
        cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x100);
    }
}
/* 02363F30 */
void daObj_Ikada_c::HandleRight() {
    f32 speed = (f32)l_HIO.m20;
    s16 end = mBckAnm.mFrameCtrl.mEnd;
    s32 frame = (s32)((u32)(s32)m1154 + (u32)gabi::ftoi(speed));
    m1154 = frame;
    if (!((f32)frame < (f32)end))
        m1154 = gabi::ftoi((f32)(s16)mBckAnm.mFrameCtrl.mStart);
}
/* 02364098 */
void daObj_Ikada_c::HandleLeft() {
    f32 speed = (f32)l_HIO.m20;
    s16 start = mBckAnm.mFrameCtrl.mStart;
    s32 frame = (s32)((u32)(s32)m1154 - (u32)gabi::ftoi(speed));
    m1154 = frame;
    if (!((f32)frame > (f32)start))
        m1154 = gabi::ftoi((f32)(s16)mBckAnm.mFrameCtrl.mEnd);
}
/* 02361268 */
void daObj_Ikada_c::createWave() {
    if (!(u32)wave_l_guard.mInitialized) {
        wave_l_direction.y = 1.0f;
        wave_l_guard.mInitialized = 1;
        wave_l_direction.x = 0.5f;
        wave_l_direction.z = -0.3f;
    }
    if (!(u32)wave_r_guard.mInitialized) {
        wave_r_direction.y = 1.0f;
        wave_r_guard.mInitialized = 1;
        wave_r_direction.x = -0.5f;
        wave_r_direction.z = -0.3f;
    }
    if (!mWaveLCallback.mpEmitter) {
        auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        ikadaApi_setParticle(gabi::at<dPa_control_c>((u32)play->play.mParticle.v), 0, 0x37, &mWavePos, &mWaveRot, nullptr, 255, &mWaveLCallback, -1, nullptr, nullptr, nullptr);
        auto* emitter = gabi::at<IkadaEmitter>(mWaveLCallback.mpEmitter);
        if (emitter) {
            emitter->mEmitterDir[0] = (f32)wave_l_direction.x;
            emitter->mEmitterDir[1] = (f32)wave_l_direction.y;
            emitter->mEmitterDir[2] = (f32)wave_l_direction.z;
        }
    }
    if (!mWaveRCallback.mpEmitter) {
        auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        ikadaApi_setParticle(gabi::at<dPa_control_c>((u32)play->play.mParticle.v), 0, 0x37, &mWavePos, &mWaveRot, nullptr, 255, &mWaveRCallback, -1, nullptr, nullptr, nullptr);
        auto* emitter = gabi::at<IkadaEmitter>(mWaveRCallback.mpEmitter);
        if (emitter) {
            emitter->mEmitterDir[0] = (f32)wave_r_direction.x;
            emitter->mEmitterDir[1] = (f32)wave_r_direction.y;
            emitter->mEmitterDir[2] = (f32)wave_r_direction.z;
        }
    }
    if (!mSplashCallBack.mpEmitter) {
        auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        ikadaApi_setParticle(gabi::at<dPa_control_c>((u32)play->play.mParticle.v), 0, 0x35, &mWavePos, &mWaveRot, nullptr, 255, &mSplashCallBack, -1, nullptr, nullptr, nullptr);
    }
    if (!mTrackCallBack.mpEmitter) {
        auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        ikadaApi_setParticle(gabi::at<dPa_control_c>((u32)play->play.mParticle.v), 5, 0x36, &mTrackPos, &shape_angle, nullptr, 0, &mTrackCallBack, -1, nullptr, nullptr, nullptr);
        auto* emitter = gabi::at<IkadaEmitter>(mTrackCallBack.mpEmitter);
        if (emitter) {
            emitter->mGlobalDynamicsScale[0] = 1.0f;
            emitter->mGlobalParticleScale[1] = 1.0f;
            emitter->mGlobalParticleScale[2] = 1.0f;
            emitter->mGlobalParticleScale[0] = 1.0f;
            emitter->mGlobalDynamicsScale[2] = 1.0f;
            emitter->mGlobalDynamicsScale[1] = 1.0f;
        }
    }
}
/* 02362968 */
void daObj_Ikada_c::setWave() {
    f32 splashTarget = (f32)l_HIO.mSplashScaleMax, waveSpeed = (f32)l_HIO.mWaveVelFade,
        waveMax = (f32)l_HIO.mWaveMaxVelocity, trackSpeed = (f32)l_HIO.mTrackVel;
    if (!(speedF > 2.0f)) {
        waveSpeed = 0.0f;
        splashTarget = 0.0f;
        mTrackCallBack.mStop = 1;
    } else
        createWave();
    mWavePos.y = ikadaApi_getWaterY(&mWavePos, &mObjAcch);
    mWaveRot.y = shape_angle.y;
    if (mTrackCallBack.mpEmitter) {
        f32 trans = (f32)l_HIO.mTrackIndTransY, scale = (f32)l_HIO.mTrackIndScaleY;
        u32 flags = (u32)mObjAcch.m_flags;
        mTrackCallBack.mSpeed = trackSpeed;
        mTrackCallBack.mIndTransY = trans;
        mTrackCallBack.mWaterY = mWavePos.y;
        mTrackCallBack.mIndScaleY = scale;
        mTrackCallBack.mWaterFlatY =
            flags & 0x800 ? (f32)mObjAcch.m_wtr.m_height : -1000000000.0f;
        mTrackCallBack.mLimitSpeed = 3.0f;
    }
    auto* emitter = gabi::at<IkadaEmitter>(mWaveLCallback.mpEmitter);
    if (emitter) {
        u32 flags = (u32)emitter->mFlags;
        emitter->mFlags = waveSpeed > 0.0f ? flags & ~1u : flags | 1u;
    }
    emitter = gabi::at<IkadaEmitter>(mWaveRCallback.mpEmitter);
    if (emitter) {
        u32 flags = (u32)emitter->mFlags;
        emitter->mFlags = waveSpeed > 0.0f ? flags & ~1u : flags | 1u;
    }
    mWaveRCallback.mVelFade1 = waveSpeed;
    mWaveLCallback.mVelFade1 = waveSpeed;
    mWaveLCallback.mMaxParticleVelocity = waveMax;
    mWaveRCallback.mMaxParticleVelocity = waveMax;
    mWaveRCallback.mVelFade2 = (f32)l_HIO.mWaveVelOffs + 1.0f;
    mWaveLCallback.mVelFade2 = 1.0f - (f32)l_HIO.mWaveVelOffs;
    f32 z1 = (f32)l_HIO.mWaveCollapsePos1.z, z0 = (f32)l_HIO.mWaveCollapsePos0.z,
        y1 = (f32)l_HIO.mWaveCollapsePos1.y, y0 = (f32)l_HIO.mWaveCollapsePos0.y,
        x1 = (f32)l_HIO.mWaveCollapsePos1.x, x0 = (f32)l_HIO.mWaveCollapsePos0.x;
    mWaveRCallback.mCollapsePos[1].y = y1;
    mWaveLCallback.mCollapsePos[1].x = -x1;
    mWaveLCallback.mCollapsePos[0].x = -x0;
    mWaveLCallback.mCollapsePos[1].y = y1;
    mWaveRCallback.mCollapsePos[1].x = x1;
    mWaveRCallback.mCollapsePos[1].z = z1;
    mWaveLCallback.mCollapsePos[0].y = y0;
    mWaveRCallback.mCollapsePos[0].x = x0;
    mWaveRCallback.mCollapsePos[0].z = z0;
    mWaveLCallback.mCollapsePos[0].z = z0;
    mWaveLCallback.mCollapsePos[1].z = z1;
    mWaveRCallback.mCollapsePos[0].y = y0;
    mWaveRCallback.mVelSpeed = (f32)l_HIO.mWaveVelSpeed;
    mWaveLCallback.mVelSpeed = (f32)l_HIO.mWaveVelSpeed;
    cLib_addCalc2(&mSplashScaleTimer, splashTarget, 0.1f, 10.0f);
    emitter = gabi::at<IkadaEmitter>(mSplashCallBack.mpEmitter);
    if (emitter) {
        f32 timer = mSplashScaleTimer;
        u32 flags = (u32)emitter->mFlags;
        emitter->mFlags = timer > 0.1f ? flags & ~1u : flags | 1u;
    }
    mSplashCallBack.mScaleTimer = mSplashScaleTimer;
    mSplashCallBack.mMaxScaleTimer = (f32)l_HIO.mSplashMaxScaleTimer;
}
/* 02363D28 */
void daObj_Ikada_c::incRopeCnt(s32 increment, s32 minimum) {
    s32 old = m07D8, newCount = (s32)((u32)old + (u32)increment);
    cXyz* velocity = m07DC + old;
    cXyz* positions = mRopeLine.getPos(0);
    if (newCount >= 200)
        newCount = 200;
    else if (newCount < minimum)
        newCount = minimum;
    s32 delta = (s32)((u32)newCount - (u32)old);
    cXyz* offset = &m_rope_base_vec;
    if (delta > 0) {
        gabi::Local<cXyz> shift;
        ikadaApi_multiplyVector(offset, shift.get(), (f32)delta);
        s32 i = 0;
        old = m07D8;
        while (i < old) {
            PSVECAdd(positions, shift.get(), positions);
            old = m07D8;
            ++i;
            ++positions;
        }
    } else
        positions += old;
    old = m07D8;
    if (old < newCount) {
        gabi::Local<cXyz> result;
        for (s32 remaining = (s32)((u32)newCount - (u32)old); remaining; --remaining) {
            velocity->copy(*offset);
            if (m07D8 != 0) {
                ikadaApi_subtractVector(positions - 1, result.get(), offset);
                positions->copy(*result.get());
            }
            ++positions;
            ++velocity;
        }
    }
    m07D8 = (s16)newCount;
}
/* 023620F0 */
void daObj_Ikada_c::setRopePos() {
    if (!(u32)ripple_scale_guard.mInitialized) {
        ripple_scale.x = 0.6f;
        ripple_scale.z = 0.6f;
        ripple_scale_guard.mInitialized = 1;
        ripple_scale.y = 0.6f;
    }
    s32 count = m07D8;
    cXyz* base = mRopeLine.getPos(0);
    s32 endIndex = count ? count - 1 : 0;
    cXyz* end = base + endIndex;
    gabi::Local<cXyz> saved, direction, normalized, tmp, delta, scaled, divided, updated,
        velocityDelta;
    saved->copy(*end);
    Mtx34* matrix = mDoMtx_stack_c::get();
    auto anchorMatrix = [&]() {
        J3DMtxBuffer_hd* block = mpModel->mpMtxBuffer;
        u32 array = block->mpAnmMtx;
        block->mFlags = (u16)block->mFlags | 0x10;
        ikadaApi_copyMatrix(gabi::at<Mtx34>(array + 0x30), matrix);
        ikadaApi_mDoMtx_stack_c_transM((f32)m_crane_offset.x, (f32)m_crane_offset.y,
                   (f32)m_crane_offset.z);
    };
    anchorMatrix();
    end->set((f32)matrix->m[0][3], (f32)matrix->m[1][3],
                             (f32)matrix->m[2][3]);
    if (mCurMode == 1) {
        J3DModel* endModel = mpRopeEnd.get();
        Mtx34* modelMatrix = endModel->getBaseTRMtx();
        direction->set((f32)modelMatrix->m[0][3] - (f32)end->x,
                       (f32)modelMatrix->m[1][3] - (f32)end->y,
                       (f32)modelMatrix->m[2][3] - (f32)end->z);
        ikadaApi_cXyz_normalizeZP(direction.get(), normalized.get());
        PSVECScale(direction.get(), direction.get(), 10.0f);
        count = m07D8;
        for (s32 i = count - 2; i >= 0; --i) {
            ikadaApi_addVector(base + i + 1, tmp.get(), direction.get());
            (base + i)->copy(*tmp.get());
            m07DC[i].copy(*&ikada_vector_zero);
        }
    } else {
        count = m07D8;
        for (s32 i = count - 2; i >= 0; --i) {
            cXyz* point = base + i;
            auto velocity = &m07DC[i];
            saved->copy(*point);
            f32 water = ikadaApi_getWaterY(point, &mObjAcch);
            f32 drag = (f32)point->y < water ? 0.6f : 0.9f;
            PSVECScale(velocity, velocity, drag);
            velocity->y = (f32)velocity->y - 3.0f;
            PSVECAdd(point, velocity, point);
            ikadaApi_subtractVector(point, delta.get(), point + 1);
            direction->copy(*delta.get());
            f32 length = std_sqrtf(PSVECSquareMag(direction.get()));
            if (length < 0.01f)
                ikadaApi_addVector(point + 1, updated.get(), &m_rope_base_vec);
            else {
                ikadaApi_multiplyVector(direction.get(), scaled.get(), 10.0f);
                ikadaApi_cXyz_dv(scaled.get(), divided.get(), length);
                ikadaApi_addVector(point + 1, updated.get(), divided.get());
            }
            point->copy(*updated.get());
            ikadaApi_subtractVector(point, velocityDelta.get(), saved.get());
            ikadaApi_multiplyVector(velocityDelta.get(), delta.get(), 0.05f);
            PSVECAdd(velocity, delta.get(), velocity);
        }
    }
    count = m07D8;
    base = mRopeLine.getPos(0);
    f32 x, y, z, dx, dy, dz;
    if (count > 1) {
        ikadaApi_subtractVector(base, delta.get(), base + 1);
        x = (f32)base->x;
        dx = delta->x;
        y = (f32)base->y;
        z = (f32)base->z;
        dy = delta->y;
        dz = delta->z;
    } else {
        anchorMatrix();
        x = (f32)base->x;
        y = (f32)base->y;
        dx = x - (f32)matrix->m[0][3];
        dy = y - (f32)matrix->m[1][3];
        z = (f32)base->z;
        dz = z - (f32)matrix->m[2][3];
    }
    u16 heading = shape_angle.y;
    f32 sine = cM_ssin(heading),
        cosine = cM_scos(heading);
    f32 rx = gabi::fmsubs(cosine, dx, sine * dz), rz = gabi::fmadds(sine, dx, cosine * dz);
    ikadaApi_setTranslationMatrix(matrix, x, y, z);
    s16 pitch = ikadaApi_cM_atan2s(rz, dy);
    s16 yaw = shape_angle.y;
    f32 horizontal = std_sqrtf(gabi::fmadds(dy, dy, rz * rz));
    s16 roll = ikadaApi_cM_atan2s(-rx, horizontal);
    ikadaApi_rotateZXY(matrix, pitch, yaw, roll);
    ikadaApi_rotateX(matrix, -0x4000);
    m0474.set((f32)matrix->m[0][3], (f32)matrix->m[1][3],
                    (f32)matrix->m[2][3]);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (f32)matrix->m[i / 4][i % 4];
    J3DModel* model = mpRopeEnd.get();
    for (u32 i = 0; i < 12; ++i)
        model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    f32 firstY = (f32)base->y, water = ikadaApi_getWaterY(base, &mObjAcch);
    base->y = firstY;
    if (water > firstY) {
        count = m07D8;
        cXyz* crossing = base;
        for (s32 i = 0; i < count; ++i) {
            f32 pointY = i ? (f32)base[i].y : firstY;
            if (!(pointY > water))
                crossing = base + i;
        }
        f32 pointX = (f32)crossing->x;
        m0444.y = water;
        m0444.x = pointX;
        m0444.z = (f32)crossing->z;
        if (!mRippleCallBack.mpEmitter) {
            auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            ikadaApi_setParticle(gabi::at<dPa_control_c>((u32)play->play.mParticle.v), 5, 0x33, &m0444, nullptr, &ripple_scale, 255, &mRippleCallBack, -1, nullptr, nullptr, nullptr);
            if (mRippleCallBack.mpEmitter) {
                u8 timer = m0440;
                mRippleCallBack.mRate = 0.0f;
                if (!timer) {
                    ikadaApi_fopKyM_createWpillar(&m0444, 0, 1.0f, 0.7f);
                    m0440 = 19;
                    return;
                }
            }
        }
    } else
        ikadaApi_dPa_rippleEcallBack_end(&mRippleCallBack);
    if (m0440)
        m0440 = (u8)((u8)m0440 - 1);
}
/* 02360420 */
void daObj_Ikada_c::setMtx() {
    Mtx34* matrix = mDoMtx_stack_c::get();
    ikadaApi_dLib_waveRot(&current.pos, &mWave, 0.0f);
    mLinkRideRockTimer = (s16)((s32)mLinkRideRockTimer + 1);
    s16 step = (s16)(REG_S(12, 4) + 5);
    cLib_addCalcAngleS2(&mLinkRideRockAmpl, 0, 10, step);
    s32 amplitude = mLinkRideRockAmpl;
    if (amplitude <= REG_S(12, 4) + 5) {
        amplitude = 0;
        mLinkRideRockAmpl = 0;
    }
    s32 product = (s32)((u32)amplitude * (u32)(REG_S(12, 5) + 3));
    u16 wavePhase = mWaveAnimTimer;
    f32 waveSine = cM_ssin(wavePhase);
    s16 rockPhase = mLinkRideRockTimer;
    s16 wavePitch = (s16)gabi::ftoi((f32)(REG_S(8, 5) + 200) * waveSine);
    s16 waveRoll = (s16)gabi::ftoi((f32)(REG_S(8, 6) + 60) * waveSine);
    s16 rockPitch = (s16)gabi::ftoi(
        (f32)product * cM_ssin((u16)((s32)rockPhase * 0x900)));
    s16 rockRoll = (s16)gabi::ftoi(
        (f32)product * cM_ssin((u16)((s32)rockPhase * 0x700)));
    if (!(u8)l_HIO.mbNoRotAnim) {
        shape_angle.x = (s16)((s32)mWave.mRotX + rockPitch + wavePitch);
        shape_angle.z = (s16)((s32)mWave.mRotZ + rockRoll + waveRoll);
        if (mbIsLinkRiding) {
            auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            auto* player = reinterpret_cast<daPy_py_c*>(play->play.mpPlayerPtr[0].get());
            gabi::Local<cXyz> delta, local;
            delta->set((f32)current.pos.x - (f32)player->current.pos.x,
                       (f32)current.pos.y - (f32)player->current.pos.y,
                       (f32)current.pos.z - (f32)player->current.pos.z);
            ikadaApi_mDoMtx_YrotS(calc_mtx(), (s16) - (s16)shape_angle.y);
            ikadaApi_MtxPosition(delta.get(), local.get());
            s16 pitch = (s16)gabi::ftoi(((f32)local->z - (-200.0f)) *
                                        (REG_F(18, 11) - 1.0f));
            s16 roll = (s16)gabi::ftoi((f32)local->x * (REG_F(18, 12) + 2.0f));
            if ((u32)player->mNoResetFlg0 & 0x02000000) {
                pitch = (s16)((s32)pitch * 2);
                roll = (s16)((s32)roll * 2);
                if (!mHDMotionMode) {
                    mHDMotionMode = 1;
                    mLinkRideRockAmpl = 150;
                }
            } else
                mHDMotionMode = 0;
            s32 p = pitch, r = roll;
            if (p > 0x400)
                p = 0x400;
            else if (p < -0x400)
                p = -0x400;
            if (r > 0x480)
                r = 0x480;
            else if (r < -0x200)
                r = -0x200;
            cLib_addCalcAngleS2(&mHDPitch, p, 4, 0x100);
            cLib_addCalcAngleS2(&mHDRoll, r, 4, 0x100);
        } else {
            mHDMotionMode = 0;
            cLib_addCalcAngleS2(&mHDPitch, 0, 0x20, 0x80);
            cLib_addCalcAngleS2(&mHDRoll, 0, 0x20, 0x80);
        }
        s16 pitch = mHDPitch, x = shape_angle.x, z = shape_angle.z, roll = mHDRoll;
        shape_angle.x = (s16)((s32)x + pitch);
        shape_angle.z = (s16)((s32)z + roll);
    }
    J3DModel* model = mpModel.get();
    f32 sx = scale.x, sy = scale.y, sz = scale.z;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    model->mBaseScale.z = sz;
    auto transform = [&]() {
        ikadaApi_setTranslationMatrix(matrix, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
        ikadaApi_mDoMtx_XYZrotM(matrix, (s16)shape_angle.x, 0, (s16)shape_angle.z);
        ikadaApi_rotateY(matrix, (s16)shape_angle.y);
    };
    transform();
    if (mType == 4)
        ikadaApi_mDoMtx_stack_c_transM(0.0f, 30.0f, -260.0f);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (f32)matrix->m[i / 4][i % 4];
    model = mpModel.get();
    for (u32 i = 0; i < 12; ++i)
        model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    if (mType == 4) {
        transform();
        ikadaApi_rotateY(matrix, 0x4000);
        for (u32 i = 0; i < 4; ++i) {
            gabi::Local<cXyz> offset;
            offset->set((f32)l_HIO.mSvOffsX[i], 0.0f, 0.0f);
            gabi::Local<be<u32>> actor;
            if (ikadaApi_fopAcM_SearchByID((u32)mSvId[i], actor.get()))
                ikadaApi_transformVector(matrix, offset.get(), &gabi::at<fopAc_ac_c>((u32)*actor.get())->current.pos);
        }
        gabi::Local<cXyz> offset;
        offset->set((f32)l_HIO.mSvWaveOffsX, 0.0f, 0.0f);
        ikadaApi_transformVector(matrix, offset.get(), &mWavePos);
        offset->x = (f32)l_HIO.mSvTrackOffsX;
        ikadaApi_transformVector(matrix, offset.get(), &mTrackPos);
    }
    ikadaApi_copyMatrix(matrix, &mMtx);
    s32 type = mType;
    if (type == 3 || type == 1) {
        mLightRotY = (s16)((s32)mLightRotY + 0xD0);
        mLightRotX = (s16)((s32)mLightRotX + 0x100);
        gabi::Local<cXyz> lightOffset;
        lightOffset->set(REG_F(12, 2), REG_F(12, 3) + 340.0f,
                         REG_F(12, 4) + 200.0f);
        model = mpModel.get();
        ikadaApi_copyMatrix(gabi::at<Mtx34>(gabi::ea(model->getBaseTRMtx())), matrix);
        ikadaApi_transformVector(matrix, lightOffset.get(), &mFirePos);
        ikadaApi_mDoMtx_stack_c_transM((f32)lightOffset->x, (f32)lightOffset->y, (f32)lightOffset->z);
        ikadaApi_rotateY(matrix, (s16)mLightRotY);
        ikadaApi_rotateX(matrix, (s16)mLightRotX);
        f32 power = mLightPower;
        ikadaApi_mDoMtx_stack_c_scaleM(power, power, power);
        ikadaApi_copyMatrix(matrix, &mLightMtx);
        mPLight.mPos.set((f32)matrix->m[0][3], (f32)matrix->m[1][3],
                         (f32)matrix->m[2][3]);
        mPLight.mColorR = 600;
        mPLight.mColorG = 400;
        mPLight.mColorB = 120;
        mPLight.mFluctuation = 250.0f;
        type = mType;
        if (type == 1 || type == 3) {
            gabi::Local<cXyz> soundOffset;
            soundOffset->set(REG_F(12, 2) - 105.0f,
                             REG_F(12, 3) + 380.0f,
                             REG_F(12, 4) + 40.0f);
            model = mpModel.get();
            ikadaApi_copyMatrix(gabi::at<Mtx34>(gabi::ea(model->getBaseTRMtx())), matrix);
            ikadaApi_transformVector(matrix, soundOffset.get(), &mSePos);
            type = mType;
        }
    }
    if (type == 4 || type == 3 || type == 1) {
        transform();
        gabi::Local<cXyz> offset;
        offset->set(0.0f, (f32)l_HIO.mShipOffsY_Attention, 0.0f);
        ikadaApi_transformVector(matrix, offset.get(), gabi::at<cXyz>(gabi::ea(this) + 0x390));
        offset->y = (f32)l_HIO.mShipOffsY_Eye;
        ikadaApi_transformVector(matrix, offset.get(), &eyePos);
        if (mType != 4) {
            f32 waveHeight = REG_F(8, 5) + 5.0f;
            if (mCurMode == 10)
                waveHeight = REG_F(8, 5) + 20.0f;
            offset->y = (f32)l_HIO.mTerryWaveOffsY;
            offset->z = (f32)l_HIO.mTerryWaveOffsZ + waveHeight;
            ikadaApi_transformVector(matrix, offset.get(), &mWavePos);
            offset->y = 0.0f;
            offset->z = (f32)l_HIO.mTerryTrackOffsZ;
            ikadaApi_transformVector(matrix, offset.get(), &mTrackPos);
        }
    }
}
/* 02360080 */
void daObj_Ikada_c::modeProc(Proc_e proc, s32 mode) {
    static ModeFuncs mode_tbl[] = {
        {&daObj_Ikada_c::modeWaitInit, &daObj_Ikada_c::modeWait, "WAIT"},
        {&daObj_Ikada_c::modeCraneUpInit, &daObj_Ikada_c::modeCraneUp, "CRANE_UP"},
        {&daObj_Ikada_c::modeCraneUpWaitInit, &daObj_Ikada_c::modeCraneUpWait, "CRANE_UP_WAIT"},
        {&daObj_Ikada_c::modeCraneDownInit, &daObj_Ikada_c::modeCraneDown, "CRANE_DOWN"},
        {&daObj_Ikada_c::modeCraneDownWaitInit, &daObj_Ikada_c::modeCraneDownWait, "CRANE_DOWN_WAIT"},
        {&daObj_Ikada_c::modeCraneTurnInit, &daObj_Ikada_c::modeCraneTurn, "CRANE_TURN"},
        {&daObj_Ikada_c::modeCraneResetInit, &daObj_Ikada_c::modeCraneReset, "CRANE_RESET"},
        {&daObj_Ikada_c::modeCraneWaitInit, &daObj_Ikada_c::modeCraneWait, "CRANE_WAIT"},
        {&daObj_Ikada_c::modePathMoveInit, &daObj_Ikada_c::modePathMove, "PATH_MOVE"},
        {&daObj_Ikada_c::modeStopInit, &daObj_Ikada_c::modeStop, "STOP"},
        {&daObj_Ikada_c::modePathMoveTerryInit, &daObj_Ikada_c::modePathMoveTerry, "PATH_MOVE_TERRY"},
        {&daObj_Ikada_c::modeStopTerryInit, &daObj_Ikada_c::modeStopTerry, "STOP"},
        {&daObj_Ikada_c::modeStopBombTerryInit, &daObj_Ikada_c::modeStopBombTerry, "STOP_BOMB"},
    };
    if (proc == PROC_INIT_e) {
        mCurMode = mode;
        (this->*mode_tbl[mode].init)();
    } else if (proc == PROC_EXEC_e) {
        (this->*mode_tbl[mCurMode].exec)();
    }
}
/* 02365184 */
void daObj_Ikada_c::modeWaitInit() {
}
/* 02363E98 */
void daObj_Ikada_c::modeWait() {
    if (mType != 4)
        return;
    s16 target = (s16)l_HIO.m1C;
    if (target) {
        if (m07D8 > target)
            incRopeCnt(-1, 0);
        else if (m07D8 < target)
            incRopeCnt(1, 0);
    }
    if (REG_S(12, 4))
        modeProc(PROC_INIT_e, MODE_CRANE_UP_e);
}
/* 02365188 */
void daObj_Ikada_c::modeCraneUpInit() {
}
/* 02363FE0 */
void daObj_Ikada_c::modeCraneUp() {
    if (m07D8 > 15) {
        HandleRight();
        incRopeCnt(-1, 0);
    } else
        modeProc(PROC_INIT_e, MODE_CRANE_UP_WAIT_e);
}
/* 02364034 */
void daObj_Ikada_c::modeCraneUpWaitInit() {
    mTimer = (s16)l_HIO.m24;
}
/* 02364044 */
void daObj_Ikada_c::modeCraneUpWait() {
    if (!ikadaApi_calcTimer(&mTimer))
        modeProc(PROC_INIT_e, MODE_CRANE_RESET_e);
}
/* 0236408C */
void daObj_Ikada_c::modeCraneDownInit() {
    m115E = 150;
}
/* 02364148 */
void daObj_Ikada_c::modeCraneDown() {
    cLib_addCalcAngleS2(&m115E, 0, 10, 10);
    if (m115E < 20) {
        HandleLeft();
        if (m07D8 < (s16)l_HIO.m1E)
            incRopeCnt(1, 0);
        else
            modeProc(PROC_INIT_e, MODE_CRANE_WAIT_e);
    } else
        m115C = (s16)((s32)m115C + REG_S(12, 2) + 0x1830);
}
/* 02364204 */
void daObj_Ikada_c::modeCraneDownWaitInit() {
    mTimer = (s16)l_HIO.m24;
}
/* 02364214 */
void daObj_Ikada_c::modeCraneDownWait() {
    if (!ikadaApi_calcTimer(&mTimer))
        modeProc(PROC_INIT_e, MODE_CRANE_DOWN_e);
}
/* 0236425C */
void daObj_Ikada_c::modeCraneTurnInit() {
    m115E = 150;
    speedF = 0.0f;
}
/* 02364274 */
void daObj_Ikada_c::modeCraneTurn() {
    cLib_addCalcAngleS2(&m115E, 0, 10, 10);
    if (m115E < 20) {
        HandleRight();
        s16 target = m1168 == 1 ? 0x4000 : -0x4000;
        m1158 = target;
        cLib_addCalcAngleS2(&m115A, target, 10, 0x100);
        if (cLib_distanceAngleS((s16)m115A, (s16)m1158) <= 0x100)
            modeProc(PROC_INIT_e, MODE_CRANE_DOWN_WAIT_e);
    } else
        m115C = (s16)((s32)m115C + REG_S(12, 2) + 0x1830);
}
/* 0236433C */
void daObj_Ikada_c::modeCraneResetInit() {
    m115E = 150;
    speedF = 0.0f;
}
/* 02364354 */
void daObj_Ikada_c::modeCraneReset() {
    cLib_addCalcAngleS2(&m115E, 0, 10, 10);
    if (m115E < 20) {
        HandleLeft();
        m1158 = 0;
        cLib_addCalcAngleS2(&m115A, 0, 10, 0x100);
        if (cLib_distanceAngleS((s16)m115A, (s16)m1158) <= 0x100)
            modeProc(PROC_INIT_e, MODE_PATH_MOVE_e);
    } else
        m115C = (s16)((s32)m115C + REG_S(12, 2) + 0x1830);
}
/* 0236440C */
void daObj_Ikada_c::modeCraneWaitInit() {
    mTimer = 600;
}
/* 02364418 */
void daObj_Ikada_c::modeCraneWait() {
    if (!ikadaApi_calcTimer(&mTimer))
        modeProc(PROC_INIT_e, MODE_CRANE_UP_e);
}
/* 02364460 */
void daObj_Ikada_c::modePathMoveInit() {
    m1168 = cM_rndF(1.0f) < 0.5 ? 0 : 1;
}
/* 02364578 */
void daObj_Ikada_c::modePathMove() {
    if ((u8)l_HIO.m06)
        HandleRight();
    if ((u8)l_HIO.m05)
        HandleLeft();
    if ((u8)l_HIO.m08 || mPathId == 255)
        return;
    if (mbCraneMode) {
        gabi::Local<cXyz> delta, horizontal;
        ikadaApi_subtractVector(&mCurPathP1, delta.get(), &current.pos);
        horizontal->set(delta->x, 0.0f, delta->z);
        f32 length = std_sqrtf(PSVECSquareMag(horizontal.get()));
        if (length < 50.0f) {
            mVelocityFwdTarget = 1.0f;
            pathMove();
            return;
        }
    }
    mVelocityFwdTarget = 8.0f;
    pathMove();
}
/* 0236518C */
void daObj_Ikada_c::modeStopInit() {
}
/* 02364668 */
void daObj_Ikada_c::modeStop() {
    if ((u8)l_HIO.m06)
        HandleRight();
    if ((u8)l_HIO.m05)
        HandleLeft();
    if (m02F4 == 8) {
        mVelocityFwdTarget = 0.0f;
        if (mPathId != 255)
            pathMove();
    }
    s32 count = m07D8;
    if (!m1166) {
        if (count <= 15) {
            HandleLeft();
            incRopeCnt(1, 0);
            return;
        }
        s32 previous = m02F4;
        modeProc(PROC_INIT_e, previous == 9 ? 8 : previous == 7 ? 3 : previous);
    } else if (count > 2) {
        HandleRight();
        incRopeCnt(-2, 0);
        if (m07D8 < 2)
            m07D8 = 2;
    }
}
/* 023647C4 */
void daObj_Ikada_c::modePathMoveTerryInit() {
    mTimer = 10;
}
/* 023649B8 */
void daObj_Ikada_c::modePathMoveTerry() {
    setCollision();
    if (checkTgHit())
        return;
    bool stop = false;
    f32 distance = searchPlayerDistance(this);
    if (distance < (f32)(s16)l_HIO.mPlayerStopDistance && !ikadaApi_calcTimer(&mStopTimer)) {
        mStopTimer = gabi::ftoi(cM_rndF(120.0f) + 240.0f);
        f32 voice = cM_rndF(100.0f);
        s32 room = (s8)current.roomNo;
        u32 sound = voice < 50.0f ? 0x6A2A : 0x6A2B;
        s32 reverb = ikadaApi_dComIfGp_getReverb(room);
        ikadaApi_startSound(sound, &mSePos, 0, reverb);
    }
    s32 type = mType;
    if (type == 1) {
        mVelocityFwdTarget = (f32)l_HIO.mVelocityTargetTerry1;
        auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        u32 flag = (u32)play->play.mPlayerStatus[0][0];
        s16 range = (flag & 0x10000 ? (s16)l_HIO.m94 : (s16)l_HIO.m9A);
        if (distance < (f32)range)
            stop = true;
    } else if (type == 3) {
        mVelocityFwdTarget = (f32)l_HIO.mVelocityTargetTerry3;
        if (distance < (f32)(s16)l_HIO.mPlayerStopDistance)
            mVelocityFwdTarget = (f32)l_HIO.mA8;
        if (distance < (f32)(s16)l_HIO.m98)
            stop = true;
    }
    if (mbIsLinkRiding) {
        mTimer = 30;
        if (mPathId != 255)
            pathMove();
        return;
    }
    if (!ikadaApi_calcTimer(&mTimer) && stop) {
        modeProc(PROC_INIT_e, MODE_STOP_TERRY_e);
        return;
    }
    if (mPathId != 255)
        pathMove();
}
/* 02364C60 */
void daObj_Ikada_c::modeStopTerryInit() {
    mTimer = 210;
}
/* 02364C6C */
void daObj_Ikada_c::modeStopTerry() {
    setCollision();
    if (checkTgHit())
        return;
    f32 distance = searchPlayerDistance(this);
    s32 type = mType;
    bool close = true;
    if (type == 1) {
        auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        u32 flag = (u32)play->play.mPlayerStatus[0][0];
        close = distance < (f32)(flag & 0x10000 ? (s16)l_HIO.m94 : (s16)l_HIO.m9A);
    } else if (type == 3)
        close = distance < (f32)(s16)l_HIO.m98;
    if (!close) {
        modeProc(PROC_INIT_e, MODE_PATH_MOVE_TERRY_e);
        return;
    }
    if (mbIsLinkRiding) {
        if (!ikadaApi_calcTimer(&mTimer)) {
            modeProc(PROC_INIT_e, MODE_PATH_MOVE_TERRY_e);
            return;
        }
    } else
        mTimer = 300;
    mVelocityFwdTarget = 0.0f;
    if (mPathId != 255)
        pathMove();
}
/* 02365190 */
void daObj_Ikada_c::modeStopBombTerryInit() {
}
/* 02364E10 */
void daObj_Ikada_c::modeStopBombTerry() {
    setCollision();
    if (!checkTgHit()) {
        mVelocityFwdTarget = 0.0f;
        if (mPathId != 255)
            pathMove();
    }
}
/* 02361E40 */
void daObj_Ikada_c::epProc() {
    if (!ikadaApi_dKy_daynight_check()) {
        ikadaApi_dPa_followEcallBack_end(&mFireParticle);
        mPLight.mPower = 0.0f;
        return;
    }
    if (!mFireParticle.mpEmitter) {
        if (!(u32)fire_scale_guard.mInitialized)
            fire_scale_guard.mInitialized = 1;
        f32 scale = REG_F(12, 10) + 0.5f;
        fire_scale.y = scale;
        fire_scale.x = scale;
        fire_scale.z = scale;
        auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        ikadaApi_setParticle(gabi::at<dPa_control_c>((u32)play->play.mParticle.v), 0, 0x1EA, &mFirePos, nullptr, &fire_scale, 255, &mFireParticle, -1, nullptr, nullptr, nullptr);
        if (!mFireParticle.mpEmitter)
            return;
    }
    gabi::Local<cXyz> pos;
    pos->set(mFirePos.x, mFirePos.y + 20.0f, mFirePos.z);
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    ikadaApi_dPa_control_c_setSimple((u32)play->play.mParticle.v, 0x4004, pos.get(), 255, 0x101D5E98,
               0x101D5E98, 0);
    if (!ikadaApi_calcTimer(&mEpTimer0)) {
        f32 value = cM_rndF(REG_F(0, 2) + 5.0f);
        mEpTimer0 = (s16)gabi::ftoi(value + REG_F(0, 3));
        value = cM_rndF(REG_F(0, 6) + 4.0f);
        m050C = (value + 8.0f) + REG_F(0, 7);
    }
    if (!ikadaApi_calcTimer(&mEpTimer1)) {
        mEpTimer1 = (s16)gabi::ftoi(cM_rndF(6.0f) + 3.0f);
        mLightPowerTarget = cM_rndF(0.075f) + 0.75f;
    }
    cLib_addCalc2(&m0508, (f32)m050C, 1.0f,
               REG_F(0, 4) + 1.0f);
    cLib_addCalc2(&mLightPower, (f32)mLightPowerTarget, 0.4f, 0.04f);
    mPLight.mPower = 150.0f * (f32)mLightPower;
}
/* 02362C54 */
s32 daObj_Ikada_c::_execute() {
    mWaveAnimTimer = (s16)((s32)mWaveAnimTimer + REG_S(8, 4) + 0x200);
    f32 target = REG_F(8, 5) + 5.0f;
    if (mCurMode == 10 && speedF > 1.0f)
        target = REG_F(8, 5) + 20.0f;
    cLib_addCalc2(&mHDWaterOffset, target, 0.1f, 1.0f);
    f32 offset = cM_ssin(mWaveAnimTimer) * (f32)mHDWaterOffset;
    if (mBombSmokeEasterEgg.mpEmitter) {
        s16 yaw = (s16)((s32)(s16)shape_angle.y + (s16)mBombSmokeAngle);
        mBombSmokeRot.z = shape_angle.z;
        mBombSmokeRot.x = shape_angle.x;
        mBombSmokeRot.y = yaw;
        auto* emitter = gabi::at<IkadaEmitter>(mBombSmokeEasterEgg.mpEmitter);
        emitter->mGlobalDynamicsScale[0] = 3.0f;
        emitter->mGlobalDynamicsScale[1] = 3.0f;
        emitter->mGlobalDynamicsScale[2] = 3.0f;
        emitter->mGlobalParticleScale[0] = 3.0f;
        emitter->mGlobalParticleScale[1] = 3.0f;
        emitter->mGlobalParticleScale[2] = 3.0f;
        mBombSmokePos.copy(current.pos);
        mBombSmokePos.y = (f32)l_HIO.m28;
        if (!ikadaApi_calcTimer(&m04A8))
            ikadaApi_dPa_followEcallBack_end(&mBombSmokeEasterEgg);
    }
    if (m1166 && mCurMode != 9) {
        m02F4 = mCurMode;
        modeProc(PROC_INIT_e, MODE_STOP_e);
    }
    if (mType == 3 || mType == 1)
        epProc();
    if (mType == 4) {
        setRopePos();
        if (mType == 4) {
            mBckAnm.mFrameCtrl.mFrame = (f32)(s32)m1154;
            s16 index = REG_S(12, 0);
            if (index) {
                mJointRot[index].x = REG_S(12, 1);
                index = REG_S(12, 0);
                mJointRot[index].y = REG_S(12, 2);
                index = REG_S(12, 0);
                mJointRot[index].z = REG_S(12, 3);
            }
        }
    }
    modeProc(PROC_EXEC_e, 13);
    current.pos.y = ikadaApi_getWaterY(&current.pos, &mObjAcch) + offset;
    setMtx();
    mpModel->calc();
    s32 type = mType;
    if (type == 4 || type == 3 || type == 1) {
        J3DModel* model = mpModel.get();
        f32 s = scale.x;
        s32 culled = ikadaApi_fopAcM_checkCullingBox(gabi::ea(model->getBaseTRMtx()), -1000.0f * s, -50.0f * s,
                                     -1000.0f * s, 1000.0f * s, 1000.0f * s, 1000.0f * s);
        bool _delete = !(speedF > 2.0f) || culled;
        if (!_delete)
            _delete = searchPlayerDistance(this) > 18000.0f;
        if (_delete) {
            ikadaApi_dPa_waveEcallBack_remove(&mWaveRCallback);
            ikadaApi_dPa_waveEcallBack_remove(&mWaveLCallback);
            ikadaApi_dPa_splashEcallBack_remove(&mSplashCallBack);
            mTrackCallBack.mStop = 1;
        } else
            setWave();
    }
    ikadaApi_dBgW_Move((u32)mpBgW.v);
    if (mType == 0 || mType == 4) {
        gabi::Local<be<u32>> id;
        *id.get() = mFlagPcId;
        u32 flag = 0;
        if ((u32)mFlagPcId != 0xFFFFFFFF)
            flag = ikadaApi_fopAcIt_Judge(0x025E1234, id.get());
        if (flag) {
            gabi::store<u32>(flag + 0x1E34, gabi::ea(&mMtx));
            f32 x = (f32)l_HIO.mFlagOffset.x, y = (f32)l_HIO.mFlagOffset.y,
                z = (f32)l_HIO.mFlagOffset.z;
            if (x != 0.0f || y != 0.0f || z != 0.0f)
                mFlagOffset.set(x, y, z);
            f32 size = (f32)l_HIO.mFlagScale;
            if (size != 0.0f)
                mFlagScale = size;
            else
                size = mFlagScale;
            gabi::store<u32>(flag + 0x1E38, gabi::ea(&mFlagOffset));
            gabi::store<f32>(flag + 0x1DFC, size);
        }
    }
    type = mType;
    mbIsLinkRiding = 0;
    if (type == 1 || type == 3) {
        gabi::Local<cXyz> delta;
        ikadaApi_subtractVector(&current.pos, delta.get(), &old.pos);
        f32 speed = std_sqrtf(PSVECSquareMag(delta.get())) / 30.0f;
        if (!(speed > 0.0f))
            speed = 0.0f;
        else if (!(speed < 1.0f))
            speed = 1.0f;
        s8 volume = (s8)gabi::ftoi(speed * 100.0f);
        s32 room = (s8)current.roomNo,
            reverb = ikadaApi_dComIfGp_getReverb(room);
        ikadaApi_startSound(0x303D, &eyePos, volume, reverb);
    }
    // HD: execute returns success rather than the GC false result.
    return 1;
}
/* 02363168 */
void daObj_Ikada_c::debugDraw() {
    // HD: debug geometry is absent; the GHS color initialization remains observable.
    auto initialize = [](IkadaGuard* guard, GXColor* storage, GXColor* color) {
        if ((u32)guard->mInitialized)
            return false;
        guard->mInitialized = 1;
        memcpy_g(reinterpret_cast<u8*>(storage), reinterpret_cast<u8*>(color), 4);
        return true;
    };
    s32 type = mType;
    bool craneReady = type == 4 && (u32)debug_yellow_guard.mInitialized;
    if (!craneReady) {
        if (type == 4) {
            initialize(&debug_yellow_guard, &debug_yellow, &debug_yellow_source);
            type = mType;
        }
        if (type == 1) {
            if (initialize(&debug_blue_guard, &debug_blue, &debug_blue_source))
                initialize(&debug_blue_guard, &debug_blue, &debug_blue_source);
            initialize(&debug_red_guard, &debug_red, &debug_red_source);
            type = mType;
        }
        if (type == 3) {
            initialize(&debug_red_guard, &debug_red, &debug_red_source);
            initialize(&debug_blue_guard, &debug_blue, &debug_blue_source);
            type = mType;
        }
        if (type == 1 || type == 3) {
            if (initialize(&debug_red_guard, &debug_red, &debug_red_source))
                type = mType;
        }
        if (type != 4 && type != 3 && type != 1)
            return;
    }

    if (initialize(&debug_red_guard, &debug_red, &debug_red_source))
        type = mType;
    if (type == 3 || type == 1 || type == 4)
        initialize(&debug_red_guard, &debug_red, &debug_red_source);
}
/* 02363374 */
void daObj_Ikada_c::drawSea() {
    ikadaApi_mDoExt_baseAnm_play(&mSeaBtk);
    auto initializeQuad = [](IkadaGuard* guard, IkadaQuad* storage, f32 left, f32 right, f32 front, f32 back,
                             bool guardFirst) {
        if (guard->mInitialized)
            return;
        if (guardFirst)
            guard->mInitialized = 1;
        const f32 values[] = {left, 0, front, right, 0, front, right, 0, back, left, 0, back};
        for (u32 i = 0; i < 4; ++i) {
            storage->mPoints[i].x = values[i * 3];
            storage->mPoints[i].y = values[i * 3 + 1];
            storage->mPoints[i].z = values[i * 3 + 2];
        }
        if (!guardFirst)
            guard->mInitialized = 1;
    };
    initializeQuad(&sea_quad_a_guard, &sea_quad_a, -600, 600, 600, -600, false);
    initializeQuad(&sea_quad_b_guard, &sea_quad_b, -350, 350, 650, -400, true);
    initializeQuad(&sea_quad_c_guard, &sea_quad_c, -550, 550, 750, -750, true);
    initializeQuad(&sea_quad_d_guard, &sea_quad_d, -140, 140, 300, -400, true);
    Mtx34* matrix = mDoMtx_stack_c::get();
    ikadaApi_setTranslationMatrix(matrix, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    ikadaApi_rotateY(matrix, (s16)shape_angle.y);
    IkadaQuad* quad = control.mValues[(s32)mType].get();
    gabi::Local<cXyz[4]> points;
    for (u32 i = 0; i < 4; ++i) {
        cXyz *point = &(*points.get())[i];
        ikadaApi_transformVector(matrix, &quad->mPoints[i], point);
        point->y = ikadaApi_getWaterY(point, &mObjAcch);
    }
    gabi::Local<cXyz> normal, a, b, cross;
    normal->set((f32)ikada_vector_zero.x, (f32)ikada_vector_zero.y,
                (f32)ikada_vector_zero.z);
    for (u32 i = 0; i < 4; i += 2) {
        ikadaApi_subtractVector(&(*points.get())[i + 1], a.get(), &(*points.get())[i]);
        ikadaApi_subtractVector(&(*points.get())[(i + 2) % 4], b.get(), &(*points.get())[i + 1]);
        ikadaApi_cXyz_outprod(a.get(), cross.get(), b.get());
        if (!ikadaApi_cXyz_normalizeRS(cross.get()))
            cross->copy(*&ikada_vector_up);
        PSVECAdd(normal.get(), cross.get(), normal.get());
        if (!ikadaApi_cXyz_normalizeRS(normal.get()))
            normal->copy(*&ikada_vector_up);
    }
    f32 plane = -3.4028234663852886e38f;
    for (u32 i = 0; i < 4; ++i) {
        f32 dot = PSVECDotProduct(normal.get(), &(*points.get())[i]);
        if (dot > plane)
            plane = dot;
    }
    if (!sea_cross_guard.mInitialized) {
        sea_cross_points.mPoints[0].x = 0.0f;
        sea_cross_points.mPoints[0].y = 0.0f;
        sea_cross_guard.mInitialized = 1;
        sea_cross_points.mPoints[0].z = 100.0f;
        sea_cross_points.mPoints[1].set(0.0f, 0.0f, -100.0f);
        sea_cross_points.mPoints[2].set(-100.0f, 0.0f, 0.0f);
        sea_cross_points.mPoints[3].set(100.0f, 0.0f, 0.0f);
    }
    gabi::Local<cXyz[4]> projected;
    for (u32 i = 0; i < 4; ++i) {
        cXyz *point = &(*projected.get())[i];
        ikadaApi_transformVector(matrix, &sea_cross_points.mPoints[i], point);
        f32 horizontal =
            gabi::fmadds((f32)point->x, (f32)normal->x, (f32)point->z * (f32)normal->z);
        point->y = (plane - horizontal) / (f32)normal->y;
    }
    gabi::Local<cXyz> xDifference, zDifference, horizontalX, horizontalZ;
    ikadaApi_subtractVector(&(*projected.get())[1], xDifference.get(), &(*projected.get())[0]);
    ikadaApi_subtractVector(&(*projected.get())[3], zDifference.get(), &(*projected.get())[2]);
    horizontalX->set(xDifference->x, 0.0f, xDifference->z);
    f32 length = std_sqrtf(PSVECSquareMag(horizontalX.get()));
    s32 pitch = ikadaApi_cM_atan2s((f32)xDifference->y, length);
    horizontalZ->set(zDifference->x, 0.0f, zDifference->z);
    length = std_sqrtf(PSVECSquareMag(horizontalZ.get()));
    s32 roll = ikadaApi_cM_atan2s((f32)zDifference->y, length);
    f32 horizontal =
            gabi::fmadds((f32)current.pos.x, (f32)normal->x, (f32)current.pos.z * (f32)normal->z),
        height = (plane - horizontal) / (f32)normal->y;
    f32 water = ikadaApi_getWaterY(&current.pos, &mObjAcch);
    f32 y = gabi::fmadds(water - height, height < water ? 1.0f : 0.0f, height);
    ikadaApi_setTranslationMatrix(matrix, (f32)current.pos.x, y, (f32)current.pos.z);
    ikadaApi_rotateZXY(matrix, pitch, (s16)shape_angle.y, roll);
    ikadaApi_mDoMtx_stack_c_transM(0.0f, 3.0f, 0.0f);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (f32)matrix->m[i / 4][i % 4];
    J3DModel* model = mpSeaModel.get();
    for (u32 i = 0; i < 12; ++i)
        model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    u32 data = gabi::ea(mpSeaModel->getModelData()),
        material = gabi::load<u32>(data + 0x10);
    gabi::Local<GXColor> seaColor, foamColor;
    ikadaApi_dKy_get_seacolor(seaColor.get(), foamColor.get());
    gabi::Local<be<s16>[4]> color;
    u32 colorEA = gabi::ea(seaColor.get());
    (*color.get())[0] = gabi::load<u8>(colorEA);
    (*color.get())[1] = gabi::load<u8>(colorEA + 1);
    (*color.get())[2] = gabi::load<u8>(colorEA + 2);
    (*color.get())[3] = 255;
    u32 light = ikadaApi_getEnvironment();
    f32 strength = gabi::load<f32>(light + 0x10B8);
    u32 tev = gabi::load<u32>(material + 0x18), vt = gabi::load<u32>(tev + 4);
    gabi::call(gabi::load<u32>(vt + 0x24), tev, 1, color.get());
    gabi::Local<IkadaColorVector> rgba;
    for (u32 i = 0; i < 4; ++i)
        rgba->mValues[i] = (f32)(s16)(*color.get())[i] / 255.0f;
    gabi::Local<IkadaColorVector> bright;
    ikadaApi_scaleColorBrightness(bright.get(), rgba.get(), strength);
    gabi::store<u32>(material + 0xA0, gabi::load<u32>(material + 0xA0) | 0x20);
    u32 output = ikadaApi_getMaterialColor(material + 0xA0, 5);
    f32 alpha = (f32)(s16)(*color.get())[3] / 255.0f;
    f32 r = bright->mValues[0], g = bright->mValues[1], bValue = bright->mValues[2];
    gabi::store<f32>(output + 4, g);
    gabi::store<f32>(output + 8, bValue);
    gabi::store<f32>(output, r);
    gabi::store<f32>(output + 12, alpha);
    mSeaBtk.entry(gabi::at<J3DModelData>(data), (f32)mSeaBtk.mFrameCtrl.mFrame);
    mDoExt_modelUpdateDL(mpSeaModel);
}
/* 02363B78 */
s32 daObj_Ikada_c::_draw() {
    if ((u8)l_HIO.mbDebugDraw)
        debugDraw();
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    gabi::store<u32>(0x104B4634, (u32)reinterpret_cast<dDlstHD::dDlst_list_c*>(&play->drawlist)->mpOpaListBG);
    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    gabi::store<u32>(0x104B4638, (u32)reinterpret_cast<dDlstHD::dDlst_list_c*>(&play->drawlist)->mpXluListBG);
    J3DModel* model = mpModel.get();
    if (mType == 4) {
        mBckAnm.entry(model->getModelData(),
                   (f32)mBckAnm.mFrameCtrl.mFrame);
        mDoExt_modelUpdateDL(mpModel);
        u32 data = gabi::ea(mpModel->getModelData());
        gabi::store<u32>(gabi::load<u32>(data + 8) + 0x14, 0);
    } else
        mDoExt_modelUpdateDL(mpModel);
    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    gabi::store<u32>(0x104B4634, (u32)reinterpret_cast<dDlstHD::dDlst_list_c*>(&play->drawlist)->mpOpaList);
    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    gabi::store<u32>(0x104B4638, (u32)reinterpret_cast<dDlstHD::dDlst_list_c*>(&play->drawlist)->mpXluList);
    drawSea();
    if (mType == 4) {
        if (m07D8 >= 2) {
            ikadaApi_mDoExt_3DlineMat1_c_update(gabi::ea(this) + 0x934, (u16)(s32)m07D8, 0x101CA4D8, 0,
                       gabi::ea(this) + 0x110, 5.0f);
            play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            u32 vt = gabi::load<u32>(gabi::ea(this) + 0xA64);
            s32 index = gabi::call<s32>(gabi::load<u32>(vt + 0x14), gabi::ea(this) + 0x934);
            ikadaApi_mDoExt_3DlineMatSortPacket_setMat(gabi::ea(&reinterpret_cast<dDlstHD::dDlst_list_c*>(&play->drawlist)->m3DlineMatSortPacket) + (u32)index * 0x9C, gabi::ea(this) + 0x934);
        }
        g_env_light.setLightTevColorType(mpRopeEnd, &tevStr);
        mDoExt_modelUpdateDL(mpRopeEnd);
        if (mType == 4)
            ikadaApi_dSnap_RegistFig(0x83, this, 1.0f, 1.0f, 1.0f);
    }
    return 1;
}
/* 02361218 */
void daObj_Ikada_c::getArg() {
    u32 param = (u32)mParameters;
    u32 type = param & 15;
    s16 homeX = home.angle.x;
    mType = type;
    if (type == 4) {
        mPathId = param >> 16;
        return;
    }
    m0294 = (param >> 4) & 63;
    m0298 = (param >> 10) & 255;
    m029C = (u8)homeX;
    mPathId = (u32)(s32)homeX >> 8;
    m02A0 = (param >> 18) & 255;
}
/* 023614C8 */
void daObj_Ikada_c::createInit() {
    mInitPos.copy(current.pos);
    f32 random = cM_rndF(120.0f);
    u32 type = mType;
    u32 position[3] = {gabi::load<u32>(gabi::ea(&current.pos)),
                       gabi::load<u32>(gabi::ea(&current.pos) + 4),
                       gabi::load<u32>(gabi::ea(&current.pos) + 8)};
    u8 path = mPathId;
    mStopTimer = gabi::ftoi(random + 240.0f);
    if (type <= 2)
        actor_status = (u32)actor_status | 0x25;
    for (u32 i = 0; i < 3; ++i)
        gabi::store<u32>(gabi::ea(&mPathPosTarget) + i * 4, position[i]);
    if (path != 255 && (mType == 4 || mType == 1 || mType == 3)) {
        mpPath = ikadaApi_dPath_GetRoomPath(path, (s8)current.roomNo);
        if (mType == 4)
            modeProc(PROC_INIT_e, MODE_PATH_MOVE_e);
    } else
        modeProc(PROC_INIT_e, MODE_WAIT_e);
    if (mType == 1 || mType == 3)
        modeProc(PROC_INIT_e, MODE_STOP_TERRY_e);
    if (mType == 4)
        for (u32 i = 0; i < 4; ++i)
            mSvId[i] =
                ikadaApi_fopAcM_createChild(0x1002B71C + i * 4, (u32)mBsPcId,
                                -1, &current.pos, (s8)tevStr.mRoomNo, 0, 0, 0);
    current.pos.y = ikadaApi_getWaterY(&current.pos, &mObjAcch);
    reinterpret_cast<background_world::dBgW*>(mpBgW.get())->mGrpRoomInf = (u8)current.roomNo;
    if (mType != 4) {
        setMtx();
        ikadaApi_dBgW_Move((u32)mpBgW.v);
    }
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    ikadaApi_dBgS_Regist(gabi::ea(&play->play.mBgS), (u32)mpBgW.v, this);
    if (mType == 1)
        reinterpret_cast<background_world::dBgW*>(mpBgW.get())->mpRideCb = gabi::guest_fn(reinterpret_cast<const void*>(ride_CB));
    u32 callback = mType == 1 ? 0x024EE708 : 0x024EE658;
    reinterpret_cast<background_world::dBgW*>(mpBgW.get())->m_crr_func = callback;
    ikadaApi_dBgS_AcchCir_SetWall(&mAcchCir, 30.0f, 30.0f);
    ikadaApi_dBgS_Acch_Set(&mObjAcch, &current.pos, &old.pos, this, 1, &mAcchCir, &speed, 0, 0);
    mObjAcch.m_flags = (u32)mObjAcch.m_flags | dBgS_Acch::WALL_NONE | dBgS_Acch::ROOF_NONE;
    s32 currentType = mType;
    if (currentType == 4 || currentType == 3 || currentType == 1) {
        createWave();
        if (mType == 3 || mType == 1)
            ikadaApi_dKy_plight_set(&mPLight);
    }
    gravity = 0.0f;
    ikadaApi_fopAcM_posMoveF(this, 0);
    J3DModel* model = mpModel.get();
    cullMtx = gabi::ea(model->getBaseTRMtx());
    f32 actorScale = scale.x;
    f32 low = -1000.0f * actorScale, high = 1000.0f * actorScale;
    ikadaApi_fopAcM_setCullSizeBox(this, low, -50.0f * actorScale, low, high, high, high);
    cullSizeFar = 10.0f;
    if (mType == 0 || mType == 4) {
        if (!(u32)flag_offset_guard.mInitialized) {
            for (u32 i = 0; i < 5; ++i) {
                flag_offset.mValues[i].x = i == 4 ? 100.0f : 0.0f;
                flag_offset.mValues[i].y = i == 0 ? 700.0f : i == 4 ? 530.0f : 0.0f;
                flag_offset.mValues[i].z = 0.0f;
            }
            flag_offset_guard.mInitialized = 1;
        }
        mFlagPcId = ikadaApi_createActor(
            fpcNm_MAJUU_FLAG_e, (u32)flagparam.mValues[(s32)mType], &current.pos,
            (s8)tevStr.mRoomNo, &current.angle, 0, -1, 0);
        currentType = mType;
        mFlagOffset.copy(flag_offset.mValues[currentType]);
        mFlagScale = flag_scale.mValues[currentType];
    }
    mWave.mAnimX = (s16)gabi::ftoi(cM_rndF(32768.0f));
    mWave.mAnimZ = (s16)gabi::ftoi(cM_rndF(32768.0f));
    if (mType == 4) {
        mpModel->calc();
        cXyz* positions = mRopeLine.getPos(0);
        m07D8 = 15;
        J3DMtxBuffer_hd* modelBlock = mpModel->mpMtxBuffer;
        u32 array = modelBlock->mpAnmMtx;
        modelBlock->mFlags = (u16)modelBlock->mFlags | 0x10;
        Mtx34* matrix = mDoMtx_stack_c::get();
        ikadaApi_copyMatrix(gabi::at<Mtx34>(array + 0x30), matrix);
        ikadaApi_mDoMtx_stack_c_transM((f32)m_crane_offset.x, (f32)m_crane_offset.y,
                   (f32)m_crane_offset.z);
        positions[14].x = (f32)matrix->m[0][3];
        positions[14].y = (f32)matrix->m[1][3];
        positions[14].z = (f32)matrix->m[2][3];
        gabi::Local<cXyz> step, tmp, difference;
        step->set(0.0f, -1.0f, 0.0f);
        PSVECScale(step.get(), step.get(), 10.0f);
        s32 count = m07D8;
        if (count >= 2)
            for (s32 i = count - 2; i >= 0; --i) {
                ikadaApi_addVector(positions + i + 1, tmp.get(), step.get());
                (positions + i)->copy(*tmp.get());
                m07DC[i].copy(*&ikada_vector_zero);
            }
        positions = mRopeLine.getPos(0);
        ikadaApi_subtractVector(positions, difference.get(), positions + 1);
        u16 heading = shape_angle.y;
        f32 sine = cM_ssin(heading),
            cosine = cM_scos(heading);
        f32 dx = difference->x, dz = difference->z, dy = difference->y;
        f32 x = gabi::fmsubs(cosine, dx, sine * dz), z = gabi::fmadds(sine, dx, cosine * dz);
        ikadaApi_setTranslationMatrix(matrix, (f32)positions->x, (f32)positions->y,
                   (f32)positions->z);
        s16 pitch = ikadaApi_cM_atan2s(z, dy);
        s16 yaw = shape_angle.y;
        f32 horizontal = std_sqrtf(gabi::fmadds(dy, dy, z * z));
        s16 roll = ikadaApi_cM_atan2s(-x, horizontal);
        ikadaApi_rotateZXY(matrix, pitch, yaw, roll);
        ikadaApi_rotateX(matrix, -0x4000);
        m0474.set((f32)matrix->m[0][3], (f32)matrix->m[1][3],
                        (f32)matrix->m[2][3]);
        f32 values[12];
        for (u32 i = 0; i < 12; ++i)
            values[i] = (f32)matrix->m[i / 4][i % 4];
        J3DModel* endModel = mpRopeEnd.get();
        for (u32 i = 0; i < 12; ++i)
            endModel->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    }
    ikadaApi_dCcD_Stts_Init(&mStts, 255, 255, this);
    ikadaApi_dCcD_Sph_Set(&mSph, 0x1002B810);
    gabi::store<u32>(gabi::ea(this) + 0x151C, gabi::ea(&mStts));
}
/* 02360C64 */
s32 daObj_Ikada_c::_createHeap() {
    auto getResource = [&](const char* archive, u32 index) {
        gabi::Local<IkadaSafeString> name;
        name->mpText = gabi::guest_addr(archive);
        name->mpVtable = gc_tu.safeString;
        return ikadaApi_dRes_control_c_getRes(gabi::load<u32>(0x101F4F28), name.get(), index);
    };
    auto getNamedResource = [&](u32 nameAddress) {
        gabi::Local<IkadaSafeString> archive, name;
        archive->mpText = gabi::guest_addr("IkadaH");
        archive->mpVtable = gc_tu.safeString;
        name->mpText = nameAddress;
        name->mpVtable = gc_tu.safeString;
        return ikadaApi_getNamedResource(gabi::load<u32>(0x101F4F28), archive.get(), name.get());
    };
    u32 data = getResource("IkadaH", bdl.mValues[(s32)mType]);
    if (!data)
        ikadaApi_assertionShow("d_a_obj_ikada.cpp", 0x92D, "modelData != (0)");
    mpModel = ikadaApi_mDoExt_J3DModel_create(data, 0x80000, 0x11000022);
    if (!mpModel)
        return 0;
    if (mType == 4) {
        u32 animation = getResource("IkadaH", dRes_INDEX_IKADAH_BCK_SVSHIP_KAITEN_e);
        if (!animation)
            ikadaApi_assertionShow("d_a_obj_ikada.cpp", 0x937, "bck != 0");
        if (!ikadaApi_mDoExt_bckAnm_init(&mBckAnm, data, animation, 1, 2, 0, -1, 0, 1.0f))
            return 0;
    }
    mpModel->mUserArea = gabi::ea(this);
    if (mType == 4) {
        u16 index = 0;
        u32 tree = ikadaApi_getJointTree(data);
        while (index < gabi::load<u16>(tree + 8)) {
            if (index >= 1 && index <= 3) {
                J3DJoint* joint = gabi::at<J3DModelData>(data)->getJointNodePointer(index);
                joint->mCallBack = gabi::guest_fn(reinterpret_cast<const void*>(nodeControl_CB));
            }
            ++index;
            tree = ikadaApi_getJointTree(data);
        }
    }
    setMtx();
    u32 seaData = getNamedResource((u32)sea_bdl.mValues[(s32)mType].v);
    mpSeaModel = ikadaApi_mDoExt_J3DModel_create(seaData, 0, 0x11020203);
    if (!mpSeaModel)
        return 0;
    u32 seaAnimation = getNamedResource((u32)sea_btk.mValues[(s32)mType].v);
    if (!ikadaApi_initSeaTextureAnimation(&mSeaBtk, seaData, seaAnimation, 1, 2, 0, -1, 0, 0, 1.0f))
        return 0;
    mpBgW = ikadaApi_dBgW_dBgW(0);
    if (!mpBgW)
        return 0;
    u32 collision = getResource("IkadaH", dzb.mValues[(s32)mType]);
    if (ikadaApi_cBgW_Set((u32)mpBgW.v, collision, 1, &mMtx))
        return 0;
    if (mType == 4) {
        u32 ropeTexture = getResource("Always", dRes_INDEX_ALWAYS_BTI_ROPE_e);
        if (!ikadaApi_mDoExt_3DlineMat1_c_init(gabi::ea(this) + 0x934, 1, 200, ropeTexture, 0))
            return 0;
        data = getResource("Link", dRes_INDEX_LINK_BDL_ROPEEND_e);
        if (!data)
            ikadaApi_assertionShow("d_a_obj_ikada.cpp", 0x989, "modelData != (0)");
        mpRopeEnd = ikadaApi_mDoExt_J3DModel_create(data, 0x80000, 0x11000002);
        if (!mpRopeEnd)
            return 0;
    }
    return 1;
}
/* 02361C8C */
s32 daObj_Ikada_c::_create() {
    u32 status = (u32)actor_condition;
    if (!(status & 8)) {
        if (gabi::ea(this)) {
            actorConstructor(this);
            status = (u32)actor_condition;
        }
        actor_condition = status | 8;
    }
    s32 phase = ikadaApi_dComIfG_resLoad(&mPhase, "IkadaH");
    if (phase == 4) {
        getArg();
        u32 size = m_heapsize.mValues[(s32)mType];
        if (!ikadaApi_fopAcM_entrySolidHeap(this, gabi::guest_fn(reinterpret_cast<const void*>(createHeap_CB)), size))
            return 5;
        createInit();
    }
    return phase;
}
/* 02361D58 */
s32 daObj_Ikada_c::_delete() {
    ikadaApi_dComIfG_resDelete(&mPhase, "IkadaH");
    ikadaApi_dPa_waveEcallBack_remove(&mWaveRCallback);
    ikadaApi_dPa_waveEcallBack_remove(&mWaveLCallback);
    ikadaApi_dPa_splashEcallBack_remove(&mSplashCallBack);
    ikadaApi_dPa_trackEcallBack_remove(&mTrackCallBack);
    ikadaApi_dPa_rippleEcallBack_end(&mRippleCallBack);
    u32 vt = gabi::load<u32>(gabi::ea(this) + 0x5CC);
    gabi::call(gabi::load<u32>(vt + 0x44), &mFireParticle);
    vt = gabi::load<u32>(gabi::ea(this) + 0x59C);
    gabi::call(gabi::load<u32>(vt + 0x44), &mBombSmokeEasterEgg);
    ikadaApi_JAIZelBasic_seDeleteObject(&mSePos);
    if (mType == 3 || mType == 1)
        ikadaApi_dKy_plight_cut(&mPLight);
    if ((u32)heap.v) {
        u32 bg = (u32)mpBgW.v;
        if (bg && gabi::load<u32>(bg) < 0x100) {
            auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            ikadaApi_cBgS_Release(gabi::ea(&play->play.mBgS), (u32)mpBgW.v);
        }
    }
    return 1;
}
/* 02361D54 */
s32 daObj_IkadaCreate(daObj_Ikada_c *actor) {
    return actor->_create();
}
/* 02361E3C */
s32 daObj_IkadaDelete(daObj_Ikada_c *actor) {
    return actor->_delete();
}
/* 02363164 */
s32 daObj_IkadaExecute(daObj_Ikada_c *actor) {
    return actor->_execute();
}
/* 02363D24 */
s32 daObj_IkadaDraw(daObj_Ikada_c *actor) {
    return actor->_draw();
}
/* 0236517C */
s32 daObj_IkadaIsDelete(daObj_Ikada_c *actor) {
    return 1;
}

// HD: compiler-generated and template code of this translation unit.
/* 0236513C */
void hioDestructor(void *self, s32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}
/* 02365150 */
cXyz *vectorConstructor(cXyz *self) {
    return self ? self : static_cast<cXyz *>(operator_new(12));
}
/* 02365248 */
void emptyVirtual(daObj_Ikada_c *actor) {
}
/* 02365194 */
void actorDestructor(daObj_Ikada_c *actor, s32 flags) {
    if (!actor)
        return;
    ikadaApi_destroyObjectStatus(gabi::ea(&actor->mStts), 2);
    ikadaApi_destroyCollisionSphere(gabi::ea(&actor->mSph), 2);
    ikadaApi_destroyRopeMaterial(gabi::ea(&actor->mRopeLine), 2);
    ikadaApi_destroyAcchCircle(gabi::ea(&actor->mAcchCir.m_cir), 2);
    actor->mObjAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0];
    actor->mObjAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
    ikadaApi_destroyAcch(gabi::ea(&actor->mObjAcch), 0);
    ikadaApi_destroyShaderUniform(gabi::ea(&actor->mBckAnm.mpAnm), 0);
    ikadaApi_fopAc_ac_c_fopAc_ac_c(actor, 0);
    if (flags & 1)
        operator_delete(actor);
}
/* 02364E6C */
void *hioConstructor(void *self) {
    if (!self) {
        self = static_cast<void *>(operator_new(0xAC));
        if (!self)
            return nullptr;
    }
    auto* hio = static_cast<daObj_Ikada_HIO_c*>(self);
    hio->__vtbl = 0x1002B364;
    hio->mbDebugDraw = 0;
    hio->m05 = 0;
    hio->m06 = 0;
    hio->mbNoRotAnim = 0;
    hio->m08 = 0;
    hio->m1C = 0;
    hio->m1E = 100;
    hio->m24 = 60;
    hio->m94 = 3000;
    hio->mPlayerStopDistance = 6000;
    hio->m98 = 1000;
    hio->m9A = 1000;
    hio->mFlagOffset.x = 0;
    hio->mFlagOffset.y = 0;
    hio->mFlagOffset.z = 0;
    hio->mFlagScale = 0;
    hio->m20 = 1.5f;
    hio->m28 = 100;
    hio->mShipOffsY_Attention = 900;
    hio->mShipOffsY_Eye = 300;
    hio->mTerryWaveOffsZ = 660;
    hio->mTerryWaveOffsY = 20;
    hio->mTerryTrackOffsZ = -180;
    hio->mSvWaveOffsX = -320;
    hio->mSvTrackOffsX = 240;
    hio->mSvOffsX[0] = 265;
    hio->mSvOffsX[1] = -18;
    hio->mSvOffsX[2] = -97;
    hio->mSvOffsX[3] = -175;
    hio->mTrackIndTransY = -0.04f;
    hio->mTrackIndScaleY = 4;
    hio->mSplashScaleMax = 200;
    hio->mSplashMaxScaleTimer = 300;
    hio->mWaveVelFade = 2;
    hio->mTrackVel = 300;
    hio->mWaveVelSpeed = 2;
    hio->mWaveVelOffs = 0;
    hio->mWaveMaxVelocity = 15;
    hio->mWaveCollapsePos0.x = -80;
    hio->mWaveCollapsePos0.y = -50;
    hio->mWaveCollapsePos0.z = -150;
    hio->mWaveCollapsePos1.x = -40;
    hio->mWaveCollapsePos1.y = -100;
    hio->mWaveCollapsePos1.z = -350;
    hio->m9C = 300;
    hio->mVelocityTargetTerry1 = 12;
    hio->mVelocityTargetTerry3 = 15;
    hio->mA8 = 30;
    return self;
}
/* 0236505C */
void __sinit_d_a_obj_ikada_cpp() {
    gabi::store<u32>(0x1046A19C, 0);
    gabi::store<u32>(0x1046A1A4, 0);
    gabi::store<u32>(0x1046A198, 0);
    gabi::store<u32>(0x1046A1A0, 0);
    ikadaApi_registerGlobalObject(0x101CA4DC);
    gabi::store<f32>(0x1046A09C, -3.1415927410125732f);
    gabi::store<f32>(0x1046A0A0, 3.1415927410125732f);
    ikadaApi_initializeMatrixStatics(0x1046A0A4);
    ikadaApi_registerGlobalObject(0x101CA4E8);
    ikadaApi_initializeMathStatics(0x1046A0A5);
    ikadaApi_registerGlobalObject(0x101CA4F4);
    m_crane_offset.x = 200.0f;
    m_rope_base_vec.x = 0.0f;
    m_rope_base_vec.y = -10.0f;
    m_crane_offset.y = -340.0f;
    m_crane_offset.z = 0.0f;
    m_rope_base_vec.z = 0.0f;
    hioConstructor(&l_HIO);
}
/* 02361004 */
daObj_Ikada_c *actorConstructor(daObj_Ikada_c *actor) {
    if (!actor) {
        actor = static_cast<daObj_Ikada_c *>(operator_new(0x16A0));
        if (!actor)
            return nullptr;
    }
    u32 p = gabi::ea(actor);
    ikadaApi_fopAc_ac_c_fopAc_ac_c(actor);
    actor->__vtbl = 0x1002B354;
    actor->mWaveLCallback.__vtbl = 0x100521A8;
    for (u32 off : {0x450u, 0x45Cu, 0x468u})
        if (!(p + off))
            operator_new(12);
    actor->mWaveRCallback.__vtbl = 0x100521A8;
    for (u32 off : {0x4B4u, 0x4C0u, 0x4CCu})
        if (!(p + off))
            operator_new(12);
    actor->mSplashCallBack.__vtbl = 0x100521E8;
    actor->mTrackCallBack.__vtbl = 0x10052268;
    ikadaApi_constructArray(p + 0x508, 3, 12, gabi::guest_fn(reinterpret_cast<const void*>(vectorConstructor)));
    ikadaApi_constructRippleCallback(gabi::ea(&actor->mRippleCallBack));
    ikadaApi_dPa_followEcallBack_dPa_followEcallBack(gabi::ea(&actor->mBombSmokeEasterEgg), 0, 0);
    ikadaApi_dPa_followEcallBack_dPa_followEcallBack(gabi::ea(&actor->mFireParticle), 0, 0);
    ikadaApi_J3DFrameCtrl_init(gabi::ea(&actor->mBckAnm.mFrameCtrl), 0);
    gabi::store<u32>(p + 0x664, 0x1016E54C);
    ikadaApi_constructShaderUniform(p + 0x668);
    gabi::store<u32>(p + 0x6DC, 0);
    gabi::store<u32>(p + 0x6D0, 0);
    gabi::store<u32>(p + 0x664, 0x1002B2FC);
    gabi::store<u32>(p + 0x6D8, 0);
    gabi::store<u32>(p + 0x69C, 0x1016D820);
    gabi::store<u32>(p + 0x6D4, 0);
    gabi::store<u32>(p + 0x6AC, 0);
    ikadaApi_dBgS_Acch_dBgS_Acch(gabi::ea(&actor->mObjAcch));
    actor->mObjAcch.cBgS_Chk::__vtbl = gc_tu.objAcch.v10;
    actor->mObjAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    actor->mObjAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    actor->mObjAcch.mbObjThrough = 1;
    ikadaApi_dBgS_AcchCir_dBgS_AcchCir(gabi::ea(&actor->mAcchCir));
    ikadaApi_constructRopeMaterial(gabi::ea(&actor->mRopeLine));
    ikadaApi_mDoExt_btkAnm_init(gabi::ea(&actor->mSeaBtk));
    ikadaApi_constructCollisionSphere(gabi::ea(&actor->mSph));
    ikadaApi_constructCollisionStatus(gabi::ea(&actor->mStts));
    ikadaApi_dCcD_GStts_dCcD_GStts(p + 0x1620);
    actor->mStts.__vtbl_gstts = 0x1004AEC0;
    actor->mStts.__vtbl = 0x1004AE88;
    actor->mPLight.mHD20 = 1.0f;
    return actor;
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
ext 025D5834 u32 ikadaApi_createActor(s32, u32, cXyz*, s8, csXyz*, s32, s32, s32)
ext 028E93CC void ikadaApi_setTranslationMatrix(Mtx34*, f32, f32, f32)
ext 025E1A40 void ikadaApi_startSound(u32, cXyz*, s32, s32)
ext 025A847C void ikadaApi_setParticle(dPa_control_c*, s32, s32, cXyz*, csXyz*, cXyz*, s32, void*, s32, void*, void*, void*)
ext 02516300 u32 ikadaApi_getTargetHitObject(dCcD_Sph*)
ext 028E8F64 void ikadaApi_transformVector(Mtx34*, cXyz*, cXyz*)
ext 025F1C28 void ikadaApi_rotateY(Mtx34*, s32)
ext 025F1BF4 void ikadaApi_rotateX(Mtx34*, s32)
ext 025F1B48 void ikadaApi_rotateZXY(Mtx34*, s32, s32, s32)
ext 028E90D4 void ikadaApi_copyMatrix(Mtx34*, Mtx34*)
ext 025871F8 f32 ikadaApi_getWaterY(cXyz*, dBgS_ObjAcch*)
ext 0201AE48 void ikadaApi_multiplyVector(cXyz*, cXyz*, f32)
ext 0201AD78 void ikadaApi_addVector(cXyz*, cXyz*, cXyz*)
ext 0201ADE0 void ikadaApi_subtractVector(cXyz*, cXyz*, cXyz*)
ext 020CB648 s32 ikadaApi_daBomb_c_getBombRestTime(fopAc_ac_c *)
ext 02018C8C void ikadaApi_cM3dGSph_SetR(u32, f32)
ext 02018D40 void ikadaApi_cM3dGSph_SetC(u32, cXyz *)
ext 0200E240 void ikadaApi_cCcS_Set(u32, dCcD_Sph *)
ext 02587D24 void ikadaApi_dLib_pathMove(cXyz *, be<s8> *, u32, s32, daObj_Ikada_c *, f32)
ext 0200F268 void ikadaApi_cLib_addCalcPosXZ2(cXyz *, cXyz *, f32, f32)
ext 0200F93C s32 ikadaApi_cLib_targetAngleY(cXyz *, cXyz *)
ext 02515E50 void ikadaApi_dCcD_GStts_Move(u32)
ext 0201B47C s32 ikadaApi_cXyz_normalizeRS(cXyz *)
ext 020195B0 s32 ikadaApi_cM_atan2s(f32, f32)
ext 0200F378 u32 ikadaApi_cLib_addCalcAngleS(be<s16> *, s32, s32, s32, s32)
ext 0200F764 void ikadaApi_cLib_chasePosXZ(cXyz *, cXyz *, f32)
ext 02555D0C u32 ikadaApi_getEnvironment()
ext 025EC62C void ikadaApi_mDoExt_3DlineMat1_c_update(u32, u16, s32, s32, u32, f32)
ext 025EDD04 void ikadaApi_mDoExt_3DlineMatSortPacket_setMat(u32, u32)
ext 025BED80 void ikadaApi_dSnap_RegistFig(s32, daObj_Ikada_c *, f32, f32, f32)
ext 02515860 void ikadaApi_destroyObjectStatus(u32, s32)
ext 02515AE8 void ikadaApi_destroyCollisionSphere(u32, s32)
ext 025EB8B8 void ikadaApi_destroyRopeMaterial(u32, s32)
ext 02018034 void ikadaApi_destroyAcchCircle(u32, s32)
ext 024EFD9C void ikadaApi_destroyAcch(u32, s32)
ext 027F3628 void ikadaApi_destroyShaderUniform(u32, s32)
ext 025D50BC void ikadaApi_fopAc_ac_c_fopAc_ac_c(daObj_Ikada_c *, s32)
ext 025204C8 void ikadaApi_dComIfG_resDelete(request_of_phase_process_class *, const char*)
ext 025A92C0 void ikadaApi_dPa_waveEcallBack_remove(IkadaWaveCallback *)
ext 025A99B8 void ikadaApi_dPa_splashEcallBack_remove(IkadaSplashCallback *)
ext 025A9E38 void ikadaApi_dPa_trackEcallBack_remove(IkadaTrackCallback *)
ext 025A9270 void ikadaApi_dPa_rippleEcallBack_end(IkadaRippleCallback *)
ext 025E1B34 void ikadaApi_JAIZelBasic_seDeleteObject(cXyz *)
ext 0255A374 void ikadaApi_dKy_plight_cut(LIGHT_INFLUENCE *)
ext 020087EC void ikadaApi_cBgS_Release(u32, u32)
ext 028F026C void ikadaApi_registerGlobalObject(s32)
ext 028ED6F8 void ikadaApi_initializeMatrixStatics(s32)
ext 028EAB2C void ikadaApi_initializeMathStatics(s32)
ext 025D6958 f32 ikadaApi_fopAcM_searchActorDistanceXZ(daObj_Ikada_c *, u32)
ext 025D4ED0 void ikadaApi_fopAc_ac_c_fopAc_ac_c(daObj_Ikada_c *)
ext 028EFFD0 void ikadaApi_constructArray(u32, s32, s32, s32)
ext 025A9084 void ikadaApi_constructRippleCallback(u32)
ext 025A5894 void ikadaApi_dPa_followEcallBack_dPa_followEcallBack(u32, s32, s32)
ext 027F2BC0 void ikadaApi_J3DFrameCtrl_init(u32, s32)
ext 027DA984 void ikadaApi_constructShaderUniform(u32)
ext 024F0474 void ikadaApi_dBgS_Acch_dBgS_Acch(u32)
ext 024EFE94 void ikadaApi_dBgS_AcchCir_dBgS_AcchCir(u32)
ext 025EB82C void ikadaApi_constructRopeMaterial(u32)
ext 025E7C6C void ikadaApi_mDoExt_btkAnm_init(u32)
ext 025166F0 void ikadaApi_constructCollisionSphere(u32)
ext 0200BD2C void ikadaApi_constructCollisionStatus(u32)
ext 02515DA0 void ikadaApi_dCcD_GStts_dCcD_GStts(u32)
ext 0273AA24 void ikadaApi_assertionShow(const char*, s32, const char*)
ext 02520460 s32 ikadaApi_dComIfG_resLoad(request_of_phase_process_class *, const char*)
ext 025D63E8 s32 ikadaApi_fopAcM_entrySolidHeap(daObj_Ikada_c *, s32, u32)
ext 02520540 s32 ikadaApi_dComIfGp_getReverb(s32)
ext 026066C4 u32 ikadaApi_dRes_control_c_getRes(u32, IkadaSafeString*, u32)
ext 02606900 u32 ikadaApi_getNamedResource(u32, IkadaSafeString*, IkadaSafeString*)
ext 025E38E0 J3DModel * ikadaApi_mDoExt_J3DModel_create(u32, s32, s32)
ext 025E8508 s32 ikadaApi_mDoExt_bckAnm_init(mDoExt_bckAnm *, u32, u32, s32, s32, s32, s32, s32, f32)
ext 027F3F94 u32 ikadaApi_getJointTree(u32)
ext 025E7CE0 s32 ikadaApi_initSeaTextureAnimation(mDoExt_btkAnm *, u32, u32, s32, s32, s32, s32, s32, s32, f32)
ext 024F23F4 dBgW * ikadaApi_dBgW_dBgW(s32)
ext 0200A030 s32 ikadaApi_cBgW_Set(u32, u32, s32, Mtx34 *)
ext 025EBA58 s32 ikadaApi_mDoExt_3DlineMat1_c_init(u32, s32, s32, u32, s32)
ext 025AAF88 path_layout::dPath * ikadaApi_dPath_GetRoomPath(u8, s8)
ext 025D5AA4 u32 ikadaApi_fopAcM_createChild(u32, u32, s32, cXyz *, s8, s32, s32, s32)
ext 024F43DC void ikadaApi_dBgW_Move(u32)
ext 024EEA6C void ikadaApi_dBgS_Regist(u32, u32, daObj_Ikada_c *)
ext 024EFF44 void ikadaApi_dBgS_AcchCir_SetWall(dBgS_AcchCir *, f32, f32)
ext 024F06B4 void ikadaApi_dBgS_Acch_Set(dBgS_ObjAcch *, cXyz *, cXyz *, daObj_Ikada_c *, s32, dBgS_AcchCir *, cXyz *, s32, s32)
ext 025564B4 void ikadaApi_dKy_plight_set(LIGHT_INFLUENCE *)
ext 025D6870 void ikadaApi_fopAcM_posMoveF(daObj_Ikada_c *, s32)
ext 025D674C void ikadaApi_fopAcM_setCullSizeBox(daObj_Ikada_c *, f32, f32, f32, f32, f32, f32)
ext 025F24E0 void ikadaApi_mDoMtx_stack_c_transM(f32, f32, f32)
ext 02515F14 void ikadaApi_dCcD_Stts_Init(dCcD_Stts *, s32, s32, daObj_Ikada_c *)
ext 0251677C void ikadaApi_dCcD_Sph_Set(dCcD_Sph *, s32)
ext 02556D14 s32 ikadaApi_dKy_daynight_check()
ext 025A5AC8 void ikadaApi_dPa_followEcallBack_end(IkadaFollowCallback *)
ext 025A8D40 void ikadaApi_dPa_control_c_setSimple(u32, s32, cXyz *, s32, s32, s32, s32)
ext 025D6B70 s32 ikadaApi_fopAcM_checkCullingBox(u32, f32, f32, f32, f32, f32, f32)
ext 025D5218 u32 ikadaApi_fopAcIt_Judge(s32, gabi::be<unsigned int> *)
ext 0201B3C0 void ikadaApi_cXyz_normalizeZP(cXyz *, cXyz *)
ext 0201AEAC void ikadaApi_cXyz_dv(cXyz *, cXyz *, f32)
ext 025DAE64 void ikadaApi_fopKyM_createWpillar(cXyz *, s32, f32, f32)
ext 025872F4 void ikadaApi_dLib_waveRot(cXyz *, IkadaWave *, f32)
ext 025F1884 void ikadaApi_mDoMtx_YrotS(Mtx34*, s16)
ext 0200FCD8 void ikadaApi_MtxPosition(cXyz *, cXyz *)
ext 025F19F8 void ikadaApi_mDoMtx_XYZrotM(Mtx34*, s16, s32, s16)
ext 025D54C4 s32 ikadaApi_fopAcM_SearchByID(u32, gabi::be<unsigned int> *)
ext 025F2518 void ikadaApi_mDoMtx_stack_c_scaleM(f32, f32, f32)
ext 025E742C void ikadaApi_mDoExt_baseAnm_play(mDoExt_btkAnm *)
ext 0201B080 void ikadaApi_cXyz_outprod(cXyz *, cXyz *, cXyz *)
ext 025602F0 void ikadaApi_dKy_get_seacolor(GXColor*, GXColor*)
ext 0274D458 void ikadaApi_scaleColorBrightness(IkadaColorVector*, IkadaColorVector*, f32)
ext 027F9F0C u32 ikadaApi_getMaterialColor(u32, s32)
data l_HIO 1046A1FC
data debug_yellow_guard 101FDA44
data debug_yellow 101FEBE8
data debug_yellow_source 1002B2C8
data debug_blue_guard 101FDAC0
data debug_blue 101FEBF8
data debug_blue_source 1002B2CC
data debug_red_guard 101FDA50
data debug_red 101FEBF4
data debug_red_source 1002B2D0
data ikada_vector_zero 101FFBA8
data ikada_vector_up 101FFBC0
str 1002B808 "IkadaH"
str 1002B5B4 "Always"
str 1002B5BC "Link"
str 1002B3A8 "d_a_obj_ikada.cpp" in 0235FE84
str 1002B5EC "d_a_obj_ikada.cpp" in 02360C64
str 1002B3BC "(jntNo >= 0) && (jntNo < JNT_END)"
str 1002B600 "modelData != (0)"
str 1002B614 "bck != 0"
data sea_quad_a_guard 1046A2D4
data sea_quad_a 1046A0A8
data sea_quad_b_guard 1046A2D8
data sea_quad_b 1046A0D8
data sea_quad_c_guard 1046A2DC
data sea_quad_c 1046A108
data sea_quad_d_guard 1046A2E0
data sea_quad_d 1046A138
data sea_cross_guard 1046A2E4
data sea_cross_points 1046A168
data control 101CA49C
data sea_bdl 101CA4B0
data sea_btk 101CA4C4
str 1002B620 "vikae_minamo00.bdl"
str 1002B634 "vtsp_minamo00.bdl"
str 1002B648 "vikah_minamo00.bdl"
str 1002B65C "vtsp2_minamo00.bdl"
str 1002B670 "vsvsp_minamo00.bdl"
str 1002B684 "vikae_minamo00.btk"
str 1002B698 "vtsp_minamo00.btk"
str 1002B6AC "vikah_minamo00.btk"
str 1002B6C0 "vtsp2_minamo00.btk"
str 1002B6D4 "vsvsp_minamo00.btk"
data flag_offset_guard 1046A2C8
data fire_scale_guard 1046A2CC
data ripple_scale_guard 1046A2D0
data fire_scale 1046A1B4
data ripple_scale 1046A1A8
data flag_offset 1046A1C0
data wave_l_guard 1046A2A8
data wave_r_guard 1046A2AC
data wave_l_direction 1046A2B0
data wave_r_direction 1046A2BC
data m_rope_base_vec 1046A2E8
data m_crane_offset 1046A2F4
data m_heapsize 1002B850
data bdl 1002B5C4
data dzb 1002B5D8
data flagparam 1002B374
data flag_scale 1002B388
data mode_tbl 1002B3E0
ext 0211D2F8 s32 ikadaApi_calcTimer(be<s32>*)
*/
