#include "JSystem/JStudio/JST_adaptors.h"
using namespace jst_adaptors;
static const Index3 soundPosition;
static const Index3 actorTranslation;
static const Index3 actorRotation;
static const Index3 actorScaling;
static const Index4 ambientColor;
static const Index3 cameraPosition;
static const Index3 cameraTarget;
static const Index4 fogColor;
static const Index4 lightColor;
static const Index3 lightPosition;
static const Index3 lightTarget;
static const Index3 particleTranslation;
static const Index3 particleRotation;
static const Index3 particleScaling;
static const Index4 particleColor;
static const Index4 particleColor1;
static ZeroInit l_zero;
static const Vec l_initialPosition;
static const f32 l_radians;
// HD: fixed frame views retain the original guest stack offsets and padding.
// Vector copies use words, preserving every float payload.
static void copyVec(Vec *dst, const Vec *src) {
    for (unsigned i = 0; i < 3; ++i)
        dst->words[i] = (u32)src->words[i];
}

/* 0283DD48 */
void TAdaptor_sound::adaptor_do_update(TObject *object, u32 frames) {
    // HD: replay the original prologue and preserve fixed guest frame slots.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    FrameLocal<Vec> result(8);
    TControl *control = object->mControl;
    Vec *position = &this->_F8;
    adaptor_getVariableValue_Vec(this, position, &soundPosition);
    if (control->mTransformOnSet) {
        MTXMultVec(&control->mTransformOnSet_Matrix, position, &*result);
        copyVec(position, &*result);
    }
}

// HD: GHS unsigned conversions: f32 paths subtract at single precision, f64 paths at double
// precision.
static u32 duration32(f32 value) {
    if (value < 2147483648.0f)
        return static_cast<u32>(ftoi(value));
    return static_cast<u32>(ftoi(value - 2147483648.0f)) + 0x80000000u;
}
static u32 duration64(f64 value) {
    if (value < 2147483648.0)
        return static_cast<u32>(ftoi(value));
    return static_cast<u32>(ftoi(value - 2147483648.0)) + 0x80000000u;
}

/* 0283DE84 */
void Sound_beginFade(SoundFadeOutput *self, f32 value, TAdaptor_sound *adaptor) {
    // HD: replay the original prologue and preserve fixed guest frame slots.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 id = adaptor->mSoundID;
    JAISound *sound = adaptor->mpSound;
    if (id & 0xC0000000) {
        if (sound)
            soundStart(sound, duration32(value));
    } else {
        if (sound && (id & 0xC0000C00))
            soundStop(sound, u32(0));
        u32 fade = duration32(value);
        Vec *position = adaptor->mPosition;
        JAIBasic *basic = adaptor->mpBasic;
        startSoundVec(basic, id, &adaptor->mpSound, position, fade, u32(0), u32(4));
    }
}

/* 0283E020 */
void Sound_endFade(SoundFadeOutput *self, f32 value, TAdaptor_sound *adaptor) {
    // HD: replay the original prologue and preserve fixed guest frame slots.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    JAISound *sound = adaptor->mpSound;
    if (sound)
        soundStop(sound, duration32(value));
}

/* 0283EA48 */
void TAdaptor_particle::adaptor_do_update(TObject *object, u32 frames) {
    // HD: replay the original prologue and preserve fixed guest frame slots.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u32 duration = this->field_0x1B8;
    if (!duration)
        return;
    u32 elapsed = this->field_0x1BC;
    if (elapsed >= duration)
        return;
    duration = this->field_0x1B8;
    elapsed += frames;
    this->field_0x1BC = elapsed;
    if (elapsed < duration)
        return;
    if (this->field_0x1B4 != 2)
        this->field_0x1B4 = 0;
    this->field_0x1BC = 0;
    this->field_0x1B8 = 0;
}

/* 0283EB98 */
void Particle_beginFade(ParticleFadeOutput *self, f32 value, TAdaptor_particle *adaptor) {
    // HD: replay the original prologue and preserve fixed guest frame slots.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    FrameLocal<Vec> position(8);
    Emitter *emitter = adaptor->mEmitter;
    u32 id = adaptor->field_0x1B0;
    if (emitter)
        forceDeleteEmitter(adaptor->mEmitterManager, emitter, u32(0));
    if (!l_zero.initialized) {
        l_zero.initialized = 1;
        initializePosition(&l_zero.position, &l_initialPosition, u32(12));
    }
    u32 x = l_zero.position.words[0];
    u32 z = l_zero.position.words[2];
    position->words[0] = x;
    u32 y = l_zero.position.words[1];
    JPAEmitterManager *manager = adaptor->mEmitterManager;
    position->words[2] = z;
    position->words[1] = y;
    emitter = createSimpleEmitterID(manager, &*position, u32(id & 0xFFFF), id >> 24,
                                    (id >> 16) & 0xFF, &adaptor->mCallback, u32(0));
    adaptor->mEmitter = emitter;
    if (emitter) {
        emitter->mFlags = emitter->mFlags | JPAEmtrStts_Immortal;
        adaptor->field_0x1B4 = 1;
        adaptor->field_0x1B8 = duration32(value);
        adaptor->field_0x1BC = 0;
    }
}

/* 0283ECE8 */
void Particle_endFade(ParticleFadeOutput *self, f32 value, TAdaptor_particle *adaptor) {
    // HD: replay the original prologue and preserve fixed guest frame slots.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!adaptor->mEmitter)
        return;
    if (adaptor->field_0x1B4 == 1 && adaptor->field_0x1BC) {
        u32 duration = adaptor->field_0x1B8;
        u32 elapsed = adaptor->field_0x1BC;
        f64 ratio = static_cast<f64>(duration) / static_cast<f64>(elapsed);
        f64 total = static_cast<f64>(value) * ratio;
        adaptor->field_0x1B4 = 2;
        // The second conversion uses the saved ratio and original float input.
        u32 totalFrames = duration64(total);
        f64 remaining = static_cast<f64>(value) * (ratio - 1.0);
        adaptor->field_0x1B8 = totalFrames;
        adaptor->field_0x1BC = duration64(remaining);
    } else {
        adaptor->field_0x1B4 = 2;
        adaptor->field_0x1B8 = duration32(value);
        adaptor->field_0x1BC = 0;
    }
}

/* 0283FF60 */
void TAdaptor_actor::adaptor_do_update(TObject *object, u32 frames) {
    // HD: replay the original prologue and preserve fixed guest frame slots.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    FrameLocal<ActorFrame> storage(8);
    TControl *control = object->mControl;
    adaptor_getVariableValue_Vec(this, &storage->srts[0], &actorTranslation);
    adaptor_getVariableValue_Vec(this, &storage->srts[1], &actorRotation);
    adaptor_getVariableValue_Vec(this, &storage->scaling, &actorScaling);
    Vec *sr = &storage->srts[0];
    if (control->mTransformOnSet) {
        MTXMultVec(&control->mTransformOnSet_Matrix, &storage->srts[0], &storage->transformed[0]);
        storage->transformed[1].words[0] = storage->srts[1].words[0];
        f32 y = storage->srts[1].y;
        f32 yaw = control->mTransformOnSet_RotationY;
        StageObject *actor = this->mObject;
        f32 result = y + yaw;
        storage->transformed[1].words[2] = storage->srts[1].words[2];
        storage->transformed[1].y = result;
        call_ptr<void>(actor->mVtable->JSGSetTranslation, actor, &storage->transformed[0]);
        sr = &storage->transformed[0];
    } else {
        StageObject *actor = this->mObject;
        call_ptr<void>(actor->mVtable->JSGSetTranslation, actor, sr);
    }
    StageObject *actor = this->mObject;
    call_ptr<void>(actor->mVtable->JSGSetRotation, actor, sr + 1);
    actor = this->mObject;
    call_ptr<void>(actor->mVtable->JSGSetScaling, actor, &storage->scaling);
}

/* 02840AC0 */
void TAdaptor_ambientLight::adaptor_do_update(TObject *object, u32 frames) {
    // HD: replay the original prologue and preserve fixed guest frame slots.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    FrameLocal<ColorFrame> color(8);
    adaptor_getVariableValue_GXColor(this, &color->input, &ambientColor);
    StageObject *target = this->mLight;
    u32 fn = target->mVtable->JSGSetAmbientColor;
    color->output.word = color->input.word;
    call_ptr<void>(fn, target, &color->output);
}

/* 02840F24 */
void TAdaptor_camera::adaptor_do_update(TObject *object, u32 frames) {
    // HD: replay the original prologue and preserve fixed guest frame slots.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    FrameLocal<CameraFrame> storage(8);
    TControl *control = object->mControl;
    adaptor_getVariableValue_Vec(this, &storage->source[0], &cameraPosition);
    adaptor_getVariableValue_Vec(this, &storage->source[1], &cameraTarget);
    Vec *vectors = &storage->source[0];
    if (control->mTransformOnSet) {
        MTXMultVec(&control->mTransformOnSet_Matrix, &storage->source[0], &storage->transformed[0]);
        MTXMultVec(&control->mTransformOnSet_Matrix, &storage->source[1], &storage->transformed[1]);
        vectors = &storage->transformed[0];
    }
    StageObject *target = this->mObject;
    call_ptr<void>(target->mVtable->JSGSetViewPosition, target, vectors);
    target = this->mObject;
    call_ptr<void>(target->mVtable->JSGSetViewTargetPosition, target, vectors + 1);
}

/* 02841678 */
void TAdaptor_fog::adaptor_do_update(TObject *object, u32 frames) {
    // HD: replay the original prologue and preserve fixed guest frame slots.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    FrameLocal<ColorFrame> color(8);
    adaptor_getVariableValue_GXColor(this, &color->input, &fogColor);
    StageObject *target = this->mObject;
    u32 fn = target->mVtable->JSGSetFogColor;
    color->output.word = color->input.word;
    call_ptr<void>(fn, target, &color->output);
}

/* 02841D7C */
void TAdaptor_light::adaptor_do_update(TObject *object, u32 frames) {
    // HD: replay the original prologue and preserve fixed guest frame slots.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    FrameLocal<LightFrame> storage(8);
    TControl *control = object->mControl;
    adaptor_getVariableValue_GXColor(this, &storage->color, &lightColor);
    StageObject *target = this->mObject;
    u32 fn = target->mVtable->JSGSetLightColor;
    storage->output.word = storage->color.word;
    call_ptr<void>(fn, target, &storage->output);
    adaptor_getVariableValue_Vec(this, &storage->source[0], &lightPosition);
    u32 mode = this->_118;
    if (mode == 1) {
        TVariableValues *values = this->mVariableValues;
        f32 radians = l_radians;
        f32 pitch = values->pitch * radians;
        f32 yaw = values->yaw;
        f64 cp = adaptorCos(pitch);
        f64 sp = adaptorSin(pitch);
        f32 yawRad = yaw * radians;
        f64 sy = adaptorSin(yawRad);
        storage->source[1].x = static_cast<f32>(cp * sy);
        storage->source[1].y = static_cast<f32>(sp);
        f64 cy = adaptorCos(yawRad);
        storage->source[1].z = static_cast<f32>(cp * cy);
    } else if (mode == 2) {
        adaptor_getVariableValue_Vec(this, &storage->target, &lightTarget);
        VECSubtract(&storage->target, &storage->source[0], &storage->source[1]);
    }
    Vec *vectors = &storage->source[0];
    if (control->mTransformOnSet) {
        MTXMultVec(&control->mTransformOnSet_Matrix, &storage->source[0], &storage->transformed[0]);
        MTXMultVecSR(&control->mTransformOnSet_Matrix, &storage->source[1],
                     &storage->transformed[1]);
        vectors = &storage->transformed[0];
    }
    target = this->mObject;
    call_ptr<void>(target->mVtable->JSGSetPosition, target, vectors);
    target = this->mObject;
    call_ptr<void>(target->mVtable->JSGSetDirection, target, vectors + 1);
}

/* 0283EE58 */
void TAdaptor_particle::TJPACallback_::execute(Emitter *emitter) {
    // HD: replay the original prologue and preserve fixed guest frame slots.
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    FrameLocal<ParticleFrame> storage(8);
    u32 status = emitter->mFlags;
    if ((status & JPAEmtrStts_EnableDeleteEmitter) &&
        emitter->mActiveParticles.mLength + emitter->mChildParticles.mLength == 0) {
        this->mAdaptor->field_0x1B4 = 0;
        this->mAdaptor->field_0x1B8 = 0;
        this->mAdaptor->field_0x1BC = 0;
        TAdaptor_particle *adaptor = this->mAdaptor;
        forceDeleteEmitter(adaptor->mEmitterManager, emitter, u32(0));
        this->mAdaptor->mEmitter = 0;
        return;
    }
    TAdaptor_particle *adaptor = this->mAdaptor;
    u32 duration = adaptor->field_0x1B8;
    u32 mode = adaptor->field_0x1B4;
    f64 alpha = 1.0;
    if (mode == 1 && duration) {
        alpha = static_cast<f64>(adaptor->field_0x1BC) / static_cast<f64>(duration);
    } else if (mode == 2) {
        if (!duration) {
            status |= JPAEmtrStts_StopEmit;
            emitter->mFlags = status;
            emitter->mMaxFrame = 1;
            alpha = 0.0;
        } else {
            u32 elapsed = adaptor->field_0x1BC;
            alpha = static_cast<f64>(duration - elapsed) / static_cast<f64>(duration);
        }
    }
    TObject *object = this->mObject;
    TControl *control = object->mControl;
    emitter->mFlags = status | JPAEmtrStts_StopDraw;
    adaptor_getVariableValue_Vec(this->mAdaptor, &storage->srts[0], &particleTranslation);
    adaptor_getVariableValue_Vec(this->mAdaptor, &storage->srts[1], &particleRotation);
    adaptor_getVariableValue_Vec(this->mAdaptor, &storage->scaling, &particleScaling);
    adaptor = this->mAdaptor;
    if (!adaptor->field_0x1CC) {
        Vec *sr = &storage->srts[0];
        if (control->mTransformOnSet) {
            MTXMultVec(&control->mTransformOnSet_Matrix, &storage->srts[0],
                       &storage->transformed[0]);
            storage->transformed[1].words[0] = storage->srts[1].words[0];
            storage->transformed[1].words[2] = storage->srts[1].words[2];
            f32 yaw = control->mTransformOnSet_RotationY;
            f32 rotation = storage->srts[1].y;
            storage->transformed[1].y = rotation + yaw;
            sr = &storage->transformed[0];
        }
        u8 group = emitter->mGroupID;
        copyVec(&emitter->mGlobalTranslation, sr);
        if (group >= 7)
            emitter->mGlobalTranslation.y = -emitter->mGlobalTranslation.y;
        f64 rx = static_cast<f64>(sr[1].x) / 360.0;
        f64 ry = static_cast<f64>(sr[1].y) / 360.0;
        s16 ax = static_cast<s16>(ftoi(rx * 65536.0));
        s16 ay = static_cast<s16>(ftoi(ry * 65536.0));
        f64 rz = static_cast<f64>(sr[1].z) / 360.0;
        s16 az = static_cast<s16>(ftoi(rz * 65536.0));
        JPAGetXYZRotateMtx(ax, ay, az, &emitter->mGlobalRotation);
        u32 sx = storage->scaling.words[0], sy = storage->scaling.words[1];
        emitter->mGlobalDynamicsScale.words[0] = sx;
        u32 sz = storage->scaling.words[2];
        emitter->mGlobalDynamicsScale.words[1] = sy;
        emitter->mGlobalDynamicsScale.words[2] = sz;
        emitter->mGlobalParticleScale.words[0] = sx;
        emitter->mGlobalParticleScale.words[1] = sy;
        emitter->mGlobalParticleScale.words[2] = sz;
    } else {
        StageObject *parent = adaptor->mObject;
        if (!parent)
            return;
        u32 fn = parent->mVtable->JSGGetNodeTransformation;
        u32 node = adaptor->field_0x1C8;
        if (!call_ptr<u32>(fn, parent, node, &storage->transformed[0]))
            return;
        getTransformation_SRxyzT(&storage->local, &storage->scaling, &storage->srts[1],
                                 &storage->srts[0]);
        MTXConcat(reinterpret_cast<Matrix *>(&storage->transformed[0]), &storage->local,
                  &storage->parentResult);
        JPASetSRTMatrix(&storage->parentResult, &emitter->mGlobalRotation,
                        &emitter->mGlobalDynamicsScale, &emitter->mGlobalTranslation);
    }
    adaptor_getVariableValue_GXColor(this->mAdaptor, &storage->color, &particleColor);
    u8 colorAlpha = storage->color.a;
    u8 green = storage->color.g, blue = storage->color.b, red = storage->color.r;
    alpha *= static_cast<f64>(colorAlpha);
    emitter->mGlobalPrmColor.r = red;
    emitter->mGlobalPrmColor.g = green;
    emitter->mGlobalPrmColor.b = blue;
    u8 outAlpha = 255;
    if (alpha < 255.0)
        outAlpha = static_cast<u8>(ftoi(alpha));
    emitter->mGlobalPrmColor.a = outAlpha;
    adaptor_getVariableValue_GXColor(this->mAdaptor, &storage->color, &particleColor1);
    red = storage->color.r;
    blue = storage->color.b;
    emitter->mGlobalEnvColor.r = red;
    green = storage->color.g;
    status = emitter->mFlags;
    emitter->mGlobalEnvColor.g = green;
    emitter->mGlobalEnvColor.b = blue;
    emitter->mFlags = status & ~u32(JPAEmtrStts_StopDraw);
}

// namespace jst_adaptors

// clang-format off
/* WWHD glue:
data soundPosition 10172F6C
data actorTranslation 10172EB8
data actorRotation 10172EC4
data actorScaling 10172ED0
data ambientColor 10172E68
data cameraPosition 10172EE8
data cameraTarget 10172EF4
data fogColor 10172E78
data lightColor 10172E88
data lightPosition 10172F18
data lightTarget 10172F24
data particleTranslation 10172F30
data particleRotation 10172F3C
data particleScaling 10172F48
data particleColor 10172E98
data particleColor1 10172EA8
data l_zero 101FE03C
data l_initialPosition 1017344C
data l_radians 10173990
ext 028E8F64 void MTXMultVec(const Matrix *, const Vec *, Vec *)
ext 028E9044 void MTXMultVecSR(const Matrix *, const Vec *, Vec *)
ext 028E8DAC void VECSubtract(const Vec *, const Vec *, Vec *)
ext 028E9108 void MTXConcat(const Matrix *, const Matrix *, Matrix *)
ext 028245AC void JPAGetXYZRotateMtx(s16, s16, s16, Matrix *)
ext 02824890 void JPASetSRTMatrix(const Matrix *, Matrix *, Vec *, Vec *)
ext 02839C28 void getTransformation_SRxyzT(Matrix *, const Vec *, const Vec *, const Vec *)
ext 0280B160 void soundStart(JAISound *, u32)
ext 0280B1D8 void soundStop(JAISound *, u32)
ext 02802848 void startSoundVec(JAIBasic *, u32, gptr<JAISound> *, Vec *, u32, u32, u32)
ext 0282215C void forceDeleteEmitter(JPAEmitterManager *, Emitter *, u32)
ext 02821448 Emitter* createSimpleEmitterID(JPAEmitterManager*, Vec*, u32, u32, u32, TAdaptor_particle::TJPACallback_*, u32)
ext C000A848 void initializePosition(Vec*, const Vec*, u32)
ext 028F4BE0 f64 adaptorCos(f32)
ext 028F43F8 f64 adaptorSin(f32)
ext 0283A1E0 void adaptor_getVariableValue_Vec(TAdaptor*, Vec*, const Index3*)
ext 0283A318 void adaptor_getVariableValue_GXColor(TAdaptor*, Color*, const Index4*)
*/
