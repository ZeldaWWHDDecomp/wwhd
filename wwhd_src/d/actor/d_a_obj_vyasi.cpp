/** Palm tree blowing in the wind (Gale Isle).
 * Rewritten against the verified HD source.
 */
#include "d/actor/d_a_obj_vyasi.h"
#include "gc_api/d/d_kankyo_wether.h"
#include "gc_api/m_Do/m_Do_mtx.h"
namespace daObjVyasi {
using Actor = Act_c;
static JointKinds M_joint_type;
static Quaternion M_identity_quat;
static WindMatrixPointer M_wind_mtx;
static BaseAngles M_base_angle;
static u32 M_base_angle_guard;
static CylSource M_cyl_src;
static CpsSource M_cps_src;
static SphSource M_sph_src;
static cXyz M_default_hit_vec;
static ProcessTable M_process_init;
static ProcessTable M_process_main;
static MathStatics M_math;
static AngleLimits M_angle_limits;
static GlobalDescriptors M_descriptors;
void* VyasiStatus_construct(void*);
void* VyasiCapsule_construct(void*);
static s16 angle_float(f32 value) {
    return (s16)gabi::ftoi(value);
}
// HD: the resource lookup uses this TU SafeString vtable.
static void* resource(s32 index) {
    return dComIfG_getObjectRes("Vyasi", index, VT_RESOURCE);
}
static BOOL switch_on(Actor* a) {
    s32 index = vyasiPrmAbstract(a, 8, 0);
    return dComIfGs_isSwitch(index, a->home.roomNo);
}
static void hit_sound(Actor* actor, dCcD_GObjInf* collision) {
    collision->GetTgHitObj();
    daObj_HitSeStart(&actor->current.pos, actor->current.roomNo, collision, 7);
}
static void sound_position(Actor* actor) {
    gabi::Local<cXyz> position;
    f32 y = actor->current.pos.y, z = actor->current.pos.z, x = actor->current.pos.x;
    position->set(x, y, z);
    dKy_Sound_set(position.get(), 4, actor->mBsPcId, 0x64);
}
/* 023B8FDC */
BOOL Act_c::SetStopJointAnimation(void* animation, f32 speed, f32 morph) {
    if (!animation)
        return 0;
    mpMorf->setAnm(static_cast<J3DAnmTransform*>(animation), 0, morph, speed, 0.0f, -1.0f, nullptr);
    m19C4 = 1;
    return 1;
}

/* 023B7F4C */
BOOL Act_c::PlayStopJointAnimation() {
    return mpMorf->play(nullptr, 0, 0) == 0;
}

/* 023B75E0 */
void Act_c::set_first_process() {
    process_init(switch_on(this) ? STATE_NORMAL : STATE_SAG);
    s32 yaw = s16(shape_angle.y) - 0x8000;
    mNormalCounter = 0;
    shape_angle.y = (s16)yaw;
    m19D4 = 1;
}

/* 023B7FDC */
void Act_c::set_collision() {
    if (mCyl.ChkTgHit()) {
        hit_sound(this, &mCyl);
        daObj_HitEff_kikuzu(this, &mCyl);
        sound_position(this);
        mCyl.ClrTgHit();
    } else {
        mCyl.SetR(79.0f);
        mCyl.SetH(250.0f);
        mCyl.SetC(&current.pos);
        cCcS_Set(dComIfG_Ccsp(), &mCyl);
    }
    f32 radius = 47.400001525878906f;
    for (u32 i = 0; i < 5; ++i) {
        Capsule* collision = &mCapsules[i];
        if (collision->ChkTgHit()) {
            hit_sound(this, collision);
            sound_position(this);
            collision->ClrTgHit();
        } else {
            CapsuleEnds* shape = &mCapsuleEnds[i];
            shape->mStart.copy(m0400[i]);
            shape->mEnd.copy(m0400[i + 1]);
            shape->mRadius = radius;
            vyasiSetCapsuleGeometry(&collision->mCps, shape);
            cCcS_Set(dComIfG_Ccsp(), collision);
        }
    }
    for (u32 i = 0; i < 4; ++i) {
        cXyz* start = &m0400[i + 1];
        cXyz* end = &m0400[i + 2];
        f32 x = start->x, y = start->y, z = start->z;
        f32 dx = (f32(end->x) - x) * 0.3333300054073334f;
        f32 dy = (f32(end->y) - y) * 0.3333300054073334f;
        f32 dz = (f32(end->z) - z) * 0.3333300054073334f;
        gabi::Local<cXyz> center;
        center->set(x + dx, y + dy, z + dz);
        dCcD_Sph* sphere = &mSpheres[i * 2];
        sphere->mSph.SetC(center.get());
        sphere->mSph.SetR(radius);
        cCcS_Set(dComIfG_Ccsp(), sphere);
        center->set(f32(start->x) + (dx + dx), f32(start->y) + (dy + dy),
                    f32(start->z) + (dz + dz));
        ++sphere;
        sphere->mSph.SetC(center.get());
        sphere->mSph.SetR(radius);
        cCcS_Set(dComIfG_Ccsp(), sphere);
    }
}

/* 023B775C */
BOOL JointNodeCallBack(void* node, s32 stage) {
    J3DModel* model = j3dSys.getModel();
    u32 index = reinterpret_cast<J3DJoint*>(node)->getJntNo();
    Actor* actor = static_cast<Actor*>(model->getUserArea());
    if (stage != 0)
        return 1;
    if (index >= 14)
        JUT_ASSERT_fail("d_a_obj_vyasi.cpp", 0x373, "no >= 0 && no < (14)");
    Mtx34* stack = mDoMtx_stack_c::get();
    PSMTXCopy(model->getAnmMtx(index), stack);
    gabi::Local<Mtx34> matrix;
    PSMTXCopy(model->getAnmMtx(index), matrix.get());
    f32 x = matrix->m[0][3], y = matrix->m[1][3], z = matrix->m[2][3];
    matrix->m[2][3] = 0;
    matrix->m[0][3] = 0;
    matrix->m[1][3] = 0;
    PSMTXTrans(stack, x, y, z);
    mDoMtx_stack_c::quatM(&actor->mJointQuat[index]);
    PSMTXConcat(stack, matrix.get(), stack);
    model->mpMtxBuffer->mFlags |= 0x10;
    // HD: preserve the existing dirty-flag write while mapping the typed matrix array.
    J3DModel::mtx_assign(gabi::at<Mtx34>(model->mpMtxBuffer->mpAnmMtx + index * 48), stack);
    PSMTXCopy(stack, J3DSys::mCurrentMtx);
    gabi::Local<csXyz[14]> angles;
    for (u32 i = 0; i < 14; ++i)
        csXyz_ct(&(*angles)[i], 0, 0, 0);
    gabi::Local<csXyz> rotation;
    rotation->x = (*angles)[index].x;
    rotation->y = (*angles)[index].y;
    rotation->z = (*angles)[index].z;
    vyasiAddAngles(rotation.get(), &actor->m03AC[index]);
    PSMTXCopy(model->getAnmMtx(index), stack);
    mDoMtx_ZXYrotM(stack, rotation->x, rotation->y, rotation->z);
    if (M_joint_type.entries[index] == 0)
        mDoMtx_stack_c::scaleM(actor->m04A8, actor->m04AC, actor->m04B0);
    J3DMtxBuffer_hd* data = model->mpMtxBuffer;
    u16 flags = data->mFlags;
    u32 base = data->mpAnmMtx;
    data->mFlags = flags | 0x10;
    J3DModel::mtx_assign(gabi::at<Mtx34>(base + index * 48), stack);
    PSMTXCopy(stack, J3DSys::mCurrentMtx);
    gabi::Local<cXyz> origin;
    origin->set(0, 0, 0);
    PSMTXMultVec(stack, origin.get(), &actor->m0400[index]);
    return 1;
}

/* 023B9848 */
BOOL Act_c::process_none_init() {
    return 1;
}

/* 023B9850 */
void Act_c::process_none_main() {}

/* 023B9058 */
BOOL Act_c::process_sag_init() {
    if (!SetStopJointAnimation(mpBckData.get(), 1.f, 0.f))
        return 0;
    mpMorf->mFrameCtrl.mRate = 0.0f;
    return 1;
}

/* 023B90EC */
void Act_c::process_sag_main() {
    gabi::Local<be<u16>> profile;
    *profile = PROFILE_EKSZS;
    fopAc_ac_c* source = fopAcIt_Judge(VYASI_PROFILE_JUDGE, profile.get());
    if (source) {
        mEkszsPos.copy(source->current.pos);
        mEkszsRotY = source->shape_angle.y;
        process_init(STATE_SAG_WIND);
    }
}

/* 023B9160 */
BOOL Act_c::process_sagWind_init() {
    if (!SetStopJointAnimation(mpBckData.get(), 1.f, 3.f))
        return 0;
    // HD: this unit rounds the squared distance to float before sqrt.
    f32 distance = PSVECSquareDistance(&mEkszsPos, &current.pos);
    distance = std_sqrtf(distance);
    // Ordered PPC comparisons also retain NaNs in the unclamped branch.
    if (distance > 2800.f)
        distance = 2800.f;
    else if (distance < 1000.f)
        distance = 1000.f;
    f32 amplitude = (distance - 2800.f) / -1800.f;
    f32 velocity = gabi::fmadds(7000.f, amplitude, 5000.f);
    m0504 = amplitude;
    s16 half = angle_float(velocity * 0.5f);
    for (u32 i = 0; i < 14; ++i) {
        s16 value = half;
        if (u8(M_joint_type.entries[i]) == 0) {
            f32 random = cM_rndF(2000.0f);
            f32 speed = velocity + random;
            value = angle_float((i & 1) ? -speed : speed);
        }
        m0524[i] = value;
    }
    mpMorf->mFrameCtrl.mRate = 0.0f;
    return 1;
}

/* 023B9390 */
void Act_c::process_sagWind_main() {
    if (switch_on(this))
        process_init(STATE_TO_NORMAL);
}

/* 023B93F4 */
BOOL Act_c::process_toNormal_init() {
    return SetStopJointAnimation(mpBckData.get(), 1.f, 0.f);
}

/* 023B940C */
void Act_c::process_toNormal_main() {
    if ((u32)m19C4 == 0) {
        f32 amplitude = (f32)m19CC;
        if (!(std::fabs(amplitude) > 0.10000000149011612f)) {
            if (process_init(STATE_NORMAL)) {
                mNormalCounter = 2;
                m19D4 = 0.f;
                m19CC = 0.f;
            }
            amplitude = (f32)m19CC;
        }
        s32 phase = (s16)m19D0 + 0x3000;
        m19D0 = phase;
        m19CC = amplitude * 0.8500000238418579f;
    } else {
        f32 amplitude = -1792.f * (f32)m0504;
        m19D0 = 0;
        m19CC = amplitude;
    }
}

/* 023B94D4 */
BOOL Act_c::process_normal_init() {
    if (!SetStopJointAnimation(mpBckData.get(), -1.f, 0.f))
        return 0;
    mpMorf->mFrameCtrl.mRate = 0.0f;
    return 1;
}

/* 023B9568 */
f32 Act_c::process_normal_main() {
    s16 counter = (s16)mNormalCounter;
    if (counter == 0 || counter == 1)
        mNormalCounter = counter + 1;
    return vyasiAddCalc(&m19D4, 1.f, 0.009999999776482582f, 1.f, 0.007000000216066837f);
}

/* 023B7528 */
BOOL Act_c::process_init(s32 state) {
    if ((u32)state >= STATE_COUNT)
        return 0;
    if (!M_process_init.entries[state].invoke<BOOL>(this))
        return 0;
    mState = state;
    return 1;
}

/* 023B7F84 */
void Act_c::process_main() {
    u32 state = mState;
    if (state >= STATE_COUNT)
        return;
    M_process_main.entries[state].invoke<void>(this);
}

/* 023B7524 */
u8 solidHeapCB(Actor* a) {
    return a->create_heap();
}

/* 023B73EC */
u8 Act_c::create_heap() {
    J3DModelData* data = static_cast<J3DModelData*>(resource(RES_MODEL));
    if (!data)
        JUT_ASSERT_fail("d_a_obj_vyasi.cpp", 0x47b, "mdl_data != 0");
    J3DAnmTransformKey* animation = static_cast<J3DAnmTransformKey*>(resource(RES_BCK));
    mpBckData = animation;
    if (!animation) {
        JUT_ASSERT_fail("d_a_obj_vyasi.cpp", 0x480, "M_bck_data != 0");
        animation = mpBckData;
    }
    if (animation && data) {
        mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr,
                                                      reinterpret_cast<J3DAnmTransform*>(animation),
                                                      0, 1.0f, 0, -1, 1, nullptr, 0, 0x11000002);
        animation = mpBckData;
        mpMorf = morf;
    }
    if (!animation)
        return 0;
    mDoExt_McaMorf* morf = mpMorf;
    return morf && morf->getModel() != nullptr;
}

/* 023B7B3C */
s32 Act_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        // HD: inline GHS construction retains each vtable write and array callback token.
        if (this) {
            vyasiActorConstruct(this);
            __vtbl = VT_ACTOR;
            dCcD_Stts_ct(&mStts);
            vyasiHitInfoConstruct(&mCyl);
            CapsuleShapeAttr* shape = reinterpret_cast<CapsuleShapeAttr*>(mCyl.mShapeAttr);
            shape->__vtbl = VT_SHAPE;
            shape->boundsVtable = VT_BOUNDS;
            vyasiCylinderGeometryConstruct(&mCyl.mCyl);
            shape->__vtbl = VT_CYL_SHAPE;
            mCyl.__vtbl_hitinf = VT_CYL_HIT;
            mCyl.mCyl.__vtbl = VT_CYL;
            vyasiConstructArray(mCapsuleStts, 5, sizeof(dCcD_Stts),
                                gabi::guest_fn((const void*)VyasiStatus_construct));
            vyasiConstructArray(mCapsules, 5, sizeof(Capsule),
                                gabi::guest_fn((const void*)VyasiCapsule_construct));
            vyasiConstructArray(mSphereStts, 8, sizeof(dCcD_Stts),
                                gabi::guest_fn((const void*)VyasiStatus_construct));
            vyasiConstructArray(mSpheres, 8, sizeof(dCcD_Sph), VYASI_SPHERE_CONSTRUCTOR);
            flags = actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&mPhs, "Vyasi");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)solidHeapCB), 0))
        return cPhs_ERROR_e;
    set_first_process();
    set_mtx();
    J3DModel* model = mpMorf->getModel();
    cullMtx = gabi::ea(model ? &model->mBaseTransformMtx : nullptr);
    fopAcM_setCullSizeBox(this, -2000.0f, 0.0f, -2000.0f, 2000.0f, 2000.0f, 2000.0f);
    cullSizeFar = 2;
    mStts.Init(255, 255, this);
    mCyl.Set(reinterpret_cast<const dCcD_SrcCyl*>(&M_cyl_src));
    mCyl.SetStts(&mStts);
    u32 objectFlags = mCyl.mGObjTg.mSPrm;
    // HD: these are word copies, with the flag store between the second and third component.
    reinterpret_cast<be<u32>&>(mCyl.mGObjTg.mVec.x) =
        reinterpret_cast<const be<u32>&>(M_default_hit_vec.x);
    reinterpret_cast<be<u32>&>(mCyl.mGObjTg.mVec.y) =
        reinterpret_cast<const be<u32>&>(M_default_hit_vec.y);
    mCyl.mGObjTg.mSPrm = objectFlags | 4;
    reinterpret_cast<be<u32>&>(mCyl.mGObjTg.mVec.z) =
        reinterpret_cast<const be<u32>&>(M_default_hit_vec.z);
    for (u32 i = 0; i < 5; ++i) {
        dCcD_Stts* status = &mCapsuleStts[i];
        Capsule* collision = &mCapsules[i];
        status->Init(100, 255, this);
        vyasiSetCapsule(collision, &M_cps_src);
        collision->SetStts(status);
        CapsuleEnds* shape = &mCapsuleEnds[i];
        shape->mStart.copy(current.pos);
        shape->mEnd.copy(current.pos);
        shape->mRadius = 100;
    }
    for (u32 i = 0; i < 8; ++i) {
        dCcD_Stts* status = &mSphereStts[i];
        dCcD_Sph* collision = &mSpheres[i];
        status->Init(100, 255, this);
        collision->Set(reinterpret_cast<const dCcD_SrcSph*>(&M_sph_src));
        collision->SetStts(status);
        objectFlags = mCyl.mGObjTg.mSPrm;
        reinterpret_cast<be<u32>&>(mCyl.mGObjTg.mVec.x) =
            reinterpret_cast<const be<u32>&>(M_default_hit_vec.x);
        reinterpret_cast<be<u32>&>(mCyl.mGObjTg.mVec.y) =
            reinterpret_cast<const be<u32>&>(M_default_hit_vec.y);
        mCyl.mGObjTg.mSPrm = objectFlags | 4;
        reinterpret_cast<be<u32>&>(mCyl.mGObjTg.mVec.z) =
            reinterpret_cast<const be<u32>&>(M_default_hit_vec.z);
    }
    model = mpMorf->getModel();
    J3DModelData* data = model->getModelData();
    model->mUserArea = gabi::ea(this);
    u16 jointCount = data->getJointNum();
    for (u32 i = 0; i < jointCount; i = (u16)(i + 1)) {
        data->getJointNodePointer(i)->setCallBack((J3DJointCallBack)JointNodeCallBack);
        jointCount = model->getModelData()->getJointNum();
    }
    for (u32 i = 0; i < 14; ++i)
        mJointQuat[i] = M_identity_quat;
    m04A8 = 1;
    m04AC = 1;
    m04B0 = 1;
    return phase;
}

/* 023B7F1C */
u8 Act_c::_delete() {
    dComIfG_resDelete(&mPhs, "Vyasi");
    return 1;
}

/* 023B7668 */
void Act_c::set_mtx() {
    J3DModel* model = mpMorf->getModel();
    f32 z = scale.z, x = scale.x, y = scale.y;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    model = mpMorf->getModel();
    model->setBaseTRMtx(mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &m04BC);
}

/* 023B86B8 */
void Act_c::calc_dif_angle() {
    gabi::Local<csXyz> desired;
    for (u32 i = 0; i < 14; ++i) {
        csXyz_ct(desired.get(), 0, 0, 0);
        s32 state = mState, rate = 2;
        if (state == STATE_SAG_WIND) {
            u8 kind = M_joint_type.entries[i];
            if (kind == 2) {
                f32 wave = f32(m0504) * cM_ssin(u16(mAnglePhase[i]));
                desired->x = angle_float(20.0f * wave);
                s16 yz = angle_float(40.0f * wave);
                desired->y = yz;
                desired->z = yz;
            } else if (kind == 1) {
                f32 amplitude = m0504;
                f32 wave = amplitude * cM_ssin(u16(mAnglePhase[i]));
                desired->x = angle_float(120.0f * wave);
                desired->y = angle_float(180.0f * wave);
                s32 z = angle_float(220.0f * wave);
                if (i == 1)
                    z += angle_float(gabi::fmadds(3200.0f, amplitude, -3200.0f));
                desired->z = (s16)z;
            } else if (kind == 0) {
                // HD: GHS initializes the shared bend angles once, setting its guard first.
                if (!M_base_angle_guard) {
                    M_base_angle_guard = 1;
                    const s32 z[14] = {0, 0, 0, 0, 0, 0, 0, 0, 5000, 0, 0, -5000, -7000, -2700};
                    for (u32 j = 0; j < 14; ++j)
                        csXyz_ct(&M_base_angle.entries[j], 0, 0, z[j]);
                }
                f32 wave = f32(m0504) * cM_ssin(u16(mAnglePhase[i]));
                const csXyz& base = M_base_angle.entries[i];
                s32 x = angle_float(700.0f * wave) + s16(base.x);
                s16 yz = angle_float(1700.0f * wave);
                desired->x = (s16)x;
                desired->y = (s16)(s32(yz) + s16(base.y));
                desired->z = (s16)(s32(yz) + s16(base.z));
                rate = 1;
            }
        } else if (state == STATE_TO_NORMAL && m19C4 == 0 && (i == 0 || i == 1 || i == 6)) {
            desired->z = angle_float(f32(m19CC) * cM_ssin(u16(m19D0)));
        }
        csXyz* rotation = &m03AC[i];
        cLib_addCalcAngleS2(&rotation->x, desired->x, rate, 0x4000);
        cLib_addCalcAngleS2(&rotation->y, desired->y, rate, 0x4000);
        cLib_addCalcAngleS2(&rotation->z, desired->z, rate, 0x4000);
        u8 kind = M_joint_type.entries[i];
        s32 increment = m0524[i];
        if (kind == 0)
            increment = angle_float(f32(increment) * 1.5f);
        mAnglePhase[i] = (s16)(s32(mAnglePhase[i]) + increment);
    }
}

/* 023B8388 */
void Act_c::quaternion_main() {
    for (u32 i = 0; i < 14; ++i) {
        gabi::Local<Quaternion> target;
        *target = M_identity_quat;
        s32 state = mState;
        if (state == STATE_NORMAL && M_joint_type.entries[i] == 0) {
            gabi::Local<cXyz> axis;
            axis->set(0, 1, 0);
            mDoMtx_YrotS(M_wind_mtx.mMatrix, (s16)-s16(current.angle.y));
            cXyz* wind = dKyw_get_wind_vec();
            gabi::Local<cXyz> localWind;
            MtxPosition(wind, localWind.get());
            f32 power = dKyw_get_wind_pow();
            gabi::Local<cXyz> cross;
            // HD: keep the hidden result in the original caller-owned slot.
            cXyz_outprod(axis.get(), cross.get(), localWind.get());
            f32 scale = m19D4;
            u16 angle = (u16)gabi::ftoi((1400.0f * power) * scale);
            f32 sine = cM_ssin(angle);
            gabi::Local<Quaternion> rotation;
            rotation->x = sine * f32(cross->x);
            rotation->y = sine * f32(cross->y);
            rotation->z = sine * f32(cross->z);
            rotation->w = cM_scos(angle);
            s16 desired = angle_float((power * 360.0f) * scale);
            cLib_addCalcAngleS2(&m02B0[i], desired > 220 ? 220 : desired, 4, 0x20);
            f32 random = cM_rndFX(256.0f);
            f32 bend = m19D4;
            s16 delta = angle_float(gabi::fmadds(power * 2048.0f, bend, random));
            s32 phase = s16(m0294[i]) + delta;
            m0294[i] = (s16)phase;
            u16 windAngle = m02B0[i];
            f32 wave = cM_ssin(windAngle) * cM_ssin((u16)phase);
            gabi::Local<Quaternion> oscillation;
            oscillation->x = wave;
            oscillation->y = 0;
            oscillation->z = wave;
            oscillation->w = cM_scos(windAngle);
            PSQUATMultiply(rotation.get(), oscillation.get(), target.get());
        }
        if (mNormalCounter == 1)
            mJointQuat[i] = *target;
        else
            mDoMtx_quatSlerp(&mJointQuat[i], target.get(), &mJointQuat[i], 0.4000000059604645f);
    }
}

/* 023B8DC8 */
void Act_c::leaf_scale_main() {
    f32 x = 1, y = 1, z = 1;
    if (mState == STATE_SAG_WIND) {
        f32 amplitude = m0504;
        y = gabi::fmadds(-0.5f, amplitude, 1.0f);
        z = y;
        x = gabi::fmadds(0.3500000238418579f, amplitude, 1.0f);
    }
    cLib_addCalc2(&m04A8, x, 0.5f, 0.5f);
    cLib_addCalc2(&m04AC, y, 0.5f, 0.5f);
    cLib_addCalc2(&m04B0, z, 0.5f, 0.5f);
}

/* 023B8EB0 */
BOOL Act_c::_execute() {
    if (mState != STATE_NONE) {
        m19C4 = PlayStopJointAnimation();
        process_main();
        set_collision();
        quaternion_main();
        calc_dif_angle();
        leaf_scale_main();
        set_mtx();
        fopAcM_rollPlayerCrash(this, 7, 79.0f);
    }
    return 1;
}

/* 023B8F38 */
BOOL Act_c::_draw() {
    if (mState != STATE_NONE) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        settingTevStruct(env, 1, &current.pos, &tevStr);
        mDoExt_McaMorf* morf = mpMorf;
        env = dKy_getEnvlight();
        setLightTevColorType(env, morf->getModel(), &tevStr);
        dComIfGd_setListBG();
        mpMorf->updateDL();
        dComIfGd_setList();
    }
    return 1;
}

/* 023B95A8 */
s32 Mthd_Create(Actor* a) {
    return a->_create();
}

/* 023B95AC */
u8 Mthd_Delete(Actor* a) {
    return a->_delete();
}

/* 023B95B0 */
BOOL Mthd_Execute(Actor* a) {
    return a->_execute();
}

/* 023B95B4 */
BOOL Mthd_Draw(Actor* a) {
    return a->_draw();
}

/* 023B9854 */
BOOL Mthd_IsDelete(Actor* a) {
    return 1;
}

// HD: compiler-generated and template code of this translation unit.

/* 023B95B8 */
void __sinit_d_a_obj_vyasi_cpp() {
    M_math.mZero[2] = 0;
    M_math.mZero[0] = 0;
    M_math.mZero[3] = 0;
    M_math.mZero[1] = 0;
    __register_global_object(gabi::ea(M_descriptors.entries[0]));
    f32 low = M_angle_limits.mMin, high = M_angle_limits.mMax;
    M_math.mMinAngle = low;
    M_math.mMaxAngle = high;
    vyasiConstructSinTable(&M_math.mSinTable);
    __register_global_object(gabi::ea(M_descriptors.entries[1]));
    vyasiConstructAtanTable(&M_math.mAtanTable);
    __register_global_object(gabi::ea(M_descriptors.entries[2]));
}

/* 023B964C */
void VyasiDescriptor_destruct(void* object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 023B9660 */
void* VyasiStatus_construct(void* object) {
    dCcD_Stts* status = static_cast<dCcD_Stts*>(object);
    if (!status) {
        status = static_cast<dCcD_Stts*>(operator_new(sizeof(dCcD_Stts)));
        if (!status)
            return nullptr;
    }
    dCcD_Stts_ct(status);
    return status;
}

/* 023B96C8 */
void* VyasiCapsule_construct(void* object) {
    Capsule* capsule = static_cast<Capsule*>(object);
    if (!capsule) {
        capsule = static_cast<Capsule*>(operator_new(sizeof(Capsule)));
        if (!capsule)
            return nullptr;
    }
    vyasiHitInfoConstruct(capsule);
    capsule->mShape.__vtbl = VT_SHAPE;
    capsule->mShape.boundsVtable = VT_BOUNDS;
    vyasiCapsuleGeometryConstruct(&capsule->mCps);
    capsule->__vtbl_hitinf = VT_CAPSULE_HIT;
    capsule->mCps.__vtbl = VT_CAPSULE;
    capsule->mShape.__vtbl = VT_CAPSULE_SHAPE;
    return capsule;
}

/* 023B9754 */
void VyasiCapsule_dummy(void* object) {}

/* 023B9758 */
void Act_destruct(Actor* a, u32 flags) {
    if (!a)
        return;
    vyasiDestructArray(a->mSpheres, 8, sizeof(dCcD_Sph), VYASI_SPHERE_DESTRUCTOR, 0, 0);
    vyasiDestructArray(a->mSphereStts, 8, sizeof(dCcD_Stts), VYASI_STATUS_DESTRUCTOR, 0, 0);
    vyasiDestructArray(a->mCapsules, 5, sizeof(Capsule), VYASI_CAPSULE_DESTRUCTOR, 0, 0);
    vyasiDestructArray(a->mCapsuleStts, 5, sizeof(dCcD_Stts), VYASI_STATUS_DESTRUCTOR, 0, 0);
    dCcD_Cyl_dt(&a->mCyl, 2);
    dCcD_Stts_dt(&a->mStts, 2);
    vyasiActorDestruct(a, 0);
    if (flags & 1)
        operator_delete(a);
}

} // namespace daObjVyasi

using namespace daObjVyasi;

/* WWHD glue
data M_math 1046C96C
data M_angle_limits 100335AC
data M_descriptors 101CDD54
ext 025D50BC void vyasiActorDestruct(fopAc_ac_c*, s32)
ext 028F0164 void vyasiDestructArray(void*, s32, s32, u32, s32, s32)
ext 02018150 void vyasiCapsuleGeometryConstruct(daObjVyasi::CapsuleGeometry*)
ext 028EAB2C void vyasiConstructAtanTable(u8*)
ext 028ED6F8 void vyasiConstructSinTable(u8*)
data M_process_init 1003348C
data M_process_main 100334F4
ext 0200ECD4 f32 vyasiAddCalc(be<f32>*, f32, f32, f32, f32)
ext 028E8DE8 f32 PSVECSquareDistance(const cXyz*, const cXyz*)
ext 025D69FC void fopAcM_rollPlayerCrash(fopAc_ac_c*, s32, f32)
ext 02349DFC s32 vyasiPrmAbstract(const fopAc_ac_c*, u32, u32)
data M_cyl_src 100333C8
data M_cps_src 1003340C
data M_sph_src 10033388
data M_default_hit_vec 101FFBA8
str 100335B8 "Vyasi"
ext 025164C0 void vyasiSetCapsule(daObjVyasi::Capsule*, const void*)
ext 028EFFD0 void vyasiConstructArray(void*, s32, s32, u32)
ext 02018590 void vyasiCylinderGeometryConstruct(cM3dGCyl*)
ext 02515FB8 void vyasiHitInfoConstruct(void*)
ext 025D4ED0 void vyasiActorConstruct(fopAc_ac_c*)
ext 020181FC void vyasiSetCapsuleGeometry(daObjVyasi::CapsuleGeometry*, const daObjVyasi::CapsuleEnds*)
ext 0255F458 void dKy_Sound_set(cXyz*, s32, u32, s32)
ext 02312C8C void daObj_HitEff_kikuzu(fopAc_ac_c*, dCcD_Cyl*)
ext 023129C4 void daObj_HitSeStart(cXyz*, s8, dCcD_GObjInf*, s32)
data M_base_angle 1046C988
data M_base_angle_guard 1046C9DC
data M_wind_mtx 1018C7B0
ext 0201B080 void cXyz_outprod(const cXyz*, cXyz*, const cXyz*)
ext 028E8B48 void PSQUATMultiply(const Quaternion*, const Quaternion*, Quaternion*)
ext 028E9108 void PSMTXConcat(const Mtx34*, const Mtx34*, Mtx34*)
data M_joint_type 101CDD44
data M_identity_quat 101E9C38
ext 0201A554 void vyasiAddAngles(csXyz*, const csXyz*)
str 10033468 "d_a_obj_vyasi.cpp" in 023B73EC
str 1003347C "mdl_data != 0"
str 10033458 "M_bck_data != 0"
str 100334B8 "d_a_obj_vyasi.cpp" in 023B775C
str 100334CC "no >= 0 && no < (14)"
*/
