/* Demo actor/camera/light layouts.
 * HD: the manager uses static pointers instead of the GameCube embedded object.
 * HD: ambient colour is packed at byte 5, and parameter data has a third size word.
 */
#pragma once
#include "bindings.h"

struct dDemo_colorWord {
    be<u32> mColor;
};
WWHD_SIZE(dDemo_colorWord, 4);

struct dDemo_prm_c {
    be<u32> mId;
    gptr<u8> mData;
    be<u32> field_0x54;
};
WWHD_SIZE(dDemo_prm_c, 0xC);

// HD: integer vector copies retain signalling NaNs and the observed store order.
struct dDemo_vecWords {
    be<u32> x, y, z;
};
WWHD_SIZE(dDemo_vecWords, 0xC);
inline dDemo_vecWords* demoWords(cXyz* v) { return reinterpret_cast<dDemo_vecWords*>(v); }
inline const dDemo_vecWords* demoWords(const cXyz* v) {
    return reinterpret_cast<const dDemo_vecWords*>(v);
}

struct J3DAnmTexPattern;
struct J3DAnmTextureSRTKey;
struct J3DAnmTevRegKey;

struct dDemo_actor_c {
    u32 JSGFindNodeID(const char* name);
    u32 JSGGetNodeTransformation(u32 index, Mtx34* out);
    fopAc_ac_c* getActor();
    J3DAnmTexPattern* getP_BtpData(const char* archive);
    J3DAnmTextureSRTKey* getP_BtkData(const char* archive);
    J3DAnmTevRegKey* getP_BrkData(const char* archive);
    f64 getPrm_Morf();
    enum PrmId { ID_UNK_1 = 1, ID_UNK_2 = 2, ID_UNK_4 = 4, ID_UNK_5 = 5, ID_UNK_6 = 6 };
    void setActor(fopAc_ac_c* actor);
    void JSGGetRotation(cXyz* out);
    void JSGSetRotation(const cXyz* input);
    // HD: f64 arguments/results retain the verified floating register widths.
    void JSGSetShape(u32 value);
    void JSGSetAnimationFrame(f64 value);
    void JSGSetAnimationTransition(f64 value);
    void JSGSetTextureAnimation(u32 value);
    void JSGSetTextureAnimationFrame(f64 value);
    void JSGSetTranslation(const cXyz* v);
    void JSGSetScaling(const cXyz* v);
    void JSGSetData(u32 a, const void* b, u32 c);
    void JSGSetAnimation(u32 animation);
    f64 JSGGetAnimationFrameMax();
    f64 JSGGetTextureAnimationFrameMax();
    void JSGGetTranslation(cXyz* out);
    void JSGGetScaling(cXyz* out);
    enum Enable_e {
        ENABLE_UNK_e = 1,
        ENABLE_TRANS_e = 2,
        ENABLE_SCALE_e = 4,
        ENABLE_ROTATE_e = 8,
        ENABLE_SHAPE_e = 0x10,
        ENABLE_ANM_e = 0x20,
        ENABLE_ANM_FRAME_e = 0x40,
        ENABLE_ANM_TRANSITION_e = 0x80,
        // HD: texture animation flags are one bit above the tww declarations.
        ENABLE_TEX_ANM = 0x100,
        ENABLE_TEX_ANM_FRAME_e = 0x200
    };
    /* 0x00 */ be<u32> mVtable;
    /* 0x04 */ be<u16> mFlags;
    u8 _06[2];
    /* 0x08 */ cXyz mTrans;
    /* 0x14 */ cXyz mScale;
    /* 0x20 */ csXyz mRotate;
    u8 _26[2];
    /* 0x28 */ be<u32> mShapeId;
    /* 0x2C */ be<u32> mAnmId;
    /* 0x30 */ be<f32> mAnmFrame;
    /* 0x34 */ be<f32> mAnmTransition;
    /* 0x38 */ be<f32> mAnmFrameMax;
    /* 0x3C */ be<u32> mTexAnimation;
    /* 0x40 */ be<f32> mTexAnimationFrame;
    /* 0x44 */ be<f32> mTexAnimationFrameMax;
    /* 0x48 */ gptr<J3DModel> mModel;
    /* 0x4C */ dDemo_prm_c mPrm;
    /* 0x58 */ be<u32> mActorId;
    /* 0x5C */ be<u32> mOldAnmId;
    /* 0x60 */ be<u32> mBtpId;
    /* 0x64 */ be<u32> mBtkId;
    /* 0x68 */ be<u32> mBrkId;
};
WWHD_SIZE(dDemo_actor_c, 0x6C);
WWHD_OFFSET(dDemo_actor_c, mFlags, 4);
WWHD_OFFSET(dDemo_actor_c, mTrans, 8);
WWHD_OFFSET(dDemo_actor_c, mScale, 0x14);
WWHD_OFFSET(dDemo_actor_c, mRotate, 0x20);
WWHD_OFFSET(dDemo_actor_c, mModel, 0x48);
WWHD_OFFSET(dDemo_actor_c, mPrm, 0x4C);
WWHD_OFFSET(dDemo_actor_c, mActorId, 0x58);
WWHD_OFFSET(dDemo_actor_c, mBrkId, 0x68);

struct dDemo_camera_c {
    // HD: f64 arguments/results retain the verified floating register widths.
    f64 JSGGetProjectionNear();
    f64 JSGGetProjectionFar();
    f64 JSGGetProjectionFovy();
    f64 JSGGetViewRoll();
    void JSGGetViewPosition(cXyz* out);
    void JSGGetViewTargetPosition(cXyz* out);
    void JSGSetProjectionNear(f64 value);
    void JSGSetProjectionFar(f64 value);
    void JSGSetProjectionFovy(f64 value);
    void JSGSetViewRoll(f64 value);
    void JSGSetViewPosition(const cXyz* v);
    void JSGSetViewTargetPosition(const cXyz* v);
    enum Enable_e {
        ENABLE_PROJ_NEAR_e = 1,
        ENABLE_PROJ_FAR_e = 2,
        ENABLE_PROJ_FOVY_e = 4,
        ENABLE_PROJ_ASPECT_e = 8,
        ENABLE_VIEW_POS_e = 0x10,
        ENABLE_VIEW_UP_VEC_e = 0x20,
        ENABLE_VIEW_TARG_POS_e = 0x40,
        ENABLE_VIEW_ROLL_e = 0x80
    };
    be<u32> mVtable;
    be<u8> mFlags;
    u8 _05[3];
    be<f32> mProjNear, mProjFar, mFovy, mAspect;
    cXyz mViewPosition, mUpVector, mTargetPosition;
    be<f32> mRoll;
};
WWHD_SIZE(dDemo_camera_c, 0x40);
WWHD_OFFSET(dDemo_camera_c, mViewPosition, 0x18);
WWHD_OFFSET(dDemo_camera_c, mTargetPosition, 0x30);
WWHD_OFFSET(dDemo_camera_c, mRoll, 0x3C);

struct dDemo_ambient_c {
    // HD: f64 arguments/results retain the verified floating register widths.
    void JSGSetColor(const dDemo_colorWord* color);
    enum { ENABLE_COLOR_e = 1 };
    be<u32> mVtable;
    be<u8> mFlags;
    // HD: the colour word is unaligned, immediately after mFlags.
    be<u32> mColor;
    u8 _09[3];
};
WWHD_SIZE(dDemo_ambient_c, 0xC);
WWHD_OFFSET(dDemo_ambient_c, mColor, 5);

struct dDemo_light_c {
    // HD: f64 arguments/results retain the verified floating register widths.
    void JSGSetLightType(u32 value);
    void JSGSetPosition(const cXyz* v);
    void JSGSetDirection(const cXyz* v);
    void JSGSetColor(const dDemo_colorWord* color);
    enum Enable_e {
        ENABLE_LIGHT_TYPE_e = 1,
        ENABLE_POSITION_e = 2,
        ENABLE_COLOR_e = 4,
        ENABLE_DIST_ATTEN_e = 8,
        ENABLE_ANGLE_ATTEN_e = 0x10,
        ENABLE_DIRECTION_e = 0x20
    };
    be<u32> mVtable;
    be<u8> mFlags;
    u8 _05[3];
    be<u32> mLightType, mAttnFn, mSpotFn;
    cXyz mPosition;
    be<u32> mColor;
    be<f32> mDistAttn0, mDistAttn1, mAngleAttn;
    cXyz mDirection;
};
WWHD_SIZE(dDemo_light_c, 0x3C);
WWHD_OFFSET(dDemo_light_c, mPosition, 0x14);
WWHD_OFFSET(dDemo_light_c, mColor, 0x20);
WWHD_OFFSET(dDemo_light_c, mDirection, 0x30);

struct dDemo_fog_c {
    // HD: f64 arguments/results retain the verified floating register widths.
    void JSGSetStartZ(f64 value);
    void JSGSetEndZ(f64 value);
    void JSGSetColor(const dDemo_colorWord* color);
    enum Enable_e {
        ENABLE_FOG_FN_e = 1,
        ENABLE_START_Z_e = 2,
        ENABLE_END_Z_e = 4,
        ENABLE_COLOR_e = 8
    };
    be<u32> mVtable;
    be<u8> mFlags, mFogType;
    u8 _06[2];
    be<f32> mStartZ, mEndZ;
    be<u32> mColor;
};
WWHD_SIZE(dDemo_fog_c, 0x14);
WWHD_OFFSET(dDemo_fog_c, mFogType, 5);
WWHD_OFFSET(dDemo_fog_c, mColor, 0x10);

struct dDemo_object_c {
    // HD: f64 arguments/results retain the verified floating register widths.
    dDemo_actor_c* appendActor(fopAc_ac_c* actor);
    void remove();
    dDemo_light_c* appendLight();
    dDemo_camera_c* getActiveCamera();
    dDemo_camera_c* createCamera();
    dDemo_ambient_c* createAmbient();
    dDemo_fog_c* createFog();
    dDemo_actor_c* getActor(u32 index);
    be<u8> mNumActor, mNumLight;
    u8 _02[2];
    gptr<dDemo_actor_c> mpActors[32];
    gptr<dDemo_camera_c> mpCamera;
    gptr<dDemo_ambient_c> mpAmbient;
    gptr<dDemo_light_c> mpLight[8];
    gptr<dDemo_fog_c> mpFog;
};
WWHD_SIZE(dDemo_object_c, 0xB0);
WWHD_OFFSET(dDemo_object_c, mpActors, 4);
WWHD_OFFSET(dDemo_object_c, mpCamera, 0x84);
WWHD_OFFSET(dDemo_object_c, mpLight, 0x8C);
WWHD_OFFSET(dDemo_object_c, mpFog, 0xAC);

struct dDemo_system_c {
    void* JSGFindObject(const char* name, u32 kind);
    enum ObjectKind {
        OBJECT_ANY = 0,
        OBJECT_ACTOR = 2,
        OBJECT_CAMERA = 3,
        OBJECT_AMBIENT = 4,
        OBJECT_LIGHT = 5,
        OBJECT_FOG = 6
    };
    be<u32> mVtable;
    gptr<dDemo_object_c> mObject;
};
WWHD_SIZE(dDemo_system_c, 8);

struct dDemo_virtualTable {
    gfn _00[3];
    gfn mDelete;
};
WWHD_SIZE(dDemo_virtualTable, 0x10);
WWHD_OFFSET(dDemo_virtualTable, mDelete, 0xC);
struct dDemo_virtualObject {
    gptr<dDemo_virtualTable> mVtable;
};
WWHD_SIZE(dDemo_virtualObject, 4);

struct dDemo_factory {
    be<u32> mVtable;
    be<u32> mCount;
    gptr<void> mNext, mPrev;
    be<u32> mCreateObjectVtable;
};
WWHD_SIZE(dDemo_factory, 0x14);
WWHD_OFFSET(dDemo_factory, mCreateObjectVtable, 0x10);

struct dDemo_createObject {
    be<u32> mPrev, mNext, mVtable;
    gptr<void> mOwner;
    gptr<dDemo_system_c> mSystem;
};
WWHD_SIZE(dDemo_createObject, 0x14);
WWHD_OFFSET(dDemo_createObject, mOwner, 0xC);
WWHD_OFFSET(dDemo_createObject, mSystem, 0x10);

struct dDemo_control {
    u8 _00[8];
    gptr<dDemo_factory> mFactory;
    u8 _0C[0x50 - 0xC];
    be<s32> mSuspend;
    be<u32> mVtable;
    be<f64> mSecondPerFrame;
    gptr<void> mCreateObject;
    u8 _64[0x74 - 0x64];
    be<u8> mTransformOriginEnabled, mTransformEnabled;
};
WWHD_SIZE(dDemo_control, 0x76);
WWHD_OFFSET(dDemo_control, mSuspend, 0x50);
WWHD_OFFSET(dDemo_control, mSecondPerFrame, 0x58);
WWHD_OFFSET(dDemo_control, mTransformOriginEnabled, 0x74);

// HD: the manager's object pool is allocated and stored by pointer.
struct dDemo_manager_c {
    gptr<dDemo_system_c> mSystem;
    gptr<dDemo_control> mControl;
    gptr<dDemo_createObject> mStage, mAudio, mParticle;
    gptr<dDemo_factory> mFactory;
    gptr<dDemo_object_c> mDemoObj;
    gptr<dDemo_createObject> mMessage;
    gptr<const u8> mCurFile;
    be<u32> mFrame, mFrameNoMsg;
    be<s32> mMode;
    be<u32> mFrameAux;
};
WWHD_SIZE(dDemo_manager_c, 0x34);
WWHD_OFFSET(dDemo_manager_c, mDemoObj, 0x18);
WWHD_OFFSET(dDemo_manager_c, mCurFile, 0x20);
WWHD_OFFSET(dDemo_manager_c, mFrame, 0x24);
WWHD_OFFSET(dDemo_manager_c, mFrameNoMsg, 0x28);
WWHD_OFFSET(dDemo_manager_c, mMode, 0x2C);

struct dDemo_staticTransform {
    be<f32> mOriginX, mOriginY;
    be<u8> mOriginEnabled, mRotationEnabled;
    u8 _0A[2];
    be<u32> mOriginNode, mOriginLink, mRotationNode, mRotationLink;
};
WWHD_SIZE(dDemo_staticTransform, 0x1C);
WWHD_OFFSET(dDemo_staticTransform, mOriginNode, 0xC);
WWHD_OFFSET(dDemo_staticTransform, mRotationLink, 0x18);

struct dDemo_doubleConstant {
    be<f64> mValue;
};
WWHD_SIZE(dDemo_doubleConstant, 8);

// HD: nullable GHS constructor adapters preserve the guest entry ABI.
dDemo_prm_c* dDemo_prm_ct(dDemo_prm_c* self);
dDemo_camera_c* dDemo_camera_ct(dDemo_camera_c* self);
dDemo_object_c* dDemo_object_ct(dDemo_object_c* self);
dDemo_ambient_c* dDemo_ambient_ct(dDemo_ambient_c* self);
dDemo_fog_c* dDemo_fog_ct(dDemo_fog_c* self);

dDemo_actor_c* dDemo_actor_ct(dDemo_actor_c* self);
dDemo_light_c* dDemo_light_ct(dDemo_light_c* self);
void dDemo_zeroColor(void* color, u32 size);

// HD: only the verified window and fop view fields are exposed here.
struct dDemo_windowView {
    u8 _00[0x28];
    be<s8> mCameraID;
    u8 _29[3];
};
WWHD_SIZE(dDemo_windowView, 0x2C);
WWHD_OFFSET(dDemo_windowView, mCameraID, 0x28);
struct dDemo_cameraView {
    u8 _00[0xCC];
    be<f32> mNear, mFar, mFovy;
    u8 _D8[4];
    cXyz mEye, mTarget;
    u8 _F4[0xC];
    be<s16> mBank;
};
WWHD_OFFSET(dDemo_cameraView, mNear, 0xCC);
WWHD_OFFSET(dDemo_cameraView, mEye, 0xDC);
WWHD_OFFSET(dDemo_cameraView, mTarget, 0xE8);
WWHD_OFFSET(dDemo_cameraView, mBank, 0x100);
// HD: camera methods pass their receiver through the otherwise unused first register.
dDemo_cameraView* dDemo_getCameraView(dDemo_camera_c* caller);

// HD: the GameCube parameter fields keep their names but contain unsigned bytes.
struct dDemo_prm_data {
    u8 _00[4];
    be<u8> field_0x4;
    u8 _05;
    be<u8> field_0x6, field_0x7;
    u8 _08[3];
    be<u8> field_0xb;
    u8 _0C[3];
    be<u8> field_0xf;
};
WWHD_SIZE(dDemo_prm_data, 0x10);
WWHD_OFFSET(dDemo_prm_data, field_0xf, 0xF);
// HD: GHS deleting-destructor dispatch is the vtable slot at 0xC.
inline void dDemo_deleteObject(void* object, u32 flags) {
    auto* base = static_cast<dDemo_virtualObject*>(object);
    gabi::call_ptr<void>(base->mVtable->mDelete.get(), object, flags);
}

void dDemo_stageCamera_dt(void* self, u32 flags);

void dDemo_stageAmbient_dt(void* self, u32 flags);

void dDemo_stageLight_dt(void* self, u32 flags);

void dDemo_stageFog_dt(void* self, u32 flags);

void dDemo_stageSystem_dt(void* self, u32 flags);

void dDemo_stageActor_dt(void* self, u32 flags);
void dDemo_deleteWithFlags(void* self, u32 flags);

// HD: opaque GHS destructor-chain records; only their identity is passed to the runtime.
struct dDemo_destructorRecord {
    be<u32> mWords[3];
};
WWHD_SIZE(dDemo_destructorRecord, 0xC);
void dDemo_registerGlobalDestructor(dDemo_destructorRecord* record);
void dDemo_initOriginFlag(be<u8>* flag);
void dDemo_initRotationFlag(be<u8>* flag);

// HD: resource IDs in the demo parameter stream are deliberately unaligned.
struct dDemo_prmShortPrimary {
    u8 _00;
    be<s16> mBtpId;
};
struct dDemo_prmShortPair {
    u8 _00[2];
    be<s16> mBtpId, mBtkId;
};
struct dDemo_prmWordPrimary {
    u8 _00;
    be<u32> mBtpId;
};
struct dDemo_prmWordTriple {
    u8 _00[2];
    be<u32> mBtpId, mBtkId, mBrkId;
};
WWHD_OFFSET(dDemo_prmShortPrimary, mBtpId, 1);
WWHD_OFFSET(dDemo_prmShortPair, mBtkId, 4);
WWHD_OFFSET(dDemo_prmWordPrimary, mBtpId, 1);
WWHD_OFFSET(dDemo_prmWordTriple, mBrkId, 10);
struct dDemo_safeStringVtable {
    gfn mMethods[6];
};
struct dDemo_safeString {
    gptr<char> mData;
    gptr<dDemo_safeStringVtable> mVtable;
};
WWHD_SIZE(dDemo_safeString, 8);
void* dDemo_getObjectIDRes(u32 control, dDemo_safeString* archive, u32 id);
struct dDemo_textureVtable {
    gfn _00[5];
    gfn mGetFrameMax;
};
struct dDemo_textureAnimation {
    u8 _00[4];
    gptr<dDemo_textureVtable> mVtable;
};
// HD: the texture animation's vtable is at byte 4; frame maximum returns a full s32.
inline s32 dDemo_textureFrameMax(void* animation) {
    auto* texture = static_cast<dDemo_textureAnimation*>(animation);
    return gabi::call_ptr<s32>(texture->mVtable->mGetFrameMax.get(), animation);
}

// HD: identity of the mutable demo archive-name buffer; content is managed elsewhere.
struct dDemo_archiveName {
    char mName[1];
};

void dDemo_object_dt(dDemo_object_c* self, u32 flags);
void dDemo_destroyAll(dDemo_control* control);
void dDemo_destroyCreateObjects(void* objects);
// HD: SDK creator, message-control and timeline interfaces have different vtable offsets.
struct dDemo_messageControl {
    u8 _00[0x38];
    gptr<dDemo_virtualTable> mVtable;
};
WWHD_OFFSET(dDemo_messageControl, mVtable, 0x38);
inline void dDemo_deleteCreator(dDemo_createObject* object) {
    auto* table = gabi::at<dDemo_virtualTable>(object->mVtable);
    gabi::call_ptr<void>(table->mDelete.get(), object, u32(3));
}
inline void dDemo_deleteControl(dDemo_control* object) {
    auto* table = gabi::at<dDemo_virtualTable>(object->mVtable);
    gabi::call_ptr<void>(table->mDelete.get(), object, u32(3));
}
inline void dDemo_deleteMessageControl(dDemo_messageControl* object) {
    gabi::call_ptr<void>(object->mVtable->mDelete.get(), object, u32(3));
}

// HD: retain the name-based actor factory's full-width parameter ABI.
dDemo_actor_c* dDemo_bindActor(dDemo_actor_c* self, fopAc_ac_c* actor);
fopAc_ac_c* dDemo_searchActorByName(const char* name, u32 first, u32 second);
fopAc_ac_c* dDemo_fastCreateActor(const char* name, u32 first, u32 second, s32 room, u32 fourth,
                                  u32 fifth, u32 sixth, u32 seventh);
inline u8 dDemo_nameByte(const char* name, u32 index) {
    return *gabi::at<be<u8>>(gabi::guest_addr(name) + index);
}

struct dDemo_factoryList {
    be<u32> mCount;
    gptr<void> mNext, mPrev;
};
WWHD_SIZE(dDemo_factoryList, 0xC);
WWHD_OFFSET(dDemo_factoryList, mNext, 4);
dDemo_messageControl* dDemo_messageControl_ct(dDemo_messageControl* self);
void dDemo_setMessageControl(u32 audioControl, dDemo_messageControl* message);
dDemo_control* dDemo_control_ct(dDemo_control* self);
void dDemo_appendCreateObject(dDemo_factory* factory, dDemo_createObject* creator);

struct dDemo_animationSound {
    be<u16> mAnimationId, mSoundId;
};
WWHD_SIZE(dDemo_animationSound, 4);
// HD: these call sites retain double arguments and a full-width reverb register.
void dDemo_setMorphAnimation(mDoExt_McaMorf* morph, J3DAnmTransform* animation, s32 mode,
                             void* sound, f64 transition, f64 speed, f64 start, f64 end);
void dDemo_playMorph(mDoExt_McaMorf* morph, cXyz* position, u32 sound, u32 reverb);

// HD: SDK parsing and cutscene overrides retain their original register widths.
struct dDemo_parse {
    be<u32> mWords[4];
};
struct dDemo_parseInput {
    gptr<u8> mData;
};
WWHD_SIZE(dDemo_parse, 0x10);
WWHD_SIZE(dDemo_parseInput, 4);
struct dDemo_environment {
    u8 _00[0x10A2];
    be<u8> mPaletteOverride;
};
WWHD_OFFSET(dDemo_environment, mPaletteOverride, 0x10A2);
enum {
    dDemo_DEMO_EVENT_e = 0xE20,
    dDemo_DEFAULT_STREAM_e = 0,
    dDemo_EVENT_STREAM_e = 1,
    dDemo_DEFAULT_PALETTE_e = 0xFF
};
void dDemo_resetControlObjects(dDemo_control* control);
void dDemo_parse_ct(dDemo_parse* parser, dDemo_control* control);
u32 dDemo_parseNext(dDemo_parse* parser, dDemo_parseInput* input, u32 flags);
void dDemo_parse_dt(dDemo_parse* parser, u32 flags);
u32 dDemo_forward(dDemo_control* control, u32 frames);
void dDemo_setGetOrigin(dDemo_control* control, const cXyz* position, f64 rotation);
u32 dDemo_setSetOrigin(dDemo_control* control, const cXyz* position, f64 rotation);
void dDemo_startEventWind();
void dDemo_setEventWind(u32 x, u32 y);
void dDemo_setWindPower(f64 power);
void dDemo_chaseWindPower(be<f32>* power, f64 target, f64 step);
// HD: these stream call sites set only r3; the other live registers pass through unchanged.
void dDemo_pauseStream(u32 stream);
void dDemo_startStream(u32 stream);
inline void dDemo_assureSafeString(dDemo_safeString* string) {
    gabi::call_ptr<void>(string->mVtable->mMethods[5].get(), string);
}

void dDemo_assureTermination(void* self);
void dDemo_resetFrames(void* unused);
void dDemo_remove();
WWHD_SIZE(dDemo_cameraView, 0x102);
WWHD_SIZE(dDemo_environment, 0x10A3);
WWHD_SIZE(dDemo_safeStringVtable, 0x18);
WWHD_SIZE(dDemo_textureVtable, 0x18);
WWHD_SIZE(dDemo_textureAnimation, 8);
