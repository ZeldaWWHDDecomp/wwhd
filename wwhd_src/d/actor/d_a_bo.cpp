/**
 * d_a_bo.cpp (WWHD) - Enemy: Boko Baba.
 * Verified HD behavior; GameCube names follow zeldaret/tww.
 */
#include "d/actor/d_a_bo.h"

static BOOL nodeCallBack_UP(J3DNode *node, s32 stage);
static BOOL daBO_Execute(bo_class *actor);

static const u16 l_attackTypes[32] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                                      11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                      22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
static const u8 l_attackReactions[32] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
                                         1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1};
static const s8 l_fireJoints[10] = {1, 3, 5, 6, 8, 9, 10, 11, 12, 13};
static const f32 l_fireScales[10] = {1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f};

static f32 l_minAngle;
static f32 l_maxAngle;
static u32 l_cameraEyeInitialized;
static u32 l_cameraCenterInitialized;
static u8 l_headerByte;
static u8 l_headerFlag;
static u32 l_headerWord0;
static u32 l_headerWord1;
static u32 l_headerWord2;
static u32 l_headerWord3;
static cXyz l_cameraEye;
static cXyz l_cameraCenter;

static u32 l_currentModel;
static u32 l_matrixStack;
static s16 l_disableDraw;
static f32 l_initialWaitExtra;
static s16 l_mouthYawAdjust;
static s16 l_mouthPitchAdjust;
static s16 l_budPitchAdjust;
static s16 l_budYawAdjust;
static f32 l_budHeightExtra;
static u8 l_enemyDeathEffect;
static f32 l_invincibilityExtra;
static s16 l_deathDelayExtra;
static f32 l_fireDurationExtra;
static f32 l_iceDurationExtra;
static f32 l_disableRecoil;
static u32 l_saveInfo;
static Mtx34 l_baseMatrix;
static Mtx34 l_currentMatrix;
static dCcD_SrcSph head_co_sph_src;
static dCcD_SrcSph foot_co_sph_src;
static dCcD_SrcCyl body_cyl_src;

static u32 l_collisionStatusVtable;
static u32 l_globalCollisionStatusVtable;
static u32 l_actorVtable;
static u32 l_salivaCallbackVtable;
static u32 l_groundPolyInfoVtable;
static u32 l_groundCheckVtable;
static u32 l_groundPolyPassVtable;
static u32 l_groundGroupPassVtable;
static u32 l_acchGroupPassVtable;
static u32 l_acchPolyPassVtable;
static u32 l_acchCheckVtable;
static u32 l_cylinderShapeVtable;
static u32 l_shapeBoundsVtable;
static u32 l_collisionHitInfoVtable;
static u32 l_collisionCylinderShapeVtable;
static u32 l_collisionCylinderVtable;
static u32 l_polyInfoVtable;
static u32 l_polyPassVtable;
static u32 l_groupPassVtable;
static u32 l_headerWordRegistration;
static u32 l_headerByteRegistration;
static u32 l_headerFlagRegistration;

static char l_eventRelativeActor[9] = {0x52, 0x65, 0x6c, 0x41, 0x63, 0x74, 0x6f, 0x72, 0x0};
static char l_eventStarter[9] = {0x40, 0x53, 0x54, 0x41, 0x52, 0x54, 0x45, 0x52, 0x0};
static char l_eventRelativeMask[11] = {0x52, 0x65, 0x6c, 0x55, 0x73, 0x65,
                                       0x4d, 0x61, 0x73, 0x6b, 0x0};
static char l_eventOr[5] = {0x2d, 0x2d, 0x6f, 0x72, 0x0};
static char l_eventTimer[6] = {0x54, 0x69, 0x6d, 0x65, 0x72, 0x0};
static char l_eventCenter[7] = {0x43, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x0};
static char l_eventEye[4] = {0x45, 0x79, 0x65, 0x0};
static char l_eventFovy[5] = {0x46, 0x6f, 0x76, 0x79, 0x0};
static char l_deleteArcName[3] = {0x42, 0x6f, 0x0};
static char l_createArcName[3] = {0x42, 0x6f, 0x0};
static char l_figureName[6] = {0x62, 0x62, 0x61, 0x62, 0x61, 0x0};
static s32 l_eventTimerValue = 30;
static f32 l_eventFovyValue = 75.f;

static u32 l_resourceControl;
static u32 l_safeStringVtable;
static const char l_arcName[3] = {0x42, 0x6F, 0} /* Bo */;
static const char l_heapArcName[3] = {0x42, 0x6F, 0} /* Bo */;

// Keep outgoing linkage and stack arguments below all live guest payloads.
template <class R = void, class... A> static R boCall(u32 target, A... args) {
    u32 previousSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[16]> outgoing;
    gabi::store<u32>(outgoing.a, previousSp);
    return gabi::call<R>(target, args...);
}
// HD: named bindings use the same outgoing linkage as the original call sites.
template <class F, class... A> static auto boInvoke(F function, A... args) {
    u32 previousSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[16]> outgoing;
    gabi::store<u32>(outgoing.a, previousSp);
    return function(args...);
}
template <class F, class O, class... A> static auto boMethod(F method, O *object, A... args) {
    u32 previousSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[16]> outgoing;
    gabi::store<u32>(outgoing.a, previousSp);
    return (object->*method)(args...);
}
template <class R = void, class... A> static R boVirtualCall(u32 target, A... args) {
    u32 previousSp = gabi::cpu->r[1];
    gabi::Local<be<u32>[16]> outgoing;
    gabi::store<u32>(outgoing.a, previousSp);
    return gabi::call_ptr<R>(target, args...);
}
// HD: keep the verified 64-byte outgoing linkage area on nested calls.
#undef WWHD_GLUE_FUNC
#define WWHD_GLUE_FUNC(addr, R, ...)                                                               \
    if (gabi::Activation::nested())                                                                \
        return boCall<R>(addr __VA_OPT__(, ) __VA_ARGS__);                                         \
    gabi::Activation wwhd_activation_

static boShapeAttrView_c *boShapeAttr(void *shape) { return (boShapeAttrView_c *)shape; }
static dComIfG_inf_c *boGameInfo() { return (dComIfG_inf_c *)boInvoke(dComIfGp_get); }
static u32 boPlay() { return gabi::ea(boGameInfo()); }
void *boGetResource(void *control, SafeString *name, s32 index);
static void *boResource(s32 index, const char *archive = (const char *)l_arcName) {
    gabi::Local<SafeString> name;
    name->__vtbl = gabi::ea(&l_safeStringVtable);
    name->mStringTop = gabi::guest_addr(archive);
    u32 controller = l_resourceControl;
    return boInvoke(boGetResource, gabi::at<void>(controller), name.get(), index);
}
static void smoke_set(bo_class *actor);
static BOOL nodeCallBack_DW(J3DNode *node, s32 stage);
static void anm_init(bo_class *actor, s32 animation, f32 blend, u8 loop, f32 speed, s32 secondary,
                     s32 lower);
static void angle_initial(bo_class *actor);
static void wait_initial(bo_class *actor);
static BOOL daBO_IsDelete(bo_class *actor);
static BOOL daBO_Delete(bo_class *actor);
static void yodare_execute(void *callback, void *emitter, void *particle);
static u32 particle_list_count(void *list);
static void callback_empty0(void *p);
static void callback_empty1(void *p);
static void callback_empty2(void *p);
static void empty_virtual(void *p);
static void emitter_set_flags(void *emitter, u32 flags);
static void trivial_destructor(void *object, u32 flags);
static void *identity_pointer(void *p);
static void bo_static_init();
static void *enemy_fire_constructor(void *object);
static bo_class *actor_constructor(bo_class *actor);
static void actor_destructor(bo_class *actor, u32 flags);
static void copy_base_matrix(u32 model);
static void draw_SUB(bo_class *actor);
static BOOL daBO_Draw(bo_class *actor);
static void monster_sound(bo_class *actor, u32 sound);
static void effect_sound(bo_class *actor, u32 sound, u32 flags);
static void nokezori_damage_rtn(bo_class *actor);
static BOOL useHeapInit(bo_class *actor);
static s32 daBO_Create(bo_class *actor);
static void upper_joint_commit(u32 model, u16 index);
static void upper_joint_position(cXyz *position);
static void upper_joint_rotate(void (*rotate)(Mtx34 *, s16), const be<s16> *field, f32 factor);
static BOOL nodeCallBack_UP(J3DNode *node, s32 stage);
static void bo4_move(bo_class *actor);
static void bo5_move(bo_class *actor);
static void boParticle(u32 effect, cXyz *position, csXyz *angle = nullptr, cXyz *scale = nullptr);
static void boDamageFinish(bo_class *actor);
static BOOL damage_check(bo_class *actor);
static void boEmitterMatrix(bo_class *actor, u32 emitter, bool identityCall);
static u32 boSpawnParticleReturn(bo_class *actor, u32 effect);
static void boFinishEmitter(bo_class *actor, gptr<void> bo_class::*emitterField,
                            be<u8> bo_class::*stateField, bool skip);
static void boRegisterBody(bo_class *actor);
static void boExecuteFinish(bo_class *actor);
static void bo3_move(bo_class *actor);
static void boApproachAngles(bo_class *actor);
static bool boAnimationEnded(bo_class *actor);
static void boStopAttack(bo_class *actor);
static void boRecoilMove(bo_class *actor, u32 player);
static void boDisableCollision(bo_class *actor);
static void boUnhide(bo_class *actor, u32 sound, bool resetAngles);
static void boWake(bo_class *actor);
static void boEmerging(bo_class *actor);
static void boWaiting(bo_class *actor);
static void boStriking(bo_class *actor, u32 companion, bool active);
static void bo2_move(bo_class *actor);
static bool boMoveMode5(bo_class *actor);
static void bo_move(bo_class *actor);
static BOOL daBO_Execute(bo_class *actor);

static void copy_base_matrix(u32 model) {
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (f32)l_baseMatrix.m[i / 4][i % 4];
    for (u32 i = 0; i < 12; ++i)
        gabi::at<J3DModel>(model)->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
}

static void monster_sound(bo_class *actor, u32 sound) {
    void *position = &actor->eyePos;
    if (!gabi::ea(position))
        return;
    u32 id = actor ? (u32)actor->mBsPcId : 0xffffffff;
    s32 room = (s8)actor->current.roomNo;
    s8 reverb = boInvoke(boReverb, room);
    boInvoke(boMonsterSound, sound, position, id, 0, (s32)reverb);
}

static void effect_sound(bo_class *actor, u32 sound, u32 flags) {
    void *position = &actor->eyePos;
    if (!gabi::ea(position))
        return;
    s32 room = (s8)actor->current.roomNo;
    s8 reverb = boInvoke(boReverb, room);
    boInvoke(boEffectSound, sound, position, flags, (s32)reverb);
}

static void upper_joint_commit(u32 model, u16 index) {
    u32 data = (u32)gabi::at<J3DModel>(model)->mpMtxBuffer.v;
    u16 flags = (u16)gabi::at<J3DMtxBuffer_hd>(data)->mFlags;
    u32 matrix = (u32)l_matrixStack;
    gabi::at<J3DMtxBuffer_hd>(data)->mFlags = flags | 0x10;
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (f32)gabi::at<Mtx34>(matrix)->m[i / 4][i % 4];
    u32 destination = (u32)gabi::at<J3DMtxBuffer_hd>(data)->mpAnmMtx + index * 0x30;
    for (u32 i = 0; i < 12; ++i)
        gabi::at<Mtx34>(destination)->m[i / 4][i % 4] = values[i];
    matrix = (u32)l_matrixStack;
    boInvoke(PSMTXCopy, gabi::at<Mtx34>(matrix), &l_currentMatrix);
}

static void upper_joint_position(cXyz *position) {
    gabi::Local<cXyz> origin;
    origin->x = 0;
    origin->y = 0;
    origin->z = 0;
    boInvoke(MtxPosition, origin.get(), position);
}

static void upper_joint_rotate(void (*rotate)(Mtx34 *, s16), const be<s16> *field, f32 factor) {
    s16 angle = *field;
    s16 scaled = (s16)gabi::ftoi((f32)angle * factor);
    u32 matrix = (u32)l_matrixStack;
    boInvoke(rotate, gabi::at<Mtx34>(matrix), (s32)scaled);
}

static void boParticle(u32 effect, cXyz *position, csXyz *angle, cXyz *scale) {
    u32 control = (u32)boGameInfo()->play.mParticle.v;
    boInvoke(boSetParticle, (dPa_control_c *)(gabi::at<void>(control)), 0, effect,
             (const cXyz *)(position), (const csXyz *)(angle), (const cXyz *)(scale), 0xff, nullptr,
             -1, nullptr, nullptr, nullptr);
}

static void boDamageFinish(bo_class *actor) {
    actor->mAction = 0;
    actor->mBendCountY = 0;
    actor->mMode = 4;
    actor->mBendCountZ = 0;
    actor->mBendFlags = 0;
    boParticle(0x8109, &actor->mTipPos);
}

// The two fading particle callbacks share the upper body's ninth joint.
static void boEmitterMatrix(bo_class *actor, u32 emitter, bool identityCall) {
    u32 morph = gabi::ea(actor->mpUpperMorph.get());
    u32 model = (u32)gabi::at<mDoExt_McaMorf>(morph)->mpModel.v;
    u32 data = (u32)gabi::at<J3DModel>(model)->mpMtxBuffer.v;
    u16 flags = (u16)gabi::at<J3DMtxBuffer_hd>(data)->mFlags;
    u32 matrices = (u32)gabi::at<J3DMtxBuffer_hd>(data)->mpAnmMtx;
    gabi::at<J3DMtxBuffer_hd>(data)->mFlags = flags | 0x10;
    void *rotation = &gabi::at<boEmitterView_c>(emitter)->mGlobalRotation;
    if (identityCall)
        rotation = identity_pointer(rotation);
    boInvoke(boSetEmitterTransform, &gabi::at<Mtx34>(matrices)[BO_UE1_JNT_J_BOKOBABA_AGO_e],
             rotation, &gabi::at<boEmitterView_c>(emitter)->mGlobalDynamicsScale,
             &gabi::at<boEmitterView_c>(emitter)->mGlobalTranslation);
}

static u32 boSpawnParticleReturn(bo_class *actor, u32 effect) {
    u32 control = (u32)boGameInfo()->play.mParticle.v;
    return gabi::ea(boInvoke(boSetParticle, (dPa_control_c *)(gabi::at<void>(control)), 0, effect,
                             (const cXyz *)(&actor->current.pos), nullptr, nullptr, 0xff, nullptr,
                             -1, nullptr, nullptr, nullptr));
}

static void boFinishEmitter(bo_class *actor, gptr<void> bo_class::*emitterField,
                            be<u8> bo_class::*stateField, bool skip) {
    u32 emitter = (actor->*emitterField).v;
    if (skip || !emitter)
        return;
    boEmitterMatrix(actor, emitter, true);
    u8 state = (actor->*stateField);
    if (state == 0) {
        emitter_set_flags(gabi::at<void>((actor->*emitterField).v), 1);
        (actor->*stateField) = (u8)((actor->*stateField) + 1);
    } else if (state == 1) {
        emitter = (actor->*emitterField).v;
        u32 first = particle_list_count(&gabi::at<boEmitterListsView_c>(emitter)->mAlivePtcl);
        u32 second = particle_list_count(&gabi::at<boEmitterListsView_c>(emitter)->mAliveChildPtcl);
        if (first + second == 0) {
            gabi::at<boEmitterView_c>(emitter)->mpEmtrCallBack.v = 0;
            u32 current = (actor->*emitterField).v;
            gabi::at<boEmitterView_c>(current)->mMaxFrame = -1;
            emitter_set_flags(gabi::at<void>(current), 1);
            (actor->*emitterField).v = 0;
            (actor->*stateField) = 0;
        }
    }
}

static void boRegisterBody(bo_class *actor) {
    boMethod(&cM3dGSph::SetC, &actor->mHeadSph.mSph, &actor->mHeadPos);
    boMethod(&cM3dGSph::SetR, &actor->mHeadSph.mSph, 60.f);
    boInvoke(boSetCollision, &boGameInfo()->play.mCcS, &actor->mHeadSph);
}

static void boExecuteFinish(bo_class *actor) {
    cXyz &attentionPosition = actor->attention_info.position;
    u8 state = actor->mWaitParticleState;
    s32 animation = actor->mAnimation;
    boFinishEmitter(actor, &bo_class::mpWaitEmitter, &bo_class::mWaitParticleState,
                    state == 0 && (animation == dRes_INDEX_BO_BCK_WAIT_L1_e ||
                                   animation == dRes_INDEX_BO_BCK_WAIT_R1_e));
    state = actor->mHoldParticleState;
    animation = actor->mAnimation;
    boFinishEmitter(actor, &bo_class::mpHoldEmitter, &bo_class::mHoldParticleState,
                    state == 0 && animation == dRes_INDEX_BO_BCK_KAMU1_e);
    f32 target = actor->mTargetScaleY;
    boInvoke(cLib_addCalc2, &actor->scale.y, target, 1.f, 0.0874999985f);
    actor->mStts.m_weight = 0xff;
    boInvoke(mDoMtx_YrotS, gabi::at<Mtx34>((u32)l_matrixStack), (s16)actor->current.angle.y);
    boInvoke(mDoMtx_XrotM, gabi::at<Mtx34>((u32)l_matrixStack), (s16)actor->current.angle.x);
    gabi::Local<cXyz> forward, velocity;
    forward->y = 0;
    forward->x = 0;
    forward->z = actor->speedF;
    boInvoke(MtxPosition, forward.get(), velocity.get());
    f32 vertical = actor->speed.y + actor->gravity;
    actor->speed.x = velocity->x;
    actor->speed.y = vertical;
    actor->speed.z = velocity->z;
    boInvoke(boMoveActor, actor, &actor->mStts);
    u8 kind = actor->mType;
    if (kind == 0 || kind == 2)
        boMethod(&mDoExt_McaMorf::play, actor->mpUpperMorph.get(), nullptr, 0, 0);
    kind = actor->mType;
    if (kind == 0 || kind == 1) {
        boMethod(&mDoExt_McaMorf::play, actor->mpLowerMorph.get(), nullptr, 0, 0);
        if (actor->mType == 0) {
            if (actor->mMode != 5) {
                attentionPosition = actor->mHeadPos;
                attentionPosition.y = attentionPosition.y + 60.f;
                actor->eyePos = actor->mTipPos;
            } else {
                cXyz *root = &actor->mRootPos;
                actor->eyePos.x = root->x;
                actor->eyePos.y = root->y;
                f32 raised = root->y + 60.f;
                attentionPosition.x = root->x;
                actor->eyePos.y = raised;
                actor->eyePos.z = root->z;
                attentionPosition.y = raised;
                attentionPosition.z = root->z;
            }
        }
    }
    if (actor->mHoldingPlayer || actor->mMode == 12) {
        attentionPosition.x = actor->current.pos.x;
        attentionPosition.z = actor->current.pos.z;
        attentionPosition.y = actor->current.pos.y + 60.f;
        actor->shape_angle.y = actor->mFacingY;
    }
    boRegisterBody(actor);
    if (actor->mAction == 0) {
        boMethod(&cM3dGCyl::SetC, &actor->mBodyCyl.mCyl, &actor->mRootPos);
        boMethod(&cM3dGCyl::SetH, &actor->mBodyCyl.mCyl, 160.f);
        boMethod(&cM3dGCyl::SetR, &actor->mBodyCyl.mCyl, 15.f);
        boInvoke(boSetCollision, &boGameInfo()->play.mCcS, &actor->mBodyCyl);
    }
    kind = actor->mType;
    if (kind == 0 || kind == 1) {
        gabi::Local<cXyz> center;
        center->x = actor->current.pos.x;
        center->y = actor->current.pos.y;
        center->z = actor->current.pos.z;
        center->y = center->y + 10.f;
        boMethod(&cM3dGSph::SetC, &actor->mFootSph.mSph, center.get());
        boMethod(&cM3dGSph::SetR, &actor->mFootSph.mSph, 45.f);
        boInvoke(boSetCollision, &boGameInfo()->play.mCcS, &actor->mFootSph);
    }
    draw_SUB(actor);
}

static void boApproachAngles(bo_class *actor) {
    boInvoke(cLib_addCalcAngleS2, &actor->mFacingY, (s16)actor->mTargetFacingY, 1, 0x800);
    boInvoke(cLib_addCalcAngleS2, &actor->mStemAngle.y, (s16)actor->mStemTargetY, 1,
             (s16)actor->mAngleStep);
    boInvoke(cLib_addCalcAngleS2, &actor->mStemAngle.z, (s16)actor->mStemTargetZ, 1,
             (s16)actor->mAngleStep);
}

static bool boAnimationEnded(bo_class *actor) {
    u32 morph = gabi::ea(actor->mpUpperMorph.get());
    return ((u8)gabi::at<mDoExt_McaMorf>(morph)->mFrameCtrl.mState & 1) ||
           (f32)gabi::at<mDoExt_McaMorf>(morph)->mFrameCtrl.mRate == 0.f;
}

static void boStopAttack(bo_class *actor) {
    u32 flags = actor->mHeadSph.mObjAt.mSPrm;
    actor->mHeadSph.mGObjAt.mSpl = 0;
    actor->mHeadSph.mObjAt.mSPrm = flags & ~1u;
}

static void boRecoilMove(bo_class *actor, u32 player) {
    u8 attack = (u8)gabi::at<boPlayerView_c>(player)->mCutType;
    s16 turn = actor->mRecoilAngle;
    u32 matrix = (u32)l_matrixStack;
    if (attack != 3)
        turn = (s16)-turn;
    boInvoke(mDoMtx_YrotS, gabi::at<Mtx34>(matrix), turn);
    gabi::Local<cXyz> forward, displacement;
    forward->x = 0;
    forward->z = actor->mRecoilDistance;
    forward->y = 0;
    boInvoke(MtxPosition, forward.get(), displacement.get());
    s16 z = (s16)gabi::ftoi(displacement->z);
    s16 x = (s16)gabi::ftoi(displacement->x);
    s16 angle = actor->mRecoilAngle;
    s16 angularVelocity = actor->mRecoilVelocity;
    actor->mHeadAngle.y = z;
    actor->mHeadAngle.z = x;
    actor->mRecoilAngle = (s16)(angle + angularVelocity);
    boInvoke(cLib_addCalc0, &actor->mRecoilDistance, 1.f, 250.f);
    boInvoke(cLib_addCalcAngleS2, &actor->mHeadTargetAngle.y, 0, 1, 0x87);
    boInvoke(cLib_addCalcAngleS2, &actor->mHeadTargetAngle.z, 0, 1, 0x87);
    if (__builtin_fabsf(actor->mRecoilDistance) < 1.f) {
        actor->mHeadTargetAngle.x = 0;
        actor->mHeadTargetAngle.y = 0;
        actor->mHeadTargetAngle.z = 0;
        actor->mHeadAngle.x = 0;
        actor->mHeadAngle.y = 0;
        actor->mHeadAngle.z = 0;
        actor->mTimers[0] = 0;
        s16 delay = actor->mDeathDelay;
        actor->mMode = 2;
        if (delay)
            actor->mTimers[3] = 45;
    }
}

static void boDisableCollision(bo_class *actor) {
    u32 cylinder = actor->mBodyCyl.mObjTg.mSPrm;
    u32 target = actor->mHeadSph.mObjCo.mSPrm;
    u32 collision = actor->mHeadSph.mObjTg.mSPrm;
    actor->mBodyCyl.mObjTg.mSPrm = cylinder & ~1u;
    actor->mHeadSph.mObjTg.mSPrm = collision & ~1u;
    actor->mHeadSph.mObjCo.mSPrm = target & ~1u;
    boMethod(&dCcD_GObjInf::ClrTgHit, &actor->mBodyCyl);
    boMethod(&dCcD_GObjInf::ClrTgHit, &actor->mHeadSph);
}

static void boUnhide(bo_class *actor, u32 sound, bool resetAngles) {
    anm_init(actor, dRes_INDEX_BO_BCK_MODORU_U1_e, 5.f, bo::LOOP_ONCE, 1.f, -1, 0);
    anm_init(actor, dRes_INDEX_BO_BCK_MODORU_S1_e, 5.f, bo::LOOP_ONCE, 1.f, -1, 1);
    actor->attention_info.flags = actor->attention_info.flags & ~(u32)fopAc_Attn_LOCKON_BATTLE_e;
    effect_sound(actor, sound, 0);
    boDisableCollision(actor);
    if (resetAngles)
        angle_initial(actor);
}

static void boWake(bo_class *actor) {
    u32 player = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
    if (!(boInvoke(fopAcM_searchActorDistance, actor, gabi::at<fopAc_ac_c>(player)) < 600.f))
        return;
    u32 cylinder = actor->mBodyCyl.mObjTg.mSPrm;
    u32 target = actor->mHeadSph.mObjCo.mSPrm;
    u32 collision = actor->mHeadSph.mObjTg.mSPrm;
    actor->mBodyCyl.mObjTg.mSPrm = cylinder | 1;
    actor->mHeadSph.mObjTg.mSPrm = collision | 1;
    actor->mHeadSph.mObjCo.mSPrm = target | 1;
    effect_sound(actor, 0x5844, 0);
    monster_sound(actor, 0x484d);
    anm_init(actor, dRes_INDEX_BO_BCK_DERU_U1_e, 5.f, bo::LOOP_ONCE, 1.f, -1, 0);
    anm_init(actor, dRes_INDEX_BO_BCK_DERU_S1_e, 5.f, bo::LOOP_ONCE, 1.f, -1, 1);
    u32 attack = actor->mHeadSph.mObjAt.mSPrm;
    u32 status = actor->attention_info.flags;
    actor->mHeadSph.mObjAt.mRPrm = 1;
    actor->mHeadSph.mObjAt.mSPrm = (attack | 1) & ~0x10u;
    actor->mHeadSph.mGObjAt.mSpl = 1;
    actor->mMode = 1;
    actor->attention_info.flags = status | fopAc_Attn_LOCKON_BATTLE_e;
}

static void boEmerging(bo_class *actor) {
    u32 player = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
    actor->mTargetFacingY = boInvoke(fopAcM_searchActorAngleY, actor, gabi::at<fopAc_ac_c>(player));
    u32 emitter = actor->mpEmergingEmitter.v;
    if (!emitter) {
        emitter = boSpawnParticleReturn(actor, 0x8106);
        actor->mpEmergingEmitter.v = emitter;
    } else {
        u32 morph = gabi::ea(actor->mpUpperMorph.get());
        u32 model = (u32)gabi::at<mDoExt_McaMorf>(morph)->mpModel.v;
        u32 data = (u32)gabi::at<J3DModel>(model)->mpMtxBuffer.v;
        u16 flags = (u16)gabi::at<J3DMtxBuffer_hd>(data)->mFlags;
        u32 matrices = (u32)gabi::at<J3DMtxBuffer_hd>(data)->mpAnmMtx;
        gabi::at<J3DMtxBuffer_hd>(data)->mFlags = flags | 0x10;
        boInvoke(boSetEmitterRotation, &gabi::at<Mtx34>(matrices)[BO_UE1_JNT_J_BOKOBABA_AGO_e],
                 &gabi::at<boEmitterView_c>(emitter)->mGlobalRotation,
                 &gabi::at<boEmitterView_c>(emitter)->mGlobalTranslation);
    }
    u32 morph = gabi::ea(actor->mpUpperMorph.get());
    f32 frame = (f32)gabi::at<mDoExt_McaMorf>(morph)->mFrameCtrl.mFrame;
    if (frame > 39.f)
        boStopAttack(actor);
    if (boAnimationEnded(actor)) {
        emitter = actor->mpEmergingEmitter.v;
        if (emitter) {
            u32 flags = (u32)gabi::at<boEmitterView_c>(emitter)->mStatus;
            gabi::at<boEmitterView_c>(emitter)->mMaxFrame = 0xffffffff;
            gabi::at<boEmitterView_c>(emitter)->mStatus = flags | 1;
            actor->mpEmergingEmitter.v = 0;
        }
        wait_initial(actor);
    }
}

static void boWaiting(bo_class *actor) {
    if (actor->mTimers[3])
        return;
    u32 emitter = actor->mpWaitEmitter.v;
    if (!emitter) {
        emitter = boSpawnParticleReturn(actor, 0x8107);
        actor->mpWaitEmitter.v = emitter;
        if (emitter)
            gabi::at<boEmitterView_c>(emitter)->mpEmtrCallBack.v =
                gabi::ea(&actor->mSalivaCallback);
        actor->mWaitParticleState = 0;
    } else
        boEmitterMatrix(actor, emitter, false);
    if (actor->mTimers[0] == 0) {
        u32 morph = gabi::ea(actor->mpUpperMorph.get());
        f32 frame = (f32)gabi::at<mDoExt_McaMorf>(morph)->mFrameCtrl.mFrame;
        s32 next = actor->mAnimation == dRes_INDEX_BO_BCK_WAIT_R1_e ? dRes_INDEX_BO_BCK_WAIT_L1_e
                                                                    : dRes_INDEX_BO_BCK_WAIT_R1_e;
        anm_init(actor, next, 20.f, bo::LOOP_REPEAT, 1.f, -1, 0);
        actor->mpUpperMorph.get()->mFrameCtrl.mFrame = (f32)(s16)gabi::ftoi(frame);
        f32 random = boInvoke(cM_rndF, 60.f);
        actor->mTimers[0] = (s16)gabi::ftoi(random + 60.f);
    }
    bool attacking = false;
    if (actor->mTimers[1] == 0) {
        u32 player = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
        attacking =
            boInvoke(fopAcM_searchActorDistance, actor, gabi::at<fopAc_ac_c>(player)) < 380.f;
    }
    if (attacking) {
        anm_init(actor, dRes_INDEX_BO_BCK_ATTACK1_e, 5.f, bo::LOOP_ONCE, 1.f, -1, 0);
        if (boInvoke(cM_rnd) < 0.5f) {
            actor->mHeadSph.mAtp = 0;
            actor->mAction = 1;
            actor->mMode = 10;
        } else {
            u32 flags = actor->mHeadSph.mObjAt.mSPrm;
            u32 target = actor->mHeadSph.mObjCo.mSPrm;
            actor->mHeadSph.mAtp = 1;
            actor->mHeadSph.mObjAt.mSPrm = flags & ~0x10u;
            actor->mHeadSph.mObjCo.mSPrm = target | 1;
            monster_sound(actor, 0x484e);
            actor->mMode = 3;
        }
    } else {
        u32 player = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
        if (boInvoke(fopAcM_searchActorDistance, actor, gabi::at<fopAc_ac_c>(player)) > 900.f) {
            boUnhide(actor, 0x5848, true);
            actor->mMode = 0;
        }
    }
    u32 player = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
    actor->mTargetFacingY = boInvoke(fopAcM_searchActorAngleY, actor, gabi::at<fopAc_ac_c>(player));
}

static void boStriking(bo_class *actor, u32 companion, bool active) {
    if (active && actor->mAnimation == dRes_INDEX_BO_BCK_ATTACK1_e) {
        u32 morph = gabi::ea(actor->mpUpperMorph.get());
        f32 frame = (f32)gabi::at<mDoExt_McaMorf>(morph)->mFrameCtrl.mFrame;
        if (!(frame < 27.f) && (f32)gabi::at<mDoExt_McaMorf>(morph)->mFrameCtrl.mFrame < 55.f) {
            if ((f32)gabi::at<mDoExt_McaMorf>(morph)->mFrameCtrl.mFrame < 38.f) {
                if ((f32)gabi::at<mDoExt_McaMorf>(morph)->mFrameCtrl.mFrame == 27.f) {
                    actor->mHeadSph.mObjAt.mSPrm = actor->mHeadSph.mObjAt.mSPrm | 1;
                    actor->mHeadSph.mObjAt.mRPrm = 1;
                    effect_sound(actor, 0x5845, 0);
                } else if (actor->mHeadSph.mGObjAt.mRPrm & 1) {
                    actor->mHeadSph.mGObjAt.mSpl = 0;
                    actor->mMode = 6;
                    actor->mHeadSph.mObjAt.mSPrm = actor->mHeadSph.mObjAt.mSPrm & ~1u;
                    anm_init(actor, dRes_INDEX_BO_BCK_HANEKAERI1_e, 0.f, bo::LOOP_ONCE, 1.f, -1, 0);
                } else if (boMethod(&dCcD_GObjInf::ChkAtHit, &actor->mHeadSph)) {
                    void *hit = boMethod(&dCcD_GObjInf::GetAtHitAc, &actor->mHeadSph);
                    if (hit && gabi::ea(hit) == companion) {
                        boStopAttack(actor);
                        anm_init(actor, dRes_INDEX_BO_BCK_HANEKAERI1_e, 0.f, bo::LOOP_ONCE, 1.f, -1,
                                 0);
                    }
                }
            } else if (boMethod(&J3DFrameCtrl::checkPass,
                                &gabi::at<mDoExt_McaMorf>(morph)->mFrameCtrl, 38.f))
                boStopAttack(actor);
        } else {
            u32 player = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
            actor->mTargetFacingY =
                boInvoke(fopAcM_searchActorAngleY, actor, gabi::at<fopAc_ac_c>(player));
        }
    }
    if (boAnimationEnded(actor)) {
        f32 random = boInvoke(cM_rndF, 30.f);
        actor->mTimers[1] = (s16)gabi::ftoi(random + 30.f);
        wait_initial(actor);
    }
}

// HD: true skips the common angle and damage update after the move.
static bool boMoveMode5(bo_class *actor) {
    s16 deathDelay = actor->mDeathDelay;
    if (deathDelay) {
        deathDelay = (s16)(deathDelay - 1);
        actor->mDeathDelay = deathDelay;
        if (deathDelay == 1) {
            actor->mAction = 2;
            actor->mMode = 20;
            return true;
        }
    }

    if (actor->mBendCountY < 16 && !(actor->mBendFlags & 1)) {
        s16 current = actor->mStemAngle.y;
        s16 target = actor->mStemTargetY;
        s16 difference = boInvoke(boDistanceAngle, (s32)current, (s32)target);
        if (difference < 0x100) {
            target = actor->mStemTargetY;
            s16 adjusted = (s16)((u16)target ^ 0xff00);
            s16 count = actor->mBendCountY;
            count = (s16)(count + 1);
            actor->mStemTargetY = (s16)((s32)adjusted / 10);
            actor->mBendCountY = count;
            if (count >= 16) {
                u8 flags = actor->mBendFlags;
                actor->mStemTargetY = 0;
                actor->mStemAngle.y = 0;
                actor->mBendFlags = flags | 1;
            }
        }
    }
    if (actor->mBendCountZ < 16 && !(actor->mBendFlags & 2)) {
        s16 current = actor->mStemAngle.z;
        s16 target = actor->mStemTargetZ;
        s16 difference = boInvoke(boDistanceAngle, (s32)current, (s32)target);
        if (difference < 0x100) {
            target = actor->mStemTargetZ;
            s16 adjusted = (s16)((u16)target ^ 0xff00);
            s16 count = actor->mBendCountZ;
            count = (s16)(count + 1);
            actor->mStemTargetZ = (s16)((s32)adjusted / 10);
            actor->mBendCountZ = count;
            if (count >= 16) {
                u8 flags = actor->mBendFlags;
                actor->mStemTargetZ = 0;
                actor->mStemAngle.z = 0;
                actor->mBendFlags = flags | 2;
            }
        }
    }
    bool atRest = actor->mStemTargetY == 0;
    if (atRest)
        atRest = actor->mStemTargetZ == 0;
    s16 count = actor->mEffectTimer;
    if (atRest) {
        count = (s16)(count + 1);
        actor->mEffectTimer = count;
    }
    if (count > 30) {
        wait_initial(actor);
        actor->mTargetScaleY = 1.f;
        actor->mEffectTimer = 0;
    }
    if (actor->mDamageType != 0)
        return false;
    if (actor->mTimers[2] != 0)
        return false;
    if (!boMethod(&dCcD_GObjInf::ChkTgHit, &actor->mBodyCyl))
        return false;
    void *hit = boMethod(&dCcD_GObjInf::GetTgHitObj, &actor->mBodyCyl);
    if (!hit)
        return false;
    u32 type = (u32)((dCcD_GObjInf *)hit)->mAtType;

    if (type == bo::AT_TYPE_FIRE || type == bo::AT_TYPE_FIRE_ARROW) {
        actor->mEnemyFire.mFireDuration = 80;
        actor->mDamageType = 1;
        nokezori_damage_rtn(actor);
        actor->mBendCountY = 0;
        actor->mBendFlags = 0;
        actor->mBendCountZ = 0;
        boParticle(0x8109, &actor->mTipPos);
        return false;
    }
    if (type == bo::AT_TYPE_ICE_ARROW) {
        f32 duration = (f32)l_iceDurationExtra;
        actor->mEnemyIce.m00C = 1;
        actor->mDamageType = 2;
        actor->mEnemyIce.mFreezeDuration = (s16)gabi::ftoi((f32)(duration + 80.f));
        actor->health = 30;
        actor->attention_info.flags = 0;
        nokezori_damage_rtn(actor);
        actor->mBendCountY = 0;
        actor->mBendCountZ = 0;
        actor->mBendFlags = 0;
        boParticle(0x8109, &actor->mTipPos);
        return false;
    }
    if (type == bo::AT_TYPE_LIGHT_ARROW) {
        actor->mEnemyIce.mParticleScale = 1.f;
        actor->mEnemyIce.mLightShrinkTimer = 1;
        f32 duration = (f32)l_iceDurationExtra;
        actor->mEnemyIce.mYOffset = (f32)(duration + 80.f);
        actor->attention_info.flags = 0;
        u8 kind = actor->mVariant;
        actor->mDamageType = 3;
        boInvoke(boCreateActor, bo::fpcNm_BO_e, ((u32)kind << 8) | 1, &actor->current.pos,
                 (s32)(s8)actor->current.roomNo, &actor->current.angle, &actor->scale, 0, 0);
        nokezori_damage_rtn(actor);
        actor->mBendCountY = 0;
        actor->mBendCountZ = 0;
        actor->mBendFlags = 0;
        boParticle(0x8109, &actor->mTipPos);
        monster_sound(actor, 0x4850);
        effect_sound(actor, 0x2828, 0x20);
        return false;
    }

    u32 player = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
    u32 attack = actor->mHeadSph.mObjAt.mSPrm;
    f32 z = actor->mBodyCyl.mGObjTg.mHitPos.z;
    actor->mHeadSph.mObjAt.mSPrm = attack & ~1u;
    gabi::Local<cXyz> point;
    point->z = z;
    f32 y = actor->mBodyCyl.mGObjTg.mHitPos.y;
    f32 x = actor->mBodyCyl.mGObjTg.mHitPos.x;
    point->y = y;
    point->x = x;
    actor->mHeadSph.mGObjAt.mSpl = 0;
    u32 status = actor->attention_info.flags;
    actor->attention_info.flags = status & ~(u32)fopAc_Attn_LOCKON_BATTLE_e;
    actor->mTargetScaleY = 1.f;
    boParticle(0x10, point.get());
    gabi::Local<cXyz> scale;
    scale->x = 2.f;
    scale->y = 2.f;
    scale->z = 2.f;
    boParticle(0xf, point.get(), &gabi::at<fopAc_ac_c>(player)->shape_angle, scale.get());
    s16 delay = (s16)l_deathDelayExtra;
    actor->mAction = 2;
    actor->mMode = 20;
    actor->mDeathDelay = (u8)((s32)delay + 5);
    monster_sound(actor, 0x4850);
    effect_sound(actor, 0x2828, 0x20);
    return false;
}

/* 020BB0E8 */
static void smoke_set(bo_class *actor) {
    boMethod(&dPa_smokeEcallBack::end, &actor->mSmokeCallback);
    s32 room = (s8)actor->current.roomNo;
    u32 particle = (u32)boGameInfo()->play.mParticle.v;
    boInvoke(boSetParticle, (dPa_control_c *)(gabi::at<void>(particle)), 2, 0x2027,
             (const cXyz *)(&actor->mSmokePos), (const csXyz *)(&actor->shape_angle), nullptr, 0xb9,
             (void *)(&actor->mSmokeCallback), room, nullptr, nullptr, nullptr);
    u32 emitter = actor->mSmokeCallback.mpEmitter.v;
    if (emitter) {
        u32 flags = (u32)gabi::at<boEmitterView_c>(emitter)->mStatus;
        gabi::at<boEmitterView_c>(emitter)->mMaxFrame = 1;
        gabi::at<boEmitterView_c>(emitter)->mInitialVelAxis = 10.f;
        gabi::at<boEmitterView_c>(emitter)->mGlobalParticleScale.x = 3.f;
        gabi::at<boEmitterView_c>(emitter)->mInitialVelOmni = 10.f;
        gabi::at<boEmitterView_c>(emitter)->mStatus = flags | 0x40;
        gabi::at<boEmitterView_c>(emitter)->mGlobalParticleScale.y = 3.f;
        gabi::at<boEmitterView_c>(emitter)->mGlobalParticleScale.z = 3.f;
        gabi::at<boEmitterView_c>(emitter)->mRate = 30.f;
        ((boSmokeColorView_c *)&actor->mSmokeCallback)->mColorRGBA = 0xA0A080FF;
    }
}

/* 020BB1E4 */
static BOOL nodeCallBack_UP(J3DNode *node, s32 stage) {
    if (stage != 0)
        return 1;
    J3DJoint *joint = boInvoke(boGetJointData, node);
    u32 model = (u32)l_currentModel;
    bo_class *actor = gabi::at<bo_class>((u32)gabi::at<J3DModel>(model)->mUserArea);
    u16 index = (u16)((boJointDataView_c *)joint)->mJntNo;
    if (!actor)
        return 1;
    u32 play = boPlay();
    u32 data = (u32)gabi::at<J3DModel>(model)->mpMtxBuffer.v;
    u16 flags = (u16)gabi::at<J3DMtxBuffer_hd>(data)->mFlags;
    void *player =
        gabi::at<void>((u32)gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    u32 matrices = (u32)gabi::at<J3DMtxBuffer_hd>(data)->mpAnmMtx;
    gabi::at<J3DMtxBuffer_hd>(data)->mFlags = flags | 0x10;
    boInvoke(PSMTXCopy, gabi::at<Mtx34>(matrices + index * 0x30),
             gabi::at<Mtx34>((u32)l_matrixStack));
    bool changed = false;
    if (index <= 10) {
        f32 factor = (f32)index * 0.02500000037252903f;
        upper_joint_rotate(mDoMtx_YrotM, &actor->mStemAngle.y, factor);
        upper_joint_rotate(mDoMtx_XrotM, &actor->mStemAngle.x, factor);
        upper_joint_rotate(mDoMtx_ZrotM, &actor->mStemAngle.z, factor);
        changed = true;
        if (index == 1)
            upper_joint_position(&actor->mRootPos);
        else if (index == 10)
            upper_joint_position(&actor->mTipPos);
    }
    if (index != 1 && index != 10) {
        if (index == 11) {
            upper_joint_position(&actor->mHeadPos);
            changed = true;
        } else if (index == 4) {
            upper_joint_position(&actor->mLeafPos);
            changed = true;
        } else if ((u32)index - 5 < 3) {
            f32 factor = (f32)(s16)(8 - index) * 1.75f;
            upper_joint_rotate(mDoMtx_YrotM, &actor->mHeadTargetAngle.y, factor);
            upper_joint_rotate(mDoMtx_ZrotM, &actor->mHeadTargetAngle.z, factor);
            changed = true;
        }
    }
    if (index == 8) {
        s16 yaw = actor->mHeadAngle.y;
        s16 adjust = (s16)l_mouthYawAdjust;
        u32 matrix = (u32)l_matrixStack;
        boInvoke(mDoMtx_YrotM, gabi::at<Mtx34>(matrix), (s32)(s16)(yaw + adjust));
        s16 pitch = actor->mHeadAngle.z;
        adjust = (s16)l_mouthPitchAdjust;
        matrix = (u32)l_matrixStack;
        boInvoke(mDoMtx_XrotM, gabi::at<Mtx34>(matrix), (s32)(s16)(pitch + adjust));
        upper_joint_position(&actor->mMouthPos);
        upper_joint_commit(model, index);
        return 1;
    }
    if (index == 12 && actor->mHoldingPlayer != 0) {
        gabi::Local<cXyz> origin, position;
        origin->x = 0;
        origin->y = 0;
        origin->z = 0;
        boInvoke(MtxPosition, origin.get(), position.get());
        f32 mouthX = actor->mMouthPos.x, x = position->x;
        u8 flipped = actor->mThrowReversed;
        f32 mouthZ = actor->mMouthPos.z, z = position->z;
        gabi::Local<csXyz> angle;
        angle->x = flipped ? (s16)-24500 : (s16)23500;
        s16 yaw = boInvoke(cM_atan2s, mouthX - x, mouthZ - z);
        angle->z = 0;
        angle->y = flipped ? (s16)(yaw + 0x7fff) : yaw;
        u32 vtable = (u32)((boPlayerView_c *)player)->__vtbl;
        u32 target = gabi::load<u32>(vtable + 0x11c);
        boVirtualCall(target, player, position.get(), angle.get());
        upper_joint_commit(model, index);
        return 1;
    }
    if (changed)
        upper_joint_commit(model, index);
    return 1;
}

/* 020BB838 */
static BOOL nodeCallBack_DW(J3DNode *node, s32 stage) {
    if (stage == 0) {
        J3DJoint *joint = boInvoke(boGetJointData, node);
        u32 model = (u32)l_currentModel;
        u32 actor = (u32)gabi::at<J3DModel>(model)->mUserArea;
        u16 index = (u16)((boJointDataView_c *)joint)->mJntNo;
        if (actor && index == 9) {
            u32 data = (u32)gabi::at<J3DModel>(model)->mpMtxBuffer.v;
            u16 flags = (u16)gabi::at<J3DMtxBuffer_hd>(data)->mFlags;
            u32 joints = (u32)gabi::at<J3DMtxBuffer_hd>(data)->mpAnmMtx;
            gabi::at<J3DMtxBuffer_hd>(data)->mFlags = flags | 0x10;
            u32 matrix = (u32)l_matrixStack;
            boInvoke(PSMTXCopy, &gabi::at<Mtx34>(joints)[9], gabi::at<Mtx34>(matrix));
            gabi::Local<cXyz> origin;
            origin->y = 0;
            origin->z = 0;
            origin->x = 0;
            boInvoke(MtxPosition, origin.get(), &gabi::at<bo_class>(actor)->mBodyPos);
        }
    }
    return 1;
}

/* 020C00C0 */
static void yodare_execute(void *callback, void *emitter, void *particle) {
    boYodareEcallBack_c *saliva = (boYodareEcallBack_c *)callback;
    boParticleView_c *drop = (boParticleView_c *)particle;
    f32 x = drop->mGlobalPosition.x, z = drop->mGlobalPosition.z, y = drop->mGlobalPosition.y;
    gabi::Local<cXyz> position;
    position->z = z;
    position->y = y + 20.f;
    position->x = x;
    saliva->mGroundCheck.m_pos.x = x;
    saliva->mGroundCheck.m_pos.z = z;
    saliva->mGroundCheck.m_pos.y = y + 20.f;
    u32 world = boPlay();
    f32 ground = boMethod(&dBgS::GroundCross, &gabi::at<dComIfG_inf_c>(world)->play.mBgS,
                          &saliva->mGroundCheck);
    position->y = ground;
    if (ground > y) {
        u32 particleControl = (u32)boGameInfo()->play.mParticle.v;
        boInvoke(boSetParticle, (dPa_control_c *)(gabi::at<void>(particleControl)), 0, 0x8108,
                 (const cXyz *)(position.get()), nullptr, nullptr, 0xff, nullptr, -1, nullptr,
                 nullptr, nullptr);
    }
}

/* 020BB8D0 */
static void draw_SUB(bo_class *actor) {
    u8 kind = actor->mType;
    Mtx34 *matrix = &l_baseMatrix;
    if (kind == 2) {
        actor->mBodyPos.copy(actor->current.pos);
        boInvoke(boPlayColorAnimation, actor->mpColorAnm.get());
    }
    if (kind == 0 || kind == 2) {
        f32 y = actor->mBodyPos.y, height = actor->mBodyHeight;
        f32 x = actor->mBodyPos.x, z = actor->mBodyPos.z;
        boInvoke(PSMTXTrans, matrix, x, y + height, z);
        boInvoke(mDoMtx_YrotM, matrix, (s32)actor->mFacingY);
        boInvoke(mDoMtx_XrotM, matrix, (s32)actor->shape_angle.x);
        boInvoke(mDoMtx_ZrotM, matrix, (s32)actor->shape_angle.z);
        height = actor->mBodyHeight;
        boInvoke(mDoMtx_stack_c::transM, 0.f, -height, 0.f);
        u32 morph = gabi::ea(actor->mpUpperMorph.get());
        copy_base_matrix((u32)gabi::at<mDoExt_McaMorf>(morph)->mpModel.v);
        boMethod(&mDoExt_McaMorf::calc, actor->mpUpperMorph.get());
        kind = actor->mType;
        if (kind == 0) {
            boInvoke(boEnemyFire, &actor->mEnemyFire);
            kind = actor->mType;
        }
    }
    if (kind == 0 || kind == 1) {
        f32 z = actor->current.pos.z, x = actor->current.pos.x, y = actor->current.pos.y;
        boInvoke(PSMTXTrans, matrix, x, y, z);
        boInvoke(mDoMtx_YrotM, matrix, (s32)actor->current.angle.y);
        boInvoke(mDoMtx_XrotM, matrix, (s32)actor->shape_angle.x);
        boInvoke(mDoMtx_ZrotM, matrix, (s32)actor->shape_angle.z);
        u32 morph = gabi::ea(actor->mpLowerMorph.get());
        copy_base_matrix((u32)gabi::at<mDoExt_McaMorf>(morph)->mpModel.v);
        boMethod(&mDoExt_McaMorf::calc, actor->mpLowerMorph.get());
    }
    dScnKy_env_light_c *environment = boInvoke(dKy_getEnvlight);
    boInvoke(settingTevStruct, environment, 0, &actor->current.pos, &actor->tevStr);
}

/* 020BBAEC */
static BOOL daBO_Draw(bo_class *actor) {
    if ((s16)l_disableDraw != 0)
        return 1;
    u8 kind = actor->mType;
    if (kind == 0 || kind == 2) {
        u32 model = (u32)(actor->mpUpperMorph.get())->mpModel.v;
        dScnKy_env_light_c *environment = boInvoke(dKy_getEnvlight);
        boInvoke(setLightTevColorType, environment, gabi::at<J3DModel>(model), &actor->tevStr);
        kind = actor->mType;
    }
    if ((kind == 0 || kind == 1) && actor->mEnemyIce.mLightShrinkTimer == 0) {
        u32 model = (u32)(actor->mpLowerMorph.get())->mpModel.v;
        dScnKy_env_light_c *environment = boInvoke(dKy_getEnvlight);
        boInvoke(setLightTevColorType, environment, gabi::at<J3DModel>(model), &actor->tevStr);
    }
    s32 yaw = actor->mFacingY;
    boInvoke(boRegisterSnap, 0xb2, actor, &actor->current.pos, yaw, 1.f, 1.f, 1.f);
    kind = actor->mType;
    if (kind == 0 && actor->mEnemyIce.mFreezeTimer > 20) {
        boInvoke(boDrawIce, actor->mpUpperMorph.get(), -1, actor->mInvisibleModel);
        kind = actor->mType;
    } else if (kind == 0 || kind == 2) {
        mDoExt_McaMorf *upper = actor->mpUpperMorph.get();
        u32 model = (u32)upper->mpModel.v;
        if (kind == 2) {
            u32 colorAnm = actor->mpColorAnm.v;
            u32 data = (u32)gabi::at<J3DModel>(model)->mModelData.v;
            f32 frame = (f32)gabi::at<mDoExt_baseAnm>(colorAnm)->mFrameCtrl.mFrame;
            boInvoke(boEntryColorAnimation, gabi::at<void>(colorAnm), gabi::at<void>(data), frame);
            upper = actor->mpUpperMorph.get();
        }
        boMethod(&mDoExt_McaMorf::entryDL, upper);
        kind = actor->mType;
        if (kind == 2) {
            u32 data = (u32)gabi::at<J3DModel>(model)->mModelData.v;
            gabi::at<boModelDataView_c>(data)->mTevRegAnimation = 0;
            kind = actor->mType;
        }
    }
    if ((kind == 0 || kind == 1) && actor->mEnemyIce.mLightShrinkTimer == 0)
        boMethod(&mDoExt_McaMorf::entryDL, actor->mpLowerMorph.get());
    return 1;
}

/* 020BBC58 */
static void anm_init(bo_class *actor, s32 animation, f32 blend, u8 loop, f32 speed, s32 secondary,
                     s32 lower) {
    actor->mAnimation = animation;
    void *first = boResource(animation);
    void *second = nullptr;
    if (secondary >= 0)
        second = boResource(secondary);
    mDoExt_McaMorf *morph = lower ? actor->mpLowerMorph.get() : actor->mpUpperMorph.get();
    boMethod(&mDoExt_McaMorf::setAnm, morph, (J3DAnmTransform *)first, (u32)loop, blend, speed, 0.f,
             -1.f, second);
}

/* 020BBE4C */
static void nokezori_damage_rtn(bo_class *actor) {
    s16 yaw = actor->mFacingY, hitYaw = actor->mHitAngleY;
    s16 delta = (s16)(hitYaw - yaw);
    anm_init(actor, dRes_INDEX_BO_BCK_DAMAGE1_e, 5.f, bo::LOOP_ONCE, 1.f, -1, 0);
    boInvoke(mDoMtx_YrotS, gabi::at<Mtx34>((u32)l_matrixStack), (s32)delta);
    gabi::Local<cXyz> direction, result;
    direction->y = 0;
    direction->z = 6250.f;
    direction->x = 0;
    boInvoke(MtxPosition, direction.get(), result.get());
    f32 x = result->x, z = result->z;
    actor->mStemTargetZ = (s16)gabi::ftoi(x);
    actor->mStemTargetY = (s16)gabi::ftoi(z);
    actor->mHeadAngle.x = 0;
    actor->mHeadAngle.y = 0;
    actor->mHeadAngle.z = 0;
    actor->scale.y = 1.75f;
    actor->mAngleStep = 1200;
    void *hit = boMethod(&dCcD_GObjInf::GetTgHitObj, &actor->mHeadSph);
    if (hit) {
        u32 type = (u32)((dCcD_GObjInf *)hit)->mAtType;
        if (type & bo::AT_TYPE_SWORD) {
            monster_sound(actor, 0x484f);
            effect_sound(actor, 0x2806, 0x20);
        } else if (type & bo::AT_TYPE_BOKO_STICK) {
            monster_sound(actor, 0x484f);
            effect_sound(actor, 0x2835, 0x20);
        } else if (type & bo::AT_TYPE_SKULL_HAMMER) {
            monster_sound(actor, 0x484f);
            effect_sound(actor, 0x2855, 0x31);
        } else if (!(type & bo::AT_TYPE_BOOMERANG)) {
            monster_sound(actor, 0x484f);
            effect_sound(actor, 0x2836, 0x20);
        }
    }
    u8 detached = actor->mDamageType;
    actor->mBendCountY = 0;
    actor->mEffectTimer = 0;
    actor->mTargetScaleY = 1.f;
    actor->mTimers[2] = 3;
    if (detached == 0) {
        actor->mAction = 0;
        actor->mMode = 5;
    } else {
        actor->mAction = 4;
        actor->mMode = 50;
    }
}

/* 020BC15C */
static BOOL damage_check(bo_class *actor) {
    void *player = gabi::at<void>((u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v);
    boMethod(&dCcD_Stts::Move, &actor->mStts);
    if (actor->mTimers[4] != 0)
        return 0;
    do {
        void *shockPlayer = gabi::at<void>((u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v);
        if ((u32)((boPlayerView_c *)shockPlayer)->mResetFlg0 & 0x20000) {
            f32 playerZ = (f32)((boPlayerView_c *)shockPlayer)->mSwordTopPos.z,
                z = actor->current.pos.z;
            f32 playerX = (f32)((boPlayerView_c *)shockPlayer)->mSwordTopPos.x,
                x = actor->current.pos.x;
            f32 dz = playerZ - z, dx = playerX - x;
            f32 distance = boInvoke(std_sqrtf, gabi::fmadds(dx, dx, dz * dz));
            if (distance < 1000.f) {
                effect_sound(actor, 0x2855, 0x31);
                actor->mReaction = 1;
                break;
            }
        }
        if (!boMethod(&dCcD_GObjInf::ChkTgHit, &actor->mHeadSph))
            return 0;
        {
            void *hit = boMethod(&dCcD_GObjInf::GetTgHitObj, &actor->mHeadSph);
            if (!hit)
                return 0;
            actor->mHitPos.copy(actor->mHeadSph.mGObjTg.mHitPos);
            s16 invincible = (s16)gabi::ftoi((f32)l_invincibilityExtra + 8.f);
            f32 x = actor->mHeadPos.x, z = actor->mHeadPos.z;
            actor->mTimers[4] = invincible;
            f32 playerX = (f32)((boPlayerView_c *)player)->current.pos.x,
                playerZ = (f32)((boPlayerView_c *)player)->current.pos.z;
            s16 angle = boInvoke(cM_atan2s, playerX - x, playerZ - z);
            actor->mHitAngleY = angle;
            actor->mDeathDelay = 0;
            actor->mReaction = 1;
            boInvoke(boStopBattleSound);
            u32 type = (u32)((dCcD_GObjInf *)hit)->mAtType;
            switch (type) {
            case 2: {
                u8 attack = (u8)((boPlayerView_c *)player)->mCutType;
                bool matched = false;
                for (u32 index = 0; index < 32; ++index) {
                    if (attack == (u16)l_attackTypes[index]) {
                        u8 reaction = (u8)l_attackReactions[index];
                        actor->mReaction = reaction;
                        matched = true;
                        break;
                    }
                }
                if (!matched)
                    return 0;
                break;
            }
            case bo::AT_TYPE_BOOMERANG: {
                actor->mReaction = 1;
                u8 timer = (u8)((s16)l_deathDelayExtra + 5);
                actor->mHeadSph.mGObjAt.mSpl = 0;
                u32 at = actor->mHeadSph.mObjAt.mSPrm;
                actor->mDeathDelay = timer;
                u32 status = actor->attention_info.flags;
                actor->mTargetScaleY = 1.f;
                actor->mHeadSph.mObjAt.mSPrm = at & ~1u;
                actor->attention_info.flags = status & ~(u32)fopAc_Attn_LOCKON_BATTLE_e;
                boParticle(0x10, &actor->mHitPos);
                gabi::Local<cXyz> scale;
                scale->x = 2.f;
                scale->z = 2.f;
                scale->y = 2.f;
                boParticle(0xf, &actor->mHitPos, &((boPlayerView_c *)player)->shape_angle,
                           scale.get());
                monster_sound(actor, 0x4850);
                effect_sound(actor, 0x2828, 0x20);
                break;
            }
            case bo::AT_TYPE_FIRE:
            case bo::AT_TYPE_FIRE_ARROW: {
                f32 timer = (f32)l_fireDurationExtra + 34.f;
                actor->mDamageType = 1;
                actor->mEnemyFire.mFireDuration = (s16)gabi::ftoi(timer);
                actor->attention_info.flags = 0;
                break;
            }
            case bo::AT_TYPE_ICE_ARROW: {
                f32 timer = (f32)l_iceDurationExtra + 80.f;
                actor->mDamageType = 2;
                actor->mEnemyIce.m00C = 1;
                actor->mEnemyIce.mFreezeDuration = (s16)gabi::ftoi(timer);
                actor->health = 30;
                actor->attention_info.flags = 0;
                break;
            }
            case bo::AT_TYPE_LIGHT_ARROW: {
                actor->mEnemyIce.mParticleScale = 1.f;
                actor->mEnemyIce.mLightShrinkTimer = 1;
                f32 timer = (f32)l_iceDurationExtra + 80.f;
                actor->attention_info.flags = 0;
                s32 room = (s8)actor->current.roomNo;
                u8 variant = actor->mVariant;
                actor->mEnemyIce.mYOffset = timer;
                actor->mDamageType = 3;
                boInvoke(boCreateActor, bo::fpcNm_BO_e, ((u32)variant << 8) | 1,
                         &actor->current.pos, room, &actor->current.angle, &actor->scale, 0, 0);
                break;
            }
            case bo::AT_TYPE_SKULL_HAMMER:
                if ((u8)((boPlayerView_c *)player)->mCutType == 17) {
                    actor->mReaction = 1;
                    break;
                }
                actor->mReaction = 2;
                break;
            case bo::AT_TYPE_GRAPPLING_HOOK: {
                if (actor->stealItemLeft > 0) {
                    u8 health = actor->health;
                    actor->health = 10;
                    void *attack = boMethod(&dCcD_GObjInf::GetTgHitObj, &actor->mHeadSph);
                    gabi::Local<be<u32>[8]> hitInfo;
                    (*hitInfo)[0] = gabi::ea(attack);
                    (*hitInfo)[5] = 0;
                    boInvoke(boCheckAttack, actor, hitInfo.get());
                    actor->health = health;
                }
                boParticle(0x27b, &actor->attention_info.position);
                actor->mReaction = 0;
                monster_sound(actor, 0x484f);
                effect_sound(actor, 0x2836, 0x20);
                break;
            }
            default:
                actor->mReaction = 1;
                break;
            }
        }
    } while (false);
    {
        s16 yaw = actor->mFacingY, hitYaw = actor->mHitAngleY;
        s16 delta = (s16)(hitYaw - yaw);
        u8 reaction = actor->mReaction;
        if (reaction == 2) {
            void *currentPlayer = gabi::at<void>((u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v);
            gabi::Local<cXyz> position;
            position->x = actor->mHeadSph.mGObjTg.mHitPos.x;
            position->y = actor->mHeadSph.mGObjTg.mHitPos.y;
            position->z = actor->mHeadSph.mGObjTg.mHitPos.z;
            boParticle(0x10, position.get());
            gabi::Local<cXyz> scale;
            scale->x = 2.f;
            scale->z = 2.f;
            scale->y = 2.f;
            boParticle(0xf, position.get(), &((boPlayerView_c *)currentPlayer)->shape_angle,
                       scale.get());
            actor->mAction = 4;
            actor->mMode = 40;
            if (actor) {
                monster_sound(actor, 0x4850);
                effect_sound(actor, 0x2828, 0x20);
            }
            return 1;
        }
        u32 attackFlags = actor->mHeadSph.mObjAt.mSPrm;
        reaction = actor->mReaction;
        actor->mHeadSph.mObjAt.mSPrm = attackFlags & ~1u;
        actor->mHeadSph.mGObjAt.mSpl = 0;
        if (reaction == 1) {
            nokezori_damage_rtn(actor);
            actor->mBendCountY = 0;
            actor->mBendCountZ = 0;
            actor->mBendFlags = 0;
            boParticle(0x8109, &actor->mTipPos);
            return 1;
        }
        if ((f32)l_disableRecoil != 0.f && actor->mMode == 4)
            return 0;
        if (actor)
            monster_sound(actor, 0x484f);
        gabi::Local<cXyz> direction, result;
        direction->x = 0;
        direction->y = 0;
        direction->z = 2250.f;
        boInvoke(mDoMtx_YrotS, gabi::at<Mtx34>((u32)l_matrixStack), (s32)delta);
        boInvoke(MtxPosition, direction.get(), result.get());
        anm_init(actor, dRes_INDEX_BO_BCK_PUNCH1_e, 2.f, bo::LOOP_ONCE, 1.f, -1, 0);
        f32 x = result->x, z = result->z;
        yaw = actor->mFacingY;
        hitYaw = actor->mHitAngleY;
        actor->mHeadTargetAngle.z = (s16)gabi::ftoi(x);
        actor->mHeadTargetAngle.y = (s16)gabi::ftoi(z);
        actor->mRecoilAngle = 0;
        actor->mRecoilVelocity = 7250;
        actor->mRecoilDistance = -10000.f;
        actor->mRecoilFacingY = (s16)(hitYaw - yaw);
        void *currentPlayer = gabi::at<void>((u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v);
        u8 attack = (u8)((boPlayerView_c *)currentPlayer)->mCutType;
        if (attack == 3)
            actor->mRecoilFacingY = (s16)(actor->mRecoilFacingY - 0x4000);
        else if (attack == 4)
            actor->mRecoilFacingY = (s16)(actor->mRecoilFacingY + 0x4000);
        actor->mAngleStep = 0x800;
        void *hit = boMethod(&dCcD_GObjInf::GetTgHitObj, &actor->mHeadSph);
        if (hit && actor && gabi::ea(&actor->eyePos)) {
            u32 type = (u32)((dCcD_GObjInf *)hit)->mAtType;
            if (type & bo::AT_TYPE_SWORD)
                effect_sound(actor, 0x2803, 0x31);
            else
                effect_sound(actor, type & bo::AT_TYPE_BOKO_STICK ? 0x2833 : 0x2834, 0x31);
        }
        boDamageFinish(actor);
        return 1;
    }
}

/* 020BCDA8 */
static void angle_initial(bo_class *actor) {
    actor->mUnusedBendAngle = 0;
    actor->mStemAngle.x = 0;
    actor->mHeadAngle.x = 0;
    actor->mHeadTargetAngle.x = 0;
    actor->mHeadAngle.z = 0;
    actor->mHeadTargetAngle.y = 0;
    actor->mStemTargetZ = 0;
    actor->mStemAngle.y = 0;
    actor->mStemAngle.z = 0;
    actor->mStemTargetY = 0;
    actor->mHeadTargetAngle.z = 0;
    actor->mHeadAngle.y = 0;
}

/* 020BCDE0 */
static void wait_initial(bo_class *actor) {
    s32 animation = dRes_INDEX_BO_BCK_WAIT_R1_e;
    if (boInvoke(cM_rnd) < 0.5f)
        animation = dRes_INDEX_BO_BCK_WAIT_L1_e;
    anm_init(actor, animation, 5.f, bo::LOOP_REPEAT, 1.f, -1, 0);
    f32 random = boInvoke(cM_rndF, 60.f);
    u32 attackFlags = actor->mHeadSph.mObjAt.mSPrm;
    s16 delay = (s16)gabi::ftoi(random + 60.f);
    u32 targetFlags = actor->mHeadSph.mObjCo.mSPrm;
    actor->mHeadSph.mGObjAt.mSpl = 0;
    actor->mHeadSph.mObjAt.mSPrm = attackFlags & ~1u;
    actor->mHeadSph.mObjCo.mSPrm = targetFlags | 1;
    actor->mTimers[0] = delay;
    angle_initial(actor);
    actor->mAction = 0;
    actor->mMode = 2;
}

static void bo_move(bo_class *actor) {
    u32 player = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
    u32 companion = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
    bool interpolate = true;
    switch ((u8)actor->mMode) {
    case 0:
        boWake(actor);
        break;
    case 1:
        boEmerging(actor);
        break;
    case 2:
        boWaiting(actor);
        break;
    case 3:
        boStriking(actor, companion, true);
        break;
    case 4:
        boRecoilMove(actor, player);
        break;
    case 5:
        interpolate = !boMoveMode5(actor);
        break;
    case 6:
        boStriking(actor, companion, false);
        break;
    case 7:
        if (boAnimationEnded(actor))
            actor->mMode = 0;
        break;
    default:
        break;
    }
    if (interpolate) {
        boApproachAngles(actor);
        bool skipDamage = false;
        if (actor->mMode == 4) {
            u32 currentPlayer = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
            s16 target =
                boInvoke(fopAcM_searchActorAngleY, actor, gabi::at<fopAc_ac_c>(currentPlayer));
            s16 angle = actor->mFacingY;
            s16 difference = boInvoke(boDistanceAngle, (s32)target, (s32)angle);
            if (difference > 0x337f) {
                wait_initial(actor);
                boUnhide(actor, 0x5848, false);
                actor->mMode = 7;
                skipDamage = true;
            }
        }
        if (!skipDamage && actor->mMode != 0 && actor->mMode != 5)
            damage_check(actor);
    }
    u16 x = actor->current.angle.x;
    u16 y = actor->current.angle.y;
    actor->shape_angle.x = x;
    u16 z = actor->current.angle.z;
    actor->shape_angle.y = y;
    actor->shape_angle.z = z;
}

static void bo2_move(bo_class *actor) {
    u32 player = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
    u32 companion = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
    f32 stickX = boInvoke(boStickX, 0);
    f32 stickY = boInvoke(boStickY, 0);
    u8 mode = actor->mMode;
    switch (mode) {
    case 10: {
        anm_init(actor, dRes_INDEX_BO_BCK_TABERU1_e, 5.f, bo::LOOP_ONCE, 1.f, -1, 0);
        effect_sound(actor, 0x5849, 0);
        u32 attack = actor->mHeadSph.mObjAt.mSPrm;
        u8 next = actor->mMode;
        actor->mCapturePending = 0;
        u32 target = actor->mHeadSph.mObjCo.mSPrm;
        next = (u8)(next + 1);
        actor->mHeadSph.mObjAt.mSPrm = attack | 0x10;
        actor->mHoldingPlayer = 0;
        actor->mMode = next;
        actor->mHeadSph.mObjCo.mSPrm = target & ~1u;
        break;
    }
    case 11: {
        u32 upper = gabi::ea(actor->mpUpperMorph.get());
        f32 frame = (f32)gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl.mFrame;
        if (!(frame < 28.f)) {
            frame = (f32)gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl.mFrame;
            if (!(frame > 33.f)) {
                if (boMethod(&dCcD_GObjInf::ChkAtHit, &actor->mHeadSph)) {
                    companion = gabi::ea(boMethod(&dCcD_GObjInf::GetAtHitAc, &actor->mHeadSph));
                    if (companion && companion == player) {
                        u32 actualPlayer = (u32)boGameInfo()->play.mpPlayerPtr[0].v;
                        if (companion == actualPlayer)
                            actor->mCapturePending = 1;
                    }
                }
                upper = gabi::ea(actor->mpUpperMorph.get());
            }
        }
        if (boMethod(&J3DFrameCtrl::checkPass, &gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl,
                     27.f)) {
            u32 attack = actor->mHeadSph.mObjAt.mSPrm;
            actor->mHeadSph.mObjAt.mSPrm = attack | 1;
            actor->mHeadSph.mObjAt.mRPrm = 1;
            // HD uses r26 as the sound's retained actor ID here.
            if (actor && (u32)(gabi::ea(actor) + offsetof(bo_class, eyePos)))
                companion = (u32)actor->mBsPcId;
            monster_sound(actor, 0x4851);
        }
        upper = gabi::ea(actor->mpUpperMorph.get());
        BOOL passed =
            boMethod(&J3DFrameCtrl::checkPass, &gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl, 33.f);
        upper = gabi::ea(actor->mpUpperMorph.get());
        if (passed) {
            u32 attack = actor->mHeadSph.mObjAt.mSPrm;
            actor->mHeadSph.mGObjAt.mSpl = 0;
            actor->mHeadSph.mObjAt.mSPrm = attack & ~1u;
        }
        if (((u8)gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl.mState & 1) ||
            (f32)gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl.mRate == 0.f) {
            wait_initial(actor);
            f32 random = boInvoke(cM_rndF, 30.f);
            actor->mTimers[1] = (s16)gabi::ftoi((f32)(random + 30.f));
        }
        break;
    }
    case 12: {
        u32 upper;
        if (actor->mHoldingPlayer == 0) {
            upper = gabi::ea(actor->mpUpperMorph.get());
            BOOL passed = boMethod(&J3DFrameCtrl::checkPass,
                                   &gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl, 33.f);
            bool setup = passed != 0;
            if (!passed) {
                upper = gabi::ea(actor->mpUpperMorph.get());
                setup = !((f32)gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl.mFrame < 33.f);
            }
            if (setup) {
                gabi::at<boPlayerView_c>(companion)->mDemo.mParam0 = 0;
                gabi::at<boPlayerView_c>(companion)->mDemo.mDemoType = 3;
                gabi::at<boPlayerView_c>(companion)->mDemo.mDemoMode = 30;
                s16 target =
                    boInvoke(fopAcM_searchActorAngleY, gabi::at<fopAc_ac_c>(player), actor);
                s16 facing = (s16)gabi::at<boPlayerView_c>(player)->shape_angle.y;
                s16 difference = boInvoke(boDistanceAngle, (s32)facing, (s32)target);
                actor->mThrowReversed = difference > 0x4000 ? 1 : 0;
                actor->mHoldingPlayer = 1;
                upper = gabi::ea(actor->mpUpperMorph.get());
            }
        } else
            upper = gabi::ea(actor->mpUpperMorph.get());
        if (!((u8)gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl.mState & 1) &&
            (f32)gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl.mRate != 0.f)
            break;
        anm_init(actor, dRes_INDEX_BO_BCK_KAMU1_e, 5.f, bo::LOOP_REPEAT, 1.f, -1, 0);
        u32 emitter = boSpawnParticleReturn(actor, 0x810c);
        if (emitter)
            boEmitterMatrix(actor, emitter, false);
        actor->mEffectTimer = 0;
        actor->mTimers[1] = 0;
        actor->mStickXDirection = 1;
        actor->mStickYDirection = 1;
        monster_sound(actor, 0x4852);
        actor->mMode = (u8)(actor->mMode + 1);
        break;
    }
    case 13: {
        u32 emitter = actor->mpHoldEmitter.v;
        if (!emitter) {
            emitter = boSpawnParticleReturn(actor, 0x8107);
            actor->mpHoldEmitter.v = emitter;
            if (emitter) {
                gabi::at<boEmitterView_c>(emitter)->mpEmtrCallBack.v =
                    gabi::ea(&actor->mSalivaCallback);
                u32 current = actor->mpHoldEmitter.v;
                gabi::at<boEmitterView_c>(current)->mRate = 0.07f;
            }
            actor->mHoldParticleState = 0;
        } else
            boEmitterMatrix(actor, emitter, false);
        if (actor->mTimers[1] == 0) {
            u32 actualPlayer = (u32)boGameInfo()->play.mpPlayerPtr[0].v;
            bool alternate = ((u32)gabi::at<boPlayerView_c>(actualPlayer)->mNoResetFlg1 & 1) != 0;
            if (!alternate)
                alternate = (s16)gabi::at<boPlayerThrowView_c>(actualPlayer)->mThrowMode != 0;
            actualPlayer = (u32)boGameInfo()->play.mpPlayerPtr[0].v;
            if (alternate)
                boInvoke(boPlayerThrowAlternate, gabi::at<void>(actualPlayer), -1.f);
            else
                boInvoke(boPlayerThrow, gabi::at<void>(actualPlayer), -1.f);
            actor->mTimers[1] = 30;
        }
        s16 count = (s16)(actor->mEffectTimer + 1);
        if (count > 120) {
            actor->mEffectTimer = 0;
            anm_init(actor, dRes_INDEX_BO_BCK_HAKIDASU1_e, 5.f, bo::LOOP_ONCE, 1.f, -1, 0);
            monster_sound(actor, 0x5846);
            u32 camera = (u32)boGameInfo()->play.mCameraInfo[0].mpCamera;
            s32 id = (s32)(u32)actor->mBsPcId;
            boInvoke(boEndEventCamera, gabi::at<boCameraView_c>(camera)->mCameraBody, id);
            actor->mMode = (u8)(actor->mMode + 1);
            break;
        }
        u32 upper = gabi::ea(actor->mpUpperMorph.get());
        actor->mEffectTimer = count;
        if (boMethod(&J3DFrameCtrl::checkPass, &gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl, 0.f)) {
            monster_sound(actor, 0x4852);
            emitter = boSpawnParticleReturn(actor, 0x810c);
            if (emitter)
                boEmitterMatrix(actor, emitter, false);
        }
        s32 direction = actor->mStickXDirection;
        if ((direction > 0 && stickX < 0.f) || (direction < 0 && stickX > 0.f)) {
            s16 current = actor->mEffectTimer;
            actor->mStickXDirection = direction > 0 ? -1 : 1;
            actor->mEffectTimer = (s16)(current + 2);
        }
        direction = actor->mStickYDirection;
        if ((direction > 0 && stickY < 0.f) || (direction < 0 && stickY > 0.f)) {
            s16 current = actor->mEffectTimer;
            actor->mStickYDirection = direction > 0 ? -1 : 1;
            actor->mEffectTimer = (s16)(current + 2);
        }
        if (boInvoke(boTriggerA, 0)) {
            s16 current = actor->mEffectTimer;
            actor->mEffectTimer = (s16)(current + 2);
        }
        break;
    }
    case 14: {
        u32 upper = gabi::ea(actor->mpUpperMorph.get());
        if (boMethod(&J3DFrameCtrl::checkPass, &gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl, 9.f)) {
            s16 angle = actor->mFacingY;
            gabi::at<boPlayerView_c>(player)->current.angle.y = angle;
            gabi::at<boPlayerView_c>(companion)->mDemo.mDemoMode = 9;
            u8 away = actor->mThrowReversed;
            actor->mHoldingPlayer = 0;
            gabi::at<boPlayerView_c>(companion)->mDemo.mParam0 = away == 0 ? 1 : 0;
        }
        upper = gabi::ea(actor->mpUpperMorph.get());
        if (((u8)gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl.mState & 1) ||
            (f32)gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl.mRate == 0.f) {
            u32 world = boPlay();
            u16 flags = (u16)gabi::at<dComIfG_inf_c>(world)->play.mEvtCtrl.mEventFlag;
            gabi::at<dComIfG_inf_c>(world)->play.mEvtCtrl.mEventFlag = flags | 8;
            wait_initial(actor);
            f32 random = boInvoke(cM_rndF, 30.f);
            s16 delay = (s16)gabi::ftoi((f32)(random + 30.f));
            actor->mTimers[1] = (s16)((s32)delay * 2);
        }
        break;
    }
    default:
        break;
    }
    if (actor->mMode < 12 && actor->mCapturePending == 0)
        damage_check(actor);
    if (actor->mCapturePending == 0)
        return;
    if (actor->eventInfo.mCommand != dEvtCmd_INDEMO_e) {
        boInvoke(boOrderEvent, actor, 2, 0xff6f, 0);
        u16 conditions = actor->eventInfo.mCondition;
        actor->eventInfo.mCondition = conditions | dEvtCnd_UNK2_e;
        return;
    }
    if (actor->mAction != 1) {
        u32 world = boPlay();
        u16 flags = (u16)gabi::at<dComIfG_inf_c>(world)->play.mEvtCtrl.mEventFlag;
        gabi::at<dComIfG_inf_c>(world)->play.mEvtCtrl.mEventFlag = flags | 8;
        actor->mCapturePending = 0;
        return;
    }
    actor->mMode = 12;
    u32 secondInitialized;
    if ((u32)l_cameraEyeInitialized)
        secondInitialized = (u32)l_cameraCenterInitialized;
    else {
        l_cameraEyeInitialized = 1;
        l_cameraEye.x = 40.f;
        secondInitialized = (u32)l_cameraCenterInitialized;
        l_cameraEye.y = 10.f;
        l_cameraEye.z = 280.f;
    }
    if (!secondInitialized) {
        l_cameraCenterInitialized = 1;
        l_cameraCenter.x = 0.f;
        l_cameraCenter.y = 50.f;
        l_cameraCenter.z = 50.f;
    }
    u32 world = boPlay();
    u32 camera = (u32)gabi::at<dComIfG_inf_c>(world)->play.mCameraInfo[0].mpCamera;
    s32 id = (s32)(u32)actor->mBsPcId;
    boInvoke(boStartEventCamera, gabi::at<boCameraView_c>(camera)->mCameraBody, 5, id,
             l_eventRelativeActor, l_eventStarter, l_eventRelativeMask, l_eventOr, l_eventTimer,
             &l_eventTimerValue, l_eventCenter, &l_cameraCenter, l_eventEye, &l_cameraEye,
             l_eventFovy, &l_eventFovyValue, 0);
    actor->mCapturePending = 0;
}

static void bo3_move(bo_class *actor) {
    u8 mode = actor->mMode;
    if (mode == 20) {
        s16 delay = actor->mDeathDelay;
        if (delay) {
            delay = (s16)(delay - 1);
            actor->mDeathDelay = delay;
        }
        if (!delay) {
            u32 cylinder = actor->mBodyCyl.mObjTg.mSPrm;
            u32 target = actor->mHeadSph.mObjCo.mSPrm;
            u32 collision = actor->mHeadSph.mObjTg.mSPrm;
            actor->mBodyCyl.mObjTg.mSPrm = cylinder & ~1u;
            actor->mHeadSph.mObjTg.mSPrm = collision & ~1u;
            actor->mHeadSph.mObjCo.mSPrm = target & ~1u;
            boMethod(&dCcD_GObjInf::ClrTgHit, &actor->mBodyCyl);
            boMethod(&dCcD_GObjInf::ClrTgHit, &actor->mHeadSph);
            gabi::Local<csXyz> angles;
            angles->x = actor->current.angle.x;
            angles->y = actor->mFacingY;
            angles->z = actor->current.angle.z;
            if (actor->mSuppressDrop == 0)
                boInvoke(boCreateActor, bo::fpcNm_BO_e, 2, &actor->mBodyPos,
                         (s32)(s8)actor->current.roomNo, angles.get(), &actor->scale, 0, 0);
            actor->mSuppressDrop = 0;
            actor->mType = 1;
            anm_init(actor, dRes_INDEX_BO_BCK_SIORERU1_e, 5.f, bo::LOOP_ONCE, 1.f, -1, 1);
            l_enemyDeathEffect = 4;
            actor->mMode = (u8)(actor->mMode + 1);
        }
    } else if (mode == 21) {
        u32 lower = gabi::ea(actor->mpLowerMorph.get());
        if (boMethod(&J3DFrameCtrl::checkPass, &gabi::at<mDoExt_McaMorf>(lower)->mFrameCtrl,
                     43.f)) {
            s32 disappearType = 0;
            if (actor->mVariant) {
                actor->mTimers[1] = 50;
                disappearType = 3;
            }
            if (actor->mDamageType != 3)
                boInvoke(boCreateDisappear, actor, &actor->current.pos, 5, disappearType,
                         (u32)actor->stealItemBitNo);
        }
        lower = gabi::ea(actor->mpLowerMorph.get());
        if (((u8)gabi::at<mDoExt_McaMorf>(lower)->mFrameCtrl.mState & 1) ||
            (f32)gabi::at<mDoExt_McaMorf>(lower)->mFrameCtrl.mRate == 0.f) {
            if (actor->mVariant)
                actor->mMode = (u8)(actor->mMode + 1);
            else {
                u32 save = (u32)l_saveInfo;
                u16 number = (u16)actor->setID;
                s32 room = (s8)(u8)actor->home.roomNo;
                boInvoke(boSaveActor, &gabi::at<boSaveView_c>(save)->mSaveInfo, (u32)number, room);
                boInvoke(fopAcM_delete, actor);
            }
        }
    } else if (mode == 22 && actor->mTimers[1] == 0) {
        boInvoke(boCreateActor, bo::fpcNm_JBO_e, 1, &actor->current.pos,
                 (s32)(s8)actor->current.roomNo, &actor->current.angle, &actor->scale, 0, 0);
        u32 save = (u32)l_saveInfo;
        u16 number = (u16)actor->setID;
        s32 room = (s8)(u8)actor->home.roomNo;
        boInvoke(boSaveActor, &gabi::at<boSaveView_c>(save)->mSaveInfo, (u32)number, room);
        boInvoke(fopAcM_delete, actor);
    }
    u32 emitter = actor->mSmokeCallback.mpEmitter.v;
    if (emitter && actor->mTimers[2] == 0) {
        u8 alpha = actor->mSmokeAlpha;
        gabi::at<boEmitterView_c>(emitter)->mGlobalPrmColor.a = alpha;
        s16 next = (s16)(actor->mEffectTimer - 4);
        actor->mEffectTimer = next;
        if (next < 0)
            boMethod(&dPa_smokeEcallBack::end, &actor->mSmokeCallback);
    }
}

/* 020C0248 */
static void bo4_move(bo_class *actor) {
    u32 emitter = 0;
    switch ((u8)actor->mMode) {
    case 30: {
        u32 co = actor->mHeadSph.mObjTg.mSPrm, tg = actor->mHeadSph.mObjCo.mSPrm,
            at = actor->mHeadSph.mObjAt.mSPrm;
        actor->mHeadSph.mObjTg.mSPrm = co & ~1u;
        actor->mHeadSph.mObjCo.mSPrm = tg & ~1u;
        actor->mHeadSph.mObjAt.mSPrm = at & ~1u;
        boMethod(&dCcD_GObjInf::ClrTgHit, &actor->mHeadSph);
        boMethod(&dCcD_GObjInf::ClrTgHit, &actor->mHeadSph);
        anm_init(actor, dRes_INDEX_BO_BCK_DAMAGE1_e, 5.f, bo::LOOP_ONCE, 1.f, -1, 0);
        actor->speed.y = 30.f;
        actor->speedF = 10.f;
        actor->gravity = -3.f;
        u32 player = (u32)boGameInfo()->play.mPlayerInfo[0].mpPlayer.v;
        s16 yaw = boInvoke(fopAcM_searchActorAngleY, actor, gabi::at<fopAc_ac_c>(player));
        f32 y = actor->current.pos.y;
        emitter = actor->mSmokeCallback.mpEmitter.v;
        u8 mode = actor->mMode;
        actor->current.angle.y = (s16)(yaw + 0x8000);
        actor->mGroundY = y - 80.f;
        actor->mMode = (u8)(mode + 1);
        break;
    }
    case 31: {
        s16 pitch = actor->shape_angle.x;
        f32 speed = actor->speed.y;
        actor->shape_angle.x = (s16)(pitch - 0x1200);
        if (speed < 0.f && actor->current.pos.y < actor->mGroundY) {
            f32 ground = actor->mGroundY;
            actor->speedF = 0;
            actor->gravity = -1.f;
            actor->mBodyHeight = 200.f;
            actor->current.pos.y = ground - 200.f;
            actor->shape_angle.x = (s16)0x8000;
            actor->speed.y = 10.f;
            effect_sound(actor, 0x5847, 0);
            u32 particles = (u32)boGameInfo()->play.mParticle.v;
            boInvoke(boSetParticle, (dPa_control_c *)(gabi::at<void>(particles)), 0, 0x810a,
                     (const cXyz *)(&actor->mTipPos), nullptr, nullptr, 0xff, nullptr, -1,
                     (const void *)(&actor->tevStr.mColorK0),
                     (const void *)(&actor->tevStr.mColorK0), nullptr);
            actor->mTimers[2] = 10;
            actor->mEffectTimer = 180;
            actor->mSmokePos.copy(actor->mTipPos);
            smoke_set(actor);
            anm_init(actor, dRes_INDEX_BO_BCK_BOU1_e, 0.f, bo::LOOP_ONCE, 1.f, -1, 0);
            emitter = actor->mSmokeCallback.mpEmitter.v;
            actor->mMode = 33;
        } else
            emitter = actor->mSmokeCallback.mpEmitter.v;
        break;
    }
    case 33: {
        s16 yaw = actor->mFacingY;
        f32 ground = actor->mGroundY;
        actor->shape_angle.z = 0x7fff;
        s16 xAdjust = (s16)l_budPitchAdjust, yAdjust = (s16)l_budYawAdjust;
        gabi::Local<csXyz> angle;
        gabi::Local<cXyz> position;
        angle->x = (s16)(xAdjust + 0x4000);
        angle->y = (s16)(yaw + 0xc000 + yAdjust);
        angle->z = 0;
        position->x = actor->mLeafPos.x;
        position->z = actor->mLeafPos.z;
        f32 extra = (f32)l_budHeightExtra;
        position->y = ((ground + 30.f) + extra) + 80.f;
        s32 room = (s8)actor->current.roomNo;
        boInvoke(boCreateActor, bo::fpcNm_BOKO_e, 8, position.get(), room, angle.get(),
                 &actor->scale, 0, 0);
        u8 mode = actor->mMode;
        actor->scale.y = 0;
        actor->scale.z = 0;
        actor->mMode = (u8)(mode + 1);
        actor->mTargetScaleY = 0;
        actor->scale.x = 0;
        actor->mTimers[1] = 10;
        emitter = actor->mSmokeCallback.mpEmitter.v;
        break;
    }
    case 34:
        if (actor->mTimers[1] == 0) {
            emitter = actor->mSmokeCallback.mpEmitter.v;
            if (!emitter) {
                boInvoke(fopAcM_delete, actor);
                emitter = actor->mSmokeCallback.mpEmitter.v;
            }
        } else
            emitter = actor->mSmokeCallback.mpEmitter.v;
        break;
    default:
        emitter = actor->mSmokeCallback.mpEmitter.v;
        break;
    }
    if (emitter && actor->mTimers[2] == 0) {
        u8 alpha = actor->mSmokeAlpha;
        gabi::at<boEmitterView_c>(emitter)->mGlobalPrmColor.a = alpha;
        s16 life = (s16)(actor->mEffectTimer - 4);
        actor->mEffectTimer = life;
        if (life < 0)
            boMethod(&dPa_smokeEcallBack::end, &actor->mSmokeCallback);
    }
}

/* 020C0624 */
static void bo5_move(bo_class *actor) {
    switch ((u8)actor->mMode) {
    case 40: {
        u32 tg = actor->mHeadSph.mObjCo.mSPrm, co = actor->mHeadSph.mObjTg.mSPrm,
            body = actor->mBodyCyl.mObjTg.mSPrm;
        actor->mHeadSph.mObjCo.mSPrm = tg & ~1u;
        actor->mEffectTimer = 3;
        actor->mHeadSph.mObjTg.mSPrm = co & ~1u;
        actor->mBodyCyl.mObjTg.mSPrm = body & ~1u;
        boMethod(&dCcD_GObjInf::ClrTgHit, &actor->mBodyCyl);
        boMethod(&dCcD_GObjInf::ClrTgHit, &actor->mHeadSph);
        l_enemyDeathEffect = 2;
        anm_init(actor, dRes_INDEX_BO_BCK_KUSIZASI1_e, 5.f, bo::LOOP_REPEAT, 1.f, -1, 0);
        actor->attention_info.flags = actor->attention_info.flags & ~(u32)fopAc_Attn_LOCKON_BATTLE_e;
        monster_sound(actor, 0x4850);
        effect_sound(actor, 0x2828, 0x20);
        actor->mMode = (u8)(actor->mMode + 1);
        return;
    }
    case 41: {
        mDoExt_McaMorf *upper = actor->mpUpperMorph.get();
        if (!boMethod(&J3DFrameCtrl::checkPass, &upper->mFrameCtrl, 15.f))
            return;
        s16 count = (s16)(actor->mEffectTimer - 1);
        actor->mEffectTimer = count;
        if (count > 0)
            return;
        actor->attention_info.flags = actor->attention_info.flags & ~(u32)fopAc_Attn_LOCKON_BATTLE_e;
        anm_init(actor, dRes_INDEX_BO_BCK_DERU_U1_e, 0.f, bo::LOOP_ONCE, 0.f, -1, 0);
        u32 particles = (u32)boGameInfo()->play.mParticle.v;
        boInvoke(boSetParticle, (dPa_control_c *)(gabi::at<void>(particles)), 0, 0x810a,
                 (const cXyz *)(&actor->mTipPos), nullptr, nullptr, 0xff, nullptr, -1,
                 (const void *)(&actor->tevStr.mColorK0), (const void *)(&actor->tevStr.mColorK0),
                 nullptr);
        actor->mTimers[2] = 10;
        actor->mSmokePos.copy(actor->mTipPos);
        actor->mEffectTimer = 180;
        smoke_set(actor);
        effect_sound(actor, 0x5801, 0);
        actor->mSuppressDrop = 1;
        actor->mAction = 2;
        actor->mMode = 20;
        return;
    }
    case 50: {
        u32 body = actor->mBodyCyl.mObjTg.mSPrm, co = actor->mHeadSph.mObjTg.mSPrm,
            tg = actor->mHeadSph.mObjCo.mSPrm;
        actor->mBodyCyl.mObjTg.mSPrm = body & ~1u;
        actor->mHeadSph.mObjTg.mSPrm = co & ~1u;
        actor->mHeadSph.mObjCo.mSPrm = tg & ~1u;
        boMethod(&dCcD_GObjInf::ClrTgHit, &actor->mBodyCyl);
        boMethod(&dCcD_GObjInf::ClrTgHit, &actor->mHeadSph);
        l_enemyDeathEffect = 2;
        anm_init(actor, dRes_INDEX_BO_BCK_KARERU1_e, 5.f, bo::LOOP_ONCE, 1.f, -1, 0);
        actor->attention_info.flags = 0;
        monster_sound(actor, 0x4850);
        effect_sound(actor, 0x2828, 0x20);
        actor->mMode = (u8)(actor->mMode + 1);
        [[fallthrough]];
    }
    case 51: {
        if (actor->mTimers[0] != 0)
            return;
        u32 upper = gabi::ea(actor->mpUpperMorph.get());
        if (!((u8)gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl.mState & 1) &&
            (f32)gabi::at<mDoExt_McaMorf>(upper)->mFrameCtrl.mRate != 0.f)
            return;
        actor->mTimers[2] = 10;
        actor->mEffectTimer = 180;
        actor->mSuppressDrop = 1;
        actor->mAction = 2;
        actor->mMode = 20;
        return;
    }
    default:
        return;
    }
}

/* 020BCEC8 */
static BOOL daBO_Execute(bo_class *actor) {
    if (actor->mType == 0 && boInvoke(boEnemyIce, &actor->mEnemyIce)) {
        if ((s8)actor->mEnemyIce.mLightShrinkTimer) {
            u32 morph = gabi::ea(actor->mpUpperMorph.get());
            copy_base_matrix((u32)gabi::at<mDoExt_McaMorf>(morph)->mpModel.v);
            boMethod(&mDoExt_McaMorf::calc, actor->mpUpperMorph.get());
        } else
            boRegisterBody(actor);
        return 1;
    }
    for (u32 index = 0; index < 5; ++index) {
        be<s16> *timer = &actor->mTimers[index];
        s16 remaining = *timer;
        if (remaining)
            *timer = (s16)(remaining - 1);
    }
    switch ((u8)actor->mAction) {
    case 0:
        bo_move(actor);
        break;
    case 1:
        bo2_move(actor);
        break;
    case 2:
        bo3_move(actor);
        break;
    case 3:
        bo4_move(actor);
        break;
    case 4:
        bo5_move(actor);
        break;
    default:
        break;
    }
    boExecuteFinish(actor);
    return 1;
}

/* 020BF56C */
static BOOL daBO_IsDelete(bo_class *actor) { return 1; }

/* 020BF574 */
static BOOL daBO_Delete(bo_class *actor) {
    u32 vtable = actor->mSmokeCallback.__vtbl;
    boVirtualCall(gabi::load<u32>(vtable + 0x44), &actor->mSmokeCallback);
    u32 emitter = actor->mpEmergingEmitter.v;
    if (emitter) {
        u32 flags = (u32)gabi::at<boEmitterView_c>(emitter)->mStatus;
        gabi::at<boEmitterView_c>(emitter)->mMaxFrame = 0xffffffff;
        gabi::at<boEmitterView_c>(emitter)->mStatus = flags | 1;
        actor->mpEmergingEmitter.v = 0;
    }
    emitter = actor->mpWaitEmitter.v;
    if (emitter) {
        boInvoke(boDeleteParticles, gabi::at<void>(emitter), 1);
        emitter = actor->mpWaitEmitter.v;
        u32 flags = (u32)gabi::at<boEmitterView_c>(emitter)->mStatus;
        gabi::at<boEmitterView_c>(emitter)->mMaxFrame = 0xffffffff;
        gabi::at<boEmitterView_c>(emitter)->mStatus = flags | 1;
        actor->mpWaitEmitter.v = 0;
    }
    emitter = actor->mpHoldEmitter.v;
    if (emitter) {
        boInvoke(boDeleteParticles, gabi::at<void>(emitter), 1);
        emitter = actor->mpHoldEmitter.v;
        u32 flags = (u32)gabi::at<boEmitterView_c>(emitter)->mStatus;
        gabi::at<boEmitterView_c>(emitter)->mMaxFrame = 0xffffffff;
        gabi::at<boEmitterView_c>(emitter)->mStatus = flags | 1;
        actor->mpHoldEmitter.v = 0;
    }
    boInvoke(boRemoveEnemyFire, &actor->mEnemyFire);
    boInvoke(boDeleteResource, actor->mPhase, (char *)l_deleteArcName);
    return 1;
}

/* 020BF65C */
static BOOL useHeapInit(bo_class *actor) {
    s32 animation = actor->mType == 2 ? dRes_INDEX_BO_BCK_DAMAGE1_e : dRes_INDEX_BO_BCK_DERU_U1_e;
    void *modelResource = boResource(dRes_INDEX_BO_BDL_BO_UE1_e, (const char *)l_heapArcName);
    void *animationResource = boResource(animation, (const char *)l_heapArcName);
    mDoExt_McaMorf *upper = boInvoke(boConstructMorph, 0, modelResource, 0, 0, animationResource, 1,
                                     0, -1, 0.f, 1, 0, 0x80000, 0x37441422);
    actor->mpUpperMorph = upper;
    if (!upper || !(u32)upper->mpModel.v)
        return 0;
    u32 model = (u32)upper->mpModel.v;
    gabi::at<J3DModel>(model)->mUserArea = gabi::ea(actor);
    model = (u32)(actor->mpUpperMorph.get())->mpModel.v;
    void *joints = boInvoke(boGetJointTree,
                            gabi::at<J3DModelData>((u32)gabi::at<J3DModel>(model)->mModelData.v));
    u16 index = 0;
    while (index < (u16)((boJointTreeView_c *)joints)->mJointNum) {
        model = (u32)(actor->mpUpperMorph.get())->mpModel.v;
        u32 data = (u32)gabi::at<J3DModel>(model)->mModelData.v,
            count = (u32)gabi::at<J3DModelData>(data)->mJointNum,
            table = (u32)gabi::at<J3DModelData>(data)->mJointNodes;
        if (index < count)
            table += index * 0x1c;
        gabi::at<J3DJoint>(table)->mCallBack = gabi::guest_fn((const void *)nodeCallBack_UP);
        index = (u16)(index + 1);
        model = (u32)(actor->mpUpperMorph.get())->mpModel.v;
        joints = boInvoke(boGetJointTree,
                          gabi::at<J3DModelData>((u32)gabi::at<J3DModel>(model)->mModelData.v));
    }
    if (actor->mType == 2) {
        model = (u32)(actor->mpUpperMorph.get())->mpModel.v;
        void *brk = boInvoke(boAllocate, 0x78);
        if (brk)
            brk = boInvoke(boConstructColorAnimation, brk);
        actor->mpColorAnm = brk;
        if (!brk)
            return 0;
        void *resource = boResource(dRes_INDEX_BO_BRK_TO_BOKO1_e, (const char *)l_heapArcName);
        u32 data = (u32)gabi::at<J3DModel>(model)->mModelData.v;
        brk = actor->mpColorAnm.get();
        if (!boInvoke(boInitColorAnimation, brk, gabi::at<void>(data), resource, 1, 0, 0, -1, 0,
                      1.f, 0))
            return 0;
    } else {
        modelResource = boResource(dRes_INDEX_BO_BDL_BO_SITA1_e, (const char *)l_heapArcName);
        animationResource = boResource(dRes_INDEX_BO_BCK_DERU_S1_e, (const char *)l_heapArcName);
        mDoExt_McaMorf *lower =
            boInvoke(boConstructMorph, 0, modelResource, 0, 0, animationResource, 1, 0, -1, 0.f, 1,
                     0, 0x80000, 0x33221202);
        actor->mpLowerMorph = lower;
        if (!lower || !(u32)lower->mpModel.v)
            return 0;
        model = (u32)lower->mpModel.v;
        gabi::at<J3DModel>(model)->mUserArea = gabi::ea(actor);
        model = (u32)(actor->mpLowerMorph.get())->mpModel.v;
        joints = boInvoke(boGetJointTree,
                          gabi::at<J3DModelData>((u32)gabi::at<J3DModel>(model)->mModelData.v));
        index = 0;
        while (index < (u16)((boJointTreeView_c *)joints)->mJointNum) {
            model = (u32)(actor->mpLowerMorph.get())->mpModel.v;
            u32 data = (u32)gabi::at<J3DModel>(model)->mModelData.v,
                count = (u32)gabi::at<J3DModelData>(data)->mJointNum,
                table = (u32)gabi::at<J3DModelData>(data)->mJointNodes;
            if (index < count)
                table += index * 0x1c;
            index = (u16)(index + 1);
            gabi::at<J3DJoint>(table)->mCallBack = gabi::guest_fn((const void *)nodeCallBack_DW);
            model = (u32)(actor->mpLowerMorph.get())->mpModel.v;
            joints = boInvoke(boGetJointTree,
                              gabi::at<J3DModelData>((u32)gabi::at<J3DModel>(model)->mModelData.v));
        }
    }
    if (actor->mType == 0) {
        model = (u32)(actor->mpUpperMorph.get())->mpModel.v;
        if (!boInvoke(boCreateInvisibleModel, actor->mInvisibleModel, gabi::at<void>(model)))
            return 0;
    }
    return 1;
}

/* 020BFC50 */
static s32 daBO_Create(bo_class *actor) {
    u32 status = actor->actor_condition;
    if (!(status & fopAcCnd_INIT_e)) {
        if (actor) {
            actor_constructor(actor);
            status = actor->actor_condition;
        }
        actor->actor_condition = status | fopAcCnd_INIT_e;
    }
    s32 phase = boInvoke(boLoadResource, actor->mPhase, (char *)l_createArcName);
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    u32 parameters = actor->mParameters;
    u8 kind = (u8)parameters, variant = (u8)(parameters >> 8);
    actor->mType = kind;
    actor->mVariant = variant;
    if (kind == 0xff) {
        variant = actor->mVariant;
        actor->mType = 0;
    }
    if (variant == 0xff)
        actor->mVariant = 0;
    u32 play = boPlay();
    s32 figure = boInvoke(boFigureIndex, gabi::at<boNameTableView_c>(play)->mNameTable,
                          (char *)l_figureName, 0);
    actor->itemTableIdx = figure;
    if (!boInvoke(boEntrySolidHeap, actor, gabi::guest_fn((const void *)useHeapInit), 0x3100))
        return cPhs_ERROR_e;
    kind = actor->mType;
    u32 morph = kind == 0 || kind == 2 ? gabi::ea(actor->mpUpperMorph.get())
                                       : gabi::ea(actor->mpLowerMorph.get());
    u32 model = (u32)gabi::at<mDoExt_McaMorf>(morph)->mpModel.v;
    actor->cullMtx = model ? model + 0xc8 : 0;
    boInvoke(boSetCullBox, actor, -150.f, 0.f, -150.f, 150.f, 330.f, 150.f);
    actor->attention_info.flags = 0;
    actor->max_health = 1;
    actor->health = 1;
    actor->mTargetScaleY = 1.f;
    dCcD_Stts *collisionStatus = &actor->mStts;
    boMethod(&dCcD_Stts::Init, collisionStatus, 0xff, 1, actor);
    kind = actor->mType;
    actor->mAction = 0;
    actor->mMode = 0;
    if (kind == 0) {
        boMethod(&dCcD_Sph::Set, &actor->mHeadSph, &head_co_sph_src);
        actor->mHeadSph.mStts = collisionStatus;
        actor->stealItemLeft = 3;
        boMethod(&dCcD_Cyl::Set, &actor->mBodyCyl, &body_cyl_src);
        actor->mEnemyFire.mpMcaMorf.v = actor->mpUpperMorph.v;
        actor->mBodyCyl.mStts = collisionStatus;
        u32 attack = actor->mHeadSph.mObjAt.mSPrm;
        actor->mHeadSph.mGObjAt.mSpl = 0;
        actor->mEnemyIce.mWallRadius = 15.f;
        actor->mEnemyFire.mpActor = actor;
        actor->mEnemyIce.mpActor = actor;
        actor->mHeadSph.mObjAt.mSPrm = attack & ~1u;
        actor->mEnemyIce.mCylHeight = 160.f;
        for (u32 i = 0; i < 10; ++i) {
            actor->mEnemyFire.mFlameJntIdxs[i] = (u8)l_fireJoints[i];
            actor->mEnemyFire.mParticleScale[i] = (f32)l_fireScales[i];
        }
        kind = actor->mType;
    }
    if (kind == 0 || kind == 1) {
        boMethod(&dCcD_Sph::Set, &actor->mFootSph, &foot_co_sph_src);
        kind = actor->mType;
        actor->mFootSph.mStts = collisionStatus;
        if (kind == 1) {
            s16 timer = (s16)gabi::ftoi((f32)l_initialWaitExtra + 15.f);
            actor->mTimers[0] = timer;
            anm_init(actor, dRes_INDEX_BO_BCK_DERU_S1_e, 0.f, bo::LOOP_ONCE, 0.f, -1, 1);
            mDoExt_McaMorf *lower = actor->mpLowerMorph.get();
            actor->mAction = 4;
            actor->mMode = 51;
            actor->mDamageType = 3;
            boMethod(&mDoExt_McaMorf::play, lower, nullptr, 0, 0);
            draw_SUB(actor);
        }
    }
    play = boPlay();
    u32 player = (u32)gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v;
    s16 yaw = boInvoke(fopAcM_searchActorAngleY, actor, gabi::at<fopAc_ac_c>(player));
    actor->mBodyPos.copy(actor->current.pos);
    kind = actor->mType;
    actor->mTargetFacingY = yaw;
    actor->mFacingY = yaw;
    if (kind == 2) {
        actor->attention_info.flags = 0;
        mDoExt_McaMorf *upper = actor->mpUpperMorph.get();
        actor->mBodyHeight = 100.f;
        actor->mAction = 3;
        actor->mMode = 30;
        boMethod(&mDoExt_McaMorf::play, upper, nullptr, 0, 0);
        draw_SUB(actor);
    }
    daBO_Execute(actor);
    return phase;
}

// HD: compiler-generated and template code of this translation unit.

/* 020BF9C0 */
static void *enemy_fire_constructor(void *object) {
    if (!object)
        object = boInvoke(boAllocate, 0x22c);
    if (object) {
        enemyfire *fire = (enemyfire *)object;
        // HD: the optional vector allocation uses wrapped guest address arithmetic.
        u32 directionAddress = gabi::ea(object) + offsetof(enemyfire, mDirection);
        if (!directionAddress)
            boInvoke(boAllocate, sizeof(cXyz));
        boInvoke(boConstructBaseCollisionStatus, &fire->mStts);
        boInvoke(boConstructGlobalCollisionStatus, &fire->mStts.__vtbl_gstts);
        fire->mStts.__vtbl = gabi::ea(&l_collisionStatusVtable);
        fire->mStts.__vtbl_gstts = gabi::ea(&l_globalCollisionStatusVtable);
        boInvoke(boConstructSphere, &fire->mSph);
        fire->mLight.mHD20 = 1.f;
    }
    return object;
}

/* 020BFA4C */
static bo_class *actor_constructor(bo_class *actor) {
    if (!actor)
        actor = (bo_class *)boInvoke(boAllocate, sizeof(bo_class));
    if (!actor)
        return actor;
    boInvoke(fopAc_ac_c_ct, actor);
    actor->__vtbl = gabi::ea(&l_actorVtable);
    boInvoke(boConstructSmoke, &actor->mSmokeCallback, 1);
    actor->mSalivaCallback.__vtbl = gabi::ea(&l_salivaCallbackVtable);
    boInvoke(boConstructGroundCheck, &actor->mSalivaCallback.mGroundCheck);
    actor->mSalivaCallback.mGroundCheck.mbBombThrough = 0;
    actor->mSalivaCallback.mGroundCheck.mbArrowThrough = 0;
    actor->mSalivaCallback.mGroundCheck.mPolyPassChk =
        gabi::ea((dBgS_PolyPassChk *)&actor->mSalivaCallback.mGroundCheck);
    actor->mSalivaCallback.mGroundCheck.mbRopeThrough = 0;
    actor->mSalivaCallback.mGroundCheck.cBgS_PolyInfo::__vtbl = gabi::ea(&l_groundPolyInfoVtable);
    actor->mSalivaCallback.mGroundCheck.mbCamThrough = 0;
    actor->mSalivaCallback.mGroundCheck.cBgS_Chk::__vtbl = gabi::ea(&l_groundCheckVtable);
    actor->mSalivaCallback.mGroundCheck.mbObjThrough = 1;
    actor->mSalivaCallback.mGroundCheck.dBgS_PolyPassChk::__vtbl =
        gabi::ea(&l_groundPolyPassVtable);
    actor->mSalivaCallback.mGroundCheck.dBgS_GrpPassChk::__vtbl =
        gabi::ea(&l_groundGroupPassVtable);
    actor->mSalivaCallback.mGroundCheck.mbBoomerangThrough = 0;
    actor->mSalivaCallback.mGroundCheck.mbLinkThrough = 0;
    actor->mSalivaCallback.mGroundCheck.mGrp = 1;
    actor->mSalivaCallback.mGroundCheck.mGrpPassChk =
        gabi::ea((dBgS_GrpPassChk *)&actor->mSalivaCallback.mGroundCheck);
    boInvoke(dBgS_AcchCir_ct, &actor->mAcchCir);
    boInvoke(boConstructAcch, &actor->mAcch);
    actor->mAcch.dBgS_GrpPassChk::__vtbl = gabi::ea(&l_acchGroupPassVtable);
    actor->mAcch.dBgS_PolyPassChk::__vtbl = gabi::ea(&l_acchPolyPassVtable);
    actor->mAcch.mbObjThrough = 1;
    actor->mAcch.cBgS_Chk::__vtbl = gabi::ea(&l_acchCheckVtable);
    boInvoke(boConstructBaseCollisionStatus, &actor->mStts);
    boInvoke(boConstructGlobalCollisionStatus, &actor->mStts.__vtbl_gstts);
    actor->mStts.__vtbl = gabi::ea(&l_collisionStatusVtable);
    actor->mStts.__vtbl_gstts = gabi::ea(&l_globalCollisionStatusVtable);
    boInvoke(boConstructSphere, &actor->mHeadSph);
    boInvoke(boConstructSphere, &actor->mFootSph);
    boInvoke(boConstructCollisionObject, &actor->mBodyCyl);
    boShapeAttr(actor->mBodyCyl.mShapeAttr)->mShapeVtable = gabi::ea(&l_cylinderShapeVtable);
    boShapeAttr(actor->mBodyCyl.mShapeAttr)->mAabVtable = gabi::ea(&l_shapeBoundsVtable);
    boInvoke(boConstructCylinderGeometry, &actor->mBodyCyl.mCyl);
    actor->mBodyCyl.__vtbl_hitinf = gabi::ea(&l_collisionHitInfoVtable);
    boShapeAttr(actor->mBodyCyl.mShapeAttr)->mShapeVtable =
        gabi::ea(&l_collisionCylinderShapeVtable);
    actor->mBodyCyl.mCyl.__vtbl = gabi::ea(&l_collisionCylinderVtable);
    boInvoke(boConstructBaseCollisionStatus, &actor->mEnemyIce.mStts);
    boInvoke(boConstructGlobalCollisionStatus, &actor->mEnemyIce.mStts.__vtbl_gstts);
    actor->mEnemyIce.mStts.__vtbl = gabi::ea(&l_collisionStatusVtable);
    actor->mEnemyIce.mStts.__vtbl_gstts = gabi::ea(&l_globalCollisionStatusVtable);
    boInvoke(boConstructCollisionObject, &actor->mEnemyIce.mCyl);
    boShapeAttr(actor->mEnemyIce.mCyl.mShapeAttr)->mShapeVtable = gabi::ea(&l_cylinderShapeVtable);
    boShapeAttr(actor->mEnemyIce.mCyl.mShapeAttr)->mAabVtable = gabi::ea(&l_shapeBoundsVtable);
    boInvoke(boConstructCylinderGeometry, &actor->mEnemyIce.mCyl.mCyl);
    actor->mEnemyIce.mCyl.__vtbl_hitinf = gabi::ea(&l_collisionHitInfoVtable);
    boShapeAttr(actor->mEnemyIce.mCyl.mShapeAttr)->mShapeVtable =
        gabi::ea(&l_collisionCylinderShapeVtable);
    actor->mEnemyIce.mCyl.mCyl.__vtbl = gabi::ea(&l_collisionCylinderVtable);
    boInvoke(dBgS_AcchCir_ct, &actor->mEnemyIce.mBgAcchCir);
    boInvoke(boConstructAcch, &actor->mEnemyIce.mBgAcch);
    actor->mEnemyIce.mBgAcch.dBgS_GrpPassChk::__vtbl = gabi::ea(&l_acchGroupPassVtable);
    actor->mEnemyIce.mBgAcch.cBgS_Chk::__vtbl = gabi::ea(&l_acchCheckVtable);
    actor->mEnemyIce.mBgAcch.mbObjThrough = 1;
    actor->mEnemyIce.mBgAcch.dBgS_PolyPassChk::__vtbl = gabi::ea(&l_acchPolyPassVtable);
    enemy_fire_constructor(&actor->mEnemyFire);
    boInvoke(boConstructInvisibleModel, actor->mInvisibleModel);
    return actor;
}

/* 020C018C */
static void bo_static_init() {
    l_headerWord2 = 0;
    l_headerWord0 = 0;
    l_headerWord3 = 0;
    l_headerWord1 = 0;
    boInvoke(boRegisterGlobal, &l_headerWordRegistration);
    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;
    boInvoke(boConstructHeaderByte, &l_headerByte);
    boInvoke(boRegisterGlobal, &l_headerByteRegistration);
    boInvoke(boConstructHeaderFlag, &l_headerFlag);
    boInvoke(boRegisterGlobal, &l_headerFlagRegistration);
}

/* 020C0A94 */
static u32 particle_list_count(void *list) { return ((boParticleListView_c *)list)->mNum; }

/* 020C0A9C */
static void callback_empty0(void *p) {}

/* 020C0AA0 */
static void callback_empty1(void *p) {}

/* 020C0AA4 */
static void callback_empty2(void *p) {}

/* 020C0BF8 */
static void empty_virtual(void *p) {}

/* 020C0220 */
static void emitter_set_flags(void *emitter, u32 flags) {
    boEmitterView_c *state = (boEmitterView_c *)emitter;
    state->mStatus = state->mStatus | flags;
}

/* 020C0230 */
static void trivial_destructor(void *object, u32 flags) {
    if (object && (flags & 1))
        boInvoke(boDeallocate, object);
}

/* 020C0244 */
static void *identity_pointer(void *p) { return p; }

/* 020C0AA8 */
static void actor_destructor(bo_class *actor, u32 flags) {
    if (!actor)
        return;
    boInvoke(boDestroyInvisibleModel, actor->mInvisibleModel, 2);
    boInvoke(boDestroySphere, &actor->mEnemyFire.mSph, 2);
    boInvoke(dCcD_Stts_dt, &actor->mEnemyFire.mStts, 2);
    actor->mEnemyIce.mBgAcch.dBgS_GrpPassChk::__vtbl = gabi::ea(&l_acchGroupPassVtable);
    actor->mEnemyIce.mBgAcch.dBgS_PolyPassChk::__vtbl = gabi::ea(&l_acchPolyPassVtable);
    boInvoke(boDestroyAcch, &actor->mEnemyIce.mBgAcch, 0);
    boInvoke(boDestroyCircle, &actor->mEnemyIce.mBgAcchCir.m_cir, 2);
    boInvoke(dCcD_Cyl_dt, &actor->mEnemyIce.mCyl, 2);
    boInvoke(dCcD_Stts_dt, &actor->mEnemyIce.mStts, 2);
    boInvoke(dCcD_Cyl_dt, &actor->mBodyCyl, 2);
    boInvoke(boDestroySphere, &actor->mFootSph, 2);
    boInvoke(boDestroySphere, &actor->mHeadSph, 2);
    boInvoke(dCcD_Stts_dt, &actor->mStts, 2);
    actor->mAcch.dBgS_GrpPassChk::__vtbl = gabi::ea(&l_acchGroupPassVtable);
    actor->mAcch.dBgS_PolyPassChk::__vtbl = gabi::ea(&l_acchPolyPassVtable);
    boInvoke(boDestroyAcch, &actor->mAcch, 0);
    boInvoke(boDestroyCircle, &actor->mAcchCir.m_cir, 2);
    actor->mSalivaCallback.mGroundCheck.cBgS_PolyInfo::__vtbl = gabi::ea(&l_polyInfoVtable);
    actor->mSalivaCallback.mGroundCheck.dBgS_PolyPassChk::__vtbl = gabi::ea(&l_polyPassVtable);
    actor->mSalivaCallback.mGroundCheck.dBgS_GrpPassChk::__vtbl = gabi::ea(&l_groupPassVtable);
    boInvoke(boDestroyGroundCheck, &actor->mSalivaCallback.mGroundCheck, 0);
    boInvoke(boDestroyActor, actor, 0);
    if (flags & 1)
        boInvoke(boDeallocate, actor);
}

// clang-format off
/* WWHD glue
data l_eventRelativeActor 1000A194
data l_eventStarter 1000A1A0
data l_eventRelativeMask 1000A1AC
data l_eventOr 1000A084
data l_eventTimer 1000A07C
data l_eventCenter 1000A08C
data l_eventEye 1000A078
data l_eventFovy 1000A094
data l_deleteArcName 1000A25C
data l_createArcName 1000A27C
data l_figureName 1000A274
data l_eventTimerValue 10191D30
data l_eventFovyValue 10191D2C
data l_collisionStatusVtable 1004AE88
data l_globalCollisionStatusVtable 1004AEC0
data l_actorVtable 1000A184
data l_salivaCallbackVtable 1000A290
data l_groundPolyInfoVtable 1000A124
data l_groundCheckVtable 1000A114
data l_groundPolyPassVtable 1000A144
data l_groundGroupPassVtable 1000A134
data l_acchGroupPassVtable 1000A164
data l_acchPolyPassVtable 1000A174
data l_acchCheckVtable 1000A154
data l_cylinderShapeVtable 100015A8
data l_shapeBoundsVtable 1000A0B4
data l_collisionHitInfoVtable 1004B108
data l_collisionCylinderShapeVtable 1004B160
data l_collisionCylinderVtable 1004B150
data l_polyInfoVtable 1000A0E4
data l_polyPassVtable 1000A104
data l_groupPassVtable 1000A0C4
data l_headerWordRegistration 10191EAC
data l_headerByteRegistration 10191EB8
data l_headerFlagRegistration 10191EC4
ext 028F026C void boRegisterGlobal(void* record)
ext 028ED6F8 void boConstructHeaderByte(void* byte)
ext 028EAB2C void boConstructHeaderFlag(void* flag)
ext 02824890 void boSetEmitterTransform(void* source, void* rotation, void* scale, void* translation)
ext 028249B0 void boSetEmitterRotation(void* source, void* rotation, void* translation)
ext 023F4CB0 void boPlayerThrowAlternate(void* player, f32 speed)
ext 023F51D0 void boPlayerThrow(void* player, f32 speed)
ext 0253E70C void boStartEventCamera(void* camera, s32 type, s32 actorId, void* mode, void* subtype, void* argument5, void* argument6, void* argument7, void* argument8, void* argument9, void* center, void* argument11, void* eye, void* argument13, void* argument14, s32 end)
ext 025BEBB8 void boRegisterSnap(s32 type, bo_class* actor, cXyz* position, s32 yaw, f32 scaleX, f32 scaleY, f32 scaleZ)
ext 0259138C void boDrawIce(mDoExt_McaMorf* morph, s32 index, void* invisibleModel)
ext 025E4F64 mDoExt_McaMorf* boConstructMorph(s32 object, void* model, s32 callbackA, s32 callbackB, void* animation, s32 mode, s32 start, s32 end, f32 rate, s32 argument9, s32 argument10, u32 flags, u32 sound)
ext 025E80D0 void* boConstructColorAnimation(void* animation)
ext 025E8154 s32 boInitColorAnimation(void* animation, void* modelData, void* resource, s32 mode, s32 argument4, s32 start, s32 end, s32 argument7, f32 speed, s32 argument9)
ext 025E8A48 s32 boCreateInvisibleModel(void* invisibleModel, void* model)
ext 0200E814 s32 boFigureIndex(void* table, char* name, s32 flag)
ext 0200FAAC s16 boDistanceAngle(s32 first, s32 second)
ext 025D5834 void boCreateActor(s32 process, u32 parameters, cXyz* position, s32 room, csXyz* angle, cXyz* scale, s32 parent, s32 subtype)
ext 025D99E8 void boCreateDisappear(bo_class* actor, cXyz* position, s32 size, s32 type, u32 itemBit)
ext 025BA5D4 void boSaveActor(void* save, u32 actorId, s32 room)
ext 025192A8 void boCheckAttack(bo_class* actor, void* hitInfo)
ext 02520460 s32 boLoadResource(void* phase, char* archive)
ext 025204C8 void boDeleteResource(void* phase, char* archive)
ext 025D6800 void boMoveActor(bo_class* actor, dCcD_Stts* status)
ext 0253E860 void boEndEventCamera(void* camera, s32 actorId)
ext 025D7B24 void boOrderEvent(bo_class* actor, s32 eventId, s32 priority, s32 flags)
ext 025A5B18 void boConstructSmoke(dPa_smokeEcallBack* callback, s32 flags)
ext 025E895C void boConstructInvisibleModel(void* model)
ext 025E89F8 void boDestroyInvisibleModel(void* model, s32 flags)
ext 025D50BC void boDestroyActor(bo_class* actor, s32 flags)
ext 025E742C void boPlayColorAnimation(void* animation)
ext 025E83FC void boEntryColorAnimation(void* animation, void* modelData, f32 frame)
ext 025D674C void boSetCullBox(bo_class* actor, f32 minX, f32 minY, f32 minZ, f32 maxX, f32 maxY, f32 maxZ)
ext 025A847C void* boSetParticle(dPa_control_c* control, s32 group, u32 effect, const cXyz* position, const csXyz* angle, const cXyz* scale, s32 alpha, void* callback, s32 room, const void* primary, const void* environment, const cXyz* scale2D)
ext 0200E240 void boSetCollision(cCcS* collision, dCcD_GObjInf* object)
ext 02520540 s8 boReverb(s32 room)
ext 025E1AA4 void boMonsterSound(u32 sound, void* position, u32 actorId, s32 flags, s32 reverb)
ext 025E1A40 void boEffectSound(u32 sound, void* position, u32 flags, s32 reverb)
ext 025E1FD8 void boStopBattleSound()
ext 0281DE68 void boDeleteParticles(void* emitter, s32 flags)
ext 02008E0C void boConstructGroundCheck(dBgS_GndChk* check)
ext 02008DAC void boDestroyGroundCheck(dBgS_GndChk* check, s32 flags)
ext 020402C8 BOOL boEnemyIce(enemyice* ice)
ext 02041570 void boEnemyFire(enemyfire* fire)
ext 02041C30 void boRemoveEnemyFire(enemyfire* fire)
ext 0200796C f32 boStickX(s32 port)
ext 02007990 f32 boStickY(s32 port)
ext 02007898 BOOL boTriggerA(s32 port)
ext 027F7878 J3DJoint* boGetJointData(J3DNode* node)
ext 027F3F94 void* boGetJointTree(J3DModelData* modelData)
data l_currentMatrix 104B4868
data head_co_sph_src 10191DB4
data foot_co_sph_src 10191DF4
data body_cyl_src 10191E68
data l_baseMatrix 1048D0CC
data l_currentModel 104B462C
data l_matrixStack 1018C7B0
data l_disableDraw 1047BDE2
data l_initialWaitExtra 1047BAC8
data l_mouthYawAdjust 1047BD4E
data l_mouthPitchAdjust 1047BD52
data l_budPitchAdjust 1047C0AC
data l_budYawAdjust 1047C0AA
data l_budHeightExtra 1047C030
data l_enemyDeathEffect 101EACB7
data l_invincibilityExtra 1047BAB8
data l_deathDelayExtra 1047BB12
data l_fireDurationExtra 1047BAC0
data l_iceDurationExtra 1047BAC4
data l_disableRecoil 1047BABC
data l_saveInfo 101F84DC
ext 0273AD10 void* boAllocate(s32 size)
ext 0273AF40 void boDeallocate(void* object)
ext 025D63E8 s32 boEntrySolidHeap(bo_class* actor, u32 callback, s32 size)
ext 0200BD2C void boConstructBaseCollisionStatus(dCcD_Stts* status)
ext 02515DA0 void boConstructGlobalCollisionStatus(void* status)
ext 02515FB8 void boConstructCollisionObject(dCcD_GObjInf* collider)
ext 025166F0 void boConstructSphere(dCcD_Sph* sphere)
ext 02018590 void boConstructCylinderGeometry(cM3dGCyl* cylinder)
ext 024F0474 void boConstructAcch(dBgS_Acch* acch)
ext 02515AE8 void boDestroySphere(dCcD_Sph* sphere, s32 flags)
ext 024EFD9C void boDestroyAcch(dBgS_Acch* acch, s32 flags)
ext 02018034 void boDestroyCircle(cM3dGCir_l* circle, s32 flags)
data l_minAngle 10462534
data l_maxAngle 10462538
data l_cameraEyeInitialized 1046253C
data l_cameraCenterInitialized 10462540
data l_headerByte 10462544
data l_headerFlag 10462545
data l_headerWord0 10462548
data l_headerWord1 1046254C
data l_headerWord2 10462550
data l_headerWord3 10462554
data l_cameraEye 10462558
data l_cameraCenter 10462564
data l_attackTypes 10191D34
data l_attackReactions 10191D74
data l_fireJoints 10191E5C
data l_fireScales 10191E34
ext 026066C4 void* boGetResource(void* control, SafeString* name, s32 index)
data l_resourceControl 101F4F28
data l_safeStringVtable 1000A09C
data l_arcName 1000A1D8
data l_heapArcName 1000A25F
*/
