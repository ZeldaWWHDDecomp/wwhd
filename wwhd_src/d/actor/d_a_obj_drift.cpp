/* WWHD Drift. Ported from zeldaret/tww with audited HD layout/behavior.
 */
#include "d/actor/d_a_obj_drift.h"
using daObjDrift::Act_c;
static cXyz sFlowerOffset;
static u32 sFlowerOffsetInitialized;
static DriftFlowerProfiles sFlowerProfiles;
static DriftDrawBuffers sDrawBuffers;
static cXyz sZero;
static DriftSinCosTable sSinCos;
static DriftModeProc sModeProc;
static DriftHeaderStatics sHeader;
static DriftDestructorRecords sDestructorRecords;
static void* sResourceManager;
static DriftCylinderSource sCylinderSource;

/* 023384F0 */
void daObjDrift::Act_c::set_mtx() {

    Mtx34* matrix = mDoMtx_stack_c::get();
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    driftTranslate(matrix, x, y, z);
    gabi::Local<cXyz> axis;
    axis->x = mTiltX;
    axis->y = 1.0f;
    axis->z = mTiltZ;
    gabi::Local<DriftQuaternion> quat;
    driftQuatRotBaseY2(quat.get(), axis.get());
    driftQuatMatrix(quat.get());
    s16 ay = shape_angle.y, az = shape_angle.z, ax = shape_angle.x;
    driftRotateZXY(matrix, ax, ay, az);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = matrix->m[i / 4][i % 4];
    J3DModel* model = mModel;
    for (u32 i = 0; i < 12; ++i)
        model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    driftCopyMatrix(matrix, &mMtx);
    f32 height = field_0x4C8, prev = mPrevPos.y;
    field_0x4CC = height;
    field_0x4C8 = prev;
    mPrevPos.x = matrix->m[0][3];
    mPrevPos.y = matrix->m[1][3];
    mPrevPos.z = matrix->m[2][3];
}
/* 02338618 */
void daObjDrift::Act_c::init_mtx() {

    J3DModel* model = mModel;
    f32 y = scale.y, x = scale.x, z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    set_mtx();
}
/* 02338638 */
void daObjDrift::Act_c::calc_flower_param(cXyz* pos, csXyz* angle) {

    if (!(u32)sFlowerOffsetInitialized) {
        sFlowerOffsetInitialized = 1;
        sFlowerOffset.z = 0;
        sFlowerOffset.x = -489.3500061035156f;
        sFlowerOffset.y = 600.0f;
    }
    driftMultVec(&mMtx, &sFlowerOffset, pos);
    driftMtxToRot(&mMtx, angle);
}
/* 02338408 */
BOOL daObjDrift::Act_c::Mthd_Delete() {

    BOOL ret = MoveBGDelete();
    driftDeleteResource(&mPhase, "Kryu");
    return ret;
}
/* 02338454 */
BOOL daObjDrift::Act_c::CreateHeap() {

    gabi::Local<SafeString> name;
    name->mStringTop = gabi::guest_addr("Kryu");
    name->__vtbl = 0x10027710;
    void* data = driftGetResource(sResourceManager.get(), name.get(), dRes_INDEX_KRYU_BDL_RYU_00_e);
    if (!data)
        driftAssert("d_a_obj_drift.cpp", 0x13A, "model_data != 0");
    mModel = driftCreateModel(data, 0x80000, 0x11000022);
    return mModel != nullptr;
}
/* 023386CC */
void daObjDrift::Act_c::make_flower() {

    if ((u32)mType < 2) {
        gabi::Local<cXyz> pos;
        gabi::Local<csXyz> angle;
        calc_flower_param(pos.get(), angle.get());
        s32 type = mType;
        s8 room = (s8)home.roomNo;
        s16 profile = (s16)sFlowerProfiles.mProfiles[(u32)type];
        mFlowerPid = driftCreateActor(profile, 0, pos.get(), room, angle.get(), 0, -1, 0);
    } else
        mFlowerPid = fpcM_ERROR_PROCESS_ID_e;
}
/* 02338D28 */
void daObjDrift::Act_c::set_flower_current() {

    u32 pid = mFlowerPid;
    if (pid == fpcM_ERROR_PROCESS_ID_e)
        return;
    fopAc_ac_c* flower = fopAcM_SearchByID(pid);
    if (!flower)
        return;
    gabi::Local<cXyz> pos;
    gabi::Local<csXyz> angle;
    calc_flower_param(pos.get(), angle.get());
    flower->current.pos.x = pos->x;
    flower->current.pos.y = pos->y;
    flower->current.pos.z = pos->z;
    csXyz* flowerAngle = mType == 0 ? &flower->shape_angle : &flower->current.angle;
    flowerAngle->x = angle->x;
    flowerAngle->y = angle->y;
    flowerAngle->z = angle->z;
}
// HD: each draw-list override re-reads game info; no cached common getter.
/* 02338EC8 */
BOOL daObjDrift::Act_c::Draw() {

    void* light = driftGetEnvLight();
    driftSetLight(light, 1, &current.pos, &tevStr);
    light = driftGetEnvLight();
    driftApplyLight(light, (J3DModel*)mModel, &tevStr);
    DriftGameInfo* play = driftGetGameInfo();
    sDrawBuffers.mOpaque = play->mOpaqueCurrent;
    play = driftGetGameInfo();
    sDrawBuffers.mTranslucent = play->mTranslucentCurrent;
    driftUpdateModel((J3DModel*)mModel, 0);
    play = driftGetGameInfo();
    sDrawBuffers.mOpaque = play->mOpaqueNormal;
    play = driftGetGameInfo();
    sDrawBuffers.mTranslucent = play->mTranslucentNormal;
    return TRUE;
}
/* 02338898 */
void daObjDrift::Act_c::mode_wait_init() {

    actor_status = (u32)actor_status | fopAcStts_NOCULLEXEC_e;
    mMode = MODE_WAIT;
}
// HD: retain the constructor and rotation update store order.
/* 02338F60 */
void daObjDrift::Act_c::mode_rot_init() {

    u32 status = actor_status & ~fopAcStts_NOCULLEXEC_e;
    s8 room = current.roomNo;
    s16 angle = shape_angle.y;
    mRotTimer = 0;
    mMode = MODE_ROT;
    mAngleChaseY = 0;
    shape_angle.y = (s16)(angle + 10);
    actor_status = status;
    mTargetAngleY = (s16)(angle + 0x8000);
    s32 reverb = driftGetReverb(room);
    driftStartSound(0x6A1C, &eyePos, 0, reverb);
}
/* 02338FF0 */
void daObjDrift::Act_c::mode_wait() {

    if (driftCheckTgHit(&mCyl)) {
        driftClearTgHit(&mCyl);
        DriftGameInfo* play = driftGetGameInfo();
        fopAc_ac_c* player = play->mpPlayer.get();
        f32 distance = driftPlayerDistanceSquared(this, player);
        if (distance > 302500.0f) {
            f32 x = (f32)mCyl.mGObjTg.mRVec.x, z = (f32)mCyl.mGObjTg.mRVec.z;
            mHitDir.x = x;
            f32 y = (f32)mCyl.mGObjTg.mRVec.y;
            mHitDir.z = z;
            mHitDir.y = y;
            gabi::Local<cXyz> output;
            driftNormalizeZP(&mHitDir, output.get());
            mode_rot_init();
        }
    } else {
        driftMoveCylinder(&mCyl, &current.pos);
        DriftGameInfo* play = driftGetGameInfo();
        driftRegisterCylinder(&play->mCcS, &mCyl);
    }
}
// HD: the timer uses a less-than comparison; its NaN behavior differs from the GC condition.
/* 023390AC */
void daObjDrift::Act_c::mode_rot() {

    s16 diff = (s16)((s16)mTargetAngleY - (s16)shape_angle.y);
    if (diff <= 0) {
        shape_angle.y = mTargetAngleY;
        mHitDir.x = (f32)sZero.x;
        mHitDir.y = (f32)sZero.y;
        mHitDir.z = (f32)sZero.z;
        mode_wait_init();
        f32 timer = mRotTimer;
        if (timer < 10.0f)
            mRotTimer = timer + 1.0f;
        else {
            field_0x4D0 = 1;
            mRotTimer = 0;
        }
    } else {
        if (diff < 0x2000)
            driftChaseAngle(&mAngleChaseY, 20, 10);
        else
            driftChaseAngle(&mAngleChaseY, 400, 20);
        shape_angle.y = (s16)((s16)shape_angle.y + (s16)mAngleChaseY);
        f32 timer = mRotTimer;
        if (timer < 10.0f)
            mRotTimer = timer + 1.0f;
        else {
            field_0x4D0 = 1;
            mRotTimer = 0;
        }
    }
}
/* 02339314 */
u32 prmAbstract(Act_c* actor, u32 width, u32 shift) {

    u32 parameters = actor->mParameters, w = width & 63, s = shift & 63;
    u32 mask = (w < 32 ? 1u << w : 0u) - 1u, value = s < 32 ? parameters >> s : 0u;
    return value & mask;
}
/* 023382AC */
s32 daObjDrift::Act_c::Mthd_Create() {

    u32 status = actor_condition;
    if (!(status & fopAcCnd_INIT_e)) {
        if (this) {
            driftConstructMoveBg(this);
            __vtbl = 0x10027884;
            dCcD_Stts_ct(&mStts);
            driftConstructGObj(&mCyl);
            DriftShapeAttr* shape = reinterpret_cast<DriftShapeAttr*>(&mCyl.mShapeAttr);
            shape->mShapeVtable = 0x100015A8;
            shape->mAabVtable = 0x10027728;
            driftConstructCyl(&mCyl.mCyl);
            mCyl.__vtbl_hitinf = 0x1004B108;
            shape->mShapeVtable = 0x1004B160;
            status = actor_condition;
            mCyl.mCyl.__vtbl = 0x1004B150;
        }
        actor_condition = status | fopAcCnd_INIT_e;
    }
    s32 phase = driftLoadResource(&mPhase, "Kryu");
    if (phase == cPhs_COMPLEATE_e) {
        mType = prmAbstract(this, PRM_TYPE_W, PRM_TYPE_S);
        phase = driftCreateMoveBg(this, "Kryu", dRes_INDEX_KRYU_DZB_RYU_00_e, 0x024EE708, 0x1B80);
        if (phase != cPhs_COMPLEATE_e && phase != cPhs_ERROR_e)
            driftAssert("d_a_obj_drift.cpp", 0x19A,
                        "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return phase;
}
/* 023391C0 */
BOOL wrapperCreate(Act_c* actor) {

    return actor->Mthd_Create();
}
/* 023391C4 */
BOOL wrapperDelete(Act_c* actor) {

    return actor->Mthd_Delete();
}
/* 023391C8 */
BOOL wrapperExecute(Act_c* actor) {

    return actor->MoveBGExecute();
}
/* 023391CC */
BOOL wrapperDraw(Act_c* actor) {

    return actor->Draw_v();
}
/* 023391DC */
BOOL wrapperIsDelete(Act_c* actor) {

    return actor->IsDelete_v();
}
// HD: compiler-generated and template code of this translation unit.
/* 023391EC */
void staticInitialize() {

    sHeader.mZero[2] = 0;
    sHeader.mZero[0] = 0;
    sHeader.mZero[3] = 0;
    sHeader.mZero[1] = 0;
    driftRegisterGlobal(&sDestructorRecords.mRecords[0]);
    sHeader.mMinAngle = -3.1415927410125732f;
    sHeader.mMaxAngle = 3.1415927410125732f;
    driftInitializeHeaderByte(&sHeader.mHeaderByte);
    driftRegisterGlobal(&sDestructorRecords.mRecords[1]);
    driftInitializeHeaderFlag(&sHeader.mHeaderFlag);
    driftRegisterGlobal(&sDestructorRecords.mRecords[2]);
}
/* 02339280 */
void deleteStatic(void* p, s32 flags) {

    if (p && (flags & 1))
        driftDeleteObject(p);
}
/* 02339294 */
BOOL IsDelete(Act_c* actor) {

    return TRUE;
}
/* 0233929C */
void emptyVirtual() {}
/* 023392A0 */
BOOL daObjDrift::Act_c::Delete() {

    return TRUE;
}
/* 023392A8 */
void destruct(Act_c* actor, s32 flags) {

    if (actor) {
        driftDestroyCylinder(&actor->mCyl, 2);
        driftDestroyStatus(&actor->mStts, 2);
        driftDestroyActor(actor, 0);
        if (flags & 1)
            driftDeleteObject(actor);
    }
}
/* 02338758 */
void rideCB(dBgW* bg, Act_c* actor, fopAc_ac_c* rider) {

    if (!rider || (s16)rider->mProfName != cc_uty::fpcNm_PLAYER_e)
        return;
    actor->mRideYOff = -20.0f;
    gabi::Local<cXyz> delta;
    driftSubtract(&rider->current.pos, delta.get(), &actor->current.pos);
    gabi::Local<Mtx34> inverse;
    if (driftInverse(&actor->mMtx, inverse.get())) {
        DriftGameInfo* play = driftGetGameInfo();
        if (!((u32)play->mPlayerStatus & 0x100)) {
            gabi::Local<cXyz> local;
            driftMultVecSR(inverse.get(), delta.get(), local.get());
            gabi::Local<cXyz> flat;
            flat->x = local->x;
            flat->y = 0;
            flat->z = local->z;
            f32 mag = driftSquaredMagnitude(flat.get()), y = local->y;
            if ((y < 400.0f && mag < 129600.0f) ||
                (!(y < 400.0f) && y < 550.0f && mag < 48400.0f)) {
                actor->mRideFlag = 1;
                return;
            }
            if (!actor->mRideFlag) {
                f32 z = delta->z, x = delta->x;
                actor->mTiltTargetZ = z * 0.00024000000848900527f;
                actor->mTiltTargetX = x * 0.00024000000848900527f;
            }
            return;
        }
    }
    actor->mRideFlag = 1;
}
/* 023388B0 */
BOOL daObjDrift::Act_c::Create() {

    J3DModel* model = mModel;
    cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    init_mtx();
    driftSetCullBox(this, -550.0f, -180.0f, -550.0f, 550.0f, 605.0f, 550.0f);
    driftInitializeStatus(&mStts, 255, 255, this);
    driftSetCylinder(&mCyl, &sCylinderSource);
    mCyl.mStts = &mStts;
    f32 zx = (f32)sZero.x, zy = (f32)sZero.y, zz = (f32)sZero.z;
    u32 flags = (u32)mCyl.mGObjTg.mSPrm;
    mCyl.mGObjTg.mVec.x = zx;
    mCyl.mGObjTg.mVec.y = zy;
    mCyl.mGObjTg.mSPrm = flags | 4;
    mCyl.mGObjTg.mVec.z = zz;
    make_flower();
    reinterpret_cast<DriftBackground*>(mpBgW.get())->mRideCallback =
        gabi::guest_fn((const void*)rideCB);
    mode_wait_init();
    gabi::Local<be<f32>> water;
    if (driftGetWaterY(&home.pos, water.get()))
        eyePos.y = *water.get();
    mRideYOff = 0;
    mTiltTargetZ = 0;
    mTiltTargetX = 0;
    mTiltVelZ = 0;
    mTiltVelX = 0;
    mTiltZ = 0;
    mTiltX = 0;
    mWavePhaseX = (s16)gabi::ftoi(driftRandomSigned(32768.0f));
    mWavePhaseY = (s16)gabi::ftoi(driftRandomSigned(32768.0f));
    s32 phase = gabi::ftoi(driftRandomSigned(32768.0f));
    mHitDir.x = zx;
    mHitDir.y = zy;
    f32 x = current.pos.x, y = current.pos.y;
    mPrevPos.x = x;
    mPrevPos.y = y;
    mHitDir.z = zz;
    f32 z = current.pos.z;
    field_0x4B0.x = 5;
    mPrevPos.z = z;
    mRideFlag = 0;
    field_0x4B0.y = 5;
    field_0x4B0.z = 5;
    field_0x4C8 = y;
    field_0x4CC = y;
    field_0x4D0 = 0;
    mWavePhaseZ = (s16)phase;
    mTargetAngleY = 0;
    mAngleChaseY = 0;
    field_0x4D4 = 0;
    return TRUE;
}
// HD: preserve fused multiply-add/subtract operations and the original float reload order.
/* 02338AF8 */
void daObjDrift::Act_c::set_current() {

    f32 y = (f32)mRideYOff + (f32)home.pos.y;
    u16 phase = (s16)mWavePhaseX + 400;
    mWavePhaseX = (s16)phase;
    f32 delta = y - (f32)current.pos.y, sine = (f32)sSinCos.mEntries[phase >> 3].mSin,
        wave = sine * 0.30000001192092896f;
    gravity = gabi::fmadds(delta, 0.019999999552965164f, wave);
    driftPosMoveStream(this, 0, &sZero, 0.03999999910593033f, 0.009999999776482582f);
    f32 factor = mRideFlag ? 0.20000000298023224f : 1.0f,
        chase = mMode == MODE_ROT ? 0.5f : 0.800000011920929f;
    PSVECScale(&mHitDir, &mHitDir, 0.949999988079071f);
    u16 py = (s16)mWavePhaseY + 1000;
    f32 targetZ = mTiltTargetZ;
    u16 pz = (s16)mWavePhaseZ + 1111;
    mWavePhaseY = (s16)py;
    mWavePhaseZ = (s16)pz;
    f32 tz = mTiltZ, sinZ = (f32)sSinCos.mEntries[pz >> 3].mSin, diffZ = tz - targetZ,
        targetX = mTiltTargetX;
    f32 waveX = sinZ * 0.009999999776482582f, sinY = (f32)sSinCos.mEntries[py >> 3].mSin,
        dampZ = diffZ * chase, tx = mTiltX;
    f32 waveZ = sinY * 0.009999999776482582f, diffX = tx - targetX;
    f32 forceZ = gabi::fmsubs(waveZ, factor, dampZ), hitZ = mHitDir.z, dampX = diffX * chase;
    mRideFlag = 0;
    f32 accelZ = gabi::fmadds(hitZ, 0.5f, forceZ), velZ = mTiltVelZ,
        forceX = gabi::fmsubs(waveX, factor, dampX), hitX = mHitDir.x;
    velZ += accelZ;
    f32 accelX = gabi::fmadds(hitX, 0.5f, forceX), velX = mTiltVelX;
    f32 nextVelZ = velZ * 0.07000000029802322f;
    mTiltTargetZ = 0;
    mRideYOff = 0;
    velX += accelX;
    f32 nextZ = tz + nextVelZ;
    mTiltTargetX = 0;
    f32 nextVelX = velX * 0.07000000029802322f;
    mTiltZ = nextZ;
    mTiltVelZ = nextVelZ;
    f32 nextX = tx + nextVelX;
    mTiltVelX = nextVelX;
    mTiltX = nextX;
}
/* 02338DFC */
BOOL daObjDrift::Act_c::Execute(gptr<Mtx34>* matrix) {

    (this->*sModeProc.mModes[(u32)mMode])();
    set_current();
    set_mtx();
    *matrix = &mMtx;
    set_flower_current();
    return TRUE;
}

/* WWHD glue
data sFlowerOffset 104696B0
data sFlowerOffsetInitialized 104696BC
data sFlowerProfiles 100277C0
data sDrawBuffers 104B4634
data sZero 101FFBA8
data sSinCos 104A44F8
data sModeProc 10027814
data sHeader 10469694
data sDestructorRecords 101C8DDC
data sResourceManager 101F4F28
data sCylinderSource 10027840
ext 028E93CC void driftTranslate(Mtx34*, f32, f32, f32)
ext 02312548 void driftQuatRotBaseY2(DriftQuaternion*, cXyz*)
ext 025F25CC void driftQuatMatrix(DriftQuaternion*)
ext 025F1B48 void driftRotateZXY(Mtx34*, s16, s16, s16)
ext 028E90D4 void driftCopyMatrix(const Mtx34*, Mtx34*)
ext 028E8F64 void driftMultVec(const Mtx34*, cXyz*, cXyz*)
ext 025F232C void driftMtxToRot(Mtx34*, csXyz*)
ext 025204C8 void driftDeleteResource(request_of_phase_process_class*, const char*)
ext 026066C4 void* driftGetResource(void*, SafeString*, s32)
ext 0273AA24 void driftAssert(const char*, s32, const char*)
ext 025E38E0 J3DModel* driftCreateModel(void*, u32, u32)
ext 025D5834 u32 driftCreateActor(s16, u32, cXyz*, s8, csXyz*, u32, s32, u32)
ext 02555D0C void* driftGetEnvLight()
ext 025626A4 void driftSetLight(void*, s32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void driftApplyLight(void*, J3DModel*, dKy_tevstr_c*)
ext 025200D4 DriftGameInfo* driftGetGameInfo()
ext 025E2DE0 void driftUpdateModel(J3DModel*, u32)
ext 02520540 s32 driftGetReverb(s32)
ext 025E1A40 void driftStartSound(u32, cXyz*, u32, s32)
ext 025162A4 s32 driftCheckTgHit(dCcD_Cyl*)
ext 0251621C void driftClearTgHit(dCcD_Cyl*)
ext 025D69AC f32 driftPlayerDistanceSquared(Act_c*, fopAc_ac_c*)
ext 0201B3C0 void driftNormalizeZP(cXyz*, cXyz*)
ext 025165A4 void driftMoveCylinder(dCcD_Cyl*, cXyz*)
ext 0200E240 void driftRegisterCylinder(void*, dCcD_Cyl*)
ext 0200F564 void driftChaseAngle(be<s16>*, s16, s16)
ext 024F1D40 void driftConstructMoveBg(Act_c*)
ext 02515FB8 void driftConstructGObj(dCcD_Cyl*)
ext 02018590 void driftConstructCyl(cM3dGCyl*)
ext 02520460 s32 driftLoadResource(request_of_phase_process_class*, const char*)
ext 024F1D9C s32 driftCreateMoveBg(Act_c*, const char*, s32, u32, u32)
ext 028F026C void driftRegisterGlobal(void*)
ext 028ED6F8 void driftInitializeHeaderByte(void*)
ext 028EAB2C void driftInitializeHeaderFlag(void*)
ext 0273AF40 void driftDeleteObject(void*)
ext 02515A70 void driftDestroyCylinder(dCcD_Cyl*, s32)
ext 02515860 void driftDestroyStatus(dCcD_Stts*, s32)
ext 025D50BC void driftDestroyActor(Act_c*, s32)
ext 0201ADE0 void driftSubtract(cXyz*, cXyz*, cXyz*)
ext 028E91EC s32 driftInverse(Mtx34*, Mtx34*)
ext 028E9044 void driftMultVecSR(Mtx34*, cXyz*, cXyz*)
ext 028E8DD0 f32 driftSquaredMagnitude(cXyz*)
ext 025D674C void driftSetCullBox(Act_c*, f32, f32, f32, f32, f32, f32)
ext 02515F14 void driftInitializeStatus(dCcD_Stts*, s32, s32, Act_c*)
ext 02516518 void driftSetCylinder(dCcD_Cyl*, void*)
ext 025D9F70 s32 driftGetWaterY(cXyz*, be<f32>*)
ext 02019918 f32 driftRandomSigned(f32)
ext 023123C0 void driftPosMoveStream(Act_c*, u32, cXyz*, f32, f32)
str 10027738 in 023382AC
str 1002774C
str 10027790
str 100277A0 in 02338454
str 10027838
*/
