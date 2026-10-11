#pragma once
#include "wwhd.h"
namespace jst_adaptors {
using namespace gabi;
struct Vec {
    union {
        struct {
            be<f32> x, y, z;
        };
        be<u32> words[3];
    };
};
struct Color {
    union {
        struct {
            be<u8> r, g, b, a;
        };
        be<u32> word;
    };
};
struct Matrix {
    be<f32> v[12];
};
struct TControl {
    u8 pad00[0x74];
    be<u8> mTransformOnSet;
    u8 pad75[0x90 - 0x75];
    be<f32> mTransformOnSet_RotationY;
    u8 pad94[4];
    Matrix mTransformOnSet_Matrix;
};
struct TObject {
    u8 pad00[0x10];
    gptr<TControl> mControl;
};
struct StageVtable {
    u8 pad00[0x64];
    be<u32> JSGGetNodeTransformation;
    u8 pad68[0xC];
    union {
        be<u32> JSGSetTranslation;
        be<u32> JSGSetAmbientColor;
    };
    u8 pad78[0xC];
    union {
        be<u32> JSGSetScaling;
        be<u32> JSGSetPosition;
    };
    u8 pad88[0xC];
    union {
        be<u32> JSGSetRotation;
        be<u32> JSGSetLightColor;
    };
    u8 pad98[0xC];
    be<u32> JSGSetFogColor;
    u8 padA8[0x1C];
    be<u32> JSGSetDirection;
    u8 padC8[0x1C];
    be<u32> JSGSetViewPosition;
    u8 padE8[0x1C];
    be<u32> JSGSetViewTargetPosition;
};
struct StageObject {
    gptr<StageVtable> mVtable;
};
struct TVariableValues {
    u8 pad00[0xC8];
    be<f32> yaw;
    u8 padCC[0x10];
    be<f32> pitch;
};
// HD: this verified adaptor view starts with variable values at +0, not the GC +4.
struct TAdaptor {
    gptr<TVariableValues> mVariableValues;
};
struct JAIBasic;
struct JAISound;
struct JPAEmitterManager;
// HD: partial emitter view: early fields are GC -4; the expanded draw tail is GC +0x48.
struct ParticleList {
    be<u32> mHead, mTail, mLength;
};
enum {
    JPAEmtrStts_StopEmit = 1,
    JPAEmtrStts_StopDraw = 4,
    JPAEmtrStts_EnableDeleteEmitter = 8,
    JPAEmtrStts_Immortal = 0x40
};
struct Emitter {
    u8 pad00[0x5C];
    be<u32> mMaxFrame;
    u8 pad60[0x1AC - 0x60];
    ParticleList mActiveParticles, mChildParticles;
    u8 pad1C4[0x1F0 - 0x1C4];
    Matrix mGlobalRotation;
    Vec mGlobalDynamicsScale;
    Vec mGlobalTranslation;
    Vec mGlobalParticleScale;
    Color mGlobalPrmColor, mGlobalEnvColor;
    u8 pad24C[8];
    be<u32> mFlags;
    u8 pad258[0xA];
    be<u8> mGroupID;
};
struct TAdaptor_sound : TAdaptor {
    u8 pad04[0xE8 - 4];
    gptr<JAIBasic> mpBasic;
    gptr<JAISound> mpSound;
    be<u32> mSoundID;
    gptr<Vec> mPosition;
    Vec _F8;
    void adaptor_do_update(TObject *, u32);
};
struct TAdaptor_particle : TAdaptor {
    struct TJPACallback_ {
        be<u32> mVtable;
        gptr<TAdaptor_particle> mAdaptor;
        gptr<TObject> mObject;
        void execute(Emitter *);
    };
    u8 pad04[0x19C - 4];
    gptr<JPAEmitterManager> mEmitterManager;
    gptr<Emitter> mEmitter;
    TJPACallback_ mCallback;
    // tww retains these unknown names: ID, fade mode, duration, elapsed.
    be<u32> field_0x1B0, field_0x1B4, field_0x1B8, field_0x1BC;
    be<u32> mSystem;
    gptr<StageObject> mObject;
    be<u32> field_0x1C8;
    be<u8> field_0x1CC;
    void adaptor_do_update(TObject *, u32);
};
struct TAdaptor_actor : TAdaptor {
    u8 pad04[0x128 - 4];
    gptr<StageObject> mObject;
    void adaptor_do_update(TObject *, u32);
};
struct TAdaptor_ambientLight : TAdaptor {
    u8 pad04[0x60 - 4];
    gptr<StageObject> mLight;
    void adaptor_do_update(TObject *, u32);
};
struct TAdaptor_camera : TAdaptor {
    u8 pad04[0xEC - 4];
    gptr<StageObject> mObject;
    void adaptor_do_update(TObject *, u32);
};
struct TAdaptor_fog : TAdaptor {
    u8 pad04[0x88 - 4];
    gptr<StageObject> mObject;
    void adaptor_do_update(TObject *, u32);
};
struct TAdaptor_light : TAdaptor {
    u8 pad04[0x114 - 4];
    gptr<StageObject> mObject;
    be<u32> _118;
    void adaptor_do_update(TObject *, u32);
};
struct SoundFadeOutput {
    be<u32> mVtable;
};
struct ParticleFadeOutput {
    be<u32> mVtable;
};
// These free entries are the tww TVVOOn_BEGIN_FADE_IN_/TVVOOn_END_FADE_OUT_ outputs.
struct ActorFrame {
    Vec transformed[2], srts[2], scaling;
    u8 padding[4];
};
struct CameraFrame {
    Vec source[2], transformed[2];
};
struct ColorFrame {
    Color input, output;
};
struct LightFrame {
    Color color;
    Vec source[2];
    Color output;
    Vec target, transformed[2];
    u8 padding[4];
};
struct ParticleFrame {
    u8 pad08[8];
    Color color;
    Vec scaling, srts[2], transformed[2];
    u8 pad50[0x18];
    Matrix local, parentResult;
};
struct ZeroInit {
    be<u32> initialized;
    Vec position;
};
struct Index3 {
    be<u32> values[3];
};
struct Index4 {
    be<u32> values[4];
};
} // namespace jst_adaptors
using namespace jst_adaptors;
void adaptor_getVariableValue_Vec(TAdaptor *, Vec *, const Index3 *);
void adaptor_getVariableValue_GXColor(TAdaptor *, Color *, const Index4 *);
void MTXMultVec(const Matrix *, const Vec *, Vec *);
void MTXMultVecSR(const Matrix *, const Vec *, Vec *);
void VECSubtract(const Vec *, const Vec *, Vec *);
void MTXConcat(const Matrix *, const Matrix *, Matrix *);
void getTransformation_SRxyzT(Matrix *, const Vec *, const Vec *, const Vec *);
void JPAGetXYZRotateMtx(s16, s16, s16, Matrix *);
void JPASetSRTMatrix(const Matrix *, Matrix *, Vec *, Vec *);
void soundStart(JAISound *, u32);
void soundStop(JAISound *, u32);
void startSoundVec(JAIBasic *, u32, gptr<JAISound> *, Vec *, u32, u32, u32);
void forceDeleteEmitter(JPAEmitterManager *, Emitter *, u32);
Emitter *createSimpleEmitterID(JPAEmitterManager *, Vec *, u32, u32, u32,
                               TAdaptor_particle::TJPACallback_ *, u32);
void initializePosition(Vec *, const Vec *, u32);
f64 adaptorCos(f32);
f64 adaptorSin(f32);
// namespace jst_adaptors
