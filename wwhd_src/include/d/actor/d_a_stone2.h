// Liftable stones: HD moving-background base is 0x3E0; actor tail is GC +0x118.
#pragma once
#include "bindings.h"
#include "d/d_particle.h"
#include "gc_api/tu.h"
namespace stone2 {
// HD: a private moving-background view avoids the incompatible duplicate dBgW declaration.
struct Background {
    be<u32> m_id;
    u8 pad04[0x68];
    be<u8> mFlags;
};
struct MoveBgView : fopAc_ac_c {
    gptr<Background> mpBgW;
    Mtx34 mBgMtx;
};
WWHD_SIZE(MoveBgView, 0x3E0);
} // namespace stone2
namespace daStone2 {
struct Attr_c {
    gptr<const char> resName;
    be<s16> m04, resIndex;
    be<u32> heapSize;
    be<u8> m0C;
    u8 pad0D;
    be<s16> m0E, m10, m12, m14;
    u8 pad16[2];
    be<u32> m18, m1C;
    be<s16> m20, m22, m24, m26, m28, m2A, m2C, m2E;
    be<f32> m30, m34, m38, m3C, m40, m44, m48, m4C;
    be<s16> m50;
    u8 pad52[2];
    be<f32> m54, m58, m5C;
};
WWHD_SIZE(Attr_c, 0x60);
struct Act_c : stone2::MoveBgView {
    enum Prm_e {
        PRM_ITEMNO_W = 6,
        PRM_ITEMNO_S = 0,
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 8,
        PRM_ITEMSAVE_W = 7,
        PRM_ITEMSAVE_S = 16,
        PRM_TYPE_W = 3,
        PRM_TYPE_S = 24,
        PRM_CULL_W = 3,
        PRM_CULL_S = 28,
        PRM_SHADOW_W = 1,
        PRM_SHADOW_S = 31
    };
    enum { Type_Max = 5 };
    enum Mode { Mode_Wait, Mode_Carry, Mode_Drop, Mode_Fine };
    enum Demo { Demo_Non, Demo_Req, Demo_Run };
    /* 0x3E0 */ request_of_phase_process_class mPhase;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ dBgS_ObjAcch mAcch;
    /* 0x5B0 */ dBgS_AcchCir mAcchCir;
    /* 0x5F0 */ dCcD_Stts mStts;
    /* 0x62C */ dCcD_Cyl mCyl;
    /* 0x75C */ be<s32> m644, m648, m64C;
    /* 0x768 */ be<s16> m650;
    /* 0x76A */ be<u16> mPrmZ;
    /* 0x76C */ be<s16> m654;
    /* 0x76E */ be<u8> m656, m657, m658, m659, m65A;
    u8 pad773;
    /* 0x774 */ dPa_smokeEcallBack m65C;
    /* 0x794 */ cXyz m67C;
    /* 0x7A0 */ dPa_smokeEcallBack m688;
    /* 0x7C0 */ be<u8> m6A8, m6A9;
    u8 pad7C2[2];
    void prmZ_init();
    BOOL is_switch();
    u8 chk_appear();
    void on_switch();
    void mode_wait_init();
    void demo_non_init();
    BOOL damage_bg_proc();
    void eff_lift_smoke_end();
    void eff_lift_smoke_remove();
    void demo_run_init();
    void demo_run();
    void demo_req_init();
    void cam_lockoff();
    void mode_wait();
    BOOL Delete();
    void mode_carry_init();
    void mode_fine_init();
    void mode_drop_init();
    void mode_fine();
    void demo_req();
    BOOL damage_cc_proc();
    BOOL damage_bg_proc_directly();
    void mode_drop();
    void mode_carry();
    BOOL Mthd_Delete();
    BOOL CreateHeap();
    void init_mtx();
    void set_senv(s32 sound, s32 radius);
    BOOL Draw();
    void demo_proc_call();
    void cull_set_draw();
    void set_mtx();
    void cull_set_move();
    BOOL mode_proc_call();
    BOOL Execute(gptr<Mtx34>* output);
    BOOL Mthd_Create();
    BOOL Create();
    void eff_b_break(u16 effect);
    void eff_m_break(u16 effect, u16 frame);
    void eff_break_ebrock();
    void eff_break_ekao();
    void eff_break_ebrock2();
    void damaged(s32 cause);
    void eff_lift_smoke_start();
    BOOL IsDelete();
    void demo_non();
};
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, m644, 0x75C);
WWHD_OFFSET(Act_c, m65C, 0x774);
WWHD_OFFSET(Act_c, m6A9, 0x7C1);
WWHD_SIZE(Act_c, 0x7C4);
} // namespace daStone2
template <class T> struct ProtectedLocal {
    struct Frame {
        u8 linkage[16];
        T value;
    };
    gabi::Local<Frame> storage;
    T* get() const {
        return gabi::at<T>(storage.a + offsetof(Frame, value));
    }
    T* operator->() const {
        return get();
    }
    operator T*() const {
        return get();
    }
};
struct StoneSafeString {
    be<u32> name, vtable;
};
namespace stone2 {
enum : u32 {
    ActorVtable = 0x1003EA2C,
    SttsVtable = 0x1004AE88,
    GSttsVtable = 0x1004AEC0,
    BaseShapeVtable = 0x100015A8,
    ShapeParentVtable = 0x1003E3F8,
    HitInfoVtable = 0x1004B108,
    CylinderVtable = 0x1004B150,
    ShapeVtable = 0x1004B160
};
struct AttrTable {
    daStone2::Attr_c values[5];
};
struct DemoTable {
    gabi::ptmf<void (daStone2::Act_c::*)()> entries[3];
};
struct ModeTable {
    gabi::ptmf<void (daStone2::Act_c::*)()> entries[4];
};
struct EffectTable {
    gabi::ptmf<void (daStone2::Act_c::*)()> entries[5];
};
struct SourceCylinder {
    u8 data[0x44];
};
struct SmokeConstants {
    be<f32> high, dynamicsLarge, particleLarge, dynamicsFace, particleFace, low, dynamicsSmall, piNegative,
        piPositive;
};
struct LightParameters {
    cXyz position, direction;
    GXColor color;
    GXColorS10 materialColor;
    be<f32> parameters[8];
};
WWHD_SIZE(LightParameters, 0x44);
struct CameraBody;
struct Camera {
    u8 pad[0x248];
};
struct Player {
    u8 pad[0x3C0];
    be<u32> mGrabFlags;
};
struct ShapeAttribute {
    u8 pad[0x18];
    be<u32> mParentVtable, mVtable;
};
struct WordVector {
    be<u32> x, y, z;
};
struct Emitter {
    u8 pad00[0x34];
    be<f32> mRate;
    u8 pad38[0x24];
    be<s32> mMaxFrame;
    u8 pad60[0x190];
    Mtx34 mGlobalRotation;
    cXyz mGlobalDynamicsScale, mGlobalTranslation, mGlobalParticleScale;
};
struct ModelEmitter;
struct EmitterList;
struct ParticleView {
    u8 pad00[0x130];
    gptr<EmitterList> mpModelEmitters;
};
inline Camera* cameraAt(u32 address) {
    return gabi::at<Camera>(address);
}
inline CameraBody* cameraBody(Camera* p) {
    return gabi::at<CameraBody>(gabi::ea(p) + 0x248);
}
inline void* massManager(dComIfG_inf_c* p) {
    return gabi::at<void>(gabi::ea(p) + 0x4EF8);
}
inline ShapeAttribute* shape(dCcD_Cyl* p) {
    return reinterpret_cast<ShapeAttribute*>(p->mShapeAttr);
}
inline WordVector* words(cXyz* p) {
    return reinterpret_cast<WordVector*>(p);
}
inline be<f32>* matrixFloats(Mtx34* p) {
    return reinterpret_cast<be<f32>*>(p);
}
inline be<f32>* modelMatrixFloats(J3DModel* p) {
    return gabi::at<be<f32>>(gabi::ea(p) + 0xC8);
}
inline u32 cullMatrixAddress(J3DModel* p) {
    return p ? gabi::ea(p) + offsetof(J3DModel, mBaseTransformMtx) : 0;
}
inline cXyz* modelScale(J3DModel* p) {
    return gabi::at<cXyz>(gabi::ea(p) + 0xBC);
}
inline daStone2::Attr_c* attributeAt(AttrTable* p, u32 i) {
    return gabi::at<daStone2::Attr_c>(gabi::ea(p) + i * 0x60);
}
inline u32 attributeFloatBits(daStone2::Attr_c* p, be<f32> daStone2::Attr_c::* m) {
    return *reinterpret_cast<be<u32>*>(&(p->*m));
}
template <class T> inline const gabi::ptmf<void (daStone2::Act_c::*)()>& procedureAt(T* p, u32 i) {
    return *gabi::at<gabi::ptmf<void (daStone2::Act_c::*)()>>(gabi::ea(p) + i * 8);
}
inline EmitterList* modelEmitterList(dPa_control_c* p) {
    return gabi::at<ParticleView>(gabi::ea(p))->mpModelEmitters;
}
inline LightParameters* lightBlock(dKy_tevstr_c* p, u32 offset) {
    return gabi::at<LightParameters>(gabi::ea(p) + offset);
}
inline BOOL dispatch(daStone2::Act_c* a, u32 slot) {
    return gabi::call<BOOL>(gabi::load<u32>((u32)a->__vtbl + slot), a);
}
} // namespace stone2
u32 stoneIsSwitch(dSv_info_c*, u32, s32);
void stoneOnSwitch(dSv_info_c*, u32, s32);
BOOL stoneEventEnd(dEvent_manager_c*, s32);
u32 stoneEventExists(dEvent_manager_c*, s32);
void stoneOrderEvent(fopAc_ac_c*, s32, u32, u32, u32, u32);
void stoneCameraLockOff(stone2::CameraBody*, u32);
BOOL stoneMoveBGExecute(daStone2::Act_c*);
BOOL stoneMoveBGDelete(daStone2::Act_c*);
void stoneMoveBGConstruct(daStone2::Act_c*);
s32 stoneMoveBGCreate(daStone2::Act_c*, const char*, s32, u32, u32);
void stoneAcchConstruct(dBgS_ObjAcch*);
void stoneAcchCirConstruct(dBgS_AcchCir*);
void stoneSttsConstruct(dCcD_Stts*);
void stoneGSttsConstruct(be<u32>*);
void stoneGObjInfConstruct(dCcD_GObjInf*);
void stoneCylConstruct(cM3dGCyl*);
void stoneSmokeConstruct(dPa_smokeEcallBack*, u32);
void stoneReleaseBackground(dBgS*, stone2::Background*);
void stoneStartSystemSound(u32);
void stoneSetRoom(fopAc_ac_c*, dBgS_GndChk*);
void stonePosMoveStream(fopAc_ac_c*, dCcD_Stts*, const cXyz*, f32, f32);
void stoneResDelete(request_of_phase_process_class*, const char*);
s32 stoneResLoad(request_of_phase_process_class*, const char*);
J3DModelData* stoneObjectResource(dRes_control_c*, StoneSafeString*, s32);
void stoneSoundEnvironment(cXyz*, s32, u32, s32);
void stoneCullSphere(fopAc_ac_c*, f32, f32, f32, f32);
void stoneRotateZXY(Mtx34*, s32, s32, s32);
void stoneTranslateMatrix(f32, f32, f32);
u8 stonePolyColor(dBgS*, cBgS_PolyInfo*);
u32 stoneCullingCheck(fopAc_ac_c*);
void stoneMoveCollision(dCcD_Cyl*, cXyz*);
void stoneRegisterCollision(cCcS*, dCcD_Cyl*);
void stoneSetMass(void*, dCcD_Cyl*, u32);
void stoneDeleteActor(fopAc_ac_c*);
void stoneMovePosition(fopAc_ac_c*, cXyz*);
s16 stoneEventIdx(dEvent_manager_c*, const char*, u32);
stone2::Emitter* stoneSetParticle(dPa_control_c*, u32, u32, const cXyz*, const csXyz*, const cXyz*, u32,
                                  dPa_smokeEcallBack*, s32, const GXColor*, const GXColor*, const cXyz*);
void stoneEmitterMatrix(const Mtx34*, Mtx34*, cXyz*);
stone2::ModelEmitter* stoneCreateModelEmitter(void*, stone2::Emitter*, J3DModelData*, u32, dKy_tevstr_c*,
                                              J3DModelData*, u32, u32);
void stoneAppendModelEmitter(stone2::EmitterList*, stone2::ModelEmitter*);
void stoneTransformVector(Mtx34*, cXyz*, cXyz*);
void stoneShock(dVibration_c*, s32, s32, cXyz*);
s8 stoneReverb(s32);
void stoneTevInit(dKy_tevstr_c*, s32, u32);
void stoneCylDestroy(dCcD_Cyl*, s32);
void stoneSttsDestroy(dCcD_Stts*, s32);
void stoneCirDestroy(cM3dGCir_l*, s32);
void stoneAcchDestroy(dBgS_ObjAcch*, u32);
void stoneActorDestroy(fopAc_ac_c*, u32);

WWHD_OFFSET(stone2::Background, mFlags, 0x6C);
WWHD_OFFSET(stone2::Player, mGrabFlags, 0x3C0);
WWHD_OFFSET(stone2::Emitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(stone2::Emitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(stone2::Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(stone2::Emitter, mGlobalParticleScale, 0x238);
WWHD_SIZE(stone2::Emitter, 0x244);
WWHD_SIZE(StoneSafeString, 8);
WWHD_SIZE(stone2::SmokeConstants, 0x24);
