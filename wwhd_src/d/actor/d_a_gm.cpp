/**
 * d_a_gm.cpp (WWHD)
 * Mini-boss - Mothula / enemy - Mothula larva.
 * Verified against the recompiled original by tools/verify (d_a_gm and d_a_gm_collider).
 * HD: the GameCube behavior functions are Nonmatching stubs. Measured actor, renderer,
 * particle and reaction layouts retain its names and the verified GHS operation order.
 */
#include "d/actor/d_a_gm.h"
#include "bindings.h"
#include "gc_api/d/actor/d_a_player.h"
#include "gc_api/tu.h"

// HD: GHS emits private vtables for the inline constructors in this translation unit.
const gc_tu_data gc_tu = {
    .safeString = 0x1000FC9C,
    .safeStringAssure = 0x02154138,
    .objAcch = {0x1000FCE4, 0x1000FD04, 0x1000FCF4},
    .objAcchDt = {0x1000FCF4, 0x1000FD04},
};

static GmHIOHD l_HIO;
static GmSinCosTable l_sincos;
static GmLarvaOffsets l_larvaOffsets;
static u32 l_larvaOffsetsInitialized;
static GmWingPose l_wingPose;
static GmWingNames l_wingNames;
static GmCounters l_counters;
static GmRegistrations l_registrations;
static f32 l_minAngle;
static f32 l_maxAngle;
static u8 l_defaultVector;
static u8 l_defaultAngles;
static GmModelPointer l_currentModel;
static Mtx34 l_jointMatrix;
static Mtx34 l_matrixStack;
static GmMatrixPointer l_currentMatrix;
static GmSavePointer l_saveInfo;
static GmResourcePointer l_resourceControl;
static s8 l_startRoomNo;
static GmProbeCoordinates l_probeCoordinates;
static GmFlameScales l_flameScales;
static GmFlameJoints l_flameJoints;
static GmColliderSource l_bodyCylSource;
static GmColliderSource l_diveCylSource;
static GmColliderSource l_windCylSource;
static GmColliderSource l_weaponSphSource;

static inline void gm_zeroWingPhases(gm_class* actor);

static inline void* gm_play() {
    // Real HD getter saves LR in the caller's outgoing linkage area.
    gabi::Local<cXyz> linkage;
    return GmHD_dComIfGp_get();
}

static inline void gm_lineSet(dBgS_LinChk* check, cXyz* start, cXyz* end, gm_class* actor) {
    gabi::Local<cXyz> linkage;
    GmHD_dBgS_LinChk_Set(check, start, end, actor);
}

// Each cylinder's inline geometry constructors write this TU's vtables.
static inline void gm_cylinderConstructor(dCcD_Cyl* collider) {
    GmHD_dCcD_GObjInf_dCcD_GObjInf(collider);
    auto* shape = reinterpret_cast<CcDataHD::cCcD_ShapeAttr*>(collider->mShapeAttr);
    shape->mVtable = 0x100015A8;
    shape->mAab.mVtable = 0x1000FCB4;
    GmHD_cM3dGCyl_ctor(&collider->mCyl);
    collider->__vtbl_hitinf = 0x1004B108;
    collider->mCyl.__vtbl = 0x1004B150;
    shape->mVtable = 0x1004B160;
}

// HD: all twelve float loads precede stores; this copy also quiets signaling NaNs.
static inline void gm_copyMatrix(u32 source, u32 destination) {
    f32 elements[12];
    for (u32 i = 0; i < 12; ++i)
        elements[i] = GmGuest<Mtx34>(source)->m[i / 4][i % 4];
    for (u32 i = 0; i < 12; ++i)
        GmGuest<Mtx34>(destination)->m[i / 4][i % 4] = elements[i];
}
enum class GmRotation { XSet, YSet, XMultiply, YMultiply, ZMultiply };
void GmHD_mDoMtx_XrotS(Mtx34*, s16);
static inline void gm_rotate(u32 matrix, GmRotation rotation, s16 angle) {
    Mtx34* mtx = gabi::at<Mtx34>(matrix);
    switch (rotation) {
    case GmRotation::XSet:
        GmHD_mDoMtx_XrotS(mtx, angle);
        break;
    case GmRotation::YSet:
        mDoMtx_YrotS(mtx, angle);
        break;
    case GmRotation::XMultiply:
        mDoMtx_XrotM(mtx, angle);
        break;
    case GmRotation::YMultiply:
        mDoMtx_YrotM(mtx, angle);
        break;
    case GmRotation::ZMultiply:
        mDoMtx_ZrotM(mtx, angle);
        break;
    }
}
static inline u32 gm_currentMatrix() { return l_currentMatrix.v; }

static inline f32 gm_sin(s16 angle) { return l_sincos.mSamples[(u16)angle >> 3].mSin; }
static inline f32 gm_cos(s16 angle) { return l_sincos.mSamples[(u16)angle >> 3].mCos; }

static inline void* gm_resource(u32 name, s32 index) {
    gabi::Local<SafeString> key;
    key->mStringTop = name;
    key->__vtbl = gc_tu.safeString;
    void* control = gabi::at<void>(l_resourceControl.v);
    return GmHD_dRes_control_c_getRes(control, key.get(), index);
}

static inline void gm_safeAssure(SafeString* string) {
    u32 vtable = string->__vtbl;
    u32 target = GmGuest<GmSafeStringMethodsHD>(vtable)->assureTermination;
    gabi::call_ptr(target, string);
}
static inline u32 gm_relative(const be<u32>& field) {
    u32 relative = field;
    return relative ? (u32)(gabi::ea(&field) + relative) : 0;
}

static inline void gm_hitSound(gm_class* actor, u32 sound, bool actorSound = false) {
    if (!gabi::ea(actor) || !gabi::ea(&actor->eyePos))
        return;
    s8 room = actor->current.roomNo;
    if (actorSound) {
        u32 id = actor->mBsPcId;
        s32 reverb = GmHD_dComIfGp_getReverb(room);
        GmHD_mDoAud_seStartLevel(sound, &actor->eyePos, id, 0, reverb);
    } else {
        s32 reverb = GmHD_dComIfGp_getReverb(room);
        GmHD_mDoAud_seStart_UnsignedID(sound, &actor->eyePos, 32, reverb);
    }
}
static inline void gm_hitParticle(u32 id, cXyz* position, csXyz* angles = nullptr,
                                  cXyz* scale = nullptr) {
    void* play = gm_play();
    void* controller = gabi::at<void>(GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mParticle.v);
    GmHD_dPa_control_c_set_Effect(controller, 0, id, position, angles ? angles : gabi::at<csXyz>(0),
                                  scale ? scale : gabi::at<cXyz>(0), 255, 0, -1, 0, 0, 0);
}

static inline void gm_actionSound(gm_class* actor, u32 sound) {
    if (!gabi::ea(&actor->eyePos))
        return;
    s8 room = actor->current.roomNo;
    s32 reverb = GmHD_dComIfGp_getReverb(room);
    GmHD_mDoAud_seStart_UnsignedID(sound, &actor->eyePos, 0, reverb);
}
static inline bool gm_animationStopped(gm_class* actor) {
    u32 morf = gabi::ea(actor->mpMorf.get());
    if ((u8)GmGuest<mDoExt_McaMorf>(morf)->mFrameCtrl.mState & 1)
        return true;
    return (f32)GmGuest<mDoExt_McaMorf>(morf)->mFrameCtrl.mRate == 0.0f;
}
static inline void gm_zeroWingPhases(gm_class* actor) {
    actor->mBodyPhase.x = 0;
    actor->mBodyPhase.y = 0;
    actor->mBodyPhase.z = 0;
    actor->mAttackCounter = 0;
    for (csXyz& phase : actor->mWingPhases) {
        phase.x = 0;
        phase.y = 0;
        phase.z = 0;
    }
    actor->mWingPhaseStep.x = 0;
    actor->mWingPhaseStep.y = 0;
    actor->mWingPhaseStep.z = 0;
    actor->mBodyPhaseStep.x = 0;
    actor->mBodyPhaseStep.y = 0;
    actor->mBodyPhaseStep.z = 0;
    actor->mBobPhase = 0;
}
static inline bool gm_groundProbe(gm_class* actor, cXyz* base, bool yaw) {
    if (yaw)
        gm_rotate(gm_currentMatrix(), GmRotation::YSet, actor->current.angle.y);
    gm_rotate(gm_currentMatrix(), yaw ? GmRotation::XMultiply : GmRotation::XSet, actor->m43E);
    gm_rotate(gm_currentMatrix(), GmRotation::ZMultiply, actor->m442);
    gabi::Local<cXyz> offset, target, copy;
    offset->x = 0.0f;
    offset->y = 100.0f;
    offset->z = yaw ? 200.0f : 0.0f;
    GmHD_MtxPosition(offset.get(), target.get());
    GmHD_PSVECAdd(target.get(), base, target.get());
    f32 x = target->x, y = target->y, z = target->z;
    copy->x = x;
    copy->y = y;
    copy->z = z;
    return Line_check(actor, copy.get()) != FALSE;
}

static inline void gm_cameraVectors(gm_class* actor, cXyz* center, cXyz* eye) {
    f32 eyeX = actor->mCameraEye.x, centerY = actor->mCameraCenter.y;
    eye->x = eyeX;
    center->y = centerY;
    f32 eyeZ = actor->mCameraEye.z, centerX = actor->mCameraCenter.x;
    eye->z = eyeZ;
    center->x = centerX;
    f32 eyeY = actor->mCameraEye.y, centerZ = actor->mCameraCenter.z;
    eye->y = eyeY;
    center->z = centerZ;
}

// Inline particle state machine emitted in daGM_Execute.
static inline void gm_removeEmitter(gptr<JPABaseEmitter>& slot, be<u8>& state) {
    u32 emitter = slot.v;
    if (!emitter)
        return;
    auto* flags = &GmGuest<GmEmitterHD>(emitter)->mStatus;
    u32 oldFlags = *flags;
    GmGuest<GmEmitterHD>(emitter)->mMaxFrame = -1;
    *flags = oldFlags | 1;
    state = 0;
    slot.v = 0;
}
static inline u32 gm_spawnEmitter(gm_class* actor, u32 id) {
    void* play = gm_play();
    void* controller = gabi::at<void>(GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mParticle.v);
    return gabi::ea(GmHD_dPa_control_c_set_Emitter(controller, 0, id, &actor->current.pos,
                                                   &actor->shape_angle, gabi::at<cXyz>(0), 255, 0,
                                                   -1, 0, 0, 0));
}
static inline void gm_attachEmitter(gm_class* actor, gptr<JPABaseEmitter>& slot, u32 joint) {
    u32 emitter = slot.v;
    if (!emitter)
        return;
    u32 morf = actor->mpMorf.v;
    u32 model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
    u32 matrixObject = GmGuest<GmModelHD>(model)->mpMtxBuffer.v;
    auto* flags = &GmGuest<J3DMtxBuffer_hd>(matrixObject)->mFlags;
    u16 oldFlags = *flags;
    u32 matrices = GmGuest<J3DMtxBuffer_hd>(matrixObject)->mpAnmMtx;
    *flags = oldFlags | 0x10;
    GmHD_JPASetRMtxTVecfromMtx(gabi::at<void>(matrices + joint * sizeof(Mtx34)),
                               &GmGuest<GmEmitterHD>(emitter)->mGlobalRotationMtx,
                               &GmGuest<GmEmitterHD>(emitter)->mGlobalPosition);
}
static inline bool gm_wingEmitter(gm_class* actor, gptr<JPABaseEmitter>& slot, be<u8>& state,
                                  be<s16>& timer, u32 joint, bool* attached = nullptr) {
    bool reloadFlags = true;
    switch ((u8)state) {
    case 0: {
        u32 emitter = gm_spawnEmitter(actor, 0x81A3);
        slot.v = emitter;
        if (emitter)
            state = (u8)state + 1;
        break;
    }
    case 1:
        if (!(u32)slot.v) {
            reloadFlags = false;
            break;
        }
        if ((u8)actor->mAction == GM_ACTION_FLY_DAMAGE) {
            state = (u8)state + 1;
            timer = 0;
        }
        break;
    case 2: {
        u32 emitter = slot.v;
        if (!emitter) {
            reloadFlags = false;
            break;
        }
        GmGuest<GmEmitterHD>(emitter)->mRate = 80.0f;
        emitter = slot.v;
        GmGuest<GmEmitterHD>(emitter)->mVolumeSize = 80.0f;
        emitter = slot.v;
        GmGuest<GmEmitterHD>(emitter)->mGlobalPrmColor.r = 255;
        GmGuest<GmEmitterHD>(emitter)->mGlobalPrmColor.g = 0;
        GmGuest<GmEmitterHD>(emitter)->mGlobalPrmColor.b = 0;
        state = (u8)state + 1;
        break;
    }
    case 3: {
        u32 emitter = slot.v;
        if (!emitter) {
            reloadFlags = false;
            break;
        }
        GmGuest<GmEmitterHD>(emitter)->mRate = 3.0f;
        emitter = slot.v;
        GmGuest<GmEmitterHD>(emitter)->mVolumeSize = 0.0f;
        state = (u8)state + 1;
        break;
    }
    case 4: {
        u32 emitter = slot.v;
        if (!emitter) {
            reloadFlags = false;
            break;
        }
        s16 next = (s16)((s16)timer + 17);
        timer = next;
        if (next > 255) {
            timer = 255;
            state = 1;
            next = 255;
        }
        GmGuest<GmEmitterHD>(emitter)->mGlobalPrmColor.r = 255;
        GmGuest<GmEmitterHD>(emitter)->mGlobalPrmColor.g = next;
        GmGuest<GmEmitterHD>(emitter)->mGlobalPrmColor.b = next;
        break;
    }
    }
    if (!(u32)slot.v)
        return reloadFlags;
    if (attached)
        *attached = true;
    gm_attachEmitter(actor, slot, joint);
    return true;
}
/* inlined into daGM_Execute */
static inline void action_hane_rakka(gm_class* actor) {
    gm_play();
    u32 state = (u32)(s32)(s16)actor->mActionState;
    if (state == 10) {
        gm_zeroWingPhases(actor);
        actor->gravity = -1.0f;
        actor->speed.y = 7.0f;
        if ((u8)actor->m2CD != 2) {
            actor->speedF = 40.0f;
            u8 type = actor->mWingFlags;
            f32 random = GmHD_cM_rndFX(4096.0f) + 16384.0f;
            s16 delta = (s16)gabi::ftoi(random);
            if (type == 1 || type == 3) {
                actor->current.angle.y = (s16)actor->current.angle.y + delta;
                actor->mBodyPhase.x = 0x1000;
            } else {
                actor->current.angle.y = (s16)actor->current.angle.y - delta;
                actor->mBodyPhase.x = -0x1000;
            }
            actor->mBodyPhase.y = 0x300;
        }
        actor->mActionState = (s16)actor->mActionState + 1;
        state = 11;
    }
    if (state == 11 && (((u32)actor->mAcch.m_flags & 0x20) ||
                        (f32)actor->mHomePos.y - 1000.0f > (f32)actor->current.pos.y)) {
        actor->shape_angle.x = 0;
        actor->mBodyPhase.x = 0;
        actor->mBodyPhase.y = 0;
        actor->mActionState = (s16)actor->mActionState + 1;
    } else if (state == 12 && (f32)actor->scale.x < 0.1f) {
        GmHD_fopAcM_delete(actor);
    }
    GmHD_cLib_addCalc0(&actor->scale.x, 1.0f, 0.02f);
    f32 scale = actor->scale.x;
    actor->scale.y = scale;
    actor->scale.z = scale;
    GmHD_cLib_addCalc0(&actor->speedF, 1.0f, 5.0f);
    if ((f32)actor->speed.y < 0.0f) {
        actor->shape_angle.y = (s16)actor->shape_angle.y + (s16)actor->mBodyPhase.x;
        actor->shape_angle.x = (s16)actor->shape_angle.x + (s16)actor->mBodyPhase.y;
    }
}
static inline void gm_uchiwaOscillation(gm_class* actor) {
    actor->mBodyPhase.x = (s16)actor->mBodyPhase.x + 400;
    actor->mBodyPhase.y = (s16)actor->mBodyPhase.y + 400;
    actor->mBodyPhase.z = (s16)actor->mBodyPhase.z + 400;
    actor->mBodyAngle.y = (s16)gabi::ftoi(gm_sin(actor->mBodyPhase.x) * 4000.0f);
    actor->mBodyAngle.x = (s16)gabi::ftoi(gm_cos(actor->mBodyPhase.y) * 4000.0f);
    actor->mBodyAngle.z = (s16)gabi::ftoi(gm_cos(actor->mBodyPhase.z) * 4000.0f);
}
/* inlined into daGM_Execute */
static inline void action_uchiwa_dousa(gm_class* actor) {
    gm_play();
    u32 state = (u32)(s32)(s16)actor->mActionState;
    if (state == 20) {
        GmHD_anm_init(actor, dRes_INDEX_GM_BCK_PAUSE_e, 1.0f, 0, 1.0f, -1);
        gm_zeroWingPhases(actor);
        void* play = gm_play();
        void* target =
            gabi::at<void>(GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
        s32 yaw = GmHD_fopAcM_searchActorAngleY_StorageWide(actor, target);
        actor->current.angle.y = yaw + 0x8000;
        if (gabi::ea(actor) + 0x37C) {
            gm_actionSound(actor, 0x58A8);
            gm_hitSound(actor, 0x48B0, true);
        }
        actor->mWeaponSph.mObjAt.mSPrm = (u32)actor->mWeaponSph.mObjAt.mSPrm & ~1u;
        actor->mDiveCyl.mObjAt.mSPrm = (u32)actor->mDiveCyl.mObjAt.mSPrm & ~1u;
        actor->mWallRadius = 250.0f;
        actor->speedF = 60.0f;
        actor->speed.y = 24.0f;
        if ((u8)actor->mDamageType == 3) {
            actor->speedF = 20.0f;
            actor->speed.y = 22.0f;
        }
        actor->mAttackCounter = 5000;
        u32 morf = actor->mpMorf.v;
        GmGuest<mDoExt_McaMorf>(morf)->mFrameCtrl.mRate = 0.0f;
        actor->mActionState = (s16)actor->mActionState + 1;
        state = 21;
    }
    if (state == 21) {
        GmHD_cLib_addCalc0(&actor->speedF, 1.0f, 3.0f);
        GmHD_cLib_addCalc0(&actor->speed.y, 1.0f, 2.0f);
        for (u32 i = 0; i < 4; ++i) {
            actor->mWingAngles[i].y = l_wingPose.mAngles[i].y;
            actor->mWingAngles[i].x = l_wingPose.mAngles[i].x;
            actor->mWingAngles[i].z = l_wingPose.mAngles[i].z;
        }
        gm_uchiwaOscillation(actor);
        if ((u8)actor->mDamageType != 3) {
            actor->shape_angle.z = (s16)actor->shape_angle.z + (s16)actor->mAttackCounter;
            GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mAttackCounter, 0, 1, 0x100);
        }
        if ((f32)actor->speed.y < 0.1f && (f32)actor->speedF < 0.1f) {
            actor->speedF = 0.0f;
            actor->mActionState = (s16)actor->mActionState + 1;
            GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mTwistAngle.y, 0x2000, 1, 0x1000);
            return;
        }
    } else if (state == 22) {
        actor->gravity = -0.08f;
        gm_uchiwaOscillation(actor);
        if ((f32)actor->current.pos.y < (f32)actor->mAcch.m_ground_h + 150.0f) {
            actor->speed.y = 0.0f;
            actor->gravity = 0.0f;
            actor->mActionState = 0;
            actor->mAction = GM_ACTION_FLY;
        }
    }
    GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mTwistAngle.y, 0x2000, 1, 0x1000);
}
static inline void gm_reduceOffset(gm_class* actor) {
    GmHD_cLib_addCalc0(&actor->mPositionOffset.x, 1.0f, 5.0f);
    GmHD_cLib_addCalc0(&actor->mPositionOffset.y, 1.0f, 5.0f);
    GmHD_cLib_addCalc0(&actor->mPositionOffset.z, 1.0f, 5.0f);
}
static inline void* gm_playerTarget() {
    void* play = gm_play();
    return gabi::at<void>(GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
}
/* inlined into daGM_Execute */
static inline void action_totugeki(gm_class* actor) {
    void* play = gm_play();
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(
        GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
    gabi::Local<dBgS_LinChk> check;
    dBgS_LinChk_ct(check.get(), {0x1000FD14, 0x1000FD24, 0x1000FD44, 0x1000FD34}, false);
    gabi::Local<cXyz> direction, start;
    start->x = 0.0f;
    start->y = 0.0f;
    start->z = 0.0f;
    s16 state = actor->mActionState;
    bool hitWall = false;
    switch (state) {
    case 30:
        gm_zeroWingPhases(actor);
        for (cXyz& target : actor->mProbeTargets) {
            target.x = 0.0f;
            target.y = 0.0f;
            target.z = 0.0f;
        }
        GmGuest<mDoExt_brkAnm>(actor->mpBrkAnms[1].v)->mFrameCtrl.mFrame = 0.0f;
        actor->mBrkIndex = 2;
        actor->mDiveCount = 0;
        for (cXyz& target : actor->mProbeTargets) {
            target.x = 0.0f;
            target.y = 0.0f;
            target.z = 0.0f;
        }
        if ((s32)actor->mAnimation != dRes_INDEX_GM_BCK_FLY_e)
            GmHD_anm_init(actor, dRes_INDEX_GM_BCK_FLY_e, 1.0f, 2, 1.0f, -1);
        actor->current.angle.y = actor->shape_angle.y;
        actor->speedF = 0.0f;
        actor->mBobOffset.x = 0.0f;
        actor->mBobOffset.y = 0.0f;
        actor->mBobOffset.z = 0.0f;
        actor->mActionState = (s16)actor->mActionState + 1;
        [[fallthrough]];
    case 31: {
        GmHD_cLib_addCalcAngleS2_IntTarget(&actor->shape_angle.x, 0, 1, 0x200);
        s32 yaw = GmHD_fopAcM_searchActorAngleY_StorageWide(actor, gm_playerTarget());
        GmHD_cLib_addCalcAngleS2_ShortTarget(&actor->shape_angle.y, yaw, 1, 0x1000);
        f32 targetY = (f32)player->current.pos.y + 400.0f;
        GmHD_cLib_addCalc2_Float(&actor->current.pos.y, targetY, 1.0f, 20.0f);
        f32 dy = (f32)actor->current.pos.y - targetY;
        actor->current.angle.y = actor->shape_angle.y;
        if (__builtin_fabsf(dy) < 2.0f) {
            GmGuest<mDoExt_McaMorf>(actor->mpMorf.v)->mFrameCtrl.mRate = 2.0f;
            actor->mTimers[0] = 45;
            actor->mActionState = (s16)actor->mActionState + 1;
        }
        if (!gabi::ea(&actor->eyePos))
            break;
        gm_actionSound(actor, 0x702F);
        break;
    }
    case 32: {
        if (gabi::ea(actor))
            gm_actionSound(actor, 0x702F);
        if ((s16)actor->mTimers[0] > 15)
            actor->current.angle.y =
                GmHD_fopAcM_searchActorAngleY_StorageWide(actor, gm_playerTarget());
        f32 distance = GmHD_fopAcM_searchActorDistance(actor, gm_playerTarget());
        if (distance < 500.0f) {
            u32 matrix = gm_currentMatrix();
            s32 yaw = GmHD_fopAcM_searchActorAngleY_StorageWide(actor, gm_playerTarget());
            gm_rotate(matrix, GmRotation::YSet, (s16)(yaw + 0x8000));
            direction->x = 0.0f;
            direction->y = 0.0f;
            direction->z = 500.0f;
            gabi::Local<cXyz> target;
            GmHD_MtxPosition(direction.get(), target.get());
            GmHD_PSVECAdd(target.get(), &player->current.pos, target.get());
            f32 x = target->x;
            if (__builtin_fabsf((f32)actor->current.pos.x - x) > 20.0f)
                GmHD_cLib_addCalc2_Float(&actor->current.pos.x, x, 1.0f, 10.0f);
            f32 z = target->z;
            if (__builtin_fabsf((f32)actor->current.pos.z - z) > 20.0f)
                GmHD_cLib_addCalc2_Float(&actor->current.pos.z, z, 1.0f, 10.0f);
        }
        GmHD_cLib_addCalcAngleS2_ShortTarget(&actor->shape_angle.y, (s16)actor->current.angle.y, 1,
                                             0x1000);
        if ((s16)actor->mTimers[0] != 0)
            break;
        gabi::Local<cXyz> target;
        target->x = (f32)player->current.pos.x;
        target->y = (f32)player->current.pos.y;
        target->z = (f32)player->current.pos.z;
        if (GmHD_Line_check(actor, target.get()))
            break;
        if ((s16)GmHD_cLib_distanceAngleS_ShortTarget((s16)actor->shape_angle.y,
                                                      (s16)actor->current.angle.y) > 0x500)
            break;
        actor->shape_angle.y = actor->current.angle.y;
        actor->mActionState = (s16)actor->mActionState + 1;
        [[fallthrough]];
    }
    case 33: {
        actor->mDiveCount = (s16)actor->mDiveCount + 1;
        GmGuest<mDoExt_McaMorf>(actor->mpMorf.v)->mFrameCtrl.mRate = 1.0f;
        GmHD_anm_init(actor, dRes_INDEX_GM_BCK_JETATACK_e, 5.0f, 2, 1.0f, -1);
        actor->mDiveCyl.mObjAt.mSPrm = (u32)actor->mDiveCyl.mObjAt.mSPrm | 1;
        actor->mDiveCyl.mObjAt.mRPrm = 1;
        actor->mPitchOffset = 0;
        actor->mBodyCyl.mObjCo.mSPrm = ((u32)actor->mBodyCyl.mObjCo.mSPrm & ~0x70u) | 0x20;
        actor->shape_angle.x = 0;
        actor->speedF = l_HIO.mDiveSpeed + 30.0f;
        actor->gravity = l_HIO.mDiveGravity + 1.0f;
        actor->speed.y = l_HIO.mDiveSpeedY + -85.0f;
        actor->mAttackFrame = 0;
        actor->mTargetPos.copy(player->current.pos);
        f32 y = player->current.pos.y;
        actor->mActionState = (s16)actor->mActionState + 1;
        actor->mTargetPos.y = y + 350.0f;
        [[fallthrough]];
    }
    case 34: {
        if (gabi::ea(&actor->eyePos))
            gm_actionSound(actor, 0x7031);
        actor->mBtNowFrame = 1000.0f;
        s16 counter = (s16)actor->mAttackFrame + 1;
        actor->mAttackFrame = counter;
        if (counter > (s16)gabi::ftoi(l_HIO.mAttackStartFrame) &&
            counter < (s16)gabi::ftoi(l_HIO.mAttackEndFrame + 62.0f))
            actor->mBtNowFrame = 5.0f;
        if ((f32)actor->speed.y > 0.0f &&
            (f32)actor->current.pos.y + (f32)actor->mPositionOffset.y > (f32)actor->mTargetPos.y) {
            actor->mAttackCounter = 1;
            GmHD_cLib_addCalc0(&actor->gravity, 1.0f, 5.0f);
            GmHD_cLib_addCalc0(&actor->speed.y, 1.0f, 5.0f);
        } else if ((s16)actor->mAttackCounter != 0) {
            GmHD_cLib_addCalc0(&actor->gravity, 1.0f, 5.0f);
            GmHD_cLib_addCalc0(&actor->speed.y, 1.0f, 5.0f);
        }
        f32 y = actor->current.pos.y;
        if (y + (f32)actor->mPositionOffset.y < (f32)start->y) {
            y = start->y;
            actor->current.pos.y = y;
        }
        f32 ground = (f32)actor->mAcch.m_ground_h + 100.0f;
        if (y < ground)
            actor->current.pos.y = ground;
        for (u32 i = 0; i < 3; ++i) {
            (&actor->mProbeHits[i])->x = 0.0f;
            (&actor->mProbeHits[i])->y = 0.0f;
            (&actor->mProbeHits[i])->z = 0.0f;
            gm_rotate(gm_currentMatrix(), GmRotation::YSet, actor->current.angle.y);
            direction->x = l_probeCoordinates.mX[i];
            direction->y = l_probeCoordinates.mY[i];
            direction->z = l_probeCoordinates.mZ[i];
            cXyz* target = (&actor->mProbeTargets[i]);
            GmHD_MtxPosition(direction.get(), target);
            GmHD_PSVECAdd(target, &actor->current.pos, target);
            GmHD_PSVECAdd(target, &actor->mPositionOffset, target);
            gabi::Local<cXyz> combined;
            GmHD_cXyz_add(&actor->current.pos, combined.get(), &actor->mPositionOffset);
            start->copy(*combined);
            gm_lineSet(check.get(), start.get(), target, actor);
            void* scene = gm_play();
            if (GmHD_cBgS_LineCross(&GmGuest<dComIfG_inf_c>(gabi::ea(scene))->play.mBgS,
                                    check.get()))
                (&actor->mProbeHits[i])->copy(*&check->mLin.mEnd);
        }
        if ((u32)actor->mAcch.m_flags & 0x10) {
            actor->mBtNowFrame = 1000.0f;
            actor->mActionState = 37;
            actor->mBodyCyl.mObjCo.mSPrm = ((u32)actor->mBodyCyl.mObjCo.mSPrm & ~0x70u) | 0x70;
            hitWall = true;
            break;
        }
        cXyz* first = &actor->mProbeHits[0];
        cXyz* second = &actor->mProbeHits[1];
        bool hitFirst = (f32)first->x != 0.0f || (f32)first->z != 0.0f;
        bool hitSecond = (f32)second->x != 0.0f || (f32)second->z != 0.0f;
        if (!hitFirst && !hitSecond)
            break;
        if (hitFirst && hitSecond) {
            actor->speedF = 0.0f;
            actor->gravity = 0.0f;
            actor->speed.y = 0.0f;
            actor->mBtNowFrame = 1000.0f;
            actor->mActionState = 40;
            actor->mAction = GM_ACTION_GROUND;
            actor->mBodyCyl.mObjCo.mSPrm = ((u32)actor->mBodyCyl.mObjCo.mSPrm & ~0x70u) | 0x70;
        } else {
            gabi::Local<cXyz> difference;
            GmHD_cXyz_subtract(second, difference.get(), &actor->current.pos);
            direction->x = (f32)difference->x;
            direction->z = (f32)difference->z;
            direction->y = (f32)difference->y;
            if ((f32)first->x != 0.0f || (f32)first->z != 0.0f) {
                GmHD_cXyz_subtract(first, difference.get(), &actor->current.pos);
                direction->x = (f32)difference->x;
                direction->y = (f32)difference->y;
                direction->z = (f32)difference->z;
            }
            s32 yaw = GmHD_cM_atan2s_WideResult((f32)direction->x, (f32)direction->z);
            actor->shape_angle.y = yaw + 0x4000;
            GmHD_cLib_addCalcAngleS2_ShortTarget(&actor->shape_angle.y, (s16)actor->current.angle.y,
                                                 1, 0x1000);
        }
        break;
    }
    case 37:
        for (cXyz& target : actor->mProbeTargets) {
            target.x = 0.0f;
            target.y = 0.0f;
            target.z = 0.0f;
        }
        actor->gravity = 0.0f;
        actor->speed.y = 0.0f;
        actor->mTimers[6] = 0;
        GmHD_anm_init(actor, dRes_INDEX_GM_BCK_UMINIGE_e, 5.0f, 2, 1.0f, -1);
        actor->speedF = 20.0f;
        actor->mActionState = (s16)actor->mActionState + 1;
        [[fallthrough]];
    case 38: {
        if (gabi::ea(actor))
            gm_actionSound(actor, 0x702F);
        GmHD_cLib_addCalc2_Float(&actor->current.pos.y, (f32)player->current.pos.y + 500.0f, 1.0f,
                                 20.0f);
        f32 dx =
            (f32)actor->mHomePos.x - ((f32)actor->current.pos.x + (f32)actor->mPositionOffset.x);
        f32 dz =
            (f32)actor->mHomePos.z - ((f32)actor->current.pos.z + (f32)actor->mPositionOffset.z);
        actor->current.angle.y = GmHD_cM_atan2s_WideResult(dx, dz);
        s32 yaw = GmHD_fopAcM_searchActorAngleY_StorageWide(actor, gm_playerTarget());
        GmHD_cLib_addCalcAngleS2_ShortTarget(&actor->shape_angle.y, yaw, 1, 0x1000);
        bool passed = false;
        for (f32 frame : {4.0f, 8.0f, 12.0f, 16.0f}) {
            u32 morf = actor->mpMorf.v;
            if (GmHD_J3DFrameCtrl_checkPass(&GmGuest<mDoExt_McaMorf>(morf)->mFrameCtrl, frame)) {
                passed = true;
                break;
            }
        }
        if (passed)
            GmHD_ks_set_rtn(actor);
        if (GmHD_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < 40.0f)
            actor->mActionState = 30;
        break;
    }
    }
    if (!hitWall)
        gm_reduceOffset(actor);
    check->dBgS_PolyPassChk::__vtbl = 0x1000FD44;
    check->dBgS_GrpPassChk::__vtbl = 0x1000FCD4;
    check->cBgS_PolyInfo::__vtbl = 0x1000FCC4;
    GmHD_cBgS_LinChk_dtor(check.get(), 0);
}
/* inlined into daGM_Execute */
static inline void action_dousa(gm_class* actor) {
    fopAc_ac_c* player = static_cast<fopAc_ac_c*>(gm_playerTarget());
    gm_play();
    f32 dx =
        (f32)player->current.pos.x - ((f32)actor->current.pos.x + (f32)actor->mPositionOffset.x);
    f32 dz =
        (f32)player->current.pos.z - ((f32)actor->current.pos.z + (f32)actor->mPositionOffset.z);
    s16 state = actor->mActionState;
    if (state == 0) {
        gm_zeroWingPhases(actor);
        for (cXyz& target : actor->mProbeTargets) {
            target.x = 0.0f;
            target.y = 0.0f;
            target.z = 0.0f;
        }
        GmGuest<mDoExt_brkAnm>(actor->mpBrkAnms[0].v)->mFrameCtrl.mFrame = 0.0f;
        actor->mWallHeight = 0.0f;
        actor->mBodyCenterY = -120.0f;
        actor->mBodyRadius = 220.0f;
        actor->mBodyHeight = 60.0f;
        actor->mBrkIndex = 0;
        actor->mWallRadius = 250.0f;
        f32 random = GmHD_cM_rndF(30.0f) + 30.0f;
        actor->mTimers[0] = 0;
        actor->mTimers[1] = (s16)gabi::ftoi(random);
        if ((s32)actor->mAnimation != dRes_INDEX_GM_BCK_FLY_e)
            GmHD_anm_init(actor, dRes_INDEX_GM_BCK_FLY_e, 10.0f, 2, 1.0f, -1);
        actor->mActionState = (s16)actor->mActionState + 1;
        state = 1;
    }
    if (state == 1) {
        bool foundTarget = false;
        actor->mTargetPos.y = (f32)player->current.pos.y + 250.0f;
        fopAc_ac_c* targetPlayer = static_cast<fopAc_ac_c*>(gm_playerTarget());
        if ((s16)actor->mTimers[0] == 0) {
            s16 delta = 0x2000;
            if (GmHD_cM_rnd() < 0.5f)
                delta = -0x2000;
            void* scene = gm_play();
            void* camera = gabi::at<void>(
                GmGuest<dComIfG_inf_c>(gabi::ea(scene))->play.mCameraInfo[0].mpCamera);
            s32 cameraAngle = GmHD_dCam_getAngleY(camera);
            for (u32 i = 0; i < 2; ++i) {
                gm_rotate(gm_currentMatrix(), GmRotation::YSet, (s16)(cameraAngle + delta));
                gabi::Local<cXyz> direction, target, copy;
                direction->x = 0.0f;
                direction->y = 0.0f;
                direction->z = 300.0f;
                GmHD_MtxPosition(direction.get(), target.get());
                GmHD_PSVECAdd(target.get(), &targetPlayer->current.pos, target.get());
                f32 y = (f32)actor->current.pos.y + (f32)actor->mPositionOffset.y;
                copy->x = (f32)target->x;
                copy->y = y;
                copy->z = (f32)target->z;
                target->y = y;
                if (!GmHD_Line_check(actor, copy.get())) {
                    f32 targetDx = (f32)target->x -
                                   ((f32)actor->current.pos.x + (f32)actor->mPositionOffset.x);
                    f32 targetDz = (f32)target->z -
                                   ((f32)actor->current.pos.z + (f32)actor->mPositionOffset.z);
                    actor->speedF = 10.0f;
                    actor->mTimers[0] = 20;
                    s16 random = (s16)gabi::ftoi(GmHD_cM_rndF(20.0f));
                    actor->mTimers[0] = (s16)actor->mTimers[0] + random;
                    actor->mTurnSpeed = 0.0f;
                    actor->mTargetYaw = GmHD_cM_atan2s_WideResult(targetDx, targetDz);
                    foundTarget = true;
                    break;
                }
                delta = (s16)(delta - 0x8000);
            }
        }
        if (!foundTarget &&
            !(__builtin_fabsf((f32)actor->current.pos.y - (f32)actor->mTargetPos.y) > 40.0f) &&
            !(GmHD_sqrtf(gabi::fmadds(dx, dx, dz * dz)) > 300.0f) && (s16)actor->mTimers[1] == 0) {
            f32 chance = 0.1f;
            u8 flags = actor->mWingFlags;
            for (u32 i = 0; i < 4; ++i) {
                if (flags & 1) {
                    chance += 0.1f;
                    if (chance > 0.3f)
                        chance = 0.3f;
                    flags >>= 1;
                }
            }
            actor->mActionState = 2;
            if (GmHD_cM_rnd() < chance && ((u8)actor->mWingFlags & 7) != 7) {
                actor->speedF = 0.0f;
                actor->mAction = GM_ACTION_DIVE;
                actor->mActionState = 30;
            }
        }
    } else if (state == 2 || state == 3) {
        if (state == 2) {
            GmHD_anm_init(actor, dRes_INDEX_GM_BCK_ATACK01_e, 10.0f, 2, 1.0f, -1);
            if (gabi::ea(&actor->eyePos))
                gm_hitSound(actor, 0x48AE, true);
            GmGuest<mDoExt_brkAnm>(actor->mpBrkAnms[1].v)->mFrameCtrl.mFrame = 0.0f;
            actor->mBrkIndex = 2;
            actor->speedF = 0.0f;
            f32 random = GmHD_cM_rndF(20.0f) + 20.0f;
            actor->mTimers[1] = (s16)gabi::ftoi(random);
            actor->mActionState = (s16)actor->mActionState + 1;
        }
        actor->mTargetPos.y = (f32)player->current.pos.y + 100.0f;
        if (!(__builtin_fabsf((f32)actor->current.pos.y - (f32)actor->mTargetPos.y) > 40.0f) &&
            (s16)actor->mTimers[1] == 0) {
            s16 yaw = actor->shape_angle.y;
            actor->mTargetYaw = yaw;
            actor->current.angle.y = yaw;
            actor->speedF = 50.0f;
            actor->mWeaponSph.mObjAt.mSPrm = (u32)actor->mWeaponSph.mObjAt.mSPrm | 1;
            actor->mWeaponSph.mObjAt.mRPrm = 1;
            GmHD_anm_init(actor, dRes_INDEX_GM_BCK_ATACK02_e, 2.0f, 0, 1.0f, 6);
            actor->mActionState = (s16)actor->mActionState + 1;
        }
    } else if (state == 4) {
        if ((s16)actor->mAttackCounter == 0) {
            if ((u32)actor->mWeaponSph.mGObjAt.mRPrm & 1) {
                actor->mWeaponSph.mObjAt.mSPrm = (u32)actor->mWeaponSph.mObjAt.mSPrm & ~1u;
                actor->speedF = -25.0f;
                actor->mAttackCounter = 1;
            } else if ((s16)GmGuest<daPy_py_c>(gabi::ea(player))->mDamageWaitTimer != 0) {
                actor->speedF = -50.0f;
                actor->mAttackCounter = 1;
            }
        }
        GmHD_cLib_addCalc0(&actor->speedF, 1.0f, 5.0f);
        if (gm_animationStopped(actor)) {
            f32 random = GmHD_cM_rndF(20.0f) + 40.0f;
            actor->speedF = 0.0f;
            actor->mTimers[1] = (s16)gabi::ftoi(random);
            GmHD_anm_init(actor, dRes_INDEX_GM_BCK_ATACK01_e, 2.0f, 2, 1.0f, -1);
            actor->mActionState = 0;
            actor->mWeaponSph.mObjAt.mSPrm = (u32)actor->mWeaponSph.mObjAt.mSPrm & ~1u;
        }
    }
    player = static_cast<fopAc_ac_c*>(gm_playerTarget());
    if (!((f32)actor->current.pos.y < (f32)player->current.pos.y + 150.0f)) {
        s16 phase = (s16)actor->mBodyPhase.x + 1000;
        actor->mBodyPhase.x = phase;
        f32 offset = gm_sin(phase) * 100.0f;
        actor->mBobOffset.y = offset;
        s16 currentState = actor->mActionState;
        if (currentState == 3 || currentState == 4) {
            actor->mBobOffset.x = 0.0f;
            actor->mBobOffset.y = 0.0f;
            actor->mBobOffset.z = 0.0f;
            offset = 0.0f;
        }
        GmHD_cLib_addCalc2_Float(&actor->mPositionOffset.y, offset, 1.0f, 10.0f);
    }
    if (((u8)actor->mWingFlags & 7) == 7 &&
        (s16)GmHD_cLib_distanceAngleS_IntTarget((s16)actor->shape_angle.z, 0) < 0x100)
        actor->mTargetRoll = -0x4000;
    if ((s16)actor->mActionState != 4) {
        s16 step = (s16)gabi::ftoi(actor->mTurnSpeed);
        GmHD_cLib_addCalcAngleS2_ShortTargetStep(&actor->current.angle.y, (s16)actor->mTargetYaw, 1,
                                                 step);
        actor->mTurnSpeed = 1700.0f;
        GmHD_cLib_addCalc2_Float(&actor->mTurnSpeed, 1700.0f, 1.0f, 3.0f);
        s32 yaw = GmHD_cM_atan2s_WideResult(dx, dz);
        GmHD_cLib_addCalcAngleS2_ShortTarget(&actor->shape_angle.y, yaw, 1, 0x800);
    }
    GmHD_cLib_addCalc2_Float(&actor->current.pos.y, (f32)actor->mTargetPos.y, 1.0f, 3.0f);
    GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mPitchOffset, 0, 1, 0x200);
    GmHD_cLib_addCalcAngleS2_IntTarget(&actor->shape_angle.x, 0, 1, 0x200);
    gm_actionSound(actor, 0x702F);
}

/* 0214BB1C */
BOOL nodeCallBack(void* node, s32 phase) {
    if (phase != 0)
        return TRUE;
    u32 model = l_currentModel.v;
    gm_class* actor = gabi::at<gm_class>(GmGuest<GmModelHD>(model)->mUserArea);
    void* joint = GmHD_J3DNode_toJoint(node);
    u16 index = GmGuest<GmJointHD>(gabi::ea(joint))->mJntNo;
    if (!gabi::ea(actor) || (u8)actor->m2CD != 0)
        return TRUE;
    u32 matrixObject = GmGuest<GmModelHD>(model)->mpMtxBuffer.v;
    u32 matrices = GmGuest<J3DMtxBuffer_hd>(matrixObject)->mpAnmMtx;
    GmGuest<J3DMtxBuffer_hd>(matrixObject)->mFlags =
        (u16)GmGuest<J3DMtxBuffer_hd>(matrixObject)->mFlags | 0x10;
    GmHD_PSMTXCopy(gabi::at<void>(matrices + index * 48), gabi::at<void>(gm_currentMatrix()));
    gabi::Local<cXyz> origin;
    if (index == 3 || index == 4 || index == 5) {
        origin->x = 0.0f;
        origin->y = 0.0f;
        origin->z = 0.0f;
        u32 position = index == 3 ? 2 : index == 4 ? 1 : 0;
        GmHD_MtxPosition(origin.get(), &actor->mJointPositions[position]);
        if (index == 5) {
            origin->x = 70.0f;
            GmHD_MtxPosition(origin.get(), &actor->mWeaponPos);
        }
        if (index == 4) {
            gm_rotate(gm_currentMatrix(), GmRotation::YMultiply, actor->mTwistAngle.y);
            gm_rotate(gm_currentMatrix(), GmRotation::XMultiply, actor->mTwistAngle.x);
            gm_rotate(gm_currentMatrix(), GmRotation::ZMultiply, actor->mTwistAngle.z);
        }
    } else if (index == 12 || index == 14 || index == 16 || index == 18) {
        u32 wing = (index - 12) / 2;
        gm_rotate(gm_currentMatrix(), GmRotation::YMultiply, actor->mWingAngles[wing].y);
        gm_rotate(gm_currentMatrix(), GmRotation::XMultiply, actor->mWingAngles[wing].x);
        gm_rotate(gm_currentMatrix(), GmRotation::ZMultiply, actor->mWingAngles[wing].z);
    }
    if (index != 12 && index != 14 && index != 16 && index != 18) {
        gm_rotate(gm_currentMatrix(), GmRotation::YMultiply, actor->mBodyAngle.y);
        gm_rotate(gm_currentMatrix(), GmRotation::XMultiply, actor->mBodyAngle.x);
        gm_rotate(gm_currentMatrix(), GmRotation::ZMultiply, actor->mBodyAngle.z);
    }
    matrixObject = GmGuest<GmModelHD>(model)->mpMtxBuffer.v;
    u32 source = gm_currentMatrix();
    matrices = GmGuest<J3DMtxBuffer_hd>(matrixObject)->mpAnmMtx;
    GmGuest<J3DMtxBuffer_hd>(matrixObject)->mFlags =
        (u16)GmGuest<J3DMtxBuffer_hd>(matrixObject)->mFlags | 0x10;
    gm_copyMatrix(source, matrices + index * 48);
    GmHD_PSMTXCopy(gabi::at<void>(gm_currentMatrix()), &l_jointMatrix);
    return TRUE;
}

/* 0214BDFC */
void draw_SUB(gm_class* actor) {
    u32 morf = gabi::ea(actor->mpMorf.get());
    u32 model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
    f32 scaleX = actor->scale.x, scaleY = actor->scale.y, scaleZ = actor->scale.z;
    GmGuest<GmModelHD>(model)->mBaseScale.x = scaleX;
    GmGuest<GmModelHD>(model)->mBaseScale.z = scaleZ;
    GmGuest<GmModelHD>(model)->mBaseScale.y = scaleY;
    f32 x = (f32)actor->current.pos.x + (f32)actor->mPositionOffset.x;
    f32 y = (f32)actor->current.pos.y + (f32)actor->mPositionOffset.y;
    f32 z = (f32)actor->current.pos.z + (f32)actor->mPositionOffset.z;
    GmHD_PSMTXTrans(&l_matrixStack, x, y, z);
    gm_rotate(gabi::ea(&l_matrixStack), GmRotation::XMultiply, actor->m43E);
    gm_rotate(gabi::ea(&l_matrixStack), GmRotation::ZMultiply, actor->m442);
    gm_rotate(gabi::ea(&l_matrixStack), GmRotation::YMultiply, actor->shape_angle.y);
    gm_rotate(gabi::ea(&l_matrixStack), GmRotation::XMultiply, actor->shape_angle.x);
    gm_rotate(gabi::ea(&l_matrixStack), GmRotation::YMultiply, actor->mPitchOffset);
    gm_rotate(gabi::ea(&l_matrixStack), GmRotation::ZMultiply, actor->shape_angle.z);
    gm_copyMatrix(gabi::ea(&l_matrixStack), model + 0xC8);
    if ((u8)actor->m2CD == 0) {
        GmHD_mDoExt_McaMorf_calc(actor->mpMorf.get());
        GmHD_enemy_fire(&actor->mEnemyFire);
    }
    void* environment = GmHD_dKy_getEnvlight();
    GmHD_dScnKy_env_light_c_settingTevStruct(environment, 0, &actor->current.pos, &actor->tevStr);
}

/* 0214BF60 */
BOOL daGM_Draw(gm_class* actor) {
    u32 morf = gabi::ea(actor->mpMorf.get());
    u32 model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
    void* environment = GmHD_dKy_getEnvlight();
    GmHD_dScnKy_env_light_c_setLightTevColorType_MAJI(environment, gabi::at<void>(model),
                                                      &actor->tevStr);
    s16 angle = actor->shape_angle.y;
    GmHD_dSnap_RegistFig(0xBE, actor, &actor->attention_info.position, angle, 1.0f, 1.0f, 1.0f);
    if ((u8)actor->m2CD == 0 && (s16)actor->mEnemyIce.mFreezeTimer > 20) {
        GmHD_dMat_ice_c_entryDL(actor->mpMorf.get(), -1, &actor->mInvisibleModel);
        return TRUE;
    }
    gm_play();
    if ((u8)actor->m2CD != 0) {
        GmHD_mDoExt_McaMorf_updateDL(actor->mpMorf.get());
        return TRUE;
    }
    u32 data = GmGuest<GmModelHD>(model)->mModelData.v;
    void* countObject = GmHD_J3DModelData_getJointTree(gabi::at<void>(data));
    u16 count = GmGuest<GmJointTreeHD>(gabi::ea(countObject))->mJointCount;
    for (u16 i = 0; i < count; ++i) {
        data = GmGuest<GmModelHD>(model)->mModelData.v;
        u32 bound = GmGuest<J3DModelData>(data)->mJointNum;
        u32 joint = GmGuest<J3DModelData>(data)->mJointNodes;
        if (i < bound)
            joint += i * 28;
        u32 material = GmGuest<GmJointHD>(joint)->mpMeshes;
        while (material) {
            u32 descriptor = GmGuest<GmMeshHD>(material)->mpDescriptor;
            u32 first = l_wingNames.mIndices[0];
            u16 name = GmGuest<GmMeshDescriptorHD>(descriptor)->mNameIndex;
            u32 visibility = GmGuest<GmMeshHD>(material)->mpShape;
            bool hidden;
            if (name == first)
                hidden = ((u8)actor->mWingFlags & 8) != 0;
            else if (name == (u32)l_wingNames.mIndices[1])
                hidden = ((u8)actor->mWingFlags & 2) != 0;
            else if (name == (u32)l_wingNames.mIndices[3])
                hidden = ((u8)actor->mWingFlags & 1) != 0;
            else if (name == (u32)l_wingNames.mIndices[2])
                hidden = ((u8)actor->mWingFlags & 4) != 0;
            else
                hidden = false;
            GmGuest<J3DShape>(visibility)->mVisible = hidden ? 0 : 1;
            material = GmGuest<GmMeshHD>(material)->mpNext;
        }
        data = GmGuest<GmModelHD>(model)->mModelData.v;
        countObject = GmHD_J3DModelData_getJointTree(gabi::at<void>(data));
        count = GmGuest<GmJointTreeHD>(gabi::ea(countObject))->mJointCount;
    }
    s16 state = actor->mBrkIndex;
    u32 animationIndex = state == 1 ? 2 : state == 2 ? 1 : state == 3 ? 3 : 0;
    void* animation = actor->mpBrkAnms[animationIndex].get();
    data = GmGuest<GmModelHD>(model)->mModelData.v;
    f32 frame = GmGuest<mDoExt_brkAnm>(gabi::ea(animation))->mFrameCtrl.mFrame;
    GmHD_mDoExt_brkAnm_entry(animation, gabi::at<void>(data), frame);
    GmHD_mDoExt_McaMorf_entryDL(actor->mpMorf.get());
    data = GmGuest<GmModelHD>(model)->mModelData.v;
    GmGuest<GmModelDataDrawHD>(data)->mDrawState = 0;
    return TRUE;
}

/* 0214C1C8 */
void anm_init(gm_class* actor, s32 animation, f32 morph, u8 mode, f32 speed, s32 auxiliary) {
    actor->mAnimation = animation;
    gabi::Local<SafeString> resourceName;
    resourceName->mStringTop = gabi::guest_addr("Gm");
    resourceName->__vtbl = gc_tu.safeString;
    void* resourceControl = gabi::at<void>(l_resourceControl.v);
    void* mainAnimation =
        GmHD_dRes_control_c_getRes(resourceControl, resourceName.get(), animation);
    void* secondary = gabi::at<void>(0);
    if (auxiliary >= 0) {
        gabi::Local<SafeString> secondaryName;
        secondaryName->mStringTop = gabi::guest_addr("Gm");
        secondaryName->__vtbl = gc_tu.safeString;
        resourceControl = gabi::at<void>(l_resourceControl.v);
        secondary = GmHD_dRes_control_c_getRes(resourceControl, secondaryName.get(), auxiliary);
    }
    GmHD_mDoExt_McaMorf_setAnm(actor->mpMorf.get(), mainAnimation, mode, secondary, morph, speed,
                               0.0f, -1.0f);
}

// HD: unlike the unfinished TWW declaration, the verified collision helper returns BOOL.
/* 0214C2F4 */
BOOL body_atari_check(gm_class* actor) {
    void* play = gm_play();
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(
        GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
    GmHD_dCcD_GStts_Move(&actor->mStts.__vtbl_gstts);
    s16 invulnerability = actor->mTimers[3];
    if (invulnerability != 0)
        return FALSE;
    s16 state = actor->mActionState;
    if (state == 90 || state == 91)
        return FALSE;
    if (!GmHD_dCcD_GObjInf_ChkTgHit(&actor->mBodyCyl)) {
        actor->mHitLatched = 0;
        return FALSE;
    }
    if ((u8)actor->mHitLatched != 0)
        return FALSE;
    actor->mDamageType = 0;
    void* hit = GmHD_dCcD_GObjInf_GetTgHitObj(&actor->mBodyCyl);
    gabi::Local<cXyz> hitPosition;
    f32 y = actor->mBodyCyl.mGObjTg.mHitPos.y, z = actor->mBodyCyl.mGObjTg.mHitPos.z;
    actor->mHitLatched = 1;
    hitPosition->z = z;
    f32 x = actor->mBodyCyl.mGObjTg.mHitPos.x;
    hitPosition->y = y;
    hitPosition->x = x;
    if (!gabi::ea(hit))
        return FALSE;
    gabi::Local<GmAttackInfo> info;
    info->pParticlePos = 0;
    actor->mTimers[3] = 8;
    u32 attackType = GmGuest<dCcD_GObjInf>(gabi::ea(hit))->mAtType;
    bool skipDamage = false, damageChecked = false;
    switch (attackType) {
    case GmCollision::AT_TYPE_GRAPPLING_HOOK:
        if ((s8)actor->stealItemLeft > 0) {
            u8 health = actor->health;
            actor->health = 10;
            info->mpObj = GmHD_dCcD_GObjInf_GetTgHitObj(&actor->mBodyCyl);
            GmHD_cc_at_check(actor, info.get());
            actor->mStealCount = (u8)((u8)actor->mStealCount + 1);
            actor->health = (s8)health;
        }
        gm_hitParticle(0x27B, &actor->attention_info.position);
        actor->mDamageType = 3;
        skipDamage = true;
        break;
    case GmCollision::AT_TYPE_SWORD: {
        gm_hitSound(actor, 0x2806);
        actor->mDamageType = 0;
        u8 cut = GmGuest<GmPlayerCutHD>(gabi::ea(player))->mCutType;
        bool strong = (cut >= 5 && cut <= 10) || cut == 12 || (cut >= 14 && cut <= 16) ||
                      cut == 21 || cut == 23 || (cut >= 25 && cut <= 27) || cut == 30 || cut == 31;
        if (strong)
            actor->mDamageType = 1;
        else if (cut >= 17 && cut <= 20) {
            info->mpObj = GmHD_dCcD_GObjInf_GetTgHitObj(&actor->mBodyCyl);
            GmHD_cc_at_check(actor, info.get());
            damageChecked = true;
        }
        break;
    }
    case GmCollision::AT_TYPE_WIND:
        actor->mDamageType = 3;
        if ((u8)actor->mAction != GM_ACTION_UCHIWA && ((u8)actor->mWingFlags & 15) != 15) {
            if ((s16)actor->mActionState == 34)
                actor->mDamageType = 9;
            actor->mActionState = 20;
            actor->mAction = GM_ACTION_UCHIWA;
            actor->mWallRadius = 250.0f;
        }
        skipDamage = true;
        break;
    case GmCollision::AT_TYPE_BOOMERANG:
        actor->mDamageType = 4;
        skipDamage = true;
        gm_hitParticle(0x27B, &actor->attention_info.position);
        gm_hitSound(actor, 0x2835);
        break;
    case GmCollision::AT_TYPE_BOKO_STICK:
        gm_hitSound(actor, 0x2835);
        break;
    case GmCollision::AT_TYPE_SKULL_HAMMER:
        gm_hitSound(actor, 0x2855);
        actor->mDamageType = 7;
        if ((u8)GmGuest<GmPlayerCutHD>(gabi::ea(player))->mCutType == 17)
            actor->mDamageType = 8;
        break;
    case GmCollision::AT_TYPE_BOMB:
        actor->mDamageType = 6;
        break;
    case GmCollision::AT_TYPE_HOOKSHOT:
        actor->mDamageType = 10;
        skipDamage = true;
        gm_hitSound(actor, 0x2836);
        break;
    case GmCollision::AT_TYPE_FIRE:
    case GmCollision::AT_TYPE_FIRE_ARROW:
        actor->mEnemyFire.mFireDuration = 100;
        actor->health = 0;
        if (gabi::ea(&actor->eyePos))
            gm_hitSound(actor, 0x48AF, true);
        actor->mDamageType = 5;
        gm_hitSound(actor, 0x2836);
        break;
    case GmCollision::AT_TYPE_LIGHT_ARROW:
        actor->mEnemyIce.mLightShrinkTimer = 1;
        actor->mEnemyIce.mYOffset = 80.0f;
        actor->mEnemyIce.mParticleScale = 1.0f;
        actor->health = 0;
        actor->attention_info.flags = 0;
        if (gabi::ea(&actor->eyePos))
            gm_hitSound(actor, 0x48AF, true);
        actor->mDamageType = 5;
        gm_hitSound(actor, 0x2836);
        break;
    case GmCollision::AT_TYPE_ICE_ARROW:
        actor->mEnemyIce.mFreezeDuration = 200;
        GmHD_enemy_fire_remove(&actor->mEnemyFire);
        gm_hitSound(actor, 0x48AF, true);
        skipDamage = true;
        actor->health = 0;
        actor->mDamageType = 5;
        gm_hitSound(actor, 0x2836);
        break;
    case GmCollision::AT_TYPE_NORMAL_ARROW:
        actor->mDamageType = 5;
        gm_hitSound(actor, 0x2836);
        break;
    default:
        actor->mDamageType = 0;
        gm_hitSound(actor, 0x2836);
        break;
    }
    if (!skipDamage) {
        if (!damageChecked) {
            info->mpObj = GmHD_dCcD_GObjInf_GetTgHitObj(&actor->mBodyCyl);
            GmHD_cc_at_check(actor, info.get());
        }
        u8 damageType = actor->mDamageType;
        if (damageType == 1 || damageType == 7 || damageType == 8 || (s8)actor->health <= 0) {
            gm_hitParticle(16, hitPosition.get());
            gabi::Local<cXyz> scale;
            scale->x = 2.0f;
            scale->y = 2.0f;
            scale->z = 2.0f;
            gm_hitParticle(15, hitPosition.get(), &player->shape_angle, scale.get());
        } else
            gm_hitParticle(13, hitPosition.get(), &player->shape_angle);
    }
    attackType = GmGuest<dCcD_GObjInf>(gabi::ea(hit))->mAtType;
    if (attackType == GmCollision::AT_TYPE_LIGHT_ARROW ||
        attackType == GmCollision::AT_TYPE_ICE_ARROW)
        return TRUE;
    u8 damageType = actor->mDamageType;
    if (damageType == 3 || damageType == 9)
        return TRUE;
    if (((u8)actor->mWingFlags & 15) == 15 || (u8)actor->m2CD != 0)
        return TRUE;
    actor->mWallRadius = 250.0f;
    play = gm_play();
    player = gabi::at<fopAc_ac_c>(
        GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
    if ((u32)l_larvaOffsetsInitialized == 0) {
        l_larvaOffsets.mPositions[2].y = 72.0f;
        l_larvaOffsetsInitialized = 1;
        l_larvaOffsets.mPositions[0].y = -72.0f;
        l_larvaOffsets.mPositions[0].z = -30.0f;
        l_larvaOffsets.mPositions[3].y = 72.0f;
        l_larvaOffsets.mPositions[2].x = 144.0f;
        l_larvaOffsets.mPositions[3].z = -30.0f;
        l_larvaOffsets.mPositions[3].x = -144.0f;
        l_larvaOffsets.mPositions[2].z = -30.0f;
        l_larvaOffsets.mPositions[1].x = -94.0f;
        l_larvaOffsets.mPositions[0].x = 94.0f;
        l_larvaOffsets.mPositions[1].z = -30.0f;
        l_larvaOffsets.mPositions[1].y = -72.0f;
    }
    u8 lastFlags = actor->mWingFlags;
    for (u32 i = 0; i < 4; ++i) {
        u32 bit = 1u << i;
        if (!((u8)actor->mWingFlags & bit)) {
            u32 parameters = (i + 1) | ((u8)actor->mAction == GM_ACTION_GROUND ? 0x200 : 0x100);
            s16 angle = actor->shape_angle.y;
            gm_rotate(gm_currentMatrix(), GmRotation::YSet, angle);
            gabi::Local<cXyz> direction;
            direction->x = l_larvaOffsets.mPositions[i].x;
            direction->y = l_larvaOffsets.mPositions[i].y;
            direction->z = l_larvaOffsets.mPositions[i].z;
            gabi::Local<cXyz> spawnPosition, basePosition;
            GmHD_MtxPosition(direction.get(), spawnPosition.get());
            GmHD_cXyz_add(&actor->current.pos, basePosition.get(), &actor->mPositionOffset);
            GmHD_PSVECAdd(spawnPosition.get(), basePosition.get(), spawnPosition.get());
            gabi::Local<csXyz> angles;
            angles->x = actor->shape_angle.x;
            angle = actor->shape_angle.y;
            angles->y = (s16)(angle - 32768);
            angles->z = actor->shape_angle.z;
            s8 room = actor->current.roomNo;
            GmHD_fopAcM_create(0xCC, parameters, spawnPosition.get(), room, angles.get(),
                               &actor->scale, 0, 0);
            gm_hitParticle(13, spawnPosition.get(), &player->shape_angle);
            lastFlags = (u8)actor->mWingFlags | bit;
            s8 health = actor->health;
            actor->mWingFlags = lastFlags;
            if (health > 0 && (u8)actor->mDamageType != 10)
                break;
        }
        if (i == 3) {
            s8 health = actor->health;
            if (health > 0 && (u8)actor->mDamageType != 10)
                return TRUE;
        }
    }
    if ((lastFlags & 15) != 15) {
        gm_hitSound(actor, 0x48AC, true);
        actor->mBtNowFrame = 1000.0f;
        actor->mActionState = (u8)actor->mAction == GM_ACTION_GROUND ? 52 : 50;
        actor->mAction = GM_ACTION_FLY_DAMAGE;
    } else if ((u8)actor->mAction != GM_ACTION_GROUND_ATTACK) {
        gm_hitSound(actor, 0x48AD, true);
        actor->mBtNowFrame = 1000.0f;
        actor->mActionState = (u8)actor->mAction == GM_ACTION_GROUND ? 61 : 60;
        actor->mWallHeight = 60.0f;
        actor->mWallRadius = 150.0f;
        f32 random = GmHD_cM_rndF(600.0f);
        actor->mAction = GM_ACTION_GROUND_ATTACK;
        actor->mTimers[2] = (s16)gabi::ftoi(random + 600.0f);
    }
    return TRUE;
}

/* 02151100 */
void BG_check(gm_class* actor) {
    f32 wallHeight = actor->mWallHeight;
    f32 wallRadius = actor->mWallRadius;
    GmHD_dBgS_AcchCir_SetWall(&actor->mAcchCir, wallHeight, wallRadius);
    GmHD_PSVECAdd(&actor->current.pos, &actor->mPositionOffset, &actor->current.pos);
    f32 oldY = (f32)actor->old.pos.y + (f32)actor->mPositionOffset.y;
    f32 y = actor->current.pos.y;
    f32 offset = actor->mGroundOffset;
    actor->current.pos.y = y - offset;
    actor->old.pos.y = oldY - offset;
    void* play = gm_play();
    GmHD_dBgS_Acch_CrrPos(&actor->mAcch, &GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mBgS);
    y = actor->current.pos.y;
    offset = actor->mGroundOffset;
    oldY = actor->old.pos.y;
    actor->current.pos.y = y + offset;
    actor->old.pos.y = oldY + offset;
    GmHD_PSVECSubtract(&actor->current.pos, &actor->mPositionOffset, &actor->current.pos);
    oldY = actor->old.pos.y;
    offset = actor->mPositionOffset.y;
    actor->old.pos.y = oldY - offset;
}

// HD: the verifier observes the BOOL result and a pointer to the stack position copy.
/* 0214CDB4 */
BOOL Line_check(gm_class* actor, cXyz* end) {
    gabi::Local<dBgS_LinChk> check;
    dBgS_LinChk_ct(check.get(), {0x1000FD14, 0x1000FD24, 0x1000FD44, 0x1000FD34}, false);
    s16 xAngle = actor->m43E;
    gm_rotate(gm_currentMatrix(), GmRotation::XSet, xAngle);
    s16 zAngle = actor->m442;
    gm_rotate(gm_currentMatrix(), GmRotation::ZMultiply, zAngle);
    gabi::Local<cXyz> direction;
    direction->x = 0.0f;
    direction->y = 100.0f;
    direction->z = 0.0f;
    gabi::Local<cXyz> start;
    GmHD_MtxPosition(direction.get(), start.get());
    GmHD_PSVECAdd(start.get(), &actor->mJointPositions[0], start.get());
    GmHD_PSVECAdd(start.get(), &actor->mPositionOffset, start.get());
    f32 startY = start->y;
    f32 endY = end->y;
    start->y = startY + 100.0f;
    end->y = endY + 100.0f;
    gm_lineSet(check.get(), start.get(), end, actor);
    void* play = gm_play();
    BOOL hit = GmHD_cBgS_LineCross(&GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mBgS, check.get());
    if (!hit) {
        actor->mLineStart.copy(*start);
        actor->mLineEnd.copy(*end);
    }
    check->dBgS_PolyPassChk::__vtbl = 0x1000FD44;
    check->dBgS_GrpPassChk::__vtbl = 0x1000FCD4;
    check->cBgS_PolyInfo::__vtbl = 0x1000FCC4;
    GmHD_cBgS_LinChk_dtor(check.get(), 0);
    return hit ? TRUE : FALSE;
}

/* 0214CFA4 */
BOOL ks_set_rtn(gm_class* actor) {
    s16 count = actor->m31E;
    if (count >= ((u8)actor->mType == 1 ? 10 : 20))
        return TRUE;
    s32 parameters = (s16)actor->mActionState == 83 ? 5 : 4;
    if (gabi::ea(actor) && gabi::ea(&actor->eyePos)) {
        s8 room = actor->current.roomNo;
        s32 reverb = GmHD_dComIfGp_getReverb(room);
        GmHD_mDoAud_seStart_SignedID(0x58A6, &actor->eyePos, 0, reverb);
    }
    gabi::Local<csXyz> angles;
    angles->x = actor->current.angle.x;
    s16 angleY = actor->current.angle.y;
    angles->y = angleY;
    angles->z = actor->current.angle.z;
    f32 randomAngle = GmHD_cM_rndFX(4096.0f);
    angles->y = (s16)gabi::ftoi((f32)angleY + randomAngle);
    u32 parent = gabi::ea(actor) ? (u32)actor->mBsPcId : 0xFFFFFFFF;
    s8 room = actor->current.roomNo;
    s32 created = GmHD_fopAcM_createChild(0xCD, parent, parameters, &actor->mJointPositions[2],
                                          room, angles.get(), &actor->scale, 0, 0);
    if (created == -1)
        return TRUE;
    actor->m31E = (s16)((s16)actor->m31E + 1);
    return FALSE;
}

/* 021511B8 */
void wing_ret_set(gm_class* actor) {
    for (u32 i = 0; i < 4; ++i) {
        GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mWingAngles[i].y, 0, 1, 0x200);
        GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mWingAngles[i].x, 0, 1, 0x200);
        GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mWingAngles[i].z, 0, 1, 0x200);
    }
    GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mBodyAngle.y, 0, 1, 0x200);
    GmHD_cLib_addCalcAngleS2_VectorX(&actor->mBodyAngle, 0, 1, 0x200);
    GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mBodyAngle.z, 0, 1, 0x200);
    GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mTwistAngle.y, 0, 1, 0x200);
}

/* 021512AC */
void action_kabehari(gm_class* actor) {
    void* play = gm_play();
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(
        GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
    gabi::Local<cXyz> target;
    target->z = 0.0f;
    target->y = 0.0f;
    target->x = 0.0f;
    s16 state = actor->mActionState;
    if (state == 40) {
        actor->mWallRadius = 70.0f;
        gm_zeroWingPhases(actor);
        actor->mDiveCyl.mObjAt.mSPrm = (u32)actor->mDiveCyl.mObjAt.mSPrm & ~1u;
        state = actor->mActionState;
        actor->mTargetPos.copy(player->current.pos);
        f32 y = player->current.pos.y;
        actor->mActionState = (s16)(state + 1);
        actor->mPitchOffset = 0;
        actor->mTimers[1] = 50;
        actor->mTargetPos.y = y + 450.0f;
    }
    if (state == 40 || state == 41) {
        GmHD_cLib_addCalcAngleS2_IntTarget(&actor->shape_angle.x, -16384, 1, 0x200);
        f32 dy = (f32)actor->current.pos.y - (f32)actor->mTargetPos.y;
        f32 dx = (f32)actor->current.pos.x - (f32)target->x;
        f32 dz = (f32)actor->current.pos.z - (f32)target->z;
        f32 distance = GmHD_sqrtf(gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy)));
        if (distance < 4.0f || ((u32)actor->mAcch.m_flags & 0x10) || (s16)actor->mTimers[1] == 0) {
            if ((s32)actor->mAnimation != dRes_INDEX_GM_BCK_TOMARU_e)
                anm_init(actor, dRes_INDEX_GM_BCK_TOMARU_e, 10.0f, 2, 1.0f, -1);
            actor->mWallRadius = 10.0f;
            actor->shape_angle.x = -16384;
            actor->mActionState = 42;
        }
    } else if (state == 42) {
        GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mPitchOffset, -32768, 1, 0x500);
        s16 difference = (s16)GmHD_cLib_distanceAngleS_IntTarget((s16)actor->mPitchOffset, -32768);
        if (difference < 0x100) {
            actor->mTimers[0] = 10;
            actor->mActionState = 43;
        }
    } else if (state == 43) {
        GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mPitchOffset, -32768, 1, 0x500);
        if ((s16)actor->mTimers[0] == 0) {
            if ((s16)actor->mDiveCount <= 1 && ((u8)actor->mWingFlags & 7) != 7) {
                play = gm_play();
                void* other = gabi::at<void>(
                    GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
                s16 angle = GmHD_fopAcM_searchActorAngleY_Angle(actor, other);
                actor->shape_angle.y = angle;
                actor->current.angle.y = angle;
                actor->mAction = GM_ACTION_DIVE;
                actor->mActionState = 33;
            } else {
                actor->mAction = GM_ACTION_FLY;
                actor->mActionState = 0;
            }
        }
    }
    if ((u8)actor->mAction != GM_ACTION_GROUND)
        return;
    s16 angle = (s16)((s16)actor->current.angle.y - 32768);
    GmHD_mDoMtx_YrotS(gabi::at<void>(gm_currentMatrix()), angle);
    gabi::Local<cXyz> direction;
    direction->x = 0.0f;
    direction->y = 0.0f;
    direction->z = 60.0f;
    GmHD_MtxPosition(direction.get(), target.get());
    GmHD_PSVECAdd(target.get(), &actor->mProbeHits[2], target.get());
    f32 x = target->x;
    GmHD_cLib_addCalc2_Float(&actor->current.pos.x, x, 1.0f, 10.0f);
    f32 y = actor->mTargetPos.y;
    GmHD_cLib_addCalc2_Float(&actor->current.pos.y, y, 1.0f, 10.0f);
    f32 z = target->z;
    GmHD_cLib_addCalc2_Float(&actor->current.pos.z, z, 1.0f, 10.0f);
    gabi::Local<cXyz> difference;
    GmHD_cXyz_subtract(&actor->mProbeHits[0], difference.get(), &actor->mProbeHits[1]);
    f32 dx = difference->x, dy = difference->y, dz = difference->z;
    direction->x = dx;
    direction->y = dy;
    direction->z = dz;
    angle = (s16)((s16)GmHD_cM_atan2s_AngleResult(dx, dz) + 0x4000);
    actor->current.angle.y = angle;
    GmHD_cLib_addCalcAngleS2_ShortTarget(&actor->shape_angle.y, angle, 1, 0x1000);
}

/* 0215163C */
void action_fly_damage(gm_class* actor) {
    gm_play();
    s16 state = actor->mActionState;
    bool animateWings = state == 51;
    if (state == 50) {
        actor->mBodyCyl.mObjCo.mSPrm = ((u32)actor->mBodyCyl.mObjCo.mSPrm & ~0x70u) | 0x70u;
        if ((u8)actor->mDamageType != 4) {
            GmGuest<mDoExt_brkAnm>(actor->mpBrkAnms[2].v)->mFrameCtrl.mFrame = 0.0f;
            actor->mBrkIndex = 1;
        } else {
            GmGuest<mDoExt_brkAnm>(actor->mpBrkAnms[0].v)->mFrameCtrl.mFrame = 0.0f;
            actor->mBrkIndex = 0;
        }
        anm_init(actor, dRes_INDEX_GM_BCK_PAUSE_e, 1.0f, 0, 1.0f, -1);
        actor->mPitchOffset = 0;
        actor->speedF = 30.0f;
    }
    if (state == 50 || state == 52) {
        actor->mWeaponSph.mObjAt.mSPrm = (u32)actor->mWeaponSph.mObjAt.mSPrm & ~1u;
        actor->mDiveCyl.mObjAt.mSPrm = (u32)actor->mDiveCyl.mObjAt.mSPrm & ~1u;
        gm_zeroWingPhases(actor);
        void* play = gm_play();
        void* player =
            gabi::at<void>(GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
        s32 yaw = GmHD_fopAcM_searchActorAngleY_StorageWide(actor, player);
        actor->current.angle.y = (s16)(yaw - 32768);
        f32 random = GmHD_cM_rnd();
        s16 increment = (s16)gabi::ftoi(10000.0f * (random < 0.5f ? -1.0f : 1.0f));
        actor->mWingPhaseStep.x = (s16)((s16)actor->mWingPhaseStep.x + increment);
        random = GmHD_cM_rnd();
        increment = (s16)gabi::ftoi(10000.0f * (random < 0.5f ? -1.0f : 1.0f));
        actor->mWingPhaseStep.y = (s16)((s16)actor->mWingPhaseStep.y + increment);
        random = GmHD_cM_rnd();
        increment = (s16)gabi::ftoi(2000.0f * (random < 0.5f ? -1.0f : 1.0f));
        s16 zPhase = actor->mWingPhaseStep.z;
        actor->speed.x = 0.0f;
        actor->mWingPhaseStep.z = (s16)(zPhase + increment);
        actor->mBodyPhaseStep.x = 5000;
        actor->mBodyPhaseStep.y = 5000;
        actor->mBodyPhaseStep.z = 7000;
        actor->mActionState = 51;
        actor->speed.y = 0.0f;
        actor->speed.z = 0.0f;
        actor->gravity = 0.0f;
        animateWings = true;
    }
    if (animateWings) {
        for (u32 i = 0; i < 4; ++i) {
            s16 x = (s16)((s16)actor->mWingPhases[i].x + (s16)actor->mWingPhaseStep.x);
            actor->mWingPhases[i].x = x;
            s16 y = (s16)((s16)actor->mWingPhases[i].y + (s16)actor->mWingPhaseStep.y);
            actor->mWingPhases[i].y = y;
            s16 z = (s16)((s16)actor->mWingPhases[i].z + (s16)actor->mWingPhaseStep.z);
            actor->mWingPhases[i].z = z;
            actor->mWingAngles[i].y = (s16)gabi::ftoi(gm_sin(x) * 5000.0f);
            y = actor->mWingPhases[i].y;
            actor->mWingAngles[i].x = (s16)gabi::ftoi(gm_cos(y) * 5000.0f);
            z = actor->mWingPhases[i].z;
            actor->mWingAngles[i].z = (s16)gabi::ftoi(gm_cos(z) * 3000.0f);
        }
        s16 x = (s16)((s16)actor->mBodyPhase.x + (s16)actor->mBodyPhaseStep.x);
        s16 y = (s16)((s16)actor->mBodyPhase.y + (s16)actor->mBodyPhaseStep.y);
        s16 z = (s16)((s16)actor->mBodyPhase.z + (s16)actor->mBodyPhaseStep.z);
        actor->mBodyPhase.x = x;
        actor->mBodyPhase.y = y;
        actor->mBodyPhase.z = z;
        actor->mBodyAngle.y = (s16)gabi::ftoi(gm_sin(x) * -3500.0f);
        actor->mBodyAngle.x = (s16)gabi::ftoi(gm_cos(y) * -3500.0f);
        z = actor->mBodyPhase.z;
        actor->mBodyAngle.z = (s16)gabi::ftoi(gm_cos(z) * -2000.0f);
        for (csXyz* step : {&actor->mWingPhaseStep, &actor->mBodyPhaseStep}) {
            GmHD_cLib_addCalcAngleS2_IntTarget(&step->x, 0, 1, 80);
            GmHD_cLib_addCalcAngleS2_IntTarget(&step->y, 0, 1, 80);
            GmHD_cLib_addCalcAngleS2_IntTarget(&step->z, 0, 1, 80);
        }
        GmHD_cLib_addCalc0(&actor->speedF, 1.0f, 2.0f);
        if ((f32)actor->speedF < 0.2f) {
            u32 animation = gabi::ea(actor->mpBrkAnms[0].get());
            actor->speedF = 0.0f;
            GmGuest<mDoExt_brkAnm>(animation)->mFrameCtrl.mFrame = 0.0f;
            s32 currentAnimation = actor->mAnimation;
            actor->mBrkIndex = 0;
            if (currentAnimation == 28 && ((u8)actor->mWingFlags & 7) != 7) {
                actor->mAction = GM_ACTION_DIVE;
                actor->mActionState = 37;
            } else {
                actor->mAction = GM_ACTION_FLY;
                actor->mActionState = 0;
            }
        }
    }
    f32 targetY = (f32)actor->mAcch.m_ground_h + 100.0f;
    if ((f32)actor->current.pos.y < targetY)
        GmHD_cLib_addCalc2_Float(&actor->current.pos.y, targetY, 1.0f, 3.0f);
}

/* 02151B98 */
void action_ground_attack(gm_class* actor) {
    void* play = gm_play();
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(
        GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
    f32 dx =
        (f32)player->current.pos.x - ((f32)actor->current.pos.x + (f32)actor->mPositionOffset.x);
    f32 dz =
        (f32)player->current.pos.z - ((f32)actor->current.pos.z + (f32)actor->mPositionOffset.z);
    s32 animation;
    u8 action;
    s16 state = actor->mActionState;
    switch (state) {
    case 60:
        actor->mWeaponSph.mGObjAt.mSpl = 0;
        actor->mBodyCyl.mObjCo.mSPrm = ((u32)actor->mBodyCyl.mObjCo.mSPrm & ~0x70u) | 0x70u;
        actor->current.angle.y = actor->shape_angle.y;
        actor->mTargetRoll = 0;
        actor->speedF = 10.0f;
        for (cXyz* offset : {&actor->mPositionOffset, &actor->mBobOffset}) {
            offset->x = 0.0f;
            offset->y = 0.0f;
            offset->z = 0.0f;
        }
        [[fallthrough]];
    case 61: {
        GmGuest<mDoExt_brkAnm>(actor->mpBrkAnms[0].v)->mFrameCtrl.mFrame = 0.0f;
        actor->mBrkIndex = 0;
        gm_zeroWingPhases(actor);
        actor->mWeaponSph.mObjAt.mSPrm = (u32)actor->mWeaponSph.mObjAt.mSPrm & ~1u;
        actor->mDiveCyl.mObjAt.mSPrm = (u32)actor->mDiveCyl.mObjAt.mSPrm & ~1u;
        actor->mBtHeight = (f32)l_HIO.mBtHeight + 120.0f;
        actor->mBtBodyR = (f32)l_HIO.mBtRadius + 100.0f;
        actor->mBtAttackType = 1;
        actor->mBtStartFrame = 0.0f;
        actor->mBtEndFrame = 10.0f;
        actor->mBtMaxDis = (f32)l_HIO.mBtDistance + 800.0f;
        actor->mBtNowFrame = 1000.0f;
        actor->mWallRadius = 250.0f;
        if ((s8)actor->health <= 0) {
            if ((u8)actor->mType == 0) {
                actor->mAction = GM_ACTION_DEMO;
                actor->mActionState = 200;
                return;
            }
            anm_init(actor, dRes_INDEX_GM_BCK_G_DEAD01_e, 1.0f, 0, 1.0f, -1);
            actor->mActionState = 100;
            break;
        }
        actor->speed.x = 0.0f;
        actor->speed.z = 0.0f;
        actor->speed.y = 10.0f;
        actor->gravity = -3.0f;
        actor->mActionState = 62;
        [[fallthrough]];
    }
    case 62: {
        actor->shape_angle.x = (s16)((s16)actor->shape_angle.x + 0x100);
        f32 ground = (f32)actor->mAcch.m_ground_h + 80.0f;
        if ((f32)actor->current.pos.y < ground) {
            s8 health = actor->health;
            anm_init(actor,
                     health > 0 ? dRes_INDEX_GM_BCK_G_CHAKUCHI_e : dRes_INDEX_GM_BCK_G_DEAD02_e,
                     health > 0 ? 10.0f : 1.0f, 0, 1.0f, -1);
            actor->mBodyCenterY = -20.0f;
            actor->mBodyRadius = 60.0f;
            actor->mBodyHeight = 100.0f;
            actor->speedF = 0.0f;
            actor->mActionState = (s16)((s16)actor->mActionState + 1);
        }
        break;
    }
    case 63:
        if ((u32)actor->mAcch.m_flags & 0x20) {
            gm_actionSound(actor, 0x58AA);
            actor->mActionState = 70;
        }
        break;
    case 70:
        gm_zeroWingPhases(actor);
        anm_init(actor, dRes_INDEX_GM_BCK_G_WAIT_e, 2.0f, 2, 1.0f, -1);
        actor->speedF = 0.0f;
        actor->mActionState = (s16)((s16)actor->mActionState + 1);
        [[fallthrough]];
    case 71: {
        void* frame = &GmGuest<mDoExt_McaMorf>(actor->mpMorf.v)->mFrameCtrl;
        if (GmHD_J3DFrameCtrl_checkPass(frame, 23.0f)) {
            s16 counter = (s16)((s16)actor->mAttackCounter + 1);
            actor->mAttackCounter = counter;
            s16 limit = (s16)((s16)l_HIO.mGroundAttackCount + 1);
            if (counter > limit) {
                actor->mActionState = (s16)((s16)actor->mActionState + 1);
                break;
            }
        }
        break;
    }
    case 72: {
        u32 id = gabi::ea(actor) ? (u32)actor->mBsPcId : 0xFFFFFFFF;
        f32 random = GmHD_cM_rndF((f32)(id & 7) * 2.0f);
        actor->mWingPhases[1].x = (s16)gabi::ftoi(random + 20.0f);
        actor->mBodyPhase.z = 0;
        if (!gm_groundProbe(actor, &player->current.pos, false)) {
            actor->current.angle.y = GmHD_cM_atan2s_AngleResult(dx, dz);
        } else if ((u8)actor->mType != 1) {
            if ((s32)actor->mAnimation != dRes_INDEX_GM_BCK_G_WAIT_e)
                anm_init(actor, dRes_INDEX_GM_BCK_G_WAIT_e, 2.0f, 2, 1.0f, -1);
            break;
        } else {
            actor->speedF = 30.0f;
            actor->mGroundTurn = 0;
            actor->mGroundTurn = (s16)((s16)l_HIO.mGroundTurnAngle + 0x4000);
            random = GmHD_cM_rnd();
            if (random < 0.5f)
                actor->mGroundTurn = (s16) - ((s16)l_HIO.mGroundTurnAngle + 0x4000);
            s16 yaw = GmHD_cM_atan2s_AngleResult(dx, dz);
            actor->current.angle.y = yaw;
            actor->current.angle.y = (s16)(yaw + (s16)actor->mGroundTurn);
        }
        if ((u8)actor->mType == 1) {
            actor->mWallHeight = 60.0f;
            actor->mWallRadius = 70.0f;
        }
        anm_init(actor, dRes_INDEX_GM_BCK_G_WALK_e, 2.0f, 2, 1.0f, -1);
        actor->mActionState = (s16)((s16)actor->mActionState + 1);
        [[fallthrough]];
    }
    case 73: {
        gm_actionSound(actor, 0x7030);
        s16 yaw = actor->current.angle.y, shapeYaw = actor->shape_angle.y;
        s16 difference = (s16)GmHD_cLib_distanceAngleS_ShortTarget(yaw, shapeYaw);
        if (difference < 0x100) {
            actor->speedF = 30.0f;
            s16 counter = (s16)((s16)actor->mBodyPhase.z + 1);
            s16 limit = actor->mWingPhases[1].x;
            actor->mBodyPhase.z = counter;
            if (counter > limit) {
                actor->mActionState = 70;
                break;
            }
            if ((u8)actor->mType == 1)
                actor->current.angle.y = GmHD_cM_atan2s_AngleResult(dx, dz);
            else if (gm_groundProbe(actor, &actor->mJointPositions[0], true)) {
                actor->mActionState = 70;
                break;
            }
        }
        f32 distance = GmHD_sqrtf(gabi::fmadds(dx, dx, dz * dz));
        if (!(distance > 400.0f)) {
            if ((u8)actor->mType == 1 && gm_groundProbe(actor, &actor->mJointPositions[0], true)) {
                actor->mActionState = 70;
                break;
            }
            actor->mAttackFrame = 0;
            actor->mActionState = (s16)((s16)actor->mActionState + 1);
            anm_init(actor, dRes_INDEX_GM_BCK_G_ATACK01_e, 2.0f, 0, 1.0f, -1);
            {
                gm_hitSound(actor, 0x48AE, true);
                actor->mBrkIndex = 2;
                actor->speedF = 0.0f;
                actor->mActionState = (s16)((s16)actor->mActionState + 1);
                break;
            }
        }
        break;
    }
    case 74:
        anm_init(actor, dRes_INDEX_GM_BCK_G_ATACK01_e, 2.0f, 0, 1.0f, -1);
        {
            gm_hitSound(actor, 0x48AE, true);
            actor->mBrkIndex = 2;
            actor->speedF = 0.0f;
            actor->mActionState = (s16)((s16)actor->mActionState + 1);
            break;
        }
    case 75: {
        gm_actionSound(actor, 0x7030);
        dx = (f32)player->current.pos.x -
             ((f32)actor->current.pos.x + (f32)actor->mPositionOffset.x);
        dz = (f32)player->current.pos.z -
             ((f32)actor->current.pos.z + (f32)actor->mPositionOffset.z);
        actor->current.angle.y = GmHD_cM_atan2s_AngleResult(dx, dz);
        if (gm_animationStopped(actor)) {
            anm_init(actor, dRes_INDEX_GM_BCK_G_ATACK02_e, 0.0f, 0, 1.0f, -1);
            actor->mBrkIndex = 0;
            actor->mWeaponSph.mObjAt.mSPrm = (u32)actor->mWeaponSph.mObjAt.mSPrm | 1;
            actor->mWeaponSph.mObjAt.mRPrm = 1;
            actor->speedF = 35.0f;
            actor->gravity = -3.0f;
            actor->speed.y = 20.0f;
            {
                actor->mActionState = (s16)((s16)actor->mActionState + 1);
                break;
            }
        }
        break;
    }
    case 76:
        if ((s16)actor->mWingPhases[0].x == 0 && ((u32)actor->mWeaponSph.mGObjAt.mRPrm & 1)) {
            actor->mWeaponSph.mObjAt.mSPrm = (u32)actor->mWeaponSph.mObjAt.mSPrm & ~1u;
            actor->speedF = -35.0f;
            actor->mWingPhases[0].x = 1;
        }
        if (gm_animationStopped(actor)) {
            anm_init(actor, dRes_INDEX_GM_BCK_G_ATACK03_e, 0.0f, 0, 1.0f, 7);
            u32 flags = actor->mWeaponSph.mObjAt.mSPrm;
            s16 oldState = actor->mActionState;
            actor->mWingPhases[0].x = 0;
            actor->mWeaponSph.mObjAt.mSPrm = flags & ~1u;
            actor->mActionState = (s16)(oldState + 1);
        }
        break;
    case 77:
        if ((u32)actor->mAcch.m_flags & 0x20) {
            actor->speedF = 0.0f;
            actor->mActionState = 70;
        }
        break;
    case 80: {
        actor->mWeaponSph.mObjAt.mSPrm = (u32)actor->mWeaponSph.mObjAt.mSPrm & ~1u;
        if ((u8)actor->mDamageType != 4) {
            gm_hitSound(actor, 0x48AC, true);
            GmGuest<mDoExt_brkAnm>(actor->mpBrkAnms[2].v)->mFrameCtrl.mFrame = 0.0f;
            actor->mBrkIndex = 1;
            actor->mTimers[5] = 0;
            anm_init(actor, dRes_INDEX_GM_BCK_G_DAMAGE_e, 0.0f, 0, 1.0f, -1);
            actor->speedF = 35.0f;
        } else {
            actor->mTimers[5] = 45;
            actor->speedF = 0.0f;
            GmGuest<mDoExt_brkAnm>(actor->mpBrkAnms[0].v)->mFrameCtrl.mFrame = 0.0f;
            actor->mBrkIndex = 0;
        }
        play = gm_play();
        void* other =
            gabi::at<void>(GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
        s32 yaw = GmHD_fopAcM_searchActorAngleY_StorageWide(actor, other);
        actor->current.angle.y = (s16)(yaw + 32768);
        if ((u8)actor->mDamageType == 1) {
            actor->gravity = -3.0f;
            actor->speed.y = 20.0f;
        }
        {
            actor->mActionState = (s16)((s16)actor->mActionState + 1);
            break;
        }
    }
    case 81: {
        GmHD_cLib_addCalc0(&actor->speedF, 1.0f, 1.5f);
        s16 target = (s16)((s16)l_HIO.mFreezeSoundFrame + 44);
        if ((s16)actor->mTimers[5] == target) {
            GmHD_enemy_piyo_set(actor);
            gm_actionSound(actor, 0x50BC);
        }
        bool finished =
            (u8)actor->mDamageType == 4 ? (s16)actor->mTimers[5] == 0 : gm_animationStopped(actor);
        if (finished) {
            anm_init(actor, dRes_INDEX_GM_BCK_G_WALK_e, 2.0f, 2, 2.0f, -1);
            GmGuest<mDoExt_brkAnm>(actor->mpBrkAnms[0].v)->mFrameCtrl.mFrame = 0.0f;
            actor->mBrkIndex = 0;
            {
                actor->mActionState = (s16)((s16)actor->mActionState + 1);
                break;
            }
        }
        break;
    }
    case 82: {
        gm_actionSound(actor, 0x7030);
        s16 yaw = (s16)((s16)GmHD_cM_atan2s_AngleResult(dx, dz) + 32768);
        s16 shapeYaw = actor->shape_angle.y;
        actor->current.angle.y = yaw;
        s16 difference = (s16)GmHD_cLib_distanceAngleS_ShortTarget(yaw, shapeYaw);
        if (difference < 0x100) {
            anm_init(actor, dRes_INDEX_GM_BCK_G_MACHINGUN_e, 2.0f, 2, 1.0f, -1);
            actor->mTimers[0] = 4;
            yaw = (s16)((s16)GmHD_cM_atan2s_AngleResult(dx, dz) + 32768);
            actor->mWingPhases[0].y = yaw;
            actor->mWingPhases[0].z = 0x4000;
            {
                actor->mActionState = (s16)((s16)actor->mActionState + 1);
                break;
            }
        }
        break;
    }
    case 83:
        if ((s16)actor->mTimers[0] == 0) {
            s16 offset = actor->mWingPhases[0].z, yaw = actor->mWingPhases[0].y;
            actor->current.angle.y = (s16)(yaw + offset);
            offset = actor->mWingPhases[0].z;
            actor->mTimers[0] = 4;
            actor->mWingPhases[0].z = (s16)(offset ^ 0x4000);
        }
        if (ks_set_rtn(actor)) {
            actor->mActionState = 70;
            break;
        }
        break;
    case 84: {
        anm_init(actor, dRes_INDEX_GM_BCK_G_UCHIWA_e, 1.0f, 0, 1.0f, -1);
        gm_zeroWingPhases(actor);
        play = gm_play();
        void* other =
            gabi::at<void>(GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
        s32 yaw = GmHD_fopAcM_searchActorAngleY_StorageWide(actor, other);
        actor->current.angle.y = (s16)(yaw + 32768);
        actor->speedF = 20.0f;
        actor->mWeaponSph.mObjAt.mSPrm = (u32)actor->mWeaponSph.mObjAt.mSPrm & ~1u;
        gm_actionSound(actor, 0x58A8);
        gm_hitSound(actor, 0x48B0, true);
        {
            actor->mActionState = (s16)((s16)actor->mActionState + 1);
            break;
        }
    }
    case 85:
        GmHD_cLib_addCalc0(&actor->speedF, 1.0f, 3.0f);
        if (gm_animationStopped(actor))
            actor->mActionState = 82;
        break;
    case 90:
        actor->mWingFlags = 0;
        anm_init(actor, dRes_INDEX_GM_BCK_G_HANE_e, 0.0f, 0, 1.0f, 8);
        actor->speedF = 0.0f;
        {
            actor->mActionState = (s16)((s16)actor->mActionState + 1);
            break;
        }
    case 91:
        if (gm_animationStopped(actor)) {
            actor->speed.y = 0.0f;
            actor->gravity = 0.0f;
            actor->m43E = 0;
            actor->mGroundAngle.y = 0;
            actor->m442 = 0;
            actor->mWeaponSph.mGObjAt.mSpl = 5;
            f32 height = l_HIO.mBtHeight, radius = l_HIO.mBtRadius;
            actor->mBtHeight = height + 200.0f;
            actor->mBtBodyR = radius + 100.0f;
            f32 distance = l_HIO.mBtDistance;
            actor->mBtStartFrame = 0.0f;
            actor->mBtAttackType = 1;
            actor->mBtEndFrame = 10.0f;
            actor->mBtMaxDis = distance + 880.0f;
            actor->mActionState = 37;
            actor->mAction = GM_ACTION_DIVE;
        }
        break;
    case 100: {
        actor->mTimers[5] = 0;
        actor->gravity = -3.0f;
        GmGuest<mDoExt_brkAnm>(actor->mpBrkAnms[0].v)->mFrameCtrl.mFrame = 0.0f;
        actor->mBrkIndex = 0;
        actor->mStts.m_weight = 255;
        gm_hitSound(actor, 0x48AF, true);
        gm_zeroWingPhases(actor);
        if ((u8)actor->mDamageType == 7) {
            actor->speedF = 0.0f;
            anm_init(actor, dRes_INDEX_GM_BCK_G_HAMMER_e, 0.0f, 0, 1.0f, -1);
        } else {
            play = gm_play();
            void* other = gabi::at<void>(
                GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
            s32 yaw = GmHD_fopAcM_searchActorAngleY_StorageWide(actor, other);
            actor->current.angle.y = (s16)(yaw + 32768);
            actor->speedF = 45.0f;
            actor->gravity = -3.0f;
            actor->speed.y = 45.0f;
            actor->mTimers[0] = 200;
            anm_init(actor, dRes_INDEX_GM_BCK_G_DEAD01_e, 1.0f, 0, 1.0f, -1);
        }
        {
            actor->mActionState = (s16)((s16)actor->mActionState + 1);
            break;
        }
    }
    case 101: {
        if ((u8)actor->mDamageType == 7) {
            if (gm_animationStopped(actor)) {
                u8 stealBit = actor->stealItemBitNo;
                GmHD_fopAcM_createDisappear(actor, &actor->current.pos, 10, 0, stealBit);
                GmHD_fopAcM_delete(actor);
                u32 save = l_saveInfo.v;
                s8 room = actor->home.roomNo;
                u16 id = actor->setID;
                GmHD_dSv_info_c_onActor(&GmGuest<GmSaveHeaderHD>(save)->mSave, id, room);
                u8 switchNo = actor->mSwitchNo;
                if (switchNo != 255 && (u8)actor->mSaveSwitch != 0) {
                    save = l_saveInfo.v;
                    room = actor->current.roomNo;
                    GmHD_dSv_info_c_onSwitch(&GmGuest<GmSaveHeaderHD>(save)->mSave, switchNo, room);
                }
            }
            break;
        }
        GmHD_cLib_addCalc0(&actor->speedF, 1.0f, 1.5f);
        s16 substate = actor->mBodyPhase.x;
        if (substate == 0) {
            if ((s16)actor->mTimers[0] == 0 || ((u32)actor->mAcch.m_flags & 0x20)) {
                gm_actionSound(actor, 0x58A9);
                anm_init(actor, dRes_INDEX_GM_BCK_G_DEAD02_e, 1.0f, 0, 1.0f, -1);
                actor->mBodyPhase.x = 1;
                actor->speedF = 0.0f;
            }
        } else if (substate == 1) {
            if (gm_animationStopped(actor)) {
                void* animation = actor->mpBrkAnms[3].get();
                actor->mBodyPhase.x = (s16)(substate + 1);
                GmGuest<mDoExt_brkAnm>(gabi::ea(animation))->mFrameCtrl.mFrame = 0.0f;
                actor->mBrkIndex = 3;
            }
        } else if (substate == 2) {
            u32 animation = gabi::ea(actor->mpBrkAnms[3].get());
            bool stopped = ((u8)GmGuest<mDoExt_brkAnm>(animation)->mFrameCtrl.mState & 1) != 0;
            if (!stopped)
                stopped = (f32)GmGuest<mDoExt_brkAnm>(animation)->mFrameCtrl.mRate == 0.0f;
            if (stopped)
                actor->mBodyPhase.x = (s16)(substate + 1);
        } else if (substate == 3) {
            if ((s16)actor->mTimers[0] == 0 || ((u32)actor->mAcch.m_flags & 0x20)) {
                s16 timer = (s16)((s16)actor->mBodyPhase.y + 1);
                actor->mBodyPhase.y = timer;
                if (timer > 15) {
                    u8 stealBit = actor->stealItemBitNo;
                    GmHD_fopAcM_createDisappear(actor, &actor->current.pos, 10, 0, stealBit);
                    u8 switchNo = actor->mSwitchNo;
                    if (switchNo != 255 && (u8)actor->mSaveSwitch != 0) {
                        u32 save = l_saveInfo.v;
                        s8 room = actor->current.roomNo;
                        GmHD_dSv_info_c_onSwitch(&GmGuest<GmSaveHeaderHD>(save)->mSave, switchNo,
                                                 room);
                    }
                    GmHD_fopAcM_delete(actor);
                    u32 save = l_saveInfo.v;
                    s8 room = actor->home.roomNo;
                    u16 id = actor->setID;
                    GmHD_dSv_info_c_onActor(&GmGuest<GmSaveHeaderHD>(save)->mSave, id, room);
                    actor->mBodyPhase.x = (s16)((s16)actor->mBodyPhase.x + 1);
                }
            }
        }
        break;
    }
    default:
        break;
    }
    actor->mBtNowFrame = 1000.0f;
    state = actor->mActionState;
    if ((u32)((s32)state - 74) < 4) {
        s16 counter = (s16)((s16)actor->mAttackFrame + 1);
        actor->mAttackFrame = counter;
        s16 first = (s16)gabi::ftoi(l_HIO.mAttackStartFrame);
        if (counter > first) {
            s16 last = (s16)gabi::ftoi((f32)l_HIO.mAttackEndFrame + 17.0f);
            if (counter < last)
                actor->mBtNowFrame = 5.0f;
        }
    }
    GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mPitchOffset, 0, 1, 0x800);
    GmHD_cLib_addCalcAngleS2_IntTarget(&actor->shape_angle.x, 0, 1, 0x800);
    state = actor->mActionState;
    if (state != 81 && state != 101 && state != 85) {
        s16 yaw = actor->current.angle.y;
        GmHD_cLib_addCalcAngleS2_ShortTarget(&actor->shape_angle.y, yaw, 1, 0x800);
    }
    animation = actor->mAnimation;
    if (animation != dRes_INDEX_GM_BCK_G_DEAD01_e && animation != dRes_INDEX_GM_BCK_G_DEAD02_e &&
        body_atari_check(actor)) {
        u8 damageType = actor->mDamageType;
        actor->mAction = GM_ACTION_GROUND_ATTACK;
        if (damageType == 7) {
            actor->health = 0;
            if ((u8)actor->mType == 0) {
                actor->mAction = GM_ACTION_DEMO;
                actor->mActionState = 200;
                return;
            }
            actor->mActionState = 100;
        } else if ((s8)actor->health <= 0) {
            if ((u8)actor->mType == 0) {
                actor->mAction = GM_ACTION_DEMO;
                actor->mActionState = 200;
                return;
            }
            actor->mActionState = 100;
        } else {
            damageType = actor->mDamageType;
            actor->mActionState = damageType == 3 ? 84 : 80;
        }
        return;
    }
    state = actor->mActionState;
    action = actor->mAction;
    if ((u32)((s32)state - 71) <= 2 && (u8)actor->mType != 1 && (s16)actor->mTimers[2] == 0)
        actor->mActionState = 90;
    if (action == 10 && ((u32)actor->mAcch.m_flags & 0x20))
        GmHD_fopAcM_getGroundAngle(actor, &actor->mGroundAngle);
    return;
}

/* 02153350 */
void action_demo(gm_class* actor) {
    void* play = gm_play();
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(
        GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
    play = gm_play();
    s8 cameraIndex = GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mCameraID;
    play = gm_play();
    u32 camera = GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mCameraInfo[cameraIndex].mpCamera;
    void* cameraBody = &GmGuest<GmCameraHD>(camera)->mCameraBody;
    gabi::Local<dBgS_LinChk> check;
    dBgS_LinChk_ct(check.get(), {0x1000FD94, 0x1000FDA4, 0x1000FDC4, 0x1000FDB4}, false);
    check->mbCamThrough = 1;
    check->mGrp = 3;
    s16 state = actor->mActionState;
    bool cameraOnly = false;
    bool skipCamera = false;
    switch (state) {
    case 200:
        gm_zeroWingPhases(actor);
        actor->attention_info.flags = 0;
        if ((s8)actor->health > 0) {
            actor->current.angle.y = 0;
            actor->shape_angle.y = 0;
            actor->shape_angle.x = -16384;
            actor->mDemoMode = 1;
            actor->current.pos.x = -100.0f;
            actor->current.pos.y = 570.0f;
            actor->current.pos.z = -1350.0f;
            anm_init(actor, dRes_INDEX_GM_BCK_TOMARU_e, 10.0f, 2, 1.0f, -1);
            actor->mPitchOffset = -32768;
        }
        actor->mActionState = (s16)((s16)actor->mActionState + 1);
        [[fallthrough]];
    case 201:
        if ((u16)actor->eventInfo.mCommand != dEvtCmd_INDEMO_e) {
            play = gm_play();
            GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mEvtCtrl.mEventFlag =
                (u16)GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mEvtCtrl.mEventFlag | 1;
            GmHD_fopAcM_orderPotentialEvent(actor, 2, 65535, 0);
            actor->eventInfo.mCondition = (u16)actor->eventInfo.mCondition | 2;
            break;
        }
        GmGuest<daPy_py_c>(gabi::ea(player))->mDemo.mParam0 = 0;
        GmGuest<daPy_py_c>(gabi::ea(player))->mDemo.mDemoType = 3;
        GmGuest<daPy_py_c>(gabi::ea(player))->mDemo.mDemoMode = 1;
        GmHD_dCamera_c_Stop(cameraBody);
        GmHD_dCamera_c_SetTrimSize(cameraBody, 2);
        actor->mCameraFov = 50.0f;
        if ((s8)actor->health > 0) {
            actor->mActionState = (s16)((s16)actor->mActionState + 1);
            actor->scale.x = 1.0f;
            actor->scale.y = 1.0f;
            actor->scale.z = 1.0f;
        } else {
            GmGuest<mDoExt_brkAnm>(actor->mpBrkAnms[0].v)->mFrameCtrl.mFrame = 0.0f;
            actor->mBrkIndex = 0;
            play = gm_play();
            void* other = gabi::at<void>(
                GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
            s32 yaw = GmHD_fopAcM_searchActorAngleY_StorageWide(actor, other);
            actor->current.angle.y = (s16)(yaw + 32768);
            actor->speedF = 20.0f;
            actor->gravity = -3.0f;
            actor->speed.y = 45.0f;
            anm_init(actor, dRes_INDEX_GM_BCK_G_DEAD01_e, 1.0f, 0, 1.0f, -1);
            actor->mActionState = 220;
            skipCamera = true;
            break;
        }
        break;
    case 202: {
        gabi::Local<cXyz> position;
        position->x = l_HIO.mPlayerStartX + 73.0f;
        position->y = player->current.pos.y;
        position->z = l_HIO.mPlayerStartZ + 1284.0f;
        u32 vtable = player->__vtbl;
        u32 function = GmGuest<GmPlayerMethodsHD>(vtable)->setPlayerPosAndAngle;
        gabi::call_ptr(function, player, position.get(), -32768);
        actor->mCameraCenter.x = l_HIO.mCameraOffsetX + 74.0f;
        actor->mCameraCenter.y = l_HIO.mCameraOffsetY + 107.0f;
        actor->mCameraCenter.z = l_HIO.mCameraOffsetZ + 1095.0f;
        actor->mCameraEye.x = l_HIO.mCameraEyeX + 71.0f;
        actor->mCameraEye.y = l_HIO.mCameraEyeY + 124.0f;
        s16 next = (s16)((s16)actor->mActionState + 1);
        actor->mTimers[0] = 10;
        actor->mActionState = next;
        actor->mCameraEye.z = l_HIO.mCameraEyeZ + 800.0f;
        [[fallthrough]];
    }
    case 203:
        if ((s16)actor->mTimers[0] == 0) {
            GmGuest<daPy_py_c>(gabi::ea(player))->mDemo.mDemoMode = 2;
            actor->mDemoMode = 2;
            GmGuest<daPy_py_c>(gabi::ea(player))->mDemo.mDemoMoveAngle = -32768;
            actor->current.pos.x = 100.0f;
            actor->current.pos.y = l_HIO.mDemoStartHeight + 450.0f;
            actor->current.pos.z = 1150.0f;
            GmHD_mDoAud_bgmAllMute(30);
            actor->mTimers[0] = (s16)(l_HIO.mDemoDelayAngle + 50);
            actor->mActionState = 205;
            cameraOnly = true;
            break;
        }
        break;
    case 205:
        if ((s16)actor->mTimers[0] == 0) {
            GmGuest<daPy_py_c>(gabi::ea(player))->mDemo.mDemoMode = 26;
            actor->mActionState = (s16)((s16)actor->mActionState + 1);
            actor->mTimers[0] = 40;
        }
        break;
    case 206:
        if ((s16)actor->mTimers[0] == 0) {
            s16 next = (s16)((s16)actor->mActionState + 1);
            actor->mTimers[0] = (s16)(l_HIO.mDemoFlightDelay + 120);
            actor->mActionState = next;
            actor->current.pos.x = 100.0f;
            actor->current.pos.y = 570.0f;
            actor->current.pos.z = 1350.0f;
            actor->mCameraStep = 0.0f;
        }
        break;
    case 207: {
        f32 targetY = l_HIO.mDemoCenterHeight + 355.0f;
        f32 step = (f32)actor->mCameraStep * 10.0f;
        GmHD_cLib_addCalc2_Float(&actor->mCameraCenter.y, targetY, 0.05f, step);
        step = (f32)actor->mCameraStep * 10.0f;
        targetY = l_HIO.mDemoEyeHeight + 121.0f;
        GmHD_cLib_addCalc2_Float(&actor->mCameraEye.y, targetY, 0.05f, step);
        GmHD_cLib_addCalc2_Float(&actor->mCameraStep, 1.0f, 1.0f, 0.05f);
        if ((s16)actor->mTimers[0] == 0) {
            anm_init(actor, dRes_INDEX_GM_BCK_JETATACK_e, 0.0f, 2, 1.0f, -1);
            f32 dx = (f32)actor->mHomePos.x -
                     ((f32)actor->current.pos.x + (f32)actor->mPositionOffset.x);
            f32 dz = (f32)actor->mHomePos.z -
                     ((f32)actor->current.pos.z + (f32)actor->mPositionOffset.z);
            actor->current.angle.y = GmHD_cM_atan2s_AngleResult(dx, dz);
            actor->shape_angle.x = 0x4000;
            s16 yaw = actor->shape_angle.y;
            actor->speedF = 22.0f;
            actor->mPitchOffset = 0;
            actor->gravity = 1.0f;
            actor->speed.y = -65.0f;
            actor->mTimers[0] = 47;
            actor->shape_angle.y = (s16)(yaw - 32768);
            GmGuest<daPy_py_c>(gabi::ea(player))->mDemo.mDemoMode = 0;
            actor->mActionState = (s16)((s16)actor->mActionState + 1);
        }
        break;
    }
    case 208: {
        if ((s16)actor->mTimers[0] == 37)
            GmGuest<daPy_py_c>(gabi::ea(player))->mDemo.mDemoMode = 24;
        if (gabi::ea(actor))
            gm_actionSound(actor, 0x7031);
        f32 height = (f32)player->current.pos.y + 220.0f;
        if ((f32)actor->current.pos.y < height) {
            actor->gravity = 0.0f;
            actor->speed.y = 0.0f;
            height = (f32)player->current.pos.y + 220.0f;
            GmHD_cLib_addCalc2_Float(&actor->current.pos.y, height, 1.0f, 50.0f);
        }
        s16 yaw = actor->current.angle.y;
        GmHD_cLib_addCalcAngleS2_ShortTarget(&actor->shape_angle.y, yaw, 1, 0x1000);
        s16 speed = (s16)gabi::ftoi(l_HIO.mDemoPitchStep + 1280.0f);
        GmHD_cLib_addCalcAngleS2_ShortStep(&actor->shape_angle.x, 0, 1, speed);
        f32 step = l_HIO.mDemoCameraStep + 50.0f;
        f32 target = actor->current.pos.x;
        GmHD_cLib_addCalc2_VectorX(&actor->mCameraCenter, target, 1.0f, step);
        target = (f32)actor->current.pos.y + l_HIO.mCameraEyeX;
        GmHD_cLib_addCalc2_Float(&actor->mCameraCenter.y, target, 1.0f, step);
        target = actor->current.pos.z;
        GmHD_cLib_addCalc2_Float(&actor->mCameraCenter.z, target, 1.0f, step);
        target = l_HIO.mCameraOffsetX + 67.0f;
        GmHD_cLib_addCalc2_VectorX(&actor->mCameraEye, target, 1.0f, step);
        target = l_HIO.mCameraOffsetY + 95.0f;
        GmHD_cLib_addCalc2_Float(&actor->mCameraEye.y, target, 1.0f, step);
        target = l_HIO.mCameraOffsetZ + 1.0f;
        GmHD_cLib_addCalc2_Float(&actor->mCameraEye.z, target, 1.0f, step);
        if ((s16)actor->mTimers[0] == 0) {
            play = gm_play();
            GmHD_dVibration_c_StopQuake(&GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mVibration,
                                        32);
            gabi::Local<cXyz> center, eye;
            gm_cameraVectors(actor, center.get(), eye.get());
            GmHD_dCamera_c_Reset(cameraBody, center.get(), eye.get());
            GmHD_dCamera_c_Start(cameraBody);
            GmHD_dCamera_c_SetTrimSize(cameraBody, 0);
            play = gm_play();
            GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mEvtCtrl.mEventFlag =
                (u16)GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mEvtCtrl.mEventFlag | 8;
            actor->attention_info.distances[2] = 4;
            actor->current.angle.x = 0;
            actor->mDemoMode = 0;
            actor->attention_info.flags = 4;
            GmHD_mDoAud_subBgmStart(0x80000019u);
            actor->actor_status = ((u32)actor->actor_status | 0x20u) & ~0x4000u;
            actor->mActionState = 0;
            actor->mAction = GM_ACTION_FLY;
            u32 morf = gabi::ea(actor->mpMorf.get());
            u32 model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
            u32 flags = (u32)GmGuest<GmModelHD>(model)->mMaterialFlags | 1;
            GmGuest<GmModelHD>(model)->mMaterialFlags = flags;
            GmHD_J3DModel_setMaterialPacketFlags(gabi::at<void>(model), flags);
        }
        break;
    }
    case 220:
        GmHD_mDoAud_subBgmStop();
        actor->mTimers[0] = 200;
        actor->mActionState = (s16)((s16)actor->mActionState + 1);
        [[fallthrough]];
    case 221: {
        if ((f32)actor->speedF != 0.0f &&
            ((s16)actor->mTimers[0] == 0 || ((u32)actor->mAcch.m_flags & 0x20))) {
            if ((s32)actor->mAnimation != dRes_INDEX_GM_BCK_G_DEAD02_e)
                anm_init(actor, dRes_INDEX_GM_BCK_G_DEAD02_e, 1.0f, 0, 1.0f, -1);
            actor->speedF = 0.0f;
        }
        u32 playerVtable = player->__vtbl;
        play = gm_play();
        void* other =
            gabi::at<void>(GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mPlayerInfo[0].mpPlayer.v);
        s32 yaw = GmHD_fopAcM_searchActorAngleY_StorageWide(actor, other);
        u32 function = GmGuest<GmPlayerMethodsHD>(playerVtable)->setPlayerPosAndAngle;
        gabi::call_ptr(function, player, &player->current.pos, (s16)(yaw + 32768));
        if (l_HIO.mPauseDeathDemo == 0 && (f32)actor->speedF == 0.0f &&
            (s32)actor->mAnimation == dRes_INDEX_GM_BCK_G_DEAD02_e && gm_animationStopped(actor)) {
            GmGuest<mDoExt_brkAnm>(actor->mpBrkAnms[3].v)->mFrameCtrl.mFrame = 0.0f;
            actor->mBrkIndex = 3;
            actor->mActionState = (s16)((s16)actor->mActionState + 1);
        }
        break;
    }
    case 222: {
        u32 animation = gabi::ea(actor->mpBrkAnms[3].get());
        bool stopped = ((u8)GmGuest<mDoExt_brkAnm>(animation)->mFrameCtrl.mState & 1) != 0;
        if (!stopped)
            stopped = (f32)GmGuest<mDoExt_brkAnm>(animation)->mFrameCtrl.mRate == 0.0f;
        if (stopped) {
            u8 stealBit = actor->stealItemBitNo;
            GmHD_fopAcM_createDisappear(actor, &actor->current.pos, 10, 0, stealBit);
            s16 next = (s16)((s16)actor->mActionState + 1);
            actor->scale.x = 0.0f;
            actor->scale.y = 0.0f;
            actor->scale.z = 0.0f;
            actor->mTimers[0] = 100;
            actor->mActionState = next;
        }
        break;
    }
    case 223:
        if ((s16)actor->mTimers[0] == 0) {
            play = gm_play();
            GmHD_dVibration_c_StopQuake(&GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mVibration,
                                        32);
            gabi::Local<cXyz> center, eye;
            gm_cameraVectors(actor, center.get(), eye.get());
            GmHD_dCamera_c_Reset(cameraBody, center.get(), eye.get());
            GmHD_dCamera_c_Start(cameraBody);
            GmHD_dCamera_c_SetTrimSize(cameraBody, 0);
            GmGuest<daPy_py_c>(gabi::ea(player))->mDemo.mDemoType = 2;
            GmGuest<daPy_py_c>(gabi::ea(player))->mDemo.mDemoMode = 1;
            play = gm_play();
            GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mEvtCtrl.mEventFlag =
                (u16)GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mEvtCtrl.mEventFlag | 8;
            u8 switchNo = actor->mSwitchNo;
            if (switchNo != 255 && (u8)actor->mSaveSwitch != 0) {
                u32 save = l_saveInfo.v;
                s8 room = actor->current.roomNo;
                GmHD_dSv_info_c_onSwitch(&GmGuest<GmSaveHeaderHD>(save)->mSave, switchNo, room);
            }
            GmHD_fopAcM_delete(actor);
            u32 save = l_saveInfo.v;
            s8 room = actor->home.roomNo;
            u16 id = actor->setID;
            GmHD_dSv_info_c_onActor(&GmGuest<GmSaveHeaderHD>(save)->mSave, id, room);
        }
        break;
    default:
        break;
    }
    if (!cameraOnly && !skipCamera) {
        state = actor->mActionState;
        if (state >= 221) {
            f32 y = (f32)actor->current.pos.y + 50.0f;
            f32 x = actor->current.pos.x;
            actor->mCameraCenter.x = x;
            y += l_HIO.mDeathCenterHeight;
            f32 z = actor->current.pos.z;
            actor->mCameraCenter.y = y;
            actor->mCameraCenter.z = z;
            u32 matrix = gm_currentMatrix();
            s32 yaw = GmHD_fopAcM_searchActorAngleY_StorageWide(actor, player);
            gm_rotate(matrix, GmRotation::YSet, (s16)(yaw + l_HIO.mDemoDelayAngle));
            f32 height = l_HIO.mDeathEyeHeight, distance = l_HIO.mDeathEyeDistance;
            gabi::Local<cXyz> direction;
            direction->x = 0.0f;
            direction->z = distance + 400.0f;
            direction->y = height + 200.0f;
            GmHD_MtxPosition(direction.get(), &actor->mCameraEye);
            GmHD_PSVECAdd(&actor->mCameraEye, &actor->current.pos, &actor->mCameraEye);
            state = actor->mActionState;
        }
    }
    if (!skipCamera && (cameraOnly || (state >= 203 && state != 220))) {
        gabi::Local<cXyz> center, eye;
        gm_cameraVectors(actor, center.get(), eye.get());
        f32 fov = actor->mCameraFov;
        GmHD_dCamera_c_Set(cameraBody, center.get(), eye.get(), 0, fov);
    }
    check->dBgS_PolyPassChk::__vtbl = 0x1000FD44;
    check->dBgS_GrpPassChk::__vtbl = 0x1000FCD4;
    check->cBgS_PolyInfo::__vtbl = 0x1000FCC4;
    GmHD_cBgS_LinChk_dtor(check.get(), 0);
}

/* 0214D128 */
BOOL daGM_Execute(gm_class* actor) {
    if ((u8)actor->m2CD == 0) {
        GmHD_fopAcM_setGbaName(actor, 45, 9, 38);
        if ((s16)actor->mDemoMode != 1) {
            u8 flags = actor->mWingFlags;
            if (flags & 4) {
                gm_removeEmitter(actor->mpEmitters[0], actor->mWingEmitterState[0]);
                flags = actor->mWingFlags;
            } else if (gm_wingEmitter(actor, actor->mpEmitters[0], actor->mWingEmitterState[0],
                                      actor->mWingEmitterTimer[0], 17))
                flags = actor->mWingFlags;
            if (flags & 8)
                gm_removeEmitter(actor->mpEmitters[1], actor->mWingEmitterState[1]);
            else {
                bool attached = false;
                gm_wingEmitter(actor, actor->mpEmitters[1], actor->mWingEmitterState[1],
                               actor->mWingEmitterTimer[1], 19, &attached);
                u32 emitter = actor->mpEmitters[1].v;
                if (attached) {
                    GmGuest<GmEmitterHD>(emitter)->mGlobalRotation.x = 0x471C;
                    GmGuest<GmEmitterHD>(emitter)->mGlobalRotation.y = -0x1555;
                    GmGuest<GmEmitterHD>(emitter)->mGlobalRotation.z = 0;
                    emitter = actor->mpEmitters[1].v;
                    GmGuest<GmEmitterHD>(emitter)->mGlobalTranslation.x = 40.0f;
                    GmGuest<GmEmitterHD>(emitter)->mGlobalTranslation.y = 0.0f;
                    GmGuest<GmEmitterHD>(emitter)->mGlobalTranslation.z = -30.0f;
                }
            }
        }
        s16 state = actor->mActionState;
        if ((s16)actor->mTimers[6] != 0 || state == 34 || state == 208) {
            u8 particleState = actor->mAttackEmitterState;
            if (particleState == 0 || particleState == 1) {
                auto& slot = actor->mpEmitters[particleState == 0 ? 2 : 3];
                u32 emitter = gm_spawnEmitter(actor, particleState == 0 ? 0x81A1 : 0x81A2);
                slot.v = emitter;
                if (emitter)
                    actor->mAttackEmitterState = (u8)actor->mAttackEmitterState + 1;
            }
            if ((s16)actor->mActionState == 34)
                actor->mTimers[6] = 45;
            gm_attachEmitter(actor, actor->mpEmitters[2], 3);
            gm_attachEmitter(actor, actor->mpEmitters[3], 3);
        } else {
            gm_removeEmitter(actor->mpEmitters[2], actor->mAttackEmitterState);
            gm_removeEmitter(actor->mpEmitters[3], actor->mAttackEmitterState);
        }
        if ((u8)actor->m2CD == 0 && GmHD_enemy_ice(&actor->mEnemyIce)) {
            u32 model = GmGuest<mDoExt_McaMorf>(actor->mpMorf.v)->mpModel.v;
            gm_copyMatrix(gabi::ea(&l_matrixStack), model + 0xC8);
            GmHD_mDoExt_McaMorf_calc(actor->mpMorf.get());
            actor->attention_info.position.x = (f32)actor->mJointPositions[0].x;
            actor->eyePos.copy(*&actor->mJointPositions[0]);
            actor->attention_info.position.y = (f32)actor->mJointPositions[0].y + 100.0f;
            actor->attention_info.position.z = (f32)actor->mJointPositions[0].z;
            return TRUE;
        }
    }
    for (u32 i = 0; i < 7; ++i) {
        s16 timer = actor->mTimers[i];
        if (timer != 0)
            actor->mTimers[i] = timer - 1;
    }
    switch ((u8)actor->mAction) {
    case GM_ACTION_FLY:
        action_dousa(actor);
        break;
    case GM_ACTION_WALL:
        action_hane_rakka(actor);
        break;
    case GM_ACTION_UCHIWA:
        action_uchiwa_dousa(actor);
        break;
    case GM_ACTION_DIVE:
        action_totugeki(actor);
        break;
    case GM_ACTION_GROUND:
        GmHD_action_kabehari(actor);
        break;
    case GM_ACTION_FLY_DAMAGE:
        GmHD_action_fly_damage(actor);
        break;
    case GM_ACTION_GROUND_ATTACK:
        GmHD_action_ground_attack(actor);
        break;
    case GM_ACTION_DEMO:
        GmHD_action_demo(actor);
        break;
    }
    gm_rotate(gm_currentMatrix(), GmRotation::YSet, actor->current.angle.y);
    gm_rotate(gm_currentMatrix(), GmRotation::XMultiply, actor->current.angle.x);
    gabi::Local<cXyz> direction, velocity, center;
    direction->x = 0.0f;
    direction->y = 0.0f;
    direction->z = (f32)actor->speedF;
    GmHD_MtxPosition(direction.get(), velocity.get());
    f32 vertical = (f32)actor->speed.y + (f32)actor->gravity;
    actor->speed.x = (f32)velocity->x;
    actor->speed.z = (f32)velocity->z;
    actor->speed.y = vertical;
    u8 larva = actor->m2CD;
    if (vertical < -100.0f)
        actor->speed.y = -100.0f;
    if (!larva) {
        u8 action = actor->mAction;
        if (action != 20) {
            bool groundAction = action == 10;
            if (action != 10) {
                GmHD_body_atari_check(actor);
                action = actor->mAction;
            }
            bool twist = true;
            if (action == 5 && !groundAction) {
                // Damage already selected the wing pose.
            } else if (action == 2 && !groundAction) {
                twist = false;
            } else {
                GmHD_wing_ret_set(actor);
                twist = (u8)actor->mAction != GM_ACTION_UCHIWA;
            }
            if (twist) {
                GmHD_cLib_addCalcAngleS2_ShortTarget(&actor->shape_angle.z, (s16)actor->mTargetRoll,
                                                     1, 0x100);
                GmHD_cLib_addCalcAngleS2_IntTarget(&actor->mTargetRoll, 0, 1, 0x100);
            }
        }
        if ((s16)actor->mTimers[5] == 0) {
            s32 material = 0;
            if ((u32)actor->mAcch.m_flags & 0x20) {
                void* play = gm_play();
                material =
                    GmHD_dBgS_GetMtrlSndId(&GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mBgS,
                                           static_cast<cBgS_PolyInfo*>(&actor->mAcch.m_gnd));
            }
            s32 reverb = GmHD_dComIfGp_getReverb((s8)actor->current.roomNo);
            GmHD_mDoExt_McaMorf_play(actor->mpMorf.get(), &actor->eyePos, material, reverb);
        }
        s16 colour = actor->mBrkIndex;
        u32 brkIndex = colour == 1 ? 2 : colour == 2 ? 1 : colour == 3 ? 3 : 0;
        GmHD_mDoExt_baseAnm_play(actor->mpBrkAnms[brkIndex].get());
        GmHD_cXyz_add(&actor->current.pos, center.get(), &actor->mPositionOffset);
        action = actor->mAction;
        if (action != 20) {
            if (action == 10) {
                actor->eyePos.copy(*&actor->mJointPositions[0]);
                actor->attention_info.position.x = (f32)actor->mJointPositions[0].x;
                actor->attention_info.position.y = (f32)actor->mJointPositions[0].y + 50.0f;
                actor->attention_info.position.z = (f32)actor->mJointPositions[0].z;
                center->x = (f32)actor->mJointPositions[1].x;
                center->z = (f32)actor->mJointPositions[1].z;
                center->y = (f32)actor->mJointPositions[1].y + (f32)actor->mBodyCenterY;
            } else {
                actor->attention_info.position.x = (f32)center->x;
                actor->attention_info.position.y = (f32)center->y;
                actor->attention_info.position.z = (f32)center->z;
                f32 y = (f32)center->y + 100.0f;
                actor->attention_info.position.y = y;
                actor->eyePos.copy(*center);
                center->y = (f32)center->y + (f32)actor->mBodyCenterY;
            }
            if ((s16)actor->mActionState == 34) {
                gabi::Local<cXyz> combined;
                GmHD_cXyz_add(&actor->current.pos, combined.get(), &actor->mPositionOffset);
                center->x = (f32)combined->x;
                center->z = (f32)combined->z;
                center->y = (f32)combined->y - 10.0f;
                GmHD_cM3dGCyl_SetC(&actor->mBodyCyl.mCyl, center.get());
                GmHD_cM3dGCyl_SetH(&actor->mBodyCyl.mCyl, 60.0f);
                GmHD_cM3dGCyl_SetR(&actor->mBodyCyl.mCyl, 375.0f);
            } else {
                GmHD_cM3dGCyl_SetC(&actor->mBodyCyl.mCyl, center.get());
                GmHD_cM3dGCyl_SetH(&actor->mBodyCyl.mCyl, (f32)actor->mBodyRadius);
                GmHD_cM3dGCyl_SetR(&actor->mBodyCyl.mCyl, (f32)actor->mBodyHeight);
            }
            void* play = gm_play();
            GmHD_cCcS_Set_Cylinder(&GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mCcS,
                                   &actor->mBodyCyl);
            gabi::Local<cXyz> combined;
            GmHD_cXyz_add(&actor->current.pos, combined.get(), &actor->mPositionOffset);
            center->x = (f32)combined->x;
            center->z = (f32)combined->z;
            center->y = (f32)combined->y - 40.0f;
            GmHD_cM3dGCyl_SetC(&actor->mDiveCyl.mCyl, center.get());
            GmHD_cM3dGCyl_SetH(&actor->mDiveCyl.mCyl, 60.0f);
            GmHD_cM3dGCyl_SetR(&actor->mDiveCyl.mCyl, 185.0f);
            play = gm_play();
            GmHD_cCcS_Set_Cylinder(&GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mCcS,
                                   &actor->mDiveCyl);
            play = gm_play();
            GmHD_dCcMassS_Mng_Set(
                &GmGuest<GmCcSHD>(gabi::ea(&GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mCcS))
                     ->mMass_Mng,
                &actor->mDiveCyl, 3);
            GmHD_cM3dGSph_SetC(&actor->mWeaponSph.mSph, &actor->mWeaponPos);
            GmHD_cM3dGSph_SetR(&actor->mWeaponSph.mSph, 55.0f);
            play = gm_play();
            GmHD_cCcS_Set_Sphere(&GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mCcS,
                                 &actor->mWeaponSph);
        }
        if ((s16)actor->mDemoMode == 0 && ((u8)actor->mWingFlags & 15) != 15 &&
            (s16)actor->mActionState != 34) {
            gabi::Local<cXyz> combined;
            GmHD_cXyz_add(&actor->current.pos, combined.get(), &actor->mPositionOffset);
            s16 phase = (s16)actor->mBobPhase + 1000;
            actor->mBobPhase = phase;
            f32 x = combined->x;
            f32 z = combined->z;
            center->y = (f32)combined->y;
            center->z = z;
            center->x = x;
            actor->mGroundCenterOffset.x = GmHD_cosLookup(&l_sincos, phase) * 300.0f;
            actor->mGroundCenterOffset.z =
                GmHD_cosLookup(&l_sincos, (s16)actor->mBobPhase) * 300.0f;
            center->x = x + (f32)actor->mGroundCenterOffset.x;
            center->z = z + (f32)actor->mGroundCenterOffset.z;
            center->y = (f32)actor->mAcch.m_ground_h;
            GmHD_cM3dGCyl_SetC(&actor->mWindCyl.mCyl, center.get());
            GmHD_cM3dGCyl_SetH(&actor->mWindCyl.mCyl, 60.0f);
            GmHD_cM3dGCyl_SetR(&actor->mWindCyl.mCyl, 200.0f);
            void* play = gm_play();
            GmHD_cCcS_Set_Cylinder(&GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mCcS,
                                   &actor->mWindCyl);
            u8 actionNow = actor->mAction;
            if (actionNow != 5 && actionNow != 2) {
                play = gm_play();
                GmHD_dCcMassS_Mng_Set(
                    &GmGuest<GmCcSHD>(gabi::ea(&GmGuest<dComIfG_inf_c>(gabi::ea(play))->play.mCcS))
                         ->mMass_Mng,
                    &actor->mWindCyl, 3);
            }
        }
    }
    bool noStatus = (s16)actor->mActionState == 34 || (u8)actor->mAction == GM_ACTION_DEMO;
    GmHD_fopAcM_posMove(actor, noStatus ? gabi::at<void>(0) : &actor->mStts);
    GmHD_BG_check(actor);
    GmHD_draw_SUB(actor);
    return TRUE;
}

/* 0214FFD0 */
BOOL daGM_IsDelete(gm_class* actor) { return TRUE; }

/* 0214FFD8 */
BOOL daGM_Delete(gm_class* actor) {
    GmHD_dComIfG_resDelete(&actor->mPhase, "Gm");
    if (gabi::ea((JKRSolidHeap*)actor->heap))
        GmHD_mDoExt_McaMorf_stopZelAnime(actor->mpMorf.get());
    GmHD_enemy_fire_remove(&actor->mEnemyFire);
    if ((s8)actor->health == -128) {
        u8 switchNo = actor->mSwitchNo;
        if (switchNo != 255 && (u8)actor->mSaveSwitch != 0) {
            u32 save = l_saveInfo.v;
            s8 room = actor->current.roomNo;
            GmHD_dSv_info_c_onSwitch(&GmGuest<GmSaveHeaderHD>(save)->mSave, switchNo, room);
        }
        u32 save = l_saveInfo.v;
        s8 room = actor->home.roomNo;
        u16 id = actor->setID;
        GmHD_dSv_info_c_onActor(&GmGuest<GmSaveHeaderHD>(save)->mSave, id, room);
    }
    for (auto& slot : actor->mpEmitters) {
        u32 particle = slot.v;
        if (particle) {
            u32 flags = GmGuest<GmEmitterHD>(particle)->mStatus;
            GmGuest<GmEmitterHD>(particle)->mMaxFrame = -1;
            GmGuest<GmEmitterHD>(particle)->mStatus = flags | 1;
            slot.v = 0;
        }
    }
    return TRUE;
}

/* 02150124 */
BOOL useHeapInit(gm_class* actor) {
    u8 larva = actor->m2CD;
    if (!larva) {
        void* resource = gm_resource(gabi::guest_addr("Gm"), dRes_INDEX_GM_BDL_GM_e);
        void* morf = GmHD_mDoExt_McaMorf_mDoExt_McaMorf(gabi::at<void>(0), resource, 0, 0, 0, 1, 0,
                                                        -1, 1, 0, 0x80000, 0x37441422, 1.0f);
        actor->mpMorf.v = gabi::ea(morf);
    } else {
        u8 type = actor->mWingFlags;
        if (type >= 1 && type <= 4) {
            void* resource =
                gm_resource(gabi::guest_addr("Gm"),
                            type <= 2 ? dRes_INDEX_GM_BDL_HANESL_e : dRes_INDEX_GM_BDL_HANEUL_e);
            void* morf = GmHD_mDoExt_McaMorf_mDoExt_McaMorf(gabi::at<void>(0), resource, 0, 0, 0, 2,
                                                            0, -1, 0, 0, 0, 0x11020203, 1.0f);
            actor->mpMorf.v = gabi::ea(morf);
        }
    }
    u32 morf = gabi::ea(actor->mpMorf.get());
    if (!morf)
        return FALSE;
    u32 model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
    if (!model)
        return FALSE;
    if ((u8)actor->m2CD == 0) {
        GmGuest<GmModelHD>(model)->mUserArea = gabi::ea(actor);
        morf = gabi::ea(actor->mpMorf.get());
        model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
        u32 data = GmGuest<GmModelHD>(model)->mModelData.v;
        void* countObject = GmHD_J3DModelData_getJointTree(gabi::at<void>(data));
        u16 count = GmGuest<GmJointTreeHD>(gabi::ea(countObject))->mJointCount;
        for (u16 i = 0; i < count; ++i) {
            morf = gabi::ea(actor->mpMorf.get());
            model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
            data = GmGuest<GmModelHD>(model)->mModelData.v;
            u32 bound = GmGuest<J3DModelData>(data)->mJointNum;
            u32 joint = GmGuest<J3DModelData>(data)->mJointNodes;
            if (i < bound)
                joint += i * 28;
            GmGuest<GmJointHD>(joint)->mCallBack = gabi::guest_fn((const void*)nodeCallBack);
            morf = gabi::ea(actor->mpMorf.get());
            model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
            data = GmGuest<GmModelHD>(model)->mModelData.v;
            countObject = GmHD_J3DModelData_getJointTree(gabi::at<void>(data));
            count = GmGuest<GmJointTreeHD>(gabi::ea(countObject))->mJointCount;
        }
        larva = actor->m2CD;
        morf = gabi::ea(actor->mpMorf.get());
    } else
        larva = actor->m2CD;
    model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
    if (larva)
        return TRUE;
    for (u32 i = 0; i < 4; ++i) {
        void* animation = GmHD_operator_new_Storage(0x78);
        if (gabi::ea(animation))
            animation = GmHD_mDoExt_brkAnm_init(animation);
        actor->mpBrkAnms[i].v = gabi::ea(animation);
        if (!gabi::ea(animation))
            return FALSE;
        void* resource = gm_resource(gabi::guest_addr("Gm"), i == 0   ? dRes_INDEX_GM_BRK_GM_e
                                                             : i == 1 ? dRes_INDEX_GM_BRK_ATACK_e
                                                             : i == 2 ? dRes_INDEX_GM_BRK_DAMAGE_e
                                                                      : dRes_INDEX_GM_BRK_DEAD_e);
        animation = actor->mpBrkAnms[i].get();
        u32 data = GmGuest<GmModelHD>(model)->mModelData.v;
        if (!GmHD_mDoExt_brkAnm_init(animation, gabi::at<void>(data), resource, 1, i < 2 ? 2 : 0, 0,
                                     -1, 0, 0, 1.0f))
            return FALSE;
    }
    morf = gabi::ea(actor->mpMorf.get());
    model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
    return GmHD_mDoExt_invisibleModel_create(&actor->mInvisibleModel, gabi::at<void>(model))
               ? TRUE
               : FALSE;
}

/* 021507C4 */
s32 daGM_Create(gm_class* actor) {
    u32 condition = actor->actor_condition;
    if (!(condition & 8)) {
        if (gabi::ea(actor)) {
            gm_constructor(actor);
            condition = actor->actor_condition;
        }
        actor->actor_condition = condition | 8;
    }
    s32 phase = GmHD_dComIfG_resLoad(&actor->mPhase, "Gm");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    u32 parameters = actor->mParameters;
    actor->mWingFlags = parameters;
    u8 type = (u32)actor->mParameters >> 8;
    actor->m2CD = type;
    actor->mType = (u32)actor->mParameters >> 16;
    actor->mSwitchNo = (u32)actor->mParameters >> 24;
    if ((u8)parameters == 255)
        actor->mWingFlags = 0;
    u8 special = actor->mType;
    if (type == 255) {
        type = 0;
        actor->m2CD = 0;
    }
    if (special == 255) {
        type = actor->m2CD;
        actor->mType = 0;
    }
    if (type == 0) {
        actor->mSaveSwitch = (s16)actor->current.angle.z;
        actor->shape_angle.x = 0;
        actor->current.angle.z = 0;
        actor->current.angle.x = 0;
        actor->shape_angle.z = 0;
    }
    u8 switchNo = actor->mSwitchNo;
    if (switchNo != 255) {
        u32 save = l_saveInfo.v;
        s8 room = l_startRoomNo;
        if (GmHD_dSv_info_c_isSwitch(&GmGuest<GmSaveHeaderHD>(save)->mSave, switchNo, room))
            return cPhs_ERROR_e;
    }
    s16 overrideType = l_HIO.mTypeOverride;
    if (overrideType != 0)
        actor->mType = (s16)(overrideType - 1);
    if (!GmHD_fopAcM_entrySolidHeap(actor, gabi::guest_fn((const void*)useHeapInit), 0x7400))
        return cPhs_ERROR_e;
    gabi::Local<SafeString> modelName, expectedName, wing1, wing2, wing3, wing4;
    modelName->__vtbl = expectedName->__vtbl = wing1->__vtbl = wing2->__vtbl = wing3->__vtbl =
        wing4->__vtbl = gc_tu.safeString;
    expectedName->mStringTop = gabi::guest_addr("gm");
    wing1->mStringTop = gabi::guest_addr("hane");
    wing2->mStringTop = gabi::guest_addr("haneB");
    wing3->mStringTop = gabi::guest_addr("haneR");
    wing4->mStringTop = gabi::guest_addr("haneBR");
    u32 morf = gabi::ea(actor->mpMorf.get());
    u32 model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
    u32 nameObject = GmGuest<GmModelHD>(model)->mpResourceHeader.v;
    u32 modelNameEA = gm_relative(GmGuest<GmResourceHeaderHD>(nameObject)->mModelNameOffset);
    modelName->mStringTop = modelNameEA;
    gm_emptyVirtual((gm_class*)expectedName.get());
    gm_safeAssure(expectedName.get());
    u32 expectedEA = expectedName->mStringTop;
    gm_safeAssure(modelName.get());
    modelNameEA = modelName->mStringTop;
    bool matching = expectedEA == modelNameEA;
    if (!matching) {
        u32 a = expectedName->mStringTop, b = modelName->mStringTop;
        matching = true;
        // HD: the name comparison is capped; additions wrap in the guest address space.
        for (u32 i = 0; i < 0x40001; ++i) {
            u8 x = *gabi::at<be<u8>>(a + i), y = *gabi::at<be<u8>>(b + i);
            if (x != y) {
                matching = false;
                break;
            }
            if (!x)
                break;
        }
    }
    if (matching) {
        SafeString* names[4] = {wing1.get(), wing2.get(), wing3.get(), wing4.get()};
        for (u32 i = 0; i < 4; ++i) {
            gm_safeAssure(names[i]);
            nameObject = GmGuest<GmModelHD>(model)->mpResourceHeader.v;
            u32 nameTable =
                gm_relative(GmGuest<GmResourceHeaderHD>(nameObject)->mMaterialNameOffset);
            u32 name = names[i]->mStringTop;
            u32 index =
                GmHD_JUTNameTab_getIndex(gabi::at<void>(nameTable), gabi::at<const char>(name));
            l_wingNames.mIndices[i] = index;
        }
    }
    actor->max_health = 12;
    actor->health = 12;
    morf = gabi::ea(actor->mpMorf.get());
    model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
    actor->cullMtx = model ? model + 0xC8 : 0;
    GmHD_fopAcM_setCullSizeBox(actor, -200.0f, -150.0f, -200.0f, 200.0f, 250.0f, 150.0f);
    GmHD_dBgS_Acch_Set(&actor->mAcch, &actor->current.pos, &actor->old.pos, actor, 1,
                       &actor->mAcchCir, &actor->speed, 0, 0);
    void* status = &actor->mStts;
    GmHD_dCcD_Stts_Init(status, 100, 1, actor);
    actor->mGroundOffset = 0.0f;
    actor->mTargetYaw = actor->current.angle.y;
    actor->mHomePos.copy(actor->current.pos);
    void* play = gm_play();
    actor->itemTableIdx = GmHD_cDT_NamePTbl_GetIndex(
        &GmGuest<GmNameTableHD>(gabi::ea(&GmGuest<dComIfG_inf_c>(gabi::ea(play))->play))
             ->mNameTable,
        "gmos", 0);
    if ((u8)actor->m2CD != 0) {
        actor->attention_info.flags = 0;
        u8 larvaType = actor->mWingFlags;
        actor->mWallRadius = 100.0f;
        if (larvaType == 2 || larvaType == 4)
            actor->shape_angle.y = (s16)((s16)actor->shape_angle.y - 32768);
        actor->mAction = GM_ACTION_WALL;
        actor->mActionState = 10;
        return phase;
    }
    GmHD_dCcD_Cyl_Set(&actor->mBodyCyl, &l_bodyCylSource);
    actor->mBodyCyl.mStts.v = gabi::ea(status);
    GmHD_dCcD_Sph_Set(&actor->mWeaponSph, &l_weaponSphSource);
    actor->mWeaponSph.mStts.v = gabi::ea(status);
    GmHD_dCcD_Cyl_Set(&actor->mDiveCyl, &l_diveCylSource);
    actor->mDiveCyl.mStts.v = gabi::ea(status);
    GmHD_dCcD_Cyl_Set(&actor->mWindCyl, &l_windCylSource);
    actor->mWindCyl.mStts.v = gabi::ea(status);
    actor->mDiveCyl.mObjAt.mSPrm = (u32)actor->mDiveCyl.mObjAt.mSPrm & ~1u;
    actor->mWeaponSph.mObjAt.mSPrm = (u32)actor->mWeaponSph.mObjAt.mSPrm & ~1u;
    actor->attention_info.flags = 4;
    actor->attention_info.distances[2] = 4;
    f32 bodyR = (f32)l_HIO.mBtRadius + 100.0f;
    u8 specialType = actor->mType;
    f32 height = l_HIO.mBtHeight;
    actor->mBtBodyR = bodyR;
    actor->mBtHeight = height + (specialType == 1 ? 120.0f : 200.0f);
    f32 maxDistance = l_HIO.mBtDistance;
    actor->mBtStartFrame = 0.0f;
    actor->mBtEndFrame = 10.0f;
    actor->mBtMaxDis = maxDistance + (specialType == 1 ? 800.0f : 880.0f);
    actor->mBtAttackType = 1;
    actor->stealItemLeft = specialType == 1 ? 1 : 3;
    if (specialType != 1)
        actor->mWeaponSph.mGObjAt.mSpl = 5;
    actor->scale.x = 0.0f;
    actor->scale.y = 0.0f;
    actor->scale.z = 0.0f;
    actor->mBtNowFrame = 1000.0f;
    if (specialType == 1)
        actor->attention_info.distances[2] = 3;
    morf = gabi::ea(actor->mpMorf.get());
    actor->mEnemyIce.mCylHeight = 40.0f;
    actor->mEnemyIce.mWallRadius = 40.0f;
    actor->mEnemyIce.mpActor.v = gabi::ea(actor);
    actor->mEnemyFire.mpActor.v = gabi::ea(actor);
    actor->mEnemyFire.mpMcaMorf.v = morf;
    actor->mEnemyIce.m1B0 = 1;
    actor->mAction = GM_ACTION_DEMO;
    actor->mActionState = 200;
    for (u32 i = 0; i < 10; ++i) {
        actor->mEnemyFire.mFlameJntIdxs[i] = l_flameJoints.mValues[i];
        actor->mEnemyFire.mParticleScale[i] = l_flameScales.mValues[i];
    }
    draw_SUB(actor);
    specialType = actor->mType;
    if (specialType == 0) {
        actor->actor_status = ((u32)actor->actor_status | 0x04004000u) & ~0x20u;
        morf = gabi::ea(actor->mpMorf.get());
        model = GmGuest<mDoExt_McaMorf>(morf)->mpModel.v;
        u32 flags = (u32)GmGuest<GmModelHD>(model)->mMaterialFlags & ~1u;
        GmGuest<GmModelHD>(model)->mMaterialFlags = flags;
        GmHD_J3DModel_setMaterialPacketFlags(gabi::at<void>(model), flags);
    } else if (specialType == 1 || specialType == 2) {
        play = gm_play();
        actor->itemTableIdx = GmHD_cDT_NamePTbl_GetIndex(
            &GmGuest<GmNameTableHD>(gabi::ea(&GmGuest<dComIfG_inf_c>(gabi::ea(play))->play))
                 ->mNameTable,
            (specialType == 2 ? "gmos_f" : "gmos_g"), 0);
        if (specialType == 1) {
            actor->mWallRadius = 70.0f;
            actor->mWallHeight = 60.0f;
        }
        actor->scale.x = 1.0f;
        actor->scale.y = 1.0f;
        actor->scale.z = 1.0f;
        if (specialType == 2) {
            actor->mActionState = 0;
            actor->mAction = GM_ACTION_FLY;
        } else {
            actor->mWingFlags = 15;
            actor->max_health = 2;
            actor->health = 2;
            actor->mActionState = 60;
            actor->mAction = GM_ACTION_GROUND_ATTACK;
        }
    }
    return phase;
}

// HD: compiler-generated and template code of this translation unit.

/* 02151294 */
f32 gm_cosLookup(void* table, u16 angle) {
    return GmGuest<GmSinCosTable>(gabi::ea(table))->mSamples[angle >> 3].mCos;
}

/* 021510EC */
void gm_staticDestructor(void* object, s32 flags) {
    if (gabi::ea(object) && (flags & 1))
        GmHD_operator_delete_Storage(object);
}

/* 02154138 */
void gm_emptyVirtual(gm_class* actor) {}

/* 02150578 */
void* gm_fireConstructor(void* object) {
    u32 base = gabi::ea(object);
    if (!base) {
        object = GmHD_operator_new_Storage(0x22C);
        base = gabi::ea(object);
        if (!base)
            return object;
    }
    if (!(base + 0x8C))
        GmHD_operator_new_Storage(12);
    GmHD_cCcD_Stts_ctor_Storage(&GmGuest<GmEnemyFire>(base)->mStts);
    GmHD_dCcD_GStts_dCcD_GStts(&GmGuest<GmEnemyFire>(base)->mStts.__vtbl_gstts);
    GmGuest<GmEnemyFire>(base)->mStts.__vtbl = 0x1004AE88;
    GmGuest<GmEnemyFire>(base)->mStts.__vtbl_gstts = 0x1004AEC0;
    GmHD_dCcD_Sph_ctor_Storage(&GmGuest<GmEnemyFire>(base)->mSph);
    GmGuest<GmEnemyFire>(base)->mLight.mHD20 = 1.0f;
    return object;
}

/* 02150604 */
gm_class* gm_constructor(gm_class* actor) {
    if (!gabi::ea(actor)) {
        actor = GmHD_operator_new_Actor(0x1290);
        if (!gabi::ea(actor))
            return actor;
    }
    GmHD_fopAc_ac_c_fopAc_ac_c(actor);
    actor->__vtbl = 0x1000FDD4;
    GmHD_dBgS_AcchCir_dBgS_AcchCir(&actor->mAcchCir);
    GmHD_dBgS_Acch_dBgS_Acch(&actor->mAcch);
    actor->mAcch.cBgS_Chk::__vtbl = gc_tu.objAcch.v10;
    actor->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    actor->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    actor->mAcch.mbObjThrough = 1;
    GmHD_cCcD_Stts_ctor_Status(&actor->mStts);
    GmHD_dCcD_GStts_dCcD_GStts(&actor->mStts.__vtbl_gstts);
    actor->mStts.__vtbl = 0x1004AE88;
    actor->mStts.__vtbl_gstts = 0x1004AEC0;
    gm_cylinderConstructor(&actor->mBodyCyl);
    gm_cylinderConstructor(&actor->mDiveCyl);
    gm_cylinderConstructor(&actor->mWindCyl);
    GmHD_dCcD_Sph_ctor_Sphere(&actor->mWeaponSph);
    GmHD_cCcD_Stts_ctor_Status(&actor->mEnemyIce.mStts);
    GmHD_dCcD_GStts_dCcD_GStts(&actor->mEnemyIce.mStts.__vtbl_gstts);
    actor->mEnemyIce.mStts.__vtbl = 0x1004AE88;
    actor->mEnemyIce.mStts.__vtbl_gstts = 0x1004AEC0;
    gm_cylinderConstructor(&actor->mEnemyIce.mCyl);
    GmHD_dBgS_AcchCir_dBgS_AcchCir(&actor->mEnemyIce.mBgAcchCir);
    GmHD_dBgS_Acch_dBgS_Acch(&actor->mEnemyIce.mBgAcch);
    actor->mEnemyIce.mBgAcch.mbObjThrough = 1;
    actor->mEnemyIce.mBgAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    actor->mEnemyIce.mBgAcch.cBgS_Chk::__vtbl = gc_tu.objAcch.v10;
    actor->mEnemyIce.mBgAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    gm_fireConstructor(&actor->mEnemyFire);
    GmHD_mDoExt_invisibleModel_ctor(&actor->mInvisibleModel);
    return actor;
}

/* 0215400C */
void gm_destructor(gm_class* actor, s32 flags) {
    if (!gabi::ea(actor))
        return;
    GmHD_mDoExt_invisibleModel_dtor(&actor->mInvisibleModel, 2);
    GmHD_dCcD_Sph_dtor_Storage(&actor->mEnemyFire.mSph, 2);
    GmHD_dCcD_Stts_dtor_Storage(&actor->mEnemyFire.mStts, 2);
    actor->mEnemyIce.mBgAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    actor->mEnemyIce.mBgAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    GmHD_dBgS_Acch_dtor(&actor->mEnemyIce.mBgAcch, 0);
    GmHD_cM3dGCir_dtor(&actor->mEnemyIce.mBgAcchCir.m_cir, 2);
    GmHD_dCcD_Cyl_dtor(&actor->mEnemyIce.mCyl, 2);
    GmHD_dCcD_Stts_dtor_Status(&actor->mEnemyIce.mStts, 2);
    GmHD_dCcD_Sph_dtor_Sphere(&actor->mWeaponSph, 2);
    GmHD_dCcD_Cyl_dtor(&actor->mWindCyl, 2);
    GmHD_dCcD_Cyl_dtor(&actor->mDiveCyl, 2);
    GmHD_dCcD_Cyl_dtor(&actor->mBodyCyl, 2);
    GmHD_dCcD_Stts_dtor_Status(&actor->mStts, 2);
    actor->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    actor->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    GmHD_dBgS_Acch_dtor(&actor->mAcch, 0);
    GmHD_cM3dGCir_dtor(&actor->mAcchCir.m_cir, 2);
    GmHD_fopAc_ac_c_dtor_fopAc_ac_c(actor, 0);
    if (flags & 1)
        GmHD_operator_delete_Actor(actor);
}

/* 02150FF8 */
void gm_staticInit() {
    l_counters.mValues[3] = 0;
    l_counters.mValues[2] = 0;
    l_counters.mValues[1] = 0;
    l_counters.mValues[0] = 0;
    GmHD___register_global_object(&l_registrations.mEntries[0]);
    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;
    GmHD_cXyz_defaultConstruct(&l_defaultVector);
    GmHD___register_global_object(&l_registrations.mEntries[1]);
    GmHD_csXyz_defaultConstruct(&l_defaultAngles);
    GmHD___register_global_object(&l_registrations.mEntries[2]);
    GmHD_csXyz_csXyz(&l_wingPose.mAngles[0], 10000, -12000, -2000);
    GmHD_csXyz_csXyz(&l_wingPose.mAngles[1], -10000, 12000, -2000);
    GmHD_csXyz_csXyz(&l_wingPose.mAngles[2], -2000, -12000, 2000);
    GmHD_csXyz_csXyz(&l_wingPose.mAngles[3], 2000, 12000, 2000);
}

// clang-format off
/* WWHD glue
str 1000FEDB "Gm" in 02150124
str 1000FF18 "gm" in 021507C4
data l_sincos 104A44F8
data l_HIO 1047B608
data l_larvaOffsets 10463FB0
data l_larvaOffsetsInitialized 10463FA8
data l_wingPose 10463FE0
data l_wingNames 101B5498
data l_counters 10463FF8
data l_registrations 101B5474
data l_minAngle 10463FA0
data l_maxAngle 10463FA4
data l_defaultVector 10463FAC
data l_defaultAngles 10463FAD
data l_currentModel 104B462C
data l_jointMatrix 104B4868
data l_matrixStack 1048D0CC
data l_currentMatrix 1018C7B0
data l_saveInfo 101F84DC
data l_resourceControl 101F4F28
data l_startRoomNo 1047E6C8
data l_probeCoordinates 101B5310
data l_flameScales 101B5374
data l_flameJoints 101B539C
data l_bodyCylSource 101B53A8
data l_diveCylSource 101B53EC
data l_windCylSource 101B5430
data l_weaponSphSource 101B5334
str 1000FDF4 "Gm" in 0214C1C8
str 1000FED8 "Gm" in 0214FFD8
str 1000FF04 "Gm" in 021507C4
str 1000FF08 "hane" in 021507C4
str 1000FEF4 "haneB" in 021507C4
str 1000FEFC "haneR" in 021507C4
str 1000FF10 "haneBR" in 021507C4
str 1000FF1C "gmos" in 021507C4
str 1000FF24 "gmos_f" in 021507C4
str 1000FF2C "gmos_g" in 021507C4
ext 025F18EC void GmHD_mDoMtx_XrotS(Mtx34*, s16)
ext 02008860 int GmHD_cBgS_LineCross(void *, dBgS_LinChk *)
ext 02008B4C void GmHD_cBgS_LinChk_dtor(dBgS_LinChk *, int)
ext 0200BD2C void GmHD_cCcD_Stts_ctor_Status(dCcD_Stts *)
ext 0200BD2C void GmHD_cCcD_Stts_ctor_Storage(void *)
ext 0200E240 void GmHD_cCcS_Set_Cylinder(void *, dCcD_Cyl *)
ext 0200E240 void GmHD_cCcS_Set_Sphere(void *, dCcD_Sph *)
ext 0200E814 int GmHD_cDT_NamePTbl_GetIndex(void *, const char *, int)
ext 0200ED84 void GmHD_cLib_addCalc2_VectorX(cXyz *, float, float, float)
ext 0200ED84 void GmHD_cLib_addCalc2_Float(gabi::be<float> *, float, float, float)
ext 0200EDC8 void GmHD_cLib_addCalc0(gabi::be<float> *, float, float)
ext 0200F428 void GmHD_cLib_addCalcAngleS2_VectorX(csXyz *, int, int, int)
ext 0200F428 void GmHD_cLib_addCalcAngleS2_IntTarget(gabi::be<short> *, int, int, int)
ext 0200F428 void GmHD_cLib_addCalcAngleS2_ShortStep(gabi::be<short> *, int, int, short)
ext 0200F428 void GmHD_cLib_addCalcAngleS2_ShortTarget(gabi::be<short> *, short, int, int)
ext 0200F428 void GmHD_cLib_addCalcAngleS2_ShortTargetStep(gabi::be<short> *, short, int, short)
ext 0200FAAC int GmHD_cLib_distanceAngleS_IntTarget(short, int)
ext 0200FAAC int GmHD_cLib_distanceAngleS_ShortTarget(short, short)
ext 0200FCD8 void GmHD_MtxPosition(cXyz *, cXyz *)
ext 02018034 void GmHD_cM3dGCir_dtor(void *, int)
ext 020182E0 void GmHD_cM3dGCyl_SetC(void *, cXyz *)
ext 02018428 void GmHD_cM3dGCyl_SetH(void *, float)
ext 020184DC void GmHD_cM3dGCyl_SetR(void *, float)
ext 02018590 void GmHD_cM3dGCyl_ctor(void *)
ext 02018C8C void GmHD_cM3dGSph_SetR(void *, float)
ext 02018D40 void GmHD_cM3dGSph_SetC(void *, cXyz *)
ext 020195B0 int GmHD_cM_atan2s_WideResult(float, float)
ext 020195B0 short GmHD_cM_atan2s_AngleResult(float, float)
ext 02019788 float GmHD_cM_rnd()
ext 020198D8 float GmHD_cM_rndF(float)
ext 02019918 float GmHD_cM_rndFX(float)
ext 0201A478 void GmHD_csXyz_csXyz(csXyz *, int, int, int)
ext 0201AD78 void GmHD_cXyz_add(cXyz *, cXyz *, cXyz *)
ext 0201ADE0 void GmHD_cXyz_subtract(cXyz *, cXyz *, cXyz *)
ext 020402C8 int GmHD_enemy_ice(void *)
ext 02041570 void GmHD_enemy_fire(void *)
ext 02041C30 void GmHD_enemy_fire_remove(void *)
ext 02041CAC void GmHD_enemy_piyo_set(gm_class *)
ext 0214BDFC void GmHD_draw_SUB(gm_class *)
ext 0214C1C8 void GmHD_anm_init(gm_class *, int, float, int, float, int)
ext 0214C2F4 void GmHD_body_atari_check(gm_class *)
ext 0214CDB4 int GmHD_Line_check(gm_class *, cXyz *)
ext 0214CFA4 void GmHD_ks_set_rtn(gm_class *)
ext 02151100 void GmHD_BG_check(gm_class *)
ext 021511B8 void GmHD_wing_ret_set(gm_class *)
ext 02151294 float GmHD_cosLookup(void *, short)
ext 021512AC void GmHD_action_kabehari(gm_class *)
ext 0215163C void GmHD_action_fly_damage(gm_class *)
ext 02151B98 void GmHD_action_ground_attack(gm_class *)
ext 02153350 void GmHD_action_demo(gm_class *)
ext 024EECAC int GmHD_dBgS_GetMtrlSndId(void *, void *)
ext 024EFD9C void GmHD_dBgS_Acch_dtor(dBgS_ObjAcch *, int)
ext 024EFE94 void GmHD_dBgS_AcchCir_dBgS_AcchCir(dBgS_AcchCir *)
ext 024EFF44 void GmHD_dBgS_AcchCir_SetWall(dBgS_AcchCir *, float, float)
ext 024F0474 void GmHD_dBgS_Acch_dBgS_Acch(dBgS_ObjAcch *)
ext 024F06B4 void GmHD_dBgS_Acch_Set(dBgS_ObjAcch *, cXyz *, cXyz *, gm_class *, int, dBgS_AcchCir *, cXyz *, int, int)
ext 024F08A8 void GmHD_dBgS_Acch_CrrPos(dBgS_ObjAcch *, void *)
ext 024F1AFC void GmHD_dBgS_LinChk_Set(dBgS_LinChk *, cXyz *, cXyz *, gm_class *)
ext 024F8000 int GmHD_dCam_getAngleY(void *)
ext 02514F2C void GmHD_dCamera_c_Stop(void *)
ext 02514F38 void GmHD_dCamera_c_Start(void *)
ext 02514F88 void GmHD_dCamera_c_Set(void *, cXyz *, cXyz *, int, float)
ext 0251510C void GmHD_dCamera_c_Reset(void *, cXyz *, cXyz *)
ext 02515280 void GmHD_dCamera_c_SetTrimSize(void *, int)
ext 02515860 void GmHD_dCcD_Stts_dtor_Status(dCcD_Stts *, int)
ext 02515860 void GmHD_dCcD_Stts_dtor_Storage(void *, int)
ext 02515A70 void GmHD_dCcD_Cyl_dtor(dCcD_Cyl *, int)
ext 02515AE8 void GmHD_dCcD_Sph_dtor_Sphere(dCcD_Sph *, int)
ext 02515AE8 void GmHD_dCcD_Sph_dtor_Storage(void *, int)
ext 02515DA0 void GmHD_dCcD_GStts_dCcD_GStts(void *)
ext 02515E50 void GmHD_dCcD_GStts_Move(void *)
ext 02515F14 void GmHD_dCcD_Stts_Init(void *, int, int, gm_class *)
ext 02515FB8 void GmHD_dCcD_GObjInf_dCcD_GObjInf(void *)
ext 025162A4 int GmHD_dCcD_GObjInf_ChkTgHit(dCcD_Cyl *)
ext 02516300 void * GmHD_dCcD_GObjInf_GetTgHitObj(dCcD_Cyl *)
ext 02516518 void GmHD_dCcD_Cyl_Set(dCcD_Cyl *, void *)
ext 025166F0 void GmHD_dCcD_Sph_ctor_Sphere(dCcD_Sph *)
ext 025166F0 void GmHD_dCcD_Sph_ctor_Storage(void *)
ext 0251677C void GmHD_dCcD_Sph_Set(dCcD_Sph *, void *)
ext 02516C14 void GmHD_dCcMassS_Mng_Set(void *, dCcD_Cyl *, int)
ext 025192A8 void GmHD_cc_at_check(gm_class *, GmAttackInfo *)
ext 025200D4 void * GmHD_dComIfGp_get()
ext 02520460 int GmHD_dComIfG_resLoad(request_of_phase_process_class *, const char *)
ext 025204C8 void GmHD_dComIfG_resDelete(request_of_phase_process_class *, const char *)
ext 02520540 int GmHD_dComIfGp_getReverb(signed char)
ext 02555D0C void * GmHD_dKy_getEnvlight()
ext 025626A4 void GmHD_dScnKy_env_light_c_settingTevStruct(void *, int, cXyz *, dKy_tevstr_c *)
ext 02562F5C void GmHD_dScnKy_env_light_c_setLightTevColorType_MAJI(void *, void *, dKy_tevstr_c *)
ext 0259138C void GmHD_dMat_ice_c_entryDL(mDoExt_McaMorf *, int, void *)
ext 025A847C void GmHD_dPa_control_c_set_Effect(void *, int, unsigned int, cXyz *, csXyz *, cXyz *, int, int, int, int, int, int)
ext 025A847C void * GmHD_dPa_control_c_set_Emitter(void *, int, unsigned int, cXyz *, csXyz *, cXyz *, int, int, int, int, int, int)
ext 025B9E38 void GmHD_dSv_info_c_onSwitch(void *, unsigned char, signed char)
ext 025BA0C0 int GmHD_dSv_info_c_isSwitch(void *, unsigned char, signed char)
ext 025BA5D4 void GmHD_dSv_info_c_onActor(void *, unsigned short, signed char)
ext 025BEBB8 void GmHD_dSnap_RegistFig(int, gm_class *, cXyz *, short, float, float, float)
ext 025CB610 void GmHD_dVibration_c_StopQuake(void *, int)
ext 025D4ED0 void GmHD_fopAc_ac_c_fopAc_ac_c(gm_class *)
ext 025D50BC void GmHD_fopAc_ac_c_dtor_fopAc_ac_c(gm_class *, int)
ext 025D57E0 void GmHD_fopAcM_delete(gm_class *)
ext 025D5834 void GmHD_fopAcM_create(int, unsigned int, cXyz *, signed char, csXyz *, cXyz *, int, int)
ext 025D5A20 int GmHD_fopAcM_createChild(int, unsigned int, int, cXyz *, signed char, csXyz *, cXyz *, int, int)
ext 025D63E8 int GmHD_fopAcM_entrySolidHeap(gm_class *, int, int)
ext 025D674C void GmHD_fopAcM_setCullSizeBox(gm_class *, float, float, float, float, float, float)
ext 025D6800 void GmHD_fopAcM_posMove(gm_class *, void *)
ext 025D6894 int GmHD_fopAcM_searchActorAngleY_ActorWide(gm_class *, fopAc_ac_c *)
ext 025D6894 int GmHD_fopAcM_searchActorAngleY_StorageWide(gm_class *, void *)
ext 025D6894 short GmHD_fopAcM_searchActorAngleY_Angle(gm_class *, void *)
ext 025D68EC float GmHD_fopAcM_searchActorDistance(gm_class *, void *)
ext 025D7B24 void GmHD_fopAcM_orderPotentialEvent(gm_class *, int, int, int)
ext 025D99E8 void GmHD_fopAcM_createDisappear(gm_class *, cXyz *, int, int, unsigned char)
ext 025D9A70 void GmHD_fopAcM_getGroundAngle(gm_class *, csXyz *)
ext 025DA088 void GmHD_fopAcM_setGbaName(gm_class *, int, int, int)
ext 025E1918 void GmHD_mDoAud_subBgmStart(unsigned int)
ext 025E1928 void GmHD_mDoAud_subBgmStop()
ext 025E1960 void GmHD_mDoAud_bgmAllMute(int)
ext 025E1A40 void GmHD_mDoAud_seStart_SignedID(int, cXyz *, int, int)
ext 025E1A40 void GmHD_mDoAud_seStart_UnsignedID(unsigned int, cXyz *, int, int)
ext 025E1AA4 void GmHD_mDoAud_seStartLevel(unsigned int, cXyz *, unsigned int, int, int)
ext 025E4A98 void GmHD_mDoExt_McaMorf_setAnm(mDoExt_McaMorf *, void *, unsigned char, void *, float, float, float, float)
ext 025E4F64 void * GmHD_mDoExt_McaMorf_mDoExt_McaMorf(void *, void *, int, int, int, int, int, int, int, int, int, int, float)
ext 025E535C void GmHD_mDoExt_McaMorf_play(mDoExt_McaMorf *, cXyz *, int, int)
ext 025E54D8 void GmHD_mDoExt_McaMorf_updateDL(mDoExt_McaMorf *)
ext 025E5590 void GmHD_mDoExt_McaMorf_entryDL(mDoExt_McaMorf *)
ext 025E55A0 void GmHD_mDoExt_McaMorf_calc(mDoExt_McaMorf *)
ext 025E563C void GmHD_mDoExt_McaMorf_stopZelAnime(mDoExt_McaMorf *)
ext 025E742C void GmHD_mDoExt_baseAnm_play(void *)
ext 025E80D0 void * GmHD_mDoExt_brkAnm_init(void *)
ext 025E8154 int GmHD_mDoExt_brkAnm_init(void *, void *, void *, int, int, int, int, int, int, float)
ext 025E83FC void GmHD_mDoExt_brkAnm_entry(void *, void *, float)
ext 025E895C void GmHD_mDoExt_invisibleModel_ctor(void *)
ext 025E89F8 void GmHD_mDoExt_invisibleModel_dtor(void *, int)
ext 025E8A48 int GmHD_mDoExt_invisibleModel_create(void *, void *)
ext 025F1884 void GmHD_mDoMtx_YrotS(void *, short)
ext 026066C4 void * GmHD_dRes_control_c_getRes(void *, SafeString *, int)
ext 0273AD10 gm_class * GmHD_operator_new_Actor(int)
ext 0273AD10 void * GmHD_operator_new_Storage(int)
ext 0273AF40 void GmHD_operator_delete_Actor(gm_class *)
ext 0273AF40 void GmHD_operator_delete_Storage(void *)
ext 027DF9B0 unsigned int GmHD_JUTNameTab_getIndex(void *, const char *)
ext 027F2BF8 int GmHD_J3DFrameCtrl_checkPass(void *, float)
ext 027F3F94 void * GmHD_J3DModelData_getJointTree(void *)
ext 027F596C void GmHD_J3DModel_setMaterialPacketFlags(void *, unsigned int)
ext 027F7878 void * GmHD_J3DNode_toJoint(void *)
ext 028249B0 void GmHD_JPASetRMtxTVecfromMtx(void *, void *, void *)
ext 028E8D88 void GmHD_PSVECAdd(cXyz *, cXyz *, cXyz *)
ext 028E8DAC void GmHD_PSVECSubtract(cXyz *, cXyz *, cXyz *)
ext 028E90D4 void GmHD_PSMTXCopy(void *, void *)
ext 028E93CC void GmHD_PSMTXTrans(void *, float, float, float)
ext 028EAB2C void GmHD_csXyz_defaultConstruct(void *)
ext 028ED6F8 void GmHD_cXyz_defaultConstruct(void *)
ext 028F026C void GmHD___register_global_object(void *)
ext 028F4384 float GmHD_sqrtf(float)
*/

// clang-format on
