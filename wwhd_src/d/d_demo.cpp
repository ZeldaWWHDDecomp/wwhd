/**
 * Demo actors, camera and lighting adapters, object pool and cutscene manager.
 *
 * Written in the GameCube decompilation style and verified against the WWHD original
 * (tools/verify, d_demo).
 * HD: the manager uses static allocated SDK interfaces and adds cutscene, wind, palette
 * and audio overrides. GHS constructor/destructor entries keep their nullable receivers.
 */

#include "d/d_demo.h"
#include "d/actor/d_a_player.h"
#include "gc_api/d/d_com_inf_game.h"

static const f32 l_degreesToShort = 182.04444885253906;
static const f32 l_shortToDegrees = 0.0054931640625;
static const f32 l_animationMax = 3.402823466e38;
using namespace gabi;
static dDemo_virtualTable l_actorVtable;
static dDemo_virtualTable l_ambientVtable;
static dDemo_virtualTable l_lightVtable;
static dDemo_virtualTable l_fogVtable;
static dDemo_virtualTable l_cameraVtable;
static dDemo_destructorRecord l_prefixTransform1RangeDestructor;
static dDemo_destructorRecord l_prefixTransform1OriginDestructor;
static dDemo_destructorRecord l_prefixTransform1RotationDestructor;
static dDemo_destructorRecord l_prefixTransform2RangeDestructor;
static dDemo_destructorRecord l_prefixTransform2OriginDestructor;
static dDemo_destructorRecord l_prefixTransform2RotationDestructor;
static dDemo_destructorRecord l_prefixTransform3RangeDestructor;
static dDemo_destructorRecord l_prefixTransform3OriginDestructor;
static dDemo_destructorRecord l_prefixTransform3RotationDestructor;
static dDemo_destructorRecord l_demoTransformRangeDestructor;
static dDemo_destructorRecord l_demoTransformOriginDestructor;
static dDemo_destructorRecord l_demoTransformRotationDestructor;
static dDemo_destructorRecord l_tailTransformRangeDestructor;
static dDemo_destructorRecord l_tailTransformOriginDestructor;
static dDemo_destructorRecord l_tailTransformRotationDestructor;
static dDemo_archiveName l_demoArchive;
static dDemo_safeStringVtable l_safeStringVtable;
static dDemo_virtualTable l_systemVtable;
static dDemo_virtualTable l_stageCreatorVtable;
static dDemo_virtualTable l_audioCreatorVtable;
static dDemo_virtualTable l_particleCreatorVtable;
static dDemo_virtualTable l_messageCreatorVtable;
static dDemo_virtualTable l_factoryVtable;
static dDemo_virtualTable l_factoryCreateVtable;
static dDemo_manager_c l_demo;
static dDemo_staticTransform l_prefixTransform1;
static dDemo_staticTransform l_prefixTransform2;
static dDemo_staticTransform l_prefixTransform3;
static dDemo_staticTransform l_demoTransform;
static dDemo_staticTransform l_tailTransform;
static u32 l_messageControl;
static f32 l_demoWindPower;
static u32 l_audioControl;
static u32 l_audioInterface;
static u32 l_emitterManager;
static u32 l_resourceControl;
static const f32 l_primaryMinAngle = -3.1415927410125732;
static const f32 l_primaryMaxAngle = 3.1415927410125732;
static const f32 l_secondaryMinAngle = -3.1415927410125732;
static const f32 l_secondaryMaxAngle = 3.1415927410125732;
static const f32 l_tertiaryMinAngle = -3.1415927410125732;
static const f32 l_tertiaryMaxAngle = 3.1415927410125732;
static const f32 l_demoMinAngle = -3.1415927410125732;
static const f32 l_demoMaxAngle = 3.1415927410125732;
static const f32 l_finalMinAngle = -3.1415927410125732;
static const f32 l_finalMaxAngle = 3.1415927410125732;
static const f32 l_zero = 0.0;
static const f32 l_one = 1.0;
static const f32 l_defaultFovy = 60.0;
static const f32 l_reverse = -1.0;
static dDemo_doubleConstant l_secondPerFrame;
static const f32 l_calmWindPower = 0.0010000000474974513;
static const f32 l_demoWindTarget = 0.8999999761581421;
static const f32 l_demoWindStep = 0.10000000149011612;
/* 025274AC */
dDemo_actor_c* dDemo_actor_ct(dDemo_actor_c* self) {
    // HD: nullable GHS construction preserves the parameter-constructor call and store order.
    if (!self)
        self = static_cast<dDemo_actor_c*>(operator_new(sizeof(dDemo_actor_c)));
    if (self) {
        f32 zero = l_zero;
        self->mFlags = 0;
        self->mAnmFrameMax = zero;
        self->mVtable = gabi::ea(&l_actorVtable);
        self->mModel = nullptr;
        self->mAnmFrame = zero;
        self->mTexAnimation = 0;
        self->mTexAnimationFrame = zero;
        self->mShapeId = 0;
        self->mTexAnimationFrameMax = zero;
        self->mAnmTransition = zero;
        self->mAnmId = 0;
        dDemo_prm_ct(&self->mPrm);
        self->mFlags = 0;
        f32 one = l_one;
        self->mBrkId = 0;
        self->mScale.z = one;
        self->mScale.y = one;
        self->mScale.x = one;
        self->mActorId = 0;
        self->mTrans.x = zero;
        f32 maximum = l_animationMax;
        self->mTrans.z = zero;
        self->mBtpId = 0;
        self->mRotate.z = 0;
        self->mTexAnimationFrameMax = maximum;
        self->mOldAnmId = 0;
        self->mAnmFrameMax = maximum;
        self->mModel = nullptr;
        self->mRotate.y = 0;
        self->mBtkId = 0;
        self->mRotate.x = 0;
        self->mTrans.y = zero;
    }
    return self;
}

/* 025275EC */
void dDemo_actor_dt(dDemo_actor_c* self, u32 flags) {
    // HD: explicit nullable GHS deleting-destructor entry.
    if (self) {
        self->mVtable = gabi::ea(&l_actorVtable);
        fopAc_ac_c* actor = self->getActor();
        if (actor)
            actor->demoActorID = 0;
        self->mBtpId = fpcM_ERROR_PROCESS_ID_e;
        self->mBrkId = fpcM_ERROR_PROCESS_ID_e;
        self->mOldAnmId = fpcM_ERROR_PROCESS_ID_e;
        self->mActorId = fpcM_ERROR_PROCESS_ID_e;
        self->mBtkId = fpcM_ERROR_PROCESS_ID_e;
        dDemo_stageActor_dt(self, 0);
        if (flags & 1)
            operator_delete(self);
    }
}

/* 025275AC */
fopAc_ac_c* dDemo_actor_c::getActor() { return fopAcM_SearchByID(mActorId); }

/* 025277D8 */
void dDemo_actor_c::setActor(fopAc_ac_c* actor) {
    if (!actor) {
        mBrkId = fpcM_ERROR_PROCESS_ID_e;
        mActorId = fpcM_ERROR_PROCESS_ID_e;
        mOldAnmId = fpcM_ERROR_PROCESS_ID_e;
        mBtpId = fpcM_ERROR_PROCESS_ID_e;
        mBtkId = fpcM_ERROR_PROCESS_ID_e;
    } else {
        mActorId = actor->mBsPcId;
        mBrkId = fpcM_ERROR_PROCESS_ID_e;
        mOldAnmId = fpcM_ERROR_PROCESS_ID_e;
        mBtkId = fpcM_ERROR_PROCESS_ID_e;
        mBtpId = fpcM_ERROR_PROCESS_ID_e;
    }
}

/* 02527828 */
J3DAnmTexPattern* dDemo_actor_c::getP_BtpData(const char* archive) {
    u16 flags = mFlags;
    u32 id = 0;
    const char* name = nullptr;
    if (flags & ENABLE_TEX_ANM) {
        u32 old = mBtpId;
        id = mTexAnimation;
        if (id == old)
            return nullptr;
    } else {
        if (!(flags & ENABLE_UNK_e))
            return nullptr;
        u32 type = mPrm.mId;
        u8* data = mPrm.mData;
        name = archive;
        if (type == ID_UNK_1) {
            u32 old = mBtpId;
            id = u32(reinterpret_cast<dDemo_prmShortPrimary*>(data)->mBtpId);
            if (id == old)
                return nullptr;
        } else if (type == ID_UNK_2) {
            u32 old = mBtpId;
            id = u32(reinterpret_cast<dDemo_prmShortPair*>(data)->mBtpId);
            if (id == old)
                return nullptr;
        } else if (type == ID_UNK_4) {
            u32 old = mBtpId;
            id = reinterpret_cast<dDemo_prmWordPrimary*>(data)->mBtpId;
            if (id == old)
                return nullptr;
        } else if (type == ID_UNK_5 || type == ID_UNK_6) {
            u32 old = mBtpId;
            id = reinterpret_cast<dDemo_prmWordTriple*>(data)->mBtpId;
            if (id == old)
                return nullptr;
        } else
            return nullptr;
    }
    mBtpId = id;
    if (id & 0x10000)
        name = l_demoArchive.mName;
    else if (!name)
        return nullptr;
    dDemo_safeString request;
    request.mData = const_cast<char*>(name);
    request.mVtable = &l_safeStringVtable;
    J3DAnmTexPattern* result = static_cast<J3DAnmTexPattern*>(
        dDemo_getObjectIDRes(l_resourceControl, &request, id & 0xFFFF));
    if (result) {
        s32 maximum = dDemo_textureFrameMax(result);
        mTexAnimationFrameMax = f32(maximum);
    }
    return result;
}

/* 02527A98 */
J3DAnmTevRegKey* dDemo_actor_c::getP_BrkData(const char* archive) {
    if (!(mFlags & ENABLE_UNK_e))
        return nullptr;
    u32 type = mPrm.mId;
    u8* data = mPrm.mData;
    if (type != ID_UNK_6)
        return nullptr;
    u32 old = mBrkId, id = reinterpret_cast<dDemo_prmWordTriple*>(data)->mBrkId;
    if (id == old)
        return nullptr;
    mBrkId = id;
    if (id & 0x10000)
        archive = l_demoArchive.mName;
    dDemo_safeString request;
    request.mData = const_cast<char*>(archive);
    request.mVtable = &l_safeStringVtable;
    return static_cast<J3DAnmTevRegKey*>(
        dDemo_getObjectIDRes(l_resourceControl, &request, id & 0xFFFF));
}

/* 025279C8 */
J3DAnmTextureSRTKey* dDemo_actor_c::getP_BtkData(const char* archive) {
    if (!(mFlags & ENABLE_UNK_e))
        return nullptr;
    u32 type = mPrm.mId;
    u8* data = mPrm.mData;
    u32 id;
    if (type == ID_UNK_2) {
        u32 old = mBtkId;
        id = u32(reinterpret_cast<dDemo_prmShortPair*>(data)->mBtkId);
        if (id == old)
            return nullptr;
    } else if (type == ID_UNK_5 || type == ID_UNK_6) {
        u32 old = mBtkId;
        id = reinterpret_cast<dDemo_prmWordTriple*>(data)->mBtkId;
        if (id == old)
            return nullptr;
    } else
        return nullptr;
    mBtkId = id;
    if (id & 0x10000)
        archive = l_demoArchive.mName;
    dDemo_safeString request;
    request.mData = const_cast<char*>(archive);
    request.mVtable = &l_safeStringVtable;
    return static_cast<J3DAnmTextureSRTKey*>(
        dDemo_getObjectIDRes(l_resourceControl, &request, id & 0xFFFF));
}

/* 02526E7C */
f64 dDemo_actor_c::getPrm_Morf() {
    u16 flags = mFlags;
    if (flags & ENABLE_ANM_TRANSITION_e)
        return mAnmTransition;
    if (!(flags & ENABLE_UNK_e))
        return l_zero;
    u32 type = mPrm.mId;
    auto* data = reinterpret_cast<dDemo_prm_data*>(mPrm.mData.get());
    // HD: these parameter bytes are unsigned; the GameCube fields are signed.
    switch (type) {
    case ID_UNK_1:
        if (mPrm.field_0x54 >= 4)
            return f32(data->field_0x4);
        break;
    case ID_UNK_2:
        if (mPrm.field_0x54 >= 7)
            return f32(data->field_0x7);
        break;
    case ID_UNK_4:
        if (mPrm.field_0x54 >= 6)
            return f32(data->field_0x6);
        break;
    case ID_UNK_5:
        if (mPrm.field_0x54 >= 0xB)
            return f32(data->field_0xb);
        break;
    case ID_UNK_6:
        if (mPrm.field_0x54 >= 0xF)
            return f32(data->field_0xf);
        break;
    }
    return l_zero;
}

/* 02527028 */
u32 dDemo_setDemoData(fopAc_ac_c* actor, u32 mask, mDoExt_McaMorf* morph, const char* archive,
                      s32 count, dDemo_animationSound* table, u32 sound, u32 reverb) {
    // HD: flags and reverb keep their full register widths; animation calls use f64 arguments.
    u32 index = actor->demoActorID;
    if (!index || index > 32)
        return 0;
    dDemo_object_c* pool = l_demo.mDemoObj;
    if (!pool) {
        JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
        pool = l_demo.mDemoObj;
    }
    dDemo_actor_c* demo = pool->getActor(index);
    if (!demo)
        return 0;
    mask &= demo->mFlags;
    if (mask & dDemo_actor_c::ENABLE_TRANS_e) {
        u32 x = demoWords(&demo->mTrans)->x;
        demoWords(&actor->current.pos)->x = x;
        u32 y = demoWords(&demo->mTrans)->y;
        demoWords(&actor->current.pos)->y = y;
        u32 z = demoWords(&demo->mTrans)->z;
        demoWords(&actor->current.pos)->z = z;
        demoWords(&actor->old.pos)->x = x;
        demoWords(&actor->old.pos)->y = y;
        demoWords(&actor->old.pos)->z = z;
    }
    if (mask & dDemo_actor_c::ENABLE_ROTATE_e) {
        u16 x = demo->mRotate.x;
        actor->shape_angle.x = x;
        u16 y = demo->mRotate.y;
        actor->shape_angle.y = y;
        u16 z = demo->mRotate.z;
        actor->shape_angle.z = z;
        actor->current.angle.x = x;
        actor->current.angle.y = y;
        actor->current.angle.z = z;
    }
    if (mask & dDemo_actor_c::ENABLE_SCALE_e) {
        demoWords(&actor->scale)->x = demoWords(&demo->mScale)->x;
        demoWords(&actor->scale)->y = demoWords(&demo->mScale)->y;
        demoWords(&actor->scale)->z = demoWords(&demo->mScale)->z;
    }
    // HD: animation transition is also written to the actor's gravity field.
    if (mask & dDemo_actor_c::ENABLE_ANM_TRANSITION_e)
        actor->gravity = demo->getPrm_Morf();
    if (morph) {
        J3DModel* model = morph->mpModel;
        f32 one = l_one;
        demo->mModel = model;
        if (mask & dDemo_actor_c::ENABLE_ANM_e) {
            u32 id = demo->mAnmId, old = demo->mOldAnmId;
            if (id != old) {
                const char* name = (id & 0x10000) ? l_demoArchive.mName : archive;
                id &= 0xFFFF;
                demo->mOldAnmId = demo->mAnmId;
                dDemo_safeString request;
                request.mData = const_cast<char*>(name);
                request.mVtable = &l_safeStringVtable;
                auto* animation = static_cast<J3DAnmTransform*>(
                    dDemo_getObjectIDRes(l_resourceControl, &request, id));
                if (!animation)
                    JUT_ASSERT_fail("d_demo.cpp", 0x24C, "i_key != (0)");
                void* audio = nullptr;
                if (count > 0 && table) {
                    for (s32 i = 0; i < count; i++) {
                        if (table[i].mAnimationId == id) {
                            request.mData = const_cast<char*>(name);
                            request.mVtable = &l_safeStringVtable;
                            audio = dDemo_getObjectIDRes(l_resourceControl, &request,
                                                         table[i].mSoundId);
                            break;
                        }
                    }
                }
                f64 transition = demo->getPrm_Morf();
                dDemo_setMorphAnimation(morph, animation, -1, audio, transition, f64(one),
                                        f64(l_zero), f64(l_reverse));
                demo->mAnmFrameMax = f32(morph->mFrameCtrl.mEnd);
            }
        }
        if (mask & dDemo_actor_c::ENABLE_ANM_FRAME_e) {
            f32 frame = demo->mAnmFrame;
            if (frame > one) {
                morph->mFrameCtrl.mFrame = f32(s16(gabi::ftoi(gabi::fsubs_ppc(frame, one))));
                dDemo_playMorph(morph, &actor->current.pos, sound, reverb);
            } else {
                morph->mFrameCtrl.mFrame = f32(s16(gabi::ftoi(frame)));
            }
        } else {
            dDemo_playMorph(morph, &actor->current.pos, sound, reverb);
        }
    }
    return 1;
}

/* 0252767C */
void dDemo_actor_c::JSGSetData(u32 a, const void* b, u32 c) {
    u16 flags = mFlags;
    mPrm.mId = a;
    mPrm.mData = static_cast<u8*>(const_cast<void*>(b));
    mPrm.field_0x54 = c;
    mFlags = flags | ENABLE_UNK_e;
}

/* 02527698 */
void dDemo_actor_c::JSGSetTranslation(const cXyz* v) {
    u32 x = demoWords(v)->x;
    u16 flags = mFlags;
    demoWords(&mTrans)->x = x;
    u32 y = demoWords(v)->y;
    demoWords(&mTrans)->y = y;
    u32 z = demoWords(v)->z;
    mFlags = flags | ENABLE_TRANS_e;
    demoWords(&mTrans)->z = z;
}

/* 025276C0 */
void dDemo_actor_c::JSGSetScaling(const cXyz* v) {
    u32 x = demoWords(v)->x;
    u16 flags = mFlags;
    demoWords(&mScale)->x = x;
    u32 y = demoWords(v)->y;
    demoWords(&mScale)->y = y;
    u32 z = demoWords(v)->z;
    mFlags = flags | ENABLE_SCALE_e;
    demoWords(&mScale)->z = z;
}

/* 025276E8 */
void dDemo_actor_c::JSGSetRotation(const cXyz* input) {
    f32 x = input->x, scale = l_degreesToShort;
    mRotate.x = s16(gabi::ftoi(gabi::fmuls_ppc(x, scale)));
    mRotate.y = s16(gabi::ftoi(gabi::fmuls_ppc(input->y, scale)));
    s16 z = s16(gabi::ftoi(gabi::fmuls_ppc(input->z, scale)));
    u16 flags = mFlags;
    mRotate.z = z;
    mFlags = flags | ENABLE_ROTATE_e;
}

/* 02527754 */
void dDemo_actor_c::JSGSetShape(u32 value) {
    u16 flags = mFlags;
    mShapeId = value;
    mFlags = flags | ENABLE_SHAPE_e;
}

/* 02527768 */
void dDemo_actor_c::JSGSetAnimation(u32 animation) {
    u16 flags = mFlags;
    mAnmId = animation;
    f32 initial = l_animationMax;
    mFlags = flags | ENABLE_ANM_e;
    mAnmFrameMax = initial;
}

/* 02527788 */
void dDemo_actor_c::JSGSetAnimationFrame(f64 value) {
    u16 flags = mFlags;
    mAnmFrame = value;
    mFlags = flags | ENABLE_ANM_FRAME_e;
}

/* 0252779C */
void dDemo_actor_c::JSGSetAnimationTransition(f64 value) {
    u16 flags = mFlags;
    mAnmTransition = value;
    mFlags = flags | ENABLE_ANM_TRANSITION_e;
}

/* 025277B0 */
void dDemo_actor_c::JSGSetTextureAnimation(u32 value) {
    u16 flags = mFlags;
    mTexAnimation = value;
    mFlags = flags | ENABLE_TEX_ANM;
}

/* 025277C4 */
void dDemo_actor_c::JSGSetTextureAnimationFrame(f64 value) {
    u16 flags = mFlags;
    mTexAnimationFrame = value;
    mFlags = flags | ENABLE_TEX_ANM_FRAME_e;
}

/* 02527400 */
dDemo_cameraView* dDemo_getView() {
    // HD: retain three separate play-object accesses and the signed window camera index.
    auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    if (!game->play.mDlstWindowNum)
        return nullptr;
    game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    auto* window = reinterpret_cast<dDemo_windowView*>(game->play.mDlstWindow);
    s32 index = window->mCameraID;
    game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    auto* camera = gabi::at<dComIfG_camera_info_class>(
        gabi::ea(&game->play.mCameraInfo[0]) + u32(index) * sizeof(dComIfG_camera_info_class));
    return gabi::at<dDemo_cameraView>(camera->mpCamera);
}

/* 02527B94 */
f64 dDemo_camera_c::JSGGetProjectionNear() {
    dDemo_cameraView* view = dDemo_getCameraView(this);
    return view ? f32(view->mNear) : f32(l_zero);
}

/* 02527BD8 */
void dDemo_camera_c::JSGSetProjectionNear(f64 value) {
    u8 flags = mFlags;
    mProjNear = value;
    mFlags = flags | ENABLE_PROJ_NEAR_e;
}

/* 02527BEC */
f64 dDemo_camera_c::JSGGetProjectionFar() {
    dDemo_cameraView* view = dDemo_getCameraView(this);
    return view ? f32(view->mFar) : f32(l_one);
}

/* 02527C30 */
void dDemo_camera_c::JSGSetProjectionFar(f64 value) {
    u8 flags = mFlags;
    mProjFar = value;
    mFlags = flags | ENABLE_PROJ_FAR_e;
}

/* 02527C44 */
f64 dDemo_camera_c::JSGGetProjectionFovy() {
    dDemo_cameraView* view = dDemo_getCameraView(this);
    return view ? f32(view->mFovy) : f32(l_defaultFovy);
}

/* 02527C88 */
void dDemo_camera_c::JSGSetProjectionFovy(f64 value) {
    u8 flags = mFlags;
    mFovy = value;
    mFlags = flags | ENABLE_PROJ_FOVY_e;
}

/* 02527C9C */
void dDemo_camera_c::JSGGetViewPosition(cXyz* out) {
    dDemo_cameraView* view = dDemo_getCameraView(this);
    if (!view) {
        f32 zero = l_zero, z = l_zero;
        out->x = zero;
        out->z = z;
        out->y = zero;
    } else {
        demoWords(out)->x = demoWords(&view->mEye)->x;
        demoWords(out)->y = demoWords(&view->mEye)->y;
        demoWords(out)->z = demoWords(&view->mEye)->z;
    }
}

/* 02527D10 */
void dDemo_camera_c::JSGSetViewPosition(const cXyz* v) {
    u32 x = demoWords(v)->x;
    u8 flags = mFlags;
    demoWords(&mViewPosition)->x = x;
    u32 y = demoWords(v)->y;
    demoWords(&mViewPosition)->y = y;
    u32 z = demoWords(v)->z;
    mFlags = flags | ENABLE_VIEW_POS_e;
    demoWords(&mViewPosition)->z = z;
}

/* 02527D38 */
void dDemo_camera_c::JSGGetViewTargetPosition(cXyz* out) {
    dDemo_cameraView* view = dDemo_getCameraView(this);
    if (!view) {
        f32 zero = l_zero, z = l_one;
        out->x = zero;
        out->z = z;
        out->y = zero;
    } else {
        demoWords(out)->x = demoWords(&view->mTarget)->x;
        demoWords(out)->y = demoWords(&view->mTarget)->y;
        demoWords(out)->z = demoWords(&view->mTarget)->z;
    }
}

/* 02527DB4 */
void dDemo_camera_c::JSGSetViewTargetPosition(const cXyz* v) {
    u32 x = demoWords(v)->x;
    u8 flags = mFlags;
    demoWords(&mTargetPosition)->x = x;
    u32 y = demoWords(v)->y;
    demoWords(&mTargetPosition)->y = y;
    u32 z = demoWords(v)->z;
    mFlags = flags | ENABLE_VIEW_TARG_POS_e;
    demoWords(&mTargetPosition)->z = z;
}

/* 02527DDC */
f64 dDemo_camera_c::JSGGetViewRoll() {
    dDemo_cameraView* view = dDemo_getCameraView(this);
    return view ? gabi::fmuls_ppc(f32(view->mBank), l_shortToDegrees) : f32(l_zero);
}

/* 02527E50 */
void dDemo_camera_c::JSGSetViewRoll(f64 value) {
    u8 flags = mFlags;
    mRoll = value;
    mFlags = flags | ENABLE_VIEW_ROLL_e;
}

/* 02527EC4 */
void dDemo_ambient_c::JSGSetColor(const dDemo_colorWord* color) {
    u32 word = color->mColor;
    mColor = word;
    mFlags = (u8)mFlags | ENABLE_COLOR_e;
}

/* 02527F74 */
void dDemo_light_c::JSGSetLightType(u32 value) {
    u8 flags = mFlags;
    mLightType = value;
    mFlags = flags | ENABLE_LIGHT_TYPE_e;
}

/* 02527F88 */
void dDemo_light_c::JSGSetPosition(const cXyz* v) {
    u32 x = demoWords(v)->x;
    u8 flags = mFlags;
    demoWords(&mPosition)->x = x;
    u32 y = demoWords(v)->y;
    demoWords(&mPosition)->y = y;
    u32 z = demoWords(v)->z;
    mFlags = flags | ENABLE_POSITION_e;
    demoWords(&mPosition)->z = z;
}

/* 02527FB0 */
void dDemo_light_c::JSGSetColor(const dDemo_colorWord* color) {
    u32 word = color->mColor;
    mColor = word;
    mFlags = (u8)mFlags | ENABLE_COLOR_e;
}

/* 02527FE0 */
void dDemo_light_c::JSGSetDirection(const cXyz* v) {
    u32 x = demoWords(v)->x;
    u8 flags = mFlags;
    demoWords(&mDirection)->x = x;
    u32 y = demoWords(v)->y;
    demoWords(&mDirection)->y = y;
    u32 z = demoWords(v)->z;
    mFlags = flags | ENABLE_DIRECTION_e;
    demoWords(&mDirection)->z = z;
}

/* 0252807C */
void dDemo_fog_c::JSGSetStartZ(f64 value) {
    u8 flags = mFlags;
    mStartZ = value;
    mFlags = flags | ENABLE_START_Z_e;
}

/* 02528090 */
void dDemo_fog_c::JSGSetEndZ(f64 value) {
    u8 flags = mFlags;
    mEndZ = value;
    mFlags = flags | ENABLE_END_Z_e;
}

/* 025280A4 */
void dDemo_fog_c::JSGSetColor(const dDemo_colorWord* color) {
    u32 word = color->mColor;
    mColor = word;
    mFlags = (u8)mFlags | ENABLE_COLOR_e;
}

/* 025280D4 */
dDemo_object_c* dDemo_object_ct(dDemo_object_c* self) {
    // HD: the object pool is allocated separately from the manager.
    if (!self)
        self = static_cast<dDemo_object_c*>(operator_new(sizeof(dDemo_object_c)));
    if (self) {
        self->mpFog = nullptr;
        self->mNumActor = 0;
        self->mpAmbient = nullptr;
        self->mNumLight = 0;
        self->mpCamera = nullptr;
    }
    return self;
}

/* 02528258 */
void dDemo_object_dt(dDemo_object_c* self, u32 flags) {
    // HD: explicit nullable GHS deleting-destructor entry.
    if (self) {
        self->remove();
        if (flags & 1)
            operator_delete(self);
    }
}

/* 025282A8 */
dDemo_actor_c* dDemo_object_c::appendActor(fopAc_ac_c* actor) {
    u32 index = actor->demoActorID;
    if (index && index <= 32) {
        dDemo_object_c* pool = l_demo.mDemoObj;
        if (!pool) {
            JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
            pool = l_demo.mDemoObj;
        }
        dDemo_actor_c* existing = pool->getActor(index);
        if (existing)
            return existing;
    }
    if (mNumActor >= 32)
        return nullptr;
    dDemo_actor_c* object = dDemo_actor_ct(nullptr);
    if (!object)
        return nullptr;
    u32 count = mNumActor;
    mNumActor = count + 1;
    mpActors[count] = object;
    // HD: this caller consumes the setter's unchanged r3 receiver register.
    dDemo_actor_c* returned = dDemo_bindActor(object, actor);
    J3DModel* model = gabi::at<J3DModel>(actor->model);
    u32 current = mNumActor;
    actor->demoActorID = current;
    if (model)
        returned->mModel = model;
    return returned;
}

/* 02526E70 */
dDemo_actor_c* dDemo_object_c::getActor(u32 index) {
    // HD: preserve full-register index multiplication and 32-bit address wrap.
    return *gabi::at<gptr<dDemo_actor_c>>(gabi::ea(this) + index * 4);
}

/* 025283B8 */
dDemo_camera_c* dDemo_object_c::createCamera() {
    dDemo_camera_c* object = mpCamera;
    if (!object) {
        object = dDemo_camera_ct(nullptr);
        mpCamera = object;
    }
    return object;
}

/* 025283F8 */
dDemo_camera_c* dDemo_object_c::getActiveCamera() { return mpCamera; }

/* 02528400 */
dDemo_ambient_c* dDemo_object_c::createAmbient() {
    dDemo_ambient_c* object = mpAmbient;
    if (!object) {
        object = dDemo_ambient_ct(nullptr);
        mpAmbient = object;
    }
    return object;
}

/* 02528440 */
dDemo_light_c* dDemo_object_c::appendLight() {
    if (mNumLight >= 8)
        return nullptr;
    dDemo_light_c* object = dDemo_light_ct(nullptr);
    if (!object)
        return nullptr;
    u32 count = mNumLight;
    mNumLight = count + 1;
    mpLight[count] = object;
    return object;
}

/* 025284B8 */
dDemo_fog_c* dDemo_object_c::createFog() {
    dDemo_fog_c* object = mpFog;
    if (!object) {
        object = dDemo_fog_ct(nullptr);
        mpFog = object;
    }
    return object;
}

/* 02528120 */
void dDemo_object_c::remove() {
    u32 count = mNumActor;
    while (count) {
        count = (count - 1) & 0xFF;
        mNumActor = count;
        dDemo_actor_c* object = mpActors[count];
        if (object) {
            dDemo_deleteObject(object, 3);
            // HD: a nonnull object's destructor may change the pool count.
            count = mNumActor;
        }
    }
    dDemo_camera_c* camera = mpCamera;
    if (camera) {
        dDemo_deleteObject(camera, 3);
        mpCamera = nullptr;
    }
    dDemo_ambient_c* ambient = mpAmbient;
    if (ambient) {
        dDemo_deleteObject(ambient, 3);
        mpAmbient = nullptr;
    }
    count = mNumLight;
    while (count) {
        count = (count - 1) & 0xFF;
        mNumLight = count;
        dDemo_light_c* object = mpLight[count];
        if (object) {
            dDemo_deleteObject(object, 3);
            count = mNumLight;
        }
    }
    dDemo_fog_c* fog = mpFog;
    if (fog) {
        dDemo_deleteObject(fog, 3);
        mpFog = nullptr;
    }
}

/* 025284F8 */
void* dDemo_system_c::JSGFindObject(const char* name, u32 kind) {
    dDemo_object_c* pool = mObject;
    if (!pool)
        return nullptr;
    if (kind == OBJECT_ANY || kind == OBJECT_ACTOR) {
        fopAc_ac_c* actor = dDemo_searchActorByName(name, 0, 0);
        if (!actor) {
            if (kind != OBJECT_ACTOR)
                return nullptr;
            // HD: compare exactly five guest bytes, retaining early NUL handling.
            for (u32 i = 0; i < 5; i++) {
                u8 a = dDemo_nameByte(name, i), b = dDemo_nameByte("d_act", i);
                if (a != b)
                    return nullptr;
                if (!a)
                    break;
            }
            actor = dDemo_fastCreateActor(name, 0, 0, -1, 0, 0, 0, 0);
            if (!actor)
                return nullptr;
            fopAcM_setStageLayer(actor);
        }
        return mObject->appendActor(actor);
    }
    switch (kind) {
    case OBJECT_CAMERA:
        return pool->createCamera();
    case OBJECT_AMBIENT:
        return pool->createAmbient();
    case OBJECT_LIGHT:
        return pool->appendLight();
    case OBJECT_FOG:
        return pool->createFog();
    default:
        return nullptr;
    }
}

/* 025286E0 */
void dDemo_init() {
    // HD: the manager allocates its SDK interfaces individually instead of embedding them.
    auto* message = static_cast<dDemo_messageControl*>(operator_new(116));
    if (message)
        message = dDemo_messageControl_ct(message);
    l_messageControl = gabi::ea(message);
    if (!message) {
        JUT_ASSERT_fail("d_demo.cpp", 0x6CD, "m_mesgControl != (0)");
        message = gabi::at<dDemo_messageControl>(l_messageControl);
    }
    dDemo_setMessageControl(l_audioControl, message);
    auto* system = static_cast<dDemo_system_c*>(operator_new(sizeof(dDemo_system_c)));
    if (system) {
        system->mObject = nullptr;
        system->mVtable = gabi::ea(&l_systemVtable);
    }
    l_demo.mSystem = system;
    if (!system)
        JUT_ASSERT_fail("d_demo.cpp", 0x6D1, "m_system != (0)");
    dDemo_control* control = dDemo_control_ct(nullptr);
    l_demo.mControl = control;
    if (!control)
        JUT_ASSERT_fail("d_demo.cpp", 0x6D3, "m_control != (0)");
    void* owner = l_demo.mSystem;
    auto* creator = static_cast<dDemo_createObject*>(operator_new(16));
    if (creator) {
        creator->mPrev = 0;
        creator->mOwner = owner;
        creator->mNext = 0;
        creator->mVtable = gabi::ea(&l_stageCreatorVtable);
        if (!owner)
            JUT_ASSERT_fail("control.h", 0x2A, "pJSGSystem_!=(0)");
    }
    l_demo.mStage = creator;
    if (!creator)
        JUT_ASSERT_fail("d_demo.cpp", 0x6D5, "m_stage != (0)");
    owner = gabi::at<void>(l_audioInterface);
    creator = static_cast<dDemo_createObject*>(operator_new(16));
    if (creator) {
        creator->mPrev = 0;
        creator->mOwner = owner;
        creator->mNext = 0;
        creator->mVtable = gabi::ea(&l_audioCreatorVtable);
        if (!owner)
            JUT_ASSERT_fail("control.h", 0x2D, "pJAIBasic_!=(0)");
    }
    l_demo.mAudio = creator;
    if (!creator)
        JUT_ASSERT_fail("d_demo.cpp", 0x6D9, "m_audio != (0)");
    dDemo_system_c* systemOwner = l_demo.mSystem;
    owner = gabi::at<void>(l_emitterManager);
    creator = static_cast<dDemo_createObject*>(operator_new(sizeof(dDemo_createObject)));
    if (creator) {
        creator->mNext = 0;
        creator->mOwner = owner;
        creator->mVtable = gabi::ea(&l_particleCreatorVtable);
        creator->mPrev = 0;
        creator->mSystem = systemOwner;
        if (!owner)
            JUT_ASSERT_fail("control.h", 0x2E, "pJPAEmitterManager_!=(0)");
    }
    l_demo.mParticle = creator;
    if (!creator)
        JUT_ASSERT_fail("d_demo.cpp", 0x6DE, "m_particle != (0)");
    owner = gabi::at<void>(l_messageControl);
    creator = static_cast<dDemo_createObject*>(operator_new(16));
    if (creator) {
        creator->mPrev = 0;
        creator->mOwner = owner;
        creator->mNext = 0;
        creator->mVtable = gabi::ea(&l_messageCreatorVtable);
        if (!owner)
            JUT_ASSERT_fail("control.h", 0x4A, "pJMSControl_!=(0)");
    }
    l_demo.mMessage = creator;
    if (!creator)
        JUT_ASSERT_fail("d_demo.cpp", 0x6E1, "m_message != (0)");
    auto* factory = static_cast<dDemo_factory*>(operator_new(sizeof(dDemo_factory)));
    if (factory) {
        factory->mVtable = gabi::ea(&l_factoryVtable);
        // HD: retain the nullable embedded-list constructor even at a wrapping guest address.
        auto* list =
            gabi::at<dDemo_factoryList>(gabi::ea(factory) + offsetof(dDemo_factory, mCount));
        if (!list)
            list = static_cast<dDemo_factoryList*>(operator_new(sizeof(dDemo_factoryList)));
        if (list) {
            list->mCount = 0;
            void* head = &list->mNext;
            list->mPrev = head;
            list->mNext = head;
        }
        factory->mCreateObjectVtable = gabi::ea(&l_factoryCreateVtable);
    }
    l_demo.mFactory = factory;
    if (!factory)
        JUT_ASSERT_fail("d_demo.cpp", 0x6E4, "m_factory != (0)");
    dDemo_object_c* pool = dDemo_object_ct(nullptr);
    l_demo.mDemoObj = pool;
    if (!pool)
        JUT_ASSERT_fail("d_demo.cpp", 0x6E7, "m_object != (0)");
    f64 timestep = l_secondPerFrame.mValue;
    control = l_demo.mControl;
    control->mSecondPerFrame = timestep;
    factory = l_demo.mFactory;
    control = l_demo.mControl;
    control->mCreateObject = factory ? &factory->mCreateObjectVtable : nullptr;
    control->mFactory = factory;
    dDemo_appendCreateObject(l_demo.mFactory, l_demo.mStage);
    dDemo_appendCreateObject(l_demo.mFactory, l_demo.mAudio);
    dDemo_appendCreateObject(l_demo.mFactory, l_demo.mParticle);
    system = l_demo.mSystem;
    pool = l_demo.mDemoObj;
    system->mObject = pool;
    dDemo_appendCreateObject(l_demo.mFactory, l_demo.mMessage);
    l_demo.mCurFile = nullptr;
    l_demo.mMode = 0;
    // HD: the reset entry receives the last SDK call's unchanged r3 register.
    dDemo_resetFrames(gabi::at<void>(cpu->r[3]));
}

/* 02528B48 */
void dDemo_delete() {
    dDemo_remove();
    dDemo_object_c* pool = l_demo.mDemoObj;
    if (pool) {
        dDemo_object_dt(pool, 3);
        l_demo.mDemoObj = nullptr;
    }
    dDemo_factory* factory = l_demo.mFactory;
    if (factory) {
        dDemo_deleteObject(factory, 3);
        l_demo.mFactory = nullptr;
    }
    dDemo_createObject* creator = l_demo.mMessage;
    if (creator) {
        dDemo_deleteCreator(creator);
        l_demo.mMessage = nullptr;
    }
    creator = l_demo.mParticle;
    if (creator) {
        dDemo_deleteCreator(creator);
        l_demo.mParticle = nullptr;
    }
    creator = l_demo.mAudio;
    if (creator) {
        dDemo_deleteCreator(creator);
        l_demo.mAudio = nullptr;
    }
    creator = l_demo.mStage;
    if (creator) {
        dDemo_deleteCreator(creator);
        l_demo.mStage = nullptr;
    }
    auto* message = gabi::at<dDemo_messageControl>(l_messageControl);
    if (message) {
        dDemo_deleteMessageControl(message);
        l_messageControl = 0;
    }
    dDemo_control* control = l_demo.mControl;
    if (control) {
        dDemo_deleteControl(control);
        l_demo.mControl = nullptr;
    }
    dDemo_system_c* system = l_demo.mSystem;
    if (system) {
        dDemo_deleteObject(system, 3);
        l_demo.mSystem = nullptr;
    }
}

/* inlined into dDemo_create, dDemo_update */
static int compare_names(const char* expected) {
    dDemo_safeString left;
    dDemo_safeString right;
    left.mData = gabi::at<char>(gabi::guest_addr(expected));
    left.mVtable = &l_safeStringVtable;
    right.mData = l_demoArchive.mName;
    right.mVtable = &l_safeStringVtable;
    dDemo_assureTermination(&left);
    dDemo_assureSafeString(&left);
    const char* a = left.mData;
    dDemo_assureSafeString(&right);
    const char* b = right.mData;
    if (a == b)
        return 1;
    // HD: the SafeString comparison has a bounded guest-byte scan.
    for (u32 i = 0; i < 0x40001; i++) {
        u8 x = dDemo_nameByte(a, i), y = dDemo_nameByte(b, i);
        if (x != y)
            return 0;
        if (!x)
            return 1;
    }
    return -1;
}

/* 02528CEC */
u32 dDemo_create(const u8* data, const cXyz* position, f64 rotation) {
    if (!l_demo.mSystem.v)
        JUT_ASSERT_fail("d_demo.cpp", 0x747, "m_system != (0)");
    dDemo_resetControlObjects(l_demo.mControl);
    dDemo_parse parser;
    dDemo_parseInput input;
    dDemo_parse_ct(&parser, l_demo.mControl);
    input.mData = const_cast<u8*>(data);
    u32 ok = dDemo_parseNext(&parser, &input, 0);
    if (!ok) {
        dDemo_parse_dt(&parser, 2);
        return 0;
    }
    int first = compare_names("Demo04");
    dDemo_control* control;
    if (first == 1) {
        dComIfGs_onEventBit(dDemo_DEMO_EVENT_e);
        control = l_demo.mControl;
    } else {
        int second = compare_names("Demo46");
        if (second == 1) {
            dDemo_forward(l_demo.mControl, 2000);
            dDemo_startEventWind();
            dDemo_setEventWind(0, 0xFFFFC000);
            dDemo_setWindPower(f64(l_calmWindPower));
            control = l_demo.mControl;
        } else {
            dDemo_forward(l_demo.mControl, 0);
            control = l_demo.mControl;
        }
    }
    if (position) {
        control->mTransformOriginEnabled = 1;
        control->mTransformEnabled = 1;
        control = l_demo.mControl;
        dDemo_setGetOrigin(control, position, rotation);
        dDemo_setSetOrigin(control, position, rotation);
    } else {
        control->mTransformEnabled = 0;
        control->mTransformOriginEnabled = 0;
    }
    l_demo.mMode = 1;
    // HD: retain the last SDK call's r3 receiver at the reset entry.
    dDemo_resetFrames(gabi::at<void>(cpu->r[3]));
    l_demo.mCurFile = data;
    int third = compare_names("Demo37");
    if (third == 1)
        dDemo_pauseStream(dDemo_EVENT_STREAM_e);
    else {
        int fourth = compare_names("Demo46");
        if (fourth == 1)
            dDemo_pauseStream(dDemo_EVENT_STREAM_e);
        else if (compare_names("Demo45") == 1)
            dDemo_startStream(dDemo_DEFAULT_STREAM_e);
    }
    dDemo_parse_dt(&parser, 2);
    return 1;
}

/* 02528AC4 */
void dDemo_remove() {
    if (!l_demo.mSystem.v)
        JUT_ASSERT_fail("d_demo.cpp", 0x7A3, "m_system != (0)");
    dDemo_control* control = l_demo.mControl;
    dDemo_destroyAll(control);
    dDemo_destroyCreateObjects(&control->mCreateObject);
    l_demo.mDemoObj->remove();
    l_demo.mMode = 0;
    l_demo.mCurFile = nullptr;
}

/* 025291C8 */
u32 dDemo_update() {
    if (!l_demo.mSystem.v)
        JUT_ASSERT_fail("d_demo.cpp", 0x7E8, "m_system != (0)");
    if (!l_demo.mCurFile.v)
        return 0;
    bool advanced = false;
    int first = compare_names("Demo46");
    f32 zero = l_zero;
    if (first == 1) {
        u32 frame = l_demo.mFrame;
        if (frame - 878 < 56 || frame - 959 < 57) {
            if (frame != 1015) {
                l_demo.mFrame = frame + 1;
                return 1;
            }
            dDemo_forward(l_demo.mControl, 55);
            frame = l_demo.mFrame;
        } else if (frame == 1775) {
            dDemo_forward(l_demo.mControl, 20);
            frame = l_demo.mFrame;
        }
        if (frame >= 948) {
            if (frame == 948)
                l_demoWindPower = zero;
            f64 target = l_demoWindTarget, step = l_demoWindStep;
            dDemo_chaseWindPower(&l_demoWindPower, target, step);
            dDemo_setWindPower(f64(l_demoWindPower));
        }
    } else if (compare_names("Demo08") == 1) {
        u32 frame = l_demo.mFrameNoMsg;
        if (frame >= 1357) {
            bool bright = frame <= 1386;
            dDemo_setWindPower(f64(bright ? f32(l_calmWindPower) : zero));
            auto* env = reinterpret_cast<dDemo_environment*>(dKy_getEnvlight());
            env->mPaletteOverride = bright ? 0 : dDemo_DEFAULT_PALETTE_e;
        }
    } else if (compare_names("Demo29") == 1) {
        u32 frame = l_demo.mFrameNoMsg;
        if (frame - 597 < 67) {
            auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            frame = l_demo.mFrameNoMsg;
            auto* player = static_cast<daPy_py_c*>(game->play.mpPlayerPtr[0].get());
            u32 flags = player->mNoResetFlg0;
            player->mNoResetFlg0 = frame == 663 ? (flags & ~daPy_py_c::daPyFlg0_NO_DRAW)
                                                : (flags | daPy_py_c::daPyFlg0_NO_DRAW);
        }
    } else if (compare_names("Demo45") == 1) {
        if (l_demo.mFrameNoMsg == 800)
            dDemo_pauseStream(dDemo_DEFAULT_STREAM_e);
    }
    advanced = dDemo_forward(l_demo.mControl, 1) != 0;
    if (!advanced)
        l_demo.mMode = 2;
    else {
        u32 frame = l_demo.mFrame;
        dDemo_control* control = l_demo.mControl;
        l_demo.mFrame = frame + 1;
        if (control->mSuspend <= 0)
            l_demo.mFrameNoMsg = l_demo.mFrameNoMsg + 1;
    }
    return 1;
}

// HD: compiler-generated and template code of this translation unit.

/* 02526CB4 */
void dDemo_initPrimaryTransformRange() {
    // HD: these SDK transform-range objects and destructor records are static.
    l_prefixTransform1.mRotationNode = 0;
    l_prefixTransform1.mOriginNode = 0;
    l_prefixTransform1.mRotationLink = 0;
    l_prefixTransform1.mOriginLink = 0;
    dDemo_registerGlobalDestructor(&l_prefixTransform1RangeDestructor);
    f32 minimum = l_primaryMinAngle, maximum = l_primaryMaxAngle;
    l_prefixTransform1.mOriginX = minimum;
    l_prefixTransform1.mOriginY = maximum;
    dDemo_initOriginFlag(&l_prefixTransform1.mOriginEnabled);
    dDemo_registerGlobalDestructor(&l_prefixTransform1OriginDestructor);
    dDemo_initRotationFlag(&l_prefixTransform1.mRotationEnabled);
    dDemo_registerGlobalDestructor(&l_prefixTransform1RotationDestructor);
}

/* 02526D48 */
void dDemo_initSecondaryTransformRange() {
    // HD: these SDK transform-range objects and destructor records are static.
    l_prefixTransform2.mRotationNode = 0;
    l_prefixTransform2.mOriginNode = 0;
    l_prefixTransform2.mRotationLink = 0;
    l_prefixTransform2.mOriginLink = 0;
    dDemo_registerGlobalDestructor(&l_prefixTransform2RangeDestructor);
    f32 minimum = l_secondaryMinAngle, maximum = l_secondaryMaxAngle;
    l_prefixTransform2.mOriginX = minimum;
    l_prefixTransform2.mOriginY = maximum;
    dDemo_initOriginFlag(&l_prefixTransform2.mOriginEnabled);
    dDemo_registerGlobalDestructor(&l_prefixTransform2OriginDestructor);
    dDemo_initRotationFlag(&l_prefixTransform2.mRotationEnabled);
    dDemo_registerGlobalDestructor(&l_prefixTransform2RotationDestructor);
}

/* 02526DDC */
void dDemo_initTertiaryTransformRange() {
    // HD: these SDK transform-range objects and destructor records are static.
    l_prefixTransform3.mRotationNode = 0;
    l_prefixTransform3.mOriginNode = 0;
    l_prefixTransform3.mRotationLink = 0;
    l_prefixTransform3.mOriginLink = 0;
    dDemo_registerGlobalDestructor(&l_prefixTransform3RangeDestructor);
    f32 minimum = l_tertiaryMinAngle, maximum = l_tertiaryMaxAngle;
    l_prefixTransform3.mOriginX = minimum;
    l_prefixTransform3.mOriginY = maximum;
    dDemo_initOriginFlag(&l_prefixTransform3.mOriginEnabled);
    dDemo_registerGlobalDestructor(&l_prefixTransform3OriginDestructor);
    dDemo_initRotationFlag(&l_prefixTransform3.mRotationEnabled);
    dDemo_registerGlobalDestructor(&l_prefixTransform3RotationDestructor);
}

/* 02527468 */
dDemo_prm_c* dDemo_prm_ct(dDemo_prm_c* self) {
    // HD: the GHS constructor allocates when its incoming receiver is null.
    if (!self)
        self = static_cast<dDemo_prm_c*>(operator_new(sizeof(dDemo_prm_c)));
    if (self) {
        self->mId = 0;
        self->field_0x54 = 0;
        self->mData = nullptr;
    }
    return self;
}

/* 02527B30 */
dDemo_camera_c* dDemo_camera_ct(dDemo_camera_c* self) {
    // HD: nullable constructor receiver and initialization order are retained.
    if (!self)
        self = static_cast<dDemo_camera_c*>(operator_new(sizeof(dDemo_camera_c)));
    if (self) {
        f32 zero = l_zero;
        self->mFlags = 0;
        self->mProjFar = zero;
        self->mVtable = gabi::ea(&l_cameraVtable);
        self->mRoll = zero;
        self->mAspect = zero;
        self->mFovy = zero;
        self->mProjNear = zero;
    }
    return self;
}

/* 02527E64 */
dDemo_ambient_c* dDemo_ambient_ct(dDemo_ambient_c* self) {
    // HD: nullable GHS construction; the colour starts at byte 5.
    if (!self)
        self = static_cast<dDemo_ambient_c*>(operator_new(sizeof(dDemo_ambient_c)));
    if (self) {
        self->mFlags = 0;
        self->mVtable = gabi::ea(&l_ambientVtable);
        dDemo_zeroColor(&self->mColor, sizeof(self->mColor));
    }
    return self;
}

/* 02527EF4 */
dDemo_light_c* dDemo_light_ct(dDemo_light_c* self) {
    // HD: nullable GHS construction and observed member initialization order.
    if (!self)
        self = static_cast<dDemo_light_c*>(operator_new(sizeof(dDemo_light_c)));
    if (self) {
        self->mFlags = 0;
        self->mVtable = gabi::ea(&l_lightVtable);
        self->mAttnFn = 0;
        self->mLightType = 0;
        self->mSpotFn = 0;
        dDemo_zeroColor(&self->mColor, sizeof(self->mColor));
        f32 zero = l_zero;
        self->mDistAttn0 = zero;
        self->mDistAttn1 = zero;
        self->mAngleAttn = zero;
    }
    return self;
}

/* 02528008 */
dDemo_fog_c* dDemo_fog_ct(dDemo_fog_c* self) {
    // HD: nullable GHS construction and packed fog-function byte.
    if (!self)
        self = static_cast<dDemo_fog_c*>(operator_new(sizeof(dDemo_fog_c)));
    if (self) {
        f32 zero = l_zero;
        self->mFogType = 0;
        self->mEndZ = zero;
        self->mVtable = gabi::ea(&l_fogVtable);
        self->mFlags = 0;
        self->mStartZ = zero;
        dDemo_zeroColor(&self->mColor, sizeof(self->mColor));
    }
    return self;
}

/* 025286C0 */
void dDemo_resetFrames(void* unused) {
    // HD: this entry resets static manager counters; it is not a pool constructor.
    l_demo.mFrame = 0;
    l_demo.mFrameAux = 0;
    l_demo.mFrameNoMsg = 0;
}

/* 025296F8 */
void dDemo_initTransformRange() {
    // HD: these SDK transform-range objects and destructor records are static.
    l_demoTransform.mRotationNode = 0;
    l_demoTransform.mOriginNode = 0;
    l_demoTransform.mRotationLink = 0;
    l_demoTransform.mOriginLink = 0;
    dDemo_registerGlobalDestructor(&l_demoTransformRangeDestructor);
    f32 minimum = l_demoMinAngle, maximum = l_demoMaxAngle;
    l_demoTransform.mOriginX = minimum;
    l_demoTransform.mOriginY = maximum;
    dDemo_initOriginFlag(&l_demoTransform.mOriginEnabled);
    dDemo_registerGlobalDestructor(&l_demoTransformOriginDestructor);
    dDemo_initRotationFlag(&l_demoTransform.mRotationEnabled);
    dDemo_registerGlobalDestructor(&l_demoTransformRotationDestructor);
}

/* 0252978C */
void dDemo_deleteFactoryBase(void* self, u32 flags) {
    // HD: this compiler-generated deleting destructor also passes flags to __dl.
    if (self && (flags & 1))
        dDemo_deleteWithFlags(self, flags);
}

/* 025297A0 */
u32 dDemo_actor_c::JSGFindNodeID(const char* name) {
    J3DModel* model = mModel;
    if (!model) {
        JUT_ASSERT_fail("d_demo.h", 0x7C, "mModel != (0)");
        model = mModel;
    }
    return u32(model->getModelData()->getJointName()->getIndex(name));
}

/* 02529828 */
u32 dDemo_actor_c::JSGGetNodeTransformation(u32 index, Mtx34* out) {
    J3DModel* model = mModel;
    if (!model) {
        JUT_ASSERT_fail("d_demo.h", 0x85, "mModel != (0)");
        model = mModel;
    }
    // HD: the full index register is masked to u16 before selecting the joint matrix.
    J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
    Mtx34* jointMatrix = gabi::at<Mtx34>(buffer->mpAnmMtx + (index & 0xFFFF) * sizeof(Mtx34));
    u16 flags = buffer->mFlags;
    buffer->mFlags = flags | 0x10;
    PSMTXCopy(jointMatrix, out);
    return 1;
}

/* 025298BC */
f64 dDemo_actor_c::JSGGetAnimationFrameMax() { return mAnmFrameMax; }

/* 025298C4 */
f64 dDemo_actor_c::JSGGetTextureAnimationFrameMax() { return mTexAnimationFrameMax; }

/* 025298CC */
void dDemo_actor_c::JSGGetTranslation(cXyz* out) {
    demoWords(out)->x = demoWords(&mTrans)->x;
    demoWords(out)->y = demoWords(&mTrans)->y;
    demoWords(out)->z = demoWords(&mTrans)->z;
}

/* 025298E8 */
void dDemo_actor_c::JSGGetScaling(cXyz* out) {
    demoWords(out)->x = demoWords(&mScale)->x;
    demoWords(out)->y = demoWords(&mScale)->y;
    demoWords(out)->z = demoWords(&mScale)->z;
}

/* 02529904 */
void dDemo_actor_c::JSGGetRotation(cXyz* out) {
    f32 x = f32(mRotate.x), scale = l_shortToDegrees;
    out->x = gabi::fmuls_ppc(x, scale);
    out->y = gabi::fmuls_ppc(f32(mRotate.y), scale);
    out->z = gabi::fmuls_ppc(f32(mRotate.z), scale);
}

/* 02529988 */
void dDemo_camera_dt(dDemo_camera_c* self, u32 flags) {
    // HD: explicit nullable GHS deleting-destructor entry.
    if (self) {
        dDemo_stageCamera_dt(self, 0);
        if (flags & 1)
            operator_delete(self);
    }
}

/* 025299DC */
void dDemo_ambient_dt(dDemo_ambient_c* self, u32 flags) {
    // HD: explicit nullable GHS deleting-destructor entry.
    if (self) {
        dDemo_stageAmbient_dt(self, 0);
        if (flags & 1)
            operator_delete(self);
    }
}

/* 02529A30 */
void dDemo_light_dt(dDemo_light_c* self, u32 flags) {
    // HD: explicit nullable GHS deleting-destructor entry.
    if (self) {
        dDemo_stageLight_dt(self, 0);
        if (flags & 1)
            operator_delete(self);
    }
}

/* 02529A84 */
void dDemo_fog_dt(dDemo_fog_c* self, u32 flags) {
    // HD: explicit nullable GHS deleting-destructor entry.
    if (self) {
        dDemo_stageFog_dt(self, 0);
        if (flags & 1)
            operator_delete(self);
    }
}

/* 02529AD8 */
void dDemo_system_dt(dDemo_system_c* self, u32 flags) {
    // HD: explicit nullable GHS deleting-destructor entry.
    if (self) {
        dDemo_stageSystem_dt(self, 0);
        if (flags & 1)
            operator_delete(self);
    }
}

/* 02529B2C */
void dDemo_assureTermination(void* self) {}

/* 02529B30 */
void dDemo_initFinalTransformRange() {
    // HD: these SDK transform-range objects and destructor records are static.
    l_tailTransform.mRotationNode = 0;
    l_tailTransform.mOriginNode = 0;
    l_tailTransform.mRotationLink = 0;
    l_tailTransform.mOriginLink = 0;
    dDemo_registerGlobalDestructor(&l_tailTransformRangeDestructor);
    f32 minimum = l_finalMinAngle, maximum = l_finalMaxAngle;
    l_tailTransform.mOriginX = minimum;
    l_tailTransform.mOriginY = maximum;
    dDemo_initOriginFlag(&l_tailTransform.mOriginEnabled);
    dDemo_registerGlobalDestructor(&l_tailTransformOriginDestructor);
    dDemo_initRotationFlag(&l_tailTransform.mRotationEnabled);
    dDemo_registerGlobalDestructor(&l_tailTransformRotationDestructor);
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data l_actorVtable 1004C044
data l_ambientVtable 1004C264
data l_lightVtable 1004C2DC
data l_fogVtable 1004C3A4
data l_cameraVtable 1004C14C
ext 02842288 void dDemo_stageActor_dt(void* self, u32 flags)
ext 0273AF40 void dDemo_deleteWithFlags(void* self, u32 flags)
ext 02842734 void dDemo_stageSystem_dt(void* self, u32 flags)
ext 028424A0 void dDemo_stageFog_dt(void* self, u32 flags)
ext 028425A4 void dDemo_stageLight_dt(void* self, u32 flags)
ext 02842330 void dDemo_stageAmbient_dt(void* self, u32 flags)
ext 0284241C void dDemo_stageCamera_dt(void* self, u32 flags)
ext 02527400 dDemo_cameraView* dDemo_getCameraView(dDemo_camera_c* caller)
ext 028F521C void dDemo_zeroColor(void* color, u32 size)
data l_prefixTransform1RangeDestructor 101D5F50
data l_prefixTransform1OriginDestructor 101D5F5C
data l_prefixTransform1RotationDestructor 101D5F68
data l_prefixTransform2RangeDestructor 101D5F74
data l_prefixTransform2OriginDestructor 101D5F80
data l_prefixTransform2RotationDestructor 101D5F8C
data l_prefixTransform3RangeDestructor 101D5F98
data l_prefixTransform3OriginDestructor 101D5FA4
data l_prefixTransform3RotationDestructor 101D5FB0
data l_demoTransformRangeDestructor 101D5FC0
data l_demoTransformOriginDestructor 101D5FCC
data l_demoTransformRotationDestructor 101D5FD8
data l_tailTransformRangeDestructor 101D6018
data l_tailTransformOriginDestructor 101D6024
data l_tailTransformRotationDestructor 101D6030
data l_demoArchive 1047E6B8
data l_safeStringVtable 1004BDC8
str 1004BFA4 "d_demo.cpp" in 02528AC4
str 1004BF94 "m_system != (0)" in 02528AC4
str 1004BE1C "d_demo.h" in 025297A0
str 1004BDF0 "mModel != (0)" in 025297A0
str 1004BE28 "d_demo.h" in 02529828
str 1004BE00 "mModel != (0)" in 02529828
str 1004BE10 "d_demo.h" in 025282A8
str 1004BDE0 "m_object != (0)" in 025282A8
str 1004BE10 "d_demo.h" in 02527028
str 1004BDE0 "m_object != (0)" in 02527028
str 1004BE3C "d_demo.cpp" in 02527028
str 1004BE48 "i_key != (0)" in 02527028
str 1004BEE4 "d_demo.cpp" in 025286E0
str 1004BEF0 "m_mesgControl != (0)" in 025286E0
str 1004BE80 "m_system != (0)" in 025286E0
str 1004BF08 "m_control != (0)" in 025286E0
str 1004BEB0 "control.h" in 025286E0
str 1004BF1C "pJSGSystem_!=(0)" in 025286E0
str 1004BF30 "m_stage != (0)" in 025286E0
str 1004BE90 "pJAIBasic_!=(0)" in 025286E0
str 1004BF40 "m_audio != (0)" in 025286E0
str 1004BF50 "pJPAEmitterManager_!=(0)" in 025286E0
str 1004BEBC "m_particle != (0)" in 025286E0
str 1004BED0 "pJMSControl_!=(0)" in 025286E0
str 1004BF6C "m_message != (0)" in 025286E0
str 1004BF80 "m_factory != (0)" in 025286E0
str 1004BEA0 "m_object != (0)" in 025286E0
str 1004BFE4 "d_demo.cpp" in 02528CEC
str 1004BFD4 "m_system != (0)" in 02528CEC
str 1004C02C "d_demo.cpp" in 025291C8
str 1004C01C "m_system != (0)" in 025291C8
str 1004BE6C "d_act"
data l_systemVtable 1004C44C
data l_stageCreatorVtable 101735C4
data l_audioCreatorVtable 1017329C
data l_particleCreatorVtable 10173424
data l_messageCreatorVtable 101733AC
data l_factoryVtable 10172CD4
data l_factoryCreateVtable 10172C74
str 1004BFB4 "Demo04" in 02528CEC
str 1004BFBC "Demo46" in 02528CEC
str 1004BFC4 "Demo37" in 02528CEC
str 1004BFCC "Demo45" in 02528CEC
str 1004BFFC "Demo46" in 025291C8
str 1004C004 "Demo08" in 025291C8
str 1004C00C "Demo29" in 025291C8
str 1004C014 "Demo45" in 025291C8
data l_demo 101D5FE4
ext 0283D4C8 void dDemo_resetControlObjects(dDemo_control* control)
ext 0283980C void dDemo_parse_ct(dDemo_parse* parser, dDemo_control* control)
ext 027ED8C0 u32 dDemo_parseNext(dDemo_parse* parser, dDemo_parseInput* input, u32 flags)
ext 0283987C void dDemo_parse_dt(dDemo_parse* parser, u32 flags)
ext 0283D514 u32 dDemo_forward(dDemo_control* control, u32 frames)
ext 02839518 void dDemo_setGetOrigin(dDemo_control* control, const cXyz* position, f64 rotation)
ext 028394F4 u32 dDemo_setSetOrigin(dDemo_control* control, const cXyz* position, f64 rotation)
ext 0257E6C4 void dDemo_startEventWind()
ext 0257E684 void dDemo_setEventWind(u32 x, u32 y)
ext 0257E594 void dDemo_setWindPower(f64 power)
ext 0200F5C8 void dDemo_chaseWindPower(be<f32>* power, f64 target, f64 step)
ext 02030FA8 void dDemo_pauseStream(u32 stream)
ext 02030E90 void dDemo_startStream(u32 stream)
ext 025E4A98 void dDemo_setMorphAnimation(mDoExt_McaMorf* morph, J3DAnmTransform* animation, s32 mode, void* sound, f64 transition, f64 speed, f64 start, f64 end)
ext 025E535C void dDemo_playMorph(mDoExt_McaMorf* morph, cXyz* position, u32 sound, u32 reverb)
ext 025F7424 dDemo_messageControl* dDemo_messageControl_ct(dDemo_messageControl* self)
ext 025F8610 void dDemo_setMessageControl(u32 audioControl, dDemo_messageControl* message)
ext 028395A8 dDemo_control* dDemo_control_ct(dDemo_control* self)
ext 0283973C void dDemo_appendCreateObject(dDemo_factory* factory, dDemo_createObject* creator)
ext 025277D8 dDemo_actor_c* dDemo_bindActor(dDemo_actor_c* self, fopAc_ac_c* actor)
ext 025D9F38 fopAc_ac_c* dDemo_searchActorByName(const char* name, u32 first, u32 second)
ext 025D59A8 fopAc_ac_c* dDemo_fastCreateActor(const char* name, u32 first, u32 second, s32 room, u32 fourth, u32 fifth, u32 sixth, u32 seventh)
ext 0283D3E8 void dDemo_destroyAll(dDemo_control* control)
ext 02838E6C void dDemo_destroyCreateObjects(void* objects)
ext 026067F4 void* dDemo_getObjectIDRes(u32 control, dDemo_safeString* archive, u32 id)
ext 028F026C void dDemo_registerGlobalDestructor(dDemo_destructorRecord* record)
ext 028ED6F8 void dDemo_initOriginFlag(be<u8>* flag)
ext 028EAB2C void dDemo_initRotationFlag(be<u8>* flag)
data l_prefixTransform1 10475658
data l_prefixTransform2 10475674
data l_prefixTransform3 10475690
data l_demoTransform 104756AC
data l_tailTransform 104756CC
data l_messageControl 104756C8
data l_demoWindPower 101D5FBC
data l_audioControl 101F4B5C
data l_audioInterface 101FFC78
data l_emitterManager 1047B2D4
data l_resourceControl 101F4F28
data l_primaryMinAngle 1004BDA0
data l_primaryMaxAngle 1004BDA4
data l_secondaryMinAngle 1004BDA8
data l_secondaryMaxAngle 1004BDAC
data l_tertiaryMinAngle 1004BDB0
data l_tertiaryMaxAngle 1004BDB4
data l_demoMinAngle 1004C038
data l_demoMaxAngle 1004C03C
data l_finalMinAngle 1004C4E0
data l_finalMaxAngle 1004C4E4
data l_zero 1004BDC0
data l_one 1004BE34
data l_defaultFovy 1004BE60
data l_reverse 1004BE38
data l_secondPerFrame 1004BE78
data l_calmWindPower 1004BFB0
data l_demoWindTarget 1004BFF4
data l_demoWindStep 1004BFF8
data l_degreesToShort 1004BE5C
data l_shortToDegrees 1004BE68
data l_animationMax 1004BE58
*/
// clang-format on
