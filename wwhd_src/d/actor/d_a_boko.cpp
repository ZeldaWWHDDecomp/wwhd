/**
 * Enemy weapons (Boko sticks, swords, mace and spear).
 * Ported from tww's daBoko_c onto the verified WWHD layout.
 * Every address remains verified.
 */
#include "d/actor/d_a_boko.h"
static dCcD_SrcCps l_cps_src;
static dCcD_SrcSph l_sph_src;
static GXColor l_particleColor;
static GXColor l_lineColor;

static u32 m_bound_se[6] = {0x5811, 0x5814, 0x6999, 0x6998, 0x5807, 0x6998};
static u32 m_heap_size[6] = {0x2300, 0x1240, 0x1240, 0x1240, 0x4200, 0x2000};
static u32 model_idx[6] = {0x3, 0x3, 0x3, 0x3, 0x3, 0x4};
static s16 base_angle[4] = {150, 200, 180, 120};
static u32 m_at_type[6] = {0x80, 0x400, 0x1000000, 0x4000000, 0x10000000, 0x20000000};
static s32 m_at_point[6] = {1, 2, 4, 4, 4, 4};
static f32 m_cps_r[6] = {30.0f, 30.0f, 50.0f, 50.0f, 50.0f, 50.0f};
static u8 m_se_type[6] = {0x4, 0x1, 0x1, 0x1, 0x3, 0x1};

// HD: runtime debug-tuning values used by this translation unit.
static s16 bokoCarryHideTimer;
static s16 bokoLitSpawnDelay;
static s16 bokoSlopeAngle;
static u8 bokoStayRoom;
static f32 bokoChainDamping;
static f32 bokoChainIndexDamping;
static f32 bokoLitSpawnSpeed;
static f32 bokoFlameScaleRate;
static f32 bokoFlameScaleGrowth;
static f32 bokoFlameScaleLimit;
static f32 bokoChainGravity;
static f32 bokoGroundProbeDepth;
static f32 bokoGroundGap;
static f32 bokoGroundCorrection;
static f32 bokoMoveDamping;
static u32 bokoFrameCounter;
static BokoResourceControlPointer bokoResourceControl;
static BokoGameInfoPointer bokoGameInfoPointer;
static BokoMatrixPointer bokoMatrixPointer;

static cXyz m_cull_min[6] = {
    {-20.0f, -10.0f, -95.0f},
    {-20.0f, -10.0f, -95.0f},
    {-35.0f, -35.0f, -125.0f},
    {-45.0f, -10.0f, -145.0f},
    {-20.0f, -20.0f, -180.0f},
    {-50.0f, -20.0f, -140.0f}
};
static cXyz m_cull_max[6] = {
    {20.0f, 10.0f, 95.0f},
    {20.0f, 10.0f, 95.0f},
    {35.0f, 35.0f, 120.0f},
    {45.0f, 10.0f, 130.0f},
    {20.0f, 20.0f, 200.0f},
    {50.0f, 20.0f, 120.0f}
};
static cXyz m_top_offset[6] = {
    {0.0f, 0.0f, 85.0f},
    {16.0f, 0.0f, 77.0f},
    {-8.0f, 3.0f, 120.0f},
    {0.0f, 0.0f, 128.0f},
    {10.0f, 0.0f, 200.0f},
    {-40.0f, 0.0f, 110.0f}
};
static cXyz m_blur_root_offset[6] = {
    {0.0f, 0.0f, -45.0f},
    {0.0f, 0.0f, -43.0f},
    {-1.0f, 3.0f, -60.0f},
    {0.0f, 0.0f, -50.0f},
    {0.0f, 0.0f, -100.0f},
    {-20.0f, 0.0f, -50.0f}
};
static cXyz m_root_offset[6] = {
    {0.0f, 0.0f, -85.0f},
    {5.0f, 0.0f, -90.0f},
    {3.0f, 0.0f, -120.0f},
    {0.0f, 0.0f, -130.0f},
    {0.0f, 0.0f, -180.0f},
    {0.0f, 0.0f, -125.0f}
};

static u32 bokoRegistration[4] = {0,0,0,0};
static u32 bokoTailRegistration[4] = {0,0,0,0};
static f32 bokoAngleMin;
static f32 bokoAngleMax;
static u8 bokoAngleStorage;
static u8 bokoFlagStorage;
static f32 bokoTailAngleMin;
static f32 bokoTailAngleMax;
static u8 bokoTailAngleStorage;
static u8 bokoTailFlagStorage;
static cXyz l_break_particle_offset;
static dBgS_ObjGndChk m_ground_check;
static dBgS_LinChk m_line_check;
static u32 bokoFlameFlags;
static u8 bokoGlobalRecord0;
static u8 bokoGlobalRecord1;
static u8 bokoGlobalRecord2;
static u8 bokoGlobalRecord3;
static u8 bokoGlobalRecord4;
static u8 bokoGlobalRecord5;
static u8 bokoGlobalRecord6;
static u8 bokoGlobalRecord7;

static BokoArchivePtr m_arc_name[6] = {{0x1000a2d8}, {0x1000a2e0}, {0x1000a2e8}, {0x1000a2f0}, {0x1000a2d0}, {0x1000a2f8}};


/* 020C0C5C */
void daBoko_c::keDraw() {
  bokoUpdateLine(this->mpLineKe.get(), 10, &l_lineColor, 2,
                   &this->tevStr, 1.25f);
  LineKe* line = this->mpLineKe;
  auto* p = bokoGameInfo();
  s32 index = bokoLineSortIndex(line);
  bokoSortLine(bokoLineSortPacket(p, index), line);
}

/* 020C1250 */
void daBoko_c::keCalc1(ke_c_s* chainObject, s32 index) {
  if (!s16(this->mChainTimer))
    return;
  f32 gravity = bokoChainGravity + -8.25f;
  BokoVectorFrame step;
  step.payload.x = 0.0f;
  step.payload.y = 0.0f;
  step.payload.z = 21.875f;
  BokoGroundFrame ground;
  u32 g = gabi::ea(&ground.payload);
  bokoGroundConstruct(&ground.payload);
  f32 coefficient = bokoChainIndexDamping + 0.007f;
  ground.payload.cBgS_Chk::__vtbl = BOKO_GROUND_CHECK_VTABLE;
  ground.payload.cBgS_PolyInfo::__vtbl = BOKO_GROUND_POLY_INFO_VTABLE;
  ground.payload.mbObjThrough = 0;
  ground.payload.mbCamThrough = 0;
  ground.payload.mbLinkThrough = 0;
  ground.payload.mbArrowThrough = 0;
  ground.payload.mbBombThrough = 0;
  ground.payload.mbBoomerangThrough = 0;
  ground.payload.mbRopeThrough = 0;
  ground.payload.dBgS_GrpPassChk::__vtbl = BOKO_GROUND_GROUP_VTABLE;
  ground.payload.dBgS_PolyPassChk::__vtbl = BOKO_GROUND_POLY_PASS_VTABLE;
  ground.payload.cBgS_Chk::mGrpPassChk = g + 0x4C;
  ground.payload.cBgS_Chk::mPolyPassChk = g + 0x40;
  ground.payload.mGrp = 1;
  f32 heightOffset = 50.0f, floorOffset = 3.0f;
  f32 damping = gabi::fmadds(f32(index), coefficient, f32(this->mpLineKe->mDamping));
  f32 sway = 0.6667f * damping;
  u32 phaseX = u32(index) * 0x1388 - 0x2328, phaseZ = (u32(index) - 1) * 0x1B58;
  for (u32 i = 1; i < 10; i++) {
    cXyz& position = chainObject->m000[i];
    cXyz& previous = chainObject->m000[i - 1];
    cXyz& velocity = chainObject->m078[i];
    f32 px = f32(position.x), pz = f32(position.z);
    f32 dx = (px - f32(previous.x)) + f32(velocity.x);
    f32 dz = (pz - f32(previous.z)) + f32(velocity.z);
    u32 count = bokoFrameCounter;
    f32 horizontalX =
        gabi::fmadds(cM_ssin(phaseX), sway, dx);
    f32 horizontalZ =
        gabi::fmadds(cM_ssin(phaseZ), sway, dz);
    f32 floor;
    // HD: stagger floor queries over eight frames and reuse the cached height.
    if ((count + i + u32(index) * 10) & 7)
      floor = f32(chainObject->mGroundHeight[i]);
    else {
      ground.payload.m_pos.z = pz;
      ground.payload.m_pos.x = px;
      ground.payload.m_pos.y = f32(position.y) + heightOffset;
      auto* play = bokoGameInfo();
      floor = bokoGroundCross(&play->play.mBgS, &ground.payload) +
              floorOffset;
      chainObject->mGroundHeight[i] = floor;
    }
    f32 vertical = f32(position.y) + gravity;
    if (vertical < floor)
      vertical = floor;
    f32 dy = vertical - f32(previous.y);
    s16 pitch = s16(-bokoAtan2(dy, horizontalZ));
    f32 square = gabi::fmadds(dy, dy, horizontalZ * horizontalZ),
        length = bokoSqrt(square);
    s32 yaw = bokoAtan2(horizontalX, length);
    bokoRotateXSet(bokoMatrixPointer.get(), pitch);
    bokoRotateY(bokoMatrixPointer.get(), yaw);
    BokoVectorFrame transformed;
    bokoTransformStackVector(&step.payload, &transformed.payload);
    velocity.x = f32(position.x);
    velocity.z = f32(position.z);
    f32 nextX = f32(previous.x) + f32(transformed.payload.x);
    position.x = nextX;
    f32 nextY = f32(previous.y) + f32(transformed.payload.y);
    position.y = nextY;
    f32 nextZ = f32(previous.z) + f32(transformed.payload.z);
    position.z = nextZ;
    f32 oldX = f32(velocity.x), oldZ = f32(velocity.z);
    velocity.x = (nextX - oldX) * damping;
    velocity.z = (f32(position.z) - oldZ) * damping;
    phaseZ -= 0x1B58;
    phaseX -= 0x2328;
  }
  LineKe* lineData = this->mpLineKe;
  cXyz* output = lineData->mRows.get()[index].mPositions;
  for (u32 i = 0; i < 10; i++) {
    bokoWord(output[i].x) = u32(bokoWord(chainObject->m000[i].x));
    bokoWord(output[i].y) = u32(bokoWord(chainObject->m000[i].y));
    bokoWord(output[i].z) = u32(bokoWord(chainObject->m000[i].z));
  }
  ground.payload.dBgS_PolyPassChk::__vtbl = BOKO_GROUND_POLY_PASS_VTABLE;
  ground.payload.cBgS_PolyInfo::__vtbl = BOKO_GROUND_POLY_INFO_VTABLE;
  ground.payload.dBgS_GrpPassChk::__vtbl = BOKO_GROUP_DESTRUCTOR_VTABLE;
  bokoCheckDestruct(&ground.payload, 0);
}

/* 020C1684 */
void daBoko_c::keCalc() {
  ke_c_s* chain = this->mpLineKe->m001C;
  f32 goal = (u32(bokoWord(this->actor_status)) & 0x2000)
                 ? bokoChainDamping + 0.7f
                 : 0.0f;
  bokoChaseFloat(&this->mpLineKe->mDamping, goal, 0.05f);
  s16 timer = s16(this->mChainTimer);
  J3DModel* model = this->mpModel;
  if (timer)
    this->mChainTimer = s16(timer - 1);
  bokoCopyMatrix(model->getBaseTRMtx(),
                   bokoMatrixPointer.get());
  f32 amplitude = 12.0f, base = 100.0f,
      depth = 10.0f;
  for (u32 i = 0; i < 16; i++) {
    bokoMatrixPush();
    f32 x = cM_ssin(i * 0x3A98) * amplitude;
    f32 y = cM_ssin(i * 0x2710) * amplitude;
    f32 z = gabi::fmadds(cM_ssin(i * 0x1B58),
                         depth, base);
    BokoVectorFrame offset;
    offset.payload.x = x;
    offset.payload.y = y;
    offset.payload.z = z;
    bokoTransformStackVector(&offset.payload, chain);
    bokoCallChain(this, chain, i);
    bokoMatrixPop();
    chain++;
  }
}

/* 020C0CE0 */
BOOL daBoko_c::draw() {
  auto* p = bokoGameInfo();
  fopAc_ac_c* player = p->play.mPlayerInfo[0].mpPlayer;
  BOOL hidden = this->checkNoDraw();
  auto* emitter = reinterpret_cast<BokoEmitterHD*>(this->mParticleCallBack.mpEmitter.get());
  if (hidden) {
    if (emitter)
      emitter->mStatus = u32(emitter->mStatus) | 4;
    return 1;
  }
  if (emitter)
    emitter->mStatus = u32(emitter->mStatus) & ~4u;
  if (this->mNowMode == Mode_PLAYER_CARRY_e) {
    if (bokoPlayerNoDraw(player))
      return 1;
  }
  auto* env = bokoEnvironment();
  bokoSetupLighting(env, 0, &this->current.pos, &this->tevStr);
  env = bokoEnvironment();
  bokoModelLighting(env, this->mpModel.get(), &this->tevStr);
  if (this->mParameters == Type_PGANON_SWORD_e) {
    J3DModel* model = this->mpModel;
    f32 frame = f32(this->mBrkAnm.mFrameCtrl.mFrame);
    bokoEnterBrk(&this->mBrkAnm,
                     model->getModelData(), frame);
  }
  bokoModelEntry(this->mpModel.get());
  if (this->mParameters == Type_PGANON_SWORD_e) {
    J3DModel* model = this->mpModel;
    static_cast<BokoModelDataHD*>(model->getModelData())->mBrkAnimation = 0;
  }
  if (this->mpLineKe.get())
    this->keDraw();
  return 1;
}

/* 020C0E04 */
BOOL daBoko_Draw(daBoko_c *a) {
  return a->draw();
}

/* 020C4084 */
void daBoko_c::getTopPos(cXyz* out) {
  J3DModel* model = this->mpModel;
  if (model) {
    u32 type = this->mParameters;
    bokoTransformVector(&model->mBaseTransformMtx, &m_top_offset[type],
                     out);
  } else {
    u32 type = this->mParameters;
    BokoVectorFrame pos;
    bokoVectorAdd(&this->current.pos, &pos.payload,
                     &m_top_offset[type]);
    u32 x = u32(bokoWord(pos.payload.x)),
        z = u32(bokoWord(pos.payload.z));
    bokoWord(out->x) = x;
    u32 y = u32(bokoWord(pos.payload.y));
    bokoWord(out->z) = z;
    bokoWord(out->y) = y;
  }
}

/* 020C4128 */
void daBoko_c::getBlurRootPos(cXyz* out) {
  J3DModel* model = this->mpModel;
  if (model) {
    u32 type = this->mParameters;
    bokoTransformVector(&model->mBaseTransformMtx, &m_blur_root_offset[type],
                     out);
  } else {
    u32 type = this->mParameters;
    BokoVectorFrame pos;
    bokoVectorAdd(&this->current.pos, &pos.payload,
                     &m_blur_root_offset[type]);
    u32 x = u32(bokoWord(pos.payload.x)),
        z = u32(bokoWord(pos.payload.z));
    bokoWord(out->x) = x;
    u32 y = u32(bokoWord(pos.payload.y));
    bokoWord(out->z) = z;
    bokoWord(out->y) = y;
  }
}

/* 020C0E08 */
void daBoko_c::setTopRootPos(s32 usePosition) {
  u32 type = this->mParameters;
  if (usePosition) {
    f32 y = f32(this->current.pos.y) + 5.0f, x = this->current.pos.x,
        z = this->current.pos.z;
    bokoMatrixTranslation(mDoMtx_stack_c::get(), x, y, z);
    s16 rx = this->shape_angle.x, ry = this->shape_angle.y, rz = this->shape_angle.z;
    bokoRotateZXY(mDoMtx_stack_c::get(), rx, ry, rz);
  } else {
    J3DModel* model = this->mpModel;
    bokoCopyMatrix(model->getBaseTRMtx(), mDoMtx_stack_c::get());
  }
  bokoTransformVector(mDoMtx_stack_c::get(), &m_top_offset[type],
                   &this->m2D0);
  bokoTransformVector(mDoMtx_stack_c::get(), &m_root_offset[type],
                   &this->m2DC);
}

/* 020C2088 */
void daBoko_c::setBaseMatrix() {
  f32 y = f32(this->current.pos.y) + 5.0f, x = this->current.pos.x,
      z = this->current.pos.z;
  bokoMatrixTranslation(mDoMtx_stack_c::get(), x, y, z);
  s16 rx = this->shape_angle.x, ry = this->shape_angle.y, rz = this->shape_angle.z;
  bokoRotateZXY(mDoMtx_stack_c::get(), rx, ry, rz);
  if (this->mParameters == Type_STALFOS_MACE_e) {
    bokoRotateX(mDoMtx_stack_c::get(), -1000);
    f32 zero = 0.0f, height = 10.0f;
    bokoTranslateMatrix(zero, height, zero);
  }
  // Load all values before stores, matching the overlapping matrix-copy
  // semantics.
  f32 values[12];
  for (u32 i = 0; i < 12; i++)
    values[i] = mDoMtx_stack_c::get()->m[i / 4][i % 4];
  J3DModel* model = this->mpModel;
  for (u32 i = 0; i < 12; i++)
    model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
}

/* 020C0BFC */
BOOL daBoko_c::checkNoDraw() {
  if (this->m2B9 || this->mNowMode == Mode_4_e)
    return 1;
  if (this->mNowMode == Mode_PLAYER_CARRY_e) {
    auto* p = bokoGameInfo();
    if (u32(p->play.mPlayerStatus[0][0]) & 0x80)
      return 1;
  }
  return 0;
}

/* 020C0EF8 */
void daBoko_c::setFlameEffect() {
  auto* data = bokoDarkStatus();
  f32 one = 1.0f;
  if (data) {
    f32 step = 0.01f, randomRange = 0.2f,
        rate = 0.5f;
    for (u32 i = 0; i < 4; i++) {
      f32 random = bokoRandom(randomRange);
      bokoApproachFloat(&this->m7E4, random + one, rate,
                       step);
      s16 factor = base_angle[i];
      u32 count = bokoFrameCounter;
      f32 radius = f32(data->mShadowRadius[i]),
          scale = f32(this->m7E4);
      s16 angle = s16(count * u32(s32(factor)));
      f32 size = radius * scale;
      f32 x = this->m2D0.x, z = this->m2D0.z, y = this->m2D0.y;
      bokoMatrixTranslation(mDoMtx_stack_c::get(), x, y, z);
      bokoRotateZXY(mDoMtx_stack_c::get(), 0, angle, angle);
      bokoScaleMatrix(size, size, size);
      bokoCopyMatrix(mDoMtx_stack_c::get(), &this->mAlphaModelMtx[i]);
    }
  }
  auto* emitter = reinterpret_cast<BokoEmitterHD*>(this->mParticleCallBack.mpEmitter.get());
  if (!emitter) {
    auto* p = bokoGameInfo();
  dPa_control_c* manager = p->play.mParticle;
    bokoSetParticle(manager, 0, 0x1EA, &this->m2D0, 0, &this->scale,
                     255, &this->mParticleCallBack, -1, 0, 0, 0);
    u32 y = u32(bokoWord(this->m2D0.y)), z = u32(bokoWord(this->m2D0.z));
    bokoWord(this->m320.y) = y;
    bokoWord(this->m320.z) = z;
    u32 x = u32(bokoWord(this->m2D0.x));
    emitter = reinterpret_cast<BokoEmitterHD*>(this->mParticleCallBack.mpEmitter.get());
    bokoWord(this->m320.x) = x;
  }
  if (emitter) {
    f32 rate = bokoFlameScaleRate + -0.03f;
    f32 x = (f32(this->m2D0.x) - f32(this->m320.x)) * rate,
        low = -1.0f;
    if (x > one)
      x = one;
    else if (x < low)
      x = low;
    f32 z = (f32(this->m2D0.z) - f32(this->m320.z)) * rate;
    if (z > one)
      z = one;
    else if (z < low)
      z = low;
    emitter->mEmitterDir.z = z;
    emitter->mEmitterDir.x = x;
    emitter->mEmitterDir.y = 0.1f;
    BokoVectorFrame motion;
    bokoVectorSubtract(&this->current.pos, &motion.payload,
                     &this->old.pos);
    f32 my = motion.payload.y, mx = motion.payload.x, mz = motion.payload.z;
    f32 square = gabi::fmadds(mz, mz, gabi::fmadds(mx, mx, my * my)),
        distance = bokoSqrt(square);
    f32 growth = bokoFlameScaleGrowth + 0.05f,
        limit = bokoFlameScaleLimit + 2.0f,
        scale = gabi::fmadds(distance, growth, one);
    if (scale > limit)
      scale = limit;
    emitter->mGlobalScale.z = one;
    emitter->mGlobalScale.x = one;
    emitter->mGlobalScale.y = scale;
    u32 y = u32(bokoWord(this->m2D0.y)), xword = u32(bokoWord(this->m2D0.x));
    bokoWord(this->m320.y) = y;
    s8 room = s8(this->current.roomNo);
    u32 zword = u32(bokoWord(this->m2D0.z));
    bokoWord(this->m320.x) = xword;
    bokoWord(this->m320.z) = zword;
    s32 reverb = bokoReverb(s32(room));
    bokoStartSound(0x6103, &this->m2D0, 0, reverb);
  }
  if (!this->checkNoDraw()) {
    auto* p = bokoGameInfo();
  dPa_control_c* manager = p->play.mParticle;
    bokoSetSimpleParticle(manager, 0x4004, &this->m2D0, 255,
                     &l_particleColor, &l_particleColor, 0);
  }
}

/* 020C28E8 */
void daBoko_c::setRoomInfo() {
  u8 room;
  if (f32(this->mAcch.m_ground_h) != -1000000000.0f) {
    auto* p = bokoGameInfo();
    room = bokoPolygonRoom(&p->play.mBgS, static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd));
    p = bokoGameInfo();
    u8 color = bokoPolygonColor(&p->play.mBgS, static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd));
    this->current.roomNo = room;
    this->mStts.mRoomId = room;
    this->tevStr.mEnvrIdxOverride = color;
    this->tevStr.mRoomNo = room;
  } else {
    room = bokoStayRoom;
    this->mStts.mRoomId = room;
    this->tevStr.mRoomNo = room;
    this->current.roomNo = room;
  }
}

/* 020C26AC */
void daBoko_c::setThrowReverse(s16 direction) {
  u32 type = this->mParameters;
  s8 room = this->current.roomNo;
  u32 sound = m_bound_se[type];
  s32 reverb = bokoReverb(s32(room));
  bokoStartSound(sound, &this->eyePos, 100, reverb);
  this->procMove_init();
  this->m2C2 = this->shape_angle.x < 0 ? 0x7FF : -0x7FF;
  type = this->mParameters;
  this->m2C0 = 0;
  this->gravity = -3.0f;
  f32 spin;
  if (type == Type_MOBLIN_SPEAR_e) {
    f32 speed = this->speedF,
        bounce = bokoChainIndexDamping + -20.0f,
        rate = 0.25f;
    spin = 1500.0f;
    this->speed.y = bounce;
    this->speedF = speed * rate;
  } else {
    f32 zero = 0.0f, rate = 0.25f;
    this->speed.y = zero;
    f32 speed = this->speedF;
    spin = 2000.0f;
    this->speedF = speed * rate;
  }
  f32 range = 5000.0f,
      random = bokoRandomSymmetric(range);
  s16 yaw = s16(gabi::ftoi(f32(s32(direction)) + random));
  this->current.angle.y = yaw;
  s16 shape = this->shape_angle.y;
  s32 distance = bokoAngleDistance(yaw, shape);
  range = 500.0f;
  random = bokoRandomSymmetric(range);
  if (distance < 0x4000) {
    s16 rotation = s16(gabi::ftoi(spin + random));
    this->shape_angle.y = this->current.angle.y;
    this->m2C2 = rotation;
  } else {
    f32 value = spin + random;
    s16 angle = this->current.angle.y, rotation = s16(gabi::ftoi(-value));
    this->shape_angle.y = s16(u16(angle) + 0x8000);
    this->m2C2 = rotation;
  }
}

/* 020C25A8 */
BOOL daBoko_c::procWait_init() {
  u32 flags = u32(bokoWord(this->attention_info.flags));
  f32 zero = 0.0f;
  this->mCurrentProc = 0x020C3964;
  this->mNowMode = Mode_WAIT_e;
  bokoWord(this->attention_info.flags) = flags | 0x10;
  this->m2C2 = 0;
  this->m2C0 = 0;
  this->speedF = zero;
  this->mProcSlot = -1;
  this->mProcDelta = 0;
  bokoCancelCarry(this);
  this->gravity = -3.0f;
  return 1;
}

/* 020C3964 */
BOOL daBoko_c::procWait() {
  BokoGroundFrame ground;
  u32 g = gabi::ea(&ground.payload);
  bokoGroundConstruct(&ground.payload);
  f32 heightOffset = 100.0f;
  ground.payload.mbObjThrough = 0;
  ground.payload.mbCamThrough = 0;
  ground.payload.mbLinkThrough = 0;
  ground.payload.mbArrowThrough = 0;
  ground.payload.mbBombThrough = 0;
  ground.payload.mbBoomerangThrough = 0;
  ground.payload.mbRopeThrough = 0;
  ground.payload.dBgS_GrpPassChk::__vtbl = BOKO_GROUND_GROUP_VTABLE;
  ground.payload.dBgS_PolyPassChk::__vtbl = BOKO_GROUND_POLY_PASS_VTABLE;
  ground.payload.cBgS_Chk::mGrpPassChk = g + 0x4C;
  ground.payload.mGrp = 1;
  ground.payload.cBgS_Chk::mPolyPassChk = g + 0x40;
  ground.payload.cBgS_Chk::__vtbl = BOKO_GROUND_CHECK_VTABLE;
  ground.payload.cBgS_PolyInfo::__vtbl = BOKO_GROUND_POLY_INFO_VTABLE;
  ground.payload.m_pos.z = f32(this->m2DC.z);
  ground.payload.m_pos.y = f32(this->m2DC.y) + heightOffset;
  ground.payload.m_pos.x = f32(this->m2DC.x);
  auto* play = bokoGameInfo();
  f32 rootFloor = bokoGroundCross(&play->play.mBgS, &ground.payload);
  ground.payload.m_pos.z = f32(this->m2D0.z);
  ground.payload.m_pos.x = f32(this->m2D0.x);
  ground.payload.m_pos.y = f32(this->m2D0.y) + heightOffset;
  BokoVectorFrame difference;
  bokoVectorSubtract(&this->m2D0, &difference.payload, &this->m2DC);
  play = bokoGameInfo();
  dBgS* world = &play->play.mBgS;
  f32 zero = 0.0f;
  BokoVectorFrame horizontal;
  horizontal.payload.x = difference.payload.x;
  horizontal.payload.y = zero;
  horizontal.payload.z = difference.payload.z;
  f32 square = bokoVectorSquareMagnitude(&horizontal.payload),
      length = bokoSqrt(square),
      topFloor = bokoGroundCross(world, &ground.payload);
  bokoAtan2(-(topFloor - rootFloor), length);
  if (u32(bokoWord(this->actor_status)) & 0x2000) {
    this->mProcDelta = 0;
    this->mProcSlot = -1;
    this->gravity = zero;
    this->mCurrentProc = 0x020C3480;
    this->speedF = zero;
    bokoWord(this->speed.x) = u32(bokoWord(cXyz::Zero.x));
    bokoWord(this->speed.y) = u32(bokoWord(cXyz::Zero.y));
    bokoWord(this->speed.z) = u32(bokoWord(cXyz::Zero.z));
    u8 mode = this->mNowMode;
    u32 flags = u32(bokoWord(this->attention_info.flags));
    if (mode != Mode_PLAYER_CARRY_e)
      this->mNowMode = Mode_ENEMY_CARRY_e;
    this->mbFloorFlg = 0;
    bokoWord(this->attention_info.flags) = flags & ~0x10u;
    bokoCallCarry(this);
  } else if (!this->mbFloorFlg) {
    bokoMovePosition(this, 0);
    play = bokoGameInfo();
    bokoCorrectPosition(&this->mAcch, &play->play.mBgS);
    BokoLineFrame line;
    u32 l = gabi::ea(&line.payload);
    bokoLineConstruct(&line.payload);
    BokoVectorFrame start;
    BokoVectorFrame end;
    f32 x = this->current.pos.x, y = this->current.pos.y, z = this->current.pos.z,
        offset = 30.0f;
    start.payload.x = x;
    start.payload.y = y - offset;
    start.payload.z = z;
    end.payload.x = x;
    end.payload.y = y + offset;
    end.payload.z = z;
    line.payload.mbObjThrough = 0;
    line.payload.mbCamThrough = 0;
    line.payload.mbLinkThrough = 0;
    line.payload.mbArrowThrough = 0;
    line.payload.mbBombThrough = 0;
    line.payload.mbBoomerangThrough = 0;
    line.payload.mbRopeThrough = 0;
    line.payload.mGrp = 1;
    line.payload.cBgS_Chk::mPolyPassChk = l + 0x58;
    line.payload.cBgS_Chk::mGrpPassChk = l + 0x64;
    line.payload.cBgS_PolyInfo::__vtbl = BOKO_LINE_POLY_INFO_VTABLE;
    line.payload.cBgS_Chk::__vtbl = BOKO_LINE_CHECK_VTABLE;
    line.payload.dBgS_PolyPassChk::__vtbl = BOKO_LINE_POLY_PASS_VTABLE;
    line.payload.dBgS_GrpPassChk::__vtbl = BOKO_LINE_GROUP_VTABLE;
    bokoSetLine(&line.payload, &start.payload, &end.payload, this);
    play = bokoGameInfo();
    BOOL crossed = bokoLineCross(&play->play.mBgS, &line.payload);
    u32 flags = u32(bokoWord(this->mAcch.m_flags));
    if (crossed) {
      f32 lower = bokoGroundProbeDepth + 60.0f;
      this->shape_angle.x = 0x1000;
      this->current.pos.y = f32(this->current.pos.y) - lower;
    }
    if (!(flags & 0x20))
      this->procMove_init();
    this->setRoomInfo();
    this->setBaseMatrix();
    line.payload.dBgS_PolyPassChk::__vtbl = BOKO_LINE_POLY_PASS_VTABLE;
    line.payload.dBgS_GrpPassChk::__vtbl = BOKO_GROUP_DESTRUCTOR_VTABLE;
    line.payload.cBgS_PolyInfo::__vtbl = BOKO_LINE_POLY_DESTRUCTOR_VTABLE;
    bokoLineDestruct(&line.payload, 0);
  }
  ground.payload.cBgS_PolyInfo::__vtbl = BOKO_GROUND_POLY_INFO_VTABLE;
  ground.payload.dBgS_PolyPassChk::__vtbl = BOKO_GROUND_POLY_PASS_VTABLE;
  ground.payload.dBgS_GrpPassChk::__vtbl = BOKO_GROUP_DESTRUCTOR_VTABLE;
  bokoCheckDestruct(&ground.payload, 0);
  return 1;
}

/* 020C2624 */
BOOL daBoko_c::procMove_init() {
  this->mProcDelta = 0;
  this->mNowMode = Mode_MOVE_e;
  this->mCurrentProc = 0x020C2C90;
  this->mProcSlot = -1;
  bokoStartCarry(this, 0);
  bokoWord(this->attention_info.flags) = u32(bokoWord(this->attention_info.flags)) & ~0x10u;
  s16 count = bokoCarryHideTimer;
  this->mbFloorFlg = 0;
  this->m2BB = u8(s32(count) + 5);
  return 1;
}

/* 020C2C90 */
BOOL daBoko_c::procMove() {
  bool bounced = false;
  this->mChainTimer = 20;
  bokoMovePosition(this, 0);
  f32 previousRootY = this->m2DC.y, previousTopY = this->m2D0.y;
  this->setTopRootPos(1);
  dBgS_GndChk* ground = &m_ground_check;
  m_ground_check.m_pos.x = f32(this->m2DC.x);
  m_ground_check.m_pos.y = previousRootY + 50.0f;
  m_ground_check.m_pos.z = f32(this->m2DC.z);
  auto* play = bokoGameInfo();
  f32 rootFloor = bokoGroundCross(&play->play.mBgS, ground);
  if (rootFloor == -1000000000.0f)
    rootFloor = f32(this->mAcch.m_ground_h);
  m_ground_check.m_pos.x = f32(this->m2D0.x);
  m_ground_check.m_pos.y = previousTopY + 50.0f;
  f32 rootGap = f32(this->m2DC.y) - rootFloor;
  m_ground_check.m_pos.z = f32(this->m2D0.z);
  play = bokoGameInfo();
  f32 topFloor = bokoGroundCross(&play->play.mBgS, ground);
  if (topFloor == -1000000000.0f)
    topFloor = f32(this->mAcch.m_ground_h);
  f32 topGap = f32(this->m2D0.y) - topFloor, upLimit = 20.0f,
      damping = 0.7f, uintThreshold = 2147483648.0f;
  auto volume = [uintThreshold](f32 value) {
    u32 result;
    if (value < uintThreshold)
      result = u32(gabi::ftoi(value));
    else
      result = u32(gabi::ftoi(value - uintThreshold)) + 0x80000000u;
    return result > 100 ? 100u : result;
  };
  if ((rootGap < topGap && rootGap < bokoGroundGap) ||
      (topGap < rootGap && topGap < bokoGroundGap)) {
    f32 vertical = this->speed.y;
    if (vertical < -10.0f) {
      vertical *= -0.5f;
      this->speed.y = vertical > upLimit ? upLimit : vertical;
    }
    if (this->m2BB) {
      f32 selected = (rootGap - topGap) >= 0 ? topGap : rootGap;
      u8 count = this->m2BB;
      this->m2BB = u8(count - 1);
      this->current.pos.y = f32(this->current.pos.y) - selected;
    }
    BokoVectorFrame direction;
    direction.payload.x = f32(this->m2D0.x) - f32(this->m2DC.x);
    direction.payload.y = 0.0f;
    direction.payload.z = f32(this->m2D0.z) - f32(this->m2DC.z);
    f32 difference = topFloor - rootFloor;
    f32 square = bokoVectorSquareMagnitude(&direction.payload),
        length = bokoSqrt(square);
    s32 desired = bokoAtan2(-difference, length);
    s16 angleDifference = s16(u32(s32(s16(this->shape_angle.x))) - u32(desired));
    f32 correction = bokoGroundCorrection + 0.15f;
    s16 angular = s16(gabi::ftoi(-(f32(s32(angleDifference)) * correction)));
    if (angular)
      this->m2C0 = angular;
    else {
      this->m2C0 = angleDifference ? s16(-angleDifference) : 0;
      if (!this->m2C0 &&
          __builtin_fabsf(f32(this->speedF)) < 1.0f) {
        this->m2C0 = s16(-angleDifference);
        u8 count = this->m2BB;
        this->m2BB = count > 2 ? u8(count - 2) : 0;
        this->m2C0 = this->shape_angle.x > 0 ? -90 : 90;
      }
    }
    u32 loudness = volume(f32(this->speed.y) * 8.0f),
        type = this->mParameters, sound = m_bound_se[type];
    s32 reverb = bokoReverb(s32(s8(this->current.roomNo)));
    bokoStartSound(sound, &this->eyePos, loudness, reverb);
    s16 rotation = this->m2C2;
    this->speedF = f32(this->speedF) * damping;
    f32 rate = bokoMoveDamping + damping;
    this->m2C2 = s16(gabi::ftoi(f32(s32(rotation)) * rate));
    bounced = true;
  }
  s32 rotation = this->m2C2, angular = this->m2C0;
  auto magnitude = [](s32 value) { return value < 0 ? -value : value; };
  s16 pitch =
      s16(s32(s16(this->shape_angle.x)) +
          (magnitude(rotation) > magnitude(angular) ? rotation : angular));
  this->shape_angle.x = pitch;
  if (!rotation) {
    if (pitch > 0x3A00)
      this->shape_angle.x = 0x3A00;
    else if (pitch < -0x3A00)
      this->shape_angle.x = -0x3A00;
  }
  if (this->m2C0) {
    s32 roll = this->shape_angle.z;
    bokoApproachAngle(&this->shape_angle.z,
                     magnitude(roll) > 0x4000 ? -0x8000 : 0, 5, 0x1000, 0x100);
  }
  f32 vertical = this->speed.y;
  play = bokoGameInfo();
  bokoCorrectPosition(&this->mAcch, &play->play.mBgS);
  this->setRoomInfo();
  if ((u32(bokoWord(this->mAcch.m_flags)) & 0x20) && !(vertical > 2.5f)) {
    play = bokoGameInfo();
    BOOL moving =
        bokoCheckMovingBackground(&play->play.mBgS, static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd));
    f32 bounce = 15.0f;
    bool treasure = false;
    if (moving) {
      play = bokoGameInfo();
      auto* other = bokoBackgroundActor(&play->play.mBgS,
                                  u16(this->mAcch.m_gnd.mBgIndex));
      treasure = other && s16(other->mProcName) == 0x124;
    }
    if (treasure) {
      if (f32(this->speedF) < bounce)
        this->speedF = bounce;
      this->speed.y = upLimit;
    } else {
      play = bokoGameInfo();
      u16 poly = u16(this->mAcch.m_gnd.mPolyIndex), object = u16(this->mAcch.m_gnd.mBgIndex);
      auto* plane = bokoTrianglePlane(&play->play.mBgS, object, poly);
      s32 desired = 0;
      s16 difference;
      if (plane) {
        s32 yaw =
            bokoAtan2(f32(plane->mNormal.x), f32(plane->mNormal.z));
        u16 delta = u16(u32(yaw) - u32(s32(s16(this->shape_angle.y))));
        f32 z = f32(plane->mNormal.z), x = f32(plane->mNormal.x),
            cosine = cM_scos(delta);
        f32 square = gabi::fmadds(x, x, z * z),
            length = bokoSqrt(square);
        desired =
            bokoAtan2(length * cosine, f32(plane->mNormal.y));
        difference = s16(u32(desired) - u32(s32(s16(this->shape_angle.x))));
      } else
        difference = s16(-s32(s16(this->shape_angle.x)));
      s32 threshold = s32(bokoSlopeAngle) + 0x800;
      if (magnitude(difference) > threshold && this->m2BB) {
        f32 speed = this->speedF;
        u8 count = this->m2BB;
        this->speed.y = bounce;
        if (speed < bounce)
          this->speedF = bounce;
        this->m2C2 = 0;
        this->m2C0 = s16(-(s32(difference) >> 2));
        this->m2BB = count > 4 ? u8(count - 4) : 0;
      } else {
        this->shape_angle.x = s16(desired);
        this->shape_angle.z = 0;
        this->procWait_init();
      }
    }
    if (!bounced) {
      f32 difference =
              __builtin_fabsf(f32(this->old.pos.y) - f32(this->current.pos.y)),
          loudnessF = difference * 8.0f;
      u32 loudness = volume(loudnessF), type = this->mParameters,
          sound = m_bound_se[type];
      s32 reverb = bokoReverb(s32(s8(this->current.roomNo)));
      bokoStartSound(sound, &this->eyePos, loudness, reverb);
      this->speedF = f32(this->speedF) * damping;
    }
  } else
    this->speed.y = vertical;
  this->setBaseMatrix();
  return 1;
}

/* 020C2978 */
BOOL daBoko_c::procThrow() {
  BokoVectorFrame previous;
  previous.payload.x = this->m2DC.x;
  previous.payload.y = this->m2DC.y;
  previous.payload.z = this->m2DC.z;
  this->mChainTimer = 20;
  bokoMovePosition(this, 0);
  f32 zero = 0.0f;
  BokoVectorFrame horizontal;
  horizontal.payload.x = this->speed.x;
  horizontal.payload.y = zero;
  horizontal.payload.z = this->speed.z;
  f32 square = bokoVectorSquareMagnitude(&horizontal.payload),
      length = bokoSqrt(square);
  s32 pitch = bokoAtan2(-f32(this->speed.y), length);
  this->shape_angle.x = s16(pitch);
  s32 yaw = bokoAtan2(f32(this->speed.x), f32(this->speed.z));
  s16 timer = this->m2C8;
  this->shape_angle.y = s16(yaw);
  this->current.angle.y = s16(yaw);
  if (timer > 0) {
    timer = s16(timer - 1);
    this->m2C8 = timer;
    if (!timer)
      this->gravity = -3.0f;
  }
  auto* play = bokoGameInfo();
  bokoCorrectPosition(&this->mAcch, &play->play.mBgS);
  this->setRoomInfo();
  this->setBaseMatrix();
  this->setTopRootPos(0);
  bool collision = false;
  s16 direction = 0;
  BOOL hit = bokoAttackHit(&this->mCps);
  u32 flags = u32(bokoWord(this->mAcch.m_flags));
  if (hit || (flags & 0x30) || (flags & 0x200)) {
    collision = true;
    if ((flags & 0x200) && f32(this->speed.y) > zero)
      this->speed.y = f32(this->speed.y) * -0.5f;
    if (bokoAttackHit(&this->mCps)) {
      f32 random = bokoRandom(16384.0f);
      direction = s16(gabi::ftoi(random + f32(s32(s16(this->current.angle.y)))));
    } else if (u32(bokoWord(this->mAcch.m_flags)) & 0x10)
      direction = s16(this->mAcchCir.m_wall_angle_y);
    else
      direction = this->current.angle.y;
  }
  bokoSetCapsuleEnds(&this->mCps.mStart, &previous.payload, &this->m2D0);
  play = bokoGameInfo();
  bokoRegisterCollision(&play->play.mCcS, &this->mCps);
  if (!collision) {
    dBgS_LinChk* line = &m_line_check;
    bokoSetLine(line, &this->old.pos, &this->m2D0, this);
    play = bokoGameInfo();
    if (bokoLineCross(&play->play.mBgS, line)) {
      play = bokoGameInfo();
      u16 poly = u16(m_line_check.mPolyIndex), object = u16(m_line_check.mBgIndex);
      auto* plane = bokoTrianglePlane(&play->play.mBgS, object, poly);
      if (plane && f32(plane->mNormal.y) < 0.5f &&
          !(f32(plane->mNormal.y) < -0.8f))
        direction = s16(
            bokoAtan2(f32(plane->mNormal.x), f32(plane->mNormal.z)));
      else
        direction = this->current.angle.y;
      collision = true;
    }
  }
  if (collision)
    this->setThrowReverse(direction);
  return 1;
}

/* 020C3480 */
BOOL daBoko_c::procCarry() {
  u32 flags = this->actor_status;
  this->mChainTimer = 20;
  if ((flags & 0x2000) || !this->mbThrow) {
    J3DModel* model = this->mpModel;
    Mtx34* matrix = model->getBaseTRMtx();
    this->current.pos.x = f32(matrix->m[0][3]);
    this->current.pos.y = f32(matrix->m[1][3]);
    model = this->mpModel;
    this->current.pos.z = f32(matrix->m[2][3]);
    bokoMatrixToRotation(model->getBaseTRMtx(),
                     &this->shape_angle);
  }
  auto* play = bokoGameInfo();
  fopAc_ac_c* player = play->play.mpPlayerPtr[0];
  f32 five = 5.0f;
  BokoVectorFrame playerPosition;
  playerPosition.payload.x = f32(player->current.pos.x);
  playerPosition.payload.y = f32(player->current.pos.y) + five;
  playerPosition.payload.z = f32(player->current.pos.z);
  if (u32(bokoWord(this->actor_status)) & 0x2000)
    return 1;
  bokoStartCarry(this, 0);
  this->setTopRootPos(1);
  f32 zero = 0.0f;
  dBgS_LinChk* line = &m_line_check;
  if (this->mbThrow) {
    u16 angle = u16(this->m2CA);
    this->gravity = zero;
    this->mProcDelta = 0;
    this->mNowMode = Mode_THROW_e;
    this->shape_angle.x = s16(angle);
    this->mProcSlot = -1;
    this->mCurrentProc = 0x020C2978;
    s16 yaw = s16(player->shape_angle.y);
    this->current.angle.y = yaw;
    this->shape_angle.y = yaw;
    this->speedF = 70.0f * cM_scos(angle);
    this->mbThrow = 0;
    this->speed.y = -70.0f * cM_ssin(angle);
    this->m2C8 = 2;
    bokoResetAttackHit(&this->mCps);
    bokoSetLine(line, &playerPosition.payload,
                     &this->current.pos, this);
    play = bokoGameInfo();
    if (bokoLineCross(&play->play.mBgS, line)) {
      bokoWord(this->current.pos.x) = u32(bokoWord(m_line_check.mLin.mEnd.x));
      bokoWord(this->current.pos.y) = u32(bokoWord(m_line_check.mLin.mEnd.y));
      bokoWord(this->current.pos.z) = u32(bokoWord(m_line_check.mLin.mEnd.z));
      play = bokoGameInfo();
      u16 poly = u16(m_line_check.mPolyIndex), object = u16(m_line_check.mBgIndex);
      auto* plane = bokoTrianglePlane(&play->play.mBgS, object, poly);
      if (!plane)
        return 1;
      BokoVectorFrame offset;
      bokoVectorMultiply(plane, &offset.payload,
                       10.0f);
      bokoAddVector(&this->current.pos, &offset.payload,
                       &this->current.pos);
      s32 angle =
          bokoAtan2(f32(plane->mNormal.x), f32(plane->mNormal.z));
      this->setThrowReverse(s16(angle));
    } else
      this->procThrow();
    return 1;
  }
  if (this->mbFloorFlg) {
    if (this->shape_angle.z >= 0x4000) {
      s16 pitch = this->shape_angle.x, yaw = this->shape_angle.y;
      this->shape_angle.z = 0;
      this->shape_angle.x = s16(u16(pitch) - 0x8000);
      this->shape_angle.y = s16(u16(yaw) - 0x8000);
    }
    this->procWait_init();
    return 1;
  }
  this->current.angle.y = s16(u16(this->shape_angle.y) + 0x8000);
  if (this->mNowMode == Mode_PLAYER_CARRY_e) {
    bokoSetLine(line, &playerPosition.payload,
                     &this->current.pos, this);
    play = bokoGameInfo();
    if (bokoLineCross(&play->play.mBgS, line)) {
      bokoWord(this->current.pos.y) = u32(bokoWord(playerPosition.payload.y));
      bokoWord(this->current.pos.z) = u32(bokoWord(playerPosition.payload.z));
      bokoWord(this->current.pos.x) = u32(bokoWord(playerPosition.payload.x));
      BokoVectorFrame offset;
      offset.payload.x = zero;
      offset.payload.y = 40.0f;
      offset.payload.z = -40.0f;
      bokoRotateYSet(bokoMatrixPointer.get(),
                       s16(player->shape_angle.y));
      bokoTransformStackVector(&offset.payload, &offset.payload);
      bokoAddVector(&this->current.pos, &offset.payload,
                       &this->current.pos);
      this->speed.y = zero;
      this->shape_angle.z = 0;
      this->shape_angle.x = -0x2000;
    }
  }
  this->procMove_init();
  f32 distance = 50.0f, invalid = -1000000000.0f;
  dBgS_GndChk* ground = &m_ground_check;
  f32 originalY = distance;
  bool found = false;
  for (u32 attempt = 0; attempt < 3; attempt++) {
    m_ground_check.m_pos.x = f32(this->current.pos.x);
    m_ground_check.m_pos.z = f32(this->current.pos.z);
    m_ground_check.m_pos.y = f32(this->current.pos.y);
    play = bokoGameInfo();
    f32 height = bokoGroundCross(&play->play.mBgS, ground);
    if (height != invalid) {
      originalY = this->current.pos.y;
      found = true;
      break;
    }
    this->current.pos.x =
        gabi::fnmsubs(distance, cM_ssin(this->shape_angle.y), f32(this->current.pos.x));
    this->current.pos.z =
        gabi::fnmsubs(distance, cM_scos(this->shape_angle.y), f32(this->current.pos.z));
  }
  if (!found) {
    bokoWord(this->current.pos.x) = u32(bokoWord(playerPosition.payload.x));
    bokoWord(this->current.pos.z) = u32(bokoWord(playerPosition.payload.z));
  }
  f32 x = this->current.pos.x, z = this->current.pos.z;
  this->old.pos.y = originalY;
  this->gravity = -3.0f;
  this->old.pos.x = x;
  this->old.pos.z = z;
  this->m2C0 = 0;
  this->current.pos.y = originalY - five;
  bokoCallMove(this);
  return 1;
}

/* 020C183C */
BOOL daBoko_c::execute() {
  s16 delay = s16(this->mSpawnDelay);
  if (delay) {
    this->mSpawnDelay = s16(delay - 1);
    return 1;
  }
  if (s16(this->mProcSlot))
    (this->*bokoCurrentProcedure(*this))();
  u32 y = u32(bokoWord(this->current.pos.y)), z = u32(bokoWord(this->current.pos.z)),
      x = u32(bokoWord(this->current.pos.x));
  bokoWord(this->eyePos.y) = y;
  bokoWord(this->attention_info.position.z) = z;
  bokoWord(this->eyePos.x) = x;
  bokoWord(this->eyePos.z) = z;
  bokoWord(this->attention_info.position.x) = x;
  bokoWord(this->attention_info.position.y) = y;
  bool extinguish = false;
  if (bokoTargetHit(&this->mSph)) {
    auto* hit = bokoTargetHitObject(&this->mSph);
    s16 timer = this->mFlameTimer;
    if (hit && timer >= 0) {
      u32 flags = u32(hit->mAtType);
      if (timer > 0) {
        if (flags & 0x100) {
          this->mFlameTimer = 1;
          extinguish = true;
        }
      } else if (flags & 0x20200) {
        s32 reverb = bokoReverb(s32(s8(this->current.roomNo)));
        bokoStartSound(0x6902, &this->m2D0, 0, reverb);
        this->cullType = 21;
        this->mFlameTimer = 900;
      }
    }
  }
  bokoModelCalculate(this->mpModel.get());
  this->setTopRootPos(0);
  f32 zero = 0.0f;
  if (this->mFlameTimer) {
    if (this->mNowMode != Mode_ENEMY_CARRY_e)
      this->mFlameTimer = s16(s16(this->mFlameTimer) - 1);
    bool wet = extinguish;
    if (!wet) {
      BokoFloatFrame height;
      if (bokoWaterHeight(&this->m2D0, &height.payload))
        wet = !(f32(height.payload) < f32(this->m2D0.y));
    }
    bool stopped = wet || !this->mFlameTimer;
    if (wet) {
      if (this->mFlameTimer) {
        auto* play = bokoGameInfo();
        bokoSetParticle(play->play.mParticle.get(), 0, 0x35A,
                         &this->m2D0, 0, 0, 255, 0, -1, 0, 0, 0);
      }
      this->m7E4 = zero;
      this->cullType = 14;
      this->mFlameTimer = 0;
    } else if (this->mFlameTimer)
      this->setFlameEffect();
    else {
      this->cullType = 14;
      this->m7E4 = zero;
    }
    if (stopped && u32(bokoWord(this->mParticleCallBack.mpEmitter.v))) {
      s32 reverb = bokoReverb(s32(s8(this->current.roomNo)));
      bokoStartSound(0x6904, &this->m2D0, 0, reverb);
      bokoEndFlame(&this->mParticleCallBack);
    }
    u32 topY = u32(bokoWord(this->m2D0.y)), topZ = u32(bokoWord(this->m2D0.z)),
        topX = u32(bokoWord(this->m2D0.x));
    bokoWord(this->mLight.mPos.z) = topZ;
    bokoWord(this->mLight.mPos.x) = topX;
    f32 factor = 1.0f;
    bokoWord(this->mLight.mPos.y) = topY;
    if (dComIfGp_getStageStagInfo()) {
      void* info = dComIfGp_getStageStagInfo();
      u32 kind = (u32(static_cast<BokoStageInfo*>(info)->mFlags) >> 16) & 7;
      if (kind == 1 || kind == 4)
        factor = 0.65217394f;
    }
    this->mLight.mPower = (150.0f * factor) * f32(this->m7E4);
  } else {
    bokoResetTargetHit(&this->mSph);
    this->mLight.mPower = zero;
  }
  if (this->mParameters == Type_BOKO_STICK_e) {
    u32 flags = u32(this->mSph.mObjAt.mSPrm);
    this->mSph.mObjAt.mSPrm = this->mFlameTimer > 0 ? flags | 1 : flags & ~1u;
    bokoSetSphereCenter(&this->mSph.mSph, &this->m2D0);
    auto* play = bokoGameInfo();
    bokoRegisterCollision(&play->play.mCcS, &this->mSph);
    u8 mode = this->mNowMode;
    play = bokoGameInfo();
    bokoSetCollisionMass(bokoCollisionMass(play), &this->mSph,
                     mode == Mode_PLAYER_CARRY_e ? 1 : 4);
  }
  if (this->mpLineKe.get())
    this->keCalc();
  if (this->mParameters == Type_PGANON_SWORD_e) {
    auto* save = bokoGameInfoPointer.get();
    if (bokoCheckEventBit(&save->save.mSavedata.mEvent, 0x3A08)) {
      bokoPlayAnimation(&this->mBrkAnm);
      if ((u8(this->mBrkAnm.mFrameCtrl.mState) & 1) || f32(this->mBrkAnm.mFrameCtrl.mRate) == zero)
        bokoRequestDelete(this);
    }
  }
  return 1;
}

/* 020C1D0C */
BOOL daBoko_Execute(daBoko_c *a) {
  return a->execute();
}

/* 020C1D10 */
BOOL daBoko_IsDelete(daBoko_c *a) {
  return 1;
}

/* 020C1D18 */
BOOL daBoko_c::bokoDelete() {
  bokoDeleteSound(&this->m2D0);
  this->mParticleCallBack.remove();
  bokoCutLight(&this->mLight);
  u32 type = this->mParameters;
  char* name = m_arc_name[type].get();
  bokoDeleteResource(&this->mPhase, name);
  return 1;
}

/* 020C1D84 */
BOOL daBoko_Delete(daBoko_c *a) {
  a->bokoDelete();
  return 1;
}

/* 020C1F54 */
BOOL daBoko_createHeap(daBoko_c *a) {
  return a->createHeap();
}

/* 020C1DA8 */
BOOL daBoko_c::createHeap() {
  u32 type = this->mParameters;
  if (type >= Type_COUNT_e)
    bokoAssert(bokoAssertText(BOKO_SOURCE_FILE), 0x84E, bokoAssertText(BOKO_TYPE_ASSERT));
  BokoNameFrame name;
  void* controller = bokoResourceControl.get();
  u32
      index = model_idx[type],
      text = gabi::ea(m_arc_name[type].get());
  name.payload.vtable = BOKO_SAFE_STRING_VTABLE;
  name.payload.string = text;
  void *data =
      bokoGetResource(controller, &name.payload, index);
  if (!data)
    bokoAssert(bokoAssertText(BOKO_SOURCE_FILE), 0x854, bokoAssertText(BOKO_MODEL_ASSERT));
  void *model = bokoCreateModel(data, 0x80000, 0x11000002);
  this->mpModel = (J3DModel*)model;
  if (!model)
    return 0;
  if (type == Type_MOBLIN_SPEAR_e) {
    void *line = bokoAllocate(0x138C);
    if (line)
      bokoLineMaterialConstruct(line);
    this->mpLineKe = static_cast<LineKe*>(line);
    if (!line)
      return 0;
    if (!bokoLineMaterialInitialize(line, 16, 10, 0))
      return 0;
    f32 rate = bokoChainDamping + 0.7f;
    this->mpLineKe->mDamping = rate;
    return 1;
  }
  if (type == Type_PGANON_SWORD_e) {
    BokoNameFrame animation;
    animation.payload.vtable = BOKO_SAFE_STRING_VTABLE;
    controller = bokoResourceControl.get();
    animation.payload.string = BOKO_BRK_ARCHIVE_NAME;
    void *resource =
        bokoGetResource(controller, &animation.payload, 7);
    f32 speed = 1.0f;
    // HD: keep the verified GHS BRK argument order and floating register.
    if (!bokoInitializeBrk(&this->mBrkAnm, data, resource,
                          1, 0, 0, -1, 0, 0, speed))
      return 0;
  }
  return 1;
}

/* 020C2188 */
s32 daBoko_c::create() {
  u32 actor = gabi::ea(this), condition = u32(bokoWord(this->actor_condition));
  if (!(condition & 8)) {
    if (gabi::ea(this) != 0) {
      daBoko_c__ct(this);
      condition = u32(bokoWord(this->actor_condition));
    }
    bokoWord(this->actor_condition) = condition | 8;
  }
  u32 type = this->mParameters, offset;
  char* name;
  if (type == Type_UNK_7_e) {
    this->mParameters = Type_BOKO_STICK_e;
    this->cullType = 21;
    this->mFlameTimer = 900;
    type = Type_BOKO_STICK_e;
    offset = 0;
    name = m_arc_name[0].get();
  } else {
    if (type == Type_LIT_STICK_e) {
      this->mParameters = Type_BOKO_STICK_e;
      s16 delay = bokoLitSpawnDelay;
      this->mSpawnDelay = s16(delay + 10);
      f32 speed = bokoLitSpawnSpeed + -10.0f;
      type = Type_BOKO_STICK_e;
      this->speedF = speed;
    }
    offset = type * 4;
    name = m_arc_name[offset / 4].get();
  }
  s32 phase = bokoLoadResource(&this->mPhase, name);
  if (phase != cPhs_COMPLEATE_e)
    return phase;
  if (type >= Type_COUNT_e ||
      !bokoCreateSolidHeap(this, daBoko_createHeap,
                        u32(m_heap_size[offset / 4])) ||
      !this->mpModel.get())
    return cPhs_ERROR_e;
  J3DModel* model = this->mpModel;
  f32 y = this->scale.y, x = this->scale.x, z = this->scale.z;
  model->mBaseScale.x = x;
  model->mBaseScale.y = y;
  model->mBaseScale.z = z;
  model = this->mpModel;
  this->cullMtx = gabi::ea(model->getBaseTRMtx());
  offset = type * 12;
  x = f32(m_cull_min[offset / 12].x);
  y = f32(m_cull_min[offset / 12].y);
  z = f32(m_cull_min[offset / 12].z);
  bokoCullMinimum(this, x, y, z);
  x = f32(m_cull_max[offset / 12].x);
  y = f32(m_cull_max[offset / 12].y);
  z = f32(m_cull_max[offset / 12].z);
  bokoCullMaximum(this, x, y, z);
  f32 height = 10.0f, radius = 60.0f;
  bokoSetCircleWall(&this->mAcchCir, height, radius);
  bokoSetAcch(&this->mAcch, &this->current.pos,
                   &this->old.pos, this, 1, &this->mAcchCir,
                   &this->speed, &this->current.angle, &this->shape_angle);
  u32 flags = u32(bokoWord(this->attention_info.flags));
  this->maxFallSpeed = -100.0f;
  this->attention_info.distances[4] = 17;
  bokoWord(this->attention_info.flags) = flags | 0x10;
  this->gravity = -3.0f;
  bokoInitializeStatus(&this->mStts, 10, 255, this);
  if (type == Type_BOKO_STICK_e) {
    model = this->mpModel;
    J3DJoint* joint = this->mpModel->getModelData()->getJointNodePointer(2);
    auto* material = reinterpret_cast<BokoJointNodeHD*>(joint)->mMaterial.get();
    material->getShape()->hide();
  }
  bokoSetSphere(&this->mSph, &l_sph_src);
  bokoWord(this->mSph.mStts.v) = actor + 0x6C4;
  bokoSetCapsule(&this->mCps, &l_cps_src);
  u32 currentType = this->mParameters, table = currentType * 4;
  bokoWord(this->mCps.mStts.v) = actor + 0x6C4;
  u32 attack = m_at_type[table / 4];
  bokoWord(this->mCps.mAtType) = attack;
  this->mCps.mAtp = u8(u32(m_at_point[table / 4]));
  this->mCps.mGObjAt.mSe = m_se_type[currentType];
  f32 capsuleRadius = m_cps_r[table / 4];
  this->mProcDelta = 0;
  this->mProcSlot = -1;
  this->mCurrentProc = 0x020C3964;
  this->mCps.mRadius = capsuleRadius;
  this->setBaseMatrix();
  this->setTopRootPos(0);
  f32 scale = 1.0f;
  if (this->mFlameTimer) {
    this->m7E4 = scale;
    bokoCallFlame(this);
  }
  bokoWord(this->mAcch.m_flags) = u32(bokoWord(this->mAcch.m_flags)) & ~8u;
  this->mAcch.m_roof_crr_height = 30.0f;
  bokoFlameFlags = bokoFlameFlags & ~0x20000000u;
  bokoModelCalculate(this->mpModel.get());
  if (type == Type_BOKO_STICK_e) {
    bokoSetLight(&this->mLight);
    this->mLight.mColorR = 600;
    this->mLight.mColorG = 400;
    this->mLight.mColorB = 120;
    void* stage = dComIfGp_getStageStagInfo();
    if (stage) {
      stage = dComIfGp_getStageStagInfo();
      u32 kind = (u32(static_cast<BokoStageInfo*>(stage)->mFlags) >> 16) & 7;
      if (kind == 1 || kind == 4)
        scale = 0.65217394f;
    }
    f32 power = 150.0f * scale, rate = f32(this->m7E4),
        distance = 250.0f;
    this->mLight.mFluctuation = distance;
    this->mLight.mPower = power * rate;
  }
  u32 py = u32(bokoWord(this->current.pos.y)), modelPointer = u32(bokoWord(this->mpModel.v));
  bokoWord(this->m2F4.y) = py;
  bokoWord(this->model) = modelPointer;
  u32 px = u32(bokoWord(this->current.pos.x)), pz = u32(bokoWord(this->current.pos.z));
  bokoWord(this->m2F4.x) = px;
  bokoWord(this->m2F4.z) = pz;
  return phase;
}

/* 020C25A4 */
s32 daBoko_Create(daBoko_c *a) {
  return a->create();
}

// HD: compiler-generated and template code of this translation unit.

/* 020C1F58 */
daBoko_c *daBoko_c__ct(daBoko_c *a) {
  if (!a) {
    a = static_cast<daBoko_c*>(bokoAllocate(sizeof(daBoko_c)));
    if (!a)
      return a;
  }
  bokoActorConstruct(a);
  bokoWord(a->__vtbl) = BOKO_ACTOR_VTABLE;
  bokoBrkConstruct(&a->mBrkAnm);
  a->mLight.mHD20 = 1.0f;
  bokoFlameCallbackConstruct(&a->mParticleCallBack, 0, 0);
  bokoAcchConstruct(&a->mAcch);
  bokoWord(a->mAcch.dBgS_GrpPassChk::__vtbl) = BOKO_ACCH_GROUP_VTABLE;
  bokoWord(a->mAcch.cBgS_Chk::__vtbl) = BOKO_ACCH_CHECK_VTABLE;
  a->mAcch.mbObjThrough = 1;
  bokoWord(a->mAcch.dBgS_PolyPassChk::__vtbl) = BOKO_ACCH_POLY_VTABLE;
  bokoCircleConstruct(&a->mAcchCir);
  bokoStatusConstruct(&a->mStts);
  bokoGlobalStatusConstruct(&a->mStts.__vtbl_gstts);
  a->mStts.__vtbl = BOKO_STATUS_VTABLE;
  a->mStts.__vtbl_gstts = BOKO_GLOBAL_STATUS_VTABLE;
  bokoSphereConstruct(&a->mSph);
  bokoObjectInfoConstruct(&a->mCps);
  a->mCps.mShapeVtable = BOKO_SHAPE_BASE_VTABLE;
  a->mCps.mAabVtable = BOKO_CAPSULE_AAB_VTABLE;
  bokoCapsuleConstruct(&a->mCps.mStart);
  bokoWord(a->mCps.__vtbl_hitinf) = BOKO_CAPSULE_HIT_VTABLE;
  a->mCps.mGeometryVtable = BOKO_CAPSULE_GEOMETRY_VTABLE;
  a->mCps.mShapeVtable = BOKO_CAPSULE_SHAPE_VTABLE;
  return a;
}

/* 020C3D0C */
void __sinit_d_a_boko() {
  bokoRegistration[1] = 0;
  bokoRegistration[3] = 0;
  bokoRegistration[0] = 0;
  bokoRegistration[2] = 0;
  bokoRegisterGlobal(&bokoGlobalRecord0);
  f32 upper = 3.1415927f, lower = -3.1415927f;
  bokoAngleMax = upper;
  bokoAngleMin = lower;
  bokoInitializeAngles(&bokoAngleStorage);
  bokoRegisterGlobal(&bokoGlobalRecord1);
  bokoInitializeFlags(&bokoFlagStorage);
  bokoRegisterGlobal(&bokoGlobalRecord2);
  f32 zero = 0.0f, distance = 30.0f;
  l_break_particle_offset.x = zero;
  l_break_particle_offset.z = distance;
  l_break_particle_offset.y = zero;
  dBgS_GndChk* ground = &m_ground_check;
  bokoGroundConstruct(ground);
  m_ground_check.cBgS_Chk::mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(ground));
  m_ground_check.cBgS_Chk::mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(ground));
  m_ground_check.mGrp = 1;
  m_ground_check.mbCamThrough = 0;
  m_ground_check.cBgS_Chk::__vtbl = BOKO_OBJECT_GROUND_CHECK_VTABLE;
  m_ground_check.mbLinkThrough = 0;
  m_ground_check.mbArrowThrough = 0;
  m_ground_check.mbBombThrough = 0;
  m_ground_check.mbBoomerangThrough = 0;
  m_ground_check.cBgS_PolyInfo::__vtbl = BOKO_OBJECT_GROUND_POLY_INFO_VTABLE;
  m_ground_check.mbRopeThrough = 0;
  m_ground_check.dBgS_GrpPassChk::__vtbl = BOKO_OBJECT_GROUND_GROUP_VTABLE;
  m_ground_check.dBgS_PolyPassChk::__vtbl = BOKO_OBJECT_GROUND_POLY_PASS_VTABLE;
  m_ground_check.mbObjThrough = 1;
  bokoRegisterGlobal(&bokoGlobalRecord3);
  dBgS_LinChk* line = &m_line_check;
  bokoLineConstruct(line);
  m_line_check.mbCamThrough = 0;
  m_line_check.mbLinkThrough = 0;
  m_line_check.mbArrowThrough = 0;
  m_line_check.cBgS_Chk::mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(line));
  m_line_check.mbBombThrough = 0;
  m_line_check.mbBoomerangThrough = 0;
  m_line_check.mbRopeThrough = 0;
  m_line_check.cBgS_Chk::mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(line));
  m_line_check.cBgS_Chk::__vtbl = BOKO_OBJECT_LINE_CHECK_VTABLE;
  m_line_check.cBgS_PolyInfo::__vtbl = BOKO_OBJECT_LINE_POLY_INFO_VTABLE;
  m_line_check.mGrp = 1;
  m_line_check.mbObjThrough = 1;
  m_line_check.dBgS_GrpPassChk::__vtbl = BOKO_OBJECT_LINE_GROUP_VTABLE;
  m_line_check.dBgS_PolyPassChk::__vtbl = BOKO_OBJECT_LINE_POLY_PASS_VTABLE;
  bokoRegisterGlobal(&bokoGlobalRecord4);
}

/* 020C3ED4 */
void bokoGlobalDeletingDestructor(void *p, s32 flags) {
  if (p && (flags & 1))
    bokoFree(p);
}

/* 020C3EE8 */
void dBgS_GndChk__dt_boko(dBgS_GndChk *object, s32 flags) {
  if (!object)
    return;
  (*object).cBgS_PolyInfo::__vtbl = BOKO_GROUND_POLY_INFO_VTABLE;
  (*object).dBgS_PolyPassChk::__vtbl = BOKO_GROUND_POLY_PASS_VTABLE;
  (*object).dBgS_GrpPassChk::__vtbl = BOKO_GROUP_DESTRUCTOR_VTABLE;
  bokoCheckDestruct(object, 0);
  if (flags & 1)
    bokoFree(object);
}

/* 020C3F60 */
void dBgS_LinChk__dt_boko(dBgS_LinChk *object, s32 flags) {
  if (!object)
    return;
  (*object).dBgS_PolyPassChk::__vtbl = BOKO_LINE_POLY_PASS_VTABLE;
  (*object).dBgS_GrpPassChk::__vtbl = BOKO_GROUP_DESTRUCTOR_VTABLE;
  (*object).cBgS_PolyInfo::__vtbl = BOKO_LINE_POLY_DESTRUCTOR_VTABLE;
  bokoLineDestruct(object, 0);
  if (flags & 1)
    bokoFree(object);
}

/* 020C3FD8 */
void daBoko_c__dt(daBoko_c *a, s32 flags) {
  if (!a)
    return;
  bokoObjectInfoDestruct(&a->mCps, 2);
  bokoSphereDestruct(&a->mSph, 2);
  bokoStatusDestruct(&a->mStts, 2);
  bokoCircleDestruct(&a->mAcchCir.m_cir, 2);
  bokoWord(a->mAcch.dBgS_GrpPassChk::__vtbl) = BOKO_ACCH_GROUP_VTABLE;
  bokoWord(a->mAcch.dBgS_PolyPassChk::__vtbl) = BOKO_ACCH_POLY_VTABLE;
  bokoAcchDestruct(&a->mAcch, 0);
  bokoActorDestruct(a, 0);
  if (flags & 1)
    bokoFree(a);
}

/* 020C4080 */
void bokoEmptyDestructor(void* object) {}

/* 020C41CC */
void __sinit_d_a_boko_tail() {
  bokoTailRegistration[2] = 0;
  bokoTailRegistration[0] = 0;
  bokoTailRegistration[3] = 0;
  bokoTailRegistration[1] = 0;
  bokoRegisterGlobal(&bokoGlobalRecord5);
  f32 lower = -3.1415927f, upper = 3.1415927f;
  bokoTailAngleMin = lower;
  bokoTailAngleMax = upper;
  bokoInitializeAngles(&bokoTailAngleStorage);
  bokoRegisterGlobal(&bokoGlobalRecord6);
  bokoInitializeFlags(&bokoTailFlagStorage);
  bokoRegisterGlobal(&bokoGlobalRecord7);
}

/* WWHD glue
data l_cps_src 10191F64
data l_sph_src 10191F24
data l_particleColor 101D5E98
data l_lineColor 10191F00
data m_arc_name 1019204C
data bokoRegistration 1046257C nocheck # BSS
data bokoTailRegistration 10462664 nocheck # BSS
data bokoAngleMin 10462570
data bokoAngleMax 10462574
data bokoAngleStorage 10462578
data bokoFlagStorage 10462579
data bokoTailAngleMin 10462658
data bokoTailAngleMax 1046265C
data bokoTailAngleStorage 10462660
data bokoTailFlagStorage 10462661
data l_break_particle_offset 1046258C
data m_ground_check 10462598
data m_line_check 104625EC
data bokoFlameFlags 10462638
data bokoGlobalRecord0 10191FB0
data bokoGlobalRecord1 10191FBC
data bokoGlobalRecord2 10191FC8
data bokoGlobalRecord3 10191FD4
data bokoGlobalRecord4 10191FE0
data bokoGlobalRecord5 101920F4
data bokoGlobalRecord6 10192100
data bokoGlobalRecord7 1019210C
data m_cull_min 10192064
data m_cull_max 101920AC
data m_top_offset 10192198
data m_blur_root_offset 101921E0
data m_root_offset 10192228
data bokoCarryHideTimer 1047BC2E
data bokoLitSpawnDelay 1047BC32
data bokoSlopeAngle 1047BC2C
data bokoStayRoom 1047E6C8
data bokoChainDamping 1047BBCC
data bokoChainIndexDamping 1047BBD4
data bokoLitSpawnSpeed 1047BBC4
data bokoFlameScaleRate 1047B620
data bokoFlameScaleGrowth 1047B640
data bokoFlameScaleLimit 1047B644
data bokoChainGravity 1047BBD0
data bokoGroundProbeDepth 1047BBDC
data bokoGroundGap 1047BBB0
data bokoGroundCorrection 1047BBC0
data bokoMoveDamping 1047BBB8
data bokoFrameCounter 101FF560
data bokoResourceControl 101F4F28
data bokoGameInfoPointer 101F84DC
data bokoMatrixPointer 1018C7B0
ext 025EA548 void bokoUpdateLine(void* arg0, s32 arg1, void* arg2, s32 arg3, void* arg4, f32 arg5)
ext 025EDD04 void bokoSortLine(void* arg0, void* arg1)
ext 025626A4 void bokoSetupLighting(void* arg0, s32 arg1, void* arg2, void* arg3)
ext 02562F5C void bokoModelLighting(void* arg0, void* arg1, void* arg2)
ext 025E83FC void bokoEnterBrk(void* arg0, void* arg1, f32 arg2)
ext 025204C8 void bokoDeleteResource(void* arg0, void* arg1)
ext 02520460 s32 bokoLoadResource(void* arg0, void* arg1)
ext 0273AA24 void bokoAssert(void* arg0, s32 arg1, void* arg2)
ext 026066C4 void* bokoGetResource(void* arg0, void* arg1, u32 arg2)
ext 025E38E0 void* bokoCreateModel(void* arg0, u32 arg1, u32 arg2)
ext 025E8154 BOOL bokoInitializeBrk(void* arg0, void* arg1, void* arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 arg8, f32 arg9)
ext 024EF130 u8 bokoPolygonRoom(void* arg0, void* arg1)
ext 024EEEB8 u8 bokoPolygonColor(void* arg0, void* arg1)
ext 025E1A40 void bokoStartSound(u32 arg0, void* arg1, u32 arg2, s32 arg3)
ext 0200FAAC s32 bokoAngleDistance(s32 arg0, s32 arg1)
ext 025D63E8 BOOL bokoCreateSolidHeap(void* arg0, BokoHeapCallback arg1, u32 arg2)
ext 025D672C void bokoCullMinimum(void* arg0, f32 arg1, f32 arg2, f32 arg3)
ext 025D673C void bokoCullMaximum(void* arg0, f32 arg1, f32 arg2, f32 arg3)
ext 024EFF44 void bokoSetCircleWall(void* arg0, f32 arg1, f32 arg2)
ext 024F06B4 void bokoSetAcch(void* arg0, void* arg1, void* arg2, void* arg3, s32 arg4, void* arg5, void* arg6, void* arg7, void* arg8)
ext 02515F14 void bokoInitializeStatus(void* arg0, s32 arg1, s32 arg2, void* arg3)
ext 0251677C void bokoSetSphere(void* arg0, void* arg1)
ext 025164C0 void bokoSetCapsule(void* arg0, void* arg1)
ext 025C44AC BokoDarkStatusHD* bokoDarkStatus()
ext 0200ED84 void bokoApproachFloat(void* arg0, f32 arg1, f32 arg2, f32 arg3)
ext 025A847C void bokoSetParticle(void* arg0, u32 arg1, u32 arg2, void* arg3, void* arg4, void* arg5, u32 arg6, void* arg7, s32 arg8, void* arg9, void* arg10, void* arg11)
ext 025A8D40 void bokoSetSimpleParticle(void* arg0, u32 arg1, void* arg2, u32 arg3, void* arg4, void* arg5, u32 arg6)
ext 0200F5C8 void bokoChaseFloat(void* arg0, f32 arg1, f32 arg2)
ext 02018D40 void bokoSetSphereCenter(void* arg0, void* arg1)
ext 0200E240 void bokoRegisterCollision(void* arg0, void* arg1)
ext 02516C14 void bokoSetCollisionMass(void* arg0, void* arg1, u32 arg2)
ext 025B8B94 BOOL bokoCheckEventBit(void* arg0, u32 arg1)
ext 024F08A8 void bokoCorrectPosition(void* arg0, void* arg1)
ext 02018808 void bokoSetCapsuleEnds(void* arg0, void* arg1, void* arg2)
ext 024F1AFC void bokoSetLine(void* arg0, void* arg1, void* arg2, void* arg3)
ext 020084C8 cM3dGPla* bokoTrianglePlane(void* arg0, u32 arg1, u32 arg2)
ext 0200F378 void bokoApproachAngle(void* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
ext 024EEABC BOOL bokoCheckMovingBackground(void* arg0, void* arg1)
ext 02008438 fopAc_ac_c* bokoBackgroundActor(void* arg0, u32 arg1)
ext 020195B0 s32 bokoAtan2(f32 arg0, f32 arg1)
ext 020198D8 f32 bokoRandom(f32 arg0)
ext 02019918 f32 bokoRandomSymmetric(f32 arg0)
ext 028F4384 f32 bokoSqrt(f32 arg0)
ext 025200D4 dComIfG_inf_c* bokoGameInfo()
ext 02555D0C dScnKy_env_light_c* bokoEnvironment()
ext 02520540 s32 bokoReverb(s32 arg0)
ext 025E1B34 void bokoDeleteSound(void* arg0)
ext 0255A374 void bokoCutLight(void* arg0)
ext 025564B4 void bokoSetLight(void* arg0)
ext 025E2E5C void bokoModelEntry(void* arg0)
ext 027F4D5C void bokoModelCalculate(void* arg0)
ext 0273AD10 void* bokoAllocate(u32 arg0)
ext 0273AF40 void bokoFree(void* arg0)
ext 028F026C void bokoRegisterGlobal(void* arg0)
ext 028ED6F8 void bokoInitializeAngles(void* arg0)
ext 028EAB2C void bokoInitializeFlags(void* arg0)
ext 0200FCF0 void bokoMatrixPush()
ext 0200FD38 void bokoMatrixPop()
ext 025F24E0 void bokoTranslateMatrix(f32 arg0, f32 arg1, f32 arg2)
ext 025F2518 void bokoScaleMatrix(f32 arg0, f32 arg1, f32 arg2)
ext 025D4ED0 void bokoActorConstruct(void* arg0)
ext 025D50BC void bokoActorDestruct(void* arg0, s32 arg1)
ext 025E80D0 void bokoBrkConstruct(void* arg0)
ext 024F0474 void bokoAcchConstruct(void* arg0)
ext 024EFE94 void bokoCircleConstruct(void* arg0)
ext 0200BD2C void bokoStatusConstruct(void* arg0)
ext 02515DA0 void bokoGlobalStatusConstruct(void* arg0)
ext 025166F0 void bokoSphereConstruct(void* arg0)
ext 02515FB8 void bokoObjectInfoConstruct(void* arg0)
ext 02018150 void bokoCapsuleConstruct(void* arg0)
ext 02008E0C void bokoGroundConstruct(void* arg0)
ext 02008FEC void bokoLineConstruct(void* arg0)
ext 02008DAC void bokoCheckDestruct(void* arg0, s32 arg1)
ext 02008B4C void bokoLineDestruct(void* arg0, s32 arg1)
ext 02515AE8 void bokoSphereDestruct(void* arg0, s32 arg1)
ext 02515980 void bokoObjectInfoDestruct(void* arg0, s32 arg1)
ext 02515860 void bokoStatusDestruct(void* arg0, s32 arg1)
ext 02018034 void bokoCircleDestruct(void* arg0, s32 arg1)
ext 024EFD9C void bokoAcchDestruct(void* arg0, s32 arg1)
ext 025D9D24 void bokoCancelCarry(void* arg0)
ext 025D9D0C void bokoStartCarry(void* arg0, s32 arg1)
ext 025D6870 void bokoMovePosition(void* arg0, void* arg1)
ext 025D57E0 void bokoRequestDelete(void* arg0)
ext 025A5AC8 void bokoEndFlame(void* arg0)
ext 025A5894 void bokoFlameCallbackConstruct(void* arg0, s32 arg1, s32 arg2)
ext 025E9960 void bokoLineMaterialConstruct(void* arg0)
ext 025E9B80 BOOL bokoLineMaterialInitialize(void* arg0, s32 arg1, s32 arg2, s32 arg3)
ext 02008974 f32 bokoGroundCross(void* arg0, void* arg1)
ext 02008860 BOOL bokoLineCross(void* arg0, void* arg1)
ext 025F1B48 void bokoRotateZXY(void* arg0, s16 arg1, s16 arg2, s16 arg3)
ext 025F1884 void bokoRotateYSet(void* arg0, s16 arg1)
ext 025F18EC void bokoRotateXSet(void* arg0, s16 arg1)
ext 025F1BF4 void bokoRotateX(void* arg0, s32 arg1)
ext 025F1C28 void bokoRotateY(void* arg0, s32 arg1)
ext 025F232C void bokoMatrixToRotation(void* arg0, void* arg1)
ext 028E93CC void bokoMatrixTranslation(void* arg0, f32 arg1, f32 arg2, f32 arg3)
ext 028E90D4 void bokoCopyMatrix(void* arg0, void* arg1)
ext 028E8F64 void bokoTransformVector(void* arg0, void* arg1, void* arg2)
ext 0200FCD8 void bokoTransformStackVector(void* arg0, void* arg1)
ext 028E8DD0 f32 bokoVectorSquareMagnitude(void* arg0)
ext 028E8D88 void bokoAddVector(void* arg0, void* arg1, void* arg2)
ext 0201AD78 void bokoVectorAdd(void* arg0, void* arg1, void* arg2)
ext 0201ADE0 void bokoVectorSubtract(void* arg0, void* arg1, void* arg2)
ext 0201AE48 void bokoVectorMultiply(void* arg0, void* arg1, f32 arg2)
ext 020C0EF8 void bokoCallFlame(void* arg0)
ext 020C1250 void bokoCallChain(void* arg0, void* arg1, s32 arg2)
ext 020C2C90 BOOL bokoCallMove(void* arg0)
ext 020C3480 BOOL bokoCallCarry(void* arg0)
ext 025D9F70 BOOL bokoWaterHeight(void* arg0, void* arg1)
ext 025E742C void bokoPlayAnimation(void* arg0)
ext 025160DC BOOL bokoAttackHit(void* arg0)
ext 02516138 void bokoResetAttackHit(void* arg0)
ext 02516264 void bokoResetTargetHit(void* arg0)
ext 025162A4 BOOL bokoTargetHit(void* arg0)
ext 02516300 dCcD_GObjInf* bokoTargetHitObject(void* arg0)
data m_bound_se 1019201C
data m_heap_size 10192034
data model_idx 1000A504
data base_angle 1000A4B0
data m_at_type 10192180
data m_at_point 10192138
data m_cps_r 10192120
data m_se_type 10192118
*/
