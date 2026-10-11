/* Tower of the Gods glowing statue/pillar. GameCube source names and typed HD layout.
 * Verified call order, full-register results, floating-point and guest-stack semantics retained. */
#include "d/actor/d_a_obj_try_private.h"
#include <cmath>
using daObjTry::Act_c;
using Actor = Act_c;
using namespace try_actor;
const gc_tu_data gc_tu = {.safeString = 0x10031C18};
static AttributesHD tryAttributes;
static SavePointerHD trySave;
static ResourcePointerHD tryResources;
static cXyz tryZero;
static ByteHD tryStay;
static ByteHD tryBingo;
static ByteHD tryRestart;
static RegistersHD tryRegisters;
static HeaderStaticsHD tryStatics;
static BoundsHD tryBounds;
static RecordHD tryRecordA;
static RecordHD tryRecordB;
static RecordHD tryRecordC;
static dCcD_SrcCyl tryCylinderSource;
static TrigTableHD tryTrig;
static ModesHD tryModes;
static WordHD tryColorGuard0;
static ColorsHD tryColors0;
static WordHD tryColorGuard1;
static ColorsHD tryColors1;
static WordHD tryColorGuard2;
static ColorsHD tryColors2;
static WordHD tryColorGuard3;
static ColorsHD tryColors3;
static WordHD tryAcchGroupVtable;
static WordHD tryAcchPolyVtable;
static WordHD tryActorVtable;
static WordHD tryAcchBaseVtable;
static WordHD tryStatusVtable;
static WordHD tryGStatusVtable;
static WordHD tryCylinderBaseVtable;
static WordHD tryShapeBaseVtable;
static WordHD tryHitVtable;
static WordHD tryCylinderShapeVtable;
static WordHD tryCylinderVtable;
static WordHD tryWaterPolyVtable;
static WordHD tryWaterGroupVtable;
static WordHD tryWaterBaseVtable;
using TryHeapCallback = s32 (*)(Actor*);
using TrySearchCallback = void* (*)(Actor*, Actor*);
s32 trySolidHeapCB(Actor*);
void* trySearchSameType(Actor*, Actor*);
f64 tryGamma(u32);
static void tryEventOff(void* a0, u32 a1);
static void tryEventOn(void* a0, u32 a1);
static void tryDeleteResource(const char* a0);
static void tryCreateWaterPillar(void* a0, s32 a1, f32 a2, f32 a3);
static void tryInitMinusOne(void* a0);
static void tryInitPlusOne(void* a0);
static void tryDestroyCylinder(void* a0, s32 a1);
static void tryDestroyStatus(void* a0, s32 a1);
static void tryDestroyCircle(void* a0, s32 a1);
static void tryDestroyAcch(void* a0, s32 a1);
static void tryInitStatus(void* a0, s32 a1, s32 a2, void* a3);
static void trySetCylinder(void* a0, void* a1);
static void tryStartShock(void* a0, u32 a1, s32 a2, void* a3);
static u32 tryGetResource(void* a0, void* a1, s16 a2);
static u32 tryCreateModel(void* a0, u32 a1, u32 a2);
static s32 tryInitBrk(void* a0, void* a1, void* a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, f32 a9);
static u32 tryParameter(void* a0, s32 a1, s32 a2);
static void trySetCurrentRoom(void* a0, void* a1);
static void tryConstructWaterCheck(void* a0);
static s32 tryQueryWaterHeight(void* a0, void* a1, f32 a2);
static void tryDestroyWaterCheckBase(void* a0, s32 a1);
static void tryEnterBrk(void* a0, void* a1, f32 a2);
static void tryUpdateModel(void* a0, s32 a1);
static void tryMoveCylinder(void* a0, void* a1);
static void tryRegisterCylinder(void* a0, void* a1);
static void tryRegisterMass(void* a0, void* a1, s32 a2);
static void tryHitSound(void* a0, s32 a1, void* a2, u32 a3);
static void tryHitEffect(void* a0, void* a1);
static u32 tryMaterialSound(void* a0, void* a1);
static s32 tryReverb(s32 a0);
static void tryStartSound(u32 a0, void* a1, u32 a2, s32 a3);
static void tryConstructBrk(void* a0);
static void tryConstructAcch(void* a0);
static void tryConstructAcchCircle(void* a0);
static void tryConstructStatus(void* a0);
static void tryConstructGStatus(void* a0);
static void tryConstructCollider(void* a0);
static void tryConstructCylinderShape(void* a0);
static void tryConstructFollowCallback(void* a0, s32 a1, s32 a2);
static s32 tryLoadResource(void* a0, const char* a1);
static s32 tryEnterHeap(void* a0, TryHeapCallback a1, u32 a2);
static void trySetWall(void* a0, f32 a1, f32 a2);
static void trySetAcch(void* a0, void* a1, void* a2, void* a3, s32 a4, void* a5, void* a6, void* a7,
                       void* a8);
static void tryMovePosition(void* a0, void* a1);
static void tryCorrectPosition(void* a0, void* a1);
static s32 tryCheckSwitch(void* a0, u32 a1, s32 a2);
static u32 trySearchActor(TrySearchCallback a0, void* a1);
static f64 tryPow(f64 a0, f64 a1);
static void trySoundEnvironment(void* a0, s32 a1, u32 a2, s32 a3);
static void tryForceLockOff(void* a0, u32 a1);
static void tryLandEffect(void* a0, void* a1, f32 a2);
// HD: callers consume the complete result register of the byte-returning water checks.
static s32 tryInWaterRegister(void* a0);
static s32 trySubmergedRegister(void* a0);
static s32 tryGroundCode(void* a0, void* a1);
static s32 tryMovingGround(void* a0, void* a1);
static u32 tryPolyColor(void* a0, void* a1);
static u32 trySetParticle(void* a0, s32 a1, u32 a2, void* a3, void* a4, void* a5, s32 a6, void* a7, s32 a8,
                          void* a9, void* a10, void* a11);
static void tryConstructAngles(void* a0, s32 a1, s32 a2, s32 a3);
// HD: assertion names and checks follow the verified image rather than the incomplete GC attr() body.
static void tryRegisterGlobal(void* a0);
static void tryDestroyActor(void* a0, s32 a1);
static void tryCylinderRadius(void* a0, f32 a1);
static void tryCylinderHeight(void* a0, f32 a1);
static void trySubtract(void* a0, void* a1, void* a2);
static f64 trySqrt(f64 a0);
static void tryMoveStream(void* a0, void* a1, void* a2, f32 a3, f32 a4);
static void trySettingTev(void* a0, s32 a1, void* a2, void* a3);
static void trySetLight(void* a0, void* a1, void* a2);
static s32 tryCheckAttackHit(void* a0);
static void tryClearAttackHit(void* a0);
static s32 tryCheckTargetHit(void* a0);
static u32 tryGetTargetHit(void* a0);
static void tryClearTargetHit(void* a0);
static s32 tryCullCheck(void* a0);
static void trySetCullSphere(void* a0, f32 a1, f32 a2, f32 a3, f32 a4);
static void tryAddCalc(void* a0, f32 a1, f32 a2, f32 a3, f32 a4);
static void tryChasePosition(void* a0, void* a1, f32 a2);
static f64 trySquareDistance(void* a0, void* a1);
static u32 checkedType(Act_c* actor) {
    u32 type = actor->type;
    if (type >= daObjTry::Type_Max) {
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
        type = actor->type;
    }
    return type;
}
static void copyMatrix(u32 model) {
    be<u32>* src = reinterpret_cast<be<u32>*>(mDoMtx_stack_c::get());
    be<u32>* dst = reinterpret_cast<be<u32>*>(&modelView(model)->mBaseMtx);
    u32 words[12];
    for (unsigned i = 0; i < 12; ++i)
        words[i] = src[i];
    for (unsigned i = 0; i < 12; ++i)
        dst[i] = words[i];
}

// HD: particle RGB values are converted with the HD gamma curve before byte truncation.
static u8 gammaByte(u32 intensity) {
    f64 corrected = tryGamma(intensity);
    return u8(gabi::ftoi(f32(corrected * 255.0)));
}
static void initializeParticleColors(WordHD* guard, ColorsHD* colors, u32 r, u32 g, u32 b, u32 er, u32 eg,
                                     u32 eb) {
    if (guard->mValue)
        return;
    guard->mValue = 1;
    colors->mPrimary.r = gammaByte(r);
    colors->mPrimary.g = gammaByte(g);
    colors->mPrimary.b = gammaByte(b);
    colors->mEnvironment.r = gammaByte(er);
    colors->mEnvironment.g = gammaByte(eg);
    colors->mEnvironment.b = gammaByte(eb);
}

/* 023AD348 */
void daObjTry::Act_c::prm_set_swSave(s32 bit) {

    u32 parameters = this->mParameters;
    this->mParameters = (parameters & ~0xFF00u) | ((u32(bit) << 8) & 0xFF00u);
}

/* 023AB89C */
s32 trySolidHeapCB(Actor* actor) { return actor->create_heap(); }

/* 023AB6B0 */
s32 daObjTry::Act_c::create_heap() {

    gabi::Local<SafeString> modelName, animationName;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    u32 type = checkedType(this);
    modelName->__vtbl = gc_tu.safeString;
    modelName->mStringTop = gabi::guest_addr("Hseki");
    s16 modelIndex = s16(tryAttributes.mRows[type].mModelIndex);
    u32 resource = tryGetResource(tryResources.mPointer.get(), modelName.get(), modelIndex);
    if (!resource)
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3F2, "mdl_data != 0");
    u32 model = tryCreateModel(gabi::at<void>(resource), 0, 0x11020203);
    type = this->type;
    this->model = model;
    s32 animationReady = 1;
    if (type >= daObjTry::Type_Max) {
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
        type = this->type;
    }
    s16 animationIndex = s16(tryAttributes.mRows[type].mBrkIndex);
    if (animationIndex >= 0) {
        if (type >= daObjTry::Type_Max) {
            JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
            type = this->type;
        }
        animationName->mStringTop = gabi::guest_addr("Hseki");
        animationName->__vtbl = gc_tu.safeString;
        animationIndex = s16(tryAttributes.mRows[type].mBrkIndex);
        u32 animation = tryGetResource(tryResources.mPointer.get(), animationName.get(), animationIndex);
        if (!animation)
            JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3FC, "brk != 0");
        animationReady = tryInitBrk(&this->mBrk, gabi::at<void>(resource), gabi::at<void>(animation), 1, 2, 0,
                                    -1, 0, 0, 1.0f);
        if (!tryParameter(this, 1, 31)) {
            s16 frame = s16(this->mBrk.mFrameCtrl.mEnd);
            this->mBrk.mFrameCtrl.mFrame = f32(frame);
        }
    }
    return this->model && animationReady ? 1 : 0;
}

/* 023AB8A0 */
void daObjTry::Act_c::init_cc() {

    u32 type = checkedType(this);
    u8 weight = u8(tryAttributes.mRows[type].mWeight);
    tryInitStatus(&this->mStts, weight, 255, this);
    trySetCylinder(&this->mCyl, &tryCylinderSource);
    type = this->type;
    this->mCyl.mStts = &this->mStts;
    if (type >= daObjTry::Type_Max) {
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
        type = this->type;
    }
    f32 radius = f32(u8(tryAttributes.mRows[type].mRadius));
    tryCylinderRadius(&this->mCyl.mCyl, radius);
    type = checkedType(this);
    f32 height = f32(u8(tryAttributes.mRows[type].mHeight));
    tryCylinderHeight(&this->mCyl.mCyl, height);
    bits(this->mCyl.mGObjAt.mVec.x) = u32(bits(tryZero.x));
    bits(this->mCyl.mGObjAt.mVec.y) = u32(bits(tryZero.y));
    bits(this->mCyl.mGObjAt.mVec.z) = u32(bits(tryZero.z));
    u32 x = u32(bits(tryZero.x)), flags = u32(this->mCyl.mGObjTg.mSPrm);
    bits(this->mCyl.mGObjTg.mVec.x) = x;
    bits(this->mCyl.mGObjTg.mVec.y) = u32(bits(tryZero.y));
    u32 z = u32(bits(tryZero.z));
    this->mCyl.mGObjTg.mSPrm = flags | 1;
    bits(this->mCyl.mGObjTg.mVec.z) = z;
}

/* 023AB4BC */
void* trySearchSameType(Actor* candidate, Actor* actor) {
    if (!candidate || !fopAc_IsActor(candidate))
        return nullptr;
    if (!candidate || s16(candidate->mProcName) != 0x1CA || candidate == actor)
        return nullptr;
    u32 otherType = tryParameter(candidate, 4, 0);
    u32 type = tryParameter(actor, 4, 0);
    if (otherType != type || !candidate->appears)
        return nullptr;
    return candidate;
}

/* 023AB584 */
s32 daObjTry::Act_c::chk_appear() {

    u32 type = this->type;
    s32 result = 1;
    if (type >= daObjTry::Type_Max) {
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
        type = this->type;
    }
    if (u8(tryAttributes.mRows[type].mSwitchAppear)) {
        bool inverse = tryParameter(this, 1, 31) != 0;
        u32 bit = tryParameter(this, 8, 8);
        SaveHD* save = trySave.mPointer;
        s8 room = this->home.roomNo;
        bool on = tryCheckSwitch(save->mSwitch, bit, room) != 0;
        type = this->type;
        if (on != inverse)
            result = 0;
    }
    if (type >= daObjTry::Type_Max) {
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
        type = this->type;
    }
    if (u8(tryAttributes.mRows[type].mUnique) && trySearchActor(trySearchSameType, this))
        result = 0;
    return result;
}

/* 023ABB7C */
s32 daObjTry::Act_c::_create() {

    u32 condition = this->actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (this) {
            fopAc_ac_c_ct(this);
            this->__vtbl = gabi::ea(&tryActorVtable);
            tryConstructBrk(&this->mBrk);
            tryConstructAcch(&this->mAcch);
            this->mAcch.mbObjThrough = 1;
            this->mAcch.cBgS_Chk::__vtbl = gabi::ea(&tryAcchBaseVtable);
            this->mAcch.dBgS_GrpPassChk::__vtbl = gabi::ea(&tryAcchGroupVtable);
            this->mAcch.dBgS_PolyPassChk::__vtbl = gabi::ea(&tryAcchPolyVtable);
            tryConstructAcchCircle(&this->mAcchCir);
            tryConstructStatus(&this->mStts);
            tryConstructGStatus(&this->mStts.__vtbl_gstts);
            this->mStts.__vtbl = gabi::ea(&tryStatusVtable);
            this->mStts.__vtbl_gstts = gabi::ea(&tryGStatusVtable);
            tryConstructCollider(&this->mCyl);
            reinterpret_cast<CylinderCtorHD*>(&this->mCyl)->mCylinderVtable =
                gabi::ea(&tryCylinderBaseVtable);
            reinterpret_cast<CylinderCtorHD*>(&this->mCyl)->mShapeVtable = gabi::ea(&tryShapeBaseVtable);
            tryConstructCylinderShape(&this->mCyl.mCyl);
            this->mCyl.__vtbl_hitinf = gabi::ea(&tryHitVtable);
            this->mCyl.mCyl.__vtbl = gabi::ea(&tryCylinderShapeVtable);
            reinterpret_cast<CylinderCtorHD*>(&this->mCyl)->mCylinderVtable = gabi::ea(&tryCylinderVtable);
            tryConstructFollowCallback(&this->mFollowCallback, 0, 0);
            this->emitter = 0;
            condition = this->actor_condition;
        }
        this->actor_condition = condition | fopAcCnd_INIT_e;
    }
    this->type = tryParameter(this, 4, 0);
    s32 appears = this->chk_appear();
    this->appears = u8(appears);
    if (!appears)
        return cPhs_ERROR_e;
    s32 phase = tryLoadResource(&this->phase, "Hseki");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    u32 type = checkedType(this), heapSize = u32(tryAttributes.mRows[type].mHeapSize);
    if (!tryEnterHeap(this, trySolidHeapCB, heapSize))
        return cPhs_ERROR_e;
    type = checkedType(this);
    f32 radius = f32(u8(tryAttributes.mRows[type].mRadius));
    trySetWall(&this->mAcchCir, 30.0f, radius);
    trySetAcch(&this->mAcch, &this->current.pos, &this->old.pos, this, 1, &this->mAcchCir, &this->speed,
               &this->current.angle, &this->shape_angle);
    u32 flags = u32(this->mAcch.m_flags);
    type = this->type;
    this->mAcch.m_flags = flags & ~0x408u;
    if (type >= daObjTry::Type_Max) {
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
        type = this->type;
    }
    this->mAcch.m_roof_crr_height = f32(u8(tryAttributes.mRows[type].mGroundOffset));
    this->init_cc();
    u32 model = this->model;
    this->cullMtx = model ? model + 0xC8 : 0;
    this->cull_set_draw();
    type = checkedType(this);
    u32 actualType = this->type, gravity = u32(bits(tryAttributes.mRows[type].mGravity));
    bits(this->gravity) = gravity;
    if (actualType == 5 || actualType == 6)
        this->actor_status = u32(this->actor_status) | fopAcStts_UNK2000000_e;
    tryMovePosition(this, 0);
    PlayHD* play = playView();
    tryCorrectPosition(&this->mAcch, play->mBgS);
    u32 x = u32(bits(this->current.pos.x)), acchFlags = u32(this->mAcch.m_flags);
    type = this->type;
    bits(this->attention_info.position.x) = x;
    u32 status = u32(this->attention_info.flags);
    this->mLandTimer = 20;
    this->attention_info.distances[4] = 23;
    this->mLandEffectDone = 1;
    this->mAcch.m_flags = acchFlags & ~0x80u;
    this->attention_info.flags = status | 0x10;
    if (type >= daObjTry::Type_Max) {
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
        type = this->type;
    }
    f32 y = this->current.pos.y;
    u32 z = u32(bits(this->current.pos.z));
    f32 offset = f32(tryAttributes.mRows[type].mAttentionOffset);
    this->mMovingGround = 1;
    this->mTimer = 0;
    this->mCollisionDelay = 0;
    this->attention_info.position.y = y + offset;
    bits(this->attention_info.position.z) = z;
    bits(this->mPreviousSpeedY) = 0;
    this->restartDelay = 0;
    this->init_mtx();
    this->bingoActive = 0;
    this->field_0x64C = 0;
    this->field_0x64D = 0;
    this->mCorrecting = 1;
    this->mLeftHomeRoom = 0;
    this->mUnused = 0;
    this->mode_wait_init();
    this->fopAc_ac_c::model = u32(this->model);
    return phase;
}

/* 023ABF90 */
s32 daObjTry::Act_c::_delete() {

    if (this->appears) {
        endCallback(&this->mFollowCallback);
        u32 emitter = this->emitter;
        if (emitter) {
            u32 flags = u32(emitterView(emitter)->mFlags);
            emitterView(emitter)->mMaxFrame = 0xFFFFFFFFu;
            emitterView(emitter)->mFlags = flags | 1;
        }
        u32 type = this->type;
        if (type == 5 || type == 6) {
            SaveHD* save = trySave.mPointer;
            tryEventOff(save->mEvent, type == 5 ? 0x108 : 0x110);
        }
        this->fopAc_ac_c::model = 0;
        tryDeleteResource("Hseki");
    }
    return 1;
}

/* 023AC678 */
void daObjTry::Act_c::mode_restart_init() {

    u32 collider = u32(this->mCyl.mObjAt.mSPrm), contact = u32(this->mCyl.mObjCo.mSPrm),
        hit = u32(this->mCyl.mObjTg.mSPrm);
    bits(this->speedF) = 0;
    u32 acch = u32(this->mAcch.m_flags), homeX = u32(bits(this->home.pos.x));
    this->mCyl.mObjAt.mSPrm = collider & ~1u;
    u32 type = this->type;
    this->mCyl.mObjTg.mSPrm = hit & ~1u;
    this->mCyl.mObjCo.mSPrm = contact | 1;
    this->mTimer = 110;
    this->mAcch.m_flags = (((acch | 12) & ~2u) | 0x400) & ~0x2000u;
    if (type >= daObjTry::Type_Max) {
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
        type = this->type;
    }
    s16 homeYaw = s16(this->home.angle.y);
    u8 offset = u8(tryAttributes.mRows[type].mGroundOffset);
    s16 homeXAngle = s16(this->home.angle.x);
    u32 status = u32(this->attention_info.flags);
    bits(this->current.pos.x) = homeX;
    u32 x = u32(bits(this->current.pos.x));
    s16 homeZAngle = s16(this->home.angle.z);
    f32 homeY = f32(this->home.pos.y), vertical = -10.0f - f32(offset);
    s16 yaw = s16(s32(homeYaw) - 32768);
    this->current.angle.y = yaw;
    bits(this->old.pos.x) = x;
    u32 homeZ = u32(bits(this->home.pos.z));
    this->old.angle.x = homeXAngle;
    this->attention_info.flags = status & ~0x10u;
    this->shape_angle.x = homeXAngle;
    this->current.angle.x = homeXAngle;
    this->current.pos.y = homeY + vertical;
    bits(this->current.pos.z) = homeZ;
    u32 z = u32(bits(this->current.pos.z));
    this->old.angle.y = yaw;
    this->old.angle.z = homeZAngle;
    this->shape_angle.y = yaw;
    this->shape_angle.z = homeZAngle;
    u32 y = u32(bits(this->current.pos.y));
    this->mode = MODE_RESTART;
    bits(this->old.pos.y) = y;
    this->current.angle.z = homeZAngle;
    bits(this->old.pos.z) = z;
}

/* 023ADA30 */
void daObjTry::Act_c::mode_restart() {

    s16 timer = s16(u16(s16(this->mTimer)) - 1);
    this->mTimer = timer;
    if (timer < 80)
        tryRestart.mValue = 1;
    this->mLandTimer = 20;
    timer = s16(this->mTimer);
    this->mLandEffectDone = 1;
    if (timer == 0) {
        s16 yaw = s16(this->home.angle.y);
        u32 y = u32(bits(this->home.pos.y));
        this->shape_angle.y = yaw;
        this->current.angle.y = yaw;
        bits(this->current.pos.y) = y;
        this->mode_wait_init();
    } else if (timer < 50) {
        u32 angle = u16(gabi::ftoi(f32(timer) * 655.36f));
        f32 factor = (1.0f - f32(tryTrig.mEntries[angle >> 3].mCos)) * 0.5f;
        u32 type = checkedType(this);
        f32 height = f32(u8(tryAttributes.mRows[type].mGroundOffset));
        s32 rotation = gabi::ftoi(factor * -32768.0f);
        s32 homeYaw = s16(this->home.angle.y);
        f32 homeY = f32(this->home.pos.y);
        u16 yaw = u16(u32(rotation) + u32(homeYaw));
        f32 y = gabi::fmadds(-10.0f - height, factor, homeY);
        this->shape_angle.y = yaw;
        this->current.pos.y = y;
        this->current.angle.y = yaw;
    }
}

/* 023ABB30 */
void daObjTry::Act_c::mode_wait_init() {

    u32 hitFlags = u32(this->mCyl.mObjTg.mSPrm), acchFlags = u32(this->mAcch.m_flags);
    u32 colliderFlags = u32(this->mCyl.mObjAt.mSPrm);
    this->mAcch.m_flags = acchFlags & 0xFFFFDBF1u;
    u32 contactFlags = u32(this->mCyl.mObjCo.mSPrm);
    this->mode = MODE_WAIT;
    this->mCyl.mObjAt.mSPrm = colliderFlags & ~1u;
    bits(this->speedF) = 0;
    this->mCyl.mObjTg.mSPrm = hitFlags | 1;
    this->mCyl.mObjCo.mSPrm = contactFlags | 1;
}

/* 023ADFE4 */
void daObjTry::Act_c::mode_wait() {

    u32 type = checkedType(this);
    bool corrected = u8(tryAttributes.mRows[type].mAllowCorrection) && this->field_0x64C;
    if (type >= daObjTry::Type_Max)
        type = checkedType(this);
    bool chase = u8(tryAttributes.mRows[type].mChaseCorrection) && this->field_0x64D;
    if (chase) {
        u32 y = u32(bits(this->current.pos.y));
        tryChasePosition(&this->current.pos, &this->field_0x63C, 10.0f);
        gabi::Local<cXyz> target, current;
        gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
        bits(target->x) = u32(bits(this->field_0x63C.x));
        bits(target->y) = 0;
        bits(current->x) = u32(bits(this->current.pos.x));
        bits(this->current.pos.y) = y;
        bits(current->y) = 0;
        bits(current->z) = u32(bits(this->current.pos.z));
        bits(target->z) = u32(bits(this->field_0x63C.z));
        f64 distance = trySquareDistance(target.get(), current.get());
        if (distance < 25.0) {
            u32 x = u32(bits(this->field_0x63C.x)), z = u32(bits(this->field_0x63C.z));
            bits(this->current.pos.x) = x;
            bits(this->current.pos.z) = z;
            distance = trySquareDistance(&this->field_0x63C, &this->current.pos);
            if (distance < 1.0)
                tryBingo.mValue = 1;
        }
        s32 yaw = s16(this->shape_angle.y), targetYaw = s16(this->field_0x648);
        s16 goal = s16(u16((u32(yaw - targetYaw + 0x2000) & 0xC000) + u32(targetYaw)));
        this->mCorrectionYaw = goal;
        cLib_addCalcAngleS(&this->shape_angle.y, goal, 3, 0x1800, 0x800);
    } else if (!corrected)
        this->bound();
    bool grounded = (u32(this->mAcch.m_flags) & 0x20) != 0;
    bool useStream = false;
    if (grounded && !corrected) {
        u32 status = u32(this->attention_info.flags);
        u8 immediate = this->field_0x64D;
        this->attention_info.flags = status | 0x10;
        useStream = immediate != 0;
    } else {
        this->attention_info.flags = u32(this->attention_info.flags) & ~0x10u;
        if (corrected)
            this->mCorrecting = 1;
        if (grounded)
            useStream = corrected || this->field_0x64D;
    }
    void* movement = useStream ? static_cast<void*>(&tryZero) : static_cast<void*>(&this->mStts);
    u32 flags = u32(this->mAcch.m_flags);
    if (!(flags & 0x20)) {
        gabi::Local<f32> gravity, drag, stream;
        gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
        this->calc_drop_param(gravity.get(), drag.get(), stream.get());
        u32 g = u32(bits(*reinterpret_cast<be<f32>*>(gravity.get())));
        f32 d = f32(*reinterpret_cast<be<f32>*>(drag.get())),
            v = f32(*reinterpret_cast<be<f32>*>(stream.get()));
        bits(this->gravity) = g;
        tryMoveStream(this, movement, &tryZero, d, v);
    } else {
        type = checkedType(this);
        bits(this->gravity) = u32(bits(tryAttributes.mRows[type].mGravity));
        tryMovePosition(this, movement);
    }
}

/* 023AC9D0 */
void daObjTry::Act_c::mode_carry_init() {

    u32 hitFlags = u32(this->mCyl.mObjTg.mSPrm), acchFlags = u32(this->mAcch.m_flags),
        contactFlags = u32(this->mCyl.mObjCo.mSPrm);
    this->mTimer = 8;
    u32 status = u32(this->attention_info.flags), colliderFlags = u32(this->mCyl.mObjAt.mSPrm);
    this->mAcch.m_flags = acchFlags & 0xFFFFDBF1u;
    this->mCyl.mObjTg.mSPrm = hitFlags | 1;
    this->mCyl.mObjAt.mSPrm = colliderFlags & ~1u;
    this->mode = MODE_CARRY;
    this->attention_info.flags = status & ~0x10u;
    this->mCyl.mObjCo.mSPrm = contactFlags & ~1u;
}

/* 023AE4A8 */
void daObjTry::Act_c::mode_carry() {

    s16 timer = s16(this->mTimer);
    if (timer > 0)
        this->mTimer = s16(timer - 1);
    u32 status = this->actor_status;
    bits(this->speed.y) = 0;
    if (!(status & fopAcStts_CARRY_e)) {
        f32 forward = f32(this->speedF);
        if (forward > 0.0f) {
            this->mode_drop_init();
            this->mode_drop();
        } else {
            trySetCurrentRoom(this, &this->mAcch.m_gnd);
            this->mCollisionDelay = 2;
            this->mode_wait_init();
        }
    }
}

/* 023AE2D4 */
void daObjTry::Act_c::mode_drop_init() {

    PlayHD* play = playView();
    u32 type = this->type;
    fopAc_ac_c* player = play->mpPlayer;
    if (type >= daObjTry::Type_Max)
        type = checkedType(this);
    AttrHD* firstRow = &tryAttributes.mRows[type];
    if (type >= daObjTry::Type_Max)
        type = checkedType(this);
    AttrHD* secondRow = &tryAttributes.mRows[type];
    u32 flags = u32(this->mAcch.m_flags), cylinder = u32(this->mCyl.mObjAt.mSPrm);
    f32 playerSpeed = f32(player->speedF);
    type = this->type;
    u32 target = u32(this->mCyl.mObjCo.mSPrm);
    f32 multiplier = f32(secondRow->mDropSpeedRate), constant = f32(firstRow->mDropSpeedBase);
    u32 status = u32(this->attention_info.flags);
    this->mCyl.mObjCo.mSPrm = target | 1;
    u32 collision = u32(this->mCyl.mObjTg.mSPrm);
    f32 forward = gabi::fmadds(playerSpeed, multiplier, constant);
    this->mAcch.m_flags = (flags & ~0x40Eu) | 0x2000;
    this->attention_info.flags = status & ~0x10u;
    this->mCyl.mObjTg.mSPrm = collision | 1;
    this->mCyl.mObjAt.mSPrm = cylinder | 1;
    if (type >= daObjTry::Type_Max)
        type = checkedType(this);
    u32 vertical = u32(bits(tryAttributes.mRows[type].mDropSpeedY));
    type = this->type;
    this->speedF = forward;
    bits(this->speed.y) = vertical;
    if (type >= daObjTry::Type_Max)
        type = checkedType(this);
    u32 gravity = u32(bits(tryAttributes.mRows[type].mGravity));
    this->mode = MODE_DROP;
    bits(this->gravity) = gravity;
}

/* 023AE438 */
void daObjTry::Act_c::mode_drop() {

    trySetCurrentRoom(this, &this->mAcch.m_gnd);
    this->bound();
    gabi::Local<f32> gravity, drag, stream;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    this->calc_drop_param(gravity.get(), drag.get(), stream.get());
    u32 gravityBits = u32(bits(*reinterpret_cast<be<f32>*>(gravity.get())));
    f32 streamValue = f32(*reinterpret_cast<be<f32>*>(stream.get())),
        dragValue = f32(*reinterpret_cast<be<f32>*>(drag.get()));
    bits(this->gravity) = gravityBits;
    tryMoveStream(this, &this->mStts, &tryZero, dragValue, streamValue);
}

/* 023AC424 */
void daObjTry::Act_c::mode_sink_init() {

    u32 hit = u32(this->mCyl.mObjTg.mSPrm), collider = u32(this->mCyl.mObjAt.mSPrm),
        acch = u32(this->mAcch.m_flags), contact = u32(this->mCyl.mObjCo.mSPrm), type = this->type;
    this->mCyl.mObjTg.mSPrm = hit | 1;
    this->mCyl.mObjAt.mSPrm = collider & ~1u;
    this->mCyl.mObjCo.mSPrm = contact | 1;
    this->mAcch.m_flags = ((acch | 8) & 0xFFFFFBF9u) | 0x2000;
    if (type >= daObjTry::Type_Max) {
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
        type = this->type;
    }
    AttrHD* attributes = &tryAttributes.mRows[type];
    AttrHD* second = attributes;
    if (type >= daObjTry::Type_Max) {
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
        type = this->type;
        second = &tryAttributes.mRows[type];
    }
    f32 forward = f32(this->speedF), gravity = f32(attributes->mGravity), square = forward * forward,
        extra = f32(second->mBuoyancy), vertical = f32(this->speed.y);
    this->gravity = gravity + extra;
    f64 speed = trySqrt(gabi::fmadds(vertical, vertical, square));
    type = checkedType(this);
    f32 limit = f32(tryAttributes.mRows[type].mMaxSinkSpeed);
    if (speed > f64(limit)) {
        if (type >= daObjTry::Type_Max) {
            JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
            type = this->type;
        }
        limit = f32(tryAttributes.mRows[type].mMaxSinkSpeed);
        f32 scale = f32(f64(limit) / speed);
        PSVECScale(&this->speed, &this->speed, scale);
        forward = f32(this->speedF);
        this->speedF = forward * scale;
    }
    u32 status = u32(this->attention_info.flags);
    this->mode = MODE_SINK;
    this->attention_info.flags = status & ~0x10u;
}

/* 023AE5E4 */
void daObjTry::Act_c::mode_sink() {

    f64 water = this->get_water_h();
    f32 ground = f32(this->mAcch.m_ground_h);
    trySetCurrentRoom(this, &this->mAcch.m_gnd);
    u32 type;
    if (f32(this->speed.y) != 0.0f) {
        type = checkedType(this);
        f32 y = f32(this->current.pos.y), offset = f32(tryAttributes.mRows[type].mWaterOffset);
        if (f64(y + offset) > water) {
            if (type >= daObjTry::Type_Max)
                type = checkedType(this);
            offset = f32(tryAttributes.mRows[type].mWaterOffset);
            f32 surface = f32(water - f64(offset));
            if (surface > ground) {
                if (type >= daObjTry::Type_Max)
                    type = checkedType(this);
                offset = f32(tryAttributes.mRows[type].mWaterOffset);
                f32 newY = f32(water - f64(offset));
                type = this->type;
                this->current.pos.y = newY;
            }
        }
    } else
        type = this->type;
    if (type >= daObjTry::Type_Max)
        type = checkedType(this);
    AttrHD* firstRow = &tryAttributes.mRows[type];
    if (type >= daObjTry::Type_Max)
        type = checkedType(this);
    f32 drag = f32(firstRow->mWaterDrag), stream = f32(tryAttributes.mRows[type].mWaterStream);
    tryMoveStream(this, &this->mStts, &tryZero, drag, stream);
}

/* 023AD358 */
s32 daObjTry::Act_c::mode_proc_call() {

    if ((u32(this->actor_status) & fopAcStts_CARRY_e) && u32(this->mode) != 2)
        this->mode_carry_init();
    tryBingo.mValue = 0;
    tryRestart.mValue = 0;
    (this->*tryModes.mEntries[u32(this->mode)])();
    u32 type = checkedType(this);
    if (s16(tryAttributes.mRows[type].mBrkIndex) >= 0) {
        u8 bingo = u8(tryBingo.mValue), restart = u8(tryRestart.mValue);
        if (bingo || restart)
            this->eff_set_bingo(bingo, restart);
        else
            this->eff_clr_bingo();
        bingo = u8(tryBingo.mValue);
        bool animate = bingo != 0;
        if (!animate)
            animate = u8(tryRestart.mValue) != 0 || u32(this->mode) == 2;
        if (animate) {
            s32 room = s8(this->current.roomNo);
            bits(this->mBrk.mFrameCtrl.mRate) = 0x3F800000;
            this->mBrk.mFrameCtrl.mAttribute = 2;
            s32 reverb = tryReverb(room);
            tryStartSound(0x61D5, &this->eyePos, 0, reverb);
        } else
            this->mBrk.mFrameCtrl.mAttribute = 0;
        type = this->type;
        if (type == 5 || type == 6) {
            SaveHD* save = trySave.mPointer;
            bool carrying = u32(this->mode) == 2;
            if (carrying)
                tryEventOn(save->mEvent, type == 5 ? 0x108 : 0x110);
            else
                tryEventOff(save->mEvent, type == 5 ? 0x108 : 0x110);
        }
        mDoExt_baseAnm_play(&this->mBrk);
    }
    u32 vy = u32(bits(this->speed.y)), y = u32(bits(this->current.pos.y));
    bits(this->mPreviousSpeedY) = vy;
    u32 vx = u32(bits(this->speed.x)), vz = u32(bits(this->speed.z)), z = u32(bits(this->current.pos.z)),
        x = u32(bits(this->current.pos.x));
    PlayHD* play = playView();
    tryCorrectPosition(&this->mAcch, play->mBgS);
    play = playView();
    s32 moving = tryMovingGround(play->mBgS, static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd));
    u32 mode = this->mode;
    if (moving)
        this->mMovingGround = 1;
    if (mode == MODE_RESTART || mode == MODE_CARRY) {
        bits(this->current.pos.x) = x;
        bits(this->speed.y) = vy;
        bits(this->speed.z) = vz;
        bits(this->speed.x) = vx;
        bits(this->current.pos.z) = z;
        bits(this->current.pos.y) = y;
    }
    this->damage_bg_proc_directly();
    if (u32(this->mode) != 2) {
        this->tevStr.mRoomNo = u8(this->current.roomNo);
        play = playView();
        u32 color = tryPolyColor(play->mBgS, static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd));
        this->tevStr.mEnvrIdxOverride = u8(color);
    }
    if (s8(this->current.roomNo) != s8(this->home.roomNo)) {
        type = this->type;
        this->mLeftHomeRoom = 1;
        if (type >= daObjTry::Type_Max)
            type = checkedType(this);
        if (u8(tryAttributes.mRows[type].mClearSaveSwitch)) {
            if (tryParameter(this, 8, 8) != 255)
                this->prm_set_swSave(255);
        }
    }
    play = playView();
    if (u8(play->mEventRunning))
        this->mCorrecting = 1;
    return 1;
}

/* 023ABA2C */
void daObjTry::Act_c::cull_set_draw() { trySetCullSphere(this, 0.0f, 65.0f, 0.0f, 100.0f); }

/* 023AC050 */
void daObjTry::Act_c::cull_set_move() { trySetCullSphere(this, 0.0f, 65.0f, 0.0f, 300.0f); }

/* 023AC7E8 */
void daObjTry::Act_c::damaged() { fopAcM_cancelCarryNow(this); }

/* 023AC0B4 */
s32 daObjTry::Act_c::damage_cc_proc() {

    if (tryCheckAttackHit(&this->mCyl)) {
        tryClearAttackHit(&this->mCyl);
        f32 forward = f32(this->speedF);
        this->speedF = forward * 0.3f;
        return 0;
    }
    if (tryCheckTargetHit(&this->mCyl)) {
        tryGetTargetHit(&this->mCyl);
        u32 type = checkedType(this);
        s8 room = this->current.roomNo;
        u32 sound = u32(tryAttributes.mRows[type].mHitSound);
        tryHitSound(&this->eyePos, room, &this->mCyl, sound);
        type = checkedType(this);
        AttrHD* first = &tryAttributes.mRows[type];
        type = checkedType(this);
        AttrHD* second = &tryAttributes.mRows[type];
        u8 context = u8(second->mHitContext), soundEnvironment = u8(first->mHitEnvironment);
        this->set_senv(soundEnvironment, context);
        tryHitEffect(this, &this->mCyl);
        tryClearTargetHit(&this->mCyl);
    }
    return 0;
}

/* 023AC7EC */
s32 daObjTry::Act_c::damage_bg_proc() {

    s32 stay = s8(u8(tryStay.mValue));
    bool grounded = (u32(this->mAcch.m_flags) & 0x20) != 0;
    s32 room = s8(this->home.roomNo);
    s32 inWater = tryInWaterRegister(this);
    u32 mode = this->mode;
    bool submerged = false;
    if (mode == MODE_WAIT) {
        if (inWater) {
            this->se_fall_water();
            if (f32(this->speed.y) != 0.0f) {
                this->eff_hit_water_splash();
                this->mWaterSplashDone = 1;
            } else
                this->mWaterSplashDone = 0;
            this->mode_sink_init();
        }
    } else if (mode == MODE_DROP) {
        if (!grounded) {
            if (inWater) {
                this->se_fall_water();
                this->eff_hit_water_splash();
                this->mWaterSplashDone = 1;
                this->mode_sink_init();
            }
            return 0;
        }
        this->mode_wait_init();
    } else if (mode == MODE_SINK) {
        if (trySubmergedRegister(this)) {
            submerged = true;
            if (!u8(this->mWaterSplashDone))
                this->eff_hit_water_splash();
        } else if (!inWater)
            this->mode_wait_init();
    }
    bool dangerousGround = false;
    if (grounded) {
        PlayHD* play = playView();
        dangerousGround = tryGroundCode(play->mBgS, static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd)) == 4;
    }
    if (!dangerousGround && !submerged)
        return 0;
    if (room == stay) {
        u32 type = checkedType(this);
        if (u8(tryAttributes.mRows[type].mUnique) || !u8(this->mLeftHomeRoom)) {
            this->mode_restart_init();
            return 0;
        }
    }
    this->damaged();
    return 1;
}

/* 023AD1C0 */
void daObjTry::Act_c::damage_bg_proc_directly() {

    u32 mode = this->mode, flags = u32(this->mAcch.m_flags);
    bool grounded = (flags & 0x20) != 0;
    if (mode == MODE_DROP) {
        bool unlock = (flags & 0x200) != 0;
        if (!unlock)
            unlock = tryInWaterRegister(this) != 0 || grounded;
        if (unlock)
            this->cam_lockoff();
    }
    s16 timer = s16(this->mLandTimer);
    if (timer > 0) {
        this->mLandTimer = timer - 1;
        return;
    }
    if (!grounded) {
        this->mLandEffectDone = 0;
        return;
    }
    if (u8(this->mLandEffectDone))
        return;
    mode = this->mode;
    if (mode != MODE_WAIT && mode != MODE_DROP)
        return;
    u32 type = checkedType(this);
    u32 sound = u32(tryAttributes.mRows[type].mLandSound);
    PlayHD* play = playView();
    s32 material = tryMaterialSound(play->mBgS, static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd));
    s32 reverb = tryReverb(s32(s8(this->current.roomNo)));
    tryStartSound(sound, &this->eyePos, material, reverb);
    if (u32(this->mPreviousMode) != 4)
        this->eff_land_smoke();
    this->mLandEffectDone = 1;
    this->mLandTimer = 20;
    this->make_vib();
}

/* 023AC1EC */
u8 daObjTry::Act_c::chk_sink_water() {

    if (!(u32(this->mAcch.m_flags) & 0x800))
        return 0;
    u32 type = this->type;
    if (type >= daObjTry::Type_Max) {
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
        type = this->type;
    }
    f32 height = this->current.pos.y;
    f32 offset = f32(tryAttributes.mRows[type].mWaterOffset);
    f32 top = height + offset, water = f32(this->mAcch.m_wtr.m_height);
    return water > top;
}

/* 023AC5B8 */
u8 daObjTry::Act_c::chk_sinkdown_water() {

    if (!(u32(this->mAcch.m_flags) & 0x800))
        return 0;
    u32 type = this->type;
    if (type >= daObjTry::Type_Max) {
        JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
        type = this->type;
    }
    f32 offset = f32(u8(tryAttributes.mRows[type].mGroundOffset)), height = this->current.pos.y;
    f32 top = (height + offset) + 50.0f, water = f32(this->mAcch.m_wtr.m_height);
    return water > top;
}

/* 023ADCD8 */
void daObjTry::Act_c::calc_drop_param(f32* gravity, f32* drag, f32* stream) {

    u32 flags = u32(this->mAcch.m_flags), type = this->type;
    if (flags & 0x800) {
        f32 depth = f32(this->current.pos.y) - f32(this->mAcch.m_wtr.m_height);
        f32 submerged = 0.0f;
        if (depth < 0.0f) {
            if (type >= daObjTry::Type_Max)
                type = checkedType(this);
            f32 height = f32(u8(tryAttributes.mRows[type].mGroundOffset));
            submerged = 0.5f;
            if (depth > -height) {
                if (type >= daObjTry::Type_Max)
                    type = checkedType(this);
                height = f32(u8(tryAttributes.mRows[type].mGroundOffset));
                submerged = -(depth * (0.5f / height));
            }
        }
        f32 above = 1.0f - submerged;
        if (type >= daObjTry::Type_Max)
            type = checkedType(this);
        AttrHD* firstRow = &tryAttributes.mRows[type];
        if (type >= daObjTry::Type_Max)
            type = checkedType(this);
        f32 air = f32(tryAttributes.mRows[type].mAirDrag), water = f32(firstRow->mWaterDrag);
        *reinterpret_cast<be<f32>*>(drag) = gabi::fmadds(submerged, water, above * air);
        type = checkedType(this);
        firstRow = &tryAttributes.mRows[type];
        if (type >= daObjTry::Type_Max)
            type = checkedType(this);
        air = f32(tryAttributes.mRows[type].mAirStream);
        water = f32(firstRow->mWaterStream);
        *reinterpret_cast<be<f32>*>(stream) = gabi::fmadds(submerged, water, above * air);
        type = checkedType(this);
        firstRow = &tryAttributes.mRows[type];
        if (type >= daObjTry::Type_Max)
            type = checkedType(this);
        f32 buoyancy = f32(firstRow->mBuoyancy), base = f32(tryAttributes.mRows[type].mGravity);
        *reinterpret_cast<be<f32>*>(gravity) = gabi::fmadds(submerged, buoyancy, base);
    } else {
        if (type >= daObjTry::Type_Max)
            type = checkedType(this);
        bits(*reinterpret_cast<be<f32>*>(drag)) = u32(bits(tryAttributes.mRows[type].mAirDrag));
        type = checkedType(this);
        bits(*reinterpret_cast<be<f32>*>(stream)) = u32(bits(tryAttributes.mRows[type].mAirStream));
        type = checkedType(this);
        bits(*reinterpret_cast<be<f32>*>(gravity)) = u32(bits(tryAttributes.mRows[type].mGravity));
    }
}

/* 023ADBFC */
s32 daObjTry::Act_c::bound() {

    u32 flags = u32(this->mAcch.m_flags);
    s32 grounded = (flags >> 5) & 1;
    if (flags & 0x10) {
        s32 normalYaw = s16(this->mAcchCir.m_wall_angle_y);
        f32 forward = f32(this->speedF);
        s32 yaw = s16(this->current.angle.y);
        flags = u32(this->mAcch.m_flags);
        this->speedF = forward * 0.3f;
        this->current.angle.y = u16(normalYaw * 2 - (yaw + 0x8000));
    }
    if (flags & 0x80) {
        f32 bounce = std::fabs(f32(this->mPreviousSpeedY) * -0.6f);
        if (bounce > 15.0f) {
            this->speed.y = 15.0f;
            return 0;
        }
    } else if (flags & 0x20) {
        tryAddCalc(&this->speedF, 0.0f, 0.5f, 5.0f, 1.0f);
    }
    return grounded;
}

/* 023AC280 */
void daObjTry::Act_c::se_fall_water() {

    u32 material = 19;
    for (unsigned i = 0; i < 2; ++i) {
        cBgS_PolyInfo* polygon = i == 0 ? static_cast<cBgS_PolyInfo*>(&waterView(this->mAcch.m_wtr))
                                        : static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd);
        if (u16(polygon->mBgIndex) < 256) {
            PlayHD* play = playView();
            material = tryMaterialSound(play->mBgS, polygon);
            break;
        }
    }
    u32 type = checkedType(this);
    s8 room = this->current.roomNo;
    u32 sound = u32(tryAttributes.mRows[type].mWaterSound);
    s32 reverb = tryReverb(room);
    tryStartSound(sound, &this->eyePos, material, reverb);
    type = checkedType(this);
    AttrHD* first = &tryAttributes.mRows[type];
    type = checkedType(this);
    AttrHD* second = &tryAttributes.mRows[type];
    u8 context = u8(second->mWaterContext), environment = u8(first->mWaterEnvironment);
    this->set_senv(environment, context);
}

/* 023AC070 */
void daObjTry::Act_c::set_senv(s32 sound, s32 context) {

    gabi::Local<cXyz> position;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    u32 x = u32(bits(this->current.pos.x)), y = u32(bits(this->current.pos.y));
    bits(position->x) = x;
    u32 z = u32(bits(this->current.pos.z)), id = u32(this->mBsPcId);
    bits(position->y) = y;
    bits(position->z) = z;
    trySoundEnvironment(position.get(), sound, id, context);
}

/* 023AD094 */
void daObjTry::Act_c::cam_lockoff() {

    PlayHD* play = playView();
    CameraHD* camera = play->mpCamera;
    u32 id = u32(this->mBsPcId);
    tryForceLockOff(camera->mCamera, id);
}

/* 023ABA4C */
void daObjTry::Act_c::set_mtx() {

    f32 x = this->current.pos.x, z = this->current.pos.z, y = this->current.pos.y;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    s16 rx = this->shape_angle.x, rz = this->shape_angle.z, ry = this->shape_angle.y;
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), rx, ry, rz);
    copyMatrix(this->model);
}

/* 023ABB10 */
void daObjTry::Act_c::init_mtx() {

    u32 model = this->model, y = u32(bits(this->scale.y)), x = u32(bits(this->scale.x)),
        z = u32(bits(this->scale.z));
    bits(modelView(model)->mScale.x) = x;
    bits(modelView(model)->mScale.y) = y;
    bits(modelView(model)->mScale.z) = z;
    this->set_mtx();
}

/* 023ACAA4 */
void daObjTry::Act_c::eff_set_bingo(s32 sound, s32 atHome) {

    if (this->bingoActive) {
        s32 reverb = tryReverb(s32(s8(this->current.roomNo)));
        tryStartSound(0x7026, &this->eyePos, 0, reverb);
        return;
    }
    initializeParticleColors(&tryColorGuard0, &tryColors0, 12, 24, 72, 72, 12, 24);
    initializeParticleColors(&tryColorGuard1, &tryColors1, 66, 73, 202, 103, 62, 202);
    initializeParticleColors(&tryColorGuard2, &tryColors2, 2, 12, 56, 56, 2, 12);
    initializeParticleColors(&tryColorGuard3, &tryColors3, 8, 62, 27, 42, 62, 98);
    u32 colorIndex = u32(u32(this->type) != 5);
    GXColor* primary = colorIndex ? &tryColors0.mEnvironment : &tryColors0.mPrimary;
    GXColor* environment = colorIndex ? &tryColors1.mEnvironment : &tryColors1.mPrimary;
    PlayHD* play = playView();
    void* particles = play->mpParticle;
    trySetParticle(particles, 0, 0x818E, &this->current.pos, &this->shape_angle, 0, 255,
                   &this->mFollowCallback, -1, primary, environment, 0);
    gabi::Local<csXyz> rotation;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    s16 yaw = s16(this->mCorrectionYaw);
    tryConstructAngles(rotation.get(), 0, yaw, 0);
    void* position = atHome ? &this->home.pos : &this->current.pos;
    play = playView();
    particles = play->mpParticle;
    u32 emitter = trySetParticle(particles, 0, 0x818F, position, rotation.get(), 0, 255, 0, -1,
                                 colorIndex ? &tryColors2.mEnvironment : &tryColors2.mPrimary,
                                 colorIndex ? &tryColors3.mEnvironment : &tryColors3.mPrimary, 0);
    this->emitter = emitter;
    if (sound) {
        s32 reverb = tryReverb(s32(s8(this->current.roomNo)));
        tryStartSound(0x695F, &this->eyePos, 0, reverb);
    }
    this->bingoActive = 1;
}

/* 023AD02C */
void daObjTry::Act_c::eff_clr_bingo() {

    if (this->bingoActive) {
        dPa_followEcallBack_end(&this->mFollowCallback);
        u32 emitter = this->emitter;
        if (emitter) {
            u32 flags = u32(emitterView(emitter)->mFlags);
            emitterView(emitter)->mMaxFrame = 0xFFFFFFFFu;
            emitterView(emitter)->mFlags = flags | 1;
            this->emitter = 0;
        }
        this->bingoActive = 0;
    }
}

/* 023AD0D0 */
void daObjTry::Act_c::eff_land_smoke() { tryLandEffect(this, &this->mAcch.m_gnd, 1.0f); }

/* 023AC3D4 */
void daObjTry::Act_c::eff_hit_water_splash() {

    gabi::Local<cXyz> position;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    u32 x = u32(bits(this->current.pos.x)), z = u32(bits(this->current.pos.z)),
        water = u32(bits(this->mAcch.m_wtr.m_height));
    bits(position->y) = water;
    bits(position->x) = x;
    bits(position->z) = z;
    tryCreateWaterPillar(position.get(), 0, 1.0f, 0.75f);
}

/* 023AD160 */
void daObjTry::Act_c::make_vib() {

    u32 nearby = this->check_circle() & 1;
    PlayHD* play = playView();
    gabi::Local<cXyz> direction;
    direction->z = 0.0f;
    direction->y = 1.0f;
    direction->x = 0.0f;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    tryStartShock(play->mVibration, nearby + 1, 1, direction.get());
}

/* 023AD0E0 */
s32 daObjTry::Act_c::check_circle() {

    gabi::Local<cXyz> difference, horizontal;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    PlayHD* play = playView();
    fopAc_ac_c* player = play->mpPlayer;
    trySubtract(&player->current.pos, difference.get(), &this->current.pos);
    u32 x = u32(bits(difference->x));
    bits(horizontal->x) = x;
    u32 z = u32(bits(difference->z));
    bits(horizontal->y) = 0;
    bits(horizontal->z) = z;
    f64 square = PSVECSquareMag(horizontal.get());
    f64 distance = trySqrt(square);
    return !(distance > 190.0);
}

/* 023AE544 */
f64 daObjTry::Act_c::get_water_h() {

    gabi::Local<WaterHD> check;
    gabi::Local<cXyz> callLinkage; // Reserve the 16-byte caller linkage area below live locals.
    tryConstructWaterCheck(check.get());
    f64 height = this->current.pos.y;
    if (tryQueryWaterHeight(&this->current.pos, check.get(), 100.0f))
        height = f32(check->mHeight);
    check->cBgS_Chk::__vtbl = gabi::ea(&tryWaterPolyVtable);
    check->dBgS_PolyPassChk::__vtbl = gabi::ea(&tryWaterGroupVtable);
    check->dBgS_GrpPassChk::__vtbl = gabi::ea(&tryWaterBaseVtable);
    tryDestroyWaterCheckBase(static_cast<cBgS_Chk*>(check.get()), 0);
    return height;
}

/* 023AD758 */
s32 daObjTry::Act_c::_execute() {

    this->cull_set_move();
    bool skip = false;
    if (!u8(this->mMovingGround) && this->mode == MODE_WAIT) {
        u32 flags = u32(this->mAcch.m_flags);
        if ((flags & 0x20) && !(flags & 0x80)) {
            u32 type = checkedType(this);
            if (!u8(tryAttributes.mRows[type].mSkipCull) && (this->actor_condition & fopAcCnd_NODRAW_e))
                skip = tryCullCheck(this) != 0;
        }
    }
    if (!skip) {
        this->mMovingGround = 0;
        bool damaged = this->damage_cc_proc() != 0;
        if (!damaged)
            damaged = this->damage_bg_proc() != 0;
        if (!damaged) {
            u8 delay = u8(this->mCollisionDelay);
            if (delay)
                this->mCollisionDelay = u8(delay - 1);
            this->mCorrecting = 0;
            if (!this->mode_proc_call())
                damaged = true;
            else {
                this->set_mtx();
                this->mStts.mRoomId = u8(this->current.roomNo);
                tryMoveCylinder(&this->mCyl, &this->current.pos);
                PlayHD* play = playView();
                tryRegisterCylinder(play->mCcS, &this->mCyl);
                u32 mode = this->mode;
                if (mode == MODE_DROP || mode == MODE_SINK || u8(this->mCollisionDelay)) {
                    play = playView();
                    tryRegisterMass(play->mMass, &this->mCyl, 3);
                }
                u32 type = this->type;
                bits(this->attention_info.position.x) = u32(bits(this->current.pos.x));
                if (type >= daObjTry::Type_Max) {
                    JUT_ASSERT_fail("d_a_obj_try.cpp", 0x3CE, "M_type >= 0 && M_type < Type_Max");
                    type = this->type;
                }
                f32 y = this->current.pos.y;
                mode = this->mode;
                u32 finalType = this->type, z = u32(bits(this->current.pos.z));
                f32 offset = f32(tryAttributes.mRows[type].mAttentionOffset);
                this->mPreviousMode = mode;
                y += offset;
                u32 x = u32(bits(this->attention_info.position.x));
                bits(this->attention_info.position.z) = z;
                bits(this->eyePos.x) = x;
                this->eyePos.y = y;
                bits(this->eyePos.z) = z;
                this->attention_info.position.y = y;
                type = finalType;
            }
        }
        if (damaged)
            fopAcM_delete(this);
    }
    u32 type = checkedType(this);
    if (!u8(tryAttributes.mRows[type].mAllowCorrection))
        this->field_0x64C = 0;
    this->field_0x64D = 0;
    this->cull_set_draw();
    return 1;
}

/* 023AD980 */
s32 daObjTry::Act_c::_draw() {

    auto* environment = dKy_getEnvlight();
    trySettingTev(environment, 0, &this->current.pos, &this->tevStr);
    environment = dKy_getEnvlight();
    u32 model = this->model;
    trySetLight(environment, modelView(model), &this->tevStr);
    u32 type = checkedType(this);
    if (s16(tryAttributes.mRows[type].mBrkIndex) >= 0) {
        model = this->model;
        f32 frame = f32(this->mBrk.mFrameCtrl.mFrame);
        u32 modelData = u32(gabi::ea(modelView(model)->mpModelData.get()));
        tryEnterBrk(&this->mBrk, gabi::at<J3DModelData>(modelData), frame);
    }
    model = this->model;
    tryUpdateModel(modelView(model), 0);
    return 1;
}

/* 023AE788 */
s32 Mthd_Create(Actor* actor) { return actor->_create(); }

/* 023AE78C */
s32 Mthd_Delete(Actor* actor) { return actor->_delete(); }

/* 023AE790 */
s32 Mthd_Execute(Actor* actor) { return actor->_execute(); }

/* 023AE794 */
s32 Mthd_Draw(Actor* actor) { return actor->_draw(); }

/* 023AE8E0 */
s32 Mthd_IsDelete(Actor* actor) { return 1; }

/* 023ACA24 */
f64 tryGamma(u32 intensity) {
    f32 fraction = f32(intensity) / 255.0f;
    f64 value = tryPow(f64(fraction), f64(2.2f));
    if (value < 0.0)
        value = 0.0;
    else if (value > 1.0)
        value = 1.0;
    return value;
}

// HD: compiler-generated and template code of this translation unit.

/* 023AE798 */
void __sinit_d_a_obj_try_cpp() {
    tryRegisters.mReadyA = 0;
    tryRegisters.mHeadA = 0;
    tryRegisters.mReadyB = 0;
    tryRegisters.mHeadB = 0;
    tryRegisterGlobal(&tryRecordA);
    bits(tryStatics.mLower) = u32(tryBounds.mLower);
    bits(tryStatics.mUpper) = u32(tryBounds.mUpper);
    tryInitMinusOne(&tryStatics.mMinusOne);
    tryRegisterGlobal(&tryRecordB);
    tryInitPlusOne(&tryStatics.mPlusOne);
    tryRegisterGlobal(&tryRecordC);
}

/* 023AE82C */
void SafeString_dt(void* object, s32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 023AE840 */
void SafeString_assureTerminationImpl(void* object) {}

/* 023AE844 */
void daObjTry_Act_c_dt(Actor* actor, s32 flags) {
    if (!actor)
        return;
    tryDestroyCylinder(&actor->mCyl, 2);
    tryDestroyStatus(&actor->mStts, 2);
    tryDestroyCircle(&actor->mAcchCir.m_cir, 2);
    actor->mAcch.dBgS_GrpPassChk::__vtbl = gabi::ea(&tryAcchGroupVtable);
    actor->mAcch.dBgS_PolyPassChk::__vtbl = gabi::ea(&tryAcchPolyVtable);
    tryDestroyAcch(&actor->mAcch, 0);
    tryDestroyActor(actor, 0);
    if (flags & 1)
        operator_delete(actor);
}
/* WWHD glue
ext 025D6CE8 s32 tryCullCheck(void* a0)
ext 025D6768 void trySetCullSphere(void* a0,f32 a1,f32 a2,f32 a3,f32 a4)
ext 0200ECD4 void tryAddCalc(void* a0,f32 a1,f32 a2,f32 a3,f32 a4)
ext 0200F62C void tryChasePosition(void* a0,void* a1,f32 a2)
ext 028E8DE8 f64 trySquareDistance(void* a0,void* a1)
ext 028F026C void tryRegisterGlobal(void* a0)
ext 025D50BC void tryDestroyActor(void* a0,s32 a1)
ext 020184DC void tryCylinderRadius(void* a0,f32 a1)
ext 02018428 void tryCylinderHeight(void* a0,f32 a1)
ext 0201ADE0 void trySubtract(void* a0,void* a1,void* a2)
ext 028F4384 f64 trySqrt(f64 a0)
ext 023123C0 void tryMoveStream(void* a0,void* a1,void* a2,f32 a3,f32 a4)
ext 025626A4 void trySettingTev(void* a0,s32 a1,void* a2,void* a3)
ext 02562F5C void trySetLight(void* a0,void* a1,void* a2)
ext 025160DC s32 tryCheckAttackHit(void* a0)
ext 02516094 void tryClearAttackHit(void* a0)
ext 025162A4 s32 tryCheckTargetHit(void* a0)
ext 02516300 u32 tryGetTargetHit(void* a0)
ext 0251621C void tryClearTargetHit(void* a0)
ext 025B8B7C void tryEventOff(void* a0,u32 a1)
ext 025B8B68 void tryEventOn(void* a0,u32 a1)
ext 02520488 void tryDeleteResource(const char* a0)
ext 025DAE64 void tryCreateWaterPillar(void* a0,s32 a1,f32 a2,f32 a3)
ext 028ED6F8 void tryInitMinusOne(void* a0)
ext 028EAB2C void tryInitPlusOne(void* a0)
ext 02515A70 void tryDestroyCylinder(void* a0,s32 a1)
ext 02515860 void tryDestroyStatus(void* a0,s32 a1)
ext 02018034 void tryDestroyCircle(void* a0,s32 a1)
ext 024EFD9C void tryDestroyAcch(void* a0,s32 a1)
ext 02515F14 void tryInitStatus(void* a0,s32 a1,s32 a2,void* a3)
ext 02516518 void trySetCylinder(void* a0,void* a1)
ext 025CB374 void tryStartShock(void* a0,u32 a1,s32 a2,void* a3)
ext 026066C4 u32 tryGetResource(void* a0,void* a1,s16 a2)
ext 025E38E0 u32 tryCreateModel(void* a0,u32 a1,u32 a2)
ext 025E8154 s32 tryInitBrk(void* a0,void* a1,void* a2,s32 a3,s32 a4,s32 a5,s32 a6,s32 a7,s32 a8,f32 a9)
ext 02333CBC u32 tryParameter(void* a0,s32 a1,s32 a2)
ext 02312968 void trySetCurrentRoom(void* a0,void* a1)
ext 024F22DC void tryConstructWaterCheck(void* a0)
ext 024F15A4 s32 tryQueryWaterHeight(void* a0,void* a1,f32 a2)
ext 02008B4C void tryDestroyWaterCheckBase(void* a0,s32 a1)
ext 025E83FC void tryEnterBrk(void* a0,void* a1,f32 a2)
ext 025E2DE0 void tryUpdateModel(void* a0,s32 a1)
ext 025165A4 void tryMoveCylinder(void* a0,void* a1)
ext 0200E240 void tryRegisterCylinder(void* a0,void* a1)
ext 02516C14 void tryRegisterMass(void* a0,void* a1,s32 a2)
ext 023129C4 void tryHitSound(void* a0,s32 a1,void* a2,u32 a3)
ext 02312E54 void tryHitEffect(void* a0,void* a1)
ext 024EECAC u32 tryMaterialSound(void* a0,void* a1)
ext 02520540 s32 tryReverb(s32 a0)
ext 025E1A40 void tryStartSound(u32 a0,void* a1,u32 a2,s32 a3)
ext 025E80D0 void tryConstructBrk(void* a0)
ext 024F0474 void tryConstructAcch(void* a0)
ext 024EFE94 void tryConstructAcchCircle(void* a0)
ext 0200BD2C void tryConstructStatus(void* a0)
ext 02515DA0 void tryConstructGStatus(void* a0)
ext 02515FB8 void tryConstructCollider(void* a0)
ext 02018590 void tryConstructCylinderShape(void* a0)
ext 025A5894 void tryConstructFollowCallback(void* a0,s32 a1,s32 a2)
ext 02520460 s32 tryLoadResource(void* a0,const char* a1)
ext 025D63E8 s32 tryEnterHeap(void* a0,TryHeapCallback a1,u32 a2)
ext 024EFF44 void trySetWall(void* a0,f32 a1,f32 a2)
ext 024F06B4 void trySetAcch(void* a0,void* a1,void* a2,void* a3,s32 a4,void* a5,void* a6,void* a7,void* a8)
ext 025D6870 void tryMovePosition(void* a0,void* a1)
ext 024F08A8 void tryCorrectPosition(void* a0,void* a1)
ext 025BA0C0 s32 tryCheckSwitch(void* a0,u32 a1,s32 a2)
ext 025D5218 u32 trySearchActor(TrySearchCallback a0,void* a1)
ext 028F4560 f64 tryPow(f64 a0,f64 a1)
ext 0255F458 void trySoundEnvironment(void* a0,s32 a1,u32 a2,s32 a3)
ext 025052BC void tryForceLockOff(void* a0,u32 a1)
ext 02311CD8 void tryLandEffect(void* a0,void* a1,f32 a2)
ext 023AC1EC s32 tryInWaterRegister(void* a0)
ext 023AC5B8 s32 trySubmergedRegister(void* a0)
ext 024EF0BC s32 tryGroundCode(void* a0,void* a1)
ext 024EEABC s32 tryMovingGround(void* a0,void* a1)
ext 024EEEB8 u32 tryPolyColor(void* a0,void* a1)
ext 025A847C u32 trySetParticle(void* a0,s32 a1,u32 a2,void* a3,void* a4,void* a5,s32 a6,void* a7,s32 a8,void* a9,void* a10,void* a11)
ext 0201A478 void tryConstructAngles(void* a0,s32 a1,s32 a2,s32 a3)
str 100321B0 "Hseki" in 023ABF90
str 10031D88 "d_a_obj_try.cpp" in 023AB8A0
str 10031D98 "M_type >= 0 && M_type < Type_Max" in 023AB8A0
str 10031D30 "d_a_obj_try.cpp" in 023AB6B0
str 10031D50 "M_type >= 0 && M_type < Type_Max" in 023AB6B0
str 10031D40 "mdl_data != 0" in 023AB6B0
str 10031D74 "brk != 0" in 023AB6B0
str 10031F18 "d_a_obj_try.cpp" in 023AC678
str 10031F28 "M_type >= 0 && M_type < Type_Max" in 023AC678
str 10031EA8 "d_a_obj_try.cpp" in 023AC424
str 10031EB8 "M_type >= 0 && M_type < Type_Max" in 023AC424
str 10032050 "d_a_obj_try.cpp" in 023AD980
str 10032060 "M_type >= 0 && M_type < Type_Max" in 023AD980
str 1003201C "d_a_obj_try.cpp" in 023AD758
str 1003202C "M_type >= 0 && M_type < Type_Max" in 023AD758
str 10031E08 "d_a_obj_try.cpp" in 023AC0B4
str 10031E18 "M_type >= 0 && M_type < Type_Max" in 023AC0B4
str 10031E70 "d_a_obj_try.cpp" in 023AC280
str 10031E80 "M_type >= 0 && M_type < Type_Max" in 023AC280
str 100321B0 "Hseki" in 023ABB7C
str 10031DCC "d_a_obj_try.cpp" in 023ABB7C
str 10031DDC "M_type >= 0 && M_type < Type_Max" in 023ABB7C
str 10031CF4 "d_a_obj_try.cpp" in 023AB584
str 10031D04 "M_type >= 0 && M_type < Type_Max" in 023AB584
str 10031E3C "d_a_obj_try.cpp" in 023AC1EC
str 10031E4C "M_type >= 0 && M_type < Type_Max" in 023AC1EC
str 10031EE0 "d_a_obj_try.cpp" in 023AC5B8
str 10031EF0 "M_type >= 0 && M_type < Type_Max" in 023AC5B8
str 10031F4C "d_a_obj_try.cpp" in 023AC7EC
str 10031F5C "M_type >= 0 && M_type < Type_Max" in 023AC7EC
str 10032090 "d_a_obj_try.cpp" in 023ADA30
str 100320A0 "M_type >= 0 && M_type < Type_Max" in 023ADA30
str 10031F8C "d_a_obj_try.cpp" in 023AD1C0
str 10031F9C "M_type >= 0 && M_type < Type_Max" in 023AD1C0
str 10032174 "d_a_obj_try.cpp" in 023AE5E4
str 10032184 "M_type >= 0 && M_type < Type_Max" in 023AE5E4
str 10032140 "d_a_obj_try.cpp" in 023AE2D4
str 10032150 "M_type >= 0 && M_type < Type_Max" in 023AE2D4
str 100320D0 "d_a_obj_try.cpp" in 023ADCD8
str 100320E0 "M_type >= 0 && M_type < Type_Max" in 023ADCD8
str 1003210C "d_a_obj_try.cpp" in 023ADFE4
str 1003211C "M_type >= 0 && M_type < Type_Max" in 023ADFE4
str 10031FE8 "d_a_obj_try.cpp" in 023AD358
str 10031FF8 "M_type >= 0 && M_type < Type_Max" in 023AD358
str 100321B0 "Hseki"
data tryAttributes 100321FC
data trySave 101F84DC
data tryResources 101F4F28
data tryZero 101FFBA8
data tryStay 1047E6C8
data tryBingo 101CD760
data tryRestart 101CD761
data tryRegisters 1046C7A0
data tryStatics 1046C794
data tryBounds 100321A8
data tryRecordA 101CD73C
data tryRecordB 101CD748
data tryRecordC 101CD754
data tryCylinderSource 100321B8
data tryTrig 104A44FC
data tryModes 10031FC0
data tryColorGuard0 1046C7B0
data tryColors0 101CD71C
data tryColorGuard1 1046C7B4
data tryColors1 101CD724
data tryColorGuard2 1046C7B8
data tryColors2 101CD72C
data tryColorGuard3 1046C7BC
data tryColors3 101CD734
data tryAcchGroupVtable 10031CC0
data tryAcchPolyVtable 10031CD0
data tryActorVtable 10031CE0
data tryAcchBaseVtable 10031CB0
data tryStatusVtable 1004AE88
data tryGStatusVtable 1004AEC0
data tryCylinderBaseVtable 100015A8
data tryShapeBaseVtable 10031C30
data tryHitVtable 1004B108
data tryCylinderShapeVtable 1004B150
data tryCylinderVtable 1004B160
data tryWaterPolyVtable 10031C50
data tryWaterGroupVtable 10031C70
data tryWaterBaseVtable 10031C40
*/
