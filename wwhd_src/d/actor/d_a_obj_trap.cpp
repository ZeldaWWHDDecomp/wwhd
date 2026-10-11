/** Blade Trap. Verified HD implementation. */
// HD: GameCube function bodies are stubbed. Preserve the verified chase, recoil and collision
// order.
#include "d/actor/d_a_obj_trap.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/tu.h"
#include <cmath>
const gc_tu_data gc_tu = {.safeString = 0x100316F0};
void trapPlus(cXyz*, cXyz*, cXyz*);
void trapMinus(cXyz*, cXyz*, cXyz*);
void trapScale(cXyz*, cXyz*, f32);
void trapAdd(cXyz*, cXyz*, cXyz*);
f64 trapSquareLength(cXyz*);
f64 trapSquareRoot(f64);
void* trapResource(void*, TrapResourceName*, s32);
void trapAssert(const char*, u32, const char*);
J3DModel* trapCreateModel(void*, u32, u32);
s32 trapInitAnimation(mDoExt_btkAnm*, void*, void*, u32, f32, s32, s32, s32, u32, u32);
dBgW* trapCreateBackground(void*, u32, Mtx34*);
s32 trapNormalize(cXyz*);
void trapTranslate(Mtx34*, f32, f32, f32);
void trapRotateZXY(Mtx34*, s16, s16, s16);
void trapCalcModel(J3DModel*);
void trapCylinderCenter(cM3dGCyl*, cXyz*);
void trapConstructActor(daObjTrap_c*);
void trapConstructAnimation(mDoExt_btkAnm*);
void trapConstructStatus(dCcD_Stts*);
void trapConstructGlobalStatus(void*);
void trapConstructCollider(TrapCylinder*);
void trapConstructCylinder(cM3dGCyl*);
s32 trapLoadResources(request_of_phase_process_class*, const char*);
s32 trapEnterHeap(daObjTrap_c*, void*, u32);
TrapPath* trapRoomPath(u32, s8);
void trapInitStatus(dCcD_Stts*, u32, u32, daObjTrap_c*);
void trapSetCylinder(TrapCylinder*, void*);
void trapRegisterBackground(dBgS*, dBgW*, daObjTrap_c*);
void trapReleaseBackground(dBgS*, dBgW*);
void trapDeleteResources(request_of_phase_process_class*, const char*);
void trapConstructLine(TrapLineCheck*);
void trapRegisterDestructor(void*);
void trapRotateY(Mtx34*, s16);
void trapTransformVector(Mtx34*, cXyz*, cXyz*);
void trapScaleVector(cXyz*, cXyz*, f32);
void trapSetLine(TrapLineCheck*, cXyz*, cXyz*, daObjTrap_c*);
s32 trapLineCross(dBgS*, TrapLineCheck*);
void trapSubtractVector(cXyz*, cXyz*, cXyz*);
s32 trapEqual(cXyz*, cXyz*);
cXyz* trapAllocateVector(u32);
TrapBlock* trapBackgroundActor(dBgS*, u32);
s32 trapIsActor(TrapBlock*);
void trapPushMatrix();
void trapTranslateMatrix(f32, f32, f32);
void trapRotateVector(Mtx34*, cXyz*, cXyz*);
void trapPopMatrix();
void trapAddCalc(be<f32>*, f32, f32, f32, f32);
void trapImpactEffect(cXyz*, cXyz*);
f32 trapDot(cXyz*, cXyz*);
s32 trapPlayAnimation(mDoExt_btkAnm*);
s32 trapReverb(s8);
void trapStartSound(u32, cXyz*, u32, s32);
s32 trapChasePosition(cXyz*, cXyz*, f32);
s32 trapNotEqual(cXyz*, cXyz*);
void trapAddCalcPosition(cXyz*, cXyz*, f32, f32, f32);
void trapSetCollision(void*, TrapCylinder*);
void trapMoveBackground(dBgW*);
void* trapEnvironment();
void trapSettingTev(void*, u32, cXyz*, dKy_tevstr_c*);
void trapLightColor(void*, J3DModel*, dKy_tevstr_c*);
void trapEnterAnimation(mDoExt_btkAnm*, J3DModelData*, f32);
void trapUpdateModel(J3DModel*, u32);
void trapInitRandom(void*);
void trapInitRandomByte(void*);
void trapDestroyLine(void*, u32);
void trapDelete(void*);
void trapDestroyCylinder(TrapCylinder*, u32);
void trapDestroyStatus(dCcD_Stts*, u32);
void trapDestroyActor(daObjTrap_c*, u32);
TrapGameInfo* trapGameInfo();
static u8 sActorVtable;
static u8 sStatusVtable;
static u8 sGlobalStatusVtable;
static u8 sInitialShapeVtable;
static u8 sCylinderAttrVtable;
static u8 sHitInfoVtable;
static u8 sCylinderVtable;
static u8 sFinalShapeVtable;
static u8 sLineCheckVtable;
static u8 sLinePolyVtable;
static u8 sLineGroupVtable;
static u8 sLinePassVtable;
static u8 sLinePassBaseVtable;
static u8 sLineGroupBaseVtable;
static u8 sLinePolyBaseVtable;
static void* sResourceControl;
static Mtx34 sMatrix;
static cXyz sZero;
static cXyz sUp;
static cXyz sBlockNormal;
static cXyz sForward;
static TrapWaitTable sWaits;
static TrapSpeedTable sSpeeds;
static TrapWallAngles sWallAngles;
static TrapWallDistances sWallDistances;
static TrapBlockAngles sBlockAngles;
static TrapBlockDirections sBlockDirections;
static TrapTrigTable sTrig;
static TrapLineCheck sWallLine;
static TrapLineCheck sBlockLine;
static u32 sWallGuard;
static u32 sBlockGuard;
static u8 sWallRegistration;
static u8 sBlockRegistration;
static u8 sPiRegistration;
static u8 sRandomRegistration;
static u8 sRandomByteRegistration;
static TrapHeaderStatics sHeader;
static u8 sCylinderSource;
static BOOL solidHeapCB(daObjTrap_c*);
static void copy(cXyz* dst, const cXyz* src) { dst->copy(*src); }
static void floats(cXyz* dst, const cXyz* src) {
    f32 x = src->x, y = src->y, z = src->z;
    dst->x = x;
    dst->y = y;
    dst->z = z;
}
static void plus(cXyz* a, cXyz* out, cXyz* b) { trapPlus(a, out, b); }
static void minus(cXyz* a, cXyz* out, cXyz* b) { trapMinus(a, out, b); }
static void scaleVector(cXyz* a, cXyz* out, f32 s) { trapScale(a, out, s); }
static void add(cXyz* a, cXyz* b, cXyz* out) { trapAdd(a, b, out); }
static f64 lengthXZ(cXyz* v, f32 zero = 0.0f) {
    gabi::Local<cXyz> h;
    h->x = v->x;
    h->y = zero;
    h->z = v->z;
    f64 n = trapSquareLength(h.get());
    return trapSquareRoot(n);
}
/* 023A6C44 */
static BOOL solidHeapCB(daObjTrap_c* a) { return a->create_heap(); }
/* 023A6AF8 */
BOOL daObjTrap_c::create_heap() {
    gabi::Local<TrapResourceName> name;
    name->mpText = gabi::at<char>(gabi::guest_addr("Trap"));
    name->mVtable = gc_tu.safeString;
    void* data = trapResource(sResourceControl, name.get(), RESOURCE_MODEL);
    if (!data) {
        trapAssert("d_a_obj_trap.cpp", 0x163, "mdl_data != 0");
        return 0;
    }
    J3DModel* model = trapCreateModel(data, 0x80000, 0x11000222);
    mpModel = model;
    if (!model)
        return 0;
    gabi::Local<TrapResourceName> texName;
    texName->mpText = gabi::at<char>(gabi::guest_addr("Trap"));
    texName->mVtable = gc_tu.safeString;
    void* tex = trapResource(sResourceControl, texName.get(), RESOURCE_TEXTURE);
    if (!tex) {
        trapAssert("d_a_obj_trap.cpp", 0x16c, "btk_data != 0");
        return 0;
    }
    if (!trapInitAnimation(&mBtk, data, tex, 1, 1.0f, 0, 0, -1, 0, 0))
        return 0;
    gabi::Local<TrapResourceName> bgName;
    bgName->mpText = gabi::at<char>(gabi::guest_addr("Trap"));
    bgName->mVtable = gc_tu.safeString;
    void* bg = trapResource(sResourceControl, bgName.get(), RESOURCE_COLLISION);
    model = mpModel;
    dBgW* result =
        trapCreateBackground(bg, dBgW::MOVE_BG_e, model ? &model->mBaseTransformMtx : nullptr);
    mpBgW = result;
    return result != nullptr;
}
/* 023A6E70 */
s32 daObjTrap_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & 8)) {
        if (gabi::ea(this) != 0) {
            trapConstructActor(this);
            __vtbl = gabi::ea(&sActorVtable);
            trapConstructAnimation(&mBtk);
            trapConstructStatus(&mStts);
            trapConstructGlobalStatus(&mStts.__vtbl_gstts);
            mStts.__vtbl = gabi::ea(&sStatusVtable);
            mStts.__vtbl_gstts = gabi::ea(&sGlobalStatusVtable);
            trapConstructCollider(&mCyl);
            mCyl.mShapeVtable = gabi::ea(&sInitialShapeVtable);
            mCyl.mAttrVtable = gabi::ea(&sCylinderAttrVtable);
            trapConstructCylinder(&mCyl.mCyl);
            mCyl.__vtbl_hitinf = gabi::ea(&sHitInfoVtable);
            mCyl.mCyl.__vtbl = gabi::ea(&sCylinderVtable);
            flags = actor_condition;
            mCyl.mShapeVtable = gabi::ea(&sFinalShapeVtable);
        }
        actor_condition = flags | 8;
    }
    s32 phase = trapLoadResources(&mPhase, "Trap");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!trapEnterHeap(this, gabi::at<void>(gabi::guest_fn((const void*)solidHeapCB)), 0))
        return cPhs_ERROR_e;
    u32 pathId = ((u32)mParameters & 0xFF);
    mPathId = pathId;
    if (pathId == PATH_NONE)
        return cPhs_ERROR_e;
    TrapPath* path = trapRoomPath(pathId, (s8)current.roomNo);
    mpPath = path;
    if (!path || path->mpPoints == nullptr) {
        mPathId = PATH_NONE;
        return cPhs_ERROR_e;
    }
    u32 type = (mParameters >> 8) & 15;
    if (type == SPEED_DEFAULT) {
        mSpeedType = 0;
        mMoveSpeed = 50.0f;
        mWaitDuration = sWaits.mWait[0];
    } else {
        mSpeedType = type;
        if (type >= 3) {
            trapAssert("d_a_obj_trap.cpp", 0x19c, "M_type < 3");
            type = mSpeedType;
        }
        u32 delayType = mSpeedType;
        path = mpPath;
        mMoveSpeed = sSpeeds.mSpeeds[type];
        mWaitDuration = sWaits.mWait[delayType];
    }
    path = mpPath;
    TrapPathPoint* point = path->mpPoints;
    f32 y = current.pos.y, x = point->mPosition.x, z = point->mPosition.z;
    current.pos.x = x;
    current.pos.z = z;
    current.pos.y = y;
    set_move_info();
    gabi::Local<cXyz> delta;
    minus(&mTarget, delta.get(), &mOrigin);
    mPathLength = lengthXZ(delta.get());
    copy(&mSavedPosition, &current.pos);
    init_mtx();
    trapInitStatus(&mStts, 0, 255, this);
    mCyl.mStts = &mStts;
    trapSetCylinder(&mCyl, &sCylinderSource);
    set_co_pos();
    TrapGameInfo* p = trapGameInfo();
    trapRegisterBackground(&p->mBgS, mpBgW.get(), this);
    return cPhs_COMPLEATE_e;
}
/* 023A7184 */
BOOL daObjTrap_c::_delete() {
    if (heap) {
        dBgW* bg = mpBgW;
        if (bg && bg->m_id < 256) {
            TrapGameInfo* p = trapGameInfo();
            trapReleaseBackground(&p->mBgS, mpBgW.get());
            mpBgW = nullptr;
        }
    }
    trapDeleteResources(&mPhase, "Trap");
    return 1;
}
// HD: each query retains its own first-use guard and static line check.
static void initializeLine(TrapLineCheck* line, be<u32>* guard, void* registration) {
    if (*guard)
        return;
    *guard = 1;
    trapConstructLine(line);
    line->mpPolyPass = &line->mPolyPassVtable;
    line->mpGroupPass = &line->mGroupPassVtable;
    line->mCheckVtable = gabi::ea(&sLineCheckVtable);
    line->mPoly.__vtbl = gabi::ea(&sLinePolyVtable);
    line->mGroupPassVtable = gabi::ea(&sLineGroupVtable);
    line->mGroup = 1;
    line->mPolyPassVtable = gabi::ea(&sLinePassVtable);
    line->mPassFlags[0] = 1;
    for (u32 i = 1; i < 7; ++i)
        line->mPassFlags[i] = 0;
    trapRegisterDestructor(registration);
}
/* 023A6D3C */
void daObjTrap_c::init_mtx() {
    floats(&mpModel->mBaseScale, &scale);
    trapTranslate(&sMatrix, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    trapRotateZXY(&sMatrix, (s16)shape_angle.x, (s16)shape_angle.y, (s16)shape_angle.z);
    f32 m[12];
    for (int i = 0; i < 12; ++i)
        m[i] = sMatrix.m[i / 4][i % 4];
    J3DModel* model = mpModel;
    for (int i = 0; i < 12; ++i)
        model->mBaseTransformMtx.m[i / 4][i % 4] = m[i];
    trapCalcModel(mpModel.get());
}
/* 023A6E24 */
void daObjTrap_c::set_co_pos() {
    gabi::Local<cXyz> p;
    f32 z = current.pos.z, x = current.pos.x, y = current.pos.y;
    p->x = x;
    p->z = z;
    p->y = y - 40.0f;
    trapCylinderCenter(&mCyl.mCyl, p.get());
}
/* 023A7DE0 */
BOOL daObjTrap_c::circle_search() {
    TrapGameInfo* p = trapGameInfo();
    fopAc_ac_c* player = p->mpPlayer;
    gabi::Local<cXyz> delta;
    minus(&player->current.pos, delta.get(), &current.pos);
    f64 distance = lengthXZ(delta.get());
    if (distance > 400.0f || mDirectionValid != 1)
        return 0;
    f32 dz = mDirection.z, z = delta->z, x = delta->x, dx = mDirection.x;
    return !(gabi::fmadds(dx, x, dz * z) < 0.0f);
}
/* 023A6C48 */
void daObjTrap_c::set_move_info() {
    TrapPath* path = mpPath;
    TrapPathPoint* points = path->mpPoints;
    u32 index = mPathPoint;
    f32 y = current.pos.y, z = points[index].mPosition.z, x = points[index].mPosition.x;
    mOrigin.y = y;
    mOrigin.z = z;
    mOrigin.x = x;
    points = path->mpPoints;
    u32 next = (index + 1) & 1;
    z = points[next].mPosition.z;
    x = points[next].mPosition.x;
    mTarget.z = z;
    mTarget.x = x;
    mTarget.y = y;
    gabi::Local<cXyz> delta;
    minus(&mTarget, delta.get(), &mOrigin);
    copy(&mDirection, delta.get());
    mDirectionValid = trapNormalize(&mDirection);
    copy(&mDisplacement, &sZero);
    scaleVector(&mDirection, delta.get(), 100.0f);
    copy(&mSearchVelocity, delta.get());
}
/* 023A7EA4 */
BOOL daObjTrap_c::check_arrival() {
    gabi::Local<cXyz> current, total;
    minus(&mSavedPosition, current.get(), &mOrigin);
    minus(&mTarget, total.get(), &mOrigin);
    f64 travelled = lengthXZ(current.get()), whole = lengthXZ(total.get());
    return !(travelled < whole);
}
/* 023A71FC */
void daObjTrap_c::check_wall(cXyz* output) {
    TrapLineCheck* line = &sWallLine;
    initializeLine(line, &sWallGuard, &sWallRegistration);
    gabi::Local<cXyz> reach, height, best, side, tmp, start, end, hit, shortVec, sum, adjusted;
    scaleVector(&mDirection, reach.get(), 150.0f);
    scaleVector(&sUp, height.get(), 75.0f);
    floats(best.get(), &sZero);
    for (int i = 0; i < 3; ++i) {
        s16 angle = sWallAngles.mAngles[i];
        trapRotateY(&sMatrix, angle);
        trapTransformVector(&sMatrix, &mDirection, side.get());
        f32 distance = sWallDistances.mDistances[i];
        trapScaleVector(side.get(), side.get(), distance);
        add(side.get(), height.get(), side.get());
        plus(&current.pos, tmp.get(), side.get());
        copy(start.get(), tmp.get());
        plus(start.get(), tmp.get(), &mDisplacement);
        copy(end.get(), tmp.get());
        add(end.get(), reach.get(), end.get());
        trapSetLine(line, start.get(), end.get(), this);
        line->mActorPid = mBsPcId;
        TrapGameInfo* p = trapGameInfo();
        if (!trapLineCross(&p->mBgS, line))
            continue;
        copy(hit.get(), &line->mEnd);
        trapSubtractVector(hit.get(), start.get(), hit.get());
        bool replace = trapEqual(best.get(), &sZero) != 0;
        if (!replace) {
            f64 current = lengthXZ(best.get());
            f64 next = lengthXZ(hit.get());
            replace = current > next;
        }
        if (replace) {
            plus(hit.get(), sum.get(), &current.pos);
            minus(sum.get(), adjusted.get(), reach.get());
            copy(best.get(), adjusted.get());
        }
    }
    if (!output)
        output = trapAllocateVector(12);
    if (output)
        floats(output, best.get());
}
/* 023A752C */
BOOL daObjTrap_c::check_block_target_pos(cXyz* position) {
    gabi::Local<cXyz> delta, side;
    minus(position, delta.get(), &current.pos);
    f32 dz = mDirection.z, x = delta->x, z = delta->z, dx = mDirection.x;
    f32 along = gabi::fmadds(dx, x, dz * z);
    if (along < 0.0f)
        return 0;
    if (!(along < (f32)mPathLength + 150.0f))
        return 0;
    trapRotateY(&sMatrix, 0x4000);
    trapTransformVector(&sMatrix, &mDirection, side.get());
    f32 sx = side->x, sz = side->z;
    x = delta->x;
    z = delta->z;
    return std::fabs(gabi::fmadds(sx, x, sz * z)) < 225.0f;
}
/* 023A760C */
void daObjTrap_c::check_block(cXyz* output, cXyz* input) {
    TrapLineCheck* line = &sBlockLine;
    initializeLine(line, &sBlockGuard, &sBlockRegistration);
    gabi::Local<cXyz> reach, best, height, side, tmp, start, end, translation, normal, blockSum,
        blockScaled, blockPos, target, hit, sum, adjusted;
    scaleVector(&mDirection, reach.get(), 150.0f);
    floats(best.get(), input);
    scaleVector(&sUp, height.get(), 75.0f);
    for (int i = 0; i < 2; ++i) {
        s16 angle = sBlockAngles.mAngles[i];
        trapRotateY(&sMatrix, angle);
        trapTransformVector(&sMatrix, &mDirection, side.get());
        trapScaleVector(side.get(), side.get(), 153.0f);
        add(side.get(), height.get(), side.get());
        plus(&current.pos, tmp.get(), side.get());
        copy(start.get(), tmp.get());
        plus(start.get(), tmp.get(), &mDisplacement);
        copy(end.get(), tmp.get());
        add(end.get(), reach.get(), end.get());
        trapSetLine(line, start.get(), end.get(), this);
        line->mActorPid = mBsPcId;
        TrapGameInfo* p = trapGameInfo();
        if (!trapLineCross(&p->mBgS, line))
            continue;
        p = trapGameInfo();
        TrapBlock* block = trapBackgroundActor(&p->mBgS, (u16)line->mPoly.mBgIndex);
        if (!block)
            continue;
        if (!trapIsActor(block))
            continue;
        if (block->mProcName != fpcNm_Obj_Movebox_e)
            continue;
        if (block->mState != 1)
            continue;
        trapPushMatrix();
        trapRotateY(&sMatrix, (s16)block->home.angle.y);
        s32 bx = block->mGridX, bz = block->mGridZ;
        trapTranslateMatrix((f32)bx, 0.0f, (f32)bz);
        translation->z = sMatrix.m[2][3];
        translation->x = sMatrix.m[0][3];
        translation->y = sMatrix.m[1][3];
        u32 direction = block->mDirection;
        s16 rotation = block->home.angle.y, addition = sBlockDirections.mAngles[direction];
        trapRotateY(&sMatrix, (s16)(rotation + addition));
        trapRotateVector(&sMatrix, &sBlockNormal, normal.get());
        trapPopMatrix();
        plus(translation.get(), blockSum.get(), normal.get());
        scaleVector(blockSum.get(), blockScaled.get(), 75.0f);
        plus(blockScaled.get(), blockPos.get(), &block->home.pos);
        copy(target.get(), blockPos.get());
        if (!check_block_target_pos(target.get()))
            continue;
        copy(hit.get(), &line->mEnd);
        trapSubtractVector(hit.get(), start.get(), hit.get());
        bool replace = trapEqual(best.get(), &sZero) != 0;
        if (!replace) {
            f64 current = lengthXZ(best.get());
            f64 next = lengthXZ(hit.get());
            replace = current > next;
        }
        if (replace) {
            plus(hit.get(), sum.get(), &current.pos);
            minus(sum.get(), adjusted.get(), reach.get());
            copy(best.get(), adjusted.get());
        }
    }
    if (!output)
        output = trapAllocateVector(12);
    if (output)
        floats(output, best.get());
}
/* 023A7C24 */
void daObjTrap_c::set_vib_mode() {
    mSavedAngle = (s16)shape_angle.x;
    mBounceTimer = 16;
    mVibrationTimer = 6;
    mBounceAmplitude = 35.0f;
    copy(&mBounceOffset, &sZero);
    mMode = MODE_VIBRATE;
    gabi::Local<cXyz> scaled, position, direction;
    scaleVector(&mDirection, scaled.get(), 150.0f);
    copy(position.get(), scaled.get());
    add(position.get(), &current.pos, position.get());
    scaleVector(&mDirection, scaled.get(), -1.0f);
    copy(direction.get(), scaled.get());
    position->y = (f32)position->y + 50.0f;
    trapImpactEffect(position.get(), direction.get());
    if (mVibrationTimer) {
        vibrate();
        mVibrationTimer = (u16)((u16)mVibrationTimer - 1);
    }
    if (mBounceTimer) {
        bound();
        mBounceTimer = (u16)((u16)mBounceTimer - 1);
    }
}
/* 023A7AD8 */
void daObjTrap_c::vibrate() {
    u32 index = ((u32)mVibrationTimer * 0x5555) & 65535;
    index = (index >> 3) * 8;
    f32 sine = sTrig.mEntries[index / 8].mSin;
    shape_angle.x = (s16)gabi::ftoi(288.0f * sine);
}
/* 023A7B20 */
void daObjTrap_c::bound() {
    gabi::Local<cXyz> offset;
    scaleVector(&mDirection, offset.get(), -1.0f);
    trapSubtractVector(&mSavedPosition, &mBounceOffset, &mSavedPosition);
    u32 phase = ((u32)mBounceTimer << 14) & 0xc000;
    f32 amplitude = mBounceAmplitude, sine = sTrig.mEntries[phase / 8].mSin;
    f32 step = std::fabs((f32)(s16)gabi::ftoi(amplitude * sine));
    trapScaleVector(offset.get(), offset.get(), step);
    add(&mSavedPosition, offset.get(), &mSavedPosition);
    copy(&mBounceOffset, offset.get());
    trapAddCalc(&mBounceAmplitude, 0.0f, 0.17f, 35.0f, 1.0f);
}
/* 023A7D5C */
void daObjTrap_c::set_shine() {
    mShining = 1;
    f32 dot = trapDot(&sForward, &mDirection);
    if (dot < 0.0f) {
        mBtk.mFrameCtrl.mFrame = 46.0f;
        mBtk.mFrameCtrl.mRate = -1.0f;
    } else {
        mBtk.mFrameCtrl.mFrame = 35.0f;
        mBtk.mFrameCtrl.mRate = 1.0f;
    }
}
/* 023A7F74 */
void daObjTrap_c::shine_move() {
    if (mShining == 1 && trapPlayAnimation(&mBtk))
        mShining = 0;
}
static void impactSound(daObjTrap_c* a, u32 sound) {
    s32 reverb = trapReverb((s8)a->current.roomNo);
    trapStartSound(sound, &a->current.pos, 0, reverb);
}
/* 023A7FC0 */
BOOL daObjTrap_c::_execute() {
    copy(&mSavedPosition, &current.pos);
    u32 mode = mMode;
    bool positionCommitted = false;
    gabi::Local<cXyz> wall, block, delta;
    if (mode == MODE_MOVE) {
        s32 arrived = trapChasePosition(&mSavedPosition, &mTarget, (f32)mMoveSpeed);
        minus(&mSavedPosition, delta.get(), &current.pos);
        copy(&mDisplacement, delta.get());
        check_wall(delta.get());
        floats(block.get(), delta.get());
        check_block(delta.get(), block.get());
        copy(wall.get(), delta.get());
        if (trapNotEqual(wall.get(), &sZero)) {
            copy(&mSavedPosition, wall.get());
            set_vib_mode();
            set_shine();
            impactSound(this, SE_TRAP_HIT);
        } else if (arrived == 1) {
            set_vib_mode();
            set_shine();
            impactSound(this, SE_TRAP_HIT);
        } else if (circle_search() == 1) {
            copy(&current.pos, &mSavedPosition);
            mMode = MODE_CHASE;
            positionCommitted = true;
        } else
            impactSound(this, SE_TRAP_MOVE);
    } else if (mode == MODE_CHASE) {
        trapAddCalcPosition(&mDisplacement, &mSearchVelocity, 0.06f, 100.0f, 1.0f);
        add(&mSavedPosition, &mDisplacement, &mSavedPosition);
        check_wall(block.get());
        floats(delta.get(), block.get());
        check_block(block.get(), delta.get());
        copy(wall.get(), block.get());
        if (trapNotEqual(wall.get(), &sZero)) {
            copy(&mSavedPosition, wall.get());
            set_vib_mode();
            set_shine();
            impactSound(this, SE_TRAP_HIT);
        } else if (check_arrival() == 1) {
            copy(&mSavedPosition, &mTarget);
            set_vib_mode();
            set_shine();
            impactSound(this, SE_TRAP_HIT);
        } else
            impactSound(this, SE_TRAP_MOVE);
    } else if (mode == MODE_VIBRATE) {
        u16 vibration = mVibrationTimer;
        if (vibration) {
            vibrate();
            vibration = (u16)((u16)mVibrationTimer - 1);
            mVibrationTimer = vibration;
        }
        if (mBounceTimer) {
            bound();
            u16 bounce = mBounceTimer;
            vibration = mVibrationTimer;
            mBounceTimer = (u16)(bounce - 1);
        }
        if (vibration == 0) {
            u16 bounce = mBounceTimer;
            s16 angle = mSavedAngle;
            shape_angle.x = angle;
            if (bounce == 0) {
                mWaitTimer = (s16)mWaitDuration;
                mMode = MODE_WAIT;
            }
        }
    } else if (mode == MODE_WAIT) {
        s16 timer = mWaitTimer;
        if (timer >= 0) {
            if (timer > 0) {
                timer = (s16)(timer - 1);
                mWaitTimer = timer;
            }
            if (timer == 0) {
                mPathPoint = ((u8)mPathPoint + 1) & 1;
                set_move_info();
                mMode = MODE_MOVE;
            }
        }
    }
    if (!positionCommitted)
        copy(&current.pos, &mSavedPosition);
    shine_move();
    init_mtx();
    set_co_pos();
    TrapGameInfo* p = trapGameInfo();
    trapSetCollision(p->mCollision, &mCyl);
    if (heap) {
        dBgW* bg = mpBgW;
        if (bg && bg->m_id < 256)
            trapMoveBackground(bg);
    }
    return 1;
}
/* 023A85D4 */
BOOL daObjTrap_c::_draw() {
    void* env = trapEnvironment();
    trapSettingTev(env, 1, &current.pos, &tevStr);
    env = trapEnvironment();
    trapLightColor(env, mpModel.get(), &tevStr);
    J3DModel* model = mpModel;
    J3DModelData* data = model->mModelData;
    trapEnterAnimation(&mBtk, data, (f32)mBtk.mFrameCtrl.mFrame);
    trapUpdateModel(mpModel.get(), 0);
    return 1;
}
/* 023A8644 */
static s32 Mthd_Create(daObjTrap_c* a) { return a->_create(); }
/* 023A8648 */
static BOOL Mthd_Delete(daObjTrap_c* a) { return a->_delete(); }
/* 023A864C */
static BOOL Mthd_Execute(daObjTrap_c* a) { return a->_execute(); }
/* 023A8650 */
static BOOL Mthd_Draw(daObjTrap_c* a) { return a->_draw(); }
/* 023A87E4 */
static BOOL Mthd_IsDelete(daObjTrap_c* a) { return 1; }

// HD: compiler-generated and template code of this translation unit.
/* 023A8654 */
static void __sinit_d_a_obj_trap_cpp() {
    sHeader.mSeed2 = 0;
    sHeader.mSeed0 = 0;
    sHeader.mSeed3 = 0;
    sHeader.mSeed1 = 0;
    trapRegisterDestructor(&sPiRegistration);
    sHeader.mMin = -3.1415927410125732f;
    sHeader.mMax = 3.1415927410125732f;
    trapInitRandom(&sHeader.mRandom);
    trapRegisterDestructor(&sRandomRegistration);
    trapInitRandomByte(&sHeader.mRandomBytes[1]);
    trapRegisterDestructor(&sRandomByteRegistration);
}
/* 023A86E8 */
static void lineDestructor(void* p, u32 flags) {
    if (p) {
        auto* line = static_cast<TrapLineCheck*>(p);
        line->mPolyPassVtable = gabi::ea(&sLinePassBaseVtable);
        line->mGroupPassVtable = gabi::ea(&sLineGroupBaseVtable);
        line->mPoly.__vtbl = gabi::ea(&sLinePolyBaseVtable);
        trapDestroyLine(p, 0);
        if (flags & 1)
            trapDelete(p);
    }
}
/* 023A8760 */
static void emptyDestructor(void* p, u32 flags) {
    if (p && (flags & 1))
        trapDelete(p);
}
/* 023A8774 */
static void actorDestructor(daObjTrap_c* a, u32 flags) {
    if (a) {
        trapDestroyCylinder(&a->mCyl, 2);
        trapDestroyStatus(&a->mStts, 2);
        trapDestroyActor(a, 0);
        if (flags & 1)
            trapDelete(a);
    }
}
/* 023A87E0 */
static void emptyVirtual(void* p) {}
// clang-format off
/* WWHD glue
str 10031880
str 100317F8 in 023A6AF8
str 100317D8
str 100317E8
str 10031818 in 023A6E70
str 1003182C
data sActorVtable 100317B8
data sStatusVtable 1004AE88
data sGlobalStatusVtable 1004AEC0
data sInitialShapeVtable 100015A8
data sCylinderAttrVtable 10031708
data sHitInfoVtable 1004B108
data sCylinderVtable 1004B150
data sFinalShapeVtable 1004B160
data sLineCheckVtable 10031778
data sLinePolyVtable 10031788
data sLineGroupVtable 10031798
data sLinePassVtable 100317A8
data sLinePassBaseVtable 10031768
data sLineGroupBaseVtable 10031728
data sLinePolyBaseVtable 10031718
data sResourceControl 101F4F28
data sMatrix 1048D0CC
data sZero 101FFBA8
data sUp 101FFBC0
data sBlockNormal 101FFBCC
data sForward 101FFBB4
data sWaits 10031878
data sSpeeds 10031888
data sWallAngles 100316E8
data sWallDistances 100317C8
data sBlockAngles 101CD5A4
data sBlockDirections 1004BCE8
data sTrig 104A44F8
data sWallLine 1046C5B0
data sBlockLine 1046C61C
data sWallGuard 1046C688
data sBlockGuard 1046C68C
data sWallRegistration 101CD5EC
data sBlockRegistration 101CD5F8
data sPiRegistration 101CD604
data sRandomRegistration 101CD610
data sRandomByteRegistration 101CD61C
data sHeader 1046C594
data sCylinderSource 101CD5A8
ext 0201AD78 void trapPlus(cXyz*, cXyz*, cXyz*)
ext 0201ADE0 void trapMinus(cXyz*, cXyz*, cXyz*)
ext 0201AE48 void trapScale(cXyz*, cXyz*, f32)
ext 028E8D88 void trapAdd(cXyz*, cXyz*, cXyz*)
ext 028E8DD0 f64 trapSquareLength(cXyz*)
ext 028F4384 f64 trapSquareRoot(f64)
ext 026066C4 void* trapResource(void*, TrapResourceName*, s32)
ext 0273AA24 void trapAssert(const char*, u32, const char*)
ext 025E38E0 J3DModel* trapCreateModel(void*, u32, u32)
ext 025E7CE0 s32 trapInitAnimation(mDoExt_btkAnm*, void*, void*, u32, f32, s32, s32, s32, u32, u32)
ext 024F2478 dBgW* trapCreateBackground(void*, u32, Mtx34*)
ext 0201B47C s32 trapNormalize(cXyz*)
ext 028E93CC void trapTranslate(Mtx34*, f32, f32, f32)
ext 025F1B48 void trapRotateZXY(Mtx34*, s16, s16, s16)
ext 027F4D5C void trapCalcModel(J3DModel*)
ext 020182E0 void trapCylinderCenter(cM3dGCyl*, cXyz*)
ext 025D4ED0 void trapConstructActor(daObjTrap_c*)
ext 025E7C6C void trapConstructAnimation(mDoExt_btkAnm*)
ext 0200BD2C void trapConstructStatus(dCcD_Stts*)
ext 02515DA0 void trapConstructGlobalStatus(void*)
ext 02515FB8 void trapConstructCollider(TrapCylinder*)
ext 02018590 void trapConstructCylinder(cM3dGCyl*)
ext 02520460 s32 trapLoadResources(request_of_phase_process_class*, const char*)
ext 025D63E8 s32 trapEnterHeap(daObjTrap_c*, void*, u32)
ext 025AAF88 TrapPath* trapRoomPath(u32, s8)
ext 02515F14 void trapInitStatus(dCcD_Stts*, u32, u32, daObjTrap_c*)
ext 02516518 void trapSetCylinder(TrapCylinder*, void*)
ext 024EEA6C void trapRegisterBackground(dBgS*, dBgW*, daObjTrap_c*)
ext 020087EC void trapReleaseBackground(dBgS*, dBgW*)
ext 025204C8 void trapDeleteResources(request_of_phase_process_class*, const char*)
ext 02008FEC void trapConstructLine(TrapLineCheck*)
ext 028F026C void trapRegisterDestructor(void*)
ext 025F1884 void trapRotateY(Mtx34*, s16)
ext 028E8F64 void trapTransformVector(Mtx34*, cXyz*, cXyz*)
ext 028E8E64 void trapScaleVector(cXyz*, cXyz*, f32)
ext 024F1AFC void trapSetLine(TrapLineCheck*, cXyz*, cXyz*, daObjTrap_c*)
ext 02008860 s32 trapLineCross(dBgS*, TrapLineCheck*)
ext 028E8DAC void trapSubtractVector(cXyz*, cXyz*, cXyz*)
ext 0201AF98 s32 trapEqual(cXyz*, cXyz*)
ext 0273AD10 cXyz* trapAllocateVector(u32)
ext 02008438 TrapBlock* trapBackgroundActor(dBgS*, u32)
ext 025D4604 s32 trapIsActor(TrapBlock*)
ext 025F23EC void trapPushMatrix()
ext 025F24E0 void trapTranslateMatrix(f32, f32, f32)
ext 028E9044 void trapRotateVector(Mtx34*, cXyz*, cXyz*)
ext 025F2468 void trapPopMatrix()
ext 0200ECD4 void trapAddCalc(be<f32>*, f32, f32, f32, f32)
ext 02312D6C void trapImpactEffect(cXyz*, cXyz*)
ext 028E8F44 f32 trapDot(cXyz*, cXyz*)
ext 025E742C s32 trapPlayAnimation(mDoExt_btkAnm*)
ext 02520540 s32 trapReverb(s8)
ext 025E1A40 void trapStartSound(u32, cXyz*, u32, s32)
ext 0200F764 s32 trapChasePosition(cXyz*, cXyz*, f32)
ext 0201AFD8 s32 trapNotEqual(cXyz*, cXyz*)
ext 0200EF78 void trapAddCalcPosition(cXyz*, cXyz*, f32, f32, f32)
ext 0200E240 void trapSetCollision(void*, TrapCylinder*)
ext 024F43DC void trapMoveBackground(dBgW*)
ext 02555D0C void* trapEnvironment()
ext 025626A4 void trapSettingTev(void*, u32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void trapLightColor(void*, J3DModel*, dKy_tevstr_c*)
ext 025E7FC4 void trapEnterAnimation(mDoExt_btkAnm*, J3DModelData*, f32)
ext 025E2DE0 void trapUpdateModel(J3DModel*, u32)
ext 028ED6F8 void trapInitRandom(void*)
ext 028EAB2C void trapInitRandomByte(void*)
ext 02008B4C void trapDestroyLine(void*, u32)
ext 0273AF40 void trapDelete(void*)
ext 02515A70 void trapDestroyCylinder(TrapCylinder*, u32)
ext 02515860 void trapDestroyStatus(dCcD_Stts*, u32)
ext 025D50BC void trapDestroyActor(daObjTrap_c*, u32)
ext 025200D4 TrapGameInfo* trapGameInfo()
*/
// clang-format on
