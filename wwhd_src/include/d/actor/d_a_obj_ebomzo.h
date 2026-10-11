// Bombable bird statue. Private WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

// HD: emitter fields used by the inline color and invalidation methods.
struct EbomzoEmitter {
    u8 mPadding0[0x5C];
    be<s32> mMaxFrame;
    u8 mPadding60[0x244 - 0x60];
    GXColor mGlobalPrmColor;
    u8 mPadding248[0x254 - 0x248];
    be<u32> mFlags;
    void setGlobalPrmColor(u8 r, u8 g, u8 b) {
        mGlobalPrmColor.r = r;
        mGlobalPrmColor.g = g;
        mGlobalPrmColor.b = b;
    }
    void becomeInvalidEmitter() {
        mMaxFrame = -1;
        mFlags = (u32)mFlags | 1;
    }
};
WWHD_OFFSET(EbomzoEmitter, mGlobalPrmColor, 0x244);
WWHD_OFFSET(EbomzoEmitter, mFlags, 0x254);
WWHD_SIZE(EbomzoEmitter, 0x258);
void Ebomzo_sphConstruct(dCcD_Sph*);
void Ebomzo_sphDestruct(dCcD_Sph*, s32);

namespace daObjEbomzo {
class Act_c : public dBgS_MoveBgActor {
public:
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0 };
    Act_c();
    s32 prm_get_swSave();
    BOOL CreateHeap();
    BOOL Create();
    BOOL Delete();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    void check();
    void demo();
    void fall() {}
    BOOL Execute(gptr<Mtx34>* matrix);
    BOOL Draw();
    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ dCcD_Stts mStts;
    /* 0x428 */ dCcD_Sph mCollider;
    /* 0x554 */ be<s32> mMode;
    /* 0x558 */ be<s16> mXRotRate;
    /* 0x55A */ u8 _55A[2];
    /* 0x55C */ gptr<EbomzoEmitter> mpParticleEmitter;
};
WWHD_OFFSET(Act_c, mCollider, 0x428);
WWHD_OFFSET(Act_c, mMode, 0x554);
WWHD_OFFSET(Act_c, mpParticleEmitter, 0x55C);
WWHD_SIZE(Act_c, 0x560);
}
