/**
 * d_a_tn.cpp (WWHD)
 * Enemy - Darknut
 *
 * GameCube names and layouts,
 * verified against cking.rpx. HD differences are marked below.
 * The two readability ranges preserve all 56 native entries.
 */
#include "d/actor/d_a_tn.h"
#include <cmath>

static TnSpinColors l_spinColors;
static TnPower6 l_spinColorInitialized;
static TnEffects3 spin_eff_name;
static TnStaticStates l_staticStates;
static TnAngleRange l_randomAngleRange;
static TnGlobalObject l_globalObject0;
static TnGlobalObject l_globalObject1;
static TnGlobalObject l_globalObject2;
static u8 l_mathInitialized;
static u8 l_randomInitialized;
static u8 hio_set;
static u8 l_pauseTimer;
static TnResourceIndices3 part_bmd;
static TnResourceIndices3 part_brk;
static TnJointIndices3 j_dt;
static TnFlameJoints fire_j;
static TnFlameScales fire_sc;
static tnHIO_c l_tnHIO;
// HD: emitted HIO virtual-table word follows the GC-named data view.
static u32 l_tnHIOVtable;
static s8 s_check;
static s32 target_info_count;
static TnTargets target_info;
static s8 search_sp;
static TnJointChecks joint_check;
static TnPathChecks check_index;
static TnFloat4 xad;
static TnFloat4 zad;
static TnByte4 check_bit;
static TnPower6 atp_0;
static TnPower6 atp_1;
static TnColor6 prim;
static TnColor6 env;
static J3DModel* l_currentModel;
static Mtx34 l_currentMtx;
static f32 l_smokeDynamicsScale;
static f32 l_smokeParticleScale;
static f32 l_weaponRadius;
static f32 l_weapon2Radius;
static s16 l_hideShape;

static constexpr u32 TN_SAFESTRING_VTABLE = 0x10040388;


// Search callbacks share the unit's bounded target list. Address arithmetic
// uses guest words, preserving the native indexing arithmetic without host UB.
static void tn_appendTarget(fopAc_ac_c* actor) {
    const s32 count = gabi::load<s32>(0x1046E718);
    if (count < 10) {
        const u32 index = static_cast<u32>(count);
        gabi::store<u32>(0x1046E718, index + 1u);
        gabi::store<u32>(0x1046E740 + index * 4u, gabi::ea(actor));
    }
}


static void tn_removeParticleCallback(u32 callbackAddress) {
    gabi::at<TnParticleCallback>(callbackAddress)->remove();
}


static void tn_constructCylinder(dCcD_Cyl* cylinder) {
    tn_abi::dCcD_GObjInf_construct(cylinder);
    auto* shape = reinterpret_cast<TnShapeAttr*>(cylinder->mShapeAttr);
    shape->mAabVtable = 0x100403A0;
    shape->mShapeVtable = 0x100015A8;
    tn_abi::cM3dGCyl_construct(reinterpret_cast<u8*>(&cylinder->mCyl));
    cylinder->mCyl.__vtbl = 0x1004B150;
    cylinder->__vtbl_hitinf = 0x1004B108;
    shape->mShapeVtable = 0x1004B160;
}


static void tn_resumeWaiting(tn_class* actor) {
    actor->mDamageReaction.mAction = ACTION_JYUNKAI;
    path_check(actor);
    wait_set(actor);
    actor->mDamageReaction.mMode = 2;
}


static void tn_constructLineCheck(tn_linecheck* check) {
    cBgS_LinChk_ct(check);
    for (be<u8>& flag : check->mPassFlags) flag = 0;
    check->mGroupMask = 1;
    const u32 base = gabi::ea(check);
    gabi::store<u32>(base, base + 0x58u);
    gabi::store<u32>(base + 4u, base + 0x64u);
    check->mCheckVtable = 0x10040440;
    check->mLineVtable = 0x10040450;
    check->mPolyPassVtable = 0x10040470;
    check->mGroupPassVtable = 0x10040460;
}

static void tn_prepareLineCheckDestruction(tn_linecheck* check) {
    check->mPolyPassVtable = 0x10040470;
    check->mGroupPassVtable = 0x100403C0;
    check->mLineVtable = 0x100403B0;
}


static void tn_constructGroundCheck(tn_groundcheck* check) {
    tn_abi::cBgS_GndChk_construct(check);
    check->mCheckVtable = 0x100403D0;
    check->mGroundVtable = 0x100403E0;
    for (be<u8>& flag : check->mPassFlags) flag = 0;
    check->mPolyPassVtable = 0x10040400;
    check->mGroupPassVtable = 0x100403F0;
    check->mGroupMask = 1;
    const u32 base = gabi::ea(check);
    gabi::store<u32>(base, base + 0x40u);
    gabi::store<u32>(base + 4u, base + 0x4Cu);
}

static void tn_destroyGroundCheck(tn_groundcheck* check) {
    check->mGroundVtable = 0x100403E0;
    check->mPolyPassVtable = 0x10040400;
    check->mGroupPassVtable = 0x100403C0;
    tn_abi::cBgS_Chk_destroy(check, 0);
}


static void tn_setBreakParticleColors(JPABaseEmitter* emitter, const u8* primary, const u8* environment) {
    if (emitter == nullptr) return;
    auto* colors = reinterpret_cast<TnEmitterColors*>(emitter);
    for (u32 i = 0; i < 3; ++i) reinterpret_cast<be<u8>*>(&colors->mGlobalPrmColor)[i] = primary[i];
    for (u32 i = 0; i < 3; ++i) reinterpret_cast<be<u8>*>(&colors->mGlobalEnvColor)[i] = environment[i];
}


static void tn_leaveWeaponSearch(tn_class* actor) {
    actor->mDamageReaction.mAction = ACTION_JYUNKAI;
    path_check(actor);
    wait_set(actor);
    actor->mDamageReaction.mMode = 2;
}


static void tn_playBombReactionSound(tn_class* actor, u32 sound) {
    const u32 base = gabi::ea(actor);
    if (base + 0x37Cu == 0) return;
    const s8 room = actor->current.roomNo;
    const u32 id = base == 0 ? 0xFFFFFFFFu : static_cast<u32>(actor->mBsPcId);
    const s32 reverb = tn_getReverb(room);
    tn_abi::monsterSound(sound, &actor->eyePos, id, 0, reverb);
}


static mDoExt_McaMorf* tn_createResourceMorf(u32 modelIndex, u32 animationIndex, f32 speed) {
    J3DModelData* model = static_cast<J3DModelData*>(
        dComIfG_getObjectRes(STR(0x100406A7), modelIndex, TN_SAFESTRING_VTABLE));
    J3DAnmTransform* animation = static_cast<J3DAnmTransform*>(
        dComIfG_getObjectRes(STR(0x100406A7), animationIndex, TN_SAFESTRING_VTABLE));
    return mDoExt_McaMorf::create(nullptr, model, nullptr, nullptr, animation, 2, speed,
                                 0, -1, 1, nullptr, 0, 0x11020203);
}

static mDoExt_brkAnm* tn_createBrkAnimation() {
    mDoExt_brkAnm* animation = static_cast<mDoExt_brkAnm*>(operator_new(0x78));
    if (animation != nullptr) animation = tn_abi::brkConstruct(animation);
    return animation;
}

// The brk pointer is reloaded from its field after the resource lookup, as the original does.
static BOOL tn_initBrkAnimation(gptr<mDoExt_brkAnm>* brkField, J3DModel* model, u32 resourceIndex,
                               u8 loopMode, f32 speed) {
    void* resource = dComIfG_getObjectRes(STR(0x100406A7), resourceIndex, TN_SAFESTRING_VTABLE);
    const u32 brk = gabi::ea(static_cast<mDoExt_brkAnm*>(*brkField));
    const u32 modelData = gabi::ea(static_cast<J3DModelData*>(model->mModelData));
    return tn_abi::brkInit(brk, gabi::at<J3DModelData>(modelData), resource,
                           1, loopMode, speed, 0, -1, 0, 0);
}

static be<u32>* tn_jointCallbackAddress(J3DModel* model, u32 joint) {
    const u32 table = gabi::ea(static_cast<J3DModelData*>(model->mModelData));
    const u32 count = static_cast<u32>(gabi::at<J3DModelData>(table)->mJointNum);
    const u32 records = static_cast<u32>(gabi::at<J3DModelData>(table)->mJointNodes);
    return &gabi::at<J3DJoint>(records + (joint < count ? joint * 0x1Cu : 0u))->mCallBack;
}


static void tn_selectJointMatrix(mDoExt_McaMorf* morf, u32 joint) {
    const u32 model = gabi::ea(morf->getModel());
    const u32 buffer = gabi::ea(static_cast<J3DMtxBuffer_hd*>(gabi::at<J3DModel>(model)->mpMtxBuffer));
    const u16 flags = static_cast<u16>(gabi::at<J3DMtxBuffer_hd>(buffer)->mFlags);
    const u32 matrices = static_cast<u32>(gabi::at<J3DMtxBuffer_hd>(buffer)->mpAnmMtx);
    gabi::at<J3DMtxBuffer_hd>(buffer)->mFlags = flags | 0x10u;
    tn_abi::MTXCopy(gabi::at<Mtx34>(matrices + joint * 0x30u),
               calc_mtx());
}

static void tn_playPartImpact(tn_class* actor, u32 sound) {
    const u32 base = gabi::ea(actor);
    if (base == 0 || base + 0x37Cu == 0) return;
    const s32 reverb = tn_getReverb(static_cast<s8>(actor->current.roomNo));
    mDoAud_seStart(sound, &actor->eyePos, 0, reverb);
}

static s16 tn_randomPartRotation(f32 magnitude) {
    return static_cast<s16>(gabi::ftoi(cM_rndFX(magnitude)));
}

static void tn_moveFlyingPart(tn_class* actor, tn_p* part, int index, tn_groundcheck* ground,
                              f32 zero, f32 horizontalBounce, f32 randomRotation) {
    part->m18.x = part->m0C.x; part->m18.y = part->m0C.y; part->m18.z = part->m0C.z;
    tn_VECAdd(&part->m0C, &part->m24, &part->m0C);
    part->m24.y = static_cast<f32>(part->m24.y) - 5.0f;
    tn_abi::addAngles(&part->m30, &part->m36);
    gabi::Local<cXyz> motion;
    tn_abi::subtractPosition(&part->m0C, motion.get(), &part->m18);
    gabi::Local<cXyz> probeOffset;
    *probeOffset = *motion;
    const f32 motionSquared = tn_abi::VECSquareMag(probeOffset.get());
    const f32 motionLength = tn_sqrtf(motionSquared);
    if (motionLength > zero) {
        gabi::Local<tn_linecheck> line;
        tn_constructLineCheck(line.get());
        const u32 matrix = gabi::ea(calc_mtx());
        const s16 heading = cM_atan2s(static_cast<f32>(probeOffset->x), static_cast<f32>(probeOffset->z));
        tn_abi::cMtx_YrotS(gabi::at<Mtx34>(matrix), heading);
        probeOffset->x = zero;
        probeOffset->z = horizontalBounce;
        probeOffset->y = 30.0f;
        gabi::Local<cXyz> endpoint;
        MtxPosition(probeOffset.get(), endpoint.get());
        probeOffset->x = part->m0C.x;
        const f32 y = part->m0C.y;
        const f32 height = 30.0f;
        probeOffset->y = y;
        probeOffset->z = part->m0C.z;
        probeOffset->y = y + height;
        tn_VECAdd(endpoint.get(), &part->m0C, endpoint.get());
        tn_abi::dBgS_LinChk_Set(line.get(), probeOffset.get(), endpoint.get(), actor);
        const u32 play = gabi::ea(dComIfGp_get());
        if (tn_abi::LineCross(gabi::at<u8>(play + 0x12A0u), line.get()) != 0) {
            const f32 factor = -0.3f;
            const f32 vx = part->m24.x;
            const f32 oldX = part->m18.x;
            const f32 vz = part->m24.z;
            const f32 oldZ = part->m18.z;
            part->m0C.x = oldX;
            part->m0C.z = oldZ;
            part->m24.x = vx * factor;
            part->m24.z = vz * factor;
        }
        tn_prepareLineCheckDestruction(line.get());
        cBgS_LinChk_dt(line.get(), 0);
    }
    probeOffset->x = part->m0C.x;
    const f32 partY = part->m0C.y;
    const f32 probeHeight = 100.0f;
    probeOffset->y = partY;
    probeOffset->z = part->m0C.z;
    ground->mPosition.x = probeOffset->x;
    ground->mPosition.y = partY + probeHeight;
    ground->mPosition.z = probeOffset->z;
    probeOffset->y = partY + probeHeight;
    const u32 play = gabi::ea(dComIfGp_get());
    f32 groundHeight = tn_abi::GroundCross(gabi::at<u8>(play + 0x12A0u), ground);
    if (index == 0) groundHeight += 35.0f;
    else if (index == 1) groundHeight += REG0_F(1);
    else if (index == 2) groundHeight += -15.0f;
    else if (index == 3) groundHeight += REG0_F(3);
    if (!(static_cast<f32>(part->m0C.y) > groundHeight)) {
        const f32 vy = part->m24.y;
        const f32 bounceThreshold = -30.0f;
        part->m0C.y = groundHeight;
        if (vy < bounceThreshold) {
            if (index == 1) {
                part->m24.y = horizontalBounce;
                part->m24.x = cM_rndFX(horizontalBounce);
                part->m24.z = cM_rndFX(horizontalBounce);
                part->m36.x = tn_randomPartRotation(randomRotation);
                part->m36.y = tn_randomPartRotation(randomRotation);
                tn_playPartImpact(actor, 0x6996);
            } else {
                const f32 vx = part->m24.x;
                const f32 factor = 0.5f;
                const f32 vz = part->m24.z;
                const f32 verticalBounce = 10.0f;
                part->m24.y = verticalBounce;
                part->m24.x = vx * factor;
                part->m24.z = vz * factor;
                tn_playPartImpact(actor, 0x6997);
            }
            part->m44 = 20;
        } else {
            part->m36.z = 0;
            part->m36.y = 0;
            part->m24.z = zero;
            part->m24.y = zero;
            part->m36.x = 0;
            part->m24.x = zero;
            cLib_addCalcAngleS2(&part->m30.x, 0, 1, 0xC00);
            cLib_addCalcAngleS2(&part->m30.z, 0x4000, 1, 0xC00);
        }
    }
    if (part->m44 == 1) {
        part->m08 = -1;
        yoroi_break(actor, &part->m0C, static_cast<u8>(index));
        if (index == 0) {
            const u32 mantleId = actor->mMantPcId;
            if (mantleId != 0xFFFFu) {
                gabi::Local<be<u32>> id;
                *id = mantleId;
                const u32 mantle = mantleId == 0xFFFFFFFFu ? 0 : tn_abi::actorJudgeAddress(TN_ACTOR_ID_JUDGE, id.get());
                if (mantle != 0) fopAcM_delete(gabi::at<fopAc_ac_c>(mantle));
                actor->mMantPcId = 0xFFFF;
            }
        }
    }
}


static bool tn_stageEquals(u32 name) {
    gabi::Local<SafeString> expected;
    gabi::Local<SafeString> stage;
    expected->mStringTop = name;
    expected->__vtbl = TN_SAFESTRING_VTABLE;
    const u32 play = gabi::ea(dComIfGp_get());
    stage->mStringTop = play + 0x5134u;
    stage->__vtbl = TN_SAFESTRING_VTABLE;
    tn_assureTermination(expected.get());
    tn_assureTermination(expected.get());
    const u32 left = expected->mStringTop;
    tn_assureTermination(stage.get());
    const u32 right = stage->mStringTop;
    if (left == right) return true;
    for (u32 i = 0; i < 0x40001u; ++i) {
        const u8 a = gabi::load<u8>(left + i);
        const u8 b = gabi::load<u8>(right + i);
        if (a != b) return false;
        if (a == 0) return true;
    }
    return false;
}

static void tn_initializeColliders(tn_class* actor, u8 equipment) {
    const u32 base = gabi::ea(actor);
    const u32 status = base + 0xCE8u;
    const u8 health = (equipment & 1u) != 0 ? 30 : 15;
    actor->health = static_cast<u8>(health);
    actor->max_health = static_cast<u8>(health);
    tn_abi::dCcD_Stts_Init(gabi::at<dCcD_Stts>(status), 0xF0, 0xFF, actor);
    tn_abi::cylinderSet(&actor->mCoCyl, gabi::at<u8>(0x101D27BC));
    actor->mCoCyl.mStts = gabi::at<dCcD_Stts>(status);
    tn_abi::cylinderSet(&actor->mTgCyl, gabi::at<u8>(0x101D2800));
    actor->mTgCyl.mStts = gabi::at<dCcD_Stts>(status);
    tn_abi::sphereSet(&actor->mHeadSph, gabi::at<u8>(0x101D2688));
    actor->mHeadSph.mStts = gabi::at<dCcD_Stts>(status);
    tn_abi::sphereSet(&actor->mWeponSph, gabi::at<u8>(0x101D26C8));
    actor->mWeponSph.mStts = gabi::at<dCcD_Stts>(status);
    tn_abi::sphereSet(&actor->mWepon2Sph, gabi::at<u8>(0x101D2708));
    actor->mWepon2Sph.mStts = gabi::at<dCcD_Stts>(status);
    tn_abi::sphereSet(&actor->mDefenceSph, gabi::at<u8>(0x101D2748));
    actor->mDefenceSph.mStts = gabi::at<dCcD_Stts>(status);
}


static BOOL tn_eventRunning(u32 name) {
    const u32 background = gabi::ea(dComIfGp_get()) + 0x12A0u;
    const u32 manager = gabi::ea(dComIfGp_get()) + 0x52C4u;
    const s32 event = tn_abi::eventIndex(gabi::at<u8>(manager), STR(name), 0xFF);
    return tn_abi::eventExists(gabi::at<u8>(background + 0x4024u), event);
}

static s32 tn_getDemoStaff() {
    const u32 manager = gabi::ea(dComIfGp_get()) + 0x52C4u;
    return tn_abi::eventStaffId(gabi::at<u8>(manager), STR(0x1004037C), 0, 0);
}

static bool tn_staffActionEquals(s32 staff, u32 expected) {
    const u32 manager = gabi::ea(dComIfGp_get()) + 0x52C4u;
    const u32 action = tn_abi::eventCutName(gabi::at<u8>(manager), staff);
    for (u32 i = 0;; ++i) {
        const u8 a = gabi::load<u8>(action + i);
        const u8 b = gabi::load<u8>(expected + i);
        if (a != b) return false;
        if (a == 0) return true;
    }
}

static void tn_setDemoAnimationRates(tn_class* actor, mDoExt_McaMorf* body, f32 speed) {
    body->mFrameCtrl.mRate = speed;
    actor->mpArmorMorf->mFrameCtrl.mRate = speed;
    actor->mpShieldMorf->mFrameCtrl.mRate = speed;
}

static bool tn_demoAnimationStopped(mDoExt_McaMorf* body, f32 zero) {
    return (static_cast<u8>(body->mFrameCtrl.mState) & 1u) != 0 || static_cast<f32>(body->mFrameCtrl.mRate) == zero;
}


// Movement reconstruction is still incomplete. The following helpers express
// audited native subregions; none is a standalone verification entry.
// movement entry to return without ordinary action dispatch or velocity setup.
static bool tn_processAirborneAttack(tn_class* actor, f32 zero) {
    if (actor->mDamageReaction.mMode > -100) return false;
    const u32 base = gabi::ea(actor);
    const f32 knockback = static_cast<f32>(actor->mDamageReaction.m478);
    const f32 activeThreshold = 40.0f;
    actor->m03F0 = static_cast<s16>(5);
    if (!(std::fabs(knockback) > activeThreshold)) return true;
    if ((static_cast<u32>(actor->mDamageReaction.mAcch.m_flags) & 0x20u) != 0) return true;
    tn_abi::sphereSetCenter(reinterpret_cast<u8*>(&actor->mWeponSph.mSph), &actor->mDamageReaction.m100[12]);
    tn_abi::sphereSetRadius(reinterpret_cast<u8*>(&actor->mWeponSph.mSph), 60.0f);
    const u32 weaponFlags = static_cast<u32>(actor->mWeponSph.mObjAt.mSPrm);
    const u32 attackFlags = static_cast<u32>(actor->mWeponSph.mObjCo.mSPrm);
    actor->mWeponSph.mGObjAt.mSpl = static_cast<u8>(1);
    actor->mWeponSph.mObjAt.mSPrm = static_cast<u32>(weaponFlags & ~4u);
    actor->mWeponSph.mObjCo.mSPrm = static_cast<u32>(attackFlags | 1u);
    const u32 collider = base + 0x127Cu;
    const u32 registration = gabi::ea(dComIfGp_get()) + 0x26A4u;
    tn_abi::setCollision(gabi::at<u8>(registration), gabi::at<dCcD_Sph>(collider));
    const u32 collisionManager = gabi::ea(dComIfGp_get()) + 0x4EF8u;
    tn_abi::setCollisionMass(gabi::at<u8>(collisionManager), gabi::at<dCcD_Sph>(collider), 3);
    if (tn_abi::checkAttackHit(gabi::at<dCcD_Sph>(collider)) != 0) {
        const f32 verticalSpeed = static_cast<f32>(actor->speed.y);
        const f32 stopThreshold = -50.0f;
        if (verticalSpeed < stopThreshold) {
            const f32 impulse = 8000.0f;
            actor->speed.y = static_cast<f32>(zero);
            actor->mDamageReaction.m474 = static_cast<f32>(impulse);
        }
    }
    actor->mCoCyl.mObjCo.mSPrm = static_cast<u32>(static_cast<u32>(actor->mCoCyl.mObjCo.mSPrm) & ~1u);
    return true;
}

static void tn_updateMovementTarget(tn_class* actor, fopAc_ac_c* player) {
    const s16 targetTimer = static_cast<s16>(actor->m1436);
    cXyz* target;
    if (targetTimer != 0) {
        actor->m1436 = static_cast<u16>(static_cast<u16>(static_cast<u16>(targetTimer) - 1u));
        target = &actor->m1438;
    } else {
        target = gabi::at<cXyz>(gabi::ea(player) + 0x314u);
        actor->mDamageReaction.m714 = gabi::at<fopAc_ac_c>(gabi::ea(player));
    }
    gabi::Local<cXyz> delta;
    tn_abi::subtractPosition(target, delta.get(), &actor->current.pos);
    const f32 dx = delta->x;
    const f32 dy = delta->y;
    const f32 dz = delta->z;
    const f32 distance = tn_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    actor->mPlayerDistance = static_cast<f32>(distance);
    actor->mPlayerHeightDistance = static_cast<f32>(std::fabs(dy));
    const s16 heading = cM_atan2s(dx, dz);
    actor->mTargetAngle = static_cast<s16>(heading);
    const f32 radiusOffset = 80.0f;
    const f32 radiusTuning = REG0_F(3);
    tn_abi::cylinderSetRadius(reinterpret_cast<u8*>(&actor->mCoCyl.mCyl), radiusTuning + radiusOffset);
    const s8 cooldown = static_cast<s8>(actor->m1434);
    if (cooldown != 0) {
        const u8 next = static_cast<u8>(static_cast<u8>(cooldown) - 1u);
        actor->m1434 = static_cast<u8>(next);
        if (next == 0) {
            actor->mDamageReaction.mAction = ACTION_D_MAHI;
            actor->mDamageReaction.mMode = 0;
        }
    }
}

// dispatch for a pending reaction while retaining the common movement tail.
static bool tn_prepareNormalMovement(tn_class* actor, fopAc_ac_c* player) {
    const u32 weaponFlags = static_cast<u32>(actor->mWeponSph.mObjAt.mSPrm);
    const u32 bodyFlags = static_cast<u32>(actor->mCoCyl.mObjCo.mSPrm);
    const s16 reaction = static_cast<s16>(actor->mDamageReaction.m48A);
    actor->mWeponSph.mObjAt.mSPrm = static_cast<u32>(weaponFlags | 4u);
    actor->mCoCyl.mObjCo.mSPrm = static_cast<u32>(bodyFlags | 1u);
    if (reaction != 0 && static_cast<s16>(actor->mDamageReaction.m488) == 0) {
        if (reaction == 1) {
            actor->mCountDownTimers[1] = static_cast<s16>(0);
            actor->mDamageReaction.mAction = ACTION_FIGHT_RUN;
            actor->mDamageReaction.mMode = 0;
        } else {
            const f32 reactionSpeed = -30.0f;
            const s16 heading = static_cast<s16>(actor->mTargetAngle);
            actor->mDamageReaction.m4D0 = static_cast<s16>(heading);
            actor->speedF = static_cast<f32>(reactionSpeed);
        }
        return false;
    }
    tn_updateMovementTarget(actor, player);
    return true;
}

// Native actions with separately emitted implementations: dispatch at
// The four inline movement actions are handled by the caller.
static bool tn_dispatchSeparateMovementAction(tn_class* actor, s16 action) {
    switch (action) {
    case ACTION_P_LOST: p_lost(actor); break;
    case ACTION_B_NIGE: b_nige(actor); break;
    case ACTION_DEFENCE: defence(actor); break;
    case ACTION_HUKKI: hukki(actor); break;
    case ACTION_WEPON_SEARCH: wepon_search(actor); break;
    case ACTION_AITE_MIRU: aite_miru(actor); break;
    case ACTION_D_SIT: d_sit(actor); break;
    case ACTION_D_MAHI: d_mahi(actor); break;
    case ACTION_FAIL: fail(actor); break;
    case ACTION_YOGAN_FAIL: yogan_fail(actor); break;
    case ACTION_D_DOZOU: d_dozou(actor); break;
    case ACTION_S_DEMO: s_demo(actor); break;
    case ACTION_JYUNKAI: case ACTION_STAND: case ACTION_FIGHT_RUN: case ACTION_FIGHT: return false;
    default: break;
    }
    return true;
}

// The movement entry retains the zero/range constants and action value across
// action calls; they must be passed here rather than reloaded prematurely.
static void tn_finishMovement(tn_class* actor, s16 action, f32 zero, f32 rangeScale) {
    if (actor->mBehaviorType == BEHAVIOR_GUARDS_AREA && action == ACTION_FIGHT_RUN) {
        gabi::Local<cXyz> homeDelta;
        tn_abi::subtractPosition(&actor->home.pos, homeDelta.get(), &actor->current.pos);
        const u8 range = actor->mRangeOrFrozenAnim;
        f32 maxDistance;
        if (range != 0xFF) {
            const f32 scaledRange = static_cast<f32>(range) * rangeScale;
            maxDistance = scaledRange * 1.5f;
        } else {
            maxDistance = 750.0f;
        }
        const f32 squaredDistance = tn_abi::VECSquareMag(homeDelta.get());
        const f32 distance = tn_sqrtf(squaredDistance);
        if (distance > maxDistance) {
            if (actor->mBehaviorType == BEHAVIOR_GUARDS_AREA) actor->mDamageReaction.mAction = ACTION_STAND;
            actor->mCountDownTimers[1] = static_cast<s16>(0);
            actor->mCountDownTimers[2] = static_cast<s16>(60);
            actor->mDamageReaction.mMode = 51;
        }
        action = actor->mDamageReaction.mAction;
    }
    gabi::Local<cXyz> forward;
    forward->y = zero;
    forward->x = zero;
    forward->z = static_cast<f32>(actor->speedF);
    s16 heading;
    if (action != ACTION_HUKKI && action != ACTION_FAIL && static_cast<s16>(actor->mDamageReaction.m48A) == 0) {
        const s16 currentHeading = static_cast<s16>(actor->current.angle.y);
        const s16 headingOffset = static_cast<s16>(actor->m0418);
        actor->mDamageReaction.m482 = static_cast<s16>(currentHeading);
        const s16 reloadedHeading = static_cast<s16>(actor->current.angle.y);
        heading = static_cast<s16>(static_cast<u16>(reloadedHeading) + static_cast<u16>(headingOffset));
    } else {
        heading = static_cast<s16>(actor->mDamageReaction.m4D0);
    }
    const u32 matrix = gabi::ea(calc_mtx());
    tn_abi::cMtx_YrotS(gabi::at<u8>(matrix), heading);
    gabi::Local<cXyz> velocity;
    MtxPosition(forward.get(), velocity.get());
    actor->speed.x = static_cast<f32>(velocity->x);
    actor->speed.z = static_cast<f32>(velocity->z);
}

// Action 1: return to the home position, then resume watching the target.
// the surrounding movement entry is complete and still excluded from coverage.
static void tn_moveReturnHome(tn_class* actor, f32 zero, f32 one, f32 rangeScale, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    dComIfGp_get();
    const f32 groundOffset = 1000.0f;
    actor->mDamageReaction.m710 = static_cast<u8>(0);
    u32 contact = tn_abi::fopAcM_otoCheck(actor, groundOffset);
    const u32 mantleId = actor->mMantPcId;
    if (mantleId != 0xFFFF) {
        gabi::Local<be<u32>> id;
        *id = mantleId;
        const u32 mantle = mantleId == 0xFFFFFFFFu ? 0 : tn_abi::actorJudgeAddress(TN_ACTOR_ID_JUDGE, id.get());
        if (mantle != 0 && static_cast<s16>(gabi::at<TnMantle>(mantle)->m1C0E) != 0) contact |= 2u;
    }
    s16 mode = actor->mDamageReaction.mMode;
    switch (mode) {
    case 0: {
        const s32 pathDriven = actor->m0C34;
        actor->mDamageReaction.mMode = 1;
        anm_init(actor, pathDriven == 0 ? 0x3D : 0x39, static_cast<f32>(actor->m151C), J3DFrameCtrl::EMode_LOOP, one, -1);
        yoroi_anm_init(actor, dRes_INDEX_TN_BCK_YKIHON1_e, static_cast<f32>(actor->m151C), J3DFrameCtrl::EMode_NONE, one);
        [[fallthrough]];
    }
    case 1: {
        const f32 vx = static_cast<f32>(actor->speed.x);
        const f32 stepScale = 0.25f;
        const f32 homeX = static_cast<f32>(actor->home.pos.x);
        const f32 approachScale = 0.5f;
        cLib_addCalc2(&actor->current.pos.x, homeX, approachScale, vx * stepScale);
        const f32 reloadedStep = 0.25f;
        const f32 vz = static_cast<f32>(actor->speed.z);
        const f32 homeZ = static_cast<f32>(actor->home.pos.z);
        cLib_addCalc2(&actor->current.pos.z, homeZ, 0.5f, vz * reloadedStep);
        const s16 homeHeading = static_cast<s16>(actor->home.angle.y);
        cLib_addCalcAngleS2(&actor->current.angle.y, homeHeading, 2, 0x800);
        actor->speedF = static_cast<f32>(zero);
        break;
    }
    case 10: {
        s16 timer = static_cast<s16>(actor->mCountDownTimers[1]);
        if (timer == 30) {
            anm_init(actor, dRes_INDEX_TN_BCK_AKIDUKU1_e, 3.0f, J3DFrameCtrl::EMode_NONE, one, -1);
            timer = static_cast<s16>(actor->mCountDownTimers[1]);
        }
        if (timer == 0) {
            anm_init(actor, dRes_INDEX_TN_BCK_AKYORO1_e, rangeScale, J3DFrameCtrl::EMode_NONE, one, -1);
            actor->mDamageReaction.mMode = 11;
            const f32 random = cM_rndF(30.0f);
            const f32 minimum = 30.0f;
            actor->mCountDownTimers[3] = static_cast<s16>(static_cast<s16>(gabi::ftoi(random + minimum)));
        }
        break;
    }
    case 11:
        if (static_cast<s16>(actor->mCountDownTimers[3]) == 0) actor->mDamageReaction.mMode = 0;
        break;
    case 20:
        if (static_cast<s16>(actor->mCountDownTimers[1]) < 10) {
            actor->mDamageReaction.m710 = static_cast<u8>(1);
            if (static_cast<s16>(actor->mCountDownTimers[1]) == 0) {
                actor->mCountDownTimers[1] = static_cast<s16>(0);
                actor->mDamageReaction.mAction = ACTION_FIGHT_RUN;
                actor->mDamageReaction.mMode = 0;
                return;
            }
        }
        break;
    case 50:
        anm_init(actor, dRes_INDEX_TN_BCK_AKYORO1_e, rangeScale, J3DFrameCtrl::EMode_NONE, one, -1);
        mode = actor->mDamageReaction.mMode;
        actor->mCountDownTimers[1] = static_cast<s16>(50);
        actor->mDamageReaction.mMode = static_cast<s16>(static_cast<u16>(mode) + 1u);
        [[fallthrough]];
    case 51:
        actor->speedF = static_cast<f32>(zero);
        if (static_cast<s16>(actor->mCountDownTimers[1]) == 0) {
            fight_run_set(actor);
            mode = actor->mDamageReaction.mMode;
            actor->mCountDownTimers[2] = static_cast<s16>(60);
            actor->mDamageReaction.mMode = static_cast<s16>(static_cast<u16>(mode) + 1u);
        }
        break;
    case 52: {
        gabi::Local<cXyz> homeDelta;
        tn_abi::subtractPosition(&actor->home.pos, homeDelta.get(), &actor->current.pos);
        const f32 dx = homeDelta->x;
        const f32 dz = homeDelta->z;
        const s16 heading = cM_atan2s(dx, dz);
        actor->mDamageReaction.m4D0 = static_cast<s16>(heading);
        const f32 distance = tn_sqrtf(gabi::fmadds(dx, dx, dz * dz));
        const f32 runSpeed = static_cast<f32>(l_tnHIO.m054);
        const f32 stepScale = 0.25f;
        const f32 closeDistance = (runSpeed * stepScale) * acceleration;
        const s16 targetHeading = static_cast<s16>(actor->mDamageReaction.m4D0);
        if (distance < closeDistance) actor->mDamageReaction.mMode = 0;
        cLib_addCalcAngleS2(&actor->current.angle.y, targetHeading, 4, 0x1000);
        cLib_addCalc2(&actor->speedF, static_cast<f32>(l_tnHIO.m054), one, acceleration);
        break;
    }
    default: break;
    }
    mode = actor->mDamageReaction.mMode;
    if (mode < 10 && contact != 0) {
        actor->mDamageReaction.mMode = 10;
        const f32 random = cM_rndF(rangeScale);
        const f32 minimum = 45.0f;
        actor->mCountDownTimers[1] = static_cast<s16>(static_cast<s16>(gabi::ftoi(random + minimum)));
    }
    if (static_cast<s16>(actor->mCountDownTimers[2]) == 0 && actor->mDamageReaction.mMode != 20) {
        const u8 range = actor->mRangeOrFrozenAnim;
        const f32 distance = static_cast<f32>(actor->mPlayerDistance);
        const f32 detectionRange = range == 0xFF ? 500.0f : static_cast<f32>(range) * rangeScale;
        bool visible = false;
        if (distance < detectionRange) {
            const s16 viewLimit = get_view_H(actor);
            const u32 target = gabi::ea(static_cast<fopAc_ac_c*>(actor->mDamageReaction.m714));
            const s16 heading = static_cast<s16>(actor->mTargetAngle);
            visible = daTn_player_view_check(actor, gabi::at<cXyz>(target + 0x314u), heading, viewLimit) != 0;
        }
        if (visible) {
            if (actor->mDamageReaction.mMode >= 50) {
                actor->mCountDownTimers[1] = static_cast<s16>(0);
                actor->mDamageReaction.mAction = ACTION_FIGHT_RUN;
                actor->mDamageReaction.mMode = 0;
            } else {
                const f32 blend = 3.0f;
                actor->mDamageReaction.mMode = 20;
                anm_init(actor, dRes_INDEX_TN_BCK_AKIDUKU1_e, blend, J3DFrameCtrl::EMode_NONE, one, -1);
                actor->mCountDownTimers[1] = static_cast<s16>(30);
                if (base != 0) tn_playBombReactionSound(actor, 0x4885);
            }
        }
        if (daTn_checkNearbyBomb(actor) != 0) {
            actor->mDamageReaction.mAction = ACTION_B_NIGE;
            actor->mDamageReaction.mMode = 0;
        }
    }
    if (actor->m0C34 == 0 && search_wepon(actor) != 0) {
        actor->mDamageReaction.mAction = ACTION_WEPON_SEARCH;
        actor->mDamageReaction.mMode = -1;
    }
}

// Path target advancement shared by the patrol action's entry modes.
// indexed point record; missing linked paths enter the native assertion call.
static void tn_advancePatrolTarget(tn_class* actor) {
    const s32 pathDriven = actor->m0C34;
    actor->mDamageReaction.mMode = 1;
    if (pathDriven == 0 && actor->m13FF == 0) {
        way_pos_check(actor, &actor->mPathXyz);
        const f32 random = cM_rndF(25.0f);
        actor->mCountDownTimers[1] = static_cast<s16>(static_cast<s16>(gabi::ftoi(random + 30.0f)));
    } else if (static_cast<s8>(actor->m1424) == 0) {
        way_pos_check(actor, &actor->mPathXyz);
    } else {
        const u8 oldPoint = static_cast<u8>(actor->m1425);
        const u8 lookahead = actor->m1426;
        s8 pointIndex = static_cast<s8>(static_cast<u8>(oldPoint + lookahead));
        u32 path = gabi::ea(static_cast<tn_path*>(actor->ppd));
        actor->m1425 = pointIndex;
        const s8 countLow = static_cast<s8>(static_cast<u16>(gabi::at<tn_path>(path)->m_num));
        if (pointIndex >= countLow) {
            if ((static_cast<u8>(gabi::at<tn_path>(path)->m_closed) & 1u) != 0) {
                pointIndex = 0;
                actor->m1425 = 0;
            } else {
                actor->m1426 = 0xFF;
                pointIndex = static_cast<s8>(static_cast<u8>(static_cast<u16>(gabi::at<tn_path>(path)->m_num) - 2u));
                actor->m1425 = pointIndex;
            }
            const u16 nextPath = static_cast<u16>(gabi::at<tn_path>(path)->m_nextID);
            if (nextPath != 0xFFFF) {
                tn_path* linked = tn_abi::dPath_GetRoomPath(nextPath, static_cast<s8>(actor->current.roomNo));
                actor->ppd = linked;
                if (linked == nullptr) tn_abi::assertCondition(STR(0x100404B0), 0x7CC, STR(0x100404BC));
                pointIndex = actor->m1425;
                path = gabi::ea(static_cast<tn_path*>(actor->ppd));
            }
        } else if (pointIndex < 0) {
            path = gabi::ea(static_cast<tn_path*>(actor->ppd));
            actor->m1426 = 1;
            actor->m1425 = 1;
            pointIndex = 1;
        }
        const u32 points = gabi::ea(static_cast<path_layout::dPnt*>(gabi::at<tn_path>(path)->m_points));
        const u32 point = points + static_cast<u32>(static_cast<s32>(pointIndex)) * 16u;
        actor->mPathXyz.x = static_cast<f32>(static_cast<f32>(gabi::at<path_layout::dPnt>(point)->m_position.x));
        actor->mPathXyz.y = static_cast<f32>(static_cast<f32>(gabi::at<path_layout::dPnt>(point)->m_position.y));
        const f32 z = static_cast<f32>(gabi::at<path_layout::dPnt>(point)->m_position.z);
        actor->mCountDownTimers[2] = static_cast<s16>(30);
        actor->mPathXyz.z = static_cast<f32>(z);
        return;
    }
    actor->mCountDownTimers[2] = static_cast<s16>(30);
}

static u32 tn_currentPatrolPoint(tn_class* actor) {
    const u32 path = gabi::ea(static_cast<tn_path*>(actor->ppd));
    const s8 index = actor->m1425;
    return gabi::ea(static_cast<path_layout::dPnt*>(gabi::at<tn_path>(path)->m_points)) + static_cast<u32>(static_cast<s32>(index)) * 16u;
}

static u32 tn_patrolContact(tn_class* actor) {
    const f32 offset = 1000.0f;
    u32 contact = tn_abi::fopAcM_otoCheck(actor, offset);
    const u32 mantleId = actor->mMantPcId;
    if (mantleId != 0xFFFF) {
        gabi::Local<be<u32>> id;
        *id = mantleId;
        const u32 mantle = mantleId == 0xFFFFFFFFu ? 0 : tn_abi::actorJudgeAddress(TN_ACTOR_ID_JUDGE, id.get());
        if (mantle != 0 && static_cast<s16>(gabi::at<TnMantle>(mantle)->m1C0E) != 0) contact |= 2u;
    }
    return contact;
}

// ordinary wandering from path patrol; reaching selected path points pauses.
static void tn_patrolLocomotion(tn_class* actor, f32 one, f32 acceleration) {
    const bool usePath = actor->m0C34 != 0 || actor->m13FF != 0;
    const f32 targetX = static_cast<f32>(actor->mPathXyz.x);
    const f32 currentX = actor->current.pos.x;
    const f32 currentZ = actor->current.pos.z;
    const f32 dx = targetX - currentX;
    const f32 dz = static_cast<f32>(actor->mPathXyz.z) - currentZ;
    const f32 speed = (usePath ? static_cast<f32>(l_tnHIO.m04C) : static_cast<f32>(l_tnHIO.m050));
    const s32 turnLimit = usePath ? 0x400 : 0x1000;
    const s16 heading = cM_atan2s(dx, dz);
    const s8 activePath = static_cast<s8>(actor->m1424);
    actor->mDamageReaction.m4D0 = static_cast<s16>(heading);
    const bool followsPath = activePath != 0 && (actor->m0C34 != 0 || actor->m13FF != 0);
    const f32 distance = tn_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    const f32 stepScale = 0.25f;
    if (followsPath) {
        const f32 arrivalDistance = (speed * stepScale) * 15.0f;
        if (distance < arrivalDistance) {
            const u32 point = tn_currentPatrolPoint(actor);
            const u8 type = static_cast<u8>(gabi::at<path_layout::dPnt>(point)->mArg3);
            if (type == 3 || type == 7 || type == 8) {
                wait_set(actor);
                if (static_cast<u8>(gabi::at<path_layout::dPnt>(point)->mArg3) >= 7) {
                    const f32 random = cM_rndF(80.0f);
                    actor->mCountDownTimers[1] = static_cast<s16>(static_cast<s16>(gabi::ftoi(random + 70.0f)));
                }
                actor->mDamageReaction.mMode = 2;
            } else {
                actor->mDamageReaction.mMode = -1;
            }
        }
    } else {
        const f32 step = speed * stepScale;
        bool wait = distance < step + step;
        if (!wait && static_cast<s16>(actor->mCountDownTimers[2]) == 0) {
            wait = (static_cast<u32>(actor->mDamageReaction.mAcch.m_flags) & 0x10u) != 0;
            if (!wait) {
                const s16 currentHeading = static_cast<s16>(actor->current.angle.y);
                wait = tn_abi::groundCheck(actor, 1, currentHeading, 200.0f) != 0;
            }
        }
        if (wait) {
            wait_set(actor);
            actor->mDamageReaction.mMode = 2;
        }
    }
    const s16 desiredHeading = static_cast<s16>(actor->mDamageReaction.m4D0);
    cLib_addCalcAngleS2(&actor->current.angle.y, desiredHeading, 4, turnLimit);
    cLib_addCalc2(&actor->speedF, speed, one, acceleration);
}

// Action 0: path or free patrol, pauses and target/bomb/weapon detection.
static void tn_movePatrol(tn_class* actor, f32 zero, f32 one, f32 blend, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    dComIfGp_get();
    if (actor->mBehaviorType == BEHAVIOR_GUARDS_AREA) {
        actor->mDamageReaction.mAction = ACTION_STAND;
        actor->mDamageReaction.mMode = 50;
        return;
    }
    const f32 radiusTuning = REG0_F(14);
    const f32 radiusBase = 40.0f;
    tn_abi::cylinderSetRadius(reinterpret_cast<u8*>(&actor->mCoCyl.mCyl), radiusTuning + radiusBase);
    const s16 mode = actor->mDamageReaction.mMode;
    switch (mode) {
    case -10:
        actor->mCountDownTimers[1] = static_cast<s16>(60);
        actor->mDamageReaction.mMode = -9;
        [[fallthrough]];
    case -9: {
        const s16 timer = static_cast<s16>(actor->mCountDownTimers[1]);
        if (timer == 0) actor->mDamageReaction.mMode = 2;
        actor->speedF = static_cast<f32>(zero);
        if (timer > 30) return;
        break;
    }
    case 0:
        anm_init(actor, dRes_INDEX_TN_BCK_AWALK1_e, blend, J3DFrameCtrl::EMode_LOOP, one, dRes_INDEX_TN_BAS_AWALK1_e);
        yoroi_anm_init(actor, dRes_INDEX_TN_BCK_YWALK1_e, blend, J3DFrameCtrl::EMode_LOOP, one);
        [[fallthrough]];
    case -1:
        tn_advancePatrolTarget(actor);
        [[fallthrough]];
    case 1:
        tn_patrolLocomotion(actor, one, acceleration);
        break;
    case 2: {
        const s16 timer = static_cast<s16>(actor->mCountDownTimers[1]);
        actor->speedF = static_cast<f32>(zero);
        if (timer != 0) break;
        if (static_cast<s8>(actor->m1424) != 0 && (actor->m0C34 != 0 || actor->m13FF != 0)) {
            const u32 point = tn_currentPatrolPoint(actor);
            const u8 type = static_cast<u8>(gabi::at<path_layout::dPnt>(point)->mArg3);
            if (type == 7 || type == 8) {
                actor->mDamageReaction.mMode = 4;
                const f32 random = cM_rndF(100.0f);
                const f32 minimum = 100.0f;
                actor->mCountDownTimers[1] = static_cast<s16>(static_cast<s16>(gabi::ftoi(random + minimum)));
                const u8 pointType = static_cast<u8>(gabi::at<path_layout::dPnt>(point)->mArg3);
                const u16 heading = static_cast<u16>(actor->mDamageReaction.m4D0);
                const u16 offset = pointType == 7 ? 0xC000 : 0x4000;
                actor->mDamageReaction.m4D0 = static_cast<u16>(heading + offset);
                break;
            }
        }
        const s32 pathDriven = actor->m0C34;
        actor->mDamageReaction.mMode = 0;
        if (pathDriven == 0 && actor->m13FF == 0 && static_cast<s16>(actor->m03EA) == 0) actor->m13FF = 1;
        break;
    }
    case 4:
        cLib_addCalcAngleS2(&actor->current.angle.y, static_cast<s16>(actor->mDamageReaction.m4D0), 4, 0x1000);
        [[fallthrough]];
    case 3:
        actor->speedF = static_cast<f32>(zero);
        if (static_cast<s16>(actor->mCountDownTimers[1]) == 0) actor->mDamageReaction.mMode = 0;
        break;
    default: break;
    }
    const u32 contact = tn_patrolContact(actor);
    s32 pathDriven = actor->m0C34;
    const s8 encounter = static_cast<s8>(search_sp);
    const u32 reaction = contact + static_cast<u32>(static_cast<s32>(encounter));
    if (pathDriven != 0 || actor->m13FF != 0) {
        bool notice = reaction != 0;
        if (!notice) {
            const f32 distance = static_cast<f32>(actor->mPlayerDistance);
            const f32 detectionRange = static_cast<f32>(l_tnHIO.m028);
            if (distance < detectionRange) {
                const s16 viewLimit = get_view_H(actor);
                const u32 target = gabi::ea(static_cast<fopAc_ac_c*>(actor->mDamageReaction.m714));
                const s16 heading = static_cast<s16>(actor->mTargetAngle);
                notice = daTn_player_view_check(actor, gabi::at<cXyz>(target + 0x314u), heading, viewLimit) != 0;
                if (!notice) pathDriven = actor->m0C34;
            }
        }
        if (notice) {
            actor->mDamageReaction.mAction = ACTION_FIGHT_RUN;
            actor->mDamageReaction.mMode = -10;
            if (base != 0) tn_playBombReactionSound(actor, 0x4885);
            pathDriven = actor->m0C34;
        }
    }
    if (pathDriven == 0 && search_wepon(actor) != 0) {
        actor->mDamageReaction.mAction = ACTION_WEPON_SEARCH;
        actor->mDamageReaction.mMode = -1;
    }
    if (daTn_checkNearbyBomb(actor) != 0) {
        actor->mDamageReaction.mAction = ACTION_B_NIGE;
        actor->mDamageReaction.mMode = 0;
    }
}

enum class tn_combat_step { checkRange, checkObstacles, beginAttack };

// distinguishes ordinary range checks from the direct attack transition.
static tn_combat_step tn_combatLocomotion(tn_class* actor, f32 steering, f32 zero,
                                         f32 one, f32 blend, f32 acceleration) {
    const s16 mode = actor->mDamageReaction.mMode;
    switch (mode) {
    case -10:
        if (static_cast<s16>(actor->mCountDownTimers[4]) == 0) {
            anm_init(actor, dRes_INDEX_TN_BCK_AKIDUKU1_e, 3.0f, J3DFrameCtrl::EMode_NONE, one, -1);
            yoroi_anm_init(actor, dRes_INDEX_TN_BCK_YKIDUKU1_e, 3.0f, J3DFrameCtrl::EMode_NONE, one);
            actor->mDamageReaction.mMode = -9;
            return tn_combat_step::checkObstacles;
        }
        actor->mDamageReaction.mMode = 0;
        actor->mCountDownTimers[1] = static_cast<s16>(0);
        break;
    case -9:
        actor->speedF = static_cast<f32>(zero);
        if (tn_demoAnimationStopped(actor->mpBodyMorf, zero)) {
            actor->mDamageReaction.mMode = 0;
            actor->mCountDownTimers[1] = static_cast<s16>(0);
        }
        break;
    case 0:
        if (static_cast<s16>(actor->mCountDownTimers[1]) != 0) {
            actor->speedF = static_cast<f32>(zero);
            break;
        }
        fight_run_set(actor);
        actor->m1420 = static_cast<u32>(0);
        actor->mDamageReaction.mMode = 1;
        [[fallthrough]];
    case 1: {
        const bool armor = (static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0;
        const f32 speed = armor ? static_cast<f32>(l_tnHIO.m04C) * 1.5f : static_cast<f32>(l_tnHIO.m054);
        cLib_addCalc2(&actor->speedF, speed, one, acceleration);
        const BOOL way = daTn_player_way_check(actor);
        const f32 distance = static_cast<f32>(actor->mPlayerDistance);
        if (way != 0) {
            if (distance < static_cast<f32>(l_tnHIO.m02C)) {
                actor->mDamageReaction.mMode = 2;
                actor->mCountDownTimers[3] = static_cast<s16>(40);
            }
        } else if (distance < static_cast<f32>(l_tnHIO.m030)) {
            const f32 heightTuning = REG_F(10, 3);
            const f32 heightBase = 250.0f;
            if (static_cast<f32>(actor->mPlayerHeightDistance) < heightTuning + heightBase) return tn_combat_step::beginAttack;
        }
        break;
    }
    case 2: {
        const f32 steeringThreshold = 0.1f;
        const s32 pathDriven = actor->m0C34;
        s16 nextMode;
        if (std::fabs(steering) > steeringThreshold) {
            anm_init(actor, pathDriven != 0 ? 0x3B : 0x53, blend, J3DFrameCtrl::EMode_LOOP, one, pathDriven != 0 ? 0x10 : 0x23);
            nextMode = steering > zero ? 5 : 6;
        } else {
            anm_init(actor, pathDriven != 0 ? 0x3A : 0x52, blend, J3DFrameCtrl::EMode_LOOP, one, pathDriven != 0 ? 0x0F : 0x22);
            const f32 closeRange = static_cast<f32>(l_tnHIO.m030);
            const f32 distance = static_cast<f32>(actor->mPlayerDistance);
            nextMode = distance < closeRange ? 4 : 3;
        }
        const f32 randomRange = 20.0f;
        actor->mDamageReaction.mMode = nextMode;
        const f32 random = cM_rndF(randomRange);
        const f32 minimum = 20.0f;
        actor->mCountDownTimers[1] = static_cast<s16>(static_cast<s16>(gabi::ftoi(random + minimum)));
        break;
    }
    case 3:
        cLib_addCalc2(&actor->speedF, static_cast<f32>(l_tnHIO.m05C), one, 20.0f);
        if (static_cast<s16>(actor->mCountDownTimers[1]) == 0) actor->mDamageReaction.mMode = 2;
        break;
    case 4:
        if (static_cast<u8>(actor->m03FE) == 2) {
            actor->mDamageReaction.mMode = 3;
            anm_init(actor, dRes_INDEX_TN_BCK_AWALK1_e, blend, J3DFrameCtrl::EMode_LOOP, one, dRes_INDEX_TN_BAS_AWALK1_e);
        } else {
            cLib_addCalc2(&actor->speedF, -static_cast<f32>(l_tnHIO.m05C), one, 20.0f);
            if (static_cast<s16>(actor->mCountDownTimers[1]) == 0) actor->mDamageReaction.mMode = 2;
        }
        break;
    case 5: case 6: {
        const u8 obstacle = static_cast<u8>(actor->m03FE);
        s16 side = mode;
        if (mode == 5 && obstacle == 4) {
            side = 6;
            actor->mDamageReaction.mMode = 6;
        } else if (mode == 6 && obstacle == 8) {
            side = 5;
            actor->mDamageReaction.mMode = 5;
        }
        actor->m0418 = static_cast<u16>(side == 5 ? 0x4000 : 0xC000);
        cLib_addCalc2(&actor->speedF, static_cast<f32>(l_tnHIO.m060), one, 30.0f);
        if (static_cast<s16>(actor->mCountDownTimers[1]) == 0) actor->mDamageReaction.mMode = 2;
        break;
    }
    case 8:
        actor->speedF = static_cast<f32>(zero);
        if (static_cast<s16>(actor->mCountDownTimers[1]) == 0) actor->mDamageReaction.mMode = 2;
        break;
    default: break;
    }
    return actor->mDamageReaction.mMode < 3 ? tn_combat_step::checkObstacles : tn_combat_step::checkRange;
}

static void tn_beginCombatAttack(tn_class* actor) {
    actor->mDamageReaction.mAction = ACTION_FIGHT;
    actor->mDamageReaction.mMode = 0;
}

// an attack has been selected and the rest of the combat action is skipped.
static bool tn_combatRangeTransition(tn_class* actor, fopAc_ac_c* player) {
    const f32 closeRange = static_cast<f32>(l_tnHIO.m02C);
    const f32 lostTargetMargin = 75.0f;
    f32 distance = static_cast<f32>(actor->mPlayerDistance);
    if (distance > closeRange + lostTargetMargin) {
        actor->mDamageReaction.mAction = ACTION_JYUNKAI;
        path_check(actor);
        wait_set(actor);
        actor->mDamageReaction.mMode = 0;
        return true;
    }
    if (actor->m0C34 != 0) {
        if (tn_checkCutCharge(player) != 0) {
            actor->mDamageReaction.mAction = ACTION_FIGHT;
            actor->mDamageReaction.mMode = 10;
            return true;
        }
        const f32 attackRange = static_cast<f32>(l_tnHIO.m030);
        const f32 rangeMargin = 62.5f;
        distance = static_cast<f32>(actor->mPlayerDistance);
        if (!(distance < attackRange + rangeMargin)) return false;
        const f32 reloadedMargin = 62.5f;
        if (!(distance > attackRange - reloadedMargin)) return false;
    } else {
        const f32 attackRange = static_cast<f32>(l_tnHIO.m030);
        const f32 rangeMargin = 62.5f;
        if (!(distance < attackRange + rangeMargin)) return false;
        const f32 reloadedMargin = 62.5f;
        if (!(distance > attackRange - reloadedMargin)) return false;
    }
    if (static_cast<s16>(actor->mCountDownTimers[3]) != 0) return false;
    const s16 delay = static_cast<s16>(l_tnHIO.m06C);
    const f32 randomRange = 100.0f;
    actor->mCountDownTimers[3] = static_cast<s16>(delay);
    const f32 random = cM_rndF(randomRange);
    if (!(random < static_cast<f32>(l_tnHIO.m070))) return false;
    const f32 heightTuning = REG_F(10, 3);
    const f32 heightBase = 250.0f;
    if (!(static_cast<f32>(actor->mPlayerHeightDistance) < heightTuning + heightBase)) return false;
    tn_beginCombatAttack(actor);
    return true;
}

// Combat response to an incoming thrown object or the player's attack.
static bool tn_combatDefensiveReaction(tn_class* actor, fopAc_ac_c* player, f32 one, f32 blend) {
    const u32 base = gabi::ea(actor);
    if (static_cast<s16>(actor->m03F2) != 0) return false;
    const u32 thrown = tn_abi::searchActors(gabi::guest_fn((const void*)&shot_s_sub), actor);
    bool incoming = false;
    if (thrown != 0 && static_cast<f32>(gabi::at<fopAc_ac_c>(thrown)->speedF) > blend) {
        gabi::Local<cXyz> delta;
        tn_abi::subtractPosition(gabi::at<cXyz>(thrown + 0x314u), delta.get(), &actor->eyePos);
        const f32 squaredDistance = tn_abi::VECSquareMag(delta.get());
        const f32 distance = tn_sqrtf(squaredDistance);
        const f32 thrownSpeed = static_cast<f32>(gabi::at<fopAc_ac_c>(thrown)->speedF);
        incoming = distance < thrownSpeed * blend;
    }
    const u32 playerAttention = gabi::ea(dComIfGp_get()) + 0x5804u;
    if (daTn_player_way_check(actor) == 0) return false;
    if (!incoming) {
        if (static_cast<u8>(reinterpret_cast<TnPlayer*>(player)->mCutType) == 0) return false;
        if (tn_abi::attentionLockon(gabi::at<u8>(playerAttention)) == 0 &&
            (static_cast<u32>(gabi::at<TnAttention>(playerAttention)->mFlags) & 0x20000000u) == 0) return false;
        if (tn_abi::attentionActionTarget(gabi::at<u8>(playerAttention), 0) != base) return false;
    }
    const u32 frameCounter = static_cast<u32>(actor->m03D8);
    if ((frameCounter & 3u) == 0) {
        const s8 pieces = actor->mRemainingEquipmentPieces;
        if ((static_cast<u8>(pieces) & 4u) != 0) {
            const f32 random = cM_rndF(one);
            const f32 defenceThreshold = 0.8f;
            if (!(random > defenceThreshold)) {
                actor->mDamageReaction.mAction = ACTION_DEFENCE;
                actor->mDamageReaction.mMode = 0;
                const u8 playerAttack = static_cast<u8>(reinterpret_cast<TnPlayer*>(player)->mCutType);
                actor->mCountDownTimers[1] = static_cast<s16>(playerAttack == 10 ? 30 : 15);
                if (base != 0) tn_playBombReactionSound(actor, 0x488B);
                return true;
            }
            if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0) return false;
        } else if ((static_cast<u8>(pieces) & 1u) != 0) {
            return false;
        }
    } else if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0) {
        return false;
    }
    const s32 pathDriven = actor->m0C34;
    actor->mDamageReaction.mAction = ACTION_FIGHT;
    actor->mDamageReaction.mMode = -20;
    actor->m03F0 = static_cast<s16>(10);
    anm_init(actor, pathDriven != 0 ? 0x48 : 0x49, 2.0f, J3DFrameCtrl::EMode_NONE, one, pathDriven != 0 ? 0x1B : 0x1C);
    const f32 forwardTuning = REG0_F(3);
    const f32 forwardBase = -60.0f;
    const f32 verticalBase = 80.0f;
    actor->speedF = static_cast<f32>(forwardTuning + forwardBase);
    const f32 verticalTuning = REG0_F(4);
    actor->speed.y = static_cast<f32>(verticalTuning + verticalBase);
    tn_playBombReactionSound(actor, 0x488A);
    return true;
}

static void tn_combatObstacles(tn_class* actor) {
    const u32 target = gabi::ea(static_cast<fopAc_ac_c*>(actor->mDamageReaction.m714));
    if (daTn_player_bg_check(actor, gabi::at<cXyz>(target + 0x314u)) != 0) {
        actor->mDamageReaction.mAction = ACTION_JYUNKAI;
        path_check(actor);
        wait_set(actor);
        actor->mDamageReaction.mMode = -10;
        return;
    }
    if (actor->m0C34 == 0 && search_wepon(actor) != 0) {
        actor->mDamageReaction.mAction = ACTION_WEPON_SEARCH;
        actor->mDamageReaction.mMode = -1;
    }
    if (daTn_checkNearbyBomb(actor) != 0) {
        actor->mDamageReaction.mAction = ACTION_B_NIGE;
        actor->mDamageReaction.mMode = 0;
    }
    const f32 distanceTuning = REG_F(6, 7);
    const f32 distanceBase = 90.0f;
    const s16 heading = static_cast<s16>(actor->current.angle.y);
    const u32 obstruction = tn_abi::groundCheck(actor, 4, heading, distanceTuning + distanceBase);
    actor->m03FE = static_cast<u8>(obstruction);
    actor->mCountDownTimers[4] = static_cast<s16>(50);
}

// locomotion, range, defensive-reaction and obstacle phases above.
static void tn_moveCombat(tn_class* actor, f32 zero, f32 one, f32 blend, f32 acceleration) {
    const u32 playerAddress = gabi::ea(static_cast<fopAc_ac_c*>(reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mPlayerInfo[0].mpPlayer));
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(playerAddress);
    const f32 steering = tn_abi::padStickX(0);
    const s16 frozen = actor->m03EC;
    const s16 heading = static_cast<s16>(actor->mTargetAngle);
    actor->mDamageReaction.m710 = static_cast<u8>(1);
    actor->mDamageReaction.m4D0 = static_cast<s16>(heading);
    if (frozen == 0) {
        const s16 mode = actor->mDamageReaction.mMode;
        if (mode != 0) cLib_addCalcAngleS2(&actor->current.angle.y, heading, 4, mode == 1 ? 0x800 : 0x400);
    }
    const tn_combat_step phase = tn_combatLocomotion(actor, steering, zero, one, blend, acceleration);
    if (phase == tn_combat_step::beginAttack) {
        tn_beginCombatAttack(actor);
        return;
    }
    if (phase == tn_combat_step::checkRange) {
        if (tn_combatRangeTransition(actor, player)) return;
        if (tn_combatDefensiveReaction(actor, player, one, blend)) return;
    }
    tn_combatObstacles(actor);
}

// The following phase must reload m0C48, as the native caller does.
static void tn_chooseAttack(tn_class* actor, f32 one, f32 armorBlend) {
    const u32 base = gabi::ea(actor);
    const s32 pathDriven = actor->m0C34;
    const f32 random = cM_rndF(100.0f);
    if (pathDriven != 0) {
        if (random < static_cast<f32>(l_tnHIO.m078)) {
            const bool armor = (static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0;
            anm_init(actor, armor ? 0x2E : 0x46, armorBlend, J3DFrameCtrl::EMode_NONE, one, armor ? 0x0B : 0x19);
            yoroi_anm_init(actor, dRes_INDEX_TN_BCK_YAT_TATE1_e, armorBlend, J3DFrameCtrl::EMode_NONE, one);
            actor->m0C48 = 0;
        } else {
            const bool armor = (static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0;
            anm_init(actor, armor ? 0x2F : 0x47, armorBlend, J3DFrameCtrl::EMode_NONE, one, armor ? 0x0C : 0x1A);
            yoroi_anm_init(actor, dRes_INDEX_TN_BCK_YAT_YOKO1_e, armorBlend, J3DFrameCtrl::EMode_NONE, one);
            actor->m0C48 = 1;
            if (base != 0) tn_playBombReactionSound(actor, 0x4886);
        }
    } else {
        const f32 threshold = static_cast<f32>(l_tnHIO.m07C);
        if (random < threshold || (static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0) {
            anm_init(actor, dRes_INDEX_TN_BCK_BAT_SYOUTEI_S1_e, 3.0f, J3DFrameCtrl::EMode_NONE, one, dRes_INDEX_TN_BAS_BAT_SYOUTEI_S1_e);
            actor->m0C48 = 2;
            if (base != 0) tn_playBombReactionSound(actor, 0x488F);
        } else {
            anm_init(actor, dRes_INDEX_TN_BCK_BAT_KERI1_e, 3.0f, J3DFrameCtrl::EMode_NONE, one, dRes_INDEX_TN_BAS_BAT_KERI1_e);
            actor->m0C48 = 3;
            if (base != 0) tn_playBombReactionSound(actor, 0x4891);
        }
    }
    actor->m13FD = 0;
    actor->mCountDownTimers[2] = static_cast<s16>(8);
    actor->m0C66 = 1;
    actor->m0C67 = static_cast<u8>(0);
    actor->mDamageReaction.mMode = 1;
}

static bool tn_attackAnimation(tn_class* actor, fopAc_ac_c* player, f32 zero,
                               f32 one, f32 blend, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    const s32 type = actor->m0C48;
    u32 body = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
    bool decay = true;
    if (type == 3) {
        const f32 frame = static_cast<f32>(gabi::at<mDoExt_McaMorf>(body)->mFrameCtrl.mFrame);
        if (!(frame < 12.0f || frame > 30.0f)) {
            const bool jumping = static_cast<s8>(actor->m0C67) != 0;
            actor->speedF = static_cast<f32>(jumping
                ? REG0_F(10) + 80.0f
                : REG0_F(9) + 50.0f);
            decay = false;
        }
    } else if (type == 4) {
        const f32 frame = static_cast<f32>(gabi::at<mDoExt_McaMorf>(body)->mFrameCtrl.mFrame);
        if (!(frame < 4.0f || frame > 21.0f)) {
            actor->speedF = static_cast<f32>(REG0_F(16) + 50.0f);
            body = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
            if (gabi::ftoi(static_cast<f32>(gabi::at<mDoExt_McaMorf>(body)->mFrameCtrl.mFrame)) == 20 && base + 0x37Cu != 0)
                tn_playBombReactionSound(actor, 0x4889);
            decay = false;
        }
    }
    if (decay) cLib_addCalc0(&actor->speedF, one,
                           REG0_F(10) + 20.0f);

    f32 start = 1000.0f;
    f32 end = start;
    u8 attackClass = 0;
    if (actor->m0C66 > 0) {
        s32 activeType = actor->m0C48;
        body = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
        if (activeType == 0) {
            const s32 frame = gabi::ftoi(static_cast<f32>(gabi::at<mDoExt_McaMorf>(body)->mFrameCtrl.mFrame));
            attackClass = 2;
            start = zero;
            end = 25.0f;
            if (frame == 20 && base != 0 && base + 0x37Cu != 0) {
                tn_playBombReactionSound(actor, 0x4887);
                activeType = actor->m0C48;
                body = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
            }
        } else if (activeType == 1) {
            const u32 frame = static_cast<u32>(gabi::ftoi(static_cast<f32>(gabi::at<mDoExt_McaMorf>(body)->mFrameCtrl.mFrame)));
            attackClass = 1;
            start = zero;
            end = 25.0f;
            if (frame == static_cast<u32>(static_cast<s32>(REG0_S(2)) + 13)) {
                actor->m0432 = static_cast<u8>(0);
                const s32 timing = static_cast<s32>(REG0_S(3)) + static_cast<s16>(l_tnHIO.m020) + 14;
                actor->m0430 = static_cast<u16>(timing);
                const s32 angle = static_cast<s32>(static_cast<s16>(actor->current.angle.y)) + REG0_S(8) + 0x2000;
                actor->m042E = static_cast<u16>(angle);
                body = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
            }
            if (gabi::ftoi(static_cast<f32>(gabi::at<mDoExt_McaMorf>(body)->mFrameCtrl.mFrame)) == 20 && base != 0 && base + 0x37Cu != 0)
                tn_playBombReactionSound(actor, 0x4887);
            activeType = actor->m0C48;
            body = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
        }
        if (activeType == 0 || activeType == 4) {
            const bool armor = (static_cast<u8>(actor->mRemainingEquipmentPieces) & 1u) != 0;
            const s32 particleFrame = armor || activeType == 4 ? 21 : 19;
            if (gabi::ftoi(static_cast<f32>(gabi::at<mDoExt_McaMorf>(body)->mFrameCtrl.mFrame)) == particleFrame) {
                const s8 room = actor->current.roomNo;
                const u32 particle = gabi::ea(static_cast<dPa_control_c*>(reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mParticle));
                tn_abi::particleSet( gabi::at<void>(particle), 2, 0xA1B3,
                           &actor->current.pos, &actor->shape_angle,
                           nullptr, 0xB9, reinterpret_cast<void*>(&actor->mSmokeCb2), room, nullptr, nullptr, nullptr);
            }
        }
    }
    actor->mBtStartFrame = static_cast<f32>(start);
    actor->mBtAttackType = static_cast<u8>(attackClass);
    actor->mBtEndFrame = static_cast<f32>(end);
    actor->mBtMaxDis = static_cast<f32>(10000.0f);
    actor->mBtMaxDis = static_cast<f32>(l_tnHIO.m0D4);
    body = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
    actor->mBtNowFrame = static_cast<f32>(static_cast<f32>(gabi::at<mDoExt_McaMorf>(body)->mFrameCtrl.mFrame));
    fopAc_ac_c* hit = wepon_hit_check(actor);
    if (hit != nullptr && static_cast<s16>(hit->mProcName) == 0xA8) {
        if (tn_checkPlayerGuard(player) != 0) {
            const s32 activeType = actor->m0C48;
            if (activeType == 0 || activeType == 1) {
                body = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
                gabi::at<mDoExt_McaMorf>(body)->mFrameCtrl.mRate = -1.0f;
                actor->m0C66 = -1;
                body = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
                tn_abi::morfPlay(gabi::at<mDoExt_McaMorf>(body), &actor->eyePos, 0, 0);
                anm_init(actor, dRes_INDEX_TN_BCK_AHAJIKI1_e, REG0_F(4) + acceleration, J3DFrameCtrl::EMode_NONE, one, dRes_INDEX_TN_BAS_AHAJIKI1_e);
                yoroi_anm_init(actor, dRes_INDEX_TN_BCK_YHAJIKI1_e, REG0_F(4) + acceleration, J3DFrameCtrl::EMode_NONE, one);
            }
        }
    }
    body = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
    if (static_cast<f32>(gabi::at<mDoExt_McaMorf>(body)->mFrameCtrl.mFrame) < blend) {
        const s16 heading = static_cast<s16>(actor->mTargetAngle);
        actor->mDamageReaction.m4D0 = static_cast<s16>(heading);
        cLib_addCalcAngleS2(&actor->current.angle.y, heading, 4, 0x400);
        body = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
    }
    return (static_cast<u8>(gabi::at<mDoExt_McaMorf>(body)->mFrameCtrl.mState) & 1u) != 0 || static_cast<f32>(gabi::at<mDoExt_McaMorf>(body)->mFrameCtrl.mRate) == zero;
}

// Attack modes 10/12..18 circle the player until the player leaves its stance.
static bool tn_attackCircle(tn_class* actor, fopAc_ac_c* player, f32 steering,
                            bool specialStance, s16 mode, f32 zero, f32 one, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    if (mode == 10) {
        actor->m0C48 = 5;
        actor->mDamageReaction.mMode = 12;
        if (base != 0 && base + 0x37Cu != 0) tn_playBombReactionSound(actor, 0x4888);
        mode = 12;
    }
    if (mode == 12) {
        if (__builtin_fabsf(steering) > 0.1f) {
            anm_init(actor, dRes_INDEX_TN_BCK_AAT_KAITEN_LR1_e, acceleration, J3DFrameCtrl::EMode_LOOP, one, dRes_INDEX_TN_BAS_AAT_KAITEN_LR1_e);
            actor->mDamageReaction.mMode = steering > zero ? 15 : 16;
        } else {
            const f32 attackDistance = static_cast<f32>(l_tnHIO.m030);
            const f32 distance = static_cast<f32>(actor->mPlayerDistance);
            if (distance < attackDistance + 25.0f &&
                distance > attackDistance - 25.0f) {
                anm_init(actor, dRes_INDEX_TN_BCK_AAT_KAITEN1_e, acceleration, J3DFrameCtrl::EMode_LOOP, one, -1);
                actor->mDamageReaction.mMode = 18;
            } else {
                actor->mDamageReaction.mMode = distance < attackDistance ? 14 : 13;
                anm_init(actor, dRes_INDEX_TN_BCK_AAT_KAITEN_FB1_e, acceleration, J3DFrameCtrl::EMode_LOOP, one, dRes_INDEX_TN_BAS_AAT_KAITEN_FB1_e);
            }
        }
        yoroi_anm_init(actor, dRes_INDEX_TN_BCK_YAT_KAITEN1_e, acceleration, J3DFrameCtrl::EMode_LOOP, one);
        const f32 random = cM_rndF(20.0f);
        actor->mCountDownTimers[1] = static_cast<u16>(static_cast<u16>(gabi::ftoi(random + 20.0f)));
        cLib_addCalc2(&actor->speedF, static_cast<f32>(l_tnHIO.m05C), one,
                     20.0f);
    } else if (mode == 13 || mode == 14) {
        const f32 speed = static_cast<f32>(l_tnHIO.m05C);
        cLib_addCalc2(&actor->speedF, mode == 14 ? -speed : speed, one,
                     20.0f);
    } else if (mode == 15 || mode == 16) {
        actor->m0418 = static_cast<s16>(mode == 15 ? 0x4000 : -0x4000);
        cLib_addCalc2(&actor->speedF, static_cast<f32>(l_tnHIO.m060), one,
                     30.0f);
    } else {
        actor->speedF = static_cast<f32>(zero);
    }
    if (static_cast<s16>(actor->mCountDownTimers[1]) == 0) actor->mDamageReaction.mMode = 12;
    bool completed = tn_checkCutCharge(player) == 0;
    if (specialStance) {
        actor->mDamageReaction.mMode = 20;
        completed = false;
        anm_init(actor, dRes_INDEX_TN_BCK_AAT_KAITEN2_e, 2.0f, J3DFrameCtrl::EMode_NONE, one, dRes_INDEX_TN_BAS_AAT_KAITEN2_e);
        yoroi_anm_init(actor, dRes_INDEX_TN_BCK_YAT_KAITEN2_e, 2.0f, J3DFrameCtrl::EMode_NONE, one);
        if (base != 0 && base + 0x37Cu != 0) tn_playBombReactionSound(actor, 0x4889);
    }
    return completed;
}

static void tn_finishAttack(tn_class* actor, f32 one, f32 blend) {
    if (static_cast<f32>(actor->mPlayerDistance) < static_cast<f32>(l_tnHIO.m02C)) {
        const s16 limit = get_view_H(actor);
        const u32 target = gabi::ea(static_cast<fopAc_ac_c*>(actor->mDamageReaction.m714));
        const s16 heading = static_cast<s16>(actor->mTargetAngle);
        if (daTn_player_view_check(actor, gabi::at<cXyz>(target + 0x314u), heading, limit) != 0) {
            const f32 random = cM_rndF(one);
            if (random < 0.5f) {
                actor->mDamageReaction.mAction = ACTION_FIGHT_RUN;
                actor->mDamageReaction.mMode = 2;
                const f32 delay = cM_rndF(20.0f);
                actor->mCountDownTimers[3] = static_cast<u16>(static_cast<u16>(gabi::ftoi(delay + blend)));
            } else actor->mDamageReaction.mMode = 0;
        } else {
            actor->mDamageReaction.mAction = ACTION_P_LOST;
            actor->mDamageReaction.mMode = 0;
        }
    } else {
        actor->mDamageReaction.mAction = ACTION_JYUNKAI;
        path_check(actor);
        wait_set(actor);
        actor->mDamageReaction.mMode = 0;
    }
}

static void tn_moveAttack(tn_class* actor, f32 zero, f32 one, f32 blend, f32 acceleration) {
    const u32 base = gabi::ea(actor);
    const u32 playerBase = gabi::ea(static_cast<fopAc_ac_c*>(reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mPlayerInfo[0].mpPlayer));
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(playerBase);
    const f32 steering = tn_abi::padStickX(0);
    actor->mDamageReaction.m710 = static_cast<u8>(1);
    const u8 playerMode = static_cast<u8>(gabi::at<TnPlayer>(playerBase)->mCutType);
    const bool specialStance = playerMode == 8 || playerMode == 9;
    const s16 mode = actor->mDamageReaction.mMode;
    bool completed = false;
    switch (mode) {
    case -20:
        if (tn_demoAnimationStopped(actor->mpBodyMorf, zero)) {
            const f32 random = cM_rndF(one);
            if (random < 0.5f) {
                if (actor->m0C34 != 0) {
                    anm_init(actor, dRes_INDEX_TN_BCK_BAT_JUMP1_e, 3.0f, J3DFrameCtrl::EMode_NONE, one, dRes_INDEX_TN_BAS_BAT_JUMP1_e);
                    actor->m0C48 = 4;
                    if (base != 0 && base + 0x37Cu != 0) tn_playBombReactionSound(actor, 0x488A);
                } else {
                    anm_init(actor, dRes_INDEX_TN_BCK_BAT_KERI1_e, 3.0f, J3DFrameCtrl::EMode_NONE, one, dRes_INDEX_TN_BAS_BAT_KERI1_e);
                    actor->m0C48 = 3;
                    actor->m0C67 = static_cast<u8>(1);
                }
                actor->m13FD = 0;
                const u32 contact = static_cast<u32>(actor->mDamageReaction.mAcch.m_flags);
                actor->mCountDownTimers[2] = static_cast<s16>(8);
                actor->mDamageReaction.mMode = 1;
                actor->m0C66 = 1;
                if ((contact & 0x20u) != 0) actor->speedF = static_cast<f32>(zero);
                break;
            }
            completed = true;
        }
        if ((static_cast<u32>(actor->mDamageReaction.mAcch.m_flags) & 0x20u) != 0) actor->speedF = static_cast<f32>(zero);
        break;
    case 0:
        tn_chooseAttack(actor, one, acceleration);
        [[fallthrough]];
    case 1:
        completed = tn_attackAnimation(actor, player, zero, one, blend, acceleration);
        break;
    case 10: case 12: case 13: case 14: case 15: case 16: case 18:
        completed = tn_attackCircle(actor, player, steering, specialStance, mode, zero, one, acceleration);
        break;
    case 20:
        if (!tn_demoAnimationStopped(actor->mpBodyMorf, zero)) break;
        actor->mDamageReaction.mMode = 21;
        anm_init(actor, dRes_INDEX_TN_BCK_AAT_KAITEN3_e, one, J3DFrameCtrl::EMode_LOOP, one, dRes_INDEX_TN_BAS_AAT_KAITEN3_e);
        yoroi_anm_init(actor, dRes_INDEX_TN_BCK_YAT_KAITEN3_e, one, J3DFrameCtrl::EMode_LOOP, one);
        actor->m14F4 = static_cast<s16>(1);
        [[fallthrough]];
    case 21: {
        actor->speedF = static_cast<f32>(zero);
        const u8 stance = static_cast<u8>(gabi::at<TnPlayer>(playerBase)->mCutType);
        actor->m14F6 = static_cast<s16>(stance == 9 ? 7 : 3);
        wepon_hit_check(actor);
        if (!specialStance) {
            actor->mDamageReaction.mMode = 22;
            anm_init(actor, dRes_INDEX_TN_BCK_AAT_KAITEN4_e, one, J3DFrameCtrl::EMode_NONE, one, dRes_INDEX_TN_BAS_AAT_KAITEN4_e);
            yoroi_anm_init(actor, dRes_INDEX_TN_BCK_YAT_KAITEN4_e, one, J3DFrameCtrl::EMode_NONE, one);
            if (base + 0x37Cu != 0) tn_playBombReactionSound(actor, 0x4889);
        }
        const u32 mantleId = actor->mMantPcId;
        if (mantleId != 0xFFFFu) {
            gabi::Local<be<u32>> id;
            *id = mantleId;
            const u32 mantle = mantleId == 0xFFFFFFFFu ? 0 : tn_abi::actorJudgeAddress(TN_ACTOR_ID_JUDGE, id.get());
            if (mantle != 0) {
                cLib_addCalc2(&gabi::at<TnMantle>(mantle)->m1BF8, REG0_F(6) + 50.0f,
                             one, 20.0f);
                cLib_addCalc2(&gabi::at<TnMantle>(mantle)->m1BFC, REG0_F(7) + 0.9f,
                             one, 0.2f);
            }
        }
        break;
    }
    case 22:
        completed = tn_demoAnimationStopped(actor->mpBodyMorf, zero);
        break;
    default:
        break;
    }
    actor->mCountDownTimers[4] = static_cast<s16>(50);
    if (completed) tn_finishAttack(actor, one, blend);
}


// Darknut execution phases retain the native guest call frame.
#include <initializer_list>

namespace tn_execute {
enum class PrefixResult { Finished, Continue };
struct PrefixState { u32 playerAddress; };

// Snapshot all twelve source scalars before writing the model matrix, matching
// the native inline copies. This is not an extra guest PSMTXCopy call.
static void copyBaseMatrix(u32 source, u32 destination) {
    f32 values[12];
    for (u32 i = 0; i < 12; ++i) values[i] = gabi::load<f32>(source + i * 4);
    for (u32 i = 0; i < 12; ++i) gabi::store<f32>(destination + i * 4, values[i]);
}
static u32 findActor(u32 id) {
    gabi::Local<be<u32>> searchId;
    gabi::Local<u8[16]> outgoingLinkage;
    *searchId = id;
    return id == 0xFFFFFFFFu ? 0
        : tn_abi::actorJudgeAddress(TN_ACTOR_ID_JUDGE, searchId.get());
}
static void playBody(tn_class* actor) {
    tn_abi::morfPlay(actor->mpBodyMorf.get(), &actor->eyePos, 0, 0);
}
static void playArmor(tn_class* actor) {
    tn_abi::morfPlay(actor->mpArmorMorf.get(), nullptr, 0, 0);
}
static void playShield(tn_class* actor) {
    tn_abi::morfPlay(actor->mpShieldMorf.get(), nullptr, 0, 0);
}
static void moveDetachedParts(tn_class* actor) {
    for (int index = 0; index < 3; ++index) {
        if (actor->mParts[index].m08 >= 0)
            part_move(actor, index);
    }
}
static void decrementShort(be<s16>* field) {
    const s16 timer = *field;
    if (timer != 0) *field = static_cast<s16>(timer - 1);
}

// original TRUE result. Continue enters the regular damage/movement phases
// assembled by the full wrapper below. This helper alone is not Execute.
PrefixResult executePrefix(tn_class* actor, PrefixState& state) {
    auto play = dComIfGp_get();
    const u8 deletionSwitch = actor->mDisableSpawnOnDeathSwitch;
    state.playerAddress = gabi::ea(static_cast<fopAc_ac_c*>(reinterpret_cast<dComIfG_inf_c*>(play)->play.mPlayerInfo[0].mpPlayer));

    // HD-only-in-this-comparison prelude,024BD1E0..024BD284. Do not transplant
    // the GC unconditional4000-unit fall rule into this switch-gated branch.
    if (deletionSwitch != 0) {
        f32 floor = gabi::fsubs_ppc(actor->home.pos.y, 5000.0f);
        if (static_cast<f32>(actor->current.pos.y) < floor) {
            u32 save = dComIfGs_ea();
            tn_abi::saveOnSwitch(gabi::at<void>(save + 0x20), deletionSwitch,
                       static_cast<s8>(actor->current.roomNo));
            save = dComIfGs_ea();
            tn_abi::saveOnActor(gabi::at<void>(save + 0x20),
                       static_cast<u16>(actor->setID), static_cast<s8>(actor->home.roomNo));
            fopAcM_delete(actor);
            if (actor->m0C34 != 0) {
                const u32 weapon = findActor(actor->mBokoPcId);
                if (weapon != 0) fopAcM_delete(gabi::at<fopAc_ac_c>(weapon));
            }
            return PrefixResult::Finished;
        }
    }

    // Signed byte wrap is explicit;1670 has no accepted gameplay name yet.
    const s8 statusTimer = static_cast<s8>(actor->mStatusTimer);
    if (statusTimer != 0) {
        const s8 next = static_cast<s8>(static_cast<u8>(statusTimer) - 1);
        actor->mStatusTimer = static_cast<s8>(next);
        if (next == 0) actor->actor_status = static_cast<u32>(actor->actor_status) & ~0x4000u;
    }
    const BOOL frozen = tn_abi::enemy_ice(&actor->mEnemyIce);
    const f32 one = 1.0f;
    if (frozen != 0) {
        // Ice+6 is a signed light-shrink/state byte. Its meaning is provisional.
        if (static_cast<s8>(actor->mEnemyIce.mLightShrinkTimer) == 0) {
            if (actor->mEnemyIce.mFreezeTimer == 23) {
                actor->mpBodyMorf->mFrameCtrl.mRate = one;
                actor->mpShieldMorf->mFrameCtrl.mRate = one;
                actor->mpArmorMorf->mFrameCtrl.mRate = one;
                // Normal live actor/eye pointee domain; original contains the
                // redundant receiver/address guards before this sound call.
                const u32 id = static_cast<u32>(actor->mBsPcId);  // read before the reverb lookup
                const s32 reverb = tn_getReverb(static_cast<s8>(actor->current.roomNo));
                tn_abi::monsterSound(0x488A, &actor->eyePos, id, 0, reverb);
            }
            playBody(actor); playArmor(actor); playShield(actor);
        }
        const u32 model = gabi::ea(static_cast<J3DModel*>(actor->mpBodyMorf->mpModel));
        copyBaseMatrix(0x1048D0CC, model + 0xC8);
        tn_abi::morfCalc(actor->mpBodyMorf.get());
        tn_abi::morfCalc(actor->mpShieldMorf.get());
        moveDetachedParts(actor);
        actor->m02DD = static_cast<u8>(0);
        if (actor->mEnemyIce.mFreezeTimer == 1) {
            path_check(actor);
            wait_set(actor);
            tate_anm_init(actor, dRes_INDEX_TN_BCK_ATATE_ON1_e, 10.0f, J3DFrameCtrl::EMode_LOOP, one);
        }
        return PrefixResult::Finished;
    }

    // HD runs this block unconditionally; no GC HIO/pad pause gate is inserted.
    actor->m03D8 = static_cast<u32>(static_cast<u32>(actor->m03D8) + 1);
    for (auto& timer : actor->mCountDownTimers) decrementShort(&timer);
    for (be<s16>* timer : {&actor->m03EA, &actor->m03FC, &actor->m1402,
                           &actor->mDamageReaction.m48A, &actor->m03F0, &actor->m03F2})
        decrementShort(timer);
    if (static_cast<s8>(actor->m140C) != 0) {
        const u32 weapon = findActor(actor->mBokoPcId);
        if (weapon != 0) {
            actor->m140C = static_cast<u8>(0);
            tn_abi::fopAcM_setCarryNow(gabi::at<void>(weapon), 0);
        }
    }
    const f32 zero = 0.0f;
    bool attentionSuppressed;
    if (actor->m02C1 != 0) {
        bool released = REG0_S(0) != 0;
        if (!released) {
            const u32 save = dComIfGs_ea();
            released = tn_abi::saveIsSwitch(gabi::at<void>(save + 0x20),
                static_cast<u8>(actor->m02C1) - 1, static_cast<s8>(actor->current.roomNo)) != 0;
        }
        if (!released) {
            actor->attention_info.flags = static_cast<u32>(0);
            const u32 weapon = findActor(actor->mBokoPcId);
            if (weapon != 0) {
                const f32 hidden = -10000.0f;
                MtxTrans(hidden, hidden, zero, 0);
                const u32 model = gabi::ea(static_cast<J3DModel*>(gabi::at<TnBoko>(weapon)->mpModel));
                if (model != 0) copyBaseMatrix(gabi::ea(calc_mtx()), model + 0xC8);
            }
            if (actor->mMantPcId != 0xFFFFu) {
                const u32 mantle = findActor(actor->mMantPcId);
                if (mantle != 0) {
                    const f32 x = -10000.0f;
                    const f32 y = 20000.0f;
                    const f32 z = -21000.0f;
                    for (cXyz* position : {&gabi::at<TnMantle>(mantle)->m1BE0,
                                          &gabi::at<TnMantle>(mantle)->m1BEC}) {
                        position->x = x;
                        position->y = y;
                        position->z = z;
                    }
                }
            }
            return PrefixResult::Finished;
        }
        actor->m02C1 = 0;
        attentionSuppressed = static_cast<s8>(actor->m14F8) != 0;
    } else attentionSuppressed = static_cast<s8>(actor->m14F8) != 0;
    if (!attentionSuppressed) {
        actor->attention_info.flags = static_cast<u32>(4);
        actor->actor_status = static_cast<u32>(actor->actor_status) | 0x20;
    } else actor->attention_info.flags = static_cast<u32>(0);
    if (static_cast<s16>(actor->m03EE) == 0) {
        playBody(actor); playArmor(actor);
        if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 4) != 0) playShield(actor);
    }
    return PrefixResult::Continue;
}

// The camera object is obtained even when the demo state is zero.
static void updateDemoCameraAndSmoke(tn_class* actor) {
    gabi::Local<cXyz> cameraCenter;
    gabi::Local<cXyz> cameraEye;
    gabi::Local<cXyz> smokeOffset;
    gabi::Local<u8[16]> outgoingLinkage;
    const u32 play = gabi::ea(dComIfGp_get());
    const u32 cameraOwner = tn_abi::dCam_getCamera(gabi::at<void>(play));
    const u32 camera = tn_abi::dCam_getBody(gabi::at<void>(cameraOwner));
    s8 demo = static_cast<s8>(actor->m14F8);
    bool setCamera = demo != 0;
    if (demo == 50) {
        if (static_cast<u16>(actor->eventInfo.mCommand) != 2) {
            tn_abi::orderPotentialEvent(actor, 2, 0xFFFF, 0);
            actor->eventInfo.mCondition = static_cast<u16>(actor->eventInfo.mCondition) | 2;
            setCamera = false;
        } else {
            tn_abi::cameraStop(gabi::at<void>(camera));
            tn_abi::cameraTrimSize(gabi::at<void>(camera), 1);
            actor->m14F8 = static_cast<s8>(51);
            const u32 currentPlay = gabi::ea(dComIfGp_get());
            const u32 view = static_cast<u32>(gabi::at<dComIfG_inf_c>(currentPlay)->play.mCameraInfo[0].mpCamera);
            actor->mCameraEyePos.x = gabi::at<TnCameraView>(view)->mEye.x;
            actor->mCameraEyePos.y = gabi::at<TnCameraView>(view)->mEye.y;
            actor->mCameraEyePos.z = gabi::at<TnCameraView>(view)->mEye.z;
            actor->mCenterPos.x = gabi::at<TnCameraView>(view)->mCenter.x;
            actor->mCenterPos.y = gabi::at<TnCameraView>(view)->mCenter.y;
            actor->mCenterPos.z = gabi::at<TnCameraView>(view)->mCenter.z;
            actor->m1514 = static_cast<f32>(55.0f);
            actor->m14FA = static_cast<s16>(0);
            demo = 51;
        }
    }
    if (demo == 51) {
        cLib_addCalc2(&actor->m1514,
            gabi::fadds_ppc(REG0_F(13), 30.0f),
            0.2f,
            gabi::fadds_ppc(REG0_F(14), 0.4f));
        if (static_cast<s8>(actor->m1520) == 0) {
            cLib_addCalc2(&actor->mCenterPos.x,
                static_cast<f32>(actor->current.pos.x), 0.1f, 100.0f);
            const f32 targetY = gabi::fadds_ppc(
                gabi::fadds_ppc(static_cast<f32>(actor->current.pos.y), 230.0f),
                REG0_F(12));
            cLib_addCalc2(&actor->mCenterPos.y, targetY,
                0.1f, 100.0f);
            cLib_addCalc2(&actor->mCenterPos.z,
                static_cast<f32>(actor->current.pos.z), 0.1f, 100.0f);
        }
        const s16 elapsed = static_cast<s16>(static_cast<s16>(actor->m14FA) + 1);
        actor->m14FA = static_cast<s16>(elapsed);
        if (elapsed > 150) {
            tn_abi::cameraTrimSize(gabi::at<void>(camera), 0);
            tn_abi::cameraStart(gabi::at<void>(camera));
            tn_abi::dMeter_mtrShow();
            const u32 currentPlay = gabi::ea(dComIfGp_get());
            gabi::at<dComIfG_inf_c>(currentPlay)->play.mEvtCtrl.mEventFlag = static_cast<u16>(gabi::at<dComIfG_inf_c>(currentPlay)->play.mEvtCtrl.mEventFlag) | 8;
            fopAcM_delete(actor);
            setCamera = false;
        }
    }
    if (setCamera) {
        *cameraCenter = *&actor->mCenterPos;
        *cameraEye = *&actor->mCameraEyePos;
        tn_abi::cameraSet(gabi::at<void>(camera), cameraCenter.get(), cameraEye.get(),
            0, static_cast<f32>(actor->m1514));
    }
    if (static_cast<s16>(actor->m0430) != 0) {
        const s16 remaining = static_cast<s16>(static_cast<s16>(actor->m0430) - 1);
        actor->m0430 = static_cast<s16>(remaining);
        if (remaining >= static_cast<s16>(l_tnHIO.m020)) {
            smokeOffset->x = 0.0f;
            smokeOffset->y = 0.0f;
            actor->m0428.x = static_cast<s16>(0); actor->m0428.z = static_cast<s16>(0);
            MtxTrans(static_cast<f32>(actor->current.pos.x), gabi::fadds_ppc(static_cast<f32>(actor->current.pos.y), 7.5f), static_cast<f32>(actor->current.pos.z), 0);
            const u8 mode = static_cast<u8>(actor->m0432);
            if (mode == 0 || mode == 1) {
                if (mode == 1)
                    tn_abi::cMtx_YrotM(calc_mtx(), static_cast<s16>(actor->current.angle.y));
                smokeOffset->z = mode == 0
                    ? gabi::fadds_ppc(REG0_F(4), -200.0f)
                    : -55.0f;
                tn_abi::cMtx_YrotM(calc_mtx(), static_cast<s16>(actor->m042E));
                MtxPosition(smokeOffset.get(), &actor->m041C);
                actor->m0428.y = static_cast<s16>(actor->m042E);
                smoke_set_s(actor, gabi::fadds_ppc(
                    (mode == 0 ? REG0_F(15) : REG0_F(20)),
                    (mode == 0 ? 3.0f : 4.0f)));
                const int increment = mode == 0 ? REG0_S(7) + 2000 : 0x1FA0;
                actor->m042E = static_cast<s16>(static_cast<s16>(static_cast<s16>(actor->m042E) + increment));
            }
        } else actor->m041C.y = gabi::fadds_ppc(static_cast<f32>(actor->mDamageReaction.mSpawnY), 25000.0f);
        if (static_cast<s16>(actor->m0430) == 0) {
            tn_abi::smokeEnd(reinterpret_cast<void*>(&actor->mSmokeCb1));
            actor->m0433 = static_cast<u8>(0);
        }
    }
    decrementShort(&actor->m03EE);
    actor->mDamageReaction.mpEnemy = actor;
    actor->mDamageReaction.mEnemyType = static_cast<s16>(damagereaction::TYPE_DARKNUT);
}

// Keep the initial player snapshot: native r24 is not the refreshed damage player.
static void updateCollisionShapes(tn_class* actor, u32 initialPlayer) {
    gabi::Local<cXyz> offset;
    gabi::Local<cXyz> coCenter;
    gabi::Local<cXyz> headCenter;
    gabi::Local<cXyz> bodyCenter;
    gabi::Local<u8[16]> outgoingLinkage;
    MtxTrans(static_cast<f32>(actor->current.pos.x), static_cast<f32>(actor->current.pos.y), static_cast<f32>(actor->current.pos.z), 0);
    tn_abi::cMtx_YrotM(calc_mtx(), static_cast<s16>(actor->current.angle.y));
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = static_cast<s16>(actor->mDamageReaction.mAction) == ACTION_D_MAHI
        ? gabi::fadds_ppc(REG_F(10, 4), -20.0f)
        : 35.0f;
    MtxPosition(offset.get(), coCenter.get());
    tn_abi::cylinderSetCenter(reinterpret_cast<void*>(&actor->mCoCyl.mCyl), coCenter.get());
    u32 play = gabi::ea(dComIfGp_get());
    tn_abi::setCollision(gabi::at<void>(play + 0x26A4), reinterpret_cast<void*>(&actor->mCoCyl));
    play = gabi::ea(dComIfGp_get());
    tn_abi::setCollisionMass(gabi::at<void>(play + 0x4EF8), reinterpret_cast<void*>(&actor->mCoCyl), 3);
    *headCenter = *&actor->m1384;
    *bodyCenter = actor->current.pos;
    if (static_cast<s16>(actor->m03F0) != 0) {
        headCenter->y = gabi::fsubs_ppc(headCenter->y, 20000.0f);
        bodyCenter->y = gabi::fsubs_ppc(bodyCenter->y, 20000.0f);
    }
    if (static_cast<s16>(actor->mDamageReaction.mAction) == ACTION_DEFENCE) {
        bodyCenter->y = gabi::fsubs_ppc(bodyCenter->y, 100.0f);
        headCenter->y = gabi::fsubs_ppc(headCenter->y, 20000.0f);
    }
    tn_abi::cylinderSetCenter(reinterpret_cast<void*>(&actor->mTgCyl.mCyl), bodyCenter.get());
    u32 bodyShield = static_cast<u32>(actor->mTgCyl.mGObjTg.mSPrm);
    if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 1) != 0) bodyShield |= 1;
    else bodyShield &= ~1u;
    actor->mTgCyl.mGObjTg.mSPrm = static_cast<u32>(bodyShield);
    play = gabi::ea(dComIfGp_get());
    tn_abi::setCollision(gabi::at<void>(play + 0x26A4), reinterpret_cast<void*>(&actor->mTgCyl));
    tn_abi::sphereSetCenter(reinterpret_cast<void*>(&actor->mHeadSph.mSph), headCenter.get());
    if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 2) != 0) {
        if (static_cast<u8>(gabi::at<TnPlayer>(initialPlayer)->mCutType) == 0x1F) {
            actor->mHeadSph.mObjTg.mSPrm = static_cast<u32>(static_cast<u32>(actor->mHeadSph.mObjTg.mSPrm) & ~1u);
        } else {
            actor->mHeadSph.mObjTg.mSPrm = static_cast<u32>(static_cast<u32>(actor->mHeadSph.mObjTg.mSPrm) | 1u);
            actor->mHeadSph.mGObjTg.mHitMark = static_cast<u8>(12);
            actor->mHeadSph.mGObjTg.mSPrm = static_cast<u32>(static_cast<u32>(actor->mHeadSph.mGObjTg.mSPrm) | 1u);
        }
    } else {
        actor->mHeadSph.mGObjTg.mHitMark = static_cast<u8>(1);
        actor->mHeadSph.mGObjTg.mSPrm = static_cast<u32>(static_cast<u32>(actor->mHeadSph.mGObjTg.mSPrm) & ~1u);
    }
    play = gabi::ea(dComIfGp_get());
    tn_abi::setCollision(gabi::at<void>(play + 0x26A4), reinterpret_cast<void*>(&actor->mHeadSph));
    if (static_cast<s16>(actor->m041A) != 0) {
        const s16 remaining = static_cast<s16>(static_cast<s16>(actor->m041A) - 1);
        actor->m041A = static_cast<s16>(remaining);
        if (remaining == 0) {
            actor->m0428.y = static_cast<s16>(actor->current.angle.y);
            play = gabi::ea(dComIfGp_get());
            tn_abi::particleSet( gabi::at<void>(gabi::ea(static_cast<dPa_control_c*>(gabi::at<dComIfG_inf_c>(play)->play.mParticle))),
                0, 0x0E, &actor->m1384, &actor->m0428,
                nullptr, 0xFF, nullptr, -1, nullptr, nullptr, 0);
        }
    }
}

static void updateGroundTilt(tn_class* actor) {
    if ((static_cast<u32>(actor->mDamageReaction.mAcch.m_flags) & 0x20) == 0) return;
    s16 pitch = 0x7FFF, roll = 0x7FFF;
    if (static_cast<s16>(actor->mDamageReaction.m480) == 0) pitch = roll = 0;
    else {
        gabi::Local<tn_groundcheck> check;
        gabi::Local<u8[16]> outgoingLinkage;
        tn_abi::cBgS_GndChk_construct(check.get());
        for (int i = 0; i < 7; ++i) check->mPassFlags[i] = 0;
        check->mCheckVtable = 0x100403D0;
        check->mPolyPassChk = gabi::ea(&check->mPolyPassVtable);
        check->mGrpPassChk = gabi::ea(&check->mGroupPassVtable);
        check->mGroundVtable = 0x100403E0;
        check->mPolyPassVtable = 0x10040400;
        check->mGroupPassVtable = 0x100403F0;
        check->mGroupMask = 1;
        const f32 x = static_cast<f32>(actor->current.pos.x);
        const f32 z = static_cast<f32>(actor->current.pos.z);
        const f32 step = 100.0f;
        const f32 sampleOffset = 50.0f;
        const f32 invalidHeight = -1000000000.0f;
        check->mPosition.x = x;
        check->mPosition.y = gabi::fadds_ppc(static_cast<f32>(actor->current.pos.y),
            gabi::fsubs_ppc(sampleOffset, static_cast<f32>(actor->mDamageReaction.m44C.y)));
        check->mPosition.z = z;
        u32 play = gabi::ea(dComIfGp_get());
        const f32 groundY = tn_abi::GroundCross(gabi::at<void>(play + 0x12A0), check.get());
        if (groundY != invalidHeight) {
            const f32 sampleY = gabi::fadds_ppc(groundY, sampleOffset);
            const f32 forwardZ = gabi::fadds_ppc(z, step);
            check->mPosition.x = x; check->mPosition.y = sampleY; check->mPosition.z = forwardZ;
            play = gabi::ea(dComIfGp_get());
            const f32 forwardY = tn_abi::GroundCross(gabi::at<void>(play + 0x12A0), check.get());
            if (forwardY != invalidHeight) {
                pitch = static_cast<s16>(-tn_abi::atan2s(
                    gabi::fsubs_ppc(forwardY, groundY), gabi::fsubs_ppc(forwardZ, z)));
                if (static_cast<u32>(static_cast<s32>(pitch) + 0x2000) >= 0x4001) pitch = 0;
            }
            const f32 rightX = gabi::fadds_ppc(x, step);
            check->mPosition.x = rightX; check->mPosition.y = sampleY; check->mPosition.z = z;
            play = gabi::ea(dComIfGp_get());
            const f32 rightY = tn_abi::GroundCross(gabi::at<void>(play + 0x12A0), check.get());
            if (rightY != invalidHeight) {
                const s32 angle = tn_abi::atan2s(
                    gabi::fsubs_ppc(rightY, groundY), gabi::fsubs_ppc(rightX, x));
                roll = static_cast<s16>(angle);
                if (static_cast<u32>(angle + 0x2000) >= 0x4001) roll = 0;
            }
        }
        check->mGroundVtable = 0x100403E0;
        check->mPolyPassVtable = 0x10040400;
        check->mGroupPassVtable = 0x100403C0;
        tn_abi::cBgS_Chk_destroy(check.get(), 0);
    }
    if (pitch != 0x7FFF) cLib_addCalcAngleS2(reinterpret_cast<be<s16>*>(&actor->mDamageReaction.m48C), pitch, 1, 0x400);
    if (roll != 0x7FFF) cLib_addCalcAngleS2(&actor->mDamageReaction.m48C.z, roll, 1, 0x400);
}

// Mark joint matrices current, then perform the original guest matrix copy.
static void copyBodyJointToCalc(tn_class* actor, u32 matrixOffset) {
    const u32 morf = gabi::ea(actor->mpBodyMorf.get());
    const u32 model = gabi::ea(static_cast<J3DModel*>(gabi::at<mDoExt_McaMorf>(morf)->mpModel));
    const u32 matrixBlock = gabi::ea(static_cast<J3DMtxBuffer_hd*>(gabi::at<J3DModel>(model)->mpMtxBuffer));
    gabi::at<J3DMtxBuffer_hd>(matrixBlock)->mFlags = static_cast<u16>(gabi::at<J3DMtxBuffer_hd>(matrixBlock)->mFlags) | 0x10;
    const u32 matrices = static_cast<u32>(gabi::at<J3DMtxBuffer_hd>(matrixBlock)->mpAnmMtx);
    tn_abi::MTXCopy(gabi::at<void>(matrices + matrixOffset),
        calc_mtx());
}
static void updateHeldWeapon(tn_class* actor) {
    if (static_cast<u32>(actor->m0C34) == 0) return;
    gabi::Local<tn_linecheck> check;
    gabi::Local<cXyz> origin;
    gabi::Local<u8[16]> outgoingLinkage;
    u32 weapon = findActor(static_cast<u32>(actor->mBokoPcId));
    if (weapon != 0) {
        if ((static_cast<u32>(gabi::at<fopAc_ac_c>(weapon)->actor_status) & 0x2000) != 0) {
            copyBodyJointToCalc(actor, 0x450);
            tn_abi::cMtx_YrotM(calc_mtx(), REG_S(8, 1));
            tn_abi::cMtx_XrotM(calc_mtx(), REG_S(8, 2));
            tn_abi::cMtx_ZrotM(calc_mtx(),
                static_cast<s16>(REG_S(8, 3) + 0x8000));
            MtxTrans(REG_F(8, 9), REG_F(8, 10), gabi::fadds_ppc(REG_F(8, 11), 85.0f), 1);
            const u32 weaponModel = gabi::ea(static_cast<J3DModel*>(gabi::at<TnBoko>(weapon)->mpModel));
            if (weaponModel != 0) copyBaseMatrix(gabi::ea(calc_mtx()), weaponModel + 0xC8);
        } else tn_abi::fopAcM_setCarryNow(gabi::at<void>(weapon), 0);
    } else if (static_cast<s8>(actor->m140C) == 0) actor->m0C34 = static_cast<u32>(0);
    if (static_cast<u32>(actor->m0C38) == 0) return;
    weapon = findActor(static_cast<u32>(actor->mBokoPcId));
    if (weapon != 0) {
        tn_abi::cancelCarry(gabi::at<void>(weapon));
        if (static_cast<u32>(actor->m0C38) != 2) {
            const f32 spin = cM_rndFX(2000.0f);
            gabi::at<TnBoko>(weapon)->m2C2 = static_cast<s16>(gabi::ftoi(spin));
            const s32 baseYaw = static_cast<s16>(actor->shape_angle.y) + 0x8000;
            const f32 spread = cM_rndFX(8000.0f);
            const s32 yaw = baseYaw + static_cast<s16>(gabi::ftoi(spread));
            const f32 horizontal = gabi::fadds_ppc(
                cM_rndF(10.0f), 20.0f);
            const f32 vertical = gabi::fadds_ppc(
                cM_rndF(10.0f), 20.0f);
            gabi::at<fopAc_ac_c>(weapon)->current.angle.y = static_cast<s16>(yaw);
            gabi::at<fopAc_ac_c>(weapon)->speed.y = static_cast<f32>(vertical);
            gabi::at<fopAc_ac_c>(weapon)->speedF = static_cast<f32>(horizontal);
        }
        cBgS_LinChk_ct(check.get());
        for (int i = 0; i < 7; ++i) check->mPassFlags[i] = 0;
        check->mCheckVtable = 0x10040440; check->mLineVtable = 0x10040450;
        check->mPolyPassVtable = 0x10040470; check->mGroupPassVtable = 0x10040460;
        check->mGroupMask = 1;
        check->mPolyPassChk = gabi::ea(&check->mPolyPassVtable);
        check->mGrpPassChk = gabi::ea(&check->mGroupPassVtable);
        tn_abi::dBgS_LinChk_Set(check.get(), &actor->eyePos, &gabi::at<TnBoko>(weapon)->current.pos, actor);
        const u32 play = gabi::ea(dComIfGp_get());
        if (tn_abi::LineCross(gabi::at<void>(play + 0x12A0), check.get()) != 0) {
            copyBodyJointToCalc(actor, 0x1E0);
            const u32 model = gabi::ea(static_cast<J3DModel*>(gabi::at<TnBoko>(weapon)->mpModel));
            if (model != 0) copyBaseMatrix(gabi::ea(calc_mtx()), model + 0xC8);
            origin->x = origin->y = origin->z = 0.0f;
            MtxPosition(origin.get(), &gabi::at<TnBoko>(weapon)->current.pos);
        }
        const s8 room = static_cast<s8>(actor->current.roomNo);
        const u32 actorId = static_cast<u32>(actor->mBsPcId);
        const int reverb = tn_getReverb(room);
        tn_abi::monsterSound(0x488C, &actor->eyePos, actorId, 0, reverb);
        check->mPolyPassVtable = 0x10040470; check->mGroupPassVtable = 0x100403C0;
        check->mLineVtable = 0x100403B0;
        cBgS_LinChk_dt(check.get(), 0);
    }
    actor->m0C38 = static_cast<u32>(0); actor->m0C34 = static_cast<u32>(0);
}

static void finishNormalEffects(tn_class* actor) {
    const u32 a = gabi::ea(actor);
    moveDetachedParts(actor);
    actor->m02DD = static_cast<u8>(0);
    if (static_cast<s16>(actor->m14F4) != 0) {
        gabi::Local<cXyz> tipOffset;
        gabi::Local<cXyz> worldTip;
        gabi::Local<cXyz> relativeTip;
        gabi::Local<u8[16]> outgoingLinkage;
        tipOffset->x = tipOffset->y = 0.0f;
        tipOffset->z = gabi::fadds_ppc(REG0_F(4), 250.0f);
        copyBodyJointToCalc(actor, 0x450);
        MtxPosition(tipOffset.get(), worldTip.get());
        tn_abi::subtractPosition(worldTip.get(), relativeTip.get(), &actor->current.pos);
        *tipOffset = *relativeTip;
        for (int i = 0; i < 10; ++i) {
            const s16 yaw = static_cast<s16>(i * (REG0_S(2) - 0x320));
            tn_abi::cMtx_YrotS(calc_mtx(), yaw);
            cXyz* sample = &actor->m147C[i];
            MtxPosition(tipOffset.get(), sample);
            tn_VECAdd(sample, &actor->current.pos, sample);
        }
        // Six one-time color pairs. Preserve the unusual 2454/2456 and
        // 245C/245E channels rather than treating these as packed u16 colors.
        struct ColorInit { be<u32>* flag; be<u8>* first; be<u8>* second; int firstValue, secondValue; };
        const ColorInit colors[] = {
            {&l_spinColorInitialized.m[0],&l_spinColors.m[0].r,&l_spinColors.m[0].g,0x40,0x40},
            {&l_spinColorInitialized.m[1],&l_spinColors.m[1].r,&l_spinColors.m[1].g,0xDC,0xDC},
            {&l_spinColorInitialized.m[2],&l_spinColors.m[2].r,&l_spinColors.m[2].g,0x78,0x78},
            {&l_spinColorInitialized.m[3],&l_spinColors.m[3].r,&l_spinColors.m[3].b,0xC8,0x40},
            {&l_spinColorInitialized.m[4],&l_spinColors.m[4].b,nullptr,0xDC,0},
            {&l_spinColorInitialized.m[5],&l_spinColors.m[5].r,&l_spinColors.m[5].b,0xC8,0x78},
        };
        for (const ColorInit& color : colors) {
            if (static_cast<u32>(*color.flag) != 0) continue;
            *color.flag = 1;
            const f32 first = daTn_colorPower(color.firstValue);
            *color.first = static_cast<u8>(gabi::ftoi(gabi::fmuls_ppc(first, 255.0f)));
            if (color.second != 0) {
                const f32 second = daTn_colorPower(color.secondValue);
                *color.second = static_cast<u8>(gabi::ftoi(gabi::fmuls_ppc(second, 255.0f)));
            }
        }
        for (int i = 0; i < 3; ++i) {
            const u32 callback = a + 0x159C + 0x10*i;
            if (static_cast<s16>(actor->m14F6) == 0) {
                tn_abi::cutTurnEnd(gabi::at<void>(callback));
                continue;
            }
            if (static_cast<s16>(actor->m14F4) == 1) {
                const bool late = static_cast<s16>(actor->m14F6) >= 5;
                const u32 primary = late ? (i == 1 ? 0x101D2458 : 0x101D2454)
                                         : (i == 1 ? 0x101D244C : 0x101D2448);
                const u32 environment = late ? 0x101D245C : 0x101D2450;
                const s8 room = static_cast<s8>(actor->current.roomNo);
                const u16 effect = static_cast<u16>(spin_eff_name.m[i]);
                const u32 play = gabi::ea(dComIfGp_get());
                const u32 emitter = tn_abi::particleSet(
                    gabi::at<void>(gabi::ea(static_cast<dPa_control_c*>(gabi::at<dComIfG_inf_c>(play)->play.mParticle))), 0, effect,
                    &actor->current.pos, &actor->shape_angle, &actor->scale, 0xFF,
                    gabi::at<void>(callback), room, gabi::at<void>(primary),
                    gabi::at<void>(environment), 0);
                if (emitter != 0) gabi::at<TnEmitterColors>(emitter)->mGlobalParticleWidthScale = gabi::fadds_ppc(REG0_F(17), 1.5f);
                actor->m14F4 = static_cast<s16>(static_cast<s16>(static_cast<s16>(actor->m14F4) + 1));
            }
            gabi::at<TnCutTurnCallback>(callback)->mpPoints = &actor->m147C[0];
            gabi::at<TnCutTurnCallback>(callback)->mCount = 10;
        }
        if (static_cast<s16>(actor->m14F6) == 0) actor->m14F4 = static_cast<s16>(0);
        else decrementShort(&actor->m14F6);
    }
    const u32 environment = tn_abi::environmentLight();
    tn_abi::settingTevStruct(gabi::at<void>(environment), 0, &actor->current.pos, &actor->tevStr);
}

static void updateDamageAnimation(tn_class* actor, u32 damagePower) {
    actor->shape_angle.x = actor->current.angle.x;
    actor->shape_angle.y = actor->current.angle.y;
    actor->shape_angle.z = actor->current.angle.z;
    const u32 reaction = tn_abi::damage_reaction(reinterpret_cast<void*>(&actor->mDamageReaction));
    if (reaction != 0) {
        actor->m0430 = static_cast<s16>(1);
        actor->m1408 = static_cast<u32>(0xFFFFFFFFu);
        switch (reaction) {
        case 1:
            anm_init(actor, dRes_INDEX_TN_BCK_BDOWN_A1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            if (static_cast<u32>(actor->m0C34) != 0) {
                bool drop = static_cast<s8>(actor->health) <= 0 || damagePower >= 4;
                if (!drop) {
                    const f32 random = cM_rndF(1.0f);
                    // Native bge tests !CR-less; unordered skips the drop branch.
                    drop = random < 0.5f;
                }
                if (drop) actor->m0C38 = static_cast<u32>(1);
            }
            actor->mDamageReaction.mAction = ACTION_JYUNKAI;
            break;
        case 2:
            anm_init(actor, dRes_INDEX_TN_BCK_BDOWN_U1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            break;
        case 5:
            if (static_cast<s16>(actor->mDamageReaction.mAction) != ACTION_FIGHT_RUN && static_cast<s16>(actor->mDamageReaction.mAction) != ACTION_HUKKI) {
                wait_set(actor);
                actor->mDamageReaction.mAction = ACTION_FIGHT_RUN; actor->mDamageReaction.mMode = static_cast<s16>(0);
                actor->mCountDownTimers[1] = static_cast<s16>(30);
            }
            actor->m041A = static_cast<s16>(5); actor->m0428.x = static_cast<s16>(-0x4000);
            break;
        case 10:
            wait_set(actor);
            actor->mDamageReaction.mMode = static_cast<s16>(2); actor->mDamageReaction.mAction = ACTION_JYUNKAI;
            path_check(actor);
            break;
        case 20:
            actor->m041A = static_cast<s16>(1); actor->m0428.x = static_cast<s16>(-0x4000);
            actor->m0430 = static_cast<s16>(static_cast<s16>(static_cast<s16>(l_tnHIO.m020) + 16));
            actor->m0432 = static_cast<u8>(1);
            actor->mDamageReaction.m4C8[2] = static_cast<s16>(l_tnHIO.m080);
            break;
        case 21:
            actor->m041A = static_cast<s16>(1); actor->m0428.x = static_cast<s16>(0);
            actor->mDamageReaction.m4C8[2] = static_cast<s16>(l_tnHIO.m080);
            break;
        case 30:
            anm_init(actor, dRes_INDEX_TN_BCK_BDOWN_U1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            if (static_cast<u32>(actor->m0C34) != 0) actor->m0C38 = static_cast<u32>(1);
            actor->mDamageReaction.mAction = ACTION_JYUNKAI;
            break;
        default: break;
        }
    }
    const u32 bodyMorf = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf));
    copyBaseMatrix(gabi::ea(calc_mtx()), gabi::ea(static_cast<J3DModel*>(gabi::at<mDoExt_McaMorf>(bodyMorf)->mpModel)) + 0xC8);
    if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 4) != 0) {
        const u32 shieldMorf = gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpShieldMorf));
        copyBaseMatrix(gabi::ea(calc_mtx()), gabi::ea(static_cast<J3DModel*>(gabi::at<mDoExt_McaMorf>(shieldMorf)->mpModel)) + 0xC8);
        if ((static_cast<u8>(actor->mRemainingEquipmentPieces) & 4) != 0)
            tn_abi::morfCalc(gabi::at<void>(gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpShieldMorf))));
    }
    tn_abi::morfCalc(gabi::at<void>(gabi::ea(static_cast<mDoExt_McaMorf*>(actor->mpBodyMorf))));
    tn_abi::enemy_fire(reinterpret_cast<void*>(&actor->mEnemyFire));
}

// Native stack damage record starts at r1+0x30. Only fields explicitly
// initialized by Execute are initialized here; callees produce the others.
struct DamageInfo {
    be<u32> hitObject; be<u32> attackActor;
    be<u8> power; be<u8> hitFlag; be<u8> kind; u8 reserved0B[3];
    be<s16> angle; u8 reserved10[2]; be<u16> flags;
    be<u32> hitPosition; be<u32> soundHandle;
};
static_assert(sizeof(DamageInfo) == 0x1C);
struct DamageSetup { u32 player; int hitPart; bool defenceHit; };
static DamageSetup prepareDamage(tn_class* actor, DamageInfo* info) {
    const u32 a = gabi::ea(actor);
    const u32 brk = gabi::ea(static_cast<mDoExt_brkAnm*>(actor->mpBrkAnm));
    gabi::at<mDoExt_brkAnm>(brk)->mFrameCtrl.mFrame = static_cast<f32>(static_cast<u8>(actor->mArmorColorIndex));
    actor->mBtStartFrame = static_cast<f32>(100.0f);
    actor->mBtEndFrame = static_cast<f32>(100.0f);
    actor->mBtMaxDis = static_cast<f32>(10000.0f);
    actor->mBtAttackType = static_cast<u8>(0); actor->mBtNowFrame = static_cast<f32>(0.0f);
    const u32 play = gabi::ea(dComIfGp_get());
    DamageSetup state{gabi::ea(static_cast<fopAc_ac_c*>(gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer)), 0, false};
    info->hitPosition = 0; info->flags = 0; info->power = 0;
    tn_abi::collisionStatusMove(&reinterpret_cast<dCcD_Stts*>(&actor->mDamageReaction.mStts)->__vtbl_gstts);
    gabi::Local<cXyz> origin;
    gabi::Local<cXyz> hiddenCenter;
    gabi::Local<u8[16]> outgoingLinkage;
    if (static_cast<s8>(actor->m1444) != 0) {
        actor->m1444 = static_cast<s8>(static_cast<s8>(static_cast<s8>(actor->m1444) - 1));
        copyBodyJointToCalc(actor, 0x2D0);
        origin->x = origin->y = origin->z = 0.0f;
        MtxPosition(origin.get(), &actor->m13E4);
        tn_abi::sphereSetRadius(reinterpret_cast<void*>(&actor->mDefenceSph.mSph), 70.0f);
        tn_abi::sphereSetCenter(reinterpret_cast<void*>(&actor->mDefenceSph.mSph), &actor->m13E4);
    } else {
        *hiddenCenter = actor->current.pos;
        hiddenCenter->y = -20000.0f;
        tn_abi::sphereSetCenter(reinterpret_cast<void*>(&actor->mDefenceSph.mSph), hiddenCenter.get());
    }
    const u32 currentPlay = gabi::ea(dComIfGp_get());
    tn_abi::setCollision(gabi::at<void>(currentPlay + 0x26A4), reinterpret_cast<void*>(&actor->mDefenceSph));
    if (tn_abi::checkTargetHit(reinterpret_cast<void*>(&actor->mDefenceSph)) != 0) {
        const u32 hit = tn_abi::getTargetHitObject(reinterpret_cast<void*>(&actor->mDefenceSph));
        tn_abi::def_se_set(actor, gabi::at<void>(hit), 0x40);
        tn_abi::cMtx_YrotS(calc_mtx(), static_cast<s16>(actor->shape_angle.y));
        origin->x = origin->y = 0.0f;
        origin->z = -10.0f;
        MtxPosition(origin.get(), &actor->mDamageReaction.m42C);
        actor->mDamageReaction.m4D4 = static_cast<f32>(-10.0f);
        state.defenceHit = true;
        return state;
    }
    if (static_cast<s16>(actor->m03F0) != 0) return state;
    if (tn_abi::checkTargetHit(reinterpret_cast<void*>(&actor->mHeadSph)) == 0 &&
        tn_abi::checkTargetHit(reinterpret_cast<void*>(&actor->mTgCyl)) == 0) return state;
    actor->m03F0 = static_cast<s16>(static_cast<s16>(REG0_S(7) + 5));
    info->hitObject = 0;
    if (tn_abi::checkTargetHit(reinterpret_cast<void*>(&actor->mHeadSph)) != 0) {
        state.hitPart = 1;
        info->hitObject = tn_abi::getTargetHitObject(reinterpret_cast<void*>(&actor->mHeadSph));
        info->hitPosition = a + 0x10F0;
    } else if (tn_abi::checkTargetHit(reinterpret_cast<void*>(&actor->mTgCyl)) != 0) {
        state.hitPart = 2;
        info->hitObject = tn_abi::getTargetHitObject(reinterpret_cast<void*>(&actor->mTgCyl));
        info->hitPosition = a + 0xFC0;
    }
    // Native repeated collision query can leave a null hit; bypass knockback.
    if (static_cast<u32>(info->hitObject) == 0) state.defenceHit = true;
    return state;
}

static void deleteMantle(tn_class* actor) {
    const u32 id = static_cast<u32>(actor->mMantPcId);
    if (id == 0xFFFF) return;
    const u32 mantle = findActor(id);
    if (mantle != 0) fopAcM_delete(gabi::at<fopAc_ac_c>(mantle));
    actor->mMantPcId = static_cast<u32>(0xFFFF);
}
// ordinary damage classification and continue with movement and zero power.
static bool applyIceOrShatter(tn_class* actor, u32 hitObject, int hitPart) {
    const u32 a = gabi::ea(actor);
    const u32 mask = static_cast<u32>(gabi::at<dCcD_GObjInf>(hitObject)->mAtType);
    if ((mask & 0x180000) == 0) return false;
    if ((mask & 0x80000) != 0) {
        const u8 equipment = static_cast<u8>(actor->mRemainingEquipmentPieces);
        if (!((hitPart == 2 && (equipment & 1) == 0) ||
              (hitPart == 1 && (equipment & 2) == 0))) return false;
        actor->mEnemyIce.m00C = static_cast<u8>(1);
        actor->mDamageReaction.mAction = ACTION_JYUNKAI; actor->mDamageReaction.mMode = static_cast<s16>(-10);
        actor->health = static_cast<u8>(static_cast<u8>(static_cast<u8>(actor->health) + 4));
        actor->mEnemyIce.mFreezeDuration = static_cast<s16>(static_cast<s16>(REG0_S(4) + 60));
        const f32 blend = 5.0f, speed = 0.0f;
        anm_init(actor, dRes_INDEX_TN_BCK_AHAKAI1_e, blend, J3DFrameCtrl::EMode_NONE, speed, -1);
        tate_anm_init(actor, dRes_INDEX_TN_BCK_AHAKAI1_e, blend, J3DFrameCtrl::EMode_NONE, speed);
        yoroi_anm_init(actor, dRes_INDEX_TN_BCK_YHAKAI1_e, blend, J3DFrameCtrl::EMode_NONE, speed);
    } else {
        actor->mEnemyIce.mLightShrinkTimer = static_cast<u8>(1);
        for (int i = 0; i < 3; ++i) {
            const u32 part = a + 0x3F8 + 0x4C*i;
            if (i <= 1 && static_cast<s8>(gabi::at<tn_p>(part)->m08) >= 0)
                yoroi_break(actor, gabi::at<cXyz>(part + 0xC), static_cast<u8>(i));
            gabi::at<tn_p>(part)->m08 = -1;
        }
    }
    tn_abi::enemy_fire_remove(reinterpret_cast<void*>(&actor->mEnemyFire));
    actor->m0430 = static_cast<s16>(0);
    for (u32 offset : {0x159Cu, 0x15ACu, 0x15BCu}) tn_abi::cutTurnEnd(gabi::at<void>(a + offset));
    for (u32 offset : {0x550u, 0x570u, 0xD38u}) tn_abi::smokeEnd(gabi::at<void>(a + offset));
    if (static_cast<u32>(actor->m0C34) != 0) actor->m0C38 = static_cast<u32>(2);
    deleteMantle(actor);
    return true;
}

static void monsterSound(tn_class* actor, int sound) {
    const s8 room = static_cast<s8>(actor->current.roomNo);
    const u32 id = static_cast<u32>(actor->mBsPcId);
    const int reverb = tn_getReverb(room);
    tn_abi::monsterSound(sound, &actor->eyePos, id, 0, reverb);
}
static u16 absoluteHitYaw(tn_class* actor) {
    s16 difference = static_cast<s16>(static_cast<s16>(actor->mTargetAngle) - static_cast<s16>(actor->current.angle.y));
    if (difference < 0) difference = static_cast<s16>(-difference);
    return static_cast<u16>(difference);
}
static int applyOrdinaryDamage(tn_class* actor, DamageInfo* info, const DamageSetup& state) {
    const s8 oldHealth = static_cast<s8>(actor->health);
    tn_abi::at_power_check(info);
    if (info->kind == 10 || info->kind == 14) actor->health = static_cast<s8>(20);
    actor->stealItemBitNo = static_cast<s8>(actor->m1400);
    info->attackActor = tn_abi::cc_at_check(actor, info);
    if (info->kind == 10 || info->kind == 14) {
        actor->health = static_cast<s8>(oldHealth);
        if (info->kind == 14 && static_cast<s8>(actor->m1400) == 0) actor->m1400 = static_cast<s8>(1);
    }
    gabi::Local<cXyz> soundPosition;
    gabi::Local<u8[16]> outgoingLinkage;
    *soundPosition = actor->current.pos;
    actor->m03F2 = static_cast<s16>(25);
    tn_abi::dKy_Sound_set(soundPosition.get(), 100, static_cast<u32>(actor->mBsPcId), 5);
    if (info->kind == 12) actor->m03F0 = static_cast<s16>(10);
    actor->m141C = static_cast<u16>(static_cast<u16>(actor->m141C) | static_cast<u16>(info->flags));
    actor->m1434 = static_cast<s8>(info->kind == 10
        ? static_cast<s8>(REG0_S(3) + 8) : 0);
    if (info->kind == 1) {
        if (static_cast<u8>(gabi::at<TnPlayer>(state.player)->mCutType) == 5) return 2;
        if (info->hitFlag != 0) return absoluteHitYaw(actor) > 0x4000 ? 3 : 1;
        return state.hitPart == 1 ? 4 : 5;
    }
    const int reaction = info->kind == 2 || info->hitFlag != 0 ? 7 : 4;
    tn_abi::cMtx_YrotS(calc_mtx(), static_cast<s16>(info->angle));
    return reaction;
}
static void updateKnockback(tn_class* actor, DamageInfo* info, int reaction) {
    if ((reaction >= 1 && reaction <= 5) || reaction == 7) {
        const u32 flag = reaction == 2 || reaction == 3 ? 0x40 : reaction == 5 ? 0x20 : 0x10;
        actor->mDamageReaction.m424 = static_cast<u32>(static_cast<u32>(actor->mDamageReaction.m424) | flag);
        if (reaction == 2) {
            tn_abi::cMtx_YrotS(calc_mtx(),
                static_cast<s16>(static_cast<s16>(actor->current.angle.y) + 0x8000));
            actor->mDamageReaction.m428 = static_cast<f32>(26.0f);
        } else {
            actor->mDamageReaction.m428 = static_cast<f32>((reaction == 4 || reaction == 5 ? 23.0f : 26.0f));
            if (reaction != 7) tn_abi::cMtx_YrotS(calc_mtx(), static_cast<s16>(actor->mTargetAngle));
        }
        if (reaction == 5) {
            anm_init(actor, dRes_INDEX_TN_BCK_BDOWN_A1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
            actor->mDamageReaction.m48A = static_cast<s16>(10); actor->mDamageReaction.m70E = static_cast<u8>(7);
            actor->mDamageReaction.m474 = static_cast<f32>(5000.0f);
            if (static_cast<s16>(actor->m041A) == 0) {
                actor->m041A = static_cast<s16>(3); actor->m0428.x = static_cast<s16>(0);
            }
        }
    }
    if (static_cast<u32>(actor->mDamageReaction.m424) != 0) {
        gabi::Local<cXyz> offset;
        gabi::Local<u8[16]> outgoingLinkage;
        offset->x = offset->y = 0.0f;
        offset->z = -10.0f;
        MtxPosition(offset.get(), &actor->mDamageReaction.m42C);
        if (static_cast<f32>(actor->mDamageReaction.m428) < 25.0f) {
            actor->mDamageReaction.m4D4 = static_cast<f32>(-static_cast<f32>(l_tnHIO.m0D0));
        } else {
            const f32 random = cM_rndF(10.0f);
            actor->mDamageReaction.m428 = static_cast<f32>(gabi::fadds_ppc(random, 70.0f));
        }
    }
    if (reaction == 0) return;
    if (static_cast<s8>(actor->health) <= 0 && info->hitFlag != 0) {
        monsterSound(actor, 0x488E);
        if ((static_cast<u32>(actor->actor_status) & 0x04000000) != 0) {
            if (static_cast<s8>(s_check) == 0) {
                actor->m14F8 = static_cast<s8>(50); tn_abi::mDoAud_subBgmStop();
            } else s_check = static_cast<s8>(0);
        }
    } else monsterSound(actor, 0x488D);
}

static void impactParticle(tn_class* actor, int effect, csXyz* rotation, cXyz* scale) {
    const u32 play = gabi::ea(dComIfGp_get());
    tn_abi::particleSet( gabi::at<void>(gabi::ea(static_cast<dPa_control_c*>(gabi::at<dComIfG_inf_c>(play)->play.mParticle))),
        0, effect, &actor->mTgCyl.mGObjTg.mHitPos, rotation, scale, 0xFF,
        nullptr, -1, nullptr, nullptr, 0);
}
// True means an armored hit bypasses ordinary damage/knockback classification.
static bool applyArmoredHit(tn_class* actor, DamageInfo* info, const DamageSetup& state) {
    const u32 hit = info->hitObject;
    const u32 attackMask = static_cast<u32>(gabi::at<dCcD_GObjInf>(hit)->mAtType);
    const u8 equipment = static_cast<u8>(actor->mRemainingEquipmentPieces);
    if ((attackMask & 0x40200) != 0)
        actor->mEnemyFire.mFireDuration = static_cast<s16>(static_cast<s16>(REG0_S(2) + ((equipment & 1) ? 40 : 100)));
    if (!((state.hitPart == 2 && (equipment & 1)) || (state.hitPart == 1 && (equipment & 2)))) return false;
    const u32 status = gabi::ea(static_cast<dCcD_Stts*>(gabi::at<dCcD_GObjInf>(hit)->mStts));
    const u32 attacker = status == 0 ? 0 : gabi::ea(static_cast<fopAc_ac_c*>(gabi::at<dCcD_Stts>(status)->mp_actor));
    if (attacker == 0 || static_cast<s16>(gabi::at<fopAc_ac_c>(attacker)->mProcName) != 0xA8) {
        if ((attackMask & 0x188000) == 0) tn_abi::def_se_set(actor, gabi::at<void>(hit), 0x40);
        return true;
    }
    if (state.hitPart == 1 && static_cast<u8>(gabi::at<TnPlayer>(state.player)->mCutType) != 5) {
        tn_abi::def_se_set(actor, gabi::at<void>(hit), 0x40); return true;
    }
    gabi::Local<csXyz> rotation;
    gabi::Local<cXyz> scale;
    gabi::Local<u8[16]> outgoingLinkage;
    // Helmet branch changes animation/sound state before preparing particles.
    if (state.hitPart == 1) {
        tn_abi::at_power_check(info); tn_abi::mDoAud_onEnemyDamage();
        tn_abi::mDoAud_bgmHitSound(gabi::at<void>(static_cast<u32>(info->soundHandle)));
        actor->m02DD = static_cast<u8>(static_cast<u8>(actor->m02DD) | 2);
        actor->mDamageReaction.mAction = ACTION_D_SIT; actor->m03F0 = static_cast<s16>(20);
        actor->actor_status = static_cast<u32>(static_cast<u32>(actor->actor_status) & ~0x100u);
        actor->mDamageReaction.mMode = static_cast<s16>(10);
        l_pauseTimer = static_cast<u8>(REG0_S(7) + 6);
        const int reverb = tn_getReverb(static_cast<s8>(actor->current.roomNo));
        mDoAud_seStart(0x58E9, &actor->eyePos, 0, reverb);
    }
    const u16 yawMagnitude = state.hitPart == 2 ? absoluteHitYaw(actor) : 0;
    rotation->x = rotation->z = 0;
    scale->x = scale->y = scale->z = 2.0f;
    const u32 play = gabi::ea(dComIfGp_get());
    rotation->y = tn_abi::searchPlayerAngleY(actor, gabi::at<void>(gabi::ea(static_cast<fopAc_ac_c*>(gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer))));
    if (state.hitPart == 1) {
        impactParticle(actor, 0x0D, rotation.get(), scale.get());
        impactParticle(actor, 0x10, nullptr, nullptr); return true;
    }
    bool mantleBusy = false;
    if (static_cast<u32>(actor->mMantPcId) != 0xFFFF) {
        const u32 mantle = findActor(static_cast<u32>(actor->mMantPcId));
        mantleBusy = mantle != 0 && (static_cast<s8>(gabi::at<TnMantle>(mantle)->m2834) != 0 || static_cast<s16>(gabi::at<TnMantle>(mantle)->m2836) != 0);
    }
    if (!mantleBusy && yawMagnitude > 0x4000) {
        actor->mDamageReaction.mAction = ACTION_D_SIT;
        actor->m02DD = static_cast<u8>(static_cast<u8>(actor->m02DD) | 1);
        actor->actor_status = static_cast<u32>(static_cast<u32>(actor->actor_status) & ~0x100u);
        actor->mDamageReaction.mMode = static_cast<s16>(0); actor->m03F0 = static_cast<s16>(20);
        l_pauseTimer = static_cast<u8>(REG0_S(7) + 6);
        tn_abi::at_power_check(info); tn_abi::mDoAud_onEnemyDamage();
        tn_abi::mDoAud_bgmHitSound(gabi::at<void>(static_cast<u32>(info->soundHandle)));
        impactParticle(actor, 0x0D, rotation.get(), scale.get()); impactParticle(actor, 0x10, nullptr, nullptr);
        deleteMantle(actor);
        tn_abi::def_se_set(actor, gabi::at<void>(static_cast<u32>(info->hitObject)), 0x21);
        monsterSound(actor, 0x4892); return true;
    }
    tn_abi::def_se_set(actor, gabi::at<void>(static_cast<u32>(info->hitObject)), 0x40);
    impactParticle(actor, 0x0D, rotation.get(), scale.get());
    actor->m1402 = static_cast<s16>(15); tn_abi::at_power_check(info);
    const s8 health = static_cast<s8>(static_cast<u8>(actor->health) - info->power);
    actor->health = static_cast<s8>(health);
    if (health <= 0) {
        actor->mDamageReaction.mAction = ACTION_D_SIT; actor->mDamageReaction.mMode = static_cast<s16>(10);
        actor->mRemainingEquipmentPieces = static_cast<u8>(static_cast<u8>(actor->mRemainingEquipmentPieces) & ~1u);
        actor->m03F0 = static_cast<s16>(20);
        actor->actor_status = static_cast<u32>(static_cast<u32>(actor->actor_status) | 0x200000);
        l_pauseTimer = static_cast<u8>(REG0_S(7) + 6);
        actor->mParts[0].m08 = -1;
        yoroi_break(actor, &actor->mParts[0].m0C, 0);
        actor->health = static_cast<s8>((static_cast<u8>(actor->mEquipmentType) & 1) ? 20 : 10);
        deleteMantle(actor);
    }
    return true;
}
} // namespace tn_execute


/* 024B91E4 */
void anm_init(tn_class* i_this, int bckFileIdx, f32 morf, s32 loopMode, f32 speed, int soundFileIdx) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this->m03EC != 0)
        return;
    J3DAnmTransform* animation =
        static_cast<J3DAnmTransform*>(dComIfG_getObjectRes("Tn", bckFileIdx, TN_SAFESTRING_VTABLE));
    void* sound = nullptr;
    if (soundFileIdx >= 0) {
        sound = dComIfG_getObjectRes("Tn", soundFileIdx, TN_SAFESTRING_VTABLE);
    }
    mDoExt_McaMorf* model = i_this->mpBodyMorf;
    model->setAnm(animation, loopMode, morf, speed, 0.0f, -1.0f, sound);
}

/* 024B931C */
void tate_anm_init(tn_class* i_this, int bckFileIdx, f32 morf, s32 loopMode, f32 speed) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this->m03EC != 0)
        return;
    J3DAnmTransform* animation =
        static_cast<J3DAnmTransform*>(dComIfG_getObjectRes("Tn", bckFileIdx, TN_SAFESTRING_VTABLE));
    mDoExt_McaMorf* model = i_this->mpShieldMorf;
    model->setAnm(animation, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
}

/* 024B93E8 */
void yoroi_anm_init(tn_class* i_this, int bckFileIdx, f32 morf, s32 loopMode, f32 speed) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this->m03EC != 0)
        return;
    if ((static_cast<u8>(i_this->mRemainingEquipmentPieces) & EQUIPMENT_PIECE_BODY_ARMOR) == 0)
        return;
    J3DAnmTransform* animation =
        static_cast<J3DAnmTransform*>(dComIfG_getObjectRes("Tn", bckFileIdx, TN_SAFESTRING_VTABLE));
    mDoExt_McaMorf* model = i_this->mpArmorMorf;
    model->setAnm(animation, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
}

/* 024B94C0 */
void smoke_set_s(tn_class* i_this, f32 scale) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<tn_linecheck> check;
    tn_constructLineCheck(check.get());
    const f32 x = i_this->m041C.x;
    const f32 z = i_this->m041C.z;
    const f32 y = i_this->m041C.y;
    const f32 height = 100.0f;
    gabi::Local<cXyz> startpoint;
    gabi::Local<cXyz> endpoint;
    startpoint->x = x;
    startpoint->y = y + height;
    startpoint->z = z;
    endpoint->x = x;
    endpoint->y = y - height;
    endpoint->z = z;
    dBgS_LinChk_Set(check.get(), startpoint.get(), endpoint.get(), i_this);
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    u32 groundType = 0;
    if (cBgS_LineCross(&play->play.mBgS, check.get()) != 0) {
        const f32 hitX = check->mIntersection.x;
        const f32 hitY = check->mIntersection.y;
        const f32 hitZ = check->mIntersection.z;
        endpoint->x = hitX;
        i_this->m041C.y = hitY;
        endpoint->z = hitZ;
        endpoint->y = hitY;
        dComIfG_inf_c* refreshedPlay = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        groundType =
            tn_GetAttributeCode(&refreshedPlay->play.mBgS, gabi::at<u8>(gabi::ea(check.get()) + 0x14u));
    } else {
        const f32 smokeY = i_this->m041C.y;
        const f32 fall = 20000.0f;
        i_this->m041C.y = smokeY - fall;
    }

    const u8 oldCounter = i_this->m0433;
    if (oldCounter == 0 || groundType == 4) {
        i_this->m0433 = static_cast<u8>(static_cast<u32>(oldCounter) + 1u);
        if (groundType < 4 || groundType == 11) {
            dPa_smokeEcallBack_end(reinterpret_cast<dPa_smokeEcallBack*>(&i_this->mSmokeCb1));
            const s8 room = i_this->current.roomNo;
            const u8 alpha = static_cast<u8>(l_tnHIO.m01E);
            dPa_control_c* particle = dComIfGp_getParticle();
            JPABaseEmitter* emitter = dPa_control_set(
                particle, 2, 0x2022, &i_this->m041C, &i_this->m0428, nullptr, alpha,
                reinterpret_cast<dPa_levelEcallBack*>(&i_this->mSmokeCb1), room, nullptr, nullptr, nullptr);
            if (emitter != nullptr) {
                reinterpret_cast<DamageEmitter*>(emitter)->mRate = scale;
                reinterpret_cast<DamageEmitter*>(emitter)->mSpread = 1.0f;
                const f32 globalScale = l_smokeDynamicsScale + 1.0f;
                reinterpret_cast<DamageEmitter*>(emitter)->mGlobalDynamicsScale.z = globalScale;
                reinterpret_cast<DamageEmitter*>(emitter)->mGlobalDynamicsScale.y = globalScale;
                reinterpret_cast<DamageEmitter*>(emitter)->mGlobalDynamicsScale.x = globalScale;
                const f32 particleScale = l_smokeParticleScale + 3.0f;
                reinterpret_cast<DamageEmitter*>(emitter)->mGlobalParticleScale.x = particleScale;
                reinterpret_cast<DamageEmitter*>(emitter)->mGlobalParticleScale.y = particleScale;
                reinterpret_cast<DamageEmitter*>(emitter)->mGlobalParticleScale.z = particleScale;
            }
        } else if (groundType == 4) {
            JPABaseEmitter* emitter = dComIfGp_particle_set(0x24, &i_this->m041C, &i_this->m0428);
            if (emitter != nullptr) {
                const f32 waterScale = scale * 0.5f;
                emitter->mMaxFrame = 3;
                emitter->mRate = waterScale;
            }
        }
    }
    tn_prepareLineCheckDestruction(check.get());
    cBgS_LinChk_dt(check.get(), 0);
}

/* 024B97A0 */
BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (calcTiming != J3DNodeCBCalcTiming_In)
        return 1;
    J3DJoint* joint = J3DNode_toJoint(node);
    const u16 jointIndex = reinterpret_cast<TnJoint*>(joint)->mJntNo;
    if (jointIndex >= 33)
        JUT_ASSERT_fail("d_a_tn.cpp", 0x38A, "no2 >= 0 && no2 < 33");
    J3DModel* model = l_currentModel;
    tn_class* i_this = gabi::at<tn_class>(model->mUserArea);
    const s8 positionIndex = joint_check.m[jointIndex];
    if (i_this == 0)
        return 1;
    if (static_cast<u32>(jointIndex) - 11u < 7u &&
        (static_cast<u8>(i_this->mRemainingEquipmentPieces) & EQUIPMENT_PIECE_SHIELD) != 0 &&
        static_cast<s16>(i_this->mEnemyIce.mFreezeTimer) == 0) {
        mDoExt_McaMorf* shieldMorf = i_this->mpShieldMorf;
        J3DModel* shieldModel = shieldMorf->getModel();
        J3DMtxBuffer_hd* sourceBuffer = shieldModel->mpMtxBuffer;
        const u16 sourceFlags = static_cast<u16>(sourceBuffer->mFlags);
        Mtx34* sourceMatrices = gabi::at<Mtx34>(sourceBuffer->mpAnmMtx);
        sourceBuffer->mFlags = sourceFlags | 0x10u;
        J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
        const u16 flags = static_cast<u16>(buffer->mFlags);
        buffer->mFlags = flags | 0x10u;
        Mtx34* matrices = gabi::at<Mtx34>(buffer->mpAnmMtx);
        mtx_copy(matrices + jointIndex, sourceMatrices + jointIndex);
    }
    J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
    const u16 flags = static_cast<u16>(buffer->mFlags);
    Mtx34* matrices = gabi::at<Mtx34>(buffer->mpAnmMtx);
    buffer->mFlags = flags | 0x10u;
    PSMTXCopy(matrices + jointIndex, calc_mtx());
    if (positionIndex < 100) {
        if (static_cast<u32>(static_cast<s32>(positionIndex)) >= 21u)
            JUT_ASSERT_fail("d_a_tn.cpp", 0x3AA, "no >= 0 && no < 20+1");
        DamageAngles* angles = tn_jointAngles(i_this, positionIndex);
        cMtx_YrotM(calc_mtx(), angles->y);
        cMtx_XrotM(calc_mtx(), angles->x);
        cMtx_ZrotM(calc_mtx(), angles->z);
    }
    J3DMtxBuffer_hd* refreshedBuffer = model->mpMtxBuffer;
    Mtx34* stackMatrix = calc_mtx();
    const u16 refreshedFlags = static_cast<u16>(refreshedBuffer->mFlags);
    Mtx34* refreshedMatrices = gabi::at<Mtx34>(refreshedBuffer->mpAnmMtx);
    refreshedBuffer->mFlags = refreshedFlags | 0x10u;
    mtx_copy(refreshedMatrices + jointIndex, stackMatrix);
    PSMTXCopy(calc_mtx(), &l_currentMtx);

    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = 0.0f;
    if (positionIndex == 18) {
        offset->z = 200.0f;
        gabi::Local<cXyz> forwardPoint;
        MtxPosition(offset.get(), forwardPoint.get());
        offset->z = 0.0f;
        MtxPosition(offset.get(), &i_this->eyePos);
        const f32 eyeX = i_this->eyePos.x;
        const f32 eyeY = i_this->eyePos.y;
        const f32 eyeZ = i_this->eyePos.z;
        i_this->attention_info.position.y = eyeY;
        i_this->attention_info.position.z = eyeZ;
        i_this->attention_info.position.x = eyeX;
        i_this->attention_info.position.y = eyeY + l_tnHIO.m024;
        const f32 headingX = static_cast<f32>(forwardPoint->x) - eyeX;
        const f32 headingZ = static_cast<f32>(forwardPoint->z) - eyeZ;
        const s16 heading = cM_atan2s(headingX, headingZ);
        i_this->m0414 = heading;
        offset->x = 20.75f;
        offset->z = 10.0f;
        MtxPosition(offset.get(), &i_this->m1384);
        offset->y = -45.0f;
    } else {
        if (positionIndex >= 100)
            return 1;
        if (static_cast<u32>(static_cast<s32>(positionIndex)) >= 21u)
            JUT_ASSERT_fail("d_a_tn.cpp", 0x3E3, "no >= 0 && no < 20+1");
    }
    MtxPosition(offset.get(), tn_jointPosition(i_this, positionIndex));
    return 1;
}

/* 024B9C1C */
BOOL nodeCallBack_P(J3DNode* node, int calcTiming) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (calcTiming != J3DNodeCBCalcTiming_In)
        return 1;
    J3DJoint* joint = J3DNode_toJoint(node);
    const u16 jointIndex = reinterpret_cast<TnJoint*>(joint)->mJntNo;
    if (jointIndex >= 33)
        JUT_ASSERT_fail("d_a_tn.cpp", 0x3F3, "no2 >= 0 && no2 < 33");
    J3DModel* model = l_currentModel;
    tn_class* i_this = gabi::at<tn_class>(model->mUserArea);
    const s8 positionIndex = joint_check.m[jointIndex];
    if (i_this == 0)
        return 1;
    if (static_cast<u32>(jointIndex) - 11u < 7u &&
        (static_cast<u8>(i_this->mRemainingEquipmentPieces) & EQUIPMENT_PIECE_SHIELD) != 0 &&
        static_cast<s16>(i_this->mEnemyIce.mFreezeTimer) == 0) {
        mDoExt_McaMorf* shieldMorf = i_this->mpShieldMorf;
        J3DModel* shieldModel = shieldMorf->getModel();
        J3DMtxBuffer_hd* sourceBuffer = shieldModel->mpMtxBuffer;
        const u16 sourceFlags = static_cast<u16>(sourceBuffer->mFlags);
        Mtx34* sourceMatrices = gabi::at<Mtx34>(sourceBuffer->mpAnmMtx);
        sourceBuffer->mFlags = sourceFlags | 0x10u;
        J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
        const u16 flags = static_cast<u16>(buffer->mFlags);
        buffer->mFlags = flags | 0x10u;
        Mtx34* matrices = gabi::at<Mtx34>(buffer->mpAnmMtx);
        mtx_copy(matrices + jointIndex, sourceMatrices + jointIndex);
        J3DMtxBuffer_hd* refreshedBuffer = model->mpMtxBuffer;
        Mtx34* refreshedMatrices = gabi::at<Mtx34>(refreshedBuffer->mpAnmMtx);
        const u16 refreshedFlags = static_cast<u16>(refreshedBuffer->mFlags);
        refreshedBuffer->mFlags = refreshedFlags | 0x10u;
        PSMTXCopy(refreshedMatrices + jointIndex, &l_currentMtx);
    }
    if (positionIndex >= 100)
        return 1;
    J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
    const u16 flags = static_cast<u16>(buffer->mFlags);
    Mtx34* matrices = gabi::at<Mtx34>(buffer->mpAnmMtx);
    buffer->mFlags = flags | 0x10u;
    PSMTXCopy(matrices + jointIndex, calc_mtx());

    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = 0.0f;
    if (positionIndex == 17) {
        offset->x = 17.5f;
        offset->y = -8.75f;
        MtxPosition(offset.get(), &i_this->m13A8);
    } else if (positionIndex == 16) {
        MtxPosition(offset.get(), &i_this->m13B4);
    } else if (positionIndex == 20) {
        MtxPosition(offset.get(), &i_this->m0C3C);
        offset->x = -50.0f;
        MtxPosition(offset.get(), &i_this->m139C);
        offset->x = 165.0f;
        MtxPosition(offset.get(), &i_this->m1390);
    } else if (static_cast<u32>(static_cast<s32>(positionIndex)) >= 21u) {
        JUT_ASSERT_fail("d_a_tn.cpp", 0x420, "no >= 0 && no < 20+1");
    }
    MtxPosition(offset.get(), tn_jointPosition(i_this, positionIndex));
    return 1;
}

/* 024B9ED0 */
BOOL nodeCallBack_mimi(J3DNode* node, int calcTiming) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (calcTiming != J3DNodeCBCalcTiming_In)
        return 1;
    J3DJoint* joint = J3DNode_toJoint(node);
    J3DModel* model = l_currentModel;
    tn_class* i_this = gabi::at<tn_class>(model->mUserArea);
    const u16 jointIndex = reinterpret_cast<TnJoint*>(joint)->mJntNo;
    if (i_this == 0 || (static_cast<u8>(i_this->mRemainingEquipmentPieces) & EQUIPMENT_PIECE_HELMET) == 0)
        return 1;

    J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
    const u16 flags = static_cast<u16>(buffer->mFlags);
    Mtx34* matrices = gabi::at<Mtx34>(buffer->mpAnmMtx);
    buffer->mFlags = flags | 0x10u;
    Mtx34* stackMatrix = calc_mtx();
    PSMTXCopy(matrices + jointIndex, stackMatrix);
    mDoMtx_YrotM(calc_mtx(), -32768);

    J3DMtxBuffer_hd* refreshedBuffer = model->mpMtxBuffer;
    const u16 refreshedFlags = static_cast<u16>(refreshedBuffer->mFlags);
    Mtx34* refreshedStack = calc_mtx();
    Mtx34* refreshedMatrices = gabi::at<Mtx34>(refreshedBuffer->mpAnmMtx);
    refreshedBuffer->mFlags = refreshedFlags | 0x10u;
    mtx_copy(refreshedMatrices + jointIndex, refreshedStack);
    PSMTXCopy(calc_mtx(), &l_currentMtx);
    return 1;
}

/* 024B9FF8 */
BOOL nodeCallBack_kata(J3DNode* node, int calcTiming) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (calcTiming != J3DNodeCBCalcTiming_In)
        return 1;
    J3DJoint* joint = J3DNode_toJoint(node);
    J3DModel* model = l_currentModel;
    tn_class* i_this = gabi::at<tn_class>(model->mUserArea);
    mDoExt_McaMorf* bodyMorf = i_this->mpBodyMorf;
    J3DModel* bodyModel = bodyMorf->getModel();
    J3DMtxBuffer_hd* bodyBuffer = bodyModel->mpMtxBuffer;
    const u16 bodyFlags = static_cast<u16>(bodyBuffer->mFlags);
    Mtx34* bodyMatrices = gabi::at<Mtx34>(bodyBuffer->mpAnmMtx);
    const u16 jointIndex = reinterpret_cast<TnJoint*>(joint)->mJntNo;
    bodyBuffer->mFlags = bodyFlags | 0x10u;

    J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
    const u16 flags = static_cast<u16>(buffer->mFlags);
    Mtx34* matrices = gabi::at<Mtx34>(buffer->mpAnmMtx);
    buffer->mFlags = flags | 0x10u;
    mtx_copy(matrices + jointIndex, bodyMatrices + jointIndex);

    J3DMtxBuffer_hd* refreshedBuffer = model->mpMtxBuffer;
    Mtx34* refreshedMatrices = gabi::at<Mtx34>(refreshedBuffer->mpAnmMtx);
    const u16 refreshedFlags = static_cast<u16>(refreshedBuffer->mFlags);
    refreshedBuffer->mFlags = refreshedFlags | 0x10u;
    PSMTXCopy(refreshedMatrices + jointIndex, &l_currentMtx);
    return 1;
}

/* 024BA0F0 */
void part_draw(tn_class* i_this, int frozen) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    for (int index = 0; index < 3; ++index) {
        tn_p* part = &i_this->mParts[index];
        if (part->m08 < 0)
            continue;
        if (index == 0) {
            mDoExt_McaMorf* armor = i_this->mpArmorMorf;
            if (frozen != 0) {
                tn_iceEntryDL(armor, -1, nullptr);
                continue;
            }
            J3DModel* model = armor->getModel();
            mDoExt_brkAnm* animation = i_this->mParts[0].mpPartBrkAnm;
            J3DModelData* data = J3DModel_getModelData(model);
            const f32 frame = animation->mFrameCtrl.mFrame;
            mDoExt_brkAnm_entry(animation, data, frame);
            void* light = tn_envLight();
            tn_setLight(light, model, &i_this->tevStr);
            armor = i_this->mpArmorMorf;
            armor->entryDL();
            continue;
        }
        if (index == 2 && (static_cast<u8>(i_this->mRemainingEquipmentPieces) & EQUIPMENT_PIECE_SHIELD) == 0)
            continue;
        J3DModel* model = part->mpPartModel;
        if (frozen != 0) {
            tn_iceUpdateDL(model, -1, nullptr);
            continue;
        }
        mDoExt_brkAnm* animation = part->mpPartBrkAnm;
        J3DModelData* data = J3DModel_getModelData(model);
        const f32 frame = animation->mFrameCtrl.mFrame;
        mDoExt_brkAnm_entry(animation, data, frame);
        void* light = tn_envLight();
        tn_setLight(light, model, &i_this->tevStr);
        tn_modelUpdateDL(model, 0);
    }
}

// HD: draws the body and detached parts without the GameCube shadow submission.
/* 024BA238 */
BOOL daTn_Draw(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const s8 drawPaused = i_this->m1520;
    mDoExt_McaMorf* body = i_this->mpBodyMorf;
    J3DModel* model = body->getModel();
    if (drawPaused != 0 || i_this->m02C1 != 0)
        return 1;
    if (i_this->mEnemyIce.mFreezeTimer > 20) {
        tn_iceEntryDL(body, -1, nullptr);
        part_draw(i_this, 1);
        return 1;
    }

    const u8 equipmentType = i_this->mEquipmentType;
    int figure = DSNAP_TYPE_TN_MIGHTY;
    if (equipmentType < EQUIPMENT_SHIELD_AND_CAPE) {
        figure = (static_cast<u8>(i_this->mRemainingEquipmentPieces) & EQUIPMENT_PIECE_SHIELD) != 0
                     ? DSNAP_TYPE_TN_SHIELD
                     : DSNAP_TYPE_TN_NO_SHIELD;
    }
    tn_RegistFig(figure, i_this, 1.0f, 1.0f, 1.0f);
    void* light = tn_envLight();
    tn_setLight(light, model, &i_this->tevStr);
    mDoExt_brkAnm* animation = i_this->mpBrkAnm;
    J3DModelData* modelData = J3DModel_getModelData(model);
    const f32 frame = animation->mFrameCtrl.mFrame;
    mDoExt_brkAnm_entry(animation, modelData, frame);

    // HD: resolves the body material by name in its model-data material array.
    gabi::Local<SafeString> materialName;
    materialName->__vtbl = TN_SAFESTRING_VTABLE;
    materialName->mStringTop = 0x100405A0;
    J3DModelData* data = J3DModel_getModelData(model);
    TnResource* resource = gabi::at<TnResource>(data->mpResHeader);
    tnSafeString_noop(materialName.get());
    const u32 nameOffset = resource->mMaterialNameOffset;
    const u32 nameTable = nameOffset == 0 ? 0u : gabi::ea(&resource->mMaterialNameOffset) + nameOffset;
    const u32 name = materialName->mStringTop;
    const s32 index = tn_materialIndex(gabi::at<u8>(nameTable), gabi::at<const char>(name));
    u32 material = 0;
    if (index >= 0) {
        const u32 count = data->mMaterialNum;
        material = data->mMaterials;
        if (static_cast<u32>(index) < count)
            material += static_cast<u32>(index) * 0x39Cu;
    }
    const s16 debugHide = l_hideShape;
    J3DShape* shape = gabi::at<J3DMaterial>(material)->mShape;
    bool hidden = debugHide != 0;
    if (!hidden)
        hidden = i_this->m1400 != 0;
    shape->mVisible = hidden ? 0 : 1;
    body = i_this->mpBodyMorf;
    body->entryDL();
    part_draw(i_this, 0);
    return 1;
}

/* 024BA42C */
void way_pos_check(tn_class* i_this, cXyz* result) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<tn_linecheck> line;
    tn_constructLineCheck(line.get());
    gabi::Local<tn_groundcheck> ground;
    tn_constructGroundCheck(ground.get());
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    const f32 height = 50.0f;
    offset->y = height;
    gabi::Local<cXyz> startpoint;
    startpoint->x = i_this->current.pos.x;
    startpoint->y = static_cast<f32>(i_this->current.pos.y) + height;
    startpoint->z = i_this->current.pos.z;
    const f32 radiusCenter = 300.0f;
    const f32 radiusSpread = 200.0f;
    const f32 angleSpread = 6.2831854820251465f;
    gabi::Local<cXyz> rotated; // frame slots reused by every attempt
    gabi::Local<cXyz> endpoint;
    for (int attempt = 0; attempt < 100; ++attempt) {
        offset->z = cM_rndF(radiusSpread) + radiusCenter;
        const f32 randomHeading = cM_rndF(angleSpread);
        tn_MtxRotY(0, randomHeading);
        MtxPosition(offset.get(), rotated.get());
        const f32 x = static_cast<f32>(i_this->current.pos.x) + rotated->x;
        const f32 y = static_cast<f32>(i_this->current.pos.y) + rotated->y;
        const f32 z = static_cast<f32>(i_this->current.pos.z) + rotated->z;
        endpoint->x = x;
        endpoint->y = y;
        result->y = y;
        result->x = x;
        endpoint->z = z;
        result->z = z;
        dBgS_LinChk_Set(line.get(), startpoint.get(), endpoint.get(), i_this);
        dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        if (cBgS_LineCross(&play->play.mBgS, line.get()) == 0) {
            ground->mPosition.y = endpoint->y;
            ground->mPosition.z = endpoint->z;
            ground->mPosition.x = endpoint->x;
            dComIfG_inf_c* refreshedPlay = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            const f32 groundY = cBgS_GroundCross(&refreshedPlay->play.mBgS, ground.get());
            const f32 actorGroundY = i_this->mDamageReaction.mAcch.m_ground_h;
            if (actorGroundY - groundY < radiusSpread)
                break;
        }
    }
    tn_destroyGroundCheck(ground.get());
    tn_prepareLineCheckDestruction(line.get());
    cBgS_LinChk_dt(line.get(), 0);
}

/* 024BA6D8 */
u32 ground_4_check(tn_class* i_this, int count, s16 heading, f32 radius) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<tn_groundcheck> check;
    tn_constructGroundCheck(check.get());
    cMtx_YrotS(calc_mtx(), heading);
    u32 result = 0;
    gabi::Local<cXyz> offset;
    offset->y = 100.0f;
    gabi::Local<cXyz> position;
    if (count > 0) {
        const f32 noGround = -1000000000.0f;
        const f32 maximumDrop = 200.0f;
        const f32 fallbackHeight = 1000000000.0f;
        for (int i = 0; i < count; ++i) {
            const f32 x = xad.m[i];
            const f32 z = zad.m[i];
            offset->x = x * radius;
            offset->z = z * radius;
            MtxPosition(offset.get(), position.get());
            tn_VECAdd(position.get(), &i_this->current.pos, position.get());
            check->mPosition.y = position->y;
            check->mPosition.z = position->z;
            check->mPosition.x = position->x;
            dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            const f32 groundY = cBgS_GroundCross(&play->play.mBgS, check.get());
            const f32 actorGroundY = i_this->mDamageReaction.mAcch.m_ground_h;
            const f32 effectiveGround = groundY == noGround ? fallbackHeight : groundY;
            position->y = effectiveGround;
            if (actorGroundY - effectiveGround > maximumDrop)
                result |= check_bit.m[i];
        }
    }
    tn_destroyGroundCheck(check.get());
    return result;
}

/* 024BA8F8 */
BOOL daTn_other_bg_check(tn_class* i_this, fopAc_ac_c* target) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<tn_linecheck> check;
    tn_constructLineCheck(check.get());
    BOOL blocked = 1;
    if (target != nullptr) {
        gabi::Local<cXyz> endpoint;
        endpoint->x = target->current.pos.x;
        endpoint->y = static_cast<f32>(target->current.pos.y) + 50.0f;
        endpoint->z = target->current.pos.z;
        gabi::Local<cXyz> startpoint;
        startpoint->x = i_this->current.pos.x;
        startpoint->y = i_this->current.pos.y;
        startpoint->z = i_this->current.pos.z;
        startpoint->y = i_this->eyePos.y;
        dBgS_LinChk_Set(check.get(), startpoint.get(), endpoint.get(), i_this);
        dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        blocked = cBgS_LineCross(&play->play.mBgS, check.get()) != 0;
    }
    tn_prepareLineCheckDestruction(check.get());
    cBgS_LinChk_dt(check.get(), 0);
    return blocked;
}

/* 024BAA68 */
s16 get_view_H(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const bool helmet = (static_cast<u8>(i_this->mRemainingEquipmentPieces) & EQUIPMENT_PIECE_HELMET) != 0;
    return static_cast<s16>(helmet ? l_tnHIO.m036 : l_tnHIO.m034);
}

/* 024BAA8C */
void* s_w_sub(fopAc_ac_c* i_this, void* context) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (tn_IsActor(i_this) == 0)
        return nullptr;
    if (i_this == nullptr)
        return nullptr;
    if (i_this->mProcName != tn_proc::fpcNm_BOKO_e)
        return nullptr;
    if ((static_cast<u32>(i_this->actor_status) & fopAcStts_CARRY_e) != 0)
        return nullptr;
    tn_appendTarget(i_this);
    return nullptr;
}

// HD: inlines the weapon visibility test and the process-ID lookup.
/* 024BAFAC */
BOOL search_wepon(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this->m03FC != 0)
        return 0;
    target_info_count = 0;
    for (u32 i = 0; i < 10; ++i)
        target_info.m[i] = nullptr;
    tn_Search(gabi::guest_fn((const void*)s_w_sub), i_this);
    s32 count = target_info_count;
    const f32 radiusStep = 50.0f;
    f32 radius = radiusStep;
    if (count <= 0) {
        i_this->mBokoPcId = 0xFFFFFFFF;
        return 0;
    }
    const f32 maxRadius = 1500.0f;
    u32 index = 0;
    gabi::Local<cXyz> delta; // frame slots reused by every iteration
    gabi::Local<cXyz> relative;
    while (static_cast<s32>(index) < count) {
        fopAc_ac_c* candidate = target_info.m[index];
        delta->x = candidate->current.pos.x - i_this->eyePos.x;
        delta->y = (candidate->current.pos.y + radiusStep) - i_this->eyePos.y;
        delta->z = candidate->current.pos.z - i_this->eyePos.z;
        const f32 dx = delta->x;
        const f32 dz = delta->z;
        const f32 squaredDistance = gabi::fmadds(dx, dx, dz * dz);
        const f32 distance = tn_sqrtf(squaredDistance);
        bool visible = false;
        if (distance < radius && daTn_other_bg_check(i_this, candidate) == 0) {
            const f32 height = (candidate->current.pos.y + radiusStep) - i_this->eyePos.y;
            if (!(std::fabs(height) > l_tnHIO.m038)) {
                const s16 heading = cM_atan2s(dx, dz);
                const s16 actorHeading = i_this->m0414;
                const s16 difference = static_cast<s16>(static_cast<s32>(actorHeading) - heading);
                const u16 magnitude = difference < 0 ? static_cast<u16>(-static_cast<s32>(difference))
                                                     : static_cast<u16>(difference);
                if (magnitude < 0x1800)
                    visible = true;
                else {
                    const s16 angle = i_this->current.angle.y;
                    cMtx_YrotS(calc_mtx(), static_cast<s16>(-static_cast<s32>(angle)));
                    MtxPosition(delta.get(), relative.get());
                    if (std::fabs(static_cast<f32>(relative->x)) < l_tnHIO.m03C &&
                        std::fabs(static_cast<f32>(relative->y)) < l_tnHIO.m040) {
                        const f32 z = relative->z;
                        visible = z > l_tnHIO.m048 && z < l_tnHIO.m044;
                    }
                }
            }
        }
        if (visible) {
            if (candidate == 0)
                break;
            const u32 id = candidate->mBsPcId;
            i_this->mBokoPcId = id;
            if (id == 0xFFFFFFFF)
                return 0;
            gabi::Local<be<u32>> searchId;
            *searchId = id;
            return tn_SearchByID(0x025E1234u, searchId.get()) != 0;
        }
        count = target_info_count;
        ++index;
        if (static_cast<s32>(index) > count)
            break;
        if (static_cast<s32>(index) == count) {
            radius += radiusStep;
            index = 0;
            if (radius > maxRadius)
                break;
        }
    }
    i_this->mBokoPcId = 0xFFFFFFFF;
    return 0;
}

/* 024BAB08 */
void* s_b_sub(fopAc_ac_c* i_this, void* context) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (tn_IsActor(i_this) == 0)
        return nullptr;
    if (i_this == nullptr)
        return nullptr;
    if (i_this->mProcName != tn_proc::fpcNm_BOMB_e)
        return nullptr;
    if (tn_bombState(i_this, 0) != 0)
        return nullptr;
    tn_appendTarget(i_this);
    return nullptr;
}

/* 024BAB8C */
fopAc_ac_c* search_bomb(tn_class* i_this, int requireView) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((i_this->m141C & 0x200u) == 0)
        return nullptr;
    target_info_count = 0;
    for (u32 i = 0; i < 10; ++i)
        target_info.m[i] = nullptr;
    tn_Search(gabi::guest_fn((const void*)s_b_sub), i_this);
    s32 count = target_info_count;
    const f32 radiusStep = 50.0f;
    f32 radius = radiusStep;
    if (count <= 0)
        return nullptr;
    const f32 maxRadius = 1500.0f;
    const f32 rangeMargin = 30.0f;
    u32 index = 0;
    gabi::Local<cXyz> delta; // frame slots reused by every iteration
    gabi::Local<cXyz> relative;
    while (static_cast<s32>(index) < count) {
        fopAc_ac_c* candidate = target_info.m[index];
        delta->x = candidate->current.pos.x - i_this->current.pos.x;
        delta->y = (candidate->current.pos.y + radiusStep) - i_this->eyePos.y;
        delta->z = candidate->current.pos.z - i_this->current.pos.z;
        const f32 dx = delta->x;
        const f32 dz = delta->z;
        const f32 distance = tn_sqrtf(gabi::fmadds(dx, dx, dz * dz));
        if (distance < radius) {
            const f32 limit = i_this->mPlayerDistance + rangeMargin;
            if (!(distance > limit)) {
                const BOOL blocked = daTn_other_bg_check(i_this, candidate);
                // HD: mode zero performs the line check but returns this bomb
                // without using its result. View mode additionally filters it.
                if (requireView == 0)
                    return candidate;
                if (blocked == 0) {
                    const f32 height = (candidate->current.pos.y + radiusStep) - i_this->eyePos.y;
                    if (!(std::fabs(height) > l_tnHIO.m038)) {
                        const s16 heading = cM_atan2s(dx, dz);
                        const s16 actorHeading = i_this->m0414;
                        const s16 difference = static_cast<s16>(static_cast<s32>(actorHeading) - heading);
                        const u16 magnitude = difference < 0 ? static_cast<u16>(-static_cast<s32>(difference))
                                                             : static_cast<u16>(difference);
                        const s16 angleLimit = get_view_H(i_this);
                        if (static_cast<s32>(magnitude) < angleLimit)
                            return candidate;
                        const s16 angle = i_this->current.angle.y;
                        cMtx_YrotS(calc_mtx(), static_cast<s16>(-static_cast<s32>(angle)));
                        MtxPosition(delta.get(), relative.get());
                        if (std::fabs(static_cast<f32>(relative->x)) < l_tnHIO.m03C &&
                            std::fabs(static_cast<f32>(relative->y)) < l_tnHIO.m040) {
                            const f32 z = relative->z;
                            if (z > l_tnHIO.m048 && z < l_tnHIO.m044)
                                return candidate;
                        }
                    }
                }
            }
        }
        count = target_info_count;
        ++index;
        if (static_cast<s32>(index) > count)
            break;
        if (static_cast<s32>(index) == count) {
            radius += radiusStep;
            index = 0;
            if (radius > maxRadius)
                break;
        }
    }
    return nullptr;
}

/* 024BB27C */
BOOL daTn_player_bg_check(tn_class* i_this, cXyz* target) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfGp_get();
    if (search_sp != 0)
        return 0;
    gabi::Local<tn_linecheck> check;
    tn_constructLineCheck(check.get());
    gabi::Local<cXyz> endpoint;
    endpoint->x = target->x;
    endpoint->y = static_cast<f32>(target->y) + 75.0f;
    endpoint->z = target->z;
    gabi::Local<cXyz> startpoint;
    startpoint->x = i_this->current.pos.x;
    startpoint->y = i_this->current.pos.y;
    startpoint->z = i_this->current.pos.z;
    startpoint->y = i_this->eyePos.y;
    dBgS_LinChk_Set(check.get(), startpoint.get(), endpoint.get(), i_this);
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    const BOOL blocked = cBgS_LineCross(&play->play.mBgS, check.get()) != 0;
    tn_prepareLineCheckDestruction(check.get());
    if (blocked != 0)
        i_this->mDamageReaction.m713 = 0;
    cBgS_LinChk_dt(check.get(), 0);
    return blocked;
}

/* 024BB404 */
BOOL daTn_player_view_check(tn_class* i_this, cXyz* target, s16 heading, s16 angleLimit) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    const s8 forceSearch = search_sp;
    fopAc_ac_c* player = play->play.mPlayerInfo[0].mpPlayer;
    if (forceSearch != 0)
        return 1;
    if (daTn_player_bg_check(i_this, target) != 0)
        return 0;

    const f32 playerY = player->current.pos.y;
    const f32 eyeY = i_this->eyePos.y;
    const f32 heightDifference = (playerY + 50.0f) - eyeY;
    if (std::fabs(heightDifference) > l_tnHIO.m038)
        return 0;

    const s16 actorHeading = i_this->m0414;
    const s16 difference = static_cast<s16>(static_cast<s32>(actorHeading) - heading);
    const u16 magnitude =
        difference < 0 ? static_cast<u16>(-static_cast<s32>(difference)) : static_cast<u16>(difference);
    if (static_cast<s32>(magnitude) < angleLimit) {
        i_this->mDamageReaction.m713 = 1;
        return 1;
    }

    const s16 angle = i_this->current.angle.y;
    Mtx34* matrix = calc_mtx();
    cMtx_YrotS(matrix, static_cast<s16>(-static_cast<s32>(angle)));
    gabi::Local<cXyz> delta;
    delta->x = static_cast<f32>(target->x) - i_this->current.pos.x;
    delta->y = static_cast<f32>(target->y) - i_this->current.pos.y;
    delta->z = static_cast<f32>(target->z) - i_this->current.pos.z;
    gabi::Local<cXyz> relative;
    MtxPosition(delta.get(), relative.get());
    if (std::fabs(static_cast<f32>(relative->x)) < l_tnHIO.m03C &&
        std::fabs(static_cast<f32>(relative->y)) < l_tnHIO.m040) {
        const f32 z = relative->z;
        if (z > l_tnHIO.m048 && z < l_tnHIO.m044) {
            i_this->mDamageReaction.m713 = 2;
            return 1;
        }
    }
    i_this->mDamageReaction.m713 = 0;
    return 0;
}

/* 024BB5C4 */
BOOL daTn_player_way_check(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    fopAc_ac_c* player = play->play.mPlayerInfo[0].mpPlayer;
    const s16 facing = i_this->current.angle.y;
    const s16 playerFacing = player->shape_angle.y;
    const s16 difference = static_cast<s16>(static_cast<s32>(facing) - playerFacing);
    const u16 magnitude =
        difference < 0 ? static_cast<u16>(-static_cast<s32>(difference)) : static_cast<u16>(difference);
    return magnitude >= 0x4000;
}

/* 024BB620 */
void wait_set(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    anm_init(i_this, dRes_INDEX_TN_BCK_AKYORO1_e, 15.0f, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
    yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YKYORO1_e, 15.0f, J3DFrameCtrl::EMode_LOOP, 1.0f);
    const f32 random = cM_rndF(30.0f);
    i_this->mCountDownTimers[1] = static_cast<s16>(gabi::ftoi(random + 50.0f));
}

/* 024BB6E0 */
void fight_run_set(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((static_cast<u8>(i_this->mRemainingEquipmentPieces) & EQUIPMENT_PIECE_BODY_ARMOR) != 0) {
        anm_init(i_this, dRes_INDEX_TN_BCK_AWALK1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.5f,
                 dRes_INDEX_TN_BAS_AWALK1_e);
        yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YWALK1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.5f);
    } else {
        if (i_this->m0C34 != 0) {
            anm_init(i_this, dRes_INDEX_TN_BCK_BRUN1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                     dRes_INDEX_TN_BAS_BRUN1_e);
        } else {
            anm_init(i_this, dRes_INDEX_TN_BCK_BRUN2_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                     dRes_INDEX_TN_BAS_BRUN2_e);
        }
        yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YKIHON1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f);
    }
}

/* 024BB800 */
void path_check(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    tn_path* path = i_this->ppd;
    if (path == nullptr)
        return;
    if (i_this->m0C34 == 0 && i_this->m13FF == 0)
        return;
    gabi::Local<tn_linecheck> check;
    tn_constructLineCheck(check.get());
    gabi::Local<cXyz> startpoint;
    const f32 height = 100.0f;
    startpoint->x = i_this->current.pos.x;
    startpoint->y = static_cast<f32>(i_this->current.pos.y) + height;
    startpoint->z = i_this->current.pos.z;
    path = i_this->ppd;
    path_layout::dPnt* point = path->m_points;
    u16 count = path->m_num;
    gabi::Local<cXyz> endpoint; // one frame slot, reused by every iteration
    for (u32 i = 0; i < count; ++i, ++point) {
        endpoint->x = point->m_position.x;
        endpoint->y = point->m_position.y + height;
        endpoint->z = point->m_position.z;
        dBgS_LinChk_Set(check.get(), startpoint.get(), endpoint.get(), i_this);
        dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        const BOOL blocked = cBgS_LineCross(&play->play.mBgS, check.get());
        check_index.m[i] = static_cast<u32>(blocked) ^ 1u;
        path = i_this->ppd;
        count = path->m_num;
    }
    if (count != 0)
        point = path->m_points;

    f32 radius = 0.0f;
    const f32 radiusStep = 50.0f;
    bool found = false;
    for (int pass = 0; pass < 100 && !found; ++pass) {
        for (u32 i = 0; i < count; ++i, ++point) {
            if (check_index.m[i] == 0)
                continue;
            const f32 dy = static_cast<f32>(i_this->current.pos.y) - point->m_position.y;
            const f32 dx = static_cast<f32>(i_this->current.pos.x) - point->m_position.x;
            const f32 dz = static_cast<f32>(i_this->current.pos.z) - point->m_position.z;
            const f32 dySquared = dy * dy;
            const f32 xySquared = gabi::fmadds(dx, dx, dySquared);
            const f32 squaredDistance = gabi::fmadds(dz, dz, xySquared);
            const f32 distance = tn_sqrtf(squaredDistance);
            path = i_this->ppd;
            count = path->m_num;
            if (distance < radius) {
                const s8 selected = static_cast<s8>(i - static_cast<u32>(i_this->m1426));
                i_this->m1425 = selected;
                if (selected >= static_cast<s8>(count))
                    i_this->m1425 = static_cast<s8>(count);
                else if (selected < 0)
                    i_this->m1425 = 0;
                i_this->m1424 = static_cast<u8>(static_cast<u32>(i_this->mPathIndex) + 1u);
                found = true;
                break;
            }
        }
        radius += radiusStep;
        if (!found && pass < 99)
            point = path->m_points;
    }
    tn_prepareLineCheckDestruction(check.get());
    if (!found)
        i_this->m1424 = 0;
    cBgS_LinChk_dt(check.get(), 0);
}

/* 024BBAEC */
void* shot_s_sub(fopAc_ac_c* i_this, void* context) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (tn_IsActor(i_this) == 0)
        return nullptr;
    if (i_this == nullptr)
        return nullptr;
    if (i_this->mProcName != tn_proc::fpcNm_HIMO2_e)
        return nullptr;
    return i_this;
}

/* 024BBB3C */
fopAc_ac_c* wepon_hit_check(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfGp_get();
    const u32 oldX = damageWord(i_this->mWeponSphMove.x);
    i_this->m13FC = 0;
    damageWord(i_this->m13D8.x) = oldX;
    mDoExt_McaMorf* body = i_this->mpBodyMorf;
    const u32 oldY = damageWord(i_this->mWeponSphMove.y);
    const u32 oldZ = damageWord(i_this->mWeponSphMove.z);
    damageWord(i_this->m13D8.y) = oldY;
    damageWord(i_this->m13D8.z) = oldZ;
    const f32 animationFrame = body->mFrameCtrl.mFrame;
    const s32 attackType = i_this->m0C48;
    const s32 frame = gabi::ftoi(animationFrame);
    int firstFrame = 0;
    int lastFrame = 0;
    switch (attackType) {
    case 0:
        firstFrame = 19;
        lastFrame = 26;
        break;
    case 1:
        if ((static_cast<u8>(i_this->mRemainingEquipmentPieces) & EQUIPMENT_PIECE_BODY_ARMOR) != 0) {
            firstFrame = 19;
            lastFrame = 28;
        } else {
            firstFrame = 14;
            lastFrame = 23;
        }
        break;
    case 2:
        firstFrame = 7;
        lastFrame = 9;
        break;
    case 3:
        firstFrame = 20;
        lastFrame = 24;
        break;
    case 4:
        firstFrame = 20;
        lastFrame = 26;
        break;
    case 5:
        lastFrame = 1000;
        break;
    }
    if (frame < firstFrame || frame > lastFrame || i_this->m0C66 < 0)
        return nullptr;
    bool footAttack = false;
    if ((static_cast<u8>(i_this->mEquipmentType) & 1u) != 0) {
        const s32 matrixAttack = i_this->m0C48;
        footAttack = matrixAttack == 3;
        const u8 power = static_cast<u8>(atp_1.m[attackType]);
        body = i_this->mpBodyMorf;
        i_this->mWeponSph.SetAtAtp(power);
        i_this->mWepon2Sph.SetAtAtp(power);
        J3DModel* model = body->getModel();
        J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
        const u16 flags = static_cast<u16>(buffer->mFlags);
        Mtx34* matrices = gabi::at<Mtx34>(buffer->mpAnmMtx);
        buffer->mFlags = flags | 0x10u;
        PSMTXCopy(matrices + (matrixAttack == 3 ? 5 : 23), calc_mtx());
    } else {
        body = i_this->mpBodyMorf;
        const u8 power = static_cast<u8>(atp_0.m[attackType]);
        const s32 matrixAttack = i_this->m0C48;
        footAttack = matrixAttack == 3;
        i_this->mWepon2Sph.SetAtAtp(power);
        i_this->mWeponSph.SetAtAtp(power);
        J3DModel* model = body->getModel();
        J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
        const u16 flags = static_cast<u16>(buffer->mFlags);
        Mtx34* matrices = gabi::at<Mtx34>(buffer->mpAnmMtx);
        buffer->mFlags = flags | 0x10u;
        PSMTXCopy(matrices + (matrixAttack == 3 ? 5 : 23), calc_mtx());
    }
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = footAttack || i_this->m0C48 == 2 ? 0.0f : 150.0f;
    MtxPosition(offset.get(), &i_this->mWeponSphMove);
    offset->z = static_cast<f32>(offset->z) * 0.3333f;
    MtxPosition(offset.get(), &i_this->mWepon2SphMove);
    const u8 initialized = i_this->m13FD;
    i_this->mWeponSph.SetAtType(8);
    i_this->mWepon2Sph.SetAtType(8);
    if (initialized == 0) {
        i_this->m13FD = 1;
        tn_StartCAt(&i_this->mWeponSph, &i_this->mWeponSphMove);
        tn_StartCAt(&i_this->mWepon2Sph, &i_this->mWepon2SphMove);
        return nullptr;
    }
    i_this->mWeponSph.SetR(l_weaponRadius + 60.0f);
    tn_MoveCAt(&i_this->mWeponSph, &i_this->mWeponSphMove);
    tn_MoveCAt(&i_this->mWepon2Sph, &i_this->mWepon2SphMove);
    i_this->mWepon2Sph.SetR(l_weapon2Radius + 40.0f);
    const s32 splAttack = i_this->m0C48;
    if (splAttack <= 1 || splAttack == 4 || splAttack == 5) {
        const u8 special = splAttack <= 1 ? 6 : 7;
        i_this->mWepon2Sph.SetAtSpl(special);
        i_this->mWeponSph.SetAtSpl(special);
    } else {
        i_this->mWeponSph.SetAtSpl(0);
        i_this->mWepon2Sph.SetAtSpl(0);
    }
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    tn_SetCollision(&play->play.mCcS, &i_this->mWeponSph);
    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    tn_SetCollision(&play->play.mCcS, &i_this->mWepon2Sph);
    if (i_this->mWeponSph.ChkAtHit() == 0 && i_this->mWepon2Sph.ChkAtHit() == 0)
        return nullptr;
    dCcD_GObjInf* hit = i_this->mWeponSph.ChkAtHit() != 0 ? tn_GetAtHitObj(&i_this->mWeponSph)
                                                          : tn_GetAtHitObj(&i_this->mWepon2Sph);
    if (hit == nullptr)
        return nullptr;
    dCcD_Stts* status = hit->mStts;
    return status == nullptr ? nullptr : static_cast<fopAc_ac_c*>(status->mp_actor);
}

/* 024C18B8 */
void p_lost(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    tn_abi::playLookupWithReceiver(i_this);
    const s16 mode = i_this->mDamageReaction.mMode;
    i_this->mDamageReaction.m710 = static_cast<u8>(0);
    if (mode == 0) {
        i_this->mDamageReaction.mMode = 1;
        anm_init(i_this, dRes_INDEX_TN_BCK_AKYORO1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YKYORO1_e, 5.0f, J3DFrameCtrl::EMode_NONE, 1.0f);
        i_this->speedF = 0.0f;
    } else if (mode == 1) {
        mDoExt_McaMorf* body = i_this->mpBodyMorf;
        const u8 flags = body->mFrameCtrl.mState;
        if ((flags & 1) != 0 || body->mFrameCtrl.mRate == 0.0f) {
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            path_check(i_this);
            wait_set(i_this);
            i_this->mDamageReaction.mMode = 0;
        }
    }

    mDoExt_McaMorf* body = i_this->mpBodyMorf;
    // Truncating/saturating a frame to an integer and comparing it with 25
    // is equivalent to this comparison, including NaN and infinite frames.
    // This avoids an undefined host float-to-integer conversion.
    if (static_cast<f32>(body->mFrameCtrl.mFrame) >= 26.0f) {
        const s16 angleLimit = get_view_H(i_this);
        const u32 target = gabi::ea(static_cast<fopAc_ac_c*>(i_this->mDamageReaction.m714));
        const s16 heading = static_cast<s16>(i_this->mTargetAngle);
        if (daTn_player_view_check(i_this, gabi::at<cXyz>(target + 0x314u),
                                  heading, angleLimit) != 0) {
            i_this->mCountDownTimers[1] = 0;
            i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
            i_this->mDamageReaction.mMode = -10;
        }
    }
}

/* 024C1A3C */
void wepon_search(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfGp_get();
    const u32 id = i_this->mBokoPcId;
    gabi::Local<be<u32>> searchId;
    *searchId = id;
    const u32 weapon = id == 0xFFFFFFFFu ? 0
        : tn_abi::actorJudgeAddress(TN_ACTOR_ID_JUDGE, searchId.get());
    s16 mode = i_this->mDamageReaction.mMode;
    if (mode < 2 && (weapon == 0 || (static_cast<u32>(gabi::at<fopAc_ac_c>(weapon)->actor_status) & 0x2000u) != 0)) {
        tn_leaveWeaponSearch(i_this);
        return;
    }
    f32 distance = 10000.0f;
    if (weapon != 0) {
        const f32 weaponX = static_cast<f32>(gabi::at<fopAc_ac_c>(weapon)->current.pos.x);
        const f32 actorX = static_cast<f32>(i_this->current.pos.x);
        const f32 weaponZ = static_cast<f32>(gabi::at<fopAc_ac_c>(weapon)->current.pos.z);
        const f32 dx = weaponX - actorX;
        const f32 dz = weaponZ - static_cast<f32>(i_this->current.pos.z);
        const s16 heading = cM_atan2s(dx, dz);
        const f32 squaredDistance = gabi::fmadds(dx, dx, dz * dz);
        i_this->mDamageReaction.m4D0 = static_cast<s16>(heading);
        distance = tn_sqrtf(squaredDistance);
        mode = i_this->mDamageReaction.mMode;
    }
    const f32 zero = 0.0f;
    const f32 one = 1.0f;
    switch (mode) {
    case -1: {
        const f32 blend = 3.0f;
        i_this->mDamageReaction.mMode = 0;
        i_this->m13FF = 0;
        anm_init(i_this, dRes_INDEX_TN_BCK_AKIDUKU1_e, blend, J3DFrameCtrl::EMode_NONE, one, -1);
        yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YKIDUKU1_e, blend, J3DFrameCtrl::EMode_NONE, one);
        i_this->mCountDownTimers[1] = static_cast<s16>(distance < 900.0f ? 20 : 200);
        i_this->m03EE = static_cast<s16>(5);
        return;
    }
    case 0: {
        const s16 delay = static_cast<s16>(i_this->m03EE);
        i_this->speedF = static_cast<f32>(zero);
        if (delay != 0) return;
        cLib_addCalcAngleS2(&i_this->current.angle.y,
                   static_cast<s16>(i_this->mDamageReaction.m4D0), 2, 0x3000);
        mDoExt_McaMorf* body = i_this->mpBodyMorf;
        if ((static_cast<u8>(body->mFrameCtrl.mState) & 1u) == 0 &&
            static_cast<f32>(body->mFrameCtrl.mRate) != zero &&
            static_cast<s16>(i_this->mCountDownTimers[1]) != 0) return;
        i_this->mDamageReaction.mMode = 1;
        i_this->m03EA = static_cast<s16>(l_tnHIO.m082);
        fight_run_set(i_this);
        return;
    }
    case 1: {
        cLib_addCalc2(&i_this->speedF, static_cast<f32>(l_tnHIO.m058),
                      one, 10.0f);
        cLib_addCalcAngleS2(&i_this->current.angle.y,
                   static_cast<s16>(i_this->mDamageReaction.m4D0), 4, 0x1000);
        if (distance < REG_F(8, 2) + 150.0f) {
            i_this->mDamageReaction.mMode = 2;
            anm_init(i_this, dRes_INDEX_TN_BCK_BHIROU_K1_e, 2.0f, J3DFrameCtrl::EMode_NONE, one, dRes_INDEX_TN_BAS_BHIROU_K1_e);
            i_this->mCountDownTimers[1] = static_cast<s16>(29);
            return;
        }
        if (static_cast<s16>(i_this->m03EA) == 0) {
            i_this->mCountDownTimers[1] = static_cast<s16>(0);
            i_this->m13FF = 1;
            i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
            return;
        }
        if ((static_cast<u32>(i_this->mDamageReaction.mAcch.m_flags) & 0x30u) == 0x30u) {
            tn_leaveWeaponSearch(i_this);
            i_this->m03FC = static_cast<s16>(70);
        }
        return;
    }
    case 2: {
        i_this->speedF = static_cast<f32>(zero);
        const s16 pickupFrame = static_cast<s16>(static_cast<s32>(REG0_S(7)) + 18);
        s16 timer = static_cast<s16>(i_this->mCountDownTimers[1]);
        if (timer == pickupFrame) {
            if (weapon != 0 && (static_cast<u32>(gabi::at<fopAc_ac_c>(weapon)->actor_status) & 0x2000u) == 0) {
                i_this->m0C34 = 2;
                tn_abi::fopAcM_setCarryNow(gabi::at<fopAc_ac_c>(weapon), 0);
                timer = static_cast<s16>(i_this->mCountDownTimers[1]);
            } else {
                tn_leaveWeaponSearch(i_this);
                timer = static_cast<s16>(i_this->mCountDownTimers[1]);
            }
        }
        if (timer < 14) {
            const s16 target = static_cast<s16>(i_this->mTargetAngle);
            i_this->mDamageReaction.m710 = static_cast<u8>(1);
            i_this->mDamageReaction.m4D0 = static_cast<s16>(target);
            cLib_addCalcAngleS2(&i_this->current.angle.y, target, 3, 0x800);
        }
        mDoExt_McaMorf* body = i_this->mpBodyMorf;
        if ((static_cast<u8>(body->mFrameCtrl.mState) & 1u) != 0 ||
            static_cast<f32>(body->mFrameCtrl.mRate) == zero) tn_leaveWeaponSearch(i_this);
        return;
    }
    default: return;
    }
}

/* 024C1ED4 */
void b_nige(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    fopAc_ac_c* bomb = search_bomb(i_this, 0);
    i_this->m1418 = bomb;
    if (bomb == nullptr) {
        tn_leaveWeaponSearch(i_this);
        return;
    }
    const u32 bombBase = gabi::ea(bomb);
    const f32 bombX = static_cast<f32>(gabi::at<fopAc_ac_c>(bombBase)->current.pos.x);
    const f32 actorX = static_cast<f32>(i_this->current.pos.x);
    const f32 actorZ = static_cast<f32>(i_this->current.pos.z);
    const f32 dx = bombX - actorX;
    const f32 dz = static_cast<f32>(gabi::at<fopAc_ac_c>(bombBase)->current.pos.z) - actorZ;
    const s16 away = cM_atan2s(-dx, -dz);
    const s16 mode = i_this->mDamageReaction.mMode;
    const f32 zero = 0.0f;
    i_this->mDamageReaction.m4D0 = static_cast<s16>(away);
    const f32 safeDistance = 800.0f;
    const f32 one = 1.0f;
    bool enterWait = false;
    switch (mode) {
    case 0:
        i_this->mDamageReaction.mMode = 1;
        anm_init(i_this, dRes_INDEX_TN_BCK_AKIDUKU1_e, 3.0f, J3DFrameCtrl::EMode_NONE, one, -1);
        tn_playBombReactionSound(i_this, 0x4885);
        i_this->mCountDownTimers[1] = static_cast<s16>(10);
        [[fallthrough]];
    case 1: {
        const s16 turnTarget = static_cast<s16>(static_cast<s32>(static_cast<s16>(i_this->mDamageReaction.m4D0)) + 0x8000);
        i_this->speedF = static_cast<f32>(zero);
        cLib_addCalcAngleS2(&i_this->current.angle.y, turnTarget, 2, 0x3000);
        if (static_cast<s16>(i_this->mCountDownTimers[1]) != 0) return;
        const s32 pathDriven = i_this->m0C34;
        i_this->mDamageReaction.mMode = 2;
        anm_init(i_this, pathDriven != 0 ? 0x48 : 0x49,
                 2.0f, J3DFrameCtrl::EMode_NONE, one, pathDriven != 0 ? 0x1B : 0x1C);
        const f32 speedBase = -60.0f;
        const f32 speed = REG0_F(3) + speedBase;
        const f32 verticalBase = 80.0f;
        i_this->speedF = static_cast<f32>(speed);
        i_this->speed.y = static_cast<f32>(REG0_F(4) + verticalBase);
        tn_playBombReactionSound(i_this, 0x488A);
        return;
    }
    case 2: {
        if ((static_cast<u32>(i_this->mDamageReaction.mAcch.m_flags) & 0x20u) == 0) return;
        mDoExt_McaMorf* body = i_this->mpBodyMorf;
        if ((static_cast<u8>(body->mFrameCtrl.mState) & 1u) == 0 &&
            static_cast<f32>(body->mFrameCtrl.mRate) != zero) return;
        const f32 squaredDistance = gabi::fmadds(dx, dx, dz * dz);
        i_this->speedF = static_cast<f32>(zero);
        const f32 distance = tn_sqrtf(squaredDistance);
        if (distance > safeDistance) enterWait = true;
        else {
            i_this->mDamageReaction.mMode = 3;
            fight_run_set(i_this);
            return;
        }
        break;
    }
    case 3: {
        const f32 speed = static_cast<f32>(l_tnHIO.m058);
        const s16 target = static_cast<s16>(i_this->mDamageReaction.m4D0);
        i_this->speedF = static_cast<f32>(speed);
        cLib_addCalcAngleS2(&i_this->current.angle.y, target, 4, 0x1000);
        const f32 distance = tn_sqrtf(gabi::fmadds(dx, dx, dz * dz));
        if (!(distance > safeDistance)) return;
        enterWait = true;
        break;
    }
    case 4: {
        const s16 target = static_cast<s16>(i_this->mTargetAngle);
        i_this->speedF = static_cast<f32>(zero);
        i_this->mDamageReaction.m4D0 = static_cast<s16>(target);
        cLib_addCalcAngleS2(&i_this->current.angle.y, target, 3, 0x1000);
        const f32 distance = tn_sqrtf(gabi::fmadds(dx, dx, dz * dz));
        if (distance < 700.0f) i_this->mDamageReaction.mMode = 0;
        return;
    }
    default: return;
    }
    if (enterWait) {
        const s32 pathDriven = i_this->m0C34;
        const f32 blend = 5.0f;
        i_this->mDamageReaction.mMode = 4;
        anm_init(i_this, pathDriven != 0 ? 0x35 : 0x4D, blend, J3DFrameCtrl::EMode_LOOP, one, -1);
    }
}

/* 024C2430 */
void defence(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const u32 play = gabi::ea(dComIfGp_get());
    const s16 targetAngle = static_cast<s16>(i_this->mTargetAngle);
    const u32 player = gabi::ea(static_cast<fopAc_ac_c*>(gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer));
    i_this->mDamageReaction.m710 = static_cast<u8>(1);
    i_this->mDamageReaction.m4D0 = static_cast<s16>(targetAngle);
    cLib_addCalcAngleS2(&i_this->current.angle.y, targetAngle, 4, 1024);
    const s16 mode = i_this->mDamageReaction.mMode;
    if (mode == 0) {
        const s32 pathDriven = i_this->m0C34;
        i_this->mDamageReaction.mMode = 1;
        const int animation = pathDriven != 0 ? 0x30 : 0x31;
        anm_init(i_this, animation, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        tate_anm_init(i_this, animation, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f);
        yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YGUARD1_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f);
    } else if (mode != 1) {
        return;
    }
    const s16 timer = i_this->mCountDownTimers[1];
    i_this->m1444 = static_cast<u8>(1);
    i_this->speedF = 0.0f;
    if (timer == 0 && static_cast<u8>(gabi::at<TnPlayer>(player)->mCutType) == 0) {
        i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
        i_this->mCountDownTimers[1] = 0;
        i_this->mDamageReaction.mMode = 0;
        tate_anm_init(i_this, dRes_INDEX_TN_BCK_ATATE_ON1_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f);
    }
}

/* 024C25F8 */
void hukki(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const u32 play = gabi::ea(dComIfGp_get());
    const f32 joint13Z = i_this->mDamageReaction.m100[13].z;
    const f32 joint10X = i_this->mDamageReaction.m100[10].x;
    const f32 joint13X = i_this->mDamageReaction.m100[13].x;
    const f32 joint10Z = i_this->mDamageReaction.m100[10].z;
    const f32 directionX = joint10X - joint13X;
    const u32 matrix = gabi::ea(calc_mtx());
    const f32 directionZ = joint10Z - joint13Z;
    const u32 player = gabi::ea(static_cast<fopAc_ac_c*>(gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer));
    const s16 heading = cM_atan2s(directionX, directionZ);
    tn_abi::cMtx_YrotS(gabi::at<Mtx34>(matrix), heading);

    const s16 mode = i_this->mDamageReaction.mMode;
    const f32 zero = 0.0f;
    const f32 one = 1.0f;
    gabi::Local<cXyz> offset;
    offset->x = zero;
    offset->y = zero;
    if (mode == 14) {
        const s16 target = static_cast<s16>(i_this->mTargetAngle);
        i_this->mDamageReaction.m710 = static_cast<u8>(1);
        i_this->mDamageReaction.m4D0 = static_cast<s16>(target);
        cLib_addCalcAngleS2(&i_this->current.angle.y, target, 3, 0x1000);
        if (static_cast<s16>(i_this->mCountDownTimers[1]) == 0) {
            i_this->mDamageReaction.mAction = ACTION_FIGHT;
            i_this->mDamageReaction.mMode = 0;
        }
        return;
    }
    if (mode != 10 && mode != 12 && mode != 13) return;
    if (mode == 10 || mode == 12) {
        anm_init(i_this, mode == 10 ? 0x4E : 0x4F, zero, J3DFrameCtrl::EMode_NONE, one, mode == 10 ? 0x1E : 0x1F);
        yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YKIHON1_e, 10.0f, J3DFrameCtrl::EMode_NONE, one);
        const s32 timer = mode == 10 ? 15 : static_cast<s32>(REG0_S(5)) + 15;
        offset->z = 125.0f;
        i_this->mCountDownTimers[2] = static_cast<s16>(timer);
        const s32 immunity = static_cast<s32>(REG_S(10, 2)) + 35;
        i_this->speedF = static_cast<f32>(zero);
        i_this->m03F0 = static_cast<s16>(immunity);
        i_this->mDamageReaction.mMode = 13;
        i_this->mDamageReaction.m480 = static_cast<s16>(0);
        MtxPosition(offset.get(), &i_this->mDamageReaction.m458);
        const f32 currentY = static_cast<f32>(i_this->current.pos.y);
        const f32 heightCorrection = static_cast<f32>(i_this->mDamageReaction.m44C.y);
        const f32 oldY = static_cast<f32>(i_this->old.pos.y);
        i_this->current.pos.y = static_cast<f32>(currentY - heightCorrection);
        if (mode == 10) i_this->mDamageReaction.m44C.y = static_cast<f32>(zero);
        i_this->old.pos.y = static_cast<f32>(oldY - heightCorrection);
        if (mode == 12) i_this->mDamageReaction.m44C.y = static_cast<f32>(zero);
    }
    const s32 slideStart = static_cast<s32>(REG_S(6, 5)) + 6;
    if (static_cast<s16>(i_this->mCountDownTimers[2]) <= slideStart) {
        const f32 tuning = REG_F(6, 5);
        const f32 baseStep = 100.0f;
        const f32 stepScale = 0.25f;
        const f32 stepX = (tuning + baseStep) * stepScale;
        cLib_addCalc0(reinterpret_cast<be<f32>*>(&i_this->mDamageReaction.m458), one, stepX);
        const f32 stepZ = (REG_F(6, 5) + baseStep) * stepScale;
        cLib_addCalc0(&i_this->mDamageReaction.m458.z, one, stepZ);
        const s32 slideEnd = static_cast<s32>(REG_S(6, 6)) + 1;
        if (static_cast<s16>(i_this->mCountDownTimers[2]) >= slideEnd) {
            offset->z = 25.0f;
            gabi::Local<cXyz> movement;
            MtxPosition(offset.get(), movement.get());
            const f32 x = static_cast<f32>(i_this->current.pos.x) + static_cast<f32>(movement->x);
            const f32 z = static_cast<f32>(i_this->current.pos.z);
            i_this->current.pos.x = static_cast<f32>(x);
            i_this->current.pos.z = static_cast<f32>(z + static_cast<f32>(movement->z));
        }
    }
    mDoExt_McaMorf* body = i_this->mpBodyMorf;
    if ((static_cast<u8>(body->mFrameCtrl.mState) & 1u) == 0 &&
        static_cast<f32>(body->mFrameCtrl.mRate) != zero) return;
    if (!(static_cast<f32>(gabi::at<TnPlayer>(player)->field_0x2b0) < zero) &&
        static_cast<f32>(i_this->mPlayerDistance) < static_cast<f32>(l_tnHIO.m02C)) {
        i_this->mDamageReaction.mMode = 14;
        i_this->mCountDownTimers[1] = static_cast<s16>(10);
        i_this->mDamageReaction.m488 = static_cast<s16>(0);
    } else {
        i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
        path_check(i_this);
        wait_set(i_this);
        i_this->mDamageReaction.mMode = 0;
        i_this->mDamageReaction.m488 = static_cast<s16>(0);
    }
}

/* 024C29CC */
void aite_miru(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const s16 mode = i_this->mDamageReaction.mMode;
    if (mode == 0) {
        const s16 delay = REG0_S(8);
        i_this->speedF = 0.0f;
        i_this->mCountDownTimers[1] = static_cast<s16>(static_cast<s32>(delay) + 20);
        i_this->mDamageReaction.mMode = 1;
    } else if (mode != 1) {
        return;
    }
    cLib_addCalcAngleS2(&i_this->m1404, 12000, 2, 6144);
    const u32 targetId = static_cast<u32>(i_this->m1408);
    i_this->mDamageReaction.m710 = static_cast<u8>(1);
    if (targetId != 0xFFFFFFFFu) {
        gabi::Local<be<u32>> searchId;
        *searchId = targetId;
        fopAc_ac_c* target = tn_abi::actorJudge(gabi::at<void>(TN_ACTOR_ID_JUDGE), searchId.get());
        if (target != nullptr) i_this->mDamageReaction.m714 = gabi::at<fopAc_ac_c>(gabi::ea(target));
    }
    if (i_this->mCountDownTimers[1] == 0) {
        i_this->m1408 = static_cast<u32>(0xFFFFFFFFu);
        i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
        i_this->mDamageReaction.mMode = 0;
        path_check(i_this);
    }
}

/* 024C2AAC */
void fail(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    i_this->attention_info.flags = static_cast<u32>(0);
    i_this->m03F0 = static_cast<s16>(5);
    const s16 mode = i_this->mDamageReaction.mMode;
    i_this->speedF = 0.0f;
    i_this->speed.y = 0.0f;
    if (mode != 0) return;

    gabi::Local<cXyz> dropPosition;
    dropPosition->x = i_this->current.pos.x;
    const f32 height = static_cast<f32>(l_tnHIO.m018) + 100.0f;
    const f32 actorY = i_this->current.pos.y;
    dropPosition->y = actorY + height;
    dropPosition->z = i_this->current.pos.z;
    const s16 timer = i_this->mCountDownTimers[2];
    const u8 itemBits = i_this->stealItemBitNo;
    fopAcM_createDisappear(i_this, dropPosition.get(), 10, timer >= 1000 ? 1 : 0, itemBits);

    if (static_cast<s8>(i_this->m14F8) != 0) {
        const s16 oldMode = i_this->mDamageReaction.mMode;
        const u8 switchNo = i_this->mDisableSpawnOnDeathSwitch;
        i_this->m1520 = 1;
        i_this->mDamageReaction.mMode = static_cast<s16>(static_cast<s32>(oldMode) + 1);
        if (switchNo != 0) {
            const u32 save = dComIfGs_ea();
            const s8 room = i_this->current.roomNo;
            tn_abi::saveOnSwitch(gabi::at<u8>(save + 0x20u), switchNo, room);
        }
    } else {
        fopAcM_delete(i_this);
        const u8 switchNo = i_this->mDisableSpawnOnDeathSwitch;
        if (switchNo != 0) {
            const u32 save = dComIfGs_ea();
            const s8 room = i_this->current.roomNo;
            tn_abi::saveOnSwitch(gabi::at<u8>(save + 0x20u), switchNo, room);
        }
    }
    const s8 homeRoom = i_this->home.roomNo;
    const u32 save = dComIfGs_ea();
    const u16 actorId = i_this->setID;
    tn_abi::saveOnActor(gabi::at<u8>(save + 0x20u), actorId, homeRoom);
    i_this->mDamageReaction.mMode = 1;
}

/* 024C2BEC */
void yogan_fail(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    i_this->m03F0 = static_cast<s16>(5);
    const s16 mode = i_this->mDamageReaction.mMode;
    i_this->attention_info.flags = static_cast<u32>(0);
    i_this->speedF = 0.0f;
    if (mode != 0 && mode != 1) return;
    if (mode == 0) {
        const s32 pathDriven = i_this->m0C34;
        i_this->mDamageReaction.mMode = 1;
        const f32 upwardSpeed = REG0_F(19) + 190.0f;
        i_this->mDamageReaction.m458.y = static_cast<f32>(0.0f);
        i_this->mDamageReaction.m44C.y = static_cast<f32>(0.0f);
        i_this->speed.y = upwardSpeed;
        if (pathDriven != 0) i_this->m0C38 = static_cast<u32>(1);
    }
    dComIfGp_particle_setSimple(0x8061, &i_this->current.pos);
    dComIfGp_particle_setSimple(0x8058, &i_this->current.pos);
    if ((static_cast<u32>(i_this->m03D8) & 3u) == 0) {
        const f32 randomAngle = cM_rndF(65536.0f);
        i_this->m0428.y = static_cast<s16>(static_cast<s16>(gabi::ftoi(randomAngle)));
        i_this->m0428.x = static_cast<s16>(-8192);
        dComIfGp_particle_set(0x0E, &i_this->m1384,
                              &i_this->m0428);
    }
    cLib_addCalcAngleS2(&i_this->current.angle.x, -16384, 10, 0x200);
    if (static_cast<f32>(i_this->speed.y) < 0.0f) {
        i_this->mCountDownTimers[2] = 2000;
        i_this->mDamageReaction.mAction = ACTION_FAIL;
        i_this->mDamageReaction.mMode = 0;
    }
}

/* 024C2DD0 */
void d_mahi(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const s16 mode = i_this->mDamageReaction.mMode;
    if (mode == 0) {
        i_this->mDamageReaction.mMode = 1;
        anm_init(i_this, dRes_INDEX_TN_BCK_AMAHI1_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
        yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YMAHI1_e, 3.0f, J3DFrameCtrl::EMode_NONE, 1.0f);
        i_this->mCountDownTimers[0] = 60;
        i_this->speedF = 0.0f;
        return;
    }
    if (mode != 1) return;

    const s16 timer = i_this->mCountDownTimers[0];
    if (static_cast<u32>(static_cast<s32>(timer) - 10) < 31u) {
        if (timer == 40) tn_abi::enemy_piyo_set(i_this);
        const u32 eyeAddress = gabi::ea(i_this) + 0x37Cu;
        if (i_this != nullptr && eyeAddress != 0) {
            const s8 room = i_this->current.roomNo;
            const s32 reverb = tn_getReverb(room);
            mDoAud_seStart(0x50BC, gabi::at<cXyz>(eyeAddress), 0, reverb);
        }
    }

    const s16 remaining = i_this->mCountDownTimers[0];
    i_this->speedF = 0.0f;
    if (remaining == 0) {
        i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
        path_check(i_this);
        wait_set(i_this);
        i_this->mDamageReaction.mMode = 2;
    }
}

/* 024C2F6C */
void d_sit(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const s16 mode = i_this->mDamageReaction.mMode;
    if (mode == 0 || mode == 1) {
        if (mode == 0) {
            i_this->mDamageReaction.mMode = 1;
            anm_init(i_this, dRes_INDEX_TN_BCK_AHAZURE1_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_TN_BAS_AHAZURE1_e);
            yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YHAZURE1_e, 2.0f, J3DFrameCtrl::EMode_NONE, 1.0f);
        }
        mDoExt_McaMorf* body = i_this->mpBodyMorf;
        i_this->speedF = 0.0f;
        const u8 flags = body->mFrameCtrl.mState;
        if ((flags & 1) != 0 || body->mFrameCtrl.mRate == 0.0f) tn_resumeWaiting(i_this);
    } else if (mode == 10 || mode == 11) {
        if (mode == 10) {
            i_this->mDamageReaction.mMode = 11;
            anm_init(i_this, dRes_INDEX_TN_BCK_AKYORO1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 2.0f, -1);
            yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YKYORO1_e, 5.0f, J3DFrameCtrl::EMode_LOOP, 2.0f);
            i_this->mCountDownTimers[0] = 55;
        }
        const s16 timer = i_this->mCountDownTimers[0];
        i_this->speedF = 0.0f;
        if (timer == 0) tn_resumeWaiting(i_this);
    }
}

/* 024C3118 */
void d_dozou(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const s16 mode = i_this->mDamageReaction.mMode;
    i_this->m03F0 = static_cast<s16>(5);
    i_this->attention_info.flags = static_cast<u32>(0);
    if (mode == 0 || mode == 1) {
        if (mode == 0) {
            i_this->mDamageReaction.mMode = 1;
            tn_abi::dCcD_Stts_Init(reinterpret_cast<dCcD_Stts*>(&i_this->mDamageReaction.mStts), 0xFF, 0xFF, i_this);
            if (i_this->mRangeOrFrozenAnim == 0) {
                anm_init(i_this, dRes_INDEX_TN_BCK_AKYORO1_e, 1.0f, J3DFrameCtrl::EMode_NONE, 0.0f, -1);
                mDoExt_McaMorf* body = i_this->mpBodyMorf;
                body->mFrameCtrl.mFrame = 41.0f;
            } else {
                anm_init(i_this, dRes_INDEX_TN_BCK_AAT_YOKO1_e, 1.0f, J3DFrameCtrl::EMode_NONE, 0.0f, dRes_INDEX_TN_BAS_AAT_YOKO1_e);
                mDoExt_McaMorf* body = i_this->mpBodyMorf;
                body->mFrameCtrl.mFrame = 27.0f;
            }
            yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YKIHON1_e, 0.0f, J3DFrameCtrl::EMode_NONE, 1.0f);
        }
        bool awaken = false;
        const u8 switchNo = i_this->mEnableSpawnSwitch;
        if (switchNo != 0xFF) {
            const u32 save = dComIfGs_ea();
            const s8 room = i_this->current.roomNo;
            awaken = tn_abi::saveIsSwitch(gabi::at<u8>(save + 0x20u), switchNo, room) != 0;
            if (!awaken && i_this->mEnableSpawnSwitch != 0xFF) return;
        }
        if (!awaken) {
            const u32 save = dComIfGs_ea();
            awaken = tn_abi::saveIsEventBit(gabi::at<u8>(save + 0x644u), 0x3802) != 0;
        }
        if (awaken) {
            mDoExt_McaMorf* body = i_this->mpBodyMorf;
            body->mFrameCtrl.mRate = 1.0f;
            i_this->mDamageReaction.mMode = 2;
        }
    } else if (mode == 2) {
        mDoExt_McaMorf* body = i_this->mpBodyMorf;
        const u8 flags = body->mFrameCtrl.mState;
        if ((flags & 1) != 0 || body->mFrameCtrl.mRate == 0.0f) {
            tn_abi::dCcD_Stts_Init(reinterpret_cast<dCcD_Stts*>(&i_this->mDamageReaction.mStts), 0xF0, 0xFF, i_this);
            i_this->mDamageReaction.mAction = ACTION_JYUNKAI;
            i_this->mDamageReaction.mMode = 0;
            i_this->attention_info.flags = static_cast<u32>(4);
        }
    }
}

/* 024C3348 */
void s_demo(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    i_this->m03F0 = static_cast<s16>(5);
    i_this->attention_info.flags = static_cast<u32>(0);
    mDoExt_McaMorf* body = i_this->mpBodyMorf;
    const f32 zero = 0.0f;
    const f32 animationFrame = body->mFrameCtrl.mFrame;
    const s16 mode = i_this->mDamageReaction.mMode;
    const s32 frame = gabi::ftoi(animationFrame);
    const f32 one = 1.0f;
    const f32 blend = 3.0f;
    switch (mode) {
    case 0: {
        const f32 initialBlend = 10.0f;
        i_this->mDamageReaction.mMode = 1;
        anm_init(i_this, dRes_INDEX_TN_BCK_ANIOU1_e, initialBlend, J3DFrameCtrl::EMode_LOOP, one, -1);
        yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YKIHON1_e, initialBlend, J3DFrameCtrl::EMode_NONE, one);
        [[fallthrough]];
    }
    case 1: {
        if (REG0_S(0) == 0) {
            const s8 room = i_this->current.roomNo;
            const u32 save = dComIfGs_ea();
            const u8 spawnSwitch = static_cast<u8>(i_this->mEnableSpawnSwitch);
            if (tn_abi::saveIsSwitch(gabi::at<u8>(save + 0x20u), spawnSwitch, room) == 0) return;
        }
        tn_abi::mDoAud_bgmAllMute(30);
        anm_init(i_this, dRes_INDEX_TN_BCK_S_DEMO_M1_e, blend, J3DFrameCtrl::EMode_NONE, one, dRes_INDEX_TN_BAS_S_DEMO_M1_e);
        yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_S_DEMO_Y1_e, blend, J3DFrameCtrl::EMode_NONE, one);
        i_this->mDamageReaction.mMode = 2;
        return;
    }
    case 2:
        if (frame == 127) { tn_playBombReactionSound(i_this, 0x4885); body = i_this->mpBodyMorf; }
        else if (frame == 172) { tn_abi::mDoAud_subBgmStart(0x80000019u); body = i_this->mpBodyMorf; }
        else if (frame == 193) { tn_playBombReactionSound(i_this, 0x4888); body = i_this->mpBodyMorf; }
        else if (frame == 231 || frame == 262) { tn_playBombReactionSound(i_this, 0x4889); body = i_this->mpBodyMorf; }
        if (tn_demoAnimationStopped(body, zero)) {
            i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
            i_this->mDamageReaction.mMode = 0;
        }
        return;
    case 10: {
        const f32 disabledHeight = 10000.0f;
        const f32 verticalOffset = 5000.0f;
        i_this->mDamageReaction.mMaxFallDistance = static_cast<f32>(disabledHeight);
        i_this->current.pos.y = static_cast<f32>(i_this->home.pos.y) + verticalOffset;
        i_this->speed.y = static_cast<f32>(zero);
        if (tn_eventRunning(0x100404A0) == 0 && tn_eventRunning(0x100404D0) == 0 && static_cast<s8>(s_check) == 0) return;
        const s32 staff = tn_getDemoStaff();
        if (static_cast<s8>(s_check) == 0 && (staff == -1 || !tn_staffActionEquals(staff, 0x10040380))) return;
        tn_abi::mDoAud_bgmAllMute(30);
        const f32 z = i_this->current.pos.z;
        const f32 zOffset = 50.0f;
        const f32 homeX = static_cast<f32>(i_this->home.pos.x);
        const f32 x = i_this->current.pos.x;
        i_this->current.pos.z = z - zOffset;
        if (homeX < zero) {
            const f32 xOffset = 250.0f;
            const f32 yOffset = 2750.0f;
            const f32 homeY = static_cast<f32>(i_this->home.pos.y);
            i_this->m1519 = static_cast<u8>(1);
            i_this->current.pos.x = x + xOffset;
            i_this->current.pos.y = homeY + yOffset;
        } else {
            const f32 homeY = static_cast<f32>(i_this->home.pos.y);
            const f32 xOffset = -130.0f;
            const f32 yOffset = 2000.0f;
            i_this->m1519 = static_cast<u8>(0);
            i_this->current.pos.x = x + xOffset;
            i_this->current.pos.y = homeY + yOffset;
        }
        tn_playPartImpact(i_this, 0x5929);
        s_check = static_cast<u8>(static_cast<u8>(static_cast<u8>(s_check) + 1u));
        anm_init(i_this, dRes_INDEX_TN_BCK_S_DEMO_M2_e, blend, J3DFrameCtrl::EMode_NONE, zero, dRes_INDEX_TN_BAS_S_DEMO_M2_e);
        yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_S_DEMO_Y2_e, blend, J3DFrameCtrl::EMode_NONE, zero);
        tate_anm_init(i_this, dRes_INDEX_TN_BCK_S_DEMO_M2_e, blend, J3DFrameCtrl::EMode_LOOP, zero);
        i_this->mDamageReaction.mMode = 11;
        return;
    }
    case 11: {
        if (tn_eventRunning(0x100404A0) != 0 || tn_eventRunning(0x100404D0) != 0) {
            const s32 staff = tn_getDemoStaff();
            if (staff != -1 && tn_staffActionEquals(staff, 0x10040370)) {
                i_this->current.pos.x = static_cast<f32>(i_this->home.pos.x);
                const u32 play = gabi::ea(dComIfGp_get());
                const u32 player = gabi::ea(static_cast<fopAc_ac_c*>(gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer));
                if (player != 0) {
                    const f32 dx = static_cast<f32>(gabi::at<fopAc_ac_c>(player)->current.pos.x) - static_cast<f32>(i_this->current.pos.x);
                    const f32 dz = static_cast<f32>(gabi::at<fopAc_ac_c>(player)->current.pos.z) - static_cast<f32>(i_this->current.pos.z);
                    i_this->shape_angle.y = cM_atan2s(dx, dz);
                }
            }
        }
        if ((static_cast<u32>(i_this->mDamageReaction.mAcch.m_flags) & 0x20u) == 0) return;
        tn_setDemoAnimationRates(i_this, i_this->mpBodyMorf, one);
        tn_playPartImpact(i_this, 0x592A);
        const s8 side = static_cast<s8>(i_this->m1519);
        const f32 homeX = static_cast<f32>(i_this->home.pos.x);
        i_this->mDamageReaction.mMode = side != 0 ? 13 : 12;
        const u32 vibration = gabi::ea(dComIfGp_get()) + 0x599Cu;
        gabi::Local<cXyz> direction;
        direction->x = zero;
        direction->y = one;
        direction->z = zero;
        tn_abi::vibrationStartShock(gabi::at<u8>(vibration), homeX < zero ? 5 : 2, -17, direction.get());
        return;
    }
    case 12:
        if (gabi::ftoi(static_cast<f32>(body->mFrameCtrl.mFrame)) == 43) {
            if (static_cast<s8>(s_check) == 10) i_this->mDamageReaction.mMode = 13;
            else tn_setDemoAnimationRates(i_this, body, zero);
        }
        return;
    case 13: {
        tn_setDemoAnimationRates(i_this, body, one);
        body = i_this->mpBodyMorf;
        if (gabi::ftoi(static_cast<f32>(body->mFrameCtrl.mFrame)) == 43) s_check = static_cast<u8>(10);
        if (frame == 63) tn_playBombReactionSound(i_this, 0x4888);
        else if (frame == 115) tn_playBombReactionSound(i_this, 0x4889);
        const s32 shieldFrame = static_cast<s32>(REG0_S(7)) + 210;
        if (static_cast<u32>(frame) == static_cast<u32>(shieldFrame)) tate_anm_init(i_this, dRes_INDEX_TN_BCK_ATATE_ON1_e, 20.0f, J3DFrameCtrl::EMode_LOOP, one);
        if (tn_demoAnimationStopped(i_this->mpBodyMorf, zero)) {
            i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
            i_this->mDamageReaction.mMode = 0;
            i_this->mDamageReaction.mMaxFallDistance = static_cast<f32>(1000.0f);
            tn_abi::mDoAud_subBgmStart(0x80000019u);
        }
        return;
    }
    case 20:
        i_this->mDamageReaction.mMode = 21;
        anm_init(i_this, dRes_INDEX_TN_BCK_ANIOU1_e, one, J3DFrameCtrl::EMode_LOOP, one, -1);
        yoroi_anm_init(i_this, dRes_INDEX_TN_BCK_YKIHON1_e, one, J3DFrameCtrl::EMode_NONE, one);
        i_this->mCountDownTimers[0] = static_cast<s16>(30);
        return;
    case 21:
        if (static_cast<s16>(i_this->mCountDownTimers[0]) == 0) {
            anm_init(i_this, dRes_INDEX_TN_BCK_AKAMAE1_e, 15.0f, J3DFrameCtrl::EMode_LOOP, one, -1);
            i_this->mCountDownTimers[0] = static_cast<s16>(30);
            i_this->mDamageReaction.mMode = 22;
            tn_playBombReactionSound(i_this, 0x4888);
        }
        return;
    case 22:
        if (static_cast<s16>(i_this->mCountDownTimers[0]) == 0) {
            i_this->mDamageReaction.mAction = ACTION_FIGHT_RUN;
            i_this->mDamageReaction.mMode = 2;
            i_this->mCountDownTimers[3] = static_cast<s16>(60);
        }
        return;
    default: return;
    }
}

// Movement owns the action dispatch and retains its constants across all calls.
/* 024C3E64 */
void Tn_move(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const u32 playerBase = gabi::ea(static_cast<fopAc_ac_c*>(reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mPlayerInfo[0].mpPlayer));
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(playerBase);
    i_this->m0418 = static_cast<s16>(0);
    i_this->mDamageReaction.m710 = static_cast<u8>(0);
    cLib_addCalcAngleS2(&i_this->m1404, 0, 2, 0x800);
    const f32 zero = 0.0f;
    if (tn_processAirborneAttack(i_this, zero)) return;
    const f32 blend = 10.0f;
    if (tn_prepareNormalMovement(i_this, player)) {
        const s16 action = i_this->mDamageReaction.mAction;
        const f32 one = 1.0f;
        const f32 acceleration = 5.0f;
        switch (action) {
        case ACTION_JYUNKAI: tn_movePatrol(i_this, zero, one, blend, acceleration); break;
        case ACTION_STAND: tn_moveReturnHome(i_this, zero, one, blend, acceleration); break;
        case ACTION_FIGHT_RUN: tn_moveCombat(i_this, zero, one, blend, acceleration); break;
        case ACTION_FIGHT: tn_moveAttack(i_this, zero, one, blend, acceleration); break;
        default: tn_dispatchSeparateMovementAction(i_this, action); break;
        }
    }
    tn_finishMovement(i_this, static_cast<s16>(i_this->mDamageReaction.mAction), zero, blend);
}

/* 024BC070 */
void yoroi_break(tn_class* i_this, cXyz* position, int variant) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const u32 base = gabi::ea(i_this);
    const u32 eyeAddress = base + 0x37Cu;
    if (i_this != nullptr && eyeAddress != 0) {
        const s8 room = i_this->current.roomNo;
        const s32 reverb = tn_getReverb(room);
        tn_seStart(0x586A, gabi::at<cXyz>(eyeAddress), 0, reverb);
    }
    gabi::Local<cXyz> particleScale;
    const f32 scale = (variant == 0 ? 0.6f : 0.4f);
    particleScale->x = scale;
    particleScale->y = scale;
    particleScale->z = scale;
    dComIfGp_particle_set(0x13, position, nullptr, particleScale.get());
    dComIfGp_particle_set(0x16, position, nullptr, particleScale.get());

    const f32 divisor = 255.0f;
    u8 primary[3];
    u8 environment[3];
    f32 light[3];
    for (u32 i = 0; i < 3; ++i) {
        light[i] = static_cast<f32>(reinterpret_cast<be<u8>*>(&i_this->tevStr.mColorK0)[i]) / divisor;
        const f32 primaryChannel = static_cast<f32>(tn_colorChannel(&prim, i_this->mArmorColorIndex, i));
        primary[i] = static_cast<u8>(gabi::ftoi(primaryChannel * light[i]));
    }
    for (u32 i = 0; i < 3; ++i) {
        const f32 environmentChannel = static_cast<f32>(tn_colorChannel(&env, i_this->mArmorColorIndex, i));
        environment[i] = static_cast<u8>(gabi::ftoi(environmentChannel * light[i]));
    }
    if (variant == 0) {
        JPABaseEmitter* emitter = dComIfGp_particle_set(0x81B4, position);
        tn_setBreakParticleColors(emitter, primary, environment);
    }
    JPABaseEmitter* emitter = dComIfGp_particle_set(0x81B5, position);
    tn_setBreakParticleColors(emitter, primary, environment);
}

/* 024BC3EC */
void part_move(tn_class* i_this, int index) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const u32 base = gabi::ea(i_this);
    const u32 play = gabi::ea(dComIfGp_get());
    const u32 player = gabi::ea(static_cast<fopAc_ac_c*>(gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer));
    tn_p* part = gabi::at<tn_p>(base + 0x3F8u + static_cast<u32>(index) * 0x4Cu);
    gabi::Local<tn_groundcheck> ground;
    tn_constructGroundCheck(ground.get());
    const f32 zero = 0.0f;
    gabi::Local<cXyz> offset;
    offset->x = zero;
    offset->y = zero;
    offset->z = zero;
    part->m42 = static_cast<s16>(static_cast<s32>(part->m42) + 1);
    const s8 timer = part->m44;
    // HD: part index 3 also reads the adjacent guest table bytes.
    const u32 joint = gabi::load<u32>(gabi::ea(&j_dt) + static_cast<u32>(index) * 4u);
    if (timer != 0) part->m44 = static_cast<s8>(static_cast<s32>(timer) - 1);
    const s8 state = part->m08;
    const f32 launchSpeed = 15.0f;
    const f32 randomRotation = 3000.0f;
    const f32 horizontalBounce = 20.0f;
    if (state == 0) {
        tn_selectJointMatrix(i_this->mpBodyMorf, joint);
        MtxPosition(offset.get(), &part->m0C);
        const u8 detachFlags = static_cast<u8>(i_this->m02DD);
        if ((index == 0 && (detachFlags & 1u) != 0) ||
            (index == 1 && (detachFlags & 2u) != 0) ||
            (index == 2 && (detachFlags & 4u) != 0)) {
            part->m08 = 1;
            if (index == 0) {
                part->m44 = 13;
                tn_abi::cMtx_YrotS(calc_mtx(), static_cast<s16>(i_this->current.angle.y));
                offset->x = zero;
                offset->y = zero;
                offset->z = REG0_F(7) + launchSpeed;
                MtxPosition(offset.get(), &part->m24);
            } else part->m44 = 0;
        }
    } else if (state == 1) {
        tn_selectJointMatrix(i_this->mpBodyMorf, joint);
        MtxPosition(offset.get(), &part->m0C);
        if (part->m44 == 0) {
            tn_playBombReactionSound(i_this, 0x488C);
            part->m08 = 5;
            part->m44 = 30;
            part->m09 = 1;
            part->m30.y = static_cast<s16>(static_cast<s32>(i_this->current.angle.y) + REG0_S(5));
            part->m30.z = 0x4000;
            if (index == 0) {
                const u32 status = static_cast<u32>(i_this->actor_status);
                const u8 equipment = i_this->mRemainingEquipmentPieces;
                const u8 count = i_this->m02DC;
                i_this->actor_status = static_cast<u32>(status | 0x200000u);
                i_this->mRemainingEquipmentPieces = equipment & 0xFEu;
                i_this->m02DC = static_cast<u8>(count - 1u);
            } else if (index == 1) {
                i_this->mRemainingEquipmentPieces = static_cast<u8>(i_this->mRemainingEquipmentPieces) & 0xFDu;
                tn_abi::cMtx_YrotS(calc_mtx(), static_cast<s16>(gabi::at<fopAc_ac_c>(player)->shape_angle.y));
                offset->x = zero;
                offset->y = horizontalBounce;
                offset->z = -20.0f;
                MtxPosition(offset.get(), &part->m24);
                part->m36.x = tn_randomPartRotation(randomRotation);
                part->m36.y = tn_randomPartRotation(randomRotation);
                i_this->m02DC = static_cast<u8>(static_cast<u8>(i_this->m02DC) - 1u);
            } else {
                if (index == 2) i_this->mRemainingEquipmentPieces = static_cast<u8>(i_this->mRemainingEquipmentPieces) & 0xFBu;
                i_this->m02DC = static_cast<u8>(static_cast<u8>(i_this->m02DC) - 1u);
            }
        }
    } else if (state == 5) {
        tn_moveFlyingPart(i_this, part, index, ground.get(), zero, horizontalBounce, randomRotation);
    }
    J3DModel* model = index == 0 ? i_this->mpArmorMorf->getModel() : static_cast<J3DModel*>(part->mpPartModel);
    const s8 transformMode = part->m09;
    if (transformMode == 1) {
        MtxTrans(static_cast<f32>(part->m0C.x), static_cast<f32>(part->m0C.y), static_cast<f32>(part->m0C.z), 0);
        tn_abi::cMtx_YrotM(calc_mtx(), static_cast<s16>(part->m30.y));
        tn_abi::cMtx_XrotM(calc_mtx(), static_cast<s16>(part->m30.x));
        tn_abi::cMtx_ZrotM(calc_mtx(), static_cast<s16>(part->m30.z));
        mtx_copy(&model->mBaseTransformMtx, calc_mtx());
    } else if (transformMode == 0) {
        tn_selectJointMatrix(i_this->mpBodyMorf, joint);
        s16 wobble = static_cast<s16>(i_this->m1402);
        if (index == 0 && wobble != 0) {
            const u32 firstAngle = (static_cast<u32>(static_cast<s32>(wobble)) * 0x5100u & 0xFFFFu) & ~7u;
            const f32 cosine = cM_scos(firstAngle);
            const f32 scalar = cosine * static_cast<f32>(wobble);
            const f32 factor = -0.0033f;
            const f32 one = 1.0f;
            const f32 scale = gabi::fmadds(scalar, factor, one);
            MtxScale(scale, scale, scale, 1);
            wobble = static_cast<s16>(i_this->m1402);
            const u32 secondAngle = (static_cast<u32>(static_cast<s32>(wobble)) * 0x3900u & 0xFFFFu) & ~7u;
            const f32 baseAngle = 0.01f;
            const f32 tuning = REG0_F(16);
            const f32 amplitude = static_cast<f32>(wobble) * (tuning + baseAngle);
            const f32 cosine2 = cM_scos(secondAngle);
            tn_MtxRotY(1, cosine2 * amplitude);
            wobble = static_cast<s16>(i_this->m1402);
            const u32 thirdAngle = (static_cast<u32>(static_cast<s32>(wobble)) * 0x4200u & 0xFFFFu) & ~7u;
            const f32 sine = cM_ssin(thirdAngle);
            tn_abi::MtxRotX(1, sine * amplitude);
        }
        mtx_copy(&model->mBaseTransformMtx, calc_mtx());
    }
    if (index == 0) {
        i_this->mpArmorMorf->calc();
        const u32 mantleId = i_this->mMantPcId;
        if (mantleId != 0xFFFFu) {
            gabi::Local<be<u32>> id;
            *id = mantleId;
            const u32 mantle = mantleId == 0xFFFFFFFFu ? 0 : tn_abi::actorJudgeAddress(TN_ACTOR_ID_JUDGE, id.get());
            if (mantle != 0) {
                tn_selectJointMatrix(i_this->mpArmorMorf, static_cast<u32>(static_cast<s32>(REG_S(6, 5)) + 6));
                offset->y = REG0_F(1) + launchSpeed;
                const f32 offsetX = REG0_F(0);
                const f32 xConstant = 2.0f;
                const f32 offsetZ = REG0_F(2);
                offset->x = offsetX + xConstant;
                offset->z = offsetZ + -43.0f;
                MtxPosition(offset.get(), &gabi::at<TnMantle>(mantle)->m1BE0);
                tn_selectJointMatrix(i_this->mpArmorMorf, static_cast<u32>(static_cast<s32>(REG_S(6, 6)) + 4));
                const f32 secondX = REG0_F(3);
                const f32 secondConstant = 2.0f;
                const f32 secondY = REG0_F(4);
                const f32 zConstant = 43.0f;
                offset->x = secondX + secondConstant;
                offset->y = secondY + launchSpeed;
                offset->z = REG0_F(5) + zConstant;
                MtxPosition(offset.get(), &gabi::at<TnMantle>(mantle)->m1BEC);
                gabi::at<TnMantle>(mantle)->current.pos.x = i_this->eyePos.x; gabi::at<TnMantle>(mantle)->current.pos.y = i_this->eyePos.y; gabi::at<TnMantle>(mantle)->current.pos.z = i_this->eyePos.z;
                gabi::at<TnMantle>(mantle)->current.angle.x = i_this->current.angle.x; gabi::at<TnMantle>(mantle)->current.angle.y = i_this->current.angle.y; gabi::at<TnMantle>(mantle)->current.angle.z = i_this->current.angle.z;
            }
        }
    }
    if (index < 3) part->mpPartBrkAnm->mFrameCtrl.mFrame = static_cast<f32>(static_cast<u8>(i_this->mArmorColorIndex));
    tn_destroyGroundCheck(ground.get());
}

// HD: execute retains its native guest frame and outgoing call-linkage area.
/* 024BD170 */
BOOL daTn_Execute(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    using namespace tn_execute;
    auto prefix = PrefixState{};
    if (executePrefix(i_this, prefix) == PrefixResult::Finished) return TRUE;
    gabi::Local<DamageInfo> info;
    gabi::Local<u8[16]> outgoingLinkage;
    const auto damage = prepareDamage(i_this, info.get());
    bool bypassOrdinary = damage.defenceHit;
    int reaction = 0;
    if (!bypassOrdinary && damage.hitPart != 0) {
        if (static_cast<u32>(info->hitObject) == 0) bypassOrdinary = true;
        else {
            bypassOrdinary = applyIceOrShatter(i_this, info->hitObject, damage.hitPart);
            if (!bypassOrdinary) bypassOrdinary = applyArmoredHit(i_this, info.get(), damage);
            if (!bypassOrdinary) reaction = applyOrdinaryDamage(i_this, info.get(), damage);
        }
    }
    if (!bypassOrdinary) updateKnockback(i_this, info.get(), reaction);
    const u32 damagePower = bypassOrdinary ? 0 : info->power;
    Tn_move(i_this);
    updateDemoCameraAndSmoke(i_this);
    updateDamageAnimation(i_this, damagePower);
    updateCollisionShapes(i_this, prefix.playerAddress);
    updateGroundTilt(i_this);
    updateHeldWeapon(i_this);
    finishNormalEffects(i_this);
    return TRUE;
}

/* 024C018C */
BOOL daTn_IsDelete(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return 1;
}

/* 024C0194 */
BOOL daTn_Delete(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_resDelete(&i_this->mPhaseTkwn, STR(0x1004069C));
    dComIfG_resDelete(&i_this->mPhaseTn, STR(0x100406A4));
    if (i_this->m1433 != 0) {
        const s8 hio = static_cast<s8>(l_tnHIO.mNo);
        hio_set = 0;
        tn_abi::mDoHIO_deleteChild(hio);
    }
    const u32 base = gabi::ea(i_this);
    tn_abi::cutTurnEnd(reinterpret_cast<u8*>(&i_this->m144C));
    tn_abi::cutTurnEnd(reinterpret_cast<u8*>(&i_this->m144C[1]));
    tn_abi::cutTurnEnd(reinterpret_cast<u8*>(&i_this->m144C[2]));
    tn_removeParticleCallback(base + 0x550);
    tn_removeParticleCallback(base + 0x570);
    tn_removeParticleCallback(base + 0xD38);
    if (i_this->heap != nullptr) {
        mDoExt_McaMorf* model = i_this->mpBodyMorf;
        model->stopZelAnime();
    }
    const u32 mantleId = i_this->mMantPcId;
    if (mantleId != 0xFFFFu) {
        gabi::Local<be<u32>> searchId;
        *searchId = mantleId;
        fopAc_ac_c* mantle = nullptr;
        if (mantleId != 0xFFFFFFFFu) {
            mantle = tn_abi::actorJudge(gabi::at<void>(TN_ACTOR_ID_JUDGE), searchId.get());
        }
        if (mantle != nullptr) fopAcM_delete(mantle);
    }
    if (i_this->health == -128 && (static_cast<u32>(i_this->actor_status) & 0x04000000u) != 0) {
        if (static_cast<s8>(s_check) == 0) tn_abi::mDoAud_subBgmStop();
        else s_check = static_cast<u8>(0);
    }
    tn_abi::enemy_fire_remove(&i_this->mEnemyFire);
    return 1;
}

/* 024C02FC */
int useHeapInit(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const u32 base = gabi::ea(i_this);
    J3DModelData* bodyData = static_cast<J3DModelData*>(
        dComIfG_getObjectRes(STR(0x100406A7), 0x6C, TN_SAFESTRING_VTABLE));
    J3DAnmTransform* bodyAnimation = static_cast<J3DAnmTransform*>(
        dComIfG_getObjectRes(STR(0x100406A7), 0x39, TN_SAFESTRING_VTABLE));
    const f32 speed = 1.0f;
    i_this->mpBodyMorf = mDoExt_McaMorf::create(nullptr, bodyData, nullptr, nullptr, bodyAnimation,
                                             2, speed, 0, -1, 1, nullptr, 0, 0x11020203);
    J3DModel* bodyModel = i_this->mpBodyMorf->getModel();
    if (bodyModel == nullptr) {
        tn_abi::assertCondition(STR(0x100406AC), 0x1A9B, STR(0x100406B8));
    }
    bodyModel->mUserArea = base;
    for (u32 joint = 0; joint < 33; ++joint) {
        const s8 slot = static_cast<s8>(joint_check.m[joint]);
        u32 callback = 0;
        if (slot >= 0) {
            callback = (slot >= 14 && slot <= 17) || slot == 20 ? gabi::guest_fn((const void*)&nodeCallBack_P) : gabi::guest_fn((const void*)&nodeCallBack);
        } else if (joint == 30 || joint == 32) callback = gabi::guest_fn((const void*)&nodeCallBack_mimi);
        if (callback != 0) *tn_jointCallbackAddress(bodyModel, joint) = callback;
    }
    i_this->mpShieldMorf = tn_createResourceMorf(0x6C, 0x3C, speed);
    J3DModel* shieldModel = i_this->mpShieldMorf->getModel();
    if (shieldModel == nullptr) return 0;
    shieldModel->mUserArea = base;
    *tn_jointCallbackAddress(shieldModel, 10) = gabi::guest_fn((const void*)&nodeCallBack_kata);
    i_this->mpBrkAnm = tn_createBrkAnimation();
    if (i_this->mpBrkAnm == nullptr) return 0;
    bodyModel = i_this->mpBodyMorf->getModel();
    if (tn_initBrkAnimation(&i_this->mpBrkAnm, bodyModel, 0x73, 2, speed) == 0) return 0;
    for (u32 part = 0; part < 3; ++part) {
        J3DModel* model;
        if (part == 0) {
            i_this->mpArmorMorf = tn_createResourceMorf(0x6E, 0x64, speed);
            model = i_this->mpArmorMorf->getModel();
        } else {
            const bool alternateWeapon = part == 1 && (static_cast<u8>(i_this->mEquipmentType) & 1u) != 0;
            const u32 index = alternateWeapon ? 0x6Bu : static_cast<u16>(part_bmd.m[part]);
            J3DModelData* data = static_cast<J3DModelData*>(
                dComIfG_getObjectRes(STR(0x100406A7), index, TN_SAFESTRING_VTABLE));
            model = mDoExt_J3DModel__create(data, 0, 0x11020203);
        }
        i_this->mParts[part].mpPartModel = model;
        if (model == nullptr) return 0;
        i_this->mParts[part].mpPartBrkAnm = tn_createBrkAnimation();
        if (i_this->mParts[part].mpPartBrkAnm == nullptr) return 0;
        const bool alternateWeapon = part == 1 && (static_cast<u8>(i_this->mEquipmentType) & 1u) != 0;
        const u32 index = alternateWeapon ? 0x72u : static_cast<u16>(part_brk.m[part]);
        model = i_this->mParts[part].mpPartModel;
        if (tn_initBrkAnimation(&i_this->mParts[part].mpPartBrkAnm, model, index, 0, speed) == 0) return 0;
    }
    bodyModel = i_this->mpBodyMorf->getModel();
    const u32 jointHit = tn_abi::JntHit_create(bodyModel, gabi::at<u8>(0x101D257C), 10);
    i_this->mpJntHit = gabi::at<void>(jointHit);
    if (jointHit == 0) return 0;
    i_this->jntHit = static_cast<u32>(jointHit);
    return 4;
}

/* 024C0B0C */
int daTn_Create(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 constructionFlags = static_cast<u32>(i_this->actor_condition);
    if ((constructionFlags & 8u) == 0) {
        if (i_this != nullptr) {
            tn_class_construct(i_this);
            constructionFlags = static_cast<u32>(i_this->actor_condition);
        }
        i_this->actor_condition = static_cast<u32>(constructionFlags | 8u);
    }
    int phase = dComIfG_resLoad(&i_this->mPhaseTkwn, STR(0x100406E8));
    if (phase != 4) return phase;
    phase = dComIfG_resLoad(&i_this->mPhaseTn, STR(0x100406F0));
    if (phase != 4) return phase;
    i_this->mSmokeCb2.m12 = 1;
    i_this->gbaName = static_cast<u8>(3);
    bool specialStage = tn_stageEquals(0x100406D8);
    if (!specialStage) specialStage = tn_stageEquals(0x100406F4);
    if (!specialStage) specialStage = tn_stageEquals(0x100406FC);
    search_sp = static_cast<u8>(specialStage ? 1 : 0);
    i_this->mBehaviorType = static_cast<u32>(i_this->mParameters) & 0xFu;
    i_this->mArmorColorIndex = static_cast<u32>(i_this->mParameters) >> 4 & 0xFu;
    i_this->mRangeOrFrozenAnim = static_cast<u8>(static_cast<u32>(i_this->mParameters) >> 8);
    i_this->mPathIndex = static_cast<u32>(i_this->mParameters) >> 16;
    i_this->mEnableSpawnSwitch = static_cast<u8>(static_cast<u32>(i_this->mParameters) >> 24);
    const u8 deathSwitch = static_cast<u8>(static_cast<s16>(i_this->current.angle.z));
    i_this->mDisableSpawnOnDeathSwitch = static_cast<u8>(deathSwitch);
    if (REG0_S(0) != 0 || (deathSwitch > 0 && deathSwitch <= 0x7F)) {
        i_this->actor_status = static_cast<u32>(static_cast<u32>(i_this->actor_status) | 0x04000000u);
        search_sp = static_cast<u8>(1);
    }
    const u8 spawnSwitch = static_cast<u8>(i_this->mEnableSpawnSwitch);
    if (spawnSwitch != 0xFF) {
        i_this->m02C1 = static_cast<u8>(spawnSwitch + 1u);
        i_this->actor_status = static_cast<u32>(static_cast<u32>(i_this->actor_status) | 0x4000u);
        i_this->mStatusTimer = static_cast<u8>(10);
    }
    i_this->mEquipmentType = static_cast<u16>(i_this->current.angle.x) >> 5 & 7u;
    i_this->current.angle.z = 0;
    i_this->current.angle.x = 0;
    if (i_this->mArmorColorIndex > 5) i_this->mArmorColorIndex = 5;
    u8 equipment = i_this->mEquipmentType;
    if (equipment > 5) { equipment = 5; i_this->mEquipmentType = equipment; }
    i_this->mRemainingEquipmentPieces = equipment < 2 ? 0xB : 0xF;
    u8 switchNumber = static_cast<u8>(i_this->mDisableSpawnOnDeathSwitch);
    if (switchNumber == 0xFF) {
        i_this->mDisableSpawnOnDeathSwitch = static_cast<u8>(0);
    } else if (switchNumber != 0) {
        const u32 save = dComIfGs_ea();
        if (tn_abi::saveIsSwitch(gabi::at<u8>(save + 0x20u), switchNumber, static_cast<s8>(i_this->current.roomNo)) != 0) return 5;
    }
    s_check = static_cast<u8>(0);
    u8 behavior = i_this->mBehaviorType;
    const f32 initialGroundHeight = 1000.0f;
    if (behavior == 13 || behavior == 14) {
        i_this->actor_status = static_cast<u32>(static_cast<u32>(i_this->actor_status) | 0x04000000u);
        search_sp = static_cast<u8>(1);
        if (i_this->mBehaviorType == BEHAVIOR_MINIBOSS) i_this->mDamageReaction.mMode = 0;
        else {
            i_this->mDamageReaction.mMode = 10;
            const f32 y = i_this->current.pos.y;
            i_this->actor_status = static_cast<u32>(static_cast<u32>(i_this->actor_status) | 0x4000u);
            i_this->current.pos.y = y + initialGroundHeight;
        }
        i_this->m02C1 = static_cast<u8>(0);
        i_this->mBehaviorType = BEHAVIOR_ROAMS_AROUND;
        i_this->mDamageReaction.mAction = ACTION_S_DEMO;
        behavior = 0;
    } else if (behavior == 12) {
        i_this->mBehaviorType = BEHAVIOR_ROAMS_AROUND;
        i_this->mDamageReaction.mMode = 20;
        i_this->mDamageReaction.mAction = ACTION_S_DEMO;
        behavior = 0;
    }
    if (behavior == 4) i_this->mDamageReaction.mAction = ACTION_STAND;
    else if (behavior == 15) {
        i_this->m02C1 = static_cast<u8>(0);
        i_this->mDamageReaction.mAction = ACTION_D_DOZOU;
    }
    const u32 play = gabi::ea(dComIfGp_get());
    const u32 sound = tn_abi::characterNameIndex(gabi::at<u8>(play + 0x50ACu), STR(0x100406F0), 0);
    i_this->itemTableIdx = static_cast<u32>(sound);
    if (tn_abi::entrySolidHeap(i_this, gabi::guest_fn((const void*)&useHeapInit), 0x29808) == 0) return 5;
    if (static_cast<u8>(hio_set) == 0) {
        const u8 hio = tn_abi::mDoHIO_createChild(STR(0x1004070C), reinterpret_cast<u8*>(&l_tnHIO.mNo));
        l_tnHIO.mNo = static_cast<u8>(hio);
        i_this->m1433 = 1;
        hio_set = 1;
    }
    const f32 minY = -50.0f;
    const f32 minZ = -100.0f;
    const f32 minX = -200.0f;
    tn_abi::fopAcM_SetMin(i_this, minX, minY, minZ);
    const f32 maxX = 125.0f;
    const f32 maxYZ = 250.0f;
    tn_abi::fopAcM_SetMax(i_this, maxX, maxYZ, maxYZ);
    const u32 bodyModel = gabi::ea(i_this->mpBodyMorf->getModel());
    const f32 culling = 220.5f;
    i_this->mBtBodyR = static_cast<f32>(maxX);
    i_this->mBtHeight = static_cast<f32>(culling);
    i_this->cullMtx = bodyModel == 0 ? 0 : bodyModel + 0xC8u;
    i_this->mDamageReaction.m70C = static_cast<u8>(1);
    const f32 y = i_this->current.pos.y;
    i_this->mDamageReaction.mMaxFallDistance = static_cast<f32>(initialGroundHeight);
    i_this->mDamageReaction.mSpawnY = static_cast<f32>(y);
    i_this->mDamageReaction.mInvincibleTimer = static_cast<s16>(5);
    i_this->attention_info.flags = static_cast<u32>(4);
    if (i_this->mPathIndex != 0xFF) {
        i_this->ppd = tn_abi::dPath_GetRoomPath(static_cast<u8>(i_this->mPathIndex), static_cast<s8>(i_this->current.roomNo));
        if (i_this->ppd == nullptr) return 5;
        const u8 pathIndex = i_this->mPathIndex;
        i_this->m1426 = 1;
        i_this->m1424 = static_cast<u8>(pathIndex + 1u);
    }
    tn_abi::dBgS_Acch_Set(reinterpret_cast<u8*>(&i_this->mDamageReaction.mAcch), &i_this->current.pos,
               reinterpret_cast<cXyz*>(&i_this->old.pos.x), i_this, 1, reinterpret_cast<u8*>(&i_this->mDamageReaction.mAcchCir),
               reinterpret_cast<cXyz*>(&i_this->speed.x), 0, 0);
    const f32 wallRadius = 50.0f;
    tn_abi::dBgS_AcchCir_SetWall(reinterpret_cast<u8*>(&i_this->mDamageReaction.mAcchCir), wallRadius, wallRadius);
    const u32 acchFlags = static_cast<u32>(i_this->mDamageReaction.mAcch.m_flags);
    const f32 wallHeight = 100.0f;
    const u8 armorType = i_this->mEquipmentType;
    i_this->mDamageReaction.mAcch.m_flags = static_cast<u32>(acchFlags & ~8u);
    i_this->mDamageReaction.mAcch.m_roof_crr_height = static_cast<f32>(wallHeight);
    tn_initializeColliders(i_this, armorType);
    i_this->m02DC = 3;
    gabi::Local<cXyz> weaponPosition;
    const f32 weaponHeight = REG0_F(12);
    const f32 actorY = i_this->current.pos.y;
    weaponPosition->x = i_this->current.pos.x;
    weaponPosition->y = actorY + weaponHeight;
    const s8 room = i_this->current.roomNo;
    weaponPosition->z = i_this->current.pos.z;
    i_this->mBokoPcId = fopAcM_create(0x1CF, 3, weaponPosition.get(), room, nullptr, nullptr, -1, 0);
    i_this->m0C34 = 1;
    i_this->m140C = static_cast<u8>(1);
    u32 mantle = 0xFFFF;
    if (i_this->mEquipmentType >= EQUIPMENT_SHIELD_AND_CAPE) mantle = fopAcM_create(0xC0, 0, &i_this->current.pos, i_this->current.roomNo, nullptr, nullptr, -1, 0);
    i_this->mEnemyIce.mpActor = i_this;
    i_this->mMantPcId = mantle;
    const f32 iceBase = 70.0f;
    const f32 iceTuning = REG0_F(6);
    const f32 iceSize = 1.5f;
    const f32 iceRadius = 200.0f;
    const u8 itemSwitch = static_cast<u8>(i_this->mDisableSpawnOnDeathSwitch);
    i_this->mEnemyIce.mWallRadius = static_cast<f32>(iceTuning + iceBase);
    const f32 iceHeight = REG0_F(7);
    i_this->mEnemyIce.mDeathSwitch = static_cast<u8>(itemSwitch);
    i_this->mEnemyIce.mParticleScale = static_cast<f32>(iceSize);
    const u32 body = gabi::ea(static_cast<mDoExt_McaMorf*>(i_this->mpBodyMorf));
    i_this->mEnemyFire.mpActor = i_this;
    i_this->mEnemyFire.mpMcaMorf = gabi::at<mDoExt_McaMorf>(body);
    i_this->mEnemyIce.mCylHeight = static_cast<f32>(iceHeight + iceRadius);
    for (u32 i = 0; i < 10; ++i) {
        const u8 joint = static_cast<u8>(fire_j.m[i]);
        i_this->mEnemyFire.mFlameJntIdxs[i] = static_cast<u8>(joint);
        const f32 radius = static_cast<f32>(fire_sc.m[i]);
        i_this->mEnemyFire.mParticleScale[i] = radius;
    }
    i_this->m151C = static_cast<f32>(1.0f);
    i_this->stealItemLeft = 5;
    for (int i = 0; i < 3; ++i) daTn_Execute(i_this);
    const s16 action = i_this->mDamageReaction.mAction;
    i_this->m151C = static_cast<f32>(10.0f);
    if (action == ACTION_D_DOZOU) {
        const u32 save = dComIfGs_ea();
        const BOOL set = tn_abi::saveIsEventBit(gabi::at<u8>(save + 0x644u), 0x3802);
        const u32 status = static_cast<u32>(i_this->actor_status);
        i_this->actor_status = static_cast<u32>(set != 0 ? status & ~0x4000u : status | 0x4000u);
    }
    bool hide = false;
    if (tn_stageEquals(0x100406E0) && i_this->current.roomNo == 17) hide = true;
    else if (tn_stageEquals(0x10040704)) hide = true;
    if (hide) {
        i_this->actor_status = static_cast<u32>(static_cast<u32>(i_this->actor_status) | 0x4000u);
        i_this->mStatusTimer = static_cast<u8>(2);
    }
    return 4;
}

// HD: compiler-generated and template code of this translation unit.

// HD: emits this small bomb-search/state update separately from the main action.
/* 024BAF44 */
BOOL daTn_checkNearbyBomb(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if ((static_cast<u8>(i_this->mRemainingEquipmentPieces) & EQUIPMENT_PIECE_BODY_ARMOR) == 0) {
        fopAc_ac_c* bomb = search_bomb(i_this, 1);
        i_this->m1418 = bomb;
        if (bomb != nullptr)
            return 1;
    }
    return 0;
}

// HD: applies gamma conversion before initializing the spin-particle colors.
/* 024BD0F0 */
f32 daTn_colorPower(u32 channel) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    const f32 normalized = static_cast<f32>(channel) / 255.0f;
    f32 value = tn_powf(normalized, 2.2f);
    if (value < 0.0f) value = 0.0f;
    else if (value > 1.0f) value = 1.0f;
    return value;
}

/* 024C0898 */
tn_enemyfire* tnEnemyFire_construct(tn_enemyfire* fire) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (fire == nullptr) {
        fire = static_cast<tn_enemyfire*>(operator_new(0x22C));
        if (fire == nullptr) return nullptr;
    }
    // The native inlined vector constructor retains its null-subobject path.
    if (gabi::ea(fire) + 0x8Cu == 0) (void)operator_new(12);
    tn_abi::cCcD_Stts_construct(&fire->mStts);
    tn_abi::dCcD_GStts_construct(reinterpret_cast<u8*>(&fire->mStts.__vtbl_gstts));
    fire->mStts.__vtbl = 0x1004AE88;
    fire->mStts.__vtbl_gstts = 0x1004AEC0;
    tn_abi::dCcD_Sph_construct(&fire->mSph);
    fire->mLightExpansion = 1.0f;
    return fire;
}

/* 024C0924 */
tn_class* tn_class_construct(tn_class* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this == nullptr) {
        i_this = static_cast<tn_class*>(operator_new(0x1C5C));
        if (i_this == nullptr) return nullptr;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = 0x10040490;
    tn_abi::smokeConstruct(&i_this->mSmokeCb1, 1);
    tn_abi::smokeConstruct(&i_this->mSmokeCb2, 1);

    // Damage-reaction background and collision status subobjects.
    tn_abi::dBgS_AcchCir_construct(reinterpret_cast<u8*>(&i_this->mDamageReaction.mAcchCir));
    tn_abi::dBgS_Acch_construct(reinterpret_cast<u8*>(&i_this->mDamageReaction.mAcch));
    i_this->mDamageReaction.mAcch.cBgS_Chk::__vtbl = 0x10040410;
    i_this->mDamageReaction.mAcch.dBgS_PolyPassChk::__vtbl = 0x10040430;
    i_this->mDamageReaction.mAcch.mbObjThrough = 1;
    i_this->mDamageReaction.mAcch.dBgS_GrpPassChk::__vtbl = 0x10040420;
    tn_abi::cCcD_Stts_construct(reinterpret_cast<u8*>(&i_this->mDamageReaction.mStts));
    tn_abi::dCcD_GStts_construct(reinterpret_cast<u8*>(&reinterpret_cast<dCcD_Stts*>(&i_this->mDamageReaction.mStts)->__vtbl_gstts));
    reinterpret_cast<dCcD_Stts*>(&i_this->mDamageReaction.mStts)->__vtbl = 0x1004AE88;
    reinterpret_cast<dCcD_Stts*>(&i_this->mDamageReaction.mStts)->__vtbl_gstts = 0x1004AEC0;
    tn_abi::smokeConstruct(reinterpret_cast<u8*>(&i_this->mDamageReaction.mParticleCallBack), 1);

    tn_constructCylinder(&i_this->mCoCyl);
    tn_constructCylinder(&i_this->mTgCyl);
    tn_abi::dCcD_Sph_construct(&i_this->mHeadSph);
    tn_abi::dCcD_Sph_construct(&i_this->mDefenceSph);
    tn_abi::dCcD_Sph_construct(&i_this->mWeponSph);
    tn_abi::dCcD_Sph_construct(&i_this->mWepon2Sph);
    tn_abi::constructArray(reinterpret_cast<u8*>(&i_this->m144C), 3, 0x10,
               gabi::at<void>(gabi::guest_fn((const void*)&tnCutTurn_construct)));

    // Ice status/cylinder and background checks; fire is a separate constructor.
    tn_abi::cCcD_Stts_construct(reinterpret_cast<u8*>(&i_this->mEnemyIce.mStts));
    tn_abi::dCcD_GStts_construct(reinterpret_cast<u8*>(&i_this->mEnemyIce.mStts.__vtbl_gstts));
    i_this->mEnemyIce.mStts.__vtbl = 0x1004AE88;
    i_this->mEnemyIce.mStts.__vtbl_gstts = 0x1004AEC0;
    tn_constructCylinder(&i_this->mEnemyIce.mCyl);
    tn_abi::dBgS_AcchCir_construct(reinterpret_cast<u8*>(&i_this->mEnemyIce.mBgAcchCir));
    tn_abi::dBgS_Acch_construct(reinterpret_cast<u8*>(&i_this->mEnemyIce.mBgAcch));
    i_this->mEnemyIce.mBgAcch.mbObjThrough = 1;
    i_this->mEnemyIce.mBgAcch.dBgS_GrpPassChk::__vtbl = 0x10040420;
    i_this->mEnemyIce.mBgAcch.dBgS_PolyPassChk::__vtbl = 0x10040430;
    i_this->mEnemyIce.mBgAcch.cBgS_Chk::__vtbl = 0x10040410;
    tnEnemyFire_construct(&i_this->mEnemyFire);
    return i_this;
}

/* 024C161C */
void tn_staticInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    // Register the native static objects and initialize the random-angle range.
    l_staticStates.m[2] = 0;
    l_staticStates.m[0] = 0;
    l_tnHIO.__vtbl = 0;
    l_staticStates.m[1] = 0;
    tn_abi::registerGlobalObject(reinterpret_cast<u8*>(&l_globalObject0));
    l_randomAngleRange.min = -3.1415927f;
    l_randomAngleRange.max = 3.1415927f;
    tn_abi::initializeMathState(reinterpret_cast<u8*>(&l_mathInitialized));
    tn_abi::registerGlobalObject(reinterpret_cast<u8*>(&l_globalObject1));
    tn_abi::initializeRandomState(reinterpret_cast<u8*>(&l_randomInitialized));
    tn_abi::registerGlobalObject(reinterpret_cast<u8*>(&l_globalObject2));

    l_tnHIOVtable = 0x10040480;
    tn_abi::JntHit_HIO_construct(l_tnHIO.mJntHitHIO);
    l_tnHIO.m005 = 0;
    l_tnHIO.m01E = 0xB9;
    l_tnHIO.m020 = 12;
    l_tnHIO.m014 = 1.0f;
    l_tnHIO.m018 = 25.0f;
    l_tnHIO.m01C = 0;

    l_tnHIO.m00C = 1.0f;
    l_tnHIO.m024 = 50.0f;
    l_tnHIO.m028 = 1000.0f;
    l_tnHIO.m02C = 400.0f;
    l_tnHIO.m030 = 250.0f;
    l_tnHIO.m038 = 400.0f;
    l_tnHIO.m03C = 500.0f;
    l_tnHIO.m040 = 130.0f;
    l_tnHIO.m044 = 500.0f;
    l_tnHIO.m048 = -125.0f;
    l_tnHIO.m04C = 20.0f;
    l_tnHIO.m006 = 0;
    l_tnHIO.m007 = 0;
    l_tnHIO.m050 = 20.0f;
    l_tnHIO.m00A = 50;
    l_tnHIO.m054 = 70.0f;
    l_tnHIO.m034 = 23000;
    l_tnHIO.m058 = 90.0f;
    l_tnHIO.m05C = 10.0f;
    l_tnHIO.m036 = 11000;
    l_tnHIO.m06C = 30;
    l_tnHIO.m080 = 30;
    l_tnHIO.m082 = 300;
    l_tnHIO.m060 = 10.0f;
    l_tnHIO.m064 = 70.0f;
    l_tnHIO.m068 = 1.0f;
    l_tnHIO.m070 = 75.0f;
    l_tnHIO.m008 = 0;
    l_tnHIO.m074 = 40.0f;
    l_tnHIO.m078 = 40.0f;
    l_tnHIO.m07C = 70.0f;
    l_tnHIO.m084 = 0.9f;
    l_tnHIO.m088 = 1.0f;
    l_tnHIO.m08C = 1.0f;
    l_tnHIO.m090 = 1.0f;
    l_tnHIO.m094 = 1.0f;
    l_tnHIO.m098 = 1.0f;
    l_tnHIO.m09C = 1.1f;
    l_tnHIO.m0A0 = 1.0f;
    l_tnHIO.m0A4 = 1.0f;
    l_tnHIO.m0A8 = 0.5f;
    l_tnHIO.m0AC = 1.0f;
    l_tnHIO.m0B0 = 1.2f;
    l_tnHIO.m0B4 = 1.0f;
    l_tnHIO.m0B8 = 1.0f;
    l_tnHIO.m0C4 = 14;
    l_tnHIO.m0C6 = 24;
    l_tnHIO.m0C8 = 22;
    l_tnHIO.m0CA = 5;
    l_tnHIO.m0CC = 100;
    l_tnHIO.m0BC = 1.0f;
    l_tnHIO.m0C0 = 1.0f;
    l_tnHIO.m0D0 = 0.0f;
    l_tnHIO.m0D4 = 500.0f;
    l_tnHIO.m0CE = 28;
}

/* 024C18A4 */
void tnSafeString_destroy(void* string, int flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (string != nullptr && (static_cast<u32>(flags) & 1) != 0) {
        operator_delete(string);
    }
}

/* 024C6BAC */
void* tnCutTurn_construct(void* callback) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (callback == nullptr) {
        callback = operator_new(0x10);
        if (callback == nullptr) return nullptr;
    }
    reinterpret_cast<TnCutTurnCallback*>(callback)->__vtbl = TN_CUTTURN_VTABLE;
    return callback;
}

/* 024C6BEC */
void tnCutTurn_destroy(void* callback, int flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (callback != nullptr && (static_cast<u32>(flags) & 1) != 0) {
        operator_delete(callback);
    }
}

/* 024C6C00 */
void tn_class_destroy(tn_class* i_this, int flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this == nullptr) return;

    // Embedded fire and ice collision objects precede their background checks.
    tn_abi::dCcD_Sph_destroy(reinterpret_cast<u8*>(&i_this->mEnemyFire.mSph), 2);
    tn_abi::dCcD_Stts_destroy(reinterpret_cast<u8*>(&i_this->mEnemyFire.mStts), 2);
    i_this->mEnemyIce.mBgAcch.dBgS_GrpPassChk::__vtbl = 0x10040420;
    i_this->mEnemyIce.mBgAcch.dBgS_PolyPassChk::__vtbl = 0x10040430;
    tn_abi::dBgS_Acch_destroy(reinterpret_cast<u8*>(&i_this->mEnemyIce.mBgAcch), 0);
    tn_abi::cM3dGCir_destroy(reinterpret_cast<u8*>(&i_this->mEnemyIce.mBgAcchCir.m_cir), 2);
    tn_abi::dCcD_Cyl_destroy(reinterpret_cast<u8*>(&i_this->mEnemyIce.mCyl), 2);
    tn_abi::dCcD_Stts_destroy(reinterpret_cast<u8*>(&i_this->mEnemyIce.mStts), 2);

    // Native array cleanup: three 0x10-byte cut-turn callbacks.
    tn_abi::destroyArray(reinterpret_cast<u8*>(&i_this->m144C), 3, 0x10,
               gabi::at<void>(gabi::guest_fn((const void*)&tnCutTurn_destroy)), 0, 0);
    tn_abi::dCcD_Sph_destroy(&i_this->mWepon2Sph, 2);
    tn_abi::dCcD_Sph_destroy(&i_this->mWeponSph, 2);
    tn_abi::dCcD_Sph_destroy(&i_this->mDefenceSph, 2);
    tn_abi::dCcD_Sph_destroy(&i_this->mHeadSph, 2);
    tn_abi::dCcD_Cyl_destroy(&i_this->mTgCyl, 2);
    tn_abi::dCcD_Cyl_destroy(&i_this->mCoCyl, 2);

    // Damage-reaction status/background subobjects and the i_this base.
    tn_abi::dCcD_Stts_destroy(reinterpret_cast<u8*>(&i_this->mDamageReaction.mStts), 2);
    i_this->mDamageReaction.mAcch.dBgS_GrpPassChk::__vtbl = 0x10040420;
    i_this->mDamageReaction.mAcch.dBgS_PolyPassChk::__vtbl = 0x10040430;
    tn_abi::dBgS_Acch_destroy(reinterpret_cast<u8*>(&i_this->mDamageReaction.mAcch), 0);
    tn_abi::cM3dGCir_destroy(reinterpret_cast<u8*>(&i_this->mDamageReaction.mAcchCir.m_cir), 2);
    tn_abi::fopAc_ac_c_destroy(i_this, 0);
    if ((static_cast<u32>(flags) & 1) != 0) {
        operator_delete(i_this);
    }
}

/* 024C6D58 */
void tnSafeString_noop(void* string) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* WWHD glue
data part_bmd 101D2678
data part_brk 101D2680
data j_dt 101D254C
data fire_j 101D27B0
data fire_sc 101D2788
data l_spinColors 101D2448
data l_spinColorInitialized 1046E724
data spin_eff_name 101D2460
data l_staticStates 1046E768
data l_randomAngleRange 1046E71C
data l_globalObject0 101D2844
data l_globalObject1 101D2850
data l_globalObject2 101D285C
data l_mathInitialized 1046E73E
data l_randomInitialized 1046E73F
data hio_set 101D2468
data l_pauseTimer 101EACB7
data l_tnHIO 1046E774
data l_tnHIOVtable 1046E878
data s_check 1046E73D
data target_info_count 1046E718
data target_info 1046E740
data search_sp 1046E73C
data joint_check 101D2558
data check_index 1046E87C
data xad 101D25F8
data zad 101D2608
data check_bit 101D25F4
data atp_0 101D2618
data atp_1 101D2630
data prim 101D2648
data env 101D2660
data l_currentModel 104B462C
data l_currentMtx 104B4868
data l_smokeDynamicsScale 1047B65C
data l_smokeParticleScale 1047B63C
data l_weaponRadius 1047B644
data l_weapon2Radius 1047B648
data l_hideShape 1047B68A
str 100404E8 "Tn" in 024B91E4
str 100404EB "Tn" in 024B931C
str 100404EE "Tn" in 024B93E8
str 10040564 "d_a_tn.cpp" in 024B9C1C
str 10040570 "no2 >= 0 && no2 < 33" in 024B9C1C
str 10040588 "no >= 0 && no < 20+1" in 024B9C1C
str 1004053C "no >= 0 && no < 20+1" in 024B97A0
str 10040518 "d_a_tn.cpp" in 024B97A0
str 10040524 "no2 >= 0 && no2 < 33" in 024B97A0
*/
