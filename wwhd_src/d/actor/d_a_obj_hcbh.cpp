// Tower of the Gods pillar.
#include "d/actor/d_a_obj_hcbh.h"
#include <cmath>
const gc_tu_data gc_tu = {
    .safeString = 0x1002A0C8,
    .objAcch = {0x1002A0F0, 0x1002A110, 0x1002A100},
    .objAcchDt = {0x1002A100, 0x1002A110},
};
// HD: private data and word-copy views for layouts not supplied by tww.
struct HcbhFragmentData {
    be<s32> mModelIndices[4];
    be<s16> mYaws[4];
};
struct HcbhSphereCenters {
    cXyz mValues[6];
};
struct HcbhWordVec {
    be<u32> x, y, z;
};
struct HcbhVtable {
    be<u32> mEntry;
};
struct HcbhHeaderStatics {
    be<f32> mPiLo, mPiHi, mHeight;
    be<u8> mCtor1, mCtor2;
    u8 _0E[2];
    be<u32> mZero[4];
};
struct HcbhDtorRecord {
    be<u32> mWords[3];
};
struct HcbhDtorRecords {
    HcbhDtorRecord mValues[3];
};
static HcbhFragmentData l_fragments;
static HcbhSphereCenters l_sphereCenters;
static HcbhVtable l_actorVtable;
static HcbhVtable l_aabVtable;
static HcbhHeaderStatics l_headerStatics;
static HcbhDtorRecords l_dtorRecords;
static dCcD_SrcCyl l_cyl_src;
static dCcD_SrcSph l_sph_src;
namespace HcbhEffect {
enum { PILLAR_DUST = 0x82DD, PILLAR_CHIPS = 0x82DE, PILLAR_FRAGMENTS = 0x82DF, PILLAR_SMOKE = 0xA2E0 };
}
namespace HcbhAttack {
enum { AT_TYPE_UNK8 = 8, AT_TYPE_BOMB = 0x20, AT_TYPE_SKULL_HAMMER = 0x10000, AT_TYPE_DARKNUT_SWORD = 0x4000000 };
}
// HD: these comparisons use mProfName, not the actor's mProcName.
enum {
    STATUE_PROFILE = 0xCA,
    HEAVY_WEAPON_PROFILE = 0xBF,
    JA_SE_OBJ_HCBH_BREAK = 0x6A10,
    JA_SE_OBJ_HCBH_FALL = 0x6A11
};
using HcbhSphConstructor = void (*)(dCcD_Sph*);
using HcbhSphDestructor = void (*)(dCcD_Sph*, s32);
u32 Hcbh_PrmAbstract(daObjHcbh_c*, u32, u32);
static BOOL solidHeapCB(fopAc_ac_c*);
void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16);
void Hcbh_SphSetC(cM3dGSph*, f32, f32, f32);
void Hcbh_SphMoveCAt(dCcD_Sph*, cXyz*);
void Hcbh_constructSpheres(dCcD_Sph*, u32, u32, HcbhSphConstructor);
void Hcbh_destroySpheres(dCcD_Sph*, u32, u32, HcbhSphDestructor, u32, u32);
void Hcbh_sphCtor(dCcD_Sph*);
void Hcbh_sphDtor(dCcD_Sph*, s32);
void Hcbh_waitProc(daObjHcbh_c*);
void Hcbh_fallProc(daObjHcbh_c*);
void Hcbh_headerCtor1(be<u8>*);
void Hcbh_headerCtor2(be<u8>*);
// HD: external callbacks need private address registration.
// The two action aliases supply guest addresses for GHS member-pointer storage.
static gabi::HostFn l_sphCtorBinding(reinterpret_cast<const void*>(Hcbh_sphCtor), 0x025166F0);
static gabi::HostFn l_sphDtorBinding(reinterpret_cast<const void*>(Hcbh_sphDtor), 0x02515AE8);
static gabi::HostFn l_waitBinding(reinterpret_cast<const void*>(Hcbh_waitProc), 0x02352BFC);
static gabi::HostFn l_fallBinding(reinterpret_cast<const void*>(Hcbh_fallProc), 0x02352CA0);

static void transformPillar(daObjHcbh_c* actor, f32 pivot) {
    f32 x = actor->current.pos.x, y = actor->mPillarHeight, z = actor->current.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
    s16 yaw = actor->mBreakYaw;
    mDoMtx_stack_c::YrotM(yaw);
    mDoMtx_stack_c::transM(0.0f, pivot, 0.0f);
    s16 angle = actor->mPillarAngle;
    mDoMtx_stack_c::XrotM(angle);
    mDoMtx_stack_c::transM(0.0f, -pivot, 0.0f);
    yaw = actor->mBreakYaw;
    mDoMtx_stack_c::YrotM(s16(-yaw));
    s16 rx = actor->shape_angle.x, ry = actor->shape_angle.y, rz = actor->shape_angle.z;
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), rx, ry, rz);
}
static void releaseBackground(daObjHcbh_c* actor, unsigned index) {
    dBgW* background = actor->mBackground[index];
    if (background && background->ChkUsed()) {
        auto* bg = dComIfG_Bgsp();
        background = actor->mBackground[index];
        bg->Release(background);
    }
}
/* 02351C10 */
s32 daObjHcbh_c::chk_appear() {
    u32 bit = Hcbh_PrmAbstract(this, 8, 13);
    if (bit == 255)
        return TRUE;
    s8 room = home.roomNo;
    return dComIfGs_isSwitch(bit, room) == 0;
}

/* 02351E3C */
void daObjHcbh_c::set_mtx() {
    f32 pivot = 50.0f * cM_ssin(u16(mPillarAngle));
    transformPillar(this, pivot);
    mModel->setBaseTRMtx(mDoMtx_stack_c::get());
    for (unsigned i = 0; i < 4; ++i) {
        f32 x = mFragmentPositions[i].x, y = mFragmentPositions[i].y, z = mFragmentPositions[i].z;
        mDoMtx_stack_c::transS(x, y, z);
        s16 yaw = mFragmentYaws[i];
        mDoMtx_stack_c::YrotM(yaw);
        s16 angle = mFragmentAngles[i];
        mDoMtx_stack_c::XrotM(angle);
        yaw = mFragmentYaws[i];
        mDoMtx_stack_c::YrotM(s16(-yaw));
        s16 rx = shape_angle.x, ry = shape_angle.y, rz = shape_angle.z;
        mDoMtx_XYZrotM(mDoMtx_stack_c::get(), rx, ry, rz);
        mFragmentModels[i]->setBaseTRMtx(mDoMtx_stack_c::get());
    }
}

/* 02352074 */
void daObjHcbh_c::init_mtx() {
    // HD: integer copies retain scale NaN payloads and the original store order.
    auto* scaleWords = reinterpret_cast<HcbhWordVec*>(&scale);
    u32 y = scaleWords->y, x = scaleWords->x;
    J3DModel* model = mModel;
    u32 z = scaleWords->z;
    auto* modelScale = reinterpret_cast<HcbhWordVec*>(&model->mBaseScale);
    modelScale->y = y;
    modelScale->x = x;
    modelScale->z = z;
    for (unsigned i = 0; i < 4; ++i) {
        model = mFragmentModels[i];
        z = scaleWords->z;
        x = scaleWords->x;
        y = scaleWords->y;
        modelScale = reinterpret_cast<HcbhWordVec*>(&model->mBaseScale);
        modelScale->x = x;
        modelScale->z = z;
        modelScale->y = y;
    }
    set_mtx();
}

/* 02351E38 */
static BOOL solidHeapCB(fopAc_ac_c* base) {
    return static_cast<daObjHcbh_c*>(base)->create_heap();
}

/* 02351C74 */
s32 daObjHcbh_c::create_heap() {
    auto* resource = static_cast<J3DModelData*>(dComIfG_getObjectRes("Hcbh", 8, gc_tu.safeString));
    if (!resource) {
        JUT_ASSERT_fail("d_a_obj_hcbh.cpp", 0x1F3, "0");
        return FALSE;
    }
    s32 result = TRUE;
    mModel = mDoExt_J3DModel__create(resource, 0x80000, 0x11000022);
    for (unsigned i = 0; i < 4; ++i) {
        s32 resourceIndex = l_fragments.mModelIndices[i];
        resource = static_cast<J3DModelData*>(dComIfG_getObjectRes("Hcbh", resourceIndex, gc_tu.safeString));
        if (!resource) {
            result = FALSE;
            break;
        }
        mFragmentModels[i] = mDoExt_J3DModel__create(resource, 0x80000, 0x11000022);
    }
    auto* dzb = static_cast<cBgD_t*>(dComIfG_getObjectRes("Hcbh", 12, gc_tu.safeString));
    J3DModel* model = mModel;
    mBackground[0] = dBgW_NewSet(dzb, dBgW::MOVE_BG_e, model ? model->getBaseTRMtx() : nullptr);
    dzb = static_cast<cBgD_t*>(dComIfG_getObjectRes("Hcbh", 11, gc_tu.safeString));
    model = mFragmentModels[0];
    dBgW* background = dBgW_NewSet(dzb, dBgW::MOVE_BG_e, model ? model->getBaseTRMtx() : nullptr);
    mBackground[1] = background;
    if (!mModel || !mFragmentModels[0] || !mFragmentModels[1] || !mFragmentModels[2] || !mFragmentModels[3] ||
        !mBackground[0] || !background)
        result = FALSE;
    return result;
}

/* 023527A0 */
void daObjHcbh_c::setup_break_condition(fopAc_ac_c* other) {
    if (!other)
        return;
    f32 otherX = other->current.pos.x, cylinderX = f32(mCyl.mCyl.mCenter.x);
    f32 otherZ = other->current.pos.z, cylinderZ = f32(mCyl.mCyl.mCenter.z);
    s16 yaw = cM_atan2s(cylinderX - otherX, cylinderZ - otherZ);
    mBreakYaw = yaw;
    for (unsigned i = 0; i < 4; ++i) {
        s16 fragmentYaw = s16(l_fragments.mYaws[i]);
        mFragmentYaws[i] = fragmentYaw;
        mFragmentAngles[i] = 0;
        fragmentYaw = s16(l_fragments.mYaws[i]);
        yaw = mBreakYaw;
        u16 halfAngle = u16(u32(s32(fragmentYaw) - s32(yaw)) >> 1);
        mFragmentDelays[i] = gabi::ftoi(std::fabs(cM_ssin(halfAngle)) * 15.0f);
    }
}

/* 02352870 */
void daObjHcbh_c::checkCollision() {
    if (!mCyl.ChkTgHit())
        return;
    auto* hit = static_cast<dCcD_GObjInf*>(mCyl.GetTgHitObj());
    if (hit) {
        u32 type = hit->mAtType;
        if (type == HcbhAttack::AT_TYPE_BOMB || type == HcbhAttack::AT_TYPE_SKULL_HAMMER ||
            type == HcbhAttack::AT_TYPE_DARKNUT_SWORD) {
            mBreakCondition = type == HcbhAttack::AT_TYPE_BOMB
                                  ? BREAK_BOMB
                                  : (type == HcbhAttack::AT_TYPE_SKULL_HAMMER ? BREAK_HAMMER : BREAK_HEAVY_WEAPON);
            fopAc_ac_c* other = mCyl.GetTgHitAc();
            setup_break_condition(other);
        } else if (type == HcbhAttack::AT_TYPE_UNK8) {
            fopAc_ac_c* other = mCyl.GetTgHitAc();
            if (!other || other->mProfName != HEAVY_WEAPON_PROFILE) {
                mCyl.ClrTgHit();
                return;
            }
            mBreakCondition = BREAK_HEAVY_WEAPON;
            other = mCyl.GetTgHitAc();
            setup_break_condition(other);
        }
    }
    mCyl.ClrTgHit();
}

/* 023520C0 */
static void co_hitCallback(daObjHcbh_c* actor, dCcD_GObjInf*, fopAc_ac_c* other, dCcD_GObjInf*) {
    s32 isActor = fopAc_IsActor(other);
    if (isActor && other && other->mProfName == STATUE_PROFILE)
        actor->mBreakCondition = daObjHcbh_c::BREAK_STATUE;
}

/* 023529B0 */
void daObjHcbh_c::particle_set() {
    gabi::Local<csXyz> angle;
    s16 yaw = mBreakYaw;
    csXyz_ct(angle.get(), 0, yaw, 0);
    dComIfGp_particle_set(HcbhEffect::PILLAR_DUST, &current.pos, nullptr, nullptr, 255, nullptr, -1, &tevStr.mColorK0);
    dComIfGp_particle_set(HcbhEffect::PILLAR_CHIPS, &current.pos, angle.get(), nullptr, 255, nullptr, -1,
                          &tevStr.mColorK0);
    if (mBreakCondition != BREAK_BOMB)
        dComIfGp_particle_set(HcbhEffect::PILLAR_FRAGMENTS, &current.pos, angle.get(), nullptr, 255, nullptr, -1,
                              &tevStr.mColorK0);
    mSmoke.end();
    auto* particles = dComIfGp_getParticle();
    particles->set(2, HcbhEffect::PILLAR_SMOKE, &current.pos, angle.get(), nullptr, 160,
                   reinterpret_cast<dPa_levelEcallBack*>(&mSmoke), -1, &tevStr.mColorK0, nullptr, nullptr);
}

/* 02352B04 */
void daObjHcbh_c::make_item() {
    u32 item = Hcbh_PrmAbstract(this, 6, 0);
    u32 saveBit = Hcbh_PrmAbstract(this, 7, 6);
    gabi::Local<cXyz> position;
    gabi::Local<csXyz> angle;
    f32 z = current.pos.z, y = current.pos.y, x = current.pos.x;
    position->y = y;
    position->z = z;
    position->x = x;
    csXyz_ct(angle.get(), 0, 0, 0);
    s8 room = home.roomNo;
    y = f32(position->y) - 30.0f;
    position->y = y;
    fopAcM_createItemFromTable(position.get(), item, saveBit, room, 0, angle.get(), 1, 0);
    u32 bit = Hcbh_PrmAbstract(this, 8, 13);
    if (bit != 255) {
        room = home.roomNo;
        dComIfGs_onSwitch(bit, room);
    }
}

/* 02352120 */
cPhs_State daObjHcbh_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = gabi::ea(&l_actorVtable);
            __ct(&mAcch);
            __ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, gabi::ea(&l_aabVtable));
            Hcbh_constructSpheres(mSph, 6, sizeof(dCcD_Sph), Hcbh_sphCtor);
            __ct(&mSmoke, 1);
            condition = actor_condition;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    if (mInitState) {
        if (!mAppears)
            return cPhs_ERROR_e;
    } else {
        f32 height = current.pos.y;
        mPillarAngle = 0;
        mPillarHeight = height;
        for (unsigned i = 0; i < 4; ++i) {
            mFragmentPositions[i].x = f32(current.pos.x);
            mFragmentPositions[i].y = f32(current.pos.y);
            mFragmentPositions[i].z = f32(current.pos.z);
            mFragmentAngles[i] = 0;
            mFragmentYaws[i] = 0;
        }
        s32 appears = chk_appear();
        mAppears = appears;
        if (!appears)
            return cPhs_ERROR_e;
    }
    cPhs_State phase = dComIfG_resLoad(&mPhase, "Hcbh");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0x2D00))
        return cPhs_ERROR_e;
    auto* bg = dComIfG_Bgsp();
    dBgW* background = mBackground[0];
    if (bg->Regist(background, this))
        return cPhs_ERROR_e;
    bg = dComIfG_Bgsp();
    background = mBackground[1];
    if (bg->Regist(background, this))
        return cPhs_ERROR_e;
    J3DModel* model = mModel;
    cullMtx = model ? gabi::ea(model->getBaseTRMtx()) : 0;
    init_mtx();
    mAcchCir.SetWall(589.0f, 70.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    u32 flags = mAcch.m_flags;
    mAcch.m_roof_crr_height = 589.0f;
    mAcch.m_flags = flags & ~(dBgS_Acch::ROOF_NONE | dBgS_Acch::WATER_NONE);
    mAcch.CrrPos(dComIfG_Bgsp());
    flags = mAcch.m_flags;
    mAcch.m_flags = flags & ~u32(dBgS_Acch::GROUND_LANDING);
    mStts.Init(255, 255, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);
    mCyl.SetC(&current.pos);
    mCyl.mGObjCo.mHitCallback = gabi::guest_fn(reinterpret_cast<const void*>(co_hitCallback));
    for (unsigned i = 0; i < 6; ++i) {
        mSph[i].Set(&l_sph_src);
        mSph[i].SetStts(&mStts);
        mSph[i].SetR(70.0f);
        f32 height = current.pos.y, yOffset = l_sphereCenters.mValues[i].y, x = current.pos.x, z = current.pos.z;
        Hcbh_SphSetC(&mSph[i].mSph, x, height + yOffset, z);
    }
    mSmoke.m11 = 0;
    mAction.d = 0;
    mSmoke.m12 = 1;
    mAction.f = gabi::guest_fn(reinterpret_cast<const void*>(Hcbh_waitProc));
    mAction.i = -1;
    mSmoke.mTevstr = gabi::ea(&tevStr);
    fopAcM_setCullSizeBox(this, -40.0f, 0.0f, -40.0f, 100.0f, 589.0f, 100.0f);
    return phase;
}

/* 0235251C */
s32 daObjHcbh_c::_delete() {
    if (mAppears) {
        dComIfG_resDelete(&mPhase, "Hcbh");
        if (heap) {
            for (unsigned i = 0; i < 2; ++i)
                if (mBackground[i]) {
                    releaseBackground(this, i);
                    mBackground[i] = nullptr;
                }
        }
        mSmoke.remove();
    }
    return TRUE;
}

/* 02352BFC */
void daObjHcbh_c::wait_act_proc() {
    checkCollision();
    if (!mBreakCondition) {
        cCcS_Set(dComIfG_Ccsp(), &mCyl);
        return;
    }
    particle_set();
    s8 room = current.roomNo;
    s32 reverb = dComIfGp_getReverb(room);
    mDoAud_seStart(JA_SE_OBJ_HCBH_BREAK, &current.pos, 0, reverb);
    mCyl.mGObjCo.mHitCallback = 0;
    mAction.d = 0;
    mAction.i = -1;
    mAction.f = gabi::guest_fn(reinterpret_cast<const void*>(Hcbh_fallProc));
}

/* 02352CA0 */
void daObjHcbh_c::fall_act_proc() {
    s16 angle = mPillarAngle;
    if (angle < 0) {
        mPillarAngle = -32768;
        mPillarAngularVelocity = 0;
    } else {
        if (angle > 0x2000) {
            releaseBackground(this, 0);
            releaseBackground(this, 1);
            angle = mPillarAngle;
        }
        // HD: preserve the fused acceleration and signed halfword narrowing.
        f32 acceleration = gabi::fmadds(127.0f, cM_ssin(u16(angle)), 4.0f);
        s16 velocity = mPillarAngularVelocity;
        mPillarAngularVelocity = s16(s32(velocity) + s32(s16(gabi::ftoi(acceleration))));
    }
    f32 homeHeight = home.pos.y - 10.0f, height = mPillarHeight;
    if (height < homeHeight) {
        make_item();
        auto* vibration = dComIfGp_getVibration();
        gabi::Local<cXyz> direction;
        direction->z = 0.0f;
        direction->x = 0.0f;
        direction->y = 1.0f;
        vibration->StartShock(8, -17, direction.get());
        s8 room = current.roomNo;
        s32 reverb = dComIfGp_getReverb(room);
        mDoAud_seStart(JA_SE_OBJ_HCBH_FALL, &current.pos, 0, reverb);
        fopAcM_delete(this);
    } else {
        f32 velocity = mFallVelocity;
        u16 angle = mPillarAngle;
        mFallVelocity = velocity - 0.011f;
        f32 pivot = 50.0f * cM_ssin(angle);
        // The original loads x after storing the velocity and reading the sine table.
        transformPillar(this, pivot);
        for (unsigned i = 0; i < 6; ++i) {
            gabi::Local<cXyz> center;
            f32 x = f32(l_sphereCenters.mValues[i].x), z = f32(l_sphereCenters.mValues[i].z),
                y = f32(l_sphereCenters.mValues[i].y);
            center->z = z;
            center->y = y;
            center->x = x;
            mDoMtx_stack_c::multVec(center.get(), center.get());
            dCcD_Sph* sphere = &mSph[i];
            Hcbh_SphMoveCAt(sphere, center.get());
            cCcS_Set(dComIfG_Ccsp(), sphere);
        }
    }
    for (unsigned i = 0; i < 4; ++i) {
        s32 delay = mFragmentDelays[i];
        if (delay > 0) {
            mFragmentDelays[i] = delay - 1;
            continue;
        }
        f32 velocity = f32(mFragmentVelocities[i]) - 0.01f;
        mFragmentVelocities[i] = velocity;
        f32 y = f32(mFragmentPositions[i].y) + velocity;
        mFragmentPositions[i].y = y;
        f32 floor = f32(home.pos.y) - 100.0f;
        if (y < floor)
            mFragmentPositions[i].y = floor;
        u16 angle = mFragmentAngles[i];
        // HD: preserve the fused acceleration and signed halfword narrowing.
        f32 acceleration = gabi::fmadds(512.0f, cM_ssin(angle), 4.0f);
        s16 velocityAngle = mFragmentAngularVelocities[i];
        s32 sum = s32(velocityAngle) + s32(s16(gabi::ftoi(acceleration)));
        mFragmentAngularVelocities[i] = s16(sum);
        s16 currentAngle = mFragmentAngles[i];
        s16 next = s16(s32(currentAngle) + sum);
        mFragmentAngles[i] = next > 0x4000 ? 0x4000 : next;
    }
}

/* 023525EC */
s32 daObjHcbh_c::_execute() {
    set_mtx();
    mStts.Move();
    mAcch.CrrPos(dComIfG_Bgsp());
    for (unsigned i = 0; i < 2; ++i) {
        dBgW* background = mBackground[i];
        if (background && background->ChkUsed())
            background->Move();
    }
    s16 angle = mPillarAngle, speed = mPillarAngularVelocity;
    f32 height = mPillarHeight, velocity = mFallVelocity;
    angle = s16(s32(angle) + s32(speed));
    mPillarHeight = height + velocity;
    mPillarAngle = angle < 0x4000 ? angle : 0x4000;
    (this->*mAction)();
    return TRUE;
}

/* 023526FC */
s32 daObjHcbh_c::_draw() {
    auto* environment = dKy_getEnvlight();
    settingTevStruct(environment, 1, &current.pos, &tevStr);
    environment = dKy_getEnvlight();
    J3DModel* model = mModel;
    setLightTevColorType(environment, model, &tevStr);
    model = mModel;
    mDoExt_modelUpdateDL(model);
    for (unsigned i = 0; i < 4; ++i) {
        environment = dKy_getEnvlight();
        model = mFragmentModels[i];
        setLightTevColorType(environment, model, &tevStr);
        model = mFragmentModels[i];
        mDoExt_modelUpdateDL(model);
    }
    return TRUE;
}

/* 02352518 */
s32 daObjHcbh_Create(daObjHcbh_c* actor) {
    return actor->_create();
}

/* 023525E8 */
s32 daObjHcbh_Delete(daObjHcbh_c* actor) {
    return actor->_delete();
}

/* 023526F8 */
s32 daObjHcbh_Execute(daObjHcbh_c* actor) {
    return actor->_execute();
}

/* 02352794 */
s32 daObjHcbh_Draw(daObjHcbh_c* actor) {
    return actor->_draw();
}

/* 02352798 */
s32 daObjHcbh_IsDelete(daObjHcbh_c* actor) {
    return 1;
}

/* 023530C8 */
void __sinit_d_a_obj_hcbh_cpp() {
    // HD: the actor height is placed between the pi pair and header constructor bytes.
    l_headerStatics.mZero[2] = 0;
    l_headerStatics.mZero[0] = 0;
    l_headerStatics.mZero[3] = 0;
    l_headerStatics.mZero[1] = 0;
    __register_global_object(gabi::ea(&l_dtorRecords.mValues[0]));
    l_headerStatics.mPiLo = -3.1415927410125732f;
    l_headerStatics.mPiHi = 3.1415927410125732f;
    Hcbh_headerCtor1(&l_headerStatics.mCtor1);
    __register_global_object(gabi::ea(&l_dtorRecords.mValues[1]));
    Hcbh_headerCtor2(&l_headerStatics.mCtor2);
    __register_global_object(gabi::ea(&l_dtorRecords.mValues[2]));
    l_headerStatics.mHeight = 589.0f;
}

/* 0235316C */
void Hcbh_SafeString_dtor(void* object, s32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 02353180 */
void Hcbh_dtor(daObjHcbh_c* actor, s32 flags) {
    if (!actor)
        return;
    Hcbh_destroySpheres(actor->mSph, 6, sizeof(dCcD_Sph), Hcbh_sphDtor, 0, 0);
    __dt(&actor->mCyl);
    __dt(&actor->mStts);
    __dt(&actor->mAcchCir);
    __dt(&actor->mAcch);
    __dt(static_cast<fopAc_ac_c*>(actor));
    if (flags & 1)
        operator_delete(actor);
}

/* 0235323C */
void Hcbh_SafeString_assureTerminationImpl() {}

/* 02353240 */
u32 Hcbh_PrmAbstract(daObjHcbh_c* actor, u32 width, u32 shift) {
    u32 mask = width & 0x20 ? 0 : (1u << (width & 31));
    u32 value = shift & 0x20 ? 0 : (u32(actor->mParameters) >> (shift & 31));
    return value & (mask - 1);
}

/* WWHD glue
 data l_fragments 101C9E04
 data l_sphereCenters 1002A1B4
 data l_actorVtable 1002A120
 data l_aabVtable 1002A0E0
 data l_headerStatics 10469E1C
 data l_dtorRecords 101C9E1C
 data l_cyl_src 1002A170
 data l_sph_src 1002A130
 str 1002A0C0 "Hcbh"
 str 1002A200 "d_a_obj_hcbh.cpp"
 str 1002A1FC "0"
 ext 025F19F8 void mDoMtx_XYZrotM(Mtx34*,s16,s16,s16)
 ext 02018F3C void Hcbh_SphSetC(cM3dGSph*,f32,f32,f32)
 ext 025167E4 void Hcbh_SphMoveCAt(dCcD_Sph*,cXyz*)
 ext 028EFFD0 void Hcbh_constructSpheres(dCcD_Sph*,u32,u32,HcbhSphConstructor)
 ext 028F0164 void Hcbh_destroySpheres(dCcD_Sph*,u32,u32,HcbhSphDestructor,u32,u32)
 ext 025166F0 void Hcbh_sphCtor(dCcD_Sph*)
 ext 02515AE8 void Hcbh_sphDtor(dCcD_Sph*,s32)
 ext 02352BFC void Hcbh_waitProc(daObjHcbh_c*)
 ext 02352CA0 void Hcbh_fallProc(daObjHcbh_c*)
 ext 028ED6F8 void Hcbh_headerCtor1(be<u8>*)
 ext 028EAB2C void Hcbh_headerCtor2(be<u8>*)
*/
