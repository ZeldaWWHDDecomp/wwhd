/** Great Sea enemy warship. GameCube names and layout with verified WWHD semantics.
 * The two Oship specs verify every marked function against the original HD executable.
 */
#include "d/actor/d_a_oship.h"
static OshipDebugRegisters l_debugRegisters;
static OshipHeaderStatics l_headerStatics;
static OshipHeaderRecords l_headerRecords;
static OshipResourceControlSlot l_resControl;
static OshipCylinderTable l_cylinderOffsets;
static OshipCylinderTable l_cylinderRadii;
static OshipCylinderTable l_cylinderHeights;
static cXyz l_waveDirection1;
static cXyz l_waveDirection2;
static cXyz l_flagOffset;
static u32 l_waveDirection1Guard;
static u32 l_waveDirection2Guard;
static u32 l_flagOffsetGuard;
static daOship_HIO_c l_HIO;
static OshipModes mode_tbl;
static dCcD_SrcCyl m_cyl_src;
static Mtx34 l_matrixStack;
static const f32 l_zero = 0.0f;
static const f32 l_one = 1.0f;
static const f32 l_waveScaleZ = -0.300000011921f;
static const f32 l_flagScale = 0.5f;
static const f32 l_trackScale = -0.5f;
static const f32 l_smokeScale = 3.0f;
static const f32 l_wavePhaseRange = 32768.0f;
static const f32 l_cullMaxZ = 800.0f;
static const f32 l_cullMinX = -300.0f;
static const f32 l_cullMinY = -100.0f;
static const f32 l_cullMinZ = -650.0f;
static const f32 l_cullMaxX = 300.0f;
static const f32 l_cullMaxY = 700.0f;
static const f32 l_ten = 10.0f;
static const f32 l_wallRadius = 30.0f;
static const f32 l_two = 2.0f;
static const f32 l_noWaterLevel = -1000000000.0f;
static const f32 l_smoothingRate = 0.10000000149f;
static const f32 l_bombHeightBias = -20.0f;
static const f32 l_maxWaveDistance = 18000.0f;
static const f32 l_hundred = 100.0f;
static const f32 l_badAimExtra = 3000.0f;
static const f32 l_damageScale = 500.0f;
static const f32 l_pathRadiusBias = 0.009999999776f;
static const f32 l_swayScale = 0.025000000373f;
static const f32 l_sinkOffset = -1000.0f;
static const f32 l_sinkSmoothing = 0.019999999553f;
static const f32 l_sinkMaxStep = 50.0f;
static const f32 l_fireDelayRange = 20.0f;
static const f32 l_defaultTrackOffsY = -0.039999999106f;
static const f32 l_defaultTrackScaleY = 4.0f;
static const f32 l_defaultAttentionOffsY = 900.0f;
static const f32 l_defaultWaveOffsZ = 380.0f;
static const f32 l_defaultRangeA = 2500.0f;
static const f32 l_defaultRangeB = 6000.0f;
static const f32 l_defaultRangeC = 12000.0f;
static const f32 l_defaultSplashScaleTarget = 200.0f;
static const f32 l_defaultMaxWaveVelocity = 15.0f;
static const f32 l_defaultCollapseStartX = -80.0f;
static const f32 l_defaultBombOffsY = -50.0f;
static const f32 l_defaultCollapseStartZ = -150.0f;
static const f32 l_defaultCollapseEndX = -40.0f;
static const f32 l_defaultCollapseEndY = -350.0f;
static const f32 l_defaultBombOffsX = 450.0f;
static const f32 l_defaultBombSpeed = 85.0f;
static const f32 l_defaultBombAcceleration = -2.0f;
static const f32 l_defaultBadAimDistance = 3500.0f;
static const f32 l_defaultPathSpeed = 0.75f;
static const f32 l_negativePi = -3.141592741013f;
static const f32 l_positivePi = 3.141592741013f;

// HD: common range update is inlined into the three range modes.
void OshipRangeUpdate(daOship_c* ship, bool allowSubmode1) {
    u8 submode = ship->mSubMode;
    if ((submode == 2 || (allowSubmode1 && submode == 1)) && ship->mPathId != 0xFF) {
        ship->mVelocityFwdTarget = f32(l_HIO.mPathSpeed);
        ship->pathMove();
    }
    u32 play = dComIfGp_ea();
    u32 player = u32(OshipGuest<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    OshipBits<u32>(ship->mTargetPos.x) =
        OshipBits<u32>(OshipGuest<fopAc_ac_c>(player)->current.pos.x);
    OshipBits<u32>(ship->mTargetPos.y) =
        OshipBits<u32>(OshipGuest<fopAc_ac_c>(player)->current.pos.y);
    OshipBits<u32>(ship->mTargetPos.z) =
        OshipBits<u32>(OshipGuest<fopAc_ac_c>(player)->current.pos.z);
    ship->current.pos.y = OshipGetWaterY(&ship->current.pos, &ship->mAcch);
    if (ship->checkTgHit())
        return;
    if (ship->plFireRepeat())
        return;
    if (!OshipCalcTimer(&ship->mAttackTimer))
        ship->modeProc(daOship_c::PROC_INIT_e, daOship_c::MODE_ATTACK);
    else
        ship->changeModeByRange();
}

/* 023C547C */
BOOL createHeap_CB(daOship_c* ship) { return ship->_createHeap(); }

/* 023C52D8 */
s32 nodeControl_CB(void* node, s32 timing) {
    if (timing == 0) {
        u32 model = gabi::ea(j3dSys.getModel());
        u32 userArea = OshipGuest<J3DModel>(model)->mUserArea;
        if (userArea)
            OshipGuest<daOship_c>(userArea)->_nodeControl(node, OshipGuest<J3DModel>(model));
    }
    return 1;
}

/* 023C5054 */
void daOship_c::_nodeControl(void* node, void* model) {
    u32 joint = OshipJointDescriptor(node);
    u16 index = u16(OshipGuest<OshipJointData>(joint)->mJntNo);
    u32 modelEA = gabi::ea(model);
    u32 matrices = u32(OshipGuest<J3DModel>(modelEA)->mpMtxBuffer.v);
    u32 matrixData = u32(OshipGuest<J3DMtxBuffer_hd>(matrices)->mpAnmMtx);
    u16 flags = u16(OshipGuest<J3DMtxBuffer_hd>(matrices)->mFlags);
    OshipGuest<J3DMtxBuffer_hd>(matrices)->mFlags = flags | 0x10;
    auto* matrix = &l_matrixStack;
    OshipMatrixCopy(OshipGuest<Mtx34>(matrixData + u32(index) * 48), matrix);
    gabi::Local<csXyz> rotation;
    OshipRotationConstruct(rotation.get(), 0, 0, 0);
    if (index == 1)
        rotation->x = this->mAimRotY;
    else if (index == 2) {
        s16 nodeRotation = s16(l_HIO.mNode2RotZ);
        OshipMatrixZrotM(matrix, nodeRotation);
        rotation->z = s16(-s32(s16(this->mAimRotX)));
    }
    s16 y = rotation->y, x = rotation->x, z = rotation->z;
    OshipMatrixZXYrotM(matrix, x, y, z);
    OshipMatrixCopy(matrix, J3DSys::mCurrentMtx);
    matrices = u32(OshipGuest<J3DModel>(modelEA)->mpMtxBuffer.v);
    flags = u16(OshipGuest<J3DMtxBuffer_hd>(matrices)->mFlags);
    OshipGuest<J3DMtxBuffer_hd>(matrices)->mFlags = flags | 0x10;
    // HD loads all twelve floating values before writing the joint matrix.
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = l_matrixStack.m[i / 4][i % 4];
    matrixData = u32(OshipGuest<J3DMtxBuffer_hd>(matrices)->mpAnmMtx) + u32(index) * 48;
    for (u32 i = 0; i < 12; ++i)
        OshipGuest<Mtx34>(matrixData)->m[i / 4][i % 4] = values[i];
}

/* 023C5658 */
BOOL pathMove_CB(cXyz* position, cXyz* start, cXyz* end, daOship_c* ship) {
    return ship->_pathMove(position, start, end);
}

/* 023C5480 */
BOOL daOship_c::_pathMove(cXyz* position, cXyz* start, cXyz* end) {
    OshipBits<u32>(this->mPathP0.x) = u32(OshipBits<u32>(start->x));
    u32 startY = u32(OshipBits<u32>(start->y));
    f32 height = this->current.pos.y;
    OshipBits<u32>(this->mPathP0.y) = startY;
    u32 startZ = u32(OshipBits<u32>(start->z));
    this->mPathP0.y = height;
    OshipBits<u32>(this->mPathP0.z) = startZ;
    OshipBits<u32>(this->mPathP1.x) = u32(OshipBits<u32>(end->x));
    OshipBits<u32>(this->mPathP1.y) = u32(OshipBits<u32>(end->y));
    u32 endZ = u32(OshipBits<u32>(end->z));
    this->mPathP1.y = height;
    OshipBits<u32>(this->mPathP1.z) = endZ;
    gabi::Local<cXyz> direction, delta, distanceVector, finalDelta, finalVector;
    OshipVectorSubtract(&this->mPathP1, direction.get(), &this->mPathP0);
    if (!OshipVectorNormalizeRS(direction.get()))
        return TRUE;
    f32 z = direction->z, x = direction->x;
    s32 angle = OshipAtan2s(x, z);
    u16 difference = OshipAddCalcAngleS(&this->current.angle.y, angle, 8, 0x200, 8);
    f32 cosine = cM_scos(difference);
    f32 speed = f32(this->speedF);
    f32 step = gabi::fmuls_ppc(speed, std::fabs(cosine));
    OshipChasePosXZ(position, &this->mPathP1, step);
    OshipVectorSubtract(position, delta.get(), &this->mPathP1);
    x = delta->x;
    f32 zero = l_zero;
    z = delta->z;
    distanceVector->y = zero;
    distanceVector->x = x;
    distanceVector->z = z;
    f32 square = PSVECSquareMag(distanceVector.get());
    // HD: retain the double result through the distance comparison.
    f64 distance = OshipSqrt(f64(square));
    f32 factor = f32(l_debugRegisters.mFloat[5]);
    factor += l_one;
    f32 threshold = gabi::fmuls_ppc(step, factor);
    if (distance < threshold)
        return TRUE;
    OshipVectorSubtract(position, finalDelta.get(), &this->mPathP1);
    x = finalDelta->x;
    z = finalDelta->z;
    finalVector->x = x;
    finalVector->y = zero;
    finalVector->z = z;
    square = PSVECSquareMag(finalVector.get());
    distance = OshipSqrt(f64(square));
    return distance == zero;
}

/* 023C7084 */
void daOship_c::pathMove() {
    // HD: pathMove receives callback, actor, then speed; GC places speed first.
    f32 target = this->mVelocityFwdTarget;
    f32 rate = l_smoothingRate, step = l_two;
    cLib_addCalc2(&this->speedF, target, rate, step);
    f32 speed = f32(this->speedF);
    u32 path = this->mpPath.v;
    OshipPathMoveHD(&this->mOrigPos, &this->m2D4, OshipGuest<dPath>(path),
                    gabi::guest_fn((const void*)pathMove_CB), this, speed);
    f32 radius = f32(l_debugRegisters.mFloat[0]);
    radius += l_pathRadiusBias;
    speed = f32(this->speedF);
    OshipAddCalcPosXZ2(&this->current.pos, &this->mOrigPos, radius, speed);
    speed = f32(this->speedF);
    f32 zero = l_zero;
    if (speed != zero && f32(this->mVelocityFwdTarget) != zero) {
        s32 yaw = OshipTargetAngleY(&this->current.pos, &this->mOrigPos);
        OshipAddCalcAngleS2(&this->shape_angle.y, yaw, 8, 0x100);
    }
}

/* 023C7CA8 */
BOOL daOship_c::plFireRepeat() {
    u32 play = dComIfGp_ea();
    u32 boat = u32(OshipGuest<dComIfG_inf_c>(play)->play.mpPlayerPtr[2].v);
    if (boat && (u32(OshipGuest<OshipBoatState>(boat)->mStateFlags) & 0x20000)) {
        f32 range = l_fireDelayRange;
        f32 delay = cM_rndF(range);
        delay += l_ten;
        s32 count = this->mAimCounter;
        this->mPlFireTimer = gabi::ftoi(delay);
        this->mAimCounter = u32(count) + 1;
    }
    if (this->mPlFireTimer != -1 && !OshipCalcTimer(&this->mPlFireTimer)) {
        this->modeProc(PROC_INIT_e, MODE_ATTACK);
        this->mPlFireTimer = -1;
        return TRUE;
    }
    return FALSE;
}

/* 023C747C */
s32 daOship_c::lineCheck(cXyz* start, cXyz* end) {
    this->mLinChk.Set(start, end, this);
    u32 play = dComIfGp_ea();
    return OshipLineCross(&OshipGuest<dComIfG_inf_c>(play)->play.mBgS, &this->mLinChk);
}

/* 023C59A4 */
void daOship_c::changeModeByRange() {
    u32 play = dComIfGp_ea();
    u32 player = u32(OshipGuest<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    f32 distance = OshipActorDistanceXZ(this, OshipGuest<fopAc_ac_c>(player));
    s32 mode;
    if (distance < f32(l_HIO.mDistRangeA))
        mode = MODE_RANGE_A;
    else if (distance < f32(l_HIO.mDistRangeB))
        mode = MODE_RANGE_B;
    else
        mode = distance < f32(l_HIO.mDistRangeC) ? MODE_RANGE_C : MODE_RANGE_D;
    if (u32(this->mCurrentProc) != u32(mode))
        this->modeProc(PROC_INIT_e, mode);
}

/* 023C5D34 */
void daOship_c::createWave() {
    u32 initialized = l_waveDirection1Guard;
    f32 unit = l_one, scaleZ = l_waveScaleZ;
    if (!initialized) {
        l_waveDirection1.y = unit;
        f32 scaleX = l_flagScale;
        l_waveDirection1Guard = 1;
        l_waveDirection1.x = scaleX;
        l_waveDirection1.z = scaleZ;
    }
    if (!l_waveDirection2Guard) {
        l_waveDirection2.y = unit;
        f32 scaleX = l_trackScale;
        l_waveDirection2.z = scaleZ;
        l_waveDirection2.x = scaleX;
        l_waveDirection2Guard = 1;
    }
    for (u32 i = 0; i < 2; ++i) {

        if (!u32((i == 0 ? mWaveCallback1 : mWaveCallback2).mpBaseEmitter)) {
            u32 play = dComIfGp_ea();
            u32 manager = u32(OshipGuest<dComIfG_inf_c>(play)->play.mParticle.v);
            OshipParticleSet(OshipGuest<dPa_control_c>(manager), 0, 0x37, &this->mWavePos,
                             &this->mWaveRot, 0, 255,
                             &(i == 0 ? this->mWaveCallback1 : this->mWaveCallback2), -1, 0, 0, 0);
            u32 emitter = u32((i == 0 ? mWaveCallback1 : mWaveCallback2).mpBaseEmitter);
            if (emitter) {
                u32 scale = i == 0 ? gabi::ea(&l_waveDirection1) : gabi::ea(&l_waveDirection2);
                OshipGuest<OshipEmitter>(emitter)->mLocalDirection.x = OshipGuest<cXyz>(scale)->x;
                OshipGuest<OshipEmitter>(emitter)->mLocalDirection.y = OshipGuest<cXyz>(scale)->y;
                OshipGuest<OshipEmitter>(emitter)->mLocalDirection.z = OshipGuest<cXyz>(scale)->z;
            }
        }
    }
    if (!u32(this->mSplashCallback.mpBaseEmitter)) {
        u32 play = dComIfGp_ea();
        u32 manager = u32(OshipGuest<dComIfG_inf_c>(play)->play.mParticle.v);
        OshipParticleSet(OshipGuest<dPa_control_c>(manager), 0, 0x35, &this->mWavePos,
                         &this->mWaveRot, 0, 255, &this->mSplashCallback, -1, 0, 0, 0);
    }
    if (!u32(this->mTrackCallback.mpBaseEmitter)) {
        u32 play = dComIfGp_ea();
        u32 manager = u32(OshipGuest<dComIfG_inf_c>(play)->play.mParticle.v);
        OshipParticleSet(OshipGuest<dPa_control_c>(manager), 5, 0x36, &this->mTrackPos,
                         &this->shape_angle, 0, 0, &this->mTrackCallback, -1, 0, 0, 0);
        u32 emitter = u32(this->mTrackCallback.mpBaseEmitter);
        if (emitter) {
            f32 scale = l_smokeScale;
            auto* particle = OshipGuest<OshipEmitter>(emitter);
            particle->mGlobalDynamicsScale.y = scale;
            particle->mGlobalParticleScale.z = scale;
            particle->mGlobalParticleScale.x = scale;
            particle->mGlobalParticleScale.y = scale;
            particle->mGlobalDynamicsScale.x = scale;
            particle->mGlobalDynamicsScale.z = scale;
        }
    }
}

/* 023C65C0 */
void daOship_c::setWave() {
    f32 cutoff = l_two;
    f32 speed = f32(this->speedF);
    f32 splashTarget = f32(l_HIO.mSplashScaleTimerTarget);
    f32 fade = f32(l_HIO.mWaveVelFade1);
    f32 maximum = f32(l_HIO.mWaveMaxVelocity);
    f32 trackVelocity = f32(l_HIO.mTrackVel);
    f32 zero = l_zero;
    if (!(speed > cutoff) || this->mCurrentProc == MODE_DELETE) {
        this->mTrackCallback.mState = 1;
        fade = zero;
        splashTarget = zero;
    } else
        this->createWave();
    f32 water = OshipGetWaterY(&this->mWavePos, &this->mAcch);
    u32 trackEmitter = u32(this->mTrackCallback.mpBaseEmitter);
    s16 yaw = s16(this->shape_angle.y);
    this->mWavePos.y = water;
    this->mWaveRot.y = yaw;
    if (trackEmitter) {
        f32 trackY = f32(l_HIO.mTrackOffsY);
        u32 collisionFlags = u32(this->mAcch.m_flags);
        f32 trackScale = f32(l_HIO.mTrackScaleY);
        this->mTrackCallback.mVel = trackVelocity;
        water = this->mWavePos.y;
        this->mTrackCallback.mExTransY = trackY;
        this->mTrackCallback.mBaseY = water;
        this->mTrackCallback.mExScaleY = trackScale;
        f32 level = (collisionFlags & 0x800) ? f32(this->mAcch.m_wtr.m_height) : l_noWaterLevel;
        f32 alpha = l_smokeScale;
        this->mTrackCallback.mMinY = level;
        this->mTrackCallback.mMinVel = alpha;
    }
    // Particle visibility bits are updated independently for the two waves.
    u32 emitter = u32(this->mWaveCallback1.mpBaseEmitter);
    if (emitter) {
        u32 flags = u32(OshipGuest<OshipEmitter>(emitter)->mFlags);
        OshipGuest<OshipEmitter>(emitter)->mFlags = fade > zero ? flags & ~1u : flags | 1;
    }
    emitter = u32(this->mWaveCallback2.mpBaseEmitter);
    if (emitter) {
        u32 flags = u32(OshipGuest<OshipEmitter>(emitter)->mFlags);
        OshipGuest<OshipEmitter>(emitter)->mFlags = fade > zero ? flags & ~1u : flags | 1;
    }
    this->mWaveCallback2.mVelFade1 = fade;
    this->mWaveCallback1.mVelFade1 = fade;
    this->mWaveCallback1.mMaxParticleVelocity = maximum;
    this->mWaveCallback2.mMaxParticleVelocity = maximum;
    f32 offset = f32(l_HIO.mWaveVelFadeOffs);
    f32 one = l_one;
    this->mWaveCallback2.mVelFade2 = offset + one;
    offset = f32(l_HIO.mWaveVelFadeOffs);
    this->mWaveCallback1.mVelFade2 = one - offset;
    f32 endZ = f32(l_HIO.mWaveCollapsePos1.z);
    f32 startZ = f32(l_HIO.mWaveCollapsePos0.z);
    f32 endY = f32(l_HIO.mWaveCollapsePos1.y);
    f32 startY = f32(l_HIO.mWaveCollapsePos0.y);
    f32 endX = f32(l_HIO.mWaveCollapsePos1.x);
    f32 startX = f32(l_HIO.mWaveCollapsePos0.x);
    this->mWaveCallback2.mCollapsePos[1].x = endX;
    this->mWaveCallback1.mCollapsePos[0].z = startZ;
    this->mWaveCallback1.mCollapsePos[1].x = -endX;
    this->mWaveCallback1.mCollapsePos[0].x = -startX;
    this->mWaveCallback2.mCollapsePos[1].z = endZ;
    this->mWaveCallback2.mCollapsePos[0].z = startZ;
    this->mWaveCallback1.mCollapsePos[0].y = startY;
    this->mWaveCallback2.mCollapsePos[0].x = startX;
    this->mWaveCallback1.mCollapsePos[1].y = endY;
    this->mWaveCallback2.mCollapsePos[1].y = endY;
    this->mWaveCallback1.mCollapsePos[1].z = endZ;
    this->mWaveCallback2.mCollapsePos[0].y = startY;
    f32 waveSpeed = f32(l_HIO.mWaveVelSpeed);
    f32 rate = l_smoothingRate;
    this->mWaveCallback2.mVelSpeed = waveSpeed;
    waveSpeed = f32(l_HIO.mWaveVelSpeed);
    f32 step = l_ten;
    this->mWaveCallback1.mVelSpeed = waveSpeed;
    cLib_addCalc2(&this->mSplashScaleTimer, splashTarget, rate, step);
    emitter = u32(this->mSplashCallback.mpBaseEmitter);
    if (emitter) {
        f32 timer = this->mSplashScaleTimer;
        u32 flags = u32(OshipGuest<OshipEmitter>(emitter)->mFlags);
        OshipGuest<OshipEmitter>(emitter)->mFlags = timer > rate ? flags & ~1u : flags | 1;
    }
    f32 timer = this->mSplashScaleTimer;
    this->mSplashCallback.mScaleTimer = timer;
    f32 splashMaximum = f32(l_HIO.mSplashMaxScaleTimer);
    this->mSplashCallback.mMaxScaleTimer = splashMaximum;
}

/* 023C714C */
BOOL daOship_c::checkTgHit() {
    this->mStts.Move();
    if (u8(l_HIO.m05)) {
        OshipBits<u8>(this->health) = 0;
        this->modeProc(PROC_INIT_e, MODE_DELETE);
        return TRUE;
    }
    if (OshipCalcTimer(&this->mHitTimer))
        return FALSE;
    gabi::Local<cXyz> hitPosition, scale;
    u32 hit = 0;
    for (u32 i = 0; i < 5; ++i) {
        u32 cylinder = gabi::ea(&mCyl[i]);
        hit = OshipGetTargetHitObject(OshipGuest<dCcD_Cyl>(cylinder));

        OshipBits<u32>(hitPosition->x) =
            u32(OshipBits<u32>(OshipGuest<dCcD_Cyl>(cylinder)->mGObjTg.mHitPos.x));
        OshipBits<u32>(hitPosition->y) =
            u32(OshipBits<u32>(OshipGuest<dCcD_Cyl>(cylinder)->mGObjTg.mHitPos.y));
        OshipBits<u32>(hitPosition->z) =
            u32(OshipBits<u32>(OshipGuest<dCcD_Cyl>(cylinder)->mGObjTg.mHitPos.z));
        OshipGetTargetHitObject(OshipGuest<dCcD_Cyl>(cylinder));
        if (hit)
            break;
    }
    if (!hit || u32(OshipGuest<dCcD_GObjInf>(hit)->mAtType) != 32)
        return FALSE;
    u8 health = u8(OshipBits<u8>(this->health));
    s32 count = this->mSmokePtclCount;
    OshipBits<u8>(this->health) = u32(health) - 1;
    if (count < 3 && !u32(OshipArrayElement(mSmokeFollowCallback, count)->mpEmitter.v)) {
        s32 yaw = OshipTargetAngleY(&this->current.pos, hitPosition.get());
        count = this->mSmokePtclCount;
        s16 shapeYaw = s16(this->shape_angle.y);
        *OshipArrayElement(mSmokeRotY, count) = u32(yaw) - s32(shapeYaw);
        count = this->mSmokePtclCount;
        u32 rotation = gabi::ea(OshipArrayElement(mSmokeRot, count));
        u16 value = u16(OshipBits<u16>(this->shape_angle.x));
        OshipBits<u16>(OshipGuest<csXyz>(rotation)->x) = value;
        value = u16(OshipBits<u16>(this->shape_angle.y));
        OshipBits<u16>(OshipGuest<csXyz>(rotation)->y) = value;
        value = u16(OshipBits<u16>(this->shape_angle.z));
        OshipBits<u16>(OshipGuest<csXyz>(rotation)->z) = value;
        count = this->mSmokePtclCount;
        rotation = gabi::ea(OshipArrayElement(mSmokeRot, count));
        s16 relativeYaw = s16(*OshipArrayElement(mSmokeRotY, count));
        s16 rotationY = s16(OshipGuest<csXyz>(rotation)->y);
        OshipGuest<csXyz>(rotation)->y = s32(rotationY) + s32(relativeYaw);
        count = this->mSmokePtclCount;
        rotation = gabi::ea(OshipArrayElement(mSmokeRot, count));
        s8 room = s8(this->current.roomNo);
        u32 callback = gabi::ea(OshipArrayElement(mSmokeFollowCallback, count));
        u32 play = dComIfGp_ea();
        u32 manager = u32(OshipGuest<dComIfG_inf_c>(play)->play.mParticle.v);
        OshipParticleSet(OshipGuest<dPa_control_c>(manager), 0, 0x3E1, &this->mSmokePos,
                         OshipGuest<csXyz>(rotation), &this->scale, 255,
                         OshipGuest<dPa_followEcallBack>(callback), room, 0, 0, 0);
        this->mSmokePtclCount = u32(s32(this->mSmokePtclCount)) + 1;
    }
    u32 play = dComIfGp_ea();
    u32 player = u32(OshipGuest<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    play = dComIfGp_ea();
    u32 manager = u32(OshipGuest<dComIfG_inf_c>(play)->play.mParticle.v);
    OshipParticleSet(OshipGuest<dPa_control_c>(manager), 0, 0x10, hitPosition.get(), 0, 0, 255, 0,
                     -1, 0, 0, 0);
    f32 size = l_two;
    scale->x = size;
    scale->y = size;
    scale->z = size;
    play = dComIfGp_ea();
    manager = u32(OshipGuest<dComIfG_inf_c>(play)->play.mParticle.v);
    OshipParticleSet(OshipGuest<dPa_control_c>(manager), 0, 0xF, hitPosition.get(),
                     &OshipGuest<fopAc_ac_c>(player)->shape_angle, scale.get(), 255, 0, -1, 0, 0,
                     0);
    s8 remaining = s8(this->health);
    this->mHitTimer = 5;
    if (remaining <= 0) {
        s8 room = s8(this->current.roomNo);
        s32 reverb = OshipGetReverb(room);
        mDoAud_seStart(0x2828, &this->eyePos, 0, reverb);
        this->modeProc(PROC_INIT_e, MODE_DELETE);
    } else {
        this->mAimCounter = 6;
        this->modeProc(PROC_INIT_e, MODE_DAMAGE);
    }
    return TRUE;
}

/* 023C641C */
void daOship_c::setAttention() {
    u32 z = u32(OshipBits<u32>(this->current.pos.z)), y = u32(OshipBits<u32>(this->current.pos.y));
    OshipBits<u32>(this->eyePos.z) = z;
    u32 x = u32(OshipBits<u32>(this->current.pos.x));
    OshipBits<u32>(this->eyePos.y) = y;
    OshipBits<u32>(this->attention_info.position.x) = x;
    OshipBits<u32>(this->attention_info.position.z) = z;
    OshipBits<u32>(this->eyePos.x) = x;
    OshipBits<u32>(this->attention_info.position.y) = y;
    u32 play = dComIfGp_ea();
    if (!u8(OshipGuest<dComIfG_inf_c>(play)->play.mEvtCtrl.mMode)) {
        f32 attentionY = f32(this->attention_info.position.y);
        attentionY += f32(l_HIO.mAttentionOffsY);
        f32 eyeY = f32(this->eyePos.y);
        this->attention_info.position.y = attentionY;
        eyeY += f32(l_HIO.mEyeOffsY);
        this->eyePos.y = eyeY;
    }
}

/* 023C64A0 */
void daOship_c::setCollision() {
    u16 angle = u16(OshipBits<u16>(this->current.angle.y));
    f32 height = l_wallRadius;

    f32 sine = cM_ssin(angle), cosine = cM_scos(angle);
    gabi::Local<cXyz> center;
    for (u32 i = 0; i < 5; ++i) {
        f32 offset = f32(l_cylinderOffsets.mValues[i]);
        f32 x = this->current.pos.x, y = this->current.pos.y;
        f32 centerX = gabi::fmadds(offset, sine, x);
        f32 z = this->current.pos.z;
        f32 centerY = y - height, centerZ = gabi::fmadds(offset, cosine, z);
        center->x = centerX;
        center->y = centerY;
        center->z = centerZ;
        u32 cylinder = gabi::ea(&mCyl[i]);
        OshipCylinderSetC(&OshipGuest<dCcD_Cyl>(cylinder)->mCyl, center.get());
        f32 radius = f32(l_cylinderRadii.mValues[i]);
        OshipCylinderSetR(&OshipGuest<dCcD_Cyl>(cylinder)->mCyl, radius);
        f32 cylinderHeight = f32(l_cylinderHeights.mValues[i]);
        OshipCylinderSetH(&OshipGuest<dCcD_Cyl>(cylinder)->mCyl, cylinderHeight);
        u32 play = dComIfGp_ea();
        OshipCollisionSet(&OshipGuest<dComIfG_inf_c>(play)->play.mCcS,
                          OshipGuest<dCcD_Cyl>(cylinder));
    }
}

/* 023C74C0 */
void daOship_c::attackCannon(s32 slot) {
    s16 shapeX = s16(this->shape_angle.x), aimPitch = this->mAimRotX, aimYaw = this->mAimRotY;
    s16 shapeY = s16(this->shape_angle.y), shapeZ = s16(this->shape_angle.z);
    gabi::Local<csXyz> angle;
    angle->x = s32(shapeX) - s32(aimPitch);
    angle->z = shapeZ;
    angle->y = s32(shapeY) + s32(aimYaw);
    u32 params = OshipBombParameters(4, 1, 1);
    s8 layer = s8(this->tevStr.mRoomNo);
    u32 bomb = OshipFastCreateBomb(fpcNm_BOMB_e, params, &this->mBombSpawnPos, layer, angle.get(),
                                   0, -1, 0, 0);
    u32 id = bomb ? u32(OshipGuest<fopAc_ac_c>(bomb)->mBsPcId) : 0xFFFFFFFF;
    *OshipArrayElement(mBombPcId, slot) = id;
    *OshipArrayElement(mBombAlloc, slot) = 1;
    s16 gravityDelay = s16(l_HIO.mBombNoGravityTime);
    OshipBombSetNoGravityTime(OshipGuest<fopAc_ac_c>(bomb), gravityDelay);
    u16 pitch = angle->x;

    f32 speed = f32(l_HIO.mBombSpeed), cosine = cM_scos(pitch);
    OshipGuest<fopAc_ac_c>(bomb)->speedF = gabi::fmuls_ppc(cosine, speed);
    pitch = angle->x;
    f32 sine = cM_ssin(pitch);
    speed = f32(l_HIO.mBombSpeed);
    f32 vertical = -gabi::fmuls_ppc(sine, speed);
    f32 scale = l_two;
    OshipGuest<fopAc_ac_c>(bomb)->speed.y = vertical;
    f32 acceleration = f32(l_HIO.mBombAcceleration);
    OshipGuest<fopAc_ac_c>(bomb)->scale.x = scale;
    OshipGuest<fopAc_ac_c>(bomb)->gravity = acceleration;
    OshipGuest<fopAc_ac_c>(bomb)->scale.y = scale;
    OshipGuest<fopAc_ac_c>(bomb)->scale.z = scale;
    OshipStartBombSound(0x2852, &this->mBombSpawnPos);
}

/* 023C5A54 */
void daOship_c::setMtx() {
    // HD: waveRot receives position and wave before the amplitude.
    s16 sway = this->mAttackSwayAmount;
    s16 debug = s16(l_debugRegisters.mShort[3]);
    f32 amplitude = gabi::fmuls_ppc(f32(sway), f32(s32(debug) + 1));
    OshipWaveRotHD(&this->current.pos, &this->mWave, amplitude);
    debug = s16(l_debugRegisters.mShort[5]);
    sway = this->mAttackSwayAmount;
    s32 strength = s32(sway) * (s32(debug) + 10);
    u16 phase = this->mAttackSwayTimer;
    f32 sine = cM_ssin(phase);
    s16 oscillation = gabi::ftoi(gabi::fmuls_ppc(f32(strength), sine));
    u32 play = dComIfGp_ea();
    u32 player = u32(OshipGuest<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    s32 playerAngle = OshipActorAngleY(this, OshipGuest<fopAc_ac_c>(player));
    s16 shapeYaw = s16(this->shape_angle.y);
    u16 angle = u32(playerAngle) + s32(shapeYaw);

    f32 cosine = cM_scos(angle);
    sine = cM_ssin(angle);
    s16 pitch = gabi::ftoi(gabi::fmuls_ppc(cosine, f32(oscillation)));
    s16 roll = gabi::ftoi(gabi::fmuls_ppc(sine, f32(oscillation)));
    s32 mode = this->mCurrentProc;
    f32 scaleY = f32(this->scale.y), scaleX = f32(this->scale.x);
    if (mode != 3) {
        s16 waveX = this->mWave.mRotX, waveZ = this->mWave.mRotZ;
        this->shape_angle.x = s32(waveX) + s32(pitch);
        this->shape_angle.z = s32(waveZ) + s32(roll);
    }
    u32 model = this->mpModel.v;
    f32 scaleZ = f32(this->scale.z);
    OshipGuest<J3DModel>(model)->mBaseScale.x = scaleX;
    OshipGuest<J3DModel>(model)->mBaseScale.y = scaleY;
    OshipGuest<J3DModel>(model)->mBaseScale.z = scaleZ;
    f32 x = this->current.pos.x, z = this->current.pos.z, y = this->current.pos.y;
    auto* matrix = &l_matrixStack;
    OshipMatrixTrans(matrix, x, y, z);
    pitch = s16(this->shape_angle.x);
    roll = s16(this->shape_angle.z);
    OshipMatrixXYZrotM(matrix, pitch, 0, roll);
    shapeYaw = s16(this->shape_angle.y);
    OshipMatrixYrotM(matrix, shapeYaw);
    OshipMatrixCopy(matrix, &this->mFlagMtx);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = l_matrixStack.m[i / 4][i % 4];
    model = this->mpModel.v;
    for (u32 i = 0; i < 12; ++i)
        OshipGuest<J3DModel>(model)->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    f32 attentionHeight = f32(l_HIO.mAttentionOffsY);
    gabi::Local<cXyz> local;
    OshipBits<u32>(local->x) = 0;
    OshipBits<u32>(local->y) = 0;
    OshipBits<u32>(local->z) = 0;
    local->y = attentionHeight;
    OshipMatrixMultVec(matrix, local.get(), &this->attention_info.position);
    local->y = f32(l_HIO.mEyeOffsY);
    OshipMatrixMultVec(matrix, local.get(), &this->eyePos);
    f32 zero = l_zero, waveOffset = f32(l_HIO.mWaveOffsZ);
    local->y = zero;
    local->z = waveOffset;
    OshipMatrixMultVec(matrix, local.get(), &this->mWavePos);
    local->z = f32(l_HIO.mTrackOffsZ);
    OshipMatrixMultVec(matrix, local.get(), &this->mTrackPos);
}

/* 023C6DB0 */
void daOship_c::modeWaitInit() { this->changeModeByRange(); }

/* 023C7080 */
void daOship_c::modeWait() { this->changeModeByRange(); }

/* 023C7034 */
void daOship_c::modeRangeAInit() { this->mAttackTimer = s16(l_HIO.mAttackDelayA); }

/* 023C7D74 */
void daOship_c::modeRangeA() { OshipRangeUpdate(this, true); }

/* 023C7044 */
void daOship_c::modeRangeBInit() {
    this->mAttackTimer = s16(this->mSubMode == 0 ? l_HIO.mAttackDelayA : l_HIO.mAttackDelayB);
}

/* 023C7E58 */
void daOship_c::modeRangeB() { OshipRangeUpdate(this, true); }

/* 023C7070 */
void daOship_c::modeRangeCInit() { this->mAttackTimer = s16(l_HIO.mAttackDelayB); }

/* 023C7F3C */
void daOship_c::modeRangeC() { OshipRangeUpdate(this, false); }

/* 023C83DC */
void daOship_c::modeRangeDInit() {}

/* 023C8018 */
void daOship_c::modeRangeD() {
    if (this->checkTgHit())
        return;
    if (this->mSubMode == 2 && this->mPathId != 0xFF) {
        this->mVelocityFwdTarget = f32(l_HIO.mPathSpeed);
        this->pathMove();
    }
    this->current.pos.y = OshipGetWaterY(&this->current.pos, &this->mAcch);
    this->changeModeByRange();
}

/* 023C6F94 */
void daOship_c::modeDamageInit() {
    this->mAttackTimer = 60;
    this->mAttackSwayAmount = 300;
}

/* 023C77C0 */
void daOship_c::modeDamage() {
    s16 debug = s16(l_debugRegisters.mShort[2]);
    this->mAttackSwayTimer = u16(this->mAttackSwayTimer) + u32(s32(debug) + 0x1830);
    OshipAddCalcAngleS2(&this->mAttackSwayAmount, 0, 10, 10);
    f32 amount = f32(s16(this->mAttackSwayAmount));
    u16 phase = this->mAttackSwayTimer;
    f32 sine = cM_ssin(phase);
    f32 sway = gabi::fmuls_ppc(amount, sine);
    f32 scale = l_swayScale;
    f32 water = OshipGetWaterY(&this->current.pos, &this->mAcch);
    this->current.pos.y = gabi::fmadds(sway, scale, water);
    if (!this->checkTgHit()) {
        if (!OshipCalcTimer(&this->mAttackTimer) || this->mAttackTimer < 0)
            this->changeModeByRange();
    }
}

/* 023C6DB4 */
void daOship_c::modeAttackInit() {
    this->mAttackTimer = -1;
    u32 play = dComIfGp_ea();
    u32 player = u32(OshipGuest<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    OshipBits<u32>(this->mTargetPos.x) =
        u32(OshipBits<u32>(OshipGuest<fopAc_ac_c>(player)->current.pos.x));
    OshipBits<u32>(this->mTargetPos.y) =
        u32(OshipBits<u32>(OshipGuest<fopAc_ac_c>(player)->current.pos.y));
    OshipBits<u32>(this->mTargetPos.z) =
        u32(OshipBits<u32>(OshipGuest<fopAc_ac_c>(player)->current.pos.z));
    gabi::Local<cXyz> delta, horizontal;
    OshipVectorSubtract(&this->current.pos, delta.get(), &this->mTargetPos);
    f32 x = delta->x, zero = l_zero;
    horizontal->x = x;
    f32 z = delta->z;
    horizontal->y = zero;
    horizontal->z = z;
    f32 square = PSVECSquareMag(horizontal.get());
    // HD: retain the double result through the distance comparison.
    f64 distance = OshipSqrt(f64(square));
    f32 start = f32(l_HIO.mBadAimAdjustDistanceStart), error = zero;
    if (distance > start) {
        f32 excess = f32(distance - f64(start));
        f32 scale = l_flagScale;
        error = gabi::fmuls_ppc(excess, scale);
    }
    f32 base = l_cullMaxX, chance = l_hundred;
    error += base;
    f32 random = cM_rndF(chance);
    if (random < l_ten)
        error = zero;
    play = dComIfGp_ea();
    u32 playerStatus = u32(OshipGuest<dComIfG_inf_c>(play)->play.mPlayerStatus[0][0]);
    if (playerStatus & 0x01100000) {
        this->mAimCounter = 0;
        u8 debug = u8(l_HIO.m08);
        f32 extra = l_badAimExtra;
        error += extra;
        if (debug)
            error = zero;
    } else {
        s32 count = this->mAimCounter;
        if (count < 6) {
            f32 scale = l_damageScale;
            s32 remaining = s32(6u - u32(count));
            error = gabi::fmadds(f32(remaining), scale, error);
        }
        if (u8(l_HIO.m08))
            error = zero;
    }
    play = dComIfGp_ea();
    player = u32(OshipGuest<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    u16 angle = OshipActorAngleY(this, OshipGuest<fopAc_ac_c>(player));

    x = this->mTargetPos.x;
    f32 sine = cM_ssin(angle);
    f32 targetX = gabi::fnmsubs(error, sine, x);
    z = this->mTargetPos.z;
    this->mTargetPos.x = targetX;
    f32 cosine = cM_scos(angle);
    this->mTargetPos.z = gabi::fnmsubs(error, cosine, z);
}

/* 023C760C */
void daOship_c::modeAttack() {
    if (this->mPathId != 0xFF) {
        this->mVelocityFwdTarget = l_zero;
        this->pathMove();
    }
    if (this->checkTgHit())
        return;
    f32 water = OshipGetWaterY(&this->current.pos, &this->mAcch);
    s32 timer = this->mAttackTimer;
    this->current.pos.y = water;
    if (timer < 0) {
        s16 angle = this->mAimRotY, target = this->mAimRotYTarget;
        OshipDistanceAngleS(angle, target);
        angle = this->mAimRotX;
        target = this->mAimRotXTarget;
        OshipDistanceAngleS(angle, target);
        this->mAttackTimer = -1;
        if (u8(l_HIO.m06) || this->lineCheck(&this->mSmokePos, &this->mTargetPos)) {
            this->changeModeByRange();
            return;
        }
        f32 cutoff = l_two;
        for (u32 i = 0; i < 5; ++i) {
            if (!this->mBombAlloc[i]) {
                f32 speed = f32(this->speedF);
                if (!(speed > cutoff)) {
                    this->attackCannon(i);
                    this->mAttackSwayAmount = 100;
                    this->mAttackTimer = 15;
                    return;
                }
            }
        }
        return;
    }
    if (!OshipCalcTimer(&this->mAttackTimer)) {
        this->changeModeByRange();
        return;
    }
    s16 debug = s16(l_debugRegisters.mShort[2]);
    s16 sway = this->mAttackSwayTimer;
    this->mAttackSwayTimer = s32(sway) + s32(debug) + 0x1830;
    OshipAddCalcAngleS2(&this->mAttackSwayAmount, 0, 10, 10);
}

/* 023C6FA8 */
void daOship_c::modeDeleteInit() {
    s8 room = s8(this->current.roomNo);
    s32 reverb = OshipGetReverb(room);
    mDoAud_seStart(0x6A19, &this->eyePos, 0, reverb);
    for (u32 i = 0; i < 3; ++i)
        this->mSmokeFollowCallback[i].end();
    OshipCreateDisappear(this, &this->current.pos, 5, 0, 0xFF);
}

/* 023C78D0 */
void daOship_c::modeDelete() {
    u8 modelType = this->mModelType;
    f32 sinkOffset = l_sinkOffset;
    f32 maxStep = l_sinkMaxStep;
    f32 nearWater = l_damageScale;
    f32 smoothing = l_sinkSmoothing;
    bool special = modelType != 255 || s16(l_debugRegisters.mShort[0]) != 0;
    if (special && u16(this->eventInfo.mCommand) != dEvtCmd_INDEMO_e) {
        OshipOrderOtherEvent(this, "GOLD_SHIP_DELETE", 1, 0xFFFF);
        return;
    }
    s32 angleResult = OshipAddCalcAngleS(&this->shape_angle, -15000, 20, 0x1000, 0x100);
    f32 water = OshipGetWaterY(&this->current.pos, &this->mAcch);
    f32 epsilon = l_ten;
    f32 result =
        OshipAddCalc(&this->current.pos.y, f32(sinkOffset + water), smoothing, maxStep, epsilon);
    s16 angleTest = s16(gabi::ftoi(std::fabs(f32(angleResult))));
    bool settled = angleTest <= 256 && std::fabs(f32(result - water)) < nearWater;
    if (settled && this->mSwitchA != 255) {
        u32 save = dComIfGs_ea();
        s8 room = s8(this->current.roomNo);
        OshipOnSwitch(OshipGuest<dSv_info_c>(u32(save + 0x20)), u32(this->mSwitchA), room);
        gabi::Local<be<u16>> actorType;
        *actorType = fpcNm_SALVAGE_e;
        u32 target = OshipActorJudge(0x025E121C, actorType.get());
        OshipSalvageOnForOship(OshipGuest<fopAc_ac_c>(target), this);
        if (special)
            OshipStartSalvageSound(0x806);
    }
    if (special) {
        u32 play = dComIfGp_ea();
        s32 end = OshipEventEndCheckOld(&OshipGuest<dComIfG_inf_c>(play)->play.mEvtManager,
                                        "GOLD_SHIP_DELETE");
        if (end) {
            u32 save = dComIfGs_ea();
            OshipOnEventBit(&(*OshipGuest<dSv_info_c>(u32(save + 0x20))).mSavedata.mEvent, 0x3E80);
            play = dComIfGp_ea();
            u16 flags = u16(OshipGuest<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag);
            OshipGuest<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag = flags | 8;
            OshipActorDelete(this);
        }
        return;
    }
    if (!settled)
        return;
    u32 flag = this->mFlagPcId;
    gabi::Local<be<u32>> id;
    *id = flag;
    u32 actor = 0;
    if (flag != 0xFFFFFFFF)
        actor = OshipActorJudge(0x025E1234, id.get());
    if (actor)
        OshipActorDelete(OshipGuest<fopAc_ac_c>(actor));
    OshipActorDelete(this);
}

/* 023C58DC */
void daOship_c::modeProc(s32 kind, s32 mode) {
    const gabi::ptmf<ModeProcFunc>* proc;
    if (kind == PROC_INIT_e) {
        u32 status = actor_status;
        u32 attention = attention_info.flags;
        mCurrentProc = mode;
        actor_status = status | fopAcStts_SHOWMAP_e;
        attention_info.flags = attention | fopAc_Attn_LOCKON_BATTLE_e;
        // HD: mode indices wrap in guest address space, including the verified out-of-range inputs.
        proc = reinterpret_cast<const gabi::ptmf<ModeProcFunc>*>(
            PPC_MEM_BASE + (gabi::ea(&mode_tbl) + u32(mode) * 20u));
    } else if (kind == PROC_EXEC_e) {
        mode = mCurrentProc;
        proc = reinterpret_cast<const gabi::ptmf<ModeProcFunc>*>(
            PPC_MEM_BASE + (gabi::ea(&mode_tbl) + 8u + u32(mode) * 20u));
    } else {
        return;
    }
    (this->*(*proc))(kind, mode);
}

/* 023C68B8 */
s32 daOship_c::_execute() {
    // HD: attention distance is written to the battle slot rather than the GC carry slot.
    this->attention_info.flags = 4;
    f32 particleScale = l_smokeScale;
    this->attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 0x22;
    for (u32 i = 0; i < 3; ++i) {
        u32 callback = gabi::ea(&mSmokeFollowCallback[i]);
        if (u32(OshipGuest<dPa_followEcallBack>(callback)->mpEmitter.v)) {
            u32 rotation = gabi::ea(&mSmokeRot[i]);
            s16 x = s16(this->shape_angle.x);
            OshipGuest<csXyz>(rotation)->x = x;
            s16 y = s16(this->shape_angle.y);
            OshipGuest<csXyz>(rotation)->y = y;
            s16 z = s16(this->shape_angle.z);
            OshipGuest<csXyz>(rotation)->z = z;
            s16 yaw = s16(mSmokeRotY[i]);
            OshipGuest<csXyz>(rotation)->y = s32(y) + s32(yaw);
            u32 particle = u32(OshipGuest<dPa_followEcallBack>(callback)->mpEmitter.v);
            auto* emitter = OshipGuest<OshipEmitter>(particle);
            emitter->mGlobalDynamicsScale.x = particleScale;
            emitter->mGlobalDynamicsScale.y = particleScale;
            emitter->mGlobalDynamicsScale.z = particleScale;
            emitter->mGlobalParticleScale.x = particleScale;
            emitter->mGlobalParticleScale.y = particleScale;
            emitter->mGlobalParticleScale.z = particleScale;
        }
    }
    f32 zeroBias = l_ten;
    f32 heightBias = l_bombHeightBias;
    f32 acceleration = l_smoothingRate;

    gabi::Local<be<u32>> bombId;
    for (u32 i = 0; i < 5; ++i) {
        if (!this->mBombAlloc[i])
            continue;
        u32 id = this->mBombPcId[i];
        *bombId = id;
        u32 bomb = 0;
        if (id != 0xFFFFFFFF)
            bomb = OshipActorJudge(0x025E1234, bombId.get());
        if (!bomb) {
            this->mBombAlloc[i] = 0;
            continue;
        }
        if (u8(l_HIO.m07) == 0)
            continue;
        f32 limit = f32(f32(l_debugRegisters.mFloat[1]) + heightBias);
        f32 height = f32(OshipGuest<fopAc_ac_c>(bomb)->speed.y);
        if (height > limit)
            continue;
        f32 target = f32(f32(l_debugRegisters.mFloat[3]) + zeroBias);
        f32 max = f32(f32(l_debugRegisters.mFloat[4]) + zeroBias);
        cLib_addCalc2(&OshipGuest<fopAc_ac_c>(bomb)->speedF, target, acceleration, max);
        f32 newHeight = f32(f32(l_debugRegisters.mFloat[2]) + heightBias);
        OshipGuest<fopAc_ac_c>(bomb)->speed.y = newHeight;
    }
    s32 yaw = OshipTargetAngleY(&this->current.pos, &this->mTargetPos);
    this->mAimRotYTarget = u32(yaw) - u32(s32(s16(this->shape_angle.y)));
    gabi::Local<cXyz> difference, flat;
    OshipVectorSubtract(&this->current.pos, difference.get(), &this->mTargetPos);
    f32 x = difference->x;
    f32 flatY = l_zero;
    f32 z = difference->z;
    flat->x = x;
    flat->z = z;
    flat->y = flatY;
    // HD: sqrt consumes the live FPR result of PSVECSquareMag without an explicit argument.
    PSVECSquareMag(flat.get());
    f32 distance = OshipSqrtLiveFpr();
    f32 threshold = f32(l_HIO.mBadAimAdjustDistanceStart);
    s32 correction = 0;
    if (distance > threshold) {
        correction =
            OshipDistanceAngleS(s32(s16(gabi::ftoi(distance))), s32(s16(gabi::ftoi(threshold))));
        s16 maximum = s16(l_HIO.mBadAimMax);
        if (correction > maximum)
            correction = maximum;
    }
    s32 pitch = OshipTargetAngleX(&this->current.pos, &this->mTargetPos);
    s16 targetPitch = s16(u32(pitch) + u32(correction));
    this->mAimRotXTarget = targetPitch;
    OshipAddCalcAngleS2(&this->mAimRotX, s32(targetPitch), 6, 0x300);
    OshipAddCalcAngleS2(&this->mAimRotY, s32(s16(this->mAimRotYTarget)), 6, 0x300);
    this->modeProc(PROC_EXEC_e, MODE_NULL);
    mpModel->calc();
    gabi::Local<cXyz> offset;
    offset->x = f32(l_HIO.mBombOffset.x);
    u32 model = this->mpModel.v;
    offset->y = f32(l_HIO.mBombOffset.y);
    offset->z = f32(l_HIO.mBombOffset.z);
    u32 modelData = u32(OshipGuest<J3DModel>(model)->mpMtxBuffer.v);
    u16 flags = u16(OshipGuest<J3DMtxBuffer_hd>(modelData)->mFlags);
    u32 matrices = u32(OshipGuest<J3DMtxBuffer_hd>(modelData)->mpAnmMtx);
    OshipGuest<J3DMtxBuffer_hd>(modelData)->mFlags = flags | 0x10;
    OshipMatrixMultVec(OshipGuest<Mtx34>(matrices + 2u * sizeof(Mtx34)), offset.get(),
                       &this->mBombSpawnPos);
    model = this->mpModel.v;
    gabi::Local<cXyz> origin;
    origin->x = 0;
    origin->y = 0;
    origin->z = 0;
    modelData = u32(OshipGuest<J3DModel>(model)->mpMtxBuffer.v);
    flags = u16(OshipGuest<J3DMtxBuffer_hd>(modelData)->mFlags);
    matrices = u32(OshipGuest<J3DMtxBuffer_hd>(modelData)->mpAnmMtx);
    OshipGuest<J3DMtxBuffer_hd>(modelData)->mFlags = flags | 0x10;
    OshipMatrixMultVec(OshipGuest<Mtx34>(matrices + sizeof(Mtx34)), origin.get(), &this->mSmokePos);
    this->setAttention();
    this->setCollision();
    this->setMtx();
    model = this->mpModel.v;
    u32 matrix = model ? model + 0xC8 : 0;
    f32 a = l_cullMinX;
    f32 d = l_cullMaxX;
    f32 c = l_cullMinZ;
    f32 e = l_cullMaxY;
    f32 f = l_cullMaxZ;
    f32 b = l_cullMinY;
    s32 culled = OshipCheckCullingBox(OshipGuest<Mtx34>(matrix), a, b, c, d, e, f);
    f32 speed = f32(this->speedF);
    f32 minimum = l_two;
    bool waves = false;
    if (speed > minimum && !culled) {
        u32 play = dComIfGp_ea();
        u32 player = u32(OshipGuest<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
        f32 playerDistance = OshipActorDistanceXZ(this, OshipGuest<fopAc_ac_c>(player));
        f32 maximum = l_maxWaveDistance;
        waves = !(playerDistance > maximum);
    }
    if (waves)
        this->setWave();
    else {
        OshipWaveRemove(&this->mWaveCallback2);
        OshipWaveRemove(&this->mWaveCallback1);
        OshipSplashRemove(&this->mSplashCallback);
        OshipBits<u16>(this->mTrackCallback.mState) = 1;
    }
    if (this->mModelType == 255 && s16(l_debugRegisters.mShort[0]) == 0) {
        u32 id = this->mFlagPcId;
        gabi::Local<be<u32>> flagId;
        *flagId = id;
        u32 flag = 0;
        if (id != 0xFFFFFFFF)
            flag = OshipActorJudge(0x025E1234, flagId.get());
        if (flag) {
            OshipGuest<OshipFlagParent>(flag)->mpParentMtx = gabi::ea(&mFlagMtx);
            OshipGuest<OshipFlagParent>(flag)->mpParentPos = gabi::ea(&mFlagOffset);
        }
    }
    return 0;
}

/* 023C6D3C */
s32 daOship_c::_draw() {
    u32 light = gabi::ea(dKy_getEnvlight());
    OshipSettingTevStruct(OshipGuest<dScnKy_env_light_c>(light), 0, &this->current.pos,
                          &this->tevStr);
    light = gabi::ea(dKy_getEnvlight());
    OshipSetLightTevColorType(OshipGuest<dScnKy_env_light_c>(light),
                              OshipGuest<J3DModel>(this->mpModel.v), &this->tevStr);
    u32 model = this->mpModel.v;
    f32 scale = f32(l_HIO.mSpecScale);
    OshipTexSpecmapST(&this->eyePos, &this->tevStr, OshipGuest<J3DModel>(model), scale);
    OshipModelEntryDL(OshipGuest<J3DModel>(this->mpModel.v));
    return 1;
}

/* 023C5F84 */
void daOship_c::createInit() {
    u32 play = dComIfGp_ea();
    u32 distance = OshipCharacterNameIndex(
        OshipGuest<OshipCharacterNameTable>(u32(play + PLAY_NAMETBL)), "Oship", 0);
    OshipBits<u32>(this->itemTableIdx) = distance;
    OshipBits<u8>(this->max_health) = 3;
    u32 x = u32(OshipBits<u32>(this->current.pos.x)), y = u32(OshipBits<u32>(this->current.pos.y));
    OshipBits<u32>(this->mOrigPos.x) = x;
    OshipBits<u8>(this->health) = 3;
    u32 z = u32(OshipBits<u32>(this->current.pos.z));
    OshipBits<u32>(this->mOrigPos.y) = y;
    this->mPlFireTimer = -1;
    OshipBits<u32>(this->mOrigPos.z) = z;
    this->changeModeByRange();
    this->attention_info.flags = 4;
    this->attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 0x22;
    this->mStts.Init(255, 0, this);
    for (u32 i = 0; i < 5; ++i) {
        u32 cylinder = gabi::ea(&mCyl[i]);
        OshipCylinderSet(OshipGuest<dCcD_Cyl>(cylinder), &m_cyl_src);
        OshipGuest<dCcD_Cyl>(cylinder)->mStts.v = gabi::ea(&mStts);
    }
    f32 range = l_wavePhaseRange;
    this->mWave.mAnimX = gabi::ftoi(cM_rndF(range));
    this->mWave.mAnimZ = gabi::ftoi(cM_rndF(range));
    this->setMtx();
    mpModel->calc();
    u32 model = this->mpModel.v;
    this->cullMtx = model ? model + 0xC8 : 0;
    f32 maximumZ = l_cullMaxZ;
    f32 maximumX = l_cullMaxX;
    f32 maximumY = l_cullMaxY;
    f32 minimumX = l_cullMinX;
    f32 minimumY = l_cullMinY;
    f32 minimumZ = l_cullMinZ;
    OshipSetCullSizeBox(this, minimumX, minimumY, minimumZ, maximumX, maximumY, maximumZ);
    u8 type = this->mModelType;
    f32 farDistance = l_ten;
    this->cullSizeFar = farDistance;
    if (type == 0xFF && !s16(l_debugRegisters.mShort[0])) {
        s8 layer = s8(this->tevStr.mRoomNo);
        this->mFlagPcId = OshipCreateFlag(fpcNm_MAJUU_FLAG_e, 4, &this->current.pos, layer,
                                          &this->current.angle, 0, -1, 0);
        if (l_flagOffsetGuard) {
            f32 zOffset = f32(l_flagOffset.z), yOffset = f32(l_flagOffset.y),
                xOffset = f32(l_flagOffset.x);
            this->mFlagOffset.y = yOffset;
            this->mFlagOffset.z = zOffset;
            this->mFlagOffset.x = xOffset;
        } else {
            l_flagOffset.y = maximumZ;
            f32 zero = l_zero;
            l_flagOffsetGuard = 1;
            l_flagOffset.x = zero;
            l_flagOffset.z = zero;
            this->mFlagOffset.x = zero;
            this->mFlagOffset.y = maximumZ;
            this->mFlagOffset.z = zero;
        }
    }
    u8 path = this->mPathId;
    if (path != 0xFF) {
        s8 room = s8(this->current.roomNo);
        this->mpPath.v = OshipGetRoomPath(path, room);
    }
    this->createWave();
    f32 radius = l_wallRadius;
    this->mAcchCir.SetWall(radius, radius);
    OshipAcchSet(&this->mAcch, &this->current.pos, &this->old.pos, this, 1, &this->mAcchCir,
                 &this->speed, 0, 0);
    u32 collisionFlags = u32(this->mAcch.m_flags);
    this->mAcch.m_flags = collisionFlags | 12;
}

/* 023C5320 */
BOOL daOship_c::_createHeap() {
    s32 resourceIndex = dRes_INDEX_OSHIP_BDL_VBTSP_e;
    if (this->mModelType != 0xFF || s16(l_debugRegisters.mShort[0]) != 0)
        resourceIndex = dRes_INDEX_OSHIP_BDL_VBTST_e;
    gabi::Local<SafeString> name;
    name->mStringTop = gabi::guest_addr("Oship");
    name->__vtbl = 0x10033F8C;
    void* data = OshipGetObjectResource(OshipGuest<dRes_control_c>(u32(l_resControl.mpControl)),
                                        name.get(), resourceIndex);
    if (!data)
        OshipAssert("d_a_oship.cpp", 0x63F, "modelData != (0)");
    void* model = OshipCreateModel(data, 0x80000, 0x37441422);
    this->mpModel.v = gabi::ea(model);
    if (!model)
        return FALSE;
    static_cast<J3DModel*>(model)->mUserArea = gabi::ea(this);
    u16 index = 0;
    u32 joints = OshipJointTreeHeader(data);
    while (index < u16(OshipGuest<OshipJointHeader>(joints)->mJointNum)) {
        if (index >= VBTSP_JNT_HEAD_e && index <= VBTSP_JNT_CANON_e) {
            u32 count = u32(static_cast<J3DModelData*>(data)->mJointNum);
            u32 node = u32(static_cast<J3DModelData*>(data)->mJointNodes);
            if (index < count)
                node += u32(index) * 28;
            OshipGuest<J3DJoint>(node)->mCallBack = gabi::guest_fn((const void*)nodeControl_CB);
        }
        index = u16(index + 1);
        joints = OshipJointTreeHeader(data);
    }
    return TRUE;
}

/* 023C5890 */
void daOship_c::getArg() {
    u32 params = u32(this->mParameters);
    s16 angle = s16(this->home.angle.x);
    this->mSubMode = params;
    this->mTriforce = (params >> 8) & 15;
    this->mPathId = (params >> 16) & 255;
    this->mSwitchA = params >> 24;
    this->mRandomSalvagePoint = (params >> 12) & 15;
    this->mSwitchB = angle;
    this->mModelType = (u16(angle) >> 8) & 255;
    this->shape_angle.x = 0;
    this->home.angle.x = 0;
    this->current.angle.x = 0;
}

/* 023C6220 */
s32 daOship_c::_create() {
    u32 condition = u32(this->actor_condition);
    if (!(condition & 8)) {
        if (gabi::ea(this) != 0) {
            daOship_c__ct(this);
            condition = u32(this->actor_condition);
        }
        this->actor_condition = condition | 8;
    }
    s32 phase = OshipResourceLoad(&this->mPhs, "Oship");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    this->getArg();
    u8 switchA = this->mSwitchA;
    if (switchA != 0xFF) {
        u32 save = dComIfGs_ea();
        s8 room = s8(this->current.roomNo);
        if (OshipIsSwitch(OshipGuest<dSv_info_c>(u32(save + 0x20)), switchA, room))
            return cPhs_ERROR_e;
    }
    u8 switchB = this->mSwitchB;
    if (switchB != 0xFF) {
        u32 save = dComIfGs_ea();
        s8 room = s8(this->current.roomNo);
        if (!OshipIsSwitch(OshipGuest<dSv_info_c>(u32(save + 0x20)), switchB, room))
            return cPhs_ERROR_e;
    }
    u8 map = this->mTriforce;
    // HD: the map check uses the packed index directly; GC adds one.
    if (map != 15) {
        u32 save = dComIfGs_ea();
        if (!OshipIsTriforceMap(&(*OshipGuest<dSv_info_c>(u32(save + 0x20))).mSavedata.mPlayer.mMap,
                                map))
            return cPhs_ERROR_e;
        u8 point = this->mRandomSalvagePoint;
        if (point != 15) {
            save = dComIfGs_ea();
            if (point != u8(OshipGuest<dSv_info_c>(u32(save + 0x20))
                                ->mSavedata.mPlayer.mInfo.mRandomSalvagePoint))
                return cPhs_ERROR_e;
        }
    }
    if (!OshipEntrySolidHeap(this, gabi::guest_fn((const void*)createHeap_CB), 0x1280))
        return cPhs_ERROR_e;
    this->createInit();
    return phase;
}

/* 023C6380 */
s32 daOship_c::_delete() {
    OshipResourceDelete(&this->mPhs, "Oship");
    for (u32 i = 0; i < 3; ++i) {
        mSmokeFollowCallback[i].remove();
    }
    OshipWaveRemove(&this->mWaveCallback2);
    OshipWaveRemove(&this->mWaveCallback1);
    OshipSplashRemove(&this->mSplashCallback);
    OshipTrackRemove(&this->mTrackCallback);
    mDoAud_seDeleteObject(&this->mBombSpawnPos);
    return 1;
}

/* 023C637C */
s32 daOshipCreate(daOship_c* ship) { return ship->_create(); }

/* 023C6418 */
s32 daOshipDelete(daOship_c* ship) { return ship->_delete(); }

/* 023C6D38 */
s32 daOshipExecute(daOship_c* ship) { return ship->_execute(); }

/* 023C6DAC */
s32 daOshipDraw(daOship_c* ship) { return ship->_draw(); }

/* 023C83C0 */
BOOL daOshipIsDelete(daOship_c* ship) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.

/* 023C82F4 */
void OshipSafeStringDelete(void* object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 023C8308 */
void* OshipVectorCtor(void* object) {
    return object ? object : static_cast<void*>(operator_new(12));
}

/* 023C83C8 */
void OshipFollowDelete(void* object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 023C84E0 */
void OshipSafeStringPrepare(void* object) {}

/* 023C5670 */
void* OshipFollowCtor(void* object) { return OshipFollowConstruct(object, 0, 0); }

/* 023C8334 */
void* OshipCylinderCtor(void* object) {
    if (!object)
        object = static_cast<void*>(operator_new(0x130));
    if (!object)
        return nullptr;
    OshipCollisionObjectConstruct(object);
    u32 ea = gabi::ea(object);
    reinterpret_cast<OshipCylinderAttr*>(&OshipGuest<dCcD_Cyl>(ea)->mShapeAttr)->mShapeVtable =
        0x100015A8;
    reinterpret_cast<OshipCylinderAttr*>(&OshipGuest<dCcD_Cyl>(ea)->mShapeAttr)->mAabbVtable =
        0x10033FA4;
    OshipCylinderShapeConstruct(&OshipGuest<dCcD_Cyl>(ea)->mCyl);
    OshipGuest<dCcD_Cyl>(ea)->__vtbl_hitinf = 0x1004B108;
    OshipGuest<dCcD_Cyl>(ea)->mCyl.__vtbl = 0x1004B150;
    reinterpret_cast<OshipCylinderAttr*>(&OshipGuest<dCcD_Cyl>(ea)->mShapeAttr)->mShapeVtable =
        0x1004B160;
    return object;
}

/* 023C567C */
daOship_c* daOship_c__ct(daOship_c* ship) {
    if (!ship)
        ship = static_cast<daOship_c*>(operator_new(0xF1C));
    if (!ship)
        return nullptr;
    fopAc_ac_c_ct(ship);
    ship->__vtbl = 0x10034084;
    // Embedded vector constructors are otherwise empty. Preserve their HD
    // allocation branch when guest address addition wraps to zero.
    for (auto* callback : {&ship->mWaveCallback1, &ship->mWaveCallback2}) {
        callback->__vtbl = 0x100521A8;
        for (u32 i = 0; i < 3; ++i)
            if (gabi::ea(&callback->mRotMtx[i]) == 0)
                operator_new(12);
    }
    ship->mSplashCallback.__vtbl = 0x100521E8;
    ship->mTrackCallback.__vtbl = 0x10052268;
    OshipConstructArray(&ship->mTrackCallback.mPos, 3, 12,
                        gabi::guest_fn((const void*)OshipVectorCtor));
    OshipConstructArray(&ship->mSmokeFollowCallback, 3, 0x14,
                        gabi::guest_fn((const void*)OshipFollowCtor));
    OshipStatusConstruct(&ship->mStts);
    OshipGStatusConstruct(&ship->mStts.__vtbl_gstts);
    ship->mStts.__vtbl = 0x1004AE88;
    ship->mStts.__vtbl_gstts = 0x1004AEC0;
    OshipConstructArray(&ship->mCyl, 5, 0x130, gabi::guest_fn((const void*)OshipCylinderCtor));
    OshipAcchConstruct(&ship->mAcch);
    ship->mAcch.dBgS_PolyPassChk::__vtbl = 0x10033FF4;
    ship->mAcch.cBgS_Chk::__vtbl = 0x10033FD4;
    ship->mAcch.mbObjThrough = 1;
    ship->mAcch.dBgS_GrpPassChk::__vtbl = 0x10033FE4;
    OshipAcchCircleConstruct(&ship->mAcchCir);
    OshipLineCheckConstruct(&ship->mLinChk);
    ship->mLinChk.mGrp = 1;
    ship->mLinChk.mbRopeThrough = 0;
    ship->mLinChk.mbBombThrough = 0;
    ship->mLinChk.mbCamThrough = 0;
    ship->mLinChk.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(&ship->mLinChk));
    ship->mLinChk.dBgS_PolyPassChk::__vtbl = 0x10034074;
    ship->mLinChk.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(&ship->mLinChk));
    ship->mLinChk.cBgS_PolyInfo::__vtbl = 0x10034054;
    ship->mLinChk.dBgS_GrpPassChk::__vtbl = 0x10034064;
    ship->mLinChk.cBgS_Chk::__vtbl = 0x10034044;
    ship->mLinChk.mbLinkThrough = 0;
    ship->mLinChk.mbBoomerangThrough = 0;
    ship->mLinChk.mbArrowThrough = 0;
    ship->mLinChk.mbObjThrough = 1;
    return ship;
}

/* 023C83E0 */
void daOship_c__dt(daOship_c* ship, u32 flags) {
    if (!ship)
        return;
    ship->mLinChk.dBgS_PolyPassChk::__vtbl = 0x10034034;
    ship->mLinChk.dBgS_GrpPassChk::__vtbl = 0x10033FC4;
    ship->mLinChk.cBgS_PolyInfo::__vtbl = 0x10033FB4;
    OshipLineCheckDestroy(&ship->mLinChk, 0);
    OshipCircleShapeDestroy(&ship->mAcchCir.m_cir, 2);
    ship->mAcch.dBgS_GrpPassChk::__vtbl = 0x10033FE4;
    ship->mAcch.dBgS_PolyPassChk::__vtbl = 0x10033FF4;
    OshipAcchDestroy(&ship->mAcch, 0);
    OshipDestroyArray(&ship->mCyl, 5, 0x130, 0x02515A70, 0, 0);
    OshipStatusDestroy(&ship->mStts, 2);
    OshipDestroyArray(&ship->mSmokeFollowCallback, 3, 0x14,
                      gabi::guest_fn((const void*)OshipFollowDelete), 0, 0);
    OshipActorDestroy(ship, 0);
    if (flags & 1)
        operator_delete(ship);
}

/* 023C8090 */
daOship_HIO_c* daOship_HIO_c__ct(daOship_HIO_c* data) {
    if (!data)
        data = static_cast<daOship_HIO_c*>(operator_new(0x94));
    if (!data)
        return nullptr;
    // Named settings retain the actual HD load/store order.
    f32 setting0 = l_defaultAttentionOffsY;
    data->m04 = 0x0;
    f32 setting1 = l_defaultWaveOffsZ;
    data->m07 = 0x0;
    data->mWaveOffsZ = setting1;
    f32 setting2 = l_defaultSplashScaleTarget;
    f32 setting3 = l_defaultRangeB;
    data->mSplashScaleTimerTarget = setting2;
    data->m08 = 0x0;
    data->mDistRangeB = setting3;
    f32 setting4 = l_defaultRangeC;
    data->mAttentionOffsY = setting0;
    data->mDistRangeC = setting4;
    f32 setting5 = l_zero;
    f32 setting6 = l_defaultTrackOffsY;
    data->mAttackDelayA = 0x1E;
    data->m05 = 0x0;
    data->mTrackOffsY = setting6;
    f32 setting7 = l_defaultMaxWaveVelocity;
    f32 setting8 = l_cullMaxX;
    f32 setting9 = l_two;
    data->mEyeOffsY = setting8;
    data->mTrackVel = setting8;
    data->mSplashMaxScaleTimer = setting8;
    f32 setting10 = l_defaultBombOffsX;
    data->m06 = 0x0;
    data->mWaveVelFade1 = setting9;
    data->mTrackOffsZ = setting5;
    f32 setting11 = l_defaultRangeA;
    data->mBombOffset.x = setting10;
    f32 setting12 = l_defaultTrackScaleY;
    data->__vtbl = 0x10034094;
    data->mDistRangeA = setting11;
    data->mWaveVelSpeed = setting9;
    f32 setting13 = l_defaultBombSpeed;
    data->mTrackScaleY = setting12;
    data->mBombSpeed = setting13;
    data->mAttackDelayB = 0xC8;
    f32 setting14 = l_defaultBombAcceleration;
    data->mBombOffset.z = setting5;
    data->mWaveMaxVelocity = setting7;
    f32 setting15 = l_defaultCollapseStartZ;
    f32 setting16 = l_defaultCollapseEndX;
    data->mWaveCollapsePos0.z = setting15;
    f32 setting17 = l_defaultCollapseEndY;
    f32 setting18 = l_defaultBadAimDistance;
    f32 setting19 = l_cullMinY;
    data->mBadAimAdjustDistanceStart = setting18;
    data->mWaveCollapsePos1.y = setting19;
    f32 setting20 = l_defaultCollapseStartX;
    data->mBombAcceleration = setting14;
    data->mWaveCollapsePos0.x = setting20;
    data->mWaveCollapsePos1.x = setting16;
    f32 setting21 = l_defaultBombOffsY;
    data->field_0x8E = 0x3000;
    data->mWaveCollapsePos0.y = setting21;
    f32 setting22 = l_fireDelayRange;
    data->mWaveCollapsePos1.z = setting17;
    data->mPathSpeed = setting22;
    data->mBombOffset.y = setting5;
    data->mBombNoGravityTime = 0x23;
    f32 setting23 = l_defaultPathSpeed;
    data->field_0x90 = 0x3500;
    data->mSpecScale = setting23;
    data->mBadAimMax = 0x3000;
    data->mNode2RotZ = 0x2800;
    data->mWaveVelFadeOffs = setting5;
    return data;
}

/* 023C8254 */
void __sinit_d_a_oship_cpp() {
    l_headerStatics.mZeroWords[2] = 0;
    l_headerStatics.mZeroWords[0] = 0;
    l_headerStatics.mZeroWords[3] = 0;
    l_headerStatics.mZeroWords[1] = 0;
    OshipRegisterGlobalObject(&l_headerRecords.mWords[0]);
    f32 zero = l_negativePi, one = l_positivePi;
    l_headerStatics.mNegativePi = zero;
    l_headerStatics.mPositivePi = one;
    OshipInitializeMath(&l_headerStatics.mMathInitialized);
    OshipRegisterGlobalObject(&l_headerRecords.mWords[3]);
    OshipInitializeMatrix(&l_headerStatics.mMatrixInitialized);
    OshipRegisterGlobalObject(&l_headerRecords.mWords[6]);
    daOship_HIO_c__ct(&l_HIO);
}

// clang-format off
/* WWHD glue
str 1003429C "Oship" in 023C5320
str 100340B4 "modelData != (0)" in 023C5320
str 100340A4 "d_a_oship.cpp" in 023C5320
str 1003429C "Oship" in 023C6220
str 1003429C "Oship" in 023C6380
str 100341EC "Oship" in 023C5F84
str 10034228 "GOLD_SHIP_DELETE" in 023C78D0
data mode_tbl 100340D0
data m_cyl_src 100342A4
data l_flagOffsetGuard 1046CBFC
data l_waveDirection2Guard 1046CBE0
data l_waveDirection1Guard 1046CBDC
data l_flagOffset 1046CB3C
data l_waveDirection2 1046CBF0
data l_waveDirection1 1046CBE4
data l_cylinderHeights 101CE504
data l_cylinderRadii 101CE4F0
data l_cylinderOffsets 101CE4DC
data l_resControl 101F4F28
data l_headerRecords 101CE518
data l_headerStatics 1046CB20
data l_debugRegisters 1047BCD0
data l_matrixStack 1048D0CC
ext 025D6B70 s32 OshipCheckCullingBox(void*, f32, f32, f32, f32, f32, f32)
ext 025D57E0 void OshipActorDelete(void*)
ext 025B8B68 void OshipOnEventBit(void*, u32)
ext 0254457C s32 OshipEventEndCheckOld(void*, const char*)
ext 025E1988 void OshipStartSalvageSound(u32)
ext 024679F4 void OshipSalvageOnForOship(void*, daOship_c*)
ext 025D5218 u32 OshipActorJudge(u32, void*)
ext 025B9E38 void OshipOnSwitch(void*, u32, s32)
ext 0200ECD4 f32 OshipAddCalc(be<f32>*, f32, f32, f32, f32)
ext 025D77DC void OshipOrderOtherEvent(daOship_c*, const char*, u32, u32)
ext 02516300 u32 OshipGetTargetHitObject(void*)
ext 025E19CC void OshipStartBombSound(u32, cXyz*)
ext 020CB89C void OshipBombSetNoGravityTime(void*, s32)
ext 025D5928 u32 OshipFastCreateBomb(s32, u32, cXyz*, s32, csXyz*, void*, s32, u32, u32)
ext 020CB8D8 u32 OshipBombParameters(u32, u32, u32)
ext 024F06B4 void OshipAcchSet(void*, cXyz*, cXyz*, daOship_c*, s32, dBgS_AcchCir*, cXyz*, void*, void*)
ext 025AAF88 u32 OshipGetRoomPath(s32, s32)
ext 025D5834 u32 OshipCreateFlag(s32, u32, cXyz*, s32, csXyz*, void*, s32, u32)
ext 025D674C void OshipSetCullSizeBox(daOship_c*, f32, f32, f32, f32, f32, f32)
ext 027F4D5C void OshipModelCalc(void*)
ext 02516518 void OshipCylinderSet(void*, void*)
ext 0200E814 u32 OshipCharacterNameIndex(void*, const char*, s32)
ext 028E8F64 void OshipMatrixMultVec(void*, cXyz*, cXyz*)
ext 025F1C28 void OshipMatrixYrotM(void*, s32)
ext 025F19F8 void OshipMatrixXYZrotM(void*, s32, s32, s32)
ext 028E93CC void OshipMatrixTrans(void*, f32, f32, f32)
ext 025D6894 u32 OshipActorAngleY(daOship_c*, void*)
ext 025872F4 void OshipWaveRotHD(cXyz*, dLib_wave_c*, f32)
ext 025A847C void OshipParticleSet(void*, u32, u32, cXyz*, const void*, const void*, u32, void*, s32, const void*, const void*, const void*)
ext 025A9E38 void OshipTrackRemove(void*)
ext 025A99B8 void OshipSplashRemove(void*)
ext 025A92C0 void OshipWaveRemove(void*)
ext 025204C8 void OshipResourceDelete(request_of_phase_process_class*, const char*)
ext 025D63E8 s32 OshipEntrySolidHeap(daOship_c*, u32, u32)
ext 025B8308 s32 OshipIsTriforceMap(void*, s32)
ext 025BA0C0 s32 OshipIsSwitch(void*, s32, s32)
ext 02520460 s32 OshipResourceLoad(request_of_phase_process_class*, const char*)
ext 0200E240 void OshipCollisionSet(void*, void*)
ext 02018428 void OshipCylinderSetH(void*, f32)
ext 020184DC void OshipCylinderSetR(void*, f32)
ext 020182E0 void OshipCylinderSetC(void*, cXyz*)
ext 025F1B48 void OshipMatrixZXYrotM(void*, s32, s32, s32)
ext 025F1C5C void OshipMatrixZrotM(void*, s32)
ext 0201A478 void OshipRotationConstruct(csXyz*, s32, s32, s32)
ext 028E90D4 void OshipMatrixCopy(void*, void*)
ext 0200F764 void OshipChasePosXZ(cXyz*, cXyz*, f32)
ext 0201B47C s32 OshipVectorNormalizeRS(cXyz*)
ext 0201ADE0 void OshipVectorSubtract(cXyz*, cXyz*, cXyz*)
ext 028EAB2C void OshipInitializeMatrix(void*)
ext 028ED6F8 void OshipInitializeMath(void*)
ext 028F026C void OshipRegisterGlobalObject(void*)
ext 025D99E8 void OshipCreateDisappear(fopAc_ac_c*, cXyz*, u32, u32, u32)
ext 0200F268 void OshipAddCalcPosXZ2(cXyz*, cXyz*, f32, f32)
ext 02587D24 void OshipPathMoveHD(cXyz*, be<s8>*, void*, u32, daOship_c*, f32)
ext 02008860 s32 OshipLineCross(void*, void*)
ext 025E2E5C void OshipModelEntryDL(void*)
ext 0252CA4C void OshipTexSpecmapST(cXyz*, dKy_tevstr_c*, void*, f32)
ext 02562F5C void OshipSetLightTevColorType(void*, void*, dKy_tevstr_c*)
ext 025626A4 void OshipSettingTevStruct(void*, u32, cXyz*, dKy_tevstr_c*)
ext 025E38E0 void* OshipCreateModel(void*, u32, u32)
ext 0273AA24 void OshipAssert(const char*, u32, const char*)
ext 026066C4 void* OshipGetObjectResource(void*, SafeString*, s32)
ext 025D50BC void OshipActorDestroy(void*, u32)
ext 02515860 void OshipStatusDestroy(void*, u32)
ext 028F0164 void OshipDestroyArray(void*, u32, u32, u32, u32, u32)
ext 024EFD9C void OshipAcchDestroy(void*, u32)
ext 02018034 void OshipCircleShapeDestroy(void*, u32)
ext 02008B4C void OshipLineCheckDestroy(void*, u32)
ext 02008FEC void OshipLineCheckConstruct(void*)
ext 024EFE94 void OshipAcchCircleConstruct(void*)
ext 024F0474 void OshipAcchConstruct(void*)
ext 02515DA0 void OshipGStatusConstruct(void*)
ext 0200BD2C void OshipStatusConstruct(void*)
ext 028EFFD0 void OshipConstructArray(void*, u32, u32, u32)
ext 025D6958 f32 OshipActorDistanceXZ(fopAc_ac_c*, void*)
ext 02018590 void OshipCylinderShapeConstruct(void*)
ext 02515FB8 void OshipCollisionObjectConstruct(void*)
ext 025A5894 void* OshipFollowConstruct(void*, u32, u32)
data l_zero 100340C8
data l_one 100340CC
data l_waveScaleZ 100341B8
data l_flagScale 100341BC
data l_trackScale 100341C0
data l_smokeScale 100341C4
data l_wavePhaseRange 100341C8
data l_cullMaxZ 100341CC
data l_cullMinX 100341D0
data l_cullMinY 100341D4
data l_cullMinZ 100341D8
data l_cullMaxX 100341DC
data l_cullMaxY 100341E0
data l_ten 100341E4
data l_wallRadius 100341E8
data l_two 100341F4
data l_noWaterLevel 100341F8
data l_smoothingRate 100341FC
data l_bombHeightBias 10034200
data l_maxWaveDistance 10034204
data l_hundred 10034208
data l_badAimExtra 1003420C
data l_damageScale 10034210
data l_pathRadiusBias 10034214
data l_swayScale 10034218
data l_sinkOffset 1003421C
data l_sinkSmoothing 10034220
data l_sinkMaxStep 10034224
data l_fireDelayRange 1003423C
data l_defaultTrackOffsY 10034240
data l_defaultTrackScaleY 10034244
data l_defaultAttentionOffsY 10034248
data l_defaultWaveOffsZ 1003424C
data l_defaultRangeA 10034250
data l_defaultRangeB 10034254
data l_defaultRangeC 10034258
data l_defaultSplashScaleTarget 1003425C
data l_defaultMaxWaveVelocity 10034260
data l_defaultCollapseStartX 10034264
data l_defaultBombOffsY 10034268
data l_defaultCollapseStartZ 1003426C
data l_defaultCollapseEndX 10034270
data l_defaultCollapseEndY 10034274
data l_defaultBombOffsX 10034278
data l_defaultBombSpeed 1003427C
data l_defaultBombAcceleration 10034280
data l_defaultBadAimDistance 10034284
data l_defaultPathSpeed 10034288
data l_negativePi 10034290
data l_positivePi 10034294
ext 028F4384 f32 OshipSqrtLiveFpr()
ext 028F4384 f64 OshipSqrt(f64)
ext 027F7878 u32 OshipJointDescriptor(void*)
ext 027F3F94 u32 OshipJointTreeHeader(void*)
ext 025871F8 f32 OshipGetWaterY(const cXyz*, void*)
ext 02520540 s32 OshipGetReverb(s32)
ext 0211D2F8 s32 OshipCalcTimer(be<s32>*)
ext 0200FAAC s32 OshipDistanceAngleS(s32, s32)
ext 0200F378 s32 OshipAddCalcAngleS(void*, s32, s32, s32, s32)
ext 0200F428 void OshipAddCalcAngleS2(be<s16>*, s32, s32, s32)
ext 020195B0 s32 OshipAtan2s(f32, f32)
ext 0200F974 s32 OshipTargetAngleX(const cXyz*, const cXyz*)
ext 0200F93C s32 OshipTargetAngleY(const cXyz*, const cXyz*)
data l_HIO 1046CB48
*/

// clang-format on
